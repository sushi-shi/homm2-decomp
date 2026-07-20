#ifndef HOMM2_SOURCE_HERO_H
#define HOMM2_SOURCE_HERO_H

#include <va.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/KB_TYPES.h>

class town;

H2_ENUM_CLASS_BEGIN(HeroSpellType)
    SPELL_TYPE_COMBAT    = 0,
    SPELL_TYPE_ADVENTURE = 1,
    SPELL_TYPE_ALL       = 2
H2_ENUM_CLASS_END(HeroSpellType)

H2_ENUM_CLASS_BEGIN(HeroSecondarySkill)
    HERO_SKILL_PATHFINDING = 0,
    HERO_SKILL_ARCHERY     = 1,
    HERO_SKILL_LOGISTICS   = 2,
    HERO_SKILL_SCOUTING    = 3,
    HERO_SKILL_DIPLOMACY   = 4,
    HERO_SKILL_NAVIGATION  = 5,
    HERO_SKILL_LEADERSHIP  = 6,
    HERO_SKILL_WISDOM      = 7,
    HERO_SKILL_MYSTICISM   = 8,
    HERO_SKILL_LUCK        = 9,
    HERO_SKILL_BALLISTICS  = 10,
    HERO_SKILL_EAGLE_EYE   = 11,
    HERO_SKILL_NECROMANCY  = 12,
    HERO_SKILL_ESTATES     = 13,
    HERO_SKILL_COUNT       = 14
H2_ENUM_CLASS_END(HeroSecondarySkill)

H2_ENUM_CLASS_BEGIN(HeroSkillLevel)
    HERO_SKILL_LEVEL_NONE     = 0,
    HERO_SKILL_LEVEL_BASIC    = 1,
    HERO_SKILL_LEVEL_ADVANCED = 2,
    HERO_SKILL_LEVEL_EXPERT   = 3,
    HERO_SKILL_LEVEL_COUNT    = 4
H2_ENUM_CLASS_END(HeroSkillLevel)

H2_ENUM_CLASS_BEGIN(HeroPrimaryStat)
    HERO_PRIMARY_ATTACK      = 0,
    HERO_PRIMARY_DEFENSE     = 1,
    HERO_PRIMARY_SPELL_POWER = 2,
    HERO_PRIMARY_KNOWLEDGE   = 3
H2_ENUM_CLASS_END(HeroPrimaryStat)

#define HERO_EXPERIENCE_GROWTH_FACTOR 1.2

H2_ENUM_BEGIN(HeroConstant)
    HERO_OWNER_NONE                           = -1,
    HERO_BOAT_NONE                            = 0xff,
    HERO_DESTINATION_NONE                     = -1,
    HERO_INTERACTION_TURN_NONE                = -99,
    HERO_LOCATION_TOWN                        = 0xa3,
    HERO_MAP_CHANGE_UNUSED                    = -999,
    HERO_MAP_CELL_PRESENT                     = 0x40,
    HERO_NAME_SIZE                            = 13,
    HERO_PRIMARY_STAT_COUNT                   = 4,
    HERO_ARTIFACT_SLOT_COUNT                  = 14,
    HERO_RUNTIME_ALIGNMENT_SIZE               = 1,
    HERO_STARTING_STAT_COUNT                  = 5,
    HERO_AVAILABLE_SLOT_COUNT                 = 2,
    HERO_AVAILABILITY_UNAVAILABLE             = -1,
    HERO_AVAILABILITY_RETREATED               = 0x40,
    HERO_AVAILABILITY_JAILED                  = 0x41,
    HERO_EXPERIENCE_LEVEL_TABLE_COUNT         = 12,
    HERO_EXPERIENCE_EXTRAPOLATION_FIRST_LEVEL = 13,
    HERO_INITIAL_LEVEL                        = 1,
    HERO_LEVEL_HIGH_THRESHOLD                 = 9,
    HERO_LEVEL_RANDOM_SEED_FACTOR             = 30,
    HERO_LEVEL_RANDOM_MAX                     = 100,
    HERO_LEVEL_TEXT_BUFFER_SIZE               = 200,
    HERO_SECONDARY_SKILL_CHOICE_COUNT         = 2,
    HERO_SECONDARY_SKILL_CAPACITY             = 8,
    HERO_SECONDARY_SKILL_RANDOM_WEIGHT        = 200,
    HERO_SECONDARY_SKILL_SEARCH_LIMIT         = 5000,
    HERO_SECONDARY_SKILL_OFFER_GAP            = 3,
    HERO_SECONDARY_SKILL_ICON_STRIDE          = 3,
    HERO_SECONDARY_SKILL_ORDER_BASE           = 1,
    HERO_SECONDARY_SKILL_ICON_FRAME_BASE      = 1,
    HERO_SECONDARY_SKILL_NONE                 = -1,
    HERO_MINIMUM_SPELL_POWER                  = 1,
    HERO_SPELL_POINTS_PER_KNOWLEDGE           = 10,
    HERO_NECROMANCY_BONUS_MAX                 = 6,
    HERO_NECROMANCY_EFFECTIVE_LEVEL_MAX       = 9,
    HERO_NECROMANCY_PERCENT_PER_LEVEL         = 10
