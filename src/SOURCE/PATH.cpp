#include <va.h>
#include <SOURCE/army.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/PATH.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/KB_TYPES.h>
#include <SOURCE/combatTypes.h>

H2_ENUM_BEGIN(CombatPathConstant)
    SPECIAL_DIRECTION_MASK = 0xc0,
    IGNORE_SPEED           = 99,
    WIDE_HEX_OFFSET        = 1
H2_ENUM_END(CombatPathConstant)

#if H2_RETAIL_COMPILER
#define pathResult pathResult2
#define savedSpeed savedSpeed2
#endif
VA(0x0047ce90, 0x10d)
i32 army::FindPath(
    i32 sourceHex,
    i32 targetHex,
    i32,
    i32 ignoreSpeed,
    ArmyPathTarget pathMode
) {
    i32 pathResult;
    i32 savedSpeed;

    if (!ValidHex(sourceHex) || !ValidHex(targetHex))
        return 0;

    savedSpeed = m_monster.speed;
    if (ignoreSpeed)
        m_monster.speed = IGNORE_SPEED;

    pathResult = gpSearchArray->FindCombatPath(sourceHex, targetHex, this, pathMode, 0);
    if (!pathResult && HAS(m_monster.attributes, MONSTER_FLAGS_WIDE)
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
            pathResult = gpSearchArray->FindCombatPath(sourceHex, targetHex, this, pathMode, 1);
    }

    m_monster.speed = savedSpeed;
    return pathResult;
}
#if H2_RETAIL_COMPILER
#undef pathResult
#undef savedSpeed
#endif

VA(0x0047cf9d, 0x80)
i32 army::ValidPath(i32 targetHex, ArmyPathTarget pathMode) {
    i32 pathResult;
    i32 H2_UNUSED(extra);

    if (!ValidHex(targetHex))
        return 0;

    if HAS(m_monster.attributes, MONSTER_FLAGS_FLYING)
        return ValidFlight(targetHex, pathMode);

    pathResult = FindPath(m_hex, targetHex, m_monster.speed, 0, pathMode);
    if (pathResult) {
        m_moveTargetHex = targetHex;
        return 1;
    }
    return 0;
}

#if H2_RETAIL_COMPILER
#define directionBitFlag directionBitFlag_a
#endif
VA(0x0047d01d, 0x63)
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
    return blockedMaskValue | (1 << IDX(COMBAT_DIRECTION_WIDE_WEST))
         | (1 << IDX(COMBAT_DIRECTION_WIDE_EAST));
}
#if H2_RETAIL_COMPILER
#undef directionBitFlag
#endif

#if H2_RETAIL_COMPILER
#define attackHexNext attackHexNext_a
#define blockedMaskValue blockedMaskValue_f
#define directionResult directionResult_a
#endif
VA(0x0047d080, 0x9b)
i32 army::GetAttackMask(i32 sourceHex, ArmyAttackTarget targetMode, i32 targetHex) {
    CombatHexDirection directionResult;
    i32 blockedMaskValue;
    i32 directionBitFlag;
    i32 directionCountNext;
    i32 attackHexNext;

    blockedMaskValue =
        HAS(m_monster.attributes, MONSTER_FLAGS_WIDE) ? 0 : SPECIAL_DIRECTION_MASK;
    directionBitFlag = 1;
    directionCountNext = HAS(m_monster.attributes, MONSTER_FLAGS_WIDE)
                             ? COMBAT_DIRECTION_COUNT
                             : COMBAT_DIRECTION_ADJACENT_COUNT;

    for (directionResult = COMBAT_DIRECTION_NORTHEAST;
         IDX(directionResult) < directionCountNext;
         directionResult++) {
        if (!ValidAttack(
                sourceHex, directionResult, targetMode, targetHex, &attackHexNext
            ))
            blockedMaskValue |= directionBitFlag;
        directionBitFlag <<= 1;
    }
    return blockedMaskValue;
}
#if H2_RETAIL_COMPILER
#undef attackHexNext
#undef blockedMaskValue
#undef directionResult
#endif

VA(0x0047d11b, 0x20)
i32 army::ValidMove(CombatHexDirection direction) {
    return ValidMove(m_hex, direction);
}

#if H2_RETAIL_COMPILER
#define destinationHexNext destHexNext
#endif
VA(0x0047d13b, 0x226)
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

    if HAS(m_monster.attributes, MONSTER_FLAGS_WIDE) {
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
            if (frontValid == 1 && rearValidResult == 1)
                return 1;
            else
                return 0;
        }
    } else
        return frontValid;
}
#if H2_RETAIL_COMPILER
#undef destinationHexNext
#endif

