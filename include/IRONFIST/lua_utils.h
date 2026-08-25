#ifndef HOMM2_IRONFIST_LUA_UTILS_H
#define HOMM2_IRONFIST_LUA_UTILS_H

#include <string>

#include <Ints.h>
#include <IRONFIST/lua.h>

void lua_setconst(lua_State* L, const char* nam, i32 i);
void lua_setconst_nil(lua_State* L, const char* nam);

void DisplayCustomLuaError(lua_State* L, const char* msg);
void DisplayCustomLuaError(lua_State* L, std::string msg);
void DisplayLuaError(lua_State* L);

i32 LuaGlobalExists(lua_State* L, const char* nam);
i32 lua_pcall_nresult(lua_State* L, i32 nargs, i32* nres);

#endif
