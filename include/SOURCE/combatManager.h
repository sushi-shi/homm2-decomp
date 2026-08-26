#ifndef HOMM2_SOURCE_COMBATMANAGER_H
#define HOMM2_SOURCE_COMBATMANAGER_H

#include <Ints.h>
#include <BASE/baseManager.h>
#include <BASE/WINMGR.h>
#include <BASE/icon.h>
#include <SOURCE/COMMAND.h>
#include "army.h"
#include "hero.h"
#include "hexcell.h"
#include "SPELLS.h"

class armyGroup;
class hero;
class heroWindow;
class icon;
class palette;
class town;
class widget;
struct SBolt {
    i32 startX;
    i32 startY;
    i32 endX;
    i32 endY;
    i32 branchDistance;
    i32 width;
    BoltColorMode colorMode;
    i32 unused1C;
    i32 angleDistance;
    float currentX;
    float currentY;
    i32 pixelX;
    i32 pixelY;
    i32 drawVertically;
    float baseAngle;
    float angle;
    i32 finished;
    i32 nearTarget;
    i32 nearestDistance;
    i32 widthFirst;
    i32 widthLast;
    i32 lastBranchX;
    i32 lastBranchY;
    i32 startWidth;
    i32 endWidth;
    i32 totalDistance;
    i32 minAngle;
    i32 maxAngle;
    float distanceRatio;
    i32 forceAngle;
};
struct tag_message;

enum class CombatCastleHex : i32 {
    COMBAT_CASTLE_HEX_NONE           = -1,
    COMBAT_CASTLE_HEX_TOP_TOWER      = 9,
    COMBAT_CASTLE_HEX_TOP_WALL       = 22,
    COMBAT_CASTLE_HEX_SECOND_TOWER   = 34,
    COMBAT_CASTLE_HEX_SECOND_WALL    = 47,
    COMBAT_CASTLE_HEX_GATE           = 59,
    COMBAT_CASTLE_HEX_THIRD_WALL     = 73,
    COMBAT_CASTLE_HEX_KEEP           = 77,
    COMBAT_CASTLE_HEX_THIRD_TOWER    = 86,
    COMBAT_CASTLE_HEX_MOAT           = 92,
    COMBAT_CASTLE_HEX_BOTTOM_WALL    = 100,
    COMBAT_CASTLE_HEX_BOTTOM_TOWER   = 113,
    COMBAT_CASTLE_SPECIAL_HEX_FIRST  = 114,
    COMBAT_CASTLE_SPECIAL_HEX_SECOND = 115
};
using enum CombatCastleHex;

enum class CombatDrawbridgeState : i32 {
    COMBAT_DRAWBRIDGE_LOWERED            = 0,
    COMBAT_DRAWBRIDGE_RAISE_FRAME_FIRST  = 1,
    COMBAT_DRAWBRIDGE_RAISE_FRAME_SECOND = 2,
    COMBAT_CASTLE_GATE_HIDDEN            = 3,
    COMBAT_DRAWBRIDGE_RAISED             = 4,
    COMBAT_CASTLE_GATE_OPEN              =
        COMBAT_DRAWBRIDGE_RAISED
};
using enum CombatDrawbridgeState;
ENABLE_ENUM_STEPS(CombatDrawbridgeState)

enum class CombatCastleWallSlot : i32 {
    COMBAT_WALL_SLOT_NONE           = -1,
    COMBAT_WALL_SLOT_TOP_TOWER      = 0,
    COMBAT_WALL_SLOT_SECOND_TOWER   = 1,
    COMBAT_WALL_SLOT_THIRD_TOWER    = 2,
    COMBAT_WALL_SLOT_BOTTOM_TOWER   = 3,
    COMBAT_WALL_SLOT_SECTION_FIRST  = 4,
    COMBAT_WALL_SLOT_SECTION_SECOND = 5,
    COMBAT_WALL_SLOT_SECTION_THIRD  = 6,
    COMBAT_WALL_SLOT_SECTION_FOURTH = 7,
    COMBAT_WALL_SLOT_KEEP           = 8
};
using enum CombatCastleWallSlot;

typedef enum CombatCastleWallConstant {
    COMBAT_WALL_SLOT_COUNT    = 9,
    COMBAT_WALL_SECTION_COUNT = 4
} CombatCastleWallConstant;

enum class CombatCastleWallState : u8 {
    COMBAT_WALL_STATE_KEEP_STANDING        = 0,
    COMBAT_WALL_STATE_KEEP_DESTROYED       = 1,
    COMBAT_WALL_STATE_TOWER_STANDING       = 1,
    COMBAT_WALL_STATE_DESTROYED            = 2,
    COMBAT_WALL_STATE_SECTION_DAMAGE_FIRST = 3,
    COMBAT_WALL_STATE_SECTION_DAMAGE_LAST  = 5,
    COMBAT_WALL_STATE_SECTION_DESTROYED    = 6
};
using enum CombatCastleWallState;
ENABLE_ENUM_STEPS(CombatCastleWallState)

