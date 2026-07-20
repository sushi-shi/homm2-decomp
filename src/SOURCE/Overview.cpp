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
#include <SOURCE/town.h>
#include <stdio.h>
#include <string.h>

H2_ENUM_BEGIN(OverviewUiConstant)
    DYNAMIC_ARRAY_BYTES      = 0x2bc0,
    HERO_TAB_WIDGET          = 100,
    TOWN_TAB_WIDGET          = 0x65,
    TITLE_WIDGET             = 0x66,
    SCROLL_TRACK_WIDGET      = 0x6e,
    SCROLL_KNOB_WIDGET       = 0x6f,
    SCROLL_UP_WIDGET         = 10,
    SCROLL_DOWN_WIDGET       = 11,
    CLOSE_WIDGET             = 14,
    TITLE_COUNT              = 3,
    RESOURCE_COUNT           = 7,
    RESOURCE_LAST            = 6,
    RESOURCE_FIRST_WIDGET    = 30,
    MINE_FIRST_WIDGET        = 20,
    TOWN_TITLE_WIDGET        = 20,
    HERO_TITLE_WIDGET        = 35,
    DAILY_GOLD_WIDGET        = 40,
    MINE_COUNT               = 144,
    SAWMILL_RESOURCE         = 100,
    FADE_STEPS               = 8,
    SCROLL_MIN_Y             = 18,
    SCROLL_RANGE             = 304,
    SCROLL_KNOB_OFFSET       = 9,
    SCROLL_LAST_PIXEL_ADJUST = 1,
    SCROLL_TRACK_Y           = 27,
    SCROLL_SCALE             = 100,
    DIALOG_CONTINUE          = 1,
    DIALOG_CLOSE             = 2,
    RETURN_NONE              = -1,
    RETURN_HERO              = 1,
    RETURN_TOWN              = 2,
    ICON_ROW_BASE            = 200,
    ICON_ROW_LIMIT           = 1000,
    HERO_SELECT_FIRST        = 100,
    HERO_SELECT_LAST         = 103,
    HERO_ARMY_FIRST          = 104,
    HERO_ARMY_LAST           = 108,
    HERO_ARMY_ALT_FIRST      = 109,
    HERO_ARMY_ALT_LAST       = 113,
    HERO_ARTIFACT_FIRST      = 114,
    HERO_ARTIFACT_LAST       = 127,
    HERO_SKILL_FIRST         = 146,
    HERO_SKILL_LAST          = 153,
    HERO_STAT_FIRST          = 170,
    HERO_STAT_LAST           = 173,
    TOWN_SELECT_WIDGET       = 4,
    TOWN_ARMY_FIRST          = 5,
    TOWN_ARMY_LAST           = 9,
    TOWN_ARMY_ALT_FIRST      = 10,
    TOWN_ARMY_ALT_LAST       = 14,
    TOWN_RECRUIT_FIRST       = 15,
    TOWN_RECRUIT_LAST        = 26,
    TOWN_RECRUIT_ALT_FIRST   = 27,
    TOWN_RECRUIT_ALT_LAST    = 38,
    TOWN_HERO_FIRST          = 39,
    TOWN_HERO_LAST           = 43
H2_ENUM_END(OverviewUiConstant)

#define RETAIL_FILE const_cast<char*>("I:\\Projects\\Heroes\\Prog\\SOURCE\\Overview.cpp")

#define OVERVIEW_TEXT_WIDGET_ROWS (reinterpret_cast<OverviewTextWidgetRow*>(textWidgetDynamic))
#define OVERVIEW_ICON_WIDGET_ROWS (reinterpret_cast<OverviewIconWidgetRow*>(iconWidgetDynamic))

