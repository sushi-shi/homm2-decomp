#include <Ints.h>
#include <SOURCE/KB_TYPES.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/PHILAI.h>
#include <BASE/Misc.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/KB.h>
#include <SOURCE/PATH.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <SOURCE/philAI.h>
#include <SOURCE/hero.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/advManager.h>
#include <SOURCE/town.h>
#include <SOURCE/game.h>
#include <SOURCE/GAME.h>
#include <SOURCE/TOWNMGR.h>
#include <SOURCE/playerData.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/CURSOR.h>
#include <EDITOR/mapcell.h>
#include <BASE/mouseManager.h>

#define AI_SECONDARY_SKILL_FIGHT_SCALE                                             \
    2500.0f
#define AI_SECONDARY_SKILL_BASE_FACTOR                                             \
    0.5
#define AI_SECONDARY_SKILL_NAVIGATION_FACTOR 1.3
#define AI_SECONDARY_SKILL_ARCHERY_SHARE 0.2
#define AI_SECONDARY_SKILL_ARCHERY_BASE                                            \
    0.25
#define AI_GENERIC_SITE_ARENA_VALUE 1000.0f
#define AI_GENERIC_SITE_MERMAID_VALUE                                              \
    200.0f
#define AI_GENERIC_SITE_DAY_VALUE 200
#define AI_MINIMUM_FIGHT_VALUE 100
#define AI_FRAME_THROTTLE_TICKS 15
#define AI_MINIMUM_TURN_MOBILITY 225
#define AI_MINIMUM_SITE_MOBILITY 75
#define AI_HERO_PURCHASE_GOLD_FLOOR 2500
#define AI_BERSERK_THRESHOLD 30000
#define AI_ZERO_FLOAT 0.0f
#define AI_RELOAD_BASE 2.0f
#define AI_RELOAD_NUMERATOR 3.0f
#define AI_EASY_STRENGTH_FACTOR                                                    \
    1.15
#define AI_BERSERK_FACTOR 0.75
#define AI_HERO_VALUE_SCALE 0.00004
#define AI_HERO_VALUE_BASE 0.4
#define AI_ARTIFACT_POOL_DIVISOR                                                   \
    33.0
#define AI_BATTLE_TOWN_DEFENDER_FACTOR                                             \
    1.11
#define AI_BATTLE_DIFFICULTY_STEP                                                  \
    0.15
#define AI_BATTLE_BASE_STRENGTH_FACTOR 0.7
#define AI_BATTLE_EASY_STRENGTH_FACTOR                                             \
    1.08
#define AI_BATTLE_HUMAN_DEFENDER_FACTOR                                            \
    1.14
#define AI_BATTLE_MINIMUM_STRENGTH                                                 \
    1.0f
#define AI_BATTLE_FULL_CHANCE 1.0f
#define AI_BATTLE_LARGE_STRENGTH                                                   \
    1000000.0f
#define AI_BATTLE_NORMAL_POWER 2.75f
#define AI_BATTLE_LARGE_POWER 2.0f
#define AI_BATTLE_ZERO_CHANCE 0.08
#define AI_BATTLE_LOW_CHANCE 0.12
#define AI_BATTLE_LOW_PENALTY                                                      \
    0.07
#define AI_BATTLE_MEDIUM_CHANCE 0.2
#define AI_BATTLE_MEDIUM_PENALTY 0.05
#define AI_BATTLE_HIGH_CHANCE 0.3
#define AI_BATTLE_HIGH_PENALTY 0.04
#define AI_BATTLE_TOP_CHANCE 0.4
#define AI_BATTLE_TOP_PENALTY 0.02
#define AI_BATTLE_LOSS_FACTOR_BASE                                                 \
    1.33
#define AI_BATTLE_PLAYER_FACTOR_BASE 0.66
#define AI_BATTLE_HERO_EXPERIENCE_FACTOR 0.8
#define AI_BATTLE_PERCENT_SCALE                                                    \
    100.0f
#define AI_QUICK_COMBAT_EXPERIENCE_DIFFICULTY_STEP 0.12
#define AI_QUICK_COMBAT_WIN_BONUS_THRESHOLD                                       \
    0.6
#define AI_QUICK_COMBAT_WIN_BONUS                                                  \
    0.65
#define AI_QUICK_COMBAT_HIGH_WIN_CHANCE 0.8
#define AI_QUICK_COMBAT_MINIMUM_CASUALTY 0.2
#define AI_QUICK_COMBAT_CASUALTY_CAP_CHANCE 0.96
#define AI_QUICK_COMBAT_CASUALTY_DIVISOR 2.0f
#define AI_QUICK_COMBAT_DAMAGE_PENALTY_LOW 0.6
#define AI_QUICK_COMBAT_DAMAGE_PENALTY_HIGH                                       \
    0.99
#define AI_QUICK_COMBAT_DAMAGE_PENALTY 0.1
#define AI_QUICK_COMBAT_DEFEAT_THRESHOLD 0.99
#define AI_QUICK_COMBAT_NECROMANCY_THRESHOLD                                      \
    0.999
#define AI_QUICK_COMBAT_NECROMANCY_FACTOR 0.12
#define AI_TOWN_DEFENSE_THRESHOLD 0.7
#define AI_TOWN_ARCHER_OVERWHELMING_FACTOR                                        \
    1.25
#define AI_TOWN_ARCHER_ADVANTAGE_FACTOR                                            \
    0.9
#define AI_UPGRADE_EXISTING_STACK_FACTOR                                           \
    1.2
#define AI_HERO_INTERACTION_ALPHA_SHARE 0.6
#define AI_HERO_INTERACTION_TARGET_SHARE 0.7f
#define AI_HERO_INTERACTION_MINIMUM_TRANSFER 0.15
#define AI_HERO_INTERACTION_VALUE_FACTOR 0.1
#define AI_PURCHASE_RANDOM_DIVISOR 100.0
#define AI_CREATURE_BALANCE_BASE 0.66
#define AI_CREATURE_VISITING_HERO_FACTOR                                           \
    1.1
#define AI_CREATURE_RANGED_BASE_FACTOR                                             \
    1.18
#define AI_CREATURE_RANGED_STACK_FACTOR                                            \
    0.06
#define AI_CREATURE_DANGER_BASE                                                    \
    0.96
#define AI_CREATURE_EASY_COST_FACTOR 1.3
#define AI_CREATURE_RANDOM_BASE 90
#define AI_HERO_PURCHASE_BONUS 500.0f
#define AI_HERO_PURCHASE_FACTOR 1.3
#define AI_FIGHT_EVENT_REWARD_500_VALUE                                           \
    500.0f
#define AI_FIGHT_EVENT_REWARD_1000_VALUE                                          \
    1000.0f
#define AI_FIGHT_EVENT_REWARD_2000_VALUE                                          \
    2000.0f
#define AI_FIGHT_EVENT_REWARD_3000_VALUE                                          \
    3000.0f
#define AI_FIGHT_EVENT_REWARD_5000_VALUE                                          \
    5000.0f
#define AI_HERO_PURCHASE_GOLD_COST 2500
#define AI_HERO_PURCHASE_EXPERIENCE_BASE 2000
#define AI_HERO_PURCHASE_CELL_PENALTY 200.0f
#define AI_HERO_PURCHASE_RANDOM_BASE                                               \
    90.0
#define AI_PURCHASE_INITIAL_VALUE -99.0f
#define AI_MINIMUM_PURCHASE_VALUE 0.02
#define AI_DIMENSION_DOOR_SPELL_POINTS 30
#define AI_MAX_MAGE_GUILD_LEVEL 5
#define AI_TOWN_ARMY_SLOTS 5
#define AI_BUILDING_MAGE_GUILD_MASK 0x00000001
#define AI_BUILDING_SHIPYARD_MASK 0x00000008
#define AI_BUILDING_CASTLE_MASK 0x40
#define AI_BUILDING_LEFT_TURRET_MASK 0x00000100
#define AI_BUILDING_RIGHT_TURRET_MASK 0x00000200
#define AI_MAGIC_BOOK_COST 500
#define AI_MANA_PER_KNOWLEDGE 10
#define AI_EARLY_TOWN_SHARE_TURN 7
#define AI_SECOND_WEEK_END_TURN 14
#define AI_THIRD_WEEK_END_TURN 21
#define AI_MINIMUM_TOWN_SHARE_DIFFERENCE 0.08
#define AI_TOWN_SHARE_DIFFERENCE_FACTOR 0.15
#define AI_TOWN_TRANSFER_BONUS 0.04
#define AI_THIRD_WEEK_TOWN_SHARE_FACTOR 0.8
#define AI_WEAKER_ARMY_TRANSFER_FACTOR 0.25f
#define AI_STRONGER_ARMY_TRANSFER_FACTOR 0.13f
#define AI_TOWN_TRANSFER_CURVE_OFFSET 1.0f
#define AI_TOWN_TRANSFER_CURVE_CENTER                                              \
    0.22
#define AI_PATHFINDING_DESERT_MOBILITY_FACTOR                                      \
    1.35
#define AI_LARGE_MAP_MOBILITY_FACTOR                                               \
    1.05
#define AI_MAGE_GUILD_LATE_FACTOR                                                  \
    0.55
#define AI_EARLY_DWELLING_FACTOR_BASE                                              \
    1.66
#define AI_CONQUERED_HERO_MOBILITY_LIMIT 20
#define AI_UNGUARDED_TOWN_VALUE 500
#define AI_TROOP_EMPTY_SLOT -1
#define AI_TROOP_REDISTRIBUTION_WORST_VALUE -9999
#define AI_TROOP_REDISTRIBUTION_MIN_SPEED 1
#define AI_TROOP_REDISTRIBUTION_MAX_SPEED 9
#define AI_TROOP_REDISTRIBUTION_ROUNDING 0.5
#define AI_TROOP_REDISTRIBUTION_STACK_SHARE 0.65
#define AI_TROOP_REDISTRIBUTION_REMAINDER_FACTOR 0.2
#define AI_TROOP_REDISTRIBUTION_BUDGET_FACTOR 1.2
#define AI_CREATURE_SELECTION_WORST_VALUE 1000000000.0f
#define AI_CREATURE_OFF_RACE_FACTOR                                                \
    0.35
#define AI_CREATURE_SELECTION_BASE_VALUE                                           \
    500.0
#define AI_DWELLING_LEVELS 12
#define AI_GOLD_RESOURCE_MULTIPLIER 100
#define AI_CAMPFIRE_RESOURCE_AMOUNT 5.0f
#define AI_CAMPFIRE_GOLD_AMOUNT 500.0f
#define AI_CAMPFIRE_AVERAGE_DIVISOR                                                \
    6.0f
#define AI_OASIS_VALUE_FACTOR 350.0f
#define AI_TREASURE_CHEST_GOLD_AMOUNT                                              \
    1600.0f
#define AI_TEMPLE_VALUE_FACTOR 400.0f
#define AI_MORALE_LUCK_SITE_VALUE_FACTOR 200.0f
#define AI_WATERING_HOLE_VALUE_FACTOR                                              \
    300.0f
#define AI_GAZEBO_VALUE_FACTOR 1000.0f
#define AI_TRAINING_SITE_VALUE_FACTOR 1000.0f
#define AI_TREE_KNOWLEDGE_VALUE_FACTOR 1500.0f
#define AI_TREE_KNOWLEDGE_GOLD_COST 2000.0f
#define AI_TREE_KNOWLEDGE_GEM_COST                                                 \
    10.0f
#define AI_XANADU_VALUE_MULTIPLE 8.0f
#define AI_TRAVEL_GATE_INITIAL_VALUE -9999
#define AI_TRAVEL_GATE_EXIT_SCALE                                                  \
    0.85
#define AI_TRAVEL_GATE_EXIT_RADIUS 3
#define AI_TRAVEL_GATE_EXIT_DEPTH 700
#define AI_TRAVEL_GATE_CURRENT_DEPTH 500
#define AI_TRAVEL_GATE_PENALTY 200
#define AI_POSITION_NEARBY_DELTA 1
#define AI_POSITION_DISTANCE_BASE 0.2
#define AI_POSITION_STRATEGIC_DISTANCE_BASE 1.0f
#define AI_STRATEGIC_DISTANCE_OFFSET                                               \
    50.0
#define AI_POSITION_EMBARKED_DISTANCE_FACTOR 0.5
#define AI_POSITION_LAND_DISTANCE_1 1.0f
#define AI_POSITION_LAND_DISTANCE_2                                                \
    1.5
#define AI_POSITION_LAND_DISTANCE_3 2.0f
#define AI_POSITION_LAND_DISTANCE_4                                                \
    3.0f
#define AI_POSITION_LAND_DISTANCE_5                                                \
    4.0f
#define AI_POSITION_LAND_DISTANCE_6                                                \
    5.0f
#define AI_POSITION_LAND_FACTOR_2 1.2
#define AI_POSITION_LAND_FACTOR_3                                                  \
    1.4
#define AI_POSITION_LAND_FACTOR_4                                                  \
    1.7
#define AI_POSITION_LAND_FACTOR_5 2.0
#define AI_POSITION_LAND_FACTOR_6                                                  \
    2.5
#define AI_POSITION_LAND_FACTOR_FAR 3.0f
#define AI_ATTENTION_RANDOM_DIVISOR 500.0
#define AI_ATTENTION_RANDOM_BASE                                                   \
    0.23
#define AI_ATTENTION_WEIGHT_A_PLAYER_FACTOR 0.07
#define AI_ATTENTION_WEIGHT_B_PLAYER_FACTOR 0.15
#define AI_TOWN_CASTLE_PRIMARY_SKILL_SHARE_BASE                                    \
    0.54
#define AI_TOWN_CASTLE_PRIMARY_SKILL_SHARE_STEP 0.02
#define AI_TOWN_PRIMARY_SKILL_SHARE_BASE                                           \
    0.33
#define AI_TOWN_PRIMARY_SKILL_SHARE_STEP                                           \
    0.01
static const float AI_TARGET_HUMAN_VALUE_FACTOR = 1.5f;
#define AI_EVENT_HUMAN_VALUE_FACTOR 1.5
static const float AI_STRATEGIC_POSITION_SCORE_FACTOR = 1.25f;
static const float AI_CREATURE_SAME_RACE_FACTOR = 1.1f;
static const float AI_FUTURE_DEFLATION_RATE = 0.15f;
static const float AI_HERO_PURCHASE_SAME_RACE_FACTOR = 0.16f;
static const float AI_ATTENTION_IDENTITY_FLOAT = 1.0f;
#define AI_HERO_PURCHASE_CLASS_DIVISOR 2.0f
#define AI_HERO_PURCHASE_IDENTITY 1.0
static const float AI_ATTENTION_IDENTITY = 1.0f;
#define AI_ATTENTION_PLAYER_CENTER                                                \
    3.0
#define AI_ATTENTION_NORMALIZER                                                   \
    4.0
#define AI_ATTENTION_UPPER_BOUND 5.0
#define AI_MONSTER_JOIN_RATIO 2.0f
#define AI_MONSTER_OVERWHELMING_RATIO 5.0f
#define AI_MONSTER_JOIN_CHANCE_SCALE                                              \
    60.0f
#define AI_MONSTER_JOIN_CHANCE_BASE                                               \
    40.0f
#define AI_MONSTER_FIGHT_CHANCE_SCALE 100.0f
#define AI_MONSTER_JOIN_PURCHASE_WEIGHT 0.6
#define AI_MONSTER_JOIN_OUTCOME_WEIGHT 0.4
#define AI_EVENT_TOWN_PLAYER_FACTOR 0.25
#define AI_EVENT_VALUE_BASE_FACTOR 1.0
#define AI_TOWN_EVENT_VALUE_BASE_FACTOR 0.9
#define AI_TOWN_PRIMARY_HUMAN_VALUE_FACTOR                                         \
    1.6
#define AI_TOWN_OTHER_HUMAN_VALUE_FACTOR 1.3
#define AI_EVENT_CERTAIN_ODDS 0.75
#define AI_EVENT_HIGH_ODDS 0.5
#define AI_EVENT_GOOD_ODDS 0.4
#define AI_EVENT_POOR_ODDS 0.3
#define AI_EVENT_BAD_ODDS 0.2
#define AI_EVENT_CERTAIN_CHANCE 100.0f
#define AI_EVENT_HIGH_CHANCE_SCALE                                                 \
    136.0f
#define AI_EVENT_GOOD_CHANCE_SCALE                                                 \
    130.0f
#define AI_EVENT_POOR_CHANCE_SCALE                                                 \
    125.0f
#define AI_EVENT_BAD_CHANCE_SCALE                                                  \
    113.0f
#define AI_DAEMON_FIGHT_VALUE_SHARE 0.3
#define AI_DAEMON_SECONDARY_FIGHT_VALUE_SHARE 0.1
#define AI_EVENT_VALUE_SCALE 1000.0
#define AI_DAEMON_GOLD_VALUE_FACTOR 2500.0f
#define AI_DAEMON_GOLD_PENALTY                                                     \
    -750.0
#define AI_DAEMON_CAVE_GOLD_REQUIRED 2500
#define AI_MAGIC_GARDEN_GOLD_AMOUNT 500.0f
#define AI_FLOTSAM_GOLD_AMOUNT                                                     \
    175.0f
#define AI_FLOTSAM_WOOD_AMOUNT 5.0f
#define AI_SEA_CHEST_ARTIFACT_GOLD_AMOUNT 1000.0f
#define AI_SEA_CHEST_LARGE_GOLD_AMOUNT                                             \
    1500.0f
#define AI_SEA_CHEST_SMALL_GOLD_AMOUNT 200.0f
#define AI_SEA_CHEST_MINIMUM_VALUE 150
#define AI_WATER_WHEEL_GOLD_AMOUNT 500
#define AI_GOLD_TURN_VALUE_SCALE                                                   \
    250.0f
#define AI_OBELISK_VISITOR_COUNT_FLOAT                                             \
    48.0f
#define AI_FUTURE_MINIMUM_VALUE                                                    \
    0.0
#define AI_TOWN_FLYING_CREATURE_FACTOR                                             \
    0.95

typedef enum AIResourceValue {
    RV_UNSET      = -32001,
    NEARBY_RADIUS = 10
} AIResourceValue;

typedef enum AIArtifactEventConstant {
    AI_EVENT_RESOURCE_TYPE_MASK             = 0xf,
    AI_ARTIFACT_EVENT_CREATURE_MASK         = 0xff,
    AI_ARTIFACT_EVENT_GUARD_ROGUE_COUNT     = 10,
} AIArtifactEventConstant;

typedef enum AIFightEventConstant {
    FIGHT_EVENT_EMPTY             = 1,
    FIGHT_EVENT_LEVEL_1           = 2,
    FIGHT_EVENT_LEVEL_2           = 3,
    FIGHT_EVENT_LEVEL_3           = 4,
    FIGHT_EVENT_LEVEL_4           = 5,
    FIGHT_EVENT_STACKS            = 5,
    FIGHT_EVENT_LEVEL_OFFSET      = 2,
    FIGHT_EVENT_LEVEL_COUNT       = 4,
    FIGHT_EVENT_GRAVEYARD_COUNT_1 = 2,
    FIGHT_EVENT_GRAVEYARD_COUNT_2 = 3,
    FIGHT_EVENT_GRAVEYARD_COUNT_3 = 5,
    FIGHT_EVENT_GRAVEYARD_COUNT_4 = 10,
    FIGHT_EVENT_SHIPWRECK_COUNT_1 = 1,
    FIGHT_EVENT_SHIPWRECK_COUNT_2 = 3,
    FIGHT_EVENT_SHIPWRECK_COUNT_3 = 6,
    FIGHT_EVENT_SHIPWRECK_COUNT_4 = 12,
    FIGHT_EVENT_DERELICT_COUNT_1  = 5,
    FIGHT_EVENT_DERELICT_COUNT_2  = 10,
    FIGHT_EVENT_DERELICT_COUNT_3  = 20,
    FIGHT_EVENT_DERELICT_COUNT_4  = 40,
    FIGHT_EVENT_REWARD_500        = 500,
    FIGHT_EVENT_REWARD_1000       = 1000,
    FIGHT_EVENT_REWARD_2000       = 2000,
    FIGHT_EVENT_REWARD_3000       = 3000,
    FIGHT_EVENT_REWARD_5000       = 5000
} AIFightEventConstant;

typedef enum AIHeroInteractionConstant {
    HERO_INTERACTION_HERO_COUNT                = 2,
    HERO_INTERACTION_COMBAT_PRIMARY_STAT_COUNT = 2,
    HERO_INTERACTION_PRIMARY_STAT_VALUE        = 800,
    HERO_INTERACTION_COMBAT_STAT_MAX           = 10
} AIHeroInteractionConstant;

typedef enum AIHourglassConstant {
    HOURGLASS_PROSPECTIVE_HERO_LIMIT = 4,
    HOURGLASS_SECOND_HERO_COUNT      = 2,
    HOURGLASS_THIRD_HERO_COUNT       = 3,
    HOURGLASS_THIRD_HERO_SKIP_FIRST  = 3,
    HOURGLASS_THIRD_HERO_SKIP_SECOND = 6,
    HOURGLASS_LAST_PHASE             = 9
} AIHourglassConstant;

typedef enum AITownEventConstant {
    TOWN_EVENT_NO_HERO      = -1,
    TOWN_EVENT_USE_GARRISON = 1
} AITownEventConstant;

typedef enum AISecondarySkillConstant {
    SECONDARY_SKILL_LEVEL_OFFSET      = 1,
    SECONDARY_SKILL_RANGED_ATTRIBUTE  = 0x00040000,
    SECONDARY_SKILL_MINIMUM_KNOWLEDGE = 2
} AISecondarySkillConstant;

typedef enum AISideConstant {
    SIDE_NO_PLAYER                = -1,
    SIDE_PRIMARY_PLAYER           = 0,
    SIDE_CAMPAIGN_SCENARIO_OFFSET = 1,
    SIDE_CAMPAIGN_SCENARIO_NINE   = 9,
    SIDE_CAMPAIGN_SCENARIO_TEN    = 10,
    SIDE_CAMPAIGN_SCENARIO_ELEVEN = 11,
    SIDE_VICTORY_SPECIAL_VALUE    = 99,
    SIDE_FIRST_COLOR              = 0,
    SIDE_FOURTH_COLOR             = 3
} AISideConstant;

typedef enum AICreaturePurchaseConstant {
    CREATURE_PURCHASE_NO_SLOT          = -1,
    CREATURE_PURCHASE_VALUE_LIMIT      = 999999,
    CREATURE_PURCHASE_EXPENSIVE_VALUE  = 1000,
    CREATURE_PURCHASE_RANGED_ATTRIBUTE = 0x04
} AICreaturePurchaseConstant;

typedef enum AIRuntimeConstant {
    FRAME_TIMER_TICKS                         = 120,
    CREATURE_UPGRADE_ARMY_COUNT               = 2,
    CREATURE_UPGRADE_BUILDING_OFFSET          =
        H2EnumIndex(BUILDING_SLOT_UPGRADE_FIRST) - H2EnumIndex(BUILDING_SLOT_DWELLING_SECOND),
    CREATURE_UPGRADE_RESOURCE_COST_MULTIPLIER = 2,
    PURCHASE_MINIMUM_GOLD                     = 200,
    SHIPYARD_GOLD_COST                        = 2000,
    SHIPYARD_WOOD_COST                        = 20,
    DIMENSION_DOOR_LANDING_CANDIDATE_COUNT    = 3,
    CREATURE_PURCHASE_UNLIMITED               = 9999
} AIRuntimeConstant;

typedef enum AIPositionValueConstant {
    POSITION_FULL_CHANCE          = 100,
    POSITION_MINIMUM_LIVE_CHANCE  = 30,
    POSITION_FAILED_VALUE         = -100,
    POSITION_EMBARKED_BOAT_BONUS  = 40,
    POSITION_DEBUG_LEVEL          = 5,
    POSITION_DEBUG_UNUSED         = -999,
    POSITION_OBJECT_NAME_COUNT    = 119,
    POSITION_DEBUG_TEXT_CAPACITY  = 100,
    POSITION_STRATEGIC_MULTIPLIER = 2
} AIPositionValueConstant;

typedef enum AIFightValueConstant {
    FIGHT_VALUE_SPELL_COUNT      = H2EnumIndex(SPELL_SUMMON_FIRE_ELEMENTAL) + 1,
    FIGHT_VALUE_SPELL_STAT_LIMIT = 10
} AIFightValueConstant;

typedef enum AIQuickCombatConstant {
    QUICK_COMBAT_TOWN_EXPERIENCE = 500,
    QUICK_COMBAT_RANDOM_LIMIT    = 100,
    QUICK_COMBAT_RETREAT_CHANCE  = 60,
    QUICK_COMBAT_LEARNABLE_SPELL = 2
} AIQuickCombatConstant;

typedef enum AIEventEvaluationConstant {
    EVENT_MODE_IGNORE                = 0,
    EVENT_MODE_AVOID                 = 1,
    EVENT_SEVERE_PENALTY             = -20000,
    EVENT_FRIENDLY_PENALTY           = -500,
    EVENT_ALLIED_PENALTY             = -1500,
    EVENT_INTERACTION_AGE            = 4,
    EVENT_HUMAN_VALUE_THRESHOLD      = 200,
    EVENT_EARLY_TURN_BASE            = 70,
    EVENT_EARLY_TURN_DIFFICULTY_STEP = 5
} AIEventEvaluationConstant;

typedef enum AITownEvaluationConstant {
    TOWN_EARLY_TURN_BASE            = 40,
    TOWN_EARLY_TURN_DIFFICULTY_STEP = 8
} AITownEvaluationConstant;

searchArray SVSearchArray;

void ResetHeroRVs(i32 resetAll, i32 x, i32 y) {
    i32 index;
    i32 node;

    if (x == -1 || y == -1)
        return;
    for (node = 0; node < MAP_WIDTH; node++) {
        for (index = 0; index < MAP_HEIGHT; index++) {
            if (resetAll != 0) {
                if (MANHATTAN_LENGTH(x - node, y - index) < NEARBY_RADIUS)
                    *(gaiHeroStrategicRVOfPos + node + index * MAP_WIDTH) = H2EnumIndex(RV_UNSET);
            } else {
                *(gaiHeroStrategicRVOfPos + node + index * MAP_WIDTH) = H2EnumIndex(RV_UNSET);
                *(gaiHeroEventStratRVOfPos + node + index * MAP_WIDTH) = H2EnumIndex(RV_UNSET);
            }
        }
    }
    *(gaiHeroEventStratRVOfPos + x + y * MAP_WIDTH) = H2EnumIndex(RV_UNSET);
    for (node = 0; node < GAME_HERO_COUNT; node++) {
        if (resetAll == 0
            || MANHATTAN_LENGTH(x - gpGame->m_heroRecs[node].m_x, y - gpGame->m_heroRecs[node].m_x)
                   < NEARBY_RADIUS)
            gaiHeroLiveChance[node] = H2EnumIndex(RV_UNSET);
    }
}

void CheckDoMain(i32 unused [[maybe_unused]], i32 doMain) {
    if (iLastFrameRateTimer + AI_FRAME_THROTTLE_TICKS < KBTickCount()
        || glTimers[0] < KBTickCount()) {
        Process1WindowsMessage();
        PollSound();
        if (glTimers[0] < KBTickCount()) {
            if (doMain == 0) {
                b32 oldShowIt = bShowIt;
                i32 oldX = gpAdvManager->m_previousOriginX;
                i32 oldY = gpAdvManager->m_previousOriginY;
                gbDrawSavedCursor = true;
                if (gConfig.blackoutComputer == 0 && gbRemoteOn == 0)
                    bShowIt = true;
                else
                    bShowIt = false;
                if (bShowIt == 0)
                    bSpecialHideCursor = true;
                if (gpAdvManager->ComboDraw(
                        gpAdvManager->m_previousOriginX,
                        gpAdvManager->m_previousOriginY,
                        0
                    ))
                    gpAdvManager->UpdateScreen(0, 0);
                else
                    gpAdvManager->UpdBottomView(false, true, true);
                bShowIt = oldShowIt;
                gbDrawSavedCursor = false;
                bSpecialHideCursor = false;
                gpAdvManager->m_previousOriginX = oldX;
                gpAdvManager->m_previousOriginY = oldY;
            }
            glTimers[0] = KBTickCount() + FRAME_TIMER_TICKS;
        }
        iLastFrameRateTimer = KBTickCount();
    }
}

void ShowStatus(void) {}

philAI::philAI(void) {
    i32 i;
    for (i = 0; i < GAME_PLAYER_COUNT; i++) {
        giBuildShipyard[i] = -1;
        giBuildBoat[i] = -1;
        giBuildBoatStuffTurn[i] = 0;
    }
}

void philAI::DoAllHeroInteractions(void) {
    i32 i;
    for (i = 0; i < gpCurPlayer->m_townCount; i++) {
        town* pTown = gpGame->GetTown(gpCurPlayer->m_townIds[i]);
        if (pTown->m_occupyingHeroId != -1)
            HeroInteractionAtTown(gpGame->GetHero(pTown->m_occupyingHeroId), pTown, 0, &iDummy);
    }
}

void philAI::CheckForCreatureUpgrades(void) {
    i32 townIndex;
    b32 hasUpgrade;
    armyGroup* army;
    CreatureType upgradedType = CREATURE_NONE;
    i32 slot;
    i32 armyNo;
    BuildingSlotType dwelling;
    i32 mergeIndex;
    town* townRef;
    ResourceType material;
    i32 goldAmount;
    i32 materialCost;

    for (townIndex = 0; townIndex < gpCurPlayer->m_townCount; townIndex++) {
        townRef = gpGame->GetTown(gpCurPlayer->m_townIds[townIndex]);
        for (armyNo = 0; armyNo < CREATURE_UPGRADE_ARMY_COUNT; armyNo++) {
            if (armyNo == 0)
                army = &townRef->m_army;
            else {
                if (townRef->m_occupyingHeroId == -1)
                    continue;
                army = &gpGame->GetHero(townRef->m_occupyingHeroId)->m_army;
            }
            for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; slot++) {
                if (army->m_creatureTypes[slot] == CREATURE_NONE)
                    continue;
                hasUpgrade = false;
                for (dwelling = BUILDING_SLOT_DWELLING_SECOND;
                     dwelling <= BUILDING_SLOT_DWELLING_SIXTH;
                     dwelling++) {
                    if (gDwellingType[H2EnumIndex(townRef->m_type)]
                                     [H2EnumIndex(dwelling) - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)]
                            == army->m_creatureTypes[slot]
                        && (H2EnumIndex((townRef->m_buildings) & ((1 << (H2EnumIndex(dwelling) + CREATURE_UPGRADE_BUILDING_OFFSET)))))) {
                        hasUpgrade = true;
                        upgradedType = NextCreatureType(army->m_creatureTypes[slot]);
                    }
                }
                if ((army->m_creatureTypes[slot] == CREATURE_GREEN_DRAGON
                     || army->m_creatureTypes[slot] == CREATURE_RED_DRAGON)
                    && (H2EnumIndex((townRef->m_buildings) & (H2EnumIndex(TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6))))) {
                    hasUpgrade = true;
                    upgradedType = CREATURE_BLACK_DRAGON;
                }
                if (hasUpgrade) {
                    goldAmount = army->m_creatureCounts[slot]
                               * CREATURE_UPGRADE_RESOURCE_COST_MULTIPLIER
                               * (gMonsterDatabase[H2EnumIndex(upgradedType)].cost
                                  - gMonsterDatabase[H2EnumIndex(army->m_creatureTypes[slot])].cost);
                    if (upgradedType == CREATURE_BLACK_DRAGON) {
                        material = RES_SULFUR;
                        materialCost = army->m_creatureCounts[slot]
                                     * CREATURE_UPGRADE_RESOURCE_COST_MULTIPLIER;
                    } else if (upgradedType == CREATURE_TITAN) {
                        material = RES_GEMS;
                        materialCost = army->m_creatureCounts[slot]
                                     * CREATURE_UPGRADE_RESOURCE_COST_MULTIPLIER;
                    } else {
                        material = RES_NONE;
                        materialCost = 0;
                    }
                    if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] >= goldAmount
                        && (material == RES_NONE
                            || gpCurPlayer->m_resources[H2EnumIndex(material)] >= materialCost)) {
                        gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= goldAmount;
                        if (material != RES_NONE)
                            gpCurPlayer->m_resources[H2EnumIndex(material)] -= materialCost;
                        army->m_creatureTypes[slot] = upgradedType;
                        for (mergeIndex = 0; mergeIndex < ARMY_GROUP_SLOT_COUNT; mergeIndex++) {
                            if (mergeIndex != slot
                                && army->m_creatureTypes[mergeIndex]
                                       == army->m_creatureTypes[slot]
                                && army->m_creatureCounts[mergeIndex] > 0) {
                                army->m_creatureCounts[mergeIndex] +=
                                    army->m_creatureCounts[slot];
                                army->m_creatureCounts[slot] = 0;
                                army->m_creatureTypes[slot] = CREATURE_NONE;
                            }
                        }
                    }
                }
            }
        }
    }
}

inline town* GetCastleSlot(i32 id) {
    return &gpGame->m_castleRecs[id];
}

void philAI::CheckBuyStuff(void) {
    b32 done = false;
    b32 bought [[maybe_unused]] = false;
    BHC bestBuy;
    town* dockTown;

    gpGame->CheckHeroConsistency();
    if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] < PURCHASE_MINIMUM_GOLD
        || (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] < AI_HERO_PURCHASE_GOLD_FLOOR
            && gpCurPlayer->m_heroCount == 0))
        return;
    LogInt("CheckBuy Start", gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)]);
    dockTown = NULL;
    if (giBuildShipyard[giCurPlayer] >= 0) {
        dockTown = &gpGame->m_castleRecs[giBuildShipyard[giCurPlayer]];
    } else if (giBuildBoat[giCurPlayer] >= 0) {
        dockTown = &gpGame->m_castleRecs[giBuildBoat[giCurPlayer]];
    }
    if (giBuildShipyard[giCurPlayer] >= 0) {
        dockTown = GetCastleSlot(giBuildShipyard[giCurPlayer]);
    } else if (giBuildBoat[giCurPlayer] >= 0) {
        dockTown = GetCastleSlot(giBuildBoat[giCurPlayer]);
    }
    if (dockTown != NULL && dockTown->m_owner != giCurPlayer) {
        giBuildShipyard[giCurPlayer] = -1;
        giBuildBoat[giCurPlayer] = -1;
        dockTown = NULL;
    }
    if (giBuildShipyard[giCurPlayer] >= 0) {
        if (CanBuy(dockTown, BUILDING_SLOT_DOCK) && CanBuild(dockTown, BUILDING_SLOT_DOCK)) {
            BuildBuilding(dockTown, BUILDING_SLOT_DOCK);
            giBuildShipyard[giCurPlayer] = -1;
        } else {
            gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= SHIPYARD_GOLD_COST;
            gpCurPlayer->m_resources[H2EnumIndex(RES_WOOD)] -= SHIPYARD_WOOD_COST;
        }
    }
    if (giBuildBoat[giCurPlayer] >= 0) {
        if ((H2EnumIndex((dockTown->m_buildings) & (AI_BUILDING_SHIPYARD_MASK)))
            && gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] >= TOWN_BOAT_GOLD_COST
            && gpCurPlayer->m_resources[H2EnumIndex(RES_WOOD)] >= TOWN_BOAT_WOOD_COST) {
            if (gpGame->GetBoatsBuilt() < GAME_BOAT_COUNT
                && gpAdvManager->GetCell(dockTown->m_boatX, dockTown->m_boatY)->m_triggerType
                       == MAP_OBJECT_NONE
                && gpGame->CreateBoat(dockTown->m_boatX, dockTown->m_boatY, 0) != -1) {
                gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= TOWN_BOAT_GOLD_COST;
                gpCurPlayer->m_resources[H2EnumIndex(RES_WOOD)] -= TOWN_BOAT_WOOD_COST;
            }
            giBuildBoat[giCurPlayer] = -1;
        } else {
            gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= TOWN_BOAT_GOLD_COST;
            gpCurPlayer->m_resources[H2EnumIndex(RES_WOOD)] -= TOWN_BOAT_WOOD_COST;
        }
    }
    CheckForCreatureUpgrades();
    DoAllHeroInteractions();
    while (!done) {
        GetBestBHC(giCurPlayer, bestBuy);
        if (H2EnumIndex(bestBuy.type) >= 0 && CanBuyBHC(bestBuy)) {
            switch (bestBuy.type) {
                case PURCHASE_BUILDING:
                    BuildBuilding(bestBuy.pTown, bestBuy.building);
                    break;
                case PURCHASE_HERO:
                    BuildHero(bestBuy.pTown, bestBuy.what);
                    break;
                case PURCHASE_CREATURE:
                    BuildCreature(bestBuy.pTown, bestBuy.what, bestBuy.num);
                    break;
            }
            bought = true;
        } else
            done = true;
    }
    if (giBuildShipyard[giCurPlayer] >= 0) {
        gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] += SHIPYARD_GOLD_COST;
        gpCurPlayer->m_resources[H2EnumIndex(RES_WOOD)] += SHIPYARD_WOOD_COST;
    }
    if (giBuildBoat[giCurPlayer] >= 0) {
        gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] += TOWN_BOAT_GOLD_COST;
        gpCurPlayer->m_resources[H2EnumIndex(RES_WOOD)] += TOWN_BOAT_WOOD_COST;
    }
    DoAllHeroInteractions();
    LogInt("CheckBuy End  ", gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)]);
}

