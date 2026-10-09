#include <H2/Ints.h>
#include <SOURCE/army.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/PATH.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/kbTypes.h>
#include <SOURCE/combatTypes.h>

typedef enum CombatPathConstant {
    WIDE_DIRECTIONS_MASK = 0xc0,
    IGNORE_SPEED           = 99,
    WIDE_HEX_OFFSET        = 1
} CombatPathConstant;

i32 army::FindPath(
    i32 sourceHex,
    i32 targetHex,
    i32,
    b32 ignoreSpeed,
    ArmyPathTarget pathMode
) {
    i32 pathResult;
    i32 savedSpeed;

    if (!ValidHex(sourceHex) || !ValidHex(targetHex))
        return 0;

    savedSpeed = m_monster.speed;
    if (ignoreSpeed)
        m_monster.speed = IGNORE_SPEED;

    pathResult = gpSearchArray->FindCombatPath(sourceHex, targetHex, this, pathMode, false);
    if (!pathResult && (((m_monster.attributes) & (MONSTER_FLAGS_WIDE)))
        && pathMode == ARMY_PATH_ANY_TARGET_HEX) {
        switch (m_facing) {
            case ARMY_FACING_LEFT:
                targetHex = GetAdjacentCellIndex(targetHex, COMBAT_DIRECTION_EAST);
                break;
            case ARMY_FACING_RIGHT:
                targetHex = GetAdjacentCellIndex(targetHex, COMBAT_DIRECTION_WEST);
                break;
        }

        if (!ValidHex(targetHex))
            pathResult = 0;
        else
            pathResult = gpSearchArray->FindCombatPath(sourceHex, targetHex, this, pathMode, true);
    }

    m_monster.speed = savedSpeed;
    return pathResult;
}

b32 army::ValidPath(i32 targetHex, ArmyPathTarget pathMode) {
    i32 pathResult;
    i32 extra [[maybe_unused]];

    if (!ValidHex(targetHex))
        return false;

    if ((((m_monster.attributes) & (MONSTER_FLAGS_FLYING))))
        return ValidFlight(targetHex, pathMode);

    pathResult = FindPath(m_hex, targetHex, m_monster.speed, false, pathMode);
    if (pathResult) {
        m_moveTargetHex = targetHex;
        return true;
    }
    return false;
}

i32 army::GetMoveMask(i32 sourceHex) {
    i32 blockedMaskValue = 0;
    i32 directionBitFlag = 1;
    CombatHexDirection directionResult;

    for (directionResult = COMBAT_DIRECTION_NORTHEAST;
         directionResult <= COMBAT_DIRECTION_NORTHWEST;
         directionResult++) {
        if (!ValidMove(sourceHex, directionResult))
            blockedMaskValue |= directionBitFlag;
        directionBitFlag <<= 1;
    }
    return blockedMaskValue | (1 << (COMBAT_DIRECTION_WIDE_NORTH))
         | (1 << (COMBAT_DIRECTION_WIDE_SOUTH));
}

i32 army::GetAttackMask(i32 sourceHex, ArmyAttackTarget targetMode, i32 targetHex) {
    CombatHexDirection directionResult;
    i32 blockedMaskValue;
    i32 directionBitFlag;
    i32 directionCountNext;
    i32 attackHexNext;

    blockedMaskValue =
        (((m_monster.attributes) & (MONSTER_FLAGS_WIDE))) ? 0 : WIDE_DIRECTIONS_MASK;
    directionBitFlag = 1;
    directionCountNext = (((m_monster.attributes) & (MONSTER_FLAGS_WIDE)))
                             ? COMBAT_DIRECTION_COUNT
                             : COMBAT_DIRECTION_ADJACENT_COUNT;

    for (directionResult = COMBAT_DIRECTION_NORTHEAST;
         (directionResult) < directionCountNext;
         directionResult++) {
        if (!ValidAttack(
                sourceHex, directionResult, targetMode, targetHex, &attackHexNext
            ))
            blockedMaskValue |= directionBitFlag;
        directionBitFlag <<= 1;
    }
    return blockedMaskValue;
}

