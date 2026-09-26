#include <Ints.h>
#include <BASE/message.h>
#include <BASE/Misc.h>
#include <IRONFIST/hooks.h>
#include <IRONFIST/prefs.h>
#include <IRONFIST/townconsts.h>
#include <BASE/widgetKind.h>
#include <BASE/border.h>
#include <BASE/executive.h>
#include <BASE/font.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Utf8.h>
#include <BASE/icon.h>
#include <BASE/iconWidget.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <BASE/textWidget.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/Castle.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/GAME.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/RECRUIT.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/bankBox.h>
#include <SOURCE/game.h>
#include <SOURCE/HERO.h>
#include <SOURCE/hero.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/playerData.h>
#include <SOURCE/philAI.h>
#include <SOURCE/recruitUnit.h>
#include <SOURCE/strip.h>
#include <SOURCE/town.h>
#include <SOURCE/townObject.h>
#include <SOURCE/townManager.h>
#include <SOURCE/TOWNMGR.h>
#include <SOURCE/tradpost.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SOURCE/Localization.h>
#include <BASE/dialog.h>
#include <BASE/display.h>
#include <BASE/widget.h>
#include <SOURCE/KB_TYPES.h>


namespace {

    typedef enum TownDialogResult {
        DIALOG_CANCEL_ID = DIALOG_BUTTON_1,
    } TownDialogResult;

    typedef enum TownManagerInputCode {
        DIALOG_BUY_SPELL_BOOK = DIALOG_BUTTON_5,
        DIALOG_BUILD_BOAT = DIALOG_BUTTON_2,
        CONTROL_CLOSE = DIALOG_BUTTON_0,
        CONTROL_PREVIOUS_TOWN = 0x387,
        CONTROL_NEXT_TOWN = 0x388,
    } TownManagerInputCode;

    enum class TownManagerWidgetId : i32 {
        TOWN_WIDGET_NONE = TOWN_WIDGET_ID_NONE,
        TOWN_WIDGET_BUILDING_MAGE_GUILD = H2EnumIndex(TOWN_OBJECT_MAGE_GUILD),
        TOWN_WIDGET_BUILDING_THIEVES_GUILD = H2EnumIndex(TOWN_OBJECT_THIEVES_GUILD),
        TOWN_WIDGET_BUILDING_TAVERN = H2EnumIndex(TOWN_OBJECT_TAVERN),
        TOWN_WIDGET_BUILDING_DOCK = H2EnumIndex(TOWN_OBJECT_DOCK),
        TOWN_WIDGET_BUILDING_WELL = H2EnumIndex(TOWN_OBJECT_WELL),
        TOWN_WIDGET_BUILDING_TENT = H2EnumIndex(TOWN_OBJECT_TENT),
        TOWN_WIDGET_BUILDING_CASTLE_UPGRADE = H2EnumIndex(TOWN_OBJECT_CASTLE_UPGRADE),
        TOWN_WIDGET_BUILDING_CASTLE = H2EnumIndex(TOWN_OBJECT_CASTLE),
        TOWN_WIDGET_BUILDING_STATUE = H2EnumIndex(TOWN_OBJECT_STATUE),
        TOWN_WIDGET_BUILDING_LEFT_TURRET = H2EnumIndex(TOWN_OBJECT_LEFT_TURRET),
        TOWN_WIDGET_BUILDING_RIGHT_TURRET = H2EnumIndex(TOWN_OBJECT_RIGHT_TURRET),
        TOWN_WIDGET_BUILDING_MARKETPLACE = H2EnumIndex(TOWN_OBJECT_MARKETPLACE),
        TOWN_WIDGET_BUILDING_SECOND_WELL = H2EnumIndex(TOWN_OBJECT_SECOND_WELL),
        TOWN_WIDGET_BUILDING_MOAT = H2EnumIndex(TOWN_OBJECT_MOAT),
        TOWN_WIDGET_BUILDING_SPECIAL = H2EnumIndex(TOWN_OBJECT_SPECIAL_BUILDING),
        TOWN_WIDGET_BUILDING_BOAT = H2EnumIndex(TOWN_OBJECT_BOAT),
        TOWN_WIDGET_BUILDING_CAPTAIN_QUARTERS = H2EnumIndex(TOWN_OBJECT_CAPTAIN_QUARTERS),
        TOWN_WIDGET_BUILDING_DWELLING_1 = H2EnumIndex(TOWN_OBJECT_DWELLING_1),
        TOWN_WIDGET_BUILDING_DWELLING_2 = H2EnumIndex(TOWN_OBJECT_DWELLING_2),
        TOWN_WIDGET_BUILDING_DWELLING_3 = H2EnumIndex(TOWN_OBJECT_DWELLING_3),
        TOWN_WIDGET_BUILDING_DWELLING_4 = H2EnumIndex(TOWN_OBJECT_DWELLING_4),
        TOWN_WIDGET_BUILDING_DWELLING_5 = H2EnumIndex(TOWN_OBJECT_DWELLING_5),
        TOWN_WIDGET_BUILDING_DWELLING_6 = H2EnumIndex(TOWN_OBJECT_DWELLING_6),
        TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_2 = H2EnumIndex(TOWN_OBJECT_UPGRADED_DWELLING_2),
        TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_3 = H2EnumIndex(TOWN_OBJECT_UPGRADED_DWELLING_3),
        TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_4 = H2EnumIndex(TOWN_OBJECT_UPGRADED_DWELLING_4),
        TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_5 = H2EnumIndex(TOWN_OBJECT_UPGRADED_DWELLING_5),
        TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_6 = H2EnumIndex(TOWN_OBJECT_UPGRADED_DWELLING_6),
        TOWN_WIDGET_BUILDING_ALTERNATE_DWELLING_6 = H2EnumIndex(TOWN_OBJECT_ALTERNATE_UPGRADED_DWELLING_6),
        TOWN_WIDGET_GARRISON_CREST = TOWN_GARRISON_FIRST_CONTROL,
        TOWN_WIDGET_GARRISON_FIRST = TOWN_GARRISON_SLOT_FIRST,
        TOWN_WIDGET_GARRISON_SECOND = TOWN_WIDGET_GARRISON_FIRST + 1,
        TOWN_WIDGET_GARRISON_THIRD = TOWN_WIDGET_GARRISON_FIRST + 2,
        TOWN_WIDGET_GARRISON_FOURTH = TOWN_WIDGET_GARRISON_FIRST + 3,
        TOWN_WIDGET_GARRISON_LAST = TOWN_GARRISON_SLOT_LAST,
        TOWN_WIDGET_HERO_CONTROL = TOWN_HERO_FIRST_CONTROL,
        TOWN_WIDGET_HERO_FIRST = TOWN_HERO_SLOT_FIRST,
        TOWN_WIDGET_HERO_SECOND = TOWN_WIDGET_HERO_FIRST + 1,
        TOWN_WIDGET_HERO_THIRD = TOWN_WIDGET_HERO_FIRST + 2,
        TOWN_WIDGET_HERO_FOURTH = TOWN_WIDGET_HERO_FIRST + 3,
        TOWN_WIDGET_HERO_LAST = TOWN_HERO_SLOT_LAST,
        TOWN_WIDGET_EMPTY_FIRST = TOWN_EMPTY_STATUS_CONTROL_FIRST,
        TOWN_WIDGET_EMPTY_LAST = TOWN_EMPTY_STATUS_CONTROL_LAST,
        TOWN_WIDGET_CLOSE = CONTROL_CLOSE
    };
using enum TownManagerWidgetId;

    constexpr TownManagerWidgetId TownManagerWidgetIdFromCode(i32 value) {
        return static_cast<TownManagerWidgetId>(value); // H2_ENUM_CODE_BOUNDARY
    }
    ENABLE_ENUM_STEPS(TownManagerWidgetId)

    enum class TownObjectRenderMask : i32 {
        RENDER_SORCERESS_LEFT_OPTION = 0x800,
        RENDER_RACE_OVERLAY_FIRST_OPTION = 0x4000,
        RENDER_DOCK_GATE = 0x4000
    };
using enum TownObjectRenderMask;

    enum class TownCommandTextId : i32 {
        TEXT_REDISTRIBUTE_ARMY = 0,
        TEXT_CANNOT_COMBINE_LAST_ARMY = 1,
        TEXT_COMBINE_ARMIES = 2,
        TEXT_REDISTRIBUTE_TO_EMPTY_SLOT = 3,
        TEXT_VIEW_ARMY = 4,
        TEXT_CANNOT_MOVE_LAST_ARMY = 5,
        TEXT_MOVE_ARMY = 6,
        TEXT_EXCHANGE_ARMIES = 7,
        TEXT_EXIT = 8,
        TEXT_EMPTY_STATUS = 9,
        TEXT_KINGDOM_OVERVIEW = 10,
        TEXT_EMPTY_SLOT = 11,
        TEXT_SELECT_ARMY = 12,
        TEXT_VIEW_HERO = 13,
        TEXT_MAGE_GUILD = 14,
        TEXT_THIEVES_GUILD = 15,
        TEXT_TAVERN = 16,
        TEXT_DOCK = 17,
        TEXT_WELL = 18,
        TEXT_TENT = 19,
        TEXT_CASTLE = 20,
        TEXT_RECRUIT = 21,
        TEXT_STATUE = 22,
        TEXT_LEFT_TURRET = 23,
        TEXT_RIGHT_TURRET = 24,
        TEXT_MOAT = 25,
        TEXT_MARKETPLACE = 26,
        TEXT_CAPTAIN_QUARTERS = 27
    };
using enum TownCommandTextId;


    typedef enum TownAnimationConstant {
        NECROMANCER_BUILD_STATE_FRAME_STRIDE = 6,
        CREST_PORTRAITS_PER_COLOR = 4
    } TownAnimationConstant;

    typedef enum TownMainConstant {
        BUILDING_DESCRIPTION_CAPACITY = 400,
        TOWN_VIEW_FIZZLE_WIDTH = 552,
        TOWN_VIEW_FIZZLE_HEIGHT = 204,
        BUILDING_DIALOG_ICON_FRAME_BASE = 19,
        MAGE_GUILD_WINDOW_TEXT_ID = 17,
        THIEVES_GUILD_WINDOW_TEXT_ID = 14,
        SMALL_DIALOG_WINDOW_X = 177,
        SMALL_DIALOG_WINDOW_Y = 20,
        SHIP_WINDOW_TEXT_ID = 12,
        BOAT_LIMIT_DIALOG_X = 208,
        BOAT_LIMIT_DIALOG_Y = 40,
        MARKETPLACE_EFFICIENCY_MAX_INDEX = KB_TRADING_POST_EFFICIENCY_COUNT - 1,
        TOWN_REDRAW_FIRST_CONTROL = 136,
        TOWN_REDRAW_LAST_CONTROL = 137,
        TOWN_VIEWPORT_HEIGHT = TOWN_GARRISON_STRIP_Y
    } TownMainConstant;

    typedef enum TownSplitConstant {
        SPLIT_SETUP_AMOUNT_CONTROL = 4
    } TownSplitConstant;

    typedef enum BuildDialogConstant {
        BUILD_ROW_RESOURCE_CAPACITY = 4,
        BUILD_DESCRIPTION_WIDTH = 240,
        BUILD_WINDOW_BASE_Y = 151,
        BUILD_TEXT_LINE_SHIFT = 4,
        BUILD_SINGLE_RESOURCE_ROW_HEIGHT = 44,
        BUILD_DOUBLE_RESOURCE_ROW_HEIGHT = 88,
        BUILD_BUTTON_AREA_HEIGHT = 39,
        BUILD_WINDOW_ROW_BASE_HEIGHT = 69,
        BUILD_WINDOW_ROW_HEIGHT = 45,
        BUILD_WINDOW_MIN_ROWS = 3,
        BUILD_WINDOW_MAX_ROWS = 6,
        BUILD_WINDOW_X = 158,
        BUILD_WINDOW_Y = 16,
        BUILD_ICON_CONTROL = 2,
        BUILD_NAME_CONTROL = 3,
        BUILD_DESCRIPTION_X = 43,
        BUILD_DESCRIPTION_Y_OFFSET = 24,
        BUILD_RESOURCE_ROW_COUNT = 2,
        BUILD_RESOURCE_ROW_HEIGHT = 44,
        BUILD_TEXT_LINE_HEIGHT = 16,
        BUILD_RESOURCE_FIRST_Y_OFFSET = 12,
        BUILD_RESOURCE_AREA_WIDTH = 256,
        BUILD_RESOURCE_AREA_LEFT = 32,
        BUILD_AMOUNT_Y_OFFSET = 35,
        BUILD_RESOURCE_WIDGET_HEIGHT = 12,
        BUILD_AMOUNT_TEXT_CAPACITY = 10,
        NECROMANCER_PREREQUISITE_MAX_MAGE_LEVEL = 2
    } BuildDialogConstant;

    typedef enum MageGuildConstant {
        MAGE_SPELL_NAME_WIDTH = 74
    } MageGuildConstant;

    typedef enum RecruitDialogConstant {
        RECRUIT_WINDOW_X = 177,
        RECRUIT_WINDOW_Y = 16,
        RECRUIT_WINDOW_TEXT_ID = 20,
        RECRUIT_DESCRIPTION_CONTROL = 1,
        RECRUIT_PORTRAIT_CONTROL = 2,
        RECRUIT_BUTTON_TEXT_CONTROL = 8,
        RECRUIT_BUTTON_ICON_CONTROL = 9,
        HEROES_PER_FACTION = H2EnumIndex(GAME_HERO_COUNT) / H2EnumIndex(FACTION_COUNT),
        TAVERN_WINDOW_X = 162,
        TAVERN_WINDOW_Y = 10,
        TAVERN_WINDOW_TEXT_ID = 22,
        TAVERN_ANIMATION_CONTROL = 2
    } RecruitDialogConstant;

    typedef enum WellConstant {
        WELL_DETAIL_TEXT_CAPACITY = 40,
        WELL_ALTERNATE_UPGRADE_INDEX = TOWN_WELL_DWELLING_COUNT * 2 - 1
    } WellConstant;

    typedef enum WellDetailText {
        WELL_DETAIL_ATTACK = 0,
        WELL_DETAIL_DEFENSE = 1,
        WELL_DETAIL_DAMAGE = 3,
        WELL_DETAIL_HIT_POINTS = 4,
        WELL_DETAIL_SPEED = 7,
        WELL_DETAIL_GROWTH = 8
    } WellDetailText;

    typedef enum ThievesGuildConstant {
        THIEVES_STAT_TEXT_CAPACITY = 200,
        THIEVES_PLAYER_COLUMN_WIDTH = 68,
        THIEVES_RANK_FIRST_X = 258,
        THIEVES_TIE_CENTERING_STEP = 9,
        THIEVES_RANK_ICON_WIDTH = 18,
        THIEVES_CATEGORY_ROW_HEIGHT = 24,
        THIEVES_FIRST_CATEGORY_Y = 27,
        THIEVES_RANK_ICON_HEIGHT = 22,
        THIEVES_HERO_Y = 300,
        THIEVES_PRIMARY_STATS_Y = 339,
        THIEVES_PERSONALITY_Y = 397,
        THIEVES_PERSONALITY_TEXT_Y = 393,
        THIEVES_CREATURE_Y = 418,
        THIEVES_HERO_LOCATOR_X = 246,
        THIEVES_HERO_LOCATOR_Y = 301,
        THIEVES_HERO_LOCATOR_FRAME = 22,
        THIEVES_HERO_PORTRAIT_X = 237,
        THIEVES_PRIMARY_LABEL_X = 239,
        THIEVES_PRIMARY_LABEL_WIDTH = 40,
        THIEVES_PRIMARY_VALUE_X = 284,
        THIEVES_PRIMARY_VALUE_WIDTH = 15,
        THIEVES_PRIMARY_WIDGET_HEIGHT = 48,
        THIEVES_PERSONALITY_X = 227,
        THIEVES_PERSONALITY_WIDTH = 74,
        THIEVES_PERSONALITY_HEIGHT = 28,
        THIEVES_CREATURE_X = 244,
        THIEVES_CREATURE_WIDTH = 40,
        THIEVES_CREATURE_HEIGHT = 34
    } ThievesGuildConstant;

}


// clang-format off
#define TOWN_OBJECT_ORDER_EMPTY_ROW                                       \
    {TOWN_OBJECT_NONE, TOWN_OBJECT_NONE, TOWN_OBJECT_NONE,                \
     TOWN_OBJECT_NONE, TOWN_OBJECT_NONE, TOWN_OBJECT_NONE,                \
     TOWN_OBJECT_NONE, TOWN_OBJECT_NONE, TOWN_OBJECT_NONE,                \
     TOWN_OBJECT_NONE, TOWN_OBJECT_NONE, TOWN_OBJECT_NONE,                \
     TOWN_OBJECT_NONE, TOWN_OBJECT_NONE, TOWN_OBJECT_NONE,                \
     TOWN_OBJECT_NONE, TOWN_OBJECT_NONE, TOWN_OBJECT_NONE,                \
     TOWN_OBJECT_NONE, TOWN_OBJECT_NONE, TOWN_OBJECT_NONE,                \
     TOWN_OBJECT_NONE, TOWN_OBJECT_NONE, TOWN_OBJECT_NONE,                \
     TOWN_OBJECT_NONE, TOWN_OBJECT_NONE, TOWN_OBJECT_NONE,                \
     TOWN_OBJECT_NONE, TOWN_OBJECT_NONE, TOWN_OBJECT_NONE,                \
     TOWN_OBJECT_NONE, TOWN_OBJECT_NONE}
// clang-format on

