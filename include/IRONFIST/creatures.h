#ifndef HOMM2_IRONFIST_CREATURES_H
#define HOMM2_IRONFIST_CREATURES_H

#include <array>
#include <string>
#include <vector>

#include <Ints.h>
#include <SOURCE/KB.h>
#include <SOURCE/KB_TYPES.h>

namespace ironfist {

/*
 * Ironfist's data-driven creature database: DATA/creatures.xml fills the
 * retail tables (gMonsterDatabase, the name and icon tables), which this
 * branch extends to KB_CREATURE_TABLE_CAPACITY. Ironfist adds twelve
 * creatures (72-83, Kobold through Cyber Behemoth), per-creature secondary
 * resource costs, data-driven random-spawn bounds, and a general attribute
 * engine for abilities the retail flag word cannot express.
 */

enum CreatureTableConstant {
    CREATURE_TABLE_UNUSED     = 255,
    MAX_BASE_CREATURE         = 65,
    MIN_EXTENDED_CREATURE     = 72,
    MAX_EXTENDED_CREATURE     = 83,
    SECONDARY_RESOURCE_COUNT = 6
};

enum class CreatureAttribute : u8 {
    StrikeAndReturn,
    PlasmaBlast,
    Teleporter,
    AstralDodge,
    ShadowMark,
    Jumper,
    Charger,
    Count
};

inline constexpr std::array<CreatureAttribute, H2EnumIndex(CreatureAttribute::Count)>
    CreatureAttributes = {
        CreatureAttribute::StrikeAndReturn,
        CreatureAttribute::PlasmaBlast,
        CreatureAttribute::Teleporter,
        CreatureAttribute::AstralDodge,
        CreatureAttribute::ShadowMark,
        CreatureAttribute::Jumper,
        CreatureAttribute::Charger
    };

extern std::vector<CreatureType> RandomizableCreatures;
extern i32 CreatureRandomBounds[KB_CREATURE_TABLE_CAPACITY][2];
extern i32 CreatureSecondaryCosts[KB_CREATURE_TABLE_CAPACITY]
                                    [SECONDARY_RESOURCE_COUNT];

// Faction starting-army and neutral-town garrison tables, table-driven so
// the Cyborg faction (row 12) can join.
struct StartingArmyRange {
    CreatureType creatureType;
    i32 lowQuantity;
    i32 highQuantity;
};

enum FactionTableConstant {
    HERO_ARMY_TIER_COUNT    = 2,
    NEUTRAL_TOWN_TIER_COUNT = 5
};

extern StartingArmyRange
    StartingArmyBounds[KB_FACTION_TABLE_CAPACITY][HERO_ARMY_TIER_COUNT];
extern CreatureType
    NeutralTownCreatures[KB_FACTION_TABLE_CAPACITY][NEUTRAL_TOWN_TIER_COUNT];

// Random towns and starting factions draw from the playable factions,
// including the Cyborgs.
extern std::vector<FactionType> PlayableFactions;

void LoadCreatures();
void UnloadCreatures();
bool HasCreatureAttribute(CreatureType creature, CreatureAttribute attribute);
char* GetCreatureName(i32 id);
char* GetCreaturePluralName(i32 id);

} // namespace ironfist

#endif