inline hero* GetHeroSlot(i32 id) {
    return &gpGame->m_heroRecs[id];
}

i32 philAI::GoodAdjacent(MapDirection* direction) {
    i32 bestValue;
    i32 eventValue;
    float savedBerserkFactor;
    float savedReduceFactor;
    i32 targetY;
    i32 targetX;
    i32 successChance;
    MapDirection bestDirection;
    MapDirection directionIndex;

    bestDirection = MAP_DIRECTION_NONE;
    savedReduceFactor = fReduceFactor;
    savedBerserkFactor = fBerserkFactor;
    fReduceFactor = 1.0f;
    fBerserkFactor = 1.0f;
    bestValue = 100;
    if ((gpAdvManager->GetCell(gpCurAIHero->m_x, gpCurAIHero->m_y)->m_triggerType
         & MAP_TRIGGER_TYPE_MASK)
        == MAP_OBJECT_STONE_LITHS)
        return 0;
    for (directionIndex = MAP_DIRECTION_NORTH; H2EnumIndex(directionIndex) < H2EnumIndex(MAP_DIRECTION_COUNT);
         directionIndex++) {
        if (gpAdvManager->ValidMoveWithEvent(gpCurAIHero, directionIndex)) {
            targetX = gpCurAIHero->m_x + normalDirTable[H2EnumIndex(directionIndex)].x;
            targetY = gpCurAIHero->m_y + normalDirTable[H2EnumIndex(directionIndex)].y;
            if ((H2EnumIndex((gpAdvManager->GetCell(targetX, targetY)->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))
                && !(MAP_EXTRA_AT(targetX, targetY) & H2EnumIndex(MAP_EXTRA_ADJACENT_MONSTER))
                && ((gpAdvManager->GetCell(targetX, targetY)->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                    != MAP_OBJECT_STONE_LITHS)
                && ((gpAdvManager->GetCell(targetX, targetY)->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                    != MAP_OBJECT_WHIRLPOOL)) {
                if (gpAdvManager->GetCell(targetX, targetY)->m_triggerType
                        != (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
                    || GetCastleSlot(gpAdvManager->GetCell(targetX, targetY)->m_objectMetadata)->m_owner
                        != giCurPlayer
                    || gpCurAIHero->m_lastTownInteractionTurn != giCurTurn) {
                    if (gpAdvManager->GetCell(targetX, targetY)->m_triggerType
                            != (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))
                        || GetHeroSlot(gpAdvManager->GetCell(targetX, targetY)->m_objectMetadata)->m_owner
                            != giCurPlayer
                        || gpCurAIHero->m_lastHeroInteractionTurn != giCurTurn) {
                        eventValue = ValueOfEventAtPosition(targetX, targetY, 2, &successChance);
                        if (successChance > 80)
                            if (eventValue > bestValue) {
                                bestValue = eventValue;
                                bestDirection = directionIndex;
                            }
                    }
                }
            }
        }
    }
    fReduceFactor = savedReduceFactor;
    fBerserkFactor = savedBerserkFactor;
    if (bestDirection != MAP_DIRECTION_NONE) {
        *direction = bestDirection;
        return 1;
    }
    return 0;
}

void philAI::CheckReload(void) {
    i32 mapY;
    i32 mapX;
    mapCell* visitedCell;
    i32 heroFightValue;
    i32 enemyFightValue;
    float enemyPressure;
    float friendlySupport;

    gbTroopReload = false;
    fReduceFactor = 1.0f;
    friendlySupport = AI_ZERO_FLOAT;
    enemyPressure = 0.0f;
    heroFightValue = FightValueOfStack(&gpCurAIHero->m_army, gpCurAIHero, 0);
    if (heroFightValue < AI_MINIMUM_FIGHT_VALUE)
        heroFightValue = AI_MINIMUM_FIGHT_VALUE;
    gpSearchArray->SeedPosition(
        gpCurAIHero->m_x,
        gpCurAIHero->m_y,
        gpCurAIHero->m_direction,
        gpCurAIHero->m_mobility << 2,
        H2EnumIndex(gpCurAIHero->m_eventFlags) & H2EnumIndex(HERO_EVENT_EMBARKED),
        0,
        gpCurAIHero->m_remainingMobility,
        H2EnumIndex(gpCurAIHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_PATHFINDING)]),
        -1,
        -1,
        0,
        0
    );
    for (mapX = 0; mapX < MAP_WIDTH; mapX++) {
        for (mapY = 0; mapY < MAP_HEIGHT; mapY++) {
            if (gpSearchArray->GetRow(mapX, 1)[MAP_WIDTH * mapY].visited) {
                visitedCell = gpAdvManager->GetCell(mapX, mapY);
                switch (visitedCell->m_triggerType) {
                    case (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)):
                        enemyFightValue = FightValueOfStack(
                            &GetCastleSlot(visitedCell->m_objectMetadata)->m_army,
                            NULL,
                            0
                        );
                        if (gpGame->m_townOwners[visitedCell->m_objectMetadata] == gpCurAIHero->m_owner) {
                            if (enemyFightValue > heroFightValue * 2) {
                                friendlySupport +=
                                    (static_cast<float>(enemyFightValue) / (heroFightValue * 2) - 1.0f)
                                    * (gpCurAIHero->m_mobility + 10)
                                    / (gpSearchArray->GetRow(mapX, 1)[MAP_WIDTH * mapY].distance + 10);
                            }
                        } else if (enemyFightValue > (heroFightValue >> 1)) {
                            enemyPressure += (static_cast<float>(enemyFightValue) / (heroFightValue >> 1) - 1.0f)
                                   * (gpCurAIHero->m_mobility + 30)
                                   / (gpSearchArray->GetRow(mapX, 1)[MAP_WIDTH * mapY].distance + 30);
                        }
                        break;
                    case (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION)):
                        if (gpGame->m_availableHeroes[visitedCell->m_objectMetadata]
                            != gpCurAIHero->m_owner) {
                            enemyFightValue = FightValueOfStack(
                                &GetHeroSlot(visitedCell->m_objectMetadata)->m_army,
                                NULL,
                                0
                            );
                            if (enemyFightValue > (heroFightValue >> 1)) {
                                enemyPressure += (static_cast<float>(enemyFightValue) / (heroFightValue >> 1) - 1.0f)
                                       * (gpCurAIHero->m_mobility + 30)
                                       / (gpSearchArray->GetRow(mapX, 1)[MAP_WIDTH * mapY].distance + 30);
                            }
                        }
                }
            }
        }
    }
    if (friendlySupport > 1.0f && enemyPressure > 1.0f) {
        fReduceFactor = AI_RELOAD_NUMERATOR / (AI_RELOAD_BASE + friendlySupport + enemyPressure);
        gbTroopReload = true;
    }
}

void philAI::CheckBerserk(void) {
    i32 column;
    i32 heroFightValue;
    i32 best = -1;
    i32 row;
    mapCell* cell;
    i32 enemyFightValue;
    hero* heroPointer;

    gbBerserk = false;
    fBerserkFactor = 1.0f;
    heroFightValue = FightValueOfStack(&gpCurAIHero->m_army, gpCurAIHero, 1);
    if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_WARRIOR)
        heroFightValue = static_cast<i32>(heroFightValue * AI_EASY_STRENGTH_FACTOR);
    if (heroFightValue < AI_MINIMUM_FIGHT_VALUE)
        heroFightValue = AI_MINIMUM_FIGHT_VALUE;
    if (heroFightValue < AI_BERSERK_THRESHOLD)
        return;
    {
        for (column = 0; column < MAP_WIDTH; column++) {
            for (row = 0; row < MAP_HEIGHT; row++) {
                cell = gpAdvManager->GetCell(column, row);
                switch (cell->m_triggerType) {
                    case (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)):
                        if (gpGame->m_townOwners[cell->m_objectMetadata] != gpCurAIHero->m_owner) {
                            if (gpGame->m_townOwners[cell->m_objectMetadata] != -1) {
                                enemyFightValue = FightValueOfStack(
                                    &GetCastleSlot(cell->m_objectMetadata)->m_army,
                                    NULL,
                                    1,
                                    1,
                                    cell->m_objectMetadata,
                                    0
                                );
                                if (enemyFightValue > heroFightValue)
                                    return;
                                if (enemyFightValue > best)
                                    best = enemyFightValue;
                            }
                        }
                        break;
                    case (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION)):
                        if (gpGame->m_availableHeroes[cell->m_objectMetadata]
                            != gpCurAIHero->m_owner) {
                            heroPointer = GetHeroSlot(cell->m_objectMetadata);
                            enemyFightValue = FightValueOfStack(
                                &heroPointer->m_army,
                                NULL,
                                1,
                                heroPointer->m_locationType
                                    == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)),
                                heroPointer->m_occupiedTown,
                                0
                            );
                            if (enemyFightValue * 2 > heroFightValue)
                                return;
                            if (enemyFightValue * 2 > best)
                                best = enemyFightValue * 2;
                        }
                        break;
                }
            }
        }
        if (best <= 0)
            return;
        fBerserkFactor = best * AI_BERSERK_FACTOR / heroFightValue;
        gbBerserk = true;
    }
}

void philAI::DimensionDoorTo(i32 x, i32 y) {
    gpAdvManager->TeleportTo(gpCurAIHero, x, H2EnumIndex(y), 0, 0);
    if (gpCurAIHero->m_remainingMobility < AI_MINIMUM_TURN_MOBILITY)
        gpCurAIHero->m_remainingMobility = 0;
    else
        gpCurAIHero->m_remainingMobility -= AI_MINIMUM_TURN_MOBILITY;
    gpCurAIHero->UseSpell(SPELL_DIMENSION_DOOR);
}

i32 philAI::DoAnywhereDDoorTownGate(i32 targetValue) {
    i32 mapX;
    i32 bestX;
    i32 destinationX;
    i32 mapY;
    i32 destinationY;
    i32 bestY;
    i32 side;
    i32 x;
    i32 y;
    i32 posValue;
    i32 bestVal;
    mapCell* arriveCell;
    mapCell* cell;

    if (gpCurAIHero->IsEmbarked())
        return 0;
    {
        bestX = -1;
        if (targetValue > 25)
            bestVal = targetValue + 1000;
        else
            bestVal = Random(0, 125) - 50;
        if (gpCurAIHero->HasSpell(SPELL_DIMENSION_DOOR)) {
            if (gpCurAIHero->m_spellPoints >= AI_DIMENSION_DOOR_SPELL_POINTS) {
                for (x = 0; x < MAP_WIDTH; x++) {
                    for (y = 0; y < MAP_HEIGHT; y++) {
                        if (gpSearchArray->GetRow(x, 1)[MAP_WIDTH * y].visited)
                            continue;
                        if (abs(x - gpCurAIHero->m_x) > 7 || abs(y - gpCurAIHero->m_y) > 7)
                            continue;
                        if (x == gpCurAIHero->m_x && y == gpCurAIHero->m_y)
                            continue;
                        cell = gpAdvManager->GetCell(x, y);
                        if (CELL_TERRAIN(cell) == TERRAIN_WATER)
                            continue;
                        if (!((H2EnumIndex((cell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))
                              || (targetValue < 25 && Random(0, 10) < 2)))
                            continue;
                        posValue = RVOfPosition(
                            x,
                            y,
                            0,
                            -1,
                            -1,
                            0,
                            -1,
                            -1,
                            2,
                            gpCurAIHero->m_remainingMobility
                        );
                        posValue += Random(0, 30);
                        if (posValue > bestVal) {
                            for (side = 0; side < DIMENSION_DOOR_LANDING_CANDIDATE_COUNT; side++) {
                                if (side == 0) {
                                    destinationX = x - 1;
                                    destinationY = y;
                                } else if (side == 1) {
                                    destinationX = x;
                                    destinationY = y + 1;
                                } else {
                                    destinationX = x + 1;
                                    destinationY = y;
                                }
                                if (gpSearchArray->GetRow(destinationX, 1)[MAP_WIDTH * destinationY].visited)
                                    continue;
                                if (destinationX == gpCurAIHero->m_x && destinationY == gpCurAIHero->m_y)
                                    continue;
                                if (destinationX < 0 || destinationX > MAP_WIDTH - 1 || destinationY < 0
                                    || destinationY > MAP_HEIGHT - 1)
                                    continue;
                                if (gpAdvManager->FindAdjacentMonster(
                                        destinationX,
                                        destinationY,
                                        &mapX,
                                        &mapY,
                                        -1,
                                        -1
                                    ))
                                    continue;
                                arriveCell = gpAdvManager->GetCell(destinationX, destinationY);
                                if (CELL_TERRAIN(arriveCell) == TERRAIN_WATER)
                                    continue;
                                if (arriveCell->m_flags & H2EnumIndex(MAP_CELL_OCCUPIED))
                                    continue;
                                if ((H2EnumIndex((arriveCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG))))
                                    continue;
                                if (arriveCell->m_objectIndex != MAPCELL_SPRITE_NONE
                                    && !(arriveCell->m_flags & H2EnumIndex(MAP_CELL_OBJECT_SHADOW_ONLY)))
                                    continue;
                                bestX = destinationX;
                                bestY = destinationY;
                                bestVal = posValue;
                            }
                        }
                    }
                }
                if (bestX != -1) {
                    DimensionDoorTo(bestX, bestY);
                    return 1;
                }
            }
        }
    }
    return 0;
}

i32 philAI::DoDimensionDoor(hero* pHero) {
    i32 i;
    i32 x, y;
    i32 length;
    i32 bestX, bestY;
    mapCell* cell;
    if (pHero->m_remainingMobility < AI_MINIMUM_SITE_MOBILITY)
        return 0;
    bestX = -1;
    x = pHero->m_x;
    y = pHero->m_y;
    for (i = gpSearchArray->m_pathLength - 1; i >= 1; i--) {
        x += normalDirTable[gpSearchArray->m_storage.directions[i]].x;
        y += normalDirTable[gpSearchArray->m_storage.directions[i]].y;
        if (abs(x - pHero->m_x) <= 7 && abs(y - pHero->m_y) <= 7) {
            cell = gpAdvManager->GetCell(x, y);
            if (!(cell->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell->m_flags & H2EnumIndex(MAP_CELL_OCCUPIED))) {
                bestX = x;
                bestY = y;
                length = gpSearchArray->m_pathLength - i;
            }
        }
    }
    if (bestX == -1 || length <= 5)
        return 0;
    DimensionDoorTo(bestX, bestY);
    return 1;
}

void philAI::SetupRelativeHeroStrengths(void) {
    hero* alpha;
    i32 i;
    i32 bestStrength;
    i32 strength;
    bestStrength = -1;
    iAlphaMale = -1;
    for (i = 0; i < gpCurPlayer->m_heroCount; i++) {
        alpha = &gpGame->m_heroRecs[gpCurPlayer->m_heroIds[i]];
        strength = FightValueOfStack(&alpha->m_army, alpha, 1, 0, -1, 0);
        if (strength > bestStrength) {
            bestStrength = strength;
            iAlphaMale = alpha->m_id;
        }
    }
}

void ValidateHero(hero* pHero) {
    i32 i;
    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        if (pHero->m_army.m_creatureTypes[i] != CREATURE_NONE) {
            if (pHero->m_army.m_creatureTypes[i] < CREATURE_PEASANT
                || pHero->m_army.m_creatureTypes[i] >= CREATURE_COUNT) {
                i32 j [[maybe_unused]] = 1;
                j++;
            }
        }
    }
}

void philAI::DoAI(i32 player) {
    i32 hiddenPointers = 0;
    b32 hourglassShown = false;
    MapDirection adjacentDirection;
    b32 stopAfterStep;
    i32 eventY;
    i32 eventX;
    b32 heroDone;
    i32 minimumValue;
    hero* currentHero;
    i32 stepLimit;
    i32 steps;
    b32 boughtAfterCapture = false;
    b32 savedShow[3];
    mapCell* eventCell;
    i32 pathIndex;
    i32 unusedVars [[maybe_unused]][7];
    i32 moveFlagB;
    i32 moveFlagA;
    town* townPointer;
    i32 townId;
    MapDirection direction;
    i32 targetValue;
    MapDirection specialDirection;

    LogInt("DO AI 1", player);
    PollSound();
    if (gbGameOver != 0)
        goto aiCleanup;
    if (giLimitPlayer != 0) {
        if (player != giLimitPlayer)
            goto aiCleanup;
    }
    LogInt("DO AI", player);
    InitAIMapVars();
    GetTurnAIVars(player);
    if (gpGame->m_day == 1 || gpGame->m_day == 1) {
        for (pathIndex = 0; pathIndex < gpCurPlayer->m_heroCount; pathIndex++) {
            GetHeroSlot(gpCurPlayer->m_heroIds[pathIndex])->m_lastTownInteractionTurn =
                HERO_INTERACTION_TURN_NONE;
        }
    }
    ShowStatus();
    SetupRelativeHeroStrengths();
    for (pathIndex = 0; pathIndex < H2EnumIndex(RES_COUNT); pathIndex++) {
        sprintf(
            gText,
            "RES - %15s  %d  %d",
            gResourceNames[pathIndex],
            gpCurPlayer->m_resources[pathIndex],
            gpCurPlayer->m_aiData.m_income[pathIndex]
        );
        LogStr(gText);
    }
    CheckBuyStuff();
    IncrementHourGlass();

    while ((currentHero = DetermineHeroToMove(player)) != NULL) {
        ValidateHero(currentHero);
        gpCurAIHero = currentHero;
        if (gpCurAIHero->m_patrolX != HERO_PATROL_NONE && gpCurAIHero->m_patrolRadius == 0) {
            gpCurAIHero->m_remainingMobility = 0;
            continue;
        }

        giCurAIHeroMorale = gpCurAIHero->m_army.GetMorale(gpCurAIHero, NULL, NULL);
        giCurAIHeroLuck = gpGame->GetLuck(gpCurAIHero, NULL, NULL);
        gpCurAIHero->m_army.GetMorale(gpCurAIHero, NULL, NULL);
        giHumanTownConquered = -1;
        iCurPlaceToVisit = 0;
        if (gbGameOver != 0)
            goto aiCleanup;

        LogStr("\n\n\n\n");
        LogStr("===================================");
        LogInt("Player with HeroTOMOVE", player);
        LogStr(gpCurAIHero->m_name);
        LogStr("\n");
        CheckReload();
        CheckBerserk();
        giShowComputerRoute = false;
        if (gConfig.blackoutComputer == 0 && gbRemoteOn == 0
            && MapExtraPosAndAdjacentsSet(
                gpCurAIHero->m_x,
                gpCurAIHero->m_y,
                giCurWatchPlayerBit
            )) {
            bShowIt = true;
            gpAdvManager->SetHeroContext(gpCurAIHero->m_id, 0);
        } else {
            bShowIt = false;
            gpAdvManager->SetHeroContext(gpCurAIHero->m_id, 0);
        }

        heroDone = false;
        ResetHeroRVs(0, 0, 0);
        stepLimit = gpCurAIHero->IsEmbarked() ? 15 : 5;
        minimumValue = gpCurAIHero->m_mobility + 800;
        stepLimit =
            static_cast<i32>(stepLimit * (1.7 - H2EnumIndex(gpGame->m_difficulty) * 0.1));
        minimumValue = static_cast<i32>(
            minimumValue * ((H2EnumIndex(gpGame->m_difficulty) - 1) * 0.06 + 0.8)
        );

        while (!heroDone && gpCurAIHero->m_remainingMobility >= AI_MINIMUM_SITE_MOBILITY) {
            if (!(gbGameOver == 0)) {
                goto aiCleanup;
            } else {
                if (gpCurAIHero->m_remainingMobility == gpCurAIHero->m_mobility
                    && gpCurPlayer->m_ultimateArtifactHintChance > 15
                    && gpCurPlayer->m_ultimateArtifactHintX == gpCurAIHero->m_x
                    && gpCurPlayer->m_ultimateArtifactHintY == gpCurAIHero->m_y) {
                    gpAdvManager->ProcessSearch(gpCurAIHero->m_x, gpCurAIHero->m_y);
                }

            aiRetarget:
                targetValue = DetermineTargetPosition(
                    gpCurAIHero->m_destinationX,
                    gpCurAIHero->m_destinationY,
                    minimumValue,
                    specialDirection
                );
                townId = gpGame->GetTownId(gpCurAIHero->m_x, gpCurAIHero->m_y);
                if (townId != -1) {
                    townPointer = GetCastleSlot(townId);
                    if (townPointer->m_threat != 0
                        && (gpAdvManager
                                ->GetCell(
                                    gpCurAIHero->m_destinationX,
                                    gpCurAIHero->m_destinationY
                                )
                                ->m_triggerType
                            & MAP_TRIGGER_TYPE_MASK)
                               != MAP_OBJECT_HERO_INTERACTION) {
                        gpCurAIHero->m_remainingMobility = 0;
                    }
                }
                if (specialDirection != MAP_DIRECTION_NONE) {
                    direction = specialDirection;
                    steps = 99;
                    goto aiMoveDirection;
                }
                if (targetValue < 1000 && specialDirection == MAP_DIRECTION_NONE
                    && DoAnywhereDDoorTownGate(targetValue))
                    goto aiRetarget;

                if (targetValue < 75 && gpGame->m_day == 7
                    && gpCurAIHero->m_locationType
                           == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))) {
                    gpCurAIHero->m_remainingMobility = 0;
                }
                for (pathIndex = 0; pathIndex < iCurPlaceToVisit; pathIndex++) {
                    if (iPlacesVisited[pathIndex][0] == gpCurAIHero->m_destinationX
                        && iPlacesVisited[pathIndex][1] == gpCurAIHero->m_destinationY
                        && gpAdvManager
                                   ->GetCell(
                                       gpCurAIHero->m_destinationX,
                                       gpCurAIHero->m_destinationY
                                   )
                                   ->m_triggerType
                               != (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))) {
                        gpCurAIHero->m_remainingMobility = 0;
                    }
                }
                if (iCurPlaceToVisit < ADVMGR_PLACE_VISIT_COUNT) {
                    iPlacesVisited[iCurPlaceToVisit][0] = gpCurAIHero->m_x;
                    iPlacesVisited[iCurPlaceToVisit][1] = gpCurAIHero->m_y;
                    iCurPlaceToVisit++;
                }
                giShowComputerRoute = true;
                if (gpCurAIHero->m_mobility == gpCurAIHero->m_remainingMobility) {
                    hourglassShown = false;
                    IncrementHourGlass();
                }
                if (gpCurAIHero->m_destinationX != -1 && gpCurAIHero->m_destinationY != -1) {
                    eventCell = NULL;
                    gpAdvManager->SetHeroContext(gpCurAIHero->m_id, 0);
                    gpSearchArray->BuildPath(
                        gpCurAIHero->m_x,
                        gpCurAIHero->m_y,
                        gpCurAIHero->m_destinationX,
                        gpCurAIHero->m_destinationY,
                        gpCurAIHero->m_remainingMobility
                    );
                    if (gpSearchArray->m_pathLength == 0) {
                        heroDone = true;
                    } else if (gpSearchArray->m_pathLength > 0) {
                        gpAdvManager->UpdateScreen(0, 0);
                        if (targetValue > 1000 && gpCurAIHero->HasSpell(SPELL_DIMENSION_DOOR)
                            && gpCurAIHero->m_spellPoints >= AI_DIMENSION_DOOR_SPELL_POINTS
                            && DoDimensionDoor(gpCurAIHero))
                            goto aiRetarget;

                        steps = 0;
                        pathIndex = gpSearchArray->m_pathLength - 1;
                        moveFlagA = 0;
                        moveFlagB = 0;
                        stopAfterStep = true;
                        while (pathIndex >= 0 && steps < stepLimit) {
                            stopAfterStep =
                                (steps + 1 == stepLimit || pathIndex == 0) ? 1 : 0;
                            if (pathIndex > 0) {
                                moveFlagA = 0;
                                moveFlagB = 0;
                                if (GoodAdjacent(&adjacentDirection)) {
                                    gpSearchArray->m_storage.directions[pathIndex] =
                                        static_cast<i8>(adjacentDirection);
                                    stopAfterStep = true;
                                }
                            }
                            direction = static_cast<MapDirection>(
                                gpSearchArray->m_storage.directions[pathIndex]
                            );
                        aiMoveDirection:
                            if (gpAdvManager->GetMoveShowIt(gpCurAIHero, direction)) {
                                savedShow[0] = bShowIt;
                                bShowIt = true;
                                gpMouseManager->HideColorPointer();
                                hiddenPointers++;
                                bShowIt = savedShow[0];
                            }
                            eventCell = gpAdvManager->MoveHero(
                                direction,
                                stopAfterStep,
                                &eventX,
                                &eventY,
                                &moveFlagA,
                                1,
                                &moveFlagB,
                                0
                            );
                            steps++;
                            if (eventCell != NULL || moveFlagA != 0 || moveFlagB != 0) {
                                break;
                            }
                            pathIndex--;
                        }

                        if (gpCurAIHero->m_owner != giCurPlayer) {
                            if (!boughtAfterCapture) {
                                boughtAfterCapture = true;
                                CheckBuyStuff();
                            } else {
                            }
                            goto aiNextHero;
                        }
                        if (gpCurAIHero->m_remainingMobility <= (gpCurAIHero->m_mobility >> 1)
                            && !hourglassShown) {
                            hourglassShown = true;
                            IncrementHourGlass();
                        }
                        if (pathIndex < 0 && gpCurPlayer->m_ultimateArtifactHintChance > 15
                            && gpCurPlayer->m_ultimateArtifactHintX == gpCurAIHero->m_x
                            && gpCurPlayer->m_ultimateArtifactHintY == gpCurAIHero->m_y) {
                            if (gpCurAIHero->m_remainingMobility == gpCurAIHero->m_mobility) {
                                gpAdvManager->ProcessSearch(-1, -1);
                            } else {
                                gpCurAIHero->m_remainingMobility = 0;
                            }
                        }
                        if (pathIndex < 0
                            && (((gpCurAIHero->m_x != gpCurAIHero->m_destinationX
                                  || gpCurAIHero->m_y != gpCurAIHero->m_destinationY)
                                 && eventCell == NULL)
                                || gpCurAIHero->m_remainingMobility < AI_MINIMUM_SITE_MOBILITY
                                || (moveFlagA != 0 && eventCell == NULL))) {
                            heroDone = true;
                        }

                        savedShow[0] = bShowIt;
                        bShowIt = true;
                        while (hiddenPointers != 0) {
                            gpMouseManager->ShowColorPointer();
                            hiddenPointers--;
                        }
                        bShowIt = savedShow[0];
                        gpAdvManager->UpdateRadar(1, 0);
                    } else {
                        heroDone = true;
                    }
                    if (eventCell != NULL) {
                        gpAdvManager->DoAIEvent(eventCell, gpCurAIHero, eventX, eventY);
                        if (gpCurPlayer->m_currentHero == -1) {
                            goto aiNextHero;
                        }
                        ValidateHero(gpCurAIHero);
                        ResetHeroRVs(1, gpCurAIHero->m_destinationX, gpCurAIHero->m_destinationY);
                    }
                } else {
                    heroDone = true;
                }
            }
        }

        gpCurAIHero->m_remainingMobility = 0;
        gpAdvManager->DeactivateCurrHero();
    aiNextHero:
        if (gpCurAIHero->m_locationType == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))) {
            CheckBuyStuff();
        }
    }

aiCleanup:
    gpGame->CheckHeroConsistency();
    gpCurAIHero = NULL;
    CloseAIMapVars();
    while (gpMouseManager->m_hideCount != 0) {
        gpMouseManager->ShowColorPointer();
    }
}

void philAI::GetGameAIVars(void) {
    i32 i;
    for (i = 0; i < gpGame->m_playerCount; i++)
        GetGameAttentionValue(i);
}

void philAI::GetTurnAIVars(i32 player) {
    i32 outcomeE;
    playerData* playerPointer;
    hero* threatHeroPointer;
    i32 ownedTownCount;
    i32 lastFightValue;
    town* townPointer;
    i32 lateTurn;
    i32 mapX;
    float winChance;
    i32 unusedFightValue [[maybe_unused]];
    float fightValueFloat;
    hero* heroPointer;
    mapCell* cell;
    i32 outcomeD;
    i32 outcomeC;
    i32 yCenter;
    i32 outcomeB;
    float townCountValue;
    i32 xCenter;
    i32 outcomeA;
    i32 firstWeekIndex;
    i32 playerIndex;
    i32 mineValue;
    i32 artifactTotal;
    i32 earlyTurn;
    i32 generalIndex;
    i32 innerIndex;
    i32 mapY;

    giCurTurn = GAME_DAY_NUMBER(*gpGame);
    GetTurnAttentionValue(player);
    TurnCostResource(player);
    iCurHourGlassPhase = 0;
    iSandAnim = 0;
    gpCurPlayer->m_aiData.m_obeliskValue = static_cast<i32>(TurnValueOfObelisk(player));
    gpCurPlayer->m_aiData.m_totalObeliskValue =
        gpCurPlayer->m_aiData.m_obeliskValue * gpGame->m_obeliskCount
        / GAME_OBELISK_VISITOR_COUNT;
    gpCurPlayer->m_aiData.m_unexploredValue = MeanRVOfUnexploredTerritory(player);
    bHeroBuiltThisTurn = false;
    if (giCurTurn - giBuildBoatStuffTurn[player] > 8) {
        giBuildShipyard[player] = -1;
        giBuildBoat[player] = -1;
    }

    unusedFightValue = 0;
    fightValueFloat = 0.0;
    if (giCurTurn <= 7) {
        fFirstWeekTownFV = 0.0;
        for (firstWeekIndex = 0; firstWeekIndex < gpCurPlayer->m_townCount; firstWeekIndex++) {
            townPointer = GetCastleSlot(gpCurPlayer->m_townIds[firstWeekIndex]);
            for (innerIndex = 0; innerIndex < GAME_HERO_COUNT; innerIndex++) {
                heroPointer = &gpGame->m_heroRecs[innerIndex];
                if (heroPointer->m_owner < 0 || heroPointer->m_owner > 5
                    || heroPointer->m_owner == giCurPlayer)
                    continue;
                if (MANHATTAN_LENGTH(heroPointer->m_x - townPointer->m_x, heroPointer->m_y - townPointer->m_y)
                    < 16) {
                    fFirstWeekTownFV = 0.3f;
                    goto firstWeekDone;
                }
            }
        }
    }

firstWeekDone:
    lastFightValue = 0;
    for (generalIndex = 0; generalIndex < gpCurPlayer->m_heroCount; generalIndex++) {
        heroPointer = GetHeroSlot(gpCurPlayer->m_heroIds[generalIndex]);
        fightValueFloat = static_cast<float>(FightValueOfStack(&heroPointer->m_army, heroPointer, 0));
        lastFightValue = static_cast<i32>(lastFightValue + fightValueFloat);
        heroPointer->m_aiFightValue =
            static_cast<float>(fightValueFloat * AI_HERO_VALUE_SCALE + AI_HERO_VALUE_BASE);
    }
    for (generalIndex = 0; generalIndex < gpCurPlayer->m_townCount; generalIndex++) {
        townPointer = GetCastleSlot(gpCurPlayer->m_townIds[generalIndex]);
        fightValueFloat = static_cast<float>(FightValueOfStack(&townPointer->m_army, NULL, 0));
        lastFightValue = static_cast<i32>(lastFightValue + fightValueFloat);
    }
    gpCurPlayer->m_aiData.m_upgradeValueWeight =
        static_cast<float>(
            gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)]
            + gpCurPlayer->m_aiData.m_income[H2EnumIndex(RES_GOLD)]
        ) / (lastFightValue + 1000)
        + gpCurPlayer->m_aiData.m_attentionWeights.upgradeBase;

    artifactTotal = 0;
    for (generalIndex = H2EnumIndex(ARTIFACT_ULTIMATE_SHIELD);
         generalIndex < H2EnumIndex(ARTIFACT_GAMBLERS_COIN);
         generalIndex++)
        artifactTotal += gArtifactBaseRV[generalIndex];
    for (generalIndex = 0; generalIndex < gpGame->m_playerCount; generalIndex++) {
        gpGame->m_players[generalIndex].m_aiData.m_artifactPoolShare =
            1.0 / (gpGame->m_playerCount + gpGame->m_deadPlayerCount);
    }
    gpCurPlayer->m_aiData.m_artifactValue =
        static_cast<float>(artifactTotal / AI_ARTIFACT_POOL_DIVISOR);

    for (generalIndex = 0; generalIndex < GAME_TOWN_COUNT; generalIndex++)
        gpGame->m_castleRecs[generalIndex].m_threat = false;
    for (generalIndex = 0; generalIndex < gpCurPlayer->m_townCount; generalIndex++)
        gpGame->m_castleRecs[gpCurPlayer->m_townIds[generalIndex]].m_threat = false;

    memset(gaiEnemyHeroReachable, 0, MAP_WIDTH * MAP_HEIGHT);
    for (playerIndex = 0; playerIndex < gpGame->m_playerCount; playerIndex++) {
        if (playerIndex == giCurPlayer)
            continue;
        if (OnMySide(playerIndex))
            continue;
        for (innerIndex = 0; innerIndex < gpGame->m_players[playerIndex].m_heroCount;
             innerIndex++) {
            threatHeroPointer =
                GetHeroSlot(gpGame->m_players[playerIndex].m_heroIds[innerIndex]);
            gpSearchArray->SeedPosition(
                threatHeroPointer->m_x,
                threatHeroPointer->m_y,
                threatHeroPointer->m_direction,
                threatHeroPointer->m_mobility + 100,
                H2EnumIndex(threatHeroPointer->m_eventFlags) & H2EnumIndex(HERO_EVENT_EMBARKED),
                1,
                threatHeroPointer->m_mobility + 100,
                H2EnumIndex(threatHeroPointer->m_secondarySkills[H2EnumIndex(HERO_SKILL_PATHFINDING)]),
                -1,
                -1,
                0,
                0
            );
            for (mapX = 0; mapX < MAP_WIDTH; mapX++) {
                for (mapY = 0; mapY < MAP_WIDTH; mapY++) {
                    if (gpSearchArray->GetRow(mapX, 1)[MAP_WIDTH * mapY].visited) {
                        town* threatTownPointer;
                        i32 threatTownId;
                        *(gaiEnemyHeroReachable + mapX + mapY * MAP_WIDTH) = 1;
                        if (mapY == 0)
                            continue;
                        cell = gpAdvManager->GetCell(mapX, mapY - 1);
                        if (cell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
                            || (cell->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                                   == MAP_OBJECT_HERO_INTERACTION) {
                            threatTownId = gpGame->GetTownId(mapX, mapY - 1);
                            if (threatTownId == -1)
                                continue;
                            threatTownPointer = GetCastleSlot(threatTownId);
                            if (threatTownPointer->m_owner == giCurPlayer
                                && (H2EnumIndex((threatTownPointer->m_buildings) & (H2EnumIndex(TOWN_BUILDING_CASTLE))))) {
                                ProbableOutcomeOfBattle(
                                    &threatHeroPointer->m_army,
                                    threatHeroPointer,
                                    &GetHeroSlot(threatTownPointer->m_occupyingHeroId)->m_army,
                                    GetHeroSlot(threatTownPointer->m_occupyingHeroId),
                                    &threatTownPointer->m_army,
                                    1,
                                    threatTownPointer->m_id,
                                    threatTownPointer->m_owner,
                                    winChance,
                                    outcomeA,
                                    outcomeB,
                                    outcomeC,
                                    outcomeD,
                                    outcomeE
                                );
                                if (winChance > AI_TOWN_DEFENSE_THRESHOLD)
                                    threatTownPointer->m_threat = true;
                            }
                        }
                    }
                }
            }
        }
    }

    memset(gaiTurnValueOfMine, 7, MAP_WIDTH * MAP_HEIGHT);
    for (playerIndex = 0; playerIndex < gpGame->m_playerCount; playerIndex++) {
        if (playerIndex != giCurPlayer) {
            playerPointer = &gpGame->m_players[playerIndex];
            for (innerIndex = 0; innerIndex < playerPointer->m_heroCount; innerIndex++) {
                xCenter = GetHeroSlot(playerPointer->m_heroIds[innerIndex])->m_x;
                yCenter = GetHeroSlot(playerPointer->m_heroIds[innerIndex])->m_y;
                for (mapX = xCenter - 10; mapX <= xCenter + 10; mapX++) {
                    for (mapY = yCenter - 10; mapY <= yCenter + 10; mapY++) {
                        if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT) {
                            mineValue =
                                abs(MANHATTAN_LENGTH(mapX - xCenter, mapY - yCenter) - 4) >> 2;
                            if (mineValue < *(gaiTurnValueOfMine + mapX + mapY * MAP_WIDTH))
                                *(gaiTurnValueOfMine + mapX + mapY * MAP_WIDTH) =
                                    static_cast<i8>(mineValue);
                        }
                    }
                }
            }
        }
    }

    for (generalIndex = 0; generalIndex < GAME_HERO_COUNT; generalIndex++)
        gfHeroInteractionBonus[generalIndex] = 1.0f;
    if (gpGame->m_difficulty == DIFFICULTY_EASY) {
        gfAttackHumanBonus = 0.6f;
        gfAttackComputerBonus = 1.3f;
    } else if (gpGame->m_difficulty == DIFFICULTY_NORMAL) {
        gfAttackHumanBonus = 1.0f;
        gfAttackComputerBonus = 1.0f;
    } else {
        gfAttackHumanBonus = static_cast<float>(H2EnumIndex(gpGame->m_difficulty) * 0.07 + 1.0);
        gfAttackComputerBonus = static_cast<float>(1.1 - H2EnumIndex(gpGame->m_difficulty) * 0.12);
    }
    if (gbIAmGreatest)
        gfAttackComputerBonus = 0.1f;

    townCountValue = 0.1f;
    for (generalIndex = 0; generalIndex < gpCurPlayer->m_townCount; generalIndex++) {
        if ((H2EnumIndex((GetCastleSlot(gpCurPlayer->m_townIds[generalIndex])->m_buildings) & (H2EnumIndex(TOWN_BUILDING_CASTLE)))))
            townCountValue += 1.0;
        else
            townCountValue += 0.5;
    }
    ownedTownCount = static_cast<i32>(townCountValue);
    giMaxHeroesForThisPlayer = 3;
    if (gpGame->m_playerCount - gpGame->m_deadPlayerCount == 2)
        giMaxHeroesForThisPlayer++;
    if (gpGame->m_playerCount - gpGame->m_deadPlayerCount == 3)
        giMaxHeroesForThisPlayer++;
    if (ownedTownCount >= 5)
        giMaxHeroesForThisPlayer++;
    if (ownedTownCount >= 10)
        giMaxHeroesForThisPlayer++;
    if (gpGame->m_mapHeader.width == MAP_DIMENSION_SMALL)
        giMaxHeroesForThisPlayer--;
    if (gpGame->m_mapHeader.width >= MAP_DIMENSION_LARGE)
        giMaxHeroesForThisPlayer++;

    earlyTurn = 16;
    lateTurn = 22;
    if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_EXPLORER) {
        earlyTurn = 8;
        lateTurn = 15;
    }
    if (giCurTurn < earlyTurn && ownedTownCount == 1 && gpCurPlayer->m_heroCount == 1) {
        town* earlyTownPointer;
        hero* earlyHeroPointer;
        earlyHeroPointer = GetHeroSlot(gpCurPlayer->m_heroIds[0]);
        earlyTownPointer = GetCastleSlot(gpCurPlayer->m_townIds[0]);
        if (MANHATTAN_LENGTH(
                earlyTownPointer->m_x - earlyHeroPointer->m_x,
                earlyTownPointer->m_y - earlyHeroPointer->m_y
            )
            < 18)
            giMaxHeroesForThisPlayer = 1;
    }
    if (giCurTurn < lateTurn && giMaxHeroesForThisPlayer > 2) {
        giMaxHeroesForThisPlayer = 2;
    }
    if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_EXPLORER)
        ownedTownCount++;
    if (giMaxHeroesForThisPlayer > ownedTownCount + 1)
        giMaxHeroesForThisPlayer = ownedTownCount + 1;
    if (giMaxHeroesForThisPlayer < gpCurPlayer->m_minimumHeroCount)
        giMaxHeroesForThisPlayer = gpCurPlayer->m_minimumHeroCount;
}

