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

using namespace std;

static bool scripting_on = false;
lua_State* map_lua = NULL;
lua_State* artifacts_lua = NULL;

static void set_lua_globals(lua_State* L) {
    set_scripting_funcs(L);
    set_scripting_consts(L);
}

// Ironfist scripts require modules like "scripts.modules.advmessagebox"
// against the game directory; resolve them under the data root with the
// same case-insensitive matching every other retail path gets, so
// SCRIPTS/MODULES/AdvMessageBox.lua is found on Unix filesystems too.
static i32 IronfistModuleSearcher(lua_State* ls) {
    string relative = luaL_checkstring(ls, 1);
    for (char& c : relative) {
        if (c == '.')
            c = '/';
    }
    string path = ResolveDataPath(relative + ".lua");
    error_code statError;
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
    lua_pushcfunction(ls, IronfistModuleSearcher);
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

static void LoadScript(lua_State** ls, const string& script_filename) {
    *ls = NewScriptState();

    if (luaL_dofile(*ls, script_filename.c_str())) {
        DisplayLuaError(*ls);
    }
}

static void LoadArtifactsScript() {
    string script_file = ResolveDataPath("SCRIPTS/GENERIC/artifacts.lua");
    error_code statError;
    if (std::filesystem::exists(script_file, statError)) {
        LoadScript(&artifacts_lua, script_file);
    }
}

static string GetScriptFileName(const string& mapFileName) {
    return ResolveDataPath("SCRIPTS/" + mapFileName + ".lua");
}

void ScriptingInit(string& map_filnam) {
    ScriptingShutdown();

    string script_file = GetScriptFileName(map_filnam);
    error_code statError;

    if (std::filesystem::exists(script_file, statError)) {
        LoadScript(&map_lua, script_file);
        scripting_on = true;
    }

    LoadArtifactsScript();
}

void ScriptingInitFromString(string& script) {
    ScriptingShutdown();

    map_lua = NewScriptState();
    if (luaL_dostring(map_lua, script.c_str())) {
        DisplayLuaError(map_lua);
    }
    scripting_on = true;

    LoadArtifactsScript();
}

void ScriptingShutdown() {
    if (map_lua != NULL) {
        lua_close(map_lua);
        map_lua = NULL;
        scripting_on = false;
    }

    if (artifacts_lua != NULL) {
        lua_close(artifacts_lua);
        artifacts_lua = NULL;
    }
}

std::string GetScriptContents(std::string mapName) {
    ifstream in(GetScriptFileName(mapName));
    stringstream buffer;
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

bool isTable(MapVarType type) {
    return (type == MAPVAR_TYPE_TABLE);
}

bool isStringNumBool(MapVarType type) {
    return (type == MAPVAR_TYPE_STRING || type == MAPVAR_TYPE_INTEGER
            || type == MAPVAR_TYPE_NUMBER || type == MAPVAR_TYPE_BOOLEAN);
}

MapVarType StringToMapVarType(std::string stringType) {
    if (stringType == "string") {
        return MAPVAR_TYPE_STRING;
    } else if (stringType == "int") {
        return MAPVAR_TYPE_INTEGER;
    } else if (stringType == "number") {
        return MAPVAR_TYPE_NUMBER;
    } else if (stringType == "boolean") {
        return MAPVAR_TYPE_BOOLEAN;
    } else if (stringType == "table") {
        return MAPVAR_TYPE_TABLE;
    } else {
        return MAPVAR_TYPE_ERROR;
    }
}

std::string MapVarTypeToString(MapVarType type) {
    switch (type) {
        case MAPVAR_TYPE_STRING:  return "string";
        case MAPVAR_TYPE_INTEGER: return "int";
        case MAPVAR_TYPE_NUMBER:  return "number";
        case MAPVAR_TYPE_BOOLEAN: return "boolean";
        case MAPVAR_TYPE_TABLE:   return "table";
        default:                  return "error";
    }
}

static MapVarType mapVarTypeForVal(lua_State* L, i32 idx) {
    if (lua_isinteger(L, idx)) {
        return MAPVAR_TYPE_INTEGER;
    } else {
        return StringToMapVarType(lua_typename(map_lua, lua_type(map_lua, idx)));
    }
}

static std::string GetMVValueFromLua(MapVarType& type, i32 idx) {
    if (type == MAPVAR_TYPE_STRING) {
        std::string stringValue(lua_tostring(map_lua, idx));
        return stringValue;
    } else if (type == MAPVAR_TYPE_INTEGER) {
        return std::to_string(lua_tointeger(map_lua, idx));
    } else if (type == MAPVAR_TYPE_NUMBER) {
        return std::to_string(lua_tonumber(map_lua, idx));
    } else if (type == MAPVAR_TYPE_BOOLEAN) {
        return std::to_string(lua_toboolean(map_lua, idx));
    }
    return std::string();
}

static std::string GetMVKeyFromLUA(i32 idx) {
    MapVarType type = mapVarTypeForVal(map_lua, idx);
    if (type != MAPVAR_TYPE_STRING) {
        DisplayError(
            "Warning: Saving tables with non-string keys may not work properly."
            "This includes arrays, which have integer keys.",
            "Script Warning"
        );
    }

    return GetMVValueFromLua(type, idx);
}

static luaTable* GetMVTablesFromLua(std::string& mapVariableId) {
    luaTable* lt = new luaTable;
    lua_pushnil(map_lua);
    while (lua_next(map_lua, -2) != 0) {
        std::string key = GetMVKeyFromLUA(-2);
        mapVariable* mapVar = new mapVariable;
        MapVarType valueType = mapVarTypeForVal(map_lua, -1);
        mapVar->type = valueType;
        if (isStringNumBool(valueType)) {
            std::string* sV = new std::string(GetMVValueFromLua(valueType, -1));
            mapVar->singleValue = sV;
            (*lt)[key] = *mapVar;
            lua_pop(map_lua, 1);
        } else if (isTable(valueType)) {
            mapVar->tableValue = GetMVTablesFromLua(key);
            (*lt)[key] = *mapVar;
        } else {
            ErrorSavingMapVariable(mapVariableId, " Wrong type in the table.");
        }
    }
    lua_pop(map_lua, 1);
    return lt;
}

std::map<std::string, mapVariable> LoadMapVariablesFromLUA() {
    std::map<std::string, mapVariable> mapVariables;

    if (!scripting_on) {
        return mapVariables;
    }

    lua_getglobal(map_lua, "mapVariables");

    if (lua_isnil(map_lua, -1)) return mapVariables;

    lua_pushnil(map_lua);
    while (lua_next(map_lua, -2) != 0) {
        std::string mapVariableId(lua_tostring(map_lua, -1));
        lua_getglobal(map_lua, mapVariableId.c_str());
        MapVarType mapVariableType = mapVarTypeForVal(map_lua, -1);
        mapVariable* mapVar = new mapVariable;
        mapVar->type = mapVariableType;
        if (isTable(mapVariableType)) {
            mapVar->tableValue = GetMVTablesFromLua(mapVariableId);
        } else if (isStringNumBool(mapVariableType)) {
            std::string* sV = new std::string;
            *sV = GetMVValueFromLua(mapVariableType, -1);
            mapVar->singleValue = sV;
            lua_pop(map_lua, 1);
        } else {
            ErrorSavingMapVariable(
                mapVariableId, " A map variable can only be a table, number, string or boolean."
            );
        }
        lua_pop(map_lua, 1);
        mapVariables[mapVariableId] = *mapVar;
    }
    return mapVariables;
}

static void PushStringNumBoolToLUA(MapVarType type, std::string value) {
    if (type == MAPVAR_TYPE_STRING) {
        lua_pushstring(map_lua, value.c_str());
    } else if (type == MAPVAR_TYPE_INTEGER) {
        lua_pushinteger(map_lua, atoi(value.c_str()));
    } else if (type == MAPVAR_TYPE_NUMBER) {
        lua_pushnumber(map_lua, atof(value.c_str()));
    } else if (type == MAPVAR_TYPE_BOOLEAN) {
        lua_pushboolean(map_lua, atoi(value.c_str()));
    }
}

static void PushTableToLUA(luaTable* lt) {
    lua_newtable(map_lua);
    i32 top = lua_gettop(map_lua);
    for (luaTable::const_iterator it = (*lt).begin(); it != (*lt).end(); ++it) {
        if (it->second.type == MAPVAR_TYPE_TABLE) {
            lua_pushstring(map_lua, it->first.c_str());
            PushTableToLUA(it->second.tableValue);
            lua_settable(map_lua, top);
        } else {
            lua_pushstring(map_lua, it->first.c_str());
            PushStringNumBoolToLUA(it->second.type, *it->second.singleValue);
            lua_settable(map_lua, top);
        }
    }
}

void WriteMapVariablesToLUA(std::map<std::string, mapVariable>& mapVariables) {
    if (!scripting_on) {
        DisplayError(
            "Warning: Map variables detected in map without a script.", "Ironfist bug detected"
        );
        return;
    }

    for (std::map<std::string, mapVariable>::const_iterator it = mapVariables.begin();
         it != mapVariables.end(); ++it) {
        if (isTable(it->second.type)) {
            PushTableToLUA(it->second.tableValue);
            lua_setglobal(map_lua, it->first.c_str());
        } else {
            PushStringNumBoolToLUA(it->second.type, *it->second.singleValue);
            lua_setglobal(map_lua, it->first.c_str());
        }
    }
}
