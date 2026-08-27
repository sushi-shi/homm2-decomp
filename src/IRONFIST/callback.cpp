#include <IRONFIST/callback.h>

#include <optional>

#include <IRONFIST/dialog.h>
#include <IRONFIST/lua_utils.h>

void ironfist_lua_push(lua_State* ls, i32 arg) {
    lua_pushinteger(ls, arg);
}

void ironfist_lua_push(lua_State* ls, void* arg) {
    lua_pushlightuserdata(ls, arg);
}

void ironfist_lua_push(lua_State* ls, bool arg) {
    lua_pushboolean(ls, arg);
}

void ironfist_lua_push(lua_State* ls, std::string arg) {
    lua_pushstring(ls, arg.c_str());
}

void ironfist_lua_push(lua_State* ls, char* arg) {
    lua_pushstring(ls, arg);
}

void ironfist_lua_push(lua_State* ls, double arg) {
    lua_pushnumber(ls, arg);
}

void ironfist_lua_pushmulti(lua_State*) {
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
