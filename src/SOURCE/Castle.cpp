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

H2_ENUM_BEGIN(CastleControl)
    CONTROL_BUILDING_OVERLAY_FIRST             = 400,
    CONTROL_STATUS_FIRST                       = 500,
    CONTROL_BUILDING_NAME_FIRST                = 600,
    CONTROL_BUILDING_ICON_FIRST                = 700,
    CONTROL_BUILDING_BUTTON_FIRST              = 800,
    CONTROL_HERO_FIRST                         = 900,
    CONTROL_RECRUIT_FIRST                      = 902,
    CONTROL_CAPTAIN_ICON                       = 1100,
    CONTROL_CAPTAIN_OVERLAY                    = 1101,
    CONTROL_CAPTAIN_BUTTON                     = 1102,
    CONTROL_CAPTAIN_STATS                      = 1104,
    CONTROL_CAPTAIN_VALUES                     = 1105,
    CONTROL_CAPTAIN_FLAG                       = 1106,
    CONTROL_CAPTAIN_FORMATION_SPREAD_INACTIVE  = 213,
    CONTROL_CAPTAIN_FORMATION_SPREAD           = 214,
    CONTROL_CAPTAIN_FORMATION_GROUPED_INACTIVE = 215,
    CONTROL_CAPTAIN_FORMATION_GROUPED          = 216,
    CONTROL_STATUS_TEXT                        = 502,
    CONTROL_CLOSE                              = 0x7800
H2_ENUM_END(CastleControl)

H2_ENUM_BEGIN(CastleWidgetFrame)
    FRAME_AVAILABLE     = 1,
    FRAME_UNAVAILABLE   = 2,
    FRAME_BUILT         = 11,
    FRAME_CANNOT_BUILD  = 12,
    FRAME_CANNOT_AFFORD = 13
H2_ENUM_END(CastleWidgetFrame)

H2_ENUM_CLASS_BEGIN(CastleInfoText)
    INFO_ALREADY_BUILT      = 4,
    INFO_CANNOT_BUILD       = 5,
    INFO_CANNOT_AFFORD      = 6,
    INFO_BUILD              = 7,
    INFO_CANNOT_AFFORD_HERO = 8,
    INFO_TOO_MANY_HEROES    = 9,
    INFO_TOWN_OCCUPIED      = 10,
    INFO_RECRUIT_HERO       = 11,
    INFO_EXIT               = 12,
    INFO_OPTIONS            = 13,
    INFO_GROUPED_FORMATION  = 14,
    INFO_SPREAD_FORMATION   = 15
H2_ENUM_CLASS_END(CastleInfoText)

H2_ENUM_BEGIN(CastleConstant)
    QUICK_VIEW_MODIFIER   = 2,
    EXIT_COMMAND          = 10,
    MAGE_GUILD_MAX_LEVEL  = 5,
    CAPTAIN_BUILDING_MASK = 0x8000,
    HERO_COUNT_LIMIT      = 8,
    PRIMARY_STAT_COUNT    = 4,
    INTERFACE_X           = 18,
    INTERFACE_Y           = 463,
    INTERFACE_WIDTH       = 604,
    INTERFACE_HEIGHT      = 16,
    BACKGROUND_LEFT       = 458,
    BACKGROUND_TOP        = 0,
    TERRAIN_ICON_COLUMNS  = 5,
    TERRAIN_ICON_FRAMES   = 2,
    RACE_ICON_FRAMES      = 32
H2_ENUM_END(CastleConstant)

