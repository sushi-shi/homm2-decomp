#include <Ints.h>
#include <IRONFIST/creatures.h>
#include <IRONFIST/expansions.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BASE/Icon2b.h>
#include <BASE/Icond2b.h>
#include <BASE/Iconm2b.h>
#include <BASE/Misc.h>
#include <BASE/bitmap.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/hero.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/town.h>
enum class CombatDrawLayer : i32 {
    DRAW_FIRST_LAYER        = 0,
    DRAW_LAYER_COUNT        = 9,
    DRAW_RIGHT_HERO_LAYER   = 1,
    DRAW_LEFT_HERO_LAYER    = 2,
    DRAW_WALL_TOP_LAYER     = 4,
    DRAW_WALL_MIDDLE_LAYER  = 5,
    DRAW_GATE_LAYER         = 6,
    DRAW_CATAPULT_LAYER     = 7,
    DRAW_CASTLE_GATE_ROW    = DRAW_WALL_TOP_LAYER,
    DRAW_CASTLE_REVERSE_ROW = DRAW_WALL_MIDDLE_LAYER
};
using enum CombatDrawLayer;
ENABLE_ENUM_STEPS(CombatDrawLayer)

enum class CombatSmallViewText : i32 {
    SMALL_VIEW_TEXT_QUANTITY_PLURAL   = 0,
    SMALL_VIEW_TEXT_QUANTITY_SINGULAR = 1,
    SMALL_VIEW_TEXT_ATTACK            = 2,
    SMALL_VIEW_TEXT_DEFENSE           = 3,
    SMALL_VIEW_TEXT_HIT_POINTS        = 4,
    SMALL_VIEW_TEXT_DAMAGE            = 5,
    SMALL_VIEW_TEXT_MORALE            = 6,
    SMALL_VIEW_TEXT_LUCK              = 7,
    SMALL_VIEW_TEXT_SHOTS             = 8
};
using enum CombatSmallViewText;

enum class CombatSmallViewModifierFrame : i32 {
    SMALL_VIEW_BAD_LUCK_FRAME       = 0,
    SMALL_VIEW_GOOD_LUCK_FRAME      = 1,
    SMALL_VIEW_NEUTRAL_LUCK_FRAME   = 2,
    SMALL_VIEW_BAD_MORALE_FRAME     = 3,
    SMALL_VIEW_GOOD_MORALE_FRAME    = 4,
    SMALL_VIEW_NEUTRAL_MORALE_FRAME = 5
};
using enum CombatSmallViewModifierFrame;

enum class CombatMessageText : i32 {
    MESSAGE_TEXT_DEFAULT          = 0,
    MESSAGE_TEXT_MOVE             = 1,
    MESSAGE_TEXT_FLY              = 2,
    MESSAGE_TEXT_ATTACK           = 3,
    MESSAGE_TEXT_SHOOT            = 4,
    MESSAGE_TEXT_HERO_OPTIONS     = 5,
    MESSAGE_TEXT_OPPOSING_HERO    = 6,
    MESSAGE_TEXT_VIEW_INFO        = 7,
    MESSAGE_TEXT_NO_SHOTS         = 8,
    MESSAGE_TEXT_CAPTAIN_OPTIONS  = 9,
    MESSAGE_TEXT_OPPOSING_CAPTAIN = 10,
    MESSAGE_TEXT_BALLISTA         = 11
};
using enum CombatMessageText;

typedef enum CombatDrawingConstant {
    FULL_SCREEN_HEIGHT            = 480,
    DRAWBRIDGE_TOWER_FRAME_BASE   = 21,
    WALL_COORDINATE_COUNT         = 8,
    WALL_FRAME_OFFSET_COUNT       = 7,
    TOP_TOWER_BASE_FRAME          = 5,
    SECOND_TOWER_BASE_FRAME       = 6,
    THIRD_TOWER_BASE_FRAME        = 7,
    BOTTOM_TOWER_BASE_FRAME       = 8,
    GATE_VISIBLE_FRAME            = 4,
    DRAWBRIDGE_TOP_CLIP_OFFSET    = 9,
    DRAWBRIDGE_BOTTOM_CLIP_OFFSET = 5,
    SPELL_POSITION_AXIS_COUNT     = 2
} CombatDrawingConstant;

typedef enum CombatWallCoordinateIndex {
    WALL_TOP_X    = 0,
    WALL_TOP_Y    = 1,
    WALL_SECOND_X = 2,
    WALL_SECOND_Y = 3,
    WALL_THIRD_X  = 4,
    WALL_THIRD_Y  = 5,
    WALL_BOTTOM_X = 6,
    WALL_BOTTOM_Y = 7
} CombatWallCoordinateIndex;

typedef enum CombatSmallViewStatRow {
    STAT_ROW_ATTACK     = 0,
    STAT_ROW_DEFENSE    = 1,
    STAT_ROW_HIT_POINTS = 2,
    STAT_ROW_DAMAGE     = 3,
    STAT_ROW_MORALE     = 4,
    STAT_ROW_LUCK       = 5
} CombatSmallViewStatRow;

void combatManager::NoShowCombatLog(const char* message) {
    char logMessage[COMBAT_MESSAGE_LOG_BUFFER_SIZE];
    sprintf(logMessage, "NC: %s", message);
    LogStr(logMessage);
}

void combatManager::ClearCombatMessages(i32 force) {
    m_combatMessagePending = 0;
    if (strlen(m_currentCombatMessage) <= 1 && strlen(m_previousCombatMessage) <= 1)
        return;
    if (force != 0 || m_combatMessageExpiration < platform::Ticks()) {
        strcpy(
            m_previousCombatMessage,
            ""
        );
        strcpy(
            m_currentCombatMessage,
            ""
        );
        m_previousCombatMessageExpiration = 0;
        m_combatMessageExpiration = 0;
        CombatMessage(
            "",
            1,
            0,
            0
        );
    }
}

void combatManager::CheckUpdateCombatMessages(void) {
    if (m_combatMessagePending != 0 && m_combatMessageExpiration != 0
        && m_combatMessageExpiration < platform::Ticks())
        CombatMessage(
            "",
            1,
            0,
            1
        );
}


static char gCombatMessageText[COMBAT_MESSAGE_WRAP_BUFFER_SIZE];

void combatManager::CombatMessage(
    const char* message, i32 updateScreen, i32 retainPrevious, i32 clear
) {
    char wrappedMessage[COMBAT_MESSAGE_WRAP_BUFFER_SIZE];
    char* newlinePtr;
    tag_message windowMessage;
    i32 oldLimit;
    i32 oldCompute;

    strcpy(gCombatMessageText, message);
    if (gbNoShowCombat != 0) {
        if (retainPrevious != 0)
            NoShowCombatLog(gCombatMessageText);
        return;
    }
    if (m_combatWindowOpen == 0)
        return;
    if (m_nonVisualCombat != 0)
        return;

    if (clear != 0) {
        strcpy(
            m_previousCombatMessage,
            ""
        );
        strcpy(m_currentCombatMessage, gCombatMessageText);
        m_previousCombatMessageExpiration = 0;
        m_combatMessageExpiration = 0;
        m_combatMessagePending = 0;
    } else {
        if (retainPrevious == 0) {
            if (m_combatMessageExpiration > platform::Ticks()) {
                return;
            } else {
                strcpy(
                    m_previousCombatMessage,
                    ""
                );
                m_previousCombatMessageExpiration = 0;
                m_combatMessageExpiration = 0;
            }
        } else {
            if (m_combatMessageExpiration != 0)
                strcpy(m_previousCombatMessage, m_currentCombatMessage);
            else
                strcpy(
                    m_previousCombatMessage,
                    ""
                );
            m_previousCombatMessageExpiration = m_combatMessageExpiration;
            m_combatMessageExpiration = platform::Ticks() + COMBAT_MESSAGE_TIMEOUT;
        }

        newlinePtr = FindToken(gCombatMessageText, '\n');
        if (newlinePtr != NULL) {
            *newlinePtr = 0;
            strcpy(wrappedMessage, gCombatMessageText);
            if (newlinePtr > gCombatMessageText && newlinePtr[-1] == '.')
                strcat(wrappedMessage, "  ");
            else
                strcat(wrappedMessage, " ");
            strcat(wrappedMessage, newlinePtr + 1);
            if (bigFont->LineLength(wrappedMessage, COMBAT_MESSAGE_LINE_WIDTH) <= 1) {
                strcpy(m_currentCombatMessage, wrappedMessage);
            } else {
                strcpy(m_previousCombatMessage, gCombatMessageText);
                strcpy(m_currentCombatMessage, newlinePtr + 1);
            }
            *newlinePtr = '\n';
        } else {
            strcpy(m_currentCombatMessage, gCombatMessageText);
        }
    }

    windowMessage.type = COMBAT_MESSAGE_BROADCAST_EVENT;
    windowMessage.payload.widget.command = COMBAT_MESSAGE_TEXT_ACTION;
    windowMessage.payload.widget.id = COMBAT_MESSAGE_WIDGET_FIRST;
    windowMessage.payload.widget.data.text = m_previousCombatMessage;
    m_combatWindow->BroadcastMessage(windowMessage);
    windowMessage.payload.widget.id = COMBAT_MESSAGE_WIDGET_SECOND;
    windowMessage.payload.widget.data.text = m_currentCombatMessage;
    m_combatWindow->BroadcastMessage(windowMessage);

    oldCompute = gbComputeExtent;
    oldLimit = gbLimitToExtent;
    gbLimitToExtent = false;
    gbComputeExtent = gbLimitToExtent;
    m_combatWindow->DrawWindow(0, COMBAT_MESSAGE_DRAW_FIRST_WIDGET, COMBAT_MESSAGE_WIDGET_SECOND);
    SaveCombatBorder();
    if (updateScreen != 0)
        gpWindowManager->UpdateScreenRegion(
            COMBAT_MESSAGE_WINDOW_X,
            COMBAT_MESSAGE_WINDOW_Y,
            COMBAT_MESSAGE_LINE_WIDTH,
            COMBAT_MESSAGE_WINDOW_HEIGHT
        );
    gbComputeExtent = oldCompute;
    gbLimitToExtent = oldLimit;
}

