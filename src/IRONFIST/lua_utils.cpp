#include <IRONFIST/lua_utils.h>

#include <stdio.h>

#include <IRONFIST/dialog.h>

namespace ironfist::script {

void SetConstant(lua_State* L, const char* nam, i32 i) {
    lua_pushinteger(L, i);
    lua_setglobal(L, nam);
}

void SetNilConstant(lua_State* L, const char* nam) {
    lua_pushnil(L);
    lua_setglobal(L, nam);
}

void DisplayCustomLuaError(lua_State* L, const char* msg) {
    fprintf(stderr, "[homm2] script error: %s\n", msg);
    DisplayError(msg, "Script Error");
    lua_pop(L, 1);
}

void DisplayCustomLuaError(lua_State* L, std::string msg) {
    DisplayCustomLuaError(L, msg.c_str());
}

void DisplayLuaError(lua_State* L) {
    const char* msg = luaL_checkstring(L, -1);
    DisplayCustomLuaError(L, msg);
}

i32 GlobalExists(lua_State* L, const char* nam) {
    i32 t = lua_getglobal(L, nam);
    lua_pop(L, 1);
    return t != LUA_TNIL;
}

// Uses the Lua stack size to get the number of returned arguments.
i32 CallWithResultCount(lua_State* L, i32 nargs, i32* nres) {
    const i32 stack_size = lua_gettop(L);

    const i32 res_code = lua_pcall(L, nargs, LUA_MULTRET, 0);

    const i32 stack_size_before_call = stack_size - nargs - 1;
    const i32 num_returns = lua_gettop(L) - stack_size_before_call;
    *nres = num_returns;

    return res_code;
}

} // namespace ironfist::script
