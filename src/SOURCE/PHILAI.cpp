#include <Ints.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/PHILAI.h>
#include <BASE/Misc.h>
#include <PLATFORM/Runtime.h>
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

enum class AIArtifactEventMode : i32 {
    AI_ARTIFACT_EVENT_VALUE               = 1,
    AI_ARTIFACT_EVENT_NO_VALUE            = 2,
    AI_ARTIFACT_EVENT_PAY_GOLD            = 3,
    AI_ARTIFACT_EVENT_REQUIRES_WISDOM     = 4,
    AI_ARTIFACT_EVENT_REQUIRES_LEADERSHIP = 5,
    AI_ARTIFACT_EVENT_PAY_RESOURCE_THREE  = 6,
    AI_ARTIFACT_EVENT_PAY_RESOURCE_FIVE   = 7
};
using enum AIArtifactEventMode;

typedef enum AIArtifactEventConstant {
    AI_ARTIFACT_EVENT_MODE_MASK             = 0xf,
    AI_EVENT_RESOURCE_TYPE_MASK             = 0xf,
    AI_ARTIFACT_EVENT_CREATURE_MASK         = 0xff,
    AI_ARTIFACT_EVENT_RESOURCE_MASK         = 0xf0,
    AI_ARTIFACT_EVENT_RESOURCE_SHIFT        = 4,
    AI_ARTIFACT_EVENT_RESOURCE_THREE_AMOUNT = 3,
    AI_ARTIFACT_EVENT_RESOURCE_FIVE_AMOUNT  = 5,
    AI_ARTIFACT_EVENT_GUARD_ROGUE_COUNT     = 10,
    AI_ARTIFACT_EVENT_GOLD_COST             = 2000,
    AI_ARTIFACT_EVENT_RESOURCE_THREE_COST   = 2500,
    AI_ARTIFACT_EVENT_RESOURCE_FIVE_COST    = 3000
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

typedef enum AIShipwreckSurvivorConstant {
    SKELETON_ARTIFACT_METADATA_OFFSET = 2
} AIShipwreckSurvivorConstant;

typedef enum AIHeroInteractionConstant {
    HERO_INTERACTION_HERO_COUNT                = 2,
    HERO_INTERACTION_COMBAT_PRIMARY_STAT_COUNT = 2,
    HERO_INTERACTION_PRIMARY_STAT_COUNT        = 4,
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
    SECONDARY_SKILL_ARMY_SLOTS        = 5,
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
    CREATURE_PURCHASE_ARMY_SLOT_COUNT  = 5,
    CREATURE_PURCHASE_VALUE_LIMIT      = 999999,
    CREATURE_PURCHASE_DWELLING_COUNT   = 12,
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
    INTERACTION_TURN_UNSET                    = -99,
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
    i32 idx;
    i32 node;

    if (x == -1 || y == -1)
        return;
    for (node = 0; node < MAP_WIDTH; node++) {
        for (idx = 0; idx < MAP_HEIGHT; idx++) {
            if (resetAll != 0) {
                if (abs(x - node) + abs(y - idx) < NEARBY_RADIUS)
                    *(gaiHeroStrategicRVOfPos + node + idx * MAP_WIDTH) = H2EnumIndex(RV_UNSET);
            } else {
                *(gaiHeroStrategicRVOfPos + node + idx * MAP_WIDTH) = H2EnumIndex(RV_UNSET);
                *(gaiHeroEventStratRVOfPos + node + idx * MAP_WIDTH) = H2EnumIndex(RV_UNSET);
            }
        }
    }
    *(gaiHeroEventStratRVOfPos + x + y * MAP_WIDTH) = H2EnumIndex(RV_UNSET);
    for (node = 0; node < GAME_HERO_COUNT; node++) {
        if (resetAll == 0
            || abs(x - gpGame->m_heroRecs[node].m_x) + abs(y - gpGame->m_heroRecs[node].m_x)
                   < NEARBY_RADIUS)
            gaiHeroLiveChance[node] = H2EnumIndex(RV_UNSET);
    }
}

void CheckDoMain(i32, i32 doMain) {
    if (iLastFrameRateTimer + AI_FRAME_THROTTLE_TICKS < platform::Ticks()
        || glTimers[0] < platform::Ticks()) {
        platform::PumpEvents();
        PollSound();
        if (glTimers[0] < platform::Ticks()) {
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
            glTimers[0] = platform::Ticks() + FRAME_TIMER_TICKS;
        }
        iLastFrameRateTimer = platform::Ticks();
    }
}

void ShowStatus(void) {}

philAI::philAI(void) {
    i32 i;
    for (i = 0; i < AI_PLAYER_COUNT; i++) {
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
    i32 townIdx;
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

    for (townIdx = 0; townIdx < gpCurPlayer->m_townCount; townIdx++) {
        townRef = gpGame->GetTown(gpCurPlayer->m_townIds[townIdx]);
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
                        && (townRef->m_buildings
                            & (1 << (H2EnumIndex(dwelling) + CREATURE_UPGRADE_BUILDING_OFFSET)))) {
                        hasUpgrade = true;
                        upgradedType = NextCreatureType(army->m_creatureTypes[slot]);
                    }
                }
                if ((army->m_creatureTypes[slot] == CREATURE_GREEN_DRAGON
                     || army->m_creatureTypes[slot] == CREATURE_RED_DRAGON)
                    && (townRef->m_buildings & H2EnumIndex(KB_DWELLING_UPGRADE_SIXTH_FLAG))) {
                    hasUpgrade = true;
                    upgradedType = CREATURE_BLACK_DRAGON;
                }
                if (hasUpgrade) {
                    goldAmount = army->m_quantities[slot]
                               * CREATURE_UPGRADE_RESOURCE_COST_MULTIPLIER
                               * (gMonsterDatabase[H2EnumIndex(upgradedType)].cost
                                  - gMonsterDatabase[H2EnumIndex(army->m_creatureTypes[slot])].cost);
                    if (upgradedType == CREATURE_BLACK_DRAGON) {
                        material = RES_SULFUR;
                        materialCost = army->m_quantities[slot]
                                     * CREATURE_UPGRADE_RESOURCE_COST_MULTIPLIER;
                    } else if (upgradedType == CREATURE_TITAN) {
                        material = RES_GEMS;
                        materialCost = army->m_quantities[slot]
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
                                && army->m_quantities[mergeIndex] > 0) {
                                army->m_quantities[mergeIndex] +=
                                    army->m_quantities[slot];
                                army->m_quantities[slot] = 0;
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

    BHC bestBuy;
    town* dockTown;

    gpGame->CheckHeroConsistency();
    if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] < PURCHASE_MINIMUM_GOLD
        || (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] < AI_HERO_PURCHASE_GOLD_FLOOR
            && gpCurPlayer->m_heroCount == 0))
        return;
    LogInt(
        "CheckBuy Start",
        gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)],
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
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
        if ((dockTown->m_buildings & AI_BUILDING_SHIPYARD_MASK)
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
    LogInt(
        "CheckBuy End  ",
        gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)],
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
}

inline hero* GetHeroSlot(i32 id) {
    return &gpGame->m_heroRecs[id];
}

i32 philAI::GoodAdjacent(MapDirection* direction) {
    i32 bestValue;
    i32 val;
    float node;
    float py;
    i32 nb;
    i32 kn;
    i32 p;
    MapDirection bestDirection;
    MapDirection dirIdx;

    bestDirection = MAP_DIRECTION_NONE;
    py = fReduceFactor;
    node = fBerserkFactor;
    fReduceFactor = 1.0f;
    fBerserkFactor = 1.0f;
    bestValue = 100;
    if ((gpAdvManager->GetCell(gpCurAIHero->m_x, gpCurAIHero->m_y)->m_triggerType
         & MAP_TRIGGER_TYPE_MASK)
        == MAP_OBJECT_STONE_LITHS)
        return 0;
    for (dirIdx = MAP_DIRECTION_NORTH; H2EnumIndex(dirIdx) < NORMAL_DIRECTION_COUNT;
         dirIdx++) {
        if (gpAdvManager->ValidMoveWithEvent(gpCurAIHero, dirIdx)) {
            kn = gpCurAIHero->m_x + normalDirTable[H2EnumIndex(dirIdx)].x;
            nb = gpCurAIHero->m_y + normalDirTable[H2EnumIndex(dirIdx)].y;
            if ((H2EnumIndex((gpAdvManager->GetCell(kn, nb)->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))
                && !(MAP_EXTRA_AT(kn, nb) & H2EnumIndex(MAP_EXTRA_ADJACENT_MONSTER))
                && ((gpAdvManager->GetCell(kn, nb)->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                    != MAP_OBJECT_STONE_LITHS)
                && ((gpAdvManager->GetCell(kn, nb)->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                    != MAP_OBJECT_WHIRLPOOL)) {
                if (gpAdvManager->GetCell(kn, nb)->m_triggerType
                        != (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
                    || GetCastleSlot(gpAdvManager->GetCell(kn, nb)->m_objectMetadata)->m_owner
                        != giCurPlayer
                    || gpCurAIHero->m_lastTownInteractionTurn != giCurTurn) {
                    if (gpAdvManager->GetCell(kn, nb)->m_triggerType
                            != (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))
                        || GetHeroSlot(gpAdvManager->GetCell(kn, nb)->m_objectMetadata)->m_owner
                            != giCurPlayer
                        || gpCurAIHero->m_lastHeroInteractionTurn != giCurTurn) {
                        val = ValueOfEventAtPosition(kn, nb, 2, &p);
                        if (p > 80)
                            if (val > bestValue) {
                                bestValue = val;
                                bestDirection = dirIdx;
                            }
                    }
                }
            }
        }
    }
    fReduceFactor = py;
    fBerserkFactor = node;
    if (bestDirection != MAP_DIRECTION_NONE) {
        *direction = bestDirection;
        return 1;
    }
    return 0;
}

void philAI::CheckReload(void) {
    i32 p;
    i32 node;
    mapCell* nb_1;
    i32 kn;
    i32 jb_5;
    float idx;
    float friendly;

    gbTroopReload = false;
    fReduceFactor = 1.0f;
    friendly = AI_ZERO_FLOAT;
    idx = 0.0f;
    kn = FightValueOfStack(&gpCurAIHero->m_army, gpCurAIHero, 0, 0, 0, 0);
    if (kn < AI_MINIMUM_FIGHT_VALUE)
        kn = AI_MINIMUM_FIGHT_VALUE;
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
    for (node = 0; node < MAP_WIDTH; node++) {
        for (p = 0; p < MAP_HEIGHT; p++) {
            if (gpSearchArray->GetRow(node, 1)[MAP_WIDTH * p].visited) {
                nb_1 = gpAdvManager->GetCell(node, p);
                switch (nb_1->m_triggerType) {
                    case (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)):
                        jb_5 = FightValueOfStack(
                            &GetCastleSlot(nb_1->m_objectMetadata)->m_army,
                            NULL,
                            0,
                            0,
                            0,
                            0
                        );
                        if (gpGame->m_castleOwners[nb_1->m_objectMetadata] == gpCurAIHero->m_owner) {
                            if (jb_5 > kn * 2) {
                                friendly +=
                                    (static_cast<float>(jb_5) / (kn * 2) - 1.0f)
                                    * (gpCurAIHero->m_mobility + 10)
                                    / (gpSearchArray->GetRow(node, 1)[MAP_WIDTH * p].distance + 10);
                            }
                        } else if (jb_5 > (kn >> 1)) {
                            idx += (static_cast<float>(jb_5) / (kn >> 1) - 1.0f)
                                   * (gpCurAIHero->m_mobility + 30)
                                   / (gpSearchArray->GetRow(node, 1)[MAP_WIDTH * p].distance + 30);
                        }
                        break;
                    case (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION)):
                        if (gpGame->m_availableHeroes[nb_1->m_objectMetadata]
                            != gpCurAIHero->m_owner) {
                            jb_5 = FightValueOfStack(
                                &GetHeroSlot(nb_1->m_objectMetadata)->m_army,
                                NULL,
                                0,
                                0,
                                0,
                                0
                            );
                            if (jb_5 > (kn >> 1)) {
                                idx += (static_cast<float>(jb_5) / (kn >> 1) - 1.0f)
                                       * (gpCurAIHero->m_mobility + 30)
                                       / (gpSearchArray->GetRow(node, 1)[MAP_WIDTH * p].distance + 30);
                            }
                        }
                }
            }
        }
    }
    if (friendly > 1.0f && idx > 1.0f) {
        fReduceFactor = AI_RELOAD_NUMERATOR / (AI_RELOAD_BASE + friendly + idx);
        gbTroopReload = true;
    }
}

void philAI::CheckBerserk(void) {
    i32 col;
    i32 jb;
    i32 best = -1;
    i32 line;
    mapCell* knIndex;
    i32 ndx;
    hero* heroPtr;

    gbBerserk = false;
    fBerserkFactor = 1.0f;
    jb = FightValueOfStack(&gpCurAIHero->m_army, gpCurAIHero, 1, 0, 0, 0);
    if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_WARRIOR)
        jb = static_cast<i32>(jb * AI_EASY_STRENGTH_FACTOR);
    if (jb < AI_MINIMUM_FIGHT_VALUE)
        jb = AI_MINIMUM_FIGHT_VALUE;
    if (jb < AI_BERSERK_THRESHOLD)
        return;
    {
        for (col = 0; col < MAP_WIDTH; col++) {
            for (line = 0; line < MAP_HEIGHT; line++) {
                knIndex = gpAdvManager->GetCell(col, line);
                switch (knIndex->m_triggerType) {
                    case (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)):
                        if (gpGame->m_castleOwners[knIndex->m_objectMetadata] != gpCurAIHero->m_owner) {
                            if (gpGame->m_castleOwners[knIndex->m_objectMetadata] != -1) {
                                ndx = FightValueOfStack(
                                    &GetCastleSlot(knIndex->m_objectMetadata)->m_army,
                                    NULL,
                                    1,
                                    1,
                                    knIndex->m_objectMetadata,
                                    0
                                );
                                if (ndx > jb)
                                    return;
                                if (ndx > best)
                                    best = ndx;
                            }
                        }
                        break;
                    case (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION)):
                        if (gpGame->m_availableHeroes[knIndex->m_objectMetadata]
                            != gpCurAIHero->m_owner) {
                            heroPtr = GetHeroSlot(knIndex->m_objectMetadata);
                            ndx = FightValueOfStack(
                                &heroPtr->m_army,
                                NULL,
                                1,
                                heroPtr->m_locationType
                                    == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)),
                                heroPtr->m_occupiedTown,
                                0
                            );
                            if (ndx * 2 > jb)
                                return;
                            if (ndx * 2 > best)
                                best = ndx * 2;
                        }
                        break;
                }
            }
        }
        if (best <= 0)
            return;
        fBerserkFactor = best * AI_BERSERK_FACTOR / jb;
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
    i32 mx;
    i32 bestX;
    i32 destX;
    i32 my;
    i32 destY;
    i32 bestY = -1;
    i32 side;
    i32 x;
    i32 y;
    i32 posValue;
    i32 bestVal;
    mapCell* arriveCell;
    mapCell* cell;

    if ((H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_EMBARKED))))
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
                        if (giGroundToTerrain[cell->m_terrainImageIndex] == TERRAIN_WATER)
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
                                    destX = x - 1;
                                    destY = y;
                                } else if (side == 1) {
                                    destX = x;
                                    destY = y + 1;
                                } else {
                                    destX = x + 1;
                                    destY = y;
                                }
                                if (gpSearchArray->GetRow(destX, 1)[MAP_WIDTH * destY].visited)
                                    continue;
                                if (destX == gpCurAIHero->m_x && destY == gpCurAIHero->m_y)
                                    continue;
                                if (destX < 0 || destX > MAP_WIDTH - 1 || destY < 0
                                    || destY > MAP_HEIGHT - 1)
                                    continue;
                                if (gpAdvManager->FindAdjacentMonster(
                                        destX,
                                        destY,
                                        &mx,
                                        &my,
                                        -1,
                                        -1
                                    ))
                                    continue;
                                arriveCell = gpAdvManager->GetCell(destX, destY);
                                if (giGroundToTerrain[arriveCell->m_terrainImageIndex]
                                    == TERRAIN_WATER)
                                    continue;
                                if (arriveCell->m_flags & H2EnumIndex(MAP_CELL_OCCUPIED))
                                    continue;
                                if ((H2EnumIndex((arriveCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG))))
                                    continue;
                                if (arriveCell->m_objectIndex != MAPCELL_SPRITE_NONE
                                    && !(arriveCell->m_flags & H2EnumIndex(MAP_CELL_OBJECT_SHADOW_ONLY)))
                                    continue;
                                bestX = destX;
                                bestY = destY;
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
    i32 len = 0;
    i32 bestX, bestY = -1;
    mapCell* cell;
    if (pHero->m_remainingMobility < AI_MINIMUM_SITE_MOBILITY)
        return 0;
    bestX = -1;
    x = pHero->m_x;
    y = pHero->m_y;
    for (i = gpSearchArray->m_pathLength - 1; i >= 1; i--) {
        x += normalDirTable[gpSearchArray->m_storage.aiPath.directions[i]].x;
        y += normalDirTable[gpSearchArray->m_storage.aiPath.directions[i]].y;
        if (abs(x - pHero->m_x) <= 7 && abs(y - pHero->m_y) <= 7) {
            cell = gpAdvManager->GetCell(x, y);
            if (!(cell->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell->m_flags & H2EnumIndex(MAP_CELL_OCCUPIED))) {
                bestX = x;
                bestY = y;
                len = gpSearchArray->m_pathLength - i;
            }
        }
    }
    if (bestX == -1 || len <= 5)
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

void ValidateHero(hero*) {}

