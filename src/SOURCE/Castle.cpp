// Reconstructed from CodeView NB09 of HEROES2W.EXE - NOT original source.
// compiland: .\Win32_Re\Castle.obj   from: (directly linked into exe)
// functions: 2   data: 3
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_carcass_types.h>
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
void townManager::SetupCastle(heroWindow *window, int updateOnly)
{
    widget *backgroundWidget;
    int backgroundFrame;
    int slot;
    int row;
    int column;
    int widgetFrame;
    int terrainIconFrame;
    int raceIconFrame;
    unsigned long captainBuilt;
    char iconName[16];
    char statLine[200];
    tag_message message;

    casWin = window;
    short builtFrame = CASTLE_FRAME_BUILT;
    short cannotBuildFrame = CASTLE_FRAME_CANNOT_BUILD;
    short cannotAffordFrame = CASTLE_FRAME_CANNOT_AFFORD;
    for (slot = 0; slot < CASTLE_SLOT_COUNT; ++slot) {
        castleSlotsUse[slot] = castleSlotsBase[slot];
        if (castleSlotsBase[slot] >= CASTLE_FIRST_DWELLING + 1 &&
            castleSlotsBase[slot] <= CASTLE_LAST_DWELLING &&
            ((m_town->m_buildings & (1L << castleSlotsBase[slot])) ||
             (m_town->m_buildings &
              (1L << (castleSlotsBase[slot] + CASTLE_UPGRADE_OFFSET))) ||
             (castleSlotsBase[slot] == CASTLE_WARLOCK_TOP_DWELLING &&
              m_town->m_type == TOWN_TYPE_WARLOCK &&
              (m_town->m_buildings & TOWN_BUILDING_CASTLE))) &&
            (gTownEligibleBuildMask[m_town->m_type] &
             (1L << (castleSlotsBase[slot] + CASTLE_UPGRADE_OFFSET)))) {
            if (castleSlotsBase[slot] == CASTLE_WARLOCK_TOP_DWELLING &&
                m_town->m_type == TOWN_TYPE_WARLOCK &&
                ((m_town->m_buildings & TOWN_BUILDING_TENT) ||
                 (m_town->m_buildings & TOWN_BUILDING_CASTLE))) {
                castleSlotsUse[slot] = CASTLE_WARLOCK_TOP_UPGRADE;
            } else {
                castleSlotsUse[slot] =
                    castleSlotsBase[slot] + CASTLE_UPGRADE_OFFSET;
            }
        }
    }

    m_buildableBuildings = 0;
    m_affordableBuildings = m_buildableBuildings;
    for (slot = 0; slot < CASTLE_SLOT_COUNT; ++slot) {
        if (CanBuy(m_town, castleSlotsUse[slot]))
            m_affordableBuildings |= 1L << castleSlotsUse[slot];
        if (CanBuild(m_town, castleSlotsUse[slot]))
            m_buildableBuildings |= 1L << castleSlotsUse[slot];
    }

    message.type = CASTLE_MESSAGE_WIDGET;
    message.payload.widget.command = CASTLE_WIDGET_FRAME;
    for (slot = 0; slot < CASTLE_SLOT_COUNT; ++slot) {
        message.payload.widget.id = CASTLE_CONTROL_BUILDING_ICON_FIRST + slot;
        message.payload.widget.data.value = castleSlotsUse[slot];
        casWin->BroadcastMessage(message);
    }

    message.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
    sprintf(iconName, "cstl%s.icn", cHeroTypeShortName[m_town->m_type]);
    message.payload.widget.data.text = iconName;
    for (slot = 0; slot < CASTLE_SLOT_COUNT; ++slot) {
        message.payload.widget.id = CASTLE_CONTROL_BUILDING_ICON_FIRST + slot;
        casWin->BroadcastMessage(message);
    }

    message.payload.widget.command = CASTLE_WIDGET_TEXT;
    for (slot = 0; slot < CASTLE_SLOT_COUNT; ++slot) {
        message.payload.widget.id = CASTLE_CONTROL_BUILDING_NAME_FIRST + slot;
        if (castleSlotsUse[slot] == CASTLE_MAGE_GUILD) {
            sprintf(gText, "Mage Guild, Level %d",
                    m_town->m_buildState + 1 < CASTLE_MAGE_GUILD_MAX_LEVEL
                        ? m_town->m_buildState + 1
                        : CASTLE_MAGE_GUILD_MAX_LEVEL);
            message.payload.widget.data.text = gText;
        } else {
            message.payload.widget.data.text = GetBuildingName(m_town->m_type, castleSlotsUse[slot]);
        }
        casWin->BroadcastMessage(message);
    }

    for (slot = 0; slot < CASTLE_SLOT_COUNT; ++slot) {
        widgetFrame = -1;
        if ((m_town->m_buildings & (1L << castleSlotsUse[slot])) &&
            (castleSlotsUse[slot] != CASTLE_MAGE_GUILD ||
             m_town->m_buildState == CASTLE_MAGE_GUILD_MAX_LEVEL)) {
            widgetFrame = CASTLE_FRAME_BUILT;
        } else {
            if (!(m_buildableBuildings & (1L << castleSlotsUse[slot])))
                widgetFrame = CASTLE_FRAME_CANNOT_BUILD;
            else if (!(m_affordableBuildings & (1L << castleSlotsUse[slot])))
                widgetFrame = CASTLE_FRAME_CANNOT_AFFORD;
        }

        if (widgetFrame != -1) {
            message.payload.widget.command = CASTLE_WIDGET_ENABLE;
            message.payload.widget.id = CASTLE_CONTROL_BUILDING_BUTTON_FIRST + slot;
            message.payload.widget.data.value = 4;
            casWin->BroadcastMessage(message);
            message.payload.widget.command = CASTLE_WIDGET_FRAME;
            message.payload.widget.data.value = widgetFrame;
            casWin->BroadcastMessage(message);
        } else {
            message.payload.widget.command = CASTLE_WIDGET_DISABLE;
            message.payload.widget.data.value = 4;
            message.payload.widget.id = CASTLE_CONTROL_BUILDING_BUTTON_FIRST + slot;
            casWin->BroadcastMessage(message);
        }

        if (widgetFrame == CASTLE_FRAME_BUILT) {
            message.payload.widget.command = CASTLE_WIDGET_DISABLE;
            message.payload.widget.data.value = 4;
            message.payload.widget.id = CASTLE_CONTROL_BUILDING_OVERLAY_FIRST + slot;
            casWin->BroadcastMessage(message);
        } else {
            message.payload.widget.command = CASTLE_WIDGET_ENABLE;
            message.payload.widget.data.value = 4;
            message.payload.widget.id = CASTLE_CONTROL_BUILDING_OVERLAY_FIRST + slot;
            casWin->BroadcastMessage(message);
            message.payload.widget.command = CASTLE_WIDGET_FRAME;
            message.payload.widget.data.value =
                widgetFrame == -1 ? CASTLE_FRAME_AVAILABLE : CASTLE_FRAME_UNAVAILABLE;
            casWin->BroadcastMessage(message);
        }
    }

    captainBuilt = m_town->m_buildings & CASTLE_CAPTAIN_BUILDING_MASK;
    message.payload.widget.command = captainBuilt != 0 ? CASTLE_WIDGET_DISABLE : CASTLE_WIDGET_ENABLE;
    message.payload.widget.id = CASTLE_CONTROL_CAPTAIN_OVERLAY;
    message.payload.widget.data.value = 6;
    casWin->BroadcastMessage(message);
    message.payload.widget.command = CASTLE_WIDGET_FRAME;
    message.payload.widget.id = CASTLE_CONTROL_CAPTAIN_ICON;
    message.payload.widget.data.value = captainBuilt != 0;
    casWin->BroadcastMessage(message);
    sprintf(gText, "CSTLCAP%c.ICN", cHeroTypeInitial[m_town->m_type]);
    message.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
    message.payload.widget.id = CASTLE_CONTROL_CAPTAIN_ICON;
    message.payload.widget.data.text = gText;
    casWin->BroadcastMessage(message);
    message.payload.widget.command = captainBuilt != 0 ? CASTLE_WIDGET_ENABLE : CASTLE_WIDGET_DISABLE;
    message.payload.widget.id = CASTLE_CONTROL_CAPTAIN_FLAG;
    message.payload.widget.data.value = 4;
    casWin->BroadcastMessage(message);
    if (captainBuilt != 0) {
        message.payload.widget.command = CASTLE_WIDGET_FRAME;
        message.payload.widget.id = CASTLE_CONTROL_CAPTAIN_FLAG;
        message.payload.widget.data.value = gpCurPlayer->m_color;
        casWin->BroadcastMessage(message);
    }

    widgetFrame = -1;
    if (captainBuilt != 0) {
        message.payload.widget.command = CASTLE_WIDGET_TEXT;
        message.payload.widget.data.text = gText;
        sprintf(gText, "");
        for (slot = 0; slot < CASTLE_PRIMARY_STAT_COUNT; ++slot) {
            sprintf(statLine, "%s\n", gStatNames[slot]);
            strcat(gText, statLine);
        }
        message.payload.widget.id = CASTLE_CONTROL_CAPTAIN_STATS;
        casWin->BroadcastMessage(message);
        sprintf(gText, "");
        for (slot = 0; slot < CASTLE_PRIMARY_STAT_COUNT; ++slot) {
            sprintf(statLine, "%d\n", captainStats[m_town->m_type][slot]);
            strcat(gText, statLine);
        }
        message.payload.widget.id = CASTLE_CONTROL_CAPTAIN_VALUES;
        casWin->BroadcastMessage(message);
        message.payload.widget.command = m_town->m_formation != TOWN_FORMATION_SPREAD
                             ? CASTLE_WIDGET_DISABLE
                             : CASTLE_WIDGET_ENABLE;
        message.payload.widget.id = CASTLE_CONTROL_CAPTAIN_FORMATION_SPREAD_INACTIVE;
        message.payload.widget.data.value = 4;
        casWin->BroadcastMessage(message);
        message.payload.widget.command = m_town->m_formation != TOWN_FORMATION_SPREAD
                             ? CASTLE_WIDGET_ENABLE
                             : CASTLE_WIDGET_DISABLE;
        message.payload.widget.id = CASTLE_CONTROL_CAPTAIN_FORMATION_GROUPED_INACTIVE;
        message.payload.widget.data.value = 4;
        casWin->BroadcastMessage(message);
    } else {
        if (!CanBuild(m_town, CASTLE_CAPTAIN))
            widgetFrame = CASTLE_FRAME_CANNOT_BUILD;
        else if (!CanBuy(m_town, CASTLE_CAPTAIN))
            widgetFrame = CASTLE_FRAME_CANNOT_AFFORD;
        if (CanBuild(m_town, CASTLE_CAPTAIN))
            m_buildableBuildings |= CASTLE_CAPTAIN_BUILDING_MASK;
        if (CanBuy(m_town, CASTLE_CAPTAIN))
            m_affordableBuildings |= CASTLE_CAPTAIN_BUILDING_MASK;
    }

    message.payload.widget.command = widgetFrame != -1 ? CASTLE_WIDGET_ENABLE : CASTLE_WIDGET_DISABLE;
    message.payload.widget.id = CASTLE_CONTROL_CAPTAIN_BUTTON;
    message.payload.widget.data.value = 4;
    casWin->BroadcastMessage(message);
    if (widgetFrame != -1) {
        message.payload.widget.command = CASTLE_WIDGET_FRAME;
        message.payload.widget.data.value = widgetFrame;
        casWin->BroadcastMessage(message);
    }

    if (gpCurPlayer->m_resources[RES_GOLD] < gHeroGoldCost)
        widgetFrame = CASTLE_FRAME_CANNOT_AFFORD;
    else if (gpCurPlayer->m_heroCount == CASTLE_HERO_COUNT_LIMIT ||
             m_town->m_occupyingHeroId != -1)
        widgetFrame = CASTLE_FRAME_CANNOT_BUILD;
    else if (m_recruitResult == 0)
        widgetFrame = -1;
    else
        widgetFrame = CASTLE_FRAME_BUILT;

    for (slot = 0; slot < 2; ++slot) {
        message.payload.widget.data.value = 4;
        message.payload.widget.id = CASTLE_CONTROL_RECRUIT_FIRST + slot;
        if (widgetFrame != -1) {
            message.payload.widget.command = CASTLE_WIDGET_ENABLE;
            casWin->BroadcastMessage(message);
            message.payload.widget.command = CASTLE_WIDGET_FRAME;
            message.payload.widget.data.value = widgetFrame;
            casWin->BroadcastMessage(message);
        } else {
            message.payload.widget.command = CASTLE_WIDGET_DISABLE;
            casWin->BroadcastMessage(message);
        }
        message.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
        sprintf(iconName, "port%04d.icn",
                gpGame->GetHero(gpCurPlayer->AvailableHeroId(slot))->m_portrait);
        message.payload.widget.data.text = iconName;
        message.payload.widget.id = CASTLE_CONTROL_HERO_FIRST + slot;
        casWin->BroadcastMessage(message);
    }

    terrainIconFrame =
        ((giGroundToTerrain[
             gpGame->m_worldMap.GetCell(m_town->m_x, m_town->m_y)->tile] *
             CASTLE_TERRAIN_ICON_COLUMNS) +
         -CASTLE_TERRAIN_ICON_COLUMNS)
        << 1;
    raceIconFrame = m_town->m_type << 5;
    if (updateOnly == 0) {
        backgroundFrame = 0;
        for (row = 5; row <= 6; ++row) {
            for (column = 4; column <= 8; ++column) {
                backgroundWidget = new iconWidget(
                    static_cast<short>((column - 4) * 32 + 458),
                    static_cast<short>((row - 2) * 32), 32, 32,
                    "objntwba.icn",
                    static_cast<short>(terrainIconFrame + backgroundFrame),
                    0, -1, 16, 1);
                if (backgroundWidget == 0)
                    MemError();
                casWin->AddWidget(backgroundWidget, -1);
                ++backgroundFrame;
            }
        }
        backgroundFrame = 0;
        for (row = 2; row <= 5; ++row) {
            for (column = 4; column <= 8; ++column) {
                if (row == 2 && column != 6) {
                    continue;
                } else {
                    backgroundWidget = new iconWidget(
                        static_cast<short>((column - 4) * 32 + 458),
                        static_cast<short>((row - 2) * 32), 32, 32,
                        "objntown.icn",
                        static_cast<short>(raceIconFrame + backgroundFrame),
                        0, -1, 16, 1);
                    if (backgroundWidget == 0)
                        MemError();
                    casWin->AddWidget(backgroundWidget, -1);
                    ++backgroundFrame;
                }
            }
        }
        if (xIsExpansionMap == 0 && m_town->m_type == TOWN_TYPE_NECROMANCER) {
            backgroundWidget = new iconWidget(149, 157, 137, 72,
                                              "caslxtra.icn", 0, 0, -1,
                                              16, 1);
            if (backgroundWidget == 0)
                MemError();
            casWin->AddWidget(backgroundWidget, -1);
        }
    }
}

