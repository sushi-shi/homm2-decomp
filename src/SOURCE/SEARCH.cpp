#include <match.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/FINDPATH.h>
#include <SOURCE/KB.h>
#include <SOURCE/philAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/playerData.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/kbTypes.h>

#if H2_RETAIL_COMPILER
#define backDirection backDir
#endif
VA(0x004916c0, 0x116)
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
        destinationX += normalDirTable[IDX(backDirection)].x;
        destinationY += normalDirTable[IDX(backDirection)].y;
    }
    return m_pathLength;
}
#if H2_RETAIL_COMPILER
#undef backDirection
#endif

// SeedPosition's scratch statics are emitted in VC6's .bss hash order
// (ascending key of each decorated name); these spellings give retail's order.
#if H2_RETAIL_COMPILER
#define s_hasTarget s_hasTarget_a
#define s_hasAdjacentMonster sk_hasAdjacentMonster
#define s_neighborX s_neighborX_o
#define s_remainingMobility s_remainingMobility_8
#define s_targetHasRoad s_targetWater_6
#define s_currentHasRoad s_currentWater
#define s_mapY s_mapY_9
#define s_directionOpen s_directionBlocked_i
#define s_neighborY sg_neighborY
#define s_targetCell s1_targetCell
#define s_directionOccupied s2_directionCosts
#define s_currentCost sr_currentCost
#define s_adjacentY s_candidateY_q
#define s_mapX s_mapX_o
#define s_adjacentMonsterX s_adjacentMonsterX_g
#define s_targetStepCost s_targetStepCost_9
#define s_direction sj_direction
#define s_processedPointCount s_processedPointCount_m
#define s_neighborNode sq_neighborNode
#define s_bestTargetCost s_bestTargetCost_g
#define s_adjacentMonsterY s_adjacentY_b
#define s_terrain s_terrain_3
#define s_currentHero s_currentHero_4
#define s_directionTerrain s_possibleDirections
#define seedMonsterCells scanMap
#endif
VA(0x004917d6, 0xcc2)
void searchArray::SeedPosition(
    i32 seedX,
    i32 seedY,
    H2_ENUM_PARAM(MapDirection, i32) seedDirection,
    i32 maximumCost,
    i32 waterMode,
    b32 findAdjacentMonster,
    i32 mobility,
    i32 pathfindingSkill,
    i32 targetX,
    i32 targetY,
    b32 continueSeed,
    b32 seedMonsterCells
) {
    DATA(0x00533da0) static b32 s_hasTarget;
    DATA(0x00533da4) static b32 s_hasAdjacentMonster;
    DATA(0x00533da8) static H2_ENUM_STORAGE(TerrainType, i8) s_directionTerrain[IDX(MAP_DIRECTION_COUNT)];
    DATA(0x00533db0) static i32 s_neighborX;
    DATA(0x00533db4) static i32 s_remainingMobility;
    // Retail 0x00533db8: no code reads this cell. VC6 allocates an unreferenced
    // local static in the hash-ordered run but emits no symbol for it.
    static i32 H2_UNUSED(s_unusedInt);
    DATA(0x00533dbc) static b32 s_targetHasRoad;
    DATA(0x00533dc0) static i32 s_adjacentCost;
    DATA(0x00533dc4) static b32 s_currentHasRoad;
    DATA(0x00533dc8) static i32 s_adjacentX;
    DATA(0x00533dd0) static searchNode s_currentNode;
    DATA(0x00533ddc) static i32 s_mapY;
    DATA(0x00533de0) static b32 s_directionOpen;
    DATA(0x00533de4) static i32 s_neighborY;
    DATA(0x00533de8) static mapCell* s_targetCell;
    DATA(0x00533dec) static i8 s_directionOccupied[IDX(MAP_DIRECTION_COUNT)];
    DATA(0x00533df4) static i32 s_currentCost;
    DATA(0x00533df8) static mapCell* s_neighborCell;
    DATA(0x00533dfc) static i32 s_adjacentY;
    DATA(0x00533e00) static i32 s_mapX;
    DATA(0x00533e04) static H2_ENUM_STORAGE(MapObjectType, i32) s_triggerType;
    DATA(0x00533e08) static i32 s_adjacentMonsterX;
    DATA(0x00533e0c) static i32 s_targetStepCost;
    DATA(0x00533e10) static H2_ENUM_STORAGE_STEPPED(MapDirection, i32) s_direction;
    DATA(0x00533e14) static i32 H2_UNUSED(s_processedPointCount);
    DATA(0x00533e18) static searchNode* s_neighborNode;
    DATA(0x00533e1c) static i32 s_bestTargetCost;
    DATA(0x00533e20) static i32 s_adjacentMonsterY;
    DATA(0x00533e24) static H2_ENUM_STORAGE(TerrainType, i32) s_terrain;
    DATA(0x00533e28) static hero* H2_UNUSED(s_currentHero);

    H2_ENUM_STORAGE(TerrainType, i32) targetTerrain;

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
        if (s_targetCell->m_flags & IDX(MAP_CELL_OCCUPIED))
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
        s_targetHasRoad = s_targetCell->m_isRoad;
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
        PushPoint(seedX, seedY, seedDirection, 0, maximumCost, 0, false, 0, 0, false, 0, 0);

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
            if (s_currentNode.hasAdjacentMonster) {
                s_hasAdjacentMonster = true;
                s_adjacentMonsterX = s_currentNode.adjacentMonsterX;
                s_adjacentMonsterY = s_currentNode.adjacentMonsterY;
            } else {
                s_hasAdjacentMonster = false;
            }

            if (s_currentNode.occupied) {
                s_triggerType = gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y)
                                    ->m_triggerType
                    & MAP_TRIGGER_TYPE_MASK;
                if (s_triggerType == MAP_OBJECT_MONSTER
                    || s_triggerType == MAP_OBJECT_HERO_INTERACTION
                    || s_triggerType == MAP_OBJECT_BOAT) {
                    if (!findAdjacentMonster || s_currentNode.hasAdjacentMonster)
                        goto point_complete;
                    s_hasAdjacentMonster = true;
                    s_adjacentMonsterX = s_currentNode.x;
                    s_adjacentMonsterY = s_currentNode.y;
                    if (s_triggerType == MAP_OBJECT_HERO_INTERACTION
                        && gpGame->m_heroOwners
                               [gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y)
                                    ->m_objectMetadata]
                            == giCurPlayer)
                        goto point_complete;
                } else {
                    if (s_triggerType == MAP_OBJECT_STONE_LITHS
                        || s_triggerType == MAP_OBJECT_WHIRLPOOL)
                        goto point_complete;
                    if (!findAdjacentMonster || s_currentNode.hasAdjacentMonster)
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
                     & IDX(MAP_EXTRA_ADJACENT_MONSTER))
                    && (s_currentNode.x != seedX || s_currentNode.y != seedY)) {
                    if (!findAdjacentMonster || s_currentNode.hasAdjacentMonster)
                        goto point_complete;
                    if (s_currentNode.hasAdjacentMonster) {
                        if (gpAdvManager->FindAdjacentMonster(
                                s_currentNode.x,
                                s_currentNode.y,
                                &s_adjacentMonsterX,
                                &s_adjacentMonsterY,
                                s_currentNode.adjacentMonsterX,
                                s_currentNode.adjacentMonsterY
                            ))
                            goto point_complete;
                    } else if (gpAdvManager->FindAdjacentMonster(
                                   s_currentNode.x,
                                   s_currentNode.y,
                                   &s_adjacentMonsterX,
                                   &s_adjacentMonsterY,
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
                s_directionTerrain,
                s_directionOccupied,
                1,
                waterMode
            );
            s_terrain = CELL_TERRAIN(gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y));
            s_currentHasRoad = gpAdvManager->GetCell(s_currentNode.x, s_currentNode.y)->m_isRoad;
            s_remainingMobility = giCurTempMobility - s_currentNode.distance;
            for (s_direction = MAP_DIRECTION_NORTH; s_direction < MAP_DIRECTION_COUNT;
                 ++s_direction) {
                if (s_directionTerrain[IDX(s_direction)] == TERRAIN_INVALID)
                    continue;
                {
                    s_neighborX = s_currentNode.x + normalDirTable[IDX(s_direction)].x;
                    s_neighborY = s_currentNode.y + normalDirTable[IDX(s_direction)].y;
                    s_neighborNode = &GetColumn(s_neighborX)[MAP_WIDTH * s_neighborY];
                    if (!(!findAdjacentMonster || s_currentNode.hasAdjacentMonster
                          || !(MAP_EXTRA_AT(s_neighborX, s_neighborY)
                               & IDX(MAP_EXTRA_ADJACENT_MONSTER))
                          || !s_neighborNode->visited || !s_neighborNode->hasAdjacentMonster
                          || s_neighborNode->distance
                                 >= s_currentNode.distance + SEARCH_MONSTER_RESEED_WINDOW
                          || !gpAdvManager->FindAdjacentMonster(
                              s_neighborX,
                              s_neighborY,
                              &s_adjacentMonsterX,
                              &s_adjacentMonsterY,
                              SEARCH_INVALID_COORDINATE,
                              SEARCH_INVALID_COORDINATE
                          )
                          || s_neighborNode->adjacentMonsterX != s_adjacentMonsterX
                          || s_neighborNode->adjacentMonsterY != s_adjacentMonsterY))
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
                                    s_currentHasRoad,
                                    gpAdvManager->GetCell(s_neighborX, s_neighborY)->m_isRoad
                                ),
                            maximumCost,
                            s_directionOccupied[IDX(s_direction)],
                            s_hasAdjacentMonster,
                            s_adjacentMonsterX,
                            s_adjacentMonsterY,
                            s_currentNode.beyondTurnMobility,
                            s_currentNode.turnEndX,
                            s_currentNode.turnEndY
                        );

                        if (s_hasTarget && s_neighborX == targetX && s_neighborY == targetY
                            && !s_currentNode.hasAdjacentMonster) {
                            s_targetStepCost = CalcTerrainCost(
                                s_directionTerrain[IDX(s_direction)],
                                IDX(s_direction),
                                giCurTempMobility - s_currentNode.distance,
                                pathfindingSkill,
                                gpAdvManager->GetCell(s_neighborX, s_neighborY)->m_isRoad,
                                s_targetHasRoad
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

    if (seedMonsterCells) {
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
                                s_adjacentY = s_mapY + normalDirTable[IDX(s_direction)].y;
                                if (!(s_adjacentX >= 0 && s_adjacentX < MAP_WIDTH
                                      && s_adjacentY >= 0 && s_adjacentY < MAP_HEIGHT))
                                    continue;
                                {
                                    s_neighborCell =
                                        gpAdvManager->GetCell(s_adjacentX, s_adjacentY);
                                    s_directionOpen = true;
                                    if (((1 << IDX(s_direction)) & MAP_DIRECTIONS_SOUTHWARD) != 0
                                        && CELL_HAS_NON_SHADOW_OBJECT(s_neighborCell)) {
                                        s_directionOpen = false;
                                    }

                                    if (s_directionOpen
                                        && GetColumn(s_adjacentX)[MAP_WIDTH * s_adjacentY]
                                               .visited
                                        && !(s_neighborCell->m_triggerType
                                             & MAP_TRIGGER_ACTION_FLAG)) {
                                        s_terrain =
                                            CELL_TERRAIN(s_neighborCell);
                                        s_adjacentCost =
                                            GetColumn(s_adjacentX)[MAP_WIDTH * s_adjacentY]
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
                                            false,
                                            SEARCH_INVALID_COORDINATE,
                                            SEARCH_INVALID_COORDINATE,
                                            false,
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
#if H2_RETAIL_COMPILER
#undef s_hasTarget
#undef s_hasAdjacentMonster
#undef s_neighborX
#undef s_remainingMobility
#undef s_targetHasRoad
#undef s_currentHasRoad
#undef s_mapY
#undef s_directionOpen
#undef s_neighborY
#undef s_targetCell
#undef s_directionOccupied
#undef s_currentCost
#undef s_adjacentY
#undef s_mapX
#undef s_adjacentMonsterX
#undef s_targetStepCost
#undef s_direction
#undef s_processedPointCount
#undef s_neighborNode
#undef s_bestTargetCost
#undef s_adjacentMonsterY
#undef s_terrain
#undef s_currentHero
#undef s_directionTerrain
#undef seedMonsterCells
#endif
