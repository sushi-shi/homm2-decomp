#include <IRONFIST/deepbinding.h>
#include <IRONFIST/state.h>
#include <SOURCE/game.h>
#include <SOURCE/X_GLOBAL.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <iterator>
#include <type_traits>
#include <vector>

namespace ironfist::script {
namespace {

enum class Kind { OBJECT_HERO, OBJECT_PLAYER, OBJECT_TOWN, OBJECT_STACK };
enum class Lifetime { LIFETIME_SESSION, LIFETIME_BATTLE_STACK, LIFETIME_CAPTAIN, LIFETIME_CALLBACK };
constexpr Kind OBJECT_KINDS[] = {Kind::OBJECT_HERO, Kind::OBJECT_PLAYER, Kind::OBJECT_TOWN, Kind::OBJECT_STACK};

constexpr std::array<const char*, 4> OBJECT_TYPE_NAMES = {
    "ironfist.hero", "ironfist.player", "ironfist.town", "ironfist.battleStack"
};
const char* TypeName(Kind kind) {
    const auto index = static_cast<size_t>(kind);
    return index < OBJECT_TYPE_NAMES.size() ? OBJECT_TYPE_NAMES[index] : "ironfist.invalid";
}

template <typename T> constexpr Kind ObjectKind();
template <> constexpr Kind ObjectKind<hero>() { return Kind::OBJECT_HERO; }
template <> constexpr Kind ObjectKind<playerData>() { return Kind::OBJECT_PLAYER; }
template <> constexpr Kind ObjectKind<town>() { return Kind::OBJECT_TOWN; }
template <> constexpr Kind ObjectKind<army>() { return Kind::OBJECT_STACK; }

// No address ever crosses into Lua. All fields are validated against an owner
// or an active callback frame before obtaining a native pointer.
struct Handle {
    Kind kind;
    Lifetime lifetime;
    u64 session;
    u64 generation;
    i32 side;
    i32 slot;
};
static_assert(std::is_trivially_copyable_v<Handle>);

struct Borrow { Kind kind; void* object; };
struct CallbackFrame { u64 token; std::vector<Borrow> objects; };
u64 gSessionGeneration = 1;
game* gSessionOwner = nullptr;
u64 gNextCallback = 0;
std::vector<CallbackFrame> gCallbackFrames;

void RefreshSessionOwner() {
    if (gSessionOwner != gpGame)
        InvalidateObjectHandles();
}

template <typename T, size_t N>
i32 SlotOf(T* object, T (&records)[N]) {
    for (size_t slot = 0; slot < N; ++slot)
        if (object == &records[slot])
            return static_cast<i32>(slot);
    return -1;
}

template <typename T, size_t N>
T* RecordAt(i32 slot, T (&records)[N]) {
    return slot >= 0 && static_cast<size_t>(slot) < N ? &records[slot] : nullptr;
}

void* Resolve(const Handle& handle) {
    RefreshSessionOwner();
    if (handle.session != gSessionGeneration)
        return nullptr;
    auto& combat = state::Get().combat;
    switch (handle.lifetime) {
        case Lifetime::LIFETIME_SESSION:
            if (!gSessionOwner)
                return nullptr;
            switch (handle.kind) {
                case Kind::OBJECT_HERO: return RecordAt(handle.slot, gSessionOwner->m_heroRecs);
                case Kind::OBJECT_PLAYER: return RecordAt(handle.slot, gSessionOwner->m_players);
                case Kind::OBJECT_TOWN: return RecordAt(handle.slot, gSessionOwner->m_castleRecs);
                case Kind::OBJECT_STACK: return nullptr;
            }
            return nullptr;
        case Lifetime::LIFETIME_BATTLE_STACK:
            return handle.kind == Kind::OBJECT_STACK && combat.IsActiveFor(gpCombatManager)
                ? combat.Resolve({handle.side, handle.slot, handle.generation}) : nullptr;
        case Lifetime::LIFETIME_CAPTAIN:
            return handle.kind == Kind::OBJECT_HERO && combat.IsActiveFor(gpCombatManager)
                ? combat.Captain(handle.generation) : nullptr;
        case Lifetime::LIFETIME_CALLBACK:
            for (const auto& frame : gCallbackFrames)
                if (frame.token == handle.generation && handle.slot >= 0
                    && static_cast<size_t>(handle.slot) < frame.objects.size()) {
                    const auto& borrow = frame.objects[handle.slot];
                    return borrow.kind == handle.kind ? borrow.object : nullptr;
                }
            return nullptr;
    }
    return nullptr;
}

const Handle* TestHandle(lua_State* L, i32 argument, Kind kind) {
    if (lua_type(L, argument) != LUA_TUSERDATA || lua_rawlen(L, argument) != sizeof(Handle))
        return nullptr;
    const auto* handle = static_cast<const Handle*>(luaL_testudata(L, argument, TypeName(kind)));
    return handle && handle->kind == kind ? handle : nullptr;
}

const Handle* TestHandle(lua_State* L, i32 argument) {
    for (const auto kind : OBJECT_KINDS)
        if (const auto* handle = TestHandle(L, argument, kind))
            return handle;
    return nullptr;
}

void* Checked(lua_State* L, i32 argument, Kind kind) {
    const auto* handle = TestHandle(L, argument, kind);
    if (!handle) {
        luaL_argerror(L, argument, TypeName(kind));
        return nullptr;
    }
    void* object = Resolve(*handle);
    if (!object)
        luaL_argerror(L, argument, "object handle has expired");
    return object;
}

void PushHandle(lua_State* L, const Handle& handle) {
    void* storage = lua_newuserdata(L, sizeof(Handle));
    std::memcpy(storage, &handle, sizeof(handle));
    luaL_setmetatable(L, TypeName(handle.kind));
}

void PushBorrow(lua_State* L, Kind kind, void* object) {
    if (gCallbackFrames.empty()) {
        // Only a scoped callback may lend an object outside the known owners.
        lua_pushnil(L);
        return;
    }
    auto& frame = gCallbackFrames.back();
    for (size_t slot = 0; slot < frame.objects.size(); ++slot)
        if (frame.objects[slot].kind == kind && frame.objects[slot].object == object) {
            PushHandle(L, {kind, Lifetime::LIFETIME_CALLBACK, gSessionGeneration, frame.token, -1,
                           static_cast<i32>(slot)});
            return;
        }
    frame.objects.push_back({kind, object});
    PushHandle(L, {kind, Lifetime::LIFETIME_CALLBACK, gSessionGeneration, frame.token, -1,
                   static_cast<i32>(frame.objects.size() - 1)});
}

template <typename T>
void PushObject(lua_State* L, T* object, bool allowBorrow = false) {
    RefreshSessionOwner();
    if (!object) {
        lua_pushnil(L);
        return;
    }
    constexpr auto kind = ObjectKind<T>();
    i32 slot = -1;
    if (gSessionOwner) {
        if constexpr (std::is_same_v<T, hero>)
            slot = SlotOf(object, gSessionOwner->m_heroRecs);
        if constexpr (std::is_same_v<T, playerData>)
            slot = SlotOf(object, gSessionOwner->m_players);
        if constexpr (std::is_same_v<T, town>)
            slot = SlotOf(object, gSessionOwner->m_castleRecs);
    }
    if (slot >= 0) {
        PushHandle(L, {kind, Lifetime::LIFETIME_SESSION, gSessionGeneration, 0, -1, slot});
        return;
    }
    auto& combat = state::Get().combat;
    if constexpr (std::is_same_v<T, army>) {
        // An unregistered/reused battle slot must not become a callback borrow.
        const auto identity = combat.Identity(*object);
        if (combat.IsActiveFor(gpCombatManager) && identity.generation)
            PushHandle(L, {kind, Lifetime::LIFETIME_BATTLE_STACK, gSessionGeneration, identity.generation,
                           identity.side, identity.slot});
        else
            lua_pushnil(L);
        return;
    }
    if constexpr (std::is_same_v<T, hero>) {
        if (combat.IsActiveFor(gpCombatManager)
            && combat.Captain(combat.BattleGeneration()) == object) {
            PushHandle(L, {kind, Lifetime::LIFETIME_CAPTAIN, gSessionGeneration, combat.BattleGeneration(), -1, 0});
            return;
        }
        if (gpCombatManager && object == &gpCombatManager->m_captain) {
            lua_pushnil(L);
            return;
        }
    }
    if (allowBorrow)
        PushBorrow(L, kind, object);
    else
        lua_pushnil(L);
}

struct Property { const char* name; const char* getter; const char* setter; };
constexpr Property HERO_PROPERTIES[] = {
    {"name", "GetHeroName", "SetHeroName"}, {"owner", "GetHeroOwner", nullptr},
    {"spellpoints", "GetSpellpoints", "SetSpellpoints"}, {"level", "GetHeroLevel", nullptr},
    {"tempMoraleBonuses", "GetHeroTempMoraleBonuses", "SetHeroTempMoraleBonuses"},
    {"tempLuckBonuses", "GetHeroTempLuckBonuses", "SetHeroTempLuckBonuses"},
    {"mobility", "GetHeroMobility", "SetHeroMobility"},
    {"remainingMobility", "GetHeroRemainingMobility", "SetHeroRemainingMobility"},
    {"x", "GetHeroX", nullptr}, {"y", "GetHeroY", nullptr}
};
constexpr Property PLAYER_PROPERTIES[] = {
    {"color", "GetPlayerColor", nullptr}, {"numHeroes", "GetNumHeroes", nullptr},
    {"daysLeftWithoutCastle", "GetDaysAfterTownLost", "SetDaysAfterTownLost"}
};
constexpr Property TOWN_PROPERTIES[] = {
    {"name", "GetTownName", "SetTownName"}, {"owner", "GetTownOwner", "SetTownOwner"},
    {"faction", "GetTownFaction", "SetTownFaction"},
    {"x", "GetTownX", nullptr}, {"y", "GetTownY", nullptr},
    {"visitingHero", "GetVisitingHero", nullptr}
};
constexpr Property STACK_PROPERTIES[] = {
    {"side", "GetStackSide", nullptr}, {"type", "GetStackType", nullptr},
    {"creatureType", "GetStackType", nullptr},
    {"quantity", "GetStackQuantity", "SetStackQuantity"},
    {"initialQuantity", "GetStackInitialQuantity", "SetStackInitialQuantity"},
    {"hex", "GetStackHex", nullptr}, {"morale", "GetStackMorale", "SetStackMorale"},
    {"luck", "GetStackLuck", "SetStackLuck"}, {"attack", "GetStackAttack", "SetStackAttack"},
    {"defense", "GetStackDefense", "SetStackDefense"}, {"speed", "GetStackSpeed", "SetStackSpeed"},
    {"shots", "GetStackShots", "SetStackShots"}, {"hp", "GetStackHp", "SetStackHp"}
};

Kind PropertyKind(lua_State* L) {
    const lua_Integer index = lua_tointeger(L, lua_upvalueindex(1));
    if (index < 0 || static_cast<size_t>(index) >= std::size(OBJECT_KINDS)) {
        luaL_error(L, "invalid object property binding");
        return Kind::OBJECT_HERO;
    }
    return OBJECT_KINDS[index];
}

i32 GetProperty(lua_State* L) {
    Checked(L, 1, PropertyKind(L));
    lua_pushvalue(L, 2);
    lua_rawget(L, lua_upvalueindex(2));
    if (lua_isnil(L, -1))
        return 1;
    lua_pushvalue(L, 1);
    lua_call(L, 1, 1);
    return 1;
}

i32 SetProperty(lua_State* L) {
    Checked(L, 1, PropertyKind(L));
    lua_pushvalue(L, 2);
    lua_rawget(L, lua_upvalueindex(2));
    if (lua_isnil(L, -1))
        return luaL_error(L, "object property is unknown or read-only");
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 3);
    lua_call(L, 2, 0);
    return 0;
}

i32 Equal(lua_State* L) {
    const auto* left = TestHandle(L, 1);
    const auto* right = TestHandle(L, 2);
    lua_pushboolean(L, left && right && left->kind == right->kind && left->lifetime == right->lifetime
        && left->session == right->session && left->generation == right->generation
        && left->side == right->side && left->slot == right->slot);
    return 1;
}

i32 Describe(lua_State* L) {
    const auto* handle = TestHandle(L, 1);
    if (!handle)
        return luaL_argerror(L, 1, "Ironfist object expected");
    if (Resolve(*handle))
        lua_pushfstring(L, "%s(%d)", TypeName(handle->kind), handle->slot);
    else
        lua_pushfstring(L, "%s(expired)", TypeName(handle->kind));
    return 1;
}

i32 IsValid(lua_State* L) {
    const auto* handle = TestHandle(L, 1);
    lua_pushboolean(L, handle && Resolve(*handle));
    return 1;
}

template <size_t N>
void RegisterType(lua_State* L, Kind kind, const Property (&properties)[N]) {
    luaL_newmetatable(L, TypeName(kind));
    lua_pushstring(L, TypeName(kind));
    lua_setfield(L, -2, "__metatable");
    lua_pushstring(L, TypeName(kind));
    lua_setfield(L, -2, "__name");
    lua_pushcfunction(L, Equal);
    lua_setfield(L, -2, "__eq");
    lua_pushcfunction(L, Describe);
    lua_setfield(L, -2, "__tostring");
    for (bool write : {false, true}) {
        lua_pushinteger(L, static_cast<i32>(kind));
        lua_newtable(L);
        for (const auto& property : properties) {
            const char* function = write ? property.setter : property.getter;
            if (function) {
                lua_getglobal(L, function);
                lua_setfield(L, -2, property.name);
            }
        }
        lua_pushcclosure(L, write ? SetProperty : GetProperty, 2);
        lua_setfield(L, -2, write ? "__newindex" : "__index");
    }
    lua_pop(L, 1);
}

} // namespace

