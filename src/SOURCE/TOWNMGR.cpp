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


namespace {

    typedef enum TownManagerInputCode {
        CONTROL_PREVIOUS_TOWN = 0x387,
        CONTROL_NEXT_TOWN = 0x388,
        CONTROL_CLOSE = 0x7800,
        DIALOG_BUILD_BOAT = 0x7802,
        DIALOG_BUY_SPELL_BOOK = 0x7805
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
        TOWN_WIDGET_GARRISON_LAST = TOWN_GARRISON_SLOT_LAST,
        TOWN_WIDGET_HERO_CONTROL = TOWN_HERO_FIRST_CONTROL,
        TOWN_WIDGET_HERO_FIRST = TOWN_HERO_SLOT_FIRST,
        TOWN_WIDGET_HERO_LAST = TOWN_HERO_SLOT_LAST,
        TOWN_WIDGET_EMPTY_FIRST = TOWN_EMPTY_STATUS_CONTROL_FIRST,
        TOWN_WIDGET_EMPTY_LAST = TOWN_EMPTY_STATUS_CONTROL_LAST,
        TOWN_WIDGET_CLOSE = CONTROL_CLOSE
    };
using enum TownManagerWidgetId;
    ENABLE_ENUM_STEPS(TownManagerWidgetId)

    enum class TownObjectRenderMask : i32 {
        RENDER_KNIGHT_LEFT_GATE = 0x4,
        RENDER_KNIGHT_LEFT_FIRST_OPTION = 0x1,
        RENDER_KNIGHT_LEFT_SECOND_OPTION = 0x1000000,
        RENDER_KNIGHT_LEFT_THIRD_OPTION = 0x20000000,
        RENDER_KNIGHT_RIGHT_GATE = 0x1,
        RENDER_KNIGHT_RIGHT_FIRST_OPTION = 0x1000000,
        RENDER_KNIGHT_RIGHT_SECOND_OPTION = 0x20000000,
        RENDER_BARBARIAN_RIGHT_GATE = 0x1,
        RENDER_BARBARIAN_RIGHT_OPTION = 0x2000,
        RENDER_BARBARIAN_OVERLAY_GATE = 0x2,
        RENDER_BARBARIAN_OVERLAY_OPTION = 0x200000,
        RENDER_SORCERESS_LEFT_GATE = 0x80,
        RENDER_SORCERESS_LEFT_OPTION = 0x800,
        RENDER_RACE_OVERLAY_FIRST_OPTION = 0x4000,
        RENDER_RACE_OVERLAY_SECOND_OPTION = 0x8,
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

    enum class TownDialogResult : i32 {
        DIALOG_CANCEL_ID = 0x7801
    };
using enum TownDialogResult;

    enum class TownPortraitIcon : i32 {
        TOWN_PORTRAIT_ICON_BASE = 90
    };
using enum TownPortraitIcon;

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
        BUILD_RESOURCE_STORAGE_COUNT = TOWN_RESOURCE_COUNT + 1,
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
        WELL_DWELLING_TYPE_STORAGE_COUNT = 8,
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

    typedef enum Cp1251Letter {
        CP1251_CAPITAL_YO = 0xa8,
        CP1251_SMALL_YO   = 0xb8,
        CP1251_SMALL_A    = 0xe0,
        CP1251_SMALL_YA   = 0xff,
        CP1251_CASE_STEP  = 0x20
    } Cp1251Letter;


