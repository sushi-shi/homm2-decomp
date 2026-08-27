#include <Ints.h>
#include <BASE/Utf8.h>
#include <BASE/message.h>
#include <BASE/BITS.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/widgetKind.h>
#include <EDITOR/fullMap.h>
#include <EDITOR/mapcell.h>
#include <IRONFIST/hooks.h>
#include <IRONFIST/townconsts.h>
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
#include <SOURCE/Localization.h>


typedef enum CastleControl {
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
} CastleControl;

typedef enum CastleWidgetFrame {
    FRAME_NONE          = -1,
    FRAME_AVAILABLE     = 1,
    FRAME_UNAVAILABLE   = 2,
    FRAME_DISALLOWED    = 3,
    FRAME_BUILT         = 11,
    FRAME_CANNOT_BUILD  = 12,
    FRAME_CANNOT_AFFORD = 13
} CastleWidgetFrame;

enum class CastleInfoText : i32 {
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
};
using enum CastleInfoText;

typedef enum CastleConstant {
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
} CastleConstant;

namespace {

    inline i32 CannotRecruitHero(void) {
        b32 cannot;

        if (!(gpTownManager->m_recruitResult != 0
              || gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] < gHeroGoldCost
              || gpCurPlayer->m_heroCount >= PLAYER_HERO_CAPACITY
              || gpTownManager->m_town->m_occupyingHeroId != -1))
            cannot = false;
        else
            cannot = true;
        return cannot;
    }

}

