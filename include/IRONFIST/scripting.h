#ifndef HOMM2_IRONFIST_SCRIPTING_H
#define HOMM2_IRONFIST_SCRIPTING_H

#include <map>
#include <string>

/*
 * These do not exactly correspond with Lua's own types.
 * Lua claims that integers and floating-points are the same type "number",
 * but it actually treats them differently in some places.
 */
enum MapVarType {
    MAPVAR_TYPE_STRING,
    MAPVAR_TYPE_INTEGER,
    MAPVAR_TYPE_NUMBER,
    MAPVAR_TYPE_TABLE,
    MAPVAR_TYPE_BOOLEAN,
    MAPVAR_TYPE_ERROR
};

struct mapVariable;

typedef std::map<std::string, mapVariable> luaTable;

struct mapVariable {
    ~mapVariable() {
        if (type == MAPVAR_TYPE_TABLE) {
            delete tableValue;
        } else {
            delete singleValue;
        }
    }

    MapVarType type;
    union {
        std::string* singleValue; // we treat all non-table values the same
        luaTable* tableValue;
    };
};

void ScriptingInit(std::string& mapFileName);
void ScriptingInitFromString(std::string& script);
void ScriptingShutdown();

std::string GetScriptContents(std::string mapName);

bool isTable(MapVarType type);
bool isStringNumBool(MapVarType type);

MapVarType StringToMapVarType(std::string stringType);
std::string MapVarTypeToString(MapVarType type);

std::map<std::string, mapVariable> LoadMapVariablesFromLUA();

void WriteMapVariablesToLUA(std::map<std::string, mapVariable>& mapVariables);

void ErrorLoadingMapVariable(std::string& mapVariableId, const std::string& addErrorMessage);

#endif
