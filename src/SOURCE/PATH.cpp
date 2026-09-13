#include <Ints.h>
#include <set>
#include <vector>
#include <IRONFIST/creatures.h>
#include <IRONFIST/state.h>
#include <SOURCE/army.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/PATH.h>
#include <SOURCE/searchArray.h>


typedef enum CombatPathConstant {
    SPECIAL_DIRECTION_MASK = 0xc0,
    IGNORE_SPEED           = 99,
    WIDE_HEX_OFFSET        = 1
} CombatPathConstant;

// The keep's wall hexes, which even a jumper cannot ignore.
static bool IsCastleWall(i32 hexIndex) {
    static const std::set<i32> wallHexes = {9, 22, 34, 47, 59, 73, 86, 100, 113, 92};
    return wallHexes.count(hexIndex) != 0;
}

static bool IsAICombatTurn(void) {
    i32 currentPlayerId = gpCombatManager->m_playerId[H2EnumIndex(gpCombatManager->m_currentSide)];
    if (currentPlayerId == -1)
        return true;
    return !gbHumanPlayer[currentPlayerId];
}

i32 army::FindPath(
    i32 sourceHex,
    i32 targetHex,
    i32,
    i32 ignoreSpeed,
    ArmyPathTarget pathMode
) {
    i32 pathResult;
    i32 savedSpeed;
    std::vector<i32> obstacleHexes;

    if (!ValidHex(sourceHex) || !ValidHex(targetHex))
        return 0;

    // A human-controlled jumper paths as if the obstacles were not there.
    if (!IsAICombatTurn() && ironfist::HasCreatureAttribute(m_monsterType, ironfist::CreatureAttribute::Jumper)
        && ironfist::state::Get().combat.stack.abilityCounter[this][ironfist::CreatureAttribute::Jumper]) {
        for (i32 hexIndex = 0; hexIndex < COMBAT_HEX_COUNT; hexIndex++) {
            if (gpCombatManager->m_hexCells[hexIndex].m_blocked && !IsCastleWall(hexIndex)) {
                obstacleHexes.push_back(hexIndex);
                gpCombatManager->m_hexCells[hexIndex].m_blocked = 0;
            }
        }
    }

    savedSpeed = m_monster.speed;
    if (ignoreSpeed)
        m_monster.speed = IGNORE_SPEED;

    pathResult = gpSearchArray->FindCombatPath(sourceHex, targetHex, this, pathMode, 0);
    if (!pathResult && (H2EnumIndex((m_monster.attributes) & (MONSTER_ATTRIBUTE_WIDE)))
        && pathMode == ARMY_PATH_ANY_TARGET_HEX) {
        switch (m_facing) {
            case ARMY_FACING_LEFT:
                targetHex = GetAdjacentCellIndex(targetHex, COMBAT_DIRECTION_EAST);
                break;
            case ARMY_FACING_RIGHT:
                targetHex = GetAdjacentCellIndex(targetHex, COMBAT_DIRECTION_WEST);
                break;
            default:
                break;
        }

        if (!ValidHex(targetHex))
            pathResult = 0;
        else
            pathResult = gpSearchArray->FindCombatPath(sourceHex, targetHex, this, pathMode, 1);
    }

    m_monster.speed = static_cast<i8>(savedSpeed);
    for (i32 obstacleHex : obstacleHexes) {
        gpCombatManager->m_hexCells[obstacleHex].m_blocked = 1;
    }
    return pathResult;
}

i32 army::ValidPath(i32 targetHex, ArmyPathTarget pathMode) {
    i32 pathResult;

    if (!ValidHex(targetHex))
        return 0;

    if (H2EnumIndex((m_monster.attributes) & (MONSTER_ATTRIBUTE_FLYING)))
        return ValidFlight(targetHex, pathMode);

    pathResult = FindPath(m_hex, targetHex, m_monster.speed, 0, pathMode);
    if (pathResult) {
        m_moveTargetHex = targetHex;
        return 1;
    }
    return 0;
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
    return blockedMaskValue | (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))
         | (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST));
}