static const H2EnumStorage<BuildingSlotType, i8>
    gTownObjectOrder[KB_FACTION_TABLE_CAPACITY][H2EnumIndex(BUILDING_SLOT_COUNT)] = {
    {TOWN_OBJECT_SECOND_WELL,
     TOWN_OBJECT_CASTLE_UPGRADE,
     TOWN_OBJECT_CASTLE,
     TOWN_OBJECT_SPECIAL_BUILDING,
     TOWN_OBJECT_LEFT_TURRET,
     TOWN_OBJECT_RIGHT_TURRET,
     TOWN_OBJECT_CAPTAIN_QUARTERS,
     TOWN_OBJECT_MOAT,
     TOWN_OBJECT_MARKETPLACE,
     TOWN_OBJECT_THIEVES_GUILD,
     TOWN_OBJECT_DWELLING_2,
     TOWN_OBJECT_UPGRADED_DWELLING_2,
     TOWN_OBJECT_DWELLING_5,
     TOWN_OBJECT_UPGRADED_DWELLING_5,
     TOWN_OBJECT_TAVERN,
     TOWN_OBJECT_MAGE_GUILD,
     TOWN_OBJECT_DWELLING_6,
     TOWN_OBJECT_UPGRADED_DWELLING_6,
     TOWN_OBJECT_KNIGHT_LEFT_OVERLAY,
     TOWN_OBJECT_KNIGHT_RIGHT_OVERLAY,
     TOWN_OBJECT_RACE_OVERLAY,
     TOWN_OBJECT_DOCK,
     TOWN_OBJECT_BOAT,
     TOWN_OBJECT_DWELLING_1,
     TOWN_OBJECT_DWELLING_3,
     TOWN_OBJECT_UPGRADED_DWELLING_3,
     TOWN_OBJECT_DWELLING_4,
     TOWN_OBJECT_UPGRADED_DWELLING_4,
     TOWN_OBJECT_STATUE,
     TOWN_OBJECT_WELL,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE},
    {TOWN_OBJECT_RACE_OVERLAY,
     TOWN_OBJECT_KNIGHT_LEFT_OVERLAY,
     TOWN_OBJECT_SPECIAL_BUILDING,
     TOWN_OBJECT_SECOND_WELL,
     TOWN_OBJECT_DWELLING_6,
     TOWN_OBJECT_THIEVES_GUILD,
     TOWN_OBJECT_CAPTAIN_QUARTERS,
     TOWN_OBJECT_DWELLING_5,
     TOWN_OBJECT_UPGRADED_DWELLING_5,
     TOWN_OBJECT_CASTLE_UPGRADE,
     TOWN_OBJECT_CASTLE,
     TOWN_OBJECT_MOAT,
     TOWN_OBJECT_MARKETPLACE,
     TOWN_OBJECT_DWELLING_2,
     TOWN_OBJECT_UPGRADED_DWELLING_2,
     TOWN_OBJECT_DWELLING_3,
     TOWN_OBJECT_BARBARIAN_OVERLAY,
     TOWN_OBJECT_DWELLING_1,
     TOWN_OBJECT_DWELLING_4,
     TOWN_OBJECT_UPGRADED_DWELLING_4,
     TOWN_OBJECT_MAGE_GUILD,
     TOWN_OBJECT_KNIGHT_RIGHT_OVERLAY,
     TOWN_OBJECT_TAVERN,
     TOWN_OBJECT_DOCK,
     TOWN_OBJECT_BOAT,
     TOWN_OBJECT_WELL,
     TOWN_OBJECT_LEFT_TURRET,
     TOWN_OBJECT_RIGHT_TURRET,
     TOWN_OBJECT_STATUE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE},
    {TOWN_OBJECT_SPECIAL_BUILDING,
     TOWN_OBJECT_DWELLING_6,
     TOWN_OBJECT_DWELLING_5,
     TOWN_OBJECT_CASTLE_UPGRADE,
     TOWN_OBJECT_CASTLE,
     TOWN_OBJECT_LEFT_TURRET,
     TOWN_OBJECT_RIGHT_TURRET,
     TOWN_OBJECT_MOAT,
     TOWN_OBJECT_CAPTAIN_QUARTERS,
     TOWN_OBJECT_DWELLING_3,
     TOWN_OBJECT_UPGRADED_DWELLING_3,
     TOWN_OBJECT_DWELLING_1,
     TOWN_OBJECT_MAGE_GUILD,
     TOWN_OBJECT_DOCK,
     TOWN_OBJECT_BOAT,
     TOWN_OBJECT_DWELLING_4,
     TOWN_OBJECT_UPGRADED_DWELLING_4,
     TOWN_OBJECT_WELL,
     TOWN_OBJECT_MARKETPLACE,
     TOWN_OBJECT_DWELLING_2,
     TOWN_OBJECT_UPGRADED_DWELLING_2,
     TOWN_OBJECT_THIEVES_GUILD,
     TOWN_OBJECT_TAVERN,
     TOWN_OBJECT_KNIGHT_LEFT_OVERLAY,
     TOWN_OBJECT_SECOND_WELL,
     TOWN_OBJECT_STATUE,
     TOWN_OBJECT_RACE_OVERLAY,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE},
    {TOWN_OBJECT_DWELLING_5,
     TOWN_OBJECT_DWELLING_3,
     TOWN_OBJECT_CASTLE_UPGRADE,
     TOWN_OBJECT_CASTLE,
     TOWN_OBJECT_LEFT_TURRET,
     TOWN_OBJECT_RIGHT_TURRET,
     TOWN_OBJECT_CAPTAIN_QUARTERS,
     TOWN_OBJECT_RACE_OVERLAY,
     TOWN_OBJECT_MOAT,
     TOWN_OBJECT_TAVERN,
     TOWN_OBJECT_THIEVES_GUILD,
     TOWN_OBJECT_MAGE_GUILD,
     TOWN_OBJECT_MARKETPLACE,
     TOWN_OBJECT_STATUE,
     TOWN_OBJECT_DOCK,
     TOWN_OBJECT_BOAT,
     TOWN_OBJECT_SECOND_WELL,
     TOWN_OBJECT_DWELLING_1,
     TOWN_OBJECT_SPECIAL_BUILDING,
     TOWN_OBJECT_DWELLING_4,
     TOWN_OBJECT_UPGRADED_DWELLING_4,
     TOWN_OBJECT_DWELLING_2,
     TOWN_OBJECT_UPGRADED_DWELLING_6,
     TOWN_OBJECT_ALTERNATE_UPGRADED_DWELLING_6,
     TOWN_OBJECT_DWELLING_6,
     TOWN_OBJECT_WELL,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE},
    {TOWN_OBJECT_UPGRADED_DWELLING_6,
     TOWN_OBJECT_DWELLING_6,
     TOWN_OBJECT_THIEVES_GUILD,
     TOWN_OBJECT_CAPTAIN_QUARTERS,
     TOWN_OBJECT_RACE_OVERLAY,
     TOWN_OBJECT_DWELLING_5,
     TOWN_OBJECT_UPGRADED_DWELLING_5,
     TOWN_OBJECT_CASTLE_UPGRADE,
     TOWN_OBJECT_CASTLE,
     TOWN_OBJECT_MOAT,
     TOWN_OBJECT_DWELLING_3,
     TOWN_OBJECT_UPGRADED_DWELLING_3,
     TOWN_OBJECT_DWELLING_1,
     TOWN_OBJECT_DWELLING_4,
     TOWN_OBJECT_MAGE_GUILD,
     TOWN_OBJECT_TAVERN,
     TOWN_OBJECT_DOCK,
     TOWN_OBJECT_BOAT,
     TOWN_OBJECT_WELL,
     TOWN_OBJECT_DWELLING_2,
     TOWN_OBJECT_SPECIAL_BUILDING,
     TOWN_OBJECT_MARKETPLACE,
     TOWN_OBJECT_LEFT_TURRET,
     TOWN_OBJECT_RIGHT_TURRET,
     TOWN_OBJECT_STATUE,
     TOWN_OBJECT_SECOND_WELL,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE},
    {TOWN_OBJECT_SPECIAL_BUILDING,
     TOWN_OBJECT_TAVERN,
     TOWN_OBJECT_CASTLE,
     TOWN_OBJECT_LEFT_TURRET,
     TOWN_OBJECT_RIGHT_TURRET,
     TOWN_OBJECT_MOAT,
     TOWN_OBJECT_CAPTAIN_QUARTERS,
     TOWN_OBJECT_THIEVES_GUILD,
     TOWN_OBJECT_DWELLING_6,
     TOWN_OBJECT_DWELLING_1,
     TOWN_OBJECT_DWELLING_3,
     TOWN_OBJECT_UPGRADED_DWELLING_3,
     TOWN_OBJECT_MAGE_GUILD,
     TOWN_OBJECT_RACE_OVERLAY,
     TOWN_OBJECT_DOCK,
     TOWN_OBJECT_BOAT,
     TOWN_OBJECT_DWELLING_5,
     TOWN_OBJECT_UPGRADED_DWELLING_5,
     TOWN_OBJECT_DWELLING_2,
     TOWN_OBJECT_UPGRADED_DWELLING_2,
     TOWN_OBJECT_DWELLING_4,
     TOWN_OBJECT_UPGRADED_DWELLING_4,
     TOWN_OBJECT_SECOND_WELL,
     TOWN_OBJECT_CASTLE_UPGRADE,
     TOWN_OBJECT_WELL,
     TOWN_OBJECT_MARKETPLACE,
     TOWN_OBJECT_STATUE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE,
     TOWN_OBJECT_NONE},
    TOWN_OBJECT_ORDER_EMPTY_ROW,
    TOWN_OBJECT_ORDER_EMPTY_ROW,
    TOWN_OBJECT_ORDER_EMPTY_ROW,
    TOWN_OBJECT_ORDER_EMPTY_ROW,
    TOWN_OBJECT_ORDER_EMPTY_ROW,
    TOWN_OBJECT_ORDER_EMPTY_ROW,
    // The Cyborg town draws every building; order from Ironfist's
    // BuildingsToDraw (the ext slots have no faction alias here).
    {TOWN_OBJECT_MAGE_GUILD,
     TOWN_OBJECT_CASTLE_UPGRADE,
     TOWN_OBJECT_CASTLE,
     TOWN_OBJECT_WELL,
     TOWN_OBJECT_STATUE,
     TOWN_OBJECT_LEFT_TURRET,
     TOWN_OBJECT_RIGHT_TURRET,
     TOWN_OBJECT_SECOND_WELL,
     TOWN_OBJECT_MOAT,
     TOWN_OBJECT_SPECIAL_BUILDING,
     TOWN_OBJECT_CAPTAIN_QUARTERS,
     BUILDING_SLOT_DISABLED_THIRD,
     BUILDING_SLOT_DISABLED_SECOND,
     BUILDING_SLOT_DISABLED_FOURTH,
     TOWN_OBJECT_DOCK,
     TOWN_OBJECT_BOAT,
     TOWN_OBJECT_DWELLING_1,
     TOWN_OBJECT_DWELLING_2,
     TOWN_OBJECT_MARKETPLACE,
     TOWN_OBJECT_TAVERN,
     TOWN_OBJECT_DWELLING_3,
     TOWN_OBJECT_DWELLING_4,
     TOWN_OBJECT_DWELLING_5,
     TOWN_OBJECT_DWELLING_6,
     TOWN_OBJECT_UPGRADED_DWELLING_2,
     TOWN_OBJECT_UPGRADED_DWELLING_3,
     TOWN_OBJECT_UPGRADED_DWELLING_4,
     TOWN_OBJECT_UPGRADED_DWELLING_5,
     TOWN_OBJECT_UPGRADED_DWELLING_6,
     TOWN_OBJECT_ALTERNATE_UPGRADED_DWELLING_6,
     TOWN_OBJECT_THIEVES_GUILD,
     BUILDING_SLOT_DISABLED_LAST}
};

SBuildingInfo sBuildingInfo[KB_FACTION_TABLE_CAPACITY][H2EnumIndex(BUILDING_SLOT_COUNT)] = {
    {
     {0, 397, 46, 84, 138},  {5, 0, 130, 53, 63},    {5, 345, 114, 83, 62},  {5, 531, 214, 113, 42},
     {0, 188, 214, 39, 42},  {0, 69, 108, 67, 55},   {5, 0, 49, 286, 116},   {0, 478, 193, 46, 63},
     {5, 7, 33, 0, 0},       {5, 134, 37, 0, 0},     {0, 219, 138, 120, 30}, {0, 286, 102, 88, 22},
     {0, 0, 146, 311, 30},   {0, 0, 78, 251, 22},    {9, 531, 211, 113, 45}, {0, 293, 107, 59, 35},
     {5, 0, 0, 0, 0},        {0, 0, 0, 0, 0},        {0, 0, 0, 0, 0},        {5, 192, 163, 69, 52},
     {0, 135, 149, 73, 32},  {5, 240, 166, 91, 66},  {0, 323, 174, 102, 69}, {7, 48, 176, 104, 80},
     {0, 445, 50, 195, 157}, {0, 135, 149, 73, 32},  {5, 240, 166, 91, 66},  {0, 323, 174, 102, 69},
     {7, 48, 176, 104, 80},  {0, 445, 50, 195, 157}, {0, 0, 0, 0, 0},        {0, 0, 0, 0, 0}},
    {{8, 346, 22, 54, 120},  {0, 466, 94, 87, 47},  {0, 0, 161, 136, 85},   {5, 505, 199, 138, 56},
     {0, 268, 189, 50, 66},  {0, 44, 109, 87, 52},  {6, 0, 0, 214, 175},    {0, 463, 154, 38, 81},
     {0, 10, 58, 0, 0},      {0, 118, 45, 0, 0},    {0, 217, 166, 67, 43},  {0, 240, 106, 73, 34},
     {5, 115, 138, 182, 42}, {0, 210, 80, 197, 61}, {9, 505, 199, 138, 56}, {5, 206, 99, 46, 42},
     {5, 0, 0, 0, 0},        {5, 0, 0, 0, 0},       {0, 0, 0, 0, 0},        {0, 290, 138, 58, 45},
     {0, 145, 195, 76, 52},  {0, 557, 48, 83, 83},  {5, 496, 136, 138, 64}, {5, 318, 174, 131, 54},
     {5, 407, 0, 113, 106},  {0, 145, 195, 76, 52}, {0, 0, 0, 0, 0},        {5, 496, 136, 138, 64},
     {5, 318, 174, 131, 54}, {0, 0, 0, 0, 0},       {0, 0, 0, 0, 0},        {0, 0, 0, 0, 0}},
    {{0, 279, 0, 63, 168},  {5, 423, 167, 87, 50}, {5, 490, 141, 148, 91}, {5, 0, 208, 178, 48},
     {0, 335, 205, 45, 29}, {0, 104, 130, 59, 42}, {5, 0, 0, 201, 179},    {0, 152, 163, 28, 65},
     {0, 98, 99, 0, 0},     {0, 151, 98, 0, 0},    {0, 404, 122, 69, 45},  {5, 131, 185, 71, 53},
     {0, 0, 171, 272, 23},  {0, 152, 0, 236, 84},  {9, 0, 208, 178, 48},   {5, 223, 122, 37, 52},
     {5, 0, 0, 0, 0},       {5, 0, 0, 0, 0},       {0, 0, 0, 0, 0},        {5, 472, 59, 111, 92},
     {5, 338, 146, 93, 61}, {0, 51, 164, 106, 40}, {0, 198, 178, 143, 71}, {0, 263, 226, 296, 30},
     {0, 179, 0, 84, 119},  {5, 338, 146, 93, 61}, {0, 51, 164, 106, 40},  {0, 198, 178, 143, 71},
     {0, 0, 0, 0, 0},       {0, 0, 0, 0, 0},       {0, 0, 0, 0, 0},        {0, 0, 0, 0, 0}},
    {{0, 586, 18, 54, 150},  {0, 520, 103, 64, 54}, {0, 476, 96, 82, 55},   {5, 517, 200, 123, 56},
     {0, 342, 205, 67, 51},  {0, 298, 135, 72, 31}, {5, 241, 18, 181, 150}, {0, 478, 161, 37, 63},
     {0, 311, 84, 0, 0},     {0, 359, 83, 0, 0},    {0, 386, 171, 71, 40},  {6, 60, 32, 63, 186},
     {5, 211, 166, 301, 21}, {0, 0, 160, 59, 96},   {9, 517, 200, 123, 56}, {5, 418, 83, 53, 84},
     {5, 0, 0, 0, 0},        {0, 0, 0, 0, 0},       {0, 0, 0, 0, 0},        {6, 0, 64, 48, 50},
     {0, 237, 168, 78, 87},  {0, 492, 50, 53, 39},  {0, 139, 163, 190, 83}, {0, 82, 92, 178, 68},
     {0, 92, 0, 64, 257},    {0, 0, 0, 0, 0},       {0, 0, 0, 0, 0},        {0, 139, 163, 190, 83},
     {0, 0, 0, 0, 0},        {0, 92, 0, 64, 257},   {0, 92, 0, 64, 257},    {0, 0, 0, 0, 0}},
    {{0, 570, 0, 70, 126},  {5, 505, 50, 51, 49},  {6, 0, 149, 118, 76},   {5, 0, 206, 206, 50},
     {0, 249, 139, 28, 33}, {0, 58, 60, 49, 42},   {5, 0, 0, 200, 99},     {0, 464, 45, 24, 72},
     {0, 30, 17, 0, 0},     {0, 128, 17, 0, 0},    {0, 255, 163, 108, 53}, {0, 237, 208, 137, 49},
     {0, 0, 90, 223, 14},   {0, 297, 95, 109, 78}, {9, 0, 206, 206, 50},   {0, 210, 52, 28, 35},
     {5, 0, 0, 0, 0},       {0, 0, 0, 0, 0},       {0, 0, 0, 0, 0},        {5, 467, 181, 38, 30},
     {0, 231, 68, 192, 36}, {5, 152, 130, 96, 60}, {0, 593, 184, 51, 31},  {0, 411, 0, 49, 167},
     {0, 160, 0, 178, 67},  {0, 0, 0, 0, 0},       {5, 152, 130, 96, 60},  {0, 0, 0, 0, 0},
     {0, 411, 0, 49, 167},  {0, 160, 0, 178, 67},  {0, 0, 0, 0, 0},        {0, 0, 0, 0, 0}},
    {{5, 557, 17, 85, 191},  {0, 275, 124, 62, 77},  {0, 455, 39, 51, 103},  {5, 500, 220, 141, 36},
     {0, 215, 213, 29, 41},  {0, 333, 115, 47, 70},  {5, 289, 10, 134, 164}, {0, 365, 154, 41, 93},
     {0, 330, 47, 0, 0},     {0, 360, 46, 0, 0},     {0, 412, 193, 98, 61},  {6, 263, 181, 90, 65},
     {0, 258, 171, 193, 19}, {0, 0, 0, 640, 63},     {9, 500, 220, 141, 36}, {0, 441, 77, 22, 99},
     {5, 0, 0, 0, 0},        {0, 0, 0, 0, 0},        {0, 0, 0, 0, 0},        {0, 396, 177, 71, 35},
     {0, 110, 174, 141, 45}, {5, 0, 28, 241, 142},   {0, 20, 107, 124, 129}, {0, 221, 127, 66, 84},
     {6, 464, 72, 105, 124}, {0, 110, 174, 141, 45}, {5, 0, 28, 241, 142},   {0, 0, 107, 144, 129},
     {0, 223, 45, 65, 166},  {0, 0, 0, 0, 0},        {0, 0, 0, 0, 0},        {0, 0, 0, 0, 0}
    },
    {},
    {},
    {},
    {},
    {},
    {},
    {
     {0, 175, 20, 30, 105},  {0, 5, 65, 85, 65},     {0, 560, 130, 83, 52},  {5, 60, 190, 106, 60},
     {0, 316, 145, 32, 42},  {0, 304, 106, 51, 40},  {6, 216, 3, 236, 130},  {0, 380, 172, 40, 80},
     {0, 7, 33, 0, 0},       {0, 134, 37, 0, 0},     {0, 517, 102, 80, 50},  {0, 147, 120, 63, 40},
     {8, 207, 107, 254, 30}, {7, 200, 7, 105, 58},   {5, 0, 191, 120, 65},   {0, 220, 110, 59, 45},
     {4, 0, 0, 0, 0},        {4, 0, 0, 0, 0},        {0, 0, 0, 0, 0},        {5, 230, 165, 90, 60},
     {0, 427, 90, 90, 60},   {0, 365, 126, 90, 72},  {5, 495, 184, 147, 75}, {5, 75, 45, 100, 50},
     {4, 547, 10, 48, 84},   {0, 135, 149, 73, 32},  {5, 240, 166, 91, 66},  {0, 323, 174, 102, 69},
     {7, 48, 176, 104, 80},  {0, 445, 50, 195, 157}, {0, 0, 0, 0, 0},        {0, 0, 0, 0, 0}
    }
};

