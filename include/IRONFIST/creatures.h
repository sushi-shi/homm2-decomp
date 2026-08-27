#ifndef HOMM2_IRONFIST_CREATURES_H
#define HOMM2_IRONFIST_CREATURES_H

#include <string>
#include <vector>

#include <Ints.h>
#include <SOURCE/KB.h>
#include <SOURCE/KB_TYPES.h>

/*
 * Ironfist's data-driven creature database: DATA/creatures.xml fills the
 * retail tables (gMonsterDatabase, the name and icon tables), which this
 * branch extends to KB_CREATURE_TABLE_CAPACITY. Ironfist adds twelve
 * creatures (72-83, Kobold through Cyber Behemoth), per-creature secondary
 * resource costs, data-driven random-spawn bounds, and a general attribute
 * engine for abilities the retail flag word cannot express.
 */

enum IronfistCreature {
    CREATURE_KOBOLD_ID                = 72,
    CREATURE_BLOODSUCKER_ID           = 73,
    CREATURE_HARPY_ID                 = 74,
    CREATURE_BLACK_KNIGHT_ID          = 75,
    CREATURE_CATOBLEBA_ID             = 76,
    CREATURE_TREANT_ID                = 77,
    CREATURE_CYBER_KOBOLD_SPEARMAN_ID = 78,
    CREATURE_CYBER_PLASMA_BERSERKER_ID = 79,
    CREATURE_CYBER_PLASMA_LANCER_ID   = 80,
    CREATURE_CYBER_INDIGO_ID          = 81,
    CREATURE_CYBER_SHADOW_ASSASSIN_ID = 82,
    CREATURE_CYBER_BEHEMOTH_ID        = 83,
    CREATURE_INVALID_ID               = 255,

    MAX_BASE_CREATURE                 = 65,
    MIN_IRONFIST_CREATURE             = 72,
    MAX_IRONFIST_CREATURE             = 83,

    IRONFIST_SECONDARY_RESOURCE_COUNT = 6
};

// Ironfist ability names understood by the general attribute engine.
#define STRIKE_AND_RETURN "strike-and-return"
#define PLASMA_BLAST      "plasma-blast"
#define TELEPORTER        "teleporter"
#define ASTRAL_DODGE      "astral-dodge"
#define SHADOW_MARK       "shadow-mark"
#define JUMPER            "jumper"
#define CHARGER           "charger"

extern i32 giNumCreatures;
extern std::vector<CreatureType> CREATURES_RANDOMIZABLE;
extern i32 gMonRandBound[KB_CREATURE_TABLE_CAPACITY][2];
extern i32 gMonSecondaryResourceCost[KB_CREATURE_TABLE_CAPACITY]
                                    [IRONFIST_SECONDARY_RESOURCE_COUNT];

// Faction starting-army and neutral-town garrison tables, table-driven so
// the Cyborg faction (row 12) can join.
struct randomHeroCreatureInfo {
    CreatureType creatureType;
    i32 lowQuantity;
    i32 highQuantity;
};

enum IronfistFactionTableConstant {
    IRONFIST_FACTION_TABLE_COUNT       = 13,
    IRONFIST_HERO_ARMY_TIER_COUNT      = 2,
    IRONFIST_NEUTRAL_TOWN_TIER_COUNT   = 5
};

extern randomHeroCreatureInfo
    randomHeroArmyBounds[IRONFIST_FACTION_TABLE_COUNT][IRONFIST_HERO_ARMY_TIER_COUNT];
extern CreatureType
    neutralTownCreatureTypes[IRONFIST_FACTION_TABLE_COUNT][IRONFIST_NEUTRAL_TOWN_TIER_COUNT];

// The playable factions, including the Cyborgs; random towns and random
// starting factions draw from this list.
extern std::vector<FactionType> FACTIONS_ACTUAL;
extern std::vector<std::string> FACTIONS_ACTUAL_NAMES;

void LoadCreatures();
void UnloadCreatures();
void GrantCreatureAttribute(i32 id, std::string name);
i32 CreatureHasAttribute(i32 id, const std::string& name);
extern std::vector<std::string> ironfistAttributeNames;
const char* GetCreatureName(i32 id);
const char* GetCreaturePluralName(i32 id);
i32 GetNumCreatures();
void Ironfist_GetMonsterCost(i32 monster, i32* const costs);

#endif
