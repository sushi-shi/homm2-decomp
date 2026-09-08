#ifndef HOMM2_IRONFIST_LUA_UTILS_H
#define HOMM2_IRONFIST_LUA_UTILS_H

#include <string>

#include <Ints.h>
#include <IRONFIST/lua.h>

namespace ironfist::script {

// Preserve the caller's stack, including when a nested callback or an error
// path produces no value, an unexpected type, or multiple values.
class LuaStackScope {
public:
    explicit LuaStackScope(lua_State* state) : m_state(state), m_top(lua_gettop(state)) {}
    ~LuaStackScope() { lua_settop(m_state, m_top); }
    LuaStackScope(const LuaStackScope&) = delete;
    LuaStackScope& operator=(const LuaStackScope&) = delete;

private:
    lua_State* m_state;
    i32 m_top;
};

void SetConstant(lua_State* L, const char* nam, i32 i);
void SetNilConstant(lua_State* L, const char* nam);

void DisplayCustomLuaError(lua_State* L, const char* msg);
void DisplayCustomLuaError(lua_State* L, std::string msg);
void DisplayLuaError(lua_State* L);

i32 GlobalExists(lua_State* L, const char* nam);
i32 CallWithResultCount(lua_State* L, i32 nargs, i32* nres);

} // namespace ironfist::script

#endif
