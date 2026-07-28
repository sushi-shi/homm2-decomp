#include <va.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/PATH.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/army.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/FINDPATH.h>
#include <SOURCE/FINDPATH_TYPES.h>
#include <SOURCE/CMBTMGR.h>
#include <EDITOR/mapcell.h>
#include <stdlib.h>
#include <string.h>

namespace {

H2_ENUM_BEGIN(FindPathConstant)
    ATTACK_MASK_SURROUNDED         = 0xff,
    ALLOCATION_SOURCE_LINE         = 20,
    DESTRUCTION_SOURCE_LINE        = 26,
    DISTANCE_MINOR_DIVISOR         = 2,
    BINARY_SEARCH_MIDPOINT_DIVISOR = 2,
    DRAWBRIDGE_MOAT_INDEX          = 4,
    MOAT_MOVEMENT_PENALTY          = 2,
    INITIAL_BEST_DISTANCE          = COMBAT_SCREEN_WIDTH
H2_ENUM_END(FindPathConstant)

inline i32 ApproximateGridDistance(i32 xDistance, i32 yDistance) {
    if (xDistance >= yDistance)
        return xDistance + yDistance / DISTANCE_MINOR_DIVISOR;
    return yDistance + xDistance / DISTANCE_MINOR_DIVISOR;
}

}

DATA(0x0052adc0) static i32 gSearchLow;
DATA(0x0052adc4) static mapCell* gSearchNextCell;
DATA(0x0052ae40) static searchNode* gSearchCell;
DATA(0x0052ae44) static i32 gSearchNextY;
DATA(0x0052ae48) static mapCell* gSearchCurrentCell;
DATA(0x0052ae4c) static i32 gSearchDirection;
DATA(0x0052ae50) static H2_ENUM_STORAGE(MapObjectType, i32) gSearchTriggerType;
DATA(0x0052ae54) static i32 gSearchNextX;
DATA(0x0052ae58) static H2_ENUM_STORAGE(TerrainType, i32) gSearchTerrain;
DATA(0x0052ae5c) static searchNode* gSearchQueueNode;
DATA(0x0052ae60) static i32 gSearchMiddle;
DATA(0x0052ae64) static i32 gSearchHigh;

DATA(0x0051733c) static SFindPathSourceLocation gSearchAllocationSource = {
    {ALLOCATION_SOURCE_LINE, 0},
    FINDPATH_SOURCE_FILE
};
DATA(0x0051736c) static SFindPathSourceLocation gSearchDestructionSource = {
    {DESTRUCTION_SOURCE_LINE, 0},
    FINDPATH_SOURCE_FILE
};

VA(0x004a4a50, 0xe)
searchArray::searchArray(void) {
    m_storage.cells = NULL;
    m_maxQueueCount = 0;
}

VA(0x004a4a60, 0x2c)
searchArray::~searchArray() {
    if (m_storage.cells != NULL)
        H2_FREE_AT(
            m_storage.cells,
            gSearchDestructionSource.sourceFile,
            gSearchDestructionSource.line.value + 1
        );
    m_storage.cells = NULL;
}

VA(0x004a4a90, 0x56)
void searchArray::Init(void) {
    if (m_storage.cells != NULL)
        H2_FREE_AT(
            m_storage.cells,
            gSearchDestructionSource.sourceFile,
            gSearchDestructionSource.line.value + 1
        );
    m_storage.cells = NULL;
    m_storage.cells = static_cast<searchCell*>(H2_ALLOC_AT(
        MAP_WIDTH * MAP_HEIGHT * sizeof(searchCell),
        gSearchAllocationSource.sourceFile,
        gSearchAllocationSource.line.value + 2
    ));
}

VA(0x004a4af0, 0x2c)
void searchArray::Close(void) {
    if (m_storage.cells != NULL)
        H2_FREE_AT(
            m_storage.cells,
            gSearchDestructionSource.sourceFile,
            gSearchDestructionSource.line.value + 1
        );
    m_storage.cells = NULL;
}

VA(0x004a4b20, 0x3c)
void searchArray::Clear(void) {
    memset(m_queue, 0, sizeof(m_queue));
    memset(m_storage.cells, 0, MAP_WIDTH * MAP_HEIGHT * sizeof(searchCell));
    m_pathLength = 0;
    m_queueCount = 0;
}

