#ifndef HOMM2_IRONFIST_CALLBACK_H
#define HOMM2_IRONFIST_CALLBACK_H

#include <optional>
#include <string>

#include <IRONFIST/lua.h>

#include <IRONFIST/deepbinding.h>
#include <IRONFIST/lua_utils.h>

void ironfist_lua_push(lua_State* ls, i32 arg);
void ironfist_lua_push(lua_State* ls, bool arg);
void ironfist_lua_push(lua_State* ls, void* arg);
void ironfist_lua_push(lua_State* ls, char* arg);
void ironfist_lua_push(lua_State* ls, std::string arg);
void ironfist_lua_push(lua_State* ls, double arg);

template <typename T>
void ironfist_lua_push(lua_State* ls, deepbind<T> t) {
    deepbound_push(ls, t);
}

void ironfist_lua_pushmulti(lua_State* ls);

/*
 * The implementations use Lua functions like lua_isnumber and lua_tonumber.
 * We need template specialization to implement these, see callback.cpp.
 */
template <typename Res>
std::optional<Res> PopLuaResult(lua_State* L, i32 arg);

template <typename T, typename... Args>
void ironfist_lua_pushmulti(lua_State* ls, T first, Args... args) {
    ironfist_lua_push(ls, first);
    ironfist_lua_pushmulti(ls, args...);
}

extern lua_State* map_lua;
extern lua_State* artifacts_lua;

template <typename... Args>
void CallbackLuaState(lua_State* ls, const char* funcName, Args... args) {
    if (!ls || !LuaGlobalExists(ls, funcName))
        return;
    lua_getglobal(ls, funcName);
    ironfist_lua_pushmulti(ls, args...);
    const i32 size = sizeof...(Args);
    if (lua_pcall(ls, size, 0, 0) != LUA_OK) {
        DisplayLuaError(ls);
    }
}

template <typename... Args>
void ScriptCallback(const char* funcName, Args... args) {
    CallbackLuaState(artifacts_lua, funcName, args...);
    CallbackLuaState(map_lua, funcName, args...);
}

template <typename Res, typename... Args>
std::optional<Res> CallbackLuaStateResult(lua_State* ls, const char* funcName, Args... args) {
    if (!ls) { // if it's not an ironfist map
        return std::optional<Res>();
    }

    if (!LuaGlobalExists(ls, funcName)) {
        return std::optional<Res>();
    }

    lua_getglobal(ls, funcName);
    ironfist_lua_pushmulti(ls, args...);
    const i32 size = sizeof...(Args);

    i32 nresult;
    if (lua_pcall_nresult(ls, size, &nresult) != LUA_OK) {
        DisplayLuaError(ls);
        return std::optional<Res>();
    }

    if (nresult == 0) {
        return std::optional<Res>();
    } else if (nresult == 1) {
        return PopLuaResult<Res>(ls, -1);
    } else {
        std::string s;
        s += funcName;
        s += " should return at most 1 result, but returned ";
        s += std::to_string(nresult);
        DisplayCustomLuaError(ls, s);
        return std::optional<Res>();
    }
}

template <typename Res, typename... Args>
std::optional<Res> ScriptCallbackResult(const char* funcName, Args... args) {
    if (map_lua) {
        auto res = CallbackLuaStateResult<Res>(map_lua, funcName, args...);
        if (res.has_value())
            return res.value();
    }
    // artifacts callback will not be called if overridden by map script
    return CallbackLuaStateResult<Res>(artifacts_lua, funcName, args...);
}

#endif
