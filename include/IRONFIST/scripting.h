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
    String,
    Integer,
    Number,
    Table,
    Boolean,
    Error
};

struct MapVariable;

using LuaTable = std::map<std::string, MapVariable>;

struct MapVariable {
    MapVariableType type = MapVariableType::Error;
    std::string value;
    LuaTable table;
};

void InitializeMap(const std::string& mapFileName);
void InitializeFromSave(const std::string& script);
void Shutdown();

std::string ScriptContents(const std::string& mapName);

bool IsTable(MapVariableType type);
bool IsScalar(MapVariableType type);

MapVariableType ParseMapVariableType(const std::string& typeName);
std::string MapVariableTypeName(MapVariableType type);

LuaTable LoadMapVariablesFromLua();

void WriteMapVariablesToLua(const LuaTable& mapVariables);

void ErrorLoadingMapVariable(std::string& mapVariableId, const std::string& addErrorMessage);

} // namespace ironfist::script

#endif
