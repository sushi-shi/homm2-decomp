#include <va.h>
#include <SOURCE/army.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/PATH.h>
#include <SOURCE/searchArray.h>

H2_ENUM_BEGIN(CombatPathConstant)
    DIRECTION_COUNT        = 6,
    WIDE_DIRECTION_COUNT   = 8,
    SPECIAL_DIRECTION_MASK = 0xc0,
    IGNORE_SPEED           = 99,
    WIDE_HEX_OFFSET        = 1
H2_ENUM_END(CombatPathConstant)

VA(0x004bdbf0, 0x14a)
i32 army::FindPath(i32 sourceHex, i32 targetHex, i32, i32 ignoreSpeed, i32 pathMode) {
    i32 pathResult;
    i32 savedSpeed;

    if (!ValidHex(sourceHex) || !ValidHex(targetHex))
        return 0;

    savedSpeed = m_monster.speed;
    if (ignoreSpeed)
        m_monster.speed = IGNORE_SPEED;

    pathResult = gpSearchArray->FindCombatPath(sourceHex, targetHex, this, pathMode, 0);
    if (!pathResult && HAS(m_monster.attributes, MONSTER_ATTRIBUTE_WIDE)
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

    m_monster.speed = static_cast<i8>(savedSpeed);
    return pathResult;
}

VA(0x004bdd3a, 0x9a)
i32 army::ValidPath(i32 targetHex, i32 pathMode) {
    i32 pathResult;
    i32 reserved;

    if (!ValidHex(targetHex))
        return 0;

    if HAS(m_monster.attributes, MONSTER_ATTRIBUTE_FLYING)
        return ValidFlight(targetHex, pathMode);

    pathResult = FindPath(m_hex, targetHex, m_monster.speed, 0, pathMode);
    if (pathResult) {
        m_moveTargetHex = targetHex;
        return 1;
    }
    return 0;
}

VA(0x004bddd4, 0x6d)
i32 army::GetMoveMask(i32 sourceHex) {
    i32 directionResult;
    i32 blockedMaskValue = 0;
    i32 directionBitFlag = 1;

    for (directionResult = 0; directionResult <= COMBAT_DIRECTION_NORTHWEST; directionResult++) {
        if (!ValidMove(sourceHex, directionResult))
            blockedMaskValue |= directionBitFlag;
        directionBitFlag <<= 1;
    }
    return blockedMaskValue | SPECIAL_DIRECTION_MASK;
}

VA(0x004bde41, 0xb5)
i32 army::GetAttackMask(i32 sourceHex, i32 targetMode, i32 targetHex) {
    i32 directionResult;
    i32 directionCountNext;
    i32 blockedMaskValue;
    i32 directionBitFlag;
    i32 attackHexNext;

    if HAS(m_monster.attributes, MONSTER_ATTRIBUTE_WIDE)
        blockedMaskValue = 0;
    else
        blockedMaskValue = SPECIAL_DIRECTION_MASK;

    directionBitFlag = 1;

    if HAS(m_monster.attributes, MONSTER_ATTRIBUTE_WIDE)
        directionCountNext = WIDE_DIRECTION_COUNT;
    else
        directionCountNext = DIRECTION_COUNT;

    for (directionResult = 0; directionResult < directionCountNext; directionResult++) {
        if (!ValidAttack(sourceHex, directionResult, targetMode, targetHex, &attackHexNext))
            blockedMaskValue |= directionBitFlag;
        directionBitFlag <<= 1;
    }
    return blockedMaskValue;
}

VA(0x004bdef6, 0x2b)
i32 army::ValidMove(i32 direction) {
    return ValidMove(m_hex, direction);
}

VA(0x004bdf21, 0x2f6)
i32 army::ValidMove(i32 sourceHex, i32 direction) {
    i32 destinationHexNext;
    i32 rearHex;
    i32 frontValid;
    i32 rearValidResult;

    if (!ValidHex(sourceHex))
        return 0;

    destinationHexNext = GetAdjacentCellIndex(sourceHex, direction);
    if (!ValidHex(destinationHexNext))
        return 0;

    frontValid = 0;
    if (gpCombatManager->m_hexCells[destinationHexNext].m_occupantSide == COMBAT_OCCUPANT_NONE
        && (!gpCombatManager->m_hexCells[destinationHexNext].m_blocked
            || (gpCombatManager->m_inCastleCombat
                && (destinationHexNext == COMBAT_CASTLE_GATE_APPROACH_HEX
                    || destinationHexNext == IDX(COMBAT_CASTLE_HEX_GATE))
                && (gpCombatManager->m_drawbridgeState != COMBAT_DRAWBRIDGE_RAISED
                    || (gpCombatManager->m_currentSide == COMBAT_DEFENDER_SIDE
                        && gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                   .m_occupantSide
                               == COMBAT_OCCUPANT_NONE
                        && gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                   .m_deadOccupantCount
                               == 0))))) {
        frontValid = 1;
    }

    if HAS(m_monster.attributes, MONSTER_ATTRIBUTE_WIDE) {
        rearHex = ARMY_HEX_INVALID;
        switch (m_facing) {
            case ARMY_FACING_LEFT:
                if (direction == COMBAT_DIRECTION_EAST)
                    return frontValid;
                else
                    rearHex = GetAdjacentCellIndex(destinationHexNext, COMBAT_DIRECTION_WEST);
                break;
            case ARMY_FACING_RIGHT:
                if (direction == COMBAT_DIRECTION_WEST)
                    return frontValid;
                else
                    rearHex = GetAdjacentCellIndex(destinationHexNext, COMBAT_DIRECTION_EAST);
                break;
        }

        rearValidResult = 0;
        if (ValidHex(rearHex) && gpCombatManager->m_hexCells[rearHex].m_occupantSide == COMBAT_OCCUPANT_NONE
            && (!gpCombatManager->m_hexCells[rearHex].m_blocked
                || (gpCombatManager->m_inCastleCombat
                    && (rearHex == COMBAT_CASTLE_GATE_APPROACH_HEX
                        || rearHex == IDX(COMBAT_CASTLE_HEX_GATE))
                    && (gpCombatManager->m_drawbridgeState != COMBAT_DRAWBRIDGE_RAISED
                        || (gpCombatManager->m_currentSide == COMBAT_DEFENDER_SIDE
                            && gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                       .m_occupantSide
                                   == COMBAT_OCCUPANT_NONE
                            && gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                       .m_deadOccupantCount
                                   == 0))))) {
            rearValidResult = 1;
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

VA(0x004be217, 0x273)
i32 army::ValidAttack(
    i32 sourceHex,
    i32 direction,
    i32 targetMode,
    i32 requiredTargetHex,
    i32* attackHex
) {
    i32 adjacentSourceHex;
    i32 occupantSide;

    if (!ValidHex(sourceHex))
        return 0;

    adjacentSourceHex = sourceHex;
    if HAS(m_monster.attributes, MONSTER_ATTRIBUTE_WIDE) {
        if (direction == COMBAT_DIRECTION_WIDE_WEST) {
            *attackHex = GetAdjacentCellIndex(
                sourceHex,
                static_cast<u32>(m_facing) < ARMY_FACING_RIGHT ? COMBAT_DIRECTION_NORTHWEST
                                                               : COMBAT_DIRECTION_NORTHEAST
            );
        } else if (direction == COMBAT_DIRECTION_WIDE_EAST) {
            *attackHex = GetAdjacentCellIndex(
                sourceHex,
                static_cast<u32>(m_facing) < ARMY_FACING_RIGHT ? COMBAT_DIRECTION_SOUTHWEST
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
            if (m_targetSide == occupantSide
                && gpCombatManager->m_hexCells[*attackHex].m_occupantIndex == m_targetIndex)
                return 1;
            break;
        case ARMY_ATTACK_TARGET_ENEMY:
            if (1 - gpCombatManager->m_currentSide == occupantSide)
                return 1;
            break;
        case ARMY_ATTACK_TARGET_OCCUPIED:
            if (occupantSide != -1)
                return 1;
            break;
    }
    return 0;
}

VA(0x004be48a, 0xa5)
i32 army::GetAdjacentCellIndex(i32 sourceHex, i32 direction) {
    if (sourceHex == ARMY_HEX_INVALID)
        return ARMY_HEX_INVALID;

    if (direction == COMBAT_DIRECTION_WIDE_WEST) {
        if (m_facing == ARMY_FACING_RIGHT)
            direction = COMBAT_DIRECTION_NORTHWEST;
        else
            direction = COMBAT_DIRECTION_NORTHEAST;
    } else if (direction == COMBAT_DIRECTION_WIDE_EAST) {
        if (m_facing == ARMY_FACING_RIGHT)
            direction = COMBAT_DIRECTION_SOUTHWEST;
        else
            direction = COMBAT_DIRECTION_SOUTHEAST;
    }

    return direction
        [gpCombatManager->m_adjacency[0] + sourceHex * COMBAT_AI_ADJACENT_DIRECTION_COUNT];
}

VA(0x004be52f, 0x6e)
i32 GetAdjacentCellIndexNoArmy(i32 sourceHex, i32 direction) {
    if (sourceHex == ARMY_HEX_INVALID)
        return ARMY_HEX_INVALID;

    if (direction == COMBAT_DIRECTION_WIDE_WEST)
        direction = COMBAT_DIRECTION_NORTHWEST;
    else if (direction == COMBAT_DIRECTION_WIDE_EAST)
        direction = COMBAT_DIRECTION_SOUTHWEST;
    return OD_STEER(direction)[gpCombatManager->m_adjacency[sourceHex]];
}

VA(0x004be59d, 0x44a)
i32 army::ValidRange(i32 targetHex) {
    i32 adjacentHex;
    i32 directionResult;

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
        }
    }
    return 0;
}

#define OPPOSITE_DIRECTION_OFFSET (DIRECTION_COUNT / 2)

VA(0x004be9e7, 0x58)
i32 OppositeDirection(i32 direction) {
    if (direction < DIRECTION_COUNT) {
        return (direction + OPPOSITE_DIRECTION_OFFSET) % DIRECTION_COUNT;
    } else {
        if (direction == COMBAT_DIRECTION_WIDE_WEST)
            return COMBAT_DIRECTION_WIDE_EAST;
        else
            return COMBAT_DIRECTION_WIDE_WEST;
    }
}

#undef OPPOSITE_DIRECTION_OFFSET

VA(0x004bea3f, 0x8ff)
i32 army::GetBestDirection(i32 sourceHex, i32 targetHex, i32 blockedMask) {
    i32 sourceRow;
    i32 sourceColumnCheck;
    i32 targetRowValue;
    i32 targetColumn;
    i32 isMovingUp;
    i32 isMovingDown;
    i32 leftFlag;
    i32 isMovingRight;

    if (!ValidHex(sourceHex) || !ValidHex(targetHex))
        return ARMY_HEX_INVALID;

    sourceColumnCheck = sourceHex % ARMY_HEX_COLUMNS;
    sourceRow = sourceHex / ARMY_HEX_COLUMNS;
    targetColumn = targetHex % ARMY_HEX_COLUMNS;
    targetRowValue = targetHex / ARMY_HEX_COLUMNS;
    isMovingUp = 0;
    isMovingDown = 0;
    leftFlag = 0;
    isMovingRight = 0;

    if ((sourceColumnCheck | 0) < targetColumn)
        isMovingRight = 1;
    else if ((sourceColumnCheck | 0) != targetColumn)
        leftFlag = 1;

    if ((sourceRow | 0) < targetRowValue)
        isMovingDown = 1;
    else if ((sourceRow | 0) != targetRowValue)
        isMovingUp = 1;

    if ((leftFlag | 0) == isMovingRight) {
        if (isMovingUp == 1) {
            if (sourceRow & 1) {
                if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                    return COMBAT_DIRECTION_WIDE_WEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                    return COMBAT_DIRECTION_WIDE_EAST;
            } else {
                if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                    return COMBAT_DIRECTION_WIDE_WEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                    return COMBAT_DIRECTION_WIDE_EAST;
            }
        } else {
            if (sourceRow & 1) {
                if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                    return COMBAT_DIRECTION_WIDE_EAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                    return COMBAT_DIRECTION_WIDE_WEST;
            } else {
                if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                    return COMBAT_DIRECTION_WIDE_EAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                    return COMBAT_DIRECTION_WIDE_WEST;
            }
        }
    }

    if (leftFlag == 1) {
        if (isMovingUp == 1) {
            if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
        } else if (isMovingDown == 1) {
            if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
        } else {
            if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
        }
    } else if (isMovingRight == 1) {
        if (isMovingUp == 1) {
            if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
        } else if (isMovingDown == 1) {
            if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
        } else {
            if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
        }
    }
    return ARMY_HEX_INVALID;
}
