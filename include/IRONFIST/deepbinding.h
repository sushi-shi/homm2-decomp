#ifndef HOMM2_IRONFIST_DEEPBINDING_H
#define HOMM2_IRONFIST_DEEPBINDING_H

#include <IRONFIST/lua.h>

#include <SOURCE/army.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>

template <typename T>
class deepbind {
    T _t;

public:
    deepbind(T t) : _t(t) {}

    T get() { return _t; }
};

void deepbound_push(lua_State* L, deepbind<army*> x);
void deepbound_push(lua_State* L, deepbind<playerData*> x);
void deepbound_push(lua_State* L, deepbind<hero*> x);
void deepbound_push(lua_State* L, deepbind<town*> x);
void deepbound_push(lua_State* L, deepbind<SCampaignChoice*> x);

void* GetPointerFromLuaClassTable(lua_State* L, i32 stackIndex);

#endif
