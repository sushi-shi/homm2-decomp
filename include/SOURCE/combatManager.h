#ifndef HOMM2_SOURCE_COMBATMANAGER_H
#define HOMM2_SOURCE_COMBATMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 149 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
#include <BASE/WINMGR.h>
#include "../BASE/icon.h"
#include "army.h"
#include "hero.h"
#include "hexcell.h"
// forward declarations:
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
    i32 colorMode;
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
SIZE(SBolt, 0x78);
struct tag_message;

H2_ENUM_CLASS_BEGIN(CombatDrawLayer)
    COMBAT_DRAW_LAYER_COUNT = 9,
    COMBAT_DRAW_RIGHT_HERO_LAYER = 1,
    COMBAT_DRAW_LEFT_HERO_LAYER = 2,
    COMBAT_DRAW_WALL_TOP_LAYER = 4,
    COMBAT_DRAW_WALL_MIDDLE_LAYER = 5,
    COMBAT_DRAW_GATE_LAYER = 6,
    COMBAT_DRAW_CATAPULT_LAYER = 7
H2_ENUM_CLASS_END(CombatDrawLayer)

H2_ENUM_CLASS_BEGIN(CombatCastleHex)
    COMBAT_CASTLE_HEX_NONE = -1,
    COMBAT_CASTLE_HEX_TOP_TOWER = 9,
    COMBAT_CASTLE_HEX_TOP_WALL = 22,
    COMBAT_CASTLE_HEX_SECOND_TOWER = 34,
    COMBAT_CASTLE_HEX_SECOND_WALL = 47,
    COMBAT_CASTLE_HEX_GATE = 59,
    COMBAT_CASTLE_HEX_THIRD_WALL = 73,
    COMBAT_CASTLE_HEX_KEEP = 77,
    COMBAT_CASTLE_HEX_THIRD_TOWER = 86,
    COMBAT_CASTLE_HEX_MOAT = 92,
    COMBAT_CASTLE_HEX_BOTTOM_WALL = 100,
    COMBAT_CASTLE_HEX_BOTTOM_TOWER = 113,
    COMBAT_CASTLE_SPECIAL_HEX_FIRST = 114,
    COMBAT_CASTLE_SPECIAL_HEX_SECOND = 115
H2_ENUM_CLASS_END(CombatCastleHex)

H2_ENUM_CLASS_BEGIN(CombatDrawbridgeState)
    COMBAT_DRAWBRIDGE_LOWERED = 0,
    COMBAT_DRAWBRIDGE_RAISE_FRAME_FIRST = 1,
    COMBAT_DRAWBRIDGE_RAISE_FRAME_SECOND = 2,
    COMBAT_CASTLE_GATE_HIDDEN = 3, // gate sprite suppressed once the bridge clears this frame
    COMBAT_DRAWBRIDGE_RAISED = 4,
    COMBAT_CASTLE_GATE_OPEN = COMBAT_DRAWBRIDGE_RAISED // alias: raised bridge reads as open gate in town view
H2_ENUM_CLASS_END(CombatDrawbridgeState)

H2_ENUM_BEGIN(CombatCastleInteriorRange)
    COMBAT_CASTLE_INTERIOR_ROW_0_FIRST = 0,
    COMBAT_CASTLE_INTERIOR_ROW_0_LAST = 8,
    COMBAT_CASTLE_INTERIOR_ROW_1_FIRST = 13,
    COMBAT_CASTLE_INTERIOR_ROW_1_LAST = 21,
    COMBAT_CASTLE_INTERIOR_ROW_2_FIRST = 26,
    COMBAT_CASTLE_INTERIOR_ROW_2_LAST = 33,
    COMBAT_CASTLE_INTERIOR_ROW_3_FIRST = 39,
    COMBAT_CASTLE_INTERIOR_ROW_3_LAST = 46,
    COMBAT_CASTLE_INTERIOR_ROW_4_FIRST = 52,
    COMBAT_CASTLE_INTERIOR_ROW_4_LAST = 58,
    COMBAT_CASTLE_INTERIOR_ROW_5_FIRST = 65,
    COMBAT_CASTLE_INTERIOR_ROW_5_LAST = 72,
    COMBAT_CASTLE_INTERIOR_ROW_6_FIRST = 78,
    COMBAT_CASTLE_INTERIOR_ROW_6_LAST = 85,
    COMBAT_CASTLE_INTERIOR_ROW_7_FIRST = 91,
    COMBAT_CASTLE_INTERIOR_ROW_7_LAST = 99,
    COMBAT_CASTLE_INTERIOR_ROW_8_FIRST = 104,
    COMBAT_CASTLE_INTERIOR_ROW_8_LAST = 112
H2_ENUM_END(CombatCastleInteriorRange)

H2_ENUM_BEGIN(CombatCastleWallSlot)
    COMBAT_WALL_SLOT_NONE = -1,
    COMBAT_WALL_SLOT_TOP_TOWER = 0,
    COMBAT_WALL_SLOT_BOTTOM_TOWER = 3,
    COMBAT_WALL_SLOT_SECTION_FIRST = 4,
    COMBAT_WALL_SLOT_SECTION_SECOND = 5,
    COMBAT_WALL_SLOT_SECTION_THIRD = 6,
    COMBAT_WALL_SLOT_SECTION_FOURTH = 7,
    COMBAT_WALL_SLOT_KEEP = 8,
    COMBAT_WALL_SECTION_COUNT = 4
H2_ENUM_END(CombatCastleWallSlot)

H2_ENUM_CLASS_BEGIN(CombatCastleWallState)
    COMBAT_WALL_STATE_KEEP_STANDING = 0,
    COMBAT_WALL_STATE_KEEP_DESTROYED = 1,
    COMBAT_WALL_STATE_TOWER_STANDING = 1,
    COMBAT_WALL_STATE_DESTROYED = 2,
    COMBAT_WALL_STATE_SECTION_DAMAGE_FIRST = 3,
    COMBAT_WALL_STATE_SECTION_DAMAGE_LAST = 5,
    COMBAT_WALL_STATE_SECTION_DESTROYED = 6
H2_ENUM_CLASS_END(CombatCastleWallState)

H2_ENUM_CLASS_BEGIN(CombatTowerSelector)
    COMBAT_TOWER_SELECTOR_GARRISON = 0,
    COMBAT_TOWER_SELECTOR_TOP = 1,
    COMBAT_TOWER_SELECTOR_BOTTOM = 2
H2_ENUM_CLASS_END(CombatTowerSelector)

H2_ENUM_CLASS_BEGIN(CombatCoordinateAxis)
    COMBAT_COORDINATE_X = 0,
    COMBAT_COORDINATE_Y = 1
H2_ENUM_CLASS_END(CombatCoordinateAxis)

H2_ENUM_CLASS_BEGIN(CombatCatapultDamage)
    COMBAT_CATAPULT_DAMAGE_NONE = 0,
    COMBAT_CATAPULT_DAMAGE_NORMAL = 1,
    COMBAT_CATAPULT_DAMAGE_DOUBLE = 2
H2_ENUM_CLASS_END(CombatCatapultDamage)

H2_ENUM_CLASS_BEGIN(CombatKeepTargetPriority)
    COMBAT_KEEP_PRIORITY_NONE = -1,
    COMBAT_KEEP_PRIORITY_DISABLED = 0,
    COMBAT_KEEP_PRIORITY_WALKER = 1,
    COMBAT_KEEP_PRIORITY_FLYER = 2,
    COMBAT_KEEP_PRIORITY_SHOOTER = 3