VA(0x004a4b60, 0x3d)
i32 searchArray::QuickDistance(i32 x1, i32 y1, i32 x2, i32 y2) {
    i32 xDistance = abs(x1 - x2);
    i32 yDistance = abs(y1 - y2);

    return ApproximateGridDistance(xDistance, yDistance);
}

VA(0x004a4ba0, 0x75)
i32 CalcTerrainCost(
    H2_ENUM_PARAM(TerrainType, i32) terrain,
    i32 diagonal,
    i32 mobility,
    i32 pathfindingLevel,
    i32 useRoad,
    i32 usePathfinding
) {
    i32 baseCost;
    i32 roadCost;

    if (mobility < giTerrainCost[IDX(terrain)][pathfindingLevel][1]) {
        baseCost = giTerrainCost[IDX(terrain)][pathfindingLevel][0];
        if (mobility < baseCost) {
            if (useRoad == 0)
                goto terrainCost;
            roadCost = giTerrainCost[IDX(TERRAIN_ROAD)][pathfindingLevel][0];
            if (mobility < roadCost)
                goto pathfindingCost;
        }
        if (useRoad != 0)
            return giTerrainCost[IDX(TERRAIN_ROAD)][pathfindingLevel][0];
        return baseCost;
    } else {
    pathfindingCost:
        if (useRoad != 0 && usePathfinding != 0)
            terrain = TERRAIN_ROAD;
    }

terrainCost:
    return giTerrainCost[IDX(terrain)][pathfindingLevel][diagonal & SEARCH_DIAGONAL_COST_MASK];
}

VA(0x004a4c20, 0x267)
void searchArray::PushPoint(
    i32 x,
    i32 y,
    H2_ENUM_PARAM(MapDirection, i32) direction,
    i32 cost,
    i32 mobility,
    i32 unknownFlag,
    i32 rvFlag1,
    i32 valueX,
    i32 valueY,
    i32 rvFlag2,
    i32 previousX,
    i32 previousY
) {
    if (cost > mobility && mobility > 0)
        return;
    if (x >= 0 && x <= MAP_WIDTH - 1 && y >= 0 && y <= MAP_HEIGHT - 1
        && m_queueCount < SEARCH_QUEUE_CAPACITY) {
        gSearchLow = 0;
        gSearchHigh = m_queueCount;
        gSearchCell = &GetNode(x, y);
        if (!gSearchCell->visited
            || ((gSearchCell->rvFlag1 || !rvFlag1)
                && (cost < gSearchCell->distance || (gSearchCell->rvFlag1 && !rvFlag1)))) {

            for (;;) {
                gSearchMiddle = (gSearchHigh + gSearchLow) >> 1;
                gSearchQueueNode = &m_queue[gSearchMiddle];
                if (gSearchLow >= gSearchHigh)
                    break;
                if (cost < gSearchQueueNode->distance)
                    gSearchLow = gSearchMiddle + 1;
                else
                    gSearchHigh = gSearchMiddle;
            }

            if (static_cast<u32>(gSearchMiddle) < m_queueCount) {
                memmove(
                    gSearchQueueNode + 1,
                    gSearchQueueNode,
                    (m_queueCount - gSearchMiddle) * sizeof(searchNode)
                );
            }
            m_queueCount++;

            if (giCurTempMobility < cost && rvFlag2 == 0) {
                gSearchQueueNode->rvFlag2 = 1;
                gSearchQueueNode->previousX =
                    static_cast<i8>(x - normalDirTable[IDX(direction)].x);
                gSearchQueueNode->previousY =
                    static_cast<i8>(y - normalDirTable[IDX(direction)].y);
            } else {
                gSearchQueueNode->rvFlag2 = static_cast<u8>(rvFlag2);
                gSearchQueueNode->previousX = static_cast<i8>(previousX);
                gSearchQueueNode->previousY = static_cast<i8>(previousY);
            }
            gSearchQueueNode->x = static_cast<u8>(x);
            gSearchQueueNode->y = static_cast<u8>(y);
            gSearchQueueNode->direction = static_cast<u8>(IDX(direction));
            gSearchQueueNode->distance = static_cast<u16>(cost);
            gSearchQueueNode->unknownFlag = static_cast<u8>(unknownFlag);
            gSearchQueueNode->rvFlag1 = static_cast<u8>(rvFlag1);
            gSearchQueueNode->valueX = static_cast<i8>(valueX);
            gSearchQueueNode->valueY = static_cast<i8>(valueY);
            gSearchQueueNode->visited = 1;
            *gSearchCell = *gSearchQueueNode;
        }
    }
}

