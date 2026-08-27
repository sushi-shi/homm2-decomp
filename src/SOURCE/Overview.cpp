#include <Ints.h>
#include <BASE/BITS.h>
#include <BASE/Misc.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <IRONFIST/heroes.h>
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
#include <PLATFORM/Runtime.h>
#include <SOURCE/Overview.h>
#include <BASE/widgetKind.h>
#include <SOURCE/playerData.h>
#include <SOURCE/recruitUnit.h>
#include <SOURCE/army.h>
#include <SOURCE/town.h>
#include <stdio.h>
#include <string.h>


namespace {

typedef enum OverviewStorageConstant {
    DYNAMIC_ARRAY_BYTES            = 0x2bc0,
    OVERVIEW_TITLE_COUNT           = 3,
    OVERVIEW_MINE_COUNT_CAPACITY   = H2EnumIndex(RES_COUNT) + 1,
    OVERVIEW_TROOP_TEXT_CAPACITY   = 6,
    OVERVIEW_PRIMARY_TEXT_CAPACITY = 4,
    OVERVIEW_SKILL_LEVEL_CAPACITY  = 2
} OverviewStorageConstant;

typedef enum OverviewPresentationConstant {
    OVERVIEW_ICON_FILL_COLOR          = 1,
    OVERVIEW_FADE_STEPS               = 8,
    OVERVIEW_LOOPING_SOUND_KEEP_COUNT = 4,
    OVERVIEW_WINDOW_TEXT_ID           = 9,
    OVERVIEW_POINTER_FRAME            = 0
} OverviewPresentationConstant;

typedef enum OverviewControlId {
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
} OverviewControlId;

typedef enum OverviewFrameConstant {
    OVERVIEW_TITLE_FRAME_BASE = 6,
    TOWN_TAB_SELECTED_FRAME   = 3,
    TOWN_TAB_UNSELECTED_FRAME = 2
} OverviewFrameConstant;

typedef enum OverviewScrollConstant {
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
} OverviewScrollConstant;

typedef enum OverviewRowLayout {
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
} OverviewRowLayout;

typedef enum OverviewTownRowLayout {
    TOWN_NAME_X                     = 35,
    TOWN_NAME_Y_OFFSET              = 80,
    TOWN_NAME_WIDTH                 = 132,
    TOWN_NAME_HEIGHT                = 10,
    TOWN_ICON_X                     = 47,
    TOWN_ICON_Y_OFFSET              = 36,
    TOWN_ICON_WIDTH                 = 46,
    TOWN_ICON_HEIGHT                = 22,
    TOWN_UNFORTIFIED_FRAME_OFFSET   = H2EnumIndex(FACTION_COUNT),
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
    TOWN_DWELLING_BUILDING_BIT_BASE = H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST)
} OverviewTownRowLayout;

typedef enum OverviewHeroRowLayout {
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
} OverviewHeroRowLayout;

typedef enum OverviewDwellingOrder {
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
} OverviewDwellingOrder;

typedef enum OverviewDwellingIndex {
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
} OverviewDwellingIndex;

typedef enum OverviewDialogConstant {
    OVERVIEW_RETURN_ID_NONE = -1,
    OVERVIEW_VIEW_ARMY_X    = 119,
    OVERVIEW_VIEW_ARMY_Y    = 20
} OverviewDialogConstant;

}

#define OVERVIEW_SCROLL_KNOB_RANGE_FLOAT 303.0
#define OVERVIEW_SCROLL_MIN_Y_FLOAT 18.0f
#define OVERVIEW_SCROLL_ROUNDING_OFFSET                                            \
    0.4

#define OVERVIEW_TEXT_WIDGET(row, item)                                                            \
    (*(textWidgetDynamic + (row) * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + (item)))
#define OVERVIEW_ICON_WIDGET(row, item)                                                            \
    (*(iconWidgetDynamic + (row) * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + (item)))