void combatManager::CombatMessage(CombatMessageCommand messageType) {
    army* currentArmy;
    army* targetArmy;
    CreatureType actingMonsterType;
    CreatureType targetMonsterType;

    if (m_combatWindowOpen == 0)
        return;
    if (gbNoShowCombat != 0)
        return;

    currentArmy = &m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex];
    actingMonsterType = currentArmy->m_monsterType;
    targetArmy = NULL;
    targetMonsterType = CreatureType(0);
    if (currentArmy->m_targetSide >= COMBAT_ATTACKER_SIDE
        && currentArmy->m_targetIndex >= 0) {
        targetArmy = &m_armies[H2EnumIndex(currentArmy->m_targetSide)][currentArmy->m_targetIndex];
        targetMonsterType = targetArmy->m_monsterType;
    }

    switch (messageType) {
        case COMBAT_MESSAGE_COMMAND_DEFAULT:
            if ((H2EnumIndex((currentArmy->m_monster.flags.all) & (COMBAT_ARMY_FLAG_SHOOTER))) != 0
                && currentArmy->m_monster.shots == 0 && targetArmy != NULL)
                strcpy(gText, cCombatMessage[H2EnumIndex(MESSAGE_TEXT_NO_SHOTS)]);
            else
                strcpy(gText, cCombatMessage[H2EnumIndex(MESSAGE_TEXT_DEFAULT)]);
            break;
        case COMBAT_MESSAGE_COMMAND_MOVE:
            sprintf(
                gText,
                cCombatMessage[H2EnumIndex(MESSAGE_TEXT_MOVE)],
                gArmyNames[H2EnumIndex(actingMonsterType)]
            );
            break;
        case COMBAT_MESSAGE_COMMAND_FLY:
            sprintf(
                gText,
                cCombatMessage[H2EnumIndex(MESSAGE_TEXT_FLY)],
                gArmyNames[H2EnumIndex(actingMonsterType)]
            );
            break;
        case COMBAT_MESSAGE_COMMAND_ATTACK:
            sprintf(
                gText,
                cCombatMessage[H2EnumIndex(MESSAGE_TEXT_ATTACK)],
                gArmyNamesPlural[H2EnumIndex(targetMonsterType)]
            );
            break;
        case COMBAT_MESSAGE_COMMAND_SHOOT:
        case COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL:
            sprintf(
                gText,
                cCombatMessage[H2EnumIndex(MESSAGE_TEXT_SHOOT)],
                gArmyNamesPlural[H2EnumIndex(targetMonsterType)],
                static_cast<i32>(currentArmy->m_monster.shots)
            );
            break;
        case COMBAT_MESSAGE_COMMAND_OPTIONS:
            if (m_heroes[H2EnumIndex(m_currentSide)] != NULL && m_heroes[H2EnumIndex(m_currentSide)]->m_isCaptain != 0)
                strcpy(gText, cCombatMessage[H2EnumIndex(MESSAGE_TEXT_CAPTAIN_OPTIONS)]);
            else
                strcpy(gText, cCombatMessage[H2EnumIndex(MESSAGE_TEXT_HERO_OPTIONS)]);
            break;
        case COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS:
            if (m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))] != NULL
                && m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]->m_isCaptain != 0)
                strcpy(gText, cCombatMessage[H2EnumIndex(MESSAGE_TEXT_OPPOSING_CAPTAIN)]);
            else
                strcpy(gText, cCombatMessage[H2EnumIndex(MESSAGE_TEXT_OPPOSING_HERO)]);
            break;
        case COMBAT_MESSAGE_COMMAND_VIEW_INFO:
            if (m_selectedHex == COMBAT_BALLISTA_HEX) {
                sprintf(gText, cCombatMessage[H2EnumIndex(MESSAGE_TEXT_BALLISTA)]);
            } else {
                actingMonsterType =
                    m_armies[H2EnumIndex(m_currentArmySide)][m_hexCells[m_selectedHex].m_occupantIndex]
                        .m_monsterType;
                if (actingMonsterType >= CREATURE_PEASANT)
                    sprintf(
                        gText,
                        cCombatMessage[H2EnumIndex(MESSAGE_TEXT_VIEW_INFO)],
                        gArmyNames[H2EnumIndex(actingMonsterType)]
                    );
                else
                    sprintf(
                        gText,
                        ""
                    );
            }
            break;
    }
    CombatMessage(gText, 1, 0, 0);
}

void combatManager::ResetLimitCreature(void) {
    CombatSide side;
    i32 armySlotIndex;

    for (side = COMBAT_ATTACKER_SIDE; H2EnumIndex(side) < COMBAT_SIDE_COUNT; side++) {
        for (armySlotIndex = 0; armySlotIndex < COMBAT_ARMY_SLOT_COUNT_DRAWING; armySlotIndex++) {
            m_limitCreatureCount[H2EnumIndex(side)][armySlotIndex]
                = (H2EnumIndex((m_armies[H2EnumIndex(side)][armySlotIndex].m_monster.flags.all) & (COMBAT_ARMY_FLAG_MIRROR_IMAGE)))
                        != 0
                    ? -1
                    : 0;
        }
    }
    m_drawHero[0] = 0;
    m_drawHero[1] = 0;
    m_drawHeroOverlay[0] = 0;
    m_drawHeroOverlay[1] = 0;
    giMaxExtentY = 0;
    giMaxExtentX = giMaxExtentY;
    giMinExtentX = COMBAT_MAX_EXTENT_X;
    giMinExtentY = COMBAT_MAX_EXTENT_Y;
}

void combatManager::UpdateCombatArea(void) {
    if (gbNoShowCombat != 0)
        return;
    if (m_combatWindowOpen == 0)
        return;

    gbEnlargeScreenBlit = false;
    gpWindowManager->UpdateScreenRegion(0, 0, COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT);
    gbEnlargeScreenBlit = true;
}

void combatManager::SetupGridForArmy(army* armyPtr) {
    i32 attackMask;
    CombatSide oldSide;
    i32 oldIndex;
    i32 j;
    i32 hexIndex;

    if (gbNoShowCombat != 0)
        return;
    if (gConfig.combatShadeLevel < 1)
        return;

    attackMask =
        armyPtr->GetAttackMask(armyPtr->m_hex, ARMY_ATTACK_TARGET_OCCUPIED, ARMY_HEX_INVALID);
    memset(m_gridState, H2EnumIndex(GRID_SHADE_NONE), sizeof(m_gridState));
    oldSide = armyPtr->m_targetSide;
    oldIndex = armyPtr->m_targetIndex;
    armyPtr->m_targetSide = COMBAT_SIDE_NONE;
    armyPtr->m_targetIndex = -1;
    gpSearchArray->SeedCombatPosition(armyPtr);
    armyPtr->m_targetSide = oldSide;
    armyPtr->m_targetIndex = oldIndex;

    for (j = 0; j < COMBAT_HEX_COUNT; j++) {
        if (j == armyPtr->m_hex) {
            m_gridState[j] = GRID_SHADE_REACHABLE;
        } else if (m_hexCells[j].m_pathReachable != 0) {
            if (m_hexCells[j].m_occupantSide != COMBAT_SIDE_NONE) {
                if (m_hexCells[j].m_occupantSide != armyPtr->m_side)
                    m_gridState[j] = GRID_SHADE_REACHABLE;
            } else {
                m_gridState[j] = GRID_SHADE_EMPTY_BLOCKED;
            }
        } else if (m_hexCells[j].m_occupantSide != COMBAT_SIDE_NONE
                   && m_hexCells[j].m_occupantSide != OppositeCombatSide(m_currentSide)
                   && (attackMask & (1 << m_hexCells[j].m_occupantIndex)) != 0) {
            m_gridState[j] = GRID_SHADE_REACHABLE;
        }
    }

    // A charger also reaches any enemy along a clear straight line.
    if (CreatureHasAttribute(H2EnumIndex(armyPtr->m_monsterType), CHARGER)) {
        for (hexIndex = 0; hexIndex < COMBAT_HEX_COUNT; hexIndex++) {
            if (m_hexCells[hexIndex].m_occupantSide != COMBAT_SIDE_NONE
                && m_hexCells[hexIndex].m_occupantSide != armyPtr->m_side
                && !armyPtr->FlightThroughObstacles(hexIndex)
                && armyPtr->TargetOnStraightLine(hexIndex)) {
                armyPtr->m_moveTargetHex = hexIndex;
                armyPtr->m_targetSide = m_hexCells[hexIndex].m_occupantSide;
                armyPtr->m_targetIndex = m_hexCells[hexIndex].m_occupantIndex;
                if (armyPtr->ValidFlight(hexIndex, ARMY_PATH_ANY_TARGET_HEX)
                    && armyPtr->GetStraightLineDistanceToHex(hexIndex)
                           <= armyPtr->m_monster.speed) {
                    m_gridState[hexIndex] = GRID_SHADE_REACHABLE;
                    m_hexCells[hexIndex].m_pathReachable = 1;
                }
            }
        }
    }
}

