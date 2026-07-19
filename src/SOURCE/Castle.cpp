#include <va.h>
#include <BASE/message.h>
#include <BASE/BITS.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/widgetKind.h>
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
    CONTROL_CLOSE                              = EVENT_WINDOW_FIRST_BUTTON
H2_ENUM_END(CastleControl)

H2_ENUM_BEGIN(CastleWidgetFrame)
    FRAME_NONE          = -1,
    FRAME_AVAILABLE     = 1,
    FRAME_UNAVAILABLE   = 2,
    FRAME_BUILT         = 11,
    FRAME_CANNOT_BUILD  = 12,
    FRAME_CANNOT_AFFORD = 13
H2_ENUM_END(CastleWidgetFrame)

H2_ENUM_CLASS_BEGIN(CastleInfoText)
    INFO_BUILD_MAGE_GUILD         = 0,
    INFO_MAGE_GUILD_MAX_LEVEL     = 1,
    INFO_CANNOT_AFFORD_MAGE_LEVEL = 2,
    INFO_ADD_MAGE_GUILD_LEVEL     = 3,
    INFO_ALREADY_BUILT            = 4,
    INFO_CANNOT_BUILD             = 5,
    INFO_CANNOT_AFFORD            = 6,
    INFO_BUILD                    = 7,
    INFO_CANNOT_AFFORD_HERO       = 8,
    INFO_TOO_MANY_HEROES          = 9,
    INFO_TOWN_OCCUPIED            = 10,
    INFO_RECRUIT_HERO             = 11,
    INFO_EXIT                     = 12,
    INFO_OPTIONS                  = 13,
    INFO_GROUPED_FORMATION        = 14,
    INFO_SPREAD_FORMATION         = 15
H2_ENUM_CLASS_END(CastleInfoText)

