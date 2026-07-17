#ifndef HOMM2_SOURCE_COMBATMANAGER_H
#define HOMM2_SOURCE_COMBATMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 149 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
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
    int startX;
    int startY;
    int endX;
    int endY;
    int branchDistance;
    int width;
    int colorMode;
    int unused1C;
    int angleDistance;
    float currentX;
    float currentY;
    int pixelX;
    int pixelY;
    int drawVertically;
    float baseAngle;
    float angle;
    int finished;
    int nearTarget;
    int nearestDistance;
    int widthFirst;
    int widthLast;
    int lastBranchX;
    int lastBranchY;
    int startWidth;
    int endWidth;
    int totalDistance;
    int minAngle;
    int maxAngle;
    float distanceRatio;
    int forceAngle;
};
SIZE(SBolt, 0x78);
struct tag_message;

typedef enum CombatDrawLayer {
    COMBAT_DRAW_LAYER_COUNT = 9,
    COMBAT_DRAW_RIGHT_HERO_LAYER = 1,
    COMBAT_DRAW_LEFT_HERO_LAYER = 2,
    COMBAT_DRAW_WALL_TOP_LAYER = 4,
    COMBAT_DRAW_WALL_MIDDLE_LAYER = 5,
    COMBAT_DRAW_GATE_LAYER = 6,
    COMBAT_DRAW_CATAPULT_LAYER = 7
} CombatDrawLayer;

typedef enum CombatCastleHex {
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
} CombatCastleHex;

typedef enum CombatDrawbridgeState {
    COMBAT_DRAWBRIDGE_LOWERED = 0,
    COMBAT_DRAWBRIDGE_RAISE_FRAME_FIRST = 1,
    COMBAT_DRAWBRIDGE_RAISE_FRAME_SECOND = 2,
    COMBAT_DRAWBRIDGE_RAISED = 4
} CombatDrawbridgeState;

typedef enum CombatCastleInteriorRange {
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
} CombatCastleInteriorRange;

typedef enum CombatCastleWallSlot {
    COMBAT_WALL_SLOT_TOP_TOWER = 0,
    COMBAT_WALL_SLOT_BOTTOM_TOWER = 3,
    COMBAT_WALL_SLOT_SECTION_FIRST = 4,
    COMBAT_WALL_SLOT_SECTION_SECOND = 5,
    COMBAT_WALL_SLOT_SECTION_THIRD = 6,
    COMBAT_WALL_SLOT_SECTION_FOURTH = 7,
    COMBAT_WALL_SLOT_KEEP = 8,
    COMBAT_WALL_SECTION_COUNT = 4
} CombatCastleWallSlot;

typedef enum CombatCastleWallState {
    COMBAT_WALL_STATE_KEEP_STANDING = 0,
    COMBAT_WALL_STATE_KEEP_DESTROYED = 1,
    COMBAT_WALL_STATE_TOWER_STANDING = 1,
    COMBAT_WALL_STATE_DESTROYED = 2,
    COMBAT_WALL_STATE_SECTION_DAMAGE_FIRST = 3,
    COMBAT_WALL_STATE_SECTION_DAMAGE_LAST = 5,
    COMBAT_WALL_STATE_SECTION_DESTROYED = 6
} CombatCastleWallState;

typedef enum CombatTowerSelector {
    COMBAT_TOWER_SELECTOR_GARRISON = 0,
    COMBAT_TOWER_SELECTOR_TOP = 1,
    COMBAT_TOWER_SELECTOR_BOTTOM = 2
} CombatTowerSelector;

typedef enum CombatCoordinateAxis {
    COMBAT_COORDINATE_X = 0,
    COMBAT_COORDINATE_Y = 1
} CombatCoordinateAxis;

typedef enum CombatCatapultDamage {
    COMBAT_CATAPULT_DAMAGE_NONE = 0,
    COMBAT_CATAPULT_DAMAGE_NORMAL = 1,
    COMBAT_CATAPULT_DAMAGE_DOUBLE = 2
} CombatCatapultDamage;

