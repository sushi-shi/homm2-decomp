#include <Ints.h>
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
#include <SOURCE/CMBTMGR.h>
#include <EDITOR/mapcell.h>
#include <stdlib.h>
#include <string.h>

namespace {

typedef enum FindPathConstant {
    ATTACK_MASK_SURROUNDED         = 0xff,
    DISTANCE_MINOR_DIVISOR         = 2,
    BINARY_SEARCH_MIDPOINT_DIVISOR = 2,
    DRAWBRIDGE_MOAT_INDEX          = 4,
    MOAT_MOVEMENT_PENALTY          = 2,
    INITIAL_BEST_DISTANCE          = COMBAT_SCREEN_WIDTH
} FindPathConstant;

}

static i32 gSearchNextY = 0;
static mapCell* gSearchNextCell = NULL;
static H2EnumStorage<MapObjectType, i32> gSearchTriggerType = 0;
static i32 gSearchNextX = 0;
static i32 gSearchDirection = 0;
static mapCell* gSearchCurrentCell = NULL;
static H2EnumStorage<TerrainType, i32> gSearchTerrain = 0;
static i32 gSearchMiddle = 0;
static searchNode* gSearchCell = NULL;
static searchNode* gSearchQueueNode = NULL;
static i32 gSearchHigh = 0;
static i32 gSearchLow = 0;

searchArray::searchArray(void) {
    m_storage.cells = NULL;
    m_maxQueueCount = 0;
}

searchArray::~searchArray() {
    Close();
}

void searchArray::Init(void) {
    Close();
    m_storage.cells =
        static_cast<searchCell*>(H2_ALLOC(MAP_WIDTH * MAP_HEIGHT * sizeof(searchCell)));
}

void searchArray::Close(void) {
    if (m_storage.cells != NULL)
        H2_FREE(m_storage.cells);
    m_storage.cells = NULL;
}

void searchArray::Clear(void) {
    memset(m_queue, 0, sizeof(m_queue));
    memset(m_storage.cells, 0, MAP_WIDTH * sizeof(searchCell) * MAP_HEIGHT);
    m_pathLength = 0;
    m_queueCount = 0;
}

i32 searchArray::QuickDistance(i32 x1, i32 y1, i32 x2, i32 y2) {
    i32 xDistance = abs(x1 - x2);
    i32 yDistance = abs(y1 - y2);

    return xDistance < yDistance ? yDistance + xDistance / DISTANCE_MINOR_DIVISOR
                                 : xDistance + yDistance / DISTANCE_MINOR_DIVISOR;
}

i32 CalcTerrainCost(
    TerrainType terrain,
    i32 diagonal,
    i32 mobility,
    i32 pathfindingLevel,
    i32 useRoad,
    i32 usePathfinding
) {
    if (mobility < giTerrainCost[H2EnumIndex(terrain)][pathfindingLevel][1]) {
        if (mobility >= giTerrainCost[H2EnumIndex(terrain)][pathfindingLevel][0]
            || (useRoad != 0
                && mobility >= giTerrainCost[H2EnumIndex(TERRAIN_ROAD)][pathfindingLevel][0])) {
            if (useRoad != 0)
                return giTerrainCost[H2EnumIndex(TERRAIN_ROAD)][pathfindingLevel][0];
            return giTerrainCost[H2EnumIndex(terrain)][pathfindingLevel][0];
        }
    }
    if (useRoad != 0 && usePathfinding != 0)
        terrain = TERRAIN_ROAD;
    return giTerrainCost[H2EnumIndex(terrain)][pathfindingLevel][diagonal & SEARCH_DIAGONAL_COST_MASK];
}