VA(0x004a4e90, 0x36f)
void searchArray::TestPossibleDirections(
    i32 x,
    i32 y,
    H2_ENUM_STORAGE(TerrainType, i8) * const terrain,
    i8* const occupied,
    i32 allowOccupied,
    i32 waterMode
) {
    TerrainType invalidTerrain = TERRAIN_INVALID;

    memset(occupied, 0, SEARCH_DIRECTION_COUNT);
    gSearchCurrentCell = gpAdvManager->GetCell(x, y);

    gSearchDirection = 0;
    do {
        gSearchNextX = x + normalDirTable[gSearchDirection].x;
        gSearchNextY = y + normalDirTable[gSearchDirection].y;
        if (gSearchNextX < 0 || MAP_WIDTH <= gSearchNextX || gSearchNextY < 0
            || MAP_HEIGHT <= gSearchNextY)
            goto invalidDirection;

        gSearchNextCell = gpAdvManager->GetCell(gSearchNextX, gSearchNextY);
        if ((gSearchNextCell->m_flags & SEARCH_CELL_UNREACHABLE) != 0
            || (gbHumanPlayer[giCurPlayer] != 0
                && (giCurPlayerBit & mapExtra[gSearchNextY * MAP_WIDTH + gSearchNextX]) == 0))
            goto invalidDirection;

        if (HAS(gSearchNextCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
            if (!allowOccupied) {
                if (m_specialTargetX != gSearchNextX || m_specialTargetY != gSearchNextY)
                    goto invalidDirection;
            } else {
                occupied[gSearchDirection] = 1;
            }
        }

        gSearchTerrain = giGroundToTerrain[gSearchNextCell->m_terrainImageIndex];
        if (gSearchTerrain == TERRAIN_WATER) {
            if (waterMode != 0) {
                if (gSearchNextCell->m_triggerType
                    == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT))
                    goto invalidDirection;
                if (giGroundToTerrain[gSearchCurrentCell->m_terrainImageIndex]
                        == TERRAIN_WATER
                    && normalDirTable[gSearchDirection].x != 0
                    && normalDirTable[gSearchDirection].y != 0) {
                    if (giGroundToTerrain
                            [gpAdvManager
                                 ->GetCell(x + normalDirTable[gSearchDirection].x, y)
                                 ->m_terrainImageIndex]
                            != TERRAIN_WATER
                        || giGroundToTerrain
                               [gpAdvManager
                                    ->GetCell(x, y + normalDirTable[gSearchDirection].y)
                                    ->m_terrainImageIndex]
                               != TERRAIN_WATER)
                        goto invalidDirection;
                }
            } else {
                if (gSearchNextCell->m_triggerType
                        != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                    && gSearchNextCell->m_triggerType
                           != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT)
                    && gSearchNextCell->m_triggerType
                           != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK))
                    goto invalidDirection;
            }
        } else if (waterMode != 0 && gSearchNextCell->m_triggerType != MAP_OBJECT_COAST) {
            goto invalidDirection;
        }

    testObjects:
        if (((1U << gSearchDirection) & SEARCH_DIRECTION_EDGE_OBJECT_MASK) != 0) {
            if ((gSearchCurrentCell->m_objectIndex != SEARCH_NO_OBJECT
                 && (gSearchCurrentCell->m_objTypeBits & SEARCH_OBJECT_TYPE_MASK)
                        != SEARCH_BLOCKING_OBJECT_TYPE
                 && (gSearchCurrentCell->m_flags & SEARCH_CELL_BLOCKED) == 0))
                goto invalidDirection;
            if (gSearchNextCell->m_overlayIndex != SEARCH_NO_OBJECT) {
                mapCell* belowNext = gpAdvManager->GetCell(gSearchNextX, gSearchNextY + 1);
                if (belowNext->m_objectIndex != SEARCH_NO_OBJECT
                    && (belowNext->m_objTypeBits & SEARCH_OBJECT_TYPE_MASK)
                           != SEARCH_BLOCKING_OBJECT_TYPE
                    && (belowNext->m_flags & SEARCH_CELL_BLOCKED) == 0)
                    goto invalidDirection;
            }
        } else if (((1U << gSearchDirection) & SEARCH_DIRECTION_OBJECT_MASK) != 0) {
            if (gSearchNextCell->m_objectIndex == SEARCH_NO_OBJECT
                || (gSearchNextCell->m_objTypeBits & SEARCH_OBJECT_TYPE_MASK)
                       == SEARCH_BLOCKING_OBJECT_TYPE
                || (gSearchNextCell->m_flags & SEARCH_CELL_BLOCKED) != 0
                || (HAS(gSearchNextCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)
                    && (gSearchTriggerType =
                            gSearchNextCell->m_triggerType & MAP_TRIGGER_TYPE_MASK,
                        StopOnTrigger(gSearchNextCell) != 0))) {
                if (gSearchCurrentCell->m_overlayIndex == SEARCH_NO_OBJECT)
                    goto storeDirection;
                mapCell* belowCurrent = gpAdvManager->GetCell(x, y + 1);
                if (belowCurrent->m_objectIndex == SEARCH_NO_OBJECT
                    || (belowCurrent->m_objTypeBits & SEARCH_OBJECT_TYPE_MASK)
                           == SEARCH_BLOCKING_OBJECT_TYPE
                    || (belowCurrent->m_flags & SEARCH_CELL_BLOCKED) != 0)
                    goto storeDirection;
            }
            goto invalidDirection;
        }

        goto storeDirection;
    invalidDirection:
        gSearchTerrain = invalidTerrain;
    storeDirection:
        terrain[gSearchDirection] = gSearchTerrain;
        gSearchDirection++;
    } while (gSearchDirection < SEARCH_DIRECTION_COUNT);
}