VA(0x00407870, 0x223e)
void game::SetupDynamicStuff(i32 redraw, i32 updateKnob, i32 forceUpdate) {
    DATA(0x004eda04) static i16 overviewDynamicSourceLine = 116;
    i32 row;
    i32 item;
    i32 textItemCount;
    i32 iconCount;
    i32 rowWidgetId6;
    char* valueText0;

    if (updateKnob) {
        if (giOverviewItems[IDX(giOverviewType)] <= OVERVIEW_VISIBLE_ROWS) {
            OVScrollKnob->m_y = 169;
        } else {
            i32 scrollDivisor = giOverviewItems[IDX(giOverviewType)] - OVERVIEW_VISIBLE_ROWS;
            float knobScale = 303.0 / scrollDivisor;
            OVScrollKnob->m_y =
                static_cast<i16>(giOverviewTop[IDX(giOverviewType)] * knobScale + 18.0f + 0.4);
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
            30,
            static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 17),
            594,
            82,
            const_cast<char*>("overview.icn"),
            static_cast<i16>(IDX(giOverviewType) + 10),
            0,
            static_cast<i16>(rowWidgetId6 + 2),
            16,
            1
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
                valueText0 = static_cast<char*>(H2_ALLOC(strlen(record->m_name) + 1, 195));
                strcpy(valueText0, record->m_name);

                OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount] = new textWidget(
                    35,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 80),
                    132,
                    10,
                    valueText0,
                    const_cast<char*>("smalfont.fnt"),
                    FONT_DRAW_DEFAULT,
                    static_cast<i16>(rowWidgetId6 + 3),
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
                overWin->AddWidget(OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount], -1);
                textItemCount++;
            }

            {
                i32 townFrame = IDX(record->m_type);
                if ((record->m_buildings & IDX(TOWN_BUILDING_CASTLE)) == 0) {
                    townFrame += 6;
                }
                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    47,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 36),
                    46,
                    22,
                    const_cast<char*>("minitown.icn"),
                    townFrame,
                    0,
                    static_cast<i16>(rowWidgetId6 + 4),
                    16,
                    1
                );
                if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                iconCount++;
            }

            if (BitTest(gpGame->m_knownTowns, static_cast<u32>(record->m_id))) {
                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    42,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 31),
                    0,
                    0,
                    const_cast<char*>("locators.icn"),
                    24,
                    0,
                    static_cast<i16>(rowWidgetId6 + 44),
                    16,
                    1
                );
                if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                iconCount++;
            }

            i32 hasCaptain = 0;
            heroData0 = NULL;
            if (record->m_occupyingHeroId != -1) {
                heroData0 = GetHero(record->m_occupyingHeroId);
            } else {
                if ((record->m_buildings & IDX(TOWN_BUILDING_CAPTAIN_QUARTERS)) != 0) {
                    hasCaptain = 1;
                }
            }

            if (heroData0 != NULL || hasCaptain) {
                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    112,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 36),
                    46,
                    22,
                    const_cast<char*>("portxtra.icn"),
                    0,
                    0,
                    static_cast<i16>(rowWidgetId6 + 39),
                    16,
                    1
                );
                if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                iconCount++;

                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    107,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 31),
                    51,
                    27,
                    const_cast<char*>(hasCaptain ? "minicapt.icn" : "miniport.icn"),
                    hasCaptain ? static_cast<u8>(static_cast<FactionType>(record->m_type))
                               : static_cast<u8>(heroData0->m_id),
                    0,
                    static_cast<i16>(rowWidgetId6 + 40),
                    16,
                    1
                );
                if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                iconCount++;

                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    112,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 36),
                    0,
                    0,
                    const_cast<char*>("mobility.icn"),
                    static_cast<i16>(
                        GetMobilityFrame(hasCaptain ? 0 : heroData0->m_remainingMobility)
                    ),
                    0,
                    static_cast<i16>(rowWidgetId6 + 43),
                    16,
                    1
                );
                if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                    MemError();
                }
                overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                iconCount++;

                i32 captainMana = record->m_type == FACTION_BARBARIAN
                                          || record->m_type == FACTION_KNIGHT
                                      ? 10
                                      : 20;
                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    151,
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 36),
                    0,
                    0,
                    const_cast<char*>("mana.icn"),
                    static_cast<i16>(
                        GetManaFrame(hasCaptain ? captainMana : heroData0->m_spellPoints)
                    ),
                    0,
                    static_cast<i16>(rowWidgetId6 + 41),
                    16,
                    1
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
                    if (record->m_army.m_creatureTypes[item] != -1
                        && record->m_army.m_creatureCounts[item] > 0) {
                        OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                            static_cast<i16>(displayedTroops * 41 + 173),
                            static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 35),
                            42,
                            40,
                            const_cast<char*>("mons32.icn"),
                            static_cast<i16>(record->m_army.m_creatureTypes[item]),
                            0,
                            static_cast<i16>(rowWidgetId6 + item + 5),
                            17,
                            1
                        );
                        if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                            MemError();
                        }
                        overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                        iconCount++;

                        valueText0 = static_cast<char*>(H2_ALLOC(6, 354));
                        sprintf(
                            valueText0,
                            "%d",
                            static_cast<i32>(record->m_army.m_creatureCounts[item])
                        );
                        OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount] = new textWidget(
                            static_cast<i16>(displayedTroops * 41 + 173),
                            static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 81),
                            40,
                            12,
                            valueText0,
                            const_cast<char*>("smalfont.fnt"),
                            FONT_DRAW_DEFAULT,
                            static_cast<i16>(rowWidgetId6 + item + 10),
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
                        case 0:
                            detailIndex = 0;
                            break;
                        case 1:
                            detailIndex = 1;
                            break;
                        case 2:
                            detailIndex = 6;
                            break;
                        case 3:
                            detailIndex = 2;
                            break;
                        case 4:
                            detailIndex = 7;
                            break;
                        case 5:
                            detailIndex = 3;
                            break;
                        case 6:
                            detailIndex = 8;
                            break;
                        case 7:
                            detailIndex = 4;
                            break;
                        case 8:
                            detailIndex = 9;
                            break;
                        case 9:
                            detailIndex = 5;
                            break;
                        case 10:
                            detailIndex = 10;
                            break;
                        default:
                            detailIndex = 11;
                            break;
                    }

                    if ((record->m_buildings & (1 << (detailIndex + 19))) != 0) {
                        OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                            static_cast<i16>(displayedTroops * 41 + 378),
                            static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 35),
                            42,
                            40,
                            const_cast<char*>("mons32.icn"),
                            static_cast<i16>(
                                gDwellingType[IDX(record->m_type)][detailIndex]
                            ),
                            0,
                            static_cast<i16>(rowWidgetId6 + detailIndex + 15),
                            17,
                            1
                        );
                        if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                            MemError();
                        }
                        overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                        iconCount++;

                        valueText0 = static_cast<char*>(H2_ALLOC(6, 414));
                        sprintf(
                            valueText0,
                            "%d",
                            static_cast<i32>(record->m_garrison[detailIndex])
                        );
                        OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount] = new textWidget(
                            static_cast<i16>(displayedTroops * 41 + 378),
                            static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 81),
                            40,
                            12,
                            valueText0,
                            const_cast<char*>("smalfont.fnt"),
                            FONT_DRAW_DEFAULT,
                            static_cast<i16>(rowWidgetId6 + detailIndex + 27),
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
                35,
                static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 21),
                46,
                22,
                const_cast<char*>("portxtra.icn"),
                0,
                0,
                static_cast<i16>(rowWidgetId6 + 100),
                16,
                1
            );
            if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                MemError();
            }
            overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
            iconCount++;

            OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                30,
                static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 16),
                51,
                27,
                const_cast<char*>("miniport.icn"),
                static_cast<i16>(heroData0->m_id),
                0,
                static_cast<i16>(rowWidgetId6 + 101),
                16,
                1
            );
            if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                MemError();
            }
            overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
            iconCount++;

            OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                35,
                static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 21),
                0,
                0,
                const_cast<char*>("mobility.icn"),
                static_cast<i16>(GetMobilityFrame(heroData0->m_remainingMobility)),
                0,
                static_cast<i16>(rowWidgetId6 + 103),
                16,
                1
            );
            if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                MemError();
            }
            overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
            iconCount++;

            OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                74,
                static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 21),
                0,
                0,
                const_cast<char*>("mana.icn"),
                static_cast<i16>(GetManaFrame(heroData0->m_spellPoints)),
                0,
                static_cast<i16>(rowWidgetId6 + 41),
                16,
                1
            );
            if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                MemError();
            }
            overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
            iconCount++;

            for (item = 0; item < 4; item++) {
                valueText0 = static_cast<char*>(H2_ALLOC(4, 502));
                sprintf(
                    valueText0,
                    "%d",
                    static_cast<i32>(heroData0->Stats(HeroPrimaryStat(item)))
                );
                OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount] = new textWidget(
                    static_cast<i16>(item * 35 + 105),
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 39),
                    14,
                    8,
                    valueText0,
                    const_cast<char*>("smalfont.fnt"),
                    FONT_DRAW_DEFAULT,
                    static_cast<i16>(rowWidgetId6 + item + 142),
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_RIGHT
                );
                overWin->AddWidget(OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount], -1);
                textItemCount++;
            }

            for (item = 0; item < 4; item++) {
                OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                    static_cast<i16>(item * 35 + 87),
                    static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 17),
                    35,
                    30,
                    const_cast<char*>("overview.icn"),
                    15,
                    0,
                    static_cast<i16>(rowWidgetId6 + item + 170),
                    16,
                    1
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
                    if (heroData0->m_army.m_creatureTypes[item] != -1
                        && heroData0->m_army.m_creatureCounts[item] > 0) {
                        OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                            static_cast<i16>(displayedTroops * 40 + 30),
                            static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 47),
                            42,
                            40,
                            const_cast<char*>("mons32.icn"),
                            static_cast<i16>(heroData0->m_army.m_creatureTypes[item]),
                            0,
                            static_cast<i16>(rowWidgetId6 + item + 104),
                            17,
                            1
                        );
                        if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                            MemError();
                        }
                        overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);
                        iconCount++;

                        valueText0 = static_cast<char*>(H2_ALLOC(6, 561));
                        sprintf(
                            valueText0,
                            "%d",
                            static_cast<i32>(heroData0->m_army.m_creatureCounts[item])
                        );
                        OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount] = new textWidget(
                            static_cast<i16>(displayedTroops * 40 + 30),
                            static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + 87),
                            40,
                            12,
                            valueText0,
                            const_cast<char*>("smalfont.fnt"),
                            FONT_DRAW_DEFAULT,
                            static_cast<i16>(rowWidgetId6 + item + 109),
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
                if (detailIndex != -1) {
                    detailRow = item / OVERVIEW_SECONDARY_SKILL_COLUMNS;
                    detailColumn = item % OVERVIEW_SECONDARY_SKILL_COLUMNS;
                    OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] = new iconWidget(
                        static_cast<i16>(detailColumn * 35 + 233),
                        static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + detailRow * 42 + 20),
                        34,
                        34,
                        const_cast<char*>("overview.icn"),
                        12,
                        0,
                        static_cast<i16>(rowWidgetId6 + item + 154),
                        16,
                        1
                    );
                    if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                        MemError();
                    }
                    overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);

                    OVERVIEW_ICON_WIDGET_ROWS[row][iconCount + 1] = new iconWidget(
                        static_cast<i16>(detailColumn * 35 + 234),
                        static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + detailRow * 42 + 21),
                        32,
                        32,
                        const_cast<char*>("miniss.icn"),
                        static_cast<i16>(detailIndex),
                        0,
                        static_cast<i16>(rowWidgetId6 + item + 146),
                        16,
                        1
                    );
                    if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount + 1] == NULL) {
                        MemError();
                    }
                    overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount + 1], -1);
                    iconCount += 2;

                    valueText0 = static_cast<char*>(H2_ALLOC(2, 623));
                    sprintf(valueText0, "%d", static_cast<i32>(heroData0->GetSSLevel(detailIndex)));
                    OVERVIEW_TEXT_WIDGET_ROWS[row][textItemCount] = new textWidget(
                        static_cast<i16>(detailColumn * 35 + 255),
                        static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + detailRow * 42 + 45),
                        8,
                        8,
                        valueText0,
                        const_cast<char*>("smalfont.fnt"),
                        FONT_DRAW_DEFAULT,
                        static_cast<i16>(rowWidgetId6 + item + 162),
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
                        static_cast<i16>(detailColumn * 35 + 378),
                        static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + detailRow * 42 + 20),
                        34,
                        34,
                        const_cast<char*>("overview.icn"),
                        12,
                        0,
                        static_cast<i16>(rowWidgetId6 + item + 128),
                        16,
                        1
                    );
                    if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount] == NULL) {
                        MemError();
                    }
                    overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount], -1);

                    OVERVIEW_ICON_WIDGET_ROWS[row][iconCount + 1] = new iconWidget(
                        static_cast<i16>(detailColumn * 35 + 379),
                        static_cast<i16>(row * OVERVIEW_ROW_HEIGHT + detailRow * 42 + 21),
                        32,
                        32,
                        const_cast<char*>("artfx.icn"),
                        static_cast<i16>(heroData0->m_artifacts[item]),
                        0,
                        static_cast<i16>(rowWidgetId6 + item + 114),
                        16,
                        1
                    );
                    if (OVERVIEW_ICON_WIDGET_ROWS[row][iconCount + 1] == NULL) {
                        MemError();
                    }
                    overWin->AddWidget(OVERVIEW_ICON_WIDGET_ROWS[row][iconCount + 1], -1);
                    iconCount++;
                    iconCount++;
                    displayedArtifacts++;
                }
            }
        }
    }

    if (redraw) {
        overWin->DrawWindow(0, 110, 999);
        gpWindowManager->UpdateScreenRegion(30, 16, 610, 341);
    }
}

