#include <Ints.h>
#include <SOURCE/KB_TYPES.h>
#include <math.h>
#include <algorithm>
#include <set>
#include <vector>
#include <IRONFIST/creatures.h>
#include <IRONFIST/expansions.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/soundManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/army.h>
#include <BASE/Misc.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/KB.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/PATH.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/display.h>
#include <SOURCE/combatTypes.h>

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
    hexcell* cell;
    i32 candidateHex;

    candidateHex = hex;
    cell = NULL;
    if (fittingHex) {
        *fittingHex = hex;
    }
    if (!ValidHex(candidateHex) || candidateHex % COMBAT_GRID_ROW_LENGTH == 0
        || candidateHex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1) {
        return 0;
    }
    if (gpCombatManager->m_hexCells[candidateHex].m_occupantSide != COMBAT_SIDE_NONE
        || gpCombatManager->m_hexCells[candidateHex].m_blocked) {
        return 0;
    }
    if ((H2EnumIndex((m_monster.attributes) & (MONSTER_FLAGS_WIDE)))) {
        candidateHex = GetAdjacentCellIndex(
            hex,
            m_facing == ARMY_FACING_RIGHT ? COMBAT_DIRECTION_EAST : COMBAT_DIRECTION_WEST
        );
        if (ValidHex(candidateHex) && candidateHex % COMBAT_GRID_ROW_LENGTH != 0
            && candidateHex % COMBAT_GRID_ROW_LENGTH != COMBAT_GRID_ROW_LENGTH - 1) {
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

i32 army::ValidFlight(i32 destination, ArmyPathTarget pathMode) {
    i32 enemyHex;
    u32 directionMask;
    i32 adjHex;

    CombatHexDirection direction;

    army* armyPointer;

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

    armyPointer = &gpCombatManager->m_armies[H2EnumIndex(m_targetSide)][m_targetIndex];
    if (pathMode != ARMY_PATH_ANY_TARGET_HEX) {
        enemyHex = destination;
    } else {
        enemyHex = armyPointer->m_hex;
    }
    if (!ValidHex(enemyHex)) {
        return 0;
    }

    attackMask = GetAttackMask(m_hex, ARMY_ATTACK_TARGET_ASSIGNED, ARMY_HEX_INVALID);
    while (attackMask != COMBAT_ALL_DIRECTIONS_BLOCKED) {
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
            attackMask |= 1 << H2EnumIndex(moveDirection);
        }
    }

    directionMask = 0;
    if ((H2EnumIndex((armyPointer->m_monster.attributes) & (MONSTER_FLAGS_WIDE)))
        && pathMode == ARMY_PATH_ANY_TARGET_HEX) {
        enemyHex += armyPointer->m_facing == ARMY_FACING_RIGHT ? 1 : -1;
        directionMask = armyPointer->m_facing == ARMY_FACING_RIGHT ? (1 << H2EnumIndex(COMBAT_DIRECTION_WEST))
                                                              : (1 << H2EnumIndex(COMBAT_DIRECTION_EAST));
    }
    while (directionMask != ALL_ADJACENT_DIRECTIONS) {
        direction = GetBestDirection(enemyHex, m_hex, directionMask);
        adjHex = GetAdjacentCellIndex(enemyHex, direction);
        if (ValidHex(adjHex)
            && CanFit(adjHex, 1 - H2EnumIndex(pathMode), &freeHex)) {
            m_moveTargetHex = freeHex;
            if (!(H2EnumIndex((m_monster.attributes) & (MONSTER_FLAGS_WIDE)))) {
                m_attackDirection = OppositeDirection(direction);
            } else {
                attackMask = ~GetAttackMask(
                    m_moveTargetHex, ARMY_ATTACK_TARGET_ASSIGNED, ARMY_HEX_INVALID
                );
                for (currentDirection = COMBAT_DIRECTION_NORTHEAST; H2EnumIndex(currentDirection) < H2EnumIndex(COMBAT_DIRECTION_COUNT); currentDirection++) {
                    if (attackMask & (1 << H2EnumIndex(currentDirection))) {
                        m_attackDirection = currentDirection;
                    }
                }
            }
            return 1;
        } else {
            directionMask |= 1 << H2EnumIndex(direction);
        }
    }

    if ((H2EnumIndex((armyPointer->m_monster.attributes) & (MONSTER_FLAGS_WIDE)))
        && pathMode == ARMY_PATH_ANY_TARGET_HEX) {
        enemyHex += armyPointer->m_facing == ARMY_FACING_RIGHT ? -1 : 1;
        directionMask = armyPointer->m_facing == ARMY_FACING_RIGHT ? (1 << H2EnumIndex(COMBAT_DIRECTION_EAST))
                                                           : (1 << H2EnumIndex(COMBAT_DIRECTION_WEST));
        while (directionMask != ALL_ADJACENT_DIRECTIONS) {
            direction = GetBestDirection(enemyHex, m_hex, directionMask);
            adjHex = GetAdjacentCellIndex(enemyHex, direction);
            if (ValidHex(adjHex) && CanFit(adjHex, 0, NULL)) {
                m_moveTargetHex = adjHex;
                m_attackDirection = GetBestDirection(m_moveTargetHex, enemyHex, 0);
                return 1;
            } else {
                directionMask |= 1 << H2EnumIndex(direction);
            }
        }
    }
    return 0;
}

i32 army::FlyTo(void) {
    return FlyTo(m_moveTargetHex);
}

// The Berserker's parabolic hop between two hexes, drawn frame by frame
// over a saved copy of the battlefield.
void army::ArcJump(i32 fromHex, i32 toHex) {
    bool firingLeft = true;
    float fromX = static_cast<float>(gpCombatManager->m_hexCells[fromHex].m_x);
    float fromY = static_cast<float>(gpCombatManager->m_hexCells[fromHex].m_gridBottom);
    float targX = static_cast<float>(gpCombatManager->m_hexCells[toHex].m_x);
    float targY = static_cast<float>(gpCombatManager->m_hexCells[toHex].m_gridBottom);
    if (fromX > targX) {
        m_facing = ARMY_FACING_LEFT;
        firingLeft = false;
    } else {
        m_facing = ARMY_FACING_RIGHT;
    }

    gpCombatManager->m_hexCells[fromHex].m_occupantIndex = -1;
    gpCombatManager->m_hexCells[fromHex].m_occupantSide = COMBAT_SIDE_NONE;
    gpCombatManager->m_hexCells[fromHex].m_occupantFrame = ARMY_FACING_NONE;
    gpCombatManager->DrawFrame(0, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);

    bitmap* savedScreen =
        new bitmap(BITMAP_TYPE_NONE, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT);
    gpWindowManager->m_screen->CopyTo(
        savedScreen, 0, 0, 0, 0, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT
    );

    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)][0] = 31;
    m_animationFrame = 0;
    m_animationSequence = ARMY_ANIMATION_WALK_MIDDLE;

    std::vector<Point> points =
        MakeCatapultArc(COMBAT_ARC_FRAME_COUNT, firingLeft, fromX, fromY, targX, targY);
    for (i32 pointIndex = 0; pointIndex < static_cast<i32>(points.size()); pointIndex++) {
        if (pointIndex == 5) {
            m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)][0] = 32;
        } else if (pointIndex == 12) {
            m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)][0] = 33;
        }
        savedScreen->CopyTo(
            gpWindowManager->m_screen, 0, 0, 0, 0, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT
        );
        DrawToBuffer(points[pointIndex].x, points[pointIndex].y, 0);
        gpCombatManager->DrawFrame(1, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 0, 1);
        gpWindowManager->UpdateScreenRegion(0, 0, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT);
        glTimers[0] = static_cast<i32>(
            platform::Ticks()
            + H2EnumIndex(COMBAT_ARC_FRAME_DURATION) * gfCombatSpeedMod[gConfig.combatSpeed]
        );
        DelayTil(glTimers);
    }

    m_hex = toHex;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = m_side;
    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = static_cast<i8>(m_index);
    gpCombatManager->m_hexCells[m_hex].m_occupantFrame = ARMY_FACING_NONE;

    savedScreen->CopyTo(
        gpWindowManager->m_screen, 0, 0, 0, 0, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT
    );
    giMinExtentY = giMinExtentX = giMaxExtentY = giMaxExtentX = 0;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)][0] = 1;

    delete savedScreen;
}

