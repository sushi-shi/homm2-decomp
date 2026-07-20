#include <va.h>
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
#include <SOURCE/kbwin.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/town.h>
H2_ENUM_CLASS_BEGIN(CombatDrawLayer)
    DRAW_LAYER_COUNT       = 9,
    DRAW_RIGHT_HERO_LAYER  = 1,
    DRAW_LEFT_HERO_LAYER   = 2,
    DRAW_WALL_TOP_LAYER    = 4,
    DRAW_WALL_MIDDLE_LAYER = 5,
    DRAW_GATE_LAYER        = 6,
    DRAW_CATAPULT_LAYER    = 7
H2_ENUM_CLASS_END(CombatDrawLayer)

H2_ENUM_CLASS_BEGIN(CombatSmallViewText)
    SMALL_VIEW_TEXT_QUANTITY_PLURAL   = 0,
    SMALL_VIEW_TEXT_QUANTITY_SINGULAR = 1,
    SMALL_VIEW_TEXT_ATTACK            = 2,
    SMALL_VIEW_TEXT_DEFENSE           = 3,
    SMALL_VIEW_TEXT_HIT_POINTS        = 4,
    SMALL_VIEW_TEXT_DAMAGE            = 5,
    SMALL_VIEW_TEXT_MORALE            = 6,
    SMALL_VIEW_TEXT_LUCK              = 7,
    SMALL_VIEW_TEXT_SHOTS             = 8
H2_ENUM_CLASS_END(CombatSmallViewText)

H2_ENUM_CLASS_BEGIN(CombatSmallViewModifierFrame)
    SMALL_VIEW_BAD_LUCK_FRAME       = 0,
    SMALL_VIEW_GOOD_LUCK_FRAME      = 1,
    SMALL_VIEW_NEUTRAL_LUCK_FRAME   = 2,
    SMALL_VIEW_BAD_MORALE_FRAME     = 3,
    SMALL_VIEW_GOOD_MORALE_FRAME    = 4,
    SMALL_VIEW_NEUTRAL_MORALE_FRAME = 5
H2_ENUM_CLASS_END(CombatSmallViewModifierFrame)

H2_ENUM_CLASS_BEGIN(CombatGridShade)
    GRID_SHADE_NONE          = 0,
    GRID_SHADE_REACHABLE     = 1,
    GRID_SHADE_EMPTY_BLOCKED = 3
H2_ENUM_CLASS_END(CombatGridShade)

H2_ENUM_CLASS_BEGIN(CombatMessageText)
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
H2_ENUM_CLASS_END(CombatMessageText)

H2_ENUM_BEGIN(CombatDrawingConstant)
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
H2_ENUM_END(CombatDrawingConstant)

H2_ENUM_BEGIN(CombatWallCoordinateIndex)
    WALL_TOP_X    = 0,
    WALL_TOP_Y    = 1,
    WALL_SECOND_X = 2,
    WALL_SECOND_Y = 3,
    WALL_THIRD_X  = 4,
    WALL_THIRD_Y  = 5,
    WALL_BOTTOM_X = 6,
    WALL_BOTTOM_Y = 7
H2_ENUM_END(CombatWallCoordinateIndex)

H2_ENUM_BEGIN(CombatSmallViewStatRow)
    STAT_ROW_ATTACK     = 0,
    STAT_ROW_DEFENSE    = 1,
    STAT_ROW_HIT_POINTS = 2,
    STAT_ROW_DAMAGE     = 3,
    STAT_ROW_MORALE     = 4,
    STAT_ROW_LUCK       = 5
H2_ENUM_END(CombatSmallViewStatRow)

VA(0x00402910, 0x41)
void combatManager::NoShowCombatLog(char* message) {
    char logMessage[COMBAT_MESSAGE_LOG_BUFFER_SIZE];
    sprintf(logMessage, "NC: %s", message);
    LogStr(logMessage);
}

VA(0x00402951, 0xda)
void combatManager::ClearCombatMessages(i32 force) {
    m_combatMessagePending = 0;
    if (strlen(m_currentCombatMessage) <= 1 && strlen(m_previousCombatMessage) <= 1)
        return;
    if (force != 0 || m_combatMessageExpiration < KBTickCount()) {
        strcpy(m_previousCombatMessage, "");
        strcpy(m_currentCombatMessage, "");
        m_previousCombatMessageExpiration = 0;
        m_combatMessageExpiration = m_previousCombatMessageExpiration;
        CombatMessage("", 1, 0, 0);
    }
}

VA(0x00402a2b, 0x5d)
void combatManager::CheckUpdateCombatMessages(void) {
    if (m_combatMessagePending != 0 && m_combatMessageExpiration != 0
        && m_combatMessageExpiration < KBTickCount())
        CombatMessage("", 1, 0, 1);
}

