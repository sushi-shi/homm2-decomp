// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\DRAWING.OBJ   from: (directly linked into exe)
// functions: 13   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_globals_model.h>
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
VA(0x00402910, 0x41)
void combatManager::NoShowCombatLog(char *message)
{
    char logMessage[COMBAT_MESSAGE_LOG_BUFFER_SIZE];
    sprintf(logMessage, "NC: %s", message);
    LogStr(logMessage);
}

VA(0x00402951, 0xda)
void combatManager::ClearCombatMessages(int force)
{
    m_combatMessagePending = 0;
    if (strlen(m_currentCombatMessage) <= 1 && strlen(m_previousCombatMessage) <= 1)
        return;
    if (force != 0 || m_combatMessageExpiration < KBTickCount()) {
        strcpy(m_previousCombatMessage, " ");
        strcpy(m_currentCombatMessage, " ");
        m_previousCombatMessageExpiration = 0;
        m_combatMessageExpiration = m_previousCombatMessageExpiration;
        CombatMessage("", 1, 0, 0);
    }
}

VA(0x00402a2b, 0x5d)
void combatManager::CheckUpdateCombatMessages(void)
{
    if (m_combatMessagePending != 0 && m_combatMessageExpiration != 0 &&
        m_combatMessageExpiration < KBTickCount())
        CombatMessage("", 1, 0, 1);
}

VA(0x00402a88, 0x3f8)
void combatManager::CombatMessage(char *message, int updateScreen, int retainPrevious,
                                  int clear)
{
    char *newlinePtr;
    char wrappedMessage[COMBAT_MESSAGE_WRAP_BUFFER_SIZE];
    tag_message windowMessage;
    int savedLimitToExtent;
    int savedComputeExtent;

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
        strcpy(m_previousCombatMessage, " ");
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
                strcpy(m_previousCombatMessage, " ");
            m_previousCombatMessageExpiration = m_combatMessageExpiration;
            m_combatMessageExpiration = KBTickCount() + COMBAT_MESSAGE_TIMEOUT;
        }

        newlinePtr = FindToken(message, '\n');
        if (newlinePtr != 0) {
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
    windowMessage.field4 = COMBAT_MESSAGE_TEXT_ACTION;
    windowMessage.field8 = COMBAT_MESSAGE_WIDGET_FIRST;
    windowMessage.text = m_previousCombatMessage;
    m_combatWindow->BroadcastMessage(windowMessage);
    windowMessage.field8 = COMBAT_MESSAGE_WIDGET_SECOND;
    windowMessage.text = m_currentCombatMessage;
    m_combatWindow->BroadcastMessage(windowMessage);

    savedComputeExtent = gbComputeExtent;
    savedLimitToExtent = gbLimitToExtent;
    gbLimitToExtent = 0;
    gbComputeExtent = gbLimitToExtent;
    m_combatWindow->DrawWindow(0, COMBAT_MESSAGE_DRAW_FIRST_WIDGET,
                               COMBAT_MESSAGE_WIDGET_SECOND);
    SaveCombatBorder();
    if (updateScreen != 0)
        gpWindowManager->UpdateScreenRegion(
            COMBAT_MESSAGE_WINDOW_X, COMBAT_MESSAGE_WINDOW_Y,
            COMBAT_MESSAGE_LINE_WIDTH, COMBAT_MESSAGE_WINDOW_HEIGHT);
    gbComputeExtent = savedComputeExtent;
    gbLimitToExtent = savedLimitToExtent;
}

// @early-stop
// reloc-masked: all 0x3fe bytes match after masking 58 aligned COFF relocations,
// including the jump table. String slots and local case labels delink under different names.
VA(0x00402e80, 0x3fe)
void combatManager::CombatMessage(int messageType)
{
    army *currentArmyPtr;
    army *targetArmy;
    int actingMonsterType;
    int targetMonsterType;

    if (m_combatWindowOpen == 0)
        return;
    if (gbNoShowCombat != 0)
        return;

    currentArmyPtr = &m_armies[m_currentArmySide][m_currentArmyIndex];
    actingMonsterType = currentArmyPtr->m_monsterType;
    targetArmy = 0;
    targetMonsterType = 0;
    if (currentArmyPtr->m_targetSide >= 0 && currentArmyPtr->m_targetIndex >= 0) {
        targetArmy = &m_armies[currentArmyPtr->m_targetSide][currentArmyPtr->m_targetIndex];
        targetMonsterType = targetArmy->m_monsterType;
    }

    switch (messageType) {
        case COMBAT_MESSAGE_COMMAND_DEFAULT:
            if ((currentArmyPtr->m_monster.flags.all & COMBAT_ARMY_FLAG_SHOOTER) != 0 &&
                currentArmyPtr->m_monster.shots == 0 && targetArmy != 0)
                strcpy(gText, cCombatMessage[COMBAT_MESSAGE_TEXT_NO_SHOTS]);
            else
                strcpy(gText, cCombatMessage[COMBAT_MESSAGE_TEXT_DEFAULT]);
            break;
        case COMBAT_MESSAGE_COMMAND_MOVE:
            sprintf(gText, cCombatMessage[COMBAT_MESSAGE_TEXT_MOVE],
                    gArmyNames[actingMonsterType]);
            break;
        case COMBAT_MESSAGE_COMMAND_FLY:
            sprintf(gText, cCombatMessage[COMBAT_MESSAGE_TEXT_FLY],
                    gArmyNames[actingMonsterType]);
            break;
        case COMBAT_MESSAGE_COMMAND_ATTACK:
            sprintf(gText, cCombatMessage[COMBAT_MESSAGE_TEXT_ATTACK],
                    gArmyNames[targetMonsterType]);
            break;
        case COMBAT_MESSAGE_COMMAND_SHOOT:
            sprintf(gText, cCombatMessage[COMBAT_MESSAGE_TEXT_SHOOT],
                    gArmyNames[targetMonsterType],
                    static_cast<int>(currentArmyPtr->m_monster.shots));
            break;
        case COMBAT_MESSAGE_COMMAND_OPTIONS:
            if (m_heroes[m_currentSide] != 0 && m_heroes[m_currentSide]->m_unknownE7 != 0)
                strcpy(gText, cCombatMessage[COMBAT_MESSAGE_TEXT_CAPTAIN_OPTIONS]);
            else
                strcpy(gText, cCombatMessage[COMBAT_MESSAGE_TEXT_HERO_OPTIONS]);
            break;
        case COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS:
            if (m_heroes[1 - m_currentSide] != 0 &&
                m_heroes[1 - m_currentSide]->m_unknownE7 != 0)
                strcpy(gText, cCombatMessage[COMBAT_MESSAGE_TEXT_OPPOSING_CAPTAIN]);
            else
                strcpy(gText, cCombatMessage[COMBAT_MESSAGE_TEXT_OPPOSING_HERO]);
            break;
        case COMBAT_MESSAGE_COMMAND_VIEW_INFO:
            if (m_selectedHex == COMBAT_BALLISTA_HEX) {
                sprintf(gText, cCombatMessage[COMBAT_MESSAGE_TEXT_BALLISTA]);
            } else {
                actingMonsterType =
                    m_armies[m_currentArmySide][m_hexCells[m_selectedHex].m_occupantIndex]
                        .m_monsterType;
                if (actingMonsterType >= 0)
                    sprintf(gText, cCombatMessage[COMBAT_MESSAGE_TEXT_VIEW_INFO],
                            gArmyNames[actingMonsterType]);
                else
                    sprintf(gText, "");
            }
            break;
    }
    CombatMessage(gText, 1, 0, 0);
}

