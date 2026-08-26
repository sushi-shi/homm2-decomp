#ifndef HOMM2_IRONFIST_DEEPBINDING_H
#define HOMM2_IRONFIST_DEEPBINDING_H

#include <IRONFIST/lua.h>

#include <SOURCE/army.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>

namespace ironfist::script {

template <typename T>
class Binding {
    T m_value;

public:
    explicit Binding(T value) : m_value(value) {}

    T Get() const { return m_value; }
};

void PushBinding(lua_State* state, Binding<army*> value);
void PushBinding(lua_State* state, Binding<playerData*> value);
void PushBinding(lua_State* state, Binding<hero*> value);
void PushBinding(lua_State* state, Binding<town*> value);
void PushBinding(lua_State* state, Binding<SCampaignChoice*> value);

void* PointerFromLuaClassTable(lua_State* state, i32 stackIndex);

} // namespace ironfist::script

#endif