b32 army::ValidMove(CombatHexDirection direction) {
    return ValidMove(m_hex, direction);
}

b32 army::ValidMove(i32 sourceHex, CombatHexDirection direction) {
    i32 destinationHexNext;
    i32 rearSquare;
    b32 frontValid;
    b32 rearValidResult;

    if (!ValidHex(sourceHex))
        return false;

    destinationHexNext = GetAdjacentCellIndex(sourceHex, direction);
    if (!ValidHex(destinationHexNext))
        return false;

    frontValid = false;
    if (gpCombatManager->m_hexCells[destinationHexNext].m_occupantSide == COMBAT_SIDE_NONE
        && (!gpCombatManager->m_hexCells[destinationHexNext].m_blocked
            || CAN_PASS_CASTLE_GATE(destinationHexNext))) {
        frontValid = true;
    }

    if ((((m_monster.attributes) & (MONSTER_FLAGS_WIDE)))) {
        rearSquare = ARMY_HEX_INVALID;
        switch (m_facing) {
            case ARMY_FACING_LEFT:
                if (direction == COMBAT_DIRECTION_EAST)
                    return frontValid;
                else
                    rearSquare = GetAdjacentCellIndex(destinationHexNext, COMBAT_DIRECTION_WEST);
                break;
            case ARMY_FACING_RIGHT:
                if (direction == COMBAT_DIRECTION_WEST)
                    return frontValid;
                else
                    rearSquare = GetAdjacentCellIndex(destinationHexNext, COMBAT_DIRECTION_EAST);
                break;
        }

        rearValidResult = false;
        if (ValidHex(rearSquare)
            && gpCombatManager->m_hexCells[rearSquare].m_occupantSide == COMBAT_SIDE_NONE
            && (!gpCombatManager->m_hexCells[rearSquare].m_blocked
                || CAN_PASS_CASTLE_GATE(rearSquare))) {
            rearValidResult = true;
        }

        if (direction == COMBAT_DIRECTION_EAST || direction == COMBAT_DIRECTION_WEST)
            return rearValidResult;
        else {
            if (frontValid == true && rearValidResult == true)
                return true;
            else
                return false;
        }
    } else
        return frontValid;
}

b32 army::ValidAttack(
    i32 sourceHex,
    CombatHexDirection direction,
    ArmyAttackTarget targetMode,
    i32 requiredTargetHex,
    i32* attackHex
) {
    i32 adjacentSourceHex;
    CombatSide occupantSide;

    if (!ValidHex(sourceHex))
        return false;

    adjacentSourceHex = sourceHex;
    if ((((m_monster.attributes) & (MONSTER_FLAGS_WIDE)))) {
        if (direction == COMBAT_DIRECTION_WIDE_NORTH) {
            *attackHex = GetAdjacentCellIndex(
                sourceHex,
                m_facing == ARMY_FACING_LEFT ? COMBAT_DIRECTION_NORTHWEST
                                             : COMBAT_DIRECTION_NORTHEAST
            );
        } else if (direction == COMBAT_DIRECTION_WIDE_SOUTH) {
            *attackHex = GetAdjacentCellIndex(
                sourceHex,
                m_facing == ARMY_FACING_LEFT ? COMBAT_DIRECTION_SOUTHWEST
                                             : COMBAT_DIRECTION_SOUTHEAST
            );
        } else {
            switch (m_facing) {
                case ARMY_FACING_LEFT:
                    if (direction >= COMBAT_DIRECTION_SOUTHWEST)
                        adjacentSourceHex =
                            GetAdjacentCellIndex(sourceHex, COMBAT_DIRECTION_WEST);
                    break;
                case ARMY_FACING_RIGHT:
                    if (direction <= COMBAT_DIRECTION_SOUTHEAST)
                        adjacentSourceHex =
                            GetAdjacentCellIndex(sourceHex, COMBAT_DIRECTION_EAST);
                    break;
            }

            if (adjacentSourceHex == ARMY_HEX_INVALID)
                return false;
            *attackHex = GetAdjacentCellIndex(adjacentSourceHex, direction);
        }
    } else {
        *attackHex = GetAdjacentCellIndex(sourceHex, direction);
    }

    if (!ValidHex(*attackHex))
        return false;
    if (requiredTargetHex != ARMY_HEX_INVALID && *attackHex != requiredTargetHex)
        return false;

    occupantSide = gpCombatManager->m_hexCells[*attackHex].m_occupantSide;
    switch (targetMode) {
        case ARMY_ATTACK_TARGET_ASSIGNED:
            if (occupantSide == m_targetSide
                && gpCombatManager->m_hexCells[*attackHex].m_occupantIndex == m_targetIndex)
                return true;
            break;
        case ARMY_ATTACK_TARGET_ENEMY:
            if (occupantSide == OppositeCombatSide(gpCombatManager->m_currentSide))
                return true;
            break;
        case ARMY_ATTACK_TARGET_OCCUPIED:
            if (occupantSide != COMBAT_SIDE_NONE)
                return true;
            break;
    }
    return false;
}

