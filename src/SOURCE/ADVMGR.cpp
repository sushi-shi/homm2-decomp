#include <Ints.h>
#include <BASE/message.h>
#include <BASE/icon.h>
#include <BASE/font.h>
#include <BASE/bitmap.h>
#include <BASE/executive.h>
#include <BASE/BITS.h>
#include <BASE/iconWidget.h>
#include <BASE/textWidget.h>
#include <BASE/Icon2b.h>
#include <BASE/icon2bc.h>
#include <BASE/Iconf2b.h>
#include <BASE/Misc.h>
#include <BASE/DebugCheck.h>
#include <BASE/widgetKind.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <BASE/MusicFlags.h>
#include <BASE/resourceManager.h>
#include <BASE/sample.h>
#include <BASE/soundManager.h>
#include <BASE/tileset.h>
#include <BASE/TILE.h>
#include <BASE/WINMGR.h>
#include <EDITOR/fullMap.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/KB.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/HERO.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/fileRequester.h>
#include <SOURCE/FINDPATH.h>
#include <SOURCE/GAME.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/philAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/ADVMGR_TYPES.h>
#include <SOURCE/game.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/playerData.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/town.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum class AdventureSystemOptionsWidgetId : i32 {
    SYSTEM_OPTION_MUSIC_VOLUME = 10,
    SYSTEM_OPTION_SOUND_VOLUME = 11,
    SYSTEM_OPTION_HERO_SPEED = 12,
    SYSTEM_OPTION_MUSIC_SOURCE = 13,
    SYSTEM_OPTION_SHOW_ROUTE = 14,
    SYSTEM_OPTION_COMPUTER_SPEED = 15,
    SYSTEM_OPTION_INTERFACE = 16,
    SYSTEM_OPTION_VIDEO = 17,
    SYSTEM_OPTION_COLOR_CURSOR = 18,
    SYSTEM_OPTION_FIRST = SYSTEM_OPTION_MUSIC_VOLUME,
    SYSTEM_OPTION_LAST = SYSTEM_OPTION_COLOR_CURSOR,
    SYSTEM_OPTION_COUNT = SYSTEM_OPTION_LAST - SYSTEM_OPTION_FIRST + 1,
    SYSTEM_OPTIONS_DIALOG_ACCEPT = 0x7800
};
using enum AdventureSystemOptionsWidgetId;

typedef enum AdventureScreenConstant {
    SCREEN_WIDTH = 640,
    SCREEN_HEIGHT = 480,
    VIEWPORT_SIZE = SCREEN_HEIGHT,
    VIEW_CELL_COUNT = 15,
    SCROLL_BORDER = 16,
    VIEW_EDGE_MARGIN = 8,
    VIEW_CENTER_CELL = VIEW_EDGE_MARGIN - 1,
    VIEW_LAST_CELL = VIEW_CELL_COUNT - 1,
    RADAR_LEFT = 480,
    RADAR_RIGHT = 624,
    RADAR_TOP = 16,
    RADAR_BOTTOM = 160,
    PUZZLE_TILE_SIZE = 32,
    PUZZLE_X_TRIM = 12
} AdventureScreenConstant;

typedef enum AdventureDrawConstant {
    CELL_PIXELS = 32,
    CELL_CENTER_PIXEL = CELL_PIXELS / 2,
    CELL_LAST_PIXEL = CELL_PIXELS - 1,
    OBJECT_BASELINE_Y = CELL_PIXELS - 2,
    DRAW_VIEW_CELLS = VIEW_CELL_COUNT,
    DRAW_LAST_CELL = VIEW_LAST_CELL,
    DRAW_CLIP_WIDTH = VIEWPORT_SIZE,
    DRAW_CLIP_HEIGHT = VIEWPORT_SIZE,
    STONE_TILE_NONE = -1,
    STONE_TILE_TOP_LEFT = 16,
    STONE_TILE_TOP_RIGHT = 17,
    STONE_TILE_BOTTOM_RIGHT = 18,
    STONE_TILE_BOTTOM_LEFT = 19,
    STONE_TILE_TOP_BASE = 20,
    STONE_TILE_RIGHT_BASE = 24,
    STONE_TILE_BOTTOM_BASE = 28,
    STONE_TILE_LEFT_BASE = 32,
    STONE_PATTERN_COORDINATE_SHIFT = 16,
    CLOUD_VARIANTS = 4,
    CLOUD_VARIANT_MASK = CLOUD_VARIANTS - 1,
    CLOUD_FLIPPED_FRAME_BASE = 100,
    CLOUD_X_ALTERNATE_FRAME_1 = 1,
    CLOUD_Y_ALTERNATE_FRAME = 3,
    CLOUD_X_ALTERNATE_FRAME_2 = 5,
    ROUTE_DRAW_X_OFFSET = 12,
    ROUTE_DRAW_Y_OFFSET = 2,
    ROUTE_FRAME_MASK = 0xff,
    TERRAIN_FLAGS_SHIFT = 14,
    MINE_GUARDIAN_ICON_SLOT = 10,
    MINE_GHOST_FRAME_COUNT = 15,
    MONSTER_FRAME_STRIDE = 9,
    MONSTER_FACING_FRAME_BASE = MONSTER_FRAME_STRIDE - 2,
    MONSTER_ANIMATION_FRAME_OFFSET = 1,
    MONSTER_SPECIAL_FRAME_FIRST = 59,
    MONSTER_SPECIAL_FRAME_LAST = 60,
    HERO_BOAT_Y_OFFSET = -10,
    HERO_SHADOW_FRAME_FIRST = 9,
    HERO_SHADOW_FRAME_END = 36,
    HERO_SHADOW_FRAME_OFFSET = 50,
    HERO_SHADOW_SOURCE_FRAME_46 = 46,
    HERO_SHADOW_SOURCE_FRAME_47 = 47,
    HERO_SHADOW_SOURCE_FRAME_49 = 49,
    HERO_SHADOW_SOURCE_FRAME_50 = 50,
    HERO_SHADOW_SOURCE_FRAME_51 = 51,
    HERO_SHADOW_TARGET_FRAME_55 = 55,
    HERO_SHADOW_TARGET_FRAME_56 = 56,
    HERO_SHADOW_TARGET_FRAME_57 = 57,
    HERO_SHADOW_TARGET_FRAME_58 = 58,
    PLAYER_FLAG_FRAME_COUNT = 8,
    PLAYER_FLAG_FRAME_BASE = 56,
    TILESET_MONSTER = 20,
    TILESET_BOAT = 39
} AdventureDrawConstant;

typedef enum AdventureUpdateScreenConstant {
    UPDATE_NONE = -1,
    UPDATE_VIEWPORT_ORIGIN = 16,
    UPDATE_VIEWPORT_SIZE = 448,
    UPDATE_ANIMATION_PHASES = 6,
    UPDATE_FRAME_CYCLE = 18
} AdventureUpdateScreenConstant;

typedef enum AdventureAnimationPhaseIndex {
    ANIMATION_PHASE_COLUMN_0 = 0,
    ANIMATION_PHASE_COLUMN_1 = 1,
    ANIMATION_PHASE_COLUMN_2 = 2,
    ANIMATION_PHASE_COLUMN_3 = 3,
    ANIMATION_PHASE_COLUMN_MASK = ADVMGR_ANIMATION_PHASE_COUNT - 1,
    ANIMATION_PHASE_COLUMN_0_INITIAL = 0,
    ANIMATION_PHASE_COLUMN_1_INITIAL = 4,
    ANIMATION_PHASE_COLUMN_2_INITIAL = 11,
    ANIMATION_PHASE_COLUMN_3_INITIAL = 13
} AdventureAnimationPhaseIndex;

typedef enum AdventureSourceLineConstant {
    CLOSE_BORDER_FREE_LINE_BASE = 0x0124,
    BORDER_INITIAL_FREE_LINE_OFFSET = 22,
    BORDER_SECONDARY_FREE_LINE_OFFSET = 9,
    BORDER_ALLOC_LINE_OFFSET = 4,
    NEW_TURN_LINE_BASE = 0x11d0,
    RESOURCE_VIEW_LINE_BASE = 0x122f,
    KINGDOM_VIEW_LINE_BASE = 0x1296,
    BOTTOM_HERO_LINE_BASE = 0x12e3,
    BOTTOM_HERO_ALLOC_LINE_OFFSET = 0x44,
    QUICK_VIEW_LINE_BASE = 0x1376,
    QUICK_VIEW_FIRST_ALLOC_LINE_OFFSET = 0x9b,
    QUICK_VIEW_SECOND_ALLOC_LINE_OFFSET = 0xe3,
    QUICK_VIEW_THIRD_ALLOC_LINE_OFFSET = 0x10e,
    TOWN_VIEW_LINE_BASE = 0x14e2,
    TOWN_VIEW_FIRST_ALLOC_LINE_OFFSET = 0x83,
    TOWN_VIEW_SECOND_ALLOC_LINE_OFFSET = 0xd6,
    TOWN_VIEW_THIRD_ALLOC_LINE_OFFSET = 0x108,
    REDRAW_BORDER_FREE_LINE_BASE = 0x1628,
    SAVE_BORDER_ALLOC_LINE_BASE = 0x2025
} AdventureSourceLineConstant;

typedef enum AdventureBorderConstant {
    BORDER_BUFFER_SIZE = 0x7400,
    BORDER_EDGE_SIZE = 16,
    BORDER_VIEW_SIZE = VIEWPORT_SIZE,
    BORDER_MIDDLE_END = BORDER_VIEW_SIZE - BORDER_EDGE_SIZE,
    BORDER_ROW_BYTES = BORDER_VIEW_SIZE,
    BORDER_SCREEN_PITCH = SCREEN_WIDTH,
    BORDER_SIDE_BYTES = BORDER_EDGE_SIZE,
    BORDER_SAVED_SIDE_BYTES = BORDER_SIDE_BYTES * 2
} AdventureBorderConstant;

typedef enum AdventureScrollConstant {
    SCROLL_MIN_ORIGIN = 1 - VIEW_EDGE_MARGIN,
    VIEW_CENTER_OFFSET = VIEW_CENTER_CELL,
    SCROLL_TICK_INTERVAL = 70
} AdventureScrollConstant;

typedef enum AdventureButtonConstant {
    BUTTON_FIRST = 1,
    BUTTON_LAST = 6,
    BUTTON_TARGET = 2,
    BUTTON_SLOT_1 = 1,
    BUTTON_SLOT_2 = 2,
    BUTTON_SLOT_3 = 3,
    BUTTON_SLOT_4 = 4,
    BUTTON_SLOT_5 = 5,
    BUTTON_SLOT_6 = 6,
    BUTTON_BROADCAST_ARG = 1,
    BUTTON_BROADCAST_FLAGS = 0x4008
} AdventureButtonConstant;

typedef enum AdventureTriggerConstant {
    SPECIAL_TRIGGER_MASK = 0x3f,
    TRIGGER_EVENT_5 = 5,
    TRIGGER_EVENT_6 = 6
} AdventureTriggerConstant;

typedef enum AdventureHoverConstant {
    HOVER_VIEW_CELLS = VIEW_CELL_COUNT,
    HOVER_MAX_CELL = VIEW_LAST_CELL,
    HOVER_OBJECT_BLOCKED = 0x08,
    HOVER_UNREACHABLE = 0x80,
    HOVER_ROUTE_DAY_LIMIT = 3,
    HOVER_ROUTE_FRAMES_PER_DAY = 6,
    HOVER_SCROLL_FRAME_FIRST = 32,
    HOVER_SCROLL_FRAME_END = 40
} AdventureHoverConstant;

typedef enum AdventurePointerFrame {
    POINTER_DEFAULT = 0,
    POINTER_HERO = 2,
    POINTER_TOWN = 3,
    POINTER_MOVE = 4,
    POINTER_ATTACK = 5,
    POINTER_SAIL = 6,
    POINTER_DISEMBARK = 7,
    POINTER_SELECT_HERO = 8,
    POINTER_ACTION = 9,
    POINTER_WATER_ACTION = 28
} AdventurePointerFrame;

typedef enum AdventureFrameConstant {
    MOBILITY_SCALE = 22,
    MOBILITY_DIVISOR = 1500,
    MANA_DIVISOR = 5,
    MANA_MIN_VISIBLE = 3,
    MOBILITY_TOP_THRESHOLD = 30,
    MOBILITY_HIGH_THRESHOLD = 26,
    MOBILITY_MID_THRESHOLD = 23,
    MANA_TOP_THRESHOLD = 33,
    MANA_HIGH_THRESHOLD = 28,
    MANA_MID_THRESHOLD = 23,
    FRAME_TOP = 25,
    FRAME_HIGH = 24,
    FRAME_MID = 23
} AdventureFrameConstant;

enum class CursorSampleSet : i32 {
    CURSOR_SAMPLE_SLOW_SET = 0,
    CURSOR_SAMPLE_FAST_SET = 2
};
using enum CursorSampleSet;

typedef enum AdventureStateConstant {
    INVALID_HERO = -1,
    LOOPING_SOUND_LIMIT = 4,
    OBJECT_ICON_COUNT = ADVMGR_OBJECT_ICON_COUNT,
    HERO_ICON_COUNT = ADVMGR_HERO_ICON_COUNT,
    LOOPING_SAMPLE_COUNT = H2EnumIndex(ADVMGR_ENVIRONMENT_SOUND_COUNT),
    CURSOR_SAMPLE_COUNT = ADVMGR_CURSOR_SAMPLE_COUNT,
    SOUND_CELL_COUNT = ADVMGR_ACTIVE_SOUND_COUNT,
    HIGH_MEMORY_BUFFER_DIVISOR = 100,
    CURSOR_SAMPLE_VOLUME = 64,
    CURSOR_SAMPLE_CHANNEL = 2,
    HERO_ICON_FROTH = H2EnumIndex(HERO_TYPE_BOAT) + 1,
    ADVENTURE_FADE_STEPS = 8,
    FORCED_MUSIC_DELAY = 6000,
    QUICK_VIEW_NONE = -99,
    UNUSED_OBJECT_ICON_1 = 21,
    UNUSED_OBJECT_ICON_2 = 38
} AdventureStateConstant;

typedef enum AdventureMainConstant {
    ADVENTURE_EVENT_CELL_RESULT_COUNT = 3
} AdventureMainConstant;

typedef enum AdventureEnvironmentSoundConstant {
    ENVIRONMENT_ORIGIN_NONE = -1,
    ENVIRONMENT_SOUND_LOG_UNUSED = -999,
    ENVIRONMENT_SOUND_DEFAULT_VOLUME = 127,
    ENVIRONMENT_SOUND_MAX_DISTANCE = 5,
    ENVIRONMENT_VOLUME_COUNT = 8,
    ENVIRONMENT_SOUND_FIRST_LAYER = 1,
    ENVIRONMENT_SOUND_LAYER_COUNT = 2,
    ENVIRONMENT_SOUND_CHANNEL_TYPE = 3,
    ENVIRONMENT_SOUND_EDGE_SPAN = 2
} AdventureEnvironmentSoundConstant;

typedef enum AdventureTeleportConstant {
    TELEPORT_MAP_CHANGE_VALUE = -999,
    TELEPORT_CELL_OBJECT_FLAG = 0x40,
    TELEPORT_RESTORE_MODE = 99,
    TELEPORT_VIEW_CENTER = VIEW_CENTER_CELL,
    TELEPORT_DELAY = 90,
    TELEPORT_FIZZLE_TIME = 128,
    TELEPORT_REMOTE_FIZZLE_ADJUSTMENT = 64
} AdventureTeleportConstant;

typedef enum AdventureTravelSpellConstant {
    TRAVEL_DIALOG_ACCEPT = 1,
    TRAVEL_MUSIC = 1,
    DIMENSION_DOOR_FIRST_BUTTON = 10,
    DIMENSION_DOOR_LAST_BUTTON = 11,
    DIMENSION_DOOR_CLOSE_BUTTON = 0x7800,
    TOWN_PORTAL_WINDOW_X = 159,
    TOWN_PORTAL_WINDOW_Y = 65,
    TOWN_PORTAL_DISTANCE_LIMIT = 1000
} AdventureTravelSpellConstant;

typedef enum AdventureTownPortalMessage {
    TOWN_PORTAL_TITLE_WIDGET = 1,
    TOWN_PORTAL_CHOICE_WIDGET = 100,
    TOWN_PORTAL_CLOSE_WIDGET = 10,
    TOWN_PORTAL_FIRST_CHOICE = 0x7801,
    TOWN_PORTAL_LAST_CHOICE = 0x7802,
} AdventureTownPortalMessage;

typedef enum AdventureSoundTilesetConstant {
    SOUND_GROUND_SHAPE_MASK = 0x0f,
    SOUND_SEAGULL_FRAME_FIRST = 0xb6,
    SOUND_SEAGULL_FRAME_LAST = 0xb7,
    SOUND_ALCHEMIST_FRAME_FIRST = 0x27,
    SOUND_ALCHEMIST_FRAME_LAST = 0x2c
} AdventureSoundTilesetConstant;

typedef enum AdventureOpenConstant {
    SCROLL_Y = 195,
    SCROLL_LEFT_X = 540,
    SCROLL_RIGHT_X = 612,
    SCROLL_WIDTH = 8,
    SCROLL_HEIGHT = 17,
    SCROLL_ICON_FRAME = 4,
    SCROLL_LEFT_ID = 26,
    SCROLL_RIGHT_ID = 27,
    TIMER_DELAY = 120,
    MANAGER_MESSAGE = 0x400
} AdventureOpenConstant;

typedef enum AdventureInterfaceConstant {
    INTERFACE_AUTO = 0,
    INTERFACE_GOOD = 1,
    INTERFACE_EVIL = 2,
    INTERFACE_TRANSLATION_COUNT = 37
} AdventureInterfaceConstant;

typedef enum AdventureSystemOptionConstant {
    OPTION_INTERFACE_COUNT = 3,
    OPTION_DIALOG_MESSAGE = 1,
    OPTION_DIALOG_HELP = 4,
    OPTION_DIALOG_NONE = -1
} AdventureSystemOptionConstant;

typedef enum AdventureSummonBoatConstant {
    SUMMON_CENTER_OFFSET = VIEW_CENTER_CELL,
    SUMMON_DIRECTION_COUNT = 8,
    SUMMON_BOAT_COUNT = 48,
    SUMMON_OCCUPIED_FLAG = 0x80,
    SUMMON_MIN_DISTANCE = 2,
    SUMMON_RESTORE_MODE = 5,
    SUMMON_SCREEN_MARGIN = 16,
    SUMMON_SCREEN_LIMIT = 464,
    SUMMON_FIZZLE_X_OFFSET = 48,
    SUMMON_FIZZLE_WIDTH = 112,
    SUMMON_FIZZLE_HEIGHT = 80,
    SUMMON_TARGET_X = 154,
    SUMMON_TARGET_Y = 152,
    SUMMON_TARGET_WIDTH = 150,
    SUMMON_TARGET_HEIGHT = 136,
    SUMMON_BOAT_TRIGGER = 0xab
} AdventureSummonBoatConstant;

typedef enum AdventureRouteConstant {
    ROUTE_PATH_COST_LIMIT = 59999,
    ROUTE_TERRAIN_COST_INFINITY = 999999999,
    ROUTE_DAY_MASK = 0x100,
    ROUTE_ARROW_FRAME_STRIDE = 24,
    ROUTE_ARROW_FRAME_OFFSET = 2,
    ROUTE_STRAIGHT_COST_0 = 75,
    ROUTE_STRAIGHT_COST_1 = 100,
    ROUTE_STRAIGHT_COST_2 = 125,
    ROUTE_STRAIGHT_COST_3 = 150,
    ROUTE_STRAIGHT_COST_4 = 175,
    ROUTE_STRAIGHT_COST_5 = 200,
    ROUTE_DIAGONAL_COST_0 = 112,
    ROUTE_DIAGONAL_COST_1 = 150,
    ROUTE_DIAGONAL_COST_2 = 187,
    ROUTE_DIAGONAL_COST_3 = 225,
    ROUTE_DIAGONAL_COST_4 = 262,
    ROUTE_DIAGONAL_COST_5 = 300
} AdventureRouteConstant;

typedef enum AdventureRouteFrame {
    ROUTE_FRAME_COST_0 = 0,
    ROUTE_FRAME_COST_1 = 1,
    ROUTE_FRAME_COST_2 = 2,
    ROUTE_FRAME_COST_3 = 3,
    ROUTE_FRAME_COST_4 = 4,
    ROUTE_FRAME_COST_5 = 5
} AdventureRouteFrame;

typedef enum AdventureSearchConstant {
    ARTIFACT_CAPACITY = HERO_ARTIFACT_SLOT_COUNT,
    DIG_HOLE_FRAME = 140,
    ULTIMATE_ARTIFACT_MUSIC = 41
} AdventureSearchConstant;

typedef enum AdventureComboDrawConstant {
    COMBO_VIEW_CELLS = VIEW_CELL_COUNT,
    COMBO_GRID_CELLS = 18,
    COMBO_CLEAR_BYTES = 256,
    COMBO_CLOUD_MARK = 10,
    COMBO_FRAME_LIMIT = 12,
    COMBO_HERO_PANEL_LEFT = 5,
    COMBO_HERO_PANEL_RIGHT = 10,
    COMBO_HERO_PANEL_TOP = 6,
    COMBO_HERO_PANEL_BOTTOM = 9,
    COMBO_UPDATE_MIN = 16,
    COMBO_UPDATE_MAX = 463,
    COMBO_FAR_NEIGHBOR_OFFSET = 2,
} AdventureComboDrawConstant;

typedef enum AdventureRadarConstant {
    RADAR_SIZE = MAP_DIMENSION_XLARGE,
    RADAR_SCREEN_PITCH = SCREEN_WIDTH,
    RADAR_ROW_GROUPS = 5,
    RADAR_GROUP_BYTES = 128,
    RADAR_UNSEEN_COLOR = 0x24,
    RADAR_VIEWPORT_COLOR = 181,
    RADAR_NEUTRAL_OWNER = GAME_PLAYER_COUNT,
    RADAR_TERRAIN_SHADE = 3,
    RADAR_PARTIAL_MARGIN = 2,
    RADAR_PARTIAL_SPAN = 16,
    RADAR_CURRENT_CELL = VIEW_CENTER_CELL,
    RADAR_TOWN_RADIUS = 2,
    RADAR_SMALL_CELL_PIXELS = 4,
    RADAR_MEDIUM_CELL_PIXELS = 2,
    RADAR_LARGE_SCALE_DIVISOR = 3,
    RADAR_LARGE_SCALE_ROUNDING = RADAR_LARGE_SCALE_DIVISOR - 1,
    RADAR_FRAME_NONE = -1,
    RADAR_FRAME_NORMAL_XLARGE = 1,
    RADAR_FRAME_NORMAL_LARGE = 2,
    RADAR_FRAME_NORMAL_MEDIUM = 3,
    RADAR_FRAME_VIEW_NEAR_XLARGE = 4,
    RADAR_FRAME_NORMAL_SMALL = 5,
    RADAR_FRAME_VIEW_MIDDLE = 6,
    RADAR_FRAME_VIEW_FAR_XLARGE = 7,
    RADAR_FRAME_VIEW_NEAR_LARGE = 8,
    RADAR_FRAME_VIEW_MIDDLE_LARGE = 9,
} AdventureRadarConstant;

typedef enum AdventureArmySizeThreshold {
    ARMY_FEW_LIMIT = 5,
    ARMY_SEVERAL_LIMIT = 10,
    ARMY_PACK_LIMIT = 20,
    ARMY_LOTS_LIMIT = 50,
    ARMY_HORDE_LIMIT = 100,
    ARMY_THRONG_LIMIT = 250,
    ARMY_SWARM_LIMIT = 500,
    ARMY_ZOUNDS_LIMIT = 1000
} AdventureArmySizeThreshold;

typedef enum AdventureArmySizeBand {
    ARMY_SIZE_FEW = 0,
    ARMY_SIZE_SEVERAL = 1,
    ARMY_SIZE_PACK = 2,
    ARMY_SIZE_LOTS = 3,
    ARMY_SIZE_HORDE = 4,
    ARMY_SIZE_THRONG = 5,
    ARMY_SIZE_SWARM = 6,
    ARMY_SIZE_ZOUNDS = 7,
    ARMY_SIZE_LEGION = 8
} AdventureArmySizeBand;

typedef enum AdventureArmyQuickViewConstant {
    ARMY_QUICK_SLOT_COUNT = ARMY_GROUP_SLOT_COUNT,
    ARMY_QUICK_ICON_SIZE = 32,
    ARMY_QUICK_ICON_BASELINE = 30,
    ARMY_QUICK_AREA_LEFT = 22,
    ARMY_QUICK_CENTER_DIVISOR = 2,
    ARMY_QUICK_LABEL_HEIGHT = 12,
    ARMY_QUICK_FIRST_ROW_SHIFT = 22,
    ARMY_QUICK_SECOND_ROW_SHIFT = 44,
    ARMY_QUICK_SECOND_ROW_ICON_SHIFT = 6,
    ARMY_QUICK_FIRST_ROW_COUNT = 2,
    ARMY_QUICK_TOP_ROW_MAX = 3,
    ARMY_QUICK_FOUR_STACK_COUNT = 4,
    ARMY_QUICK_FIVE_STACK_COUNT = ARMY_GROUP_SLOT_COUNT,
    ARMY_QUICK_FIVE_STACK_X_SHIFT = 12,
    ARMY_QUICK_EMPTY_SLOT = -1,
    ARMY_QUICK_TEXT_WIDTH = 60,
    ARMY_QUICK_TEXT_X_ADJUSTMENT = 14
} AdventureArmyQuickViewConstant;

typedef enum AdventureHeroQuickViewConstant {
    HERO_QUICK_ARMY_AREA_WIDTH = 160,
    HERO_QUICK_DETAILED_CREATURE_Y = 124,
    HERO_QUICK_VAGUE_FIRST_ROW_Y = 73,
    HERO_QUICK_DEFAULT_WINDOW_X = 288,
    HERO_QUICK_LOCATOR_ROW_HEIGHT = 30,
    HERO_QUICK_LOCATOR_BASE_Y = 97,
    HERO_QUICK_WINDOW_TEXT = 18,
    HERO_QUICK_NAME_WIDGET = 1,
    HERO_QUICK_PORTRAIT_WIDGET = 2,
    HERO_QUICK_PRIMARY_STAT_WIDGET = 3,
    HERO_QUICK_MANA_WIDGET = 7,
    HERO_QUICK_PLAYER_COLOR_WIDGET = 8,
    HERO_QUICK_PLAYER_COLOR_STRIDE = 2,
    HERO_QUICK_ARMY_LABEL_CAPACITY = 5,
    HERO_QUICK_SECOND_ROW_FIRST_SLOT = 2,
    HERO_QUICK_SECOND_ROW_TEXT_SHIFT = 38
} AdventureHeroQuickViewConstant;

typedef enum AdventureTownQuickViewConstant {
    TOWN_QUICK_ARMY_AREA_WIDTH = 192,
    TOWN_QUICK_FIRST_ROW_Y = 76,
    TOWN_QUICK_DEFAULT_WINDOW_X = 328,
    TOWN_QUICK_DEFAULT_WINDOW_Y = 176,
    TOWN_QUICK_WINDOW_TEXT = 19,
    TOWN_QUICK_NAME_WIDGET = 1,
    TOWN_QUICK_PORTRAIT_WIDGET = 2,
    TOWN_QUICK_PLAYER_COLOR_WIDGET = 8,
    TOWN_QUICK_TYPE_FRAME_BASE = 9,
    TOWN_QUICK_VILLAGE_FRAME_OFFSET = 6,
    TOWN_QUICK_KNOWN_MARKER_WIDGET = 300,
    TOWN_QUICK_EMPTY_LABEL_CAPACITY = 20,
    TOWN_QUICK_EMPTY_LABEL_X = 13,
    TOWN_QUICK_EMPTY_LABEL_Y = 117,
    TOWN_QUICK_EMPTY_LABEL_WIDTH = 211,
    TOWN_QUICK_ARMY_LABEL_CAPACITY = 15,
    TOWN_QUICK_INFORMATION_UNKNOWN = 0,
    TOWN_QUICK_INFORMATION_NAMES = 1,
    TOWN_QUICK_INFORMATION_ESTIMATES = 2,
    TOWN_QUICK_INFORMATION_EXACT = 3,
    TOWN_QUICK_DEBUG_INFORMATION = 2
} AdventureTownQuickViewConstant;

typedef enum AdventureBottomHeroViewConstant {
    BOTTOM_HERO_ARMY_SLOTS = ARMY_GROUP_SLOT_COUNT,
    BOTTOM_HERO_EMPTY_SLOT = -1,
    BOTTOM_HERO_LABEL_BYTES = 6,
    BOTTOM_HERO_MAX_FULL_COUNT = 1999,
    BOTTOM_HERO_COUNT_DIVISOR = 1000,
    BOTTOM_HERO_PANEL_X = 480,
    BOTTOM_HERO_PANEL_Y = 392,
    BOTTOM_HERO_PANEL_WIDTH = 143,
    BOTTOM_HERO_PANEL_HEIGHT = 71,
    BOTTOM_HERO_ICON_WIDTH = 32,
    BOTTOM_HERO_ICON_HEIGHT = 28,
    BOTTOM_HERO_LABEL_HEIGHT = 12,
    BOTTOM_HERO_LABEL_Y_OFFSET = 25,
    BOTTOM_HERO_TOP_LABEL_SHIFT = 2,
    BOTTOM_HERO_TOP_MIN_HEIGHT = 35,
    BOTTOM_HERO_LOWER_MIN_HEIGHT = 37,
    BOTTOM_HERO_GROUP_WIDTH = 45,
    BOTTOM_HERO_CHARACTER_WIDTH = 5,
    BOTTOM_HERO_ABBREVIATED_LABEL_PADDING = 4,
    BOTTOM_HERO_POSITION_COMPONENT_COUNT = 2,
    BOTTOM_HERO_LAYOUT_SLOT_COUNT = 8,
    BOTTOM_HERO_ICON_POSITION_BYTES =
        BOTTOM_HERO_LAYOUT_SLOT_COUNT * BOTTOM_HERO_POSITION_COMPONENT_COUNT,
    BOTTOM_HERO_CREATURE_BOUND_COUNT = 2,
    BOTTOM_HERO_FIRST_ICON_ID = 2002,
    BOTTOM_HERO_FIRST_TEXT_ID = 2101,
} AdventureBottomHeroViewConstant;

typedef enum AdventureLocatorConstant {
    LOCATOR_VISIBLE_COUNT = 4,
    LOCATOR_PAGE_THRESHOLD = 5,
    LOCATOR_PAGE_DENOMINATOR_OFFSET = 4,
    LOCATOR_HERO_WIDGET_BASE = 100,
    LOCATOR_HERO_WIDGET_STRIDE = 7,
    LOCATOR_HERO_MOBILITY_OFFSET = 1,
    LOCATOR_HERO_PORTRAIT_OFFSET = 2,
    LOCATOR_HERO_MANA_OFFSET = 3,
    LOCATOR_HERO_IMAGE_OFFSET = 5,
    LOCATOR_HERO_BORDER_OFFSET = 6,
    LOCATOR_HERO_SELECT_BASE = LOCATOR_HERO_WIDGET_BASE + LOCATOR_HERO_IMAGE_OFFSET,
    LOCATOR_HERO_SELECT_1 = LOCATOR_HERO_SELECT_BASE + LOCATOR_HERO_WIDGET_STRIDE,
    LOCATOR_HERO_SELECT_2 = LOCATOR_HERO_SELECT_1 + LOCATOR_HERO_WIDGET_STRIDE,
    LOCATOR_HERO_SELECT_3 = LOCATOR_HERO_SELECT_2 + LOCATOR_HERO_WIDGET_STRIDE,
    LOCATOR_HERO_SCROLL_CONTROL = 22,
    LOCATOR_TOWN_SCROLL_CONTROL = 25,
    LOCATOR_HERO_KNOB_CONTROL = 26,
    LOCATOR_TOWN_KNOB_CONTROL = 27,
    LOCATOR_TOWN_IMAGE_BASE = 16,
    LOCATOR_TOWN_IMAGE_1 = LOCATOR_TOWN_IMAGE_BASE + 1,
    LOCATOR_TOWN_IMAGE_2 = LOCATOR_TOWN_IMAGE_1 + 1,
    LOCATOR_TOWN_IMAGE_3 = LOCATOR_TOWN_IMAGE_2 + 1,
    LOCATOR_TOWN_BORDER_BASE = 32,
    LOCATOR_TOWN_FLAG_BASE = 300,
    LOCATOR_SELECTED_COLOR = 0x99,
    LOCATOR_NORMAL_COLOR = 0x24,
    LOCATOR_HERO_EMPTY_FRAME_BASE = 1,
    LOCATOR_HERO_DEFAULT_FRAME = 21,
    LOCATOR_HERO_ENABLE_FLAGS = 6,
    LOCATOR_HERO_DISABLE_FLAGS = 4,
    LOCATOR_TOWN_EMPTY_FRAME_BASE = 5,
    LOCATOR_TOWN_ENABLE_FLAGS = 2,
    LOCATOR_TOWN_DISABLE_FLAGS = 4,
    LOCATOR_TOWN_TYPE_FRAME_BASE = 9,
    LOCATOR_TOWN_VILLAGE_FRAME_OFFSET = 6,
    LOCATOR_SCROLL_NO_PAGES_Y = 232,
    LOCATOR_SCROLL_BASE_Y = 195,
    LOCATOR_SCROLL_MOUSE_BASE_Y = LOCATOR_SCROLL_BASE_Y - 1,
    LOCATOR_SCROLL_MOUSE_SPAN = 92,
    LOCATOR_HERO_SCROLL_SPAN = 73,
    LOCATOR_TOWN_SCROLL_SPAN = 74,
    LOCATOR_KNOB_MAX_Y = LOCATOR_SCROLL_BASE_Y + LOCATOR_HERO_SCROLL_SPAN,
    LOCATOR_HERO_DRAW_LEFT = 0,
    LOCATOR_SCREEN_LEFT = 481,
    LOCATOR_SCREEN_TOP = 177,
    LOCATOR_SCREEN_ROW_HEIGHT = 32,
    LOCATOR_SCREEN_WIDTH = 54,
    LOCATOR_SCREEN_HEIGHT = 30
} AdventureLocatorConstant;

typedef enum AdventureQuickViewPlacementConstant {
    QUICK_VIEW_MIN_X = 30,
    QUICK_VIEW_MIN_Y = 16,
    QUICK_VIEW_RIGHT = 448,
    QUICK_VIEW_BOTTOM = 464,
    HERO_QUICK_VIEW_X_OFFSET = 73,
    HERO_QUICK_VIEW_Y_OFFSET = 72,
    HERO_QUICK_VIEW_WIDTH = 192,
    HERO_QUICK_VIEW_HEIGHT = 190,
    HERO_QUICK_VIEW_RIGHT_X = 256,
    HERO_QUICK_VIEW_BOTTOM_Y = 274,
    TOWN_QUICK_VIEW_X_OFFSET = 89,
    TOWN_QUICK_VIEW_Y_OFFSET = 70,
    TOWN_QUICK_VIEW_WIDTH = 224,
    TOWN_QUICK_VIEW_HEIGHT = 186,
    TOWN_QUICK_VIEW_RIGHT_X = 224,
    TOWN_QUICK_VIEW_BOTTOM_Y = 278
} AdventureQuickViewPlacementConstant;

typedef enum AdventureSpellConstant {
    TRAVEL_SPELL_MOBILITY_COST = 225,
    MINE_GUARDIAN_MAX_POWER = 51,
    MINE_GUARDIANS_PER_POWER = 4,
    MINE_GUARDIAN_VALUE_COUNT = 2,
    MINE_GUARDIAN_TYPE_INDEX = 0
} AdventureSpellConstant;

typedef enum AdventureSaveConstant {
    SAVE_PLAYER_COUNT = GAME_PLAYER_COUNT,
    SAVE_EXTENSION_SIZE = 8,
    SAVE_PATTERN_SIZE = 16,
    SAVE_REQUESTER_X = 131,
    SAVE_REQUESTER_Y = 58,
    SAVE_POINTER_FRAME = 0
} AdventureSaveConstant;

typedef enum AdventureBottomViewConstant {
    BOTTOM_VIEW_PANEL_X = 480,
    BOTTOM_VIEW_PANEL_Y = 392,
    BOTTOM_VIEW_PANEL_WIDTH = 143,
    BOTTOM_VIEW_BACKGROUND_WIDTH = 159,
    BOTTOM_VIEW_PANEL_HEIGHT = 71,
    BOTTOM_VIEW_BACKGROUND_ID = 2000,
    BOTTOM_VIEW_FOREGROUND_ID = 2001,
    BOTTOM_VIEW_TEXT_ID = 2100,
    BOTTOM_VIEW_TEXT_ID_2 = 2101,
    BOTTOM_VIEW_DRAW_LEFT = 0,
    BOTTOM_VIEW_DRAW_TOP = 2000,
    BOTTOM_VIEW_DRAW_BOTTOM = 2200,
    BOTTOM_VIEW_TEXT_BUFFER_SIZE = 30,
    BOTTOM_VIEW_COUNT_BUFFER_SIZE = 8,
    BOTTOM_VIEW_WIDGET_CAPACITY = 12,
    BOTTOM_VIEW_CENTER_DIVISOR = 2,
    BOTTOM_VIEW_NO_ENEMY = -1,
    BOTTOM_VIEW_NO_ANIMATION = -1
} AdventureBottomViewConstant;

typedef enum AdventureNewTurnViewConstant {
    NEW_TURN_DATE_ICON_X = 480,
    NEW_TURN_DATE_ICON_Y = 392,
    NEW_TURN_DATE_ICON_WIDTH = 143,
    NEW_TURN_DATE_ICON_HEIGHT = 71,
    NEW_TURN_DATE_TEXT_X = 479,
    NEW_TURN_WEEK_TEXT_Y = 424,
    NEW_TURN_DAY_TEXT_Y = 438,
    NEW_TURN_DATE_TEXT_WIDTH = 145,
    NEW_TURN_WEEK_TEXT_HEIGHT = 12,
    NEW_TURN_DAY_TEXT_HEIGHT = 25,
    NEW_TURN_FIRST_DAY = 1,
    NEW_TURN_WEEK_ALLOC_LINE_OFFSET = 0x29,
    NEW_TURN_DAY_ALLOC_LINE_OFFSET = 0x43
} AdventureNewTurnViewConstant;

typedef enum AdventureResourceViewConstant {
    RESOURCE_VIEW_TEXT_BASE_Y = 398,
    RESOURCE_VIEW_MULTILINE_HEIGHT = 32,
    RESOURCE_VIEW_LINE_HEIGHT = 6,
    RESOURCE_VIEW_TEXT_HEIGHT = 36,
    RESOURCE_VIEW_GOLD_WIDTH = 76,
    RESOURCE_VIEW_GOLD_HEIGHT = 26,
    RESOURCE_VIEW_ICON_WIDTH = 38,
    RESOURCE_VIEW_ICON_HEIGHT = 32,
    RESOURCE_VIEW_ICON_BOTTOM = 463,
    RESOURCE_VIEW_ICON_BOTTOM_PADDING = 14,
    RESOURCE_VIEW_COUNT_X = 511,
    RESOURCE_VIEW_COUNT_Y = 453,
    RESOURCE_VIEW_COUNT_WIDTH = 80,
    RESOURCE_VIEW_COUNT_HEIGHT = 12,
    RESOURCE_VIEW_MESSAGE_ALLOC_LINE_OFFSET = 0x26,
    RESOURCE_VIEW_COUNT_ALLOC_LINE_OFFSET = 0x50
} AdventureResourceViewConstant;

typedef enum AdventureKingdomViewConstant {
    KINGDOM_VIEW_RESOURCE_COUNT = H2EnumIndex(RES_COUNT),
    KINGDOM_VIEW_ENTRY_COUNT = 9,
    KINGDOM_VIEW_CASTLE_ENTRY = 7,
    KINGDOM_VIEW_TOWN_ENTRY = 8,
    KINGDOM_VIEW_ICON_X = 481,
    KINGDOM_VIEW_ICON_Y = 393,
    KINGDOM_VIEW_TEXT_X_BASE = 464,
    KINGDOM_VIEW_TEXT_Y_BASE = 395,
    KINGDOM_VIEW_TEXT_WIDTH = 32,
    KINGDOM_VIEW_TEXT_HEIGHT = 12,
    KINGDOM_VIEW_RESOURCE_TEXT_Y = 59,
    KINGDOM_VIEW_TOWN_TEXT_Y = 28,
    KINGDOM_VIEW_WOOD_TEXT_X = 15,
    KINGDOM_VIEW_MERCURY_TEXT_X = 38,
    KINGDOM_VIEW_ORE_TEXT_X = 61,
    KINGDOM_VIEW_SULFUR_TEXT_X = 85,
    KINGDOM_VIEW_CRYSTAL_TEXT_X = 109,
    KINGDOM_VIEW_GEMS_TEXT_X = 132,
    KINGDOM_VIEW_GOLD_TEXT_X = 123,
    KINGDOM_VIEW_CASTLE_TEXT_X = 27,
    KINGDOM_VIEW_VILLAGE_TEXT_X = 80,
    KINGDOM_VIEW_COUNT_ALLOC_LINE_OFFSET = 0x32
} AdventureKingdomViewConstant;

typedef enum AdventureVisionsConstant {
    VISIONS_RADIUS = 3,
    VISIONS_NO_MONSTER_DISTANCE = 100,
    VISIONS_MESSAGE_BUFFER_SIZE = 200,
    VISIONS_JOIN_COST_MULTIPLIER = 2
} AdventureVisionsConstant;

typedef enum AdventureEnemyTurnViewConstant {
    ENEMY_TURN_BACKGROUND_X = 480,
    ENEMY_TURN_BACKGROUND_Y = 392,
    ENEMY_TURN_BACKGROUND_WIDTH = 143,
    ENEMY_TURN_BACKGROUND_HEIGHT = 71,
    ENEMY_TURN_HOURGLASS_X = 493,
    ENEMY_TURN_HOURGLASS_Y = 403,
    ENEMY_TURN_HOURGLASS_WIDTH = 118,
    ENEMY_TURN_HOURGLASS_HEIGHT = 51,
    ENEMY_TURN_CREST_X = 495,
    ENEMY_TURN_ANIMATION_X = 559,
    ENEMY_TURN_ANIMATION_Y = 405,
    ENEMY_TURN_ANIMATION_WIDTH = 50,
    ENEMY_TURN_ANIMATION_HEIGHT = 47,
    ENEMY_TURN_BACKGROUND_ID = 2000,
    ENEMY_TURN_HOURGLASS_ID = 2001,
    ENEMY_TURN_CREST_ID = 2002,
    ENEMY_TURN_SAND_ID = 2003,
    ENEMY_TURN_PHASE_ID = 2004,
    ENEMY_TURN_BACKGROUND_Z = 1000,
    ENEMY_TURN_HOURGLASS_Z = 1010,
    ENEMY_TURN_SAND_Z = 1020,
    ENEMY_TURN_CREST_Z = 1030,
    ENEMY_TURN_PHASE_Z = 1040,
    ENEMY_TURN_SAND_FRAME_OFFSET = 11,
    ENEMY_TURN_SAND_FRAME_LIMIT = 20,
    ENEMY_TURN_SAND_RESTART_FRAME = 16,
    ENEMY_TURN_PHASE_FRAME_OFFSET = 1,
    ENEMY_TURN_CREST_SLOT = 0,
    ENEMY_TURN_SAND_SLOT = 1,
    ENEMY_TURN_PHASE_SLOT = 2,
    ENEMY_TURN_ANIMATION_DELAY = 300,
    ENEMY_TURN_PHASE_DELAY = 700
} AdventureEnemyTurnViewConstant;

typedef enum AdventureCloudNeighbor {
    CLOUD_NORTH = 0x01,
    CLOUD_EAST = 0x02,
    CLOUD_SOUTH = 0x04,
    CLOUD_WEST = 0x08,
    CLOUD_NORTH_EAST = 0x10,
    CLOUD_SOUTH_EAST = 0x20,
    CLOUD_SOUTH_WEST = 0x40,
    CLOUD_NORTH_WEST = 0x80,
    CLOUD_LEFT_EDGE = 0xc8,
    CLOUD_RIGHT_EDGE = 0x32,
    CLOUD_TOP_EDGE = 0x91,
    CLOUD_BOTTOM_EDGE = 0x64
} AdventureCloudNeighbor;

typedef enum AdventureQuickInfoObject {
    OBELISK_INDEX_BASE = 1,
    RESOURCE_FRAME_PAIR_MASK = ~1,
    RESOURCE_FRAME_PAIR_STRIDE = 2,
    CRYSTAL_BALL_RADIUS = 8,
    BARRIER_COLOR_MASK = KB_BARRIER_COLOR_NAME_COUNT - 1,
    ROUTE_BEYOND_MOBILITY_FLAG = 0x100,
    HERO_FRAME_MIRROR_FLAG = 0x80,
    HERO_FRAME_INDEX_MASK = 0x7f,
    QUICK_INFO_TEXT_CAPACITY = 200,
    QUICK_INFO_SAVED_TEXT_CAPACITY = 800,
    QUICK_INFO_X_OFFSET = 57,
    QUICK_INFO_MIN_X = QUICK_VIEW_MIN_X,
    QUICK_INFO_WIDTH = 160,
    QUICK_INFO_RIGHT = QUICK_VIEW_BOTTOM,
    QUICK_INFO_RIGHT_X = QUICK_INFO_RIGHT - QUICK_INFO_WIDTH,
    QUICK_INFO_Y_OFFSET = 25,
    QUICK_INFO_MIN_Y = QUICK_VIEW_MIN_Y,
    QUICK_INFO_HEIGHT = 96,
    QUICK_INFO_BOTTOM = QUICK_VIEW_RIGHT,
    QUICK_INFO_BOTTOM_Y = QUICK_INFO_BOTTOM - QUICK_INFO_HEIGHT
} AdventureQuickInfoObject;

typedef enum AdventureCheatConstant {
    CHEAT_SEQUENCE_RADIX = 10,
    CHEAT_SEQUENCE_MODULUS = 10000000,
    CHEAT_CREATURES_MODULUS = 100000,
    CHEAT_LOSE_MODULUS = 10000,
    CHEAT_SHORT_MODULUS = 1000,
    CHEAT_CREATURES = 32167,
    CHEAT_WIN = 911,
    CHEAT_LOSE = 1313,
    CHEAT_REVEAL_MAP = 8675309,
    CHEAT_INFO = 411,
    CHEAT_RESOURCE_AMOUNT = 10,
    CHEAT_GOLD_AMOUNT = 1000,
    CHEAT_EXPERIENCE_AMOUNT = 3000,
    CHEAT_BLACK_DRAGON_COUNT = 5,
    CHEAT_REVEAL_CENTER = 30,
    CHEAT_REVEAL_RADIUS = 180
} AdventureCheatConstant;

typedef enum AdventureSiteConstant {
    GENERIC_SITE_1_END = 4,
    GENERIC_SITE_2_END = 0x48,
    GENERIC_ALTAR_END = 5,
    GENERIC_UNUSED_END = 10,
    GENERIC_TOWER_END = 0x2f,
    GENERIC_SPRING_END = 0x6f,
    GENERIC_SITE_3_SPLIT = 0x20,
    GENERIC_SITE_3_END = 0x3b,
    RECRUITMENT_START = 0x48,
    RECRUITMENT_1_END = 0x4e,
    RECRUITMENT_2_END = 0x70,
    RECRUITMENT_3_END = 0x78,
    RECRUITMENT_4_END = 0x81,
    RECRUITMENT_5_END = 0x89
} AdventureSiteConstant;

typedef enum AdventurePanelCommand {
    PANEL_NEXT_HERO = 1,
    PANEL_CONTINUE_ROUTE = 2,
    PANEL_OVERVIEW = 3,
    PANEL_END_TURN = 4,
    PANEL_ADVENTURE_OPTIONS = 5,
    PANEL_CONTROL_OPTIONS = 6,
    PANEL_SYSTEM_OPTIONS = 7,
    PANEL_CAST_SPELL = 8,
    PANEL_RADAR = 9,
    PANEL_MAP_VIEW = 10,
    PANEL_HERO_PAGE_PREVIOUS = 20,
    PANEL_HERO_PAGE_NEXT = 21,
    PANEL_TOWN_PAGE_PREVIOUS = 23,
    PANEL_TOWN_PAGE_NEXT = 24,
    DIALOG_OK = 0x7806,
    BOTTOM_VIEW_FIRST_MESSAGE = 2000,
    BOTTOM_VIEW_LAST_MESSAGE = 2200,
    BOTTOM_VIEW_DURATION = 3000,
    LOCATORS_PER_PAGE = 5,
    LOCATOR_PAGE_STEP = 4
} AdventurePanelCommand;

typedef enum AdventureAdjacentMonsterConstant {
    ADJACENT_MONSTER_RADIUS = 1,
    ADJACENT_MONSTER_END_OFFSET = 2,
    ADJACENT_OBJECT_INDEX_NONE = 0xFF
} AdventureAdjacentMonsterConstant;

typedef enum AdventurePuzzleViewConstant {
    PUZZLE_MUSIC = 23,
    PUZZLE_WINDOW_X = DRAW_CLIP_WIDTH,
    PUZZLE_WINDOW_Y = SCROLL_BORDER,
    PUZZLE_VIEW_ORIGIN = SCROLL_BORDER,
    PUZZLE_VIEW_SIZE = UPDATE_VIEWPORT_SIZE,
    PUZZLE_VIEW_END = PUZZLE_VIEW_ORIGIN + PUZZLE_VIEW_SIZE,
    PUZZLE_COORDINATE_OFFSET = 7,
    PUZZLE_ALIGNMENT_DIVISOR = 3,
    PUZZLE_Y_ADJUST_X_FACTOR = 2,
    PUZZLE_Y_ADJUST_Y_FACTOR = 5,
    PUZZLE_PARITY_DIVISOR = 2,
    PUZZLE_FIZZLE_TIME = 220
} AdventurePuzzleViewConstant;

typedef enum AdventurePanelDialogConstant {
    PANEL_NO_HELP = -1,
    PANEL_WINDOW_X = 144,
    PANEL_WINDOW_Y = 40,
    PANEL_DISABLED_WIDGET = 4,
    PANEL_FIRST_WIDGET = 1,
    PANEL_LAST_WIDGET = 4,
    PANEL_CLOSE_WIDGET = 0x7800,
    PANEL_VIEW_WORLD_HELP = 0,
    PANEL_VIEW_PUZZLE_HELP = 1,
    PANEL_SCENARIO_INFO_HELP = 2,
    PANEL_SEARCH_HELP = 3,
    PANEL_CLOSE_HELP = 4,
    PANEL_VIEW_WORLD = 1,
    PANEL_VIEW_PUZZLE = 2,
    PANEL_SCENARIO_INFO = 3,
    PANEL_SEARCH = 4,
    CONTROL_RESTART = 101,
    CONTROL_NEW_GAME = 102,
    CONTROL_MAIN_MENU = 105,
    CONTROL_SAVE_GAME = 106,
    CONTROL_RESTART_HELP = 0,
    CONTROL_NEW_GAME_HELP = 1,
    CONTROL_SAVE_GAME_HELP = 2,
    CONTROL_MAIN_MENU_HELP = 3,
    CONTROL_CLOSE_HELP = 4,
    CONTROL_CONFIRMATION_SIZE = 200
} AdventurePanelDialogConstant;

typedef enum AdventureSystemOptionsPrivateConstant {
    SYSTEM_OPTIONS_HELP_ACCEPT = 0,
    SYSTEM_OPTIONS_HELP_MUSIC_VOLUME = 1,
    SYSTEM_OPTIONS_HELP_SOUND_VOLUME = 2,
    SYSTEM_OPTIONS_HELP_HERO_SPEED = 3,
    SYSTEM_OPTIONS_HELP_MUSIC_SOURCE = 4,
    SYSTEM_OPTIONS_HELP_SHOW_ROUTE = 5,
    SYSTEM_OPTIONS_HELP_COMPUTER_SPEED = 6,
    SYSTEM_OPTIONS_HELP_INTERFACE = 7,
    SYSTEM_OPTIONS_HELP_VIDEO = 8,
    SYSTEM_OPTIONS_HELP_COLOR_CURSOR = 9,
    SYSTEM_OPTIONS_HELP_FIRST_OPTION = SYSTEM_OPTIONS_HELP_MUSIC_VOLUME,
    SYSTEM_OPTIONS_TEXT_CAPACITY = 120
} AdventureSystemOptionsPrivateConstant;

enum class AdventureMusicQuality : i32 {
    MUSIC_QUALITY_MIDI = 0,
    MUSIC_QUALITY_CD_STEREO = 1,
    MUSIC_QUALITY_CD_OPERA = 2
};
using enum AdventureMusicQuality;

#define ADVMGR_ENVIRONMENT_VOLUME(distance) environmentVolumes[distance]
#define ADVMGR_RADAR_LEFT_FLOAT \
    static_cast<float>(RADAR_LEFT)
#define ADVMGR_RADAR_TOP_FLOAT \
    static_cast<float>(RADAR_TOP)
#define ADVMGR_LOCATOR_SCROLL_BASE_Y_DOUBLE \
    static_cast<double>(LOCATOR_SCROLL_BASE_Y)
#define ADVMGR_LOCATOR_HERO_SCROLL_SPAN_DOUBLE \
    static_cast<double>(LOCATOR_HERO_SCROLL_SPAN)
#define ADVMGR_LOCATOR_TOWN_SCROLL_SPAN_DOUBLE \
    static_cast<double>(LOCATOR_TOWN_SCROLL_SPAN)
#define ADVMGR_REMOTE_PAYLOAD(packet) (reinterpret_cast<AdventureRemotePayload*>((packet)->payload))


#define ADVMGR_VISIBILITY_AT(column, row) (*(m_visibilityMap + column + (row) * MAP_WIDTH))

static const i32 environmentVolumes[ENVIRONMENT_VOLUME_COUNT] = {64, 57, 40, 21, 7, 5, 3, 0};

i32 iThisMaxY = 0;
static i32 s_drawMonsterFrame = 0;
i32 iLastAnimFrame = 0;
i32 giFrameStep = 0;
struct tag_message CDMsg = {};
static i32 s_drawHeroYOffset = 0;
i32 iThisMinY = 0;
static mapCellExtra* s_drawExtra = NULL;
static i32 s_adjacentMonsterEndY = 0;
static i32 s_drawFlipCloud = 0;
static i32 s_drawHeroFrame = 0;
static i32 s_drawStoneTile = 0;
static mapCell* s_drawCell = NULL;
static i32 s_adjacentMonsterX = 0;
static i32 s_adjacentMonsterEndX = 0;
static mapCell* s_drawAdjacentCell = NULL;
static i32 s_adjacentMonsterY = 0;
static u16 s_drawGroundTile = 0;
i32 giLimitUpdMaxX = 0;
i32 giLimitUpdMaxY = 0;
i32 bPrefsChanged = 0;
i32 giTownPortalChoice = 0;
i8 bComboDraw[COMBO_GRID_CELLS][COMBO_GRID_CELLS] = {0};
static i32 s_drawCovered = 0;
i32 giLimitUpdMinY = 0;
static mineRecord* s_drawMine = NULL;
static i32 s_adjacentMonsterMinX = 0;
static i32 s_drawCloudFrame = 0;
static i32 s_drawPlayerColor = 0;
struct tag_message USMsg = {};
class heroWindow* cPanel = NULL;
static i32 s_drawPixelY = 0;
static H2EnumStorage<HeroCursorType, i32> s_drawHeroType = 0;
static hero* s_drawHero = NULL;
class heroWindow* townPortalWin = NULL;
static i32 s_drawHasHero = 0;
static i32 s_drawAnimationLength = 0;
static i32 s_drawPixelX = 0;
char cArmySizeName[ADVMGR_ARMY_SIZE_NAME_SIZE] = {0};
static i32 s_adjacentMonsterMinY = 0;
typedef enum AdvVisitMetadata {
    VISIT_BIT_INDEX_MASK = 0x1f
} AdvVisitMetadata;

advManager::advManager(void) {
    m_groundTiles = NULL;
    m_puzzleIcon = NULL;
    m_mapOriginX = 0;
    m_mapOriginY = 0;
    m_updateMinX = 0;
    m_updateMinY = 0;
    m_updateMaxX = 0;
    m_updateMaxY = 0;
    m_updatePending = 0;
    m_selectedCell = ADVMGR_COMMAND_NONE;
    m_cursorActive = 0;
    m_identifyHeroActive = 0;
    m_drawHeroShadows = 1;
    m_adventureBorder = NULL;

    i32 index;
    for (index = 0; index < OBJECT_ICON_COUNT; ++index) {
        m_objectIcons[index] = NULL;
    }
    for (index = 0; index < HERO_ICON_COUNT; ++index) {
        m_heroIcons[index] = NULL;
    }
    for (index = 0; index < PLAYER_COLOR_COUNT; ++index) {
        m_flagIcons[index] = NULL;
        m_boatFlagIcons[index] = NULL;
    }
    for (index = 0; index < LOOPING_SAMPLE_COUNT; ++index) {
        m_loopingSamples[index] = NULL;
    }
    for (index = 0; index < CURSOR_SAMPLE_COUNT; ++index) {
        m_cursorSamples[index] = NULL;
    }

    m_puzzleIcon = NULL;
    m_cloudOverlayIcon = NULL;
    m_boatShadowIcon = NULL;
    m_shadowIcon = NULL;
    m_groundTiles = NULL;
    m_cloudTiles = NULL;
    m_stoneTiles = NULL;
    m_adventureWindow = NULL;
    m_visibilityMap = NULL;
    m_heroContextLocked = 0;
    m_townContextLocked = 0;
    bShowIt = 1;
    m_lastQuickViewX = QUICK_VIEW_NONE;
    m_lastQuickViewY = QUICK_VIEW_NONE;
    m_animationPhases[ANIMATION_PHASE_COLUMN_0] = ANIMATION_PHASE_COLUMN_0_INITIAL;
    m_animationPhases[ANIMATION_PHASE_COLUMN_1] = ANIMATION_PHASE_COLUMN_1_INITIAL;
    m_animationPhases[ANIMATION_PHASE_COLUMN_2] = ANIMATION_PHASE_COLUMN_2_INITIAL;
    m_animationPhases[ANIMATION_PHASE_COLUMN_3] = ANIMATION_PHASE_COLUMN_3_INITIAL;
    m_mapData = gpGame->GetWorldMapData();
    gMapX = 0;
    gMapY = 0;
    m_cursorFrameCount = 0;
    m_cursorCycle = 0;
    m_cursorTurning = 0;
}

i32 advManager::Open(i32 id) {
    iCurBottomView = BOTTOM_VIEW_NONE;
    m_openState = 0;
    bShowIt = 0;
    m_adventureBorder = NULL;

    i32 i;
    for (i = 0; i < ADVMGR_LOCATOR_STATE_COUNT; ++i) {
        m_heroLocatorState[i] = 0;
        m_townLocatorState[i] = 0;
    }

    if (m_adventureWindow == NULL) {
        m_adventureWindow = new heroWindow(
            0,
            0,
            "adv_wind.bin"
        );
        if (m_adventureWindow == NULL) {
            MemError();
        }
        m_scrollLeftButton = new iconWidget(
            SCROLL_LEFT_X,
            SCROLL_Y,
            SCROLL_WIDTH,
            SCROLL_HEIGHT,
            "scroll.icn",
            SCROLL_ICON_FRAME,
            ICON_DRAW_NORMAL,
            SCROLL_LEFT_ID,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (m_scrollLeftButton == NULL) {
            MemError();
        }
        m_adventureWindow->AddWidget(m_scrollLeftButton, -1);
        m_scrollRightButton = new iconWidget(
            SCROLL_RIGHT_X,
            SCROLL_Y,
            SCROLL_WIDTH,
            SCROLL_HEIGHT,
            "scroll.icn",
            SCROLL_ICON_FRAME,
            ICON_DRAW_NORMAL,
            SCROLL_RIGHT_ID,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (m_scrollRightButton == NULL) {
            MemError();
        }
        m_adventureWindow->AddWidget(m_scrollRightButton, -1);
    }

    if (gbThisNetHumanPlayer[giCurPlayer]) {
        gpMouseManager->SetPointer(
            "advmice.mse",
            0,
            MOUSE_AUTO_CURSOR_TYPE
        );
    } else {
        gpMouseManager->SetPointer(
            "advmice.mse",
            1,
            MOUSE_AUTO_CURSOR_TYPE
        );
    }

    if (m_visibilityMap == NULL) {
        m_visibilityMap = new u16[MAP_WIDTH * MAP_HEIGHT];
        if (m_visibilityMap == NULL) {
            MemError();
        }
    }
    m_visibilityMapValid = 0;
    gpWindowManager->AddWindow(m_adventureWindow, 0, 1);

    if (m_groundTiles == NULL) {
        m_groundTiles = gpResourceManager->GetTileset(
            "ground32.til"
        );
    }
    if (m_cloudTiles == NULL) {
        m_cloudTiles = gpResourceManager->GetTileset(
            "clof32.til"
        );
    }
    if (m_stoneTiles == NULL) {
        m_stoneTiles = gpResourceManager->GetTileset(
            "ston.til"
        );
    }
    if (m_cloudOverlayIcon == NULL) {
        m_cloudOverlayIcon = gpResourceManager->GetIcon(
            "clop32.icn"
        );
    }

    for (i = 0; i < OBJECT_ICON_COUNT; ++i) {
        if (strlen(gTilesetFiles[i]) > 1 && m_objectIcons[i] == NULL
            && i != UNUSED_OBJECT_ICON_1 && i != UNUSED_OBJECT_ICON_2) {
            m_objectIcons[i] = gpResourceManager->GetIcon(gTilesetFiles[i]);
        }
    }

    if (m_heroIcons[H2EnumIndex(FACTION_KNIGHT)] == NULL) {
        m_heroIcons[H2EnumIndex(FACTION_KNIGHT)] = gpResourceManager->GetIcon(
            "kngt32.icn"
        );
    }
    if (m_heroIcons[H2EnumIndex(FACTION_BARBARIAN)] == NULL) {
        m_heroIcons[H2EnumIndex(FACTION_BARBARIAN)] = gpResourceManager->GetIcon(
            "barb32.icn"
        );
    }
    if (m_heroIcons[H2EnumIndex(FACTION_SORCERESS)] == NULL) {
        m_heroIcons[H2EnumIndex(FACTION_SORCERESS)] = gpResourceManager->GetIcon(
            "sorc32.icn"
        );
    }
    if (m_heroIcons[H2EnumIndex(FACTION_WARLOCK)] == NULL) {
        m_heroIcons[H2EnumIndex(FACTION_WARLOCK)] = gpResourceManager->GetIcon(
            "wrlk32.icn"
        );
    }
    if (m_heroIcons[H2EnumIndex(FACTION_WIZARD)] == NULL) {
        m_heroIcons[H2EnumIndex(FACTION_WIZARD)] = gpResourceManager->GetIcon(
            "wzrd32.icn"
        );
    }
    if (m_heroIcons[H2EnumIndex(FACTION_NECROMANCER)] == NULL) {
        m_heroIcons[H2EnumIndex(FACTION_NECROMANCER)] = gpResourceManager->GetIcon(
            "necr32.icn"
        );
    }
    if (m_heroIcons[H2EnumIndex(HERO_TYPE_BOAT)] == NULL) {
        m_heroIcons[H2EnumIndex(HERO_TYPE_BOAT)] = gpResourceManager->GetIcon(
            "boat32.icn"
        );
    }
    if (m_heroIcons[HERO_ICON_FROTH] == NULL) {
        m_heroIcons[HERO_ICON_FROTH] = gpResourceManager->GetIcon(
            "froth.icn"
        );
    }

    gbLoadingMonoIcon = true;
    if (m_shadowIcon == NULL) {
        m_shadowIcon = gpResourceManager->GetIcon(
            "shadow32.icn"
        );
    }
    if (m_boatShadowIcon == NULL) {
        m_boatShadowIcon = gpResourceManager->GetIcon(
            "boatshad.icn"
        );
    }
    gbLoadingMonoIcon = false;

    if (m_flagIcons[PLAYER_COLOR_BLUE] == NULL) {
        m_flagIcons[PLAYER_COLOR_BLUE] = gpResourceManager->GetIcon(
            "b-flag32.icn"
        );
    }
    if (m_flagIcons[PLAYER_COLOR_GREEN] == NULL) {
        m_flagIcons[PLAYER_COLOR_GREEN] = gpResourceManager->GetIcon(
            "g-flag32.icn"
        );
    }
    if (m_flagIcons[PLAYER_COLOR_RED] == NULL) {
        m_flagIcons[PLAYER_COLOR_RED] = gpResourceManager->GetIcon(
            "r-flag32.icn"
        );
    }
    if (m_flagIcons[PLAYER_COLOR_YELLOW] == NULL) {
        m_flagIcons[PLAYER_COLOR_YELLOW] = gpResourceManager->GetIcon(
            "y-flag32.icn"
        );
    }
    if (m_flagIcons[PLAYER_COLOR_ORANGE] == NULL) {
        m_flagIcons[PLAYER_COLOR_ORANGE] = gpResourceManager->GetIcon(
            "o-flag32.icn"
        );
    }
    if (m_flagIcons[PLAYER_COLOR_PURPLE] == NULL) {
        m_flagIcons[PLAYER_COLOR_PURPLE] = gpResourceManager->GetIcon(
            "p-flag32.icn"
        );
    }
    if (m_boatFlagIcons[PLAYER_COLOR_BLUE] == NULL) {
        m_boatFlagIcons[PLAYER_COLOR_BLUE] = gpResourceManager->GetIcon(
            "b-bflg32.icn"
        );
    }
    if (m_boatFlagIcons[PLAYER_COLOR_GREEN] == NULL) {
        m_boatFlagIcons[PLAYER_COLOR_GREEN] = gpResourceManager->GetIcon(
            "g-bflg32.icn"
        );
    }
    if (m_boatFlagIcons[PLAYER_COLOR_RED] == NULL) {
        m_boatFlagIcons[PLAYER_COLOR_RED] = gpResourceManager->GetIcon(
            "r-bflg32.icn"
        );
    }
    if (m_boatFlagIcons[PLAYER_COLOR_YELLOW] == NULL) {
        m_boatFlagIcons[PLAYER_COLOR_YELLOW] = gpResourceManager->GetIcon(
            "y-bflg32.icn"
        );
    }
    if (m_boatFlagIcons[PLAYER_COLOR_ORANGE] == NULL) {
        m_boatFlagIcons[PLAYER_COLOR_ORANGE] = gpResourceManager->GetIcon(
            "o-bflg32.icn"
        );
    }
    if (m_boatFlagIcons[PLAYER_COLOR_PURPLE] == NULL) {
        m_boatFlagIcons[PLAYER_COLOR_PURPLE] = gpResourceManager->GetIcon(
            "p-bflg32.icn"
        );
    }

    gbLoadingMonoIcon = true;
    if (m_puzzleIcon == NULL) {
        m_puzzleIcon = gpResourceManager->GetIcon(
            "radar.icn"
        );
    }
    gbLoadingMonoIcon = false;

    for (i = 0; i < LOOPING_SAMPLE_COUNT; ++i) {
        m_loopingSamples[i] = NULL;
    }
    for (i = 0; i < SOUND_CELL_COUNT; ++i) {
        m_activeSounds[i].soundId = ADVMGR_ENVIRONMENT_SOUND_NONE;
        m_activeSounds[i].volume = ENVIRONMENT_SOUND_DEFAULT_VOLUME;
        m_activeSoundMask = 0;
    }

    GetCursorSampleSet(gConfig.walkSpeed);
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        gpGame->TurnOnAIMusic();
        platform::SetDialogMenusEnabled(0);
    } else {
        platform::SetDialogMenusEnabled(1);
    }

    glTimers[0] = platform::Ticks() + TIMER_DELAY;
    ConfigVolumeLevel savedVolume = gConfig.soundVolume;
    if (gConfig.soundVolume != CONFIG_VOLUME_MUTED) {
        gConfig.soundVolume = CONFIG_VOLUME_MAX;
    }
    SetInitialMapOrigin();

    bShowIt = gbThisNetHumanPlayer[giCurPlayer];
    i32 savedPlayer = giCurPlayer;
    i32 savedShowIt = bShowIt;
    giCurPlayer = giCurWatchPlayer;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    bShowIt = 1;
    RedrawAdvScreen(1, 0);
    giCurPlayer = savedPlayer;
    bShowIt = savedShowIt;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        gpGame->ShowComputerScreen();
    }
    platform::ChangeMenu(hmnuAdv);
    ForceNewHover();
    gpWindowManager->FadeScreen(FADE_IN, ADVENTURE_FADE_STEPS, gPalette);
    giBottomViewOverride = BOTTOM_VIEW_NONE;
    gConfig.soundVolume = savedVolume;
    gpSoundManager->AdjustSoundVolumes();
    m_messageMask = BASE_MANAGER_ACCEPT_ADVENTURE;
    m_priority = id;
    m_active = true;
    strcpy(m_name, "advManager");
    return 0;
}

void advManager::Close(void) {

    ClearBottomView();
    gpMouseManager->SetPointer(-1);
    if (!bEnteringTown || gConfig.useOpera != CONFIG_OPERA_DISABLED
        || gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI) {
        gpSoundManager->SwitchAmbientMusic(-1);
        gpSoundManager->StopAllSamples(1);
    } else {
        gpSoundManager->StopAllSamples(0);
    }
    if (m_adventureBorder != NULL) {
        H2_FREE(m_adventureBorder);
        m_adventureBorder = NULL;
    }

    i32 index;
    if (gAdvDisposeLevel <= ADV_DISPOSE_PARTIAL) {
        for (index = 0; index < OBJECT_ICON_COUNT; ++index) {
            if (m_objectIcons[index] != NULL) {
                gpResourceManager->Dispose(m_objectIcons[index]);
            }
            m_objectIcons[index] = NULL;
        }
    }
    if (gAdvDisposeLevel <= ADV_DISPOSE_NONE) {
        gpResourceManager->Dispose(m_puzzleIcon);
        m_puzzleIcon = NULL;
        gpResourceManager->Dispose(m_cloudOverlayIcon);
        m_cloudOverlayIcon = NULL;
        for (index = 0; index < HERO_ICON_COUNT; ++index) {
            gpResourceManager->Dispose(m_heroIcons[index]);
            m_heroIcons[index] = NULL;
        }
        gpResourceManager->Dispose(m_shadowIcon);
        m_shadowIcon = NULL;
        gpResourceManager->Dispose(m_boatShadowIcon);
        m_boatShadowIcon = NULL;
        for (index = 0; index < PLAYER_COLOR_COUNT; ++index) {
            gpResourceManager->Dispose(m_flagIcons[index]);
            m_flagIcons[index] = NULL;
            gpResourceManager->Dispose(m_boatFlagIcons[index]);
            m_boatFlagIcons[index] = NULL;
        }
        gpResourceManager->Dispose(m_groundTiles);
        m_groundTiles = NULL;
        gpResourceManager->Dispose(m_cloudTiles);
        m_cloudTiles = NULL;
        gpResourceManager->Dispose(m_stoneTiles);
        m_stoneTiles = NULL;
    }
    for (index = 0; index < LOOPING_SAMPLE_COUNT; ++index) {
        if (m_loopingSamples[index] != NULL) {
            gpResourceManager->Dispose(m_loopingSamples[index]);
        }
        m_loopingSamples[index] = NULL;
    }
    for (index = 0; index < CURSOR_SAMPLE_COUNT; ++index) {
        gpResourceManager->Dispose(m_cursorSamples[index]);
        m_cursorSamples[index] = NULL;
    }
    gpWindowManager->RemoveWindow(m_adventureWindow);
    delete m_adventureWindow;
    m_adventureWindow = NULL;
    if (m_visibilityMap != NULL) {
        delete m_visibilityMap;
    }
    m_visibilityMap = NULL;
    iCurBottomView = BOTTOM_VIEW_NONE;
    m_active = false;
}

void advManager::GetCursorSampleSet(ConfigWalkSpeed sampleSet) {
    CursorSampleSet cursorSampleSet =
        sampleSet >= CONFIG_WALK_SPEED_SLOW ? CURSOR_SAMPLE_FAST_SET : CURSOR_SAMPLE_SLOW_SET;
    i32 suffixSample[CURSOR_SAMPLE_COUNT] = {0, 3, 5, 3, 4, 5, 6, 3, 3};
    for (i32 index = 0; index < CURSOR_SAMPLE_COUNT; ++index) {
        sprintf(
            gText,
            "wsnd%1d%1d.82M",
            H2EnumIndex(cursorSampleSet),
            suffixSample[index]
        );
        m_cursorSamples[index] = gpResourceManager->GetSample(gText);
        m_cursorSamples[index]->m_playbackData.volume = CURSOR_SAMPLE_VOLUME;
        m_cursorSamples[index]->m_playbackData.channelType = CURSOR_SAMPLE_CHANNEL;
    }
}

class mapCell* advManager::DoAdvCommand(void) {
    mapCell* eventCellState = NULL;
    char bMoveStopped = 0;
    i32 oldMapValid;
    hero* selectedHero;
    town* viewTown;
    tag_message messageValue;
    i32 moveChanged;
    char newHover = 0;
    i32 pathIndex;
    i32 moveDone;
    if (gpCurPlayer->m_currentHero == INVALID_HERO) {
        selectedHero = NULL;
    } else {
        selectedHero = &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
    }

    switch (m_selectedCell) {
        case ADVMGR_COMMAND_MOVE_TO:
            if (selectedHero == NULL) {
                break;
            }
            selectedHero->m_destinationX = m_commandTargetX,
            selectedHero->m_destinationY = m_commandTargetY;
            goto continue_route;
        case ADVMGR_COMMAND_CONTINUE_ROUTE:
        continue_route:
            if (selectedHero == NULL) {
                break;
            }
            if (selectedHero->m_destinationX == HERO_DESTINATION_NONE
                || selectedHero->m_destinationY == HERO_DESTINATION_NONE) {
                break;
            }
            gpSearchArray->BuildPath(
                selectedHero->m_x,
                selectedHero->m_y,
                selectedHero->m_destinationX,
                selectedHero->m_destinationY,
                ROUTE_PATH_COST_LIMIT
            );
            if (gpSearchArray->m_pathLength > 0) {
                oldMapValid = m_visibilityMapValid;
                MobilizeCurrHero(1);
                if (gConfig.showRoute || oldMapValid) {
                    ShowRoute(0, 0, 0);
                } else {
                    if (m_visibilityMapValid && m_selectedCell != ADVMGR_COMMAND_CONTINUE_ROUTE) {
                        HideRoute(1, 0, 1);
                    }
                }
                gpMouseManager->HideColorPointer();
                gpInputManager->Flush();

                pathIndex = gpSearchArray->m_pathLength - 1;
                for (; pathIndex >= 0; --pathIndex) {
                    eventCellState = MoveHero(
                        static_cast<MapDirection>(
                            gpSearchArray->m_storage.path.directions[pathIndex + 1]
                        ),
                        pathIndex == 0,
                        &TrigX,
                        &TrigY,
                        &moveChanged,
                        0,
                        &moveDone,
                        0
                    );
                    UpdateHeroLocator(-1, 1, 1);
                    if (eventCellState != NULL) {
                        break;
                    }
                    if (moveChanged || moveDone || gbHitEvent) {
                        goto movement_done;
                    }
                    messageValue = gpInputManager->GetEvent();
                    while (messageValue.type != MESSAGE_NONE) {
                        if (messageValue.type == MESSAGE_KEY_DOWN
                            || messageValue.type == MESSAGE_LEFT_BUTTON_DOWN
                            || messageValue.type == MESSAGE_RIGHT_BUTTON_DOWN
                            || messageValue.type == MESSAGE_WIDGET) {
                            bMoveStopped = 1;
                            StopCursor(1);
                            goto movement_done;
                        }
                        platform::PumpEvents();
                        messageValue = gpInputManager->GetEvent();
                    }
                }

            movement_done:
                if ((pathIndex <= 0
                     && selectedHero->m_x == selectedHero->m_destinationX
                     && selectedHero->m_y == selectedHero->m_destinationY)
                    || (bMoveStopped && !gConfig.showRoute) || eventCellState != NULL) {
                    HideRoute(1, 1, 1);
                } else if (m_selectedCell == ADVMGR_COMMAND_CONTINUE_ROUTE || gConfig.showRoute) {
                    ShowRoute(0, 1, 1);
                }
                while (gpMouseManager->m_hideCount != 0) {
                    gpMouseManager->ShowColorPointer();
                }
                gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]);
                UpdBottomView(1, 1, 1);
                if (eventCellState != NULL) {
                    StopCursor(1);
                    DoEvent(eventCellState, TrigX, TrigY);
                    eventCellState = NULL;
                }
                Reseed(0, 0);
                newHover = 1;
                CheckDimHero();
            }
            break;

        case ADVMGR_COMMAND_OCCUPIED_TOWN_VIEW:
            DemobilizeCurrHero();
            gpMouseManager->SetPointer(0);
            viewTown = gpGame->GetTown(selectedHero->m_occupiedTown);
            viewTown->View(0);
            eventCellState = NULL;
            break;

        case ADVMGR_COMMAND_TOWN_VIEW:
            DemobilizeCurrHero();
            gpMouseManager->SetPointer(0);
            eventCellState = GetCell(
                gpGame->m_castleRecs[gpCurPlayer->m_currentTown].m_x,
                gpGame->m_castleRecs[gpCurPlayer->m_currentTown].m_y
            );
            gpGame->m_castleRecs[gpCurPlayer->m_currentTown].View(0);
            eventCellState = NULL;
            break;

        case ADVMGR_COMMAND_HERO_VIEW:
            if (selectedHero == NULL) {
                break;
            }
            gpMouseManager->SetPointer(0);
            TrimLoopingSounds(0);
            HeroView(gpCurPlayer->m_currentHero, 0, 0);
            RedrawAdvScreen(1, 0);
            gpWindowManager->FadeScreen(FADE_IN, ADVENTURE_FADE_STEPS, NULL);
            break;

        case ADVMGR_COMMAND_SELECT_HERO:
            SetHeroContext(
                GetCell(m_mapOriginX + m_lastHoverCell, m_mapOriginY + m_hoverCellY)
                    ->m_objectMetadata,
                0
            );
            break;

        case ADVMGR_COMMAND_SELECT_TOWN:
            SetTownContext(GetCell(m_mapOriginX + m_lastHoverCell, m_mapOriginY + m_hoverCellY)
                               ->m_objectMetadata);
            break;

        case ADVMGR_COMMAND_NONE:
            break;
    }

    m_selectedCell = ADVMGR_COMMAND_NONE;
    m_hoverCellY = CURSOR_INVALID_POSITION;
    m_lastHoverCell = CURSOR_INVALID_POSITION;
    if (newHover) {
        ForceNewHover();
    }
    return eventCellState;
}

void advManager::CheckSetEvilInterface(i32 redraw, i32 player) {
    i32 shouldChange;
    i32 translationIndex;
    i32 savedShowIt;
    tag_message message;

    if (player == -1) {
        player = giCurWatchPlayer;
    }

    shouldChange = 0;
    if (gConfig.evilInterfaceUsage == INTERFACE_EVIL && !gbUseEvilInterface) {
        shouldChange = 1;
    } else if (gConfig.evilInterfaceUsage == INTERFACE_GOOD && gbUseEvilInterface) {
        shouldChange = 1;
    } else if (gConfig.evilInterfaceUsage == INTERFACE_AUTO
               && gbUseEvilInterface != gpGame->m_players[player].m_evilInterface) {
        shouldChange = 1;
    }

    if (shouldChange) {
        gbUseEvilInterface = 1 - gbUseEvilInterface;
        if (redraw) {
            message.type = ADVMGR_INTERFACE_MESSAGE;
            message.payload.widget.command = ADVMGR_INTERFACE_REPLACE_RESOURCE;
            for (translationIndex = 0; translationIndex < INTERFACE_TRANSLATION_COUNT;
                 ++translationIndex) {
                message.payload.widget.id = gpResourceManager->MakeId(
                    cEvilTranslate[translationIndex][1 - gbUseEvilInterface],
                    0
                );
                message.payload.widget.data.value = gpResourceManager->MakeId(
                    cEvilTranslate[translationIndex][gbUseEvilInterface],
                    0
                );
                m_adventureWindow->BroadcastMessage(message);
            }
            savedShowIt = bShowIt;
            bShowIt = 1;
            RedrawAdvScreen(1, 1);
            bShowIt = savedShowIt;
        }
    }
}

MessageDispatchResult advManager::Main(struct tag_message& message) {
    MessageDispatchResult result;
    i32 quit;
    i32 c;
    i32 mx;
    i32 my;
    i32 town;
    mapCell* evtCell;
    MapDirection direction;
    hero* hero2;
    i32 helpId;
    hero* curHero;
    i32 bMoved;
    i32 bEnded;
    i32 curMusic;

    if (glTimers[0] < platform::Ticks() && ComboDraw(1)) {
        UpdateScreen(1, 0);
    }
    if (gbGameOver) {
        message.type = MESSAGE_EXECUTIVE;
        message.payload.executive.command = EXECUTIVE_COMMAND_TERMINATE_LOOP;
        return MESSAGE_DISPATCH_FORWARD;
    }

    CheckHandleNet();
    if (!gbThisNetGotAdventureControl) {
        return CheckHandleNetPlayerWait(message, 0);
    }
    if (!gbHumanPlayer[giCurPlayer]) {
        gpPhilAI->DoAI(giCurPlayer);
        gpGame->NextPlayer();
        return MESSAGE_DISPATCH_CONSUME;
    }
    if (giScreenScroll && gbForegroundApp) {
        CheckScreenScroll();
    }
    if (gConfig.musicVolume != CONFIG_VOLUME_MUTED && giForceSwitchMusic > 0
        && platform::Ticks() - giForceSwitchMusic > FORCED_MUSIC_DELAY) {
        giForceSwitchMusic = -1;
        curMusic = gpSoundManager->m_musicTrack;
        if (curMusic == WAIT_AMBIENT_MUSIC) {
            gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]);
        }
        SetEnvironmentOrigin(
            m_mapOriginX + VIEW_CENTER_OFFSET,
            m_mapOriginY + VIEW_CENTER_OFFSET,
            1
        );
    }

    result = MESSAGE_DISPATCH_CONSUME;
    quit = 0;
    evtCell = NULL;
    if (message.type != MESSAGE_NONE) {
        switch (message.type) {
            case MESSAGE_WIDGET:
                switch (message.payload.widget.command) {
                    case WIDGET_COMMAND_DESELECT:
                        if (!(
                                (H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))
                            )) {
                            result =
                                ProcessDeSelect(&message, &quit, &evtCell);
                        }
                        break;
                    case WIDGET_COMMAND_SELECT:
                    case WIDGET_COMMAND_ALTERNATE_SELECT:
                        if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                            helpId = -1;
                            switch (message.payload.widget.id) {
                                case PANEL_NEXT_HERO:
                                    helpId = 0;
                                    break;
                                case PANEL_CONTINUE_ROUTE:
                                    helpId = 1;
                                    break;
                                case PANEL_OVERVIEW:
                                    helpId = 2;
                                    break;
                                case PANEL_END_TURN:
                                    helpId = 3;
                                    break;
                                case PANEL_ADVENTURE_OPTIONS:
                                    helpId = 4;
                                    break;
                                case PANEL_CONTROL_OPTIONS:
                                    helpId = 5;
                                    break;
                                case PANEL_SYSTEM_OPTIONS:
                                    helpId = 6;
                                    break;
                                case PANEL_CAST_SPELL:
                                    helpId = 7;
                                    break;
                            }
                            if (helpId >= 0) {
                                NormalDialog(
                                    gAdvMenuHelp[helpId],
                                    NORMAL_DIALOG_QUICK_VIEW,
                                    -1,
                                    -1,
                                    -1,
                                    0,
                                    -1,
                                    0,
                                    -1,
                                    0
                                );
                                break;
                            }
                        }
                        result = ProcessSelect(&message, &evtCell);
                        break;
                }
                break;
            case MESSAGE_MOUSE_MOVE:
                result =
                    ProcessHover(message.payload.mouse.screenX, message.payload.mouse.screenY);
                break;
            case MESSAGE_KEY_DOWN:
                direction = MAP_DIRECTION_NONE;
                if (gpCurPlayer->CurrentHero() != INVALID_HERO) {
                    curHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
                } else {
                    curHero = NULL;
                }
                if (giDebugLevel < 1
                    && (message.payload.keyboard.keyCode == INPUT_SCAN_F3
                        || message.payload.keyboard.keyCode == INPUT_SCAN_F4
                        || message.payload.keyboard.keyCode == INPUT_SCAN_F5
                        || message.payload.keyboard.keyCode == INPUT_SCAN_F6
                        || message.payload.keyboard.keyCode == INPUT_SCAN_F7
                        || message.payload.keyboard.keyCode == INPUT_SCAN_F8
                        || message.payload.keyboard.keyCode == INPUT_SCAN_F9
                        || message.payload.keyboard.keyCode == INPUT_SCAN_F10
                        || message.payload.keyboard.keyCode == INPUT_SCAN_F11
                        || message.payload.keyboard.keyCode == INPUT_SCAN_F12)) {
                    break;
                }

                switch (message.payload.keyboard.keyCode) {
                    case INPUT_SCAN_F2:
                        CreateColorTables();
                        PopNetBox(NULL, -1);
                        break;
                    case INPUT_SCAN_F6:
                        gpGame->CheckHeroConsistency();
                        break;
                    case INPUT_SCAN_F7:
                        gpCurPlayer->m_cheatValue += 12;
                        if (curHero != NULL) {
                            GiveExperience(curHero, CHEAT_EXPERIENCE_AMOUNT, 1);
                        }
                        break;
                    case INPUT_SCAN_F8:
                        break;
                    case INPUT_SCAN_F9:
                        for (c = 0; c < H2EnumIndex(RES_COUNT); ++c) {
                            gpCurPlayer->m_resources[c] +=
                                (c == H2EnumIndex(RES_GOLD) ? CHEAT_GOLD_AMOUNT : CHEAT_RESOURCE_AMOUNT);
                        }
                        break;
                    case INPUT_SCAN_0:
                        c = 0;
                        goto process_cheat_digit;
                    case INPUT_SCAN_1:
                        c = 1;
                        goto process_cheat_digit;
                    case INPUT_SCAN_2:
                        c = 2;
                        goto process_cheat_digit;
                    case INPUT_SCAN_3:
                        c = 3;
                        goto process_cheat_digit;
                    case INPUT_SCAN_4:
                        c = 4;
                        goto process_cheat_digit;
                    case INPUT_SCAN_5:
                        c = 5;
                        goto process_cheat_digit;
                    case INPUT_SCAN_6:
                        c = 6;
                        goto process_cheat_digit;
                    case INPUT_SCAN_7:
                        c = 7;
                        goto process_cheat_digit;
                    case INPUT_SCAN_8:
                        c = 8;
                        goto process_cheat_digit;
                    case INPUT_SCAN_9:
                        c = 9;
                        goto process_cheat_digit;
                    process_cheat_digit:
                        hero2 = NULL;
                        if (gpCurPlayer->CurrentHero() != INVALID_HERO) {
                            hero2 = gpGame->GetHero(gpCurPlayer->m_currentHero);
                        }
                        giCheatSeq =
                            giCheatSeq * CHEAT_SEQUENCE_RADIX % CHEAT_SEQUENCE_MODULUS + c;
                        if (!gbRemoteOn) {
                            if (giCheatSeq % CHEAT_CREATURES_MODULUS == CHEAT_CREATURES
                                && hero2 != NULL) {
                                gpGame->m_cheated = 1;
                                if (gbInCampaign) {
                                    gpGame->m_campaignCheated = 1;
                                }
                                gpGame->GiveArmy(
                                    &hero2->m_army,
                                    CREATURE_BLACK_DRAGON,
                                    CHEAT_BLACK_DRAGON_COUNT,
                                    -1
                                );
                                UpdBottomView(1, 1, 1);
                            }
                            if (giCheatSeq % CHEAT_SHORT_MODULUS == CHEAT_WIN) {
                                gpGame->m_cheated = 1;
                                if (gbInCampaign) {
                                    gpGame->m_campaignCheated = 1;
                                }
                                CheckEndGame(END_GAME_FORCE_VICTORY, false);
                            }
                            if (giCheatSeq % CHEAT_LOSE_MODULUS == CHEAT_LOSE) {
                                gpGame->m_cheated = 1;
                                CheckEndGame(END_GAME_FORCE_DEFEAT, false);
                            }
                        }
                        if (giCheatSeq % CHEAT_SEQUENCE_MODULUS == CHEAT_REVEAL_MAP) {
                            gpGame->m_cheated = 1;
                            if (gbInCampaign) {
                                gpGame->m_campaignCheated = 1;
                            }
                            gpGame->SetVisibility(
                                CHEAT_REVEAL_CENTER,
                                CHEAT_REVEAL_CENTER,
                                PLAYER_COLOR_BLUE,
                                CHEAT_REVEAL_RADIUS
                            );
                            gpGame->SetVisibility(
                                CHEAT_REVEAL_CENTER,
                                CHEAT_REVEAL_CENTER,
                                PLAYER_COLOR_GREEN,
                                CHEAT_REVEAL_RADIUS
                            );
                            gpGame->SetVisibility(
                                CHEAT_REVEAL_CENTER,
                                CHEAT_REVEAL_CENTER,
                                PLAYER_COLOR_RED,
                                CHEAT_REVEAL_RADIUS
                            );
                            gpGame->SetVisibility(
                                CHEAT_REVEAL_CENTER,
                                CHEAT_REVEAL_CENTER,
                                PLAYER_COLOR_YELLOW,
                                CHEAT_REVEAL_RADIUS
                            );
                            gpGame->SetVisibility(
                                CHEAT_REVEAL_CENTER,
                                CHEAT_REVEAL_CENTER,
                                PLAYER_COLOR_ORANGE,
                                CHEAT_REVEAL_RADIUS
                            );
                            gpGame->SetVisibility(
                                CHEAT_REVEAL_CENTER,
                                CHEAT_REVEAL_CENTER,
                                PLAYER_COLOR_PURPLE,
                                CHEAT_REVEAL_RADIUS
                            );
                            Reseed(0, 0);
                            UpdateRadar(1, 0);
                            CompleteDraw(0);
                            UpdateScreen(0, 0);
                        }
                        if (giCheatSeq % CHEAT_SHORT_MODULUS == CHEAT_INFO) {
                            sprintf(
                                gText,
                                "Coordinates at top left corner of view:\n\n  X: %d\n  Y: %d",
                                m_mapOriginX,
                                m_mapOriginY
                            );
                            NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                        }
                        break;
                    case INPUT_SCAN_ESCAPE:
                        break;
                    case INPUT_SCAN_NUMPAD_8:
                        if ((H2EnumIndex((message.payload.keyboard.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))) {
                            ScreenScroll(MAP_DIRECTION_NORTH, 0);
                        } else {
                            direction = MAP_DIRECTION_NORTH;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_9:
                        if ((H2EnumIndex((message.payload.keyboard.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))) {
                            ScreenScroll(MAP_DIRECTION_NORTH_EAST, 0);
                        } else {
                            direction = MAP_DIRECTION_NORTH_EAST;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_6:
                        if ((H2EnumIndex((message.payload.keyboard.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))) {
                            ScreenScroll(MAP_DIRECTION_EAST, 0);
                        } else {
                            direction = MAP_DIRECTION_EAST;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_3:
                        if ((H2EnumIndex((message.payload.keyboard.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))) {
                            ScreenScroll(MAP_DIRECTION_SOUTH_EAST, 0);
                        } else {
                            direction = MAP_DIRECTION_SOUTH_EAST;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_2:
                        if ((H2EnumIndex((message.payload.keyboard.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))) {
                            ScreenScroll(MAP_DIRECTION_SOUTH, 0);
                        } else {
                            direction = MAP_DIRECTION_SOUTH;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_1:
                        if ((H2EnumIndex((message.payload.keyboard.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))) {
                            ScreenScroll(MAP_DIRECTION_SOUTH_WEST, 0);
                        } else {
                            direction = MAP_DIRECTION_SOUTH_WEST;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_4:
                        if ((H2EnumIndex((message.payload.keyboard.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))) {
                            ScreenScroll(MAP_DIRECTION_WEST, 0);
                        } else {
                            direction = MAP_DIRECTION_WEST;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_7:
                        if ((H2EnumIndex((message.payload.keyboard.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))) {
                            ScreenScroll(MAP_DIRECTION_NORTH_WEST, 0);
                        } else {
                            direction = MAP_DIRECTION_NORTH_WEST;
                        }
                        break;
                    case INPUT_SCAN_C:
                        CheckCastSpell();
                        break;
                    case INPUT_SCAN_D:
                        ProcessSearch(-1, -1);
                        break;
                    case INPUT_SCAN_P:
                        ViewPuzzle();
                        break;
                    case INPUT_SCAN_V:
                        ViewWorld(SPELL_VIEW_ALL, false, false);
                        break;
                    case INPUT_SCAN_N:
                        c = 'e';
                        strcpy(
                            gText,
                            "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee \xf5\xee\xf2\xe8\xf2\xe5 \xed\xe0\xf7\xe0\xf2\xfc \xf1\xed\xe0\xf7\xe0\xeb\xe0?  (\xdd\xf2\xe0 \xe8\xe3\xf0\xe0 \xe1\xf3\xe4\xe5\xf2 \xef\xee\xf2\xe5\xf0\xff\xed\xe0)"
                        );
                        goto confirm_game_command;
                    case INPUT_SCAN_L:
                        c = 'f';
                        strcpy(
                            gText,
                            "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee \xf5\xee\xf2\xe8\xf2\xe5 \xe7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc \xed\xee\xe2\xf3\xfe \xe8\xe3\xf0\xf3? (\xdd\xf2\xe0 \xe8\xe3\xf0\xe0 \xe1\xf3\xe4\xe5\xf2 \xef\xee\xf2\xe5\xf0\xff\xed\xe0)"
                        );
                        goto confirm_game_command;
                    case INPUT_SCAN_Q:
                        c = 'i';
                        strcpy(
                            gText,
                            "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee \xf5\xee\xf2\xe8\xf2\xe5 \xe2\xfb\xe9\xf2\xe8?"
                        );
                        goto confirm_game_command;
                    confirm_game_command:
                        quit = 1;
                        NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                        if (gpWindowManager->m_dialogResult == DIALOG_OK) {
                            quit = 0;
                        } else {
                            gGameCommand = c;
                        }
                        break;
                    case INPUT_SCAN_S:
                        SaveGame();
                        break;
                    case INPUT_SCAN_I:
                        if (gbInCampaign) {
                            SetEnvironmentOrigin(
                                ENVIRONMENT_ORIGIN_NONE,
                                ENVIRONMENT_ORIGIN_NONE,
                                1
                            );
                            gpGame->ShowCampaignInfo(1, 0);
                            SetEnvironmentOrigin(
                                m_mapOriginX + VIEW_CENTER_OFFSET,
                                m_mapOriginY + VIEW_CENTER_OFFSET,
                                1
                            );
                            RedrawAdvScreen(1, 0);
                            gpSoundManager->SwitchAmbientMusic(
                                giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]
                            );
                        } else if (xIsPlayingExpansionCampaign) {
                            SetEnvironmentOrigin(
                                ENVIRONMENT_ORIGIN_NONE,
                                ENVIRONMENT_ORIGIN_NONE,
                                1
                            );
                            xCampaign.ShowInfo(1, 0);
                            SetEnvironmentOrigin(
                                m_mapOriginX + VIEW_CENTER_OFFSET,
                                m_mapOriginY + VIEW_CENTER_OFFSET,
                                1
                            );
                            RedrawAdvScreen(1, 0);
                            gpSoundManager->SwitchAmbientMusic(
                                giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]
                            );
                        } else {
                            gpGame->ShowScenInfo();
                        }
                        break;
                    case INPUT_SCAN_T:
                        if (gpCurPlayer->m_townCount >= 0) {
                            if (gpCurPlayer->CurrentTown() == TOWN_ID_NONE) {
                                town = gpCurPlayer->m_townIds[0];
                            } else {
                                town = 0;
                                for (c = 0;
                                     c < gpCurPlayer->m_townCount;
                                     ++c) {
                                    if (gpCurPlayer->CurrentTown()
                                        == gpCurPlayer->m_townIds[c]) {
                                        if (c == gpCurPlayer->m_townCount - 1) {
                                            town = gpCurPlayer->m_townIds[0];
                                        } else {
                                            town =
                                                gpCurPlayer->m_townIds[c + 1];
                                        }
                                    }
                                }
                            }
                            SetTownContext(town);
                        }
                        break;
                    case INPUT_SCAN_H:
                        SetHeroContext(gpCurPlayer->NextHero(0), 0);
                        break;
                    case INPUT_SCAN_ENTER:
                        if (gpCurPlayer->CurrentTown() != TOWN_ID_NONE) {
                            m_selectedCell = ADVMGR_COMMAND_TOWN_VIEW;
                            DoAdvCommand();
                        } else {
                            if (gpCurPlayer->CurrentHero() != INVALID_HERO) {
                                m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                                DoAdvCommand();
                            }
                        }
                        break;
                }

                if (gpCurPlayer->m_currentHero != INVALID_HERO
                    && direction >= MAP_DIRECTION_NORTH) {
                    HideRoute(1, 1, 1);
                    gpMouseManager->HideColorPointer();
                    evtCell = MoveHero(
                        direction,
                        1,
                        &TrigX,
                        &TrigY,
                        &bMoved,
                        0,
                        &bEnded,
                        0
                    );
                    UpdateHeroLocator(-1, 1, 1);
                    while (gpMouseManager->m_hideCount != 0) {
                        gpMouseManager->ShowColorPointer();
                    }
                    gpSoundManager->SwitchAmbientMusic(
                        giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]
                    );
                    if (evtCell != NULL) {
                        StopCursor(1);
                        DoEvent(evtCell, TrigX, TrigY);
                        evtCell = NULL;
                    }
                    Reseed(0, 0);
                    ForceNewHover();
                    UpdBottomView(1, 1, 1);
                    CheckDimHero();
                }
                break;
        }
    }

finish_message:
    if (evtCell != NULL) {
        DoEvent(evtCell, TrigX, TrigY);
    }
    if (gbGameOver || quit == 1 || giMenuCommand != -1) {
        message.type = MESSAGE_EXECUTIVE;
        message.payload.executive.command = EXECUTIVE_COMMAND_TERMINATE_LOOP;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return result;
}

void advManager::Reseed(i32, i32) {
    giSeedingValid = 0;
}

MessageDispatchResult
advManager::ProcessSelect(struct tag_message* message, class mapCell** eventCell) {
    i32 iPage;
    i32 mouseX;
    i32 objectTypeState;
    i32 mouseY;
    i32 objectIdIndex;
    i32 visible;
    mapCell* theCell;
    tag_message radMsg;
    float fScale;
    tag_message msg;
    i32 mobileResult;
    hero* currentHero;

    visible = 1;
    mouseX = message->payload.mouse.screenX;
    mouseY = message->payload.mouse.screenY;

    switch (message->payload.widget.id) {
        case LOCATOR_HERO_SELECT_BASE:
        case LOCATOR_HERO_SELECT_1:
        case LOCATOR_HERO_SELECT_2:
        case LOCATOR_HERO_SELECT_3: {
            iPage = (message->payload.widget.id - LOCATOR_HERO_SELECT_BASE)
                        / LOCATOR_HERO_WIDGET_STRIDE;
            if (iPage >= gpCurPlayer->m_heroCount) {
                break;
            }
            objectTypeState = gpCurPlayer->m_heroIds[gpCurPlayer->m_heroLocatorPage + iPage];
            if ((H2EnumIndex((message->payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                HeroQuickView(objectTypeState, iPage, -1, -1);
            } else {
                if (objectTypeState == gpCurPlayer->CurrentHero()) {
                    m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                    DoAdvCommand();
                } else {
                    HideRoute(1, 0, 1);
                    SetHeroContext(objectTypeState, 0);
                }
            }
            break;
        }
        case LOCATOR_TOWN_IMAGE_BASE:
        case LOCATOR_TOWN_IMAGE_1:
        case LOCATOR_TOWN_IMAGE_2:
        case LOCATOR_TOWN_IMAGE_3: {
            objectTypeState = gpCurPlayer->m_townIds
                                  [gpCurPlayer->m_townLocatorPage + message->payload.widget.id
                                   - LOCATOR_TOWN_IMAGE_BASE];
            if ((H2EnumIndex((message->payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                TownQuickView(
                    objectTypeState,
                    message->payload.widget.id - LOCATOR_TOWN_IMAGE_BASE,
                    -1,
                    -1
                );
            } else {
                HideRoute(1, 0, 1);
                if (objectTypeState == gpCurPlayer->CurrentTown()) {
                    m_selectedCell = ADVMGR_COMMAND_TOWN_VIEW;
                    *eventCell = DoAdvCommand();
                } else {
                    SetTownContext(objectTypeState);
                }
            }
            break;
        }
        case LOCATOR_HERO_KNOB_CONTROL:
            DoHeroKnob();
            break;
        case LOCATOR_HERO_SCROLL_CONTROL: {
            mouseY -= LOCATOR_SCROLL_MOUSE_BASE_Y;
            if (gpCurPlayer->m_heroCount > LOCATOR_PAGE_STEP) {
                iPage = mouseY
                            / (LOCATOR_SCROLL_MOUSE_SPAN
                               / (gpCurPlayer->m_heroCount - (LOCATOR_VISIBLE_COUNT - 1)));
                if (iPage > gpCurPlayer->m_heroCount - LOCATOR_VISIBLE_COUNT) {
                    iPage = gpCurPlayer->m_heroCount - LOCATOR_VISIBLE_COUNT;
                }
            } else {
                iPage = 0;
            }
            gpCurPlayer->m_heroLocatorPage = iPage;
            UpdateHeroLocators(1, 1);
            break;
        }
        case LOCATOR_TOWN_KNOB_CONTROL:
            DoTownKnob();
            break;
        case LOCATOR_TOWN_SCROLL_CONTROL: {
            mouseY -= LOCATOR_SCROLL_MOUSE_BASE_Y;
            if (gpCurPlayer->m_townCount > LOCATOR_PAGE_STEP) {
                iPage = mouseY
                            / (LOCATOR_SCROLL_MOUSE_SPAN
                               / (gpCurPlayer->m_townCount - (LOCATOR_VISIBLE_COUNT - 1)));
                if (iPage > gpCurPlayer->m_townCount - LOCATOR_VISIBLE_COUNT) {
                    iPage = gpCurPlayer->m_townCount - LOCATOR_VISIBLE_COUNT;
                }
            } else {
                iPage = 0;
            }
            gpCurPlayer->m_townLocatorPage = iPage;
            UpdateTownLocators(1, 1);
            break;
        }
        case PANEL_MAP_VIEW: {
            if (!(*(mapExtra + (m_mapOriginX + m_lastHoverCell)
                    + MAP_WIDTH * (m_mapOriginY + m_hoverCellY))
                  & giCurPlayerBit)) {
                visible = 0;
            }
            theCell = GetCell(m_mapOriginX + m_lastHoverCell, m_mapOriginY + m_hoverCellY);
            if ((H2EnumIndex((message->payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                if (!visible) {
                    QuickInfo(m_lastHoverCell, m_hoverCellY);
                } else {
                    if (m_lastHoverCell == VIEW_CENTER_CELL && m_hoverCellY == VIEW_CENTER_CELL
                        && gpCurPlayer->CurrentHero() != INVALID_HERO && m_heroContextLocked) {
                        objectTypeState = H2EnumIndex(MAP_OBJECT_HERO_INTERACTION);
                        objectIdIndex = gpCurPlayer->CurrentHero();
                    } else {
                        objectTypeState = H2EnumIndex(theCell->m_triggerType & MAP_TRIGGER_TYPE_MASK);
                        objectIdIndex = theCell->m_objectMetadata;
                    }
                    switch (static_cast<MapObjectType>(objectTypeState)) {
                        case MAP_OBJECT_HERO_INTERACTION:
                            mouseX = m_lastHoverCell * CELL_PIXELS - HERO_QUICK_VIEW_X_OFFSET;
                            if (mouseX < QUICK_VIEW_MIN_X) {
                                mouseX = QUICK_VIEW_MIN_X;
                            }
                            if (mouseX + HERO_QUICK_VIEW_WIDTH > QUICK_VIEW_RIGHT) {
                                mouseX = HERO_QUICK_VIEW_RIGHT_X;
                            }
                            mouseY = m_hoverCellY * CELL_PIXELS - HERO_QUICK_VIEW_Y_OFFSET;
                            if (mouseY < QUICK_VIEW_MIN_Y) {
                                mouseY = QUICK_VIEW_MIN_Y;
                            }
                            if (mouseY + HERO_QUICK_VIEW_HEIGHT > QUICK_VIEW_BOTTOM) {
                                mouseY = HERO_QUICK_VIEW_BOTTOM_Y;
                            }
                            HeroQuickView(objectIdIndex, -1, mouseX, mouseY);
                            break;
                        case MAP_OBJECT_CASTLE:
                            mouseX = m_lastHoverCell * CELL_PIXELS - TOWN_QUICK_VIEW_X_OFFSET;
                            if (mouseX < QUICK_VIEW_MIN_X) {
                                mouseX = QUICK_VIEW_MIN_X;
                            }
                            if (mouseX + TOWN_QUICK_VIEW_WIDTH > QUICK_VIEW_RIGHT) {
                                mouseX = TOWN_QUICK_VIEW_RIGHT_X;
                            }
                            mouseY = m_hoverCellY * CELL_PIXELS - TOWN_QUICK_VIEW_Y_OFFSET;
                            if (mouseY < QUICK_VIEW_MIN_Y) {
                                mouseY = QUICK_VIEW_MIN_Y;
                            }
                            if (mouseY + TOWN_QUICK_VIEW_HEIGHT > QUICK_VIEW_BOTTOM) {
                                mouseY = TOWN_QUICK_VIEW_BOTTOM_Y;
                            }
                            TownQuickView(objectIdIndex, -1, mouseX, mouseY);
                            break;
                        default:
                            if (*(mapExtra + (m_mapOriginX + m_lastHoverCell)
                                  + MAP_WIDTH * (m_mapOriginY + m_hoverCellY))
                                & giCurPlayerBit) {
                                QuickInfo(m_lastHoverCell, m_hoverCellY);
                            }
                            break;
                    }
                }
            } else if (visible) {
                currentHero = NULL;
                mobileResult = 0;
                if (gpCurPlayer->m_currentHero != INVALID_HERO) {
                    currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
                    mobileResult = gpGame->IsMobile(currentHero->m_id);
                }
                if (currentHero != NULL) {
                    if (m_lastHoverCell == VIEW_CENTER_CELL && m_hoverCellY == VIEW_CENTER_CELL
                        && gpCurPlayer->CurrentHero() != INVALID_HERO && m_heroContextLocked) {
                        m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                        DoAdvCommand();
                    } else if ((!mobileResult
                                || (H2EnumIndex((message->payload.widget.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))
                                || (gConfig.showRoute
                                    && (currentHero->m_destinationX != m_commandTargetX
                                        || currentHero->m_destinationY != m_commandTargetY)))
                               && gpSearchArray->GetCell(m_commandTargetX, m_commandTargetY)
                                      .flags) {
                        currentHero->m_destinationX = m_commandTargetX;
                        currentHero->m_destinationY = m_commandTargetY;
                        ShowRoute(1, 1, 1);
                    } else {
                        *eventCell = DoAdvCommand();
                    }
                } else {
                    objectTypeState = H2EnumIndex(theCell->m_triggerType & MAP_TRIGGER_TYPE_MASK);
                    objectIdIndex = theCell->m_objectMetadata;
                    if (objectTypeState == H2EnumIndex(MAP_OBJECT_HERO_INTERACTION)) {
                        if (objectIdIndex == gpCurPlayer->CurrentHero()) {
                            m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                            DoAdvCommand();
                        } else if (gpGame->GetHero(objectIdIndex)->m_owner == giCurPlayer) {
                            SetHeroContext(objectIdIndex, 0);
                        }
                    }
                    if (objectTypeState == H2EnumIndex(MAP_OBJECT_CASTLE)) {
                        if (objectIdIndex == gpCurPlayer->CurrentTown()) {
                            m_selectedCell = ADVMGR_COMMAND_TOWN_VIEW;
                            *eventCell = DoAdvCommand();
                        } else if (gpGame->GetTown(objectIdIndex)->m_owner == giCurPlayer) {
                            SetTownContext(objectIdIndex);
                        }
                    }
                }
            }
            break;
        }
        case PANEL_RADAR:
            if ((H2EnumIndex((message->payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                NormalDialog(
                    "{\xca\xe0\xf0\xf2\xe0 \xec\xe8\xf0\xe0}\n\n"
                        "\xcc\xe8\xed\xe8\xea\xe0\xf0\xf2\xe0 \xe8\xe7\xf3\xf7\xe5\xed\xed\xee\xe3\xee \xec\xe8\xf0\xe0. \xd9\xe5\xeb\xf7\xea\xee\xec \xeb\xe5\xe2\xee\xe9 \xea\xed\xee\xef\xea\xe8 \xf3\xef\xf0\xe0\xe2\xeb\xff\xf2\xfc \xef\xf0\xee\xf1\xec\xee\xf2\xf0\xee\xec.",
                    NORMAL_DIALOG_QUICK_VIEW,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                break;
            }
            DemobilizeCurrHero();
            switch (MAP_HEIGHT) {
                case MAP_DIMENSION_SMALL:
                    fScale = 4.0f;
                    break;
                case MAP_DIMENSION_MEDIUM:
                    fScale = 2.0f;
                    break;
                case MAP_DIMENSION_LARGE:
                    fScale = 1.3333f;
                    break;
                default:
                    fScale = 1.0f;
                    break;
            }
            mouseX = static_cast<i32>((mouseX - RADAR_LEFT) / fScale);
            mouseY = static_cast<i32>((mouseY - RADAR_TOP) / fScale);
            m_mapOriginX = mouseX - VIEW_CENTER_CELL;
            m_mapOriginY = mouseY - VIEW_CENTER_CELL;
            if (m_mapOriginX < SCROLL_MIN_ORIGIN) {
                m_mapOriginX = SCROLL_MIN_ORIGIN;
            }
            if (m_mapOriginY < SCROLL_MIN_ORIGIN) {
                m_mapOriginY = SCROLL_MIN_ORIGIN;
            }
            if (m_mapOriginX > MAP_WIDTH - VIEW_EDGE_MARGIN) {
                m_mapOriginX = MAP_WIDTH - VIEW_EDGE_MARGIN;
            }
            if (m_mapOriginY > MAP_HEIGHT - VIEW_EDGE_MARGIN) {
                m_mapOriginY = MAP_HEIGHT - VIEW_EDGE_MARGIN;
            }
            UpdateRadar(1, 0);
            CompleteDraw(0);
            UpdateScreen(0, 0);

            msg.type = MESSAGE_NONE;
            while (msg.type != MESSAGE_LEFT_BUTTON_UP) {
                platform::PumpEvents();
                msg = gpInputManager->GetEvent();
                radMsg = msg;
                while (msg.type != MESSAGE_LEFT_BUTTON_UP
                       && msg.type != MESSAGE_NONE) {
                    if (msg.type == MESSAGE_MOUSE_MOVE) {
                        radMsg = msg;
                    }
                    platform::PumpEvents();
                    msg = gpInputManager->GetEvent();
                }
                if (radMsg.type == MESSAGE_MOUSE_MOVE) {
                    if (radMsg.payload.mouse.x < RADAR_LEFT) {
                        radMsg.payload.mouse.x = RADAR_LEFT;
                    }
                    if (radMsg.payload.mouse.x >= RADAR_RIGHT) {
                        radMsg.payload.mouse.x = RADAR_RIGHT - 1;
                    }
                    if (radMsg.payload.mouse.y < RADAR_TOP) {
                        radMsg.payload.mouse.y = RADAR_TOP;
                    }
                    if (radMsg.payload.mouse.y >= RADAR_BOTTOM) {
                        radMsg.payload.mouse.y = RADAR_BOTTOM - 1;
                    }
                    gpMouseManager->Main(radMsg);
                    mouseX =
                        static_cast<i32>((radMsg.payload.mouse.x - RADAR_LEFT) / fScale);
                    mouseY =
                        static_cast<i32>((radMsg.payload.mouse.y - RADAR_TOP) / fScale);
                    m_mapOriginX = mouseX - VIEW_CENTER_CELL;
                    m_mapOriginY = mouseY - VIEW_CENTER_CELL;
                    if (m_mapOriginX < SCROLL_MIN_ORIGIN) {
                        m_mapOriginX = SCROLL_MIN_ORIGIN;
                    }
                    if (m_mapOriginY < SCROLL_MIN_ORIGIN) {
                        m_mapOriginY = SCROLL_MIN_ORIGIN;
                    }
                    if (m_mapOriginX > MAP_WIDTH - VIEW_EDGE_MARGIN) {
                        m_mapOriginX = MAP_WIDTH - VIEW_EDGE_MARGIN;
                    }
                    if (m_mapOriginY > MAP_HEIGHT - VIEW_EDGE_MARGIN) {
                        m_mapOriginY = MAP_HEIGHT - VIEW_EDGE_MARGIN;
                    }
                    UpdateRadar(1, 0);
                    CompleteDraw(0);
                    UpdateScreen(0, 0);
                    radMsg.type = MESSAGE_NONE;
                }
            }
            break;
    }

    if (((H2EnumIndex((message->payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON))))
        && message->payload.widget.id >= BOTTOM_VIEW_FIRST_MESSAGE
        && message->payload.widget.id <= BOTTOM_VIEW_LAST_MESSAGE) {
        NormalDialog(
            "{\xce\xea\xed\xee \xf1\xf2\xe0\xf2\xf3\xf1\xe0}\n\n"
                "\xdd\xf2\xee \xee\xea\xed\xee \xf1\xee\xee\xe1\xf9\xe0\xe5\xf2 \xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xfe \xee \xf1\xee\xf1\xf2\xee\xff\xed\xe8\xe8 \xe2\xe0\xf8\xe5\xe3\xee \xe3\xe5\xf0\xee\xff \xe8\xeb\xe8 \xea\xee\xf0\xee\xeb\xe5\xe2\xf1\xf2\xe2\xe0, \xe0 \xf2\xe0\xea\xe6\xe5, \xef\xee\xea\xe0\xe7\xfb\xe2\xe0\xe5\xf2 \xea\xe0\xeb\xe5\xed\xe4\xe0\xf0\xfc. "
                "\xc8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xff \xe2 \xee\xea\xed\xe5 \xec\xe5\xed\xff\xe5\xf2\xf1\xff \xef\xf0\xe8 \xf6\xe8\xea\xeb\xe8\xf7\xed\xee\xec \xed\xe0\xe6\xe0\xf2\xe8\xe8 \xeb\xe5\xe2\xee\xe9 \xea\xed\xee\xef\xea\xe8 \xec\xfb\xf8\xe8 \xed\xe0 \xed\xe5\xec.",
            NORMAL_DIALOG_QUICK_VIEW,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
    }
    return MESSAGE_DISPATCH_CONSUME;
}

MessageDispatchResult
advManager::ProcessDeSelect(struct tag_message* message, i32* result, class mapCell** eventCell) {
    switch (message->payload.widget.id) {
        case PANEL_TOWN_PAGE_PREVIOUS:
            if (gpCurPlayer->m_townLocatorPage > 0) {
                --gpCurPlayer->m_townLocatorPage;
                UpdateTownLocators(1, 1);
            }
            break;
        case PANEL_TOWN_PAGE_NEXT:
            if (gpCurPlayer->m_townLocatorPage + LOCATOR_PAGE_STEP < gpCurPlayer->m_townCount) {
                ++gpCurPlayer->m_townLocatorPage;
                UpdateTownLocators(1, 1);
            }
            break;
        case PANEL_HERO_PAGE_PREVIOUS:
            if (gpCurPlayer->m_heroLocatorPage > 0) {
                --gpCurPlayer->m_heroLocatorPage;
                UpdateHeroLocators(1, 1);
            }
            break;
        case PANEL_HERO_PAGE_NEXT:
            if (gpCurPlayer->m_heroLocatorPage + LOCATOR_PAGE_STEP < gpCurPlayer->m_heroCount) {
                ++gpCurPlayer->m_heroLocatorPage;
                UpdateHeroLocators(1, 1);
            }
            break;
        case PANEL_CONTINUE_ROUTE:
            m_selectedCell = ADVMGR_COMMAND_CONTINUE_ROUTE;
            *eventCell = DoAdvCommand();
            break;
        case PANEL_ADVENTURE_OPTIONS:
            AdvPanel();
            break;
        case PANEL_CONTROL_OPTIONS:
            *result = ControlPanel();
            break;
        case PANEL_END_TURN:
            if (gpCurPlayer->HasMobileHero()) {
                NormalDialog(
                    "\xce\xe4\xe8\xed \xe8\xeb\xe8 \xed\xe5\xf1\xea\xee\xeb\xfc\xea\xee \xe3\xe5\xf0\xee\xe5\xe2 \xe2\xf1\xe5 \xe5\xf9\xe5 \xec\xee\xe3\xf3\xf2 \xf5\xee\xe4\xe8\xf2\xfc. \xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee \xf5\xee\xf2\xe8\xf2\xe5 \xe7\xe0\xea\xee\xed\xf7\xe8\xf2\xfc \xf5\xee\xe4?",
                    NORMAL_DIALOG_CONFIRM,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                if (gpWindowManager->m_dialogResult == DIALOG_OK) {
                    break;
                }
            }
            gpGame->NextPlayer();
            break;
        case PANEL_NEXT_HERO:
            HideRoute(1, 0, 1);
            SetHeroContext(gpCurPlayer->NextHero(1), 0);
            break;
        case PANEL_OVERVIEW: {
            TrimLoopingSounds(0);
            gpGame->Overview();
            i32 fadeAfter = 1;
            if (giOverviewReturnAction == OVERVIEW_RETURN_HERO) {
                gpMouseManager->SetPointer(0);
                TrimLoopingSounds(0);
                HeroView(
                    giOverviewReturnActionExtra,
                    reinterpret_cast<i32>(
                        gpGame->GetHero(H2EnumIndex(giOverviewReturnAction))->GetOccupiedTown()
                    ),
                    1
                );
            } else if (giOverviewReturnAction == OVERVIEW_RETURN_TOWN) {
                DemobilizeCurrHero();
                gpMouseManager->SetPointer(0);
                gpGame->GetTown(giOverviewReturnActionExtra)->View(1);
                fadeAfter = 0;
            }
            RedrawAdvScreen(1, 0);
            if (fadeAfter) {
                gpWindowManager->FadeScreen(FADE_IN, ADVENTURE_FADE_STEPS, NULL);
            }
            break;
        }
        case PANEL_CAST_SPELL:
            CheckCastSpell();
            break;
        case PANEL_SYSTEM_OPTIONS:
            SystemOptions();
            break;
    }

    if (message->payload.widget.id >= BOTTOM_VIEW_FIRST_MESSAGE
        && message->payload.widget.id <= BOTTOM_VIEW_LAST_MESSAGE) {
        if (giBottomViewOverride == BOTTOM_VIEW_KINGDOM) {
            giBottomViewOverride = BOTTOM_VIEW_NEW_TURN;
        } else if (giBottomViewOverride != BOTTOM_VIEW_NONE) {
            giBottomViewOverride = BOTTOM_VIEW_NONE;
        } else if (iCurBottomView == BOTTOM_VIEW_KINGDOM) {
            giBottomViewOverride = BOTTOM_VIEW_NEW_TURN;
        } else {
            giBottomViewOverride = BOTTOM_VIEW_KINGDOM;
        }
        giBottomViewOverrideEndTime = platform::Ticks() + BOTTOM_VIEW_DURATION;
        UpdBottomView(1, 1, 1);
    }
    return MESSAGE_DISPATCH_CONSUME;
}

inline hero* GetHeroSlot(i32 i) {
    return &gpGame->m_heroRecs[i];
}

i32 advManager::ProcessSearch(i32 x, i32 y) {
    SAMPLE2 sample;
    hero* hero;
    i32 pl;
    mapCell* cellPtr;
    tag_message evt;
    char special;
    i32 gaveArtifact;

    sample = NULL;
    hero = GetHeroSlot(gpCurPlayer->m_currentHero);

    if (hero->m_remainingMobility != hero->m_mobility) {
        if (!gbHumanPlayer[giCurPlayer]) {
            goto search_end;
        }
        NormalDialog(
            "\xd0\xe0\xf1\xea\xee\xef\xea\xe8 \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\xe0 \xe7\xe0\xed\xe8\xec\xe0\xfe\xf2 \xf6\xe5\xeb\xfb\xe9 \xe4\xe5\xed\xfc. \xcf\xee\xef\xf0\xee\xe1\xf3\xe9\xf2\xe5 \xf1\xed\xee\xe2\xe0, \xe7\xe0\xe2\xf2\xf0\xe0."
                 ,
            1,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
        return 1;
    }
    if (hero->NumArtifacts() == ARTIFACT_CAPACITY) {
        if (!gbHumanPlayer[giCurPlayer]) {
            goto search_end;
        }
        NormalDialog(
            "\xcf\xee\xe8\xf1\xea\xe8 \xcc\xee\xe3\xf3\xf9\xe5\xf1\xf2\xe2\xe5\xed\xed\xee\xe3\xee \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\xe0 \xe1\xf3\xe4\xf3\xf2 \xe1\xe5\xf1\xef\xee\xeb\xe5\xe7\xed\xfb\xec\xe8. \xc5\xf1\xeb\xe8 \xe4\xe0\xe6\xe5 \xe2\xe0\xf8 \xe3\xe5\xf0\xee\xe9 \xe8 \xed\xe0\xe9\xe4\xe5\xf2 \xe5\xe3\xee, \xf2\xee \xf3 \xed\xe5\xe3\xee \xef\xee\xef\xf0\xee\xf1\xf2\xf3 \xed\xe5\xf2 \xec\xe5\xf1\xf2\xe0, \xf7\xf2\xee\xe1\xfb \xf3\xed\xe5\xf1\xf2\xe8 \xe5\xe3\xee. \xc2\xe5\xf1\xfc \xe8\xed\xe2\xe5\xed\xf2\xe0\xf0\xfc \xe3\xe5\xf0\xee\xff \xe7\xe0\xed\xff\xf2.",
            1,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
        return 1;
    }

    MobilizeCurrHero(0);
    CompleteDraw(0);
    UpdateScreen(0, 0);
    if (x == -1) {
        x = m_mapOriginX + VIEW_CENTER_OFFSET;
        y = m_mapOriginY + VIEW_CENTER_OFFSET;
    }
    cellPtr = GetCell(x, y);
    if (!((cellPtr->m_objectIndex == MAPCELL_SPRITE_NONE
           || cellPtr->m_objectTileset == TILESET_DUMMY)
          && cellPtr->m_overlayIndex == MAPCELL_SPRITE_NONE)) {
        if (!gbHumanPlayer[giCurPlayer]) {
            goto search_end;
        }
        NormalDialog(
            "\xcf\xee\xef\xf0\xee\xe1\xf3\xe9\xf2\xe5 \xe8\xf1\xea\xe0\xf2\xfc \xed\xe0 \xf7\xe8\xf1\xf2\xee\xe9 \xe7\xe5\xec\xeb\xe5.",
            1,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
        return 1;
    }
    if (giGroundToTerrain[cellPtr->m_terrainImageIndex] == TERRAIN_WATER) {
        if (!gbHumanPlayer[giCurPlayer]) {
            goto search_end;
        }
        NormalDialog(
            "\xcf\xee\xef\xfb\xf2\xe0\xe9\xf2\xe5\xf1\xfc \xe8\xf1\xea\xe0\xf2\xfc \xed\xe0 \xf1\xf3\xf8\xe5!!!",
            1,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
        return 1;
    }

    if (gbHumanPlayer[giCurPlayer]) {
        sample = LoadPlaySample(
            "DIGSOUND.82M"
        );
    }
    if (cellPtr->m_objectIndex == MAPCELL_SPRITE_NONE
        || cellPtr->m_objectTileset == TILESET_DUMMY) {
        cellPtr->m_objectTileset = TILESET_OBJNDIRT;
        cellPtr->m_objectIndex = DIG_HOLE_FRAME;
        cellPtr->m_objectLayerBit0 = 1;
        cellPtr->m_objectLayerBit1 = 1;
        cellPtr->m_flags |= H2EnumIndex(MAP_CELL_OBJECT_SHADOW_ONLY);
    }
    CompleteDraw(0);
    UpdateScreen(0, 0);

    if (gpGame->m_ultimateArtifactX == x && gpGame->m_ultimateArtifactY == y
        && gpGame->m_ultimateArtifactId != ARTIFACT_NONE) {
        if (hero->NumArtifacts() >= ARTIFACT_CAPACITY) {
            if (gbHumanPlayer[giCurPlayer]) {
                NormalDialog(
                    "\xd3 \xe2\xe0\xf1 \xed\xe5\xf2 \xec\xe5\xf1\xf2\xe0 \xe4\xeb\xff \xe5\xf9\xe5 \xee\xe4\xed\xee\xe3\xee \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\xe0!",
                    1,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            }
        } else {
            special = 0;
            if (xIsPlayingExpansionCampaign) {
                special = xCampaign.IsSpecialUA();
            }
            if (gbHumanPlayer[giCurPlayer]) {
                gpSoundManager->SwitchAmbientMusic(ULTIMATE_ARTIFACT_MUSIC);
                if (special) {
                    sprintf(
                        gText,
                        "%s%s",
                        "\xcf\xee\xe7\xe4\xf0\xe0\xe2\xeb\xff\xe5\xec! \xcf\xf0\xee\xe2\xe5\xe4\xff \xec\xed\xee\xe3\xe8\xe5 \xf7\xe0\xf1\xfb \xe2 \xf0\xe0\xf1\xea\xee\xef\xea\xe0\xf5, \xe2\xfb \xf0\xe0\xe7\xfb\xf1\xea\xe0\xeb\xe8 \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2: ",
                        gArtifactNames[H2EnumIndex(ARTIFACT_SPHERE_NEGATION)]
                    );
                } else {
                    sprintf(
                        gText,
                        "%s%s",
                        "\xcf\xee\xe7\xe4\xf0\xe0\xe2\xeb\xff\xe5\xec! \xcf\xf0\xee\xe2\xe5\xe4\xff \xec\xed\xee\xe3\xe8\xe5 \xf7\xe0\xf1\xfb \xe2 \xf0\xe0\xf1\xea\xee\xef\xea\xe0\xf5, \xe2\xfb \xf0\xe0\xe7\xfb\xf1\xea\xe0\xeb\xe8 \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2: ",
                        gArtifactNames[H2EnumIndex(gpGame->m_ultimateArtifactId)]
                    );
                }
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                if (special) {
                    hero->ViewArtifact(ARTIFACT_SPHERE_NEGATION, 0, -1);
                } else {
                    hero->ViewArtifact(gpGame->m_ultimateArtifactId, 0, -1);
                }
                gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]);
            }
            if (special) {
                GiveArtifact(hero, ARTIFACT_SPHERE_NEGATION, 1, -1);
            } else {
                gaveArtifact =
                    GiveArtifact(hero, gpGame->m_ultimateArtifactId, 1, -1);
            }
            gpGame->m_ultimateArtifactId = ARTIFACT_NONE;
        }
    } else if (gbHumanPlayer[giCurPlayer]) {
        NormalDialog(
            "\xc7\xe4\xe5\xf1\xfc \xed\xe8\xf7\xe5\xe3\xee.\n\xc3\xe4\xe5 \xe1\xfb \xfd\xf2\xee \xec\xee\xe3\xeb\xee \xe1\xfb\xf2\xfc?",
            1,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
    }
    if (gbHumanPlayer[giCurPlayer]) {
        WaitEndSample(&sample, -1);
    }
    for (pl = 0; pl < gpGame->m_playerCount; ++pl) {
        ComputeUALoc(pl);
    }
    hero->m_remainingMobility = 0;
    UpdBottomView(1, 1, 1);
    CheckDimHero();
    Reseed(0, 0);
    CheckEndGame(END_GAME_FORCE_NONE, false);
    return 1;
search_end:
    gpCurPlayer->m_ultimateArtifactHintChance = 0;
    return 1;
}

MessageDispatchResult advManager::ProcessHover(i32 mouseX, i32 mouseY) {
    i32 heroPosX;
    i32 heroPosY;
    i32 mouseCellX;
    mapCell* hoverCell;
    town* pTown;
    i32 mouseCellY;
    hero* pHero;
    i32 daysLeft;
    i32 cursorBase;

    if (InMapArea(mouseX, mouseY)) {
        if (mouseX > DRAW_CLIP_WIDTH) {
            gpMouseManager->SetPointer(POINTER_DEFAULT);
            return MESSAGE_DISPATCH_CONSUME;
        }

        mouseCellX = mouseX / CELL_PIXELS;
        mouseCellY = mouseY / CELL_PIXELS;
        if (mouseCellX < 0) {
            mouseCellX = 0;
        }
        if (mouseCellY < 0) {
            mouseCellY = 0;
        }
        if (mouseCellX > HOVER_MAX_CELL) {
            mouseCellX = HOVER_MAX_CELL;
        }
        if (mouseCellY > HOVER_MAX_CELL) {
            mouseCellY = HOVER_MAX_CELL;
        }

        if (m_lastHoverCell != mouseCellX || m_hoverCellY != mouseCellY) {

            m_selectedCell = ADVMGR_COMMAND_NONE;
            m_lastHoverCell = mouseCellX;
            m_hoverCellY = mouseCellY;
            m_commandTargetX = m_mapOriginX + mouseCellX;
            m_commandTargetY = m_mapOriginY + mouseCellY;

            if (m_commandTargetX < 0 || m_commandTargetY < 0 || m_commandTargetX > MAP_WIDTH - 1
                || m_commandTargetY > MAP_HEIGHT - 1
                || !(MAP_EXTRA_AT_WFIRST(m_commandTargetX, m_commandTargetY)
                     & giCurPlayerBit)) {
                gpMouseManager->SetPointer(POINTER_DEFAULT);
                return MESSAGE_DISPATCH_CONSUME;
            }

            hoverCell = GetCell(m_commandTargetX, m_commandTargetY);
            if (gpCurPlayer->m_currentHero == INVALID_HERO) {
                if ((hoverCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_CASTLE
                    && gpGame->GetTown(hoverCell->m_objectMetadata)->m_owner == giCurPlayer) {
                    gpMouseManager->SetPointer(POINTER_TOWN);
                    m_selectedCell = ADVMGR_COMMAND_TOWN_VIEW;
                    return MESSAGE_DISPATCH_CONSUME;
                } else {
                    if ((hoverCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                            == MAP_OBJECT_HERO_INTERACTION
                        && gpGame->GetHero(hoverCell->m_objectMetadata)->m_owner
                               == giCurPlayer) {
                        gpMouseManager->SetPointer(POINTER_HERO);
                        m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                        return MESSAGE_DISPATCH_CONSUME;
                    } else {
                        gpMouseManager->SetPointer(POINTER_DEFAULT);
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                }
            } else {
                pHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
                heroPosX = pHero->m_x - m_mapOriginX;
                heroPosY = pHero->m_y - m_mapOriginY;
                if (mouseCellX == heroPosX && mouseCellY == heroPosY) {
                    gpMouseManager->SetPointer(POINTER_HERO);
                    m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                    return MESSAGE_DISPATCH_CONSUME;
                }

                if (hoverCell->m_flags & HOVER_OBJECT_BLOCKED) {
                    if ((hoverCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                        == MAP_OBJECT_CASTLE) {
                        pTown = gpGame->GetTown(hoverCell->m_objectMetadata);
                        if (pTown->m_owner == giCurPlayer) {
                            gpMouseManager->SetPointer(POINTER_TOWN);
                            m_selectedCell = ADVMGR_COMMAND_SELECT_TOWN;
                            return MESSAGE_DISPATCH_CONSUME;
                        }
                    }
                    gpSearchArray->m_pathLength = 0;
                    gpMouseManager->SetPointer(POINTER_DEFAULT);
                    return MESSAGE_DISPATCH_CONSUME;
                }

                if (!((m_cursorType == HERO_TYPE_BOAT
                       || giGroundToTerrain[hoverCell->m_terrainImageIndex] != TERRAIN_WATER
                       || hoverCell->m_triggerType
                              == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                       || hoverCell->m_triggerType
                              == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT)
                       || hoverCell->m_triggerType
                              == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK))
                      && (m_cursorType != HERO_TYPE_BOAT
                          || giGroundToTerrain[hoverCell->m_terrainImageIndex] == TERRAIN_WATER
                          || hoverCell->m_triggerType == MAP_OBJECT_COAST))) {
                    gpSearchArray->m_pathLength = 0;
                    gpMouseManager->SetPointer(POINTER_DEFAULT);
                    return MESSAGE_DISPATCH_CONSUME;
                }
                SeedTo(m_commandTargetX, m_commandTargetY);
                if (gpSearchArray->GetCell(m_commandTargetX, m_commandTargetY).flags) {
                    if (gpSearchArray->GetCell(m_commandTargetX, m_commandTargetY).previous
                        <= pHero->m_remainingMobility) {
                        daysLeft = 0;
                    } else {
                        daysLeft =
                            (gpSearchArray->GetCell(m_commandTargetX, m_commandTargetY).previous
                             - pHero->m_remainingMobility)
                                / pHero->m_mobility
                            + 1;
                        if (daysLeft > HOVER_ROUTE_DAY_LIMIT) {
                            daysLeft = HOVER_ROUTE_DAY_LIMIT;
                        }
                    }
                    cursorBase = daysLeft * HOVER_ROUTE_FRAMES_PER_DAY;

                    switch (hoverCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
                        case MAP_OBJECT_BOAT:
                            if (m_cursorType != HERO_TYPE_BOAT) {
                                gpMouseManager->SetPointer(cursorBase + POINTER_SAIL);
                                m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            } else {
                                gpMouseManager->SetPointer(cursorBase);
                            }
                            break;
                        case MAP_OBJECT_COAST:
                            if (m_cursorType == HERO_TYPE_BOAT) {
                                gpMouseManager->SetPointer(cursorBase + POINTER_DISEMBARK);
                            } else if (*(mapExtra + m_commandTargetX
                                         + m_commandTargetY * MAP_WIDTH)
                                       & HOVER_UNREACHABLE) {
                                gpMouseManager->SetPointer(cursorBase + POINTER_ATTACK);
                            } else {
                                gpMouseManager->SetPointer(cursorBase + POINTER_MOVE);
                            }
                            m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            break;
                        case MAP_OBJECT_MONSTER:
                            gpMouseManager->SetPointer(cursorBase + POINTER_ATTACK);
                            m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            break;
                        case MAP_OBJECT_HERO_INTERACTION:
                            if (gpGame->GetHero(hoverCell->m_objectMetadata)->m_owner
                                != giCurPlayer) {
                                gpMouseManager->SetPointer(cursorBase + POINTER_ATTACK);
                                m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            } else {
                                gpMouseManager->SetPointer(cursorBase + POINTER_SELECT_HERO);
                                m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            }
                            break;
                        case MAP_OBJECT_CASTLE:
                            pTown = gpGame->GetTown(hoverCell->m_objectMetadata);
                            if ((H2EnumIndex((hoverCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))
                                && pTown->m_owner != giCurPlayer
                                && pTown->HasGarrison()) {
                                gpMouseManager->SetPointer(cursorBase + POINTER_ATTACK);
                                m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                                break;
                            }
                            goto process_default_hover;
                        default:
                        process_default_hover:
                            if ((*(mapExtra + m_commandTargetX
                                   + m_commandTargetY * MAP_WIDTH)
                                 & HOVER_UNREACHABLE)
                                && m_cursorType != HERO_TYPE_BOAT
                                && (!(hoverCell->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                                    || !StopOnTrigger(hoverCell))) {
                                gpMouseManager->SetPointer(cursorBase + POINTER_ATTACK);
                            } else {
                                if ((H2EnumIndex((hoverCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                                    if (m_cursorType != HERO_TYPE_BOAT) {
                                        if (giGroundToTerrain[hoverCell->m_terrainImageIndex]
                                            != TERRAIN_WATER) {
                                            gpMouseManager->SetPointer(
                                                cursorBase + POINTER_ACTION
                                            );
                                        } else if (hoverCell->m_triggerType
                                                   == (MAP_TRIGGER_ACTION_FLAG
                                                       | MAP_OBJECT_SHIPWRECK)) {
                                            gpMouseManager->SetPointer(
                                                cursorBase + POINTER_ACTION
                                            );
                                        } else {
                                            gpMouseManager->SetPointer(
                                                cursorBase + POINTER_MOVE
                                            );
                                            break;
                                        }
                                    } else {
                                        if (giGroundToTerrain[hoverCell->m_terrainImageIndex]
                                            == TERRAIN_WATER) {
                                            gpMouseManager->SetPointer(
                                                daysLeft + POINTER_WATER_ACTION
                                            );
                                        } else {
                                            gpMouseManager->SetPointer(
                                                cursorBase + POINTER_SAIL
                                            );
                                        }
                                    }
                                } else {
                                    if (m_cursorType == HERO_TYPE_BOAT) {
                                        gpMouseManager->SetPointer(
                                            cursorBase + POINTER_SAIL
                                        );
                                    } else {
                                        gpMouseManager->SetPointer(
                                            cursorBase + POINTER_MOVE
                                        );
                                    }
                                }
                            }
                            m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            break;
                    }
                    return MESSAGE_DISPATCH_CONSUME;
                } else {
                    gpMouseManager->SetPointer(POINTER_DEFAULT);
                    return MESSAGE_DISPATCH_CONSUME;
                }
            }
        }
    } else {
        if (!(gpMouseManager->m_cursorFrame >= HOVER_SCROLL_FRAME_FIRST
              && gpMouseManager->m_cursorFrame < HOVER_SCROLL_FRAME_END && MouseInScrollZone())) {
            gpMouseManager->SetPointer(POINTER_DEFAULT);
        }
    }
    return MESSAGE_DISPATCH_CONSUME;
}

void advManager::UpdateScreen(i32, i32 forceUpdate) {
    if (forceUpdate == 0 && bShowIt == 0) {
        if (glTimers[0] < platform::Ticks()) {
            glTimers[0] = platform::Ticks() + TIMER_DELAY;
        }
        return;
    }
    PollSound();
    giScrollX = m_updateMinX;
    giScrollY = m_updateMinY;
    if (giLimitUpdMinX == UPDATE_NONE) {
        BlitBitmapToScreen(
            gpWindowManager->m_screen,
            UPDATE_VIEWPORT_ORIGIN,
            UPDATE_VIEWPORT_ORIGIN,
            UPDATE_VIEWPORT_SIZE,
            UPDATE_VIEWPORT_SIZE,
            UPDATE_VIEWPORT_ORIGIN,
            UPDATE_VIEWPORT_ORIGIN
        );
    } else {
        BlitBitmapToScreen(
            gpWindowManager->m_screen,
            giLimitUpdMinX,
            giLimitUpdMinY,
            giLimitUpdMaxX - giLimitUpdMinX,
            giLimitUpdMaxY - giLimitUpdMinY,
            giLimitUpdMinX,
            giLimitUpdMinY
        );
    }
    giScrollY = 0;
    giScrollX = giScrollY;
    PollSound();

    if (glTimers[0] < platform::Ticks()) {
        ++m_updateMaxY;
        ++m_updateMaxX;
        if (m_updateMaxX >= UPDATE_ANIMATION_PHASES) {
            m_updateMaxX = 0;
        }
        glTimers[0] = platform::Ticks() + TIMER_DELAY;

        if (m_updateMaxX == 1 || m_updateMaxX == 3 || m_updateMaxX == 5) {
            ++m_animationPhases[ANIMATION_PHASE_COLUMN_1];
            m_animationPhases[ANIMATION_PHASE_COLUMN_1] %= UPDATE_FRAME_CYCLE;
            ++m_animationPhases[ANIMATION_PHASE_COLUMN_3];
            m_animationPhases[ANIMATION_PHASE_COLUMN_3] %= UPDATE_FRAME_CYCLE;
        } else {
            ++m_animationPhases[ANIMATION_PHASE_COLUMN_0];
            m_animationPhases[ANIMATION_PHASE_COLUMN_0] %= UPDATE_FRAME_CYCLE;
            ++m_animationPhases[ANIMATION_PHASE_COLUMN_2];
            m_animationPhases[ANIMATION_PHASE_COLUMN_2] %= UPDATE_FRAME_CYCLE;
        }
    }
    giLimitUpdMinX = UPDATE_NONE;
    platform::PumpEvents();
}

void advManager::CompleteDraw(i32 originX, i32 originY, i32 forceDraw, i32 updateBottomView) {
    i32 drawY;
    i32 drawX;

    PollSound();
    if (forceDraw == 0 && bShowIt == 0) {
        return;
    }

    giLimitUpdMinX = UPDATE_NONE;
    m_previousOriginX = m_mapOriginX;
    m_previousOriginY = m_mapOriginY;
    if (gbAllBlack != 0) {
        m_mapOriginY = 0;
        m_mapOriginX = 0;
    }

    gpMouseManager->m_cursorReady = 0;
    m_comboHeroDrawn = 0;
    m_forceCompleteDraw = 0;

    for (drawY = 0; drawY < DRAW_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < DRAW_VIEW_CELLS; ++drawX) {
            DrawCell(originX + drawX, originY + drawY, drawX, drawY, ADVMGR_DRAW_GROUND, forceDraw);
        }
    }

    for (drawY = 0; drawY < DRAW_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < DRAW_VIEW_CELLS; ++drawX) {
            DrawCell(
                originX + drawX,
                originY + drawY,
                drawX,
                drawY,
                ADVMGR_DRAW_HERO_SHADOW,
                forceDraw
            );
        }
    }

    for (drawX = 0; drawX < DRAW_VIEW_CELLS; ++drawX) {
        DrawCell(originX + drawX, originY, drawX, 0, ADVMGR_DRAW_OBJECT, forceDraw);
    }

    for (drawY = 1; drawY < DRAW_VIEW_CELLS; ++drawY) {
        PollSound();
        if (m_cursorDirection > MAP_DIRECTION_SOUTH) {
            for (drawX = 0; drawX < DRAW_VIEW_CELLS; ++drawX) {
                DrawCell(
                    originX + drawX,
                    originY + drawY - 1,
                    drawX,
                    drawY - 1,
                    ADVMGR_DRAW_HERO,
                    forceDraw
                );
            }
            for (drawX = 0; drawX < DRAW_VIEW_CELLS; ++drawX) {
                DrawCell(
                    originX + drawX,
                    originY + drawY - 1,
                    drawX,
                    drawY - 1,
                    ADVMGR_DRAW_OVERLAY,
                    forceDraw
                );
            }
        } else {
            for (drawX = DRAW_LAST_CELL; drawX >= 0; --drawX) {
                DrawCell(
                    originX + drawX,
                    originY + drawY - 1,
                    drawX,
                    drawY - 1,
                    ADVMGR_DRAW_HERO,
                    forceDraw
                );
            }
            for (drawX = DRAW_LAST_CELL; drawX >= 0; --drawX) {
                DrawCell(
                    originX + drawX,
                    originY + drawY - 1,
                    drawX,
                    drawY - 1,
                    ADVMGR_DRAW_OVERLAY,
                    forceDraw
                );
            }
        }

        for (drawX = 0; drawX < DRAW_VIEW_CELLS; ++drawX) {
            DrawCell(originX + drawX, originY + drawY, drawX, drawY, ADVMGR_DRAW_OBJECT, forceDraw);
        }
    }

    for (drawX = 0; drawX < DRAW_VIEW_CELLS; ++drawX) {
        DrawCell(
            originX + drawX,
            originY + DRAW_LAST_CELL,
            drawX,
            DRAW_LAST_CELL,
            ADVMGR_DRAW_HERO,
            forceDraw
        );
    }
    for (drawX = 0; drawX < DRAW_VIEW_CELLS; ++drawX) {
        DrawCell(
            originX + drawX,
            originY + DRAW_LAST_CELL,
            drawX,
            DRAW_LAST_CELL,
            ADVMGR_DRAW_OVERLAY,
            forceDraw
        );
    }

    for (drawY = 0; drawY < DRAW_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < DRAW_VIEW_CELLS; ++drawX) {
            DrawCell(
                originX + drawX,
                originY + drawY,
                drawX,
                drawY,
                ADVMGR_DRAW_OVERLAY_TOP,
                forceDraw
            );
        }
    }
    for (drawY = 0; drawY < DRAW_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < DRAW_VIEW_CELLS; ++drawX) {
            DrawCell(originX + drawX, originY + drawY, drawX, drawY, ADVMGR_DRAW_CLOUD, forceDraw);
        }
    }

    DrawAdventureBorder();
    gpMouseManager->m_cursorReady = 1;
    PollSound();
    if (updateBottomView != 0) {
        UpdBottomView(0, 1, 1);
    }

    if (gbAllBlack != 0) {
        m_mapOriginX = m_previousOriginX;
        m_mapOriginY = m_previousOriginY;
    }
}

void advManager::CompleteDraw(i32 update) {
    CompleteDraw(m_mapOriginX, m_mapOriginY, update, 1);
}

i32 advManager::GetCloudLookup(i32 x, i32 y) {
    u32 cloudMask = 0;

    if (x < 1) {
        cloudMask |= CLOUD_LEFT_EDGE;
    } else if (x >= MAP_WIDTH - 1) {
        cloudMask |= CLOUD_RIGHT_EDGE;
    }

    if (y < 1) {
        cloudMask |= CLOUD_TOP_EDGE;
    } else if (y >= MAP_HEIGHT - 1) {
        cloudMask |= CLOUD_BOTTOM_EDGE;
    }

    if (cloudMask == 0) {
        if ((MAP_EXTRA_AT(x, y - 1) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_NORTH;
        }
        if ((MAP_EXTRA_AT(x + 1, y) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_EAST;
        }
        if ((MAP_EXTRA_AT(x, y + 1) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_SOUTH;
        }
        if ((MAP_EXTRA_AT(x - 1, y) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_WEST;
        }
        if ((MAP_EXTRA_AT(x + 1, y - 1) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_NORTH_EAST;
        }
        if ((MAP_EXTRA_AT(x + 1, y + 1) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_SOUTH_EAST;
        }
        if ((MAP_EXTRA_AT(x - 1, y + 1) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_SOUTH_WEST;
        }
        if ((MAP_EXTRA_AT(x - 1, y - 1) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_NORTH_WEST;
        }
    } else {
        if ((cloudMask & CLOUD_NORTH) == 0
            && (MAP_EXTRA_AT(x, y - 1) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_NORTH;
        }
        if ((cloudMask & CLOUD_EAST) == 0
            && (MAP_EXTRA_AT(x + 1, y) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_EAST;
        }
        if ((cloudMask & CLOUD_SOUTH) == 0
            && (MAP_EXTRA_AT(x, y + 1) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_SOUTH;
        }
        if ((cloudMask & CLOUD_WEST) == 0
            && (MAP_EXTRA_AT(x - 1, y) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_WEST;
        }
        if ((cloudMask & CLOUD_NORTH_EAST) == 0
            && (MAP_EXTRA_AT(x + 1, y - 1) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_NORTH_EAST;
        }
        if ((cloudMask & CLOUD_SOUTH_EAST) == 0
            && (MAP_EXTRA_AT(x + 1, y + 1) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_SOUTH_EAST;
        }
        if ((cloudMask & CLOUD_SOUTH_WEST) == 0
            && (MAP_EXTRA_AT(x - 1, y + 1) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_SOUTH_WEST;
        }
        if ((cloudMask & CLOUD_NORTH_WEST) == 0
            && (MAP_EXTRA_AT(x - 1, y - 1) & giCurWatchPlayerBit) == 0) {
            cloudMask |= CLOUD_NORTH_WEST;
        }
    }
    return giCloudType[cloudMask];
}

void advManager::DrawCell(
    i32 mapX,
    i32 mapY,
    i32 screenX,
    i32 screenY,
    AdventureDrawMask drawMask,
    i32 forceDraw
) {
    i32 animFrame;
    i32 cursorFrame;
    i32 boatFrameIndex;
    i32 cursorSuppressed;

    if (forceDraw == 0 && bShowIt == 0) {
        return;
    }

    s_drawPixelX = screenX * CELL_PIXELS;
    s_drawPixelY = screenY * CELL_PIXELS;
    s_drawCell = GetCell(mapX, mapY);

    if (gbAllBlack == 0 && (mapX < 0 || mapY < 0 || mapX >= MAP_WIDTH || mapY >= MAP_HEIGHT)) {
        s_drawStoneTile = STONE_TILE_NONE;
        if (mapX == -1) {
            if (mapY == -1) {
                s_drawStoneTile = STONE_TILE_TOP_LEFT;
            } else if (mapY == MAP_HEIGHT) {
                s_drawStoneTile = STONE_TILE_BOTTOM_LEFT;
            } else if (mapY >= 0 && mapY < MAP_HEIGHT) {
                s_drawStoneTile = ((mapY + STONE_PATTERN_COORDINATE_SHIFT) & CLOUD_VARIANT_MASK)
                        + STONE_TILE_LEFT_BASE;
            }
        } else if (mapX == MAP_WIDTH) {
            if (mapY == -1) {
                s_drawStoneTile = STONE_TILE_TOP_RIGHT;
            } else if (mapY == MAP_HEIGHT) {
                s_drawStoneTile = STONE_TILE_BOTTOM_RIGHT;
            } else if (mapY >= 0 && mapY < MAP_HEIGHT) {
                s_drawStoneTile = ((mapY + STONE_PATTERN_COORDINATE_SHIFT) & CLOUD_VARIANT_MASK)
                        + STONE_TILE_RIGHT_BASE;
            }
        } else if (mapY == -1) {
            if (mapX >= 0 && mapX < MAP_WIDTH) {
                s_drawStoneTile = ((mapX + STONE_PATTERN_COORDINATE_SHIFT) & CLOUD_VARIANT_MASK)
                        + STONE_TILE_TOP_BASE;
            }
        } else if (mapY == MAP_HEIGHT && mapX >= 0 && mapX < MAP_WIDTH) {
            s_drawStoneTile = ((mapX + STONE_PATTERN_COORDINATE_SHIFT) & CLOUD_VARIANT_MASK)
                    + STONE_TILE_BOTTOM_BASE;
        }
        if (s_drawStoneTile == STONE_TILE_NONE) {
            s_drawStoneTile = (mapX + STONE_PATTERN_COORDINATE_SHIFT) % CLOUD_VARIANTS
                + ((mapY + STONE_PATTERN_COORDINATE_SHIFT) % CLOUD_VARIANTS) * CLOUD_VARIANTS;
        }
        TileToBitmap(
            m_stoneTiles,
            s_drawStoneTile,
            gpWindowManager->m_screen,
            s_drawPixelX,
            s_drawPixelY
        );
        return;
    } else {

        if (!(((gbAllBlack == 0
                && (MAP_EXTRA_AT_WFIRST(mapX, mapY) & giCurWatchPlayerBit) != 0)
               || gbDrawingPuzzle != 0))) {
            s_drawCovered = 1;
            if (gbAllBlack != 0) {
                s_drawCloudFrame = 0;
            } else {
                s_drawCloudFrame = GetCloudLookup(mapX, mapY);
            }
            if (s_drawCloudFrame == 0) {
                if ((H2EnumIndex((drawMask) & (ADVMGR_DRAW_CLOUD)))) {
                    TileToBitmap(
                        m_cloudTiles,
                        (mapX + mapY) & CLOUD_VARIANT_MASK,
                        gpWindowManager->m_screen,
                        s_drawPixelX,
                        s_drawPixelY
                    );
                }
                return;
            }
            if (s_drawCloudFrame >= CLOUD_FLIPPED_FRAME_BASE) {
                s_drawFlipCloud = 1;
                s_drawCloudFrame -= CLOUD_FLIPPED_FRAME_BASE;
            } else {
                s_drawFlipCloud = 0;
            }
            if ((s_drawCloudFrame == CLOUD_X_ALTERNATE_FRAME_1
                 || s_drawCloudFrame == CLOUD_X_ALTERNATE_FRAME_2)
                && (mapX & 1)) {
                ++s_drawCloudFrame;
            }
            if (s_drawCloudFrame == CLOUD_Y_ALTERNATE_FRAME && (mapY & 1)) {
                ++s_drawCloudFrame;
            }
        } else {
            s_drawCovered = 0;
        }

        if ((H2EnumIndex((drawMask) & (ADVMGR_DRAW_CLOUD))) && !gbDrawingPuzzle) {
            if (s_drawCovered != 0) {
                if (s_drawFlipCloud != 0) {
                    FlipIconToBitmap(
                        m_cloudOverlayIcon,
                        gpWindowManager->m_screen,
                        s_drawPixelX + CELL_LAST_PIXEL,
                        s_drawPixelY,
                        s_drawCloudFrame - 1,
                        ICON_DRAW_NO_CLIP,
                        0,
                        0,
                        0,
                        0,
                        0
                    );
                } else {
                    IconToBitmap(
                        m_cloudOverlayIcon,
                        gpWindowManager->m_screen,
                        s_drawPixelX,
                        s_drawPixelY,
                        s_drawCloudFrame - 1,
                        ICON_DRAW_NO_CLIP,
                        0,
                        0,
                        0,
                        0,
                        0
                    );
                }
            } else if (m_visibilityMapValid && ADVMGR_VISIBILITY_AT(mapX, mapY) != 0) {
                if ((ADVMGR_VISIBILITY_AT(mapX, mapY)
                     & ROUTE_BEYOND_MOBILITY_FLAG)
                    != 0) {
                    IconToBitmapColorTable(
                        m_objectIcons[H2EnumIndex(TILESET_ROUTE)],
                        gpWindowManager->m_screen,
                        s_drawPixelX - ROUTE_DRAW_X_OFFSET,
                        s_drawPixelY + ROUTE_DRAW_Y_OFFSET,
                        (ADVMGR_VISIBILITY_AT(mapX, mapY) - 1) & ROUTE_FRAME_MASK,
                        ICON_DRAW_CLIP,
                        0,
                        0,
                        DRAW_CLIP_WIDTH,
                        DRAW_CLIP_HEIGHT,
                        0,
                        gColorTableRed,
                        1
                    );
                } else {
                    IconToBitmap(
                        m_objectIcons[H2EnumIndex(TILESET_ROUTE)],
                        gpWindowManager->m_screen,
                        s_drawPixelX - ROUTE_DRAW_X_OFFSET,
                        s_drawPixelY + ROUTE_DRAW_Y_OFFSET,
                        (ADVMGR_VISIBILITY_AT(mapX, mapY) - 1) & ROUTE_FRAME_MASK,
                        ICON_DRAW_CLIP,
                        0,
                        0,
                        DRAW_CLIP_WIDTH,
                        DRAW_CLIP_HEIGHT,
                        0
                    );
                }
            }
        } else {
            if ((H2EnumIndex((drawMask) & (ADVMGR_DRAW_GROUND)))) {
                s_drawGroundTile = s_drawCell->m_flags;
                s_drawGroundTile <<= TERRAIN_FLAGS_SHIFT;
                s_drawGroundTile |= s_drawCell->m_terrainImageIndex;
                TileToBitmap(
                    m_groundTiles,
                    s_drawGroundTile,
                    gpWindowManager->m_screen,
                    s_drawPixelX,
                    s_drawPixelY
                );

                if (s_drawCell->m_objectLayerBit0
                    && (gbDrawingPuzzle == 0 || s_drawCell->m_objectTileset != TILESET_OBJNDIRT
                        || s_drawCell->m_objectIndex != DIG_HOLE_FRAME)
                    && (gbDrawingPuzzle == 0 || bPuzzleDraw[H2EnumIndex(s_drawCell->m_objectTileset)])) {
                    IconToBitmap(
                        m_objectIcons[H2EnumIndex(s_drawCell->m_objectTileset)],
                        gpWindowManager->m_screen,
                        s_drawPixelX,
                        s_drawPixelY,
                        s_drawCell->m_objectIndex,
                        ICON_DRAW_NO_CLIP,
                        0,
                        0,
                        DRAW_CLIP_WIDTH,
                        DRAW_CLIP_HEIGHT,
                        0
                    );
                    if (s_drawCell->m_animatedObject) {
                        s_drawAnimationLength = GetIconEntry(
                                                    m_objectIcons[H2EnumIndex(s_drawCell->m_objectTileset)],
                                                    s_drawCell->m_objectIndex
                        )
                                                    ->flags;
                        IconToBitmap(
                            m_objectIcons[H2EnumIndex(s_drawCell->m_objectTileset)],
                            gpWindowManager->m_screen,
                            s_drawPixelX,
                            s_drawPixelY,
                            s_drawCell->m_objectIndex + m_updateMaxY % s_drawAnimationLength + 1,
                            ICON_DRAW_NO_CLIP,
                            0,
                            0,
                            DRAW_CLIP_WIDTH,
                            DRAW_CLIP_HEIGHT,
                            0
                        );
                    }
                }

                if (s_drawCell->m_extraIndex != 0
                    && m_mapData->Extra(s_drawCell->m_extraIndex)->objectIndex
                           != MAPCELL_SPRITE_NONE) {
                    s_drawExtra = m_mapData->Extra(s_drawCell->m_extraIndex);
                } else {
                    s_drawExtra = NULL;
                }
                while (s_drawExtra != NULL) {
                    if (s_drawExtra->objectLayerBit0
                        && (gbDrawingPuzzle == 0 || bPuzzleDraw[H2EnumIndex(s_drawExtra->objectTileset)])) {
                        IconToBitmap(
                            m_objectIcons[H2EnumIndex(s_drawExtra->objectTileset)],
                            gpWindowManager->m_screen,
                            s_drawPixelX,
                            s_drawPixelY,
                            s_drawExtra->objectIndex,
                            ICON_DRAW_NO_CLIP,
                            0,
                            0,
                            DRAW_CLIP_WIDTH,
                            DRAW_CLIP_HEIGHT,
                            0
                        );
                        if (s_drawExtra->animatedObject) {
                            s_drawAnimationLength =
                                GetIconEntry(
                                    m_objectIcons[H2EnumIndex(s_drawExtra->objectTileset)],
                                    s_drawExtra->objectIndex
                                )
                                    ->flags;
                            IconToBitmap(
                                m_objectIcons[H2EnumIndex(s_drawExtra->objectTileset)],
                                gpWindowManager->m_screen,
                                s_drawPixelX,
                                s_drawPixelY,
                                s_drawExtra->objectIndex + m_updateMaxY % s_drawAnimationLength + 1,
                                ICON_DRAW_NO_CLIP,
                                0,
                                0,
                                DRAW_CLIP_WIDTH,
                                DRAW_CLIP_HEIGHT,
                                0
                            );
                        }
                    }
                    if (s_drawExtra->nextIndex != 0
                        && m_mapData->Extra(s_drawExtra->nextIndex)->objectIndex
                               != MAPCELL_SPRITE_NONE) {
                        s_drawExtra = m_mapData->Extra(s_drawExtra->nextIndex);
                    } else {
                        s_drawExtra = NULL;
                    }
                }

                if (s_drawCell->m_objectLayerBit1 && !s_drawCell->m_objectLayerBit0
                    && (gbDrawingPuzzle == 0 || bPuzzleDraw[H2EnumIndex(s_drawCell->m_objectTileset)])) {
                    IconToBitmap(
                        m_objectIcons[H2EnumIndex(s_drawCell->m_objectTileset)],
                        gpWindowManager->m_screen,
                        s_drawPixelX,
                        s_drawPixelY,
                        s_drawCell->m_objectIndex,
                        ICON_DRAW_NO_CLIP,
                        0,
                        0,
                        DRAW_CLIP_WIDTH,
                        DRAW_CLIP_HEIGHT,
                        0
                    );
                    if (s_drawCell->m_animatedObject) {
                        s_drawAnimationLength = GetIconEntry(
                                                    m_objectIcons[H2EnumIndex(s_drawCell->m_objectTileset)],
                                                    s_drawCell->m_objectIndex
                        )
                                                    ->flags;
                        IconToBitmap(
                            m_objectIcons[H2EnumIndex(s_drawCell->m_objectTileset)],
                            gpWindowManager->m_screen,
                            s_drawPixelX,
                            s_drawPixelY,
                            s_drawCell->m_objectIndex + m_updateMaxY % s_drawAnimationLength + 1,
                            ICON_DRAW_NO_CLIP,
                            0,
                            0,
                            DRAW_CLIP_WIDTH,
                            DRAW_CLIP_HEIGHT,
                            0
                        );
                    }
                }

                if (s_drawCell->m_extraIndex != 0
                    && m_mapData->Extra(s_drawCell->m_extraIndex)->objectIndex
                           != MAPCELL_SPRITE_NONE) {
                    s_drawExtra = m_mapData->Extra(s_drawCell->m_extraIndex);
                } else {
                    s_drawExtra = NULL;
                }
                while (s_drawExtra != NULL) {
                    if (s_drawExtra->objectLayerBit1 && !s_drawExtra->objectLayerBit0
                        && (gbDrawingPuzzle == 0 || bPuzzleDraw[H2EnumIndex(s_drawExtra->objectTileset)])) {
                        IconToBitmap(
                            m_objectIcons[H2EnumIndex(s_drawExtra->objectTileset)],
                            gpWindowManager->m_screen,
                            s_drawPixelX,
                            s_drawPixelY,
                            s_drawExtra->objectIndex,
                            ICON_DRAW_NO_CLIP,
                            0,
                            0,
                            DRAW_CLIP_WIDTH,
                            DRAW_CLIP_HEIGHT,
                            0
                        );
                        if (s_drawExtra->animatedObject) {
                            s_drawAnimationLength =
                                GetIconEntry(
                                    m_objectIcons[H2EnumIndex(s_drawExtra->objectTileset)],
                                    s_drawExtra->objectIndex
                                )
                                    ->flags;
                            IconToBitmap(
                                m_objectIcons[H2EnumIndex(s_drawExtra->objectTileset)],
                                gpWindowManager->m_screen,
                                s_drawPixelX,
                                s_drawPixelY,
                                s_drawExtra->objectIndex + m_updateMaxY % s_drawAnimationLength + 1,
                                ICON_DRAW_NO_CLIP,
                                0,
                                0,
                                DRAW_CLIP_WIDTH,
                                DRAW_CLIP_HEIGHT,
                                0
                            );
                        }
                    }
                    if (s_drawExtra->nextIndex != 0
                        && m_mapData->Extra(s_drawExtra->nextIndex)->objectIndex
                               != MAPCELL_SPRITE_NONE) {
                        s_drawExtra = m_mapData->Extra(s_drawExtra->nextIndex);
                    } else {
                        s_drawExtra = NULL;
                    }
                }
            }

            if ((H2EnumIndex((drawMask) & (ADVMGR_DRAW_OBJECT)))) {
                if (s_drawCell->m_objectIndex != MAPCELL_SPRITE_NONE
                    && !s_drawCell->m_objectLayerBit0 && !s_drawCell->m_objectLayerBit1
                    && !s_drawCell->m_objectDrawnAsOverlay
                    && s_drawCell->m_objectTileset != TILESET_MONS32
                    && (gbDrawingPuzzle == 0 || bPuzzleDraw[H2EnumIndex(s_drawCell->m_objectTileset)])) {
                    IconToBitmap(
                        m_objectIcons[H2EnumIndex(s_drawCell->m_objectTileset)],
                        gpWindowManager->m_screen,
                        s_drawPixelX,
                        s_drawPixelY,
                        s_drawCell->m_objectIndex,
                        ICON_DRAW_NO_CLIP,
                        0,
                        0,
                        DRAW_CLIP_WIDTH,
                        DRAW_CLIP_HEIGHT,
                        0
                    );
                    if (s_drawCell->m_animatedObject) {
                        s_drawAnimationLength = GetIconEntry(
                                                    m_objectIcons[H2EnumIndex(s_drawCell->m_objectTileset)],
                                                    s_drawCell->m_objectIndex
                        )
                                                    ->flags;
                        animFrame = m_updateMaxY % s_drawAnimationLength;
                        if (s_drawCell->m_triggerType
                            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MAGIC_GARDEN)) {
                            if (s_drawCell->m_objectMetadata != 0) {
                                animFrame = m_updateMaxY % (s_drawAnimationLength - 1);
                            } else {
                                animFrame = s_drawAnimationLength - 1;
                            }
                        }
                        IconToBitmap(
                            m_objectIcons[H2EnumIndex(s_drawCell->m_objectTileset)],
                            gpWindowManager->m_screen,
                            s_drawPixelX,
                            s_drawPixelY,
                            s_drawCell->m_objectIndex + animFrame + 1,
                            ICON_DRAW_NO_CLIP,
                            0,
                            0,
                            DRAW_CLIP_WIDTH,
                            DRAW_CLIP_HEIGHT,
                            0
                        );
                    }
                }

                if (s_drawCell->m_extraIndex != 0
                    && m_mapData->Extra(s_drawCell->m_extraIndex)->objectIndex
                           != MAPCELL_SPRITE_NONE) {
                    s_drawExtra = m_mapData->Extra(s_drawCell->m_extraIndex);
                } else {
                    s_drawExtra = NULL;
                }
                while (s_drawExtra != NULL) {
                    if (!s_drawExtra->objectLayerBit0 && !s_drawExtra->objectLayerBit1
                        && !s_drawExtra->objectDrawnAsOverlay
                        && s_drawExtra->objectTileset != TILESET_MONS32
                        && (gbDrawingPuzzle == 0 || bPuzzleDraw[H2EnumIndex(s_drawExtra->objectTileset)])) {
                        IconToBitmap(
                            m_objectIcons[H2EnumIndex(s_drawExtra->objectTileset)],
                            gpWindowManager->m_screen,
                            s_drawPixelX,
                            s_drawPixelY,
                            s_drawExtra->objectIndex,
                            ICON_DRAW_NO_CLIP,
                            0,
                            0,
                            DRAW_CLIP_WIDTH,
                            DRAW_CLIP_HEIGHT,
                            0
                        );
                        if (s_drawExtra->animatedObject) {
                            s_drawAnimationLength =
                                GetIconEntry(
                                    m_objectIcons[H2EnumIndex(s_drawExtra->objectTileset)],
                                    s_drawExtra->objectIndex
                                )
                                    ->flags;
                            IconToBitmap(
                                m_objectIcons[H2EnumIndex(s_drawExtra->objectTileset)],
                                gpWindowManager->m_screen,
                                s_drawPixelX,
                                s_drawPixelY,
                                s_drawExtra->objectIndex + m_updateMaxY % s_drawAnimationLength + 1,
                                ICON_DRAW_NO_CLIP,
                                0,
                                0,
                                DRAW_CLIP_WIDTH,
                                DRAW_CLIP_HEIGHT,
                                0
                            );
                        }
                    }
                    if (s_drawExtra->nextIndex != 0
                        && m_mapData->Extra(s_drawExtra->nextIndex)->objectIndex
                               != MAPCELL_SPRITE_NONE) {
                        s_drawExtra = m_mapData->Extra(s_drawExtra->nextIndex);
                    } else {
                        s_drawExtra = NULL;
                    }
                }
            }

            if (((H2EnumIndex((drawMask) & (ADVMGR_DRAW_HERO))) || (H2EnumIndex((drawMask) & (ADVMGR_DRAW_HERO_SHADOW))))
                && gbDrawingPuzzle == 0) {
                s_drawHasHero = 0;
                s_drawHero = NULL;
                if ((H2EnumIndex((drawMask) & (ADVMGR_DRAW_HERO)))) {
                    if (mapX > 0) {
                        s_drawAdjacentCell = GetCell(mapX - 1, mapY);
                        if (s_drawAdjacentCell->m_triggerType
                            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE)) {
                            s_drawMine = &gpGame->m_mines[s_drawAdjacentCell->m_objectMetadata];
                            if (s_drawMine->guardianType == CREATURE_GHOST) {
                                IconToBitmap(
                                    m_objectIcons[MINE_GUARDIAN_ICON_SLOT],
                                    gpWindowManager->m_screen,
                                    s_drawPixelX - CELL_CENTER_PIXEL,
                                    s_drawPixelY,
                                    (m_updateMaxY + mapX + mapY) % MINE_GHOST_FRAME_COUNT,
                                    ICON_DRAW_CLIP,
                                    0,
                                    0,
                                    DRAW_CLIP_WIDTH,
                                    DRAW_CLIP_HEIGHT,
                                    0
                                );
                            } else if (s_drawMine->guardianType != CREATURE_NONE) {
                                IconToBitmap(
                                    m_objectIcons[TILESET_BOAT],
                                    gpWindowManager->m_screen,
                                    s_drawPixelX - CELL_PIXELS,
                                    s_drawPixelY,
                                    H2EnumIndex(s_drawMine->guardianType) - H2EnumIndex(CREATURE_EARTH_ELEMENTAL),
                                    ICON_DRAW_CLIP,
                                    0,
                                    0,
                                    DRAW_CLIP_WIDTH,
                                    DRAW_CLIP_HEIGHT,
                                    0
                                );
                            }
                        }
                    }

                    if (s_drawCell->m_objectTileset == TILESET_MONS32) {
                        if (mapX == m_lastQuickViewX && mapY == m_lastQuickViewY) {
                            IconToBitmap(
                                m_objectIcons[TILESET_MONSTER],
                                gpWindowManager->m_screen,
                                s_drawPixelX + CELL_CENTER_PIXEL,
                                s_drawPixelY + OBJECT_BASELINE_Y,
                                s_drawCell->m_objectIndex * MONSTER_FRAME_STRIDE
                                    + (m_mineGuardianFacingLeft != 0
                                           ? MONSTER_FACING_FRAME_BASE + 1
                                           : MONSTER_FACING_FRAME_BASE),
                                ICON_DRAW_CLIP,
                                0,
                                0,
                                DRAW_CLIP_WIDTH,
                                DRAW_CLIP_HEIGHT,
                                0
                            );
                        } else {
                            IconToBitmap(
                                m_objectIcons[TILESET_MONSTER],
                                gpWindowManager->m_screen,
                                s_drawPixelX + CELL_CENTER_PIXEL,
                                s_drawPixelY + OBJECT_BASELINE_Y,
                                s_drawCell->m_objectIndex * MONSTER_FRAME_STRIDE,
                                ICON_DRAW_CLIP,
                                0,
                                0,
                                DRAW_CLIP_WIDTH,
                                DRAW_CLIP_HEIGHT,
                                0
                            );
                            if (s_drawCell->m_objectIndex == MONSTER_SPECIAL_FRAME_FIRST
                                || s_drawCell->m_objectIndex == MONSTER_SPECIAL_FRAME_LAST) {
                                s_drawMonsterFrame =
                                    m_animationPhases[mapX & ANIMATION_PHASE_COLUMN_MASK]
                                    % UPDATE_ANIMATION_PHASES;
                            } else {
                                s_drawMonsterFrame = monAnimDrawFrame
                                    [m_animationPhases[mapX & ANIMATION_PHASE_COLUMN_MASK]];
                            }
                            IconToBitmap(
                                m_objectIcons[TILESET_MONSTER],
                                gpWindowManager->m_screen,
                                s_drawPixelX + CELL_CENTER_PIXEL,
                                s_drawPixelY + OBJECT_BASELINE_Y,
                                s_drawCell->m_objectIndex * MONSTER_FRAME_STRIDE
                                    + s_drawMonsterFrame + MONSTER_ANIMATION_FRAME_OFFSET,
                                ICON_DRAW_CLIP,
                                0,
                                0,
                                DRAW_CLIP_WIDTH,
                                DRAW_CLIP_HEIGHT,
                                0
                            );
                        }
                    }
                }

                if (s_drawCell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT)) {
                    s_drawPlayerColor = -1;
                    s_drawHeroType = HERO_TYPE_BOAT;
                    s_drawHeroFrame =
                        GetCursorBaseFrame(gpGame->m_boats[s_drawCell->m_objectMetadata].direction);
                    s_drawHasHero = 1;
                    s_drawHeroYOffset = HERO_BOAT_Y_OFFSET;
                } else {
                    s_drawHeroYOffset = 0;
                    if (s_drawCell->m_triggerType
                        == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)) {
                        s_drawHero = gpGame->GetHero(s_drawCell->m_objectMetadata);
                        s_drawPlayerColor = gpGame->m_players[s_drawHero->m_owner].m_color;
                        s_drawHeroType = (H2EnumIndex((s_drawHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))
                            ? HERO_TYPE_BOAT
                            : static_cast<HeroCursorType>(s_drawHero->m_cursorType);
                        s_drawHeroFrame = GetCursorBaseFrame(s_drawHero->m_direction);
                        s_drawHasHero = 1;
                        if ((H2EnumIndex((s_drawHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))) {
                            s_drawHeroYOffset = HERO_BOAT_Y_OFFSET;
                        }
                    }
                }

                if (s_drawHasHero) {
                    if (s_drawHeroFrame & HERO_FRAME_MIRROR_FLAG) {
                        if ((H2EnumIndex((drawMask) & (ADVMGR_DRAW_HERO_SHADOW)))) {
                            if (m_drawHeroShadows != 0 && s_drawHeroType != HERO_TYPE_BOAT) {
                                cursorFrame = s_drawHeroFrame & HERO_FRAME_INDEX_MASK;
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_51) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_56;
                                }
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_50) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_57;
                                }
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_49) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_58;
                                }
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_47) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_55;
                                }
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_46) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_55;
                                }
                                IconToBitmap(
                                    m_shadowIcon,
                                    gpWindowManager->m_screen,
                                    s_drawPixelX,
                                    s_drawPixelY + CELL_LAST_PIXEL,
                                    cursorFrame
                                        + (cursorFrame >= HERO_SHADOW_FRAME_FIRST
                                                   && cursorFrame < HERO_SHADOW_FRAME_END
                                               ? HERO_SHADOW_FRAME_OFFSET
                                               : 0),
                                    ICON_DRAW_CLIP,
                                    0,
                                    0,
                                    DRAW_CLIP_WIDTH,
                                    DRAW_CLIP_HEIGHT,
                                    0
                                );
                            }
                            if (m_drawHeroShadows != 0 && s_drawHeroType == HERO_TYPE_BOAT) {
                                boatFrameIndex = s_drawHeroFrame & HERO_FRAME_INDEX_MASK;
                                IconToBitmap(
                                    m_boatShadowIcon,
                                    gpWindowManager->m_screen,
                                    s_drawPixelX,
                                    s_drawPixelY + s_drawHeroYOffset + CELL_LAST_PIXEL,
                                    boatFrameIndex
                                        + (boatFrameIndex >= HERO_SHADOW_FRAME_FIRST
                                                   && boatFrameIndex < HERO_SHADOW_FRAME_END
                                               ? HERO_SHADOW_FRAME_END
                                               : 0),
                                    ICON_DRAW_CLIP,
                                    0,
                                    0,
                                    DRAW_CLIP_WIDTH,
                                    DRAW_CLIP_HEIGHT,
                                    0
                                );
                            }
                        } else {
                            if (s_drawHeroType == HERO_TYPE_BOAT
                                && (s_drawCell->m_flags & CURSOR_CELL_UNCOVERED_FLAG) == 0) {
                                FlipIconToBitmap(
                                    m_heroIcons[HERO_ICON_FROTH],
                                    gpWindowManager->m_screen,
                                    s_drawPixelX + CELL_PIXELS,
                                    s_drawPixelY + s_drawHeroYOffset + CELL_LAST_PIXEL,
                                    s_drawHeroFrame & HERO_FRAME_INDEX_MASK,
                                    ICON_DRAW_CLIP,
                                    0,
                                    0,
                                    DRAW_CLIP_WIDTH,
                                    DRAW_CLIP_HEIGHT,
                                    0
                                );
                            }
                            FlipIconToBitmap(
                                m_heroIcons[H2EnumIndex(s_drawHeroType)],
                                gpWindowManager->m_screen,
                                s_drawPixelX + CELL_PIXELS,
                                s_drawPixelY + s_drawHeroYOffset + CELL_LAST_PIXEL,
                                s_drawHeroFrame & HERO_FRAME_INDEX_MASK,
                                ICON_DRAW_CLIP,
                                0,
                                0,
                                DRAW_CLIP_WIDTH,
                                DRAW_CLIP_HEIGHT,
                                0
                            );
                            if (s_drawPlayerColor != -1) {
                                if (s_drawHeroType == HERO_TYPE_BOAT) {
                                    FlipIconToBitmap(
                                        m_boatFlagIcons[s_drawPlayerColor],
                                        gpWindowManager->m_screen,
                                        s_drawPixelX + CELL_PIXELS,
                                        s_drawPixelY + s_drawHeroYOffset + CELL_LAST_PIXEL,
                                        s_drawHeroFrame & HERO_FRAME_INDEX_MASK,
                                        ICON_DRAW_CLIP,
                                        0,
                                        0,
                                        DRAW_CLIP_WIDTH,
                                        DRAW_CLIP_HEIGHT,
                                        0
                                    );
                                } else {
                                    FlipIconToBitmap(
                                        m_flagIcons[s_drawPlayerColor],
                                        gpWindowManager->m_screen,
                                        s_drawPixelX + CELL_PIXELS,
                                        s_drawPixelY + CELL_LAST_PIXEL,
                                        (s_drawHeroFrame & HERO_FRAME_INDEX_MASK)
                                            + m_updateMaxY % PLAYER_FLAG_FRAME_COUNT
                                            + PLAYER_FLAG_FRAME_BASE,
                                        ICON_DRAW_CLIP,
                                        0,
                                        0,
                                        DRAW_CLIP_WIDTH,
                                        DRAW_CLIP_HEIGHT,
                                        0
                                    );
                                }
                            }
                        }
                    } else {
                        if ((H2EnumIndex((drawMask) & (ADVMGR_DRAW_HERO_SHADOW)))) {
                            if (m_drawHeroShadows != 0 && s_drawHeroType != HERO_TYPE_BOAT
                                && (H2EnumIndex((drawMask) & (ADVMGR_DRAW_HERO_SHADOW)))) {
                                IconToBitmap(
                                    m_shadowIcon,
                                    gpWindowManager->m_screen,
                                    s_drawPixelX,
                                    s_drawPixelY + CELL_LAST_PIXEL,
                                    s_drawHeroFrame,
                                    ICON_DRAW_CLIP,
                                    0,
                                    0,
                                    DRAW_CLIP_WIDTH,
                                    DRAW_CLIP_HEIGHT,
                                    0
                                );
                            }
                            if (m_drawHeroShadows != 0 && s_drawHeroType == HERO_TYPE_BOAT) {
                                IconToBitmap(
                                    m_boatShadowIcon,
                                    gpWindowManager->m_screen,
                                    s_drawPixelX,
                                    s_drawPixelY + s_drawHeroYOffset + CELL_LAST_PIXEL,
                                    s_drawHeroFrame,
                                    ICON_DRAW_CLIP,
                                    0,
                                    0,
                                    DRAW_CLIP_WIDTH,
                                    DRAW_CLIP_HEIGHT,
                                    0
                                );
                            }
                        } else {
                            if (s_drawHeroType == HERO_TYPE_BOAT
                                && (s_drawCell->m_flags & CURSOR_CELL_UNCOVERED_FLAG) == 0) {
                                IconToBitmap(
                                    m_heroIcons[HERO_ICON_FROTH],
                                    gpWindowManager->m_screen,
                                    s_drawPixelX,
                                    s_drawPixelY + s_drawHeroYOffset + CELL_LAST_PIXEL,
                                    s_drawHeroFrame,
                                    ICON_DRAW_CLIP,
                                    0,
                                    0,
                                    DRAW_CLIP_WIDTH,
                                    DRAW_CLIP_HEIGHT,
                                    0
                                );
                            }
                            IconToBitmap(
                                m_heroIcons[H2EnumIndex(s_drawHeroType)],
                                gpWindowManager->m_screen,
                                s_drawPixelX,
                                s_drawPixelY + s_drawHeroYOffset + CELL_LAST_PIXEL,
                                s_drawHeroFrame,
                                ICON_DRAW_CLIP,
                                0,
                                0,
                                DRAW_CLIP_WIDTH,
                                DRAW_CLIP_HEIGHT,
                                0
                            );
                            if (s_drawPlayerColor != -1) {
                                if (s_drawHeroType == HERO_TYPE_BOAT) {
                                    IconToBitmap(
                                        m_boatFlagIcons[s_drawPlayerColor],
                                        gpWindowManager->m_screen,
                                        s_drawPixelX,
                                        s_drawPixelY + s_drawHeroYOffset + CELL_LAST_PIXEL,
                                        s_drawHeroFrame & HERO_FRAME_INDEX_MASK,
                                        ICON_DRAW_CLIP,
                                        0,
                                        0,
                                        DRAW_CLIP_WIDTH,
                                        DRAW_CLIP_HEIGHT,
                                        0
                                    );
                                } else {
                                    IconToBitmap(
                                        m_flagIcons[s_drawPlayerColor],
                                        gpWindowManager->m_screen,
                                        s_drawPixelX,
                                        s_drawPixelY + CELL_LAST_PIXEL,
                                        (s_drawHeroFrame & HERO_FRAME_INDEX_MASK)
                                            + m_updateMaxY % PLAYER_FLAG_FRAME_COUNT
                                            + PLAYER_FLAG_FRAME_BASE,
                                        ICON_DRAW_CLIP,
                                        0,
                                        0,
                                        DRAW_CLIP_WIDTH,
                                        DRAW_CLIP_HEIGHT,
                                        0
                                    );
                                }
                            }
                        }
                    }
                }

                if (m_cursorActive != 0 && (s_drawCell->m_flags & CURSOR_MAP_VISIBLE_FLAG) != 0
                    && (m_comboHeroDrawn == 0 || (H2EnumIndex((drawMask) & (ADVMGR_DRAW_HERO_SHADOW))))
                    && mapX == m_mapOriginX + VIEW_CENTER_CELL
                    && mapY == m_mapOriginY + VIEW_CENTER_CELL) {
                    if ((H2EnumIndex((drawMask) & (ADVMGR_DRAW_HERO_SHADOW)))) {
                        cursorSuppressed = 1;
                    } else {
                        DrawCursorShadow();
                        DrawCursor();
                        m_comboHeroDrawn = 1;
                    }
                }
            }

            if ((H2EnumIndex((drawMask) & (ADVMGR_DRAW_OVERLAY))) || (H2EnumIndex((drawMask) & (ADVMGR_DRAW_OVERLAY_TOP)))) {
                if ((H2EnumIndex((drawMask) & (ADVMGR_DRAW_OVERLAY)))
                    && s_drawCell->m_objectIndex != MAPCELL_SPRITE_NONE) {
                    if (s_drawCell->m_objectDrawnAsOverlay
                        && (gbDrawingPuzzle == 0
                            || bPuzzleDraw[H2EnumIndex(s_drawCell->m_objectTileset)])) {
                        IconToBitmap(
                            m_objectIcons[H2EnumIndex(s_drawCell->m_objectTileset)],
                            gpWindowManager->m_screen,
                            s_drawPixelX,
                            s_drawPixelY,
                            s_drawCell->m_objectIndex,
                            ICON_DRAW_NO_CLIP,
                            0,
                            0,
                            DRAW_CLIP_WIDTH,
                            DRAW_CLIP_HEIGHT,
                            0
                        );
                        if (s_drawCell->m_animatedObject) {
                            s_drawAnimationLength = GetIconEntry(
                                                        m_objectIcons
                                                            [H2EnumIndex(s_drawCell->m_objectTileset)],
                                                        s_drawCell->m_objectIndex
                            )
                                                        ->flags;
                            IconToBitmap(
                                m_objectIcons[H2EnumIndex(s_drawCell->m_objectTileset)],
                                gpWindowManager->m_screen,
                                s_drawPixelX,
                                s_drawPixelY,
                                s_drawCell->m_objectIndex
                                    + m_updateMaxY % s_drawAnimationLength + 1,
                                ICON_DRAW_NO_CLIP,
                                0,
                                0,
                                DRAW_CLIP_WIDTH,
                                DRAW_CLIP_HEIGHT,
                                0
                            );
                        }
                    }

                    if (s_drawCell->m_extraIndex != 0
                        && m_mapData->Extra(s_drawCell->m_extraIndex)->objectIndex
                               != MAPCELL_SPRITE_NONE) {
                        s_drawExtra = m_mapData->Extra(s_drawCell->m_extraIndex);
                    } else {
                        s_drawExtra = NULL;
                    }
                    while (s_drawExtra != NULL) {
                        if (s_drawExtra->objectDrawnAsOverlay
                            && (gbDrawingPuzzle == 0
                                || bPuzzleDraw[H2EnumIndex(s_drawExtra->objectTileset)])) {
                            IconToBitmap(
                                m_objectIcons[H2EnumIndex(s_drawExtra->objectTileset)],
                                gpWindowManager->m_screen,
                                s_drawPixelX,
                                s_drawPixelY,
                                s_drawExtra->objectIndex,
                                ICON_DRAW_NO_CLIP,
                                0,
                                0,
                                DRAW_CLIP_WIDTH,
                                DRAW_CLIP_HEIGHT,
                                0
                            );
                            if (s_drawExtra->animatedObject) {
                                s_drawAnimationLength =
                                    GetIconEntry(
                                        m_objectIcons[H2EnumIndex(s_drawExtra->objectTileset)],
                                        s_drawExtra->objectIndex
                                    )
                                        ->flags;
                                IconToBitmap(
                                    m_objectIcons[H2EnumIndex(s_drawExtra->objectTileset)],
                                    gpWindowManager->m_screen,
                                    s_drawPixelX,
                                    s_drawPixelY,
                                    s_drawExtra->objectIndex
                                        + m_updateMaxY % s_drawAnimationLength + 1,
                                    ICON_DRAW_NO_CLIP,
                                    0,
                                    0,
                                    DRAW_CLIP_WIDTH,
                                    DRAW_CLIP_HEIGHT,
                                    0
                                );
                            }
                        }
                        if (s_drawExtra->nextIndex != 0
                            && m_mapData->Extra(s_drawExtra->nextIndex)->objectIndex
                                   != MAPCELL_SPRITE_NONE) {
                            s_drawExtra = m_mapData->Extra(s_drawExtra->nextIndex);
                        } else {
                            s_drawExtra = NULL;
                        }
                    }
                }

                if (s_drawCell->m_overlayIndex != MAPCELL_SPRITE_NONE
                    && (((H2EnumIndex((drawMask) & (ADVMGR_DRAW_OVERLAY))) && !s_drawCell->m_drawOverlayOnTop)
                        || ((H2EnumIndex((drawMask) & (ADVMGR_DRAW_OVERLAY_TOP)))
                            && s_drawCell->m_drawOverlayOnTop))
                    && (gbDrawingPuzzle == 0 || bPuzzleDraw[H2EnumIndex(s_drawCell->m_overlayTileset)])) {
                    IconToBitmap(
                        m_objectIcons[H2EnumIndex(s_drawCell->m_overlayTileset)],
                        gpWindowManager->m_screen,
                        s_drawPixelX,
                        s_drawPixelY,
                        s_drawCell->m_overlayIndex,
                        s_drawCell->m_overlayTileset == TILESET_FLAG32 ? ICON_DRAW_CLIP
                                                                       : ICON_DRAW_NO_CLIP,
                        0,
                        0,
                        DRAW_CLIP_WIDTH,
                        DRAW_CLIP_HEIGHT,
                        0
                    );
                    if (s_drawCell->m_animatedOverlay) {
                        s_drawAnimationLength =
                            GetIconEntry(
                                m_objectIcons[H2EnumIndex(s_drawCell->m_overlayTileset)],
                                s_drawCell->m_overlayIndex
                            )
                                ->flags;
                        IconToBitmap(
                            m_objectIcons[H2EnumIndex(s_drawCell->m_overlayTileset)],
                            gpWindowManager->m_screen,
                            s_drawPixelX,
                            s_drawPixelY,
                            s_drawCell->m_overlayIndex + m_updateMaxY % s_drawAnimationLength + 1,
                            ICON_DRAW_NO_CLIP,
                            0,
                            0,
                            DRAW_CLIP_WIDTH,
                            DRAW_CLIP_HEIGHT,
                            0
                        );
                    }
                }

                if (s_drawCell->m_extraIndex != 0
                    && m_mapData->Extra(s_drawCell->m_extraIndex)->overlayIndex
                           != MAPCELL_SPRITE_NONE) {
                    s_drawExtra = m_mapData->Extra(s_drawCell->m_extraIndex);
                } else {
                    s_drawExtra = NULL;
                }
                while (s_drawExtra != NULL) {
                    if (((H2EnumIndex((drawMask) & (ADVMGR_DRAW_OVERLAY))) && !s_drawExtra->drawOverlayOnTop)
                        || ((H2EnumIndex((drawMask) & (ADVMGR_DRAW_OVERLAY_TOP)))
                            && s_drawExtra->drawOverlayOnTop)) {
                        if (gbDrawingPuzzle == 0 || bPuzzleDraw[H2EnumIndex(s_drawExtra->overlayTileset)]) {
                            IconToBitmap(
                                m_objectIcons[H2EnumIndex(s_drawExtra->overlayTileset)],
                                gpWindowManager->m_screen,
                                s_drawPixelX,
                                s_drawPixelY,
                                s_drawExtra->overlayIndex,
                                s_drawExtra->overlayTileset == TILESET_FLAG32 ? ICON_DRAW_CLIP
                                                                              : ICON_DRAW_NO_CLIP,
                                0,
                                0,
                                DRAW_CLIP_WIDTH,
                                DRAW_CLIP_HEIGHT,
                                0
                            );
                            if (s_drawExtra->animatedOverlay) {
                                s_drawAnimationLength =
                                    GetIconEntry(
                                        m_objectIcons[H2EnumIndex(s_drawExtra->overlayTileset)],
                                        s_drawExtra->overlayIndex
                                    )
                                        ->flags;
                                IconToBitmap(
                                    m_objectIcons[H2EnumIndex(s_drawExtra->overlayTileset)],
                                    gpWindowManager->m_screen,
                                    s_drawPixelX,
                                    s_drawPixelY,
                                    s_drawExtra->overlayIndex + m_updateMaxY % s_drawAnimationLength
                                        + 1,
                                    ICON_DRAW_NO_CLIP,
                                    0,
                                    0,
                                    DRAW_CLIP_WIDTH,
                                    DRAW_CLIP_HEIGHT,
                                    0
                                );
                            }
                        }
                    }
                    if (s_drawExtra->nextIndex != 0
                        && m_mapData->Extra(s_drawExtra->nextIndex)->overlayIndex
                               != MAPCELL_SPRITE_NONE) {
                        s_drawExtra = m_mapData->Extra(s_drawExtra->nextIndex);
                    } else {
                        s_drawExtra = NULL;
                    }
                }
            }
        }
    }
}

class mapCell* advManager::GetCell(i32 x, i32 y) {
    if (x < 0 || y < 0 || x >= MAP_WIDTH || y >= MAP_HEIGHT) {
        return m_mapData->GetCell(0, 0);
    } else {
        return m_mapData->GetCell(x, y);
    }
}

void advManager::UpdateRadar(i32 updateScreen, i32 partial) {
    u8* line;
    u8* pixPtr;
    u8 color = RADAR_UNSEEN_COLOR;
    i32 xrem;
    i32 ymod;
    i32 w;
    i32 offX;
    i32 delta;
    float fScale;
    i32 townx;
    i32 towny;
    i32 frame;
    i32 oldColor;
    i32 bNoFrame;
    i32 i;
    i32 j;
    i32 cx;
    i32 minx;
    i32 miny;
    i32 xhi;
    i32 yhi;
    mapCell* cell;
    H2EnumStorage<TilesetId, i32> setId;
    i32 owner;

    if (partial == 0) {
        minx = 0;
        miny = 0;
        xhi = MAP_WIDTH - 1;
        yhi = MAP_HEIGHT - 1;
    } else {
        minx = m_mapOriginX - RADAR_PARTIAL_MARGIN;
        miny = m_mapOriginY - RADAR_PARTIAL_MARGIN;
        xhi = m_mapOriginX + RADAR_PARTIAL_SPAN;
        yhi = m_mapOriginY + RADAR_PARTIAL_SPAN;
        if (minx < 0) {
            minx = 0;
        }
        if (miny < 0) {
            miny = 0;
        }
        if (xhi > MAP_WIDTH - 1) {
            xhi = MAP_WIDTH - 1;
        }
        if (yhi > MAP_HEIGHT - 1) {
            yhi = MAP_HEIGHT - 1;
        }
    }

    if (gbThisNetHumanPlayer[giCurPlayer] == 0) {
        return;
    }

    gpAdvManager->m_openState = 0;
    xrem = ymod = 0;
    switch (MAP_HEIGHT) {
        case MAP_DIMENSION_SMALL:
            line = gpWindowManager->m_screen->m_pixels
                   + (miny * RADAR_SMALL_CELL_PIXELS + RADAR_TOP) * RADAR_SCREEN_PITCH
                   + RADAR_LEFT;
            offX = minx * RADAR_SMALL_CELL_PIXELS;
            break;
        case MAP_DIMENSION_MEDIUM:
            line = gpWindowManager->m_screen->m_pixels
                   + (miny * RADAR_MEDIUM_CELL_PIXELS + RADAR_TOP) * RADAR_SCREEN_PITCH
                   + RADAR_LEFT;
            offX = minx * RADAR_MEDIUM_CELL_PIXELS;
            break;
        case MAP_DIMENSION_LARGE:
            line = gpWindowManager->m_screen->m_pixels
                   + (miny + (miny + RADAR_LARGE_SCALE_ROUNDING) / RADAR_LARGE_SCALE_DIVISOR
                      + RADAR_TOP)
                         * RADAR_SCREEN_PITCH
                   + RADAR_LEFT;
            offX = minx + (minx + RADAR_LARGE_SCALE_ROUNDING) / RADAR_LARGE_SCALE_DIVISOR;
            xrem = minx % RADAR_LARGE_SCALE_DIVISOR;
            ymod = miny % RADAR_LARGE_SCALE_DIVISOR;
            break;
        default:
            line = gpWindowManager->m_screen->m_pixels + (miny + RADAR_TOP) * RADAR_SCREEN_PITCH
                   + RADAR_LEFT;
            offX = minx;
            break;
    }

    for (j = miny; j <= yhi; ++j) {
        pixPtr = line + offX;
        switch (MAP_HEIGHT) {
            case MAP_DIMENSION_SMALL:
                line += RADAR_SCREEN_PITCH * RADAR_SMALL_CELL_PIXELS;
                break;
            case MAP_DIMENSION_MEDIUM:
                line += RADAR_SCREEN_PITCH * RADAR_MEDIUM_CELL_PIXELS;
                break;
            case MAP_DIMENSION_LARGE:
                ++ymod;
                if (ymod > RADAR_LARGE_SCALE_DIVISOR - 1) {
                    ymod = 0;
                }
                if (ymod != 0) {
                    line += RADAR_SCREEN_PITCH;
                } else {
                    line += RADAR_SCREEN_PITCH * RADAR_MEDIUM_CELL_PIXELS;
                }
                break;
            case MAP_DIMENSION_XLARGE:
                line += RADAR_SCREEN_PITCH;
                break;
        }

        for (i = minx; i <= xhi; ++i) {
            if (gbAllBlack != 0 || (MAP_EXTRA_AT_WFIRST(i, j) & giCurPlayerBit) == 0) {
                color = RADAR_UNSEEN_COLOR;
            } else {
                cell = m_mapData->GetCell(i, j);
                if ((cell->m_flags & CURSOR_MAP_VISIBLE_FLAG) != 0
                    && i == m_mapOriginX + RADAR_CURRENT_CELL
                    && j == m_mapOriginY + RADAR_CURRENT_CELL) {
                    color = gOwnerColors[gpGame->m_players[giCurPlayer].m_color];
                } else {
                    if ((cell->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                        == MAP_OBJECT_HERO_INTERACTION) {
                        owner = gpGame->m_availableHeroes[cell->m_objectMetadata];
                        if (owner == giCurPlayer) {
                            color = gOwnerColors
                                [owner < 0 ? RADAR_NEUTRAL_OWNER
                                           : gpGame->m_players[owner].m_color];
                        }
                    } else {
                        setId = -1;
                        if (cell->m_objectIndex != MAPCELL_SPRITE_NONE) {
                            setId = cell->m_objectTileset;
                        } else if (cell->m_overlayIndex != MAPCELL_SPRITE_NONE) {
                            setId = cell->m_overlayTileset;
                        }

                        if (cell->m_triggerType == MAP_OBJECT_CASTLE
                            || (setId == TILESET_FLAG32 && i > 0
                                && i < MAP_WIDTH - 1
                                && m_mapData->GetCell(i - 1, j)->m_triggerType
                                       == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE))
                            || m_mapData->GetCell(i + 1, j)->m_triggerType
                                   == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
                            setId = TILESET_OBJNTOWN;
                        }

                        if (setId == TILESET_X_LOC2
                            && cell->m_triggerType == MAP_OBJECT_ROCK) {
                            color =
                                gMapColors[H2EnumIndex(giGroundToTerrain[cell->m_terrainImageIndex])]
                                + RADAR_TERRAIN_SHADE;
                        } else {
                            switch (setId) {
                                case TILESET_OBJNTOWN:
                                case TILESET_OBJNTWBA:
                                    owner = gpGame->m_townOwners[cell->m_objectMetadata];
                                    townx = gpGame->m_castleRecs[cell->m_objectMetadata].m_x;
                                    towny = gpGame->m_castleRecs[cell->m_objectMetadata].m_y;
                                    color = gOwnerColors
                                        [owner < 0 ? RADAR_NEUTRAL_OWNER
                                                   : gpGame->m_players[owner].m_color];
                                    if (j < towny - RADAR_TOWN_RADIUS || j > towny
                                        || i < townx - RADAR_TOWN_RADIUS
                                        || i > townx + RADAR_TOWN_RADIUS) {
                                        goto radar_default_object;
                                    }
                                    break;
                                case TILESET_MTNSNOW:
                                case TILESET_MTNSWMP:
                                case TILESET_MTNLAVA:
                                case TILESET_MTNDSRT:
                                case TILESET_MTNDIRT:
                                case TILESET_MTNMULT:
                                case TILESET_MTNCRCK:
                                case TILESET_MTNGRAS:
                                case TILESET_TREJNGL:
                                case TILESET_TREEVIL:
                                case TILESET_TRESNOW:
                                case TILESET_TREFIR:
                                case TILESET_TREFALL:
                                case TILESET_TREDECI:
                                    switch (cell->m_triggerType) {
                                        case MAP_OBJECT_ALCHEMIST_LAB:
                                        case MAP_OBJECT_MINE:
                                        case MAP_OBJECT_SAWMILL:
                                        case (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB):
                                        case (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE):
                                        case (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SAWMILL):
                                            owner = gpGame->m_mineOwners[cell->m_objectMetadata];
                                            color = gOwnerColors
                                                [owner < 0
                                                     ? RADAR_NEUTRAL_OWNER
                                                     : gpGame->m_players[owner].m_color];
                                            break;
                                        default:
                                            color =
                                                gMapColors[H2EnumIndex(giGroundToTerrain[cell
                                                                          ->m_terrainImageIndex])]
                                                + RADAR_TERRAIN_SHADE;
                                            break;
                                    }
                                    break;
                                default:
                                radar_default_object:
                                    switch (cell->m_triggerType) {
                                        case MAP_OBJECT_ALCHEMIST_LAB:
                                        case MAP_OBJECT_MINE:
                                        case MAP_OBJECT_SAWMILL:
                                        case (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB):
                                        case (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE):
                                        case (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SAWMILL):
                                            owner = gpGame->m_mineOwners[cell->m_objectMetadata];
                                            color = gOwnerColors
                                                [owner < 0
                                                     ? RADAR_NEUTRAL_OWNER
                                                     : gpGame->m_players[owner].m_color];
                                            break;
                                        default:
                                            color = gMapColors[H2EnumIndex(giGroundToTerrain[cell->m_terrainImageIndex])];
                                            break;
                                    }
                                    break;
                            }
                        }
                    }
                }
            }

            switch (MAP_HEIGHT) {
                case MAP_DIMENSION_SMALL:
                    memset(pixPtr, color, RADAR_SMALL_CELL_PIXELS);
                    memset(
                        pixPtr + RADAR_SCREEN_PITCH,
                        color,
                        RADAR_SMALL_CELL_PIXELS
                    );
                    memset(
                        pixPtr + RADAR_SCREEN_PITCH * RADAR_MEDIUM_CELL_PIXELS,
                        color,
                        RADAR_SMALL_CELL_PIXELS
                    );
                    memset(
                        pixPtr + RADAR_SCREEN_PITCH * (RADAR_SMALL_CELL_PIXELS - 1),
                        color,
                        RADAR_SMALL_CELL_PIXELS
                    );
                    pixPtr += RADAR_SMALL_CELL_PIXELS;
                    break;
                case MAP_DIMENSION_MEDIUM:
                    memset(pixPtr, color, RADAR_MEDIUM_CELL_PIXELS);
                    memset(
                        pixPtr + RADAR_SCREEN_PITCH,
                        color,
                        RADAR_MEDIUM_CELL_PIXELS
                    );
                    pixPtr += RADAR_MEDIUM_CELL_PIXELS;
                    break;
                case MAP_DIMENSION_LARGE:
                    if (xrem != 0) {
                        if (ymod != 0) {
                            pixPtr[0] = color;
                            ++pixPtr;
                        } else {
                            pixPtr[0] = color;
                            pixPtr[RADAR_SCREEN_PITCH] = color;
                            ++pixPtr;
                        }
                    } else if (ymod != 0) {
                        pixPtr[0] = color;
                        pixPtr[1] = color;
                        pixPtr += RADAR_MEDIUM_CELL_PIXELS;
                    } else {
                        pixPtr[0] = color;
                        pixPtr[1] = color;
                        pixPtr[RADAR_SCREEN_PITCH] = color;
                        pixPtr[RADAR_SCREEN_PITCH + 1] = color;
                        pixPtr += RADAR_MEDIUM_CELL_PIXELS;
                    }
                    ++xrem;
                    if (xrem > RADAR_LARGE_SCALE_DIVISOR - 1) {
                        xrem = 0;
                    }
                    break;
                case MAP_DIMENSION_XLARGE:
                    *pixPtr++ = color;
                    break;
            }
        }
    }

    frame = RADAR_FRAME_NONE;
    bNoFrame = 0;
    if (gbInViewWorld != 0) {
        switch (MAP_HEIGHT) {
            case MAP_DIMENSION_SMALL:
                fScale = RADAR_SMALL_CELL_PIXELS;
                bNoFrame = 1;
                break;
            case MAP_DIMENSION_MEDIUM:
                fScale = RADAR_MEDIUM_CELL_PIXELS;
                if (giViewWorldScale <= VIEW_WORLD_SCALE_MIDDLE) {
                    bNoFrame = 1;
                } else {
                    frame = RADAR_FRAME_VIEW_MIDDLE;
                }
                break;
            case MAP_DIMENSION_LARGE:
                fScale = 1.33f;
                if (giViewWorldScale <= VIEW_WORLD_SCALE_FAR) {
                    bNoFrame = 1;
                } else if (giViewWorldScale == VIEW_WORLD_SCALE_MIDDLE) {
                    frame = RADAR_FRAME_VIEW_MIDDLE_LARGE;
                } else {
                    frame = RADAR_FRAME_VIEW_NEAR_LARGE;
                }
                break;
            default:
                fScale = 1.0f;
                if (giViewWorldScale == VIEW_WORLD_SCALE_FAR) {
                    frame = RADAR_FRAME_VIEW_FAR_XLARGE;
                } else if (giViewWorldScale == VIEW_WORLD_SCALE_MIDDLE) {
                    frame = RADAR_FRAME_VIEW_MIDDLE;
                } else {
                    frame = RADAR_FRAME_VIEW_NEAR_XLARGE;
                }
                break;
        }
    } else {
        switch (MAP_HEIGHT) {
            case MAP_DIMENSION_SMALL:
                frame = RADAR_FRAME_NORMAL_SMALL;
                fScale = RADAR_SMALL_CELL_PIXELS;
                break;
            case MAP_DIMENSION_MEDIUM:
                frame = RADAR_FRAME_NORMAL_MEDIUM;
                fScale = RADAR_MEDIUM_CELL_PIXELS;
                break;
            case MAP_DIMENSION_LARGE:
                frame = RADAR_FRAME_NORMAL_LARGE;
                fScale = 1.33f;
                break;
            default:
                frame = RADAR_FRAME_NORMAL_XLARGE;
                fScale = 1.0f;
                break;
        }
    }

    if (bNoFrame == 0) {
        if (gbInViewWorld != 0) {
            m_puzzleIcon->ClipFillToBuffer(
                static_cast<i32>(iVWMapOriginX * fScale + ADVMGR_RADAR_LEFT_FLOAT),
                static_cast<i32>(iVWMapOriginY * fScale + ADVMGR_RADAR_TOP_FLOAT),
                frame,
                RADAR_VIEWPORT_COLOR,
                ICON_DRAW_NORMAL,
                RADAR_LEFT,
                RADAR_TOP,
                RADAR_SIZE,
                RADAR_SIZE
            );
        } else {
            m_puzzleIcon->ClipFillToBuffer(
                static_cast<i32>(m_mapOriginX * fScale + ADVMGR_RADAR_LEFT_FLOAT),
                static_cast<i32>(m_mapOriginY * fScale + ADVMGR_RADAR_TOP_FLOAT),
                frame,
                RADAR_VIEWPORT_COLOR,
                ICON_DRAW_NORMAL,
                RADAR_LEFT,
                RADAR_TOP,
                RADAR_SIZE,
                RADAR_SIZE
            );
        }
    }

    if (updateScreen != 0) {
        if (partial != 0) {
            gpWindowManager->UpdateScreenRegion(
                static_cast<i32>(minx * fScale + ADVMGR_RADAR_LEFT_FLOAT),
                static_cast<i32>(miny * fScale + ADVMGR_RADAR_TOP_FLOAT),
                static_cast<i32>((xhi - minx + 1) * fScale),
                static_cast<i32>((yhi - miny + 1) * fScale)
            );
        } else {
            gpWindowManager->UpdateScreenRegion(RADAR_LEFT, RADAR_TOP, RADAR_SIZE, RADAR_SIZE);
        }
    }
}

void advManager::QuickInfo(i32 cellX, i32 cellY) {
    HeroEventFlag visitedMaskValue;
    TilesetId icn;
    mapCell* currentCell;
    i32 posX;
    heroWindow* pWin;
    i32 iFrame;
    hero* pHero;
    i32 posY;
    char guardStr[QUICK_INFO_TEXT_CAPACITY];
    char savedTextLocal[QUICK_INFO_SAVED_TEXT_CAPACITY];
    tag_message message;
    char ch;
    i32 quickInfoShowFlag;
    i32 j;
    i32 expansionSite;
    char uppercaseResult;
    i32 blocked;

    quickInfoShowFlag = 1;
    currentCell = NULL;
    pHero = NULL;
    if (gpCurPlayer->CurrentHero() != -1) {
        pHero = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];
    } else {
        pHero = NULL;
    }

    posX = cellX * CELL_PIXELS - QUICK_INFO_X_OFFSET;
    if (posX < QUICK_INFO_MIN_X) {
        posX = QUICK_INFO_MIN_X;
    }
    if (posX + QUICK_INFO_WIDTH > QUICK_INFO_RIGHT) {
        posX = QUICK_INFO_RIGHT_X;
    }

    posY = cellY * CELL_PIXELS - QUICK_INFO_Y_OFFSET;
    if (posY < QUICK_INFO_MIN_Y) {
        posY = QUICK_INFO_MIN_Y;
    }
    if (posY + QUICK_INFO_HEIGHT > QUICK_INFO_BOTTOM) {
        posY = QUICK_INFO_BOTTOM_Y;
    }

    pWin = new heroWindow(
        posX,
        posY,
        "qwikinfo.bin"
    );
    if (pWin == NULL) {
        MemError();
    }
    visitedMaskValue = HERO_EVENT_NONE;

    if (m_mapOriginX + cellX < 0 || m_mapOriginX + cellX >= MAP_WIDTH || m_mapOriginY + cellY < 0
        || m_mapOriginY + cellY >= MAP_HEIGHT) {
        sprintf(
            gText,
            "%s",
            "\xc3\xf0\xe0\xed\xe8\xf6\xe0"
        );
    } else {
        currentCell = GetCell(m_mapOriginX + cellX, m_mapOriginY + cellY);
        if ((MAP_EXTRA_AT_WFIRST((m_mapOriginX + cellX), m_mapOriginY + cellY)
             & giCurPlayerBit)
            == 0) {
            sprintf(
                gText,
                "%s",
                "\xcd\xe5\xe8\xe7\xf3\xf7\xe5\xed\xed\xe0\xff \xf2\xe5\xf0\xf0\xe8\xf2\xee\xf0\xe8\xff"
            );
        } else {

            switch (currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
                case MAP_OBJECT_ARTIFACT:
                    sprintf(
                        gText,
                        "%s",
                        "\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2"
                    );
                    break;
                case MAP_OBJECT_OBELISK:
                    if (H2EnumIndex((currentCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG))) {
                        sprintf(
                            gText,
                            "%s\n\n%s",
                            gQuickViewText[H2EnumIndex(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (gpGame->m_obeliskVisitors
                                 [currentCell->m_objectMetadata - OBELISK_INDEX_BASE]
                             & (1u << giCurPlayer))
                                ? "(\xd3\xe6\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                                : "(\xdd\xf2\xee \xec\xe5\xf1\xf2\xee \xe5\xf9\xe5 \xed\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_GAZEBO:
                    if (pHero != NULL
                        && (H2EnumIndex((currentCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                        sprintf(
                            gText,
                            "%s\n\n%s",
                            gQuickViewText[H2EnumIndex(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (pHero->m_gazeboVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? "(\xd3\xe6\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                                : "(\xdd\xf2\xee \xec\xe5\xf1\xf2\xee \xe5\xf9\xe5 \xed\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_FORT:
                    if (pHero != NULL
                        && (H2EnumIndex((currentCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                        sprintf(
                            gText,
                            "%s\n\n%s",
                            gQuickViewText[H2EnumIndex(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (pHero->m_fortVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? "(\xd3\xe6\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                                : "(\xdd\xf2\xee \xec\xe5\xf1\xf2\xee \xe5\xf9\xe5 \xed\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_WITCH_DOCTOR_HUT:
                    if (pHero != NULL
                        && (H2EnumIndex((currentCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                        sprintf(
                            gText,
                            "%s\n\n%s",
                            gQuickViewText[H2EnumIndex(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (pHero->m_witchDoctorVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? "(\xd3\xe6\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                                : "(\xdd\xf2\xee \xec\xe5\xf1\xf2\xee \xe5\xf9\xe5 \xed\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_MERCENARY_CAMP:
                    if (pHero != NULL
                        && (H2EnumIndex((currentCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                        sprintf(
                            gText,
                            "%s\n\n%s",
                            gQuickViewText[H2EnumIndex(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (pHero->m_mercenaryCampVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? "(\xd3\xe6\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                                : "(\xdd\xf2\xee \xec\xe5\xf1\xf2\xee \xe5\xf9\xe5 \xed\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_STANDING_STONES:
                    if (pHero != NULL
                        && (H2EnumIndex((currentCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                        sprintf(
                            gText,
                            "%s\n\n%s",
                            gQuickViewText[H2EnumIndex(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (pHero->m_standingStoneVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? "(\xd3\xe6\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                                : "(\xdd\xf2\xee \xec\xe5\xf1\xf2\xee \xe5\xf9\xe5 \xed\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_TREE_OF_KNOWLEDGE:
                    if (pHero != NULL
                        && (H2EnumIndex((currentCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                        sprintf(
                            gText,
                            "%s\n\n%s",
                            gQuickViewText[H2EnumIndex(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (pHero->m_treeKnowledgeVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? "(\xd3\xe6\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                                : "(\xdd\xf2\xee \xec\xe5\xf1\xf2\xee \xe5\xf9\xe5 \xed\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_XANADU:
                    if (pHero != NULL
                        && (H2EnumIndex((currentCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                        sprintf(
                            gText,
                            "%s\n\n%s",
                            gQuickViewText[H2EnumIndex(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (pHero->m_xanaduVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? "(\xd3\xe6\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                                : "(\xdd\xf2\xee \xec\xe5\xf1\xf2\xee \xe5\xf9\xe5 \xed\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_BUOY:
                    visitedMaskValue = ADVMGR_VISIT_FORT;
                    goto quick_info_default;
                case MAP_OBJECT_FOUNTAIN:
                    visitedMaskValue = ADVMGR_VISIT_GAZEBO;
                    goto quick_info_default;
                case MAP_OBJECT_OASIS:
                    visitedMaskValue = ADVMGR_VISIT_MERCENARY_CAMP;
                    goto quick_info_default;
                case MAP_OBJECT_FAERIE_RING:
                    visitedMaskValue = ADVMGR_VISIT_STANDING_STONES;
                    goto quick_info_default;
                case MAP_OBJECT_TEMPLE:
                    visitedMaskValue = ADVMGR_VISIT_WITCH_DOCTOR;
                    goto quick_info_default;
                case MAP_OBJECT_WATERING_HOLE:
                    visitedMaskValue = ADVMGR_VISIT_EVENT_SITE;
                    goto quick_info_default;
                case MAP_OBJECT_MAGIC_WELL:
                    visitedMaskValue = ADVMGR_VISIT_XANADU;
                    goto quick_info_default;
                case MAP_OBJECT_IDOL:
                    visitedMaskValue = ADVMGR_VISIT_TREE_OF_KNOWLEDGE;
                    goto quick_info_default;
                case MAP_OBJECT_NONE:
                case MAP_OBJECT_MAP_EVENT:
                case MAP_OBJECT_COAST:
                case MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT:
                case MAP_OBJECT_MOSSY_ROCK:
                    if ((currentCell->m_objectIndex != MAPCELL_SPRITE_NONE
                         && currentCell->m_objectTileset != TILESET_DUMMY)
                        || currentCell->m_overlayIndex != MAPCELL_SPRITE_NONE
                        || giGroundToTerrain[currentCell->m_terrainImageIndex] == TERRAIN_WATER) {
                        blocked = 1;
                    } else {
                        blocked = 0;
                    }
                    sprintf(
                        gText,
                        "%s\n%s",
                        gTerrainNames[H2EnumIndex(giGroundToTerrain[currentCell->m_terrainImageIndex])],
                        blocked
                            ? "(\xed\xe5\xeb\xfc\xe7\xff \xea\xee\xef\xe0\xf2\xfc)"
                            : "(\xec\xee\xe6\xed\xee \xea\xee\xef\xe0\xf2\xfc)"
                    );
                    break;
                case MAP_OBJECT_ABANDONED_MINE:
                    sprintf(
                        gText,
                        "%s",
                        gQuickViewText[H2EnumIndex(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)]
                    );
                    goto quick_info_guarded;
                case MAP_OBJECT_MINE:
                    if (gpGame->m_mines[currentCell->m_objectMetadata].guardianType
                        != CREATURE_NONE) {
                        sprintf(
                            gText,
                            "%s",
                            gMineNames[H2EnumIndex(gpGame->m_mines[currentCell->m_objectMetadata]
                                               .resourceType)]
                        );
                    quick_info_guarded:
                        sprintf(
                            guardStr,
                            "\n\n\xee\xf5\xf0\xe0\xed\xff\xfe\xf2 %s %s",
                            GetArmySizeName(
                                gpGame->m_mines[currentCell->m_objectMetadata].guardianCount,
                                ARMY_SIZE_NAME_INLINE
                            ),
                            gArmyNamesPlural[H2EnumIndex(gpGame->m_mines[currentCell->m_objectMetadata]
                                                     .guardianType)]
                        );
                        strcat(gText, guardStr);
                    } else {
                        sprintf(
                            gText,
                            "%s",
                            gMineNames[H2EnumIndex(gpGame->m_mines[currentCell->m_objectMetadata]
                                               .resourceType)]
                        );
                    }
                    break;
                case MAP_OBJECT_RESOURCE:
                    sprintf(
                        gText,
                        "%s",
                        gResourceNames[currentCell->m_objectIndex / RESOURCE_FRAME_PAIR_STRIDE]
                    );
                    break;
                case MAP_OBJECT_MONSTER:
                    if (IsCrystalBallInEffect(
                            m_mapOriginX + cellX,
                            m_mapOriginY + cellY,
                            CRYSTAL_BALL_RADIUS
                        )) {
                        sprintf(
                            gText,
                            "%d %s",
                            currentCell->m_objectMetadata & H2EnumIndex(MAP_MONSTER_COUNT_MASK),
                            gArmyNamesPlural[currentCell->m_objectIndex]
                        );
                    } else {
                        sprintf(
                            gText,
                            "%s %s",
                            GetArmySizeName(
                                currentCell->m_objectMetadata & H2EnumIndex(MAP_MONSTER_COUNT_MASK),
                                ARMY_SIZE_NAME_SENTENCE
                            ),
                            gArmyNamesPlural[currentCell->m_objectIndex]
                        );
                    }
                    break;
                case MAP_OBJECT_BARRIER:
                case MAP_OBJECT_TRAVELER_TENT:
                    if ((H2EnumIndex((currentCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                        sprintf(
                            gText,
                            gQuickViewText[H2EnumIndex(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            xBarrierColor[currentCell->m_objectMetadata & BARRIER_COLOR_MASK]
                        );
                        uppercaseResult = static_cast<char>(
                            toupper(static_cast<i32>(static_cast<i8>(gText[0])))
                        );
                        gText[0] = uppercaseResult;
                    } else {
                        sprintf(
                            gText,
                            "%s",
                            gTerrainNames[H2EnumIndex(giGroundToTerrain[currentCell->m_terrainImageIndex])]
                        );
                    }
                    break;
                case MAP_OBJECT_EXPANSION_OBJECT: {
                    ch = -1;
                    if (currentCell->m_objectIndex != MAPCELL_SPRITE_NONE) {
                        iFrame = currentCell->m_objectIndex;
                        icn = currentCell->m_objectTileset;
                    } else {
                        iFrame = currentCell->m_overlayIndex;
                        icn = currentCell->m_overlayTileset;
                    }
                    expansionSite = H2EnumIndex(GENERIC_SITE_UNKNOWN);
                    switch (icn) {
                        case TILESET_X_LOC1:
                            if (iFrame < 0) {
                                break;
                            } else {
                                if (iFrame < GENERIC_SITE_1_END) {
                                    expansionSite = H2EnumIndex(GENERIC_SITE_ALCHEMIST_TOWER);
                                } else if (iFrame < GENERIC_SITE_2_END) {
                                    expansionSite = H2EnumIndex(GENERIC_SITE_ARENA);
                                    visitedMaskValue = ADVMGR_VISIT_GENERIC_HUT;
                                }
                            }
                            break;
                        case TILESET_X_LOC2:
                            if (iFrame < 0) {
                                break;
                            } else {
                                if (iFrame < GENERIC_ALTAR_END) {
                                    expansionSite = H2EnumIndex(GENERIC_SITE_STABLES);
                                    visitedMaskValue = ADVMGR_VISIT_GENERIC_ALTAR;
                                } else if (iFrame < GENERIC_UNUSED_END) {
                                    expansionSite = H2EnumIndex(GENERIC_SITE_UNKNOWN);
                                } else if (iFrame < GENERIC_TOWER_END) {
                                    expansionSite = H2EnumIndex(GENERIC_SITE_MERMAID);
                                    visitedMaskValue = ADVMGR_VISIT_GENERIC_TOWER;
                                } else if (iFrame < GENERIC_SPRING_END) {
                                    expansionSite = H2EnumIndex(GENERIC_SITE_SIRENS);
                                    visitedMaskValue = ADVMGR_VISIT_GENERIC_SPRING;
                                }
                            }
                            break;
                        case TILESET_X_LOC3:
                            if (iFrame < 0) {
                                break;
                            } else {
                                if (iFrame < GENERIC_SITE_3_SPLIT) {
                                    expansionSite = H2EnumIndex(GENERIC_SITE_HUT_OF_MAGI);
                                } else if (iFrame < GENERIC_SITE_3_END) {
                                    expansionSite = H2EnumIndex(GENERIC_SITE_EYE_OF_MAGI);
                                }
                            }
                            break;
                    }
                    if (expansionSite == H2EnumIndex(GENERIC_SITE_UNKNOWN)) {
                        sprintf(
                            gText,
                            "\xcd\xe5\xe8\xe7\xe2\xe5\xf1\xf2\xed\xee"
                        );
                    } else {
                        sprintf(gText, xGenericSiteNames[expansionSite]);
                    }
                    if (pHero != NULL && visitedMaskValue != HERO_EVENT_NONE) {
                        strcat(
                            gText,
                            "\n\n"
                        );
                        strcat(
                            gText,
                            (H2EnumIndex((pHero->m_eventFlags) & (visitedMaskValue)))
                                ? "(\xd3\xe6\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                                : "(\xdd\xf2\xee \xec\xe5\xf1\xf2\xee \xe5\xf9\xe5 \xed\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                        );
                    }
                    break;
                }
                case MAP_OBJECT_EXPANSION_DWELLING: {
                    if (currentCell->m_overlayIndex == MAPCELL_SPRITE_NONE) {
                        iFrame = currentCell->m_objectIndex;
                        icn = currentCell->m_objectTileset;
                    } else {
                        iFrame = currentCell->m_overlayIndex;
                        icn = currentCell->m_overlayTileset;
                    }
                    expansionSite = H2EnumIndex(RECRUITMENT_SITE_UNKNOWN);
                    switch (icn) {
                        case TILESET_X_LOC1:
                            if (iFrame < RECRUITMENT_START) {
                                break;
                            } else {
                                if (iFrame < RECRUITMENT_1_END) {
                                    expansionSite = H2EnumIndex(RECRUITMENT_SITE_BARROW_MOUNDS);
                                } else if (iFrame < RECRUITMENT_2_END) {
                                    expansionSite = H2EnumIndex(RECRUITMENT_SITE_EARTH_ALTAR);
                                } else if (iFrame < RECRUITMENT_3_END) {
                                    expansionSite = H2EnumIndex(RECRUITMENT_SITE_AIR_ALTAR);
                                } else if (iFrame < RECRUITMENT_4_END) {
                                    expansionSite = H2EnumIndex(RECRUITMENT_SITE_FIRE_ALTAR);
                                } else if (iFrame < RECRUITMENT_5_END) {
                                    expansionSite = H2EnumIndex(RECRUITMENT_SITE_WATER_ALTAR);
                                }
                            }
                            break;
                    }
                    if (expansionSite == H2EnumIndex(RECRUITMENT_SITE_UNKNOWN)) {
                        sprintf(
                            gText,
                            "\xcd\xe5\xe8\xe7\xe2\xe5\xf1\xf2\xed\xee"
                        );
                    } else {
                        sprintf(gText, xRecruitmentSiteNames[expansionSite]);
                    }
                    break;
                }
                case MAP_OBJECT_ROCK:
                    if (currentCell->m_objectTileset == TILESET_X_LOC2) {
                        sprintf(
                            gText,
                            "\xd0\xe8\xf4\xfb"
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                default:
                quick_info_default:
                    if (visitedMaskValue != HERO_EVENT_NONE && pHero != NULL) {
                        sprintf(
                            gText,
                            "%s\n\n%s",
                            gQuickViewText[H2EnumIndex(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (H2EnumIndex((pHero->m_eventFlags) & (visitedMaskValue)))
                                ? "(\xd3\xe6\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                                : "(\xdd\xf2\xee \xec\xe5\xf1\xf2\xee \xe5\xf9\xe5 \xed\xe5 \xef\xee\xf1\xe5\xf9\xe5\xed\xee)"
                        );
                    } else {
                        sprintf(
                            gText,
                            "%s",
                            gQuickViewText[H2EnumIndex(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)]
                        );
                    }
                    break;
            }
        }
    }

quick_info_ready:
    strcpy(savedTextLocal, gText);
    if (giDebugLevel > 0 && currentCell != NULL) {
        sprintf(
            gText,
            "gi%d obtile%d obi%d ot%d ei%d bl%d %s X%d Y%d",
            currentCell->m_terrainImageIndex,
            static_cast<i32>(currentCell->m_objectTileset),
            currentCell->m_objectIndex,
            H2EnumIndex(currentCell->m_triggerType),
            currentCell->m_objectMetadata,
            currentCell->m_flags & H2EnumIndex(MAP_CELL_OCCUPIED),
            savedTextLocal,
            m_mapOriginX + cellX,
            m_mapOriginY + cellY
        );
    }
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = 1;
    message.payload.widget.data.text = gText;
    pWin->BroadcastMessage(message);
    gpWindowManager->AddWindow(pWin, -1, 1);
    QuickViewWait();
    gpWindowManager->RemoveWindow(pWin);
    delete pWin;
}

void advManager::UpdateHeroLocator(i32 locatorSlot, i32 drawWindow, i32 updateScreen) {
    i32 widgetBase;
    i32 whichHero;
    i32 curHero;
    tag_message message;
    i32 i;
    i32 manaFr;
    hero* heroPtr;
    i32 moveFrm;

    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        return;
    }

    if (locatorSlot == -1) {
        curHero = gpCurPlayer->m_currentHero;
        DebugCheck();
        if (curHero == INVALID_HERO) {
            return;
        }
        for (i = 0; i < LOCATOR_VISIBLE_COUNT; ++i) {
            if (curHero == gpCurPlayer->m_heroIds[gpCurPlayer->m_heroLocatorPage + i]) {
                locatorSlot = i;
            }
        }
        if (locatorSlot == -1) {
            return;
        }
    }

    widgetBase = locatorSlot * LOCATOR_HERO_WIDGET_STRIDE + LOCATOR_HERO_WIDGET_BASE;
    message.type = ADVMGR_LOCATOR_MESSAGE_TYPE;
    whichHero = gpCurPlayer->m_heroIds[gpCurPlayer->m_heroLocatorPage + locatorSlot];
    message.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_COLOR;
    message.payload.widget.id = widgetBase + LOCATOR_HERO_BORDER_OFFSET;
    message.payload.widget.data.value =
        (whichHero == gpCurPlayer->m_currentHero && gpCurPlayer->m_currentHero != INVALID_HERO
         && !gbAllBlack)
            ? LOCATOR_SELECTED_COLOR
            : LOCATOR_NORMAL_COLOR;
    m_adventureWindow->BroadcastMessage(message);

    if (whichHero == INVALID_HERO || gbAllBlack) {
        message.payload.widget.id = widgetBase + LOCATOR_HERO_IMAGE_OFFSET;
        message.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        message.payload.widget.data.value = locatorSlot + LOCATOR_HERO_EMPTY_FRAME_BASE;
        m_adventureWindow->BroadcastMessage(message);

        message.payload.widget.command = ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = LOCATOR_HERO_DISABLE_FLAGS;
        for (i = 0; i <= LOCATOR_VISIBLE_COUNT - 1; ++i) {
            message.payload.widget.id = widgetBase + i;
            m_adventureWindow->BroadcastMessage(message);
        }
    } else {
        heroPtr = gpGame->GetHero(whichHero);
        message.payload.widget.id = widgetBase + LOCATOR_HERO_IMAGE_OFFSET;
        message.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        message.payload.widget.data.value = LOCATOR_HERO_DEFAULT_FRAME;
        m_adventureWindow->BroadcastMessage(message);

        message.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = LOCATOR_HERO_ENABLE_FLAGS;
        for (i = 0; i <= LOCATOR_HERO_WIDGET_STRIDE - 1; ++i) {
            message.payload.widget.id = widgetBase + i;
            m_adventureWindow->BroadcastMessage(message);
        }

        moveFrm = GetMobilityFrame(heroPtr->m_remainingMobility);
        message.payload.widget.id = widgetBase + LOCATOR_HERO_MOBILITY_OFFSET;
        message.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        message.payload.widget.data.value = moveFrm;
        m_adventureWindow->BroadcastMessage(message);

        manaFr = GetManaFrame(heroPtr->m_spellPoints);
        message.payload.widget.id = widgetBase + LOCATOR_HERO_MANA_OFFSET;
        message.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        message.payload.widget.data.value = manaFr;
        m_adventureWindow->BroadcastMessage(message);

        message.payload.widget.id = widgetBase + LOCATOR_HERO_PORTRAIT_OFFSET;
        message.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        message.payload.widget.data.value = H2EnumIndex(heroPtr->m_portrait);
        m_adventureWindow->BroadcastMessage(message);
    }

    if (drawWindow) {
        m_adventureWindow->DrawWindow(
            LOCATOR_HERO_DRAW_LEFT,
            widgetBase,
            widgetBase + LOCATOR_HERO_BORDER_OFFSET
        );
        if (updateScreen) {
            gpWindowManager->UpdateScreenRegion(
                LOCATOR_SCREEN_LEFT,
                locatorSlot * LOCATOR_SCREEN_ROW_HEIGHT + LOCATOR_SCREEN_TOP,
                LOCATOR_SCREEN_WIDTH,
                LOCATOR_SCREEN_HEIGHT
            );
        }
    }
}

void advManager::UpdateHeroLocators(i32 drawWindow, i32 updateScreen) {
    i32 locatorSlot;
    double scrollStep;

    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        return;
    }

    for (locatorSlot = 0; locatorSlot < LOCATOR_VISIBLE_COUNT; ++locatorSlot) {
        UpdateHeroLocator(locatorSlot, 0, 0);
    }

    if (gpCurPlayer->m_heroCount < LOCATOR_PAGE_THRESHOLD) {
        m_scrollLeftButton->m_y = LOCATOR_SCROLL_NO_PAGES_Y;
    } else {
        scrollStep = ADVMGR_LOCATOR_HERO_SCROLL_SPAN_DOUBLE

                     / (gpCurPlayer->m_heroCount - LOCATOR_PAGE_DENOMINATOR_OFFSET);
        m_scrollLeftButton->m_y = static_cast<i16>(
            gpCurPlayer->m_heroLocatorPage * scrollStep + ADVMGR_LOCATOR_SCROLL_BASE_Y_DOUBLE
        );
    }
    if (drawWindow) {
        m_adventureWindow->DrawWindow(updateScreen);
    }
}

void advManager::UpdateTownLocators(i32 drawWindow, i32 updateScreen) {
    i32 i;
    i32 whichTown;
    tag_message msg;
    double step;

    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        return;
    }

    msg.type = ADVMGR_LOCATOR_MESSAGE_TYPE;
    for (i = 0; i < LOCATOR_VISIBLE_COUNT; ++i) {
        whichTown = gpCurPlayer->m_townIds[gpCurPlayer->m_townLocatorPage + i];
        msg.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_COLOR;
        msg.payload.widget.id = i + LOCATOR_TOWN_BORDER_BASE;
        msg.payload.widget.data.value =
            (gpCurPlayer->m_currentTown != TOWN_ID_NONE
             && whichTown == gpCurPlayer->m_currentTown && !gbAllBlack)
                ? LOCATOR_SELECTED_COLOR
                : LOCATOR_NORMAL_COLOR;
        m_adventureWindow->BroadcastMessage(msg);

        msg.payload.widget.id = i + LOCATOR_TOWN_IMAGE_BASE;
        if (whichTown == TOWN_ID_NONE || gbAllBlack) {
            msg.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
            msg.payload.widget.data.value =
                i + LOCATOR_TOWN_EMPTY_FRAME_BASE;
            m_adventureWindow->BroadcastMessage(msg);
            msg.payload.widget.command = ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS;
            msg.payload.widget.data.value = LOCATOR_TOWN_ENABLE_FLAGS;
            m_adventureWindow->BroadcastMessage(msg);
            msg.payload.widget.command = ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS;
            msg.payload.widget.data.value = LOCATOR_TOWN_DISABLE_FLAGS;
            msg.payload.widget.id = i + LOCATOR_TOWN_FLAG_BASE;
            m_adventureWindow->BroadcastMessage(msg);
        } else {
            msg.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FLAGS;
            msg.payload.widget.data.value = LOCATOR_TOWN_ENABLE_FLAGS;
            m_adventureWindow->BroadcastMessage(msg);
            msg.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
            msg.payload.widget.data.value =
                H2EnumIndex(gpGame->GetTown(whichTown)->m_type) + LOCATOR_TOWN_TYPE_FRAME_BASE;
            if (!(gpGame->GetTown(whichTown)->m_buildings & H2EnumIndex(TOWN_BUILDING_CASTLE))) {
                msg.payload.widget.data.value += LOCATOR_TOWN_VILLAGE_FRAME_OFFSET;
            }
            m_adventureWindow->BroadcastMessage(msg);

            if (H2BitTest(gpGame->m_knownTowns, whichTown)) {
                msg.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FLAGS;
            } else {
                msg.payload.widget.command = ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS;
            }
            msg.payload.widget.data.value = LOCATOR_TOWN_DISABLE_FLAGS;
            msg.payload.widget.id = i + LOCATOR_TOWN_FLAG_BASE;
            m_adventureWindow->BroadcastMessage(msg);
        }
    }

    if (gpCurPlayer->m_townCount < LOCATOR_PAGE_THRESHOLD) {
        m_scrollRightButton->m_y = LOCATOR_SCROLL_NO_PAGES_Y;
    } else {
        step = ADVMGR_LOCATOR_TOWN_SCROLL_SPAN_DOUBLE

                     / (gpCurPlayer->m_townCount - LOCATOR_PAGE_DENOMINATOR_OFFSET);
        m_scrollRightButton->m_y = static_cast<i16>(
            gpCurPlayer->m_townLocatorPage * step + ADVMGR_LOCATOR_SCROLL_BASE_Y_DOUBLE
        );
    }
    if (drawWindow) {
        m_adventureWindow->DrawWindow(updateScreen);
    }
}

void advManager::UpdBottomView(i32 forceUpdate, i32 drawWindow, i32 updateScreen) {
    i32 updated;

    updated = 0;
    gbForceUpdate = forceUpdate;
    if (giBottomViewOverride == BOTTOM_VIEW_OVERRIDE_DISABLED) {
        return;
    }

    if (giBottomViewOverride > BOTTOM_VIEW_NONE) {
        if (platform::Ticks() > giBottomViewOverrideEndTime) {
            giBottomViewOverride = BOTTOM_VIEW_NONE;
        } else {
            switch (giBottomViewOverride) {
                case BOTTOM_VIEW_NEW_TURN:
                    updated = UpdBottomViewNewTurn();
                    break;
                case BOTTOM_VIEW_KINGDOM:
                    updated = UpdBottomViewKingdom();
                    break;
                case BOTTOM_VIEW_RESOURCE:
                    updated = UpdBottomViewResMsg();
                    break;
            }
            goto update_bottom_view;
        }
    }

    if (!gbThisNetHumanPlayer[giCurPlayer] || gbAllBlack
        || gpCurPlayer->m_color != gpGame->m_players[giCurPlayer].m_color) {
        updated = UpdBottomViewEnemyTurn();
    } else if (gpCurPlayer->m_currentHero == INVALID_HERO) {
        updated = UpdBottomViewKingdom();
    } else {
        updated = UpdBottomViewHero();
    }

update_bottom_view:
    if (updated && drawWindow) {
        m_adventureWindow
            ->DrawWindow(BOTTOM_VIEW_DRAW_LEFT, BOTTOM_VIEW_DRAW_TOP, BOTTOM_VIEW_DRAW_BOTTOM);
        if (updateScreen) {
            gpWindowManager->UpdateScreenRegion(
                BOTTOM_VIEW_PANEL_X,
                BOTTOM_VIEW_PANEL_Y,
                BOTTOM_VIEW_PANEL_WIDTH,
                BOTTOM_VIEW_PANEL_HEIGHT
            );
        }
    }
    forceUpdate = gbForceUpdate;
}

void advManager::ClearBottomView(void) {
    i32 widgetIndex;

    if (iCurBottomView == BOTTOM_VIEW_NONE) {
        return;
    }

    for (widgetIndex = 0; widgetIndex < BOTTOM_VIEW_WIDGET_CAPACITY; ++widgetIndex) {
        if (m_bottomViewPrimaryWidgets[widgetIndex] != NULL) {
            m_adventureWindow->RemoveWidget(m_bottomViewPrimaryWidgets[widgetIndex]);
            delete m_bottomViewPrimaryWidgets[widgetIndex];
        }
        if (m_bottomViewSecondaryWidgets[widgetIndex] != NULL) {
            m_adventureWindow->RemoveWidget(m_bottomViewSecondaryWidgets[widgetIndex]);
            delete m_bottomViewSecondaryWidgets[widgetIndex];
        }
        m_bottomViewPrimaryWidgets[widgetIndex] = NULL;
        m_bottomViewSecondaryWidgets[widgetIndex] = NULL;
    }
    iCurBottomViewEnemy = BOTTOM_VIEW_NO_ENEMY;
    iCurBottomView = BOTTOM_VIEW_NONE;
    iLastAnimFrame = BOTTOM_VIEW_NO_ANIMATION;
}

i32 advManager::UpdBottomViewEnemyTurn(void) {
    i32 updated;
    tag_message msg;

    updated = 0;
    msg.type = ADVMGR_ENEMY_TURN_MESSAGE_TYPE;
    if (iCurBottomView != BOTTOM_VIEW_ENEMY_TURN) {
        updated = 1;
        gbForceUpdate = true;
        ClearBottomView();
        iCurBottomView = BOTTOM_VIEW_ENEMY_TURN;

        m_bottomViewBackground = new iconWidget(
            ENEMY_TURN_BACKGROUND_X,
            ENEMY_TURN_BACKGROUND_Y,
            ENEMY_TURN_BACKGROUND_WIDTH,
            ENEMY_TURN_BACKGROUND_HEIGHT,
            "stonback.icn",
            0,
            ICON_DRAW_NORMAL,
            ENEMY_TURN_BACKGROUND_ID,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (m_bottomViewBackground == NULL) {
            MemError();
        }
        m_adventureWindow->AddWidget(m_bottomViewBackground, ENEMY_TURN_BACKGROUND_Z);

        m_bottomViewHourglassBackground = new iconWidget(
            ENEMY_TURN_HOURGLASS_X,
            ENEMY_TURN_HOURGLASS_Y,
            ENEMY_TURN_HOURGLASS_WIDTH,
            ENEMY_TURN_HOURGLASS_HEIGHT,
            "hourglas.icn",
            0,
            ICON_DRAW_NORMAL,
            ENEMY_TURN_HOURGLASS_ID,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (m_bottomViewHourglassBackground == NULL) {
            MemError();
        }
        m_adventureWindow->AddWidget(m_bottomViewHourglassBackground, ENEMY_TURN_HOURGLASS_Z);
    }

    if (gbForceUpdate || platform::Ticks() - iLastSandAnimTime > ENEMY_TURN_ANIMATION_DELAY) {
        iLastSandAnimTime = platform::Ticks();
        iLastAnimFrame = m_updateMaxX;
        if (platform::Ticks() - iLastNewSandAnimTime > ENEMY_TURN_ANIMATION_DELAY) {
            iLastNewSandAnimTime = platform::Ticks();
            ++iSandAnim;
            if (iSandAnim >= ENEMY_TURN_SAND_FRAME_LIMIT) {
                iSandAnim = ENEMY_TURN_SAND_RESTART_FRAME;
            }
            updated = 1;

            if (m_bottomViewIcons[ENEMY_TURN_SAND_SLOT] != NULL) {
                msg.payload.widget.command = ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME;
                msg.payload.widget.id = ENEMY_TURN_SAND_ID;
                msg.payload.widget.data.value = iSandAnim + ENEMY_TURN_SAND_FRAME_OFFSET;
                m_adventureWindow->BroadcastMessage(msg);
            } else {
                m_bottomViewIcons[ENEMY_TURN_SAND_SLOT] = new iconWidget(
                    ENEMY_TURN_ANIMATION_X,
                    ENEMY_TURN_ANIMATION_Y,
                    ENEMY_TURN_ANIMATION_WIDTH,
                    ENEMY_TURN_ANIMATION_HEIGHT,
                    "hourglas.icn",
                    iSandAnim + ENEMY_TURN_SAND_FRAME_OFFSET,
                    ICON_DRAW_NORMAL,
                    ENEMY_TURN_SAND_ID,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
                if (m_bottomViewIcons[ENEMY_TURN_SAND_SLOT] == NULL) {
                    MemError();
                }
                m_adventureWindow->AddWidget(
                    m_bottomViewIcons[ENEMY_TURN_SAND_SLOT],
                    ENEMY_TURN_SAND_Z
                );
            }
        }
    }

    if (gbForceUpdate || iCurBottomViewEnemy != giCurPlayer) {
        updated = 1;
        iCurBottomViewEnemy = giCurPlayer;
        if (iCurBottomViewEnemy != giCurPlayer) {
            iCurHourGlassPhase = 0;
        }
        if (m_bottomViewIcons[ENEMY_TURN_CREST_SLOT] != NULL) {
            msg.payload.widget.command = ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME;
            msg.payload.widget.id = ENEMY_TURN_CREST_ID;
            msg.payload.widget.data.value =
                gpGame->m_players[static_cast<char>(giCurPlayer)].m_color;
            m_adventureWindow->BroadcastMessage(msg);
        } else {
            m_bottomViewIcons[ENEMY_TURN_CREST_SLOT] = new iconWidget(
                ENEMY_TURN_CREST_X,
                ENEMY_TURN_ANIMATION_Y,
                ENEMY_TURN_ANIMATION_WIDTH,
                ENEMY_TURN_ANIMATION_HEIGHT,
                "brcrest.icn",
                gpGame->GetPlayerColor(static_cast<char>(giCurPlayer)),
                ICON_DRAW_NORMAL,
                ENEMY_TURN_CREST_ID,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (m_bottomViewIcons[ENEMY_TURN_CREST_SLOT] == NULL) {
                MemError();
            }
            m_adventureWindow->AddWidget(
                m_bottomViewIcons[ENEMY_TURN_CREST_SLOT],
                ENEMY_TURN_CREST_Z
            );
        }
    }

    if (gbForceUpdate || iCurHourGlassPhase < iLastHourGlassPhase || iLastHourGlassPhase < 0
        || (iCurHourGlassPhase > iLastHourGlassPhase
            && platform::Ticks() - giLastHourGlassUpdateTime >= ENEMY_TURN_PHASE_DELAY)) {
        updated = 1;
        iLastHourGlassPhase = iCurHourGlassPhase;
        giLastHourGlassUpdateTime = platform::Ticks();
        if (m_bottomViewIcons[ENEMY_TURN_PHASE_SLOT] != NULL) {
            msg.payload.widget.command = ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME;
            msg.payload.widget.id = ENEMY_TURN_PHASE_ID;
            msg.payload.widget.data.value = iCurHourGlassPhase + ENEMY_TURN_PHASE_FRAME_OFFSET;
            m_adventureWindow->BroadcastMessage(msg);
        } else {
            m_bottomViewIcons[ENEMY_TURN_PHASE_SLOT] = new iconWidget(
                ENEMY_TURN_ANIMATION_X,
                ENEMY_TURN_ANIMATION_Y,
                ENEMY_TURN_ANIMATION_WIDTH,
                ENEMY_TURN_ANIMATION_HEIGHT,
                "hourglas.icn",
                iCurHourGlassPhase + ENEMY_TURN_PHASE_FRAME_OFFSET,
                ICON_DRAW_NORMAL,
                ENEMY_TURN_PHASE_ID,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (m_bottomViewIcons[ENEMY_TURN_PHASE_SLOT] == NULL) {
                MemError();
            }
            m_adventureWindow->AddWidget(
                m_bottomViewIcons[ENEMY_TURN_PHASE_SLOT],
                ENEMY_TURN_PHASE_Z
            );
        }
    }
    return updated;
}

i32 advManager::UpdBottomViewNewTurn(void) {

    i32 frameIndex;
    i32 month;
    char* week;
    char* day;

    frameIndex = 0;
    if (!gbForceUpdate && iCurBottomView == BOTTOM_VIEW_NEW_TURN) {
        return 0;
    }

    ClearBottomView();
    iCurBottomView = BOTTOM_VIEW_NEW_TURN;
    if (gpGame->m_day == NEW_TURN_FIRST_DAY
        && (gpGame->m_month != NEW_TURN_FIRST_DAY || gpGame->m_week != NEW_TURN_FIRST_DAY
            || gpGame->m_day != NEW_TURN_FIRST_DAY)) {
        frameIndex = gpGame->m_week;
    }

    m_bottomViewBackground = new iconWidget(
        BOTTOM_VIEW_PANEL_X,
        BOTTOM_VIEW_PANEL_Y,
        BOTTOM_VIEW_BACKGROUND_WIDTH,
        BOTTOM_VIEW_PANEL_HEIGHT,
        "stonback.icn",
        0,
        ICON_DRAW_NORMAL,
        BOTTOM_VIEW_BACKGROUND_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_bottomViewBackground == NULL) {
        MemError();
    }
    m_adventureWindow->AddWidget(m_bottomViewBackground, -1);

    m_bottomViewHourglassBackground = new iconWidget(
        NEW_TURN_DATE_ICON_X,
        NEW_TURN_DATE_ICON_Y,
        NEW_TURN_DATE_ICON_WIDTH,
        NEW_TURN_DATE_ICON_HEIGHT,
        "sunmoon.icn",
        frameIndex,
        ICON_DRAW_NORMAL,
        BOTTOM_VIEW_FOREGROUND_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_bottomViewHourglassBackground == NULL) {
        MemError();
    }
    m_adventureWindow->AddWidget(m_bottomViewHourglassBackground, -1);

    week = static_cast<char*>(H2_ALLOC(BOTTOM_VIEW_TEXT_BUFFER_SIZE));
    sprintf(
        week,
        "%s: %d  %s: %d",
        "\xcc\xe5\xf1\xff\xf6"  ,
        gpGame->m_month,
        "\xcd\xe5\xe4\xe5\xeb\xff"  ,
        gpGame->m_week
    );
    m_bottomViewAllTexts[0] = new textWidget(
        NEW_TURN_DATE_TEXT_X,
        NEW_TURN_WEEK_TEXT_Y,
        NEW_TURN_DATE_TEXT_WIDTH,
        NEW_TURN_WEEK_TEXT_HEIGHT,
        week,
        "smalfont.fnt",
        FONT_DRAW_DEFAULT,
        BOTTOM_VIEW_TEXT_ID,
        WIDGET_KIND_TEXT,
        FONT_ALIGN_CENTER
    );
    if (m_bottomViewAllTexts[0] == NULL) {
        MemError();
    }
    m_adventureWindow->AddWidget(m_bottomViewAllTexts[0], -1);

    day = static_cast<char*>(H2_ALLOC(BOTTOM_VIEW_TEXT_BUFFER_SIZE));
    sprintf(
        day,
        "%s: %d",
        "\xc4\xe5\xed\xfc"  ,
        gpGame->m_day
    );
    m_bottomViewAllTexts[0] = new textWidget(
        NEW_TURN_DATE_TEXT_X,
        NEW_TURN_DAY_TEXT_Y,
        NEW_TURN_DATE_TEXT_WIDTH,
        NEW_TURN_DAY_TEXT_HEIGHT,
        day,
        "bigfont.fnt",
        FONT_DRAW_DEFAULT,
        BOTTOM_VIEW_TEXT_ID,
        WIDGET_KIND_TEXT,
        FONT_ALIGN_CENTER
    );
    if (m_bottomViewAllTexts[0] == NULL) {
        MemError();
    }
    m_adventureWindow->AddWidget(m_bottomViewAllTexts[0], -1);
    return 1;
}

i32 advManager::UpdBottomViewResMsg(void) {

    i32 iconWidth;
    i32 iconHeight;
    i32 textY;
    i32 lineCnt;
    char* messageText;
    char* countString;

    if (!gbForceUpdate && iCurBottomView == BOTTOM_VIEW_RESOURCE) {
        return 0;
    }

    ClearBottomView();
    iCurBottomView = BOTTOM_VIEW_RESOURCE;
    m_bottomViewBackground = new iconWidget(
        BOTTOM_VIEW_PANEL_X,
        BOTTOM_VIEW_PANEL_Y,
        BOTTOM_VIEW_BACKGROUND_WIDTH,
        BOTTOM_VIEW_PANEL_HEIGHT,
        "stonback.icn",
        0,
        ICON_DRAW_NORMAL,
        BOTTOM_VIEW_BACKGROUND_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_bottomViewBackground == NULL) {
        MemError();
    }
    m_adventureWindow->AddWidget(m_bottomViewBackground, -1);

    textY = 0;
    if (giBottomViewResource < RES_VALID_BEGIN) {
        textY = RESOURCE_VIEW_MULTILINE_HEIGHT;
        lineCnt = smallFont->LineLength(gcBottomViewText, BOTTOM_VIEW_PANEL_WIDTH);
        textY -= lineCnt * RESOURCE_VIEW_LINE_HEIGHT;
    }
    messageText = static_cast<char*>(H2_ALLOC(strlen(gcBottomViewText) + 1));
    sprintf(messageText, gcBottomViewText);
    m_bottomViewAllTexts[0] = new textWidget(
        BOTTOM_VIEW_PANEL_X,
        textY + RESOURCE_VIEW_TEXT_BASE_Y,
        BOTTOM_VIEW_PANEL_WIDTH,
        RESOURCE_VIEW_TEXT_HEIGHT,
        messageText,
        "smalfont.fnt",
        FONT_DRAW_DEFAULT,
        BOTTOM_VIEW_TEXT_ID,
        WIDGET_KIND_TEXT,
        FONT_ALIGN_CENTER
    );
    if (m_bottomViewAllTexts[0] == NULL) {
        MemError();
    }
    m_adventureWindow->AddWidget(m_bottomViewAllTexts[0], -1);

    if (giBottomViewResource >= RES_VALID_BEGIN) {
        if (giBottomViewResource == RES_GOLD) {
            iconWidth = RESOURCE_VIEW_GOLD_WIDTH;
            iconHeight = RESOURCE_VIEW_GOLD_HEIGHT;
        } else {
            iconWidth = RESOURCE_VIEW_ICON_WIDTH;
            iconHeight = RESOURCE_VIEW_ICON_HEIGHT;
        }
        m_bottomViewHourglassBackground = new iconWidget(
            (BOTTOM_VIEW_PANEL_WIDTH - iconWidth) / BOTTOM_VIEW_CENTER_DIVISOR
                + BOTTOM_VIEW_PANEL_X,
            RESOURCE_VIEW_ICON_BOTTOM - iconHeight - RESOURCE_VIEW_ICON_BOTTOM_PADDING,
            iconWidth,
            iconHeight,
            "resource.icn",
            H2EnumIndex(giBottomViewResource),
            ICON_DRAW_NORMAL,
            BOTTOM_VIEW_FOREGROUND_ID,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (m_bottomViewHourglassBackground == NULL) {
            MemError();
        }
        m_adventureWindow->AddWidget(m_bottomViewHourglassBackground, -1);

        countString = static_cast<char*>(H2_ALLOC(BOTTOM_VIEW_COUNT_BUFFER_SIZE));
        sprintf(
            countString,
            "%d",
            giBottomViewResourceQty
        );
        m_bottomViewAllTexts[1] = new textWidget(
            RESOURCE_VIEW_COUNT_X,
            RESOURCE_VIEW_COUNT_Y,
            RESOURCE_VIEW_COUNT_WIDTH,
            RESOURCE_VIEW_COUNT_HEIGHT,
            countString,
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            BOTTOM_VIEW_TEXT_ID_2,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (m_bottomViewAllTexts[1] == NULL) {
            MemError();
        }
        m_adventureWindow->AddWidget(m_bottomViewAllTexts[1], -1);
    }
    return 1;
}

i32 advManager::UpdBottomViewKingdom(void) {

    i32 numVillage;
    i32 i;
    i32 nCastles;
    i8 rowY[KINGDOM_VIEW_ENTRY_COUNT];
    u8 textX[KINGDOM_VIEW_ENTRY_COUNT];
    char* countText[KINGDOM_VIEW_ENTRY_COUNT];

    if (!gbForceUpdate && iCurBottomView == BOTTOM_VIEW_KINGDOM) {
        return 0;
    }

    ClearBottomView();
    iCurBottomView = BOTTOM_VIEW_KINGDOM;
    rowY[H2EnumIndex(RES_WOOD)] = KINGDOM_VIEW_RESOURCE_TEXT_Y;
    rowY[H2EnumIndex(RES_MERCURY)] = KINGDOM_VIEW_RESOURCE_TEXT_Y;
    rowY[H2EnumIndex(RES_ORE)] = KINGDOM_VIEW_RESOURCE_TEXT_Y;
    rowY[H2EnumIndex(RES_SULFUR)] = KINGDOM_VIEW_RESOURCE_TEXT_Y;
    rowY[H2EnumIndex(RES_CRYSTAL)] = KINGDOM_VIEW_RESOURCE_TEXT_Y;
    rowY[H2EnumIndex(RES_GEMS)] = KINGDOM_VIEW_RESOURCE_TEXT_Y;
    rowY[H2EnumIndex(RES_GOLD)] = KINGDOM_VIEW_TOWN_TEXT_Y;
    rowY[KINGDOM_VIEW_CASTLE_ENTRY] = KINGDOM_VIEW_TOWN_TEXT_Y;
    rowY[KINGDOM_VIEW_TOWN_ENTRY] = KINGDOM_VIEW_TOWN_TEXT_Y;
    textX[H2EnumIndex(RES_WOOD)] = KINGDOM_VIEW_WOOD_TEXT_X;
    textX[H2EnumIndex(RES_MERCURY)] = KINGDOM_VIEW_MERCURY_TEXT_X;
    textX[H2EnumIndex(RES_ORE)] = KINGDOM_VIEW_ORE_TEXT_X;
    textX[H2EnumIndex(RES_SULFUR)] = KINGDOM_VIEW_SULFUR_TEXT_X;
    textX[H2EnumIndex(RES_CRYSTAL)] = KINGDOM_VIEW_CRYSTAL_TEXT_X;
    textX[H2EnumIndex(RES_GEMS)] = KINGDOM_VIEW_GEMS_TEXT_X;
    textX[H2EnumIndex(RES_GOLD)] = KINGDOM_VIEW_GOLD_TEXT_X;
    textX[KINGDOM_VIEW_CASTLE_ENTRY] = KINGDOM_VIEW_CASTLE_TEXT_X;
    textX[KINGDOM_VIEW_TOWN_ENTRY] = KINGDOM_VIEW_VILLAGE_TEXT_X;
    numVillage = 0;
    nCastles = 0;

    m_bottomViewBackground = new iconWidget(
        BOTTOM_VIEW_PANEL_X,
        BOTTOM_VIEW_PANEL_Y,
        BOTTOM_VIEW_BACKGROUND_WIDTH,
        BOTTOM_VIEW_PANEL_HEIGHT,
        "stonback.icn",
        0,
        ICON_DRAW_NORMAL,
        BOTTOM_VIEW_BACKGROUND_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_bottomViewBackground == NULL) {
        MemError();
    }
    m_adventureWindow->AddWidget(m_bottomViewBackground, -1);

    m_bottomViewHourglassBackground = new iconWidget(
        KINGDOM_VIEW_ICON_X,
        KINGDOM_VIEW_ICON_Y,
        BOTTOM_VIEW_PANEL_WIDTH,
        BOTTOM_VIEW_PANEL_HEIGHT,
        "ressmall.icn",
        0,
        ICON_DRAW_NORMAL,
        BOTTOM_VIEW_FOREGROUND_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_bottomViewHourglassBackground == NULL) {
        MemError();
    }
    m_adventureWindow->AddWidget(m_bottomViewHourglassBackground, -1);

    for (i = 0; i < gpCurPlayer->m_townCount; ++i) {
        if (gpGame->m_castleRecs[gpCurPlayer->m_townIds[i]].m_buildings
            & H2EnumIndex(TOWN_BUILDING_CASTLE)) {
            ++nCastles;
        } else {
            ++numVillage;
        }
    }

    for (i = 0; i < KINGDOM_VIEW_ENTRY_COUNT; ++i) {
        countText[i] = static_cast<char*>(H2_ALLOC(BOTTOM_VIEW_COUNT_BUFFER_SIZE));
        if (i < KINGDOM_VIEW_RESOURCE_COUNT) {
            sprintf(
                countText[i],
                "%d",
                gpCurPlayer->m_resources[i]
            );
        } else if (i == KINGDOM_VIEW_CASTLE_ENTRY) {
            sprintf(
                countText[i],
                "%d",
                nCastles
            );
        } else {
            sprintf(
                countText[i],
                "%d",
                numVillage
            );
        }

        m_bottomViewAllTexts[i] = new textWidget(
            textX[i] + KINGDOM_VIEW_TEXT_X_BASE,
            rowY[i] + KINGDOM_VIEW_TEXT_Y_BASE,
            KINGDOM_VIEW_TEXT_WIDTH,
            KINGDOM_VIEW_TEXT_HEIGHT,
            countText[i],
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            i + BOTTOM_VIEW_TEXT_ID,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (m_bottomViewAllTexts[i] == NULL) {
            MemError();
        }
        m_adventureWindow->AddWidget(m_bottomViewAllTexts[i], -1);
    }
    return 1;
}

i32 advManager::UpdBottomViewHero(void) {

    i32 iconX;
    i32 usedCount;
    i32 labelDrawX;
    i32 iconY;
    i32 slotNumber;
    hero* targetHero;
    char* armyCountLabelsResult[BOTTOM_HERO_ARMY_SLOTS];
    i32 labelY;
    i32 leftEdge;
    icon* creatureIcons;
    i32 layoutPos;
    i32 countWidth;
    i32 blockWidth;
    i32 displayIndex;
    i32 creature;
    IconEntry* iconEntryValue;
    i32 rightEdge;

    if (!gbForceUpdate && iCurBottomView == BOTTOM_VIEW_HERO) {
        return 0;
    }

    ClearBottomView();
    iCurBottomView = BOTTOM_VIEW_HERO;
    targetHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    usedCount = 0;

    m_bottomViewBackground = new iconWidget(
        BOTTOM_HERO_PANEL_X,
        BOTTOM_HERO_PANEL_Y,
        BOTTOM_HERO_PANEL_WIDTH,
        BOTTOM_HERO_PANEL_HEIGHT,
        "stonback.icn",
        0,
        ICON_DRAW_NORMAL,
        BOTTOM_VIEW_FIRST_MESSAGE,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_bottomViewBackground == NULL) {
        MemError();
    }
    m_adventureWindow->AddWidget(m_bottomViewBackground, -1);

    for (slotNumber = 0; slotNumber < BOTTOM_HERO_ARMY_SLOTS; ++slotNumber) {
        if (targetHero->m_army.m_creatureTypes[slotNumber] != CREATURE_NONE) {
            ++usedCount;
        }
    }

    if (usedCount != 0) {
        displayIndex = 0;
        creatureIcons = gpResourceManager->GetIcon("mons32.icn");
        for (slotNumber = 0; slotNumber < BOTTOM_HERO_ARMY_SLOTS; ++slotNumber) {
            creature = H2EnumIndex(targetHero->m_army.m_creatureTypes[slotNumber]);
            if (creature != BOTTOM_HERO_EMPTY_SLOT) {
                u8 iconPositions[BOTTOM_HERO_ICON_POSITION_BYTES] =
                    {50, 3, 96, 3, 50, 17, 73, 17, 96, 17, 27, 32, 73, 32, 119, 32};
                i8 slotLayouts[BOTTOM_HERO_ARMY_SLOTS][BOTTOM_HERO_ARMY_SLOTS] = {
                    {3, -1, -1, -1, -1},
                    {2, 4, -1, -1, -1},
                    {0, 1, 6, -1, -1},
                    {0, 1, 5, 6, -1},
                    {0, 1, 5, 6, 7}
                };

                armyCountLabelsResult[displayIndex] =
                    static_cast<char*>(H2_ALLOC(BOTTOM_HERO_LABEL_BYTES));
                if (targetHero->m_army.m_creatureCounts[slotNumber] > BOTTOM_HERO_MAX_FULL_COUNT) {
                    sprintf(
                        armyCountLabelsResult[displayIndex],
                        "%dk",
                        targetHero->m_army.m_creatureCounts[slotNumber] / BOTTOM_HERO_COUNT_DIVISOR
                    );
                } else {
                    sprintf(
                        armyCountLabelsResult[displayIndex],
                        "%d",
                        targetHero->m_army.m_creatureCounts[slotNumber]
                    );
                }

                layoutPos = slotLayouts[usedCount - 1][displayIndex];
                iconX = iconPositions[layoutPos * BOTTOM_HERO_POSITION_COMPONENT_COUNT];
                iconY = iconPositions[layoutPos * BOTTOM_HERO_POSITION_COMPONENT_COUNT + 1];
                labelY = iconY + BOTTOM_HERO_LABEL_Y_OFFSET;
                iconEntryValue = reinterpret_cast<IconEntry*>(
                    creature * sizeof(IconEntry) + creatureIcons->m_data
                );
                if (layoutPos == 0 || layoutPos == 1) {
                    labelY -= BOTTOM_HERO_TOP_LABEL_SHIFT;
                    if (iconEntryValue->h < BOTTOM_HERO_TOP_MIN_HEIGHT) {
                        iconY += BOTTOM_HERO_TOP_MIN_HEIGHT - iconEntryValue->h;
                    }
                } else if (iconEntryValue->h < BOTTOM_HERO_LOWER_MIN_HEIGHT) {
                    iconY += BOTTOM_HERO_LOWER_MIN_HEIGHT - iconEntryValue->h;
                }

                countWidth = smallFont->LineWidth(armyCountLabelsResult[displayIndex]);
                blockWidth = iconEntryValue->w + countWidth;
                if (blockWidth > BOTTOM_HERO_GROUP_WIDTH) {
                    blockWidth = BOTTOM_HERO_GROUP_WIDTH;
                }
                iconX -= (blockWidth + 1) / BOTTOM_VIEW_CENTER_DIVISOR;
                labelDrawX = iconX + blockWidth - 1 - (countWidth - 1);

                m_bottomViewIcons[displayIndex] = new iconWidget(
                    iconX + BOTTOM_HERO_PANEL_X,
                    iconY + BOTTOM_HERO_PANEL_Y,
                    BOTTOM_HERO_ICON_WIDTH,
                    BOTTOM_HERO_ICON_HEIGHT,
                    "mons32.icn",
                    creature,
                    ICON_DRAW_NORMAL,
                    displayIndex + BOTTOM_HERO_FIRST_ICON_ID,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
                if (m_bottomViewIcons[displayIndex] == NULL) {
                    MemError();
                }

                m_bottomViewTexts[displayIndex] = new textWidget(
                    labelDrawX + BOTTOM_HERO_PANEL_X,
                    labelY + BOTTOM_HERO_PANEL_Y,
                    strlen(armyCountLabelsResult[displayIndex]) * BOTTOM_HERO_CHARACTER_WIDTH
                        + (targetHero->m_army.m_creatureCounts[slotNumber]
                                   > BOTTOM_HERO_MAX_FULL_COUNT
                               ? BOTTOM_HERO_ABBREVIATED_LABEL_PADDING
                               : 0),
                    BOTTOM_HERO_LABEL_HEIGHT,
                    armyCountLabelsResult[displayIndex],
                    "smalfont.fnt",
                    FONT_DRAW_DEFAULT,
                    displayIndex + BOTTOM_HERO_FIRST_TEXT_ID,
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
                if (m_bottomViewTexts[displayIndex] == NULL) {
                    MemError();
                }

                m_adventureWindow->AddWidget(m_bottomViewIcons[displayIndex], -1);
                m_adventureWindow->AddWidget(m_bottomViewTexts[displayIndex], -1);
                ++displayIndex;
            }
        }
        gpResourceManager->Dispose(creatureIcons);
    }
    return 1;
}

void advManager::HeroQuickView(i32 heroId, i32 locatorSlot, i32 windowX, i32 windowY) {

    i32 creatureCount;
    i16 armyWidth = HERO_QUICK_ARMY_AREA_WIDTH;
    i16 leftEdge = ARMY_QUICK_AREA_LEFT;
    i16 creatureY = HERO_QUICK_DETAILED_CREATURE_Y;
    i16 iconWidth = ARMY_QUICK_ICON_SIZE;
    i16 creatureIconHeight = ARMY_QUICK_ICON_SIZE;
    textWidget* sizeWidgets[ARMY_QUICK_SLOT_COUNT];
    i16 enableFlag = 1;
    i16 portId = HERO_QUICK_PORTRAIT_WIDGET;
    icon* iconRef;
    i16 statId = HERO_QUICK_PRIMARY_STAT_WIDGET;
    i16 playerColorWidget = HERO_QUICK_PLAYER_COLOR_WIDGET;
    heroWindow* win;
    hero* targetHero;
    char* labels[ARMY_QUICK_SLOT_COUNT];
    i32 ii;
    i32 savedY;
    i32 oldX;
    iconWidget* stackIcons[ARMY_QUICK_SLOT_COUNT];
    tag_message msg;

    msg.type = MESSAGE_WIDGET;
    if (heroId == INVALID_HERO) {
        return;
    }

    iconRef = gpResourceManager->GetIcon("mons32.icn");
    targetHero = gpGame->GetHero(heroId);
    if (targetHero->m_owner == giCurPlayer || m_identifyHeroActive == 1
        || IsCrystalBallInEffect(targetHero->m_x, targetHero->m_y, CRYSTAL_BALL_RADIUS)) {
        if (windowX == -1) {
            windowX = HERO_QUICK_DEFAULT_WINDOW_X;
            windowY = locatorSlot * HERO_QUICK_LOCATOR_ROW_HEIGHT + HERO_QUICK_LOCATOR_BASE_Y;
        }
        win = new heroWindow(
            windowX,
            windowY,
            "qhero0.bin"
        );
        if (win == NULL) {
            MemError();
        }
        SetWinText(win, HERO_QUICK_WINDOW_TEXT);
    } else {
        win = new heroWindow(
            windowX,
            windowY,
            "qhero1.bin"
        );
        if (win == NULL) {
            MemError();
        }
    }

    msg.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    msg.payload.widget.id = HERO_QUICK_PORTRAIT_WIDGET;
    msg.payload.widget.data.value = H2EnumIndex(targetHero->m_portrait);
    win->BroadcastMessage(msg);
    msg.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    msg.payload.widget.id = HERO_QUICK_PLAYER_COLOR_WIDGET;
    msg.payload.widget.data.value =
        gpGame->m_players[targetHero->m_owner].m_color * HERO_QUICK_PLAYER_COLOR_STRIDE;
    win->BroadcastMessage(msg);
    ++msg.payload.widget.id;
    ++msg.payload.widget.data.value;
    win->BroadcastMessage(msg);
    sprintf(
        gText,
        "%s",
        targetHero->m_name
    );
    msg.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    msg.payload.widget.id = HERO_QUICK_NAME_WIDGET;
    msg.payload.widget.data.text = gText;
    win->BroadcastMessage(msg);

    creatureCount = 0;
    for (ii = 0; ii < ARMY_QUICK_SLOT_COUNT; ++ii) {
        if (targetHero->m_army.m_creatureTypes[ii] != CREATURE_NONE) {
            ++creatureCount;
        }
    }

    if (targetHero->m_owner == giCurPlayer || m_identifyHeroActive == 1
        || IsCrystalBallInEffect(targetHero->m_x, targetHero->m_y, CRYSTAL_BALL_RADIUS)) {
        for (ii = 0; ii < HERO_PRIMARY_STAT_COUNT; ++ii) {
            sprintf(
                gText,
                "%d",
                targetHero->Stats(HeroPrimaryStat(ii))
            );
            msg.payload.widget.id = ii + HERO_QUICK_PRIMARY_STAT_WIDGET;
            msg.payload.widget.data.text = gText;
            win->BroadcastMessage(msg);
        }
        sprintf(
            gText,
            "%d/%d",
            targetHero->m_spellPoints,
            targetHero->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE
        );
        msg.payload.widget.id = HERO_QUICK_MANA_WIDGET;
        msg.payload.widget.data.text = gText;
        win->BroadcastMessage(msg);

        if (creatureCount != 0) {
            i32 armyStart =
                (HERO_QUICK_ARMY_AREA_WIDTH - creatureCount * ARMY_QUICK_ICON_SIZE)
                    / ARMY_QUICK_CENTER_DIVISOR
                + ARMY_QUICK_AREA_LEFT;
            i32 idx = 0;
            i32 monster;
            for (ii = 0; ii < creatureCount; ++ii) {
                while (targetHero->m_army.m_creatureTypes[idx]
                       == CREATURE_NONE) {
                    ++idx;
                }
                monster = H2EnumIndex(targetHero->m_army.m_creatureTypes[idx]);
                if (monster != ARMY_QUICK_EMPTY_SLOT) {
                    stackIcons[ii] = new iconWidget(
                        static_cast<i16>(
                            armyStart + ii * ARMY_QUICK_ICON_SIZE
                            - GetIconEntry(iconRef, monster)->x
                            + (ARMY_QUICK_ICON_SIZE - GetIconEntry(iconRef, monster)->w)
                                  / ARMY_QUICK_CENTER_DIVISOR
                            + 1
                        ),
                        static_cast<i16>(
                            HERO_QUICK_DETAILED_CREATURE_Y
                            - GetIconEntry(iconRef, monster)->y
                            + (ARMY_QUICK_ICON_BASELINE - GetIconEntry(iconRef, monster)->h)
                        ),
                        ARMY_QUICK_ICON_SIZE,
                        ARMY_QUICK_ICON_SIZE,
                        "mons32.icn",
                        static_cast<i16>(monster),
                        ICON_DRAW_NORMAL,
                        -1,
                        WIDGET_KIND_ICON_DIRECT,
                        1
                    );
                    if (stackIcons[ii] == NULL) {
                        MemError();
                    }
                    labels[ii] =
                        static_cast<char*>(H2_ALLOC(HERO_QUICK_ARMY_LABEL_CAPACITY));
                    sprintf(
                        labels[ii],
                        "%d",
                        targetHero->m_army.m_creatureCounts[idx]
                    );
                    sizeWidgets[ii] = new textWidget(
                        static_cast<i16>(armyStart + ii * ARMY_QUICK_ICON_SIZE),
                        static_cast<i16>(
                            HERO_QUICK_DETAILED_CREATURE_Y + H2EnumIndex(ARMY_QUICK_ICON_SIZE)
                        ),
                        ARMY_QUICK_ICON_SIZE,
                        ARMY_QUICK_LABEL_HEIGHT,
                        labels[ii],
                        "smalfont.fnt",
                        FONT_DRAW_DEFAULT,
                        -1,
                        WIDGET_KIND_TEXT,
                        FONT_ALIGN_CENTER
                    );
                    if (sizeWidgets[ii] == NULL) {
                        MemError();
                    }
                    win->AddWidget(stackIcons[ii], -1);
                    win->AddWidget(sizeWidgets[ii], -1);
                }
                ++idx;
            }
        }
    } else if (creatureCount != 0) {
        i32 rowY = HERO_QUICK_VAGUE_FIRST_ROW_Y;
        i32 topRow;
        i32 row2;
        i32 monster;
        switch (creatureCount) {
            case 1:
            case ARMY_QUICK_FIRST_ROW_COUNT:
            case ARMY_QUICK_TOP_ROW_MAX:
                rowY += ARMY_QUICK_FIRST_ROW_SHIFT;
                topRow = creatureCount;
                row2 = 0;
                break;
            case ARMY_QUICK_FOUR_STACK_COUNT:
                topRow = ARMY_QUICK_FIRST_ROW_COUNT;
                row2 = ARMY_QUICK_FIRST_ROW_COUNT;
                break;
            default:
                topRow = ARMY_QUICK_FIRST_ROW_COUNT;
                row2 = ARMY_QUICK_TOP_ROW_MAX;
                break;
        }

        i32 idx = 0;
        i32 stride = HERO_QUICK_ARMY_AREA_WIDTH / topRow;
        i32 armyStart =
            (stride - ARMY_QUICK_ICON_SIZE) / ARMY_QUICK_CENTER_DIVISOR + ARMY_QUICK_AREA_LEFT;
        for (ii = 0; ii < topRow; ++ii) {
            while (targetHero->m_army.m_creatureTypes[idx] == CREATURE_NONE) {
                ++idx;
            }
            monster = H2EnumIndex(targetHero->m_army.m_creatureTypes[idx]);
            stackIcons[ii] = new iconWidget(
                static_cast<i16>(
                    armyStart + stride * ii
                    - GetIconEntry(iconRef, monster)->x
                    + (ARMY_QUICK_ICON_SIZE - GetIconEntry(iconRef, monster)->w)
                          / ARMY_QUICK_CENTER_DIVISOR
                    + 1
                ),
                static_cast<i16>(
                    rowY - GetIconEntry(iconRef, monster)->y
                    + (ARMY_QUICK_ICON_BASELINE - GetIconEntry(iconRef, monster)->h)
                ),
                ARMY_QUICK_ICON_SIZE,
                ARMY_QUICK_ICON_SIZE,
                "mons32.icn",
                static_cast<i16>(monster),
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (stackIcons[ii] == NULL) {
                MemError();
            }
            labels[ii] = static_cast<char*>(H2_ALLOC(15));
            strcpy(
                labels[ii],
                GetArmySizeName(
                    targetHero->m_army.m_creatureCounts[idx],
                    ARMY_SIZE_NAME_TITLE
                )
            );
            sizeWidgets[ii] = new textWidget(
                static_cast<i16>(stride * ii + ARMY_QUICK_AREA_LEFT),
                static_cast<i16>(rowY + ARMY_QUICK_ICON_SIZE),
                stride,
                ARMY_QUICK_LABEL_HEIGHT,
                labels[ii],
                "smalfont.fnt",
                FONT_DRAW_DEFAULT,
                -1,
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (sizeWidgets[ii] == NULL) {
                MemError();
            }
            win->AddWidget(stackIcons[ii], -1);
            win->AddWidget(sizeWidgets[ii], -1);
            ++idx;
        }

        if (row2 != 0) {
            stride = HERO_QUICK_ARMY_AREA_WIDTH / row2;
            armyStart = (stride - ARMY_QUICK_ICON_SIZE) / ARMY_QUICK_CENTER_DIVISOR
                                + ARMY_QUICK_AREA_LEFT;
            rowY += ARMY_QUICK_SECOND_ROW_SHIFT;
            for (ii = topRow; ii < topRow + row2;
                 ++ii) {
                while (targetHero->m_army.m_creatureTypes[idx] == CREATURE_NONE) {
                    ++idx;
                }
                monster = H2EnumIndex(targetHero->m_army.m_creatureTypes[idx]);
                stackIcons[ii] = new iconWidget(
                    static_cast<i16>(
                        armyStart + stride * (ii - HERO_QUICK_SECOND_ROW_FIRST_SLOT)
                        - GetIconEntry(iconRef, monster)->x
                        + (ARMY_QUICK_ICON_SIZE - GetIconEntry(iconRef, monster)->w)
                              / ARMY_QUICK_CENTER_DIVISOR
                        + 1
                    ),
                    static_cast<i16>(
                        rowY - GetIconEntry(iconRef, monster)->y
                        + (ARMY_QUICK_ICON_BASELINE - GetIconEntry(iconRef, monster)->h)
                        + ARMY_QUICK_SECOND_ROW_ICON_SHIFT
                    ),
                    ARMY_QUICK_ICON_SIZE,
                    ARMY_QUICK_ICON_SIZE,
                    "mons32.icn",
                    static_cast<i16>(monster),
                    ICON_DRAW_NORMAL,
                    -1,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
                if (stackIcons[ii] == NULL) {
                    MemError();
                }
                labels[ii] = static_cast<char*>(H2_ALLOC(15));
                strcpy(
                    labels[ii],
                    GetArmySizeName(
                        targetHero->m_army.m_creatureCounts[idx],
                        ARMY_SIZE_NAME_TITLE
                    )
                );
                sizeWidgets[ii] = new textWidget(
                    static_cast<i16>(
                        stride * (ii - HERO_QUICK_SECOND_ROW_FIRST_SLOT)
                        + ARMY_QUICK_AREA_LEFT
                    ),
                    static_cast<i16>(rowY + HERO_QUICK_SECOND_ROW_TEXT_SHIFT),
                    stride,
                    ARMY_QUICK_LABEL_HEIGHT,
                    labels[ii],
                    "smalfont.fnt",
                    FONT_DRAW_DEFAULT,
                    -1,
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
                if (sizeWidgets[ii] == NULL) {
                    MemError();
                }
                win->AddWidget(stackIcons[ii], -1);
                win->AddWidget(sizeWidgets[ii], -1);
                ++idx;
            }
        }
    }

    oldX = m_mapOriginX;
    savedY = m_mapOriginY;
    m_mapOriginX = targetHero->m_x - VIEW_CENTER_CELL;
    m_mapOriginY = targetHero->m_y - VIEW_CENTER_CELL;
    UpdateRadar(1, 0);
    gpWindowManager->AddWindow(win, -1, 1);
    QuickViewWait();
    gpWindowManager->RemoveWindow(win);
    delete win;
    m_mapOriginX = oldX;
    m_mapOriginY = savedY;
    UpdateRadar(1, 0);
    CompleteDraw(0);
    UpdateScreen(0, 0);
    if (msg.type == MESSAGE_LEFT_BUTTON_DOWN
        && targetHero->m_owner == giCurPlayer) {
        SetHeroContext(targetHero->m_id, 0);
    }
    gpResourceManager->Dispose(iconRef);
}

char* advManager::GetArmySizeName(i32 armySize, ArmySizeNameVariant grammar) {
    if (giDebugLevel > 0) {
        sprintf(
            cArmySizeName,
            "%d",
            armySize
        );
        return cArmySizeName;
    }
    if (armySize < ARMY_FEW_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_FEW][H2EnumIndex(grammar)];
    }
    if (armySize < ARMY_SEVERAL_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_SEVERAL][H2EnumIndex(grammar)];
    }
    if (armySize < ARMY_PACK_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_PACK][H2EnumIndex(grammar)];
    }
    if (armySize < ARMY_LOTS_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_LOTS][H2EnumIndex(grammar)];
    }
    if (armySize < ARMY_HORDE_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_HORDE][H2EnumIndex(grammar)];
    }
    if (armySize < ARMY_THRONG_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_THRONG][H2EnumIndex(grammar)];
    }
    if (armySize < ARMY_SWARM_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_SWARM][H2EnumIndex(grammar)];
    }
    if (armySize < ARMY_ZOUNDS_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_ZOUNDS][H2EnumIndex(grammar)];
    }
    return gArmySizeNames[ARMY_SIZE_LEGION][H2EnumIndex(grammar)];
}

void advManager::TownQuickView(i32 townId, i32 locatorSlot, i32 windowX, i32 windowY) {

    i32 creatureCount;
    i16 creatureIconHeight;
    i16 on;
    heroWindow* window;
    town* townPtr;
    i16 playerColorWidget;
    i32 oldX;
    i32 scouting;
    i32 oldY;
    i32 armyIndex;
    i16 leftEdge;
    i16 creatureIconWidth;
    i16 faceWidget;
    tag_message message;
    icon* creatureIcon;
    i16 armyAreaWidth;
    widget* noArmyText;
    char* blankLabel;

    armyAreaWidth = TOWN_QUICK_ARMY_AREA_WIDTH;
    leftEdge = ARMY_QUICK_AREA_LEFT;
    creatureIconWidth = ARMY_QUICK_ICON_SIZE;
    creatureIconHeight = ARMY_QUICK_ICON_SIZE;
    on = 1;
    faceWidget = TOWN_QUICK_PORTRAIT_WIDGET;
    playerColorWidget = TOWN_QUICK_PLAYER_COLOR_WIDGET;

    if (townId == INVALID_HERO) {
        return;
    }

    creatureIcon = gpResourceManager->GetIcon("mons32.icn");
    townPtr = gpGame->GetTown(townId);
    if (windowX == -1) {
        windowX = TOWN_QUICK_DEFAULT_WINDOW_X;
        windowY = TOWN_QUICK_DEFAULT_WINDOW_Y;
    }
    window = new heroWindow(
        windowX,
        windowY,
        "qtown1.bin"
    );
    if (window == NULL) {
        MemError();
    }

    if (townPtr->m_owner == giCurPlayer || giDebugLevel >= TOWN_QUICK_DEBUG_INFORMATION) {
        scouting = TOWN_QUICK_INFORMATION_EXACT;
    } else {
        scouting = gpGame->GetNumThievesGuilds(giCurPlayer);
        if (scouting > TOWN_QUICK_INFORMATION_ESTIMATES) {
            scouting = TOWN_QUICK_INFORMATION_ESTIMATES;
        }
    }
    if (IsCrystalBallInEffect(townPtr->m_x, townPtr->m_y, CRYSTAL_BALL_RADIUS)) {
        scouting = TOWN_QUICK_INFORMATION_EXACT;
    }

    SetWinText(window, TOWN_QUICK_WINDOW_TEXT);
    creatureCount = 0;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    message.payload.widget.id = TOWN_QUICK_PORTRAIT_WIDGET;
    message.payload.widget.data.value =
        H2EnumIndex(townPtr->m_type) + TOWN_QUICK_TYPE_FRAME_BASE;
    if ((gpGame->GetTown(townId)->m_buildings & (1 << H2EnumIndex(BUILDING_SLOT_CASTLE))) == 0) {
        message.payload.widget.data.value += TOWN_QUICK_VILLAGE_FRAME_OFFSET;
    }
    window->BroadcastMessage(message);

    if (scouting != TOWN_QUICK_INFORMATION_EXACT
        || H2BitTest(gpGame->m_knownTowns, static_cast<i8>(townPtr->m_id)) == 0) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.id = TOWN_QUICK_KNOWN_MARKER_WIDGET;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        window->BroadcastMessage(message);
    }

    if (townPtr->m_owner == -1) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.id = TOWN_QUICK_PLAYER_COLOR_WIDGET;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        window->BroadcastMessage(message);
        ++message.payload.widget.id;
        window->BroadcastMessage(message);
    } else {
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = TOWN_QUICK_PLAYER_COLOR_WIDGET;
        message.payload.widget.data.value =
            gpGame->m_players[townPtr->m_owner].m_color * HERO_QUICK_PLAYER_COLOR_STRIDE;
        window->BroadcastMessage(message);
        ++message.payload.widget.id;
        ++message.payload.widget.data.value;
        window->BroadcastMessage(message);
    }

    sprintf(gText, GetTownName(static_cast<i8>(townPtr->m_id)));
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = TOWN_QUICK_NAME_WIDGET;
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);

    creatureCount = 0;
    for (armyIndex = 0; armyIndex < ARMY_QUICK_SLOT_COUNT; ++armyIndex) {
        if (townPtr->m_army.m_creatureTypes[armyIndex] != CREATURE_NONE) {
            ++creatureCount;
        }
    }

    if (scouting == TOWN_QUICK_INFORMATION_UNKNOWN || creatureCount == 0) {
        blankLabel = static_cast<char*>(H2_ALLOC(TOWN_QUICK_EMPTY_LABEL_CAPACITY));
        if (scouting == TOWN_QUICK_INFORMATION_UNKNOWN) {
            sprintf(
                blankLabel,
                "\xcd\xe5\xe8\xe7\xe2\xe5\xf1\xf2\xed\xee"
            );
        } else {
            sprintf(
                blankLabel,
                "\xcd\xe5\xf2"
            );
        }
        noArmyText = new textWidget(
            TOWN_QUICK_EMPTY_LABEL_X,
            TOWN_QUICK_EMPTY_LABEL_Y,
            TOWN_QUICK_EMPTY_LABEL_WIDTH,
            ARMY_QUICK_LABEL_HEIGHT,
            blankLabel,
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            -1,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (noArmyText == NULL) {
            MemError();
        }
        window->AddWidget(noArmyText, -1);
    } else {
        i32 wIndex;
        textWidget* sizeWidgets[ARMY_QUICK_SLOT_COUNT];
        i32 monster;
        iconWidget* stackIcons[ARMY_QUICK_SLOT_COUNT];
        i32 row2;
        i32 stride;
        i32 armySlot;
        char* troopNames[ARMY_QUICK_SLOT_COUNT];
        i32 fiveShift;
        i32 basePos;
        i32 curY;
        i32 creatureSlot;
        i32 topRow;

        curY = TOWN_QUICK_FIRST_ROW_Y;
        switch (creatureCount) {
            case 1:
            case ARMY_QUICK_FIRST_ROW_COUNT:
            case ARMY_QUICK_TOP_ROW_MAX:
                curY += ARMY_QUICK_FIRST_ROW_SHIFT;
                topRow = creatureCount;
                row2 = 0;
                break;
            case ARMY_QUICK_FOUR_STACK_COUNT:
                topRow = ARMY_QUICK_FIRST_ROW_COUNT;
                row2 = ARMY_QUICK_FIRST_ROW_COUNT;
                break;
            default:
                topRow = ARMY_QUICK_FIRST_ROW_COUNT;
                row2 = ARMY_QUICK_TOP_ROW_MAX;
                break;
        }

        armySlot = 0;
        wIndex = 0;
        creatureSlot = 0;
        stride = TOWN_QUICK_ARMY_AREA_WIDTH / topRow;
        basePos = (stride - ARMY_QUICK_ICON_SIZE) / ARMY_QUICK_CENTER_DIVISOR
                         + ARMY_QUICK_AREA_LEFT;
        fiveShift = 0;
        for (armyIndex = 0; armyIndex < topRow; ++armyIndex) {
            if (creatureCount == ARMY_QUICK_FIVE_STACK_COUNT) {
                fiveShift =
                    armyIndex == 0 ? ARMY_QUICK_FIVE_STACK_X_SHIFT : -ARMY_QUICK_FIVE_STACK_X_SHIFT;
            }
            while (townPtr->m_army.m_creatureTypes[creatureSlot] == CREATURE_NONE) {
                ++creatureSlot;
            }
            monster = H2EnumIndex(townPtr->m_army.m_creatureTypes[creatureSlot]);
            stackIcons[wIndex] = new iconWidget(
                static_cast<i16>(
                    basePos + stride * wIndex + fiveShift
                    - GetIconEntry(creatureIcon, monster)->x
                    + (ARMY_QUICK_ICON_SIZE - GetIconEntry(creatureIcon, monster)->w)
                          / ARMY_QUICK_CENTER_DIVISOR
                    + 1
                ),
                static_cast<i16>(
                    curY - GetIconEntry(creatureIcon, monster)->y
                    + (ARMY_QUICK_ICON_BASELINE - GetIconEntry(creatureIcon, monster)->h)
                ),
                ARMY_QUICK_ICON_SIZE,
                ARMY_QUICK_ICON_SIZE,
                "mons32.icn",
                static_cast<i16>(monster),
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (stackIcons[wIndex] == NULL) {
                MemError();
            }
            troopNames[wIndex] =
                static_cast<char*>(H2_ALLOC(TOWN_QUICK_ARMY_LABEL_CAPACITY));
            if (scouting == TOWN_QUICK_INFORMATION_EXACT) {
                sprintf(
                    troopNames[wIndex],
                    "%d",
                    townPtr->m_army.m_creatureCounts[creatureSlot]
                );
            } else if (scouting == TOWN_QUICK_INFORMATION_ESTIMATES) {
                strcpy(
                    troopNames[wIndex],
                    GetArmySizeName(
                        townPtr->m_army.m_creatureCounts[creatureSlot],
                        ARMY_SIZE_NAME_TITLE
                    )
                );
            } else {
                strcpy(
                    troopNames[wIndex],
                    "???"
                );
            }
            sizeWidgets[wIndex] = new textWidget(
                static_cast<i16>(
                    basePos + stride * wIndex + fiveShift
                    - ARMY_QUICK_TEXT_X_ADJUSTMENT
                ),
                static_cast<i16>(curY + ARMY_QUICK_ICON_SIZE),
                ARMY_QUICK_TEXT_WIDTH,
                ARMY_QUICK_LABEL_HEIGHT,
                troopNames[wIndex],
                "smalfont.fnt",
                FONT_DRAW_DEFAULT,
                -1,
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (sizeWidgets[wIndex] == NULL) {
                MemError();
            }
            window->AddWidget(stackIcons[wIndex], -1);
            window->AddWidget(sizeWidgets[wIndex], -1);
            ++wIndex;
            ++creatureSlot;
        }

        if (row2 != 0) {
            stride = TOWN_QUICK_ARMY_AREA_WIDTH / row2;
            basePos = (stride - ARMY_QUICK_ICON_SIZE) / ARMY_QUICK_CENTER_DIVISOR
                             + ARMY_QUICK_AREA_LEFT;
            curY += ARMY_QUICK_SECOND_ROW_SHIFT;
            for (armyIndex = topRow;
                 armyIndex < topRow + row2;
                 ++armyIndex) {
                while (townPtr->m_army.m_creatureTypes[creatureSlot] == CREATURE_NONE) {
                    ++creatureSlot;
                }
                monster = H2EnumIndex(townPtr->m_army.m_creatureTypes[creatureSlot]);
                stackIcons[wIndex] = new iconWidget(
                    static_cast<i16>(
                        basePos + stride * (wIndex - topRow)
                        - GetIconEntry(creatureIcon, monster)->x
                        + (ARMY_QUICK_ICON_SIZE - GetIconEntry(creatureIcon, monster)->w)
                              / ARMY_QUICK_CENTER_DIVISOR
                        + 1
                    ),
                    static_cast<i16>(
                        curY - GetIconEntry(creatureIcon, monster)->y
                        + (ARMY_QUICK_ICON_BASELINE - GetIconEntry(creatureIcon, monster)->h)
                    ),
                    ARMY_QUICK_ICON_SIZE,
                    ARMY_QUICK_ICON_SIZE,
                    "mons32.icn",
                    static_cast<i16>(monster),
                    ICON_DRAW_NORMAL,
                    -1,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
                if (stackIcons[wIndex] == NULL) {
                    MemError();
                }
                troopNames[wIndex] =
                    static_cast<char*>(H2_ALLOC(TOWN_QUICK_ARMY_LABEL_CAPACITY));
                if (scouting == TOWN_QUICK_INFORMATION_EXACT) {
                    sprintf(
                        troopNames[wIndex],
                        "%d",
                        townPtr->m_army.m_creatureCounts[creatureSlot]
                    );
                } else if (scouting == TOWN_QUICK_INFORMATION_ESTIMATES) {
                    strcpy(
                        troopNames[wIndex],
                        GetArmySizeName(
                            townPtr->m_army.m_creatureCounts[creatureSlot],
                            ARMY_SIZE_NAME_TITLE
                        )
                    );
                } else {
                    strcpy(
                        troopNames[wIndex],
                        "???"
                    );
                }
                sizeWidgets[wIndex] = new textWidget(
                    static_cast<i16>(
                        basePos + stride * (wIndex - topRow)
                        - ARMY_QUICK_TEXT_X_ADJUSTMENT
                    ),
                    static_cast<i16>(curY + ARMY_QUICK_ICON_SIZE),
                    ARMY_QUICK_TEXT_WIDTH,
                    ARMY_QUICK_LABEL_HEIGHT,
                    troopNames[wIndex],
                    "smalfont.fnt",
                    FONT_DRAW_DEFAULT,
                    -1,
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
                if (sizeWidgets[wIndex] == NULL) {
                    MemError();
                }
                window->AddWidget(stackIcons[wIndex], -1);
                window->AddWidget(sizeWidgets[wIndex], -1);
                ++wIndex;
                ++creatureSlot;
            }
        }
    }

    gpWindowManager->AddWindow(window, -1, 1);
    oldX = m_mapOriginX;
    oldY = m_mapOriginY;
    m_mapOriginX = townPtr->m_x - VIEW_CENTER_CELL;
    m_mapOriginY = townPtr->m_y - VIEW_CENTER_CELL;
    UpdateRadar(1, 0);
    QuickViewWait();
    gpWindowManager->RemoveWindow(window);
    delete window;
    m_mapOriginX = oldX;
    m_mapOriginY = oldY;
    UpdateRadar(1, 0);
    CompleteDraw(0);
    UpdateScreen(0, 0);
    if (message.type == MESSAGE_LEFT_BUTTON_DOWN && townPtr->m_owner == giCurPlayer) {
        SetTownContext(static_cast<i8>(townPtr->m_id));
    }
    gpResourceManager->Dispose(creatureIcon);
}

void advManager::RedrawAdvScreen(i32 update, i32 freeBorder) {

    if (!bShowIt) {
        return;
    }
    gpResourceManager->GetBackdrop(
        "advbord.icn",
        gpWindowManager->m_screen,
        1
    );
    if (freeBorder) {
        H2_FREE(m_adventureBorder);
        m_adventureBorder = NULL;
    }
    SaveAdventureBorder();
    UpdateHeroLocators(0, 0);
    UpdateTownLocators(0, 0);
    UpdBottomView(1, 0, 0);
    m_adventureWindow->DrawWindow(0);
    if (update) {
        gpWindowManager->UpdateScreenRegion(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    UpdateRadar(update, 0);
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    if (update) {
        UpdateScreen(0, 0);
    }
}

void advManager::DeactivateCurrTown(void) {
    gpCurPlayer->m_currentTown = -1;
}

void advManager::DeactivateCurrHero(void) {
    DemobilizeCurrHero();
    gpCurPlayer->m_currentHero = -1;
}

void advManager::MobilizeCurrHero(i32 update) {
    if (gpCurPlayer->m_currentHero == INVALID_HERO) {
        return;
    }
    if (m_heroContextLocked) {
        return;
    }
    SetHeroContext(gpCurPlayer->m_currentHero, update);
}

void advManager::DemobilizeCurrHero(void) {
    if (gpCurPlayer->m_currentHero == INVALID_HERO) {
        return;
    }
    if (!m_heroContextLocked) {
        return;
    }

    m_heroContextLocked = 0;
    hero* hp = gpGame->GetHero(gpCurPlayer->m_currentHero);
    StopCursor(1);
    mapCell* cell = GetCell(hp->m_x, hp->m_y);
    hp->m_locationType = cell->m_triggerType;
    hp->m_occupiedTown = cell->m_objectMetadata;
    hp->m_direction = m_cursorDirection;
    if (m_cursorType == HERO_TYPE_BOAT) {
        hp->m_eventFlags =
            HeroEventFlag(static_cast<i32>(hp->m_eventFlags) | H2EnumIndex(HERO_EVENT_EMBARKED));
    }
    cell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION;
    cell->m_objectMetadata = hp->m_id;
    cell->m_flags &= ~CURSOR_MAP_VISIBLE_FLAG;
    m_cursorActive = 0;
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    UpdateScreen(0, 0);
}

void advManager::SetTownContext(i32 townId) {
    DeactivateCurrHero();
    gpCurPlayer->m_currentTown = static_cast<i8>(townId);
    town* tp = gpGame->GetTown(gpCurPlayer->m_currentTown);
    m_mapOriginX = tp->m_x - VIEW_CENTER_OFFSET;
    m_mapOriginY = tp->m_y - VIEW_CENTER_OFFSET;

    i32 townNo = 0;
    i32 index;
    for (index = 0; index < gpCurPlayer->m_townCount; ++index) {
        if (gpCurPlayer->m_townIds[index] == townId) {
            townNo = index;
        }
    }
    if (townNo < gpCurPlayer->m_townLocatorPage) {
        gpCurPlayer->m_townLocatorPage = static_cast<i8>(townNo);
    } else if (townNo > gpCurPlayer->m_townLocatorPage + LOCATOR_VISIBLE_COUNT - 1) {
        gpCurPlayer->m_townLocatorPage =
            static_cast<i8>(townNo - (LOCATOR_VISIBLE_COUNT - 1));
    }

    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    HideRoute(0, 0, 1);
    UpdBottomView(1, 1, 1);
    UpdateRadar(1, 0);
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    UpdateScreen(0, 0);
    SetEnvironmentOrigin(m_mapOriginX + VIEW_CENTER_OFFSET, m_mapOriginY + VIEW_CENTER_OFFSET, 1);

    townNo =
        H2EnumIndex(giGroundToTerrain[GetCell(tp->m_x, tp->m_y)
                                  ->m_terrainImageIndex]);
    if (static_cast<TerrainType>(townNo) != m_currentTerrain) {
        m_currentTerrain = townNo;
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]);
    }
    gpInputManager->ForceMouseMove();
    m_lastHoverCell = 0;
}

void advManager::SetHeroContext(i32 heroId, i32 update) {
    if (heroId == INVALID_HERO) {
        return;
    }

    DeactivateCurrTown();
    HideRoute(0, 0, 1);
    DeactivateCurrHero();
    m_heroContextLocked = 1;
    gpCurPlayer->m_currentHero = static_cast<i8>(heroId);
    hero* contextHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    m_mapOriginX = contextHero->m_x - VIEW_CENTER_OFFSET;
    m_mapOriginY = contextHero->m_y - VIEW_CENTER_OFFSET;
    m_cursorMapY = VIEW_CENTER_CELL;
    m_cursorMapX = VIEW_CENTER_CELL;
    m_previousCursorMapY = CURSOR_INVALID_POSITION;
    m_previousCursorMapX = CURSOR_INVALID_POSITION;
    m_cursorType = (H2EnumIndex((contextHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))
        ? HERO_TYPE_BOAT
        : static_cast<HeroCursorType>(contextHero->m_cursorType);
    m_cursorDirection = contextHero->m_direction;
    m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);

    mapCell* currentCell = GetCell(contextHero->m_x, contextHero->m_y);
    currentCell->m_flags |= CURSOR_MAP_VISIBLE_FLAG;
    gpGame->RestoreCell(
        contextHero->m_x,
        contextHero->m_y,
        contextHero->m_locationType,
        contextHero->m_occupiedTown,
        NULL,
        4
    );

    i32 heroSlot = 0;
    i32 slotIndex;
    for (slotIndex = 0; slotIndex < gpCurPlayer->m_heroCount; ++slotIndex) {
        if (gpCurPlayer->m_heroIds[slotIndex] == heroId) {
            heroSlot = slotIndex;
        }
    }
    if (heroSlot < gpCurPlayer->m_heroLocatorPage) {
        gpCurPlayer->m_heroLocatorPage = static_cast<i8>(heroSlot);
    } else if (heroSlot > gpCurPlayer->m_heroLocatorPage + LOCATOR_VISIBLE_COUNT - 1) {
        gpCurPlayer->m_heroLocatorPage =
            static_cast<i8>(heroSlot - (LOCATOR_VISIBLE_COUNT - 1));
    }

    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    if (!update && (m_active == 1 || gbThisNetHumanPlayer[giCurPlayer])) {
        Reseed(0, 0);
        SeedTo(contextHero->m_destinationX, contextHero->m_destinationY);
        ShowRoute(0, 0, !update);
    }
    UpdBottomView(1, 1, 1);
    m_cursorActive = 1;
    UpdateRadar(1, 0);
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    UpdateScreen(0, 0);
    SetEnvironmentOrigin(m_mapOriginX + VIEW_CENTER_OFFSET, m_mapOriginY + VIEW_CENTER_OFFSET, 1);

    heroSlot = static_cast<i32>(giGroundToTerrain[currentCell->m_terrainImageIndex]);
    if (static_cast<TerrainType>(heroSlot) != m_currentTerrain) {
        m_currentTerrain = heroSlot;
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]);
    }
    if (!gbHeroMoving) {
        gpInputManager->ForceMouseMove();
        m_lastHoverCell = 0;
    }
}

void advManager::DoHeroKnob(void) {
    i32 prevPage = gpCurPlayer->m_heroLocatorPage;
    i32 count = gpCurPlayer->m_heroCount;
    i32 pageIndex;
    double scale = ADVMGR_LOCATOR_HERO_SCROLL_SPAN_DOUBLE / (count - LOCATOR_VISIBLE_COUNT);
    i32 x;
    i32 y;
    gpMouseManager->MouseCoords(x, y);
    i32 offset = y - m_scrollLeftButton->m_y;
    gpInputManager->Flush();
    tag_message message;
    message = gpInputManager->GetEvent();

    while (message.type != MESSAGE_LEFT_BUTTON_UP && message.type != MESSAGE_RIGHT_BUTTON_UP) {
        if (message.type == MESSAGE_MOUSE_MOVE) {
            if (message.payload.mouse.y < offset + LOCATOR_SCROLL_BASE_Y) {
                message.payload.mouse.y = offset + LOCATOR_SCROLL_BASE_Y;
            }
            if (message.payload.mouse.y > offset + LOCATOR_KNOB_MAX_Y) {
                message.payload.mouse.y = offset + LOCATOR_KNOB_MAX_Y;
            }
            gpMouseManager->Main(message);
            m_scrollLeftButton->m_y = message.payload.mouse.y - offset;
            m_adventureWindow->DrawWindow();
            if (count > LOCATOR_VISIBLE_COUNT) {
                pageIndex = static_cast<i32>(
                    (m_scrollLeftButton->m_y - LOCATOR_SCROLL_BASE_Y) / scale
                );
                if (pageIndex != prevPage) {
                    gpCurPlayer->m_heroLocatorPage = static_cast<i8>(pageIndex);
                    if (pageIndex > count - (LOCATOR_VISIBLE_COUNT - 1)) {
                        pageIndex = count - (LOCATOR_VISIBLE_COUNT - 1);
                    }
                    UpdateHeroLocators(0, 1);
                    m_scrollLeftButton->m_y = message.payload.mouse.y - offset;
                    m_adventureWindow->DrawWindow();
                    prevPage = pageIndex;
                }
            }
        }
        platform::PumpEvents();
        message = gpInputManager->GetEvent();
    }
    m_scrollLeftButton->m_flags &= ~WIDGET_FLAG_SELECTED;
    UpdateHeroLocators(1, 1);
}

void advManager::DoTownKnob(void) {
    i32 prevPage = gpCurPlayer->m_townLocatorPage;
    i32 count = gpCurPlayer->m_townCount;
    i32 pageIndex;
    double scale = ADVMGR_LOCATOR_HERO_SCROLL_SPAN_DOUBLE / (count - LOCATOR_VISIBLE_COUNT);
    i32 x;
    i32 y;
    gpMouseManager->MouseCoords(x, y);
    i32 offset = y - m_scrollRightButton->m_y;
    gpInputManager->Flush();
    tag_message message;
    message = gpInputManager->GetEvent();

    while (message.type != MESSAGE_LEFT_BUTTON_UP && message.type != MESSAGE_RIGHT_BUTTON_UP) {
        if (message.type == MESSAGE_MOUSE_MOVE) {
            if (message.payload.mouse.y < offset + LOCATOR_SCROLL_BASE_Y) {
                message.payload.mouse.y = offset + LOCATOR_SCROLL_BASE_Y;
            }
            if (message.payload.mouse.y > offset + LOCATOR_KNOB_MAX_Y) {
                message.payload.mouse.y = offset + LOCATOR_KNOB_MAX_Y;
            }
            gpMouseManager->Main(message);
            m_scrollRightButton->m_y = message.payload.mouse.y - offset;
            m_adventureWindow->DrawWindow();
            if (count > LOCATOR_VISIBLE_COUNT) {
                pageIndex = static_cast<i32>(
                    (m_scrollRightButton->m_y - LOCATOR_SCROLL_BASE_Y) / scale
                );
                if (pageIndex != prevPage) {
                    gpCurPlayer->m_townLocatorPage = static_cast<i8>(pageIndex);
                    if (pageIndex > count - (LOCATOR_VISIBLE_COUNT - 1)) {
                        pageIndex = count - (LOCATOR_VISIBLE_COUNT - 1);
                    }
                    UpdateTownLocators(0, 1);
                    m_scrollRightButton->m_y = message.payload.mouse.y - offset;
                    m_adventureWindow->DrawWindow();
                    prevPage = pageIndex;
                }
            }
        }
        platform::PumpEvents();
        message = gpInputManager->GetEvent();
    }
    m_scrollRightButton->m_flags &= ~WIDGET_FLAG_SELECTED;
    UpdateTownLocators(1, 1);
}

void advManager::CastSpell(SpellType spell) {
    hero* hero;
    CreatureType mineGuard;
    i32 guardianCount;
    mapCell* cell;
    i32 power;
    if (gpCurPlayer->m_currentHero != INVALID_HERO) {
        hero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    } else {
        hero = NULL;
    }

    switch (spell) {
        case SPELL_SET_EARTH_GUARDIAN:
            mineGuard = CREATURE_EARTH_ELEMENTAL;
            goto setMineGuardian;
        case SPELL_SET_AIR_GUARDIAN:
            mineGuard = CREATURE_AIR_ELEMENTAL;
            goto setMineGuardian;
        case SPELL_SET_FIRE_GUARDIAN:
            mineGuard = CREATURE_FIRE_ELEMENTAL;
            goto setMineGuardian;
        case SPELL_SET_WATER_GUARDIAN:
            mineGuard = CREATURE_WATER_ELEMENTAL;
            goto setMineGuardian;
        case SPELL_HAUNT:
            mineGuard = CREATURE_GHOST;
            goto setMineGuardian;
        setMineGuardian:
            cell = gpAdvManager->GetCell(hero->m_x, hero->m_y);
            if (cell->m_triggerType != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE)) {
                NormalDialog(
                    "\xc2\xfb \xe4\xee\xeb\xe6\xed\xfb \xe2\xf1\xf2\xe0\xf2\xfc \xf3 \xe2\xf5\xee\xe4\xe0 \xef\xe5\xf0\xe5\xe4 \xf8\xe0\xf5\xf2\xee\xe9 (\xeb\xe5\xf1\xee\xef\xe8\xeb\xea\xee\xe9 \xe8\xeb\xe8 \xeb\xe0\xe1\xee\xf0\xe0\xf2\xee\xf0\xe8\xe5\xe9 \xe0\xeb\xf5\xe8\xec\xe8\xea\xe0), \xf7\xf2\xee\xe1\xfb \xed\xe0\xef\xf0\xe0\xe2\xe8\xf2\xfc \xfd\xf2\xee \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5.",
                    1,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                return;
            }
            gpGame->m_mines[cell->m_objectMetadata].guardianType =
                mineGuard;
            power = hero->Stats(HERO_PRIMARY_SPELL_POWER);
            if (power > MINE_GUARDIAN_MAX_POWER) {
                power = MINE_GUARDIAN_MAX_POWER;
            }
            gpGame->m_mines[cell->m_objectMetadata].guardianCount =
                static_cast<u8>(power * MINE_GUARDIANS_PER_POWER);
            if (spell == SPELL_HAUNT) {
                gpGame->ClaimMine(cell->m_objectMetadata, -1);
            }
            break;
        case SPELL_VIEW_MINES:
        case SPELL_VIEW_RESOURCES:
        case SPELL_VIEW_ARTIFACTS:
        case SPELL_VIEW_TOWNS:
        case SPELL_VIEW_HEROES:
        case SPELL_VIEW_ALL:
            ViewWorld(spell, spell == SPELL_VIEW_ALL, spell == SPELL_VIEW_ALL);
            break;
        case SPELL_IDENTIFY_HERO:
            m_identifyHeroActive = 1;
            NormalDialog(
                "\xd2\xe5\xef\xe5\xf0\xfc \xe2\xf1\xe5 \xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xe5 \xe3\xe5\xf0\xee\xe8 \xef\xee\xeb\xed\xee\xf1\xf2\xfc\xfe \xee\xef\xee\xe7\xed\xe0\xed\xfb.",
                1,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
            break;
        case SPELL_SUMMON_BOAT:
            SummonBoat();
            break;
        case SPELL_DIMENSION_DOOR:
        case SPELL_TOWN_GATE:
        case SPELL_TOWN_PORTAL:
            if (hero->m_remainingMobility == 0) {
                NormalDialog(
                    "\xc2\xe0\xf8 \xe3\xe5\xf0\xee\xe9 \xf1\xeb\xe8\xf8\xea\xee\xec \xe8\xe7\xec\xee\xf2\xe0\xed, \xf7\xf2\xee\xe1\xfb \xed\xe0\xef\xf0\xe0\xe2\xeb\xff\xf2\xfc \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff \xf1\xe5\xe3\xee\xe4\xed\xff. \xcf\xee\xef\xf0\xee\xe1\xf3\xe9\xf2\xe5 \xe7\xe0\xe2\xf2\xf0\xe0.",
                    1,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                return;
            }
            if (hero->m_remainingMobility < TRAVEL_SPELL_MOBILITY_COST) {
                hero->m_remainingMobility = 0;
            } else {
                hero->m_remainingMobility -= TRAVEL_SPELL_MOBILITY_COST;
            }
            UpdateHeroLocator(-1, 1, 1);
            if (spell == SPELL_DIMENSION_DOOR) {
                DimensionDoor();
            } else {
                TownGate(spell);
            }
            break;
        case SPELL_VISIONS:
            if (!DoVisions(hero)) {
                return;
            }
            break;
        default:
            break;
    }

    if (spell != SPELL_DIMENSION_DOOR && spell != SPELL_TOWN_GATE && spell != SPELL_TOWN_PORTAL) {
        gpGame->GetHero(gpCurPlayer->m_currentHero)->UseSpell(spell);
    }
}

i32 SaveGame(void) {
    i32 ok = 0;
    i32 nPlayers = 0;
    gpAdvManager->DisableButtons();
    gpMouseManager->SetPointer(
        "advmice.mse",
        SAVE_POINTER_FRAME,
        MOUSE_AUTO_CURSOR_TYPE
    );
    i32 i;
    for (i = 0; i < SAVE_PLAYER_COUNT; ++i) {
        if (!gpGame->m_playerDead[i] && gbHumanPlayer[i]) {
            ++nPlayers;
        }
    }

    char suffix[SAVE_EXTENSION_SIZE];
    char pattern[SAVE_PATTERN_SIZE];
    if (gbInCampaign) {
        sprintf(suffix, ".GMC");
        sprintf(pattern, "*.GMC");
    } else if (xIsPlayingExpansionCampaign) {
        sprintf(suffix, ".GXC");
        sprintf(pattern, "*.GXC");
    } else if (xIsExpansionMap) {
        sprintf(suffix, ".GX%d", nPlayers);
        sprintf(pattern, "*.GX%d", nPlayers);
    } else {
        sprintf(suffix, ".GM%d", nPlayers);
        sprintf(pattern, "*.GM%d", nPlayers);
    }

    fileRequester* req = new fileRequester(
        SAVE_REQUESTER_X,
        SAVE_REQUESTER_Y,
        FILE_REQUESTER_SAVE_GAME,
        pattern,
        gcGamePath,
        suffix
    );
    if (req == NULL) {
        MemError();
    }
    i32 status = gpExec->DoDialog(req);
    if (status == FILE_REQUESTER_OK) {
        ok = 1;
        bFreshSave = 1;
        ok = gpGame->SaveGame(gLastFilename, 0, 0);
        if (ok) {
            NormalDialog(
                "\xc8\xe3\xf0\xe0 \xf3\xf1\xef\xe5\xf8\xed\xee \xf1\xee\xf5\xf0\xe0\xed\xe5\xed\xe0."
                     ,
                1,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
        }
    }
    delete req;
    gpAdvManager->EnableButtons();
    return ok;
}

void advManager::CheckCastSpell(void) {
    if (gpCurPlayer->m_currentHero != INVALID_HERO) {
        MobilizeCurrHero(0);
        CompleteDraw(0);
        UpdateScreen(0, 0);
        gpMouseManager->SetPointer(
            "advmice.mse",
            POINTER_DEFAULT,
            MOUSE_AUTO_CURSOR_TYPE
        );
        CastSpell(gpGame->ViewSpells(
            gpGame->GetHero(gpCurPlayer->m_currentHero),
            SPELL_TYPE_ADVENTURE,
            NullHandler,
            0
        ));
    }
}

MessageDispatchResult DimensionDoorHandler(tag_message& message) {
    if (glTimers[0] < platform::Ticks()) {
        gpAdvManager->CompleteDraw(gpAdvManager->m_mapOriginX, gpAdvManager->m_mapOriginY, 0, 1);
        gpAdvManager->UpdateScreen(0, 0);
    }

    i32 mouseX = message.payload.mouse.screenX;
    i32 mouseY = message.payload.mouse.screenY;
    i32 result = 0;

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_SELECT:
                    switch (message.payload.widget.id) {
                        case DIMENSION_DOOR_FIRST_BUTTON:
                        case DIMENSION_DOOR_LAST_BUTTON:
                            if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                            } else {
                                if (gpWindowManager->m_dialogResult == TRAVEL_DIALOG_ACCEPT) {
                                    result = 1;
                                }
                            }
                            break;
                    }
                    break;
                case WIDGET_COMMAND_DESELECT:
                    switch (message.payload.widget.id) {
                        case DIMENSION_DOOR_CLOSE_BUTTON:
                            gpWindowManager->m_dialogResult = 0;
                            result = 1;
                            break;
                    }
                    break;
            }
            break;

        case MESSAGE_MOUSE_MOVE:
            if (InMapArea(message.payload.mouse.screenX, message.payload.mouse.screenY)) {
                mouseX /= CELL_PIXELS;
                mouseY /= CELL_PIXELS;
                if (mouseX < 0) {
                    mouseX = 0;
                }
                if (mouseY < 0) {
                    mouseY = 0;
                }
                if (mouseX > DRAW_LAST_CELL) {
                    mouseX = DRAW_LAST_CELL;
                }
                if (mouseY > DRAW_LAST_CELL) {
                    mouseY = DRAW_LAST_CELL;
                }

                if (gpAdvManager->m_lastHoverCell != mouseX
                    || gpAdvManager->m_hoverCellY != mouseY) {
                    gpAdvManager->m_lastHoverCell = mouseX;
                    gpAdvManager->m_hoverCellY = mouseY;
                    mapCell* cell = gpAdvManager->GetCell(
                        gpAdvManager->m_mapOriginX + mouseX,
                        gpAdvManager->m_mapOriginY + mouseY
                    );
                    if ((H2EnumIndex((cell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))
                        || (cell->m_flags & HOVER_OBJECT_BLOCKED)) {
                        gpWindowManager->m_dialogResult = 0;
                        gpMouseManager->SetPointer(POINTER_DEFAULT);
                    } else {
                        gpWindowManager->m_dialogResult = TRAVEL_DIALOG_ACCEPT;
                        gpMouseManager->SetPointer(POINTER_MOVE);
                    }
                }
            } else {
                gpWindowManager->m_dialogResult = 0;
                gpMouseManager->SetPointer(POINTER_DEFAULT);
                return MESSAGE_DISPATCH_CONSUME;
            }
            break;
    }

    if (result) {
        message.payload.widget.id = DIMENSION_DOOR_FIRST_BUTTON;
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

i32 advManager::ComboDraw(i32 originX, i32 originY, i32 animate) {
    i32 updateCount;
    i32 tileX;
    i32 drawX;
    i32 tileY;
    i32 drawY;
    mapCell* cell;

    PollSound();
    if (bShowIt == 0) {
        return 0;
    }

    if (m_forceCompleteDraw != 0) {
        CompleteDraw(originX, originY, 0, 1);
        return 1;
    }

    if (animate != 0) {
        giFrameCount += giFrameStep;
        if (giFrameCount < COMBO_FRAME_LIMIT) {
            platform::PumpEvents();
            if (glTimers[0] < platform::Ticks()) {
                glTimers[0] = platform::Ticks() + TIMER_DELAY;
            }
            PollSound();
            return 0;
        } else {
            giFrameCount = 0;
        }
    }

    m_previousOriginX = m_mapOriginX;
    m_previousOriginY = m_mapOriginY;
    memset(bComboDraw, 0, COMBO_CLEAR_BYTES);
    m_comboHeroDrawn = 0;

    for (drawY = 0; drawY < COMBO_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
            if (originX + drawX >= 0 && originX + drawX < MAP_WIDTH && originY + drawY >= 0
                && originY + drawY < MAP_HEIGHT) {
                cell = GetCell(originX + drawX, originY + drawY);

                if (cell->m_animatedObject || cell->m_animatedOverlay) {
                    ++bComboDraw[drawX][drawY];
                }
                if ((cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_WINDMILL) {
                    ++bComboDraw[drawX][drawY];
                }
                if ((cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_ALCHEMIST_LAB) {
                    ++bComboDraw[drawX][drawY];
                }

                if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER)) {
                    ++bComboDraw[drawX][drawY];
                    ++bComboDraw[drawX - 1][drawY];
                    if (GetCloudLookup(drawX + originX, drawY + originY) != 0) {
                        bComboDraw[drawX + 1][drawY] += COMBO_CLOUD_MARK;
                        if (drawY >= 1) {
                            bComboDraw[drawX][drawY - 1] += COMBO_CLOUD_MARK;
                            bComboDraw[drawX + 1][drawY - 1] += COMBO_CLOUD_MARK;
                        }
                    } else {
                        ++bComboDraw[drawX + 1][drawY];
                        if (drawY >= 1) {
                            ++bComboDraw[drawX][drawY - 1];
                            ++bComboDraw[drawX + 1][drawY - 1];
                        }
                    }
                }

                if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                    || cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT)) {
                    ++bComboDraw[drawX][drawY];
                    if (GetCloudLookup(drawX + originX, drawY + originY) != 0) {
                        bComboDraw[drawX + 1][drawY] += COMBO_CLOUD_MARK;
                        bComboDraw[drawX][drawY + 1] += COMBO_CLOUD_MARK;
                        bComboDraw[drawX + 1][drawY + 1] += COMBO_CLOUD_MARK;
                        bComboDraw[drawX + COMBO_FAR_NEIGHBOR_OFFSET][drawY] += COMBO_CLOUD_MARK;
                        if (drawY >= 1) {
                            bComboDraw[drawX][drawY - 1] += COMBO_CLOUD_MARK;
                        }
                        if (drawX >= 1) {
                            bComboDraw[drawX - 1][drawY] += COMBO_CLOUD_MARK;
                            *(bComboDraw[drawX - 1] + drawY + 1) += COMBO_CLOUD_MARK;
                            if (drawX >= COMBO_FAR_NEIGHBOR_OFFSET) {
                                bComboDraw[drawX - COMBO_FAR_NEIGHBOR_OFFSET][drawY] +=
                                    COMBO_CLOUD_MARK;
                            }
                            if (drawY >= 1) {
                                ++bComboDraw[drawX - COMBO_FAR_NEIGHBOR_OFFSET][drawY - 1];
                            }
                        }
                    } else {
                        ++bComboDraw[drawX + 1][drawY];
                        ++bComboDraw[drawX][drawY + 1];
                        ++bComboDraw[drawX + 1][drawY + 1];
                        ++bComboDraw[drawX + COMBO_FAR_NEIGHBOR_OFFSET][drawY];
                        if (drawY >= 1) {
                            ++bComboDraw[drawX][drawY - 1];
                        }
                        if (drawX >= 1) {
                            ++bComboDraw[drawX - 1][drawY];
                            ++bComboDraw[drawX - 1][drawY + 1];
                            if (drawX >= COMBO_FAR_NEIGHBOR_OFFSET) {
                                ++bComboDraw[drawX - COMBO_FAR_NEIGHBOR_OFFSET][drawY];
                            }
                            if (drawY >= 1) {
                                ++bComboDraw[drawX - COMBO_FAR_NEIGHBOR_OFFSET][drawY - 1];
                            }
                        }
                    }
                }
            }
        }
    }

    for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
        for (drawY = 0; drawY < COMBO_VIEW_CELLS; ++drawY) {
            if (bComboDraw[drawX][drawY] != 0) {
                if (originX + drawX < 0 || originX + drawX >= MAP_WIDTH || originY + drawY < 0
                    || originY + drawY >= MAP_HEIGHT) {
                    *(bComboDraw[drawX] + drawY) = 0;
                } else if (*(bComboDraw[drawX] + drawY) < COMBO_CLOUD_MARK
                           && GetCloudLookup(drawX + originX, drawY + originY) == 0) {
                    *(bComboDraw[drawX] + drawY) = 0;
                }
            }
        }
    }

    if (m_heroContextLocked != 0) {
        for (drawY = COMBO_HERO_PANEL_TOP; drawY <= COMBO_HERO_PANEL_BOTTOM - 1; ++drawY) {
            for (drawX = COMBO_HERO_PANEL_LEFT; drawX <= COMBO_HERO_PANEL_RIGHT - 1; ++drawX) {
                ++bComboDraw[drawX][drawY];
            }
        }
    }

    if (m_cursorType == HERO_TYPE_BOAT) {
        ++bComboDraw[VIEW_CENTER_CELL - 1][COMBO_HERO_PANEL_LEFT];
        ++bComboDraw[VIEW_CENTER_CELL][COMBO_HERO_PANEL_LEFT];
        ++bComboDraw[VIEW_CENTER_CELL + 1][COMBO_HERO_PANEL_LEFT];
    }

    for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
        for (drawY = 0; drawY < COMBO_VIEW_CELLS; ++drawY) {
            cell = GetCell(originX + drawX, originY + drawY);
            if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE)) {
                if (gpGame->m_mines[cell->m_objectMetadata].guardianType == CREATURE_GHOST) {
                    ++bComboDraw[drawX][drawY];
                    ++bComboDraw[drawX + 1][drawY];
                    if (drawX < COMBO_VIEW_CELLS) {
                        ++bComboDraw[drawX + COMBO_FAR_NEIGHBOR_OFFSET][drawY];
                    }
                    if (drawX > 0) {
                        ++bComboDraw[drawX - 1][drawY];
                    }
                    if (drawY > 0) {
                        ++bComboDraw[drawX][drawY - 1];
                        ++bComboDraw[drawX + 1][drawY - 1];
                        if (drawX < COMBO_VIEW_CELLS) {
                            ++bComboDraw[drawX + COMBO_FAR_NEIGHBOR_OFFSET][drawY - 1];
                        }
                        if (drawX > 0) {
                            ++bComboDraw[drawX - 1][drawY - 1];
                        }
                    }
                    if (drawY > 1) {
                        ++bComboDraw[drawX][drawY - COMBO_FAR_NEIGHBOR_OFFSET];
                        ++bComboDraw[drawX + 1][drawY - COMBO_FAR_NEIGHBOR_OFFSET];
                        if (drawX < COMBO_VIEW_CELLS) {
                            ++bComboDraw[drawX + COMBO_FAR_NEIGHBOR_OFFSET]
                                        [drawY - COMBO_FAR_NEIGHBOR_OFFSET];
                        }
                        if (drawX > 0) {
                            ++bComboDraw[drawX - 1][drawY - COMBO_FAR_NEIGHBOR_OFFSET];
                        }
                    }
                } else if (drawY > 0 && bComboDraw[drawX][drawY - 1] != 0) {
                    ++bComboDraw[drawX][drawY];
                }
            }
        }
    }

    if (m_visibilityMapValid != 0 && m_visibilityMap != NULL) {
        for (drawY = 1; drawY < COMBO_VIEW_CELLS - 1; ++drawY) {
            for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
                if (bComboDraw[drawX][drawY] == 0) {
                    continue;
                }
                tileX = originX + drawX;
                tileY = originY + drawY;
                if (tileX < 0 || tileX > MAP_WIDTH - 1 || tileY < 1
                    || tileY > MAP_HEIGHT - COMBO_FAR_NEIGHBOR_OFFSET) {
                    continue;
                }
                if (ADVMGR_VISIBILITY_AT(tileX, tileY) != 0) {
                    ++bComboDraw[drawX][drawY + 1];
                }
                if (ADVMGR_VISIBILITY_AT(tileX, tileY - 1) != 0) {
                    ++bComboDraw[drawX][drawY - 1];
                }
            }
        }
    }

    gpMouseManager->m_cursorReady = 0;
    for (drawY = 0; drawY < COMBO_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
            if (bComboDraw[drawX][drawY] != 0) {
                DrawCell(originX + drawX, originY + drawY, drawX, drawY, ADVMGR_DRAW_GROUND, 0);
            }
        }
    }

    for (drawY = 0; drawY < COMBO_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
            if (bComboDraw[drawX][drawY] != 0) {
                DrawCell(
                    originX + drawX,
                    originY + drawY,
                    drawX,
                    drawY,
                    ADVMGR_DRAW_HERO_SHADOW,
                    0
                );
            }
        }
    }

    for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
        if (bComboDraw[drawX][0] != 0) {
            DrawCell(originX + drawX, originY, drawX, 0, ADVMGR_DRAW_OBJECT, 0);
        }
    }

    for (drawY = 1; drawY < COMBO_VIEW_CELLS; ++drawY) {
        PollSound();
        for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
            if (bComboDraw[drawX][drawY - 1] != 0) {
                DrawCell(
                    originX + drawX,
                    originY + drawY - 1,
                    drawX,
                    drawY - 1,
                    ADVMGR_DRAW_HERO,
                    0
                );
            }
        }
        for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
            if (bComboDraw[drawX][drawY - 1] != 0) {
                DrawCell(
                    originX + drawX,
                    originY + drawY - 1,
                    drawX,
                    drawY - 1,
                    ADVMGR_DRAW_OVERLAY,
                    0
                );
            }
        }
        for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
            if (giDeferObjDrawX == originX + drawX && giDeferObjDrawY == originY + drawY) {
                continue;
            }
            if (giDeferObjDrawX == originX + drawX && giDeferObjDrawY + 1 == originY + drawY) {
                DrawCell(
                    originX + drawX,
                    originY + drawY - 1,
                    drawX,
                    drawY - 1,
                    ADVMGR_DRAW_OBJECT,
                    0
                );
            }
            if (bComboDraw[drawX][drawY] != 0) {
                DrawCell(originX + drawX, originY + drawY, drawX, drawY, ADVMGR_DRAW_OBJECT, 0);
            }
        }
    }

    for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
        if (bComboDraw[drawX][COMBO_VIEW_CELLS - 1] != 0) {
            DrawCell(
                originX + drawX,
                originY + COMBO_VIEW_CELLS - 1,
                drawX,
                COMBO_VIEW_CELLS - 1,
                ADVMGR_DRAW_HERO,
                0
            );
        }
    }
    for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
        if (bComboDraw[drawX][COMBO_VIEW_CELLS - 1] != 0) {
            DrawCell(
                originX + drawX,
                originY + COMBO_VIEW_CELLS - 1,
                drawX,
                COMBO_VIEW_CELLS - 1,
                ADVMGR_DRAW_OVERLAY,
                0
            );
        }
    }

    for (drawY = 0; drawY < COMBO_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
            if (bComboDraw[drawX][drawY] != 0) {
                DrawCell(
                    originX + drawX,
                    originY + drawY,
                    drawX,
                    drawY,
                    ADVMGR_DRAW_OVERLAY_TOP,
                    0
                );
            }
        }
    }
    for (drawY = 0; drawY < COMBO_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
            if (bComboDraw[drawX][drawY] != 0) {
                DrawCell(originX + drawX, originY + drawY, drawX, drawY, ADVMGR_DRAW_CLOUD, 0);
            }
        }
    }

    DrawAdventureBorder();
    gpMouseManager->m_cursorReady = 1;
    PollSound();
    UpdBottomView(0, 1, 1);

    giLimitUpdMinX = COMBO_VIEW_CELLS;
    giLimitUpdMinY = COMBO_VIEW_CELLS;
    giLimitUpdMaxX = 0;
    giLimitUpdMaxY = 0;
    updateCount = 0;
    for (drawY = 0; drawY < COMBO_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < COMBO_VIEW_CELLS; ++drawX) {
            if (bComboDraw[drawX][drawY] != 0) {
                ++updateCount;
                if (drawX < giLimitUpdMinX) {
                    giLimitUpdMinX = drawX;
                }
                if (drawX > giLimitUpdMaxX) {
                    giLimitUpdMaxX = drawX;
                }
                if (drawY < giLimitUpdMinY) {
                    giLimitUpdMinY = drawY;
                }
                if (drawY > giLimitUpdMaxY) {
                    giLimitUpdMaxY = drawY;
                }
            }
        }
    }

    giLimitUpdMinX *= CELL_PIXELS;
    giLimitUpdMinY *= CELL_PIXELS;
    giLimitUpdMaxX = (giLimitUpdMaxX + 1) * CELL_PIXELS - 1;
    giLimitUpdMaxY = (giLimitUpdMaxY + 1) * CELL_PIXELS - 1;
    if (giLimitUpdMinX < COMBO_UPDATE_MIN) {
        giLimitUpdMinX = COMBO_UPDATE_MIN;
    }
    if (giLimitUpdMaxX > COMBO_UPDATE_MAX) {
        giLimitUpdMaxX = COMBO_UPDATE_MAX;
    }
    if (giLimitUpdMinY < COMBO_UPDATE_MIN) {
        giLimitUpdMinY = COMBO_UPDATE_MIN;
    }
    if (giLimitUpdMaxY > COMBO_UPDATE_MAX) {
        giLimitUpdMaxY = COMBO_UPDATE_MAX;
    }

    if (giLimitUpdMinX > giLimitUpdMaxX || giLimitUpdMinY > giLimitUpdMaxY) {
        giLimitUpdMinX = giLimitUpdMaxX - 1;
        giLimitUpdMinY = giLimitUpdMaxY - 1;
        return 0;
    }
    return 1;
}

i32 advManager::ComboDraw(i32 update) {
    return ComboDraw(m_mapOriginX, m_mapOriginY, update);
}

void advManager::SetEnvironmentOrigin(i32 originX, i32 originY, i32 stopSounds) {
    i32 soundRadius;
    i32 edgeOffset;
    i32 maxCells = SOUND_CELL_COUNT;
    i32 layer;

    if (gSoundBackendsReady == 0) {
        return;
    }

    for (edgeOffset = 0; edgeOffset < SOUND_CELL_COUNT; ++edgeOffset) {
        if (m_activeSounds[edgeOffset].soundId != ADVMGR_ENVIRONMENT_SOUND_NONE) {
            if (stopSounds != 0) {
                gpSoundManager->StopSample(
                    m_loopingSamples[H2EnumIndex(m_activeSounds[edgeOffset].soundId)]
                );
                m_activeSounds[edgeOffset].soundId = ADVMGR_ENVIRONMENT_SOUND_NONE;
                m_activeSounds[edgeOffset].volume = ENVIRONMENT_SOUND_DEFAULT_VOLUME;
            } else {
                m_activeSounds[edgeOffset].volume = ENVIRONMENT_SOUND_DEFAULT_VOLUME;
            }
        }
    }

    if (originX == ENVIRONMENT_ORIGIN_NONE) {
        return;
    }

    if (gConfig.soundVolume != CONFIG_VOLUME_MUTED) {
        m_activeSoundMask = 0;
        for (layer = ENVIRONMENT_SOUND_FIRST_LAYER;
             layer <= ENVIRONMENT_SOUND_LAYER_COUNT;
             ++layer) {
            InsertSound(originX, originY, 0, layer);
            for (soundRadius = 0; soundRadius < SOUND_CELL_COUNT; ++soundRadius) {
                for (edgeOffset = 0; edgeOffset < soundRadius * ENVIRONMENT_SOUND_EDGE_SPAN;
                     ++edgeOffset) {
                    InsertSound(
                        originX - soundRadius + edgeOffset,
                        originY - soundRadius,
                        soundRadius,
                        layer
                    );
                    InsertSound(
                        originX + soundRadius,
                        originY - soundRadius + edgeOffset,
                        soundRadius,
                        layer
                    );
                    InsertSound(
                        originX + soundRadius - edgeOffset,
                        originY + soundRadius,
                        soundRadius,
                        layer
                    );
                    InsertSound(
                        originX - soundRadius,
                        originY + soundRadius - edgeOffset,
                        soundRadius,
                        layer
                    );
                }
            }
        }

        for (edgeOffset = 0; edgeOffset < SOUND_CELL_COUNT; ++edgeOffset) {
            if (m_activeSounds[edgeOffset].soundId != ADVMGR_ENVIRONMENT_SOUND_NONE
                && m_activeSounds[edgeOffset].volume > ENVIRONMENT_SOUND_MAX_DISTANCE) {
                gpSoundManager->StopSample(
                    m_loopingSamples[H2EnumIndex(m_activeSounds[edgeOffset].soundId)]
                );
                m_activeSounds[edgeOffset].soundId = ADVMGR_ENVIRONMENT_SOUND_NONE;
            }
            if (m_activeSounds[edgeOffset].soundId != ADVMGR_ENVIRONMENT_SOUND_NONE
                && (m_activeSoundMask & (1 << H2EnumIndex(m_activeSounds[edgeOffset].soundId))) != 0) {
                gpSoundManager->ModifySample(
                    m_loopingSamples[H2EnumIndex(m_activeSounds[edgeOffset].soundId)],
                    ADVMGR_ENVIRONMENT_VOLUME(m_activeSounds[edgeOffset].volume)
                );
            }
        }
    }
}

void advManager::CheckLoadSample(i32 index) {
    if (m_loopingSamples[index] == NULL) {
        TrimLoopingSounds(LOOPING_SOUND_LIMIT);
        sprintf(
            gText,
            "loop%04d.82M",
            index
        );
        m_loopingSamples[index] = gpResourceManager->GetSample(gText);
    }
}

AdventureEnvironmentSoundId advManager::GetSoundId(i32 x, i32 y) {
    mapCell* cell = m_mapData->GetCell(x, y);
    AdventureEnvironmentSoundId soundId = ADVMGR_ENVIRONMENT_SOUND_NONE;

    if (giGroundToTerrain[cell->m_terrainImageIndex] == TERRAIN_WATER
        && (giGroundShape[cell->m_terrainImageIndex] & SOUND_GROUND_SHAPE_MASK)) {
        return ADVMGR_SOUND_COASTLINE;
    }

    if ((H2EnumIndex((cell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
        switch (cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
            case MAP_OBJECT_ARCHER_HOUSE:
                return ADVMGR_SOUND_DWELLING;
            case MAP_OBJECT_DWARF_COTTAGE:
                return ADVMGR_SOUND_DWELLING;
            case MAP_OBJECT_PEASANT_HUT:
                return ADVMGR_SOUND_DWELLING;
            case MAP_OBJECT_LOG_CABIN:
                return ADVMGR_SOUND_DWELLING;
            case MAP_OBJECT_SIRENS:
                return ADVMGR_SOUND_DWELLING;
            case MAP_OBJECT_RUINS:
                return ADVMGR_SOUND_RUINS;
            case MAP_OBJECT_DERELICT_SHIP:
                return ADVMGR_SOUND_DERELICT_SHIP;
            case MAP_OBJECT_TRADING_POST:
                return ADVMGR_SOUND_TRADING_POST;
            case MAP_OBJECT_SHRINE_FIRST_CIRCLE:
                return ADVMGR_SOUND_SHRINE;
            case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
                return ADVMGR_SOUND_SHRINE;
            case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
                return ADVMGR_SOUND_SHRINE;
            case MAP_OBJECT_BUOY:
                return ADVMGR_SOUND_BUOY;
            case MAP_OBJECT_SHIPWRECK:
                return ADVMGR_SOUND_SHIPWRECK;
            case MAP_OBJECT_WHIRLPOOL:
                return ADVMGR_SOUND_COAST;
            case MAP_OBJECT_CAMPFIRE:
                return ADVMGR_SOUND_CAMPFIRE;
            case MAP_OBJECT_WINDMILL:
                return ADVMGR_SOUND_WINDMILL;
            case MAP_OBJECT_FOUNTAIN:
                return ADVMGR_SOUND_FOUNTAIN;
            case MAP_OBJECT_ARTESIAN_SPRING:
                return ADVMGR_SOUND_FOUNTAIN;
            case MAP_OBJECT_WATERING_HOLE:
                return ADVMGR_SOUND_WATERING_HOLE;
            case MAP_OBJECT_STONE_LITHS:
                return ADVMGR_SOUND_STONE_LITHS;
            case MAP_OBJECT_ORACLE:
                return ADVMGR_SOUND_ORACLE;
            case MAP_OBJECT_WATER_WHEEL:
                return ADVMGR_SOUND_WATER_WHEEL;
            case MAP_OBJECT_ALCHEMIST_LAB:
                if ((H2EnumIndex((cell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                    return ADVMGR_SOUND_ALCHEMIST_LAB_ACTION;
                }
                break;
            case MAP_OBJECT_MINE:
                if ((H2EnumIndex((cell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                    return ADVMGR_SOUND_MINE;
                }
                break;
            case MAP_OBJECT_ABANDONED_MINE:
                if ((H2EnumIndex((cell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                    return ADVMGR_SOUND_ABANDONED_MINE;
                }
                break;
            case MAP_OBJECT_SAWMILL:
                if ((H2EnumIndex((cell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                    return ADVMGR_SOUND_SAWMILL;
                }
                break;
            case MAP_OBJECT_DAEMON_CAVE:
                if ((H2EnumIndex((cell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                    return ADVMGR_SOUND_DAEMON_CAVE;
                }
                break;
        }
    } else {
        switch (cell->m_triggerType) {
            case MAP_OBJECT_TAR_PIT:
                return ADVMGR_SOUND_TAR_PIT;
            case MAP_OBJECT_LAVA_POOL:
                if (cell->m_objectIndex >= SOUND_ALCHEMIST_FRAME_FIRST
                    && cell->m_objectIndex <= SOUND_ALCHEMIST_FRAME_LAST) {
                    return ADVMGR_SOUND_ALCHEMIST_LAB;
                } else {
                    return ADVMGR_SOUND_LAVA_POOL;
                }
            case MAP_OBJECT_VOLCANO:
                if (cell->m_objectTileset == TILESET_OBJNLAV3
                    || cell->m_objectTileset == TILESET_OBJNLAV2) {
                    return ADVMGR_SOUND_LARGE_VOLCANO;
                } else {
                    return ADVMGR_SOUND_SMALL_VOLCANO;
                }
            case MAP_OBJECT_WATER_LAKE:
                if (cell->m_objectTileset == TILESET_OBJNSNOW) {
                    break;
                }
                return ADVMGR_SOUND_WATERING_HOLE;
        }

        switch (cell->m_objectTileset) {
            case TILESET_STREAM:
                return ADVMGR_SOUND_STREAM;
            case TILESET_OBJNWAT2:
                if (cell->m_objectIndex == SOUND_SEAGULL_FRAME_FIRST
                    || cell->m_objectIndex == SOUND_SEAGULL_FRAME_LAST) {
                    return ADVMGR_SOUND_SEAGULLS;
                }
                break;
        }
    }

    return ADVMGR_ENVIRONMENT_SOUND_NONE;
}

void advManager::InsertSound(i32 x, i32 mapY, i32 distance, i32 soundLayer) {
    i32 soundSlot;
    i32 distanceLimit;
    i32 activeIndex;
    AdventureEnvironmentSoundId soundId;

    if (x < 0 || mapY < 0 || x >= MAP_WIDTH || mapY >= MAP_HEIGHT) {
        return;
    }

    soundId = GetSoundId(x, mapY);
    if (soundId == ADVMGR_ENVIRONMENT_SOUND_NONE) {
        return;
    }

    for (activeIndex = 0; activeIndex < SOUND_CELL_COUNT; ++activeIndex) {
        if (m_activeSounds[activeIndex].soundId == soundId) {
            if (m_activeSounds[activeIndex].volume > distance) {
                m_activeSounds[activeIndex].volume = distance;
                m_activeSoundMask |= (1 << H2EnumIndex(m_activeSounds[activeIndex].soundId));
            }
            return;
        }
    }

    if (soundLayer == ENVIRONMENT_SOUND_FIRST_LAYER) {
        return;
    }

    distanceLimit = distance;
    soundSlot = -1;
    for (activeIndex = 0; activeIndex < SOUND_CELL_COUNT; ++activeIndex) {
        if (m_activeSounds[activeIndex].volume > distanceLimit) {
            distanceLimit = m_activeSounds[activeIndex].volume;
            soundSlot = activeIndex;
        }
    }

    if (soundSlot != -1) {
        if (m_activeSounds[soundSlot].soundId != ADVMGR_ENVIRONMENT_SOUND_NONE) {
            gpSoundManager->StopSample(m_loopingSamples[H2EnumIndex(m_activeSounds[soundSlot].soundId)]);
        }
        m_activeSounds[soundSlot].soundId = soundId;
        m_activeSounds[soundSlot].volume = distance;
        CheckLoadSample(H2EnumIndex(soundId));
        m_loopingSamples[H2EnumIndex(soundId)]->m_playbackData.volume = ADVMGR_ENVIRONMENT_VOLUME(distance);
        m_loopingSamples[H2EnumIndex(soundId)]->m_playbackData.loopCount = 1;
        m_loopingSamples[H2EnumIndex(soundId)]->m_playbackData.channelType = ENVIRONMENT_SOUND_CHANNEL_TYPE;
        gpSoundManager->MemorySample(m_loopingSamples[H2EnumIndex(soundId)]);
        m_activeSoundMask ^= 1 << H2EnumIndex(m_activeSounds[soundSlot].soundId);
    }
}

void advManager::TeleportTo(
    hero* mapHero,
    i32 destinationX,
    i32 destinationY,
    i32,
    i32 skipMapChange
) {
    i32 savedShow;
    H2EnumStorage<TerrainType, i32> terrain;
    mapCell* cellOld2;
    i32 oldCellFlag26;
    i32 unused;
    mapCell* destinationCell29;
    i32 fizzleTime36;
    town* occupiedTown47;

    savedShow = bShowIt;
    if (skipMapChange == 0) {
        SendMapChange(
            MAP_CHANGE_TELEPORT_HERO,
            mapHero->m_id,
            static_cast<u8>(destinationX),
            static_cast<u8>(destinationY),
            TELEPORT_MAP_CHANGE_VALUE,
            0,
            0
        );
    }

    destinationCell29 = GetCell(destinationX, destinationY);
    cellOld2 = GetCell(mapHero->m_x, mapHero->m_y);
    if (mapHero->m_locationType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
        occupiedTown47 = gpGame->GetTown(mapHero->m_occupiedTown);
        occupiedTown47->m_occupyingHeroId = INVALID_HERO;
    }

    oldCellFlag26 = 0;
    if (cellOld2->m_flags & TELEPORT_CELL_OBJECT_FLAG) {
        cellOld2->m_flags -= TELEPORT_CELL_OBJECT_FLAG;
        oldCellFlag26 = 1;
    } else {
        gpGame->RestoreCell(
            mapHero->m_x,
            mapHero->m_y,
            mapHero->m_locationType,
            mapHero->m_occupiedTown,
            NULL,
            TELEPORT_RESTORE_MODE
        );
    }

    CompleteDraw(0);
    if (gbThisNetHumanPlayer[giCurPlayer] == 0) {
        if ((gConfig.blackoutComputer == 0
             && MapExtraPosAndAdjacentsSet(mapHero->m_x, mapHero->m_y, giCurWatchPlayerBit))
            || MapExtraPosAndAdjacentsSet(destinationX, destinationY, giCurWatchPlayerBit)) {
            bShowIt = 1;
        } else {
            bShowIt = 0;
        }
    }

    if (savedShow != 0) {
        HideRoute(1, 1, 1);
    }

    if (bShowIt != 0) {
        m_mapOriginX = destinationX - TELEPORT_VIEW_CENTER;
        m_mapOriginY = destinationY - TELEPORT_VIEW_CENTER;
        DelayMilli(TELEPORT_DELAY);
    }

    mapHero->m_x = destinationX;
    mapHero->m_y = destinationY;
    gpGame->SetVisibility(
        m_mapOriginX + TELEPORT_VIEW_CENTER,
        m_mapOriginY + TELEPORT_VIEW_CENTER,
        giCurPlayer,
        giVisRange[H2EnumIndex(mapHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_SCOUTING)])]
            + (mapHero->HasArtifact(ARTIFACT_TELESCOPE) != 0)
    );

    if (bShowIt != 0) {
        destinationCell29->m_flags |= TELEPORT_CELL_OBJECT_FLAG;
        gpWindowManager->SaveFizzleSource(
            UPDATE_VIEWPORT_ORIGIN,
            UPDATE_VIEWPORT_ORIGIN,
            UPDATE_VIEWPORT_SIZE,
            UPDATE_VIEWPORT_SIZE
        );
        CompleteDraw(0);
        PollSound();
        fizzleTime36 = TELEPORT_FIZZLE_TIME;
        if (gbThisNetHumanPlayer[giCurPlayer] == 0) {
            fizzleTime36 -= TELEPORT_REMOTE_FIZZLE_ADJUSTMENT;
        }
        gpWindowManager->FizzleForward(
            UPDATE_VIEWPORT_ORIGIN,
            UPDATE_VIEWPORT_ORIGIN,
            UPDATE_VIEWPORT_SIZE,
            UPDATE_VIEWPORT_SIZE,
            -1,
            NULL,
            NULL
        );
        PollSound();
    } else {
        mapHero->m_locationType = destinationCell29->m_triggerType;
        mapHero->m_occupiedTown = destinationCell29->m_objectMetadata;
        if (oldCellFlag26 != 0) {
            destinationCell29->m_flags |= TELEPORT_CELL_OBJECT_FLAG;
        } else {
            destinationCell29->m_triggerType =
                (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION);
            destinationCell29->m_objectMetadata = mapHero->m_id;
        }
        if (m_cursorType == HERO_TYPE_BOAT) {
            mapHero->m_eventFlags =
                HeroEventFlag(static_cast<i32>(mapHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_EMBARKED));
        }
        m_cursorActive = 0;
    }

    SetEnvironmentOrigin(
        m_mapOriginX + TELEPORT_VIEW_CENTER,
        m_mapOriginY + TELEPORT_VIEW_CENTER,
        1
    );
    terrain = giGroundToTerrain[destinationCell29->m_terrainImageIndex];
    if (terrain != m_currentTerrain) {
        m_currentTerrain = terrain;
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]);
    }
    Reseed(0, 0);
    UpdateRadar(1, 0);
    CompleteDraw(0);
    ForceNewHover();
}

void advManager::DimensionDoor(void) {
    hero* targetHero;
    heroWindow* window;
    i32 newX;
    i32 newY;
    mapCell* targetCell;

    window = new heroWindow(
        0,
        0,
        "dimdoor.bin"
    );
    if (window == NULL) {
        MemError();
    }
    gpWindowManager->DoDialog(window, DimensionDoorHandler, 0);
    delete window;

    targetHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    if (gpWindowManager->m_dialogResult == TRAVEL_DIALOG_ACCEPT) {
        newX = m_mapOriginX + m_lastHoverCell;
        newY = m_mapOriginY + m_hoverCellY;
        targetCell = GetCell(newX, newY);
        if (((H2EnumIndex((targetHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))
             && giGroundToTerrain[targetCell->m_terrainImageIndex] != TERRAIN_WATER)
            || (!(H2EnumIndex((targetHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))
                && giGroundToTerrain[targetCell->m_terrainImageIndex] == TERRAIN_WATER)) {
            NormalDialog(
                "\xcd\xe5 \xf3\xe4\xe0\xeb\xee\xf1\xfc \xee\xf2\xea\xf0\xfb\xf2\xfc \xcf\xf0\xee\xf1\xf2\xf0\xe0\xed\xf1\xf2\xe2\xe5\xed\xed\xfb\xe5 \xe2\xf0\xe0\xf2\xe0!!!",
                OPTION_DIALOG_MESSAGE,
                OPTION_DIALOG_NONE,
                OPTION_DIALOG_NONE,
                OPTION_DIALOG_NONE,
                0,
                OPTION_DIALOG_NONE,
                0,
                OPTION_DIALOG_NONE,
                0
            );
            UpdateRadar(1, 0);
        } else {
            gpSoundManager->SwitchAmbientMusic(TRAVEL_MUSIC);
            TeleportTo(targetHero, newX, newY, 0, 0);
            gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]);
        }
        gpGame->GetHero(gpCurPlayer->m_currentHero)->UseSpell(SPELL_DIMENSION_DOOR);
    } else {
        UpdateRadar(1, 0);
    }
}

MessageDispatchResult TownPortalHandler(tag_message& message) {
    tag_message choiceMessage;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == 1) {
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[H2EnumIndex(gpAdvManager->m_currentTerrain)]
        );
    }

    if (message.type == ADVMGR_TOWN_PORTAL_MESSAGE) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case TOWN_PORTAL_FIRST_CHOICE:
                    case TOWN_PORTAL_LAST_CHOICE:
                        choiceMessage.type = ADVMGR_TOWN_PORTAL_MESSAGE;
                        choiceMessage.payload.widget.id = TOWN_PORTAL_CHOICE_WIDGET;
                        choiceMessage.payload.widget.command = ADVMGR_TOWN_PORTAL_COMMAND_CHOICE;
                        townPortalWin->BroadcastMessage(choiceMessage);
                        giTownPortalChoice = choiceMessage.payload.widget.data.value;
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        message.payload.widget.id = TOWN_PORTAL_CLOSE_WIDGET;
                        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
                        return MESSAGE_DISPATCH_FORWARD;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    return MESSAGE_DISPATCH_CONSUME;
}

void advManager::TownGate(SpellType spellId) {
    hero* targetHero;
    i32 dist;
    i32 selectedTown;
    tag_message message;
    i32 i;
    i32 nearestDistance;

    nearestDistance = TOWN_PORTAL_DISTANCE_LIMIT;
    selectedTown = INVALID_HERO;
    targetHero = gpGame->GetHero(gpCurPlayer->m_currentHero);

    if (gpCurPlayer->m_townCount == 0) {
        NormalDialog(
            "\xcd\xe5\xf2 \xe4\xee\xf1\xf2\xf3\xef\xed\xee\xe3\xee \xe3\xee\xf0\xee\xe4\xe0.  \xc7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5 \xef\xf0\xe5\xf0\xe2\xe0\xed\xee!",
            OPTION_DIALOG_MESSAGE,
            OPTION_DIALOG_NONE,
            OPTION_DIALOG_NONE,
            OPTION_DIALOG_NONE,
            0,
            OPTION_DIALOG_NONE,
            0,
            OPTION_DIALOG_NONE,
            0
        );
        return;
    }
    if ((H2EnumIndex((targetHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))) {
        NormalDialog(
            "\xc7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5 \xef\xf0\xe5\xf0\xe2\xe0\xed\xee! \xc2\xfb \xe4\xee\xeb\xe6\xed\xfb \xe1\xfb\xf2\xfc \xed\xe0 \xf1\xf3\xf8\xe5, \xf7\xf2\xee\xe1\xfb \xfd\xf2\xee \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5 \xf1\xf0\xe0\xe1\xee\xf2\xe0\xeb\xee.",
            OPTION_DIALOG_MESSAGE,
            OPTION_DIALOG_NONE,
            OPTION_DIALOG_NONE,
            OPTION_DIALOG_NONE,
            0,
            OPTION_DIALOG_NONE,
            0,
            OPTION_DIALOG_NONE,
            0
        );
        return;
    }

    if (spellId == SPELL_TOWN_PORTAL) {
        townPortalWin = new heroWindow(
            TOWN_PORTAL_WINDOW_X,
            TOWN_PORTAL_WINDOW_Y,
            "townport.bin"
        );
        sprintf(
            gText,
            "{\xcf\xee\xf0\xf2\xe0\xeb \xe3\xee\xf0\xee\xe4\xe0}\x0a\x0a\xc2\xfb\xe1\xe5\xf0\xe8\xf2\xe5 \xe3\xee\xf0\xee\xe4, \xea\xf3\xe4\xe0 \xf5\xee\xf2\xe8\xf2\xe5 \xef\xe5\xf0\xe5\xed\xe5\xf1\xf2\xe8\xf1\xfc:"
        );
        message.type = ADVMGR_TOWN_PORTAL_MESSAGE;
        message.payload.widget.command = ADVMGR_TOWN_PORTAL_COMMAND_TEXT;
        message.payload.widget.id = TOWN_PORTAL_TITLE_WIDGET;
        message.payload.widget.data.text = gText;
        townPortalWin->BroadcastMessage(message);

        for (i = 0; i < gpCurPlayer->m_townCount; ++i) {
            sprintf(gText, gpGame->m_castleRecs[gpCurPlayer->m_townIds[i]].m_name);
            message.type = ADVMGR_TOWN_PORTAL_MESSAGE;
            message.payload.widget.command = ADVMGR_TOWN_PORTAL_COMMAND_ADD_TOWN;
            message.payload.widget.id = TOWN_PORTAL_CHOICE_WIDGET;
            message.payload.widget.data.text = gText;
            townPortalWin->BroadcastMessage(message);
        }
        message.payload.widget.command = ADVMGR_TOWN_PORTAL_COMMAND_FINISH;
        message.payload.widget.data.text = NULL;
        townPortalWin->BroadcastMessage(message);
        gpWindowManager->DoDialog(townPortalWin, TownPortalHandler, 0);
        selectedTown = giTownPortalChoice;
        delete townPortalWin;
        if (gpWindowManager->m_dialogResult == TOWN_PORTAL_FIRST_CHOICE) {
            return;
        }
    } else {
        for (i = 0; i < gpCurPlayer->m_townCount; ++i) {
            dist = abs(

                            gpGame->m_castleRecs[gpCurPlayer->m_townIds[i]].m_x
                            - targetHero->m_x
                        )
                        + abs(
                            gpGame->m_castleRecs[gpCurPlayer->m_townIds[i]].m_y
                            - targetHero->m_y
                        );
            if (dist < nearestDistance) {
                nearestDistance = dist;
                selectedTown = i;
            }
        }
    }

    if (gpGame->m_castleRecs[gpCurPlayer->m_townIds[selectedTown]].m_occupyingHeroId
        != INVALID_HERO) {
        NormalDialog(
            "\xc1\xeb\xe8\xe6\xe0\xe9\xf8\xe8\xe9 \xe3\xee\xf0\xee\xe4 \xee\xea\xea\xf3\xef\xe8\xf0\xee\xe2\xe0\xed. \xc7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5 \xef\xf0\xe5\xf0\xe2\xe0\xed\xee!",
            OPTION_DIALOG_MESSAGE,
            OPTION_DIALOG_NONE,
            OPTION_DIALOG_NONE,
            OPTION_DIALOG_NONE,
            0,
            OPTION_DIALOG_NONE,
            0,
            OPTION_DIALOG_NONE,
            0
        );
        return;
    }

    gpSoundManager->SwitchAmbientMusic(TRAVEL_MUSIC);
    TeleportTo(
        targetHero,
        gpGame->m_castleRecs[gpCurPlayer->m_townIds[selectedTown]].m_x,
        gpGame->m_castleRecs[gpCurPlayer->m_townIds[selectedTown]].m_y,
        0,
        0
    );
    targetHero->UseSpell(spellId);
    gpGame->m_castleRecs[gpCurPlayer->m_townIds[selectedTown]].m_occupyingHeroId =
        targetHero->m_id;
    gpGame->m_castleRecs[gpCurPlayer->m_townIds[selectedTown]].GiveSpells(NULL);
    targetHero->m_locationType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE;
    targetHero->m_occupiedTown = gpCurPlayer->m_townIds[selectedTown];
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]);
}

void advManager::SummonBoat(void) {
    i32 placeX;
    hero* summonHero;
    mapCell* destinationCell;
    i32 okCell;
    i32 slotIndex;
    i32 iDir;
    i32 placeY;
    i32 foundBoat;
    i32 heroSlot;
    boatRecord* boatRec;
    mapCell* fromCell;
    i32 clipWidth;
    i32 clipX;
    i32 clipY;
    i32 clipHeight;

    summonHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    okCell = 0;
    foundBoat = 0;
    destinationCell =
        GetCell(m_mapOriginX + SUMMON_CENTER_OFFSET, m_mapOriginY + SUMMON_CENTER_OFFSET);
    if (giGroundToTerrain[destinationCell->m_terrainImageIndex] == TERRAIN_WATER) {
        goto summon_done;
    }

    for (iDir = 0; iDir < SUMMON_DIRECTION_COUNT; ++iDir) {
        placeX = m_mapOriginX + SUMMON_CENTER_OFFSET + normalDirTable[iDir].x;
        placeY = m_mapOriginY + SUMMON_CENTER_OFFSET + normalDirTable[iDir].y;
        if (placeX < 0 || placeX >= MAP_WIDTH || placeY < 0
            || placeY >= MAP_HEIGHT) {
            continue;
        }

        destinationCell = GetCell(placeX, placeY);
        if (destinationCell->m_objectIndex == MAPCELL_SPRITE_NONE
            && destinationCell->m_triggerType == MAP_OBJECT_NONE
            && giGroundToTerrain[destinationCell->m_terrainImageIndex] == TERRAIN_WATER) {
            okCell = 1;
            break;
        }
    }

    if (okCell) {
        heroSlot = gpCurPlayer->m_currentHero;
        for (slotIndex = 0; slotIndex < SUMMON_BOAT_COUNT; ++slotIndex) {
            if (gpGame->m_boatSlots[slotIndex] != -1
                && gpGame->m_boats[slotIndex].heroId
                       == (heroSlot | SUMMON_OCCUPIED_FLAG)) {
                foundBoat = 1;
                break;
            }
        }

        if (!foundBoat) {
            for (slotIndex = 0; slotIndex < SUMMON_BOAT_COUNT; ++slotIndex) {
                if (gpGame->m_boatSlots[slotIndex] != -1
                    && (gpGame->m_boats[slotIndex].heroId & SUMMON_OCCUPIED_FLAG)
                    && gpGame->m_boats[slotIndex].owner == giCurPlayer
                    && abs(gpGame->m_boats[slotIndex].x - summonHero->m_x)
                               + abs(gpGame->m_boats[slotIndex].y - summonHero->m_y)
                           > SUMMON_MIN_DISTANCE) {
                    foundBoat = 1;
                    break;
                }
            }
        }

        if (foundBoat) {
            boatRec = &gpGame->m_boats[slotIndex];
            fromCell = GetCell(boatRec->x, boatRec->y);
            gpGame->RestoreCell(
                boatRec->x,
                boatRec->y,
                boatRec->savedTriggerType,
                boatRec->savedEventData,
                NULL,
                SUMMON_RESTORE_MODE
            );

            if (boatRec->x >= m_mapOriginX && boatRec->x < m_mapOriginX + HOVER_VIEW_CELLS
                && boatRec->y >= m_mapOriginY && boatRec->y < m_mapOriginY + HOVER_VIEW_CELLS) {
                clipX = (boatRec->x - m_mapOriginX) * CELL_PIXELS - SUMMON_FIZZLE_X_OFFSET;
                if (clipX < SUMMON_SCREEN_MARGIN) {
                    clipX = SUMMON_SCREEN_MARGIN;
                }
                clipY = (boatRec->y - m_mapOriginY) * CELL_PIXELS - SUMMON_FIZZLE_X_OFFSET;
                if (clipY < SUMMON_SCREEN_MARGIN) {
                    clipY = SUMMON_SCREEN_MARGIN;
                }

                clipWidth = SUMMON_FIZZLE_WIDTH;
                clipHeight = SUMMON_FIZZLE_HEIGHT;
                if (clipX + clipWidth >= SUMMON_SCREEN_LIMIT) {
                    clipWidth = SUMMON_SCREEN_LIMIT - clipX;
                }
                if (clipY + clipHeight >= SUMMON_SCREEN_LIMIT) {
                    clipHeight = SUMMON_SCREEN_LIMIT - clipY;
                }
                gpWindowManager
                    ->SaveFizzleSource(clipX, clipY, clipWidth, clipHeight);
                CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
                gpWindowManager->FizzleForward(
                    clipX,
                    clipY,
                    clipWidth,
                    clipHeight,
                    -1,
                    NULL,
                    NULL
                );
            }

            boatRec->x = static_cast<i8>(
                m_mapOriginX + SUMMON_CENTER_OFFSET + normalDirTable[iDir].x
            );
            boatRec->y = static_cast<i8>(
                m_mapOriginY + SUMMON_CENTER_OFFSET + normalDirTable[iDir].y
            );
            boatRec->savedTriggerType = destinationCell->m_triggerType;
            boatRec->savedEventData = static_cast<u8>(destinationCell->m_objectMetadata);
            destinationCell->m_triggerType = SUMMON_BOAT_TRIGGER;
            destinationCell->m_objectMetadata = slotIndex;

            gpWindowManager->SaveFizzleSource(
                SUMMON_TARGET_X,
                SUMMON_TARGET_Y,
                SUMMON_TARGET_WIDTH,
                SUMMON_TARGET_HEIGHT
            );
            CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
            gpWindowManager->FizzleForward(
                SUMMON_TARGET_X,
                SUMMON_TARGET_Y,
                SUMMON_TARGET_WIDTH,
                SUMMON_TARGET_HEIGHT,
                -1,
                NULL,
                NULL
            );
        }
    }

summon_done:
    UpdateScreen(0, 0);
    Reseed(0, 0);
    if (!foundBoat) {
        NormalDialog(
            "\xcd\xe5 \xf3\xe4\xe0\xeb\xee\xf1\xfc \xef\xf0\xe8\xe7\xe2\xe0\xf2\xfc \xea\xee"
                "\xf0\xe0\xe1\xeb\xfc!!!"
                 ,
            OPTION_DIALOG_MESSAGE,
            OPTION_DIALOG_NONE,
            OPTION_DIALOG_NONE,
            OPTION_DIALOG_NONE,
            0,
            OPTION_DIALOG_NONE,
            0,
            OPTION_DIALOG_NONE,
            0
        );
    }
}

void advManager::ShowRoute(i32 redraw, i32, i32 updateButton) {
    hero* hero;
    i32 nPath;
    i32 reachable;
    mapCell* nextTile;
    i32 frame;
    i32 cost;
    i32 fromDir;
    i32 remain;
    i32 mapX;
    i32 index;
    i32 mapY;
    mapCell* thisTile;
    i32 dir;
    H2EnumStorage<TerrainType, i32> terr;
    BaseWidgetCommand buttonFrame;

    reachable = 0;
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        return;
    }

    if (gpCurPlayer->m_currentHero == INVALID_HERO) {
        HideRoute(redraw, 0, 1);
        return;
    }

    hero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    if (hero->m_destinationX == HERO_DESTINATION_NONE) {
        HideRoute(redraw, 1, 1);
        return;
    }

    nPath = gpSearchArray->BuildPath(
        hero->m_x,
        hero->m_y,
        hero->m_destinationX,
        hero->m_destinationY,
        ROUTE_PATH_COST_LIMIT
    );
    if (gpSearchArray->m_pathLength > 0 && nPath > 0) {
        memset(m_visibilityMap, 0, MAP_WIDTH * MAP_HEIGHT * sizeof(*m_visibilityMap));
        m_visibilityMapValid = 1;
        remain = hero->m_remainingMobility;
        mapX = hero->m_x;
        mapY = hero->m_y;

        for (index = gpSearchArray->m_pathLength - 1; index >= 0; --index) {
            dir = static_cast<u8>(gpSearchArray->m_storage.path.directions[index + 1]);
            thisTile = GetCell(mapX, mapY);
            mapX += normalDirTable[dir].x;
            mapY += normalDirTable[dir].y;
            nextTile = GetCell(mapX, mapY);
            terr = giGroundToTerrain[thisTile->m_terrainImageIndex];
            cost = CalcTerrainCost(
                giGroundToTerrain[nextTile->m_terrainImageIndex],
                dir & 1,
                ROUTE_TERRAIN_COST_INFINITY,
                H2EnumIndex(hero->m_secondarySkills[H2EnumIndex(HERO_SKILL_PATHFINDING)]),
                thisTile->m_isRoad,
                nextTile->m_isRoad
            );
            remain -= CalcTerrainCost(
                terr,
                dir & 1,
                remain,
                H2EnumIndex(hero->m_secondarySkills[H2EnumIndex(HERO_SKILL_PATHFINDING)]),
                thisTile->m_isRoad,
                nextTile->m_isRoad
            );

            if (dir & 1) {
                if (cost == ROUTE_DIAGONAL_COST_0) {
                    frame = ROUTE_FRAME_COST_0;
                } else if (cost == ROUTE_DIAGONAL_COST_1) {
                    frame = ROUTE_FRAME_COST_1;
                } else if (cost == ROUTE_DIAGONAL_COST_2) {
                    frame = ROUTE_FRAME_COST_2;
                } else if (cost == ROUTE_DIAGONAL_COST_3) {
                    frame = ROUTE_FRAME_COST_3;
                } else if (cost == ROUTE_DIAGONAL_COST_4) {
                    frame = ROUTE_FRAME_COST_4;
                } else if (cost == ROUTE_DIAGONAL_COST_5) {
                    frame = ROUTE_FRAME_COST_5;
                } else {
                    frame = ROUTE_FRAME_COST_1;
                }
            } else {
                if (cost == ROUTE_STRAIGHT_COST_0) {
                    frame = ROUTE_FRAME_COST_0;
                } else if (cost == ROUTE_STRAIGHT_COST_1) {
                    frame = ROUTE_FRAME_COST_1;
                } else if (cost == ROUTE_STRAIGHT_COST_2) {
                    frame = ROUTE_FRAME_COST_2;
                } else if (cost == ROUTE_STRAIGHT_COST_3) {
                    frame = ROUTE_FRAME_COST_3;
                } else if (cost == ROUTE_STRAIGHT_COST_4) {
                    frame = ROUTE_FRAME_COST_4;
                } else if (cost == ROUTE_STRAIGHT_COST_5) {
                    frame = ROUTE_FRAME_COST_5;
                } else {
                    frame = ROUTE_FRAME_COST_1;
                }
            }

            if (index == 0) {
                m_visibilityMap[mapX + mapY * MAP_WIDTH] = 1;
            } else {
                fromDir =
                    static_cast<u8>(gpSearchArray->m_storage.path.directions[index]);
                m_visibilityMap[mapX + mapY * MAP_WIDTH] = static_cast<u16>(
                    frame * ROUTE_ARROW_FRAME_STRIDE + gbArrow[fromDir][dir]
                    + ROUTE_ARROW_FRAME_OFFSET
                );
            }

            if (remain < 0) {
                m_visibilityMap[mapX + mapY * MAP_WIDTH] += ROUTE_DAY_MASK;
            } else {
                reachable = 1;
            }
        }

        if (updateButton) {
            buttonFrame = reachable ? ADVMGR_BUTTON_DISABLE : ADVMGR_BUTTON_ENABLE;
            gpWindowManager->BroadcastMessage(
                ADVMGR_BUTTON_MESSAGE,
                buttonFrame,
                BUTTON_TARGET,
                BUTTON_BROADCAST_FLAGS
            );
        }
    } else {
        HideRoute(redraw, 1, 1);
    }

    if (redraw) {
        CompleteDraw(0);
        UpdateScreen(0, 0);
    }
}

void advManager::HideRoute(i32 redraw, i32 clearDestination, i32 updateButton) {
    hero* currentHero;

    if (!gbThisNetHumanPlayer[giCurPlayer] && (!giDebugLevel || !giShowComputerRoute)) {
        return;
    }

    if (updateButton) {
        gpWindowManager->BroadcastMessage(
            ADVMGR_BUTTON_MESSAGE,
            ADVMGR_BUTTON_ENABLE,
            BUTTON_TARGET,
            BUTTON_BROADCAST_FLAGS
        );
    }

    if (clearDestination && gpCurPlayer->m_currentHero != INVALID_HERO) {
        currentHero = &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
        currentHero->m_destinationX = HERO_DESTINATION_NONE;
        currentHero->m_destinationY = HERO_DESTINATION_NONE;
    }

    if (!m_visibilityMapValid) {
        return;
    }

    m_visibilityMapValid = 0;
    if (redraw) {
        CompleteDraw(0);
        UpdateScreen(0, 0);
    }
}

void advManager::CheckDimHero(void) {
    if (!gbThisNetHumanPlayer[giCurPlayer] || gpCurPlayer->CurrentHero() == INVALID_HERO) {
        return;
    }
    if (!gpGame->IsMobile(gpCurPlayer->CurrentHero())) {
        ShowRoute(1, 0, 0);
        UpdateHeroLocators(1, 1);
        gpAdvManager->CheckDimNextHeroBut();
    }
}

void advManager::CheckDimNextHeroBut(void) {
    BaseWidgetCommand frame;
    frame = gbThisNetHumanPlayer[giCurPlayer] && gpCurPlayer->HasMobileHero()
        ? ADVMGR_BUTTON_DISABLE
        : ADVMGR_BUTTON_ENABLE;
    gpWindowManager->BroadcastMessage(
        ADVMGR_BUTTON_MESSAGE,
        frame,
        BUTTON_BROADCAST_ARG,
        BUTTON_BROADCAST_FLAGS
    );
}

void advManager::SeedTo(i32 targetX, i32 targetY) {
    hero* currentHero;

    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        return;
    }
    if (gpCurPlayer->m_currentHero == INVALID_HERO) {
        return;
    }

    currentHero = &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
    if (!giSeedingValid) {
        gpSearchArray->SeedPosition(
            currentHero->m_x,
            currentHero->m_y,
            m_cursorDirection,
            ROUTE_PATH_COST_LIMIT,
            m_cursorType == HERO_TYPE_BOAT,
            0,
            currentHero->m_remainingMobility,
            H2EnumIndex(currentHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_PATHFINDING)]),
            targetX,
            targetY,
            0,
            1
        );
    } else if (!giFullySeeded) {
        gpSearchArray->SeedPosition(
            currentHero->m_x,
            currentHero->m_y,
            m_cursorDirection,
            ROUTE_PATH_COST_LIMIT,
            m_cursorType == HERO_TYPE_BOAT,
            0,
            currentHero->m_remainingMobility,
            H2EnumIndex(currentHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_PATHFINDING)]),
            targetX,
            targetY,
            1,
            1
        );
    }
}

void advManager::ForceNewHover(void) {
    i32 x;
    i32 y;

    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        return;
    }
    gpMouseManager->MouseCoords(x, y);
    m_lastHoverCell = CURSOR_INVALID_POSITION;
    ProcessHover(x, y);
}

void advManager::ScreenScroll(MapDirection direction, i32 updatePointer) {
    i32 xOrigin;
    i32 yOrigin;

    xOrigin = m_mapOriginX;
    yOrigin = m_mapOriginY;
    iLastScrollTime = platform::Ticks();

    switch (direction) {
        case MAP_DIRECTION_NORTH:
            --yOrigin;
            break;
        case MAP_DIRECTION_NORTH_EAST:
            ++xOrigin;
            --yOrigin;
            break;
        case MAP_DIRECTION_EAST:
            ++xOrigin;
            break;
        case MAP_DIRECTION_SOUTH_EAST:
            ++xOrigin;
            ++yOrigin;
            break;
        case MAP_DIRECTION_SOUTH:
            ++yOrigin;
            break;
        case MAP_DIRECTION_SOUTH_WEST:
            --xOrigin;
            ++yOrigin;
            break;
        case MAP_DIRECTION_WEST:
            --xOrigin;
            break;
        case MAP_DIRECTION_NORTH_WEST:
            --xOrigin;
            --yOrigin;
            break;
    }

    if (updatePointer) {
        gpMouseManager->SetPointer(H2EnumIndex(direction) + H2EnumIndex(HOVER_SCROLL_FRAME_FIRST));
    }

    if (xOrigin < SCROLL_MIN_ORIGIN) {
        xOrigin = SCROLL_MIN_ORIGIN;
    }
    if (xOrigin > MAP_WIDTH - VIEW_EDGE_MARGIN) {
        xOrigin = MAP_WIDTH - VIEW_EDGE_MARGIN;
    }
    if (yOrigin < SCROLL_MIN_ORIGIN) {
        yOrigin = SCROLL_MIN_ORIGIN;
    }
    if (yOrigin > MAP_HEIGHT - VIEW_EDGE_MARGIN) {
        yOrigin = MAP_HEIGHT - VIEW_EDGE_MARGIN;
    }

    if (xOrigin != m_mapOriginX || yOrigin != m_mapOriginY) {
        DemobilizeCurrHero();
        m_mapOriginX = xOrigin;
        m_mapOriginY = yOrigin;
        UpdateRadar(1, 0);
        CompleteDraw(0);
        UpdateScreen(0, 0);
    }
}

void advManager::CheckScreenScroll(void) {
    i32 mouseX;
    i32 mouseY;
    i32 oldMapX;
    i32 oldMapY;

    if (platform::Ticks() - iLastScrollTime > SCROLL_TICK_INTERVAL) {
        iLastScrollTime = platform::Ticks();
        oldMapX = m_mapOriginX;
        oldMapY = m_mapOriginY;
        gpMouseManager->MouseCoords(mouseX, mouseY);

        if (mouseX >= 0 && mouseX < SCREEN_WIDTH && mouseY >= 0 && mouseY < SCREEN_HEIGHT) {
            if (mouseX < SCROLL_BORDER) {
                if (mouseY < SCROLL_BORDER) {
                    ScreenScroll(MAP_DIRECTION_NORTH_WEST, 1);
                } else if (mouseY > SCREEN_HEIGHT - SCROLL_BORDER) {
                    ScreenScroll(MAP_DIRECTION_SOUTH_WEST, 1);
                } else {
                    ScreenScroll(MAP_DIRECTION_WEST, 1);
                }
            } else if (mouseX > SCREEN_WIDTH - SCROLL_BORDER - 1) {
                if (mouseY < SCROLL_BORDER) {
                    ScreenScroll(MAP_DIRECTION_NORTH_EAST, 1);
                } else if (mouseY > SCREEN_HEIGHT - SCROLL_BORDER) {
                    ScreenScroll(MAP_DIRECTION_SOUTH_EAST, 1);
                } else {
                    ScreenScroll(MAP_DIRECTION_EAST, 1);
                }
            } else if (mouseY < SCROLL_BORDER) {
                ScreenScroll(MAP_DIRECTION_NORTH, 1);
            } else if (mouseY > SCREEN_HEIGHT - SCROLL_BORDER) {
                ScreenScroll(MAP_DIRECTION_SOUTH, 1);
            }
        }

        if (gpMouseManager->m_cursorFrame >= HOVER_SCROLL_FRAME_FIRST
            && gpMouseManager->m_cursorFrame < HOVER_SCROLL_FRAME_END && oldMapX == m_mapOriginX
            && oldMapY == m_mapOriginY) {
            gpMouseManager->SetPointer(POINTER_DEFAULT);
        }
    }
}

i32 advManager::MouseInScrollZone(void) {
    i32 x;
    i32 y;

    gpMouseManager->MouseCoords(x, y);
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        if (x < SCROLL_BORDER || x > SCREEN_WIDTH - SCROLL_BORDER - 1 || y < SCROLL_BORDER
            || y > SCREEN_HEIGHT - SCROLL_BORDER) {
            return 1;
        }
    }
    return 0;
}

void advManager::SetInitialMapOrigin(void) {
    game* gameState;
    town* currentTown9;
    playerData* initialPlayer8;
    hero* initialHero5;
    town* initialTown9;

    gpWindowManager->BroadcastMessage(
        ADVMGR_BUTTON_MESSAGE,
        ADVMGR_BUTTON_ENABLE,
        BUTTON_TARGET,
        BUTTON_BROADCAST_FLAGS
    );
    m_hoverCellY = 0;
    m_lastHoverCell = 0;
    m_cursorActive = 0;
    gbHeroMoving = false;

    if (gbThisNetHumanPlayer[giCurPlayer] && gpCurPlayer->m_currentTown != TOWN_ID_NONE) {
        currentTown9 = &gpGame->m_castleRecs[gpCurPlayer->m_currentTown];
        m_mapOriginX = currentTown9->m_x - VIEW_CENTER_OFFSET;
        m_mapOriginY = currentTown9->m_y - VIEW_CENTER_OFFSET;
    } else if (gbThisNetHumanPlayer[giCurPlayer] && gpCurPlayer->m_currentHero != INVALID_HERO) {
        MobilizeCurrHero(0);
    } else {
        if (gbThisNetHumanPlayer[giCurPlayer]) {
            initialPlayer8 = gpCurPlayer;
        } else {
            initialPlayer8 = &gpGame->m_players[giThisGamePos];
        }

        if (initialPlayer8->m_heroCount > 0) {
            initialHero5 = &gpGame->m_heroRecs[initialPlayer8->m_heroIds[0]];
            m_mapOriginX = initialHero5->m_x - VIEW_CENTER_OFFSET;
            m_mapOriginY = initialHero5->m_y - VIEW_CENTER_OFFSET;
        } else if (initialPlayer8->m_townCount > 0) {
            initialTown9 = &gpGame->m_castleRecs[initialPlayer8->m_townIds[0]];
            m_mapOriginX = initialTown9->m_x - VIEW_CENTER_OFFSET;
            m_mapOriginY = initialTown9->m_y - VIEW_CENTER_OFFSET;
        } else {
            m_mapOriginX = 0;
            m_mapOriginY = 0;
        }
    }

    m_currentTerrain = giGroundToTerrain
        [GetCell(m_mapOriginX + VIEW_CENTER_OFFSET, m_mapOriginY + VIEW_CENTER_OFFSET)
             ->m_terrainImageIndex];
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]);
    SetEnvironmentOrigin(m_mapOriginX + VIEW_CENTER_OFFSET, m_mapOriginY + VIEW_CENTER_OFFSET, 1);
    Reseed(0, 0);
    CheckDimNextHeroBut();
}

void advManager::LoadRemote(void) {
    if (gbThisNetHumanPlayer[giCurPlayer]) {
        gpMouseManager->SetPointer(
            "advmice.mse",
            POINTER_DEFAULT,
            MOUSE_AUTO_CURSOR_TYPE
        );
    }

    gpGame->LoadGame(gConfig.rmtRCName, 0, 1);
    if ((gpGame->m_day != 1 || (gpGame->m_week == 1 && gpGame->m_month == 1)) && gbRemoteOn
        && gbThisNetHumanPlayer[giCurPlayer]) {
        gSoundBackendsReady = 1;
        gpSoundManager->SwitchAmbientMusic(WAIT_AMBIENT_MUSIC);
        gSoundBackendsReady = 0;
        giForceSwitchMusic = platform::Ticks();
    }

    if (gpGame->m_playerDead[giCurPlayer]) {
        ComputeAdvNetControl();
    }

    if (gbThisNetHumanPlayer[giCurPlayer]) {
        gpGame->CancelComputerScreen();
        gbThisNetGotAdventureControl = true;
        gSoundBackendsReady = 0;
    }

    gpGame->DoNewTurn();
    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    UpdateRadar(1, 0);
    UpdBottomView(1, 1, 1);
    gpAdvManager->ForceNewHover();
    SendMapChange(MAP_CHANGE_MY_TURN, 0, 0, 0, MAP_CHANGE_CURRENT_PLAYER, 0, 0);
    gSoundBackendsReady = 1;

    if (static_cast<i8>(gpGame->m_cheated)) {
        static i32 cheatWarned = 0;
        if (!cheatWarned) {
            cheatWarned = 1;
            sprintf(
                gText,
                "\xc8\xf1\xef\xee\xeb\xfc\xe7\xf3\xfe\xf2\xf1\xff \xf7\xe8\xf2-\xea\xee\xe4\xfb!\n"
            );
            NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
        }
    }
    if (giDebugLevel > 0) {
        static i32 debugWarned = 0;
        if (!debugWarned) {
            debugWarned = 1;
            sprintf(
                gText,
                "Someone has their debug level set!\n"
            );
            NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
        }
    }
}

char* advManager::CheckHandleNet(void) {
    RemoteMessage* packet9;
    i32 playerExited5;
    SPlayerExit exitInfo4;

    packet9 = reinterpret_cast<RemoteMessage*>(GetRemoteData(ADVMGR_REMOTE_DATA_REQUEST));
    if (packet9
        && (packet9->type == REMOTE_MESSAGE_RELIABLE
            || packet9->type == REMOTE_MESSAGE_UNRELIABLE)) {
        switch (packet9->command) {
            case ADVMGR_REMOTE_COMMAND_SAVE_GAME:
                playerExited5 = ADVMGR_REMOTE_PAYLOAD(packet9)->savePlayerExited;
                if (!gpGame->ReceiveSaveGame(
                        ADVMGR_REMOTE_PAYLOAD(packet9)->saveDataSize,
                        ADVMGR_REMOTE_PAYLOAD(packet9)->saveCrc,
                        ADVMGR_REMOTE_PAYLOAD(packet9)->saveTransmitCrc,
                        packet9->sender
                    )) {
                    ShutDown(NULL);
                }
                if (playerExited5) {
                    exitInfo4.netPosition = packet9->sender;
                    exitInfo4.gamePosition = static_cast<i8>(NetPosToGamePos(packet9->sender));
                    exitInfo4.updateNetworkControl = 0;
                    exitInfo4.eliminated = 1;
                    exitInfo4.hostReported = 1;
                    exitInfo4.timedOut = 1;
                    ReceiveRemotePlayerExit(exitInfo4);
                }
                LoadRemote();
                break;

            case ADVMGR_REMOTE_COMMAND_POP_NET_BOX:
                PopNetBox(ADVMGR_REMOTE_PAYLOAD(packet9)->bytes, packet9->sender);
                break;

            case ADVMGR_REMOTE_COMMAND_COMBAT:
                if (gbInCombat) {
                    return reinterpret_cast<char*>(packet9);
                } else {
                    DoNetCombat(reinterpret_cast<char*>(packet9));
                }
                break;

            case ADVMGR_REMOTE_COMMAND_PLAYER_EXIT:
                LogStr("Receive Remote Player Exit");
                ReceiveRemotePlayerExit(ADVMGR_REMOTE_PAYLOAD(packet9)->playerExit);
                break;

            case ADVMGR_REMOTE_COMMAND_HOST_PLAYER_EXIT:
                LogStr("Host Reports Player Exit");
                ReceiveHostReportsPlayerExit(
                    packet9->sender,
                    ADVMGR_REMOTE_PAYLOAD(packet9)->playerExit,
                    0
                );
                break;

            case ADVMGR_REMOTE_COMMAND_GROUP_MAP_CHANGE:
                ProcessIncomingGroupMapChange(ADVMGR_REMOTE_PAYLOAD(packet9)->bytes);
                break;

            default:
                return reinterpret_cast<char*>(packet9);
        }
    }
    return NULL;
}

MessageDispatchResult
advManager::CheckHandleNetPlayerWait(struct tag_message& message, i32 doMain) {
    if (message.type == ADVMGR_REMOTE_WAIT_MOUSE_MESSAGE) {
        gpMouseManager->Main(message);
    }

    CheckDoMain(1, doMain);
    if (message.type == ADVMGR_REMOTE_WAIT_COMMAND_MESSAGE) {
        switch (message.payload.widget.command) {
            case ADVMGR_REMOTE_WAIT_POP_NET_BOX_COMMAND:
                PopNetBox(NULL, -1);
                break;

            case ADVMGR_REMOTE_WAIT_EXIT_COMMAND:
                if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))) {
                    message.type = ADVMGR_REMOTE_WAIT_EXIT_MESSAGE;
                    message.payload.executive.command = EXECUTIVE_COMMAND_TERMINATE_LOOP;
                    return MESSAGE_DISPATCH_FORWARD;
                }

            default:
                break;
        }
    }

    UpdBottomView(0, 1, 1);
    return MESSAGE_DISPATCH_CONTINUE;
}

void advManager::TrimLoopingSounds(i32 maxSamples) {
    if (giHighMemBuffer > 0) {
        maxSamples += giHighMemBuffer / HIGH_MEMORY_BUFFER_DIVISOR;
    }

    if (MAP_WIDTH != MAP_DIMENSION_XLARGE) {
        ++maxSamples;
    }

    if (maxSamples >= LOOPING_SAMPLE_COUNT) {
        return;
    }

    i32 loaded = 0;
    char keep[LOOPING_SAMPLE_COUNT];
    memset(keep, 0, sizeof(keep));

    i32 i;
    for (i = 0; i < SOUND_CELL_COUNT; ++i) {
        if (m_activeSounds[i].soundId >= ADVMGR_SOUND_BUOY
            && m_activeSounds[i].soundId < ADVMGR_ENVIRONMENT_SOUND_COUNT) {
            ++keep[H2EnumIndex(m_activeSounds[i].soundId)];
        }
    }

    for (i = 0; i < LOOPING_SAMPLE_COUNT; ++i) {
        if (keep[i] != 0) {
            ++loaded;
        }
    }

    if (loaded < maxSamples) {
        for (i = 0; i < LOOPING_SAMPLE_COUNT; ++i) {
            if (keep[i] == 0 && m_loopingSamples[i] != NULL) {
                ++keep[i];
                ++loaded;
                if (loaded >= maxSamples)
                    goto disposeSamples;
            }
        }
    }

disposeSamples:
    for (i = 0; i < LOOPING_SAMPLE_COUNT; ++i) {
        if (m_loopingSamples[i] != NULL && keep[i] == 0) {
            gpResourceManager->Dispose(m_loopingSamples[i]);
            m_loopingSamples[i] = NULL;
        }
    }
}

void advManager::DisableButtons(void) {
    if (gpAdvManager->m_active != 1) {
        return;
    }
    tag_message msg;
    msg.type = ADVMGR_BUTTON_MESSAGE;
    msg.payload.widget.command = ADVMGR_BUTTON_DISABLE;
    msg.payload.widget.data.value = BUTTON_TARGET;
    msg.payload.widget.id = BUTTON_SLOT_1;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_2;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_3;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_4;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_5;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_6;
    m_adventureWindow->BroadcastMessage(msg);
}

void advManager::EnableButtons(void) {
    if (gpAdvManager->m_active != 1) {
        return;
    }
    tag_message msg;
    msg.type = ADVMGR_BUTTON_MESSAGE;
    msg.payload.widget.command = ADVMGR_BUTTON_ENABLE;
    msg.payload.widget.data.value = BUTTON_TARGET;
    msg.payload.widget.id = BUTTON_SLOT_1;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_2;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_3;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_4;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_5;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_6;
    m_adventureWindow->BroadcastMessage(msg);
}

void advManager::SaveAdventureBorder(void) {

    if (m_adventureBorder != NULL) {
        return;
    }

    m_adventureBorder = static_cast<u8*>(H2_ALLOC(BORDER_BUFFER_SIZE));
    u8* savedPixels = m_adventureBorder;
    u8* screen = gpWindowManager->m_screen->m_pixels;
    i32 row;
    for (row = 0; row < BORDER_EDGE_SIZE; ++row) {
        memcpy(savedPixels, screen, BORDER_ROW_BYTES);
        screen += BORDER_SCREEN_PITCH;
        savedPixels += BORDER_ROW_BYTES;
    }
    for (row = BORDER_EDGE_SIZE; row < BORDER_MIDDLE_END; ++row) {
        memcpy(savedPixels, screen, BORDER_SIDE_BYTES);
        memcpy(savedPixels + BORDER_SIDE_BYTES, screen + BORDER_MIDDLE_END, BORDER_SIDE_BYTES);
        screen += BORDER_SCREEN_PITCH;
        savedPixels += BORDER_SAVED_SIDE_BYTES;
    }
    for (row = BORDER_MIDDLE_END; row < SCREEN_HEIGHT; ++row) {
        memcpy(savedPixels, screen, BORDER_ROW_BYTES);
        screen += BORDER_SCREEN_PITCH;
        savedPixels += BORDER_ROW_BYTES;
    }
}

void advManager::DrawAdventureBorder(void) {
    u8* savedPixels;
    u8* screen;
    i32 row;

    if (m_adventureBorder == NULL) {
        return;
    }
    if (gbNoBorder != 0) {
        return;
    }


    screen = gpWindowManager->m_screen->m_pixels;
    savedPixels = m_adventureBorder;
    for (row = 0; row < BORDER_EDGE_SIZE; ++row) {
        memcpy(screen, savedPixels, BORDER_ROW_BYTES);
        screen += BORDER_SCREEN_PITCH;
        savedPixels += BORDER_ROW_BYTES;
    }
    for (row = BORDER_EDGE_SIZE; row < BORDER_MIDDLE_END; ++row) {
        memcpy(screen, savedPixels, BORDER_SIDE_BYTES);
        memcpy(screen + BORDER_MIDDLE_END, savedPixels + BORDER_SIDE_BYTES, BORDER_SIDE_BYTES);
        screen += BORDER_SCREEN_PITCH;
        savedPixels += BORDER_SAVED_SIDE_BYTES;
    }
    for (row = BORDER_MIDDLE_END; row < SCREEN_HEIGHT; ++row) {
        memcpy(screen, savedPixels, BORDER_ROW_BYTES);
        screen += BORDER_SCREEN_PITCH;
        savedPixels += BORDER_ROW_BYTES;
    }
}

i32 advManager::FindAdjacentMonster(
    i32 originX,
    i32 originY,
    i32* monsterX,
    i32* monsterY,
    i32 excludedX,
    i32 excludedY
) {
    s_adjacentMonsterEndX = originX + ADJACENT_MONSTER_END_OFFSET;
    s_adjacentMonsterEndY = originY + ADJACENT_MONSTER_END_OFFSET;

    if (originX > 0 && originY > 0 && originX < MAP_WIDTH - 1 && originY < MAP_HEIGHT - 1) {
        for (s_adjacentMonsterX = originX - ADJACENT_MONSTER_RADIUS;
             s_adjacentMonsterX < s_adjacentMonsterEndX;
             ++s_adjacentMonsterX) {
            for (s_adjacentMonsterY = originY - ADJACENT_MONSTER_RADIUS;
                 s_adjacentMonsterY < s_adjacentMonsterEndY;
                 ++s_adjacentMonsterY) {
                if (m_mapData->GetCell(s_adjacentMonsterX, s_adjacentMonsterY)->m_triggerType
                    == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER)) {
                    if (s_adjacentMonsterY < originY) {
                        if ((GetCell(originX, originY)->m_objectIndex == ADJACENT_OBJECT_INDEX_NONE
                             || GetCell(originX, originY)->m_objectTileset == TILESET_DUMMY
                             || (GetCell(originX, originY)->m_flags & HOVER_UNREACHABLE))
                            && (s_adjacentMonsterX != excludedX
                                || s_adjacentMonsterY != excludedY)) {
                            goto foundAdjacentMonster;
                        }
                    } else if (s_adjacentMonsterX != excludedX || s_adjacentMonsterY != excludedY) {
                        goto foundAdjacentMonster;
                    }
                }
            }
        }
    } else {
        if (originX == MAP_WIDTH - 1) {
            s_adjacentMonsterEndX = originX + 1;
        }
        if (originY == MAP_HEIGHT - 1) {
            s_adjacentMonsterEndY = originY + 1;
        }
        if (originX == 0) {
            s_adjacentMonsterMinX = 0;
        } else {
            s_adjacentMonsterMinX = originX - ADJACENT_MONSTER_RADIUS;
        }
        if (originY == 0) {
            s_adjacentMonsterMinY = 0;
        } else {
            s_adjacentMonsterMinY = originY - ADJACENT_MONSTER_RADIUS;
        }

        for (s_adjacentMonsterX = s_adjacentMonsterMinX; s_adjacentMonsterX < s_adjacentMonsterEndX;
             ++s_adjacentMonsterX) {
            for (s_adjacentMonsterY = s_adjacentMonsterMinY;
                 s_adjacentMonsterY < s_adjacentMonsterEndY;
                 ++s_adjacentMonsterY) {
                if (m_mapData->GetCell(s_adjacentMonsterX, s_adjacentMonsterY)->m_triggerType
                    == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER)) {
                    if (s_adjacentMonsterY < originY) {
                        if ((GetCell(originX, originY)->m_objectIndex == ADJACENT_OBJECT_INDEX_NONE
                             || GetCell(originX, originY)->m_objectTileset == TILESET_DUMMY
                             || (GetCell(originX, originY)->m_flags & HOVER_UNREACHABLE))
                            && (s_adjacentMonsterX != excludedX
                                || s_adjacentMonsterY != excludedY)) {
                            goto foundAdjacentMonster;
                        }
                    } else if (s_adjacentMonsterX != excludedX || s_adjacentMonsterY != excludedY) {
                        goto foundAdjacentMonster;
                    }
                }
            }
        }
    }
    return 0;

foundAdjacentMonster:
    *monsterX = s_adjacentMonsterX;
    *monsterY = s_adjacentMonsterY;
    return 1;
}

void ComputeAdvNetControl(void) {
    if (!gbRemoteOn) {
        gbThisNetGotAdventureControl = true;
        return;
    }
    {
        i32 selected = -1;
        i32 player;
        i32 myPlayer;
        if (gpGame->m_playerDead[giCurPlayer]) {
            player = (giCurPlayer + 1) % GAME_PLAYER_COUNT;
            while (player != giCurPlayer) {
                if (!gpGame->m_playerDead[player] && gbHumanPlayer[player]) {
                    gbThisNetGotAdventureControl = gbThisNetHumanPlayer[player];
                    return;
                }
            }
        }

        player = (giCurPlayer + 1) % GAME_PLAYER_COUNT;
        while (player != giCurPlayer) {
            player = (player + 1) % GAME_PLAYER_COUNT;
            if (!gpGame->m_playerDead[player] && gbHumanPlayer[player]) {
                selected = player;
            }
        }
        gbThisNetGotAdventureControl = gbThisNetHumanPlayer[selected];
    }
}

i32 MapExtraPosAndAdjacentsSet(i32 x, i32 y, u8 mask) {
    if (MAP_EXTRA_AT_WFIRST(x, y) & mask) {
        return 1;
    }
    for (i32 checkX = x - 1; checkX <= x + 1; ++checkX) {
        if (checkX < 0 || checkX >= MAP_WIDTH) {
            continue;
        }
        for (i32 checkY = y - 1; checkY <= y + 1; ++checkY) {
            if (checkY < 0 || checkY >= MAP_HEIGHT) {
                continue;
            }
            if (MAP_EXTRA_AT_WFIRST(checkX, checkY) & mask) {
                return 1;
            }
        }
    }
    return 0;
}

void advManager::ViewPuzzle(void) {
    i32 puzzleX;
    i32 puzzleY;
    i32 i;
    i32 visibleCount;
    icon* puzzleIcn;
    heroWindow* pWin;

    gpGame->SetupPuzzlePieces(giCurPlayer, 0);
    u8 order[PUZZLE_PIECE_COUNT] = {23, 7,  44, 5,  24, 47, 1,  39, 16, 36, 11, 45,
                                    31, 2,  30, 38, 43, 4,  3,  14, 40, 37, 34, 0,
                                    12, 17, 35, 42, 15, 8,  26, 41, 28, 46, 10, 22,
                                    21, 6,  32, 18, 19, 29, 13, 27, 9,  20, 33, 25};
    visibleCount = 0;

    gpSoundManager->SwitchAmbientMusic(PUZZLE_MUSIC);
    gpMouseManager->SetPointer(
        "advmice.mse",
        POINTER_DEFAULT,
        MOUSE_AUTO_CURSOR_TYPE
    );
    puzzleIcn = gpResourceManager->GetIcon("puzzle.icn");
    i32 j;
    for (j = 0; j < PUZZLE_PIECE_COUNT; ++j) {
        puzzleIcn->DrawToBuffer(0, 0, j, ICON_DRAW_NORMAL);
    }

    gpWindowManager->UpdateScreenRegion(
        PUZZLE_VIEW_ORIGIN,
        PUZZLE_VIEW_ORIGIN,
        PUZZLE_VIEW_SIZE,
        PUZZLE_VIEW_SIZE
    );
    gpWindowManager->SaveFizzleSource(
        PUZZLE_VIEW_ORIGIN,
        PUZZLE_VIEW_ORIGIN,
        PUZZLE_VIEW_SIZE,
        PUZZLE_VIEW_SIZE
    );
    pWin = new heroWindow(
        PUZZLE_WINDOW_X,
        PUZZLE_WINDOW_Y,
        "viewpuzl.bin"
    );
    if (pWin == NULL) {
        MemError();
    }
    gpWindowManager->AddWindow(pWin, -1, 1);

    puzzleX = gpGame->m_ultimateArtifactX - PUZZLE_COORDINATE_OFFSET;
    puzzleY = gpGame->m_ultimateArtifactY - PUZZLE_COORDINATE_OFFSET;
    i32 xOff = 0;
    i32 yOff = 0;
    xOff =
        (gpGame->m_ultimateArtifactX + gpGame->m_ultimateArtifactY) % PUZZLE_ALIGNMENT_DIVISOR - 1;
    yOff = (gpGame->m_ultimateArtifactX * PUZZLE_Y_ADJUST_X_FACTOR
            + gpGame->m_ultimateArtifactY * PUZZLE_Y_ADJUST_Y_FACTOR)
               % PUZZLE_ALIGNMENT_DIVISOR
           - 1;
    if ((gpGame->m_ultimateArtifactX + gpGame->m_ultimateArtifactY) % PUZZLE_ALIGNMENT_DIVISOR
        == 1) {
        if (xOff > 0) {
            ++xOff;
        } else if (xOff < 0) {
            --xOff;
        }
    } else if ((gpGame->m_ultimateArtifactX + gpGame->m_ultimateArtifactY) % PUZZLE_PARITY_DIVISOR
               == 1) {
        if (yOff > 0) {
            ++yOff;
        } else if (yOff < 0) {
            --yOff;
        }
    }

    puzzleX += xOff;
    puzzleY += yOff;
    PuzzleDraw(puzzleX, puzzleY, gpGame->m_ultimateArtifactX, gpGame->m_ultimateArtifactY);

    u8* pixPtr;
    u8* end;
    for (i = PUZZLE_VIEW_ORIGIN; i < PUZZLE_VIEW_END; ++i) {
        pixPtr =
            gpWindowManager->m_screen->m_pixels + i * SCREEN_WIDTH + PUZZLE_VIEW_ORIGIN;
        end = pixPtr + PUZZLE_VIEW_SIZE;
        for (; pixPtr < end; ++pixPtr) {
            *pixPtr = gColorTableTan[*pixPtr];
        }
    }

    for (j = 0; j < PUZZLE_PIECE_COUNT; ++j) {
        if (!H2BitTest(puzzlePiecesRemoved, j)) {
            puzzleIcn->DrawToBuffer(0, 0, order[j], ICON_DRAW_NORMAL);
            ++visibleCount;
        }
    }
    if (visibleCount != PUZZLE_PIECE_COUNT) {
        gpWindowManager->FizzleForward(
            PUZZLE_VIEW_ORIGIN,
            PUZZLE_VIEW_ORIGIN,
            PUZZLE_VIEW_SIZE,
            PUZZLE_VIEW_SIZE,
            PUZZLE_FIZZLE_TIME,
            NULL,
            NULL
        );
    } else {
        gpWindowManager->ReleaseFizzleSource();
    }

    gpWindowManager->DoDialog(pWin, EventWindowHandler, 0);
    delete pWin;
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    UpdateScreen(0, 0);
    UpdateRadar(1, 0);
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]);
}

void advManager::PuzzleDraw(i32 left, i32 top, i32 right, i32 bottom) {
    i32 unusedPuzzle0;
    i32 unused1;
    i32 unused2;
    i32 unused3;
    i32 unused4;
    i32 unused6;

    gbDrawingPuzzle = true;
    CompleteDraw(left, top, 0, 0);
    gbDrawingPuzzle = false;
    IconToBitmap(
        m_objectIcons[H2EnumIndex(TILESET_ROUTE)],
        gpWindowManager->m_screen,
        (right - left) * PUZZLE_TILE_SIZE - PUZZLE_X_TRIM,
        (bottom - top) * PUZZLE_TILE_SIZE,
        0,
        ICON_DRAW_CLIP,
        0,
        0,
        SCREEN_HEIGHT,
        SCREEN_HEIGHT,
        0
    );
}

void advManager::AdvPanel(void) {
    heroWindow* adventurePanel;
    {
        TrimLoopingSounds(LOOPING_SOUND_LIMIT);
        gpMouseManager->SetPointer(
            "advmice.mse",
            POINTER_DEFAULT,
            MOUSE_AUTO_CURSOR_TYPE
        );
        i32 heroWasMobilized = m_heroContextLocked;
        tag_message message;
        DemobilizeCurrHero();

        adventurePanel = new heroWindow(
            PANEL_WINDOW_X,
            PANEL_WINDOW_Y,
            "apanel.bin"
        );
        if (adventurePanel == NULL) {
            MemError();
        }
        if (gpCurPlayer->m_currentHero == INVALID_HERO) {
            message.type = MESSAGE_WIDGET;
            message.payload.widget.id = PANEL_DISABLED_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = BUTTON_TARGET;
            adventurePanel->BroadcastMessage(message);
            message.payload.widget.id = PANEL_DISABLED_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_COMMAND_DIMMED);
            adventurePanel->BroadcastMessage(message);
        }

        gpWindowManager->DoDialog(adventurePanel, APanelHandler, 0);
        delete adventurePanel;
        switch (gpWindowManager->m_dialogResult) {
            case PANEL_SCENARIO_INFO:
                if (gbInCampaign) {
                    SetEnvironmentOrigin(ENVIRONMENT_ORIGIN_NONE, ENVIRONMENT_ORIGIN_NONE, 1);
                    gpGame->ShowCampaignInfo(1, 0);
                    SetEnvironmentOrigin(
                        m_mapOriginX + VIEW_CENTER_OFFSET,
                        m_mapOriginY + VIEW_CENTER_OFFSET,
                        1
                    );
                    RedrawAdvScreen(1, 0);
                    gpSoundManager->SwitchAmbientMusic(
                        giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]
                    );
                } else if (xIsPlayingExpansionCampaign) {
                    SetEnvironmentOrigin(ENVIRONMENT_ORIGIN_NONE, ENVIRONMENT_ORIGIN_NONE, 1);
                    xCampaign.ShowInfo(1, 0);
                    SetEnvironmentOrigin(
                        m_mapOriginX + VIEW_CENTER_OFFSET,
                        m_mapOriginY + VIEW_CENTER_OFFSET,
                        1
                    );
                    RedrawAdvScreen(1, 0);
                    gpSoundManager->SwitchAmbientMusic(
                        giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]
                    );
                } else {
                    gpGame->ShowScenInfo();
                }
                break;
            case PANEL_SEARCH:
                ProcessSearch(CURSOR_INVALID_POSITION, CURSOR_INVALID_POSITION);
                break;
            case PANEL_VIEW_WORLD:
                ViewWorld(SPELL_VIEW_ALL, false, false);
                break;
            case PANEL_VIEW_PUZZLE:
                ViewPuzzle();
                break;
        }

        if (heroWasMobilized) {
            MobilizeCurrHero(0);
        }
    }
}

MessageDispatchResult APanelHandler(tag_message& message) {
    i32 handled = 0;
    if (message.type == MESSAGE_WIDGET) {
        if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
            if (message.payload.widget.command == WIDGET_COMMAND_SELECT
                || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT) {
                i32 helpIndex = PANEL_NO_HELP;
                switch (message.payload.widget.id) {
                    case PANEL_VIEW_WORLD:
                        helpIndex = PANEL_VIEW_WORLD_HELP;
                        break;
                    case PANEL_VIEW_PUZZLE:
                        helpIndex = PANEL_VIEW_PUZZLE_HELP;
                        break;
                    case PANEL_SCENARIO_INFO:
                        helpIndex = PANEL_SCENARIO_INFO_HELP;
                        break;
                    case PANEL_SEARCH:
                        helpIndex = PANEL_SEARCH_HELP;
                        break;
                    case PANEL_CLOSE_WIDGET:
                        helpIndex = PANEL_CLOSE_HELP;
                        break;
                }
                if (helpIndex >= 0) {
                    NormalDialog(
                        gAPanelHelp[helpIndex],
                        NORMAL_DIALOG_QUICK_VIEW,
                        NORMAL_DIALOG_NO_RESOURCE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0
                    );
                }
            }
        } else {
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_DESELECT:
                    switch (message.payload.widget.id) {
                        case PANEL_VIEW_WORLD:
                        case PANEL_VIEW_PUZZLE:
                        case PANEL_SCENARIO_INFO:
                        case PANEL_SEARCH:
                        case PANEL_CLOSE_WIDGET:
                            handled = 1;
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    if (handled) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT);
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

i32 advManager::ControlPanel(void) {
    tag_message message;

    TrimLoopingSounds(LOOPING_SOUND_LIMIT);
    i32 selectedCommand = PANEL_NO_HELP;
    gpMouseManager->SetPointer(
        "advmice.mse",
        POINTER_DEFAULT,
        MOUSE_AUTO_CURSOR_TYPE
    );
    i32 heroWasMobilized = m_heroContextLocked;
    DemobilizeCurrHero();

    heroWindow* panel = new heroWindow(
        PANEL_WINDOW_X,
        PANEL_WINDOW_Y,
        "cpanel.bin"
    );
    if (panel == NULL) {
        MemError();
    }
    if (gbRemoteOn) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.id = CONTROL_RESTART;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_COMMAND_DIMMED);
        panel->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = BUTTON_TARGET;
        panel->BroadcastMessage(message);
        message.payload.widget.id = CONTROL_NEW_GAME;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_COMMAND_DIMMED);
        panel->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = BUTTON_TARGET;
        panel->BroadcastMessage(message);
    }

    gpWindowManager->DoDialog(panel, CPanelHandler, 0);
    delete panel;
    switch (gpWindowManager->m_dialogResult) {
        case CONTROL_RESTART:
        case CONTROL_NEW_GAME:
        case CONTROL_MAIN_MENU:
            selectedCommand = gpWindowManager->m_dialogResult;
            break;
        case CONTROL_SAVE_GAME:
            SaveGame();
            break;
    }
    if (heroWasMobilized) {
        MobilizeCurrHero(0);
    }
    if (selectedCommand != PANEL_NO_HELP) {
        gGameCommand = selectedCommand;
        return 1;
    }
    return 0;
}

MessageDispatchResult CPanelHandler(tag_message& message) {
    i32 handled = 0;
    char question[CONTROL_CONFIRMATION_SIZE];
    i32 helpIndex;

    if (message.type == MESSAGE_WIDGET) {
        if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
            if (message.payload.widget.command == WIDGET_COMMAND_SELECT
                || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT) {
                helpIndex = PANEL_NO_HELP;
                switch (message.payload.widget.id) {
                    case CONTROL_RESTART:
                        helpIndex = CONTROL_RESTART_HELP;
                        break;
                    case CONTROL_NEW_GAME:
                        helpIndex = CONTROL_NEW_GAME_HELP;
                        break;
                    case CONTROL_SAVE_GAME:
                        helpIndex = CONTROL_SAVE_GAME_HELP;
                        break;
                    case CONTROL_MAIN_MENU:
                        helpIndex = CONTROL_MAIN_MENU_HELP;
                        break;
                    case PANEL_CLOSE_WIDGET:
                        helpIndex = CONTROL_CLOSE_HELP;
                        break;
                }
                if (helpIndex >= 0) {
                    NormalDialog(
                        gCPanelHelp[helpIndex],
                        NORMAL_DIALOG_QUICK_VIEW,
                        NORMAL_DIALOG_NO_RESOURCE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0
                    );
                }
            }
        } else {
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_DESELECT:
                    switch (message.payload.widget.id) {
                        case CONTROL_RESTART:
                            strcpy(
                                question,
                                "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee "
                                    "\xf5\xee\xf2\xe8\xf2\xe5 \xed\xe0\xf7\xe0\xf2\xfc \xf1\xed\xe0"
                                    "\xf7\xe0\xeb\xe0?  (\xdd\xf2\xe0 \xe8\xe3\xf0\xe0 \xe1\xf3\xe4"
                                    "\xe5\xf2 \xef\xee\xf2\xe5\xf0\xff\xed\xe0)"

                            );
                            goto confirm_reset;
                        case CONTROL_NEW_GAME:
                            strcpy(
                                question,
                                "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee "
                                    "\xf5\xee\xf2\xe8\xf2\xe5 \xe7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc "
                                    "\xed\xee\xe2\xf3\xfe \xe8\xe3\xf0\xf3? (\xdd\xf2\xe0 \xe8\xe3\xf0"
                                    "\xe0 \xe1\xf3\xe4\xe5\xf2 \xef\xee\xf2\xe5\xf0\xff\xed\xe0)"

                            );
                            goto confirm_reset;
                        case CONTROL_MAIN_MENU:
                            strcpy(
                                question,
                                "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee "
                                    "\xf5\xee\xf2\xe8\xf2\xe5 \xe2\xfb\xe9\xf2\xe8?"

                            );
                        confirm_reset:
                            handled = 1;
                            if (!bFreshSave) {
                                NormalDialog(
                                    question,
                                    NORMAL_DIALOG_CONFIRM,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0
                                );
                                if (gpWindowManager->m_dialogResult == DIALOG_OK) {
                                    handled = 0;
                                }
                            }
                            break;
                        case CONTROL_SAVE_GAME:
                        case PANEL_CLOSE_WIDGET:
                            handled = 1;
                            break;
                        default:
                            break;
                    }
                    break;
            }
        }
    }

    if (handled) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT);
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

void advManager::SystemOptions(void) {
    tag_message message;
    i32 oldInterfaceMode;
    ConfigWalkSpeed prevWalkSpeed;
    i32 heroMobile;
    i32 n;

    TrimLoopingSounds(LOOPING_SOUND_LIMIT);
    gpMouseManager->SetPointer(
        "advmice.mse",
        POINTER_DEFAULT,
        MOUSE_AUTO_CURSOR_TYPE
    );
    prevWalkSpeed = gConfig.walkSpeed;
    oldInterfaceMode = gConfig.evilInterfaceUsage;
    heroMobile = m_heroContextLocked;
    bPrefsChanged = 0;
    DemobilizeCurrHero();

    cPanel = new heroWindow(
        ADVMGR_SYSTEM_OPTIONS_WINDOW_X,
        ADVMGR_SYSTEM_OPTIONS_WINDOW_Y,
        "spanel.bin"
    );
    if (cPanel == NULL) {
        MemError();
    }
    SetWinText(cPanel, ADVMGR_SYSTEM_OPTIONS_TITLE);
    UpdateSystemOptions(1);
    gpWindowManager->DoDialog(cPanel, SystemOptionsHandler, 0);
    delete cPanel;

    if (gConfig.walkSpeed != prevWalkSpeed) {
        for (n = 0; n < CURSOR_SAMPLE_COUNT; ++n) {
            gpResourceManager->Dispose(m_cursorSamples[n]);
        }
        GetCursorSampleSet(gConfig.walkSpeed);
    }
    if (bPrefsChanged) {
        WritePrefs();
    }
    if (oldInterfaceMode != gConfig.evilInterfaceUsage) {
        CheckSetEvilInterface(1, -1);
    }
    if (heroMobile) {
        MobilizeCurrHero(0);
    }
}

void UpdateSystemOptions(i32 initialDraw) {
    AdventureMusicQuality musicQuality;
    tag_message msg;
    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.command = ADVMGR_SYSTEM_OPTIONS_SET_FRAME;

    msg.payload.widget.id = H2EnumIndex(SYSTEM_OPTION_MUSIC_VOLUME);
    msg.payload.widget.data.value = gConfig.musicVolume != CONFIG_VOLUME_MUTED;
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id = H2EnumIndex(SYSTEM_OPTION_SOUND_VOLUME);
    msg.payload.widget.data.value = static_cast<i32>(gConfig.soundVolume != CONFIG_VOLUME_MUTED)
                                    + ADVMGR_SYSTEM_OPTIONS_SOUND_FRAME_BASE;
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id = H2EnumIndex(SYSTEM_OPTION_HERO_SPEED);
    msg.payload.widget.data.value =
        H2EnumIndex(gConfig.walkSpeed) + ADVMGR_SYSTEM_OPTIONS_SPEED_FRAME_BASE;
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id = H2EnumIndex(SYSTEM_OPTION_MUSIC_SOURCE);
    if (gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI) {
        musicQuality = MUSIC_QUALITY_MIDI;
    } else if (gConfig.useOpera == CONFIG_OPERA_DISABLED) {
        musicQuality = MUSIC_QUALITY_CD_STEREO;
    } else {
        musicQuality = MUSIC_QUALITY_CD_OPERA;
    }
    msg.payload.widget.data.value =
        H2EnumIndex(musicQuality) + ADVMGR_SYSTEM_OPTIONS_MUSIC_SOURCE_FRAME_BASE;
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id = H2EnumIndex(SYSTEM_OPTION_SHOW_ROUTE);
    msg.payload.widget.data.value =
        static_cast<i32>(gConfig.showRoute == 0) + ADVMGR_SYSTEM_OPTIONS_ROUTE_FRAME_BASE;
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id = H2EnumIndex(SYSTEM_OPTION_COMPUTER_SPEED);
    if (gConfig.blackoutComputer != 0) {
        msg.payload.widget.data.value = ADVMGR_SYSTEM_OPTIONS_COMPUTER_HIDDEN_FRAME;
    } else {
        msg.payload.widget.data.value =
            H2EnumIndex(gConfig.computerWalkSpeed) + ADVMGR_SYSTEM_OPTIONS_SPEED_FRAME_BASE;
    }
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id = H2EnumIndex(SYSTEM_OPTION_INTERFACE);
    msg.payload.widget.data.value =
        gConfig.evilInterfaceUsage + ADVMGR_SYSTEM_OPTIONS_INTERFACE_FRAME_BASE;
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id = H2EnumIndex(SYSTEM_OPTION_VIDEO);
    msg.payload.widget.data.value =
        static_cast<i32>(gConfig.slowVideo != 0) + ADVMGR_SYSTEM_OPTIONS_VIDEO_FRAME_BASE;
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id = H2EnumIndex(SYSTEM_OPTION_COLOR_CURSOR);
    msg.payload.widget.data.value = gConfig.gfx[H2EnumIndex(CONFIG_EXECUTABLE_GAME)].colorMouseCursor
                                        + ADVMGR_SYSTEM_OPTIONS_CURSOR_FRAME_BASE;
    cPanel->BroadcastMessage(msg);

    msg.payload.widget.command = ADVMGR_SYSTEM_OPTIONS_SET_TEXT;
    msg.payload.widget.id =
        H2EnumIndex(SYSTEM_OPTION_MUSIC_VOLUME) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    msg.payload.widget.data.text = onOffText[H2EnumIndex(gConfig.musicVolume)];
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id =
        H2EnumIndex(SYSTEM_OPTION_SOUND_VOLUME) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    msg.payload.widget.data.text = onOffText[H2EnumIndex(gConfig.soundVolume)];
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id =
        H2EnumIndex(SYSTEM_OPTION_HERO_SPEED) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    msg.payload.widget.data.text = walkSpeedText[H2EnumIndex(gConfig.walkSpeed)];
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id =
        H2EnumIndex(SYSTEM_OPTION_MUSIC_SOURCE) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    msg.payload.widget.data.text = musicQualityText[H2EnumIndex(musicQuality)];
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id =
        H2EnumIndex(SYSTEM_OPTION_SHOW_ROUTE) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    msg.payload.widget.data.text = onOffText[gConfig.showRoute];
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id =
        H2EnumIndex(SYSTEM_OPTION_COMPUTER_SPEED) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    if (gConfig.blackoutComputer != 0) {
        msg.payload.widget.data.text = "\xcd\xe5 \xef\xee\xea\xe0\xe7\xfb\xe2\xe0\xf2\xfc";
    } else {
        msg.payload.widget.data.text = walkSpeedText[H2EnumIndex(gConfig.computerWalkSpeed)];
    }
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id = H2EnumIndex(SYSTEM_OPTION_INTERFACE) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    msg.payload.widget.data.text = gInterfaceTypeText[gConfig.evilInterfaceUsage];
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id = H2EnumIndex(SYSTEM_OPTION_VIDEO) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    msg.payload.widget.data.text = cSlowVideoLevelText[gConfig.slowVideo != 0];
    cPanel->BroadcastMessage(msg);
    msg.payload.widget.id =
        H2EnumIndex(SYSTEM_OPTION_COLOR_CURSOR) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    msg.payload.widget.data.text =
        cBWMouseText[gConfig.gfx[H2EnumIndex(CONFIG_EXECUTABLE_GAME)].colorMouseCursor];
    cPanel->BroadcastMessage(msg);

    if (initialDraw == 0) {
        cPanel->DrawWindow(1, 0, ADVMGR_SYSTEM_OPTIONS_DRAW_MASK);
    }
}


static inline bool RedbookMusicPresent(void) {
    return true;
}

static inline bool MidiMusicPresent(void) {
    return true;
}

MessageDispatchResult SystemOptionsHandler(struct tag_message& message) {
    i32 preferencesChanged = 0;
    i32 accepted = 0;

    if (message.type == ADVMGR_SYSTEM_OPTIONS_MESSAGE) {
        if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
            if (message.payload.widget.command == ADVMGR_SYSTEM_OPTIONS_ACTIVATE
                || message.payload.widget.command == ADVMGR_SYSTEM_OPTIONS_HOVER) {
                i32 helpIndex = OPTION_DIALOG_NONE;

                switch (static_cast<AdventureSystemOptionsWidgetId>(message.payload.widget.id)) {
                    case SYSTEM_OPTIONS_DIALOG_ACCEPT:
                        helpIndex = SYSTEM_OPTIONS_HELP_ACCEPT;
                        break;
                    case SYSTEM_OPTION_MUSIC_VOLUME:
                        helpIndex = SYSTEM_OPTIONS_HELP_MUSIC_VOLUME;
                        break;
                    case SYSTEM_OPTION_SOUND_VOLUME:
                        helpIndex = SYSTEM_OPTIONS_HELP_SOUND_VOLUME;
                        break;
                    case SYSTEM_OPTION_HERO_SPEED:
                        helpIndex = SYSTEM_OPTIONS_HELP_HERO_SPEED;
                        break;
                    case SYSTEM_OPTION_MUSIC_SOURCE:
                        helpIndex = SYSTEM_OPTIONS_HELP_MUSIC_SOURCE;
                        break;
                    case SYSTEM_OPTION_SHOW_ROUTE:
                        helpIndex = SYSTEM_OPTIONS_HELP_SHOW_ROUTE;
                        break;
                    case SYSTEM_OPTION_COMPUTER_SPEED:
                        helpIndex = SYSTEM_OPTIONS_HELP_COMPUTER_SPEED;
                        break;
                    case SYSTEM_OPTION_INTERFACE:
                        helpIndex = SYSTEM_OPTIONS_HELP_INTERFACE;
                        break;
                    case SYSTEM_OPTION_VIDEO:
                        helpIndex = SYSTEM_OPTIONS_HELP_VIDEO;
                        break;
                    case SYSTEM_OPTION_COLOR_CURSOR:
                        helpIndex = SYSTEM_OPTIONS_HELP_COLOR_CURSOR;
                        break;
                }

                if (helpIndex >= 0) {
                    NormalDialog(
                        gSPanelHelp[helpIndex],
                        OPTION_DIALOG_HELP,
                        OPTION_DIALOG_NONE,
                        OPTION_DIALOG_NONE,
                        OPTION_DIALOG_NONE,
                        0,
                        OPTION_DIALOG_NONE,
                        0,
                        OPTION_DIALOG_NONE,
                        0
                    );
                }
            }
        } else {
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_DESELECT:
                    switch (
                        static_cast<AdventureSystemOptionsWidgetId>(message.payload.widget.id)
                    ) {
                        case SYSTEM_OPTIONS_DIALOG_ACCEPT:
                            accepted = 1;
                            break;
                    }
                    break;

                case ADVMGR_SYSTEM_OPTIONS_ACTIVATE: {
                    switch (
                        static_cast<AdventureSystemOptionsWidgetId>(message.payload.widget.id)
                    ) {
                        case SYSTEM_OPTION_MUSIC_VOLUME:
                            if (gConfig.musicVolume == CONFIG_VOLUME_MUTED
                                && !RedbookMusicPresent() && !MidiMusicPresent()) {
                                NormalDialog(
                                    "\xdd\xf2\xe0 \xf1\xe8\xf1\xf2\xe5\xec\xe0 \xed\xe5 \xef\xee"
                                        "\xe7\xe2\xee\xeb\xff\xe5\xf2 \xef\xf0\xee\xe8\xe3\xf0\xfb\xe2"
                                        "\xe0\xf2\xfc MIDI \xe8 Redbook \xec\xf3\xe7\xfb\xea\xf3."
                                         ,
                                    OPTION_DIALOG_MESSAGE,
                                    OPTION_DIALOG_NONE,
                                    OPTION_DIALOG_NONE,
                                    OPTION_DIALOG_NONE,
                                    0,
                                    OPTION_DIALOG_NONE,
                                    0,
                                    OPTION_DIALOG_NONE,
                                    0
                                );
                                break;
                            }
                            gConfig.musicVolume =
                                (gConfig.musicVolume + 1) % CONFIG_VOLUME_LEVEL_COUNT;
                            gpSoundManager->AdjustMusicVolumes();
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case SYSTEM_OPTION_SOUND_VOLUME:
                            if (gConfig.soundVolume == CONFIG_VOLUME_MUTED
                                && static_cast<bool>(IsAudiereBackend(gpSoundManager)
                                                     || IsMilesBackend(gpSoundManager))
                                       == false) {
                                NormalDialog(
                                    "\xd6\xe8\xf4\xf0\xee\xe2\xe0\xff \xec\xf3\xe7\xfb\xea\xe0 "
                                        "\xed\xe5 \xe4\xee\xf1\xf2\xf3\xef\xed\xe0 \xe2 \xfd\xf2\xee"
                                        "\xe9 \xf1\xe8\xf1\xf2\xe5\xec\xe5."
                                         ,
                                    OPTION_DIALOG_MESSAGE,
                                    OPTION_DIALOG_NONE,
                                    OPTION_DIALOG_NONE,
                                    OPTION_DIALOG_NONE,
                                    0,
                                    OPTION_DIALOG_NONE,
                                    0,
                                    OPTION_DIALOG_NONE,
                                    0
                                );
                                break;
                            }
                            gConfig.soundVolume =
                                (gConfig.soundVolume + 1) % CONFIG_VOLUME_LEVEL_COUNT;
                            gpSoundManager->AdjustSoundVolumes();
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case SYSTEM_OPTION_HERO_SPEED:
                            ++gConfig.walkSpeed;
                            gConfig.walkSpeed %= CONFIG_WALK_SPEED_COUNT;
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case SYSTEM_OPTION_COMPUTER_SPEED:
                            if (gConfig.blackoutComputer) {
                                gConfig.blackoutComputer = 0;
                                gConfig.computerWalkSpeed = CONFIG_WALK_SPEED_NORMAL;
                            } else if (gConfig.computerWalkSpeed < CONFIG_WALK_SPEED_INSTANT) {
                                ++gConfig.computerWalkSpeed;
                            } else {
                                gConfig.blackoutComputer = 1;
                            }
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case SYSTEM_OPTION_MUSIC_SOURCE:
                            if (gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI) {
                                if (!gpSoundManager->CDStartup()) {
                                    NormalDialog(
                                        "\xcd\xe5\xe2\xee\xe7\xec\xee\xe6\xed\xee \xf3\xf1\xf2\xe0"
                                            "\xed\xee\xe2\xe8\xf2\xfc \xe2\xee\xf1\xef\xf0\xee\xe8\xe7"
                                            "\xe2\xe5\xe4\xe5\xed\xe8\xe5 CD \xec\xf3\xe7\xfb\xea\xe8."
                                            " \xc2\xe0\xf8 \xef\xf0\xe8\xe2\xee\xe4 CD, \xe2\xee\xe7"
                                            "\xec\xee\xe6\xed\xee, \xe8\xf1\xef\xee\xeb\xfc\xe7\xf3"
                                            "\xe5\xf2\xf1\xff \xe4\xf0\xf3\xe3\xee\xe9 \xef\xf0\xee"
                                            "\xe3\xf0\xe0\xec\xec\xee\xe9 \xe8\xeb\xe8 \xe4\xf0\xe0"
                                            "\xe9\xe2\xe5\xf0 \xe7\xe2\xf3\xea\xe0 \xed\xe5 \xef\xee"
                                            "\xe4\xe4\xe5\xf0\xe6\xe8\xe2\xe0\xe5\xf2 \xe2\xee\xf1\xef"
                                            "\xf0\xee\xe8\xe7\xe2\xe5\xe4\xe5\xed\xe8\xe5 CD \xf1\xf2"
                                            "\xe5\xf0\xe5\xee \xe7\xe2\xf3\xea\xe0."
                                             ,
                                        OPTION_DIALOG_MESSAGE,
                                        OPTION_DIALOG_NONE,
                                        OPTION_DIALOG_NONE,
                                        OPTION_DIALOG_NONE,
                                        0,
                                        OPTION_DIALOG_NONE,
                                        0,
                                        OPTION_DIALOG_NONE,
                                        0
                                    );
                                    break;
                                }
                                gpSoundManager->SetMusicQuality(H2EnumIndex(CONFIG_MUSIC_SOURCE_CD));
                                gConfig.useOpera = CONFIG_OPERA_DISABLED;
                            } else if (gConfig.useOpera == CONFIG_OPERA_DISABLED) {
                                gConfig.useOpera = CONFIG_OPERA_ENABLED;
                            } else {
                                if (!MusicFlagsActive()) {
                                    gpSoundManager->StartupMilesBackend();
                                }
                                if (GetMusicFlagA() == 0) {
                                    gConfig.useOpera =
                                        static_cast<ConfigOperaMode>(1 - H2EnumIndex(gConfig.useOpera));
                                } else {
                                    gpSoundManager->SetMusicQuality(H2EnumIndex(CONFIG_MUSIC_SOURCE_MIDI));
                                }
                            }
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case SYSTEM_OPTION_SHOW_ROUTE:
                            gConfig.showRoute = 1 - gConfig.showRoute;
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case SYSTEM_OPTION_INTERFACE:
                            gConfig.evilInterfaceUsage =
                                (gConfig.evilInterfaceUsage + 1) % OPTION_INTERFACE_COUNT;
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case SYSTEM_OPTION_VIDEO:
                            gConfig.slowVideo = gConfig.slowVideo == 0;
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case SYSTEM_OPTION_COLOR_CURSOR:
                            gConfig.gfx[H2EnumIndex(CONFIG_EXECUTABLE_GAME)].colorMouseCursor =
                                1 - gConfig.gfx[H2EnumIndex(CONFIG_EXECUTABLE_GAME)].colorMouseCursor;
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            gpMouseManager->SetColorMice(
                                gConfig.gfx[H2EnumIndex(CONFIG_EXECUTABLE_GAME)].colorMouseCursor
                            );
                            break;
                        case SYSTEM_OPTION_COUNT:
                            break;
                    }
                    break;
                }
            }
        }
    }

    if (preferencesChanged) {
        UpdateSystemOptions(0);
    }
    if (accepted) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = H2EnumIndex(SYSTEM_OPTION_FIRST);
        message.payload.widget.command = static_cast<BaseWidgetCommand>(H2EnumIndex(SYSTEM_OPTION_FIRST));
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

i32 GetMobilityFrame(i32 mobility) {
    i32 frame = mobility * MOBILITY_SCALE / MOBILITY_DIVISOR;
    if (frame < 0) {
        frame = 0;
    }
    if (frame > MOBILITY_TOP_THRESHOLD) {
        frame = FRAME_TOP;
    } else if (frame > MOBILITY_HIGH_THRESHOLD) {
        frame = FRAME_HIGH;
    } else if (frame > MOBILITY_MID_THRESHOLD) {
        frame = FRAME_MID;
    }
    return frame;
}

i32 GetManaFrame(i32 mana) {
    i32 frame = mana / MANA_DIVISOR;
    if (frame == 0 && mana >= MANA_MIN_VISIBLE) {
        frame = 1;
    }
    if (frame > MANA_TOP_THRESHOLD) {
        frame = FRAME_TOP;
    } else if (frame > MANA_HIGH_THRESHOLD) {
        frame = FRAME_HIGH;
    } else if (frame > MANA_MID_THRESHOLD) {
        frame = FRAME_MID;
    }
    return frame;
}

i32 advManager::DoVisions(hero* visionHero) {
    CreatureType type;
    mapCell* spot;
    i32 tryX;
    i32 tryY;
    i32 dist;
    i32 bestY;
    i32 hitX;
    i32 count;
    float fRatio;
    i32 isForced;
    char msg[VISIONS_MESSAGE_BUFFER_SIZE];
    i32 nearDist;
    i32 joinNum;
    i32 joinFee;

    nearDist = VISIONS_NO_MONSTER_DISTANCE;
    bestY = -1;
    hitX = bestY;
    for (tryX = visionHero->m_x - VISIONS_RADIUS;
         tryX <= visionHero->m_x + VISIONS_RADIUS;
         ++tryX) {
        for (tryY = visionHero->m_y - VISIONS_RADIUS;
             tryY <= visionHero->m_y + VISIONS_RADIUS;
             ++tryY) {
            spot = GetCell(tryX, tryY);
            if (spot->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER)) {
                if ((dist = abs(visionHero->m_x - tryX) + abs(visionHero->m_y - tryY))
                    < nearDist) {
                    nearDist = dist;
                    hitX = tryX;
                    bestY = tryY;
                }
            }
        }
    }

    if (nearDist == VISIONS_NO_MONSTER_DISTANCE) {
        NormalDialog(
            "\xc2\xfb \xe4\xee\xeb\xe6\xed\xfb \xe1\xfb\xf2\xfc \xea\xe0\xea \xec\xe8\xed\xe8\xec\xf3\xec \xe2 \xf2\xf0\xe5\xf5 \xf8\xe0\xe3\xe0\xf5 \xee\xf2 \xec\xee\xed\xf1\xf2\xf0\xe0, \xf7\xf2\xee\xe1\xfb \xe8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xf2\xfc \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5 \xc2\xe8\xe4\xe5\xed\xe8\xff."
                 ,
            1,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
        return 0;
    }

    spot = GetCell(hitX, bestY);
    type = static_cast<CreatureType>(spot->m_objectIndex);
    isForced = spot->m_objectMetadata & MONSTER_JOIN_FORCED;
    count = spot->m_objectMetadata & MONSTER_COUNT_MASK;
    sprintf(
        gText,
        "{%d %s}\n\n",
        count,
        gArmyNamesPlural[H2EnumIndex(type)]
    );
    fRatio = static_cast<double>(
                 gpPhilAI->FightValueOfStack(&visionHero->m_army, visionHero, 0, 0, 0, 0)
             )
             / static_cast<double>(count * gMonsterDatabase[H2EnumIndex(type)].fightValue);

    if (visionHero->m_army.CanJoin(type) && fRatio > MONSTER_STRENGTH_JOIN
        && !visionHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) && type != CREATURE_GHOST
        && type != CREATURE_EARTH_ELEMENTAL && type != CREATURE_AIR_ELEMENTAL
        && type != CREATURE_FIRE_ELEMENTAL && type != CREATURE_WATER_ELEMENTAL) {
        if (isForced) {
            sprintf(
                msg,
                "\xd1\xf3\xf9\xe5\xf1\xf2\xe2\xe0 \xf5\xee\xf2\xff\xf2 \xe2\xf1\xf2\xf3\xef\xe8\xf2\xfc \xe2 \xe2\xe0\xf8\xf3 \xe0\xf0\xec\xe8\xfe!"

            );
            strcat(gText, msg);
            goto showVision;
        } else if (visionHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_DIPLOMACY)]
                   != HERO_SKILL_LEVEL_NONE) {
            if (visionHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_DIPLOMACY)]
                == HERO_SKILL_LEVEL_EXPERT) {
                joinNum = count;
            } else if (visionHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_DIPLOMACY)]
                       == HERO_SKILL_LEVEL_ADVANCED) {
                joinNum = count / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
            } else {
                joinNum = count / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
            }
            if (joinNum == 0) {
                joinNum = 1;
            }

            joinFee = gMonsterDatabase[H2EnumIndex(type)].cost * count;
            if (joinFee
                > gpGame->m_players[visionHero->m_owner].m_resources[H2EnumIndex(RES_GOLD)]) {
                if (fRatio > MONSTER_STRENGTH_FLEE) {
                    goto creaturesFlee;
                } else {
                    goto creaturesFight;
                }
            }

            if (joinNum == count) {
                sprintf(
                    msg,
                    "\xc2\xf1\xe5 \xf1\xf3\xf9\xe5\xf1\xf2\xe2\xe0 \xe2\xf1\xf2\xf3\xef\xff\xf2 \xe2 \xed\xe0\xf8\xf3 \xe0\xf0\xec\xe8\xfe...\n\n\xe7\xe0 \xef\xeb\xe0\xf2\xf3 \xe2 \xf0\xe0\xe7\xec\xe5\xf0\xe5 %d \xe7\xee\xeb\xee\xf2\xfb\xf5."

,
                    joinFee
                );
            } else {
                sprintf(
                    msg,
                    "%d \xf1\xf3\xf9\xe5\xf1\xf2\xe2 \xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xff\xf2\xf1\xff \xea \xed\xe0\xec...\n\n\xe7\xe0 \xef\xeb\xe0\xf2\xf3 \xe2 \xf0\xe0\xe7\xec\xe5\xf0\xe5 %d \xe7\xee\xeb\xee\xf2\xfb\xf5."

,
                    count,
                    joinFee
                );
            }
            strcat(gText, msg);
            goto showVision;
        }
    }

    if (fRatio > MONSTER_STRENGTH_FLEE) {
    creaturesFlee:
        sprintf(
            msg,
            "\xdd\xf2\xe8 \xf1\xeb\xe0\xe1\xfb\xe5 \xf2\xe2\xe0\xf0\xe8 \xee\xef\xf0\xe5\xe4\xe5\xeb\xe5\xed\xed\xee \xef\xee\xe1\xe5\xe3\xf3\xf2, \xe5\xe4\xe2\xe0 \xe7\xe0\xe2\xe8\xe4\xe5\xe2 \xed\xe0\xf1."

        );
        strcat(gText, msg);
        goto showVision;
    }
creaturesFight:
    sprintf(
        msg,
        "\xc1\xee\xfe\xf1\xfc, \xfd\xf2\xe8 \xf1\xee\xe7\xe4\xe0\xed\xe8\xff \xe2 \xee\xf2\xe2\xf0\xe0\xf2\xe8\xf2\xe5\xeb\xfc\xed\xee\xec \xed\xe0\xf1\xf2\xf0\xee\xe5\xed\xe8\xe8 \xe8 \xe1\xf3\xe4\xf3\xf2 \xf1\xf0\xe0\xe6\xe0\xf2\xfc\xf1\xff."

    );
    strcat(gText, msg);
    goto showVision;

showVision:
    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
    return 1;
}

i32 advManager::IsCrystalBallInEffect(i32 x, i32 y, i32 radius) {
    i32 i;
    hero* crystalHero;
    i32 distance;
    for (i = 0; i < gpCurPlayer->m_heroCount; ++i) {
        crystalHero = &gpGame->m_heroRecs[gpCurPlayer->m_heroIds[i]];
        if (crystalHero->HasArtifact(ARTIFACT_CRYSTAL_BALL)) {
            distance = static_cast<i32>(sqrt(
                static_cast<double>(
                    (crystalHero->m_x - x) * (crystalHero->m_x - x)
                    + (crystalHero->m_y - y) * (crystalHero->m_y - y)
                )
            ));
            if (distance <= radius) {
                return 1;
            }
        }
    }
    return 0;
}

u8 StopOnTrigger(class mapCell* cell) {
    MapObjectType type = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
    if (type != MAP_OBJECT_EXPANSION_OBJECT) {
        return bStopOnTrigger[H2EnumIndex(type)];
    }

    i32 special = cell->m_objectMetadata;
    special &= SPECIAL_TRIGGER_MASK;
    switch (special) {
        case TRIGGER_EVENT_5:
        case TRIGGER_EVENT_6:
            return 1;
    }
    return 0;
}

i32 giLimitUpdMinX = -1;
i32 iLastScrollTime = 0;
i32 iSandAnim = 0;
i32 giLastHourGlassUpdateTime = 0;
i32 TrigX = 0;
i32 TrigY = 0;
BottomViewMode iCurBottomView = BOTTOM_VIEW_NONE;
i32 iCurBottomViewEnemy = -1;
i32 iCurHourGlassPhase = 0;
i32 iLastHourGlassPhase = 1;
b32 gbForceUpdate = false;
i32 giCheatSeq = 0;
i32 iQWE = 0;
u8 monAnimDrawFrame[ADVMGR_MONSTER_ANIMATION_TABLE_SIZE] =
    {0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 3, 4, 5, 5, 4, 3, 0, 0};
i32 iLastSandAnimTime = 0;
i32 iLastNewSandAnimTime = 0;
i32 giFrameCount = 0;
