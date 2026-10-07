#include <IRONFIST/deepbinding.h>

#include <cstdint>
#include <string>

namespace ironfist::script {

static void PushObject(lua_State* state, void* ptr, const std::string& metatableName) {
    lua_newtable(state);
    lua_pushstring(state, "ptr");
    lua_pushinteger(state, static_cast<lua_Integer>(reinterpret_cast<intptr_t>(ptr)));
    lua_settable(state, -3);
    lua_getglobal(state, metatableName.c_str());
    lua_setmetatable(state, -2);
}

void PushBinding(lua_State* state, Binding<army*> value) {
    PushObject(state, value.Get(), "battleStack_mt");
}

void PushBinding(lua_State* state, Binding<hero*> value) {
    PushObject(state, value.Get(), "hero_mt");
}

void PushBinding(lua_State* state, Binding<playerData*> value) {
    PushObject(state, value.Get(), "player_mt");
}

void PushBinding(lua_State* state, Binding<town*> value) {
    PushObject(state, value.Get(), "town_mt");
}

void PushBinding(lua_State* state, Binding<SCampaignChoice> value) {
    // A choice is an authored value. Scripts receive a snapshot, never a
    // mutable pointer into a catalog that can be replaced by loading a file.
    const auto choice = value.Get();
    lua_newtable(state);
    lua_pushinteger(state, H2EnumIndex(choice.type));
    lua_setfield(state, -2, "type");
    lua_pushinteger(state, choice.value);
    lua_setfield(state, -2, "field");
    lua_pushinteger(state, choice.amount);
    lua_setfield(state, -2, "amount");
}

void* PointerFromLuaClassTable(lua_State* state, i32 stackIndex) {
    lua_pushstring(state, "ptr");
    lua_gettable(state, -1 - stackIndex);
    void* ret = reinterpret_cast<void*>(static_cast<intptr_t>(lua_tonumber(state, -1)));
    lua_pop(state, 1);
    return ret;
}

} // namespace ironfist::script
