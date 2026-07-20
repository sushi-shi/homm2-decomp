#include <va.h>
#include <BASE/BITS.h>
#include <BASE/Misc.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <BASE/textWidget.h>
#include <BASE/executive.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/advManager.h>
#include <SOURCE/GAME.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/RECRUIT.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/game.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/Overview.h>
#include <BASE/widgetKind.h>
#include <SOURCE/playerData.h>
#include <SOURCE/recruitUnit.h>
#include <SOURCE/army.h>
#include <SOURCE/town.h>
#include <stdio.h>
#include <string.h>

namespace {

H2_ENUM_BEGIN(OverviewStorageConstant)
    DYNAMIC_ARRAY_BYTES            = 0x2bc0,
    OVERVIEW_TITLE_COUNT           = 3,
    OVERVIEW_MINE_COUNT_CAPACITY   = IDX(RES_COUNT) + 1,
    OVERVIEW_TROOP_TEXT_CAPACITY   = 6,
    OVERVIEW_PRIMARY_TEXT_CAPACITY = 4,
    OVERVIEW_SKILL_LEVEL_CAPACITY  = 2
H2_ENUM_END(OverviewStorageConstant)

H2_ENUM_BEGIN(OverviewPresentationConstant)
    OVERVIEW_ICON_FILL_COLOR          = 1,
    OVERVIEW_FADE_STEPS               = 8,
    OVERVIEW_LOOPING_SOUND_KEEP_COUNT = 4,
    OVERVIEW_WINDOW_TEXT_ID           = 9,
    OVERVIEW_POINTER_FRAME            = 0
H2_ENUM_END(OverviewPresentationConstant)

H2_ENUM_BEGIN(OverviewSourceLine)
    DYNAMIC_SOURCE_LINE_BASE      = 116,
    DYNAMIC_TOWN_NAME_LINE_OFFSET = 79,
    DYNAMIC_TOWN_ARMY_LINE_OFFSET = 238,
    DYNAMIC_DWELLING_LINE_OFFSET  = 298,
    DYNAMIC_PRIMARY_LINE_OFFSET   = 386,
    DYNAMIC_HERO_ARMY_LINE_OFFSET = 445,
    DYNAMIC_SKILL_LINE_OFFSET     = 507,
    SETUP_SOURCE_LINE_BASE        = 698,
    SETUP_TITLE_LINE_OFFSET       = 42,
    DIALOG_SOURCE_LINE_BASE       = 778,
    DIALOG_TEXT_ARRAY_LINE_OFFSET = 17,
    DIALOG_ICON_ARRAY_LINE_OFFSET = 18,
    DIALOG_TEXT_ARRAY_FREE_OFFSET = 104,
    DIALOG_ICON_ARRAY_FREE_OFFSET = 105
H2_ENUM_END(OverviewSourceLine)

H2_ENUM_BEGIN(OverviewControlId)
    HERO_TAB_WIDGET             = 100,
    TOWN_TAB_WIDGET             = 101,
    TITLE_WIDGET                = 102,
    SCROLL_TRACK_WIDGET         = 110,
    SCROLL_KNOB_WIDGET          = 111,
    SCROLL_UP_WIDGET            = 10,
    SCROLL_DOWN_WIDGET          = 11,
    CLOSE_WIDGET                = 14,
    RESOURCE_FIRST_WIDGET       = 30,
    MINE_FIRST_WIDGET           = 20,
    TOWN_TITLE_WIDGET           = 20,
    HERO_TITLE_WIDGET           = 35,
    DAILY_GOLD_WIDGET           = 40,
    LIGHTHOUSE_WIDGET_ID_OFFSET = 49,
    OVERVIEW_COLUMN_TITLE_FIRST = 110,
    OVERVIEW_DYNAMIC_DRAW_FIRST = OVERVIEW_COLUMN_TITLE_FIRST,
    OVERVIEW_SCROLL_DRAW_LAST   = 111,
    ICON_ROW_BASE               = 200,
    ICON_ROW_LIMIT              = 1000,
    ROW_BACKGROUND_CONTROL      = 2,
    TOWN_NAME_CONTROL           = 3,
    TOWN_SELECT_WIDGET          = 4,
    TOWN_ARMY_FIRST             = 5,
    TOWN_ARMY_LAST              = 9,
    TOWN_ARMY_ALT_FIRST         = 10,
    TOWN_ARMY_ALT_LAST          = 14,
    TOWN_RECRUIT_FIRST          = 15,
    TOWN_RECRUIT_LAST           = 26,
    TOWN_RECRUIT_ALT_FIRST      = 27,
    TOWN_RECRUIT_ALT_LAST       = 38,
    TOWN_HERO_FIRST             = 39,
    TOWN_HERO_PORTRAIT_CONTROL  = 40,
    TOWN_HERO_MANA_CONTROL      = 41,
    TOWN_HERO_MOBILITY_CONTROL  = 43,
    TOWN_HERO_LAST              = TOWN_HERO_MOBILITY_CONTROL,
    TOWN_LOCATOR_CONTROL        = 44,
    HERO_SELECT_FIRST           = 100,
    HERO_PORTRAIT_CONTROL       = 101,
    HERO_MOBILITY_CONTROL       = 103,
    HERO_SELECT_LAST            = HERO_MOBILITY_CONTROL,
    HERO_ARMY_FIRST             = 104,
    HERO_ARMY_LAST              = 108,
    HERO_ARMY_ALT_FIRST         = 109,
    HERO_ARMY_ALT_LAST          = 113,
    HERO_ARTIFACT_FIRST         = 114,
    HERO_ARTIFACT_LAST          = 127,
    HERO_ARTIFACT_FRAME_FIRST   = 128,
    HERO_PRIMARY_TEXT_FIRST     = 142,
    HERO_SKILL_FIRST            = 146,
    HERO_SKILL_LAST             = 153,
    HERO_SKILL_FRAME_FIRST      = 154,
    HERO_SKILL_LEVEL_FIRST      = 162,
    HERO_STAT_FIRST             = 170,
    HERO_STAT_LAST              = 173
H2_ENUM_END(OverviewControlId)

H2_ENUM_BEGIN(OverviewFrameConstant)
    OVERVIEW_TITLE_FRAME_BASE = 6,
    TOWN_TAB_SELECTED_FRAME   = 3,
    TOWN_TAB_UNSELECTED_FRAME = 2
H2_ENUM_END(OverviewFrameConstant)

H2_ENUM_BEGIN(OverviewScrollConstant)
    SCROLL_KNOB_STATIC_Y      = 169,
    SCROLL_MIN_Y              = 18,
    SCROLL_RANGE              = 304,
    SCROLL_KNOB_OFFSET        = 9,
    SCROLL_LAST_PIXEL_ADJUST  = 1,
    SCROLL_SCALE              = 100,
    SCROLL_TRACK_PIXEL_COUNT  = 321,
    SCROLL_SCALED_TRACK_RANGE = SCROLL_TRACK_PIXEL_COUNT * SCROLL_SCALE,
    SCROLL_KNOB_X             = 629,
    SCROLL_KNOB_Y             = 18,
    SCROLL_KNOB_WIDTH         = 8,
    SCROLL_KNOB_HEIGHT        = 17,
    SCROLL_KNOB_FRAME         = 4,
    SCROLL_UPDATE_X           = 627,
    SCROLL_UPDATE_Y           = 17,
    SCROLL_UPDATE_WIDTH       = 11,
    SCROLL_UPDATE_HEIGHT      = 322
H2_ENUM_END(OverviewScrollConstant)

H2_ENUM_BEGIN(OverviewRowLayout)
    ROW_BACKGROUND_X          = 30,
    ROW_BACKGROUND_Y_OFFSET   = 17,
    ROW_BACKGROUND_WIDTH      = 594,
    ROW_BACKGROUND_HEIGHT     = 82,
    ROW_BACKGROUND_FRAME_BASE = 10,
    OVERVIEW_TITLE_Y          = 3,
    OVERVIEW_TITLE_HEIGHT     = 12,
    ROW_DYNAMIC_UPDATE_X      = 30,
    ROW_DYNAMIC_UPDATE_Y      = 16,
    ROW_DYNAMIC_UPDATE_WIDTH  = 610,
    ROW_DYNAMIC_UPDATE_HEIGHT = 341,
    TROOP_ICON_WIDTH          = 42,
    TROOP_ICON_HEIGHT         = 40,
    TROOP_TEXT_WIDTH          = 40,
    TROOP_TEXT_HEIGHT         = 12,
    DETAIL_COLUMN_STRIDE      = 35,
    DETAIL_ROW_STRIDE         = 42,
    DETAIL_FRAME_WIDTH        = 34,
    DETAIL_FRAME_HEIGHT       = 34,
    DETAIL_FRAME_ICON         = 12,
    DETAIL_ICON_WIDTH         = 32,
    DETAIL_ICON_HEIGHT        = 32,
    DETAIL_ICON_WIDGET_COUNT  = 2
H2_ENUM_END(OverviewRowLayout)

H2_ENUM_BEGIN(OverviewTownRowLayout)
    TOWN_NAME_X                     = 35,
    TOWN_NAME_Y_OFFSET              = 80,
    TOWN_NAME_WIDTH                 = 132,
    TOWN_NAME_HEIGHT                = 10,
    TOWN_ICON_X                     = 47,
    TOWN_ICON_Y_OFFSET              = 36,
    TOWN_ICON_WIDTH                 = 46,
    TOWN_ICON_HEIGHT                = 22,
    TOWN_UNFORTIFIED_FRAME_OFFSET   = IDX(FACTION_COUNT),
    TOWN_LOCATOR_X                  = 42,
    TOWN_LOCATOR_Y_OFFSET           = 31,
    TOWN_LOCATOR_FRAME              = 24,
    TOWN_HERO_FRAME_X               = 112,
    TOWN_HERO_FRAME_Y_OFFSET        = 36,
    TOWN_HERO_FRAME_WIDTH           = 46,
    TOWN_HERO_FRAME_HEIGHT          = 22,
    TOWN_HERO_PORTRAIT_X            = 107,
    TOWN_HERO_PORTRAIT_Y_OFFSET     = 31,
    TOWN_HERO_PORTRAIT_WIDTH        = 51,
    TOWN_HERO_PORTRAIT_HEIGHT       = 27,
    TOWN_MOBILITY_X                 = 112,
    TOWN_MOBILITY_Y_OFFSET          = 36,
    TOWN_MANA_X                     = 151,
    TOWN_MANA_Y_OFFSET              = 36,
    CAPTAIN_MANA_LOW                = 10,
    CAPTAIN_MANA_HIGH               = 20,
    TOWN_TROOP_COLUMN_STRIDE        = 41,
    TOWN_TROOP_FIRST_X              = 173,
    TOWN_TROOP_ICON_Y_OFFSET        = 35,
    TOWN_TROOP_TEXT_Y_OFFSET        = 81,
    TOWN_DWELLING_FIRST_X           = 378,
    TOWN_DWELLING_BUILDING_BIT_BASE = IDX(BUILDING_SLOT_DWELLING_FIRST)
H2_ENUM_END(OverviewTownRowLayout)

H2_ENUM_BEGIN(OverviewHeroRowLayout)
    HERO_FRAME_X                 = 35,
    HERO_FRAME_Y_OFFSET          = 21,
    HERO_FRAME_WIDTH             = 46,
    HERO_FRAME_HEIGHT            = 22,
    HERO_PORTRAIT_X              = 30,
    HERO_PORTRAIT_Y_OFFSET       = 16,
    HERO_PORTRAIT_WIDTH          = 51,
    HERO_PORTRAIT_HEIGHT         = 27,
    HERO_MOBILITY_X              = 35,
    HERO_MOBILITY_Y_OFFSET       = 21,
    HERO_MANA_X                  = 74,
    HERO_MANA_Y_OFFSET           = 21,
    HERO_PRIMARY_COLUMN_STRIDE   = 35,
    HERO_PRIMARY_TEXT_FIRST_X    = 105,
    HERO_PRIMARY_TEXT_Y_OFFSET   = 39,
    HERO_PRIMARY_TEXT_WIDTH      = 14,
    HERO_PRIMARY_TEXT_HEIGHT     = 8,
    HERO_PRIMARY_ICON_FIRST_X    = 87,
    HERO_PRIMARY_ICON_Y_OFFSET   = 17,
    HERO_PRIMARY_ICON_WIDTH      = 35,
    HERO_PRIMARY_ICON_HEIGHT     = 30,
    HERO_PRIMARY_ICON_FRAME      = 15,
    HERO_TROOP_COLUMN_STRIDE     = 40,
    HERO_TROOP_FIRST_X           = 30,
    HERO_TROOP_ICON_Y_OFFSET     = 47,
    HERO_TROOP_TEXT_Y_OFFSET     = 87,
    HERO_SKILL_FRAME_FIRST_X     = 233,
    HERO_SKILL_FRAME_Y_OFFSET    = 20,
    HERO_SKILL_ICON_FIRST_X      = 234,
    HERO_SKILL_ICON_Y_OFFSET     = 21,
    HERO_SKILL_LEVEL_FIRST_X     = 255,
    HERO_SKILL_LEVEL_Y_OFFSET    = 45,
    HERO_SKILL_LEVEL_WIDTH       = 8,
    HERO_SKILL_LEVEL_HEIGHT      = 8,
    HERO_ARTIFACT_FRAME_FIRST_X  = 378,
    HERO_ARTIFACT_FRAME_Y_OFFSET = 20,
    HERO_ARTIFACT_ICON_FIRST_X   = 379,
    HERO_ARTIFACT_ICON_Y_OFFSET  = 21
H2_ENUM_END(OverviewHeroRowLayout)

H2_ENUM_BEGIN(OverviewDwellingOrder)
    DWELLING_ORDER_LEVEL_1          = 0,
    DWELLING_ORDER_LEVEL_2          = 1,
    DWELLING_ORDER_UPGRADED_LEVEL_2 = 2,
    DWELLING_ORDER_LEVEL_3          = 3,
    DWELLING_ORDER_UPGRADED_LEVEL_3 = 4,
    DWELLING_ORDER_LEVEL_4          = 5,
    DWELLING_ORDER_UPGRADED_LEVEL_4 = 6,
    DWELLING_ORDER_LEVEL_5          = 7,
    DWELLING_ORDER_UPGRADED_LEVEL_5 = 8,
    DWELLING_ORDER_LEVEL_6          = 9,
    DWELLING_ORDER_UPGRADED_LEVEL_6 = 10
H2_ENUM_END(OverviewDwellingOrder)

H2_ENUM_BEGIN(OverviewDwellingIndex)
    DWELLING_INDEX_LEVEL_1           = 0,
    DWELLING_INDEX_LEVEL_2           = 1,
    DWELLING_INDEX_LEVEL_3           = 2,
    DWELLING_INDEX_LEVEL_4           = 3,
    DWELLING_INDEX_LEVEL_5           = 4,
    DWELLING_INDEX_LEVEL_6           = 5,
    DWELLING_INDEX_UPGRADED_LEVEL_2  = 6,
    DWELLING_INDEX_UPGRADED_LEVEL_3  = 7,
    DWELLING_INDEX_UPGRADED_LEVEL_4  = 8,
    DWELLING_INDEX_UPGRADED_LEVEL_5  = 9,
    DWELLING_INDEX_UPGRADED_LEVEL_6  = 10,
    DWELLING_INDEX_ALTERNATE_LEVEL_6 = 11
H2_ENUM_END(OverviewDwellingIndex)

H2_ENUM_BEGIN(OverviewDialogConstant)
    OVERVIEW_HANDLER_CONTINUE = 1,
    OVERVIEW_HANDLER_CLOSE    = 2,
    OVERVIEW_RETURN_ID_NONE   = -1,
    OVERVIEW_VIEW_ARMY_X      = 119,
    OVERVIEW_VIEW_ARMY_Y      = 20
H2_ENUM_END(OverviewDialogConstant)

}