typedef enum CombatCatapultConstant {
    COMBAT_CATAPULT_TARGET_ROLL_MIN           = 0,
    COMBAT_CATAPULT_TARGET_ROLL_MAX           = 20,
    COMBAT_CATAPULT_BALLISTICS_ROLL_MIN       = 0,
    COMBAT_CATAPULT_BALLISTICS_ROLL_MAX       = 100,
    COMBAT_CATAPULT_AI_ROLL_PENALTY           = 5,
    COMBAT_CATAPULT_NO_SKILL_DOUBLE_THRESHOLD = 25,
    COMBAT_CATAPULT_NO_SKILL_MISS_THRESHOLD   = 75,
    COMBAT_CATAPULT_ADVANCED_DOUBLE_THRESHOLD = 50,
    COMBAT_CATAPULT_MISS_HEX_COUNT            = 11,
    COMBAT_CATAPULT_DIRECTION_COUNT           = 6,
    COMBAT_CATAPULT_DIRECTION_ROLL_MIN        = 0,
    COMBAT_CATAPULT_DIRECTION_ROLL_MAX        = 5,
    COMBAT_CATAPULT_MISS_Y_OFFSET             = 17,
    COMBAT_CATAPULT_LOAD_FRAME_COUNT          = 5,
    COMBAT_CATAPULT_PROJECTILE_FRAME_COUNT    = 25,
    COMBAT_CATAPULT_DESCENT_FRAME             = 12,
    COMBAT_CATAPULT_EARLY_EXTENT_LAST_FRAME   = 6,
    COMBAT_CATAPULT_BOULDER_FRAME_COUNT       = 8,
    COMBAT_CATAPULT_ADVANCE_FRAME_COUNT       = 4,
    COMBAT_CATAPULT_CLOUD_FRAME_COUNT         = 12,
    COMBAT_CATAPULT_CLOUD_VISIBLE_FRAME_COUNT = 10,
    COMBAT_CATAPULT_GATE_CLOUD_FIRST_FRAME    = 2,
    COMBAT_CATAPULT_WALL_IMPACT_FRAME         = 6,
    COMBAT_CATAPULT_GATE_IMPACT_FRAME         = 7,
    COMBAT_CATAPULT_ANIMATION_DELAY           = 75,
    COMBAT_CATAPULT_PROJECTILE_DELAY          = 63,
    COMBAT_CATAPULT_TIMER_SLOT                = 1,
    COMBAT_CATAPULT_LAUNCH_SOUND              = 0,
    COMBAT_CATAPULT_IMPACT_SOUND              = 2,
    COMBAT_CATAPULT_INITIAL_MAX_X             = 160,
    COMBAT_CATAPULT_INITIAL_MIN_Y             = 263,
    COMBAT_CATAPULT_INITIAL_MAX_Y             = 413,
    COMBAT_CATAPULT_START_X                   = 84,
    COMBAT_CATAPULT_START_Y                   = 304,
    COMBAT_CATAPULT_MIDPOINT_DIVISOR          = 2,
    COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS  = 15,
    COMBAT_CATAPULT_MIN_VISIBLE_EXTENT_Y      = 1,
    COMBAT_CATAPULT_IMPACT_EXTENT_X           = 75,
    COMBAT_CATAPULT_IMPACT_EXTENT_TOP         = 50,
    COMBAT_CATAPULT_IMPACT_EXTENT_BOTTOM      = 150,
    COMBAT_CATAPULT_GATE_CLOUD_X_OFFSET       = 45,
    COMBAT_CATAPULT_GATE_CLOUD_Y_OFFSET       = 70,
    COMBAT_CATAPULT_CLOUD_Y_OFFSET            = 25,
    COMBAT_CATAPULT_KEEP_IMPACT_X             = 600,
    COMBAT_CATAPULT_KEEP_IMPACT_Y             = 160,
    COMBAT_KEEP_FACTION_COUNT                 = H2EnumIndex(FACTION_COUNT),
    COMBAT_KEEP_TOWER_COUNT                   = 3,
    COMBAT_KEEP_MISSILE_ANGLE_COUNT           = 9,
    COMBAT_KEEP_TOWER_DAMAGE_BONUS            = 2,
    COMBAT_KEEP_ATTACK_STAT_LIMIT             = 20,
    COMBAT_KEEP_ATTACK_STAT_INDEX_OFFSET      = 20,
    COMBAT_KEEP_RANDOM_DAMAGE_MIN             = 2,
    COMBAT_KEEP_RANDOM_DAMAGE_MAX             = 3,
    COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR       = 2,
    COMBAT_KEEP_MIN_DAMAGE                    = 1
} CombatCatapultConstant;

