#ifndef HOMM2_IRONFIST_LUA_UTILS_H
#define HOMM2_IRONFIST_LUA_UTILS_H

#include <string>

#include <Ints.h>
#include <IRONFIST/lua.h>

namespace ironfist::script {

void SetConstant(lua_State* L, const char* nam, i32 i);
void SetNilConstant(lua_State* L, const char* nam);

void DisplayCustomLuaError(lua_State* L, const char* msg);
void DisplayCustomLuaError(lua_State* L, std::string msg);
void DisplayLuaError(lua_State* L);

i32 GlobalExists(lua_State* L, const char* nam);
i32 CallWithResultCount(lua_State* L, i32 nargs, i32* nres);

} // namespace ironfist::script

#endif