H2_ENUM_END(HeroConstant)

H2_ENUM_CLASS_BEGIN_T(HeroEventFlag, u32)
    HERO_EVENT_NONE                       = 0,
    HERO_EVENT_BUOY                       = 0x2,
    ADVMGR_VISIT_FORT                     = HERO_EVENT_BUOY,
    HERO_EVENT_FOUNTAIN                   = 0x4,
    ADVMGR_VISIT_GAZEBO                   = HERO_EVENT_FOUNTAIN,
    HERO_EVENT_OASIS                      = 0x8,
    ADVMGR_VISIT_MERCENARY_CAMP           = HERO_EVENT_OASIS,
    HERO_EVENT_FAERIE_RING                = 0x10,
    ADVMGR_VISIT_STANDING_STONES          = HERO_EVENT_FAERIE_RING,
    HERO_EVENT_GRAVEYARD                  = 0x20,
    HERO_EVENT_SHIPWRECK                  = 0x40,
    HERO_EVENT_EMBARKED                   = 0x80,
    HERO_EVENT_TEMPLE                     = 0x100,
    ADVMGR_VISIT_WITCH_DOCTOR             = HERO_EVENT_TEMPLE,
    HERO_EVENT_WATERING_HOLE              = 0x200,
    ADVMGR_VISIT_EVENT_SITE               = HERO_EVENT_WATERING_HOLE,
    HERO_EVENT_DERELICT_SHIP              = 0x400,
    HERO_EVENT_MAGIC_WELL                 = 0x1000,
    ADVMGR_VISIT_XANADU                   = HERO_EVENT_MAGIC_WELL,
    HERO_EVENT_IDOL                       = 0x2000,
    ADVMGR_VISIT_TREE_OF_KNOWLEDGE        = HERO_EVENT_IDOL,
    HERO_EVENT_PYRAMID                    = 0x4000,
    HERO_EVENT_WEEKLY_VISIT               = 0x10000,
    WEEKLY_HERO_RESERVED_FLAG             = HERO_EVENT_WEEKLY_VISIT,
    HERO_EVENT_MERMAID                    = 0x100000,
    AI_GENERIC_SITE_FAERIE_RING_FLAG      = HERO_EVENT_MERMAID,
    ADVMGR_VISIT_GENERIC_TOWER            = HERO_EVENT_MERMAID,
    HERO_EVENT_SIRENS                     = 0x200000,
    AI_GENERIC_SITE_GRAVEYARD_FLAG        = HERO_EVENT_SIRENS,
    ADVMGR_VISIT_GENERIC_SPRING           = HERO_EVENT_SIRENS,
    HERO_EVENT_ARENA                      = 0x400000,
    AI_GENERIC_SITE_SHIPWRECK_FLAG        = HERO_EVENT_ARENA,
    ADVMGR_VISIT_GENERIC_HUT              = HERO_EVENT_ARENA,
    HERO_EVENT_STABLES                    = 0x800000,
    AI_GENERIC_SITE_CREATURE_UPGRADE_FLAG = HERO_EVENT_STABLES,
    ADVMGR_VISIT_GENERIC_ALTAR            = HERO_EVENT_STABLES,
    WEEKLY_HERO_VISIT_FLAG                = HERO_EVENT_STABLES,
    HERO_EVENT_GROUPED_FORMATION          = 0x00008000