VA(0x0041d040, 0x10bb)
void townManager::SetupCastle(heroWindow* window, i32 updateOnly) {
    widget* backgroundWidget19;
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
    i16 builtFrame7 = IDX(FRAME_BUILT);
    i16 cannotBuildFrame17 = IDX(FRAME_CANNOT_BUILD);
    i16 cannotAffordFrame6 = IDX(FRAME_CANNOT_AFFORD);
    for (slot7 = 0; slot7 < CASTLE_SLOT_COUNT; ++slot7) {
        castleSlotsUse[slot7] = castleSlotsBase[slot7];
        if (castleSlotsBase[slot7] >= IDX(BUILDING_SLOT_DWELLING_FIRST) + 1
            && castleSlotsBase[slot7] <= IDX(BUILDING_SLOT_DWELLING_SIXTH)
            && ((m_town->m_buildings & (1L << castleSlotsBase[slot7]))
                || (m_town->m_buildings & (1L << (castleSlotsBase[slot7] + CASTLE_UPGRADE_OFFSET)))
                || (castleSlotsBase[slot7] == IDX(BUILDING_SLOT_DWELLING_SIXTH)
                    && m_town->m_type == IDX(FACTION_WARLOCK)
                    && (m_town->m_buildings & IDX(TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6))))
            && (gTownEligibleBuildMask[m_town->m_type]
                & (1L << (castleSlotsBase[slot7] + CASTLE_UPGRADE_OFFSET)))) {
            if (castleSlotsBase[slot7] == IDX(BUILDING_SLOT_DWELLING_SIXTH)
                && m_town->m_type == IDX(FACTION_WARLOCK)
                && ((m_town->m_buildings & IDX(TOWN_BUILDING_UPGRADED_DWELLING_6))
                    || (m_town->m_buildings & IDX(TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6)))) {
                castleSlotsUse[slot7] = IDX(BUILDING_SLOT_DWELLING_LAST);
            } else {
                castleSlotsUse[slot7] = castleSlotsBase[slot7] + CASTLE_UPGRADE_OFFSET;
            }
        }
    }

    m_buildableBuildings = 0;
    m_affordableBuildings = m_buildableBuildings;
    for (slot7 = 0; slot7 < CASTLE_SLOT_COUNT; ++slot7) {
        if (CanBuy(m_town, BuildingSlotType(castleSlotsUse[slot7])))
            m_affordableBuildings |= 1L << castleSlotsUse[slot7];
        if (CanBuild(m_town, BuildingSlotType(castleSlotsUse[slot7])))
            m_buildableBuildings |= 1L << castleSlotsUse[slot7];
    }

    message3.type = MESSAGE_WIDGET;
    message3.payload.widget.command = CASTLE_WIDGET_FRAME;
    for (slot7 = 0; slot7 < CASTLE_SLOT_COUNT; ++slot7) {
        message3.payload.widget.id = CONTROL_BUILDING_ICON_FIRST + slot7;
        message3.payload.widget.data.value = castleSlotsUse[slot7];
        casWin->BroadcastMessage(message3);
    }

    message3.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
    sprintf(iconName4, "cstl%s.icn", cHeroTypeShortName[m_town->m_type]);
    message3.payload.widget.data.text = iconName4;
    for (slot7 = 0; slot7 < CASTLE_SLOT_COUNT; ++slot7) {
        message3.payload.widget.id = CONTROL_BUILDING_ICON_FIRST + slot7;
        casWin->BroadcastMessage(message3);
    }

    message3.payload.widget.command = CASTLE_WIDGET_TEXT;
    for (slot7 = 0; slot7 < CASTLE_SLOT_COUNT; ++slot7) {
        message3.payload.widget.id = CONTROL_BUILDING_NAME_FIRST + slot7;
        if (castleSlotsUse[slot7] == IDX(CASTLE_MAGE_GUILD)) {
            sprintf(
                gText,
                "Mage Guild, Level %d",
                m_town->m_buildState + 1 < MAGE_GUILD_MAX_LEVEL ? m_town->m_buildState + 1
                                                                       : MAGE_GUILD_MAX_LEVEL
            );
            message3.payload.widget.data.text = gText;
        } else {
            message3.payload.widget.data.text = GetBuildingName(
                FactionType(m_town->m_type),
                BuildingSlotType(castleSlotsUse[slot7])
            );
        }
        casWin->BroadcastMessage(message3);
    }

    for (slot7 = 0; slot7 < CASTLE_SLOT_COUNT; ++slot7) {
        widgetFrame12 = -1;
        if ((m_town->m_buildings & (1L << castleSlotsUse[slot7]))
            && (castleSlotsUse[slot7] != IDX(CASTLE_MAGE_GUILD)
                || m_town->m_buildState == MAGE_GUILD_MAX_LEVEL)) {
            widgetFrame12 = FRAME_BUILT;
        } else {
            if (!(m_buildableBuildings & (1L << castleSlotsUse[slot7])))
                widgetFrame12 = FRAME_CANNOT_BUILD;
            else if (!(m_affordableBuildings & (1L << castleSlotsUse[slot7])))
                widgetFrame12 = FRAME_CANNOT_AFFORD;
        }

        if (widgetFrame12 != -1) {
            message3.payload.widget.command = CASTLE_WIDGET_ENABLE;
            message3.payload.widget.id = CONTROL_BUILDING_BUTTON_FIRST + slot7;
            message3.payload.widget.data.value = 4;
            casWin->BroadcastMessage(message3);
            message3.payload.widget.command = CASTLE_WIDGET_FRAME;
            message3.payload.widget.data.value = widgetFrame12;
            casWin->BroadcastMessage(message3);
        } else {
            message3.payload.widget.command = CASTLE_WIDGET_DISABLE;
            message3.payload.widget.data.value = 4;
            message3.payload.widget.id = CONTROL_BUILDING_BUTTON_FIRST + slot7;
            casWin->BroadcastMessage(message3);
        }

        if (widgetFrame12 == FRAME_BUILT) {
            message3.payload.widget.command = CASTLE_WIDGET_DISABLE;
            message3.payload.widget.data.value = 4;
            message3.payload.widget.id = CONTROL_BUILDING_OVERLAY_FIRST + slot7;
            casWin->BroadcastMessage(message3);
        } else {
            message3.payload.widget.command = CASTLE_WIDGET_ENABLE;
            message3.payload.widget.data.value = 4;
            message3.payload.widget.id = CONTROL_BUILDING_OVERLAY_FIRST + slot7;
            casWin->BroadcastMessage(message3);
            message3.payload.widget.command = CASTLE_WIDGET_FRAME;
            message3.payload.widget.data.value =
                widgetFrame12 == -1 ? IDX(FRAME_AVAILABLE) : FRAME_UNAVAILABLE;
            casWin->BroadcastMessage(message3);
        }
    }

    captainBuilt = m_town->m_buildings & CAPTAIN_BUILDING_MASK;
    message3.payload.widget.command =
        captainBuilt != 0 ? CASTLE_WIDGET_DISABLE : CASTLE_WIDGET_ENABLE;
    message3.payload.widget.id = CONTROL_CAPTAIN_OVERLAY;
    message3.payload.widget.data.value = 6;
    casWin->BroadcastMessage(message3);
    message3.payload.widget.command = CASTLE_WIDGET_FRAME;
    message3.payload.widget.id = CONTROL_CAPTAIN_ICON;
    message3.payload.widget.data.value = captainBuilt != 0;
    casWin->BroadcastMessage(message3);
    sprintf(gText, "CSTLCAP%c.ICN", cHeroTypeInitial[m_town->m_type]);
    message3.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
    message3.payload.widget.id = CONTROL_CAPTAIN_ICON;
    message3.payload.widget.data.text = gText;
    casWin->BroadcastMessage(message3);
    message3.payload.widget.command =
        captainBuilt != 0 ? CASTLE_WIDGET_ENABLE : CASTLE_WIDGET_DISABLE;
    message3.payload.widget.id = CONTROL_CAPTAIN_FLAG;
    message3.payload.widget.data.value = 4;
    casWin->BroadcastMessage(message3);
    if (captainBuilt != 0) {
        message3.payload.widget.command = CASTLE_WIDGET_FRAME;
        message3.payload.widget.id = CONTROL_CAPTAIN_FLAG;
        message3.payload.widget.data.value = gpCurPlayer->m_color;
        casWin->BroadcastMessage(message3);
    }

    widgetFrame12 = -1;
    if (captainBuilt != 0) {
        message3.payload.widget.command = CASTLE_WIDGET_TEXT;
        message3.payload.widget.data.text = gText;
        sprintf(gText, "");
        for (slot7 = 0; slot7 < PRIMARY_STAT_COUNT; ++slot7) {
            sprintf(statLine11, "%s\n", gStatNames[slot7]);
            strcat(gText, statLine11);
        }
        message3.payload.widget.id = CONTROL_CAPTAIN_STATS;
        casWin->BroadcastMessage(message3);
        sprintf(gText, "");
        for (slot7 = 0; slot7 < PRIMARY_STAT_COUNT; ++slot7) {
            sprintf(statLine11, "%d\n", captainStats[m_town->m_type][slot7]);
            strcat(gText, statLine11);
        }
        message3.payload.widget.id = CONTROL_CAPTAIN_VALUES;
        casWin->BroadcastMessage(message3);
        message3.payload.widget.command = m_town->m_formation != TOWN_FORMATION_SPREAD
                                              ? CASTLE_WIDGET_DISABLE
                                              : CASTLE_WIDGET_ENABLE;
        message3.payload.widget.id = CONTROL_CAPTAIN_FORMATION_SPREAD_INACTIVE;
        message3.payload.widget.data.value = 4;
        casWin->BroadcastMessage(message3);
        message3.payload.widget.command = m_town->m_formation == TOWN_FORMATION_SPREAD
                                              ? CASTLE_WIDGET_DISABLE
                                              : CASTLE_WIDGET_ENABLE;
        message3.payload.widget.id = CONTROL_CAPTAIN_FORMATION_GROUPED_INACTIVE;
        message3.payload.widget.data.value = 4;
        casWin->BroadcastMessage(message3);
    } else {
        if (!CanBuild(m_town, CASTLE_CAPTAIN))
            widgetFrame12 = FRAME_CANNOT_BUILD;
        else if (!CanBuy(m_town, CASTLE_CAPTAIN))
            widgetFrame12 = FRAME_CANNOT_AFFORD;
        if (CanBuild(m_town, CASTLE_CAPTAIN))
            m_buildableBuildings |= CAPTAIN_BUILDING_MASK;
        if (CanBuy(m_town, CASTLE_CAPTAIN))
            m_affordableBuildings |= CAPTAIN_BUILDING_MASK;
    }

    message3.payload.widget.command =
        widgetFrame12 == -1 ? CASTLE_WIDGET_DISABLE : CASTLE_WIDGET_ENABLE;
    message3.payload.widget.id = CONTROL_CAPTAIN_BUTTON;
    message3.payload.widget.data.value = 4;
    casWin->BroadcastMessage(message3);
    if (widgetFrame12 != -1) {
        message3.payload.widget.command = CASTLE_WIDGET_FRAME;
        message3.payload.widget.data.value = widgetFrame12;
        casWin->BroadcastMessage(message3);
    }

    if (gpCurPlayer->m_resources[IDX(RES_GOLD)] < gHeroGoldCost)
        widgetFrame12 = FRAME_CANNOT_AFFORD;
    else if (gpCurPlayer->m_heroCount == HERO_COUNT_LIMIT || m_town->m_occupyingHeroId != -1)
        widgetFrame12 = FRAME_CANNOT_BUILD;
    else if (m_recruitResult != 0)
        widgetFrame12 = FRAME_BUILT;
    else
        widgetFrame12 = -1;

    for (slot7 = 0; slot7 < 2; ++slot7) {
        message3.payload.widget.data.value = 4;
        message3.payload.widget.id = CONTROL_RECRUIT_FIRST + slot7;
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
        sprintf(
            iconName4,
            "port%04d.icn",
            gpGame->m_heroRecs[gpCurPlayer->AvailableHeroId(slot7)].m_portrait
        );
        message3.payload.widget.data.text = iconName4;
        message3.payload.widget.id = CONTROL_HERO_FIRST + slot7;
        casWin->BroadcastMessage(message3);
    }

    i32 backgroundLeft4 = BACKGROUND_LEFT;
    i32 backgroundTop12 = BACKGROUND_TOP;
    terrainIconFrame27 = (giGroundToTerrain[gpGame->m_worldMap.GetCell(m_town->m_x, m_town->m_y)
                                                ->m_terrainImageIndex]
                          - 1)
                         * TERRAIN_ICON_COLUMNS * TERRAIN_ICON_FRAMES;
    raceIconFrame = m_town->m_type << 5;
    if (updateOnly == 0) {
        backgroundFrame6 = 0;
        for (row9 = 5; row9 <= 6; ++row9) {
            for (column8 = 4; column8 <= 8; ++column8) {
                backgroundWidget19 = new iconWidget(
                    static_cast<i16>((column8 - 4) * 32 + BACKGROUND_LEFT),
                    static_cast<i16>((row9 - 2) * 32),
                    32,
                    32,
                    "objntwba.icn",
                    static_cast<i16>(OD_STEER(terrainIconFrame27) + backgroundFrame6),
                    0,
                    -1,
                    16,
                    1
                );
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
                    static_cast<i16>((column8 - 4) * 32 + BACKGROUND_LEFT),
                    static_cast<i16>((row9 - 2) * 32),
                    32,
                    32,
                    "objntown.icn",
                    static_cast<i16>(OD_STEER(raceIconFrame) + backgroundFrame6),
                    0,
                    -1,
                    16,
                    1
                );
                if (backgroundWidget19 == 0)
                    MemError();
                casWin->AddWidget(backgroundWidget19, -1);
                ++backgroundFrame6;
            }
        }
        if (xIsExpansionMap == 0 && m_town->m_type == IDX(FACTION_NECROMANCER)) {
            backgroundWidget19 = new iconWidget(149, 157, 137, 72, "caslxtra.icn", 0, 0, -1, 16, 1);
            if (backgroundWidget19 == 0)
                MemError();
            casWin->AddWidget(backgroundWidget19, -1);
        }
    }
}