VA(0x0040327e, 0x126)
void combatManager::ResetLimitCreature(void)
{
    int side;
    int armySlotIndex;

    for (side = 0; side < COMBAT_SIDE_COUNT_DRAWING; side++) {
        for (armySlotIndex = 0; armySlotIndex < COMBAT_ARMY_SLOT_COUNT_DRAWING;
             armySlotIndex++) {
            if ((m_armies[side][armySlotIndex].m_monster.flags.all &
                 COMBAT_ARMY_FLAG_MIRROR_IMAGE) != 0)
                m_limitCreatureCount[side][armySlotIndex] = -1;
            else
                m_limitCreatureCount[side][armySlotIndex] = 0;
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
void combatManager::UpdateCombatArea(void)
{
    if (gbNoShowCombat != 0)
        return;
    if (m_combatWindowOpen == 0)
        return;

    gbEnlargeScreenBlit = 0;
    gpWindowManager->UpdateScreenRegion(0, 0, COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT);
    gbEnlargeScreenBlit = 1;
}

VA(0x0040340e, 0x213)
void combatManager::SetupGridForArmy(army *armyPtr)
{
    int attackMask;
    int savedTargetSide;
    int targetIndexSave;
    int hexIndex;

    if (gbNoShowCombat != 0)
        return;
    if (giCombatShadeLevel < 1)
        return;

    attackMask = armyPtr->GetAttackMask(armyPtr->m_hex, 2, -1);
    memset(m_gridState, COMBAT_GRID_SHADE_NONE, sizeof(m_gridState));
    savedTargetSide = armyPtr->m_targetSide;
    targetIndexSave = armyPtr->m_targetIndex;
    armyPtr->m_targetSide = -1;
    armyPtr->m_targetIndex = -1;
    gpSearchArray->SeedCombatPosition(armyPtr);
    armyPtr->m_targetSide = savedTargetSide;
    armyPtr->m_targetIndex = targetIndexSave;

    for (hexIndex = 0; hexIndex < COMBAT_HEX_COUNT; hexIndex++) {
        if (armyPtr->m_hex == hexIndex) {
            m_gridState[hexIndex] = COMBAT_GRID_SHADE_REACHABLE;
        } else if (m_hexCells[hexIndex].m_pathReachable != 0) {
            if (m_hexCells[hexIndex].m_occupantSide != -1) {
                if (m_hexCells[hexIndex].m_occupantSide != armyPtr->m_side)
                    m_gridState[hexIndex] = COMBAT_GRID_SHADE_REACHABLE;
            } else {
                m_gridState[hexIndex] = COMBAT_GRID_SHADE_EMPTY_BLOCKED;
            }
        } else if (m_hexCells[hexIndex].m_occupantSide != -1 &&
                   m_hexCells[hexIndex].m_occupantSide != 1 - m_currentSide &&
                   (attackMask & (1 << m_hexCells[hexIndex].m_occupantIndex)) != 0) {
            m_gridState[hexIndex] = COMBAT_GRID_SHADE_REACHABLE;
        }
    }
}

// @early-stop
// Reloc-masked instructions are byte-exact; only delinker symbol identities
// differ for data VAs 0x00528d88 and 0x00528d90.
VA(0x00403621, 0x5fb)
int combatManager::UpdateGrid(int resetGridDisplay, int rebuildGrid)
{
    int retval;
    int minX;
    int minY;
    int maxX;
    int maxY;
    int cellIndex;
    int gridChanged;
    int drawShading;
    int hadOldShade;

    if (gbNoShowCombat != 0)
        return 0;

    if (rebuildGrid != 0) {
        if (m_playerId[m_currentSide] == -1 ||
            gbThisNetHumanPlayer[m_playerId[m_currentSide]] == 0 ||
            m_gridSelectionDisabled != 0) {
            memset(m_gridState, COMBAT_GRID_SHADE_NONE, sizeof(m_gridState));
        } else {
            SetupGridForArmy(&m_armies[m_currentArmySide][m_currentArmyIndex]);
        }
    }
    if (resetGridDisplay != 0)
        bGridWasShowing = 0;
    if (giCombatShadeLevel < 1 && gbShowCombatGrid == 0)
        return 0;

    retval = 0;
    minX = COMBAT_MAX_EXTENT_X;
    minY = COMBAT_MAX_EXTENT_Y;
    maxX = 0;
    maxY = 0;
    drawShading = 0;
    hadOldShade = 0;
    gridChanged = 0;

    if (giCombatShadeLevel < 1)
        goto DrawCombatGrid;

    for (cellIndex = 0; cellIndex < COMBAT_HEX_COUNT; cellIndex++) {
        if (m_previousGridState[cellIndex] != m_gridState[cellIndex])
            gridChanged = 1;
        if (m_gridState[cellIndex] != COMBAT_GRID_SHADE_NONE)
            drawShading = 1;
        if (m_previousGridState[cellIndex] != COMBAT_GRID_SHADE_NONE)
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
                if (m_previousGridState[cellIndex] != m_gridState[cellIndex] ||
                    m_gridState[cellIndex] != COMBAT_GRID_SHADE_NONE) {
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
                m_backgroundBuffer, minX, minY,
                minX - COMBAT_GRID_COPY_LEFT, minY - COMBAT_GRID_COPY_TOP,
                maxX - minX + 1, maxY - minY + 1);
            retval = 1;
        }
    }

    if (drawShading == 0)
        goto DrawCombatGrid;
    for (cellIndex = 0; cellIndex < COMBAT_HEX_COUNT; cellIndex++) {
        if (m_gridState[cellIndex] != COMBAT_GRID_SHADE_NONE) {
            DimIconToBitmap(
                m_gridIcon, m_backgroundBuffer, m_hexCells[cellIndex].m_x,
                m_hexCells[cellIndex].m_y, 1,
                m_gridState[cellIndex] - 1, 1, 0, 0,
                COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT);
            retval = 1;
        }
    }

DrawCombatGrid:
    if (gbShowCombatGrid != 0) {
        if (bGridWasShowing != 0 && retval == 0)
            goto CopyGridState;
        for (cellIndex = 0; cellIndex < COMBAT_HEX_COUNT; cellIndex++) {
            if (cellIndex % COMBAT_GRID_ROW_LENGTH != 0 &&
                cellIndex % COMBAT_GRID_ROW_LENGTH != COMBAT_GRID_ROW_LENGTH - 1) {
                MonoIconToBitmap(
                    m_gridIcon, m_backgroundBuffer, m_hexCells[cellIndex].m_x,
                    m_hexCells[cellIndex].m_y, COMBAT_GRID_LINE_FRAME,
                    COMBAT_GRID_LINE_COLOR, 1, 0, 0,
                    COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT);
            }
        }
        retval = 1;
        bGridWasShowing = 1;
    }

CopyGridState:
    memcpy(m_previousGridState, m_gridState, sizeof(m_previousGridState));
    return retval;
}

// @early-stop
// Reloc-masked instructions are byte-exact; only the three string-literal
// relocation symbol identities differ.
VA(0x00403c1c, 0x364)
void combatManager::DrawBackground(void)
{
    icon *backgroundIcon;

    if (gbNoShowCombat != 0)
        return;
    if (m_backgroundDrawn != 0)
        return;

    backgroundIcon = gpResourceManager->GetIcon(m_battlefieldBackgroundName);
    IconToBitmap(backgroundIcon, m_backgroundBuffer, 0, 0, 0, 1, 0, 0,
                 COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT, 0);
    gpResourceManager->Dispose(backgroundIcon);

    if (m_debugFormation != 0) {
        sprintf(gText, "covr%04d.icn", m_debugFormation);
        backgroundIcon = gpResourceManager->GetIcon(gText);
        IconToBitmap(backgroundIcon, m_backgroundBuffer, 0, 0, 0, 0, 0, 0,
                     COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT, 0);
        gpResourceManager->Dispose(backgroundIcon);
    }
    if (m_battlefieldFringe != -1) {
        sprintf(gText, "frng%04d.icn", m_battlefieldFringe);
        backgroundIcon = gpResourceManager->GetIcon(gText);
        if (m_inCastleCombat != 0)
            IconToBitmap(backgroundIcon, m_backgroundBuffer, 0, 0, 0, 1, 0, 0,
                         COMBAT_SCREEN_WIDTH / 2, COMBAT_AREA_HEIGHT, 0);
        else
            IconToBitmap(backgroundIcon, m_backgroundBuffer, 0, 0, 0, 0, 0, 0,
                         COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT, 0);
        gpResourceManager->Dispose(backgroundIcon);
    }
    if (m_inCastleCombat != 0) {
        sprintf(gText, "castbkg%c.icn", cHeroTypeInitial[m_castle->m_type]);
        backgroundIcon = gpResourceManager->GetIcon(gText);
        IconToBitmap(backgroundIcon, m_backgroundBuffer, 0, 0,
                     COMBAT_CASTLE_BACKGROUND_BASE_FRAME, 0, 0, 0,
                     COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT, 0);
        if (m_drawbridgeBackgroundVisible != 0)
            IconToBitmap(m_drawbridgeIcon, m_backgroundBuffer, 0, 0, 0, 0, 0, 0,
                         COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT, 0);
        if (m_castle->m_type == TOWN_TYPE_KNIGHT &&
            (m_castle->m_buildings & TOWN_BUILDING_RAINBOW) != 0)
            IconToBitmap(backgroundIcon, m_backgroundBuffer, 0, 0,
                         COMBAT_CASTLE_BACKGROUND_BUILDING_FRAME, 0, 0, 0,
                         COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT, 0);
        else
            IconToBitmap(backgroundIcon, m_backgroundBuffer, 0, 0,
                         COMBAT_CASTLE_BACKGROUND_DEFAULT_FRAME, 0, 0, 0,
                         COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT, 0);
        gpResourceManager->Dispose(backgroundIcon);
    }

    m_backgroundBuffer->CopyToCareful(
        m_combatBuffer, COMBAT_GRID_COPY_LEFT, COMBAT_GRID_COPY_TOP, 0, 0,
        COMBAT_BACKGROUND_COPY_WIDTH, COMBAT_BACKGROUND_COPY_HEIGHT);
    UpdateGrid(1, 0);
    m_backgroundBuffer->CopyToCareful(
        gpWindowManager->m_screen, 0, 0, 0, 0,
        COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT);
    m_backgroundDrawn = 1;
}

// @early-stop
// Reloc-masked instructions are byte-exact; only the delinker symbol identity
// differs for data VA 0x00528d8c.
VA(0x00403f80, 0x64c)
void combatManager::UpdateMouseGrid(int hexIndex, int forceUpdate)
{
    int oldLimit;
    int copyHeight;
    int maxXSave;
    int oldMinX;
    int oldMaxYBackup;
    int savedComputeExtents;
    int savedExtentMinY;

    if (m_nonVisualCombat != 0)
        return;
    if (gbNoShowCombat != 0)
        return;
    if (gbShowCombatMouseHex == 0)
        return;
    if (gbProcessingCombatAction != 0 && forceUpdate == 0)
        return;

    if (hexIndex < 0 || hexIndex >= COMBAT_HEX_COUNT ||
        hexIndex % COMBAT_GRID_ROW_LENGTH == 0 ||
        hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1)
        hexIndex = -1;
    if (m_mouseGridHex == hexIndex)
        return;

    if (m_mouseGridBuffer == 0)
        m_mouseGridBuffer = new bitmap(0, COMBAT_MOUSE_HEX_WIDTH, COMBAT_MOUSE_HEX_HEIGHT);

    if (m_mouseGridHex != -1) {
        if (m_hexCells[m_mouseGridHex].m_y + COMBAT_MOUSE_HEX_MAX_Y_OFFSET >
            COMBAT_MAX_EXTENT_Y)
            copyHeight = COMBAT_MAX_EXTENT_Y - m_hexCells[m_mouseGridHex].m_y + 1;
        else
            copyHeight = COMBAT_MOUSE_HEX_HEIGHT;
        m_mouseGridBuffer->CopyToCareful(
            m_backgroundBuffer, m_hexCells[m_mouseGridHex].m_x,
            m_hexCells[m_mouseGridHex].m_y, 0, 0,
            COMBAT_MOUSE_HEX_WIDTH, copyHeight);
    }
    if (hexIndex != -1) {
        if (m_hexCells[hexIndex].m_y + COMBAT_MOUSE_HEX_MAX_Y_OFFSET >
            COMBAT_MAX_EXTENT_Y)
            copyHeight = COMBAT_MAX_EXTENT_Y - m_hexCells[hexIndex].m_y + 1;
        else
            copyHeight = COMBAT_MOUSE_HEX_HEIGHT;
        m_backgroundBuffer->CopyToCareful(
            m_mouseGridBuffer, 0, 0, m_hexCells[hexIndex].m_x,
            m_hexCells[hexIndex].m_y, COMBAT_MOUSE_HEX_WIDTH, copyHeight);
        DimIconToBitmap(m_gridIcon, m_backgroundBuffer, m_hexCells[hexIndex].m_x,
                        m_hexCells[hexIndex].m_y, 1, COMBAT_GRID_MOUSE_FRAME,
                        1, 0, 0, COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT);
    }

    oldMinX = giMinExtentX;
    savedExtentMinY = giMinExtentY;
    maxXSave = giMaxExtentX;
    oldMaxYBackup = giMaxExtentY;
    oldLimit = gbLimitToExtent;
    savedComputeExtents = gbComputeExtent;
    if (m_mouseGridHex != -1) {
        giMinExtentX = m_hexCells[m_mouseGridHex].m_x;
        giMinExtentY = m_hexCells[m_mouseGridHex].m_y;
        giMaxExtentX = m_hexCells[m_mouseGridHex].m_x + COMBAT_MOUSE_HEX_MAX_X_OFFSET;
        giMaxExtentY = m_hexCells[m_mouseGridHex].m_y + COMBAT_MOUSE_HEX_MAX_Y_OFFSET;
    } else {
        giMinExtentX = COMBAT_SCREEN_WIDTH;
        giMinExtentY = 480;
        giMaxExtentX = 0;
        giMaxExtentY = 0;
    }
    if (hexIndex != -1) {
        if (m_hexCells[hexIndex].m_x < giMinExtentX)
            giMinExtentX = m_hexCells[hexIndex].m_x;
        if (m_hexCells[hexIndex].m_y < giMinExtentY)
            giMinExtentY = m_hexCells[hexIndex].m_y;
        if (m_hexCells[hexIndex].m_x + COMBAT_MOUSE_HEX_MAX_X_OFFSET > giMaxExtentX)
            giMaxExtentX = m_hexCells[hexIndex].m_x + COMBAT_MOUSE_HEX_MAX_X_OFFSET;
        if (m_hexCells[hexIndex].m_y + COMBAT_MOUSE_HEX_MAX_Y_OFFSET > giMaxExtentY)
            giMaxExtentY = m_hexCells[hexIndex].m_y + COMBAT_MOUSE_HEX_MAX_Y_OFFSET;
    }
    if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
        giMaxExtentY = COMBAT_MAX_EXTENT_Y;

    gbLimitToExtent = 1;
    gbComputeExtent = 1;
    m_backgroundBuffer->CopyTo(
        gpWindowManager->m_screen, giMinExtentX, giMinExtentY,
        giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1);
    DrawFrame(0, 0, 0, 0, COMBAT_MOUSE_REDRAW_DELAY, 1, 1);
    gpWindowManager->UpdateScreenRegion(
        giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1);
    giMinExtentX = oldMinX;
    giMinExtentY = savedExtentMinY;
    giMaxExtentX = maxXSave;
    giMaxExtentY = oldMaxYBackup;
    gbLimitToExtent = oldLimit;
    gbComputeExtent = savedComputeExtents;
    m_mouseGridHex = hexIndex;
}

VA(0x004045cc, 0x173f)
void combatManager::DrawFrame(int updateScreen, int computeExtent, int redrawExtent,
                              int extentOnly, int delay, int drawBackground,
                              int waitForTimer)
{
    int side;
    int armyIndex;
    int extentChanged;
    int heroX;
    int heroY;
    int row;
    int column;
    int firstColumn;
    int endColumn;
    int columnStep;
    int phase;
    int skipSpecialOccupants;
    int hexIndex;
    int wallFrame;
    int wallX;
    int wallY;
    int drawbridgeY;
    int drawbridgeBottom;
    int gridWasShowing;

    if (m_nonVisualCombat != 0)
        return;
    if (gbNoShowCombat != 0)
        return;
    if (m_combatWindowOpen == 0)
        return;

    PollSound();
    gpMouseManager->m_cursorReady = 0;

    if (computeExtent != 0) {
        extentChanged = 0;
        for (side = 0; side < COMBAT_SIDE_COUNT_DRAWING; side++) {
            for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT_DRAWING; armyIndex++) {
                if (m_limitCreatureCount[side][armyIndex] > 0) {
                    extentChanged = 1;
                    gbComputeExtent = 1;
                    gbSaveBiggestExtent = 1;
                    gbReturnAfterComputeExtent = 1;
                    armyIndex[m_armies[side]].DrawToBuffer(
                        m_hexCells[armyIndex[m_armies[side]].m_hex].m_x,
                        m_hexCells[armyIndex[m_armies[side]].m_hex].m_y, 0);
                    gbReturnAfterComputeExtent = 0;
                    gbComputeExtent = 0;
                    gbSaveBiggestExtent = 0;
                }
            }
        }

        for (side = 0; side < COMBAT_SIDE_COUNT_DRAWING; side++) {
            if (m_drawHero[side] != 0) {
                extentChanged = 1;
                gbComputeExtent = 1;
                gbSaveBiggestExtent = 1;
                gbReturnAfterComputeExtent = 1;
                m_heroIcons[side]->CombatClipDrawToBuffer(
                    side == 0 ? COMBAT_HERO_LEFT_X : COMBAT_HERO_RIGHT_X,
                    side == 0 ? COMBAT_HERO_LEFT_Y : COMBAT_HERO_RIGHT_Y,
                    sCmbtHero[m_heroSpriteIndex[side]]
                        .animationFrames[m_heroAnimationState[side]][m_heroAnimationFrame[side]],
                    &m_heroLimits[side], 1, 0, 0, 0);
                gbReturnAfterComputeExtent = 0;
                gbComputeExtent = 0;
                gbSaveBiggestExtent = 0;
            }

            if (m_drawHeroOverlay[side] != 0) {
                extentChanged = 1;
                gbComputeExtent = 1;
                gbSaveBiggestExtent = 1;
                gbReturnAfterComputeExtent = 1;
                if (side == 0) {
                    heroY = COMBAT_HERO_LEFT_Y;
                    heroX = COMBAT_HERO_LEFT_X;
                } else {
                    heroY = ((m_heroes[side]->m_unknownE7 < 1) - 1 &
                             (COMBAT_HERO_RIGHT_ALT_Y - COMBAT_HERO_RIGHT_Y)) +
                            COMBAT_HERO_RIGHT_Y;
                    heroX = ((m_heroes[side]->m_unknownE7 < 1) - 1 &
                             (COMBAT_HERO_RIGHT_ALT_X - COMBAT_HERO_RIGHT_X)) +
                            COMBAT_HERO_RIGHT_X;
                }
                m_heroOverlayIcons[side]->CombatClipDrawToBuffer(
                    heroX, heroY, m_heroOverlayFrame[side], &m_heroOverlayLimits[side],
                    1, 0, 0, 0);
                gbReturnAfterComputeExtent = 0;
                gbComputeExtent = 0;
                gbSaveBiggestExtent = 0;
            }
        }

        if (extentOnly != 0 || extentChanged == 0)
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
                    gpWindowManager->m_screen, giMinExtentX, giMinExtentY,
                    giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
                    giMaxExtentY - giMinExtentY + 1);
            } else {
                m_backgroundBuffer->CopyTo(gpWindowManager->m_screen, 0, 0, 0, 0,
                                           COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT);
            }
        } else {
            DrawBackground();
        }
    }

    if (computeExtent != 0) {
        gbLimitToExtent = 1;
        gbComputeExtent = 1;
    }

    for (row = 0; row < COMBAT_DRAW_LAYER_COUNT; row++) {
        if (row == COMBAT_DRAW_RIGHT_HERO_LAYER && m_heroes[1] != 0) {
            m_heroIcons[1]->CombatClipDrawToBuffer(
                ((m_heroes[1]->m_unknownE7 < 1) - 1 &
                 (COMBAT_HERO_RIGHT_ALT_X - COMBAT_HERO_RIGHT_X)) +
                    COMBAT_HERO_RIGHT_X,
                ((m_heroes[1]->m_unknownE7 < 1) - 1 &
                 (COMBAT_HERO_RIGHT_ALT_Y - COMBAT_HERO_RIGHT_Y)) +
                    COMBAT_HERO_RIGHT_Y,
                sCmbtHero[m_heroSpriteIndex[1]]
                    .animationFrames[m_heroAnimationState[1]][m_heroAnimationFrame[1]],
                &m_heroLimits[1], 1, 0, 0, 0);
            m_heroOverlayIcons[1]->CombatClipDrawToBuffer(
                COMBAT_HERO_RIGHT_X, COMBAT_HERO_RIGHT_Y,
                m_heroOverlayFrame[1], &m_heroOverlayLimits[1], 1, 0, 0, 0);
        }

        if (row == COMBAT_DRAW_LEFT_HERO_LAYER && m_heroes[0] != 0) {
            m_heroIcons[0]->CombatClipDrawToBuffer(
                COMBAT_HERO_LEFT_X, COMBAT_HERO_LEFT_Y,
                sCmbtHero[m_heroSpriteIndex[0]]
                    .animationFrames[m_heroAnimationState[0]][m_heroAnimationFrame[0]],
                &m_heroLimits[0], 0, 0, 0, 0);
            m_heroOverlayIcons[0]->CombatClipDrawToBuffer(
                COMBAT_HERO_LEFT_X, COMBAT_HERO_LEFT_Y,
                m_heroOverlayFrame[0], &m_heroOverlayLimits[0], 0, 0, 0, 0);
        }

        firstColumn = COMBAT_GRID_FIRST_COLUMN;
        endColumn = COMBAT_GRID_COLUMN_END;
        columnStep = 1;
        if (m_inCastleCombat != 0 && row >= COMBAT_CASTLE_REVERSE_ROW) {
            firstColumn = COMBAT_GRID_REVERSE_FIRST_COLUMN;
            endColumn = COMBAT_GRID_REVERSE_COLUMN_END;
            columnStep = -1;
        }

        if (m_inCastleCombat != 0 && row == COMBAT_DRAW_CATAPULT_LAYER) {
            m_catapultIcon->CombatClipDrawToBuffer(
                COMBAT_CATAPULT_X, COMBAT_CATAPULT_Y, m_catapultFrame,
                &m_catapultLimits, 0, 0, 0, 0);
        }
        if (m_inCastleCombat != 0 && row == COMBAT_DRAW_WALL_TOP_LAYER &&
            m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN) {
            m_towerIcon->CombatClipDrawToBuffer(
                0, 0, m_drawbridgeState + 0x15, &m_upperWallLimits,
                0, 0, 0, 0);
        }
        if (m_inCastleCombat != 0 && row == COMBAT_DRAW_WALL_MIDDLE_LAYER &&
            m_drawbridgeState == 0) {
            m_towerIcon->CombatClipDrawToBuffer(
                0, 0, COMBAT_CASTLE_TOP_LAYER_FRAME, &m_middleWallLimits,
                0, 0, 0, 0);
        }
        if (m_inCastleCombat != 0 && row == COMBAT_DRAW_GATE_LAYER) {
            m_towerIcon->CombatClipDrawToBuffer(
                0, 0,
                m_wallStates[8] == 0 ? COMBAT_CASTLE_GATE_CLOSED_FRAME
                                      : COMBAT_CASTLE_GATE_FRAME,
                &m_gateLimits, 0, 0, 0, 0);
        }

        skipSpecialOccupants = 0;
        if (m_inCastleCombat != 0 && m_wallStates[7] != 2 && m_wallStates[7] != 6) {
            skipSpecialOccupants = 1;
            for (side = 0; side < COMBAT_DRAW_PHASE_COUNT; side++) {
                m_hexCells[COMBAT_CASTLE_SPECIAL_HEX_FIRST].DrawOccupant(side, 0);
                m_hexCells[COMBAT_CASTLE_SPECIAL_HEX_SECOND].DrawOccupant(side, 0);
            }
        }

        for (column = firstColumn; column != endColumn; column += columnStep)
            m_hexCells[row * COMBAT_GRID_ROW_LENGTH + column].DrawLowerDeadOccupants();

        for (column = firstColumn; column != endColumn; column += columnStep)
            m_hexCells[row * COMBAT_GRID_ROW_LENGTH + column].DrawUpperDeadOccupant();

        for (phase = 0; phase < COMBAT_DRAW_PHASE_COUNT; phase++) {
            if (phase == 1) {
                for (column = firstColumn; column != endColumn; column += columnStep) {
                    if (m_hexCells[row * COMBAT_GRID_ROW_LENGTH + column]
                            .m_obstacleIndex != -1) {
                        m_hexCells[row * COMBAT_GRID_ROW_LENGTH + column]
                            .DrawObstacle();
                    }
                }
            }

            for (column = firstColumn; column != endColumn; column += columnStep) {
                unsigned short wallCoordinates[6][8] = {
                    { 443, 153, 399, 237, 399, 321, 443, 405 },
                    { 443, 153, 399, 237, 399, 321, 443, 405 },
                    { 443, 153, 399, 237, 399, 321, 443, 405 },
                    { 443, 153, 399, 237, 399, 321, 443, 405 },
                    { 443, 153, 399, 237, 399, 321, 443, 405 },
                    { 443, 153, 399, 237, 399, 321, 443, 405 }
                };
                unsigned char wallFrameOffsets[7] = { 0, 4, 8, 23, 27, 35, 31 };

                if (m_inCastleCombat != 0 && phase == 0) {
                    hexIndex = row * COMBAT_GRID_ROW_LENGTH + column;
                    wallFrame = 0;
                    wallX = 0;
                    wallY = 0;
                    switch (hexIndex) {
                    case COMBAT_CASTLE_HEX_TOP_TOWER:
                        wallFrame = wallFrameOffsets[m_wallStates[4]] + 5;
                        break;
                    case COMBAT_CASTLE_HEX_SECOND_TOWER:
                        wallFrame = wallFrameOffsets[m_wallStates[5]] + 6;
                        break;
                    case COMBAT_CASTLE_HEX_THIRD_TOWER:
                        wallFrame = wallFrameOffsets[m_wallStates[6]] + 7;
                        break;
                    case COMBAT_CASTLE_HEX_BOTTOM_TOWER:
                        wallFrame = wallFrameOffsets[m_wallStates[7]] + 8;
                        break;
                    case COMBAT_CASTLE_HEX_TOP_WALL:
                        wallFrame = m_wallStates[0] + 0x11;
                        wallX = wallCoordinates[m_castle->m_type][0];
                        wallY = wallCoordinates[m_castle->m_type][1];
                        break;
                    case COMBAT_CASTLE_HEX_SECOND_WALL:
                        wallFrame = m_wallStates[1] + 0x11;
                        wallX = wallCoordinates[m_castle->m_type][2];
                        wallY = wallCoordinates[m_castle->m_type][3];
                        break;
                    case COMBAT_CASTLE_HEX_THIRD_WALL:
                        wallFrame = m_wallStates[2] + 0x11;
                        wallX = wallCoordinates[m_castle->m_type][4];
                        wallY = wallCoordinates[m_castle->m_type][5];
                        break;
                    case COMBAT_CASTLE_HEX_BOTTOM_WALL:
                        wallFrame = m_wallStates[3] + 0x11;
                        wallX = wallCoordinates[m_castle->m_type][6];
                        wallY = wallCoordinates[m_castle->m_type][7];
                        break;
                    case COMBAT_CASTLE_HEX_GATE:
                        if (m_drawbridgeState != COMBAT_CASTLE_GATE_HIDDEN)
                            wallFrame = 4;
                        break;
                    }
                    if (wallFrame != 0) {
                        m_towerIcon->CombatClipDrawToBuffer(
                            wallX, wallY, wallFrame, &m_hexCells[hexIndex].m_limits[0],
                            0, 0, 0, 0);
                    }
                }

                if (skipSpecialOccupants == 0 ||
                    (row * COMBAT_GRID_ROW_LENGTH + column !=
                         COMBAT_CASTLE_SPECIAL_HEX_FIRST &&
                     row * COMBAT_GRID_ROW_LENGTH + column !=
                         COMBAT_CASTLE_SPECIAL_HEX_SECOND)) {
                    m_hexCells[row * COMBAT_GRID_ROW_LENGTH + column]
                        .DrawOccupant(phase, 0);
                }
            }
        }

        if (m_inCastleCombat != 0 &&
            (m_castle->m_buildings & TOWN_BUILDING_MOAT) != 0 &&
            (row != COMBAT_CASTLE_GATE_ROW ||
             m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN)) {
            if (moatCell[row] == giWalkingTo || moatCell[row] == giWalkingTo2 ||
                moatCell[row] == giWalkingFrom || moatCell[row] == giWalkingFrom2) {
                if (abs(giWalkingTo - giWalkingFrom) <= 1)
                    goto drawMoat;
                if ((giWalkingTo / COMBAT_GRID_ROW_LENGTH <
                     giWalkingFrom / COMBAT_GRID_ROW_LENGTH
                         ? giWalkingFrom / COMBAT_GRID_ROW_LENGTH
                         : giWalkingTo / COMBAT_GRID_ROW_LENGTH) == row) {
                    if (gpCombatManager->m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN &&
                        (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW ||
                         giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW)) {
                        if ((giWalkingTo / COMBAT_GRID_ROW_LENGTH !=
                                 COMBAT_CASTLE_GATE_ROW ||
                             giWalkingFrom / COMBAT_GRID_ROW_LENGTH !=
                                 COMBAT_CASTLE_GATE_ROW - 1) &&
                            (giWalkingTo / COMBAT_GRID_ROW_LENGTH !=
                                 COMBAT_CASTLE_GATE_ROW - 1 ||
                             giWalkingFrom / COMBAT_GRID_ROW_LENGTH !=
                                 COMBAT_CASTLE_GATE_ROW)) {
                            if (giWalkingTo < giWalkingFrom) {
                                if (moatCell[row] == giWalkingFrom ||
                                    moatCell[row] == giWalkingFrom2)
                                    goto drawMoat;
                            } else if (moatCell[row] == giWalkingTo ||
                                       moatCell[row] == giWalkingTo2) {
                                goto drawMoat;
                            }
                        }
                    } else {
                        drawbridgeY = m_hexCells[giWalkingFrom].m_y - 9 + giWalkingYMod;
                        drawbridgeBottom =
                            m_hexCells[giWalkingFrom <= giWalkingTo
                                           ? giWalkingTo
                                           : giWalkingFrom]
                                .m_y + 5;
                        IconToBitmap(m_drawbridgeIcon, gpWindowManager->m_screen,
                                     0, 0, 0, 1, 0, drawbridgeY,
                                     COMBAT_SCREEN_WIDTH,
                                     drawbridgeBottom - drawbridgeY + 1, 0);
                    }
                }
            } else if (m_hexCells[moatCell[row]].m_occupantSide != -1) {
drawMoat:
                m_moatIcon->CombatClipDrawToBuffer(
                    0, 0, row, &m_moatLimits[row], 0, 0, 0, 0);
                m_hexCells[moatCell[row] - 1].DrawOccupant(
                    COMBAT_DRAW_ALL_OCCUPANTS, 1);
                m_hexCells[moatCell[row]].DrawOccupant(
                    COMBAT_DRAW_ALL_OCCUPANTS, 1);
                m_hexCells[moatCell[row] + 1].DrawOccupant(
                    COMBAT_DRAW_ALL_OCCUPANTS, 1);
            }
        }
        gridWasShowing = 0;
    }

    if (gConfig.combatArmyInfoLevel > 0 && m_smallViewSide[0] != -1) {
        DrawSmallView(0, 0);
        DrawSmallView(1, 0);
    }

    gpMouseManager->m_cursorReady = 1;
    PollSound();
    if (computeExtent != 0 || redrawExtent != 0) {
        redrawExtent = 0;
        gbLimitToExtent = 0;
        gbComputeExtent = 0;
        if (waitForTimer != 0)
            DelayTil(glTimers);
        glTimers[0] = static_cast<int>(
            KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * delay);
        gbFullCombatScreenDrawn = 0;
        if (updateScreen == 1) {
            if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
                giMaxExtentY = COMBAT_MAX_EXTENT_Y;
            gbEnlargeScreenBlit = 0;
            gpWindowManager->UpdateScreenRegion(
                giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
                giMaxExtentY - giMinExtentY + 1);
            gbEnlargeScreenBlit = 1;
        }
    } else if (updateScreen == 1) {
        if (waitForTimer != 0)
            DelayTil(glTimers);
        gbFullCombatScreenDrawn = 1;
        glTimers[0] = static_cast<int>(
            KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * delay);
        UpdateCombatArea();
    }