void philAI::GetBestBHC(i32 player [[maybe_unused]], BHC& best) {
    float bhcValue = 1.0f;
    float topVal = -99.0f;
    i32 totalStrength = 0;
    i32 totalWeight = 0;
    BHC choice;
    i32 idealStrength[H2EnumIndex(GAME_TOWN_COUNT)];
    town* curTown;
    i32 avgStrength;
    i32 townStrength[H2EnumIndex(GAME_TOWN_COUNT)];
    i32 townNo;

    for (townNo = 0; townNo < gpCurPlayer->m_townCount; townNo++) {
        curTown = &gpGame->m_castleRecs[gpCurPlayer->m_townIds[townNo]];
        townStrength[townNo] = FightValueOfStack(&curTown->m_army, NULL, 0) + 400;
        totalStrength += townStrength[townNo];
        if ((H2EnumIndex((curTown->m_buildings) & (H2EnumIndex(TOWN_BUILDING_CASTLE)))))
            totalWeight += 10;
        else
            totalWeight += 7;
    }
    if (totalWeight < 1)
        totalWeight = 1;
    avgStrength = totalStrength / totalWeight;
    for (townNo = 0; townNo < gpCurPlayer->m_townCount; townNo++) {
        curTown = &gpGame->m_castleRecs[gpCurPlayer->m_townIds[townNo]];
        idealStrength[townNo] =
            avgStrength * ((H2EnumIndex((curTown->m_buildings) & (H2EnumIndex(TOWN_BUILDING_CASTLE)))) ? 10 : 7) + 400;
    }

    for (townNo = 0; townNo < gpCurPlayer->m_townCount; townNo++) {
        curTown = &gpGame->m_castleRecs[gpCurPlayer->m_townIds[townNo]];
        LogInt("Turns Owned", curTown->m_turnsOwned);
        if (giCurTurn > 3 && curTown->m_turnsOwned < 3)
            continue;
        {
            CheckDoMain(0, 0);
            GetBestBuilding(curTown, choice, bhcValue);
            bhcValue = static_cast<float>(
                bhcValue * ((100 - Random(0, 10)) / AI_PURCHASE_RANDOM_DIVISOR)
            );
            if (bhcValue > topVal) {
                topVal = bhcValue;
                best = choice;
            }

            CheckDoMain(0, 0);
            GetBestCreature(curTown, choice, bhcValue);
            bhcValue = static_cast<float>(
                bhcValue
                * (static_cast<float>(idealStrength[townNo])
                       / (static_cast<float>(townStrength[townNo])) / 3.0f
                   + AI_CREATURE_BALANCE_BASE)
            );
            bhcValue = static_cast<float>(
                bhcValue * ((100 - Random(0, 10)) / AI_PURCHASE_RANDOM_DIVISOR)
            );
            if (bhcValue > topVal) {
                topVal = bhcValue;
                best = choice;
            }

            CheckDoMain(0, 0);
            if (gpCurPlayer->m_heroCount < giMaxHeroesForThisPlayer
                && (H2EnumIndex((curTown->m_buildings) & (H2EnumIndex(TOWN_BUILDING_CASTLE))))) {
                GetBestHero(curTown, choice, bhcValue);
                bhcValue = static_cast<float>(
                    bhcValue * ((100 - Random(0, 10)) / AI_PURCHASE_RANDOM_DIVISOR)
                );
                if (!bHeroBuiltThisTurn && giCurTurn > 5 && bhcValue > 0.0f) {
                    if (!((gpCurPlayer->m_aiData.m_income[H2EnumIndex(RES_GOLD)] < 1250
                           || gpCurPlayer->m_heroCount >= giMaxHeroesForThisPlayer - 2)
                          && gpCurPlayer->m_heroCount > 1)) {
                        bhcValue += AI_HERO_PURCHASE_BONUS;
                    } else if (gpCurPlayer->m_aiData.m_income[H2EnumIndex(RES_GOLD)] >= 1500
                               && gpCurPlayer->m_heroCount < giMaxHeroesForThisPlayer - 1) {
                        bhcValue = static_cast<float>(bhcValue * AI_HERO_PURCHASE_FACTOR);
                    }
                } else if (gpCurPlayer->m_heroCount == 0) {
                    bhcValue += AI_HERO_PURCHASE_BONUS;
                }
                if (bhcValue > topVal) {
                    topVal = bhcValue;
                    best = choice;
                }
            }
        }
    }
    LogInt("BestBHC ", H2EnumIndex(best.type), static_cast<i32>(topVal * 100.0f), best.what, 0, 0);
    if (topVal < AI_MINIMUM_PURCHASE_VALUE)
        best.type = PURCHASE_NONE;
}

hero* philAI::DetermineHeroToMove(i32 player) {
    i32 value;
    i32 bestMobility;
    i32 best;
    i32 i;
    bestMobility = 0;
    best = -1;
    if (gpCurPlayer->HasMobileHero()) {
        for (i = 0; i < gpCurPlayer->m_heroCount; i++) {
            value = gpGame->m_heroRecs[gpGame->m_players[player].m_heroIds[i]]
                          .m_remainingMobility;
            if (value > bestMobility) {
                bestMobility = value;
                best = i;
            }
        }
    }
    if (best >= 0)
        return &gpGame->m_heroRecs[gpGame->m_players[player].m_heroIds[best]];
    gpGame->m_players[player].m_currentHero = -1;
    return NULL;
}

i32 philAI::DetermineTargetPosition(
    i32& targetX,
    i32& targetY,
    i32 mobility,
    MapDirection& direction
) {
    town* pPort;
    i32 bestRV;
    mapCell* cell;
    i32 columnCount;
    H2EnumStorage<TerrainType, i32> ground;
    i32 posValue;
    MapDirection goodDirection;
    i32 boatDist;
    i32 heroNum;
    i32 pass;
    i32 y;
    i32 chosenY;
    i32 restMobility;
    i32 x;
    i32 chosenX;
    i32 bottomY;
    i32 spread;
    i32 rightX;
    i32 topY;
    b32 good;
    i32 rowCounter;
    i32 dblMob;
    i32 currentDistance;
    i32 leftX;

    if (GoodAdjacent(&goodDirection)) {
        targetX = gpCurAIHero->m_x + normalDirTable[H2EnumIndex(goodDirection)].x;
        targetY = gpCurAIHero->m_y + normalDirTable[H2EnumIndex(goodDirection)].y;
        direction = goodDirection;
        return 1000;
    }

    direction = MAP_DIRECTION_NONE;
    chosenX = -1;
    chosenY = -1;
    bestRV = -999999;
    giBestShipyardId = -1;
    gbPossibleShipyardFound = false;
    gbActualShipyardFound = false;
    gbActualBoatFound = false;

    cell = gpAdvManager->GetCell(gpCurAIHero->m_x, gpCurAIHero->m_y);
    ground = CELL_TERRAIN(cell);
    if (gpCurAIHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_PATHFINDING)]
        <= HERO_SKILL_LEVEL_BASIC) {
        if (ground == TERRAIN_SNOW || ground == TERRAIN_SWAMP)
            mobility = static_cast<i32>(mobility * 1.15);
        if (ground == TERRAIN_DESERT)
            mobility = static_cast<i32>(mobility * AI_PATHFINDING_DESERT_MOBILITY_FACTOR);
        if (ground == TERRAIN_WASTELAND)
            mobility = static_cast<i32>(mobility * 1.25);
    } else if (gpGame->m_mapHeader.width == MAP_DIMENSION_SMALL) {
        mobility = static_cast<i32>(mobility * 0.9);
    } else if (gpGame->m_mapHeader.width == MAP_DIMENSION_LARGE) {
        mobility = static_cast<i32>(mobility * AI_LARGE_MAP_MOBILITY_FACTOR);
    } else if (gpGame->m_mapHeader.width == MAP_DIMENSION_XLARGE) {
        mobility = static_cast<i32>(mobility * 1.1);
    }
    if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_EXPLORER)
        mobility = static_cast<i32>(mobility * 1.15);

    spread = mobility / 100 / 4;
    if (spread < 6)
        spread = 6;
    dblMob = mobility * 2;
    gpSearchArray->SeedPosition(
        gpCurAIHero->m_x,
        gpCurAIHero->m_y,
        gpCurAIHero->m_direction,
        dblMob,
        H2EnumIndex(gpCurAIHero->m_eventFlags) & H2EnumIndex(HERO_EVENT_EMBARKED),
        1,
        gpCurAIHero->m_remainingMobility,
        H2EnumIndex(gpCurAIHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_PATHFINDING)]),
        -1,
        -1,
        0,
        0
    );
    gpSearchArray->GetNode(gpCurAIHero->m_x, gpCurAIHero->m_y).visited = 0;

    columnCount = -1;
    leftX = gpCurAIHero->m_x - 25;
    if (leftX < 0)
        leftX = 0;
    topY = gpCurAIHero->m_y - 25;
    if (topY < 0)
        topY = 0;
    rightX = gpCurAIHero->m_x + 25;
    if (rightX > MAP_WIDTH - 1)
        rightX = MAP_WIDTH;
    bottomY = gpCurAIHero->m_y + 25;
    if (bottomY > MAP_HEIGHT - 1)
        bottomY = MAP_HEIGHT;

    for (pass = 0; pass < 2; pass++) {
        if (gpCurAIHero->IsEmbarked()) {
            if (pass == 0)
                continue;
        }
        for (x = leftX; x < rightX; x++) {
            rowCounter = -1;
            columnCount++;
            if (columnCount >= spread)
                columnCount = 0;
            for (y = topY; y < bottomY; y++) {
                rowCounter++;
                if (rowCounter >= spread)
                    rowCounter = 0;
                if (gpSearchArray->GetNode(x, y).visited) {
                    if (gpCurAIHero->IsEmbarked())
                        goto position_reachable;
                    currentDistance = MANHATTAN_LENGTH(x - gpCurAIHero->m_x, y - gpCurAIHero->m_y);
                    if ((pass == 0 && currentDistance > 5
                         && (x != gpCurAIHero->m_destinationX
                             || y != gpCurAIHero->m_destinationY)
                         && (currentDistance >= 16
                             || (gpAdvManager->GetCell(x, y)->m_triggerType
                                     != (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))
                                 && (gpAdvManager->GetCell(x, y)->m_triggerType
                                         != (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
                                     || GetCastleSlot(gpAdvManager->GetCell(x, y)
                                                          ->m_objectMetadata)
                                                ->m_owner
                                            == giCurPlayer))))
                        || (pass == 1
                            && (currentDistance <= 5
                                || (x == gpCurAIHero->m_destinationX
                                    && y == gpCurAIHero->m_destinationY)
                                || (currentDistance < 16
                                    && (gpAdvManager->GetCell(x, y)->m_triggerType
                                            == (MAP_TRIGGER_ACTION_FLAG
                                                | MAP_OBJECT_HERO_INTERACTION)
                                        || (gpAdvManager->GetCell(x, y)->m_triggerType
                                                == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
                                            && GetCastleSlot(gpAdvManager->GetCell(x, y)
                                                                 ->m_objectMetadata)
                                                       ->m_owner
                                                   != giCurPlayer))))))
                        continue;

                position_reachable:
                    cell = gpAdvManager->GetCell(x, y);
                    if (gpSearchArray->GetNode(x, y).distance > mobility) {
                        if (gpSearchArray->GetNode(x, y).distance > dblMob) {
                            good = false;
                        } else {
                            good =
                                (cell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
                                 || cell->m_triggerType
                                        == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                                 || (cell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_BOAT))
                                     && !gpCurAIHero->IsEmbarked()));
                        }
                    } else {
                        good =
                            ((H2EnumIndex((cell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))
                             || (cell->m_triggerType == MAP_OBJECT_COAST
                                 && gpCurAIHero->IsEmbarked())
                             || (x % spread == 0 && y % spread == 0
                                 && ((gpCurAIHero->IsEmbarked()
                                      && CELL_TERRAIN(cell) == TERRAIN_WATER)
                                     || (!gpCurAIHero->IsEmbarked()
                                         && CELL_TERRAIN(cell) != TERRAIN_WATER)))
                             || (x == gpCurPlayer->m_ultimateArtifactHintX
                                 && y == gpCurPlayer->m_ultimateArtifactHintY));
                    }

                    if (good && gpCurAIHero->m_patrolX != HERO_PATROL_NONE) {
                        boatDist = MANHATTAN_LENGTH(
                            x - gpCurAIHero->m_patrolX,
                            y - gpCurAIHero->m_patrolY
                        );
                        if (boatDist > gpCurAIHero->m_patrolRadius)
                            good = false;
                    }
                    if (good) {
                        for (heroNum = 0; heroNum < gpCurPlayer->m_heroCount;
                             heroNum++) {
                            if (cell->m_triggerType
                                    != (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
                                && cell->m_triggerType
                                       != (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))
                                && gpCurPlayer->m_heroIds[heroNum] != gpCurAIHero->m_id
                                && gpGame->m_heroRecs[gpCurPlayer->m_heroIds[heroNum]]
                                           .m_destinationX
                                       == x
                                && gpGame->m_heroRecs[gpCurPlayer->m_heroIds[heroNum]]
                                           .m_destinationY
                                       == y) {
                                posValue = -2000;
                                goto candidate_scored;
                            }
                        }
                        CheckDoMain(0, 0);
                        restMobility =
                            gpCurAIHero->m_mobility - gpSearchArray->GetNode(x, y).distance;
                        if (restMobility < 0)
                            restMobility = 0;
                        posValue = RVOfPosition(
                            x,
                            y,
                            gpSearchArray->GetNode(x, y).rvFlag1,
                            gpSearchArray->GetNode(x, y).adjacentMonsterX,
                            gpSearchArray->GetNode(x, y).adjacentMonsterY,
                            gpSearchArray->GetNode(x, y).rvFlag2,
                            gpSearchArray->GetNode(x, y).previousFlags,
                            gpSearchArray->GetNode(x, y).terrain,
                            2,
                            restMobility
                        );
                        posValue = posValue * (Random(1, 50) + 75);
                        posValue /= 100;
                    } else {
                        posValue = -100;
                    }
                    if (x == targetX && y == targetY) {
                        posValue = static_cast<i32>(
                            posValue * AI_TARGET_HUMAN_VALUE_FACTOR);
                        posValue += 20;
                    }

                candidate_scored:
                    if (posValue > bestRV) {
                        chosenX = x;
                        chosenY = y;
                        bestRV = posValue;
                    } else if (posValue == bestRV && posValue == 0
                               && !(H2EnumIndex((gpAdvManager->GetCell(x, y)->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                        if ((H2EnumIndex((gpAdvManager->GetCell(chosenX, chosenY)->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))
                            || MANHATTAN_LENGTH(x - gpCurAIHero->m_x, y - gpCurAIHero->m_y)
                                   > MANHATTAN_LENGTH(
                                       chosenX - gpCurAIHero->m_x,
                                       chosenY - gpCurAIHero->m_y
                                   )) {
                            chosenX = x;
                            chosenY = y;
                        }
                    }
                }
            }
        }
        if (pass == 0 && bestRV > 150)
            break;
    }

    if (bestRV < 75 && (gbPossibleShipyardFound || gbActualShipyardFound) && !gbActualBoatFound
        && giCurTurn > 3) {
        if ((gbActualShipyardFound || giBuildShipyard[giCurPlayer] < 0
             || giBuildShipyard[giCurPlayer] == giBestShipyardId)
            && (gpCurPlayer->m_resources[0] + gpCurPlayer->m_aiData.m_income[0] * 6
                >= (!gbActualShipyardFound ? 20 : 0) + 10)) {
            if (!gbActualShipyardFound)
                giBuildShipyard[giCurPlayer] = static_cast<i8>(giBestShipyardId);
            giBuildBoat[giCurPlayer] = static_cast<i8>(giBestShipyardId);
            giBuildBoatStuffTurn[giCurPlayer] = static_cast<i8>(giCurTurn);
            pPort = GetCastleSlot(giBestShipyardId);
            bestRV = 123;
            chosenX = pPort->m_x;
            chosenY = pPort->m_y;
            if (gpCurAIHero->m_x == chosenX && gpCurAIHero->m_y == chosenY)
                gpCurAIHero->m_remainingMobility = 0;
        }
        CheckBuyStuff();
    }

    targetX = chosenX;
    targetY = chosenY;
    if (gpCurAIHero->m_patrolX != HERO_PATROL_NONE && bestRV <= 0) {
        targetX = gpCurAIHero->m_patrolX;
        targetY = gpCurAIHero->m_patrolY;
    }
    LogInt(
        "Hero, Best RV target XY  current XY",
        gpCurAIHero->m_owner,
        bestRV,
        targetX,
        targetY,
        gpCurAIHero->m_x,
        gpCurAIHero->m_y
    );
    LogStr("\n\n****");
    return bestRV;
}

void philAI::ProbableOutcomeOfBattle(
    armyGroup* attacker,
    hero* attackerHero,
    armyGroup* defender,
    hero* defenderHero,
    armyGroup* townArmy,
    i32 useTown,
    i32 townId,
    i32 enemyPlayer,
    float& winChance,
    i32& attackerLoss,
    i32& defenderLoss,
    i32& attackerRemaining,
    i32& defenderRemaining,
    i32& outcomeValue
) {
    float attackerRaw;
    float defRaw;
    float attStr;
    float powCurve;
    float defP;
    float difficulty;
    i32 j;
    float attackerPower;
    i32 notUsed [[maybe_unused]];
    float defStrong;
    i32 aArt = 0;
    i32 experience;
    float attFight;
    float defenderFight;
    i32 artsD = 0;

    attFight =
        static_cast<float>(FightValueOfStack(attacker, attackerHero, 1, 0, 0, useTown));
    defenderFight =
        static_cast<float>(FightValueOfStack(defender, defenderHero, 1, useTown, townId, 0));
    if (townArmy != NULL)
        defenderFight += static_cast<float>(FightValueOfStack(townArmy, NULL, 1));

    attackerRaw = static_cast<float>(FightValueOfStack(attacker, attackerHero, 0));
    defRaw = static_cast<float>(FightValueOfStack(defender, defenderHero, 0));
    if (townArmy != NULL)
        defRaw += static_cast<float>(FightValueOfStack(townArmy, NULL, 0));

    if (useTown != 0)
        defenderFight =
            static_cast<float>(defenderFight * AI_BATTLE_TOWN_DEFENDER_FACTOR);

    defStrong = defenderFight;
    if (enemyPlayer == AI_BATTLE_NO_PLAYER) {
        attStr = static_cast<float>(
            attFight
            * (H2EnumIndex(gpGame->m_difficulty) * AI_BATTLE_DIFFICULTY_STEP
               + AI_BATTLE_BASE_STRENGTH_FACTOR)
        );
        if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_WARRIOR)
            attStr =
                static_cast<float>(attStr * AI_BATTLE_EASY_STRENGTH_FACTOR);
    } else {
        attStr = attFight;
        if (gbHumanPlayer[enemyPlayer] != 0) {
            defStrong =
                static_cast<float>(defStrong * AI_BATTLE_HUMAN_DEFENDER_FACTOR);
        } else if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_WARRIOR) {
            attStr =
                static_cast<float>(attStr * AI_BATTLE_EASY_STRENGTH_FACTOR);
        }
    }

    if (attStr < AI_BATTLE_MINIMUM_STRENGTH)
        attStr = AI_BATTLE_MINIMUM_STRENGTH;
    if (defStrong < AI_BATTLE_MINIMUM_STRENGTH)
        defStrong = AI_BATTLE_MINIMUM_STRENGTH;

    powCurve = AI_BATTLE_NORMAL_POWER;
    if (attStr > AI_BATTLE_LARGE_STRENGTH || defStrong > AI_BATTLE_LARGE_STRENGTH)
        powCurve = AI_BATTLE_LARGE_POWER;

    attackerPower = static_cast<float>(
        pow(static_cast<double>(attStr), static_cast<double>(powCurve))
    );
    defP = static_cast<float>(
        pow(static_cast<double>(defStrong), static_cast<double>(powCurve))
    );
    winChance = attackerPower / (attackerPower + defP);

    if (winChance < AI_BATTLE_ZERO_CHANCE) {
        winChance = 0.0f;
    } else if (winChance < AI_BATTLE_LOW_CHANCE) {
        winChance = static_cast<float>(winChance - AI_BATTLE_LOW_PENALTY);
    } else if (winChance < AI_BATTLE_MEDIUM_CHANCE) {
        winChance = static_cast<float>(winChance - AI_BATTLE_MEDIUM_PENALTY);
    } else if (winChance < AI_BATTLE_HIGH_CHANCE) {
        winChance = static_cast<float>(winChance - AI_BATTLE_HIGH_PENALTY);
    } else if (winChance < AI_BATTLE_TOP_CHANCE) {
        winChance = static_cast<float>(winChance - AI_BATTLE_TOP_PENALTY);
    }

    attackerLoss = static_cast<i32>((1.0 - winChance) * attackerRaw);
    defenderLoss = static_cast<i32>(defRaw * winChance);
    attackerRemaining =
        static_cast<i32>(attackerLoss * winChance + (1.0f - winChance) * attackerRaw);
    defenderRemaining =
        static_cast<i32>(defenderLoss * (1.0f - winChance) + defRaw * winChance);

    difficulty = static_cast<float>(
        AI_BATTLE_LOSS_FACTOR_BASE - gpCurPlayer->m_aiData.m_attentionWeights.upgradeBase
    );
    outcomeValue = static_cast<i32>(-attackerRemaining * difficulty * difficulty);
    if (enemyPlayer >= 0) {
        difficulty = static_cast<float>(
            gpCurPlayer->m_aiData.m_attentionWeights.upgradeBase + AI_BATTLE_PLAYER_FACTOR_BASE
        );
        if (gbHumanPlayer[enemyPlayer] != 0)
            outcomeValue = static_cast<i32>(
                outcomeValue + defenderRemaining * gfAttackHumanBonus * difficulty * difficulty
            );
        else
            outcomeValue = static_cast<i32>(
                outcomeValue + defenderRemaining * gfAttackComputerBonus * difficulty * difficulty
            );
    }
    outcomeValue = static_cast<i32>(outcomeValue * gpCurPlayer->m_aiData.m_upgradeValueWeight);

    if (attackerHero != NULL) {
        for (j = 0; j < HERO_ARTIFACT_SLOT_COUNT;
             j++) {
            if (attackerHero->m_artifacts[j] >= ARTIFACT_ULTIMATE_BOOK
                && H2EnumIndex(attackerHero->m_artifacts[j])
                       < AI_BATTLE_BASE_ARTIFACT_LIMIT)
                aArt +=
                    gArtifactBaseRV[H2EnumIndex(attackerHero->m_artifacts[j])];
        }
        if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO
            && gpGame->m_mapHeader.victoryConditionValue == attackerHero->m_id)
            aArt += AI_BATTLE_SPECIAL_ARTIFACT_VALUE;

        outcomeValue = static_cast<i32>(
            outcomeValue
            - (aArt + AI_BATTLE_ATTACKER_ARTIFACT_BASE)
                  * (AI_BATTLE_FULL_CHANCE - winChance)
        );
        experience = gpGame->ExperienceValueOfStack(defender, defenderHero);
        outcomeValue = static_cast<i32>(
            outcomeValue
            + experience * AI_BATTLE_HERO_EXPERIENCE_FACTOR * winChance
                  * attackerHero->m_aiFightValue
        );
    }

    if (defenderHero != NULL) {
        for (j = 0; j < HERO_ARTIFACT_SLOT_COUNT;
             j++) {
            if (defenderHero->m_artifacts[j] >= ARTIFACT_ULTIMATE_BOOK
                && H2EnumIndex(defenderHero->m_artifacts[j])
                       < AI_BATTLE_BASE_ARTIFACT_LIMIT)
                artsD +=
                    gArtifactBaseRV[H2EnumIndex(defenderHero->m_artifacts[j])];
        }
        if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_HERO
            && gpGame->m_mapHeader.lossConditionValue == defenderHero->m_id)
            artsD += AI_BATTLE_SPECIAL_ARTIFACT_VALUE;

        outcomeValue = static_cast<i32>(
            outcomeValue
            + (artsD + AI_BATTLE_DEFENDER_ARTIFACT_BASE)
                  * (gbHumanPlayer[H2EnumIndex(defenderHero->m_owner)] != 0 ? gfAttackHumanBonus
                                                              : gfAttackComputerBonus)
                  * winChance
        );
    }

    if (giDebugLevel >= AI_BATTLE_DEBUG_LEVEL) {
        LogInt(
            "POBA",
            static_cast<i32>(attStr),
            static_cast<i32>(defStrong),
            static_cast<i32>(
                gpCurPlayer->m_aiData.m_attentionWeights.upgradeBase * AI_BATTLE_PERCENT_SCALE
            ),
            0,
            aArt,
            artsD,
            static_cast<i32>(gpCurPlayer->m_aiData.m_upgradeValueWeight)
        );
        LogInt(
            "POB",
            static_cast<i32>(attFight),
            static_cast<i32>(defenderFight),
            static_cast<i32>(winChance * AI_BATTLE_PERCENT_SCALE),
            defenderLoss,
            attackerRemaining,
            defenderRemaining,
            outcomeValue
        );
    }
}

float philAI::GetOddsOfWinning(i32) {
    return 1.0f;
}