VA(0x00409aae, 0x357)
void game::SetupNewOverviewType(OverviewType overviewType, i32 redrawFrom) {
    DATA(0x004edca8) static i16 overviewSetupSourceLine = 698;
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
    message.payload.widget.data.value = IDX(giOverviewType) + 6;
    overWin->BroadcastMessage(message);
    message.payload.widget.command = OVERVIEW_WIDGET_SET_FRAME;
    message.payload.widget.id = HERO_TAB_WIDGET;
    message.payload.widget.data.value = giOverviewType == OVERVIEW_HEROES;
    overWin->BroadcastMessage(message);
    message.payload.widget.command = OVERVIEW_WIDGET_SET_FRAME;
    message.payload.widget.id = TOWN_TAB_WIDGET;
    if (giOverviewType == OVERVIEW_TOWNS) {
        message.payload.widget.data.value = 3;
    } else {
        message.payload.widget.data.value = 2;
    }
    overWin->BroadcastMessage(message);

    i16 titleLefts[2][TITLE_COUNT] = {{35, 232, 377}, {35, 177, 379}};
    i16 titleWidth[2][TITLE_COUNT] = {{194, 142, 241}, {140, 199, 239}};

    for (title = 0; title < TITLE_COUNT; title++) {
        if (textWidgetTitle[title] != NULL) {
            overWin->RemoveWidget(textWidgetTitle[title]);
            delete textWidgetTitle[title];
            textWidgetTitle[title] = NULL;
        }
    }
    for (title = 0; title < TITLE_COUNT; title++) {
        titleCopy = static_cast<char*>(H2_ALLOC(
            strlen(cOverviewText[IDX(giOverviewType) * IDX(TITLE_COUNT) + title]) + 1,
            740
        ));
        strcpy(titleCopy, cOverviewText[IDX(giOverviewType) * IDX(TITLE_COUNT) + title]);
        textWidgetTitle[title] = new textWidget(
            titleLefts[IDX(giOverviewType)][OD_STEER(title)],
            3,
            titleWidth[IDX(giOverviewType)][OD_STEER(title)],
            12,
            titleCopy,
            const_cast<char*>("smalfont.fnt"),
            FONT_DRAW_DEFAULT,
            static_cast<i16>(title + 110),
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        overWin->AddWidget(textWidgetTitle[title], -1);
    }
    SetupDynamicStuff(0, 1, 0);
    if (redrawFrom != 0) {
        overWin->DrawWindow(redrawFrom, HERO_TAB_WIDGET, 999);
    }
}

VA(0x00409e05, 0x84)
void game::SetupResources(void) {
    i32 resourceIdx;
    tag_message message;

    message.type = MESSAGE_WIDGET;
    for (resourceIdx = 0; resourceIdx < RESOURCE_COUNT; resourceIdx++) {
        message.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
        message.payload.widget.data.text = gText;
        sprintf(gText, "%d", gpCurPlayer->m_resources[resourceIdx]);
        message.payload.widget.id = resourceIdx + RESOURCE_FIRST_WIDGET;
        overWin->BroadcastMessage(message);
    }
}

VA(0x00409e89, 0x4c7)
void game::Overview(void) {
    DATA(0x004edcec) static i16 overviewDialogSourceLine = 778;
    char mineCounts4[8];
    i32 sawmills4;
    i32 mine4;
    tag_message message8;
    i32 row0;

    giOverviewReturnAction = RETURN_NONE;
    giOverviewReturnActionExtra = RETURN_NONE;
    message8.type = MESSAGE_WIDGET;
    gpAdvManager->TrimLoopingSounds(4);
    gpWindowManager->FadeScreen(FADE_OUT, FADE_STEPS, NULL);
    for (mine4 = 0; mine4 < TITLE_COUNT; mine4++) {
        textWidgetTitle[mine4] = NULL;
    }

    textWidgetDynamic = static_cast<textWidget**>(H2_ALLOC(DYNAMIC_ARRAY_BYTES, 795));
    iconWidgetDynamic = static_cast<iconWidget**>(H2_ALLOC(DYNAMIC_ARRAY_BYTES, 796));
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
    gpMouseManager->SetPointer(const_cast<char*>("advmice.mse"), 0, MOUSE_AUTO_CURSOR_TYPE);
    gpResourceManager->GetBackdrop(const_cast<char*>("overback.icn"), gpWindowManager->m_screen, 1);
    overWin = new heroWindow(0, 0, const_cast<char*>("overwind.bin"));
    if (overWin == NULL) {
        MemError();
    }
    SetWinText(overWin, 9);
    OVScrollKnob = new iconWidget(
        629,
        18,
        8,
        17,
        const_cast<char*>("scrollcn.icn"),
        4,
        0,
        SCROLL_KNOB_WIDGET,
        16,
        1
    );
    if (OVScrollKnob == NULL) {
        MemError();
    }
    overWin->AddWidget(OVScrollKnob, -1);

    SetupResources();
    memset(mineCounts4, 0, RESOURCE_COUNT);
    sawmills4 = 0;
    for (mine4 = 0; mine4 < MINE_COUNT; mine4++) {
        if (m_mineOwners[mine4] == giCurPlayer) {
            if (m_mines[mine4].resourceType <= RESOURCE_LAST) {
                mineCounts4[m_mines[mine4].resourceType]++;
            } else if (m_mines[mine4].resourceType == SAWMILL_RESOURCE) {
                sawmills4++;
            }
        }
    }
    for (mine4 = 0; mine4 < RESOURCE_COUNT; mine4++) {
        message8.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
        message8.payload.widget.data.text = gText;
        sprintf(gText, "%d", static_cast<i32>(mineCounts4[mine4]));
        message8.payload.widget.id = mine4 + MINE_FIRST_WIDGET;
        overWin->BroadcastMessage(message8);
    }
    for (mine4 = OVERVIEW_VISIBLE_ROWS; mine4 > 0; mine4--) {
        if (OD_STEER(sawmills4) < mine4) {
            message8.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message8.payload.widget.id = mine4 + 49;
            message8.payload.widget.data.value = 4;
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
    gpWindowManager->FadeScreen(FADE_OUT, FADE_STEPS, NULL);
    delete overWin;
    overWin = NULL;
    H2_FREE(textWidgetDynamic, 882);
    H2_FREE(iconWidgetDynamic, 883);
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
        overWin->DrawWindow(0, 110, 111);
        gpWindowManager->UpdateScreenRegion(627, 17, 11, 322);
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
                        scrollDivisor3 = 32100 / scrollItemCount0;
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
        return DIALOG_CLOSE;
    }
    return DIALOG_CONTINUE;
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
                giOverviewReturnAction = RETURN_HERO;
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
                    119,
                    20,
                    selectedHero13->m_army.m_creatureTypes[selectionIndex2],
                    selectedHero13->m_army.m_creatureCounts[selectionIndex2],
                    NULL,
                    selectedHero13->m_army.GetNumArmies() == 1,
                    1,
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
                        quickView == 0 ? 1 : 4,
                        -1,
                        28,
                        -1,
                        0,
                        -1,
                        0,
                        -1,
                        0
                    );
                }
            }
            if (widgetId >= HERO_STAT_FIRST && widgetId <= HERO_STAT_LAST) {
                selectionIndex2 = widgetId - HERO_STAT_FIRST;
                NormalDialog(
                    gStatDesc[selectionIndex2],
                    quickView == 0 ? 1 : 4,
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
        if (giOverviewType == OVERVIEW_TOWNS) {
            selectedTown3 =
                GetTown(gpCurPlayer->m_townIds[giOverviewTop[IDX(giOverviewType)] + row0]);
            if (widgetId == TOWN_SELECT_WIDGET) {
                giOverviewReturnAction = RETURN_TOWN;
                giOverviewReturnActionExtra =
                    gpCurPlayer->m_townIds[giOverviewTop[IDX(giOverviewType)] + row0];
                return 1;
            }
            if (widgetId >= TOWN_HERO_FIRST && widgetId <= TOWN_HERO_LAST
                && selectedTown3->m_occupyingHeroId != -1) {
                giOverviewReturnAction = RETURN_HERO;
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
                    119,
                    20,
                    selectedTown3->m_army.m_creatureTypes[selectionIndex2],
                    selectedTown3->m_army.m_creatureCounts[selectionIndex2],
                    selectedTown3,
                    0,
                    1,
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
DATA(0x004ed9f8) i32 giOverviewTop[2] = {0};
DATA(0x004eda00) class iconWidget* OVScrollKnob = NULL;
DATA(0x00523258) OverviewType iLastDynamicType;
DATA(0x0052325c) i32 iLastDynamicTop;
DATA(0x00523260) i32 iOverviewItems;
DATA(0x00523268) i32 giOverviewItems[2];
DATA(0x00523270) class textWidget* textWidgetTitle[4];

#undef RETAIL_FILE
