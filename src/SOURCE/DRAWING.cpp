// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\DRAWING.OBJ   from: (directly linked into exe)
// functions: 13   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdlib.h>
#include <BASE/Icon2b.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/mouseManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/hero.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/town.h>
VA(0x00402910, 0x41)
void combatManager::NoShowCombatLog(char *) {}

VA(0x00402951, 0xda)
void combatManager::ClearCombatMessages(int) {}

VA(0x00402a2b, 0x5d)
void combatManager::CheckUpdateCombatMessages(void) {}

VA(0x00402a88, 0x3f8)
void combatManager::CombatMessage(char *, int, int, int) {}

VA(0x00402e80, 0x3fe)
void combatManager::CombatMessage(int) {}

VA(0x0040327e, 0x126)
void combatManager::ResetLimitCreature(void) {}

VA(0x004033a4, 0x6a)
void combatManager::UpdateCombatArea(void) {}

VA(0x0040340e, 0x213)
void combatManager::SetupGridForArmy(class army *) {}

VA(0x00403621, 0x5fb)
int combatManager::UpdateGrid(int, int) { return 0; }

VA(0x00403c1c, 0x364)
void combatManager::DrawBackground(void) {}

VA(0x00403f80, 0x64c)
void combatManager::UpdateMouseGrid(int, int) {}

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
void combatManager::DrawSmallView(int, int) {}

// ---- globals (definitions, RVA order) ----
DATA(0x004ed25c) int bGridWasShowing;
DATA(0x004ed290) int gbInDrawSmallView;