typedef enum CombatGridConstant {
    COMBAT_GRID_ROW_COUNT                   = 9,
    COMBAT_GRID_ROW_LENGTH                  = 13,
    COMBAT_GRID_FIRST_COLUMN                = 1,
    COMBAT_GRID_COLUMN_END                  = 12,
    COMBAT_GRID_REVERSE_FIRST_COLUMN        = 11,
    COMBAT_GRID_REVERSE_COLUMN_END          = 0,
    COMBAT_ARMY_SLOT_COUNT                  = 20,
    COMBAT_ARMY_STORAGE_SLOT_COUNT          = 21,
    COMBAT_RUNTIME_ALIGNMENT_SIZE           = 4,
    COMBAT_MESSAGE_STATE_PAD_SIZE           = 0x64,
    COMBAT_RUNTIME_DOUBLE_PAD_SIZE          = 0x8,
    COMBAT_SMALL_VIEW_PAD_SIZE              = 0x10,
    COMBAT_WIN_LOSE_WIDGET_COUNT            = 25,
    COMBAT_DIRECTION_MAP_COUNT              = 24,
    COMBAT_BACKGROUND_NAME_SIZE             = 13,
    COMBAT_DURATION_MOD_COUNT               = 11,
    COMBAT_CANCEL_DURATION_MOD_COUNT        = 11,
    COMBAT_TRANSFER_ARTIFACT_COUNT          = 16,
    COMBAT_CASTLE_REVERSE_ROW               = 5,
    COMBAT_CASTLE_GATE_ROW                  = 4,
    COMBAT_CASTLE_GATE_APPROACH_HEX         = 58,
    COMBAT_ARMY_SLOT_COUNT_DRAWING          = COMBAT_ARMY_SLOT_COUNT,
    COMBAT_HERO_LEFT_X                      = 30,
    COMBAT_HERO_LEFT_Y                      = 183,
    COMBAT_HERO_RIGHT_X                     = 610,
    COMBAT_HERO_RIGHT_Y                     = 148,
    COMBAT_HERO_RIGHT_ALT_X                 = 615,
    COMBAT_HERO_RIGHT_ALT_Y                 = 135,
    COMBAT_CATAPULT_X                       = 22,
    COMBAT_CATAPULT_Y                       = 390,
    COMBAT_CASTLE_TOP_LAYER_FRAME           = 0x19,
    COMBAT_CASTLE_GATE_FRAME                = 0x1a,
    COMBAT_CASTLE_GATE_CLOSED_FRAME         = 0x14,
    COMBAT_CASTLE_WALL_BASE_FRAME           = 0x11,
    COMBAT_SCREEN_WIDTH                     = 0x280,
    COMBAT_SCREEN_HEIGHT                    = 480,
    COMBAT_AREA_HEIGHT                      = 0x1bb,
    COMBAT_MAX_EXTENT_X                     = 0x27f,
    COMBAT_MAX_EXTENT_Y                     = 0x1ba,
    COMBAT_OBSTACLE_TYPE_COUNT              = 32,
    COMBAT_OBSTACLE_INCLUSIVE_ROLL_HIGH     = 32,
    COMBAT_OBSTACLE_CELL_ROLL_MAX           = 116,
    COMBAT_OBSTACLE_TRY_LIMIT               = 500,
    COMBAT_OBSTACLE_MIN_COLUMN_OFFSET       = 1,
    COMBAT_OBSTACLE_LEFT_COLUMN_LIMIT       = 3,
    COMBAT_OBSTACLE_RIGHT_COLUMN_FIRST      = 10,
    COMBAT_ELEVATION_OVERLAY_COUNT          = 25,
    COMBAT_ELEVATION_OVERLAY_CELL_COUNT     = 15,
    COMBAT_ELEVATION_OVERLAY_CHANCE         = 40,
    COMBAT_ELEVATION_OVERLAY_TRY_LIMIT      = 100,
    COMBAT_RANDOM_PERCENT_MAX               = 99,
    COMBAT_RANDOM_OBSTACLE_MIN              = 3,
    COMBAT_RANDOM_OBSTACLE_MAX              = 7,
    COMBAT_CASTLE_STRUCTURE_COUNT           = 4,
    COMBAT_DOOR_ANIMATION_DELAY             = 75,
    COMBAT_DOOR_EXTENT_MIN_X                = 304,
    COMBAT_DOOR_EXTENT_MIN_Y                = 218,
    COMBAT_DOOR_EXTENT_MAX_X                = 384,
    COMBAT_DOOR_EXTENT_MAX_Y                = 294,
    COMBAT_WALL_TRACE_SUBDIVISIONS          = 10,
    COMBAT_MISSILE_DIRECTION_COUNT          = 9,
    COMBAT_MISSILE_LAST_DIRECTION           = 8,
    COMBAT_MISSILE_HALF_WIDTH               = 25,
    COMBAT_MISSILE_HALF_HEIGHT              = 25,
    COMBAT_MISSILE_SPACING                  = 31,
    COMBAT_MISSILE_SPACING_ROUND            = 15,
    COMBAT_MISSILE_TIMER_DELAY              = 25,
    COMBAT_SMALL_VIEW_FULL_INFO             = 2,
    COMBAT_SMALL_VIEW_LEFT_X                = 5,
    COMBAT_SMALL_VIEW_RIGHT_X               = 555,
    COMBAT_SMALL_VIEW_FULL_Y                = 299,
    COMBAT_SMALL_VIEW_FULL_RIGHT_Y          = 154,
    COMBAT_SMALL_VIEW_COMPACT_Y             = 366,
    COMBAT_SMALL_VIEW_COMPACT_RIGHT_Y       = 288,
    COMBAT_SMALL_VIEW_WIDTH                 = 83,
    COMBAT_SMALL_VIEW_FULL_HEIGHT           = 141,
    COMBAT_SMALL_VIEW_COMPACT_HEIGHT        = 74,
    COMBAT_SMALL_VIEW_ICON_SIZE             = 19,
    COMBAT_SMALL_VIEW_MAX_SPELLS            = 6,
    COMBAT_SMALL_VIEW_UNUSED_POSITION       = 255,
    COMBAT_SMALL_VIEW_SPELL_X_FIRST         = 1,
    COMBAT_SMALL_VIEW_SPELL_X_SECOND        = 11,
    COMBAT_SMALL_VIEW_SPELL_X_THIRD         = 22,
    COMBAT_SMALL_VIEW_SPELL_X_FOURTH        = 32,
    COMBAT_SMALL_VIEW_SPELL_X_FIFTH         = 43,
    COMBAT_SMALL_VIEW_SPELL_Y_FIRST         = 0,
    COMBAT_SMALL_VIEW_SPELL_Y_SECOND        = 10,
    COMBAT_SMALL_VIEW_SPELL_Y_THIRD         = 21,
    COMBAT_SMALL_VIEW_INSET_X               = 6,
    COMBAT_SMALL_VIEW_TEXT_X                = 8,
    COMBAT_SMALL_VIEW_TEXT_WIDTH            = 57,
    COMBAT_SMALL_VIEW_TEXT_HEIGHT           = 12,
    COMBAT_SMALL_VIEW_QUANTITY_Y            = 7,
    COMBAT_SMALL_VIEW_FIRST_STAT_Y          = 20,
    COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT       = 9,
    COMBAT_SMALL_VIEW_SHOTS_Y               = 74,
    COMBAT_SMALL_VIEW_FULL_SPELL_Y          = 86,
    COMBAT_SMALL_VIEW_COMPACT_SPELL_Y       = 19,
    COMBAT_SMALL_VIEW_MODIFIER_STEP         = 10,
    COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X      = 57,
    COMBAT_SMALL_VIEW_NEUTRAL_MORALE_X      = 45,
    COMBAT_SMALL_VIEW_NEUTRAL_LUCK_X        = 50,
    COMBAT_MESSAGE_LINE_SIZE                = 120,
    COMBAT_MESSAGE_WRAP_BUFFER_SIZE         = 400,
    COMBAT_MESSAGE_LOG_BUFFER_SIZE          = 700,
    COMBAT_MESSAGE_TIMEOUT                  = 2500,
    COMBAT_MESSAGE_LINE_WIDTH               = 474,
    COMBAT_MESSAGE_WINDOW_X                 = 83,
    COMBAT_MESSAGE_WINDOW_Y                 = 446,
    COMBAT_MESSAGE_WINDOW_HEIGHT            = 33,
    COMBAT_MESSAGE_WIDGET_FIRST             = 12,
    COMBAT_MESSAGE_WIDGET_SECOND            = 13,
    COMBAT_MESSAGE_DRAW_FIRST_WIDGET        = 10,
    COMBAT_BALLISTA_HEX                     = 77,
    COMBAT_HEX_COUNT                        = 117,
    COMBAT_GRID_COPY_LEFT                   = 67,
    COMBAT_GRID_COPY_TOP                    = 63,
    COMBAT_GRID_COPY_RIGHT                  = 573,
    COMBAT_GRID_COPY_BOTTOM                 = 442,
    COMBAT_GRID_LINE_FRAME                  = 0,
    COMBAT_GRID_LINE_COLOR                  = 0xe2,
    COMBAT_GRID_MOUSE_FRAME                 = 2,
    COMBAT_MOUSE_HEX_WIDTH                  = 44,
    COMBAT_MOUSE_HEX_HEIGHT                 = 52,
    COMBAT_MOUSE_HEX_MAX_X_OFFSET           = 43,
    COMBAT_MOUSE_HEX_MAX_Y_OFFSET           = 51,
    COMBAT_MOUSE_REDRAW_DELAY               = 75,
    COMBAT_BACKGROUND_COPY_WIDTH            = 507,
    COMBAT_BACKGROUND_COPY_HEIGHT           = 380,
    COMBAT_PALETTE_DATA_SIZE                = 0x300,
    COMBAT_HEX_HORIZONTAL_STEP              = 44,
    COMBAT_HEX_VERTICAL_STEP                = 42,
    COMBAT_HEX_ROW_STAGGER                  = 22,
    COMBAT_HEX_CENTER_X_ORIGIN              = 89,
    COMBAT_HEX_CENTER_Y_ORIGIN              = 63,
    COMBAT_HEX_GRID_LEFT_ORIGIN             = 67,
    COMBAT_HEX_GRID_TOP_ORIGIN              = 63,
    COMBAT_GRID_INDEX_X_ORIGIN              = 23,
    COMBAT_GRID_DIAGONAL_HEIGHT             = 10,
    COMBAT_GRID_DIAGONAL_SLOPE_DIVISOR      = 2,
    COMBAT_GRID_RIGHT_HERO_HEX              = 25,
    COMBAT_GRID_LEFT_SPECIAL_X_MAX          = 74,
    COMBAT_GRID_LEFT_SPECIAL_Y_MIN          = 80,
    COMBAT_GRID_LEFT_SPECIAL_Y_MAX          = 196,
    COMBAT_GRID_LEFT_SPECIAL_HEX            = 26,
    COMBAT_GRID_RIGHT_SPECIAL_X_MIN         = 566,
    COMBAT_GRID_RIGHT_UPPER_Y_MIN           = 37,
    COMBAT_GRID_RIGHT_UPPER_Y_MAX           = 153,
    COMBAT_GRID_RIGHT_UPPER_HEX             = 25,
    COMBAT_GRID_RIGHT_LOWER_Y_MIN           = 154,
    COMBAT_GRID_RIGHT_LOWER_Y_MAX           = 310,
    COMBAT_VALID_HEX_MAX                    = 125,
    COMBAT_FIXED_ICON_COUNT                 = 15,
    COMBAT_OBSTACLE_ICON_LOAD_COUNT         = 8,
    COMBAT_ARMY_CAPACITY                    = 20,
    COMBAT_SPEED_LEVEL_COUNT                = 15,
    COMBAT_MAX_SPEED                        = 14,
    COMBAT_HERO_EXPERIENCE_VALUE            = 500,
    COMBAT_GROUPED_HEX_STEP                 = 13,
    COMBAT_SPREAD_HEX_STEP                  = 26,
    COMBAT_ATTACKER_GROUPED_HEX             = 27,
    COMBAT_ATTACKER_SPREAD_HEX              = 1,
    COMBAT_DEFENDER_GROUPED_HEX             = 37,
    COMBAT_DEFENDER_SPREAD_HEX              = 11,
    COMBAT_CASTLE_BACKGROUND_BASE_FRAME     = 1,
    COMBAT_CASTLE_BACKGROUND_BUILDING_FRAME = 4,
    COMBAT_CASTLE_BACKGROUND_DEFAULT_FRAME  = 3
} CombatGridConstant;

