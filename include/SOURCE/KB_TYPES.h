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
enum { RES_WOOD, RES_MERCURY, RES_ORE, RES_SULFUR, RES_CRYSTAL, RES_GEMS, RES_GOLD };
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
typedef unsigned int UInt32;
struct MemEntry;
struct _SAMPLE;                // SAMPLE2::pMem — opaque

enum { MENU_ENABLE_STATUS_COUNT = 70 };
#pragma pack(push, 1)
struct SMenuEnableStatus {
    unsigned int command;
    unsigned char normalEnabled;
    unsigned char setupEnabled;
    // All 70 retail values are zero, and the executable has no xref to this byte.
    unsigned char reserved;
};
#pragma pack(pop)
SIZE(SMenuEnableStatus, 7);

#include <SOURCE/CONFIG_TYPES.h>
struct tag_tilePoint {
    signed char x;
    signed char y;
    short frameOffset;
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
            short cost;
            union {
                int fightValue;
                int randomValue;
            };
        };
    };
    signed char iconIndex;
    signed char growth;
    unsigned short hitPoints;
    signed char race;
    signed char speed;
    signed char attack;
    signed char defense;
    signed char damageMin;
    signed char damageMax;
    signed char shots;
    char spriteName[MONSTER_SPRITE_NAME_SIZE];
    union {
        int attributes;
        union {
            int all;
            int abilityFlags;
            struct {
                signed char abilities;
                signed char attributes;
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
    short x[2];
    short y[2];
    unsigned char idleAnimationCount;
    signed char animationFrameCount[COMBAT_HERO_ANIMATION_COUNT];
    signed char animationFrames[COMBAT_HERO_ANIMATION_COUNT]
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
    unsigned char level;
    unsigned char iconIndex;
    unsigned char combatEffect;
    short aiValue;
    unsigned char cost;
    unsigned char raceChance[6];
    unsigned char attributes;
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
    unsigned char windowId;
    unsigned short widgetId;
    char *text;
};
SIZE(SWinSetup, 7);
#pragma pack(pop)

#endif // HOMM2_SOURCE_KB_TYPES_H