H2_ENUM_CLASS_END(CombatKeepTargetPriority)

#define COMBAT_CATAPULT_TARGET_ROLL_MIN 0
#define COMBAT_CATAPULT_TARGET_ROLL_MAX 20
#define COMBAT_CATAPULT_BALLISTICS_ROLL_MIN 0
#define COMBAT_CATAPULT_BALLISTICS_ROLL_MAX 100
#define COMBAT_CATAPULT_AI_ROLL_PENALTY 5
#define COMBAT_CATAPULT_NO_SKILL_DOUBLE_THRESHOLD 25
#define COMBAT_CATAPULT_NO_SKILL_MISS_THRESHOLD 75
#define COMBAT_CATAPULT_ADVANCED_DOUBLE_THRESHOLD 50
#define COMBAT_CATAPULT_MISS_HEX_COUNT 11
#define COMBAT_CATAPULT_DIRECTION_COUNT 6
#define COMBAT_CATAPULT_DIRECTION_ROLL_MIN 0
#define COMBAT_CATAPULT_DIRECTION_ROLL_MAX 5
#define COMBAT_CATAPULT_MISS_Y_OFFSET 17
#define COMBAT_CATAPULT_LOAD_FRAME_COUNT 5
#define COMBAT_CATAPULT_PROJECTILE_FRAME_COUNT 25
#define COMBAT_CATAPULT_DESCENT_FRAME 12
#define COMBAT_CATAPULT_EARLY_EXTENT_LAST_FRAME 6
#define COMBAT_CATAPULT_BOULDER_FRAME_COUNT 8
#define COMBAT_CATAPULT_ADVANCE_FRAME_COUNT 4
#define COMBAT_CATAPULT_CLOUD_FRAME_COUNT 12
#define COMBAT_CATAPULT_CLOUD_VISIBLE_FRAME_COUNT 10
#define COMBAT_CATAPULT_GATE_CLOUD_FIRST_FRAME 2
#define COMBAT_CATAPULT_WALL_IMPACT_FRAME 6
#define COMBAT_CATAPULT_GATE_IMPACT_FRAME 7
#define COMBAT_CATAPULT_ANIMATION_DELAY 75
#define COMBAT_CATAPULT_PROJECTILE_DELAY 63
#define COMBAT_CATAPULT_TIMER_SLOT 1
#define COMBAT_CATAPULT_LAUNCH_SOUND 0
#define COMBAT_CATAPULT_IMPACT_SOUND 2
#define COMBAT_CATAPULT_INITIAL_MAX_X 160
#define COMBAT_CATAPULT_INITIAL_MIN_Y 263
#define COMBAT_CATAPULT_INITIAL_MAX_Y 413
#define COMBAT_CATAPULT_START_X 84
#define COMBAT_CATAPULT_START_Y 304
#define COMBAT_CATAPULT_MIDPOINT_DIVISOR 2
#define COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS 15
#define COMBAT_CATAPULT_MIN_VISIBLE_EXTENT_Y 1
#define COMBAT_CATAPULT_IMPACT_EXTENT_X 75
#define COMBAT_CATAPULT_IMPACT_EXTENT_TOP 50
#define COMBAT_CATAPULT_IMPACT_EXTENT_BOTTOM 150
#define COMBAT_CATAPULT_GATE_CLOUD_X_OFFSET 45
#define COMBAT_CATAPULT_GATE_CLOUD_Y_OFFSET 70
#define COMBAT_CATAPULT_CLOUD_Y_OFFSET 25
#define COMBAT_CATAPULT_KEEP_IMPACT_X 600
#define COMBAT_CATAPULT_KEEP_IMPACT_Y 160
#define COMBAT_CATAPULT_HORIZONTAL_STEP_DIVISOR 12.5
#define COMBAT_CATAPULT_VERTICAL_STEP_DIVISOR 78.0f
#define COMBAT_CATAPULT_DISTANCE_ARC_SCALE 0.3
#define COMBAT_CATAPULT_TARGET_ARC_SCALE 0.35
#define COMBAT_KEEP_FACTION_COUNT 6
#define COMBAT_KEEP_TOWER_COUNT 3
#define COMBAT_KEEP_MISSILE_ANGLE_COUNT 9
#define COMBAT_KEEP_TOWER_DAMAGE_BONUS 2
#define COMBAT_KEEP_ATTACK_STAT_LIMIT 20
#define COMBAT_KEEP_ATTACK_STAT_INDEX_OFFSET 20
#define COMBAT_KEEP_RANDOM_DAMAGE_MIN 2
#define COMBAT_KEEP_RANDOM_DAMAGE_MAX 3
#define COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR 2
#define COMBAT_KEEP_MIN_DAMAGE 1
#define COMBAT_MISSILE_DEGREES_PER_RADIAN 180.0
#define COMBAT_MISSILE_PI 3.14159
#define COMBAT_MISSILE_DIRECTION_AVERAGE_DIVISOR 2.0f
#define COMBAT_CREATURE_VANISH_DURATION 150.0f

H2_ENUM_CLASS_BEGIN(CombatSmallViewText)
    COMBAT_SMALL_VIEW_TEXT_QUANTITY_PLURAL = 0,
    COMBAT_SMALL_VIEW_TEXT_QUANTITY_SINGULAR = 1,
    COMBAT_SMALL_VIEW_TEXT_ATTACK = 2,
    COMBAT_SMALL_VIEW_TEXT_DEFENSE = 3,
    COMBAT_SMALL_VIEW_TEXT_HIT_POINTS = 4,
    COMBAT_SMALL_VIEW_TEXT_DAMAGE = 5,
    COMBAT_SMALL_VIEW_TEXT_MORALE = 6,
    COMBAT_SMALL_VIEW_TEXT_LUCK = 7,
    COMBAT_SMALL_VIEW_TEXT_SHOTS = 8
H2_ENUM_CLASS_END(CombatSmallViewText)

H2_ENUM_CLASS_BEGIN(CombatSmallViewModifierFrame)
    COMBAT_SMALL_VIEW_BAD_LUCK_FRAME = 0,
    COMBAT_SMALL_VIEW_GOOD_LUCK_FRAME = 1,
    COMBAT_SMALL_VIEW_NEUTRAL_LUCK_FRAME = 2,
    COMBAT_SMALL_VIEW_BAD_MORALE_FRAME = 3,
    COMBAT_SMALL_VIEW_GOOD_MORALE_FRAME = 4,
    COMBAT_SMALL_VIEW_NEUTRAL_MORALE_FRAME = 5
H2_ENUM_CLASS_END(CombatSmallViewModifierFrame)