enum class BattlefieldFringeFrame : i32 {
    FRINGE_NONE        = -1,
    FRINGE_BEACH       = 2,
    FRINGE_WASTELAND   = 3,
    FRINGE_DESERT      = 4,
    FRINGE_LAVA        = 5,
    FRINGE_SNOW_TREES  = 6,
    FRINGE_SNOW        = 7,
    FRINGE_SWAMP       = 8,
    FRINGE_DIRT        = 9,
    FRINGE_DIRT_TREES  = 10,
    FRINGE_GRASS       = 11,
    FRINGE_GRASS_TREES = 12,
    FRINGE_WATER       = 13
};
using enum BattlefieldFringeFrame;

enum class CombatTowerSelector : i32 {
    COMBAT_TOWER_GARRISON = 0,
    COMBAT_TOWER_TOP      = 1,
    COMBAT_TOWER_BOTTOM   = 2
};
using enum CombatTowerSelector;

enum class CombatRippleMode : i32 {
    COMBAT_RIPPLE_WAVE         = 0,
    COMBAT_RIPPLE_DEATH_RIPPLE = 1,
    COMBAT_RIPPLE_DEATH_WAVE   = 2
};
using enum CombatRippleMode;

enum class CombatIconIndex : i32 {
    COMBAT_ICON_STATUS                = 1,
    COMBAT_ICON_CATAPULT              = 3,
    COMBAT_ICON_TOWER                 = 5,
    COMBAT_ICON_KEEP                  = 7,
    COMBAT_ICON_SPELLS                = 8,
    COMBAT_ICON_GRID                  = 9,
    COMBAT_ICON_SMALL_VIEW_BACKGROUND = 10,
    COMBAT_ICON_SMALL_VIEW_MODIFIER   = 11,
    COMBAT_ICON_SMALL_VIEW_SPELL      = 12,
    COMBAT_ICON_MOAT                  = 13,
    COMBAT_ICON_DRAWBRIDGE            = 14
};
using enum CombatIconIndex;

