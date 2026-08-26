#include <IRONFIST/scripting.h>

#include <cstdio>
#include <cstdlib>
#include <strings.h>

#include <filesystem>
#include <system_error>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <utility>

#include <IRONFIST/dialog.h>
#include <IRONFIST/lua.h>
#include <IRONFIST/lua_utils.h>
#include <IRONFIST/paths.h>
#include <IRONFIST/register.h>

#include <PLATFORM/Platform.h>

namespace ironfist::script {

static bool s_scriptingEnabled = false;
static lua_State* s_mapState = NULL;
static lua_State* s_artifactState = NULL;

lua_State* MapState() {
    return s_mapState;
}

lua_State* ArtifactState() {
    return s_artifactState;
}

static void set_lua_globals(lua_State* L) {
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

    set_lua_globals(ls);

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
        LoadScript(&s_artifactState, scriptFile);
    }
}

static std::string GetScriptFileName(const std::string& mapFileName) {
    return ResolveDataPath("SCRIPTS/" + mapFileName + ".lua");
}

void InitializeMap(const std::string& mapFileName) {
    Shutdown();

    std::string scriptFile = GetScriptFileName(mapFileName);
    std::error_code statError;

    if (std::filesystem::exists(scriptFile, statError)) {
        LoadScript(&s_mapState, scriptFile);
        s_scriptingEnabled = true;
    }

    LoadArtifactsScript();
}

void InitializeFromSave(const std::string& script) {
    Shutdown();

    s_mapState = NewScriptState();
    if (luaL_dostring(s_mapState, script.c_str())) {
        DisplayLuaError(s_mapState);
    }
    s_scriptingEnabled = true;

    LoadArtifactsScript();
}

void Shutdown() {
    if (s_mapState != NULL) {
        lua_close(s_mapState);
        s_mapState = NULL;
        s_scriptingEnabled = false;
    }

    if (s_artifactState != NULL) {
        lua_close(s_artifactState);
        s_artifactState = NULL;
    }
}

std::string ScriptContents(const std::string& mapName) {
    std::ifstream in(GetScriptFileName(mapName));
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
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
    return type == MapVariableType::Table;
}

bool IsScalar(MapVariableType type) {
    return type == MapVariableType::String || type == MapVariableType::Integer
        || type == MapVariableType::Number || type == MapVariableType::Boolean;
}

MapVariableType ParseMapVariableType(const std::string& typeName) {
    if (typeName == "string") {
        return MapVariableType::String;
    } else if (typeName == "int") {
        return MapVariableType::Integer;
    } else if (typeName == "number") {
        return MapVariableType::Number;
    } else if (typeName == "boolean") {
        return MapVariableType::Boolean;
    } else if (typeName == "table") {
        return MapVariableType::Table;
    } else {
        return MapVariableType::Error;
    }
}

std::string MapVariableTypeName(MapVariableType type) {
    switch (type) {
        case MapVariableType::String:  return "string";
        case MapVariableType::Integer: return "int";
        case MapVariableType::Number:  return "number";
        case MapVariableType::Boolean: return "boolean";
        case MapVariableType::Table:   return "table";
        default:                       return "error";
    }
}

static MapVariableType MapVariableTypeForValue(lua_State* L, i32 idx) {
    if (lua_isinteger(L, idx)) {
        return MapVariableType::Integer;
    } else {
        return ParseMapVariableType(lua_typename(s_mapState, lua_type(s_mapState, idx)));
    }
}

static std::string GetMapVariableValue(lua_State* state, MapVariableType type, i32 idx) {
    if (type == MapVariableType::String) {
        std::string stringValue(lua_tostring(state, idx));
        return stringValue;
    } else if (type == MapVariableType::Integer) {
        return std::to_string(lua_tointeger(state, idx));
    } else if (type == MapVariableType::Number) {
        return std::to_string(lua_tonumber(state, idx));
    } else if (type == MapVariableType::Boolean) {
        return std::to_string(lua_toboolean(state, idx));
    }
    return std::string();
}