void philAI::ValueOfBuyingBuilding(
    town* townPointer,
    BuildingSlotType building,
    i32& resourceValue,
    float& benefitCost
) {
    FactionType currentTownRace;
    i32 dwellingTotal;
    i32 highestDwellingId;
    i32 currentOccupiedSlots;
    float adjustedValue;
    b32 creatureLocated;
    i32 buildingLevel;
    i32 indexBuilding;
    CreatureType currentCreatureType;
    i32 costsByResource[H2EnumIndex(RES_COUNT)];
    float estimatedAttackChance;
    float enemyStrengthLocal;
    i32 currentAttackTurns;
    i32 projectedAttackValue;
    i32 estimatedAttackWeeks;
    float dangerRating;
    currentTownRace = townPointer->m_type;
    dwellingTotal = 0;
    highestDwellingId = -1;
    for (indexBuilding = H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST);
         indexBuilding < H2EnumIndex(BUILDING_SLOT_DWELLING_LAST);
         indexBuilding++) {
        if ((H2EnumIndex((townPointer->m_buildings) & ((1 << indexBuilding))))) {
            dwellingTotal++;
            highestDwellingId = indexBuilding;
        }
    }
    currentOccupiedSlots = 0;
    for (indexBuilding = 0; indexBuilding < AI_TOWN_ARMY_SLOTS; indexBuilding++) {
        if (townPointer->m_army.m_creatureCounts[indexBuilding] > 0)
            currentOccupiedSlots++;
    }
    adjustedValue = static_cast<float>(GetBuildingBaseResourceValue(
        currentTownRace,
        building,
        building == BUILDING_SLOT_MAGE_GUILD ? townPointer->m_buildState + 1 : 1
    ));
    if (building >= BUILDING_SLOT_UPGRADE_FIRST && building <= BUILDING_SLOT_UPGRADE_LAST) {
        adjustedValue -= static_cast<float>(
            GetBuildingBaseResourceValue(
                currentTownRace,
                building - CREATURE_UPGRADE_BUILDING_OFFSET,
                1
            )
        );
    }
    if (building == BUILDING_SLOT_DWELLING_LAST) {
        adjustedValue -= static_cast<float>(GetBuildingBaseResourceValue(
            currentTownRace,
            BUILDING_SLOT_DWELLING_LAST - 1,
            1
        ));
    }
    if (building == BUILDING_SLOT_MAGE_GUILD && townPointer->m_buildState > 1) {
        adjustedValue -= static_cast<float>(
            GetBuildingBaseResourceValue(currentTownRace, building, townPointer->m_buildState)
        );
    }

    switch (building) {
        case BUILDING_SLOT_CASTLE:
            if (townPointer->m_mayNotUpgradeToCastle != 0)
                adjustedValue = -99.0f;
            break;
        case BUILDING_SLOT_MAGE_GUILD:
            if (townPointer->m_type == FACTION_KNIGHT || townPointer->m_type == FACTION_BARBARIAN) {
                if ((townPointer->m_buildState >= 4 && giCurTurn < 40)
                    || (townPointer->m_buildState >= 3 && giCurTurn < 30)
                    || (townPointer->m_buildState >= 2 && giCurTurn < 20))
                    adjustedValue = static_cast<float>(adjustedValue * 0.4);
                else
                    adjustedValue = static_cast<float>(adjustedValue * 0.65);
            } else if (townPointer->m_buildState == 0) {
                adjustedValue = static_cast<float>(adjustedValue * 1.2);
            } else if ((townPointer->m_buildState >= 4 && giCurTurn < 35)
                       || (townPointer->m_buildState >= 3 && giCurTurn < 25)
                       || (townPointer->m_buildState >= 2 && giCurTurn < 15)) {
                adjustedValue =
                    static_cast<float>(adjustedValue * AI_MAGE_GUILD_LATE_FACTOR);
            }
            break;
        case BUILDING_SLOT_SPECIAL_ONE:
            if (townPointer->m_type == FACTION_NECROMANCER
                && (H2EnumIndex((townPointer->m_buildings) & ((1 << H2EnumIndex(BUILDING_SLOT_DWELLING_THIRD))))))
                adjustedValue = 1500.0f;
            else if (giCurTurn < 21)
                adjustedValue = 0.0;
            break;
        case BUILDING_SLOT_SPECIAL_SEVEN:
            if (giCurTurn < 3 && !(H2EnumIndex((townPointer->m_buildings) & ((1 << H2EnumIndex(BUILDING_SLOT_DWELLING_THIRD))))))
                adjustedValue = 0.0f;
            break;
        case BUILDING_SLOT_SPECIAL_TEN:
            if (giCurTurn < 21)
                adjustedValue = 0.0f;
            break;
        case BUILDING_SLOT_WELL_EXTRA:
            if (gpGame->m_day < 6)
                adjustedValue = 0.0f;
            break;
        case BUILDING_SLOT_SPECIAL_EIGHT:
        case BUILDING_SLOT_SPECIAL_NINE:
        case BUILDING_SLOT_SPECIAL_TWELVE:
        case BUILDING_SLOT_NEUTRAL_LAST:
        deferEarlyBuilding:
            if (giCurTurn < 16)
                adjustedValue = 0.0f;
            break;
        case BUILDING_SLOT_SPECIAL:
            if (townPointer->m_type == FACTION_KNIGHT || townPointer->m_type == FACTION_SORCERESS
                || townPointer->m_type == FACTION_BARBARIAN
                || townPointer->m_type == FACTION_NECROMANCER) {
                goto deferEarlyBuilding;
            }
            break;
        case BUILDING_SLOT_DISABLED_FIRST:
            break;
        case BUILDING_SLOT_DOCK:
            adjustedValue = 0.0f;
            break;
        case BUILDING_SLOT_SPECIAL_FOUR:
            adjustedValue = static_cast<float>(
                adjustedValue * (gpCurPlayer->m_aiData.m_attentionWeights.buildingValue + 0.66)
            );
            adjustedValue = static_cast<float>(
                adjustedValue
                * (gpCurPlayer->m_aiData.m_attentionWeights.upgradeBase * 2.0f + 0.33)
            );
            adjustedValue = static_cast<float>(adjustedValue * (dwellingTotal * 0.33 + 0.66));
            if ((townPointer->m_type != FACTION_KNIGHT
                 || !(H2EnumIndex((townPointer->m_buildings) & ((1 << H2EnumIndex(BUILDING_SLOT_DWELLING_SECOND))))))
                && gpGame->m_day < 6)
                adjustedValue = 0.0f;
            break;
        case BUILDING_SLOT_NECROMANCER_SHRINE:
            if (townPointer->m_type == FACTION_NECROMANCER)
                break;
            if ((townPointer->m_type == FACTION_KNIGHT
                 && (H2EnumIndex((townPointer->m_buildings) & ((1 << H2EnumIndex(BUILDING_SLOT_DWELLING_THIRD))))))
                || (townPointer->m_type == FACTION_SORCERESS
                    && (H2EnumIndex((townPointer->m_buildings) & ((1 << H2EnumIndex(BUILDING_SLOT_DWELLING_THIRD))))))) {
                adjustedValue = 1000.0f;
            } else {
                goto deferEarlyBuilding;
            }
            break;
        case BUILDING_SLOT_DWELLING_FIRST:
        case BUILDING_SLOT_DWELLING_SECOND:
        case BUILDING_SLOT_DWELLING_THIRD:
        case BUILDING_SLOT_DWELLING_FOURTH:
        case BUILDING_SLOT_DWELLING_FIFTH:
        case BUILDING_SLOT_DWELLING_SIXTH:
        case BUILDING_SLOT_UPGRADE_FIRST:
        case BUILDING_SLOT_UPGRADE_SECOND:
        case BUILDING_SLOT_UPGRADE_THIRD:
        case BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE:
        case BUILDING_SLOT_SPECIAL_TWENTY_NINE:
        case BUILDING_SLOT_SPECIAL_THIRTY:
            if (currentOccupiedSlots == AI_TOWN_ARMY_SLOTS) {
                creatureLocated = false;
                for (indexBuilding = 0; indexBuilding < AI_TOWN_ARMY_SLOTS; indexBuilding++) {
                    if (townPointer->m_army.m_creatureTypes[indexBuilding]
                        == gDwellingType[H2EnumIndex(townPointer->m_type)]
                                        [H2EnumIndex(building) - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)]) {
                        creatureLocated = true;
                    }
                }
                if (!creatureLocated)
                    break;
            }
            adjustedValue = static_cast<float>(
                adjustedValue * (gpCurPlayer->m_aiData.m_attentionWeights.buildingValue + 0.66)
            );
            adjustedValue = static_cast<float>(
                adjustedValue
                * (gpCurPlayer->m_aiData.m_attentionWeights.upgradeBase * 2.0f + 0.33)
            );
            adjustedValue = static_cast<float>(
                adjustedValue
                * (1.0 - gpCurPlayer->BuildingsOwned(currentTownRace, building, 0) * 0.05)
            );
            if (static_cast<i32>(building) - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)
                < highestDwellingId)
                adjustedValue = static_cast<float>(
                    adjustedValue
                    * (AI_EARLY_DWELLING_FACTOR_BASE - dwellingTotal * 0.33)
                );
            if ((H2EnumIndex((townPointer->m_buildings) & ((1 << H2EnumIndex(BUILDING_SLOT_SPECIAL_FOUR))))))
                adjustedValue = static_cast<float>(adjustedValue * 1.1);
            for (buildingLevel = 0; buildingLevel < AI_DWELLING_LEVELS; buildingLevel++) {
                currentCreatureType = gDwellingType[H2EnumIndex(townPointer->m_type)][buildingLevel];
                if ((H2EnumIndex((townPointer->m_buildings) & ((1 << H2EnumIndex(buildingLevel + H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST))))))

                    && townPointer->m_garrison[buildingLevel] > 0
                    && gMonsterDatabase[H2EnumIndex(currentCreatureType)].iconIndex * 1.2
                           > gMonsterDatabase[H2EnumIndex(gDwellingType[H2EnumIndex(townPointer->m_type)][H2EnumIndex(building)
                                                 - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)])]
                                 .iconIndex) {
                    adjustedValue = 0.0f;
                    break;
                }
            }
            break;
    }

    LikelihoodOfEnemyAttacking(
        townPointer,
        NULL,
        estimatedAttackChance,
        enemyStrengthLocal,
        currentAttackTurns,
        projectedAttackValue,
        estimatedAttackWeeks,
        dangerRating
    );
    adjustedValue = static_cast<float>(adjustedValue * (1.0 - dangerRating * 3.0));
    if (townPointer->m_originalOwner != giCurPlayer)
        adjustedValue = static_cast<float>(adjustedValue * 0.85);
    if (adjustedValue < 0.0f)
        adjustedValue = 0.0f;
    GetBuildingCost(
        currentTownRace,
        building,
        costsByResource,
        building == BUILDING_SLOT_MAGE_GUILD ? townPointer->m_buildState : 0
    );
    adjustedValue = FutureDeflator(costsByResource) * adjustedValue;
    resourceValue = static_cast<i32>(adjustedValue);
    benefitCost = adjustedValue / RVConversion(costsByResource);
}

void philAI::GetBestBuilding(town* townPointer, BHC& purchase, float& benefitCost) {
    float score;
    float bestScore;
    BuildingSlotType currentBuilding;
    float buildingBenefitCost;
    BuildingSlotType bestBuilding;
    float bestBenefitCost;
    i32 cost;
    bestBenefitCost = -99.0f;
    bestScore = -99.0f;
    bestBuilding = BUILDING_SLOT_NONE;
    for (currentBuilding = BUILDING_SLOT_MAGE_GUILD; currentBuilding < BUILDING_SLOT_COUNT; currentBuilding++) {
        if (!(H2EnumIndex((townPointer->m_buildings) & ((1 << H2EnumIndex(currentBuilding)))))
            || (currentBuilding == BUILDING_SLOT_MAGE_GUILD && townPointer->m_buildState < AI_MAX_MAGE_GUILD_LEVEL)) {
            if (CanBuild(townPointer, currentBuilding)) {
                ValueOfBuyingBuilding(townPointer, currentBuilding, cost, buildingBenefitCost);
                if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_BUILDER)
                    cost = static_cast<i32>(cost * 1.3);
                score = (Random(1, 5) + 95) * buildingBenefitCost / 100.0f;
                if (score > bestScore) {
                    bestBuilding = currentBuilding;
                    bestBenefitCost = buildingBenefitCost;
                    bestScore = score;
                }
                if (giDebugLevel >= AI_PURCHASE_VALUE_DEBUG_LEVEL) {
                    sprintf(
                        gText,
                        "Town:%2d  Building: % 18s   Raw BC = %8.2f,  RandBC = %8.2f.",
                        townPointer->m_id,
                        GetBuildingName(townPointer->m_type, currentBuilding),
                        buildingBenefitCost,
                        score
                    );
                    LogStr(gText);
                }
            }
        }
    }
    purchase.pTown = townPointer;
    purchase.type = PURCHASE_BUILDING;
    purchase.building = bestBuilding;
    benefitCost = bestBenefitCost;
}

void philAI::ValueOfBuyingCreature(
    town* townPointer,
    CreatureType creature,
    i32& resourceValue,
    i32 purchaseCount,
    float& benefitCost
) {
    float chance;
    i32 buyCost[H2EnumIndex(RES_COUNT)];
    float peril;
    i32 creatureRV;
    i32 costRV;
    float foeStrength;
    i32 turnsToAttack;
    i32 archers;
    i32 weeksLeft;
    i32 points;
    float riskFactor;
    hero* heroPointer;
    i32 troopSlot;
    i32 townSlot;

    archers = 0;
    GetMonsterCost(creature, buyCost);
    costRV = purchaseCount * RVConversion(buyCost);
    creatureRV = static_cast<i32>(
        purchaseCount * gMonsterDatabase[H2EnumIndex(creature)].fightValue
        * gpCurPlayer->m_aiData.m_upgradeValueWeight
    );

    if (townPointer->m_occupyingHeroId != AI_TROOP_EMPTY_SLOT) {
        heroPointer = gpGame->GetHero(townPointer->m_occupyingHeroId);
        creatureRV = static_cast<i32>(creatureRV * AI_CREATURE_VISITING_HERO_FACTOR);
        if (gMonsterDatabase[H2EnumIndex(creature)].race == heroPointer->m_cursorType)
            creatureRV = static_cast<i32>(creatureRV * AI_CREATURE_SAME_RACE_FACTOR);
        if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(creature)].attributes) & (MONSTER_FLAGS_SHOOTER)))) {
            for (troopSlot = 0; troopSlot < ARMY_GROUP_SLOT_COUNT;
                 troopSlot++) {
                if (heroPointer->m_army.m_creatureTypes[troopSlot]
                        != CREATURE_NONE
                    && (H2EnumIndex((gMonsterDatabase[H2EnumIndex(heroPointer->m_army
                                                 .m_creatureTypes[troopSlot])]
                            .attributes) & (MONSTER_FLAGS_SHOOTER)))) {
                    archers++;
                }
            }
            creatureRV = static_cast<i32>(
                creatureRV
                * (AI_CREATURE_RANGED_BASE_FACTOR - archers * AI_CREATURE_RANGED_STACK_FACTOR)
            );
        }
        creatureRV = static_cast<i32>(
            creatureRV
            * (gpGame->m_players[H2EnumIndex(townPointer->m_owner)].m_aiData.m_attentionWeights.upgradeBase
               + AI_CREATURE_BALANCE_BASE)
        );
    }

    if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(creature)].attributes) & (MONSTER_FLAGS_SHOOTER)))) {
        for (townSlot = 0; townSlot < ARMY_GROUP_SLOT_COUNT;
             townSlot++) {
            if (townPointer->m_army.m_creatureTypes[townSlot] != CREATURE_NONE
                && (H2EnumIndex((gMonsterDatabase[H2EnumIndex(townPointer->m_army.m_creatureTypes[townSlot])].attributes) & (MONSTER_FLAGS_SHOOTER)))) {
                archers++;
            }
        }
        creatureRV = static_cast<i32>(
            creatureRV
            * (AI_CREATURE_RANGED_BASE_FACTOR - archers * AI_CREATURE_RANGED_STACK_FACTOR)
        );
    }

    LikelihoodOfEnemyAttacking(
        townPointer,
        NULL,
        chance,
        foeStrength,
        turnsToAttack,
        points,
        weeksLeft,
        peril
    );
    riskFactor = static_cast<float>(peril + AI_CREATURE_DANGER_BASE);
    creatureRV = static_cast<i32>(creatureRV * (riskFactor * riskFactor * riskFactor));
    creatureRV = static_cast<i32>(creatureRV * FutureDeflator(buyCost));
    resourceValue = creatureRV;


    benefitCost = static_cast<float>(resourceValue) / (static_cast<float>(costRV));
}

void philAI::GetBestCreature(town* townPointer, BHC& best, float& bestValue) {
    i32 numberToBuy;
    float bestRawValue;
    i32 bestDwelling;
    i32 armyIndex;
    i32 availableUnits;
    i32 dwelling;
    CreatureType candidateMonster;
    float unrandomizedValue;
    float bestRandomizedScore;
    i32 weakestArmyValue;
    b32 canAddUnit;
    float jitteredValue;
    i32 resourceValue;

    bestDwelling = CREATURE_PURCHASE_NO_SLOT;
    numberToBuy = 0;
    bestRawValue = AI_PURCHASE_INITIAL_VALUE;
    bestRandomizedScore = AI_PURCHASE_INITIAL_VALUE;
    for (dwelling = 0; dwelling < DWELLING_TYPE_COUNT; dwelling++) {
        candidateMonster = gDwellingType[H2EnumIndex(townPointer->m_type)][dwelling];
        weakestArmyValue = CREATURE_PURCHASE_VALUE_LIMIT;
        if ((H2EnumIndex((townPointer->m_buildings) & ((1 << H2EnumIndex(dwelling + H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST))))))
            && townPointer->m_garrison[dwelling] > 0) {
            canAddUnit = false;
            for (armyIndex = 0; armyIndex < ARMY_GROUP_SLOT_COUNT; armyIndex++) {
                if (townPointer->m_army.m_creatureTypes[armyIndex] == CREATURE_NONE
                    || townPointer->m_army.m_creatureTypes[armyIndex] == candidateMonster) {
                    canAddUnit = true;
                }
            }
            for (armyIndex = 0; armyIndex < ARMY_GROUP_SLOT_COUNT; armyIndex++) {
                if (townPointer->m_army.m_creatureTypes[armyIndex] != CREATURE_NONE
                    && gMonsterDatabase[H2EnumIndex(townPointer->m_army.m_creatureTypes[armyIndex])].fightValue
                           < weakestArmyValue) {
                    weakestArmyValue =
                        gMonsterDatabase[H2EnumIndex(townPointer->m_army.m_creatureTypes[armyIndex])].fightValue;
                }
            }
            if (gMonsterDatabase[H2EnumIndex(candidateMonster)].fightValue > weakestArmyValue
                && gMonsterDatabase[H2EnumIndex(candidateMonster)].fightValue
                       > CREATURE_PURCHASE_EXPENSIVE_VALUE) {
                canAddUnit = true;
            }
            if (dwelling == 0 && townPointer->m_occupyingHeroId != AI_TROOP_EMPTY_SLOT
                && ((H2EnumIndex((townPointer->m_buildings) & (H2EnumIndex(TOWN_BUILDING_UPGRADED_DWELLING_5))))
                    || (H2EnumIndex((townPointer->m_buildings) & (H2EnumIndex(TOWN_BUILDING_DWELLING_5))))
                    || (H2EnumIndex((townPointer->m_buildings) & (H2EnumIndex(TOWN_BUILDING_UPGRADED_DWELLING_6))))
                    || (H2EnumIndex((townPointer->m_buildings) & (H2EnumIndex(TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6))))
                    || (H2EnumIndex((townPointer->m_buildings) & (H2EnumIndex(TOWN_BUILDING_DWELLING_6)))))) {
                canAddUnit = false;
            }
            if (canAddUnit) {
                availableUnits = CreaturesToBuy(townPointer, dwelling);
                if (availableUnits > 0) {
                    ValueOfBuyingCreature(
                        townPointer,
                        candidateMonster,
                        resourceValue,
                        availableUnits,
                        unrandomizedValue
                    );
                    if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_WARRIOR)
                        resourceValue =
                            static_cast<i32>(resourceValue * AI_CREATURE_EASY_COST_FACTOR);
                    if (townPointer->m_threat != 0)
                        resourceValue <<= 1;
                    jitteredValue = static_cast<float>(
                        (Random(1, 10) + AI_CREATURE_RANDOM_BASE)
                        * unrandomizedValue
                        / AI_PURCHASE_RANDOM_DIVISOR
                    );
                    if (jitteredValue > bestRandomizedScore) {
                        bestDwelling = dwelling;
                        bestRawValue = unrandomizedValue;
                        bestRandomizedScore = jitteredValue;
                        numberToBuy = availableUnits;
                    }
                    if (giDebugLevel >= AI_PURCHASE_VALUE_DEBUG_LEVEL) {
                        sprintf(
                            gText,
                            "Town:%2d  Creature: % 18s   Raw BC = %8.2f,  RandBC = %8.2f.",
                            townPointer->m_id,
                            GetMonsterName(candidateMonster),
                            unrandomizedValue,
                            jitteredValue
                        );
                        LogStr(gText);
                    }
                }
            }
        }
    }
    best.pTown = townPointer;
    best.type = PURCHASE_CREATURE;
    best.what = bestDwelling;
    best.num = numberToBuy;
    bestValue = bestRawValue;
}

i32 philAI::CreaturesToBuy(town* townPointer, i32 level) {
    i32 nGarrison = townPointer->m_garrison[level];
    return CreaturesToBuy(gDwellingType[H2EnumIndex(townPointer->m_type)][level], nGarrison);
}

i32 philAI::CreaturesToBuy(CreatureType creatureType, i32 availableCount) {
    i32 purchaseCount = MaxBuyableCreatures(creatureType);
    if (purchaseCount > 1)
        purchaseCount >>= 1;
    if (purchaseCount > availableCount)
        purchaseCount = availableCount;
    if (purchaseCount > 1)
        return purchaseCount;
    else
        return 0;
}

i32 philAI::MaxBuyableCreatures(CreatureType creatureType) {
    i32 resourceIndex;
    i32 i;
    i32 cost[H2EnumIndex(RES_COUNT)];
    GetMonsterCost(creatureType, cost);
    for (i = 0; i < H2EnumIndex(RES_COUNT); i++) {
        if (cost[i] == 0)
            resourceIndex = CREATURE_PURCHASE_UNLIMITED;
        else if (gpCurPlayer->m_resources[i] > 0)
            resourceIndex = gpCurPlayer->m_resources[i] / cost[i];
        else
            resourceIndex = 0;
    }
    return resourceIndex;
}

void philAI::ValueOfBuyingHero(
    town* townPointer,
    hero* heroPointer,
    i32& resourceValue,
    float& benefitCost
) {
    i32 heroCost[H2EnumIndex(RES_COUNT)];
    i32 rvCost;
    b32 spellCaster;
    i32 i;
    i32 j [[maybe_unused]];
    i32 heroRV;

    heroCost[H2EnumIndex(RES_WOOD)] = 0;
    heroCost[H2EnumIndex(RES_MERCURY)] = 0;
    heroCost[H2EnumIndex(RES_ORE)] = 0;
    heroCost[H2EnumIndex(RES_SULFUR)] = 0;
    heroCost[H2EnumIndex(RES_CRYSTAL)] = 0;
    heroCost[H2EnumIndex(RES_GEMS)] = 0;
    heroCost[H2EnumIndex(RES_GOLD)] = AI_HERO_PURCHASE_GOLD_COST;
    rvCost = RVConversion(heroCost);
    heroRV = heroPointer->m_experience + AI_HERO_PURCHASE_EXPERIENCE_BASE;
    for (i = 0; i < HERO_ARTIFACT_SLOT_COUNT; i++) {
        if (heroPointer->m_artifacts[i] >= ARTIFACT_ULTIMATE_BOOK
            && heroPointer->m_artifacts[i] < ARTIFACT_EDITOR_ANY_ULTIMATE
            && heroPointer->m_artifacts[i] != ARTIFACT_MAGIC_BOOK) {
            heroRV += gArtifactBaseRV[H2EnumIndex(heroPointer->m_artifacts[i])];
        }
    }
    heroRV += heroPointer->m_experience / 2;
    heroRV = static_cast<i32>(
        heroRV
        * (gpCurPlayer->m_aiData.m_attentionWeights.heroValue + 1.0
           - gpCurPlayer->m_aiData.m_attentionWeights.upgradeBase)
    );
    if (heroPointer->m_cursorType == FACTION_SORCERESS
        || heroPointer->m_cursorType == FACTION_WARLOCK
        || heroPointer->m_cursorType == FACTION_WIZARD
        || heroPointer->m_cursorType == FACTION_NECROMANCER)
        spellCaster = true;
    else
        spellCaster = false;
    if (townPointer->m_type == heroPointer->m_cursorType) {
        heroRV *= AI_HERO_PURCHASE_SAME_RACE_FACTOR + AI_HERO_PURCHASE_IDENTITY;
    } else if ((townPointer->m_buildState >= 2 && spellCaster)
               || (townPointer->m_buildState < 2 && !spellCaster)) {
        heroRV *= AI_HERO_PURCHASE_SAME_RACE_FACTOR / AI_HERO_PURCHASE_CLASS_DIVISOR
                  + AI_HERO_PURCHASE_IDENTITY;
    } else {
        heroRV *= AI_HERO_PURCHASE_IDENTITY
                  - AI_HERO_PURCHASE_SAME_RACE_FACTOR / AI_HERO_PURCHASE_CLASS_DIVISOR;
    }
    heroRV = static_cast<i32>(heroRV * FutureDeflator(heroCost));
    benefitCost = static_cast<float>(heroRV) / rvCost;
    resourceValue = heroRV;
}

void philAI::GetBestHero(town* townPointer, BHC& best, float& bestValue) {
    float bestRawLocal;
    i32 heroIndex;
    i32 bestHeroIndex;
    float bestScore;
    float benefitCost;
    float randomizedScore;
    i32 resourceValue;
    hero* heroPointer;

    bestHeroIndex = CREATURE_PURCHASE_NO_SLOT;
    bestRawLocal = AI_PURCHASE_INITIAL_VALUE;
    bestScore = AI_PURCHASE_INITIAL_VALUE;
    for (heroIndex = 0; heroIndex < HERO_AVAILABLE_SLOT_COUNT; heroIndex++) {
        heroPointer = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[heroIndex]];
        ValueOfBuyingHero(townPointer, heroPointer, resourceValue, benefitCost);
        randomizedScore = static_cast<float>(
            benefitCost * (Random(1, 10) + AI_HERO_PURCHASE_RANDOM_BASE)
            / AI_PURCHASE_RANDOM_DIVISOR
        );
        if (randomizedScore > bestScore) {
            bestHeroIndex = heroIndex;
            bestRawLocal = benefitCost;
            bestScore = randomizedScore;
        }
        if (giDebugLevel >= AI_PURCHASE_VALUE_DEBUG_LEVEL) {
            sprintf(
                gText,
                "Town:%2d  Hero    : % 15i   Raw BC = %8.2f,  RandBC = %8.2f.",
                townPointer->m_id,
                heroIndex,
                benefitCost,
                randomizedScore
            );
            LogStr(gText);
        }
    }
    best.pTown = townPointer;
    best.type = PURCHASE_HERO;
    best.what = bestHeroIndex;
    bestValue = bestRawLocal;
    if (gpGame->m_worldMap.GetCell(townPointer->m_x, townPointer->m_y)->m_triggerType
        == (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))) {
        bestValue -= AI_HERO_PURCHASE_CELL_PENALTY;
    }
}

void philAI::LikelihoodOfEnemyAttacking(
    town*,
    hero*,
    float& chanceA,
    float& chanceB,
    i32& nAttack,
    i32& nValue,
    i32& nWeeks,
    float& fOut
) {
    chanceA = 0.15f;
    chanceB = 0.6f;
    nAttack = 3000;
    nValue = static_cast<i32>(nAttack * chanceA);
    nWeeks = 6;
    fOut = chanceA * chanceB;
}

i32 philAI::MeanRVOfUnexploredTerritory(i32) {
    return 0;
}

void philAI::GetGameAttentionValue(i32 player) {
    playerAttentionWeights* attention =
        &gpGame->m_players[player].m_aiData.m_attentionWeights;
    attention->gameWeightA = static_cast<float>(Random(0, 100) / AI_ATTENTION_RANDOM_DIVISOR)
                             + AI_ATTENTION_RANDOM_BASE;
    attention->gameWeightB = static_cast<float>(Random(0, 100) / AI_ATTENTION_RANDOM_DIVISOR)
                             + AI_ATTENTION_RANDOM_BASE;
    attention->gameWeightB *=
        (AI_ATTENTION_IDENTITY_FLOAT + AI_ATTENTION_PLAYER_CENTER) / AI_ATTENTION_NORMALIZER;
    attention->gameWeightB *=
        (AI_ATTENTION_UPPER_BOUND - AI_ATTENTION_IDENTITY) / AI_ATTENTION_NORMALIZER;
    attention->gameWeightA *=
        (AI_ATTENTION_IDENTITY + AI_ATTENTION_PLAYER_CENTER) / AI_ATTENTION_NORMALIZER;
    attention->gameWeightB = static_cast<float>(
        attention->gameWeightB
        * ((AI_ATTENTION_PLAYER_CENTER - gpGame->m_playerCount)
               * AI_ATTENTION_WEIGHT_B_PLAYER_FACTOR
           + 1.0)
    );
    attention->gameWeightA = static_cast<float>(
        attention->gameWeightA
        * ((AI_ATTENTION_PLAYER_CENTER - gpGame->m_playerCount)
               * AI_ATTENTION_WEIGHT_A_PLAYER_FACTOR
           + 1.0)
    );
    attention->gameRemainder = ((1.0f - attention->gameWeightB) - attention->gameWeightA);
}

void philAI::GetTurnAttentionValue(i32 player) {
    playerAttentionWeights* attentionWeights = &gpGame->m_players[player].m_aiData.m_attentionWeights;
    attentionWeights->gameWeightA = 0.4f;
    attentionWeights->gameWeightB = 0.3f;
    attentionWeights->gameRemainder = 0.3f;
    attentionWeights->buildingValue = attentionWeights->gameWeightA;
    attentionWeights->heroValue = attentionWeights->gameWeightB;
    attentionWeights->upgradeBase = attentionWeights->gameRemainder;
    float scale;
    if (giCurTurn < 5)
        scale = 1.6f;
    else if (giCurTurn < 10)
        scale = 1.4f;
    else if (giCurTurn < 20)
        scale = 1.2f;
    else if (giCurTurn < 30)
        scale = 1.0f;
    else
        scale = 0.8f;
    attentionWeights->heroValue = attentionWeights->heroValue * scale;
}

i32 philAI::RVConversion(i32* const resources) {
    return static_cast<i32>((((((((resources[H2EnumIndex(RES_GOLD)] * gafAITurnCostResource[H2EnumIndex(RES_GOLD)])
                      + resources[H2EnumIndex(RES_WOOD)] * gafAITurnCostResource[H2EnumIndex(RES_WOOD)])
                     + resources[H2EnumIndex(RES_ORE)] * gafAITurnCostResource[H2EnumIndex(RES_ORE)])
                    + resources[H2EnumIndex(RES_CRYSTAL)] * gafAITurnCostResource[H2EnumIndex(RES_CRYSTAL)])
                   + resources[H2EnumIndex(RES_SULFUR)] * gafAITurnCostResource[H2EnumIndex(RES_SULFUR)])
                  + resources[H2EnumIndex(RES_MERCURY)] * gafAITurnCostResource[H2EnumIndex(RES_MERCURY)])
                 + resources[H2EnumIndex(RES_GEMS)] * gafAITurnCostResource[H2EnumIndex(RES_GEMS)]));
}

float philAI::TurnsToBuy(i32* const resources) {
    float maxT = 0;
    i32 resourceIndex;
    float turnCount;
    for (resourceIndex = 0; resourceIndex < H2EnumIndex(RES_COUNT); resourceIndex++) {
        if (gpCurPlayer->m_resources[resourceIndex] < resources[resourceIndex]) {
            if (gpCurPlayer->m_aiData.m_income[resourceIndex] > 0)
                turnCount = static_cast<float>(
                    (resources[resourceIndex] - gpCurPlayer->m_resources[resourceIndex]) / gpCurPlayer->m_aiData.m_income[resourceIndex] + 1
                );
            else
                turnCount = 99.0f;
            maxT = turnCount > maxT ? turnCount : maxT;
        }
    }
    return maxT;
}

i32 philAI::RVOfPosition(
    i32 x,
    i32 y,
    i32 hasEvent,
    i32 eventX,
    i32 eventY,
    i32 hasStrategicEvent,
    i32 strategicX,
    i32 strategicY,
    i32 eventMode,
    i32 extraDistance
) {
    i32 adjacentMonsterEventChance;
    i32 adjacentY;
    float distanceFactor;
    i32 adjacentX;
    i32 primaryEventChance;
    i32 adjacentEventChance;
    MapObjectType objectType;
    i32 totalValue;
    i32 strategicLiveChance;
    i32 strategicEventValue;
    i32 currentStrategicValue;
    MapObjectType debugObjectType;
    i32 unusedValue [[maybe_unused]];
    i32 targetLiveChance;
    i32 eventValue;
    i32 strategicDelta;
    MapTriggerCode triggerType;
    i32 currentLiveChance;
    char debugText[POSITION_DEBUG_TEXT_CAPACITY];

    strategicEventValue = 0;
    targetLiveChance = POSITION_FULL_CHANCE;
    adjacentEventChance = POSITION_FULL_CHANCE;
    triggerType = gpAdvManager->GetCell(x, y)->m_triggerType;
    objectType = triggerType & MAP_TRIGGER_TYPE_MASK;
    primaryEventChance = POSITION_FULL_CHANCE;
    strategicLiveChance = POSITION_FULL_CHANCE;
    adjacentMonsterEventChance = POSITION_FULL_CHANCE;

    if (abs(x - gpCurAIHero->m_x) <= AI_POSITION_NEARBY_DELTA
        && abs(y - gpCurAIHero->m_y) <= AI_POSITION_NEARBY_DELTA) {
        currentStrategicValue = 0;
        strategicDelta = 0;
    } else {
        currentStrategicValue = StrategicValueOfPosition(
            gpCurAIHero->m_x,
            gpCurAIHero->m_y,
            0,
            1,
            &currentLiveChance,
            0
        );
        strategicDelta = StrategicValueOfPosition(x, y, 0, 1, &targetLiveChance, extraDistance);
        strategicDelta -= currentStrategicValue;
    }
    if (objectType == MAP_OBJECT_BOAT && strategicDelta < 0)
        strategicDelta = 0;

    totalValue = 0;
    if (hasEvent)
        totalValue += ValueOfEventAtPosition(eventX, eventY, 1, &strategicLiveChance);
    if (hasStrategicEvent) {
        strategicEventValue =
            StrategicValueOfPosition(strategicX, strategicY, 1, 1, &adjacentEventChance, 0);
        if (strategicEventValue < 0)
            totalValue += strategicEventValue;
    }

    if (gpAdvManager->FindAdjacentMonster(x, y, &adjacentX, &adjacentY, -1, -1)) {
        if (StopOnTrigger(gpAdvManager->GetCell(x, y))) {
            eventValue =
                ValueOfEventAtPosition(adjacentX, adjacentY, 1, &adjacentMonsterEventChance);
            if (eventValue < 0)
                totalValue += eventValue;
            if (strategicLiveChance == POSITION_FULL_CHANCE)
                strategicLiveChance = adjacentMonsterEventChance;
            else
                strategicLiveChance = strategicLiveChance * adjacentMonsterEventChance
                                      / POSITION_FULL_CHANCE;
        }
    }

    if ((H2EnumIndex((triggerType) & (MAP_TRIGGER_ACTION_FLAG)))
        || (x == gpCurPlayer->m_ultimateArtifactHintX
            && y == gpCurPlayer->m_ultimateArtifactHintY)) {
        eventValue = ValueOfEventAtPosition(x, y, eventMode, &primaryEventChance);
    } else {
        eventValue = 0;
    }
    if (primaryEventChance < POSITION_FULL_CHANCE)
        strategicDelta = strategicDelta * primaryEventChance / POSITION_FULL_CHANCE;

    if (targetLiveChance < POSITION_MINIMUM_LIVE_CHANCE)
        return POSITION_FAILED_VALUE;
    if (targetLiveChance < POSITION_FULL_CHANCE) {
        eventValue = eventValue * targetLiveChance / POSITION_FULL_CHANCE;
        strategicDelta = strategicDelta * targetLiveChance / POSITION_FULL_CHANCE;
    }
    if (adjacentEventChance < POSITION_MINIMUM_LIVE_CHANCE)
        return POSITION_FAILED_VALUE;
    if (adjacentEventChance < POSITION_FULL_CHANCE) {
        eventValue = eventValue * adjacentEventChance / POSITION_FULL_CHANCE;
        strategicDelta = strategicDelta * adjacentEventChance / POSITION_FULL_CHANCE;
    }
    if (strategicLiveChance < POSITION_FULL_CHANCE) {
        if (totalValue > 0) {
            totalValue = (totalValue + eventValue + strategicDelta) * strategicLiveChance
                         / POSITION_FULL_CHANCE;
        } else {
            totalValue +=
                (eventValue + strategicDelta) * strategicLiveChance / POSITION_FULL_CHANCE;
        }
    } else {
        totalValue += eventValue;
    }

    distanceFactor = static_cast<float>(gpSearchArray->GetRow(x, 1)[MAP_WIDTH * y].distance)
                     / gpCurAIHero->m_mobility;
    if (gpCurAIHero->IsEmbarked()) {
        distanceFactor = static_cast<float>(
            distanceFactor * AI_POSITION_EMBARKED_DISTANCE_FACTOR
            + AI_POSITION_EMBARKED_DISTANCE_FACTOR
        );
    } else if (distanceFactor > AI_POSITION_LAND_DISTANCE_6) {
        distanceFactor *= AI_POSITION_LAND_FACTOR_FAR;
    } else if (distanceFactor > AI_POSITION_LAND_DISTANCE_5) {
        distanceFactor = static_cast<float>(distanceFactor * AI_POSITION_LAND_FACTOR_6);
    } else if (distanceFactor > AI_POSITION_LAND_DISTANCE_4) {
        distanceFactor = static_cast<float>(distanceFactor * AI_POSITION_LAND_FACTOR_5);
    } else if (distanceFactor > AI_POSITION_LAND_DISTANCE_3) {
        distanceFactor = static_cast<float>(distanceFactor * AI_POSITION_LAND_FACTOR_4);
    } else if (distanceFactor > AI_POSITION_LAND_DISTANCE_2) {
        distanceFactor = static_cast<float>(distanceFactor * AI_POSITION_LAND_FACTOR_3);
    } else if (distanceFactor > AI_POSITION_LAND_DISTANCE_1) {
        distanceFactor = static_cast<float>(distanceFactor * AI_POSITION_LAND_FACTOR_2);
    }

    totalValue = static_cast<i32>(totalValue / (distanceFactor + AI_POSITION_DISTANCE_BASE));
    strategicDelta = static_cast<i32>(
        strategicDelta * POSITION_STRATEGIC_MULTIPLIER
        / (AI_POSITION_STRATEGIC_DISTANCE_BASE + distanceFactor)
    );
    if (strategicLiveChance == POSITION_FULL_CHANCE)
        totalValue += strategicDelta;
    if (gpCurAIHero->IsEmbarked() && triggerType == MAP_OBJECT_COAST) {
        totalValue += POSITION_EMBARKED_BOAT_BONUS;
    }

    if (giDebugLevel > POSITION_DEBUG_LEVEL - 1) {
        debugObjectType = triggerType & MAP_TRIGGER_TYPE_MASK;
        sprintf(
            debugText,
            "FUN U :% 15s",
            (debugObjectType > MAP_OBJECT_NONE
             && H2EnumIndex(debugObjectType) < POSITION_OBJECT_NAME_COUNT)
                ? gQuickViewText[H2EnumIndex(debugObjectType)]
                : " "
        );
        LogInt(
            debugText,
            x,
            y,
            totalValue,
            eventValue,
            strategicDelta,
            targetLiveChance * POSITION_FULL_CHANCE
        );
    }
    return totalValue;
}

