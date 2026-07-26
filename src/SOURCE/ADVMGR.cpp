#include <va.h>
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
#include <BASE/widgetKind.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
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
#include <SOURCE/kbwin.h>
#include <SOURCE/playerData.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/town.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

H2_ENUM_CLASS_BEGIN(AdventureSystemOptionsWidgetId)
    SYSTEM_OPTION_MUSIC_VOLUME   = 10,
    SYSTEM_OPTION_SOUND_VOLUME   = 11,
    SYSTEM_OPTION_HERO_SPEED     = 12,
    SYSTEM_OPTION_MUSIC_SOURCE   = 13,
    SYSTEM_OPTION_SHOW_ROUTE     = 14,
    SYSTEM_OPTION_COMPUTER_SPEED = 15,
    SYSTEM_OPTION_INTERFACE      = 16,
    SYSTEM_OPTION_VIDEO          = 17,
    SYSTEM_OPTION_COLOR_CURSOR   = 18,
    SYSTEM_OPTION_FIRST          = SYSTEM_OPTION_MUSIC_VOLUME,
    SYSTEM_OPTION_LAST           = SYSTEM_OPTION_COLOR_CURSOR,
    SYSTEM_OPTION_COUNT          = SYSTEM_OPTION_LAST - SYSTEM_OPTION_FIRST + 1,
    SYSTEM_OPTIONS_DIALOG_ACCEPT = 0x7800
H2_ENUM_CLASS_END(AdventureSystemOptionsWidgetId)

H2_ENUM_BEGIN(AdventureScreenConstant)
    SCREEN_WIDTH     = 640,
    SCREEN_HEIGHT    = 480,
    VIEWPORT_SIZE    = SCREEN_HEIGHT,
    VIEW_CELL_COUNT  = 15,
    SCROLL_BORDER    = 16,
    VIEW_EDGE_MARGIN = 8,
    VIEW_CENTER_CELL = VIEW_EDGE_MARGIN - 1,
    VIEW_LAST_CELL   = VIEW_CELL_COUNT - 1,
    RADAR_LEFT       = 480,
    RADAR_RIGHT      = 624,
    RADAR_TOP        = 16,
    RADAR_BOTTOM     = 160,
    PUZZLE_TILE_SIZE = 32,
    PUZZLE_X_TRIM    = 12
H2_ENUM_END(AdventureScreenConstant)

H2_ENUM_BEGIN(AdventureDrawConstant)
    CELL_PIXELS                    = 32,
    CELL_CENTER_PIXEL              = CELL_PIXELS / 2,
    CELL_LAST_PIXEL                = CELL_PIXELS - 1,
    OBJECT_BASELINE_Y              = CELL_PIXELS - 2,
    DRAW_VIEW_CELLS                = VIEW_CELL_COUNT,
    DRAW_LAST_CELL                 = VIEW_LAST_CELL,
    DRAW_CLIP_WIDTH                = VIEWPORT_SIZE,
    DRAW_CLIP_HEIGHT               = VIEWPORT_SIZE,
    STONE_TILE_NONE                = -1,
    STONE_TILE_TOP_LEFT            = 16,
    STONE_TILE_TOP_RIGHT           = 17,
    STONE_TILE_BOTTOM_RIGHT        = 18,
    STONE_TILE_BOTTOM_LEFT         = 19,
    STONE_TILE_TOP_BASE            = 20,
    STONE_TILE_RIGHT_BASE          = 24,
    STONE_TILE_BOTTOM_BASE         = 28,
    STONE_TILE_LEFT_BASE           = 32,
    STONE_PATTERN_COORDINATE_SHIFT = 16,
    CLOUD_VARIANTS                 = 4,
    CLOUD_VARIANT_MASK             = CLOUD_VARIANTS - 1,
    CLOUD_FLIPPED_FRAME_BASE       = 100,
    CLOUD_X_ALTERNATE_FRAME_1      = 1,
    CLOUD_Y_ALTERNATE_FRAME        = 3,
    CLOUD_X_ALTERNATE_FRAME_2      = 5,
    ROUTE_DRAW_X_OFFSET            = 12,
    ROUTE_DRAW_Y_OFFSET            = 2,
    ROUTE_FRAME_MASK               = 0xff,
    TERRAIN_FLAGS_SHIFT            = 14,
    MINE_GUARDIAN_ICON_SLOT        = 10,
    MINE_GHOST_FRAME_COUNT         = 15,
    MONSTER_FRAME_STRIDE           = 9,
    MONSTER_REVERSED_FRAME         = MONSTER_FRAME_STRIDE - 1,
    MONSTER_ANIMATION_FRAME_OFFSET = 1,
    MONSTER_SPECIAL_FRAME_FIRST    = 59,
    MONSTER_SPECIAL_FRAME_LAST     = 60,
    HERO_BOAT_Y_OFFSET             = -10,
    HERO_SHADOW_FRAME_FIRST        = 9,
    HERO_SHADOW_FRAME_END          = 36,
    HERO_SHADOW_FRAME_OFFSET       = 50,
    HERO_SHADOW_SOURCE_FRAME_46    = 46,
    HERO_SHADOW_SOURCE_FRAME_47    = 47,
    HERO_SHADOW_SOURCE_FRAME_49    = 49,
    HERO_SHADOW_SOURCE_FRAME_50    = 50,
    HERO_SHADOW_SOURCE_FRAME_51    = 51,
    HERO_SHADOW_TARGET_FRAME_55    = 55,
    HERO_SHADOW_TARGET_FRAME_56    = 56,
    HERO_SHADOW_TARGET_FRAME_57    = 57,
    HERO_SHADOW_TARGET_FRAME_58    = 58,
    PLAYER_FLAG_FRAME_COUNT        = 8,
    PLAYER_FLAG_FRAME_BASE         = 56,
    TILESET_MONSTER                = 20,
    TILESET_BOAT                   = 39
H2_ENUM_END(AdventureDrawConstant)

H2_ENUM_BEGIN(AdventureUpdateScreenConstant)
    UPDATE_NONE             = -1,
    UPDATE_VIEWPORT_ORIGIN  = 16,
    UPDATE_VIEWPORT_SIZE    = 448,
    UPDATE_ANIMATION_PHASES = 6,
    UPDATE_FRAME_CYCLE      = 18
H2_ENUM_END(AdventureUpdateScreenConstant)

H2_ENUM_BEGIN(AdventureAnimationPhaseIndex)
    ANIMATION_PHASE_COLUMN_0         = 0,
    ANIMATION_PHASE_COLUMN_1         = 1,
    ANIMATION_PHASE_COLUMN_2         = 2,
    ANIMATION_PHASE_COLUMN_3         = 3,
    ANIMATION_PHASE_COLUMN_MASK      = ADVMGR_ANIMATION_PHASE_COUNT - 1,
    ANIMATION_PHASE_COLUMN_0_INITIAL = 0,
    ANIMATION_PHASE_COLUMN_1_INITIAL = 4,
    ANIMATION_PHASE_COLUMN_2_INITIAL = 11,
    ANIMATION_PHASE_COLUMN_3_INITIAL = 13
H2_ENUM_END(AdventureAnimationPhaseIndex)

H2_ENUM_BEGIN(AdventureSourceLineConstant)
    CLOSE_BORDER_FREE_LINE_BASE         = 0x0124,
    BORDER_INITIAL_FREE_LINE_OFFSET     = 22,
    BORDER_SECONDARY_FREE_LINE_OFFSET   = 9,
    BORDER_ALLOC_LINE_OFFSET            = 4,
    NEW_TURN_LINE_BASE                  = 0x11d0,
    RESOURCE_VIEW_LINE_BASE             = 0x122f,
    KINGDOM_VIEW_LINE_BASE              = 0x1296,
    BOTTOM_HERO_LINE_BASE               = 0x12e3,
    BOTTOM_HERO_ALLOC_LINE_OFFSET       = 0x44,
    QUICK_VIEW_LINE_BASE                = 0x1376,
    QUICK_VIEW_FIRST_ALLOC_LINE_OFFSET  = 0x9b,
    QUICK_VIEW_SECOND_ALLOC_LINE_OFFSET = 0xe3,
    QUICK_VIEW_THIRD_ALLOC_LINE_OFFSET  = 0x10e,
    TOWN_VIEW_LINE_BASE                 = 0x14e2,
    TOWN_VIEW_FIRST_ALLOC_LINE_OFFSET   = 0x83,
    TOWN_VIEW_SECOND_ALLOC_LINE_OFFSET  = 0xd6,
    TOWN_VIEW_THIRD_ALLOC_LINE_OFFSET   = 0x108,
    REDRAW_BORDER_FREE_LINE_BASE        = 0x1628,
    SAVE_BORDER_ALLOC_LINE_BASE         = 0x2025
H2_ENUM_END(AdventureSourceLineConstant)

H2_ENUM_BEGIN(AdventureBorderConstant)
    BORDER_BUFFER_SIZE      = 0x7400,
    BORDER_EDGE_SIZE        = 16,
    BORDER_VIEW_SIZE        = VIEWPORT_SIZE,
    BORDER_MIDDLE_END       = BORDER_VIEW_SIZE - BORDER_EDGE_SIZE,
    BORDER_ROW_BYTES        = BORDER_VIEW_SIZE,
    BORDER_SCREEN_PITCH     = SCREEN_WIDTH,
    BORDER_SIDE_BYTES       = BORDER_EDGE_SIZE,
    BORDER_SAVED_SIDE_BYTES = BORDER_SIDE_BYTES * 2
H2_ENUM_END(AdventureBorderConstant)

H2_ENUM_BEGIN(AdventureScrollConstant)
    SCROLL_MIN_ORIGIN    = 1 - VIEW_EDGE_MARGIN,
    VIEW_CENTER_OFFSET   = VIEW_CENTER_CELL,
    SCROLL_TICK_INTERVAL = 70
H2_ENUM_END(AdventureScrollConstant)

H2_ENUM_BEGIN(AdventureButtonConstant)
    BUTTON_FIRST           = 1,
    BUTTON_LAST            = 6,
    BUTTON_TARGET          = 2,
    BUTTON_SLOT_1          = 1,
    BUTTON_SLOT_2          = 2,
    BUTTON_SLOT_3          = 3,
    BUTTON_SLOT_4          = 4,
    BUTTON_SLOT_5          = 5,
    BUTTON_SLOT_6          = 6,
    BUTTON_BROADCAST_ARG   = 1,
    BUTTON_BROADCAST_FLAGS = 0x4008
H2_ENUM_END(AdventureButtonConstant)

H2_ENUM_BEGIN(AdventureTriggerConstant)
    SPECIAL_TRIGGER_MASK = 0x3f,
    TRIGGER_EVENT_5      = 5,
    TRIGGER_EVENT_6      = 6
H2_ENUM_END(AdventureTriggerConstant)

H2_ENUM_BEGIN(AdventureHoverConstant)
    HOVER_VIEW_CELLS           = VIEW_CELL_COUNT,
    HOVER_MAX_CELL             = VIEW_LAST_CELL,
    HOVER_OBJECT_BLOCKED       = 0x08,
    HOVER_UNREACHABLE          = 0x80,
    HOVER_ROUTE_DAY_LIMIT      = 3,
    HOVER_ROUTE_FRAMES_PER_DAY = 6,
    HOVER_SCROLL_FRAME_FIRST   = 32,
    HOVER_SCROLL_FRAME_END     = 40
H2_ENUM_END(AdventureHoverConstant)

H2_ENUM_BEGIN(AdventurePointerFrame)
    POINTER_DEFAULT      = 0,
    POINTER_HERO         = 2,
    POINTER_TOWN         = 3,
    POINTER_MOVE         = 4,
    POINTER_ATTACK       = 5,
    POINTER_SAIL         = 6,
    POINTER_DISEMBARK    = 7,
    POINTER_SELECT_HERO  = 8,
    POINTER_ACTION       = 9,
    POINTER_WATER_ACTION = 28
H2_ENUM_END(AdventurePointerFrame)

H2_ENUM_BEGIN(AdventureFrameConstant)
    MOBILITY_SCALE          = 22,
    MOBILITY_DIVISOR        = 1500,
    MANA_DIVISOR            = 5,
    MANA_MIN_VISIBLE        = 3,
    MOBILITY_TOP_THRESHOLD  = 30,
    MOBILITY_HIGH_THRESHOLD = 26,
    MOBILITY_MID_THRESHOLD  = 23,
    MANA_TOP_THRESHOLD      = 33,
    MANA_HIGH_THRESHOLD     = 28,
    MANA_MID_THRESHOLD      = 23,
    FRAME_TOP               = 25,
    FRAME_HIGH              = 24,
    FRAME_MID               = 23
H2_ENUM_END(AdventureFrameConstant)

H2_ENUM_BEGIN(AdventureStateConstant)
    INVALID_HERO               = -1,
    LOOPING_SOUND_LIMIT        = 4,
    OBJECT_ICON_COUNT          = ADVMGR_OBJECT_ICON_COUNT,
    HERO_ICON_COUNT            = ADVMGR_HERO_ICON_COUNT,
    LOOPING_SAMPLE_COUNT       = IDX(ADVMGR_ENVIRONMENT_SOUND_COUNT),
    CURSOR_SAMPLE_COUNT        = ADVMGR_CURSOR_SAMPLE_COUNT,
    SOUND_CELL_COUNT           = ADVMGR_ACTIVE_SOUND_COUNT,
    HIGH_MEMORY_BUFFER_DIVISOR = 100,
    CURSOR_SAMPLE_VOLUME       = 64,
    CURSOR_SAMPLE_CHANNEL      = 2,
    CURSOR_SAMPLE_FAST_SET     = 2,
    HERO_ICON_FROTH            = IDX(HERO_TYPE_BOAT) + 1,
    ADVENTURE_FADE_STEPS       = 8,
    FORCED_MUSIC_DELAY         = 6000,
    QUICK_VIEW_NONE            = -99,
    UNUSED_OBJECT_ICON_1       = 21,
    UNUSED_OBJECT_ICON_2       = 38
H2_ENUM_END(AdventureStateConstant)

H2_ENUM_BEGIN(AdventureMainConstant)
    ADVENTURE_EVENT_CELL_RESULT_COUNT = 3
H2_ENUM_END(AdventureMainConstant)

H2_ENUM_BEGIN(AdventureEnvironmentSoundConstant)
    ENVIRONMENT_ORIGIN_NONE          = -1,
    ENVIRONMENT_SOUND_LOG_UNUSED     = -999,
    ENVIRONMENT_SOUND_DEFAULT_VOLUME = 127,
    ENVIRONMENT_SOUND_MAX_DISTANCE   = 5,
    ENVIRONMENT_VOLUME_COUNT         = 8,
    ENVIRONMENT_SOUND_FIRST_LAYER    = 1,
    ENVIRONMENT_SOUND_LAYER_COUNT    = 2,
    ENVIRONMENT_SOUND_CHANNEL_TYPE   = 3,
    ENVIRONMENT_SOUND_EDGE_SPAN      = 2
H2_ENUM_END(AdventureEnvironmentSoundConstant)

H2_ENUM_BEGIN(AdventureTeleportConstant)
    TELEPORT_MAP_CHANGE_VALUE         = -999,
    TELEPORT_CELL_OBJECT_FLAG         = 0x40,
    TELEPORT_RESTORE_MODE             = 99,
    TELEPORT_VIEW_CENTER              = VIEW_CENTER_CELL,
    TELEPORT_DELAY                    = 90,
    TELEPORT_FIZZLE_TIME              = 128,
    TELEPORT_REMOTE_FIZZLE_ADJUSTMENT = 64
H2_ENUM_END(AdventureTeleportConstant)

H2_ENUM_BEGIN(AdventureTravelSpellConstant)
    TRAVEL_DIALOG_ACCEPT        = 1,
    TRAVEL_MUSIC                = 1,
    DIMENSION_DOOR_FIRST_BUTTON = 10,
    DIMENSION_DOOR_LAST_BUTTON  = 11,
    DIMENSION_DOOR_CLOSE_BUTTON = 0x7800,
    TOWN_PORTAL_WINDOW_X        = 159,
    TOWN_PORTAL_WINDOW_Y        = 65,
    TOWN_PORTAL_DISTANCE_LIMIT  = 1000
H2_ENUM_END(AdventureTravelSpellConstant)

H2_ENUM_BEGIN(AdventureTownPortalMessage)
    TOWN_PORTAL_TITLE_WIDGET  = 1,
    TOWN_PORTAL_CHOICE_WIDGET = 100,
    TOWN_PORTAL_CLOSE_WIDGET  = 10,
    TOWN_PORTAL_FIRST_CHOICE  = 0x7801,
    TOWN_PORTAL_LAST_CHOICE   = 0x7802,
H2_ENUM_END(AdventureTownPortalMessage)

H2_ENUM_BEGIN(AdventureSoundTilesetConstant)
    SOUND_GROUND_SHAPE_MASK     = 0x0f,
    SOUND_SEAGULL_FRAME_FIRST   = 0xb6,
    SOUND_SEAGULL_FRAME_LAST    = 0xb7,
    SOUND_ALCHEMIST_FRAME_FIRST = 0x27,
    SOUND_ALCHEMIST_FRAME_LAST  = 0x2c
H2_ENUM_END(AdventureSoundTilesetConstant)

H2_ENUM_BEGIN(AdventureOpenConstant)
    SCROLL_Y          = 195,
    SCROLL_LEFT_X     = 540,
    SCROLL_RIGHT_X    = 612,
    SCROLL_WIDTH      = 8,
    SCROLL_HEIGHT     = 17,
    SCROLL_ICON_FRAME = 4,
    SCROLL_LEFT_ID    = 26,
    SCROLL_RIGHT_ID   = 27,
    TIMER_DELAY       = 120,
    MANAGER_MESSAGE   = 0x400
H2_ENUM_END(AdventureOpenConstant)

H2_ENUM_BEGIN(AdventureInterfaceConstant)
    INTERFACE_AUTO              = 0,
    INTERFACE_GOOD              = 1,
    INTERFACE_EVIL              = 2,
    INTERFACE_TRANSLATION_COUNT = 37
H2_ENUM_END(AdventureInterfaceConstant)

H2_ENUM_BEGIN(AdventureSystemOptionConstant)
    OPTION_INTERFACE_COUNT = 3,
    OPTION_DIALOG_MESSAGE  = 1,
    OPTION_DIALOG_HELP     = 4,
    OPTION_DIALOG_NONE     = -1
H2_ENUM_END(AdventureSystemOptionConstant)

H2_ENUM_BEGIN(AdventureSummonBoatConstant)
    SUMMON_CENTER_OFFSET   = VIEW_CENTER_CELL,
    SUMMON_DIRECTION_COUNT = 8,
    SUMMON_BOAT_COUNT      = 48,
    SUMMON_OCCUPIED_FLAG   = 0x80,
    SUMMON_MIN_DISTANCE    = 2,
    SUMMON_RESTORE_MODE    = 5,
    SUMMON_SCREEN_MARGIN   = 16,
    SUMMON_SCREEN_LIMIT    = 464,
    SUMMON_FIZZLE_X_OFFSET = 48,
    SUMMON_FIZZLE_WIDTH    = 112,
    SUMMON_FIZZLE_HEIGHT   = 80,
    SUMMON_TARGET_X        = 154,
    SUMMON_TARGET_Y        = 152,
    SUMMON_TARGET_WIDTH    = 150,
    SUMMON_TARGET_HEIGHT   = 136,
    SUMMON_BOAT_TRIGGER    = 0xab
H2_ENUM_END(AdventureSummonBoatConstant)

H2_ENUM_BEGIN(AdventureRouteConstant)
    ROUTE_PATH_COST_LIMIT       = 59999,
    ROUTE_TERRAIN_COST_INFINITY = 999999999,
    ROUTE_DAY_MASK              = 0x100,
    ROUTE_ARROW_FRAME_STRIDE    = 24,
    ROUTE_ARROW_FRAME_OFFSET    = 2,
    ROUTE_STRAIGHT_COST_0       = 75,
    ROUTE_STRAIGHT_COST_1       = 100,
    ROUTE_STRAIGHT_COST_2       = 125,
    ROUTE_STRAIGHT_COST_3       = 150,
    ROUTE_STRAIGHT_COST_4       = 175,
    ROUTE_STRAIGHT_COST_5       = 200,
    ROUTE_DIAGONAL_COST_0       = 112,
    ROUTE_DIAGONAL_COST_1       = 150,
    ROUTE_DIAGONAL_COST_2       = 187,
    ROUTE_DIAGONAL_COST_3       = 225,
    ROUTE_DIAGONAL_COST_4       = 262,
    ROUTE_DIAGONAL_COST_5       = 300
H2_ENUM_END(AdventureRouteConstant)

H2_ENUM_BEGIN(AdventureRouteFrame)
    ROUTE_FRAME_COST_0 = 0,
    ROUTE_FRAME_COST_1 = 1,
    ROUTE_FRAME_COST_2 = 2,
    ROUTE_FRAME_COST_3 = 3,
    ROUTE_FRAME_COST_4 = 4,
    ROUTE_FRAME_COST_5 = 5
H2_ENUM_END(AdventureRouteFrame)

H2_ENUM_BEGIN(AdventureSearchConstant)
    ARTIFACT_CAPACITY       = HERO_ARTIFACT_SLOT_COUNT,
    DIG_HOLE_FRAME          = 140,
    ULTIMATE_ARTIFACT_MUSIC = 41
H2_ENUM_END(AdventureSearchConstant)

H2_ENUM_BEGIN(AdventureComboDrawConstant)
    COMBO_VIEW_CELLS          = VIEW_CELL_COUNT,
    COMBO_GRID_CELLS          = 18,
    COMBO_CLEAR_BYTES         = 256,
    COMBO_CLOUD_MARK          = 10,
    COMBO_FRAME_LIMIT         = 12,
    COMBO_HERO_PANEL_LEFT     = 5,
    COMBO_HERO_PANEL_RIGHT    = 10,
    COMBO_HERO_PANEL_TOP      = 6,
    COMBO_HERO_PANEL_BOTTOM   = 9,
    COMBO_UPDATE_MIN          = 16,
    COMBO_UPDATE_MAX          = 463,
    COMBO_FAR_NEIGHBOR_OFFSET = 2,
H2_ENUM_END(AdventureComboDrawConstant)

H2_ENUM_BEGIN(AdventureRadarConstant)
    RADAR_SIZE                    = MAP_DIMENSION_XLARGE,
    RADAR_SCREEN_PITCH            = SCREEN_WIDTH,
    RADAR_ROW_GROUPS              = 5,
    RADAR_GROUP_BYTES             = 128,
    RADAR_UNSEEN_COLOR            = 0x24,
    RADAR_VIEWPORT_COLOR          = 181,
    RADAR_NEUTRAL_OWNER           = GAME_PLAYER_COUNT,
    RADAR_TERRAIN_SHADE           = 3,
    RADAR_PARTIAL_MARGIN          = 2,
    RADAR_PARTIAL_SPAN            = 16,
    RADAR_CURRENT_CELL            = VIEW_CENTER_CELL,
    RADAR_TOWN_RADIUS             = 2,
    RADAR_SMALL_CELL_PIXELS       = 4,
    RADAR_MEDIUM_CELL_PIXELS      = 2,
    RADAR_LARGE_SCALE_DIVISOR     = 3,
    RADAR_LARGE_SCALE_ROUNDING    = RADAR_LARGE_SCALE_DIVISOR - 1,
    RADAR_FRAME_NONE              = -1,
    RADAR_FRAME_NORMAL_XLARGE     = 1,
    RADAR_FRAME_NORMAL_LARGE      = 2,
    RADAR_FRAME_NORMAL_MEDIUM     = 3,
    RADAR_FRAME_VIEW_NEAR_XLARGE  = 4,
    RADAR_FRAME_NORMAL_SMALL      = 5,
    RADAR_FRAME_VIEW_MIDDLE       = 6,
    RADAR_FRAME_VIEW_FAR_XLARGE   = 7,
    RADAR_FRAME_VIEW_NEAR_LARGE   = 8,
    RADAR_FRAME_VIEW_MIDDLE_LARGE = 9,
H2_ENUM_END(AdventureRadarConstant)

H2_ENUM_BEGIN(AdventureArmySizeThreshold)
    ARMY_FEW_LIMIT     = 5,
    ARMY_SEVERAL_LIMIT = 10,
    ARMY_PACK_LIMIT    = 20,
    ARMY_LOTS_LIMIT    = 50,
    ARMY_HORDE_LIMIT   = 100,
    ARMY_THRONG_LIMIT  = 250,
    ARMY_SWARM_LIMIT   = 500,
    ARMY_ZOUNDS_LIMIT  = 1000
H2_ENUM_END(AdventureArmySizeThreshold)

H2_ENUM_BEGIN(AdventureArmySizeBand)
    ARMY_SIZE_FEW     = 0,
    ARMY_SIZE_SEVERAL = 1,
    ARMY_SIZE_PACK    = 2,
    ARMY_SIZE_LOTS    = 3,
    ARMY_SIZE_HORDE   = 4,
    ARMY_SIZE_THRONG  = 5,
    ARMY_SIZE_SWARM   = 6,
    ARMY_SIZE_ZOUNDS  = 7,
    ARMY_SIZE_LEGION  = 8
H2_ENUM_END(AdventureArmySizeBand)

H2_ENUM_BEGIN(AdventureArmyQuickViewConstant)
    ARMY_QUICK_SLOT_COUNT            = ARMY_GROUP_SLOT_COUNT,
    ARMY_QUICK_ICON_SIZE             = 32,
    ARMY_QUICK_ICON_BASELINE         = 30,
    ARMY_QUICK_AREA_LEFT             = 22,
    ARMY_QUICK_CENTER_DIVISOR        = 2,
    ARMY_QUICK_LABEL_HEIGHT          = 12,
    ARMY_QUICK_FIRST_ROW_SHIFT       = 22,
    ARMY_QUICK_SECOND_ROW_SHIFT      = 44,
    ARMY_QUICK_SECOND_ROW_ICON_SHIFT = 6,
    ARMY_QUICK_FIRST_ROW_COUNT       = 2,
    ARMY_QUICK_TOP_ROW_MAX           = 3,
    ARMY_QUICK_FOUR_STACK_COUNT      = 4,
    ARMY_QUICK_FIVE_STACK_COUNT      = ARMY_GROUP_SLOT_COUNT,
    ARMY_QUICK_FIVE_STACK_X_SHIFT    = 12,
    ARMY_QUICK_EMPTY_SLOT            = -1,
    ARMY_QUICK_TEXT_WIDTH            = 60,
    ARMY_QUICK_TEXT_X_ADJUSTMENT     = 14
H2_ENUM_END(AdventureArmyQuickViewConstant)

H2_ENUM_BEGIN(AdventureHeroQuickViewConstant)
    HERO_QUICK_ARMY_AREA_WIDTH       = 160,
    HERO_QUICK_DETAILED_CREATURE_Y   = 124,
    HERO_QUICK_VAGUE_FIRST_ROW_Y     = 73,
    HERO_QUICK_DEFAULT_WINDOW_X      = 288,
    HERO_QUICK_LOCATOR_ROW_HEIGHT    = 30,
    HERO_QUICK_LOCATOR_BASE_Y        = 97,
    HERO_QUICK_WINDOW_TEXT           = 18,
    HERO_QUICK_NAME_WIDGET           = 1,
    HERO_QUICK_PORTRAIT_WIDGET       = 2,
    HERO_QUICK_PRIMARY_STAT_WIDGET   = 3,
    HERO_QUICK_MANA_WIDGET           = 7,
    HERO_QUICK_PLAYER_COLOR_WIDGET   = 8,
    HERO_QUICK_PLAYER_COLOR_STRIDE   = 2,
    HERO_QUICK_ARMY_LABEL_CAPACITY   = 5,
    HERO_QUICK_SECOND_ROW_FIRST_SLOT = 2,
    HERO_QUICK_SECOND_ROW_TEXT_SHIFT = 38
H2_ENUM_END(AdventureHeroQuickViewConstant)

H2_ENUM_BEGIN(AdventureTownQuickViewConstant)
    TOWN_QUICK_ARMY_AREA_WIDTH       = 192,
    TOWN_QUICK_FIRST_ROW_Y           = 76,
    TOWN_QUICK_DEFAULT_WINDOW_X      = 328,
    TOWN_QUICK_DEFAULT_WINDOW_Y      = 176,
    TOWN_QUICK_WINDOW_TEXT           = 19,
    TOWN_QUICK_NAME_WIDGET           = 1,
    TOWN_QUICK_PORTRAIT_WIDGET       = 2,
    TOWN_QUICK_PLAYER_COLOR_WIDGET   = 8,
    TOWN_QUICK_TYPE_FRAME_BASE       = 9,
    TOWN_QUICK_VILLAGE_FRAME_OFFSET  = 6,
    TOWN_QUICK_KNOWN_MARKER_WIDGET   = 300,
    TOWN_QUICK_EMPTY_LABEL_CAPACITY  = 20,
    TOWN_QUICK_EMPTY_LABEL_X         = 13,
    TOWN_QUICK_EMPTY_LABEL_Y         = 117,
    TOWN_QUICK_EMPTY_LABEL_WIDTH     = 211,
    TOWN_QUICK_ARMY_LABEL_CAPACITY   = 15,
    TOWN_QUICK_INFORMATION_UNKNOWN   = 0,
    TOWN_QUICK_INFORMATION_NAMES     = 1,
    TOWN_QUICK_INFORMATION_ESTIMATES = 2,
    TOWN_QUICK_INFORMATION_EXACT     = 3,
    TOWN_QUICK_DEBUG_INFORMATION     = 2
H2_ENUM_END(AdventureTownQuickViewConstant)

H2_ENUM_BEGIN(AdventureBottomHeroViewConstant)
    BOTTOM_HERO_ARMY_SLOTS                = ARMY_GROUP_SLOT_COUNT,
    BOTTOM_HERO_EMPTY_SLOT                = -1,
    BOTTOM_HERO_LABEL_BYTES               = 6,
    BOTTOM_HERO_MAX_FULL_COUNT            = 1999,
    BOTTOM_HERO_COUNT_DIVISOR             = 1000,
    BOTTOM_HERO_PANEL_X                   = 480,
    BOTTOM_HERO_PANEL_Y                   = 392,
    BOTTOM_HERO_PANEL_WIDTH               = 143,
    BOTTOM_HERO_PANEL_HEIGHT              = 71,
    BOTTOM_HERO_ICON_WIDTH                = 32,
    BOTTOM_HERO_ICON_HEIGHT               = 28,
    BOTTOM_HERO_LABEL_HEIGHT              = 12,
    BOTTOM_HERO_LABEL_Y_OFFSET            = 25,
    BOTTOM_HERO_TOP_LABEL_SHIFT           = 2,
    BOTTOM_HERO_TOP_MIN_HEIGHT            = 35,
    BOTTOM_HERO_LOWER_MIN_HEIGHT          = 37,
    BOTTOM_HERO_GROUP_WIDTH               = 45,
    BOTTOM_HERO_CHARACTER_WIDTH           = 5,
    BOTTOM_HERO_ABBREVIATED_LABEL_PADDING = 4,
    BOTTOM_HERO_POSITION_COMPONENT_COUNT  = 2,
    BOTTOM_HERO_LAYOUT_SLOT_COUNT         = 8,
    BOTTOM_HERO_ICON_POSITION_BYTES       = BOTTOM_HERO_LAYOUT_SLOT_COUNT
        * BOTTOM_HERO_POSITION_COMPONENT_COUNT,
    BOTTOM_HERO_CREATURE_BOUND_COUNT      = 2,
    BOTTOM_HERO_FIRST_ICON_ID             = 2002,
    BOTTOM_HERO_FIRST_TEXT_ID             = 2101,
H2_ENUM_END(AdventureBottomHeroViewConstant)

H2_ENUM_BEGIN(AdventureLocatorConstant)
    LOCATOR_VISIBLE_COUNT             = 4,
    LOCATOR_PAGE_THRESHOLD            = 5,
    LOCATOR_PAGE_DENOMINATOR_OFFSET   = 4,
    LOCATOR_HERO_WIDGET_BASE          = 100,
    LOCATOR_HERO_WIDGET_STRIDE        = 7,
    LOCATOR_HERO_MOBILITY_OFFSET      = 1,
    LOCATOR_HERO_PORTRAIT_OFFSET      = 2,
    LOCATOR_HERO_MANA_OFFSET          = 3,
    LOCATOR_HERO_IMAGE_OFFSET         = 5,
    LOCATOR_HERO_BORDER_OFFSET        = 6,
    LOCATOR_HERO_SELECT_BASE          = LOCATOR_HERO_WIDGET_BASE + LOCATOR_HERO_IMAGE_OFFSET,
    LOCATOR_HERO_SELECT_1             = LOCATOR_HERO_SELECT_BASE + LOCATOR_HERO_WIDGET_STRIDE,
    LOCATOR_HERO_SELECT_2             = LOCATOR_HERO_SELECT_1 + LOCATOR_HERO_WIDGET_STRIDE,
    LOCATOR_HERO_SELECT_3             = LOCATOR_HERO_SELECT_2 + LOCATOR_HERO_WIDGET_STRIDE,
    LOCATOR_HERO_SCROLL_CONTROL       = 22,
    LOCATOR_TOWN_SCROLL_CONTROL       = 25,
    LOCATOR_HERO_KNOB_CONTROL         = 26,
    LOCATOR_TOWN_KNOB_CONTROL         = 27,
    LOCATOR_TOWN_IMAGE_BASE           = 16,
    LOCATOR_TOWN_IMAGE_1              = LOCATOR_TOWN_IMAGE_BASE + 1,
    LOCATOR_TOWN_IMAGE_2              = LOCATOR_TOWN_IMAGE_1 + 1,
    LOCATOR_TOWN_IMAGE_3              = LOCATOR_TOWN_IMAGE_2 + 1,
    LOCATOR_TOWN_BORDER_BASE          = 32,
    LOCATOR_TOWN_FLAG_BASE            = 300,
    LOCATOR_SELECTED_COLOR            = 0x99,
    LOCATOR_NORMAL_COLOR              = 0x24,
    LOCATOR_HERO_EMPTY_FRAME_BASE     = 1,
    LOCATOR_HERO_DEFAULT_FRAME        = 21,
    LOCATOR_HERO_ENABLE_FLAGS         = 6,
    LOCATOR_HERO_DISABLE_FLAGS        = 4,
    LOCATOR_TOWN_EMPTY_FRAME_BASE     = 5,
    LOCATOR_TOWN_ENABLE_FLAGS         = 2,
    LOCATOR_TOWN_DISABLE_FLAGS        = 4,
    LOCATOR_TOWN_TYPE_FRAME_BASE      = 9,
    LOCATOR_TOWN_VILLAGE_FRAME_OFFSET = 6,
    LOCATOR_SCROLL_NO_PAGES_Y         = 232,
    LOCATOR_SCROLL_BASE_Y             = 195,
    LOCATOR_SCROLL_MOUSE_BASE_Y       = LOCATOR_SCROLL_BASE_Y - 1,
    LOCATOR_SCROLL_MOUSE_SPAN         = 92,
    LOCATOR_HERO_SCROLL_SPAN          = 73,
    LOCATOR_TOWN_SCROLL_SPAN          = 74,
    LOCATOR_KNOB_MAX_Y                = LOCATOR_SCROLL_BASE_Y + LOCATOR_HERO_SCROLL_SPAN,
    LOCATOR_HERO_DRAW_LEFT            = 0,
    LOCATOR_SCREEN_LEFT               = 481,
    LOCATOR_SCREEN_TOP                = 177,
    LOCATOR_SCREEN_ROW_HEIGHT         = 32,
    LOCATOR_SCREEN_WIDTH              = 54,
    LOCATOR_SCREEN_HEIGHT             = 30
H2_ENUM_END(AdventureLocatorConstant)

H2_ENUM_BEGIN(AdventureQuickViewPlacementConstant)
    QUICK_VIEW_MIN_X         = 30,
    QUICK_VIEW_MIN_Y         = 16,
    QUICK_VIEW_RIGHT         = 448,
    QUICK_VIEW_BOTTOM        = 464,
    HERO_QUICK_VIEW_X_OFFSET = 73,
    HERO_QUICK_VIEW_Y_OFFSET = 72,
    HERO_QUICK_VIEW_WIDTH    = 192,
    HERO_QUICK_VIEW_HEIGHT   = 190,
    HERO_QUICK_VIEW_RIGHT_X  = 256,
    HERO_QUICK_VIEW_BOTTOM_Y = 274,
    TOWN_QUICK_VIEW_X_OFFSET = 89,
    TOWN_QUICK_VIEW_Y_OFFSET = 70,
    TOWN_QUICK_VIEW_WIDTH    = 224,
    TOWN_QUICK_VIEW_HEIGHT   = 186,
    TOWN_QUICK_VIEW_RIGHT_X  = 224,
    TOWN_QUICK_VIEW_BOTTOM_Y = 278
H2_ENUM_END(AdventureQuickViewPlacementConstant)

H2_ENUM_BEGIN(AdventureSpellConstant)
    TRAVEL_SPELL_MOBILITY_COST = 225,
    MINE_GUARDIAN_MAX_POWER    = 51,
    MINE_GUARDIANS_PER_POWER   = 4,
    MINE_GUARDIAN_VALUE_COUNT  = 2,
    MINE_GUARDIAN_TYPE_INDEX   = 0
H2_ENUM_END(AdventureSpellConstant)

H2_ENUM_BEGIN(AdventureSaveConstant)
    SAVE_PLAYER_COUNT   = GAME_PLAYER_COUNT,
    SAVE_EXTENSION_SIZE = 8,
    SAVE_PATTERN_SIZE   = 16,
    SAVE_REQUESTER_X    = 131,
    SAVE_REQUESTER_Y    = 58,
    SAVE_POINTER_FRAME  = 0
H2_ENUM_END(AdventureSaveConstant)

H2_ENUM_BEGIN(AdventureBottomViewConstant)
    BOTTOM_VIEW_PANEL_X           = 480,
    BOTTOM_VIEW_PANEL_Y           = 392,
    BOTTOM_VIEW_PANEL_WIDTH       = 143,
    BOTTOM_VIEW_BACKGROUND_WIDTH  = 159,
    BOTTOM_VIEW_PANEL_HEIGHT      = 71,
    BOTTOM_VIEW_BACKGROUND_ID     = 2000,
    BOTTOM_VIEW_FOREGROUND_ID     = 2001,
    BOTTOM_VIEW_TEXT_ID           = 2100,
    BOTTOM_VIEW_TEXT_ID_2         = 2101,
    BOTTOM_VIEW_DRAW_LEFT         = 0,
    BOTTOM_VIEW_DRAW_TOP          = 2000,
    BOTTOM_VIEW_DRAW_BOTTOM       = 2200,
    BOTTOM_VIEW_TEXT_BUFFER_SIZE  = 30,
    BOTTOM_VIEW_COUNT_BUFFER_SIZE = 8,
    BOTTOM_VIEW_WIDGET_CAPACITY   = 12,
    BOTTOM_VIEW_CENTER_DIVISOR    = 2,
    BOTTOM_VIEW_NO_ENEMY          = -1,
    BOTTOM_VIEW_NO_ANIMATION      = -1
H2_ENUM_END(AdventureBottomViewConstant)

H2_ENUM_BEGIN(AdventureNewTurnViewConstant)
    NEW_TURN_DATE_ICON_X            = 480,
    NEW_TURN_DATE_ICON_Y            = 392,
    NEW_TURN_DATE_ICON_WIDTH        = 143,
    NEW_TURN_DATE_ICON_HEIGHT       = 71,
    NEW_TURN_DATE_TEXT_X            = 479,
    NEW_TURN_WEEK_TEXT_Y            = 424,
    NEW_TURN_DAY_TEXT_Y             = 438,
    NEW_TURN_DATE_TEXT_WIDTH        = 145,
    NEW_TURN_WEEK_TEXT_HEIGHT       = 12,
    NEW_TURN_DAY_TEXT_HEIGHT        = 25,
    NEW_TURN_FIRST_DAY              = 1,
    NEW_TURN_WEEK_ALLOC_LINE_OFFSET = 0x29,
    NEW_TURN_DAY_ALLOC_LINE_OFFSET  = 0x43
H2_ENUM_END(AdventureNewTurnViewConstant)

H2_ENUM_BEGIN(AdventureResourceViewConstant)
    RESOURCE_VIEW_TEXT_BASE_Y               = 398,
    RESOURCE_VIEW_MULTILINE_HEIGHT          = 32,
    RESOURCE_VIEW_LINE_HEIGHT               = 6,
    RESOURCE_VIEW_TEXT_HEIGHT               = 36,
    RESOURCE_VIEW_GOLD_WIDTH                = 76,
    RESOURCE_VIEW_GOLD_HEIGHT               = 26,
    RESOURCE_VIEW_ICON_WIDTH                = 38,
    RESOURCE_VIEW_ICON_HEIGHT               = 32,
    RESOURCE_VIEW_ICON_BOTTOM               = 463,
    RESOURCE_VIEW_ICON_BOTTOM_PADDING       = 14,
    RESOURCE_VIEW_COUNT_X                   = 511,
    RESOURCE_VIEW_COUNT_Y                   = 453,
    RESOURCE_VIEW_COUNT_WIDTH               = 80,
    RESOURCE_VIEW_COUNT_HEIGHT              = 12,
    RESOURCE_VIEW_MESSAGE_ALLOC_LINE_OFFSET = 0x26,
    RESOURCE_VIEW_COUNT_ALLOC_LINE_OFFSET   = 0x50
H2_ENUM_END(AdventureResourceViewConstant)

H2_ENUM_BEGIN(AdventureKingdomViewConstant)
    KINGDOM_VIEW_RESOURCE_COUNT          = IDX(RES_COUNT),
    KINGDOM_VIEW_ENTRY_COUNT             = 9,
    KINGDOM_VIEW_CASTLE_ENTRY            = 7,
    KINGDOM_VIEW_TOWN_ENTRY              = 8,
    KINGDOM_VIEW_ICON_X                  = 481,
    KINGDOM_VIEW_ICON_Y                  = 393,
    KINGDOM_VIEW_TEXT_X_BASE             = 464,
    KINGDOM_VIEW_TEXT_Y_BASE             = 395,
    KINGDOM_VIEW_TEXT_WIDTH              = 32,
    KINGDOM_VIEW_TEXT_HEIGHT             = 12,
    KINGDOM_VIEW_RESOURCE_TEXT_Y         = 59,
    KINGDOM_VIEW_TOWN_TEXT_Y             = 28,
    KINGDOM_VIEW_WOOD_TEXT_X             = 15,
    KINGDOM_VIEW_MERCURY_TEXT_X          = 38,
    KINGDOM_VIEW_ORE_TEXT_X              = 61,
    KINGDOM_VIEW_SULFUR_TEXT_X           = 85,
    KINGDOM_VIEW_CRYSTAL_TEXT_X          = 109,
    KINGDOM_VIEW_GEMS_TEXT_X             = 132,
    KINGDOM_VIEW_GOLD_TEXT_X             = 123,
    KINGDOM_VIEW_CASTLE_TEXT_X           = 27,
    KINGDOM_VIEW_VILLAGE_TEXT_X          = 80,
    KINGDOM_VIEW_COUNT_ALLOC_LINE_OFFSET = 0x32
H2_ENUM_END(AdventureKingdomViewConstant)

H2_ENUM_BEGIN(AdventureVisionsConstant)
    VISIONS_RADIUS               = 3,
    VISIONS_NO_MONSTER_DISTANCE  = 100,
    VISIONS_MESSAGE_BUFFER_SIZE  = 200,
    VISIONS_JOIN_COST_MULTIPLIER = 2
H2_ENUM_END(AdventureVisionsConstant)

H2_ENUM_BEGIN(AdventureEnemyTurnViewConstant)
    ENEMY_TURN_BACKGROUND_X       = 480,
    ENEMY_TURN_BACKGROUND_Y       = 392,
    ENEMY_TURN_BACKGROUND_WIDTH   = 143,
    ENEMY_TURN_BACKGROUND_HEIGHT  = 71,
    ENEMY_TURN_HOURGLASS_X        = 493,
    ENEMY_TURN_HOURGLASS_Y        = 403,
    ENEMY_TURN_HOURGLASS_WIDTH    = 118,
    ENEMY_TURN_HOURGLASS_HEIGHT   = 51,
    ENEMY_TURN_CREST_X            = 495,
    ENEMY_TURN_ANIMATION_X        = 559,
    ENEMY_TURN_ANIMATION_Y        = 405,
    ENEMY_TURN_ANIMATION_WIDTH    = 50,
    ENEMY_TURN_ANIMATION_HEIGHT   = 47,
    ENEMY_TURN_BACKGROUND_ID      = 2000,
    ENEMY_TURN_HOURGLASS_ID       = 2001,
    ENEMY_TURN_CREST_ID           = 2002,
    ENEMY_TURN_SAND_ID            = 2003,
    ENEMY_TURN_PHASE_ID           = 2004,
    ENEMY_TURN_BACKGROUND_Z       = 1000,
    ENEMY_TURN_HOURGLASS_Z        = 1010,
    ENEMY_TURN_SAND_Z             = 1020,
    ENEMY_TURN_CREST_Z            = 1030,
    ENEMY_TURN_PHASE_Z            = 1040,
    ENEMY_TURN_SAND_FRAME_OFFSET  = 11,
    ENEMY_TURN_SAND_FRAME_LIMIT   = 20,
    ENEMY_TURN_SAND_RESTART_FRAME = 16,
    ENEMY_TURN_PHASE_FRAME_OFFSET = 1,
    ENEMY_TURN_CREST_SLOT         = 0,
    ENEMY_TURN_SAND_SLOT          = 1,
    ENEMY_TURN_PHASE_SLOT         = 2,
    ENEMY_TURN_ANIMATION_DELAY    = 300,
    ENEMY_TURN_PHASE_DELAY        = 700
H2_ENUM_END(AdventureEnemyTurnViewConstant)

H2_ENUM_BEGIN(AdventureCloudNeighbor)
    CLOUD_NORTH       = 0x01,
    CLOUD_EAST        = 0x02,
    CLOUD_SOUTH       = 0x04,
    CLOUD_WEST        = 0x08,
    CLOUD_NORTH_EAST  = 0x10,
    CLOUD_SOUTH_EAST  = 0x20,
    CLOUD_SOUTH_WEST  = 0x40,
    CLOUD_NORTH_WEST  = 0x80,
    CLOUD_LEFT_EDGE   = 0xc8,
    CLOUD_RIGHT_EDGE  = 0x32,
    CLOUD_TOP_EDGE    = 0x91,
    CLOUD_BOTTOM_EDGE = 0x64
H2_ENUM_END(AdventureCloudNeighbor)

H2_ENUM_BEGIN(AdventureQuickInfoObject)
    OBELISK_INDEX_BASE          = 1,
    RESOURCE_FRAME_PAIR_MASK    = ~1,
    RESOURCE_FRAME_PAIR_STRIDE  = 2,
    CRYSTAL_BALL_RADIUS         = 8,
    BARRIER_COLOR_MASK          = KB_BARRIER_COLOR_NAME_COUNT - 1,
    ROUTE_BEYOND_MOBILITY_FLAG  = 0x100,
    HERO_FRAME_MIRROR_FLAG      = 0x80,
    HERO_FRAME_INDEX_MASK       = 0x7f,
    QUICK_INFO_TEXT_CAPACITY    = 200,
    QUICK_INFO_SITE_FRAME_COUNT = 2,
    QUICK_INFO_X_OFFSET         = 57,
    QUICK_INFO_MIN_X            = QUICK_VIEW_MIN_X,
    QUICK_INFO_WIDTH            = 160,
    QUICK_INFO_RIGHT            = QUICK_VIEW_BOTTOM,
    QUICK_INFO_RIGHT_X          = QUICK_INFO_RIGHT - QUICK_INFO_WIDTH,
    QUICK_INFO_Y_OFFSET         = 25,
    QUICK_INFO_MIN_Y            = QUICK_VIEW_MIN_Y,
    QUICK_INFO_HEIGHT           = 96,
    QUICK_INFO_BOTTOM           = QUICK_VIEW_RIGHT,
    QUICK_INFO_BOTTOM_Y         = QUICK_INFO_BOTTOM - QUICK_INFO_HEIGHT
H2_ENUM_END(AdventureQuickInfoObject)

H2_ENUM_BEGIN(AdventureCheatConstant)
    CHEAT_SEQUENCE_RADIX     = 10,
    CHEAT_SEQUENCE_MODULUS   = 10000000,
    CHEAT_CREATURES_MODULUS  = 100000,
    CHEAT_LOSE_MODULUS       = 10000,
    CHEAT_SHORT_MODULUS      = 1000,
    CHEAT_CREATURES          = 32167,
    CHEAT_WIN                = 911,
    CHEAT_LOSE               = 1313,
    CHEAT_REVEAL_MAP         = 8675309,
    CHEAT_INFO               = 411,
    CHEAT_RESOURCE_AMOUNT    = 10,
    CHEAT_GOLD_AMOUNT        = 1000,
    CHEAT_EXPERIENCE_AMOUNT  = 3000,
    CHEAT_BLACK_DRAGON_COUNT = 5,
    CHEAT_REVEAL_CENTER      = 30,
    CHEAT_REVEAL_RADIUS      = 180
H2_ENUM_END(AdventureCheatConstant)

H2_ENUM_BEGIN(AdventureSiteConstant)
    GENERIC_SITE_1_END   = 4,
    GENERIC_SITE_2_END   = 0x48,
    GENERIC_ALTAR_END    = 5,
    GENERIC_UNUSED_END   = 10,
    GENERIC_TOWER_END    = 0x2f,
    GENERIC_SPRING_END   = 0x6f,
    GENERIC_SITE_3_SPLIT = 0x20,
    GENERIC_SITE_3_END   = 0x3b,
    RECRUITMENT_START    = 0x48,
    RECRUITMENT_1_END    = 0x4e,
    RECRUITMENT_2_END    = 0x70,
    RECRUITMENT_3_END    = 0x78,
    RECRUITMENT_4_END    = 0x81,
    RECRUITMENT_5_END    = 0x89
H2_ENUM_END(AdventureSiteConstant)

H2_ENUM_BEGIN(AdventurePanelCommand)
    PANEL_NEXT_HERO           = 1,
    PANEL_CONTINUE_ROUTE      = 2,
    PANEL_OVERVIEW            = 3,
    PANEL_END_TURN            = 4,
    PANEL_ADVENTURE_OPTIONS   = 5,
    PANEL_CONTROL_OPTIONS     = 6,
    PANEL_SYSTEM_OPTIONS      = 7,
    PANEL_CAST_SPELL          = 8,
    PANEL_RADAR               = 9,
    PANEL_MAP_VIEW            = 10,
    PANEL_HERO_PAGE_PREVIOUS  = 20,
    PANEL_HERO_PAGE_NEXT      = 21,
    PANEL_TOWN_PAGE_PREVIOUS  = 23,
    PANEL_TOWN_PAGE_NEXT      = 24,
    DIALOG_OK                 = 0x7806,
    BOTTOM_VIEW_FIRST_MESSAGE = 2000,
    BOTTOM_VIEW_LAST_MESSAGE  = 2200,
    BOTTOM_VIEW_DURATION      = 3000,
    LOCATORS_PER_PAGE         = 5,
    LOCATOR_PAGE_STEP         = 4
H2_ENUM_END(AdventurePanelCommand)

H2_ENUM_BEGIN(AdventureAdjacentMonsterConstant)
    ADJACENT_MONSTER_RADIUS     = 1,
    ADJACENT_MONSTER_END_OFFSET = 2,
    ADJACENT_OBJECT_INDEX_NONE  = 0xFF
H2_ENUM_END(AdventureAdjacentMonsterConstant)

H2_ENUM_BEGIN(AdventurePuzzleViewConstant)
    PUZZLE_MUSIC             = 23,
    PUZZLE_WINDOW_X          = DRAW_CLIP_WIDTH,
    PUZZLE_WINDOW_Y          = SCROLL_BORDER,
    PUZZLE_VIEW_ORIGIN       = SCROLL_BORDER,
    PUZZLE_VIEW_SIZE         = UPDATE_VIEWPORT_SIZE,
    PUZZLE_VIEW_END          = PUZZLE_VIEW_ORIGIN + PUZZLE_VIEW_SIZE,
    PUZZLE_COORDINATE_OFFSET = 7,
    PUZZLE_ALIGNMENT_DIVISOR = 3,
    PUZZLE_Y_ADJUST_X_FACTOR = 2,
    PUZZLE_Y_ADJUST_Y_FACTOR = 5,
    PUZZLE_PARITY_DIVISOR    = 2,
    PUZZLE_FIZZLE_TIME       = 220
H2_ENUM_END(AdventurePuzzleViewConstant)

H2_ENUM_BEGIN(AdventurePanelDialogConstant)
    PANEL_NO_HELP             = -1,
    PANEL_WINDOW_X            = 144,
    PANEL_WINDOW_Y            = 40,
    PANEL_DISABLED_WIDGET     = 4,
    PANEL_FIRST_WIDGET        = 1,
    PANEL_LAST_WIDGET         = 4,
    PANEL_CLOSE_WIDGET        = 0x7800,
    PANEL_VIEW_WORLD_HELP     = 0,
    PANEL_VIEW_PUZZLE_HELP    = 1,
    PANEL_SCENARIO_INFO_HELP  = 2,
    PANEL_SEARCH_HELP         = 3,
    PANEL_CLOSE_HELP          = 4,
    PANEL_VIEW_WORLD          = 1,
    PANEL_VIEW_PUZZLE         = 2,
    PANEL_SCENARIO_INFO       = 3,
    PANEL_SEARCH              = 4,
    CONTROL_RESTART           = 101,
    CONTROL_NEW_GAME          = 102,
    CONTROL_MAIN_MENU         = 105,
    CONTROL_SAVE_GAME         = 106,
    CONTROL_RESTART_HELP      = 0,
    CONTROL_NEW_GAME_HELP     = 1,
    CONTROL_SAVE_GAME_HELP    = 2,
    CONTROL_MAIN_MENU_HELP    = 3,
    CONTROL_CLOSE_HELP        = 4,
    CONTROL_CONFIRMATION_SIZE = 200
H2_ENUM_END(AdventurePanelDialogConstant)

H2_ENUM_BEGIN(AdventureSystemOptionsPrivateConstant)
    SYSTEM_OPTIONS_HELP_ACCEPT         = 0,
    SYSTEM_OPTIONS_HELP_MUSIC_VOLUME   = 1,
    SYSTEM_OPTIONS_HELP_SOUND_VOLUME   = 2,
    SYSTEM_OPTIONS_HELP_HERO_SPEED     = 3,
    SYSTEM_OPTIONS_HELP_MUSIC_SOURCE   = 4,
    SYSTEM_OPTIONS_HELP_SHOW_ROUTE     = 5,
    SYSTEM_OPTIONS_HELP_COMPUTER_SPEED = 6,
    SYSTEM_OPTIONS_HELP_INTERFACE      = 7,
    SYSTEM_OPTIONS_HELP_VIDEO          = 8,
    SYSTEM_OPTIONS_HELP_COLOR_CURSOR   = 9,
    SYSTEM_OPTIONS_HELP_FIRST_OPTION   = SYSTEM_OPTIONS_HELP_MUSIC_VOLUME,
    SYSTEM_OPTIONS_TEXT_CAPACITY       = 120
H2_ENUM_END(AdventureSystemOptionsPrivateConstant)

H2_ENUM_CLASS_BEGIN(AdventureMusicQuality)
    MUSIC_QUALITY_MIDI      = 0,
    MUSIC_QUALITY_CD_STEREO = 1,
    MUSIC_QUALITY_CD_OPERA  = 2
H2_ENUM_CLASS_END(AdventureMusicQuality)

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\ADVMGR.CPP"
#define ADVMGR_ENVIRONMENT_VOLUME(distance) environmentVolumes[distance]
#define ADVMGR_REMOTE_PAYLOAD(packet) (reinterpret_cast<AdventureRemotePayload*>((packet)->payload))

DATA(0x004eb660) static const i32 environmentVolumes[ENVIRONMENT_VOLUME_COUNT] = {64, 57, 40, 21, 7, 5, 3, 0};

DATA(0x00527eb8) static u16 s_drawGroundTile;
DATA(0x00527ebc) static i32 s_adjacentMonsterX;
DATA(0x00527ec0) static i32 s_drawPixelY;
DATA(0x00527ec4) static mapCellExtra* s_drawExtra;
DATA(0x00527ecc) static i32 s_adjacentMonsterY;
DATA(0x00527ed4) static i32 s_adjacentMonsterMinX;
DATA(0x00527ed8) static i32 s_adjacentMonsterMinY;
DATA(0x00527ee4) static i32 s_drawMonsterFrame;
DATA(0x00527f0c) static i32 s_drawCloudFrame;
DATA(0x00527f10) static i32 s_drawStoneTile;
DATA(0x00527f18) static mapCell* s_drawCell;
DATA(0x00527f1c) static mineRecord* s_drawMine;
DATA(0x00527f20) static hero* s_drawHero;
DATA(0x00527f3c) static H2_ENUM_STORAGE(HeroCursorType, i32) s_drawHeroType;
DATA(0x00527f44) static i32 s_drawCovered;
DATA(0x00527f48) static mapCell* s_drawAdjacentCell;
DATA(0x00528094) static i32 s_drawHasHero;
DATA(0x00528098) static i32 s_adjacentMonsterEndY;
DATA(0x0052809c) static i32 s_drawFlipCloud;
DATA(0x005280a0) static i32 s_drawHeroFrame;
DATA(0x005280a4) static i32 s_drawPixelX;
DATA(0x005280a8) static i32 s_drawPlayerColor;
DATA(0x005280cc) static i32 s_adjacentMonsterEndX;
DATA(0x005280d0) static i32 s_drawAnimationLength;
DATA(0x005280d8) static i32 s_drawHeroYOffset;
H2_ENUM_BEGIN(AdvVisitMetadata)
    VISIT_BIT_INDEX_MASK = 0x1f
H2_ENUM_END(AdvVisitMetadata)

VA(0x00456350, 0x30f)
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

VA(0x0045665f, 0x9c9)
i32 advManager::Open(i32 id) {
    iCurBottomView = BOTTOM_VIEW_NONE;
    m_openState = 0;
    bShowIt = 0;
    m_adventureBorder = NULL;

    i32 resourceIndex;
    for (resourceIndex = 0; resourceIndex < ADVMGR_LOCATOR_STATE_COUNT; ++resourceIndex) {
        m_heroLocatorState[resourceIndex] = 0;
        m_townLocatorState[resourceIndex] = 0;
    }

    if (m_adventureWindow == NULL) {
        m_adventureWindow = new heroWindow(0, 0, DATA_COMPGEN(0x004f57dc, openAdvWindBin, "adv_wind.bin"));
        if (m_adventureWindow == NULL) {
            MemError();
        }
        m_scrollLeftButton = new iconWidget(
            SCROLL_LEFT_X,
            SCROLL_Y,
            SCROLL_WIDTH,
            SCROLL_HEIGHT,
            DATA_COMPGEN(0x004f57ec, openScrollIcn, "scroll.icn"),
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
            DATA_COMPGEN(0x004f57f8, openScrollIcn2, "scroll.icn"),
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
        gpMouseManager->SetPointer(DATA_COMPGEN(0x004f5804, openAdvmiceMse, "advmice.mse"), 0, MOUSE_AUTO_CURSOR_TYPE);
    } else {
        gpMouseManager->SetPointer(DATA_COMPGEN(0x004f5810, openAdvmiceMse2, "advmice.mse"), 1, MOUSE_AUTO_CURSOR_TYPE);
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
        m_groundTiles = gpResourceManager->GetTileset(DATA_COMPGEN(0x004f581c, openGround32Til, "ground32.til"));
    }
    if (m_cloudTiles == NULL) {
        m_cloudTiles = gpResourceManager->GetTileset(DATA_COMPGEN(0x004f582c, openClof32Til, "clof32.til"));
    }
    if (m_stoneTiles == NULL) {
        m_stoneTiles = gpResourceManager->GetTileset(DATA_COMPGEN(0x004f5838, openStonTil, "ston.til"));
    }
    if (m_cloudOverlayIcon == NULL) {
        m_cloudOverlayIcon = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5844, openClop32Icn, "clop32.icn"));
    }

    for (resourceIndex = 0; resourceIndex < OBJECT_ICON_COUNT; ++resourceIndex) {
        if (strlen(gTilesetFiles[resourceIndex]) > 1 && m_objectIcons[resourceIndex] == NULL
            && resourceIndex != UNUSED_OBJECT_ICON_1
            && resourceIndex != UNUSED_OBJECT_ICON_2) {
            m_objectIcons[resourceIndex] = gpResourceManager->GetIcon(gTilesetFiles[resourceIndex]);
        }
    }

    if (m_heroIcons[IDX(FACTION_KNIGHT)] == NULL) {
        m_heroIcons[IDX(FACTION_KNIGHT)] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5850, openKngt32Icn, "kngt32.icn"));
    }
    if (m_heroIcons[IDX(FACTION_BARBARIAN)] == NULL) {
        m_heroIcons[IDX(FACTION_BARBARIAN)] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f585c, openBarb32Icn, "barb32.icn"));
    }
    if (m_heroIcons[IDX(FACTION_SORCERESS)] == NULL) {
        m_heroIcons[IDX(FACTION_SORCERESS)] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5868, openSorc32Icn, "sorc32.icn"));
    }
    if (m_heroIcons[IDX(FACTION_WARLOCK)] == NULL) {
        m_heroIcons[IDX(FACTION_WARLOCK)] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5874, openWrlk32Icn, "wrlk32.icn"));
    }
    if (m_heroIcons[IDX(FACTION_WIZARD)] == NULL) {
        m_heroIcons[IDX(FACTION_WIZARD)] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5880, openWzrd32Icn, "wzrd32.icn"));
    }
    if (m_heroIcons[IDX(FACTION_NECROMANCER)] == NULL) {
        m_heroIcons[IDX(FACTION_NECROMANCER)] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f588c, openNecr32Icn, "necr32.icn"));
    }
    if (m_heroIcons[IDX(HERO_TYPE_BOAT)] == NULL) {
        m_heroIcons[IDX(HERO_TYPE_BOAT)] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5898, openBoat32Icn, "boat32.icn"));
    }
    if (m_heroIcons[HERO_ICON_FROTH] == NULL) {
        m_heroIcons[HERO_ICON_FROTH] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f58a4, openFrothIcn, "froth.icn"));
    }

    gbLoadingMonoIcon = true;
    if (m_shadowIcon == NULL) {
        m_shadowIcon = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f58b0, openShadow32Icn, "shadow32.icn"));
    }
    if (m_boatShadowIcon == NULL) {
        m_boatShadowIcon = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f58c0, openBoatshadIcn, "boatshad.icn"));
    }
    gbLoadingMonoIcon = false;

    if (m_flagIcons[PLAYER_COLOR_BLUE] == NULL) {
        m_flagIcons[PLAYER_COLOR_BLUE] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f58d0, openBFlag32Icn, "b-flag32.icn"));
    }
    if (m_flagIcons[PLAYER_COLOR_GREEN] == NULL) {
        m_flagIcons[PLAYER_COLOR_GREEN] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f58e0, openGFlag32Icn, "g-flag32.icn"));
    }
    if (m_flagIcons[PLAYER_COLOR_RED] == NULL) {
        m_flagIcons[PLAYER_COLOR_RED] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f58f0, openRFlag32Icn, "r-flag32.icn"));
    }
    if (m_flagIcons[PLAYER_COLOR_YELLOW] == NULL) {
        m_flagIcons[PLAYER_COLOR_YELLOW] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5900, openYFlag32Icn, "y-flag32.icn"));
    }
    if (m_flagIcons[PLAYER_COLOR_ORANGE] == NULL) {
        m_flagIcons[PLAYER_COLOR_ORANGE] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5910, openOFlag32Icn, "o-flag32.icn"));
    }
    if (m_flagIcons[PLAYER_COLOR_PURPLE] == NULL) {
        m_flagIcons[PLAYER_COLOR_PURPLE] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5920, openPFlag32Icn, "p-flag32.icn"));
    }
    if (m_boatFlagIcons[PLAYER_COLOR_BLUE] == NULL) {
        m_boatFlagIcons[PLAYER_COLOR_BLUE] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5930, openBBflg32Icn, "b-bflg32.icn"));
    }
    if (m_boatFlagIcons[PLAYER_COLOR_GREEN] == NULL) {
        m_boatFlagIcons[PLAYER_COLOR_GREEN] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5940, openGBflg32Icn, "g-bflg32.icn"));
    }
    if (m_boatFlagIcons[PLAYER_COLOR_RED] == NULL) {
        m_boatFlagIcons[PLAYER_COLOR_RED] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5950, openRBflg32Icn, "r-bflg32.icn"));
    }
    if (m_boatFlagIcons[PLAYER_COLOR_YELLOW] == NULL) {
        m_boatFlagIcons[PLAYER_COLOR_YELLOW] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5960, openYBflg32Icn, "y-bflg32.icn"));
    }
    if (m_boatFlagIcons[PLAYER_COLOR_ORANGE] == NULL) {
        m_boatFlagIcons[PLAYER_COLOR_ORANGE] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5970, openOBflg32Icn, "o-bflg32.icn"));
    }
    if (m_boatFlagIcons[PLAYER_COLOR_PURPLE] == NULL) {
        m_boatFlagIcons[PLAYER_COLOR_PURPLE] = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5980, openPBflg32Icn, "p-bflg32.icn"));
    }

    gbLoadingMonoIcon = true;
    if (m_puzzleIcon == NULL) {
        m_puzzleIcon = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f5990, openRadarIcn, "radar.icn"));
    }
    gbLoadingMonoIcon = false;

    for (resourceIndex = 0; resourceIndex < LOOPING_SAMPLE_COUNT; ++resourceIndex) {
        m_loopingSamples[resourceIndex] = NULL;
    }
    for (resourceIndex = 0; resourceIndex < SOUND_CELL_COUNT; ++resourceIndex) {
        m_activeSounds[resourceIndex].soundId = ADVMGR_ENVIRONMENT_SOUND_NONE;
        m_activeSounds[resourceIndex].volume = ENVIRONMENT_SOUND_DEFAULT_VOLUME;
        m_activeSoundMask = 0;
    }

    GetCursorSampleSet(gConfig.walkSpeed);
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        gpGame->TurnOnAIMusic();
        SetNoDialogMenus(0);
    } else {
        SetNoDialogMenus(1);
    }

    glTimers[0] = KBTickCount() + TIMER_DELAY;
    ConfigVolumeLevel oldSampleVolumeState = gConfig.soundVolume;
    if (gConfig.soundVolume != CONFIG_VOLUME_MUTED) {
        gConfig.soundVolume = CONFIG_VOLUME_MAX;
    }
    SetInitialMapOrigin();

    bShowIt = gbThisNetHumanPlayer[giCurPlayer];
    i32 oldPlayer = giCurPlayer;
    i32 oldShowItValue = bShowIt;
    giCurPlayer = giCurWatchPlayer;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    bShowIt = 1;
    RedrawAdvScreen(1, 0);
    giCurPlayer = oldPlayer;
    bShowIt = oldShowItValue;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        gpGame->ShowComputerScreen();
    }
    KBChangeMenu(hmnuAdv);
    ForceNewHover();
    gpWindowManager->FadeScreen(FADE_IN, ADVENTURE_FADE_STEPS, gPalette);
    giBottomViewOverride = BOTTOM_VIEW_NONE;
    gConfig.soundVolume = oldSampleVolumeState;
    gpSoundManager->AdjustSoundVolumes();
    m_messageMask = BASE_MANAGER_ACCEPT_ADVENTURE;
    m_priority = id;
    m_active = true;
    strcpy(m_name, DATA_COMPGEN(0x004f599c, openAdvManager, "advManager"));
    return 0;
}

VA(0x00457028, 0x40a)
void advManager::Close(void) {
    DATA(0x004f59a8) static i16 s_closeBorderFreeLineBase = CLOSE_BORDER_FREE_LINE_BASE;

    ClearBottomView();
    gpMouseManager->SetPointer(-1);
    if (!bEnteringTown || gConfig.useOpera
        || gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI) {
        gpSoundManager->SwitchAmbientMusic(-1);
        gpSoundManager->StopAllSamples(1);
    } else {
        gpSoundManager->StopAllSamples(0);
    }
    if (m_adventureBorder != NULL) {
        H2_FREE_AT(m_adventureBorder, DATA_COMPGEN(0x004f59ac, closeSourceFile, RETAIL_FILE), s_closeBorderFreeLineBase + BORDER_INITIAL_FREE_LINE_OFFSET);
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

VA(0x00457432, 0xe9)
void advManager::GetCursorSampleSet(i32 sampleSet) {
    if (sampleSet >= 1)
        sampleSet = CURSOR_SAMPLE_FAST_SET;
    // Ordered filename suffixes select the retail walking-sound variants.
    // NOLINTBEGIN(readability-magic-numbers)
    i32 sampleSuffix[CURSOR_SAMPLE_COUNT] = {0, 3, 5, 3, 4, 5, 6, 3, 3};
    // NOLINTEND(readability-magic-numbers)
    for (i32 index = 0; index < CURSOR_SAMPLE_COUNT; ++index) {
        sprintf(gText, DATA_COMPGEN(0x004f59d8, getCursorSampleSetWsnd1d1d82M, "wsnd%1d%1d.82M"), sampleSet, sampleSuffix[index]);
        m_cursorSamples[index] = gpResourceManager->GetSample(gText);
        m_cursorSamples[index]->m_playbackData.volume = CURSOR_SAMPLE_VOLUME;
        m_cursorSamples[index]->m_playbackData.channelType = CURSOR_SAMPLE_CHANNEL;
    }
}

VA(0x0045751b, 0x6c0)
class mapCell* advManager::DoAdvCommand(void) {
    mapCell* eventCellState = NULL;
    town* viewTownValue;
    char movementStopped = 0;
    char refreshHover = 0;
    hero* currentHeroState;
    i32 oldVisibilityStateLocal;
    i32 pathIndexLocal;
    i32 movementChangedResult;
    i32 movementEndedLocal;
    tag_message messageValue;
    if (gpCurPlayer->m_currentHero == INVALID_HERO) {
        currentHeroState = NULL;
    } else {
        currentHeroState = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];
    }

    switch (m_selectedCell) {
        case ADVMGR_COMMAND_MOVE_TO:
            if (currentHeroState == NULL) {
                break;
            }
            currentHeroState->m_destinationX = m_commandTargetX,
            currentHeroState->m_destinationY = m_commandTargetY;
            goto continue_route;
        case ADVMGR_COMMAND_CONTINUE_ROUTE:
        continue_route:
            if (currentHeroState == NULL) {
                break;
            }
            if (currentHeroState->m_destinationX == HERO_DESTINATION_NONE
                || currentHeroState->m_destinationY == HERO_DESTINATION_NONE) {
                break;
            }
            gpSearchArray->BuildPath(
                currentHeroState->m_x,
                currentHeroState->m_y,
                currentHeroState->m_destinationX,
                currentHeroState->m_destinationY,
                ROUTE_PATH_COST_LIMIT
            );
            if (gpSearchArray->m_pathLength > 0) {
                oldVisibilityStateLocal = m_visibilityMapValid;
                MobilizeCurrHero(1);
                if (gConfig.showRoute || oldVisibilityStateLocal) {
                    ShowRoute(0, 0, 0);
                } else {
                    if (m_visibilityMapValid && m_selectedCell != ADVMGR_COMMAND_CONTINUE_ROUTE) {
                        HideRoute(1, 0, 1);
                    }
                }
                gpMouseManager->HideColorPointer();
                gpInputManager->Flush();

                pathIndexLocal = gpSearchArray->m_pathLength - 1;
                for (; pathIndexLocal >= 0; --pathIndexLocal) {
                    eventCellState = MoveHero(
                        static_cast<MapDirection>(
                            gpSearchArray->m_storage.path.directions[pathIndexLocal + 1]
                        ),
                        pathIndexLocal == 0,
                        &TrigX,
                        &TrigY,
                        &movementChangedResult,
                        0,
                        &movementEndedLocal,
                        0
                    );
                    UpdateHeroLocator(-1, 1, 1);
                    if (eventCellState != NULL) {
                        break;
                    }
                    if (movementChangedResult || movementEndedLocal || gbHitEvent) {
                        goto movement_done;
                    }
                    messageValue = gpInputManager->GetEvent();
                    while (messageValue.type != MESSAGE_NONE) {
                        if (messageValue.type == MESSAGE_KEY_DOWN
                            || messageValue.type == MESSAGE_LEFT_BUTTON_DOWN
                            || messageValue.type == MESSAGE_RIGHT_BUTTON_DOWN
                            || messageValue.type == MESSAGE_WIDGET) {
                            movementStopped = 1;
                            StopCursor(1);
                            goto movement_done;
                        }
                        Process1WindowsMessage();
                        messageValue = gpInputManager->GetEvent();
                    }
                }

            movement_done:
                if ((pathIndexLocal <= 0
                     && currentHeroState->m_x == currentHeroState->m_destinationX
                     && currentHeroState->m_destinationY == currentHeroState->m_y)
                    || (movementStopped && !gConfig.showRoute) || eventCellState != NULL) {
                    HideRoute(1, 1, 1);
                } else if (m_selectedCell == ADVMGR_COMMAND_CONTINUE_ROUTE || gConfig.showRoute) {
                    ShowRoute(0, 1, 1);
                }
                while (gpMouseManager->m_hideCount != 0) {
                    gpMouseManager->ShowColorPointer();
                }
                gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
                UpdBottomView(1, 1, 1);
                if (eventCellState != NULL) {
                    StopCursor(1);
                    DoEvent(eventCellState, TrigX, TrigY);
                    eventCellState = NULL;
                }
                Reseed(0, 0);
                refreshHover = 1;
                CheckDimHero();
            }
            break;

        case ADVMGR_COMMAND_OCCUPIED_TOWN_VIEW:
            DemobilizeCurrHero();
            gpMouseManager->SetPointer(0);
            viewTownValue = gpGame->GetTown(currentHeroState->m_occupiedTown);
            viewTownValue->View(0);
            eventCellState = NULL;
            break;

        case ADVMGR_COMMAND_TOWN_VIEW:
            DemobilizeCurrHero();
            gpMouseManager->SetPointer(0);
            eventCellState = GetCell(
                gpGame->m_castleRecs[gpCurPlayer->CurrentTown()].m_x,
                gpGame->m_castleRecs[gpCurPlayer->CurrentTown()].m_y
            );
            gpGame->m_castleRecs[gpCurPlayer->CurrentTown()].View(0);
            eventCellState = NULL;
            break;

        case ADVMGR_COMMAND_HERO_VIEW:
            if (currentHeroState == NULL) {
                break;
            }
            gpMouseManager->SetPointer(0);
            if (gbLowMemory) {
                SetEnvironmentOrigin(ENVIRONMENT_ORIGIN_NONE, ENVIRONMENT_ORIGIN_NONE, 1);
            }
            TrimLoopingSounds(0);
            HeroView(gpCurPlayer->CurrentHero(), 0, 0);
            if (gbLowMemory) {
                SetEnvironmentOrigin(
                    m_mapOriginX + VIEW_CENTER_OFFSET,
                    m_mapOriginY + VIEW_CENTER_OFFSET,
                    1
                );
            }
            RedrawAdvScreen(1, 0);
            gpWindowManager->FadeScreen(FADE_IN, ADVENTURE_FADE_STEPS, NULL);
            break;

        case ADVMGR_COMMAND_SELECT_HERO:
            SetHeroContext(
                GetCell(m_mapOriginX + m_lastHoverCell, m_hoverCellY + m_mapOriginY)
                    ->m_objectMetadata,
                0
            );
            break;

        case ADVMGR_COMMAND_SELECT_TOWN:
            SetTownContext(GetCell(m_mapOriginX + m_lastHoverCell, m_hoverCellY + m_mapOriginY)
                               ->m_objectMetadata);
            break;

        case ADVMGR_COMMAND_NONE:
            break;
    }

    m_selectedCell = ADVMGR_COMMAND_NONE;
    m_hoverCellY = CURSOR_INVALID_POSITION;
    m_lastHoverCell = m_hoverCellY;
    if (refreshHover) {
        ForceNewHover();
    }
    return eventCellState;
}

VA(0x00457bdb, 0x191)
void advManager::CheckSetEvilInterface(i32 redraw, i32 player) {
    i32 shouldChange;
    i32 translationIndex;
    i32 savedShowIt;
    tag_message interfaceMessage;

    if (player == -1) {
        player = giCurWatchPlayer;
    }

    shouldChange = 0;
    if (gConfig.evilInterfaceUsage == INTERFACE_EVIL && !gbUseEvilInterface) {
        shouldChange = 1;
    } else if (gConfig.evilInterfaceUsage == INTERFACE_GOOD && gbUseEvilInterface) {
        shouldChange = 1;
    } else if (gConfig.evilInterfaceUsage == INTERFACE_AUTO
               && gpGame->m_players[player].m_evilInterface != gbUseEvilInterface) {
        shouldChange = 1;
    }

    if (shouldChange) {
        gbUseEvilInterface = 1 - gbUseEvilInterface;
        if (redraw) {
            interfaceMessage.type = ADVMGR_INTERFACE_MESSAGE;
            interfaceMessage.payload.widget.command = ADVMGR_INTERFACE_REPLACE_RESOURCE;
            for (translationIndex = 0; translationIndex < INTERFACE_TRANSLATION_COUNT;
                 ++translationIndex) {
                interfaceMessage.payload.widget.id = gpResourceManager->MakeId(
                    cEvilTranslate[translationIndex][1 - gbUseEvilInterface],
                    0
                );
                interfaceMessage.payload.widget.data.value = gpResourceManager->MakeId(
                    cEvilTranslate[translationIndex][gbUseEvilInterface],
                    0
                );
                m_adventureWindow->BroadcastMessage(interfaceMessage);
            }
            savedShowIt = bShowIt;
            bShowIt = 1;
            RedrawAdvScreen(1, 1);
            bShowIt = savedShowIt;
        }
    }
}

VA(0x00457d6c, 0xfda)
MessageDispatchResult advManager::Main(struct tag_message& message) {
    if (KBTickCount() > glTimers[0] && ComboDraw(1)) {
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
    if (!gbNoSound && gConfig.musicVolume != CONFIG_VOLUME_MUTED && giForceSwitchMusic > 0
        && KBTickCount() - giForceSwitchMusic > FORCED_MUSIC_DELAY) {
        giForceSwitchMusic = -1;
        if (gpSoundManager->m_currentTrack == WAIT_AMBIENT_MUSIC) {
            gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
        }
        SetEnvironmentOrigin(
            m_mapOriginX + VIEW_CENTER_OFFSET,
            m_mapOriginY + VIEW_CENTER_OFFSET,
            1
        );
    }

    MessageDispatchResult processResult = MESSAGE_DISPATCH_CONSUME;
    i32 exitRequestedFlag = 0;
    // Retail reserves three result slots; only the first is used.
    mapCell* eventCellsResult[ADVENTURE_EVENT_CELL_RESULT_COUNT];
    eventCellsResult[0] = NULL;
    MapDirection moveDirectionState;
    i32 cheatDigitLocal;
    i32 nextTownId;
    hero* currentHero;
    if (message.type != MESSAGE_NONE) {
        switch (message.type) {
            case MESSAGE_WIDGET:
                switch (message.payload.widget.command) {
                    case WIDGET_COMMAND_DESELECT:
                        if (!(HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON))) {
                            processResult =
                                ProcessDeSelect(&message, &exitRequestedFlag, eventCellsResult);
                        }
                        break;
                    case WIDGET_COMMAND_SELECT:
                    case WIDGET_COMMAND_ALTERNATE_SELECT: {
                        i32 helpIndexState;
                        if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) {
                            helpIndexState = -1;
                            switch (message.payload.widget.id) {
                                case PANEL_NEXT_HERO:
                                    helpIndexState = 0;
                                    break;
                                case PANEL_CONTINUE_ROUTE:
                                    helpIndexState = 1;
                                    break;
                                case PANEL_OVERVIEW:
                                    helpIndexState = 2;
                                    break;
                                case PANEL_END_TURN:
                                    helpIndexState = 3;
                                    break;
                                case PANEL_ADVENTURE_OPTIONS:
                                    helpIndexState = 4;
                                    break;
                                case PANEL_CONTROL_OPTIONS:
                                    helpIndexState = 5;
                                    break;
                                case PANEL_SYSTEM_OPTIONS:
                                    helpIndexState = 6;
                                    break;
                                case PANEL_CAST_SPELL:
                                    helpIndexState = 7;
                                    break;
                            }
                            if (helpIndexState >= 0) {
                                NormalDialog(
                                    gAdvMenuHelp[helpIndexState],
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
                        processResult = ProcessSelect(&message, eventCellsResult);
                        break;
                    }
                }
                break;
            case MESSAGE_MOUSE_MOVE:
                processResult =
                    ProcessHover(message.payload.mouse.screenX, message.payload.mouse.screenY);
            default:
                break;
            case MESSAGE_KEY_DOWN:
                moveDirectionState = MAP_DIRECTION_NONE;
                if (gpCurPlayer->CurrentHero() != INVALID_HERO) {
                    currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
                } else {
                    currentHero = NULL;
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
                        // Retail's debug command advances an opaque cheat-state payload.
                        // NOLINTNEXTLINE(readability-magic-numbers)
                        gpCurPlayer->m_cheatValue += 12;
                        if (currentHero != NULL) {
                            GiveExperience(currentHero, CHEAT_EXPERIENCE_AMOUNT, 1);
                        }
                        break;
                    case INPUT_SCAN_F8:
                        break;
                    case INPUT_SCAN_F9:
                        for (cheatDigitLocal = 0; cheatDigitLocal < IDX(RES_COUNT);
                             ++cheatDigitLocal) {
                            if (cheatDigitLocal == IDX(RES_GOLD)) {
                                gpCurPlayer->m_resources[cheatDigitLocal] += CHEAT_GOLD_AMOUNT;
                            } else {
                                gpCurPlayer->m_resources[cheatDigitLocal] += CHEAT_RESOURCE_AMOUNT;
                            }
                        }
                        break;
                    // Scan-code cases intentionally decode to their literal decimal digits.
                    // NOLINTBEGIN(readability-magic-numbers)
                    case INPUT_SCAN_0:
                        cheatDigitLocal = 0;
                        goto process_cheat_digit;
                    case INPUT_SCAN_1:
                        cheatDigitLocal = 1;
                        goto process_cheat_digit;
                    case INPUT_SCAN_2:
                        cheatDigitLocal = 2;
                        goto process_cheat_digit;
                    case INPUT_SCAN_3:
                        cheatDigitLocal = 3;
                        goto process_cheat_digit;
                    case INPUT_SCAN_4:
                        cheatDigitLocal = 4;
                        goto process_cheat_digit;
                    case INPUT_SCAN_5:
                        cheatDigitLocal = 5;
                        goto process_cheat_digit;
                    case INPUT_SCAN_6:
                        cheatDigitLocal = 6;
                        goto process_cheat_digit;
                    case INPUT_SCAN_7:
                        cheatDigitLocal = 7;
                        goto process_cheat_digit;
                    case INPUT_SCAN_8:
                        cheatDigitLocal = 8;
                        goto process_cheat_digit;
                    case INPUT_SCAN_9:
                        cheatDigitLocal = 9;
                        goto process_cheat_digit;
                    // NOLINTEND(readability-magic-numbers)
                    process_cheat_digit: {
                        hero* cheatHero = NULL;
                        if (gpCurPlayer->CurrentHero() != INVALID_HERO) {
                            cheatHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
                        }
                        giCheatSeq = cheatDigitLocal
                                     + (giCheatSeq * CHEAT_SEQUENCE_RADIX)
                                           % CHEAT_SEQUENCE_MODULUS;
                        if (!gbRemoteOn) {
                            if (giCheatSeq % CHEAT_CREATURES_MODULUS == CHEAT_CREATURES
                                && cheatHero != NULL) {
                                gpGame->m_cheated = 1;
                                if (gbInCampaign) {
                                    gpGame->m_campaignCheated = 1;
                                }
                                gpGame->GiveArmy(
                                    &cheatHero->m_army,
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
                                DATA_COMPGEN(0x004f59f0, mainCoordinatesAtTopLeftCornerOf, "Coordinates at top left corner of view:\n\n  X: %d\n  Y: %d"),
                                m_mapOriginX,
                                m_mapOriginY
                            );
                            NormalDialog(
                                gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0
                            );
                        }
                        break;
                    }
                    // Retail places this case after the cheat-digit block in source order.
                    case INPUT_SCAN_ESCAPE:
                        break;
                    case INPUT_SCAN_NUMPAD_8:
                        if (HAS(message.payload.keyboard.modifiers, MESSAGE_MODIFIER_CONTROL_KEYS)) {
                            ScreenScroll(MAP_DIRECTION_NORTH, 0);
                        } else {
                            moveDirectionState = MAP_DIRECTION_NORTH;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_9:
                        if (HAS(message.payload.keyboard.modifiers, MESSAGE_MODIFIER_CONTROL_KEYS)) {
                            ScreenScroll(MAP_DIRECTION_NORTH_EAST, 0);
                        } else {
                            moveDirectionState = MAP_DIRECTION_NORTH_EAST;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_6:
                        if (HAS(message.payload.keyboard.modifiers, MESSAGE_MODIFIER_CONTROL_KEYS)) {
                            ScreenScroll(MAP_DIRECTION_EAST, 0);
                        } else {
                            moveDirectionState = MAP_DIRECTION_EAST;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_3:
                        if (HAS(message.payload.keyboard.modifiers, MESSAGE_MODIFIER_CONTROL_KEYS)) {
                            ScreenScroll(MAP_DIRECTION_SOUTH_EAST, 0);
                        } else {
                            moveDirectionState = MAP_DIRECTION_SOUTH_EAST;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_2:
                        if (HAS(message.payload.keyboard.modifiers, MESSAGE_MODIFIER_CONTROL_KEYS)) {
                            ScreenScroll(MAP_DIRECTION_SOUTH, 0);
                        } else {
                            moveDirectionState = MAP_DIRECTION_SOUTH;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_1:
                        if (HAS(message.payload.keyboard.modifiers, MESSAGE_MODIFIER_CONTROL_KEYS)) {
                            ScreenScroll(MAP_DIRECTION_SOUTH_WEST, 0);
                        } else {
                            moveDirectionState = MAP_DIRECTION_SOUTH_WEST;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_4:
                        if (HAS(message.payload.keyboard.modifiers, MESSAGE_MODIFIER_CONTROL_KEYS)) {
                            ScreenScroll(MAP_DIRECTION_WEST, 0);
                        } else {
                            moveDirectionState = MAP_DIRECTION_WEST;
                        }
                        break;
                    case INPUT_SCAN_NUMPAD_7:
                        if (HAS(message.payload.keyboard.modifiers, MESSAGE_MODIFIER_CONTROL_KEYS)) {
                            ScreenScroll(MAP_DIRECTION_NORTH_WEST, 0);
                        } else {
                            moveDirectionState = MAP_DIRECTION_NORTH_WEST;
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
                        cheatDigitLocal = 'e';
                        strcpy(
                            gText,
                            DATA_COMPGEN(0x004f5a2c, mainAreYouSureYouWantTo, "Are you sure you want to restart?  (Your current game will be lost)")
                        );
                        goto confirm_game_command;
                    case INPUT_SCAN_L:
                        cheatDigitLocal = 'f';
                        strcpy(
                            gText,
                            DATA_COMPGEN(0x004f5a70, mainAreYouSureYouWantTo2, "Are you sure you want to load a new game?  (Your current game will be "
                            "lost)")
                        );
                        goto confirm_game_command;
                    case INPUT_SCAN_Q:
                        cheatDigitLocal = 'i';
                        strcpy(gText, DATA_COMPGEN(0x004f5abc, mainAreYouSureYouWantTo3, "Are you sure you want to quit?"));
                        goto confirm_game_command;
                    confirm_game_command:
                        exitRequestedFlag = 1;
                        NormalDialog(
                            gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0
                        );
                        if (gpWindowManager->m_dialogResult == DIALOG_OK) {
                            exitRequestedFlag = 0;
                        } else {
                            gGameCommand = cheatDigitLocal;
                        }
                        break;
                    case INPUT_SCAN_S:
                        SaveGame();
                        break;
                    case INPUT_SCAN_I:
                        if (gbInCampaign) {
                            SetEnvironmentOrigin(
                                ENVIRONMENT_ORIGIN_NONE, ENVIRONMENT_ORIGIN_NONE, 1
                            );
                            gpGame->ShowCampaignInfo(1, 0);
                            SetEnvironmentOrigin(
                                m_mapOriginX + VIEW_CENTER_OFFSET,
                                m_mapOriginY + VIEW_CENTER_OFFSET,
                                1
                            );
                            RedrawAdvScreen(1, 0);
                            gpSoundManager->SwitchAmbientMusic(
                                giTerrainToMusicTrack[IDX(m_currentTerrain)]
                            );
                        } else if (xIsPlayingExpansionCampaign) {
                            SetEnvironmentOrigin(
                                ENVIRONMENT_ORIGIN_NONE, ENVIRONMENT_ORIGIN_NONE, 1
                            );
                            xCampaign.ShowInfo(1, 0);
                            SetEnvironmentOrigin(
                                m_mapOriginX + VIEW_CENTER_OFFSET,
                                m_mapOriginY + VIEW_CENTER_OFFSET,
                                1
                            );
                            RedrawAdvScreen(1, 0);
                            gpSoundManager->SwitchAmbientMusic(
                                giTerrainToMusicTrack[IDX(m_currentTerrain)]
                            );
                        } else {
                            gpGame->ShowScenInfo();
                        }
                        break;
                    case INPUT_SCAN_T:
                        if (gpCurPlayer->m_townCount >= 0) {
                            if (gpCurPlayer->CurrentTown() == TOWN_ID_NONE) {
                                nextTownId = gpCurPlayer->m_townIds[0];
                            } else {
                                nextTownId = 0;
                                for (cheatDigitLocal = 0;
                                     cheatDigitLocal < gpCurPlayer->m_townCount;
                                     ++cheatDigitLocal) {
                                    if (gpCurPlayer->TownId(cheatDigitLocal)
                                        == gpCurPlayer->m_currentTown) {
                                        if (cheatDigitLocal == gpCurPlayer->m_townCount - 1) {
                                            nextTownId = gpCurPlayer->m_townIds[0];
                                        } else {
                                            nextTownId =
                                                gpCurPlayer->m_townIds[cheatDigitLocal + 1];
                                        }
                                    }
                                }
                            }
                            SetTownContext(nextTownId);
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
                    && moveDirectionState >= MAP_DIRECTION_NORTH) {
                    HideRoute(1, 1, 1);
                    gpMouseManager->HideColorPointer();
                    i32 movementChanged;
                    i32 movementEnded;
                    eventCellsResult[0] = MoveHero(
                        moveDirectionState,
                        1,
                        &TrigX,
                        &TrigY,
                        &movementChanged,
                        0,
                        &movementEnded,
                        0
                    );
                    UpdateHeroLocator(-1, 1, 1);
                    while (gpMouseManager->m_hideCount != 0) {
                        gpMouseManager->ShowColorPointer();
                    }
                    gpSoundManager->SwitchAmbientMusic(
                        giTerrainToMusicTrack[IDX(m_currentTerrain)]
                    );
                    if (eventCellsResult[0] != NULL) {
                        StopCursor(1);
                        DoEvent(eventCellsResult[0], TrigX, TrigY);
                        eventCellsResult[0] = NULL;
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
    if (eventCellsResult[0] != NULL) {
        DoEvent(eventCellsResult[0], TrigX, TrigY);
    }
    if (gbGameOver || exitRequestedFlag == 1 || giMenuCommand != -1) {
        message.type = MESSAGE_EXECUTIVE;
        message.payload.executive.command = EXECUTIVE_COMMAND_TERMINATE_LOOP;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return processResult;
}

VA(0x00458d46, 0x22)
void advManager::Reseed(i32, i32) {
    giSeedingValid = 0;
}

VA(0x00458d68, 0xeb1)
MessageDispatchResult
advManager::ProcessSelect(struct tag_message* message, class mapCell** eventCell) {
    i32 mouseX;
    mapCell* currentCell;
    i32 objectTypeState;
    i32 objectIdIndex;
    i32 mouseY;
    i32 visible;
    i32 pageState;
    float radarScale;
    tag_message currentMessageLocal;
    tag_message radarMessage;
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
            pageState = (message->payload.widget.id - LOCATOR_HERO_SELECT_BASE)
                        / LOCATOR_HERO_WIDGET_STRIDE;
            if (pageState >= gpCurPlayer->m_heroCount) {
                break;
            }
            objectTypeState = gpCurPlayer->m_heroIds[gpCurPlayer->m_heroLocatorPage + pageState];
            if (HAS(message->payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) {
                HeroQuickView(objectTypeState, pageState, -1, -1);
            } else {
                if (gpCurPlayer->CurrentHero() == objectTypeState) {
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
            objectTypeState =
                gpCurPlayer
                    ->m_townIds[gpCurPlayer->m_townLocatorPage + message->payload.widget.id
                               - LOCATOR_TOWN_IMAGE_BASE];
            if (HAS(message->payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) {
                TownQuickView(
                    objectTypeState,
                    message->payload.widget.id - LOCATOR_TOWN_IMAGE_BASE,
                    -1,
                    -1
                );
            } else {
                HideRoute(1, 0, 1);
                if (gpCurPlayer->CurrentTown() == objectTypeState) {
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
                pageState = mouseY
                            / (LOCATOR_SCROLL_MOUSE_SPAN
                               / (gpCurPlayer->m_heroCount
                                  - (LOCATOR_VISIBLE_COUNT - 1)));
                if (pageState > gpCurPlayer->m_heroCount - LOCATOR_VISIBLE_COUNT) {
                    pageState = gpCurPlayer->m_heroCount - LOCATOR_VISIBLE_COUNT;
                }
            } else {
                pageState = 0;
            }
            gpCurPlayer->m_heroLocatorPage = pageState;
            UpdateHeroLocators(1, 1);
            break;
        }
        case LOCATOR_TOWN_KNOB_CONTROL:
            DoTownKnob();
            break;
        case LOCATOR_TOWN_SCROLL_CONTROL: {
            mouseY -= LOCATOR_SCROLL_MOUSE_BASE_Y;
            if (gpCurPlayer->m_townCount > LOCATOR_PAGE_STEP) {
                pageState = mouseY
                            / (LOCATOR_SCROLL_MOUSE_SPAN
                               / (gpCurPlayer->m_townCount
                                  - (LOCATOR_VISIBLE_COUNT - 1)));
                if (pageState > gpCurPlayer->m_townCount - LOCATOR_VISIBLE_COUNT) {
                    pageState = gpCurPlayer->m_townCount - LOCATOR_VISIBLE_COUNT;
                }
            } else {
                pageState = 0;
            }
            gpCurPlayer->m_townLocatorPage = pageState;
            UpdateTownLocators(1, 1);
            break;
        }
        case PANEL_MAP_VIEW: {
            if (!(giCurPlayerBit
                  & (mapExtra
                     + (m_hoverCellY + m_mapOriginY)
                           * MAP_WIDTH)[m_lastHoverCell + m_mapOriginX])) {
                visible = 0;
            }
            currentCell = GetCell(m_lastHoverCell + m_mapOriginX, m_hoverCellY + m_mapOriginY);
            if (HAS(message->payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) {
                if (!visible) {
                    QuickInfo(m_lastHoverCell, m_hoverCellY);
                } else {
                    if (m_lastHoverCell == VIEW_CENTER_CELL && m_hoverCellY == VIEW_CENTER_CELL
                        && gpCurPlayer->CurrentHero() != INVALID_HERO
                        && m_heroContextLocked) {
                        objectTypeState = IDX(MAP_OBJECT_HERO_INTERACTION);
                        objectIdIndex = gpCurPlayer->CurrentHero();
                    } else {
                        objectTypeState = IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK);
                        objectIdIndex = currentCell->m_objectMetadata;
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
                            if (giCurPlayerBit
                                & (mapExtra
                                   + (m_hoverCellY + m_mapOriginY)
                                         * MAP_WIDTH)[m_lastHoverCell + m_mapOriginX]) {
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
                        && gpCurPlayer->CurrentHero() != INVALID_HERO
                        && m_heroContextLocked) {
                        m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                        DoAdvCommand();
                    } else if ((!mobileResult
                                || HAS(
                                    message->payload.widget.modifiers,
                                    MESSAGE_MODIFIER_CONTROL_KEYS
                                )
                                || (gConfig.showRoute
                                    && (m_commandTargetX != currentHero->m_destinationX
                                        || m_commandTargetY != currentHero->m_destinationY)))
                               && (gpSearchArray->GetCell(m_commandTargetX, m_commandTargetY).flags
                                   & 1)) {
                        currentHero->m_destinationX = m_commandTargetX;
                        currentHero->m_destinationY = m_commandTargetY;
                        ShowRoute(1, 1, 1);
                    } else {
                        *eventCell = DoAdvCommand();
                    }
                } else {
                    objectTypeState = IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK);
                    objectIdIndex = currentCell->m_objectMetadata;
                    if (objectTypeState == IDX(MAP_OBJECT_HERO_INTERACTION)) {
                        if (gpCurPlayer->CurrentHero() == objectIdIndex) {
                            m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                            DoAdvCommand();
                        } else if (gpGame->GetHero(objectIdIndex)->m_owner == giCurPlayer) {
                            SetHeroContext(objectIdIndex, 0);
                        }
                    }
                    if (objectTypeState == IDX(MAP_OBJECT_CASTLE)) {
                        if (gpCurPlayer->CurrentTown() == objectIdIndex) {
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
            if (HAS(message->payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) {
                NormalDialog(
                    DATA_COMPGEN(0x004f5adc, processSelectWorldMapAMiniatureViewOf, "{World Map}\n\nA miniature view of the known world.  "
                    "Left click to move viewing area."),
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
            // These are retail's scale payloads for the four supported map dimensions.
            // NOLINTBEGIN(readability-magic-numbers)
            switch (MAP_HEIGHT) {
                case MAP_DIMENSION_SMALL:
                    radarScale = DATA_COMPGEN(0x004eb680, processSelectConstant3, 4.0f);
                    break;
                case MAP_DIMENSION_MEDIUM:
                    radarScale = DATA_COMPGEN(0x004eb684, processSelectConstant, 2.0f);
                    break;
                case MAP_DIMENSION_LARGE:
                    radarScale = DATA_COMPGEN(0x004eb688, processSelectConstant4, 1.3333f);
                    break;
                default:
                    radarScale = DATA_COMPGEN(0x004eb68c, processSelectConstant2, 1.0f);
                    break;
            }
            // NOLINTEND(readability-magic-numbers)
            mouseX = static_cast<i32>((mouseX - RADAR_LEFT) / radarScale);
            mouseY = static_cast<i32>((mouseY - RADAR_TOP) / radarScale);
            m_mapOriginX = mouseX - VIEW_CENTER_CELL;
            m_mapOriginY = mouseY - VIEW_CENTER_CELL;
            if (m_mapOriginX < SCROLL_MIN_ORIGIN) {
                m_mapOriginX = SCROLL_MIN_ORIGIN;
            }
            if (m_mapOriginY < SCROLL_MIN_ORIGIN) {
                m_mapOriginY = SCROLL_MIN_ORIGIN;
            }
            if (!(MAP_WIDTH - VIEW_EDGE_MARGIN >= m_mapOriginX)) {
                m_mapOriginX = MAP_WIDTH - VIEW_EDGE_MARGIN;
            }
            if (!(MAP_HEIGHT - VIEW_EDGE_MARGIN >= m_mapOriginY)) {
                m_mapOriginY = MAP_HEIGHT - VIEW_EDGE_MARGIN;
            }
            UpdateRadar(1, 0);
            CompleteDraw(0);
            UpdateScreen(0, 0);

            currentMessageLocal.type = MESSAGE_NONE;
            while (currentMessageLocal.type != MESSAGE_LEFT_BUTTON_UP) {
                Process1WindowsMessage();
                currentMessageLocal = gpInputManager->GetEvent();
                radarMessage = currentMessageLocal;
                while (currentMessageLocal.type != MESSAGE_LEFT_BUTTON_UP
                       && currentMessageLocal.type != MESSAGE_NONE) {
                    if (currentMessageLocal.type == MESSAGE_MOUSE_MOVE) {
                        radarMessage = currentMessageLocal;
                    }
                    Process1WindowsMessage();
                    currentMessageLocal = gpInputManager->GetEvent();
                }
                if (radarMessage.type == MESSAGE_MOUSE_MOVE) {
                    if (radarMessage.payload.mouse.x < RADAR_LEFT) {
                        radarMessage.payload.mouse.x = RADAR_LEFT;
                    }
                    if (radarMessage.payload.mouse.x >= RADAR_RIGHT) {
                        radarMessage.payload.mouse.x = RADAR_RIGHT - 1;
                    }
                    if (radarMessage.payload.mouse.y < RADAR_TOP) {
                        radarMessage.payload.mouse.y = RADAR_TOP;
                    }
                    if (radarMessage.payload.mouse.y >= RADAR_BOTTOM) {
                        radarMessage.payload.mouse.y = RADAR_BOTTOM - 1;
                    }
                    gpMouseManager->Main(radarMessage);
                    mouseX = static_cast<i32>(
                        (radarMessage.payload.mouse.x - RADAR_LEFT) / radarScale
                    );
                    mouseY = static_cast<i32>(
                        (radarMessage.payload.mouse.y - RADAR_TOP) / radarScale
                    );
                    m_mapOriginX = mouseX - VIEW_CENTER_CELL;
                    m_mapOriginY = mouseY - VIEW_CENTER_CELL;
                    if (m_mapOriginX < SCROLL_MIN_ORIGIN) {
                        m_mapOriginX = SCROLL_MIN_ORIGIN;
                    }
                    if (m_mapOriginY < SCROLL_MIN_ORIGIN) {
                        m_mapOriginY = SCROLL_MIN_ORIGIN;
                    }
                    if (!(MAP_WIDTH - VIEW_EDGE_MARGIN >= m_mapOriginX)) {
                        m_mapOriginX = MAP_WIDTH - VIEW_EDGE_MARGIN;
                    }
                    if (!(MAP_HEIGHT - VIEW_EDGE_MARGIN >= m_mapOriginY)) {
                        m_mapOriginY = MAP_HEIGHT - VIEW_EDGE_MARGIN;
                    }
                    UpdateRadar(1, 0);
                    CompleteDraw(0);
                    UpdateScreen(0, 0);
                    radarMessage.type = MESSAGE_NONE;
                }
            }
            break;
    }

    if ((HAS(message->payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON))
        && message->payload.widget.id >= BOTTOM_VIEW_FIRST_MESSAGE
        && message->payload.widget.id <= BOTTOM_VIEW_LAST_MESSAGE) {
        NormalDialog(
            DATA_COMPGEN(0x004f5b30, processSelectStatusWindowThisWindowProvidesInformation, "{Status Window}\n\nThis window provides information on the status "
            "of your hero or kingdom, and shows the date.  Left click here to "
            "cycle through these windows."),
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

VA(0x00459c19, 0x463)
MessageDispatchResult advManager::ProcessDeSelect(
    struct tag_message* message,
    i32* result,
    class mapCell** eventCell
) {
    switch (message->payload.widget.id) {
        case PANEL_TOWN_PAGE_PREVIOUS:
            if (gpCurPlayer->m_townLocatorPage > 0) {
                --gpCurPlayer->m_townLocatorPage;
                UpdateTownLocators(1, 1);
            }
            break;
        case PANEL_TOWN_PAGE_NEXT:
            if (gpCurPlayer->m_townLocatorPage + LOCATOR_PAGE_STEP
                < gpCurPlayer->m_townCount) {
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
            if (gpCurPlayer->m_heroLocatorPage + LOCATOR_PAGE_STEP
                < gpCurPlayer->m_heroCount) {
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
                    DATA_COMPGEN(0x004f5bd0, processDeSelectOneOrMoreHeroesMayStill, "One or more heroes may still move, are you sure you want to end your turn?"),
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
            if (gbLowMemory) {
                SetEnvironmentOrigin(ENVIRONMENT_ORIGIN_NONE, ENVIRONMENT_ORIGIN_NONE, 1);
            }
            TrimLoopingSounds(0);
            gpGame->Overview();
            i32 fadeAfter = 1;
            if (giOverviewReturnAction == OVERVIEW_RETURN_HERO) {
                gpMouseManager->SetPointer(0);
                if (gbLowMemory) {
                    SetEnvironmentOrigin(ENVIRONMENT_ORIGIN_NONE, ENVIRONMENT_ORIGIN_NONE, 1);
                }
                TrimLoopingSounds(0);
                // Retail uses the action discriminator for this occupied-town lookup;
                // HeroView receives the selected hero ID separately.
                HeroView(
                    giOverviewReturnActionExtra,
                    reinterpret_cast<i32>(
                        gpGame->GetHero(IDX(giOverviewReturnAction))->GetOccupiedTown()
                    ),
                    1
                );
                if (gbLowMemory) {
                    SetEnvironmentOrigin(
                        m_mapOriginX + VIEW_CENTER_OFFSET,
                        m_mapOriginY + VIEW_CENTER_OFFSET,
                        1
                    );
                }
            } else if (giOverviewReturnAction == OVERVIEW_RETURN_TOWN) {
                DemobilizeCurrHero();
                gpMouseManager->SetPointer(0);
                gpGame->GetTown(giOverviewReturnActionExtra)->View(1);
                fadeAfter = 0;
            } else if (gbLowMemory) {
                SetEnvironmentOrigin(
                    m_mapOriginX + VIEW_CENTER_OFFSET,
                    m_mapOriginY + VIEW_CENTER_OFFSET,
                    1
                );
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
        giBottomViewOverrideEndTime = KBTickCount() + BOTTOM_VIEW_DURATION;
        UpdBottomView(1, 1, 1);
    }
    return MESSAGE_DISPATCH_CONSUME;
}

inline hero* GetHeroSlot(i32 i) {
    return &gpGame->m_heroRecs[i];
}

VA(0x0045a07c, 0x5c8)
i32 advManager::ProcessSearch(i32 x, i32 y) {
    mapCell* currentCell;
    i32 playerState;
    tag_message messageValue;
    char specialArtifactValue;
    hero* searchingHeroState;
    SAMPLE2 digSampleState;
    i32 artifactResultLocal;

    digSampleState = NULL_SAMPLE2;
    searchingHeroState = GetHeroSlot(gpCurPlayer->m_currentHero);

    if (searchingHeroState->m_mobility != searchingHeroState->m_remainingMobility) {
        if (!gbHumanPlayer[giCurPlayer]) {
            goto search_end;
        }
        NormalDialog(
            DATA_COMPGEN(0x004f5c1c, processSearchDiggingForArtifactsRequiresAWhole, "Digging for artifacts requires a whole day, try again tomorrow."),
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
    if (searchingHeroState->NumArtifacts() == ARTIFACT_CAPACITY) {
        if (!gbHumanPlayer[giCurPlayer]) {
            goto search_end;
        }
        NormalDialog(
            DATA_COMPGEN(0x004f5c5c, processSearchSearchingForTheUltimateArtifactIs, "Searching for the Ultimate Artifact is fruitless.  Your hero could not carry it even "
            "if he found it - all his artifact slots are full."),
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
    currentCell = GetCell(x, y);
    if (!((currentCell->m_objectIndex == MAPCELL_SPRITE_NONE
           || currentCell->m_objectTileset == TILESET_DUMMY)
          && currentCell->m_overlayIndex == MAPCELL_SPRITE_NONE)) {
        if (!gbHumanPlayer[giCurPlayer]) {
            goto search_end;
        }
        NormalDialog(DATA_COMPGEN(0x004f5ce4, processSearchTrySearchingOnClearGround, "Try searching on clear ground."), 1, -1, -1, -1, 0, -1, 0, -1, 0);
        return 1;
    }
    if (giGroundToTerrain[currentCell->m_terrainImageIndex] == TERRAIN_WATER) {
        if (!gbHumanPlayer[giCurPlayer]) {
            goto search_end;
        }
        NormalDialog(DATA_COMPGEN(0x004f5d04, processSearchTryLookingOnLand, "Try looking on land!!!"), 1, -1, -1, -1, 0, -1, 0, -1, 0);
        return 1;
    }

    if (gbHumanPlayer[giCurPlayer]) {
        digSampleState = LoadPlaySample(DATA_COMPGEN(0x004f5d1c, processSearchDIGSOUND82M, "DIGSOUND.82M"));
    }
    if (currentCell->m_objectIndex == MAPCELL_SPRITE_NONE
        || currentCell->m_objectTileset == TILESET_DUMMY) {
        currentCell->m_objectTileset = TILESET_OBJNDIRT;
        currentCell->m_objectIndex = DIG_HOLE_FRAME;
        currentCell->m_objectLayerBit0 = 1;
        currentCell->m_objectLayerBit1 = 1;
        currentCell->m_flags |= IDX(MAP_CELL_OBJECT_SHADOW_ONLY);
    }
    CompleteDraw(0);
    UpdateScreen(0, 0);

    if (gpGame->m_ultimateArtifactX == x && gpGame->m_ultimateArtifactY == y
        && gpGame->m_ultimateArtifactId != ARTIFACT_NONE) {
        if (searchingHeroState->NumArtifacts() >= ARTIFACT_CAPACITY) {
            if (gbHumanPlayer[giCurPlayer]) {
                NormalDialog(
                    DATA_COMPGEN(0x004f5d2c, processSearchYouHaveNoRoomToCarry, "You have no room to carry another artifact!"),
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
            specialArtifactValue = 0;
            if (xIsPlayingExpansionCampaign) {
                specialArtifactValue = xCampaign.IsSpecialUA();
            }
            if (gbHumanPlayer[giCurPlayer]) {
                gpSoundManager->SwitchAmbientMusic(ULTIMATE_ARTIFACT_MUSIC);
                if (specialArtifactValue) {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x004f5dac, processSearchSS, "%s%s"),
                        DATA_COMPGEN(0x004f5d58, processSearchCongratulationsAfterSpendingManyHoursDigging, "Congratulations! After spending many hours digging here, you have "
                        "uncovered the "),
                        gArtifactNames[IDX(ARTIFACT_SPHERE_NEGATION)]
                    );
                } else {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x004f5e08, processSearchSS2, "%s%s"),
                        DATA_COMPGEN(0x004f5db4, processSearchCongratulationsAfterSpendingManyHoursDigging2, "Congratulations! After spending many hours digging here, you have "
                        "uncovered the "),
                        gArtifactNames[IDX(gpGame->m_ultimateArtifactId)]
                    );
                }
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                if (specialArtifactValue) {
                    searchingHeroState->ViewArtifact(ARTIFACT_SPHERE_NEGATION, 0, -1);
                } else {
                    searchingHeroState->ViewArtifact(gpGame->m_ultimateArtifactId, 0, -1);
                }
                gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
            }
            if (specialArtifactValue) {
                GiveArtifact(searchingHeroState, ARTIFACT_SPHERE_NEGATION, 1, -1);
            } else {
                artifactResultLocal = GiveArtifact(
                    searchingHeroState,
                    gpGame->m_ultimateArtifactId,
                    1,
                    -1
                );
            }
            gpGame->m_ultimateArtifactId = ARTIFACT_NONE;
        }
    } else if (gbHumanPlayer[giCurPlayer]) {
        NormalDialog(DATA_COMPGEN(0x004f5e10, processSearchNothingHereWhereCouldItBe, "Nothing here.\nWhere could it be?"), 1, -1, -1, -1, 0, -1, 0, -1, 0);
    }
    if (gbHumanPlayer[giCurPlayer]) {
        WaitEndSample(digSampleState, -1);
    }
    for (playerState = 0; playerState < gpGame->m_playerCount; ++playerState) {
        ComputeUALoc(playerState);
    }
    searchingHeroState->m_remainingMobility = 0;
    UpdBottomView(1, 1, 1);
    CheckDimHero();
    Reseed(0, 0);
    CheckEndGame(END_GAME_FORCE_NONE, false);
    return 1;
search_end:
    gpCurPlayer->m_ultimateArtifactHintChance = 0;
    return 1;
}

VA(0x0045a644, 0xa50)
MessageDispatchResult advManager::ProcessHover(i32 mouseX, i32 mouseY) {
    i32 heroXHero;
    i32 heroYCoordinate;
    i32 cellXPosition;
    i32 cellYCurrent;
    town* hoverTownCell;
    mapCell* hoverCellLocal;
    i32 routeDaysCount;
    hero* hoverHeroLocal;
    i32 pointerBaseCursor;

    if (InMapArea(mouseX, mouseY)) {
        if (mouseX > DRAW_CLIP_WIDTH) {
            gpMouseManager->SetPointer(POINTER_DEFAULT);
            return MESSAGE_DISPATCH_CONSUME;
        }

        cellXPosition = mouseX / CELL_PIXELS;
        cellYCurrent = mouseY / CELL_PIXELS;
        if (cellXPosition < 0) {
            cellXPosition = 0;
        }
        if (cellYCurrent < 0) {
            cellYCurrent = 0;
        }
        if (cellXPosition > HOVER_MAX_CELL) {
            cellXPosition = HOVER_MAX_CELL;
        }
        if (cellYCurrent > HOVER_MAX_CELL) {
            cellYCurrent = HOVER_MAX_CELL;
        }

        if (m_lastHoverCell != cellXPosition || m_hoverCellY != cellYCurrent) {

            m_selectedCell = ADVMGR_COMMAND_NONE;
            m_lastHoverCell = cellXPosition;
            m_hoverCellY = cellYCurrent;
            m_commandTargetX = m_mapOriginX + cellXPosition;
            m_commandTargetY = m_mapOriginY + cellYCurrent;

            if (m_commandTargetX < 0 || m_commandTargetY < 0 || m_commandTargetX > MAP_WIDTH - 1
                || m_commandTargetY > MAP_HEIGHT - 1
                || !(giCurPlayerBit & mapExtra[m_commandTargetY * MAP_WIDTH + m_commandTargetX])) {
                gpMouseManager->SetPointer(POINTER_DEFAULT);
                return MESSAGE_DISPATCH_CONSUME;
            }

            hoverCellLocal = GetCell(m_commandTargetX, m_commandTargetY);
            if (gpCurPlayer->m_currentHero == INVALID_HERO) {
                if ((hoverCellLocal->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_CASTLE
                    && gpGame->GetTown(hoverCellLocal->m_objectMetadata)->m_owner == giCurPlayer) {
                    gpMouseManager->SetPointer(POINTER_TOWN);
                    m_selectedCell = ADVMGR_COMMAND_TOWN_VIEW;
                    return MESSAGE_DISPATCH_CONSUME;
                } else {
                    if ((hoverCellLocal->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                            == MAP_OBJECT_HERO_INTERACTION
                        && gpGame->GetHero(hoverCellLocal->m_objectMetadata)->m_owner
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
                hoverHeroLocal = gpGame->GetHero(gpCurPlayer->m_currentHero);
                heroXHero = hoverHeroLocal->m_x - m_mapOriginX;
                heroYCoordinate = hoverHeroLocal->m_y - m_mapOriginY;
                if (cellXPosition == heroXHero && cellYCurrent == heroYCoordinate) {
                    gpMouseManager->SetPointer(POINTER_HERO);
                    m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                    return MESSAGE_DISPATCH_CONSUME;
                }

                if (hoverCellLocal->m_flags & HOVER_OBJECT_BLOCKED) {
                    if ((hoverCellLocal->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                        == MAP_OBJECT_CASTLE) {
                        hoverTownCell = gpGame->GetTown(hoverCellLocal->m_objectMetadata);
                        if (hoverTownCell->m_owner == giCurPlayer) {
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
                       || giGroundToTerrain[hoverCellLocal->m_terrainImageIndex] != TERRAIN_WATER
                       || hoverCellLocal->m_triggerType
                              == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                       || hoverCellLocal->m_triggerType
                              == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT)
                       || hoverCellLocal->m_triggerType
                              == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK))
                      && (m_cursorType != HERO_TYPE_BOAT
                          || giGroundToTerrain[hoverCellLocal->m_terrainImageIndex]
                                 == TERRAIN_WATER
                          || hoverCellLocal->m_triggerType == MAP_OBJECT_COAST))) {
                    gpSearchArray->m_pathLength = 0;
                    gpMouseManager->SetPointer(POINTER_DEFAULT);
                    return MESSAGE_DISPATCH_CONSUME;
                }
                SeedTo(m_commandTargetX, m_commandTargetY);
                if (gpSearchArray->GetCell(m_commandTargetX, m_commandTargetY).flags) {
                    if (gpSearchArray->GetCell(m_commandTargetX, m_commandTargetY).previous
                        <= hoverHeroLocal->m_remainingMobility) {
                        routeDaysCount = 0;
                    } else {
                        routeDaysCount =
                            (gpSearchArray->GetCell(m_commandTargetX, m_commandTargetY).previous
                             - hoverHeroLocal->m_remainingMobility)
                                / hoverHeroLocal->m_mobility
                            + 1;
                        if (routeDaysCount > HOVER_ROUTE_DAY_LIMIT) {
                            routeDaysCount = HOVER_ROUTE_DAY_LIMIT;
                        }
                    }
                    pointerBaseCursor = routeDaysCount * HOVER_ROUTE_FRAMES_PER_DAY;

                    switch (hoverCellLocal->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
                        case MAP_OBJECT_BOAT:
                            if (m_cursorType != HERO_TYPE_BOAT) {
                                gpMouseManager->SetPointer(pointerBaseCursor + POINTER_SAIL);
                                m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            } else {
                                gpMouseManager->SetPointer(pointerBaseCursor);
                            }
                            break;
                        case MAP_OBJECT_COAST:
                            if (m_cursorType == HERO_TYPE_BOAT) {
                                gpMouseManager->SetPointer(
                                    pointerBaseCursor + POINTER_DISEMBARK
                                );
                            } else if (mapExtra[m_commandTargetY * MAP_WIDTH + m_commandTargetX]
                                       & HOVER_UNREACHABLE) {
                                gpMouseManager->SetPointer(
                                    pointerBaseCursor + POINTER_ATTACK
                                );
                            } else {
                                gpMouseManager->SetPointer(pointerBaseCursor + POINTER_MOVE);
                            }
                            m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            break;
                        case MAP_OBJECT_MONSTER:
                            gpMouseManager->SetPointer(pointerBaseCursor + POINTER_ATTACK);
                            m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            break;
                        case MAP_OBJECT_HERO_INTERACTION:
                            if (gpGame->GetHero(hoverCellLocal->m_objectMetadata)->m_owner
                                != giCurPlayer) {
                                gpMouseManager->SetPointer(
                                    pointerBaseCursor + POINTER_ATTACK
                                );
                                m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            } else {
                                gpMouseManager->SetPointer(
                                    pointerBaseCursor + POINTER_SELECT_HERO
                                );
                                m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            }
                            break;
                        case MAP_OBJECT_CASTLE:
                            hoverTownCell = gpGame->GetTown(hoverCellLocal->m_objectMetadata);
                            if (HAS(hoverCellLocal->m_triggerType, MAP_TRIGGER_ACTION_FLAG)
                                && hoverTownCell->m_owner != giCurPlayer
                                && hoverTownCell->HasGarrison()) {
                                gpMouseManager->SetPointer(
                                    pointerBaseCursor + POINTER_ATTACK
                                );
                                m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                                break;
                            }
                            goto process_default_hover;
                        default:
                        process_default_hover:
                            if (!((mapExtra[m_commandTargetY * MAP_WIDTH + m_commandTargetX]
                                   & HOVER_UNREACHABLE)
                                  && m_cursorType != HERO_TYPE_BOAT
                                  && (!(hoverCellLocal->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                                      || !StopOnTrigger(hoverCellLocal))
                                  && (gpMouseManager->SetPointer(
                                          pointerBaseCursor + POINTER_ATTACK
                                      ),
                                      1))) {
                                if (HAS(hoverCellLocal->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                                    if (m_cursorType != HERO_TYPE_BOAT) {
                                        if (giGroundToTerrain[hoverCellLocal
                                                                  ->m_terrainImageIndex]
                                            != TERRAIN_WATER) {
                                            gpMouseManager->SetPointer(
                                                pointerBaseCursor + POINTER_ACTION
                                            );
                                        } else if (hoverCellLocal->m_triggerType
                                                   == (MAP_TRIGGER_ACTION_FLAG
                                                       | MAP_OBJECT_SHIPWRECK)) {
                                            gpMouseManager->SetPointer(
                                                pointerBaseCursor + POINTER_ACTION
                                            );
                                        } else {
                                            gpMouseManager->SetPointer(
                                                pointerBaseCursor + POINTER_MOVE
                                            );
                                            break;
                                        }
                                    } else {
                                        if (giGroundToTerrain[hoverCellLocal
                                                                  ->m_terrainImageIndex]
                                            == TERRAIN_WATER) {
                                            gpMouseManager->SetPointer(
                                                routeDaysCount + POINTER_WATER_ACTION
                                            );
                                        } else {
                                            gpMouseManager->SetPointer(
                                                pointerBaseCursor + POINTER_SAIL
                                            );
                                        }
                                    }
                                } else {
                                    if (m_cursorType == HERO_TYPE_BOAT) {
                                        gpMouseManager->SetPointer(
                                            pointerBaseCursor + POINTER_SAIL
                                        );
                                    } else {
                                        gpMouseManager->SetPointer(
                                            pointerBaseCursor + POINTER_MOVE
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
                }
            }
        }
        return MESSAGE_DISPATCH_CONSUME;
    } else {
        if (!(gpMouseManager->m_cursorFrame >= HOVER_SCROLL_FRAME_FIRST
              && gpMouseManager->m_cursorFrame < HOVER_SCROLL_FRAME_END
              && MouseInScrollZone())) {
            gpMouseManager->SetPointer(POINTER_DEFAULT);
        }
        return MESSAGE_DISPATCH_CONSUME;
    }
}

VA(0x0045b094, 0x21a)
void advManager::UpdateScreen(i32, i32 forceUpdate) {
    if (forceUpdate == 0 && bShowIt == 0) {
        if (KBTickCount() > glTimers[0]) {
            glTimers[0] = KBTickCount() + TIMER_DELAY;
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

    if (KBTickCount() > glTimers[0]) {
        ++m_updateMaxY;
        ++m_updateMaxX;
        if (m_updateMaxX >= UPDATE_ANIMATION_PHASES) {
            m_updateMaxX = 0;
        }
        glTimers[0] = KBTickCount() + TIMER_DELAY;

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
    Process1WindowsMessage();
}

VA(0x0045b2ae, 0x4eb)
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
        m_mapOriginX = m_mapOriginY;
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

VA(0x0045b799, 0x3a)
void advManager::CompleteDraw(i32 update) {
    CompleteDraw(m_mapOriginX, m_mapOriginY, update, 1);
}

VA(0x0045b7d3, 0x3a9)
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
        if ((giCurWatchPlayerBit & mapExtra[(y - 1) * MAP_WIDTH + x]) == 0) {
            cloudMask |= CLOUD_NORTH;
        }
        if ((giCurWatchPlayerBit & mapExtra[MAP_WIDTH * y + x + 1]) == 0) {
            cloudMask |= CLOUD_EAST;
        }
        if ((giCurWatchPlayerBit & mapExtra[(y + 1) * MAP_WIDTH + x]) == 0) {
            cloudMask |= CLOUD_SOUTH;
        }
        if ((giCurWatchPlayerBit & (x + mapExtra - 1)[y * MAP_WIDTH]) == 0) {
            cloudMask |= CLOUD_WEST;
        }
        if ((giCurWatchPlayerBit & mapExtra[(y - 1) * MAP_WIDTH + x + 1]) == 0) {
            cloudMask |= CLOUD_NORTH_EAST;
        }
        if ((giCurWatchPlayerBit & mapExtra[(y + 1) * MAP_WIDTH + x + 1]) == 0) {
            cloudMask |= CLOUD_SOUTH_EAST;
        }
        if ((giCurWatchPlayerBit & (x + mapExtra - 1)[(y + 1) * MAP_WIDTH]) == 0) {
            cloudMask |= CLOUD_SOUTH_WEST;
        }
        if ((giCurWatchPlayerBit & (x + mapExtra - 1)[(y - 1) * MAP_WIDTH]) == 0) {
            cloudMask |= CLOUD_NORTH_WEST;
        }
    } else {
        if ((cloudMask & CLOUD_NORTH) == 0
            && (giCurWatchPlayerBit & mapExtra[(y - 1) * MAP_WIDTH + x]) == 0) {
            cloudMask |= CLOUD_NORTH;
        }
        if ((cloudMask & CLOUD_EAST) == 0
            && (giCurWatchPlayerBit & mapExtra[MAP_WIDTH * y + x + 1]) == 0) {
            cloudMask |= CLOUD_EAST;
        }
        if ((cloudMask & CLOUD_SOUTH) == 0
            && (giCurWatchPlayerBit & mapExtra[(y + 1) * MAP_WIDTH + x]) == 0) {
            cloudMask |= CLOUD_SOUTH;
        }
        if ((cloudMask & CLOUD_WEST) == 0
            && (giCurWatchPlayerBit & (x + mapExtra - 1)[y * MAP_WIDTH]) == 0) {
            cloudMask |= CLOUD_WEST;
        }
        if ((cloudMask & CLOUD_NORTH_EAST) == 0
            && (giCurWatchPlayerBit & mapExtra[(y - 1) * MAP_WIDTH + x + 1]) == 0) {
            cloudMask |= CLOUD_NORTH_EAST;
        }
        if ((cloudMask & CLOUD_SOUTH_EAST) == 0
            && (giCurWatchPlayerBit & mapExtra[(y + 1) * MAP_WIDTH + x + 1]) == 0) {
            cloudMask |= CLOUD_SOUTH_EAST;
        }
        if ((cloudMask & CLOUD_SOUTH_WEST) == 0
            && (giCurWatchPlayerBit & (x + mapExtra - 1)[(y + 1) * MAP_WIDTH]) == 0) {
            cloudMask |= CLOUD_SOUTH_WEST;
        }
        if ((cloudMask & CLOUD_NORTH_WEST) == 0
            && (giCurWatchPlayerBit & (x + mapExtra - 1)[(y - 1) * MAP_WIDTH]) == 0) {
            cloudMask |= CLOUD_NORTH_WEST;
        }
    }
    return giCloudType[cloudMask];
}

VA(0x0045bb7c, 0x24cb)
void advManager::DrawCell(
    i32 mapX,
    i32 mapY,
    i32 screenX,
    i32 screenY,
    AdventureDrawMask drawMask,
    i32 forceDraw
) {
    i32 animFrame;
    i32 heroShadowOffset;
    i32 cursorFrame;
    i32 boatFrameIndex;
    i32 boatShadowFrameOffset;
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
                s_drawStoneTile = (mapY & CLOUD_VARIANT_MASK) + STONE_TILE_LEFT_BASE;
            }
        } else if (mapX == MAP_WIDTH) {
            if (mapY == -1) {
                s_drawStoneTile = STONE_TILE_TOP_RIGHT;
            } else if (mapY == MAP_HEIGHT) {
                s_drawStoneTile = STONE_TILE_BOTTOM_RIGHT;
            } else if (mapY >= 0 && mapY < MAP_HEIGHT) {
                s_drawStoneTile = (mapY & CLOUD_VARIANT_MASK) + STONE_TILE_RIGHT_BASE;
            }
        } else if (mapY == -1) {
            if (mapX >= 0 && mapX < MAP_WIDTH) {
                s_drawStoneTile = (mapX & CLOUD_VARIANT_MASK) + STONE_TILE_TOP_BASE;
            }
        } else if (mapY == MAP_HEIGHT && mapX >= 0 && mapX < MAP_WIDTH) {
            s_drawStoneTile = (mapX & CLOUD_VARIANT_MASK) + STONE_TILE_BOTTOM_BASE;
        }
        if (s_drawStoneTile == STONE_TILE_NONE) {
            s_drawStoneTile =
                ((mapY + STONE_PATTERN_COORDINATE_SHIFT) % CLOUD_VARIANTS) * CLOUD_VARIANTS
                + (mapX + STONE_PATTERN_COORDINATE_SHIFT) % CLOUD_VARIANTS;
        }
        TileToBitmap(
            m_stoneTiles,
            s_drawStoneTile,
            gpWindowManager->m_screen,
            s_drawPixelX,
            s_drawPixelY
        );
    } else {

        if (!(((gbAllBlack == 0 && (giCurWatchPlayerBit & mapExtra[mapY * MAP_WIDTH + mapX]) != 0)
               || gbDrawingPuzzle != 0))) {
            s_drawCovered = 1;
            if (gbAllBlack != 0) {
                s_drawCloudFrame = 0;
            } else {
                s_drawCloudFrame = GetCloudLookup(mapX, mapY);
            }
            if (s_drawCloudFrame == 0) {
                if (HAS(drawMask, ADVMGR_DRAW_CLOUD)) {
                    TileToBitmap(
                        m_cloudTiles,
                        (mapY + mapX) & CLOUD_VARIANT_MASK,
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

        if (HAS(drawMask, ADVMGR_DRAW_CLOUD) && !gbDrawingPuzzle) {
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
            } else if (m_visibilityMapValid && m_visibilityMap[mapY * MAP_WIDTH + mapX] != 0) {
                if ((m_visibilityMap[mapY * MAP_WIDTH + mapX] & ROUTE_BEYOND_MOBILITY_FLAG)
                    != 0) {
                    IconToBitmapColorTable(
                        m_objectIcons[IDX(TILESET_ROUTE)],
                        gpWindowManager->m_screen,
                        s_drawPixelX - ROUTE_DRAW_X_OFFSET,
                        s_drawPixelY + ROUTE_DRAW_Y_OFFSET,
                        (m_visibilityMap[mapY * MAP_WIDTH + mapX] - 1) & ROUTE_FRAME_MASK,
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
                        m_objectIcons[IDX(TILESET_ROUTE)],
                        gpWindowManager->m_screen,
                        s_drawPixelX - ROUTE_DRAW_X_OFFSET,
                        s_drawPixelY + ROUTE_DRAW_Y_OFFSET,
                        (m_visibilityMap[mapY * MAP_WIDTH + mapX] - 1) & ROUTE_FRAME_MASK,
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
            if (HAS(drawMask, ADVMGR_DRAW_GROUND)) {
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
                    && (gbDrawingPuzzle == 0 || bPuzzleDraw[IDX(s_drawCell->m_objectTileset)])) {
                    IconToBitmap(
                        m_objectIcons[IDX(s_drawCell->m_objectTileset)],
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
                                                    m_objectIcons[IDX(s_drawCell->m_objectTileset)],
                                                    s_drawCell->m_objectIndex
                        )
                                                    ->flags;
                        IconToBitmap(
                            m_objectIcons[IDX(s_drawCell->m_objectTileset)],
                            gpWindowManager->m_screen,
                            s_drawPixelX,
                            s_drawPixelY,
                            m_updateMaxY % s_drawAnimationLength + s_drawCell->m_objectIndex + 1,
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
                        && (gbDrawingPuzzle == 0 || bPuzzleDraw[IDX(s_drawExtra->objectTileset)])) {
                        IconToBitmap(
                            m_objectIcons[IDX(s_drawExtra->objectTileset)],
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
                            s_drawAnimationLength = GetIconEntry(
                                                        m_objectIcons[IDX(s_drawExtra->objectTileset)],
                                                        s_drawExtra->objectIndex
                            )
                                                        ->flags;
                            IconToBitmap(
                                m_objectIcons[IDX(s_drawExtra->objectTileset)],
                                gpWindowManager->m_screen,
                                s_drawPixelX,
                                s_drawPixelY,
                                m_updateMaxY % s_drawAnimationLength + s_drawExtra->objectIndex + 1,
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
                    && (gbDrawingPuzzle == 0 || bPuzzleDraw[IDX(s_drawCell->m_objectTileset)])) {
                    IconToBitmap(
                        m_objectIcons[IDX(s_drawCell->m_objectTileset)],
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
                                                    m_objectIcons[IDX(s_drawCell->m_objectTileset)],
                                                    s_drawCell->m_objectIndex
                        )
                                                    ->flags;
                        IconToBitmap(
                            m_objectIcons[IDX(s_drawCell->m_objectTileset)],
                            gpWindowManager->m_screen,
                            s_drawPixelX,
                            s_drawPixelY,
                            m_updateMaxY % s_drawAnimationLength + s_drawCell->m_objectIndex + 1,
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
                        && (gbDrawingPuzzle == 0 || bPuzzleDraw[IDX(s_drawExtra->objectTileset)])) {
                        IconToBitmap(
                            m_objectIcons[IDX(s_drawExtra->objectTileset)],
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
                            s_drawAnimationLength = GetIconEntry(
                                                        m_objectIcons[IDX(s_drawExtra->objectTileset)],
                                                        s_drawExtra->objectIndex
                            )
                                                        ->flags;
                            IconToBitmap(
                                m_objectIcons[IDX(s_drawExtra->objectTileset)],
                                gpWindowManager->m_screen,
                                s_drawPixelX,
                                s_drawPixelY,
                                m_updateMaxY % s_drawAnimationLength + s_drawExtra->objectIndex + 1,
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

            if (HAS(drawMask, ADVMGR_DRAW_OBJECT)) {
                if (s_drawCell->m_objectIndex != MAPCELL_SPRITE_NONE
                    && !s_drawCell->m_objectLayerBit0 && !s_drawCell->m_objectLayerBit1
                    && !s_drawCell->m_objectDrawnAsOverlay
                    && s_drawCell->m_objectTileset != TILESET_MONS32
                    && (gbDrawingPuzzle == 0 || bPuzzleDraw[IDX(s_drawCell->m_objectTileset)])) {
                    IconToBitmap(
                        m_objectIcons[IDX(s_drawCell->m_objectTileset)],
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
                                                    m_objectIcons[IDX(s_drawCell->m_objectTileset)],
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
                            m_objectIcons[IDX(s_drawCell->m_objectTileset)],
                            gpWindowManager->m_screen,
                            s_drawPixelX,
                            s_drawPixelY,
                            animFrame + s_drawCell->m_objectIndex + 1,
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
                        && (gbDrawingPuzzle == 0 || bPuzzleDraw[IDX(s_drawExtra->objectTileset)])) {
                        IconToBitmap(
                            m_objectIcons[IDX(s_drawExtra->objectTileset)],
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
                            s_drawAnimationLength = GetIconEntry(
                                                        m_objectIcons[IDX(s_drawExtra->objectTileset)],
                                                        s_drawExtra->objectIndex
                            )
                                                        ->flags;
                            IconToBitmap(
                                m_objectIcons[IDX(s_drawExtra->objectTileset)],
                                gpWindowManager->m_screen,
                                s_drawPixelX,
                                s_drawPixelY,
                                m_updateMaxY % s_drawAnimationLength + s_drawExtra->objectIndex + 1,
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

            if ((HAS(drawMask, ADVMGR_DRAW_HERO) || HAS(drawMask, ADVMGR_DRAW_HERO_SHADOW))
                && gbDrawingPuzzle == 0) {
                s_drawHasHero = 0;
                s_drawHero = NULL;
                if (HAS(drawMask, ADVMGR_DRAW_HERO)) {
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
                                    (m_updateMaxY + mapY + mapX) % MINE_GHOST_FRAME_COUNT,
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
                                    IDX(s_drawMine->guardianType) - IDX(CREATURE_EARTH_ELEMENTAL),
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
                        if (m_lastQuickViewX == mapX && m_lastQuickViewY == mapY) {
                            IconToBitmap(
                                m_objectIcons[TILESET_MONSTER],
                                gpWindowManager->m_screen,
                                s_drawPixelX + CELL_CENTER_PIXEL,
                                s_drawPixelY + OBJECT_BASELINE_Y,
                                (MONSTER_REVERSED_FRAME - (m_mineGuardianFacingLeft == 0))
                                    + s_drawCell->m_objectIndex * MONSTER_FRAME_STRIDE,
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
                                s_drawCell->m_objectIndex * MONSTER_FRAME_STRIDE + s_drawMonsterFrame
                                    + MONSTER_ANIMATION_FRAME_OFFSET,
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
                        if (HAS(s_drawHero->m_eventFlags, HERO_EVENT_EMBARKED)) {
                            s_drawHeroType = HERO_TYPE_BOAT;
                        } else {
                            s_drawHeroType = s_drawHero->m_cursorType;
                        }
                        s_drawHeroFrame = GetCursorBaseFrame(s_drawHero->m_direction);
                        s_drawHasHero = 1;
                        if (HAS(s_drawHero->m_eventFlags, HERO_EVENT_EMBARKED)) {
                            s_drawHeroYOffset = HERO_BOAT_Y_OFFSET;
                        }
                    }
                }

                if (s_drawHasHero) {
                    if (s_drawHeroFrame & HERO_FRAME_MIRROR_FLAG) {
                        if (HAS(drawMask, ADVMGR_DRAW_HERO_SHADOW)) {
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
                                if (cursorFrame >= HERO_SHADOW_FRAME_FIRST
                                    && cursorFrame < HERO_SHADOW_FRAME_END) {
                                    heroShadowOffset = HERO_SHADOW_FRAME_OFFSET;
                                } else {
                                    heroShadowOffset = 0;
                                }
                                IconToBitmap(
                                    m_shadowIcon,
                                    gpWindowManager->m_screen,
                                    s_drawPixelX,
                                    s_drawPixelY + CELL_LAST_PIXEL,
                                    cursorFrame + heroShadowOffset,
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
                                if (boatFrameIndex >= HERO_SHADOW_FRAME_FIRST
                                    && boatFrameIndex < HERO_SHADOW_FRAME_END) {
                                    boatShadowFrameOffset = HERO_SHADOW_FRAME_END;
                                } else {
                                    boatShadowFrameOffset = 0;
                                }
                                IconToBitmap(
                                    m_boatShadowIcon,
                                    gpWindowManager->m_screen,
                                    s_drawPixelX,
                                    s_drawHeroYOffset + s_drawPixelY + CELL_LAST_PIXEL,
                                    boatFrameIndex + boatShadowFrameOffset,
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
                                    s_drawHeroYOffset + s_drawPixelY + CELL_LAST_PIXEL,
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
                                m_heroIcons[IDX(s_drawHeroType)],
                                gpWindowManager->m_screen,
                                s_drawPixelX + CELL_PIXELS,
                                s_drawHeroYOffset + s_drawPixelY + CELL_LAST_PIXEL,
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
                                        s_drawHeroYOffset + s_drawPixelY + CELL_LAST_PIXEL,
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
                                        m_updateMaxY % PLAYER_FLAG_FRAME_COUNT
                                            + (s_drawHeroFrame & HERO_FRAME_INDEX_MASK)
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
                        if (HAS(drawMask, ADVMGR_DRAW_HERO_SHADOW)) {
                            if (m_drawHeroShadows != 0 && s_drawHeroType != HERO_TYPE_BOAT
                                && HAS(drawMask, ADVMGR_DRAW_HERO_SHADOW)) {
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
                                    s_drawHeroYOffset + s_drawPixelY + CELL_LAST_PIXEL,
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
                                    s_drawHeroYOffset + s_drawPixelY + CELL_LAST_PIXEL,
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
                                m_heroIcons[IDX(s_drawHeroType)],
                                gpWindowManager->m_screen,
                                s_drawPixelX,
                                s_drawHeroYOffset + s_drawPixelY + CELL_LAST_PIXEL,
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
                                        s_drawHeroYOffset + s_drawPixelY + CELL_LAST_PIXEL,
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
                                        m_updateMaxY % PLAYER_FLAG_FRAME_COUNT
                                            + (s_drawHeroFrame & HERO_FRAME_INDEX_MASK)
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
                    && (m_comboHeroDrawn == 0 || HAS(drawMask, ADVMGR_DRAW_HERO_SHADOW))
                    && m_mapOriginX + VIEW_CENTER_CELL == mapX
                    && m_mapOriginY + VIEW_CENTER_CELL == mapY) {
                    if (HAS(drawMask, ADVMGR_DRAW_HERO_SHADOW)) {
                        cursorSuppressed = 1;
                    } else {
                        DrawCursorShadow();
                        DrawCursor();
                        m_comboHeroDrawn = 1;
                    }
                }
            }

            if (HAS(drawMask, ADVMGR_DRAW_OVERLAY) || HAS(drawMask, ADVMGR_DRAW_OVERLAY_TOP)) {
                if (HAS(drawMask, ADVMGR_DRAW_OVERLAY)
                    && s_drawCell->m_objectIndex != MAPCELL_SPRITE_NONE
                    && s_drawCell->m_objectDrawnAsOverlay
                    && (gbDrawingPuzzle == 0 || bPuzzleDraw[IDX(s_drawCell->m_objectTileset)])) {
                    IconToBitmap(
                        m_objectIcons[IDX(s_drawCell->m_objectTileset)],
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
                                                    m_objectIcons[IDX(s_drawCell->m_objectTileset)],
                                                    s_drawCell->m_objectIndex
                        )
                                                    ->flags;
                        IconToBitmap(
                            m_objectIcons[IDX(s_drawCell->m_objectTileset)],
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
                    if (s_drawExtra->objectDrawnAsOverlay
                        && (gbDrawingPuzzle == 0 || bPuzzleDraw[IDX(s_drawExtra->objectTileset)])) {
                        IconToBitmap(
                            m_objectIcons[IDX(s_drawExtra->objectTileset)],
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
                            s_drawAnimationLength = GetIconEntry(
                                                        m_objectIcons[IDX(s_drawExtra->objectTileset)],
                                                        s_drawExtra->objectIndex
                            )
                                                        ->flags;
                            IconToBitmap(
                                m_objectIcons[IDX(s_drawExtra->objectTileset)],
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

                if (s_drawCell->m_overlayIndex != MAPCELL_SPRITE_NONE
                    && ((HAS(drawMask, ADVMGR_DRAW_OVERLAY) && !s_drawCell->m_drawOverlayOnTop)
                        || (HAS(drawMask, ADVMGR_DRAW_OVERLAY_TOP)
                            && s_drawCell->m_drawOverlayOnTop))
                    && (gbDrawingPuzzle == 0 || bPuzzleDraw[IDX(s_drawCell->m_overlayTileset)])) {
                    IconToBitmap(
                        m_objectIcons[IDX(s_drawCell->m_overlayTileset)],
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
                        s_drawAnimationLength = GetIconEntry(
                                                    m_objectIcons[IDX(s_drawCell->m_overlayTileset)],
                                                    s_drawCell->m_overlayIndex
                        )
                                                    ->flags;
                        IconToBitmap(
                            m_objectIcons[IDX(s_drawCell->m_overlayTileset)],
                            gpWindowManager->m_screen,
                            s_drawPixelX,
                            s_drawPixelY,
                            m_updateMaxY % s_drawAnimationLength + s_drawCell->m_overlayIndex + 1,
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
                    if ((HAS(drawMask, ADVMGR_DRAW_OVERLAY) && !s_drawExtra->drawOverlayOnTop)
                        || (HAS(drawMask, ADVMGR_DRAW_OVERLAY_TOP)
                            && s_drawExtra->drawOverlayOnTop)) {
                        if (gbDrawingPuzzle == 0 || bPuzzleDraw[IDX(s_drawExtra->overlayTileset)]) {
                            IconToBitmap(
                                m_objectIcons[IDX(s_drawExtra->overlayTileset)],
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
                                        m_objectIcons[IDX(s_drawExtra->overlayTileset)],
                                        s_drawExtra->overlayIndex
                                    )
                                        ->flags;
                                IconToBitmap(
                                    m_objectIcons[IDX(s_drawExtra->overlayTileset)],
                                    gpWindowManager->m_screen,
                                    s_drawPixelX,
                                    s_drawPixelY,
                                    m_updateMaxY % s_drawAnimationLength + s_drawExtra->overlayIndex
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

VA(0x0045e047, 0x93)
class mapCell* advManager::GetCell(i32 x, i32 y) {
    if (x < 0 || y < 0 || x >= MAP_WIDTH || MAP_HEIGHT <= y) {
        return m_mapData->Cells();
    } else {
        return &m_mapData->Row(y)[x];
    }
}

VA(0x0045e0da, 0x104d)
void advManager::UpdateRadar(i32 updateScreen, i32 partial) {
    i32 maxXLocal;
    i32 unusedRadarRowLocal;
    i32 radarFrameLocal;
    u8 radarColorValue = RADAR_UNSEEN_COLOR;
    float radarScaleState;
    i32 townXValue;
    i32 maxYLocal;
    i32 unusedRadarCoordinateLocal;
    i32 townYValue;
    i32 rowRemainderState;
    i32 unusedRadarDimensionValue;
    i32 unusedRadarColumnState;
    i32 mapRow;
    i32 mapColumnLimit;
    i32 ownerIndexValue;
    u8* radarPixel;
    mapCell* cellValue;
    i32 screenColumnIndex;
    u8* screenRowOffset;
    i32 columnRemainderValue;
    i32 minYOffset;
    i32 minXSlot;
    i32 skipFrameIndex;
    H2_ENUM_STORAGE(TilesetId, i32) objectTilesetLocal;


    if (partial == 0) {
        minXSlot = 0;
        minYOffset = 0;
        maxXLocal = MAP_WIDTH - 1;
        maxYLocal = MAP_HEIGHT - 1;
    } else {
        minXSlot = m_mapOriginX - RADAR_PARTIAL_MARGIN;
        minYOffset = m_mapOriginY - RADAR_PARTIAL_MARGIN;
        maxXLocal = m_mapOriginX + RADAR_PARTIAL_SPAN;
        maxYLocal = m_mapOriginY + RADAR_PARTIAL_SPAN;
        if (minXSlot < 0) {
            minXSlot = 0;
        }
        if (minYOffset < 0) {
            minYOffset = 0;
        }
        if (maxXLocal > MAP_WIDTH - 1) {
            maxXLocal = MAP_WIDTH - 1;
        }
        if (maxYLocal > MAP_HEIGHT - 1) {
            maxYLocal = MAP_HEIGHT - 1;
        }
    }

    if (gbThisNetHumanPlayer[giCurPlayer] == 0) {
        return;
    }

    gpAdvManager->m_openState = 0;
    columnRemainderValue = rowRemainderState = 0;
    switch (MAP_HEIGHT) {
        case MAP_DIMENSION_SMALL:
            screenRowOffset = gpWindowManager->m_screen->m_pixels
                              + (minYOffset * RADAR_SMALL_CELL_PIXELS + RADAR_TOP)
                                    * RADAR_ROW_GROUPS
                                    * RADAR_GROUP_BYTES
                              + RADAR_LEFT;
            screenColumnIndex = minXSlot * RADAR_SMALL_CELL_PIXELS;
            break;
        case MAP_DIMENSION_MEDIUM:
            screenRowOffset = gpWindowManager->m_screen->m_pixels
                              + (minYOffset * RADAR_MEDIUM_CELL_PIXELS + RADAR_TOP)
                                    * RADAR_ROW_GROUPS
                                    * RADAR_GROUP_BYTES
                              + RADAR_LEFT;
            screenColumnIndex = minXSlot * RADAR_MEDIUM_CELL_PIXELS;
            break;
        case MAP_DIMENSION_LARGE:
            screenRowOffset = gpWindowManager->m_screen->m_pixels
                              + (minYOffset
                                 + (minYOffset + RADAR_LARGE_SCALE_ROUNDING)
                                       / RADAR_LARGE_SCALE_DIVISOR
                                 + RADAR_TOP)
                                    * RADAR_ROW_GROUPS * RADAR_GROUP_BYTES
                              + RADAR_LEFT;
            screenColumnIndex = minXSlot
                + (minXSlot + RADAR_LARGE_SCALE_ROUNDING) / RADAR_LARGE_SCALE_DIVISOR;
            columnRemainderValue = minXSlot % RADAR_LARGE_SCALE_DIVISOR;
            rowRemainderState = minYOffset % RADAR_LARGE_SCALE_DIVISOR;
            break;
        default:
            screenRowOffset = &gpWindowManager->m_screen->m_pixels
                                   [(minYOffset + RADAR_TOP) * RADAR_ROW_GROUPS
                                    * RADAR_GROUP_BYTES]
                              + RADAR_LEFT;
            screenColumnIndex = minXSlot;
            break;
    }

    for (mapRow = minYOffset; !(maxYLocal < mapRow); ++mapRow) {
        radarPixel = screenRowOffset + screenColumnIndex;
        switch (MAP_HEIGHT) {
            case MAP_DIMENSION_SMALL:
                screenRowOffset += RADAR_SCREEN_PITCH * RADAR_SMALL_CELL_PIXELS;
                break;
            case MAP_DIMENSION_MEDIUM:
                screenRowOffset += RADAR_SCREEN_PITCH * RADAR_MEDIUM_CELL_PIXELS;
                break;
            case MAP_DIMENSION_LARGE:
                ++rowRemainderState;
                if (rowRemainderState >= RADAR_LARGE_SCALE_DIVISOR) {
                    rowRemainderState = 0;
                }
                if (rowRemainderState != 0) {
                    screenRowOffset += RADAR_SCREEN_PITCH;
                } else {
                    screenRowOffset += RADAR_SCREEN_PITCH * RADAR_MEDIUM_CELL_PIXELS;
                }
                break;
            case MAP_DIMENSION_XLARGE:
                screenRowOffset += RADAR_SCREEN_PITCH;
                break;
        }

        for (mapColumnLimit = minXSlot; mapColumnLimit <= maxXLocal; ++mapColumnLimit) {
            if (gbAllBlack != 0
                || (giCurPlayerBit & mapExtra[mapRow * MAP_WIDTH + mapColumnLimit]) == 0) {
                radarColorValue = RADAR_UNSEEN_COLOR;
            } else {
                cellValue = &m_mapData->Row(mapRow)[mapColumnLimit];
                if ((cellValue->m_flags & CURSOR_MAP_VISIBLE_FLAG) != 0
                    && m_mapOriginX + RADAR_CURRENT_CELL == mapColumnLimit
                    && m_mapOriginY + RADAR_CURRENT_CELL == mapRow) {
                    radarColorValue = gOwnerColors[gpGame->m_players[giCurPlayer].m_color];
                } else {
                    if ((cellValue->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                        == MAP_OBJECT_HERO_INTERACTION) {
                        ownerIndexValue = gpGame->m_availableHeroes[cellValue->m_objectMetadata];
                        if (!(giCurPlayer != ownerIndexValue)) {
                            i32 ownerColorIndex;
                            if (ownerIndexValue >= 0) {
                                ownerColorIndex = gpGame->m_players[ownerIndexValue].m_color;
                            } else {
                                ownerColorIndex = RADAR_NEUTRAL_OWNER;
                            }
                            radarColorValue = gOwnerColors[ownerColorIndex];
                        }
                    } else {
                        objectTilesetLocal = -1;
                        if (cellValue->m_objectIndex != MAPCELL_SPRITE_NONE) {
                            objectTilesetLocal = cellValue->m_objectTileset;
                        } else if (cellValue->m_overlayIndex != MAPCELL_SPRITE_NONE) {
                            objectTilesetLocal = cellValue->m_overlayTileset;
                        }

                        if (cellValue->m_triggerType == MAP_OBJECT_CASTLE
                            || (objectTilesetLocal == TILESET_FLAG32 && mapColumnLimit > 0
                                && mapColumnLimit < MAP_WIDTH - 1
                                && m_mapData->Row(mapRow)[mapColumnLimit - 1].m_triggerType
                                       == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE))
                            || m_mapData->Row(mapRow)[mapColumnLimit + 1].m_triggerType
                                   == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
                            objectTilesetLocal = TILESET_OBJNTOWN;
                        }

                        if (objectTilesetLocal == TILESET_X_LOC2
                            && cellValue->m_triggerType == MAP_OBJECT_ROCK) {
                            radarColorValue =
                                gMapColors[IDX(giGroundToTerrain
                                                   [cellValue->m_terrainImageIndex])]
                                + RADAR_TERRAIN_SHADE;
                        } else {
                            switch (objectTilesetLocal) {
                                case TILESET_OBJNTOWN:
                                case TILESET_OBJNTWBA: {
                                    i32 ownerColorIndex;
                                    ownerIndexValue =
                                        gpGame->m_townOwners[cellValue->m_objectMetadata];
                                    townXValue = gpGame->GetTown(cellValue->m_objectMetadata)->m_x;
                                    townYValue = gpGame->GetTown(cellValue->m_objectMetadata)->m_y;
                                    if (ownerIndexValue >= 0) {
                                        ownerColorIndex =
                                            gpGame->m_players[ownerIndexValue].m_color;
                                    } else {
                                        ownerColorIndex = RADAR_NEUTRAL_OWNER;
                                    }
                                    radarColorValue = gOwnerColors[ownerColorIndex];
                                    if (mapRow < townYValue - RADAR_TOWN_RADIUS
                                        || townYValue < mapRow
                                        || mapColumnLimit < townXValue - RADAR_TOWN_RADIUS
                                        || mapColumnLimit > townXValue + RADAR_TOWN_RADIUS) {
                                        goto radar_default_object;
                                    }
                                    break;
                                }
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
                                    switch (cellValue->m_triggerType) {
                                        case MAP_OBJECT_ALCHEMIST_LAB:
                                        case MAP_OBJECT_MINE:
                                        case MAP_OBJECT_SAWMILL:
                                        case (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB):
                                        case (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE):
                                        case (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SAWMILL): {
                                            i32 ownerColorIndex;
                                            ownerIndexValue =
                                                gpGame->m_mineOwners[cellValue->m_objectMetadata];
                                            if (ownerIndexValue >= 0) {
                                                ownerColorIndex =
                                                    gpGame->m_players[ownerIndexValue].m_color;
                                            } else {
                                                ownerColorIndex = RADAR_NEUTRAL_OWNER;
                                            }
                                            radarColorValue = gOwnerColors[ownerColorIndex];
                                            break;
                                        }
                                        default:
                                            radarColorValue =
                                                gMapColors[IDX(giGroundToTerrain
                                                                   [cellValue
                                                                        ->m_terrainImageIndex])]
                                                + RADAR_TERRAIN_SHADE;
                                            break;
                                    }
                                    break;
                                default:
                                radar_default_object:
                                    switch (cellValue->m_triggerType) {
                                        case MAP_OBJECT_ALCHEMIST_LAB:
                                        case MAP_OBJECT_MINE:
                                        case MAP_OBJECT_SAWMILL:
                                        case (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB):
                                        case (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE):
                                        case (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SAWMILL): {
                                            i32 ownerColorIndex;
                                            ownerIndexValue =
                                                gpGame->m_mineOwners[cellValue->m_objectMetadata];
                                            if (ownerIndexValue >= 0) {
                                                ownerColorIndex =
                                                    gpGame->m_players[ownerIndexValue].m_color;
                                            } else {
                                                ownerColorIndex = RADAR_NEUTRAL_OWNER;
                                            }
                                            radarColorValue = gOwnerColors[ownerColorIndex];
                                            break;
                                        }
                                        default:
                                            radarColorValue = gMapColors
                                                [IDX(giGroundToTerrain
                                                         [cellValue->m_terrainImageIndex])];
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
                    memset(radarPixel, radarColorValue, RADAR_SMALL_CELL_PIXELS);
                    memset(
                        radarPixel + RADAR_SCREEN_PITCH,
                        radarColorValue,
                        RADAR_SMALL_CELL_PIXELS
                    );
                    memset(
                        radarPixel + RADAR_SCREEN_PITCH * RADAR_MEDIUM_CELL_PIXELS,
                        radarColorValue,
                        RADAR_SMALL_CELL_PIXELS
                    );
                    memset(
                        radarPixel
                            + RADAR_SCREEN_PITCH * (RADAR_SMALL_CELL_PIXELS - 1),
                        radarColorValue,
                        RADAR_SMALL_CELL_PIXELS
                    );
                    radarPixel += RADAR_SMALL_CELL_PIXELS;
                    break;
                case MAP_DIMENSION_MEDIUM:
                    memset(radarPixel, radarColorValue, RADAR_MEDIUM_CELL_PIXELS);
                    memset(
                        radarPixel + RADAR_SCREEN_PITCH,
                        radarColorValue,
                        RADAR_MEDIUM_CELL_PIXELS
                    );
                    radarPixel += RADAR_MEDIUM_CELL_PIXELS;
                    break;
                case MAP_DIMENSION_LARGE:
                    if (columnRemainderValue != 0) {
                        if (rowRemainderState != 0) {
                            radarPixel[0] = radarColorValue;
                            ++radarPixel;
                        } else {
                            radarPixel[0] = radarColorValue;
                            radarPixel[RADAR_SCREEN_PITCH] = radarColorValue;
                            ++radarPixel;
                        }
                    } else if (rowRemainderState != 0) {
                        radarPixel[0] = radarColorValue;
                        radarPixel[1] = radarColorValue;
                        radarPixel += RADAR_MEDIUM_CELL_PIXELS;
                    } else {
                        radarPixel[0] = radarColorValue;
                        radarPixel[1] = radarColorValue;
                        radarPixel[RADAR_SCREEN_PITCH] = radarColorValue;
                        radarPixel[RADAR_SCREEN_PITCH + 1] = radarColorValue;
                        radarPixel += RADAR_MEDIUM_CELL_PIXELS;
                    }
                    ++columnRemainderValue;
                    if (columnRemainderValue >= RADAR_LARGE_SCALE_DIVISOR) {
                        columnRemainderValue = 0;
                    }
                    break;
                case MAP_DIMENSION_XLARGE:
                    *radarPixel++ = radarColorValue;
                    break;
            }
        }
    }

    radarFrameLocal = RADAR_FRAME_NONE;
    skipFrameIndex = 0;
    if (gbInViewWorld != 0) {
        switch (MAP_HEIGHT) {
            case MAP_DIMENSION_SMALL:
                radarScaleState = RADAR_SMALL_CELL_PIXELS;
                skipFrameIndex = 1;
                break;
            case MAP_DIMENSION_MEDIUM:
                radarScaleState = RADAR_MEDIUM_CELL_PIXELS;
                if (giViewWorldScale <= VIEW_WORLD_SCALE_MIDDLE) {
                    skipFrameIndex = 1;
                } else {
                    radarFrameLocal = RADAR_FRAME_VIEW_MIDDLE;
                }
                break;
            case MAP_DIMENSION_LARGE:
                // Retail approximates the large-map radar ratio rather than using 4/3.
                // NOLINTNEXTLINE(readability-magic-numbers)
                radarScaleState = DATA_COMPGEN(0x004eb690, updateRadarConstant, 1.33f);
                if (giViewWorldScale <= VIEW_WORLD_SCALE_FAR) {
                    skipFrameIndex = 1;
                } else if (giViewWorldScale == VIEW_WORLD_SCALE_MIDDLE) {
                    radarFrameLocal = RADAR_FRAME_VIEW_MIDDLE_LARGE;
                } else {
                    radarFrameLocal = RADAR_FRAME_VIEW_NEAR_LARGE;
                }
                break;
            default:
                radarScaleState = 1.0f;
                if (giViewWorldScale == VIEW_WORLD_SCALE_FAR) {
                    radarFrameLocal = RADAR_FRAME_VIEW_FAR_XLARGE;
                } else if (giViewWorldScale == VIEW_WORLD_SCALE_MIDDLE) {
                    radarFrameLocal = RADAR_FRAME_VIEW_MIDDLE;
                } else {
                    radarFrameLocal = RADAR_FRAME_VIEW_NEAR_XLARGE;
                }
                break;
        }
    } else {
        switch (MAP_HEIGHT) {
            case MAP_DIMENSION_SMALL:
                radarFrameLocal = RADAR_FRAME_NORMAL_SMALL;
                radarScaleState = RADAR_SMALL_CELL_PIXELS;
                break;
            case MAP_DIMENSION_MEDIUM:
                radarFrameLocal = RADAR_FRAME_NORMAL_MEDIUM;
                radarScaleState = RADAR_MEDIUM_CELL_PIXELS;
                break;
            case MAP_DIMENSION_LARGE:
                radarFrameLocal = RADAR_FRAME_NORMAL_LARGE;
                // Retail approximates the large-map radar ratio rather than using 4/3.
                // NOLINTNEXTLINE(readability-magic-numbers)
                radarScaleState = 1.33f;
                break;
            default:
                radarFrameLocal = RADAR_FRAME_NORMAL_XLARGE;
                radarScaleState = 1.0f;
                break;
        }
    }

    if (skipFrameIndex == 0) {
        if (gbInViewWorld != 0) {
            m_puzzleIcon->ClipFillToBuffer(
                static_cast<i32>(
                    iVWMapOriginX * radarScaleState
                    + DATA_COMPGEN(0x004eb698, radarLeftFloat, IDX(RADAR_LEFT))
                ),
                static_cast<i32>(
                    iVWMapOriginY * radarScaleState
                    + DATA_COMPGEN(0x004eb694, radarTopFloat, IDX(RADAR_TOP))
                ),
                radarFrameLocal,
                RADAR_VIEWPORT_COLOR,
                ICON_DRAW_NORMAL,
                RADAR_LEFT,
                RADAR_TOP,
                RADAR_SIZE,
                RADAR_SIZE
            );
        } else {
            m_puzzleIcon->ClipFillToBuffer(
                static_cast<i32>(m_mapOriginX * radarScaleState + IDX(RADAR_LEFT)),
                static_cast<i32>(m_mapOriginY * radarScaleState + IDX(RADAR_TOP)),
                radarFrameLocal,
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
                static_cast<i32>(minXSlot * radarScaleState + IDX(RADAR_LEFT)),
                static_cast<i32>(minYOffset * radarScaleState + IDX(RADAR_TOP)),
                static_cast<i32>((maxXLocal - minXSlot + 1) * radarScaleState),
                static_cast<i32>((maxYLocal - minYOffset + 1) * radarScaleState)
            );
        } else {
            gpWindowManager->UpdateScreenRegion(
                RADAR_LEFT,
                RADAR_TOP,
                RADAR_SIZE,
                RADAR_SIZE
            );
        }
    }
}

VA(0x0045f127, 0x133e)
void advManager::QuickInfo(i32 cellX, i32 cellY) {
    i32 quickInfoShowFlag = 1;
    mapCell* currentCell = NULL;
    hero* heroLocal = NULL;
    i32 quickInfoScreenX;
    i32 dialogTopPosition;
    heroWindow* windowLocal;
    HeroEventFlag visitedMaskValue;
    char savedTextLocal[QUICK_INFO_TEXT_CAPACITY];
    char guardCaption[QUICK_INFO_TEXT_CAPACITY];
    i32 siteFrameLocal[QUICK_INFO_SITE_FRAME_COUNT];
    TilesetId objectTilesetLocal;
    char uppercaseResult;
    char mapObjectKindValue;
    i32 expansionSite;
    tag_message message;

    if (gpCurPlayer->CurrentHero() != -1) {
        heroLocal = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];
    } else {
        heroLocal = NULL;
    }

    quickInfoScreenX = cellX * CELL_PIXELS - QUICK_INFO_X_OFFSET;
    if (quickInfoScreenX < QUICK_INFO_MIN_X) {
        quickInfoScreenX = QUICK_INFO_MIN_X;
    }
    if (quickInfoScreenX + QUICK_INFO_WIDTH > QUICK_INFO_RIGHT) {
        quickInfoScreenX = QUICK_INFO_RIGHT_X;
    }

    dialogTopPosition = cellY * CELL_PIXELS - QUICK_INFO_Y_OFFSET;
    if (dialogTopPosition < QUICK_INFO_MIN_Y) {
        dialogTopPosition = QUICK_INFO_MIN_Y;
    }
    if (dialogTopPosition + QUICK_INFO_HEIGHT > QUICK_INFO_BOTTOM) {
        dialogTopPosition = QUICK_INFO_BOTTOM_Y;
    }

    windowLocal = new heroWindow(quickInfoScreenX, dialogTopPosition, DATA_COMPGEN(0x004f5e4c, quickInfoQwikinfoBin, "qwikinfo.bin"));
    if (windowLocal == NULL) {
        MemError();
    }
    visitedMaskValue = HERO_EVENT_NONE;

    if (m_mapOriginX + cellX < 0 || m_mapOriginX + cellX >= MAP_WIDTH || m_mapOriginY + cellY < 0
        || m_mapOriginY + cellY >= MAP_HEIGHT) {
        sprintf(gText, DATA_COMPGEN(0x004f5e64, quickInfoS, "%s"), DATA_COMPGEN(0x004f5e5c, quickInfoBorder, "Border"));
    } else {
        currentCell = GetCell(m_mapOriginX + cellX, m_mapOriginY + cellY);
        if ((giCurPlayerBit & (mapExtra + (m_mapOriginY + cellY) * MAP_WIDTH)[m_mapOriginX + cellX])
            == 0) {
            sprintf(gText, DATA_COMPGEN(0x004f5e7c, quickInfoS2, "%s"), DATA_COMPGEN(0x004f5e68, quickInfoUnchartedTerritory, "Uncharted Territory"));
        } else {

            switch (currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
                case MAP_OBJECT_ARTIFACT:
                    sprintf(gText, DATA_COMPGEN(0x004f5e8c, quickInfoS3, "%s"), DATA_COMPGEN(0x004f5e80, quickInfoArtifact, "Artifact"));
                    break;
                case MAP_OBJECT_OBELISK:
                    if HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f5eb4, quickInfoSS, "%s\n\n%s"),
                            gQuickViewText[IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (gpGame->m_obeliskVisitors
                                 [currentCell->m_objectMetadata - OBELISK_INDEX_BASE]
                             & (1u << giCurPlayer))
                                ? DATA_COMPGEN(0x004f5e90, quickInfoAlreadyVisited, "(already visited)")
                                : DATA_COMPGEN(0x004f5ea4, quickInfoNotVisited, "(not visited)")
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_GAZEBO:
                    if (heroLocal != NULL && HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f5ee0, quickInfoSS2, "%s\n\n%s"),
                            gQuickViewText[IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (heroLocal->m_gazeboVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? DATA_COMPGEN(0x004f5ebc, quickInfoAlreadyVisited2, "(already visited)")
                                : DATA_COMPGEN(0x004f5ed0, quickInfoNotVisited2, "(not visited)")
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_FORT:
                    if (heroLocal != NULL && HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f5f0c, quickInfoSS3, "%s\n\n%s"),
                            gQuickViewText[IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (heroLocal->m_fortVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? DATA_COMPGEN(0x004f5ee8, quickInfoAlreadyVisited3, "(already visited)")
                                : DATA_COMPGEN(0x004f5efc, quickInfoNotVisited3, "(not visited)")
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_WITCH_DOCTOR_HUT:
                    if (heroLocal != NULL && HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f5f38, quickInfoSS4, "%s\n\n%s"),
                            gQuickViewText[IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (heroLocal->m_witchDoctorVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? DATA_COMPGEN(0x004f5f14, quickInfoAlreadyVisited4, "(already visited)")
                                : DATA_COMPGEN(0x004f5f28, quickInfoNotVisited4, "(not visited)")
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_MERCENARY_CAMP:
                    if (heroLocal != NULL && HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f5f64, quickInfoSS5, "%s\n\n%s"),
                            gQuickViewText[IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (heroLocal->m_mercenaryCampVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? DATA_COMPGEN(0x004f5f40, quickInfoAlreadyVisited5, "(already visited)")
                                : DATA_COMPGEN(0x004f5f54, quickInfoNotVisited5, "(not visited)")
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_STANDING_STONES:
                    if (heroLocal != NULL && HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f5f90, quickInfoSS6, "%s\n\n%s"),
                            gQuickViewText[IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (heroLocal->m_standingStoneVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? DATA_COMPGEN(0x004f5f6c, quickInfoAlreadyVisited6, "(already visited)")
                                : DATA_COMPGEN(0x004f5f80, quickInfoNotVisited6, "(not visited)")
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_TREE_OF_KNOWLEDGE:
                    if (heroLocal != NULL && HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f5fbc, quickInfoSS7, "%s\n\n%s"),
                            gQuickViewText[IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (heroLocal->m_treeKnowledgeVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? DATA_COMPGEN(0x004f5f98, quickInfoAlreadyVisited7, "(already visited)")
                                : DATA_COMPGEN(0x004f5fac, quickInfoNotVisited7, "(not visited)")
                        );
                    } else {
                        goto quick_info_default;
                    }
                    break;
                case MAP_OBJECT_XANADU:
                    if (heroLocal != NULL && HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f5fe8, quickInfoSS8, "%s\n\n%s"),
                            gQuickViewText[IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            (heroLocal->m_xanaduVisits
                             & (1u << (currentCell->m_objectMetadata & VISIT_BIT_INDEX_MASK)))
                                ? DATA_COMPGEN(0x004f5fc4, quickInfoAlreadyVisited8, "(already visited)")
                                : DATA_COMPGEN(0x004f5fd8, quickInfoNotVisited8, "(not visited)")
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
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x004f5ff0, quickInfoS4, "%s"),
                        gTerrainNames[IDX(giGroundToTerrain
                                              [currentCell->m_terrainImageIndex])]
                    );
                    break;
                case MAP_OBJECT_ABANDONED_MINE:
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x004f5ff4, quickInfoS5, "%s"),
                        gQuickViewText[IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)]
                    );
                    goto quick_info_guarded;
                case MAP_OBJECT_MINE:
                    if (gpGame->m_mines[currentCell->m_objectMetadata].guardianType
                        != CREATURE_NONE) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f6000, quickInfoSS9, "%s %s"),
                            gResourceNames[IDX(
                                gpGame->m_mines[currentCell->m_objectMetadata].resourceType
                            )],
                            DATA_COMPGEN(0x004f5ff8, quickInfoMine, "Mine")
                        );
                    quick_info_guarded:
                        sprintf(
                            guardCaption,
                            DATA_COMPGEN(0x004f6008, quickInfoGuardedBySS, "\n\nguarded by %s %s"),
                            GetArmySizeName(
                                gpGame->m_mines[currentCell->m_objectMetadata].guardianCount,
                                ARMY_SIZE_NAME_INLINE
                            ),
                            gArmyNamesPlural[IDX(
                                gpGame->m_mines[currentCell->m_objectMetadata].guardianType
                            )]
                        );
                        strcat(gText, guardCaption);
                    } else {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f6024, quickInfoSS10, "%s %s"),
                            gResourceNames[IDX(
                                gpGame->m_mines[currentCell->m_objectMetadata].resourceType
                            )],
                            DATA_COMPGEN(0x004f601c, quickInfoMine2, "Mine")
                        );
                    }
                    break;
                case MAP_OBJECT_RESOURCE:
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x004f602c, quickInfoS6, "%s"),
                        gResourceNames
                            [(currentCell->m_objectIndex & RESOURCE_FRAME_PAIR_MASK)
                             / RESOURCE_FRAME_PAIR_STRIDE]
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
                            DATA_COMPGEN(0x004f6030, quickInfoDS, "%d %s"),
                            currentCell->m_objectMetadata & IDX(MAP_MONSTER_COUNT_MASK),
                            gArmyNamesPlural[currentCell->m_objectIndex]
                        );
                    } else {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f6038, quickInfoSS11, "%s %s"),
                            GetArmySizeName(
                                currentCell->m_objectMetadata & IDX(MAP_MONSTER_COUNT_MASK),
                                ARMY_SIZE_NAME_SENTENCE
                            ),
                            gArmyNamesPlural[currentCell->m_objectIndex]
                        );
                    }
                    break;
                case MAP_OBJECT_BARRIER:
                case MAP_OBJECT_TRAVELER_TENT:
                    sprintf(
                        gText,
                        gQuickViewText[IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                        xBarrierColor[currentCell->m_objectMetadata & BARRIER_COLOR_MASK]
                    );
                    uppercaseResult =
                        static_cast<char>(toupper(static_cast<i32>(static_cast<i8>(gText[0]))));
                    gText[0] = uppercaseResult;
                    break;
                case MAP_OBJECT_EXPANSION_OBJECT: {
                    mapObjectKindValue = -1;
                    if (currentCell->m_objectIndex != MAPCELL_SPRITE_NONE) {
                        siteFrameLocal[0] = currentCell->m_objectIndex;
                        objectTilesetLocal = currentCell->m_objectTileset;
                    } else {
                        siteFrameLocal[0] = currentCell->m_overlayIndex;
                        objectTilesetLocal = currentCell->m_overlayTileset;
                    }
                    expansionSite = IDX(GENERIC_SITE_UNKNOWN);
                    switch (objectTilesetLocal) {
                        case TILESET_X_LOC1:
                            if (siteFrameLocal[0] < 0) {
                                break;
                            } else {
                                if (siteFrameLocal[0] < GENERIC_SITE_1_END) {
                                    expansionSite = IDX(GENERIC_SITE_ALCHEMIST_TOWER);
                                } else if (siteFrameLocal[0] < GENERIC_SITE_2_END) {
                                    expansionSite = IDX(GENERIC_SITE_ARENA);
                                    visitedMaskValue = ADVMGR_VISIT_GENERIC_HUT;
                                }
                            }
                            break;
                        case TILESET_X_LOC2:
                            if (siteFrameLocal[0] < 0) {
                                break;
                            } else {
                                if (siteFrameLocal[0] < GENERIC_ALTAR_END) {
                                    expansionSite = IDX(GENERIC_SITE_STABLES);
                                    visitedMaskValue = ADVMGR_VISIT_GENERIC_ALTAR;
                                } else if (siteFrameLocal[0] < GENERIC_UNUSED_END) {
                                    expansionSite = IDX(GENERIC_SITE_UNKNOWN);
                                } else if (siteFrameLocal[0] < GENERIC_TOWER_END) {
                                    expansionSite = IDX(GENERIC_SITE_MERMAID);
                                    visitedMaskValue = ADVMGR_VISIT_GENERIC_TOWER;
                                } else if (siteFrameLocal[0] < GENERIC_SPRING_END) {
                                    expansionSite = IDX(GENERIC_SITE_SIRENS);
                                    visitedMaskValue = ADVMGR_VISIT_GENERIC_SPRING;
                                }
                            }
                            break;
                        case TILESET_X_LOC3:
                            if (siteFrameLocal[0] < 0) {
                                break;
                            } else {
                                if (siteFrameLocal[0] < GENERIC_SITE_3_SPLIT) {
                                    expansionSite = IDX(GENERIC_SITE_HUT_OF_MAGI);
                                } else if (siteFrameLocal[0] < GENERIC_SITE_3_END) {
                                    expansionSite = IDX(GENERIC_SITE_EYE_OF_MAGI);
                                }
                            }
                            break;
                    }
                    if (expansionSite == IDX(GENERIC_SITE_UNKNOWN)) {
                        sprintf(gText, DATA_COMPGEN(0x004f6040, quickInfoUnknown, "Unknown"));
                    } else {
                        sprintf(gText, xGenericSiteNames[expansionSite]);
                    }
                    if (heroLocal != NULL && visitedMaskValue != HERO_EVENT_NONE) {
                        strcat(gText, DATA_COMPGEN(0x004f6048, quickInfoEmptyString, "\n\n"));
                        strcat(
                            gText,
                            HAS(heroLocal->m_eventFlags, visitedMaskValue) ? DATA_COMPGEN(0x004f604c, quickInfoAlreadyVisited9, "(already visited)")
                                                                           : DATA_COMPGEN(0x004f6060, quickInfoNotVisited9, "(not visited)")
                        );
                    }
                    break;
                }
                case MAP_OBJECT_EXPANSION_DWELLING: {
                    if (currentCell->m_overlayIndex == MAPCELL_SPRITE_NONE) {
                        siteFrameLocal[0] = currentCell->m_objectIndex;
                        objectTilesetLocal = currentCell->m_objectTileset;
                    } else {
                        siteFrameLocal[0] = currentCell->m_overlayIndex;
                        objectTilesetLocal = currentCell->m_overlayTileset;
                    }
                    expansionSite = IDX(RECRUITMENT_SITE_UNKNOWN);
                    switch (objectTilesetLocal) {
                        case TILESET_X_LOC1:
                            if (siteFrameLocal[0] < RECRUITMENT_START) {
                                break;
                            } else {
                                if (siteFrameLocal[0] < RECRUITMENT_1_END) {
                                    expansionSite = IDX(RECRUITMENT_SITE_BARROW_MOUNDS);
                                } else if (siteFrameLocal[0] < RECRUITMENT_2_END) {
                                    expansionSite = IDX(RECRUITMENT_SITE_EARTH_ALTAR);
                                } else if (siteFrameLocal[0] < RECRUITMENT_3_END) {
                                    expansionSite = IDX(RECRUITMENT_SITE_AIR_ALTAR);
                                } else if (siteFrameLocal[0] < RECRUITMENT_4_END) {
                                    expansionSite = IDX(RECRUITMENT_SITE_FIRE_ALTAR);
                                } else if (siteFrameLocal[0] < RECRUITMENT_5_END) {
                                    expansionSite = IDX(RECRUITMENT_SITE_WATER_ALTAR);
                                }
                            }
                            break;
                    }
                    if (expansionSite == IDX(RECRUITMENT_SITE_UNKNOWN)) {
                        sprintf(gText, DATA_COMPGEN(0x004f6070, quickInfoUnknown2, "Unknown"));
                    } else {
                        sprintf(gText, xRecruitmentSiteNames[expansionSite]);
                    }
                    break;
                }
                case MAP_OBJECT_ROCK:
                    if (currentCell->m_objectTileset == TILESET_X_LOC2) {
                        sprintf(gText, DATA_COMPGEN(0x004f6078, quickInfoReefs, "Reefs"));
                    } else {
                        goto quick_info_default;
                    }
                    break;
                default:
                quick_info_default:
                    if (visitedMaskValue != HERO_EVENT_NONE && heroLocal != NULL) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f60a4, quickInfoSS12, "%s\n\n%s"),
                            gQuickViewText[IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)],
                            HAS(heroLocal->m_eventFlags, visitedMaskValue) ? DATA_COMPGEN(0x004f6080, quickInfoAlreadyVisited10, "(already visited)")
                                                                           : DATA_COMPGEN(0x004f6094, quickInfoNotVisited10, "(not visited)")
                        );
                    } else {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x004f60ac, quickInfoS7, "%s"),
                            gQuickViewText[IDX(currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)]
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
            DATA_COMPGEN(0x004f60b0, quickInfoGiDObtileDObiD, "gi%d obtile%d obi%d ot%d ei%d bl%d %s X%d Y%d"),
            currentCell->m_terrainImageIndex,
            currentCell->m_objectTileset,
            currentCell->m_objectIndex,
            IDX(currentCell->m_triggerType),
            currentCell->m_objectMetadata,
            currentCell->m_flags & IDX(MAP_CELL_OCCUPIED),
            savedTextLocal,
            m_mapOriginX + cellX,
            m_mapOriginY + cellY
        );
    }
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = 1;
    message.payload.widget.data.text = gText;
    windowLocal->BroadcastMessage(message);
    gpWindowManager->AddWindow(windowLocal, -1, 1);
    QuickViewWait();
    gpWindowManager->RemoveWindow(windowLocal);
    delete windowLocal;
}

VA(0x00460465, 0x348)
void advManager::UpdateHeroLocator(i32 locatorSlot, i32 drawWindow, i32 updateScreen) {
    hero* locatorHero8;
    i32 mobilityFrame18;
    i32 manaFrame36;
    i32 selectedHero9;
    i32 widgetBase7;
    i32 index3;
    tag_message locatorMessage15;
    i32 heroId9;

    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        return;
    }

    if (locatorSlot == -1) {
        selectedHero9 = gpCurPlayer->m_currentHero;
        if (selectedHero9 == INVALID_HERO) {
            return;
        }
        for (index3 = 0; index3 < LOCATOR_VISIBLE_COUNT; ++index3) {
            if (gpCurPlayer->m_heroIds[gpCurPlayer->m_heroLocatorPage + index3] == selectedHero9) {
                locatorSlot = index3;
            }
        }
        if (locatorSlot == -1) {
            return;
        }
    }

    widgetBase7 = locatorSlot * LOCATOR_HERO_WIDGET_STRIDE + LOCATOR_HERO_WIDGET_BASE;
    locatorMessage15.type = ADVMGR_LOCATOR_MESSAGE_TYPE;
    heroId9 = gpCurPlayer->m_heroIds[gpCurPlayer->m_heroLocatorPage + locatorSlot];
    locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_COLOR;
    locatorMessage15.payload.widget.id = widgetBase7 + LOCATOR_HERO_BORDER_OFFSET;
    if (gpCurPlayer->m_currentHero == heroId9 && gpCurPlayer->m_currentHero != INVALID_HERO
        && !gbAllBlack) {
        locatorMessage15.payload.widget.data.value = LOCATOR_SELECTED_COLOR;
    } else {
        locatorMessage15.payload.widget.data.value = LOCATOR_NORMAL_COLOR;
    }
    m_adventureWindow->BroadcastMessage(locatorMessage15);

    if (heroId9 == INVALID_HERO || gbAllBlack) {
        locatorMessage15.payload.widget.id = widgetBase7 + LOCATOR_HERO_IMAGE_OFFSET;
        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        locatorMessage15.payload.widget.data.value =
            locatorSlot + LOCATOR_HERO_EMPTY_FRAME_BASE;
        m_adventureWindow->BroadcastMessage(locatorMessage15);

        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS;
        locatorMessage15.payload.widget.data.value = LOCATOR_HERO_DISABLE_FLAGS;
        for (index3 = 0; index3 <= LOCATOR_VISIBLE_COUNT - 1; ++index3) {
            locatorMessage15.payload.widget.id = widgetBase7 + index3;
            m_adventureWindow->BroadcastMessage(locatorMessage15);
        }
    } else {
        locatorHero8 = &gpGame->m_heroRecs[heroId9];
        locatorMessage15.payload.widget.id = widgetBase7 + LOCATOR_HERO_IMAGE_OFFSET;
        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        locatorMessage15.payload.widget.data.value = LOCATOR_HERO_DEFAULT_FRAME;
        m_adventureWindow->BroadcastMessage(locatorMessage15);

        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FLAGS;
        locatorMessage15.payload.widget.data.value = LOCATOR_HERO_ENABLE_FLAGS;
        for (index3 = 0; index3 <= LOCATOR_HERO_WIDGET_STRIDE - 1; ++index3) {
            locatorMessage15.payload.widget.id = widgetBase7 + index3;
            m_adventureWindow->BroadcastMessage(locatorMessage15);
        }

        mobilityFrame18 = GetMobilityFrame(locatorHero8->m_remainingMobility);
        locatorMessage15.payload.widget.id = widgetBase7 + LOCATOR_HERO_MOBILITY_OFFSET;
        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        locatorMessage15.payload.widget.data.value = mobilityFrame18;
        m_adventureWindow->BroadcastMessage(locatorMessage15);

        manaFrame36 = GetManaFrame(locatorHero8->m_spellPoints);
        locatorMessage15.payload.widget.id = widgetBase7 + LOCATOR_HERO_MANA_OFFSET;
        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        locatorMessage15.payload.widget.data.value = manaFrame36;
        m_adventureWindow->BroadcastMessage(locatorMessage15);

        locatorMessage15.payload.widget.id = widgetBase7 + LOCATOR_HERO_PORTRAIT_OFFSET;
        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        locatorMessage15.payload.widget.data.value = IDX(locatorHero8->m_portrait);
        m_adventureWindow->BroadcastMessage(locatorMessage15);
    }

    if (drawWindow) {
        m_adventureWindow->DrawWindow(
            LOCATOR_HERO_DRAW_LEFT,
            widgetBase7,
            widgetBase7 + LOCATOR_HERO_BORDER_OFFSET
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

VA(0x004607ad, 0x102)
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
        scrollStep = DATA_COMPGEN(
                         0x004eb6a0,
                         heroLocatorScrollSpan,
                         static_cast<double>(LOCATOR_HERO_SCROLL_SPAN)
                     )
                     / (gpCurPlayer->m_heroCount - LOCATOR_PAGE_DENOMINATOR_OFFSET);
        m_scrollLeftButton->m_y = static_cast<i16>(
            gpCurPlayer->m_heroLocatorPage * scrollStep
            + DATA_COMPGEN(0x004eb6a8, locatorScrollBaseY, IDX(LOCATOR_SCROLL_BASE_Y))
        );
    }
    if (drawWindow) {
        m_adventureWindow->DrawWindow(updateScreen);
    }
}

VA(0x004608af, 0x2e8)
void advManager::UpdateTownLocators(i32 drawWindow, i32 updateScreen) {
    i32 locatorSlot;
    tag_message locatorMessage14;
    i32 townId37;
    double scrollStep;

    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        return;
    }

    locatorMessage14.type = ADVMGR_LOCATOR_MESSAGE_TYPE;
    for (locatorSlot = 0; locatorSlot < LOCATOR_VISIBLE_COUNT; ++locatorSlot) {
        townId37 = gpCurPlayer->m_townIds[gpCurPlayer->m_townLocatorPage + locatorSlot];
        locatorMessage14.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_COLOR;
        locatorMessage14.payload.widget.id = locatorSlot + LOCATOR_TOWN_BORDER_BASE;
        if (gpCurPlayer->m_currentTown != TOWN_ID_NONE
            && gpCurPlayer->m_currentTown == townId37 && !gbAllBlack) {
            locatorMessage14.payload.widget.data.value = LOCATOR_SELECTED_COLOR;
        } else {
            locatorMessage14.payload.widget.data.value = LOCATOR_NORMAL_COLOR;
        }
        m_adventureWindow->BroadcastMessage(locatorMessage14);

        locatorMessage14.payload.widget.id = locatorSlot + LOCATOR_TOWN_IMAGE_BASE;
        if (townId37 == TOWN_ID_NONE || gbAllBlack) {
            locatorMessage14.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
            locatorMessage14.payload.widget.data.value =
                locatorSlot + LOCATOR_TOWN_EMPTY_FRAME_BASE;
            m_adventureWindow->BroadcastMessage(locatorMessage14);
            locatorMessage14.payload.widget.command = ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS;
            locatorMessage14.payload.widget.data.value = LOCATOR_TOWN_ENABLE_FLAGS;
            m_adventureWindow->BroadcastMessage(locatorMessage14);
            locatorMessage14.payload.widget.command = ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS;
            locatorMessage14.payload.widget.data.value = LOCATOR_TOWN_DISABLE_FLAGS;
            locatorMessage14.payload.widget.id = locatorSlot + LOCATOR_TOWN_FLAG_BASE;
            m_adventureWindow->BroadcastMessage(locatorMessage14);
        } else {
            locatorMessage14.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FLAGS;
            locatorMessage14.payload.widget.data.value = LOCATOR_TOWN_ENABLE_FLAGS;
            m_adventureWindow->BroadcastMessage(locatorMessage14);
            locatorMessage14.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
            locatorMessage14.payload.widget.data.value =
                IDX(gpGame->GetTown(townId37)->m_type) + LOCATOR_TOWN_TYPE_FRAME_BASE;
            if (!(gpGame->GetTown(townId37)->m_buildings & IDX(TOWN_BUILDING_CASTLE))) {
                locatorMessage14.payload.widget.data.value +=
                    LOCATOR_TOWN_VILLAGE_FRAME_OFFSET;
            }
            m_adventureWindow->BroadcastMessage(locatorMessage14);

            if (BitTest(gpGame->m_knownTowns, townId37)) {
                locatorMessage14.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FLAGS;
            } else {
                locatorMessage14.payload.widget.command = ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS;
            }
            locatorMessage14.payload.widget.data.value = LOCATOR_TOWN_DISABLE_FLAGS;
            locatorMessage14.payload.widget.id = locatorSlot + LOCATOR_TOWN_FLAG_BASE;
            m_adventureWindow->BroadcastMessage(locatorMessage14);
        }
    }

    if (gpCurPlayer->m_townCount < LOCATOR_PAGE_THRESHOLD) {
        m_scrollRightButton->m_y = LOCATOR_SCROLL_NO_PAGES_Y;
    } else {
        scrollStep = DATA_COMPGEN(
                         0x004eb6b0,
                         townLocatorScrollSpan,
                         static_cast<double>(LOCATOR_TOWN_SCROLL_SPAN)
                     )
                     / (gpCurPlayer->m_townCount - LOCATOR_PAGE_DENOMINATOR_OFFSET);
        m_scrollRightButton->m_y = static_cast<i16>(
            gpCurPlayer->m_townLocatorPage * scrollStep + IDX(LOCATOR_SCROLL_BASE_Y)
        );
    }
    if (drawWindow) {
        m_adventureWindow->DrawWindow(updateScreen);
    }
}

VA(0x00460b97, 0x1cc)
void advManager::UpdBottomView(i32 forceUpdate, i32 drawWindow, i32 updateScreen) {
    i32 updated;

    updated = 0;
    gbForceUpdate = forceUpdate;
    if (giBottomViewOverride == BOTTOM_VIEW_OVERRIDE_DISABLED) {
        return;
    }

    if (giBottomViewOverride > BOTTOM_VIEW_NONE) {
        if (KBTickCount() > giBottomViewOverrideEndTime) {
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
        || gpGame->m_players[giCurPlayer].m_color != gpCurPlayer->m_color) {
        updated = UpdBottomViewEnemyTurn();
    } else if (gpCurPlayer->m_currentHero == INVALID_HERO) {
        updated = UpdBottomViewKingdom();
    } else {
        updated = UpdBottomViewHero();
    }

update_bottom_view:
    if (updated && drawWindow) {
        m_adventureWindow->DrawWindow(
            BOTTOM_VIEW_DRAW_LEFT,
            BOTTOM_VIEW_DRAW_TOP,
            BOTTOM_VIEW_DRAW_BOTTOM
        );
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

VA(0x00460d63, 0x132)
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

VA(0x00460e95, 0x51b)
i32 advManager::UpdBottomViewEnemyTurn(void) {
    i32 updated;
    tag_message message;

    updated = 0;
    message.type = ADVMGR_ENEMY_TURN_MESSAGE_TYPE;
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
            DATA_COMPGEN(0x004f60e8, updBottomViewEnemyTurnStonbackIcn, "stonback.icn"),
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
            DATA_COMPGEN(0x004f60f8, updBottomViewEnemyTurnHourglasIcn, "hourglas.icn"),
            0,
            ICON_DRAW_NORMAL,
            ENEMY_TURN_HOURGLASS_ID,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (m_bottomViewHourglassBackground == NULL) {
            MemError();
        }
        m_adventureWindow->AddWidget(
            m_bottomViewHourglassBackground,
            ENEMY_TURN_HOURGLASS_Z
        );
    }

    if (gbForceUpdate || KBTickCount() - iLastSandAnimTime > ENEMY_TURN_ANIMATION_DELAY) {
        iLastSandAnimTime = KBTickCount();
        iLastAnimFrame = m_updateMaxX;
        if (KBTickCount() - iLastNewSandAnimTime > ENEMY_TURN_ANIMATION_DELAY) {
            iLastNewSandAnimTime = KBTickCount();
            ++iSandAnim;
            if (iSandAnim >= ENEMY_TURN_SAND_FRAME_LIMIT) {
                iSandAnim = ENEMY_TURN_SAND_RESTART_FRAME;
            }
            updated = 1;

            if (m_bottomViewIcons[ENEMY_TURN_SAND_SLOT] != NULL) {
                message.payload.widget.command = ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME;
                message.payload.widget.id = ENEMY_TURN_SAND_ID;
                message.payload.widget.data.value = iSandAnim + ENEMY_TURN_SAND_FRAME_OFFSET;
                m_adventureWindow->BroadcastMessage(message);
            } else {
                m_bottomViewIcons[ENEMY_TURN_SAND_SLOT] = new iconWidget(
                    ENEMY_TURN_ANIMATION_X,
                    ENEMY_TURN_ANIMATION_Y,
                    ENEMY_TURN_ANIMATION_WIDTH,
                    ENEMY_TURN_ANIMATION_HEIGHT,
                    DATA_COMPGEN(0x004f6108, updBottomViewEnemyTurnHourglasIcn2, "hourglas.icn"),
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
            message.payload.widget.command = ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME;
            message.payload.widget.id = ENEMY_TURN_CREST_ID;
            message.payload.widget.data.value =
                gpGame->GetPlayerColor(static_cast<char>(giCurPlayer));
            m_adventureWindow->BroadcastMessage(message);
        } else {
            m_bottomViewIcons[ENEMY_TURN_CREST_SLOT] = new iconWidget(
                ENEMY_TURN_CREST_X,
                ENEMY_TURN_ANIMATION_Y,
                ENEMY_TURN_ANIMATION_WIDTH,
                ENEMY_TURN_ANIMATION_HEIGHT,
                DATA_COMPGEN(0x004f6118, updBottomViewEnemyTurnBrcrestIcn, "brcrest.icn"),
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

    if (gbForceUpdate || iLastHourGlassPhase > iCurHourGlassPhase || iLastHourGlassPhase < 0
        || (iCurHourGlassPhase > iLastHourGlassPhase
            && KBTickCount() - giLastHourGlassUpdateTime >= ENEMY_TURN_PHASE_DELAY)) {
        updated = 1;
        iLastHourGlassPhase = iCurHourGlassPhase;
        giLastHourGlassUpdateTime = KBTickCount();
        if (m_bottomViewIcons[ENEMY_TURN_PHASE_SLOT] != NULL) {
            message.payload.widget.command = ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME;
            message.payload.widget.id = ENEMY_TURN_PHASE_ID;
            message.payload.widget.data.value =
                iCurHourGlassPhase + ENEMY_TURN_PHASE_FRAME_OFFSET;
            m_adventureWindow->BroadcastMessage(message);
        } else {
            m_bottomViewIcons[ENEMY_TURN_PHASE_SLOT] = new iconWidget(
                ENEMY_TURN_ANIMATION_X,
                ENEMY_TURN_ANIMATION_Y,
                ENEMY_TURN_ANIMATION_WIDTH,
                ENEMY_TURN_ANIMATION_HEIGHT,
                DATA_COMPGEN(0x004f6124, updBottomViewEnemyTurnHourglasIcn3, "hourglas.icn"),
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

VA(0x004613b0, 0x366)
i32 advManager::UpdBottomViewNewTurn(void) {
    DATA(0x004f6134) static i16 s_newTurnLineBase = NEW_TURN_LINE_BASE;

    i32 frameIndex;
    i32 month;
    char* weekText;
    char* dayText;

    frameIndex = 0;
    if (!gbForceUpdate && iCurBottomView == BOTTOM_VIEW_NEW_TURN) {
        return 0;
    }

    ClearBottomView();
    iCurBottomView = BOTTOM_VIEW_NEW_TURN;
    if (gpGame->m_day == NEW_TURN_FIRST_DAY
        && (gpGame->m_month != NEW_TURN_FIRST_DAY
            || gpGame->m_week != NEW_TURN_FIRST_DAY
            || gpGame->m_day != NEW_TURN_FIRST_DAY)) {
        frameIndex = gpGame->m_week;
    }

    m_bottomViewBackground = new iconWidget(
        BOTTOM_VIEW_PANEL_X,
        BOTTOM_VIEW_PANEL_Y,
        BOTTOM_VIEW_BACKGROUND_WIDTH,
        BOTTOM_VIEW_PANEL_HEIGHT,
        DATA_COMPGEN(0x004f6138, updBottomViewNewTurnStonbackIcn, "stonback.icn"),
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
        DATA_COMPGEN(0x004f6148, updBottomViewNewTurnSunmoonIcn, "sunmoon.icn"),
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

    weekText = static_cast<char*>(
        H2_ALLOC_AT(
            BOTTOM_VIEW_TEXT_BUFFER_SIZE, DATA_COMPGEN(0x004f6154, updBottomViewNewTurnSourceFile, RETAIL_FILE),
            NEW_TURN_LINE_BASE + IDX(NEW_TURN_WEEK_ALLOC_LINE_OFFSET)
        )
    );
    sprintf(weekText, DATA_COMPGEN(0x004f6190, updBottomViewNewTurnSDSD, "%s: %d  %s: %d"), DATA_COMPGEN(0x004f6188, updBottomViewNewTurnMonth, "Month"), gpGame->m_month, DATA_COMPGEN(0x004f6180, updBottomViewNewTurnWeek, "Week"), gpGame->m_week);
    m_bottomViewAllTexts[0] = new textWidget(
        NEW_TURN_DATE_TEXT_X,
        NEW_TURN_WEEK_TEXT_Y,
        NEW_TURN_DATE_TEXT_WIDTH,
        NEW_TURN_WEEK_TEXT_HEIGHT,
        weekText,
        DATA_COMPGEN(0x004f61a0, updBottomViewNewTurnSmalfontFnt, "smalfont.fnt"),
        FONT_DRAW_DEFAULT,
        BOTTOM_VIEW_TEXT_ID,
        WIDGET_KIND_TEXT,
        FONT_ALIGN_CENTER
    );
    if (m_bottomViewAllTexts[0] == NULL) {
        MemError();
    }
    m_adventureWindow->AddWidget(m_bottomViewAllTexts[0], -1);

    dayText = static_cast<char*>(
        H2_ALLOC_AT(
            BOTTOM_VIEW_TEXT_BUFFER_SIZE, DATA_COMPGEN(0x004f61b0, updBottomViewNewTurnSourceFile2, RETAIL_FILE),
            NEW_TURN_LINE_BASE + IDX(NEW_TURN_DAY_ALLOC_LINE_OFFSET)
        )
    );
    sprintf(dayText, DATA_COMPGEN(0x004f61e0, updBottomViewNewTurnSD, "%s: %d"), DATA_COMPGEN(0x004f61dc, updBottomViewNewTurnDay, "Day"), gpGame->m_day);
    m_bottomViewAllTexts[0] = new textWidget(
        NEW_TURN_DATE_TEXT_X,
        NEW_TURN_DAY_TEXT_Y,
        NEW_TURN_DATE_TEXT_WIDTH,
        NEW_TURN_DAY_TEXT_HEIGHT,
        dayText,
        DATA_COMPGEN(0x004f61e8, updBottomViewNewTurnBigfontFnt, "bigfont.fnt"),
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

VA(0x00461716, 0x35f)
i32 advManager::UpdBottomViewResMsg(void) {
    DATA(0x004f61f4) static i16 s_resourceViewLineBase = RESOURCE_VIEW_LINE_BASE;

    i32 iconWidth6;
    i32 iconHeight11;
    i32 textY19;
    i32 lineCount10;
    char* messageText2;
    char* resourceCountText6;

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
        DATA_COMPGEN(0x004f61f8, updBottomViewResMsgStonbackIcn, "stonback.icn"),
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

    textY19 = 0;
    if (giBottomViewResource == RES_NONE) {
        textY19 = RESOURCE_VIEW_MULTILINE_HEIGHT;
        lineCount10 = smallFont->LineLength(gcBottomViewText, BOTTOM_VIEW_PANEL_WIDTH);
        textY19 -= lineCount10 * RESOURCE_VIEW_LINE_HEIGHT;
    }
    messageText2 = static_cast<char*>(H2_ALLOC_AT(
        strlen(gcBottomViewText) + 1, DATA_COMPGEN(0x004f6208, updBottomViewResMsgSourceFile, RETAIL_FILE),
        RESOURCE_VIEW_LINE_BASE + IDX(RESOURCE_VIEW_MESSAGE_ALLOC_LINE_OFFSET)
    ));
    sprintf(messageText2, gcBottomViewText);
    m_bottomViewAllTexts[0] = new textWidget(
        BOTTOM_VIEW_PANEL_X,
        textY19 + RESOURCE_VIEW_TEXT_BASE_Y,
        BOTTOM_VIEW_PANEL_WIDTH,
        RESOURCE_VIEW_TEXT_HEIGHT,
        messageText2,
        DATA_COMPGEN(0x004f6234, updBottomViewResMsgSmalfontFnt, "smalfont.fnt"),
        FONT_DRAW_DEFAULT,
        BOTTOM_VIEW_TEXT_ID,
        WIDGET_KIND_TEXT,
        FONT_ALIGN_CENTER
    );
    if (m_bottomViewAllTexts[0] == NULL) {
        MemError();
    }
    m_adventureWindow->AddWidget(m_bottomViewAllTexts[0], -1);

    if (giBottomViewResource != RES_NONE) {
        if (giBottomViewResource == RES_GOLD) {
            iconWidth6 = RESOURCE_VIEW_GOLD_WIDTH;
            iconHeight11 = RESOURCE_VIEW_GOLD_HEIGHT;
        } else {
            iconWidth6 = RESOURCE_VIEW_ICON_WIDTH;
            iconHeight11 = RESOURCE_VIEW_ICON_HEIGHT;
        }
        m_bottomViewHourglassBackground = new iconWidget(
            (BOTTOM_VIEW_PANEL_WIDTH - iconWidth6) / BOTTOM_VIEW_CENTER_DIVISOR
                + BOTTOM_VIEW_PANEL_X,
            RESOURCE_VIEW_ICON_BOTTOM - iconHeight11
                - RESOURCE_VIEW_ICON_BOTTOM_PADDING,
            iconWidth6,
            iconHeight11,
            DATA_COMPGEN(0x004f6244, updBottomViewResMsgResourceIcn, "resource.icn"),
            IDX(giBottomViewResource),
            ICON_DRAW_NORMAL,
            BOTTOM_VIEW_FOREGROUND_ID,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (m_bottomViewHourglassBackground == NULL) {
            MemError();
        }
        m_adventureWindow->AddWidget(m_bottomViewHourglassBackground, -1);

        resourceCountText6 = static_cast<char*>(H2_ALLOC_AT(
            BOTTOM_VIEW_COUNT_BUFFER_SIZE, DATA_COMPGEN(0x004f6254, updBottomViewResMsgSourceFile2, RETAIL_FILE),
            RESOURCE_VIEW_LINE_BASE + IDX(RESOURCE_VIEW_COUNT_ALLOC_LINE_OFFSET)
        ));
        sprintf(resourceCountText6, DATA_COMPGEN(0x004f6280, updBottomViewResMsgD, "%d"), giBottomViewResourceQty);
        m_bottomViewAllTexts[1] = new textWidget(
            RESOURCE_VIEW_COUNT_X,
            RESOURCE_VIEW_COUNT_Y,
            RESOURCE_VIEW_COUNT_WIDTH,
            RESOURCE_VIEW_COUNT_HEIGHT,
            resourceCountText6,
            DATA_COMPGEN(0x004f6284, updBottomViewResMsgSecondarySmalfontFnt, "smalfont.fnt"),
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

VA(0x00461a75, 0x363)
i32 advManager::UpdBottomViewKingdom(void) {
    DATA(0x004f6294) static i16 s_kingdomViewLineBase = KINGDOM_VIEW_LINE_BASE;

    i32 villageCount37;
    i32 index11;
    i32 castleCount12;
    i8 textY5[KINGDOM_VIEW_ENTRY_COUNT];
    u8 textX[KINGDOM_VIEW_ENTRY_COUNT];
    char* countText14[KINGDOM_VIEW_ENTRY_COUNT];

    if (!gbForceUpdate && iCurBottomView == BOTTOM_VIEW_KINGDOM) {
        return 0;
    }

    ClearBottomView();
    iCurBottomView = BOTTOM_VIEW_KINGDOM;
    textY5[IDX(RES_WOOD)] = KINGDOM_VIEW_RESOURCE_TEXT_Y;
    textY5[IDX(RES_MERCURY)] = KINGDOM_VIEW_RESOURCE_TEXT_Y;
    textY5[IDX(RES_ORE)] = KINGDOM_VIEW_RESOURCE_TEXT_Y;
    textY5[IDX(RES_SULFUR)] = KINGDOM_VIEW_RESOURCE_TEXT_Y;
    textY5[IDX(RES_CRYSTAL)] = KINGDOM_VIEW_RESOURCE_TEXT_Y;
    textY5[IDX(RES_GEMS)] = KINGDOM_VIEW_RESOURCE_TEXT_Y;
    textY5[IDX(RES_GOLD)] = KINGDOM_VIEW_TOWN_TEXT_Y;
    textY5[KINGDOM_VIEW_CASTLE_ENTRY] = KINGDOM_VIEW_TOWN_TEXT_Y;
    textY5[KINGDOM_VIEW_TOWN_ENTRY] = KINGDOM_VIEW_TOWN_TEXT_Y;
    textX[IDX(RES_WOOD)] = KINGDOM_VIEW_WOOD_TEXT_X;
    textX[IDX(RES_MERCURY)] = KINGDOM_VIEW_MERCURY_TEXT_X;
    textX[IDX(RES_ORE)] = KINGDOM_VIEW_ORE_TEXT_X;
    textX[IDX(RES_SULFUR)] = KINGDOM_VIEW_SULFUR_TEXT_X;
    textX[IDX(RES_CRYSTAL)] = KINGDOM_VIEW_CRYSTAL_TEXT_X;
    textX[IDX(RES_GEMS)] = KINGDOM_VIEW_GEMS_TEXT_X;
    textX[IDX(RES_GOLD)] = KINGDOM_VIEW_GOLD_TEXT_X;
    textX[KINGDOM_VIEW_CASTLE_ENTRY] = KINGDOM_VIEW_CASTLE_TEXT_X;
    textX[KINGDOM_VIEW_TOWN_ENTRY] = KINGDOM_VIEW_VILLAGE_TEXT_X;
    villageCount37 = 0;
    castleCount12 = 0;

    m_bottomViewBackground = new iconWidget(
        BOTTOM_VIEW_PANEL_X,
        BOTTOM_VIEW_PANEL_Y,
        BOTTOM_VIEW_BACKGROUND_WIDTH,
        BOTTOM_VIEW_PANEL_HEIGHT,
        DATA_COMPGEN(0x004f6298, updBottomViewKingdomStonbackIcn, "stonback.icn"),
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
        DATA_COMPGEN(0x004f62a8, updBottomViewKingdomRessmallIcn, "ressmall.icn"),
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

    for (index11 = 0; index11 < gpCurPlayer->m_townCount; ++index11) {
        if (gpGame->m_castleRecs[gpCurPlayer->m_townIds[index11]].m_buildings
            & IDX(TOWN_BUILDING_CASTLE)) {
            ++castleCount12;
        } else {
            ++villageCount37;
        }
    }

    for (index11 = 0; index11 < KINGDOM_VIEW_ENTRY_COUNT; ++index11) {
        countText14[index11] = static_cast<char*>(H2_ALLOC_AT(
            BOTTOM_VIEW_COUNT_BUFFER_SIZE, DATA_COMPGEN(0x004f62b8, updBottomViewKingdomSourceFile, RETAIL_FILE),
            KINGDOM_VIEW_LINE_BASE + IDX(KINGDOM_VIEW_COUNT_ALLOC_LINE_OFFSET)
        ));
        if (index11 < KINGDOM_VIEW_RESOURCE_COUNT) {
            sprintf(countText14[index11], DATA_COMPGEN(0x004f62e4, updBottomViewKingdomD, "%d"), gpCurPlayer->m_resources[index11]);
        } else if (index11 == KINGDOM_VIEW_CASTLE_ENTRY) {
            sprintf(countText14[index11], DATA_COMPGEN(0x004f62e8, updBottomViewKingdomD2, "%d"), castleCount12);
        } else {
            sprintf(countText14[index11], DATA_COMPGEN(0x004f62ec, updBottomViewKingdomD3, "%d"), villageCount37);
        }

        m_bottomViewAllTexts[index11] = new textWidget(
            textX[index11] + KINGDOM_VIEW_TEXT_X_BASE,
            textY5[index11] + KINGDOM_VIEW_TEXT_Y_BASE,
            KINGDOM_VIEW_TEXT_WIDTH,
            KINGDOM_VIEW_TEXT_HEIGHT,
            countText14[index11],
            DATA_COMPGEN(0x004f62f0, updBottomViewKingdomSmalfontFnt, "smalfont.fnt"),
            FONT_DRAW_DEFAULT,
            index11 + BOTTOM_VIEW_TEXT_ID,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (m_bottomViewAllTexts[index11] == NULL) {
            MemError();
        }
        m_adventureWindow->AddWidget(m_bottomViewAllTexts[index11], -1);
    }
    return 1;
}

VA(0x00461dd8, 0x583)
i32 advManager::UpdBottomViewHero(void) {
    DATA(0x004f6300) static i16 s_bottomHeroLineBase = BOTTOM_HERO_LINE_BASE;

    char* armyCountLabelsResult[BOTTOM_HERO_ARMY_SLOTS];
    icon* monsterIconsLocal;
    i32 occupiedSlotsLocal;
    hero* targetHero;
    i32 armySlot;
    i32 displayIndexData;
    i32 creature;
    IconEntry* iconEntryValue;
    i32 groupWidthRef;
    i32 layoutIndexIndex;
    i32 iconX;
    i32 iconY;
    i32 labelY;
    i32 labelWidthCount;
    i32 labelX;
    i32 creatureBoundsLocal[BOTTOM_HERO_CREATURE_BOUND_COUNT];

    if (!gbForceUpdate && iCurBottomView == BOTTOM_VIEW_HERO) {
        return 0;
    }

    ClearBottomView();
    iCurBottomView = BOTTOM_VIEW_HERO;
    targetHero = gpGame->GetHero(gpCurPlayer->CurrentHero());
    occupiedSlotsLocal = 0;

    m_bottomViewBackground = new iconWidget(
        BOTTOM_HERO_PANEL_X,
        BOTTOM_HERO_PANEL_Y,
        BOTTOM_HERO_PANEL_WIDTH,
        BOTTOM_HERO_PANEL_HEIGHT,
        DATA_COMPGEN(0x004f6304, updBottomViewHeroStonbackIcn, "stonback.icn"),
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

    for (armySlot = 0; armySlot < BOTTOM_HERO_ARMY_SLOTS; ++armySlot) {
        if (targetHero->m_army.m_creatureTypes[armySlot] != CREATURE_NONE) {
            ++occupiedSlotsLocal;
        }
    }

    if (occupiedSlotsLocal != 0) {
        displayIndexData = 0;
        monsterIconsLocal = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f6314, updBottomViewHeroMons32Icn, "mons32.icn"));
        for (armySlot = 0; armySlot < BOTTOM_HERO_ARMY_SLOTS; ++armySlot) {
            creature = IDX(targetHero->m_army.m_creatureTypes[armySlot]);
            if (creature != BOTTOM_HERO_EMPTY_SLOT) {
                // Retail stores eight compact (x, y) layout anchors and indexes them through
                // the occupied-slot table below; the coordinate payload has no separate domain.
                // NOLINTBEGIN(readability-magic-numbers)
                u8 iconPositions[BOTTOM_HERO_ICON_POSITION_BYTES] =
                    {50, 3, 96, 3, 50, 17, 73, 17, 96, 17, 27, 32, 73, 32, 119, 32};
                i8 armyLayouts[BOTTOM_HERO_ARMY_SLOTS][BOTTOM_HERO_ARMY_SLOTS] = {
                    {3, -1, -1, -1, -1},
                    {2, 4, -1, -1, -1},
                    {0, 1, 6, -1, -1},
                    {0, 1, 5, 6, -1},
                    {0, 1, 5, 6, 7}
                };
                // NOLINTEND(readability-magic-numbers)

                armyCountLabelsResult[displayIndexData] = static_cast<char*>(H2_ALLOC_AT(
                    BOTTOM_HERO_LABEL_BYTES, DATA_COMPGEN(0x004f6320, updBottomViewHeroSourceFile, RETAIL_FILE),
                    BOTTOM_HERO_LINE_BASE + BOTTOM_HERO_ALLOC_LINE_OFFSET
                ));
                if (targetHero->m_army.m_creatureCounts[armySlot]
                    > BOTTOM_HERO_MAX_FULL_COUNT) {
                    sprintf(
                        armyCountLabelsResult[displayIndexData],
                        DATA_COMPGEN(0x004f634c, updBottomViewHeroDk, "%dk"),
                        targetHero->m_army.m_creatureCounts[armySlot]
                            / BOTTOM_HERO_COUNT_DIVISOR
                    );
                } else {
                    sprintf(
                        armyCountLabelsResult[displayIndexData],
                        DATA_COMPGEN(0x004f6350, updBottomViewHeroD, "%d"),
                        targetHero->m_army.m_creatureCounts[armySlot]
                    );
                }

                layoutIndexIndex = armyLayouts[occupiedSlotsLocal - 1][displayIndexData];
                iconX = iconPositions[layoutIndexIndex * BOTTOM_HERO_POSITION_COMPONENT_COUNT];
                iconY = iconPositions[
                    layoutIndexIndex * BOTTOM_HERO_POSITION_COMPONENT_COUNT + 1
                ];
                labelY = iconY + BOTTOM_HERO_LABEL_Y_OFFSET;
                iconEntryValue = reinterpret_cast<IconEntry*>(
                    creature * sizeof(IconEntry) + monsterIconsLocal->m_data
                );
                if (layoutIndexIndex == 0 || layoutIndexIndex == 1) {
                    labelY -= BOTTOM_HERO_TOP_LABEL_SHIFT;
                    if (iconEntryValue->h < BOTTOM_HERO_TOP_MIN_HEIGHT) {
                        iconY += BOTTOM_HERO_TOP_MIN_HEIGHT - iconEntryValue->h;
                    }
                } else if (iconEntryValue->h < BOTTOM_HERO_LOWER_MIN_HEIGHT) {
                    iconY += BOTTOM_HERO_LOWER_MIN_HEIGHT - iconEntryValue->h;
                }

                labelWidthCount = smallFont->LineWidth(armyCountLabelsResult[displayIndexData]);
                groupWidthRef = iconEntryValue->w + labelWidthCount;
                if (groupWidthRef > BOTTOM_HERO_GROUP_WIDTH) {
                    groupWidthRef = BOTTOM_HERO_GROUP_WIDTH;
                }
                iconX -= (groupWidthRef + 1) / BOTTOM_VIEW_CENTER_DIVISOR;
                labelX = groupWidthRef - 1 + iconX - (labelWidthCount - 1);

                m_bottomViewIcons[displayIndexData] = new iconWidget(
                    iconX + BOTTOM_HERO_PANEL_X,
                    iconY + BOTTOM_HERO_PANEL_Y,
                    BOTTOM_HERO_ICON_WIDTH,
                    BOTTOM_HERO_ICON_HEIGHT,
                    DATA_COMPGEN(0x004f6354, updBottomViewHeroSecondaryMons32Icn, "mons32.icn"),
                    creature,
                    ICON_DRAW_NORMAL,
                    displayIndexData + BOTTOM_HERO_FIRST_ICON_ID,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
                if (m_bottomViewIcons[displayIndexData] == NULL) {
                    MemError();
                }

                m_bottomViewTexts[displayIndexData] = new textWidget(
                    labelX + BOTTOM_HERO_PANEL_X,
                    labelY + BOTTOM_HERO_PANEL_Y,
                    ((targetHero->m_army.m_creatureCounts[armySlot]
                      <= BOTTOM_HERO_MAX_FULL_COUNT)
                         - 1
                     & BOTTOM_HERO_ABBREVIATED_LABEL_PADDING)
                        + strlen(armyCountLabelsResult[displayIndexData])
                              * BOTTOM_HERO_CHARACTER_WIDTH,
                    BOTTOM_HERO_LABEL_HEIGHT,
                    armyCountLabelsResult[displayIndexData],
                    DATA_COMPGEN(0x004f6360, updBottomViewHeroSmalfontFnt, "smalfont.fnt"),
                    FONT_DRAW_DEFAULT,
                    displayIndexData + BOTTOM_HERO_FIRST_TEXT_ID,
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
                if (m_bottomViewTexts[displayIndexData] == NULL) {
                    MemError();
                }

                m_adventureWindow->AddWidget(m_bottomViewIcons[displayIndexData], -1);
                m_adventureWindow->AddWidget(m_bottomViewTexts[displayIndexData], -1);
                ++displayIndexData;
            }
        }
        gpResourceManager->Dispose(monsterIconsLocal);
    }
    return 1;
}

VA(0x0046235b, 0xd32)
void advManager::HeroQuickView(i32 heroId, i32 locatorSlot, i32 windowX, i32 windowY) {
    DATA(0x004f6370) static i16 s_quickViewLineBase = QUICK_VIEW_LINE_BASE;

    i16 armyAreaWidthLocal = HERO_QUICK_ARMY_AREA_WIDTH;
    i16 armyAreaLeftLocal = ARMY_QUICK_AREA_LEFT;
    i16 detailedCreatureY = HERO_QUICK_DETAILED_CREATURE_Y;
    i16 stackIconWidthData = ARMY_QUICK_ICON_SIZE;
    i16 creatureIconHeight = ARMY_QUICK_ICON_SIZE;
    i16 widgetEnableFlagLocal = 1;
    i16 portraitWidgetLocal = HERO_QUICK_PORTRAIT_WIDGET;
    i16 primaryStatsWidgetValue = HERO_QUICK_PRIMARY_STAT_WIDGET;
    i16 playerColorWidgetId = HERO_QUICK_PLAYER_COLOR_WIDGET;
    iconWidget* stackIconsWidgets[ARMY_QUICK_SLOT_COUNT];
    textWidget* creatureTextWidgetsLocal[ARMY_QUICK_SLOT_COUNT];
    char* armyLabelsStrings[ARMY_QUICK_SLOT_COUNT];
    tag_message quickViewMessageState;
    icon* monsterIconRef;
    hero* targetHero;
    heroWindow* quickWindowSlot;
    i32 visibleArmyCountState;
    i32 armyIndex;
    i32 previousOriginXState;
    i32 savedOriginY;

    quickViewMessageState.type = MESSAGE_WIDGET;
    if (heroId == INVALID_HERO) {
        return;
    }

    monsterIconRef = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f6374, heroQuickViewMons32Icn, "mons32.icn"));
    targetHero = gpGame->GetHero(heroId);
    if (targetHero->m_owner == giCurPlayer || m_identifyHeroActive == 1
        || IsCrystalBallInEffect(targetHero->m_x, targetHero->m_y, CRYSTAL_BALL_RADIUS)) {
        if (windowX == -1) {
            windowX = HERO_QUICK_DEFAULT_WINDOW_X;
            windowY = locatorSlot * HERO_QUICK_LOCATOR_ROW_HEIGHT + HERO_QUICK_LOCATOR_BASE_Y;
        }
        quickWindowSlot = new heroWindow(windowX, windowY, DATA_COMPGEN(0x004f6380, heroQuickViewQhero0Bin, "qhero0.bin"));
        if (quickWindowSlot == NULL) {
            MemError();
        }
        SetWinText(quickWindowSlot, HERO_QUICK_WINDOW_TEXT);
    } else {
        quickWindowSlot = new heroWindow(windowX, windowY, DATA_COMPGEN(0x004f638c, heroQuickViewQhero1Bin, "qhero1.bin"));
        if (quickWindowSlot == NULL) {
            MemError();
        }
    }

    quickViewMessageState.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    quickViewMessageState.payload.widget.id = HERO_QUICK_PORTRAIT_WIDGET;
    quickViewMessageState.payload.widget.data.value = IDX(targetHero->m_portrait);
    quickWindowSlot->BroadcastMessage(quickViewMessageState);
    quickViewMessageState.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    quickViewMessageState.payload.widget.id = HERO_QUICK_PLAYER_COLOR_WIDGET;
    quickViewMessageState.payload.widget.data.value =
        gpGame->GetPlayerColor(targetHero->m_owner) * HERO_QUICK_PLAYER_COLOR_STRIDE;
    quickWindowSlot->BroadcastMessage(quickViewMessageState);
    ++quickViewMessageState.payload.widget.id;
    ++quickViewMessageState.payload.widget.data.value;
    quickWindowSlot->BroadcastMessage(quickViewMessageState);
    sprintf(gText, DATA_COMPGEN(0x004f6398, heroQuickViewS, "%s"), targetHero->m_name);
    quickViewMessageState.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    quickViewMessageState.payload.widget.id = HERO_QUICK_NAME_WIDGET;
    quickViewMessageState.payload.widget.data.text = gText;
    quickWindowSlot->BroadcastMessage(quickViewMessageState);

    visibleArmyCountState = 0;
    for (armyIndex = 0; armyIndex < ARMY_QUICK_SLOT_COUNT; ++armyIndex) {
        if (targetHero->m_army.m_creatureTypes[armyIndex] != CREATURE_NONE) {
            ++visibleArmyCountState;
        }
    }

    if (targetHero->m_owner == giCurPlayer || m_identifyHeroActive == 1
        || IsCrystalBallInEffect(targetHero->m_x, targetHero->m_y, CRYSTAL_BALL_RADIUS)) {
        for (armyIndex = 0; armyIndex < HERO_PRIMARY_STAT_COUNT; ++armyIndex) {
            sprintf(gText, DATA_COMPGEN(0x004f639c, heroQuickViewD, "%d"), targetHero->Stats(HeroPrimaryStat(armyIndex)));
            quickViewMessageState.payload.widget.id = armyIndex + HERO_QUICK_PRIMARY_STAT_WIDGET;
            quickViewMessageState.payload.widget.data.text = gText;
            quickWindowSlot->BroadcastMessage(quickViewMessageState);
        }
        sprintf(
            gText,
            DATA_COMPGEN(0x004f63a0, heroQuickViewDD, "%d/%d"),
            targetHero->m_spellPoints,
            targetHero->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE
        );
        quickViewMessageState.payload.widget.id = HERO_QUICK_MANA_WIDGET;
        quickViewMessageState.payload.widget.data.text = gText;
        quickWindowSlot->BroadcastMessage(quickViewMessageState);

        if (visibleArmyCountState != 0) {
            i32 armyStartPosition =
                (HERO_QUICK_ARMY_AREA_WIDTH - visibleArmyCountState * ARMY_QUICK_ICON_SIZE)
                    / ARMY_QUICK_CENTER_DIVISOR
                + ARMY_QUICK_AREA_LEFT;
            i32 displayIndexStateOffset = 0;
            i32 creature;
            for (armyIndex = 0; armyIndex < visibleArmyCountState; ++armyIndex) {
                while (targetHero->m_army.m_creatureTypes[displayIndexStateOffset]
                       == CREATURE_NONE) {
                    ++displayIndexStateOffset;
                }
                creature = IDX(targetHero->m_army.m_creatureTypes[displayIndexStateOffset]);
                if (creature != ARMY_QUICK_EMPTY_SLOT) {
                    stackIconsWidgets[armyIndex] = new iconWidget(
                        static_cast<i16>(
                            armyIndex * ARMY_QUICK_ICON_SIZE + armyStartPosition
                            - GetIconEntry(monsterIconRef, creature)->x
                            + (ARMY_QUICK_ICON_SIZE - GetIconEntry(monsterIconRef, creature)->w)
                                  / ARMY_QUICK_CENTER_DIVISOR
                            + 1
                        ),
                        static_cast<i16>(
                            HERO_QUICK_DETAILED_CREATURE_Y
                            - GetIconEntry(monsterIconRef, creature)->y
                            - GetIconEntry(monsterIconRef, creature)->h
                            + ARMY_QUICK_ICON_BASELINE
                        ),
                        ARMY_QUICK_ICON_SIZE,
                        ARMY_QUICK_ICON_SIZE,
                        DATA_COMPGEN(0x004f63a8, heroQuickViewMons32Icn2, "mons32.icn"),
                        static_cast<i16>(creature),
                        ICON_DRAW_NORMAL,
                        -1,
                        WIDGET_KIND_ICON_DIRECT,
                        1
                    );
                    if (stackIconsWidgets[armyIndex] == NULL) {
                        MemError();
                    }
                    armyLabelsStrings[armyIndex] = static_cast<char*>(
                        H2_ALLOC_AT(
                            HERO_QUICK_ARMY_LABEL_CAPACITY, DATA_COMPGEN(0x004f63b4, heroQuickViewSourceFile, RETAIL_FILE),
                            QUICK_VIEW_LINE_BASE + QUICK_VIEW_FIRST_ALLOC_LINE_OFFSET
                        )
                    );
                    sprintf(
                        armyLabelsStrings[armyIndex],
                        DATA_COMPGEN(0x004f63e0, heroQuickViewD2, "%d"),
                        targetHero->m_army.m_creatureCounts[displayIndexStateOffset]
                    );
                    creatureTextWidgetsLocal[armyIndex] = new textWidget(
                        static_cast<i16>(armyIndex * ARMY_QUICK_ICON_SIZE + armyStartPosition),
                        static_cast<i16>(
                            HERO_QUICK_DETAILED_CREATURE_Y + IDX(ARMY_QUICK_ICON_SIZE)
                        ),
                        ARMY_QUICK_ICON_SIZE,
                        ARMY_QUICK_LABEL_HEIGHT,
                        armyLabelsStrings[armyIndex],
                        DATA_COMPGEN(0x004f63e4, heroQuickViewSmalfontFnt, "smalfont.fnt"),
                        FONT_DRAW_DEFAULT,
                        -1,
                        WIDGET_KIND_TEXT,
                        FONT_ALIGN_CENTER
                    );
                    if (creatureTextWidgetsLocal[armyIndex] == NULL) {
                        MemError();
                    }
                    quickWindowSlot->AddWidget(stackIconsWidgets[armyIndex], -1);
                    quickWindowSlot->AddWidget(creatureTextWidgetsLocal[armyIndex], -1);
                }
                ++displayIndexStateOffset;
            }
        }
    } else if (visibleArmyCountState != 0) {
        i32 rowYCurrent = HERO_QUICK_VAGUE_FIRST_ROW_Y;
        i32 topRowCount;
        i32 secondRowCountTotal;
        i32 creatureTypeId;
        switch (visibleArmyCountState) {
            case 1:
            case ARMY_QUICK_FIRST_ROW_COUNT:
            case ARMY_QUICK_TOP_ROW_MAX:
                rowYCurrent += ARMY_QUICK_FIRST_ROW_SHIFT;
                topRowCount = visibleArmyCountState;
                secondRowCountTotal = 0;
                break;
            case ARMY_QUICK_FOUR_STACK_COUNT:
                topRowCount = ARMY_QUICK_FIRST_ROW_COUNT;
                secondRowCountTotal = ARMY_QUICK_FIRST_ROW_COUNT;
                break;
            default:
                topRowCount = ARMY_QUICK_FIRST_ROW_COUNT;
                secondRowCountTotal = ARMY_QUICK_TOP_ROW_MAX;
                break;
        }

        i32 displayIndexValue = 0;
        i32 armySpacing = HERO_QUICK_ARMY_AREA_WIDTH / topRowCount;
        i32 slotStartPosition = (armySpacing - ARMY_QUICK_ICON_SIZE)
                / ARMY_QUICK_CENTER_DIVISOR
            + ARMY_QUICK_AREA_LEFT;
        for (armyIndex = 0; armyIndex < topRowCount; ++armyIndex) {
            while (targetHero->m_army.m_creatureTypes[displayIndexValue]
                   == CREATURE_NONE) {
                ++displayIndexValue;
            }
            creatureTypeId = IDX(targetHero->m_army.m_creatureTypes[displayIndexValue]);
            stackIconsWidgets[armyIndex] = new iconWidget(
                static_cast<i16>(
                    armyIndex * armySpacing + slotStartPosition
                    - GetIconEntry(monsterIconRef, creatureTypeId)->x
                    + (ARMY_QUICK_ICON_SIZE - GetIconEntry(monsterIconRef, creatureTypeId)->w)
                          / ARMY_QUICK_CENTER_DIVISOR
                    + 1
                ),
                static_cast<i16>(
                    rowYCurrent - GetIconEntry(monsterIconRef, creatureTypeId)->y
                    - GetIconEntry(monsterIconRef, creatureTypeId)->h + ARMY_QUICK_ICON_BASELINE
                ),
                ARMY_QUICK_ICON_SIZE,
                ARMY_QUICK_ICON_SIZE,
                DATA_COMPGEN(0x004f63f4, heroQuickViewMons32Icn3, "mons32.icn"),
                static_cast<i16>(creatureTypeId),
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (stackIconsWidgets[armyIndex] == NULL) {
                MemError();
            }
            armyLabelsStrings[armyIndex] =
                static_cast<char*>(H2_ALLOC_AT(15, DATA_COMPGEN(0x004f6400, heroQuickViewSourceFile2, RETAIL_FILE), 4982 + QUICK_VIEW_SECOND_ALLOC_LINE_OFFSET));
            strcpy(
                armyLabelsStrings[armyIndex],
                GetArmySizeName(
                    targetHero->m_army.m_creatureCounts[displayIndexValue],
                    ARMY_SIZE_NAME_TITLE
                )
            );
            creatureTextWidgetsLocal[armyIndex] = new textWidget(
                static_cast<i16>(armyIndex * armySpacing + ARMY_QUICK_AREA_LEFT),
                static_cast<i16>(rowYCurrent + ARMY_QUICK_ICON_SIZE),
                armySpacing,
                ARMY_QUICK_LABEL_HEIGHT,
                armyLabelsStrings[armyIndex],
                DATA_COMPGEN(0x004f642c, heroQuickViewSmalfontFnt2, "smalfont.fnt"),
                FONT_DRAW_DEFAULT,
                -1,
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (creatureTextWidgetsLocal[armyIndex] == NULL) {
                MemError();
            }
            quickWindowSlot->AddWidget(stackIconsWidgets[armyIndex], -1);
            quickWindowSlot->AddWidget(creatureTextWidgetsLocal[armyIndex], -1);
            ++displayIndexValue;
        }

        if (secondRowCountTotal != 0) {
            armySpacing = HERO_QUICK_ARMY_AREA_WIDTH / secondRowCountTotal;
            slotStartPosition = (armySpacing - ARMY_QUICK_ICON_SIZE)
                    / ARMY_QUICK_CENTER_DIVISOR
                + ARMY_QUICK_AREA_LEFT;
            rowYCurrent += ARMY_QUICK_SECOND_ROW_SHIFT;
            for (armyIndex = topRowCount; armyIndex < topRowCount + secondRowCountTotal;
                 ++armyIndex) {
                while (targetHero->m_army.m_creatureTypes[displayIndexValue] == CREATURE_NONE) {
                    ++displayIndexValue;
                }
                creatureTypeId = IDX(targetHero->m_army.m_creatureTypes[displayIndexValue]);
                stackIconsWidgets[armyIndex] = new iconWidget(
                    static_cast<i16>(
                        (armyIndex - HERO_QUICK_SECOND_ROW_FIRST_SLOT) * armySpacing
                        + slotStartPosition
                        - GetIconEntry(monsterIconRef, creatureTypeId)->x
                        + (ARMY_QUICK_ICON_SIZE - GetIconEntry(monsterIconRef, creatureTypeId)->w)
                              / ARMY_QUICK_CENTER_DIVISOR
                        + 1
                    ),
                    static_cast<i16>(
                        rowYCurrent - GetIconEntry(monsterIconRef, creatureTypeId)->y
                        - GetIconEntry(monsterIconRef, creatureTypeId)->h
                        + ARMY_QUICK_ICON_BASELINE + ARMY_QUICK_SECOND_ROW_ICON_SHIFT
                    ),
                    ARMY_QUICK_ICON_SIZE,
                    ARMY_QUICK_ICON_SIZE,
                    DATA_COMPGEN(0x004f643c, heroQuickViewMons32Icn4, "mons32.icn"),
                    static_cast<i16>(creatureTypeId),
                    ICON_DRAW_NORMAL,
                    -1,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
                if (stackIconsWidgets[armyIndex] == NULL) {
                    MemError();
                }
                armyLabelsStrings[armyIndex] = static_cast<char*>(
                    H2_ALLOC_AT(15, DATA_COMPGEN(0x004f6448, heroQuickViewSourceFile3, RETAIL_FILE), 4982 + QUICK_VIEW_THIRD_ALLOC_LINE_OFFSET)
                );
                strcpy(
                    armyLabelsStrings[armyIndex],
                    GetArmySizeName(
                        targetHero->m_army.m_creatureCounts[displayIndexValue],
                        ARMY_SIZE_NAME_TITLE
                    )
                );
                creatureTextWidgetsLocal[armyIndex] = new textWidget(
                    static_cast<i16>(
                        (armyIndex - HERO_QUICK_SECOND_ROW_FIRST_SLOT) * armySpacing
                        + ARMY_QUICK_AREA_LEFT
                    ),
                    static_cast<i16>(rowYCurrent + HERO_QUICK_SECOND_ROW_TEXT_SHIFT),
                    armySpacing,
                    ARMY_QUICK_LABEL_HEIGHT,
                    armyLabelsStrings[armyIndex],
                    DATA_COMPGEN(0x004f6474, heroQuickViewSmalfontFnt3, "smalfont.fnt"),
                    FONT_DRAW_DEFAULT,
                    -1,
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
                if (creatureTextWidgetsLocal[armyIndex] == NULL) {
                    MemError();
                }
                quickWindowSlot->AddWidget(stackIconsWidgets[armyIndex], -1);
                quickWindowSlot->AddWidget(creatureTextWidgetsLocal[armyIndex], -1);
                ++displayIndexValue;
            }
        }
    }

    previousOriginXState = m_mapOriginX;
    savedOriginY = m_mapOriginY;
    m_mapOriginX = targetHero->m_x - VIEW_CENTER_CELL;
    m_mapOriginY = targetHero->m_y - VIEW_CENTER_CELL;
    UpdateRadar(1, 0);
    gpWindowManager->AddWindow(quickWindowSlot, -1, 1);
    QuickViewWait();
    gpWindowManager->RemoveWindow(quickWindowSlot);
    delete quickWindowSlot;
    m_mapOriginX = previousOriginXState;
    m_mapOriginY = savedOriginY;
    UpdateRadar(1, 0);
    CompleteDraw(0);
    UpdateScreen(0, 0);
    if (quickViewMessageState.type == MESSAGE_LEFT_BUTTON_DOWN
        && targetHero->m_owner == giCurPlayer) {
        SetHeroContext(targetHero->m_id, 0);
    }
    gpResourceManager->Dispose(monsterIconRef);
}

VA(0x0046308d, 0x120)
char* advManager::GetArmySizeName(
    i32 armySize,
    H2_ENUM_PARAM(ArmySizeNameVariant, i32) grammar
) {
    if (giDebugLevel > 0) {
        sprintf(cArmySizeName, DATA_COMPGEN(0x004f6484, getArmySizeNameD, "%d"), armySize);
        return cArmySizeName;
    }
    if (armySize < ARMY_FEW_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_FEW][IDX(grammar)];
    }
    if (armySize < ARMY_SEVERAL_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_SEVERAL][IDX(grammar)];
    }
    if (armySize < ARMY_PACK_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_PACK][IDX(grammar)];
    }
    if (armySize < ARMY_LOTS_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_LOTS][IDX(grammar)];
    }
    if (armySize < ARMY_HORDE_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_HORDE][IDX(grammar)];
    }
    if (armySize < ARMY_THRONG_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_THRONG][IDX(grammar)];
    }
    if (armySize < ARMY_SWARM_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_SWARM][IDX(grammar)];
    }
    if (armySize < ARMY_ZOUNDS_LIMIT) {
        return gArmySizeNames[ARMY_SIZE_ZOUNDS][IDX(grammar)];
    }
    return gArmySizeNames[ARMY_SIZE_LEGION][IDX(grammar)];
}

VA(0x004631ad, 0xc29)
void advManager::TownQuickView(i32 townId, i32 locatorSlot, i32 windowX, i32 windowY) {
    DATA(0x004f6488) static i16 s_townViewLineBase = TOWN_VIEW_LINE_BASE;

    icon* monsterIconLocal;
    i16 portraitWidgetLocal;
    i16 armyIconHeightState;
    tag_message messageLocal;
    i32 armyCountLocal;
    i32 armyIndex;
    i16 armyIconWidthState;
    i16 widgetEnabledData;
    i16 colorWidgetValue;
    i32 previousOriginXValue;
    heroWindow* townQuickWindow;
    town* quickTownLocal;
    i32 previousOriginYSlot;
    i32 informationLevel;
    char* emptyArmyLabel;
    i16 armyAreaWidth;
    i16 armyAreaLeftValue;
    widget* emptyArmyTextState;

    armyAreaWidth = TOWN_QUICK_ARMY_AREA_WIDTH;
    armyAreaLeftValue = ARMY_QUICK_AREA_LEFT;
    armyIconWidthState = ARMY_QUICK_ICON_SIZE;
    armyIconHeightState = ARMY_QUICK_ICON_SIZE;
    widgetEnabledData = 1;
    portraitWidgetLocal = TOWN_QUICK_PORTRAIT_WIDGET;
    colorWidgetValue = TOWN_QUICK_PLAYER_COLOR_WIDGET;

    if (townId == INVALID_HERO) {
        return;
    }

    monsterIconLocal = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f648c, townQuickViewMons32Icn, "mons32.icn"));
    quickTownLocal = gpGame->GetTown(townId);
    if (windowX == -1) {
        windowX = TOWN_QUICK_DEFAULT_WINDOW_X;
        windowY = TOWN_QUICK_DEFAULT_WINDOW_Y;
    }
    townQuickWindow = new heroWindow(windowX, windowY, DATA_COMPGEN(0x004f6498, townQuickViewQtown1Bin, "qtown1.bin"));
    if (townQuickWindow == NULL) {
        MemError();
    }

    if (quickTownLocal->m_owner == giCurPlayer
        || giDebugLevel >= TOWN_QUICK_DEBUG_INFORMATION) {
        informationLevel = TOWN_QUICK_INFORMATION_EXACT;
    } else {
        informationLevel = gpGame->GetNumThievesGuilds(giCurPlayer);
        if (informationLevel > TOWN_QUICK_INFORMATION_ESTIMATES) {
            informationLevel = TOWN_QUICK_INFORMATION_ESTIMATES;
        }
    }
    if (IsCrystalBallInEffect(
            quickTownLocal->m_x,
            quickTownLocal->m_y,
            CRYSTAL_BALL_RADIUS
        )) {
        informationLevel = TOWN_QUICK_INFORMATION_EXACT;
    }

    SetWinText(townQuickWindow, TOWN_QUICK_WINDOW_TEXT);
    armyCountLocal = 0;
    messageLocal.type = MESSAGE_WIDGET;
    messageLocal.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    messageLocal.payload.widget.id = TOWN_QUICK_PORTRAIT_WIDGET;
    messageLocal.payload.widget.data.value =
        IDX(quickTownLocal->m_type) + TOWN_QUICK_TYPE_FRAME_BASE;
    if ((gpGame->GetTown(townId)->m_buildings & BIT(BUILDING_SLOT_CASTLE)) == 0) {
        messageLocal.payload.widget.data.value += TOWN_QUICK_VILLAGE_FRAME_OFFSET;
    }
    townQuickWindow->BroadcastMessage(messageLocal);

    if (informationLevel != TOWN_QUICK_INFORMATION_EXACT
        || BitTest(gpGame->m_knownTowns, static_cast<i8>(quickTownLocal->m_id)) == 0) {
        messageLocal.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        messageLocal.payload.widget.id = TOWN_QUICK_KNOWN_MARKER_WIDGET;
        messageLocal.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
        townQuickWindow->BroadcastMessage(messageLocal);
    }

    if (quickTownLocal->m_owner == -1) {
        messageLocal.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        messageLocal.payload.widget.id = TOWN_QUICK_PLAYER_COLOR_WIDGET;
        messageLocal.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
        townQuickWindow->BroadcastMessage(messageLocal);
        ++messageLocal.payload.widget.id;
        townQuickWindow->BroadcastMessage(messageLocal);
    } else {
        messageLocal.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        messageLocal.payload.widget.id = TOWN_QUICK_PLAYER_COLOR_WIDGET;
        messageLocal.payload.widget.data.value =
            gpGame->GetPlayerColor(quickTownLocal->m_owner) * HERO_QUICK_PLAYER_COLOR_STRIDE;
        townQuickWindow->BroadcastMessage(messageLocal);
        ++messageLocal.payload.widget.id;
        ++messageLocal.payload.widget.data.value;
        townQuickWindow->BroadcastMessage(messageLocal);
    }

    sprintf(gText, GetTownName(static_cast<i8>(quickTownLocal->m_id)));
    messageLocal.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    messageLocal.payload.widget.id = TOWN_QUICK_NAME_WIDGET;
    messageLocal.payload.widget.data.text = gText;
    townQuickWindow->BroadcastMessage(messageLocal);

    armyCountLocal = 0;
    for (armyIndex = 0; armyIndex < ARMY_QUICK_SLOT_COUNT; ++armyIndex) {
        if (quickTownLocal->m_army.m_creatureTypes[armyIndex] != CREATURE_NONE) {
            ++armyCountLocal;
        }
    }

    if (informationLevel == TOWN_QUICK_INFORMATION_UNKNOWN || armyCountLocal == 0) {
        emptyArmyLabel = static_cast<char*>(H2_ALLOC_AT(
            TOWN_QUICK_EMPTY_LABEL_CAPACITY, DATA_COMPGEN(0x004f64a4, townQuickViewSourceFile, RETAIL_FILE),
            s_townViewLineBase + TOWN_VIEW_FIRST_ALLOC_LINE_OFFSET
        ));
        if (informationLevel == TOWN_QUICK_INFORMATION_UNKNOWN) {
            sprintf(emptyArmyLabel, DATA_COMPGEN(0x004f64d0, townQuickViewUnknown, "Unknown"));
        } else {
            sprintf(emptyArmyLabel, DATA_COMPGEN(0x004f64d8, townQuickViewNone, "None"));
        }
        emptyArmyTextState = new textWidget(
            TOWN_QUICK_EMPTY_LABEL_X,
            TOWN_QUICK_EMPTY_LABEL_Y,
            TOWN_QUICK_EMPTY_LABEL_WIDTH,
            ARMY_QUICK_LABEL_HEIGHT,
            emptyArmyLabel,
            DATA_COMPGEN(0x004f64e0, townQuickViewSmalfontFnt, "smalfont.fnt"),
            FONT_DRAW_DEFAULT,
            -1,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (emptyArmyTextState == NULL) {
            MemError();
        }
        townQuickWindow->AddWidget(emptyArmyTextState, -1);
    } else {
        i32 secondRowCountState;
        i32 creatureSlotLocal;
        char* armyLabelsResult[ARMY_QUICK_SLOT_COUNT];
        i32 creatureLocal;
        iconWidget* armyIcons[ARMY_QUICK_SLOT_COUNT];
        textWidget* armyTexts[ARMY_QUICK_SLOT_COUNT];
        i32 displayIndexLocal;
        i32 widgetIndexWidget;
        i32 slotWidthSlot;
        i32 fiveArmyShiftValue;
        i32 slotStartState;
        i32 rowY;
        i32 firstRowCountState;

        rowY = TOWN_QUICK_FIRST_ROW_Y;
        switch (armyCountLocal) {
            case 1:
            case ARMY_QUICK_FIRST_ROW_COUNT:
            case ARMY_QUICK_TOP_ROW_MAX:
                rowY += ARMY_QUICK_FIRST_ROW_SHIFT;
                firstRowCountState = armyCountLocal;
                secondRowCountState = 0;
                break;
            case ARMY_QUICK_FOUR_STACK_COUNT:
                firstRowCountState = ARMY_QUICK_FIRST_ROW_COUNT;
                secondRowCountState = ARMY_QUICK_FIRST_ROW_COUNT;
                break;
            default:
                firstRowCountState = ARMY_QUICK_FIRST_ROW_COUNT;
                secondRowCountState = ARMY_QUICK_TOP_ROW_MAX;
                break;
        }

        displayIndexLocal = 0;
        widgetIndexWidget = 0;
        creatureSlotLocal = 0;
        slotWidthSlot = TOWN_QUICK_ARMY_AREA_WIDTH / firstRowCountState;
        slotStartState = (slotWidthSlot - ARMY_QUICK_ICON_SIZE)
                / ARMY_QUICK_CENTER_DIVISOR
            + ARMY_QUICK_AREA_LEFT;
        fiveArmyShiftValue = 0;
        for (armyIndex = 0; armyIndex < firstRowCountState; ++armyIndex) {
            if (armyCountLocal == ARMY_QUICK_FIVE_STACK_COUNT) {
                fiveArmyShiftValue = armyIndex == 0 ? ARMY_QUICK_FIVE_STACK_X_SHIFT
                                                    : -ARMY_QUICK_FIVE_STACK_X_SHIFT;
            }
            while (quickTownLocal->m_army.m_creatureTypes[creatureSlotLocal]
                   == CREATURE_NONE) {
                ++creatureSlotLocal;
            }
            creatureLocal = IDX(quickTownLocal->m_army.m_creatureTypes[creatureSlotLocal]);
            armyIcons[widgetIndexWidget] = new iconWidget(
                static_cast<i16>(
                    slotWidthSlot * widgetIndexWidget + slotStartState + fiveArmyShiftValue
                    - GetIconEntry(monsterIconLocal, creatureLocal)->x
                    + (ARMY_QUICK_ICON_SIZE - GetIconEntry(monsterIconLocal, creatureLocal)->w)
                          / ARMY_QUICK_CENTER_DIVISOR
                    + 1
                ),
                static_cast<i16>(
                    rowY - GetIconEntry(monsterIconLocal, creatureLocal)->y
                    - GetIconEntry(monsterIconLocal, creatureLocal)->h + ARMY_QUICK_ICON_BASELINE
                ),
                ARMY_QUICK_ICON_SIZE,
                ARMY_QUICK_ICON_SIZE,
                DATA_COMPGEN(0x004f64f0, townQuickViewSecondaryMons32Icn, "mons32.icn"),
                static_cast<i16>(creatureLocal),
                ICON_DRAW_NORMAL,
                -1,
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (armyIcons[widgetIndexWidget] == NULL) {
                MemError();
            }
            armyLabelsResult[widgetIndexWidget] = static_cast<char*>(H2_ALLOC_AT(
                TOWN_QUICK_ARMY_LABEL_CAPACITY, DATA_COMPGEN(0x004f64fc, townQuickViewSourceFile2, RETAIL_FILE),
                s_townViewLineBase + TOWN_VIEW_SECOND_ALLOC_LINE_OFFSET
            ));
            if (informationLevel == TOWN_QUICK_INFORMATION_EXACT) {
                sprintf(
                    armyLabelsResult[widgetIndexWidget],
                    DATA_COMPGEN(0x004f6528, townQuickViewD, "%d"),
                    quickTownLocal->m_army.m_creatureCounts[creatureSlotLocal]
                );
            } else if (informationLevel == TOWN_QUICK_INFORMATION_ESTIMATES) {
                strcpy(
                    armyLabelsResult[widgetIndexWidget],
                    GetArmySizeName(
                        quickTownLocal->m_army.m_creatureCounts[creatureSlotLocal],
                        ARMY_SIZE_NAME_TITLE
                    )
                );
            } else {
                strcpy(armyLabelsResult[widgetIndexWidget], DATA_COMPGEN(0x004f652c, townQuickViewEmptyString, "???"));
            }
            armyTexts[widgetIndexWidget] = new textWidget(
                static_cast<i16>(
                    slotWidthSlot * widgetIndexWidget + slotStartState + fiveArmyShiftValue
                    - ARMY_QUICK_TEXT_X_ADJUSTMENT
                ),
                static_cast<i16>(rowY + ARMY_QUICK_ICON_SIZE),
                ARMY_QUICK_TEXT_WIDTH,
                ARMY_QUICK_LABEL_HEIGHT,
                armyLabelsResult[widgetIndexWidget],
                DATA_COMPGEN(0x004f6530, townQuickViewSmalfontFnt2, "smalfont.fnt"),
                FONT_DRAW_DEFAULT,
                -1,
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (armyTexts[widgetIndexWidget] == NULL) {
                MemError();
            }
            townQuickWindow->AddWidget(armyIcons[widgetIndexWidget], -1);
            townQuickWindow->AddWidget(armyTexts[widgetIndexWidget], -1);
            ++widgetIndexWidget;
            ++creatureSlotLocal;
        }

        if (secondRowCountState != 0) {
            slotWidthSlot = TOWN_QUICK_ARMY_AREA_WIDTH / secondRowCountState;
            slotStartState = (slotWidthSlot - ARMY_QUICK_ICON_SIZE)
                    / ARMY_QUICK_CENTER_DIVISOR
                + ARMY_QUICK_AREA_LEFT;
            rowY += ARMY_QUICK_SECOND_ROW_SHIFT;
            for (armyIndex = firstRowCountState;
                 armyIndex < firstRowCountState + secondRowCountState;
                 ++armyIndex) {
                while (quickTownLocal->m_army.m_creatureTypes[creatureSlotLocal]
                       == CREATURE_NONE) {
                    ++creatureSlotLocal;
                }
                creatureLocal = IDX(quickTownLocal->m_army.m_creatureTypes[creatureSlotLocal]);
                armyIcons[widgetIndexWidget] = new iconWidget(
                    static_cast<i16>(
                        (widgetIndexWidget - firstRowCountState) * slotWidthSlot + slotStartState
                        - GetIconEntry(monsterIconLocal, creatureLocal)->x
                        + (ARMY_QUICK_ICON_SIZE - GetIconEntry(monsterIconLocal, creatureLocal)->w)
                              / ARMY_QUICK_CENTER_DIVISOR
                        + 1
                    ),
                    static_cast<i16>(
                        rowY - GetIconEntry(monsterIconLocal, creatureLocal)->y
                        - GetIconEntry(monsterIconLocal, creatureLocal)->h
                        + ARMY_QUICK_ICON_BASELINE
                    ),
                    ARMY_QUICK_ICON_SIZE,
                    ARMY_QUICK_ICON_SIZE,
                    DATA_COMPGEN(0x004f6540, townQuickViewMons32Icn2, "mons32.icn"),
                    static_cast<i16>(creatureLocal),
                    ICON_DRAW_NORMAL,
                    -1,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
                if (armyIcons[widgetIndexWidget] == NULL) {
                    MemError();
                }
                armyLabelsResult[widgetIndexWidget] = static_cast<char*>(H2_ALLOC_AT(
                    TOWN_QUICK_ARMY_LABEL_CAPACITY, DATA_COMPGEN(0x004f654c, townQuickViewSourceFile3, RETAIL_FILE),
                    s_townViewLineBase + TOWN_VIEW_THIRD_ALLOC_LINE_OFFSET
                ));
                if (informationLevel == TOWN_QUICK_INFORMATION_EXACT) {
                    sprintf(
                        armyLabelsResult[widgetIndexWidget],
                        DATA_COMPGEN(0x004f6578, townQuickViewD2, "%d"),
                        quickTownLocal->m_army.m_creatureCounts[creatureSlotLocal]
                    );
                } else if (informationLevel == TOWN_QUICK_INFORMATION_ESTIMATES) {
                    strcpy(
                        armyLabelsResult[widgetIndexWidget],
                        GetArmySizeName(
                            quickTownLocal->m_army.m_creatureCounts[creatureSlotLocal],
                            ARMY_SIZE_NAME_TITLE
                        )
                    );
                } else {
                    strcpy(armyLabelsResult[widgetIndexWidget], DATA_COMPGEN(0x004f657c, townQuickViewEmptyString2, "???"));
                }
                armyTexts[widgetIndexWidget] = new textWidget(
                    static_cast<i16>(
                        (widgetIndexWidget - firstRowCountState) * slotWidthSlot + slotStartState
                        - ARMY_QUICK_TEXT_X_ADJUSTMENT
                    ),
                    static_cast<i16>(rowY + ARMY_QUICK_ICON_SIZE),
                    ARMY_QUICK_TEXT_WIDTH,
                    ARMY_QUICK_LABEL_HEIGHT,
                    armyLabelsResult[widgetIndexWidget],
                    DATA_COMPGEN(0x004f6580, townQuickViewSmalfontFnt3, "smalfont.fnt"),
                    FONT_DRAW_DEFAULT,
                    -1,
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
                if (armyTexts[widgetIndexWidget] == NULL) {
                    MemError();
                }
                townQuickWindow->AddWidget(armyIcons[widgetIndexWidget], -1);
                townQuickWindow->AddWidget(armyTexts[widgetIndexWidget], -1);
                ++widgetIndexWidget;
                ++creatureSlotLocal;
            }
        }
    }

    gpWindowManager->AddWindow(townQuickWindow, -1, 1);
    previousOriginXValue = m_mapOriginX;
    previousOriginYSlot = m_mapOriginY;
    m_mapOriginX = quickTownLocal->m_x - VIEW_CENTER_CELL;
    m_mapOriginY = quickTownLocal->m_y - VIEW_CENTER_CELL;
    UpdateRadar(1, 0);
    QuickViewWait();
    gpWindowManager->RemoveWindow(townQuickWindow);
    delete townQuickWindow;
    m_mapOriginX = previousOriginXValue;
    m_mapOriginY = previousOriginYSlot;
    UpdateRadar(1, 0);
    CompleteDraw(0);
    UpdateScreen(0, 0);
    if (messageLocal.type == MESSAGE_LEFT_BUTTON_DOWN && quickTownLocal->m_owner == giCurPlayer) {
        SetTownContext(static_cast<i8>(quickTownLocal->m_id));
    }
    gpResourceManager->Dispose(monsterIconLocal);
}

VA(0x00463dd6, 0x11f)
void advManager::RedrawAdvScreen(i32 update, i32 freeBorder) {
    DATA(0x004f6590) static i16 s_redrawBorderFreeLineBase = REDRAW_BORDER_FREE_LINE_BASE;

    if (!bShowIt) {
        return;
    }
    gpResourceManager->GetBackdrop(DATA_COMPGEN(0x004f6594, redrawAdvScreenAdvbordIcn, "advbord.icn"), gpWindowManager->m_screen, 1);
    if (freeBorder) {
        H2_FREE_AT(
            m_adventureBorder, DATA_COMPGEN(0x004f65a0, redrawAdvScreenSourceFile, RETAIL_FILE),
            s_redrawBorderFreeLineBase + BORDER_SECONDARY_FREE_LINE_OFFSET
        );
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

VA(0x00463ef5, 0x1f)
void advManager::DeactivateCurrTown(void) {
    gpCurPlayer->m_currentTown = -1;
}

VA(0x00463f14, 0x27)
void advManager::DeactivateCurrHero(void) {
    DemobilizeCurrHero();
    gpCurPlayer->m_currentHero = -1;
}

VA(0x00463f3b, 0x5a)
void advManager::MobilizeCurrHero(i32 update) {
    if (gpCurPlayer->m_currentHero == INVALID_HERO) {
        return;
    }
    if (m_heroContextLocked) {
        return;
    }
    SetHeroContext(gpCurPlayer->m_currentHero, update);
}

VA(0x00463f95, 0x16c)
void advManager::DemobilizeCurrHero(void) {
    if (gpCurPlayer->m_currentHero == INVALID_HERO) {
        return;
    }
    if (!m_heroContextLocked) {
        return;
    }

    m_heroContextLocked = 0;
    hero* currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    StopCursor(1);
    mapCell* currentCell = GetCell(currentHero->m_x, currentHero->m_y);
    currentHero->m_locationType = currentCell->m_triggerType;
    currentHero->m_occupiedTown = currentCell->m_objectMetadata;
    currentHero->m_direction = m_cursorDirection;
    if (m_cursorType == HERO_TYPE_BOAT) {
        currentHero->m_eventFlags = HERO_EVENT_EMBARKED | currentHero->m_eventFlags;
    }
    currentCell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION;
    currentCell->m_objectMetadata = currentHero->m_id;
    currentCell->m_flags &= ~CURSOR_MAP_VISIBLE_FLAG;
    m_cursorActive = 0;
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    UpdateScreen(0, 0);
}

VA(0x00464101, 0x217)
void advManager::SetTownContext(i32 townId) {
    DeactivateCurrHero();
    gpCurPlayer->m_currentTown = static_cast<i8>(townId);
    town* currentTownValue = gpGame->GetTown(gpCurPlayer->m_currentTown);
    m_mapOriginX = currentTownValue->m_x - VIEW_CENTER_OFFSET;
    m_mapOriginY = currentTownValue->m_y - VIEW_CENTER_OFFSET;

    i32 contextValue7 = 0;
    i32 index;
    for (index = 0; index < gpCurPlayer->m_townCount; ++index) {
        if (gpCurPlayer->m_townIds[index] == townId) {
            contextValue7 = index;
        }
    }
    if (contextValue7 < gpCurPlayer->m_townLocatorPage) {
        gpCurPlayer->m_townLocatorPage = static_cast<i8>(contextValue7);
    } else if (gpCurPlayer->m_townLocatorPage + LOCATOR_VISIBLE_COUNT - 1 < contextValue7) {
        gpCurPlayer->m_townLocatorPage =
            static_cast<i8>(contextValue7 - (LOCATOR_VISIBLE_COUNT - 1));
    }

    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    HideRoute(0, 0, 1);
    UpdBottomView(1, 1, 1);
    UpdateRadar(1, 0);
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    UpdateScreen(0, 0);
    SetEnvironmentOrigin(
        m_mapOriginX + VIEW_CENTER_OFFSET,
        m_mapOriginY + VIEW_CENTER_OFFSET,
        1
    );

    contextValue7 = IDX(
        giGroundToTerrain[GetCell(currentTownValue->m_x, currentTownValue->m_y)
                              ->m_terrainImageIndex]
    );
    if (m_currentTerrain != static_cast<TerrainType>(contextValue7)) {
        m_currentTerrain = contextValue7;
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
    }
    gpInputManager->ForceMouseMove();
    m_lastHoverCell = 0;
}

VA(0x00464318, 0x392)
void advManager::SetHeroContext(i32 heroId, i32 update) {
    if (heroId == INVALID_HERO) {
        return;
    }

    DeactivateCurrTown();
    HideRoute(0, 0, 1);
    DeactivateCurrHero();
    m_heroContextLocked = 1;
    gpCurPlayer->m_currentHero = static_cast<i8>(heroId);
    hero* currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    m_mapOriginX = currentHero->m_x - VIEW_CENTER_OFFSET;
    m_mapOriginY = currentHero->m_y - VIEW_CENTER_OFFSET;
    m_cursorMapY = VIEW_CENTER_CELL;
    m_cursorMapX = m_cursorMapY;
    m_previousCursorMapY = CURSOR_INVALID_POSITION;
    m_previousCursorMapX = m_previousCursorMapY;
    if (HAS(currentHero->m_eventFlags, HERO_EVENT_EMBARKED)) {
        m_cursorType = HERO_TYPE_BOAT;
    } else {
        m_cursorType = currentHero->m_cursorType;
    }
    m_cursorDirection = currentHero->m_direction;
    m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);

    mapCell* currentCell = GetCell(currentHero->m_x, currentHero->m_y);
    currentCell->m_flags |= CURSOR_MAP_VISIBLE_FLAG;
    gpGame->RestoreCell(
        currentHero->m_x,
        currentHero->m_y,
        currentHero->m_locationType,
        currentHero->m_occupiedTown,
        NULL,
        // Retail's final RestoreCell argument is unused.
        // NOLINTNEXTLINE(readability-magic-numbers)
        4
    );

    // Retail reuses this locator index slot for the terrain value below.
    i32 contextValue7 = 0;
    i32 index;
    for (index = 0; index < gpCurPlayer->m_heroCount; ++index) {
        if (gpCurPlayer->m_heroIds[index] == heroId) {
            contextValue7 = index;
        }
    }
    if (contextValue7 < gpCurPlayer->m_heroLocatorPage) {
        gpCurPlayer->m_heroLocatorPage = static_cast<i8>(contextValue7);
    } else if (gpCurPlayer->m_heroLocatorPage + LOCATOR_VISIBLE_COUNT - 1 < contextValue7) {
        gpCurPlayer->m_heroLocatorPage =
            static_cast<i8>(contextValue7 - (LOCATOR_VISIBLE_COUNT - 1));
    }

    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    if (!update && (m_active == 1 || gbThisNetHumanPlayer[giCurPlayer])) {
        Reseed(0, 0);
        SeedTo(currentHero->m_destinationX, currentHero->m_destinationY);
        ShowRoute(0, 0, !update);
    }
    UpdBottomView(1, 1, 1);
    m_cursorActive = 1;
    UpdateRadar(1, 0);
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    UpdateScreen(0, 0);
    SetEnvironmentOrigin(
        m_mapOriginX + VIEW_CENTER_OFFSET,
        m_mapOriginY + VIEW_CENTER_OFFSET,
        1
    );

    contextValue7 =
        static_cast<i32>(giGroundToTerrain[currentCell->m_terrainImageIndex]);
    if (m_currentTerrain != static_cast<TerrainType>(contextValue7)) {
        m_currentTerrain = contextValue7;
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
    }
    if (!gbHeroMoving) {
        gpInputManager->ForceMouseMove();
        m_lastHoverCell = 0;
    }
}

VA(0x004646aa, 0x22f)
void advManager::DoHeroKnob(void) {
    i32 previousPageSlot = gpCurPlayer->m_heroLocatorPage;
    i32 locatorCount29 = gpCurPlayer->m_heroCount;
    i32 newPageState;
    double pageHeight7 = static_cast<double>(LOCATOR_HERO_SCROLL_SPAN)
                         / (locatorCount29 - LOCATOR_VISIBLE_COUNT);
    i32 mouseX4;
    i32 mouseYState;
    gpMouseManager->MouseCoords(mouseX4, mouseYState);
    i32 dragOffset5 = mouseYState - m_scrollLeftButton->m_y;
    gpInputManager->Flush();
    tag_message message = gpInputManager->GetEvent();

    while (message.type != MESSAGE_LEFT_BUTTON_UP && message.type != MESSAGE_RIGHT_BUTTON_UP) {
        if (message.type == MESSAGE_MOUSE_MOVE) {
            if (message.payload.mouse.y < dragOffset5 + LOCATOR_SCROLL_BASE_Y) {
                message.payload.mouse.y = dragOffset5 + LOCATOR_SCROLL_BASE_Y;
            }
            if (message.payload.mouse.y > dragOffset5 + LOCATOR_KNOB_MAX_Y) {
                message.payload.mouse.y = dragOffset5 + LOCATOR_KNOB_MAX_Y;
            }
            gpMouseManager->Main(message);
            m_scrollLeftButton->m_y = message.payload.mouse.y - dragOffset5;
            m_adventureWindow->DrawWindow();
            if (locatorCount29 > LOCATOR_VISIBLE_COUNT) {
                newPageState = static_cast<i32>(
                    (m_scrollLeftButton->m_y - LOCATOR_SCROLL_BASE_Y) / pageHeight7
                );
                if (newPageState != previousPageSlot) {
                    gpCurPlayer->m_heroLocatorPage = static_cast<i8>(newPageState);
                    if (newPageState > locatorCount29 - (LOCATOR_VISIBLE_COUNT - 1)) {
                        newPageState = locatorCount29 - (LOCATOR_VISIBLE_COUNT - 1);
                    }
                    UpdateHeroLocators(0, 1);
                    m_scrollLeftButton->m_y = message.payload.mouse.y - dragOffset5;
                    m_adventureWindow->DrawWindow();
                    previousPageSlot = newPageState;
                }
            }
        }
        Process1WindowsMessage();
        message = gpInputManager->GetEvent();
    }
    m_scrollLeftButton->m_flags &= ~WIDGET_FLAG_SELECTED;
    UpdateHeroLocators(1, 1);
}

VA(0x004648d9, 0x22f)
void advManager::DoTownKnob(void) {
    i32 previousPageSlot = gpCurPlayer->m_townLocatorPage;
    i32 locatorCount29 = gpCurPlayer->m_townCount;
    i32 newPageState;
    double pageHeight7 = static_cast<double>(LOCATOR_HERO_SCROLL_SPAN)
                         / (locatorCount29 - LOCATOR_VISIBLE_COUNT);
    i32 mouseX4;
    i32 mouseYState;
    gpMouseManager->MouseCoords(mouseX4, mouseYState);
    i32 dragOffset5 = mouseYState - m_scrollRightButton->m_y;
    gpInputManager->Flush();
    tag_message message = gpInputManager->GetEvent();

    while (message.type != MESSAGE_LEFT_BUTTON_UP && message.type != MESSAGE_RIGHT_BUTTON_UP) {
        if (message.type == MESSAGE_MOUSE_MOVE) {
            if (message.payload.mouse.y < dragOffset5 + LOCATOR_SCROLL_BASE_Y) {
                message.payload.mouse.y = dragOffset5 + LOCATOR_SCROLL_BASE_Y;
            }
            if (message.payload.mouse.y > dragOffset5 + LOCATOR_KNOB_MAX_Y) {
                message.payload.mouse.y = dragOffset5 + LOCATOR_KNOB_MAX_Y;
            }
            gpMouseManager->Main(message);
            m_scrollRightButton->m_y = message.payload.mouse.y - dragOffset5;
            m_adventureWindow->DrawWindow();
            if (locatorCount29 > LOCATOR_VISIBLE_COUNT) {
                newPageState = static_cast<i32>(
                    (m_scrollRightButton->m_y - LOCATOR_SCROLL_BASE_Y) / pageHeight7
                );
                if (newPageState != previousPageSlot) {
                    gpCurPlayer->m_townLocatorPage = static_cast<i8>(newPageState);
                    if (newPageState > locatorCount29 - (LOCATOR_VISIBLE_COUNT - 1)) {
                        newPageState = locatorCount29 - (LOCATOR_VISIBLE_COUNT - 1);
                    }
                    UpdateTownLocators(0, 1);
                    m_scrollRightButton->m_y = message.payload.mouse.y - dragOffset5;
                    m_adventureWindow->DrawWindow();
                    previousPageSlot = newPageState;
                }
            }
        }
        Process1WindowsMessage();
        message = gpInputManager->GetEvent();
    }
    m_scrollRightButton->m_flags &= ~WIDGET_FLAG_SELECTED;
    UpdateTownLocators(1, 1);
}

VA(0x00464b08, 0x397)
void advManager::CastSpell(SpellType spell) {
    hero* currentHeroSlot;
    if (gpCurPlayer->CurrentHero() != INVALID_HERO) {
        currentHeroSlot = gpGame->GetHero(gpCurPlayer->m_currentHero);
    } else {
        currentHeroSlot = NULL;
    }

    CreatureType guardianTypes1[MINE_GUARDIAN_VALUE_COUNT];
    mapCell* currentCell;
    i32 spellPowerValue;
    switch (spell) {
        case SPELL_SET_EARTH_GUARDIAN:
            guardianTypes1[MINE_GUARDIAN_TYPE_INDEX] = CREATURE_EARTH_ELEMENTAL;
            goto setMineGuardian;
        case SPELL_SET_AIR_GUARDIAN:
            guardianTypes1[MINE_GUARDIAN_TYPE_INDEX] = CREATURE_AIR_ELEMENTAL;
            goto setMineGuardian;
        case SPELL_SET_FIRE_GUARDIAN:
            guardianTypes1[MINE_GUARDIAN_TYPE_INDEX] = CREATURE_FIRE_ELEMENTAL;
            goto setMineGuardian;
        case SPELL_SET_WATER_GUARDIAN:
            guardianTypes1[MINE_GUARDIAN_TYPE_INDEX] = CREATURE_WATER_ELEMENTAL;
            goto setMineGuardian;
        case SPELL_HAUNT:
            guardianTypes1[MINE_GUARDIAN_TYPE_INDEX] = CREATURE_GHOST;
            goto setMineGuardian;
        setMineGuardian:
            currentCell = gpAdvManager->GetCell(currentHeroSlot->m_x, currentHeroSlot->m_y);
            if (currentCell->m_triggerType != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE)) {
                NormalDialog(
                    DATA_COMPGEN(0x004f65cc, castSpellYouMustBeStandingOnThe, "You must be standing on the entrance to a mine (sawmills and alchemists don't "
                    "count) to cast this spell."),
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
            gpGame->m_mines[currentCell->m_objectMetadata].guardianType =
                guardianTypes1[MINE_GUARDIAN_TYPE_INDEX];
            spellPowerValue = currentHeroSlot->Stats(HERO_PRIMARY_SPELL_POWER);
            if (spellPowerValue > MINE_GUARDIAN_MAX_POWER) {
                spellPowerValue = MINE_GUARDIAN_MAX_POWER;
            }
            gpGame->m_mines[currentCell->m_objectMetadata].guardianCount =
                static_cast<u8>(spellPowerValue * MINE_GUARDIANS_PER_POWER);
            if (spell == SPELL_HAUNT) {
                gpGame->ClaimMine(currentCell->m_objectMetadata, -1);
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
                DATA_COMPGEN(0x004f6638, castSpellEnemyHeroesAreNowFullyIdentifiable, "Enemy heroes are now fully identifiable."),
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
            if (currentHeroSlot->m_remainingMobility == 0) {
                NormalDialog(
                    DATA_COMPGEN(0x004f6664, castSpellYourHeroIsTooTiredTo, "Your hero is too tired to cast this spell today.  Try again tomorrow."),
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
            if (currentHeroSlot->m_remainingMobility < TRAVEL_SPELL_MOBILITY_COST) {
                currentHeroSlot->m_remainingMobility = 0;
            } else {
                currentHeroSlot->m_remainingMobility -= TRAVEL_SPELL_MOBILITY_COST;
            }
            UpdateHeroLocator(-1, 1, 1);
            if (spell == SPELL_DIMENSION_DOOR) {
                DimensionDoor();
            } else {
                TownGate(spell);
            }
            break;
        case SPELL_VISIONS:
            if (!DoVisions(currentHeroSlot)) {
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

VA(0x00464e9f, 0x24c)
i32 SaveGame(void) {
    i32 result11 = 0;
    i32 humanPlayerCount1 = 0;
    gpAdvManager->DisableButtons();
    gpMouseManager->SetPointer(DATA_COMPGEN(0x004f66ac, saveGameAdvmiceMse, "advmice.mse"), SAVE_POINTER_FRAME, MOUSE_AUTO_CURSOR_TYPE);
    i32 playerLocal;
    for (playerLocal = 0; playerLocal < SAVE_PLAYER_COUNT; ++playerLocal) {
        if (!gpGame->m_playerDead[playerLocal] && gbHumanPlayer[playerLocal]) {
            ++humanPlayerCount1;
        }
    }

    char extension7[SAVE_EXTENSION_SIZE];
    char patternState[SAVE_PATTERN_SIZE];
    if (gbInCampaign) {
        sprintf(extension7, DATA_COMPGEN(0x004f66b8, saveGameGMC, ".GMC"));
        sprintf(patternState, DATA_COMPGEN(0x004f66c0, saveGameGMC2, "*.GMC"));
    } else if (xIsPlayingExpansionCampaign) {
        sprintf(extension7, DATA_COMPGEN(0x004f66c8, saveGameGXC, ".GXC"));
        sprintf(patternState, DATA_COMPGEN(0x004f66d0, saveGameGXC2, "*.GXC"));
    } else if (xIsExpansionMap) {
        sprintf(extension7, DATA_COMPGEN(0x004f66d8, saveGameGXD, ".GX%d"), humanPlayerCount1);
        sprintf(patternState, DATA_COMPGEN(0x004f66e0, saveGameGXD2, "*.GX%d"), humanPlayerCount1);
    } else {
        sprintf(extension7, DATA_COMPGEN(0x004f66e8, saveGameGMD, ".GM%d"), humanPlayerCount1);
        sprintf(patternState, DATA_COMPGEN(0x004f66f0, saveGameGMD2, "*.GM%d"), humanPlayerCount1);
    }

    fileRequester* requester2 = new fileRequester(
        SAVE_REQUESTER_X,
        SAVE_REQUESTER_Y,
        FILE_REQUESTER_SAVE_GAME,
        patternState,
        gcGamePath,
        extension7
    );
    if (requester2 == NULL) {
        MemError();
    }
    i32 dialogResult7 = gpExec->DoDialog(requester2);
    if (dialogResult7 == FILE_REQUESTER_OK) {
        result11 = 1;
        bFreshSave = 1;
        result11 = gpGame->SaveGame(gLastFilename, 0, 0);
        if (result11) {
            NormalDialog(DATA_COMPGEN(0x004f66f8, saveGameGameSavedSuccessfully, "Game saved successfully."), 1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
    }
    delete requester2;
    gpAdvManager->EnableButtons();
    return result11;
}

VA(0x004650eb, 0xa6)
void advManager::CheckCastSpell(void) {
    if (gpCurPlayer->CurrentHero() != INVALID_HERO) {
        MobilizeCurrHero(0);
        CompleteDraw(0);
        UpdateScreen(0, 0);
        gpMouseManager->SetPointer(DATA_COMPGEN(0x004f6714, checkCastSpellAdvmiceMse, "advmice.mse"), POINTER_DEFAULT, MOUSE_AUTO_CURSOR_TYPE);
        CastSpell(gpGame->ViewSpells(
            gpGame->GetHero(gpCurPlayer->m_currentHero), SPELL_TYPE_ADVENTURE, NullHandler, 0
        ));
    }
}

VA(0x00465191, 0x31c)
MessageDispatchResult DimensionDoorHandler(tag_message& message) {
    if (glTimers[0] < KBTickCount()) {
        gpAdvManager->CompleteDraw(gpAdvManager->m_mapOriginX, gpAdvManager->m_mapOriginY, 0, 1);
        gpAdvManager->UpdateScreen(0, 0);
    }

    i32 mouseX = message.payload.mouse.x;
    i32 mouseY = message.payload.mouse.y;
    i32 handled = 0;

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_SELECT:
                    switch (message.payload.widget.id) {
                        case DIMENSION_DOOR_FIRST_BUTTON:
                        case DIMENSION_DOOR_LAST_BUTTON:
                            if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_LEFT_SHIFT)) {
                            } else {
                                if (gpWindowManager->m_dialogResult
                                    == TRAVEL_DIALOG_ACCEPT) {
                                    handled = 1;
                                }
                            }
                            break;
                    }
                    break;
                case WIDGET_COMMAND_DESELECT:
                    switch (message.payload.widget.id) {
                        case DIMENSION_DOOR_CLOSE_BUTTON:
                            gpWindowManager->m_dialogResult = 0;
                            handled = 1;
                            break;
                    }
                    break;
            }
            break;

        case MESSAGE_MOUSE_MOVE:
            if (InMapArea(message.payload.mouse.x, message.payload.mouse.y)) {
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
                    if (HAS(cell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)
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

    if (handled) {
        message.payload.widget.id = DIMENSION_DOOR_FIRST_BUTTON;
        message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004654ad, 0x11a9)
i32 advManager::ComboDraw(i32 originX, i32 originY, i32 animate) {
    i32 updateCount;
    i32 mapCellX;
    i32 column;
    i32 mapRow;
    mapCell* cell;
    i32 mapYValue;

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
            Process1WindowsMessage();
            if (glTimers[0] < KBTickCount()) {
                glTimers[0] = KBTickCount() + TIMER_DELAY;
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

    for (mapRow = 0; mapRow < COMBO_VIEW_CELLS; ++mapRow) {
        for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
            if (column + originX >= 0 && column + originX < MAP_WIDTH && mapRow + originY >= 0
                && mapRow + originY < MAP_HEIGHT) {
                cell = GetCell(column + originX, mapRow + originY);

                if (cell->m_animatedObject || cell->m_animatedOverlay) {
                    ++bComboDraw[column][mapRow];
                }
                if ((cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_WINDMILL) {
                    ++bComboDraw[column][mapRow];
                }
                if ((cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_ALCHEMIST_LAB) {
                    ++bComboDraw[column][mapRow];
                }

                if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER)) {
                    ++bComboDraw[column][mapRow];
                    ++bComboDraw[column - 1][mapRow];
                    if (GetCloudLookup(column + originX, mapRow + originY) != 0) {
                        bComboDraw[column + 1][mapRow] += COMBO_CLOUD_MARK;
                        if (mapRow >= 1) {
                            bComboDraw[column][mapRow - 1] += COMBO_CLOUD_MARK;
                            bComboDraw[column + 1][mapRow - 1] += COMBO_CLOUD_MARK;
                        }
                    } else {
                        ++bComboDraw[column + 1][mapRow];
                        if (mapRow >= 1) {
                            ++bComboDraw[column][mapRow - 1];
                            ++bComboDraw[column + 1][mapRow - 1];
                        }
                    }
                }

                if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                    || cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT)) {
                    ++bComboDraw[column][mapRow];
                    if (GetCloudLookup(column + originX, mapRow + originY) != 0) {
                        bComboDraw[column + 1][mapRow] += COMBO_CLOUD_MARK;
                        bComboDraw[column][mapRow + 1] += COMBO_CLOUD_MARK;
                        bComboDraw[column + 1][mapRow + 1] += COMBO_CLOUD_MARK;
                        bComboDraw[column + COMBO_FAR_NEIGHBOR_OFFSET][mapRow] += COMBO_CLOUD_MARK;
                        if (mapRow >= 1) {
                            bComboDraw[column][mapRow - 1] += COMBO_CLOUD_MARK;
                        }
                        if (column >= 1) {
                            bComboDraw[column - 1][mapRow] += COMBO_CLOUD_MARK;
                            *(bComboDraw[column - 1] + mapRow + 1) += COMBO_CLOUD_MARK;
                            if (column >= COMBO_FAR_NEIGHBOR_OFFSET) {
                                bComboDraw[column - COMBO_FAR_NEIGHBOR_OFFSET][mapRow] +=
                                    COMBO_CLOUD_MARK;
                            }
                            if (mapRow >= 1) {
                                ++bComboDraw[column - COMBO_FAR_NEIGHBOR_OFFSET][mapRow - 1];
                            }
                        }
                    } else {
                        ++bComboDraw[column + 1][mapRow];
                        ++bComboDraw[column][mapRow + 1];
                        ++bComboDraw[column + 1][mapRow + 1];
                        ++bComboDraw[column + COMBO_FAR_NEIGHBOR_OFFSET][mapRow];
                        if (mapRow >= 1) {
                            ++bComboDraw[column][mapRow - 1];
                        }
                        if (column >= 1) {
                            ++bComboDraw[column - 1][mapRow];
                            ++bComboDraw[column - 1][mapRow + 1];
                            if (column >= COMBO_FAR_NEIGHBOR_OFFSET) {
                                ++bComboDraw[column - COMBO_FAR_NEIGHBOR_OFFSET][mapRow];
                            }
                            if (mapRow >= 1) {
                                ++bComboDraw[column - COMBO_FAR_NEIGHBOR_OFFSET][mapRow - 1];
                            }
                        }
                    }
                }
            }
        }
    }

    for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
        for (mapRow = 0; mapRow < COMBO_VIEW_CELLS; ++mapRow) {
            if (bComboDraw[0][mapRow + column * COMBO_GRID_CELLS] != 0) {
                if (column + originX < 0 || column + originX >= MAP_WIDTH || mapRow + originY < 0
                    || mapRow + originY >= MAP_HEIGHT) {
                    *(bComboDraw[column] + mapRow) = 0;
                } else if (*(bComboDraw[column] + mapRow) < COMBO_CLOUD_MARK
                           && GetCloudLookup(column + originX, mapRow + originY) == 0) {
                    *(bComboDraw[column] + mapRow) = 0;
                }
            }
        }
    }

    if (m_heroContextLocked != 0) {
        for (mapRow = COMBO_HERO_PANEL_TOP; mapRow <= COMBO_HERO_PANEL_BOTTOM - 1;
             ++mapRow) {
            for (column = COMBO_HERO_PANEL_LEFT; column <= COMBO_HERO_PANEL_RIGHT - 1;
                 ++column) {
                ++bComboDraw[column][mapRow];
            }
        }
    }

    if (m_cursorType == HERO_TYPE_BOAT) {
        ++bComboDraw[VIEW_CENTER_CELL - 1][COMBO_HERO_PANEL_LEFT];
        ++bComboDraw[VIEW_CENTER_CELL][COMBO_HERO_PANEL_LEFT];
        ++bComboDraw[VIEW_CENTER_CELL + 1][COMBO_HERO_PANEL_LEFT];
    }

    for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
        for (mapRow = 0; mapRow < COMBO_VIEW_CELLS; ++mapRow) {
            cell = GetCell(column + originX, mapRow + originY);
            if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE)) {
                if (gpGame->m_mines[cell->m_objectMetadata].guardianType == CREATURE_GHOST) {
                    ++bComboDraw[column][mapRow];
                    ++bComboDraw[column + 1][mapRow];
                    if (column < COMBO_VIEW_CELLS) {
                        ++bComboDraw[column + COMBO_FAR_NEIGHBOR_OFFSET][mapRow];
                    }
                    if (column > 0) {
                        ++bComboDraw[column - 1][mapRow];
                    }
                    if (mapRow > 0) {
                        ++bComboDraw[column][mapRow - 1];
                        ++bComboDraw[column + 1][mapRow - 1];
                        if (column < COMBO_VIEW_CELLS) {
                            ++bComboDraw[column + COMBO_FAR_NEIGHBOR_OFFSET][mapRow - 1];
                        }
                        if (column > 0) {
                            ++bComboDraw[column - 1][mapRow - 1];
                        }
                    }
                    if (mapRow > 1) {
                        ++bComboDraw[column][mapRow - COMBO_FAR_NEIGHBOR_OFFSET];
                        ++bComboDraw[column + 1][mapRow - COMBO_FAR_NEIGHBOR_OFFSET];
                        if (column < COMBO_VIEW_CELLS) {
                            ++bComboDraw[column + COMBO_FAR_NEIGHBOR_OFFSET]
                                        [mapRow - COMBO_FAR_NEIGHBOR_OFFSET];
                        }
                        if (column > 0) {
                            ++bComboDraw[column - 1][mapRow - COMBO_FAR_NEIGHBOR_OFFSET];
                        }
                    }
                } else if (mapRow > 0 && bComboDraw[column][mapRow - 1] != 0) {
                    ++bComboDraw[column][mapRow];
                }
            }
        }
    }

    if (m_visibilityMapValid != 0 && m_visibilityMap != NULL) {
        for (mapRow = 1; mapRow < COMBO_VIEW_CELLS - 1; ++mapRow) {
            for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
                if (bComboDraw[column][mapRow] == 0) {
                    continue;
                }
                mapCellX = column + originX;
                mapYValue = mapRow + originY;
                if (mapCellX < 0 || mapCellX > MAP_WIDTH - 1 || mapYValue < 1
                    || mapYValue > MAP_HEIGHT - COMBO_FAR_NEIGHBOR_OFFSET) {
                    continue;
                }
                if (m_visibilityMap[mapYValue * MAP_WIDTH + mapCellX] != 0) {
                    ++bComboDraw[column][mapRow + 1];
                }
                if (m_visibilityMap[(mapYValue - 1) * MAP_WIDTH + mapCellX] != 0) {
                    ++bComboDraw[column][mapRow - 1];
                }
            }
        }
    }

    gpMouseManager->m_cursorReady = 0;
    for (mapRow = 0; mapRow < COMBO_VIEW_CELLS; ++mapRow) {
        for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
            if (bComboDraw[column][mapRow] != 0) {
                DrawCell(column + originX, mapRow + originY, column, mapRow, ADVMGR_DRAW_GROUND, 0);
            }
        }
    }

    for (mapRow = 0; mapRow < COMBO_VIEW_CELLS; ++mapRow) {
        for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
            if (bComboDraw[column][mapRow] != 0) {
                DrawCell(
                    column + originX,
                    mapRow + originY,
                    column,
                    mapRow,
                    ADVMGR_DRAW_HERO_SHADOW,
                    0
                );
            }
        }
    }

    for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
        if (bComboDraw[column][0] != 0) {
            DrawCell(column + originX, originY, column, 0, ADVMGR_DRAW_OBJECT, 0);
        }
    }

    for (mapRow = 1; mapRow < COMBO_VIEW_CELLS; ++mapRow) {
        PollSound();
        for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
            if (bComboDraw[column][mapRow - 1] != 0) {
                DrawCell(
                    column + originX,
                    mapRow + originY - 1,
                    column,
                    mapRow - 1,
                    ADVMGR_DRAW_HERO,
                    0
                );
            }
        }
        for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
            if (bComboDraw[column][mapRow - 1] != 0) {
                DrawCell(
                    column + originX,
                    mapRow + originY - 1,
                    column,
                    mapRow - 1,
                    ADVMGR_DRAW_OVERLAY,
                    0
                );
            }
        }
        for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
            if (column + originX == giDeferObjDrawX && mapRow + originY == giDeferObjDrawY) {
                continue;
            }
            if (column + originX == giDeferObjDrawX && mapRow + originY == giDeferObjDrawY + 1) {
                DrawCell(
                    column + originX,
                    mapRow + originY - 1,
                    column,
                    mapRow - 1,
                    ADVMGR_DRAW_OBJECT,
                    0
                );
            }
            if (bComboDraw[column][mapRow] != 0) {
                DrawCell(column + originX, mapRow + originY, column, mapRow, ADVMGR_DRAW_OBJECT, 0);
            }
        }
    }

    for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
        if (bComboDraw[column][COMBO_VIEW_CELLS - 1] != 0) {
            DrawCell(
                column + originX,
                originY + COMBO_VIEW_CELLS - 1,
                column,
                COMBO_VIEW_CELLS - 1,
                ADVMGR_DRAW_HERO,
                0
            );
        }
    }
    for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
        if (bComboDraw[column][COMBO_VIEW_CELLS - 1] != 0) {
            DrawCell(
                column + originX,
                originY + COMBO_VIEW_CELLS - 1,
                column,
                COMBO_VIEW_CELLS - 1,
                ADVMGR_DRAW_OVERLAY,
                0
            );
        }
    }

    for (mapRow = 0; mapRow < COMBO_VIEW_CELLS; ++mapRow) {
        for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
            if (bComboDraw[column][mapRow] != 0) {
                DrawCell(
                    column + originX,
                    mapRow + originY,
                    column,
                    mapRow,
                    ADVMGR_DRAW_OVERLAY_TOP,
                    0
                );
            }
        }
    }
    for (mapRow = 0; mapRow < COMBO_VIEW_CELLS; ++mapRow) {
        for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
            if (bComboDraw[column][mapRow] != 0) {
                DrawCell(column + originX, mapRow + originY, column, mapRow, ADVMGR_DRAW_CLOUD, 0);
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
    for (mapRow = 0; mapRow < COMBO_VIEW_CELLS; ++mapRow) {
        for (column = 0; column < COMBO_VIEW_CELLS; ++column) {
            if (bComboDraw[column][mapRow] != 0) {
                ++updateCount;
                if (column < giLimitUpdMinX) {
                    giLimitUpdMinX = column;
                }
                if (giLimitUpdMaxX < column) {
                    giLimitUpdMaxX = column;
                }
                if (giLimitUpdMinY > mapRow) {
                    giLimitUpdMinY = mapRow;
                }
                if (giLimitUpdMaxY < mapRow) {
                    giLimitUpdMaxY = mapRow;
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

    if (giLimitUpdMaxX < giLimitUpdMinX || giLimitUpdMaxY < giLimitUpdMinY) {
        giLimitUpdMinX = giLimitUpdMaxX - 1;
        giLimitUpdMinY = giLimitUpdMaxY - 1;
        return 0;
    }
    return 1;
}

VA(0x00466656, 0x38)
i32 advManager::ComboDraw(i32 update) {
    return ComboDraw(m_mapOriginX, m_mapOriginY, update);
}

VA(0x0046668e, 0x338)
void advManager::SetEnvironmentOrigin(i32 originX, i32 originY, i32 stopSounds) {
    i32 soundLayer;
    i32 maxSounds = SOUND_CELL_COUNT;
    i32 soundRadius;
    i32 edgeOffset;

    if (gpSoundManager->m_samplesReady == 0) {
        return;
    }

    for (edgeOffset = 0; edgeOffset < SOUND_CELL_COUNT; ++edgeOffset) {
        if (m_activeSounds[edgeOffset].soundId != ADVMGR_ENVIRONMENT_SOUND_NONE) {
            LogInt(
                DATA_COMPGEN(0x004f6724, setEnvironmentOriginSEOA, "SEO a"),
                edgeOffset,
                ENVIRONMENT_SOUND_LOG_UNUSED,
                ENVIRONMENT_SOUND_LOG_UNUSED,
                ENVIRONMENT_SOUND_LOG_UNUSED,
                ENVIRONMENT_SOUND_LOG_UNUSED,
                ENVIRONMENT_SOUND_LOG_UNUSED,
                ENVIRONMENT_SOUND_LOG_UNUSED
            );
            if (stopSounds != 0) {
                LogInt(
                    DATA_COMPGEN(0x004f672c, setEnvironmentOriginSEOB, "SEO b"),
                    edgeOffset,
                    ENVIRONMENT_SOUND_LOG_UNUSED,
                    ENVIRONMENT_SOUND_LOG_UNUSED,
                    ENVIRONMENT_SOUND_LOG_UNUSED,
                    ENVIRONMENT_SOUND_LOG_UNUSED,
                    ENVIRONMENT_SOUND_LOG_UNUSED,
                    ENVIRONMENT_SOUND_LOG_UNUSED
                );
                gpSoundManager->StopSample(
                    m_loopingSamples[IDX(m_activeSounds[edgeOffset].soundId)]
                        ->m_playbackData.activeSample
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
        for (soundLayer = ENVIRONMENT_SOUND_FIRST_LAYER;
             soundLayer <= ENVIRONMENT_SOUND_LAYER_COUNT;
             ++soundLayer) {
            InsertSound(originX, originY, 0, soundLayer);
            for (soundRadius = 0; soundRadius < SOUND_CELL_COUNT; ++soundRadius) {
                for (edgeOffset = 0;
                     edgeOffset < soundRadius * ENVIRONMENT_SOUND_EDGE_SPAN;
                     ++edgeOffset) {
                    InsertSound(
                        originX - soundRadius + edgeOffset,
                        originY - soundRadius,
                        soundRadius,
                        soundLayer
                    );
                    InsertSound(
                        originX + soundRadius,
                        originY - soundRadius + edgeOffset,
                        soundRadius,
                        soundLayer
                    );
                    InsertSound(
                        originX + soundRadius - edgeOffset,
                        originY + soundRadius,
                        soundRadius,
                        soundLayer
                    );
                    InsertSound(
                        originX - soundRadius,
                        originY + soundRadius - edgeOffset,
                        soundRadius,
                        soundLayer
                    );
                }
            }
        }

        for (edgeOffset = 0; edgeOffset < SOUND_CELL_COUNT; ++edgeOffset) {
            if (m_activeSounds[edgeOffset].soundId != ADVMGR_ENVIRONMENT_SOUND_NONE
                && m_activeSounds[edgeOffset].volume > ENVIRONMENT_SOUND_MAX_DISTANCE) {
                gpSoundManager->StopSample(
                    m_loopingSamples[IDX(m_activeSounds[edgeOffset].soundId)]
                        ->m_playbackData.activeSample
                );
                m_activeSounds[edgeOffset].soundId = ADVMGR_ENVIRONMENT_SOUND_NONE;
            }
            if (m_activeSounds[edgeOffset].soundId != ADVMGR_ENVIRONMENT_SOUND_NONE
                && (m_activeSoundMask & BIT(m_activeSounds[edgeOffset].soundId)) != 0) {
                gpSoundManager->ModifySample(
                    m_loopingSamples[IDX(m_activeSounds[edgeOffset].soundId)]
                        ->m_playbackData.activeSample,
                    SOUND_SAMPLE_OPERATION_EFFECT_VOLUME,
                    ADVMGR_ENVIRONMENT_VOLUME(m_activeSounds[edgeOffset].volume)
                );
            }
        }
    }
}

VA(0x004669c6, 0x69)
void advManager::CheckLoadSample(i32 index) {
    if (m_loopingSamples[index] == NULL) {
        TrimLoopingSounds(LOOPING_SOUND_LIMIT);
        sprintf(gText, DATA_COMPGEN(0x004f6734, checkLoadSampleLoop04d82M, "loop%04d.82M"), index);
        m_loopingSamples[index] = gpResourceManager->GetSample(gText);
    }
}

VA(0x00466a2f, 0x4c1)
AdventureEnvironmentSoundId advManager::GetSoundId(i32 x, i32 y) {
    mapCell* currentCell = &m_mapData->Row(y)[x];
    AdventureEnvironmentSoundId soundId = ADVMGR_ENVIRONMENT_SOUND_NONE;

    if (giGroundToTerrain[currentCell->m_terrainImageIndex] == TERRAIN_WATER
        && (giGroundShape[currentCell->m_terrainImageIndex] & SOUND_GROUND_SHAPE_MASK)) {
        return ADVMGR_SOUND_COASTLINE;
    }

    if (HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
        switch (currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
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
                if (HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                    return ADVMGR_SOUND_ALCHEMIST_LAB_ACTION;
                }
                break;
            case MAP_OBJECT_MINE:
                if (HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                    return ADVMGR_SOUND_MINE;
                }
                break;
            case MAP_OBJECT_ABANDONED_MINE:
                if (HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                    return ADVMGR_SOUND_ABANDONED_MINE;
                }
                break;
            case MAP_OBJECT_SAWMILL:
                if (HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                    return ADVMGR_SOUND_SAWMILL;
                }
                break;
            case MAP_OBJECT_DAEMON_CAVE:
                if (HAS(currentCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                    return ADVMGR_SOUND_DAEMON_CAVE;
                }
                break;
        }
    } else {
        switch (currentCell->m_triggerType) {
            case MAP_OBJECT_TAR_PIT:
                return ADVMGR_SOUND_TAR_PIT;
            case MAP_OBJECT_LAVA_POOL:
                if (currentCell->m_objectIndex >= SOUND_ALCHEMIST_FRAME_FIRST
                    && currentCell->m_objectIndex <= SOUND_ALCHEMIST_FRAME_LAST) {
                    return ADVMGR_SOUND_ALCHEMIST_LAB;
                } else {
                    return ADVMGR_SOUND_LAVA_POOL;
                }
            case MAP_OBJECT_VOLCANO:
                if (currentCell->m_objectTileset == TILESET_OBJNLAV3
                    || currentCell->m_objectTileset == TILESET_OBJNLAV2) {
                    return ADVMGR_SOUND_LARGE_VOLCANO;
                } else {
                    return ADVMGR_SOUND_SMALL_VOLCANO;
                }
            case MAP_OBJECT_WATER_LAKE:
                if (currentCell->m_objectTileset == TILESET_OBJNSNOW) {
                    break;
                }
                return ADVMGR_SOUND_WATERING_HOLE;
        }

        switch (currentCell->m_objectTileset) {
            case TILESET_STREAM:
                return ADVMGR_SOUND_STREAM;
            case TILESET_OBJNWAT2:
                if (currentCell->m_objectIndex == SOUND_SEAGULL_FRAME_FIRST
                    || currentCell->m_objectIndex == SOUND_SEAGULL_FRAME_LAST) {
                    return ADVMGR_SOUND_SEAGULLS;
                }
                break;
        }
    }

    return ADVMGR_ENVIRONMENT_SOUND_NONE;
}

VA(0x00466ef0, 0x23a)
void advManager::InsertSound(i32 x, i32 mapY, i32 distance, i32 soundLayer) {
    i32 soundSlot;
    i32 distanceLimit;
    i32 activeIndex;
    AdventureEnvironmentSoundId soundId;

    if (x < 0 || mapY < 0 || MAP_WIDTH <= x || MAP_HEIGHT <= mapY) {
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
                m_activeSoundMask |= BIT(m_activeSounds[activeIndex].soundId);
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
            gpSoundManager->StopSample(
                m_loopingSamples[IDX(m_activeSounds[soundSlot].soundId)]
                    ->m_playbackData.activeSample
            );
        }
        m_activeSounds[soundSlot].soundId = soundId;
        m_activeSounds[soundSlot].volume = distance;
        CheckLoadSample(IDX(soundId));
        m_loopingSamples[IDX(soundId)]->m_playbackData.volume = ADVMGR_ENVIRONMENT_VOLUME(distance);
        m_loopingSamples[IDX(soundId)]->m_playbackData.loopCount = 0;
        m_loopingSamples[IDX(soundId)]->m_playbackData.channelType =
            ENVIRONMENT_SOUND_CHANNEL_TYPE;
        gpSoundManager->MemorySample(m_loopingSamples[IDX(soundId)]);
        m_activeSoundMask ^= 1 << IDX(m_activeSounds[soundSlot].soundId);
    }
}

VA(0x0046712a, 0x40f)
void advManager::TeleportTo(
    hero* mapHero,
    i32 destinationX,
    i32 destinationY,
    i32,
    i32 skipMapChange
) {
    i32 savedShow11;
    H2_ENUM_STORAGE(TerrainType, i32) terrain5;
    mapCell* oldCell2;
    i32 oldCellFlag26;
    i32 unused47;
    mapCell* destinationCell29;
    i32 fizzleTime36;
    town* occupiedTown47;

    savedShow11 = bShowIt;
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
    oldCell2 = GetCell(mapHero->m_x, mapHero->m_y);
    if (mapHero->m_locationType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
        occupiedTown47 = gpGame->GetTown(mapHero->m_occupiedTown);
        occupiedTown47->m_occupyingHeroId = INVALID_HERO;
    }

    oldCellFlag26 = 0;
    if (oldCell2->m_flags & TELEPORT_CELL_OBJECT_FLAG) {
        oldCell2->m_flags -= TELEPORT_CELL_OBJECT_FLAG;
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

    if (savedShow11 != 0) {
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
        giVisRange[IDX(mapHero->m_secondarySkills[IDX(HERO_SKILL_SCOUTING)])]
            + (static_cast<u32>(mapHero->HasArtifact(ARTIFACT_TELESCOPE)) >= 1)
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
            mapHero->m_eventFlags = HERO_EVENT_EMBARKED | mapHero->m_eventFlags;
        }
        m_cursorActive = 0;
    }

    SetEnvironmentOrigin(
        m_mapOriginX + TELEPORT_VIEW_CENTER,
        m_mapOriginY + TELEPORT_VIEW_CENTER,
        1
    );
    terrain5 = giGroundToTerrain[destinationCell29->m_terrainImageIndex];
    if (m_currentTerrain != terrain5) {
        m_currentTerrain = terrain5;
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
    }
    Reseed(0, 0);
    UpdateRadar(1, 0);
    CompleteDraw(0);
    ForceNewHover();
}

VA(0x00467539, 0x1fb)
void advManager::DimensionDoor(void) {
    hero* targetHero;
    heroWindow* dimensionDoorWindow;
    i32 x;
    i32 y;
    mapCell* targetCell;

    dimensionDoorWindow = new heroWindow(0, 0, DATA_COMPGEN(0x004f6744, dimensionDoorDimdoorBin, "dimdoor.bin"));
    if (dimensionDoorWindow == NULL) {
        MemError();
    }
    gpWindowManager->DoDialog(dimensionDoorWindow, DimensionDoorHandler, 0);
    delete dimensionDoorWindow;

    targetHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    if (gpWindowManager->m_dialogResult == TRAVEL_DIALOG_ACCEPT) {
        x = m_lastHoverCell + m_mapOriginX;
        y = m_hoverCellY + m_mapOriginY;
        targetCell = GetCell(x, y);
        if ((HAS(targetHero->m_eventFlags, HERO_EVENT_EMBARKED)
             && giGroundToTerrain[targetCell->m_terrainImageIndex] != TERRAIN_WATER)
            || (!HAS(targetHero->m_eventFlags, HERO_EVENT_EMBARKED)
                && giGroundToTerrain[targetCell->m_terrainImageIndex] == TERRAIN_WATER)) {
            NormalDialog(
                DATA_COMPGEN(0x004f6750, dimensionDoorDimensionDoorFailed, "Dimension Door failed!!!"),
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
            TeleportTo(targetHero, x, y, 0, 0);
            gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
        }
        gpGame->GetHero(gpCurPlayer->m_currentHero)->UseSpell(SPELL_DIMENSION_DOOR);
    } else {
        UpdateRadar(1, 0);
    }
}

VA(0x00467734, 0x129)
MessageDispatchResult TownPortalHandler(tag_message& message) {
    tag_message choiceMessage;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == 1) {
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[IDX(gpAdvManager->m_currentTerrain)]
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

VA(0x0046785d, 0x43e)
void advManager::TownGate(SpellType spellId) {
    i32 distance0;
    hero* targetHero;
    tag_message message;
    i32 selectedTownIndex;
    i32 nearestDistance;
    i32 townListIndex;

    nearestDistance = TOWN_PORTAL_DISTANCE_LIMIT;
    selectedTownIndex = INVALID_HERO;
    targetHero = gpGame->GetHero(gpCurPlayer->m_currentHero);

    if (gpCurPlayer->m_townCount == 0) {
        NormalDialog(
            DATA_COMPGEN(0x004f676c, townGateNoAvailableTownSpellFailed, "No available town.  Spell Failed!!!"),
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
    if (HAS(targetHero->m_eventFlags, HERO_EVENT_EMBARKED)) {
        NormalDialog(
            DATA_COMPGEN(0x004f6790, townGateSpellFailedYouMustBeOn, "Spell Failed!!!  You must be on land for this spell to work."),
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
            DATA_COMPGEN(0x004f67d0, townGateTownportBin, "townport.bin")
        );
        sprintf(gText, DATA_COMPGEN(0x004f67e0, townGateTownPortalSelectTownToPort, "{Town Portal}\n\nSelect town to port to."));
        message.type = ADVMGR_TOWN_PORTAL_MESSAGE;
        message.payload.widget.command = ADVMGR_TOWN_PORTAL_COMMAND_TEXT;
        message.payload.widget.id = TOWN_PORTAL_TITLE_WIDGET;
        message.payload.widget.data.text = gText;
        townPortalWin->BroadcastMessage(message);

        for (townListIndex = 0; townListIndex < gpCurPlayer->m_townCount; ++townListIndex) {
            sprintf(gText, gpGame->m_castleRecs[gpCurPlayer->TownId(townListIndex)].m_name);
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
        selectedTownIndex = giTownPortalChoice;
        delete townPortalWin;
        if (gpWindowManager->m_dialogResult == TOWN_PORTAL_FIRST_CHOICE) {
            return;
        }
    } else {
        for (townListIndex = 0; townListIndex < gpCurPlayer->m_townCount; ++townListIndex) {
            distance0 = abs(
                            
                                gpGame->m_castleRecs[gpCurPlayer->m_townIds[townListIndex]].m_y
                             - targetHero->m_y
                        )
                        + abs(
                            gpGame->m_castleRecs[gpCurPlayer->m_townIds[townListIndex]].m_x
                            - targetHero->m_x
                        );
            if (distance0 < nearestDistance) {
                nearestDistance = distance0;
                selectedTownIndex = townListIndex;
            }
        }
    }

    if (gpGame->m_castleRecs[gpCurPlayer->m_townIds[selectedTownIndex]].m_occupyingHeroId
        != INVALID_HERO) {
        NormalDialog(
            DATA_COMPGEN(0x004f6808, townGateNearestTownOccupiedSpellFailed, "Nearest town occupied.  Spell Failed!!!"),
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
        gpGame->m_castleRecs[gpCurPlayer->m_townIds[selectedTownIndex]].m_x,
        gpGame->m_castleRecs[gpCurPlayer->m_townIds[selectedTownIndex]].m_y,
        0,
        0
    );
    targetHero->UseSpell(spellId);
    gpGame->m_castleRecs[gpCurPlayer->m_townIds[selectedTownIndex]].m_occupyingHeroId =
        targetHero->m_id;
    gpGame->m_castleRecs[gpCurPlayer->m_townIds[selectedTownIndex]].GiveSpells(NULL);
    targetHero->m_locationType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE;
    targetHero->m_occupiedTown = gpCurPlayer->m_townIds[selectedTownIndex];
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
}

VA(0x00467c9b, 0x5ac)
void advManager::SummonBoat(void) {
    i32 boatIndex9;
    mapCell* destinationCell;
    i32 foundBoat;
    hero* currentHero11;
    i32 destinationX10;
    i32 direction5;
    i32 destinationY15;
    i32 foundDestination9;
    i32 currentHeroId3;
    boatRecord* boat1;
    mapCell* oldBoatCell26;
    i32 screenX4;
    i32 screenY3;
    i32 fizzleHeight;
    i32 fizzleWidth;

    currentHero11 = gpGame->GetHero(gpCurPlayer->m_currentHero);
    foundDestination9 = 0;
    foundBoat = 0;
    destinationCell = GetCell(
        m_mapOriginX + SUMMON_CENTER_OFFSET,
        m_mapOriginY + SUMMON_CENTER_OFFSET
    );
    if (giGroundToTerrain[destinationCell->m_terrainImageIndex] == TERRAIN_WATER) {
        return;
    } else {

        for (direction5 = 0; direction5 < SUMMON_DIRECTION_COUNT; ++direction5) {
            destinationX10 =
                normalDirTable[direction5].x + m_mapOriginX + SUMMON_CENTER_OFFSET;
            destinationY15 =
                normalDirTable[direction5].y + m_mapOriginY + SUMMON_CENTER_OFFSET;
            if (destinationX10 < 0 || destinationX10 >= MAP_WIDTH || destinationY15 < 0
                || destinationY15 >= MAP_HEIGHT) {
                continue;
            }

            destinationCell = GetCell(destinationX10, destinationY15);
            if (destinationCell->m_objectIndex == MAPCELL_SPRITE_NONE
                && destinationCell->m_triggerType == MAP_OBJECT_NONE
                && giGroundToTerrain[destinationCell->m_terrainImageIndex] == TERRAIN_WATER) {
                foundDestination9 = 1;
                break;
            }
        }

        if (foundDestination9) {
            currentHeroId3 = gpCurPlayer->CurrentHero();
            for (boatIndex9 = 0; boatIndex9 < SUMMON_BOAT_COUNT; ++boatIndex9) {
                if (gpGame->m_boatSlots[boatIndex9] != -1
                    && gpGame->m_boats[boatIndex9].heroId
                           == (currentHeroId3 | SUMMON_OCCUPIED_FLAG)) {
                    foundBoat = 1;
                    break;
                }
            }

            if (!foundBoat) {
                for (boatIndex9 = 0; boatIndex9 < SUMMON_BOAT_COUNT; ++boatIndex9) {
                    if (gpGame->m_boatSlots[boatIndex9] != -1
                        && (gpGame->m_boats[boatIndex9].heroId & SUMMON_OCCUPIED_FLAG)
                        && gpGame->m_boats[boatIndex9].owner == giCurPlayer
                        && abs(gpGame->m_boats[boatIndex9].y - currentHero11->m_y)
                                   + abs(gpGame->m_boats[boatIndex9].x - currentHero11->m_x)
                               > SUMMON_MIN_DISTANCE) {
                        foundBoat = 1;
                        break;
                    }
                }
            }

            if (foundBoat) {
                boat1 = &gpGame->m_boats[boatIndex9];
                oldBoatCell26 = GetCell(boat1->x, boat1->y);
                gpGame->RestoreCell(
                    boat1->x,
                    boat1->y,
                    boat1->savedTriggerType,
                    boat1->savedEventData,
                    NULL,
                    SUMMON_RESTORE_MODE
                );

                if (boat1->x >= m_mapOriginX && boat1->x < m_mapOriginX + HOVER_VIEW_CELLS
                    && boat1->y >= m_mapOriginY
                    && boat1->y < m_mapOriginY + HOVER_VIEW_CELLS) {
                    screenX4 = (boat1->x - m_mapOriginX) * CELL_PIXELS
                               - SUMMON_FIZZLE_X_OFFSET;
                    if (screenX4 < SUMMON_SCREEN_MARGIN) {
                        screenX4 = SUMMON_SCREEN_MARGIN;
                    }
                    screenY3 = (boat1->y - m_mapOriginY) * CELL_PIXELS
                               - SUMMON_FIZZLE_X_OFFSET;
                    if (screenY3 < SUMMON_SCREEN_MARGIN) {
                        screenY3 = SUMMON_SCREEN_MARGIN;
                    }

                    fizzleWidth = SUMMON_FIZZLE_WIDTH;
                    fizzleHeight = SUMMON_FIZZLE_HEIGHT;
                    if (screenX4 + fizzleWidth >= SUMMON_SCREEN_LIMIT) {
                        fizzleWidth = SUMMON_SCREEN_LIMIT - screenX4;
                    }
                    if (screenY3 + fizzleHeight >= SUMMON_SCREEN_LIMIT) {
                        fizzleHeight = SUMMON_SCREEN_LIMIT - screenY3;
                    }
                    gpWindowManager
                        ->SaveFizzleSource(screenX4, screenY3, fizzleWidth, fizzleHeight);
                    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
                    gpWindowManager
                        ->FizzleForward(screenX4, screenY3, fizzleWidth, fizzleHeight, -1, NULL, NULL);
                }

                boat1->x = static_cast<i8>(
                    normalDirTable[direction5].x + m_mapOriginX + SUMMON_CENTER_OFFSET
                );
                boat1->y = static_cast<i8>(
                    normalDirTable[direction5].y + m_mapOriginY + SUMMON_CENTER_OFFSET
                );
                boat1->savedTriggerType = destinationCell->m_triggerType;
                boat1->savedEventData = static_cast<u8>(destinationCell->m_objectMetadata);
                destinationCell->m_triggerType = SUMMON_BOAT_TRIGGER;
                destinationCell->m_objectMetadata = boatIndex9;

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

        UpdateScreen(0, 0);
        Reseed(0, 0);
        if (!foundBoat) {
            NormalDialog(
                DATA_COMPGEN(0x004f6830, summonBoatSummonBoatFailed, "Summon Boat failed!!!"),
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
        return;
    }
}

VA(0x00468247, 0x4d9)
void advManager::ShowRoute(i32 redraw, i32, i32 updateButton) {
    i32 routeReachable8;
    i32 pathFound5;
    i32 routeX1;
    mapCell* nextCell7;
    i32 previousDirection0;
    hero* currentHero0;
    i32 routeY1;
    i32 direction;
    i32 terrainCost;
    i32 remainingMobility2;
    i32 pathIndex;
    H2_ENUM_STORAGE(TerrainType, i32) currentTerrain0;
    mapCell* currentCell2;
    i32 routeFrame;
    BaseWidgetCommand buttonFrame;

    routeReachable8 = 0;
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        return;
    }

    if (gpCurPlayer->m_currentHero == INVALID_HERO) {
        HideRoute(redraw, 0, 1);
        return;
    }

    currentHero0 = gpGame->GetHero(gpCurPlayer->m_currentHero);
    if (currentHero0->m_destinationX == HERO_DESTINATION_NONE) {
        HideRoute(redraw, 1, 1);
        return;
    }

    pathFound5 = gpSearchArray->BuildPath(
        currentHero0->m_x,
        currentHero0->m_y,
        currentHero0->m_destinationX,
        currentHero0->m_destinationY,
        ROUTE_PATH_COST_LIMIT
    );
    if (gpSearchArray->m_pathLength > 0 && pathFound5 > 0) {
        memset(
            m_visibilityMap,
            0,
            MAP_WIDTH * MAP_HEIGHT * sizeof(*m_visibilityMap)
        );
        m_visibilityMapValid = 1;
        remainingMobility2 = currentHero0->m_remainingMobility;
        routeX1 = currentHero0->m_x;
        routeY1 = currentHero0->m_y;

        for (pathIndex = gpSearchArray->m_pathLength - 1; pathIndex >= 0; --pathIndex) {
            direction = static_cast<u8>(gpSearchArray->m_storage.path.directions[pathIndex + 1]);
            currentCell2 = GetCell(routeX1, routeY1);
            routeX1 += normalDirTable[direction].x;
            routeY1 += normalDirTable[direction].y;
            nextCell7 = GetCell(routeX1, routeY1);
            currentTerrain0 = giGroundToTerrain[currentCell2->m_terrainImageIndex];
            terrainCost = CalcTerrainCost(
                giGroundToTerrain[nextCell7->m_terrainImageIndex],
                direction & 1,
                ROUTE_TERRAIN_COST_INFINITY,
                IDX(currentHero0->m_secondarySkills[IDX(HERO_SKILL_PATHFINDING)]),
                currentCell2->m_isRoad,
                nextCell7->m_isRoad
            );
            remainingMobility2 -= CalcTerrainCost(
                currentTerrain0,
                direction & 1,
                remainingMobility2,
                IDX(currentHero0->m_secondarySkills[IDX(HERO_SKILL_PATHFINDING)]),
                currentCell2->m_isRoad,
                nextCell7->m_isRoad
            );

            if (direction & 1) {
                if (terrainCost == ROUTE_DIAGONAL_COST_0) {
                    routeFrame = ROUTE_FRAME_COST_0;
                } else if (terrainCost == ROUTE_DIAGONAL_COST_1) {
                    routeFrame = ROUTE_FRAME_COST_1;
                } else if (terrainCost == ROUTE_DIAGONAL_COST_2) {
                    routeFrame = ROUTE_FRAME_COST_2;
                } else if (terrainCost == ROUTE_DIAGONAL_COST_3) {
                    routeFrame = ROUTE_FRAME_COST_3;
                } else if (terrainCost == ROUTE_DIAGONAL_COST_4) {
                    routeFrame = ROUTE_FRAME_COST_4;
                } else if (terrainCost == ROUTE_DIAGONAL_COST_5) {
                    routeFrame = ROUTE_FRAME_COST_5;
                } else {
                    routeFrame = ROUTE_FRAME_COST_1;
                }
            } else {
                if (terrainCost == ROUTE_STRAIGHT_COST_0) {
                    routeFrame = ROUTE_FRAME_COST_0;
                } else if (terrainCost == ROUTE_STRAIGHT_COST_1) {
                    routeFrame = ROUTE_FRAME_COST_1;
                } else if (terrainCost == ROUTE_STRAIGHT_COST_2) {
                    routeFrame = ROUTE_FRAME_COST_2;
                } else if (terrainCost == ROUTE_STRAIGHT_COST_3) {
                    routeFrame = ROUTE_FRAME_COST_3;
                } else if (terrainCost == ROUTE_STRAIGHT_COST_4) {
                    routeFrame = ROUTE_FRAME_COST_4;
                } else if (terrainCost == ROUTE_STRAIGHT_COST_5) {
                    routeFrame = ROUTE_FRAME_COST_5;
                } else {
                    routeFrame = ROUTE_FRAME_COST_1;
                }
            }

            if (pathIndex == 0) {
                m_visibilityMap[routeY1 * OR_STEER(MAP_WIDTH) + routeX1] = 1;
            } else {
                previousDirection0 =
                    static_cast<u8>(gpSearchArray->m_storage.path.directions[pathIndex]);
                m_visibilityMap[routeY1 * OR_STEER(MAP_WIDTH) + routeX1] = static_cast<u16>(
                    gbArrow[previousDirection0][direction | 0]
                    + routeFrame * ROUTE_ARROW_FRAME_STRIDE + ROUTE_ARROW_FRAME_OFFSET
                );
            }

            if (remainingMobility2 < 0) {
                m_visibilityMap[routeY1 * OR_STEER(MAP_WIDTH) + routeX1] += ROUTE_DAY_MASK;
            } else {
                routeReachable8 = 1;
            }
        }

        if (updateButton) {
            buttonFrame = routeReachable8 ? ADVMGR_BUTTON_DISABLE : ADVMGR_BUTTON_ENABLE;
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

VA(0x00468720, 0x107)
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
        currentHero = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];
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

VA(0x00468827, 0x8d)
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

VA(0x004688b4, 0x6b)
void advManager::CheckDimNextHeroBut(void) {
    BaseWidgetCommand frame;
    if (!gbThisNetHumanPlayer[giCurPlayer] || !gpCurPlayer->HasMobileHero()) {
        frame = ADVMGR_BUTTON_ENABLE;
    } else {
        frame = ADVMGR_BUTTON_DISABLE;
    }
    gpWindowManager->BroadcastMessage(
        ADVMGR_BUTTON_MESSAGE,
        frame,
        BUTTON_BROADCAST_ARG,
        BUTTON_BROADCAST_FLAGS
    );
}

VA(0x0046891f, 0x138)
void advManager::SeedTo(i32 targetX, i32 targetY) {
    hero* currentHero;

    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        return;
    }
    if (gpCurPlayer->m_currentHero == INVALID_HERO) {
        return;
    }

    currentHero = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];
    if (!giSeedingValid) {
        gpSearchArray->SeedPosition(
            currentHero->m_x,
            currentHero->m_y,
            m_cursorDirection,
            ROUTE_PATH_COST_LIMIT,
            m_cursorType == HERO_TYPE_BOAT,
            0,
            currentHero->m_remainingMobility,
            IDX(currentHero->m_secondarySkills[IDX(HERO_SKILL_PATHFINDING)]),
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
            IDX(currentHero->m_secondarySkills[IDX(HERO_SKILL_PATHFINDING)]),
            targetX,
            targetY,
            1,
            1
        );
    }
}

VA(0x00468a57, 0x5f)
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

VA(0x00468ab6, 0x1a6)
void advManager::ScreenScroll(H2_ENUM_PARAM(MapDirection, i32) direction, i32 updatePointer) {
    i32 originX;
    i32 originY;

    originX = m_mapOriginX;
    originY = m_mapOriginY;
    iLastScrollTime = KBTickCount();

    switch (direction) {
        case MAP_DIRECTION_NORTH:
            --originY;
            break;
        case MAP_DIRECTION_NORTH_EAST:
            ++originX;
            --originY;
            break;
        case MAP_DIRECTION_EAST:
            ++originX;
            break;
        case MAP_DIRECTION_SOUTH_EAST:
            ++originX;
            ++originY;
            break;
        case MAP_DIRECTION_SOUTH:
            ++originY;
            break;
        case MAP_DIRECTION_SOUTH_WEST:
            --originX;
            ++originY;
            break;
        case MAP_DIRECTION_WEST:
            --originX;
            break;
        case MAP_DIRECTION_NORTH_WEST:
            --originX;
            --originY;
            break;
    }

    if (updatePointer) {
        gpMouseManager->SetPointer(IDX(direction) + IDX(HOVER_SCROLL_FRAME_FIRST));
    }

    if (originX < SCROLL_MIN_ORIGIN) {
        originX = SCROLL_MIN_ORIGIN;
    }
    if (MAP_WIDTH - VIEW_EDGE_MARGIN < originX) {
        originX = MAP_WIDTH - VIEW_EDGE_MARGIN;
    }
    if (originY < SCROLL_MIN_ORIGIN) {
        originY = SCROLL_MIN_ORIGIN;
    }
    if (MAP_HEIGHT - VIEW_EDGE_MARGIN < originY) {
        originY = MAP_HEIGHT - VIEW_EDGE_MARGIN;
    }

    if (m_mapOriginX != originX || m_mapOriginY != originY) {
        DemobilizeCurrHero();
        m_mapOriginX = originX;
        m_mapOriginY = originY;
        UpdateRadar(1, 0);
        CompleteDraw(0);
        UpdateScreen(0, 0);
    }
}

VA(0x00468c5c, 0x1bb)
void advManager::CheckScreenScroll(void) {
    i32 mouseX6;
    i32 mouseY1;
    i32 oldOriginX9;
    i32 oldOriginY3;

    if (KBTickCount() - iLastScrollTime > SCROLL_TICK_INTERVAL) {
        iLastScrollTime = KBTickCount();
        oldOriginX9 = m_mapOriginX;
        oldOriginY3 = m_mapOriginY;
        gpMouseManager->MouseCoords(mouseX6, mouseY1);

        if (mouseX6 >= 0 && mouseX6 < SCREEN_WIDTH && mouseY1 >= 0
            && mouseY1 < SCREEN_HEIGHT) {
            if (mouseX6 < SCROLL_BORDER) {
                if (mouseY1 < SCROLL_BORDER) {
                    ScreenScroll(MAP_DIRECTION_NORTH_WEST, 1);
                } else if (mouseY1 > SCREEN_HEIGHT - SCROLL_BORDER) {
                    ScreenScroll(MAP_DIRECTION_SOUTH_WEST, 1);
                } else {
                    ScreenScroll(MAP_DIRECTION_WEST, 1);
                }
            } else if (mouseX6 > SCREEN_WIDTH - SCROLL_BORDER - 1) {
                if (mouseY1 < SCROLL_BORDER) {
                    ScreenScroll(MAP_DIRECTION_NORTH_EAST, 1);
                } else if (mouseY1 > SCREEN_HEIGHT - SCROLL_BORDER) {
                    ScreenScroll(MAP_DIRECTION_SOUTH_EAST, 1);
                } else {
                    ScreenScroll(MAP_DIRECTION_EAST, 1);
                }
            } else if (mouseY1 < SCROLL_BORDER) {
                ScreenScroll(MAP_DIRECTION_NORTH, 1);
            } else if (mouseY1 > SCREEN_HEIGHT - SCROLL_BORDER) {
                ScreenScroll(MAP_DIRECTION_SOUTH, 1);
            }
        }

        if (gpMouseManager->m_cursorFrame >= HOVER_SCROLL_FRAME_FIRST
            && gpMouseManager->m_cursorFrame < HOVER_SCROLL_FRAME_END
            && m_mapOriginX == oldOriginX9 && m_mapOriginY == oldOriginY3) {
            gpMouseManager->SetPointer(POINTER_DEFAULT);
        }
    }
}

VA(0x00468e17, 0x91)
i32 advManager::MouseInScrollZone(void) {
    i32 x;
    i32 y;

    gpMouseManager->MouseCoords(x, y);
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        if (x < SCROLL_BORDER || x > SCREEN_WIDTH - SCROLL_BORDER - 1
            || y < SCROLL_BORDER || y > SCREEN_HEIGHT - SCROLL_BORDER) {
            return 1;
        }
    }
    return 0;
}

VA(0x00468ea8, 0x2b8)
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
    m_lastHoverCell = m_hoverCellY;
    m_cursorActive = 0;
    gbHeroMoving = false;

    if (gbThisNetHumanPlayer[giCurPlayer]
        && gpCurPlayer->CurrentTown() != TOWN_ID_NONE) {
        currentTown9 = &gpGame->m_castleRecs[gpCurPlayer->CurrentTown()];
        m_mapOriginX = currentTown9->m_x - VIEW_CENTER_OFFSET;
        m_mapOriginY = currentTown9->m_y - VIEW_CENTER_OFFSET;
    } else if (gbThisNetHumanPlayer[giCurPlayer]
               && gpCurPlayer->CurrentHero() != INVALID_HERO) {
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
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
    SetEnvironmentOrigin(
        m_mapOriginX + VIEW_CENTER_OFFSET,
        m_mapOriginY + VIEW_CENTER_OFFSET,
        1
    );
    Reseed(0, 0);
    CheckDimNextHeroBut();
}

VA(0x00469160, 0x1be)
void advManager::LoadRemote(void) {
    if (gbThisNetHumanPlayer[giCurPlayer]) {
        gpMouseManager->SetPointer(DATA_COMPGEN(0x004f6848, loadRemoteAdvmiceMse, "advmice.mse"), POINTER_DEFAULT, MOUSE_AUTO_CURSOR_TYPE);
    }

    gpGame->LoadGame(gConfig.rmtRCName, 0, 1);
    if ((gpGame->m_day != 1 || (gpGame->m_week == 1 && gpGame->m_month == 1)) && gbRemoteOn
        && gbThisNetHumanPlayer[giCurPlayer]) {
        gpSoundManager->m_samplesReady = 1;
        gpSoundManager->SwitchAmbientMusic(WAIT_AMBIENT_MUSIC);
        gpSoundManager->m_samplesReady = 0;
        giForceSwitchMusic = KBTickCount();
    }

    if (gpGame->m_playerDead[giCurPlayer]) {
        ComputeAdvNetControl();
    }

    if (gbThisNetHumanPlayer[giCurPlayer]) {
        gpGame->CancelComputerScreen();
        gbThisNetGotAdventureControl = true;
        gpSoundManager->m_samplesReady = 0;
    }

    gpGame->DoNewTurn();
    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    UpdateRadar(1, 0);
    UpdBottomView(1, 1, 1);
    gpAdvManager->ForceNewHover();
    SendMapChange(MAP_CHANGE_MY_TURN, 0, 0, 0, MAP_CHANGE_CURRENT_PLAYER, 0, 0);
    gpSoundManager->m_samplesReady = 1;
}

VA(0x0046931e, 0x20c)
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
                LogStr(DATA_COMPGEN(0x004f6854, checkHandleNetReceiveRemotePlayerExit, "Receive Remote Player Exit"));
                ReceiveRemotePlayerExit(ADVMGR_REMOTE_PAYLOAD(packet9)->playerExit);
                break;

            case ADVMGR_REMOTE_COMMAND_HOST_PLAYER_EXIT:
                LogStr(DATA_COMPGEN(0x004f6870, checkHandleNetHostReportsPlayerExit, "Host Reports Player Exit"));
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

VA(0x0046952a, 0xcd)
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
                if (HAS(
                        message.payload.widget.modifiers,
                        MESSAGE_MODIFIER_CONTROL_KEYS
                    )) {
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

VA(0x004695f7, 0x1d5)
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

    i32 loadedSampleCount = 0;
    char retainedSamples[LOOPING_SAMPLE_COUNT];
    memset(retainedSamples, 0, sizeof(retainedSamples));

    i32 soundIndex;
    for (soundIndex = 0; soundIndex < SOUND_CELL_COUNT; ++soundIndex) {
        if (m_activeSounds[soundIndex].soundId >= ADVMGR_SOUND_BUOY
            && m_activeSounds[soundIndex].soundId < ADVMGR_ENVIRONMENT_SOUND_COUNT) {
            ++retainedSamples[IDX(m_activeSounds[soundIndex].soundId)];
        }
    }

    for (soundIndex = 0; soundIndex < LOOPING_SAMPLE_COUNT; ++soundIndex) {
        if (retainedSamples[soundIndex] != 0) {
            ++loadedSampleCount;
        }
    }

    if (loadedSampleCount < maxSamples) {
        for (soundIndex = 0; soundIndex < LOOPING_SAMPLE_COUNT; ++soundIndex) {
            if (retainedSamples[soundIndex] == 0 && m_loopingSamples[soundIndex] != NULL) {
                ++retainedSamples[soundIndex];
                ++loadedSampleCount;
                if (loadedSampleCount >= maxSamples) {
                    break;
                }
            }
        }
    }

    for (soundIndex = 0; soundIndex < LOOPING_SAMPLE_COUNT; ++soundIndex) {
        if (m_loopingSamples[soundIndex] != NULL && retainedSamples[soundIndex] == 0) {
            gpResourceManager->Dispose(m_loopingSamples[soundIndex]);
            m_loopingSamples[soundIndex] = NULL;
        }
    }
}

VA(0x004697cc, 0xd5)
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

VA(0x004698a1, 0xd5)
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

VA(0x00469976, 0x145)
void advManager::SaveAdventureBorder(void) {
    DATA(0x004f688c) static i16 s_saveBorderAllocLineBase = SAVE_BORDER_ALLOC_LINE_BASE;

    if (m_adventureBorder != NULL) {
        return;
    }

    m_adventureBorder = static_cast<u8*>(
        H2_ALLOC_AT(BORDER_BUFFER_SIZE, DATA_COMPGEN(0x004f6890, saveAdventureBorderSourceFile, RETAIL_FILE), s_saveBorderAllocLineBase + BORDER_ALLOC_LINE_OFFSET)
    );
    u8* savedPixels = m_adventureBorder;
    u8* screenPixel = gpWindowManager->m_screen->m_pixels;
    i32 row;
    for (row = 0; row < BORDER_EDGE_SIZE; ++row) {
        memcpy(savedPixels, screenPixel, BORDER_ROW_BYTES);
        screenPixel += BORDER_SCREEN_PITCH;
        savedPixels += BORDER_ROW_BYTES;
    }
    for (row = BORDER_EDGE_SIZE; row < BORDER_MIDDLE_END; ++row) {
        memcpy(savedPixels, screenPixel, BORDER_SIDE_BYTES);
        memcpy(
            savedPixels + BORDER_SIDE_BYTES,
            screenPixel + BORDER_MIDDLE_END,
            BORDER_SIDE_BYTES
        );
        screenPixel += BORDER_SCREEN_PITCH;
        savedPixels += BORDER_SAVED_SIDE_BYTES;
    }
    for (row = BORDER_MIDDLE_END; row < SCREEN_HEIGHT; ++row) {
        memcpy(savedPixels, screenPixel, BORDER_ROW_BYTES);
        screenPixel += BORDER_SCREEN_PITCH;
        savedPixels += BORDER_ROW_BYTES;
    }
}

VA(0x00469abb, 0x134)
void advManager::DrawAdventureBorder(void) {
    if (m_adventureBorder == NULL) {
        return;
    }
    if (gbNoBorder != 0) {
        return;
    }

    u8* screenPixel = gpWindowManager->m_screen->m_pixels;
    u8* savedPixels = m_adventureBorder;
    i32 row;
    for (row = 0; row < BORDER_EDGE_SIZE; ++row) {
        memcpy(screenPixel, savedPixels, BORDER_ROW_BYTES);
        screenPixel += BORDER_SCREEN_PITCH;
        savedPixels += BORDER_ROW_BYTES;
    }
    for (row = BORDER_EDGE_SIZE; row < BORDER_MIDDLE_END; ++row) {
        memcpy(screenPixel, savedPixels, BORDER_SIDE_BYTES);
        memcpy(
            screenPixel + BORDER_MIDDLE_END,
            savedPixels + BORDER_SIDE_BYTES,
            BORDER_SIDE_BYTES
        );
        screenPixel += BORDER_SCREEN_PITCH;
        savedPixels += BORDER_SAVED_SIDE_BYTES;
    }
    for (row = BORDER_MIDDLE_END; row < SCREEN_HEIGHT; ++row) {
        memcpy(screenPixel, savedPixels, BORDER_ROW_BYTES);
        screenPixel += BORDER_SCREEN_PITCH;
        savedPixels += BORDER_ROW_BYTES;
    }
}

VA(0x00469bef, 0x3d3)
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
                if (m_mapData->Row(s_adjacentMonsterY)[s_adjacentMonsterX].m_triggerType
                    == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER)) {
                    if (originY > s_adjacentMonsterY) {
                        if ((GetCell(originX, originY)->m_objectIndex
                                 == ADJACENT_OBJECT_INDEX_NONE
                             || GetCell(originX, originY)->m_objectTileset
                                    == TILESET_DUMMY
                             || (GetCell(originX, originY)->m_flags & HOVER_UNREACHABLE))
                            && (s_adjacentMonsterX != excludedX
                                || excludedY != s_adjacentMonsterY)) {
                            goto foundAdjacentMonster;
                        }
                    } else if (s_adjacentMonsterX != excludedX || excludedY != s_adjacentMonsterY) {
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
                if (m_mapData->Row(s_adjacentMonsterY)[s_adjacentMonsterX].m_triggerType
                    == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER)) {
                    if (originY > s_adjacentMonsterY) {
                        if ((GetCell(originX, originY)->m_objectIndex
                                 == ADJACENT_OBJECT_INDEX_NONE
                             || GetCell(originX, originY)->m_objectTileset
                                    == TILESET_DUMMY
                             || (GetCell(originX, originY)->m_flags & HOVER_UNREACHABLE))
                            && (s_adjacentMonsterX != excludedX
                                || excludedY != s_adjacentMonsterY)) {
                            goto foundAdjacentMonster;
                        }
                    } else if (s_adjacentMonsterX != excludedX || excludedY != s_adjacentMonsterY) {
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

VA(0x00469fc2, 0x125)
void ComputeAdvNetControl(void) {
    if (!gbRemoteOn) {
        gbThisNetGotAdventureControl = true;
        return;
    }
    {
        i32 selectedPlayer = -1;
        i32 player;
        i32 currentPlayer;
        if (gpGame->m_playerDead[giCurPlayer]) {
            player = (giCurPlayer + 1) % GAME_PLAYER_COUNT;
            while (giCurPlayer != player) {
                if (!gpGame->m_playerDead[player] && gbHumanPlayer[player]) {
                    gbThisNetGotAdventureControl = gbThisNetHumanPlayer[player];
                    return;
                }
            }
        }

        player = (giCurPlayer + 1) % GAME_PLAYER_COUNT;
        while (giCurPlayer != player) {
            player = (player + 1) % GAME_PLAYER_COUNT;
            if (!gpGame->m_playerDead[player] && gbHumanPlayer[player]) {
                selectedPlayer = player;
            }
        }
        gbThisNetGotAdventureControl = gbThisNetHumanPlayer[selectedPlayer];
    }
}

VA(0x0046a0e7, 0xf6)
i32 MapExtraPosAndAdjacentsSet(i32 x, i32 y, u8 mask) {
    if (mapExtra[MAP_WIDTH * y + x] & mask) {
        return 1;
    }
    for (i32 checkX = x - 1; checkX <= x + 1; ++checkX) {
        if (checkX < 0 || checkX >= MAP_WIDTH) {
            continue;
        }
        for (i32 checkY = y - 1; checkY <= y + 1; ++checkY) {
            if (checkY >= 0) {
                if (MAP_HEIGHT <= OD_STEER(checkY)) {
                } else {
                    if (mapExtra[OD_STEER(checkY) * MAP_WIDTH + checkX] & mask) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

VA(0x0046a1dd, 0x4c6)
void advManager::ViewPuzzle(void) {
    gpGame->SetupPuzzlePieces(giCurPlayer, 0);
    // Retail's fixed reveal permutation is payload, not a numeric domain.
    // NOLINTBEGIN(readability-magic-numbers)
    u8 puzzleOrderLocal[PUZZLE_PIECE_COUNT] = {23, 7,  44, 5,  24, 47, 1,  39, 16, 36,
                                                      11, 45, 31, 2,  30, 38, 43, 4,  3,  14,
                                                      40, 37, 34, 0,  12, 17, 35, 42, 15, 8,
                                                      26, 41, 28, 46, 10, 22, 21, 6,  32, 18,
                                                      19, 29, 13, 27, 9,  20, 33, 25};
    // NOLINTEND(readability-magic-numbers)
    i32 puzzlePiecesVisible = 0;

    gpSoundManager->SwitchAmbientMusic(PUZZLE_MUSIC);
    gpMouseManager->SetPointer(DATA_COMPGEN(0x004f68bc, viewPuzzleAdvmiceMse, "advmice.mse"), POINTER_DEFAULT, MOUSE_AUTO_CURSOR_TYPE);
    icon* puzzleIconObjectPointer = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f68c8, puzzleIconObjectPointerPuzzleIcn, "puzzle.icn"));
    i32 pieceIndexPosition;
    for (pieceIndexPosition = 0; pieceIndexPosition < PUZZLE_PIECE_COUNT;
         ++pieceIndexPosition) {
        puzzleIconObjectPointer->DrawToBuffer(0, 0, pieceIndexPosition, ICON_DRAW_NORMAL);
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
    heroWindow* puzzleWindowObject =
        new heroWindow(PUZZLE_WINDOW_X, PUZZLE_WINDOW_Y, DATA_COMPGEN(0x004f68d4, puzzleWindowObjectViewpuzlBin, "viewpuzl.bin"));
    if (puzzleWindowObject == NULL) {
        MemError();
    }
    gpWindowManager->AddWindow(puzzleWindowObject, -1, 1);

    i32 puzzleXViewCurrent = gpGame->m_ultimateArtifactX - PUZZLE_COORDINATE_OFFSET;
    i32 puzzleYTop = gpGame->m_ultimateArtifactY - PUZZLE_COORDINATE_OFFSET;
    i32 xAdjustmentOffset = 0;
    i32 yAdjustmentOffsetLocal = 0;
    xAdjustmentOffset = (gpGame->m_ultimateArtifactX + gpGame->m_ultimateArtifactY)
                            % PUZZLE_ALIGNMENT_DIVISOR
                        - 1;
    yAdjustmentOffsetLocal =
        (gpGame->m_ultimateArtifactX * PUZZLE_Y_ADJUST_X_FACTOR
         + gpGame->m_ultimateArtifactY * PUZZLE_Y_ADJUST_Y_FACTOR)
            % PUZZLE_ALIGNMENT_DIVISOR
        - 1;
    if ((gpGame->m_ultimateArtifactX + gpGame->m_ultimateArtifactY)
            % PUZZLE_ALIGNMENT_DIVISOR
        == 1) {
        if (xAdjustmentOffset > 0) {
            ++xAdjustmentOffset;
        } else if (xAdjustmentOffset < 0) {
            --xAdjustmentOffset;
        }
    } else if ((gpGame->m_ultimateArtifactX + gpGame->m_ultimateArtifactY)
                   % PUZZLE_PARITY_DIVISOR
               == 1) {
        if (yAdjustmentOffsetLocal > 0) {
            ++yAdjustmentOffsetLocal;
        } else if (yAdjustmentOffsetLocal < 0) {
            --yAdjustmentOffsetLocal;
        }
    }

    puzzleXViewCurrent += xAdjustmentOffset;
    puzzleYTop += yAdjustmentOffsetLocal;
    PuzzleDraw(
        puzzleXViewCurrent,
        puzzleYTop,
        gpGame->m_ultimateArtifactX,
        gpGame->m_ultimateArtifactY
    );

    i32 rowCounterNumber;
    u8* pixelIterator;
    u8* rowLimitAddress;
    for (rowCounterNumber = PUZZLE_VIEW_ORIGIN; rowCounterNumber < PUZZLE_VIEW_END;
         ++rowCounterNumber) {
        pixelIterator = gpWindowManager->m_screen->m_pixels + rowCounterNumber * SCREEN_WIDTH
                        + PUZZLE_VIEW_ORIGIN;
        rowLimitAddress = pixelIterator + PUZZLE_VIEW_SIZE;
        for (; OD_STEER(pixelIterator) < rowLimitAddress; ++pixelIterator) {
            *pixelIterator = gColorTableTan[*pixelIterator];
        }
    }

    for (pieceIndexPosition = 0; pieceIndexPosition < PUZZLE_PIECE_COUNT;
         ++pieceIndexPosition) {
        if (!BitTest(puzzlePiecesRemoved, pieceIndexPosition)) {
            puzzleIconObjectPointer->DrawToBuffer(0, 0, puzzleOrderLocal[pieceIndexPosition], ICON_DRAW_NORMAL);
            ++puzzlePiecesVisible;
        }
    }
    if (puzzlePiecesVisible != PUZZLE_PIECE_COUNT) {
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

    gpWindowManager->DoDialog(puzzleWindowObject, EventWindowHandler, 0);
    delete puzzleWindowObject;
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    UpdateScreen(0, 0);
    UpdateRadar(1, 0);
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
}

VA(0x0046a6a3, 0x81)
void advManager::PuzzleDraw(i32 left, i32 top, i32 right, i32 bottom) {
    // Retail reserves six unreferenced four-byte locals before the implicit `this` slot.
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
        m_objectIcons[IDX(TILESET_ROUTE)],
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

VA(0x0046a724, 0x2ac)
void advManager::AdvPanel(void) {
    heroWindow* adventurePanel;
    {
        TrimLoopingSounds(LOOPING_SOUND_LIMIT);
        gpMouseManager->SetPointer(DATA_COMPGEN(0x004f68e4, advPanelAdvmiceMse, "advmice.mse"), POINTER_DEFAULT, MOUSE_AUTO_CURSOR_TYPE);
        i32 heroWasMobilized = m_heroContextLocked;
        tag_message message;
        DemobilizeCurrHero();

        adventurePanel = new heroWindow(PANEL_WINDOW_X, PANEL_WINDOW_Y, DATA_COMPGEN(0x004f68f0, advPanelApanelBin, "apanel.bin"));
        if (adventurePanel == NULL) {
            MemError();
        }
        if (gpCurPlayer->CurrentHero() == INVALID_HERO) {
            message.type = MESSAGE_WIDGET;
            message.payload.widget.id = PANEL_DISABLED_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = BUTTON_TARGET;
            adventurePanel->BroadcastMessage(message);
            message.payload.widget.id = PANEL_DISABLED_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_COMMAND_DIMMED);
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
                        giTerrainToMusicTrack[IDX(m_currentTerrain)]
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
                        giTerrainToMusicTrack[IDX(m_currentTerrain)]
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

VA(0x0046a9d0, 0x1ca)
MessageDispatchResult APanelHandler(tag_message& message) {
    i32 handled = 0;
    if (message.type == MESSAGE_WIDGET) {
        if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) {
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
        message.payload.widget.id = IDX(WIDGET_COMMAND_DIALOG_SELECT);
        message.payload.widget.command =
            static_cast<BaseWidgetCommand>(message.payload.widget.id);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0046ab9a, 0x1e4)
i32 advManager::ControlPanel(void) {
    TrimLoopingSounds(LOOPING_SOUND_LIMIT);
    i32 selectedCommand = PANEL_NO_HELP;
    gpMouseManager->SetPointer(DATA_COMPGEN(0x004f68fc, controlPanelAdvmiceMse, "advmice.mse"), POINTER_DEFAULT, MOUSE_AUTO_CURSOR_TYPE);
    i32 heroWasMobilized = m_heroContextLocked;
    DemobilizeCurrHero();

    heroWindow* controlPanel =
        new heroWindow(PANEL_WINDOW_X, PANEL_WINDOW_Y, DATA_COMPGEN(0x004f6908, controlPanelCpanelBin, "cpanel.bin"));
    if (controlPanel == NULL) {
        MemError();
    }
    if (gbRemoteOn) {
        tag_message message;
        message.type = MESSAGE_WIDGET;
        message.payload.widget.id = CONTROL_RESTART;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = IDX(WIDGET_COMMAND_DIMMED);
        controlPanel->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = BUTTON_TARGET;
        controlPanel->BroadcastMessage(message);
        message.payload.widget.id = CONTROL_NEW_GAME;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = IDX(WIDGET_COMMAND_DIMMED);
        controlPanel->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = BUTTON_TARGET;
        controlPanel->BroadcastMessage(message);
    }

    gpWindowManager->DoDialog(controlPanel, CPanelHandler, 0);
    delete controlPanel;
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
    }
    return selectedCommand != PANEL_NO_HELP;
}

VA(0x0046ad7e, 0x304)
MessageDispatchResult CPanelHandler(tag_message& message) {
    i32 handled = 0;
    if (message.type == MESSAGE_WIDGET) {
        if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_LEFT_SHIFT)) {
            if (message.payload.widget.command == WIDGET_COMMAND_SELECT
                || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT) {
                i32 helpIndex = PANEL_NO_HELP;
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
                case WIDGET_COMMAND_DESELECT: {
                    char confirmation[CONTROL_CONFIRMATION_SIZE];
                    switch (message.payload.widget.id) {
                        case CONTROL_RESTART:
                            strcpy(
                                confirmation,
                                DATA_COMPGEN(0x004f6914, cPanelHandlerAreYouSureYouWantTo, "Are you sure you want to restart?  (Your current game will be "
                                "lost)")
                            );
                            break;
                        case CONTROL_NEW_GAME:
                            strcpy(
                                confirmation,
                                DATA_COMPGEN(0x004f6958, cPanelHandlerAreYouSureYouWantTo2, "Are you sure you want to load a new game?  (Your current game "
                                "will be lost)")
                            );
                            break;
                        case CONTROL_MAIN_MENU:
                            strcpy(confirmation, DATA_COMPGEN(0x004f69a4, cPanelHandlerAreYouSureYouWantTo3, "Are you sure you want to quit?"));
                            break;
                        case CONTROL_SAVE_GAME:
                        case PANEL_CLOSE_WIDGET:
                            handled = 1;
                            break;
                        default:
                            break;
                    }
                    if (message.payload.widget.id == CONTROL_RESTART
                        || message.payload.widget.id == CONTROL_NEW_GAME
                        || message.payload.widget.id == CONTROL_MAIN_MENU) {
                        handled = 1;
                        if (!bFreshSave) {
                            NormalDialog(
                                confirmation,
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
                    }
                    break;
                }
            }
        }
    }

    if (handled) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = IDX(WIDGET_COMMAND_DIALOG_SELECT);
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0046b082, 0x197)
void advManager::SystemOptions(void) {
    tag_message message6;
    i32 oldInterfaceMode;
    ConfigWalkSpeed prevWalkSpeed;
    i32 heroContextLocked;
    i32 sampleIndex;

    TrimLoopingSounds(LOOPING_SOUND_LIMIT);
    gpMouseManager->SetPointer(DATA_COMPGEN(0x004f69c4, systemOptionsAdvmiceMse, "advmice.mse"), POINTER_DEFAULT, MOUSE_AUTO_CURSOR_TYPE);
    prevWalkSpeed = gConfig.walkSpeed;
    oldInterfaceMode = gConfig.evilInterfaceUsage;
    heroContextLocked = m_heroContextLocked;
    bPrefsChanged = 0;
    DemobilizeCurrHero();

    cPanel = new heroWindow(
        ADVMGR_SYSTEM_OPTIONS_WINDOW_X,
        ADVMGR_SYSTEM_OPTIONS_WINDOW_Y,
        DATA_COMPGEN(0x004f69d0, systemOptionsSpanelBin, "spanel.bin")
    );
    if (cPanel == NULL) {
        MemError();
    }
    SetWinText(cPanel, ADVMGR_SYSTEM_OPTIONS_TITLE);
    UpdateSystemOptions(1);
    gpWindowManager->DoDialog(cPanel, SystemOptionsHandler, 0);
    delete cPanel;

    if (gConfig.walkSpeed != prevWalkSpeed) {
        for (sampleIndex = 0; sampleIndex < CURSOR_SAMPLE_COUNT; ++sampleIndex) {
            gpResourceManager->Dispose(m_cursorSamples[sampleIndex]);
        }
        GetCursorSampleSet(gConfig.walkSpeed);
    }
    if (bPrefsChanged) {
        WritePrefs();
    }
    if (gConfig.evilInterfaceUsage != oldInterfaceMode) {
        CheckSetEvilInterface(1, -1);
    }
    if (heroContextLocked) {
        MobilizeCurrHero(0);
    }
}

VA(0x0046b219, 0x35f)
void UpdateSystemOptions(i32 initialDraw) {
    tag_message message;
    AdventureMusicQuality musicQuality;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = ADVMGR_SYSTEM_OPTIONS_SET_FRAME;

    message.payload.widget.id = IDX(SYSTEM_OPTION_MUSIC_VOLUME);
    message.payload.widget.data.value = gConfig.musicVolume != CONFIG_VOLUME_MUTED;
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = IDX(SYSTEM_OPTION_SOUND_VOLUME);
    if (gConfig.soundVolume == CONFIG_VOLUME_MUTED) {
        message.payload.widget.data.value = ADVMGR_SYSTEM_OPTIONS_SOUND_FRAME_BASE;
    } else {
        message.payload.widget.data.value = ADVMGR_SYSTEM_OPTIONS_SOUND_FRAME_BASE + 1;
    }
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = IDX(SYSTEM_OPTION_HERO_SPEED);
    message.payload.widget.data.value =
        IDX(gConfig.walkSpeed) + ADVMGR_SYSTEM_OPTIONS_SPEED_FRAME_BASE;
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = IDX(SYSTEM_OPTION_MUSIC_SOURCE);
    if (gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI) {
        musicQuality = MUSIC_QUALITY_MIDI;
    } else if (gConfig.useOpera == CONFIG_OPERA_DISABLED) {
        musicQuality = MUSIC_QUALITY_CD_STEREO;
    } else {
        musicQuality = MUSIC_QUALITY_CD_OPERA;
    }
    message.payload.widget.data.value =
        IDX(musicQuality) + ADVMGR_SYSTEM_OPTIONS_MUSIC_SOURCE_FRAME_BASE;
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = IDX(SYSTEM_OPTION_SHOW_ROUTE);
    message.payload.widget.data.value =
        (gConfig.showRoute == 0) + ADVMGR_SYSTEM_OPTIONS_ROUTE_FRAME_BASE;
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = IDX(SYSTEM_OPTION_COMPUTER_SPEED);
    if (gConfig.blackoutComputer == 0) {
        message.payload.widget.data.value =
            IDX(gConfig.computerWalkSpeed) + ADVMGR_SYSTEM_OPTIONS_SPEED_FRAME_BASE;
    } else {
        message.payload.widget.data.value = ADVMGR_SYSTEM_OPTIONS_COMPUTER_HIDDEN_FRAME;
    }
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = IDX(SYSTEM_OPTION_INTERFACE);
    message.payload.widget.data.value =
        gConfig.evilInterfaceUsage + ADVMGR_SYSTEM_OPTIONS_INTERFACE_FRAME_BASE;
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = IDX(SYSTEM_OPTION_VIDEO);
    message.payload.widget.data.value =
        ADVMGR_SYSTEM_OPTIONS_VIDEO_FRAME_BASE + (gConfig.slowVideo != 0);
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = IDX(SYSTEM_OPTION_COLOR_CURSOR);
    message.payload.widget.data.value =
        gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].colorMouseCursor
        + ADVMGR_SYSTEM_OPTIONS_CURSOR_FRAME_BASE;
    cPanel->BroadcastMessage(message);

    message.payload.widget.command = ADVMGR_SYSTEM_OPTIONS_SET_TEXT;
    message.payload.widget.id =
        IDX(SYSTEM_OPTION_MUSIC_VOLUME) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text = onOffText[IDX(gConfig.musicVolume)];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id =
        IDX(SYSTEM_OPTION_SOUND_VOLUME) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text = onOffText[IDX(gConfig.soundVolume)];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id =
        IDX(SYSTEM_OPTION_HERO_SPEED) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text = walkSpeedText[IDX(gConfig.walkSpeed)];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id =
        IDX(SYSTEM_OPTION_MUSIC_SOURCE) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text = musicQualityText[IDX(musicQuality)];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id =
        IDX(SYSTEM_OPTION_SHOW_ROUTE) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text = onOffText[gConfig.showRoute];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id =
        IDX(SYSTEM_OPTION_COMPUTER_SPEED) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    if (gConfig.blackoutComputer == 0) {
        message.payload.widget.data.text = walkSpeedText[IDX(gConfig.computerWalkSpeed)];
    } else {
        message.payload.widget.data.text = DATA_COMPGEN(0x004f69dc, updateSystemOptionsDonTShow, "Don't Show");
    }
    cPanel->BroadcastMessage(message);
    message.payload.widget.id =
        IDX(SYSTEM_OPTION_INTERFACE) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text = gInterfaceTypeText[gConfig.evilInterfaceUsage];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id =
        IDX(SYSTEM_OPTION_VIDEO) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text = cSlowVideoLevelText[gConfig.slowVideo != 0];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id =
        IDX(SYSTEM_OPTION_COLOR_CURSOR) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text =
        cBWMouseText[gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].colorMouseCursor];
    cPanel->BroadcastMessage(message);

    if (initialDraw == 0) {
        cPanel->DrawWindow(1, 0, ADVMGR_SYSTEM_OPTIONS_DRAW_MASK);
    }
}

VA(0x0046b578, 0x672)
MessageDispatchResult SystemOptionsHandler(struct tag_message& message) {
    i32 preferencesChanged = 0;
    char textData[SYSTEM_OPTIONS_TEXT_CAPACITY];
    i32 accepted = 0;

    if (message.type == ADVMGR_SYSTEM_OPTIONS_MESSAGE) {
        if (HAS(
                message.payload.widget.modifiers,
                MESSAGE_MODIFIER_RIGHT_BUTTON
            )) {
            if (message.payload.widget.command == ADVMGR_SYSTEM_OPTIONS_ACTIVATE
                || message.payload.widget.command == ADVMGR_SYSTEM_OPTIONS_HOVER) {
                i32 helpIndex = OPTION_DIALOG_NONE;

                switch (static_cast<AdventureSystemOptionsWidgetId>(
                    message.payload.widget.id
                )) {
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
                    switch (static_cast<AdventureSystemOptionsWidgetId>(
                        message.payload.widget.id
                    )) {
                        case SYSTEM_OPTIONS_DIALOG_ACCEPT:
                            accepted = 1;
                            break;
                    }
                    break;

                case ADVMGR_SYSTEM_OPTIONS_ACTIVATE: {
                    switch (static_cast<AdventureSystemOptionsWidgetId>(
                        message.payload.widget.id
                    )) {
                        case SYSTEM_OPTION_MUSIC_VOLUME:
                            if (gConfig.musicVolume == CONFIG_VOLUME_MUTED && gpSoundManager->m_cdReady == 0
                                && gpSoundManager->m_midiReady == 0) {
                                NormalDialog(
                                    DATA_COMPGEN(0x004f69e8, systemOptionsHandlerNeitherMIDINorRedbookMusicIs, "Neither MIDI nor Redbook music is currently available on this "
                                    "system."),
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
                            if (gConfig.soundVolume == CONFIG_VOLUME_MUTED && gpSoundManager->m_digitalDriver == NULL) {
                                NormalDialog(
                                    DATA_COMPGEN(0x004f6a30, systemOptionsHandlerDigitalSoundIsNotCurrentlyAvailable, "Digital sound is not currently available on this system."),
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
                            } else if (gConfig.computerWalkSpeed
                                       < CONFIG_WALK_SPEED_INSTANT) {
                                ++gConfig.computerWalkSpeed;
                            } else {
                                gConfig.blackoutComputer = 1;
                            }
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case SYSTEM_OPTION_MUSIC_SOURCE:
                            if (gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI) {
                                if (gpSoundManager->m_cdStarted == 0) {
                                    gpSoundManager->CDStartup();
                                }
                                if (gpSoundManager->m_cdReady == 0) {
                                    NormalDialog(
                                        DATA_COMPGEN(0x004f6a6c, systemOptionsHandlerUnableToSetUpCDStereo, "Unable to set up CD stereo music.  Your CD player might "
                                        "be in use by another program, or your sound driver might "
                                        "not support CD stereo."),
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
                                gpSoundManager->SetMusicQuality(IDX(CONFIG_MUSIC_SOURCE_CD));
                                gConfig.useOpera = CONFIG_OPERA_DISABLED;
                            } else if (gConfig.useOpera == CONFIG_OPERA_DISABLED) {
                                gConfig.useOpera = CONFIG_OPERA_ENABLED;
                            } else {
                                if (gpSoundManager->m_midiStarted == 0) {
                                    gpSoundManager->MIDIStartup();
                                }
                                if (gpSoundManager->m_midiReady == 0) {
                                    gConfig.useOpera = static_cast<ConfigOperaMode>(
                                        1 - IDX(gConfig.useOpera)
                                    );
                                } else {
                                    gpSoundManager->SetMusicQuality(IDX(CONFIG_MUSIC_SOURCE_MIDI));
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
                            if (gbLowMemory) {
                                NormalDialog(
                                    DATA_COMPGEN(0x004f6af4, systemOptionsHandlerYouDonTHaveEnoughMemory, "You don't have enough memory for non-interlaced video."),
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
                            if (gConfig.slowVideo) {
                                gConfig.slowVideo = 0;
                            } else {
                                gConfig.slowVideo = 1;
                            }
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case SYSTEM_OPTION_COLOR_CURSOR:
                            gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].colorMouseCursor =
                                1 - gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].colorMouseCursor;
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            gpMouseManager->SetColorMice(
                                gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].colorMouseCursor
                            );
                            break;
#ifdef HOMM2_STRICT_ENUM_TYPES
                        case SYSTEM_OPTION_COUNT:
                            break;
#endif
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
        message.payload.widget.id = IDX(SYSTEM_OPTION_FIRST);
        message.payload.widget.command =
            static_cast<BaseWidgetCommand>(message.payload.widget.id);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0046bbea, 0x7f)
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

VA(0x0046bc69, 0x7f)
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

VA(0x0046bce8, 0x559)
i32 advManager::DoVisions(hero* visionHero) {
    char visionMessageResult[VISIONS_MESSAGE_BUFFER_SIZE];
    CreatureType creatureData;
    i32 nearestDistanceState;
    i32 nearestXId;
    i32 nearestYData;
    i32 scanXType;
    i32 scanYLocal;
    mapCell* cellData;
    i32 joiningCount;
    i32 monsterCountIndex;
    i32 currentDistanceId;
    i32 forcedJoinState;
    float strengthRatioCurrent;
    i32 joiningCostIndex;

    nearestDistanceState = VISIONS_NO_MONSTER_DISTANCE;
    nearestYData = -1;
    nearestXId = nearestYData;
    for (scanXType = visionHero->m_x - VISIONS_RADIUS;
         scanXType <= visionHero->m_x + VISIONS_RADIUS;
         ++scanXType) {
        for (scanYLocal = visionHero->m_y - VISIONS_RADIUS;
             scanYLocal <= visionHero->m_y + VISIONS_RADIUS;
             ++scanYLocal) {
            cellData = GetCell(scanXType, scanYLocal);
            if (cellData->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER)) {
                if (nearestDistanceState
                    > (currentDistanceId =
                           abs(visionHero->m_x - scanXType) + abs(visionHero->m_y - scanYLocal))) {
                    nearestDistanceState = currentDistanceId;
                    nearestXId = scanXType;
                    nearestYData = scanYLocal;
                }
            }
        }
    }

    if (nearestDistanceState == VISIONS_NO_MONSTER_DISTANCE) {
        NormalDialog(
            DATA_COMPGEN(0x004f6b2c, doVisionsYouMustBeWithin3Spaces, "You must be within 3 spaces of a monster for the Visions spell to work."),
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

    cellData = GetCell(nearestXId, nearestYData);
    creatureData = static_cast<CreatureType>(cellData->m_objectIndex);
    forcedJoinState = cellData->m_objectMetadata & MONSTER_JOIN_FORCED;
    monsterCountIndex = cellData->m_objectMetadata & MONSTER_COUNT_MASK;
    sprintf(gText, DATA_COMPGEN(0x004f6b74, doVisionsDS, "{%d %s}\n\n"), monsterCountIndex, gArmyNamesPlural[IDX(creatureData)]);
    strengthRatioCurrent =
        static_cast<float>(gpPhilAI->FightValueOfStack(&visionHero->m_army, visionHero, 0, 0, 0, 0))
        / static_cast<float>(gMonsterDatabase[IDX(creatureData)].fightValue * monsterCountIndex);

    if (visionHero->m_army.CanJoin(creatureData)
        && strengthRatioCurrent
            > DATA_COMPGEN(0x004eb6b8, monsterStrengthJoinThreshold, MONSTER_STRENGTH_JOIN)
        && !visionHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) && creatureData != CREATURE_GHOST
        && creatureData != CREATURE_EARTH_ELEMENTAL && creatureData != CREATURE_AIR_ELEMENTAL
        && creatureData != CREATURE_FIRE_ELEMENTAL && creatureData != CREATURE_WATER_ELEMENTAL) {
        if (forcedJoinState) {
            sprintf(visionMessageResult, DATA_COMPGEN(0x004f6b80, doVisionsTheCreaturesAreWillingToJoin, "The creatures are willing to join us!"));
            strcat(gText, visionMessageResult);
            goto showVision;
        } else if (visionHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                   != HERO_SKILL_LEVEL_NONE) {
            if (visionHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                == HERO_SKILL_LEVEL_EXPERT) {
                joiningCount = monsterCountIndex;
            } else if (visionHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                       == HERO_SKILL_LEVEL_ADVANCED) {
                joiningCount = monsterCountIndex / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
            } else {
                joiningCount = monsterCountIndex / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
            }
            if (joiningCount == 0) {
                joiningCount = 1;
            }

            joiningCostIndex = gMonsterDatabase[IDX(creatureData)].cost * monsterCountIndex
                * VISIONS_JOIN_COST_MULTIPLIER;
            if (joiningCostIndex
                > gpGame->m_players[visionHero->m_owner].m_resources[IDX(RES_GOLD)]) {
                if (strengthRatioCurrent
                    > DATA_COMPGEN(
                        0x004eb6c0, monsterStrengthFleeThreshold, MONSTER_STRENGTH_FLEE
                    )) {
                    goto creaturesFlee;
                } else {
                    goto creaturesFight;
                }
            }

            if (joiningCount == monsterCountIndex) {
                sprintf(
                    visionMessageResult,
                    DATA_COMPGEN(0x004f6ba8, doVisionsAllTheCreaturesWillJoinUs, "All the creatures will join us...\n\nfor a fee of %d gold."),
                    joiningCostIndex
                );
            } else {
                sprintf(
                    visionMessageResult,
                    DATA_COMPGEN(0x004f6be4, doVisionsDOfTheCreaturesWillJoin, "%d of the creatures will join us...\n\nfor a fee of %d gold."),
                    monsterCountIndex,
                    joiningCostIndex
                );
            }
            strcat(gText, visionMessageResult);
            goto showVision;
        }
    }

    if (strengthRatioCurrent > MONSTER_STRENGTH_FLEE) {
    creaturesFlee:
        sprintf(visionMessageResult, DATA_COMPGEN(0x004f6c20, doVisionsTheseWeakCreaturesWillSurelyFlee, "These weak creatures will surely flee before us."));
        strcat(gText, visionMessageResult);
        goto showVision;
    }
creaturesFight:
    sprintf(visionMessageResult, DATA_COMPGEN(0x004f6c54, doVisionsIFearTheseCreaturesAreIn, "I fear these creatures are in the mood for a fight."));
    strcat(gText, visionMessageResult);
    goto showVision;

showVision:
    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
    return 1;
}

VA(0x0046c241, 0xd7)
i32 advManager::IsCrystalBallInEffect(i32 x, i32 y, i32 radius) {
    i32 heroIndex;
    hero* crystalHero;
    i32 distance;
    for (heroIndex = 0; heroIndex < gpCurPlayer->m_heroCount; ++heroIndex) {
        crystalHero = gpGame->GetHero(gpCurPlayer->m_heroIds[heroIndex]);
        if (crystalHero->HasArtifact(ARTIFACT_CRYSTAL_BALL)) {
            distance = static_cast<i32>(sqrt(
                static_cast<double>(
                    (crystalHero->m_y - y) * (crystalHero->m_y - y)
                    + (crystalHero->m_x - x) * (crystalHero->m_x - x)
                )
            ));
            if (distance <= radius) {
                return 1;
            }
        }
    }
    return 0;
}

VA(0x0046c318, 0x85)
u8 StopOnTrigger(class mapCell* cell) {
    MapObjectType type = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
    if (type != MAP_OBJECT_EXPANSION_OBJECT) {
        return bStopOnTrigger[IDX(type)];
    }

    i32 trigger = cell->m_objectMetadata;
    trigger &= SPECIAL_TRIGGER_MASK;
    switch (trigger) {
        case TRIGGER_EVENT_5:
        case TRIGGER_EVENT_6:
            return 1;
    }
    return 0;
}


VTBL(advManager, 0x004eb6c8);

DATA(0x004f57b0) i32 giLimitUpdMinX = -1;
DATA(0x004f57b4) i32 iLastScrollTime = 0;
DATA(0x004f57b8) i32 iSandAnim = 0;
DATA(0x004f57bc) i32 giLastHourGlassUpdateTime = 0;
DATA(0x004f57c0) i32 TrigX = 0;
DATA(0x004f57c4) i32 TrigY = 0;
DATA(0x004f57c8) H2_ENUM_STORAGE(BottomViewMode, i32) iCurBottomView = BOTTOM_VIEW_NONE;
DATA(0x004f57cc) i32 iCurBottomViewEnemy = -1;
DATA(0x004f57d0) i32 iCurHourGlassPhase = 0;
DATA(0x004f57d4) i32 iLastHourGlassPhase = 1;
DATA(0x004f57d8) b32 gbForceUpdate = false;
DATA(0x004f59e8) i32 giCheatSeq = 0;
DATA(0x004f59ec) i32 iQWE = 0;
// Retail animation-phase payload for the adventure monster overlay.
// NOLINTNEXTLINE(readability-magic-numbers)
DATA(0x004f5e38) u8 monAnimDrawFrame[ADVMGR_MONSTER_ANIMATION_TABLE_SIZE] =
    {0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 3, 4, 5, 5, 4, 3, 0, 0};
DATA(0x004f60e0) i32 iLastSandAnimTime = 0;
DATA(0x004f60e4) i32 iLastNewSandAnimTime = 0;
DATA(0x004f6720) i32 giFrameCount = 0;
DATA(0x00527ec8) class heroWindow* cPanel;
DATA(0x00527ed0) i32 iThisMaxY;
DATA(0x00527edc) i32 giTownPortalChoice;
DATA(0x00527ee0) i32 iThisMinY;
DATA(0x00527ee8) class heroWindow* townPortalWin;
DATA(0x00527ef0) struct tag_message USMsg;
DATA(0x00527f14) i32 giFrameStep;
DATA(0x00527f28) char cArmySizeName[ADVMGR_ARMY_SIZE_NAME_SIZE];
DATA(0x00527f34) i32 giLimitUpdMaxX;
DATA(0x00527f38) i32 giLimitUpdMaxY;
DATA(0x00527f40) i32 bPrefsChanged;
DATA(0x00527f4c) i32 giLimitUpdMinY;
DATA(0x00527f50) i8 bComboDraw[COMBO_GRID_CELLS][COMBO_GRID_CELLS];
DATA(0x005280b0) struct tag_message CDMsg;
DATA(0x005280d4) i32 iLastAnimFrame;

#undef RETAIL_FILE