void InvalidateObjectHandles() {
    ++gSessionGeneration;
    gSessionOwner = gpGame;
    gCallbackFrames.clear();
}

BindingScope::BindingScope() : m_token(++gNextCallback) {
    RefreshSessionOwner();
    gCallbackFrames.push_back({m_token, {}});
}

BindingScope::~BindingScope() {
    std::erase_if(gCallbackFrames, [this](const CallbackFrame& frame) { return frame.token == m_token; });
}

template <typename T>
T* CheckObject(lua_State* L, i32 argument) {
    return static_cast<T*>(Checked(L, argument, ObjectKind<T>()));
}
template army* CheckObject<army>(lua_State*, i32);
template playerData* CheckObject<playerData>(lua_State*, i32);
template hero* CheckObject<hero>(lua_State*, i32);
template town* CheckObject<town>(lua_State*, i32);

void RegisterBindings(lua_State* L) {
    RegisterType(L, Kind::OBJECT_HERO, HERO_PROPERTIES);
    RegisterType(L, Kind::OBJECT_PLAYER, PLAYER_PROPERTIES);
    RegisterType(L, Kind::OBJECT_TOWN, TOWN_PROPERTIES);
    RegisterType(L, Kind::OBJECT_STACK, STACK_PROPERTIES);
    lua_register(L, "IsObjectValid", IsValid);
}