townObject::townObject(
    FactionType townType,
    BuildingSlotType buildingId,
    char* iconBaseName
) {
    char name[TOWN_OBJECT_FILENAME_SIZE];
    i32 x;
    i32 y;

    i32 currentWidth;
    i32 currentHeight;
    H2EnumStorage<BuildingSlotType, i32> objectBuildingId;

    m_animationFrame = 0;
    m_icon = NULL;
    m_border = NULL;
    m_visible = 1;
    m_animationFrameCount = sBuildingInfo[H2EnumIndex(townType)][H2EnumIndex(buildingId)].animationFrameCount;
    x = sBuildingInfo[H2EnumIndex(townType)][H2EnumIndex(buildingId)].x;
    y = sBuildingInfo[H2EnumIndex(townType)][H2EnumIndex(buildingId)].y;
    currentWidth = sBuildingInfo[H2EnumIndex(townType)][H2EnumIndex(buildingId)].width;
    currentHeight = sBuildingInfo[H2EnumIndex(townType)][H2EnumIndex(buildingId)].height;
    objectBuildingId = buildingId;
    m_buildingId = objectBuildingId;
    utf8::Format(name, "%s.icn", iconBaseName);
    m_icon = gpResourceManager->GetIcon(name);
    if (objectBuildingId != TOWN_OBJECT_NONE) {
        m_border = new border(
            x,
            y,
            currentWidth,
            currentHeight,
            H2EnumIndex(objectBuildingId),
            WIDGET_KIND_TRANSPARENT,
            0,
            NULL
        );
        if (m_border == NULL)
            MemError();
    }
}

townObject::~townObject() {
    if (m_border != NULL)
        delete m_border;
    gpResourceManager->Dispose(m_icon);
}

void townObject::Draw(i32 advanceAnimation) {
    i32 baseFrame;

    if (m_visible == 0)
        return;
    if (m_buildingId == TOWN_OBJECT_CASTLE_UPGRADE)
        return;
    if (m_buildingId == TOWN_OBJECT_KNIGHT_LEFT_OVERLAY
        && gpTownManager->m_town->m_type == FACTION_KNIGHT
        && (!(gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_TAVERN))
            || (!(gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_MAGE_GUILD))
                && !(gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_DWELLING_6))
                && !(gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_UPGRADED_DWELLING_6)))))
        return;
    if (m_buildingId == TOWN_OBJECT_KNIGHT_RIGHT_OVERLAY
        && gpTownManager->m_town->m_type == FACTION_KNIGHT
        && (!(gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_MAGE_GUILD))
            || (!(gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_DWELLING_6))
                && !(gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_UPGRADED_DWELLING_6)))))
        return;
    if (m_buildingId == TOWN_OBJECT_KNIGHT_RIGHT_OVERLAY
        && gpTownManager->m_town->m_type == FACTION_BARBARIAN
        && (!(gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_MAGE_GUILD))
            || !(gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_COLISEUM))))
        return;
    if (m_buildingId == TOWN_OBJECT_BARBARIAN_OVERLAY
        && gpTownManager->m_town->m_type == FACTION_BARBARIAN
        && (!(gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_THIEVES_GUILD))
            || !(gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_DWELLING_3))))
        return;
    if (gpTownManager->m_town->m_type == FACTION_SORCERESS
        && m_buildingId == TOWN_OBJECT_KNIGHT_LEFT_OVERLAY
        && (!(gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_CAPTAIN))
            || !(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_SORCERESS_LEFT_OPTION))))
        return;
    if (gpTownManager->m_town->m_type == FACTION_SORCERESS
        && (m_buildingId == TOWN_OBJECT_SORCERESS_LEFT_OVERLAY
            || m_buildingId == TOWN_OBJECT_SORCERESS_RIGHT_OVERLAY)
        && (gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_CAPTAIN))
        && (gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_SORCERESS_LEFT_OPTION)))
        return;
    if (m_buildingId == TOWN_OBJECT_RACE_OVERLAY
        && (gpTownManager->m_town->m_type == FACTION_NECROMANCER
            || gpTownManager->m_town->m_type == FACTION_WARLOCK
            || gpTownManager->m_town->m_type == FACTION_SORCERESS
            || gpTownManager->m_town->m_type == FACTION_KNIGHT)
        && ((gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_RACE_OVERLAY_FIRST_OPTION))
            || (gpTownManager->m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_DOCK))))
        return;
    if (m_buildingId == TOWN_OBJECT_DOCK
        && (gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_DOCK_GATE)))
        return;

    if (m_buildingId == TOWN_OBJECT_PRIMARY_ANIMATION) {
        if (gpTownManager->m_town->m_type == FACTION_NECROMANCER) {
            baseFrame =
                NECROMANCER_BUILD_STATE_FRAME_STRIDE * (gpTownManager->m_town->m_buildState - 1);
        } else {
            baseFrame = gpTownManager->m_town->m_buildState - 1;
        }
        m_icon->DrawToBuffer(0, 0, baseFrame, ICON_DRAW_NORMAL);
        if (m_animationFrameCount != 0) {
            if (gpTownManager->m_town->m_type == FACTION_BARBARIAN
                && gpTownManager->m_town->m_buildState < TOWN_BARBARIAN_ANIMATION_BUILD_STATE)
                return;
            m_icon->DrawToBuffer(0, 0, baseFrame + m_animationFrame + 1, ICON_DRAW_NORMAL);
            if (advanceAnimation == 1) {
                ++m_animationFrame;
                if (m_animationFrame == m_animationFrameCount)
                    m_animationFrame = 0;
            }
        }
        return;
    }

    m_icon->DrawToBuffer(0, 0, 0, ICON_DRAW_NORMAL);
    if (m_animationFrameCount != 0) {
        m_icon->DrawToBuffer(0, 0, m_animationFrame + 1, ICON_DRAW_NORMAL);
        if (advanceAnimation == 1) {
            ++m_animationFrame;
            if (m_animationFrame == m_animationFrameCount)
                m_animationFrame = 0;
        }
    }
}

townManager::townManager(void) {
    m_town = NULL;
    m_heroWindow0 = NULL;
    m_unknownC6 = 0;
    m_selectedBuilding = BUILDING_SLOT_NONE;
    m_castleDialogActive = false;
}

void townManager::SetupExtraStuff(void) {
    m_town->m_buildings &= H2EnumIndex(TOWN_EXTRA_DYNAMIC_CLEAR_MASK);
    if (m_town->m_type == FACTION_WIZARD)
        m_town->m_buildings |= H2EnumIndex(TOWN_EXTRA_RACE_FIRST_MASK);
    if (m_town->m_type == FACTION_SORCERESS) {
        m_town->m_buildings |= H2EnumIndex(TOWN_EXTRA_RACE_FIRST_MASK);
        m_town->m_buildings |= H2EnumIndex(TOWN_EXTRA_RACE_SECOND_MASK);
    }
    if (m_town->m_type == FACTION_KNIGHT) {
        m_town->m_buildings |= H2EnumIndex(TOWN_EXTRA_RACE_SECOND_MASK);
        m_town->m_buildings |= H2EnumIndex(TOWN_EXTRA_RACE_THIRD_MASK);
    }
    if (m_town->m_type == FACTION_BARBARIAN) {
        m_town->m_buildings |= H2EnumIndex(TOWN_EXTRA_RACE_SECOND_MASK);
        m_town->m_buildings |= H2EnumIndex(TOWN_EXTRA_RACE_THIRD_MASK);
        m_town->m_buildings |= H2EnumIndex(TOWN_EXTRA_RACE_LAST_MASK);
    }
    if ((m_town->m_type == FACTION_WARLOCK || m_town->m_type == FACTION_KNIGHT
         || m_town->m_type == FACTION_BARBARIAN || m_town->m_type == FACTION_NECROMANCER
         || m_town->m_type == FACTION_CYBORG)
        && m_town->CanBuildDock())
        m_town->m_buildings |= H2EnumIndex(TOWN_EXTRA_RACE_FIRST_MASK);
    if (m_town->m_type == FACTION_CYBORG)
        m_town->m_buildings |= H2EnumIndex(TOWN_EXTRA_RACE_SECOND_MASK);
    if ((m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_DOCK))
        && gpAdvManager->GetCell(m_town->m_boatX, m_town->m_boatY)->m_triggerType
               != MAP_OBJECT_NONE)
        m_town->m_buildings |= H2EnumIndex(TOWN_EXTRA_DOCK_GRAPHIC_MASK);
    else
        m_town->m_buildings &= ~H2EnumIndex(TOWN_EXTRA_DOCK_GRAPHIC_MASK);
}

i32 townManager::Open(i32 id) {
    gpGame->CheckHeroConsistency();
    if (gConfig.useOpera != CONFIG_OPERA_DISABLED
        || gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI)
        gpSoundManager->SwitchAmbientMusic(townTheme[H2EnumIndex(m_town->m_type)]);
    PollSound();
    m_townWindow = new heroWindow(0, 0, "townwind.bin");
    if (m_townWindow == NULL)
        MemError();
    glTimers[0] = platform::Ticks() + TOWN_REDRAW_INTERVAL;
    m_lastTownType = FACTION_UNINITIALIZED;
    m_castleDialogActive = false;
    m_recruitResult = false;
    m_lastHoverId = TOWN_HOVER_NONE;
    m_lastHoverSubId = 0;
    m_townObjectCount = 0;
    m_unknownC6 = 0;
    m_garrisonStrip = NULL;
    m_heroStrip = NULL;
    m_selectedStrip = NULL;
    m_swapStrip = NULL;
    m_pendingStrip = NULL;
    m_bankBox = NULL;
    m_backgroundIcon = NULL;
    SetupExtraStuff();
    SetupTown();
    platform::ChangeMenu(hmnuTown);
    gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
    m_messageMask = BASE_MANAGER_ACCEPT_TOWN_EVENT;
    m_priority = id;
    m_active = true;
    strcpy(m_name, "townManager");
    gpWindowManager->FadeScreen(FADE_IN, TOWN_FADE_STEPS, NULL);
    ironfist::hooks::TownOpened(m_town);
    gpSoundManager->SwitchAmbientMusic(townTheme[H2EnumIndex(m_town->m_type)]);
    return 0;
}

void townManager::ChangeTown(void) {
    tag_message message;

    SetupExtraStuff();
    SetupTown();
    message.type = MESSAGE_WIDGET;
    message.payload.widget.id = TOWN_WIDGET_ID_NONE;
    SetCommandAndText(message);
    if (gConfig.useOpera != CONFIG_OPERA_DISABLED
        || gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI) {
        if (gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI)
            platform::Host().Sleep(100);
        gpSoundManager->SwitchAmbientMusic(townTheme[H2EnumIndex(m_town->m_type)]);
    }
}

void townManager::SetupTown(void) {
    tag_message message;
    i32 i;

    utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, GetTownName(m_town->m_id));
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, TOWN_WINDOW_TEXT_CONTROL);
    message.payload.widget.data.text = gText;
    m_townWindow->BroadcastMessage(message);
    utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("town.screen.title"));
    message.payload.widget.id = TOWN_CONTROL_STATUS_TEXT;
    message.payload.widget.data.text = gText;
    m_townWindow->BroadcastMessage(message);
    m_townWindow->DrawWindow(
        WINDOW_DRAW_BUFFER_ONLY,
        TOWN_NAVIGATION_DRAW_FIRST_WIDGET,
        TOWN_NAVIGATION_DRAW_LAST_WIDGET
    );

    if (gpCurPlayer->m_townCount == 1) {
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAGS_ARGUMENT_DIMMED);
        message.payload.widget.id = CONTROL_PREVIOUS_TOWN;
        m_townWindow->BroadcastMessage(message);
        message.payload.widget.id = CONTROL_NEXT_TOWN;
        m_townWindow->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
        message.payload.widget.id = CONTROL_PREVIOUS_TOWN;
        m_townWindow->BroadcastMessage(message);
        message.payload.widget.id = CONTROL_NEXT_TOWN;
        m_townWindow->BroadcastMessage(message);
    }

    if (m_lastTownType != m_town->m_type) {
        if (m_lastTownType != FACTION_NONE)
            UnloadTown();
        m_bankBox = new bankBox(TOWN_BANK_BOX_X, TOWN_GARRISON_STRIP_Y, gpCurPlayer);
        if (m_bankBox == NULL)
            MemError();
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "townbkg%d.icn", H2EnumIndex(m_town->m_type));
        m_backgroundIcon = gpResourceManager->GetIcon(gText);
        m_townObjectCount = 0;
        for (i = 0; i < H2EnumIndex(BUILDING_SLOT_COUNT); ++i) {
            H2EnumStorage<BuildingSlotType, i32> buildId = gTownObjectOrder[H2EnumIndex(m_town->m_type)][i];
            if (buildId != TOWN_OBJECT_NONE) {
                utf8::Format(
                    gText, GLOBAL_TEXT_BUFFER_SIZE,
                    "%s%s",
                    gTownPrefixNames[H2EnumIndex(m_town->m_type)],
                    gTownObjNames[H2EnumIndex(buildId)]
                );
                m_townObjects[m_townObjectCount] = new townObject(m_town->m_type, buildId, gText);
                if (m_townObjects[m_townObjectCount] == NULL)
                    MemError();
                if (m_townObjects[m_townObjectCount]->m_border != NULL) {
                    if (!(m_town->m_buildings & (1 << H2EnumIndex(buildId)))) {
                        m_townObjects[m_townObjectCount]->m_border->m_flags &= ~WIDGET_FLAG_ENABLED;
                        m_townObjects[m_townObjectCount]->m_visible = 0;
                    }
                    m_townWindow->AddWidget(
                        m_townObjects[m_townObjectCount]->m_border,
                        TOWN_WIDGET_INSERT_DEFAULT
                    );
                }
                ++m_townObjectCount;
            }
        }
        gpWindowManager->AddWindow(m_townWindow, 0, 1);
    } else {
        m_townObjectCount = 0;
        for (i = 0; i < H2EnumIndex(BUILDING_SLOT_COUNT); ++i) {
            H2EnumStorage<BuildingSlotType, i32> buildId = gTownObjectOrder[H2EnumIndex(m_town->m_type)][i];
            if (buildId != TOWN_OBJECT_NONE) {
                if (m_townObjects[m_townObjectCount]->m_border != NULL) {
                    if (!(m_town->m_buildings & (1 << H2EnumIndex(buildId)))) {
                        m_townObjects[m_townObjectCount]->m_border->m_flags &= ~WIDGET_FLAG_ENABLED;
                        m_townObjects[m_townObjectCount]->m_visible = 0;
                    } else {
                        m_townObjects[m_townObjectCount]->m_border->m_flags |= WIDGET_FLAG_ENABLED;
                        m_townObjects[m_townObjectCount]->m_visible = 1;
                    }
                }
                ++m_townObjectCount;
            }
        }
        if (m_heroStrip != NULL)
            delete m_heroStrip;
        m_heroStrip = NULL;
        if (m_garrisonStrip != NULL)
            delete m_garrisonStrip;
        m_garrisonStrip = NULL;
    }

    m_garrisonStrip = new strip(
        0,
        TOWN_GARRISON_STRIP_Y,
        m_town->m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE
            ? static_cast<i8>(TOWN_CREST_FRAME_WITHOUT_HERO)
            : static_cast<i8>(TOWN_CREST_FRAME_WITH_HERO),
        gpResourceManager->MakeId("crest.icn", TOWN_ICON_RESOURCE_TYPE),
        gpCurPlayer->m_color,
        &m_town->m_army,
        TOWN_GARRISON_FIRST_CONTROL,
        0,
        -1
    );
    if (m_garrisonStrip == NULL)
        MemError();

    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "port%04d.icn", H2EnumIndex(gpGame->GetHero(m_town->m_occupyingHeroId)->m_portrait));
        m_heroStrip = new strip(
            0,
            TOWN_HERO_STRIP_Y,
            TOWN_HERO_STRIP_FRAME_COUNT,
            gpResourceManager->MakeId(gText, TOWN_ICON_RESOURCE_TYPE),
            0,
            &gpGame->GetHero(m_town->m_occupyingHeroId)->m_army,
            TOWN_HERO_FIRST_CONTROL,
            0,
            -1
        );
        if (m_heroStrip == NULL)
            MemError();
        if (m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_MAGE_GUILD))
            m_town->GiveSpells(NULL);
    } else if (m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_CAPTAIN_QUARTERS)) {
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "port%04d.icn", H2EnumIndex(m_town->m_type) + TOWN_PORTRAIT_FRAME_BASE);
        m_heroStrip = new strip(
            0,
            TOWN_HERO_STRIP_Y,
            TOWN_HERO_STRIP_FRAME_COUNT,
            gpResourceManager->MakeId(gText, TOWN_ICON_RESOURCE_TYPE),
            0,
            NULL,
            -1,
            0,
            gpCurPlayer->m_color
        );
        if (m_heroStrip == NULL)
            MemError();
    } else {
        m_heroStrip = new strip(
            0,
            TOWN_HERO_STRIP_Y,
            TOWN_HERO_STRIP_FRAME_COUNT,
            gpResourceManager->MakeId("strip.icn", TOWN_ICON_RESOURCE_TYPE),
            TOWN_HERO_STRIP_FRAME_COUNT,
            NULL,
            -1,
            0,
            -1
        );
        if (m_heroStrip == NULL)
            MemError();
    }

    m_lastTownType = m_town->m_type;
    m_pendingStrip = NULL;
    m_swapStrip = NULL;
    m_selectedStrip = NULL;
    m_pendingArmySlot = TOWN_ARMY_SLOT_NONE;
    m_swapArmySlot = TOWN_ARMY_SLOT_NONE;
    m_selectedArmySlot = TOWN_ARMY_SLOT_NONE;
    DrawTown(0, 0);
    gpWindowManager->UpdateScreenRegion(0, 0, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT);
}

void townManager::UnloadTown(void) {
    i32 index;

    if (m_bankBox != NULL)
        delete m_bankBox;
    m_bankBox = NULL;
    if (m_heroStrip != NULL)
        delete m_heroStrip;
    m_heroStrip = NULL;
    if (m_garrisonStrip != NULL)
        delete m_garrisonStrip;
    m_garrisonStrip = NULL;
    for (index = 0; index < m_townObjectCount; ++index) {
        m_townWindow->RemoveWidget(m_townObjects[index]->m_border);
        delete m_townObjects[index];
        m_townObjects[index] = NULL;
    }
    if (m_backgroundIcon != NULL) {
        gpResourceManager->Dispose(m_backgroundIcon);
        m_backgroundIcon = NULL;
    }
}

void townManager::Close(void) {
    UnloadTown();
    if (m_townWindow != NULL) {
        gpWindowManager->RemoveWindow(m_townWindow);
        delete m_townWindow;
    }
    m_townWindow = NULL;
    if (gConfig.useOpera != CONFIG_OPERA_DISABLED
        || gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI)
        gpSoundManager->SwitchAmbientMusic(TOWN_MUSIC_STOP);
    gpWindowManager->FadeScreen(FADE_OUT, TOWN_FADE_STEPS, NULL);
    gpMouseManager->SetPointer(TOWN_POINTER_DEFAULT);
    m_active = false;
    m_town->m_buildings &= H2EnumIndex(TOWN_CLOSE_DYNAMIC_CLEAR_MASK);
}