    inline char ToUpperCp1251(u8 letter) {
        char capital;

        if (letter >= 'a' && letter <= 'z')
            capital = letter - CP1251_CASE_STEP;
        else if (letter >= CP1251_SMALL_A && letter <= CP1251_SMALL_YA)
            capital = letter - CP1251_CASE_STEP;
        else if (letter == CP1251_SMALL_YO)
            capital = CP1251_CAPITAL_YO;
        else
            capital = letter;
        return capital;
    }

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
    gTownObjectOrder[KB_FACTION_TABLE_CAPACITY][TOWN_BUILDING_COUNT] = {
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

SBuildingInfo sBuildingInfo[KB_FACTION_TABLE_CAPACITY][TOWN_BUILDING_COUNT] = {
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
    i32 tempY;
    i32 w;
    i32 h;
    H2EnumStorage<BuildingSlotType, i32> id_h;

    m_animationFrame = 0;
    m_icon = NULL;
    m_border = NULL;
    m_visible = 1;
    m_animationFrameCount = sBuildingInfo[H2EnumIndex(townType)][H2EnumIndex(buildingId)].animationFrameCount;
    x = sBuildingInfo[H2EnumIndex(townType)][H2EnumIndex(buildingId)].x;
    y = sBuildingInfo[H2EnumIndex(townType)][H2EnumIndex(buildingId)].y;
    w = sBuildingInfo[H2EnumIndex(townType)][H2EnumIndex(buildingId)].width;
    h = sBuildingInfo[H2EnumIndex(townType)][H2EnumIndex(buildingId)].height;
    id_h = buildingId;
    m_buildingId = id_h;
    sprintf(name, "%s.icn", iconBaseName);
    m_icon = gpResourceManager->GetIcon(name);
    if (id_h != TOWN_OBJECT_NONE) {
        m_border = new border(
            x,
            y,
            w,
            h,
            static_cast<i16>(H2EnumIndex(id_h)),
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
        && (!(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_KNIGHT_LEFT_GATE))
            || (!(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_KNIGHT_LEFT_FIRST_OPTION))
                && !(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_KNIGHT_LEFT_SECOND_OPTION))
                && !(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_KNIGHT_LEFT_THIRD_OPTION)))))
        return;
    if (m_buildingId == TOWN_OBJECT_KNIGHT_RIGHT_OVERLAY
        && gpTownManager->m_town->m_type == FACTION_KNIGHT
        && (!(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_KNIGHT_RIGHT_GATE))
            || (!(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_KNIGHT_RIGHT_FIRST_OPTION))
                && !(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_KNIGHT_RIGHT_SECOND_OPTION)))))
        return;
    if (m_buildingId == TOWN_OBJECT_KNIGHT_RIGHT_OVERLAY
        && gpTownManager->m_town->m_type == FACTION_BARBARIAN
        && (!(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_BARBARIAN_RIGHT_GATE))
            || !(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_BARBARIAN_RIGHT_OPTION))))
        return;
    if (m_buildingId == TOWN_OBJECT_BARBARIAN_OVERLAY
        && gpTownManager->m_town->m_type == FACTION_BARBARIAN
        && (!(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_BARBARIAN_OVERLAY_GATE))
            || !(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_BARBARIAN_OVERLAY_OPTION))))
        return;
    if (gpTownManager->m_town->m_type == FACTION_SORCERESS
        && m_buildingId == TOWN_OBJECT_KNIGHT_LEFT_OVERLAY
        && (!(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_SORCERESS_LEFT_GATE))
            || !(gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_SORCERESS_LEFT_OPTION))))
        return;
    if (gpTownManager->m_town->m_type == FACTION_SORCERESS
        && (m_buildingId == TOWN_OBJECT_SORCERESS_LEFT_OVERLAY
            || m_buildingId == TOWN_OBJECT_SORCERESS_RIGHT_OVERLAY)
        && (gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_SORCERESS_LEFT_GATE))
        && (gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_SORCERESS_LEFT_OPTION)))
        return;
    if (m_buildingId == TOWN_OBJECT_RACE_OVERLAY
        && (gpTownManager->m_town->m_type == FACTION_NECROMANCER
            || gpTownManager->m_town->m_type == FACTION_WARLOCK
            || gpTownManager->m_town->m_type == FACTION_SORCERESS
            || gpTownManager->m_town->m_type == FACTION_KNIGHT)
        && ((gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_RACE_OVERLAY_FIRST_OPTION))
            || (gpTownManager->m_town->m_buildings & H2EnumIndex(RENDER_RACE_OVERLAY_SECOND_OPTION))))
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
    m_castleDialogActive = 0;
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
    m_castleDialogActive = 0;
    m_recruitResult = 0;
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
    i32 crestFrame;

    sprintf(gText, GetTownName(m_town->m_id));
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = TOWN_WIDGET_SET_TEXT;
    message.payload.widget.id = TOWN_WINDOW_TEXT_CONTROL;
    message.payload.widget.data.text = gText;
    m_townWindow->BroadcastMessage(message);
    strcpy(gText, localization::Tr("town.screen.title"));
    message.payload.widget.id = TOWN_CONTROL_STATUS_TEXT;
    message.payload.widget.data.text = gText;
    m_townWindow->BroadcastMessage(message);
    m_townWindow->DrawWindow(0, TOWN_WINDOW_DRAW_WIDTH, TOWN_WINDOW_DRAW_RIGHT);

    if (gpCurPlayer->m_townCount == 1) {
        message.payload.widget.command = TOWN_WIDGET_DISABLE;
        message.payload.widget.data.value = TOWN_WIDGET_DISABLED_VALUE;
        message.payload.widget.id = CONTROL_PREVIOUS_TOWN;
        m_townWindow->BroadcastMessage(message);
        message.payload.widget.id = CONTROL_NEXT_TOWN;
        m_townWindow->BroadcastMessage(message);
        message.payload.widget.command = TOWN_WIDGET_ENABLE;
        message.payload.widget.data.value = TOWN_WIDGET_ENABLED_VALUE;
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
        sprintf(gText, "townbkg%d.icn", H2EnumIndex(m_town->m_type));
        m_backgroundIcon = gpResourceManager->GetIcon(gText);
        m_townObjectCount = 0;
        for (i = 0; i < TOWN_BUILDING_COUNT; ++i) {
            H2EnumStorage<BuildingSlotType, i32> buildId = gTownObjectOrder[H2EnumIndex(m_town->m_type)][i];
            if (buildId != TOWN_OBJECT_NONE) {
                sprintf(
                    gText,
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
        for (i = 0; i < TOWN_BUILDING_COUNT; ++i) {
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

    crestFrame = gpCurPlayer->m_color;
    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
        crestFrame *= CREST_PORTRAITS_PER_COLOR;
        crestFrame += H2EnumIndex(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
    } else {
        crestFrame += TOWN_EMPTY_HERO_PORTRAIT_OFFSET;
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
        sprintf(gText, "port%04d.icn", H2EnumIndex(gpGame->GetHero(m_town->m_occupyingHeroId)->m_portrait));
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
        sprintf(gText, "port%04d.icn", H2EnumIndex(m_town->m_type) + TOWN_PORTRAIT_FRAME_BASE);
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
    gpWindowManager->UpdateScreenRegion(0, 0, TOWN_SCREEN_WIDTH, TOWN_SCREEN_HEIGHT);
}

void townManager::UnloadTown(void) {
    i32 index_i;

    if (m_bankBox != NULL)
        delete m_bankBox;
    m_bankBox = NULL;
    if (m_heroStrip != NULL)
        delete m_heroStrip;
    m_heroStrip = NULL;
    if (m_garrisonStrip != NULL)
        delete m_garrisonStrip;
    m_garrisonStrip = NULL;
    for (index_i = 0; index_i < m_townObjectCount; ++index_i) {
        m_townWindow->RemoveWidget(m_townObjects[index_i]->m_border);
        delete m_townObjects[index_i];
        m_townObjects[index_i] = NULL;
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
    i32 cantMoveLastArmy;
    i32 sameType;

    m_command = ARMY_COMMAND_NONE;
    cantMoveLastArmy = 0;
    if (m_swapStrip->m_army->GetNumArmies() == 1 && &m_swapStrip[0] == m_heroStrip
        && m_pendingStrip != m_swapStrip)
        cantMoveLastArmy = 1;

    if (m_swapStrip == m_pendingStrip && m_swapArmySlot == m_pendingArmySlot) {
        sprintf(
            m_statusText,
            cTownCommand[H2EnumIndex(TEXT_VIEW_ARMY)],
            gArmyNamesPlural[H2EnumIndex(m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])]
        );
        m_command = ARMY_COMMAND_VIEW;
    } else {
        sameType = 0;
        if (m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]
            == m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])
            sameType = 1;
        if (sameType) {
            if (qualifier != 0) {
                sprintf(
                    m_statusText,
                    cTownCommand[H2EnumIndex(TEXT_REDISTRIBUTE_ARMY)],
                    gArmyNamesPlural[H2EnumIndex(m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])]
                );
                m_command = ARMY_COMMAND_SPLIT;
            } else if (cantMoveLastArmy) {
                strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_CANNOT_COMBINE_LAST_ARMY)]);
                return;
            } else {
                sprintf(
                    m_statusText,
                    cTownCommand[H2EnumIndex(TEXT_COMBINE_ARMIES)],
                    gArmyNamesPlural[H2EnumIndex(m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])]
                );
                m_command = ARMY_COMMAND_MERGE;
            }
        } else if (qualifier != 0
                   && m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] == CREATURE_NONE) {
            sprintf(
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
            sprintf(
                m_statusText,
                cTownCommand[H2EnumIndex(TEXT_MOVE_ARMY)],
                gArmyNamesPlural[H2EnumIndex(m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])]
            );
            m_command = ARMY_COMMAND_SWAP;
        }
    } else {
        sprintf(
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
    switch (static_cast<TownManagerWidgetId>(objectId)) {
        case TOWN_WIDGET_CLOSE:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_EXIT)]);
            break;
        case TOWN_WIDGET_NONE:
        case TOWN_WIDGET_EMPTY_FIRST:
        case TOWN_WIDGET_EMPTY_LAST:
            strcpy(m_statusText, cTownCommand[H2EnumIndex(TEXT_EMPTY_STATUS)]);
            break;
        case TOWN_WIDGET_GARRISON_CREST:
            sprintf(
                m_statusText,
                localization::Tr("calendar.date.status"),
                gpGame->m_month,
                gpGame->m_week,
                gpGame->m_day
            );
            break;
        case TOWN_WIDGET_GARRISON_FIRST:
        case TOWN_WIDGET_GARRISON_FIRST + 1:
        case TOWN_WIDGET_GARRISON_FIRST + 2:
        case TOWN_WIDGET_GARRISON_FIRST + 3:
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
                    sprintf(
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
        case TOWN_WIDGET_HERO_FIRST + 1:
        case TOWN_WIDGET_HERO_FIRST + 2:
        case TOWN_WIDGET_HERO_FIRST + 3:
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
                    sprintf(
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
            sprintf(
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

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = TOWN_WIDGET_SET_TEXT;
    message.payload.widget.id = TOWN_CONTROL_STATUS_TEXT;
    message.payload.widget.data.text = m_statusText;
    m_townWindow->BroadcastMessage(message);
    m_townWindow->DrawWindow(TOWN_STATUS_DRAW_LEFT, TOWN_STATUS_DRAW_WIDTH, TOWN_STATUS_DRAW_RIGHT);
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
    i32 quickView;
    char text[BUILDING_DESCRIPTION_CAPACITY];
    i32 dbgBuild;
    baseManager* manager;
    i32 status;
    i32 tradeCount;

    if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON))))
        quickView = 1;
    else
        quickView = 0;

    if (giDebugBuildingToBuild != -1) {
        dbgBuild = giDebugBuildingToBuild;
        giDebugBuildingToBuild = -1;
        if (dbgBuild == TOWN_DEBUG_BUILD_ALL) {
            for (loop = 0; loop < TOWN_BUILDING_COUNT; ++loop) {
                if ((gTownEligibleBuildMask[H2EnumIndex(m_town->m_type)]
                     & (1 << H2EnumIndex(static_cast<BuildingSlotType>(loop))))
                    || loop == H2EnumIndex(BUILDING_SLOT_CASTLE))
                    BuildObj(static_cast<BuildingSlotType>(loop));
            }
        } else {
            if ((gTownEligibleBuildMask[H2EnumIndex(m_town->m_type)]
                 & (1 << H2EnumIndex(static_cast<BuildingSlotType>(dbgBuild))))
                || dbgBuild == H2EnumIndex(BUILDING_SLOT_CASTLE))
                BuildObj(static_cast<BuildingSlotType>(dbgBuild));
        }
    }

    if (glTimers[0] < platform::Ticks()) {
        DrawTown(1, 1);
        glTimers[0] = platform::Ticks() + TOWN_REDRAW_INTERVAL;
    }

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_SELECT:
                case WIDGET_COMMAND_ALTERNATE_SELECT: {
                    switch (static_cast<TownManagerWidgetId>(message.payload.widget.id)) {
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
                                goto showBuildingInformation;
                            }
                            {
                                m_heroWindow0 = new heroWindow(0, 0, "caslwind.bin");
                                if (m_heroWindow0 == NULL)
                                    MemError();
                                SetupCastle(m_heroWindow0, 0);
                                m_castleDialogActive = 1;
                                m_recruitResult = 0;
                                gpWindowManager->DoDialog(m_heroWindow0, CastleHandler, 0);
                                m_castleDialogActive = 0;
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
                                    sprintf(gText, "port%04d.icn", H2EnumIndex(m_recruitHero->m_portrait));
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
                                    hero* townHero = gpGame->GetHero(m_town->m_occupyingHeroId);
                                    i32 width = TOWN_VIEW_FIZZLE_WIDTH;
                                    m_townWindow->DrawWindow(0);
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
                                    WaitEndSample(&buildSound, -1);
                                    m_recruitResult = 0;
                                    gpWindowManager->ReleaseFizzleSource();
                                } else {
                                    if (m_selectedBuilding == BUILDING_SLOT_NEUTRAL_LAST
                                        && m_town->m_occupyingHeroId == -1) {
                                        if (m_heroStrip != NULL)
                                            delete m_heroStrip;
                                        m_heroStrip = NULL;
                                        sprintf(
                                            gText,
                                            "port%04d.icn",
                                            H2EnumIndex(m_town->m_type) + H2EnumIndex(TOWN_PORTRAIT_ICON_BASE)
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
                                goto showBuildingInformation;
                            }
                            {
                                if (m_town->m_occupyingHeroId != -1
                                    && !gpGame->GetHero(m_town->m_occupyingHeroId)
                                            ->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                                    if (gpGame->GetHero(m_town->m_occupyingHeroId)->NumArtifacts()
                                        == TOWN_MAX_ARTIFACTS) {
                                        NormalDialog(
                                            localization::Tr("town.mage_guild.spell_book.no_artifact_space")  ,
                                            NORMAL_DIALOG_INFO,
                                            -1,
                                            -1,
                                            -1,
                                            0,
                                            -1,
                                            0,
                                            -1,
                                            0
                                        );
                                    } else if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)]
                                               < TOWN_SPELL_BOOK_COST) {
                                        NormalDialog(
                                            localization::Tr("town.mage_guild.spell_book.cannot_afford")  ,
                                            NORMAL_DIALOG_INFO,
                                            -1,
                                            -1,
                                            NORMAL_DIALOG_ARTIFACT,
                                            H2EnumIndex(ARTIFACT_MAGIC_BOOK),
                                            -1,
                                            0,
                                            -1,
                                            0
                                        );
                                    } else {
                                        NormalDialog(
                                            localization::Tr("town.mage_guild.spell_book.confirm_purchase")  ,
                                            NORMAL_DIALOG_CONFIRM,
                                            -1,
                                            -1,
                                            NORMAL_DIALOG_ARTIFACT,
                                            H2EnumIndex(ARTIFACT_MAGIC_BOOK),
                                            -1,
                                            0,
                                            -1,
                                            0
                                        );
                                        if (gpWindowManager->m_dialogResult
                                            == DIALOG_BUY_SPELL_BOOK) {
                                            GiveArtifact(
                                                gpGame->GetHero(m_town->m_occupyingHeroId),
                                                ARTIFACT_MAGIC_BOOK,
                                                1,
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
                                goto showBuildingInformation;
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
                                goto showBuildingInformation;
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
                                goto showBuildingInformation;
                            }
                            {
                                if (m_town->m_type == FACTION_NECROMANCER) {
                                    sprintf(
                                        text,
                                        GetBuildingInfo(
                                            m_town->m_type,
                                            static_cast<BuildingSlotType>(
                                                message.payload.widget.id
                                            ),
                                            1
                                        )
                                    );
                                    NormalDialog(
                                        text,
                                        NORMAL_DIALOG_INFO,
                                        -1,
                                        -1,
                                        H2EnumIndex(m_town->m_type) + BUILDING_DIALOG_ICON_FRAME_BASE,
                                        message.payload.widget.id,
                                        -1,
                                        0,
                                        -1,
                                        0
                                    );
                                } else {
                                    DoTavern();
                                }
                            }
                            break;

                        case TOWN_WIDGET_BUILDING_CASTLE_UPGRADE:
                            if (quickView) {
                                goto showBuildingInformation;
                            }
                            {
                                if (m_town->m_mayNotUpgradeToCastle != 0) {
                                    NormalDialog(
                                        localization::Tr("town.upgrade.castle_forbidden")  ,
                                        NORMAL_DIALOG_INFO,
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
                                goto showBuildingInformation;
                            }
                            {
                                gpWindowManager->BroadcastMessage(
                                    MESSAGE_WIDGET,
                                    WIDGET_COMMAND_SET_FLAGS,
                                    CONTROL_CLOSE,
                                    TOWN_INTERFACE_BROADCAST_FLAGS
                                );
                                if (gpGame->GetBoatsBuilt() < TOWN_MAX_BOATS
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
                                        message.type = MESSAGE_WIDGET;
                                        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
                                        message.payload.widget.id = DIALOG_BUILD_BOAT;
                                        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_GRAYED);
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
                                            i32 result = 0;
                                            LogStr(localization::Tr("town.boat.creation_failed")  );
                                        }
                                    }
                                } else {
                                    NormalDialog(
                                        localization::Tr("town.boat.limit_reached")  ,
                                        NORMAL_DIALOG_INFO,
                                        BOAT_LIMIT_DIALOG_X,
                                        BOAT_LIMIT_DIALOG_Y,
                                        -1,
                                        0,
                                        -1,
                                        0,
                                        -1,
                                        0
                                    );
                                }
                                gpWindowManager->BroadcastMessage(
                                    MESSAGE_WIDGET,
                                    WIDGET_COMMAND_CLEAR_FLAGS,
                                    CONTROL_CLOSE,
                                    TOWN_INTERFACE_BROADCAST_FLAGS
                                );
                            }
                            break;

                        case TOWN_WIDGET_BUILDING_MARKETPLACE:
                            if (quickView) {
                                goto showBuildingInformation;
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
                            showBuildingInformation:
                                sprintf(
                                    text,
                                    GetBuildingInfo(
                                        m_town->m_type,
                                        static_cast<BuildingSlotType>(message.payload.widget.id),
                                        1
                                    )
                                );
                                NormalDialog(
                                    text,
                                    NORMAL_DIALOG_QUICK_VIEW,
                                    -1,
                                    -1,
                                    H2EnumIndex(m_town->m_type) + BUILDING_DIALOG_ICON_FRAME_BASE,
                                    message.payload.widget.id,
                                    -1,
                                    0,
                                    -1,
                                    0
                                );
                                break;
                            }
                            {
                                sprintf(
                                    text,
                                    GetBuildingInfo(
                                        m_town->m_type,
                                        static_cast<BuildingSlotType>(message.payload.widget.id),
                                        1
                                    )
                                );
                                NormalDialog(
                                    text,
                                    NORMAL_DIALOG_INFO,
                                    -1,
                                    -1,
                                    H2EnumIndex(m_town->m_type) + BUILDING_DIALOG_ICON_FRAME_BASE,
                                    message.payload.widget.id,
                                    -1,
                                    0,
                                    -1,
                                    0
                                );
                            }
                            break;

                        case TOWN_WIDGET_CLOSE:
                            if (!quickView)
                                SetCommandAndText(message);
                            break;

                        default:
                            if (quickView) {
                                i32 armySelected = 0;
                                hero* viewedHero;
                                if (message.payload.widget.id >= TOWN_GARRISON_SLOT_FIRST
                                    && message.payload.widget.id <= TOWN_GARRISON_SLOT_LAST) {
                                    m_selectedStrip = m_garrisonStrip;
                                    m_selectedArmySlot =
                                        message.payload.widget.id - TOWN_GARRISON_SLOT_FIRST;
                                    armySelected = 1;
                                }
                                if (message.payload.widget.id >= TOWN_HERO_SLOT_FIRST
                                    && message.payload.widget.id <= TOWN_HERO_SLOT_LAST) {
                                    m_selectedStrip = m_heroStrip;
                                    m_selectedArmySlot =
                                        message.payload.widget.id - TOWN_HERO_SLOT_FIRST;
                                    armySelected = 1;
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

                case WIDGET_COMMAND_DESELECT:
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
                for (slot = 0; slot < TOWN_ARMY_SLOT_COUNT; ++slot) {
                    if (m_pendingStrip->m_army->m_creatureTypes[slot]
                        == m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])
                        break;
                }
                if (slot < TOWN_ARMY_SLOT_COUNT)
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
                static_cast<CreatureType>(oldValue);
            ResetStrips();
            break;

        case ARMY_COMMAND_VIEW_HERO:
            HeroView(m_town->m_occupyingHeroId, 1, 0);
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
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = TOWN_CONTROL_STATUS_TEXT;
    message.payload.widget.data.text = m_statusText;
    m_townWindow->BroadcastMessage(message);
    m_townWindow->DrawWindow(0);
    m_garrisonStrip->DrawIcons(0);
    m_heroStrip->DrawIcons(0);
    m_bankBox->Update(0);
    gpWindowManager->UpdateScreenRegion(0, 0, TOWN_SCREEN_WIDTH, TOWN_SCREEN_HEIGHT);
}

void townManager::SplitArmy(void) {
    i16 msgId = 1;
    i16 amountId = 4;
    i32 sameType;
    tag_message message;

    m_heroWindow1 = new heroWindow(SMALL_DIALOG_WINDOW_X, SMALL_DIALOG_WINDOW_Y, "splitwin.bin");
    if (m_heroWindow1 == NULL)
        MemError();
    m_splitAmount = 0;
    m_splitMaximum = m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
    message.type = MESSAGE_WIDGET;
    if (m_pendingStrip->m_army == m_swapStrip->m_army) {
        sprintf(gText, localization::Tr("hero.army.split.prompt")  );
    } else {
        sprintf(
            gText,
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
    sprintf(gText, "%d", m_splitAmount);
    message.payload.widget.id = SPLIT_SETUP_AMOUNT_CONTROL;
    message.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message);
    gpWindowManager->DoDialog(m_heroWindow1, SplitArmyHandler, 0);
    delete m_heroWindow1;
    if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
        sameType = 0;
        if (m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]
            == m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])
            sameType = 1;
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
    m_townWindow->DrawWindow(0, TOWN_REDRAW_FIRST_CONTROL, TOWN_REDRAW_LAST_CONTROL);
    PollSound();
    if (updateScreen != 0)
        BlitBitmapToScreen(
            gpWindowManager->m_screen,
            0,
            0,
            TOWN_SCREEN_WIDTH,
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
    u32l prerequisiteMask_c;
    i32 prerequisiteCount_p;
    char* description_b;
    i16 dialogWidth_e;
    textWidget* amountWidgets_b[TOWN_RESOURCE_COUNT];
    char* amountText_n[TOWN_RESOURCE_COUNT];
    iconWidget* resourceWidgets_m[TOWN_RESOURCE_COUNT];
    i32 rowResourceTypes_a[BUILD_ROW_RESOURCE_CAPACITY];
    i32 topRowCount_c;
    tag_message message_m;
    i32 widgetIndex_f;
    i32 xStart_b;
    i16 dialogLeft_a;
    i16 dialogButtonCount_m;
    i32 row_l;
    i32 resourcesInRow_l;
    i32 rowY_o;
    i32 windowY_m;
    i8 resourceTypes_o[BUILD_RESOURCE_STORAGE_COUNT];
    i32 costCount_o;
    i16 dialogResult_b;
    i32 index_h;
    i32 spacing_h;
    i32 bottomRowCount_o;
    heroWindow* window_a;
    char iconName_o[TOWN_OBJECT_FILENAME_SIZE];
    i32 rowWidth_h;
    i16 dialogButtonWidth_l;
    i32 lineCount_j;
    i32 windowHeight_a;
    icon* resourceIcon_c;
    i16 dialogControl_g;
    i32 windowRows_b;
    i32 mageLevel_k;
    i32 x_d;
    i32 resourceCount_a;
    i32 dwelling_k;
    i16 dialogHeight_f;
    i32 entryWidth_o;
    i16 costs_e[BUILD_RESOURCE_STORAGE_COUNT];
    widget* descriptionWidget_g;

    mageLevel_k = 0;
    index_h = 0;
    costCount_o = 0;
    description_b = static_cast<char*>(H2_ALLOC(BUILDING_DESCRIPTION_CAPACITY));

    for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
        costs_e[index_h] = -1;
        resourceTypes_o[index_h] = -1;
    }

    dwelling_k = -1;
    if (building >= TOWN_OBJECT_DWELLING_1 && building <= TOWN_OBJECT_ALTERNATE_UPGRADED_DWELLING_6)
        dwelling_k = H2EnumIndex(building) - H2EnumIndex(TOWN_OBJECT_DWELLING_1);

    if (building == BUILDING_SLOT_TAVERN && m_town->m_type == FACTION_NECROMANCER) {
        for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
            if (xShrineBuildingCost[index_h] > 0) {
                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
                costs_e[costCount_o] = static_cast<i16>(xShrineBuildingCost[index_h]);
                ++costCount_o;
            }
        }
    } else if (building == BUILDING_SLOT_MAGE_GUILD) {
        mageLevel_k = gpTownManager->m_town->m_buildState;
        for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
            if (gMageBuildingCosts
                    [mageLevel_k + 1 < TOWN_MAGE_GUILD_MAX_LEVEL ? mageLevel_k + 1
                                                                 : TOWN_MAGE_GUILD_MAX_LEVEL]
                    [index_h]
                > 0) {
                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
                costs_e[costCount_o] =
                    static_cast<i16>(gMageBuildingCosts
                                         [mageLevel_k + 1 < TOWN_MAGE_GUILD_MAX_LEVEL
                                              ? mageLevel_k + 1
                                              : TOWN_MAGE_GUILD_MAX_LEVEL][index_h]);
                ++costCount_o;
            }
        }
    } else if (building == BUILDING_SLOT_SPECIAL) {
        for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
            if (gSpecialBuildingCosts[H2EnumIndex(gpTownManager->m_town->m_type)][index_h] > 0) {
                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
                costs_e[costCount_o] = static_cast<i16>(
                    gSpecialBuildingCosts[H2EnumIndex(gpTownManager->m_town->m_type)][index_h]
                );
                ++costCount_o;
            }
        }
    } else if (building <= BUILDING_SLOT_NEUTRAL_LAST) {
        for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
            if (gNeutralBuildingCosts[H2EnumIndex(building)][index_h] > 0) {
                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
                costs_e[costCount_o] =
                    static_cast<i16>(gNeutralBuildingCosts[H2EnumIndex(building)][index_h]);
                ++costCount_o;
            }
        }
    } else {
        for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
            if (gDwellingCosts[H2EnumIndex(gpTownManager->m_town->m_type)][dwelling_k][index_h] > 0) {
                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
                costs_e[costCount_o] = static_cast<i16>(
                    gDwellingCosts[H2EnumIndex(gpTownManager->m_town->m_type)][dwelling_k][index_h]
                );
                ++costCount_o;
            }
        }
    }

    dialogWidth_e = 80;
    dialogHeight_f = 40;
    dialogLeft_a = 32;
    dialogControl_g = 289;
    dialogResult_b = 0;
    dialogButtonWidth_l = 2;
    dialogButtonCount_m = 3;
    widgetIndex_f = 0;
    resourceCount_a = 0;
    topRowCount_c = 0;
    bottomRowCount_o = 0;

    for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
        if (resourceTypes_o[index_h] != -1)
            ++resourceCount_a;
    }
    if (resourceCount_a <= 4) {
        topRowCount_c = resourceCount_a;
    } else if (resourceCount_a == 5) {
        topRowCount_c = 2;
        bottomRowCount_o = 3;
    } else if (resourceCount_a == 6) {
        topRowCount_c = 3;
        bottomRowCount_o = 3;
    } else if (resourceCount_a == TOWN_RESOURCE_COUNT) {
        topRowCount_c = 3;
        bottomRowCount_o = 4;
    }


    sprintf(
        description_b,
        GetBuildingInfo(m_town->m_type, building, 0)
    );
    i32 disallowed_p = m_town->IsBuildingDisallowed(H2EnumIndex(building));
    if (disallowed_p) {
        strcat(description_b, localization::Tr("town.build.disallowed"));
        cannotBuy = 1;
    } else if (dwelling_k >= 0) {
        prerequisiteCount_p = 0;
        prerequisiteMask_c = gHierarchyMask[H2EnumIndex(m_town->m_type)][dwelling_k];
        for (index_h = 0; index_h < TOWN_BUILDING_COUNT; ++index_h) {
            if (prerequisiteMask_c & (1L << index_h)) {
                if (prerequisiteCount_p == 0)
                    strcat(description_b, localization::Tr("town.build.requires")  );
                ++prerequisiteCount_p;
                strcat(description_b, "\n");
                strcat(description_b, GetBuildingName(m_town->m_type, BuildingSlotType(index_h)));
            }
        }
        if (m_town->m_type == FACTION_NECROMANCER
            && building == BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE
            && m_town->m_buildState <= NECROMANCER_PREREQUISITE_MAX_MAGE_LEVEL)
            strcat(description_b, localization::Tr("town.build.requires_mage_guild_level_2")  );
    }
    strcat(description_b, "\n ");

    lineCount_j = bigFont->LineLength(description_b, BUILD_DESCRIPTION_WIDTH);
    windowY_m = BUILD_WINDOW_BASE_Y;
    windowHeight_a = windowY_m;
    windowHeight_a += lineCount_j << BUILD_TEXT_LINE_SHIFT;
    if (resourceCount_a <= BUILD_ROW_RESOURCE_CAPACITY)
        windowHeight_a += BUILD_SINGLE_RESOURCE_ROW_HEIGHT;
    else
        windowHeight_a += BUILD_DOUBLE_RESOURCE_ROW_HEIGHT;
    if (quickView == 0)
        windowHeight_a += BUILD_BUTTON_AREA_HEIGHT;
    windowRows_b = (windowHeight_a - BUILD_WINDOW_ROW_BASE_HEIGHT) / BUILD_WINDOW_ROW_HEIGHT;
    if (windowRows_b < BUILD_WINDOW_MIN_ROWS)
        windowRows_b = BUILD_WINDOW_MIN_ROWS;
    if (windowRows_b > BUILD_WINDOW_MAX_ROWS)
        windowRows_b = BUILD_WINDOW_MAX_ROWS;

    sprintf(gText, "buybuil%d.bin", windowRows_b);
    window_a = new heroWindow(BUILD_WINDOW_X, BUILD_WINDOW_Y, gText);
    if (window_a == NULL)
        MemError();

    message_m.type = MESSAGE_WIDGET;
    message_m.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    message_m.payload.widget.id = BUILD_ICON_CONTROL;
    sprintf(iconName_o, "cstl%s.icn", cHeroTypeShortName[H2EnumIndex(m_town->m_type)]);
    message_m.payload.widget.data.text = iconName_o;
    window_a->BroadcastMessage(message_m);
    message_m.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    message_m.payload.widget.id = BUILD_ICON_CONTROL;
    message_m.payload.widget.data.value = H2EnumIndex(building);
    window_a->BroadcastMessage(message_m);

    if (building == BUILDING_SLOT_MAGE_GUILD) {
        sprintf(
            gText,
            localization::Tr(
                m_town->m_type == FACTION_CYBORG ? "castle.cybernetics_lab.level"
                                                 : "castle.mage_guild.level"
            ),
            mageLevel_k + 1 < TOWN_MAGE_GUILD_MAX_LEVEL ? mageLevel_k + 1
                                                        : TOWN_MAGE_GUILD_MAX_LEVEL
        );
    } else {
        strcpy(gText, GetBuildingName(m_town->m_type, building));
    }
    message_m.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message_m.payload.widget.id = BUILD_NAME_CONTROL;
    message_m.payload.widget.data.text = gText;
    window_a->BroadcastMessage(message_m);

    descriptionWidget_g = new textWidget(
        BUILD_DESCRIPTION_X,
        static_cast<i16>(windowY_m + BUILD_DESCRIPTION_Y_OFFSET),
        BUILD_DESCRIPTION_WIDTH,
        static_cast<i16>(lineCount_j << BUILD_TEXT_LINE_SHIFT),
        description_b,
        "bigfont.fnt",
        FONT_DRAW_DEFAULT,
        -1,
        WIDGET_KIND_UNDIMMED,
        FONT_ALIGN_CENTER
    );
    if (descriptionWidget_g == NULL)
        MemError();
    window_a->AddWidget(descriptionWidget_g, -1);

    widgetIndex_f = 0;
    if (!disallowed_p) {
        resourceIcon_c = gpResourceManager->GetIcon("resource.icn");
        for (row_l = 0; row_l < BUILD_RESOURCE_ROW_COUNT; ++row_l) {
            rowY_o = row_l * BUILD_RESOURCE_ROW_HEIGHT + lineCount_j * BUILD_TEXT_LINE_HEIGHT
                     + windowY_m + BUILD_RESOURCE_FIRST_Y_OFFSET;
            if (row_l == 0)
                resourcesInRow_l = topRowCount_c;
            else
                resourcesInRow_l = bottomRowCount_o;
            if (resourcesInRow_l > 0) {
                rowWidth_h = 0;
                costCount_o = widgetIndex_f;
                for (index_h = 0; index_h < BUILD_ROW_RESOURCE_CAPACITY; ++index_h) {
                    if (index_h < resourcesInRow_l) {
                        while (resourceTypes_o[costCount_o] == -1)
                            ++costCount_o;
                        rowResourceTypes_a[index_h] = resourceTypes_o[costCount_o];
                        ++costCount_o;
                    } else {
                        rowResourceTypes_a[index_h] = -1;
                    }
                }
                for (index_h = 0; index_h < resourcesInRow_l; ++index_h) {
                    rowWidth_h += GetIconEntry(resourceIcon_c, rowResourceTypes_a[index_h])->w;
                }
                spacing_h = (BUILD_RESOURCE_AREA_WIDTH - rowWidth_h) / (resourcesInRow_l + 1);
                xStart_b = spacing_h + BUILD_RESOURCE_AREA_LEFT;
                x_d = xStart_b;
                for (index_h = 0; index_h < resourcesInRow_l; ++index_h) {
                    entryWidth_o = GetIconEntry(resourceIcon_c, rowResourceTypes_a[index_h])->w;
                    amountText_n[widgetIndex_f] = static_cast<char*>(H2_ALLOC(BUILD_AMOUNT_TEXT_CAPACITY));
                    sprintf(amountText_n[widgetIndex_f], "%d", costs_e[widgetIndex_f]);
                    i32 widgetXOffset = 0;
                    amountWidgets_b[widgetIndex_f] = new textWidget(
                        static_cast<i16>(x_d + widgetXOffset),
                        static_cast<i16>(rowY_o + BUILD_AMOUNT_Y_OFFSET),
                        static_cast<i16>(entryWidth_o),
                        BUILD_RESOURCE_WIDGET_HEIGHT,
                        amountText_n[widgetIndex_f],
                        "smalfont.fnt",
                        FONT_DRAW_DEFAULT,
                        -1,
                        WIDGET_KIND_UNDIMMED,
                        FONT_ALIGN_CENTER
                    );
                    if (amountWidgets_b[widgetIndex_f] == NULL)
                        MemError();
                    resourceWidgets_m[widgetIndex_f] = new iconWidget(
                        static_cast<i16>(
                            x_d + widgetXOffset
                            - GetIconEntry(resourceIcon_c, rowResourceTypes_a[index_h])->x
                        ),
                        static_cast<i16>(rowY_o),
                        static_cast<i16>(entryWidth_o),
                        BUILD_RESOURCE_WIDGET_HEIGHT,
                        "resource.icn",
                        resourceTypes_o[widgetIndex_f],
                        ICON_DRAW_NORMAL,
                        -1,
                        WIDGET_KIND_ICON_DIRECT,
                        1
                    );
                    if (resourceWidgets_m[widgetIndex_f] == NULL)
                        MemError();
                    window_a->AddWidget(amountWidgets_b[widgetIndex_f], -1);
                    window_a->AddWidget(resourceWidgets_m[widgetIndex_f], -1);
                    ++widgetIndex_f;
                    x_d += spacing_h + entryWidth_o;
                }
            }
        }
        gpResourceManager->Dispose(resourceIcon_c);
    }

    if (quickView == 0)
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_SET_FLAGS,
            CONTROL_CLOSE,
            TOWN_INTERFACE_BROADCAST_FLAGS
        );
    m_selectedBuilding = BUILDING_SLOT_NONE;
    if (quickView != 0) {
        message_m.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message_m.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message_m.payload.widget.id = TOWN_DIALOG_CONFIRM;
        window_a->BroadcastMessage(message_m);
        message_m.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message_m.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message_m.payload.widget.id = H2EnumIndex(DIALOG_CANCEL_ID);
        window_a->BroadcastMessage(message_m);
        message_m.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message_m.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message_m.payload.widget.id = 0;
        window_a->BroadcastMessage(message_m);
        gpWindowManager->AddWindow(window_a, -1, 1);
        QuickViewWait();
        gpWindowManager->RemoveWindow(window_a);
    } else {
        if (cannotBuy != 0) {
            message_m.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_m.payload.widget.id = TOWN_DIALOG_CONFIRM;
            message_m.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
            window_a->BroadcastMessage(message_m);
            message_m.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_m.payload.widget.id = TOWN_DIALOG_CONFIRM;
            message_m.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_GRAYED);
            window_a->BroadcastMessage(message_m);
        }
        gpWindowManager->DoDialog(window_a, TrueFalseDialogHandler, 0);
        if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
            m_selectedBuilding = building;
            for (index_h = 0; index_h < resourceCount_a; ++index_h)
                gpCurPlayer->m_resources[resourceTypes_o[index_h]] -= costs_e[index_h];
        }
    }
    if (quickView == 0)
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_CLEAR_FLAGS,
            CONTROL_CLOSE,
            TOWN_INTERFACE_BROADCAST_FLAGS
        );
    delete window_a;
    if (quickView != 0) {
        return 0;
    } else {
        return gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM;
    }
}

