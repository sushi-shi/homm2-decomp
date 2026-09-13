#include <va.h>
#include <SOURCE/KB_TYPES.h>
#include <math.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/soundManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/army.h>
#include <BASE/Misc.h>
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

H2_ENUM_BEGIN(ArmyFlightConstant)
    ALL_ADJACENT_DIRECTIONS    = 0x3f,
    FLIGHT_SOUND_FRAME         = 1,
    VAMPIRE_FLIGHT_SOUND_DELAY = 100
H2_ENUM_END(ArmyFlightConstant)

}

#if H2_RETAIL_COMPILER
#define cell cell_9
#endif
VA(0x0044b000, 0x21e)
i32 army::CanFit(i32 hex, i32 tryOtherSide, i32* fittingHex) {
    hexcell* cell;
    i32 candidateHex;

    candidateHex = hex;
    cell = NULL;
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
    if (HAS(m_monster.attributes, MONSTER_FLAGS_WIDE)) {
        candidateHex = GetAdjacentCellIndex(
            hex,
            m_facing == ARMY_FACING_RIGHT ? COMBAT_DIRECTION_EAST : COMBAT_DIRECTION_WEST
        );
        if (ValidHex(candidateHex) && candidateHex % ARMY_HEX_COLUMNS != 0
            && candidateHex % ARMY_HEX_COLUMNS != ARMY_HEX_COLUMNS - 1) {
            cell = &gpCombatManager->m_hexCells[candidateHex];
        }
        if (ValidHex(candidateHex)
            && (cell->m_occupantSide == COMBAT_SIDE_NONE
                || HEX_HAS_OCCUPANT(
                    *cell,
                    gpCombatManager->m_currentArmySide,
                    gpCombatManager->m_currentArmyIndex
                ))
            && !cell->m_blocked) {
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
                    cell = &gpCombatManager->m_hexCells[candidateHex];
                } else {
                    return 0;
                }
                if ((cell->m_occupantSide == COMBAT_SIDE_NONE
                     || HEX_HAS_OCCUPANT(
                         *cell,
                         gpCombatManager->m_currentArmySide,
                         gpCombatManager->m_currentArmyIndex
                     ))
                    && !cell->m_blocked) {
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
#if H2_RETAIL_COMPILER
#undef cell
#endif

#if H2_RETAIL_COMPILER
#define armyPointer foe
#define currentDirection n
#define moveDirection moveDir
#endif
VA(0x0044b21e, 0x396)
i32 army::ValidFlight(i32 destination, ArmyPathTarget pathMode) {
    i32 enemyHex;
    u32 directionMask;
    i32 adjHex;
    i32 H2_UNUSED(spare);
    CombatHexDirection direction;
    i32 H2_UNUSED(cost);
    army* armyPointer;
    i32 H2_UNUSED(otherHex);
    i32 attackMask;
    i32 attackHex;
    CombatHexDirection currentDirection;
    i32 freeHex;
    CombatHexDirection moveDirection;

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

    armyPointer = &gpCombatManager->m_armies[IDX(m_targetSide)][m_targetIndex];
    if (pathMode != ARMY_PATH_ANY_TARGET_HEX) {
        enemyHex = destination;
    } else {
        enemyHex = armyPointer->m_hex;
    }
    if (!ValidHex(enemyHex)) {
        return 0;
    }

    attackMask = GetAttackMask(m_hex, ARMY_ATTACK_TARGET_ASSIGNED, ARMY_HEX_INVALID);
    while (attackMask != ARMY_ALL_ATTACK_DIRECTIONS) {
        moveDirection = GetBestDirection(m_hex, enemyHex, attackMask);
        if (ValidAttack(
                m_hex,
                moveDirection,
                ARMY_ATTACK_TARGET_ASSIGNED,
                ARMY_HEX_INVALID,
                &attackHex
            )) {
            m_attackDirection = moveDirection;
            m_moveTargetHex = m_hex;
            return 1;
        } else {
            attackMask |= 1 << IDX(moveDirection);
        }
    }

    directionMask = 0;
    if (HAS(armyPointer->m_monster.attributes, MONSTER_FLAGS_WIDE)
        && pathMode == ARMY_PATH_ANY_TARGET_HEX) {
        enemyHex += armyPointer->m_facing == ARMY_FACING_RIGHT ? 1 : -1;
        directionMask = armyPointer->m_facing == ARMY_FACING_RIGHT ? BIT(COMBAT_DIRECTION_WEST)
                                                              : BIT(COMBAT_DIRECTION_EAST);
    }
    while (directionMask != ALL_ADJACENT_DIRECTIONS) {
        direction = GetBestDirection(enemyHex, m_hex, directionMask);
        adjHex = GetAdjacentCellIndex(enemyHex, direction);
        if (ValidHex(adjHex)
            && CanFit(adjHex, 1 - IDX(pathMode), &freeHex)) {
            m_moveTargetHex = freeHex;
            if (!HAS(m_monster.attributes, MONSTER_FLAGS_WIDE)) {
                m_attackDirection = OppositeDirection(direction);
            } else {
                attackMask = ~GetAttackMask(
                    m_moveTargetHex, ARMY_ATTACK_TARGET_ASSIGNED, ARMY_HEX_INVALID
                );
                for (currentDirection = COMBAT_DIRECTION_NORTHEAST; IDX(currentDirection) < ARMY_COMBAT_DIRECTION_COUNT; currentDirection++) {
                    if (attackMask & BIT(currentDirection)) {
                        m_attackDirection = currentDirection;
                    }
                }
            }
            return 1;
        } else {
            directionMask |= 1 << IDX(direction);
        }
    }

    if (HAS(armyPointer->m_monster.attributes, MONSTER_FLAGS_WIDE)
        && pathMode == ARMY_PATH_ANY_TARGET_HEX) {
        enemyHex += armyPointer->m_facing == ARMY_FACING_RIGHT ? -1 : 1;
        directionMask = armyPointer->m_facing == ARMY_FACING_RIGHT ? BIT(COMBAT_DIRECTION_EAST)
                                                           : BIT(COMBAT_DIRECTION_WEST);
        while (directionMask != ALL_ADJACENT_DIRECTIONS) {
            direction = GetBestDirection(enemyHex, m_hex, directionMask);
            adjHex = GetAdjacentCellIndex(enemyHex, direction);
            if (ValidHex(adjHex) && CanFit(adjHex, 0, NULL)) {
                m_moveTargetHex = adjHex;
                m_attackDirection = GetBestDirection(m_moveTargetHex, enemyHex, 0);
                return 1;
            } else {
                directionMask |= 1 << IDX(direction);
            }
        }
    }
    return 0;
}
#if H2_RETAIL_COMPILER
#undef armyPointer
#undef currentDirection
#undef moveDirection
#endif

VA(0x0044b5b4, 0x1a)
i32 army::FlyTo(void) {
    return FlyTo(m_moveTargetHex);
}

#if H2_RETAIL_COMPILER
#define columnDelta columnDelta1
#define dead dead2
#define frameCount frameCount0
#define oldMaxX oldMaxX0
#define padding pad9
#define sourceRearHex srcRearHex0
#define stepCount stepCount1
#define toColumn toColumn1
#define xPosition xPos
#define yPosition yPos
#define yRate yRate0
#define ySpan ySpan0
#endif
VA(0x0044b5ce, 0x9e2)
i32 army::FlyTo(i32 destination) {
    float xPosition;
    float yRate;
    float yPosition;
    i32 endX;
    i32 columnDelta;
    i32 fromX;
    i32 xDistance;
    i32 toColumn;
    i32 H2_UNUSED(padding);
    i32 H2_UNUSED(dead);
    i32 stepCount;
    i32 endY;
    i32 leg;
    i32 sourceY;
    i32 ySpan;
    float xSpeed;
    i32 column;
    i32 frameCount;
    i32 sourceRearHex;
    i32 H2_UNUSED(slack);
    i32 length;
    i32 lastMinX;
    i32 oldMaxX;
    i32 oldMinY;
    i32 midCount;
    i32 frameStart;
    i32 endRearHex;
    i32 oldMaxY;

    if (!ValidHex(destination)) {
        return 0;
    }

    column = m_hex % ARMY_HEX_COLUMNS;
    toColumn = destination % ARMY_HEX_COLUMNS;
    columnDelta = toColumn - column;
    m_facingChanged = false;
    if (columnDelta > 0 && m_facing == ARMY_FACING_LEFT) {
        m_facingChanged = true;
        m_facing = OppositeArmyFacing(m_facing);
        if (HAS(m_monster.attributes, MONSTER_FLAGS_WIDE)) {
            m_hex--;
            destination--;
        }
    } else if (columnDelta < 0 && m_facing == ARMY_FACING_RIGHT) {
        m_facingChanged = true;
        m_facing = OppositeArmyFacing(m_facing);
        if (HAS(m_monster.attributes, MONSTER_FLAGS_WIDE)) {
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
    xPosition = fromX;
    yPosition = sourceY;
    xDistance = endX - fromX;
    ySpan = endY - sourceY;
    length = INTEGER_VECTOR_LENGTH(xDistance, ySpan);
    stepCount = 0;
    if (m_frameInfo.flightSpeed > 0) {
        stepCount = (length + (m_frameInfo.flightSpeed >> 1)) / m_frameInfo.flightSpeed;
    }
    if (stepCount <= 0) {
        stepCount = 1;
    }
    /* The parenthesised divisor casts are load-bearing: without them VC6 folds the
       divisor into a single `fidiv` instead of pushing both operands onto the x87
       stack. */
    xSpeed = static_cast<float>(xDistance) / (static_cast<float>(stepCount));
    yRate = static_cast<float>(ySpan) / (static_cast<float>(stepCount));

    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = COMBAT_SIDE_NONE;
    gpCombatManager->m_hexCells[m_hex].m_occupantFrame = ARMY_FACING_NONE;
    if (HAS(m_monster.attributes, MONSTER_FLAGS_WIDE)) {
        sourceRearHex = m_hex + (m_facing == ARMY_FACING_LEFT ? -1 : 1);
        gpCombatManager->m_hexCells[sourceRearHex].m_occupantIndex = -1;
        gpCombatManager->m_hexCells[sourceRearHex].m_occupantSide = COMBAT_SIDE_NONE;
        gpCombatManager->m_hexCells[sourceRearHex].m_occupantFrame = ARMY_FACING_NONE;
    }

    if (!gbNoShowCombat) {
        frameCount = 0;
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
        for (leg = 0; leg < stepCount; leg++) {
            BuildTempWalkSeq(
                &m_frameInfo,
                leg + 1 == stepCount,
                leg > 0
            );
            if (stepCount == 0) {
                frameCount = m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)];
                frameStart = 0;
            } else {
                if (leg > 0) {
                    frameCount +=
                        m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK_BEGIN_STANDING)];
                    frameStart = 0;
                } else {
                    frameStart = m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK_BEGIN)];
                }
                frameCount = m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK_MIDDLE)];
                midCount = frameCount;
                if (leg + 1 < stepCount) {
                    frameCount += m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK_END)];
                }
            }

            for (m_animationFrame = 0;
                 m_animationFrame < m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)];
                 m_animationFrame++) {
                if (m_animationFrame >= frameStart
                    && m_animationFrame < frameStart + frameCount) {
                    xPosition += xSpeed / frameCount;
                    yPosition += yRate / frameCount;
                }
                if (m_animationFrame % m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)]
                    == FLIGHT_SOUND_FRAME) {
                    if (IS_VAMPIRE_CREATURE(m_monsterType) && leg == 0) {
                        gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_EXTRA_ONE)]);
                        DelayMilli(VAMPIRE_FLIGHT_SOUND_DELAY);
                    } else if (IS_VAMPIRE_CREATURE(m_monsterType) && leg == stepCount - 1) {
                        gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_EXTRA_TWO)]);
                    } else {
                        gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_MOVE)]);
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
                    oldMaxX = giMaxExtentX;
                    oldMaxY = giMaxExtentY;
                } else {
                    lastMinX = 0;
                    oldMinY = 0;
                    oldMaxX = ARMY_COMBAT_MAX_X;
                    oldMaxY = ARMY_COMBAT_MAX_Y;
                }
                giMinExtentY = ARMY_COMBAT_WIDTH;
                giMinExtentX = giMinExtentY;
                giMaxExtentY = 0;
                giMaxExtentX = giMaxExtentY;
                gbComputeExtent = true;
                gbSaveBiggestExtent = true;
                DrawToBuffer(static_cast<i32>(xPosition), static_cast<i32>(yPosition), 0);
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
                if (giMaxExtentX > oldMaxX)
                    oldMaxX = giMaxExtentX;
                if (giMaxExtentY > oldMaxY)
                    oldMaxY = giMaxExtentY;

                DelayTil(glTimers);
                if (m_animationFrame < frameStart
                    || (m_animationFrame + 1 >= midCount && IS_VAMPIRE_CREATURE(m_monsterType))) {
                    glTimers[0] = static_cast<i32>(
                        KBTickCount()
                        + m_frameInfo.walkDuration * ARMY_VAMPIRE_FLIGHT_DURATION_SCALE
                              * gfCombatSpeedMod[gConfig.combatSpeed] / frameCount
                    );
                } else {
                    glTimers[0] = static_cast<i32>(
                        KBTickCount()
                        + m_frameInfo.walkDuration * gfCombatSpeedMod[gConfig.combatSpeed]
                              / frameCount
                    );
                }
                UPDATE_INCLUSIVE_REGION(lastMinX, oldMinY, oldMaxX, oldMaxY);
                if (m_animationFrame
                    == m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)] - 1) {
                    xPosition = fromX + (leg + 1) * xSpeed;
                    yPosition = sourceY + (leg + 1) * yRate;
                }
            }
        }
    }

    CancelSpellType(ARMY_CANCEL_SPELLS_AFTER_MOVE);
    gpCombatManager->m_hexCells[destination].m_occupantSide =
        gpCombatManager->m_currentArmySide;
    gpCombatManager->m_hexCells[destination].m_occupantIndex =
        gpCombatManager->m_currentArmyIndex;
    gpCombatManager->m_hexCells[destination].m_occupantFrame = ARMY_FACING_NONE;
    if (HAS(m_monster.attributes, MONSTER_FLAGS_WIDE)) {
        endRearHex = destination + (m_facing == ARMY_FACING_LEFT ? -1 : 1);
        gpCombatManager->m_hexCells[endRearHex].m_occupantSide =
            gpCombatManager->m_currentArmySide;
        gpCombatManager->m_hexCells[endRearHex].m_occupantIndex =
            gpCombatManager->m_currentArmyIndex;
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
        if (HAS(m_monster.attributes, MONSTER_FLAGS_WIDE)) {
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
#if H2_RETAIL_COMPILER
#undef columnDelta
#undef dead
#undef frameCount
#undef oldMaxX
#undef padding
#undef sourceRearHex
#undef stepCount
#undef toColumn
#undef xPosition
#undef yPosition
#undef yRate
#undef ySpan
#endif
