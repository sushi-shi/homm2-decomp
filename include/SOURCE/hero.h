#ifndef HOMM2_SOURCE_HERO_H
#define HOMM2_SOURCE_HERO_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 34 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/KB_TYPES.h>

// forward declarations:
class town;

typedef enum HeroSecondarySkill {
    HERO_SKILL_PATHFINDING = 0,
    HERO_SKILL_ARCHERY = 1,
    HERO_SKILL_LOGISTICS = 2,
    HERO_SKILL_SCOUTING = 3,
    HERO_SKILL_DIPLOMACY = 4,
    HERO_SKILL_NAVIGATION = 5,
    HERO_SKILL_LEADERSHIP = 6,
    HERO_SKILL_WISDOM = 7,
    HERO_SKILL_MYSTICISM = 8,
    HERO_SKILL_LUCK = 9,
    HERO_SKILL_BALLISTICS = 10,
    HERO_SKILL_EAGLE_EYE = 11,
    HERO_SKILL_NECROMANCY = 12,
    HERO_SKILL_ESTATES = 13,
    HERO_SKILL_COUNT = 14
} HeroSecondarySkill;

typedef enum HeroSkillLevel {
    HERO_SKILL_LEVEL_NONE = 0,
    HERO_SKILL_LEVEL_BASIC = 1,
    HERO_SKILL_LEVEL_ADVANCED = 2,
    HERO_SKILL_LEVEL_EXPERT = 3,
    HERO_SKILL_LEVEL_COUNT = 4
} HeroSkillLevel;

typedef enum HeroPrimaryStat {
    HERO_PRIMARY_ATTACK = 0,
    HERO_PRIMARY_DEFENSE = 1,
    HERO_PRIMARY_SPELL_POWER = 2,
    HERO_PRIMARY_KNOWLEDGE = 3
} HeroPrimaryStat;

#define HERO_PRIMARY_STAT_COUNT 4
#define HERO_ARTIFACT_SLOT_COUNT 14
#define HERO_EXPERIENCE_GROWTH_FACTOR 1.2

typedef enum HeroConstant {
    HERO_OWNER_NONE = -1,
    HERO_DESTINATION_NONE = -1,
    HERO_INTERACTION_TURN_NONE = -99,
    HERO_LOCATION_TOWN = 0xa3,
    HERO_MAP_CHANGE_UNUSED = -999,
    HERO_MAP_CELL_PRESENT = 0x40,
    HERO_STARTING_STAT_COUNT = 5,
    HERO_AVAILABLE_SLOT_COUNT = 2,
    HERO_AVAILABILITY_UNAVAILABLE = -1,
    HERO_AVAILABILITY_RETREATED = 0x40,
    HERO_EXPERIENCE_LEVEL_TABLE_COUNT = 12,
    HERO_EXPERIENCE_EXTRAPOLATION_FIRST_LEVEL = 13,
    HERO_LEVEL_HIGH_THRESHOLD = 9,
    HERO_LEVEL_RANDOM_SEED_FACTOR = 30,
    HERO_LEVEL_RANDOM_MAX = 100,
    HERO_LEVEL_TEXT_BUFFER_SIZE = 200,
    HERO_SECONDARY_SKILL_CHOICE_COUNT = 2,
    HERO_SECONDARY_SKILL_CAPACITY = 8,
    HERO_SECONDARY_SKILL_RANDOM_WEIGHT = 200,
    HERO_SECONDARY_SKILL_SEARCH_LIMIT = 5000,
    HERO_SECONDARY_SKILL_OFFER_GAP = 3,
    HERO_SECONDARY_SKILL_ICON_STRIDE = 3,
    HERO_SECONDARY_SKILL_ORDER_BASE = 1,
    HERO_SECONDARY_SKILL_ICON_FRAME_BASE = 1,
    HERO_SECONDARY_SKILL_NONE = -1,
    HERO_MINIMUM_SPELL_POWER = 1,
    HERO_SPELL_POINTS_PER_KNOWLEDGE = 10,
    HERO_NECROMANCY_BONUS_MAX = 6,
    HERO_NECROMANCY_EFFECTIVE_LEVEL_MAX = 9,
    HERO_NECROMANCY_PERCENT_PER_LEVEL = 10
} HeroConstant;

typedef enum HeroArtifactConstant {
    HERO_ARTIFACT_TRANSFERABLE_FIRST = ARTIFACT_ARCANE_NECKLACE
} HeroArtifactConstant;

typedef enum HeroSpellType {
    HERO_SPELL_TYPE_COMBAT = 0,
    HERO_SPELL_TYPE_ADVENTURE = 1,
    HERO_SPELL_TYPE_ALL = 2
} HeroSpellType;

typedef enum HeroMobilityConstant {
    HERO_BASE_RECORD_SIZE = 0xec,
    HERO_SEA_BASE_MOBILITY = 1500,
    HERO_LIGHTHOUSE_MOBILITY_BONUS = 500,
    HERO_ASTROLABE_MOBILITY_BONUS = 1000,
    HERO_COMPASS_MOBILITY_BONUS = 500,
    HERO_NOMAD_BOOTS_MOBILITY_BONUS = 600,
    HERO_TRAVELER_BOOTS_MOBILITY_BONUS = 300,
    HERO_STABLES_MOBILITY_BONUS = 400,
    HERO_AI_DIFFICULTY_MOBILITY_BONUS = 75,
    HERO_AI_STATE_MOBILITY_BONUS = 50,
    HERO_LIGHTHOUSE_MINE_TYPE = 100
} HeroMobilityConstant;