i32 philAI::StrategicValueOfPosition(
    i32 targetX,
    i32 targetY,
    i32 immediate,
    i32 checkEnemies,
    i32* liveChance,
    i32 extraDistance
) {
    i32 seedDist;
    i32 scratch [[maybe_unused]];
    i32 eventPoints;
    H2EnumStorage<TerrainType, i32> terrain;
    i32 xPosition;
    i32 inBoat;
    i32 dangerVal;
    b32 madeSearch;
    i32 extra [[maybe_unused]];
    i32 worth;
    i32 mapY;
    i32 heroIndex;
    i32 nearDistance;
    searchArray* search;
    i32 enemyRange;
    i32 gap;
    H2EnumStorage<TerrainType, i32> targetTerrain;
    mapCell* cell;
    i32 penalty;
    b32 townOK;

    if (!extraDistance && !immediate
        && *(gaiHeroStrategicRVOfPos + targetX + targetY * MAP_WIDTH) != RV_UNSET) {
        *liveChance = *(gaiLiveChanceOfPos + targetX + targetY * MAP_WIDTH);
        return *(gaiHeroStrategicRVOfPos + targetX + targetY * MAP_WIDTH);
    }

    worth = 0;
    madeSearch = false;
    townOK = false;
    cell = gpAdvManager->GetCell(targetX, targetY);
    if (cell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
        && GetCastleSlot(cell->m_objectMetadata)->m_owner == giCurPlayer
        && (H2EnumIndex((GetCastleSlot(cell->m_objectMetadata)->m_buildings) & (H2EnumIndex(TOWN_BUILDING_CASTLE))))) {
        townOK = true;
    }
    *liveChance = POSITION_FULL_CHANCE;
    if (checkEnemies && !*(gaiEnemyHeroReachable + targetX + targetY * MAP_WIDTH)) {
        if (immediate)
            return 0;
        else
            checkEnemies = 0;
    }

    if (bSVSearchArrayInUse) {
        madeSearch = true;
        search = new searchArray;
        if (!search)
            MemError();
        search->Init();
    } else {
        bSVSearchArrayInUse = true;
        search = &SVSearchArray;
    }

    inBoat = gpCurAIHero->IsEmbarked();
    if (inBoat && gpAdvManager->GetCell(targetX, targetY)->m_triggerType == MAP_OBJECT_COAST)
        inBoat = 0;
    nearDistance = 700;
    if (immediate || checkEnemies)
        seedDist = 1700;
    else
        seedDist = nearDistance;
    search->SeedPosition(
        targetX,
        targetY,
        MAP_DIRECTION_EAST,
        seedDist,
        H2EnumIndex(inBoat),
        0,
        59999,
        H2EnumIndex(gpCurAIHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_PATHFINDING)]),
        -1,
        -1,
        0,
        0
    );
    search->GetNode(targetX, targetY).visited = 0;

    for (xPosition = 0; xPosition < MAP_WIDTH; xPosition++) {
        for (mapY = 0; mapY < MAP_HEIGHT; mapY++) {
            if (search->GetNode(xPosition, mapY).visited) {
                cell = gpAdvManager->GetCell(xPosition, mapY);
                if ((!immediate && (H2EnumIndex((cell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))
                     && search->GetNode(xPosition, mapY).distance <= nearDistance)
                    || (checkEnemies
                        && cell->m_triggerType
                               == (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION)))) {
                    CheckDoMain(0, 0);
                    eventPoints = static_cast<i32>(
                        (ValueOfEventAtPosition(xPosition, mapY, 0, &iDummy) * 25)
                        / (search->GetNode(xPosition, mapY).distance + AI_STRATEGIC_DISTANCE_OFFSET)
                    );
                    if (!townOK
                        || cell->m_triggerType
                               != (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))
                        || eventPoints >= 0)
                        worth += eventPoints;
                }

                if (cell->m_triggerType
                    == (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))) {
                    if (gaiHeroLiveChance[cell->m_objectMetadata] == RV_UNSET)
                        ValueOfEventAtPosition(xPosition, mapY, 0, &iDummy);
                    if (gaiHeroLiveChance[cell->m_objectMetadata] != RV_UNSET
                        && gaiHeroLiveChance[cell->m_objectMetadata] < 100) {
                        if (!townOK) {
                            enemyRange = GetHeroSlot(cell->m_objectMetadata)->m_mobility;
                            if (gbHumanPlayer[gpGame->m_availableHeroes[cell->m_objectMetadata]]) {
                                if (search->GetNode(xPosition, mapY).distance + extraDistance
                                    <= enemyRange) {
                                    if (search->GetNode(xPosition, mapY).distance + extraDistance
                                        <= 500) {
                                        dangerVal = 100 - gaiHeroLiveChance[cell->m_objectMetadata];
                                    } else {
                                        dangerVal =
                                            (100 - gaiHeroLiveChance[cell->m_objectMetadata])
                                            * ((enemyRange
                                                - (search->GetNode(xPosition, mapY).distance
                                                   + extraDistance))
                                               + 250)
                                            / enemyRange;
                                    }
                                } else {
                                    dangerVal = static_cast<i32>(
                                        (100 - gaiHeroLiveChance[cell->m_objectMetadata]) * 0.2
                                    );
                                }
                            } else {
                                dangerVal =
                                    (100 - gaiHeroLiveChance[cell->m_objectMetadata])
                                    * ((enemyRange + 500)
                                       - (search->GetNode(xPosition, mapY).distance + extraDistance))
                                    / (enemyRange + 500);
                            }
                            *liveChance = *liveChance * (100 - dangerVal) / 100;
                        }
                    }
                }

                if (gpCurAIHero->m_id != iAlphaMale && (targetX != xPosition || targetY != mapY)
                    && search->GetNode(xPosition, mapY).distance < 1024
                    && gpAdvManager->GetCell(xPosition, mapY)->m_triggerType
                           == (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))
                    && gpAdvManager->GetCell(xPosition, mapY)->m_objectMetadata != gpCurAIHero->m_id
                    && gpGame->m_availableHeroes[gpAdvManager->GetCell(xPosition, mapY)
                                                     ->m_objectMetadata]
                           == gpCurAIHero->m_owner) {
                    penalty = (1024 - search->GetNode(xPosition, mapY).distance) * 600 >> 10;
                    if (penalty > 0)
                        worth -= penalty;
                }
            }
        }
    }

    targetTerrain = CELL_TERRAIN(gpAdvManager->GetCell(targetX, targetY));
    for (heroIndex = 0; heroIndex < gpCurPlayer->m_heroCount; heroIndex++) {
        if (gpCurPlayer->m_heroIds[heroIndex] != gpCurAIHero->m_id) {
            gap = MANHATTAN_LENGTH(
                gpGame->m_heroRecs[gpCurPlayer->m_heroIds[heroIndex]].m_destinationX - targetX,
                gpGame->m_heroRecs[gpCurPlayer->m_heroIds[heroIndex]].m_destinationY - targetY
            );
            if (gap < 9) {
                terrain = CELL_TERRAIN(gpAdvManager->GetCell(
                    gpGame->m_heroRecs[gpCurPlayer->m_heroIds[heroIndex]].m_destinationX,
                    gpGame->m_heroRecs[gpCurPlayer->m_heroIds[heroIndex]].m_destinationY
                ));
                if (!((targetTerrain == TERRAIN_WATER && terrain > TERRAIN_WATER)
                      || (targetTerrain > TERRAIN_WATER
                          && terrain == TERRAIN_WATER))) {
                    worth -= (9 - gap) * 600 / 9;
                }
            }
        }
    }

    if (madeSearch)
        delete search;
    else
        bSVSearchArrayInUse = false;

    if (*liveChance < 100)
        worth -= (100 - *liveChance) * 2500 / 100;
    worth = static_cast<i32>(worth * AI_STRATEGIC_POSITION_SCORE_FACTOR);
    if (worth > 32000)
        worth = 32000;
    if (!immediate && !extraDistance) {
        *(gaiHeroStrategicRVOfPos + targetX + targetY * MAP_WIDTH) = static_cast<i16>(worth);
        *(gaiLiveChanceOfPos + targetX + targetY * MAP_WIDTH) = static_cast<i16>(*liveChance);
    }
    return worth;
}

i32 philAI::ValueOfTown(town* townPointer) {
    i32 sum = 0;
    BuildingSlotType building;
    for (building = BUILDING_SLOT_MAGE_GUILD; building <= BUILDING_SLOT_DWELLING_SIXTH;
         building++) {
        if ((H2EnumIndex((townPointer->m_buildings) & ((1 << H2EnumIndex(building))))))
            sum += GetBuildingBaseResourceValue(townPointer->m_type, building, townPointer->m_buildState);
    }
    sum = static_cast<i32>(
        sum
        + AI_GOLD_TURN_VALUE_SCALE * gafAITurnCostResource[H2EnumIndex(RES_GOLD)] * 5.0f * 1.5
    );
    sum += 750;
    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_TOWN
        && gpGame->m_mapHeader.lossConditionValue == townPointer->m_x
        && gpGame->m_mapHeader.lossTownY == townPointer->m_y)
        sum += 50000;
    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_CAPTURE_TOWN
        && gpGame->m_mapHeader.victoryConditionValue == townPointer->m_x
        && gpGame->m_mapHeader.victoryTownY == townPointer->m_y)
        sum += 50000;
    return sum;
}

void philAI::TurnCostResource(i32 player) {
    playerAIData* playerAI;
    float frac[H2EnumIndex(RES_COUNT)];
    float average;
    i32 resourceIndex;
    i32 sum;
    i32 resValue[H2EnumIndex(RES_COUNT)];
    playerAI = &gpGame->m_players[player].m_aiData;
    sum = 0;
    for (resourceIndex = 0; resourceIndex < H2EnumIndex(RES_COUNT); resourceIndex++) {
        resValue[resourceIndex] = static_cast<i32>(
            gResourceBaseValue[resourceIndex]
            * ((playerAI->m_income[resourceIndex] * 5) * 0.7
               + gpGame->m_players[player].m_resources[resourceIndex])
        );
        sum += resValue[resourceIndex];
    }
    average = (sum / H2EnumIndex(RES_COUNT));
    for (resourceIndex = 0; resourceIndex < H2EnumIndex(RES_COUNT); resourceIndex++) {
        frac[resourceIndex] = resValue[resourceIndex] / average;
        gafAITurnCostResource[resourceIndex] = (gResourceBaseValue[resourceIndex] / (frac[resourceIndex] / 2.0f + 0.5));
    }
}

float philAI::TurnValueOfObelisk(i32 player) {
    playerAIData* playerAI;
    i32 each;
    i32 artRV;
    playerAI = &gpGame->m_players[player].m_aiData;
    artRV = gArtifactBaseRV[H2EnumIndex(gpGame->m_ultimateArtifactId)];
    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_FIND_ARTIFACT)
        artRV <<= 1;
    each = artRV / 110;
    if (gpGame->m_ultimateArtifactId == ARTIFACT_NONE)
        return 0.0f;
    playerAI->m_obeliskValue = each * GAME_OBELISK_VISITOR_COUNT / gpGame->m_obeliskCount;
    if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_EXPLORER)
        playerAI->m_obeliskValue = static_cast<i32>(playerAI->m_obeliskValue * 1.4);
    playerAI->m_obeliskValue = static_cast<i32>(
        playerAI->m_obeliskValue
        * (1.5
           - abs(GAME_OBELISK_VISITOR_COUNT - gpGame->SetupPuzzlePieces(giCurPlayer, 1))
                 / AI_OBELISK_VISITOR_COUNT_FLOAT)
    );
    playerAI->m_obeliskValue =
        static_cast<i32>(playerAI->m_obeliskValue * (playerAI->m_attentionWeights.heroValue + 0.66));
    return playerAI->m_obeliskValue;
}

float philAI::FutureDeflator(i32* const resources) {
    float turns = TurnsToBuy(resources);
    float value = 1.0f - turns * AI_FUTURE_DEFLATION_RATE;
    if (value < AI_FUTURE_MINIMUM_VALUE)
        value = 0;
    return value;
}

i32 philAI::FightValueOfStack(
    armyGroup* group,
    hero* heroPointer,
    i32 useHero,
    i32 useTown,
    i32 townId,
    i32 useEnemyMods
) {
    i32 armyValue;
    float rangeVal [[maybe_unused]];
    town* townPointer;
    float closeCombat;
    i32 castleValue;
    float countMod;
    i32 spellIndex;
    i32 slot;
    i32 magicTotal;
    float archerMod;
    float flyMod;
    i32 stackWorth;
    i32 numShooters;
    i32 heroLuck;
    i32 towerArchers;
    i32 archerGrade;
    i32 statSum;
    i32 morale;
    i32 numSpellCasts;
    i32 spellCount;
    float lengthMod;
    i32 maxScore;
    i32 spellScores[FIGHT_VALUE_SPELL_COUNT];
    i32 spellScore;
    i32 bestSpellIndex;
    float magicMod;

    armyValue = 0;
    magicTotal = 0;
    castleValue = 0;
    archerMod = 0.9f;
    closeCombat = 1.1f;
    flyMod = 0.75f;
    rangeVal = 0.1f;
    townPointer = NULL;

    if (useEnemyMods) {
        if (heroPointer->HasArtifact(ARTIFACT_BALLISTA) || heroPointer->HasSpell(SPELL_EARTHQUAKE)
            || heroPointer->m_secondarySkills[H2EnumIndex(HERO_SKILL_BALLISTICS)]
                   != HERO_SKILL_LEVEL_NONE) {
            closeCombat = 1.05f;
            flyMod = 0.95f;
        }
        if (heroPointer->m_secondarySkills[H2EnumIndex(HERO_SKILL_ARCHERY)] != HERO_SKILL_LEVEL_NONE
            || heroPointer->HasArtifact(ARTIFACT_GOLDEN_BOW))
            archerMod = 1.05f;
    }
    if (useTown) {
        townPointer = GetCastleSlot(townId);
        if (!(H2EnumIndex((townPointer->m_buildings) & (AI_BUILDING_CASTLE_MASK))))
            useTown = 0;
    }

    for (slot = 0; slot < AI_TOWN_ARMY_SLOTS; slot++) {
        if (group->m_creatureTypes[slot] != CREATURE_NONE) {
            stackWorth = group->m_creatureCounts[slot]
                       * gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].fightValue;
            if (useHero) {
                if (group->m_creatureCounts[slot] > 500)
                    countMod = 4.4f;
                else if (group->m_creatureCounts[slot] > 300)
                    countMod = 3.9f;
                else if (group->m_creatureCounts[slot] > 180)
                    countMod = 3.08f;
                else if (group->m_creatureCounts[slot] > 140)
                    countMod = 2.42f;
                else if (group->m_creatureCounts[slot] > 100)
                    countMod = 2.1f;
                else if (group->m_creatureCounts[slot] > 75)
                    countMod = 1.63f;
                else if (group->m_creatureCounts[slot] > 50)
                    countMod = 1.4f;
                else if (group->m_creatureCounts[slot] > 35)
                    countMod = 1.0f;
                else if (group->m_creatureCounts[slot] > 23)
                    countMod = 0.75f;
                else if (group->m_creatureCounts[slot] > 16)
                    countMod = 0.5f;
                else if (group->m_creatureCounts[slot] > 11)
                    countMod = 0.32f;
                else if (group->m_creatureCounts[slot] > 8)
                    countMod = 0.21f;
                else if (group->m_creatureCounts[slot] > 5)
                    countMod = 0.0;
                else if (group->m_creatureCounts[slot] > 3)
                    countMod = -0.1f;
                else if (group->m_creatureCounts[slot] > 2)
                    countMod = -0.3f;
                else if (group->m_creatureCounts[slot] > 2)
                    countMod = -0.4f;
                else
                    countMod = -0.58f;

                if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].attributes) & (MONSTER_FLAGS_SHOOTER)))
                    || IS_VAMPIRE_CREATURE(group->m_creatureTypes[slot])
                    || group->m_creatureTypes[slot] == CREATURE_SPRITE
                    || group->m_creatureTypes[slot] == CREATURE_ROGUE
                    || group->m_creatureTypes[slot] == CREATURE_HYDRA
                    || group->m_creatureTypes[slot] == CREATURE_TROLL) {
                    countMod = static_cast<float>(countMod * 0.7);
                } else if (group->m_creatureTypes[slot] == CREATURE_WOLF) {
                    countMod = static_cast<float>(countMod * 0.85);
                } else if (group->m_creatureTypes[slot] == CREATURE_GRIFFIN
                           || group->m_creatureTypes[slot] == CREATURE_GHOST) {
                    countMod = static_cast<float>(countMod * 1.2);
                }
                stackWorth = static_cast<i32>(stackWorth * (1.0f + countMod));

                if (useTown) {
                    if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].attributes) & (MONSTER_FLAGS_SHOOTER))))
                        stackWorth = static_cast<i32>(stackWorth * 1.18);
                    if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].attributes) & (MONSTER_FLAGS_FLYING))))
                        stackWorth =
                            static_cast<i32>(stackWorth * AI_TOWN_FLYING_CREATURE_FACTOR);
                }
                if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].attributes) & (MONSTER_FLAGS_SHOOTER)))
                    && heroPointer
                    && heroPointer->m_secondarySkills[H2EnumIndex(HERO_SKILL_ARCHERY)]
                           != HERO_SKILL_LEVEL_NONE) {
                    stackWorth = static_cast<i32>(
                        stackWorth
                        * gfSSAIArcheryMod
                            [H2EnumIndex(heroPointer->m_secondarySkills[H2EnumIndex(HERO_SKILL_ARCHERY)])]
                    );
                }
                if (useEnemyMods) {
                    if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].attributes) & (MONSTER_FLAGS_SHOOTER))))
                        stackWorth = static_cast<i32>(stackWorth * archerMod);
                    if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].attributes) & (MONSTER_FLAGS_FLYING))))
                        stackWorth = static_cast<i32>(stackWorth * closeCombat);
                    else
                        stackWorth = static_cast<i32>(stackWorth * flyMod);
                }
            }
            armyValue += stackWorth;
        }
    }

    if (useTown) {
        towerArchers = 0;
        townPointer->CalcNumLevelArchers(&numShooters, &archerGrade);
        if ((H2EnumIndex((townPointer->m_buildings) & (AI_BUILDING_LEFT_TURRET_MASK))))
            towerArchers += numShooters >> 1;
        if ((H2EnumIndex((townPointer->m_buildings) & (AI_BUILDING_RIGHT_TURRET_MASK))))
            towerArchers += numShooters >> 1;
        numShooters += towerArchers;
        castleValue = static_cast<i32>(numShooters * 400 * (archerGrade * 0.1 + 1.0));
    }

    if (useHero && heroPointer) {
        statSum =
            heroPointer->Stats(HERO_PRIMARY_ATTACK) + heroPointer->Stats(HERO_PRIMARY_DEFENSE) + 20;
        if (statSum < 0)
            statSum = 0;
        if (statSum > 40)
            statSum = 40;
        armyValue = static_cast<i32>(armyValue * gfStatPower[statSum]);
        castleValue =
            static_cast<i32>(castleValue * gfStatPower[statSum]);

        morale = heroPointer->m_army.GetMorale(heroPointer, heroPointer->GetOccupiedTown(), NULL);
        if (morale > 0)
            armyValue = armyValue * (morale + 48) / 48;
        else if (morale < 0)
            armyValue = armyValue * (morale + 24) / 24;

        heroLuck = gpGame->GetLuck(heroPointer, NULL, useTown ? townPointer : NULL);
        if (heroLuck)
            armyValue = armyValue * (heroLuck + 16) / 16;

        if (heroPointer->m_spellPoints >= 3) {
            spellCount = 0;
            if (heroPointer->Stats(HERO_PRIMARY_SPELL_POWER) <= FIGHT_VALUE_SPELL_STAT_LIMIT) {
                lengthMod =
                    gfPhilAIDurationMod[heroPointer->Stats(HERO_PRIMARY_SPELL_POWER)];
                magicMod =
                    gfPhilAISpellPowerMod[heroPointer->Stats(HERO_PRIMARY_SPELL_POWER)];
            } else {
                lengthMod = gfPhilAIDurationMod[FIGHT_VALUE_SPELL_STAT_LIMIT];
                magicMod = gfPhilAISpellPowerMod[FIGHT_VALUE_SPELL_STAT_LIMIT];
            }
            for (slot = 0; slot < FIGHT_VALUE_SPELL_COUNT; slot++) {
                if (heroPointer->HasSpell(SpellType(slot))) {
                    spellScore = gsSpellInfo[slot].aiValue;
                    if ((H2EnumIndex((gsSpellInfo[slot].attributes) & (SPELL_INFO_ATTRIBUTE_DURATION))))
                        spellScore = static_cast<i32>(spellScore * lengthMod);
                    else if ((H2EnumIndex((gsSpellInfo[slot].attributes) & (SPELL_INFO_ATTRIBUTE_POWER))))
                        spellScore = static_cast<i32>(spellScore * magicMod);
                    numSpellCasts =
                        heroPointer->m_spellPoints / GetManaCost(SpellType(slot), heroPointer);
                    if (numSpellCasts > FIGHT_VALUE_SPELL_STAT_LIMIT)
                        numSpellCasts = FIGHT_VALUE_SPELL_STAT_LIMIT;
                    spellScore =
                        static_cast<i32>(spellScore * gfSpellCastNumMod[numSpellCasts]);
                    if (spellScore <= 0)
                        spellScore = 1;
                    spellScores[spellCount] = spellScore;
                    spellCount++;
                }
            }
            for (slot = 0; slot < KB_QUICK_COMBAT_SPELL_TYPE_COUNT;
                 slot++) {
                if (slot >= spellCount) {
                    break;
                }
                maxScore = 0;
                for (spellIndex = 0; spellIndex < spellCount; spellIndex++) {
                    if (spellScores[spellIndex] > maxScore) {
                        maxScore = spellScores[spellIndex];
                        bestSpellIndex = spellIndex;
                    }
                }
                magicTotal = static_cast<i32>(
                    magicTotal + maxScore * gfSpellTypeNumMod[slot]
                );
                spellScores[bestSpellIndex] = 0;
            }
        }
    }

    if (magicTotal > armyValue * 2)
        magicTotal = static_cast<i32>(armyValue * 1.25);
    else if (magicTotal > armyValue * 1.5) {
        magicTotal = armyValue;
    } else if (magicTotal > armyValue)
        magicTotal = static_cast<i32>(armyValue * 0.75);
    if (castleValue > armyValue * 2)
        castleValue = static_cast<i32>(armyValue * 1.5);
    else if (castleValue > armyValue * 1.5) {
        castleValue = static_cast<i32>(armyValue * AI_TOWN_ARCHER_OVERWHELMING_FACTOR);
    } else if (castleValue > armyValue)
        castleValue = static_cast<i32>(armyValue * AI_TOWN_ARCHER_ADVANTAGE_FACTOR);
    if (giDebugLevel == AI_BATTLE_DEBUG_LEVEL)
        LogInt("FV3", armyValue, magicTotal, castleValue, 0, 0);
    armyValue += magicTotal;
    armyValue += castleValue;
    return armyValue;
}

void philAI::EvaluateOneTimeCreaturePurchase(
    CreatureType creature,
    i32 availableCount,
    i32 useAvailableCount,
    i32& purchaseCount,
    i32& purchaseValue,
    i32& replacementSlot
) {
    i32 replacementStackValue;
    i32 leastStackValue;
    i32 purchaseFightValue;
    i32 armyIndex;

    purchaseCount = 0;
    purchaseValue = 0;
    replacementSlot = CREATURE_PURCHASE_NO_SLOT;
    leastStackValue = CREATURE_PURCHASE_VALUE_LIMIT;
    if (useAvailableCount != 0)
        purchaseCount = availableCount;
    else
        purchaseCount = MaxBuyableCreatures(creature);
    if (purchaseCount > availableCount)
        purchaseCount = availableCount;

    if (purchaseCount == 0)
        return;
    {
        purchaseFightValue = purchaseCount * gMonsterDatabase[H2EnumIndex(creature)].fightValue;
        if (gpCurAIHero->m_army.CanJoin(creature) == 0) {
            for (armyIndex = 0; armyIndex < ARMY_GROUP_SLOT_COUNT; armyIndex++) {
                if (gpCurAIHero->m_army.m_creatureTypes[armyIndex] == creature) {
                    replacementSlot = CREATURE_PURCHASE_NO_SLOT;
                    armyIndex = ARMY_GROUP_SLOT_COUNT;
                } else {
                    replacementStackValue = gpCurAIHero->m_army.m_creatureCounts[armyIndex]
                                             * gMonsterDatabase[armyIndex].fightValue;
                    if (replacementStackValue < leastStackValue) {
                        leastStackValue = replacementStackValue;
                        replacementSlot = armyIndex;
                    }
                }
            }
        }
        if (replacementSlot != CREATURE_PURCHASE_NO_SLOT)
            purchaseFightValue -= leastStackValue;

        purchaseValue = static_cast<i32>(
            purchaseFightValue
            * gpGame->m_players[H2EnumIndex(gpCurAIHero->m_owner)].m_aiData.m_upgradeValueWeight
        );
        if (useAvailableCount == 0) {
            GetMonsterCost(creature, costTemp);
            purchaseValue -= purchaseCount * RVConversion(costTemp);
        }
        if (purchaseValue < 0) {
            purchaseValue = 0;
            purchaseCount = 0;
        }
    }
}

i32 philAI::QuickCombat(
    armyGroup* attacker,
    hero* attackerHero,
    armyGroup* defender,
    hero* defenderHero,
    i32 townBattle,
    i32 townId,
    float& attackerDamage,
    float& defenderDamage
) {
    i32 defExp;
    float roll;
    armyGroup* army [[maybe_unused]];
    i32 attTotal;
    i32 result;
    i32 atkExp;
    float diff;
    i32 defenderCount;
    hero* defeated;
    hero* winHero;
    float fracLost;
    i32 attackerDead;
    i32 spare [[maybe_unused]];
    b32 attackerWin;
    i32 slot;
    i32 temporary [[maybe_unused]];
    float chanceWin;
    i32 defenderDead;
    i32 attackerRemaining;
    float winnerChance;
    i32 remainB;
    i32 necroCount;

    atkExp = gpGame->ExperienceValueOfStack(attacker, attackerHero);
    if (townBattle != 0)
        atkExp += QUICK_COMBAT_TOWN_EXPERIENCE;
    defExp = gpGame->ExperienceValueOfStack(defender, defenderHero);
    atkExp = static_cast<i32>(
        atkExp * (H2EnumIndex(gpGame->m_difficulty) * AI_QUICK_COMBAT_EXPERIENCE_DIFFICULTY_STEP + 1.0)
    );
    defExp = static_cast<i32>(
        defExp * (H2EnumIndex(gpGame->m_difficulty) * AI_QUICK_COMBAT_EXPERIENCE_DIFFICULTY_STEP + 1.0)
    );

    attackerWin = false;
    army = NULL;
    ProbableOutcomeOfBattle(
        attacker,
        attackerHero,
        defender,
        defenderHero,
        NULL,
        townBattle,
        townId,
        (defenderHero != NULL ? defenderHero->m_owner : AI_BATTLE_NO_PLAYER),
        chanceWin,
        attackerDead,
        defenderDead,
        attackerRemaining,
        remainB,
        result
    );

    roll = static_cast<float>(
        Random(0, QUICK_COMBAT_RANDOM_LIMIT) / static_cast<double>(QUICK_COMBAT_RANDOM_LIMIT)
    );
    if (roll < chanceWin) {
        attackerWin = true;
        winnerChance = chanceWin;
        army = attacker;
    } else {
        winnerChance = AI_BATTLE_FULL_CHANCE - chanceWin;
        army = defender;
    }

    diff = (roll > chanceWin ? roll - chanceWin : chanceWin - roll);
    if (attackerWin != 0 && chanceWin > AI_QUICK_COMBAT_WIN_BONUS_THRESHOLD)
        diff = static_cast<float>(diff * (chanceWin + AI_QUICK_COMBAT_WIN_BONUS));
    if (diff > AI_BATTLE_FULL_CHANCE)
        diff = AI_BATTLE_FULL_CHANCE;

    fracLost =
        static_cast<float>((1.0 - diff) * (1.0 - diff));
    if (winnerChance > AI_QUICK_COMBAT_HIGH_WIN_CHANCE
        && fracLost > AI_QUICK_COMBAT_MINIMUM_CASUALTY)
        fracLost *= fracLost;
    if (winnerChance > AI_QUICK_COMBAT_CASUALTY_CAP_CHANCE
        && fracLost
               > (AI_BATTLE_FULL_CHANCE - winnerChance) / AI_QUICK_COMBAT_CASUALTY_DIVISOR)
        fracLost =
            (AI_BATTLE_FULL_CHANCE - winnerChance) / AI_QUICK_COMBAT_CASUALTY_DIVISOR;
    if (fracLost > AI_QUICK_COMBAT_DAMAGE_PENALTY_LOW
        && fracLost < AI_QUICK_COMBAT_DAMAGE_PENALTY_HIGH)
        fracLost =
            static_cast<float>(fracLost - AI_QUICK_COMBAT_DAMAGE_PENALTY);

    if (attackerWin != 0) {
        if (attackerHero != NULL) {
            gpAdvManager->GiveExperience(attackerHero, defExp, 1);
            attackerHero->ApplyBattleWinTemps();
        }
        attackerHero->CheckLevel();
        defenderDamage = AI_BATTLE_FULL_CHANCE;
        attackerDamage = fracLost;
    } else {
        if (attackerHero != NULL) {
            attackerHero->m_remainingMobility = 0;
            attackerHero->ApplyBattleLossTemps();
        }
        if (defenderHero != NULL)
            attackerHero->ApplyBattleWinTemps();
        defenderDamage = fracLost;
        attackerDamage = AI_BATTLE_FULL_CHANCE;
        if (attackerDamage >= AI_QUICK_COMBAT_DEFEAT_THRESHOLD && defenderHero != NULL) {
            gpAdvManager->GiveExperience(defenderHero, defExp, 1);
            defenderHero->CheckLevel();
        }
    }

    attTotal = 0;
    defenderCount = 0;
    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; slot++) {
        if (attackerHero->m_army.m_creatureTypes[slot] != CREATURE_NONE)
            attTotal += attackerHero->m_army.m_creatureCounts[slot];
        if (defenderHero != NULL
            && defenderHero->m_army.m_creatureTypes[slot] != CREATURE_NONE)
            defenderCount += defenderHero->m_army.m_creatureCounts[slot];
    }

    gbRetreatWin = false;
    if ((attackerWin == 0 || (attackerWin != 0 && defenderHero != NULL))
        && Random(0, QUICK_COMBAT_RANDOM_LIMIT) < QUICK_COMBAT_RETREAT_CHANCE)
        gbRetreatWin = true;
    if (gbRetreatWin == 0) {
        if (attackerDamage > AI_QUICK_COMBAT_DEFEAT_THRESHOLD)
            gpAdvManager->TransferArtifacts(attackerHero, defenderHero);
        else if (defenderDamage > AI_QUICK_COMBAT_DEFEAT_THRESHOLD)
            gpAdvManager->TransferArtifacts(defenderHero, attackerHero);
    }

    DamageGroup(attacker, attackerHero, defenderHero, attackerDamage);
    DamageGroup(defender, defenderHero, attackerHero, defenderDamage);

    if (attackerWin != 0 && attackerHero->GetSSLevel(HERO_SKILL_NECROMANCY) != 0) {
        necroCount = static_cast<i32>(
            static_cast<float>(defenderCount) * AI_QUICK_COMBAT_NECROMANCY_FACTOR
            * static_cast<float>(attackerHero->GetSSLevel(HERO_SKILL_NECROMANCY))
        );
        if (necroCount <= 0)
            necroCount = 1;
        attackerHero->m_army
            .Add(CREATURE_SKELETON, necroCount, CREATURE_PURCHASE_NO_SLOT);
    } else if (defenderHero != NULL && defenderDamage <= AI_QUICK_COMBAT_NECROMANCY_THRESHOLD
               && defenderHero->GetSSLevel(HERO_SKILL_NECROMANCY) != 0) {
        necroCount = static_cast<i32>(
            static_cast<float>(attTotal) * AI_QUICK_COMBAT_NECROMANCY_FACTOR
            * static_cast<float>(defenderHero->GetSSLevel(HERO_SKILL_NECROMANCY))
        );
        if (necroCount <= 0)
            necroCount = 1;
        defenderHero->m_army
            .Add(CREATURE_SKELETON, necroCount, CREATURE_PURCHASE_NO_SLOT);
    }

    defeated = NULL;
    winHero = NULL;
    if (attackerWin != 0) {
        defeated = defenderHero;
        winHero = attackerHero;
    } else if (defenderDamage <= AI_QUICK_COMBAT_NECROMANCY_THRESHOLD) {
        defeated = attackerHero;
        winHero = defenderHero;
    }
    if (defeated != NULL
        && defeated->m_secondarySkills[H2EnumIndex(HERO_SKILL_EAGLE_EYE)]
               != HERO_SKILL_LEVEL_NONE
        && winHero != NULL) {
        for (slot = 0; slot < H2EnumIndex(SPELL_COUNT); slot++) {
            if (defeated->HasSpell(SpellType(slot)) != 0
                && winHero->HasSpell(SpellType(slot)) == 0
                && H2EnumIndex(winHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_EAGLE_EYE)]) + 1
                       >= H2EnumIndex(gsSpellInfo[slot].level)
                && (H2EnumIndex((gsSpellInfo[slot].attributes) & (SPELL_INFO_ATTRIBUTE_COMBAT)))) {
                winHero->m_spells[slot] = 1;
                break;
            }
        }
    }

    attackerHero->ApplyBattleWinTemps();
    if (defenderHero != NULL)
        defenderHero->ApplyBattleWinTemps();
    if (attackerWin != 0 && townBattle != 0)
        gpGame->ClaimTown(townId, giCurPlayer, 0);
    gbRetreatWin = false;
    return attackerWin;
}

