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

struct SeedPositionState {
    i32 hasTarget;
    i32 hasAdjacentMonster;
    i8 possibleDirections[SEARCH_DIRECTION_COUNT];
    i32 neighborX;
    i32 remainingMobility;
    u8 padding18[4];
    i32 targetWater;
    i32 adjacentCost;
    i32 currentWater;
    i32 adjacentX;
    u8 padding2c[4];
    searchNode currentNode;
    i32 mapY;
    i32 directionBlocked;
    i32 neighborY;
    mapCell* targetCell;
    i8 directionCosts[SEARCH_DIRECTION_COUNT];
    i32 currentCost;
    mapCell* neighborCell;
    i32 candidateY;
    i32 mapX;
    i32 triggerType;
    i32 adjacentMonsterX;
    i32 targetStepCost;
    i32 direction;
    i32 processedPointCount;
    searchNode* neighborNode;
    i32 bestTargetCost;
    i32 adjacentY;
    i32 terrain;
    hero* currentHero;
};


static SeedPositionState s_seedPositionState;

i32 searchArray::BuildPath(
    i32 startX,
    i32 startY,
    i32 destinationX,
    i32 destinationY,
    i32 maximumCost
) {
    u8* pathDirection = &m_storage.path.directions[1];
    m_pathLength = 0;
    while (destinationX != startX || destinationY != startY) {
        searchNode* node = &GetColumn(destinationX)[MAP_WIDTH * destinationY];
        if (node->x != destinationX && node->y != destinationY)
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
        MapDirection backDir =
            OppositeMapDirection(static_cast<MapDirection>(node->direction));
        destinationX += normalDirTable[(backDir)].x;
        destinationY += normalDirTable[(backDir)].y;
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
    i32 targetTerrain;

    if (!continueSeed) {
        giCurTempMobility = mobility;
        giFullySeeded = 0;
        Clear();
        m_lastX = SEARCH_INVALID_COORDINATE;
        m_lastY = SEARCH_INVALID_COORDINATE;
        s_seedPositionState.currentCost = 0;
    }

    giSeedingValid = 1;
    if (targetX >= 0) {
        if (!(MAP_EXTRA_AT_WFIRST(targetX, targetY) & giCurPlayerBit)) {
            return;
        }

        s_seedPositionState.targetCell = gpAdvManager->GetCell(targetX, targetY);
        if (s_seedPositionState.targetCell->m_flags & (MAP_CELL_OCCUPIED))
            return;

        targetTerrain = giGroundToTerrain[s_seedPositionState.targetCell->m_terrainImageIndex];
        if (targetTerrain == TERRAIN_WATER) {
            if (waterMode) {
                if (s_seedPositionState.targetCell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT))
                    return;
            } else {
                if (s_seedPositionState.targetCell->m_triggerType
                        != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                    && s_seedPositionState.targetCell->m_triggerType != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT)
                    && s_seedPositionState.targetCell->m_triggerType
                           != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK))
                    return;
            }
        }

        s_seedPositionState.hasTarget = 1;
        s_seedPositionState.targetWater = s_seedPositionState.targetCell->m_isRoad;
        s_seedPositionState.bestTargetCost = SEARCH_MAX_COST;
    } else {
        s_seedPositionState.hasTarget = 0;
    }

    if (s_seedPositionState.hasTarget && continueSeed) {
        s_seedPositionState.currentNode = GetColumn(targetX)[MAP_WIDTH * targetY];
        if (s_seedPositionState.currentNode.visited
            && s_seedPositionState.currentNode.distance <= s_seedPositionState.currentCost + SEARCH_TARGET_COST_WINDOW)
            return;
    }

    if (!continueSeed)
        PushPoint(seedX, seedY, seedDirection, 0, maximumCost, 0, 0, 0, 0, 0, 0, 0);

    s_seedPositionState.currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);

    while (m_queueCount > 0) {
        --m_queueSize;
        s_seedPositionState.currentNode = m_queue[m_queueSize];

        if (s_seedPositionState.hasTarget && s_seedPositionState.bestTargetCost < SEARCH_MAX_COST
            && s_seedPositionState.currentNode.distance + SEARCH_TARGET_COST_WINDOW >= s_seedPositionState.bestTargetCost) {
            s_seedPositionState.currentCost = s_seedPositionState.currentNode.distance;
            ++m_queueSize;
            return;
        }

        if (s_seedPositionState.currentNode.distance > maximumCost && maximumCost > 0)
            goto point_complete;

        {
            if (s_seedPositionState.currentNode.rvFlag1) {
                s_seedPositionState.hasAdjacentMonster = 1;
                s_seedPositionState.adjacentMonsterX = s_seedPositionState.currentNode.adjacentMonsterX;
                s_seedPositionState.adjacentY = s_seedPositionState.currentNode.adjacentMonsterY;
            } else {
                s_seedPositionState.hasAdjacentMonster = 0;
            }

            if (s_seedPositionState.currentNode.unknownFlag) {
                s_seedPositionState.triggerType = gpAdvManager->GetCell(s_seedPositionState.currentNode.x, s_seedPositionState.currentNode.y)
                                    ->m_triggerType
                    & MAP_TRIGGER_TYPE_MASK;
                if (s_seedPositionState.triggerType == MAP_OBJECT_MONSTER
                    || s_seedPositionState.triggerType == MAP_OBJECT_HERO_INTERACTION
                    || s_seedPositionState.triggerType == MAP_OBJECT_BOAT) {
                    if (!findAdjacentMonster || s_seedPositionState.currentNode.rvFlag1)
                        goto point_complete;
                    s_seedPositionState.hasAdjacentMonster = 1;
                    s_seedPositionState.adjacentMonsterX = s_seedPositionState.currentNode.x;
                    s_seedPositionState.adjacentY = s_seedPositionState.currentNode.y;
                    if (s_seedPositionState.triggerType == MAP_OBJECT_HERO_INTERACTION
                        && gpGame->m_availableHeroes
                               [gpAdvManager->GetCell(s_seedPositionState.currentNode.x, s_seedPositionState.currentNode.y)
                                    ->m_objectMetadata]
                            == giCurPlayer)
                        goto point_complete;
                } else {
                    if (s_seedPositionState.triggerType == MAP_OBJECT_STONE_LITHS
                        || s_seedPositionState.triggerType == MAP_OBJECT_WHIRLPOOL)
                        goto point_complete;
                    if (!findAdjacentMonster || s_seedPositionState.currentNode.rvFlag1)
                        goto point_complete;
                    if (StopOnTrigger(gpAdvManager->GetCell(s_seedPositionState.currentNode.x, s_seedPositionState.currentNode.y)))
                        goto point_complete;
                }
            }

        expand_directions:
            if (waterMode) {
                s_seedPositionState.triggerType =
                    gpAdvManager->GetCell(s_seedPositionState.currentNode.x, s_seedPositionState.currentNode.y)->m_triggerType;
                if (s_seedPositionState.triggerType == MAP_OBJECT_COAST)
                    goto point_complete;
            } else {
                if ((*(mapExtra + s_seedPositionState.currentNode.x + s_seedPositionState.currentNode.y * MAP_WIDTH)
                     & SEARCH_MAP_BLOCKED)
                    && (s_seedPositionState.currentNode.x != seedX || s_seedPositionState.currentNode.y != seedY)) {
                    if (!findAdjacentMonster || s_seedPositionState.currentNode.rvFlag1)
                        goto point_complete;
                    if (s_seedPositionState.currentNode.rvFlag1) {
                        if (gpAdvManager->FindAdjacentMonster(
                                s_seedPositionState.currentNode.x,
                                s_seedPositionState.currentNode.y,
                                &s_seedPositionState.adjacentMonsterX,
                                &s_seedPositionState.adjacentY,
                                s_seedPositionState.currentNode.adjacentMonsterX,
                                s_seedPositionState.currentNode.adjacentMonsterY
                            ))
                            goto point_complete;
                    } else if (gpAdvManager->FindAdjacentMonster(
                                   s_seedPositionState.currentNode.x,
                                   s_seedPositionState.currentNode.y,
                                   &s_seedPositionState.adjacentMonsterX,
                                   &s_seedPositionState.adjacentY,
                                   SEARCH_INVALID_COORDINATE,
                                   SEARCH_INVALID_COORDINATE
                               )) {
                        s_seedPositionState.hasAdjacentMonster = 1;
                    }
                }
            }

            TestPossibleDirections(
                s_seedPositionState.currentNode.x,
                s_seedPositionState.currentNode.y,
                s_seedPositionState.possibleDirections,
                s_seedPositionState.directionCosts,
                1,
                waterMode
            );
            s_seedPositionState.terrain = giGroundToTerrain[gpAdvManager->GetCell(s_seedPositionState.currentNode.x, s_seedPositionState.currentNode.y)
                                              ->m_terrainImageIndex];
            s_seedPositionState.currentWater = gpAdvManager->GetCell(s_seedPositionState.currentNode.x, s_seedPositionState.currentNode.y)->m_isRoad;
            s_seedPositionState.remainingMobility = giCurTempMobility - s_seedPositionState.currentNode.distance;
            for (s_seedPositionState.direction = MAP_DIRECTION_NORTH; s_seedPositionState.direction < MAP_DIRECTION_COUNT;
                 ++s_seedPositionState.direction) {
                if (s_seedPositionState.possibleDirections[(s_seedPositionState.direction)] == TERRAIN_INVALID)
                    continue;
                {
                    s_seedPositionState.neighborX = s_seedPositionState.currentNode.x + normalDirTable[(s_seedPositionState.direction)].x;
                    s_seedPositionState.neighborY = s_seedPositionState.currentNode.y + normalDirTable[(s_seedPositionState.direction)].y;
                    s_seedPositionState.neighborNode = &GetColumn(s_seedPositionState.neighborX)[MAP_WIDTH * s_seedPositionState.neighborY];
                    if (!(!findAdjacentMonster || s_seedPositionState.currentNode.rvFlag1
                          || !(MAP_EXTRA_AT(s_seedPositionState.neighborX, s_seedPositionState.neighborY)
                               & SEARCH_MAP_BLOCKED)
                          || !s_seedPositionState.neighborNode->visited || !s_seedPositionState.neighborNode->rvFlag1
                          || s_seedPositionState.neighborNode->distance
                                 >= s_seedPositionState.currentNode.distance + SEARCH_MONSTER_RESEED_WINDOW
                          || !gpAdvManager->FindAdjacentMonster(
                              s_seedPositionState.neighborX,
                              s_seedPositionState.neighborY,
                              &s_seedPositionState.adjacentMonsterX,
                              &s_seedPositionState.adjacentY,
                              SEARCH_INVALID_COORDINATE,
                              SEARCH_INVALID_COORDINATE
                          )
                          || s_seedPositionState.neighborNode->adjacentMonsterX != s_seedPositionState.adjacentMonsterX
                          || s_seedPositionState.neighborNode->adjacentMonsterY != s_seedPositionState.adjacentY))
                        continue;
                    {
                        PushPoint(
                            s_seedPositionState.neighborX,
                            s_seedPositionState.neighborY,
                            s_seedPositionState.direction,
                            s_seedPositionState.currentNode.distance
                                + CalcTerrainCost(
                                    s_seedPositionState.terrain,
                                    (s_seedPositionState.direction),
                                    s_seedPositionState.remainingMobility,
                                    pathfindingSkill,
                                    s_seedPositionState.currentWater,
                                    gpAdvManager->GetCell(s_seedPositionState.neighborX, s_seedPositionState.neighborY)->m_isRoad
                                ),
                            maximumCost,
                            s_seedPositionState.directionCosts[(s_seedPositionState.direction)],
                            s_seedPositionState.hasAdjacentMonster,
                            s_seedPositionState.adjacentMonsterX,
                            s_seedPositionState.adjacentY,
                            s_seedPositionState.currentNode.rvFlag2,
                            s_seedPositionState.currentNode.previousFlags,
                            s_seedPositionState.currentNode.terrain
                        );

                        if (s_seedPositionState.hasTarget && s_seedPositionState.neighborX == targetX && s_seedPositionState.neighborY == targetY
                            && !s_seedPositionState.currentNode.rvFlag1) {
                            s_seedPositionState.targetStepCost = CalcTerrainCost(
                                s_seedPositionState.possibleDirections[(s_seedPositionState.direction)],
                                (s_seedPositionState.direction),
                                giCurTempMobility - s_seedPositionState.currentNode.distance,
                                pathfindingSkill,
                                gpAdvManager->GetCell(s_seedPositionState.neighborX, s_seedPositionState.neighborY)->m_isRoad,
                                s_seedPositionState.targetWater
                            );
                            if (s_seedPositionState.currentNode.distance + s_seedPositionState.targetStepCost < s_seedPositionState.bestTargetCost)
                                s_seedPositionState.bestTargetCost = s_seedPositionState.currentNode.distance + s_seedPositionState.targetStepCost;
                        }
                    }
                }
            }
        }

    point_complete:
        s_seedPositionState.processedPointCount++;
    }

    if (scanMap) {
        for (s_seedPositionState.mapX = 0; s_seedPositionState.mapX < MAP_WIDTH; ++s_seedPositionState.mapX) {
            {
                for (s_seedPositionState.mapY = 0; s_seedPositionState.mapY < MAP_WIDTH; ++s_seedPositionState.mapY) {
                    {
                        if ((gpAdvManager->GetCell(s_seedPositionState.mapX, s_seedPositionState.mapY)->m_triggerType
                             & MAP_TRIGGER_TYPE_MASK)
                            == MAP_OBJECT_MONSTER) {
                            s_seedPositionState.targetCell = gpAdvManager->GetCell(s_seedPositionState.mapX, s_seedPositionState.mapY);
                            for (s_seedPositionState.direction = MAP_DIRECTION_NORTH;
                                 s_seedPositionState.direction < MAP_DIRECTION_COUNT;
                                 ++s_seedPositionState.direction) {
                                s_seedPositionState.adjacentX = s_seedPositionState.mapX + normalDirTable[(s_seedPositionState.direction)].x;
                                s_seedPositionState.candidateY = s_seedPositionState.mapY + normalDirTable[(s_seedPositionState.direction)].y;
                                if (!(s_seedPositionState.adjacentX >= 0 && s_seedPositionState.adjacentX < MAP_WIDTH
                                      && s_seedPositionState.candidateY >= 0 && s_seedPositionState.candidateY < MAP_HEIGHT))
                                    continue;
                                {
                                    s_seedPositionState.neighborCell =
                                        gpAdvManager->GetCell(s_seedPositionState.adjacentX, s_seedPositionState.candidateY);
                                    s_seedPositionState.directionBlocked = 1;
                                    if (((1 << (s_seedPositionState.direction)) & SEARCH_DIRECTION_OBJECT_MASK) != 0
                                        && s_seedPositionState.neighborCell->m_objectIndex != SEARCH_NO_OBJECT
                                        && s_seedPositionState.neighborCell->m_objectTileset != TILESET_DUMMY
                                        && !(s_seedPositionState.neighborCell->m_flags & SEARCH_CELL_BLOCKED)) {
                                        s_seedPositionState.directionBlocked = 0;
                                    }

                                    if (s_seedPositionState.directionBlocked
                                        && GetColumn(s_seedPositionState.adjacentX)[MAP_WIDTH * s_seedPositionState.candidateY]
                                               .visited
                                        && !(s_seedPositionState.neighborCell->m_triggerType
                                             & MAP_TRIGGER_ACTION_FLAG)) {
                                        s_seedPositionState.terrain =
                                            giGroundToTerrain[s_seedPositionState.neighborCell->m_terrainImageIndex];
                                        s_seedPositionState.adjacentCost =
                                            GetColumn(s_seedPositionState.adjacentX)[MAP_WIDTH * s_seedPositionState.candidateY]
                                                .distance;
                                        PushPoint(
                                            s_seedPositionState.mapX,
                                            s_seedPositionState.mapY,
                                            (s_seedPositionState.direction + MAP_DIRECTION_OPPOSITE_OFFSET)
                                                & MAP_DIRECTION_INDEX_MASK,
                                            s_seedPositionState.adjacentCost
                                                + CalcTerrainCost(
                                                    s_seedPositionState.terrain,
                                                    (s_seedPositionState.direction),
                                                    giCurTempMobility - s_seedPositionState.adjacentCost,
                                                    pathfindingSkill,
                                                    s_seedPositionState.neighborCell->m_isRoad,
                                                    s_seedPositionState.targetCell->m_isRoad
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
                            }
                        }
                    }
                }
            }
        }
    }
    giFullySeeded = 1;
}