i32 army::GetAttackMask(i32 sourceHex, ArmyAttackTarget targetMode, i32 targetHex) {
    CombatHexDirection directionResult;
    i32 blockedMaskValue;
    i32 directionBitFlag;
    i32 directionCountNext;
    i32 attackHexNext;

    blockedMaskValue =
        (H2EnumIndex((m_monster.attributes) & (MONSTER_ATTRIBUTE_WIDE))) ? 0 : SPECIAL_DIRECTION_MASK;
    directionBitFlag = 1;
    directionCountNext = (H2EnumIndex((m_monster.attributes) & (MONSTER_ATTRIBUTE_WIDE)))
                             ? COMBAT_DIRECTION_COUNT
                             : COMBAT_DIRECTION_ADJACENT_COUNT;

    for (directionResult = COMBAT_DIRECTION_NORTHEAST;
         H2EnumIndex(directionResult) < directionCountNext;
         directionResult++) {
        if (!ValidAttack(
                sourceHex, directionResult, targetMode, targetHex, &attackHexNext
            ))
            blockedMaskValue |= directionBitFlag;
        directionBitFlag <<= 1;
    }
    return blockedMaskValue;
}

i32 army::ValidMove(CombatHexDirection direction) {
    return ValidMove(m_hex, direction);
}

i32 army::ValidMove(i32 sourceHex, CombatHexDirection direction) {
    i32 destinationHexNext;
    i32 rearSquare;
    b32 frontValid;
    b32 rearValidResult;

    if (!ValidHex(sourceHex))
        return 0;

    destinationHexNext = GetAdjacentCellIndex(sourceHex, direction);
    if (!ValidHex(destinationHexNext))
        return 0;

    frontValid = false;
    if (gpCombatManager->m_hexCells[destinationHexNext].m_occupantSide == COMBAT_SIDE_NONE
        && (!gpCombatManager->m_hexCells[destinationHexNext].m_blocked
            || CAN_PASS_CASTLE_GATE(destinationHexNext))) {
        frontValid = true;
    }

    if (H2EnumIndex((m_monster.attributes) & (MONSTER_ATTRIBUTE_WIDE))) {
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
            default:
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
            if (frontValid == 1 && rearValidResult == 1)
                return 1;
            else
                return 0;
        }
    } else
        return frontValid;
}

i32 army::ValidAttack(
    i32 sourceHex,
    CombatHexDirection direction,
    ArmyAttackTarget targetMode,
    i32 requiredTargetHex,
    i32* attackHex
) {
    i32 adjacentSourceHex;
    CombatSide occupantSide;

    if (!ValidHex(sourceHex))
        return 0;

    adjacentSourceHex = sourceHex;
    if (H2EnumIndex((m_monster.attributes) & (MONSTER_ATTRIBUTE_WIDE))) {
        if (direction == COMBAT_DIRECTION_WIDE_WEST) {
            *attackHex = GetAdjacentCellIndex(
                sourceHex,
                m_facing == ARMY_FACING_LEFT ? COMBAT_DIRECTION_NORTHWEST
                                             : COMBAT_DIRECTION_NORTHEAST
            );
        } else if (direction == COMBAT_DIRECTION_WIDE_EAST) {
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
                default:
                    break;
            }

            if (adjacentSourceHex == ARMY_HEX_INVALID)
                return 0;
            *attackHex = GetAdjacentCellIndex(adjacentSourceHex, direction);
        }
    } else {
        *attackHex = GetAdjacentCellIndex(sourceHex, direction);
    }

    if (!ValidHex(*attackHex))
        return 0;
    if (requiredTargetHex != ARMY_HEX_INVALID && *attackHex != requiredTargetHex)
        return 0;

    occupantSide = gpCombatManager->m_hexCells[*attackHex].m_occupantSide;
    switch (targetMode) {
        case ARMY_ATTACK_TARGET_ASSIGNED:
            if (occupantSide == m_targetSide
                && gpCombatManager->m_hexCells[*attackHex].m_occupantIndex == m_targetIndex)
                return 1;
            break;
        case ARMY_ATTACK_TARGET_ENEMY:
            if (occupantSide == OppositeCombatSide(gpCombatManager->m_currentSide))
                return 1;
            break;
        case ARMY_ATTACK_TARGET_OCCUPIED:
            if (occupantSide != COMBAT_SIDE_NONE)
                return 1;
            break;
    }
    return 0;
}

i32 army::GetAdjacentCellIndex(i32 sourceHex, CombatHexDirection direction) {
    if (sourceHex == ARMY_HEX_INVALID)
        return ARMY_HEX_INVALID;

    if (direction == COMBAT_DIRECTION_WIDE_WEST) {
        direction = m_facing == ARMY_FACING_RIGHT ? COMBAT_DIRECTION_NORTHWEST
                                                  : COMBAT_DIRECTION_NORTHEAST;
    } else if (direction == COMBAT_DIRECTION_WIDE_EAST) {
        direction = m_facing == ARMY_FACING_RIGHT ? COMBAT_DIRECTION_SOUTHWEST
                                                  : COMBAT_DIRECTION_SOUTHEAST;
    }

    return gpCombatManager->m_adjacency[sourceHex][H2EnumIndex(direction)];
}