finish:
    gpMouseManager->m_cursorReady = 1;
    PollSound();
}

VA(0x00405d0b, 0xb99)
void combatManager::DrawSmallView(int viewIndex, int updateScreen)
{
    int iconX;
    int iconY2;
    int savedLimitToExtent9;
    army *viewArmy1;
    int viewX;
    int spellIndex1;
    int drawn6;
    unsigned char spellPositions[6][6][2] = {
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
    int viewY2;
    int spellCount6;
    int spellFrame1;

    if (gbNoShowCombat != 0)
        return;
    if (m_combatWindowOpen == 0)
        return;
    if (gConfig.combatArmyInfoLevel == 0)
        return;
    if (gbInDrawSmallView != 0)
        return;

    gbInDrawSmallView = 1;
    if (m_smallViewSide[viewIndex] == -1 ||
        (m_smallViewLastX[viewIndex] == COMBAT_SMALL_VIEW_LEFT_X &&
         m_smallViewSide[viewIndex] == 1) ||
        (m_smallViewLastX[viewIndex] == COMBAT_SMALL_VIEW_RIGHT_X &&
         m_smallViewSide[viewIndex] == 0)) {
        if (m_smallViewLastX[viewIndex] >= 0) {
            gbLimitToExtent = 1;
            giMinExtentX = m_smallViewLastX[viewIndex];
            giMinExtentY = m_smallViewLastY[viewIndex];
            giMaxExtentX = m_smallViewWidth[viewIndex] + m_smallViewLastX[viewIndex] - 1;
            giMaxExtentY = m_smallViewHeight[viewIndex] + m_smallViewLastY[viewIndex] - 1;
            DrawFrame(0, 0, 0, 0, 0, 1, 1);
            gpWindowManager->UpdateScreenRegion(
                giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
                giMaxExtentY - giMinExtentY + 1);
            gbLimitToExtent = 0;
            m_smallViewLastX[viewIndex] = -1;
        }
        if (m_smallViewSide[viewIndex] == -1) {
            gbInDrawSmallView = 0;
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
            viewY2 = m_smallViewSide[1] == m_smallViewSide[0]
                         ? COMBAT_SMALL_VIEW_FULL_RIGHT_Y
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
            viewY2 = m_smallViewSide[1] == m_smallViewSide[0]
                         ? COMBAT_SMALL_VIEW_COMPACT_RIGHT_Y
                         : COMBAT_SMALL_VIEW_COMPACT_Y;
        }
        m_smallViewWidth[viewIndex] = COMBAT_SMALL_VIEW_WIDTH;
        m_smallViewHeight[viewIndex] = COMBAT_SMALL_VIEW_COMPACT_HEIGHT;
    }

    m_smallViewLastX[viewIndex] = viewX;
    m_smallViewLastY[viewIndex] = viewY2;
    savedLimitToExtent9 = gbLimitToExtent;
    if (updateScreen != 0)
        gbLimitToExtent = 0;

    viewArmy1 = &m_armies[m_smallViewSide[viewIndex]][m_smallViewArmyIndex[viewIndex]];
    drawn6 = m_smallViewBackgroundIcon->CombatClipDrawToBuffer(
        viewX, viewY2, gConfig.combatArmyInfoLevel != COMBAT_SMALL_VIEW_FULL_INFO,
        &m_smallViewLimits, 0, 0, 0, 0);
    viewX += COMBAT_SMALL_VIEW_INSET_X;
    if (drawn6 != 0) {
        if (viewArmy1->m_quantity > 1)
            sprintf(gText, cMiniViewText[COMBAT_SMALL_VIEW_TEXT_QUANTITY_PLURAL],
                    viewArmy1->m_quantity);
        else
            sprintf(gText, cMiniViewText[COMBAT_SMALL_VIEW_TEXT_QUANTITY_SINGULAR],
                    viewArmy1->m_quantity);
        smallFont->DrawBoundedString(
            gText, viewX + COMBAT_SMALL_VIEW_TEXT_X,
            viewY2 + COMBAT_SMALL_VIEW_QUANTITY_Y, COMBAT_SMALL_VIEW_TEXT_WIDTH,
            COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 1);

        if (gConfig.combatArmyInfoLevel == COMBAT_SMALL_VIEW_FULL_INFO) {
            smallFont->DrawBoundedString(cMiniViewText[COMBAT_SMALL_VIEW_TEXT_ATTACK],
                                         viewX + COMBAT_SMALL_VIEW_TEXT_X,
                                         viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y,
                                         COMBAT_SMALL_VIEW_TEXT_WIDTH,
                                         COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 0);
            smallFont->DrawBoundedString(cMiniViewText[COMBAT_SMALL_VIEW_TEXT_DEFENSE],
                                         viewX + COMBAT_SMALL_VIEW_TEXT_X,
                                         viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                                             COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT,
                                         COMBAT_SMALL_VIEW_TEXT_WIDTH,
                                         COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 0);
            smallFont->DrawBoundedString(cMiniViewText[COMBAT_SMALL_VIEW_TEXT_HIT_POINTS],
                                         viewX + COMBAT_SMALL_VIEW_TEXT_X,
                                         viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                                             COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * 2,
                                         COMBAT_SMALL_VIEW_TEXT_WIDTH,
                                         COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 0);
            smallFont->DrawBoundedString(cMiniViewText[COMBAT_SMALL_VIEW_TEXT_DAMAGE],
                                         viewX + COMBAT_SMALL_VIEW_TEXT_X,
                                         viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                                             COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * 3,
                                         COMBAT_SMALL_VIEW_TEXT_WIDTH,
                                         COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 0);
            smallFont->DrawBoundedString(cMiniViewText[COMBAT_SMALL_VIEW_TEXT_MORALE],
                                         viewX + COMBAT_SMALL_VIEW_TEXT_X,
                                         viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                                             COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * 4,
                                         COMBAT_SMALL_VIEW_TEXT_WIDTH,
                                         COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 0);
            smallFont->DrawBoundedString(cMiniViewText[COMBAT_SMALL_VIEW_TEXT_LUCK],
                                         viewX + COMBAT_SMALL_VIEW_TEXT_X,
                                         viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                                             COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * 5,
                                         COMBAT_SMALL_VIEW_TEXT_WIDTH,
                                         COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 0);
            if (viewArmy1->m_monster.flags.all & COMBAT_ARMY_FLAG_SHOOTER)
                smallFont->DrawBoundedString(cMiniViewText[COMBAT_SMALL_VIEW_TEXT_SHOTS],
                                             viewX + COMBAT_SMALL_VIEW_TEXT_X,
                                             viewY2 + COMBAT_SMALL_VIEW_SHOTS_Y,
                                             COMBAT_SMALL_VIEW_TEXT_WIDTH,
                                             COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 0);

            sprintf(gText, "%d", static_cast<int>(viewArmy1->m_monster.attack));
            smallFont->DrawBoundedString(gText, viewX + COMBAT_SMALL_VIEW_TEXT_X,
                                         viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y,
                                         COMBAT_SMALL_VIEW_TEXT_WIDTH,
                                         COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 2);
            sprintf(gText, "%d", static_cast<int>(viewArmy1->m_monster.defense));
            smallFont->DrawBoundedString(gText, viewX + COMBAT_SMALL_VIEW_TEXT_X,
                                         viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                                             COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT,
                                         COMBAT_SMALL_VIEW_TEXT_WIDTH,
                                         COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 2);
            sprintf(gText, "%d",
                    static_cast<unsigned int>(viewArmy1->m_monster.hitPoints));
            smallFont->DrawBoundedString(gText, viewX + COMBAT_SMALL_VIEW_TEXT_X,
                                         viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                                             COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * 2,
                                         COMBAT_SMALL_VIEW_TEXT_WIDTH,
                                         COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 2);
            sprintf(gText, "%d-%d", static_cast<int>(viewArmy1->m_monster.damageMin),
                    static_cast<int>(viewArmy1->m_monster.damageMax));
            smallFont->DrawBoundedString(gText, viewX + COMBAT_SMALL_VIEW_TEXT_X,
                                         viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                                             COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * 3,
                                         COMBAT_SMALL_VIEW_TEXT_WIDTH,
                                         COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 2);

            spellIndex1 = 0;
            if (viewArmy1->m_morale > 0) {
                for (spellIndex1 = 0; spellIndex1 < viewArmy1->m_morale; spellIndex1++)
                    m_smallViewModifierIcon->DrawToBuffer(
                        viewX + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X -
                            spellIndex1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
                        viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                            COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * 4,
                        COMBAT_SMALL_VIEW_GOOD_MORALE_FRAME, 0);
            } else if (viewArmy1->m_morale < 0) {
                for (spellIndex1 = 0; spellIndex1 < -viewArmy1->m_morale; spellIndex1++)
                    m_smallViewModifierIcon->DrawToBuffer(
                        viewX + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X -
                            spellIndex1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
                        viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                            COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * 4,
                        COMBAT_SMALL_VIEW_BAD_MORALE_FRAME, 0);
            } else {
                m_smallViewModifierIcon->DrawToBuffer(
                    viewX + COMBAT_SMALL_VIEW_NEUTRAL_MORALE_X,
                    viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                        COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * 4,
                    COMBAT_SMALL_VIEW_NEUTRAL_MORALE_FRAME, 0);
            }

            spellIndex1 = 0;
            if (viewArmy1->m_luck > 0) {
                for (spellIndex1 = 0; spellIndex1 < viewArmy1->m_luck; spellIndex1++)
                    m_smallViewModifierIcon->DrawToBuffer(
                        viewX + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X -
                            spellIndex1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
                        viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                            COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * 5,
                        COMBAT_SMALL_VIEW_GOOD_LUCK_FRAME, 0);
            } else if (viewArmy1->m_luck < 0) {
                for (spellIndex1 = 0; spellIndex1 < -viewArmy1->m_luck; spellIndex1++)
                    m_smallViewModifierIcon->DrawToBuffer(
                        viewX + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X -
                            spellIndex1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
                        viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                            COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * 5,
                        COMBAT_SMALL_VIEW_BAD_LUCK_FRAME, 0);
            } else {
                m_smallViewModifierIcon->DrawToBuffer(
                    viewX + COMBAT_SMALL_VIEW_NEUTRAL_LUCK_X,
                    viewY2 + COMBAT_SMALL_VIEW_FIRST_STAT_Y +
                        COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * 5,
                    COMBAT_SMALL_VIEW_NEUTRAL_LUCK_FRAME, 0);
            }

            if (viewArmy1->m_monster.flags.all & COMBAT_ARMY_FLAG_SHOOTER) {
                sprintf(gText, "%d", static_cast<int>(viewArmy1->m_monster.shots));
                smallFont->DrawBoundedString(gText, viewX + COMBAT_SMALL_VIEW_TEXT_X,
                                             viewY2 + COMBAT_SMALL_VIEW_SHOTS_Y,
                                             COMBAT_SMALL_VIEW_TEXT_WIDTH,
                                             COMBAT_SMALL_VIEW_TEXT_HEIGHT, 1, 2);
            }
        }

        spellCount6 = viewArmy1->m_spellCount < COMBAT_SMALL_VIEW_MAX_SPELLS
                          ? viewArmy1->m_spellCount
                          : COMBAT_SMALL_VIEW_MAX_SPELLS;
        spellFrame1 = -1;
        for (spellIndex1 = 0; spellIndex1 < spellCount6; spellIndex1++) {
            spellFrame1++;
            while (viewArmy1->m_spellInfluence[spellFrame1] == 0)
                spellFrame1++;

            iconX = viewX + COMBAT_SMALL_VIEW_INSET_X;
            if (gConfig.combatArmyInfoLevel == COMBAT_SMALL_VIEW_FULL_INFO)
                iconY2 = viewY2 + COMBAT_SMALL_VIEW_FULL_SPELL_Y;
            else
                iconY2 = viewY2 + COMBAT_SMALL_VIEW_COMPACT_SPELL_Y;
            iconX += spellPositions[spellCount6 - 1][spellIndex1][0];
            iconY2 += spellPositions[spellCount6 - 1][spellIndex1][1];
            iconX += (COMBAT_SMALL_VIEW_ICON_SIZE -
                      GetIconEntry(m_smallViewSpellIcon, spellFrame1)->w) >> 1;
            iconY2 += (COMBAT_SMALL_VIEW_ICON_SIZE -
                       GetIconEntry(m_smallViewSpellIcon, spellFrame1)->h) >> 1;
            m_smallViewSpellIcon->DrawToBuffer(iconX, iconY2, spellFrame1, 0);
        }
    }

    gbLimitToExtent = savedLimitToExtent9;
    viewX -= COMBAT_SMALL_VIEW_INSET_X;
    if (updateScreen != 0)
        gpWindowManager->UpdateScreenRegion(viewX, viewY2, m_smallViewWidth[viewIndex],
                                            m_smallViewHeight[viewIndex]);
    gbInDrawSmallView = 0;
}

// ---- globals (definitions, RVA order) ----
DATA(0x004ed25c) int bGridWasShowing;
DATA(0x004ed290) int gbInDrawSmallView;