VA(0x004a5200, 0x1ed)
void searchArray::SeedCombatPosition(class army* unit) {
    i32 hex;

    for (hex = 0; hex < COMBAT_HEX_COUNT; hex++)
        gpCombatManager->m_hexCells[hex].m_pathReachable = 0;

    if (HAS(unit->m_monster.attributes, MONSTER_ATTRIBUTE_FLYING) != 0) {
        for (hex = 0; hex < COMBAT_HEX_COUNT; hex++) {
            if (unit->CanFit(hex, 0, NULL))
                gpCombatManager->m_hexCells[hex].m_pathReachable = 1;
        }
    } else {
        for (hex = 0; hex < COMBAT_HEX_COUNT; hex++) {
            if (unit->ValidPath(hex, ARMY_PATH_EXACT_TARGET_HEX))
                gpCombatManager->m_hexCells[hex].m_pathReachable = 1;
        }
    }

    for (i32 index = 0;
         index < gpCombatManager->m_armyCount[IDX(OppositeCombatSide(unit->m_side))];
         index++) {
        army* enemy =
            &gpCombatManager->m_armies[IDX(OppositeCombatSide(unit->m_side))][index];
        unit->m_targetSide = enemy->m_side;
        unit->m_targetIndex = enemy->m_index;
        hex = enemy->m_hex;

        if (unit->m_monster.speed <= 0
            || unit->GetAttackMask(unit->m_hex, ARMY_ATTACK_TARGET_ENEMY, ARMY_HEX_INVALID)
                != ATTACK_MASK_SURROUNDED) {
            if (unit->ValidPath(hex, ARMY_PATH_EXACT_TARGET_HEX) == 1)
                gpCombatManager->m_hexCells[hex].m_pathReachable = 1;
        } else {
            gpCombatManager->m_hexCells[hex].m_pathReachable = 1;
        }

        if (HAS(enemy->m_monster.attributes, MONSTER_ATTRIBUTE_WIDE) != 0) {
            hex = enemy->GetAdjacentCellIndex(
                hex,
                enemy->m_facing == ARMY_FACING_RIGHT ? COMBAT_DIRECTION_EAST
                                                      : COMBAT_DIRECTION_WEST
            );
            if ((unit->m_monster.speed > 0
                 && unit->GetAttackMask(
                        unit->m_hex, ARMY_ATTACK_TARGET_ENEMY, ARMY_HEX_INVALID
                    )
                     == ATTACK_MASK_SURROUNDED)
                || unit->ValidPath(hex, ARMY_PATH_EXACT_TARGET_HEX) == 1) {
                gpCombatManager->m_hexCells[hex].m_pathReachable = 1;
            }
        }
    }
    unit->m_targetIndex = -1;
    unit->m_targetSide = COMBAT_SIDE_NONE;
}

