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
#include <SOURCE/KB_TYPES.h>


static i32 s_currentWater;
static i32 s_candidateY;
static i32 s_adjacentY;
static i32 s_adjacentX;
static mapCell* s_targetCell;
static i32 s_targetStepCost;
static H2EnumStorage<MapObjectType, i32> s_triggerType;
static mapCell* s_neighborCell;
static b32 s_hasAdjacentMonster;
static H2EnumStorage<TerrainType, i32> s_terrain;
static searchNode s_currentNode;
static i32 s_neighborY;
static i32 s_neighborX;
static i32 s_targetWater;
static i32 s_adjacentMonsterX;
static i32 s_remainingMobility;
static b32 s_directionBlocked;
static searchNode* s_neighborNode;
static i32 s_adjacentCost;
static i32 s_mapX;
static i32 s_mapY;
static i8 s_directionCosts[H2EnumIndex(MAP_DIRECTION_COUNT)];
static i32 s_currentCost;
static b32 s_hasTarget;
static i32 s_processedPointCount;
static hero* s_currentHero;
static H2EnumStorage<TerrainType, i8> s_possibleDirections[H2EnumIndex(MAP_DIRECTION_COUNT)];
static i32 s_bestTargetCost;
static H2SteppedEnumStorage<MapDirection, i32> s_direction;

i32 searchArray::BuildPath(
    i32 startX,
    i32 startY,
    i32 destinationX,
    i32 destinationY,
    i32 maximumCost
) {
    u8* pathDirection = m_storage.directions;
    m_pathLength = 0;
    while (destinationX != startX || destinationY != startY) {
        searchNode* node = &GetNode(destinationX, destinationY);
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
        MapDirection backDirection =
            OppositeMapDirection(static_cast<MapDirection>(node->direction));
        destinationX += normalDirTable[H2EnumIndex(backDirection)].x;
        destinationY += normalDirTable[H2EnumIndex(backDirection)].y;
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
    H2EnumStorage<TerrainType, i32> targetTerrain;

    if (!continueSeed) {
        giCurTempMobility = mobility;
        giFullySeeded = false;
        Clear();
        m_specialTargetY = SEARCH_INVALID_COORDINATE;
        m_specialTargetX = SEARCH_INVALID_COORDINATE;
        s_currentCost = 0;
    }

    giSeedingValid = true;
    if (targetX >= 0) {
        if (!(MAP_EXTRA_AT_WFIRST(targetX, targetY) & giCurPlayerBit)) {
            return;
        }

        s_targetCell = gpAdvManager->GetCell(targetX, targetY);
        if (s_targetCell->m_flags & H2EnumIndex(MAP_CELL_OCCUPIED))
            return;

        targetTerrain = CELL_TERRAIN(s_targetCell);
        if (targetTerrain == TERRAIN_WATER) {
            if (waterMode) {
                if (s_targetCell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_BOAT)))
                    return;
            } else {
                if (s_targetCell->m_triggerType
                        != (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))
                    && s_targetCell->m_triggerType != (MAP_ACTION_TRIGGER(MAP_OBJECT_BOAT))
                    && s_targetCell->m_triggerType
                           != (MAP_ACTION_TRIGGER(MAP_OBJECT_SHIPWRECK)))
                    return;
            }
        }

        s_hasTarget = true;
        s_targetWater = s_targetCell->m_isRoad;
        s_bestTargetCost = SEARCH_MAX_COST;
    } else {
        s_hasTarget = false;
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
        --m_queueCount;
        s_currentNode = m_queue[m_queueCount];

        if (s_hasTarget && s_bestTargetCost < SEARCH_MAX_COST
            && s_currentNode.distance + SEARCH_TARGET_COST_WINDOW >= s_bestTargetCost) {
            s_currentCost = s_currentNode.distance;
            ++m_queueCount;
            return;
        }

        if (s_currentNode.distance > maximumCost && maximumCost > 0)
            goto point_complete;

        {
            if (s_currentNode.rvFlag1) {
                s_hasAdjacentMonster = true;
                s_adjacentMonsterX = s_currentNode.adjacentMonsterX;
                s_adjacentY = s_currentNode.adjacentMonsterY;
            } else {
                s_hasAdjacentMonster = false;
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
                    s_hasAdjacentMonster = true;
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
                        s_hasAdjacentMonster = true;
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
            s_terrain = CELL_TERRAIN(gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y));
            s_currentWater = gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y)->m_isRoad;
            s_remainingMobility = giCurTempMobility - s_currentNode.distance;
            for (s_direction = MAP_DIRECTION_NORTH; s_direction < MAP_DIRECTION_COUNT;
                 ++s_direction) {
                if (s_possibleDirections[H2EnumIndex(s_direction)] == TERRAIN_INVALID)
                    continue;
                {
                    s_neighborX = s_currentNode.x + normalDirTable[H2EnumIndex(s_direction)].x;
                    s_neighborY = s_currentNode.y + normalDirTable[H2EnumIndex(s_direction)].y;
                    s_neighborNode = &GetColumn(s_neighborX)[MAP_WIDTH * s_neighborY];
                    if (!(!findAdjacentMonster || s_currentNode.rvFlag1
                          || !(MAP_EXTRA_AT(s_neighborX, s_neighborY)
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

                        if (s_hasTarget && s_neighborX == targetX && s_neighborY == targetY
                            && !s_currentNode.rvFlag1) {
                            s_targetStepCost = CalcTerrainCost(
                                s_possibleDirections[H2EnumIndex(s_direction)],
                                H2EnumIndex(s_direction),
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
                                s_adjacentX = s_mapX + normalDirTable[H2EnumIndex(s_direction)].x;
                                s_candidateY = s_mapY + normalDirTable[H2EnumIndex(s_direction)].y;
                                if (!(s_adjacentX >= 0 && s_adjacentX < MAP_WIDTH
                                      && s_candidateY >= 0 && s_candidateY < MAP_HEIGHT))
                                    continue;
                                {
                                    s_neighborCell =
                                        gpAdvManager->GetCell(s_adjacentX, s_candidateY);
                                    s_directionBlocked = true;
                                    if (((1 << H2EnumIndex(s_direction)) & SEARCH_DIRECTION_OBJECT_MASK) != 0
                                        && CELL_HAS_NON_SHADOW_OBJECT(s_neighborCell)) {
                                        s_directionBlocked = false;
                                    }

                                    if (s_directionBlocked
                                        && GetColumn(s_adjacentX)[MAP_WIDTH * s_candidateY]
                                               .visited
                                        && !(s_neighborCell->m_triggerType
                                             & MAP_TRIGGER_ACTION_FLAG)) {
                                        s_terrain =
                                            CELL_TERRAIN(s_neighborCell);
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
                            }
                        }
                    }
                }
            }
        }
    }
    giFullySeeded = true;
}