typedef enum HeroEventFlag {
    HERO_EVENT_BUOY = 0x2,
    HERO_EVENT_FOUNTAIN = 0x4,
    HERO_EVENT_OASIS = 0x8,
    HERO_EVENT_FAERIE_RING = 0x10,
    HERO_EVENT_GRAVEYARD = 0x20,
    HERO_EVENT_SHIPWRECK = 0x40,
    HERO_EVENT_EMBARKED = 0x80,
    HERO_EVENT_TEMPLE = 0x100,
    HERO_EVENT_WATERING_HOLE = 0x200,
    HERO_EVENT_DERELICT_SHIP = 0x400,
    HERO_EVENT_MAGIC_WELL = 0x1000,
    HERO_EVENT_IDOL = 0x2000,
    HERO_EVENT_PYRAMID = 0x4000,
    HERO_EVENT_WEEKLY_VISIT = 0x10000,
    HERO_EVENT_MERMAID = 0x100000,
    HERO_EVENT_SIRENS = 0x200000,
    HERO_EVENT_ARENA = 0x400000,
    HERO_EVENT_STABLES = 0x800000,
    HERO_EVENT_GROUPED_FORMATION = 0x00008000
} HeroEventFlag;

#pragma pack(push, 1) // recovered layout is byte-packed
class hero {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    i16 m_spellPoints;             // +0x00
    u8 m_id;                       // +0x02
    char m_owner;                  // +0x03
    i16 m_lastHeroInteractionTurn; // +0x04
    i8 m_lastInteractionHeroId;    // +0x06
    i16 m_lastTownInteractionTurn; // +0x07
    u8 m_visitedTownId;            // +0x09
    char m_name[13];               // +0x0a
    u8 m_cursorType;               // +0x17
    u8 m_portrait;                 // +0x18
    i32 m_x;                       // +0x19
    i32 m_y;                       // +0x1d
    i32 m_destinationX;            // +0x21
    i32 m_destinationY;            // +0x25
    union {
        struct {
            u8 m_boatId;          // +0x29
            char m_boatDestY;     // +0x2a
            i8 m_boatTravelRange; // +0x2b
        };
        struct {
            i8 m_patrolX;      // +0x29
            i8 m_patrolY;      // +0x2a
            i8 m_patrolRadius; // +0x2b
        };
    };
    u8 m_direction;                              // +0x2c
    i16 m_locationType;                          // +0x2d
    i16 m_occupiedTown;                          // +0x2f
    i32 m_mobility;                              // +0x31
    i32 m_remainingMobility;                     // +0x35
    i32 m_experience;                            // +0x39
    i16 m_level;                                 // +0x3d
    i8 m_primaryStats[HERO_STARTING_STAT_COUNT]; // +0x3f
    i8 m_morale;                                 // +0x44
    i8 m_luck;                                   // +0x45
    char _pad_0x46[0x1];
    u32 m_gazeboVisits;                       // +0x47
    u32 m_fortVisits;                         // +0x4b
    u32 m_witchDoctorVisits;                  // +0x4f
    u32 m_mercenaryCampVisits;                // +0x53
    u32 m_standingStoneVisits;                // +0x57
    u32 m_treeKnowledgeVisits;                // +0x5b
    u32 m_xanaduVisits;                       // +0x5f
    u8 m_randomSeed;                          // +0x63
    u8 m_enabled;                             // +0x64
    class armyGroup m_army;                   // +0x65
    i8 m_secondarySkills[14];                 // +0x74
    u8 m_secondarySkillOrder[14];             // +0x82
    i32 m_secondarySkillCount;                // +0x90
    i8 m_spells[SPELL_COUNT];                 // +0x94
    i8 m_artifacts[HERO_ARTIFACT_SLOT_COUNT]; // +0xd5
    u32 m_eventFlags;                         // +0xe3
    u8 m_isCaptain;                           // +0xe7
    float m_aiFightValue;                     // +0xe8
    i8 m_artifactExtra[14];                   // +0xec
    i32 IsEmbarked(void) {
        return m_eventFlags & HERO_EVENT_EMBARKED;
    }
    // --- constructors ---
    hero(void);
    // --- methods ---
    void Read(i32, i8);
    void Write(i32, i8);
    void GetArmyStrengths(u32l* const);
    i32 HasArtifact(i32);
    i32 CalcMobility(void);
    i32 HasSpell(i32);
    i32 GetNthSpell(i32, i32);
    i32 GetNumSpells(i32);
    void UseSpell(i32);
    void AddSpell(i32, i32);
    void HeroScreenUpdate(void);
    void UpdateArmies(void);
    void ViewStat(i32, i32);
    void ViewArtifact(i32, i32, i32);
    i32 Dismiss(void);
    void Deallocate(i32);
    i32 GetExperience(i32);
    i32 GetLevel(i32);
    void ApplyBattleWinTemps(void);
    void ApplyBattleLossTemps(void);
    void CheckLevel(void);
    i32 NumArtifacts(void);
    void SetSS(i32, i32);
    i32 TakeSS(i32, i32);
    i32 GiveSS(i32, i32);
    i32 CreatureTypeCount(i32);
    void UpgradeCreatures(i32, i32);
    i32 GetNthSS(i32);
    class town* GetOccupiedTown(void);
    i8 Stats(i32);
    i8 GetSSLevel(i32);
    void DoSSLevelDialog(i32, i32);
    void CheckAnduranPieces(i32);
};
#pragma pack(pop)
SIZE(hero, 250);
// ---- globals (declarations, RVA order) ----
extern class hero* gpHVHero;
extern class heroWindow* gheroWin;
extern i16 gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT];
extern i32 iOrigHeroViewID;
extern i32 gbNoDismiss;

#endif // HOMM2_SOURCE_HERO_H
