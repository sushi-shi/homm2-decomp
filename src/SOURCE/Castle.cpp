// Reconstructed from CodeView NB09 of HEROES2W.EXE - NOT original source.
// compiland: .\Win32_Re\Castle.obj   from: (directly linked into exe)
// functions: 2   data: 3
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/message.h>
#include <BASE/BITS.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <EDITOR/fullMap.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/Castle.h>
#include <SOURCE/HERO.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/TOWNMGR.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/bankBox.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>
#include <SOURCE/townManager.h>
#include <SOURCE/townObject.h>
#include <stdio.h>
#include <string.h>

VA(0x0041d040, 0x10bb)
void townManager::SetupCastle(heroWindow *window, i32 updateOnly)
{
    widget *backgroundWidget19;
    i32 backgroundFrame6;
    i32 slot7;
    i32 row9;
    i32 column8;
    i32 widgetFrame12;
    i32 terrainIconFrame27;
    i32 raceIconFrame;
    u32l captainBuilt;
    char iconName4[16];
    char statLine11[200];
    tag_message message3;

    casWin = window;
    i16 builtFrame7 = CASTLE_FRAME_BUILT;
    i16 cannotBuildFrame17 = CASTLE_FRAME_CANNOT_BUILD;
    i16 cannotAffordFrame6 = CASTLE_FRAME_CANNOT_AFFORD;
    for (slot7 = 0; slot7 < CASTLE_SLOT_COUNT; ++slot7) {
        castleSlotsUse[slot7] = castleSlotsBase[slot7];
        if (castleSlotsBase[slot7] >= CASTLE_FIRST_DWELLING + 1 &&
            castleSlotsBase[slot7] <= CASTLE_LAST_DWELLING &&
            ((m_town->m_buildings & (1L << castleSlotsBase[slot7])) ||
             (m_town->m_buildings &
              (1L << (castleSlotsBase[slot7] + CASTLE_UPGRADE_OFFSET))) ||
             (castleSlotsBase[slot7] == CASTLE_WARLOCK_TOP_DWELLING &&
              m_town->m_type == FACTION_WARLOCK &&
              (m_town->m_buildings &
               TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6))) &&
            (gTownEligibleBuildMask[m_town->m_type] &
             (1L << (castleSlotsBase[slot7] + CASTLE_UPGRADE_OFFSET)))) {
            if (castleSlotsBase[slot7] == CASTLE_WARLOCK_TOP_DWELLING &&
                m_town->m_type == FACTION_WARLOCK &&
                ((m_town->m_buildings & TOWN_BUILDING_UPGRADED_DWELLING_6) ||
                 (m_town->m_buildings &
                  TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6))) {
                castleSlotsUse[slot7] = CASTLE_WARLOCK_TOP_UPGRADE;
            } else {
                castleSlotsUse[slot7] =
                    castleSlotsBase[slot7] + CASTLE_UPGRADE_OFFSET;
            }
        }
    }

    m_buildableBuildings = 0;
    m_affordableBuildings = m_buildableBuildings;
    for (slot7 = 0; slot7 < CASTLE_SLOT_COUNT; ++slot7) {
        if (CanBuy(m_town, castleSlotsUse[slot7]))
            m_affordableBuildings |= 1L << castleSlotsUse[slot7];
        if (CanBuild(m_town, castleSlotsUse[slot7]))
            m_buildableBuildings |= 1L << castleSlotsUse[slot7];
    }

    message3.type = CASTLE_MESSAGE_WIDGET;
    message3.payload.widget.command = CASTLE_WIDGET_FRAME;
    for (slot7 = 0; slot7 < CASTLE_SLOT_COUNT; ++slot7) {
        message3.payload.widget.id = CASTLE_CONTROL_BUILDING_ICON_FIRST + slot7;
        message3.payload.widget.data.value = castleSlotsUse[slot7];
        casWin->BroadcastMessage(message3);
    }

    message3.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
    sprintf(iconName4, "cstl%s.icn", cHeroTypeShortName[m_town->m_type]);
    message3.payload.widget.data.text = iconName4;
    for (slot7 = 0; slot7 < CASTLE_SLOT_COUNT; ++slot7) {
        message3.payload.widget.id = CASTLE_CONTROL_BUILDING_ICON_FIRST + slot7;
        casWin->BroadcastMessage(message3);
    }

    message3.payload.widget.command = CASTLE_WIDGET_TEXT;
    for (slot7 = 0; slot7 < CASTLE_SLOT_COUNT; ++slot7) {
        message3.payload.widget.id = CASTLE_CONTROL_BUILDING_NAME_FIRST + slot7;
        if (castleSlotsUse[slot7] == CASTLE_MAGE_GUILD) {
            sprintf(gText, "Mage Guild, Level %d",
                    m_town->m_buildState + 1 < CASTLE_MAGE_GUILD_MAX_LEVEL
                        ? m_town->m_buildState + 1
                        : CASTLE_MAGE_GUILD_MAX_LEVEL);
            message3.payload.widget.data.text = gText;
        } else {
            message3.payload.widget.data.text = GetBuildingName(m_town->m_type, castleSlotsUse[slot7]);
        }
        casWin->BroadcastMessage(message3);
    }

    for (slot7 = 0; slot7 < CASTLE_SLOT_COUNT; ++slot7) {
        widgetFrame12 = -1;
        if ((m_town->m_buildings & (1L << castleSlotsUse[slot7])) &&
            (castleSlotsUse[slot7] != CASTLE_MAGE_GUILD ||
             m_town->m_buildState == CASTLE_MAGE_GUILD_MAX_LEVEL)) {
            widgetFrame12 = CASTLE_FRAME_BUILT;
        } else {
            if (!(m_buildableBuildings & (1L << castleSlotsUse[slot7])))
                widgetFrame12 = CASTLE_FRAME_CANNOT_BUILD;
            else if (!(m_affordableBuildings & (1L << castleSlotsUse[slot7])))
                widgetFrame12 = CASTLE_FRAME_CANNOT_AFFORD;
        }

        if (widgetFrame12 != -1) {
            message3.payload.widget.command = CASTLE_WIDGET_ENABLE;
            message3.payload.widget.id = CASTLE_CONTROL_BUILDING_BUTTON_FIRST + slot7;
            message3.payload.widget.data.value = 4;
            casWin->BroadcastMessage(message3);
            message3.payload.widget.command = CASTLE_WIDGET_FRAME;
            message3.payload.widget.data.value = widgetFrame12;
            casWin->BroadcastMessage(message3);
        } else {
            message3.payload.widget.command = CASTLE_WIDGET_DISABLE;
            message3.payload.widget.data.value = 4;
            message3.payload.widget.id = CASTLE_CONTROL_BUILDING_BUTTON_FIRST + slot7;
            casWin->BroadcastMessage(message3);
        }

        if (widgetFrame12 == CASTLE_FRAME_BUILT) {
            message3.payload.widget.command = CASTLE_WIDGET_DISABLE;
            message3.payload.widget.data.value = 4;
            message3.payload.widget.id = CASTLE_CONTROL_BUILDING_OVERLAY_FIRST + slot7;
            casWin->BroadcastMessage(message3);
        } else {
            message3.payload.widget.command = CASTLE_WIDGET_ENABLE;
            message3.payload.widget.data.value = 4;
            message3.payload.widget.id = CASTLE_CONTROL_BUILDING_OVERLAY_FIRST + slot7;
            casWin->BroadcastMessage(message3);
            message3.payload.widget.command = CASTLE_WIDGET_FRAME;
            message3.payload.widget.data.value =
                widgetFrame12 == -1 ? CASTLE_FRAME_AVAILABLE : CASTLE_FRAME_UNAVAILABLE;
            casWin->BroadcastMessage(message3);
        }
    }

    captainBuilt = m_town->m_buildings & CASTLE_CAPTAIN_BUILDING_MASK;
    message3.payload.widget.command = captainBuilt != 0 ? CASTLE_WIDGET_DISABLE : CASTLE_WIDGET_ENABLE;
    message3.payload.widget.id = CASTLE_CONTROL_CAPTAIN_OVERLAY;
    message3.payload.widget.data.value = 6;
    casWin->BroadcastMessage(message3);
    message3.payload.widget.command = CASTLE_WIDGET_FRAME;
    message3.payload.widget.id = CASTLE_CONTROL_CAPTAIN_ICON;
    message3.payload.widget.data.value = captainBuilt != 0;
    casWin->BroadcastMessage(message3);
    sprintf(gText, "CSTLCAP%c.ICN", cHeroTypeInitial[m_town->m_type]);
    message3.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
    message3.payload.widget.id = CASTLE_CONTROL_CAPTAIN_ICON;
    message3.payload.widget.data.text = gText;
    casWin->BroadcastMessage(message3);
    message3.payload.widget.command = captainBuilt != 0 ? CASTLE_WIDGET_ENABLE : CASTLE_WIDGET_DISABLE;
    message3.payload.widget.id = CASTLE_CONTROL_CAPTAIN_FLAG;
    message3.payload.widget.data.value = 4;
    casWin->BroadcastMessage(message3);
    if (captainBuilt != 0) {
        message3.payload.widget.command = CASTLE_WIDGET_FRAME;
        message3.payload.widget.id = CASTLE_CONTROL_CAPTAIN_FLAG;
        message3.payload.widget.data.value = gpCurPlayer->m_color;
        casWin->BroadcastMessage(message3);
    }

    widgetFrame12 = -1;
    if (captainBuilt != 0) {
        message3.payload.widget.command = CASTLE_WIDGET_TEXT;
        message3.payload.widget.data.text = gText;
        sprintf(gText, "");
        for (slot7 = 0; slot7 < CASTLE_PRIMARY_STAT_COUNT; ++slot7) {
            sprintf(statLine11, "%s\n", gStatNames[slot7]);
            strcat(gText, statLine11);
        }
        message3.payload.widget.id = CASTLE_CONTROL_CAPTAIN_STATS;
        casWin->BroadcastMessage(message3);
        sprintf(gText, "");
        for (slot7 = 0; slot7 < CASTLE_PRIMARY_STAT_COUNT; ++slot7) {
            sprintf(statLine11, "%d\n", captainStats[m_town->m_type][slot7]);
            strcat(gText, statLine11);
        }
        message3.payload.widget.id = CASTLE_CONTROL_CAPTAIN_VALUES;
        casWin->BroadcastMessage(message3);
        message3.payload.widget.command = m_town->m_formation != TOWN_FORMATION_SPREAD
                             ? CASTLE_WIDGET_DISABLE
                             : CASTLE_WIDGET_ENABLE;
        message3.payload.widget.id = CASTLE_CONTROL_CAPTAIN_FORMATION_SPREAD_INACTIVE;
        message3.payload.widget.data.value = 4;
        casWin->BroadcastMessage(message3);
        message3.payload.widget.command = m_town->m_formation == TOWN_FORMATION_SPREAD
                             ? CASTLE_WIDGET_DISABLE
                             : CASTLE_WIDGET_ENABLE;
        message3.payload.widget.id = CASTLE_CONTROL_CAPTAIN_FORMATION_GROUPED_INACTIVE;
        message3.payload.widget.data.value = 4;
        casWin->BroadcastMessage(message3);
    } else {
        if (!CanBuild(m_town, CASTLE_CAPTAIN))
            widgetFrame12 = CASTLE_FRAME_CANNOT_BUILD;
        else if (!CanBuy(m_town, CASTLE_CAPTAIN))
            widgetFrame12 = CASTLE_FRAME_CANNOT_AFFORD;
        if (CanBuild(m_town, CASTLE_CAPTAIN))
            m_buildableBuildings |= CASTLE_CAPTAIN_BUILDING_MASK;
        if (CanBuy(m_town, CASTLE_CAPTAIN))
            m_affordableBuildings |= CASTLE_CAPTAIN_BUILDING_MASK;
    }

    message3.payload.widget.command = widgetFrame12 == -1 ? CASTLE_WIDGET_DISABLE : CASTLE_WIDGET_ENABLE;
    message3.payload.widget.id = CASTLE_CONTROL_CAPTAIN_BUTTON;
    message3.payload.widget.data.value = 4;
    casWin->BroadcastMessage(message3);
    if (widgetFrame12 != -1) {
        message3.payload.widget.command = CASTLE_WIDGET_FRAME;
        message3.payload.widget.data.value = widgetFrame12;
        casWin->BroadcastMessage(message3);
    }

    if (gpCurPlayer->m_resources[RES_GOLD] < gHeroGoldCost)
        widgetFrame12 = CASTLE_FRAME_CANNOT_AFFORD;
    else if (gpCurPlayer->m_heroCount == CASTLE_HERO_COUNT_LIMIT ||
             m_town->m_occupyingHeroId != -1)
        widgetFrame12 = CASTLE_FRAME_CANNOT_BUILD;
    else if (m_recruitResult != 0)
        widgetFrame12 = CASTLE_FRAME_BUILT;
    else
        widgetFrame12 = -1;

    for (slot7 = 0; slot7 < 2; ++slot7) {
        message3.payload.widget.data.value = 4;
        message3.payload.widget.id = CASTLE_CONTROL_RECRUIT_FIRST + slot7;
        if (widgetFrame12 != -1) {
            message3.payload.widget.command = CASTLE_WIDGET_ENABLE;
            casWin->BroadcastMessage(message3);
            message3.payload.widget.command = CASTLE_WIDGET_FRAME;
            message3.payload.widget.data.value = widgetFrame12;
            casWin->BroadcastMessage(message3);
        } else {
            message3.payload.widget.command = CASTLE_WIDGET_DISABLE;
            casWin->BroadcastMessage(message3);
        }
        message3.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
        sprintf(iconName4, "port%04d.icn",
                gpGame->m_heroRecs[
                    gpCurPlayer->AvailableHeroId(slot7)].m_portrait);
        message3.payload.widget.data.text = iconName4;
        message3.payload.widget.id = CASTLE_CONTROL_HERO_FIRST + slot7;
        casWin->BroadcastMessage(message3);
    }

    i32 backgroundLeft4 = CASTLE_BACKGROUND_LEFT;
    i32 backgroundTop12 = CASTLE_BACKGROUND_TOP;
    terrainIconFrame27 =
        (giGroundToTerrain[
             gpGame->m_worldMap.GetCell(m_town->m_x, m_town->m_y)->m_terrainImageIndex] - 1) *
        CASTLE_TERRAIN_ICON_COLUMNS * CASTLE_TERRAIN_ICON_FRAMES;
    raceIconFrame = m_town->m_type << 5;
    if (updateOnly == 0) {
        backgroundFrame6 = 0;
        for (row9 = 5; row9 <= 6; ++row9) {
            for (column8 = 4; column8 <= 8; ++column8) {
                backgroundWidget19 = new iconWidget(
                    static_cast<i16>((column8 - 4) * 32 + CASTLE_BACKGROUND_LEFT),
                    static_cast<i16>((row9 - 2) * 32), 32, 32,
                    "objntwba.icn",
                    static_cast<i16>(0[&terrainIconFrame27] + backgroundFrame6),
                    0, -1, 16, 1);
                if (backgroundWidget19 == 0)
                    MemError();
                casWin->AddWidget(backgroundWidget19, -1);
                ++backgroundFrame6;
            }
        }
        backgroundFrame6 = 0;
        for (row9 = 2; row9 <= 5; ++row9) {
            for (column8 = 4; column8 <= 8; ++column8) {
                if (row9 == 2 && column8 != 6) {
                    continue;
                }
                backgroundWidget19 = new iconWidget(
                    static_cast<i16>((column8 - 4) * 32 + CASTLE_BACKGROUND_LEFT),
                    static_cast<i16>((row9 - 2) * 32), 32, 32,
                    "objntown.icn",
                    static_cast<i16>(0[&raceIconFrame] + backgroundFrame6),
                    0, -1, 16, 1);
                if (backgroundWidget19 == 0)
                    MemError();
                casWin->AddWidget(backgroundWidget19, -1);
                ++backgroundFrame6;
            }
        }
        if (xIsExpansionMap == 0 && m_town->m_type == FACTION_NECROMANCER) {
            backgroundWidget19 = new iconWidget(149, 157, 137, 72,
                                              "caslxtra.icn", 0, 0, -1,
                                              16, 1);
            if (backgroundWidget19 == 0)
                MemError();
            casWin->AddWidget(backgroundWidget19, -1);
        }
    }
}