typedef enum CombatKeepTargetPriority {
    COMBAT_KEEP_PRIORITY_DISABLED = 0,
    COMBAT_KEEP_PRIORITY_WALKER = 1,
    COMBAT_KEEP_PRIORITY_FLYER = 2,
    COMBAT_KEEP_PRIORITY_SHOOTER = 3
} CombatKeepTargetPriority;

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
#define COMBAT_KEEP_TOWN_TYPE_COUNT 6
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

typedef enum CombatSmallViewText {
    COMBAT_SMALL_VIEW_TEXT_QUANTITY_PLURAL = 0,
    COMBAT_SMALL_VIEW_TEXT_QUANTITY_SINGULAR = 1,
    COMBAT_SMALL_VIEW_TEXT_ATTACK = 2,
    COMBAT_SMALL_VIEW_TEXT_DEFENSE = 3,
    COMBAT_SMALL_VIEW_TEXT_HIT_POINTS = 4,
    COMBAT_SMALL_VIEW_TEXT_DAMAGE = 5,
    COMBAT_SMALL_VIEW_TEXT_MORALE = 6,
    COMBAT_SMALL_VIEW_TEXT_LUCK = 7,
    COMBAT_SMALL_VIEW_TEXT_SHOTS = 8
} CombatSmallViewText;

typedef enum CombatSmallViewModifierFrame {
    COMBAT_SMALL_VIEW_BAD_LUCK_FRAME = 0,
    COMBAT_SMALL_VIEW_GOOD_LUCK_FRAME = 1,
    COMBAT_SMALL_VIEW_NEUTRAL_LUCK_FRAME = 2,
    COMBAT_SMALL_VIEW_BAD_MORALE_FRAME = 3,
    COMBAT_SMALL_VIEW_GOOD_MORALE_FRAME = 4,
    COMBAT_SMALL_VIEW_NEUTRAL_MORALE_FRAME = 5
} CombatSmallViewModifierFrame;

enum {
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
    COMBAT_CASTLE_GATE_OPEN = 4,
    COMBAT_CASTLE_GATE_HIDDEN = 3,
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
    COMBAT_ARTIFACT_GOLDEN_BOW = 0x3f,
    COMBAT_ARTIFACT_BALLISTA = 0x15,
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
};

typedef enum CombatMoraleConstant {
    COMBAT_MORALE_ROLL_MIN = 1,
    COMBAT_GOOD_MORALE_ROLL_MAX = 24,
    COMBAT_BAD_MORALE_ROLL_MAX = 12,
    COMBAT_BAD_MORALE_NETWORK_ROLL_MAX = 4,
    COMBAT_BAD_MORALE_NETWORK_SKIP_ROLL = 1,
    COMBAT_GOOD_MORALE_EFFECT = 11,
    COMBAT_BAD_MORALE_EFFECT = 12,
    COMBAT_MORALE_EFFECT_DURATION = 180
} CombatMoraleConstant;

typedef enum CombatTerrainType {
    COMBAT_TERRAIN_WATER = 0,
    COMBAT_TERRAIN_GRASS = 1,
    COMBAT_TERRAIN_SNOW = 2,
    COMBAT_TERRAIN_SWAMP = 3,
    COMBAT_TERRAIN_LAVA = 4,
    COMBAT_TERRAIN_DESERT = 5,
    COMBAT_TERRAIN_DIRT = 6,
    COMBAT_TERRAIN_WASTELAND = 7,
    COMBAT_TERRAIN_BEACH = 8
} CombatTerrainType;

typedef enum CombatMapTrigger {
    COMBAT_TRIGGER_MINE = 0x97,
    COMBAT_TRIGGER_MONSTER = 0x98,
    COMBAT_TRIGGER_HERO = 0xaa
} CombatMapTrigger;

typedef enum CombatNearbyTileset {
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
} CombatNearbyTileset;

typedef enum CombatIconIndex {
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
} CombatIconIndex;

