// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\PATH.OBJ   from: (directly linked into exe)
// functions: 12   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/army.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/PATH.h>
#include <SOURCE/searchArray.h>

VA(0x004bdbf0, 0x14a)
int army::FindPath(int sourceHex, int targetHex, int, int ignoreSpeed, int pathMode)
{
    int pathResult;
    int savedSpeed;

    if (!ValidHex(sourceHex) || !ValidHex(targetHex))
        return 0;

    savedSpeed = m_monster.speed;
    if (ignoreSpeed)
        m_monster.speed = COMBAT_PATH_IGNORE_SPEED;

    pathResult = gpSearchArray->FindCombatPath(
        sourceHex, targetHex, this, pathMode, 0);
    if (!pathResult && (m_monster.attributes & MONSTER_ATTRIBUTE_WIDE) &&
        pathMode == 0) {
        switch (m_facing) {
        case 0:
            targetHex = GetAdjacentCellIndex(
                targetHex, COMBAT_DIRECTION_NORTHEAST);
            break;
        case 1:
            targetHex = GetAdjacentCellIndex(
                targetHex, COMBAT_DIRECTION_SOUTHWEST);
            break;
        }

        if (!ValidHex(targetHex))
            pathResult = 0;
        else
            pathResult = gpSearchArray->FindCombatPath(
                sourceHex, targetHex, this, pathMode, 1);
    }

    m_monster.speed = static_cast<signed char>(savedSpeed);
    return pathResult;
}

VA(0x004bdd3a, 0x9a)
int army::ValidPath(int targetHex, int pathMode)
{
    int pathResult;
    int reserved;

    if (!ValidHex(targetHex))
        return 0;

    if (m_monster.attributes & MONSTER_ATTRIBUTE_FLYING)
        return ValidFlight(targetHex, pathMode);

    pathResult = FindPath(
        m_hex, targetHex, m_monster.speed, 0, pathMode);
    if (pathResult) {
        m_moveTargetHex = targetHex;
        return 1;
    }
    return 0;
}

VA(0x004bddd4, 0x6d)
int army::GetMoveMask(int sourceHex)
{
    int directionResult;
    int blockedMaskValue = 0;
    int directionBitFlag = 1;

    for (directionResult = 0;
         directionResult <= COMBAT_DIRECTION_WEST;
         directionResult++) {
        if (!ValidMove(sourceHex, directionResult))
            blockedMaskValue |= directionBitFlag;
        directionBitFlag <<= 1;
    }
    return blockedMaskValue | COMBAT_PATH_SPECIAL_DIRECTION_MASK;
}

VA(0x004bde41, 0xb5)
int army::GetAttackMask(int sourceHex, int targetMode, int targetHex)
{
    int directionResult;
    int directionCountNext;
    int blockedMaskValue;
    int directionBitFlag;
    int attackHexNext;

    if (m_monster.attributes & MONSTER_ATTRIBUTE_WIDE)
        blockedMaskValue = 0;
    else
        blockedMaskValue = COMBAT_PATH_SPECIAL_DIRECTION_MASK;

    directionBitFlag = 1;

    if (m_monster.attributes & MONSTER_ATTRIBUTE_WIDE)
        directionCountNext = COMBAT_PATH_WIDE_DIRECTION_COUNT;
    else
        directionCountNext = COMBAT_PATH_DIRECTION_COUNT;

    for (directionResult = 0;
         directionResult < directionCountNext;
         directionResult++) {
        if (!ValidAttack(sourceHex, directionResult, targetMode, targetHex,
                         &attackHexNext))
            blockedMaskValue |= directionBitFlag;
        directionBitFlag <<= 1;
    }
    return blockedMaskValue;
}

VA(0x004bdef6, 0x2b)
int army::ValidMove(int direction)
{
    return ValidMove(m_hex, direction);
}