H2_ENUM_BEGIN(CombatGridConstant)
    COMBAT_GRID_ROW_COUNT = 9,
    COMBAT_GRID_ROW_LENGTH = 13,
    COMBAT_GRID_FIRST_COLUMN = 1,
    COMBAT_GRID_COLUMN_END = 12,
    COMBAT_GRID_REVERSE_FIRST_COLUMN = 11,
    COMBAT_GRID_REVERSE_COLUMN_END = 0,
    COMBAT_MANAGER_SIDE_COUNT = 2,
    COMBAT_ARMY_SLOT_COUNT = 20,
    COMBAT_ARMY_STORAGE_SLOT_COUNT = 21,
    COMBAT_DRAW_PHASE_COUNT = 4,
    COMBAT_DRAW_ALL_OCCUPANTS = 100,
    COMBAT_CASTLE_REVERSE_ROW = 5,
    COMBAT_CASTLE_GATE_ROW = 4,
    COMBAT_CASTLE_GATE_APPROACH_HEX = 58,
    COMBAT_SIDE_COUNT_DRAWING = 2,
    COMBAT_ARMY_SLOT_COUNT_DRAWING = COMBAT_ARMY_SLOT_COUNT,
    COMBAT_HERO_LEFT_X = 30,
    COMBAT_HERO_LEFT_Y = 183,
    COMBAT_HERO_RIGHT_X = 610,
    COMBAT_HERO_RIGHT_Y = 148,
    COMBAT_HERO_RIGHT_ALT_X = 615,
    COMBAT_HERO_RIGHT_ALT_Y = 135,
    COMBAT_CATAPULT_X = 22,
    COMBAT_CATAPULT_Y = 390,
    COMBAT_CASTLE_TOP_LAYER_FRAME = 0x19,
    COMBAT_CASTLE_GATE_FRAME = 0x1a,
    COMBAT_CASTLE_GATE_CLOSED_FRAME = 0x14,
    COMBAT_CASTLE_WALL_BASE_FRAME = 0x11,
    COMBAT_SCREEN_WIDTH = 0x280,
    COMBAT_AREA_HEIGHT = 0x1bb,
    COMBAT_MAX_EXTENT_X = 0x27f,
    COMBAT_MAX_EXTENT_Y = 0x1ba,
    COMBAT_OBSTACLE_TYPE_COUNT = 32,
    COMBAT_OBSTACLE_INCLUSIVE_ROLL_HIGH = 32,
    COMBAT_OBSTACLE_CELL_ROLL_MAX = 116,
    COMBAT_OBSTACLE_TRY_LIMIT = 500,
    COMBAT_OBSTACLE_MIN_COLUMN_OFFSET = 1,
    COMBAT_OBSTACLE_LEFT_COLUMN_LIMIT = 3,
    COMBAT_OBSTACLE_RIGHT_COLUMN_FIRST = 10,
    COMBAT_ELEVATION_OVERLAY_COUNT = 25,
    COMBAT_ELEVATION_OVERLAY_CELL_COUNT = 15,
    COMBAT_ELEVATION_OVERLAY_CHANCE = 40,
    COMBAT_ELEVATION_OVERLAY_TRY_LIMIT = 100,
    COMBAT_RANDOM_PERCENT_MAX = 99,
    COMBAT_RANDOM_OBSTACLE_MIN = 3,
    COMBAT_RANDOM_OBSTACLE_MAX = 7,
    COMBAT_CASTLE_STRUCTURE_COUNT = 4,
    COMBAT_DOOR_ANIMATION_DELAY = 75,
    COMBAT_DOOR_EXTENT_MIN_X = 304,
    COMBAT_DOOR_EXTENT_MIN_Y = 218,
    COMBAT_DOOR_EXTENT_MAX_X = 384,
    COMBAT_DOOR_EXTENT_MAX_Y = 294,
    COMBAT_WALL_TRACE_SUBDIVISIONS = 10,
    COMBAT_MISSILE_DIRECTION_COUNT = 9,
    COMBAT_MISSILE_LAST_DIRECTION = 8,
    COMBAT_MISSILE_HALF_WIDTH = 25,
    COMBAT_MISSILE_HALF_HEIGHT = 25,
    COMBAT_MISSILE_BITMAP_TYPE = 33,
    COMBAT_MISSILE_SPACING = 31,
    COMBAT_MISSILE_SPACING_ROUND = 15,
    COMBAT_MISSILE_TIMER_DELAY = 25,
    COMBAT_SMALL_VIEW_FULL_INFO = 2,
    COMBAT_SMALL_VIEW_LEFT_X = 5,
    COMBAT_SMALL_VIEW_RIGHT_X = 555,
    COMBAT_SMALL_VIEW_FULL_Y = 299,
    COMBAT_SMALL_VIEW_FULL_RIGHT_Y = 154,
    COMBAT_SMALL_VIEW_COMPACT_Y = 366,
    COMBAT_SMALL_VIEW_COMPACT_RIGHT_Y = 288,
    COMBAT_SMALL_VIEW_WIDTH = 83,
    COMBAT_SMALL_VIEW_FULL_HEIGHT = 141,
    COMBAT_SMALL_VIEW_COMPACT_HEIGHT = 74,
    COMBAT_SMALL_VIEW_ICON_SIZE = 19,
    COMBAT_SMALL_VIEW_MAX_SPELLS = 6,
    COMBAT_SMALL_VIEW_UNUSED_POSITION = 255,
    COMBAT_SMALL_VIEW_SPELL_X_FIRST = 1,
    COMBAT_SMALL_VIEW_SPELL_X_SECOND = 11,
    COMBAT_SMALL_VIEW_SPELL_X_THIRD = 22,
    COMBAT_SMALL_VIEW_SPELL_X_FOURTH = 32,
    COMBAT_SMALL_VIEW_SPELL_X_FIFTH = 43,
    COMBAT_SMALL_VIEW_SPELL_Y_FIRST = 0,
    COMBAT_SMALL_VIEW_SPELL_Y_SECOND = 10,
    COMBAT_SMALL_VIEW_SPELL_Y_THIRD = 21,
    COMBAT_SMALL_VIEW_INSET_X = 6,
    COMBAT_SMALL_VIEW_TEXT_X = 8,
    COMBAT_SMALL_VIEW_TEXT_WIDTH = 57,
    COMBAT_SMALL_VIEW_TEXT_HEIGHT = 12,
    COMBAT_SMALL_VIEW_QUANTITY_Y = 7,
    COMBAT_SMALL_VIEW_FIRST_STAT_Y = 20,
    COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT = 9,
    COMBAT_SMALL_VIEW_SHOTS_Y = 74,
    COMBAT_SMALL_VIEW_FULL_SPELL_Y = 86,
    COMBAT_SMALL_VIEW_COMPACT_SPELL_Y = 19,
    COMBAT_SMALL_VIEW_MODIFIER_STEP = 10,
    COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X = 57,
    COMBAT_SMALL_VIEW_NEUTRAL_MORALE_X = 45,
    COMBAT_SMALL_VIEW_NEUTRAL_LUCK_X = 50,
    COMBAT_ATTACKER_SIDE = 0,
    COMBAT_DEFENDER_SIDE = 1,
    COMBAT_ARMY_FLAG_FLYER = 2,
    COMBAT_ARMY_FLAG_SHOOTER = 4,
    COMBAT_ARMY_FLAG_MIRROR_IMAGE = 0x10,
    COMBAT_ARMY_FLAG_FULL_AI_QUANTITY = 0x80,
    COMBAT_MESSAGE_LINE_SIZE = 120,
    COMBAT_MESSAGE_WRAP_BUFFER_SIZE = 400,
    COMBAT_MESSAGE_LOG_BUFFER_SIZE = 700,
    COMBAT_MESSAGE_TIMEOUT = 2500,
    COMBAT_MESSAGE_LINE_WIDTH = 474,
    COMBAT_MESSAGE_WINDOW_X = 83,
    COMBAT_MESSAGE_WINDOW_Y = 446,
    COMBAT_MESSAGE_WINDOW_HEIGHT = 33,
    COMBAT_MESSAGE_WIDGET_FIRST = 12,
    COMBAT_MESSAGE_WIDGET_SECOND = 13,
    COMBAT_MESSAGE_BROADCAST_EVENT = 0x200,
    COMBAT_MESSAGE_TEXT_ACTION = 3,
    COMBAT_MESSAGE_DRAW_FIRST_WIDGET = 10,
    COMBAT_BALLISTA_HEX = 77,
    COMBAT_HEX_COUNT = 117,
    COMBAT_GRID_COPY_LEFT = 67,
    COMBAT_GRID_COPY_TOP = 63,
    COMBAT_GRID_COPY_RIGHT = 573,
    COMBAT_GRID_COPY_BOTTOM = 442,
    COMBAT_GRID_LINE_FRAME = 0,
    COMBAT_GRID_LINE_COLOR = 0xe2,
    COMBAT_GRID_MOUSE_FRAME = 2,
    COMBAT_MOUSE_HEX_WIDTH = 44,
    COMBAT_MOUSE_HEX_HEIGHT = 52,
    COMBAT_MOUSE_HEX_MAX_X_OFFSET = 43,
    COMBAT_MOUSE_HEX_MAX_Y_OFFSET = 51,
    COMBAT_MOUSE_REDRAW_DELAY = 75,
    COMBAT_BACKGROUND_COPY_WIDTH = 507,
    COMBAT_BACKGROUND_COPY_HEIGHT = 380,
    COMBAT_PALETTE_DATA_SIZE = 0x300,
    COMBAT_HEX_HORIZONTAL_STEP = 44,
    COMBAT_HEX_VERTICAL_STEP = 42,
    COMBAT_HEX_ROW_STAGGER = 22,
    COMBAT_HEX_CENTER_X_ORIGIN = 89,
    COMBAT_HEX_CENTER_Y_ORIGIN = 63,
    COMBAT_HEX_GRID_LEFT_ORIGIN = 67,
    COMBAT_HEX_GRID_TOP_ORIGIN = 63,
    COMBAT_GRID_INDEX_X_ORIGIN = 23,
    COMBAT_GRID_DIAGONAL_HEIGHT = 10,
    COMBAT_GRID_DIAGONAL_SLOPE_DIVISOR = 2,
    COMBAT_GRID_RIGHT_HERO_HEX = 25,
    COMBAT_GRID_LEFT_SPECIAL_X_MAX = 74,
    COMBAT_GRID_LEFT_SPECIAL_Y_MIN = 80,
    COMBAT_GRID_LEFT_SPECIAL_Y_MAX = 196,
    COMBAT_GRID_LEFT_SPECIAL_HEX = 26,
    COMBAT_GRID_RIGHT_SPECIAL_X_MIN = 566,
    COMBAT_GRID_RIGHT_UPPER_Y_MIN = 37,
    COMBAT_GRID_RIGHT_UPPER_Y_MAX = 153,
    COMBAT_GRID_RIGHT_UPPER_HEX = 25,
    COMBAT_GRID_RIGHT_LOWER_Y_MIN = 154,
    COMBAT_GRID_RIGHT_LOWER_Y_MAX = 310,
    COMBAT_VALID_HEX_MAX = 125,
    COMBAT_FIXED_ICON_COUNT = 15,
    COMBAT_OBSTACLE_ICON_LOAD_COUNT = 8,
    COMBAT_ARMY_CAPACITY = 20,
    COMBAT_SPEED_LEVEL_COUNT = 15,
    COMBAT_MAX_SPEED = 14,
    COMBAT_HERO_EXPERIENCE_VALUE = 500,
    COMBAT_GROUPED_HEX_STEP = 13,
    COMBAT_SPREAD_HEX_STEP = 26,
    COMBAT_ATTACKER_GROUPED_HEX = 27,
    COMBAT_ATTACKER_SPREAD_HEX = 1,
    COMBAT_DEFENDER_GROUPED_HEX = 37,
    COMBAT_DEFENDER_SPREAD_HEX = 11,
    COMBAT_CASTLE_BACKGROUND_BASE_FRAME = 1,
    COMBAT_CASTLE_BACKGROUND_BUILDING_FRAME = 4,
    COMBAT_CASTLE_BACKGROUND_DEFAULT_FRAME = 3
