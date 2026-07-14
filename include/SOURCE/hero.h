#ifndef HOMM2_SOURCE_HERO_H
#define HOMM2_SOURCE_HERO_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 34 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <SOURCE/armyGroup.h>

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
    HERO_SKILL_ESTATES = 13
} HeroSecondarySkill;

typedef enum HeroSkillLevel {
    HERO_SKILL_LEVEL_NONE = 0,
    HERO_SKILL_LEVEL_BASIC = 1,
    HERO_SKILL_LEVEL_ADVANCED = 2,
    HERO_SKILL_LEVEL_EXPERT = 3,
    HERO_SKILL_LEVEL_COUNT = 4
} HeroSkillLevel;

typedef enum HeroClassType {
    HERO_CLASS_KNIGHT = 0,
    HERO_CLASS_BARBARIAN = 1,
    HERO_CLASS_SORCERESS = 2,
    HERO_CLASS_WARLOCK = 3,
    HERO_CLASS_WIZARD = 4,
    HERO_CLASS_NECROMANCER = 5,
    HERO_CLASS_COUNT = 6
} HeroClassType;

typedef enum HeroPrimaryStat {
    HERO_PRIMARY_ATTACK = 0,
    HERO_PRIMARY_DEFENSE = 1,
    HERO_PRIMARY_SPELL_POWER = 2,
    HERO_PRIMARY_KNOWLEDGE = 3
} HeroPrimaryStat;

#define HERO_PRIMARY_STAT_COUNT 4
#define HERO_ARTIFACT_SLOT_COUNT 14
#define HERO_SPELL_COUNT 65
#define HERO_EXPERIENCE_GROWTH_FACTOR 1.2

typedef enum HeroConstant {
    HERO_OWNER_NONE = -1,
    HERO_DESTINATION_NONE = -1,
    HERO_INTERACTION_TURN_NONE = -99,
    HERO_LOCATION_TOWN = 0xa3,
    HERO_MAP_CHANGE_UNUSED = -999,
    HERO_MAP_CELL_PRESENT = 0x40,
    HERO_AVAILABLE_SLOT_COUNT = 2,
    HERO_AVAILABILITY_UNAVAILABLE = -1,
    HERO_AVAILABILITY_RETREATED = 0x40,
    HERO_EXPERIENCE_LEVEL_TABLE_COUNT = 12,
    HERO_EXPERIENCE_EXTRAPOLATION_FIRST_LEVEL = 13
} HeroConstant;

typedef enum HeroArtifact {
    HERO_ARTIFACT_NOMAD_BOOTS = 0x21,
    HERO_ARTIFACT_TRAVELER_BOOTS = 0x22,
    HERO_ARTIFACT_TRUE_COMPASS = 0x27,
    HERO_ARTIFACT_SAILORS_ASTROLABE = 0x28,
    HERO_ARTIFACT_MAGIC_BOOK = 0x51,
    HERO_ARTIFACT_SPELL_SCROLL = 0x56,
    HERO_ARTIFACT_BATTLE_GARB = 0x5a
} HeroArtifact;

typedef enum HeroSpell {
    HERO_SPELL_NONE = -1,
    HERO_SPELL_TOWN_PORTAL = 0x3a
} HeroSpell;

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

#pragma pack(push, 1)  // recovered layout is byte-packed
class hero {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    short  m_spellPoints;  // +0x00
    unsigned char m_id;  // +0x02
    char   m_owner;  // +0x03
    short m_lastHeroInteractionTurn;  // +0x04
    signed char m_lastInteractionHeroId;  // +0x06
    short m_lastTownInteractionTurn;  // +0x07
    unsigned char m_visitedTownId;  // +0x09
    char m_name[13];  // +0x0a
    unsigned char m_cursorType;  // +0x17
    unsigned char m_portrait;  // +0x18
    int    m_x;  // +0x19
    int    m_y;  // +0x1d
    int    m_destinationX;  // +0x21
    int    m_destinationY;  // +0x25
    union {
        struct {
            unsigned char m_boatId;  // +0x29
            char m_unknown2a;  // +0x2a
            signed char m_unknown2b;  // +0x2b
        };
        struct {
            signed char m_patrolX;  // +0x29
            signed char m_patrolY;  // +0x2a
            signed char m_patrolRadius;  // +0x2b
        };
    };
    unsigned char m_direction;  // +0x2c
    short  m_locationType;  // +0x2d
    short  m_occupiedTown;  // +0x2f
    int m_mobility;  // +0x31
    int m_remainingMobility;  // +0x35
    int m_experience;  // +0x39
    short m_level;  // +0x3d
    signed char m_primaryStats[4];  // +0x3f
    signed char m_unknown43;  // +0x43
    signed char m_morale;  // +0x44
    signed char m_luck;  // +0x45
    char _pad_0x46[0x1];
    unsigned int m_gazeboVisits;  // +0x47
    unsigned int m_fortVisits;  // +0x4b
    unsigned int m_witchDoctorVisits;  // +0x4f
    unsigned int m_mercenaryCampVisits;  // +0x53
    unsigned int m_standingStoneVisits;  // +0x57
    unsigned int m_treeKnowledgeVisits;  // +0x5b
    unsigned int m_xanaduVisits;  // +0x5f
    unsigned char m_randomSeed;  // +0x63
    unsigned char m_enabled;  // +0x64
    class armyGroup m_army;  // +0x65
    signed char m_secondarySkills[14];  // +0x74
    signed char m_secondarySkillOrder[14];  // +0x82
    int m_secondarySkillCount;  // +0x90
    signed char m_spells[HERO_SPELL_COUNT];  // +0x94
    signed char m_artifacts[HERO_ARTIFACT_SLOT_COUNT];  // +0xd5
    unsigned int m_eventFlags;  // +0xe3
    unsigned char m_isCaptain;  // +0xe7
    float m_aiFightValue;  // +0xe8
    signed char m_artifactExtra[14];  // +0xec
    int IsEmbarked(void) { return m_eventFlags & HERO_EVENT_EMBARKED; }
    // --- constructors ---
    hero(void);
    // --- methods ---
    void Read(int, signed char);
    void Write(int, signed char);
    void GetArmyStrengths(unsigned long int * const);
    int HasArtifact(int);
    int CalcMobility(void);
    int HasSpell(int);
    int GetNthSpell(int, int);
    int GetNumSpells(int);
    void UseSpell(int);
    void AddSpell(int, int);
    void HeroScreenUpdate(void);
    void UpdateArmies(void);
    void ViewStat(int, int);
    void ViewArtifact(int, int, int);
    int Dismiss(void);
    void Deallocate(int);
    int GetExperience(int);
    int GetLevel(int);
    void ApplyBattleWinTemps(void);
    void ApplyBattleLossTemps(void);
    void CheckLevel(void);
    int NumArtifacts(void);
    void SetSS(int, int);
    int TakeSS(int, int);
    int GiveSS(int, int);
    int CreatureTypeCount(int);
    void UpgradeCreatures(int, int);
    int GetNthSS(int);
    class town * GetOccupiedTown(void);
    signed char Stats(int);
    signed char GetSSLevel(int);
    void DoSSLevelDialog(int, int);
    void CheckAnduranPieces(int);
};
#pragma pack(pop)
SIZE(hero, 250);
// ---- globals (declarations, RVA order) ----
extern class hero *gpHVHero;
extern class heroWindow *gheroWin;
extern short gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT];
extern int iOrigHeroViewID;
extern int gbNoDismiss;

#endif // HOMM2_SOURCE_HERO_H