VA(0x004a53f0, 0x402)
i32 searchArray::FindCombatPath(
    i32 sourceHex,
    i32 targetHex,
    class army* unit,
    ArmyPathTarget attackPath,
    i32 ignoreTargetMoat
) {
    u8 savedMoatState[KB_MOAT_CELL_COUNT];
    memset(bIsMoatSlowed, 0, sizeof(bIsMoatSlowed));

    if (gpCombatManager->m_drawbridgeBackgroundVisible != 0) {
        i32 moatIndex;
        i32 sourceWideHex = -1;
        i32 targetWideHex = -1;
        if (HAS(unit->m_monster.attributes, MONSTER_ATTRIBUTE_WIDE) != 0) {
            i32 offset = unit->m_facing == ARMY_FACING_RIGHT ? 1 : -1;
            sourceWideHex = unit->m_hex + offset;
            targetWideHex = targetHex + offset;
        }

        for (moatIndex = 0; moatIndex < KB_MOAT_CELL_COUNT; moatIndex++) {
            i32 moatHex = moatCell[moatIndex];
            savedMoatState[moatIndex] = gpCombatManager->m_hexCells[moatHex].m_blocked;
            if ((moatIndex != DRAWBRIDGE_MOAT_INDEX
                 || gpCombatManager->m_drawbridgeState == COMBAT_DRAWBRIDGE_RAISED)
                && ((targetHex != moatHex && targetWideHex != moatHex) || ignoreTargetMoat != 0)
                && (unit->m_hex != moatHex && sourceWideHex != moatHex)) {
                bIsMoatSlowed[moatHex] = 1;
            }
        }
    }

    i32 bestDistance = INITIAL_BEST_DISTANCE;
    i32 bestHex = -1;
    i32 attackTargetHex =
        attackPath != ARMY_PATH_ANY_TARGET_HEX ? targetHex : ARMY_HEX_INVALID;
    memset(m_queue, 0, sizeof(m_queue));
    memset(m_storage.cells, 0, MAP_WIDTH * MAP_HEIGHT * sizeof(searchCell));
    m_pathLength = 0;
    m_queueCount = 0;

    u8* path;
    if (ValidHex(sourceHex) && ValidHex(targetHex) && unit != NULL) {
        path = m_storage.aiPath.directions;
        u32 attackMask;
        i32 attackDirection;
        i32 currentHex;
        searchNode node;
        PushCombatPoint(
            sourceHex,
            unit->m_facing == ARMY_FACING_LEFT ? COMBAT_DIRECTION_WEST : COMBAT_DIRECTION_EAST,
            0,
            unit->m_monster.speed
        );

        while (m_queueCount != 0) {
            node = m_queue[--m_queueCount];
            if (node.distance > unit->m_monster.speed)
                continue;

            currentHex = node.x;
            i32 xDistance =
                abs(gpCombatManager->m_hexCells[currentHex].m_x
                    - gpCombatManager->m_hexCells[targetHex].m_x);
            i32 yDistance =
                abs(gpCombatManager->m_hexCells[currentHex].m_y
                    - gpCombatManager->m_hexCells[targetHex].m_y);
            i32 distance = ApproximateGridDistance(xDistance, yDistance);

            if (unit->m_targetSide != COMBAT_SIDE_NONE) {
                attackMask =
                    unit->GetAttackMask(currentHex, ARMY_ATTACK_TARGET_ASSIGNED, attackTargetHex);
                if (attackMask != ATTACK_MASK_SURROUNDED) {
                    attackDirection = 0;
                    goto findAttackDirection;
                }
            }

            if (distance < bestDistance) {
                bestHex = currentHex;
                bestDistance = distance;
                if (distance == 0)
                    break;
            }

            u32 moveMask = unit->GetMoveMask(currentHex);
            for (CombatHexDirection direction = COMBAT_DIRECTION_NORTHEAST;
                 IDX(direction) < SEARCH_DIRECTION_COUNT;
                 direction++) {
                if ((moveMask & BIT(direction)) == 0) {
                    i32 nextHex = unit->GetAdjacentCellIndex(currentHex, direction);
                    i32 moatCost = bIsMoatSlowed[nextHex]
                        ? unit->m_speed + MOAT_MOVEMENT_PENALTY
                        : 0;
                    PushCombatPoint(
                        nextHex,
                        direction,
                        node.distance + moatCost + 1,
                        unit->m_monster.speed
                    );
                }
            }
        }
        goto searchComplete;

    findAttackDirection:
        if ((attackMask & (1U << attackDirection)) == 0) {
            *path++ = static_cast<i8>(attackDirection);
            m_pathLength++;
            bestHex = node.x;
            goto searchComplete;
        }
        attackDirection++;
        if (attackDirection >= SEARCH_DIRECTION_COUNT)
            goto searchComplete;
        goto findAttackDirection;

    searchComplete:
        if (unit->m_targetSide != COMBAT_SIDE_NONE) {
            if (m_pathLength == 0)
                goto restoreMoatFailure;
        } else if (bestHex != targetHex) {
            goto restoreMoatFailure;
        }
        goto reconstructPath;
    }

restoreMoatFailure:
    attackTargetHex = 0;
restoreMoat:
    if (gpCombatManager->m_drawbridgeBackgroundVisible != 0) {
        for (i32 moatIndex = 0; moatIndex < KB_MOAT_CELL_COUNT; moatIndex++)
            gpCombatManager->m_hexCells[moatCell[moatIndex]].m_blocked =
                savedMoatState[moatIndex];
    }
    return attackTargetHex;

reconstructPath:
    while (bestHex != sourceHex) {
        searchNode& cell = m_storage.nodes[bestHex];
        *path++ = static_cast<i8>(cell.direction);
        m_pathLength++;
        if (m_pathLength >= SEARCH_PATH_CAPACITY)
            break;
        CombatHexDirection opposite =
            OppositeDirection(static_cast<CombatHexDirection>(cell.direction));
        bestHex = unit->GetAdjacentCellIndex(bestHex, opposite);
    }
    attackTargetHex = m_pathLength;
    goto restoreMoat;
}