void searchArray::PushPoint(
    i32 x,
    i32 y,
    MapDirection direction,
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
    if (x < 0 || x > MAP_WIDTH - 1 || y < 0 || y > MAP_HEIGHT - 1)
        return;
    if (m_queueCount >= SEARCH_QUEUE_CAPACITY)
        return;

    gSearchHigh = m_queueCount;
    gSearchLow = 0;
    gSearchCell = &GetNode(x, y);
    if (gSearchCell->visited) {
        if (!gSearchCell->rvFlag1 && rvFlag1)
            return;
        if (gSearchCell->distance <= cost && (!gSearchCell->rvFlag1 || rvFlag1))
            return;
    }

    for (;;) {
        gSearchMiddle = (gSearchHigh + gSearchLow) >> 1;
        gSearchQueueNode = &m_queue[gSearchMiddle];
        if (gSearchHigh <= gSearchLow)
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

    if (cost > giCurTempMobility && rvFlag2 == 0) {
        gSearchQueueNode->rvFlag2 = 1;
        gSearchQueueNode->previousX = static_cast<i8>(x - normalDirTable[H2EnumIndex(direction)].x);
        gSearchQueueNode->previousY = static_cast<i8>(y - normalDirTable[H2EnumIndex(direction)].y);
    } else {
        gSearchQueueNode->rvFlag2 = static_cast<u8>(rvFlag2);
        gSearchQueueNode->previousX = static_cast<i8>(previousX);
        gSearchQueueNode->previousY = static_cast<i8>(previousY);
    }
    gSearchQueueNode->x = static_cast<u8>(x);
    gSearchQueueNode->y = static_cast<u8>(y);
    gSearchQueueNode->direction = static_cast<u8>(H2EnumIndex(direction));
    gSearchQueueNode->distance = static_cast<u16>(cost);
    gSearchQueueNode->unknownFlag = static_cast<u8>(unknownFlag);
    gSearchQueueNode->rvFlag1 = static_cast<u8>(rvFlag1);
    gSearchQueueNode->valueX = static_cast<i8>(valueX);
    gSearchQueueNode->valueY = static_cast<i8>(valueY);
    gSearchQueueNode->visited = 1;
    *gSearchCell = *gSearchQueueNode;
}

void searchArray::TestPossibleDirections(
    i32 x,
    i32 y,
    H2EnumStorage<TerrainType, i8> * const terrain,
    i8* const occupied,
    i32 allowOccupied,
    i32 waterMode
) {

    memset(occupied, 0, SEARCH_DIRECTION_COUNT);
    gSearchCurrentCell = gpAdvManager->GetCell(x, y);

    for (gSearchDirection = 0; gSearchDirection < SEARCH_DIRECTION_COUNT; gSearchDirection++) {
        gSearchNextX = x + normalDirTable[gSearchDirection].x;
        gSearchNextY = y + normalDirTable[gSearchDirection].y;
        if (gSearchNextX < 0 || gSearchNextX >= MAP_WIDTH || gSearchNextY < 0
            || gSearchNextY >= MAP_HEIGHT) {
            gSearchTerrain = TERRAIN_INVALID;
            goto storeDirection;
        }

        gSearchNextCell = gpAdvManager->GetCell(gSearchNextX, gSearchNextY);
        if ((gSearchNextCell->m_flags & SEARCH_CELL_UNREACHABLE) != 0) {
            gSearchTerrain = TERRAIN_INVALID;
            goto storeDirection;
        }
        if (gbHumanPlayer[giCurPlayer] != 0
            && (MAP_EXTRA_AT_WFIRST(gSearchNextX, gSearchNextY) & giCurPlayerBit) == 0) {
            gSearchTerrain = TERRAIN_INVALID;
            goto storeDirection;
        }

        if ((H2EnumIndex((gSearchNextCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
            if (!allowOccupied) {
                if (gSearchNextX != m_specialTargetX || gSearchNextY != m_specialTargetY) {
                    gSearchTerrain = TERRAIN_INVALID;
                    goto storeDirection;
                }
            } else {
                occupied[gSearchDirection] = 1;
            }
        }

        gSearchTerrain = giGroundToTerrain[gSearchNextCell->m_terrainImageIndex];
        if (gSearchTerrain == TERRAIN_WATER) {
            if (waterMode != 0) {
                if (gSearchNextCell->m_triggerType
                    == (MAP_ACTION_TRIGGER(MAP_OBJECT_BOAT))) {
                    gSearchTerrain = TERRAIN_INVALID;
                    goto storeDirection;
                }
                if (giGroundToTerrain[gSearchCurrentCell->m_terrainImageIndex] == TERRAIN_WATER
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
                               != TERRAIN_WATER) {
                        gSearchTerrain = TERRAIN_INVALID;
                        goto storeDirection;
                    }
                }
            } else {
                if (gSearchNextCell->m_triggerType
                        != (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))
                    && gSearchNextCell->m_triggerType
                           != (MAP_ACTION_TRIGGER(MAP_OBJECT_BOAT))
                    && gSearchNextCell->m_triggerType
                           != (MAP_ACTION_TRIGGER(MAP_OBJECT_SHIPWRECK))) {
                    gSearchTerrain = TERRAIN_INVALID;
                    goto storeDirection;
                }
            }
        } else if (waterMode != 0 && gSearchNextCell->m_triggerType != MAP_OBJECT_COAST) {
            gSearchTerrain = TERRAIN_INVALID;
            goto storeDirection;
        }

        if (((1U << gSearchDirection) & SEARCH_DIRECTION_EDGE_OBJECT_MASK) != 0) {
            if (gSearchCurrentCell->m_objectIndex != SEARCH_NO_OBJECT
                && gSearchCurrentCell->ObjectTileset() != TILESET_DUMMY
                && (gSearchCurrentCell->m_flags & SEARCH_CELL_BLOCKED) == 0) {
                gSearchTerrain = TERRAIN_INVALID;
                goto storeDirection;
            }
            if (gSearchNextCell->m_overlayIndex != SEARCH_NO_OBJECT) {
                mapCell* belowNext = gpAdvManager->GetCell(gSearchNextX, gSearchNextY + 1);

                if (belowNext->m_objectIndex != SEARCH_NO_OBJECT
                    && belowNext->ObjectTileset() != TILESET_DUMMY
                    && (belowNext->m_flags & SEARCH_CELL_BLOCKED) == 0) {
                    gSearchTerrain = TERRAIN_INVALID;
                    goto storeDirection;
                }
            }
        } else if (((1U << gSearchDirection) & SEARCH_DIRECTION_OBJECT_MASK) != 0) {
            if (gSearchNextCell->m_objectIndex != SEARCH_NO_OBJECT
                && gSearchNextCell->ObjectTileset() != TILESET_DUMMY
                && (gSearchNextCell->m_flags & SEARCH_CELL_BLOCKED) == 0) {
                if ((H2EnumIndex((gSearchNextCell->m_triggerType) & (MAP_TRIGGER_ACTION_FLAG)))) {
                    gSearchTriggerType = gSearchNextCell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
                    if (!StopOnTrigger(gSearchNextCell)) {
                        gSearchTerrain = TERRAIN_INVALID;
                        goto storeDirection;
                    }
                } else {
                    gSearchTerrain = TERRAIN_INVALID;
                    goto storeDirection;
                }
            }
            if (gSearchCurrentCell->m_overlayIndex != SEARCH_NO_OBJECT) {
                mapCell* belowCurrent = gpAdvManager->GetCell(x, y + 1);

                if (belowCurrent->m_objectIndex != SEARCH_NO_OBJECT
                    && belowCurrent->ObjectTileset() != TILESET_DUMMY
                    && (belowCurrent->m_flags & SEARCH_CELL_BLOCKED) == 0) {
                    gSearchTerrain = TERRAIN_INVALID;
                    goto storeDirection;
                }
            }
        }

    storeDirection:
        terrain[gSearchDirection] = gSearchTerrain;
    }
}