void townManager::BuildObj(BuildingSlotType building) {
    i32 objectIndex_g;
    SLimitData limits_h;
    i32 index_h;
    SAMPLE2 buildSample_b;
    i32 frame_f;

    if ((m_town->m_buildings & (1 << H2EnumIndex(building)))
        && (building != BUILDING_SLOT_MAGE_GUILD
            || m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL)) {
        return;
    }
    if (building == BUILDING_SLOT_DOCK && !m_town->CanBuildDock()) {
        return;
    }
    {
        DrawTown(1, 1);
        m_town->BuildBuilding(building);
        for (index_h = 0; index_h < m_townObjectCount; ++index_h) {
            if (m_town->m_buildings & (1 << H2EnumIndex(m_townObjects[index_h]->m_buildingId))) {
                m_townObjects[index_h]->m_visible = 1;
                m_townObjects[index_h]->m_border->m_flags |= WIDGET_FLAG_ENABLED;
            } else {
                m_townObjects[index_h]->m_visible = 0;
                m_townObjects[index_h]->m_border->m_flags &= ~WIDGET_FLAG_ENABLED;
            }
        }

        objectIndex_g = -1;
        for (index_h = 0; index_h < m_townObjectCount; ++index_h) {
            if (m_townObjects[index_h]->m_buildingId == building)
                objectIndex_g = index_h;
        }

        giMaxExtentY = 0;
        giMaxExtentX = giMaxExtentY;
        giMinExtentX = TOWN_SCREEN_WIDTH - 1;
        giMinExtentY = TOWN_VIEWPORT_HEIGHT - 1;
        gbComputeExtent = true;
        gbSaveBiggestExtent = true;
        gbReturnAfterComputeExtent = true;
        if (building == BUILDING_SLOT_MAGE_GUILD) {
            if (gpTownManager->m_town->m_type == FACTION_NECROMANCER)
                frame_f = (gpTownManager->m_town->m_buildState - 1)
                          * NECROMANCER_BUILD_STATE_FRAME_STRIDE;
            else
                frame_f = gpTownManager->m_town->m_buildState - 1;
            m_townObjects[objectIndex_g]
                ->m_icon
                ->CombatClipDrawToBuffer(0, 0, frame_f, &limits_h, ICON_DRAW_NORMAL, 0, NULL, NULL);
            if (m_townObjects[objectIndex_g]->m_animationFrameCount != 0)
                m_townObjects[objectIndex_g]->m_icon->CombatClipDrawToBuffer(
                    0,
                    0,
                    frame_f + 1,
                    &limits_h,
                    ICON_DRAW_NORMAL,
                    0,
                    NULL,
                    NULL
                );
        } else {
            m_townObjects[objectIndex_g]
                ->m_icon
                ->CombatClipDrawToBuffer(0, 0, 0, &limits_h, ICON_DRAW_NORMAL, 0, NULL, NULL);
            if (m_townObjects[objectIndex_g]->m_animationFrameCount != 0)
                m_townObjects[objectIndex_g]
                    ->m_icon
                    ->CombatClipDrawToBuffer(0, 0, 1, &limits_h, ICON_DRAW_NORMAL, 0, NULL, NULL);
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
        buildSample_b = LoadPlaySample("buildtwn.82M");
        gpWindowManager->FizzleForward(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1,
            -1,
            NULL,
            NULL
        );
        WaitEndSample(&buildSample_b, -1);
        PollSound();
        m_selectedBuilding = BUILDING_SLOT_NONE;
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_CLEAR_FLAGS,
            CONTROL_CLOSE,
            TOWN_INTERFACE_BROADCAST_FLAGS
        );
        RedrawTownScreen();
    }
}

