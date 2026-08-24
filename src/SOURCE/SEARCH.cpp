#include <Ints.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/FINDPATH.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/playerData.h>
#include <SOURCE/searchArray.h>

static i32 s_directionBlocked;
static H2EnumStorage<MapObjectType, i32> s_triggerType;
static i32 s_processedPointCount;
static i32 s_remainingMobility;
static i32 s_adjacentX;
static H2EnumStorage<TerrainType, i8> s_possibleDirections[SEARCH_DIRECTION_COUNT];
static i32 s_targetWater;
static i32 s_neighborX;
static i32 s_mapY;
static i32 s_currentCost;
static mapCell* s_neighborCell;
static i32 s_adjacentY;
static mapCell* s_targetCell;
static i32 s_currentWater;
static i32 s_bestTargetCost;
static i32 s_adjacentMonsterX;
static i32 s_adjacentCost;
static i32 s_mapX;
static i32 s_neighborY;
static i32 s_hasAdjacentMonster;
static H2SteppedEnumStorage<MapDirection, i32> s_direction;
static i32 s_targetStepCost;
static i32 s_candidateY;
static searchNode* s_neighborNode;
static H2EnumStorage<TerrainType, i32> s_terrain;
static searchNode s_currentNode;
static i32 s_hasTarget;
static i8 s_directionCosts[SEARCH_DIRECTION_COUNT];
static hero* s_currentHero;

i32 searchArray::BuildPath(
    i32 startX,
    i32 startY,
    i32 destinationX,
    i32 destinationY,
    i32 maximumCost
) {
     i32 currentDestinationX = destinationX;
     i32 currentDestinationY = destinationY;
    u8* pathDirection = &m_storage.path.directions[1];
    m_pathLength = 0;
    while (startX != currentDestinationX || startY != currentDestinationY) {
        searchNode* node = m_storage.nodes + currentDestinationY * MAP_WIDTH + currentDestinationX;
        if (node->x != currentDestinationX && node->y != currentDestinationY)
            return 0;
        if (node->distance <= maximumCost) {
            *pathDirection = node->direction;
            ++pathDirection;
            ++m_pathLength;
            if (m_pathLength >= SEARCH_PATH_CAPACITY) {
                m_pathLength = 0;
                break;
            }
        }
        MapDirection reverseDirection =
            OppositeMapDirection(static_cast<MapDirection>(node->direction));
        currentDestinationX += normalDirTable[H2EnumIndex(reverseDirection)].x;
        currentDestinationY += normalDirTable[H2EnumIndex(reverseDirection)].y;
    }
    return m_pathLength;
}

void searchArray::SeedPosition(
    i32 seedX,
    i32 seedY,
    MapDirection seedDirection,
    i32 maximumCost,
    i32 waterMode,
    i32 findAdjacentMonster,
    i32 mobility,
    i32 pathfindingSkill,
    i32 targetX,
    i32 targetY,
    i32 continueSeed,
    i32 scanMap
) {
     i32 continuing = continueSeed;
     i32 targetColumn = targetX;
    searchArray* const search = this;

    if (!continuing) {
        giFullySeeded = 0;
        giCurTempMobility = mobility;
        search->Clear();
        search->m_lastX = SEARCH_INVALID_COORDINATE;
        search->m_lastY = SEARCH_INVALID_COORDINATE;
        s_currentCost = 0;
    }

    giSeedingValid = 1;
    if (targetColumn >= 0) {
        i32 targetIndex = MAP_WIDTH * targetY + targetColumn;
        if (!(mapExtra[targetIndex] & giCurPlayerBit)) {
            return;
        }

        s_targetCell = gpAdvManager->GetCell(targetColumn, targetY);
        if (s_targetCell->m_flags & H2EnumIndex(MAP_CELL_OCCUPIED))
            return;

        if (giGroundToTerrain[s_targetCell->m_terrainImageIndex] == TERRAIN_WATER) {
            if (waterMode) {
                if (s_targetCell->m_triggerType
                    == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT))
                    return;
            } else {
                H2EnumStorage<MapObjectType, u8> targetTrigger = s_targetCell->m_triggerType;
                if (targetTrigger != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                    && targetTrigger != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT)
                    && targetTrigger != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK))
                    return;
            }
        }

        s_hasTarget = 1;
        s_targetWater = s_targetCell->m_isRoad;
        s_bestTargetCost = SEARCH_MAX_COST;
    } else {
        s_hasTarget = 0;
    }

    if (!s_hasTarget) {
    continue_check:
        if (continuing)
            goto seed_loop;
    } else if (continuing) {
        s_currentNode = search->m_storage.nodes[MAP_WIDTH * targetY + targetColumn];
        if (s_currentNode.visited
            && s_currentCost + SEARCH_TARGET_COST_WINDOW >= s_currentNode.distance)
            return;
        goto continue_check;
    }

    search->PushPoint(seedX, seedY, seedDirection, 0, maximumCost, 0, 0, 0, 0, 0, 0, 0);

