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
#include <BASE/textWidget.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/game.h>
#include <SOURCE/Overview.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>
#include <stdio.h>
#include <string.h>

#define OVERVIEW_SOURCE_FILE \
    const_cast<char *>("I:\\Projects\\Heroes\\Prog\\SOURCE\\Overview.cpp")
#define OVERVIEW_SETUP_LINE (*reinterpret_cast<const short *>("t"))

VA(0x00407870, 0x223e)
void game::SetupDynamicStuff(int redraw, int updateKnob, int forceUpdate)
{
    int row;
    int item;
    short rowIdBase;
    short nextRowIdBase;

    if (updateKnob) {
        if (giOverviewItems[giOverviewType] <= OVERVIEW_VISIBLE_ROWS) {
            OVScrollKnob->m_y = 169;
        } else {
            float knobScale = 303.0 / (giOverviewItems[giOverviewType] - 4);
            OVScrollKnob->m_y = static_cast<short>(
                giOverviewTop[giOverviewType] * knobScale + 18.0f + 0.4);
        }
    }

    if (!forceUpdate && giOverviewType == iLastDynamicType &&
        giOverviewTop[iLastDynamicType] == iLastDynamicTop) {
        return;
    }

    for (row = 0; row < OVERVIEW_VISIBLE_ROWS; row++) {
        for (item = 0; item < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; item++) {
            if (textWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item] != 0) {
                overWin->RemoveWidget(textWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item]);
                delete textWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item];
                textWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item] = 0;
            }
            if (iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item] != 0) {
                overWin->RemoveWidget(iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item]);
                delete iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item];
                iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + item] = 0;
            }
        }
    }

    for (row = 0; row < OVERVIEW_VISIBLE_ROWS; row++) {
        int textCount = 0;
        int iconCount = 0;
        short rowValue = static_cast<short>(row);
        rowIdBase = static_cast<short>(rowValue * OVERVIEW_ROW_ID_STRIDE);
        nextRowIdBase = static_cast<short>(rowIdBase + OVERVIEW_ROW_ID_STRIDE);

        if (giOverviewTop[giOverviewType] + row >=
            giOverviewItems[giOverviewType]) {
            break;
        }

        iconWidgetDynamic[
            row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
            new iconWidget(30,
                           static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 17),
                           594, 82, const_cast<char *>("overview.icn"),
                           static_cast<short>(giOverviewType + 10), 0,
                           static_cast<short>(rowIdBase + 202), 16, 1);
        if (iconWidgetDynamic[
                row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
            MemError();
        overWin->AddWidget(
            iconWidgetDynamic[
                row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);
        iconCount++;

        if (giOverviewType == OVERVIEW_TOWNS) {
            town *townData = reinterpret_cast<town *>(&m_castleRecs[
                gpCurPlayer->m_townIds[giOverviewTop[giOverviewType] + row]]);
            char *townName = static_cast<char *>(BaseAlloc(
                strlen(townData->m_name) + 1,
                OVERVIEW_SOURCE_FILE, OVERVIEW_SETUP_LINE + 0x4f));
            strcpy(townName, townData->m_name);

            textWidgetDynamic[
                row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + textCount] =
                new textWidget(35,
                               static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 80),
                               132, 10, townName,
                               const_cast<char *>("smalfont.fnt"), 1,
                               static_cast<short>(rowIdBase + 203), 0x200, 1);
            overWin->AddWidget(
                textWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + textCount], -1);
            textCount++;

            short townFrame = static_cast<short>(townData->m_type);
            if ((townData->m_buildings & TOWN_BUILDING_CASTLE) == 0)
                townFrame = static_cast<short>(townData->m_type + 6);
            iconWidgetDynamic[
                row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                new iconWidget(47,
                               static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 36),
                               46, 22, const_cast<char *>("minitown.icn"),
                               townFrame, 0,
                               static_cast<short>(rowIdBase + 204), 16, 1);
            if (iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                MemError();
            overWin->AddWidget(
                iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);
            iconCount++;

            if (BitTest(gpGame->m_knownTowns,
                        static_cast<unsigned int>(townData->m_id))) {
                iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                    new iconWidget(42,
                                   static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 31),
                                   0, 0, const_cast<char *>("locators.icn"), 24, 0,
                                   static_cast<short>(rowIdBase + 244), 16, 1);
                if (iconWidgetDynamic[
                        row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                    MemError();
                overWin->AddWidget(
                    iconWidgetDynamic[
                        row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);
                iconCount++;
            }

            int hasCaptain = 0;
            hero *visitingHero = 0;
            if (townData->m_occupyingHeroId != -1) {
                visitingHero = &m_heroRecs[townData->m_occupyingHeroId];
            } else if ((townData->m_buildings & TOWN_BUILDING_CAPTAIN) != 0) {
                hasCaptain = 1;
            }

            if (visitingHero != 0 || hasCaptain) {
                iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                    new iconWidget(112,
                                   static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 36),
                                   46, 22, const_cast<char *>("portxtra.icn"), 0, 0,
                                   static_cast<short>(rowIdBase + 239), 16, 1);
                if (iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                    MemError();
                overWin->AddWidget(
                    iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);

                int portraitIndex = iconCount + 1;
                short portraitFrame = hasCaptain
                    ? static_cast<short>(townData->m_type)
                    : static_cast<short>(visitingHero->m_id);
                char *portraitFile = hasCaptain
                    ? const_cast<char *>("minicapt.icn")
                    : const_cast<char *>("miniport.icn");
                iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + portraitIndex] =
                    new iconWidget(107,
                                   static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 31),
                                   51, 27, portraitFile, portraitFrame, 0,
                                   static_cast<short>(rowIdBase + 240), 16, 1);
                if (iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + portraitIndex] == 0)
                    MemError();
                overWin->AddWidget(
                    iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + portraitIndex], -1);

                int mobilityIndex = iconCount + 2;
                int mobility = hasCaptain ? 0 : visitingHero->m_remainingMobility;
                iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mobilityIndex] =
                    new iconWidget(112,
                                   static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 36),
                                   0, 0, const_cast<char *>("mobility.icn"),
                                   static_cast<short>(GetMobilityFrame(mobility)), 0,
                                   static_cast<short>(rowIdBase + 243), 16, 1);
                if (iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mobilityIndex] == 0)
                    MemError();
                overWin->AddWidget(
                    iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mobilityIndex], -1);

                int manaIndex = iconCount + 3;
                int captainMana =
                    townData->m_type == TOWN_TYPE_KNIGHT ||
                    townData->m_type == TOWN_TYPE_BARBARIAN ? 10 : 20;
                int mana = hasCaptain ? captainMana : visitingHero->m_spellPoints;
                iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + manaIndex] =
                    new iconWidget(151,
                                   static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 36),
                                   0, 0, const_cast<char *>("mana.icn"),
                                   static_cast<short>(GetManaFrame(mana)), 0,
                                   static_cast<short>(rowIdBase + 241), 16, 1);
                if (iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + manaIndex] == 0)
                    MemError();
                overWin->AddWidget(
                    iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + manaIndex], -1);
                iconCount += 4;
            }

            short displayedTroops = 0;
            for (item = 0; item < OVERVIEW_TROOP_SLOTS; item++) {
                if (townData->m_army.m_creatureTypes[item] != -1 &&
                    townData->m_army.m_creatureCounts[item] > 0) {
                    iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                        new iconWidget(
                            static_cast<short>(displayedTroops * 41 + 173),
                            static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 35),
                            42, 40, const_cast<char *>("mons32.icn"),
                            static_cast<short>(townData->m_army.m_creatureTypes[item]),
                            0, static_cast<short>(nextRowIdBase + item + 5), 17, 1);
                    if (iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                        MemError();
                    overWin->AddWidget(
                        iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);
                    iconCount++;

                    char *troopCount = static_cast<char *>(BaseAlloc(
                        6, OVERVIEW_SOURCE_FILE, OVERVIEW_SETUP_LINE + 0xee));
                    sprintf(troopCount, "%d",
                            static_cast<int>(townData->m_army.m_creatureCounts[item]));
                    textWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + textCount] =
                        new textWidget(
                            static_cast<short>(displayedTroops * 41 + 173),
                            static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 81),
                            40, 12, troopCount, const_cast<char *>("smalfont.fnt"),
                            1, static_cast<short>(nextRowIdBase + item + 10),
                            0x200, 1);
                    overWin->AddWidget(
                        textWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + textCount], -1);
                    textCount++;
                    displayedTroops++;
                }
            }

            short displayedDwellings = 0;
            for (item = 0; item < OVERVIEW_DWELLING_SLOTS; item++) {
                int dwelling;
                switch (item) {
                case 0: dwelling = 0; break;
                case 1: dwelling = 1; break;
                case 2: dwelling = 6; break;
                case 3: dwelling = 2; break;
                case 4: dwelling = 7; break;
                case 5: dwelling = 3; break;
                case 6: dwelling = 8; break;
                case 7: dwelling = 4; break;
                case 8: dwelling = 9; break;
                case 9: dwelling = 5; break;
                case 10: dwelling = 10; break;
                default: dwelling = 11; break;
                }

                if ((townData->m_buildings & (1 << (dwelling + 19))) != 0) {
                    iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                        new iconWidget(
                            static_cast<short>(displayedDwellings * 41 + 378),
                            static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 35),
                            42, 40, const_cast<char *>("mons32.icn"),
                            static_cast<short>(
                                gDwellingType[static_cast<signed char>(townData->m_type)][dwelling]),
                            0, static_cast<short>(nextRowIdBase + dwelling + 15),
                            17, 1);
                    if (iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                        MemError();
                    overWin->AddWidget(
                        iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);
                    iconCount++;

                    char *dwellingCount = static_cast<char *>(BaseAlloc(
                        6, OVERVIEW_SOURCE_FILE, OVERVIEW_SETUP_LINE + 0x12a));
                    sprintf(dwellingCount, "%d",
                            static_cast<int>(townData->m_garrison[dwelling]));
                    textWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + textCount] =
                        new textWidget(
                            static_cast<short>(displayedDwellings * 41 + 378),
                            static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 81),
                            40, 12, dwellingCount,
                            const_cast<char *>("smalfont.fnt"), 1,
                            static_cast<short>(nextRowIdBase + dwelling + 27),
                            0x200, 1);
                    overWin->AddWidget(
                        textWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + textCount], -1);
                    textCount++;
                    displayedDwellings++;
                }
            }
        } else {
            signed char heroId = gpCurPlayer->m_heroIds[
                giOverviewTop[giOverviewType] + row];
            hero *heroData = &m_heroRecs[heroId];

            iconWidgetDynamic[
                row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                new iconWidget(35,
                               static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 21),
                               46, 22, const_cast<char *>("portxtra.icn"), 0, 0,
                               static_cast<short>(rowIdBase + 300), 16, 1);
            if (iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                MemError();
            overWin->AddWidget(
                iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);
            iconCount++;

            iconWidgetDynamic[
                row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                new iconWidget(30,
                               static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 16),
                               51, 27, const_cast<char *>("miniport.icn"),
                               static_cast<short>(heroData->m_id), 0,
                               static_cast<short>(rowIdBase + 301), 16, 1);
            if (iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                MemError();
            overWin->AddWidget(
                iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);
            iconCount++;

            iconWidgetDynamic[
                row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                new iconWidget(35,
                               static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 21),
                               0, 0, const_cast<char *>("mobility.icn"),
                               static_cast<short>(GetMobilityFrame(
                                   heroData->m_remainingMobility)),
                               0, static_cast<short>(rowIdBase + 303), 16, 1);
            if (iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                MemError();
            overWin->AddWidget(
                iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);
            iconCount++;

            iconWidgetDynamic[
                row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                new iconWidget(74,
                               static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 21),
                               0, 0, const_cast<char *>("mana.icn"),
                               static_cast<short>(GetManaFrame(heroData->m_spellPoints)),
                               0, static_cast<short>(rowIdBase + 241), 16, 1);
            if (iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                MemError();
            overWin->AddWidget(
                iconWidgetDynamic[
                    row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);
            iconCount++;

            for (item = 0; item < 4; item++) {
                char *statText = static_cast<char *>(BaseAlloc(
                    4, OVERVIEW_SOURCE_FILE, OVERVIEW_SETUP_LINE + 0x182));
                sprintf(statText, "%d", static_cast<int>(heroData->Stats(item)));
                textWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + textCount] =
                    new textWidget(
                        static_cast<short>(item * 35 + 105),
                        static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 39),
                        14, 8, statText, const_cast<char *>("smalfont.fnt"), 1,
                        static_cast<short>(nextRowIdBase + item + 142),
                        0x200, 2);
                overWin->AddWidget(
                    textWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + textCount], -1);
                textCount++;
            }

            for (item = 0; item < 4; item++) {
                iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                    new iconWidget(
                        static_cast<short>(item * 35 + 87),
                        static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 17),
                        35, 30, const_cast<char *>("overview.icn"), 15, 0,
                        static_cast<short>(nextRowIdBase + item + 170), 16, 1);
                if (iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                    MemError();
                overWin->AddWidget(
                    iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);
                iconCount++;
            }

            short displayedTroops = 0;
            for (item = 0; item < OVERVIEW_TROOP_SLOTS; item++) {
                if (heroData->m_army.m_creatureTypes[item] != -1 &&
                    heroData->m_army.m_creatureCounts[item] > 0) {
                    iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                        new iconWidget(
                            static_cast<short>(displayedTroops * 40 + 30),
                            static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 47),
                            42, 40, const_cast<char *>("mons32.icn"),
                            static_cast<short>(heroData->m_army.m_creatureTypes[item]),
                            0, static_cast<short>(nextRowIdBase + item + 104), 17, 1);
                    if (iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                        MemError();
                    overWin->AddWidget(
                        iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);
                    iconCount++;

                    char *troopCount = static_cast<char *>(BaseAlloc(
                        6, OVERVIEW_SOURCE_FILE, OVERVIEW_SETUP_LINE + 0x1bd));
                    sprintf(troopCount, "%d",
                            static_cast<int>(heroData->m_army.m_creatureCounts[item]));
                    textWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + textCount] =
                        new textWidget(
                            static_cast<short>(displayedTroops * 40 + 30),
                            static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + 87),
                            40, 12, troopCount, const_cast<char *>("smalfont.fnt"),
                            1, static_cast<short>(nextRowIdBase + item + 109),
                            0x200, 1);
                    overWin->AddWidget(
                        textWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + textCount], -1);
                    textCount++;
                    displayedTroops++;
                }
            }

            for (item = 0; item < OVERVIEW_SECONDARY_SKILL_SLOTS; item++) {
                int skill = heroData->GetNthSS(item);
                if (skill != -1) {
                    short column = static_cast<short>(item % OVERVIEW_SECONDARY_SKILL_COLUMNS);
                    short skillRow = static_cast<short>(item / OVERVIEW_SECONDARY_SKILL_COLUMNS);
                    iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                        new iconWidget(
                            static_cast<short>(column * 35 + 233),
                            static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + skillRow * 42 + 20),
                            34, 34, const_cast<char *>("overview.icn"), 12, 0,
                            static_cast<short>(nextRowIdBase + item + 154), 16, 1);
                    if (iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                        MemError();
                    overWin->AddWidget(
                        iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);

                    int skillIconIndex = iconCount + 1;
                    iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + skillIconIndex] =
                        new iconWidget(
                            static_cast<short>(column * 35 + 234),
                            static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + skillRow * 42 + 21),
                            32, 32, const_cast<char *>("miniss.icn"),
                            static_cast<short>(skill), 0,
                            static_cast<short>(nextRowIdBase + item + 146), 16, 1);
                    if (iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + skillIconIndex] == 0)
                        MemError();
                    overWin->AddWidget(
                        iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + skillIconIndex], -1);
                    iconCount += 2;

                    char *skillLevel = static_cast<char *>(BaseAlloc(
                        2, OVERVIEW_SOURCE_FILE, OVERVIEW_SETUP_LINE + 0x1fb));
                    sprintf(skillLevel, "%d",
                            static_cast<int>(heroData->GetSSLevel(skill)));
                    textWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + textCount] =
                        new textWidget(
                            static_cast<short>(column * 35 + 255),
                            static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + skillRow * 42 + 45),
                            8, 8, skillLevel, const_cast<char *>("smalfont.fnt"),
                            1, static_cast<short>(nextRowIdBase + item + 162),
                            0x200, 2);
                    overWin->AddWidget(
                        textWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + textCount], -1);
                    textCount++;
                }
            }

            int displayedArtifacts = 0;
            for (item = 0; item < OVERVIEW_ARTIFACT_SLOTS; item++) {
                if (heroData->m_artifacts[item] != -1) {
                    short column = static_cast<short>(
                        displayedArtifacts % OVERVIEW_ARTIFACT_COLUMNS);
                    short artifactRow = static_cast<short>(
                        displayedArtifacts / OVERVIEW_ARTIFACT_COLUMNS);
                    iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] =
                        new iconWidget(
                            static_cast<short>(column * 35 + 378),
                            static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + artifactRow * 42 + 20),
                            34, 34, const_cast<char *>("overview.icn"), 12, 0,
                            static_cast<short>(nextRowIdBase + item + 128), 16, 1);
                    if (iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount] == 0)
                        MemError();
                    overWin->AddWidget(
                        iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + iconCount], -1);

                    int artifactIconIndex = iconCount + 1;
                    iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + artifactIconIndex] =
                        new iconWidget(
                            static_cast<short>(column * 35 + 379),
                            static_cast<short>(rowValue * OVERVIEW_ROW_HEIGHT + artifactRow * 42 + 21),
                            32, 32, const_cast<char *>("artfx.icn"),
                            static_cast<short>(heroData->m_artifacts[item]), 0,
                            static_cast<short>(nextRowIdBase + item + 114), 16, 1);
                    if (iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + artifactIconIndex] == 0)
                        MemError();
                    overWin->AddWidget(
                        iconWidgetDynamic[row * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + artifactIconIndex], -1);
                    iconCount += 2;
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
void game::SetupNewOverviewType(int, int) {}

VA(0x00409e05, 0x84)
void game::SetupResources(void) {}

VA(0x00409e89, 0x4c7)
void game::Overview(void) {}

VA(0x0040a350, 0x31f)
void game::DoKnob(void) {}

VA(0x0040a66f, 0x4fd)
int OverviewHandler(struct tag_message &) { return 0; }

VA(0x0040ab6c, 0x4fa)
int game::ProcessIconSelect(int, int) { return 0; }

// ---- globals (definitions, RVA order) ----
DATA(0x004ed9e8) class heroWindow *overWin;
DATA(0x004ed9ec) class textWidget **textWidgetDynamic;
DATA(0x004ed9f0) class iconWidget **iconWidgetDynamic;
DATA(0x004ed9f4) int giOverviewType;
DATA(0x004ed9f8) int giOverviewTop[2];
DATA(0x004eda00) class iconWidget *OVScrollKnob;
DATA(0x00523258) int iLastDynamicType;
DATA(0x0052325c) int iLastDynamicTop;
DATA(0x00523260) int iOverviewItems;
DATA(0x00523268) int giOverviewItems[2];
DATA(0x00523270) class textWidget *textWidgetTitle[4];