void townManager::SetupMage(heroWindow* window) {
    i16 unusedZero_g = 0;
    i16 unusedAvailable_n = 1;
    i16 unusedInvalid_d = TOWN_MAGE_SPELL_UNAVAILABLE;
    i16 unusedIconState_m = 2;
    i16 unusedFirstSpell_c = TOWN_MAGE_FIRST_SPELL_CONTROL;
    i16 unusedFirstIcon_c = TOWN_MAGE_FIRST_ICON_CONTROL;
    i16 unusedFirstDescription_h = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
    i16 unusedGuildIcon_i = TOWN_MAGE_GUILD_ICON_CONTROL;
    i16 unusedDescription_b = TOWN_MAGE_DESCRIPTION_CONTROL;
    tag_message message_i;
    i32 level_f;
    i32 slot_o;
    i32 spellState_c;
    i32 lineCount_k;
    i32 unusedGuildFrame_g;
    i32 unusedLocal_l;

    message_i.type = MESSAGE_WIDGET;
    hero* occupyingHero = m_town->m_occupyingHeroId == -1
        ? NULL
        : gpGame->GetHero(m_town->m_occupyingHeroId);
    if (occupyingHero == NULL || !occupyingHero->HasArtifact(ARTIFACT_MAGIC_BOOK))
        strcpy(gText, localization::Tr("town.mage_guild.spells_available"));
    else if (m_town->m_type == FACTION_CYBORG
             && occupyingHero->m_cursorType != FACTION_CYBORG)
        strcpy(gText, localization::Tr("town.cybernetics.requires_cyborg"));
    else if (occupyingHero->m_cursorType == FACTION_CYBORG)
        strcpy(gText, localization::Tr("town.cybernetics.level_limit"));
    else
        strcpy(gText, localization::Tr("town.mage_guild.spells_added"));
    message_i.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message_i.payload.widget.id = TOWN_MAGE_DESCRIPTION_CONTROL;
    message_i.payload.widget.data.text = gText;
    window->BroadcastMessage(message_i);

    for (level_f = 0; level_f < TOWN_MAGE_GUILD_MAX_LEVEL; ++level_f) {
        for (slot_o = 0; slot_o < TOWN_MAGE_SPELLS_PER_LEVEL; ++slot_o) {
            i32 slotLimit =
                gSpellLimits[level_f]
                + (m_town->m_type == FACTION_WIZARD
                   && (m_town->m_buildings & TOWN_WIZARD_LIBRARY_BUILDING_FLAG));
            if (m_town->m_type == FACTION_CYBORG)
                slotLimit = ironfist::CyborgSpellLimits[level_f];
            if (slot_o >= slotLimit) {
                spellState_c = TOWN_MAGE_SPELL_UNAVAILABLE;
            } else {
                spellState_c =
                    static_cast<i16>(slot_o >= m_town->m_spellCounts[level_f + 1]);
            }

            message_i.payload.widget.command =
                spellState_c == TOWN_MAGE_SPELL_UNAVAILABLE ? WIDGET_COMMAND_CLEAR_FLAGS
                                                            : WIDGET_COMMAND_SET_FLAGS;
            message_i.payload.widget.id =
                level_f * TOWN_MAGE_SPELLS_PER_LEVEL + slot_o + TOWN_MAGE_FIRST_SPELL_CONTROL;
            message_i.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            window->BroadcastMessage(message_i);

            if (spellState_c != TOWN_MAGE_SPELL_UNAVAILABLE) {
                message_i.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                message_i.payload.widget.id =
                    level_f * TOWN_MAGE_SPELLS_PER_LEVEL + slot_o + TOWN_MAGE_FIRST_SPELL_CONTROL;
                message_i.payload.widget.data.value = spellState_c;
                window->BroadcastMessage(message_i);
            }

            if (spellState_c != 0) {
                message_i.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
                message_i.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
                message_i.payload.widget.id =
                    level_f * TOWN_MAGE_SPELLS_PER_LEVEL + slot_o + TOWN_MAGE_FIRST_ICON_CONTROL;
                window->BroadcastMessage(message_i);
                message_i.payload.widget.id = level_f * TOWN_MAGE_SPELLS_PER_LEVEL + slot_o
                                              + TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
                window->BroadcastMessage(message_i);
            } else {
                message_i.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                message_i.payload.widget.id =
                    level_f * TOWN_MAGE_SPELLS_PER_LEVEL + slot_o + TOWN_MAGE_FIRST_ICON_CONTROL;
                message_i.payload.widget.data.value =
                    gsSpellInfo[H2EnumIndex(m_town->m_spells[level_f][slot_o])].iconIndex;
                window->BroadcastMessage(message_i);
                lineCount_k = smallFont->LineLength(
                    gSpellNames[H2EnumIndex(m_town->m_spells[level_f][slot_o])],
                    MAGE_SPELL_NAME_WIDTH
                );
                if (lineCount_k == 1)
                    sprintf(
                        gText,
                        "%s\n[%d]",
                        gSpellNames[H2EnumIndex(m_town->m_spells[level_f][slot_o])],
                        GetManaCost(m_town->m_spells[level_f][slot_o], NULL)
                    );
                else
                    sprintf(
                        gText,
                        "%s  [%d]",
                        gSpellNames[H2EnumIndex(m_town->m_spells[level_f][slot_o])],
                        GetManaCost(m_town->m_spells[level_f][slot_o], NULL)
                    );
                message_i.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                message_i.payload.widget.id = level_f * TOWN_MAGE_SPELLS_PER_LEVEL + slot_o
                                              + TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
                message_i.payload.widget.data.text = gText;
                window->BroadcastMessage(message_i);
            }
        }
    }

    unusedGuildFrame_g = m_town->m_buildState - 1;
    message_i.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    message_i.payload.widget.id = TOWN_MAGE_GUILD_ICON_CONTROL;
    message_i.payload.widget.data.value = unusedGuildFrame_g;
    window->BroadcastMessage(message_i);
    sprintf(gText, "magegld%c.icn", cHeroTypeInitial[H2EnumIndex(m_town->m_type)]);
    message_i.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    message_i.payload.widget.id = TOWN_MAGE_GUILD_ICON_CONTROL;
    message_i.payload.widget.data.text = gText;
    window->BroadcastMessage(message_i);
}