#define OVERVIEW_SCROLL_KNOB_RANGE_FLOAT 303.0
#define OVERVIEW_SCROLL_MIN_Y_FLOAT 18.0f
#define OVERVIEW_SCROLL_ROUNDING_OFFSET 0.4

#define RETAIL_FILE const_cast<char*>("I:\\Projects\\Heroes\\Prog\\SOURCE\\Overview.cpp")

#define OVERVIEW_TEXT_WIDGET_ROWS (reinterpret_cast<OverviewTextWidgetRow*>(textWidgetDynamic))
#define OVERVIEW_ICON_WIDGET_ROWS (reinterpret_cast<OverviewIconWidgetRow*>(iconWidgetDynamic))

VA(0x00407870, 0x223e)
void game::SetupDynamicStuff(i32 redraw, i32 updateKnob, i32 forceUpdate) {
    DATA(0x004eda04) static i16 overviewDynamicSourceLine = DYNAMIC_SOURCE_LINE_BASE;
    i32 row;
    i32 item;
    i32 textItemCount;
    i32 iconCount;
    i32 rowWidgetId6;
    char* valueText0;

    if (updateKnob) {
        if (giOverviewItems[IDX(giOverviewType)] <= OVERVIEW_VISIBLE_ROWS) {
            OVScrollKnob->m_y = SCROLL_KNOB_STATIC_Y;
        } else {
            i32 scrollDivisor = giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
            float knobScale = OVERVIEW_SCROLL_KNOB_RANGE_FLOAT / scrollDivisor;
            OVScrollKnob->m_y = static_cast<i16>(
                giOverviewTop[IDX(giOverviewType)] * knobScale + OVERVIEW_SCROLL_MIN_Y_FLOAT
                + OVERVIEW_SCROLL_ROUNDING_OFFSET
            );
        }
    }

    if (!forceUpdate && giOverviewType == iLastDynamicType
        && giOverviewTop[IDX(iLastDynamicType)] == iLastDynamicTop) {
        return;
    }

    for (row = 0; row < OVERVIEW_VISIBLE_ROWS; row++) {
        for (item = 0; item < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; item++) {
            if (*(textWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item) != NULL) {
                overWin->RemoveWidget(
                    *(textWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item)
                );
                delete *(textWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item);
                *(textWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item) = NULL;
            }
            if (*(iconWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item) != NULL) {
                overWin->RemoveWidget(
                    *(iconWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item)
                );
                delete *(iconWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item);
                *(iconWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item) = NULL;
            }
        }
    }

    for (row = 0; row < OVERVIEW_VISIBLE_ROWS; row++) {
        textItemCount = 0;
        iconCount = textItemCount;
        rowWidgetId6 = row * OVERVIEW_ROW_ID_STRIDE + ICON_ROW_BASE;

        if (giOverviewTop[IDX(giOverviewType)] + row >= giOverviewItems[IDX(giOverviewType)]) {
            break;
        }

        OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
            ROW_BACKGROUND_X,
            static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + ROW_BACKGROUND_Y_OFFSET),
            ROW_BACKGROUND_WIDTH,
            ROW_BACKGROUND_HEIGHT,
            const_cast<char*>("overview.icn"),
            static_cast<i16>(IDX(giOverviewType) + ROW_BACKGROUND_FRAME_BASE),
            0,
            static_cast<i16>(rowWidgetId6 + ROW_BACKGROUND_CONTROL),
            WIDGET_KIND_ICON_DIRECT,
            OVERVIEW_ICON_FILL_COLOR
        );
        if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
            MemError();
        }
        overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
        iconCount++;

        if (giOverviewType == OVERVIEW_TOWNS) {
            hero* heroData0;
            town* record =
                GetTown(gpCurPlayer->m_townIds[giOverviewTop[IDX(giOverviewType)] + row]);
            {
                valueText0 = static_cast<char*>(H2_ALLOC(
                    strlen(record->m_name) + 1,
                    overviewDynamicSourceLine + DYNAMIC_TOWN_NAME_LINE_OFFSET
                ));
                strcpy(valueText0, record->m_name);

                OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount] = new textWidget(
                    TOWN_NAME_X,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + TOWN_NAME_Y_OFFSET),
                    TOWN_NAME_WIDTH,
                    TOWN_NAME_HEIGHT,
                    valueText0,
                    const_cast<char*>("smalfont.fnt"),
                    FONT_DRAW_DEFAULT,
                    static_cast<i16>(rowWidgetId6 + TOWN_NAME_CONTROL),
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
                overWin->AddWidget(OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount], -1);
                textItemCount++;
            }

            {
                i32 townFrame = IDX(record->m_type);
                if ((record->m_buildings & IDX(TOWN_BUILDING_CASTLE)) == 0) {
                    townFrame += TOWN_UNFORTIFIED_FRAME_OFFSET;
                }
                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    TOWN_ICON_X,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + TOWN_ICON_Y_OFFSET),
                    TOWN_ICON_WIDTH,
                    TOWN_ICON_HEIGHT,
                    const_cast<char*>("minitown.icn"),
                    townFrame,
                    0,
                    static_cast<i16>(rowWidgetId6 + TOWN_SELECT_WIDGET),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                iconCount++;
            }

            if (BitTest(gpGame->m_knownTowns, static_cast<u32>(record->m_id))) {
                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    TOWN_LOCATOR_X,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + TOWN_LOCATOR_Y_OFFSET),
                    0,
                    0,
                    const_cast<char*>("locators.icn"),
                    TOWN_LOCATOR_FRAME,
                    0,
                    static_cast<i16>(rowWidgetId6 + TOWN_LOCATOR_CONTROL),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                iconCount++;
            }

            i32 hasCaptain = 0;
            heroData0 = NULL;
            if (record->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
                heroData0 = GetHero(record->m_occupyingHeroId);
            } else {
                if ((record->m_buildings & IDX(TOWN_BUILDING_CAPTAIN_QUARTERS)) != 0) {
                    hasCaptain = 1;
                }
            }

            if (heroData0 != NULL || hasCaptain) {
                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    TOWN_HERO_FRAME_X,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + TOWN_HERO_FRAME_Y_OFFSET),
                    TOWN_HERO_FRAME_WIDTH,
                    TOWN_HERO_FRAME_HEIGHT,
                    const_cast<char*>("portxtra.icn"),
                    0,
                    0,
                    static_cast<i16>(rowWidgetId6 + TOWN_HERO_FIRST),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                iconCount++;

                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    TOWN_HERO_PORTRAIT_X,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + TOWN_HERO_PORTRAIT_Y_OFFSET),
                    TOWN_HERO_PORTRAIT_WIDTH,
                    TOWN_HERO_PORTRAIT_HEIGHT,
                    const_cast<char*>(hasCaptain ? "minicapt.icn" : "miniport.icn"),
                    hasCaptain ? static_cast<u8>(static_cast<FactionType>(record->m_type))
                               : static_cast<u8>(heroData0->m_id),
                    0,
                    static_cast<i16>(rowWidgetId6 + TOWN_HERO_PORTRAIT_CONTROL),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                iconCount++;

                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    TOWN_MOBILITY_X,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + TOWN_MOBILITY_Y_OFFSET),
                    0,
                    0,
                    const_cast<char*>("mobility.icn"),
                    static_cast<i16>(
                        GetMobilityFrame(hasCaptain ? 0 : heroData0->m_remainingMobility)
                    ),
                    0,
                    static_cast<i16>(rowWidgetId6 + TOWN_HERO_MOBILITY_CONTROL),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                iconCount++;

                i32 captainMana = record->m_type == FACTION_BARBARIAN
                                          || record->m_type == FACTION_KNIGHT
                                      ? CAPTAIN_MANA_LOW
                                      : CAPTAIN_MANA_HIGH;
                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    TOWN_MANA_X,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + TOWN_MANA_Y_OFFSET),
                    0,
                    0,
                    const_cast<char*>("mana.icn"),
                    static_cast<i16>(
                        GetManaFrame(hasCaptain ? captainMana : heroData0->m_spellPoints)
                    ),
                    0,
                    static_cast<i16>(rowWidgetId6 + TOWN_HERO_MANA_CONTROL),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                iconCount++;
            }

            {
                i32 displayedTroops = 0;
                for (item = 0; item < OVERVIEW_TROOP_SLOTS; item++) {
                    if (record->m_army.m_creatureTypes[item] != IDX(CREATURE_NONE)
                        && record->m_army.m_creatureCounts[item] > 0) {
                        OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                            static_cast<i16>(
                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_TROOP_FIRST_X
                            ),
                            static_cast<i16>(
                                row * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_ICON_Y_OFFSET
                            ),
                            TROOP_ICON_WIDTH,
                            TROOP_ICON_HEIGHT,
                            const_cast<char*>("mons32.icn"),
                            static_cast<i16>(record->m_army.m_creatureTypes[item]),
                            0,
                            static_cast<i16>(rowWidgetId6 + item + TOWN_ARMY_FIRST),
                            WIDGET_KIND_ICON_CENTERED,
                            OVERVIEW_ICON_FILL_COLOR
                        );
                        if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                            MemError();
                        }
                        overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                        iconCount++;

                        valueText0 = static_cast<char*>(H2_ALLOC(
                            OVERVIEW_TROOP_TEXT_CAPACITY,
                            overviewDynamicSourceLine + DYNAMIC_TOWN_ARMY_LINE_OFFSET
                        ));
                        sprintf(
                            valueText0,
                            "%d",
                            static_cast<i32>(record->m_army.m_creatureCounts[item])
                        );
                        OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount] = new textWidget(
                            static_cast<i16>(
                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_TROOP_FIRST_X
                            ),
                            static_cast<i16>(
                                row * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_TEXT_Y_OFFSET
                            ),
                            TROOP_TEXT_WIDTH,
                            TROOP_TEXT_HEIGHT,
                            valueText0,
                            const_cast<char*>("smalfont.fnt"),
                            FONT_DRAW_DEFAULT,
                            static_cast<i16>(rowWidgetId6 + item + TOWN_ARMY_ALT_FIRST),
                            WIDGET_KIND_TEXT,
                            FONT_ALIGN_CENTER
                        );
                        overWin->AddWidget(OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount], -1);
                        textItemCount++;
                        displayedTroops++;
                    }
                }
            }

            {
                i32 displayedTroops = 0;
                for (item = 0; item < OVERVIEW_DWELLING_SLOTS; item++) {
                    i32 detailIndex;
                    switch (item) {
                        case DWELLING_ORDER_LEVEL_1:
                            detailIndex = DWELLING_INDEX_LEVEL_1;
                            break;
                        case DWELLING_ORDER_LEVEL_2:
                            detailIndex = DWELLING_INDEX_LEVEL_2;
                            break;
                        case DWELLING_ORDER_UPGRADED_LEVEL_2:
                            detailIndex = DWELLING_INDEX_UPGRADED_LEVEL_2;
                            break;
                        case DWELLING_ORDER_LEVEL_3:
                            detailIndex = DWELLING_INDEX_LEVEL_3;
                            break;
                        case DWELLING_ORDER_UPGRADED_LEVEL_3:
                            detailIndex = DWELLING_INDEX_UPGRADED_LEVEL_3;
                            break;
                        case DWELLING_ORDER_LEVEL_4:
                            detailIndex = DWELLING_INDEX_LEVEL_4;
                            break;
                        case DWELLING_ORDER_UPGRADED_LEVEL_4:
                            detailIndex = DWELLING_INDEX_UPGRADED_LEVEL_4;
                            break;
                        case DWELLING_ORDER_LEVEL_5:
                            detailIndex = DWELLING_INDEX_LEVEL_5;
                            break;
                        case DWELLING_ORDER_UPGRADED_LEVEL_5:
                            detailIndex = DWELLING_INDEX_UPGRADED_LEVEL_5;
                            break;
                        case DWELLING_ORDER_LEVEL_6:
                            detailIndex = DWELLING_INDEX_LEVEL_6;
                            break;
                        case DWELLING_ORDER_UPGRADED_LEVEL_6:
                            detailIndex = DWELLING_INDEX_UPGRADED_LEVEL_6;
                            break;
                        default:
                            detailIndex = DWELLING_INDEX_ALTERNATE_LEVEL_6;
                            break;
                    }

                    if ((record->m_buildings
                         & (1 << (detailIndex + TOWN_DWELLING_BUILDING_BIT_BASE)))
                        != 0) {
                        OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                            static_cast<i16>(
                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE
                                + TOWN_DWELLING_FIRST_X
                            ),
                            static_cast<i16>(
                                row * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_ICON_Y_OFFSET
                            ),
                            TROOP_ICON_WIDTH,
                            TROOP_ICON_HEIGHT,
                            const_cast<char*>("mons32.icn"),
                            static_cast<i16>(
                                gDwellingType[IDX(record->m_type)][detailIndex]
                            ),
                            0,
                            static_cast<i16>(rowWidgetId6 + detailIndex + TOWN_RECRUIT_FIRST),
                            WIDGET_KIND_ICON_CENTERED,
                            OVERVIEW_ICON_FILL_COLOR
                        );
                        if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                            MemError();
                        }
                        overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                        iconCount++;

                        valueText0 = static_cast<char*>(H2_ALLOC(
                            OVERVIEW_TROOP_TEXT_CAPACITY,
                            overviewDynamicSourceLine + DYNAMIC_DWELLING_LINE_OFFSET
                        ));
                        sprintf(
                            valueText0,
                            "%d",
                            static_cast<i32>(record->m_garrison[detailIndex])
                        );
                        OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount] = new textWidget(
                            static_cast<i16>(
                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE
                                + TOWN_DWELLING_FIRST_X
                            ),
                            static_cast<i16>(
                                row * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_TEXT_Y_OFFSET
                            ),
                            TROOP_TEXT_WIDTH,
                            TROOP_TEXT_HEIGHT,
                            valueText0,
                            const_cast<char*>("smalfont.fnt"),
                            FONT_DRAW_DEFAULT,
                            static_cast<i16>(rowWidgetId6 + detailIndex + TOWN_RECRUIT_ALT_FIRST),
                            WIDGET_KIND_TEXT,
                            FONT_ALIGN_CENTER
                        );
                        overWin->AddWidget(OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount], -1);
                        textItemCount++;
                        displayedTroops++;
                    }
                }
            }
        } else {
            hero* heroData0;
            i32 detailRow;
            i32 detailColumn;
            i32 detailIndex;
            heroData0 =
                &m_heroRecs[gpCurPlayer->m_heroIds[giOverviewTop[IDX(giOverviewType)] + row]];

            OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                HERO_FRAME_X,
                static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + HERO_FRAME_Y_OFFSET),
                HERO_FRAME_WIDTH,
                HERO_FRAME_HEIGHT,
                const_cast<char*>("portxtra.icn"),
                0,
                0,
                static_cast<i16>(rowWidgetId6 + HERO_SELECT_FIRST),
                WIDGET_KIND_ICON_DIRECT,
                OVERVIEW_ICON_FILL_COLOR
            );
            if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                MemError();
            }
            overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
            iconCount++;

            OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                HERO_PORTRAIT_X,
                static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + HERO_PORTRAIT_Y_OFFSET),
                HERO_PORTRAIT_WIDTH,
                HERO_PORTRAIT_HEIGHT,
                const_cast<char*>("miniport.icn"),
                static_cast<i16>(heroData0->m_id),
                0,
                static_cast<i16>(rowWidgetId6 + HERO_PORTRAIT_CONTROL),
                WIDGET_KIND_ICON_DIRECT,
                OVERVIEW_ICON_FILL_COLOR
            );
            if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                MemError();
            }
            overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
            iconCount++;

            OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                HERO_MOBILITY_X,
                static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + HERO_MOBILITY_Y_OFFSET),
                0,
                0,
                const_cast<char*>("mobility.icn"),
                static_cast<i16>(GetMobilityFrame(heroData0->m_remainingMobility)),
                0,
                static_cast<i16>(rowWidgetId6 + HERO_MOBILITY_CONTROL),
                WIDGET_KIND_ICON_DIRECT,
                OVERVIEW_ICON_FILL_COLOR
            );
            if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                MemError();
            }
            overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
            iconCount++;

            OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                HERO_MANA_X,
                static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + HERO_MANA_Y_OFFSET),
                0,
                0,
                const_cast<char*>("mana.icn"),
                static_cast<i16>(GetManaFrame(heroData0->m_spellPoints)),
                0,
                static_cast<i16>(rowWidgetId6 + TOWN_HERO_MANA_CONTROL),
                WIDGET_KIND_ICON_DIRECT,
                OVERVIEW_ICON_FILL_COLOR
            );
            if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                MemError();
            }
            overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
            iconCount++;

            for (item = 0; item < HERO_PRIMARY_STAT_COUNT; item++) {
                valueText0 = static_cast<char*>(H2_ALLOC(
                    OVERVIEW_PRIMARY_TEXT_CAPACITY,
                    overviewDynamicSourceLine + DYNAMIC_PRIMARY_LINE_OFFSET
                ));
                sprintf(
                    valueText0,
                    "%d",
                    static_cast<i32>(heroData0->Stats(HeroPrimaryStat(item)))
                );
                OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount] = new textWidget(
                    static_cast<i16>(
                        item * HERO_PRIMARY_COLUMN_STRIDE + HERO_PRIMARY_TEXT_FIRST_X
                    ),
                    static_cast<i16>(
                        row * OVERVIEW_ROW_HEIGHT + HERO_PRIMARY_TEXT_Y_OFFSET
                    ),
                    HERO_PRIMARY_TEXT_WIDTH,
                    HERO_PRIMARY_TEXT_HEIGHT,
                    valueText0,
                    const_cast<char*>("smalfont.fnt"),
                    FONT_DRAW_DEFAULT,
                    static_cast<i16>(rowWidgetId6 + item + HERO_PRIMARY_TEXT_FIRST),
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_RIGHT
                );
                overWin->AddWidget(OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount], -1);
                textItemCount++;
            }

            for (item = 0; item < HERO_PRIMARY_STAT_COUNT; item++) {
                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    static_cast<i16>(
                        item * HERO_PRIMARY_COLUMN_STRIDE + HERO_PRIMARY_ICON_FIRST_X
                    ),
                    static_cast<i16>(
                        row * OVERVIEW_ROW_HEIGHT + HERO_PRIMARY_ICON_Y_OFFSET
                    ),
                    HERO_PRIMARY_ICON_WIDTH,
                    HERO_PRIMARY_ICON_HEIGHT,
                    const_cast<char*>("overview.icn"),
                    HERO_PRIMARY_ICON_FRAME,
                    0,
                    static_cast<i16>(rowWidgetId6 + item + HERO_STAT_FIRST),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                iconCount++;
            }

            {
                i32 displayedTroops = 0;
                for (item = 0; item < OVERVIEW_TROOP_SLOTS; item++) {
                    if (heroData0->m_army.m_creatureTypes[item] != IDX(CREATURE_NONE)
                        && heroData0->m_army.m_creatureCounts[item] > 0) {
                        OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                            static_cast<i16>(
                                displayedTroops * HERO_TROOP_COLUMN_STRIDE + HERO_TROOP_FIRST_X
                            ),
                            static_cast<i16>(
                                row * OVERVIEW_ROW_HEIGHT + HERO_TROOP_ICON_Y_OFFSET
                            ),
                            TROOP_ICON_WIDTH,
                            TROOP_ICON_HEIGHT,
                            const_cast<char*>("mons32.icn"),
                            static_cast<i16>(heroData0->m_army.m_creatureTypes[item]),
                            0,
                            static_cast<i16>(rowWidgetId6 + item + HERO_ARMY_FIRST),
                            WIDGET_KIND_ICON_CENTERED,
                            OVERVIEW_ICON_FILL_COLOR
                        );
                        if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                            MemError();
                        }
                        overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                        iconCount++;

                        valueText0 = static_cast<char*>(H2_ALLOC(
                            OVERVIEW_TROOP_TEXT_CAPACITY,
                            overviewDynamicSourceLine + DYNAMIC_HERO_ARMY_LINE_OFFSET
                        ));
                        sprintf(
                            valueText0,
                            "%d",
                            static_cast<i32>(heroData0->m_army.m_creatureCounts[item])
                        );
                        OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount] = new textWidget(
                            static_cast<i16>(
                                displayedTroops * HERO_TROOP_COLUMN_STRIDE + HERO_TROOP_FIRST_X
                            ),
                            static_cast<i16>(
                                row * OVERVIEW_ROW_HEIGHT + HERO_TROOP_TEXT_Y_OFFSET
                            ),
                            TROOP_TEXT_WIDTH,
                            TROOP_TEXT_HEIGHT,
                            valueText0,
                            const_cast<char*>("smalfont.fnt"),
                            FONT_DRAW_DEFAULT,
                            static_cast<i16>(rowWidgetId6 + item + HERO_ARMY_ALT_FIRST),
                            WIDGET_KIND_TEXT,
                            FONT_ALIGN_CENTER
                        );
                        overWin->AddWidget(OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount], -1);
                        textItemCount++;
                        displayedTroops++;
                    }
                }
            }

            for (item = 0; item < OVERVIEW_SECONDARY_SKILL_SLOTS; item++) {
                detailIndex = heroData0->GetNthSS(item);
                if (detailIndex != HERO_SECONDARY_SKILL_NONE) {
                    detailRow = item / OVERVIEW_SECONDARY_SKILL_COLUMNS;
                    detailColumn = item % OVERVIEW_SECONDARY_SKILL_COLUMNS;
                    OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                        static_cast<i16>(
                            detailColumn * DETAIL_COLUMN_STRIDE + HERO_SKILL_FRAME_FIRST_X
                        ),
                        static_cast<i16>(
                            row * OVERVIEW_ROW_HEIGHT + detailRow * DETAIL_ROW_STRIDE
                            + HERO_SKILL_FRAME_Y_OFFSET
                        ),
                        DETAIL_FRAME_WIDTH,
                        DETAIL_FRAME_HEIGHT,
                        const_cast<char*>("overview.icn"),
                        DETAIL_FRAME_ICON,
                        0,
                        static_cast<i16>(rowWidgetId6 + item + HERO_SKILL_FRAME_FIRST),
                        WIDGET_KIND_ICON_DIRECT,
                        OVERVIEW_ICON_FILL_COLOR
                    );
                    if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                        MemError();
                    }
                    overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);

                    OVERVIEW_ICON_WIDGET_ROWS[row][iconCount + 1] = new iconWidget(
                        static_cast<i16>(
                            detailColumn * DETAIL_COLUMN_STRIDE + HERO_SKILL_ICON_FIRST_X
                        ),
                        static_cast<i16>(
                            row * OVERVIEW_ROW_HEIGHT + detailRow * DETAIL_ROW_STRIDE
                            + HERO_SKILL_ICON_Y_OFFSET
                        ),
                        DETAIL_ICON_WIDTH,
                        DETAIL_ICON_HEIGHT,
                        const_cast<char*>("miniss.icn"),
                        static_cast<i16>(detailIndex),
                        0,
                        static_cast<i16>(rowWidgetId6 + item + HERO_SKILL_FIRST),
                        WIDGET_KIND_ICON_DIRECT,
                        OVERVIEW_ICON_FILL_COLOR
                    );
                    if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount + 1] == NULL) {
                        MemError();
                    }
                    overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount + 1], -1);
                    iconCount += DETAIL_ICON_WIDGET_COUNT;

                    valueText0 = static_cast<char*>(H2_ALLOC(
                        OVERVIEW_SKILL_LEVEL_CAPACITY,
                        overviewDynamicSourceLine + DYNAMIC_SKILL_LINE_OFFSET
                    ));
                    sprintf(valueText0, "%d", static_cast<i32>(heroData0->GetSSLevel(detailIndex)));
                    OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount] = new textWidget(
                        static_cast<i16>(
                            detailColumn * DETAIL_COLUMN_STRIDE + HERO_SKILL_LEVEL_FIRST_X
                        ),
                        static_cast<i16>(
                            row * OVERVIEW_ROW_HEIGHT + detailRow * DETAIL_ROW_STRIDE
                            + HERO_SKILL_LEVEL_Y_OFFSET
                        ),
                        HERO_SKILL_LEVEL_WIDTH,
                        HERO_SKILL_LEVEL_HEIGHT,
                        valueText0,
                        const_cast<char*>("smalfont.fnt"),
                        FONT_DRAW_DEFAULT,
                        static_cast<i16>(rowWidgetId6 + item + HERO_SKILL_LEVEL_FIRST),
                        WIDGET_KIND_TEXT,
                        FONT_ALIGN_RIGHT
                    );
                    overWin->AddWidget(OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount], -1);
                    textItemCount++;
                }
            }

            i32 displayedArtifacts = 0;
            for (item = 0; item < OVERVIEW_ARTIFACT_SLOTS; item++) {
                if (heroData0->m_artifacts[item] != IDX(ARTIFACT_NONE)) {
                    detailRow = displayedArtifacts / OVERVIEW_ARTIFACT_COLUMNS;
                    detailColumn = displayedArtifacts % OVERVIEW_ARTIFACT_COLUMNS;
                    OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                        static_cast<i16>(
                            detailColumn * DETAIL_COLUMN_STRIDE + HERO_ARTIFACT_FRAME_FIRST_X
                        ),
                        static_cast<i16>(
                            row * OVERVIEW_ROW_HEIGHT + detailRow * DETAIL_ROW_STRIDE
                            + HERO_ARTIFACT_FRAME_Y_OFFSET
                        ),
                        DETAIL_FRAME_WIDTH,
                        DETAIL_FRAME_HEIGHT,
                        const_cast<char*>("overview.icn"),
                        DETAIL_FRAME_ICON,
                        0,
                        static_cast<i16>(rowWidgetId6 + item + HERO_ARTIFACT_FRAME_FIRST),
                        WIDGET_KIND_ICON_DIRECT,
                        OVERVIEW_ICON_FILL_COLOR
                    );
                    if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                        MemError();
                    }
                    overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);

                    OVERVIEW_ICON_WIDGET_ROWS[row][iconCount + 1] = new iconWidget(
                        static_cast<i16>(
                            detailColumn * DETAIL_COLUMN_STRIDE + HERO_ARTIFACT_ICON_FIRST_X
                        ),
                        static_cast<i16>(
                            row * OVERVIEW_ROW_HEIGHT + detailRow * DETAIL_ROW_STRIDE
                            + HERO_ARTIFACT_ICON_Y_OFFSET
                        ),
                        DETAIL_ICON_WIDTH,
                        DETAIL_ICON_HEIGHT,
                        const_cast<char*>("artfx.icn"),
                        static_cast<i16>(heroData0->m_artifacts[item]),
                        0,
                        static_cast<i16>(rowWidgetId6 + item + HERO_ARTIFACT_FIRST),
                        WIDGET_KIND_ICON_DIRECT,
                        OVERVIEW_ICON_FILL_COLOR
                    );
                    if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount + 1] == NULL) {
                        MemError();
                    }
                    overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount + 1], -1);
                    iconCount += DETAIL_ICON_WIDGET_COUNT;
                    displayedArtifacts++;
                }
            }
        }
    }

    if (redraw) {
        overWin->DrawWindow(
            WINDOW_FLAG_NONE,
            OVERVIEW_DYNAMIC_DRAW_FIRST,
            ICON_ROW_LIMIT - 1
        );
        gpWindowManager->UpdateScreenRegion(
            ROW_DYNAMIC_UPDATE_X,
            ROW_DYNAMIC_UPDATE_Y,
            ROW_DYNAMIC_UPDATE_WIDTH,
            ROW_DYNAMIC_UPDATE_HEIGHT
        );
    }
}