VA(0x004a5800, 0x100)
void searchArray::PushCombatPoint(
    i32 hex, H2_ENUM_PARAM(CombatHexDirection, i32) direction, i32 distance, i32 speed
) {
    if (ValidHex(hex)) {
        i32 low = 0;
        i32 high = m_queueCount;
        searchNode* cell;
        if (speed <= 0 || distance <= speed) {
            cell = &m_storage.nodes[hex];
            if ((!cell->visited || distance < cell->distance)
                && m_queueCount < SEARCH_QUEUE_CAPACITY) {
                i32 middle;
                searchNode* node;

                for (;;) {
                    middle = (low + high) / BINARY_SEARCH_MIDPOINT_DIVISOR;
                    node = &m_queue[middle];
                    if (high <= low)
                        break;
                    if (distance < node->distance)
                        low = middle + 1;
                    else
                        high = middle;
                }
                if (middle < m_queueCount) {
                    memmove(node + 1, node, (m_queueCount - middle) * sizeof(searchNode));
                }
                m_queueCount++;
                if (m_maxQueueCount < m_queueCount)
                    m_maxQueueCount = m_queueCount;

                node->x = static_cast<u8>(hex);
                node->y = 0;
                node->direction = static_cast<u8>(direction);
                node->distance = static_cast<u16>(distance);

                cell->visited = 1;
                cell->direction = static_cast<u8>(direction);
                cell->distance = static_cast<u16>(distance);
            }
        }
    }
}

DATA(0x0052adc8) u8 bIsMoatSlowed[SEARCH_COMBAT_HEX_COUNT];