H2_ENUM_CLASS_END_T(HeroEventFlag, u32)
H2_ENUM_FLAGS(HeroEventFlag)

#pragma pack(push, 1)
class hero {
public:
    i16 m_spellPoints;
    u8 m_id;
    char m_owner;
    i16 m_lastHeroInteractionTurn;
    i8 m_lastInteractionHeroId;
    i16 m_lastTownInteractionTurn;
    u8 m_visitedTownId;
    char m_name[HERO_NAME_SIZE];
    H2_ENUM_STORAGE(HeroCursorType, u8) m_cursorType;
    u8 m_portrait;
    i32 m_x;
    i32 m_y;
    i32 m_destinationX;
    i32 m_destinationY;
    union {
        struct {
            u8 m_boatId;
            char m_boatDestY;
            i8 m_boatTravelRange;
        };
        struct {
            i8 m_patrolX;
            i8 m_patrolY;
            i8 m_patrolRadius;
        };
    };
    u8 m_direction;
    i16 m_locationType;
    i16 m_occupiedTown;
    i32 m_mobility;
    i32 m_remainingMobility;
    i32 m_experience;
    i16 m_level;
    i8 m_primaryStats[IDX(HERO_STARTING_STAT_COUNT)];
    i8 m_morale;
    i8 m_luck;
    char _pad_0x46[HERO_RUNTIME_ALIGNMENT_SIZE];
    u32 m_gazeboVisits;
    u32 m_fortVisits;
    u32 m_witchDoctorVisits;
    u32 m_mercenaryCampVisits;
    u32 m_standingStoneVisits;
    u32 m_treeKnowledgeVisits;
    u32 m_xanaduVisits;
    u8 m_randomSeed;
    u8 m_enabled;
    class armyGroup m_army;
    i8 m_secondarySkills[IDX(HERO_SKILL_COUNT)];
    u8 m_secondarySkillOrder[IDX(HERO_SKILL_COUNT)];
    i32 m_secondarySkillCount;
    i8 m_spells[IDX(SPELL_COUNT)];
    i8 m_artifacts[HERO_ARTIFACT_SLOT_COUNT];
    HeroEventFlag m_eventFlags;
    u8 m_isCaptain;
    float m_aiFightValue;
    i8 m_artifactExtra[HERO_ARTIFACT_SLOT_COUNT];
    i32 IsEmbarked(void) {
        return HAS(m_eventFlags, HERO_EVENT_EMBARKED);
    }
    hero(void);
    void Read(i32, i8);
    void Write(i32, i8);
    void GetArmyStrengths(u32l* const);
    i32 HasArtifact(ArtifactType);
    i32 CalcMobility(void);
    i32 HasSpell(SpellType);
    SpellType GetNthSpell(HeroSpellType, i32);
    i32 GetNumSpells(HeroSpellType);
    void UseSpell(SpellType);
    void AddSpell(i32, i32);
    void HeroScreenUpdate(void);
    void UpdateArmies(void);
    void ViewStat(i32, i32);
    void ViewArtifact(ArtifactType, b32, i32);
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
    i8 Stats(HeroPrimaryStat);
    i8 GetSSLevel(i32);
    void DoSSLevelDialog(i32, i32);
    void CheckAnduranPieces(i32);
};
#pragma pack(pop)
SIZE(hero, 250);
extern class hero* gpHVHero;
extern class heroWindow* gheroWin;
extern i16 gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT];
extern i32 iOrigHeroViewID;
extern b32 gbNoDismiss;

#endif