typedef enum CombatRuntimeConstant {
    COMBAT_RANDOM_X_MULTIPLIER            = 100,
    COMBAT_CAPTAIN_SPELL_POINT_MULTIPLIER = 10,
    COMBAT_CAPTAIN_SPRITE_OFFSET          = H2EnumIndex(FACTION_COUNT),
    COMBAT_NEUTRAL_HERO_COLOR             = H2EnumIndex(FACTION_COUNT),
    COMBAT_POINTER_DEFAULT                = 6,
    COMBAT_HERO_OVERLAY_FRAME_COUNT       = 5,
    COMBAT_HERO_ANIMATION_TRACK_COUNT     = COMBAT_SIDE_COUNT + 1,
    COMBAT_INITIAL_COMMAND                = 15,
    COMBAT_INVALID_HISTORY_INDEX          = -99
} CombatRuntimeConstant;

typedef enum CombatAIConstant {
    COMBAT_AI_ARMY_SLOT_COUNT          = 20,
    COMBAT_AI_GROUP_SLOT_COUNT         = 5,
    COMBAT_AI_GROUP_SCAN_DONE          = 999,
    COMBAT_AI_LAST_HERO_COUNT          = 1,
    COMBAT_AI_MAX_DIFFICULTY           = 4,
    COMBAT_AI_FIGHT_VALUE_MODE         = 1,
    COMBAT_AI_STRENGTH_ROUNDING        = 4,
    COMBAT_AI_STRENGTH_FRACTION        = 5,
    COMBAT_AI_ATTACK_NONE              = 0,
    COMBAT_AI_ATTACK_SHOOT             = 1,
    COMBAT_AI_ATTACK_FLY               = 2,
    COMBAT_AI_ATTACK_WALK              = 3,
    COMBAT_AI_MASK_FIRST_BIT           = 1,
    COMBAT_AI_NO_ARMY                  = -1,
    COMBAT_AI_LICH_DAMAGE_PER_CREATURE = 9,
    COMBAT_AI_LICH_HIT_POINT_BONUS     = 100,
    COMBAT_AI_ATTACK_DIRECTION_COUNT   = 8,
    COMBAT_AI_ALL_ATTACK_DIRECTIONS    = 0xFF,
    COMBAT_AI_UNLIMITED_PATH_SPEED     = 0x7f,
    COMBAT_AI_DISTANCE_WEIGHT          = 1000,
    COMBAT_AI_WORST_STRENGTH_LIMIT     = 999999999,
    COMBAT_AI_CLOSEST_ARMY_LIMIT       = 9999999,
    COMBAT_AI_ADJACENT_DIRECTION_COUNT = 6,
    COMBAT_AI_CASTLE_BOUNDARY_COUNT    = 12,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_0    = 8,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_1    = 0x15,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_2    = 0x21,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_3    = 0x2e,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_4    = 0x3a,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_5    = 0x48,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_6    = 0x55,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_7    = 99,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_8    = 0x70,
    COMBAT_AI_TOWER_STRENGTH           = 300,
    COMBAT_AI_MIN_ARTIFACT_VALUE       = 1000,
    COMBAT_AI_MIN_HERO_EXPERIENCE      = 2000,
    COMBAT_AI_MEDIUM_ARTIFACT_VALUE    = 5000,
    COMBAT_AI_HIGH_ARTIFACT_VALUE      = 10000,
    COMBAT_AI_STRENGTH_2500            = 2500,
    COMBAT_AI_STRENGTH_5000            = 5000,
    COMBAT_AI_STRENGTH_15000           = 15000,
    COMBAT_AI_STRENGTH_20000           = 20000,
    COMBAT_AI_STRENGTH_30000           = 30000,
    COMBAT_AI_STRENGTH_40000           = 40000,
    COMBAT_AI_EXPERIENCE_DIVISOR       = 200000
} CombatAIConstant;

typedef enum CombatSpellAIConstant {
    COMBAT_SPELL_AI_MIRROR_POWER_ONE                 = 1,
    COMBAT_SPELL_AI_MIRROR_POWER_TWO                 = 2,
    COMBAT_SPELL_AI_HASTE_SPEED_BONUS                = 2,
    COMBAT_SPELL_AI_MINIMUM_DISTANCE                 = 2,
    COMBAT_SPELL_AI_CASTLE_DISTANCE_BONUS            = 3,
    COMBAT_SPELL_AI_RIGHT_DISTANCE_COLUMN            = 10,
    COMBAT_SPELL_AI_ALL_ATTACK_DIRECTIONS            = 0xFF,
    COMBAT_SPELL_AI_CURE_POINTS_PER_POWER            = 5,
    COMBAT_SPELL_AI_RESURRECT_POINTS_PER_POWER       = 50,
    COMBAT_SPELL_AI_FIRE_DAMAGE_PER_POWER            = 10,
    COMBAT_SPELL_AI_LIGHTNING_DAMAGE_PER_POWER       = 25,
    COMBAT_SPELL_AI_CHAIN_LIGHTNING_DAMAGE_PER_POWER = 40,
    COMBAT_SPELL_AI_HOLY_WORD_DAMAGE_PER_POWER       = 10,
    COMBAT_SPELL_AI_HOLY_SHOUT_DAMAGE_PER_POWER      = 20,
    COMBAT_SPELL_AI_MAGIC_ARROW_DAMAGE_PER_POWER     = 10,
    COMBAT_SPELL_AI_ARMAGEDDON_DAMAGE_PER_POWER      = 50,
    COMBAT_SPELL_AI_ELEMENTAL_STORM_DAMAGE_PER_POWER = 25,
    COMBAT_SPELL_AI_COLD_RAY_DAMAGE_PER_POWER        = 20,
    COMBAT_SPELL_AI_COLD_RING_DAMAGE_PER_POWER       = 10,
    COMBAT_SPELL_AI_DEATH_RIPPLE_DAMAGE_PER_POWER    = 5,
    COMBAT_SPELL_AI_DEATH_WAVE_DAMAGE_PER_POWER      = 10,
    COMBAT_SPELL_AI_DECISIVE_EFFECT                  = 100000000,
    COMBAT_SPELL_AI_MIRROR_LETHAL_DAMAGE             = 999999
} CombatSpellAIConstant;