void townManager::SetArmyCommand(i32 qualifier) {
    b32 cantMoveLastArmy;
    b32 sameType;

    m_command = ARMY_COMMAND_NONE;
    cantMoveLastArmy = false;
    if (m_swapStrip->m_army->GetNumArmies() == 1 && m_swapStrip == m_heroStrip
        && m_pendingStrip != m_swapStrip)
        cantMoveLastArmy = true;

    if (m_swapStrip == m_pendingStrip && m_swapArmySlot == m_pendingArmySlot) {
        utf8::Format(
            m_statusText,
            cTownCommand[H2EnumIndex(TEXT_VIEW_ARMY)],
            gArmyNamesPlural[H2EnumIndex(m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])]
        );
        m_command = ARMY_COMMAND_VIEW;
    } else {
        sameType = false;
        if (m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]
            == m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])
            sameType = true;
        if (sameType) {
            if (qualifier != 0) {
                utf8::Format(
                    m_statusText,
                    cTownCommand[H2EnumIndex(TEXT_REDISTRIBUTE_ARMY)],
                    gArmyNamesPlural[H2EnumIndex(m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])]
                );
                m_command = ARMY_COMMAND_SPLIT;
            } else if (cantMoveLastArmy) {
                strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_CANNOT_COMBINE_LAST_ARMY)]);
                return;
            } else {
                utf8::Format(
                    m_statusText,
                    cTownCommand[H2EnumIndex(TEXT_COMBINE_ARMIES)],
                    gArmyNamesPlural[H2EnumIndex(m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])]
                );
                m_command = ARMY_COMMAND_MERGE;
            }
        } else if (qualifier != 0
                   && m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] == CREATURE_NONE) {
            utf8::Format(
                m_statusText,
                cTownCommand[H2EnumIndex(TEXT_REDISTRIBUTE_TO_EMPTY_SLOT)],
                gArmyNamesPlural[H2EnumIndex(m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])]
            );
            m_command = ARMY_COMMAND_SPLIT;
        }
    }

    if (m_command != ARMY_COMMAND_NONE)
        return;
    if (m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] == CREATURE_NONE) {
        if (cantMoveLastArmy) {
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_CANNOT_MOVE_LAST_ARMY)]);
            return;
        } else {
            utf8::Format(
                m_statusText,
                cTownCommand[H2EnumIndex(TEXT_MOVE_ARMY)],
                gArmyNamesPlural[H2EnumIndex(m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])]
            );
            m_command = ARMY_COMMAND_SWAP;
        }
    } else {
        utf8::Format(
            m_statusText,
            cTownCommand[H2EnumIndex(TEXT_EXCHANGE_ARMIES)],
            gArmyNamesPlural[H2EnumIndex(m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])],
            gArmyNamesPlural[H2EnumIndex(m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot])]
        );
        m_command = ARMY_COMMAND_SWAP;
    }
}

void townManager::SetCommandAndText(struct tag_message& message) {
    i32 objectId = message.payload.widget.id;

    m_command = ARMY_COMMAND_NONE;
    switch (TownManagerWidgetIdFromCode(objectId)) {
        case TOWN_WIDGET_CLOSE:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_EXIT)]);
            break;
        case TOWN_WIDGET_NONE:
        case TOWN_WIDGET_EMPTY_FIRST:
        case TOWN_WIDGET_EMPTY_LAST:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_EMPTY_STATUS)]);
            break;
        case TOWN_WIDGET_GARRISON_CREST:
            utf8::Format(
                m_statusText,
                localization::Tr("calendar.date.status"),
                gpGame->m_month,
                gpGame->m_week,
                gpGame->m_day
            );
            break;
        case TOWN_WIDGET_GARRISON_FIRST:
        case TOWN_WIDGET_GARRISON_SECOND:
        case TOWN_WIDGET_GARRISON_THIRD:
        case TOWN_WIDGET_GARRISON_FOURTH:
        case TOWN_WIDGET_GARRISON_LAST:
            if (m_swapArmySlot != TOWN_ARMY_SLOT_NONE) {
                m_pendingStrip = m_garrisonStrip;
                m_pendingArmySlot = objectId - TOWN_GARRISON_SLOT_FIRST;
                SetArmyCommand(message.payload.widget.parameter & TOWN_ARMY_QUALIFIER_MASK);
            } else {
                m_selectedStrip = m_garrisonStrip;
                m_selectedArmySlot = objectId - TOWN_GARRISON_SLOT_FIRST;
                if (m_selectedStrip->m_army->m_creatureTypes[m_selectedArmySlot] == CREATURE_NONE) {
                    strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_EMPTY_SLOT)]);
                } else {
                    utf8::Format(
                        m_statusText,
                        cTownCommand[H2EnumIndex(TEXT_SELECT_ARMY)],
                        gArmyNames[H2EnumIndex(m_selectedStrip->m_army
                                           ->m_creatureTypes[m_selectedArmySlot])]
                    );
                    m_command = ARMY_COMMAND_SELECT;
                }
            }
            break;
        case TOWN_WIDGET_HERO_CONTROL:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_VIEW_HERO)]);
            m_command = ARMY_COMMAND_VIEW_HERO;
            break;
        case TOWN_WIDGET_HERO_FIRST:
        case TOWN_WIDGET_HERO_SECOND:
        case TOWN_WIDGET_HERO_THIRD:
        case TOWN_WIDGET_HERO_FOURTH:
        case TOWN_WIDGET_HERO_LAST:
            if (m_swapArmySlot != TOWN_ARMY_SLOT_NONE) {
                m_pendingStrip = m_heroStrip;
                m_pendingArmySlot = objectId - TOWN_HERO_SLOT_FIRST;
                SetArmyCommand(message.payload.widget.parameter & TOWN_ARMY_QUALIFIER_MASK);
            } else {
                m_selectedStrip = m_heroStrip;
                m_selectedArmySlot = objectId - TOWN_HERO_SLOT_FIRST;
                if (m_selectedStrip->m_army == NULL
                    || m_selectedStrip->m_army->m_creatureTypes[m_selectedArmySlot]
                           == CREATURE_NONE) {
                    strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_EMPTY_SLOT)]);
                    m_command = ARMY_COMMAND_NONE;
                } else {
                    utf8::Format(
                        m_statusText,
                        cTownCommand[H2EnumIndex(TEXT_SELECT_ARMY)],
                        gArmyNames[H2EnumIndex(m_selectedStrip->m_army
                                           ->m_creatureTypes[m_selectedArmySlot])]
                    );
                    m_command = ARMY_COMMAND_SELECT;
                }
            }
            break;
        case TOWN_WIDGET_BUILDING_MAGE_GUILD:
            strcpy(m_statusText, GetBuildingName(m_town->m_type, BUILDING_SLOT_MAGE_GUILD));
            break;
        case TOWN_WIDGET_BUILDING_THIEVES_GUILD:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_THIEVES_GUILD)]);
            break;
        case TOWN_WIDGET_BUILDING_TAVERN:
            if (m_town->m_type == FACTION_NECROMANCER)
                strcpy(m_statusText, xNecromancerShrine);
            else
                strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_TAVERN)]);
            break;
        case TOWN_WIDGET_BUILDING_DOCK:
        case TOWN_WIDGET_BUILDING_BOAT:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_DOCK)]);
            break;
        case TOWN_WIDGET_BUILDING_WELL:
            strcpy(m_statusText, GetBuildingName(m_town->m_type, BUILDING_SLOT_WELL));
            break;
        case TOWN_WIDGET_BUILDING_TENT:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_TENT)]);
            break;
        case TOWN_WIDGET_BUILDING_CASTLE:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_CASTLE)]);
            break;
        case TOWN_WIDGET_BUILDING_STATUE:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_STATUE)]);
            break;
        case TOWN_WIDGET_BUILDING_LEFT_TURRET:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_LEFT_TURRET)]);
            break;
        case TOWN_WIDGET_BUILDING_RIGHT_TURRET:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_RIGHT_TURRET)]);
            break;
        case TOWN_WIDGET_BUILDING_MOAT:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_MOAT)]);
            break;
        case TOWN_WIDGET_BUILDING_MARKETPLACE:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_MARKETPLACE)]);
            break;
        case TOWN_WIDGET_BUILDING_CAPTAIN_QUARTERS:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_CAPTAIN_QUARTERS)]);
            break;
        case TOWN_WIDGET_BUILDING_SPECIAL:
            strcpy(m_statusText, gSpecialBuildingNames[H2EnumIndex(m_town->m_type)]);
            break;
        case TOWN_WIDGET_BUILDING_SECOND_WELL:
            strcpy(m_statusText, gWellExtraNames[H2EnumIndex(m_town->m_type)]);
            break;
        case TOWN_WIDGET_BUILDING_DWELLING_1:
        case TOWN_WIDGET_BUILDING_DWELLING_2:
        case TOWN_WIDGET_BUILDING_DWELLING_3:
        case TOWN_WIDGET_BUILDING_DWELLING_4:
        case TOWN_WIDGET_BUILDING_DWELLING_5:
        case TOWN_WIDGET_BUILDING_DWELLING_6:
        case TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_2:
        case TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_3:
        case TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_4:
        case TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_5:
        case TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_6:
        case TOWN_WIDGET_BUILDING_ALTERNATE_DWELLING_6:
            utf8::Format(
                m_statusText,
                cTownCommand[H2EnumIndex(TEXT_RECRUIT)],
                gArmyNamesPlural[H2EnumIndex(gDwellingType[H2EnumIndex(m_town->m_type)][objectId - H2EnumIndex(TOWN_OBJECT_DWELLING_1)])]
            );
            break;
    }
    ShowText(m_statusText);
}

void townManager::ShowText(char*) {
    tag_message message;

    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, TOWN_CONTROL_STATUS_TEXT);
    message.payload.widget.data.text = m_statusText;
    m_townWindow->BroadcastMessage(message);
    m_townWindow->DrawWindow(
        WINDOW_DRAW_BUFFER_ONLY,
        TOWN_STATUS_DRAW_FIRST_WIDGET,
        TOWN_STATUS_DRAW_LAST_WIDGET
    );
    gpWindowManager->UpdateScreenRegion(
        TOWN_STATUS_REGION_X,
        TOWN_STATUS_REGION_Y,
        TOWN_STATUS_REGION_WIDTH,
        TOWN_STATUS_REGION_HEIGHT
    );
}