void townManager::SetupCastle(heroWindow* window, i32 updateOnly) {
    u32l captainQuarters;

    i32 terrainIconFrame;
    i32 column;
    i32 rowPos;
    widget* backgroundWidget;
    i32 raceBase;

    i32 slotNum;
    char icnName[TOWN_OBJECT_FILENAME_SIZE];
    i32 backFrame;

    char captainStatLine[CAPTAIN_STAT_LINE_CAPACITY];
    i32 stateFrame;
    tag_message msg;

    casWin = window;

    for (slotNum = 0; slotNum < CASTLE_SLOT_COUNT; ++slotNum) {
        castleSlotsUse[slotNum] = castleSlotsBase[slotNum];
        if (castleSlotsBase[slotNum] >= BUILDING_SLOT_DWELLING_SECOND
            && castleSlotsBase[slotNum] <= BUILDING_SLOT_DWELLING_SIXTH
            && ((m_town->m_buildings & (1L << H2EnumIndex(castleSlotsBase[slotNum])))
                || (m_town->m_buildings
                    & (1L << (H2EnumIndex(castleSlotsBase[slotNum]) + CASTLE_UPGRADE_OFFSET)))
                || (castleSlotsBase[slotNum] == BUILDING_SLOT_DWELLING_SIXTH
                    && m_town->m_type == FACTION_WARLOCK
                    && (m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6))))
            && (gTownEligibleBuildMask[H2EnumIndex(m_town->m_type)]
                & (1L << (H2EnumIndex(castleSlotsBase[slotNum]) + CASTLE_UPGRADE_OFFSET)))) {
            if (castleSlotsBase[slotNum] == BUILDING_SLOT_DWELLING_SIXTH
                && m_town->m_type == FACTION_WARLOCK
                && ((m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_UPGRADED_DWELLING_6))
                    || (m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6)))) {
                castleSlotsUse[slotNum] = BUILDING_SLOT_DWELLING_LAST;
            } else {
                castleSlotsUse[slotNum] = castleSlotsBase[slotNum] + CASTLE_UPGRADE_OFFSET;
            }
        }
    }

    m_affordableBuildings = m_buildableBuildings = 0;
    for (slotNum = 0; slotNum < CASTLE_SLOT_COUNT; ++slotNum) {
        if (CanBuy(m_town, castleSlotsUse[slotNum]))
            m_affordableBuildings |= 1L << H2EnumIndex(castleSlotsUse[slotNum]);
        if (CanBuild(m_town, castleSlotsUse[slotNum]))
            m_buildableBuildings |= 1L << H2EnumIndex(castleSlotsUse[slotNum]);
    }

    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.command = CASTLE_WIDGET_FRAME;
    for (slotNum = 0; slotNum < CASTLE_SLOT_COUNT; ++slotNum) {
        msg.payload.widget.id = CONTROL_BUILDING_ICON_FIRST + slotNum;
        msg.payload.widget.data.value = H2EnumIndex(castleSlotsUse[slotNum]);
        casWin->BroadcastMessage(msg);
    }

    msg.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
    sprintf(
        icnName,
        "cstl%s.icn",
        cHeroTypeShortName[H2EnumIndex(m_town->m_type)]
    );
    msg.payload.widget.data.text = icnName;
    for (slotNum = 0; slotNum < CASTLE_SLOT_COUNT; ++slotNum) {
        msg.payload.widget.id = CONTROL_BUILDING_ICON_FIRST + slotNum;
        casWin->BroadcastMessage(msg);
    }

    msg.payload.widget.command = CASTLE_WIDGET_TEXT;
    for (slotNum = 0; slotNum < CASTLE_SLOT_COUNT; ++slotNum) {
        msg.payload.widget.id = CONTROL_BUILDING_NAME_FIRST + slotNum;
        if (castleSlotsUse[slotNum] == CASTLE_MAGE_GUILD) {
            sprintf(
                gText,
                localization::Tr(
                    m_town->m_type == FACTION_CYBORG ? "castle.cybernetics_lab.level"
                                                     : "castle.mage_guild.level"
                ),
                m_town->m_buildState + 1 < TOWN_MAGE_GUILD_MAX_LEVEL ? m_town->m_buildState + 1
                                                                     : TOWN_MAGE_GUILD_MAX_LEVEL
            );
            msg.payload.widget.data.text = gText;
        } else {
            msg.payload.widget.data.text = GetBuildingName(
                m_town->m_type,
                castleSlotsUse[slotNum]
            );
        }
        casWin->BroadcastMessage(msg);
    }

    for (slotNum = 0; slotNum < CASTLE_SLOT_COUNT; ++slotNum) {
        i32 disallowed =
            m_town->IsBuildingDisallowed(H2EnumIndex(castleSlotsUse[slotNum]));
        stateFrame = FRAME_NONE;
        if (disallowed) {
            stateFrame = FRAME_CANNOT_BUILD;
        } else if ((m_town->m_buildings & (1L << H2EnumIndex(castleSlotsUse[slotNum])))
            && (castleSlotsUse[slotNum] != CASTLE_MAGE_GUILD
                || m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL)) {
            stateFrame = FRAME_BUILT;
        } else {
            if (!(m_buildableBuildings & (1L << H2EnumIndex(castleSlotsUse[slotNum]))))
                stateFrame = FRAME_CANNOT_BUILD;
            else if (!(m_affordableBuildings & (1L << H2EnumIndex(castleSlotsUse[slotNum]))))
                stateFrame = FRAME_CANNOT_AFFORD;
        }

        if (stateFrame != FRAME_NONE) {
            msg.payload.widget.command = CASTLE_WIDGET_ENABLE;
            msg.payload.widget.id = CONTROL_BUILDING_BUTTON_FIRST + slotNum;
            msg.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            casWin->BroadcastMessage(msg);
            msg.payload.widget.command = CASTLE_WIDGET_FRAME;
            msg.payload.widget.data.value = stateFrame;
            casWin->BroadcastMessage(msg);
        } else {
            msg.payload.widget.command = CASTLE_WIDGET_DISABLE;
            msg.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            msg.payload.widget.id = CONTROL_BUILDING_BUTTON_FIRST + slotNum;
            casWin->BroadcastMessage(msg);
        }

        if (stateFrame == FRAME_BUILT) {
            msg.payload.widget.command = CASTLE_WIDGET_DISABLE;
            msg.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            msg.payload.widget.id = CONTROL_BUILDING_OVERLAY_FIRST + slotNum;
            casWin->BroadcastMessage(msg);
        } else {
            msg.payload.widget.command = CASTLE_WIDGET_ENABLE;
            msg.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            msg.payload.widget.id = CONTROL_BUILDING_OVERLAY_FIRST + slotNum;
            casWin->BroadcastMessage(msg);
            msg.payload.widget.command = CASTLE_WIDGET_FRAME;
            msg.payload.widget.data.value = disallowed
                ? H2EnumIndex(FRAME_DISALLOWED)
                : (stateFrame == FRAME_NONE ? H2EnumIndex(FRAME_AVAILABLE) : FRAME_UNAVAILABLE);
            casWin->BroadcastMessage(msg);
        }
    }

    captainQuarters = m_town->m_buildings & H2EnumIndex(TOWN_BUILDING_CAPTAIN_QUARTERS);
    msg.payload.widget.command =
        captainQuarters != 0 ? CASTLE_WIDGET_DISABLE : CASTLE_WIDGET_ENABLE;
    msg.payload.widget.id = CONTROL_CAPTAIN_OVERLAY;
    msg.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
    casWin->BroadcastMessage(msg);
    msg.payload.widget.command = CASTLE_WIDGET_FRAME;
    msg.payload.widget.id = CONTROL_CAPTAIN_ICON;
    msg.payload.widget.data.value = captainQuarters != 0;
    casWin->BroadcastMessage(msg);
    sprintf(
        gText,
        "CSTLCAP%c.ICN",
        cHeroTypeInitial[H2EnumIndex(m_town->m_type)]
    );
    msg.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
    msg.payload.widget.id = CONTROL_CAPTAIN_ICON;
    msg.payload.widget.data.text = gText;
    casWin->BroadcastMessage(msg);
    msg.payload.widget.command =
        captainQuarters != 0 ? CASTLE_WIDGET_ENABLE : CASTLE_WIDGET_DISABLE;
    msg.payload.widget.id = CONTROL_CAPTAIN_FLAG;
    msg.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
    casWin->BroadcastMessage(msg);
    if (captainQuarters != 0) {
        msg.payload.widget.command = CASTLE_WIDGET_FRAME;
        msg.payload.widget.id = CONTROL_CAPTAIN_FLAG;
        msg.payload.widget.data.value = gpCurPlayer->m_color;
        casWin->BroadcastMessage(msg);
    }

    stateFrame = FRAME_NONE;
    if (captainQuarters != 0) {
        msg.payload.widget.command = CASTLE_WIDGET_TEXT;
        msg.payload.widget.data.text = gText;
        gText[0] = 0;
        for (slotNum = 0; slotNum < HERO_PRIMARY_STAT_COUNT; ++slotNum) {
            sprintf(
                captainStatLine,
                "%s\n",
                gStatNames[slotNum]
            );
            strcat(gText, captainStatLine);
        }
        msg.payload.widget.id = CONTROL_CAPTAIN_STATS;
        casWin->BroadcastMessage(msg);
        gText[0] = 0;
        for (slotNum = 0; slotNum < HERO_PRIMARY_STAT_COUNT; ++slotNum) {
            sprintf(
                captainStatLine,
                "%d\n",
                captainStats[H2EnumIndex(m_town->m_type)][slotNum]
            );
            strcat(gText, captainStatLine);
        }
        msg.payload.widget.id = CONTROL_CAPTAIN_VALUES;
        casWin->BroadcastMessage(msg);
        msg.payload.widget.command = m_town->m_formation != TOWN_FORMATION_SPREAD
                                              ? CASTLE_WIDGET_DISABLE
                                              : CASTLE_WIDGET_ENABLE;
        msg.payload.widget.id = CONTROL_CAPTAIN_FORMATION_SPREAD_INACTIVE;
        msg.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        casWin->BroadcastMessage(msg);
        msg.payload.widget.command = m_town->m_formation == TOWN_FORMATION_SPREAD
                                              ? CASTLE_WIDGET_DISABLE
                                              : CASTLE_WIDGET_ENABLE;
        msg.payload.widget.id = CONTROL_CAPTAIN_FORMATION_GROUPED_INACTIVE;
        msg.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        casWin->BroadcastMessage(msg);
    } else {
        if (!CanBuild(m_town, CASTLE_CAPTAIN))
            stateFrame = FRAME_CANNOT_BUILD;
        else if (!CanBuy(m_town, CASTLE_CAPTAIN))
            stateFrame = FRAME_CANNOT_AFFORD;
        if (CanBuild(m_town, CASTLE_CAPTAIN))
            m_buildableBuildings |= H2EnumIndex(TOWN_BUILDING_CAPTAIN_QUARTERS);
        if (CanBuy(m_town, CASTLE_CAPTAIN))
            m_affordableBuildings |= H2EnumIndex(TOWN_BUILDING_CAPTAIN_QUARTERS);
    }

    msg.payload.widget.command =
        stateFrame == FRAME_NONE ? CASTLE_WIDGET_DISABLE : CASTLE_WIDGET_ENABLE;
    msg.payload.widget.id = CONTROL_CAPTAIN_BUTTON;
    msg.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
    casWin->BroadcastMessage(msg);
    if (stateFrame != FRAME_NONE) {
        msg.payload.widget.command = CASTLE_WIDGET_FRAME;
        msg.payload.widget.data.value = stateFrame;
        casWin->BroadcastMessage(msg);
    }

    if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] < gHeroGoldCost)
        stateFrame = FRAME_CANNOT_AFFORD;
    else if (gpCurPlayer->m_heroCount == PLAYER_HERO_CAPACITY || m_town->m_occupyingHeroId != -1)
        stateFrame = FRAME_CANNOT_BUILD;
    else if (m_recruitResult != 0)
        stateFrame = FRAME_BUILT;
    else
        stateFrame = FRAME_NONE;

    for (slotNum = 0; slotNum < PLAYER_AVAILABLE_HERO_COUNT; ++slotNum) {
        msg.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        msg.payload.widget.id = CONTROL_RECRUIT_FIRST + slotNum;
        if (stateFrame != FRAME_NONE) {
            msg.payload.widget.command = CASTLE_WIDGET_ENABLE;
            casWin->BroadcastMessage(msg);
            msg.payload.widget.command = CASTLE_WIDGET_FRAME;
            msg.payload.widget.data.value = stateFrame;
            casWin->BroadcastMessage(msg);
        } else {
            msg.payload.widget.command = CASTLE_WIDGET_DISABLE;
            casWin->BroadcastMessage(msg);
        }
        msg.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
        sprintf(
            icnName,
            "port%04d.icn",
            H2EnumIndex(gpGame->m_heroRecs[gpCurPlayer->AvailableHeroId(slotNum)].m_portrait)
        );
        msg.payload.widget.data.text = icnName;
        msg.payload.widget.id = CONTROL_HERO_FIRST + slotNum;
        casWin->BroadcastMessage(msg);
    }

    terrainIconFrame = (H2EnumIndex(giGroundToTerrain
                                  [gpGame->m_worldMap.GetCell(m_town->m_x, m_town->m_y)
                                       ->m_terrainImageIndex])
                          - 1)
                         * (TERRAIN_ICON_COLUMNS * TERRAIN_ICON_FRAMES);
    // Cyborg towns use sprite slot 6, following the OBJNTOWN.ICN order.
    raceBase =
        (m_town->m_type == FACTION_CYBORG ? ironfist::CYBORG_SPRITE_SLOT
         : H2EnumIndex(m_town->m_type))
        * RACE_ICON_FRAMES;
    if (updateOnly == 0) {
        backFrame = 0;
        for (rowPos = BACKGROUND_TERRAIN_FIRST_ROW; rowPos <= BACKGROUND_TERRAIN_LAST_ROW; ++rowPos) {
            for (column = BACKGROUND_FIRST_COLUMN; column <= BACKGROUND_LAST_COLUMN; ++column) {
                backgroundWidget = new iconWidget(
                    static_cast<i16>(
                        (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
                    ),
                    static_cast<i16>((rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
                    BACKGROUND_TILE_SIZE,
                    BACKGROUND_TILE_SIZE,
                    "objntwba.icn",
                    static_cast<i16>(terrainIconFrame + backFrame),
                    ICON_DRAW_NORMAL,
                    TOWN_WIDGET_ID_NONE,
                    WIDGET_KIND_ICON_DIRECT,
                    BACKGROUND_FILL_COLOR
                );
                if (backgroundWidget == NULL)
                    MemError();
                casWin->AddWidget(backgroundWidget, TOWN_WIDGET_INSERT_DEFAULT);
                ++backFrame;
            }
        }
        backFrame = 0;
        for (rowPos = BACKGROUND_TOWN_FIRST_ROW; rowPos <= BACKGROUND_TOWN_LAST_ROW; ++rowPos) {
            for (column = BACKGROUND_FIRST_COLUMN; column <= BACKGROUND_LAST_COLUMN; ++column) {
                if (rowPos == BACKGROUND_TOWN_FIRST_ROW && column != BACKGROUND_TOP_CENTER_COLUMN) {
                    continue;
                }
                backgroundWidget = new iconWidget(
                    static_cast<i16>(
                        (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
                    ),
                    static_cast<i16>((rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
                    BACKGROUND_TILE_SIZE,
                    BACKGROUND_TILE_SIZE,
                    "objntown.icn",
                    static_cast<i16>(raceBase + backFrame),
                    ICON_DRAW_NORMAL,
                    TOWN_WIDGET_ID_NONE,
                    WIDGET_KIND_ICON_DIRECT,
                    BACKGROUND_FILL_COLOR
                );
                if (backgroundWidget == NULL)
                    MemError();
                casWin->AddWidget(backgroundWidget, TOWN_WIDGET_INSERT_DEFAULT);
                ++backFrame;
            }
        }
        if (xIsExpansionMap == 0 && m_town->m_type == FACTION_NECROMANCER) {
            backgroundWidget = new iconWidget(
                EXPANSION_OVERLAY_X,
                EXPANSION_OVERLAY_Y,
                EXPANSION_OVERLAY_WIDTH,
                EXPANSION_OVERLAY_HEIGHT,
                "caslxtra.icn",
                0,
                ICON_DRAW_NORMAL,
                TOWN_WIDGET_ID_NONE,
                WIDGET_KIND_ICON_DIRECT,
                BACKGROUND_FILL_COLOR
            );
            if (backgroundWidget == NULL)
                MemError();
            casWin->AddWidget(backgroundWidget, TOWN_WIDGET_INSERT_DEFAULT);
        }
    }
}

MessageDispatchResult CastleHandler(tag_message& message) {
    i32 objIndex;
    b32 hoverMessage;
    i32 heroChoiceIndex;
    i32 whichBuilding;
    b32 quickFlag;
    i32 ret;

    whichBuilding = H2EnumIndex(BUILDING_SLOT_NONE);
    ret = 0;
    hoverMessage = false;

    if (message.type == MESSAGE_MOUSE_MOVE || message.type == MESSAGE_WIDGET) {
        if (message.type == MESSAGE_MOUSE_MOVE) {
            gpWindowManager->ConvertToHover(message);
            hoverMessage = true;
        }
        if (message.payload.widget.id == CONTROL_CAPTAIN_ICON)
            whichBuilding = H2EnumIndex(CASTLE_CAPTAIN);
        else if (message.payload.widget.id == CONTROL_CAPTAIN_FORMATION_GROUPED)
            whichBuilding = CONTROL_CAPTAIN_FORMATION_GROUPED;
        else if (message.payload.widget.id == CONTROL_CAPTAIN_FORMATION_SPREAD)
            whichBuilding = CONTROL_CAPTAIN_FORMATION_SPREAD;
        else {
            if (message.payload.widget.id >= CONTROL_BUILDING_NAME_FIRST
                && message.payload.widget.id
                       < CONTROL_BUILDING_NAME_FIRST + static_cast<i32>(CASTLE_SLOT_COUNT))
                whichBuilding = message.payload.widget.id - CONTROL_BUILDING_NAME_FIRST;
            else if (message.payload.widget.id >= CONTROL_BUILDING_ICON_FIRST
                     && message.payload.widget.id
                            < CONTROL_BUILDING_ICON_FIRST + static_cast<i32>(CASTLE_SLOT_COUNT))
                whichBuilding = message.payload.widget.id - CONTROL_BUILDING_ICON_FIRST;
            else if (message.payload.widget.id >= CONTROL_BUILDING_BUTTON_FIRST
                     && message.payload.widget.id
                            < CONTROL_BUILDING_BUTTON_FIRST + static_cast<i32>(CASTLE_SLOT_COUNT))
                whichBuilding = message.payload.widget.id - CONTROL_BUILDING_BUTTON_FIRST;
            if (whichBuilding != H2EnumIndex(BUILDING_SLOT_NONE))
                whichBuilding = H2EnumIndex(castleSlotsUse[whichBuilding]);
        }
    }

    if (hoverMessage) {
        if (gpTownManager->m_lastHoverId == message.payload.widget.id)
            return MESSAGE_DISPATCH_CONSUME;
        gpTownManager->m_lastHoverId = message.payload.widget.id;
        switch (whichBuilding) {

            case CONTROL_CAPTAIN_FORMATION_GROUPED:
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cCastleInfo[H2EnumIndex(INFO_GROUPED_FORMATION)]);
                break;

            case CONTROL_CAPTAIN_FORMATION_SPREAD:
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cCastleInfo[H2EnumIndex(INFO_SPREAD_FORMATION)]);
                break;

            case H2EnumIndex(TOWN_OBJECT_MAGE_GUILD):
                if (!(gpTownManager->m_buildableBuildings & (1 << whichBuilding))) {
                    sprintf(
                        gText,
                        cCastleInfo[H2EnumIndex(INFO_CANNOT_BUILD)],
                        GetBuildingName(
                            gpTownManager->m_town->m_type,
                            static_cast<BuildingSlotType>(whichBuilding)
                        )
                    );
                } else if (!(gpTownManager->m_affordableBuildings & (1 << whichBuilding))) {
                    sprintf(
                        gText,
                        cCastleInfo[H2EnumIndex(INFO_CANNOT_AFFORD)],
                        GetBuildingName(
                            gpTownManager->m_town->m_type,
                            static_cast<BuildingSlotType>(whichBuilding)
                        )
                    );
                } else {
                    if (!(gpTownManager->m_town->m_buildings & 1L))
                        objIndex = H2EnumIndex(INFO_BUILD_MAGE_GUILD);
                    else if (gpTownManager->m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL)
                        objIndex = H2EnumIndex(INFO_MAGE_GUILD_MAX_LEVEL);
                    else if (!CanBuy(gpTownManager->m_town, CASTLE_MAGE_GUILD))
                        objIndex = H2EnumIndex(INFO_CANNOT_AFFORD_MAGE_LEVEL);
                    else
                        objIndex = H2EnumIndex(INFO_ADD_MAGE_GUILD_LEVEL);
                    if (gpTownManager->m_town->m_type == FACTION_CYBORG
                        && objIndex == H2EnumIndex(INFO_BUILD_MAGE_GUILD))
                        strcpy(gText, localization::Tr("castle.cybernetics_lab.build"));
                    else if (gpTownManager->m_town->m_type == FACTION_CYBORG
                             && objIndex == H2EnumIndex(INFO_ADD_MAGE_GUILD_LEVEL))
                        strcpy(gText, localization::Tr("castle.cybernetics_lab.add_level"));
                    else
                        strcpy(gText, cCastleInfo[objIndex]);
                }
                break;

            case H2EnumIndex(BUILDING_SLOT_SPECIAL_ONE):
            case H2EnumIndex(BUILDING_SLOT_NECROMANCER_SHRINE):
            case H2EnumIndex(BUILDING_SLOT_DOCK):
            case H2EnumIndex(BUILDING_SLOT_SPECIAL_FOUR):
            case H2EnumIndex(BUILDING_SLOT_SPECIAL_SEVEN):
            case H2EnumIndex(BUILDING_SLOT_SPECIAL_EIGHT):
            case H2EnumIndex(BUILDING_SLOT_SPECIAL_NINE):
            case H2EnumIndex(BUILDING_SLOT_SPECIAL_TEN):
            case H2EnumIndex(BUILDING_SLOT_WELL_EXTRA):
            case H2EnumIndex(BUILDING_SLOT_SPECIAL_TWELVE):
            case H2EnumIndex(BUILDING_SLOT_SPECIAL):
            case H2EnumIndex(BUILDING_SLOT_NEUTRAL_LAST):
            case H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST):
            case H2EnumIndex(BUILDING_SLOT_DWELLING_SECOND):
            case H2EnumIndex(BUILDING_SLOT_DWELLING_THIRD):
            case H2EnumIndex(BUILDING_SLOT_DWELLING_FOURTH):
            case H2EnumIndex(BUILDING_SLOT_DWELLING_FIFTH):
            case H2EnumIndex(BUILDING_SLOT_DWELLING_SIXTH):
            case H2EnumIndex(BUILDING_SLOT_UPGRADE_FIRST):
            case H2EnumIndex(BUILDING_SLOT_UPGRADE_SECOND):
            case H2EnumIndex(BUILDING_SLOT_UPGRADE_THIRD):
            case H2EnumIndex(BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE):
            case H2EnumIndex(BUILDING_SLOT_SPECIAL_TWENTY_NINE):
            case H2EnumIndex(BUILDING_SLOT_SPECIAL_THIRTY):
                if (H2BitTest(gpGame->m_dailyEventFlags, gpTownManager->m_town->m_id)) {
                    utf8::Copy(
                        gText, GLOBAL_TEXT_BUFFER_SIZE,
                        localization::Tr("castle.build.already_this_turn")

                    );
                } else if (gpTownManager->m_town->m_buildings & (1 << whichBuilding)) {
                    sprintf(
                        gText,
                        cCastleInfo[H2EnumIndex(INFO_ALREADY_BUILT)],
                        GetBuildingName(
                            gpTownManager->m_town->m_type,
                            static_cast<BuildingSlotType>(whichBuilding)
                        )
                    );
                } else {
                    if (!(gpTownManager->m_buildableBuildings & (1 << whichBuilding)))
                        sprintf(
                            gText,
                            cCastleInfo[H2EnumIndex(INFO_CANNOT_BUILD)],
                            GetBuildingName(
                                gpTownManager->m_town->m_type,
                                static_cast<BuildingSlotType>(whichBuilding)
                            )
                        );
                    else if (!(gpTownManager->m_affordableBuildings & (1 << whichBuilding)))
                        sprintf(
                            gText,
                            cCastleInfo[H2EnumIndex(INFO_CANNOT_AFFORD)],
                            GetBuildingName(
                                gpTownManager->m_town->m_type,
                                static_cast<BuildingSlotType>(whichBuilding)
                            )
                        );
                    else
                        sprintf(
                            gText,
                            cCastleInfo[H2EnumIndex(INFO_BUILD)],
                            GetBuildingName(
                                gpTownManager->m_town->m_type,
                                static_cast<BuildingSlotType>(whichBuilding)
                            )
                        );
                }
                break;

            case H2EnumIndex(BUILDING_SLOT_NONE):
                switch (message.payload.widget.id) {
                    case CONTROL_HERO_FIRST:
                    case CONTROL_HERO_FIRST + 1:
                    case CONTROL_RECRUIT_FIRST:
                    case CONTROL_RECRUIT_FIRST + 1:
                        if (message.payload.widget.id == CONTROL_HERO_FIRST
                            || message.payload.widget.id == CONTROL_HERO_FIRST + 1)
                            heroChoiceIndex = message.payload.widget.id - CONTROL_HERO_FIRST;
                        else
                            heroChoiceIndex = message.payload.widget.id - CONTROL_RECRUIT_FIRST;
                        if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] < gHeroGoldCost) {
                            strcpy(gText, cCastleInfo[H2EnumIndex(INFO_CANNOT_AFFORD_HERO)]);
                        } else if (gpCurPlayer->m_heroCount == PLAYER_HERO_CAPACITY) {
                            sprintf(
                                gText,
                                cCastleInfo[H2EnumIndex(INFO_TOO_MANY_HEROES)],
                                PLAYER_HERO_CAPACITY
                            );
                        } else if (gpTownManager->m_town->m_occupyingHeroId != -1) {
                            strcpy(gText, cCastleInfo[H2EnumIndex(INFO_TOWN_OCCUPIED)]);
                        } else {
                            sprintf(
                                gText,
                                cCastleInfo[H2EnumIndex(INFO_RECRUIT_HERO)],
                                gpGame->m_heroRecs[gpCurPlayer->AvailableHeroId(heroChoiceIndex)]
                                    .m_name,
                                gAlignmentNames
                                    [H2EnumIndex(gpGame
                                             ->m_heroRecs[gpCurPlayer
                                                              ->AvailableHeroId(heroChoiceIndex)]
                                             .m_cursorType)]
                            );
                        }
                        break;
                    case CONTROL_CLOSE:
                        strcpy(gText, cCastleInfo[H2EnumIndex(INFO_EXIT)]);
                        break;
                    default:
                        strcpy(gText, cCastleInfo[H2EnumIndex(INFO_OPTIONS)]);
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
        return MESSAGE_DISPATCH_CONSUME;
    }

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                if (message.payload.widget.id == CONTROL_CLOSE)
                    ret = 1;
                break;
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                quickFlag = ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0;
                switch (whichBuilding) {

                    case CONTROL_CAPTAIN_FORMATION_SPREAD:
                        if (quickFlag) {
                            NormalDialog(
                                localization::Tr("formation.spread.help")
                                     ,
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
                            break;
                        }
                        gpTownManager->m_town->m_formation = TOWN_FORMATION_SPREAD;
                        gpTownManager->SetupCastle(gpTownManager->m_heroWindow0, 1);
                        gpTownManager->m_heroWindow0->DrawWindow();
                        break;

                    case CONTROL_CAPTAIN_FORMATION_GROUPED:
                        if (quickFlag) {
                            NormalDialog(
                                localization::Tr("formation.grouped.help")
                                     ,
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
                            break;
                        }
                        gpTownManager->m_town->m_formation = TOWN_FORMATION_GROUPED;
                        gpTownManager->SetupCastle(gpTownManager->m_heroWindow0, 1);
                        gpTownManager->m_heroWindow0->DrawWindow();
                        break;

                    case H2EnumIndex(TOWN_OBJECT_MAGE_GUILD):
                        if (!quickFlag) {
                            if (gpTownManager->m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL
                                || !(gpTownManager->m_buildableBuildings & (1 << whichBuilding)))
                                break;
                        }
                        goto buy_building;

                    case H2EnumIndex(BUILDING_SLOT_SPECIAL_ONE):
                    case H2EnumIndex(BUILDING_SLOT_NECROMANCER_SHRINE):
                    case H2EnumIndex(BUILDING_SLOT_DOCK):
                    case H2EnumIndex(BUILDING_SLOT_SPECIAL_FOUR):
                    case H2EnumIndex(BUILDING_SLOT_SPECIAL_SEVEN):
                    case H2EnumIndex(BUILDING_SLOT_SPECIAL_EIGHT):
                    case H2EnumIndex(BUILDING_SLOT_SPECIAL_NINE):
                    case H2EnumIndex(BUILDING_SLOT_SPECIAL_TEN):
                    case H2EnumIndex(BUILDING_SLOT_WELL_EXTRA):
                    case H2EnumIndex(BUILDING_SLOT_SPECIAL_TWELVE):
                    case H2EnumIndex(BUILDING_SLOT_SPECIAL):
                    case H2EnumIndex(BUILDING_SLOT_NEUTRAL_LAST):
                    case H2EnumIndex(BUILDING_SLOT_DWELLING_FIRST):
                    case H2EnumIndex(BUILDING_SLOT_DWELLING_SECOND):
                    case H2EnumIndex(BUILDING_SLOT_DWELLING_THIRD):
                    case H2EnumIndex(BUILDING_SLOT_DWELLING_FOURTH):
                    case H2EnumIndex(BUILDING_SLOT_DWELLING_FIFTH):
                    case H2EnumIndex(BUILDING_SLOT_DWELLING_SIXTH):
                    case H2EnumIndex(BUILDING_SLOT_UPGRADE_FIRST):
                    case H2EnumIndex(BUILDING_SLOT_UPGRADE_SECOND):
                    case H2EnumIndex(BUILDING_SLOT_UPGRADE_THIRD):
                    case H2EnumIndex(BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE):
                    case H2EnumIndex(BUILDING_SLOT_SPECIAL_TWENTY_NINE):
                    case H2EnumIndex(BUILDING_SLOT_SPECIAL_THIRTY):
                        if (!quickFlag) {
                            if ((gpTownManager->m_town->m_buildings & (1 << whichBuilding))
                                || !(gpTownManager->m_buildableBuildings & (1 << whichBuilding)))
                                break;
                        }
                    buy_building:
                        for (objIndex = 0; objIndex < gpTownManager->m_townObjectCount;
                             ++objIndex) {
                            if (H2EnumIndex(gpTownManager->m_townObjects[objIndex]->m_buildingId)
                                == whichBuilding)
                                break;
                        }
                        ret = gpTownManager->BuyBuild(
                            static_cast<BuildingSlotType>(whichBuilding),
                            (gpTownManager->m_affordableBuildings & (1 << whichBuilding)) == 0,
                            quickFlag
                        );
                        break;

                    case H2EnumIndex(BUILDING_SLOT_NONE):
                        switch (message.payload.widget.id) {
                            case CONTROL_HERO_FIRST:
                            case CONTROL_HERO_FIRST + 1:
                            case CONTROL_RECRUIT_FIRST:
                            case CONTROL_RECRUIT_FIRST + 1:
                                if (message.payload.widget.id == CONTROL_HERO_FIRST
                                    || message.payload.widget.id == CONTROL_HERO_FIRST + 1)
                                    heroChoiceIndex =
                                        message.payload.widget.id - CONTROL_HERO_FIRST;
                                else
                                    heroChoiceIndex =
                                        message.payload.widget.id - CONTROL_RECRUIT_FIRST;
                                if (quickFlag) {
                                    HeroView(gpCurPlayer->AvailableHeroId(heroChoiceIndex), 1, 0);
                                    casWin->DrawWindow();
                                    gpTownManager->m_bankBox->Update(1);
                                    gpWindowManager
                                        ->FadeScreen(FADE_IN, TOWN_FADE_STEPS, NULL);
                                } else {
                                    ret = gpTownManager->RecruitHero(
                                        heroChoiceIndex,
                                        CannotRecruitHero()
                                    );
                                }
                                break;
                        }
                        break;
                }
                break;
        }
    }

selection_done:
    if (ret != 0) {
        message.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

H2EnumStorage<BuildingSlotType, u8> castleSlotsBase[CASTLE_SLOT_COUNT] =
    {19, 20, 21, 22, 23, 24, 0, 2, 1, 3, 7, 10, 4, 11, 13, 8, 9, 12};
heroWindow* casWin;
H2EnumStorage<BuildingSlotType, u8> castleSlotsUse[CASTLE_SLOT_COUNT];