enum class CombatMessageCommand : i32 {
    COMBAT_INVALID_COMMAND                    = -99,
    COMBAT_MESSAGE_COMMAND_DEFAULT            = 0,
    COMBAT_MESSAGE_COMMAND_MOVE               = 1,
    COMBAT_MESSAGE_COMMAND_FLY                = 2,
    COMBAT_MESSAGE_COMMAND_SHOOT              = 3,
    COMBAT_MESSAGE_COMMAND_OPTIONS            = 4,
    COMBAT_MESSAGE_COMMAND_VIEW_INFO          = 5,
    COMBAT_MESSAGE_COMMAND_ATTACK             = 7,
    COMBAT_MESSAGE_COMMAND_CAST_SPELL         = 10,
    COMBAT_MESSAGE_COMMAND_RETREAT            = 11,
    COMBAT_MESSAGE_COMMAND_SURRENDER          = 12,
    COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS   = 13,
    COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL = 15
};
using enum CombatMessageCommand;

enum class CombatGridShade : u8 {
    GRID_SHADE_NONE          = 0,
    GRID_SHADE_REACHABLE     = 1,
    GRID_SHADE_EMPTY_BLOCKED = 3
};
using enum CombatGridShade;

#pragma pack(push, 1)
class combatManager : public baseManager {
public:
    class palette* m_combatPalette;
    i8 m_savedPalette[COMBAT_PALETTE_DATA_SIZE];
    char m_previousCombatMessage[COMBAT_MESSAGE_LINE_SIZE];
    char m_currentCombatMessage[COMBAT_MESSAGE_LINE_SIZE];
    H2EnumStorage<CombatGridShade, u8> m_previousGridState[COMBAT_HEX_COUNT];
    H2EnumStorage<CombatGridShade, u8> m_gridState[COMBAT_HEX_COUNT];
    hexcell m_hexCells[COMBAT_HEX_COUNT];
    H2EnumStorage<TerrainType, i32> m_terrainType;
    BattlefieldFringeFrame m_battlefieldFringe;
    class town* m_originalCombatTown;
    WindowColorCycleMode m_colorCycleType;
    char _pad_0x31ee[COMBAT_RUNTIME_ALIGNMENT_SIZE];
    i32 m_debugFormation;
    class icon* m_combatIcons[COMBAT_FIXED_ICON_COUNT];
    class icon* m_obstacleIcons[COMBAT_OBSTACLE_ICON_LOAD_COUNT];
    i32 m_obstacleCount;
    H2EnumStorage<SpellType, i16> m_eagleEyeSpell[COMBAT_SIDE_COUNT];
    CombatDrawbridgeState m_drawbridgeState;
    i32 m_drawbridgeBackgroundVisible;
    H2SteppedEnumStorage<CombatCastleWallState, u8>
    m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_COUNT)];
    class bitmap* m_combatBuffer;
    class bitmap* m_backgroundBuffer;
    class bitmap* m_mouseGridBuffer;
    i32 m_backgroundDrawn;
    class mapCell* m_battlefieldCell;
    class town* m_combatTowns[COMBAT_SIDE_COUNT];
    class hero* m_heroes[COMBAT_SIDE_COUNT];
    class hero m_captain;
    i32 m_spellPower[COMBAT_SIDE_COUNT];
    class armyGroup* m_armyGroups[COMBAT_SIDE_COUNT];
    i32 m_mouseGridHex;
    u8 m_heroDeathPending[COMBAT_SIDE_COUNT];
    u8 m_heroAlternateDeathPending[COMBAT_SIDE_COUNT];
    u8 m_heroDeathAnimationPlayed[COMBAT_SIDE_COUNT];
    u8 m_heroAlternateDeathAnimationPlayed[COMBAT_SIDE_COUNT];
    i32 m_heroAnimationState[COMBAT_SIDE_COUNT];
    i32 m_heroAnimationFrame[COMBAT_SIDE_COUNT];
    i32 m_heroSpriteIndex[COMBAT_SIDE_COUNT];
    i32l m_heroCycleTimer[COMBAT_SIDE_COUNT];
    class icon* m_heroIcons[COMBAT_SIDE_COUNT];
    class icon* m_heroOverlayIcons[COMBAT_SIDE_COUNT];
    i32 m_heroOverlayFrame[COMBAT_SIDE_COUNT];
    struct SLimitData m_heroLimits[COMBAT_SIDE_COUNT];
    struct SLimitData m_heroOverlayLimits[COMBAT_SIDE_COUNT];
    struct SLimitData m_moatLimits[H2EnumIndex(COMBAT_WALL_SLOT_COUNT)];
    i32l m_previousCombatMessageExpiration;
    i32l m_combatMessageExpiration;
    i32 m_combatMessagePending;
    char _pad_0x34b9[COMBAT_MESSAGE_STATE_PAD_SIZE];
    H2EnumStorage<CreatureType, u8> m_summonedCreatureType[COMBAT_SIDE_COUNT];
    i32 m_sideDefeated[COMBAT_SIDE_COUNT];
    i32 m_networkArmyPresent[COMBAT_SIDE_COUNT];
    i32 m_playerId[COMBAT_SIDE_COUNT];
    i32 m_experienceValue[COMBAT_SIDE_COUNT];
    i32 m_heroCastSpell[COMBAT_SIDE_COUNT];
    i32 m_armyCount[COMBAT_SIDE_COUNT];
    class army m_armies[COMBAT_SIDE_COUNT][COMBAT_ARMY_STORAGE_SLOT_COUNT];
    H2EnumStorage<CombatSide, i32> m_currentArmySide;
    i32 m_currentArmyIndex;
    i32 m_currentSpeed;
    H2EnumStorage<CombatSide, i32> m_currentSide;
    i32 m_gridSelectionDisabled;
    i32 m_limitCreature;
    i32 m_limitCreatureHex;
    i32 m_showArmyQuantities;
    i32 m_selectedHex;
    i32 m_directionTargetHex;
    CombatMessageCommand m_previousCommand;
    CombatMessageCommand m_currentCommand;
    struct SLimitData m_catapultLimits;
    struct SLimitData m_gateLimits;
    struct SLimitData m_upperWallLimits;
    struct SLimitData m_middleWallLimits;
    i32 m_catapultFrame[COMBAT_SIDE_COUNT];
    i32 m_catapultAttackCount[COMBAT_SIDE_COUNT];
    i32 m_catapultAttacksRemaining[COMBAT_SIDE_COUNT];
    i32 m_keepAttacksRemaining[COMBAT_SIDE_COUNT];
    b32 m_inCastleCombat;
    i32 m_unknownF337[COMBAT_SIDE_COUNT];
    i32 m_visitingHeroPresent[COMBAT_SIDE_COUNT];
    char _pad_0xf347[COMBAT_RUNTIME_ALIGNMENT_SIZE];
    i32 m_unknownF34B;
    i32 m_unknownF34F;
    i32 m_unknownF353;
    i32 m_nonVisualCombat;
    i32 m_unknownF35B;
    i32 m_killBenefit[COMBAT_SIDE_COUNT];
    class heroWindow* m_combatWindow;
    char _pad_0xf36b[COMBAT_RUNTIME_DOUBLE_PAD_SIZE];
    i32 m_unknownF373;
    i32 m_sideRetreated[COMBAT_SIDE_COUNT];
    i32 m_limitCreatureCount[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
    i32 m_drawHero[COMBAT_SIDE_COUNT];
    i32 m_drawHeroOverlay[COMBAT_SIDE_COUNT];
    i32 m_combatWindowOpen;
    class widget* m_winLoseBottomWidgets[COMBAT_WIN_LOSE_WIDGET_COUNT];
    class widget* m_winLoseBottomTextWidgets[COMBAT_WIN_LOSE_WIDGET_COUNT];
    i32 m_combatX;
    i32 m_combatY;
    i8 m_directionMap[COMBAT_DIRECTION_MAP_COUNT];
    i32 m_mouseDirection;
    i32 m_validDirectionCount;
    struct SLimitData m_smallViewLimits;
    char _pad_0xf533[COMBAT_SMALL_VIEW_PAD_SIZE];
    H2EnumStorage<CombatSide, i32> m_smallViewSide[COMBAT_SIDE_COUNT];
    i32 m_smallViewArmyIndex[COMBAT_SIDE_COUNT];
    i32 m_smallViewLastX[COMBAT_SIDE_COUNT];
    i32 m_smallViewLastY[COMBAT_SIDE_COUNT];
    i32 m_smallViewWidth[COMBAT_SIDE_COUNT];
    i32 m_smallViewHeight[COMBAT_SIDE_COUNT];
    char _pad_0xf573[COMBAT_RUNTIME_ALIGNMENT_SIZE];
    u8 m_removedArmies[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
    u8 m_removedArmyPresent;
    char m_battlefieldBackgroundName[COMBAT_BACKGROUND_NAME_SIZE];
    i8 m_adjacency[COMBAT_HEX_COUNT][COMBAT_AI_ADJACENT_DIRECTION_COUNT];
    class heroWindow* m_winLoseWindow;
    H2EnumStorage<SpellType, i32> m_selectedSpell;
    H2EnumStorage<CombatResult, i32> m_combatResult;
    combatManager(void);
    virtual i32 Open(i32) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message&) override;
    void NoShowCombatLog(const char*);
    void ClearCombatMessages(i32);
    void CheckUpdateCombatMessages(void);
    void CombatMessage(const char*, i32, i32, i32);
    void CombatMessage(CombatMessageCommand);
    void ResetLimitCreature(void);
    void UpdateCombatArea(void);
    void SetupGridForArmy(class army*);
    i32 UpdateGrid(i32, i32);
    void DrawBackground(void);
    void UpdateMouseGrid(i32, i32);
    void DrawFrame(i32, i32, i32, i32, i32, i32, i32);
    void DrawSmallView(i32, i32);
    i32 ViewGeneral(CombatSide, i32, i32);
    void ViewArmy(class army*, i32);
    i32 HasValidSpellTarget(SpellType);
    i32 ViewSpells(i32);
    i32 FindResurrectArmyIndex(
        CombatSide, SpellType, i32
    );
    i32 ValidSpellTarget(SpellType, i32);
    void SpellMessage(SpellType, i32);
    void CastSpell(SpellType, i32, i32, i32);
    void DefaultSpell(i32);
    void Fireball(i32, SpellType);
    void MeteorShower(i32);
    void ElementalStorm(void);
    void Armageddon(void);
    void TurnToStone(class army*);
    void BloodLustEffect(class army*, MonsterFlags);
    void Ripple(i32);
    void Blur(i32, i32, i32);
    void ResetBoltAngle(struct SBolt*);
    void DrawBolt(struct SBolt*, i32);
    void AddBolt(
        struct SBolt*,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        BoltColorMode,
        i32,
        i32,
        i32,
        i32
    );
    void DoBolt(
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        BoltColorMode,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32
    );
    i32 GetNextChainLightningTarget(class army*, i32);
    void ChainLightning(i32, i32);
    void VaporizeCreature(CombatSide, i32);
    void RippleCreature(CombatSide, i32, CombatRippleMode);
    void ShowMassSpell(
        i8 (*const)[COMBAT_ARMY_SLOT_COUNT],
        CombatEffectType,
        i32
    );
    void CastMassSpell(SpellType, i32);
    void MirrorImage(i32);
    void SummonElemental(CreatureType, i32);
    void DoLuck(CombatSide, i32);
    void DoBlast(i32, SpellType);
    void Resurrect(SpellType, i32, i32);
    i32 SpaceForElementalExists(void);
    void ShowSpellCastFailure(class army*, i32);
    void ModifyDamageForArtifacts(
        i32l*,
        SpellType,
        class hero*,
        class hero*
    );
    void Earthquake(void);
    void ShowSpellMessage(i32, SpellType, class army*);
    i32 ValidHexToStandOn(i32);
    void SetCombatDirections(i32);
    void CheckSetMouseDirection(i32, i32, i32);
    i32 GetPointer(CombatMessageCommand, i32);
    MessageDispatchResult ProcessCombatMsg(struct tag_message&);
    i32 IsNegationSphereInEffect(void);
    void ResetRound(void);
    i32 CheckWin(struct tag_message*);
    CombatMessageCommand GetCommand(i32);
    i32 RightClick(i32);
    void DoCommand(CombatMessageCommand);
    void ClearWinLoseBottom(class heroWindow*);
    void ShowWinLoseArtifact(class heroWindow*, ArtifactType);
    void ShowSkeletons(class heroWindow*);
    void ShowEagleEyeSpell(class heroWindow*);
    void ShowDeadArmies(class heroWindow*);
    void DoVictory(CombatResult);
    void DoLoseWindow(void);
    i32 DoSurrender(void);
    void CheckChangeSelector(void);
    void CheckCastleAttack(void);
    void CheckGetAIMove(void);
    void GetControl(void);
    void ResetMouse(void);
    MessageDispatchResult ProcessNextAction(struct tag_message&);
    void ResetCyclingCreatures(void);
    void ResetCycleTimers(void);
    void CycleCombatScreen(void);
    void SetCombatViewArmySmallLevel(i32);
    void SetCombatGrid(i32, i32, i32);
    void AddArmy(
        CombatSide,
        CreatureType,
        i32,
        i32,
        MonsterFlags,
        i32
    );
    void SetupSmallView(void);
    void ViewBallista(i32);
    i32 DoSpellAI(CombatSide, i32);
    void DetermineEffectOfSpell(SpellType, i32*, i32*);
    i32 EffectSpellCreateCreature(i32, SpellType);
    i32 RawEffectSpellInfluence(class army*, ArmySpellInfluence);
    void ClearEffects(void);
    void NextPos(i32*);
    i32 FirstArmy(i32, i32, i32*);
    i32 FirstResurrectable(i32, i32*, SpellType);
    void EffectSpellCure(i32*, i32, i32, i32);
    void EffectSpellResurrect(i32*, i32, SpellType);
    void EffectSpellDamage(i32*, SpellType, i32);
    void CombineGroups(class armyGroup*, class armyGroup*);
    void SetupCombat(
        i32,
        i32,
        class hero*,
        class armyGroup*,
        class town*,
        class hero*,
        class armyGroup*,
        i32,
        i32,
        i32
    );
    void InitNonVisualVars(void);
    void SetupAdjacencyArray(void);
    void UpdateArmyGroup(CombatSide);
    void GenerateMap(void);
    const char* GetBackgroundName(void);
    i32 MoreTreesNear(void);
    void LoadIcons(void);
    void FreeIcons(void);
    void LoadArmies(void);
    void FreeArmies(void);
    i32 GetGridIndex(i32, i32);
    void CheckApplyGoodMorale(CombatSide, i32);
    i32 CheckApplyBadMorale(CombatSide, i32);
    i32 GetNextArmy(i32);
    i32 IsWinner(CombatSide);
    void CatAttack(CombatSide);
    void KeepAttack(CombatTowerSelector);
    i32 ExperienceValueOfStack(CombatSide);
    void ResetHitByCreature(void);
    void SaveCombatBorder(void);
    void DrawCombatBorder(void);
    void SetupAndLoadObstacles(void);
    void MakeCreaturesVanish(void);
    void LowerDoor(void);
    void RaiseDoor(void);
    void TestRaiseDoor(void);
    i32 InCastle(i32);
    i32 ShotIsThroughWall(CombatSide, i32, i32);
    void ShootMissile(i32, i32, i32, i32, float*, class icon*);
    void CombatSystemOptions(void);
    i32 AICheckRetreat(void);
    void DoCompAI(CombatSide);
    float GetModLichDamage(class army*, float);
    void DoLichShot(class army*);
    i32 GetShooterMask(CombatSide);
    i32 GetMirrorImageMask(CombatSide);
    i32 GetFlyerMask(CombatSide);
    i32 GetAllMask(CombatSide);
    i32 GetWalkerMask(CombatSide);
    i32 GetOutOfItMask(CombatSide);
    i32 GetTraitorMask(CombatSide);
    i32 GetBestArmy(CombatSide, i32);
    i32 GetWorstArmy(CombatSide, i32);
    i32 GetClosestArmy(class army*, CombatSide, i32);
    u32l GetStrength(CombatSide, i32);
    i32 AttemptAttack(class army*, CombatSide, i32);
    i32 AttemptAdjacentAttack(class army*);
    i32 WalkTowardArmyFront(class army*, CombatSide, i32);
    i32 WalkTowardArmy(class army*, CombatSide, i32);
};
#pragma pack(pop)
extern i32 bGridWasShowing;
extern b32 gbInDrawSmallView;
extern H2EnumStorage<CombatSide, i32> iViewGeneralWhichSide;
extern i32 castX;
extern i32 castY;
extern i32 bInTeleportGetDest;
extern i32 indexToCastOn;
extern u8 uRedBeam[];
extern u8 uRainbow[];
extern i8* gyModify;
extern float gfDurationMods[COMBAT_DURATION_MOD_COUNT];
extern float gfCancelDurationMods[COMBAT_CANCEL_DURATION_MOD_COUNT];
extern i32 bInHighMoraleBonus;
extern i32 giSeed;
extern u8 wallHex[COMBAT_WALL_SECTION_COUNT];
extern b32 gbThisNetHasControl;
extern i32 iCurTransferArtifact;
extern i8 iTransferArtifactsInfo[COMBAT_TRANSFER_ARTIFACT_COUNT];
extern b32 gbWhichAnimationPlaying;
extern i32 iMaxTransferArtifacts;
extern i32 giNextActionExtra;
extern i32 bSkeletonsShown;
extern b32 gbShowingLoseWindow;
extern i32 giWinCmbtFrame;
extern i32 giNextActionGridIndex;
extern i32 giSurrenderCost;
extern i32 giSkeletonsCreated;
extern H2EnumStorage<ArtifactType, i8> iTransferArtifacts[COMBAT_TRANSFER_ARTIFACT_COUNT];
extern H2EnumStorage<CombatAction, i32> giNextAction;
extern i32 giNextActionGridIndex2;
extern i32 giCurrSpellGroup;
extern i32 bMouseWasVis;
extern class heroWindow* CSPanel;
extern i32 bCPrefsChanged;

#endif