MessageDispatchResult townManager::Main(tag_message& message) {
    SAMPLE2 buildSound = NULL;
    i32 loop;
    i32 leaveTown = 0;
    b32 quickView;
    char text[BUILDING_DESCRIPTION_CAPACITY];
    i32 dbgBuild;
    baseManager* manager;

    i32 tradeCount;

    const auto showBuildingInformation = [&]() {
        utf8::Copy(
            text, sizeof(text),
            GetBuildingInfo(
                m_town->m_type,
                BuildingSlotTypeFromCode(message.payload.widget.id),
                1
            )
        );
        NormalDialog(text, NORMAL_DIALOG_QUICK_VIEW, -1, -1, H2EnumIndex(m_town->m_type) + BUILDING_DIALOG_ICON_FRAME_BASE, message.payload.widget.id);
    };

    if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON))))
        quickView = true;
    else
        quickView = false;

    if (giDebugBuildingToBuild != -1) {
        dbgBuild = giDebugBuildingToBuild;
        giDebugBuildingToBuild = -1;
        if (dbgBuild == TOWN_DEBUG_BUILD_ALL) {
            for (loop = 0; loop < H2EnumIndex(BUILDING_SLOT_COUNT); ++loop) {
                if ((gTownEligibleBuildMask[H2EnumIndex(m_town->m_type)]
                     & (1 << H2EnumIndex(BuildingSlotTypeFromOrdinal(loop))))
                    || loop == H2EnumIndex(BUILDING_SLOT_CASTLE))
                    BuildObj(BuildingSlotTypeFromOrdinal(loop));
            }
        } else {
            if ((gTownEligibleBuildMask[H2EnumIndex(m_town->m_type)]
                 & (1 << H2EnumIndex(BuildingSlotTypeFromOrdinal(dbgBuild))))
                || dbgBuild == H2EnumIndex(BUILDING_SLOT_CASTLE))
                BuildObj(BuildingSlotTypeFromOrdinal(dbgBuild));
        }
    }

    if (glTimers[0] < platform::Ticks()) {
        DrawTown(1, 1);
        glTimers[0] = platform::Ticks() + TOWN_REDRAW_INTERVAL;
    }

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_SELECT:
                case WIDGET_NOTIFY_RIGHT_CLICK: {
                    switch (TownManagerWidgetIdFromCode(message.payload.widget.id)) {
                        case TOWN_WIDGET_BUILDING_DWELLING_1:
                        case TOWN_WIDGET_BUILDING_DWELLING_2:
                        case TOWN_WIDGET_BUILDING_DWELLING_3:
                        case TOWN_WIDGET_BUILDING_DWELLING_4:
                        case TOWN_WIDGET_BUILDING_DWELLING_5:
                        case TOWN_WIDGET_BUILDING_DWELLING_6:
                        case TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_2:
                        case TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_3:
                        case TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_4:
                        case TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_5:
                        case TOWN_WIDGET_BUILDING_UPGRADED_DWELLING_6:
                        case TOWN_WIDGET_BUILDING_ALTERNATE_DWELLING_6:
                            if (quickView) {
                                QuickViewRecruit(
                                    m_town,
                                    message.payload.widget.id - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)
                                );
                                break;
                            }
                            {
                                DrawTown(1, 1);
                                manager = new recruitUnit(
                                    m_town,
                                    message.payload.widget.id - H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST),
                                    1
                                );
                                if (manager == NULL)
                                    MemError();
                                gpExec->DoDialog(manager);
                                delete manager;
                            }
                            break;

                        case TOWN_WIDGET_BUILDING_CASTLE:
                            if (quickView) {
                                showBuildingInformation();
                                break;
                            }
                            {
                                m_heroWindow0 = new heroWindow(0, 0, "caslwind.bin");
                                if (m_heroWindow0 == NULL)
                                    MemError();
                                SetupCastle(m_heroWindow0, 0);
                                m_castleDialogActive = true;
                                m_recruitResult = false;
                                gpWindowManager->DoDialog(m_heroWindow0, CastleHandler, 0);
                                m_castleDialogActive = false;
                                delete m_heroWindow0;

                                if (m_recruitResult != 0) {
                                    RedrawTownScreen();
                                    gpWindowManager->SaveFizzleSource(
                                        0,
                                        TOWN_VIEWPORT_HEIGHT,
                                        TOWN_VIEW_FIZZLE_WIDTH,
                                        TOWN_VIEW_FIZZLE_HEIGHT
                                    );
                                    delete m_heroStrip;
                                    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "port%04d.icn", H2EnumIndex(m_recruitHero->m_portrait));
                                    m_heroStrip = new strip(
                                        0,
                                        TOWN_HERO_STRIP_Y,
                                        TOWN_HERO_STRIP_FRAME_COUNT,
                                        gpResourceManager->MakeId(gText, TOWN_ICON_RESOURCE_TYPE),
                                        0,
                                        &m_recruitHero->m_army,
                                        TOWN_HERO_FIRST_CONTROL,
                                        0,
                                        -1
                                    );
                                    if (m_heroStrip == NULL)
                                        MemError();
                                    buildSound = LoadPlaySample("buildtwn.82M");

                                    i32 width = TOWN_VIEW_FIZZLE_WIDTH;
                                    m_townWindow->DrawWindow(WINDOW_DRAW_BUFFER_ONLY);
                                    m_garrisonStrip->DrawIcons(0);
                                    m_heroStrip->DrawIcons(0);
                                    gpWindowManager->FizzleForward(
                                        0,
                                        TOWN_VIEWPORT_HEIGHT,
                                        width,
                                        TOWN_VIEW_FIZZLE_HEIGHT,
                                        -1,
                                        NULL,
                                        NULL
                                    );
                                    WaitEndSample(&buildSound);
                                    m_recruitResult = false;
                                    gpWindowManager->ReleaseFizzleSource();
                                } else {
                                    if (m_selectedBuilding == BUILDING_SLOT_NEUTRAL_LAST
                                        && m_town->m_occupyingHeroId == -1) {
                                        if (m_heroStrip != NULL)
                                            delete m_heroStrip;
                                        m_heroStrip = NULL;
                                        utf8::Format(
                                            gText, GLOBAL_TEXT_BUFFER_SIZE,
                                            "port%04d.icn",
                                            H2EnumIndex(m_town->m_type) + H2EnumIndex(TOWN_PORTRAIT_FRAME_BASE)
                                        );
                                        m_heroStrip = new strip(
                                            0,
                                            TOWN_HERO_STRIP_Y,
                                            TOWN_HERO_STRIP_FRAME_COUNT,
                                            gpResourceManager
                                                ->MakeId(gText, TOWN_ICON_RESOURCE_TYPE),
                                            0,
                                            NULL,
                                            -1,
                                            0,
                                            gpCurPlayer->m_color
                                        );
                                        if (m_heroStrip == NULL)
                                            MemError();
                                    }
                                    RedrawTownScreen();
                                    if (m_selectedBuilding != BUILDING_SLOT_NONE)
                                        BuildObj(m_selectedBuilding);
                                }
                            }
                            break;

                        case TOWN_WIDGET_BUILDING_MAGE_GUILD:
                            if (quickView) {
                                showBuildingInformation();
                                break;
                            }
                            {
                                if (m_town->m_occupyingHeroId != -1
                                    && !gpGame->GetHero(m_town->m_occupyingHeroId)
                                            ->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                                    if (gpGame->GetHero(m_town->m_occupyingHeroId)->NumArtifacts()
                                        == HERO_ARTIFACT_SLOT_COUNT) {
                                        NormalDialog(localization::Tr("town.mage_guild.spell_book.no_artifact_space"), NORMAL_DIALOG_INFO);
                                    } else if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)]
                                               < TOWN_SPELL_BOOK_COST) {
                                        NormalDialog(localization::Tr("town.mage_guild.spell_book.cannot_afford"), NORMAL_DIALOG_INFO, -1, -1, NORMAL_DIALOG_ARTIFACT, H2EnumIndex(ARTIFACT_MAGIC_BOOK));
                                    } else {
                                        NormalDialog(localization::Tr("town.mage_guild.spell_book.confirm_purchase"), NORMAL_DIALOG_CONFIRM, -1, -1, NORMAL_DIALOG_ARTIFACT, H2EnumIndex(ARTIFACT_MAGIC_BOOK));
                                        if (gpWindowManager->m_dialogResult
                                            == DIALOG_BUY_SPELL_BOOK) {
                                            GiveArtifact(
                                                gpGame->GetHero(m_town->m_occupyingHeroId),
                                                ARTIFACT_MAGIC_BOOK,
                                                true,
                                                -1
                                            );
                                            gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -=
                                                TOWN_SPELL_BOOK_COST;
                                            m_bankBox->Update(1);
                                            m_townWindow->DrawWindow();
                                            m_town->GiveSpells(NULL);
                                        }
                                    }
                                } else {
                                    m_heroWindow0 = new heroWindow(0, 0, "magewind.bin");
                                    if (m_heroWindow0 == NULL)
                                        MemError();
                                    SetWinText(m_heroWindow0, MAGE_GUILD_WINDOW_TEXT_ID);
                                    SetupMage(m_heroWindow0);
                                    gpWindowManager->DoDialog(m_heroWindow0, MageGuildHandler, 0);
                                    delete m_heroWindow0;
                                }
                                m_town->GiveSpells(NULL);
                                RedrawTownScreen();
                            }
                            break;

                        case TOWN_WIDGET_BUILDING_WELL:
                            if (quickView) {
                                showBuildingInformation();
                                break;
                            }
                            {
                                m_heroWindow0 = new heroWindow(0, 0, "wellwind.bin");
                                if (m_heroWindow0 == NULL)
                                    MemError();
                                SetupWell(m_heroWindow0);
                                gpWindowManager->DoDialog(m_heroWindow0, TrueFalseDialogHandler, 0);
                                delete m_heroWindow0;
                                RedrawTownScreen();
                            }
                            break;

                        case TOWN_WIDGET_BUILDING_THIEVES_GUILD:
                            if (quickView) {
                                showBuildingInformation();
                                break;
                            }
                            {
                                m_heroWindow0 = new heroWindow(0, 0, "thiefwin.bin");
                                if (m_heroWindow0 == NULL)
                                    MemError();
                                SetWinText(m_heroWindow0, THIEVES_GUILD_WINDOW_TEXT_ID);
                                SetupThievesGuild(m_heroWindow0, -1);
                                gpWindowManager->DoDialog(m_heroWindow0, TrueFalseDialogHandler, 0);
                                delete m_heroWindow0;
                                RedrawTownScreen();
                            }
                            break;

                        case TOWN_WIDGET_BUILDING_TAVERN:
                            if (quickView) {
                                showBuildingInformation();
                                break;
                            }
                            {
                                if (m_town->m_type == FACTION_NECROMANCER) {
                                    utf8::Copy(
                                        text, sizeof(text),
                                        GetBuildingInfo(
                                            m_town->m_type,
                                            BuildingSlotTypeFromCode(
                                                message.payload.widget.id
                                            ),
                                            1
                                        )
                                    );
                                    NormalDialog(text, NORMAL_DIALOG_INFO, -1, -1, H2EnumIndex(m_town->m_type) + BUILDING_DIALOG_ICON_FRAME_BASE, message.payload.widget.id);
                                } else {
                                    DoTavern();
                                }
                            }
                            break;

                        case TOWN_WIDGET_BUILDING_CASTLE_UPGRADE:
                            if (quickView) {
                                showBuildingInformation();
                                break;
                            }
                            {
                                if (m_town->m_mayNotUpgradeToCastle != 0) {
                                    NormalDialog(localization::Tr("town.upgrade.castle_forbidden"), NORMAL_DIALOG_INFO);
                                    break;
                                }
                                if (BuyBuild(
                                        BUILDING_SLOT_CASTLE,
                                        CanBuy(m_town, BUILDING_SLOT_CASTLE) == 0,
                                        quickView
                                    ))
                                    BuildObj(BUILDING_SLOT_CASTLE);
                            }
                            break;

                        case TOWN_WIDGET_BUILDING_DOCK:
                            if (quickView) {
                                showBuildingInformation();
                                break;
                            }
                            {
                                gpWindowManager->BroadcastMessage(
                                    MESSAGE_WIDGET,
                                    WIDGET_COMMAND_SET_FLAGS,
                                    CONTROL_CLOSE,
                                    H2EnumIndex(WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED)
                                );
                                if (gpGame->GetBoatsBuilt() < GAME_BOAT_COUNT
                                    && gpAdvManager->GetCell(m_town->m_boatX, m_town->m_boatY)
                                               ->m_triggerType
                                           == MAP_OBJECT_NONE) {
                                    m_heroWindow0 = new heroWindow(
                                        SMALL_DIALOG_WINDOW_X,
                                        SMALL_DIALOG_WINDOW_Y,
                                        "shipwind.bin"
                                    );
                                    if (m_heroWindow0 == NULL)
                                        MemError();
                                    SetWinText(m_heroWindow0, SHIP_WINDOW_TEXT_ID);
                                    if (gpGame->m_players[giCurPlayer].m_resources[H2EnumIndex(RES_GOLD)]
                                            < TOWN_BOAT_GOLD_COST
                                        || gpGame->m_players[giCurPlayer].m_resources[H2EnumIndex(RES_WOOD)]
                                               < TOWN_BOAT_WOOD_COST) {
                                        SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FLAGS, DIALOG_BUILD_BOAT);
                                        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAGS_ARGUMENT_DIMMED);
                                        m_heroWindow0->BroadcastMessage(message);
                                        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
                                        message.payload.widget.data.value =
                                            H2EnumIndex(WIDGET_FLAG_ENABLED);
                                        m_heroWindow0->BroadcastMessage(message);
                                    }
                                    gpWindowManager
                                        ->DoDialog(m_heroWindow0, TrueFalseDialogHandler, 0);
                                    delete m_heroWindow0;
                                    if (gpWindowManager->m_dialogResult == DIALOG_BUILD_BOAT) {
                                        if (gpGame->CreateBoat(m_town->m_boatX, m_town->m_boatY, 0)
                                            != -1) {
                                            BuildObj(BUILDING_SLOT_DISABLED_FIRST);
                                            gpGame->m_players[giCurPlayer]
                                                .m_resources[H2EnumIndex(RES_GOLD)] -= TOWN_BOAT_GOLD_COST;
                                            gpGame->m_players[giCurPlayer]
                                                .m_resources[H2EnumIndex(RES_WOOD)] -= TOWN_BOAT_WOOD_COST;
                                            m_bankBox->Update(1);
                                        } else {

                                            LogStr(localization::Tr("town.boat.creation_failed")  );
                                        }
                                    }
                                } else {
                                    NormalDialog(localization::Tr("town.boat.limit_reached"), NORMAL_DIALOG_INFO, BOAT_LIMIT_DIALOG_X, BOAT_LIMIT_DIALOG_Y);
                                }
                                gpWindowManager->BroadcastMessage(
                                    MESSAGE_WIDGET,
                                    WIDGET_COMMAND_CLEAR_FLAGS,
                                    CONTROL_CLOSE,
                                    H2EnumIndex(WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED)
                                );
                            }
                            break;

                        case TOWN_WIDGET_BUILDING_MARKETPLACE:
                            if (quickView) {
                                showBuildingInformation();
                                break;
                            }
                            {
                                tradeCount = 0;
                                for (loop = 0; loop < gpCurPlayer->m_townCount; ++loop) {
                                    if (gpGame->GetTown(gpCurPlayer->m_townIds[loop])->m_buildings
                                        & H2EnumIndex(TOWN_BUILDING_MARKETPLACE))
                                        ++tradeCount;
                                }
                                if (tradeCount > MARKETPLACE_EFFICIENCY_MAX_INDEX)
                                    tradeCount = MARKETPLACE_EFFICIENCY_MAX_INDEX;
                                DoTradingPost(1, fTradingPostEfficency[tradeCount]);
                                RedrawTownScreen();
                            }
                            break;

                        case TOWN_WIDGET_BUILDING_STATUE:
                        case TOWN_WIDGET_BUILDING_LEFT_TURRET:
                        case TOWN_WIDGET_BUILDING_RIGHT_TURRET:
                        case TOWN_WIDGET_BUILDING_MOAT:
                        case TOWN_WIDGET_BUILDING_SECOND_WELL:
                        case TOWN_WIDGET_BUILDING_SPECIAL:
                        case TOWN_WIDGET_BUILDING_CAPTAIN_QUARTERS:
                            if (quickView) {
                                showBuildingInformation();
                                break;
                            }
                            {
                                utf8::Copy(
                                    text, sizeof(text),
                                    GetBuildingInfo(
                                        m_town->m_type,
                                        BuildingSlotTypeFromCode(message.payload.widget.id),
                                        1
                                    )
                                );
                                NormalDialog(text, NORMAL_DIALOG_INFO, -1, -1, H2EnumIndex(m_town->m_type) + BUILDING_DIALOG_ICON_FRAME_BASE, message.payload.widget.id);
                            }
                            break;

                        case TOWN_WIDGET_CLOSE:
                            if (!quickView)
                                SetCommandAndText(message);
                            break;

                        default:
                            if (quickView) {
                                b32 armySelected = false;
                                hero* viewedHero;
                                if (message.payload.widget.id >= TOWN_GARRISON_SLOT_FIRST
                                    && message.payload.widget.id <= TOWN_GARRISON_SLOT_LAST) {
                                    m_selectedStrip = m_garrisonStrip;
                                    m_selectedArmySlot =
                                        message.payload.widget.id - TOWN_GARRISON_SLOT_FIRST;
                                    armySelected = true;
                                }
                                if (message.payload.widget.id >= TOWN_HERO_SLOT_FIRST
                                    && message.payload.widget.id <= TOWN_HERO_SLOT_LAST) {
                                    m_selectedStrip = m_heroStrip;
                                    m_selectedArmySlot =
                                        message.payload.widget.id - TOWN_HERO_SLOT_FIRST;
                                    armySelected = true;
                                }
                                if (armySelected
                                    && m_selectedStrip->m_army->m_creatureTypes[m_selectedArmySlot]
                                           != CREATURE_NONE) {
                                    viewedHero = m_selectedStrip == m_heroStrip
                                                     ? gpGame->GetHero(m_town->m_occupyingHeroId)
                                                     : NULL;
                                    gpGame->ViewArmy(
                                        TOWN_ARMY_VIEW_X,
                                        TOWN_ARMY_VIEW_Y,
                                        m_selectedStrip->m_army
                                            ->m_creatureTypes[m_selectedArmySlot],
                                        m_selectedStrip->m_army
                                            ->m_creatureCounts[m_selectedArmySlot],
                                        m_town,
                                        1,
                                        ARMY_FACING_RIGHT,
                                        1,
                                        viewedHero,
                                        NULL,
                                        m_selectedStrip->m_army,
                                        m_selectedArmySlot
                                    );
                                    m_bankBox->Update(1);
                                }
                            } else {
                                if (message.payload.widget.id >= TOWN_GARRISON_SLOT_FIRST
                                    && message.payload.widget.id <= TOWN_GARRISON_SLOT_LAST) {
                                    m_selectedStrip = m_garrisonStrip;
                                    m_selectedArmySlot =
                                        message.payload.widget.id - TOWN_GARRISON_SLOT_FIRST;
                                }
                                if (message.payload.widget.id >= TOWN_HERO_SLOT_FIRST
                                    && message.payload.widget.id <= TOWN_HERO_SLOT_LAST) {
                                    m_selectedStrip = m_heroStrip;
                                    m_selectedArmySlot =
                                        message.payload.widget.id - TOWN_HERO_SLOT_FIRST;
                                }
                                DoCommand(m_command);
                                SetCommandAndText(message);
                            }
                            break;
                    }
                    break;
                }

                case WIDGET_NOTIFY_DESELECT:
                    switch (message.payload.widget.id) {
                        case CONTROL_CLOSE:
                            if (quickView)
                                break;
                            ++leaveTown;
                            break;

                        case CONTROL_PREVIOUS_TOWN:
                        case CONTROL_NEXT_TOWN:
                            if (m_town->m_owner != giCurPlayer)
                                break;
                            if (gpCurPlayer->m_townCount <= 1)
                                break;
                            {
                                i32 townPosition =
                                    gpGame->TownIDToTownPos(gpCurPlayer, m_town->m_id);
                                townPosition =
                                    (townPosition + gpCurPlayer->m_townCount
                                     + (message.payload.widget.id == CONTROL_PREVIOUS_TOWN ? -1 : 1)
                                    )
                                    % gpCurPlayer->m_townCount;
                                m_town = gpGame->GetTown(gpCurPlayer->m_townIds[townPosition]);
                                ChangeTown();
                            }
                            break;
                    }
                    break;
            }
            break;

        case MESSAGE_MOUSE_MOVE:
            gpWindowManager->ConvertToHover(message);
            if (message.payload.hover.id == m_lastHoverId
                && message.payload.hover.subId == m_lastHoverSubId)
                return MESSAGE_DISPATCH_CONSUME;
            m_lastHoverId = message.payload.hover.id;
            m_lastHoverSubId = message.payload.hover.subId;
            SetCommandAndText(message);
            return MESSAGE_DISPATCH_CONSUME;

        case MESSAGE_KEY_UP:
            switch (message.payload.keyboard.keyCode) {
                case INPUT_SCAN_LEFT_SHIFT:
                case INPUT_SCAN_RIGHT_SHIFT:
                    ShiftQualChange();
                    break;
            }
            break;

        case MESSAGE_KEY_DOWN:
            switch (message.payload.keyboard.keyCode) {
                case INPUT_SCAN_LEFT_SHIFT:
                case INPUT_SCAN_RIGHT_SHIFT:
                    ShiftQualChange();
                    break;

                case INPUT_SCAN_ESCAPE:
                    ++leaveTown;
                    break;
            }
            break;
    }

    if (leaveTown == 1) {
        message.type = MESSAGE_EXECUTIVE;
        message.payload.executive.command = EXECUTIVE_COMMAND_TERMINATE_LOOP;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

void townManager::DoCommand(TownManagerArmyCommand command) {
    hero* viewedHero;
    i32 slot;
    i32 oldValue;

    switch (command) {
        case ARMY_COMMAND_NONE:
            break;

        case ARMY_COMMAND_SELECT:
            m_swapStrip = m_selectedStrip;
            m_swapArmySlot = m_selectedArmySlot;
            m_swapStrip->m_selectedSlot = m_swapArmySlot;
            m_swapStrip->Draw();
            break;

        case ARMY_COMMAND_VIEW:
            viewedHero = m_selectedStrip == m_heroStrip
                             ? gpGame->GetHero(m_town->m_occupyingHeroId)
                             : NULL;
            gpGame->ViewArmy(
                TOWN_ARMY_VIEW_X,
                TOWN_ARMY_VIEW_Y,
                m_selectedStrip->m_army->m_creatureTypes[m_selectedArmySlot],
                m_selectedStrip->m_army->m_creatureCounts[m_selectedArmySlot],
                m_town,
                m_castleDialogActive == 1
                    || (m_selectedStrip == m_heroStrip
                        && m_selectedStrip->m_army->GetNumArmies() == 1),
                ARMY_FACING_RIGHT,
                0,
                viewedHero,
                NULL,
                m_selectedStrip->m_army,
                m_selectedArmySlot
            );
            m_bankBox->Update(1);
            if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
                m_selectedStrip->m_army->m_creatureTypes[m_selectedArmySlot] = CREATURE_NONE;
                m_selectedStrip->m_army->m_creatureCounts[m_selectedArmySlot] = 0;
            }
            ResetStrips();
            break;

        case ARMY_COMMAND_MERGE:
            if (m_pendingStrip != m_swapStrip
                && m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]
                       != m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot]) {
                for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
                    if (m_pendingStrip->m_army->m_creatureTypes[slot]
                        == m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])
                        break;
                }
                if (slot < ARMY_GROUP_SLOT_COUNT)
                    m_pendingArmySlot = slot;
            }
            m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] +=
                m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
            m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] = CREATURE_NONE;
            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = 0;
            ResetStrips();
            break;

        case ARMY_COMMAND_SWAP:
            oldValue = m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot];
            m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] =
                m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = oldValue;
            oldValue = H2EnumIndex(m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]);
            m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] =
                m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot];
            m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] =
                CreatureTypeFromOrdinal(oldValue);
            ResetStrips();
            break;

        case ARMY_COMMAND_VIEW_HERO:
            HeroView(m_town->m_occupyingHeroId, true, false);
            RedrawTownScreen();
            gpWindowManager->FadeScreen(FADE_IN, TOWN_FADE_STEPS, NULL);
            break;

        case ARMY_COMMAND_SPLIT:
            SplitArmy();
            ResetStrips();
            break;
    }
    m_lastHoverId = -1;
}

void townManager::RedrawTownScreen(void) {
    tag_message message;

    DrawTown(0, 1);
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, TOWN_CONTROL_STATUS_TEXT);
    message.payload.widget.data.text = m_statusText;
    m_townWindow->BroadcastMessage(message);
    m_townWindow->DrawWindow(WINDOW_DRAW_BUFFER_ONLY);
    m_garrisonStrip->DrawIcons(0);
    m_heroStrip->DrawIcons(0);
    m_bankBox->Update(0);
    gpWindowManager->UpdateScreenRegion(0, 0, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT);
}

void townManager::SplitArmy(void) {

    b32 sameType;
    tag_message message;

    m_heroWindow1 = new heroWindow(SMALL_DIALOG_WINDOW_X, SMALL_DIALOG_WINDOW_Y, "splitwin.bin");
    if (m_heroWindow1 == NULL)
        MemError();
    m_splitAmount = 0;
    m_splitMaximum = m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
    message.type = MESSAGE_WIDGET;
    if (m_pendingStrip->m_army == m_swapStrip->m_army) {
        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("hero.army.split.prompt")  );
    } else {
        utf8::Format(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
            localization::Tr(
                m_swapStrip == m_heroStrip
                    ? "town.army.split.hero_to_garrison"
                    : "town.army.split.garrison_to_hero"
            ),
            gArmyNamesPlural[H2EnumIndex(m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])]
        );
    }
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = 1;
    message.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message);
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%d", m_splitAmount);
    message.payload.widget.id = SPLIT_SETUP_AMOUNT_CONTROL;
    message.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message);
    gpWindowManager->DoDialog(m_heroWindow1, SplitArmyHandler, 0);
    delete m_heroWindow1;
    if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
        sameType = false;
        if (m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]
            == m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])
            sameType = true;
        if (sameType != 0) {
            m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] += m_splitAmount;
        } else {
            m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] =
                m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot];
            m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] = m_splitAmount;
        }
        m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] -= m_splitAmount;
    }
}

void townManager::ShiftQualChange(void) {
    gpInputManager->ForceMouseMove();
}

void townManager::ResetStrips(void) {
    if (m_swapStrip != NULL)
        m_swapStrip->m_selectedSlot = -1;
    if (m_pendingStrip != NULL)
        m_pendingStrip->m_selectedSlot = -1;
    m_heroStrip->Draw();
    m_garrisonStrip->Draw();
    m_swapStrip = m_pendingStrip = NULL;
    m_swapArmySlot = m_pendingArmySlot = -1;
}

void townManager::Toggle(BuildingSlotType building) {
    i32 index;

    if (m_town->m_buildings & (1 << H2EnumIndex(building))) {
        for (index = 0; index < m_townObjectCount; ++index) {
            if (m_townObjects[index]->m_buildingId == building)
                m_townObjects[index]->m_visible ^= 1;
        }
    }
}

void townManager::DrawTown(i32 updateScreen, i32 drawFlags) {
    i32 index;

    PollSound();
    m_backgroundIcon->DrawToBuffer(0, 0, 0, ICON_DRAW_NORMAL);
    PollSound();
    for (index = 0; index < m_townObjectCount; ++index) {
        m_townObjects[index]->Draw(drawFlags);
        PollSound();
    }
    m_townWindow
        ->DrawWindow(WINDOW_DRAW_BUFFER_ONLY, TOWN_REDRAW_FIRST_CONTROL, TOWN_REDRAW_LAST_CONTROL);
    PollSound();
    if (updateScreen != 0)
        BlitBitmapToScreen(
            gpWindowManager->m_screen,
            0,
            0,
            LOGICAL_SCREEN_WIDTH,
            TOWN_VIEWPORT_HEIGHT,
            0,
            0
        );
    PollSound();
}