void philAI::HeroInteractionAtHero(
    hero* firstHero,
    hero* secondHero,
    i32 evaluateOnly,
    i32* value
) {
    float transferFraction;
    i32 recipientFightValue;
    i32 dominantFightValue;
    i32 heroIndex;
    float armyShare;
    i32 heroValues[HERO_INTERACTION_HERO_COUNT];
    ArtifactType artifactType;
    float dominantShare;
    hero* dominantHero;
    i32 interactionValue;
    i32 statIndex;
    hero* recipientHero;
    hero* currentHero;
    float desiredShare;
    hero* savedHero [[maybe_unused]];

    *value = 0;
    if (evaluateOnly != 0 && firstHero->m_id != iAlphaMale
        && secondHero->m_id != iAlphaMale)
        return;
    {
        if (evaluateOnly == 0) {
            firstHero->m_lastHeroInteractionTurn = static_cast<i16>(giCurTurn);
            firstHero->m_lastInteractionHeroId = secondHero->m_id;
            secondHero->m_lastHeroInteractionTurn = static_cast<i16>(giCurTurn);
            secondHero->m_lastInteractionHeroId = firstHero->m_id;
        }

        for (heroIndex = 0; heroIndex < HERO_INTERACTION_HERO_COUNT; heroIndex++) {
            currentHero = heroIndex == 0 ? firstHero : secondHero;
            heroValues[heroIndex] = 0;
            for (statIndex = 0; statIndex < HERO_PRIMARY_STAT_COUNT;
                 statIndex++) {
                if (statIndex < HERO_INTERACTION_COMBAT_PRIMARY_STAT_COUNT
                    || currentHero->HasArtifact(ARTIFACT_MAGIC_BOOK))
                    heroValues[heroIndex] += currentHero->Stats(HeroPrimaryStat(statIndex))
                                                * HERO_INTERACTION_PRIMARY_STAT_VALUE;
            }
            for (statIndex = H2EnumIndex(HERO_SKILL_PATHFINDING); statIndex < H2EnumIndex(HERO_SKILL_COUNT);
                 statIndex++) {
                if (HeroSecondarySkill(statIndex) == HERO_SKILL_ESTATES)
                    continue;
                if (currentHero->m_secondarySkills[statIndex] != HERO_SKILL_LEVEL_NONE) {
                    heroValues[heroIndex] +=
                        gSSValues[statIndex]
                                 [H2EnumIndex(currentHero->m_secondarySkills[statIndex])
                                  - SECONDARY_SKILL_LEVEL_OFFSET];
                }
            }
        }

        dominantShare = static_cast<float>(heroValues[0])
                          / static_cast<float>(heroValues[0] + heroValues[1]);
        if (heroValues[0] > heroValues[1]) {
            dominantHero = firstHero;
            recipientHero = secondHero;
        } else {
            dominantHero = secondHero;
            recipientHero = firstHero;
            dominantShare = static_cast<float>(1.0 - dominantShare);
        }
        if (evaluateOnly != 0 && recipientHero->m_id == iAlphaMale
            && dominantShare < AI_HERO_INTERACTION_ALPHA_SHARE) {
            savedHero = recipientHero;
            recipientHero = dominantHero;
            dominantHero = recipientHero;
        }

        dominantFightValue = FightValueOfStack(&dominantHero->m_army, NULL, 0);
        recipientFightValue = FightValueOfStack(&recipientHero->m_army, NULL, 0);
        desiredShare = AI_HERO_INTERACTION_TARGET_SHARE;
        armyShare = static_cast<float>(dominantFightValue)
                     / static_cast<float>(dominantFightValue + recipientFightValue);
        transferFraction = (desiredShare < armyShare) ? (armyShare - desiredShare)
                                                           : (desiredShare - armyShare);
        if (transferFraction < AI_HERO_INTERACTION_MINIMUM_TRANSFER)
            return;
        {
            interactionValue = static_cast<i32>(
                gpCurPlayer->m_aiData.m_upgradeValueWeight
                * (static_cast<float>(dominantFightValue + recipientFightValue)
                   * transferFraction)
            );
            if (evaluateOnly != 0) {
                for (statIndex = 0; statIndex < HERO_ARTIFACT_SLOT_COUNT; statIndex++) {
                    artifactType = recipientHero->m_artifacts[statIndex];
                    if (artifactType != ARTIFACT_NONE && artifactType != ARTIFACT_MAGIC_BOOK)
                        interactionValue += gArtifactBaseRV[H2EnumIndex(artifactType)];
                }
            } else {
                gpAdvManager->TransferArtifacts(recipientHero, dominantHero);
                if (desiredShare < armyShare) {
                    RedistributeTroops(
                        &dominantHero->m_army,
                        &recipientHero->m_army,
                        1,
                        1,
                        dominantFightValue,
                        recipientFightValue,
                        static_cast<i32>(
                            static_cast<float>(dominantFightValue + recipientFightValue)
                            * transferFraction
                        )
                    );
                } else {
                    RedistributeTroops(
                        &recipientHero->m_army,
                        &dominantHero->m_army,
                        1,
                        1,
                        recipientFightValue,
                        dominantFightValue,
                        static_cast<i32>(
                            static_cast<float>(dominantFightValue + recipientFightValue)
                            * transferFraction
                        )
                    );
                }
                SetupRelativeHeroStrengths();
            }
            *value = static_cast<i32>(interactionValue * AI_HERO_INTERACTION_VALUE_FACTOR);
        }
    }
}

void philAI::HeroInteractionAtTown(hero* heroPointer, town* townPointer, i32 doInteraction, i32* value) {
    i32 townFV;
    i32 moveCount;
    armyGroup* fromArmy;
    b32 townBetter;
    i32 winStrength;
    i32 scratchVal [[maybe_unused]];
    float townShare;
    i32 whichSpell;
    armyGroup* secondArmy;
    i32 otherIndex;
    float wantShare;
    i32 stackSlot;
    i32 castLvl;
    i32 statSum;
    float transferShare;
    i32 battlePower;
    i32 destinationStrength;
    i32 transferRating;
    b32 hasRoom;
    float transferFactor;
    float curveTerm;

    *value = 0;
    if (doInteraction != 0) {
        if ((H2EnumIndex((townPointer->m_buildings) & (AI_BUILDING_SHIPYARD_MASK)))
            && giBestShipyardId != townPointer->m_id) {
            stackSlot = MANHATTAN_LENGTH(townPointer->m_x - heroPointer->m_x, townPointer->m_y - heroPointer->m_y);
            if (gbActualShipyardFound) {
                if (stackSlot < giBestShipyardDist) {
                    giBestShipyardDist = stackSlot;
                    giBestShipyardId = townPointer->m_id;
                }
            } else {
                giBestShipyardDist = stackSlot;
                giBestShipyardId = townPointer->m_id;
            }
            gbPossibleShipyardFound = true;
            gbActualShipyardFound = true;
        } else if ((H2EnumIndex((townPointer->m_buildings) & (AI_BUILDING_CASTLE_MASK)))
                   && CELL_TERRAIN(gpAdvManager->GetCell(townPointer->m_x - 1, townPointer->m_y + 1))
                          == TERRAIN_WATER
                   && !gbActualShipyardFound && giBestShipyardId != townPointer->m_id) {
            stackSlot = MANHATTAN_LENGTH(townPointer->m_x - heroPointer->m_x, townPointer->m_y - heroPointer->m_y);
            if (gbPossibleShipyardFound) {
                if (stackSlot < giBestShipyardDist) {
                    giBestShipyardDist = stackSlot;
                    giBestShipyardId = townPointer->m_id;
                }
            } else {
                giBestShipyardDist = stackSlot;
                giBestShipyardId = townPointer->m_id;
            }
            gbPossibleShipyardFound = true;
        }
    } else {
        heroPointer->m_lastTownInteractionTurn = static_cast<i16>(giCurTurn);
        heroPointer->m_visitedTownId = static_cast<u8>(townPointer->m_id);
        if (!heroPointer->HasArtifact(ARTIFACT_MAGIC_BOOK)
            && (H2EnumIndex((townPointer->m_buildings) & (AI_BUILDING_MAGE_GUILD_MASK)))) {
            if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] >= AI_MAGIC_BOOK_COST) {
                GiveArtifact(heroPointer, ARTIFACT_MAGIC_BOOK, true);
                gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= AI_MAGIC_BOOK_COST;
            } else {
                heroPointer->m_remainingMobility = 0;
            }
        }
        if ((H2EnumIndex((townPointer->m_buildings) & (AI_BUILDING_MAGE_GUILD_MASK)))
            && heroPointer->HasArtifact(ARTIFACT_MAGIC_BOOK)
            && heroPointer->m_spellPoints < HERO_NORMAL_SPELL_POINTS(*heroPointer)) {
            heroPointer->m_remainingMobility = 0;
        }
    }

    if ((H2EnumIndex((townPointer->m_buildings) & (AI_BUILDING_MAGE_GUILD_MASK)))
        && (doInteraction != 0 || heroPointer->HasArtifact(ARTIFACT_MAGIC_BOOK))) {
        *value += ManaRefreshValue(heroPointer, 1);
        for (castLvl = 1;
             castLvl
             <= H2EnumIndex(heroPointer->m_secondarySkills[H2EnumIndex(HERO_SKILL_WISDOM)])
                    + HERO_BASE_LEARNABLE_SPELL_LEVEL;
             castLvl++) {
            for (whichSpell = 0;
                 whichSpell < townPointer->m_spellCounts[castLvl - TOWN_MAGE_GUILD_FIRST_LEVEL];
                 whichSpell++) {
                if (!heroPointer->HasSpell(townPointer->m_spells[castLvl - 1][whichSpell])) {
                    *value +=
                        gsSpellInfo[H2EnumIndex(townPointer->m_spells[castLvl - 1][whichSpell])].aiValue
                        * ((H2EnumIndex((gsSpellInfo[H2EnumIndex(townPointer->m_spells[castLvl - 1][whichSpell])]
                                   .attributes) & (SPELL_INFO_ATTRIBUTE_POWER)))
                               ? heroPointer->Stats(HERO_PRIMARY_KNOWLEDGE)
                               : 1);
                }
            }
        }
    }

    battlePower = FightValueOfStack(&heroPointer->m_army, NULL, 0);
    townFV = FightValueOfStack(&townPointer->m_army, NULL, 0);
    townShare = static_cast<double>(townFV) / (townFV + battlePower);
    statSum = 0;
    statSum =
        heroPointer->Stats(HERO_PRIMARY_ATTACK) + heroPointer->Stats(HERO_PRIMARY_DEFENSE);
    if (statSum > HERO_INTERACTION_COMBAT_STAT_MAX)
        statSum = HERO_INTERACTION_COMBAT_STAT_MAX;
    if ((H2EnumIndex((townPointer->m_buildings) & (AI_BUILDING_CASTLE_MASK))))
        wantShare = static_cast<float>(
            AI_TOWN_CASTLE_PRIMARY_SKILL_SHARE_BASE
            - statSum * AI_TOWN_CASTLE_PRIMARY_SKILL_SHARE_STEP
        );
    else
        wantShare = static_cast<float>(
            AI_TOWN_PRIMARY_SKILL_SHARE_BASE
            - statSum * AI_TOWN_PRIMARY_SKILL_SHARE_STEP
        );
    if (giCurTurn <= AI_EARLY_TOWN_SHARE_TURN)
        wantShare = fFirstWeekTownFV;
    else if (giCurTurn <= AI_SECOND_WEEK_END_TURN)
        wantShare = static_cast<float>(wantShare * 0.5);
    else if (giCurTurn <= AI_THIRD_WEEK_END_TURN)
        wantShare =
            static_cast<float>(wantShare * AI_THIRD_WEEK_TOWN_SHARE_FACTOR);
    if (heroPointer->m_id == iAlphaMale)
        wantShare = static_cast<float>(wantShare * 0.5);
    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_CAPTURE_TOWN
        && gpGame->m_mapHeader.victoryConditionValue == townPointer->m_x
        && gpGame->m_mapHeader.victoryTownY == townPointer->m_y) {
        wantShare = 0.8f;
    }
    transferShare = (wantShare < townShare ? townShare - wantShare : wantShare - townShare);
    if (!(transferShare >= wantShare * AI_TOWN_SHARE_DIFFERENCE_FACTOR)
        || transferShare < AI_MINIMUM_TOWN_SHARE_DIFFERENCE)
        return;
    townBetter = false;
    if (wantShare < townShare)
        townBetter = true;
    if (doInteraction != 0) {
        if (battlePower < townFV)
            transferFactor = AI_WEAKER_ARMY_TRANSFER_FACTOR;
        else
            transferFactor = AI_STRONGER_ARMY_TRANSFER_FACTOR;
        curveTerm = static_cast<float>(
            AI_TOWN_TRANSFER_CURVE_OFFSET + transferShare - AI_TOWN_TRANSFER_CURVE_CENTER
        );
        transferRating = static_cast<i32>(
            (curveTerm * curveTerm - AI_TOWN_TRANSFER_CURVE_OFFSET)
            * gpCurPlayer->m_aiData.m_upgradeValueWeight * (townFV + battlePower)
            * transferFactor
        );
        if (transferRating < 0)
            transferRating = 0;
        hasRoom = false;
        if (townBetter) {
            for (stackSlot = 0; stackSlot < AI_TOWN_ARMY_SLOTS; stackSlot++) {
                if (heroPointer->m_army.m_creatureCounts[stackSlot] <= 0)
                    hasRoom = true;
            }
        } else {
            for (stackSlot = 0; stackSlot < AI_TOWN_ARMY_SLOTS; stackSlot++) {
                if (townPointer->m_army.m_creatureCounts[stackSlot] <= 0)
                    hasRoom = true;
            }
        }
        if (!hasRoom) {
            for (stackSlot = 0; stackSlot < AI_TOWN_ARMY_SLOTS; stackSlot++) {
                for (otherIndex = 0; otherIndex < AI_TOWN_ARMY_SLOTS; otherIndex++) {
                    if (townPointer->m_army.m_creatureTypes[stackSlot]
                        == heroPointer->m_army.m_creatureTypes[otherIndex]) {
                        hasRoom = true;
                        break;
                    }
                }
            }
        }
        if (!hasRoom)
            transferRating = 0;
        *value += transferRating;
        if (townPointer->m_threat != 0 && townPointer->m_occupyingHeroId == -1)
            *value += AI_UNGUARDED_TOWN_VALUE;
    } else {
        townPointer->GiveSpells(NULL);
        if (townBetter)
            transferShare = static_cast<float>(transferShare + AI_TOWN_TRANSFER_BONUS);
        moveCount = static_cast<i32>((battlePower + townFV) * transferShare);
        fromArmy = (townBetter ? &townPointer->m_army : &heroPointer->m_army);
        secondArmy = (townBetter ? &heroPointer->m_army : &townPointer->m_army);
        if (townBetter) {
            winStrength = townFV;
            destinationStrength = battlePower;
        } else {
            winStrength = battlePower;
            destinationStrength = townFV;
        }
        RedistributeTroops(
            fromArmy,
            secondArmy,
            !townBetter,
            townBetter,
            winStrength,
            destinationStrength,
            moveCount
        );
        if (giHumanTownConquered == townPointer->m_id
            && heroPointer->m_remainingMobility <= AI_CONQUERED_HERO_MOBILITY_LIMIT)
            heroPointer->m_remainingMobility = 0;
    }
}

void philAI::RedistributeTroops(
    armyGroup* sourceArmy,
    armyGroup* destinationArmy,
    i32 preserveOne,
    i32 preferFast,
    i32 sourceStrength,
    i32 destinationStrength [[maybe_unused]],
    i32 transferBudget
) {
    i32 howMany;
    i32 bestVal;
    i32 stackValue;
    i32 bestSpeed;
    i32 bestSlot;
    i32 destinationIndex;
    i32 fromIndex;
    b32 again;
    i32 army;

    again = true;
    gbTroopReload = false;
    while (again) {
        if (preserveOne != 0) {
            army = 0;
            for (fromIndex = 0; fromIndex < AI_TOWN_ARMY_SLOTS; fromIndex++) {
                if (sourceArmy->m_creatureTypes[fromIndex] != CREATURE_NONE)
                    army += sourceArmy->m_creatureCounts[fromIndex];
            }
            if (army <= 1)
                return;
        }

        bestSlot = AI_TROOP_EMPTY_SLOT;
        for (fromIndex = 0; fromIndex < AI_TOWN_ARMY_SLOTS; fromIndex++) {
            if (bestSlot == AI_TROOP_EMPTY_SLOT) {
                for (destinationIndex = 0; destinationIndex < AI_TOWN_ARMY_SLOTS;
                     destinationIndex++) {
                    if (sourceArmy->m_creatureTypes[fromIndex] != CREATURE_NONE
                        && sourceArmy->m_creatureTypes[fromIndex]
                               == destinationArmy->m_creatureTypes[destinationIndex]) {
                        bestSlot = fromIndex;
                        break;
                    }
                }
            }
        }

        if (bestSlot == AI_TROOP_EMPTY_SLOT) {
            bestVal = AI_TROOP_REDISTRIBUTION_WORST_VALUE;
            if (preferFast != 0)
                bestSpeed = AI_TROOP_REDISTRIBUTION_MIN_SPEED;
            else
                bestSpeed = AI_TROOP_REDISTRIBUTION_MAX_SPEED;

            for (fromIndex = 0; fromIndex < AI_TOWN_ARMY_SLOTS; fromIndex++) {
                if (sourceArmy->m_creatureTypes[fromIndex] != CREATURE_NONE) {
                    stackValue =
                        sourceArmy->m_creatureCounts[fromIndex]
                        * gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[fromIndex])].fightValue;
                    if ((preferFast != 0
                         && gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[fromIndex])].speed
                                > bestSpeed)
                        || (preferFast == 0
                            && gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[fromIndex])].speed
                                   < bestSpeed)) {
                        bestSpeed =
                            gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[fromIndex])].speed;
                        bestVal = stackValue;
                        bestSlot = fromIndex;
                    } else if (gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[fromIndex])].speed
                                   == bestSpeed
                               && stackValue > bestVal) {
                        bestVal = stackValue;
                        bestSlot = fromIndex;
                    }
                }
            }
        }

        if (bestSlot == AI_TROOP_EMPTY_SLOT) {
            again = false;
        } else if (destinationArmy->CanJoin(sourceArmy->m_creatureTypes[bestSlot])) {
            howMany = static_cast<i32>(static_cast<float>(
                static_cast<double>(transferBudget)
                    / gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[bestSlot])].fightValue
                + AI_TROOP_REDISTRIBUTION_ROUNDING
            ));
            if (howMany > 0) {
                if (howMany > sourceArmy->m_creatureCounts[bestSlot]) {
                    howMany = sourceArmy->m_creatureCounts[bestSlot];
                } else {
                    again = false;
                    if ((howMany
                             >= sourceArmy->m_creatureCounts[bestSlot]
                                    * AI_TROOP_REDISTRIBUTION_STACK_SHARE
                         || howMany >= sourceArmy->m_creatureCounts[bestSlot] - 1)
                        && (sourceArmy->m_creatureCounts[bestSlot] - howMany)
                                   * gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[bestSlot])]
                                         .fightValue
                               < (sourceStrength - transferBudget)
                                     * AI_TROOP_REDISTRIBUTION_REMAINDER_FACTOR) {
                        howMany = sourceArmy->m_creatureCounts[bestSlot];
                    }
                }

                if (preserveOne != 0 && howMany >= army) {
                    howMany = army - 1;
                    again = false;
                }

                if (gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[bestSlot])].fightValue
                        * howMany * AI_TROOP_REDISTRIBUTION_BUDGET_FACTOR
                    > transferBudget) {
                    again = false;
                } else {
                    transferBudget -=
                        gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[bestSlot])].fightValue
                        * howMany;
                }

                destinationArmy->Add(
                    sourceArmy->m_creatureTypes[bestSlot],
                    howMany,
                    AI_TROOP_EMPTY_SLOT
                );
                sourceArmy->m_creatureCounts[bestSlot] -= howMany;
                if (sourceArmy->m_creatureCounts[bestSlot] == 0)
                    sourceArmy->m_creatureTypes[bestSlot] = CREATURE_NONE;
            } else {
                again = false;
            }
        } else {
            again = false;
        }
    }
}

i32 philAI::ChooseGoldOrExperience(i32, i32) {
    return gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] > 4000 ? 1 : 0;
}

void philAI::ChooseEvaluateBattle(
    armyGroup* attackerArmy,
    hero* attackerHero,
    armyGroup* defenderArmy,
    hero* defenderHero,
    i32 isCastle,
    i32 castleId,
    i32 rewardValue,
    i32& outFlag,
    i32& outValue
) {
    i32 defenderRemaining, attackerRemaining, defenderLoss;
    i32 battleValue;
    i32 attackerLoss;
    float winChance;
    i32 unused [[maybe_unused]];

    ProbableOutcomeOfBattle(attackerArmy, attackerHero, defenderArmy, defenderHero, NULL, isCastle, castleId, (defenderHero != NULL ? defenderHero->m_owner : -1), winChance, attackerLoss, defenderLoss, attackerRemaining, defenderRemaining, battleValue);
    battleValue = static_cast<i32>(battleValue + rewardValue * winChance);
    if (battleValue <= 0) {
        outValue = 0;
        outFlag = 0;
    } else {
        outValue = battleValue;
        outFlag = 1;
    }
}

i32 philAI::ChooseToFightForArtifact(
    ArtifactType artifact,
    CreatureType monster,
    i32 quantity [[maybe_unused]]
) {
    i32 remainB;
    i32 remainA;
    i32 lostB;
    i32 lostA;
    i32 artValue;
    i32 index;
    float chance;
    i32 score;
    i32 result;
    artValue = gArtifactBaseRV[H2EnumIndex(artifact)];
    for (index = 0; index < AI_TOWN_ARMY_SLOTS; index++) {
        gpMonGroup->m_creatureTypes[index] = monster;
        gpMonGroup->m_creatureCounts[index] = static_cast<i16>(index == 0);
    }
    ProbableOutcomeOfBattle(
        &gpCurAIHero->m_army,
        gpCurAIHero,
        gpMonGroup,
        NULL,
        NULL,
        0,
        0,
        -1,
        chance,
        lostA,
        lostB,
        remainA,
        remainB,
        result
    );
    score = static_cast<i32>(result + artValue * chance);
    if (score > 0)
        return 1;
    else
        return 0;
}

i32 philAI::NetValueOfArtifact(i32 artifact, i32 goldCost, i32 resourceType, i32 resourceCost) {
    return static_cast<i32>(
        gArtifactBaseRV[artifact]
        - (goldCost * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
           + resourceCost * gafAITurnCostResource[resourceType])
    );
}

i32 philAI::ChooseToPayRansomOnHero(i32) {
    return 1;
}

void philAI::BuildBuilding(town* townPointer, BuildingSlotType building) {
    i32 i;
    i32 cost[H2EnumIndex(RES_COUNT)];
    sprintf(
        gText,
        "Player %d built %s in town %d.\n",
        giCurPlayer,
        GetBuildingName(townPointer->m_type, building),
        townPointer->m_id
    );
    LogStr(gText);
    if (giDebugLevel >= AI_PURCHASE_DEBUG_LEVEL) {
        AiPrint(gText);
        DelayMilli(AI_PURCHASE_DEBUG_DELAY);
    }
    GetBuildingCost(townPointer->m_type, building, cost, townPointer->m_buildState);
    for (i = 0; i < H2EnumIndex(RES_COUNT); i++)
        gpCurPlayer->m_resources[i] -= cost[i];
    townPointer->BuildBuilding(building);
    ShowStatus();
}

void philAI::BuildHero(town* townPointer, i32 availableHeroIndex) {
    hero* newHero;
    i32 townX;
    i32 townY;

    sprintf(
        gText,
        "Player %d built hero in town %d.\n",
        giCurPlayer,
        townPointer->m_id
    );
    LogStr(gText);
    if (giDebugLevel >= AI_PURCHASE_DEBUG_LEVEL) {
        AiPrint(gText);
        DelayMilli(AI_PURCHASE_DEBUG_DELAY);
    }

    gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= gHeroGoldCost;
    gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] =
        gpCurPlayer->m_availableHeroIds[availableHeroIndex];
    gpCurPlayer->m_heroCount++;

    townX = townPointer->m_x;
    townY = townPointer->m_y;
    newHero = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
    gpGame->SetRandomHeroArmies(newHero->m_id, 1);
    newHero->m_lastHeroInteractionTurn = AI_HERO_BUILD_COORDINATE_UNSET;
    newHero->m_lastTownInteractionTurn = AI_HERO_BUILD_COORDINATE_UNSET;
    newHero->m_owner = static_cast<char>(giCurPlayer);
    newHero->m_x = townX;
    newHero->m_y = townY;
    newHero->m_eventFlags = HERO_EVENT_NONE;
    newHero->m_direction = MAP_DIRECTION_EAST;
    newHero->m_remainingMobility = newHero->CalcMobility();
    newHero->m_mobility = newHero->m_remainingMobility;

    newHero->m_locationType = gpGame->m_worldMap.GetCell(townX, townY)->m_triggerType;
    newHero->m_occupiedTown = gpGame->m_worldMap.GetCell(townX, townY)->m_objectMetadata;
    gpGame->m_worldMap.GetCell(townX, townY)->m_triggerType =
        (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION));
    gpGame->m_worldMap.GetCell(townX, townY)->m_objectMetadata =
        gpCurPlayer->m_availableHeroIds[availableHeroIndex];
    gpGame->m_availableHeroes[newHero->m_id] = townPointer->m_owner;

    CheckValidAvailableHeroes();
    SendMapChange(
        MAP_CHANGE_RECRUIT_HERO,
        newHero->m_id,
        static_cast<u8>(newHero->m_x),
        static_cast<u8>(newHero->m_y),
        MAP_CHANGE_CURRENT_PLAYER,
        0,
        0
    );
    townPointer->m_occupyingHeroId = newHero->m_id;
    townPointer->GiveSpells(NULL);

    gpCurPlayer->m_availableHeroIds[availableHeroIndex] =
        static_cast<i8>(gpGame->GetNewHeroId(giCurPlayer, FACTION_ANY, 1));
    gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[availableHeroIndex]] =
        AI_HERO_AVAILABLE_FLAG;
    bHeroBuiltThisTurn = true;
    HeroInteractionAtTown(newHero, townPointer, 0, &iDummy);
    ShowStatus();
}

void philAI::BuildCreature(town* townPointer, i32 dwelling, i32 purchaseCount) {
    float weakestValue;
    i32 monsterCosts[H2EnumIndex(RES_COUNT)];
    b32 hasRoom;
    i32 lowSlot;
    i32 slotIndex;
    float stackValue;
    CreatureType creature;

    sprintf(
        gText,
        "Player %d built %d %s in town %d.\n",
        giCurPlayer,
        purchaseCount,
        GetMonsterName(gDwellingType[H2EnumIndex(townPointer->m_type)][dwelling]),
        townPointer->m_id
    );
    LogStr(gText);
    if (giDebugLevel >= AI_PURCHASE_DEBUG_LEVEL) {
        AiPrint(gText);
        DelayMilli(AI_PURCHASE_DEBUG_DELAY);
    }

    creature = gDwellingType[H2EnumIndex(townPointer->m_type)][dwelling];
    hasRoom = false;
    for (slotIndex = 0; slotIndex < AI_TOWN_ARMY_SLOTS; slotIndex++) {
        if (townPointer->m_army.m_creatureTypes[slotIndex] == CREATURE_NONE
            || townPointer->m_army.m_creatureTypes[slotIndex] == creature) {
            hasRoom = true;
        }
    }

    weakestValue = AI_CREATURE_SELECTION_WORST_VALUE;
    lowSlot = AI_TROOP_EMPTY_SLOT;
    if (hasRoom == 0) {
        for (slotIndex = 0; slotIndex < AI_TOWN_ARMY_SLOTS; slotIndex++) {
            stackValue = static_cast<float>(
                gMonsterDatabase[H2EnumIndex(townPointer->m_army.m_creatureTypes[slotIndex])].fightValue
                * townPointer->m_army.m_creatureCounts[slotIndex]
            );
            if (gMonsterDatabase[H2EnumIndex(townPointer->m_army.m_creatureTypes[slotIndex])].race
                != townPointer->m_type) {
                stackValue = static_cast<float>(stackValue * AI_CREATURE_OFF_RACE_FACTOR);
            }
            stackValue = static_cast<float>(
                stackValue
                * ((gMonsterDatabase[H2EnumIndex(townPointer->m_army.m_creatureTypes[slotIndex])].fightValue
                    + AI_CREATURE_SELECTION_BASE_VALUE)
                   / AI_CREATURE_SELECTION_BASE_VALUE)
            );
            if (stackValue < weakestValue) {
                weakestValue = stackValue;
                lowSlot = slotIndex;
            }
        }
        if (lowSlot == AI_TROOP_EMPTY_SLOT)
            lowSlot = 0;

        gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] +=
            townPointer->m_army.m_creatureCounts[lowSlot]
            * gMonsterDatabase[H2EnumIndex(townPointer->m_army.m_creatureTypes[lowSlot])].cost;
        townPointer->m_army.m_creatureTypes[lowSlot] = CREATURE_NONE;
        townPointer->m_army.m_creatureCounts[lowSlot] = 0;
    }

    GetMonsterCost(creature, monsterCosts);
    for (slotIndex = 0; slotIndex < H2EnumIndex(RES_COUNT); slotIndex++) {
        gpCurPlayer->m_resources[slotIndex] -= purchaseCount * monsterCosts[slotIndex];
    }
    townPointer->m_garrison[dwelling] -= purchaseCount;
    townPointer->m_army.Add(creature, purchaseCount, AI_TROOP_EMPTY_SLOT);
    ShowStatus();
}

i32 philAI::CanBuyBHC(BHC& purchase) {
    i32 j;
    i32 index;
    i32 cost[H2EnumIndex(RES_COUNT)];
    switch (purchase.type) {
        case PURCHASE_BUILDING:
            if (CanBuy(purchase.pTown, purchase.building))
                return 1;
            break;
        case PURCHASE_HERO:
            if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] >= gHeroGoldCost
                && purchase.pTown->m_occupyingHeroId == -1
                && bHeroBuiltThisTurn == 0)
                return 1;
            break;
        case PURCHASE_CREATURE:
            j = H2EnumIndex(gDwellingType[H2EnumIndex(purchase.pTown->m_type)][purchase.what]);
            if (purchase.num > purchase.pTown->m_garrison[purchase.what])
                return 0;
            GetMonsterCost(CreatureType(j), cost);
            for (index = 0; index < H2EnumIndex(RES_COUNT); index++)
                if (gpCurPlayer->m_resources[index] < cost[index] * purchase.num)
                    return 0;
            return 1;
    }
    return 0;
}

i32 philAI::CombatMonsterEvent(
    hero* heroPointer,
    CreatureType monType,
    i32* pCount,
    mapCell* cell [[maybe_unused]]
) {
    i32 stackIndex;
    float defenderLoss;
    float casualtyRatio;
    i32 battleWon;
    i32 total;
    CLEAR_ARMY_GROUP(*gpMonGroup);
    if (*pCount / AI_TOWN_ARMY_SLOTS > 0) {
        for (stackIndex = 0; stackIndex < AI_TOWN_ARMY_SLOTS; stackIndex++) {
            gpMonGroup->m_creatureTypes[stackIndex] = monType;
            gpMonGroup->m_creatureCounts[stackIndex] = static_cast<i16>(*pCount / AI_TOWN_ARMY_SLOTS);
        }
    }
    for (stackIndex = *pCount % AI_TOWN_ARMY_SLOTS - 1; stackIndex >= 0; stackIndex--) {
        gpMonGroup->m_creatureTypes[stackIndex] = monType;
        gpMonGroup->m_creatureCounts[stackIndex]++;
    }
    battleWon = gpPhilAI->QuickCombat(&heroPointer->m_army, heroPointer, gpMonGroup, NULL, 0, 0, casualtyRatio, defenderLoss);
    total = 0;
    for (stackIndex = 0; stackIndex < AI_TOWN_ARMY_SLOTS; stackIndex++)
        total += gpMonGroup->m_creatureCounts[stackIndex];
    *pCount = total;
    if (battleWon != 0)
        return 1;
    return 0;
}

i32 philAI::FightEvent(hero* heroPointer, mapCell* cell, i32 evaluateOnly) {
    i32 combatResult;
    i32 monsterCount;
    float attackerLoss;
    i32 unusedValue [[maybe_unused]];
    i32 stackIndex;
    float defenderLoss;
    CreatureType monsterType;
    i32 battleWon;
    MapObjectType eventType;
    i32 battleValue;
    i32 rewardValue;

    eventType = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
    if (cell->m_objectMetadata == FIGHT_EVENT_EMPTY)
        return 0;

    i32 shipwreckCounts[FIGHT_EVENT_LEVEL_COUNT] = {
        FIGHT_EVENT_SHIPWRECK_COUNT_1,
        FIGHT_EVENT_SHIPWRECK_COUNT_2,
        FIGHT_EVENT_SHIPWRECK_COUNT_3,
        FIGHT_EVENT_SHIPWRECK_COUNT_4
    };
    i32 derelictCounts[FIGHT_EVENT_LEVEL_COUNT] = {
        FIGHT_EVENT_DERELICT_COUNT_1,
        FIGHT_EVENT_DERELICT_COUNT_2,
        FIGHT_EVENT_DERELICT_COUNT_3,
        FIGHT_EVENT_DERELICT_COUNT_4
    };
    i32 graveyardCounts[FIGHT_EVENT_LEVEL_COUNT] = {
        FIGHT_EVENT_GRAVEYARD_COUNT_1,
        FIGHT_EVENT_GRAVEYARD_COUNT_2,
        FIGHT_EVENT_GRAVEYARD_COUNT_3,
        FIGHT_EVENT_GRAVEYARD_COUNT_4
    };

    switch (eventType) {
        case MAP_OBJECT_SHIPWRECK:
            monsterType = CREATURE_GHOST;
            monsterCount =
                shipwreckCounts[cell->m_objectMetadata - FIGHT_EVENT_LEVEL_OFFSET];
            break;
        case MAP_OBJECT_GRAVEYARD:
            monsterType = CREATURE_ZOMBIE;
            monsterCount =
                graveyardCounts[cell->m_objectMetadata - FIGHT_EVENT_LEVEL_OFFSET];
            break;
        default:
            monsterType = CREATURE_SKELETON;
            monsterCount = derelictCounts[cell->m_objectMetadata - FIGHT_EVENT_LEVEL_OFFSET];
            break;
    }

    for (stackIndex = 0; stackIndex < FIGHT_EVENT_STACKS; stackIndex++) {
        gpMonGroup->m_creatureTypes[stackIndex] = monsterType;
        gpMonGroup->m_creatureCounts[stackIndex] = static_cast<i16>(monsterCount);
    }

    rewardValue = 0;
    switch (eventType) {
        case MAP_OBJECT_GRAVEYARD:
            switch (cell->m_objectMetadata) {
                case FIGHT_EVENT_LEVEL_1:
                    rewardValue = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_500_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_2:
                    rewardValue = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_1000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_3:
                    rewardValue = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_3000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_4:
                    rewardValue = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_1000_VALUE
                            * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                        + gpCurPlayer->m_aiData.m_artifactValue
                    );
                    break;
            }
            break;
        case MAP_OBJECT_SHIPWRECK:
            switch (cell->m_objectMetadata) {
                case FIGHT_EVENT_LEVEL_1:
                    rewardValue = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_1000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_2:
                    rewardValue = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_2000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_3:
                    rewardValue = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_5000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_4:
                    rewardValue = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_2000_VALUE
                            * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                        + gpCurPlayer->m_aiData.m_artifactValue
                    );
                    break;
            }
            break;
        case MAP_OBJECT_DERELICT_SHIP:
            switch (cell->m_objectMetadata) {
                case FIGHT_EVENT_LEVEL_1:
                    rewardValue = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_500_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_2:
                    rewardValue = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_1000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_3:
                    rewardValue = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_2000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_4:
                    rewardValue = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_5000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
            }
            break;
    }

    ChooseEvaluateBattle(
        &heroPointer->m_army,
        heroPointer,
        gpMonGroup,
        NULL,
        0,
        0,
        rewardValue,
        battleWon,
        battleValue
    );
    if (evaluateOnly != 0)
        return battleValue;
    if (battleWon != 0
        && (combatResult =
                QuickCombat(&heroPointer->m_army, heroPointer, gpMonGroup, NULL, 0, 0, attackerLoss, defenderLoss))
               != 0) {
        switch (eventType) {
            case MAP_OBJECT_SHIPWRECK:
                switch (cell->m_objectMetadata) {
                    case FIGHT_EVENT_LEVEL_1:
                        gpAdvManager->GiveResource(heroPointer, RES_GOLD, FIGHT_EVENT_REWARD_1000);
                        break;
                    case FIGHT_EVENT_LEVEL_2:
                        gpAdvManager->GiveResource(heroPointer, RES_GOLD, FIGHT_EVENT_REWARD_2000);
                        break;
                    case FIGHT_EVENT_LEVEL_3:
                        gpAdvManager->GiveResource(heroPointer, RES_GOLD, FIGHT_EVENT_REWARD_5000);
                        break;
                    case FIGHT_EVENT_LEVEL_4:
                        gpAdvManager->GiveResource(heroPointer, RES_GOLD, FIGHT_EVENT_REWARD_2000);
                        gpAdvManager->GiveRandomArtifact(heroPointer);
                        break;
                }
                break;
            case MAP_OBJECT_GRAVEYARD:
                switch (cell->m_objectMetadata) {
                    case FIGHT_EVENT_LEVEL_1:
                        gpAdvManager->GiveResource(heroPointer, RES_GOLD, FIGHT_EVENT_REWARD_500);
                        break;
                    case FIGHT_EVENT_LEVEL_2:
                        gpAdvManager->GiveResource(heroPointer, RES_GOLD, FIGHT_EVENT_REWARD_1000);
                        break;
                    case FIGHT_EVENT_LEVEL_3:
                        gpAdvManager->GiveResource(heroPointer, RES_GOLD, FIGHT_EVENT_REWARD_3000);
                        break;
                    case FIGHT_EVENT_LEVEL_4:
                        gpAdvManager->GiveResource(heroPointer, RES_GOLD, FIGHT_EVENT_REWARD_1000);
                        gpAdvManager->GiveRandomArtifact(heroPointer);
                        break;
                }
                break;
            case MAP_OBJECT_DERELICT_SHIP:
                switch (cell->m_objectMetadata) {
                    case FIGHT_EVENT_LEVEL_1:
                        gpAdvManager->GiveResource(heroPointer, RES_GOLD, FIGHT_EVENT_REWARD_500);
                        break;
                    case FIGHT_EVENT_LEVEL_2:
                        gpAdvManager->GiveResource(heroPointer, RES_GOLD, FIGHT_EVENT_REWARD_1000);
                        break;
                    case FIGHT_EVENT_LEVEL_3:
                        gpAdvManager->GiveResource(heroPointer, RES_GOLD, FIGHT_EVENT_REWARD_2000);
                        break;
                    case FIGHT_EVENT_LEVEL_4:
                        gpAdvManager->GiveResource(heroPointer, RES_GOLD, FIGHT_EVENT_REWARD_5000);
                        break;
                }
                break;
        }
        cell->m_objectMetadata = FIGHT_EVENT_EMPTY;
    }
    return 0;
}