MessageDispatchResult MageGuildHandler(tag_message& message) {
    i16 unusedFirstSpell_b = TOWN_MAGE_FIRST_SPELL_CONTROL;
    i16 unusedFirstIcon_c = TOWN_MAGE_FIRST_ICON_CONTROL;
    i16 unusedFirstDescription_h = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
    u32 quickView_i;
    i32 spellSlot_b;
    i32 level_d;
    i32 slot_j;
    SpellType spell_j;

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                quickView_i = (H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)));
                spellSlot_b = -1;
                if (message.payload.widget.id >= TOWN_MAGE_FIRST_SPELL_CONTROL
                    && message.payload.widget.id
                           < TOWN_MAGE_FIRST_SPELL_CONTROL
                                 + TOWN_MAGE_GUILD_MAX_LEVEL * TOWN_MAGE_SPELLS_PER_LEVEL)
                    spellSlot_b = message.payload.widget.id - TOWN_MAGE_FIRST_SPELL_CONTROL;
                if (message.payload.widget.id >= TOWN_MAGE_FIRST_ICON_CONTROL
                    && message.payload.widget.id
                           < TOWN_MAGE_FIRST_ICON_CONTROL
                                 + TOWN_MAGE_GUILD_MAX_LEVEL * TOWN_MAGE_SPELLS_PER_LEVEL)
                    spellSlot_b = message.payload.widget.id - TOWN_MAGE_FIRST_ICON_CONTROL;
                if (message.payload.widget.id >= TOWN_MAGE_FIRST_DESCRIPTION_CONTROL
                    && message.payload.widget.id
                           < TOWN_MAGE_FIRST_DESCRIPTION_CONTROL
                                 + TOWN_MAGE_GUILD_MAX_LEVEL * TOWN_MAGE_SPELLS_PER_LEVEL)
                    spellSlot_b = message.payload.widget.id - TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
                if (spellSlot_b != -1) {
                    level_d = spellSlot_b / TOWN_MAGE_SPELLS_PER_LEVEL;
                    slot_j = spellSlot_b % TOWN_MAGE_SPELLS_PER_LEVEL;
                    if (slot_j >= level_d[gpTownManager->m_town->m_spellCounts + 1])
                        return MESSAGE_DISPATCH_CONSUME;
                    spell_j = gpTownManager->m_town->m_spells[level_d][slot_j];
                    NormalDialog(
                        gSpellDesc[H2EnumIndex(spell_j)],
                        quickView_i != 0 ? NORMAL_DIALOG_QUICK_VIEW : NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        NORMAL_DIALOG_SPELL,
                        H2EnumIndex(spell_j),
                        -1,
                        0,
                        -1,
                        0
                    );
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
    i16 unusedTextState_j = 1;
    i16 unusedPortraitState_i = 2;
    i16 unusedTextControl_g = 3;
    i16 unusedIconState_e = 4;
    i16 unusedDimState_d = 6;
    i16 unusedPortraitControl_i = 7;
    i16 unusedButtonText_h = 8;
    i16 unusedButtonIcon_e = 9;
    tag_message message_e;
    i32 artifactCount_h;
    i32 index_j;

    m_heroWindow1 = new heroWindow(RECRUIT_WINDOW_X, RECRUIT_WINDOW_Y, "rcrthero.bin");
    if (m_heroWindow1 == NULL)
        MemError();
    SetWinText(m_heroWindow1, RECRUIT_WINDOW_TEXT_ID);
    m_recruitHero = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
    m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
    message_e.type = MESSAGE_WIDGET;

    if (cannotRecruit != 0) {
        message_e.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message_e.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
        message_e.payload.widget.id = RECRUIT_BUTTON_TEXT_CONTROL;
        m_heroWindow1->BroadcastMessage(message_e);
        message_e.payload.widget.id = RECRUIT_BUTTON_ICON_CONTROL;
        m_heroWindow1->BroadcastMessage(message_e);
        message_e.payload.widget.id = TOWN_DIALOG_CONFIRM;
        m_heroWindow1->BroadcastMessage(message_e);
        message_e.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message_e.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_GRAYED);
        message_e.payload.widget.id = RECRUIT_BUTTON_TEXT_CONTROL;
        m_heroWindow1->BroadcastMessage(message_e);
        message_e.payload.widget.id = RECRUIT_BUTTON_ICON_CONTROL;
        m_heroWindow1->BroadcastMessage(message_e);
        message_e.payload.widget.id = TOWN_DIALOG_CONFIRM;
        m_heroWindow1->BroadcastMessage(message_e);
    }

    artifactCount_h = 0;
    for (index_j = 0; index_j < TOWN_MAX_ARTIFACTS; ++index_j) {
        if (m_recruitHero->m_artifacts[index_j] != ARTIFACT_NONE
            && m_recruitHero->m_artifacts[index_j] != ARTIFACT_MAGIC_BOOK)
            ++artifactCount_h;
    }
    sprintf(
        gText,
        localization::TrPlural("town.recruit_hero.description", artifactCount_h),
        m_recruitHero->m_name,
        m_recruitHero->m_level,
        gAlignmentNames[H2EnumIndex(m_recruitHero->m_cursorType)],
        artifactCount_h
    );
    message_e.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message_e.payload.widget.id = RECRUIT_DESCRIPTION_CONTROL;
    message_e.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message_e);
    sprintf(gText, "port%04d.icn", H2EnumIndex(m_recruitHero->m_portrait));
    message_e.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    message_e.payload.widget.id = RECRUIT_PORTRAIT_CONTROL;
    message_e.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message_e);

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
            (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION);
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
        m_recruitResult = 1;
        m_town->m_occupyingHeroId = m_recruitHero->m_id;
        gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
            static_cast<i8>(giCurPlayer);
        CheckValidAvailableHeroes();
        if (m_town->m_buildings & 1)
            m_town->GiveSpells(NULL);

        newHeroClass = static_cast<FactionType>(
            gpCurPlayer->m_availableHeroIds[1 - m_recruitState] / HEROES_PER_FACTION
        );
        newHeroClass = (newHeroClass + Random(1, H2EnumIndex(FACTION_COUNT) - 1)) % TOWN_FACTION_COUNT;
        gpCurPlayer->m_availableHeroIds[m_recruitState] =
            static_cast<i8>(gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0));
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
        TOWN_INTERFACE_BROADCAST_FLAGS
    );
    m_recruitHero->m_owner = -1;
    if (m_recruitState != -1)
        m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
    recruitedHero->m_remainingMobility = previousMobility;
    return m_recruitState != -1;
}

