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
void PushBinding(lua_State* state, Binding<SCampaignChoice> value);

// Callback arguments may borrow temporaries for the current BindingScope.
// Getter functions use PushBinding and can only return an owned object.
template <typename T>
void PushCallbackBinding(lua_State* state, Binding<T> value) { PushBinding(state, value); }
void PushCallbackBinding(lua_State* state, Binding<army*> value);
void PushCallbackBinding(lua_State* state, Binding<playerData*> value);
void PushCallbackBinding(lua_State* state, Binding<hero*> value);
void PushCallbackBinding(lua_State* state, Binding<town*> value);

// Only the matching native userdata type is accepted. A stale session, battle
// slot, captain, or callback borrow reports a Lua argument error.
template <typename T> T* CheckObject(lua_State* state, i32 argument);
extern template army* CheckObject<army>(lua_State*, i32);
extern template playerData* CheckObject<playerData>(lua_State*, i32);
extern template hero* CheckObject<hero>(lua_State*, i32);
extern template town* CheckObject<town>(lua_State*, i32);

void RegisterBindings(lua_State* state);
void InvalidateObjectHandles();

// A callback can expose a temporary native value (for example an AI's copied
// hero). Such a borrow expires as soon as this scope returns, including errors.
class BindingScope {
public:
    BindingScope();
    ~BindingScope();
    BindingScope(const BindingScope&) = delete;
    BindingScope& operator=(const BindingScope&) = delete;
private:
    u64 m_token;
};

} // namespace ironfist::script

#endif
