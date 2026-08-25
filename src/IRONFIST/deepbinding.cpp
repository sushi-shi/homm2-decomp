#include <IRONFIST/deepbinding.h>

#include <cstdint>
#include <string>

static void make_deepbound_object(lua_State* L, void* ptr, std::string metatable_name) {
    lua_newtable(L);
    lua_pushstring(L, "ptr");
    lua_pushinteger(L, static_cast<lua_Integer>(reinterpret_cast<intptr_t>(ptr)));
    lua_settable(L, -3);
    lua_getglobal(L, metatable_name.c_str());
    lua_setmetatable(L, -2);
}

void deepbound_push(lua_State* L, deepbind<army*> x) {
    make_deepbound_object(L, x.get(), "battleStack_mt");
}

void deepbound_push(lua_State* L, deepbind<hero*> x) {
    make_deepbound_object(L, x.get(), "hero_mt");
}

void deepbound_push(lua_State* L, deepbind<playerData*> x) {
    make_deepbound_object(L, x.get(), "player_mt");
}

void deepbound_push(lua_State* L, deepbind<town*> x) {
    make_deepbound_object(L, x.get(), "town_mt");
}

void deepbound_push(lua_State* L, deepbind<SCampaignChoice*> x) {
    make_deepbound_object(L, x.get(), "campaignChoice_mt");
}

void* GetPointerFromLuaClassTable(lua_State* L, i32 stackIndex) {
    lua_pushstring(L, "ptr");
    lua_gettable(L, -1 - stackIndex);
    void* ret = reinterpret_cast<void*>(static_cast<intptr_t>(lua_tonumber(L, -1)));
    lua_pop(L, 1);
    return ret;
}