i32 townManager::BuyBuild(
    BuildingSlotType building,
    i32 cannotBuy,
    i32 quickView
) {
    u32l prerequisiteMask;
    i32 prerequisiteCount;
    char* description;

    textWidget* amountWidgets[H2EnumIndex(RES_COUNT)];
    char* amountText[H2EnumIndex(RES_COUNT)];
    iconWidget* resourceWidgets[H2EnumIndex(RES_COUNT)];
    i32 rowResourceTypes[BUILD_ROW_RESOURCE_CAPACITY];
    i32 topRowCount;
    tag_message message;
    i32 widgetIndex;
    i32 xStart;

    i32 resourceRow;
    i32 resourcesInRow;
    i32 rowY;
    i32 windowY;
    i8 resourceTypes[H2EnumIndex(RES_COUNT)];
    i32 costCount;

    i32 index;
    i32 spacing;
    i32 bottomRowCount;
    heroWindow* window;
    char iconName[TOWN_OBJECT_FILENAME_SIZE];
    i32 rowWidth;

    i32 lineCount;
    i32 windowHeight;
    icon* resourceIcon;

    i32 windowRows;
    i32 mageLevel;
    i32 resourceX;
    i32 resourceCount;
    i32 dwelling;

    i32 entryWidth;
    i16 costs[H2EnumIndex(RES_COUNT)];
    widget* descriptionWidget;

    mageLevel = 0;
    index = 0;
    costCount = 0;
    description = static_cast<char*>(H2_ALLOC(BUILDING_DESCRIPTION_CAPACITY));

    for (index = 0; index < H2EnumIndex(RES_COUNT); ++index) {
        costs[index] = -1;
        resourceTypes[index] = -1;
    }

    dwelling = -1;
    if (building >= TOWN_OBJECT_DWELLING_1 && building <= TOWN_OBJECT_ALTERNATE_UPGRADED_DWELLING_6)
        dwelling = H2EnumIndex(building) - H2EnumIndex(TOWN_OBJECT_DWELLING_1);

    if (building == BUILDING_SLOT_TAVERN && m_town->m_type == FACTION_NECROMANCER) {
        for (index = 0; index < H2EnumIndex(RES_COUNT); ++index) {
            if (xShrineBuildingCost[index] > 0) {
                resourceTypes[costCount] = static_cast<i8>(index);
                costs[costCount] = static_cast<i16>(xShrineBuildingCost[index]);
                ++costCount;
            }
        }
    } else if (building == BUILDING_SLOT_MAGE_GUILD) {
        mageLevel = gpTownManager->m_town->m_buildState;
        for (index = 0; index < H2EnumIndex(RES_COUNT); ++index) {
            if (gMageBuildingCosts
                    [NEXT_MAGE_GUILD_LEVEL(mageLevel)]
                    [index]
                > 0) {
                resourceTypes[costCount] = static_cast<i8>(index);
                costs[costCount] =
                    static_cast<i16>(gMageBuildingCosts
                                         [NEXT_MAGE_GUILD_LEVEL(mageLevel)][index]);
                ++costCount;
            }
        }
    } else if (building == BUILDING_SLOT_SPECIAL) {
        for (index = 0; index < H2EnumIndex(RES_COUNT); ++index) {
            if (gSpecialBuildingCosts[H2EnumIndex(gpTownManager->m_town->m_type)][index] > 0) {
                resourceTypes[costCount] = static_cast<i8>(index);
                costs[costCount] = static_cast<i16>(
                    gSpecialBuildingCosts[H2EnumIndex(gpTownManager->m_town->m_type)][index]
                );
                ++costCount;
            }
        }
    } else if (building <= BUILDING_SLOT_NEUTRAL_LAST) {
        for (index = 0; index < H2EnumIndex(RES_COUNT); ++index) {
            if (gNeutralBuildingCosts[H2EnumIndex(building)][index] > 0) {
                resourceTypes[costCount] = static_cast<i8>(index);
                costs[costCount] =
                    static_cast<i16>(gNeutralBuildingCosts[H2EnumIndex(building)][index]);
                ++costCount;
            }
        }
    } else {
        for (index = 0; index < H2EnumIndex(RES_COUNT); ++index) {
            if (gDwellingCosts[H2EnumIndex(gpTownManager->m_town->m_type)][dwelling][index] > 0) {
                resourceTypes[costCount] = static_cast<i8>(index);
                costs[costCount] = static_cast<i16>(
                    gDwellingCosts[H2EnumIndex(gpTownManager->m_town->m_type)][dwelling][index]
                );
                ++costCount;
            }
        }
    }

    widgetIndex = 0;
    resourceCount = 0;
    topRowCount = 0;
    bottomRowCount = 0;

    for (index = 0; index < H2EnumIndex(RES_COUNT); ++index) {
        if (resourceTypes[index] != -1)
            ++resourceCount;
    }
    if (resourceCount <= 4) {
        topRowCount = resourceCount;
    } else if (resourceCount == 5) {
        topRowCount = 2;
        bottomRowCount = 3;
    } else if (resourceCount == 6) {
        topRowCount = 3;
        bottomRowCount = 3;
    } else if (resourceCount == H2EnumIndex(RES_COUNT)) {
        topRowCount = 3;
        bottomRowCount = 4;
    }

    utf8::Copy(
        description,
        BUILDING_DESCRIPTION_CAPACITY,
        GetBuildingInfo(m_town->m_type, building, 0)
    );
    b32 disallowed_p = m_town->IsBuildingDisallowed(H2EnumIndex(building));
    if (disallowed_p) {
        utf8::Append(
            description,
            BUILDING_DESCRIPTION_CAPACITY,
            localization::Tr("town.build.disallowed")
        );
        cannotBuy = 1;
    } else if (dwelling >= 0) {
        prerequisiteCount = 0;
        prerequisiteMask = gHierarchyMask[H2EnumIndex(m_town->m_type)][dwelling];
        for (index = 0; index < H2EnumIndex(BUILDING_SLOT_COUNT); ++index) {
            if (prerequisiteMask & (1L << index)) {
                if (prerequisiteCount == 0)
                    utf8::Append(
                        description,
                        BUILDING_DESCRIPTION_CAPACITY,
                        localization::Tr("town.build.requires")
                    );
                ++prerequisiteCount;
                utf8::Append(description, BUILDING_DESCRIPTION_CAPACITY, "\n");
                utf8::Append(
                    description,
                    BUILDING_DESCRIPTION_CAPACITY,
                    GetBuildingName(m_town->m_type, BuildingSlotType(index))
                );
            }
        }
        if (m_town->m_type == FACTION_NECROMANCER
            && building == BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE
            && m_town->m_buildState <= NECROMANCER_PREREQUISITE_MAX_MAGE_LEVEL)
            utf8::Append(
                description,
                BUILDING_DESCRIPTION_CAPACITY,
                localization::Tr("town.build.requires_mage_guild_level_2")
            );
    }
    utf8::Append(description, BUILDING_DESCRIPTION_CAPACITY, "\n ");

    lineCount = bigFont->LineLength(description, BUILD_DESCRIPTION_WIDTH);
    windowY = BUILD_WINDOW_BASE_Y;
    windowHeight = windowY;
    windowHeight += lineCount << BUILD_TEXT_LINE_SHIFT;
    if (resourceCount <= BUILD_ROW_RESOURCE_CAPACITY)
        windowHeight += BUILD_SINGLE_RESOURCE_ROW_HEIGHT;
    else
        windowHeight += BUILD_DOUBLE_RESOURCE_ROW_HEIGHT;
    if (quickView == 0)
        windowHeight += BUILD_BUTTON_AREA_HEIGHT;
    windowRows = (windowHeight - BUILD_WINDOW_ROW_BASE_HEIGHT) / BUILD_WINDOW_ROW_HEIGHT;
    if (windowRows < BUILD_WINDOW_MIN_ROWS)
        windowRows = BUILD_WINDOW_MIN_ROWS;
    if (windowRows > BUILD_WINDOW_MAX_ROWS)
        windowRows = BUILD_WINDOW_MAX_ROWS;

    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "buybuil%d.bin", windowRows);
    window = new heroWindow(BUILD_WINDOW_X, BUILD_WINDOW_Y, gText);
    if (window == NULL)
        MemError();

    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_ICON, BUILD_ICON_CONTROL);
    utf8::Format(iconName, "cstl%s.icn", cHeroTypeShortName[H2EnumIndex(m_town->m_type)]);
    message.payload.widget.data.text = iconName;
    window->BroadcastMessage(message);
    message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    message.payload.widget.id = BUILD_ICON_CONTROL;
    message.payload.widget.data.value = H2EnumIndex(building);
    window->BroadcastMessage(message);

    if (building == BUILDING_SLOT_MAGE_GUILD) {
        utf8::Format(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
            localization::Tr(
                m_town->m_type == FACTION_CYBORG ? "castle.cybernetics_lab.level"
                                                 : "castle.mage_guild.level"
            ),
            NEXT_MAGE_GUILD_LEVEL(mageLevel)
        );
    } else {
        strcpy(gText, GetBuildingName(m_town->m_type, building));
    }
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = BUILD_NAME_CONTROL;
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);

    descriptionWidget = new textWidget(
        BUILD_DESCRIPTION_X,
        static_cast<i16>(windowY + BUILD_DESCRIPTION_Y_OFFSET),
        BUILD_DESCRIPTION_WIDTH,
        static_cast<i16>(lineCount << BUILD_TEXT_LINE_SHIFT),
        description,
        "bigfont.fnt",
        FONT_DRAW_DEFAULT,
        -1,
        WIDGET_KIND_UNDIMMED,
        FONT_ALIGN_CENTER
    );
    if (descriptionWidget == NULL)
        MemError();
    window->AddWidget(descriptionWidget, -1);

    widgetIndex = 0;
    if (!disallowed_p) {
        resourceIcon = gpResourceManager->GetIcon("resource.icn");
        for (resourceRow = 0; resourceRow < BUILD_RESOURCE_ROW_COUNT; ++resourceRow) {
            rowY = resourceRow * BUILD_RESOURCE_ROW_HEIGHT + lineCount * BUILD_TEXT_LINE_HEIGHT
                     + windowY + BUILD_RESOURCE_FIRST_Y_OFFSET;
            if (resourceRow == 0)
                resourcesInRow = topRowCount;
            else
                resourcesInRow = bottomRowCount;
            if (resourcesInRow > 0) {
                rowWidth = 0;
                costCount = widgetIndex;
                for (index = 0; index < BUILD_ROW_RESOURCE_CAPACITY; ++index) {
                    if (index < resourcesInRow) {
                        while (resourceTypes[costCount] == -1)
                            ++costCount;
                        rowResourceTypes[index] = resourceTypes[costCount];
                        ++costCount;
                    } else {
                        rowResourceTypes[index] = -1;
                    }
                }
                for (index = 0; index < resourcesInRow; ++index) {
                    rowWidth += GetIconEntry(resourceIcon, rowResourceTypes[index])->w;
                }
                spacing = (BUILD_RESOURCE_AREA_WIDTH - rowWidth) / (resourcesInRow + 1);
                xStart = spacing + BUILD_RESOURCE_AREA_LEFT;
                resourceX = xStart;
                for (index = 0; index < resourcesInRow; ++index) {
                    entryWidth = GetIconEntry(resourceIcon, rowResourceTypes[index])->w;
                    amountText[widgetIndex] = static_cast<char*>(H2_ALLOC(BUILD_AMOUNT_TEXT_CAPACITY));
                    utf8::Format(
                        amountText[widgetIndex],
                        BUILD_AMOUNT_TEXT_CAPACITY,
                        "%d",
                        costs[widgetIndex]
                    );
                    i32 widgetXOffset = 0;
                    amountWidgets[widgetIndex] = new textWidget(
                        static_cast<i16>(resourceX + widgetXOffset),
                        static_cast<i16>(rowY + BUILD_AMOUNT_Y_OFFSET),
                        static_cast<i16>(entryWidth),
                        BUILD_RESOURCE_WIDGET_HEIGHT,
                        amountText[widgetIndex],
                        "smalfont.fnt",
                        FONT_DRAW_DEFAULT,
                        -1,
                        WIDGET_KIND_UNDIMMED,
                        FONT_ALIGN_CENTER
                    );
                    if (amountWidgets[widgetIndex] == NULL)
                        MemError();
                    resourceWidgets[widgetIndex] = new iconWidget(
                        static_cast<i16>(
                            resourceX + widgetXOffset
                            - GetIconEntry(resourceIcon, rowResourceTypes[index])->x
                        ),
                        static_cast<i16>(rowY),
                        static_cast<i16>(entryWidth),
                        BUILD_RESOURCE_WIDGET_HEIGHT,
                        "resource.icn",
                        resourceTypes[widgetIndex],
                        ICON_DRAW_NORMAL,
                        -1,
                        WIDGET_KIND_ICON_DIRECT,
                        1
                    );
                    if (resourceWidgets[widgetIndex] == NULL)
                        MemError();
                    window->AddWidget(amountWidgets[widgetIndex], -1);
                    window->AddWidget(resourceWidgets[widgetIndex], -1);
                    ++widgetIndex;
                    resourceX += spacing + entryWidth;
                }
            }
        }
        gpResourceManager->Dispose(resourceIcon);
    }

    if (quickView == 0)
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_SET_FLAGS,
            CONTROL_CLOSE,
            H2EnumIndex(WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED)
        );
    m_selectedBuilding = BUILDING_SLOT_NONE;
    if (quickView != 0) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message.payload.widget.id = TOWN_DIALOG_CONFIRM;
        window->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message.payload.widget.id = H2EnumIndex(DIALOG_CANCEL_ID);
        window->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message.payload.widget.id = 0;
        window->BroadcastMessage(message);
        gpWindowManager->AddWindow(window, -1, 1);
        QuickViewWait();
        gpWindowManager->RemoveWindow(window);
    } else {
        if (cannotBuy != 0) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = TOWN_DIALOG_CONFIRM;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
            window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.id = TOWN_DIALOG_CONFIRM;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAGS_ARGUMENT_DIMMED);
            window->BroadcastMessage(message);
        }
        gpWindowManager->DoDialog(window, TrueFalseDialogHandler, 0);
        if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
            m_selectedBuilding = building;
            for (index = 0; index < resourceCount; ++index)
                gpCurPlayer->m_resources[resourceTypes[index]] -= costs[index];
        }
    }
    if (quickView == 0)
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_CLEAR_FLAGS,
            CONTROL_CLOSE,
            H2EnumIndex(WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED)
        );
    delete window;
    if (quickView != 0) {
        return 0;
    } else {
        return gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM;
    }
}

void townManager::BuildObj(BuildingSlotType building) {
    i32 objectIndex;
    SLimitData limits;
    i32 index;
    SAMPLE2 buildSample;
    i32 frame;

    if (TOWN_BUILDING_COMPLETE(*m_town, building)) {
        return;
    }
    if (building == BUILDING_SLOT_DOCK && !m_town->CanBuildDock()) {
        return;
    }
    {
        DrawTown(1, 1);
        m_town->BuildBuilding(building);
        for (index = 0; index < m_townObjectCount; ++index) {
            if (m_town->m_buildings & (1 << H2EnumIndex(m_townObjects[index]->m_buildingId))) {
                m_townObjects[index]->m_visible = 1;
                m_townObjects[index]->m_border->m_flags |= WIDGET_FLAG_ENABLED;
            } else {
                m_townObjects[index]->m_visible = 0;
                m_townObjects[index]->m_border->m_flags &= ~WIDGET_FLAG_ENABLED;
            }
        }

        objectIndex = -1;
        for (index = 0; index < m_townObjectCount; ++index) {
            if (m_townObjects[index]->m_buildingId == building)
                objectIndex = index;
        }

        giMaxExtentY = 0;
        giMaxExtentX = giMaxExtentY;
        giMinExtentX = LOGICAL_SCREEN_WIDTH - 1;
        giMinExtentY = TOWN_VIEWPORT_HEIGHT - 1;
        gbComputeExtent = true;
        gbSaveBiggestExtent = true;
        gbReturnAfterComputeExtent = true;
        if (building == BUILDING_SLOT_MAGE_GUILD) {
            if (gpTownManager->m_town->m_type == FACTION_NECROMANCER)
                frame = (gpTownManager->m_town->m_buildState - 1)
                          * NECROMANCER_BUILD_STATE_FRAME_STRIDE;
            else
                frame = gpTownManager->m_town->m_buildState - 1;
            m_townObjects[objectIndex]
                ->m_icon
                ->CombatClipDrawToBuffer(0, 0, frame, &limits, ICON_DRAW_NORMAL, 0, NULL, NULL);
            if (m_townObjects[objectIndex]->m_animationFrameCount != 0)
                m_townObjects[objectIndex]->m_icon->CombatClipDrawToBuffer(
                    0,
                    0,
                    frame + 1,
                    &limits,
                    ICON_DRAW_NORMAL,
                    0,
                    NULL,
                    NULL
                );
        } else {
            m_townObjects[objectIndex]
                ->m_icon
                ->CombatClipDrawToBuffer(0, 0, 0, &limits, ICON_DRAW_NORMAL, 0, NULL, NULL);
            if (m_townObjects[objectIndex]->m_animationFrameCount != 0)
                m_townObjects[objectIndex]
                    ->m_icon
                    ->CombatClipDrawToBuffer(0, 0, 1, &limits, ICON_DRAW_NORMAL, 0, NULL, NULL);
        }
        gbComputeExtent = false;
        gbSaveBiggestExtent = false;
        gbReturnAfterComputeExtent = false;

        gpWindowManager->SaveFizzleSource(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
        DrawTown(0, 1);
        buildSample = LoadPlaySample("buildtwn.82M");
        gpWindowManager->FizzleForward(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1,
            -1,
            NULL,
            NULL
        );
        WaitEndSample(&buildSample);
        PollSound();
        m_selectedBuilding = BUILDING_SLOT_NONE;
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_CLEAR_FLAGS,
            CONTROL_CLOSE,
            H2EnumIndex(WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED)
        );
        RedrawTownScreen();
    }
}

void townManager::SetupMage(heroWindow* window) {

    tag_message message;
    i32 level;
    i32 slot;
    i32 spellState;
    i32 lineCount;
    i32 unusedGuildFrame;

    message.type = MESSAGE_WIDGET;
    hero* occupyingHero = m_town->m_occupyingHeroId == -1
        ? NULL
        : gpGame->GetHero(m_town->m_occupyingHeroId);
    if (occupyingHero == NULL || !occupyingHero->HasArtifact(ARTIFACT_MAGIC_BOOK))
        utf8::Copy(
            gText,
            GLOBAL_TEXT_BUFFER_SIZE,
            localization::Tr("town.mage_guild.spells_available")
        );
    else if (m_town->m_type == FACTION_CYBORG
             && occupyingHero->m_cursorType != FACTION_CYBORG)
        utf8::Copy(
            gText,
            GLOBAL_TEXT_BUFFER_SIZE,
            localization::Tr("town.cybernetics.requires_cyborg")
        );
    else if (occupyingHero->m_cursorType == FACTION_CYBORG)
        utf8::Copy(
            gText,
            GLOBAL_TEXT_BUFFER_SIZE,
            localization::Tr("town.cybernetics.level_limit")
        );
    else
        utf8::Copy(
            gText,
            GLOBAL_TEXT_BUFFER_SIZE,
            localization::Tr("town.mage_guild.spells_added")
        );
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = TOWN_MAGE_DESCRIPTION_CONTROL;
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);

    for (level = 0; level < TOWN_MAGE_GUILD_LEVEL_COUNT; ++level) {
        for (slot = 0; slot < TOWN_MAGE_GUILD_SPELLS_PER_LEVEL; ++slot) {
            i32 slotLimit =
                gSpellLimits[level]
                + (m_town->m_type == FACTION_WIZARD
                   && (m_town->m_buildings & TOWN_WIZARD_LIBRARY_BUILDING_FLAG));
            if (m_town->m_type == FACTION_CYBORG)
                slotLimit = ironfist::CyborgSpellLimits[level];
            if (slot >= slotLimit) {
                spellState = TOWN_MAGE_SPELL_UNAVAILABLE;
            } else {
                spellState =
                    static_cast<i16>(slot >= m_town->m_spellCounts[level]);
            }

            message.payload.widget.command =
                spellState == TOWN_MAGE_SPELL_UNAVAILABLE ? WIDGET_COMMAND_CLEAR_FLAGS
                                                            : WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.id =
                level * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + slot + TOWN_MAGE_FIRST_SPELL_CONTROL;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            window->BroadcastMessage(message);

            if (spellState != TOWN_MAGE_SPELL_UNAVAILABLE) {
                message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                message.payload.widget.id =
                    level * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + slot + TOWN_MAGE_FIRST_SPELL_CONTROL;
                message.payload.widget.data.value = spellState;
                window->BroadcastMessage(message);
            }

            if (spellState != 0) {
                message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
                message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
                message.payload.widget.id =
                    level * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + slot + TOWN_MAGE_FIRST_ICON_CONTROL;
                window->BroadcastMessage(message);
                message.payload.widget.id = level * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + slot
                                              + TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
                window->BroadcastMessage(message);
            } else {
                message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                message.payload.widget.id =
                    level * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + slot + TOWN_MAGE_FIRST_ICON_CONTROL;
                message.payload.widget.data.value =
                    gsSpellInfo[H2EnumIndex(m_town->m_spells[level][slot])].iconIndex;
                window->BroadcastMessage(message);
                lineCount = smallFont->LineLength(
                    gSpellNames[H2EnumIndex(m_town->m_spells[level][slot])],
                    MAGE_SPELL_NAME_WIDTH
                );
                if (lineCount == 1)
                    utf8::Format(
                        gText, GLOBAL_TEXT_BUFFER_SIZE,
                        "%s\n[%d]",
                        gSpellNames[H2EnumIndex(m_town->m_spells[level][slot])],
                        GetManaCost(m_town->m_spells[level][slot], NULL)
                    );
                else
                    utf8::Format(
                        gText, GLOBAL_TEXT_BUFFER_SIZE,
                        "%s  [%d]",
                        gSpellNames[H2EnumIndex(m_town->m_spells[level][slot])],
                        GetManaCost(m_town->m_spells[level][slot], NULL)
                    );
                message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                message.payload.widget.id = level * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + slot
                                              + TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
                message.payload.widget.data.text = gText;
                window->BroadcastMessage(message);
            }
        }
    }

    unusedGuildFrame = m_town->m_buildState - 1;
    message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    message.payload.widget.id = TOWN_MAGE_GUILD_ICON_CONTROL;
    message.payload.widget.data.value = unusedGuildFrame;
    window->BroadcastMessage(message);
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "magegld%c.icn", cHeroTypeInitial[H2EnumIndex(m_town->m_type)]);
    message.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    message.payload.widget.id = TOWN_MAGE_GUILD_ICON_CONTROL;
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);
}