VA(0x00402a88, 0x3f8)
void combatManager::CombatMessage(char* message, i32 updateScreen, i32 retainPrevious, i32 clear) {
    char* newlinePtr;
    char wrappedMessage[COMBAT_MESSAGE_WRAP_BUFFER_SIZE];
    tag_message windowMessage;
    i32 savedLimitToExtent;
    i32 savedComputeExtent;

    if (gbNoShowCombat != 0) {
        if (retainPrevious != 0)
            NoShowCombatLog(message);
        return;
    }
    if (m_combatWindowOpen == 0)
        return;
    if (m_nonVisualCombat != 0)
        return;

    if (clear != 0) {
        strcpy(m_previousCombatMessage, "");
        strcpy(m_currentCombatMessage, message);
        m_previousCombatMessageExpiration = 0;
        m_combatMessageExpiration = m_previousCombatMessageExpiration;
        m_combatMessagePending = 0;
    } else {
        if (retainPrevious == 0) {
            if (KBTickCount() < m_combatMessageExpiration) {
                return;
            } else {
                strcpy(m_previousCombatMessage, "");
                m_previousCombatMessageExpiration = 0;
                m_combatMessageExpiration = m_previousCombatMessageExpiration;
            }
        } else {
            if (m_combatMessageExpiration != 0)
                strcpy(m_previousCombatMessage, m_currentCombatMessage);
            else
                strcpy(m_previousCombatMessage, "");
            m_previousCombatMessageExpiration = m_combatMessageExpiration;
            m_combatMessageExpiration = KBTickCount() + COMBAT_MESSAGE_TIMEOUT;
        }

        newlinePtr = FindToken(message, '\n');
        if (newlinePtr != NULL) {
            *newlinePtr = 0;
            strcpy(wrappedMessage, message);
            if (newlinePtr > message && newlinePtr[-1] == '.')
                strcat(wrappedMessage, "  ");
            else
                strcat(wrappedMessage, " ");
            strcat(wrappedMessage, newlinePtr + 1);
            if (bigFont->LineLength(wrappedMessage, COMBAT_MESSAGE_LINE_WIDTH) <= 1) {
                strcpy(m_currentCombatMessage, wrappedMessage);
            } else {
                strcpy(m_previousCombatMessage, message);
                strcpy(m_currentCombatMessage, newlinePtr + 1);
            }
            *newlinePtr = '\n';
        } else {
            strcpy(m_currentCombatMessage, message);
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

    savedComputeExtent = gbComputeExtent;
    savedLimitToExtent = gbLimitToExtent;
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
    gbComputeExtent = savedComputeExtent;
    gbLimitToExtent = savedLimitToExtent;
}

VA(0x00402e80, 0x3fe)
void combatManager::CombatMessage(CombatMessageCommand messageType) {
    army* currentArmyPtr;
    army* targetArmy;
    CreatureType actingMonsterType;
    CreatureType targetMonsterType;

    if (m_combatWindowOpen == 0)
        return;
    if (gbNoShowCombat != 0)
        return;

    currentArmyPtr = &m_armies[m_currentArmySide][m_currentArmyIndex];
    actingMonsterType = currentArmyPtr->m_monsterType;
    targetArmy = NULL;
    targetMonsterType = CreatureType(0);
    if (currentArmyPtr->m_targetSide >= 0 && currentArmyPtr->m_targetIndex >= 0) {
        targetArmy = &m_armies[currentArmyPtr->m_targetSide][currentArmyPtr->m_targetIndex];
        targetMonsterType = targetArmy->m_monsterType;
    }

    switch (messageType) {
        case COMBAT_MESSAGE_COMMAND_DEFAULT:
            if (HAS(currentArmyPtr->m_monster.flags.all, COMBAT_ARMY_FLAG_SHOOTER) != 0
                && currentArmyPtr->m_monster.shots == 0 && targetArmy != NULL)
                strcpy(gText, cCombatMessage[IDX(MESSAGE_TEXT_NO_SHOTS)]);
            else
                strcpy(gText, cCombatMessage[IDX(MESSAGE_TEXT_DEFAULT)]);
            break;
        case COMBAT_MESSAGE_COMMAND_MOVE:
            sprintf(
                gText,
                cCombatMessage[IDX(MESSAGE_TEXT_MOVE)],
                gArmyNames[IDX(actingMonsterType)]
            );
            break;
        case COMBAT_MESSAGE_COMMAND_FLY:
            sprintf(
                gText,
                cCombatMessage[IDX(MESSAGE_TEXT_FLY)],
                gArmyNames[IDX(actingMonsterType)]
            );
            break;
        case COMBAT_MESSAGE_COMMAND_ATTACK:
            sprintf(
                gText,
                cCombatMessage[IDX(MESSAGE_TEXT_ATTACK)],
                gArmyNames[IDX(targetMonsterType)]
            );
            break;
        case COMBAT_MESSAGE_COMMAND_SHOOT:
            sprintf(
                gText,
                cCombatMessage[IDX(MESSAGE_TEXT_SHOOT)],
                gArmyNames[IDX(targetMonsterType)],
                static_cast<i32>(currentArmyPtr->m_monster.shots)
            );
            break;
        case COMBAT_MESSAGE_COMMAND_OPTIONS:
            if (m_heroes[m_currentSide] != NULL && m_heroes[m_currentSide]->m_isCaptain != 0)
                strcpy(gText, cCombatMessage[IDX(MESSAGE_TEXT_CAPTAIN_OPTIONS)]);
            else
                strcpy(gText, cCombatMessage[IDX(MESSAGE_TEXT_HERO_OPTIONS)]);
            break;
        case COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS:
            if (m_heroes[1 - m_currentSide] != NULL && m_heroes[1 - m_currentSide]->m_isCaptain != 0)
                strcpy(gText, cCombatMessage[IDX(MESSAGE_TEXT_OPPOSING_CAPTAIN)]);
            else
                strcpy(gText, cCombatMessage[IDX(MESSAGE_TEXT_OPPOSING_HERO)]);
            break;
        case COMBAT_MESSAGE_COMMAND_VIEW_INFO:
            if (m_selectedHex == COMBAT_BALLISTA_HEX) {
                sprintf(gText, cCombatMessage[IDX(MESSAGE_TEXT_BALLISTA)]);
            } else {
                actingMonsterType =
                    m_armies[m_currentArmySide][m_hexCells[m_selectedHex].m_occupantIndex]
                        .m_monsterType;
                if (IDX(actingMonsterType) >= 0)
                    sprintf(
                        gText,
                        cCombatMessage[IDX(MESSAGE_TEXT_VIEW_INFO)],
                        gArmyNames[IDX(actingMonsterType)]
                    );
                else
                    sprintf(gText, "");
            }
            break;
    }
    CombatMessage(gText, 1, 0, 0);
}

VA(0x0040327e, 0x126)
void combatManager::ResetLimitCreature(void) {
    i32 side;
    i32 armySlotIndex;

    for (side = 0; side < COMBAT_SIDE_COUNT_DRAWING; side++) {
        for (armySlotIndex = 0; armySlotIndex < COMBAT_ARMY_SLOT_COUNT_DRAWING; armySlotIndex++) {
            if (HAS(m_armies[side][armySlotIndex].m_monster.flags.all,
                    COMBAT_ARMY_FLAG_MIRROR_IMAGE)
                != 0)
                m_limitCreatureCount[side][OD_STEER(armySlotIndex)] = -1;
            else
                m_limitCreatureCount[side][OD_STEER(armySlotIndex)] = 0;
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

VA(0x004033a4, 0x6a)
void combatManager::UpdateCombatArea(void) {
    if (gbNoShowCombat != 0)
        return;
    if (m_combatWindowOpen == 0)
        return;

    gbEnlargeScreenBlit = false;
    gpWindowManager->UpdateScreenRegion(0, 0, COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT);
    gbEnlargeScreenBlit = true;
}

VA(0x0040340e, 0x213)
void combatManager::SetupGridForArmy(army* armyPtr) {
    i32 attackMask;
    i32 savedTargetSide;
    i32 targetIndexSave;
    i32 hexIndex;

    if (gbNoShowCombat != 0)
        return;
    if (gConfig.combatShadeLevel < 1)
        return;

    attackMask =
        armyPtr->GetAttackMask(armyPtr->m_hex, ARMY_ATTACK_TARGET_OCCUPIED, ARMY_HEX_INVALID);
    memset(m_gridState, IDX(GRID_SHADE_NONE), sizeof(m_gridState));
    savedTargetSide = armyPtr->m_targetSide;
    targetIndexSave = armyPtr->m_targetIndex;
    armyPtr->m_targetSide = -1;
    armyPtr->m_targetIndex = -1;
    gpSearchArray->SeedCombatPosition(armyPtr);
    armyPtr->m_targetSide = savedTargetSide;
    armyPtr->m_targetIndex = targetIndexSave;

    for (hexIndex = 0; hexIndex < COMBAT_HEX_COUNT; hexIndex++) {
        if (armyPtr->m_hex == hexIndex) {
            m_gridState[hexIndex] = IDX(GRID_SHADE_REACHABLE);
        } else if (m_hexCells[hexIndex].m_pathReachable != 0) {
            if (m_hexCells[hexIndex].m_occupantSide != COMBAT_OCCUPANT_NONE) {
                if (m_hexCells[hexIndex].m_occupantSide != armyPtr->m_side)
                    m_gridState[hexIndex] = IDX(GRID_SHADE_REACHABLE);
            } else {
                m_gridState[hexIndex] = IDX(GRID_SHADE_EMPTY_BLOCKED);
            }
        } else if (m_hexCells[hexIndex].m_occupantSide != COMBAT_OCCUPANT_NONE
                   && m_hexCells[hexIndex].m_occupantSide != 1 - m_currentSide
                   && (attackMask & (1 << m_hexCells[hexIndex].m_occupantIndex)) != 0) {
            m_gridState[hexIndex] = IDX(GRID_SHADE_REACHABLE);
        }
    }
}

VA(0x00403621, 0x5fb)
i32 combatManager::UpdateGrid(i32 resetGridDisplay, i32 rebuildGrid) {
    i32 retval;
    i32 minX;
    i32 minY;
    i32 maxX;
    i32 maxY;
    i32 cellIndex;
    i32 gridChanged;
    i32 drawShading;
    i32 hadOldShade;

    if (gbNoShowCombat != 0)
        return 0;

    if (rebuildGrid != 0) {
        if (m_playerId[m_currentSide] == -1 || gbThisNetHumanPlayer[m_playerId[m_currentSide]] == 0
            || m_gridSelectionDisabled != 0) {
            memset(m_gridState, IDX(GRID_SHADE_NONE), sizeof(m_gridState));
        } else {
            SetupGridForArmy(&m_armies[OD_STEER(m_currentArmySide)][m_currentArmyIndex]);
        }
    }
    if (resetGridDisplay != 0)
        bGridWasShowing = 0;
    if (gConfig.combatShadeLevel < 1 && gConfig.showCombatGrid == 0)
        return 0;

    retval = 0;
    minX = COMBAT_MAX_EXTENT_X;
    minY = COMBAT_MAX_EXTENT_Y;
    maxX = 0;
    maxY = 0;
    drawShading = 0;
    hadOldShade = 0;
    gridChanged = 0;

    if (gConfig.combatShadeLevel < 1)
        goto DrawCombatGrid;

    for (cellIndex = 0; cellIndex < COMBAT_HEX_COUNT; cellIndex++) {
        if (m_previousGridState[cellIndex] != m_gridState[cellIndex])
            gridChanged = 1;
        if (m_gridState[cellIndex] != IDX(GRID_SHADE_NONE))
            drawShading = 1;
        if (m_previousGridState[cellIndex] != IDX(GRID_SHADE_NONE))
            hadOldShade = 1;
    }

    if (resetGridDisplay != 0) {
        if (drawShading == 0)
            goto DrawCombatGrid;
    } else {
        if (gridChanged == 0)
            return 0;
        if (hadOldShade != 0) {
            for (cellIndex = 0; cellIndex < COMBAT_HEX_COUNT; cellIndex++) {
                if (m_previousGridState[cellIndex] != m_gridState[cellIndex]
                    || m_gridState[cellIndex] != IDX(GRID_SHADE_NONE)) {
                    if (m_hexCells[cellIndex].m_gridLeft < minX)
                        minX = m_hexCells[cellIndex].m_gridLeft;
                    if (m_hexCells[cellIndex].m_gridTop < minY)
                        minY = m_hexCells[cellIndex].m_gridTop;
                    if (m_hexCells[cellIndex].m_gridRight > maxX)
                        maxX = m_hexCells[cellIndex].m_gridRight;
                    if (m_hexCells[cellIndex].m_gridBottom > maxY)
                        maxY = m_hexCells[cellIndex].m_gridBottom;
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
            retval = 1;
        }
    }

    if (drawShading == 0)
        goto DrawCombatGrid;
    for (cellIndex = 0; cellIndex < COMBAT_HEX_COUNT; cellIndex++) {
        if (m_gridState[cellIndex] != IDX(GRID_SHADE_NONE)) {
            DimIconToBitmap(
                m_combatIcons[IDX(COMBAT_ICON_GRID)],
                m_backgroundBuffer,
                m_hexCells[cellIndex].m_gridLeft,
                m_hexCells[cellIndex].m_gridTop,
                1,
                m_gridState[cellIndex] - 1,
                1,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT
            );
            retval = 1;
        }
    }

DrawCombatGrid:
    if (gConfig.showCombatGrid != 0) {
        if (bGridWasShowing != 0 && retval == 0)
            goto CopyGridState;
        for (cellIndex = 0; cellIndex < COMBAT_HEX_COUNT; cellIndex++) {
            if (cellIndex % COMBAT_GRID_ROW_LENGTH != 0
                && cellIndex % COMBAT_GRID_ROW_LENGTH != COMBAT_GRID_ROW_LENGTH - 1) {
                MonoIconToBitmap(
                    m_combatIcons[IDX(COMBAT_ICON_GRID)],
                    m_backgroundBuffer,
                    m_hexCells[cellIndex].m_gridLeft,
                    m_hexCells[cellIndex].m_gridTop,
                    COMBAT_GRID_LINE_FRAME,
                    COMBAT_GRID_LINE_COLOR,
                    1,
                    0,
                    0,
                    COMBAT_SCREEN_WIDTH,
                    COMBAT_AREA_HEIGHT
                );
            }
        }
        retval = 1;
        bGridWasShowing = 1;
    }

CopyGridState:
    memcpy(m_previousGridState, m_gridState, sizeof(m_previousGridState));
    return retval;
}

VA(0x00403c1c, 0x364)
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
        1,
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
            0,
            0,
            0,
            COMBAT_SCREEN_WIDTH,
            COMBAT_AREA_HEIGHT,
            0
        );
        gpResourceManager->Dispose(backgroundIcon);
    }
    if (m_battlefieldFringe != FRINGE_NONE) {
        sprintf(gText, "frng%04d.icn", IDX(m_battlefieldFringe));
        backgroundIcon = gpResourceManager->GetIcon(gText);
        if (m_inCastleCombat != 0)
            IconToBitmap(
                backgroundIcon,
                m_backgroundBuffer,
                0,
                0,
                0,
                1,
                0,
                0,
                COMBAT_SCREEN_WIDTH / COMBAT_MANAGER_SIDE_COUNT,
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
                0,
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
            cHeroTypeInitial[IDX(m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type)]
        );
        backgroundIcon = gpResourceManager->GetIcon(gText);
        IconToBitmap(
            backgroundIcon,
            m_backgroundBuffer,
            0,
            0,
            COMBAT_CASTLE_BACKGROUND_BASE_FRAME,
            0,
            0,
            0,
            COMBAT_SCREEN_WIDTH,
            COMBAT_AREA_HEIGHT,
            0
        );
        if (m_drawbridgeBackgroundVisible != 0)
            IconToBitmap(
                m_combatIcons[IDX(COMBAT_ICON_DRAWBRIDGE)],
                m_backgroundBuffer,
                0,
                0,
                0,
                0,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
        if (m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type == FACTION_KNIGHT
            && (m_combatTowns[COMBAT_DEFENDER_SIDE]->m_buildings & IDX(TOWN_BUILDING_RAINBOW)) != 0)
            IconToBitmap(
                backgroundIcon,
                m_backgroundBuffer,
                0,
                0,
                COMBAT_CASTLE_BACKGROUND_BUILDING_FRAME,
                0,
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
                0,
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
    m_backgroundDrawn = 1;
}

VA(0x00403f80, 0x64c)
void combatManager::UpdateMouseGrid(i32 hexIndex, i32 forceUpdate) {
    i32 oldLimit;
    i32 copyHeight;
    i32 maxXSave;
    i32 oldMinX;
    i32 oldMaxYBackup;
    i32 savedComputeExtents;
    i32 savedExtentMinY;

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
    if (m_mouseGridHex == hexIndex)
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
            m_combatIcons[IDX(COMBAT_ICON_GRID)],
            m_backgroundBuffer,
            m_hexCells[hexIndex].m_gridLeft,
            m_hexCells[hexIndex].m_gridTop,
            1,
            COMBAT_GRID_MOUSE_FRAME,
            1,
            0,
            0,
            COMBAT_SCREEN_WIDTH,
            COMBAT_AREA_HEIGHT
        );
    }

    oldMinX = giMinExtentX;
    savedExtentMinY = giMinExtentY;
    maxXSave = giMaxExtentX;
    oldMaxYBackup = giMaxExtentY;
    oldLimit = gbLimitToExtent;
    savedComputeExtents = gbComputeExtent;
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
        if (m_hexCells[hexIndex].m_gridLeft < giMinExtentX)
            giMinExtentX = m_hexCells[hexIndex].m_gridLeft;
        if (m_hexCells[hexIndex].m_gridTop < giMinExtentY)
            giMinExtentY = m_hexCells[hexIndex].m_gridTop;
        if (m_hexCells[hexIndex].m_gridLeft + COMBAT_MOUSE_HEX_MAX_X_OFFSET > giMaxExtentX)
            giMaxExtentX = m_hexCells[hexIndex].m_gridLeft + COMBAT_MOUSE_HEX_MAX_X_OFFSET;
        if (m_hexCells[hexIndex].m_gridTop + COMBAT_MOUSE_HEX_MAX_Y_OFFSET > giMaxExtentY)
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
    giMinExtentX = oldMinX;
    giMinExtentY = savedExtentMinY;
    giMaxExtentX = maxXSave;
    giMaxExtentY = oldMaxYBackup;
    gbLimitToExtent = oldLimit;
    gbComputeExtent = savedComputeExtents;
    m_mouseGridHex = hexIndex;
}

VA(0x004045cc, 0x173f)
void combatManager::DrawFrame(
    i32 updateScreen,
    i32 computeExtent,
    i32 redrawExtent,
    i32 extentOnly,
    i32 delay,
    i32 drawBackground,
    i32 waitForTimer
) {
    i32 column1;
    i32 unusedDrawState11;
    i32 unusedDrawState05;
    i32 side5;
    i32 row;
    i32 unusedDrawState213;
    i32 unusedDrawState3;
    i32 armyIndex2;
    i32 unusedDrawState4;
    i32 extentChanged1;
    i32 firstColumn;
    i32 skipSpecialOccupants6;
    i32 columnStep3;
    i32 endColumn;
    i32 gridWasShowing1;

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
        for (side5 = 0; side5 < COMBAT_SIDE_COUNT_DRAWING; side5++) {
            for (armyIndex2 = 0; armyIndex2 < COMBAT_ARMY_SLOT_COUNT_DRAWING; armyIndex2++) {
                if (m_limitCreatureCount[side5][armyIndex2] > 0) {
                    extentChanged1 = 1;
                    gbComputeExtent = true;
                    gbSaveBiggestExtent = true;
                    gbReturnAfterComputeExtent = true;
                    m_armies[side5][armyIndex2].DrawToBuffer(
                        m_hexCells[m_armies[side5][armyIndex2].m_hex].m_x,
                        m_hexCells[m_armies[side5][armyIndex2].m_hex].m_y,
                        0
                    );
                    gbReturnAfterComputeExtent = false;
                    gbComputeExtent = false;
                    gbSaveBiggestExtent = false;
                }
            }
        }

        for (side5 = 0; side5 < COMBAT_SIDE_COUNT_DRAWING; side5++) {
            if (m_drawHero[side5] != 0) {
                extentChanged1 = 1;
                gbComputeExtent = true;
                gbSaveBiggestExtent = true;
                gbReturnAfterComputeExtent = true;
                m_heroIcons[side5]->CombatClipDrawToBuffer(
                    side5 == 0 ? COMBAT_HERO_LEFT_X : COMBAT_HERO_RIGHT_X,
                    side5 == 0 ? COMBAT_HERO_LEFT_Y : COMBAT_HERO_RIGHT_Y,
                    sCmbtHero[m_heroSpriteIndex[side5]]
                        .animationFrames[m_heroAnimationState[side5]][m_heroAnimationFrame[side5]],
                    &m_heroLimits[side5],
                    1,
                    0,
                    NULL,
                    NULL
                );
                gbReturnAfterComputeExtent = false;
                gbComputeExtent = false;
                gbSaveBiggestExtent = false;
            }

            if (m_drawHeroOverlay[side5] != 0) {
                extentChanged1 = 1;
                gbComputeExtent = true;
                gbSaveBiggestExtent = true;
                gbReturnAfterComputeExtent = true;
                m_heroOverlayIcons[side5]->CombatClipDrawToBuffer(
                    side5 == 0 ? COMBAT_HERO_LEFT_X
                               : ((m_heroes[side5]->m_isCaptain ? -1 : 0)
                                  & (COMBAT_HERO_RIGHT_ALT_X - COMBAT_HERO_RIGHT_X))
                                     + COMBAT_HERO_RIGHT_X,
                    side5 == 0 ? COMBAT_HERO_LEFT_Y
                               : ((m_heroes[side5]->m_isCaptain ? -1 : 0)
                                  & (COMBAT_HERO_RIGHT_ALT_Y - COMBAT_HERO_RIGHT_Y))
                                     + COMBAT_HERO_RIGHT_Y,
                    m_heroOverlayFrame[side5],
                    &m_heroOverlayLimits[side5],
                    1,
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

    for (row = 0; row < IDX(DRAW_LAYER_COUNT); row++) {
        if (row == IDX(DRAW_RIGHT_HERO_LAYER) && m_heroes[1] != NULL) {
            m_heroIcons[1]->CombatClipDrawToBuffer(
                ((m_heroes[1]->m_isCaptain ? -1 : 0)
                 & (COMBAT_HERO_RIGHT_ALT_X - COMBAT_HERO_RIGHT_X))
                    + COMBAT_HERO_RIGHT_X,
                ((m_heroes[1]->m_isCaptain ? -1 : 0)
                 & (COMBAT_HERO_RIGHT_ALT_Y - COMBAT_HERO_RIGHT_Y))
                    + COMBAT_HERO_RIGHT_Y,
                sCmbtHero[m_heroSpriteIndex[COMBAT_DEFENDER_SIDE]]
                    .animationFrames[m_heroAnimationState[COMBAT_DEFENDER_SIDE]]
                                    [m_heroAnimationFrame[COMBAT_DEFENDER_SIDE]],
                &m_heroLimits[1],
                1,
                0,
                NULL,
                NULL
            );
            m_heroOverlayIcons[1]->CombatClipDrawToBuffer(
                COMBAT_HERO_RIGHT_X,
                COMBAT_HERO_RIGHT_Y,
                m_heroOverlayFrame[1],
                &m_heroOverlayLimits[1],
                1,
                0,
                NULL,
                NULL
            );
        }

        if (row == IDX(DRAW_LEFT_HERO_LAYER) && m_heroes[0] != NULL) {
            m_heroIcons[0]->CombatClipDrawToBuffer(
                COMBAT_HERO_LEFT_X,
                COMBAT_HERO_LEFT_Y,
                sCmbtHero[m_heroSpriteIndex[COMBAT_ATTACKER_SIDE]]
                    .animationFrames[m_heroAnimationState[COMBAT_ATTACKER_SIDE]]
                                    [m_heroAnimationFrame[COMBAT_ATTACKER_SIDE]],
                &m_heroLimits[0],
                0,
                0,
                NULL,
                NULL
            );
            m_heroOverlayIcons[0]->CombatClipDrawToBuffer(
                COMBAT_HERO_LEFT_X,
                COMBAT_HERO_LEFT_Y,
                m_heroOverlayFrame[0],
                &m_heroOverlayLimits[0],
                0,
                0,
                NULL,
                NULL
            );
        }

        firstColumn = COMBAT_GRID_FIRST_COLUMN;
        endColumn = COMBAT_GRID_COLUMN_END;
        columnStep3 = 1;
        if (m_inCastleCombat != 0 && row >= COMBAT_CASTLE_REVERSE_ROW) {
            firstColumn = COMBAT_GRID_REVERSE_FIRST_COLUMN;
            endColumn = COMBAT_GRID_REVERSE_COLUMN_END;
            columnStep3 = -1;
        }

        if (m_inCastleCombat != 0 && row == IDX(DRAW_CATAPULT_LAYER)) {
            m_combatIcons[IDX(COMBAT_ICON_CATAPULT)]->CombatClipDrawToBuffer(
                COMBAT_CATAPULT_X,
                COMBAT_CATAPULT_Y,
                m_catapultFrame[COMBAT_ATTACKER_SIDE],
                &m_catapultLimits,
                0,
                0,
                NULL,
                NULL
            );
        }
        if (m_inCastleCombat != 0 && row == IDX(DRAW_WALL_TOP_LAYER)
            && m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN) {
            m_combatIcons[IDX(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
                0,
                0,
                IDX(m_drawbridgeState) + DRAWBRIDGE_TOWER_FRAME_BASE,
                &m_upperWallLimits,
                0,
                0,
                NULL,
                NULL
            );
        }
        if (m_inCastleCombat != 0 && row == IDX(DRAW_WALL_MIDDLE_LAYER)
            && m_drawbridgeState == COMBAT_DRAWBRIDGE_LOWERED) {
            m_combatIcons[IDX(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
                0,
                0,
                COMBAT_CASTLE_TOP_LAYER_FRAME,
                &m_middleWallLimits,
                0,
                0,
                NULL,
                NULL
            );
        }
        if (m_inCastleCombat != 0 && row == IDX(DRAW_GATE_LAYER)) {
            m_combatIcons[IDX(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
                0,
                0,
                m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)]
                            == IDX(COMBAT_WALL_STATE_KEEP_STANDING)
                    ? COMBAT_CASTLE_GATE_CLOSED_FRAME
                    : COMBAT_CASTLE_GATE_FRAME,
                &m_gateLimits,
                0,
                0,
                NULL,
                NULL
            );
        }

        skipSpecialOccupants6 = 0;
        if (m_inCastleCombat != 0
            && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FOURTH)]
                   != IDX(COMBAT_WALL_STATE_DESTROYED)
            && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FOURTH)]
                   != IDX(COMBAT_WALL_STATE_SECTION_DESTROYED)) {
            skipSpecialOccupants6 = 1;
            for (side5 = 0; side5 < ARMY_DRAW_PHASE_COUNT; side5++) {
                m_hexCells[IDX(COMBAT_CASTLE_SPECIAL_HEX_FIRST)].DrawOccupant(
                    static_cast<ArmyDrawState>(side5),
                    0
                );
                m_hexCells[IDX(COMBAT_CASTLE_SPECIAL_HEX_SECOND)].DrawOccupant(
                    static_cast<ArmyDrawState>(side5),
                    0
                );
            }
        }

        for (column1 = firstColumn; endColumn != column1; column1 += columnStep3)
            m_hexCells[row * COMBAT_GRID_ROW_LENGTH + column1].DrawLowerDeadOccupants();

        for (column1 = firstColumn; endColumn != column1; column1 += columnStep3)
            m_hexCells[row * COMBAT_GRID_ROW_LENGTH + column1].DrawUpperDeadOccupant();

        for (side5 = 0; side5 < ARMY_DRAW_PHASE_COUNT; side5++) {
            if (side5 == 1) {
                for (column1 = firstColumn; endColumn != column1; column1 += columnStep3) {
                    if (m_hexCells[row * COMBAT_GRID_ROW_LENGTH + column1].m_obstacleIndex != -1) {
                        m_hexCells[row * COMBAT_GRID_ROW_LENGTH + column1].DrawObstacle();
                    }
                }
            }

            for (column1 = firstColumn; endColumn != column1; column1 += columnStep3) {
                // Retail castle rendering lookup payloads.
                // NOLINTBEGIN(readability-magic-numbers)
                u16 wallCoordinates1[IDX(FACTION_COUNT)][WALL_COORDINATE_COUNT] = {
                    {443, 153, 399, 237, 399, 321, 443, 405},
                    {443, 153, 399, 237, 399, 321, 443, 405},
                    {443, 153, 399, 237, 399, 321, 443, 405},
                    {443, 153, 399, 237, 399, 321, 443, 405},
                    {443, 153, 399, 237, 399, 321, 443, 405},
                    {443, 153, 399, 237, 399, 321, 443, 405}
                };
                u8 wallFrameOffsets1[WALL_FRAME_OFFSET_COUNT] = {0, 4, 8, 23, 27, 35, 31};
                // NOLINTEND(readability-magic-numbers)
                i32 wallX7;
                i32 wallFrame1;
                i32 wallY;
                i32 hexIndex6;

                if (m_inCastleCombat != 0 && side5 == 0) {
                    hexIndex6 = row * COMBAT_GRID_ROW_LENGTH + column1;
                    wallFrame1 = 0;
                    wallX7 = 0;
                    wallY = 0;
                    switch (static_cast<CombatCastleHex>(hexIndex6)) {
                        case COMBAT_CASTLE_HEX_TOP_TOWER:
                            wallFrame1 =
                                wallFrameOffsets1[m_wallStates[COMBAT_WALL_SLOT_SECTION_FIRST]]
                                + TOP_TOWER_BASE_FRAME;
                            break;
                        case COMBAT_CASTLE_HEX_SECOND_TOWER:
                            wallFrame1 =
                                wallFrameOffsets1[m_wallStates[COMBAT_WALL_SLOT_SECTION_SECOND]]
                                + SECOND_TOWER_BASE_FRAME;
                            break;
                        case COMBAT_CASTLE_HEX_THIRD_TOWER:
                            wallFrame1 =
                                wallFrameOffsets1[m_wallStates[COMBAT_WALL_SLOT_SECTION_THIRD]]
                                + THIRD_TOWER_BASE_FRAME;
                            break;
                        case COMBAT_CASTLE_HEX_BOTTOM_TOWER:
                            wallFrame1 =
                                wallFrameOffsets1[m_wallStates[COMBAT_WALL_SLOT_SECTION_FOURTH]]
                                + BOTTOM_TOWER_BASE_FRAME;
                            break;
                        case COMBAT_CASTLE_HEX_TOP_WALL:
                            wallFrame1 = m_wallStates[COMBAT_WALL_SLOT_TOP_TOWER]
                                         + COMBAT_CASTLE_WALL_BASE_FRAME;
                            wallX7 =
                                wallCoordinates1[IDX(m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type)]
                                                [WALL_TOP_X];
                            wallY =
                                wallCoordinates1[IDX(m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type)]
                                                [WALL_TOP_Y];
                            break;
                        case COMBAT_CASTLE_HEX_SECOND_WALL:
                            wallFrame1 = m_wallStates[COMBAT_WALL_SLOT_SECOND_TOWER]
                                         + COMBAT_CASTLE_WALL_BASE_FRAME;
                            wallX7 =
                                wallCoordinates1[IDX(m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type)]
                                                [WALL_SECOND_X];
                            wallY =
                                wallCoordinates1[IDX(m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type)]
                                                [WALL_SECOND_Y];
                            break;
                        case COMBAT_CASTLE_HEX_THIRD_WALL:
                            wallFrame1 = m_wallStates[COMBAT_WALL_SLOT_THIRD_TOWER]
                                         + COMBAT_CASTLE_WALL_BASE_FRAME;
                            wallX7 =
                                wallCoordinates1[IDX(m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type)]
                                                [WALL_THIRD_X];
                            wallY =
                                wallCoordinates1[IDX(m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type)]
                                                [WALL_THIRD_Y];
                            break;
                        case COMBAT_CASTLE_HEX_BOTTOM_WALL:
                            wallFrame1 = m_wallStates[COMBAT_WALL_SLOT_BOTTOM_TOWER]
                                         + COMBAT_CASTLE_WALL_BASE_FRAME;
                            wallX7 =
                                wallCoordinates1[IDX(m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type)]
                                                [WALL_BOTTOM_X];
                            wallY =
                                wallCoordinates1[IDX(m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type)]
                                                [WALL_BOTTOM_Y];
                            break;
                        case COMBAT_CASTLE_HEX_GATE:
                            if (m_drawbridgeState != COMBAT_CASTLE_GATE_HIDDEN)
                                wallFrame1 = GATE_VISIBLE_FRAME;
                            break;
                    }
                    if (wallFrame1 != 0) {
                        m_combatIcons[IDX(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
                            wallX7,
                            wallY,
                            wallFrame1,
                            &m_hexCells[hexIndex6].m_limits[0],
                            0,
                            0,
                            NULL,
                            NULL
                        );
                    }
                }

                if (skipSpecialOccupants6 == 0
                    || (row * COMBAT_GRID_ROW_LENGTH + column1
                            != IDX(COMBAT_CASTLE_SPECIAL_HEX_FIRST)
                        && row * COMBAT_GRID_ROW_LENGTH + column1
                               != IDX(COMBAT_CASTLE_SPECIAL_HEX_SECOND))) {
                    m_hexCells[row * COMBAT_GRID_ROW_LENGTH + column1].DrawOccupant(
                        static_cast<ArmyDrawState>(side5),
                        0
                    );
                }
            }
        }

        if (m_inCastleCombat == 0
            || (m_combatTowns[COMBAT_DEFENDER_SIDE]->m_buildings & IDX(TOWN_BUILDING_MOAT)) == 0)
            goto endRow;
        if (row == COMBAT_CASTLE_GATE_ROW && m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN)
            goto endRow;

        if (moatCell[row] == giWalkingTo || moatCell[row] == giWalkingTo2
            || moatCell[row] == giWalkingFrom || moatCell[row] == giWalkingFrom2) {
            if (abs(giWalkingTo - giWalkingFrom) <= 1)
                goto drawMoat;
            if ((giWalkingFrom / COMBAT_GRID_ROW_LENGTH > giWalkingTo / COMBAT_GRID_ROW_LENGTH
                     ? giWalkingFrom / COMBAT_GRID_ROW_LENGTH
                     : giWalkingTo / COMBAT_GRID_ROW_LENGTH)
                != row)
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

                if (giWalkingFrom <= giWalkingTo)
                    goto checkWalkingTo;
                if (moatCell[row] == giWalkingFrom || moatCell[row] == giWalkingFrom2)
                    goto drawMoat;
                goto endRow;
            checkWalkingTo:
                if (moatCell[row] == giWalkingTo || moatCell[row] == giWalkingTo2)
                    goto drawMoat;
                goto endRow;
            } else {
                i32 drawbridgeY;
                i32 drawbridgeBottom;

                drawbridgeY = m_hexCells[giWalkingFrom].m_y - DRAWBRIDGE_TOP_CLIP_OFFSET
                              + giWalkingYMod;
                drawbridgeBottom =
                    m_hexCells[giWalkingFrom > giWalkingTo ? giWalkingFrom : giWalkingTo].m_y
                    + DRAWBRIDGE_BOTTOM_CLIP_OFFSET;
                IconToBitmap(
                    m_combatIcons[IDX(COMBAT_ICON_DRAWBRIDGE)],
                    gpWindowManager->m_screen,
                    0,
                    0,
                    0,
                    1,
                    0,
                    drawbridgeY,
                    COMBAT_SCREEN_WIDTH,
                    drawbridgeBottom - drawbridgeY + 1,
                    0
                );
                goto endRow;
            }
        } else {
            if (m_hexCells[moatCell[row]].m_occupantSide == COMBAT_OCCUPANT_NONE)
                goto endRow;
        drawMoat:
            m_combatIcons[IDX(COMBAT_ICON_MOAT)]
                ->CombatClipDrawToBuffer(0, 0, row, &m_moatLimits[row], 0, 0, NULL, NULL);
            m_hexCells[moatCell[row] - 1].DrawOccupant(ARMY_DRAW_ALL, 1);
            m_hexCells[moatCell[row]].DrawOccupant(ARMY_DRAW_ALL, 1);
            m_hexCells[moatCell[row] + 1].DrawOccupant(ARMY_DRAW_ALL, 1);
        }
    endRow:
        gridWasShowing1 = 0;
    }

    if (gConfig.combatArmyInfoLevel > 0 && m_smallViewSide[0] != -1) {
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
            static_cast<i32>(KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * delay);
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
            static_cast<i32>(KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * delay);
        UpdateCombatArea();
    }

finish:
    gpMouseManager->m_cursorReady = 1;
    PollSound();
}

VA(0x00405d0b, 0xb99)
void combatManager::DrawSmallView(i32 viewIndex, i32 updateScreen) {
    i32 iconX;
    i32 iconY2;
    i32 savedLimitToExtent9;
    army* viewArmy1;
    i32 viewX;
    i32 spellSlot;
    i32 drawn6;
    u8 spellPositions[COMBAT_SMALL_VIEW_MAX_SPELLS][COMBAT_SMALL_VIEW_MAX_SPELLS]
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
    i32 viewY2;
    i32 visibleSpellCount5;
    i32 unusedSpellSlot15;
    i32 spellFrame1;
    i32 unusedSpellSlot2;

    if (gbNoShowCombat != 0)
        return;
    if (m_combatWindowOpen == 0)
        return;
    if (gConfig.combatArmyInfoLevel == 0)
        return;
    if (gbInDrawSmallView != 0)
        return;

    gbInDrawSmallView = true;
    if (m_smallViewSide[viewIndex] == -1
        || (m_smallViewLastX[viewIndex] == COMBAT_SMALL_VIEW_LEFT_X
            && m_smallViewSide[viewIndex] == 1)
        || (m_smallViewLastX[viewIndex] == COMBAT_SMALL_VIEW_RIGHT_X
            && m_smallViewSide[viewIndex] == 0)) {
        if (m_smallViewLastX[viewIndex] >= 0) {
            gbLimitToExtent = true;
            giMinExtentX = m_smallViewLastX[viewIndex];
            giMinExtentY = m_smallViewLastY[viewIndex];
            giMaxExtentX = m_smallViewWidth[viewIndex] + m_smallViewLastX[viewIndex] - 1;
            giMaxExtentY = m_smallViewHeight[viewIndex] + m_smallViewLastY[viewIndex] - 1;
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
        if (m_smallViewSide[viewIndex] == -1) {
            gbInDrawSmallView = false;
            return;
        }
    }

    if (gConfig.combatArmyInfoLevel == COMBAT_SMALL_VIEW_FULL_INFO) {
        if (viewIndex == 0) {
            viewX = m_smallViewSide[viewIndex] == 0 ? COMBAT_SMALL_VIEW_LEFT_X
                                                    : COMBAT_SMALL_VIEW_RIGHT_X;
            viewY2 = COMBAT_SMALL_VIEW_FULL_Y;
        } else {
            viewX = m_smallViewSide[viewIndex] == 0 ? COMBAT_SMALL_VIEW_LEFT_X
                                                    : COMBAT_SMALL_VIEW_RIGHT_X;
            viewY2 = m_smallViewSide[1] == m_smallViewSide[0] ? COMBAT_SMALL_VIEW_FULL_RIGHT_Y
                                                              : COMBAT_SMALL_VIEW_FULL_Y;
        }
        m_smallViewWidth[viewIndex] = COMBAT_SMALL_VIEW_WIDTH;
        m_smallViewHeight[viewIndex] = COMBAT_SMALL_VIEW_FULL_HEIGHT;
    } else {
        if (viewIndex == 0) {
            viewX = m_smallViewSide[viewIndex] == 0 ? COMBAT_SMALL_VIEW_LEFT_X
                                                    : COMBAT_SMALL_VIEW_RIGHT_X;
            viewY2 = COMBAT_SMALL_VIEW_COMPACT_Y;
        } else {
            viewX = m_smallViewSide[viewIndex] == 0 ? COMBAT_SMALL_VIEW_LEFT_X
                                                    : COMBAT_SMALL_VIEW_RIGHT_X;
            viewY2 = m_smallViewSide[1] == m_smallViewSide[0] ? COMBAT_SMALL_VIEW_COMPACT_RIGHT_Y
                                                              : COMBAT_SMALL_VIEW_COMPACT_Y;
        }
        m_smallViewWidth[viewIndex] = COMBAT_SMALL_VIEW_WIDTH;
        m_smallViewHeight[viewIndex] = COMBAT_SMALL_VIEW_COMPACT_HEIGHT;
    }

    m_smallViewLastX[viewIndex] = viewX;
    m_smallViewLastY[viewIndex] = viewY2;
    savedLimitToExtent9 = gbLimitToExtent;
    if (updateScreen != 0)
        gbLimitToExtent = false;

    viewArmy1 = &m_armies[m_smallViewSide[viewIndex]][m_smallViewArmyIndex[viewIndex]];
    drawn6 = m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_BACKGROUND)]->CombatClipDrawToBuffer(
        viewX,
        viewY2,
        gConfig.combatArmyInfoLevel != COMBAT_SMALL_VIEW_FULL_INFO,
        &m_smallViewLimits,
        0,
        0,
        NULL,
        NULL
    );
    viewX += COMBAT_SMALL_VIEW_INSET_X;
    if (drawn6 != 0) {
        if (viewArmy1->m_quantity > 1)
            sprintf(
                gText,
                cMiniViewText[IDX(SMALL_VIEW_TEXT_QUANTITY_PLURAL)],
                viewArmy1->m_quantity
            );
        else
            sprintf(
                gText,
                cMiniViewText[IDX(SMALL_VIEW_TEXT_QUANTITY_SINGULAR)],
                viewArmy1->m_quantity
            );
        smallFont->DrawBoundedString(
            gText,
            viewX + COMBAT_SMALL_VIEW_TEXT_X,
            viewY2 + COMBAT_SMALL_VIEW_QUANTITY_Y,
            COMBAT_SMALL_VIEW_TEXT_WIDTH,
            COMBAT_SMALL_VIEW_TEXT_HEIGHT,
            FONT_DRAW_DEFAULT,
            FONT_ALIGN_CENTER
        );

        if (gConfig.combatArmyInfoLevel == COMBAT_SMALL_VIEW_FULL_INFO) {
            smallFont->DrawBoundedString(
                cMiniViewText[IDX(SMALL_VIEW_TEXT_ATTACK)],
                viewX + COMBAT_SMALL_VIEW_TEXT_X,
                viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_ATTACK,
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_LEFT
            );
            smallFont->DrawBoundedString(
                cMiniViewText[IDX(SMALL_VIEW_TEXT_DEFENSE)],
                viewX + COMBAT_SMALL_VIEW_TEXT_X,
                viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_DEFENSE,
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_LEFT
            );
            smallFont->DrawBoundedString(
                cMiniViewText[IDX(SMALL_VIEW_TEXT_HIT_POINTS)],
                viewX + COMBAT_SMALL_VIEW_TEXT_X,
                viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_HIT_POINTS,
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_LEFT
            );
            smallFont->DrawBoundedString(
                cMiniViewText[IDX(SMALL_VIEW_TEXT_DAMAGE)],
                viewX + COMBAT_SMALL_VIEW_TEXT_X,
                viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_DAMAGE,
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_LEFT
            );
            smallFont->DrawBoundedString(
                cMiniViewText[IDX(SMALL_VIEW_TEXT_MORALE)],
                viewX + COMBAT_SMALL_VIEW_TEXT_X,
                viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_MORALE,
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_LEFT
            );
            smallFont->DrawBoundedString(
                cMiniViewText[IDX(SMALL_VIEW_TEXT_LUCK)],
                viewX + COMBAT_SMALL_VIEW_TEXT_X,
                viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_LUCK,
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_LEFT
            );
            if (HAS(viewArmy1->m_monster.flags.all, COMBAT_ARMY_FLAG_SHOOTER))
                smallFont->DrawBoundedString(
                    cMiniViewText[IDX(SMALL_VIEW_TEXT_SHOTS)],
                    viewX + COMBAT_SMALL_VIEW_TEXT_X,
                    viewY2 + COMBAT_SMALL_VIEW_SHOTS_Y,
                    COMBAT_SMALL_VIEW_TEXT_WIDTH,
                    COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                    FONT_DRAW_DEFAULT,
                    FONT_ALIGN_LEFT
                );

            sprintf(gText, "%d", static_cast<i32>(viewArmy1->m_monster.attack));
            smallFont->DrawBoundedString(
                gText,
                viewX + COMBAT_SMALL_VIEW_TEXT_X,
                viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_ATTACK,
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_RIGHT
            );
            sprintf(gText, "%d", static_cast<i32>(viewArmy1->m_monster.defense));
            smallFont->DrawBoundedString(
                gText,
                viewX + COMBAT_SMALL_VIEW_TEXT_X,
                viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_DEFENSE,
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_RIGHT
            );
            sprintf(gText, "%d", static_cast<u32>(viewArmy1->m_monster.hitPoints));
            smallFont->DrawBoundedString(
                gText,
                viewX + COMBAT_SMALL_VIEW_TEXT_X,
                viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_HIT_POINTS,
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_RIGHT
            );
            sprintf(
                gText,
                "%d-%d",
                static_cast<i32>(viewArmy1->m_monster.damageMin),
                static_cast<i32>(viewArmy1->m_monster.damageMax)
            );
            smallFont->DrawBoundedString(
                gText,
                viewX + COMBAT_SMALL_VIEW_TEXT_X,
                viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                    + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_DAMAGE,
                COMBAT_SMALL_VIEW_TEXT_WIDTH,
                COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_RIGHT
            );

            spellSlot = 0;
            if (viewArmy1->m_morale > 0) {
                for (spellSlot = 0; spellSlot < viewArmy1->m_morale; spellSlot++)
                    m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
                        viewX + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
                            - spellSlot * COMBAT_SMALL_VIEW_MODIFIER_STEP,
                        viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                            + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_MORALE,
                        IDX(SMALL_VIEW_GOOD_MORALE_FRAME),
                        0
                    );
            } else if (viewArmy1->m_morale < 0) {
                for (spellSlot = 0; spellSlot < -viewArmy1->m_morale; spellSlot++)
                    m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
                        viewX + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
                            - spellSlot * COMBAT_SMALL_VIEW_MODIFIER_STEP,
                        viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                            + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_MORALE,
                        IDX(SMALL_VIEW_BAD_MORALE_FRAME),
                        0
                    );
            } else {
                m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
                    viewX + COMBAT_SMALL_VIEW_NEUTRAL_MORALE_X,
                    viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                        + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_MORALE,
                    IDX(SMALL_VIEW_NEUTRAL_MORALE_FRAME),
                    0
                );
            }

            spellSlot = 0;
            if (viewArmy1->m_luck > 0) {
                for (spellSlot = 0; spellSlot < viewArmy1->m_luck; spellSlot++)
                    m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
                        viewX + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
                            - spellSlot * COMBAT_SMALL_VIEW_MODIFIER_STEP,
                        viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                            + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_LUCK,
                        IDX(SMALL_VIEW_GOOD_LUCK_FRAME),
                        0
                    );
            } else if (viewArmy1->m_luck < 0) {
                for (spellSlot = 0; spellSlot < -viewArmy1->m_luck; spellSlot++)
                    m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
                        viewX + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
                            - spellSlot * COMBAT_SMALL_VIEW_MODIFIER_STEP,
                        viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                            + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_LUCK,
                        IDX(SMALL_VIEW_BAD_LUCK_FRAME),
                        0
                    );
            } else {
                m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
                    viewX + COMBAT_SMALL_VIEW_NEUTRAL_LUCK_X,
                    viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y
                        + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * STAT_ROW_LUCK,
                    IDX(SMALL_VIEW_NEUTRAL_LUCK_FRAME),
                    0
                );
            }

            if (HAS(viewArmy1->m_monster.flags.all, COMBAT_ARMY_FLAG_SHOOTER)) {
                sprintf(gText, "%d", static_cast<i32>(viewArmy1->m_monster.shots));
                smallFont->DrawBoundedString(
                    gText,
                    viewX + COMBAT_SMALL_VIEW_TEXT_X,
                    viewY2 + COMBAT_SMALL_VIEW_SHOTS_Y,
                    COMBAT_SMALL_VIEW_TEXT_WIDTH,
                    COMBAT_SMALL_VIEW_TEXT_HEIGHT,
                    FONT_DRAW_DEFAULT,
                    FONT_ALIGN_RIGHT
                );
            }
        }

        visibleSpellCount5 = viewArmy1->m_spellCount < COMBAT_SMALL_VIEW_MAX_SPELLS
                                 ? viewArmy1->m_spellCount
                                 : COMBAT_SMALL_VIEW_MAX_SPELLS;
        spellFrame1 = -1;
        for (spellSlot = 0; spellSlot < visibleSpellCount5; spellSlot++) {
            spellFrame1++;
            while (viewArmy1->m_spellInfluence[spellFrame1] == 0)
                spellFrame1++;

            iconX = viewX + COMBAT_SMALL_VIEW_INSET_X;
            if (gConfig.combatArmyInfoLevel == COMBAT_SMALL_VIEW_FULL_INFO)
                iconY2 = viewY2 + COMBAT_SMALL_VIEW_FULL_SPELL_Y;
            else
                iconY2 = viewY2 + COMBAT_SMALL_VIEW_COMPACT_SPELL_Y;
            iconX += spellPositions[visibleSpellCount5 - 1][spellSlot][0];
            iconY2 += spellPositions[visibleSpellCount5 - 1][spellSlot][1];
            iconX +=
                (COMBAT_SMALL_VIEW_ICON_SIZE
                 - GetIconEntry(m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_SPELL)], spellFrame1)->w)
                >> 1;
            iconY2 +=
                (COMBAT_SMALL_VIEW_ICON_SIZE
                 - GetIconEntry(m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_SPELL)], spellFrame1)->h)
                >> 1;
            m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_SPELL)]
                ->DrawToBuffer(iconX, iconY2, spellFrame1, 0);
        }
    }

    gbLimitToExtent = savedLimitToExtent9;
    viewX -= COMBAT_SMALL_VIEW_INSET_X;
    if (updateScreen != 0)
        gpWindowManager->UpdateScreenRegion(
            viewX,
            viewY2,
            m_smallViewWidth[viewIndex],
            m_smallViewHeight[viewIndex]
        );
    gbInDrawSmallView = false;
}

DATA(0x004ed25c) i32 bGridWasShowing = 0;
DATA(0x004ed290) b32 gbInDrawSmallView = false;