H2_ENUM_END(CombatGridConstant)

H2_ENUM_BEGIN(CombatMoraleConstant)
    COMBAT_MORALE_ROLL_MIN = 1,
    COMBAT_GOOD_MORALE_ROLL_MAX = 24,
    COMBAT_BAD_MORALE_ROLL_MAX = 12,
    COMBAT_BAD_MORALE_NETWORK_ROLL_MAX = 4,
    COMBAT_BAD_MORALE_NETWORK_SKIP_ROLL = 1,
    COMBAT_GOOD_MORALE_EFFECT = 11,
    COMBAT_BAD_MORALE_EFFECT = 12,
    COMBAT_MORALE_EFFECT_DURATION = 180
H2_ENUM_END(CombatMoraleConstant)

H2_ENUM_CLASS_BEGIN(CombatMapTrigger)
    COMBAT_TRIGGER_MINE = 0x97,
    COMBAT_TRIGGER_MONSTER = 0x98,
    COMBAT_TRIGGER_HERO = 0xaa
H2_ENUM_CLASS_END(CombatMapTrigger)

H2_ENUM_CLASS_BEGIN(CombatNearbyTileset)
    COMBAT_TILESET_SNOW_MOUNTAINS = 0x16,
    COMBAT_TILESET_SWAMP_MOUNTAINS = 0x17,
    COMBAT_TILESET_LAVA_MOUNTAINS = 0x18,
    COMBAT_TILESET_DESERT_MOUNTAINS = 0x19,
    COMBAT_TILESET_DIRT_MOUNTAINS = 0x1a,
    COMBAT_TILESET_MIXED_MOUNTAINS = 0x1b,
    COMBAT_TILESET_CRACKED_MOUNTAINS = 0x1f,
    COMBAT_TILESET_GRASS_MOUNTAINS = 0x20,
    COMBAT_TILESET_JUNGLE_TREES = 0x21,
    COMBAT_TILESET_EVIL_TREES = 0x22,
    COMBAT_TILESET_SNOW_TREES = 0x2a,
    COMBAT_TILESET_SUMMER_TREES = 0x2b,
    COMBAT_TILESET_AUTUMN_TREES = 0x2c
H2_ENUM_CLASS_END(CombatNearbyTileset)

H2_ENUM_CLASS_BEGIN(CombatIconIndex)
    COMBAT_ICON_STATUS = 1,
    COMBAT_ICON_CATAPULT = 3,
    COMBAT_ICON_TOWER = 5,
    COMBAT_ICON_KEEP = 7,
    COMBAT_ICON_SPELLS = 8,
    COMBAT_ICON_GRID = 9,
    COMBAT_ICON_SMALL_VIEW_BACKGROUND = 10,
    COMBAT_ICON_SMALL_VIEW_MODIFIER = 11,
    COMBAT_ICON_SMALL_VIEW_SPELL = 12,
    COMBAT_ICON_MOAT = 13,
    COMBAT_ICON_DRAWBRIDGE = 14
H2_ENUM_CLASS_END(CombatIconIndex)

#define COMBAT_RANDOM_X_MULTIPLIER 100
#define COMBAT_CAPTAIN_SPELL_POINT_MULTIPLIER 10
#define COMBAT_CAPTAIN_PORTRAIT_BASE 90
#define COMBAT_INITIAL_COMMAND 15
#define COMBAT_INVALID_HISTORY_INDEX (-99)

H2_ENUM_CLASS_BEGIN(CombatGridShade)
    COMBAT_GRID_SHADE_NONE = 0,
    COMBAT_GRID_SHADE_REACHABLE = 1,
    COMBAT_GRID_SHADE_EMPTY_BLOCKED = 3
H2_ENUM_CLASS_END(CombatGridShade)