void philAI::DoAI(i32 player) {
    i32 hiddenPointers0 = 0;
    b32 hourglassShown0 = false;
    MapDirection adjacentDirection2;
    b32 stopAfterStep6 = true;
    i32 eventY1;
    i32 eventX5;
    b32 heroDone5;
    i32 minimumValue1;
    hero* currentHero0;
    i32 stepLimit0;
    i32 steps4;
    b32 boughtAfterCapture5 = false;
    b32 savedShow3[3];
    mapCell* eventCell3;
    i32 pathIndex0;

    i32 moveFlagB12;
    i32 moveFlagA3;
    town* townPtr9;
    i32 townId3;
    MapDirection direction26;
    i32 targetValue11;
    MapDirection specialDirection6;

    LogInt(
        "DO AI 1",
        player,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
    PollSound();
    if (gbGameOver != 0)
        goto aiCleanup;
    if (giLimitPlayer != 0) {
        if (player != giLimitPlayer)
            goto aiCleanup;
    }
    LogInt(
        "DO AI",
        player,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
    InitAIMapVars();
    GetTurnAIVars(player);
    if (gpGame->m_day == 1) {
        for (pathIndex0 = 0; pathIndex0 < gpCurPlayer->m_heroCount; pathIndex0++) {
            GetHeroSlot(gpCurPlayer->m_heroIds[pathIndex0])->m_lastTownInteractionTurn =
                INTERACTION_TURN_UNSET;
        }
    }
    ShowStatus();
    SetupRelativeHeroStrengths();
    for (pathIndex0 = 0; pathIndex0 < H2EnumIndex(RES_COUNT); pathIndex0++) {
        sprintf(
            gText,
            "RES - %15s  %d  %d",
            gResourceNames[pathIndex0],
            gpCurPlayer->m_resources[pathIndex0],
            gpCurPlayer->m_aiData.m_income[pathIndex0]
        );
        LogStr(gText);
    }
    CheckBuyStuff();
    IncrementHourGlass();

    while ((currentHero0 = DetermineHeroToMove(player)) != NULL) {
        ValidateHero(currentHero0);
        gpCurAIHero = currentHero0;
        if (gpCurAIHero->m_boatId != HERO_BOAT_NONE && gpCurAIHero->m_boatTravelRange == 0) {
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
        LogInt(
            "Player with HeroTOMOVE",
            player,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE
        );
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

        heroDone5 = false;
        ResetHeroRVs(0, 0, 0);
        stepLimit0 = (H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_EMBARKED))) ? 15 : 5;
        minimumValue1 = gpCurAIHero->m_mobility + 800;
        stepLimit0 =
            static_cast<i32>(stepLimit0 * (1.7 - H2EnumIndex(gpGame->m_difficulty) * 0.1));
        minimumValue1 = static_cast<i32>(
            minimumValue1 * ((H2EnumIndex(gpGame->m_difficulty) - 1) * 0.06 + 0.8)
        );

        while (!heroDone5 && gpCurAIHero->m_remainingMobility >= AI_MINIMUM_SITE_MOBILITY) {
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
                targetValue11 = DetermineTargetPosition(
                    gpCurAIHero->m_destinationX,
                    gpCurAIHero->m_destinationY,
                    minimumValue1,
                    specialDirection6
                );
                townId3 = gpGame->GetTownId(gpCurAIHero->m_x, gpCurAIHero->m_y);
                if (townId3 != -1) {
                    townPtr9 = GetCastleSlot(townId3);
                    if (townPtr9->m_threat != 0
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
                if (specialDirection6 != MAP_DIRECTION_NONE) {
                    direction26 = specialDirection6;
                    steps4 = 99;
                    moveFlagA3 = 0;
                    moveFlagB12 = 0;
                    goto aiMoveDirection;
                }
                if (targetValue11 < 1000 && specialDirection6 == MAP_DIRECTION_NONE
                    && DoAnywhereDDoorTownGate(targetValue11))
                    goto aiRetarget;

                if (targetValue11 < 75 && gpGame->m_day == 7
                    && gpCurAIHero->m_locationType
                           == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))) {
                    gpCurAIHero->m_remainingMobility = 0;
                }
                for (pathIndex0 = 0; pathIndex0 < iCurPlaceToVisit; pathIndex0++) {
                    if (iPlacesVisited[pathIndex0][0] == gpCurAIHero->m_destinationX
                        && iPlacesVisited[pathIndex0][1] == gpCurAIHero->m_destinationY
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
                    hourglassShown0 = false;
                    IncrementHourGlass();
                }
                if (gpCurAIHero->m_destinationX != -1 && gpCurAIHero->m_destinationY != -1) {
                    eventCell3 = NULL;
                    gpAdvManager->SetHeroContext(gpCurAIHero->m_id, 0);
                    gpSearchArray->BuildPath(
                        gpCurAIHero->m_x,
                        gpCurAIHero->m_y,
                        gpCurAIHero->m_destinationX,
                        gpCurAIHero->m_destinationY,
                        gpCurAIHero->m_remainingMobility
                    );
                    if (gpSearchArray->m_pathLength == 0) {
                        heroDone5 = true;
                    } else if (gpSearchArray->m_pathLength > 0) {
                        gpAdvManager->UpdateScreen(0, 0);
                        if (targetValue11 > 1000 && gpCurAIHero->HasSpell(SPELL_DIMENSION_DOOR)
                            && gpCurAIHero->m_spellPoints >= AI_DIMENSION_DOOR_SPELL_POINTS
                            && DoDimensionDoor(gpCurAIHero))
                            goto aiRetarget;

                        steps4 = 0;
                        pathIndex0 = gpSearchArray->m_pathLength - 1;
                        moveFlagA3 = 0;
                        moveFlagB12 = 0;
                        stopAfterStep6 = true;
                        while (pathIndex0 >= 0 && steps4 < stepLimit0) {
                            stopAfterStep6 =
                                (steps4 + 1 == stepLimit0 || pathIndex0 == 0) ? 1 : 0;
                            if (pathIndex0 > 0) {
                                moveFlagA3 = 0;
                                moveFlagB12 = 0;
                                if (GoodAdjacent(&adjacentDirection2)) {
                                    gpSearchArray->m_storage.aiPath.directions[pathIndex0] =
                                        static_cast<i8>(adjacentDirection2);
                                    stopAfterStep6 = true;
                                }
                            }
                            direction26 = static_cast<MapDirection>(
                                gpSearchArray->m_storage.aiPath.directions[pathIndex0]
                            );
                        aiMoveDirection:
                            if (gpAdvManager->GetMoveShowIt(gpCurAIHero, direction26)) {
                                savedShow3[0] = bShowIt;
                                bShowIt = true;
                                gpMouseManager->HideColorPointer();
                                hiddenPointers0++;
                                bShowIt = savedShow3[0];
                            }
                            eventCell3 = gpAdvManager->MoveHero(
                                direction26,
                                stopAfterStep6,
                                &eventX5,
                                &eventY1,
                                &moveFlagA3,
                                1,
                                &moveFlagB12,
                                0
                            );
                            steps4++;
                            if (eventCell3 != NULL || moveFlagA3 != 0 || moveFlagB12 != 0) {
                                break;
                            }
                            pathIndex0--;
                        }

                        if (gpCurAIHero->m_owner != giCurPlayer) {
                            if (!boughtAfterCapture5) {
                                boughtAfterCapture5 = true;
                                CheckBuyStuff();
                            } else {
                            }
                            goto aiNextHero;
                        }
                        if (gpCurAIHero->m_remainingMobility <= (gpCurAIHero->m_mobility >> 1)
                            && !hourglassShown0) {
                            hourglassShown0 = true;
                            IncrementHourGlass();
                        }
                        if (pathIndex0 < 0 && gpCurPlayer->m_ultimateArtifactHintChance > 15
                            && gpCurPlayer->m_ultimateArtifactHintX == gpCurAIHero->m_x
                            && gpCurPlayer->m_ultimateArtifactHintY == gpCurAIHero->m_y) {
                            if (gpCurAIHero->m_remainingMobility == gpCurAIHero->m_mobility) {
                                gpAdvManager->ProcessSearch(-1, -1);
                            } else {
                                gpCurAIHero->m_remainingMobility = 0;
                            }
                        }
                        if (pathIndex0 < 0
                            && (((gpCurAIHero->m_x != gpCurAIHero->m_destinationX
                                  || gpCurAIHero->m_y != gpCurAIHero->m_destinationY)
                                 && eventCell3 == NULL)
                                || gpCurAIHero->m_remainingMobility < AI_MINIMUM_SITE_MOBILITY
                                || (moveFlagA3 != 0 && eventCell3 == NULL))) {
                            heroDone5 = true;
                        }

                        savedShow3[0] = bShowIt;
                        bShowIt = true;
                        while (hiddenPointers0 != 0) {
                            gpMouseManager->ShowColorPointer();
                            hiddenPointers0--;
                        }
                        bShowIt = savedShow3[0];
                        gpAdvManager->UpdateRadar(1, 0);
                    } else {
                        heroDone5 = true;
                    }
                    if (eventCell3 != NULL) {
                        gpAdvManager->DoAIEvent(eventCell3, gpCurAIHero, eventX5, eventY1);
                        if (gpCurPlayer->m_currentHero == -1) {
                            goto aiNextHero;
                        }
                        ValidateHero(gpCurAIHero);
                        ResetHeroRVs(1, gpCurAIHero->m_destinationX, gpCurAIHero->m_destinationY);
                    }
                } else {
                    heroDone5 = true;
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
    i32 outcomeE0;
    playerData* playerPtr27;
    hero* threatHeroPtr8;
    i32 ownedTownCount3;
    i32 lastFightValue8;
    town* townPtr0;
    i32 lateTurn40;
    i32 x3;
    float winChance11;

    float fightValueFloat0;
    hero* heroPtr0;
    mapCell* cell10;
    i32 outcomeD1;
    i32 outcomeC5;
    i32 yCenter0;
    i32 outcomeB9;
    float townCountValue26;
    i32 xCenter12;
    i32 outcomeA15;
    i32 firstWeekIndex0;
    i32 playerIndex0;
    i32 mineValue17;
    i32 artifactTotal8;
    i32 earlyTurn8;
    i32 generalIndex4;
    i32 innerIndex27;
    i32 y4;

    giCurTurn = gpGame->m_day + (gpGame->m_week - 1) * CALENDAR_DAYS_PER_WEEK
                + (gpGame->m_month - 1) * CALENDAR_DAYS_PER_MONTH;
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

    fightValueFloat0 = 0.0;
    if (giCurTurn <= 7) {
        fFirstWeekTownFV = 0.0;
        for (firstWeekIndex0 = 0; firstWeekIndex0 < gpCurPlayer->m_townCount; firstWeekIndex0++) {
            townPtr0 = GetCastleSlot(gpCurPlayer->m_townIds[firstWeekIndex0]);
            for (innerIndex27 = 0; innerIndex27 < GAME_HERO_COUNT; innerIndex27++) {
                heroPtr0 = &gpGame->m_heroRecs[innerIndex27];
                if (heroPtr0->m_owner < 0 || heroPtr0->m_owner > 5
                    || heroPtr0->m_owner == giCurPlayer)
                    continue;
                if (abs(heroPtr0->m_x - townPtr0->m_x) + abs(heroPtr0->m_y - townPtr0->m_y)
                    < 16) {
                    fFirstWeekTownFV = 0.3f;
                    goto firstWeekDone;
                }
            }
        }
    }

firstWeekDone:
    lastFightValue8 = 0;
    for (generalIndex4 = 0; generalIndex4 < gpCurPlayer->m_heroCount; generalIndex4++) {
        heroPtr0 = GetHeroSlot(gpCurPlayer->m_heroIds[generalIndex4]);
        fightValueFloat0 =
            static_cast<float>(FightValueOfStack(&heroPtr0->m_army, heroPtr0, 0, 0, 0, 0));
        lastFightValue8 = static_cast<i32>(lastFightValue8 + fightValueFloat0);
        heroPtr0->m_aiFightValue =
            static_cast<float>(fightValueFloat0 * AI_HERO_VALUE_SCALE + AI_HERO_VALUE_BASE);
    }
    for (generalIndex4 = 0; generalIndex4 < gpCurPlayer->m_townCount; generalIndex4++) {
        townPtr0 = GetCastleSlot(gpCurPlayer->m_townIds[generalIndex4]);
        fightValueFloat0 = static_cast<float>(FightValueOfStack(&townPtr0->m_army, NULL, 0, 0, 0, 0));
        lastFightValue8 = static_cast<i32>(lastFightValue8 + fightValueFloat0);
    }
    gpCurPlayer->m_aiData.m_upgradeValueWeight =
        static_cast<float>(
            gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)]
            + gpCurPlayer->m_aiData.m_income[H2EnumIndex(RES_GOLD)]
        ) / (lastFightValue8 + 1000)
        + gpCurPlayer->m_aiData.m_attentionWeights.upgradeBase;

    artifactTotal8 = 0;
    for (generalIndex4 = H2EnumIndex(ARTIFACT_ULTIMATE_SHIELD);
         generalIndex4 < H2EnumIndex(ARTIFACT_GAMBLERS_COIN);
         generalIndex4++)
        artifactTotal8 += gArtifactBaseRV[generalIndex4];
    for (generalIndex4 = 0; generalIndex4 < gpGame->m_playerCount; generalIndex4++) {
        gpGame->m_players[generalIndex4].m_aiData.m_artifactPoolShare =
            1.0 / (gpGame->m_playerCount + gpGame->m_deadPlayerCount);
    }
    gpCurPlayer->m_aiData.m_artifactValue =
        static_cast<float>(artifactTotal8 / AI_ARTIFACT_POOL_DIVISOR);

    for (generalIndex4 = 0; generalIndex4 < GAME_TOWN_COUNT; generalIndex4++)
        gpGame->m_castleRecs[generalIndex4].m_threat = 0;
    for (generalIndex4 = 0; generalIndex4 < gpCurPlayer->m_townCount; generalIndex4++)
        gpGame->m_castleRecs[gpCurPlayer->m_townIds[generalIndex4]].m_threat = 0;

    memset(gaiEnemyHeroReachable, 0, MAP_WIDTH * MAP_HEIGHT);
    for (playerIndex0 = 0; playerIndex0 < gpGame->m_playerCount; playerIndex0++) {
        if (playerIndex0 == giCurPlayer)
            continue;
        if (OnMySide(playerIndex0))
            continue;
        for (innerIndex27 = 0; innerIndex27 < gpGame->m_players[playerIndex0].m_heroCount;
             innerIndex27++) {
            threatHeroPtr8 =
                GetHeroSlot(gpGame->m_players[playerIndex0].m_heroIds[innerIndex27]);
            gpSearchArray->SeedPosition(
                threatHeroPtr8->m_x,
                threatHeroPtr8->m_y,
                threatHeroPtr8->m_direction,
                threatHeroPtr8->m_mobility + 100,
                H2EnumIndex(threatHeroPtr8->m_eventFlags) & H2EnumIndex(HERO_EVENT_EMBARKED),
                1,
                threatHeroPtr8->m_mobility + 100,
                H2EnumIndex(threatHeroPtr8->m_secondarySkills[H2EnumIndex(HERO_SKILL_PATHFINDING)]),
                -1,
                -1,
                0,
                0
            );
            for (x3 = 0; x3 < MAP_WIDTH; x3++) {
                for (y4 = 0; y4 < MAP_WIDTH; y4++) {
                    if (gpSearchArray->GetRow(x3, 1)[MAP_WIDTH * y4].visited) {
                        town* threatTownPtr10;
                        i32 threatTownId9;
                        *(gaiEnemyHeroReachable + x3 + y4 * MAP_WIDTH) = 1;
                        if (y4 == 0)
                            continue;
                        cell10 = gpAdvManager->GetCell(x3, y4 - 1);
                        if (cell10->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
                            || (cell10->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                                   == MAP_OBJECT_HERO_INTERACTION) {
                            threatTownId9 = gpGame->GetTownId(x3, y4 - 1);
                            if (threatTownId9 == -1)
                                continue;
                            threatTownPtr10 = GetCastleSlot(threatTownId9);
                            if (threatTownPtr10->m_owner == giCurPlayer
                                && (threatTownPtr10->m_buildings & H2EnumIndex(TOWN_BUILDING_CASTLE))) {
                                ProbableOutcomeOfBattle(
                                    &threatHeroPtr8->m_army,
                                    threatHeroPtr8,
                                    &GetHeroSlot(threatTownPtr10->m_occupyingHeroId)->m_army,
                                    GetHeroSlot(threatTownPtr10->m_occupyingHeroId),
                                    &threatTownPtr10->m_army,
                                    1,
                                    threatTownPtr10->m_id,
                                    threatTownPtr10->m_owner,
                                    winChance11,
                                    outcomeA15,
                                    outcomeB9,
                                    outcomeC5,
                                    outcomeD1,
                                    outcomeE0
                                );
                                if (winChance11 > AI_TOWN_DEFENSE_THRESHOLD)
                                    threatTownPtr10->m_threat = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    memset(gaiTurnValueOfMine, 7, MAP_WIDTH * MAP_HEIGHT);
    for (playerIndex0 = 0; playerIndex0 < gpGame->m_playerCount; playerIndex0++) {
        if (playerIndex0 != giCurPlayer) {
            playerPtr27 = &gpGame->m_players[playerIndex0];
            for (innerIndex27 = 0; innerIndex27 < playerPtr27->m_heroCount; innerIndex27++) {
                xCenter12 = GetHeroSlot(playerPtr27->m_heroIds[innerIndex27])->m_x;
                yCenter0 = GetHeroSlot(playerPtr27->m_heroIds[innerIndex27])->m_y;
                for (x3 = xCenter12 - 10; x3 <= xCenter12 + 10; x3++) {
                    for (y4 = yCenter0 - 10; y4 <= yCenter0 + 10; y4++) {
                        if (x3 >= 0 && x3 < MAP_WIDTH && y4 >= 0 && y4 < MAP_HEIGHT) {
                            mineValue17 = abs(abs(x3 - xCenter12) + abs(y4 - yCenter0) - 4) >> 2;
                            if (mineValue17 < *(gaiTurnValueOfMine + x3 + y4 * MAP_WIDTH))
                                *(gaiTurnValueOfMine + x3 + y4 * MAP_WIDTH) =
                                    static_cast<i8>(mineValue17);
                        }
                    }
                }
            }
        }
    }

    for (generalIndex4 = 0; generalIndex4 < GAME_HERO_COUNT; generalIndex4++)
        gfHeroInteractionBonus[generalIndex4] = 1.0f;
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

    townCountValue26 = 0.1f;
    for (generalIndex4 = 0; generalIndex4 < gpCurPlayer->m_townCount; generalIndex4++) {
        if (GetCastleSlot(gpCurPlayer->m_townIds[generalIndex4])->m_buildings
            & H2EnumIndex(TOWN_BUILDING_CASTLE))
            townCountValue26 += 1.0;
        else
            townCountValue26 += 0.5;
    }
    ownedTownCount3 = static_cast<i32>(townCountValue26);
    giMaxHeroesForThisPlayer = 3;
    if (gpGame->m_playerCount - gpGame->m_deadPlayerCount == 2)
        giMaxHeroesForThisPlayer++;
    if (gpGame->m_playerCount - gpGame->m_deadPlayerCount == 3)
        giMaxHeroesForThisPlayer++;
    if (ownedTownCount3 >= 5)
        giMaxHeroesForThisPlayer++;
    if (ownedTownCount3 >= 10)
        giMaxHeroesForThisPlayer++;
    if (gpGame->m_mapHeader.width == MAP_DIMENSION_SMALL)
        giMaxHeroesForThisPlayer--;
    if (gpGame->m_mapHeader.width >= MAP_DIMENSION_LARGE)
        giMaxHeroesForThisPlayer++;

    earlyTurn8 = 16;
    lateTurn40 = 22;
    if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_EXPLORER) {
        earlyTurn8 = 8;
        lateTurn40 = 15;
    }
    if (giCurTurn < earlyTurn8 && ownedTownCount3 == 1 && gpCurPlayer->m_heroCount == 1) {
        town* earlyTownPtr29;
        hero* earlyHeroPtr6;
        earlyHeroPtr6 = GetHeroSlot(gpCurPlayer->m_heroIds[0]);
        earlyTownPtr29 = GetCastleSlot(gpCurPlayer->m_townIds[0]);
        if (abs(earlyTownPtr29->m_x - earlyHeroPtr6->m_x)
                + abs(earlyTownPtr29->m_y - earlyHeroPtr6->m_y)
            < 18)
            giMaxHeroesForThisPlayer = 1;
    }
    if (giCurTurn < lateTurn40 && giMaxHeroesForThisPlayer > 2) {
        giMaxHeroesForThisPlayer = 2;
    }
    if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_EXPLORER)
        ownedTownCount3++;
    if (giMaxHeroesForThisPlayer > ownedTownCount3 + 1)
        giMaxHeroesForThisPlayer = ownedTownCount3 + 1;
    if (giMaxHeroesForThisPlayer < gpCurPlayer->m_minimumHeroCount)
        giMaxHeroesForThisPlayer = gpCurPlayer->m_minimumHeroCount;
}

void philAI::GetBestBHC(i32, BHC& best) {
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
        townStrength[townNo] = FightValueOfStack(&curTown->m_army, NULL, 0, 0, 0, 0) + 400;
        totalStrength += townStrength[townNo];
        if (curTown->m_buildings & H2EnumIndex(TOWN_BUILDING_CASTLE))
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
            avgStrength * ((curTown->m_buildings & H2EnumIndex(TOWN_BUILDING_CASTLE)) ? 10 : 7) + 400;
    }

    for (townNo = 0; townNo < gpCurPlayer->m_townCount; townNo++) {
        curTown = &gpGame->m_castleRecs[gpCurPlayer->m_townIds[townNo]];
        LogInt(
            "Turns Owned",
            curTown->m_turnsOwned,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE
        );
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
                && (curTown->m_buildings & H2EnumIndex(TOWN_BUILDING_CASTLE))) {
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
    LogInt(
        "BestBHC ",
        H2EnumIndex(best.type),
        static_cast<i32>(topVal * 100.0f),
        best.what,
        0,
        0,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
    if (topVal < AI_MINIMUM_PURCHASE_VALUE)
        best.type = PURCHASE_NONE;
}

hero* philAI::DetermineHeroToMove(i32 player) {
    i32 value;
    i32 jb;
    i32 best;
    i32 i;
    jb = 0;
    best = -1;
    if (gpCurPlayer->HasMobileHero()) {
        for (i = 0; i < gpCurPlayer->m_heroCount; i++) {
            value = gpGame->m_heroRecs[gpGame->m_players[player].m_heroIds[i]]
                          .m_remainingMobility;
            if (value > jb) {
                jb = value;
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
    i32 colCnt;
    H2EnumStorage<TerrainType, i32> ground;
    i32 posValue;
    MapDirection goodDir;
    i32 boatDist;
    i32 heroNum;
    i32 pass;
    i32 y;
    i32 chosenY;
    i32 restMobility;
    i32 x;
    i32 chosenX;
    i32 hiY;
    i32 spread;
    i32 rightX;
    i32 topY;
    i32 good;
    i32 rowCounter;
    i32 dblMob;
    i32 dxy;
    i32 leftX;

    if (GoodAdjacent(&goodDir)) {
        targetX = gpCurAIHero->m_x + normalDirTable[H2EnumIndex(goodDir)].x;
        targetY = gpCurAIHero->m_y + normalDirTable[H2EnumIndex(goodDir)].y;
        direction = goodDir;
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
    ground = giGroundToTerrain[cell->m_terrainImageIndex];
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

    colCnt = -1;
    leftX = gpCurAIHero->m_x - 25;
    if (leftX < 0)
        leftX = 0;
    topY = gpCurAIHero->m_y - 25;
    if (topY < 0)
        topY = 0;
    rightX = gpCurAIHero->m_x + 25;
    if (rightX > MAP_WIDTH - 1)
        rightX = MAP_WIDTH;
    hiY = gpCurAIHero->m_y + 25;
    if (hiY > MAP_HEIGHT - 1)
        hiY = MAP_HEIGHT;

    for (pass = 0; pass < 2; pass++) {
        if ((H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))) {
            if (pass == 0)
                continue;
        }
        for (x = leftX; x < rightX; x++) {
            rowCounter = -1;
            colCnt++;
            if (colCnt >= spread)
                colCnt = 0;
            for (y = topY; y < hiY; y++) {
                rowCounter++;
                if (rowCounter >= spread)
                    rowCounter = 0;
                if (gpSearchArray->GetNode(x, y).visited) {
                    if (gpCurAIHero->IsEmbarked())
                        goto position_reachable;
                    dxy = abs(x - gpCurAIHero->m_x) + abs(y - gpCurAIHero->m_y);
                    if ((pass == 0 && dxy > 5
                         && (x != gpCurAIHero->m_destinationX
                             || y != gpCurAIHero->m_destinationY)
                         && (dxy >= 16
                             || (gpAdvManager->GetCell(x, y)->m_triggerType
                                     != (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))
                                 && (gpAdvManager->GetCell(x, y)->m_triggerType
                                         != (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
                                     || GetCastleSlot(gpAdvManager->GetCell(x, y)
                                                          ->m_objectMetadata)
                                                ->m_owner
                                            == giCurPlayer))))
                        || (pass == 1
                            && (dxy <= 5
                                || (x == gpCurAIHero->m_destinationX
                                    && y == gpCurAIHero->m_destinationY)
                                || (dxy < 16
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
                            good = 0;
                        } else {
                            good = (cell->m_triggerType
                                        == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
                                    || cell->m_triggerType
                                           == (MAP_TRIGGER_ACTION_FLAG
                                               | MAP_OBJECT_HERO_INTERACTION)
                                    || (cell->m_triggerType
                                            == (MAP_ACTION_TRIGGER(MAP_OBJECT_BOAT))
                                        && !(H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))));
                        }
                    } else {
                        good = ((H2EnumIndex((cell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))
                                || (cell->m_triggerType == MAP_OBJECT_COAST
                                    && (H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_EMBARKED))))
                                || (x % spread == 0 && y % spread == 0
                                    && (((H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))
                                         && giGroundToTerrain[cell->m_terrainImageIndex]
                                                == TERRAIN_WATER)
                                        || (!(H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))
                                            && giGroundToTerrain[cell->m_terrainImageIndex]
                                                   != TERRAIN_WATER)))
                                || (x == gpCurPlayer->m_ultimateArtifactHintX
                                    && y == gpCurPlayer->m_ultimateArtifactHintY));
                    }

                    if (good && gpCurAIHero->m_boatId != HERO_BOAT_NONE) {
                        boatDist =
                            abs(x - gpCurAIHero->m_boatId)
                            + abs(y - static_cast<u8>(gpCurAIHero->m_boatDestY));
                        if (boatDist > gpCurAIHero->m_boatTravelRange)
                            good = 0;
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
                        if ((H2EnumIndex((gpAdvManager->GetCell(chosenX, chosenY)
                                    ->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))
                            || abs(x - gpCurAIHero->m_x) + abs(y - gpCurAIHero->m_y)
                                   > abs(chosenX - gpCurAIHero->m_x)
                                       + abs(chosenY - gpCurAIHero->m_y)) {
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
    if (gpCurAIHero->m_boatId != HERO_BOAT_NONE && bestRV <= 0) {
        targetX = gpCurAIHero->m_boatId;
        targetY = static_cast<u8>(gpCurAIHero->m_boatDestY);
    }
    LogInt(
        "Hero, Best RV target XY  current XY",
        gpCurAIHero->m_owner,
        bestRV,
        targetX,
        targetY,
        gpCurAIHero->m_x,
        gpCurAIHero->m_y,
        LOG_UNUSED_VALUE
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
        defenderFight += static_cast<float>(FightValueOfStack(townArmy, NULL, 1, 0, 0, 0));

    attackerRaw = static_cast<float>(FightValueOfStack(attacker, attackerHero, 0, 0, 0, 0));
    defRaw = static_cast<float>(FightValueOfStack(defender, defenderHero, 0, 0, 0, 0));
    if (townArmy != NULL)
        defRaw += static_cast<float>(FightValueOfStack(townArmy, NULL, 0, 0, 0, 0));

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
        for (j = 0; j < AI_BATTLE_ARTIFACT_SLOT_COUNT;
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
        for (j = 0; j < AI_BATTLE_ARTIFACT_SLOT_COUNT;
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
                  * (gbHumanPlayer[defenderHero->m_owner] != 0 ? gfAttackHumanBonus
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
    town* townPtr,
    BuildingSlotType building,
    i32& resourceValue,
    float& benefitCost
) {
    FactionType currentTownRace_3;
    i32 dwellingTotal_6;
    i32 highestDwellingId_2;
    i32 currentOccupiedSlots_16;
    float adjustedValue_11;
    b32 creatureLocated_9;
    i32 buildingLevel;
    i32 indexBuilding_2;
    CreatureType currentCreatureType_1;
    i32 costsByResource_7[AI_PURCHASE_RESOURCE_COUNT];
    float estimatedAttackChance_9;
    float enemyStrengthLocal_9;
    i32 currentAttackTurns_2;
    i32 projectedAttackValue_2;
    i32 estimatedAttackWeeks_8;
    float dangerRating;
    currentTownRace_3 = townPtr->m_type;
    dwellingTotal_6 = 0;
    highestDwellingId_2 = -1;
    for (indexBuilding_2 = H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST);
         indexBuilding_2 < H2EnumIndex(BUILDING_SLOT_DWELLING_LAST);
         indexBuilding_2++) {
        if (townPtr->m_buildings & (1 << indexBuilding_2)) {
            dwellingTotal_6++;
            highestDwellingId_2 = indexBuilding_2;
        }
    }
    currentOccupiedSlots_16 = 0;
    for (indexBuilding_2 = 0; indexBuilding_2 < AI_TOWN_ARMY_SLOTS; indexBuilding_2++) {
        if (townPtr->m_army.m_quantities[indexBuilding_2] > 0)
            currentOccupiedSlots_16++;
    }
    adjustedValue_11 = static_cast<float>(GetBuildingBaseResourceValue(
        currentTownRace_3,
        building,
        building == BUILDING_SLOT_MAGE_GUILD ? townPtr->m_buildState + 1 : 1
    ));
    if (building >= BUILDING_SLOT_UPGRADE_FIRST && building <= BUILDING_SLOT_UPGRADE_LAST) {
        adjustedValue_11 -= static_cast<float>(
            GetBuildingBaseResourceValue(
                currentTownRace_3,
                building - CREATURE_UPGRADE_BUILDING_OFFSET,
                1
            )
        );
    }
    if (building == BUILDING_SLOT_DWELLING_LAST) {
        adjustedValue_11 -= static_cast<float>(GetBuildingBaseResourceValue(
            currentTownRace_3,
            BUILDING_SLOT_DWELLING_LAST - 1,
            1
        ));
    }
    if (building == BUILDING_SLOT_MAGE_GUILD && townPtr->m_buildState > 1) {
        adjustedValue_11 -= static_cast<float>(
            GetBuildingBaseResourceValue(currentTownRace_3, building, townPtr->m_buildState)
        );
    }

    switch (building) {
        case BUILDING_SLOT_CASTLE:
            if (townPtr->m_mayNotUpgradeToCastle != 0)
                adjustedValue_11 = -99.0f;
            break;
        case BUILDING_SLOT_MAGE_GUILD:
            if (townPtr->m_type == FACTION_KNIGHT || townPtr->m_type == FACTION_BARBARIAN) {
                if ((townPtr->m_buildState >= 4 && giCurTurn < 40)
                    || (townPtr->m_buildState >= 3 && giCurTurn < 30)
                    || (townPtr->m_buildState >= 2 && giCurTurn < 20))
                    adjustedValue_11 = static_cast<float>(adjustedValue_11 * 0.4);
                else
                    adjustedValue_11 = static_cast<float>(adjustedValue_11 * 0.65);
            } else if (townPtr->m_buildState == 0) {
                adjustedValue_11 = static_cast<float>(adjustedValue_11 * 1.2);
            } else if ((townPtr->m_buildState >= 4 && giCurTurn < 35)
                       || (townPtr->m_buildState >= 3 && giCurTurn < 25)
                       || (townPtr->m_buildState >= 2 && giCurTurn < 15)) {
                adjustedValue_11 =
                    static_cast<float>(adjustedValue_11 * AI_MAGE_GUILD_LATE_FACTOR);
            }
            break;
        case BUILDING_SLOT_SPECIAL_ONE:
            if (townPtr->m_type == FACTION_NECROMANCER
                && (townPtr->m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_DWELLING_THIRD))))
                adjustedValue_11 = 1500.0f;
            else if (giCurTurn < 21)
                adjustedValue_11 = 0.0;
            break;
        case BUILDING_SLOT_SPECIAL_SEVEN:
            if (giCurTurn < 3 && !(townPtr->m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_DWELLING_THIRD))))
                adjustedValue_11 = 0.0f;
            break;
        case BUILDING_SLOT_SPECIAL_TEN:
            if (giCurTurn < 21)
                adjustedValue_11 = 0.0f;
            break;
        case BUILDING_SLOT_WELL_EXTRA:
            if (gpGame->m_day < 6)
                adjustedValue_11 = 0.0f;
            break;
        case BUILDING_SLOT_SPECIAL_EIGHT:
        case BUILDING_SLOT_SPECIAL_NINE:
        case BUILDING_SLOT_SPECIAL_TWELVE:
        case BUILDING_SLOT_NEUTRAL_LAST:
        deferEarlyBuilding:
            if (giCurTurn < 16)
                adjustedValue_11 = 0.0f;
            break;
        case BUILDING_SLOT_SPECIAL:
            if (townPtr->m_type == FACTION_KNIGHT || townPtr->m_type == FACTION_SORCERESS
                || townPtr->m_type == FACTION_BARBARIAN
                || townPtr->m_type == FACTION_NECROMANCER) {
                goto deferEarlyBuilding;
            }
            break;
        case BUILDING_SLOT_DISABLED_FIRST:
            break;
        case BUILDING_SLOT_DOCK:
            adjustedValue_11 = 0.0f;
            break;
        case BUILDING_SLOT_SPECIAL_FOUR:
            adjustedValue_11 = static_cast<float>(
                adjustedValue_11 * (gpCurPlayer->m_aiData.m_attentionWeights.buildingValue + 0.66)
            );
            adjustedValue_11 = static_cast<float>(
                adjustedValue_11
                * (gpCurPlayer->m_aiData.m_attentionWeights.upgradeBase * 2.0f + 0.33)
            );
            adjustedValue_11 = static_cast<float>(adjustedValue_11 * (dwellingTotal_6 * 0.33 + 0.66));
            if ((townPtr->m_type != FACTION_KNIGHT
                 || !(townPtr->m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_DWELLING_SECOND))))
                && gpGame->m_day < 6)
                adjustedValue_11 = 0.0f;
            break;
        case BUILDING_SLOT_NECROMANCER_SHRINE:
            if (townPtr->m_type == FACTION_NECROMANCER)
                break;
            if ((townPtr->m_type == FACTION_KNIGHT
                 && (townPtr->m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_DWELLING_THIRD))))
                || (townPtr->m_type == FACTION_SORCERESS
                    && (townPtr->m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_DWELLING_THIRD))))) {
                adjustedValue_11 = 1000.0f;
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
            if (currentOccupiedSlots_16 == AI_TOWN_ARMY_SLOTS) {
                creatureLocated_9 = false;
                for (indexBuilding_2 = 0; indexBuilding_2 < AI_TOWN_ARMY_SLOTS; indexBuilding_2++) {
                    if (townPtr->m_army.m_creatureTypes[indexBuilding_2]
                        == gDwellingType[H2EnumIndex(townPtr->m_type)]
                                        [H2EnumIndex(building) - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)]) {
                        creatureLocated_9 = true;
                    }
                }
                if (!creatureLocated_9)
                    break;
            }
            adjustedValue_11 = static_cast<float>(
                adjustedValue_11 * (gpCurPlayer->m_aiData.m_attentionWeights.buildingValue + 0.66)
            );
            adjustedValue_11 = static_cast<float>(
                adjustedValue_11
                * (gpCurPlayer->m_aiData.m_attentionWeights.upgradeBase * 2.0f + 0.33)
            );
            adjustedValue_11 = static_cast<float>(
                adjustedValue_11
                * (1.0 - gpCurPlayer->BuildingsOwned(currentTownRace_3, building, 0) * 0.05)
            );
            if (static_cast<i32>(building) - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)
                < highestDwellingId_2)
                adjustedValue_11 = static_cast<float>(
                    adjustedValue_11
                    * (AI_EARLY_DWELLING_FACTOR_BASE - dwellingTotal_6 * 0.33)
                );
            if (townPtr->m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_SPECIAL_FOUR)))
                adjustedValue_11 = static_cast<float>(adjustedValue_11 * 1.1);
            for (buildingLevel = 0; buildingLevel < AI_DWELLING_LEVELS; buildingLevel++) {
                currentCreatureType_1 = gDwellingType[H2EnumIndex(townPtr->m_type)][buildingLevel];
                if ((townPtr->m_buildings & (1 << H2EnumIndex(buildingLevel + H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST))))
                    && townPtr->m_garrison[buildingLevel] > 0
                    && gMonsterDatabase[H2EnumIndex(currentCreatureType_1)].iconIndex * 1.2
                           > gMonsterDatabase[H2EnumIndex(gDwellingType[H2EnumIndex(townPtr->m_type)]
                                                               [H2EnumIndex(building)
                                                                - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)])]
                                 .iconIndex) {
                    adjustedValue_11 = 0.0f;
                    break;
                }
            }
            break;
    }

    LikelihoodOfEnemyAttacking(
        townPtr,
        NULL,
        estimatedAttackChance_9,
        enemyStrengthLocal_9,
        currentAttackTurns_2,
        projectedAttackValue_2,
        estimatedAttackWeeks_8,
        dangerRating
    );
    adjustedValue_11 = static_cast<float>(adjustedValue_11 * (1.0 - dangerRating * 3.0));
    if (townPtr->m_originalOwner != giCurPlayer)
        adjustedValue_11 = static_cast<float>(adjustedValue_11 * 0.85);
    if (adjustedValue_11 < 0.0f)
        adjustedValue_11 = 0.0f;
    GetBuildingCost(
        currentTownRace_3,
        building,
        costsByResource_7,
        building == BUILDING_SLOT_MAGE_GUILD ? townPtr->m_buildState : 0
    );
    adjustedValue_11 = FutureDeflator(costsByResource_7) * adjustedValue_11;
    resourceValue = static_cast<i32>(adjustedValue_11);
    benefitCost = adjustedValue_11 / RVConversion(costsByResource_7);
}

void philAI::GetBestBuilding(town* t, BHC& bhc, float& fOut) {
    float score;
    float kn;
    BuildingSlotType node;
    float idx;
    BuildingSlotType jb;
    float nb;
    i32 cost;
    nb = -99.0f;
    kn = -99.0f;
    jb = BUILDING_SLOT_NONE;
    for (node = BUILDING_SLOT_MAGE_GUILD; node < BUILDING_SLOT_COUNT; node++) {
        if (!(t->m_buildings & (1 << H2EnumIndex(node)))
            || (node == BUILDING_SLOT_MAGE_GUILD && t->m_buildState < AI_MAX_MAGE_GUILD_LEVEL)) {
            if (CanBuild(t, node)) {
                ValueOfBuyingBuilding(t, node, cost, idx);
                if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_BUILDER)
                    cost = static_cast<i32>(cost * 1.3);
                score = (Random(1, 5) + 95) * idx / 100.0f;
                if (score > kn) {
                    jb = node;
                    nb = idx;
                    kn = score;
                }
                if (giDebugLevel >= AI_PURCHASE_VALUE_DEBUG_LEVEL) {
                    sprintf(
                        gText,
                        "Town:%2d  Building: %18s   Raw BC = %8.2f,  RandBC = %8.2f.",
                        t->m_id,
                        GetBuildingName(t->m_type, node),
                        idx,
                        score
                    );
                    LogStr(gText);
                }
            }
        }
    }
    bhc.pTown = t;
    bhc.type = PURCHASE_BUILDING;
    bhc.building = jb;
    fOut = nb;
}

void philAI::ValueOfBuyingCreature(
    town* townPtr,
    CreatureType creature,
    i32& resourceValue,
    i32 purchaseCount,
    float& benefitCost
) {
    float chance;
    i32 buyCost[AI_PURCHASE_RESOURCE_COUNT];
    float peril;
    i32 creatureRV;
    i32 costRV;
    float foeStrength;
    i32 turnsToAttack;
    i32 archers;
    i32 weeksLeft;
    i32 points;
    float riskFactor;
    hero* heroPtr;
    i32 troopSlot;
    i32 townSlot;

    archers = 0;
    GetMonsterCost(creature, buyCost);
    costRV = purchaseCount * RVConversion(buyCost);
    creatureRV = static_cast<i32>(
        purchaseCount * gMonsterDatabase[H2EnumIndex(creature)].fightValue
        * gpCurPlayer->m_aiData.m_upgradeValueWeight
    );

    if (townPtr->m_occupyingHeroId != AI_TROOP_EMPTY_SLOT) {
        heroPtr = gpGame->GetHero(townPtr->m_occupyingHeroId);
        creatureRV = static_cast<i32>(creatureRV * AI_CREATURE_VISITING_HERO_FACTOR);
        if (gMonsterDatabase[H2EnumIndex(creature)].race == heroPtr->m_cursorType)
            creatureRV = static_cast<i32>(creatureRV * AI_CREATURE_SAME_RACE_FACTOR);
        if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(creature)].attributes) & (MONSTER_ATTRIBUTE_RANGED)))) {
            for (troopSlot = 0; troopSlot < CREATURE_PURCHASE_ARMY_SLOT_COUNT;
                 troopSlot++) {
                if (heroPtr->m_army.m_creatureTypes[troopSlot]
                        != CREATURE_NONE
                    && (H2EnumIndex((gMonsterDatabase[H2EnumIndex(heroPtr->m_army
                                                 .m_creatureTypes[troopSlot])]
                            .attributes) & (MONSTER_ATTRIBUTE_RANGED)))) {
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
            * (gpGame->m_players[townPtr->m_owner].m_aiData.m_attentionWeights.upgradeBase
               + AI_CREATURE_BALANCE_BASE)
        );
    }

    if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(creature)].attributes) & (MONSTER_ATTRIBUTE_RANGED)))) {
        for (townSlot = 0; townSlot < CREATURE_PURCHASE_ARMY_SLOT_COUNT;
             townSlot++) {
            if (townPtr->m_army.m_creatureTypes[townSlot] != CREATURE_NONE
                && (H2EnumIndex((gMonsterDatabase[H2EnumIndex(townPtr->m_army.m_creatureTypes[townSlot])].attributes) & (MONSTER_ATTRIBUTE_RANGED)))) {
                archers++;
            }
        }
        creatureRV = static_cast<i32>(
            creatureRV
            * (AI_CREATURE_RANGED_BASE_FACTOR - archers * AI_CREATURE_RANGED_STACK_FACTOR)
        );
    }

    LikelihoodOfEnemyAttacking(
        townPtr,
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

void philAI::GetBestCreature(town* townPtr, BHC& best, float& bestValue) {
    i32 numberToBuy;
    float bestRawValue0;
    i32 bestDwelling;
    i32 armyIndex;
    i32 availableUnits;
    i32 dwelling;
    CreatureType candidateMonster;
    float unrandomizedValue0;
    float bestRandomizedScore0;
    i32 weakestArmyValue;
    b32 canAddUnit8;
    float jitteredValue0;
    i32 resourceValue;

    bestDwelling = CREATURE_PURCHASE_NO_SLOT;
    numberToBuy = 0;
    bestRawValue0 = AI_PURCHASE_INITIAL_VALUE;
    bestRandomizedScore0 = AI_PURCHASE_INITIAL_VALUE;
    for (dwelling = 0; dwelling < CREATURE_PURCHASE_DWELLING_COUNT; dwelling++) {
        candidateMonster = gDwellingType[H2EnumIndex(townPtr->m_type)][dwelling];
        weakestArmyValue = CREATURE_PURCHASE_VALUE_LIMIT;
        if ((townPtr->m_buildings & (1 << H2EnumIndex(dwelling + H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST))))
            && townPtr->m_garrison[dwelling] > 0) {
            canAddUnit8 = false;
            for (armyIndex = 0; armyIndex < CREATURE_PURCHASE_ARMY_SLOT_COUNT; armyIndex++) {
                if (townPtr->m_army.m_creatureTypes[armyIndex] == CREATURE_NONE
                    || townPtr->m_army.m_creatureTypes[armyIndex] == candidateMonster) {
                    canAddUnit8 = true;
                }
            }
            for (armyIndex = 0; armyIndex < CREATURE_PURCHASE_ARMY_SLOT_COUNT; armyIndex++) {
                if (townPtr->m_army.m_creatureTypes[armyIndex] != CREATURE_NONE
                    && gMonsterDatabase[H2EnumIndex(townPtr->m_army.m_creatureTypes[armyIndex])].randomValue
                           < weakestArmyValue) {
                    weakestArmyValue =
                        gMonsterDatabase[H2EnumIndex(townPtr->m_army.m_creatureTypes[armyIndex])].randomValue;
                }
            }
            if (gMonsterDatabase[H2EnumIndex(candidateMonster)].randomValue > weakestArmyValue
                && gMonsterDatabase[H2EnumIndex(candidateMonster)].randomValue
                       > CREATURE_PURCHASE_EXPENSIVE_VALUE) {
                canAddUnit8 = true;
            }
            if (dwelling == 0 && townPtr->m_occupyingHeroId != AI_TROOP_EMPTY_SLOT
                && ((townPtr->m_buildings & H2EnumIndex(TOWN_BUILDING_UPGRADED_DWELLING_5))
                    || (townPtr->m_buildings & H2EnumIndex(TOWN_BUILDING_DWELLING_5))
                    || (townPtr->m_buildings & H2EnumIndex(TOWN_BUILDING_UPGRADED_DWELLING_6))
                    || (townPtr->m_buildings & H2EnumIndex(TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6))
                    || (townPtr->m_buildings & H2EnumIndex(TOWN_BUILDING_DWELLING_6)))) {
                canAddUnit8 = false;
            }
            if (canAddUnit8) {
                availableUnits = CreaturesToBuy(townPtr, dwelling);
                if (availableUnits > 0) {
                    ValueOfBuyingCreature(
                        townPtr,
                        candidateMonster,
                        resourceValue,
                        availableUnits,
                        unrandomizedValue0
                    );
                    if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_WARRIOR)
                        resourceValue =
                            static_cast<i32>(resourceValue * AI_CREATURE_EASY_COST_FACTOR);
                    if (townPtr->m_threat != 0)
                        resourceValue <<= 1;
                    jitteredValue0 = static_cast<float>(
                        (Random(1, 10) + AI_CREATURE_RANDOM_BASE)
                        * unrandomizedValue0
                        / AI_PURCHASE_RANDOM_DIVISOR
                    );
                    if (jitteredValue0 > bestRandomizedScore0) {
                        bestDwelling = dwelling;
                        bestRawValue0 = unrandomizedValue0;
                        bestRandomizedScore0 = jitteredValue0;
                        numberToBuy = availableUnits;
                    }
                    if (giDebugLevel >= AI_PURCHASE_VALUE_DEBUG_LEVEL) {
                        sprintf(
                            gText,
                            "Town:%2d  Creature: %18s   Raw BC = %8.2f,  RandBC = %8.2f.",
                            townPtr->m_id,
                            GetMonsterName(candidateMonster),
                            unrandomizedValue0,
                            jitteredValue0
                        );
                        LogStr(gText);
                    }
                }
            }
        }
    }
    best.pTown = townPtr;
    best.type = PURCHASE_CREATURE;
    best.what = bestDwelling;
    best.num = numberToBuy;
    bestValue = bestRawValue0;
}