i32 combatManager::UpdateGrid(i32 resetGridDisplay, i32 rebuildGrid) {
    i32 didRedraw;
    i32 oldShading;
    i32 maxY;
    i32 gridChanged;
    i32 minY;
    i32 minX;
    i32 maxX;
    i32 cell;
    i32 doShading;

    if (gbNoShowCombat != 0)
        return 0;

    if (rebuildGrid != 0) {
        if (m_playerId[H2EnumIndex(m_currentSide)] == -1 || gbThisNetHumanPlayer[m_playerId[H2EnumIndex(m_currentSide)]] == 0
            || m_gridSelectionDisabled != 0) {
            memset(m_gridState, H2EnumIndex(GRID_SHADE_NONE), sizeof(m_gridState));
        } else {
            SetupGridForArmy(&m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex]);
        }
    }
    if (resetGridDisplay != 0)
        bGridWasShowing = 0;
    if (gConfig.combatShadeLevel < 1 && gConfig.showCombatGrid == 0)
        return 0;

    didRedraw = 0;
    minX = COMBAT_MAX_EXTENT_X;
    minY = COMBAT_MAX_EXTENT_Y;
    maxX = 0;
    maxY = 0;
    doShading = 0;
    oldShading = 0;
    gridChanged = 0;

    if (gConfig.combatShadeLevel < 1)
        goto DrawCombatGrid;

    for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
        if (m_previousGridState[cell] != m_gridState[cell])
            gridChanged = 1;
        if (m_gridState[cell] != GRID_SHADE_NONE)
            doShading = 1;
        if (m_previousGridState[cell] != GRID_SHADE_NONE)
            oldShading = 1;
    }

    if (resetGridDisplay != 0) {
        if (doShading == 0)
            goto DrawCombatGrid;
    } else {
        if (gridChanged == 0)
            return 0;
        if (oldShading != 0) {
            for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
                if (m_previousGridState[cell] != m_gridState[cell]
                    || m_gridState[cell] != GRID_SHADE_NONE) {
                    if (m_hexCells[cell].m_gridLeft < minX)
                        minX = m_hexCells[cell].m_gridLeft;
                    if (m_hexCells[cell].m_gridTop < minY)
                        minY = m_hexCells[cell].m_gridTop;
                    if (m_hexCells[cell].m_gridRight > maxX)
                        maxX = m_hexCells[cell].m_gridRight;
                    if (m_hexCells[cell].m_gridBottom > maxY)
                        maxY = m_hexCells[cell].m_gridBottom;
                }
            }
            if (minX < COMBAT_GRID_COPY_LEFT)
                minX = COMBAT_GRID_COPY_LEFT;
            if (minY < COMBAT_GRID_COPY_TOP)
                minY = COMBAT_GRID_COPY_TOP;
            if (maxX > COMBAT_GRID_COPY_RIGHT)
                maxX = COMBAT_GRID_COPY_RIGHT;
            if (maxY > COMBAT_GRID_COPY_BOTTOM)
                maxY = COMBAT_GRID_COPY_BOTTOM;
            m_combatBuffer->CopyToCareful(
                m_backgroundBuffer,
                minX,
                minY,
                minX - COMBAT_GRID_COPY_LEFT,
                minY - COMBAT_GRID_COPY_TOP,
                maxX - minX + 1,
                maxY - minY + 1
            );
            didRedraw = 1;
        }
    }

    if (doShading == 0)
        goto DrawCombatGrid;
    for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
        if (m_gridState[cell] != GRID_SHADE_NONE) {
            DimIconToBitmap(
                m_combatIcons[H2EnumIndex(COMBAT_ICON_GRID)],
                m_backgroundBuffer,
                m_hexCells[cell].m_gridLeft,
                m_hexCells[cell].m_gridTop,
                1,
                H2EnumIndex(m_gridState[cell]) - 1,
                ICON_DRAW_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT
            );
            didRedraw = 1;
        }
    }

DrawCombatGrid:
    if (gConfig.showCombatGrid != 0) {
        if (bGridWasShowing != 0 && didRedraw == 0)
            goto CopyGridState;
        for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
            if (cell % COMBAT_GRID_ROW_LENGTH != 0
                && cell % COMBAT_GRID_ROW_LENGTH != COMBAT_GRID_ROW_LENGTH - 1) {
                MonoIconToBitmap(
                    m_combatIcons[H2EnumIndex(COMBAT_ICON_GRID)],
                    m_backgroundBuffer,
                    m_hexCells[cell].m_gridLeft,
                    m_hexCells[cell].m_gridTop,
                    COMBAT_GRID_LINE_FRAME,
                    COMBAT_GRID_LINE_COLOR,
                    ICON_DRAW_CLIP,
                    0,
                    0,
                    COMBAT_SCREEN_WIDTH,
                    COMBAT_AREA_HEIGHT
                );
            }
        }
        didRedraw = 1;
        bGridWasShowing = 1;
    }

CopyGridState:
    memcpy(m_previousGridState, m_gridState, sizeof(m_previousGridState));
    return didRedraw;
}

