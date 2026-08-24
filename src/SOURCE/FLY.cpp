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

#define ARMY_VAMPIRE_FLIGHT_DURATION_SCALE 1.3

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
        if (ValidHex(candidateHex)) {
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
    i32 attackHex7[ARMY_ATTACK_HEX_COUNT];
    u32 directionMask;
    army* target0;
    i32 spare;
    CombatHexDirection direction4;
    i32 attackMask29;
    i32 adjacentHex1;
    i32 cost;
    CombatHexDirection i5;
    i32 fittingHex3;
    CombatHexDirection initialDirection4;
    i32 targetHex;

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

    target0 = &gpCombatManager->m_armies[(m_targetSide)][m_targetIndex];
    if (pathMode != ARMY_PATH_ANY_TARGET_HEX) {
        targetHex = destination;
    } else {
        targetHex = target0->m_hex;
    }
    if (!ValidHex(targetHex)) {
        return 0;
    }

    attackMask29 = GetAttackMask(m_hex, ARMY_ATTACK_TARGET_ASSIGNED, ARMY_HEX_INVALID);
    while (attackMask29 != ARMY_ALL_ATTACK_DIRECTIONS) {
        initialDirection4 = GetBestDirection(m_hex, targetHex, attackMask29);
        if (ValidAttack(
                m_hex,
                initialDirection4,
                ARMY_ATTACK_TARGET_ASSIGNED,
                ARMY_HEX_INVALID,
                attackHex7
            )) {
            m_attackDirection = initialDirection4;
            m_moveTargetHex = m_hex;
            return 1;
        } else {
            attackMask29 |= 1 << (initialDirection4);
        }
    }

    directionMask = 0;
    if ((((target0->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))
        && pathMode == ARMY_PATH_ANY_TARGET_HEX) {
        if (target0->m_facing == ARMY_FACING_RIGHT) {
            targetHex++;
        } else {
            targetHex--;
        }
        if (target0->m_facing == ARMY_FACING_RIGHT) {
            directionMask = (1 << (COMBAT_DIRECTION_WEST));
        } else {
            directionMask = (1 << (COMBAT_DIRECTION_EAST));
        }
    }
    while (directionMask != ALL_ADJACENT_DIRECTIONS) {
        direction4 = GetBestDirection(targetHex, m_hex, directionMask);
        adjacentHex1 = GetAdjacentCellIndex(targetHex, direction4);
        if (ValidHex(adjacentHex1)
            && CanFit(adjacentHex1, 1 - (pathMode), &fittingHex3)) {
            m_moveTargetHex = fittingHex3;
            if (!(((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                m_attackDirection = OppositeDirection(direction4);
            } else {
                attackMask29 = ~GetAttackMask(
                    m_moveTargetHex, ARMY_ATTACK_TARGET_ASSIGNED, ARMY_HEX_INVALID
                );
                for (i5 = COMBAT_DIRECTION_NORTHEAST; (i5) < ARMY_COMBAT_DIRECTION_COUNT; i5++) {
                    if (attackMask29 & (1 << (i5))) {
                        m_attackDirection = i5;
                    }
                }
            }
            return 1;
        } else {
            directionMask |= 1 << (direction4);
        }
    }

    if ((((target0->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))
        && pathMode == ARMY_PATH_ANY_TARGET_HEX) {
        if (target0->m_facing == ARMY_FACING_RIGHT) {
            targetHex--;
        } else {
            targetHex++;
        }
        if (target0->m_facing == ARMY_FACING_RIGHT) {
            directionMask = (1 << (COMBAT_DIRECTION_EAST));
        } else {
            directionMask = (1 << (COMBAT_DIRECTION_WEST));
        }
        while (directionMask != ALL_ADJACENT_DIRECTIONS) {
            direction4 = GetBestDirection(targetHex, m_hex, directionMask);
            adjacentHex1 = GetAdjacentCellIndex(targetHex, direction4);
            if (ValidHex(adjacentHex1) && CanFit(adjacentHex1, 0, NULL)) {
                m_moveTargetHex = adjacentHex1;
                m_attackDirection = GetBestDirection(m_moveTargetHex, targetHex, 0);
                return 1;
            } else {
                directionMask |= 1 << (direction4);
            }
        }
    }
    return 0;
}

i32 army::FlyTo(void) {
    return FlyTo(m_moveTargetHex);
}

i32 army::FlyTo(i32 destination) {
    i32 endRearHex;
    i32 oldMinExtentY;
    i32 middleFrames2;
    i32 prevMinX5;
    i32 frameBegin3;
    i32 oldMaxY;
    i32 oldRight;
    i32 flightFrameCount3;
    i32 sourceRearHex;
    i32 segmentIndex;
    i32 flightSteps;
    i32 pad2;
    i32 destinationColumn;
    i32 goalY;
    i32 destinationX;
    i32 sourceY;
    i32 dist;
    i32 sourceX;
    i32 unusedB;
    float yRate3;
    i32 ySpan;
    float xIncrement3;
    i32 dead2;
    float flyY1;
    i32 xDistance;
    i32 hexColumn;
    i32 columnDiff5;
    float xPos;

    if (!ValidHex(destination)) {
        return 0;
    }

    hexColumn = m_hex % ARMY_HEX_COLUMNS;
    destinationColumn = destination % ARMY_HEX_COLUMNS;
    columnDiff5 = destinationColumn - hexColumn;
    m_facingChanged = 0;
    if (columnDiff5 > 0 && m_facing == ARMY_FACING_LEFT) {
        m_facingChanged = 1;
        m_facing = OppositeArmyFacing(m_facing);
        if ((((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            m_hex--;
            destination--;
        }
    } else if (columnDiff5 < 0 && m_facing == ARMY_FACING_RIGHT) {
        m_facingChanged = 1;
        m_facing = OppositeArmyFacing(m_facing);
        if ((((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            m_hex++;
            destination++;
        }
    }
    if (m_facingChanged) {
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }

    destinationX = gpCombatManager->m_hexCells[destination].m_x;
    goalY = gpCombatManager->m_hexCells[destination].m_y;
    sourceX = gpCombatManager->m_hexCells[m_hex].m_x;
    sourceY = gpCombatManager->m_hexCells[m_hex].m_y;
    xPos = static_cast<float>(sourceX);
    flyY1 = static_cast<float>(sourceY);
    xDistance = destinationX - sourceX;
    ySpan = goalY - sourceY;
    dist =
        static_cast<i32>(sqrt(static_cast<double>(xDistance * xDistance + ySpan * ySpan)));
    flightSteps = 0;
    if (m_frameInfo.flightSpeed > 0) {
        flightSteps = ((m_frameInfo.flightSpeed >> 1) + dist) / m_frameInfo.flightSpeed;
    }
    if (flightSteps <= 0) {
        flightSteps = 1;
    }
    xIncrement3 = static_cast<float>(xDistance) / flightSteps;
    yRate3 = static_cast<float>(ySpan) / flightSteps;

    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = COMBAT_SIDE_NONE;
    gpCombatManager->m_hexCells[m_hex].m_occupantFrame = ARMY_FACING_NONE;
    if ((((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
        sourceRearHex = (static_cast<u32>(m_facing) < static_cast<u32>(ARMY_FACING_RIGHT) ? -1 : 1) + m_hex;
        gpCombatManager->m_hexCells[sourceRearHex].m_occupantIndex = -1;
        gpCombatManager->m_hexCells[sourceRearHex].m_occupantSide = COMBAT_SIDE_NONE;
        gpCombatManager->m_hexCells[sourceRearHex].m_occupantFrame = ARMY_FACING_NONE;
    }

    if (!gbNoShowCombat) {
        flightFrameCount3 = 0;
        frameBegin3 = 0;
        middleFrames2 = 0;
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
        gpCombatManager->m_backgroundDrawn = 0;
        m_animationSequence = ARMY_ANIMATION_WALK;
        for (segmentIndex = 0; !(flightSteps <= segmentIndex); segmentIndex++) {
            BuildTempWalkSeq(
                &m_frameInfo,
                segmentIndex + 1 == flightSteps,
                segmentIndex > 0
            );
            if (flightSteps == 0) {
                flightFrameCount3 = m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK)];
                frameBegin3 = 0;
            } else {
                if (segmentIndex > 0) {
                    flightFrameCount3 +=
                        m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK_BEGIN_STANDING)];
                    frameBegin3 = 0;
                } else {
                    frameBegin3 = m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK_BEGIN)];
                }
                flightFrameCount3 = m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK_MIDDLE)];
                middleFrames2 = flightFrameCount3;
                if (segmentIndex + 1 < flightSteps) {
                    flightFrameCount3 += m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK_END)];
                }
            }

            for (m_animationFrame = 0;
                 m_animationFrame < m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK)];
                 m_animationFrame++) {
                if (m_animationFrame >= frameBegin3
                    && m_animationFrame < flightFrameCount3 + frameBegin3) {
                    xPos += xIncrement3 / flightFrameCount3;
                    flyY1 += yRate3 / flightFrameCount3;
                }
                if (m_animationFrame % m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK)]
                    == FLIGHT_SOUND_FRAME) {
                    if ((m_monsterType == CREATURE_VAMPIRE
                         || m_monsterType == CREATURE_VAMPIRE_LORD)
                        && segmentIndex == 0) {
                        gpSoundManager->MemorySample(m_samples[(ARMY_SAMPLE_EXTRA_ONE)]);
                        DelayMilli(VAMPIRE_FLIGHT_SOUND_DELAY);
                    } else if ((m_monsterType == CREATURE_VAMPIRE
                                || m_monsterType == CREATURE_VAMPIRE_LORD)
                               && flightSteps - 1 == segmentIndex) {
                        gpSoundManager->MemorySample(m_samples[(ARMY_SAMPLE_EXTRA_TWO)]);
                    } else {
                        gpSoundManager->MemorySample(m_samples[(ARMY_SAMPLE_MOVE)]);
                    }
                }

                if (segmentIndex != 0 || m_animationFrame != 0) {
                    gpCombatManager->m_backgroundBuffer->CopyTo(
                        gpWindowManager->m_screen,
                        giMinExtentX,
                        giMinExtentY,
                        giMinExtentX,
                        giMinExtentY,
                        giMaxExtentX - giMinExtentX + 1,
                        giMaxExtentY - giMinExtentY + 1
                    );
                    prevMinX5 = giMinExtentX;
                    oldMinExtentY = giMinExtentY;
                    oldRight = giMaxExtentX;
                    oldMaxY = giMaxExtentY;
                } else {
                    prevMinX5 = 0;
                    oldMinExtentY = 0;
                    oldRight = ARMY_COMBAT_MAX_X;
                    oldMaxY = ARMY_COMBAT_MAX_Y;
                }
                giMinExtentY = ARMY_COMBAT_WIDTH;
                giMinExtentX = giMinExtentY;
                giMaxExtentY = 0;
                giMaxExtentX = giMaxExtentY;
                gbComputeExtent = true;
                gbSaveBiggestExtent = true;
                DrawToBuffer(static_cast<i32>(xPos), static_cast<i32>(flyY1), 0);
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
                if (!(prevMinX5 <= giMinExtentX))
                    prevMinX5 = giMinExtentX;
                if (!(oldMinExtentY <= giMinExtentY))
                    oldMinExtentY = giMinExtentY;
                if (oldRight < giMaxExtentX)
                    oldRight = giMaxExtentX;
                if (oldMaxY < giMaxExtentY)
                    oldMaxY = giMaxExtentY;

                DelayTil(glTimers);
                if (m_animationFrame < frameBegin3
                    || (m_animationFrame + 1 >= middleFrames2
                        && (m_monsterType == CREATURE_VAMPIRE
                            || m_monsterType == CREATURE_VAMPIRE_LORD))) {
                    glTimers[0] = static_cast<i32>(
                        KBTickCount()
                        + m_frameInfo.walkDuration * gfCombatSpeedMod[gConfig.combatSpeed]
                              * ARMY_VAMPIRE_FLIGHT_DURATION_SCALE / flightFrameCount3
                    );
                } else {
                    glTimers[0] = static_cast<i32>(
                        KBTickCount()
                        + m_frameInfo.walkDuration * gfCombatSpeedMod[gConfig.combatSpeed]
                              / flightFrameCount3
                    );
                }
                gpWindowManager->UpdateScreenRegion(
                    prevMinX5,
                    oldMinExtentY,
                    oldRight - prevMinX5 + 1,
                    oldMaxY - oldMinExtentY + 1
                );
                if (m_animationFrame
                    == m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WALK)] - 1) {
                    xPos = sourceX + (segmentIndex + 1) * xIncrement3;
                    flyY1 = sourceY + (segmentIndex + 1) * yRate3;
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
        endRearHex =
            (static_cast<u32>(m_facing) < static_cast<u32>(ARMY_FACING_RIGHT) ? -1 : 1) + destination;
        gpCombatManager->m_hexCells[endRearHex].m_occupantSide =
            static_cast<i8>(gpCombatManager->m_currentArmySide);
        gpCombatManager->m_hexCells[endRearHex].m_occupantIndex =
            static_cast<i8>(gpCombatManager->m_currentArmyIndex);
        if (endRearHex >= destination) {
            gpCombatManager->m_hexCells[endRearHex].m_occupantFrame = ARMY_FACING_RIGHT;
        } else {
            gpCombatManager->m_hexCells[endRearHex].m_occupantFrame = ARMY_FACING_LEFT;
        }
        if (endRearHex <= destination) {
            gpCombatManager->m_hexCells[destination].m_occupantFrame = ARMY_FACING_RIGHT;
        } else {
            gpCombatManager->m_hexCells[destination].m_occupantFrame = ARMY_FACING_LEFT;
        }
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
        m_facingChanged = 0;
    }
    gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    gpCombatManager->TestRaiseDoor();
    return 1;
}