H2_ENUM_BEGIN(CombatAIConstant)
    COMBAT_AI_SIDE_COUNT = 2,
    COMBAT_AI_ARMY_SLOT_COUNT = 20,
    COMBAT_AI_GROUP_SLOT_COUNT = 5,
    COMBAT_AI_GROUP_SCAN_DONE = 999,
    COMBAT_AI_DEFEAT_HERO_VICTORY = 2,
    COMBAT_AI_LAST_HERO_COUNT = 1,
    COMBAT_AI_MAX_DIFFICULTY = 4,
    COMBAT_AI_FIGHT_VALUE_MODE = 1,
    COMBAT_AI_STRENGTH_ROUNDING = 4,
    COMBAT_AI_STRENGTH_FRACTION = 5,
    COMBAT_AI_WALL_GATE = 8,
    COMBAT_AI_WALL_LEFT_TOWER = 0,
    COMBAT_AI_WALL_RIGHT_TOWER = 3,
    COMBAT_AI_WALL_INTACT = 1,
    COMBAT_AI_ATTACK_NONE = 0,
    COMBAT_AI_ATTACK_SHOOT = 1,
    COMBAT_AI_ATTACK_FLY = 2,
    COMBAT_AI_ATTACK_WALK = 3,
    COMBAT_AI_ACTION_MOVE = 2,
    COMBAT_AI_ACTION_WAIT = 3,
    COMBAT_AI_ACTION_RETREAT = 4,
    COMBAT_AI_ACTION_SURRENDER = 5,
    COMBAT_AI_ACTION_ATTACK = 6,
    COMBAT_AI_MASK_FIRST_BIT = 1,
    COMBAT_AI_NO_ARMY = -1,
    COMBAT_AI_LICH_DAMAGE_PER_CREATURE = 9,
    COMBAT_AI_LICH_HIT_POINT_BONUS = 100,
    COMBAT_AI_ATTACK_DIRECTION_COUNT = 8,
    COMBAT_AI_ALL_ATTACK_DIRECTIONS = 0xFF,
    COMBAT_AI_UNLIMITED_PATH_SPEED = 0x7f,
    COMBAT_AI_DISTANCE_WEIGHT = 1000,
    COMBAT_AI_WORST_STRENGTH_LIMIT = 999999999,
    COMBAT_AI_CLOSEST_ARMY_LIMIT = 9999999,
    COMBAT_AI_PATH_TO_FRONT = 1,
    COMBAT_AI_PATH_TO_TARGET = -1,
    COMBAT_AI_ADJACENT_DIRECTION_COUNT = 6,
    COMBAT_AI_CASTLE_BOUNDARY_COUNT = 12,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_0 = 8,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_1 = 0x15,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_2 = 0x21,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_3 = 0x2e,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_4 = 0x3a,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_5 = 0x48,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_6 = 0x55,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_7 = 99,
    COMBAT_AI_CASTLE_BOUNDARY_ROW_8 = 0x70,
    COMBAT_AI_TOWER_STRENGTH = 300,
    COMBAT_AI_MIN_ARTIFACT_VALUE = 1000,
    COMBAT_AI_MIN_HERO_EXPERIENCE = 2000,
    COMBAT_AI_MEDIUM_ARTIFACT_VALUE = 5000,
    COMBAT_AI_HIGH_ARTIFACT_VALUE = 10000,
    COMBAT_AI_STRENGTH_2500 = 2500,
    COMBAT_AI_STRENGTH_5000 = 5000,
    COMBAT_AI_STRENGTH_15000 = 15000,
    COMBAT_AI_STRENGTH_20000 = 20000,
    COMBAT_AI_STRENGTH_30000 = 30000,
    COMBAT_AI_STRENGTH_40000 = 40000,
    COMBAT_AI_EXPERIENCE_DIVISOR = 200000
H2_ENUM_END(CombatAIConstant)

H2_ENUM_CLASS_BEGIN(CombatSpellAITargetMode)
    COMBAT_SPELL_AI_GLOBAL = 0,
    COMBAT_SPELL_AI_SUM_FRIENDLY = 1,
    COMBAT_SPELL_AI_SUM_ENEMY = 2,
    COMBAT_SPELL_AI_AREA = 3,
    COMBAT_SPELL_AI_FRIENDLY = 4,
    COMBAT_SPELL_AI_ENEMY = 5,
    COMBAT_SPELL_AI_RESURRECT = 6,
    COMBAT_SPELL_AI_ANY_ARMY = 7
H2_ENUM_CLASS_END(CombatSpellAITargetMode)

H2_ENUM_BEGIN(CombatLayoutConstant)
    COMBAT_SPELL_AI_FIRST_HEX = 1,
    COMBAT_SPELL_AI_LAST_HEX = 0x73,
    COMBAT_SPELL_AI_ANY_SIDE = 2,
    COMBAT_SPELL_AI_MAX_DURATION = 10,
    COMBAT_SPELL_AI_MAX_MANA_RATIO = 10,
    COMBAT_SPELL_AI_EARTHQUAKE_WALL_FIRST = 4,
    COMBAT_SPELL_AI_EARTHQUAKE_WALL_COUNT = 4,
    COMBAT_SPELL_AI_WALL_DAMAGED = 2,
    COMBAT_SPELL_AI_WALL_DESTROYED = 6,
    COMBAT_SPELL_AI_EARTHQUAKE_NO_DAMAGE_SCORE = 29999,
    COMBAT_SPELL_AI_EARTHQUAKE_WALL_SCORE = 100,
    COMBAT_SPELL_AI_CAST_ACTION = 1
H2_ENUM_END(CombatLayoutConstant)