i32 army::GetAdjacentCellIndex(i32 sourceHex, CombatHexDirection direction) {
    if (sourceHex == ARMY_HEX_INVALID)
        return ARMY_HEX_INVALID;

    if (direction == COMBAT_DIRECTION_WIDE_NORTH) {
        direction = m_facing == ARMY_FACING_RIGHT ? COMBAT_DIRECTION_NORTHWEST
                                                  : COMBAT_DIRECTION_NORTHEAST;
    } else if (direction == COMBAT_DIRECTION_WIDE_SOUTH) {
        direction = m_facing == ARMY_FACING_RIGHT ? COMBAT_DIRECTION_SOUTHWEST
                                                  : COMBAT_DIRECTION_SOUTHEAST;
    }

    return gpCombatManager->m_adjacency[sourceHex][(direction)];
}

i32 GetAdjacentCellIndexNoArmy(i32 sourceHex, CombatHexDirection direction) {
    if (sourceHex == ARMY_HEX_INVALID)
        return ARMY_HEX_INVALID;

    if (direction == COMBAT_DIRECTION_WIDE_NORTH)
        direction = COMBAT_DIRECTION_NORTHWEST;
    else if (direction == COMBAT_DIRECTION_WIDE_SOUTH)
        direction = COMBAT_DIRECTION_SOUTHWEST;
    return gpCombatManager->m_adjacency[sourceHex][(direction)];
}