void PushBinding(lua_State* L, Binding<army*> value) { PushObject(L, value.Get()); }
void PushBinding(lua_State* L, Binding<hero*> value) { PushObject(L, value.Get()); }
void PushBinding(lua_State* L, Binding<playerData*> value) { PushObject(L, value.Get()); }
void PushBinding(lua_State* L, Binding<town*> value) { PushObject(L, value.Get()); }

void PushCallbackBinding(lua_State* L, Binding<army*> value) { PushObject(L, value.Get(), true); }
void PushCallbackBinding(lua_State* L, Binding<hero*> value) { PushObject(L, value.Get(), true); }
void PushCallbackBinding(lua_State* L, Binding<playerData*> value) { PushObject(L, value.Get(), true); }
void PushCallbackBinding(lua_State* L, Binding<town*> value) { PushObject(L, value.Get(), true); }

void PushBinding(lua_State* L, Binding<SCampaignChoice> value) {
    // Authored values are snapshots, independent of catalog replacement.
    const auto choice = value.Get();
    lua_newtable(L);
    lua_pushinteger(L, H2EnumIndex(choice.type));
    lua_setfield(L, -2, "type");
    lua_pushinteger(L, choice.value);
    lua_setfield(L, -2, "field");
    lua_pushinteger(L, choice.amount);
    lua_setfield(L, -2, "amount");
}

} // namespace ironfist::script