void combatManager::DrawBackground(void) {
    icon* backgroundIcon;
    i32 unusedBackgroundHeight;
    i32 unusedBackgroundSide;

    if (gbNoShowCombat != 0)
        return;
    if (m_backgroundDrawn != 0)
        return;

    backgroundIcon = gpResourceManager->GetIcon(m_battlefieldBackgroundName);
    IconToBitmap(
        backgroundIcon,
        m_backgroundBuffer,
        0,
        0,
        0,
        ICON_DRAW_CLIP,
        0,
        0,
        COMBAT_SCREEN_WIDTH,
        COMBAT_AREA_HEIGHT,
        0
    );
    gpResourceManager->Dispose(backgroundIcon);

    if (m_debugFormation != 0) {
        sprintf(gText, "covr%04d.icn", m_debugFormation);
        backgroundIcon = gpResourceManager->GetIcon(gText);
        IconToBitmap(
            backgroundIcon,
            m_backgroundBuffer,
            0,
            0,
            0,
            ICON_DRAW_NO_CLIP,
            0,
            0,
            COMBAT_SCREEN_WIDTH,
            COMBAT_AREA_HEIGHT,
            0
        );
        gpResourceManager->Dispose(backgroundIcon);
    }
    if (m_battlefieldFringe != FRINGE_NONE) {
        sprintf(gText, "frng%04d.icn", H2EnumIndex(m_battlefieldFringe));
        backgroundIcon = gpResourceManager->GetIcon(gText);
        if (m_inCastleCombat != 0)
            IconToBitmap(
                backgroundIcon,
                m_backgroundBuffer,
                0,
                0,
                0,
                ICON_DRAW_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH / H2EnumIndex(COMBAT_SIDE_COUNT),
                COMBAT_AREA_HEIGHT,
                0
            );
        else
            IconToBitmap(
                backgroundIcon,
                m_backgroundBuffer,
                0,
                0,
                0,
                ICON_DRAW_NO_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
        gpResourceManager->Dispose(backgroundIcon);
    }
    if (m_inCastleCombat != 0) {
        sprintf(
            gText,
            "castbkg%c.icn",
            cHeroTypeInitial[H2EnumIndex(m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_type)]
        );
        backgroundIcon = gpResourceManager->GetIcon(gText);
        IconToBitmap(
            backgroundIcon,
            m_backgroundBuffer,
            0,
            0,
            COMBAT_CASTLE_BACKGROUND_BASE_FRAME,
            ICON_DRAW_NO_CLIP,
            0,
            0,
            COMBAT_SCREEN_WIDTH,
            COMBAT_AREA_HEIGHT,
            0
        );
        if (m_drawbridgeBackgroundVisible != 0)
            IconToBitmap(
                m_combatIcons[H2EnumIndex(COMBAT_ICON_DRAWBRIDGE)],
                m_backgroundBuffer,
                0,
                0,
                0,
                ICON_DRAW_NO_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
        if (m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_type == FACTION_KNIGHT
            && (m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_buildings & H2EnumIndex(TOWN_BUILDING_RAINBOW)) != 0)
            IconToBitmap(
                backgroundIcon,
                m_backgroundBuffer,
                0,
                0,
                COMBAT_CASTLE_BACKGROUND_BUILDING_FRAME,
                ICON_DRAW_NO_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
        else
            IconToBitmap(
                backgroundIcon,
                m_backgroundBuffer,
                0,
                0,
                COMBAT_CASTLE_BACKGROUND_DEFAULT_FRAME,
                ICON_DRAW_NO_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
        gpResourceManager->Dispose(backgroundIcon);
    }

    m_backgroundBuffer->CopyToCareful(
        m_combatBuffer,
        0,
        0,
        COMBAT_GRID_COPY_LEFT,
        COMBAT_GRID_COPY_TOP,
        COMBAT_BACKGROUND_COPY_WIDTH,
        COMBAT_BACKGROUND_COPY_HEIGHT
    );
    UpdateGrid(1, 0);
    m_backgroundBuffer->CopyToCareful(
        gpWindowManager->m_screen,
        0,
        0,
        0,
        0,
        COMBAT_SCREEN_WIDTH,
        COMBAT_AREA_HEIGHT
    );
    m_backgroundDrawn = true;
}

void combatManager::UpdateMouseGrid(i32 hexIndex, i32 forceUpdate) {
    i32 savedLimit;
    i32 copyHeight;
    i32 oldMaxX;
    i32 savedMinX;
    i32 backupMaxY;
    i32 oldCompute;
    i32 savedMinY;

    if (m_nonVisualCombat != 0)
        return;
    if (gbNoShowCombat != 0)
        return;
    if (gConfig.showCombatMouseHex == 0)
        return;
    if (gbProcessingCombatAction != 0 && forceUpdate == 0)
        return;

    if (hexIndex < 0 || hexIndex >= COMBAT_HEX_COUNT || hexIndex % COMBAT_GRID_ROW_LENGTH == 0
        || hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1)
        hexIndex = -1;
    if (hexIndex == m_mouseGridHex)
        return;

    if (m_mouseGridBuffer == NULL)
        m_mouseGridBuffer =
            new bitmap(BITMAP_TYPE_NONE, COMBAT_MOUSE_HEX_WIDTH, COMBAT_MOUSE_HEX_HEIGHT);

    if (m_mouseGridHex != -1) {
        if (m_hexCells[m_mouseGridHex].m_gridTop + COMBAT_MOUSE_HEX_MAX_Y_OFFSET
            > COMBAT_MAX_EXTENT_Y)
            copyHeight = COMBAT_MAX_EXTENT_Y - m_hexCells[m_mouseGridHex].m_gridTop + 1;
        else
            copyHeight = COMBAT_MOUSE_HEX_HEIGHT;
        m_mouseGridBuffer->CopyToCareful(
            m_backgroundBuffer,
            m_hexCells[m_mouseGridHex].m_gridLeft,
            m_hexCells[m_mouseGridHex].m_gridTop,
            0,
            0,
            COMBAT_MOUSE_HEX_WIDTH,
            copyHeight
        );
    }
    if (hexIndex != -1) {
        if (m_hexCells[hexIndex].m_gridTop + COMBAT_MOUSE_HEX_MAX_Y_OFFSET > COMBAT_MAX_EXTENT_Y)
            copyHeight = COMBAT_MAX_EXTENT_Y - m_hexCells[hexIndex].m_gridTop + 1;
        else
            copyHeight = COMBAT_MOUSE_HEX_HEIGHT;
        m_backgroundBuffer->CopyToCareful(
            m_mouseGridBuffer,
            0,
            0,
            m_hexCells[hexIndex].m_gridLeft,
            m_hexCells[hexIndex].m_gridTop,
            COMBAT_MOUSE_HEX_WIDTH,
            copyHeight
        );
        DimIconToBitmap(
            m_combatIcons[H2EnumIndex(COMBAT_ICON_GRID)],
            m_backgroundBuffer,
            m_hexCells[hexIndex].m_gridLeft,
            m_hexCells[hexIndex].m_gridTop,
            1,
            COMBAT_GRID_MOUSE_FRAME,
            ICON_DRAW_CLIP,
            0,
            0,
            COMBAT_SCREEN_WIDTH,
            COMBAT_AREA_HEIGHT
        );
    }

    savedMinX = giMinExtentX;
    savedMinY = giMinExtentY;
    oldMaxX = giMaxExtentX;
    backupMaxY = giMaxExtentY;
    savedLimit = gbLimitToExtent;
    oldCompute = gbComputeExtent;
    if (m_mouseGridHex != -1) {
        giMinExtentX = m_hexCells[m_mouseGridHex].m_gridLeft;
        giMinExtentY = m_hexCells[m_mouseGridHex].m_gridTop;
        giMaxExtentX = m_hexCells[m_mouseGridHex].m_gridLeft + COMBAT_MOUSE_HEX_MAX_X_OFFSET;
        giMaxExtentY = m_hexCells[m_mouseGridHex].m_gridTop + COMBAT_MOUSE_HEX_MAX_Y_OFFSET;
    } else {
        giMinExtentX = COMBAT_SCREEN_WIDTH;
        giMinExtentY = FULL_SCREEN_HEIGHT;
        giMaxExtentX = 0;
        giMaxExtentY = 0;
    }
    if (hexIndex != -1) {
        if (giMinExtentX > m_hexCells[hexIndex].m_gridLeft)
            giMinExtentX = m_hexCells[hexIndex].m_gridLeft;
        if (giMinExtentY > m_hexCells[hexIndex].m_gridTop)
            giMinExtentY = m_hexCells[hexIndex].m_gridTop;
        if (giMaxExtentX < m_hexCells[hexIndex].m_gridLeft + COMBAT_MOUSE_HEX_MAX_X_OFFSET)
            giMaxExtentX = m_hexCells[hexIndex].m_gridLeft + COMBAT_MOUSE_HEX_MAX_X_OFFSET;
        if (giMaxExtentY < m_hexCells[hexIndex].m_gridTop + COMBAT_MOUSE_HEX_MAX_Y_OFFSET)
            giMaxExtentY = m_hexCells[hexIndex].m_gridTop + COMBAT_MOUSE_HEX_MAX_Y_OFFSET;
    }
    if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
        giMaxExtentY = COMBAT_MAX_EXTENT_Y;

    gbLimitToExtent = true;
    gbComputeExtent = true;
    m_backgroundBuffer->CopyTo(
        gpWindowManager->m_screen,
        giMinExtentX,
        giMinExtentY,
        giMinExtentX,
        giMinExtentY,
        giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1
    );
    DrawFrame(0, 0, 0, 0, COMBAT_MOUSE_REDRAW_DELAY, 1, 1);
    gpWindowManager->UpdateScreenRegion(
        giMinExtentX,
        giMinExtentY,
        giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1
    );
    giMinExtentX = savedMinX;
    giMinExtentY = savedMinY;
    giMaxExtentX = oldMaxX;
    giMaxExtentY = backupMaxY;
    gbLimitToExtent = savedLimit;
    gbComputeExtent = oldCompute;
    m_mouseGridHex = hexIndex;
}

void combatManager::DrawFrame(
    i32 updateScreen,
    i32 computeExtent,
    i32 redrawExtent,
    i32 extentOnly,
    i32 delay,
    i32 drawBackground,
    i32 waitForTimer
) {
    i32 col;
    CombatDrawLayer row;
    i32 unusedDrawState1;
    H2SteppedEnumStorage<ArmyDrawState, i32> state;
    i32 extentChanged1;
    i32 unusedDrawState26;
    i32 unused7;
    i32 unusedDrawState8;
    i32 unusedDrawState;
    i32 gridWasShowing1;
    i32 endColumn;
    i32 skipSpecialOccupants8;
    i32 columnStep1;
    i32 startColumn;
    i32 armyIndex7;

    if (m_nonVisualCombat != 0)
        return;
    if (gbNoShowCombat != 0)
        return;
    if (m_combatWindowOpen == 0)
        return;

    PollSound();
    gpMouseManager->m_cursorReady = 0;

    if (computeExtent != 0) {
        extentChanged1 = 0;
        for (state = 0; H2EnumIndex(state) < COMBAT_SIDE_COUNT; state++) {
            for (armyIndex7 = 0; armyIndex7 < COMBAT_ARMY_SLOT_COUNT_DRAWING; armyIndex7++) {
                if (m_limitCreatureCount[H2EnumIndex(state)][armyIndex7] > 0) {
                    extentChanged1 = 1;
                    gbComputeExtent = true;
                    gbSaveBiggestExtent = true;
                    gbReturnAfterComputeExtent = true;
                    m_armies[H2EnumIndex(state)][armyIndex7].DrawToBuffer(
                        m_hexCells[m_armies[H2EnumIndex(state)][armyIndex7].m_hex].m_x,
                        m_hexCells[m_armies[H2EnumIndex(state)][armyIndex7].m_hex].m_y,
                        0
                    );
                    gbReturnAfterComputeExtent = false;
                    gbComputeExtent = false;
                    gbSaveBiggestExtent = false;
                }
            }
        }

        for (state = 0; H2EnumIndex(state) < COMBAT_SIDE_COUNT; state++) {
            if (m_drawHero[H2EnumIndex(state)] != 0) {
                extentChanged1 = 1;
                gbComputeExtent = true;
                gbSaveBiggestExtent = true;
                gbReturnAfterComputeExtent = true;
                m_heroIcons[H2EnumIndex(state)]->CombatClipDrawToBuffer(
                    H2EnumIndex(state) == 0 ? COMBAT_HERO_LEFT_X : COMBAT_HERO_RIGHT_X,
                    H2EnumIndex(state) == 0 ? COMBAT_HERO_LEFT_Y : COMBAT_HERO_RIGHT_Y,
                    sCmbtHero[m_heroSpriteIndex[H2EnumIndex(state)]]
                        .animationFrames[m_heroAnimationState[H2EnumIndex(state)]][m_heroAnimationFrame[H2EnumIndex(state)]],
                    &m_heroLimits[H2EnumIndex(state)],
                    ICON_DRAW_FLIPPED,
                    0,
                    NULL,
                    NULL
                );
                gbReturnAfterComputeExtent = false;
                gbComputeExtent = false;
                gbSaveBiggestExtent = false;
            }

            if (m_drawHeroOverlay[H2EnumIndex(state)] != 0) {
                extentChanged1 = 1;
                gbComputeExtent = true;
                gbSaveBiggestExtent = true;
                gbReturnAfterComputeExtent = true;
                m_heroOverlayIcons[H2EnumIndex(state)]->CombatClipDrawToBuffer(
                    H2EnumIndex(state) == 0 ? COMBAT_HERO_LEFT_X
                               : ((m_heroes[H2EnumIndex(state)]->m_isCaptain ? -1 : 0)
                                  & (COMBAT_HERO_RIGHT_ALT_X - COMBAT_HERO_RIGHT_X))
                                     + COMBAT_HERO_RIGHT_X,
                    H2EnumIndex(state) == 0 ? COMBAT_HERO_LEFT_Y
                               : ((m_heroes[H2EnumIndex(state)]->m_isCaptain ? -1 : 0)
                                  & (COMBAT_HERO_RIGHT_ALT_Y - COMBAT_HERO_RIGHT_Y))
                                     + COMBAT_HERO_RIGHT_Y,
                    m_heroOverlayFrame[H2EnumIndex(state)],
                    &m_heroOverlayLimits[H2EnumIndex(state)],
                    ICON_DRAW_FLIPPED,
                    0,
                    NULL,
                    NULL
                );
                gbReturnAfterComputeExtent = false;
                gbComputeExtent = false;
                gbSaveBiggestExtent = false;
            }
        }

        if (extentOnly != 0)
            goto finish;
        if (extentChanged1 == 0)
            goto finish;

        giMinExtentX--;
        giMinExtentY--;
        giMaxExtentX++;
        giMaxExtentY++;

        // The fire walls burn inside the redraw extent too.
        if (!gIronfistExtra.combat.spell.fireBombWalls.empty()) {
            icon* wallIcon = gpResourceManager->GetIcon(gCombatFxNames[H2EnumIndex(COMBAT_EFFECT_FIRE_BOMB)]);
            IconEntry* wallEntry = GetIconEntry(wallIcon, 0);
            for (auto& wall : gIronfistExtra.combat.spell.fireBombWalls) {
                hexcell* wallCell = &m_hexCells[wall.hexIdx];
                i32 drawX = wallCell->m_x + wallEntry->x;
                i32 drawY = wallCell->m_gridTop + wallEntry->y;
                i32 drawMaxX = drawX + wallEntry->w;
                i32 drawMaxY = drawY + wallEntry->h;
                if (giMinExtentX > drawX)
                    giMinExtentX = drawX;
                if (giMaxExtentX < drawMaxX)
                    giMaxExtentX = drawMaxX;
                if (giMinExtentY > drawY)
                    giMinExtentY = drawY;
                if (giMaxExtentY < drawMaxY)
                    giMaxExtentY = drawMaxY;
            }
        }

        if (giMinExtentX < 0)
            giMinExtentX = 0;
        if (giMinExtentY < 0)
            giMinExtentY = 0;
        if (giMaxExtentX > COMBAT_MAX_EXTENT_X)
            giMaxExtentX = COMBAT_MAX_EXTENT_X;
        if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
            giMaxExtentY = COMBAT_MAX_EXTENT_Y;
    }

    if (drawBackground != 0) {
        if (m_backgroundDrawn != 0) {
            if (computeExtent != 0 || redrawExtent != 0 || gbLimitToExtent != 0) {
                m_backgroundBuffer->CopyTo(
                    gpWindowManager->m_screen,
                    giMinExtentX,
                    giMinExtentY,
                    giMinExtentX,
                    giMinExtentY,
                    giMaxExtentX - giMinExtentX + 1,
                    giMaxExtentY - giMinExtentY + 1
                );
            } else {
                m_backgroundBuffer->CopyTo(
                    gpWindowManager->m_screen,
                    0,
                    0,
                    0,
                    0,
                    COMBAT_SCREEN_WIDTH,
                    COMBAT_AREA_HEIGHT
                );
            }
        } else {
            DrawBackground();
        }
    }

    if (computeExtent != 0) {
        gbLimitToExtent = true;
        gbComputeExtent = true;
    }

    // The lingering fire walls burn under the creatures.
    for (auto& wall : gIronfistExtra.combat.spell.fireBombWalls) {
        SLimitData wallLimits;
        gpResourceManager->GetIcon(gCombatFxNames[H2EnumIndex(COMBAT_EFFECT_FIRE_BOMB)])
            ->CombatClipDrawToBuffer(
                m_hexCells[wall.hexIdx].m_x,
                m_hexCells[wall.hexIdx].m_gridTop,
                wall.currentFrame,
                &wallLimits,
                ICON_DRAW_NORMAL,
                0,
                NULL,
                NULL
            );
    }

    for (row = DRAW_FIRST_LAYER; row < DRAW_LAYER_COUNT; row++) {
        if (row == DRAW_RIGHT_HERO_LAYER && m_heroes[1] != NULL) {
            m_heroIcons[1]->CombatClipDrawToBuffer(
                ((m_heroes[1]->m_isCaptain ? -1 : 0)
                 & (COMBAT_HERO_RIGHT_ALT_X - COMBAT_HERO_RIGHT_X))
                    + COMBAT_HERO_RIGHT_X,
                ((m_heroes[1]->m_isCaptain ? -1 : 0)
                 & (COMBAT_HERO_RIGHT_ALT_Y - COMBAT_HERO_RIGHT_Y))
                    + COMBAT_HERO_RIGHT_Y,
                sCmbtHero[m_heroSpriteIndex[H2EnumIndex(COMBAT_DEFENDER_SIDE)]]
                    .animationFrames[m_heroAnimationState[H2EnumIndex(COMBAT_DEFENDER_SIDE)]]
                                    [m_heroAnimationFrame[H2EnumIndex(COMBAT_DEFENDER_SIDE)]],
                &m_heroLimits[1],
                ICON_DRAW_FLIPPED,
                0,
                NULL,
                NULL
            );
            m_heroOverlayIcons[1]->CombatClipDrawToBuffer(
                COMBAT_HERO_RIGHT_X,
                COMBAT_HERO_RIGHT_Y,
                m_heroOverlayFrame[1],
                &m_heroOverlayLimits[1],
                ICON_DRAW_FLIPPED,
                0,
                NULL,
                NULL
            );
        }

        if (row == DRAW_LEFT_HERO_LAYER && m_heroes[0] != NULL) {
            m_heroIcons[0]->CombatClipDrawToBuffer(
                COMBAT_HERO_LEFT_X,
                COMBAT_HERO_LEFT_Y,
                sCmbtHero[m_heroSpriteIndex[H2EnumIndex(COMBAT_ATTACKER_SIDE)]]
                    .animationFrames[m_heroAnimationState[H2EnumIndex(COMBAT_ATTACKER_SIDE)]]
                                    [m_heroAnimationFrame[H2EnumIndex(COMBAT_ATTACKER_SIDE)]],
                &m_heroLimits[0],
                ICON_DRAW_NORMAL,
                0,
                NULL,
                NULL
            );
            m_heroOverlayIcons[0]->CombatClipDrawToBuffer(
                COMBAT_HERO_LEFT_X,
                COMBAT_HERO_LEFT_Y,
                m_heroOverlayFrame[0],
                &m_heroOverlayLimits[0],
                ICON_DRAW_NORMAL,
                0,
                NULL,
                NULL
            );
        }

        startColumn = COMBAT_GRID_FIRST_COLUMN;
        endColumn = COMBAT_GRID_COLUMN_END;
        columnStep1 = 1;
        if (m_inCastleCombat != 0 && row >= DRAW_CASTLE_REVERSE_ROW) {
            startColumn = COMBAT_GRID_REVERSE_FIRST_COLUMN;
            endColumn = COMBAT_GRID_REVERSE_COLUMN_END;
            columnStep1 = -1;
        }

        if (m_inCastleCombat != 0 && row == DRAW_CATAPULT_LAYER) {
            m_combatIcons[H2EnumIndex(COMBAT_ICON_CATAPULT)]->CombatClipDrawToBuffer(
                COMBAT_CATAPULT_X,
                COMBAT_CATAPULT_Y,
                m_catapultFrame[H2EnumIndex(COMBAT_ATTACKER_SIDE)],
                &m_catapultLimits,
                ICON_DRAW_NORMAL,
                0,
                NULL,
                NULL
            );
        }
        if (m_inCastleCombat != 0 && row == DRAW_WALL_TOP_LAYER
            && m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN) {
            m_combatIcons[H2EnumIndex(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
                0,
                0,
                H2EnumIndex(m_drawbridgeState) + DRAWBRIDGE_TOWER_FRAME_BASE,
                &m_upperWallLimits,
                ICON_DRAW_NORMAL,
                0,
                NULL,
                NULL
            );
        }
        if (m_inCastleCombat != 0 && row == DRAW_WALL_MIDDLE_LAYER
            && m_drawbridgeState == COMBAT_DRAWBRIDGE_LOWERED) {
            m_combatIcons[H2EnumIndex(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
                0,
                0,
                COMBAT_CASTLE_TOP_LAYER_FRAME,
                &m_middleWallLimits,
                ICON_DRAW_NORMAL,
                0,
                NULL,
                NULL
            );
        }
        if (m_inCastleCombat != 0 && row == DRAW_GATE_LAYER) {
            m_combatIcons[H2EnumIndex(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
                0,
                0,
                m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_KEEP)]
                            == COMBAT_WALL_STATE_KEEP_STANDING
                    ? COMBAT_CASTLE_GATE_CLOSED_FRAME
                    : COMBAT_CASTLE_GATE_FRAME,
                &m_gateLimits,
                ICON_DRAW_NORMAL,
                0,
                NULL,
                NULL
            );
        }

        skipSpecialOccupants8 = 0;
        if (m_inCastleCombat != 0
            && m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FOURTH)]
                   != COMBAT_WALL_STATE_DESTROYED
            && m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FOURTH)]
                   != COMBAT_WALL_STATE_SECTION_DESTROYED) {
            skipSpecialOccupants8 = 1;
            for (state = ARMY_DRAW_BEHIND; state < ARMY_DRAW_PHASE_COUNT; state++) {
                m_hexCells[H2EnumIndex(COMBAT_CASTLE_SPECIAL_HEX_FIRST)].DrawOccupant(state, 0);
                m_hexCells[H2EnumIndex(COMBAT_CASTLE_SPECIAL_HEX_SECOND)].DrawOccupant(state, 0);
            }
        }

        for (col = startColumn; col != endColumn; col += columnStep1)
            m_hexCells[H2EnumIndex(row) * COMBAT_GRID_ROW_LENGTH + col].DrawLowerDeadOccupants();

        for (col = startColumn; col != endColumn; col += columnStep1)
            m_hexCells[H2EnumIndex(row) * COMBAT_GRID_ROW_LENGTH + col].DrawUpperDeadOccupant();

        for (state = ARMY_DRAW_BEHIND; state < ARMY_DRAW_PHASE_COUNT; state++) {
            if (state == ARMY_DRAW_NORMAL) {
                for (col = startColumn; col != endColumn; col += columnStep1) {
                    if (m_hexCells[H2EnumIndex(row) * COMBAT_GRID_ROW_LENGTH + col].m_obstacleIndex
                        != -1) {
                        m_hexCells[H2EnumIndex(row) * COMBAT_GRID_ROW_LENGTH + col].DrawObstacle();
                    }
                }
            }

            for (col = startColumn; col != endColumn; col += columnStep1) {
                u16 wallCoordinates1[H2EnumIndex(FACTION_COUNT)][WALL_COORDINATE_COUNT] = {
                    {443, 153, 399, 237, 399, 321, 443, 405},
                    {443, 153, 399, 237, 399, 321, 443, 405},
                    {443, 153, 399, 237, 399, 321, 443, 405},
                    {443, 153, 399, 237, 399, 321, 443, 405},
                    {443, 153, 399, 237, 399, 321, 443, 405},
                    {443, 153, 399, 237, 399, 321, 443, 405}
                };
                u8 wallFrameOffsets1[WALL_FRAME_OFFSET_COUNT] = {0, 4, 8, 23, 27, 35, 31};
                i32 wallX7;
                i32 wallFrame1;
                i32 wallY;
                H2EnumStorage<CombatCastleHex, i32> castleHex;

                if (m_inCastleCombat != 0 && state == ARMY_DRAW_BEHIND) {
                    castleHex = H2EnumIndex(row) * COMBAT_GRID_ROW_LENGTH + col;
                    wallFrame1 = 0;
                    wallX7 = 0;
                    wallY = 0;
                    switch (castleHex) {
                        case COMBAT_CASTLE_HEX_TOP_TOWER:
                            wallFrame1 =
                                wallFrameOffsets1[H2EnumIndex(m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FIRST)])]
                                + TOP_TOWER_BASE_FRAME;
                            break;
                        case COMBAT_CASTLE_HEX_SECOND_TOWER:
                            wallFrame1 =
                                wallFrameOffsets1[H2EnumIndex(m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_SECTION_SECOND)])]
                                + SECOND_TOWER_BASE_FRAME;
                            break;
                        case COMBAT_CASTLE_HEX_THIRD_TOWER:
                            wallFrame1 =
                                wallFrameOffsets1[H2EnumIndex(m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_SECTION_THIRD)])]
                                + THIRD_TOWER_BASE_FRAME;
                            break;
                        case COMBAT_CASTLE_HEX_BOTTOM_TOWER:
                            wallFrame1 =
                                wallFrameOffsets1[H2EnumIndex(m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FOURTH)])]
                                + BOTTOM_TOWER_BASE_FRAME;
                            break;
                        case COMBAT_CASTLE_HEX_TOP_WALL:
                            wallFrame1 = H2EnumIndex(m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_TOP_TOWER)])
                                         + COMBAT_CASTLE_WALL_BASE_FRAME;
                            wallX7 =
                                wallCoordinates1[H2EnumIndex(m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_type)]
                                                [WALL_TOP_X];
                            wallY =
                                wallCoordinates1[H2EnumIndex(m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_type)]
                                                [WALL_TOP_Y];
                            break;
                        case COMBAT_CASTLE_HEX_SECOND_WALL:
                            wallFrame1 = H2EnumIndex(m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_SECOND_TOWER)])
                                         + COMBAT_CASTLE_WALL_BASE_FRAME;
                            wallX7 =
                                wallCoordinates1[H2EnumIndex(m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_type)]
                                                [WALL_SECOND_X];
                            wallY =
                                wallCoordinates1[H2EnumIndex(m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_type)]
                                                [WALL_SECOND_Y];
                            break;
                        case COMBAT_CASTLE_HEX_THIRD_WALL:
                            wallFrame1 = H2EnumIndex(m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_THIRD_TOWER)])
                                         + COMBAT_CASTLE_WALL_BASE_FRAME;
                            wallX7 =
                                wallCoordinates1[H2EnumIndex(m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_type)]
                                                [WALL_THIRD_X];
                            wallY =
                                wallCoordinates1[H2EnumIndex(m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_type)]
                                                [WALL_THIRD_Y];
                            break;
                        case COMBAT_CASTLE_HEX_BOTTOM_WALL:
                            wallFrame1 = H2EnumIndex(m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_BOTTOM_TOWER)])
                                         + COMBAT_CASTLE_WALL_BASE_FRAME;
                            wallX7 =
                                wallCoordinates1[H2EnumIndex(m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_type)]
                                                [WALL_BOTTOM_X];
                            wallY =
                                wallCoordinates1[H2EnumIndex(m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_type)]
                                                [WALL_BOTTOM_Y];
                            break;
                        case COMBAT_CASTLE_HEX_GATE:
                            if (m_drawbridgeState != COMBAT_CASTLE_GATE_HIDDEN)
                                wallFrame1 = GATE_VISIBLE_FRAME;
                            break;
                    }
                    if (wallFrame1 != 0) {
                        m_combatIcons[H2EnumIndex(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
                            wallX7,
                            wallY,
                            wallFrame1,
                            &m_hexCells[H2EnumIndex(castleHex)].m_limits[0],
                            ICON_DRAW_NORMAL,
                            0,
                            NULL,
                            NULL
                        );
                    }
                }

                if (skipSpecialOccupants8 == 0
                    || (H2EnumIndex(row) * COMBAT_GRID_ROW_LENGTH + col
                            != H2EnumIndex(COMBAT_CASTLE_SPECIAL_HEX_FIRST)
                        && H2EnumIndex(row) * COMBAT_GRID_ROW_LENGTH + col
                               != H2EnumIndex(COMBAT_CASTLE_SPECIAL_HEX_SECOND))) {
                    m_hexCells[H2EnumIndex(row) * COMBAT_GRID_ROW_LENGTH + col].DrawOccupant(
                        state,
                        0
                    );
                }
            }
        }

        if (m_inCastleCombat == 0
            || (m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_buildings & H2EnumIndex(TOWN_BUILDING_MOAT)) == 0)
            goto endRow;
        if (row == DRAW_CASTLE_GATE_ROW && m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN)
            goto endRow;

        if (giWalkingTo == moatCell[H2EnumIndex(row)] || giWalkingTo2 == moatCell[H2EnumIndex(row)]
            || giWalkingFrom == moatCell[H2EnumIndex(row)] || giWalkingFrom2 == moatCell[H2EnumIndex(row)]) {
            if (abs(giWalkingTo - giWalkingFrom) <= 1)
                goto drawMoat;
            if (H2EnumIndex(row)
                != (giWalkingTo / COMBAT_GRID_ROW_LENGTH > giWalkingFrom / COMBAT_GRID_ROW_LENGTH
                        ? giWalkingTo / COMBAT_GRID_ROW_LENGTH
                        : giWalkingFrom / COMBAT_GRID_ROW_LENGTH))
                goto endRow;

            if (gpCombatManager->m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN
                && (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW
                    || giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW)) {
                if (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW
                    && giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW - 1)
                    goto endRow;
                if (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW - 1
                    && giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW)
                    goto endRow;

                if (giWalkingTo < giWalkingFrom) {
                    if (moatCell[H2EnumIndex(row)] == giWalkingFrom
                        || moatCell[H2EnumIndex(row)] == giWalkingFrom2)
                        goto drawMoat;
                    goto endRow;
                } else {
                    if (moatCell[H2EnumIndex(row)] == giWalkingTo || moatCell[H2EnumIndex(row)] == giWalkingTo2)
                        goto drawMoat;
                    goto endRow;
                }
            } else {
                i32 drawbridgeTop;
                i32 drawbridgeBottom;

                drawbridgeTop = m_hexCells[giWalkingFrom].m_y - DRAWBRIDGE_TOP_CLIP_OFFSET
                              + giWalkingYMod;
                drawbridgeBottom =
                    m_hexCells[giWalkingFrom > giWalkingTo ? giWalkingFrom : giWalkingTo].m_y
                    + DRAWBRIDGE_BOTTOM_CLIP_OFFSET;
                IconToBitmap(
                    m_combatIcons[H2EnumIndex(COMBAT_ICON_DRAWBRIDGE)],
                    gpWindowManager->m_screen,
                    0,
                    0,
                    0,
                    ICON_DRAW_CLIP,
                    0,
                    drawbridgeTop,
                    COMBAT_SCREEN_WIDTH,
                    drawbridgeBottom - drawbridgeTop + 1,
                    0
                );
                goto endRow;
            }
        } else {
            if (m_hexCells[moatCell[H2EnumIndex(row)]].m_occupantSide == COMBAT_SIDE_NONE)
                goto endRow;
        drawMoat:
            m_combatIcons[H2EnumIndex(COMBAT_ICON_MOAT)]
                ->CombatClipDrawToBuffer(
                    0, 0, H2EnumIndex(row), &m_moatLimits[H2EnumIndex(row)], ICON_DRAW_NORMAL, 0, NULL, NULL
                );
            m_hexCells[moatCell[H2EnumIndex(row)] - 1].DrawOccupant(ARMY_DRAW_ALL, 1);
            m_hexCells[moatCell[H2EnumIndex(row)]].DrawOccupant(ARMY_DRAW_ALL, 1);
            m_hexCells[moatCell[H2EnumIndex(row)] + 1].DrawOccupant(ARMY_DRAW_ALL, 1);
        }
    endRow:
        gridWasShowing1 = 0;
    }

    if (gConfig.combatArmyInfoLevel > 0 && m_smallViewSide[0] != COMBAT_SIDE_NONE) {
        DrawSmallView(0, 0);
        DrawSmallView(1, 0);
    }

    gpMouseManager->m_cursorReady = 1;
    PollSound();
    if (computeExtent != 0 || redrawExtent != 0) {
        redrawExtent = 0;
        gbLimitToExtent = false;
        gbComputeExtent = false;
        if (waitForTimer != 0)
            DelayTil(glTimers);
        glTimers[0] =
            static_cast<i32>(platform::Ticks() + gfCombatSpeedMod[gConfig.combatSpeed] * delay);
        gbFullCombatScreenDrawn = false;
        if (updateScreen == 1) {
            if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
                giMaxExtentY = COMBAT_MAX_EXTENT_Y;
            gbEnlargeScreenBlit = false;
            gpWindowManager->UpdateScreenRegion(
                giMinExtentX,
                giMinExtentY,
                giMaxExtentX - giMinExtentX + 1,
                giMaxExtentY - giMinExtentY + 1
            );
            gbEnlargeScreenBlit = true;
        }
    } else if (updateScreen == 1) {
        if (waitForTimer != 0)
            DelayTil(glTimers);
        gbFullCombatScreenDrawn = true;
        glTimers[0] =
            static_cast<i32>(platform::Ticks() + gfCombatSpeedMod[gConfig.combatSpeed] * delay);
        UpdateCombatArea();
    }