b32 army::ValidRange(i32 targetHex) {
    i32 adjacentHex;
    CombatHexDirection directionResult;

    if (!ValidHex(targetHex))
        return false;

    m_moveTargetHex = m_hex;
    if (!(m_monster.attributes & MONSTER_FLAGS_WIDE)) {
        m_attackDirection = GetBestDirection(m_hex, targetHex, WIDE_DIRECTIONS_MASK);
        adjacentHex = GetAdjacentCellIndex(m_hex, m_attackDirection);
        if (adjacentHex == targetHex)
            return true;
        adjacentHex = GetAdjacentCellIndex(adjacentHex, m_attackDirection);
        if (adjacentHex == targetHex)
            return true;
    } else {
        switch (m_facing) {
            case ARMY_FACING_RIGHT:
                directionResult =
                    GetBestDirection(m_hex, targetHex, WIDE_DIRECTIONS_MASK);
                if (directionResult > COMBAT_DIRECTION_SOUTHEAST) {
                    m_attackDirection = directionResult;
                    adjacentHex = GetAdjacentCellIndex(m_hex, directionResult);
                    if (adjacentHex == targetHex)
                        return true;
                    adjacentHex = GetAdjacentCellIndex(adjacentHex, directionResult);
                    if (adjacentHex == targetHex)
                        return true;
                }

                directionResult =
                    GetBestDirection(m_hex + WIDE_HEX_OFFSET, targetHex, WIDE_DIRECTIONS_MASK);
                if (directionResult < COMBAT_DIRECTION_SOUTHWEST) {
                    m_attackDirection = directionResult;
                    adjacentHex = GetAdjacentCellIndex(m_hex + WIDE_HEX_OFFSET, directionResult);
                    if (adjacentHex == targetHex)
                        return true;
                    adjacentHex = GetAdjacentCellIndex(adjacentHex, directionResult);
                    if (adjacentHex == targetHex)
                        return true;
                }
                if (directionResult == COMBAT_DIRECTION_WEST)
                    return false;
                if (directionResult == COMBAT_DIRECTION_NORTHWEST)
                    m_attackDirection = COMBAT_DIRECTION_WIDE_NORTH;
                else if (directionResult == COMBAT_DIRECTION_SOUTHWEST)
                    m_attackDirection = COMBAT_DIRECTION_WIDE_SOUTH;

                adjacentHex = GetAdjacentCellIndex(m_hex + WIDE_HEX_OFFSET, directionResult);
                if (adjacentHex == targetHex)
                    return true;
                adjacentHex = GetAdjacentCellIndex(adjacentHex, directionResult);
                if (adjacentHex == targetHex)
                    return true;
                break;

            case ARMY_FACING_LEFT:
                directionResult =
                    GetBestDirection(m_hex, targetHex, WIDE_DIRECTIONS_MASK);
                if (directionResult < COMBAT_DIRECTION_SOUTHWEST) {
                    m_attackDirection = directionResult;
                    adjacentHex = GetAdjacentCellIndex(m_hex, directionResult);
                    if (adjacentHex == targetHex)
                        return true;
                    adjacentHex = GetAdjacentCellIndex(adjacentHex, directionResult);
                    if (adjacentHex == targetHex)
                        return true;
                    return false;
                }

                directionResult =
                    GetBestDirection(m_hex - WIDE_HEX_OFFSET, targetHex, WIDE_DIRECTIONS_MASK);
                if (directionResult > COMBAT_DIRECTION_SOUTHEAST) {
                    m_attackDirection = directionResult;
                    adjacentHex = GetAdjacentCellIndex(m_hex - WIDE_HEX_OFFSET, directionResult);
                    if (adjacentHex == targetHex)
                        return true;
                    adjacentHex = GetAdjacentCellIndex(adjacentHex, directionResult);
                    if (adjacentHex == targetHex)
                        return true;
                    return false;
                }
                if (directionResult == COMBAT_DIRECTION_EAST)
                    return false;
                if (directionResult == COMBAT_DIRECTION_NORTHEAST)
                    m_attackDirection = COMBAT_DIRECTION_WIDE_NORTH;
                else if (directionResult == COMBAT_DIRECTION_SOUTHEAST)
                    m_attackDirection = COMBAT_DIRECTION_WIDE_SOUTH;

                adjacentHex = GetAdjacentCellIndex(m_hex - WIDE_HEX_OFFSET, directionResult);
                if (adjacentHex == targetHex)
                    return true;
                adjacentHex = GetAdjacentCellIndex(adjacentHex, directionResult);
                if (adjacentHex == targetHex)
                    return true;
                break;
        }
    }
    return false;
}

CombatHexDirection OppositeDirection(CombatHexDirection direction) {
    if ((direction) < COMBAT_DIRECTION_ADJACENT_COUNT) {
        return (direction + COMBAT_DIRECTION_OPPOSITE_OFFSET)
               % COMBAT_DIRECTION_ADJACENT_COUNT;
    } else {
        if (direction == COMBAT_DIRECTION_WIDE_NORTH)
            return COMBAT_DIRECTION_WIDE_SOUTH;
        else
            return COMBAT_DIRECTION_WIDE_NORTH;
    }
}

