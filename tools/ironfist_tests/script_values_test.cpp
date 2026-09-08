#include <IRONFIST/callback.h>
#include <IRONFIST/scripting.h>
#include <IRONFIST/register.h>
#include <IRONFIST/runtime.h>
#include <IRONFIST/state.h>
#include <SOURCE/game.h>
#include <SOURCE/X_GLOBAL.h>

#include <cassert>
#include <cmath>
#include <limits>
#include <cstdio>
#include <cstring>

using namespace ironfist::script;

static void CheckLua(lua_State* L, const char* source) {
    const int status = luaL_dostring(L, source);
    if (status != LUA_OK)
        std::fprintf(stderr, "%s\n", lua_tostring(L, -1));
    assert(status == LUA_OK);
}

static int NestedBorrow(lua_State* L) {
    hero inner;
    std::strcpy(inner.m_name, "Inner");
    InvokeState(L, "captureInner", Binding<hero*>(&inner));
    return 0;
}

static void CheckObjectBindings() {
    gpGame = new game{};
    gpGame->m_playerCount = 1;
    gpCurPlayer = &gpGame->m_players[0];
    gpCurPlayer->m_currentHero = -1;
    std::strcpy(gpGame->m_heroRecs[0].m_name, "Hero");
    std::strcpy(gpGame->m_castleRecs[0].m_name, "Town");
    gpCombatManager = new combatManager;
    auto& combat = ironfist::state::Get().combat;
    combat.BeginBattle(*gpCombatManager);
    gpCombatManager->m_armyCount[0] = 1;
    gpCombatManager->m_armyCount[1] = 0;
    gpCombatManager->m_heroes[0] = nullptr;
    gpCombatManager->m_heroes[1] = &gpCombatManager->m_captain;
    std::strcpy(gpCombatManager->m_captain.m_name, "Captain");
    auto& stack = gpCombatManager->m_armies[0][0];
    stack.m_monsterType = CREATURE_PEASANT;
    stack.m_quantity = 4;
    combat.ResetStack(stack);

    // Use an independently owned Lua state to retain handles across real
    // session lifecycle transitions, which close the normal script states.
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    RegisterFunctions(L);
    CheckLua(L, R"(
        h=GetHeroInPool(0); p=GetPlayer(0); t=GetTown(0)
        s=BattleGetStack(0,0); captain=BattleGetHero(1)
        assert(type(h)=='userdata' and getmetatable(h)=='ironfist.hero')
        assert(h.ptr==nil and h==GetHeroInPool(0) and h~=p)
        assert(h.name=='Hero' and t.name=='Town' and s.quantity==4)
        assert(captain.name=='Captain' and BattleGetHero(0)==nil)
        assert(GetCurrentHero()==nil and IsObjectValid(h) and not IsObjectValid(nil))
        h.name='Renamed'; h.spellpoints=17; t.name='Renamed town'; s.quantity=6
        assert(GetHeroName(h, 'extra argument')=='Renamed' and GetSpellpoints(h)==17)
        assert(GetTownName(t)=='Renamed town' and GetStackQuantity(s)==6)
        assert(not pcall(SetHeroName, t, 'Wrong type'))
        assert(not pcall(GetHeroName, {ptr=123456}))
        assert(not pcall(GetHeroName, 123456))
        assert(not pcall(GetHeroName, nil))
        assert(not pcall(function() h.ptr=1 end))
        assert(not pcall(BattleGetStack, 0, 1))
        assert(not pcall(BattleGetStack, -1, 0))
        assert(not pcall(BattleGetHero, 2))
        assert(not pcall(GetHeroInPool, 4294967296))
        local forged=setmetatable({ptr=1}, debug.getmetatable(h))
        assert(not IsObjectValid(forged) and not pcall(GetHeroName, forged))
        local stackMeta=debug.getmetatable(s)
        debug.setmetatable(s, debug.getmetatable(h))
        assert(not pcall(GetHeroName, s))
        debug.setmetatable(s, stackMeta)
    )");

    stack.InitClean();
    stack.m_monsterType = CREATURE_PEASANT;
    stack.m_quantity = 7;
    combat.ResetStack(stack);
    CheckLua(L, R"(
        newer=BattleGetStack(0,0)
        assert(not IsObjectValid(s) and newer~=s and newer.quantity==7)
        assert(not pcall(function() s.quantity=99 end) and newer.quantity==7)
    )");
    combat.EndBattle();
    CheckLua(L, R"(
        assert(not IsObjectValid(newer) and not IsObjectValid(captain))
        assert(not pcall(GetHeroName, captain) and not pcall(BattleGetStack,0,0))
    )");
    combat.BeginBattle(*gpCombatManager);
    combat.ResetStack(stack);
    CheckLua(L, R"(
        assert(IsObjectValid(BattleGetStack(0,0)) and not IsObjectValid(newer))
        assert(IsObjectValid(BattleGetHero(1)) and not IsObjectValid(captain))
    )");

    lua_pushcfunction(L, NestedBorrow);
    lua_setglobal(L, "nestedBorrow");
    CheckLua(L, R"(
        function captureInner(value)
            inner=value; assert(inner.name=='Inner' and IsObjectValid(outer))
        end
        function captureOuter(value)
            outer=value; assert(value.name=='Temporary')
            nestedBorrow()
            assert(IsObjectValid(outer) and not IsObjectValid(inner))
            value.spellpoints=23
        end
        function captureError(value)
            failed=value; error('expected borrowed handle error')
        end
    )");
    hero temporary;
    std::strcpy(temporary.m_name, "Temporary");
    InvokeState(L, "captureOuter", Binding<hero*>(&temporary));
    assert(temporary.m_spellPoints == 23);
    CheckLua(L, "assert(not IsObjectValid(outer) and not pcall(GetHeroName,outer))");
    InvokeState(L, "captureError", Binding<hero*>(&temporary));
    CheckLua(L, "assert(not IsObjectValid(failed) and not IsObjectValid(outer))");
    PushBinding(L, Binding<hero*>(&temporary));
    assert(lua_isnil(L, -1)); // Getters cannot accidentally lend an unowned pointer.
    lua_pop(L, 1);

    ironfist::runtime::BeginSessionLoad();
    CheckLua(L, R"(
        assert(not IsObjectValid(h) and not IsObjectValid(p) and not IsObjectValid(t))
        assert(not pcall(GetHeroName,h) and not pcall(BattleGetStack,0,0))
        assert(IsObjectValid(GetHeroInPool(0)))
    )");
    ironfist::runtime::FinishSessionLoad();
    lua_close(L);
    combat.EndBattle();
    delete gpCombatManager;
    gpCombatManager = nullptr;
    delete gpGame;
    gpGame = nullptr;
    gpCurPlayer = nullptr;
    InvalidateObjectHandles();
}

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
    CheckLua(state, "local binding=require('scripts.modules.binding'); assert(binding.isValid==IsObjectValid)");
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
    invalid["overflow"] = {MapVariableType::MAP_VARIABLE_INTEGER, "9223372036854775808", {}};
    invalid["trailing"] = {MapVariableType::MAP_VARIABLE_NUMBER, "1.5junk", {}};
    WriteMapVariablesToLua(invalid);
    assert(lua_gettop(state) == 1);
    assert(luaL_dostring(state, "assert(overflow==nil and trailing==nil)") == LUA_OK);
    Shutdown();
    CheckObjectBindings();
}
