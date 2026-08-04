#include <va.h>
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
static H2_ENUM_STORAGE(MapObjectType, i32) s_triggerType;
static i32 s_processedPointCount;
static i32 s_remainingMobility;
static i32 s_adjacentX;
static H2_ENUM_STORAGE(TerrainType, i8) s_possibleDirections[SEARCH_DIRECTION_COUNT];
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
static H2_ENUM_STORAGE_STEPPED(MapDirection, i32) s_direction;
static i32 s_targetStepCost;
static i32 s_candidateY;
static searchNode* s_neighborNode;
static H2_ENUM_STORAGE(TerrainType, i32) s_terrain;
static searchNode s_currentNode;
static i32 s_hasTarget;
static i8 s_directionCosts[SEARCH_DIRECTION_COUNT];
static hero* s_currentHero;

VA(0x004916c0, 0x116)
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
        destinationX += normalDirTable[IDX(backDir)].x;
        destinationY += normalDirTable[IDX(backDir)].y;
    }
    return m_pathLength;
}

VA(0x004917d6, 0xcc2)
void searchArray::SeedPosition(
    i32 seedX,
    i32 seedY,
    H2_ENUM_PARAM(MapDirection, i32) seedDirection,
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
    H2_ENUM_STORAGE(TerrainType, i32) targetTerrain;

    if (!continueSeed) {
        giCurTempMobility = mobility;
        giFullySeeded = 0;
        Clear();
        m_lastX = SEARCH_INVALID_COORDINATE;
        m_lastY = SEARCH_INVALID_COORDINATE;
        s_currentCost = 0;
    }

    giSeedingValid = 1;
    if (targetX >= 0) {
        if (!(*(mapExtra + targetX + MAP_WIDTH * targetY) & giCurPlayerBit)) {
            return;
        }

        s_targetCell = gpAdvManager->GetCell(targetX, targetY);
        if (s_targetCell->m_flags & IDX(MAP_CELL_OCCUPIED))
            return;

        targetTerrain = giGroundToTerrain[s_targetCell->m_terrainImageIndex];
        if (targetTerrain == TERRAIN_WATER) {
            if (waterMode) {
                if (s_targetCell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT))
                    return;
            } else {
                if (s_targetCell->m_triggerType
                        != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                    && s_targetCell->m_triggerType != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT)
                    && s_targetCell->m_triggerType
                           != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK))
                    return;
            }
        }

        s_hasTarget = 1;
        s_targetWater = s_targetCell->m_isRoad;
        s_bestTargetCost = SEARCH_MAX_COST;
    } else {
        s_hasTarget = 0;
    }

    if (s_hasTarget && continueSeed) {
        s_currentNode = GetColumn(targetX)[MAP_WIDTH * targetY];
        if (s_currentNode.visited
            && s_currentNode.distance <= s_currentCost + SEARCH_TARGET_COST_WINDOW)
            return;
    }

    if (!continueSeed)
        PushPoint(seedX, seedY, seedDirection, 0, maximumCost, 0, 0, 0, 0, 0, 0, 0);

    s_currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);

    while (m_queueCount > 0) {
        --m_queueSize;
        s_currentNode = m_queue[m_queueSize];

        if (s_hasTarget && s_bestTargetCost < SEARCH_MAX_COST
            && s_currentNode.distance + SEARCH_TARGET_COST_WINDOW >= s_bestTargetCost) {
            s_currentCost = s_currentNode.distance;
            ++m_queueSize;
            return;
        }

        if (s_currentNode.distance > maximumCost && maximumCost > 0)
            goto point_complete;

        {
            if (s_currentNode.rvFlag1) {
                s_hasAdjacentMonster = 1;
                s_adjacentMonsterX = s_currentNode.adjacentMonsterX;
                s_adjacentY = s_currentNode.adjacentMonsterY;
            } else {
                s_hasAdjacentMonster = 0;
            }

            if (s_currentNode.unknownFlag) {
                s_triggerType = gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y)
                                    ->m_triggerType
                    & MAP_TRIGGER_TYPE_MASK;
                if (s_triggerType == MAP_OBJECT_MONSTER
                    || s_triggerType == MAP_OBJECT_HERO_INTERACTION
                    || s_triggerType == MAP_OBJECT_BOAT) {
                    if (!findAdjacentMonster || s_currentNode.rvFlag1)
                        goto point_complete;
                    s_hasAdjacentMonster = 1;
                    s_adjacentMonsterX = s_currentNode.x;
                    s_adjacentY = s_currentNode.y;
                    if (s_triggerType == MAP_OBJECT_HERO_INTERACTION
                        && gpGame->m_availableHeroes
                               [gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y)
                                    ->m_objectMetadata]
                            == giCurPlayer)
                        goto point_complete;
                } else {
                    if (s_triggerType == MAP_OBJECT_STONE_LITHS
                        || s_triggerType == MAP_OBJECT_WHIRLPOOL)
                        goto point_complete;
                    if (!findAdjacentMonster || s_currentNode.rvFlag1)
                        goto point_complete;
                    if (StopOnTrigger(gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y)))
                        goto point_complete;
                }
            }

        expand_directions:
            if (waterMode) {
                s_triggerType =
                    gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y)->m_triggerType;
                if (s_triggerType == MAP_OBJECT_COAST)
                    goto point_complete;
            } else {
                if ((*(mapExtra + s_currentNode.x + s_currentNode.y * MAP_WIDTH)
                     & SEARCH_MAP_BLOCKED)
                    && (s_currentNode.x != seedX || s_currentNode.y != seedY)) {
                    if (!findAdjacentMonster || s_currentNode.rvFlag1)
                        goto point_complete;
                    if (s_currentNode.rvFlag1) {
                        if (gpAdvManager->FindAdjacentMonster(
                                s_currentNode.x,
                                s_currentNode.y,
                                &s_adjacentMonsterX,
                                &s_adjacentY,
                                s_currentNode.adjacentMonsterX,
                                s_currentNode.adjacentMonsterY
                            ))
                            goto point_complete;
                    } else if (gpAdvManager->FindAdjacentMonster(
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

            TestPossibleDirections(
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
            s_remainingMobility = giCurTempMobility - s_currentNode.distance;
            for (s_direction = MAP_DIRECTION_NORTH; s_direction < MAP_DIRECTION_COUNT;
                 ++s_direction) {
                if (s_possibleDirections[IDX(s_direction)] == TERRAIN_INVALID)
                    continue;
                {
                    s_neighborX = s_currentNode.x + normalDirTable[IDX(s_direction)].x;
                    s_neighborY = s_currentNode.y + normalDirTable[IDX(s_direction)].y;
                    s_neighborNode = &GetColumn(s_neighborX)[MAP_WIDTH * s_neighborY];
                    if (!(!findAdjacentMonster || s_currentNode.rvFlag1
                          || !(*(mapExtra + s_neighborX + MAP_WIDTH * s_neighborY)
                               & SEARCH_MAP_BLOCKED)
                          || !s_neighborNode->visited || !s_neighborNode->rvFlag1
                          || s_neighborNode->distance
                                 >= s_currentNode.distance + SEARCH_MONSTER_RESEED_WINDOW
                          || !gpAdvManager->FindAdjacentMonster(
                              s_neighborX,
                              s_neighborY,
                              &s_adjacentMonsterX,
                              &s_adjacentY,
                              SEARCH_INVALID_COORDINATE,
                              SEARCH_INVALID_COORDINATE
                          )
                          || s_neighborNode->adjacentMonsterX != s_adjacentMonsterX
                          || s_neighborNode->adjacentMonsterY != s_adjacentY))
                        continue;
                    {
                        PushPoint(
                            s_neighborX,
                            s_neighborY,
                            s_direction,
                            s_currentNode.distance
                                + CalcTerrainCost(
                                    s_terrain,
                                    IDX(s_direction),
                                    s_remainingMobility,
                                    pathfindingSkill,
                                    s_currentWater,
                                    gpAdvManager->GetCell(s_neighborX, s_neighborY)->m_isRoad
                                ),
                            maximumCost,
                            s_directionCosts[IDX(s_direction)],
                            s_hasAdjacentMonster,
                            s_adjacentMonsterX,
                            s_adjacentY,
                            s_currentNode.rvFlag2,
                            s_currentNode.previousFlags,
                            s_currentNode.terrain
                        );

                        if (s_hasTarget && s_neighborX == targetX && s_neighborY == targetY
                            && !s_currentNode.rvFlag1) {
                            s_targetStepCost = CalcTerrainCost(
                                s_possibleDirections[IDX(s_direction)],
                                IDX(s_direction),
                                giCurTempMobility - s_currentNode.distance,
                                pathfindingSkill,
                                gpAdvManager->GetCell(s_neighborX, s_neighborY)->m_isRoad,
                                s_targetWater
                            );
                            if (s_currentNode.distance + s_targetStepCost < s_bestTargetCost)
                                s_bestTargetCost = s_currentNode.distance + s_targetStepCost;
                        }
                    }
                }
            }
        }

    point_complete:
        s_processedPointCount++;
    }

    if (scanMap) {
        for (s_mapX = 0; s_mapX < MAP_WIDTH; ++s_mapX) {
            {
                for (s_mapY = 0; s_mapY < MAP_WIDTH; ++s_mapY) {
                    {
                        if ((gpAdvManager->GetCell(s_mapX, s_mapY)->m_triggerType
                             & MAP_TRIGGER_TYPE_MASK)
                            == MAP_OBJECT_MONSTER) {
                            s_targetCell = gpAdvManager->GetCell(s_mapX, s_mapY);
                            for (s_direction = MAP_DIRECTION_NORTH;
                                 s_direction < MAP_DIRECTION_COUNT;
                                 ++s_direction) {
                                s_adjacentX = s_mapX + normalDirTable[IDX(s_direction)].x;
                                s_candidateY = s_mapY + normalDirTable[IDX(s_direction)].y;
                                if (!(s_adjacentX >= 0 && s_adjacentX < MAP_WIDTH
                                      && s_candidateY >= 0 && s_candidateY < MAP_HEIGHT))
                                    continue;
                                {
                                    s_neighborCell =
                                        gpAdvManager->GetCell(s_adjacentX, s_candidateY);
                                    s_directionBlocked = 1;
                                    if (((1 << IDX(s_direction)) & SEARCH_DIRECTION_OBJECT_MASK) != 0
                                        && s_neighborCell->m_objectIndex != SEARCH_NO_OBJECT
                                        && s_neighborCell->m_objectTileset != TILESET_DUMMY
                                        && !(s_neighborCell->m_flags & SEARCH_CELL_BLOCKED)) {
                                        s_directionBlocked = 0;
                                    }

                                    if (s_directionBlocked
                                        && GetColumn(s_adjacentX)[MAP_WIDTH * s_candidateY]
                                               .visited
                                        && !(s_neighborCell->m_triggerType
                                             & MAP_TRIGGER_ACTION_FLAG)) {
                                        s_terrain =
                                            giGroundToTerrain[s_neighborCell->m_terrainImageIndex];
                                        s_adjacentCost =
                                            GetColumn(s_adjacentX)[MAP_WIDTH * s_candidateY]
                                                .distance;
                                        PushPoint(
                                            s_mapX,
                                            s_mapY,
                                            (s_direction + MAP_DIRECTION_OPPOSITE_OFFSET)
                                                & MAP_DIRECTION_INDEX_MASK,
                                            s_adjacentCost
                                                + CalcTerrainCost(
                                                    s_terrain,
                                                    IDX(s_direction),
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
                            }
                        }
                    }
                }
            }
        }
    }
    giFullySeeded = 1;
}