CombatHexDirection army::GetBestDirection(i32 sourceHex, i32 targetHex, i32 blockedMask) {
    b32 isMovingDown;
    b32 leftFl;
    i32 sourceColumnCheck;
    i32 colTarget;
    i32 targetRowVal;
    b32 isMovingUp;
    i32 sourceRow;
    b32 isMovingRight;

    if (!ValidHex(sourceHex) || !ValidHex(targetHex))
        return COMBAT_DIRECTION_INVALID;

    sourceColumnCheck = sourceHex % COMBAT_GRID_ROW_LENGTH;
    sourceRow = sourceHex / COMBAT_GRID_ROW_LENGTH;
    colTarget = targetHex % COMBAT_GRID_ROW_LENGTH;
    targetRowVal = targetHex / COMBAT_GRID_ROW_LENGTH;
    isMovingUp = false;
    isMovingDown = false;
    leftFl = false;
    isMovingRight = false;

    if (colTarget > sourceColumnCheck)
        isMovingRight = true;
    else if (colTarget != sourceColumnCheck)
        leftFl = true;

    if (targetRowVal > sourceRow)
        isMovingDown = true;
    else if (targetRowVal != sourceRow)
        isMovingUp = true;

    if (isMovingRight == leftFl) {
        if (isMovingUp == true) {
            if (sourceRow & 1) {
                if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHWEST))))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHEAST))))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WEST))))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_EAST))))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHWEST))))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHEAST))))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_NORTH))))
                    return COMBAT_DIRECTION_WIDE_NORTH;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_SOUTH))))
                    return COMBAT_DIRECTION_WIDE_SOUTH;
            } else {
                if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHEAST))))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHWEST))))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_EAST))))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WEST))))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHEAST))))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHWEST))))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_NORTH))))
                    return COMBAT_DIRECTION_WIDE_NORTH;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_SOUTH))))
                    return COMBAT_DIRECTION_WIDE_SOUTH;
            }
        } else {
            if (sourceRow & 1) {
                if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHWEST))))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHEAST))))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WEST))))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_EAST))))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHWEST))))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHEAST))))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_SOUTH))))
                    return COMBAT_DIRECTION_WIDE_SOUTH;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_NORTH))))
                    return COMBAT_DIRECTION_WIDE_NORTH;
            } else {
                if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHEAST))))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHWEST))))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_EAST))))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WEST))))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHEAST))))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHWEST))))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_SOUTH))))
                    return COMBAT_DIRECTION_WIDE_SOUTH;
                else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_NORTH))))
                    return COMBAT_DIRECTION_WIDE_NORTH;
            }
        }
    }

    if (leftFl == true) {
        if (isMovingUp == true) {
            if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHWEST))))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WEST))))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHEAST))))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHWEST))))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_EAST))))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHEAST))))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_NORTH))))
                return COMBAT_DIRECTION_WIDE_NORTH;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_SOUTH))))
                return COMBAT_DIRECTION_WIDE_SOUTH;
        } else if (isMovingDown == true) {
            if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHWEST))))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WEST))))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHEAST))))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHWEST))))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_EAST))))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHEAST))))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_SOUTH))))
                return COMBAT_DIRECTION_WIDE_SOUTH;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_NORTH))))
                return COMBAT_DIRECTION_WIDE_NORTH;
        } else {
            if (!(blockedMask & (1 << (COMBAT_DIRECTION_WEST))))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHWEST))))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHWEST))))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHEAST))))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHEAST))))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_EAST))))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_SOUTH))))
                return COMBAT_DIRECTION_WIDE_SOUTH;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_NORTH))))
                return COMBAT_DIRECTION_WIDE_NORTH;
        }
    } else if (isMovingRight == true) {
        if (isMovingUp == true) {
            if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHEAST))))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_EAST))))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHWEST))))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHEAST))))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WEST))))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHWEST))))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_NORTH))))
                return COMBAT_DIRECTION_WIDE_NORTH;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_SOUTH))))
                return COMBAT_DIRECTION_WIDE_SOUTH;
        } else if (isMovingDown == true) {
            if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHEAST))))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_EAST))))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHWEST))))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHEAST))))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHWEST))))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WEST))))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_SOUTH))))
                return COMBAT_DIRECTION_WIDE_SOUTH;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_NORTH))))
                return COMBAT_DIRECTION_WIDE_NORTH;
        } else {
            if (!(blockedMask & (1 << (COMBAT_DIRECTION_EAST))))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHEAST))))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHEAST))))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_NORTHWEST))))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_SOUTHWEST))))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WEST))))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_SOUTH))))
                return COMBAT_DIRECTION_WIDE_SOUTH;
            else if (!(blockedMask & (1 << (COMBAT_DIRECTION_WIDE_NORTH))))
                return COMBAT_DIRECTION_WIDE_NORTH;
        }
    }
    return COMBAT_DIRECTION_INVALID;
}