MessageDispatchResult MageGuildHandler(tag_message& message) {

    u32 quickView;
    i32 spellSlot;
    i32 level;
    i32 slot;
    SpellType spell;

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_NOTIFY_SELECT:
            case WIDGET_NOTIFY_RIGHT_CLICK:
                quickView = (H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)));
                spellSlot = -1;
                if (message.payload.widget.id >= TOWN_MAGE_FIRST_SPELL_CONTROL
                    && message.payload.widget.id
                           < TOWN_MAGE_FIRST_SPELL_CONTROL
                                 + TOWN_MAGE_GUILD_LEVEL_COUNT * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL)
                    spellSlot = message.payload.widget.id - TOWN_MAGE_FIRST_SPELL_CONTROL;
                if (message.payload.widget.id >= TOWN_MAGE_FIRST_ICON_CONTROL
                    && message.payload.widget.id
                           < TOWN_MAGE_FIRST_ICON_CONTROL
                                 + TOWN_MAGE_GUILD_LEVEL_COUNT * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL)
                    spellSlot = message.payload.widget.id - TOWN_MAGE_FIRST_ICON_CONTROL;
                if (message.payload.widget.id >= TOWN_MAGE_FIRST_DESCRIPTION_CONTROL
                    && message.payload.widget.id
                           < TOWN_MAGE_FIRST_DESCRIPTION_CONTROL
                                 + TOWN_MAGE_GUILD_LEVEL_COUNT * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL)
                    spellSlot = message.payload.widget.id - TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
                if (spellSlot != -1) {
                    level = spellSlot / TOWN_MAGE_GUILD_SPELLS_PER_LEVEL;
                    slot = spellSlot % TOWN_MAGE_GUILD_SPELLS_PER_LEVEL;
                    if (slot >= gpTownManager->m_town->m_spellCounts[level])
                        return MESSAGE_DISPATCH_CONSUME;
                    spell = gpTownManager->m_town->m_spells[level][slot];
                    NormalDialog(gSpellDesc[H2EnumIndex(spell)], quickView != 0 ? NORMAL_DIALOG_QUICK_VIEW : NORMAL_DIALOG_INFO, -1, -1, NORMAL_DIALOG_SPELL, H2EnumIndex(spell));
                    return MESSAGE_DISPATCH_CONSUME;
                }
        }
    }
    return EventWindowHandler(message);
}

i32 townManager::RecruitHero(i32 availableHeroIndex, i32 cannotRecruit) {
    // Rehiring a hero during the same turn must not refresh movement.
    hero* recruitedHero =
        &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
    i32 previousMobility = recruitedHero->m_remainingMobility;
    tag_message message;
    i32 artifactCount;
    i32 index;

    m_heroWindow1 = new heroWindow(RECRUIT_WINDOW_X, RECRUIT_WINDOW_Y, "rcrthero.bin");
    if (m_heroWindow1 == NULL)
        MemError();
    SetWinText(m_heroWindow1, RECRUIT_WINDOW_TEXT_ID);
    m_recruitHero = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
    m_recruitHero->m_owner = giCurPlayer;
    message.type = MESSAGE_WIDGET;

    if (cannotRecruit != 0) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
        message.payload.widget.id = RECRUIT_BUTTON_TEXT_CONTROL;
        m_heroWindow1->BroadcastMessage(message);
        message.payload.widget.id = RECRUIT_BUTTON_ICON_CONTROL;
        m_heroWindow1->BroadcastMessage(message);
        message.payload.widget.id = TOWN_DIALOG_CONFIRM;
        m_heroWindow1->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAGS_ARGUMENT_DIMMED);
        message.payload.widget.id = RECRUIT_BUTTON_TEXT_CONTROL;
        m_heroWindow1->BroadcastMessage(message);
        message.payload.widget.id = RECRUIT_BUTTON_ICON_CONTROL;
        m_heroWindow1->BroadcastMessage(message);
        message.payload.widget.id = TOWN_DIALOG_CONFIRM;
        m_heroWindow1->BroadcastMessage(message);
    }

    artifactCount = 0;
    for (index = 0; index < HERO_ARTIFACT_SLOT_COUNT; ++index) {
        if (m_recruitHero->m_artifacts[index] != ARTIFACT_NONE
            && m_recruitHero->m_artifacts[index] != ARTIFACT_MAGIC_BOOK)
            ++artifactCount;
    }
    utf8::Format(
        gText, GLOBAL_TEXT_BUFFER_SIZE,
        localization::TrPlural("town.recruit_hero.description", artifactCount),
        m_recruitHero->m_name,
        m_recruitHero->m_level,
        gAlignmentNames[H2EnumIndex(m_recruitHero->m_cursorType)],
        artifactCount
    );
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = RECRUIT_DESCRIPTION_CONTROL;
    message.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message);
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "port%04d.icn", H2EnumIndex(m_recruitHero->m_portrait));
    message.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    message.payload.widget.id = RECRUIT_PORTRAIT_CONTROL;
    message.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message);

    m_recruitState = -1;
    gpWindowManager->DoDialog(m_heroWindow1, RecruitHeroHandler, 0);
    delete m_heroWindow1;
    if (m_recruitState != -1) {
        FactionType newHeroClass;
        i32 townXh;
        i32 townYWork;

        m_recruitState = availableHeroIndex;
        gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= gHeroGoldCost;
        gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] =
            gpCurPlayer->m_availableHeroIds[m_recruitState];
        ++gpCurPlayer->m_heroCount;

        townXh = m_town->m_x;
        townYWork = m_town->m_y;
        m_recruitHero->m_x = townXh;
        m_recruitHero->m_y = townYWork;
        m_recruitHero->m_eventFlags &= HERO_EVENT_SIRENS | HERO_EVENT_ARENA;
        m_recruitHero->m_direction = MAP_DIRECTION_EAST;
        m_recruitHero->m_remainingMobility = m_recruitHero->CalcMobility();
        m_recruitHero->m_mobility = m_recruitHero->m_remainingMobility;
        m_recruitHero->m_locationType =
            gpGame->m_worldMap.GetCell(townXh, townYWork)->m_triggerType;
        m_recruitHero->m_occupiedTown =
            gpGame->m_worldMap.GetCell(townXh, townYWork)->m_objectMetadata;
        gpGame->m_worldMap.GetCell(townXh, townYWork)->m_triggerType =
            (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION));
        gpGame->m_worldMap.GetCell(townXh, townYWork)->m_objectMetadata =
            gpCurPlayer->m_availableHeroIds[m_recruitState];
        SendMapChange(
            MAP_CHANGE_RECRUIT_HERO,
            m_recruitHero->m_id,
            townXh,
            townYWork,
            MAP_CHANGE_CURRENT_PLAYER,
            0,
            0
        );
        m_recruitResult = true;
        m_town->m_occupyingHeroId = m_recruitHero->m_id;
        gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
            giCurPlayer;
        CheckValidAvailableHeroes();
        if (m_town->m_buildings & 1)
            m_town->GiveSpells(NULL);

        newHeroClass = FactionTypeFromCode(
            gpCurPlayer->m_availableHeroIds[1 - m_recruitState] / HEROES_PER_FACTION
        );
        newHeroClass = (newHeroClass + Random(1, H2EnumIndex(FACTION_COUNT) - 1)) % H2EnumIndex(FACTION_COUNT);
        gpCurPlayer->m_availableHeroIds[m_recruitState] =
            gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0);
        gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
            AI_HERO_AVAILABLE_FLAG;
    } else {
        if (m_castleDialogActive != 0)
            SetupCastle(m_heroWindow0, 0);
        if (m_castleDialogActive != 0)
            m_heroWindow0->DrawWindow();
    }

    m_bankBox->Update(1);
    gpWindowManager->BroadcastMessage(
        MESSAGE_WIDGET,
        WIDGET_COMMAND_CLEAR_FLAGS,
        CONTROL_CLOSE,
        H2EnumIndex(WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED)
    );
    m_recruitHero->m_owner = -1;
    if (m_recruitState != -1)
        m_recruitHero->m_owner = giCurPlayer;
    recruitedHero->m_remainingMobility = previousMobility;
    return m_recruitState != -1;
}

MessageDispatchResult TavernHandler(tag_message& message) {

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_NOTIFY_DESELECT:
                switch (message.payload.widget.id) {
                    case DIALOG_BUTTON_0:
                    case DIALOG_BUTTON_1:
                    case TOWN_DIALOG_CONFIRM:
                        FINISH_DIALOG_MESSAGE(message);
                        return MESSAGE_DISPATCH_FORWARD;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    if (glTimers[0] < platform::Ticks()) {
        SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FRAME, TAVERN_ANIMATION_CONTROL);
        ++gpGame->m_viewArmyResult;
        message.payload.widget.data.value =
            gpGame->m_viewArmyResult % TOWN_TAVERN_ANIMATION_FRAME_COUNT
            + TOWN_TAVERN_FIRST_ANIMATION_FRAME;
        gpTownManager->m_heroWindow0->BroadcastMessage(message);
        gpTownManager->m_heroWindow0->MoveWindow(0, 0);
        glTimers[0] = static_cast<i32>(platform::Ticks() + TOWN_TAVERN_ANIMATION_DELAY);
    }
    return MESSAGE_DISPATCH_CONSUME;
}

void townManager::DoTavern(void) {

    tag_message message;

    m_heroWindow0 = new heroWindow(TAVERN_WINDOW_X, TAVERN_WINDOW_Y, "tavwin.bin");
    if (m_heroWindow0 == NULL)
        MemError();
    SetWinText(m_heroWindow0, TAVERN_WINDOW_TEXT_ID);
    utf8::Format(
        gText, GLOBAL_TEXT_BUFFER_SIZE,
        localization::Tr("town.tavern.rumor")

        ,
        gpGame->m_rumour
    );
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, TOWN_TAVERN_RUMOUR_CONTROL);
    message.payload.widget.data.text = gText;
    m_heroWindow0->BroadcastMessage(message);
    gpWindowManager->DoDialog(m_heroWindow0, TavernHandler, 0);
    delete m_heroWindow0;
}

MessageDispatchResult SplitArmyHandler(tag_message& message) {

    b32 handled = false;

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_NOTIFY_SELECT:
                switch (message.payload.widget.id) {
                    case TOWN_SPLIT_AMOUNT_CONTROL:
                        message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                        gpTownManager->m_heroWindow1->BroadcastMessage(message);
                        gpTownManager->m_splitAmount = atoi(message.payload.widget.data.text);
                        if (gpTownManager->m_splitAmount < 0)
                            gpTownManager->m_splitAmount = 0;
                        if (gpTownManager->m_splitAmount >= gpTownManager->m_splitMaximum)
                            gpTownManager->m_splitAmount = gpTownManager->m_splitMaximum - 1;
                        goto update_amount;
                }
                break;
            case WIDGET_NOTIFY_DESELECT:
                switch (message.payload.widget.id) {
                    case TOWN_SPLIT_INCREASE_CONTROL:
                        ++gpTownManager->m_splitAmount;
                        if (gpTownManager->m_splitAmount >= gpTownManager->m_splitMaximum)
                            gpTownManager->m_splitAmount = gpTownManager->m_splitMaximum - 1;
                        goto update_amount;
                    case TOWN_SPLIT_DECREASE_CONTROL:
                        --gpTownManager->m_splitAmount;
                        if (gpTownManager->m_splitAmount < 0)
                            gpTownManager->m_splitAmount = 0;
                        goto update_amount;
                    case DIALOG_BUTTON_0:
                    case DIALOG_BUTTON_1:
                        gpTownManager->m_splitAmount = 0;
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        handled = true;
                        break;
                    case TOWN_DIALOG_CONFIRM:
                        if (gpTownManager->m_splitAmount == 0)
                            gpWindowManager->m_dialogResult = H2EnumIndex(DIALOG_CANCEL_ID);
                        else
                            gpWindowManager->m_dialogResult = H2EnumIndex(TOWN_DIALOG_CONFIRM);
                        handled = true;
                        break;
                    default:
                        break;
                }
                break;
        }
    }

    if (handled == 1) {
        message.payload.widget.id = H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT);
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;

update_amount:
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%d", gpTownManager->m_splitAmount);
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, TOWN_SPLIT_AMOUNT_CONTROL);
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    gpTownManager->m_heroWindow1->DrawWindow(
        WINDOW_DRAW_UPDATE_SCREEN,
        TOWN_SPLIT_AMOUNT_CONTROL,
        TOWN_SPLIT_AMOUNT_CONTROL
    );
    return MESSAGE_DISPATCH_CONSUME;
}

void townManager::SetupWell(heroWindow* window) {

    u8 dwellingTypes[TOWN_WELL_DWELLING_COUNT];
    i32 available;
    i32 dwellingResult;
    tag_message message;
    char iconName[TOWN_OBJECT_FILENAME_SIZE];
    char detailText[WELL_DETAIL_TEXT_CAPACITY];
    tag_monsterInfo monsterInfo;
    i32 creatureGrowth;

    for (dwellingResult = 0; dwellingResult < TOWN_WELL_DWELLING_COUNT; ++dwellingResult) {
        if (dwellingResult == TOWN_WELL_DWELLING_COUNT - 1
            && (m_town->m_buildings & (1L << TOWN_WELL_LAST_UPGRADE_BUILDING))) {
            dwellingTypes[dwellingResult] = WELL_ALTERNATE_UPGRADE_INDEX;
        } else if (dwellingResult >= 1
                   && (m_town->m_buildings
                       & (1L << (dwellingResult + TOWN_WELL_FIRST_UPGRADE_BUILDING)))) {
            dwellingTypes[dwellingResult] =
                dwellingResult + TOWN_WELL_FIRST_UPGRADE_OFFSET;
        } else {
            dwellingTypes[dwellingResult] = dwellingResult;
        }
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    utf8::Format(iconName, "cstl%s.icn", cHeroTypeShortName[H2EnumIndex(m_town->m_type)]);
    message.payload.widget.data.text = iconName;
    for (dwellingResult = 0; dwellingResult < TOWN_WELL_DWELLING_COUNT; ++dwellingResult) {
        message.payload.widget.id = dwellingResult + 1;
        window->BroadcastMessage(message);
    }

    for (dwellingResult = 0; dwellingResult < TOWN_WELL_DWELLING_COUNT; ++dwellingResult) {
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = dwellingResult + 1;
        message.payload.widget.data.value =
            dwellingTypes[dwellingResult] + H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST);
        window->BroadcastMessage(message);
        utf8::Format(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
            "monh%04d.icn",
            H2EnumIndex(gDwellingType[H2EnumIndex(m_town->m_type)][dwellingTypes[dwellingResult]])
        );
        message.payload.widget.command = WIDGET_COMMAND_SET_ICON;
        message.payload.widget.id = dwellingResult + TOWN_WELL_FIRST_MONSTER_ICON_CONTROL;
        message.payload.widget.data.text = gText;
        window->BroadcastMessage(message);
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    for (dwellingResult = 0; dwellingResult < TOWN_WELL_DWELLING_COUNT; ++dwellingResult) {
        utf8::Copy(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
            GetBuildingName(
                m_town->m_type,
                BuildingSlotType(
                    dwellingTypes[dwellingResult] + H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)
                )
            )
        );
        message.payload.widget.id = dwellingResult + TOWN_WELL_FIRST_NAME_CONTROL;
        message.payload.widget.data.text = gText;
        window->BroadcastMessage(message);

        if (m_town->m_buildings
            & (1L << (dwellingTypes[dwellingResult] + H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)))) {
            available = m_town->m_garrison[dwellingTypes[dwellingResult]];
            utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("town.well.available")  );
            message.payload.widget.id = dwellingResult + TOWN_WELL_FIRST_AVAILABLE_CONTROL;
            message.payload.widget.data.text = gText;
            window->BroadcastMessage(message);
            utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%d", available);
            message.payload.widget.id = dwellingResult + TOWN_WELL_FIRST_AVAILABLE_COUNT_CONTROL;
            message.payload.widget.data.text = gText;
            window->BroadcastMessage(message);
        }

        message.payload.widget.id = dwellingResult + TOWN_WELL_FIRST_CREATURE_CONTROL;
        strcpy(
            gText,
            gArmyNames[H2EnumIndex(gDwellingType[H2EnumIndex(m_town->m_type)][dwellingTypes[dwellingResult]])]
        );
        utf8::UppercaseFirst(gText);
        message.payload.widget.data.text = gText;
        window->BroadcastMessage(message);
    }

    for (dwellingResult = 0; dwellingResult < TOWN_WELL_DWELLING_COUNT; ++dwellingResult) {
        monsterInfo = gMonsterDatabase[H2EnumIndex(gDwellingType[H2EnumIndex(m_town->m_type)][dwellingTypes[dwellingResult]])];
        strcpy(
            gText,
            ""
        );
        utf8::Format(detailText, "%s%d", cWellDetail[WELL_DETAIL_ATTACK], monsterInfo.attack);
        strcat(gText, detailText);
        utf8::Format(detailText, "\n%s%d", cWellDetail[WELL_DETAIL_DEFENSE], monsterInfo.defense);
        strcat(gText, detailText);
        utf8::Format(detailText, "\n%s%d", cWellDetail[WELL_DETAIL_DAMAGE], monsterInfo.damageMin);
        strcat(gText, detailText);
        if (monsterInfo.damageMin != monsterInfo.damageMax) {
            utf8::Format(detailText, "-%d", monsterInfo.damageMax);
            strcat(gText, detailText);
        }
        utf8::Format(detailText, "\n%s%d", cWellDetail[WELL_DETAIL_HIT_POINTS], monsterInfo.hitPoints);
        strcat(gText, detailText);
        utf8::Format(detailText, cWellDetail[WELL_DETAIL_SPEED], speedText[monsterInfo.speed]);
        strcat(gText, detailText);
        if (m_town->m_buildings
            & (1L << (dwellingTypes[dwellingResult] + H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)))) {
            creatureGrowth = gMonsterDatabase[H2EnumIndex(gDwellingType[H2EnumIndex(m_town->m_type)]
                                                        [dwellingTypes[dwellingResult]])]
                          .growth;
            if (!ironfist::IsWellDisabled())
                creatureGrowth += TOWN_WELL_BASE_GROWTH_BONUS;
            if (dwellingResult == 0
                && (m_town->m_buildings & (1L << TOWN_WELL_FIRST_DWELLING_GROWTH_BUILDING)))
                creatureGrowth += TOWN_WELL_FIRST_DWELLING_GROWTH_BONUS;
            utf8::Format(detailText, cWellDetail[WELL_DETAIL_GROWTH], creatureGrowth);
            strcat(gText, detailText);
        }
        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message.payload.widget.id = dwellingResult + TOWN_WELL_FIRST_DETAIL_CONTROL;
        message.payload.widget.data.text = gText;
        window->BroadcastMessage(message);
    }
}

