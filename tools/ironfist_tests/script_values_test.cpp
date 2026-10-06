#include <IRONFIST/callback.h>
#include <IRONFIST/scripting.h>

#include <cassert>
#include <cmath>
#include <limits>

using namespace ironfist::script;

static int Nested(lua_State* state) {
    const int top = lua_gettop(state);
    assert(InvokeStateResult<i32>(state, "answer") == 42);
    assert(lua_gettop(state) == top);
    lua_pushinteger(state, 43);
    return 1;
}

int main() {
    InitializeFromSave(R"(
        function nothing() end
        function nilResult() return nil end
        function answer() return 42 end
        function many() return 1, 2, 3 end
        function wrong() return {} end
        function broken() error('expected test error') end
    )");
    lua_State* state = MapState();
    lua_pushstring(state, "caller-owned");
    for (int i = 0; i < 100; ++i) {
        assert(!InvokeStateResult<std::string>(state, "nilResult"));
        assert(!InvokeStateResult<std::string>(state, "nothing"));
        assert(!InvokeStateResult<std::string>(state, "missing"));
        assert(LoadMapVariablesFromLua().empty());
        assert(lua_gettop(state) == 1);
    }
    assert(!InvokeStateResult<i32>(state, "many"));
    assert(!InvokeStateResult<bool>(state, "wrong"));
    assert(!InvokeStateResult<std::string>(state, "wrong"));
    assert(!InvokeStateResult<double>(state, "broken"));
    InvokeState(state, "broken");
    assert(lua_gettop(state) == 1);
    lua_pushcfunction(state, Nested);
    lua_setglobal(state, "nested");
    assert(InvokeStateResult<i32>(state, "nested") == 43);
    assert(lua_gettop(state) == 1);

    // Unsupported values must not disturb lua_next's table/key positions.
    assert(luaL_dostring(state, "mapVariables={'answer','missing',false};") == LUA_OK);
    assert(LoadMapVariablesFromLua().empty());
    assert(lua_gettop(state) == 1);
    assert(luaL_dostring(state, "mapVariables=7") == LUA_OK);
    assert(LoadMapVariablesFromLua().empty());
    assert(lua_gettop(state) == 1);

    assert(luaL_dostring(state, R"(
        big=4294967297; smallest=math.mininteger; largest=math.maxinteger
        tiny=1e-8; precise=math.pi; negativeZero=-0.0; verySmall=1e-308
        nestedValues={big=big, tiny=tiny, inner={precise=precise}}
        mapVariables={'big','smallest','largest','tiny','precise',
                      'negativeZero','verySmall','nestedValues'}
    )") == LUA_OK);
    LuaTable saved;
    for (int i = 0; i < 100; ++i) {
        saved = LoadMapVariablesFromLua();
        assert(saved.size() == 8);
        assert(lua_gettop(state) == 1);
    }
    InitializeFromSave("");
    state = MapState();
    lua_pushinteger(state, 123);
    WriteMapVariablesToLua(saved);
    assert(lua_gettop(state) == 1);
    assert(luaL_dostring(state, R"(
        assert(big==4294967297 and math.type(big)=='integer')
        assert(smallest==math.mininteger and largest==math.maxinteger)
        assert(tiny==1e-8 and precise==math.pi and verySmall==1e-308)
        assert(1/negativeZero==-math.huge)
        assert(nestedValues.big==big and nestedValues.tiny==tiny)
        assert(nestedValues.inner.precise==precise)
    )") == LUA_OK);
    LuaTable invalid;
    invalid["overflow"] = {MapVariableType::Integer, "9223372036854775808", {}};
    invalid["trailing"] = {MapVariableType::Number, "1.5junk", {}};
    WriteMapVariablesToLua(invalid);
    assert(lua_gettop(state) == 1);
    assert(luaL_dostring(state, "assert(overflow==nil and trailing==nil)") == LUA_OK);
    Shutdown();
}