#define COMBAT_RANDOM_X_MULTIPLIER 100
#define COMBAT_CAPTAIN_SPELL_POINT_MULTIPLIER 10
#define COMBAT_CAPTAIN_PORTRAIT_BASE 90
#define COMBAT_CAPTAIN_MAGIC_BOOK 0x51
#define COMBAT_INITIAL_COMMAND 15
#define COMBAT_INVALID_HISTORY_INDEX (-99)

typedef enum CombatGridShade {
    COMBAT_GRID_SHADE_NONE = 0,
    COMBAT_GRID_SHADE_REACHABLE = 1,
    COMBAT_GRID_SHADE_EMPTY_BLOCKED = 3
} CombatGridShade;

typedef enum CombatAIConstant {
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
    COMBAT_AI_ALL_ATTACK_DIRECTIONS = 0xff,
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
} CombatAIConstant;

typedef enum CombatSpellAITargetMode {
    COMBAT_SPELL_AI_GLOBAL = 0,
    COMBAT_SPELL_AI_SUM_FRIENDLY = 1,
    COMBAT_SPELL_AI_SUM_ENEMY = 2,
    COMBAT_SPELL_AI_AREA = 3,
    COMBAT_SPELL_AI_FRIENDLY = 4,
    COMBAT_SPELL_AI_ENEMY = 5,
    COMBAT_SPELL_AI_RESURRECT = 6,
    COMBAT_SPELL_AI_ANY_ARMY = 7
} CombatSpellAITargetMode;

enum {
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
};

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
#define COMBAT_SPELL_AI_ALL_ATTACK_DIRECTIONS 0xff
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
#define COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER 1.0f
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

typedef enum CombatMessageCommand {
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
} CombatMessageCommand;

typedef enum CombatMessageText {
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
} CombatMessageText;