// @semantic: Semantics/CFG complete; frame 0x38 and all named/compiler slots match.
// First divergence at +0x16e: retail loads gpTownManager before message.id for the
// m_lastHoverId comparison; swapping operands and a gpTownManager pointer barrier
// emit the same base order. All 165 external relocation identities agree. Retail
// jump tables are +5c9/10, +60a/24, +aef/10, +b2b/24; base tables are +5c7/10,
// +603/24, +af4/10, +b30/24. Reordering Grouped before Spread aligns the second
// table's case-body identities; all four tables now preserve the same normalized
// case multiplicities/destinations under accumulated code-size shifts. Early-goto
// command CFG was also tried and regressed layout. Revisit after a material Castle
// predecessor/header or comparison-tool change.
VA(0x0041e0fb, 0xca3)
i32 CastleHandler(tag_message &message)
{
    i32 result;
    i16 textControl;
    i32 quickFlag;
    i32 buildingIndex;
    i32 loopIndex;
    i32 cannotRecruitHero;
    i32 hoverMessage;
    i32 heroChoiceIndex;

    textControl = CASTLE_CONTROL_STATUS_TEXT;
    buildingIndex = -1;
    result = 0;
    hoverMessage = 0;

    if (message.type == CASTLE_MESSAGE_HOVER ||
        message.type == CASTLE_MESSAGE_WIDGET) {
        if (message.type == CASTLE_MESSAGE_HOVER) {
            gpWindowManager->ConvertToHover(message);
            hoverMessage = 1;
        }
        if (message.payload.widget.id == CASTLE_CONTROL_CAPTAIN_ICON)
            buildingIndex = CASTLE_CAPTAIN;
        else if (message.payload.widget.id == CASTLE_CONTROL_CAPTAIN_FORMATION_GROUPED)
            buildingIndex = CASTLE_CONTROL_CAPTAIN_FORMATION_GROUPED;
        else if (message.payload.widget.id == CASTLE_CONTROL_CAPTAIN_FORMATION_SPREAD)
            buildingIndex = CASTLE_CONTROL_CAPTAIN_FORMATION_SPREAD;
        else {
            if (message.payload.widget.id >= CASTLE_CONTROL_BUILDING_NAME_FIRST &&
                message.payload.widget.id < CASTLE_CONTROL_BUILDING_NAME_FIRST +
                                     CASTLE_SLOT_COUNT)
                buildingIndex = message.payload.widget.id - CASTLE_CONTROL_BUILDING_NAME_FIRST;
            else if (message.payload.widget.id >= CASTLE_CONTROL_BUILDING_ICON_FIRST &&
                     message.payload.widget.id < CASTLE_CONTROL_BUILDING_ICON_FIRST +
                                          CASTLE_SLOT_COUNT)
                buildingIndex = message.payload.widget.id - CASTLE_CONTROL_BUILDING_ICON_FIRST;
            else if (message.payload.widget.id >= CASTLE_CONTROL_BUILDING_BUTTON_FIRST &&
                     message.payload.widget.id < CASTLE_CONTROL_BUILDING_BUTTON_FIRST +
                                          CASTLE_SLOT_COUNT)
                buildingIndex = message.payload.widget.id - CASTLE_CONTROL_BUILDING_BUTTON_FIRST;
            if (buildingIndex != -1)
                buildingIndex = castleSlotsUse[buildingIndex];
        }
    }

    if (hoverMessage) {
        if (message.payload.widget.id == gpTownManager->m_lastHoverId)
            return 1;
        gpTownManager->m_lastHoverId = message.payload.widget.id;
        switch (buildingIndex) {
        case CASTLE_CONTROL_CAPTAIN_FORMATION_GROUPED:
            sprintf(gText, cCastleInfo[CASTLE_INFO_GROUPED_FORMATION]);
            break;
        case CASTLE_CONTROL_CAPTAIN_FORMATION_SPREAD:
            sprintf(gText, cCastleInfo[CASTLE_INFO_SPREAD_FORMATION]);
            break;

        case CASTLE_MAGE_GUILD:
            if (!(gpTownManager->m_buildableBuildings & (1L << buildingIndex))) {
                sprintf(gText, cCastleInfo[CASTLE_INFO_CANNOT_BUILD],
                        GetBuildingName(gpTownManager->m_town->m_type, buildingIndex));
            } else if (!(gpTownManager->m_affordableBuildings & (1L << buildingIndex))) {
                sprintf(gText, cCastleInfo[CASTLE_INFO_CANNOT_AFFORD],
                        GetBuildingName(gpTownManager->m_town->m_type, buildingIndex));
            } else {
                if (!(gpTownManager->m_town->m_buildings & 1L))
                    loopIndex = 0;
                else if (gpTownManager->m_town->m_buildState ==
                         CASTLE_MAGE_GUILD_MAX_LEVEL)
                    loopIndex = 1;
                else if (!CanBuy(gpTownManager->m_town, CASTLE_MAGE_GUILD))
                    loopIndex = 2;
                else
                    loopIndex = 3;
                strcpy(gText, cCastleInfo[loopIndex]);
            }
            break;

        case 1:
        case 2:
        case 3:
        case 4:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 15:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
            if (BitTest(gpGame->m_dailyEventFlags,
                        gpTownManager->m_town->m_id)) {
                sprintf(gText, "Cannot build.  Already built here this turn.");
            } else if (gpTownManager->m_town->m_buildings &
                       (1L << buildingIndex)) {
                sprintf(gText, cCastleInfo[CASTLE_INFO_ALREADY_BUILT],
                        GetBuildingName(gpTownManager->m_town->m_type,
                                        buildingIndex));
            } else {
                if (!(gpTownManager->m_buildableBuildings & (1L << buildingIndex)))
                    sprintf(gText, cCastleInfo[CASTLE_INFO_CANNOT_BUILD],
                            GetBuildingName(gpTownManager->m_town->m_type,
                                            buildingIndex));
                else if (!(gpTownManager->m_affordableBuildings &
                           (1L << buildingIndex)))
                    sprintf(gText, cCastleInfo[CASTLE_INFO_CANNOT_AFFORD],
                            GetBuildingName(gpTownManager->m_town->m_type,
                                            buildingIndex));
                else
                    sprintf(gText, cCastleInfo[CASTLE_INFO_BUILD],
                            GetBuildingName(gpTownManager->m_town->m_type,
                                            buildingIndex));
            }
            break;

        case -1:
            switch (message.payload.widget.id) {
            case CASTLE_CONTROL_HERO_FIRST:
            case CASTLE_CONTROL_HERO_FIRST + 1:
                heroChoiceIndex = message.payload.widget.id - CASTLE_CONTROL_HERO_FIRST;
                goto hero_hover_text;
            case CASTLE_CONTROL_RECRUIT_FIRST:
            case CASTLE_CONTROL_RECRUIT_FIRST + 1:
                heroChoiceIndex = message.payload.widget.id - CASTLE_CONTROL_RECRUIT_FIRST;
hero_hover_text:
                if (gpCurPlayer->m_resources[RES_GOLD] < gHeroGoldCost) {
                    strcpy(gText, cCastleInfo[CASTLE_INFO_CANNOT_AFFORD_HERO]);
                } else if (gpCurPlayer->m_heroCount == CASTLE_HERO_COUNT_LIMIT) {
                    sprintf(gText, cCastleInfo[CASTLE_INFO_TOO_MANY_HEROES],
                            CASTLE_HERO_COUNT_LIMIT);
                } else if (gpTownManager->m_town->m_occupyingHeroId != -1) {
                    strcpy(gText, cCastleInfo[CASTLE_INFO_TOWN_OCCUPIED]);
                } else {
                    sprintf(gText, cCastleInfo[CASTLE_INFO_RECRUIT_HERO],
                            gpGame->m_heroRecs[
                                gpCurPlayer->AvailableHeroId(heroChoiceIndex)].m_name,
                            gAlignmentNames[
                                gpGame->m_heroRecs[
                                    gpCurPlayer->AvailableHeroId(heroChoiceIndex)].m_cursorType]);
                }
                break;
            default:
                if (message.payload.widget.id == CASTLE_CONTROL_CLOSE)
                    strcpy(gText, cCastleInfo[CASTLE_INFO_EXIT]);
                else
                    strcpy(gText, cCastleInfo[CASTLE_INFO_OPTIONS]);
                break;
            }
            break;
        }

hover_text_ready:
        message.type = CASTLE_MESSAGE_WIDGET;
        message.payload.widget.command = CASTLE_WIDGET_TEXT;
        message.payload.widget.id = CASTLE_CONTROL_STATUS_TEXT;
        message.payload.widget.data.text = gText;
        gpTownManager->m_heroWindow0->BroadcastMessage(message);
        gpTownManager->m_heroWindow0->DrawWindow(
            0, CASTLE_CONTROL_STATUS_FIRST, CASTLE_CONTROL_STATUS_TEXT);
        gpWindowManager->UpdateScreenRegion(
            CASTLE_INTERFACE_X, CASTLE_INTERFACE_Y,
            CASTLE_INTERFACE_WIDTH, CASTLE_INTERFACE_HEIGHT);
        return 1;
    }

    if (message.type == CASTLE_MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
        case CASTLE_INPUT_DESELECT:
            result = message.payload.widget.id == CASTLE_CONTROL_CLOSE;
            break;
        case CASTLE_INPUT_SELECT:
        case CASTLE_INPUT_ALTERNATE_SELECT:
            quickFlag = (message.payload.widget.parameter & CASTLE_QUICK_VIEW_MODIFIER) != 0;
            switch (buildingIndex) {
            case CASTLE_CONTROL_CAPTAIN_FORMATION_SPREAD:
                if (quickFlag) {
                    NormalDialog(
                        "{Spread Formation}\n\n'Spread' combat formation spreads your armies from the top to the bottom of the battlefield, with at least one empty space between each army.",
                        4, -1, -1, -1, 0, -1, 0, -1, 0);
                } else {
                    gpTownManager->m_town->m_formation = TOWN_FORMATION_SPREAD;
                    gpTownManager->SetupCastle(gpTownManager->m_heroWindow0, 1);
                    gpTownManager->m_heroWindow0->DrawWindow();
                }
                break;

            case CASTLE_CONTROL_CAPTAIN_FORMATION_GROUPED:
                if (quickFlag) {
                    NormalDialog(
                        "{Grouped Formation}\n\n'Grouped' combat formation bunches your army together in the center of your side of the battlefield.",
                        4, -1, -1, -1, 0, -1, 0, -1, 0);
                } else {
                    gpTownManager->m_town->m_formation = TOWN_FORMATION_GROUPED;
                    gpTownManager->SetupCastle(gpTownManager->m_heroWindow0, 1);
                    gpTownManager->m_heroWindow0->DrawWindow();
                }
                break;

            case CASTLE_MAGE_GUILD:
                if (quickFlag ||
                    gpTownManager->m_town->m_buildState ==
                        CASTLE_MAGE_GUILD_MAX_LEVEL ||
                    (gpTownManager->m_buildableBuildings & (1L << buildingIndex)))
                    goto buy_building;
                break;

            case 1:
            case 2:
            case 3:
            case 4:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 15:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 30:
                if (quickFlag ||
                    (!(gpTownManager->m_town->m_buildings & (1L << buildingIndex)) &&
                     (gpTownManager->m_buildableBuildings & (1L << buildingIndex)))) {
buy_building:
                    for (loopIndex = 0;
                         loopIndex < gpTownManager->m_townObjectCount &&
                         gpTownManager->m_townObjects[loopIndex]->m_buildingId !=
                             buildingIndex;
                         ++loopIndex) {
                    }
                    result = gpTownManager->BuyBuild(
                        buildingIndex,
                        (gpTownManager->m_affordableBuildings &
                         (1L << buildingIndex)) == 0,
                        quickFlag);
                }
                break;

            case -1:
                switch (message.payload.widget.id) {
                case CASTLE_CONTROL_HERO_FIRST:
                case CASTLE_CONTROL_HERO_FIRST + 1:
                    heroChoiceIndex = message.payload.widget.id - CASTLE_CONTROL_HERO_FIRST;
                    goto hero_selected;
                case CASTLE_CONTROL_RECRUIT_FIRST:
                case CASTLE_CONTROL_RECRUIT_FIRST + 1:
                    heroChoiceIndex = message.payload.widget.id - CASTLE_CONTROL_RECRUIT_FIRST;
hero_selected:
                    if (quickFlag) {
                        HeroView(gpCurPlayer->AvailableHeroId(heroChoiceIndex), 1, 0);
                        casWin->DrawWindow();
                        gpTownManager->m_bankBox->Update(1);
                        gpWindowManager->FadeScreen(0, 8, 0);
                    } else {
                        cannotRecruitHero =
                            gpTownManager->m_recruitResult != 0 ||
                            gpCurPlayer->m_resources[RES_GOLD] < gHeroGoldCost ||
                            gpCurPlayer->m_heroCount >= CASTLE_HERO_COUNT_LIMIT ||
                            gpTownManager->m_town->m_occupyingHeroId != -1;
                        result = gpTownManager->RecruitHero(heroChoiceIndex,
                                                            cannotRecruitHero);
                    }
                    break;
                default:
                    goto selection_done;
                }
                break;
            }
            break;
        }
    }

selection_done:
    if (result == 0)
        return 1;
    message.payload.widget.id = CASTLE_EXIT_COMMAND;
    message.payload.widget.command = message.payload.widget.id;
    return 2;
}

// ---- globals (definitions, RVA order) ----
DATA(0x004ef5e0) u8 castleSlotsBase[CASTLE_SLOT_COUNT] = {
    19, 20, 21, 22, 23, 24, 0, 2, 1, 3, 7, 10, 4, 11, 13, 8, 9, 12
};
DATA(0x00525040) heroWindow *casWin;
DATA(0x00525048) u8 castleSlotsUse[CASTLE_SLOT_COUNT];