#define COMBAT_AI_QUANTITY_ESTIMATE 1.2
#define COMBAT_AI_TOWN_STRENGTH_MODIFIER 1.1
#define COMBAT_AI_BASE_RETREAT_CHANCE 0.16f
#define COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER 0.5
#define COMBAT_SPELL_AI_SIEGE_SHOOTER_MODIFIER 1.5
#define COMBAT_SPELL_AI_MIRROR_POWER_ONE 1
#define COMBAT_SPELL_AI_MIRROR_POWER_TWO 2
#define COMBAT_SPELL_AI_HASTE_SPEED_BONUS 2
#define COMBAT_SPELL_AI_MINIMUM_DISTANCE 2
#define COMBAT_SPELL_AI_CASTLE_DISTANCE_BONUS 3
#define COMBAT_SPELL_AI_RIGHT_DISTANCE_COLUMN 10
#define COMBAT_SPELL_AI_ALL_ATTACK_DIRECTIONS 0xFF
#define COMBAT_SPELL_AI_CURE_POINTS_PER_POWER 5
#define COMBAT_SPELL_AI_RESURRECT_POINTS_PER_POWER 50
#define COMBAT_SPELL_AI_FIRE_DAMAGE_PER_POWER 10
#define COMBAT_SPELL_AI_LIGHTNING_DAMAGE_PER_POWER 25
#define COMBAT_SPELL_AI_CHAIN_LIGHTNING_DAMAGE_PER_POWER 40
#define COMBAT_SPELL_AI_HOLY_WORD_DAMAGE_PER_POWER 10
#define COMBAT_SPELL_AI_HOLY_SHOUT_DAMAGE_PER_POWER 20
#define COMBAT_SPELL_AI_MAGIC_ARROW_DAMAGE_PER_POWER 10
#define COMBAT_SPELL_AI_ARMAGEDDON_DAMAGE_PER_POWER 50
#define COMBAT_SPELL_AI_ELEMENTAL_STORM_DAMAGE_PER_POWER 25
#define COMBAT_SPELL_AI_COLD_RAY_DAMAGE_PER_POWER 20
#define COMBAT_SPELL_AI_COLD_RING_DAMAGE_PER_POWER 10
#define COMBAT_SPELL_AI_DEATH_RIPPLE_DAMAGE_PER_POWER 5
#define COMBAT_SPELL_AI_DEATH_WAVE_DAMAGE_PER_POWER 10
#define COMBAT_SPELL_AI_DECISIVE_EFFECT 100000000
#define COMBAT_SPELL_AI_MIRROR_LETHAL_DAMAGE 999999
static const float COMBAT_SPELL_AI_BLIND_MODIFIER = -0.4f;
static const float COMBAT_SPELL_AI_BERSERK_MODIFIER = -0.55f;
static const float COMBAT_SPELL_AI_PARALYZE_MODIFIER = -0.5f;
static const float COMBAT_SPELL_AI_HYPNOTIZE_MODIFIER = -0.65f;
static const float COMBAT_SPELL_AI_PETRIFIED_MODIFIER = -0.25f;
static const float COMBAT_SPELL_AI_BLOODLUST_MODIFIER = 0.14f;
static const float COMBAT_SPELL_AI_STONE_SKIN_MODIFIER = 0.16f;
static const float COMBAT_SPELL_AI_DRAGON_SLAYER_MODIFIER = 0.28f;
static const float COMBAT_SPELL_AI_ANTI_MAGIC_MODIFIER = 0.2f;
static const float COMBAT_SPELL_AI_STEEL_SKIN_MODIFIER = 0.28f;
static const float COMBAT_SPELL_AI_SHIELD_MODIFIER = 0.45f;
static const float COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER = 1.0f;
#define COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER
#define COMBAT_SPELL_AI_MIRROR_POWER_ONE_MODIFIER 0.16f
#define COMBAT_SPELL_AI_MIRROR_POWER_TWO_MODIFIER 0.2f
#define COMBAT_SPELL_AI_MIRROR_DEFAULT_MODIFIER 0.22f
#define COMBAT_SPELL_AI_MIRROR_SHOOTER_MODIFIER 1.4
#define COMBAT_SPELL_AI_TURN_CAP 7.0f
#define COMBAT_SPELL_AI_TURN_DIVISOR 10.0f
#define COMBAT_SPELL_AI_ZERO_EFFECT 0.0
#define COMBAT_SPELL_AI_AVERAGE_DAMAGE_MODIFIER 0.5
#define COMBAT_SPELL_AI_BLESS_CURSE_MODIFIER 0.45
#define COMBAT_SPELL_AI_SIEGE_SHIELD_BONUS 0.3
#define COMBAT_SPELL_AI_CURE_VALUE_MODIFIER 0.75
#define COMBAT_SPELL_AI_RESURRECT_VALUE_MODIFIER 0.85
#define COMBAT_SPELL_AI_PARTIAL_DAMAGE_MODIFIER 0.75
#define COMBAT_SPELL_AI_DISRUPTING_RAY_MODIFIER 0.05
#define COMBAT_SPELL_AI_CASTLE_EFFECT_MODIFIER 1.25
#define COMBAT_AI_MAX_RETREAT_CHANCE_COMPARE 0.16
#define COMBAT_AI_MAX_RETREAT_CHANCE 0.16f
#define COMBAT_AI_HIGH_ARTIFACT_RETREAT_BONUS 0.06
#define COMBAT_AI_MEDIUM_ARTIFACT_RETREAT_BONUS 0.05
#define COMBAT_AI_LOW_ARTIFACT_RETREAT_BONUS 0.04
#define COMBAT_AI_STRENGTH_30000_RETREAT_PENALTY 0.08
#define COMBAT_AI_STRENGTH_15000_RETREAT_PENALTY 0.06
#define COMBAT_AI_STRENGTH_5000_RETREAT_PENALTY 0.04
#define COMBAT_AI_STRENGTH_2500_RETREAT_PENALTY 0.02
#define COMBAT_AI_DIFFICULTY_RETREAT_STEP 0.015
#define COMBAT_AI_MAX_EXPERIENCE_BONUS_COMPARE 0.03
#define COMBAT_AI_MAX_EXPERIENCE_BONUS 0.03f
#define COMBAT_AI_ATTACKER_RETREAT_PENALTY 0.06
#define COMBAT_AI_TOWER_LEVEL_SCALE 0.1
#define COMBAT_AI_TOWER_BASE_SCALE 1.0
#define COMBAT_AI_LICH_PRIORITY_MULTIPLIER 1.3
#define COMBAT_AI_LICH_HIT_POINT_SCALE 100.0f
#define COMBAT_AI_MIN_LICH_DAMAGE_SCORE (-99999.0f)

H2_ENUM_CLASS_BEGIN(CombatMessageCommand)
    COMBAT_INVALID_COMMAND = -99,
    COMBAT_MESSAGE_COMMAND_DEFAULT = 0,
    COMBAT_MESSAGE_COMMAND_MOVE = 1,
    COMBAT_MESSAGE_COMMAND_FLY = 2,
    COMBAT_MESSAGE_COMMAND_SHOOT = 3,
    COMBAT_MESSAGE_COMMAND_OPTIONS = 4,
    COMBAT_MESSAGE_COMMAND_VIEW_INFO = 5,
    COMBAT_MESSAGE_COMMAND_ATTACK = 7,
    COMBAT_MESSAGE_COMMAND_CAST_SPELL = 10,
    COMBAT_MESSAGE_COMMAND_RETREAT = 11,
    COMBAT_MESSAGE_COMMAND_SURRENDER = 12,
    COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS = 13,
    COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL = 15
H2_ENUM_CLASS_END(CombatMessageCommand)

H2_ENUM_CLASS_BEGIN(CombatMessageText)
    COMBAT_MESSAGE_TEXT_DEFAULT = 0,
    COMBAT_MESSAGE_TEXT_MOVE = 1,
    COMBAT_MESSAGE_TEXT_FLY = 2,
    COMBAT_MESSAGE_TEXT_ATTACK = 3,
    COMBAT_MESSAGE_TEXT_SHOOT = 4,
    COMBAT_MESSAGE_TEXT_HERO_OPTIONS = 5,
    COMBAT_MESSAGE_TEXT_OPPOSING_HERO = 6,
    COMBAT_MESSAGE_TEXT_VIEW_INFO = 7,
    COMBAT_MESSAGE_TEXT_NO_SHOTS = 8,
    COMBAT_MESSAGE_TEXT_CAPTAIN_OPTIONS = 9,
    COMBAT_MESSAGE_TEXT_OPPOSING_CAPTAIN = 10,
    COMBAT_MESSAGE_TEXT_BALLISTA = 11
H2_ENUM_CLASS_END(CombatMessageText)