void townManager::SetupThievesGuild(heroWindow* window, i32 informationLevel) {

    TownThievesGuildCategory category;
    i8 categoryOrder[GAME_PLAYER_COUNT];
    i32 rank;
    i32 tiedCount;
    i32 rankX;
    i32 lastAtRank;
    i32 firstAtRank;
    widget* iconControl;
    char* widgetText;
    i32 maxCategories;
    i32 strongestHeroValue;
    char statText[THIEVES_STAT_TEXT_CAPACITY];
    i32 armySlot;
    i32l categoryStats[GAME_PLAYER_COUNT];
    i32 heroPosition;
    i32 heroValue;
    i32 strongestCreatureValue;
    i32 strongestHeroPosition;
    hero* strongestHero;

    town* playerTown;

    i32 position;
    tag_message message;
    widget* textControl;
    CreatureType strongestCreature;

    if (informationLevel == -1)
        informationLevel = gpGame->GetNumThievesGuilds(giCurPlayer);

    if (informationLevel >= TOWN_THIEVES_INFO_ALL_CATEGORIES) {
        maxCategories = TOWN_THIEVES_MAX_CATEGORIES;
    } else {
        if (informationLevel == TOWN_THIEVES_INFO_STRONGEST_CREATURE)
            maxCategories = TOWN_THIEVES_CREATURE_CATEGORY_COUNT;
        else if (informationLevel == TOWN_THIEVES_INFO_PERSONALITY)
            maxCategories = TOWN_THIEVES_PERSONALITY_CATEGORY_COUNT;
        else if (informationLevel == TOWN_THIEVES_INFO_PRIMARY_STATS)
            maxCategories = TOWN_THIEVES_PRIMARY_CATEGORY_COUNT;
        else
            maxCategories = TOWN_THIEVES_BASIC_CATEGORY_COUNT;
    }

    for (position = gpGame->m_playerCount - gpGame->m_deadPlayerCount;
         position < GAME_PLAYER_COUNT;
         ++position) {
        SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_CLEAR_FLAGS, position + TOWN_THIEVES_FIRST_RANK_CONTROL);
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        window->BroadcastMessage(message);
        message.payload.widget.id = position + TOWN_THIEVES_FIRST_PLAYER_CONTROL;
        window->BroadcastMessage(message);
    }
    for (position = gpGame->m_playerCount - gpGame->m_deadPlayerCount;
         position < GAME_PLAYER_COUNT;
         ++position) {
        SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_CLEAR_FLAGS, position + TOWN_THIEVES_FIRST_PLAYER_CONTROL);
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        window->BroadcastMessage(message);
    }

    for (category = THIEVES_CATEGORY_TOWNS; !(H2EnumIndex(category) >= maxCategories);
         ++category) {
        GetCategoryStats(category, categoryStats, categoryOrder);
        SortStats(categoryStats, categoryOrder);
        firstAtRank = 0;
        lastAtRank = 0;
        for (rank = 0; rank < GAME_PLAYER_COUNT; ++rank) {
            if (firstAtRank == gpGame->m_playerCount - gpGame->m_deadPlayerCount)
                break;
            tiedCount = 1;
            while (lastAtRank + 1 < gpGame->m_playerCount
                   && categoryStats[lastAtRank + 1] == categoryStats[lastAtRank]) {
                ++tiedCount;
                ++lastAtRank;
            }
            rankX = THIEVES_PLAYER_COLUMN_WIDTH * rank + THIEVES_RANK_FIRST_X
                      - (tiedCount - 1) * THIEVES_TIE_CENTERING_STEP;
            for (position = firstAtRank; !(position > lastAtRank); ++position) {
                iconControl = new iconWidget(
                    static_cast<i16>(
                        rankX + (position - firstAtRank) * THIEVES_RANK_ICON_WIDTH
                    ),
                    static_cast<i16>(
                        H2EnumIndex(category) * THIEVES_CATEGORY_ROW_HEIGHT + THIEVES_FIRST_CATEGORY_Y
                    ),
                    THIEVES_RANK_ICON_WIDTH,
                    THIEVES_RANK_ICON_HEIGHT,
                    "townwind.icn",
                    static_cast<i16>(
                        gpGame->m_players[categoryOrder[position]].m_color
                        + TOWN_THIEVES_RANK_ICON_FRAME_BASE
                    ),
                    ICON_DRAW_NORMAL,
                    -1,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
                if (iconControl == NULL)
                    MemError();
                window->AddWidget(iconControl, -1);
            }
            ++lastAtRank;
            firstAtRank = lastAtRank;
        }
    }

    rank = 0;
    for (position = 0; position < gpGame->m_playerCount - gpGame->m_deadPlayerCount;
         ++position) {
        while (gpGame->m_playerDead[rank] != 0)
            ++rank;
        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, gColors[gpGame->m_players[rank].m_color]);
        utf8::UppercaseFirst(gText);
        SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, position + TOWN_THIEVES_FIRST_PLAYER_CONTROL);
        message.payload.widget.data.text = gText;
        window->BroadcastMessage(message);

        if (informationLevel < TOWN_THIEVES_INFO_STRONGEST_HERO)
            goto nextRank;
        {
            strongestHeroPosition = -1;
            strongestHeroValue = 0;
            for (heroPosition = 0; heroPosition < gpGame->m_players[rank].m_heroCount;
                 ++heroPosition) {
                strongestHero = gpGame->GetPlayerHero(rank, heroPosition);
                heroValue =
                    gpPhilAI
                        ->FightValueOfStack(&strongestHero->m_army, strongestHero, 0, 0, 0, 0);
                if (!(heroValue <= strongestHeroValue)) {
                    strongestHeroValue = heroValue;
                    strongestHeroPosition = heroPosition;
                }
            }

            if (strongestHeroPosition != -1) {
                iconControl = new iconWidget(
                    static_cast<i16>(
                        position * THIEVES_PLAYER_COLUMN_WIDTH + THIEVES_HERO_LOCATOR_X
                    ),
                    THIEVES_HERO_LOCATOR_Y,
                    0,
                    0,
                    "locators.icn",
                    THIEVES_HERO_LOCATOR_FRAME,
                    ICON_DRAW_NORMAL,
                    -1,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
                if (iconControl == NULL)
                    MemError();
                window->AddWidget(iconControl, -1);
                iconControl = new iconWidget(
                    static_cast<i16>(
                        position * THIEVES_PLAYER_COLUMN_WIDTH + THIEVES_HERO_PORTRAIT_X
                    ),
                    THIEVES_HERO_Y,
                    0,
                    0,
                    "miniport.icn",
                    static_cast<i16>(
                        gpGame->GetPlayerHero(rank, strongestHeroPosition)->m_portrait
                    ),
                    ICON_DRAW_NORMAL,
                    -1,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
                if (iconControl == NULL)
                    MemError();
                window->AddWidget(iconControl, -1);
            }

            if (informationLevel < TOWN_THIEVES_INFO_PRIMARY_STATS)
                goto nextRank;
            {
                if (strongestHeroPosition != -1) {
                    strongestHero = gpGame->GetPlayerHero(rank, strongestHeroPosition);
                    utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("town.thieves_guild.primary_stats")  );
                    ALLOC_COPY_STRING(widgetText, gText);
                    textControl = new textWidget(
                        static_cast<i16>(
                            position * THIEVES_PLAYER_COLUMN_WIDTH + THIEVES_PRIMARY_LABEL_X
                        ),
                        THIEVES_PRIMARY_STATS_Y,
                        THIEVES_PRIMARY_LABEL_WIDTH,
                        THIEVES_PRIMARY_WIDGET_HEIGHT,
                        widgetText,
                        "smalfont.fnt",
                        FONT_DRAW_DEFAULT,
                        -1,
                        WIDGET_KIND_TEXT,
                        FONT_ALIGN_LEFT
                    );
                    window->AddWidget(textControl, -1);

                    gText[0] = 0;
                    for (heroPosition = 0;
                         heroPosition < HERO_PRIMARY_STAT_COUNT;
                         ++heroPosition) {
                        utf8::Format(
                            statText,
                            "%d\n",
                            strongestHero->Stats(HeroPrimaryStat(heroPosition))
                        );
                        strcat(gText, statText);
                    }
                    ALLOC_COPY_STRING(widgetText, gText);
                    textControl = new textWidget(
                        static_cast<i16>(
                            position * THIEVES_PLAYER_COLUMN_WIDTH + THIEVES_PRIMARY_VALUE_X
                        ),
                        THIEVES_PRIMARY_STATS_Y,
                        THIEVES_PRIMARY_VALUE_WIDTH,
                        THIEVES_PRIMARY_WIDGET_HEIGHT,
                        widgetText,
                        "smalfont.fnt",
                        FONT_DRAW_DEFAULT,
                        -1,
                        WIDGET_KIND_TEXT,
                        FONT_ALIGN_LEFT
                    );
                    window->AddWidget(textControl, -1);
                }

                if (informationLevel < TOWN_THIEVES_INFO_PERSONALITY)
                    goto nextRank;
                {
                    strcpy(gText, cPersonality[H2EnumIndex(gpGame->m_players[rank].m_aiDifficulty)]);
                    ALLOC_COPY_STRING(widgetText, gText);
                    textControl = new textWidget(
                        static_cast<i16>(
                            position * THIEVES_PLAYER_COLUMN_WIDTH + THIEVES_PERSONALITY_X
                        ),
                        THIEVES_PERSONALITY_TEXT_Y,
                        THIEVES_PERSONALITY_WIDTH,
                        THIEVES_PERSONALITY_HEIGHT,
                        widgetText,
                        "smalfont.fnt",
                        FONT_DRAW_DEFAULT,
                        -1,
                        WIDGET_KIND_UNDIMMED,
                        FONT_ALIGN_CENTER
                    );
                    window->AddWidget(textControl, -1);

                    if (informationLevel < TOWN_THIEVES_INFO_STRONGEST_CREATURE)
                        goto nextRank;
                    {
                        strongestCreature = CREATURE_NONE;
                        strongestCreatureValue = 0;
                        for (heroPosition = 0;
                             heroPosition < gpGame->m_players[rank].m_townCount;
                             ++heroPosition) {
                            playerTown = gpGame->GetPlayerTown(rank, heroPosition);
                            for (armySlot = 0; armySlot < ARMY_GROUP_SLOT_COUNT;
                                 ++armySlot) {
                                if (ARMY_GROUP_HAS_POSITIVE_STACK(playerTown->m_army, armySlot)
                                    && gMonsterDatabase[H2EnumIndex(playerTown->m_army
                                                                .m_creatureTypes[armySlot])]
                                               .fightValue
                                           > strongestCreatureValue) {
                                    strongestCreature =
                                        playerTown->m_army.m_creatureTypes[armySlot];
                                    strongestCreatureValue =
                                        gMonsterDatabase[H2EnumIndex(playerTown->m_army
                                                                 .m_creatureTypes[armySlot])]
                                            .fightValue;
                                }
                            }
                        }
                        for (heroPosition = 0;
                             heroPosition < gpGame->m_players[rank].m_heroCount;
                             ++heroPosition) {
                            strongestHero = gpGame->GetPlayerHero(rank, heroPosition);
                            for (armySlot = 0; armySlot < ARMY_GROUP_SLOT_COUNT;
                                 ++armySlot) {
                                if (ARMY_GROUP_HAS_POSITIVE_STACK(
                                        strongestHero->m_army,
                                        armySlot
                                    )
                                    && gMonsterDatabase[H2EnumIndex(strongestHero->m_army
                                                                .m_creatureTypes[armySlot])]
                                               .fightValue
                                           > strongestCreatureValue) {
                                    strongestCreature =
                                        strongestHero->m_army.m_creatureTypes[armySlot];
                                    strongestCreatureValue =
                                        gMonsterDatabase[H2EnumIndex(strongestHero->m_army
                                                                 .m_creatureTypes[armySlot])]
                                            .fightValue;
                                }
                            }
                        }
                        if (strongestCreature != CREATURE_NONE) {
                            iconControl = new iconWidget(
                                static_cast<i16>(
                                    position * THIEVES_PLAYER_COLUMN_WIDTH
                                    + THIEVES_CREATURE_X
                                ),
                                THIEVES_CREATURE_Y,
                                THIEVES_CREATURE_WIDTH,
                                THIEVES_CREATURE_HEIGHT,
                                "mons32.icn",
                                static_cast<i16>(strongestCreature),
                                ICON_DRAW_NORMAL,
                                -1,
                                WIDGET_KIND_ICON_CENTERED,
                                1
                            );
                            if (iconControl == NULL)
                                MemError();
                            window->AddWidget(iconControl, -1);
                        }
                    }
                }
            }
        }
    nextRank:
        ++rank;
    }
}

void GetCategoryStats(
    TownThievesGuildCategory category,
    i32l* const stats,
    i8* const order
) {
    i32 player;
    i32 townIndex;
    hero* playerHero;
    i32 heroIndex;
    i32 townCount;
    i32 castleCount;
    i32 armyStrength;
    town* playerTown;

    for (player = 0; player < gpGame->m_playerCount; ++player) {
        townCount = 0;
        castleCount = 0;
        order[player] = static_cast<i8>(player);
        if (gpGame->m_playerDead[player]) {
            stats[player] = TOWN_THIEVES_DEAD_PLAYER_STAT;
        } else {
            switch (category) {
                case THIEVES_CATEGORY_TOWNS:
                    for (townIndex = 0; townIndex < GAME_TOWN_COUNT; ++townIndex) {
                        if (gpGame->m_castleRecs[townIndex].m_owner == player
                            && (gpGame->m_castleRecs[townIndex].m_buildings
                                & H2EnumIndex(TOWN_BUILDING_TENT))) {
                            ++townCount;
                        }
                    }
                    stats[player] = townCount;
                    break;
                case THIEVES_CATEGORY_CASTLES:
                    for (townIndex = 0; townIndex < GAME_TOWN_COUNT; ++townIndex) {
                        if (gpGame->m_castleRecs[townIndex].m_owner == player
                            && (gpGame->m_castleRecs[townIndex].m_buildings
                                & H2EnumIndex(TOWN_BUILDING_CASTLE))) {
                            ++castleCount;
                        }
                    }
                    stats[player] = castleCount;
                    break;
                case THIEVES_CATEGORY_HEROES:
                    stats[player] = gpGame->m_players[player].m_heroCount;
                    break;
                case THIEVES_CATEGORY_GOLD:
                    stats[player] = gpGame->m_players[player].m_resources[H2EnumIndex(RES_GOLD)];
                    break;
                case THIEVES_CATEGORY_WOOD_AND_ORE:
                    stats[player] = gpGame->m_players[player].m_resources[H2EnumIndex(RES_WOOD)]
                                    + gpGame->m_players[player].m_resources[H2EnumIndex(RES_ORE)];
                    break;
                case THIEVES_CATEGORY_RARE_RESOURCES:
                    stats[player] = gpGame->m_players[player].m_resources[H2EnumIndex(RES_GEMS)]
                                    + gpGame->m_players[player].m_resources[H2EnumIndex(RES_CRYSTAL)]
                                    + gpGame->m_players[player].m_resources[H2EnumIndex(RES_SULFUR)]
                                    + gpGame->m_players[player].m_resources[H2EnumIndex(RES_MERCURY)];
                    break;
                case THIEVES_CATEGORY_OBELISKS:
                    stats[player] = GetNumObelisks(player);
                    break;
                case THIEVES_CATEGORY_ARTIFACTS:
                    stats[player] = 0;
                    for (townIndex = 0; townIndex < gpGame->m_players[player].m_heroCount;
                         ++townIndex) {
                        playerHero =
                            gpGame->GetHero(gpGame->m_players[player].m_heroIds[townIndex]);
                        for (heroIndex = 0; heroIndex < HERO_ARTIFACT_SLOT_COUNT; ++heroIndex) {
                            if (playerHero->m_artifacts[heroIndex] != ARTIFACT_NONE
                                && playerHero->m_artifacts[heroIndex] != ARTIFACT_MAGIC_BOOK) {
                                ++stats[player];
                            }
                        }
                    }
                    break;
                case THIEVES_CATEGORY_ARMY_STRENGTH:
                    armyStrength = 0;
                    for (heroIndex = 0; heroIndex < gpGame->m_players[player].m_heroCount;
                         ++heroIndex) {
                        playerHero = gpGame->GetPlayerHero(player, heroIndex);
                        armyStrength += gpPhilAI->FightValueOfStack(
                            &playerHero->m_army,
                            playerHero,
                            0,
                            0,
                            0,
                            0
                        );
                    }
                    for (heroIndex = 0; heroIndex < gpGame->m_players[player].m_townCount;
                         ++heroIndex) {
                        playerTown = gpGame->GetPlayerTown(player, heroIndex);
                        if (playerTown->HasGarrison()) {
                            armyStrength +=
                                gpPhilAI->FightValueOfStack(&playerTown->m_army, NULL, 0, 0, 0, 0);
                        }
                    }
                    stats[player] = armyStrength;
                    break;
                case THIEVES_CATEGORY_INCOME:
                    stats[player] = gpGame->ComputeDailyGold(player);
                    break;
            }
        }
    }
}

void SortStats(i32l* const stats, i8* const order) {
    i32 temporaryOrder;
    i32 secondPlayer;
    i32 firstPlayer;
    i32l tempStat;

    for (firstPlayer = 0; firstPlayer < gpGame->m_playerCount - 1; ++firstPlayer) {
        for (secondPlayer = firstPlayer + 1; secondPlayer < gpGame->m_playerCount; ++secondPlayer) {
            if (stats[secondPlayer] > stats[firstPlayer]) {
                tempStat = stats[firstPlayer];
                stats[firstPlayer] = stats[secondPlayer];
                stats[secondPlayer] = tempStat;
                temporaryOrder = order[firstPlayer];
                order[firstPlayer] = order[secondPlayer];
                order[secondPlayer] = static_cast<i8>(temporaryOrder);
            }
        }
    }
}