H2_ENUM_BEGIN(CastleConstant)
    CAPTAIN_STAT_LINE_CAPACITY   = 200,
    INTERFACE_X                  = 18,
    INTERFACE_Y                  = 463,
    INTERFACE_WIDTH              = 604,
    INTERFACE_HEIGHT             = 16,
    BACKGROUND_LEFT              = 458,
    BACKGROUND_TOP               = 0,
    BACKGROUND_TERRAIN_FIRST_ROW = 5,
    BACKGROUND_TERRAIN_LAST_ROW  = 6,
    BACKGROUND_TOWN_FIRST_ROW    = 2,
    BACKGROUND_TOWN_LAST_ROW     = 5,
    BACKGROUND_FIRST_COLUMN      = 4,
    BACKGROUND_LAST_COLUMN       = 8,
    BACKGROUND_TOP_CENTER_COLUMN = 6,
    BACKGROUND_TILE_SIZE         = 32,
    BACKGROUND_FILL_COLOR        = 1,
    EXPANSION_OVERLAY_X          = 149,
    EXPANSION_OVERLAY_Y          = 157,
    EXPANSION_OVERLAY_WIDTH      = 137,
    EXPANSION_OVERLAY_HEIGHT     = 72,
    TERRAIN_ICON_COLUMNS         = 5,
    TERRAIN_ICON_FRAMES          = 2,
    RACE_ICON_FRAMES             = 32
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
    char iconName4[TOWN_OBJECT_FILENAME_SIZE];
    char statLine11[CAPTAIN_STAT_LINE_CAPACITY];
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
                m_town->m_buildState + 1 < TOWN_MAGE_GUILD_MAX_LEVEL ? m_town->m_buildState + 1
                                                                     : TOWN_MAGE_GUILD_MAX_LEVEL
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
        widgetFrame12 = FRAME_NONE;
        if ((m_town->m_buildings & (1L << castleSlotsUse[slot7]))
            && (castleSlotsUse[slot7] != IDX(CASTLE_MAGE_GUILD)
                || m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL)) {
            widgetFrame12 = FRAME_BUILT;
        } else {
            if (!(m_buildableBuildings & (1L << castleSlotsUse[slot7])))
                widgetFrame12 = FRAME_CANNOT_BUILD;
            else if (!(m_affordableBuildings & (1L << castleSlotsUse[slot7])))
                widgetFrame12 = FRAME_CANNOT_AFFORD;
        }

        if (widgetFrame12 != FRAME_NONE) {
            message3.payload.widget.command = CASTLE_WIDGET_ENABLE;
            message3.payload.widget.id = CONTROL_BUILDING_BUTTON_FIRST + slot7;
            message3.payload.widget.data.value = WIDGET_FLAG_DRAW;
            casWin->BroadcastMessage(message3);
            message3.payload.widget.command = CASTLE_WIDGET_FRAME;
            message3.payload.widget.data.value = widgetFrame12;
            casWin->BroadcastMessage(message3);
        } else {
            message3.payload.widget.command = CASTLE_WIDGET_DISABLE;
            message3.payload.widget.data.value = WIDGET_FLAG_DRAW;
            message3.payload.widget.id = CONTROL_BUILDING_BUTTON_FIRST + slot7;
            casWin->BroadcastMessage(message3);
        }

        if (widgetFrame12 == FRAME_BUILT) {
            message3.payload.widget.command = CASTLE_WIDGET_DISABLE;
            message3.payload.widget.data.value = WIDGET_FLAG_DRAW;
            message3.payload.widget.id = CONTROL_BUILDING_OVERLAY_FIRST + slot7;
            casWin->BroadcastMessage(message3);
        } else {
            message3.payload.widget.command = CASTLE_WIDGET_ENABLE;
            message3.payload.widget.data.value = WIDGET_FLAG_DRAW;
            message3.payload.widget.id = CONTROL_BUILDING_OVERLAY_FIRST + slot7;
            casWin->BroadcastMessage(message3);
            message3.payload.widget.command = CASTLE_WIDGET_FRAME;
            message3.payload.widget.data.value =
                widgetFrame12 == FRAME_NONE ? IDX(FRAME_AVAILABLE) : FRAME_UNAVAILABLE;
            casWin->BroadcastMessage(message3);
        }
    }

    captainBuilt = m_town->m_buildings & IDX(TOWN_BUILDING_CAPTAIN_QUARTERS);
    message3.payload.widget.command =
        captainBuilt != 0 ? CASTLE_WIDGET_DISABLE : CASTLE_WIDGET_ENABLE;
    message3.payload.widget.id = CONTROL_CAPTAIN_OVERLAY;
    message3.payload.widget.data.value = WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW;
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
    message3.payload.widget.data.value = WIDGET_FLAG_DRAW;
    casWin->BroadcastMessage(message3);
    if (captainBuilt != 0) {
        message3.payload.widget.command = CASTLE_WIDGET_FRAME;
        message3.payload.widget.id = CONTROL_CAPTAIN_FLAG;
        message3.payload.widget.data.value = gpCurPlayer->m_color;
        casWin->BroadcastMessage(message3);
    }

    widgetFrame12 = FRAME_NONE;
    if (captainBuilt != 0) {
        message3.payload.widget.command = CASTLE_WIDGET_TEXT;
        message3.payload.widget.data.text = gText;
        sprintf(gText, "");
        for (slot7 = 0; slot7 < HERO_PRIMARY_STAT_COUNT; ++slot7) {
            sprintf(statLine11, "%s\n", gStatNames[slot7]);
            strcat(gText, statLine11);
        }
        message3.payload.widget.id = CONTROL_CAPTAIN_STATS;
        casWin->BroadcastMessage(message3);
        sprintf(gText, "");
        for (slot7 = 0; slot7 < HERO_PRIMARY_STAT_COUNT; ++slot7) {
            sprintf(statLine11, "%d\n", captainStats[m_town->m_type][slot7]);
            strcat(gText, statLine11);
        }
        message3.payload.widget.id = CONTROL_CAPTAIN_VALUES;
        casWin->BroadcastMessage(message3);
        message3.payload.widget.command = m_town->m_formation != TOWN_FORMATION_SPREAD
                                              ? CASTLE_WIDGET_DISABLE
                                              : CASTLE_WIDGET_ENABLE;
        message3.payload.widget.id = CONTROL_CAPTAIN_FORMATION_SPREAD_INACTIVE;
        message3.payload.widget.data.value = WIDGET_FLAG_DRAW;
        casWin->BroadcastMessage(message3);
        message3.payload.widget.command = m_town->m_formation == TOWN_FORMATION_SPREAD
                                              ? CASTLE_WIDGET_DISABLE
                                              : CASTLE_WIDGET_ENABLE;
        message3.payload.widget.id = CONTROL_CAPTAIN_FORMATION_GROUPED_INACTIVE;
        message3.payload.widget.data.value = WIDGET_FLAG_DRAW;
        casWin->BroadcastMessage(message3);
    } else {
        if (!CanBuild(m_town, CASTLE_CAPTAIN))
            widgetFrame12 = FRAME_CANNOT_BUILD;
        else if (!CanBuy(m_town, CASTLE_CAPTAIN))
            widgetFrame12 = FRAME_CANNOT_AFFORD;
        if (CanBuild(m_town, CASTLE_CAPTAIN))
            m_buildableBuildings |= IDX(TOWN_BUILDING_CAPTAIN_QUARTERS);
        if (CanBuy(m_town, CASTLE_CAPTAIN))
            m_affordableBuildings |= IDX(TOWN_BUILDING_CAPTAIN_QUARTERS);
    }

    message3.payload.widget.command =
        widgetFrame12 == FRAME_NONE ? CASTLE_WIDGET_DISABLE : CASTLE_WIDGET_ENABLE;
    message3.payload.widget.id = CONTROL_CAPTAIN_BUTTON;
    message3.payload.widget.data.value = WIDGET_FLAG_DRAW;
    casWin->BroadcastMessage(message3);
    if (widgetFrame12 != FRAME_NONE) {
        message3.payload.widget.command = CASTLE_WIDGET_FRAME;
        message3.payload.widget.data.value = widgetFrame12;
        casWin->BroadcastMessage(message3);
    }

    if (gpCurPlayer->m_resources[IDX(RES_GOLD)] < gHeroGoldCost)
        widgetFrame12 = FRAME_CANNOT_AFFORD;
    else if (gpCurPlayer->m_heroCount == PLAYER_HERO_CAPACITY || m_town->m_occupyingHeroId != -1)
        widgetFrame12 = FRAME_CANNOT_BUILD;
    else if (m_recruitResult != 0)
        widgetFrame12 = FRAME_BUILT;
    else
        widgetFrame12 = FRAME_NONE;

    for (slot7 = 0; slot7 < PLAYER_AVAILABLE_HERO_COUNT; ++slot7) {
        message3.payload.widget.data.value = WIDGET_FLAG_DRAW;
        message3.payload.widget.id = CONTROL_RECRUIT_FIRST + slot7;
        if (widgetFrame12 != FRAME_NONE) {
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
    raceIconFrame = m_town->m_type * RACE_ICON_FRAMES;
    if (updateOnly == 0) {
        backgroundFrame6 = 0;
        for (row9 = BACKGROUND_TERRAIN_FIRST_ROW; row9 <= BACKGROUND_TERRAIN_LAST_ROW; ++row9) {
            for (column8 = BACKGROUND_FIRST_COLUMN; column8 <= BACKGROUND_LAST_COLUMN; ++column8) {
                backgroundWidget19 = new iconWidget(
                    static_cast<i16>(
                        (column8 - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
                    ),
                    static_cast<i16>((row9 - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
                    BACKGROUND_TILE_SIZE,
                    BACKGROUND_TILE_SIZE,
                    "objntwba.icn",
                    static_cast<i16>(OD_STEER(terrainIconFrame27) + backgroundFrame6),
                    0,
                    TOWN_WIDGET_ID_NONE,
                    WIDGET_KIND_ICON_DIRECT,
                    BACKGROUND_FILL_COLOR
                );
                if (backgroundWidget19 == NULL)
                    MemError();
                casWin->AddWidget(backgroundWidget19, TOWN_WIDGET_INSERT_DEFAULT);
                ++backgroundFrame6;
            }
        }
        backgroundFrame6 = 0;
        for (row9 = BACKGROUND_TOWN_FIRST_ROW; row9 <= BACKGROUND_TOWN_LAST_ROW; ++row9) {
            for (column8 = BACKGROUND_FIRST_COLUMN; column8 <= BACKGROUND_LAST_COLUMN; ++column8) {
                if (row9 == BACKGROUND_TOWN_FIRST_ROW && column8 != BACKGROUND_TOP_CENTER_COLUMN) {
                    continue;
                }
                backgroundWidget19 = new iconWidget(
                    static_cast<i16>(
                        (column8 - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
                    ),
                    static_cast<i16>((row9 - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
                    BACKGROUND_TILE_SIZE,
                    BACKGROUND_TILE_SIZE,
                    "objntown.icn",
                    static_cast<i16>(OD_STEER(raceIconFrame) + backgroundFrame6),
                    0,
                    TOWN_WIDGET_ID_NONE,
                    WIDGET_KIND_ICON_DIRECT,
                    BACKGROUND_FILL_COLOR
                );
                if (backgroundWidget19 == NULL)
                    MemError();
                casWin->AddWidget(backgroundWidget19, TOWN_WIDGET_INSERT_DEFAULT);
                ++backgroundFrame6;
            }
        }
        if (xIsExpansionMap == 0 && m_town->m_type == IDX(FACTION_NECROMANCER)) {
            backgroundWidget19 = new iconWidget(
                EXPANSION_OVERLAY_X,
                EXPANSION_OVERLAY_Y,
                EXPANSION_OVERLAY_WIDTH,
                EXPANSION_OVERLAY_HEIGHT,
                "caslxtra.icn",
                0,
                0,
                TOWN_WIDGET_ID_NONE,
                WIDGET_KIND_ICON_DIRECT,
                BACKGROUND_FILL_COLOR
            );
            if (backgroundWidget19 == NULL)
                MemError();
            casWin->AddWidget(backgroundWidget19, TOWN_WIDGET_INSERT_DEFAULT);
        }
    }
}

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
    buildingIndex = TOWN_SELECTED_BUILDING_NONE;
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
                     && message.payload.widget.id
                            < CONTROL_BUILDING_ICON_FIRST + static_cast<i32>(CASTLE_SLOT_COUNT))
                buildingIndex = message.payload.widget.id - CONTROL_BUILDING_ICON_FIRST;
            else if (message.payload.widget.id >= CONTROL_BUILDING_BUTTON_FIRST
                     && message.payload.widget.id
                            < CONTROL_BUILDING_BUTTON_FIRST + static_cast<i32>(CASTLE_SLOT_COUNT))
                buildingIndex = message.payload.widget.id - CONTROL_BUILDING_BUTTON_FIRST;
            if (buildingIndex != TOWN_SELECTED_BUILDING_NONE)
                buildingIndex = castleSlotsUse[buildingIndex];
        }
    }

    if (hoverMessage) {
        if (message.payload.widget.id == gpTownManager->m_lastHoverId)
            return EVENT_WINDOW_CONTINUE;
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
                        loopIndex = IDX(INFO_BUILD_MAGE_GUILD);
                    else if (gpTownManager->m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL)
                        loopIndex = IDX(INFO_MAGE_GUILD_MAX_LEVEL);
                    else if (!CanBuy(gpTownManager->m_town, CASTLE_MAGE_GUILD))
                        loopIndex = IDX(INFO_CANNOT_AFFORD_MAGE_LEVEL);
                    else
                        loopIndex = IDX(INFO_ADD_MAGE_GUILD_LEVEL);
                    strcpy(gText, cCastleInfo[loopIndex]);
                }
                break;

            case BUILDING_SLOT_SPECIAL_ONE:
            case BUILDING_SLOT_NECROMANCER_SHRINE:
            case BUILDING_SLOT_DOCK:
            case BUILDING_SLOT_SPECIAL_FOUR:
            case BUILDING_SLOT_SPECIAL_SEVEN:
            case BUILDING_SLOT_SPECIAL_EIGHT:
            case BUILDING_SLOT_SPECIAL_NINE:
            case BUILDING_SLOT_SPECIAL_TEN:
            case BUILDING_SLOT_WELL_EXTRA:
            case BUILDING_SLOT_SPECIAL_TWELVE:
            case BUILDING_SLOT_SPECIAL:
            case BUILDING_SLOT_NEUTRAL_LAST:
            case BUILDING_SLOT_DWELLING_FIRST:
            case BUILDING_SLOT_DWELLING_SECOND:
            case BUILDING_SLOT_DWELLING_THIRD:
            case BUILDING_SLOT_DWELLING_FOURTH:
            case BUILDING_SLOT_DWELLING_FIFTH:
            case BUILDING_SLOT_DWELLING_SIXTH:
            case BUILDING_SLOT_UPGRADE_FIRST:
            case BUILDING_SLOT_UPGRADE_SECOND:
            case BUILDING_SLOT_UPGRADE_THIRD:
            case BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE:
            case BUILDING_SLOT_SPECIAL_TWENTY_NINE:
            case BUILDING_SLOT_SPECIAL_THIRTY:
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

            case TOWN_SELECTED_BUILDING_NONE:
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
                        } else if (gpCurPlayer->m_heroCount == PLAYER_HERO_CAPACITY) {
                            sprintf(
                                gText,
                                cCastleInfo[IDX(INFO_TOO_MANY_HEROES)],
                                PLAYER_HERO_CAPACITY
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
        gpTownManager->m_heroWindow0->DrawWindow(0, CONTROL_STATUS_FIRST, CONTROL_STATUS_TEXT);
        gpWindowManager
            ->UpdateScreenRegion(INTERFACE_X, INTERFACE_Y, INTERFACE_WIDTH, INTERFACE_HEIGHT);
        return EVENT_WINDOW_CONTINUE;
    }

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                result = message.payload.widget.id == CONTROL_CLOSE;
                break;
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                quickFlag = (message.payload.widget.parameter & MESSAGE_MODIFIER_LEFT_SHIFT) != 0;
                switch (buildingIndex) {
                    case CONTROL_CAPTAIN_FORMATION_SPREAD:
                        if (quickFlag) {
                            NormalDialog(
                                "{Spread Formation}\n\n'Spread' combat formation spreads your "
                                "armies from the top to the bottom of the battlefield, with at "
                                "least one empty space between each army.",
                                NORMAL_DIALOG_QUICK_VIEW,
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
                                NORMAL_DIALOG_QUICK_VIEW,
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
                            || gpTownManager->m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL
                            || (gpTownManager->m_buildableBuildings & (1L << buildingIndex)))
                            goto buy_building;
                        break;

                    case BUILDING_SLOT_SPECIAL_ONE:
                    case BUILDING_SLOT_NECROMANCER_SHRINE:
                    case BUILDING_SLOT_DOCK:
                    case BUILDING_SLOT_SPECIAL_FOUR:
                    case BUILDING_SLOT_SPECIAL_SEVEN:
                    case BUILDING_SLOT_SPECIAL_EIGHT:
                    case BUILDING_SLOT_SPECIAL_NINE:
                    case BUILDING_SLOT_SPECIAL_TEN:
                    case BUILDING_SLOT_WELL_EXTRA:
                    case BUILDING_SLOT_SPECIAL_TWELVE:
                    case BUILDING_SLOT_SPECIAL:
                    case BUILDING_SLOT_NEUTRAL_LAST:
                    case BUILDING_SLOT_DWELLING_FIRST:
                    case BUILDING_SLOT_DWELLING_SECOND:
                    case BUILDING_SLOT_DWELLING_THIRD:
                    case BUILDING_SLOT_DWELLING_FOURTH:
                    case BUILDING_SLOT_DWELLING_FIFTH:
                    case BUILDING_SLOT_DWELLING_SIXTH:
                    case BUILDING_SLOT_UPGRADE_FIRST:
                    case BUILDING_SLOT_UPGRADE_SECOND:
                    case BUILDING_SLOT_UPGRADE_THIRD:
                    case BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE:
                    case BUILDING_SLOT_SPECIAL_TWENTY_NINE:
                    case BUILDING_SLOT_SPECIAL_THIRTY:
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

                    case TOWN_SELECTED_BUILDING_NONE:
                        switch (message.payload.widget.id) {
                            case IDX(CONTROL_HERO_FIRST):
                            case CONTROL_HERO_FIRST + 1:
                                heroChoiceIndex = message.payload.widget.id - CONTROL_HERO_FIRST;
                                goto hero_selected;
                            case IDX(CONTROL_RECRUIT_FIRST):
                            case CONTROL_RECRUIT_FIRST + 1:
                                heroChoiceIndex = message.payload.widget.id - CONTROL_RECRUIT_FIRST;
                            hero_selected:
                                if (quickFlag) {
                                    HeroView(gpCurPlayer->AvailableHeroId(heroChoiceIndex), 1, 0);
                                    casWin->DrawWindow();
                                    gpTownManager->m_bankBox->Update(1);
                                    gpWindowManager
                                        ->FadeScreen(TOWN_FADE_IN, TOWN_FADE_STEPS, NULL);
                                } else {
                                    cannotRecruitHero =
                                        gpTownManager->m_recruitResult != 0
                                        || gpCurPlayer->m_resources[IDX(RES_GOLD)] < gHeroGoldCost
                                        || gpCurPlayer->m_heroCount >= PLAYER_HERO_CAPACITY
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
        return EVENT_WINDOW_CONTINUE;
    message.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
    message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
    return EVENT_WINDOW_CLOSE;
}

// Retail castle building-slot order payload.
// NOLINTBEGIN(readability-magic-numbers)
DATA(0x004ef5e0) u8 castleSlotsBase[CASTLE_SLOT_COUNT] =
    {19, 20, 21, 22, 23, 24, 0, 2, 1, 3, 7, 10, 4, 11, 13, 8, 9, 12};
// NOLINTEND(readability-magic-numbers)
DATA(0x00525040) heroWindow* casWin;
DATA(0x00525048) u8 castleSlotsUse[CASTLE_SLOT_COUNT];