// @semantic: First divergence at +0x16e: retail loads gpTownManager before message.id for the m_lastHoverId comparison.
VA(0x0041e0fb, 0xca3)
i32 CastleHandler(tag_message& message) {
    i32 result;
    i16 textControl;
    i32 quickFlag;
    i32 buildingIndex;
    i32 loopIndex;
    i32 cannotRecruitHero;
    i32 hoverMessage;
    i32 heroChoiceIndex;

    textControl = IDX(CONTROL_STATUS_TEXT);
    buildingIndex = -1;
    result = 0;
    hoverMessage = 0;

    if (message.type == MESSAGE_MOUSE_MOVE || message.type == MESSAGE_WIDGET) {
        if (message.type == MESSAGE_MOUSE_MOVE) {
            gpWindowManager->ConvertToHover(message);
            hoverMessage = 1;
        }
        if (message.payload.widget.id == CONTROL_CAPTAIN_ICON)
            buildingIndex = IDX(CASTLE_CAPTAIN);
        else if (message.payload.widget.id == CONTROL_CAPTAIN_FORMATION_GROUPED)
            buildingIndex = CONTROL_CAPTAIN_FORMATION_GROUPED;
        else if (message.payload.widget.id == CONTROL_CAPTAIN_FORMATION_SPREAD)
            buildingIndex = CONTROL_CAPTAIN_FORMATION_SPREAD;
        else {
            if (message.payload.widget.id >= CONTROL_BUILDING_NAME_FIRST
                && message.payload.widget.id
                       < CONTROL_BUILDING_NAME_FIRST + static_cast<i32>(CASTLE_SLOT_COUNT))
                buildingIndex = message.payload.widget.id - CONTROL_BUILDING_NAME_FIRST;
            else if (message.payload.widget.id >= CONTROL_BUILDING_ICON_FIRST
                     && message.payload.widget.id < CONTROL_BUILDING_ICON_FIRST
                                                        + static_cast<i32>(CASTLE_SLOT_COUNT))
                buildingIndex = message.payload.widget.id - CONTROL_BUILDING_ICON_FIRST;
            else if (message.payload.widget.id >= CONTROL_BUILDING_BUTTON_FIRST
                     && message.payload.widget.id < CONTROL_BUILDING_BUTTON_FIRST
                                                        + static_cast<i32>(CASTLE_SLOT_COUNT))
                buildingIndex = message.payload.widget.id - CONTROL_BUILDING_BUTTON_FIRST;
            if (buildingIndex != -1)
                buildingIndex = castleSlotsUse[buildingIndex];
        }
    }

    if (hoverMessage) {
        if (message.payload.widget.id == gpTownManager->m_lastHoverId)
            return 1;
        gpTownManager->m_lastHoverId = message.payload.widget.id;
        switch (buildingIndex) {
            case CONTROL_CAPTAIN_FORMATION_GROUPED:
                sprintf(gText, cCastleInfo[IDX(INFO_GROUPED_FORMATION)]);
                break;
            case CONTROL_CAPTAIN_FORMATION_SPREAD:
                sprintf(gText, cCastleInfo[IDX(INFO_SPREAD_FORMATION)]);
                break;

            case IDX(CASTLE_MAGE_GUILD):
                if (!(gpTownManager->m_buildableBuildings & (1L << buildingIndex))) {
                    sprintf(
                        gText,
                        cCastleInfo[IDX(INFO_CANNOT_BUILD)],
                        GetBuildingName(
                            FactionType(gpTownManager->m_town->m_type),
                            BuildingSlotType(buildingIndex)
                        )
                    );
                } else if (!(gpTownManager->m_affordableBuildings & (1L << buildingIndex))) {
                    sprintf(
                        gText,
                        cCastleInfo[IDX(INFO_CANNOT_AFFORD)],
                        GetBuildingName(
                            FactionType(gpTownManager->m_town->m_type),
                            BuildingSlotType(buildingIndex)
                        )
                    );
                } else {
                    if (!(gpTownManager->m_town->m_buildings & 1L))
                        loopIndex = 0;
                    else if (gpTownManager->m_town->m_buildState == MAGE_GUILD_MAX_LEVEL)
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
                if (BitTest(gpGame->m_dailyEventFlags, gpTownManager->m_town->m_id)) {
                    sprintf(gText, "Cannot build.  Already built here this turn.");
                } else if (gpTownManager->m_town->m_buildings & (1L << buildingIndex)) {
                    sprintf(
                        gText,
                        cCastleInfo[IDX(INFO_ALREADY_BUILT)],
                        GetBuildingName(
                            FactionType(gpTownManager->m_town->m_type),
                            BuildingSlotType(buildingIndex)
                        )
                    );
                } else {
                    if (!(gpTownManager->m_buildableBuildings & (1L << buildingIndex)))
                        sprintf(
                            gText,
                            cCastleInfo[IDX(INFO_CANNOT_BUILD)],
                            GetBuildingName(
                                FactionType(gpTownManager->m_town->m_type),
                                BuildingSlotType(buildingIndex)
                            )
                        );
                    else if (!(gpTownManager->m_affordableBuildings & (1L << buildingIndex)))
                        sprintf(
                            gText,
                            cCastleInfo[IDX(INFO_CANNOT_AFFORD)],
                            GetBuildingName(
                                FactionType(gpTownManager->m_town->m_type),
                                BuildingSlotType(buildingIndex)
                            )
                        );
                    else
                        sprintf(
                            gText,
                            cCastleInfo[IDX(INFO_BUILD)],
                            GetBuildingName(
                                FactionType(gpTownManager->m_town->m_type),
                                BuildingSlotType(buildingIndex)
                            )
                        );
                }
                break;

            case -1:
                switch (message.payload.widget.id) {
                    case IDX(CONTROL_HERO_FIRST):
                    case CONTROL_HERO_FIRST + 1:
                        heroChoiceIndex = message.payload.widget.id - CONTROL_HERO_FIRST;
                        goto hero_hover_text;
                    case IDX(CONTROL_RECRUIT_FIRST):
                    case CONTROL_RECRUIT_FIRST + 1:
                        heroChoiceIndex = message.payload.widget.id - CONTROL_RECRUIT_FIRST;
                    hero_hover_text:
                        if (gpCurPlayer->m_resources[IDX(RES_GOLD)] < gHeroGoldCost) {
                            strcpy(gText, cCastleInfo[IDX(INFO_CANNOT_AFFORD_HERO)]);
                        } else if (gpCurPlayer->m_heroCount == HERO_COUNT_LIMIT) {
                            sprintf(
                                gText,
                                cCastleInfo[IDX(INFO_TOO_MANY_HEROES)],
                                HERO_COUNT_LIMIT
                            );
                        } else if (gpTownManager->m_town->m_occupyingHeroId != -1) {
                            strcpy(gText, cCastleInfo[IDX(INFO_TOWN_OCCUPIED)]);
                        } else {
                            sprintf(
                                gText,
                                cCastleInfo[IDX(INFO_RECRUIT_HERO)],
                                gpGame->m_heroRecs[gpCurPlayer->AvailableHeroId(heroChoiceIndex)]
                                    .m_name,
                                gAlignmentNames
                                    [gpGame
                                         ->m_heroRecs[gpCurPlayer->AvailableHeroId(heroChoiceIndex)]
                                         .m_cursorType]
                            );
                        }
                        break;
                    default:
                        if (message.payload.widget.id == CONTROL_CLOSE)
                            strcpy(gText, cCastleInfo[IDX(INFO_EXIT)]);
                        else
                            strcpy(gText, cCastleInfo[IDX(INFO_OPTIONS)]);
                        break;
                }
                break;
        }

    hover_text_ready:
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = CASTLE_WIDGET_TEXT;
        message.payload.widget.id = CONTROL_STATUS_TEXT;
        message.payload.widget.data.text = gText;
        gpTownManager->m_heroWindow0->BroadcastMessage(message);
        gpTownManager->m_heroWindow0
            ->DrawWindow(0, CONTROL_STATUS_FIRST, CONTROL_STATUS_TEXT);
        gpWindowManager->UpdateScreenRegion(
            INTERFACE_X,
            INTERFACE_Y,
            INTERFACE_WIDTH,
            INTERFACE_HEIGHT
        );
        return 1;
    }

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                result = message.payload.widget.id == CONTROL_CLOSE;
                break;
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                quickFlag = (message.payload.widget.parameter & QUICK_VIEW_MODIFIER) != 0;
                switch (buildingIndex) {
                    case CONTROL_CAPTAIN_FORMATION_SPREAD:
                        if (quickFlag) {
                            NormalDialog(
                                "{Spread Formation}\n\n'Spread' combat formation spreads your "
                                "armies from the top to the bottom of the battlefield, with at "
                                "least one empty space between each army.",
                                4,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                        } else {
                            gpTownManager->m_town->m_formation = TOWN_FORMATION_SPREAD;
                            gpTownManager->SetupCastle(gpTownManager->m_heroWindow0, 1);
                            gpTownManager->m_heroWindow0->DrawWindow();
                        }
                        break;

                    case CONTROL_CAPTAIN_FORMATION_GROUPED:
                        if (quickFlag) {
                            NormalDialog(
                                "{Grouped Formation}\n\n'Grouped' combat formation bunches your "
                                "army together in the center of your side of the battlefield.",
                                4,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                        } else {
                            gpTownManager->m_town->m_formation = TOWN_FORMATION_GROUPED;
                            gpTownManager->SetupCastle(gpTownManager->m_heroWindow0, 1);
                            gpTownManager->m_heroWindow0->DrawWindow();
                        }
                        break;

                    case IDX(CASTLE_MAGE_GUILD):
                        if (quickFlag
                            || gpTownManager->m_town->m_buildState == MAGE_GUILD_MAX_LEVEL
                            || (gpTownManager->m_buildableBuildings & (1L << buildingIndex)))
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
                        if (quickFlag
                            || (!(gpTownManager->m_town->m_buildings & (1L << buildingIndex))
                                && (gpTownManager->m_buildableBuildings & (1L << buildingIndex)))) {
                        buy_building:
                            for (loopIndex = 0;
                                 loopIndex < gpTownManager->m_townObjectCount
                                 && gpTownManager->m_townObjects[loopIndex]->m_buildingId
                                        != buildingIndex;
                                 ++loopIndex) {
                            }
                            result = gpTownManager->BuyBuild(
                                buildingIndex,
                                (gpTownManager->m_affordableBuildings & (1L << buildingIndex)) == 0,
                                quickFlag
                            );
                        }
                        break;

                    case -1:
                        switch (message.payload.widget.id) {
                            case IDX(CONTROL_HERO_FIRST):
                            case CONTROL_HERO_FIRST + 1:
                                heroChoiceIndex =
                                    message.payload.widget.id - CONTROL_HERO_FIRST;
                                goto hero_selected;
                            case IDX(CONTROL_RECRUIT_FIRST):
                            case CONTROL_RECRUIT_FIRST + 1:
                                heroChoiceIndex =
                                    message.payload.widget.id - CONTROL_RECRUIT_FIRST;
                            hero_selected:
                                if (quickFlag) {
                                    HeroView(gpCurPlayer->AvailableHeroId(heroChoiceIndex), 1, 0);
                                    casWin->DrawWindow();
                                    gpTownManager->m_bankBox->Update(1);
                                    gpWindowManager->FadeScreen(0, 8, 0);
                                } else {
                                    cannotRecruitHero =
                                        gpTownManager->m_recruitResult != 0
                                        || gpCurPlayer->m_resources[IDX(RES_GOLD)] < gHeroGoldCost
                                        || gpCurPlayer->m_heroCount >= HERO_COUNT_LIMIT
                                        || gpTownManager->m_town->m_occupyingHeroId != -1;
                                    result = gpTownManager->RecruitHero(
                                        heroChoiceIndex,
                                        cannotRecruitHero
                                    );
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
    message.payload.widget.id = EXIT_COMMAND;
    message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
    return 2;
}

DATA(0x004ef5e0) u8 castleSlotsBase[CASTLE_SLOT_COUNT] =
    {19, 20, 21, 22, 23, 24, 0, 2, 1, 3, 7, 10, 4, 11, 13, 8, 9, 12};
DATA(0x00525040) heroWindow* casWin;
DATA(0x00525048) u8 castleSlotsUse[CASTLE_SLOT_COUNT];
