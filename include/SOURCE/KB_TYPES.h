#ifndef HOMM2_SOURCE_KB_TYPES_H
#define HOMM2_SOURCE_KB_TYPES_H
// Compatibility aggregate for shared records used by KB.cpp and its consumers. Domain
// headers are canonical where split out; MSVC mirrors remain flattened when a nested include
// boundary would perturb cumulative compiler state.
#include <va.h>
// Forward declarations for opaque pointer members.
class sample;

// Resource-type index for the game's per-resource arrays (gafAITurnCostResource, p[] amounts,
// etc.), in the standard HoMM2 order. MERCURY/ORE/CRYSTAL are confirmed by the retail's
// per-element .rdata float symbols referenced in philAI::RVConversion; the rest follow the
// canonical order.
HOMM2_ENUM_BEGIN(ResourceType)
    RES_WOOD = 0,
    RES_MERCURY = 1,
    RES_ORE = 2,
    RES_SULFUR = 3,
    RES_CRYSTAL = 4,
    RES_GEMS = 5,
    RES_GOLD = 6
HOMM2_ENUM_END(ResourceType)

HOMM2_ENUM_BEGIN(FactionType)
    FACTION_KNIGHT = 0,
    FACTION_BARBARIAN = 1,
    FACTION_SORCERESS = 2,
    FACTION_WARLOCK = 3,
    FACTION_WIZARD = 4,
    FACTION_NECROMANCER = 5,
    FACTION_COUNT = 6
HOMM2_ENUM_END(FactionType)

HOMM2_ENUM_BEGIN(TerrainType)
    TERRAIN_WATER = 0,
    TERRAIN_GRASS = 1,
    TERRAIN_SNOW = 2,
    TERRAIN_SWAMP = 3,
    TERRAIN_LAVA = 4,
    TERRAIN_DESERT = 5,
    TERRAIN_DIRT = 6,
    TERRAIN_WASTELAND = 7,
    TERRAIN_BEACH = 8,
    TERRAIN_COUNT = 9
HOMM2_ENUM_END(TerrainType)

typedef enum MonsterFlags {
    MONSTER_FLAGS_WIDE = 0x00000001,
    MONSTER_FLAGS_FLYING = 0x00000002,
    MONSTER_FLAGS_LIGHT_PALETTE = 0x00000100,
    MONSTER_FLAGS_RED_PALETTE = 0x00000200,
    MONSTER_FLAGS_DARK_BROWN_PALETTE = 0x00002000,
    MONSTER_FLAGS_GRAY_PALETTE = 0x00004000,
    MONSTER_FLAGS_SHOOTER = 0x00000004,
    MONSTER_FLAGS_BREATH_ATTACK = 0x00000008,
    MONSTER_FLAGS_DEAD = 0x00000010,
    MONSTER_FLAGS_AI_EXCLUDED = 0x00000010,
    MONSTER_FLAGS_RETALIATED = 0x00000040,
    MONSTER_FLAGS_WOKE_FROM_DAMAGE = 0x00000080,
    MONSTER_FLAGS_FULL_AI_QUANTITY = 0x00000080,
    MONSTER_FLAGS_MIRROR_IMAGE = 0x00000100,
    MONSTER_FLAGS_SUMMONED = 0x00000800,
    MONSTER_FLAGS_UNDEAD = 0x00000400,
    MONSTER_FLAGS_NO_MORALE = 0x00000400
} MonsterFlags;

typedef enum MonsterAbilityFlags {
    MONSTER_ABILITY_FLAG_WIDE = 0x01,
    MONSTER_ABILITY_FLAG_FLYING = 0x02,
    MONSTER_ABILITY_FLAG_SHOOTER = 0x04,
    MONSTER_ABILITY_FLAG_BREATH_ATTACK = 0x08,
    MONSTER_ABILITY_FLAG_AI_EXCLUDED = 0x10,
    MONSTER_ABILITY_FLAG_HIGH_MORALE = 0x20,
    MONSTER_ABILITY_FLAG_BAD_MORALE = 0x80,
    MONSTER_ABILITY_FLAG_FULL_AI_QUANTITY = 0x80,
    MONSTER_ABILITY_FLAG_DEFERRED_TURN = 0x1000
} MonsterAbilityFlags;