i32 philAI::DamageGroup(armyGroup* armyGroupPointer, hero* loser, hero*, float damage) {
    if (damage < AI_QUICK_COMBAT_DEFEAT_THRESHOLD) {
        armyGroupPointer->DamageGroup(damage);
        return 0;
    } else {
        if (loser != NULL)
            gpAdvManager->HeroLoses(loser);
        else
            armyGroupPointer->DamageGroup(damage);
        return 1;
    }
}

void philAI::IncrementHourGlass(void) {
    i32 heroCount = gpCurPlayer->m_heroCount;
    if (heroCount < HOURGLASS_PROSPECTIVE_HERO_LIMIT
        && gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] >= AI_HERO_PURCHASE_GOLD_FLOOR
        && bHeroBuiltThisTurn == 0)
        heroCount++;
    iCurHourGlassPhase++;
    if (heroCount == 1) {
        iCurHourGlassPhase++;
        iCurHourGlassPhase++;
    }
    if (heroCount == HOURGLASS_SECOND_HERO_COUNT && iCurHourGlassPhase != 1)
        iCurHourGlassPhase++;
    if (heroCount == HOURGLASS_THIRD_HERO_COUNT
        && (iCurHourGlassPhase == HOURGLASS_THIRD_HERO_SKIP_FIRST
            || iCurHourGlassPhase == HOURGLASS_THIRD_HERO_SKIP_SECOND))
        iCurHourGlassPhase++;
    if (iCurHourGlassPhase > HOURGLASS_LAST_PHASE)
        iCurHourGlassPhase = HOURGLASS_LAST_PHASE;
}

void philAI::TownEvent(mapCell* cell, hero* heroPointer, i32 x, i32 y) {
    town* pTown;
    i32 quickResult [[maybe_unused]];
    float defenderLoss;
    float attackerLoss;
    hero* defenderHero;
    CombatResult combatResult;
    i32 owner [[maybe_unused]];

    pTown = GetCastleSlot(cell->m_objectMetadata);
    owner = giCurPlayer;
    gpAdvManager->DemobilizeCurrHero();

    if (pTown->m_owner != giCurPlayer) {
        if (pTown->HasGarrison()) {
            if (pTown->m_owner < 0 || gbHumanPlayer[H2EnumIndex(pTown->m_owner)] == 0) {
                quickResult = QuickCombat(
                    &heroPointer->m_army,
                    heroPointer,
                    &pTown->m_army,
                    NULL,
                    TOWN_EVENT_USE_GARRISON,
                    pTown->m_id,
                    defenderLoss,
                    attackerLoss
                );
            } else {
                defenderHero = pTown->m_occupyingHeroId == TOWN_EVENT_NO_HERO
                                     ? NULL
                                     : GetHeroSlot(pTown->m_occupyingHeroId);

                combatResult = gpAdvManager->DoCombat(
                    x,
                    y,
                    heroPointer,
                    &heroPointer->m_army,
                    pTown,
                    defenderHero,
                    &pTown->m_army,
                    x,
                    y,
                    AI_BATTLE_NO_PLAYER,
                    TOWN_EVENT_USE_GARRISON
                );
                if (combatResult == COMBAT_RESULT_ATTACKER) {
                    gpGame->ClaimTown(pTown->m_id, giCurPlayer, 0);
                    giHumanTownConquered = pTown->m_id;
                }
            }
        } else {
            gpGame->ClaimTown(pTown->m_id, giCurPlayer, 0);
        }
    }

    if (pTown->m_owner == giCurPlayer && heroPointer->m_x == x && heroPointer->m_y == y) {
        pTown->m_occupyingHeroId = gpCurPlayer->CurrentHero();
        heroPointer->m_locationType = (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE));
        heroPointer->m_occupiedTown = pTown->m_id;
        HeroInteractionAtTown(heroPointer, pTown, 0, &iDummy);
    }

    gpAdvManager->MobilizeCurrHero(0);
    pTown->GiveSpells(NULL);
}

i32 philAI::ComputeUpgradeValue(CreatureType baseCreatureType, CreatureType upgradedCreatureType) {
    i32 count = gpCurAIHero->CreatureTypeCount(baseCreatureType);
    if (count == 0)
        return 0;
    i32 result = static_cast<i32>(
        static_cast<float>(
            count * (gMonsterDatabase[H2EnumIndex(upgradedCreatureType)].fightValue - gMonsterDatabase[H2EnumIndex(baseCreatureType)].fightValue)
        )
        * gpCurPlayer->m_aiData.m_upgradeValueWeight
    );
    if (gpCurAIHero->CreatureTypeCount(upgradedCreatureType) != 0)
        result = static_cast<i32>(result * AI_UPGRADE_EXISTING_STACK_FACTOR);
    return result;
}

i32 philAI::ComputeValueOfSS(
    hero* heroPointer,
    HeroSecondarySkill skill,
    HeroSkillLevel level
) {
    i32 index;
    i32 score;
    i32 armyValue;
    i32 archerValue;
    i32 totalValue;
    float ratio;
    i32 troopValue;

    score = gSSValues[H2EnumIndex(skill)][H2EnumIndex(level) - SECONDARY_SKILL_LEVEL_OFFSET];
    armyValue = FightValueOfStack(&heroPointer->m_army, heroPointer, 1);
    if (skill != HERO_SKILL_ESTATES) {
        score = static_cast<i32>(
            score
            * (static_cast<float>(armyValue) / gpCurPlayer->m_aiData.m_upgradeValueWeight
                   / AI_SECONDARY_SKILL_FIGHT_SCALE
               + AI_SECONDARY_SKILL_BASE_FACTOR)
        );
    }

    switch (skill) {
        case HERO_SKILL_NAVIGATION:
            if (heroPointer->IsEmbarked())
                score = static_cast<i32>(score * AI_SECONDARY_SKILL_NAVIGATION_FACTOR);
            break;
        case HERO_SKILL_ARCHERY:
            archerValue = 0;
            totalValue = archerValue;
            for (index = 0; index < ARMY_GROUP_SLOT_COUNT; index++) {
                if (heroPointer->m_army.m_creatureTypes[index] != CREATURE_NONE) {
                    troopValue = heroPointer->m_army.m_creatureCounts[index]
                        * gMonsterDatabase[H2EnumIndex(heroPointer->m_army.m_creatureTypes[index])].fightValue;
                    totalValue += troopValue;
                    if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(heroPointer->m_army.m_creatureTypes[index])].attributes) & (MONSTER_FLAGS_SHOOTER)))) {
                        archerValue += troopValue;
                    }
                }
            }

            ratio = static_cast<float>(archerValue) / (static_cast<float>(totalValue));
            score = static_cast<i32>(
                score
                * (ratio / AI_SECONDARY_SKILL_ARCHERY_SHARE + AI_SECONDARY_SKILL_ARCHERY_BASE)
            );
            break;
        case HERO_SKILL_WISDOM:
        case HERO_SKILL_MYSTICISM:
            if (!heroPointer->HasArtifact(ARTIFACT_MAGIC_BOOK)
                || heroPointer->Stats(HERO_PRIMARY_KNOWLEDGE) < SECONDARY_SKILL_MINIMUM_KNOWLEDGE) {
                score = static_cast<i32>(score * AI_SECONDARY_SKILL_BASE_FACTOR);
            }
            break;
    }
    return score;
}

i32 philAI::ComputeValueOfFreeSS(
    hero* heroPointer, HeroSecondarySkill skill
) {
    if (heroPointer->m_secondarySkills[H2EnumIndex(skill)] != HERO_SKILL_LEVEL_NONE
        || heroPointer->m_secondarySkillCount >= HERO_SECONDARY_SKILL_CAPACITY)
        return 0;
    else
        return ComputeValueOfSS(heroPointer, skill, HERO_SKILL_LEVEL_BASIC);
}

i32 philAI::ManaRefreshValue(hero* heroPointer, i32 level) {
    i32 maximumSpellPoints;
    i32 refreshValue;
    float missingFraction;
    i32 deficit;

    refreshValue = 0;
    maximumSpellPoints = HERO_NORMAL_SPELL_POINTS(*heroPointer) * level;
    deficit = maximumSpellPoints - heroPointer->m_spellPoints;
    if (deficit <= 0)
        return 0;

    missingFraction = deficit / (static_cast<float>(maximumSpellPoints));
    if (deficit > 0)
        refreshValue = static_cast<i32>(deficit * 5 * missingFraction);
    return refreshValue;
}

i32 philAI::ValueOfEventAtPosition(i32 x, i32 y, i32 immediate, i32* liveChance) {
    mapCell* theCell;
    i32 oracleState [[maybe_unused]];
    i32 armySlot;
    i32 artifactState [[maybe_unused]][3];
    b32 freeFlag;
    i32 cellState [[maybe_unused]][3];
    H2EnumStorage<ResourceType, i32> resourceType;
    i32 prize;
    i32 purchaseState [[maybe_unused]][3];
    i32 amount;
    i32 pyramidState [[maybe_unused]];
    i32 bBattleWon;
    i32 ruinState [[maybe_unused]];
    i32 numToBuy;
    i32 lampState [[maybe_unused]];
    CreatureType buyCreature;
    i32 combatState [[maybe_unused]];
    i32 eventRV;
    i32 townState [[maybe_unused]][9];
    i32 i;
    i32 gateX;
    mapCell* exitCell;
    i32 exitRV;
    i32 gateY;
    i32 chosenExitY [[maybe_unused]];
    i32 chosenExitX [[maybe_unused]];
    i32 costList[H2EnumIndex(RES_COUNT)];
    i32 positionValue;
    i32 exitLiveChance;
    i32 bestRV;

    if (!immediate && *(gaiHeroEventStratRVOfPos + x + y * MAP_WIDTH) != RV_UNSET)
        return *(gaiHeroEventStratRVOfPos + x + y * MAP_WIDTH);

    gbReduceByReload = true;
    gbReduceByBerserk = true;
    *liveChance = POSITION_FULL_CHANCE;
    eventRV = 0;
    theCell = gpAdvManager->GetCell(x, y);

    if (gpCurPlayer->m_ultimateArtifactHintChance > 15 && gpCurPlayer->m_ultimateArtifactHintX == x
        && gpCurPlayer->m_ultimateArtifactHintY == y) {
        eventRV = gUltArtifactAvgValue * (gpCurPlayer->m_ultimateArtifactHintChance - 15) / 100;
    } else if ((H2EnumIndex((theCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
        switch (theCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
            case MAP_OBJECT_MONSTER:
                eventRV = EvaluateMonsterEvent(
                    CreatureType(theCell->m_objectIndex),
                    theCell->m_objectMetadata,
                    liveChance
                );
                break;
            case MAP_OBJECT_RESOURCE:
                resourceType = theCell->m_objectIndex >> 1;
                switch (resourceType) {
                    case RES_GOLD:
                        eventRV = static_cast<i32>(
                            gafAITurnCostResource[H2EnumIndex(resourceType)]
                            * (theCell->m_objectMetadata * AI_GOLD_RESOURCE_MULTIPLIER)
                        );
                        break;
                    default:
                        eventRV = static_cast<i32>(
                            gafAITurnCostResource[H2EnumIndex(resourceType)] * theCell->m_objectMetadata
                        );
                        break;
                }
                break;
            case MAP_OBJECT_TREASURE_CHEST:
                eventRV = static_cast<i32>(
                    AI_TREASURE_CHEST_GOLD_AMOUNT * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                );
                break;
            case MAP_OBJECT_HERO_INTERACTION:
                eventRV = EvaluateHeroEvent(theCell->m_objectMetadata, x, y, immediate, liveChance);
                break;
            case MAP_OBJECT_CASTLE:
                eventRV = EvaluateTownEvent(theCell->m_objectMetadata, x, y, immediate, liveChance);
                break;
            case MAP_OBJECT_CAMPFIRE:
                eventRV = static_cast<i32>(
                    AI_CAMPFIRE_GOLD_AMOUNT * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    + (gafAITurnCostResource[H2EnumIndex(RES_WOOD)] + gafAITurnCostResource[H2EnumIndex(RES_ORE)]
                       + gafAITurnCostResource[H2EnumIndex(RES_CRYSTAL)]
                       + gafAITurnCostResource[H2EnumIndex(RES_SULFUR)]
                       + gafAITurnCostResource[H2EnumIndex(RES_MERCURY)]
                       + gafAITurnCostResource[H2EnumIndex(RES_GEMS)])
                          / AI_CAMPFIRE_AVERAGE_DIVISOR * AI_CAMPFIRE_RESOURCE_AMOUNT
                );
                break;
            case MAP_OBJECT_ARTIFACT:
                eventRV = EvaluateArtifactEvent(
                    ArtifactType(theCell->m_objectIndex >> 1),
                    H2EnumIndex(theCell->m_objectMetadata)
                );
                break;
            case MAP_OBJECT_ALCHEMIST_LAB:
            case MAP_OBJECT_MINE:
            case MAP_OBJECT_SAWMILL:
                eventRV = EvaluateMineEvent(theCell->m_objectMetadata, x, y, liveChance);
                break;
            case MAP_OBJECT_OBELISK:
                if (gpGame->m_obeliskVisitors[theCell->m_objectMetadata - 1] & giCurPlayerBit)
                    eventRV = 0;
                else
                    eventRV = gpCurPlayer->m_aiData.m_obeliskValue;
                break;
            case MAP_OBJECT_OASIS:
                if (!(gpCurAIHero->m_eventFlags & HERO_EVENT_OASIS))
                    eventRV = static_cast<i32>(AI_OASIS_VALUE_FACTOR * gpCurAIHero->m_aiFightValue);
                break;
            case MAP_OBJECT_BUOY:
                if (!(gpCurAIHero->m_eventFlags & HERO_EVENT_BUOY)
                    && giCurAIHeroMorale < ARMY_GROUP_MORALE_MAX)
                    eventRV = static_cast<i32>(AI_MORALE_LUCK_SITE_VALUE_FACTOR * gpCurAIHero->m_aiFightValue);
                break;
            case MAP_OBJECT_TEMPLE:
                if (!(H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_TEMPLE))) && giCurAIHeroMorale < 3)
                    eventRV = static_cast<i32>(
                        AI_TEMPLE_VALUE_FACTOR * gpCurAIHero->m_aiFightValue
                    );
                break;
            case MAP_OBJECT_FAERIE_RING:
                if (!(H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_FAERIE_RING))) && giCurAIHeroLuck < 3)
                    eventRV = static_cast<i32>(
                        AI_MORALE_LUCK_SITE_VALUE_FACTOR * gpCurAIHero->m_aiFightValue
                    );
                break;
            case MAP_OBJECT_IDOL:
                if (!(H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_IDOL))) && giCurAIHeroLuck < 3)
                    eventRV = static_cast<i32>(
                        AI_MORALE_LUCK_SITE_VALUE_FACTOR * gpCurAIHero->m_aiFightValue
                    );
                break;
            case MAP_OBJECT_FOUNTAIN:
                if (!(gpCurAIHero->m_eventFlags & HERO_EVENT_FOUNTAIN) && giCurAIHeroLuck < 3)
                    eventRV = static_cast<i32>(
                        AI_MORALE_LUCK_SITE_VALUE_FACTOR * gpCurAIHero->m_aiFightValue
                    );
                break;
            case MAP_OBJECT_WATERING_HOLE:
                if (!(H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_WATERING_HOLE))))
                    eventRV = static_cast<i32>(
                        AI_WATERING_HOLE_VALUE_FACTOR * gpCurAIHero->m_aiFightValue
                    );
                break;
            case MAP_OBJECT_SHRINE_FIRST_CIRCLE:
            case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
            case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
                if (gpCurAIHero->Stats(HERO_PRIMARY_KNOWLEDGE) > 0
                    && gpCurAIHero->HasArtifact(ARTIFACT_MAGIC_BOOK)
                    && !gpCurAIHero->HasSpell(SpellType(theCell->m_objectMetadata - 1))) {
                    if (H2EnumIndex(gsSpellInfo[theCell->m_objectMetadata - 1].level)
                        <= H2EnumIndex(gpCurAIHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_WISDOM)])
                               + HERO_BASE_LEARNABLE_SPELL_LEVEL) {
                        eventRV = gsSpellInfo[theCell->m_objectMetadata - 1].aiValue;
                        if ((H2EnumIndex((gsSpellInfo[theCell->m_objectMetadata - 1].attributes) & (SPELL_INFO_ATTRIBUTE_POWER)))) {
                            eventRV = static_cast<i32>(
                                eventRV
                                * (gpCurAIHero->Stats(HERO_PRIMARY_KNOWLEDGE)
                                           <= BATTLE_STAT_TABLE_MAX_INDEX
                                       ? gfStatPower[gpCurAIHero->Stats(HERO_PRIMARY_KNOWLEDGE)]
                                       : gfStatPower[BATTLE_STAT_TABLE_MAX_INDEX])
                            );
                        }
                    }
                } else
                    eventRV = 0;
                break;
            case MAP_OBJECT_GAZEBO:
                if (gpCurAIHero->m_gazeboVisits & (1U << theCell->m_objectMetadata))
                    eventRV = 0;
                else
                    eventRV =
                        static_cast<i32>(gpCurAIHero->m_aiFightValue * AI_GAZEBO_VALUE_FACTOR);
                break;
            case MAP_OBJECT_TREE_OF_KNOWLEDGE:
                eventRV = 0;
                if (!(gpCurAIHero->m_treeKnowledgeVisits
                      & (1U << (theCell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK)))) {
                    switch (theCell->m_objectMetadata >> 6) {
                        case 1:
                            eventRV = static_cast<i32>(
                                gpCurAIHero->m_aiFightValue * AI_TREE_KNOWLEDGE_VALUE_FACTOR
                            );
                            break;
                        case 2:
                            if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)]
                                >= static_cast<i32>(AI_TREE_KNOWLEDGE_GOLD_COST)) {
                                eventRV = static_cast<i32>(
                                    gpCurAIHero->m_aiFightValue * AI_TREE_KNOWLEDGE_VALUE_FACTOR
                                );
                                eventRV = static_cast<i32>(
                                    eventRV
                                    - AI_TREE_KNOWLEDGE_GOLD_COST
                                          * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                                );
                            }
                            break;
                        case 3:
                            if (gpCurPlayer->m_resources[H2EnumIndex(RES_GEMS)]
                                >= static_cast<i32>(AI_TREE_KNOWLEDGE_GEM_COST)) {
                                eventRV = static_cast<i32>(
                                    gpCurAIHero->m_aiFightValue * AI_TREE_KNOWLEDGE_VALUE_FACTOR
                                );
                                eventRV = static_cast<i32>(
                                    eventRV
                                    - AI_TREE_KNOWLEDGE_GEM_COST
                                          * gafAITurnCostResource[H2EnumIndex(RES_GEMS)]
                                );
                            }
                            break;
                    }
                    if (eventRV < 0)
                        eventRV = 0;
                }
                break;
            case MAP_OBJECT_WINDMILL:
                if (theCell->m_objectMetadata == 99) {
                    eventRV = 0;
                } else {
                    memset(costList, 0, sizeof(costList));
                    costList[theCell->m_objectMetadata] = 2;
                    eventRV = RVConversion(costList);
                }
                break;
            case MAP_OBJECT_MAGIC_GARDEN:
                if (!theCell->m_objectMetadata)
                    eventRV = 0;
                else if (theCell->m_objectMetadata - 1 == H2EnumIndex(RES_GOLD))
                    eventRV = static_cast<i32>(
                        AI_MAGIC_GARDEN_GOLD_AMOUNT * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                else
                    eventRV =
                        static_cast<i32>(
                            5.0f * gafAITurnCostResource[theCell->m_objectMetadata - 1]
                        );
                break;
            case MAP_OBJECT_FLOTSAM:
                eventRV =
                    static_cast<i32>(AI_FLOTSAM_GOLD_AMOUNT * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]);
                eventRV =
                    static_cast<i32>(AI_FLOTSAM_WOOD_AMOUNT * gafAITurnCostResource[H2EnumIndex(RES_WOOD)]);
                break;
            case MAP_OBJECT_SEA_CHEST:
                if (theCell->m_objectMetadata & MAP_EVENT_ARTIFACT_GUARD_FLAG)
                    eventRV = static_cast<i32>(
                        AI_SEA_CHEST_ARTIFACT_GOLD_AMOUNT * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                        + gArtifactBaseRV[theCell->m_objectMetadata & MAP_EVENT_ARTIFACT_ID_MASK]
                    );
                else if (theCell->m_objectMetadata == 1)
                    eventRV = static_cast<i32>(
                        AI_SEA_CHEST_LARGE_GOLD_AMOUNT * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                else
                    eventRV = static_cast<i32>(
                        AI_SEA_CHEST_SMALL_GOLD_AMOUNT * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                if (eventRV <= 0)
                    eventRV = AI_SEA_CHEST_MINIMUM_VALUE;
                break;
            case MAP_OBJECT_WAGON:
            case MAP_OBJECT_LEAN_TO:
                if (!theCell->m_objectMetadata)
                    eventRV = 0;
                else if (theCell->m_objectMetadata & MAP_EVENT_ARTIFACT_CONDITION_FLAG) {
                    eventRV = gArtifactBaseRV
                        [theCell->m_objectMetadata & MAP_EVENT_ARTIFACT_CONDITION_ID_MASK];
                } else {
                    resourceType = (theCell->m_objectMetadata & AI_EVENT_RESOURCE_TYPE_MASK) - 1;
                    amount =
                        (theCell->m_objectMetadata & ARTIFACT_EVENT_RESOURCE_MASK)
                        >> ARTIFACT_EVENT_RESOURCE_SHIFT;
                    eventRV = static_cast<i32>(gafAITurnCostResource[H2EnumIndex(resourceType)] * amount);
                }
                break;
            case MAP_OBJECT_ANCIENT_LAMP:
                buyCreature = CREATURE_GENIE;
                freeFlag = false;
                goto creature_purchase;
            case MAP_OBJECT_TREE_CITY:
                buyCreature = CREATURE_SPRITE;
                freeFlag = false;
                goto creature_purchase;
            case MAP_OBJECT_WATCH_TOWER:
                buyCreature = CREATURE_ORC;
                freeFlag = true;
                goto creature_purchase;
            case MAP_OBJECT_TREE_HOUSE:
                buyCreature = CREATURE_SPRITE;
                freeFlag = true;
                goto creature_purchase;
            case MAP_OBJECT_EXCAVATION:
                buyCreature = CREATURE_SKELETON;
                freeFlag = true;
                goto creature_purchase;
            case MAP_OBJECT_HALFLING_HOLE:
                buyCreature = CREATURE_HALFLING;
                freeFlag = true;
                goto creature_purchase;
            case MAP_OBJECT_RUINS:
                buyCreature = CREATURE_MEDUSA;
                freeFlag = false;
                goto creature_purchase;
            case MAP_OBJECT_TROLL_BRIDGE:
                if (theCell->m_objectMetadata & MAP_EVENT_ARTIFACT_GUARD_FLAG) {
                    eventRV = 0;
                } else {
                    buyCreature = CREATURE_TROLL;
                    freeFlag = false;
                    goto creature_purchase;
                }
                break;
            case MAP_OBJECT_DRAGON_CITY:
                if (theCell->m_objectMetadata & MAP_EVENT_ARTIFACT_GUARD_FLAG) {
                    eventRV = 0;
                } else {
                    buyCreature = CREATURE_RED_DRAGON;
                    freeFlag = false;
                    goto creature_purchase;
                }
                break;
            case MAP_OBJECT_CITY_OF_DEAD:
                if (theCell->m_objectMetadata & MAP_EVENT_ARTIFACT_GUARD_FLAG) {
                    eventRV = 0;
                } else {
                    buyCreature = CREATURE_POWER_LICH;
                    freeFlag = false;
                    goto creature_purchase;
                }
                break;
            case MAP_OBJECT_CAVE:
                buyCreature = CREATURE_CENTAUR;
                freeFlag = true;
                goto creature_purchase;
            case MAP_OBJECT_ARCHER_HOUSE:
                buyCreature = CREATURE_ARCHER;
                freeFlag = true;
                goto creature_purchase;
            case MAP_OBJECT_GOBLIN_HUT:
                buyCreature = CREATURE_GOBLIN;
                freeFlag = true;
                goto creature_purchase;
            case MAP_OBJECT_PEASANT_HUT:
                buyCreature = CREATURE_PEASANT;
                freeFlag = true;
                goto creature_purchase;
            case MAP_OBJECT_DWARF_COTTAGE:
            case MAP_OBJECT_SIRENS:
                buyCreature = CREATURE_DWARF;
                freeFlag = true;
                goto creature_purchase;
            case MAP_OBJECT_DESERT_TENT:
                buyCreature = CREATURE_NOMAD;
                freeFlag = false;
                goto creature_purchase;
            case MAP_OBJECT_WAGON_CAMP:
                buyCreature = CREATURE_ROGUE;
                freeFlag = false;
            creature_purchase:
                EvaluateOneTimeCreaturePurchase(
                    buyCreature,
                    theCell->m_objectMetadata,
                    freeFlag,
                    numToBuy,
                    eventRV,
                    armySlot
                );
                gbReduceByReload = false;
                break;
            case MAP_OBJECT_SHIPWRECK_SURVIVOR:
                eventRV = gArtifactBaseRV[theCell->m_objectMetadata];
                if (eventRV < 125)
                    eventRV = 125;
                break;
            case MAP_OBJECT_SKELETON:
                if (theCell->m_objectMetadata == 1)
                    eventRV = 0;
                else
                    eventRV =
                        gArtifactBaseRV[theCell->m_objectMetadata - SKELETON_ARTIFACT_OFFSET];
                break;
            case MAP_OBJECT_GRAVEYARD:
            case MAP_OBJECT_SHIPWRECK:
            case MAP_OBJECT_DERELICT_SHIP:
                eventRV = FightEvent(gpCurAIHero, theCell, 1);
                break;
            case MAP_OBJECT_PYRAMID:
                if (!theCell->m_objectMetadata) {
                    eventRV = 0;
                } else {
                    i = theCell->m_objectMetadata - 1;
                    prize = static_cast<i32>(
                        gsSpellInfo[i].aiValue
                        * ((H2EnumIndex((gsSpellInfo[i].attributes) & (SPELL_INFO_ATTRIBUTE_POWER)))
                               ? (gpCurAIHero->Stats(HERO_PRIMARY_SPELL_POWER)
                                          <= BATTLE_STAT_TABLE_MAX_INDEX
                                      ? gfBattleStat[gpCurAIHero->Stats(HERO_PRIMARY_SPELL_POWER)]
                                      : gfBattleStat[BATTLE_STAT_TABLE_MAX_INDEX])
                               : 1.0f)
                        * gpCurPlayer->m_aiData.m_upgradeValueWeight
                    );
                    for (i = 0; i < AI_TOWN_ARMY_SLOTS; i++) {
                        gpMonGroup->m_creatureTypes[i] = CREATURE_ROYAL_MUMMY;
                        gpMonGroup->m_creatureCounts[i] = 10;
                    }
                    ChooseEvaluateBattle(
                        &gpCurAIHero->m_army,
                        gpCurAIHero,
                        gpMonGroup,
                        NULL,
                        0,
                        0,
                        prize,
                        bBattleWon,
                        eventRV
                    );
                    if (!bBattleWon)
                        eventRV = -50;
                }
                break;
            case MAP_OBJECT_DAEMON_CAVE:
                if (theCell->m_objectMetadata == 1)
                    eventRV = 0;
                else {
                    eventRV = static_cast<i32>(
                        gpCurAIHero->m_aiFightValue * AI_DAEMON_FIGHT_VALUE_SHARE
                            * AI_EVENT_VALUE_SCALE
                        + (gpCurAIHero->m_aiFightValue * AI_DAEMON_SECONDARY_FIGHT_VALUE_SHARE
                               * AI_EVENT_VALUE_SCALE
                           + gpCurPlayer->m_aiData.m_artifactValue)
                        + (gpCurAIHero->m_aiFightValue * AI_DAEMON_FIGHT_VALUE_SHARE
                               * AI_EVENT_VALUE_SCALE
                           + AI_DAEMON_GOLD_VALUE_FACTOR * gafAITurnCostResource[H2EnumIndex(RES_GOLD)])
                        + gafAITurnCostResource[H2EnumIndex(RES_GOLD)] * AI_DAEMON_GOLD_PENALTY
                    );
                    if (theCell->m_objectMetadata == 5
                        && gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] < AI_DAEMON_CAVE_GOLD_REQUIRED)
                        eventRV = -100;
                }
                break;
            case MAP_OBJECT_ABANDONED_MINE:
                prize = static_cast<i32>(
                    gMineCharacteristics[H2EnumIndex(RES_GOLD)] * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    * *(gaiTurnValueOfMine + x + y * MAP_WIDTH)
                );
                for (i = 0; i < AI_TOWN_ARMY_SLOTS; i++) {
                    gpMonGroup->m_creatureTypes[i] =
                        gpGame->m_mines[theCell->m_objectMetadata].guardianType;
                    gpMonGroup->m_creatureCounts[i] = static_cast<i16>(
                        gpGame->m_mines[theCell->m_objectMetadata].guardianCount / 5
                    );
                }
                ChooseEvaluateBattle(
                    &gpCurAIHero->m_army,
                    gpCurAIHero,
                    gpMonGroup,
                    NULL,
                    0,
                    0,
                    prize,
                    bBattleWon,
                    eventRV
                );
                if (!bBattleWon)
                    eventRV = -50;
                break;
            case MAP_OBJECT_STONE_LITHS:
            case MAP_OBJECT_WHIRLPOOL:
                if (!bEvaluatingTravelGates) {
                    eventRV = 0;
                    break;
                }
                bEvaluatingTravelGates = false;
                bestRV = AI_TRAVEL_GATE_INITIAL_VALUE;
                for (gateY = 0; gateY < MAP_HEIGHT; gateY++) {
                    for (gateX = 0; gateX < MAP_WIDTH; gateX++) {
                        exitCell = gpAdvManager->GetCell(gateX, gateY);
                        if (exitCell->m_triggerType == theCell->m_triggerType
                            && exitCell->m_objectIndex == theCell->m_objectIndex
                            && MANHATTAN_LENGTH(x - gateX, y - gateY)
                                   > AI_TRAVEL_GATE_EXIT_RADIUS) {
                            exitRV = StrategicValueOfPosition(
                                gateX,
                                gateY,
                                0,
                                0,
                                &exitLiveChance,
                                AI_TRAVEL_GATE_EXIT_DEPTH
                            );
                            exitRV = static_cast<i32>(exitRV * AI_TRAVEL_GATE_EXIT_SCALE);
                            if (exitRV > bestRV) {
                                bestRV = exitRV;
                                chosenExitX = gateX;
                                chosenExitY = gateY;
                            }
                        }
                    }
                }
                positionValue = StrategicValueOfPosition(
                    gpCurAIHero->m_x,
                    gpCurAIHero->m_y,
                    0,
                    0,
                    &exitLiveChance,
                    AI_TRAVEL_GATE_CURRENT_DEPTH
                );
                if (bestRV > positionValue + AI_TRAVEL_GATE_PENALTY)
                    eventRV = bestRV - positionValue - AI_TRAVEL_GATE_PENALTY;
                else if (!immediate)
                    eventRV = 0;
                else
                    eventRV = -AI_TRAVEL_GATE_PENALTY;
                bEvaluatingTravelGates = true;
                gbReduceByReload = false;
                break;
            case MAP_OBJECT_FORT:
                if (gpCurAIHero->m_fortVisits & (1U << theCell->m_objectMetadata))
                    eventRV = 0;
                else
                    eventRV = static_cast<i32>(
                        gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR
                    );
                break;
            case MAP_OBJECT_WITCH_DOCTOR_HUT:
                if (gpCurAIHero->m_witchDoctorVisits & (1U << theCell->m_objectMetadata))
                    eventRV = 0;
                else
                    eventRV = static_cast<i32>(
                        gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR
                    );
                break;
            case MAP_OBJECT_STANDING_STONES:
                if (gpCurAIHero->m_standingStoneVisits & (1U << theCell->m_objectMetadata))
                    eventRV = 0;
                else
                    eventRV = static_cast<i32>(
                        gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR
                    );
                break;
            case MAP_OBJECT_MERCENARY_CAMP:
                if (gpCurAIHero->m_mercenaryCampVisits & (1U << theCell->m_objectMetadata))
                    eventRV = 0;
                else
                    eventRV = static_cast<i32>(
                        gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR
                    );
                break;
            case MAP_OBJECT_XANADU:
                if (gpCurAIHero->m_xanaduVisits & (1U << theCell->m_objectMetadata))
                    eventRV = 0;
                else {
                    if (gpCurAIHero->m_level
                                + H2EnumIndex(gpCurAIHero->m_secondarySkills
                                          [H2EnumIndex(HERO_SKILL_DIPLOMACY)])
                                      * 2
                            >= 10)
                        eventRV =
                            static_cast<i32>(
                                gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR
                                * AI_XANADU_VALUE_MULTIPLE
                            );
                    else
                        eventRV = 0;
                }
                break;
            case MAP_OBJECT_LIGHTHOUSE:
                if (gpGame->m_mines[theCell->m_objectMetadata].owner == gpCurAIHero->m_owner
                    || OnMySide(gpGame->m_mines[theCell->m_objectMetadata].owner))
                    eventRV = 0;
                else
                    eventRV = 1000;
                break;
            case MAP_OBJECT_WATER_WHEEL:
                eventRV = static_cast<i32>(
                    theCell->m_objectMetadata * AI_WATER_WHEEL_GOLD_AMOUNT
                    * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                );
                break;
            case MAP_OBJECT_BOAT:
                gbActualBoatFound = true;
                eventRV = 90;
                break;
            case MAP_OBJECT_BOTTLE:
                eventRV = 105;
                break;
            case MAP_OBJECT_HILL_FORT:
                eventRV = ComputeUpgradeValue(CREATURE_ORC, CREATURE_ORC_CHIEF)
                          + ComputeUpgradeValue(CREATURE_OGRE, CREATURE_OGRE_LORD)
                          + ComputeUpgradeValue(CREATURE_DWARF, CREATURE_BATTLE_DWARF);
                break;
            case MAP_OBJECT_FREEMANS_FOUNDRY:
                eventRV =
                    ComputeUpgradeValue(CREATURE_IRON_GOLEM, CREATURE_STEEL_GOLEM)
                    + ComputeUpgradeValue(CREATURE_PIKEMAN, CREATURE_VETERAN_PIKEMAN)
                    + ComputeUpgradeValue(CREATURE_SWORDSMAN, CREATURE_MASTER_SWORDSMAN);
                break;
            case MAP_OBJECT_MAGIC_WELL:
                eventRV = ManaRefreshValue(gpCurAIHero, 1);
                break;
            case MAP_OBJECT_ARTESIAN_SPRING:
                if (!theCell->m_objectMetadata)
                    eventRV = 0;
                else
                    eventRV = ManaRefreshValue(gpCurAIHero, 2);
                break;
            case MAP_OBJECT_WITCH_HUT:
                eventRV = ComputeValueOfFreeSS(
                    gpCurAIHero,
                    static_cast<HeroSecondarySkill>(theCell->m_objectMetadata)
                );
                break;
            case MAP_OBJECT_SIGN:
            case MAP_OBJECT_ORACLE:
            case MAP_OBJECT_TRADING_POST:
            case MAP_OBJECT_SPHINX:
            case MAP_OBJECT_TAR_PIT:
            case MAP_OBJECT_MAGELLAN_MAPS:
            case MAP_OBJECT_OBSERVATION_TOWER:
                eventRV = 0;
                break;
            case MAP_OBJECT_EXPANSION_OBJECT:
                eventRV = EvaluateGenericSite(theCell);
                break;
            case MAP_OBJECT_BARRIER:
                eventRV = EvaluateBarrier(theCell);
                break;
            case MAP_OBJECT_TRAVELER_TENT:
                eventRV = EvaluatePassword(theCell);
                break;
            case MAP_OBJECT_EXPANSION_DWELLING:
                eventRV = EvaluateRecruitSite(theCell);
                break;
            case MAP_OBJECT_JAIL:
                eventRV = EvaluateJail(theCell);
                break;
            default:
                sprintf(
                    gText,
                    "AI encountered object type %d and doesn't know how to deal with it.   "
                        "Tell Phil",
                    H2EnumIndex(theCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                );
                NormalDialog(gText, 1);
                eventRV = 0;
                break;
        }
    } else if (!(MAP_EXTRA_AT_WFIRST(x, y) & giCurPlayerBit)) {
        eventRV = 5;
    }

    if (gbTroopReload && gbReduceByReload)
        eventRV = static_cast<i32>(eventRV * fReduceFactor);
    if (gbBerserk && gbReduceByBerserk)
        eventRV = static_cast<i32>(eventRV * fBerserkFactor);
    if (!immediate) {
        if (eventRV > 0 && (MAP_EXTRA_AT(x, y) & H2EnumIndex(MAP_EXTRA_ADJACENT_MONSTER))
            && (theCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) != MAP_OBJECT_MONSTER)
            eventRV = 0;
        if (eventRV < 0
            && (theCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) != MAP_OBJECT_HERO_INTERACTION)
            eventRV = 0;
        else if (eventRV > 32000)
            eventRV = 32000;
        else if (eventRV < -32000)
            eventRV = -32000;
        *(gaiHeroEventStratRVOfPos + x + y * MAP_WIDTH) =
            static_cast<i16>(eventRV);
    }
    return eventRV;
}