VA(0x004bdf21, 0x2f6)
int army::ValidMove(int sourceHex, int direction)
{
    int destinationHexNext;
    int rearHex;
    int frontValid;
    int rearValidResult;

    if (!ValidHex(sourceHex))
        return 0;

    destinationHexNext = GetAdjacentCellIndex(sourceHex, direction);
    if (!ValidHex(destinationHexNext))
        return 0;

    frontValid = 0;
    if (gpCombatManager->m_hexCells[destinationHexNext].m_occupantSide == -1 &&
        (!gpCombatManager->m_hexCells[destinationHexNext].m_blocked ||
         (gpCombatManager->m_inCastleCombat &&
          (destinationHexNext == COMBAT_CASTLE_GATE_APPROACH_HEX ||
           destinationHexNext == COMBAT_CASTLE_HEX_GATE) &&
          (gpCombatManager->m_drawbridgeState != COMBAT_DRAWBRIDGE_RAISED ||
           (gpCombatManager->m_currentSide == COMBAT_DEFENDER_SIDE &&
            gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                    .m_occupantSide == -1 &&
            gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                    .m_deadOccupantCount == 0))))) {
        frontValid = 1;
    }

    if (m_monster.attributes & MONSTER_ATTRIBUTE_WIDE) {
        rearHex = COMBAT_PATH_INVALID_HEX;
        switch (m_facing) {
        case 0:
            if (direction == COMBAT_DIRECTION_NORTHEAST)
                return frontValid;
            else
                rearHex = GetAdjacentCellIndex(
                    destinationHexNext, COMBAT_DIRECTION_SOUTHWEST);
            break;
        case 1:
            if (direction == COMBAT_DIRECTION_SOUTHWEST)
                return frontValid;
            else
                rearHex = GetAdjacentCellIndex(
                    destinationHexNext, COMBAT_DIRECTION_NORTHEAST);
            break;
        }

        rearValidResult = 0;
        if (ValidHex(rearHex) &&
            gpCombatManager->m_hexCells[rearHex].m_occupantSide == -1 &&
            (!gpCombatManager->m_hexCells[rearHex].m_blocked ||
             (gpCombatManager->m_inCastleCombat &&
              (rearHex == COMBAT_CASTLE_GATE_APPROACH_HEX ||
               rearHex == COMBAT_CASTLE_HEX_GATE) &&
              (gpCombatManager->m_drawbridgeState != COMBAT_DRAWBRIDGE_RAISED ||
               (gpCombatManager->m_currentSide == COMBAT_DEFENDER_SIDE &&
                gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                        .m_occupantSide == -1 &&
                gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                        .m_deadOccupantCount == 0))))) {
            rearValidResult = 1;
        }

        if (direction == COMBAT_DIRECTION_NORTHEAST ||
            direction == COMBAT_DIRECTION_SOUTHWEST)
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
int army::ValidAttack(int sourceHex, int direction, int targetMode,
                      int requiredTargetHex, int *attackHex)
{
    int adjacentSourceHex;
    int occupantSide;

    if (!ValidHex(sourceHex))
        return 0;

    adjacentSourceHex = sourceHex;
    if (m_monster.attributes & MONSTER_ATTRIBUTE_WIDE) {
        if (direction == COMBAT_DIRECTION_WIDE_WEST) {
            *attackHex = GetAdjacentCellIndex(
                sourceHex,
                static_cast<unsigned int>(m_facing) < 1
                    ? COMBAT_DIRECTION_WEST
                    : COMBAT_DIRECTION_NORTHWEST);
        } else if (direction == COMBAT_DIRECTION_WIDE_EAST) {
            *attackHex = GetAdjacentCellIndex(
                sourceHex,
                static_cast<unsigned int>(m_facing) < 1
                    ? COMBAT_DIRECTION_SOUTHEAST
                    : COMBAT_DIRECTION_EAST);
        } else {
            switch (m_facing) {
            case 0:
                if (direction >= COMBAT_DIRECTION_SOUTHEAST)
                    adjacentSourceHex = GetAdjacentCellIndex(
                        sourceHex, COMBAT_DIRECTION_SOUTHWEST);
                break;
            case 1:
                if (direction <= COMBAT_DIRECTION_EAST)
                    adjacentSourceHex = GetAdjacentCellIndex(
                        sourceHex, COMBAT_DIRECTION_NORTHEAST);
                break;
            }

            if (adjacentSourceHex == COMBAT_PATH_INVALID_HEX)
                return 0;
            *attackHex = GetAdjacentCellIndex(adjacentSourceHex, direction);
        }
    } else {
        *attackHex = GetAdjacentCellIndex(sourceHex, direction);
    }

    if (!ValidHex(*attackHex))
        return 0;
    if (requiredTargetHex != COMBAT_PATH_INVALID_HEX &&
        *attackHex != requiredTargetHex)
        return 0;

    occupantSide = gpCombatManager->m_hexCells[*attackHex].m_occupantSide;
    switch (targetMode) {
    case 0:
        if (m_targetSide == occupantSide &&
            gpCombatManager->m_hexCells[*attackHex].m_occupantIndex ==
                m_targetIndex)
            return 1;
        break;
    case 1:
        if (1 - gpCombatManager->m_currentSide == occupantSide)
            return 1;
        break;
    case 2:
        if (occupantSide != -1)
            return 1;
        break;
    }
    return 0;
}

// @early-stop
// TU-cumulative +0x7f..+0x8a: both streams compute direction + sourceHex*6,
// but retail loads direction first and uses two LEAs while ours scales sourceHex
// first. Frame/CFG and the gpCombatManager relocation agree; direct, flat,
// pointer, reordered, and SIB-index spellings were exhausted.
VA(0x004be48a, 0xa5)
int army::GetAdjacentCellIndex(int sourceHex, int direction)
{
    if (sourceHex == COMBAT_PATH_INVALID_HEX)
        return COMBAT_PATH_INVALID_HEX;

    if (direction == COMBAT_DIRECTION_WIDE_WEST) {
        if (m_facing == 1)
            direction = COMBAT_DIRECTION_WEST;
        else
            direction = COMBAT_DIRECTION_NORTHWEST;
    } else if (direction == COMBAT_DIRECTION_WIDE_EAST) {
        if (m_facing == 1)
            direction = COMBAT_DIRECTION_SOUTHEAST;
        else
            direction = COMBAT_DIRECTION_EAST;
    }

    return direction[gpCombatManager->m_adjacency[0] +
                     sourceHex * COMBAT_AI_ADJACENT_DIRECTION_COUNT];
}

VA(0x004be52f, 0x6e)
int GetAdjacentCellIndexNoArmy(int sourceHex, int direction)
{
    if (sourceHex == COMBAT_PATH_INVALID_HEX)
        return COMBAT_PATH_INVALID_HEX;

    if (direction == COMBAT_DIRECTION_WIDE_WEST)
        direction = COMBAT_DIRECTION_WEST;
    else if (direction == COMBAT_DIRECTION_WIDE_EAST)
        direction = COMBAT_DIRECTION_SOUTHEAST;
    return 0[&direction][gpCombatManager->m_adjacency[sourceHex]];
}

VA(0x004be59d, 0x44a)
int army::ValidRange(int targetHex)
{
    int adjacentHex;
    int directionResult;

    if (!ValidHex(targetHex))
        return 0;

    m_moveTargetHex = m_hex;
    if (!(m_monster.attributes & MONSTER_ATTRIBUTE_WIDE)) {
        m_attackDirection = GetBestDirection(
            m_hex, targetHex, COMBAT_PATH_SPECIAL_DIRECTION_MASK);
        adjacentHex = GetAdjacentCellIndex(m_hex, m_attackDirection);
        if (adjacentHex == targetHex)
            return 1;
        adjacentHex = GetAdjacentCellIndex(adjacentHex, m_attackDirection);
        if (adjacentHex == targetHex)
            return 1;
    } else {
        switch (m_facing) {
        case 1:
            directionResult = GetBestDirection(
                m_hex, targetHex, COMBAT_PATH_SPECIAL_DIRECTION_MASK);
            if (directionResult > COMBAT_DIRECTION_EAST) {
                m_attackDirection = directionResult;
                adjacentHex = GetAdjacentCellIndex(m_hex, directionResult);
                if (adjacentHex == targetHex)
                    return 1;
                adjacentHex = GetAdjacentCellIndex(
                    adjacentHex, directionResult);
                if (adjacentHex == targetHex)
                    return 1;
            }

            directionResult = GetBestDirection(
                m_hex + 1, targetHex, COMBAT_PATH_SPECIAL_DIRECTION_MASK);
            if (directionResult < COMBAT_DIRECTION_SOUTHEAST) {
                m_attackDirection = directionResult;
                adjacentHex = GetAdjacentCellIndex(
                    m_hex + 1, directionResult);
                if (adjacentHex == targetHex)
                    return 1;
                adjacentHex = GetAdjacentCellIndex(
                    adjacentHex, directionResult);
                if (adjacentHex == targetHex)
                    return 1;
            }
            if (directionResult == COMBAT_DIRECTION_SOUTHWEST)
                return 0;
            if (directionResult == COMBAT_DIRECTION_WEST)
                m_attackDirection = COMBAT_DIRECTION_WIDE_WEST;
            else if (directionResult == COMBAT_DIRECTION_SOUTHEAST)
                m_attackDirection = COMBAT_DIRECTION_WIDE_EAST;

            adjacentHex = GetAdjacentCellIndex(
                m_hex + 1, directionResult);
            if (adjacentHex == targetHex)
                return 1;
            adjacentHex = GetAdjacentCellIndex(
                adjacentHex, directionResult);
            if (adjacentHex == targetHex)
                return 1;
            break;

        case 0:
            directionResult = GetBestDirection(
                m_hex, targetHex, COMBAT_PATH_SPECIAL_DIRECTION_MASK);
            if (directionResult < COMBAT_DIRECTION_SOUTHEAST) {
                m_attackDirection = directionResult;
                adjacentHex = GetAdjacentCellIndex(m_hex, directionResult);
                if (adjacentHex == targetHex)
                    return 1;
                adjacentHex = GetAdjacentCellIndex(
                    adjacentHex, directionResult);
                if (adjacentHex == targetHex)
                    return 1;
                return 0;
            }

            directionResult = GetBestDirection(
                m_hex - 1, targetHex, COMBAT_PATH_SPECIAL_DIRECTION_MASK);
            if (directionResult > COMBAT_DIRECTION_EAST) {
                m_attackDirection = directionResult;
                adjacentHex = GetAdjacentCellIndex(
                    m_hex - 1, directionResult);
                if (adjacentHex == targetHex)
                    return 1;
                adjacentHex = GetAdjacentCellIndex(
                    adjacentHex, directionResult);
                if (adjacentHex == targetHex)
                    return 1;
                return 0;
            }
            if (directionResult == COMBAT_DIRECTION_NORTHEAST)
                return 0;
            if (directionResult == COMBAT_DIRECTION_NORTHWEST)
                m_attackDirection = COMBAT_DIRECTION_WIDE_WEST;
            else if (directionResult == COMBAT_DIRECTION_EAST)
                m_attackDirection = COMBAT_DIRECTION_WIDE_EAST;

            adjacentHex = GetAdjacentCellIndex(
                m_hex - 1, directionResult);
            if (adjacentHex == targetHex)
                return 1;
            adjacentHex = GetAdjacentCellIndex(
                adjacentHex, directionResult);
            if (adjacentHex == targetHex)
                return 1;
            break;
        }
    }
    return 0;
}

VA(0x004be9e7, 0x58)
int OppositeDirection(int direction)
{
    if (direction < COMBAT_PATH_DIRECTION_COUNT) {
        return (direction + 3) % COMBAT_PATH_DIRECTION_COUNT;
    } else {
        if (direction == COMBAT_DIRECTION_WIDE_WEST)
            return COMBAT_DIRECTION_WIDE_EAST;
        else
            return COMBAT_DIRECTION_WIDE_WEST;
    }
}

VA(0x004bea3f, 0x8ff)
int army::GetBestDirection(int sourceHex, int targetHex, int blockedMask)
{
    int sourceRow;
    int sourceColumnCheck;
    int targetRowValue;
    int targetColumn;
    int isMovingUp;
    int isMovingDown;
    int leftFlag;
    int isMovingRight;

    if (!ValidHex(sourceHex) || !ValidHex(targetHex))
        return COMBAT_PATH_INVALID_HEX;

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
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                    return COMBAT_DIRECTION_WIDE_WEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                    return COMBAT_DIRECTION_WIDE_EAST;
            } else {
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
            }
        } else {
            if (sourceRow & 1) {
                if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                    return COMBAT_DIRECTION_WIDE_EAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                    return COMBAT_DIRECTION_WIDE_WEST;
            } else {
                if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                    return COMBAT_DIRECTION_EAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                    return COMBAT_DIRECTION_SOUTHEAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                    return COMBAT_DIRECTION_NORTHEAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                    return COMBAT_DIRECTION_SOUTHWEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                    return COMBAT_DIRECTION_NORTHWEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                    return COMBAT_DIRECTION_WEST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                    return COMBAT_DIRECTION_WIDE_EAST;
                else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                    return COMBAT_DIRECTION_WIDE_WEST;
            }
        }
    }

    if (leftFlag == 1) {
        if (isMovingUp == 1) {
            if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
        } else if (isMovingDown == 1) {
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
        } else {
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
        }
    } else if (isMovingRight == 1) {
        if (isMovingUp == 1) {
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
        } else if (isMovingDown == 1) {
            if (!(blockedMask & (1 << COMBAT_DIRECTION_EAST)))
                return COMBAT_DIRECTION_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHEAST)))
                return COMBAT_DIRECTION_NORTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHEAST)))
                return COMBAT_DIRECTION_SOUTHEAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_NORTHWEST)))
                return COMBAT_DIRECTION_NORTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WEST)))
                return COMBAT_DIRECTION_WEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_SOUTHWEST)))
                return COMBAT_DIRECTION_SOUTHWEST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
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
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_EAST)))
                return COMBAT_DIRECTION_WIDE_EAST;
            else if (!(blockedMask & (1 << COMBAT_DIRECTION_WIDE_WEST)))
                return COMBAT_DIRECTION_WIDE_WEST;
        }
    }
    return COMBAT_PATH_INVALID_HEX;
}