static std::string GetMapVariableKey(i32 idx) {
    MapVariableType type = MapVariableTypeForValue(s_mapState, idx);
    if (type != MapVariableType::String) {
        DisplayError(
            "Warning: Saving tables with non-string keys may not work properly."
            "This includes arrays, which have integer keys.",
            "Script Warning"
        );
    }

    return GetMapVariableValue(s_mapState, type, idx);
}

static LuaTable ReadLuaTable(std::string& mapVariableId) {
    LuaTable table;
    lua_pushnil(s_mapState);
    while (lua_next(s_mapState, -2) != 0) {
        std::string key = GetMapVariableKey(-2);
        MapVariable& mapVariable = table[key];
        mapVariable.type = MapVariableTypeForValue(s_mapState, -1);
        if (IsScalar(mapVariable.type)) {
            mapVariable.value = GetMapVariableValue(s_mapState, mapVariable.type, -1);
            lua_pop(s_mapState, 1);
        } else if (IsTable(mapVariable.type)) {
            mapVariable.table = ReadLuaTable(key);
        } else {
            ErrorSavingMapVariable(mapVariableId, " Wrong type in the table.");
            table.erase(key);
            lua_pop(s_mapState, 1);
        }
    }
    lua_pop(s_mapState, 1);
    return table;
}

LuaTable LoadMapVariablesFromLua() {
    LuaTable mapVariables;

    if (!s_scriptingEnabled) {
        return mapVariables;
    }

    lua_getglobal(s_mapState, "mapVariables");

    if (lua_isnil(s_mapState, -1)) return mapVariables;

    lua_pushnil(s_mapState);
    while (lua_next(s_mapState, -2) != 0) {
        std::string mapVariableId(lua_tostring(s_mapState, -1));
        lua_getglobal(s_mapState, mapVariableId.c_str());
        MapVariable& mapVariable = mapVariables[mapVariableId];
        mapVariable.type = MapVariableTypeForValue(s_mapState, -1);
        if (IsTable(mapVariable.type)) {
            mapVariable.table = ReadLuaTable(mapVariableId);
        } else if (IsScalar(mapVariable.type)) {
            mapVariable.value = GetMapVariableValue(s_mapState, mapVariable.type, -1);
            lua_pop(s_mapState, 1);
        } else {
            ErrorSavingMapVariable(
                mapVariableId, " A map variable can only be a table, number, string or boolean."
            );
            mapVariables.erase(mapVariableId);
        }
        lua_pop(s_mapState, 1);
    }
    return mapVariables;
}

static void PushScalarToLua(MapVariableType type, const std::string& value) {
    if (type == MapVariableType::String) {
        lua_pushstring(s_mapState, value.c_str());
    } else if (type == MapVariableType::Integer) {
        lua_pushinteger(s_mapState, atoi(value.c_str()));
    } else if (type == MapVariableType::Number) {
        lua_pushnumber(s_mapState, atof(value.c_str()));
    } else if (type == MapVariableType::Boolean) {
        lua_pushboolean(s_mapState, atoi(value.c_str()));
    }
}

static void PushTableToLua(const LuaTable& table) {
    lua_newtable(s_mapState);
    i32 top = lua_gettop(s_mapState);
    for (const auto& [key, variable] : table) {
        if (IsTable(variable.type)) {
            lua_pushstring(s_mapState, key.c_str());
            PushTableToLua(variable.table);
            lua_settable(s_mapState, top);
        } else {
            lua_pushstring(s_mapState, key.c_str());
            PushScalarToLua(variable.type, variable.value);
            lua_settable(s_mapState, top);
        }
    }
}

void WriteMapVariablesToLua(const LuaTable& mapVariables) {
    if (!s_scriptingEnabled) {
        DisplayError(
            "Warning: Map variables detected in map without a script.", "Ironfist bug detected"
        );
        return;
    }

    for (const auto& [name, variable] : mapVariables) {
        if (IsTable(variable.type)) {
            PushTableToLua(variable.table);
            lua_setglobal(s_mapState, name.c_str());
        } else {
            PushScalarToLua(variable.type, variable.value);
            lua_setglobal(s_mapState, name.c_str());
        }
    }
}

} // namespace ironfist::script