i32 philAI::CreaturesToBuy(town* t, i32 level) {
    i32 nGarrison = t->m_garrison[level];
    return CreaturesToBuy(gDwellingType[H2EnumIndex(t->m_type)][level], nGarrison);
}

i32 philAI::CreaturesToBuy(CreatureType a, i32 b) {
    i32 n = MaxBuyableCreatures(a);
    if (n > 1)
        n >>= 1;
    if (n > b)
        n = b;
    if (n > 1)
        return n;
    else
        return 0;
}

i32 philAI::MaxBuyableCreatures(CreatureType level) {
    i32 res;
    i32 i;
    i32 cost[AI_PURCHASE_RESOURCE_COUNT];
    GetMonsterCost(level, cost);
    for (i = 0; i < AI_PURCHASE_RESOURCE_COUNT; i++) {
        if (cost[i] == 0)
            res = CREATURE_PURCHASE_UNLIMITED;
        else if (gpCurPlayer->m_resources[i] > 0)
            res = gpCurPlayer->m_resources[i] / cost[i];
        else
            res = 0;
    }
    return res;
}

void philAI::ValueOfBuyingHero(
    town* townPtr,
    hero* heroPtr,
    i32& resourceValue,
    float& benefitCost
) {
    i32 heroCost[AI_PURCHASE_RESOURCE_COUNT];
    i32 rvCost;
    b32 spellCaster;
    i32 i;

    i32 heroRV;

    heroCost[H2EnumIndex(RES_WOOD)] = 0;
    heroCost[H2EnumIndex(RES_MERCURY)] = 0;
    heroCost[H2EnumIndex(RES_ORE)] = 0;
    heroCost[H2EnumIndex(RES_SULFUR)] = 0;
    heroCost[H2EnumIndex(RES_CRYSTAL)] = 0;
    heroCost[H2EnumIndex(RES_GEMS)] = 0;
    heroCost[H2EnumIndex(RES_GOLD)] = AI_HERO_PURCHASE_GOLD_COST;
    rvCost = RVConversion(heroCost);
    heroRV = heroPtr->m_experience + AI_HERO_PURCHASE_EXPERIENCE_BASE;
    for (i = 0; i < AI_BATTLE_ARTIFACT_SLOT_COUNT; i++) {
        if (heroPtr->m_artifacts[i] >= ARTIFACT_ULTIMATE_BOOK
            && heroPtr->m_artifacts[i] < ARTIFACT_EDITOR_ANY_ULTIMATE
            && heroPtr->m_artifacts[i] != ARTIFACT_MAGIC_BOOK) {
            heroRV += gArtifactBaseRV[H2EnumIndex(heroPtr->m_artifacts[i])];
        }
    }
    heroRV += heroPtr->m_experience / 2;
    heroRV = static_cast<i32>(
        heroRV
        * (gpCurPlayer->m_aiData.m_attentionWeights.heroValue + 1.0
           - gpCurPlayer->m_aiData.m_attentionWeights.upgradeBase)
    );
    if (heroPtr->m_cursorType == FACTION_SORCERESS
        || heroPtr->m_cursorType == FACTION_WARLOCK
        || heroPtr->m_cursorType == FACTION_WIZARD
        || heroPtr->m_cursorType == FACTION_NECROMANCER)
        spellCaster = true;
    else
        spellCaster = false;
    if (townPtr->m_type == heroPtr->m_cursorType) {
        heroRV *= AI_HERO_PURCHASE_SAME_RACE_FACTOR + AI_HERO_PURCHASE_IDENTITY;
    } else if ((townPtr->m_buildState >= 2 && spellCaster)
               || (townPtr->m_buildState < 2 && !spellCaster)) {
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

void philAI::GetBestHero(town* townPtr, BHC& best, float& bestValue) {
    float bestRawLocal6;
    i32 heroIndex;
    i32 bestHeroIndex10;
    float bestScore;
    float benefitCost9;
    float randomizedScore;
    i32 resourceValue;
    hero* heroPtr;

    bestHeroIndex10 = CREATURE_PURCHASE_NO_SLOT;
    bestRawLocal6 = AI_PURCHASE_INITIAL_VALUE;
    bestScore = AI_PURCHASE_INITIAL_VALUE;
    for (heroIndex = 0; heroIndex < HERO_AVAILABLE_SLOT_COUNT; heroIndex++) {
        heroPtr = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[heroIndex]];
        ValueOfBuyingHero(townPtr, heroPtr, resourceValue, benefitCost9);
        randomizedScore = static_cast<float>(
            benefitCost9 * (Random(1, 10) + AI_HERO_PURCHASE_RANDOM_BASE)
            / AI_PURCHASE_RANDOM_DIVISOR
        );
        if (randomizedScore > bestScore) {
            bestHeroIndex10 = heroIndex;
            bestRawLocal6 = benefitCost9;
            bestScore = randomizedScore;
        }
        if (giDebugLevel >= AI_PURCHASE_VALUE_DEBUG_LEVEL) {
            sprintf(
                gText,
                "Town:%2d  Hero    : % 15i   Raw BC = %8.2f,  RandBC = %8.2f.",
                townPtr->m_id,
                heroIndex,
                benefitCost9,
                randomizedScore
            );
            LogStr(gText);
        }
    }
    best.pTown = townPtr;
    best.type = PURCHASE_HERO;
    best.what = bestHeroIndex10;
    bestValue = bestRawLocal6;
    if (gpGame->m_worldMap.GetCell(townPtr->m_x, townPtr->m_y)->m_triggerType
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
    nValue = static_cast<i32>(static_cast<float>(nAttack) * chanceA);
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
    playerAttentionWeights* ptr = &gpGame->m_players[player].m_aiData.m_attentionWeights;
    ptr->gameWeightA = 0.4f;
    ptr->gameWeightB = 0.3f;
    ptr->gameRemainder = 0.3f;
    ptr->buildingValue = ptr->gameWeightA;
    ptr->heroValue = ptr->gameWeightB;
    ptr->upgradeBase = ptr->gameRemainder;
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
    ptr->heroValue = ptr->heroValue * scale;
}

i32 philAI::RVConversion(i32* const p) {
    return static_cast<i32>(
        ((((((static_cast<float>(p[H2EnumIndex(RES_GOLD)])
              * gafAITurnCostResource[H2EnumIndex(RES_GOLD)])
             + static_cast<float>(p[H2EnumIndex(RES_WOOD)])
                   * gafAITurnCostResource[H2EnumIndex(RES_WOOD)])
            + static_cast<float>(p[H2EnumIndex(RES_ORE)])
                  * gafAITurnCostResource[H2EnumIndex(RES_ORE)])
           + static_cast<float>(p[H2EnumIndex(RES_CRYSTAL)])
                 * gafAITurnCostResource[H2EnumIndex(RES_CRYSTAL)])
          + static_cast<float>(p[H2EnumIndex(RES_SULFUR)])
                * gafAITurnCostResource[H2EnumIndex(RES_SULFUR)])
         + static_cast<float>(p[H2EnumIndex(RES_MERCURY)])
               * gafAITurnCostResource[H2EnumIndex(RES_MERCURY)])
        + static_cast<float>(p[H2EnumIndex(RES_GEMS)])
              * gafAITurnCostResource[H2EnumIndex(RES_GEMS)]
    );
}

float philAI::TurnsToBuy(i32* const p) {
    float maxT = 0;
    i32 res;
    float turnCount;
    for (res = 0; res < AI_PURCHASE_RESOURCE_COUNT; res++) {
        if (gpCurPlayer->m_resources[res] < p[res]) {
            if (gpCurPlayer->m_aiData.m_income[res] > 0)
                turnCount = static_cast<float>(
                    (p[res] - gpCurPlayer->m_resources[res]) / gpCurPlayer->m_aiData.m_income[res] + 1
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
    i32 adjacentMonsterEventChance6;
    i32 adjacentY5;
    float distanceFactor3;
    i32 adjacentX9;
    i32 primaryEventChance1;
    i32 adjacentEventChance3;
    MapObjectType objectType6;
    i32 totalValue2;
    i32 strategicLiveChance0;
    i32 strategicEventValue0;
    i32 currentStrategicValue3;
    MapObjectType debugObjectType0;

    i32 targetLiveChance12;
    i32 eventValue5;
    i32 strategicDelta5;
    MapTriggerCode triggerType7;
    i32 currentLiveChance1;
    char debugText1[POSITION_DEBUG_TEXT_CAPACITY];

    strategicEventValue0 = 0;
    targetLiveChance12 = POSITION_FULL_CHANCE;
    adjacentEventChance3 = POSITION_FULL_CHANCE;
    triggerType7 = gpAdvManager->GetCell(x, y)->m_triggerType;
    objectType6 = triggerType7 & MAP_TRIGGER_TYPE_MASK;
    primaryEventChance1 = POSITION_FULL_CHANCE;
    strategicLiveChance0 = POSITION_FULL_CHANCE;
    adjacentMonsterEventChance6 = POSITION_FULL_CHANCE;

    if (abs(x - gpCurAIHero->m_x) <= AI_POSITION_NEARBY_DELTA
        && abs(y - gpCurAIHero->m_y) <= AI_POSITION_NEARBY_DELTA) {
        currentStrategicValue3 = 0;
        strategicDelta5 = 0;
    } else {
        currentStrategicValue3 = StrategicValueOfPosition(
            gpCurAIHero->m_x,
            gpCurAIHero->m_y,
            0,
            1,
            &currentLiveChance1,
            0
        );
        strategicDelta5 = StrategicValueOfPosition(x, y, 0, 1, &targetLiveChance12, extraDistance);
        strategicDelta5 -= currentStrategicValue3;
    }
    if (objectType6 == MAP_OBJECT_BOAT && strategicDelta5 < 0)
        strategicDelta5 = 0;

    totalValue2 = 0;
    if (hasEvent)
        totalValue2 += ValueOfEventAtPosition(eventX, eventY, 1, &strategicLiveChance0);
    if (hasStrategicEvent) {
        strategicEventValue0 =
            StrategicValueOfPosition(strategicX, strategicY, 1, 1, &adjacentEventChance3, 0);
        if (strategicEventValue0 < 0)
            totalValue2 += strategicEventValue0;
    }

    if (gpAdvManager->FindAdjacentMonster(x, y, &adjacentX9, &adjacentY5, -1, -1)) {
        if (StopOnTrigger(gpAdvManager->GetCell(x, y))) {
            eventValue5 =
                ValueOfEventAtPosition(adjacentX9, adjacentY5, 1, &adjacentMonsterEventChance6);
            if (eventValue5 < 0)
                totalValue2 += eventValue5;
            if (strategicLiveChance0 == POSITION_FULL_CHANCE)
                strategicLiveChance0 = adjacentMonsterEventChance6;
            else
                strategicLiveChance0 = strategicLiveChance0 * adjacentMonsterEventChance6
                                      / POSITION_FULL_CHANCE;
        }
    }

    if ((H2EnumIndex((triggerType7) & (MAP_TRIGGER_ACTION_FLAG)))
        || (x == gpCurPlayer->m_ultimateArtifactHintX
            && y == gpCurPlayer->m_ultimateArtifactHintY)) {
        eventValue5 = ValueOfEventAtPosition(x, y, eventMode, &primaryEventChance1);
    } else {
        eventValue5 = 0;
    }
    if (primaryEventChance1 < POSITION_FULL_CHANCE)
        strategicDelta5 = strategicDelta5 * primaryEventChance1 / POSITION_FULL_CHANCE;

    if (targetLiveChance12 < POSITION_MINIMUM_LIVE_CHANCE)
        return POSITION_FAILED_VALUE;
    if (targetLiveChance12 < POSITION_FULL_CHANCE) {
        eventValue5 = eventValue5 * targetLiveChance12 / POSITION_FULL_CHANCE;
        strategicDelta5 = strategicDelta5 * targetLiveChance12 / POSITION_FULL_CHANCE;
    }
    if (adjacentEventChance3 < POSITION_MINIMUM_LIVE_CHANCE)
        return POSITION_FAILED_VALUE;
    if (adjacentEventChance3 < POSITION_FULL_CHANCE) {
        eventValue5 = eventValue5 * adjacentEventChance3 / POSITION_FULL_CHANCE;
        strategicDelta5 = strategicDelta5 * adjacentEventChance3 / POSITION_FULL_CHANCE;
    }
    if (strategicLiveChance0 < POSITION_FULL_CHANCE) {
        if (totalValue2 > 0) {
            totalValue2 = (totalValue2 + eventValue5 + strategicDelta5) * strategicLiveChance0
                         / POSITION_FULL_CHANCE;
        } else {
            totalValue2 +=
                (eventValue5 + strategicDelta5) * strategicLiveChance0 / POSITION_FULL_CHANCE;
        }
    } else {
        totalValue2 += eventValue5;
    }

    distanceFactor3 = static_cast<float>(gpSearchArray->GetRow(x, 1)[MAP_WIDTH * y].distance)
                     / gpCurAIHero->m_mobility;
    if ((H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))) {
        distanceFactor3 = static_cast<float>(
            distanceFactor3 * AI_POSITION_EMBARKED_DISTANCE_FACTOR
            + AI_POSITION_EMBARKED_DISTANCE_FACTOR
        );
    } else if (distanceFactor3 > AI_POSITION_LAND_DISTANCE_6) {
        distanceFactor3 *= AI_POSITION_LAND_FACTOR_FAR;
    } else if (distanceFactor3 > AI_POSITION_LAND_DISTANCE_5) {
        distanceFactor3 = static_cast<float>(distanceFactor3 * AI_POSITION_LAND_FACTOR_6);
    } else if (distanceFactor3 > AI_POSITION_LAND_DISTANCE_4) {
        distanceFactor3 = static_cast<float>(distanceFactor3 * AI_POSITION_LAND_FACTOR_5);
    } else if (distanceFactor3 > AI_POSITION_LAND_DISTANCE_3) {
        distanceFactor3 = static_cast<float>(distanceFactor3 * AI_POSITION_LAND_FACTOR_4);
    } else if (distanceFactor3 > AI_POSITION_LAND_DISTANCE_2) {
        distanceFactor3 = static_cast<float>(distanceFactor3 * AI_POSITION_LAND_FACTOR_3);
    } else if (distanceFactor3 > AI_POSITION_LAND_DISTANCE_1) {
        distanceFactor3 = static_cast<float>(distanceFactor3 * AI_POSITION_LAND_FACTOR_2);
    }

    totalValue2 = static_cast<i32>(totalValue2 / (distanceFactor3 + AI_POSITION_DISTANCE_BASE));
    strategicDelta5 = static_cast<i32>(
        strategicDelta5 * POSITION_STRATEGIC_MULTIPLIER
        / (AI_POSITION_STRATEGIC_DISTANCE_BASE + distanceFactor3)
    );
    if (strategicLiveChance0 == POSITION_FULL_CHANCE)
        totalValue2 += strategicDelta5;
    if ((H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_EMBARKED))) && triggerType7 == MAP_OBJECT_COAST) {
        totalValue2 += POSITION_EMBARKED_BOAT_BONUS;
    }

    if (giDebugLevel > POSITION_DEBUG_LEVEL - 1) {
        debugObjectType0 = triggerType7 & MAP_TRIGGER_TYPE_MASK;
        sprintf(
            debugText1,
            "FUN U :%15s",
            (debugObjectType0 > MAP_OBJECT_NONE
             && H2EnumIndex(debugObjectType0) < POSITION_OBJECT_NAME_COUNT)
                ? gQuickViewText[H2EnumIndex(debugObjectType0)]
                : " "
        );
        LogInt(
            debugText1,
            x,
            y,
            totalValue2,
            eventValue5,
            strategicDelta5,
            targetLiveChance12 * POSITION_FULL_CHANCE,
            POSITION_DEBUG_UNUSED
        );
    }
    return totalValue2;
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

    i32 eventPoints;
    H2EnumStorage<TerrainType, i32> terrain2;
    i32 xPos;
    i32 inBoat;
    i32 dangerVal;
    b32 madeSearch;

    i32 worth;
    i32 yy;
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
        && (GetCastleSlot(cell->m_objectMetadata)->m_buildings
            & H2EnumIndex(TOWN_BUILDING_CASTLE))) {
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

    inBoat = (H2EnumIndex((gpCurAIHero->m_eventFlags) & (HERO_EVENT_EMBARKED)));
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

    for (xPos = 0; xPos < MAP_WIDTH; xPos++) {
        for (yy = 0; yy < MAP_HEIGHT; yy++) {
            if (search->GetNode(xPos, yy).visited) {
                cell = gpAdvManager->GetCell(xPos, yy);
                if ((!immediate && (H2EnumIndex((cell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))
                     && search->GetNode(xPos, yy).distance <= nearDistance)
                    || (checkEnemies
                        && cell->m_triggerType
                               == (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION)))) {
                    CheckDoMain(0, 0);
                    eventPoints = static_cast<i32>(
                        (ValueOfEventAtPosition(xPos, yy, 0, &iDummy) * 25)
                        / (search->GetNode(xPos, yy).distance + AI_STRATEGIC_DISTANCE_OFFSET)
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
                        ValueOfEventAtPosition(xPos, yy, 0, &iDummy);
                    if (gaiHeroLiveChance[cell->m_objectMetadata] != RV_UNSET
                        && gaiHeroLiveChance[cell->m_objectMetadata] < 100) {
                        if (!townOK) {
                            enemyRange = GetHeroSlot(cell->m_objectMetadata)->m_mobility;
                            if (gbHumanPlayer[gpGame->m_availableHeroes[cell->m_objectMetadata]]) {
                                if (search->GetNode(xPos, yy).distance + extraDistance
                                    <= enemyRange) {
                                    if (search->GetNode(xPos, yy).distance + extraDistance
                                        <= 500) {
                                        dangerVal = 100 - gaiHeroLiveChance[cell->m_objectMetadata];
                                    } else {
                                        dangerVal =
                                            (100 - gaiHeroLiveChance[cell->m_objectMetadata])
                                            * ((enemyRange
                                                - (search->GetNode(xPos, yy).distance
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
                                       - (search->GetNode(xPos, yy).distance + extraDistance))
                                    / (enemyRange + 500);
                            }
                            *liveChance = *liveChance * (100 - dangerVal) / 100;
                        }
                    }
                }

                if (gpCurAIHero->m_id != iAlphaMale && (targetX != xPos || targetY != yy)
                    && search->GetNode(xPos, yy).distance < 1024
                    && gpAdvManager->GetCell(xPos, yy)->m_triggerType
                           == (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))
                    && gpAdvManager->GetCell(xPos, yy)->m_objectMetadata != gpCurAIHero->m_id
                    && gpGame->m_availableHeroes[gpAdvManager->GetCell(xPos, yy)
                                                     ->m_objectMetadata]
                           == gpCurAIHero->m_owner) {
                    penalty = (1024 - search->GetNode(xPos, yy).distance) * 600 >> 10;
                    if (penalty > 0)
                        worth -= penalty;
                }
            }
        }
    }

    targetTerrain =
        giGroundToTerrain[gpAdvManager->GetCell(targetX, targetY)->m_terrainImageIndex];
    for (heroIndex = 0; heroIndex < gpCurPlayer->m_heroCount; heroIndex++) {
        if (gpCurPlayer->m_heroIds[heroIndex] != gpCurAIHero->m_id) {
            gap =
                abs(gpGame->m_heroRecs[gpCurPlayer->m_heroIds[heroIndex]].m_destinationX
                    - targetX)
                + abs(gpGame->m_heroRecs[gpCurPlayer->m_heroIds[heroIndex]].m_destinationY
                      - targetY);
            if (gap < 9) {
                terrain2 = giGroundToTerrain
                    [gpAdvManager
                         ->GetCell(
                             gpGame->m_heroRecs[gpCurPlayer->m_heroIds[heroIndex]]
                                 .m_destinationX,
                             gpGame->m_heroRecs[gpCurPlayer->m_heroIds[heroIndex]]
                                 .m_destinationY
                         )
                         ->m_terrainImageIndex];
                if (!((targetTerrain == TERRAIN_WATER && terrain2 > TERRAIN_WATER)
                      || (targetTerrain > TERRAIN_WATER
                          && terrain2 == TERRAIN_WATER))) {
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

i32 philAI::ValueOfTown(town* t) {
    i32 sum = 0;
    BuildingSlotType building;
    for (building = BUILDING_SLOT_MAGE_GUILD; building <= BUILDING_SLOT_DWELLING_SIXTH;
         building++) {
        if (t->m_buildings & (1 << H2EnumIndex(building)))
            sum += GetBuildingBaseResourceValue(t->m_type, building, t->m_buildState);
    }
    sum = static_cast<i32>(
        sum
        + AI_GOLD_TURN_VALUE_SCALE * gafAITurnCostResource[H2EnumIndex(RES_GOLD)] * 5.0f * 1.5f
    );
    sum += 750;
    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_TOWN
        && gpGame->m_mapHeader.lossConditionValue == t->m_x
        && gpGame->m_mapHeader.lossTownY == t->m_y)
        sum += 50000;
    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_CAPTURE_TOWN
        && gpGame->m_mapHeader.victoryConditionValue == t->m_x
        && gpGame->m_mapHeader.victoryTownY == t->m_y)
        sum += 50000;
    return sum;
}

void philAI::TurnCostResource(i32 player) {
    playerAIData* pAI;
    float frac[AI_PURCHASE_RESOURCE_COUNT];
    float avg;
    i32 res;
    i32 sum;
    i32 resValue[AI_PURCHASE_RESOURCE_COUNT];
    pAI = &gpGame->m_players[player].m_aiData;
    sum = 0;
    for (res = 0; res < AI_PURCHASE_RESOURCE_COUNT; res++) {
        resValue[res] = static_cast<i32>(
            static_cast<double>(gResourceBaseValue[res])
            * (static_cast<double>(pAI->m_income[res] * 5) * 0.7
               + static_cast<double>(gpGame->m_players[player].m_resources[res]))
        );
        sum += resValue[res];
    }
    avg = static_cast<float>(sum / AI_PURCHASE_RESOURCE_COUNT);
    for (res = 0; res < AI_PURCHASE_RESOURCE_COUNT; res++) {
        frac[res] = static_cast<float>(resValue[res]) / avg;
        gafAITurnCostResource[res] =
            static_cast<float>(gResourceBaseValue[res] / (frac[res] / 2.0f + 0.5));
    }
}

float philAI::TurnValueOfObelisk(i32 player) {
    playerAIData* ai;
    i32 each;
    i32 artRV;
    ai = &gpGame->m_players[player].m_aiData;
    artRV = gArtifactBaseRV[H2EnumIndex(gpGame->m_ultimateArtifactId)];
    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_FIND_ARTIFACT)
        artRV <<= 1;
    each = artRV / 110;
    if (gpGame->m_ultimateArtifactId == ARTIFACT_NONE)
        return 0.0f;
    ai->m_obeliskValue = each * GAME_OBELISK_VISITOR_COUNT / gpGame->m_obeliskCount;
    if (gpCurPlayer->m_aiDifficulty == PLAYER_PERSONALITY_EXPLORER)
        ai->m_obeliskValue = static_cast<i32>(ai->m_obeliskValue * 1.4);
    ai->m_obeliskValue = static_cast<i32>(
        ai->m_obeliskValue
        * (1.5
           - abs(GAME_OBELISK_VISITOR_COUNT - gpGame->SetupPuzzlePieces(giCurPlayer, 1))
                 / AI_OBELISK_VISITOR_COUNT_FLOAT)
    );
    ai->m_obeliskValue =
        static_cast<i32>(ai->m_obeliskValue * (ai->m_attentionWeights.heroValue + 0.66));
    return static_cast<float>(ai->m_obeliskValue);
}

float philAI::FutureDeflator(i32* const p) {
    float t = TurnsToBuy(p);
    float value = 1.0f - t * AI_FUTURE_DEFLATION_RATE;
    if (value < AI_FUTURE_MINIMUM_VALUE)
        value = 0;
    return value;
}

i32 philAI::FightValueOfStack(
    armyGroup* group,
    hero* heroPtr,
    i32 useHero,
    i32 useTown,
    i32 townId,
    i32 useEnemyMods
) {
    i32 armyValue;

    town* townPtr;
    float closeCombat;
    i32 castleValue;
    float countMod;
    i32 t;
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
    i32 bestSpellIdx;
    float magicMod;

    armyValue = 0;
    magicTotal = 0;
    castleValue = 0;
    archerMod = 0.9f;
    closeCombat = 1.1f;
    flyMod = 0.75f;

    townPtr = NULL;

    if (useEnemyMods) {
        if (heroPtr->HasArtifact(ARTIFACT_BALLISTA) || heroPtr->HasSpell(SPELL_EARTHQUAKE)
            || heroPtr->m_secondarySkills[H2EnumIndex(HERO_SKILL_BALLISTICS)]
                   != HERO_SKILL_LEVEL_NONE) {
            closeCombat = 1.05f;
            flyMod = 0.95f;
        }
        if (heroPtr->m_secondarySkills[H2EnumIndex(HERO_SKILL_ARCHERY)] != HERO_SKILL_LEVEL_NONE
            || heroPtr->HasArtifact(ARTIFACT_GOLDEN_BOW))
            archerMod = 1.05f;
    }
    if (useTown) {
        townPtr = GetCastleSlot(townId);
        if (!(townPtr->m_buildings & AI_BUILDING_CASTLE_MASK))
            useTown = 0;
    }

    for (slot = 0; slot < AI_TOWN_ARMY_SLOTS; slot++) {
        if (group->m_creatureTypes[slot] != CREATURE_NONE) {
            stackWorth = group->m_quantities[slot]
                       * gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].fightValue;
            if (useHero) {
                if (group->m_quantities[slot] > 500)
                    countMod = 4.4f;
                else if (group->m_quantities[slot] > 300)
                    countMod = 3.9f;
                else if (group->m_quantities[slot] > 180)
                    countMod = 3.08f;
                else if (group->m_quantities[slot] > 140)
                    countMod = 2.42f;
                else if (group->m_quantities[slot] > 100)
                    countMod = 2.1f;
                else if (group->m_quantities[slot] > 75)
                    countMod = 1.63f;
                else if (group->m_quantities[slot] > 50)
                    countMod = 1.4f;
                else if (group->m_quantities[slot] > 35)
                    countMod = 1.0f;
                else if (group->m_quantities[slot] > 23)
                    countMod = 0.75f;
                else if (group->m_quantities[slot] > 16)
                    countMod = 0.5f;
                else if (group->m_quantities[slot] > 11)
                    countMod = 0.32f;
                else if (group->m_quantities[slot] > 8)
                    countMod = 0.21f;
                else if (group->m_quantities[slot] > 5)
                    countMod = 0.0;
                else if (group->m_quantities[slot] > 3)
                    countMod = -0.1f;
                else if (group->m_quantities[slot] > 2)
                    countMod = -0.3f;
                else if (group->m_quantities[slot] > 1)
                    countMod = -0.4f;
                else
                    countMod = -0.58f;

                if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].attributes) & (MONSTER_ATTRIBUTE_RANGED)))
                    || group->m_creatureTypes[slot] == CREATURE_VAMPIRE
                    || group->m_creatureTypes[slot] == CREATURE_VAMPIRE_LORD
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
                    if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].attributes) & (MONSTER_ATTRIBUTE_RANGED))))
                        stackWorth = static_cast<i32>(stackWorth * 1.18);
                    if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].attributes) & (MONSTER_ATTRIBUTE_FLYING))))
                        stackWorth =
                            static_cast<i32>(stackWorth * AI_TOWN_FLYING_CREATURE_FACTOR);
                }
                if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].attributes) & (MONSTER_ATTRIBUTE_RANGED)))
                    && heroPtr
                    && heroPtr->m_secondarySkills[H2EnumIndex(HERO_SKILL_ARCHERY)]
                           != HERO_SKILL_LEVEL_NONE) {
                    stackWorth = static_cast<i32>(
                        stackWorth
                        * gfSSAIArcheryMod
                            [H2EnumIndex(heroPtr->m_secondarySkills[H2EnumIndex(HERO_SKILL_ARCHERY)])]
                    );
                }
                if (useEnemyMods) {
                    if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].attributes) & (MONSTER_ATTRIBUTE_RANGED))))
                        stackWorth = static_cast<i32>(stackWorth * archerMod);
                    if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(group->m_creatureTypes[slot])].attributes) & (MONSTER_ATTRIBUTE_FLYING))))
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
        townPtr->CalcNumLevelArchers(&numShooters, &archerGrade);
        if (townPtr->m_buildings & AI_BUILDING_LEFT_TURRET_MASK)
            towerArchers += numShooters >> 1;
        if (townPtr->m_buildings & AI_BUILDING_RIGHT_TURRET_MASK)
            towerArchers += numShooters >> 1;
        numShooters += towerArchers;
        castleValue = static_cast<i32>(numShooters * 400 * (archerGrade * 0.1 + 1.0));
    }

    if (useHero && heroPtr) {
        statSum =
            heroPtr->Stats(HERO_PRIMARY_ATTACK) + heroPtr->Stats(HERO_PRIMARY_DEFENSE) + 20;
        if (statSum < 0)
            statSum = 0;
        if (statSum > 40)
            statSum = 40;
        armyValue = static_cast<i32>(armyValue * gfStatPower[statSum]);
        castleValue =
            static_cast<i32>(castleValue * gfStatPower[statSum]);

        morale = heroPtr->m_army.GetMorale(heroPtr, heroPtr->GetOccupiedTown(), NULL);
        if (morale > 0)
            armyValue = armyValue * (morale + 48) / 48;
        else if (morale < 0)
            armyValue = armyValue * (morale + 24) / 24;

        heroLuck = gpGame->GetLuck(heroPtr, NULL, useTown ? townPtr : NULL);
        if (heroLuck)
            armyValue = armyValue * (heroLuck + 16) / 16;

        if (heroPtr->m_spellPoints >= 3) {
            spellCount = 0;
            if (heroPtr->Stats(HERO_PRIMARY_SPELL_POWER) <= FIGHT_VALUE_SPELL_STAT_LIMIT) {
                lengthMod =
                    gfPhilAIDurationMod[heroPtr->Stats(HERO_PRIMARY_SPELL_POWER)];
                magicMod =
                    gfPhilAISpellPowerMod[heroPtr->Stats(HERO_PRIMARY_SPELL_POWER)];
            } else {
                lengthMod = gfPhilAIDurationMod[FIGHT_VALUE_SPELL_STAT_LIMIT];
                magicMod = gfPhilAISpellPowerMod[FIGHT_VALUE_SPELL_STAT_LIMIT];
            }
            for (slot = 0; slot < FIGHT_VALUE_SPELL_COUNT; slot++) {
                if (heroPtr->HasSpell(SpellType(slot))) {
                    spellScore = gsSpellInfo[slot].aiValue;
                    if ((H2EnumIndex((gsSpellInfo[slot].attributes) & (SPELL_INFO_ATTRIBUTE_DURATION))))
                        spellScore = static_cast<i32>(spellScore * lengthMod);
                    else if ((H2EnumIndex((gsSpellInfo[slot].attributes) & (SPELL_INFO_ATTRIBUTE_POWER))))
                        spellScore = static_cast<i32>(spellScore * magicMod);
                    numSpellCasts =
                        heroPtr->m_spellPoints / GetManaCost(SpellType(slot), heroPtr);
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
                bestSpellIdx = -1;
                for (t = 0; t < spellCount; t++) {
                    if (spellScores[t] > maxScore) {
                        maxScore = spellScores[t];
                        bestSpellIdx = t;
                    }
                }
                if (bestSpellIdx == -1)
                    break;
                magicTotal = static_cast<i32>(
                    magicTotal + maxScore * gfSpellTypeNumMod[slot]
                );
                spellScores[bestSpellIdx] = 0;
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
        LogInt(
            "FV3",
            armyValue,
            magicTotal,
            castleValue,
            0,
            0,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE
        );
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
    i32 replacementStackValue7;
    i32 leastStackValue3;
    i32 purchaseFightValue15;
    i32 armyIndex3;

    purchaseCount = 0;
    purchaseValue = 0;
    replacementSlot = CREATURE_PURCHASE_NO_SLOT;
    leastStackValue3 = CREATURE_PURCHASE_VALUE_LIMIT;
    if (useAvailableCount != 0)
        purchaseCount = availableCount;
    else
        purchaseCount = MaxBuyableCreatures(creature);
    if (purchaseCount > availableCount)
        purchaseCount = availableCount;

    if (purchaseCount == 0)
        return;
    {
        purchaseFightValue15 = purchaseCount * gMonsterDatabase[H2EnumIndex(creature)].fightValue;
        if (gpCurAIHero->m_army.CanJoin(creature) == 0) {
            for (armyIndex3 = 0; armyIndex3 < CREATURE_PURCHASE_ARMY_SLOT_COUNT; armyIndex3++) {
                if (gpCurAIHero->m_army.m_creatureTypes[armyIndex3] == creature) {
                    replacementSlot = CREATURE_PURCHASE_NO_SLOT;
                    armyIndex3 = CREATURE_PURCHASE_ARMY_SLOT_COUNT;
                } else {
                    replacementStackValue7 = gpCurAIHero->m_army.m_quantities[armyIndex3]
                                             * gMonsterDatabase[armyIndex3].fightValue;
                    if (replacementStackValue7 < leastStackValue3) {
                        leastStackValue3 = replacementStackValue7;
                        replacementSlot = armyIndex3;
                    }
                }
            }
        }
        if (replacementSlot != CREATURE_PURCHASE_NO_SLOT)
            purchaseFightValue15 -= leastStackValue3;

        purchaseValue = static_cast<i32>(
            purchaseFightValue15
            * gpGame->m_players[gpCurAIHero->m_owner].m_aiData.m_upgradeValueWeight
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

    i32 attTotal;
    i32 result;
    i32 atkExp;
    float diff;
    i32 defenderCount;
    hero* defeated;
    hero* winHero;
    float fracLost;
    i32 attackerDead;

    b32 attackerWin;
    i32 slot;

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

    ProbableOutcomeOfBattle(
        attacker,
        attackerHero,
        defender,
        defenderHero,
        NULL,
        townBattle,
        townId,
        (defenderHero != NULL ? defenderHero->m_owner : H2EnumIndex(AI_BATTLE_NO_PLAYER)),
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

    } else {
        winnerChance = AI_BATTLE_FULL_CHANCE - chanceWin;

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
    for (slot = 0; slot < CREATURE_PURCHASE_ARMY_SLOT_COUNT; slot++) {
        if (attackerHero->m_army.m_creatureTypes[slot] != CREATURE_NONE)
            attTotal += attackerHero->m_army.m_quantities[slot];
        if (defenderHero != NULL
            && defenderHero->m_army.m_creatureTypes[slot] != CREATURE_NONE)
            defenderCount += defenderHero->m_army.m_quantities[slot];
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
    float transferFraction3;
    i32 recipientFightValue0;
    i32 dominantFightValue4;
    i32 heroIndex0;
    float armyShare7;
    i32 heroValues6[HERO_INTERACTION_HERO_COUNT];
    ArtifactType artifactType4;
    float dominantShare0;
    hero* dominantHero0;
    i32 interactionValue1;
    i32 statIndex10;
    hero* recipientHero0;
    hero* currentHero9;
    float desiredShare6;

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

        for (heroIndex0 = 0; heroIndex0 < HERO_INTERACTION_HERO_COUNT; heroIndex0++) {
            currentHero9 = heroIndex0 == 0 ? firstHero : secondHero;
            heroValues6[heroIndex0] = 0;
            for (statIndex10 = 0; statIndex10 < HERO_INTERACTION_PRIMARY_STAT_COUNT;
                 statIndex10++) {
                if (statIndex10 < HERO_INTERACTION_COMBAT_PRIMARY_STAT_COUNT
                    || currentHero9->HasArtifact(ARTIFACT_MAGIC_BOOK))
                    heroValues6[heroIndex0] += currentHero9->Stats(HeroPrimaryStat(statIndex10))
                                                * HERO_INTERACTION_PRIMARY_STAT_VALUE;
            }
            for (statIndex10 = H2EnumIndex(HERO_SKILL_PATHFINDING); statIndex10 < H2EnumIndex(HERO_SKILL_COUNT);
                 statIndex10++) {
                if (HeroSecondarySkill(statIndex10) == HERO_SKILL_ESTATES)
                    continue;
                if (currentHero9->m_secondarySkills[statIndex10] != HERO_SKILL_LEVEL_NONE) {
                    heroValues6[heroIndex0] +=
                        gSSValues[statIndex10]
                                 [H2EnumIndex(currentHero9->m_secondarySkills[statIndex10])
                                  - SECONDARY_SKILL_LEVEL_OFFSET];
                }
            }
        }

        dominantShare0 = static_cast<float>(heroValues6[0])
                          / static_cast<float>(heroValues6[0] + heroValues6[1]);
        if (heroValues6[0] > heroValues6[1]) {
            dominantHero0 = firstHero;
            recipientHero0 = secondHero;
        } else {
            dominantHero0 = secondHero;
            recipientHero0 = firstHero;
            dominantShare0 = static_cast<float>(1.0 - dominantShare0);
        }
        if (evaluateOnly != 0 && recipientHero0->m_id == iAlphaMale
            && dominantShare0 < AI_HERO_INTERACTION_ALPHA_SHARE) {

            recipientHero0 = dominantHero0;
            dominantHero0 = recipientHero0;
        }

        dominantFightValue4 = FightValueOfStack(&dominantHero0->m_army, NULL, 0, 0, 0, 0);
        recipientFightValue0 = FightValueOfStack(&recipientHero0->m_army, NULL, 0, 0, 0, 0);
        desiredShare6 = AI_HERO_INTERACTION_TARGET_SHARE;
        armyShare7 = static_cast<float>(dominantFightValue4)
                     / static_cast<float>(dominantFightValue4 + recipientFightValue0);
        transferFraction3 = (desiredShare6 < armyShare7) ? (armyShare7 - desiredShare6)
                                                           : (desiredShare6 - armyShare7);
        if (transferFraction3 < AI_HERO_INTERACTION_MINIMUM_TRANSFER)
            return;
        {
            interactionValue1 = static_cast<i32>(
                gpCurPlayer->m_aiData.m_upgradeValueWeight
                * (static_cast<float>(dominantFightValue4 + recipientFightValue0)
                   * transferFraction3)
            );
            if (evaluateOnly != 0) {
                for (statIndex10 = 0; statIndex10 < AI_BATTLE_ARTIFACT_SLOT_COUNT; statIndex10++) {
                    artifactType4 = recipientHero0->m_artifacts[statIndex10];
                    if (artifactType4 != ARTIFACT_NONE && artifactType4 != ARTIFACT_MAGIC_BOOK)
                        interactionValue1 += gArtifactBaseRV[H2EnumIndex(artifactType4)];
                }
            } else {
                gpAdvManager->TransferArtifacts(recipientHero0, dominantHero0);
                if (desiredShare6 < armyShare7) {
                    RedistributeTroops(
                        &dominantHero0->m_army,
                        &recipientHero0->m_army,
                        1,
                        1,
                        dominantFightValue4,
                        recipientFightValue0,
                        static_cast<i32>(
                            static_cast<float>(dominantFightValue4 + recipientFightValue0)
                            * transferFraction3
                        )
                    );
                } else {
                    RedistributeTroops(
                        &recipientHero0->m_army,
                        &dominantHero0->m_army,
                        1,
                        1,
                        recipientFightValue0,
                        dominantFightValue4,
                        static_cast<i32>(
                            static_cast<float>(dominantFightValue4 + recipientFightValue0)
                            * transferFraction3
                        )
                    );
                }
                SetupRelativeHeroStrengths();
            }
            *value = static_cast<i32>(interactionValue1 * AI_HERO_INTERACTION_VALUE_FACTOR);
        }
    }
}

void philAI::HeroInteractionAtTown(hero* heroPtr, town* townPtr, i32 doInteraction, i32* value) {
    i32 townFV;
    i32 moveCount;
    armyGroup* fromArmy;
    b32 townBetter;
    i32 winStrength;

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
    i32 dstStrength;
    i32 transferRating;
    b32 hasRoom;
    float transferFactor;
    float curveTerm;

    *value = 0;
    if (doInteraction != 0) {
        if ((townPtr->m_buildings & AI_BUILDING_SHIPYARD_MASK)
            && giBestShipyardId != townPtr->m_id) {
            stackSlot =
                abs(townPtr->m_x - heroPtr->m_x) + abs(townPtr->m_y - heroPtr->m_y);
            if (gbActualShipyardFound) {
                if (stackSlot < giBestShipyardDist) {
                    giBestShipyardDist = stackSlot;
                    giBestShipyardId = townPtr->m_id;
                }
            } else {
                giBestShipyardDist = stackSlot;
                giBestShipyardId = townPtr->m_id;
            }
            gbPossibleShipyardFound = true;
            gbActualShipyardFound = true;
        } else if ((townPtr->m_buildings & AI_BUILDING_CASTLE_MASK)
                   && giGroundToTerrain[gpAdvManager->GetCell(townPtr->m_x - 1, townPtr->m_y + 1)
                                            ->m_terrainImageIndex]
                          == TERRAIN_WATER
                   && !gbActualShipyardFound && giBestShipyardId != townPtr->m_id) {
            stackSlot =
                abs(townPtr->m_x - heroPtr->m_x) + abs(townPtr->m_y - heroPtr->m_y);
            if (gbPossibleShipyardFound) {
                if (stackSlot < giBestShipyardDist) {
                    giBestShipyardDist = stackSlot;
                    giBestShipyardId = townPtr->m_id;
                }
            } else {
                giBestShipyardDist = stackSlot;
                giBestShipyardId = townPtr->m_id;
            }
            gbPossibleShipyardFound = true;
        }
    } else {
        heroPtr->m_lastTownInteractionTurn = static_cast<i16>(giCurTurn);
        heroPtr->m_visitedTownId = static_cast<u8>(townPtr->m_id);
        if (!heroPtr->HasArtifact(ARTIFACT_MAGIC_BOOK)
            && (townPtr->m_buildings & AI_BUILDING_MAGE_GUILD_MASK)) {
            if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] >= AI_MAGIC_BOOK_COST) {
                GiveArtifact(heroPtr, ARTIFACT_MAGIC_BOOK, true, -1);
                gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= AI_MAGIC_BOOK_COST;
            } else {
                heroPtr->m_remainingMobility = 0;
            }
        }
        if ((townPtr->m_buildings & AI_BUILDING_MAGE_GUILD_MASK)
            && heroPtr->HasArtifact(ARTIFACT_MAGIC_BOOK)
            && heroPtr->m_spellPoints
                   < heroPtr->Stats(HERO_PRIMARY_KNOWLEDGE) * AI_MANA_PER_KNOWLEDGE) {
            heroPtr->m_remainingMobility = 0;
        }
    }

    if ((townPtr->m_buildings & AI_BUILDING_MAGE_GUILD_MASK)
        && (doInteraction != 0 || heroPtr->HasArtifact(ARTIFACT_MAGIC_BOOK))) {
        *value += ManaRefreshValue(heroPtr, 1);
        for (castLvl = 1;
             castLvl
             <= H2EnumIndex(heroPtr->m_secondarySkills[H2EnumIndex(HERO_SKILL_WISDOM)])
                    + WISDOM_SPELL_LEVEL_BONUS;
             castLvl++) {
            for (whichSpell = 0; whichSpell < townPtr->m_spellCounts[castLvl];
                 whichSpell++) {
                if (!heroPtr->HasSpell(townPtr->m_spells[castLvl - 1][whichSpell])) {
                    *value +=
                        gsSpellInfo[H2EnumIndex(townPtr->m_spells[castLvl - 1][whichSpell])].aiValue
                        * ((H2EnumIndex((gsSpellInfo[H2EnumIndex(townPtr->m_spells[castLvl - 1][whichSpell])]
                                   .attributes) & (SPELL_INFO_ATTRIBUTE_POWER)))
                               ? heroPtr->Stats(HERO_PRIMARY_KNOWLEDGE)
                               : 1);
                }
            }
        }
    }

    battlePower = FightValueOfStack(&heroPtr->m_army, NULL, 0, 0, 0, 0);
    townFV = FightValueOfStack(&townPtr->m_army, NULL, 0, 0, 0, 0);
    townShare = static_cast<double>(townFV) / (townFV + battlePower);
    statSum = 0;
    statSum =
        heroPtr->Stats(HERO_PRIMARY_ATTACK) + heroPtr->Stats(HERO_PRIMARY_DEFENSE);
    if (statSum > HERO_INTERACTION_COMBAT_STAT_MAX)
        statSum = HERO_INTERACTION_COMBAT_STAT_MAX;
    if (townPtr->m_buildings & AI_BUILDING_CASTLE_MASK)
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
    if (heroPtr->m_id == iAlphaMale)
        wantShare = static_cast<float>(wantShare * 0.5);
    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_CAPTURE_TOWN
        && gpGame->m_mapHeader.victoryConditionValue == townPtr->m_x
        && gpGame->m_mapHeader.victoryTownY == townPtr->m_y) {
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
                if (heroPtr->m_army.m_quantities[stackSlot] <= 0)
                    hasRoom = true;
            }
        } else {
            for (stackSlot = 0; stackSlot < AI_TOWN_ARMY_SLOTS; stackSlot++) {
                if (townPtr->m_army.m_quantities[stackSlot] <= 0)
                    hasRoom = true;
            }
        }
        if (!hasRoom) {
            for (stackSlot = 0; stackSlot < AI_TOWN_ARMY_SLOTS; stackSlot++) {
                for (otherIndex = 0; otherIndex < AI_TOWN_ARMY_SLOTS; otherIndex++) {
                    if (townPtr->m_army.m_creatureTypes[stackSlot]
                        == heroPtr->m_army.m_creatureTypes[otherIndex]) {
                        hasRoom = true;
                        break;
                    }
                }
            }
        }
        if (!hasRoom)
            transferRating = 0;
        *value += transferRating;
        if (townPtr->m_threat != 0 && townPtr->m_occupyingHeroId == -1)
            *value += AI_UNGUARDED_TOWN_VALUE;
    } else {
        townPtr->GiveSpells(NULL);
        if (townBetter)
            transferShare = static_cast<float>(transferShare + AI_TOWN_TRANSFER_BONUS);
        moveCount = static_cast<i32>((battlePower + townFV) * transferShare);
        fromArmy = (townBetter ? &townPtr->m_army : &heroPtr->m_army);
        secondArmy = (townBetter ? &heroPtr->m_army : &townPtr->m_army);
        if (townBetter) {
            winStrength = townFV;
            dstStrength = battlePower;
        } else {
            winStrength = battlePower;
            dstStrength = townFV;
        }
        RedistributeTroops(
            fromArmy,
            secondArmy,
            !townBetter,
            townBetter,
            winStrength,
            dstStrength,
            moveCount
        );
        if (giHumanTownConquered == townPtr->m_id
            && heroPtr->m_remainingMobility <= AI_CONQUERED_HERO_MOBILITY_LIMIT)
            heroPtr->m_remainingMobility = 0;
    }
}

void philAI::RedistributeTroops(
    armyGroup* sourceArmy,
    armyGroup* destinationArmy,
    i32 preserveOne,
    i32 preferFast,
    i32 sourceStrength,
    i32,
    i32 transferBudget
) {
    i32 howMany;
    i32 bestVal;
    i32 stackValue;
    i32 bestSpeed;
    i32 bestSlot;
    i32 dstI;
    i32 fromIdx;
    b32 again;
    i32 army = 0;

    again = true;
    gbTroopReload = false;
    while (again) {
        if (preserveOne != 0) {
            army = 0;
            for (fromIdx = 0; fromIdx < AI_TOWN_ARMY_SLOTS; fromIdx++) {
                if (sourceArmy->m_creatureTypes[fromIdx] != CREATURE_NONE)
                    army += sourceArmy->m_quantities[fromIdx];
            }
            if (army <= 1)
                return;
        }

        bestSlot = AI_TROOP_EMPTY_SLOT;
        for (fromIdx = 0; fromIdx < AI_TOWN_ARMY_SLOTS; fromIdx++) {
            if (bestSlot == AI_TROOP_EMPTY_SLOT) {
                for (dstI = 0; dstI < AI_TOWN_ARMY_SLOTS;
                     dstI++) {
                    if (sourceArmy->m_creatureTypes[fromIdx] != CREATURE_NONE
                        && sourceArmy->m_creatureTypes[fromIdx]
                               == destinationArmy->m_creatureTypes[dstI]) {
                        bestSlot = fromIdx;
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

            for (fromIdx = 0; fromIdx < AI_TOWN_ARMY_SLOTS; fromIdx++) {
                if (sourceArmy->m_creatureTypes[fromIdx] != CREATURE_NONE) {
                    stackValue =
                        sourceArmy->m_quantities[fromIdx]
                        * gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[fromIdx])].fightValue;
                    if ((preferFast != 0
                         && gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[fromIdx])].speed
                                > bestSpeed)
                        || (preferFast == 0
                            && gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[fromIdx])].speed
                                   < bestSpeed)) {
                        bestSpeed =
                            gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[fromIdx])].speed;
                        bestVal = stackValue;
                        bestSlot = fromIdx;
                    } else if (gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[fromIdx])].speed
                                   == bestSpeed
                               && stackValue > bestVal) {
                        bestVal = stackValue;
                        bestSlot = fromIdx;
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
                if (howMany > sourceArmy->m_quantities[bestSlot]) {
                    howMany = sourceArmy->m_quantities[bestSlot];
                } else {
                    again = false;
                    if ((howMany
                             >= sourceArmy->m_quantities[bestSlot]
                                    * AI_TROOP_REDISTRIBUTION_STACK_SHARE
                         || howMany >= sourceArmy->m_quantities[bestSlot] - 1)
                        && (sourceArmy->m_quantities[bestSlot] - howMany)
                                   * gMonsterDatabase[H2EnumIndex(sourceArmy->m_creatureTypes[bestSlot])]
                                         .fightValue
                               < (sourceStrength - transferBudget)
                                     * AI_TROOP_REDISTRIBUTION_REMAINDER_FACTOR) {
                        howMany = sourceArmy->m_quantities[bestSlot];
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
                sourceArmy->m_quantities[bestSlot] -= howMany;
                if (sourceArmy->m_quantities[bestSlot] == 0)
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
    armyGroup* ag1,
    hero* h1,
    armyGroup* ag2,
    hero* h2,
    i32 a,
    i32 b,
    i32 c,
    i32& outFlag,
    i32& outValue
) {
    i32 node, nb, kn;
    i32 val;
    i32 jb;
    float idx;

    ProbableOutcomeOfBattle(ag1, h1, ag2, h2, NULL, a, b, (h2 != NULL ? h2->m_owner : -1), idx, jb, kn, nb, node, val);
    val = static_cast<i32>(val + c * idx);
    if (val <= 0) {
        outValue = 0;
        outFlag = 0;
    } else {
        outValue = val;
        outFlag = 1;
    }
}

i32 philAI::ChooseToFightForArtifact(
    ArtifactType artifact, CreatureType monster, i32
) {
    i32 remainB;
    i32 remainA;
    i32 lostB;
    i32 lostA;
    i32 artValue;
    i32 idx;
    float chance;
    i32 score;
    i32 result;
    artValue = gArtifactBaseRV[H2EnumIndex(artifact)];
    for (idx = 0; idx < AI_TOWN_ARMY_SLOTS; idx++) {
        gpMonGroup->m_creatureTypes[idx] = monster;
        gpMonGroup->m_quantities[idx] = static_cast<i16>(idx == 0);
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

i32 philAI::NetValueOfArtifact(i32 a1, i32 a2, i32 a3, i32 a4) {
    return static_cast<i32>(
        static_cast<float>(gArtifactBaseRV[a1])
        - (static_cast<float>(a2) * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
           + static_cast<float>(a4) * gafAITurnCostResource[a3])
    );
}

i32 philAI::ChooseToPayRansomOnHero(i32) {
    return 1;
}

void philAI::BuildBuilding(town* t, BuildingSlotType building) {
    i32 i;
    i32 cost[AI_PURCHASE_RESOURCE_COUNT];
    sprintf(
        gText,
        "Player %d built %s in town %d.\n",
        giCurPlayer,
        GetBuildingName(t->m_type, building),
        t->m_id
    );
    LogStr(gText);
    if (giDebugLevel >= AI_PURCHASE_DEBUG_LEVEL) {
        AiPrint(gText);
        DelayMilli(AI_PURCHASE_DEBUG_DELAY);
    }
    GetBuildingCost(t->m_type, building, cost, t->m_buildState);
    for (i = 0; i < AI_PURCHASE_RESOURCE_COUNT; i++)
        gpCurPlayer->m_resources[i] -= cost[i];
    t->BuildBuilding(building);
    ShowStatus();
}

void philAI::BuildHero(town* townPtr, i32 availableHeroIndex) {
    hero* newHero;
    i32 townX;
    i32 townY;

    sprintf(
        gText,
        "Player %d built hero in town %d.\n",
        giCurPlayer,
        townPtr->m_id
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

    townX = townPtr->m_x;
    townY = townPtr->m_y;
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
    gpGame->m_availableHeroes[newHero->m_id] = townPtr->m_owner;

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
    townPtr->m_occupyingHeroId = newHero->m_id;
    townPtr->GiveSpells(NULL);

    gpCurPlayer->m_availableHeroIds[availableHeroIndex] =
        static_cast<i8>(gpGame->GetNewHeroId(giCurPlayer, FACTION_ANY, 1));
    gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[availableHeroIndex]] =
        AI_HERO_AVAILABLE_FLAG;
    bHeroBuiltThisTurn = true;
    HeroInteractionAtTown(newHero, townPtr, 0, &iDummy);
    ShowStatus();
}

void philAI::BuildCreature(town* townPtr, i32 dwelling, i32 purchaseCount) {
    float weakestValue;
    i32 monsterCosts[AI_PURCHASE_RESOURCE_COUNT];
    b32 hasRoom;
    i32 lowSlot;
    i32 slotIdx;
    float stackValue;
    CreatureType creature;

    sprintf(
        gText,
        "Player %d built %d %s in town %d.\n",
        giCurPlayer,
        purchaseCount,
        GetMonsterName(gDwellingType[H2EnumIndex(townPtr->m_type)][dwelling]),
        townPtr->m_id
    );
    LogStr(gText);
    if (giDebugLevel >= AI_PURCHASE_DEBUG_LEVEL) {
        AiPrint(gText);
        DelayMilli(AI_PURCHASE_DEBUG_DELAY);
    }

    creature = gDwellingType[H2EnumIndex(townPtr->m_type)][dwelling];
    hasRoom = false;
    for (slotIdx = 0; slotIdx < AI_TOWN_ARMY_SLOTS; slotIdx++) {
        if (townPtr->m_army.m_creatureTypes[slotIdx] == CREATURE_NONE
            || townPtr->m_army.m_creatureTypes[slotIdx] == creature) {
            hasRoom = true;
        }
    }

    weakestValue = AI_CREATURE_SELECTION_WORST_VALUE;
    lowSlot = AI_TROOP_EMPTY_SLOT;
    if (hasRoom == 0) {
        for (slotIdx = 0; slotIdx < AI_TOWN_ARMY_SLOTS; slotIdx++) {
            stackValue = static_cast<float>(
                gMonsterDatabase[H2EnumIndex(townPtr->m_army.m_creatureTypes[slotIdx])].fightValue
                * townPtr->m_army.m_quantities[slotIdx]
            );
            if (gMonsterDatabase[H2EnumIndex(townPtr->m_army.m_creatureTypes[slotIdx])].race
                != townPtr->m_type) {
                stackValue = static_cast<float>(stackValue * AI_CREATURE_OFF_RACE_FACTOR);
            }
            stackValue = static_cast<float>(
                stackValue
                * ((gMonsterDatabase[H2EnumIndex(townPtr->m_army.m_creatureTypes[slotIdx])].fightValue
                    + AI_CREATURE_SELECTION_BASE_VALUE)
                   / AI_CREATURE_SELECTION_BASE_VALUE)
            );
            if (stackValue < weakestValue) {
                weakestValue = stackValue;
                lowSlot = slotIdx;
            }
        }
        if (lowSlot == AI_TROOP_EMPTY_SLOT)
            lowSlot = 0;

        gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] +=
            townPtr->m_army.m_quantities[lowSlot]
            * gMonsterDatabase[H2EnumIndex(townPtr->m_army.m_creatureTypes[lowSlot])].cost;
        townPtr->m_army.m_creatureTypes[lowSlot] = CREATURE_NONE;
        townPtr->m_army.m_quantities[lowSlot] = 0;
    }

    GetMonsterCost(creature, monsterCosts);
    for (slotIdx = 0; slotIdx < AI_PURCHASE_RESOURCE_COUNT; slotIdx++) {
        gpCurPlayer->m_resources[slotIdx] -= purchaseCount * monsterCosts[slotIdx];
    }
    townPtr->m_garrison[dwelling] -= purchaseCount;
    townPtr->m_army.Add(creature, purchaseCount, AI_TROOP_EMPTY_SLOT);
    ShowStatus();
}

i32 philAI::CanBuyBHC(BHC& bhc) {
    i32 j;
    i32 index;
    i32 cost[AI_PURCHASE_RESOURCE_COUNT];
    switch (bhc.type) {
        case PURCHASE_BUILDING:
            if (CanBuy(bhc.pTown, bhc.building))
                return 1;
            break;
        case PURCHASE_HERO:
            if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] >= gHeroGoldCost
                && bhc.pTown->m_occupyingHeroId == -1
                && bHeroBuiltThisTurn == 0)
                return 1;
            break;
        case PURCHASE_CREATURE:
            j = H2EnumIndex(gDwellingType[H2EnumIndex(bhc.pTown->m_type)][bhc.what]);
            if (bhc.num > bhc.pTown->m_garrison[bhc.what])
                return 0;
            GetMonsterCost(CreatureType(j), cost);
            for (index = 0; index < AI_PURCHASE_RESOURCE_COUNT; index++)
                if (gpCurPlayer->m_resources[index] < cost[index] * bhc.num)
                    return 0;
            return 1;
    }
    return 0;
}

i32 philAI::CombatMonsterEvent(
    hero* h, CreatureType monType, i32* pCount, mapCell*
) {
    i32 kn;
    float idx;
    float f2;
    i32 jb;
    i32 total;
    memset(gpMonGroup->m_creatureTypes, -1, sizeof(gpMonGroup->m_creatureTypes));
    memset(gpMonGroup->m_quantities, 0, sizeof(gpMonGroup->m_quantities));
    if (*pCount / AI_TOWN_ARMY_SLOTS > 0) {
        for (kn = 0; kn < AI_TOWN_ARMY_SLOTS; kn++) {
            gpMonGroup->m_creatureTypes[kn] = monType;
            gpMonGroup->m_quantities[kn] = static_cast<i16>(*pCount / AI_TOWN_ARMY_SLOTS);
        }
    }
    for (kn = *pCount % AI_TOWN_ARMY_SLOTS - 1; kn >= 0; kn--) {
        gpMonGroup->m_creatureTypes[kn] = monType;
        gpMonGroup->m_quantities[kn]++;
    }
    jb = gpPhilAI->QuickCombat(&h->m_army, h, gpMonGroup, NULL, 0, 0, f2, idx);
    total = 0;
    for (kn = 0; kn < AI_TOWN_ARMY_SLOTS; kn++)
        total += gpMonGroup->m_quantities[kn];
    *pCount = total;
    if (jb != 0)
        return 1;
    return 0;
}

i32 philAI::FightEvent(hero* h, mapCell* cell, i32 evaluateOnly) {
    i32 combatResult3;
    i32 monsterCount3;
    float attackerLoss5;

    i32 stackIndex0;
    float defenderLoss8;
    CreatureType monsterType4;
    i32 battleWon9;
    MapObjectType eventType1;
    i32 battleValue1;
    i32 rewardValue0;

    eventType1 = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
    if (cell->m_objectMetadata == FIGHT_EVENT_EMPTY)
        return 0;

    i32 shipwreckCounts15[FIGHT_EVENT_LEVEL_COUNT] = {
        FIGHT_EVENT_SHIPWRECK_COUNT_1,
        FIGHT_EVENT_SHIPWRECK_COUNT_2,
        FIGHT_EVENT_SHIPWRECK_COUNT_3,
        FIGHT_EVENT_SHIPWRECK_COUNT_4
    };
    i32 derelictCounts0[FIGHT_EVENT_LEVEL_COUNT] = {
        FIGHT_EVENT_DERELICT_COUNT_1,
        FIGHT_EVENT_DERELICT_COUNT_2,
        FIGHT_EVENT_DERELICT_COUNT_3,
        FIGHT_EVENT_DERELICT_COUNT_4
    };
    i32 graveyardCounts0[FIGHT_EVENT_LEVEL_COUNT] = {
        FIGHT_EVENT_GRAVEYARD_COUNT_1,
        FIGHT_EVENT_GRAVEYARD_COUNT_2,
        FIGHT_EVENT_GRAVEYARD_COUNT_3,
        FIGHT_EVENT_GRAVEYARD_COUNT_4
    };

    switch (eventType1) {
        case MAP_OBJECT_SHIPWRECK:
            monsterType4 = CREATURE_GHOST;
            monsterCount3 =
                shipwreckCounts15[cell->m_objectMetadata - FIGHT_EVENT_LEVEL_OFFSET];
            break;
        case MAP_OBJECT_GRAVEYARD:
            monsterType4 = CREATURE_ZOMBIE;
            monsterCount3 =
                graveyardCounts0[cell->m_objectMetadata - FIGHT_EVENT_LEVEL_OFFSET];
            break;
        default:
            monsterType4 = CREATURE_SKELETON;
            monsterCount3 = derelictCounts0[cell->m_objectMetadata - FIGHT_EVENT_LEVEL_OFFSET];
            break;
    }

    for (stackIndex0 = 0; stackIndex0 < FIGHT_EVENT_STACKS; stackIndex0++) {
        gpMonGroup->m_creatureTypes[stackIndex0] = monsterType4;
        gpMonGroup->m_quantities[stackIndex0] = static_cast<i16>(monsterCount3);
    }

    rewardValue0 = 0;
    switch (eventType1) {
        case MAP_OBJECT_GRAVEYARD:
            switch (cell->m_objectMetadata) {
                case FIGHT_EVENT_LEVEL_1:
                    rewardValue0 = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_500_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_2:
                    rewardValue0 = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_1000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_3:
                    rewardValue0 = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_3000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_4:
                    rewardValue0 = static_cast<i32>(
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
                    rewardValue0 = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_1000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_2:
                    rewardValue0 = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_2000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_3:
                    rewardValue0 = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_5000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_4:
                    rewardValue0 = static_cast<i32>(
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
                    rewardValue0 = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_500_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_2:
                    rewardValue0 = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_1000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_3:
                    rewardValue0 = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_2000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
                case FIGHT_EVENT_LEVEL_4:
                    rewardValue0 = static_cast<i32>(
                        AI_FIGHT_EVENT_REWARD_5000_VALUE
                        * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
                    );
                    break;
            }
            break;
    }

    ChooseEvaluateBattle(
        &h->m_army,
        h,
        gpMonGroup,
        NULL,
        0,
        0,
        rewardValue0,
        battleWon9,
        battleValue1
    );
    if (evaluateOnly != 0)
        return battleValue1;
    if (battleWon9 != 0
        && (combatResult3 =
                QuickCombat(&h->m_army, h, gpMonGroup, NULL, 0, 0, attackerLoss5, defenderLoss8))
               != 0) {
        switch (eventType1) {
            case MAP_OBJECT_SHIPWRECK:
                switch (cell->m_objectMetadata) {
                    case FIGHT_EVENT_LEVEL_1:
                        gpAdvManager->GiveResource(h, RES_GOLD, FIGHT_EVENT_REWARD_1000);
                        break;
                    case FIGHT_EVENT_LEVEL_2:
                        gpAdvManager->GiveResource(h, RES_GOLD, FIGHT_EVENT_REWARD_2000);
                        break;
                    case FIGHT_EVENT_LEVEL_3:
                        gpAdvManager->GiveResource(h, RES_GOLD, FIGHT_EVENT_REWARD_5000);
                        break;
                    case FIGHT_EVENT_LEVEL_4:
                        gpAdvManager->GiveResource(h, RES_GOLD, FIGHT_EVENT_REWARD_2000);
                        gpAdvManager->GiveRandomArtifact(h);
                        break;
                }
                break;
            case MAP_OBJECT_GRAVEYARD:
                switch (cell->m_objectMetadata) {
                    case FIGHT_EVENT_LEVEL_1:
                        gpAdvManager->GiveResource(h, RES_GOLD, FIGHT_EVENT_REWARD_500);
                        break;
                    case FIGHT_EVENT_LEVEL_2:
                        gpAdvManager->GiveResource(h, RES_GOLD, FIGHT_EVENT_REWARD_1000);
                        break;
                    case FIGHT_EVENT_LEVEL_3:
                        gpAdvManager->GiveResource(h, RES_GOLD, FIGHT_EVENT_REWARD_3000);
                        break;
                    case FIGHT_EVENT_LEVEL_4:
                        gpAdvManager->GiveResource(h, RES_GOLD, FIGHT_EVENT_REWARD_1000);
                        gpAdvManager->GiveRandomArtifact(h);
                        break;
                }
                break;
            case MAP_OBJECT_DERELICT_SHIP:
                switch (cell->m_objectMetadata) {
                    case FIGHT_EVENT_LEVEL_1:
                        gpAdvManager->GiveResource(h, RES_GOLD, FIGHT_EVENT_REWARD_500);
                        break;
                    case FIGHT_EVENT_LEVEL_2:
                        gpAdvManager->GiveResource(h, RES_GOLD, FIGHT_EVENT_REWARD_1000);
                        break;
                    case FIGHT_EVENT_LEVEL_3:
                        gpAdvManager->GiveResource(h, RES_GOLD, FIGHT_EVENT_REWARD_2000);
                        break;
                    case FIGHT_EVENT_LEVEL_4:
                        gpAdvManager->GiveResource(h, RES_GOLD, FIGHT_EVENT_REWARD_5000);
                        break;
                }
                break;
        }
        cell->m_objectMetadata = FIGHT_EVENT_EMPTY;
    }
    return 0;
}

i32 philAI::DamageGroup(armyGroup* ag, hero* loser, hero*, float dmg) {
    if (dmg < AI_QUICK_COMBAT_DEFEAT_THRESHOLD) {
        ag->DamageGroup(dmg);
        return 0;
    } else {
        if (loser != NULL)
            gpAdvManager->HeroLoses(loser);
        else
            ag->DamageGroup(dmg);
        return 1;
    }
}

void philAI::IncrementHourGlass(void) {
    i32 nb = gpCurPlayer->m_heroCount;
    if (nb < HOURGLASS_PROSPECTIVE_HERO_LIMIT
        && gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] >= AI_HERO_PURCHASE_GOLD_FLOOR
        && bHeroBuiltThisTurn == 0)
        nb++;
    iCurHourGlassPhase++;
    if (nb == 1) {
        iCurHourGlassPhase++;
        iCurHourGlassPhase++;
    }
    if (nb == HOURGLASS_SECOND_HERO_COUNT && iCurHourGlassPhase != 1)
        iCurHourGlassPhase++;
    if (nb == HOURGLASS_THIRD_HERO_COUNT
        && (iCurHourGlassPhase == HOURGLASS_THIRD_HERO_SKIP_FIRST
            || iCurHourGlassPhase == HOURGLASS_THIRD_HERO_SKIP_SECOND))
        iCurHourGlassPhase++;
    if (iCurHourGlassPhase > HOURGLASS_LAST_PHASE)
        iCurHourGlassPhase = HOURGLASS_LAST_PHASE;
}

void philAI::TownEvent(mapCell* cell, hero* h, i32 x, i32 y) {
    town* pTown;

    float defenderLoss;
    float attackerLoss;
    hero* defenderHero;
    CombatResult combatResult;

    pTown = GetCastleSlot(cell->m_objectMetadata);

    gpAdvManager->DemobilizeCurrHero();

    if (pTown->m_owner != giCurPlayer) {
        if (pTown->HasGarrison()) {
            if (pTown->m_owner < 0 || gbHumanPlayer[pTown->m_owner] == 0) {
                QuickCombat(
                    &h->m_army,
                    h,
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
                    h,
                    &h->m_army,
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

    if (pTown->m_owner == giCurPlayer && h->m_x == x && h->m_y == y) {
        pTown->m_occupyingHeroId = gpCurPlayer->CurrentHero();
        h->m_locationType = (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE));
        h->m_occupiedTown = pTown->m_id;
        HeroInteractionAtTown(h, pTown, 0, &iDummy);
    }

    gpAdvManager->MobilizeCurrHero(0);
    pTown->GiveSpells(NULL);
}

i32 philAI::ComputeUpgradeValue(CreatureType a1, CreatureType a2) {
    i32 cnt = gpCurAIHero->CreatureTypeCount(a1);
    if (cnt == 0)
        return 0;
    i32 rv = static_cast<i32>(
        static_cast<float>(
            cnt * (gMonsterDatabase[H2EnumIndex(a2)].fightValue - gMonsterDatabase[H2EnumIndex(a1)].fightValue)
        )
        * gpCurPlayer->m_aiData.m_upgradeValueWeight
    );
    if (gpCurAIHero->CreatureTypeCount(a2) != 0)
        rv = static_cast<i32>(rv * AI_UPGRADE_EXISTING_STACK_FACTOR);
    return rv;
}

i32 philAI::ComputeValueOfSS(
    hero* h,
    HeroSecondarySkill skill,
    HeroSkillLevel level
) {
    i32 idx;
    i32 score;
    i32 armyValue;
    i32 archerValue;
    i32 totalValue;
    float ratio;
    i32 troopValue;

    score = gSSValues[H2EnumIndex(skill)][H2EnumIndex(level) - SECONDARY_SKILL_LEVEL_OFFSET];
    armyValue = FightValueOfStack(&h->m_army, h, 1, 0, 0, 0);
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
            if ((H2EnumIndex((h->m_eventFlags) & (HERO_EVENT_EMBARKED))))
                score = static_cast<i32>(score * AI_SECONDARY_SKILL_NAVIGATION_FACTOR);
            break;
        case HERO_SKILL_ARCHERY:
            archerValue = 0;
            totalValue = archerValue;
            for (idx = 0; idx < SECONDARY_SKILL_ARMY_SLOTS; idx++) {
                if (h->m_army.m_creatureTypes[idx] != CREATURE_NONE) {
                    troopValue = h->m_army.m_quantities[idx]
                        * gMonsterDatabase[H2EnumIndex(h->m_army.m_creatureTypes[idx])].fightValue;
                    totalValue += troopValue;
                    if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(h->m_army.m_creatureTypes[idx])].attributes) & (MONSTER_ATTRIBUTE_RANGED)))) {
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
            if (!h->HasArtifact(ARTIFACT_MAGIC_BOOK)
                || h->Stats(HERO_PRIMARY_KNOWLEDGE) < SECONDARY_SKILL_MINIMUM_KNOWLEDGE) {
                score = static_cast<i32>(score * AI_SECONDARY_SKILL_BASE_FACTOR);
            }
            break;
    }
    return score;
}

i32 philAI::ComputeValueOfFreeSS(
    hero* h, HeroSecondarySkill skill
) {
    if (h->m_secondarySkills[H2EnumIndex(skill)] != HERO_SKILL_LEVEL_NONE
        || h->m_secondarySkillCount >= HERO_SECONDARY_SKILL_CAPACITY)
        return 0;
    else
        return ComputeValueOfSS(h, skill, HERO_SKILL_LEVEL_BASIC);
}

i32 philAI::ManaRefreshValue(hero* h, i32 level) {
    i32 sp;
    i32 v;
    float frac;
    i32 deficit;

    v = 0;
    sp = h->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE * level;
    deficit = sp - h->m_spellPoints;
    if (deficit <= 0)
        return 0;

    frac = static_cast<float>(deficit) / static_cast<float>(sp);
    if (deficit > 0)
        v = static_cast<i32>(deficit * 5 * frac);
    return v;
}

i32 philAI::ValueOfEventAtPosition(i32 x, i32 y, i32 immediate, i32* liveChance) {
    mapCell* theCell;

    i32 armySlot;

    b32 freeFlag;

    H2EnumStorage<ResourceType, i32> res;
    i32 prize;

    i32 amount;

    i32 bBattleWon;

    i32 numToBuy;

    CreatureType buyCreature;

    i32 eventRV;

    i32 i;
    i32 gateX;
    mapCell* exitCell;
    i32 exitRV;
    i32 gateY;

    i32 costList[AI_PURCHASE_RESOURCE_COUNT];
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
                res = theCell->m_objectIndex >> 1;
                switch (res) {
                    case RES_GOLD:
                        eventRV = static_cast<i32>(
                            gafAITurnCostResource[H2EnumIndex(res)]
                            * (theCell->m_objectMetadata * AI_GOLD_RESOURCE_MULTIPLIER)
                        );
                        break;
                    default:
                        eventRV = static_cast<i32>(
                            gafAITurnCostResource[H2EnumIndex(res)] * theCell->m_objectMetadata
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
                               + WISDOM_SPELL_LEVEL_BONUS) {
                        eventRV = gsSpellInfo[theCell->m_objectMetadata - 1].aiValue;
                        if ((H2EnumIndex((gsSpellInfo[theCell->m_objectMetadata - 1].attributes) & (SPELL_INFO_ATTRIBUTE_POWER)))) {
                            eventRV = static_cast<i32>(
                                eventRV
                                * (gpCurAIHero->Stats(HERO_PRIMARY_KNOWLEDGE)
                                           <= AI_BATTLE_STAT_MAX
                                       ? gfStatPower[gpCurAIHero->Stats(HERO_PRIMARY_KNOWLEDGE)]
                                       : gfStatPower[AI_BATTLE_STAT_MAX])
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
                    res = (theCell->m_objectMetadata & AI_EVENT_RESOURCE_TYPE_MASK) - 1;
                    amount =
                        (theCell->m_objectMetadata & AI_ARTIFACT_EVENT_RESOURCE_MASK)
                        >> AI_ARTIFACT_EVENT_RESOURCE_SHIFT;
                    eventRV = static_cast<i32>(gafAITurnCostResource[H2EnumIndex(res)] * amount);
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
                        gArtifactBaseRV[theCell->m_objectMetadata - SKELETON_ARTIFACT_METADATA_OFFSET];
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
                                          <= AI_BATTLE_STAT_MAX
                                      ? gfBattleStat[gpCurAIHero->Stats(HERO_PRIMARY_SPELL_POWER)]
                                      : gfBattleStat[AI_BATTLE_STAT_MAX])
                               : 1.0f)
                        * gpCurPlayer->m_aiData.m_upgradeValueWeight
                    );
                    for (i = 0; i < AI_TOWN_ARMY_SLOTS; i++) {
                        gpMonGroup->m_creatureTypes[i] = CREATURE_ROYAL_MUMMY;
                        gpMonGroup->m_quantities[i] = 10;
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
                    gpMonGroup->m_quantities[i] = static_cast<i16>(
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
                            && abs(x - gateX) + abs(y - gateY)
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
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
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
    i32 value;
    i32 badArtifacts;
    GenericSiteType genericType;

    i32 slot;
    i32 count;
    CreatureType unitType;
    i32 armyWorth;
    i32 removedQuantity;

    badArtifacts = 0;
    genericType = static_cast<GenericSiteType>(cell->m_objectMetadata);
    genericType = static_cast<GenericSiteType>(H2EnumIndex(genericType) & H2EnumIndex(GENERIC_SITE_TYPE_MASK));
    value = 0;

    switch (genericType) {
        case GENERIC_SITE_ALCHEMIST_TOWER:
            for (slot = 0; slot < AI_BATTLE_ARTIFACT_SLOT_COUNT;
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
                for (slot = 0; slot < AI_GENERIC_SITE_ARMY_SLOTS;
                     slot++) {
                    unitType = gpCurAIHero->m_army.m_creatureTypes[slot];
                    if (unitType != CREATURE_NONE) {
                        count = gpCurAIHero->m_army.m_quantities[slot];
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
            value = ComputeUpgradeValue(AI_GENERIC_SITE_UPGRADE_FROM, AI_GENERIC_SITE_UPGRADE_TO);
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
    if (gpCurPlayer->m_barrierTents & (1 << color))
        return 5000;
    else
        return 0;
}

i32 philAI::EvaluatePassword(mapCell* cell) {
    i32 color = cell->m_tentColor;
    color &= EVENT_BARRIER_COLOR_MASK;
    if (!(gpCurPlayer->m_barrierTents & (1 << color)))
        return 2500;
    else
        return 0;
}

i32 philAI::EvaluateRecruitSite(mapCell* cell) {
    i32 idx;
    RecruitSiteType recruitmentSiteType;
    i32 nb;
    i32 val;
    i32 kn;

    i16 lvl;
    recruitmentSiteType = static_cast<RecruitSiteType>(cell->m_tentColor);
    recruitmentSiteType =
        static_cast<RecruitSiteType>(H2EnumIndex(recruitmentSiteType) & EVENT_RECRUIT_TYPE_MASK);
    lvl = cell->m_tentColor;
    lvl >>= EVENT_RECRUIT_COUNT_SHIFT;
    nb = 0;
    val = 0;
    switch (recruitmentSiteType) {
        case RECRUITMENT_SITE_BARROW_MOUNDS:
            nb = H2EnumIndex(CREATURE_GHOST);
            break;
        case RECRUITMENT_SITE_EARTH_ALTAR:
            nb = H2EnumIndex(CREATURE_EARTH_ELEMENTAL);
            break;
        case RECRUITMENT_SITE_AIR_ALTAR:
            nb = H2EnumIndex(CREATURE_AIR_ELEMENTAL);
            break;
        case RECRUITMENT_SITE_FIRE_ALTAR:
            nb = H2EnumIndex(CREATURE_FIRE_ELEMENTAL);
            break;
        case RECRUITMENT_SITE_WATER_ALTAR:
            nb = H2EnumIndex(CREATURE_WATER_ELEMENTAL);
            break;
    }
    EvaluateOneTimeCreaturePurchase(CreatureType(nb), lvl, 0, idx, val, kn);
    gbReduceByReload = false;
    return val;
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
    i32 idx;
    float chance;
    i32 result;

    i32 guardRV;
    i32 plainVal;

    i32 newOutcome;

    i32 lostA;
    i32 lostB;
    i32 remainA;
    i32 remainB;

    if (gpCurAIHero->NumArtifacts() == AI_BATTLE_ARTIFACT_SLOT_COUNT)
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
        for (idx = 0; idx < ARMY_GROUP_SLOT_COUNT; idx++) {
            gpMonGroup->m_creatureTypes[idx] =
                static_cast<CreatureType>(eventData & AI_ARTIFACT_EVENT_CREATURE_MASK);
            gpMonGroup->m_quantities[idx] = static_cast<i16>(
                gpMonGroup->m_creatureTypes[idx] == CREATURE_ROGUE
                    ? H2EnumIndex(AI_ARTIFACT_EVENT_GUARD_ROGUE_COUNT)
                    : (idx == 0)
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
        switch (static_cast<AIArtifactEventMode>(eventData & AI_ARTIFACT_EVENT_MODE_MASK)) {
            case AI_ARTIFACT_EVENT_VALUE:
                result = plainVal;
                break;
            case AI_ARTIFACT_EVENT_REQUIRES_WISDOM:
                if (gpCurAIHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_WISDOM)]
                    != HERO_SKILL_LEVEL_NONE)
                    result = plainVal;
                else
                    result = 0;
                break;
            case AI_ARTIFACT_EVENT_REQUIRES_LEADERSHIP:
                if (gpCurAIHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_LEADERSHIP)]
                    != HERO_SKILL_LEVEL_NONE)
                    result = plainVal;
                else
                    result = 0;
                break;
            case AI_ARTIFACT_EVENT_NO_VALUE:
                break;
            case AI_ARTIFACT_EVENT_PAY_GOLD:
                result = NetValueOfArtifact(H2EnumIndex(artifact), AI_ARTIFACT_EVENT_GOLD_COST, 0, 0);
                break;
            case AI_ARTIFACT_EVENT_PAY_RESOURCE_THREE:
                result = NetValueOfArtifact(
                    H2EnumIndex(artifact),
                    AI_ARTIFACT_EVENT_RESOURCE_THREE_COST,
                    (eventData & AI_ARTIFACT_EVENT_RESOURCE_MASK)
                        >> AI_ARTIFACT_EVENT_RESOURCE_SHIFT,
                    AI_ARTIFACT_EVENT_RESOURCE_THREE_AMOUNT
                );
                break;
            case AI_ARTIFACT_EVENT_PAY_RESOURCE_FIVE:
                result = NetValueOfArtifact(
                    H2EnumIndex(artifact),
                    AI_ARTIFACT_EVENT_RESOURCE_FIVE_COST,
                    (eventData & AI_ARTIFACT_EVENT_RESOURCE_MASK)
                        >> AI_ARTIFACT_EVENT_RESOURCE_SHIFT,
                    AI_ARTIFACT_EVENT_RESOURCE_FIVE_AMOUNT
                );
                break;
        }
    }
    return result;
}

i32 philAI::EvaluateMineEvent(i32 mineIndex, i32 x, i32 y, i32* liveChance) {
    i32 outcomeValue0;
    i32 guardianCount9;
    i32 result3 = 0;
    i32 attackerLoss2;
    i32 stackIndex0;
    float winChance5;
    i32 defenderLoss;
    i32 attackerRemaining;
    i32 defenderRemaining0;
    i32 mineValue0;

    if (gpGame->m_mineOwners[mineIndex] == gpCurAIHero->m_owner
        || OnMySide(gpGame->m_mineOwners[mineIndex]))
        return result3;

    if (gpGame->m_mines[mineIndex].guardianType != CREATURE_NONE) {
        guardianCount9 = gpGame->m_mines[mineIndex].guardianCount;
        memset(gpMonGroup->m_creatureTypes, ARMY_GROUP_EMPTY_SLOT, ARMY_GROUP_SLOT_COUNT);
        memset(
            gpMonGroup->m_quantities,
            0,
            ARMY_GROUP_SLOT_COUNT * sizeof(gpMonGroup->m_quantities[0])
        );

        if (guardianCount9 / ARMY_GROUP_SLOT_COUNT > 0) {
            for (stackIndex0 = 0; stackIndex0 < ARMY_GROUP_SLOT_COUNT; stackIndex0++) {
                gpMonGroup->m_creatureTypes[stackIndex0] =
                    gpGame->m_mines[mineIndex].guardianType;
                gpMonGroup->m_quantities[stackIndex0] =
                    static_cast<i16>(guardianCount9 / ARMY_GROUP_SLOT_COUNT);
            }
        }
        for (stackIndex0 = guardianCount9 % ARMY_GROUP_SLOT_COUNT - 1; stackIndex0 >= 0;
             stackIndex0--) {
            gpMonGroup->m_creatureTypes[stackIndex0] =
                gpGame->m_mines[mineIndex].guardianType;
            gpMonGroup->m_quantities[stackIndex0]++;
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
            winChance5,
            attackerLoss2,
            defenderLoss,
            attackerRemaining,
            defenderRemaining0,
            outcomeValue0
        );
        *liveChance = static_cast<i32>(winChance5);
        result3 = outcomeValue0;
    }

    if (gbIAmGreatest && gpGame->m_mineOwners[mineIndex] >= 0
        && !gbHumanPlayer[gpGame->m_mineOwners[mineIndex]])
        return result3;
    else {
        mineValue0 = static_cast<i32>(
            static_cast<float>(
                gMineCharacteristics[H2EnumIndex(gpGame->m_mines[mineIndex].resourceType)]
            )
            * gafAITurnCostResource[H2EnumIndex(gpGame->m_mines[mineIndex].resourceType)]
            * *(gaiTurnValueOfMine + x + y * MAP_WIDTH)
        );
        if (gpGame->m_mineOwners[mineIndex] >= 0) {
            mineValue0 = static_cast<i32>(mineValue0 * (gbHumanPlayer[gpGame->m_mineOwners[mineIndex]] ? gfAttackHumanBonus : gfAttackComputerBonus));
        }
        result3 += mineValue0;
    }
    return result3;
}

i32 philAI::EvaluateMonsterEvent(CreatureType monsterType, i32 eventData, i32* liveChance) {
    i32 defenderRemaining6;
    i32 attackerRemaining3;
    i32 unusedPurchaseValue7;
    i32 defenderLoss4;
    i32 attackerLoss6;
    i32 stackIndex29;
    i32 purchaseCost9;
    float winChance9;
    i32 willJoin15;
    float strengthRatio26;
    i32 result5;
    i32 monsterCount4;
    i32 outcomeValue0;

    monsterCount4 = eventData & H2EnumIndex(MAP_MONSTER_COUNT_MASK);
    willJoin15 = eventData & H2EnumIndex(MAP_MONSTER_GUARD_FLAG);
    result5 = 0;
    memset(gpMonGroup->m_creatureTypes, ARMY_GROUP_EMPTY_SLOT, ARMY_GROUP_SLOT_COUNT);
    memset(
        gpMonGroup->m_quantities,
        0,
        ARMY_GROUP_SLOT_COUNT * sizeof(gpMonGroup->m_quantities[0])
    );
    if (monsterCount4 / ARMY_GROUP_SLOT_COUNT > 0) {
        for (stackIndex29 = 0; stackIndex29 < ARMY_GROUP_SLOT_COUNT; stackIndex29++) {
            gpMonGroup->m_creatureTypes[stackIndex29] = monsterType;
            gpMonGroup->m_quantities[stackIndex29] =
                static_cast<i16>(monsterCount4 / ARMY_GROUP_SLOT_COUNT);
        }
    }
    for (stackIndex29 = monsterCount4 % ARMY_GROUP_SLOT_COUNT - 1; stackIndex29 >= 0;
         stackIndex29--) {
        gpMonGroup->m_creatureTypes[stackIndex29] = monsterType;
        gpMonGroup->m_quantities[stackIndex29]++;
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
        winChance9,
        attackerLoss6,
        defenderLoss4,
        attackerRemaining3,
        defenderRemaining6,
        outcomeValue0
    );
    EvaluateOneTimeCreaturePurchase(
        monsterType,
        monsterCount4,
        1,
        purchaseCost9,
        attackerLoss6,
        unusedPurchaseValue7
    );
    strengthRatio26 =
        static_cast<float>(
            gpPhilAI->FightValueOfStack(&gpCurAIHero->m_army, gpCurAIHero, 0, 0, 0, 0)
        )
        / static_cast<float>(monsterCount4 * gMonsterDatabase[H2EnumIndex(monsterType)].fightValue);

    if (willJoin15 && strengthRatio26 > AI_MONSTER_JOIN_RATIO
        && !gpCurAIHero->HasArtifact(ARTIFACT_HIDEOUS_MASK)
        && gpCurAIHero->m_army.CanJoin(monsterType) && monsterType != CREATURE_GHOST
        && monsterType != CREATURE_EARTH_ELEMENTAL && monsterType != CREATURE_AIR_ELEMENTAL
        && monsterType != CREATURE_FIRE_ELEMENTAL && monsterType != CREATURE_WATER_ELEMENTAL) {
        *liveChance = POSITION_FULL_CHANCE;
        *liveChance = static_cast<i32>(
            winChance9 * AI_MONSTER_JOIN_CHANCE_SCALE + AI_MONSTER_JOIN_CHANCE_BASE
        );
        if (gpCurAIHero->m_army.CanJoin(monsterType))
            result5 = attackerLoss6;
        else
            result5 = 0;
        result5 = static_cast<i32>(
            result5 * AI_MONSTER_JOIN_PURCHASE_WEIGHT
            + outcomeValue0 * AI_MONSTER_JOIN_OUTCOME_WEIGHT
        );
    } else if (strengthRatio26 > AI_MONSTER_OVERWHELMING_RATIO) {
        if (gpCurAIHero->GetSSLevel(HERO_SKILL_NECROMANCY) != 0)
            result5 = 120;
        else
            result5 = 0;
        result5 += monsterCount4 * gMonsterDatabase[H2EnumIndex(monsterType)].hitPoints;
    } else {
        *liveChance = static_cast<i32>(winChance9 * AI_MONSTER_FIGHT_CHANCE_SCALE);
        result5 = outcomeValue0;
    }

    if (result5 < 0)
        gbReduceByReload = false;
    return result5;
}

i32 philAI::EvaluateHeroEvent(i32 heroId, i32 x, i32 y, i32 mode, i32* liveChance) {

    float winChance;
    i32 result;

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
                           * (gbHumanPlayer[defTown->m_owner] ? gfAttackHumanBonus
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

        if (gbHumanPlayer[otherHero->m_owner] && result > EVENT_HUMAN_VALUE_THRESHOLD) {
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
                   * (gbHumanPlayer[pTownRec->m_owner] ? gfAttackHumanBonus
                                                       : gfAttackComputerBonus))
            );
        }
        eventVal = static_cast<i32>(townValue * fWin + result);
        if (gpGame->m_townOwners[townId] != TOWN_OWNER_NONE)
            gbReduceByBerserk = false;
    }

    if (pTownRec->m_owner != TOWN_OWNER_NONE && gbHumanPlayer[pTownRec->m_owner]
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
i8 giBuildShipyard[AI_PLAYER_COUNT];
i32 giMaxHeroesForThisPlayer;
i8 giBuildBoat[AI_PLAYER_COUNT];
float fReduceFactor;
u8 giCurPlayerBit;
i32 giBestShipyardDist;
b32 bHeroBuiltThisTurn;
i16 gaiHeroLiveChance[GAME_HERO_COUNT];
i32 giHumanTownConquered;
i32 giCurTurn;
i32 costTemp[AI_PURCHASE_RESOURCE_COUNT];
i32 iAlphaMale;
i32 iDummy;
b32 gbPossibleShipyardFound;
float gafAITurnCostResource[AI_PURCHASE_RESOURCE_COUNT];
i32 iCurPlaceToVisit;
i32 giBestShipyardId;
b32 gbActualBoatFound;
u8 giCurWatchPlayerBit;
playerData* gpCurPlayer;
float gfHeroInteractionBonus[GAME_HERO_COUNT];
b32 gbBerserk;
i32 giCurAIHeroMorale;
i8 giBuildBoatStuffTurn[AI_PLAYER_COUNT];
i32 iPlacesVisited[ADVMGR_PLACE_VISIT_COUNT][ADVMGR_PLACE_COORDINATE_COUNT];
b32 gbReduceByReload;
b32 gbTroopReload;
i32 giCurAIHeroLuck;
b32 gbActualShipyardFound;
