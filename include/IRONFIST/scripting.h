#ifndef HOMM2_IRONFIST_SCRIPTING_H
#define HOMM2_IRONFIST_SCRIPTING_H

#include <map>
#include <string>

namespace ironfist::script {

/*
 * These do not exactly correspond with Lua's own types.
 * Lua claims that integers and floating-points are the same type "number",
 * but it actually treats them differently in some places.
 */
enum class MapVariableType {
    MAP_VARIABLE_STRING,
    MAP_VARIABLE_INTEGER,
    MAP_VARIABLE_NUMBER,
    MAP_VARIABLE_TABLE,
    MAP_VARIABLE_BOOLEAN,
    MAP_VARIABLE_ERROR
};

struct MapVariable;

using LuaTable = std::map<std::string, MapVariable>;

struct MapVariable {
    MapVariableType type = MapVariableType::MAP_VARIABLE_ERROR;
    std::string value;
    LuaTable table;
};

void InitializeMap(const std::string& mapFileName);
void InitializeFromSave(std::string script);
void InitializeWithoutMap();
void Shutdown();

const std::string& ActiveScriptContents();

bool IsTable(MapVariableType type);
bool IsScalar(MapVariableType type);

MapVariableType ParseMapVariableType(const std::string& typeName);
std::string MapVariableTypeName(MapVariableType type);

LuaTable LoadMapVariablesFromLua();

void WriteMapVariablesToLua(const LuaTable& mapVariables);

void ErrorLoadingMapVariable(std::string& mapVariableId, const std::string& addErrorMessage);

} // namespace ironfist::script

#endif