MessageDispatchResult TavernHandler(tag_message& message) {
    i32 unusedDelay = TOWN_TAVERN_ANIMATION_DELAY;
    i16 unusedFirstFrame = TOWN_TAVERN_FIRST_ANIMATION_FRAME;

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case EVENT_WINDOW_FIRST_BUTTON:
                    case EVENT_WINDOW_SECOND_BUTTON:
                    case TOWN_DIALOG_CONFIRM:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        message.payload.widget.id = H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT);
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
    if (glTimers[0] < platform::Ticks()) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = TAVERN_ANIMATION_CONTROL;
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
    i32 unusedValue = 0;
    tag_message message;

    m_heroWindow0 = new heroWindow(TAVERN_WINDOW_X, TAVERN_WINDOW_Y, "tavwin.bin");
    if (m_heroWindow0 == NULL)
        MemError();
    SetWinText(m_heroWindow0, TAVERN_WINDOW_TEXT_ID);
    sprintf(
        gText,
        localization::Tr("town.tavern.rumor")


        ,
        gpGame->m_rumour
    );
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = TOWN_TAVERN_RUMOUR_CONTROL;
    message.payload.widget.data.text = gText;
    m_heroWindow0->BroadcastMessage(message);
    gpWindowManager->DoDialog(m_heroWindow0, TavernHandler, 0);
    delete m_heroWindow0;
}

MessageDispatchResult SplitArmyHandler(tag_message& message) {
    i16 plusButton_d = TOWN_SPLIT_INCREASE_CONTROL;
    i16 minusButton_g = TOWN_SPLIT_DECREASE_CONTROL;
    i16 amountControl_e = TOWN_SPLIT_AMOUNT_CONTROL;
    i32 handled_c = 0;
    i32 unusedAction_l;

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
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
            case WIDGET_COMMAND_DESELECT:
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
                    case EVENT_WINDOW_FIRST_BUTTON:
                    case EVENT_WINDOW_SECOND_BUTTON:
                        gpTownManager->m_splitAmount = 0;
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        handled_c = 1;
                        break;
                    case TOWN_DIALOG_CONFIRM:
                        if (gpTownManager->m_splitAmount == 0)
                            gpWindowManager->m_dialogResult = H2EnumIndex(DIALOG_CANCEL_ID);
                        else
                            gpWindowManager->m_dialogResult = H2EnumIndex(TOWN_DIALOG_CONFIRM);
                        handled_c = 1;
                        break;
                    default:
                        break;
                }
                break;
        }
    }

    if (handled_c == 1) {
        message.payload.widget.id = H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT);
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;

update_amount:
    sprintf(gText, "%d", gpTownManager->m_splitAmount);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = TOWN_SPLIT_AMOUNT_CONTROL;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    gpTownManager->m_heroWindow1
        ->DrawWindow(1, TOWN_SPLIT_AMOUNT_CONTROL, TOWN_SPLIT_AMOUNT_CONTROL);
    return MESSAGE_DISPATCH_CONSUME;
}

void townManager::SetupWell(heroWindow* window) {
    i16 unusedFirstIcon_d = 1;
    i16 unusedFirstName_b = TOWN_WELL_FIRST_NAME_CONTROL;
    i16 unusedFirstMonsterIcon_p = TOWN_WELL_FIRST_MONSTER_ICON_CONTROL;
    i16 unusedFirstCreature_f = TOWN_WELL_FIRST_CREATURE_CONTROL;
    i16 unusedFirstDetail_g = TOWN_WELL_FIRST_DETAIL_CONTROL;
    i16 unusedFirstAvailable = TOWN_WELL_FIRST_AVAILABLE_CONTROL;
    i16 unusedFirstAvailableCount = TOWN_WELL_FIRST_AVAILABLE_COUNT_CONTROL;
    u8 dwellingTypes_c[WELL_DWELLING_TYPE_STORAGE_COUNT];
    i32 available_e;
    i32 dwellingResult_a;
    tag_message message_i;
    char iconName_b[TOWN_OBJECT_FILENAME_SIZE];
    char detailText_i[WELL_DETAIL_TEXT_CAPACITY];
    tag_monsterInfo monsterInfo_f;
    i32 growth_a;

    for (dwellingResult_a = 0; dwellingResult_a < TOWN_WELL_DWELLING_COUNT; ++dwellingResult_a) {
        if (dwellingResult_a == TOWN_WELL_DWELLING_COUNT - 1
            && (m_town->m_buildings & (1L << TOWN_WELL_LAST_UPGRADE_BUILDING))) {
            dwellingTypes_c[dwellingResult_a] = WELL_ALTERNATE_UPGRADE_INDEX;
        } else if (dwellingResult_a >= 1
                   && (m_town->m_buildings
                       & (1L << (dwellingResult_a + TOWN_WELL_FIRST_UPGRADE_BUILDING)))) {
            dwellingTypes_c[dwellingResult_a] =
                static_cast<u8>(dwellingResult_a + TOWN_WELL_FIRST_UPGRADE_OFFSET);
        } else {
            dwellingTypes_c[dwellingResult_a] = static_cast<u8>(dwellingResult_a);
        }
    }

    message_i.type = MESSAGE_WIDGET;
    message_i.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    sprintf(iconName_b, "cstl%s.icn", cHeroTypeShortName[H2EnumIndex(m_town->m_type)]);
    message_i.payload.widget.data.text = iconName_b;
    for (dwellingResult_a = 0; dwellingResult_a < TOWN_WELL_DWELLING_COUNT; ++dwellingResult_a) {
        message_i.payload.widget.id = dwellingResult_a + 1;
        window->BroadcastMessage(message_i);
    }

    for (dwellingResult_a = 0; dwellingResult_a < TOWN_WELL_DWELLING_COUNT; ++dwellingResult_a) {
        message_i.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message_i.payload.widget.id = dwellingResult_a + 1;
        message_i.payload.widget.data.value =
            dwellingTypes_c[dwellingResult_a] + H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST);
        window->BroadcastMessage(message_i);
        sprintf(
            gText,
            "monh%04d.icn",
            H2EnumIndex(gDwellingType[H2EnumIndex(m_town->m_type)][dwellingTypes_c[dwellingResult_a]])
        );
        message_i.payload.widget.command = WIDGET_COMMAND_SET_ICON;
        message_i.payload.widget.id = dwellingResult_a + TOWN_WELL_FIRST_MONSTER_ICON_CONTROL;
        message_i.payload.widget.data.text = gText;
        window->BroadcastMessage(message_i);
    }

    message_i.type = MESSAGE_WIDGET;
    message_i.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    for (dwellingResult_a = 0; dwellingResult_a < TOWN_WELL_DWELLING_COUNT; ++dwellingResult_a) {
        sprintf(
            gText,
            GetBuildingName(
                m_town->m_type,
                BuildingSlotType(
                    dwellingTypes_c[dwellingResult_a] + H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)
                )
            )
        );
        message_i.payload.widget.id = dwellingResult_a + TOWN_WELL_FIRST_NAME_CONTROL;
        message_i.payload.widget.data.text = gText;
        window->BroadcastMessage(message_i);

        if (m_town->m_buildings
            & (1L << (dwellingTypes_c[dwellingResult_a] + H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)))) {
            available_e = m_town->m_garrison[dwellingTypes_c[dwellingResult_a]];
            sprintf(gText, localization::Tr("town.well.available")  );
            message_i.payload.widget.id = dwellingResult_a + TOWN_WELL_FIRST_AVAILABLE_CONTROL;
            message_i.payload.widget.data.text = gText;
            window->BroadcastMessage(message_i);
            sprintf(gText, "%d", available_e);
            message_i.payload.widget.id = dwellingResult_a + TOWN_WELL_FIRST_AVAILABLE_COUNT_CONTROL;
            message_i.payload.widget.data.text = gText;
            window->BroadcastMessage(message_i);
        }

        message_i.payload.widget.id = dwellingResult_a + TOWN_WELL_FIRST_CREATURE_CONTROL;
        strcpy(
            gText,
            gArmyNames[H2EnumIndex(gDwellingType[H2EnumIndex(m_town->m_type)][dwellingTypes_c[dwellingResult_a]])]
        );
        utf8::UppercaseFirst(gText);
        message_i.payload.widget.data.text = gText;
        window->BroadcastMessage(message_i);
    }

    for (dwellingResult_a = 0; dwellingResult_a < TOWN_WELL_DWELLING_COUNT; ++dwellingResult_a) {
        monsterInfo_f = gMonsterDatabase[H2EnumIndex(gDwellingType[H2EnumIndex(m_town->m_type)][dwellingTypes_c[dwellingResult_a]])];
        strcpy(
            gText,
            ""
        );
        sprintf(detailText_i, "%s%d", cWellDetail[WELL_DETAIL_ATTACK], monsterInfo_f.attack);
        strcat(gText, detailText_i);
        sprintf(detailText_i, "\n%s%d", cWellDetail[WELL_DETAIL_DEFENSE], monsterInfo_f.defense);
        strcat(gText, detailText_i);
        sprintf(detailText_i, "\n%s%d", cWellDetail[WELL_DETAIL_DAMAGE], monsterInfo_f.damageMin);
        strcat(gText, detailText_i);
        if (monsterInfo_f.damageMin != monsterInfo_f.damageMax) {
            sprintf(detailText_i, "-%d", monsterInfo_f.damageMax);
            strcat(gText, detailText_i);
        }
        sprintf(detailText_i, "\n%s%d", cWellDetail[WELL_DETAIL_HIT_POINTS], monsterInfo_f.hitPoints);
        strcat(gText, detailText_i);
        sprintf(detailText_i, cWellDetail[WELL_DETAIL_SPEED], speedText[monsterInfo_f.speed]);
        strcat(gText, detailText_i);
        if (m_town->m_buildings
            & (1L << (dwellingTypes_c[dwellingResult_a] + H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)))) {
            growth_a = gMonsterDatabase[H2EnumIndex(gDwellingType[H2EnumIndex(m_town->m_type)]
                                                        [dwellingTypes_c[dwellingResult_a]])]
                          .growth;
            if (!ironfist::IsWellDisabled())
                growth_a += TOWN_WELL_BASE_GROWTH_BONUS;
            if (dwellingResult_a == 0
                && (m_town->m_buildings & (1L << TOWN_WELL_FIRST_DWELLING_GROWTH_BUILDING)))
                growth_a += TOWN_WELL_FIRST_DWELLING_GROWTH_BONUS;
            sprintf(detailText_i, cWellDetail[WELL_DETAIL_GROWTH], growth_a);
            strcat(gText, detailText_i);
        }
        message_i.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message_i.payload.widget.id = dwellingResult_a + TOWN_WELL_FIRST_DETAIL_CONTROL;
        message_i.payload.widget.data.text = gText;
        window->BroadcastMessage(message_i);
    }
}