VA(0x0041e0fb, 0xca3)
int CastleHandler(tag_message &message)
{
    int result;
    short textControl;
    int quickFlag;
    int buildingIndex;
    int loopIndex;
    int cannotRecruitHero;
    int hoverMessage;
    int heroChoiceIndex;

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
        if (gpTownManager->m_lastHoverId == message.payload.widget.id)
            return 1;
        gpTownManager->m_lastHoverId = message.payload.widget.id;
        switch (buildingIndex) {
        case CASTLE_CONTROL_CAPTAIN_FORMATION_SPREAD:
            sprintf(gText, cCastleInfo[CASTLE_INFO_SPREAD_FORMATION]);
            break;
        case CASTLE_CONTROL_CAPTAIN_FORMATION_GROUPED:
            sprintf(gText, cCastleInfo[CASTLE_INFO_GROUPED_FORMATION]);
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
                break;
            case CASTLE_CONTROL_RECRUIT_FIRST:
            case CASTLE_CONTROL_RECRUIT_FIRST + 1:
                heroChoiceIndex = message.payload.widget.id - CASTLE_CONTROL_RECRUIT_FIRST;
                break;
            default:
                if (message.payload.widget.id == CASTLE_CONTROL_CLOSE)
                    strcpy(gText, cCastleInfo[CASTLE_INFO_EXIT]);
                else
                    strcpy(gText, cCastleInfo[CASTLE_INFO_OPTIONS]);
                goto hover_text_ready;
            }

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
        if (message.payload.widget.command == CASTLE_INPUT_DESELECT) {
            result = message.payload.widget.id == CASTLE_CONTROL_CLOSE;
        } else if (message.payload.widget.command == CASTLE_INPUT_SELECT ||
                   message.payload.widget.command == CASTLE_INPUT_ALTERNATE_SELECT) {
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
                    break;
                case CASTLE_CONTROL_RECRUIT_FIRST:
                case CASTLE_CONTROL_RECRUIT_FIRST + 1:
                    heroChoiceIndex = message.payload.widget.id - CASTLE_CONTROL_RECRUIT_FIRST;
                    break;
                default:
                    goto selection_done;
                }
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
            }
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
DATA(0x004ef5e0) unsigned char castleSlotsBase[CASTLE_SLOT_COUNT] = {
    19, 20, 21, 22, 23, 24, 0, 2, 1, 3, 7, 10, 4, 11, 13, 8, 9, 12
};
DATA(0x00525040) heroWindow *casWin;
DATA(0x00525048) unsigned char castleSlotsUse[CASTLE_SLOT_COUNT];