VA(0x00409aae, 0x357)
void game::SetupNewOverviewType(OverviewType overviewType, i32 redrawFrom) {
    DATA(0x004edca8) static i16 overviewSetupSourceLine = SETUP_SOURCE_LINE_BASE;
    i32 title;
    tag_message message;
    char* titleCopy;

    giOverviewType = overviewType;
    iOverviewItems =
        giOverviewType == OVERVIEW_HEROES ? gpCurPlayer->m_heroCount : gpCurPlayer->m_townCount;

    if (iOverviewItems - OVERVIEW_VISIBLE_ROWS < giOverviewTop[IDX(giOverviewType)]) {
        giOverviewTop[IDX(giOverviewType)] = iOverviewItems - OVERVIEW_VISIBLE_ROWS;
    }
    if (giOverviewTop[IDX(giOverviewType)] < 0) {
        giOverviewTop[IDX(giOverviewType)] = 0;
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = OVERVIEW_WIDGET_SET_FRAME;
    message.payload.widget.id = TITLE_WIDGET;
    message.payload.widget.data.value = IDX(giOverviewType) + OVERVIEW_TITLE_FRAME_BASE;
    overWin->BroadcastMessage(message);
    message.payload.widget.command = OVERVIEW_WIDGET_SET_FRAME;
    message.payload.widget.id = HERO_TAB_WIDGET;
    message.payload.widget.data.value = giOverviewType == OVERVIEW_HEROES;
    overWin->BroadcastMessage(message);
    message.payload.widget.command = OVERVIEW_WIDGET_SET_FRAME;
    message.payload.widget.id = TOWN_TAB_WIDGET;
    if (giOverviewType == OVERVIEW_TOWNS) {
        message.payload.widget.data.value = TOWN_TAB_SELECTED_FRAME;
    } else {
        message.payload.widget.data.value = TOWN_TAB_UNSELECTED_FRAME;
    }
    overWin->BroadcastMessage(message);

    // Mode-indexed retail geometry for the three overview column titles.
    // NOLINTBEGIN(readability-magic-numbers)
    i16 titleLefts[IDX(OVERVIEW_TYPE_COUNT)][OVERVIEW_TITLE_COUNT] = {
        {35, 232, 377},
        {35, 177, 379}
    };
    i16 titleWidth[IDX(OVERVIEW_TYPE_COUNT)][OVERVIEW_TITLE_COUNT] = {
        {194, 142, 241},
        {140, 199, 239}
    };
    // NOLINTEND(readability-magic-numbers)

    for (title = 0; title < OVERVIEW_TITLE_COUNT; title++) {
        if (textWidgetTitle[title] != NULL) {
            overWin->RemoveWidget(textWidgetTitle[title]);
            delete textWidgetTitle[title];
            textWidgetTitle[title] = NULL;
        }
    }
    for (title = 0; title < OVERVIEW_TITLE_COUNT; title++) {
        titleCopy = static_cast<char*>(H2_ALLOC(
            strlen(cOverviewText[IDX(giOverviewType) * OVERVIEW_TITLE_COUNT + title]) + 1,
            overviewSetupSourceLine + SETUP_TITLE_LINE_OFFSET
        ));
        strcpy(titleCopy, cOverviewText[IDX(giOverviewType) * OVERVIEW_TITLE_COUNT + title]);
        textWidgetTitle[title] = new textWidget(
            titleLefts[IDX(giOverviewType)][OD_STEER(title)],
            OVERVIEW_TITLE_Y,
            titleWidth[IDX(giOverviewType)][OD_STEER(title)],
            OVERVIEW_TITLE_HEIGHT,
            titleCopy,
            const_cast<char*>("smalfont.fnt"),
            FONT_DRAW_DEFAULT,
            static_cast<i16>(title + OVERVIEW_COLUMN_TITLE_FIRST),
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        overWin->AddWidget(textWidgetTitle[title], -1);
    }
    SetupDynamicStuff(0, 1, 0);
    if (redrawFrom != 0) {
        overWin->DrawWindow(redrawFrom, HERO_TAB_WIDGET, ICON_ROW_LIMIT - 1);
    }
}

VA(0x00409e05, 0x84)
void game::SetupResources(void) {
    i32 resourceIdx;
    tag_message message;

    message.type = MESSAGE_WIDGET;
    for (resourceIdx = 0; resourceIdx < IDX(RES_COUNT); resourceIdx++) {
        message.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
        message.payload.widget.data.text = gText;
        sprintf(gText, "%d", gpCurPlayer->m_resources[resourceIdx]);
        message.payload.widget.id = resourceIdx + RESOURCE_FIRST_WIDGET;
        overWin->BroadcastMessage(message);
    }
}

VA(0x00409e89, 0x4c7)
void game::Overview(void) {
    DATA(0x004edcec) static i16 overviewDialogSourceLine = DIALOG_SOURCE_LINE_BASE;
    char mineCounts4[OVERVIEW_MINE_COUNT_CAPACITY];
    i32 lighthouseCount4;
    i32 mine4;
    tag_message message8;
    i32 row0;

    giOverviewReturnAction = OVERVIEW_RETURN_NONE;
    giOverviewReturnActionExtra = OVERVIEW_RETURN_ID_NONE;
    message8.type = MESSAGE_WIDGET;
    gpAdvManager->TrimLoopingSounds(OVERVIEW_LOOPING_SOUND_KEEP_COUNT);
    gpWindowManager->FadeScreen(FADE_OUT, OVERVIEW_FADE_STEPS, NULL);
    for (mine4 = 0; mine4 < OVERVIEW_TITLE_COUNT; mine4++) {
        textWidgetTitle[mine4] = NULL;
    }

    textWidgetDynamic = static_cast<textWidget**>(H2_ALLOC(
        DYNAMIC_ARRAY_BYTES,
        overviewDialogSourceLine + DIALOG_TEXT_ARRAY_LINE_OFFSET
    ));
    iconWidgetDynamic = static_cast<iconWidget**>(H2_ALLOC(
        DYNAMIC_ARRAY_BYTES,
        overviewDialogSourceLine + DIALOG_ICON_ARRAY_LINE_OFFSET
    ));
    for (row0 = 0; row0 < OVERVIEW_VISIBLE_ROWS; row0++) {
        for (mine4 = 0; mine4 < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; mine4++) {
            OVERVIEW_TEXT_WIDGET_ROWS[row0][mine4] = NULL;
            OVERVIEW_ICON_WIDGET_ROWS[row0][mine4] = NULL;
        }
    }

    iLastDynamicType = OVERVIEW_NONE;
    iLastDynamicTop = -1;
    giOverviewItems[IDX(OVERVIEW_HEROES)] = gpCurPlayer->m_heroCount;
    giOverviewItems[IDX(OVERVIEW_TOWNS)] = gpCurPlayer->m_townCount;
    gpMouseManager->SetPointer(
        const_cast<char*>("advmice.mse"),
        OVERVIEW_POINTER_FRAME,
        MOUSE_AUTO_CURSOR_TYPE
    );
    gpResourceManager->GetBackdrop(const_cast<char*>("overback.icn"), gpWindowManager->m_screen, 1);
    overWin = new heroWindow(0, 0, const_cast<char*>("overwind.bin"));
    if (overWin == NULL) {
        MemError();
    }
    SetWinText(overWin, OVERVIEW_WINDOW_TEXT_ID);
    OVScrollKnob = new iconWidget(
        SCROLL_KNOB_X,
        SCROLL_KNOB_Y,
        SCROLL_KNOB_WIDTH,
        SCROLL_KNOB_HEIGHT,
        const_cast<char*>("scrollcn.icn"),
        SCROLL_KNOB_FRAME,
        0,
        SCROLL_KNOB_WIDGET,
        WIDGET_KIND_ICON_DIRECT,
        OVERVIEW_ICON_FILL_COLOR
    );
    if (OVScrollKnob == NULL) {
        MemError();
    }
    overWin->AddWidget(OVScrollKnob, -1);

    SetupResources();
    memset(mineCounts4, 0, IDX(RES_COUNT));
    lighthouseCount4 = 0;
    for (mine4 = 0; mine4 < IDX(GAME_MINE_COUNT); mine4++) {
        if (m_mineOwners[mine4] == giCurPlayer) {
            if (m_mines[mine4].resourceType <= IDX(RES_GOLD)) {
                mineCounts4[m_mines[mine4].resourceType]++;
            } else if (m_mines[mine4].resourceType == MINE_TYPE_LIGHTHOUSE) {
                lighthouseCount4++;
            }
        }
    }
    for (mine4 = 0; mine4 < IDX(RES_COUNT); mine4++) {
        message8.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
        message8.payload.widget.data.text = gText;
        sprintf(gText, "%d", static_cast<i32>(mineCounts4[mine4]));
        message8.payload.widget.id = mine4 + MINE_FIRST_WIDGET;
        overWin->BroadcastMessage(message8);
    }
    for (mine4 = OVERVIEW_VISIBLE_ROWS; mine4 > 0; mine4--) {
        if (OD_STEER(lighthouseCount4) < mine4) {
            message8.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message8.payload.widget.id = mine4 + LIGHTHOUSE_WIDGET_ID_OFFSET;
            message8.payload.widget.data.value = WIDGET_FLAG_DRAW;
            overWin->BroadcastMessage(message8);
        }
    }

    SetupResources();
    message8.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
    message8.payload.widget.id = DAILY_GOLD_WIDGET;
    message8.payload.widget.data.text = gText;
    sprintf(gText, "%d", ComputeDailyGold(giCurPlayer));
    overWin->BroadcastMessage(message8);
    SetupNewOverviewType(giOverviewType, 0);
    gpWindowManager->DoDialog(overWin, OverviewHandler, 1);
    gpWindowManager->FadeScreen(FADE_OUT, OVERVIEW_FADE_STEPS, NULL);
    delete overWin;
    overWin = NULL;
    H2_FREE(
        textWidgetDynamic,
        overviewDialogSourceLine + DIALOG_TEXT_ARRAY_FREE_OFFSET
    );
    H2_FREE(
        iconWidgetDynamic,
        overviewDialogSourceLine + DIALOG_ICON_ARRAY_FREE_OFFSET
    );
    textWidgetDynamic = NULL;
    iconWidgetDynamic = NULL;
}

VA(0x0040a350, 0x31f)
void game::DoKnob(void) {
    tag_message pendingMessage4;
    float scrollRange13;
    i32 mouseY8;
    i32 mouseX9;
    float scrollTop11;
    double pixelsPerItem13;
    i32 newTop8;
    tag_message message9;
    i32 previousTop6;

    if (giOverviewItems[IDX(giOverviewType)] <= OVERVIEW_VISIBLE_ROWS) {
        return;
    }

    {
        scrollRange13 = static_cast<float>(SCROLL_RANGE);
        scrollTop11 = static_cast<float>(SCROLL_MIN_Y);
        previousTop6 = giOverviewTop[IDX(giOverviewType)];
        pixelsPerItem13 =
            scrollRange13 / (giOverviewItems[IDX(giOverviewType)] - (OVERVIEW_VISIBLE_ROWS - 1));
        gpMouseManager->MouseCoords(mouseX9, mouseY8);
        gpInputManager->Flush();
        message9 = gpInputManager->GetEvent();
        while (message9.type != MESSAGE_LEFT_BUTTON_UP
               && message9.type != MESSAGE_RIGHT_BUTTON_UP) {
            if (message9.type == MESSAGE_MOUSE_MOVE) {
                if (static_cast<float>(message9.payload.mouse.y) < scrollTop11) {
                    message9.payload.mouse.y = static_cast<i32>(scrollTop11);
                }
                if (scrollRange13 + scrollTop11 - IDX(SCROLL_LAST_PIXEL_ADJUST)
                    < static_cast<float>(message9.payload.mouse.y)) {
                    message9.payload.mouse.y = static_cast<i32>(
                        scrollRange13 + scrollTop11 - IDX(SCROLL_LAST_PIXEL_ADJUST)
                    );
                }
                gpMouseManager->Main(message9);
                OVScrollKnob->m_y =
                    static_cast<i16>(message9.payload.mouse.y - SCROLL_KNOB_OFFSET);
                newTop8 = static_cast<i32>((OVScrollKnob->m_y - scrollTop11) / pixelsPerItem13);
                if (OD_STEER(previousTop6) != newTop8) {
                    if (newTop8 > giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS) {
                        newTop8 = giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
                    }
                    if (newTop8 < 0) {
                        newTop8 = 0;
                    }
                    giOverviewTop[IDX(giOverviewType)] = newTop8;
                    OVScrollKnob->m_y =
                        static_cast<i16>(message9.payload.mouse.y - SCROLL_KNOB_OFFSET);
                    SetupDynamicStuff(1, 0, 0);
                    previousTop6 = newTop8;
                } else {
                    overWin->DrawWindow(1, 0, IDX(WINDOW_DRAW_ID_LIMIT));
                }
            }
            Process1WindowsMessage();
            message9 = gpInputManager->GetEvent();
            if (message9.type == MESSAGE_MOUSE_MOVE) {
                i32 discardMouseMoves7 = 1;
                while (discardMouseMoves7) {
                    pendingMessage4 = gpInputManager->PeekEvent();
                    if (pendingMessage4.type == MESSAGE_MOUSE_MOVE) {
                        message9 = gpInputManager->GetEvent();
                    } else {
                        discardMouseMoves7 = 0;
                    }
                }
            }
        }
        OVScrollKnob->m_flags &= ~WIDGET_FLAG_SELECTED;
        SetupDynamicStuff(0, 1, 0);
        overWin->DrawWindow(
            WINDOW_FLAG_NONE,
            OVERVIEW_DYNAMIC_DRAW_FIRST,
            OVERVIEW_SCROLL_DRAW_LAST
        );
        gpWindowManager->UpdateScreenRegion(
            SCROLL_UPDATE_X,
            SCROLL_UPDATE_Y,
            SCROLL_UPDATE_WIDTH,
            SCROLL_UPDATE_HEIGHT
        );
    }
}

VA(0x0040a66f, 0x4fd)
i32 OverviewHandler(struct tag_message& message) {
    i32 closeDialog5;
    i32 quickView15;
    i32 scrollItemCount0;
    i32 scrollDivisor3;
    i32 scrollY7;
    i32 newTop8;

    closeDialog5 = 0;
    quickView15 = 0;
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                quickView15 = 1;
            case WIDGET_COMMAND_SELECT:
                if (message.payload.keyboard.modifiers & MESSAGE_MODIFIER_RIGHT_BUTTON) {
                    quickView15 = 1;
                }
                switch (message.payload.widget.id) {
                    case SCROLL_KNOB_WIDGET:
                        if (quickView15 != 0) {
                            break;
                        }
                        gpGame->DoKnob();
                        break;
                    case SCROLL_TRACK_WIDGET:
                        if (quickView15 != 0) {
                            break;
                        }
                        if (giOverviewItems[IDX(giOverviewType)] <= OVERVIEW_VISIBLE_ROWS) {
                            break;
                        }
                        scrollItemCount0 =
                            giOverviewItems[IDX(giOverviewType)] - (OVERVIEW_VISIBLE_ROWS - 1);
                        scrollDivisor3 = SCROLL_SCALED_TRACK_RANGE / scrollItemCount0;
                        scrollY7 = message.payload.mouse.screenY;
                        scrollY7 -= SCROLL_MIN_Y;
                        scrollY7 -= SCROLL_KNOB_OFFSET;
                        newTop8 = scrollY7 * SCROLL_SCALE / scrollDivisor3;
                        giOverviewTop[IDX(giOverviewType)] = newTop8;
                        if (giOverviewTop[IDX(giOverviewType)] + (OVERVIEW_VISIBLE_ROWS - 1)
                            >= giOverviewItems[IDX(giOverviewType)]) {
                            giOverviewTop[IDX(giOverviewType)] =
                                giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
                        }
                        if (giOverviewTop[IDX(giOverviewType)] < 0) {
                            giOverviewTop[IDX(giOverviewType)] = 0;
                        }
                        gpGame->SetupDynamicStuff(1, 1, 0);
                        break;
                    case HERO_TAB_WIDGET:
                        if (quickView15 != 0) {
                            break;
                        }
                    selectHeroes:
                        if (giOverviewType != OVERVIEW_HEROES) {
                            gpGame->SetupNewOverviewType(OVERVIEW_HEROES, 1);
                        }
                        break;
                    case TOWN_TAB_WIDGET:
                        if (quickView15 != 0) {
                            break;
                        }
                    selectTowns:
                        if (giOverviewType != OVERVIEW_TOWNS) {
                            gpGame->SetupNewOverviewType(OVERVIEW_TOWNS, 1);
                        }
                        break;
                    default:
                        closeDialog5 =
                            gpGame->ProcessIconSelect(message.payload.widget.id, quickView15);
                        break;
                }
                break;
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case HERO_TITLE_WIDGET:
                        goto selectHeroes;
                    case TOWN_TITLE_WIDGET:
                        goto selectTowns;
                    case SCROLL_UP_WIDGET:
                        if (giOverviewTop[IDX(giOverviewType)] > 0) {
                            giOverviewTop[IDX(giOverviewType)]--;
                            gpGame->SetupDynamicStuff(1, 1, 0);
                        }
                        break;
                    case SCROLL_DOWN_WIDGET:
                        if (giOverviewTop[IDX(giOverviewType)]
                            < giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS) {
                            giOverviewTop[IDX(giOverviewType)]++;
                            gpGame->SetupDynamicStuff(1, 1, 0);
                        }
                        break;
                    case CLOSE_WIDGET:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        closeDialog5 = 1;
                        break;
                }
                break;
        }
    }

    if (message.type == MESSAGE_KEY_DOWN) {
        switch (message.payload.keyboard.keyCode) {
            case INPUT_SCAN_NUMPAD_9:
                giOverviewTop[IDX(giOverviewType)] -= OVERVIEW_VISIBLE_ROWS;
                if (giOverviewTop[IDX(giOverviewType)] < 0) {
                    giOverviewTop[IDX(giOverviewType)] = 0;
                }
                gpGame->SetupDynamicStuff(1, 1, 0);
                break;
            case INPUT_SCAN_NUMPAD_3:
                giOverviewTop[IDX(giOverviewType)] += OVERVIEW_VISIBLE_ROWS;
                if (giOverviewTop[IDX(giOverviewType)]
                    > giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS) {
                    giOverviewTop[IDX(giOverviewType)] =
                        giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
                }
                gpGame->SetupDynamicStuff(1, 1, 0);
                break;
            case INPUT_SCAN_NUMPAD_7:
                giOverviewTop[IDX(giOverviewType)] = 0;
                gpGame->SetupDynamicStuff(1, 1, 0);
                break;
            case INPUT_SCAN_NUMPAD_1:
                giOverviewTop[IDX(giOverviewType)] =
                    giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
                gpGame->SetupDynamicStuff(1, 1, 0);
                break;
        }
    }
    if (closeDialog5 == 1) {
        message.payload.widget.id = SCROLL_UP_WIDGET;
        message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
        return OVERVIEW_HANDLER_CLOSE;
    }
    return OVERVIEW_HANDLER_CONTINUE;
}