void game::SetupDynamicStuff(i32 redraw, i32 updateKnob, i32 forceUpdate) {
    i32 rowIndex;
    i32 i;
    i32 texts;
    i32 icons;
    i32 rowWidgetId;
    char* valueText;

    if (updateKnob) {
        if (giOverviewItems[H2EnumIndex(giOverviewType)] <= OVERVIEW_VISIBLE_ROWS) {
            OVScrollKnob->m_y = SCROLL_KNOB_STATIC_Y;
        } else {

            i32 steps = giOverviewItems[H2EnumIndex(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
            float pixelsPerItem = OVERVIEW_SCROLL_KNOB_RANGE_FLOAT / steps;
            OVScrollKnob->m_y = static_cast<i16>(
                giOverviewTop[H2EnumIndex(giOverviewType)] * pixelsPerItem + OVERVIEW_SCROLL_MIN_Y_FLOAT
                + OVERVIEW_SCROLL_ROUNDING_OFFSET
            );
        }
    }

    if (!forceUpdate && giOverviewType == iLastDynamicType
        && giOverviewTop[H2EnumIndex(iLastDynamicType)] == iLastDynamicTop) {
        return;
    }

    for (rowIndex = 0; rowIndex < OVERVIEW_VISIBLE_ROWS; rowIndex++) {
        for (i = 0; i < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; i++) {
            if (*(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) != NULL) {
                overWin->RemoveWidget(
                    *(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i)
                );
                delete *(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i);
                *(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) = NULL;
            }
            if (*(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) != NULL) {
                overWin->RemoveWidget(
                    *(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i)
                );
                delete *(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i);
                *(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) = NULL;
            }
        }
    }

    for (rowIndex = 0; rowIndex < OVERVIEW_VISIBLE_ROWS; rowIndex++) {
        texts = 0;
        icons = texts;
        rowWidgetId = rowIndex * OVERVIEW_ROW_ID_STRIDE + ICON_ROW_BASE;

        if (giOverviewTop[H2EnumIndex(giOverviewType)] + rowIndex >= giOverviewItems[H2EnumIndex(giOverviewType)]) {
            break;
        }

        OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
            ROW_BACKGROUND_X,
            static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + ROW_BACKGROUND_Y_OFFSET),
            ROW_BACKGROUND_WIDTH,
            ROW_BACKGROUND_HEIGHT,
            "overview.icn",
            static_cast<i16>(H2EnumIndex(giOverviewType) + ROW_BACKGROUND_FRAME_BASE),
            ICON_DRAW_NORMAL,
            static_cast<i16>(rowWidgetId + ROW_BACKGROUND_CONTROL),
            WIDGET_KIND_ICON_DIRECT,
            OVERVIEW_ICON_FILL_COLOR
        );
        if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
            MemError();
        }
        overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
        icons++;

        if (giOverviewType == OVERVIEW_TOWNS) {
            OverviewDwellingIndex building;
            i32 displayedTroops;
            hero* heroData;
            town* record =
                GetTown(gpCurPlayer->m_townIds[giOverviewTop[H2EnumIndex(giOverviewType)] + rowIndex]);
            i32 townFrame;
            b32 capt;
            i32 captainMana;
            {
                valueText = static_cast<char*>(H2_ALLOC(strlen(record->m_name) + 1));
                strcpy(valueText, record->m_name);

                OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
                    TOWN_NAME_X,
                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_NAME_Y_OFFSET),
                    TOWN_NAME_WIDTH,
                    TOWN_NAME_HEIGHT,
                    valueText,
                    "smalfont.fnt",
                    FONT_DRAW_DEFAULT,
                    static_cast<i16>(rowWidgetId + TOWN_NAME_CONTROL),
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
                overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
                texts++;
            }

            {
                townFrame = H2EnumIndex(record->m_type);
                if ((record->m_buildings & H2EnumIndex(TOWN_BUILDING_CASTLE)) == 0) {
                    townFrame += TOWN_UNFORTIFIED_FRAME_OFFSET;
                }
                OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                    TOWN_ICON_X,
                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_ICON_Y_OFFSET),
                    TOWN_ICON_WIDTH,
                    TOWN_ICON_HEIGHT,
                    "minitown.icn",
                    townFrame,
                    ICON_DRAW_NORMAL,
                    static_cast<i16>(rowWidgetId + TOWN_SELECT_WIDGET),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                icons++;
            }

            if (H2BitTest(gpGame->m_knownTowns, static_cast<u32>(record->m_id))) {
                OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                    TOWN_LOCATOR_X,
                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_LOCATOR_Y_OFFSET),
                    0,
                    0,
                    "locators.icn",
                    TOWN_LOCATOR_FRAME,
                    ICON_DRAW_NORMAL,
                    static_cast<i16>(rowWidgetId + TOWN_LOCATOR_CONTROL),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                icons++;
            }

            capt = false;
            heroData = NULL;
            if (record->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
                heroData = GetHero(record->m_occupyingHeroId);
            } else {
                if ((record->m_buildings & H2EnumIndex(TOWN_BUILDING_CAPTAIN_QUARTERS)) != 0) {
                    capt = true;
                }
            }

            if (heroData != NULL || capt) {
                OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                    TOWN_HERO_FRAME_X,
                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_HERO_FRAME_Y_OFFSET),
                    TOWN_HERO_FRAME_WIDTH,
                    TOWN_HERO_FRAME_HEIGHT,
                    "portxtra.icn",
                    0,
                    ICON_DRAW_NORMAL,
                    static_cast<i16>(rowWidgetId + TOWN_HERO_FIRST),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                icons++;

                OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                    TOWN_HERO_PORTRAIT_X,
                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_HERO_PORTRAIT_Y_OFFSET),
                    TOWN_HERO_PORTRAIT_WIDTH,
                    TOWN_HERO_PORTRAIT_HEIGHT,
                    capt ? "minicapt.icn" : "miniport.icn",
                    capt ? static_cast<u8>(H2EnumIndex(record->m_type)) : H2EnumIndex(heroData->m_portrait),
                    ICON_DRAW_NORMAL,
                    static_cast<i16>(rowWidgetId + TOWN_HERO_PORTRAIT_CONTROL),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                icons++;

                OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                    TOWN_MOBILITY_X,
                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_MOBILITY_Y_OFFSET),
                    0,
                    0,
                    "mobility.icn",
                    static_cast<i16>(GetMobilityFrame(capt ? 0 : heroData->m_remainingMobility)),
                    ICON_DRAW_NORMAL,
                    static_cast<i16>(rowWidgetId + TOWN_HERO_MOBILITY_CONTROL),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                icons++;

                if (record->m_type == FACTION_BARBARIAN || record->m_type == FACTION_KNIGHT) {
                    captainMana = CAPTAIN_MANA_LOW;
                } else {
                    captainMana = CAPTAIN_MANA_HIGH;
                }
                OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                    TOWN_MANA_X,
                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_MANA_Y_OFFSET),
                    0,
                    0,
                    "mana.icn",
                    static_cast<i16>(GetManaFrame(capt ? captainMana : heroData->m_spellPoints)),
                    ICON_DRAW_NORMAL,
                    static_cast<i16>(rowWidgetId + TOWN_HERO_MANA_CONTROL),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                icons++;
            }

            {
                displayedTroops = 0;
                for (i = 0; i < OVERVIEW_TROOP_SLOTS; i++) {
                    if (record->m_army.m_creatureTypes[i] != CREATURE_NONE
                        && record->m_army.m_creatureCounts[i] > 0) {
                        OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                            static_cast<i16>(
                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_TROOP_FIRST_X
                            ),
                            static_cast<i16>(
                                rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_ICON_Y_OFFSET
                            ),
                            TROOP_ICON_WIDTH,
                            TROOP_ICON_HEIGHT,
                            "mons32.icn",
                            static_cast<i16>(record->m_army.m_creatureTypes[i]),
                            ICON_DRAW_NORMAL,
                            static_cast<i16>(rowWidgetId + i + TOWN_ARMY_FIRST),
                            WIDGET_KIND_ICON_CENTERED,
                            OVERVIEW_ICON_FILL_COLOR
                        );
                        if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                            MemError();
                        }
                        overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                        icons++;

                        valueText = static_cast<char*>(H2_ALLOC(OVERVIEW_TROOP_TEXT_CAPACITY));
                        sprintf(
                            valueText,
                            "%d",
                            static_cast<i32>(record->m_army.m_creatureCounts[i])
                        );
                        OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
                            static_cast<i16>(
                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_TROOP_FIRST_X
                            ),
                            static_cast<i16>(
                                rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_TEXT_Y_OFFSET
                            ),
                            TROOP_TEXT_WIDTH,
                            TROOP_TEXT_HEIGHT,
                            valueText,
                            "smalfont.fnt",
                            FONT_DRAW_DEFAULT,
                            static_cast<i16>(rowWidgetId + i + TOWN_ARMY_ALT_FIRST),
                            WIDGET_KIND_TEXT,
                            FONT_ALIGN_CENTER
                        );
                        overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
                        texts++;
                        displayedTroops++;
                    }
                }
            }

            {
                displayedTroops = 0;
                for (i = 0; i < OVERVIEW_DWELLING_SLOTS; i++) {
                    switch (i) {
                        case DWELLING_ORDER_LEVEL_1:
                            building = DWELLING_INDEX_LEVEL_1;
                            break;
                        case DWELLING_ORDER_LEVEL_2:
                            building = DWELLING_INDEX_LEVEL_2;
                            break;
                        case DWELLING_ORDER_UPGRADED_LEVEL_2:
                            building = DWELLING_INDEX_UPGRADED_LEVEL_2;
                            break;
                        case DWELLING_ORDER_LEVEL_3:
                            building = DWELLING_INDEX_LEVEL_3;
                            break;
                        case DWELLING_ORDER_UPGRADED_LEVEL_3:
                            building = DWELLING_INDEX_UPGRADED_LEVEL_3;
                            break;
                        case DWELLING_ORDER_LEVEL_4:
                            building = DWELLING_INDEX_LEVEL_4;
                            break;
                        case DWELLING_ORDER_UPGRADED_LEVEL_4:
                            building = DWELLING_INDEX_UPGRADED_LEVEL_4;
                            break;
                        case DWELLING_ORDER_LEVEL_5:
                            building = DWELLING_INDEX_LEVEL_5;
                            break;
                        case DWELLING_ORDER_UPGRADED_LEVEL_5:
                            building = DWELLING_INDEX_UPGRADED_LEVEL_5;
                            break;
                        case DWELLING_ORDER_LEVEL_6:
                            building = DWELLING_INDEX_LEVEL_6;
                            break;
                        case DWELLING_ORDER_UPGRADED_LEVEL_6:
                            building = DWELLING_INDEX_UPGRADED_LEVEL_6;
                            break;
                        default:
                            building = DWELLING_INDEX_ALTERNATE_LEVEL_6;
                            break;
                    }

                    if ((record->m_buildings
                         & (1 << (H2EnumIndex(building)
                                  + H2EnumIndex(TOWN_DWELLING_BUILDING_BIT_BASE))))
                        != 0) {
                        OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                            static_cast<i16>(
                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_DWELLING_FIRST_X
                            ),
                            static_cast<i16>(
                                rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_ICON_Y_OFFSET
                            ),
                            TROOP_ICON_WIDTH,
                            TROOP_ICON_HEIGHT,
                            "mons32.icn",
                            static_cast<i16>(gDwellingType[H2EnumIndex(record->m_type)][building]),
                            ICON_DRAW_NORMAL,
                            static_cast<i16>(rowWidgetId + building + TOWN_RECRUIT_FIRST),
                            WIDGET_KIND_ICON_CENTERED,
                            OVERVIEW_ICON_FILL_COLOR
                        );
                        if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                            MemError();
                        }
                        overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                        icons++;

                        valueText = static_cast<char*>(H2_ALLOC(OVERVIEW_TROOP_TEXT_CAPACITY));
                        sprintf(valueText, "%d", static_cast<i32>(record->m_garrison[building]));
                        OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
                            static_cast<i16>(
                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_DWELLING_FIRST_X
                            ),
                            static_cast<i16>(
                                rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_TEXT_Y_OFFSET
                            ),
                            TROOP_TEXT_WIDTH,
                            TROOP_TEXT_HEIGHT,
                            valueText,
                            "smalfont.fnt",
                            FONT_DRAW_DEFAULT,
                            static_cast<i16>(rowWidgetId + building + TOWN_RECRUIT_ALT_FIRST),
                            WIDGET_KIND_TEXT,
                            FONT_ALIGN_CENTER
                        );
                        overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
                        texts++;
                        displayedTroops++;
                    }
                }
            }
        } else {
            i32 shown;
            hero* curHero;
            i32 column;
            HeroSecondarySkill skillIndex;
            i32 detailRow;
            curHero =
                GetHero(gpCurPlayer->m_heroIds[giOverviewTop[H2EnumIndex(giOverviewType)] + rowIndex]);

            OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                HERO_FRAME_X,
                static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_FRAME_Y_OFFSET),
                HERO_FRAME_WIDTH,
                HERO_FRAME_HEIGHT,
                "portxtra.icn",
                0,
                ICON_DRAW_NORMAL,
                static_cast<i16>(rowWidgetId + HERO_SELECT_FIRST),
                WIDGET_KIND_ICON_DIRECT,
                OVERVIEW_ICON_FILL_COLOR
            );
            if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                MemError();
            }
            overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
            icons++;

            OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                HERO_PORTRAIT_X,
                static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_PORTRAIT_Y_OFFSET),
                HERO_PORTRAIT_WIDTH,
                HERO_PORTRAIT_HEIGHT,
                "miniport.icn",
                static_cast<i16>(curHero->m_portrait),
                ICON_DRAW_NORMAL,
                static_cast<i16>(rowWidgetId + HERO_PORTRAIT_CONTROL),
                WIDGET_KIND_ICON_DIRECT,
                OVERVIEW_ICON_FILL_COLOR
            );
            if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                MemError();
            }
            overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
            icons++;

            OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                HERO_MOBILITY_X,
                static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_MOBILITY_Y_OFFSET),
                0,
                0,
                "mobility.icn",
                static_cast<i16>(GetMobilityFrame(curHero->m_remainingMobility)),
                ICON_DRAW_NORMAL,
                static_cast<i16>(rowWidgetId + HERO_MOBILITY_CONTROL),
                WIDGET_KIND_ICON_DIRECT,
                OVERVIEW_ICON_FILL_COLOR
            );
            if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                MemError();
            }
            overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
            icons++;

            OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                HERO_MANA_X,
                static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_MANA_Y_OFFSET),
                0,
                0,
                "mana.icn",
                static_cast<i16>(GetManaFrame(curHero->m_spellPoints)),
                ICON_DRAW_NORMAL,
                static_cast<i16>(rowWidgetId + TOWN_HERO_MANA_CONTROL),
                WIDGET_KIND_ICON_DIRECT,
                OVERVIEW_ICON_FILL_COLOR
            );
            if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                MemError();
            }
            overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
            icons++;

            for (i = 0; i < HERO_PRIMARY_STAT_COUNT; i++) {
                valueText = static_cast<char*>(H2_ALLOC(OVERVIEW_PRIMARY_TEXT_CAPACITY));
                sprintf(valueText, "%d", static_cast<i32>(curHero->Stats(HeroPrimaryStat(i))));
                OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
                    static_cast<i16>(i * HERO_PRIMARY_COLUMN_STRIDE + HERO_PRIMARY_TEXT_FIRST_X),
                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_PRIMARY_TEXT_Y_OFFSET),
                    HERO_PRIMARY_TEXT_WIDTH,
                    HERO_PRIMARY_TEXT_HEIGHT,
                    valueText,
                    "smalfont.fnt",
                    FONT_DRAW_DEFAULT,
                    static_cast<i16>(rowWidgetId + i + HERO_PRIMARY_TEXT_FIRST),
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_RIGHT
                );
                overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
                texts++;
            }

            for (i = 0; i < HERO_PRIMARY_STAT_COUNT; i++) {
                OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                    static_cast<i16>(i * HERO_PRIMARY_COLUMN_STRIDE + HERO_PRIMARY_ICON_FIRST_X),
                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_PRIMARY_ICON_Y_OFFSET),
                    HERO_PRIMARY_ICON_WIDTH,
                    HERO_PRIMARY_ICON_HEIGHT,
                    "overview.icn",
                    HERO_PRIMARY_ICON_FRAME,
                    ICON_DRAW_NORMAL,
                    static_cast<i16>(rowWidgetId + i + HERO_STAT_FIRST),
                    WIDGET_KIND_ICON_DIRECT,
                    OVERVIEW_ICON_FILL_COLOR
                );
                if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                icons++;
            }

            {
                shown = 0;
                for (i = 0; i < OVERVIEW_TROOP_SLOTS; i++) {
                    if (curHero->m_army.m_creatureTypes[i] != CREATURE_NONE
                        && curHero->m_army.m_creatureCounts[i] > 0) {
                        OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                            static_cast<i16>(shown * HERO_TROOP_COLUMN_STRIDE + HERO_TROOP_FIRST_X),
                            static_cast<i16>(
                                rowIndex * OVERVIEW_ROW_HEIGHT + HERO_TROOP_ICON_Y_OFFSET
                            ),
                            TROOP_ICON_WIDTH,
                            TROOP_ICON_HEIGHT,
                            "mons32.icn",
                            static_cast<i16>(curHero->m_army.m_creatureTypes[i]),
                            ICON_DRAW_NORMAL,
                            static_cast<i16>(rowWidgetId + i + HERO_ARMY_FIRST),
                            WIDGET_KIND_ICON_CENTERED,
                            OVERVIEW_ICON_FILL_COLOR
                        );
                        if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                            MemError();
                        }
                        overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                        icons++;

                        valueText = static_cast<char*>(H2_ALLOC(OVERVIEW_TROOP_TEXT_CAPACITY));
                        sprintf(
                            valueText,
                            "%d",
                            static_cast<i32>(curHero->m_army.m_creatureCounts[i])
                        );
                        OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
                            static_cast<i16>(shown * HERO_TROOP_COLUMN_STRIDE + HERO_TROOP_FIRST_X),
                            static_cast<i16>(
                                rowIndex * OVERVIEW_ROW_HEIGHT + HERO_TROOP_TEXT_Y_OFFSET
                            ),
                            TROOP_TEXT_WIDTH,
                            TROOP_TEXT_HEIGHT,
                            valueText,
                            "smalfont.fnt",
                            FONT_DRAW_DEFAULT,
                            static_cast<i16>(rowWidgetId + i + HERO_ARMY_ALT_FIRST),
                            WIDGET_KIND_TEXT,
                            FONT_ALIGN_CENTER
                        );
                        overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
                        texts++;
                        shown++;
                    }
                }
            }

            for (i = 0; i < OVERVIEW_SECONDARY_SKILL_SLOTS; i++) {
                skillIndex = curHero->GetNthSS(i);
                if (skillIndex != HERO_SKILL_NONE) {
                    detailRow = i / OVERVIEW_SECONDARY_SKILL_COLUMNS;
                    column = i % OVERVIEW_SECONDARY_SKILL_COLUMNS;
                    OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                        static_cast<i16>(column * DETAIL_COLUMN_STRIDE + HERO_SKILL_FRAME_FIRST_X),
                        static_cast<i16>(
                            detailRow * DETAIL_ROW_STRIDE + rowIndex * OVERVIEW_ROW_HEIGHT
                            + HERO_SKILL_FRAME_Y_OFFSET
                        ),
                        DETAIL_FRAME_WIDTH,
                        DETAIL_FRAME_HEIGHT,
                        "overview.icn",
                        DETAIL_FRAME_ICON,
                        ICON_DRAW_NORMAL,
                        static_cast<i16>(rowWidgetId + i + HERO_SKILL_FRAME_FIRST),
                        WIDGET_KIND_ICON_DIRECT,
                        OVERVIEW_ICON_FILL_COLOR
                    );
                    if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                        MemError();
                    }
                    overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                    icons++;

                    OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                        static_cast<i16>(column * DETAIL_COLUMN_STRIDE + HERO_SKILL_ICON_FIRST_X),
                        static_cast<i16>(
                            detailRow * DETAIL_ROW_STRIDE + rowIndex * OVERVIEW_ROW_HEIGHT
                            + HERO_SKILL_ICON_Y_OFFSET
                        ),
                        DETAIL_ICON_WIDTH,
                        DETAIL_ICON_HEIGHT,
                        "miniss.icn",
                        // The Cyborg Wisdom slot draws the Cybernetics small icon.
                        static_cast<i16>(
                            curHero->m_cursorType == FACTION_CYBORG
                                    && skillIndex == HERO_SKILL_WISDOM
                                ? H2EnumIndex(ironfist::CYBERNETICS_MINI_SKILL_FRAME)
                                : H2EnumIndex(skillIndex)
                        ),
                        ICON_DRAW_NORMAL,
                        static_cast<i16>(rowWidgetId + i + HERO_SKILL_FIRST),
                        WIDGET_KIND_ICON_DIRECT,
                        OVERVIEW_ICON_FILL_COLOR
                    );
                    if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                        MemError();
                    }
                    overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                    icons++;

                    valueText = static_cast<char*>(H2_ALLOC(OVERVIEW_SKILL_LEVEL_CAPACITY));
                    sprintf(valueText, "%d", static_cast<i32>(curHero->GetSSLevel(skillIndex)));
                    OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
                        static_cast<i16>(column * DETAIL_COLUMN_STRIDE + HERO_SKILL_LEVEL_FIRST_X),
                        static_cast<i16>(
                            detailRow * DETAIL_ROW_STRIDE + rowIndex * OVERVIEW_ROW_HEIGHT
                            + HERO_SKILL_LEVEL_Y_OFFSET
                        ),
                        HERO_SKILL_LEVEL_WIDTH,
                        HERO_SKILL_LEVEL_HEIGHT,
                        valueText,
                        "smalfont.fnt",
                        FONT_DRAW_DEFAULT,
                        static_cast<i16>(rowWidgetId + i + HERO_SKILL_LEVEL_FIRST),
                        WIDGET_KIND_TEXT,
                        FONT_ALIGN_RIGHT
                    );
                    overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
                    texts++;
                }
            }

            i32 displayedArtifacts = 0;
            for (i = 0; i < OVERVIEW_ARTIFACT_SLOTS; i++) {
                if (curHero->m_artifacts[i] != ARTIFACT_NONE) {
                    detailRow = displayedArtifacts / OVERVIEW_ARTIFACT_COLUMNS;
                    column = displayedArtifacts % OVERVIEW_ARTIFACT_COLUMNS;
                    OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                        static_cast<i16>(
                            column * DETAIL_COLUMN_STRIDE + HERO_ARTIFACT_FRAME_FIRST_X
                        ),
                        static_cast<i16>(
                            detailRow * DETAIL_ROW_STRIDE + rowIndex * OVERVIEW_ROW_HEIGHT
                            + HERO_ARTIFACT_FRAME_Y_OFFSET
                        ),
                        DETAIL_FRAME_WIDTH,
                        DETAIL_FRAME_HEIGHT,
                        "overview.icn",
                        DETAIL_FRAME_ICON,
                        ICON_DRAW_NORMAL,
                        static_cast<i16>(rowWidgetId + i + HERO_ARTIFACT_FRAME_FIRST),
                        WIDGET_KIND_ICON_DIRECT,
                        OVERVIEW_ICON_FILL_COLOR
                    );
                    if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                        MemError();
                    }
                    overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                    icons++;

                    OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
                        static_cast<i16>(
                            column * DETAIL_COLUMN_STRIDE + HERO_ARTIFACT_ICON_FIRST_X
                        ),
                        static_cast<i16>(
                            detailRow * DETAIL_ROW_STRIDE + rowIndex * OVERVIEW_ROW_HEIGHT
                            + HERO_ARTIFACT_ICON_Y_OFFSET
                        ),
                        DETAIL_ICON_WIDTH,
                        DETAIL_ICON_HEIGHT,
                        "artfx.icn",
                        static_cast<i16>(curHero->m_artifacts[i]),
                        ICON_DRAW_NORMAL,
                        static_cast<i16>(rowWidgetId + i + HERO_ARTIFACT_FIRST),
                        WIDGET_KIND_ICON_DIRECT,
                        OVERVIEW_ICON_FILL_COLOR
                    );
                    if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
                        MemError();
                    }
                    overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
                    icons++;
                    displayedArtifacts++;
                }
            }
        }
    }

    if (redraw) {
        overWin->DrawWindow(
            false,
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

void game::SetupNewOverviewType(OverviewType overviewType, i32 redrawFrom) {
    i32 col;
    tag_message message;
    char* titleText;

    giOverviewType = overviewType;
    iOverviewItems =
        giOverviewType == OVERVIEW_HEROES ? gpCurPlayer->m_heroCount : gpCurPlayer->m_townCount;

    if (giOverviewTop[H2EnumIndex(giOverviewType)] > iOverviewItems - OVERVIEW_VISIBLE_ROWS) {
        giOverviewTop[H2EnumIndex(giOverviewType)] = iOverviewItems - OVERVIEW_VISIBLE_ROWS;
    }
    if (giOverviewTop[H2EnumIndex(giOverviewType)] < 0) {
        giOverviewTop[H2EnumIndex(giOverviewType)] = 0;
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = OVERVIEW_WIDGET_SET_FRAME;
    message.payload.widget.id = TITLE_WIDGET;
    message.payload.widget.data.value = H2EnumIndex(giOverviewType) + OVERVIEW_TITLE_FRAME_BASE;
    overWin->BroadcastMessage(message);
    message.payload.widget.command = OVERVIEW_WIDGET_SET_FRAME;
    message.payload.widget.id = HERO_TAB_WIDGET;
    message.payload.widget.data.value = giOverviewType == OVERVIEW_HEROES;
    overWin->BroadcastMessage(message);
    message.payload.widget.command = OVERVIEW_WIDGET_SET_FRAME;
    message.payload.widget.id = TOWN_TAB_WIDGET;
    message.payload.widget.data.value =
        giOverviewType == OVERVIEW_TOWNS ? TOWN_TAB_SELECTED_FRAME : TOWN_TAB_UNSELECTED_FRAME;
    overWin->BroadcastMessage(message);

    i16 titleX[H2EnumIndex(OVERVIEW_TYPE_COUNT)][OVERVIEW_TITLE_COUNT] = {{35, 232, 377}, {35, 177, 379}};
    i16 titleWidths[H2EnumIndex(OVERVIEW_TYPE_COUNT)][OVERVIEW_TITLE_COUNT] = {
        {194, 142, 241},
        {140, 199, 239}
    };

    for (col = 0; col < OVERVIEW_TITLE_COUNT; col++) {
        if (textWidgetTitle[col] != NULL) {
            overWin->RemoveWidget(textWidgetTitle[col]);
            delete textWidgetTitle[col];
            textWidgetTitle[col] = NULL;
        }
    }
    for (col = 0; col < OVERVIEW_TITLE_COUNT; col++) {
        titleText = static_cast<char*>(
            H2_ALLOC(strlen(cOverviewText[col + H2EnumIndex(giOverviewType) * OVERVIEW_TITLE_COUNT]) + 1)
        );
        strcpy(titleText, cOverviewText[col + H2EnumIndex(giOverviewType) * OVERVIEW_TITLE_COUNT]);
        textWidgetTitle[col] = new textWidget(
            titleX[H2EnumIndex(giOverviewType)][col],
            OVERVIEW_TITLE_Y,
            titleWidths[H2EnumIndex(giOverviewType)][col],
            OVERVIEW_TITLE_HEIGHT,
            titleText,
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            static_cast<i16>(col + OVERVIEW_COLUMN_TITLE_FIRST),
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        overWin->AddWidget(textWidgetTitle[col], -1);
    }
    SetupDynamicStuff(0, 1, 0);
    if (redrawFrom != 0) {
        overWin->DrawWindow(redrawFrom, HERO_TAB_WIDGET, ICON_ROW_LIMIT - 1);
    }
}

void game::SetupResources(void) {
    H2SteppedEnumStorage<ResourceType, i32> resourceIdx;
    tag_message msg;

    msg.type = MESSAGE_WIDGET;
    for (resourceIdx = RES_WOOD; resourceIdx < RES_COUNT; resourceIdx++) {
        msg.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
        msg.payload.widget.data.text = gText;
        sprintf(gText, "%d", gpCurPlayer->m_resources[H2EnumIndex(resourceIdx)]);
        msg.payload.widget.id = H2EnumIndex(resourceIdx) + RESOURCE_FIRST_WIDGET;
        overWin->BroadcastMessage(msg);
    }
}

void game::Overview(void) {
    char mineCounts[OVERVIEW_MINE_COUNT_CAPACITY];
    i32 lighthouseCount;
    i32 mine;
    tag_message message;
    i32 y;

    giOverviewReturnAction = OVERVIEW_RETURN_NONE;
    giOverviewReturnActionExtra = OVERVIEW_RETURN_ID_NONE;
    message.type = MESSAGE_WIDGET;
    gpAdvManager->TrimLoopingSounds(OVERVIEW_LOOPING_SOUND_KEEP_COUNT);
    gpWindowManager->FadeScreen(FADE_OUT, OVERVIEW_FADE_STEPS, NULL);
    for (mine = 0; mine < OVERVIEW_TITLE_COUNT; mine++) {
        textWidgetTitle[mine] = NULL;
    }

    textWidgetDynamic = static_cast<textWidget**>(H2_ALLOC(DYNAMIC_ARRAY_BYTES));
    iconWidgetDynamic = static_cast<iconWidget**>(H2_ALLOC(DYNAMIC_ARRAY_BYTES));
    for (y = 0; y < OVERVIEW_VISIBLE_ROWS; y++) {
        for (mine = 0; mine < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; mine++) {
            *(textWidgetDynamic + y * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mine) = NULL;
            *(iconWidgetDynamic + y * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mine) = NULL;
        }
    }

    iLastDynamicType = OVERVIEW_NONE;
    iLastDynamicTop = -1;
    giOverviewItems[H2EnumIndex(OVERVIEW_HEROES)] = gpCurPlayer->m_heroCount;
    giOverviewItems[H2EnumIndex(OVERVIEW_TOWNS)] = gpCurPlayer->m_townCount;
    gpMouseManager->SetPointer(
        "advmice.mse",
        OVERVIEW_POINTER_FRAME,
        MOUSE_AUTO_CURSOR_TYPE
    );
    gpResourceManager->GetBackdrop("overback.icn", gpWindowManager->m_screen, 1);
    overWin = new heroWindow(0, 0, "overwind.bin");
    if (overWin == NULL) {
        MemError();
    }
    SetWinText(overWin, OVERVIEW_WINDOW_TEXT_ID);
    OVScrollKnob = new iconWidget(
        SCROLL_KNOB_X,
        SCROLL_KNOB_Y,
        SCROLL_KNOB_WIDTH,
        SCROLL_KNOB_HEIGHT,
        "scrollcn.icn",
        SCROLL_KNOB_FRAME,
        ICON_DRAW_NORMAL,
        SCROLL_KNOB_WIDGET,
        WIDGET_KIND_ICON_DIRECT,
        OVERVIEW_ICON_FILL_COLOR
    );
    if (OVScrollKnob == NULL) {
        MemError();
    }
    overWin->AddWidget(OVScrollKnob, -1);

    SetupResources();
    memset(mineCounts, 0, H2EnumIndex(RES_COUNT));
    lighthouseCount = 0;
    for (mine = 0; mine < GAME_MINE_COUNT; mine++) {
        if (m_mineOwners[mine] == giCurPlayer) {
            if (m_mines[mine].resourceType <= MINE_TYPE_GOLD) {
                mineCounts[H2EnumIndex(m_mines[mine].resourceType)]++;
            } else if (m_mines[mine].resourceType == MINE_TYPE_LIGHTHOUSE) {
                lighthouseCount++;
            }
        }
    }
    for (mine = 0; mine < H2EnumIndex(RES_COUNT); mine++) {
        message.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
        message.payload.widget.data.text = gText;
        sprintf(gText, "%d", static_cast<i32>(mineCounts[mine]));
        message.payload.widget.id = mine + MINE_FIRST_WIDGET;
        overWin->BroadcastMessage(message);
    }
    for (mine = OVERVIEW_VISIBLE_ROWS; mine > 0; mine--) {
        if (mine > lighthouseCount) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = mine + LIGHTHOUSE_WIDGET_ID_OFFSET;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            overWin->BroadcastMessage(message);
        }
    }

    SetupResources();
    message.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
    message.payload.widget.id = DAILY_GOLD_WIDGET;
    message.payload.widget.data.text = gText;
    sprintf(gText, "%d", ComputeDailyGold(giCurPlayer));
    overWin->BroadcastMessage(message);
    SetupNewOverviewType(giOverviewType, 0);
    gpWindowManager->DoDialog(overWin, OverviewHandler, 1);
    gpWindowManager->FadeScreen(FADE_OUT, OVERVIEW_FADE_STEPS, NULL);
    delete overWin;
    overWin = NULL;
    H2_FREE(textWidgetDynamic);
    H2_FREE(iconWidgetDynamic);
    textWidgetDynamic = NULL;
    iconWidgetDynamic = NULL;
}

void game::DoKnob(void) {
    i32 topBefore;
    double itemPixels;
    float scrollRange;
    tag_message widgetMessage;
    i32 ptX;
    i32 ptY;
    i32 topNow;
    float topValue;
    tag_message pendingMessage;

    if (giOverviewItems[H2EnumIndex(giOverviewType)] <= OVERVIEW_VISIBLE_ROWS) {
        return;
    }

    {
        scrollRange = static_cast<float>(SCROLL_RANGE);
        topValue = static_cast<float>(SCROLL_MIN_Y);
        topBefore = giOverviewTop[H2EnumIndex(giOverviewType)];
        itemPixels =
            scrollRange / (giOverviewItems[H2EnumIndex(giOverviewType)] - (OVERVIEW_VISIBLE_ROWS - 1));
        gpMouseManager->MouseCoords(ptX, ptY);
        gpInputManager->Flush();
        widgetMessage = gpInputManager->GetEvent();
        while (widgetMessage.type != MESSAGE_LEFT_BUTTON_UP
               && widgetMessage.type != MESSAGE_RIGHT_BUTTON_UP) {
            if (widgetMessage.type == MESSAGE_MOUSE_MOVE) {
                if (static_cast<float>(widgetMessage.payload.mouse.y) < topValue) {
                    widgetMessage.payload.mouse.y = static_cast<i32>(topValue);
                }
                if (static_cast<float>(widgetMessage.payload.mouse.y)
                    > topValue + scrollRange - H2EnumIndex(SCROLL_LAST_PIXEL_ADJUST)) {
                    widgetMessage.payload.mouse.y = static_cast<i32>(
                        topValue + scrollRange - H2EnumIndex(SCROLL_LAST_PIXEL_ADJUST)
                    );
                }
                gpMouseManager->Main(widgetMessage);
                OVScrollKnob->m_y =
                    static_cast<i16>(widgetMessage.payload.mouse.y - SCROLL_KNOB_OFFSET);
                topNow = static_cast<i32>((OVScrollKnob->m_y - topValue) / itemPixels);
                if (topNow != topBefore) {
                    if (topNow > giOverviewItems[H2EnumIndex(giOverviewType)] - OVERVIEW_VISIBLE_ROWS) {
                        topNow = giOverviewItems[H2EnumIndex(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
                    }
                    if (topNow < 0) {
                        topNow = 0;
                    }
                    giOverviewTop[H2EnumIndex(giOverviewType)] = topNow;
                    OVScrollKnob->m_y =
                        static_cast<i16>(widgetMessage.payload.mouse.y - SCROLL_KNOB_OFFSET);
                    SetupDynamicStuff(1, 0, 0);
                    topBefore = topNow;
                } else {
                    overWin->DrawWindow(1, 0, WINDOW_DRAW_ID_LIMIT);
                }
            }
            platform::PumpEvents();
            widgetMessage = gpInputManager->GetEvent();
            if (widgetMessage.type == MESSAGE_MOUSE_MOVE) {
                b32 discardMouseMoves = true;
                while (discardMouseMoves) {
                    pendingMessage = gpInputManager->PeekEvent();
                    if (pendingMessage.type == MESSAGE_MOUSE_MOVE) {
                        widgetMessage = gpInputManager->GetEvent();
                    } else {
                        discardMouseMoves = false;
                    }
                }
            }
        }
        OVScrollKnob->m_flags &= ~WIDGET_FLAG_SELECTED;
        SetupDynamicStuff(0, 1, 0);
        overWin->DrawWindow(
            false,
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

MessageDispatchResult OverviewHandler(struct tag_message& message) {
    i32 topIndex;
    i32 scrollSpan;
    i32 scrollDivisor;
    i32 done;
    b32 quickView;
    i32 y;

    done = 0;
    quickView = false;
    const auto selectOverview = [](OverviewType type) {
        if (giOverviewType != type)
            gpGame->SetupNewOverviewType(type, 1);
    };
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                quickView = true;
            case WIDGET_COMMAND_SELECT:
                if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                    quickView = true;
                }
                switch (message.payload.widget.id) {
                    case SCROLL_KNOB_WIDGET:
                        if (quickView != 0) {
                            break;
                        }
                        gpGame->DoKnob();
                        break;
                    case SCROLL_TRACK_WIDGET:
                        if (quickView != 0) {
                            break;
                        }
                        if (giOverviewItems[H2EnumIndex(giOverviewType)] <= OVERVIEW_VISIBLE_ROWS) {
                            break;
                        }
                        scrollSpan =
                            giOverviewItems[H2EnumIndex(giOverviewType)] - (OVERVIEW_VISIBLE_ROWS - 1);
                        scrollDivisor = SCROLL_SCALED_TRACK_RANGE / scrollSpan;
                        y = message.payload.mouse.screenY;
                        y -= SCROLL_MIN_Y;
                        y -= SCROLL_KNOB_OFFSET;
                        topIndex = y * SCROLL_SCALE / scrollDivisor;
                        giOverviewTop[H2EnumIndex(giOverviewType)] = topIndex;
                        if (giOverviewTop[H2EnumIndex(giOverviewType)] + (OVERVIEW_VISIBLE_ROWS - 1)
                            >= giOverviewItems[H2EnumIndex(giOverviewType)]) {
                            giOverviewTop[H2EnumIndex(giOverviewType)] =
                                giOverviewItems[H2EnumIndex(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
                        }
                        if (giOverviewTop[H2EnumIndex(giOverviewType)] < 0) {
                            giOverviewTop[H2EnumIndex(giOverviewType)] = 0;
                        }
                        gpGame->SetupDynamicStuff(1, 1, 0);
                        break;
                    case HERO_TAB_WIDGET:
                        if (quickView != 0) {
                            break;
                        }
                        selectOverview(OVERVIEW_HEROES);
                        break;
                    case TOWN_TAB_WIDGET:
                        if (quickView != 0) {
                            break;
                        }
                        selectOverview(OVERVIEW_TOWNS);
                        break;
                    default:
                        done = gpGame->ProcessIconSelect(message.payload.widget.id, quickView);
                        break;
                }
                break;
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case HERO_TITLE_WIDGET:
                        selectOverview(OVERVIEW_HEROES);
                        break;
                    case TOWN_TITLE_WIDGET:
                        selectOverview(OVERVIEW_TOWNS);
                        break;
                    case SCROLL_UP_WIDGET:
                        if (giOverviewTop[H2EnumIndex(giOverviewType)] > 0) {
                            giOverviewTop[H2EnumIndex(giOverviewType)]--;
                            gpGame->SetupDynamicStuff(1, 1, 0);
                        }
                        break;
                    case SCROLL_DOWN_WIDGET:
                        if (giOverviewTop[H2EnumIndex(giOverviewType)]
                            < giOverviewItems[H2EnumIndex(giOverviewType)] - OVERVIEW_VISIBLE_ROWS) {
                            giOverviewTop[H2EnumIndex(giOverviewType)]++;
                            gpGame->SetupDynamicStuff(1, 1, 0);
                        }
                        break;
                    case CLOSE_WIDGET:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        done = 1;
                        break;
                }
                break;
        }
    }

    if (message.type == MESSAGE_KEY_DOWN) {
        switch (message.payload.keyboard.keyCode) {
            case INPUT_SCAN_NUMPAD_9:
                giOverviewTop[H2EnumIndex(giOverviewType)] -= OVERVIEW_VISIBLE_ROWS;
                if (giOverviewTop[H2EnumIndex(giOverviewType)] < 0) {
                    giOverviewTop[H2EnumIndex(giOverviewType)] = 0;
                }
                gpGame->SetupDynamicStuff(1, 1, 0);
                break;
            case INPUT_SCAN_NUMPAD_3:
                giOverviewTop[H2EnumIndex(giOverviewType)] += OVERVIEW_VISIBLE_ROWS;
                if (giOverviewTop[H2EnumIndex(giOverviewType)]
                    > giOverviewItems[H2EnumIndex(giOverviewType)] - OVERVIEW_VISIBLE_ROWS) {
                    giOverviewTop[H2EnumIndex(giOverviewType)] =
                        giOverviewItems[H2EnumIndex(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
                }
                gpGame->SetupDynamicStuff(1, 1, 0);
                break;
            case INPUT_SCAN_NUMPAD_7:
                giOverviewTop[H2EnumIndex(giOverviewType)] = 0;
                gpGame->SetupDynamicStuff(1, 1, 0);
                break;
            case INPUT_SCAN_NUMPAD_1:
                giOverviewTop[H2EnumIndex(giOverviewType)] =
                    giOverviewItems[H2EnumIndex(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
                gpGame->SetupDynamicStuff(1, 1, 0);
                break;
        }
    }
    if (done == 1) {
        message.payload.widget.id = SCROLL_UP_WIDGET;
        message.payload.widget.command = BaseWidgetCommand(SCROLL_UP_WIDGET);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

i32 game::ProcessIconSelect(i32 widgetId, b32 quickView) {
    i32 item;
    town* selectedTown;
    hero* curHero;
    i32 rowIndex;

    if (widgetId >= ICON_ROW_BASE && widgetId <= ICON_ROW_LIMIT - 1) {
        rowIndex = (widgetId - ICON_ROW_BASE) / OVERVIEW_ROW_ID_STRIDE;
        if (giOverviewTop[H2EnumIndex(giOverviewType)] + rowIndex > giOverviewItems[H2EnumIndex(giOverviewType)]) {
            return 0;
        }
        widgetId -= ICON_ROW_BASE;
        widgetId %= OVERVIEW_ROW_ID_STRIDE;

        if (giOverviewType == OVERVIEW_HEROES) {
            curHero =
                GetHero(gpCurPlayer->m_heroIds[giOverviewTop[H2EnumIndex(giOverviewType)] + rowIndex]);
            if (widgetId >= HERO_SELECT_FIRST && widgetId <= HERO_SELECT_LAST) {
                giOverviewReturnAction = OVERVIEW_RETURN_HERO;
                giOverviewReturnActionExtra =
                    gpCurPlayer->m_heroIds[giOverviewTop[H2EnumIndex(giOverviewType)] + rowIndex];
                return 1;
            }
            if (widgetId >= HERO_ARMY_ALT_FIRST
                && widgetId <= HERO_ARMY_ALT_LAST) {
                widgetId -= OVERVIEW_TROOP_SLOTS;
            }
            if (widgetId >= HERO_ARMY_FIRST && widgetId <= HERO_ARMY_LAST) {
                item = widgetId - HERO_ARMY_FIRST;
                gpGame->ViewArmy(
                    OVERVIEW_VIEW_ARMY_X,
                    OVERVIEW_VIEW_ARMY_Y,
                    curHero->m_army.m_creatureTypes[item],
                    curHero->m_army.m_creatureCounts[item],
                    NULL,
                    curHero->m_army.GetNumArmies() == 1,
                    ARMY_FACING_RIGHT,
                    quickView,
                    curHero,
                    NULL,
                    &curHero->m_army,
                    item
                );
                if (quickView == 0) {
                    SetupDynamicStuff(1, 1, 1);
                }
            }
            if (widgetId >= HERO_SKILL_FIRST && widgetId <= HERO_SKILL_LAST) {
                HeroSecondarySkill secondarySkill12 =
                    curHero->GetNthSS(widgetId - HERO_SKILL_FIRST);
                curHero->DoSSLevelDialog(secondarySkill12, quickView);
            }
            if (widgetId >= HERO_ARTIFACT_FIRST
                && widgetId <= HERO_ARTIFACT_LAST) {
                ArtifactType selectedArtifact =
                    curHero->m_artifacts[widgetId - HERO_ARTIFACT_FIRST];
                if (
                    selectedArtifact == ARTIFACT_MAGIC_BOOK
                ) {
                    gpGame->ViewSpells(curHero, SPELL_TYPE_ALL, ViewSpecialHandler, 1);
                } else {
                    curHero->ViewArtifact(
                        selectedArtifact,
                        quickView,
                        curHero->m_artifactExtra[widgetId - HERO_ARTIFACT_FIRST]
                    );
                }
            }
            if (widgetId >= HERO_STAT_FIRST && widgetId <= HERO_STAT_LAST) {
                item = widgetId - HERO_STAT_FIRST;
                NormalDialog(
                    gStatDesc[item],
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
            selectedTown =
                GetTown(gpCurPlayer->m_townIds[giOverviewTop[H2EnumIndex(giOverviewType)] + rowIndex]);
            if (widgetId == TOWN_SELECT_WIDGET) {
                giOverviewReturnAction = OVERVIEW_RETURN_TOWN;
                giOverviewReturnActionExtra =
                    gpCurPlayer->m_townIds[giOverviewTop[H2EnumIndex(giOverviewType)] + rowIndex];
                return 1;
            }
            if (widgetId >= TOWN_HERO_FIRST && widgetId <= TOWN_HERO_LAST
                && selectedTown->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
                giOverviewReturnAction = OVERVIEW_RETURN_HERO;
                giOverviewReturnActionExtra = selectedTown->m_occupyingHeroId;
                return 1;
            }
            if (widgetId >= TOWN_ARMY_ALT_FIRST
                && widgetId <= TOWN_ARMY_ALT_LAST) {
                widgetId -= OVERVIEW_TROOP_SLOTS;
            }
            if (widgetId >= TOWN_ARMY_FIRST && widgetId <= TOWN_ARMY_LAST) {
                item = widgetId - TOWN_ARMY_FIRST;
                gpGame->ViewArmy(
                    OVERVIEW_VIEW_ARMY_X,
                    OVERVIEW_VIEW_ARMY_Y,
                    selectedTown->m_army.m_creatureTypes[item],
                    selectedTown->m_army.m_creatureCounts[item],
                    selectedTown,
                    0,
                    ARMY_FACING_RIGHT,
                    quickView,
                    NULL,
                    NULL,
                    &selectedTown->m_army,
                    item
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
                item = widgetId - TOWN_RECRUIT_FIRST;
                if (quickView != 0) {
                    QuickViewRecruit(selectedTown, item);
                } else {
                    recruitUnit* recruitDialog = new recruitUnit(selectedTown, item, 0);
                    if (recruitDialog == NULL) {
                        MemError();
                    }
                    gpExec->DoDialog(recruitDialog);
                    delete recruitDialog;
                    gpResourceManager->GetBackdrop(
                        "overback.icn",
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

class heroWindow* overWin = NULL;
class textWidget** textWidgetDynamic = NULL;
class iconWidget** iconWidgetDynamic = NULL;
OverviewType giOverviewType = OVERVIEW_HEROES;
i32 giOverviewTop[H2EnumIndex(OVERVIEW_TYPE_COUNT)] = {0};
class iconWidget* OVScrollKnob = NULL;
OverviewType iLastDynamicType;
i32 iLastDynamicTop;
i32 iOverviewItems;
i32 giOverviewItems[H2EnumIndex(OVERVIEW_TYPE_COUNT)];
class textWidget* textWidgetTitle[OVERVIEW_TITLE_COUNT];

#undef OVERVIEW_SCROLL_ROUNDING_OFFSET
#undef OVERVIEW_SCROLL_MIN_Y_FLOAT
#undef OVERVIEW_SCROLL_KNOB_RANGE_FLOAT