i32 GetAdjacentCellIndexNoArmy(i32 sourceHex, CombatHexDirection direction) {
    if (sourceHex == ARMY_HEX_INVALID)
        return ARMY_HEX_INVALID;

    if (direction == COMBAT_DIRECTION_WIDE_WEST)
        direction = COMBAT_DIRECTION_NORTHWEST;
    else if (direction == COMBAT_DIRECTION_WIDE_EAST)
        direction = COMBAT_DIRECTION_SOUTHWEST;
    return gpCombatManager->m_adjacency[sourceHex][H2EnumIndex(direction)];
}

i32 army::ValidRange(i32 targetHex) {
    i32 adjacentHex;
    CombatHexDirection directionResult;

    if (!ValidHex(targetHex))
        return 0;

    m_moveTargetHex = m_hex;
    if (!(m_monster.attributes & MONSTER_ATTRIBUTE_WIDE)) {
        m_attackDirection = GetBestDirection(m_hex, targetHex, SPECIAL_DIRECTION_MASK);
        adjacentHex = GetAdjacentCellIndex(m_hex, m_attackDirection);
        if (adjacentHex == targetHex)
            return 1;
        adjacentHex = GetAdjacentCellIndex(adjacentHex, m_attackDirection);
        if (adjacentHex == targetHex)
            return 1;
    } else {
        switch (m_facing) {
            case ARMY_FACING_RIGHT:
                directionResult =
                    GetBestDirection(m_hex, targetHex, SPECIAL_DIRECTION_MASK);
                if (directionResult > COMBAT_DIRECTION_SOUTHEAST) {
                    m_attackDirection = directionResult;
                    adjacentHex = GetAdjacentCellIndex(m_hex, directionResult);
                    if (adjacentHex == targetHex)
                        return 1;
                    adjacentHex = GetAdjacentCellIndex(adjacentHex, directionResult);
                    if (adjacentHex == targetHex)
                        return 1;
                }

                directionResult =
                    GetBestDirection(m_hex + WIDE_HEX_OFFSET, targetHex, SPECIAL_DIRECTION_MASK);
                if (directionResult < COMBAT_DIRECTION_SOUTHWEST) {
                    m_attackDirection = directionResult;
                    adjacentHex = GetAdjacentCellIndex(m_hex + WIDE_HEX_OFFSET, directionResult);
                    if (adjacentHex == targetHex)
                        return 1;
                    adjacentHex = GetAdjacentCellIndex(adjacentHex, directionResult);
                    if (adjacentHex == targetHex)
                        return 1;
                }
                if (directionResult == COMBAT_DIRECTION_WEST)
                    return 0;
                if (directionResult == COMBAT_DIRECTION_NORTHWEST)
                    m_attackDirection = COMBAT_DIRECTION_WIDE_WEST;
                else if (directionResult == COMBAT_DIRECTION_SOUTHWEST)
                    m_attackDirection = COMBAT_DIRECTION_WIDE_EAST;

                adjacentHex = GetAdjacentCellIndex(m_hex + WIDE_HEX_OFFSET, directionResult);
                if (adjacentHex == targetHex)
                    return 1;
                adjacentHex = GetAdjacentCellIndex(adjacentHex, directionResult);
                if (adjacentHex == targetHex)
                    return 1;
                break;

            case ARMY_FACING_LEFT:
                directionResult =
                    GetBestDirection(m_hex, targetHex, SPECIAL_DIRECTION_MASK);
                if (directionResult < COMBAT_DIRECTION_SOUTHWEST) {
                    m_attackDirection = directionResult;
                    adjacentHex = GetAdjacentCellIndex(m_hex, directionResult);
                    if (adjacentHex == targetHex)
                        return 1;
                    adjacentHex = GetAdjacentCellIndex(adjacentHex, directionResult);
                    if (adjacentHex == targetHex)
                        return 1;
                    return 0;
                }

                directionResult =
                    GetBestDirection(m_hex - WIDE_HEX_OFFSET, targetHex, SPECIAL_DIRECTION_MASK);
                if (directionResult > COMBAT_DIRECTION_SOUTHEAST) {
                    m_attackDirection = directionResult;
                    adjacentHex = GetAdjacentCellIndex(m_hex - WIDE_HEX_OFFSET, directionResult);
                    if (adjacentHex == targetHex)
                        return 1;
                    adjacentHex = GetAdjacentCellIndex(adjacentHex, directionResult);
                    if (adjacentHex == targetHex)
                        return 1;
                    return 0;
                }
                if (directionResult == COMBAT_DIRECTION_EAST)
                    return 0;
                if (directionResult == COMBAT_DIRECTION_NORTHEAST)
                    m_attackDirection = COMBAT_DIRECTION_WIDE_WEST;
                else if (directionResult == COMBAT_DIRECTION_SOUTHEAST)
                    m_attackDirection = COMBAT_DIRECTION_WIDE_EAST;

                adjacentHex = GetAdjacentCellIndex(m_hex - WIDE_HEX_OFFSET, directionResult);
                if (adjacentHex == targetHex)
                    return 1;
                adjacentHex = GetAdjacentCellIndex(adjacentHex, directionResult);
                if (adjacentHex == targetHex)
                    return 1;
                break;
            default:
                break;
        }
    }
    return 0;
}

