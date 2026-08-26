#include <IRONFIST/deepbinding.h>

#include <cstdint>
#include <string>

namespace ironfist::script {

static void PushObject(lua_State* L, void* ptr, const std::string& metatableName) {
    lua_newtable(L);
    lua_pushstring(L, "ptr");
    lua_pushinteger(L, static_cast<lua_Integer>(reinterpret_cast<intptr_t>(ptr)));
    lua_settable(L, -3);
    lua_getglobal(L, metatableName.c_str());
    lua_setmetatable(L, -2);
}

void PushBinding(lua_State* L, Binding<army*> x) {
    PushObject(L, x.Get(), "battleStack_mt");
}

void PushBinding(lua_State* L, Binding<hero*> x) {
    PushObject(L, x.Get(), "hero_mt");
}

void PushBinding(lua_State* L, Binding<playerData*> x) {
    PushObject(L, x.Get(), "player_mt");
}

void PushBinding(lua_State* L, Binding<town*> x) {
    PushObject(L, x.Get(), "town_mt");
}

void PushBinding(lua_State* L, Binding<SCampaignChoice*> x) {
    PushObject(L, x.Get(), "campaignChoice_mt");
}

void* PointerFromLuaClassTable(lua_State* L, i32 stackIndex) {
    lua_pushstring(L, "ptr");
    lua_gettable(L, -1 - stackIndex);
    void* ret = reinterpret_cast<void*>(static_cast<intptr_t>(lua_tonumber(L, -1)));
    lua_pop(L, 1);
    return ret;
}

} // namespace ironfist::script
