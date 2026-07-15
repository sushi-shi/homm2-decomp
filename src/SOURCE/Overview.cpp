// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Overview.obj   from: (directly linked into exe)
// functions: 7   data: 11
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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
#include <SOURCE/playerData.h>
#include <SOURCE/recruitUnit.h>
#include <SOURCE/town.h>
#include <stdio.h>
#include <string.h>

#define OVERVIEW_SOURCE_FILE const_cast<char *>("I:\\Projects\\Heroes\\Prog\\SOURCE\\Overview.cpp")

DATA(0x004edca8)
static short overviewSetupSourceLine = OVERVIEW_SETUP_SOURCE_LINE;
DATA(0x004edcec)
static short overviewDialogSourceLine = OVERVIEW_DIALOG_SOURCE_LINE;

#define OVERVIEW_SETUP_LINE overviewSetupSourceLine

// @match-note: live 98.94%. Recovered flat row-major cleanup indexing, the
// GetTown/GetHero inline accessors and both retail continuation jumps, the two
// distinct final icon-count increments, cached row widget-ID base, captain CFG,
// and shared skill/artifact locals. Frame 0xe8, this -0xdc, primary slots, CFG,
// and semantics agree. All 340 relocation offsets are present; the first real
// residual is the cached-type equality at +0xc6/+0xcc, where ours names
// giOverviewType then iLastDynamicType and retail names them in reverse. MSVC
// canonicalizes both commuted equality spellings identically. Knob scale/divisor
// remain ours -0x1c/-0x20 versus retail -0x20/-0x24, and later detail locals have
// different slot identities. The next aligned symbol is now only one byte early
// (ours +0x223f, retail +0x2240). Rejected: row-pointer globals (wrong ABI),
// iconCounts[2] (96.44%), separate text locals plus function-wide hero (0xf0),
// hoisted captainMana, top-level padding dword, coordinate-pair splits, and slot
// suffix variants. Revisit for slot/equality steering; do not repeat these forms.
VA(0x00407870, 0x223e)
void game::SetupDynamicStuff(int redraw, int updateKnob, int forceUpdate)
{
    int row;
    int item;
    int textItemCount;
    int iconCount;
    int rowWidgetId6;
    char *valueText0;

    if (updateKnob) {
        if (giOverviewItems[giOverviewType] <= OVERVIEW_VISIBLE_ROWS) {
            OVScrollKnob->m_y = 169;
        } else {
            int scrollDivisor = giOverviewItems[giOverviewType] - OVERVIEW_VISIBLE_ROWS;
            float knobScale = 303.0 / scrollDivisor;
            OVScrollKnob->m_y =
                static_cast<short>(giOverviewTop[giOverviewType] * knobScale + 18.0f + 0.4);
        }
    }

    if (!forceUpdate && giOverviewType == iLastDynamicType
        && giOverviewTop[iLastDynamicType] == iLastDynamicTop) {
        return;
    }

    for (row = 0; row < OVERVIEW_VISIBLE_ROWS; row++) {
        for (item = 0; item < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; item++) {
            if (*(textWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item) != 0) {
                overWin->RemoveWidget(
                    *(textWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item)
                );
                delete *(textWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item);
                *(textWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item) = 0;
            }
            if (*(iconWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item) != 0) {
                overWin->RemoveWidget(
                    *(iconWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item)
                );
                delete *(iconWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item);
                *(iconWidgetDynamic + row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item) = 0;
            }
        }
    }

    for (row = 0; row < OVERVIEW_VISIBLE_ROWS; row++) {
        textItemCount = 0;
        iconCount = textItemCount;
        rowWidgetId6 = row * OVERVIEW_ROW_ID_STRIDE + OVERVIEW_ICON_ROW_BASE;

        if (giOverviewTop[giOverviewType] + row >= giOverviewItems[giOverviewType]) {
            break;
        }

        reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] = new iconWidget(
            30,
            static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 17),
            594,
            82,
            const_cast<char *>("overview.icn"),
            static_cast<short>(giOverviewType + 10),
            0,
            static_cast<short>(rowWidgetId6 + 2),
            16,
            1
        );
        if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] == 0) {
            MemError();
        }
        overWin->AddWidget(
            reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
            -1
        );
        iconCount++;

        if (giOverviewType == OVERVIEW_TOWNS) {
            hero *heroData0;
            town *record = GetTown(
                gpCurPlayer->m_townIds[giOverviewTop[giOverviewType] + row]
            );
            {
                valueText0 = static_cast<char *>(BaseAlloc(
                    strlen(record->m_name) + 1,
                    OVERVIEW_SOURCE_FILE,
                    OVERVIEW_SETUP_LINE + 0x4f
                ));
                strcpy(valueText0, record->m_name);

                reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row][textItemCount] =
                    new textWidget(
                        35,
                        static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 80),
                        132,
                        10,
                        valueText0,
                        const_cast<char *>("smalfont.fnt"),
                        1,
                        static_cast<short>(rowWidgetId6 + 3),
                        0x200,
                        1
                    );
            overWin->AddWidget(
                    reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row][textItemCount],
                    -1
                );
                textItemCount++;
            }

            {
                int townFrame = record->m_type;
                if ((record->m_buildings & TOWN_BUILDING_CASTLE) == 0) {
                    townFrame += 6;
                }
                reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] =
                    new iconWidget(
                        47,
                        static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 36),
                        46,
                        22,
                        const_cast<char *>("minitown.icn"),
                        townFrame,
                        0,
                        static_cast<short>(rowWidgetId6 + 4),
                        16,
                        1
                    );
                if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] == 0) {
                MemError();
                }
            overWin->AddWidget(
                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                    -1
                );
            iconCount++;
            }

            if (BitTest(gpGame->m_knownTowns, static_cast<unsigned int>(record->m_id))) {
                reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] =
                    new iconWidget(
                        42,
                        static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 31),
                        0,
                        0,
                        const_cast<char *>("locators.icn"),
                        24,
                        0,
                        static_cast<short>(rowWidgetId6 + 44),
                        16,
                        1
                    );
                if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] == 0) {
                    MemError();
                }
                overWin->AddWidget(
                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                    -1
                );
                iconCount++;
            }

            int hasCaptain = 0;
            heroData0 = 0;
            if (record->m_occupyingHeroId != -1) {
                heroData0 = GetHero(record->m_occupyingHeroId);
            } else {
                if ((record->m_buildings & TOWN_BUILDING_CAPTAIN_QUARTERS) != 0) {
                    hasCaptain = 1;
                }
            }

            if (heroData0 != 0 || hasCaptain) {
                reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] =
                    new iconWidget(
                        112,
                        static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 36),
                        46,
                        22,
                        const_cast<char *>("portxtra.icn"),
                        0,
                        0,
                        static_cast<short>(rowWidgetId6 + 39),
                        16,
                        1
                    );
                if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] == 0) {
                    MemError();
                }
                overWin->AddWidget(
                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                    -1
                );
                iconCount++;

                reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] =
                    new iconWidget(
                        107,
                        static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 31),
                        51,
                        27,
                        hasCaptain ? "minicapt.icn" : "miniport.icn",
                        hasCaptain ? static_cast<unsigned char>(record->m_type)
                                   : static_cast<unsigned char>(heroData0->m_id),
                        0,
                        static_cast<short>(rowWidgetId6 + 40),
                        16,
                        1
                    );
                if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount]
                    == 0) {
                    MemError();
                }
                overWin->AddWidget(
                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                    -1
                );
                iconCount++;

                reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] =
                    new iconWidget(
                        112,
                        static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 36),
                        0,
                        0,
                        const_cast<char *>("mobility.icn"),
                        static_cast<short>(
                            GetMobilityFrame(hasCaptain ? 0 : heroData0->m_remainingMobility)
                        ),
                        0,
                        static_cast<short>(rowWidgetId6 + 43),
                        16,
                        1
                    );
                if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount]
                    == 0) {
                    MemError();
                }
                overWin->AddWidget(
                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                    -1
                );
                iconCount++;

                int captainMana =
                    record->m_type == TOWN_TYPE_BARBARIAN || record->m_type == TOWN_TYPE_KNIGHT
                        ? 10
                        : 20;
                reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] =
                    new iconWidget(
                        151,
                        static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 36),
                        0,
                        0,
                        const_cast<char *>("mana.icn"),
                        static_cast<short>(
                            GetManaFrame(hasCaptain ? captainMana : heroData0->m_spellPoints)
                        ),
                        0,
                        static_cast<short>(rowWidgetId6 + 41),
                        16,
                        1
                    );
                if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount]
                    == 0) {
                    MemError();
                }
                overWin->AddWidget(
                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                    -1
                );
                iconCount++;
            }

            {
            int displayedTroops = 0;
            for (item = 0; item < OVERVIEW_TROOP_SLOTS; item++) {
                    if (record->m_army.m_creatureTypes[item] != -1
                        && record->m_army.m_creatureCounts[item] > 0) {
                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] =
                        new iconWidget(
                            static_cast<short>(displayedTroops * 41 + 173),
                                static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 35),
                                42,
                                40,
                                const_cast<char *>("mons32.icn"),
                                static_cast<short>(record->m_army.m_creatureTypes[item]),
                                0,
                                static_cast<short>(rowWidgetId6 + item + 5),
                                17,
                                1
                            );
                        if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount]
                            == 0) {
                        MemError();
                        }
                    overWin->AddWidget(
                            reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                            -1
                        );
                    iconCount++;

                        valueText0 = static_cast<char *>(
                            BaseAlloc(6, OVERVIEW_SOURCE_FILE, OVERVIEW_SETUP_LINE + 0xee)
                        );
                        sprintf(
                            valueText0,
                            "%d",
                            static_cast<int>(record->m_army.m_creatureCounts[item])
                        );
                        reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row][textItemCount] =
                        new textWidget(
                            static_cast<short>(displayedTroops * 41 + 173),
                                static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 81),
                                40,
                                12,
                                valueText0,
                                const_cast<char *>("smalfont.fnt"),
                                1,
                                static_cast<short>(
                                    rowWidgetId6 + item + 10
                                ),
                                0x200,
                                1
                            );
                    overWin->AddWidget(
                            reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row][textItemCount],
                            -1
                        );
                        textItemCount++;
                    displayedTroops++;
                }
            }
            }

            {
                int displayedTroops = 0;
            for (item = 0; item < OVERVIEW_DWELLING_SLOTS; item++) {
                    int detailIndex;
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
                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] =
                        new iconWidget(
                                static_cast<short>(displayedTroops * 41 + 378),
                                static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 35),
                                42,
                                40,
                                const_cast<char *>("mons32.icn"),
                            static_cast<short>(
                                    gDwellingType[static_cast<signed char>(record->m_type)]
                                                 [detailIndex]
                                ),
                                0,
                                static_cast<short>(
                                    rowWidgetId6 + detailIndex + 15
                                ),
                                17,
                                1
                            );
                        if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount]
                            == 0) {
                        MemError();
                        }
                    overWin->AddWidget(
                            reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                            -1
                        );
                    iconCount++;

                        valueText0 = static_cast<char *>(
                            BaseAlloc(6, OVERVIEW_SOURCE_FILE, OVERVIEW_SETUP_LINE + 0x12a)
                        );
                        sprintf(valueText0, "%d", static_cast<int>(record->m_garrison[detailIndex]));
                        reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row][textItemCount] =
                        new textWidget(
                                static_cast<short>(displayedTroops * 41 + 378),
                                static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 81),
                                40,
                                12,
                                valueText0,
                                const_cast<char *>("smalfont.fnt"),
                                1,
                                static_cast<short>(
                                    rowWidgetId6 + detailIndex + 27
                                ),
                                0x200,
                                1
                            );
                    overWin->AddWidget(
                            reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row][textItemCount],
                            -1
                        );
                        textItemCount++;
                        displayedTroops++;
                    }
                }
            }
        } else {
            hero *heroData0;
            int detailRow;
            int detailColumn;
            int detailIndex;
            heroData0 = &m_heroRecs[gpCurPlayer->m_heroIds[giOverviewTop[giOverviewType] + row]];

            reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] = new iconWidget(
                35,
                static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 21),
                46,
                22,
                const_cast<char *>("portxtra.icn"),
                0,
                0,
                static_cast<short>(rowWidgetId6 + 100),
                16,
                1
            );
            if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] == 0) {
                MemError();
            }
            overWin->AddWidget(
                reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                -1
            );
            iconCount++;

            reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] = new iconWidget(
                30,
                static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 16),
                51,
                27,
                const_cast<char *>("miniport.icn"),
                static_cast<short>(heroData0->m_id),
                0,
                static_cast<short>(rowWidgetId6 + 101),
                16,
                1
            );
            if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] == 0) {
                MemError();
            }
            overWin->AddWidget(
                reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                -1
            );
            iconCount++;

            reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] = new iconWidget(
                35,
                static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 21),
                0,
                0,
                const_cast<char *>("mobility.icn"),
                static_cast<short>(GetMobilityFrame(heroData0->m_remainingMobility)),
                0,
                static_cast<short>(rowWidgetId6 + 103),
                16,
                1
            );
            if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] == 0) {
                MemError();
            }
            overWin->AddWidget(
                reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                -1
            );
            iconCount++;

            reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] = new iconWidget(
                74,
                static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 21),
                0,
                0,
                const_cast<char *>("mana.icn"),
                static_cast<short>(GetManaFrame(heroData0->m_spellPoints)),
                0,
                static_cast<short>(rowWidgetId6 + 41),
                16,
                1
            );
            if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] == 0) {
                MemError();
            }
            overWin->AddWidget(
                reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                -1
            );
            iconCount++;

            for (item = 0; item < 4; item++) {
                valueText0 = static_cast<char *>(
                    BaseAlloc(4, OVERVIEW_SOURCE_FILE, OVERVIEW_SETUP_LINE + 0x182)
                );
                sprintf(valueText0, "%d", static_cast<int>(heroData0->Stats(item)));
                reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row][textItemCount] =
                    new textWidget(
                        static_cast<short>(item * 35 + 105),
                        static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 39),
                        14,
                        8,
                        valueText0,
                        const_cast<char *>("smalfont.fnt"),
                        1,
                        static_cast<short>(rowWidgetId6 + item + 142),
                        0x200,
                        2
                    );
                overWin->AddWidget(
                    reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row][textItemCount],
                    -1
                );
                textItemCount++;
            }

            for (item = 0; item < 4; item++) {
                reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] =
                    new iconWidget(
                        static_cast<short>(item * 35 + 87),
                        static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 17),
                        35,
                        30,
                        const_cast<char *>("overview.icn"),
                        15,
                        0,
                        static_cast<short>(rowWidgetId6 + item + 170),
                        16,
                        1
                    );
                if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] == 0) {
                    MemError();
                }
                overWin->AddWidget(
                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                    -1
                );
                iconCount++;
            }

            {
            int displayedTroops = 0;
            for (item = 0; item < OVERVIEW_TROOP_SLOTS; item++) {
                    if (heroData0->m_army.m_creatureTypes[item] != -1
                        && heroData0->m_army.m_creatureCounts[item] > 0) {
                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] =
                        new iconWidget(
                            static_cast<short>(displayedTroops * 40 + 30),
                                static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 47),
                                42,
                                40,
                                const_cast<char *>("mons32.icn"),
                                static_cast<short>(heroData0->m_army.m_creatureTypes[item]),
                                0,
                                static_cast<short>(
                                    rowWidgetId6 + item + 104
                                ),
                                17,
                                1
                            );
                        if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount]
                            == 0) {
                        MemError();
                        }
                    overWin->AddWidget(
                            reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                            -1
                        );
                    iconCount++;

                        valueText0 = static_cast<char *>(
                            BaseAlloc(6, OVERVIEW_SOURCE_FILE, OVERVIEW_SETUP_LINE + 0x1bd)
                        );
                        sprintf(
                            valueText0,
                            "%d",
                            static_cast<int>(heroData0->m_army.m_creatureCounts[item])
                        );
                        reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row][textItemCount] =
                        new textWidget(
                            static_cast<short>(displayedTroops * 40 + 30),
                                static_cast<short>(row * OVERVIEW_ROW_HEIGHT + 87),
                                40,
                                12,
                                valueText0,
                                const_cast<char *>("smalfont.fnt"),
                                1,
                                static_cast<short>(
                                    rowWidgetId6 + item + 109
                                ),
                                0x200,
                                1
                            );
                    overWin->AddWidget(
                            reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row][textItemCount],
                            -1
                        );
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
                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] =
                        new iconWidget(
                            static_cast<short>(
                                detailColumn * 35 + 233
                            ),
                            static_cast<short>(
                                row * OVERVIEW_ROW_HEIGHT
                                + detailRow * 42 + 20
                            ),
                            34,
                            34,
                            const_cast<char *>("overview.icn"),
                            12,
                            0,
                            static_cast<short>(rowWidgetId6 + item + 154),
                            16,
                            1
                        );
                    if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount]
                        == 0) {
                        MemError();
                    }
                    overWin->AddWidget(
                        reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                        -1
                    );

                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount + 1] =
                        new iconWidget(
                            static_cast<short>(
                                detailColumn * 35 + 234
                            ),
                            static_cast<short>(
                                row * OVERVIEW_ROW_HEIGHT
                                + detailRow * 42 + 21
                            ),
                            32,
                            32,
                            const_cast<char *>("miniss.icn"),
                            static_cast<short>(detailIndex),
                            0,
                            static_cast<short>(rowWidgetId6 + item + 146),
                            16,
                            1
                        );
                    if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount + 1]
                        == 0) {
                        MemError();
                    }
                    overWin->AddWidget(
                        reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount + 1],
                        -1
                    );
                    iconCount += 2;

                    valueText0 = static_cast<char *>(
                        BaseAlloc(2, OVERVIEW_SOURCE_FILE, OVERVIEW_SETUP_LINE + 0x1fb)
                    );
                    sprintf(valueText0, "%d", static_cast<int>(heroData0->GetSSLevel(detailIndex)));
                    reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row][textItemCount] =
                        new textWidget(
                            static_cast<short>(
                                detailColumn * 35 + 255
                            ),
                            static_cast<short>(
                                row * OVERVIEW_ROW_HEIGHT
                                + detailRow * 42 + 45
                            ),
                            8,
                            8,
                            valueText0,
                            const_cast<char *>("smalfont.fnt"),
                            1,
                            static_cast<short>(rowWidgetId6 + item + 162),
                            0x200,
                            2
                        );
                    overWin->AddWidget(
                        reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row][textItemCount],
                        -1
                    );
                    textItemCount++;
                }
            }

            int displayedArtifacts = 0;
            for (item = 0; item < OVERVIEW_ARTIFACT_SLOTS; item++) {
                if (heroData0->m_artifacts[item] != -1) {
                    detailRow = displayedArtifacts / OVERVIEW_ARTIFACT_COLUMNS;
                    detailColumn = displayedArtifacts % OVERVIEW_ARTIFACT_COLUMNS;
                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount] =
                        new iconWidget(
                            static_cast<short>(
                                detailColumn * 35 + 378
                            ),
                            static_cast<short>(
                                row * OVERVIEW_ROW_HEIGHT
                                + detailRow * 42 + 20
                            ),
                            34,
                            34,
                            const_cast<char *>("overview.icn"),
                            12,
                            0,
                            static_cast<short>(rowWidgetId6 + item + 128),
                            16,
                            1
                        );
                    if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount]
                        == 0) {
                        MemError();
                    }
                    overWin->AddWidget(
                        reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount],
                        -1
                    );

                    reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount + 1] =
                        new iconWidget(
                            static_cast<short>(
                                detailColumn * 35 + 379
                            ),
                            static_cast<short>(
                                row * OVERVIEW_ROW_HEIGHT
                                + detailRow * 42 + 21
                            ),
                            32,
                            32,
                            const_cast<char *>("artfx.icn"),
                            static_cast<short>(heroData0->m_artifacts[item]),
                            0,
                            static_cast<short>(rowWidgetId6 + item + 114),
                            16,
                            1
                        );
                    if (reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount + 1]
                        == 0) {
                        MemError();
                    }
                    overWin->AddWidget(
                        reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row][iconCount + 1],
                        -1
                    );
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

