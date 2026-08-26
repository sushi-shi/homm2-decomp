#ifndef HOMM2_IRONFIST_CALLBACK_H
#define HOMM2_IRONFIST_CALLBACK_H

#include <optional>
#include <string>

#include <IRONFIST/lua.h>

#include <IRONFIST/deepbinding.h>
#include <IRONFIST/lua_utils.h>

namespace ironfist::script {

void PushLuaValue(lua_State* state, i32 value);
void PushLuaValue(lua_State* state, bool value);
void PushLuaValue(lua_State* state, void* value);
void PushLuaValue(lua_State* state, char* value);
void PushLuaValue(lua_State* state, const std::string& value);
void PushLuaValue(lua_State* state, double value);

template <typename T>
void PushLuaValue(lua_State* state, Binding<T> value) {
    PushBinding(state, value);
}

void PushLuaValues(lua_State* state);

/*
 * The implementations use Lua functions like lua_isnumber and lua_tonumber.
 * We need template specialization to implement these, see callback.cpp.
 */
template <typename Res>
std::optional<Res> PopLuaResult(lua_State* L, i32 arg);

template <typename T, typename... Args>
void PushLuaValues(lua_State* state, T first, Args... args) {
    PushLuaValue(state, first);
    PushLuaValues(state, args...);
}

lua_State* MapState();
lua_State* ArtifactState();

template <typename... Args>
void InvokeState(lua_State* ls, const char* funcName, Args... args) {
    if (!ls || !GlobalExists(ls, funcName))
        return;
    lua_getglobal(ls, funcName);
    PushLuaValues(ls, args...);
    const i32 size = sizeof...(Args);
    if (lua_pcall(ls, size, 0, 0) != LUA_OK) {
        DisplayLuaError(ls);
    }
}

template <typename... Args>
void Invoke(const char* funcName, Args... args) {
    InvokeState(ArtifactState(), funcName, args...);
    InvokeState(MapState(), funcName, args...);
}

template <typename Res, typename... Args>
std::optional<Res> InvokeStateResult(lua_State* ls, const char* funcName, Args... args) {
    if (!ls) { // if it's not an ironfist map
        return std::optional<Res>();
    }

    if (!GlobalExists(ls, funcName)) {
        return std::optional<Res>();
    }

    lua_getglobal(ls, funcName);
    PushLuaValues(ls, args...);
    const i32 size = sizeof...(Args);

    i32 nresult;
    if (CallWithResultCount(ls, size, &nresult) != LUA_OK) {
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
std::optional<Res> InvokeResult(const char* funcName, Args... args) {
    if (MapState()) {
        auto res = InvokeStateResult<Res>(MapState(), funcName, args...);
        if (res.has_value())
            return res.value();
    }
    // artifacts callback will not be called if overridden by map script
    return InvokeStateResult<Res>(ArtifactState(), funcName, args...);
}

} // namespace ironfist::script

#endif