// Whether the hex borders this stack's current position (either hex of a
// wide creature).
bool army::IsCloseMove(i32 destination) {
    for (i32 direction = 0; direction < COMBAT_DIRECTION_ADJACENT_COUNT; direction++) {
        if ((H2EnumIndex((m_monster.attributes) & (MONSTER_FLAGS_WIDE)))
            && GetAdjacentCellIndex(m_hex + 1, CombatHexDirection(direction)) == destination) {
            return true;
        }
        if (GetAdjacentCellIndex(m_hex, CombatHexDirection(direction)) == destination) {
            return true;
        }
    }
    return false;
}

i32 army::FlyTo(i32 destination) {
    std::vector<i32> chargeAffectedHexes;
    float xPosition;
    float yRate;
    float yPosition;
    i32 endX;
    i32 columnDelta;
    i32 fromX;
    i32 xDistance;
    i32 toColumn;

    i32 stepCount;
    i32 endY;
    i32 leg;
    i32 sourceY;
    i32 ySpan;
    float xSpeed;
    i32 column;
    i32 frameCount;
    i32 sourceRearHex;

    i32 length;
    i32 lastMinX;
    i32 oldMaxX;
    i32 oldMinY;
    i32 midCount;
    i32 frameStart;
    i32 endRearHex;
    i32 oldMaxY;

    gCloseMove = IsCloseMove(destination);
    if (!ValidHex(destination)) {
        return 0;
    }

    column = m_hex % COMBAT_GRID_ROW_LENGTH;
    toColumn = destination % COMBAT_GRID_ROW_LENGTH;
    columnDelta = toColumn - column;
    m_facingChanged = false;
    if (columnDelta > 0 && m_facing == ARMY_FACING_LEFT) {
        m_facingChanged = true;
        m_facing = OppositeArmyFacing(m_facing);
        if ((H2EnumIndex((m_monster.attributes) & (MONSTER_FLAGS_WIDE)))) {
            m_hex--;
            destination--;
        }
    } else if (columnDelta < 0 && m_facing == ARMY_FACING_RIGHT) {
        m_facingChanged = true;
        m_facing = OppositeArmyFacing(m_facing);
        if ((H2EnumIndex((m_monster.attributes) & (MONSTER_FLAGS_WIDE)))) {
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

    xSpeed = static_cast<float>(xDistance) / (static_cast<float>(stepCount));
    yRate = static_cast<float>(ySpan) / (static_cast<float>(stepCount));

    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = COMBAT_SIDE_NONE;
    gpCombatManager->m_hexCells[m_hex].m_occupantFrame = ARMY_FACING_NONE;
    if ((H2EnumIndex((m_monster.attributes) & (MONSTER_FLAGS_WIDE)))) {
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
            LOGICAL_SCREEN_WIDTH,
            COMBAT_MAX_EXTENT_Y
        );
        gpCombatManager->m_backgroundDrawn = false;
        m_animationSequence = ARMY_ANIMATION_WALK;
        for (leg = 0; leg < stepCount; leg++) {
            if (CreatureHasAttribute(H2EnumIndex(m_monsterType), TELEPORTER)) {
                BuildTeleporterTempWalkSeq(
                    &m_frameInfo,
                    leg + 1 == stepCount,
                    leg > 0,
                    gCloseMove
                );
            } else {
                BuildTempWalkSeq(&m_frameInfo, leg + 1 == stepCount, leg > 0);
                if (CreatureHasAttribute(H2EnumIndex(m_monsterType), CHARGER)) {
                    gCharging = true;
                    ChargingDirection chargeDirection = CHARGING_FORWARD;
                    double chargeAngle =
                        (180.0 / M_PI) * atan2(static_cast<double>(ySpan), abs(xDistance));
                    if (chargeAngle > 45) {
                        chargeDirection = CHARGING_DOWN;
                    } else if (chargeAngle < -45) {
                        chargeDirection = CHARGING_UP;
                    }
                    SetChargingMoveAnimation(chargeDirection);
                }
            }
            if (stepCount == 0) {
                frameCount = m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)];
                frameStart = 0;
            } else {
                if (leg > 0) {
                    frameCount +=
                        m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN_STANDING)];
                    frameStart = 0;
                } else {
                    frameStart = m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN)];
                }
                frameCount = m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)];
                midCount = frameCount;
                if (leg + 1 < stepCount) {
                    frameCount += m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_END)];
                }
            }

            for (m_animationFrame = 0;
                 m_animationFrame < m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)];
                 m_animationFrame++) {
                if (m_animationFrame >= frameStart
                    && m_animationFrame < frameStart + frameCount) {
                    // A far teleport snaps straight to the destination.
                    if (CreatureHasAttribute(H2EnumIndex(m_monsterType), TELEPORTER)
                        && !gCloseMove) {
                        xPosition = static_cast<float>(endX);
                        yPosition = static_cast<float>(endY);
                    } else {
                        xPosition += xSpeed / frameCount;
                        yPosition += yRate / frameCount;
                    }
                }
                if (m_animationFrame % m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]
                    == FLIGHT_SOUND_FRAME) {
                    if (IS_VAMPIRE_CREATURE(m_monsterType) && leg == 0) {
                        gpSoundManager->MemorySample(m_samples[H2EnumIndex(ARMY_SAMPLE_EXTRA_ONE)]);
                        DelayMilli(VAMPIRE_FLIGHT_SOUND_DELAY);
                    } else if (IS_VAMPIRE_CREATURE(m_monsterType) && leg == stepCount - 1) {
                        gpSoundManager->MemorySample(m_samples[H2EnumIndex(ARMY_SAMPLE_EXTRA_TWO)]);
                    } else {
                        gpSoundManager->MemorySample(m_samples[H2EnumIndex(ARMY_SAMPLE_MOVE)]);
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
                    oldMaxX = LOGICAL_SCREEN_MAX_X;
                    oldMaxY = COMBAT_MAX_EXTENT_Y;
                }
                giMinExtentY = LOGICAL_SCREEN_WIDTH;
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
                if (giMaxExtentX > LOGICAL_SCREEN_MAX_X)
                    giMaxExtentX = LOGICAL_SCREEN_MAX_X;
                if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
                    giMaxExtentY = COMBAT_MAX_EXTENT_Y;
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
                        platform::Ticks()
                        + m_frameInfo.walkDuration * ARMY_VAMPIRE_FLIGHT_DURATION_SCALE
                              * gfCombatSpeedMod[gConfig.combatSpeed] / frameCount
                    );
                } else {
                    glTimers[0] = static_cast<i32>(
                        platform::Ticks()
                        + m_frameInfo.walkDuration * gfCombatSpeedMod[gConfig.combatSpeed]
                              / frameCount
                    );
                }
                UPDATE_INCLUSIVE_REGION(lastMinX, oldMinY, oldMaxX, oldMaxY);
                if (m_animationFrame
                    == m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)] - 1) {
                    xPosition = fromX + (leg + 1) * xSpeed;
                    yPosition = sourceY + (leg + 1) * yRate;
                }
                // The charger hurts everything it flies over.
                i32 flownHex = gpCombatManager->GetGridIndex(
                    static_cast<i32>(xPosition),
                    static_cast<i32>(yPosition) - ARMY_CHARGE_SPRITE_OFFSET
                );
                if (IsEnemyCreatureHex(flownHex)) {
                    chargeAffectedHexes.push_back(flownHex);
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
    if ((H2EnumIndex((m_monster.attributes) & (MONSTER_FLAGS_WIDE)))) {
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
        if ((H2EnumIndex((m_monster.attributes) & (MONSTER_FLAGS_WIDE)))) {
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
    if (CreatureHasAttribute(H2EnumIndex(m_monsterType), CHARGER)) {
        std::set<i32> uniqueHexes(chargeAffectedHexes.begin(), chargeAffectedHexes.end());
        chargeAffectedHexes.assign(uniqueHexes.begin(), uniqueHexes.end());
        // The landing target takes the full hit, not the path damage.
        auto landingHex = std::find(
            chargeAffectedHexes.begin(), chargeAffectedHexes.end(), giNextActionGridIndex
        );
        if (landingHex != chargeAffectedHexes.end()) {
            chargeAffectedHexes.erase(landingHex);
        }
        ChargingDamage(chargeAffectedHexes);
    }
    gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    if (CreatureHasAttribute(H2EnumIndex(m_monsterType), CHARGER)) {
        RevertChargingMoveAnimation();
    }
    gpCombatManager->TestRaiseDoor();
    gpCombatManager->CheckBurnCreature(this);
    return 1;
}
