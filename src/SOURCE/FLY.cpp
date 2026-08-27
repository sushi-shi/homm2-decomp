#include <Ints.h>
#include <math.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/soundManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/army.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/PATH.h>
#include <SOURCE/X_GLOBAL.h>

#define ARMY_VAMPIRE_FLIGHT_DURATION_SCALE \
    1.3

namespace {

typedef enum ArmyFlightConstant {
    ALL_ADJACENT_DIRECTIONS    = 0x3f,
    FLIGHT_SOUND_FRAME         = 1,
    VAMPIRE_FLIGHT_SOUND_DELAY = 100
} ArmyFlightConstant;

}

i32 army::CanFit(i32 hex, i32 tryOtherSide, i32* fittingHex) {
    hexcell* cell_9;
    i32 candidateHex;

    candidateHex = hex;
    cell_9 = NULL;
    if (fittingHex) {
        *fittingHex = hex;
    }
    if (!ValidHex(candidateHex) || candidateHex % ARMY_HEX_COLUMNS == 0
        || candidateHex % ARMY_HEX_COLUMNS == ARMY_HEX_COLUMNS - 1) {
        return 0;
    }
    if (gpCombatManager->m_hexCells[candidateHex].m_occupantSide != COMBAT_SIDE_NONE
        || gpCombatManager->m_hexCells[candidateHex].m_blocked) {
        return 0;
    }
    if ((((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
        candidateHex = GetAdjacentCellIndex(
            hex,
            m_facing == ARMY_FACING_RIGHT ? COMBAT_DIRECTION_EAST : COMBAT_DIRECTION_WEST
        );
        if (ValidHex(candidateHex) && candidateHex % ARMY_HEX_COLUMNS != 0
            && candidateHex % ARMY_HEX_COLUMNS != ARMY_HEX_COLUMNS - 1) {
            cell_9 = &gpCombatManager->m_hexCells[candidateHex];
        }
        if (ValidHex(candidateHex)
            && (cell_9->m_occupantSide == COMBAT_SIDE_NONE
                || (cell_9->m_occupantSide == gpCombatManager->m_currentArmySide
                    && cell_9->m_occupantIndex == gpCombatManager->m_currentArmyIndex))
            && !cell_9->m_blocked) {
            return 1;
        } else {
            if (!tryOtherSide) {
                return 0;
            } else {
                candidateHex = GetAdjacentCellIndex(
                    hex,
                    m_facing == ARMY_FACING_RIGHT ? COMBAT_DIRECTION_WEST : COMBAT_DIRECTION_EAST
                );
                if (ValidHex(candidateHex)) {
                    cell_9 = &gpCombatManager->m_hexCells[candidateHex];
                } else {
                    return 0;
                }
                if ((cell_9->m_occupantSide == COMBAT_SIDE_NONE
                     || (cell_9->m_occupantSide == gpCombatManager->m_currentArmySide
                         && cell_9->m_occupantIndex == gpCombatManager->m_currentArmyIndex))
                    && !cell_9->m_blocked) {
                    if (fittingHex) {
                        *fittingHex = candidateHex;
                    }
                    return 1;
                } else {
                    return 0;
                }
            }
        }
    } else {
        return 1;
    }
}

i32 army::ValidFlight(i32 destination, ArmyPathTarget pathMode) {
    i32 enemyHex;
    u32 directionMask;
    i32 adjHex;
    i32 spare [[maybe_unused]];
    CombatHexDirection direction;
    i32 cost [[maybe_unused]];
    army* foe;
    i32 otherHex [[maybe_unused]];
    i32 attackMask;
    i32 attackHex;
    CombatHexDirection n;
    i32 freeHex;
    CombatHexDirection moveDir;

    if (!ValidHex(destination)) {
        return 0;
    }
    if (m_targetSide < COMBAT_ATTACKER_SIDE || m_targetSide > COMBAT_DEFENDER_SIDE
        || m_targetIndex < 0
        || m_targetIndex > COMBAT_ARMY_SLOT_COUNT - 1) {
        if (CanFit(destination, 0, NULL)) {
            m_moveTargetHex = destination;
            return 1;
        } else {
            return 0;
        }
    }

    foe = &gpCombatManager->m_armies[(m_targetSide)][m_targetIndex];
    if (pathMode != ARMY_PATH_ANY_TARGET_HEX) {
        enemyHex = destination;
    } else {
        enemyHex = foe->m_hex;
    }
    if (!ValidHex(enemyHex)) {
        return 0;
    }

    attackMask = GetAttackMask(m_hex, ARMY_ATTACK_TARGET_ASSIGNED, ARMY_HEX_INVALID);
    while (attackMask != ARMY_ALL_ATTACK_DIRECTIONS) {
        moveDir = GetBestDirection(m_hex, enemyHex, attackMask);
        if (ValidAttack(
                m_hex,
                moveDir,
                ARMY_ATTACK_TARGET_ASSIGNED,
                ARMY_HEX_INVALID,
                &attackHex
            )) {
            m_attackDirection = moveDir;
            m_moveTargetHex = m_hex;
            return 1;
        } else {
            attackMask |= 1 << (moveDir);
        }
    }

    directionMask = 0;
    if ((((foe->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))
        && pathMode == ARMY_PATH_ANY_TARGET_HEX) {
        enemyHex += foe->m_facing == ARMY_FACING_RIGHT ? 1 : -1;
        directionMask = foe->m_facing == ARMY_FACING_RIGHT ? (1 << (COMBAT_DIRECTION_WEST))
                                                              : (1 << (COMBAT_DIRECTION_EAST));
    }
    while (directionMask != ALL_ADJACENT_DIRECTIONS) {
        direction = GetBestDirection(enemyHex, m_hex, directionMask);
        adjHex = GetAdjacentCellIndex(enemyHex, direction);
        if (ValidHex(adjHex)
            && CanFit(adjHex, 1 - (pathMode), &freeHex)) {
            m_moveTargetHex = freeHex;
            if (!(((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                m_attackDirection = OppositeDirection(direction);
            } else {
                attackMask = ~GetAttackMask(
                    m_moveTargetHex, ARMY_ATTACK_TARGET_ASSIGNED, ARMY_HEX_INVALID
                );
                for (n = COMBAT_DIRECTION_NORTHEAST; (n) < ARMY_COMBAT_DIRECTION_COUNT; n++) {
                    if (attackMask & (1 << (n))) {
                        m_attackDirection = n;
                    }
                }
            }
            return 1;
        } else {
            directionMask |= 1 << (direction);
        }
    }

    if ((((foe->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))
        && pathMode == ARMY_PATH_ANY_TARGET_HEX) {
        enemyHex += foe->m_facing == ARMY_FACING_RIGHT ? -1 : 1;
        directionMask = foe->m_facing == ARMY_FACING_RIGHT ? (1 << (COMBAT_DIRECTION_EAST))
                                                           : (1 << (COMBAT_DIRECTION_WEST));
        while (directionMask != ALL_ADJACENT_DIRECTIONS) {
            direction = GetBestDirection(enemyHex, m_hex, directionMask);
            adjHex = GetAdjacentCellIndex(enemyHex, direction);
            if (ValidHex(adjHex) && CanFit(adjHex, 0, NULL)) {
                m_moveTargetHex = adjHex;
                m_attackDirection = GetBestDirection(m_moveTargetHex, enemyHex, 0);
                return 1;
            } else {
                directionMask |= 1 << (direction);
            }
        }
    }
    return 0;
}

i32 army::FlyTo(void) {
    return FlyTo(m_moveTargetHex);
}

i32 army::FlyTo(i32 destination) {
    float xPos;
    float yRate0;
    float yPos;
    i32 endX;
    i32 columnDelta1;
    i32 fromX;
    i32 xDistance;
    i32 toColumn1;
    i32 pad9 [[maybe_unused]];
    i32 dead2 [[maybe_unused]];
    i32 stepCount1;
    i32 endY;
    i32 leg;
    i32 sourceY;
    i32 ySpan0;
    float xSpeed;
    i32 column;
    i32 frameCount0;
    i32 srcRearHex0;
    i32 slack [[maybe_unused]];
    i32 length;
    i32 lastMinX;
    i32 oldMaxX0;
    i32 oldMinY;
    i32 midCount;
    i32 frameStart;
    i32 endRearHex;
    i32 oldMaxY;

    if (!ValidHex(destination)) {
        return 0;
    }

    column = m_hex % ARMY_HEX_COLUMNS;
    toColumn1 = destination % ARMY_HEX_COLUMNS;
    columnDelta1 = toColumn1 - column;
    m_facingChanged = false;
    if (columnDelta1 > 0 && m_facing == ARMY_FACING_LEFT) {
        m_facingChanged = true;
        m_facing = OppositeArmyFacing(m_facing);
        if ((((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            m_hex--;
            destination--;
        }
    } else if (columnDelta1 < 0 && m_facing == ARMY_FACING_RIGHT) {
        m_facingChanged = true;
        m_facing = OppositeArmyFacing(m_facing);
        if ((((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            m_hex++;
            destination++;
        }
    }
    if (m_facingChanged) {
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }

    endX = gpCombatManager->m_hexCells[destination].m_x;
    endY = gpCombatManager->m_hexCells[destination].m_y;
    fromX = gpCombatManager->m_hexCells[m_hex].m_x;
    sourceY = gpCombatManager->m_hexCells[m_hex].m_y;
    xPos = static_cast<float>(fromX);
    yPos = static_cast<float>(sourceY);
    xDistance = endX - fromX;
    ySpan0 = endY - sourceY;
    length =
        static_cast<i32>(sqrt(static_cast<double>(xDistance * xDistance + ySpan0 * ySpan0)));
    stepCount1 = 0;
    if (m_frameInfo.flightSpeed > 0) {
        stepCount1 = (length + (m_frameInfo.flightSpeed >> 1)) / m_frameInfo.flightSpeed;
    }
    if (stepCount1 <= 0) {
        stepCount1 = 1;
    }


    xSpeed = static_cast<float>(xDistance) / (static_cast<float>(stepCount1));
    yRate0 = static_cast<float>(ySpan0) / (static_cast<float>(stepCount1));

    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = COMBAT_SIDE_NONE;
    gpCombatManager->m_hexCells[m_hex].m_occupantFrame = ARMY_FACING_NONE;
    if ((((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
        srcRearHex0 = m_hex + (m_facing == ARMY_FACING_LEFT ? -1 : 1);
        gpCombatManager->m_hexCells[srcRearHex0].m_occupantIndex = -1;
        gpCombatManager->m_hexCells[srcRearHex0].m_occupantSide = COMBAT_SIDE_NONE;
        gpCombatManager->m_hexCells[srcRearHex0].m_occupantFrame = ARMY_FACING_NONE;
    }

    if (!gbNoShowCombat) {
        frameCount0 = 0;
        frameStart = 0;
        midCount = 0;
        gpCombatManager->DrawFrame(0, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        gpWindowManager->m_screen->CopyTo(
            gpCombatManager->m_backgroundBuffer,
            0,
            0,
            0,
            0,
            ARMY_COMBAT_WIDTH,
            ARMY_COMBAT_MAX_Y
        );
        gpCombatManager->m_backgroundDrawn = false;
        m_animationSequence = ARMY_ANIMATION_WALK;
        for (leg = 0; leg < stepCount1; leg++) {
            BuildTempWalkSeq(
                &m_frameInfo,
                leg + 1 == stepCount1,
                leg > 0
            );
            if (stepCount1 == 0) {
                frameCount0 = m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK)];
                frameStart = 0;
            } else {
                if (leg > 0) {
                    frameCount0 +=
                        m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK_BEGIN_STANDING)];
                    frameStart = 0;
                } else {
                    frameStart = m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK_BEGIN)];
                }
                frameCount0 = m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK_MIDDLE)];
                midCount = frameCount0;
                if (leg + 1 < stepCount1) {
                    frameCount0 += m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK_END)];
                }
            }

            for (m_animationFrame = 0;
                 m_animationFrame < m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK)];
                 m_animationFrame++) {
                if (m_animationFrame >= frameStart
                    && m_animationFrame < frameStart + frameCount0) {
                    xPos += xSpeed / frameCount0;
                    yPos += yRate0 / frameCount0;
                }
                if (m_animationFrame % m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK)]
                    == FLIGHT_SOUND_FRAME) {
                    if ((m_monsterType == CREATURE_VAMPIRE
                         || m_monsterType == CREATURE_VAMPIRE_LORD)
                        && leg == 0) {
                        gpSoundManager->MemorySample(m_samples[(ARMY_SAMPLE_EXTRA_ONE)]);
                        DelayMilli(VAMPIRE_FLIGHT_SOUND_DELAY);
                    } else if ((m_monsterType == CREATURE_VAMPIRE
                                || m_monsterType == CREATURE_VAMPIRE_LORD)
                               && leg == stepCount1 - 1) {
                        gpSoundManager->MemorySample(m_samples[(ARMY_SAMPLE_EXTRA_TWO)]);
                    } else {
                        gpSoundManager->MemorySample(m_samples[(ARMY_SAMPLE_MOVE)]);
                    }
                }

                if (leg != 0 || m_animationFrame != 0) {
                    gpCombatManager->m_backgroundBuffer->CopyTo(
                        gpWindowManager->m_screen,
                        giMinExtentX,
                        giMinExtentY,
                        giMinExtentX,
                        giMinExtentY,
                        giMaxExtentX - giMinExtentX + 1,
                        giMaxExtentY - giMinExtentY + 1
                    );
                    lastMinX = giMinExtentX;
                    oldMinY = giMinExtentY;
                    oldMaxX0 = giMaxExtentX;
                    oldMaxY = giMaxExtentY;
                } else {
                    lastMinX = 0;
                    oldMinY = 0;
                    oldMaxX0 = ARMY_COMBAT_MAX_X;
                    oldMaxY = ARMY_COMBAT_MAX_Y;
                }
                giMinExtentY = ARMY_COMBAT_WIDTH;
                giMinExtentX = giMinExtentY;
                giMaxExtentY = 0;
                giMaxExtentX = giMaxExtentY;
                gbComputeExtent = true;
                gbSaveBiggestExtent = true;
                DrawToBuffer(static_cast<i32>(xPos), static_cast<i32>(yPos), 0);
                gbComputeExtent = false;
                gbSaveBiggestExtent = false;
                if (giMinExtentX < 0)
                    giMinExtentX = 0;
                if (giMinExtentY < 0)
                    giMinExtentY = 0;
                if (giMaxExtentX > ARMY_COMBAT_MAX_X)
                    giMaxExtentX = ARMY_COMBAT_MAX_X;
                if (giMaxExtentY > ARMY_COMBAT_MAX_Y)
                    giMaxExtentY = ARMY_COMBAT_MAX_Y;
                if (giMinExtentX < lastMinX)
                    lastMinX = giMinExtentX;
                if (giMinExtentY < oldMinY)
                    oldMinY = giMinExtentY;
                if (giMaxExtentX > oldMaxX0)
                    oldMaxX0 = giMaxExtentX;
                if (giMaxExtentY > oldMaxY)
                    oldMaxY = giMaxExtentY;

                DelayTil(glTimers);
                if (m_animationFrame < frameStart
                    || (m_animationFrame + 1 >= midCount
                        && (m_monsterType == CREATURE_VAMPIRE
                            || m_monsterType == CREATURE_VAMPIRE_LORD))) {
                    glTimers[0] = static_cast<i32>(
                        KBTickCount()
                        + m_frameInfo.walkDuration * ARMY_VAMPIRE_FLIGHT_DURATION_SCALE
                              * gfCombatSpeedMod[gConfig.combatSpeed] / frameCount0
                    );
                } else {
                    glTimers[0] = static_cast<i32>(
                        KBTickCount()
                        + m_frameInfo.walkDuration * gfCombatSpeedMod[gConfig.combatSpeed]
                              / frameCount0
                    );
                }
                gpWindowManager->UpdateScreenRegion(
                    lastMinX,
                    oldMinY,
                    oldMaxX0 - lastMinX + 1,
                    oldMaxY - oldMinY + 1
                );
                if (m_animationFrame
                    == m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK)] - 1) {
                    xPos = fromX + (leg + 1) * xSpeed;
                    yPos = sourceY + (leg + 1) * yRate0;
                }
            }
        }
    }

    CancelSpellType(ARMY_CANCEL_SPELLS_AFTER_MOVE);
    gpCombatManager->m_hexCells[destination].m_occupantSide =
        static_cast<i8>(gpCombatManager->m_currentArmySide);
    gpCombatManager->m_hexCells[destination].m_occupantIndex =
        static_cast<i8>(gpCombatManager->m_currentArmyIndex);
    gpCombatManager->m_hexCells[destination].m_occupantFrame = ARMY_FACING_NONE;
    if ((((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
        endRearHex = destination + (m_facing == ARMY_FACING_LEFT ? -1 : 1);
        gpCombatManager->m_hexCells[endRearHex].m_occupantSide =
            static_cast<i8>(gpCombatManager->m_currentArmySide);
        gpCombatManager->m_hexCells[endRearHex].m_occupantIndex =
            static_cast<i8>(gpCombatManager->m_currentArmyIndex);
        gpCombatManager->m_hexCells[endRearHex].m_occupantFrame =
            endRearHex >= destination ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
        gpCombatManager->m_hexCells[destination].m_occupantFrame =
            destination >= endRearHex ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
    }
    m_hex = destination;
    m_animationSequence = ARMY_ANIMATION_STAND;
    m_animationFrame = 0;
    if (m_facingChanged) {
        m_facing = OppositeArmyFacing(m_facing);
        if ((((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            if (m_facing == ARMY_FACING_LEFT) {
                m_hex++;
                destination++;
            } else {
                m_hex--;
                destination--;
            }
        }
        m_facingChanged = false;
    }
    gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    gpCombatManager->TestRaiseDoor();
    return 1;
}