#pragma pack(push, 1)  // recovered layout is byte-packed
class combatManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    class palette *m_combatPalette;  // +0x36
    signed char m_savedPalette[COMBAT_PALETTE_DATA_SIZE];  // +0x3a
    char m_previousCombatMessage[COMBAT_MESSAGE_LINE_SIZE];  // +0x33a
    char m_currentCombatMessage[COMBAT_MESSAGE_LINE_SIZE];  // +0x3b2
    unsigned char m_previousGridState[COMBAT_HEX_COUNT];  // +0x42a
    unsigned char m_gridState[COMBAT_HEX_COUNT];  // +0x49f
    hexcell m_hexCells[COMBAT_HEX_COUNT];  // +0x514
    int m_terrainType;  // +0x31de
    int m_battlefieldFringe;  // +0x31e2
    class town *m_originalCombatTown;  // +0x31e6
    int m_colorCycleType;  // +0x31ea
    char _pad_0x31ee[0x4];
    int m_debugFormation;  // +0x31f2
    class icon *m_combatIcons[COMBAT_FIXED_ICON_COUNT];  // +0x31f6
    class icon *m_obstacleIcons[8];  // +0x3232
    int m_obstacleCount;  // +0x3252
    short m_eagleEyeSpell[2];  // +0x3256
    int m_drawbridgeState;  // +0x325a
    int m_drawbridgeBackgroundVisible;  // +0x325e
    unsigned char m_wallStates[9];  // +0x3262
    class bitmap *m_combatBuffer;  // +0x326b
    class bitmap *m_backgroundBuffer;  // +0x326f
    class bitmap *m_mouseGridBuffer;  // +0x3273
    int m_backgroundDrawn;  // +0x3277
    class mapCell *m_battlefieldCell;  // +0x327b
    class town *m_combatTowns[2];  // +0x327f
    class hero *m_heroes[2];  // +0x3287
    class hero m_captain;  // +0x328f
    int m_spellPower[2];  // +0x3389
    class armyGroup *m_armyGroups[2];  // +0x3391
    int m_mouseGridHex;  // +0x3399
    unsigned char m_heroDeathPending[2];  // +0x339d
    unsigned char m_heroAlternateDeathPending[2];  // +0x339f
    unsigned char m_heroDeathAnimationPlayed[2];  // +0x33a1
    unsigned char m_heroAlternateDeathAnimationPlayed[2];  // +0x33a3
    int m_heroAnimationState[2];  // +0x33a5
    int m_heroAnimationFrame[2];  // +0x33ad
    int m_heroSpriteIndex[2];  // +0x33b5
    long m_heroCycleTimer[2];  // +0x33bd
    class icon *m_heroIcons[2];  // +0x33c5
    class icon *m_heroOverlayIcons[2];  // +0x33cd
    int m_heroOverlayFrame[2];  // +0x33d5
    struct SLimitData m_heroLimits[2];  // +0x33dd
    struct SLimitData m_heroOverlayLimits[2];  // +0x33fd
    struct SLimitData m_moatLimits[9];  // +0x341d
    long m_previousCombatMessageExpiration;  // +0x34ad
    long m_combatMessageExpiration;  // +0x34b1
    int m_combatMessagePending;  // +0x34b5
    char _pad_0x34b9[0x64];
    unsigned char m_unknown351D[2];  // +0x351d
    int m_sideDefeated[2];  // +0x351f
    int m_networkArmyPresent[2];  // +0x3527
    int m_playerId[2];  // +0x352f
    int m_experienceValue[2];  // +0x3537
    int m_heroCastSpell[2];  // +0x353f
    int m_armyCount[2];  // +0x3547
    class army m_armies[2][21];  // +0x354f
    int m_currentArmySide;  // +0xf2a3
    int m_currentArmyIndex;  // +0xf2a7
    int m_currentSpeed;  // +0xf2ab
    int m_currentSide;  // +0xf2af
    int m_gridSelectionDisabled;  // +0xf2b3
    int m_limitCreature;  // +0xf2b7
    int m_limitCreatureHex;  // +0xf2bb
    int m_showArmyQuantities;  // +0xf2bf
    int m_selectedHex;  // +0xf2c3
    int m_directionTargetHex;  // +0xf2c7
    int m_previousCommand;  // +0xf2cb
    int m_currentCommand;  // +0xf2cf
    struct SLimitData m_catapultLimits;  // +0xf2d3
    struct SLimitData m_gateLimits;  // +0xf2e3
    struct SLimitData m_upperWallLimits;  // +0xf2f3
    struct SLimitData m_middleWallLimits;  // +0xf303
    int m_catapultFrame[2];  // +0xf313
    int m_catapultAttackCount[2];  // +0xf31b
    int m_catapultAttacksRemaining[2];  // +0xf323
    int m_keepAttacksRemaining[2];  // +0xf32b
    int m_inCastleCombat;  // +0xf333
    int m_unknownF337[2];  // +0xf337
    int m_visitingHeroPresent[2];  // +0xf33f
    char _pad_0xf347[0x4];
    int m_unknownF34B;  // +0xf34b
    int m_unknownF34F;  // +0xf34f
    int m_unknownF353;  // +0xf353
    int m_nonVisualCombat;  // +0xf357
    int m_unknownF35B;  // +0xf35b
    int m_killBenefit[2];  // +0xf35f
    class heroWindow *m_combatWindow;  // +0xf367
    char _pad_0xf36b[0x8];
    int m_unknownF373;  // +0xf373
    int m_sideRetreated[2];  // +0xf377
    int m_limitCreatureCount[2][20];  // +0xf37f
    int m_drawHero[2];  // +0xf41f
    int m_drawHeroOverlay[2];  // +0xf427
    int m_combatWindowOpen;  // +0xf42f
    class widget *m_winLoseBottomWidgets[25];  // +0xf433
    class widget *m_winLoseBottomTextWidgets[25];  // +0xf497
    int m_combatX;  // +0xf4fb
    int m_combatY;  // +0xf4ff
    signed char m_directionMap[24];  // +0xf503
    int m_mouseDirection;  // +0xf51b
    int m_validDirectionCount;  // +0xf51f
    struct SLimitData m_smallViewLimits;  // +0xf523
    char _pad_0xf533[0x10];
    int m_smallViewSide[2];  // +0xf543
    int m_smallViewArmyIndex[2];  // +0xf54b
    int m_smallViewLastX[2];  // +0xf553
    int m_smallViewLastY[2];  // +0xf55b
    int m_smallViewWidth[2];  // +0xf563
    int m_smallViewHeight[2];  // +0xf56b
    char _pad_0xf573[0x4];
    unsigned char m_removedArmies[2][20];  // +0xf577
    unsigned char m_removedArmyPresent;  // +0xf59f
    char m_battlefieldBackgroundName[13];  // +0xf5a0
    signed char m_adjacency[COMBAT_HEX_COUNT][COMBAT_AI_ADJACENT_DIRECTION_COUNT];  // +0xf5ad
    class heroWindow *m_winLoseWindow;  // +0xf86b
    int m_selectedSpell;  // +0xf86f
    int m_combatResult;  // +0xf873
    // --- constructors ---
    combatManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void NoShowCombatLog(char *);
    void ClearCombatMessages(int);
    void CheckUpdateCombatMessages(void);
    void CombatMessage(char *, int, int, int);
    void CombatMessage(int);
    void ResetLimitCreature(void);
    void UpdateCombatArea(void);
    void SetupGridForArmy(class army *);
    int UpdateGrid(int, int);
    void DrawBackground(void);
    void UpdateMouseGrid(int, int);
    void DrawFrame(int, int, int, int, int, int, int);
    void DrawSmallView(int, int);
    int ViewGeneral(int, int, int);
    void ViewArmy(class army *, int);
    int HasValidSpellTarget(int);
    int ViewSpells(int);
    int FindResurrectArmyIndex(int, int, int);
    int ValidSpellTarget(int, int);
    void SpellMessage(int, int);
    void CastSpell(int, int, int, int);
    void DefaultSpell(int);
    void Fireball(int, int);
    void MeteorShower(int);
    void ElementalStorm(void);
    void Armageddon(void);
    void TurnToStone(class army *);
    void BloodLustEffect(class army *, int);
    void Ripple(int);
    void Blur(int, int, int);
    void ResetBoltAngle(struct SBolt *);
    void DrawBolt(struct SBolt *, int);
    void AddBolt(struct SBolt *, int, int, int, int, int, int, int, int, int, int, int, int);
    void DoBolt(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int);
    int GetNextChainLightningTarget(class army *, int);
    void ChainLightning(int, int);
    void VaporizeCreature(int, int);
    void RippleCreature(int, int, int);
    void ShowMassSpell(signed char (* const)[20], int, int);
    void CastMassSpell(int, int);
    void MirrorImage(int);
    void SummonElemental(int, int);
    void DoLuck(int, int);
    void DoBlast(int, int);
    void Resurrect(int, int, int);
    int SpaceForElementalExists(void);
    void ShowSpellCastFailure(class army *, int);
    void ModifyDamageForArtifacts(long int *, int, class hero *, class hero *);
    void Earthquake(void);
    void ShowSpellMessage(int, int, class army *);
    int ValidHexToStandOn(int);
    void SetCombatDirections(int);
    void CheckSetMouseDirection(int, int, int);
    int GetPointer(int, int);
    int ProcessCombatMsg(struct tag_message &);
    int IsNegationSphereInEffect(void);
    void ResetRound(void);
    int CheckWin(struct tag_message *);
    int GetCommand(int);
    int RightClick(int);
    void DoCommand(int);
    void ClearWinLoseBottom(class heroWindow *);
    void ShowWinLoseArtifact(class heroWindow *, int);
    void ShowSkeletons(class heroWindow *);
    void ShowEagleEyeSpell(class heroWindow *);
    void ShowDeadArmies(class heroWindow *);
    void DoVictory(int);
    void DoLoseWindow(void);
    int DoSurrender(void);
    void CheckChangeSelector(void);
    void CheckCastleAttack(void);
    void CheckGetAIMove(void);
    void GetControl(void);
    void ResetMouse(void);
    int ProcessNextAction(struct tag_message &);
    void ResetCyclingCreatures(void);
    void ResetCycleTimers(void);
    void CycleCombatScreen(void);
    void SetCombatViewArmySmallLevel(int);
    void SetCombatGrid(int, int, int);
    void AddArmy(int, int, int, int, int, int);
    void SetupSmallView(void);
    void ViewBallista(int);
    int DoSpellAI(int, int);
    void DetermineEffectOfSpell(int, int *, int *);
    int EffectSpellCreateCreature(int, int);
    int RawEffectSpellInfluence(class army *, int);
    void ClearEffects(void);
    void NextPos(int *);
    int FirstArmy(int, int, int *);
    int FirstResurrectable(int, int *, int);
    void EffectSpellCure(int *, int, int, int);
    void EffectSpellResurrect(int *, int, int);
    void EffectSpellDamage(int *, int, int);
    void CombineGroups(class armyGroup *, class armyGroup *);
    void SetupCombat(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int);
    void InitNonVisualVars(void);
    void SetupAdjacencyArray(void);
    void UpdateArmyGroup(int);
    void GenerateMap(void);
    char * GetBackgroundName(void);
    int MoreTreesNear(void);
    void LoadIcons(void);
    void FreeIcons(void);
    void LoadArmies(void);
    void FreeArmies(void);
    int GetGridIndex(int, int);
    void CheckApplyGoodMorale(int, int);
    int CheckApplyBadMorale(int, int);
    int GetNextArmy(int);
    int IsWinner(int);
    void CatAttack(int);
    void KeepAttack(int);
    int ExperienceValueOfStack(int);
    void ResetHitByCreature(void);
    void SaveCombatBorder(void);
    void DrawCombatBorder(void);
    void SetupAndLoadObstacles(void);
    void MakeCreaturesVanish(void);
    void LowerDoor(void);
    void RaiseDoor(void);
    void TestRaiseDoor(void);
    int InCastle(int);
    int ShotIsThroughWall(int, int, int);
    void ShootMissile(int, int, int, int, float *, class icon *);
    void CombatSystemOptions(void);
    int AICheckRetreat(void);
    void DoCompAI(int);
    float GetModLichDamage(class army *, float);
    void DoLichShot(class army *);
    int GetShooterMask(int);
    int GetMirrorImageMask(int);
    int GetFlyerMask(int);
    int GetAllMask(int);
    int GetWalkerMask(int);
    int GetOutOfItMask(int);
    int GetTraitorMask(int);
    int GetBestArmy(int, int);
    int GetWorstArmy(int, int);
    int GetClosestArmy(class army *, int, int);
    unsigned long int GetStrength(int, int);
    int AttemptAttack(class army *, int, int);
    int AttemptAdjacentAttack(class army *);
    int WalkTowardArmyFront(class army *, int, int);
    int WalkTowardArmy(class army *, int, int);
};
#pragma pack(pop)
SIZE(combatManager, 0xf877);
// ---- globals (declarations, RVA order) ----
extern int bGridWasShowing;
extern int gbInDrawSmallView;
extern int iViewGeneralWhichSide;
extern int castX;
extern int castY;
extern int bInTeleportGetDest;
extern int indexToCastOn;
extern unsigned char uRedBeam[];
extern unsigned char uRainbow[];
extern signed char *gyModify;
extern float gfDurationMods[12];
extern float gfCancelDurationMods[11];
extern int bInHighMoraleBonus;
extern int giSeed;
extern unsigned char wallHex[4];
extern int gbThisNetHasControl;
extern int iCurTransferArtifact;
extern signed char iTransferArtifactsInfo[16];
extern int gbWhichAnimationPlaying;
extern int iMaxTransferArtifacts;
extern int giNextActionExtra;
extern int bSkeletonsShown;
extern int gbShowingLoseWindow;
extern int giWinCmbtFrame;
extern int giNextActionGridIndex;
extern int giSurrenderCost;
extern int giSkeletonsCreated;
extern signed char iTransferArtifacts[16];
extern int giNextAction;
extern int giNextActionGridIndex2;
extern int giCurrSpellGroup;
extern int bMouseWasVis;
extern class heroWindow *CSPanel;
extern int bCPrefsChanged;

#endif // HOMM2_SOURCE_COMBATMANAGER_H