VA(0x0047d361, 0x1d8)
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
    if HAS(m_monster.attributes, MONSTER_FLAGS_WIDE) {
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

VA(0x0047d539, 0x6d)
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

    return gpCombatManager->m_adjacency[sourceHex][IDX(direction)];
}

VA(0x0047d5a6, 0x50)
i32 GetAdjacentCellIndexNoArmy(i32 sourceHex, CombatHexDirection direction) {
    if (sourceHex == ARMY_HEX_INVALID)
        return ARMY_HEX_INVALID;

    if (direction == COMBAT_DIRECTION_WIDE_WEST)
        direction = COMBAT_DIRECTION_NORTHWEST;
    else if (direction == COMBAT_DIRECTION_WIDE_EAST)
        direction = COMBAT_DIRECTION_SOUTHWEST;
    return gpCombatManager->m_adjacency[sourceHex][IDX(direction)];
}

#if H2_RETAIL_COMPILER
#define adjacentHex adj
#define directionResult directionResult1
#endif
VA(0x0047d5f6, 0x3c1)
i32 army::ValidRange(i32 targetHex) {
    i32 adjacentHex;
    CombatHexDirection directionResult;

    if (!ValidHex(targetHex))
        return 0;

    m_moveTargetHex = m_hex;
    if (!(m_monster.attributes & MONSTER_FLAGS_WIDE)) {
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
        }
    }
    return 0;
}
#if H2_RETAIL_COMPILER
#undef adjacentHex
#undef directionResult
#endif

VA(0x0047d9b7, 0x35)
CombatHexDirection OppositeDirection(CombatHexDirection direction) {
    if (IDX(direction) < COMBAT_DIRECTION_ADJACENT_COUNT) {
        return (direction + COMBAT_DIRECTION_OPPOSITE_OFFSET)
               % COMBAT_DIRECTION_ADJACENT_COUNT;
    } else {
        if (direction == COMBAT_DIRECTION_WIDE_WEST)
            return COMBAT_DIRECTION_WIDE_EAST;
        else
            return COMBAT_DIRECTION_WIDE_WEST;
    }
}

#if H2_RETAIL_COMPILER
#define sourceRow srcRow
#endif
VA(0x0047d9ec, 0x77e)
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
        if (isMovingUp == 1) {
            if (sourceRow & 1) {
                if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHWEST)))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHEAST)))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_WEST)))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_EAST)))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHWEST)))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHEAST)))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_WEST)))
                    return COMBAT_DIRECTION_WIDE_WEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_EAST)))
                    return COMBAT_DIRECTION_WIDE_EAST;
            } else {
                if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHEAST)))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHWEST)))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_EAST)))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_WEST)))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHEAST)))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHWEST)))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_WEST)))
                    return COMBAT_DIRECTION_WIDE_WEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_EAST)))
                    return COMBAT_DIRECTION_WIDE_EAST;
            }
        } else {
            if (sourceRow & 1) {
                if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHWEST)))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHEAST)))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_WEST)))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_EAST)))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHWEST)))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHEAST)))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_EAST)))
                    return COMBAT_DIRECTION_WIDE_EAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_WEST)))
                    return COMBAT_DIRECTION_WIDE_WEST;
            } else {
                if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHEAST)))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHWEST)))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_EAST)))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_WEST)))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHEAST)))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHWEST)))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_EAST)))
                    return COMBAT_DIRECTION_WIDE_EAST;
                else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_WEST)))
                    return COMBAT_DIRECTION_WIDE_WEST;
            }
        }
    }

    if (leftFl == 1) {
        if (isMovingUp == 1) {
            if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
        } else if (isMovingDown == 1) {
            if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
        } else {
            if (!(blockedMask & BIT(COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
        }
    } else if (isMovingRight == 1) {
        if (isMovingUp == 1) {
            if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
        } else if (isMovingDown == 1) {
            if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
        } else {
            if (!(blockedMask & BIT(COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & BIT(COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
        }
    }
    return COMBAT_DIRECTION_INVALID;
}
#if H2_RETAIL_COMPILER
#undef sourceRow
#endif

VA_COMPGEN(0x0047e16a, 0x27, LOCALE_FACET_ID_INIT, WCharCtypeId)
VA_COMPGEN(0x0047e191, 0x12, LOCALE_FACET_ID_ATEXIT, WCharCtypeId)
