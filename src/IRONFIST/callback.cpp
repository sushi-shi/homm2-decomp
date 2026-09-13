#include <IRONFIST/callback.h>

#include <optional>

#include <IRONFIST/dialog.h>
#include <IRONFIST/lua_utils.h>

namespace ironfist::script {

void PushLuaValue(lua_State* state, i32 value) {
    lua_pushinteger(state, value);
}

void PushLuaValue(lua_State* state, void* value) {
    lua_pushlightuserdata(state, value);
}

void PushLuaValue(lua_State* state, bool value) {
    lua_pushboolean(state, value);
}

void PushLuaValue(lua_State* state, const std::string& value) {
    lua_pushstring(state, value.c_str());
}

void PushLuaValue(lua_State* state, char* value) {
    lua_pushstring(state, value);
}

void PushLuaValue(lua_State* state, double value) {
    lua_pushnumber(state, value);
}

void PushLuaValues(lua_State*) {
}

template <>
std::optional<bool> PopLuaResult(lua_State* L, i32 arg) {
    if (lua_isboolean(L, arg)) {
        const bool retVal = (lua_toboolean(L, arg) != 0);
        lua_remove(L, arg);
        return retVal;
    } else {
        DisplayError("Incorrect return value: expected bool; got something else", "Script error");
        return {};
    }
}

template <>
std::optional<std::string> PopLuaResult(lua_State* L, i32 arg) {
    const char* luaStr = lua_tostring(L, arg);
    if (luaStr) {
        const std::string ret = luaStr;
        lua_remove(L, arg);
        return ret;
    }

    // Tolerate nil but warn if we got some other type.
    if (!lua_isnil(L, arg)) {
        DisplayError("Incorrect return value: expected string; got something else", "Script error");
    }
    return {};
}

template <>
std::optional<i32> PopLuaResult(lua_State* L, i32 arg) {
    if (lua_isinteger(L, arg)) {
        const i32 retVal = lua_tointeger(L, arg);
        lua_remove(L, arg);
        return retVal;
    } else {
        DisplayError("Incorrect return value: expected int; got something else", "Script error");
        return {};
    }
}

template <>
std::optional<double> PopLuaResult(lua_State* L, i32 arg) {
    if (lua_isnumber(L, arg)) {
        const double retVal = lua_tonumber(L, arg);
        lua_remove(L, arg);
        return retVal;
    } else {
        DisplayError("Incorrect return value: expected double; got something else", "Script error");
        return {};
    }
}

} // namespace ironfist::script