// Retained in the historical declaration sequence because MSVC 4.2's cumulative TU state
// changes code generation when these otherwise redundant declarations are removed.
typedef u32 UInt32;
struct MemEntry;
struct _SAMPLE;                // SAMPLE2::pMem — opaque

enum { MENU_ENABLE_STATUS_COUNT = 70 };
#pragma pack(push, 1)
struct SMenuEnableStatus {
    u32 command;
    u8 normalEnabled;
    u8 setupEnabled;
    // All 70 retail values are zero, and the executable has no xref to this byte.
    u8 reserved;
};
#pragma pack(pop)
SIZE(SMenuEnableStatus, 7);

#include <SOURCE/CONFIG_TYPES.h>
struct tag_tilePoint {
    i8 x;
    i8 y;
    i16 frameOffset;
};
SIZE(tag_tilePoint, 4);
typedef enum MonsterDatabaseConstant {
    MONSTER_DATABASE_COUNT = 66,
    MONSTER_SPRITE_NAME_SIZE = 5
} MonsterDatabaseConstant;
#pragma pack(push, 1)
struct tag_monsterInfo {
    union {
        struct {
            i16 cost;
            union {
                i32 fightValue;
                i32 randomValue;
            };
        };
    };
    i8 iconIndex;
    i8 growth;
    u16 hitPoints;
    i8 race;
    i8 speed;
    i8 attack;
    i8 defense;
    i8 damageMin;
    i8 damageMax;
    i8 shots;
    char spriteName[MONSTER_SPRITE_NAME_SIZE];
    union {
        i32 attributes;
        union {
            i32 all;
            i32 abilityFlags;
            struct {
                i8 abilities;
                i8 attributeFlags;
            } bytes;
        } flags;
    };
};
#pragma pack(pop)
SIZE(tag_monsterInfo, 0x1a);
typedef enum CombatHeroAnimationConstant {
    COMBAT_HERO_ANIMATION_COUNT = 12,
    COMBAT_HERO_ANIMATION_FRAME_COUNT = 9
} CombatHeroAnimationConstant;

#pragma pack(push, 1)
struct SCmbtHero {
    i16 x[2];
    i16 y[2];
    u8 idleAnimationCount;
    i8 animationFrameCount[COMBAT_HERO_ANIMATION_COUNT];
    i8 animationFrames[COMBAT_HERO_ANIMATION_COUNT]
                               [COMBAT_HERO_ANIMATION_FRAME_COUNT];
};
SIZE(SCmbtHero, 0x81);
#pragma pack(pop)
typedef enum SpellInfoAttribute {
    SPELL_INFO_ATTRIBUTE_POWER = 0x01,
    SPELL_INFO_ATTRIBUTE_COMBAT = 0x02,
    SPELL_INFO_ATTRIBUTE_ADVENTURE = 0x04,
    SPELL_INFO_ATTRIBUTE_DURATION = 0x08
} SpellInfoAttribute;

struct SSpellInfo {
    char soundName[9];
    u8 level;
    u8 iconIndex;
    u8 combatEffect;
    i16 aiValue;
    u8 cost;
    u8 raceChance[6];
    u8 attributes;
};  // gsSpellInfo[] (sizeof 22)
SIZE(SSpellInfo, 0x16);
struct SAMPLE2 { class sample *pSample; struct _SAMPLE *pMem; };            // NULL_SAMPLE2

typedef enum MonsterAttribute {
    MONSTER_ATTRIBUTE_WIDE = 0x01,
    MONSTER_ATTRIBUTE_FLYING = 0x02,
    MONSTER_ATTRIBUTE_RANGED = 0x04,
    MONSTER_ATTRIBUTE_TWO_HEX_ATTACKER = 0x08,
    MONSTER_ATTRIBUTE_UNDEAD = 0x0400,
    MONSTER_ATTRIBUTE_DEAD = 0x10,
    MONSTER_ATTRIBUTE_RETALIATED = 0x40,
    MONSTER_ATTRIBUTE_WOKE_FROM_DAMAGE = 0x80
} MonsterAttribute;
#pragma pack(push, 1)
struct SWinSetup {
    u8 windowId;
    u16 widgetId;
    char *text;
};
SIZE(SWinSetup, 7);
#pragma pack(pop)

#endif // HOMM2_SOURCE_KB_TYPES_H