CombatHexDirection OppositeDirection(CombatHexDirection direction) {
    if (H2EnumIndex(direction) < COMBAT_DIRECTION_ADJACENT_COUNT) {
        return (direction + COMBAT_DIRECTION_OPPOSITE_OFFSET)
               % COMBAT_DIRECTION_ADJACENT_COUNT;
    } else {
        if (direction == COMBAT_DIRECTION_WIDE_WEST)
            return COMBAT_DIRECTION_WIDE_EAST;
        else
            return COMBAT_DIRECTION_WIDE_WEST;
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

    sourceColumnCheck = sourceHex % ARMY_HEX_COLUMNS;
    sourceRow = sourceHex / ARMY_HEX_COLUMNS;
    colTarget = targetHex % ARMY_HEX_COLUMNS;
    targetRowVal = targetHex / ARMY_HEX_COLUMNS;
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
        if (isMovingUp == 1) {
            if (sourceRow & 1) {
                if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHWEST))))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHEAST))))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WEST))))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_EAST))))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST))))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST))))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))))
                    return COMBAT_DIRECTION_WIDE_WEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST))))
                    return COMBAT_DIRECTION_WIDE_EAST;
            } else {
                if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHEAST))))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHWEST))))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_EAST))))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WEST))))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST))))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST))))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))))
                    return COMBAT_DIRECTION_WIDE_WEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST))))
                    return COMBAT_DIRECTION_WIDE_EAST;
            }
        } else {
            if (sourceRow & 1) {
                if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST))))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST))))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WEST))))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_EAST))))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHWEST))))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHEAST))))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST))))
                    return COMBAT_DIRECTION_WIDE_EAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))))
                    return COMBAT_DIRECTION_WIDE_WEST;
            } else {
                if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST))))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST))))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_EAST))))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WEST))))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHEAST))))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHWEST))))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST))))
                    return COMBAT_DIRECTION_WIDE_EAST;
                else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))))
                    return COMBAT_DIRECTION_WIDE_WEST;
            }
        }
    }

    if (leftFl == 1) {
        if (isMovingUp == 1) {
            if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHWEST))))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WEST))))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHEAST))))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST))))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_EAST))))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST))))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))))
                return COMBAT_DIRECTION_WIDE_WEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST))))
                return COMBAT_DIRECTION_WIDE_EAST;
        } else if (isMovingDown == 1) {
            if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST))))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WEST))))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST))))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHWEST))))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_EAST))))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHEAST))))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST))))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))))
                return COMBAT_DIRECTION_WIDE_WEST;
        } else {
            if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WEST))))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHWEST))))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST))))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHEAST))))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST))))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_EAST))))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST))))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))))
                return COMBAT_DIRECTION_WIDE_WEST;
        }
    } else if (isMovingRight == 1) {
        if (isMovingUp == 1) {
            if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHEAST))))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_EAST))))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHWEST))))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST))))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WEST))))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST))))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))))
                return COMBAT_DIRECTION_WIDE_WEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST))))
                return COMBAT_DIRECTION_WIDE_EAST;
        } else if (isMovingDown == 1) {
            if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST))))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_EAST))))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST))))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHEAST))))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHWEST))))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WEST))))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST))))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))))
                return COMBAT_DIRECTION_WIDE_WEST;
        } else {
            if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_EAST))))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHEAST))))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST))))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_NORTHWEST))))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST))))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WEST))))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST))))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & (1 << H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))))
                return COMBAT_DIRECTION_WIDE_WEST;
        }
    }
    return COMBAT_DIRECTION_INVALID;
}