finish:
    gpMouseManager->m_cursorReady = 1;
    PollSound();
}

void combatManager::DrawSmallView(i32 viewIndex, i32 updateScreen) {
    u8 spellPositions4[COMBAT_SMALL_VIEW_MAX_SPELLS][COMBAT_SMALL_VIEW_MAX_SPELLS]
                     [SPELL_POSITION_AXIS_COUNT] = {
        {{COMBAT_SMALL_VIEW_SPELL_X_THIRD, COMBAT_SMALL_VIEW_SPELL_Y_SECOND},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION}},
        {{COMBAT_SMALL_VIEW_SPELL_X_SECOND, COMBAT_SMALL_VIEW_SPELL_Y_SECOND},
         {COMBAT_SMALL_VIEW_SPELL_X_FOURTH, COMBAT_SMALL_VIEW_SPELL_Y_SECOND},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION}},
        {{COMBAT_SMALL_VIEW_SPELL_X_FIRST, COMBAT_SMALL_VIEW_SPELL_Y_SECOND},
         {COMBAT_SMALL_VIEW_SPELL_X_THIRD, COMBAT_SMALL_VIEW_SPELL_Y_SECOND},
         {COMBAT_SMALL_VIEW_SPELL_X_FIFTH, COMBAT_SMALL_VIEW_SPELL_Y_SECOND},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION}},
        {{COMBAT_SMALL_VIEW_SPELL_X_SECOND, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
         {COMBAT_SMALL_VIEW_SPELL_X_FOURTH, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
         {COMBAT_SMALL_VIEW_SPELL_X_SECOND, COMBAT_SMALL_VIEW_SPELL_Y_THIRD},
         {COMBAT_SMALL_VIEW_SPELL_X_FOURTH, COMBAT_SMALL_VIEW_SPELL_Y_THIRD},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION}},
        {{COMBAT_SMALL_VIEW_SPELL_X_FIRST, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
         {COMBAT_SMALL_VIEW_SPELL_X_THIRD, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
         {COMBAT_SMALL_VIEW_SPELL_X_FIFTH, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
         {COMBAT_SMALL_VIEW_SPELL_X_SECOND, COMBAT_SMALL_VIEW_SPELL_Y_THIRD},
         {COMBAT_SMALL_VIEW_SPELL_X_FOURTH, COMBAT_SMALL_VIEW_SPELL_Y_THIRD},
         {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION}},
        {{COMBAT_SMALL_VIEW_SPELL_X_FIRST, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
         {COMBAT_SMALL_VIEW_SPELL_X_THIRD, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
         {COMBAT_SMALL_VIEW_SPELL_X_FIFTH, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
         {COMBAT_SMALL_VIEW_SPELL_X_FIRST, COMBAT_SMALL_VIEW_SPELL_Y_THIRD},
         {COMBAT_SMALL_VIEW_SPELL_X_THIRD, COMBAT_SMALL_VIEW_SPELL_Y_THIRD},
         {COMBAT_SMALL_VIEW_SPELL_X_FIFTH, COMBAT_SMALL_VIEW_SPELL_Y_THIRD}}
    };
    i32 oldLimit;
    i32 unusedSpell1;
    army* smallArmy;
    IconDrawResult drawResult1;
    i32 unusedSlot;
    i32 viewX1;
    i32 iconX9;
    i32 spellSlot1;
    i32 spellIcon;
    i32 viewY;
    i32 iconY9;
    i32 visibleSpellCount;

    if (gbNoShowCombat != 0)
        return;
    if (m_combatWindowOpen == 0)
        return;
    if (gConfig.combatArmyInfoLevel == 0)
        return;
    if (gbInDrawSmallView != 0)
        return;

    gbInDrawSmallView = true;
    if (m_smallViewSide[viewIndex] == COMBAT_SIDE_NONE
        || (m_smallViewLastX[viewIndex] == COMBAT_SMALL_VIEW_LEFT_X
            && m_smallViewSide[viewIndex] == COMBAT_DEFENDER_SIDE)
        || (m_smallViewLastX[viewIndex] == COMBAT_SMALL_VIEW_RIGHT_X
            && m_smallViewSide[viewIndex] == COMBAT_ATTACKER_SIDE)) {
        if (m_smallViewLastX[viewIndex] >= 0) {
            gbLimitToExtent = true;
            giMinExtentX = m_smallViewLastX[viewIndex];
            giMinExtentY = m_smallViewLastY[viewIndex];
            giMaxExtentX = m_smallViewLastX[viewIndex] + m_smallViewWidth[viewIndex] - 1;
            giMaxExtentY = m_smallViewLastY[viewIndex] + m_smallViewHeight[viewIndex] - 1;
            DrawFrame(0, 0, 0, 0, 0, 1, 1);
            gpWindowManager->UpdateScreenRegion(
                giMinExtentX,
                giMinExtentY,
                giMaxExtentX - giMinExtentX + 1,
                giMaxExtentY - giMinExtentY + 1
            );
            gbLimitToExtent = false;
            m_smallViewLastX[viewIndex] = -1;
        }
        if (m_smallViewSide[viewIndex] == COMBAT_SIDE_NONE) {
            gbInDrawSmallView = false;
            return;
        }
    }

    if (gConfig.combatArmyInfoLevel == COMBAT_SMALL_VIEW_FULL_INFO) {
        if (viewIndex == 0) {
            if (m_smallViewSide[viewIndex] == COMBAT_ATTACKER_SIDE)
                viewX1 = COMBAT_SMALL_VIEW_LEFT_X;
            else
                viewX1 = COMBAT_SMALL_VIEW_RIGHT_X;
            viewY = COMBAT_SMALL_VIEW_FULL_Y;
        } else {
            if (m_smallViewSide[viewIndex] == COMBAT_ATTACKER_SIDE)
                viewX1 = COMBAT_SMALL_VIEW_LEFT_X;
            else
                viewX1 = COMBAT_SMALL_VIEW_RIGHT_X;
            if (m_smallViewSide[1] == m_smallViewSide[0])
                viewY = COMBAT_SMALL_VIEW_FULL_RIGHT_Y;
            else
                viewY = COMBAT_SMALL_VIEW_FULL_Y;
        }
        m_smallViewWidth[viewIndex] = COMBAT_SMALL_VIEW_WIDTH;
        m_smallViewHeight[viewIndex] = COMBAT_SMALL_VIEW_FULL_HEIGHT;
    } else {
        if (viewIndex == 0) {
            if (m_smallViewSide[viewIndex] == COMBAT_ATTACKER_SIDE)
                viewX1 = COMBAT_SMALL_VIEW_LEFT_X;
            else
                viewX1 = COMBAT_SMALL_VIEW_RIGHT_X;
            viewY = COMBAT_SMALL_VIEW_COMPACT_Y;
        } else {
            if (m_smallViewSide[viewIndex] == COMBAT_ATTACKER_SIDE)
                viewX1 = COMBAT_SMALL_VIEW_LEFT_X;
            else
                viewX1 = COMBAT_SMALL_VIEW_RIGHT_X;
            if (m_smallViewSide[1] == m_smallViewSide[0])
                viewY = COMBAT_SMALL_VIEW_COMPACT_RIGHT_Y;
            else
                viewY = COMBAT_SMALL_VIEW_COMPACT_Y;
        }
        m_smallViewWidth[viewIndex] = COMBAT_SMALL_VIEW_WIDTH;
        m_smallViewHeight[viewIndex] = COMBAT_SMALL_VIEW_COMPACT_HEIGHT;
    }

    m_smallViewLastX[viewIndex] = viewX1;
    m_smallViewLastY[viewIndex] = viewY;
    oldLimit = gbLimitToExtent;
    if (updateScreen != 0)
        gbLimitToExtent = false;

    smallArmy = &m_armies[H2EnumIndex(m_smallViewSide[viewIndex])][m_smallViewArmyIndex[viewIndex]];
    drawResult1 = m_combatIcons[H2EnumIndex(COMBAT_ICON_SMALL_VIEW_BACKGROUND)]->CombatClipDrawToBuffer(
        viewX1,
        viewY,
        gConfig.combatArmyInfoLevel != COMBAT_SMALL_VIEW_FULL_INFO,
        &m_smallViewLimits,
        ICON_DRAW_NORMAL,
        0,
        NULL,
        NULL
    );
    viewX1 += COMBAT_SMALL_VIEW_INSET_X;
    if (drawResult1 != ICON_DRAW_SKIPPED) {
        if (smallArmy->m_quantity > 1)
            sprintf(
                gText,
                cMiniViewText[H2EnumIndex(SMALL_VIEW_TEXT_QUANTITY_PLURAL)],
                smallArmy->m_quantity
            );
        else
            sprintf(
                gText,
                cMiniViewText[H2EnumIndex(SMALL_VIEW_TEXT_QUANTITY_SINGULAR)],
                smallArmy->m_quantity
            );
        smallFont->DrawBoundedString(
            gText,
            viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
            viewY + COMBAT_SMALL_VIEW_QUANTITY_Y,
            COMBAT_SMALL_VIEW_TEXT_WIDTH,
            COMBAT_SMALL_VIEW_TEXT_HEIGHT,
            FONT_DRAW_DEFAULT,
            FONT_ALIGN_CENTER
        );

        if (gConfig.combatArmyInfoLevel == COMBAT_SMALL_VIEW_FULL_INFO) {
            smallFont->DrawBoundedString(
                cMiniViewText[H2EnumIndex(SMALL_VIEW_TEXT_ATTACK)],
                viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
                viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_ATTACK),
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_LEFT
            );
            smallFont->DrawBoundedString(
                cMiniViewText[H2EnumIndex(SMALL_VIEW_TEXT_DEFENSE)],
                viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
                viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_DEFENSE),
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_LEFT
            );
            smallFont->DrawBoundedString(
                cMiniViewText[H2EnumIndex(SMALL_VIEW_TEXT_HIT_POINTS)],
                viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
                viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_HIT_POINTS),
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_LEFT
            );
            smallFont->DrawBoundedString(
                cMiniViewText[H2EnumIndex(SMALL_VIEW_TEXT_DAMAGE)],
                viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
                viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_DAMAGE),
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_LEFT
            );
            smallFont->DrawBoundedString(
                cMiniViewText[H2EnumIndex(SMALL_VIEW_TEXT_MORALE)],
                viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
                viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_MORALE),
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_LEFT
            );
            smallFont->DrawBoundedString(
                cMiniViewText[H2EnumIndex(SMALL_VIEW_TEXT_LUCK)],
                viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
                viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_LUCK),
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_LEFT
            );
            if ((H2EnumIndex((smallArmy->m_monster.flags.all) & (COMBAT_ARMY_FLAG_SHOOTER))))
                smallFont->DrawBoundedString(
                    cMiniViewText[H2EnumIndex(SMALL_VIEW_TEXT_SHOTS)],
                    viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
                    viewY + COMBAT_SMALL_VIEW_SHOTS_Y,
                    COMBAT_SMALL_VIEW_TEXT_WIDTH,
                    COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                    FONT_DRAW_DEFAULT,
                    FONT_ALIGN_LEFT
                );

            sprintf(gText, "%d", static_cast<i32>(smallArmy->m_monster.attack));
            smallFont->DrawBoundedString(
                gText,
                viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
                viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_ATTACK),
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_RIGHT
            );
            sprintf(gText, "%d", static_cast<i32>(smallArmy->m_monster.defense));
            smallFont->DrawBoundedString(
                gText,
                viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
                viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_DEFENSE),
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_RIGHT
            );
            sprintf(gText, "%d", static_cast<u32>(smallArmy->m_monster.hitPoints));
            smallFont->DrawBoundedString(
                gText,
                viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
                viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_HIT_POINTS),
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_RIGHT
            );
            sprintf(
                gText,
                "%d-%d",
                static_cast<i32>(smallArmy->m_monster.damageMin),
                static_cast<i32>(smallArmy->m_monster.damageMax)
            );
            smallFont->DrawBoundedString(
                gText,
                viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
                viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_DAMAGE),
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_RIGHT
            );

            spellSlot1 = 0;
            if (smallArmy->m_morale > 0) {
                for (spellSlot1 = 0; spellSlot1 < smallArmy->m_morale; spellSlot1++)
                    m_combatIcons[H2EnumIndex(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
                        viewX1 + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
                            - spellSlot1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
                        viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                            + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_MORALE),
                        H2EnumIndex(SMALL_VIEW_GOOD_MORALE_FRAME),
                        ICON_DRAW_NORMAL
                    );
            } else if (smallArmy->m_morale < 0) {
                for (spellSlot1 = 0; spellSlot1 < -smallArmy->m_morale; spellSlot1++)
                    m_combatIcons[H2EnumIndex(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
                        viewX1 + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
                            - spellSlot1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
                        viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                            + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_MORALE),
                        H2EnumIndex(SMALL_VIEW_BAD_MORALE_FRAME),
                        ICON_DRAW_NORMAL
                    );
            } else {
                m_combatIcons[H2EnumIndex(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
                    viewX1 + COMBAT_SMALL_VIEW_NEUTRAL_MORALE_X,
                    viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                        + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_MORALE),
                    H2EnumIndex(SMALL_VIEW_NEUTRAL_MORALE_FRAME),
                    ICON_DRAW_NORMAL
                );
            }

            spellSlot1 = 0;
            if (smallArmy->m_luck > 0) {
                for (spellSlot1 = 0; spellSlot1 < smallArmy->m_luck; spellSlot1++)
                    m_combatIcons[H2EnumIndex(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
                        viewX1 + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
                            - spellSlot1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
                        viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                            + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_LUCK),
                        H2EnumIndex(SMALL_VIEW_GOOD_LUCK_FRAME),
                        ICON_DRAW_NORMAL
                    );
            } else if (smallArmy->m_luck < 0) {
                for (spellSlot1 = 0; spellSlot1 < -smallArmy->m_luck; spellSlot1++)
                    m_combatIcons[H2EnumIndex(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
                        viewX1 + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
                            - spellSlot1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
                        viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                            + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_LUCK),
                        H2EnumIndex(SMALL_VIEW_BAD_LUCK_FRAME),
                        ICON_DRAW_NORMAL
                    );
            } else {
                m_combatIcons[H2EnumIndex(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
                    viewX1 + COMBAT_SMALL_VIEW_NEUTRAL_LUCK_X,
                    viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                        + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * H2EnumIndex(STAT_ROW_LUCK),
                    H2EnumIndex(SMALL_VIEW_NEUTRAL_LUCK_FRAME),
                    ICON_DRAW_NORMAL
                );
            }

            if ((H2EnumIndex((smallArmy->m_monster.flags.all) & (COMBAT_ARMY_FLAG_SHOOTER)))) {
                sprintf(gText, "%d", static_cast<i32>(smallArmy->m_monster.shots));
                smallFont->DrawBoundedString(
                    gText,
                    viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
                    viewY + COMBAT_SMALL_VIEW_SHOTS_Y,
                    COMBAT_SMALL_VIEW_TEXT_WIDTH,
                    COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                    FONT_DRAW_DEFAULT,
                    FONT_ALIGN_RIGHT
                );
            }
        }

        visibleSpellCount = smallArmy->m_spellCount < COMBAT_SMALL_VIEW_MAX_SPELLS
                                 ? smallArmy->m_spellCount
                                 : COMBAT_SMALL_VIEW_MAX_SPELLS;
        spellIcon = -1;
        for (spellSlot1 = 0; spellSlot1 < visibleSpellCount; spellSlot1++) {
            spellIcon++;
            while (smallArmy->m_spellInfluence[spellIcon] == 0)
                spellIcon++;

            iconX9 = viewX1 + COMBAT_SMALL_VIEW_INSET_X;
            if (gConfig.combatArmyInfoLevel == COMBAT_SMALL_VIEW_FULL_INFO)
                iconY9 = viewY + COMBAT_SMALL_VIEW_FULL_SPELL_Y;
            else
                iconY9 = viewY + COMBAT_SMALL_VIEW_COMPACT_SPELL_Y;
            iconX9 += spellPositions4[visibleSpellCount - 1][spellSlot1][0];
            iconY9 += spellPositions4[visibleSpellCount - 1][spellSlot1][1];
            iconX9 +=
                (COMBAT_SMALL_VIEW_ICON_SIZE
                 - GetIconEntry(m_combatIcons[H2EnumIndex(COMBAT_ICON_SMALL_VIEW_SPELL)], spellIcon)->w)
                >> 1;
            iconY9 +=
                (COMBAT_SMALL_VIEW_ICON_SIZE
                 - GetIconEntry(m_combatIcons[H2EnumIndex(COMBAT_ICON_SMALL_VIEW_SPELL)], spellIcon)->h)
                >> 1;
            m_combatIcons[H2EnumIndex(COMBAT_ICON_SMALL_VIEW_SPELL)]
                ->DrawToBuffer(iconX9, iconY9, spellIcon, ICON_DRAW_NORMAL);
        }
    }

    gbLimitToExtent = oldLimit;
    viewX1 -= COMBAT_SMALL_VIEW_INSET_X;
    if (updateScreen != 0)
        gpWindowManager->UpdateScreenRegion(
            viewX1,
            viewY,
            m_smallViewWidth[viewIndex],
            m_smallViewHeight[viewIndex]
        );
    gbInDrawSmallView = false;
}

i32 bGridWasShowing = 0;
b32 gbInDrawSmallView = false;