#pragma pack(push, 1) // recovered layout is byte-packed
class combatManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    class palette* m_combatPalette;                         // +0x36
    i8 m_savedPalette[COMBAT_PALETTE_DATA_SIZE];            // +0x3a
    char m_previousCombatMessage[COMBAT_MESSAGE_LINE_SIZE]; // +0x33a
    char m_currentCombatMessage[COMBAT_MESSAGE_LINE_SIZE];  // +0x3b2
    u8 m_previousGridState[COMBAT_HEX_COUNT];               // +0x42a
    u8 m_gridState[COMBAT_HEX_COUNT];                       // +0x49f
    hexcell m_hexCells[COMBAT_HEX_COUNT];                   // +0x514
    i32 m_terrainType;                                      // +0x31de
    i32 m_battlefieldFringe;                                // +0x31e2
    class town* m_originalCombatTown;                       // +0x31e6
    WindowColorCycleMode m_colorCycleType;                                   // +0x31ea
    char _pad_0x31ee[0x4];
    i32 m_debugFormation;                               // +0x31f2
    class icon* m_combatIcons[COMBAT_FIXED_ICON_COUNT]; // +0x31f6
    class icon* m_obstacleIcons[8];                     // +0x3232
    i32 m_obstacleCount;                                // +0x3252
    i16 m_eagleEyeSpell[2];                             // +0x3256
    CombatDrawbridgeState m_drawbridgeState;                              // +0x325a
    i32 m_drawbridgeBackgroundVisible;                  // +0x325e
    u8 m_wallStates[9];                                 // +0x3262
    class bitmap* m_combatBuffer;                       // +0x326b
    class bitmap* m_backgroundBuffer;                   // +0x326f
    class bitmap* m_mouseGridBuffer;                    // +0x3273
    i32 m_backgroundDrawn;                              // +0x3277
    class mapCell* m_battlefieldCell;                   // +0x327b
    class town* m_combatTowns[2];                       // +0x327f
    class hero* m_heroes[2];                            // +0x3287
    class hero m_captain;                               // +0x328f
    i32 m_spellPower[2];                                // +0x3389
    class armyGroup* m_armyGroups[2];                   // +0x3391
    i32 m_mouseGridHex;                                 // +0x3399
    u8 m_heroDeathPending[2];                           // +0x339d
    u8 m_heroAlternateDeathPending[2];                  // +0x339f
    u8 m_heroDeathAnimationPlayed[2];                   // +0x33a1
    u8 m_heroAlternateDeathAnimationPlayed[2];          // +0x33a3
    i32 m_heroAnimationState[2];                        // +0x33a5
    i32 m_heroAnimationFrame[2];                        // +0x33ad
    i32 m_heroSpriteIndex[2];                           // +0x33b5
    i32l m_heroCycleTimer[2];                           // +0x33bd
    class icon* m_heroIcons[2];                         // +0x33c5
    class icon* m_heroOverlayIcons[2];                  // +0x33cd
    i32 m_heroOverlayFrame[2];                          // +0x33d5
    struct SLimitData m_heroLimits[2];                  // +0x33dd
    struct SLimitData m_heroOverlayLimits[2];           // +0x33fd
    struct SLimitData m_moatLimits[9];                  // +0x341d
    i32l m_previousCombatMessageExpiration;             // +0x34ad
    i32l m_combatMessageExpiration;                     // +0x34b1
    i32 m_combatMessagePending;                         // +0x34b5
    char _pad_0x34b9[0x64];
    u8 m_summonedCreatureType
        [2]; // +0x351d per-side elemental type summoned this battle; gates Summon Elemental
    i32 m_sideDefeated[2];                // +0x351f
    i32 m_networkArmyPresent[2];          // +0x3527
    i32 m_playerId[2];                    // +0x352f
    i32 m_experienceValue[2];             // +0x3537
    i32 m_heroCastSpell[2];               // +0x353f
    i32 m_armyCount[2];                   // +0x3547
    class army m_armies[2][21];           // +0x354f
    i32 m_currentArmySide;                // +0xf2a3
    i32 m_currentArmyIndex;               // +0xf2a7
    i32 m_currentSpeed;                   // +0xf2ab
    i32 m_currentSide;                    // +0xf2af
    i32 m_gridSelectionDisabled;          // +0xf2b3
    i32 m_limitCreature;                  // +0xf2b7
    i32 m_limitCreatureHex;               // +0xf2bb
    i32 m_showArmyQuantities;             // +0xf2bf
    i32 m_selectedHex;                    // +0xf2c3
    i32 m_directionTargetHex;             // +0xf2c7
    CombatMessageCommand m_previousCommand;                // +0xf2cb
    CombatMessageCommand m_currentCommand;                 // +0xf2cf
    struct SLimitData m_catapultLimits;   // +0xf2d3
    struct SLimitData m_gateLimits;       // +0xf2e3
    struct SLimitData m_upperWallLimits;  // +0xf2f3
    struct SLimitData m_middleWallLimits; // +0xf303
    i32 m_catapultFrame[2];               // +0xf313
    i32 m_catapultAttackCount[2];         // +0xf31b
    i32 m_catapultAttacksRemaining[2];    // +0xf323
    i32 m_keepAttacksRemaining[2];        // +0xf32b
    b32 m_inCastleCombat;                 // +0xf333
    i32 m_unknownF337[2];                 // +0xf337 zeroed per side in the ctor, never read
    i32 m_visitingHeroPresent[2];         // +0xf33f
    char _pad_0xf347[0x4];
    i32 m_unknownF34B;     // +0xf34b zeroed at the end of CatAttack, never read
    i32 m_unknownF34F;     // +0xf34f seeds m_catapultFrame in the ctor, zeroed in CatAttack
    i32 m_unknownF353;     // +0xf353 -1 in the ctor feeding m_unknownF34F, never read
    i32 m_nonVisualCombat; // +0xf357
    i32 m_unknownF35B;     // +0xf35b zeroed in the ctor, never read
    i32 m_killBenefit[2];  // +0xf35f
    class heroWindow* m_combatWindow; // +0xf367
    char _pad_0xf36b[0x8];
    i32 m_unknownF373;                            // +0xf373 -1 in the ctor, never read
    i32 m_sideRetreated[2];                       // +0xf377
    i32 m_limitCreatureCount[2][20];              // +0xf37f
    i32 m_drawHero[2];                            // +0xf41f
    i32 m_drawHeroOverlay[2];                     // +0xf427
    i32 m_combatWindowOpen;                       // +0xf42f
    class widget* m_winLoseBottomWidgets[25];     // +0xf433
    class widget* m_winLoseBottomTextWidgets[25]; // +0xf497
    i32 m_combatX;                                // +0xf4fb
    i32 m_combatY;                                // +0xf4ff
    i8 m_directionMap[24];                        // +0xf503
    i32 m_mouseDirection;                         // +0xf51b
    i32 m_validDirectionCount;                    // +0xf51f
    struct SLimitData m_smallViewLimits;          // +0xf523
    char _pad_0xf533[0x10];
    i32 m_smallViewSide[2];      // +0xf543
    i32 m_smallViewArmyIndex[2]; // +0xf54b
    i32 m_smallViewLastX[2];     // +0xf553
    i32 m_smallViewLastY[2];     // +0xf55b
    i32 m_smallViewWidth[2];     // +0xf563
    i32 m_smallViewHeight[2];    // +0xf56b
    char _pad_0xf573[0x4];
    u8 m_removedArmies[2][20];                                            // +0xf577
    u8 m_removedArmyPresent;                                              // +0xf59f
    char m_battlefieldBackgroundName[13];                                 // +0xf5a0
    i8 m_adjacency[COMBAT_HEX_COUNT][COMBAT_AI_ADJACENT_DIRECTION_COUNT]; // +0xf5ad
    class heroWindow* m_winLoseWindow;                                    // +0xf86b
    SpellType m_selectedSpell;                                            // +0xf86f
    i32 m_combatResult;                                                   // +0xf873
    // --- constructors ---
    combatManager(void);
    // --- virtual methods (vtable order) ---
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    // --- methods ---
    void NoShowCombatLog(char*);
    void ClearCombatMessages(i32);
    void CheckUpdateCombatMessages(void);
    void CombatMessage(char*, i32, i32, i32);
    void CombatMessage(i32);
    void ResetLimitCreature(void);
    void UpdateCombatArea(void);
    void SetupGridForArmy(class army*);
    i32 UpdateGrid(i32, i32);
    void DrawBackground(void);
    void UpdateMouseGrid(i32, i32);
    void DrawFrame(i32, i32, i32, i32, i32, i32, i32);
    void DrawSmallView(i32, i32);
    i32 ViewGeneral(i32, i32, i32);
    void ViewArmy(class army*, i32);
    i32 HasValidSpellTarget(SpellType);
    i32 ViewSpells(i32);
    i32 FindResurrectArmyIndex(i32, i32, i32);
    i32 ValidSpellTarget(SpellType, i32);
    void SpellMessage(i32, i32);
    void CastSpell(SpellType, i32, i32, i32);
    void DefaultSpell(i32);
    void Fireball(i32, SpellType);
    void MeteorShower(i32);
    void ElementalStorm(void);
    void Armageddon(void);
    void TurnToStone(class army*);
    void BloodLustEffect(class army*, i32);
    void Ripple(i32);
    void Blur(i32, i32, i32);
    void ResetBoltAngle(struct SBolt*);
    void DrawBolt(struct SBolt*, i32);
    void AddBolt(struct SBolt*, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32);
    void
        DoBolt(i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32);
    i32 GetNextChainLightningTarget(class army*, i32);
    void ChainLightning(i32, i32);
    void VaporizeCreature(i32, i32);
    void RippleCreature(i32, i32, i32);
    void ShowMassSpell(i8 (*const)[20], i32, i32);
    void CastMassSpell(SpellType, i32);
    void MirrorImage(i32);
    void SummonElemental(i32, i32);
    void DoLuck(i32, i32);
    void DoBlast(i32, i32);
    void Resurrect(i32, i32, i32);
    i32 SpaceForElementalExists(void);
    void ShowSpellCastFailure(class army*, i32);
    void ModifyDamageForArtifacts(i32l*, i32, class hero*, class hero*);
    void Earthquake(void);
    void ShowSpellMessage(i32, i32, class army*);
    i32 ValidHexToStandOn(i32);
    void SetCombatDirections(i32);
    void CheckSetMouseDirection(i32, i32, i32);
    i32 GetPointer(CombatMessageCommand, i32);
    i32 ProcessCombatMsg(struct tag_message&);
    i32 IsNegationSphereInEffect(void);
    void ResetRound(void);
    i32 CheckWin(struct tag_message*);
    CombatMessageCommand GetCommand(i32);
    i32 RightClick(i32);
    void DoCommand(i32);
    void ClearWinLoseBottom(class heroWindow*);
    void ShowWinLoseArtifact(class heroWindow*, i32);
    void ShowSkeletons(class heroWindow*);
    void ShowEagleEyeSpell(class heroWindow*);
    void ShowDeadArmies(class heroWindow*);
    void DoVictory(i32);
    void DoLoseWindow(void);
    i32 DoSurrender(void);
    void CheckChangeSelector(void);
    void CheckCastleAttack(void);
    void CheckGetAIMove(void);
    void GetControl(void);
    void ResetMouse(void);
    i32 ProcessNextAction(struct tag_message&);
    void ResetCyclingCreatures(void);
    void ResetCycleTimers(void);
    void CycleCombatScreen(void);
    void SetCombatViewArmySmallLevel(i32);
    void SetCombatGrid(i32, i32, i32);
    void AddArmy(i32, i32, i32, i32, i32, i32);
    void SetupSmallView(void);
    void ViewBallista(i32);
    i32 DoSpellAI(i32, i32);
    void DetermineEffectOfSpell(SpellType, i32*, i32*);
    i32 EffectSpellCreateCreature(i32, SpellType);
    i32 RawEffectSpellInfluence(class army*, i32);
    void ClearEffects(void);
    void NextPos(i32*);
    i32 FirstArmy(i32, i32, i32*);
    i32 FirstResurrectable(i32, i32*, i32);
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
    void UpdateArmyGroup(i32);
    void GenerateMap(void);
    char* GetBackgroundName(void);
    i32 MoreTreesNear(void);
    void LoadIcons(void);
    void FreeIcons(void);
    void LoadArmies(void);
    void FreeArmies(void);
    i32 GetGridIndex(i32, i32);
    void CheckApplyGoodMorale(i32, i32);
    i32 CheckApplyBadMorale(i32, i32);
    i32 GetNextArmy(i32);
    i32 IsWinner(i32);
    void CatAttack(i32);
    void KeepAttack(i32);
    i32 ExperienceValueOfStack(i32);
    void ResetHitByCreature(void);
    void SaveCombatBorder(void);
    void DrawCombatBorder(void);
    void SetupAndLoadObstacles(void);
    void MakeCreaturesVanish(void);
    void LowerDoor(void);
    void RaiseDoor(void);
    void TestRaiseDoor(void);
    i32 InCastle(i32);
    i32 ShotIsThroughWall(i32, i32, i32);
    void ShootMissile(i32, i32, i32, i32, float*, class icon*);
    void CombatSystemOptions(void);
    i32 AICheckRetreat(void);
    void DoCompAI(i32);
    float GetModLichDamage(class army*, float);
    void DoLichShot(class army*);
    i32 GetShooterMask(i32);
    i32 GetMirrorImageMask(i32);
    i32 GetFlyerMask(i32);
    i32 GetAllMask(i32);
    i32 GetWalkerMask(i32);
    i32 GetOutOfItMask(i32);
    i32 GetTraitorMask(i32);
    i32 GetBestArmy(i32, i32);
    i32 GetWorstArmy(i32, i32);
    i32 GetClosestArmy(class army*, i32, i32);
    u32l GetStrength(i32, i32);
    i32 AttemptAttack(class army*, i32, i32);
    i32 AttemptAdjacentAttack(class army*);
    i32 WalkTowardArmyFront(class army*, i32, i32);
    i32 WalkTowardArmy(class army*, i32, i32);
};
#pragma pack(pop)
SIZE(combatManager, 0xf877);
// ---- globals (declarations, RVA order) ----
extern i32 bGridWasShowing;
extern b32 gbInDrawSmallView;
extern i32 iViewGeneralWhichSide;
extern i32 castX;
extern i32 castY;
extern i32 bInTeleportGetDest;
extern i32 indexToCastOn;
extern u8 uRedBeam[];
extern u8 uRainbow[];
extern i8* gyModify;
extern float gfDurationMods[12];
extern float gfCancelDurationMods[11];
extern i32 bInHighMoraleBonus;
extern i32 giSeed;
extern u8 wallHex[4];
extern b32 gbThisNetHasControl;
extern i32 iCurTransferArtifact;
extern i8 iTransferArtifactsInfo[16];
extern b32 gbWhichAnimationPlaying;
extern i32 iMaxTransferArtifacts;
extern i32 giNextActionExtra;
extern i32 bSkeletonsShown;
extern b32 gbShowingLoseWindow;
extern i32 giWinCmbtFrame;
extern i32 giNextActionGridIndex;
extern i32 giSurrenderCost;
extern i32 giSkeletonsCreated;
extern i8 iTransferArtifacts[16];
extern i32 giNextAction;
extern i32 giNextActionGridIndex2;
extern i32 giCurrSpellGroup;
extern i32 bMouseWasVis;
extern class heroWindow* CSPanel;
extern i32 bCPrefsChanged;

#endif // HOMM2_SOURCE_COMBATMANAGER_H
