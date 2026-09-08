#include <IRONFIST/scripting.h>

#include <cstdio>
#include <charconv>
#include <cstdlib>
#include <strings.h>

#include <filesystem>
#include <system_error>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <IRONFIST/dialog.h>
#include <IRONFIST/deepbinding.h>
#include <IRONFIST/lua.h>
#include <IRONFIST/lua_utils.h>
#include <IRONFIST/paths.h>
#include <IRONFIST/register.h>

#include <PLATFORM/Platform.h>

namespace ironfist::script {

static bool gScriptingEnabled = false;
static lua_State* gMapState = NULL;
static lua_State* gArtifactState = NULL;
static std::string gMapScript;

lua_State* MapState() {
    return gMapState;
}

lua_State* ArtifactState() {
    return gArtifactState;
}

static void SetLuaGlobals(lua_State* L) {
    RegisterFunctions(L);
    RegisterConstants(L);
}

// Ironfist scripts require modules like "scripts.modules.advmessagebox"
// against the game directory; resolve them under the data root with the
// same case-insensitive matching every other retail path gets, so
// SCRIPTS/MODULES/AdvMessageBox.lua is found on Unix filesystems too.
static i32 ModuleSearcher(lua_State* ls) {
    std::string relative = luaL_checkstring(ls, 1);
    for (char& c : relative) {
        if (c == '.')
            c = '/';
    }
    std::string path = ResolveDataPath(relative + ".lua");
    std::error_code statError;
    if (!std::filesystem::exists(path, statError)) {
        lua_pushfstring(ls, "\n\tno ironfist module '%s'", path.c_str());
        return 1;
    }
    if (luaL_loadfile(ls, path.c_str()) != LUA_OK)
        return lua_error(ls);
    lua_pushstring(ls, path.c_str());
    return 2;
}

static void RegisterModuleSearcher(lua_State* ls) {
    lua_getglobal(ls, "package");
    lua_getfield(ls, -1, "searchers");
    // Slot 2 sits right after package.preload, ahead of the stock
    // package.path searchers.
    for (i32 slot = static_cast<i32>(lua_rawlen(ls, -1)); slot >= 2; slot--) {
        lua_rawgeti(ls, -1, slot);
        lua_rawseti(ls, -2, slot + 1);
    }
    lua_pushcfunction(ls, ModuleSearcher);
    lua_rawseti(ls, -2, 2);
    lua_pop(ls, 2);
}

static lua_State* NewScriptState() {
    lua_State* ls = luaL_newstate();

    luaL_openlibs(ls);
    RegisterModuleSearcher(ls);

    SetLuaGlobals(ls);

    const LuaStackScope stack(ls);
    if (luaL_dofile(ls, ResolveDataPath("SCRIPTS/MODULES/binding.lua").c_str())) {
        DisplayLuaError(ls);
    }
    return ls;
}

static void LoadScript(lua_State** ls, const std::string& scriptFilename) {
    *ls = NewScriptState();

    if (luaL_dofile(*ls, scriptFilename.c_str())) {
        DisplayLuaError(*ls);
    }
}

static void LoadArtifactsScript() {
    std::string scriptFile = ResolveDataPath("SCRIPTS/GENERIC/artifacts.lua");
    std::error_code statError;
    if (std::filesystem::exists(scriptFile, statError)) {
        LoadScript(&gArtifactState, scriptFile);
    }
}

static std::string GetScriptFileName(const std::string& mapFileName) {
    return ResolveDataPath("SCRIPTS/" + mapFileName + ".lua");
}

static std::string ReadScriptContents(const std::string& filename) {
    std::ifstream in(filename);
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

void InitializeMap(const std::string& mapFileName) {
    Shutdown();

    std::string scriptFile = GetScriptFileName(mapFileName);
    std::error_code statError;

    if (std::filesystem::exists(scriptFile, statError)) {
        gMapScript = ReadScriptContents(scriptFile);
        LoadScript(&gMapState, scriptFile);
        gScriptingEnabled = true;
    }

    LoadArtifactsScript();
}

void InitializeFromSave(std::string script) {
    Shutdown();

    gMapScript = std::move(script);
    gMapState = NewScriptState();
    // Match luaL_loadfile's handling of text-file prefixes when restoring
    // source that originally came from an installed map script.
    std::string_view source(gMapScript);
    if (source.starts_with("\xEF\xBB\xBF"))
        source.remove_prefix(3);
    if (source.starts_with('#')) {
        const size_t newline = source.find('\n');
        source.remove_prefix(newline == std::string_view::npos ? source.size() : newline);
    }
    if (luaL_loadbuffer(gMapState, source.data(), source.size(), "saved map") != LUA_OK
        || lua_pcall(gMapState, 0, 0, 0) != LUA_OK) {
        DisplayLuaError(gMapState);
    }
    gScriptingEnabled = true;

    LoadArtifactsScript();
}

void InitializeWithoutMap() {
    Shutdown();
    LoadArtifactsScript();
}

void Shutdown() {
    gMapScript.clear();
    gScriptingEnabled = false;
    // Disable hook entry before finalizers run, and invalidate external states'
    // handles before any game records can be replaced or freed.
    lua_State* map = std::exchange(gMapState, nullptr);
    lua_State* artifacts = std::exchange(gArtifactState, nullptr);
    InvalidateObjectHandles();
    if (map)
        lua_close(map);
    if (artifacts)
        lua_close(artifacts);
}

const std::string& ActiveScriptContents() {
    return gMapScript;
}

/*****************************   Map variables ***********************************************/

static void ErrorMapVariable(
    std::string& mapVariableId,
    const std::string& s2,
    const std::string& addErrorMessage
) {
    const std::string s1("MapVariable '");
    const std::string errorMessage = s1 + mapVariableId + s2 + addErrorMessage;
    const std::string errorLabel("mapVariable Error");
    DisplayError(errorMessage, errorLabel);
}

static void ErrorSavingMapVariable(std::string& mapVariableId, const std::string& addErrorMessage) {
    const std::string s2("' could not be saved properly.");
    ErrorMapVariable(mapVariableId, s2, addErrorMessage);
}

void ErrorLoadingMapVariable(std::string& mapVariableId, const std::string& addErrorMessage) {
    const std::string s2("' could not be loaded properly.");
    ErrorMapVariable(mapVariableId, s2, addErrorMessage);
}

bool IsTable(MapVariableType type) {
    return type == MapVariableType::MAP_VARIABLE_TABLE;
}

bool IsScalar(MapVariableType type) {
    return type == MapVariableType::MAP_VARIABLE_STRING || type == MapVariableType::MAP_VARIABLE_INTEGER
        || type == MapVariableType::MAP_VARIABLE_NUMBER || type == MapVariableType::MAP_VARIABLE_BOOLEAN;
}

MapVariableType ParseMapVariableType(const std::string& typeName) {
    if (typeName == "string") {
        return MapVariableType::MAP_VARIABLE_STRING;
    } else if (typeName == "int") {
        return MapVariableType::MAP_VARIABLE_INTEGER;
    } else if (typeName == "number") {
        return MapVariableType::MAP_VARIABLE_NUMBER;
    } else if (typeName == "boolean") {
        return MapVariableType::MAP_VARIABLE_BOOLEAN;
    } else if (typeName == "table") {
        return MapVariableType::MAP_VARIABLE_TABLE;
    } else {
        return MapVariableType::MAP_VARIABLE_ERROR;
    }
}

std::string MapVariableTypeName(MapVariableType type) {
    switch (type) {
        case MapVariableType::MAP_VARIABLE_STRING:  return "string";
        case MapVariableType::MAP_VARIABLE_INTEGER: return "int";
        case MapVariableType::MAP_VARIABLE_NUMBER:  return "number";
        case MapVariableType::MAP_VARIABLE_BOOLEAN: return "boolean";
        case MapVariableType::MAP_VARIABLE_TABLE:   return "table";
        default:                       return "error";
    }
}

static MapVariableType MapVariableTypeForValue(lua_State* L, i32 idx) {
    if (lua_isinteger(L, idx)) {
        return MapVariableType::MAP_VARIABLE_INTEGER;
    } else {
        return ParseMapVariableType(lua_typename(gMapState, lua_type(gMapState, idx)));
    }
}

static std::string GetMapVariableValue(lua_State* state, MapVariableType type, i32 idx) {
    if (type == MapVariableType::MAP_VARIABLE_STRING) {
        std::string stringValue(lua_tostring(state, idx));
        return stringValue;
    } else if (type == MapVariableType::MAP_VARIABLE_INTEGER) {
        return std::to_string(lua_tointeger(state, idx));
    } else if (type == MapVariableType::MAP_VARIABLE_NUMBER) {
        // The shortest representation that round-trips to the same Lua
        // number, independent of the user's decimal separator.
        char number[128];
        const auto result = std::to_chars(number, number + sizeof(number), lua_tonumber(state, idx));
        return std::string(number, result.ptr);
    } else if (type == MapVariableType::MAP_VARIABLE_BOOLEAN) {
        return std::to_string(lua_toboolean(state, idx));
    }
    return std::string();
}

static std::string GetMapVariableKey(i32 idx) {
    MapVariableType type = MapVariableTypeForValue(gMapState, idx);
    if (type != MapVariableType::MAP_VARIABLE_STRING) {
        DisplayError(
            "Warning: Saving tables with non-string keys may not work properly."
            "This includes arrays, which have integer keys.",
            "Script Warning"
        );
    }

    return GetMapVariableValue(gMapState, type, idx);
}

static LuaTable ReadLuaTable(std::string& mapVariableId) {
    LuaTable table;
    lua_pushnil(gMapState);
    while (lua_next(gMapState, -2) != 0) {
        std::string key = GetMapVariableKey(-2);
        MapVariable& mapVariable = table[key];
        mapVariable.type = MapVariableTypeForValue(gMapState, -1);
        if (IsScalar(mapVariable.type)) {
            mapVariable.value = GetMapVariableValue(gMapState, mapVariable.type, -1);
            lua_pop(gMapState, 1);
        } else if (IsTable(mapVariable.type)) {
            mapVariable.table = ReadLuaTable(key);
        } else {
            ErrorSavingMapVariable(mapVariableId, " Wrong type in the table.");
            table.erase(key);
            lua_pop(gMapState, 1);
        }
    }
    lua_pop(gMapState, 1);
    return table;
}

LuaTable LoadMapVariablesFromLua() {
    LuaTable mapVariables;

    if (!gScriptingEnabled) {
        return mapVariables;
    }

    const LuaStackScope stack(gMapState);
    lua_getglobal(gMapState, "mapVariables");

    if (lua_isnil(gMapState, -1)) return mapVariables;
    if (!lua_istable(gMapState, -1)) {
        DisplayError("mapVariables must be a table of variable names.", "Script error");
        return mapVariables;
    }

    lua_pushnil(gMapState);
    while (lua_next(gMapState, -2) != 0) {
        if (lua_type(gMapState, -1) != LUA_TSTRING) {
            DisplayError("mapVariables entries must be variable names.", "Script error");
            lua_pop(gMapState, 1);
            continue;
        }
        std::string mapVariableId(lua_tostring(gMapState, -1));
        lua_getglobal(gMapState, mapVariableId.c_str());
        MapVariable& mapVariable = mapVariables[mapVariableId];
        mapVariable.type = MapVariableTypeForValue(gMapState, -1);
        if (IsTable(mapVariable.type)) {
            mapVariable.table = ReadLuaTable(mapVariableId);
        } else if (IsScalar(mapVariable.type)) {
            mapVariable.value = GetMapVariableValue(gMapState, mapVariable.type, -1);
            lua_pop(gMapState, 1);
        } else {
            ErrorSavingMapVariable(
                mapVariableId, " A map variable can only be a table, number, string or boolean."
            );
            mapVariables.erase(mapVariableId);
            lua_pop(gMapState, 1);
        }
        lua_pop(gMapState, 1);
    }
    return mapVariables;
}

static void PushScalarToLua(MapVariableType type, const std::string& value) {
    if (type == MapVariableType::MAP_VARIABLE_STRING) {
        lua_pushstring(gMapState, value.c_str());
    } else if (type == MapVariableType::MAP_VARIABLE_INTEGER) {
        lua_Integer number = 0;
        const auto result = std::from_chars(value.data(), value.data() + value.size(), number);
        if (result.ec != std::errc{} || result.ptr != value.data() + value.size()) {
            DisplayError("Invalid saved integer: " + value, "Script error");
            lua_pushnil(gMapState);
        } else {
            lua_pushinteger(gMapState, number);
        }
    } else if (type == MapVariableType::MAP_VARIABLE_NUMBER) {
        lua_Number number = 0;
        const auto result = std::from_chars(value.data(), value.data() + value.size(), number);
        if (result.ec != std::errc{} || result.ptr != value.data() + value.size()) {
            DisplayError("Invalid saved number: " + value, "Script error");
            lua_pushnil(gMapState);
        } else {
            lua_pushnumber(gMapState, number);
        }
    } else if (type == MapVariableType::MAP_VARIABLE_BOOLEAN) {
        lua_pushboolean(gMapState, atoi(value.c_str()));
    } else {
        lua_pushnil(gMapState);
    }
}

static void PushTableToLua(const LuaTable& table) {
    lua_newtable(gMapState);
    i32 top = lua_gettop(gMapState);
    for (const auto& [key, variable] : table) {
        if (IsTable(variable.type)) {
            lua_pushstring(gMapState, key.c_str());
            PushTableToLua(variable.table);
            lua_settable(gMapState, top);
        } else {
            lua_pushstring(gMapState, key.c_str());
            PushScalarToLua(variable.type, variable.value);
            lua_settable(gMapState, top);
        }
    }
}

void WriteMapVariablesToLua(const LuaTable& mapVariables) {
    if (!gScriptingEnabled) {
        DisplayError(
            "Warning: Map variables detected in map without a script.", "Ironfist bug detected"
        );
        return;
    }

    const LuaStackScope stack(gMapState);
    for (const auto& [name, variable] : mapVariables) {
        if (IsTable(variable.type)) {
            PushTableToLua(variable.table);
            lua_setglobal(gMapState, name.c_str());
        } else {
            PushScalarToLua(variable.type, variable.value);
            lua_setglobal(gMapState, name.c_str());
        }
    }
}

} // namespace ironfist::script