VA(0x0040ab6c, 0x4fa)
i32 game::ProcessIconSelect(i32 widgetId, i32 quickView) {
    i32 row0;
    i32 selectionIndex2;
    hero* selectedHero13;
    town* selectedTown3;

    if (widgetId >= ICON_ROW_BASE && widgetId <= ICON_ROW_LIMIT - 1) {
        row0 = (widgetId - ICON_ROW_BASE) / OVERVIEW_ROW_ID_STRIDE;
        if (giOverviewTop[IDX(giOverviewType)] + row0 > giOverviewItems[IDX(giOverviewType)]) {
            return 0;
        }
        widgetId -= ICON_ROW_BASE;
        widgetId %= OVERVIEW_ROW_ID_STRIDE;

        if (giOverviewType == OVERVIEW_HEROES) {
            selectedHero13 =
                GetHero(gpCurPlayer->m_heroIds[giOverviewTop[IDX(giOverviewType)] + row0]);
            if (widgetId >= HERO_SELECT_FIRST && widgetId <= HERO_SELECT_LAST) {
                giOverviewReturnAction = OVERVIEW_RETURN_HERO;
                giOverviewReturnActionExtra =
                    gpCurPlayer->m_heroIds[giOverviewTop[IDX(giOverviewType)] + row0];
                return 1;
            }
            if (widgetId >= HERO_ARMY_ALT_FIRST
                && widgetId <= HERO_ARMY_ALT_LAST) {
                widgetId -= OVERVIEW_TROOP_SLOTS;
            }
            if (widgetId >= HERO_ARMY_FIRST && widgetId <= HERO_ARMY_LAST) {
                selectionIndex2 = widgetId - HERO_ARMY_FIRST;
                gpGame->ViewArmy(
                    OVERVIEW_VIEW_ARMY_X,
                    OVERVIEW_VIEW_ARMY_Y,
                    selectedHero13->m_army.m_creatureTypes[selectionIndex2],
                    selectedHero13->m_army.m_creatureCounts[selectionIndex2],
                    NULL,
                    selectedHero13->m_army.GetNumArmies() == 1,
                    ARMY_FACING_RIGHT,
                    quickView,
                    selectedHero13,
                    NULL,
                    &selectedHero13->m_army,
                    selectionIndex2
                );
                if (quickView == 0) {
                    SetupDynamicStuff(1, 1, 1);
                }
            }
            if (widgetId >= HERO_SKILL_FIRST && widgetId <= HERO_SKILL_LAST) {
                selectionIndex2 = selectedHero13->GetNthSS(widgetId - HERO_SKILL_FIRST);
                selectedHero13->DoSSLevelDialog(selectionIndex2, quickView);
            }
            if (widgetId >= HERO_ARTIFACT_FIRST
                && widgetId <= HERO_ARTIFACT_LAST) {
                selectionIndex2 =
                    selectedHero13->m_artifacts[widgetId - HERO_ARTIFACT_FIRST];
                if (selectionIndex2 == IDX(ARTIFACT_MAGIC_BOOK)) {
                    gpGame->ViewSpells(selectedHero13, SPELL_TYPE_ALL, ViewSpecialHandler, 1);
                } else {
                    NormalDialog(
                        gArtifactDesc[selectionIndex2],
                        quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_MAX_TOP,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_VALUE,
                        0
                    );
                }
            }
            if (widgetId >= HERO_STAT_FIRST && widgetId <= HERO_STAT_LAST) {
                selectionIndex2 = widgetId - HERO_STAT_FIRST;
                NormalDialog(
                    gStatDesc[selectionIndex2],
                    quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
                    NORMAL_DIALOG_NO_VALUE,
                    NORMAL_DIALOG_NO_VALUE,
                    NORMAL_DIALOG_NO_RESOURCE,
                    0,
                    NORMAL_DIALOG_NO_RESOURCE,
                    0,
                    NORMAL_DIALOG_NO_VALUE,
                    0
                );
            }
        }
        if (giOverviewType == OVERVIEW_TOWNS) {
            selectedTown3 =
                GetTown(gpCurPlayer->m_townIds[giOverviewTop[IDX(giOverviewType)] + row0]);
            if (widgetId == TOWN_SELECT_WIDGET) {
                giOverviewReturnAction = OVERVIEW_RETURN_TOWN;
                giOverviewReturnActionExtra =
                    gpCurPlayer->m_townIds[giOverviewTop[IDX(giOverviewType)] + row0];
                return 1;
            }
            if (widgetId >= TOWN_HERO_FIRST && widgetId <= TOWN_HERO_LAST
                && selectedTown3->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
                giOverviewReturnAction = OVERVIEW_RETURN_HERO;
                giOverviewReturnActionExtra = selectedTown3->m_occupyingHeroId;
                return 1;
            }
            if (widgetId >= TOWN_ARMY_ALT_FIRST
                && widgetId <= TOWN_ARMY_ALT_LAST) {
                widgetId -= OVERVIEW_TROOP_SLOTS;
            }
            if (widgetId >= TOWN_ARMY_FIRST && widgetId <= TOWN_ARMY_LAST) {
                selectionIndex2 = widgetId - TOWN_ARMY_FIRST;
                gpGame->ViewArmy(
                    OVERVIEW_VIEW_ARMY_X,
                    OVERVIEW_VIEW_ARMY_Y,
                    selectedTown3->m_army.m_creatureTypes[selectionIndex2],
                    selectedTown3->m_army.m_creatureCounts[selectionIndex2],
                    selectedTown3,
                    0,
                    ARMY_FACING_RIGHT,
                    quickView,
                    NULL,
                    NULL,
                    &selectedTown3->m_army,
                    selectionIndex2
                );
                if (quickView == 0) {
                    SetupDynamicStuff(1, 1, 1);
                }
            }
            if (widgetId >= TOWN_RECRUIT_ALT_FIRST
                && widgetId <= TOWN_RECRUIT_ALT_LAST) {
                widgetId -= OVERVIEW_DWELLING_SLOTS;
            }
            if (widgetId >= TOWN_RECRUIT_FIRST && widgetId <= TOWN_RECRUIT_LAST) {
                selectionIndex2 = widgetId - TOWN_RECRUIT_FIRST;
                if (quickView != 0) {
                    QuickViewRecruit(selectedTown3, selectionIndex2);
                } else {
                    recruitUnit* recruitDialog = new recruitUnit(selectedTown3, selectionIndex2, 0);
                    if (recruitDialog == NULL) {
                        MemError();
                    }
                    gpExec->DoDialog(recruitDialog);
                    delete recruitDialog;
                    gpResourceManager->GetBackdrop(
                        const_cast<char*>("overback.icn"),
                        gpWindowManager->m_screen,
                        1
                    );
                    SetupResources();
                    SetupDynamicStuff(0, 1, 1);
                    overWin->DrawWindow();
                }
            }
        }
    }
    return 0;
}

DATA(0x004ed9e8) class heroWindow* overWin = NULL;
DATA(0x004ed9ec) class textWidget** textWidgetDynamic = NULL;
DATA(0x004ed9f0) class iconWidget** iconWidgetDynamic = NULL;
DATA(0x004ed9f4) OverviewType giOverviewType = OVERVIEW_HEROES;
DATA(0x004ed9f8) i32 giOverviewTop[IDX(OVERVIEW_TYPE_COUNT)] = {0};
DATA(0x004eda00) class iconWidget* OVScrollKnob = NULL;
DATA(0x00523258) OverviewType iLastDynamicType;
DATA(0x0052325c) i32 iLastDynamicTop;
DATA(0x00523260) i32 iOverviewItems;
DATA(0x00523268) i32 giOverviewItems[IDX(OVERVIEW_TYPE_COUNT)];
DATA(0x00523270) class textWidget* textWidgetTitle[OVERVIEW_VISIBLE_ROWS];

#undef OVERVIEW_SCROLL_ROUNDING_OFFSET
#undef OVERVIEW_SCROLL_MIN_Y_FLOAT
#undef OVERVIEW_SCROLL_KNOB_RANGE_FLOAT
#undef RETAIL_FILE