void townManager::SetupThievesGuild(heroWindow* window, i32 informationLevel) {
    i16 unusedRankX_i = THIEVES_RANK_FIRST_X;
    i16 unusedRankWidth_g = THIEVES_PLAYER_COLUMN_WIDTH;
    i16 unusedRankY_n = THIEVES_FIRST_CATEGORY_Y;
    i16 unusedRankHeight_k = THIEVES_CATEGORY_ROW_HEIGHT;
    i16 unusedRankIconHeight_o = THIEVES_RANK_ICON_HEIGHT;
    i16 unusedIconWidth_f = THIEVES_RANK_ICON_WIDTH;
    i16 unusedIconHeight_a = THIEVES_RANK_ICON_HEIGHT;
    i16 unusedPlayerWidth_n = 72;
    TownThievesGuildCategory category_l;
    i8 categoryOrder_a[TOWN_THIEVES_ORDER_BUFFER_SIZE];
    i32 rank_a;
    i32 tiedCount_j;
    i32 rankX_e;
    i32 lastAtRank_b;
    i32 firstAtRank_k;
    widget* iconControl_a;
    char* widgetText_c;
    i32 maxCategories_n;
    i32 strongestHeroValue_c;
    char statText_h[THIEVES_STAT_TEXT_CAPACITY];
    i32 armySlot_n;
    i32l categoryStats_a[TOWN_THIEVES_PLAYER_COUNT];
    i32 heroPosition_d;
    i32 heroValue_i;
    i32 strongestCreatureValue_l;
    i32 strongestHeroPosition_j;
    hero* strongestHero_d;
    i32 unusedThievesValue_i;
    town* playerTown_j;
    i16 unusedFirstRankControl_j = TOWN_THIEVES_FIRST_RANK_CONTROL;
    i16 unusedFirstPlayerControl_e = TOWN_THIEVES_FIRST_PLAYER_CONTROL;
    i16 unusedHeroY_k = THIEVES_HERO_Y;
    i16 unusedPrimaryStatsY = THIEVES_PRIMARY_STATS_Y;
    i16 unusedPersonalityY_m = THIEVES_PERSONALITY_Y;
    i16 unusedCreatureY_d = THIEVES_CREATURE_Y;
    i32 position_a;
    tag_message message_h;
    widget* textControl_p;
    CreatureType strongestCreature_o;

    if (informationLevel == -1)
        informationLevel = gpGame->GetNumThievesGuilds(giCurPlayer);

    if (informationLevel >= TOWN_THIEVES_INFO_ALL_CATEGORIES) {
        maxCategories_n = TOWN_THIEVES_MAX_CATEGORIES;
    } else {
        if (informationLevel == TOWN_THIEVES_INFO_STRONGEST_CREATURE)
            maxCategories_n = TOWN_THIEVES_CREATURE_CATEGORY_COUNT;
        else if (informationLevel == TOWN_THIEVES_INFO_PERSONALITY)
            maxCategories_n = TOWN_THIEVES_PERSONALITY_CATEGORY_COUNT;
        else if (informationLevel == TOWN_THIEVES_INFO_PRIMARY_STATS)
            maxCategories_n = TOWN_THIEVES_PRIMARY_CATEGORY_COUNT;
        else
            maxCategories_n = TOWN_THIEVES_BASIC_CATEGORY_COUNT;
    }

    for (position_a = gpGame->m_playerCount - gpGame->m_deadPlayerCount;
         position_a < TOWN_THIEVES_PLAYER_COUNT;
         ++position_a) {
        message_h.type = MESSAGE_WIDGET;
        message_h.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message_h.payload.widget.id = position_a + TOWN_THIEVES_FIRST_RANK_CONTROL;
        message_h.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        window->BroadcastMessage(message_h);
        message_h.payload.widget.id = position_a + TOWN_THIEVES_FIRST_PLAYER_CONTROL;
        window->BroadcastMessage(message_h);
    }
    for (position_a = gpGame->m_playerCount - gpGame->m_deadPlayerCount;
         position_a < TOWN_THIEVES_PLAYER_COUNT;
         ++position_a) {
        message_h.type = MESSAGE_WIDGET;
        message_h.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message_h.payload.widget.id = position_a + TOWN_THIEVES_FIRST_PLAYER_CONTROL;
        message_h.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        window->BroadcastMessage(message_h);
    }

    for (category_l = THIEVES_CATEGORY_TOWNS; !(H2EnumIndex(category_l) >= maxCategories_n);
         ++category_l) {
        GetCategoryStats(category_l, categoryStats_a, categoryOrder_a);
        SortStats(categoryStats_a, categoryOrder_a);
        firstAtRank_k = 0;
        lastAtRank_b = 0;
        for (rank_a = 0; rank_a < TOWN_THIEVES_PLAYER_COUNT; ++rank_a) {
            if (firstAtRank_k == gpGame->m_playerCount - gpGame->m_deadPlayerCount)
                break;
            tiedCount_j = 1;
            while (lastAtRank_b + 1 < gpGame->m_playerCount
                   && categoryStats_a[lastAtRank_b + 1] == categoryStats_a[lastAtRank_b]) {
                ++tiedCount_j;
                ++lastAtRank_b;
            }
            rankX_e = THIEVES_PLAYER_COLUMN_WIDTH * rank_a + THIEVES_RANK_FIRST_X
                      - (tiedCount_j - 1) * THIEVES_TIE_CENTERING_STEP;
            for (position_a = firstAtRank_k; !(position_a > lastAtRank_b); ++position_a) {
                iconControl_a = new iconWidget(
                    static_cast<i16>(
                        rankX_e + (position_a - firstAtRank_k) * THIEVES_RANK_ICON_WIDTH
                    ),
                    static_cast<i16>(
                        H2EnumIndex(category_l) * THIEVES_CATEGORY_ROW_HEIGHT + THIEVES_FIRST_CATEGORY_Y
                    ),
                    THIEVES_RANK_ICON_WIDTH,
                    THIEVES_RANK_ICON_HEIGHT,
                    "townwind.icn",
                    static_cast<i16>(
                        gpGame->m_players[categoryOrder_a[position_a]].m_color
                        + TOWN_THIEVES_RANK_ICON_FRAME_BASE
                    ),
                    ICON_DRAW_NORMAL,
                    -1,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
                if (iconControl_a == NULL)
                    MemError();
                window->AddWidget(iconControl_a, -1);
            }
            ++lastAtRank_b;
            firstAtRank_k = lastAtRank_b;
        }
    }

    rank_a = 0;
    for (position_a = 0; position_a < gpGame->m_playerCount - gpGame->m_deadPlayerCount;
         ++position_a) {
        while (gpGame->m_playerDead[rank_a] != 0)
            ++rank_a;
        sprintf(gText, gColors[gpGame->m_players[rank_a].m_color]);
        utf8::UppercaseFirst(gText);
        message_h.type = MESSAGE_WIDGET;
        message_h.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message_h.payload.widget.id = position_a + TOWN_THIEVES_FIRST_PLAYER_CONTROL;
        message_h.payload.widget.data.text = gText;
        window->BroadcastMessage(message_h);

        if (informationLevel < TOWN_THIEVES_INFO_STRONGEST_HERO)
            goto nextRank;
        {
            strongestHeroPosition_j = -1;
            strongestHeroValue_c = 0;
            for (heroPosition_d = 0; heroPosition_d < gpGame->m_players[rank_a].m_heroCount;
                 ++heroPosition_d) {
                strongestHero_d = gpGame->GetPlayerHero(rank_a, heroPosition_d);
                heroValue_i =
                    gpPhilAI
                        ->FightValueOfStack(&strongestHero_d->m_army, strongestHero_d, 0, 0, 0, 0);
                if (!(heroValue_i <= strongestHeroValue_c)) {
                    strongestHeroValue_c = heroValue_i;
                    strongestHeroPosition_j = heroPosition_d;
                }
            }

            if (strongestHeroPosition_j != -1) {
                iconControl_a = new iconWidget(
                    static_cast<i16>(
                        position_a * THIEVES_PLAYER_COLUMN_WIDTH + THIEVES_HERO_LOCATOR_X
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
                if (iconControl_a == NULL)
                    MemError();
                window->AddWidget(iconControl_a, -1);
                iconControl_a = new iconWidget(
                    static_cast<i16>(
                        position_a * THIEVES_PLAYER_COLUMN_WIDTH + THIEVES_HERO_PORTRAIT_X
                    ),
                    THIEVES_HERO_Y,
                    0,
                    0,
                    "miniport.icn",
                    static_cast<i16>(
                        gpGame->GetPlayerHero(rank_a, strongestHeroPosition_j)->m_portrait
                    ),
                    ICON_DRAW_NORMAL,
                    -1,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
                if (iconControl_a == NULL)
                    MemError();
                window->AddWidget(iconControl_a, -1);
            }

            if (informationLevel < TOWN_THIEVES_INFO_PRIMARY_STATS)
                goto nextRank;
            {
                if (strongestHeroPosition_j != -1) {
                    strongestHero_d = gpGame->GetPlayerHero(rank_a, strongestHeroPosition_j);
                    sprintf(gText, localization::Tr("town.thieves_guild.primary_stats")  );
                    widgetText_c = static_cast<char*>(H2_ALLOC(strlen(gText) + 1));
                    strcpy(widgetText_c, gText);
                    textControl_p = new textWidget(
                        static_cast<i16>(
                            position_a * THIEVES_PLAYER_COLUMN_WIDTH + THIEVES_PRIMARY_LABEL_X
                        ),
                        THIEVES_PRIMARY_STATS_Y,
                        THIEVES_PRIMARY_LABEL_WIDTH,
                        THIEVES_PRIMARY_WIDGET_HEIGHT,
                        widgetText_c,
                        "smalfont.fnt",
                        FONT_DRAW_DEFAULT,
                        -1,
                        WIDGET_KIND_TEXT,
                        FONT_ALIGN_LEFT
                    );
                    window->AddWidget(textControl_p, -1);

                    sprintf(
                        gText,
                        ""
                    );
                    for (heroPosition_d = 0;
                         heroPosition_d < TOWN_THIEVES_PRIMARY_STAT_COUNT;
                         ++heroPosition_d) {
                        sprintf(
                            statText_h,
                            "%d\n",
                            strongestHero_d->Stats(HeroPrimaryStat(heroPosition_d))
                        );
                        strcat(gText, statText_h);
                    }
                    widgetText_c = static_cast<char*>(H2_ALLOC(strlen(gText) + 1));
                    strcpy(widgetText_c, gText);
                    textControl_p = new textWidget(
                        static_cast<i16>(
                            position_a * THIEVES_PLAYER_COLUMN_WIDTH + THIEVES_PRIMARY_VALUE_X
                        ),
                        THIEVES_PRIMARY_STATS_Y,
                        THIEVES_PRIMARY_VALUE_WIDTH,
                        THIEVES_PRIMARY_WIDGET_HEIGHT,
                        widgetText_c,
                        "smalfont.fnt",
                        FONT_DRAW_DEFAULT,
                        -1,
                        WIDGET_KIND_TEXT,
                        FONT_ALIGN_LEFT
                    );
                    window->AddWidget(textControl_p, -1);
                }

                if (informationLevel < TOWN_THIEVES_INFO_PERSONALITY)
                    goto nextRank;
                {
                    strcpy(gText, cPersonality[H2EnumIndex(gpGame->m_players[rank_a].m_aiDifficulty)]);
                    widgetText_c = static_cast<char*>(H2_ALLOC(strlen(gText) + 1));
                    strcpy(widgetText_c, gText);
                    textControl_p = new textWidget(
                        static_cast<i16>(
                            position_a * THIEVES_PLAYER_COLUMN_WIDTH + THIEVES_PERSONALITY_X
                        ),
                        THIEVES_PERSONALITY_TEXT_Y,
                        THIEVES_PERSONALITY_WIDTH,
                        THIEVES_PERSONALITY_HEIGHT,
                        widgetText_c,
                        "smalfont.fnt",
                        FONT_DRAW_DEFAULT,
                        -1,
                        WIDGET_KIND_UNDIMMED,
                        FONT_ALIGN_CENTER
                    );
                    window->AddWidget(textControl_p, -1);

                    if (informationLevel < TOWN_THIEVES_INFO_STRONGEST_CREATURE)
                        goto nextRank;
                    {
                        strongestCreature_o = CREATURE_NONE;
                        strongestCreatureValue_l = 0;
                        for (heroPosition_d = 0;
                             heroPosition_d < gpGame->m_players[rank_a].m_townCount;
                             ++heroPosition_d) {
                            playerTown_j = gpGame->GetPlayerTown(rank_a, heroPosition_d);
                            for (armySlot_n = 0; armySlot_n < TOWN_ARMY_SLOT_COUNT;
                                 ++armySlot_n) {
                                if (playerTown_j->m_army.m_creatureTypes[armySlot_n]
                                        != CREATURE_NONE
                                    && playerTown_j->m_army.m_creatureCounts[armySlot_n] > 0
                                    && gMonsterDatabase
                                               [H2EnumIndex(playerTown_j->m_army
                                                        .m_creatureTypes[armySlot_n])]
                                                   .fightValue
                                           > strongestCreatureValue_l) {
                                    strongestCreature_o =
                                        playerTown_j->m_army.m_creatureTypes[armySlot_n];
                                    strongestCreatureValue_l =
                                        gMonsterDatabase[H2EnumIndex(playerTown_j->m_army
                                                                 .m_creatureTypes[armySlot_n])]
                                            .fightValue;
                                }
                            }
                        }
                        for (heroPosition_d = 0;
                             heroPosition_d < gpGame->m_players[rank_a].m_heroCount;
                             ++heroPosition_d) {
                            strongestHero_d = gpGame->GetPlayerHero(rank_a, heroPosition_d);
                            for (armySlot_n = 0; armySlot_n < TOWN_ARMY_SLOT_COUNT;
                                 ++armySlot_n) {
                                if (strongestHero_d->m_army.m_creatureTypes[armySlot_n]
                                        != CREATURE_NONE
                                    && strongestHero_d->m_army.m_creatureCounts[armySlot_n] > 0
                                    && gMonsterDatabase
                                               [H2EnumIndex(strongestHero_d->m_army
                                                        .m_creatureTypes[armySlot_n])]
                                                   .fightValue
                                           > strongestCreatureValue_l) {
                                    strongestCreature_o =
                                        strongestHero_d->m_army.m_creatureTypes[armySlot_n];
                                    strongestCreatureValue_l =
                                        gMonsterDatabase[H2EnumIndex(strongestHero_d->m_army
                                                                 .m_creatureTypes[armySlot_n])]
                                            .fightValue;
                                }
                            }
                        }
                        if (strongestCreature_o != CREATURE_NONE) {
                            iconControl_a = new iconWidget(
                                static_cast<i16>(
                                    position_a * THIEVES_PLAYER_COLUMN_WIDTH
                                    + THIEVES_CREATURE_X
                                ),
                                THIEVES_CREATURE_Y,
                                THIEVES_CREATURE_WIDTH,
                                THIEVES_CREATURE_HEIGHT,
                                "mons32.icn",
                                static_cast<i16>(strongestCreature_o),
                                ICON_DRAW_NORMAL,
                                -1,
                                WIDGET_KIND_ICON_CENTERED,
                                1
                            );
                            if (iconControl_a == NULL)
                                MemError();
                            window->AddWidget(iconControl_a, -1);
                        }
                    }
                }
            }
        }
    nextRank:
        ++rank_a;
    }
}

void GetCategoryStats(
    TownThievesGuildCategory category,
    i32l* const stats,
    i8* const order
) {
    i32 player;
    i32 townIndex_c;
    hero* playerHero_h;
    i32 heroIndex_n;
    i32 townCount_k;
    i32 castleCount_p;
    i32 armyStrength;
    town* playerTown;

    for (player = 0; player < gpGame->m_playerCount; ++player) {
        townCount_k = 0;
        castleCount_p = 0;
        order[player] = static_cast<i8>(player);
        if (gpGame->m_playerDead[player]) {
            stats[player] = TOWN_THIEVES_DEAD_PLAYER_STAT;
        } else {
            switch (category) {
                case THIEVES_CATEGORY_TOWNS:
                    for (townIndex_c = 0; townIndex_c < GAME_TOWN_COUNT; ++townIndex_c) {
                        if (gpGame->m_castleRecs[townIndex_c].m_owner == player
                            && (gpGame->m_castleRecs[townIndex_c].m_buildings
                                & H2EnumIndex(TOWN_BUILDING_TENT))) {
                            ++townCount_k;
                        }
                    }
                    stats[player] = townCount_k;
                    break;
                case THIEVES_CATEGORY_CASTLES:
                    for (townIndex_c = 0; townIndex_c < GAME_TOWN_COUNT; ++townIndex_c) {
                        if (gpGame->m_castleRecs[townIndex_c].m_owner == player
                            && (gpGame->m_castleRecs[townIndex_c].m_buildings
                                & H2EnumIndex(TOWN_BUILDING_CASTLE))) {
                            ++castleCount_p;
                        }
                    }
                    stats[player] = castleCount_p;
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
                    for (townIndex_c = 0; townIndex_c < gpGame->m_players[player].m_heroCount;
                         ++townIndex_c) {
                        playerHero_h =
                            gpGame->GetHero(gpGame->m_players[player].m_heroIds[townIndex_c]);
                        for (heroIndex_n = 0; heroIndex_n < TOWN_MAX_ARTIFACTS; ++heroIndex_n) {
                            if (playerHero_h->m_artifacts[heroIndex_n] != ARTIFACT_NONE
                                && playerHero_h->m_artifacts[heroIndex_n] != ARTIFACT_MAGIC_BOOK) {
                                ++stats[player];
                            }
                        }
                    }
                    break;
                case THIEVES_CATEGORY_ARMY_STRENGTH:
                    armyStrength = 0;
                    for (heroIndex_n = 0; heroIndex_n < gpGame->m_players[player].m_heroCount;
                         ++heroIndex_n) {
                        playerHero_h = gpGame->GetPlayerHero(player, heroIndex_n);
                        armyStrength += gpPhilAI->FightValueOfStack(
                            &playerHero_h->m_army,
                            playerHero_h,
                            0,
                            0,
                            0,
                            0
                        );
                    }
                    for (heroIndex_n = 0; heroIndex_n < gpGame->m_players[player].m_townCount;
                         ++heroIndex_n) {
                        playerTown = gpGame->GetPlayerTown(player, heroIndex_n);
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