i32 philAI::EvaluateGenericSite(mapCell* cell) {
    i32 level [[maybe_unused]];
    i32 value;
    i32 badArtifacts;
    GenericSiteType genericType;
    i32 unusedValue [[maybe_unused]];
    i32 slot;
    i32 count;
    CreatureType unitType;
    i32 armyWorth;
    i32 removedQuantity;

    badArtifacts = 0;
    genericType = static_cast<GenericSiteType>(cell->m_objectMetadata);
    genericType = static_cast<GenericSiteType>(H2EnumIndex(genericType) & H2EnumIndex(GENERIC_SITE_TYPE_MASK));
    level = cell->m_objectMetadata;
    level >>= GENERIC_SITE_LEVEL_SHIFT;
    value = 0;

    switch (genericType) {
        case GENERIC_SITE_ALCHEMIST_TOWER:
            for (slot = 0; slot < HERO_ARTIFACT_SLOT_COUNT;
                 slot++) {
                if (IsCursedItem(gpCurAIHero->m_artifacts[slot]))
                    badArtifacts++;
            }
            if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] > AI_GENERIC_SITE_GOLD_THRESHOLD) {
                value = badArtifacts * AI_GENERIC_SITE_CURSED_ARTIFACT_VALUE;
            }
            break;
        case GENERIC_SITE_ARENA:
            if (!(gpCurAIHero->m_eventFlags & HERO_EVENT_ARENA)) {
                value =
                    static_cast<i32>(gpCurAIHero->m_aiFightValue * AI_GENERIC_SITE_ARENA_VALUE);
            }
            break;
        case GENERIC_SITE_MERMAID:
            if (!(gpCurAIHero->m_eventFlags & HERO_EVENT_MERMAID)
                && giCurAIHeroLuck < AI_GENERIC_SITE_MAX_LUCK) {
                value = static_cast<i32>(
                    AI_GENERIC_SITE_MERMAID_VALUE * gpCurAIHero->m_aiFightValue
                );
            }
            break;
        case GENERIC_SITE_HUT_OF_MAGI:
        case GENERIC_SITE_EYE_OF_MAGI:
            break;
        case GENERIC_SITE_SIRENS:
            if (!(gpCurAIHero->m_eventFlags & HERO_EVENT_SIRENS)) {
                armyWorth = 0;
                for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT;
                     slot++) {
                    unitType = gpCurAIHero->m_army.m_creatureTypes[slot];
                    if (unitType != CREATURE_NONE) {
                        count = gpCurAIHero->m_army.m_creatureCounts[slot];
                        removedQuantity =
                            static_cast<i32>(count * AI_GENERIC_SITE_SIRENS_ARMY_REMAINDER);
                        armyWorth += gMonsterDatabase[H2EnumIndex(unitType)].hitPoints
                                      * (count - removedQuantity);
                    }
                }
                value = static_cast<i32>(armyWorth * gpCurAIHero->m_aiFightValue);
            }
            break;
        case GENERIC_SITE_STABLES:
            value = ComputeUpgradeValue(CREATURE_CAVALRY, CREATURE_CHAMPION);
            if (!(gpCurAIHero->m_eventFlags & HERO_EVENT_STABLES)) {
                value = static_cast<i32>(
                    value
                    + (AI_GENERIC_SITE_WEEK_END - gpGame->m_day) * AI_GENERIC_SITE_DAY_VALUE
                          * gpCurAIHero->m_aiFightValue
                );
            }
            break;
    }
    return value;
}

i32 philAI::EvaluateBarrier(mapCell* cell) {
    i32 color = cell->m_tentColor;
    color &= EVENT_BARRIER_COLOR_MASK;
    if (PLAYER_HAS_VISITED_TENT(*gpCurPlayer, color))
        return 5000;
    else
        return 0;
}

i32 philAI::EvaluatePassword(mapCell* cell) {
    i32 color = cell->m_tentColor;
    color &= EVENT_BARRIER_COLOR_MASK;
    if (!PLAYER_HAS_VISITED_TENT(*gpCurPlayer, color))
        return 2500;
    else
        return 0;
}

i32 philAI::EvaluateRecruitSite(mapCell* cell) {
    i32 purchaseCount;
    RecruitSiteType recruitmentSiteType;
    i32 creatureType;
    i32 purchaseValue;
    i32 replacementSlot;
    i32 unusedValue [[maybe_unused]];
    i16 availableCount;
    recruitmentSiteType = static_cast<RecruitSiteType>(cell->m_tentColor);
    recruitmentSiteType =
        static_cast<RecruitSiteType>(H2EnumIndex(recruitmentSiteType) & EVENT_RECRUIT_TYPE_MASK);
    availableCount = cell->m_tentColor;
    availableCount >>= EVENT_RECRUIT_COUNT_SHIFT;
    creatureType = 0;
    purchaseValue = 0;
    switch (recruitmentSiteType) {
        case RECRUITMENT_SITE_BARROW_MOUNDS:
            creatureType = H2EnumIndex(CREATURE_GHOST);
            break;
        case RECRUITMENT_SITE_EARTH_ALTAR:
            creatureType = H2EnumIndex(CREATURE_EARTH_ELEMENTAL);
            break;
        case RECRUITMENT_SITE_AIR_ALTAR:
            creatureType = H2EnumIndex(CREATURE_AIR_ELEMENTAL);
            break;
        case RECRUITMENT_SITE_FIRE_ALTAR:
            creatureType = H2EnumIndex(CREATURE_FIRE_ELEMENTAL);
            break;
        case RECRUITMENT_SITE_WATER_ALTAR:
            creatureType = H2EnumIndex(CREATURE_WATER_ELEMENTAL);
            break;
    }
    EvaluateOneTimeCreaturePurchase(CreatureType(creatureType), availableCount, 0, purchaseCount, purchaseValue, replacementSlot);
    gbReduceByReload = false;
    return purchaseValue;
}

i32 philAI::EvaluateJail(mapCell*) {
    return 10000;
}

void InitAIMapVars(void) {
    CloseAIMapVars();
    SVSearchArray.Init();
    gaiLiveChanceOfPos = static_cast<i16*>(H2_ALLOC(MAP_HEIGHT * MAP_WIDTH * 2));
    gaiHeroStrategicRVOfPos =
        static_cast<i16*>(H2_ALLOC(MAP_HEIGHT * MAP_WIDTH * 2));
    gaiHeroEventStratRVOfPos =
        static_cast<i16*>(H2_ALLOC(MAP_HEIGHT * MAP_WIDTH * 2));
    gaiTurnValueOfMine = static_cast<i8*>(H2_ALLOC(MAP_HEIGHT * MAP_WIDTH));
    gaiEnemyHeroReachable = static_cast<i8*>(H2_ALLOC(MAP_HEIGHT * MAP_WIDTH));
}

void CloseAIMapVars(void) {
    if (gaiLiveChanceOfPos != NULL)
        H2_FREE(gaiLiveChanceOfPos);
    if (gaiHeroStrategicRVOfPos != NULL)
        H2_FREE(gaiHeroStrategicRVOfPos);
    if (gaiHeroEventStratRVOfPos != NULL)
        H2_FREE(gaiHeroEventStratRVOfPos);
    if (gaiTurnValueOfMine != NULL)
        H2_FREE(gaiTurnValueOfMine);
    if (gaiEnemyHeroReachable != NULL)
        H2_FREE(gaiEnemyHeroReachable);
    gaiLiveChanceOfPos = NULL;
    gaiHeroStrategicRVOfPos = NULL;
    gaiHeroEventStratRVOfPos = NULL;
    gaiTurnValueOfMine = NULL;
    gaiEnemyHeroReachable = NULL;
    SVSearchArray.Close();
}

i32 OnMySide(i32 player) {
    if (player != SIDE_NO_PLAYER
        && (player == giCurPlayer
            || (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ROLAND
                && gpGame->m_campaignScenario + SIDE_CAMPAIGN_SCENARIO_OFFSET
                       == SIDE_CAMPAIGN_SCENARIO_TEN
                && player != SIDE_PRIMARY_PLAYER)
            || (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
                && gpGame->m_campaignScenario + SIDE_CAMPAIGN_SCENARIO_OFFSET
                       == SIDE_CAMPAIGN_SCENARIO_ELEVEN
                && player != SIDE_PRIMARY_PLAYER)
            || (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_SIDE
                && ((gpGame->m_mapHeader.victoryConditionValue == SIDE_VICTORY_SPECIAL_VALUE
                     && player != SIDE_PRIMARY_PLAYER)
                    || (gpGame->m_mapHeader.victoryConditionValue != SIDE_VICTORY_SPECIAL_VALUE
                        && ((gpGame->m_players[giCurPlayer].m_color
                                 < gpGame->m_mapHeader.victorySideThreshold
                             && gpGame->m_players[player].m_color
                                    < gpGame->m_mapHeader.victorySideThreshold)
                            || (gpGame->m_players[giCurPlayer].m_color
                                    >= gpGame->m_mapHeader.victorySideThreshold
                                && gpGame->m_players[player].m_color
                                       >= gpGame->m_mapHeader.victorySideThreshold)))))
            || (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ROLAND
                && gpGame->m_campaignScenario + SIDE_CAMPAIGN_SCENARIO_OFFSET
                       == SIDE_CAMPAIGN_SCENARIO_NINE
                && gpGame->m_players[player].m_color != SIDE_FIRST_COLOR
                && gpGame->m_players[player].m_color != SIDE_FOURTH_COLOR))) {
        return 1;
    } else
        return 0;
}

i32 philAI::EvaluateArtifactEvent(ArtifactType artifact, i32 eventData) {
    i32 artValue;
    i32 index;
    float chance;
    i32 result;
    i32 extraValue [[maybe_unused]];
    i32 guardRV;
    i32 plainVal;
    i32 junkVal [[maybe_unused]];
    i32 newOutcome;
    i32 unusedRV [[maybe_unused]];
    i32 lostA;
    i32 lostB;
    i32 remainA;
    i32 remainB;

    if (gpCurAIHero->NumArtifacts() == HERO_ARTIFACT_SLOT_COUNT)
        return 0;

    result = 0;
    artValue = gArtifactBaseRV[H2EnumIndex(artifact)];

    if (artifact == ARTIFACT_SPELL_SCROLL) {
        if (gpCurAIHero->HasSpell(SpellType(eventData)))
            return artValue;
        else
            return artValue + gsSpellInfo[eventData].aiValue;
    }

    plainVal = artValue;
    if (eventData & MAP_EVENT_ARTIFACT_GUARD_FLAG) {
        for (index = 0; index < ARMY_GROUP_SLOT_COUNT; index++) {
            gpMonGroup->m_creatureTypes[index] =
                static_cast<CreatureType>(eventData & AI_ARTIFACT_EVENT_CREATURE_MASK);
            gpMonGroup->m_creatureCounts[index] = static_cast<i16>(
                gpMonGroup->m_creatureTypes[index] == CREATURE_ROGUE
                    ? AI_ARTIFACT_EVENT_GUARD_ROGUE_COUNT
                    : (index == 0)
            );
        }
        ProbableOutcomeOfBattle(
            &gpCurAIHero->m_army,
            gpCurAIHero,
            gpMonGroup,
            NULL,
            NULL,
            0,
            0,
            -1,
            chance,
            lostA,
            lostB,
            remainA,
            remainB,
            newOutcome
        );
        guardRV = static_cast<i32>(newOutcome + gArtifactBaseRV[H2EnumIndex(artifact)] * chance);
        if (guardRV < 0)
            guardRV = 0;
        result = guardRV;
    } else {
        switch (static_cast<ArtifactEventMode>(eventData & ARTIFACT_EVENT_MODE_MASK)) {
            case ARTIFACT_EVENT_MODE_PICKUP:
                result = plainVal;
                break;
            case ARTIFACT_EVENT_MODE_WISDOM:
                if (gpCurAIHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_WISDOM)]
                    != HERO_SKILL_LEVEL_NONE)
                    result = plainVal;
                else
                    result = 0;
                break;
            case ARTIFACT_EVENT_MODE_LEADERSHIP:
                if (gpCurAIHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_LEADERSHIP)]
                    != HERO_SKILL_LEVEL_NONE)
                    result = plainVal;
                else
                    result = 0;
                break;
            case ARTIFACT_EVENT_MODE_UNKNOWN_2:
                break;
            case ARTIFACT_EVENT_MODE_GOLD:
                result = NetValueOfArtifact(H2EnumIndex(artifact), ARTIFACT_EVENT_GOLD_COST, 0, 0);
                break;
            case ARTIFACT_EVENT_MODE_RESOURCE_3:
                result = NetValueOfArtifact(
                    H2EnumIndex(artifact),
                    ARTIFACT_EVENT_RESOURCE_3_GOLD_COST,
                    (eventData & ARTIFACT_EVENT_RESOURCE_MASK)
                        >> ARTIFACT_EVENT_RESOURCE_SHIFT,
                    ARTIFACT_EVENT_RESOURCE_3_AMOUNT
                );
                break;
            case ARTIFACT_EVENT_MODE_RESOURCE_5:
                result = NetValueOfArtifact(
                    H2EnumIndex(artifact),
                    ARTIFACT_EVENT_RESOURCE_5_GOLD_COST,
                    (eventData & ARTIFACT_EVENT_RESOURCE_MASK)
                        >> ARTIFACT_EVENT_RESOURCE_SHIFT,
                    ARTIFACT_EVENT_RESOURCE_5_AMOUNT
                );
                break;
        }
    }
    return result;
}

i32 philAI::EvaluateMineEvent(i32 mineIndex, i32 x, i32 y, i32* liveChance) {
    i32 outcomeValue;
    i32 mineGuardianCount;
    i32 result = 0;
    i32 attackerLoss;
    i32 stackIndex;
    float winChance;
    i32 defenderLoss;
    i32 attackerRemaining;
    i32 defenderRemaining;
    i32 mineValue;

    if (gpGame->m_mineOwners[mineIndex] == gpCurAIHero->m_owner
        || OnMySide(gpGame->m_mineOwners[mineIndex]))
        return result;

    if (gpGame->m_mines[mineIndex].guardianType != CREATURE_NONE) {
        mineGuardianCount = gpGame->m_mines[mineIndex].guardianCount;
        CLEAR_ARMY_GROUP(*gpMonGroup);

        if (mineGuardianCount / ARMY_GROUP_SLOT_COUNT > 0) {
            for (stackIndex = 0; stackIndex < ARMY_GROUP_SLOT_COUNT; stackIndex++) {
                gpMonGroup->m_creatureTypes[stackIndex] =
                    gpGame->m_mines[mineIndex].guardianType;
                gpMonGroup->m_creatureCounts[stackIndex] =
                    static_cast<i16>(mineGuardianCount / ARMY_GROUP_SLOT_COUNT);
            }
        }
        for (stackIndex = mineGuardianCount % ARMY_GROUP_SLOT_COUNT - 1; stackIndex >= 0;
             stackIndex--) {
            gpMonGroup->m_creatureTypes[stackIndex] =
                gpGame->m_mines[mineIndex].guardianType;
            gpMonGroup->m_creatureCounts[stackIndex]++;
        }

        ProbableOutcomeOfBattle(
            &gpCurAIHero->m_army,
            gpCurAIHero,
            gpMonGroup,
            NULL,
            NULL,
            0,
            0,
            -1,
            winChance,
            attackerLoss,
            defenderLoss,
            attackerRemaining,
            defenderRemaining,
            outcomeValue
        );
        *liveChance = static_cast<i32>(winChance);
        result = outcomeValue;
    }

    if (gbIAmGreatest && gpGame->m_mineOwners[mineIndex] >= 0
        && !gbHumanPlayer[gpGame->m_mineOwners[mineIndex]])
        return result;
    else {
        mineValue = static_cast<i32>(
            static_cast<float>(
                gMineCharacteristics[H2EnumIndex(gpGame->m_mines[mineIndex].resourceType)]
            )
            * gafAITurnCostResource[H2EnumIndex(gpGame->m_mines[mineIndex].resourceType)]
            * *(gaiTurnValueOfMine + x + y * MAP_WIDTH)
        );
        if (gpGame->m_mineOwners[mineIndex] >= 0) {
            mineValue = static_cast<i32>(mineValue * (gbHumanPlayer[gpGame->m_mineOwners[mineIndex]] ? gfAttackHumanBonus : gfAttackComputerBonus));
        }
        result += mineValue;
    }
    return result;
}

i32 philAI::EvaluateMonsterEvent(CreatureType monsterType, i32 eventData, i32* liveChance) {
    i32 defenderRemaining;
    i32 attackerRemaining;
    i32 unusedPurchaseValue;
    i32 defenderLoss;
    i32 attackerLoss;
    i32 stackIndex;
    i32 purchaseCost;
    float winChance;
    i32 willJoin;
    float strengthRatio;
    i32 result;
    i32 monsterCount;
    i32 outcomeValue;

    monsterCount = eventData & H2EnumIndex(MAP_MONSTER_COUNT_MASK);
    willJoin = eventData & H2EnumIndex(MAP_MONSTER_GUARD_FLAG);
    result = 0;
    CLEAR_ARMY_GROUP(*gpMonGroup);
    if (monsterCount / ARMY_GROUP_SLOT_COUNT > 0) {
        for (stackIndex = 0; stackIndex < ARMY_GROUP_SLOT_COUNT; stackIndex++) {
            gpMonGroup->m_creatureTypes[stackIndex] = monsterType;
            gpMonGroup->m_creatureCounts[stackIndex] =
                static_cast<i16>(monsterCount / ARMY_GROUP_SLOT_COUNT);
        }
    }
    for (stackIndex = monsterCount % ARMY_GROUP_SLOT_COUNT - 1; stackIndex >= 0;
         stackIndex--) {
        gpMonGroup->m_creatureTypes[stackIndex] = monsterType;
        gpMonGroup->m_creatureCounts[stackIndex]++;
    }

    ProbableOutcomeOfBattle(
        &gpCurAIHero->m_army,
        gpCurAIHero,
        gpMonGroup,
        NULL,
        NULL,
        0,
        0,
        -1,
        winChance,
        attackerLoss,
        defenderLoss,
        attackerRemaining,
        defenderRemaining,
        outcomeValue
    );
    EvaluateOneTimeCreaturePurchase(
        monsterType,
        monsterCount,
        1,
        purchaseCost,
        attackerLoss,
        unusedPurchaseValue
    );
    strengthRatio =
        static_cast<float>(gpPhilAI->FightValueOfStack(&gpCurAIHero->m_army, gpCurAIHero, 0))
        / static_cast<float>(monsterCount * gMonsterDatabase[H2EnumIndex(monsterType)].fightValue);

    if (willJoin && strengthRatio > AI_MONSTER_JOIN_RATIO
        && !gpCurAIHero->HasArtifact(ARTIFACT_HIDEOUS_MASK)
        && gpCurAIHero->m_army.CanJoin(monsterType) && monsterType != CREATURE_GHOST
        && !IS_ELEMENTAL_CREATURE(monsterType)) {
        *liveChance = POSITION_FULL_CHANCE;
        *liveChance = static_cast<i32>(
            winChance * AI_MONSTER_JOIN_CHANCE_SCALE + AI_MONSTER_JOIN_CHANCE_BASE
        );
        if (gpCurAIHero->m_army.CanJoin(monsterType))
            result = attackerLoss;
        else
            result = 0;
        result = static_cast<i32>(
            result * AI_MONSTER_JOIN_PURCHASE_WEIGHT
            + outcomeValue * AI_MONSTER_JOIN_OUTCOME_WEIGHT
        );
    } else if (strengthRatio > AI_MONSTER_OVERWHELMING_RATIO) {
        if (gpCurAIHero->GetSSLevel(HERO_SKILL_NECROMANCY) != 0)
            result = 120;
        else
            result = 0;
        result += monsterCount * gMonsterDatabase[H2EnumIndex(monsterType)].hitPoints;
    } else {
        *liveChance = static_cast<i32>(winChance * AI_MONSTER_FIGHT_CHANCE_SCALE);
        result = outcomeValue;
    }

    if (result < 0)
        gbReduceByReload = false;
    return result;
}

i32 philAI::EvaluateHeroEvent(i32 heroId, i32 x, i32 y, i32 mode, i32* liveChance) {

    float winChance;
    i32 result;
    i32 scratch [[maybe_unused]];
    i32 attackerLost;
    i32 defenderLost;
    i32 aliveA;
    i32 aliveB;
    i32 townValue;
    hero* otherHero;
    town* defTown;
    i32 townId;
    armyGroup* townGroup;

    if (gpGame->m_availableHeroes[heroId] == gpCurAIHero->m_owner) {
        if (mode == EVENT_MODE_IGNORE)
            result = 0;
        else if (mode == EVENT_MODE_AVOID)
            result = EVENT_SEVERE_PENALTY;
        else
            result = EVENT_FRIENDLY_PENALTY;

        if (giCurTurn - gpCurAIHero->m_lastHeroInteractionTurn >= EVENT_INTERACTION_AGE
            || gpGame->GetHero(heroId)->m_id != gpCurAIHero->m_lastInteractionHeroId) {
            HeroInteractionAtHero(gpCurAIHero, gpGame->GetHero(heroId), 1, &result);
        }
        gaiHeroLiveChance[heroId] = POSITION_FULL_CHANCE;
    } else if (OnMySide(gpGame->m_availableHeroes[heroId])) {
        if (mode == EVENT_MODE_IGNORE)
            result = 0;
        else if (mode == EVENT_MODE_AVOID)
            result = EVENT_SEVERE_PENALTY;
        else
            result = EVENT_ALLIED_PENALTY;
    } else if (gbIAmGreatest && !gbHumanPlayer[gpGame->m_availableHeroes[heroId]]) {
        result = 0;
        *liveChance = POSITION_FULL_CHANCE;
    } else {
        result = 0;
        townValue = 0;
        defTown = NULL;
        townGroup = NULL;
        otherHero = gpGame->GetHero(heroId);
        if (otherHero->m_locationType == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))) {
            defTown = gpGame->GetTown(otherHero->m_occupiedTown);
            townGroup = &defTown->m_army;
            townValue = ValueOfTown(defTown);
            townId = defTown->m_id;
            if (defTown->m_owner >= 0) {
                townValue = static_cast<i32>(
                    townValue
                    * ((EVENT_EARLY_TURN_DIFFICULTY_STEP - gpGame->m_playerCount)
                           * AI_EVENT_TOWN_PLAYER_FACTOR
                           * (gbHumanPlayer[H2EnumIndex(defTown->m_owner)] ? gfAttackHumanBonus
                                                              : gfAttackComputerBonus)
                       + AI_EVENT_VALUE_BASE_FACTOR)
                );
            }
        } else {
            townId = TOWN_ID_NONE;
        }

        ProbableOutcomeOfBattle(
            &gpCurAIHero->m_army,
            gpCurAIHero,
            &otherHero->m_army,
            otherHero,
            townGroup,
            townGroup != NULL,
            townId,
            otherHero->m_owner,
            winChance,
            attackerLost,
            defenderLost,
            aliveA,
            aliveB,
            result
        );
        *liveChance = static_cast<i32>(winChance * AI_EVENT_CERTAIN_CHANCE);
        if (townValue > 0)
            result = static_cast<i32>(result + townValue * winChance);

        if (gbHumanPlayer[H2EnumIndex(otherHero->m_owner)] && result > EVENT_HUMAN_VALUE_THRESHOLD) {
            if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_WARRIOR)
                result *= 2;
            else
                result = static_cast<i32>(result * AI_EVENT_HUMAN_VALUE_FACTOR);
        }

        if (winChance > AI_EVENT_CERTAIN_ODDS)
            gaiHeroLiveChance[heroId] = POSITION_FULL_CHANCE;
        else if (winChance > AI_EVENT_HIGH_ODDS)
            gaiHeroLiveChance[heroId] = static_cast<i16>(winChance * AI_EVENT_HIGH_CHANCE_SCALE);
        else if (winChance > AI_EVENT_GOOD_ODDS)
            gaiHeroLiveChance[heroId] = static_cast<i16>(winChance * AI_EVENT_GOOD_CHANCE_SCALE);
        else if (winChance > AI_EVENT_POOR_ODDS)
            gaiHeroLiveChance[heroId] = static_cast<i16>(winChance * AI_EVENT_POOR_CHANCE_SCALE);
        else if (winChance > AI_EVENT_BAD_ODDS)
            gaiHeroLiveChance[heroId] = static_cast<i16>(winChance * AI_EVENT_BAD_CHANCE_SCALE);
        else
            gaiHeroLiveChance[heroId] = static_cast<i16>(winChance * AI_EVENT_CERTAIN_CHANCE);
        if (gaiHeroLiveChance[heroId] > POSITION_FULL_CHANCE)
            gaiHeroLiveChance[heroId] = POSITION_FULL_CHANCE;

        if (mode == EVENT_MODE_IGNORE && winChance < AI_EVENT_GOOD_ODDS)
            result = static_cast<i32>(result * (3.0f - winChance * 2.0f));
        if (mode == EVENT_MODE_IGNORE && winChance < AI_EVENT_BAD_ODDS)
            result = static_cast<i32>(result * (2.0f - winChance * 2.0f));

        if (result < 0)
            gbReduceByReload = false;
        gbReduceByBerserk = false;
        if (result > 0
            && giCurTurn < EVENT_EARLY_TURN_BASE
                               - H2EnumIndex(gpGame->m_difficulty) * EVENT_EARLY_TURN_DIFFICULTY_STEP
            && !(MAP_EXTRA_AT_WFIRST(x, y) & giCurPlayerBit)) {
            result = 0;
        }
    }
    return result;
}

i32 philAI::EvaluateTownEvent(i32 townId, i32 x, i32 y, i32 mode, i32* liveChance) {
    i32 townValue;
    i32 remainB;
    i32 remainA;
    i32 lostB;
    i32 lostA;
    float fWin;
    i32 eventVal;
    town* pTownRec;
    i32 result;

    eventVal = 0;
    pTownRec = gpGame->GetTown(townId);

    if (gpGame->m_townOwners[townId] == gpCurAIHero->m_owner) {
        if (pTownRec->m_occupyingHeroId != TOWN_ID_NONE) {
            if (mode == EVENT_MODE_IGNORE)
                eventVal = 0;
            else
                eventVal = EVENT_FRIENDLY_PENALTY;
        } else {
            eventVal = 0;
            if (giCurTurn - gpCurAIHero->m_lastTownInteractionTurn >= EVENT_INTERACTION_AGE
                || pTownRec->m_id != gpCurAIHero->m_visitedTownId) {
                HeroInteractionAtTown(gpCurAIHero, pTownRec, 1, &eventVal);
                eventVal = static_cast<i32>(eventVal * gfHeroInteractionBonus[gpCurAIHero->m_id]);
            }
        }
        gbReduceByReload = false;
    } else if (OnMySide(gpGame->m_townOwners[townId])) {
        if (mode == EVENT_MODE_IGNORE)
            eventVal = 0;
        else
            eventVal = EVENT_ALLIED_PENALTY;
    } else if (gbIAmGreatest && gpGame->m_townOwners[townId] >= 0
               && !gbHumanPlayer[gpGame->m_townOwners[townId]]) {
        eventVal = 0;
    } else if (gpGame->m_difficulty < DIFFICULTY_EXPERT
               && giCurTurn < TOWN_EARLY_TURN_BASE
                                  - H2EnumIndex(gpGame->m_difficulty) * TOWN_EARLY_TURN_DIFFICULTY_STEP
               && !(MAP_EXTRA_AT_WFIRST(x, y) & giCurPlayerBit)) {
        eventVal = 0;
    } else {
        townValue = ValueOfTown(pTownRec);
        if (gpGame->m_castleRecs[townId].OccupyingHero() != TOWN_ID_NONE) {
            ProbableOutcomeOfBattle(
                &gpCurAIHero->m_army,
                gpCurAIHero,
                &gpGame->m_heroRecs[pTownRec->OccupyingHero()].m_army,
                &gpGame->m_heroRecs[pTownRec->OccupyingHero()],
                &pTownRec->m_army,
                1,
                townId,
                pTownRec->m_owner,
                fWin,
                lostA,
                lostB,
                remainA,
                remainB,
                result
            );
        } else if (pTownRec->HasGarrison()) {
            ProbableOutcomeOfBattle(
                &gpCurAIHero->m_army,
                gpCurAIHero,
                &pTownRec->m_army,
                NULL,
                NULL,
                1,
                townId,
                pTownRec->m_owner,
                fWin,
                lostA,
                lostB,
                remainA,
                remainB,
                result
            );
        } else {
            fWin = 1.0f;
            result = 0;
        }
        *liveChance = static_cast<i32>(fWin * AI_EVENT_CERTAIN_CHANCE);

        if (pTownRec->m_owner >= 0) {
            townValue = static_cast<i32>(
                townValue
                * (((EVENT_EARLY_TURN_DIFFICULTY_STEP - gpGame->m_playerCount)
                        * AI_EVENT_TOWN_PLAYER_FACTOR
                    + AI_TOWN_EVENT_VALUE_BASE_FACTOR)
                   * (gbHumanPlayer[H2EnumIndex(pTownRec->m_owner)] ? gfAttackHumanBonus
                                                       : gfAttackComputerBonus))
            );
        }
        eventVal = static_cast<i32>(townValue * fWin + result);
        if (gpGame->m_townOwners[townId] != TOWN_OWNER_NONE)
            gbReduceByBerserk = false;
    }

    if (pTownRec->m_owner != TOWN_OWNER_NONE && gbHumanPlayer[H2EnumIndex(pTownRec->m_owner)]
        && eventVal > EVENT_HUMAN_VALUE_THRESHOLD) {
        if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_WARRIOR)
            eventVal = static_cast<i32>(eventVal * AI_TOWN_PRIMARY_HUMAN_VALUE_FACTOR);
        else
            eventVal = static_cast<i32>(eventVal * AI_TOWN_OTHER_HUMAN_VALUE_FACTOR);
    }
    return eventVal;
}


float fFirstWeekTownFV = 0.0f;
i32 iVepCacheHits = 0;
i32 iTotalVepHits = 0;
b32 giShowComputerRoute = false;
i16* gaiLiveChanceOfPos = NULL;
i16* gaiHeroStrategicRVOfPos = NULL;
i16* gaiHeroEventStratRVOfPos = NULL;
i8* gaiTurnValueOfMine = NULL;
i8* gaiEnemyHeroReachable = NULL;
i32l glLastStartTick = 0;
i32l glCurTicks = 0;
i32l glTotalTicks = 0;
hero* gpCurAIHero = NULL;
float gfAttackHumanBonus = 2.0f;
float gfAttackComputerBonus = 0.8f;
i32 iLastFrameRateTimer = 0;
b32 bSVSearchArrayInUse = false;
b32 bEvaluatingTravelGates = true;
b32 gbReduceByBerserk;
float fBerserkFactor;
i32 giCurPlayer;
i8 giBuildShipyard[GAME_PLAYER_COUNT];
i32 giMaxHeroesForThisPlayer;
i8 giBuildBoat[GAME_PLAYER_COUNT];
float fReduceFactor;
u8 giCurPlayerBit;
i32 giBestShipyardDist;
b32 bHeroBuiltThisTurn;
i16 gaiHeroLiveChance[GAME_HERO_COUNT];
i32 giHumanTownConquered;
i32 giCurTurn;
i32 costTemp[H2EnumIndex(RES_COUNT)];
i32 iAlphaMale;
i32 iDummy;
b32 gbPossibleShipyardFound;
float gafAITurnCostResource[H2EnumIndex(RES_COUNT)];
i32 iCurPlaceToVisit;
i32 giBestShipyardId;
b32 gbActualBoatFound;
u8 giCurWatchPlayerBit;
playerData* gpCurPlayer;
float gfHeroInteractionBonus[GAME_HERO_COUNT];
b32 gbBerserk;
i32 giCurAIHeroMorale;
i8 giBuildBoatStuffTurn[GAME_PLAYER_COUNT];
i32 iPlacesVisited[ADVMGR_PLACE_VISIT_COUNT][ADVMGR_PLACE_COORDINATE_COUNT];
b32 gbReduceByReload;
b32 gbTroopReload;
i32 giCurAIHeroLuck;
b32 gbActualShipyardFound;