seed_loop:
    s_currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);

    while (search->m_queueSize != 0) {
        --search->m_queueSize;
        s_currentNode = search->m_queue[search->m_queueSize];

        if (s_hasTarget && s_bestTargetCost < SEARCH_MAX_COST
            && s_bestTargetCost <= s_currentNode.distance + SEARCH_TARGET_COST_WINDOW) {
            s_currentCost = s_currentNode.distance;
            ++search->m_queueSize;
            return;
        }

        if (s_currentNode.distance > maximumCost && maximumCost > 0)
            goto point_complete;

        {
            mapCell* currentCell;
            if (s_currentNode.rvFlag1) {
                s_hasAdjacentMonster = 1;
                s_adjacentMonsterX = s_currentNode.adjacentMonsterX;
                s_adjacentY = s_currentNode.adjacentMonsterY;
            } else {
                s_hasAdjacentMonster = 0;
            }

            if (!s_currentNode.unknownFlag)
                goto expand_directions;

            currentCell = gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y);
            s_triggerType = currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
            if (s_triggerType == MAP_OBJECT_MONSTER
                || s_triggerType == MAP_OBJECT_HERO_INTERACTION
                || s_triggerType == MAP_OBJECT_BOAT) {
                if (findAdjacentMonster && !s_currentNode.rvFlag1) {
                    s_adjacentMonsterX = s_currentNode.x;
                    s_adjacentY = s_currentNode.y;
                    s_hasAdjacentMonster = 1;
                    if (s_triggerType != MAP_OBJECT_HERO_INTERACTION
                        || gpGame->m_availableHeroes[gpAdvManager
                                                         ->GetCell(s_adjacentMonsterX, s_adjacentY)
                                                         ->m_objectMetadata]
                               != giCurPlayer)
                        goto expand_directions;
                }
            } else if (s_triggerType != MAP_OBJECT_STONE_LITHS
                       && s_triggerType != MAP_OBJECT_WHIRLPOOL && findAdjacentMonster
                       && !s_currentNode.rvFlag1) {
                if (StopOnTrigger(gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y)))
                    goto point_complete;
                goto expand_directions;
            }
            goto point_complete;

        expand_directions:
            if (waterMode) {
                s_triggerType =
                    gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y)->m_triggerType;
                if (s_triggerType == MAP_OBJECT_COAST)
                    goto point_complete;
            } else {
                if ((mapExtra[MAP_WIDTH * s_currentNode.y + s_currentNode.x] & SEARCH_MAP_BLOCKED)
                    && (seedX != s_currentNode.x || seedY != s_currentNode.y)) {
                    if (!findAdjacentMonster || s_currentNode.rvFlag1)
                        goto point_complete;
                    if (gpAdvManager->FindAdjacentMonster(
                            s_currentNode.x,
                            s_currentNode.y,
                            &s_adjacentMonsterX,
                            &s_adjacentY,
                            SEARCH_INVALID_COORDINATE,
                            SEARCH_INVALID_COORDINATE
                        )) {
                        s_hasAdjacentMonster = 1;
                    }
                }
            }

            search->TestPossibleDirections(
                s_currentNode.x,
                s_currentNode.y,
                s_possibleDirections,
                s_directionCosts,
                1,
                waterMode
            );
            s_terrain = giGroundToTerrain[gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y)
                                              ->m_terrainImageIndex];
            s_currentWater = gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y)->m_isRoad;
            s_direction = MAP_DIRECTION_NORTH;
            s_remainingMobility = giCurTempMobility - s_currentNode.distance;
            do {
                if (s_possibleDirections[H2EnumIndex(s_direction)] != TERRAIN_INVALID) {
                    s_neighborX = normalDirTable[H2EnumIndex(s_direction)].x + s_currentNode.x;
                    s_neighborY = normalDirTable[H2EnumIndex(s_direction)].y + s_currentNode.y;
                    i32 neighborIndex = MAP_WIDTH * s_neighborY + s_neighborX;
                    s_neighborNode = &search->m_storage.nodes[neighborIndex];
                    if (!findAdjacentMonster || s_currentNode.rvFlag1
                        || !(mapExtra[neighborIndex] & SEARCH_MAP_BLOCKED)
                        || !s_neighborNode->visited || !s_neighborNode->rvFlag1
                        || s_currentNode.distance + SEARCH_MONSTER_RESEED_WINDOW
                               <= s_neighborNode->distance
                        || !gpAdvManager->FindAdjacentMonster(
                            s_neighborX,
                            s_neighborY,
                            &s_adjacentMonsterX,
                            &s_adjacentY,
                            SEARCH_INVALID_COORDINATE,
                            SEARCH_INVALID_COORDINATE
                        )
                        || s_neighborNode->adjacentMonsterX != s_adjacentMonsterX
                        || s_neighborNode->adjacentMonsterY != s_adjacentY) {
                        search->PushPoint(
                            s_neighborX,
                            s_neighborY,
                            s_direction,
                            s_currentNode.distance
                                + CalcTerrainCost(
                                    s_terrain,
                                    H2EnumIndex(s_direction),
                                    s_remainingMobility,
                                    pathfindingSkill,
                                    s_currentWater,
                                    gpAdvManager->GetCell(s_neighborX, s_neighborY)->m_isRoad
                                ),
                            maximumCost,
                            s_directionCosts[H2EnumIndex(s_direction)],
                            s_hasAdjacentMonster,
                            s_adjacentMonsterX,
                            s_adjacentY,
                            s_currentNode.rvFlag2,
                            s_currentNode.previousFlags,
                            s_currentNode.terrain
                        );

                        if (s_hasTarget && targetColumn == s_neighborX && s_neighborY == targetY
                            && !s_currentNode.rvFlag1) {
                            mapCell* neighborCell = gpAdvManager->GetCell(s_neighborX, s_neighborY);
                            s_targetStepCost = CalcTerrainCost(
                                s_possibleDirections[H2EnumIndex(s_direction)],
                                H2EnumIndex(s_direction),
                                giCurTempMobility - s_currentNode.distance,
                                pathfindingSkill,
                                neighborCell->m_isRoad,
                                s_targetWater
                            );
                            i32 targetCost = s_currentNode.distance + s_targetStepCost;
                            if (targetCost < s_bestTargetCost)
                                s_bestTargetCost = targetCost;
                        }
                    }
                }
                ++s_direction;
            } while (s_direction < MAP_DIRECTION_COUNT);
        }

    point_complete:
        s_processedPointCount++;
    }

    if (scanMap) {
        s_mapX = 0;
        if (MAP_WIDTH > 0) {
            do {
                s_mapY = 0;
                if (MAP_WIDTH > 0) {
                    do {
                        mapCell* mapPosition = gpAdvManager->GetCell(s_mapX, s_mapY);
                        if ((mapPosition->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                            == MAP_OBJECT_MONSTER) {
                            s_targetCell = gpAdvManager->GetCell(s_mapX, s_mapY);
                            s_direction = MAP_DIRECTION_NORTH;
                            do {
                                s_adjacentX = normalDirTable[H2EnumIndex(s_direction)].x + s_mapX;
                                s_candidateY = normalDirTable[H2EnumIndex(s_direction)].y + s_mapY;
                                if (s_adjacentX >= 0 && s_adjacentX < MAP_WIDTH && s_candidateY >= 0
                                    && s_candidateY < MAP_HEIGHT) {
                                    s_neighborCell =
                                        gpAdvManager->GetCell(s_adjacentX, s_candidateY);
                                    s_directionBlocked = 1;
                                    if (((1 << H2EnumIndex(s_direction)) & SEARCH_DIRECTION_OBJECT_MASK) != 0
                                        && s_neighborCell->m_objectIndex != SEARCH_NO_OBJECT
                                        && (s_neighborCell->m_objTypeBits & SEARCH_OBJECT_TYPE_MASK)
                                               != SEARCH_BLOCKING_OBJECT_TYPE
                                        && !(s_neighborCell->m_flags & SEARCH_CELL_BLOCKED)) {
                                        s_directionBlocked = 0;
                                    }

                                    i32 adjacentIndex = MAP_WIDTH * s_candidateY + s_adjacentX;
                                    if (s_directionBlocked
                                        && search->m_storage.nodes[adjacentIndex].visited
                                        && !(s_neighborCell->m_triggerType
                                             & MAP_TRIGGER_ACTION_FLAG)) {
                                        s_terrain =
                                            giGroundToTerrain[s_neighborCell->m_terrainImageIndex];
                                        s_adjacentCost =
                                            search->m_storage.nodes[adjacentIndex].distance;
                                        search->PushPoint(
                                            s_mapX,
                                            s_mapY,
                                            OppositeMapDirection(s_direction),
                                            s_adjacentCost
                                                + CalcTerrainCost(
                                                    s_terrain,
                                                    H2EnumIndex(s_direction),
                                                    giCurTempMobility - s_adjacentCost,
                                                    pathfindingSkill,
                                                    s_neighborCell->m_isRoad,
                                                    s_targetCell->m_isRoad
                                                ),
                                            maximumCost,
                                            1,
                                            0,
                                            SEARCH_INVALID_COORDINATE,
                                            SEARCH_INVALID_COORDINATE,
                                            0,
                                            SEARCH_INVALID_COORDINATE,
                                            SEARCH_INVALID_COORDINATE
                                        );
                                    }
                                }
                                ++s_direction;
                            } while (s_direction < MAP_DIRECTION_COUNT);
                        }
                        ++s_mapY;
                    } while (s_mapY < MAP_WIDTH);
                }
                ++s_mapX;
            } while (s_mapX < MAP_WIDTH);
        }
    }
    giFullySeeded = 1;
}