// @match-note: live 96.78%. The 0x50 frame, local-array bytes, CFG, semantics,
// and ordered 52-relocation target sequence agree. In each of the two title
// table arguments, ours loads giOverviewType before title; retail loads title
// first and emits a one-byte-longer address sequence. The body is consequently
// two bytes short, followed by one retail alignment byte (next symbol ours
// +0x2594, retail +0x2598 including the inherited one-byte entry gap). Tried the
// natural short[2][3] access, grouped byte-offset C++ casts (same code), and a
// flat short[6] indexed title-first (96.03%, ten bytes shorter). Revisit with
// audited AST/codegen steering; do not change the proven local storage layout.
VA(0x00409aae, 0x357)
void game::SetupNewOverviewType(int overviewType, int redrawFrom)
{
    int title;
    tag_message message;
    char *titleCopy;

    giOverviewType = overviewType;
    iOverviewItems =
        giOverviewType == OVERVIEW_HEROES ? gpCurPlayer->m_heroCount : gpCurPlayer->m_townCount;

    if (iOverviewItems - OVERVIEW_VISIBLE_ROWS < giOverviewTop[giOverviewType]) {
        giOverviewTop[giOverviewType] = iOverviewItems - OVERVIEW_VISIBLE_ROWS;
    }
    if (giOverviewTop[giOverviewType] < 0) {
        giOverviewTop[giOverviewType] = 0;
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = OVERVIEW_WIDGET_SET_FRAME;
    message.payload.widget.id = OVERVIEW_TITLE_WIDGET;
    message.payload.widget.data.value = giOverviewType + 6;
    overWin->BroadcastMessage(message);
    message.payload.widget.command = OVERVIEW_WIDGET_SET_FRAME;
    message.payload.widget.id = OVERVIEW_HERO_TAB_WIDGET;
    message.payload.widget.data.value = giOverviewType == OVERVIEW_HEROES;
    overWin->BroadcastMessage(message);
    message.payload.widget.command = OVERVIEW_WIDGET_SET_FRAME;
    message.payload.widget.id = OVERVIEW_TOWN_TAB_WIDGET;
    if (giOverviewType == OVERVIEW_TOWNS) {
        message.payload.widget.data.value = 3;
    } else {
        message.payload.widget.data.value = 2;
    }
    overWin->BroadcastMessage(message);

    short titleLefts[2][OVERVIEW_TITLE_COUNT] = {{35, 232, 377}, {35, 177, 379}};
    short titleWidth[2][OVERVIEW_TITLE_COUNT] = {{194, 142, 241}, {140, 199, 239}};

    for (title = 0; title < OVERVIEW_TITLE_COUNT; title++) {
        if (textWidgetTitle[title] != 0) {
            overWin->RemoveWidget(textWidgetTitle[title]);
            delete textWidgetTitle[title];
            textWidgetTitle[title] = 0;
        }
    }
    for (title = 0; title < OVERVIEW_TITLE_COUNT; title++) {
        titleCopy = static_cast<char *>(BaseAlloc(
            strlen(cOverviewText[giOverviewType * OVERVIEW_TITLE_COUNT + title]) + 1,
            OVERVIEW_SOURCE_FILE,
            OVERVIEW_SETUP_LINE + 0x2a
        ));
        strcpy(titleCopy, cOverviewText[giOverviewType * OVERVIEW_TITLE_COUNT + title]);
        textWidgetTitle[title] = new textWidget(
            titleLefts[giOverviewType][title],
            3,
            titleWidth[giOverviewType][title],
            12,
            titleCopy,
            const_cast<char *>("smalfont.fnt"),
            1,
            static_cast<short>(title + 110),
            0x200,
            1
        );
        overWin->AddWidget(textWidgetTitle[title], -1);
    }
    SetupDynamicStuff(0, 1, 0);
    if (redrawFrom != 0) {
        overWin->DrawWindow(redrawFrom, OVERVIEW_HERO_TAB_WIDGET, 999);
    }
}

VA(0x00409e05, 0x84)
void game::SetupResources(void)
{
    int resourceIdx;
    tag_message message;

    message.type = MESSAGE_WIDGET;
    for (resourceIdx = 0; resourceIdx < OVERVIEW_RESOURCE_COUNT; resourceIdx++) {
        message.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
        message.payload.widget.data.text = gText;
        sprintf(gText, "%d", gpCurPlayer->m_resources[resourceIdx]);
        message.payload.widget.id = resourceIdx + OVERVIEW_RESOURCE_FIRST_WIDGET;
        overWin->BroadcastMessage(message);
    }
}

// @match-note: retained/live 99.79%. The 0x44 frame, slots, row-major zeroing
// loop, semantics, CFG, and all 90/90 external relocations agree. The only
// non-relocation instruction residual is in the sawmill loop: ours loads
// sawmills4 and emits `cmp mine4,sawmills4; jle`, while retail loads mine4 and
// emits `cmp sawmills4,mine4; jge`. Tried `sawmills4 < mine4`, the swapped
// `mine4 > sawmills4`, both polarity forms, and an early-continue skip arm
// (99.44%). A bounded AST pass found no audited improvement before stopping.
VA(0x00409e89, 0x4c7)
void game::Overview(void)
{
    char mineCounts4[8];
    int sawmills4;
    int mine4;
    tag_message message8;
    int row0;

    giOverviewReturnAction = OVERVIEW_RETURN_NONE;
    giOverviewReturnActionExtra = OVERVIEW_RETURN_NONE;
    message8.type = MESSAGE_WIDGET;
    gpAdvManager->TrimLoopingSounds(4);
    gpWindowManager->FadeScreen(1, OVERVIEW_FADE_STEPS, 0);
    for (mine4 = 0; mine4 < OVERVIEW_TITLE_COUNT; mine4++) {
        textWidgetTitle[mine4] = 0;
    }

    textWidgetDynamic = static_cast<textWidget **>(BaseAlloc(
        OVERVIEW_DYNAMIC_ARRAY_BYTES,
        OVERVIEW_SOURCE_FILE,
        overviewDialogSourceLine + 0x11
    ));
    iconWidgetDynamic = static_cast<iconWidget **>(BaseAlloc(
        OVERVIEW_DYNAMIC_ARRAY_BYTES,
        OVERVIEW_SOURCE_FILE,
        overviewDialogSourceLine + 0x12
    ));
    for (row0 = 0; row0 < OVERVIEW_VISIBLE_ROWS; row0++) {
        for (mine4 = 0; mine4 < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; mine4++) {
            reinterpret_cast<OverviewTextWidgetRow *>(textWidgetDynamic)[row0][mine4] = 0;
            reinterpret_cast<OverviewIconWidgetRow *>(iconWidgetDynamic)[row0][mine4] = 0;
        }
    }

    iLastDynamicType = -1;
    iLastDynamicTop = -1;
    giOverviewItems[OVERVIEW_HEROES] = gpCurPlayer->m_heroCount;
    giOverviewItems[OVERVIEW_TOWNS] = gpCurPlayer->m_townCount;
    gpMouseManager->SetPointer(const_cast<char *>("advmice.mse"), 0, -999);
    gpResourceManager->GetBackdrop(const_cast<char *>("overback.icn"), gpWindowManager->m_screen, 1);
    overWin = new heroWindow(0, 0, const_cast<char *>("overwind.bin"));
    if (overWin == 0) {
        MemError();
    }
    SetWinText(overWin, 9);
    OVScrollKnob = new iconWidget(
        629,
        18,
        8,
        17,
        const_cast<char *>("scrollcn.icn"),
        4,
        0,
        OVERVIEW_SCROLL_KNOB_WIDGET,
        16,
        1
    );
    if (OVScrollKnob == 0) {
        MemError();
    }
    overWin->AddWidget(OVScrollKnob, -1);

    SetupResources();
    memset(mineCounts4, 0, OVERVIEW_RESOURCE_COUNT);
    sawmills4 = 0;
    for (mine4 = 0; mine4 < OVERVIEW_MINE_COUNT; mine4++) {
        if (m_mineOwners[mine4] == giCurPlayer) {
            if (m_mines[mine4].resourceType <= OVERVIEW_RESOURCE_LAST) {
                mineCounts4[m_mines[mine4].resourceType]++;
            } else if (m_mines[mine4].resourceType == OVERVIEW_SAWMILL_RESOURCE) {
                sawmills4++;
            }
        }
    }
    for (mine4 = 0; mine4 < OVERVIEW_RESOURCE_COUNT; mine4++) {
        message8.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
        message8.payload.widget.data.text = gText;
        sprintf(gText, "%d", static_cast<int>(mineCounts4[mine4]));
        message8.payload.widget.id = mine4 + OVERVIEW_MINE_FIRST_WIDGET;
        overWin->BroadcastMessage(message8);
    }
    for (mine4 = OVERVIEW_VISIBLE_ROWS; mine4 > 0; mine4--) {
        if (sawmills4 < mine4) {
            message8.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message8.payload.widget.id = mine4 + 49;
            message8.payload.widget.data.value = 4;
            overWin->BroadcastMessage(message8);
        }
    }

    SetupResources();
    message8.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
    message8.payload.widget.id = OVERVIEW_DAILY_GOLD_WIDGET;
    message8.payload.widget.data.text = gText;
    sprintf(gText, "%d", ComputeDailyGold(giCurPlayer));
    overWin->BroadcastMessage(message8);
    SetupNewOverviewType(giOverviewType, 0);
    gpWindowManager->DoDialog(overWin, OverviewHandler, 1);
    gpWindowManager->FadeScreen(1, OVERVIEW_FADE_STEPS, 0);
    delete overWin;
    overWin = 0;
    BaseFree(textWidgetDynamic, OVERVIEW_SOURCE_FILE, overviewDialogSourceLine + 0x68);
    BaseFree(iconWidgetDynamic, OVERVIEW_SOURCE_FILE, overviewDialogSourceLine + 0x69);
    textWidgetDynamic = 0;
    iconWidgetDynamic = 0;
}

VA(0x0040a350, 0x31f)
void game::DoKnob(void)
{
    tag_message pendingMessage4;
    float scrollRange13;
    int mouseY8;
    int mouseX9;
    float scrollTop11;
    double pixelsPerItem13;
    int newTop8;
    tag_message message9;
    int previousTop6;

    if (giOverviewItems[giOverviewType] <= OVERVIEW_VISIBLE_ROWS) {
        return;
    }

    {
        scrollRange13 = static_cast<float>(OVERVIEW_SCROLL_RANGE);
        scrollTop11 = static_cast<float>(OVERVIEW_SCROLL_MIN_Y);
        previousTop6 = giOverviewTop[giOverviewType];
        pixelsPerItem13 =
            scrollRange13 / (giOverviewItems[giOverviewType] - (OVERVIEW_VISIBLE_ROWS - 1));
        gpMouseManager->MouseCoords(mouseX9, mouseY8);
        gpInputManager->Flush();
        message9 = gpInputManager->GetEvent();
        while (message9.type != MESSAGE_LEFT_BUTTON_UP
               && message9.type != MESSAGE_RIGHT_BUTTON_UP) {
            if (message9.type == MESSAGE_MOUSE_MOVE) {
                if (static_cast<float>(message9.payload.mouse.y) < scrollTop11) {
                    message9.payload.mouse.y = static_cast<int>(scrollTop11);
                }
                if (scrollRange13 + scrollTop11 - OVERVIEW_SCROLL_LAST_PIXEL_ADJUST
                    < static_cast<float>(message9.payload.mouse.y)) {
                    message9.payload.mouse.y = static_cast<int>(
                        scrollRange13 + scrollTop11 - OVERVIEW_SCROLL_LAST_PIXEL_ADJUST
                    );
                }
                gpMouseManager->Main(message9);
                OVScrollKnob->m_y =
                    static_cast<short>(message9.payload.mouse.y - OVERVIEW_SCROLL_KNOB_OFFSET);
                newTop8 = static_cast<int>((OVScrollKnob->m_y - scrollTop11) / pixelsPerItem13);
                if (newTop8 != previousTop6) {
                    if (newTop8 > giOverviewItems[giOverviewType] - OVERVIEW_VISIBLE_ROWS) {
                        newTop8 = giOverviewItems[giOverviewType] - OVERVIEW_VISIBLE_ROWS;
                    }
                    if (newTop8 < 0) {
                        newTop8 = 0;
                    }
                    giOverviewTop[giOverviewType] = newTop8;
                    OVScrollKnob->m_y =
                        static_cast<short>(message9.payload.mouse.y - OVERVIEW_SCROLL_KNOB_OFFSET);
                    SetupDynamicStuff(1, 0, 0);
                    previousTop6 = newTop8;
                } else {
                    overWin->DrawWindow(1, 0, 0x7fff);
                }
            }
            Process1WindowsMessage();
            message9 = gpInputManager->GetEvent();
            if (message9.type == MESSAGE_MOUSE_MOVE) {
                int discardMouseMoves7 = 1;
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

// @early-stop
// reloc-masked: all declared 0x4fd bytes are identical; all 97 relocation
// positions and external targets agree. Residual is only folded local
// identities/addends for the three mid-function jump/selector tables.
VA(0x0040a66f, 0x4fd)
int OverviewHandler(struct tag_message &message)
{
    int closeDialog5;
    int quickView15;
    int scrollItemCount0;
    int scrollDivisor3;
    int scrollY7;
    int newTop8;

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
                    case OVERVIEW_SCROLL_KNOB_WIDGET:
                        if (quickView15 != 0) {
                            break;
                        }
                        gpGame->DoKnob();
                        break;
                    case OVERVIEW_SCROLL_TRACK_WIDGET:
                        if (quickView15 != 0) {
                            break;
                        }
                        if (giOverviewItems[giOverviewType] <= OVERVIEW_VISIBLE_ROWS) {
                            break;
                        }
                        scrollItemCount0 =
                            giOverviewItems[giOverviewType] - (OVERVIEW_VISIBLE_ROWS - 1);
                        scrollDivisor3 = 0x7d64 / scrollItemCount0;
                        scrollY7 = message.payload.mouse.screenY;
                        scrollY7 -= OVERVIEW_SCROLL_MIN_Y;
                        scrollY7 -= OVERVIEW_SCROLL_KNOB_OFFSET;
                        newTop8 = scrollY7 * OVERVIEW_SCROLL_SCALE / scrollDivisor3;
                        giOverviewTop[giOverviewType] = newTop8;
                        if (giOverviewTop[giOverviewType] + (OVERVIEW_VISIBLE_ROWS - 1)
                            >= giOverviewItems[giOverviewType]) {
                            giOverviewTop[giOverviewType] =
                                giOverviewItems[giOverviewType] - OVERVIEW_VISIBLE_ROWS;
                        }
                        if (giOverviewTop[giOverviewType] < 0) {
                            giOverviewTop[giOverviewType] = 0;
                        }
                        gpGame->SetupDynamicStuff(1, 1, 0);
                        break;
                    case OVERVIEW_HERO_TAB_WIDGET:
                        if (quickView15 != 0) {
                            break;
                        }
                    selectHeroes:
                        if (giOverviewType != OVERVIEW_HEROES) {
                            gpGame->SetupNewOverviewType(OVERVIEW_HEROES, 1);
                        }
                        break;
                    case OVERVIEW_TOWN_TAB_WIDGET:
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
                    case OVERVIEW_HERO_TITLE_WIDGET:
                        goto selectHeroes;
                    case OVERVIEW_TOWN_TITLE_WIDGET:
                        goto selectTowns;
                    case OVERVIEW_SCROLL_UP_WIDGET:
                        if (giOverviewTop[giOverviewType] > 0) {
                            giOverviewTop[giOverviewType]--;
                            gpGame->SetupDynamicStuff(1, 1, 0);
                        }
                        break;
                    case OVERVIEW_SCROLL_DOWN_WIDGET:
                        if (giOverviewTop[giOverviewType]
                            < giOverviewItems[giOverviewType] - OVERVIEW_VISIBLE_ROWS) {
                            giOverviewTop[giOverviewType]++;
                            gpGame->SetupDynamicStuff(1, 1, 0);
                        }
                        break;
                    case OVERVIEW_CLOSE_WIDGET:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        closeDialog5 = 1;
                        break;
                }
                break;
        }
    }

    if (message.type == MESSAGE_KEY_DOWN) {
        switch (message.payload.keyboard.keyCode) {
            case OVERVIEW_KEY_PAGE_UP:
                giOverviewTop[giOverviewType] -= OVERVIEW_VISIBLE_ROWS;
                if (giOverviewTop[giOverviewType] < 0) {
                    giOverviewTop[giOverviewType] = 0;
                }
                gpGame->SetupDynamicStuff(1, 1, 0);
                break;
            case OVERVIEW_KEY_PAGE_DOWN:
                giOverviewTop[giOverviewType] += OVERVIEW_VISIBLE_ROWS;
                if (giOverviewTop[giOverviewType]
                    > giOverviewItems[giOverviewType] - OVERVIEW_VISIBLE_ROWS) {
                    giOverviewTop[giOverviewType] =
                        giOverviewItems[giOverviewType] - OVERVIEW_VISIBLE_ROWS;
                }
                gpGame->SetupDynamicStuff(1, 1, 0);
                break;
            case OVERVIEW_KEY_HOME:
                giOverviewTop[giOverviewType] = 0;
                gpGame->SetupDynamicStuff(1, 1, 0);
                break;
            case OVERVIEW_KEY_END:
                giOverviewTop[giOverviewType] =
                    giOverviewItems[giOverviewType] - OVERVIEW_VISIBLE_ROWS;
                gpGame->SetupDynamicStuff(1, 1, 0);
                break;
        }
    }
    if (closeDialog5 == 1) {
        message.payload.widget.id = OVERVIEW_SCROLL_UP_WIDGET;
        message.payload.widget.command = message.payload.widget.id;
        return OVERVIEW_DIALOG_CLOSE;
    }
    return OVERVIEW_DIALOG_CONTINUE;
}

VA(0x0040ab6c, 0x4fa)
int game::ProcessIconSelect(int widgetId, int quickView)
{
    int row0;
    int selectionIndex2;
    hero *selectedHero13;
    town *selectedTown3;

    if (widgetId >= OVERVIEW_ICON_ROW_BASE && widgetId <= OVERVIEW_ICON_ROW_LIMIT - 1) {
        row0 = (widgetId - OVERVIEW_ICON_ROW_BASE) / OVERVIEW_ROW_ID_STRIDE;
        if (giOverviewTop[giOverviewType] + row0 > giOverviewItems[giOverviewType]) {
            return 0;
        }
        widgetId -= OVERVIEW_ICON_ROW_BASE;
        widgetId %= OVERVIEW_ROW_ID_STRIDE;

        if (giOverviewType == OVERVIEW_HEROES) {
            selectedHero13 = GetHero(
                gpCurPlayer->m_heroIds[giOverviewTop[giOverviewType] + row0]
            );
            if (widgetId >= OVERVIEW_HERO_SELECT_FIRST && widgetId <= OVERVIEW_HERO_SELECT_LAST) {
                giOverviewReturnAction = OVERVIEW_RETURN_HERO;
                giOverviewReturnActionExtra =
                    gpCurPlayer->m_heroIds[giOverviewTop[giOverviewType] + row0];
                return 1;
            }
            if (widgetId >= OVERVIEW_HERO_ARMY_ALT_FIRST
                && widgetId <= OVERVIEW_HERO_ARMY_ALT_LAST) {
                widgetId -= OVERVIEW_TROOP_SLOTS;
            }
            if (widgetId >= OVERVIEW_HERO_ARMY_FIRST && widgetId <= OVERVIEW_HERO_ARMY_LAST) {
                selectionIndex2 = widgetId - OVERVIEW_HERO_ARMY_FIRST;
                gpGame->ViewArmy(
                    119,
                    20,
                    selectedHero13->m_army.m_creatureTypes[selectionIndex2],
                    selectedHero13->m_army.m_creatureCounts[selectionIndex2],
                    0,
                    selectedHero13->m_army.GetNumArmies() == 1,
                    1,
                    quickView,
                    selectedHero13,
                    0,
                    &selectedHero13->m_army,
                    selectionIndex2
                );
                if (quickView == 0) {
                    SetupDynamicStuff(1, 1, 1);
                }
            }
            if (widgetId >= OVERVIEW_HERO_SKILL_FIRST && widgetId <= OVERVIEW_HERO_SKILL_LAST) {
                selectionIndex2 = selectedHero13->GetNthSS(widgetId - OVERVIEW_HERO_SKILL_FIRST);
                selectedHero13->DoSSLevelDialog(selectionIndex2, quickView);
            }
            if (widgetId >= OVERVIEW_HERO_ARTIFACT_FIRST
                && widgetId <= OVERVIEW_HERO_ARTIFACT_LAST) {
                selectionIndex2 =
                    selectedHero13->m_artifacts[widgetId - OVERVIEW_HERO_ARTIFACT_FIRST];
                if (selectionIndex2 == OVERVIEW_MAGIC_BOOK_ARTIFACT) {
                    gpGame->ViewSpells(selectedHero13, 2, ViewSpecialHandler, 1);
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
            if (widgetId >= OVERVIEW_HERO_STAT_FIRST && widgetId <= OVERVIEW_HERO_STAT_LAST) {
                selectionIndex2 = widgetId - OVERVIEW_HERO_STAT_FIRST;
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
            selectedTown3 = GetTown(
                gpCurPlayer->m_townIds[giOverviewTop[giOverviewType] + row0]
            );
            if (widgetId == OVERVIEW_TOWN_SELECT_WIDGET) {
                giOverviewReturnAction = OVERVIEW_RETURN_TOWN;
                giOverviewReturnActionExtra =
                    gpCurPlayer->m_townIds[giOverviewTop[giOverviewType] + row0];
                return 1;
            }
            if (widgetId >= OVERVIEW_TOWN_HERO_FIRST && widgetId <= OVERVIEW_TOWN_HERO_LAST
                && selectedTown3->m_occupyingHeroId != -1) {
                giOverviewReturnAction = OVERVIEW_RETURN_HERO;
                giOverviewReturnActionExtra = selectedTown3->m_occupyingHeroId;
                return 1;
            }
            if (widgetId >= OVERVIEW_TOWN_ARMY_ALT_FIRST
                && widgetId <= OVERVIEW_TOWN_ARMY_ALT_LAST) {
                widgetId -= OVERVIEW_TROOP_SLOTS;
            }
            if (widgetId >= OVERVIEW_TOWN_ARMY_FIRST && widgetId <= OVERVIEW_TOWN_ARMY_LAST) {
                selectionIndex2 = widgetId - OVERVIEW_TOWN_ARMY_FIRST;
                gpGame->ViewArmy(
                    119,
                    20,
                    selectedTown3->m_army.m_creatureTypes[selectionIndex2],
                    selectedTown3->m_army.m_creatureCounts[selectionIndex2],
                    selectedTown3,
                    0,
                    1,
                    quickView,
                    0,
                    0,
                    &selectedTown3->m_army,
                    selectionIndex2
                );
                if (quickView == 0) {
                    SetupDynamicStuff(1, 1, 1);
                }
            }
            if (widgetId >= OVERVIEW_TOWN_RECRUIT_ALT_FIRST
                && widgetId <= OVERVIEW_TOWN_RECRUIT_ALT_LAST) {
                widgetId -= OVERVIEW_DWELLING_SLOTS;
            }
            if (widgetId >= OVERVIEW_TOWN_RECRUIT_FIRST && widgetId <= OVERVIEW_TOWN_RECRUIT_LAST) {
                selectionIndex2 = widgetId - OVERVIEW_TOWN_RECRUIT_FIRST;
                if (quickView != 0) {
                    QuickViewRecruit(selectedTown3, selectionIndex2);
                } else {
                    recruitUnit *recruitDialog = new recruitUnit(selectedTown3, selectionIndex2, 0);
                    if (recruitDialog == 0) {
                        MemError();
                    }
                    gpExec->DoDialog(recruitDialog);
                    delete recruitDialog;
                    gpResourceManager->GetBackdrop(
                        const_cast<char *>("overback.icn"),
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

// ---- globals (definitions, RVA order) ----
DATA(0x004ed9e8) class heroWindow *overWin = 0;
DATA(0x004ed9ec) class textWidget **textWidgetDynamic = 0;
DATA(0x004ed9f0) class iconWidget **iconWidgetDynamic = 0;
DATA(0x004ed9f4) int giOverviewType = 0;
DATA(0x004ed9f8) int giOverviewTop[2] = {0};
DATA(0x004eda00) class iconWidget *OVScrollKnob = 0;
DATA(0x00523258) int iLastDynamicType;
DATA(0x0052325c) int iLastDynamicTop;
DATA(0x00523260) int iOverviewItems;
DATA(0x00523268) int giOverviewItems[2];
DATA(0x00523270) class textWidget *textWidgetTitle[4];
