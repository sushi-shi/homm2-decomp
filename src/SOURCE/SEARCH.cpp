// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SEARCH.OBJ   from: (directly linked into exe)
// functions: 2   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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

DATA(0x0052a1d8) static i32 s_directionBlocked;
DATA(0x0052a1dc) static i32 s_triggerType;
DATA(0x0052a1e4) static i32 s_processedPointCount;
DATA(0x0052a1e8) static i32 s_remainingMobility;
DATA(0x0052a1ec) static i32 s_adjacentX;
DATA(0x0052a1f0) static i8 s_possibleDirections[SEARCH_DIRECTION_COUNT];
DATA(0x0052a1f8) static i32 s_targetWater;
DATA(0x0052a1fc) static i32 s_neighborX;
DATA(0x0052a200) static i32 s_mapY;
DATA(0x0052a204) static i32 s_currentCost;
DATA(0x0052a208) static mapCell *s_neighborCell;
DATA(0x0052a20c) static i32 s_adjacentY;
DATA(0x0052a210) static mapCell *s_targetCell;
DATA(0x0052a214) static i32 s_currentWater;
DATA(0x0052a218) static i32 s_bestTargetCost;
DATA(0x0052a21c) static i32 s_adjacentMonsterX;
DATA(0x0052a220) static i32 s_adjacentCost;
DATA(0x0052a224) static i32 s_mapX;
DATA(0x0052a228) static i32 s_neighborY;
DATA(0x0052a22c) static i32 s_hasAdjacentMonster;
DATA(0x0052a230) static i32 s_direction;
DATA(0x0052a234) static i32 s_targetStepCost;
DATA(0x0052a238) static i32 s_candidateY;
DATA(0x0052a23c) static searchNode *s_neighborNode;
DATA(0x0052a240) static i32 s_terrain;
DATA(0x0052a248) static searchNode s_currentNode;
DATA(0x0052a254) static i32 s_hasTarget;
DATA(0x0052a258) static i8 s_directionCosts[SEARCH_DIRECTION_COUNT];
DATA(0x0052a260) static hero *s_currentHero;

// @semantic
// /O2 structural checkpoint: complete loop/CFG and FPO saved-register set; base has
// 0xbc meaningful bytes versus retail 0xbc plus a four-byte trailing alignment LEA.
// First divergence is +0x0: base loads destination X into EAX before saving EBX,
// while retail saves EBX and materializes the path cursor in EAX first. Relocs are
// 3/3; retail's normalDirTable+1 relocation is delinked as the adjacent local ??_C
// alias, and MAP_WIDTH agrees. Tried explicit and register cursors, direct member
// indexing, comma sequencing, coordinate aliases, both union views, condition
// polarity, multiplication order, and shared-return CFG. Revisit in the 95% /O2 pass.
VA(0x004a25e0, 0xc0)
i32 searchArray::BuildPath(i32 startX, i32 startY, i32 destinationX,
                           i32 destinationY, i32 maximumCost)
{
    register i32 currentDestinationX = destinationX;
    register i32 currentDestinationY = destinationY;
    i8 *pathDirection = &m_storage.path.directions[1];
    m_pathLength = 0;
    while (startX != currentDestinationX || startY != currentDestinationY) {
        searchNode *node =
            m_storage.nodes + currentDestinationY * MAP_WIDTH +
                currentDestinationX;
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
        i32 reverseDirection =
            (node->direction + SEARCH_DIRECTION_REVERSE) & SEARCH_DIRECTION_MASK;
        currentDestinationX += normalDirTable[reverseDirection].x;
        currentDestinationY += normalDirTable[reverseDirection].y;
    }
    return m_pathLength;
}

// @semantic
// /O2 structural checkpoint: complete semantics, 1024x9 queue layout, stack arguments,
// call order, and bottom-tested queue CFG. The FPO prologue is exact: EBX/ESI/EDI/EBP
// are saved, this is ESI, continueSeed is EBX, and target X is EDI. Base has 0x9ea
// meaningful bytes versus retail 0x9df plus one pad byte. The first code divergence is
// target visibility indexing: base adds mapExtra before target X and tests [EAX+EDI],
// while retail adds target X first and tests [EAX+ECX]. Relocs are base 231/retail 232:
// every candidate owner/addend occurs in retail, and candidate is short one zero-addend
// s_adjacentX reference; every external global/callee count agrees. Tried direct/local target
// indexing, branch polarities, nested cost arguments, queue aliases, register/const alias
// orders, and both top- and bottom-tested queue loops. Revisit in the 95% /O2 pass.
VA(0x004a26a0, 0x9df)
void searchArray::SeedPosition(i32 seedX, i32 seedY, i32 seedDirection,
                               i32 maximumCost, i32 waterMode,
                               i32 findAdjacentMonster, i32 mobility,
                               i32 pathfindingSkill, i32 targetX, i32 targetY,
                               i32 continueSeed, i32 scanMap)
{
    register i32 continuing = continueSeed;
    register i32 targetColumn = targetX;
    searchArray * const search = this;

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
        if (s_targetCell->m_flags & 8)
            return;

        if (giGroundToTerrain[s_targetCell->m_terrainImageIndex] == 0) {
            if (waterMode) {
                if (s_targetCell->m_triggerType == SEARCH_WATER_ENTRY_SECOND)
                    return;
            } else {
                u8 targetTrigger = s_targetCell->m_triggerType;
                if (targetTrigger != SEARCH_WATER_ENTRY_FIRST &&
                    targetTrigger != SEARCH_WATER_ENTRY_SECOND &&
                    targetTrigger != SEARCH_WATER_ENTRY_THIRD)
                    return;
            }
        }

        s_hasTarget = 1;
        s_targetWater = (s_targetCell->m_objTypeBits & 2) >> 1;
        s_bestTargetCost = SEARCH_MAX_COST;
    } else {
        s_hasTarget = 0;
    }

    if (!s_hasTarget) {
continue_check:
        if (continuing)
            goto seed_loop;
    } else if (continuing) {
        s_currentNode =
            search->m_storage.nodes[MAP_WIDTH * targetY + targetColumn];
        if (s_currentNode.visited &&
            s_currentCost + SEARCH_TARGET_COST_WINDOW >= s_currentNode.distance)
            return;
        goto continue_check;
    }

    search->PushPoint(seedX, seedY, seedDirection, 0, maximumCost,
                      0, 0, 0, 0, 0, 0, 0);

seed_loop:
    s_currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);

    while (search->m_queueSize != 0) {
        --search->m_queueSize;
        s_currentNode = search->m_queue[search->m_queueSize];

        if (s_hasTarget && s_bestTargetCost < SEARCH_MAX_COST &&
            s_bestTargetCost <=
                s_currentNode.distance + SEARCH_TARGET_COST_WINDOW) {
            s_currentCost = s_currentNode.distance;
            ++search->m_queueSize;
            return;
        }

        if (s_currentNode.distance > maximumCost && maximumCost > 0)
            goto point_complete;

        {
            mapCell *currentCell;
            if (s_currentNode.rvFlag1) {
                s_hasAdjacentMonster = 1;
                s_adjacentMonsterX = s_currentNode.adjacentMonsterX;
                s_adjacentY = s_currentNode.adjacentMonsterY;
            } else {
                s_hasAdjacentMonster = 0;
            }

            if (!s_currentNode.unknownFlag)
                goto expand_directions;

            currentCell = gpAdvManager->GetCell(
                s_currentNode.x, s_currentNode.y);
            s_triggerType = currentCell->m_triggerType & SEARCH_TRIGGER_MASK;
            if (s_triggerType == SEARCH_TRIGGER_MONSTER ||
                s_triggerType == SEARCH_TRIGGER_TOWN ||
                s_triggerType == SEARCH_TRIGGER_TOWN_ALT) {
                if (findAdjacentMonster && !s_currentNode.rvFlag1) {
                    s_adjacentMonsterX = s_currentNode.x;
                    s_adjacentY = s_currentNode.y;
                    s_hasAdjacentMonster = 1;
                    if (s_triggerType != SEARCH_TRIGGER_TOWN ||
                        gpGame->m_availableHeroes[
                            gpAdvManager->GetCell(s_adjacentMonsterX,
                                                  s_adjacentY)->m_objectMetadata] != giCurPlayer)
                        goto expand_directions;
                }
            } else if (s_triggerType != SEARCH_TRIGGER_IGNORE_FIRST &&
                       s_triggerType != SEARCH_TRIGGER_IGNORE_SECOND &&
                       findAdjacentMonster && !s_currentNode.rvFlag1) {
                if (StopOnTrigger(gpAdvManager->GetCell(
                        s_currentNode.x, s_currentNode.y)))
                    goto point_complete;
                goto expand_directions;
            }
            goto point_complete;

expand_directions:
            if (waterMode) {
                s_triggerType = gpAdvManager->GetCell(
                    s_currentNode.x, s_currentNode.y)->m_triggerType;
                if (s_triggerType == SEARCH_TRIGGER_BOAT)
                    goto point_complete;
            } else {
                if ((mapExtra[MAP_WIDTH * s_currentNode.y + s_currentNode.x] &
                     SEARCH_MAP_BLOCKED) &&
                    (seedX != s_currentNode.x || seedY != s_currentNode.y)) {
                    if (!findAdjacentMonster || s_currentNode.rvFlag1)
                        goto point_complete;
                    if (gpAdvManager->FindAdjacentMonster(
                            s_currentNode.x, s_currentNode.y,
                            &s_adjacentMonsterX, &s_adjacentY,
                            SEARCH_INVALID_COORDINATE,
                            SEARCH_INVALID_COORDINATE)) {
                        s_hasAdjacentMonster = 1;
                    }
                }
            }

            search->TestPossibleDirections(s_currentNode.x, s_currentNode.y,
                                           s_possibleDirections, s_directionCosts,
                                           1, waterMode);
            s_terrain = giGroundToTerrain[gpAdvManager->GetCell(
                s_currentNode.x, s_currentNode.y)->m_terrainImageIndex];
            s_currentWater = (gpAdvManager->GetCell(
                s_currentNode.x, s_currentNode.y)->m_objTypeBits & 2) >> 1;
            s_direction = 0;
            s_remainingMobility = giCurTempMobility - s_currentNode.distance;
            do {
                if (s_possibleDirections[s_direction] != -1) {
                    s_neighborX = normalDirTable[s_direction].x + s_currentNode.x;
                    s_neighborY = normalDirTable[s_direction].y + s_currentNode.y;
                    i32 neighborIndex = MAP_WIDTH * s_neighborY + s_neighborX;
                    s_neighborNode = &search->m_storage.nodes[neighborIndex];
                    if (!findAdjacentMonster || s_currentNode.rvFlag1 ||
                        !(mapExtra[neighborIndex] & SEARCH_MAP_BLOCKED) ||
                        !s_neighborNode->visited || !s_neighborNode->rvFlag1 ||
                        s_currentNode.distance + SEARCH_MONSTER_RESEED_WINDOW <=
                            s_neighborNode->distance ||
                        !gpAdvManager->FindAdjacentMonster(
                            s_neighborX, s_neighborY,
                            &s_adjacentMonsterX, &s_adjacentY,
                            SEARCH_INVALID_COORDINATE,
                            SEARCH_INVALID_COORDINATE) ||
                        s_neighborNode->adjacentMonsterX != s_adjacentMonsterX ||
                        s_neighborNode->adjacentMonsterY != s_adjacentY) {
                        search->PushPoint(
                            s_neighborX, s_neighborY, s_direction,
                            s_currentNode.distance + CalcTerrainCost(
                                s_terrain, s_direction, s_remainingMobility,
                                pathfindingSkill, s_currentWater,
                                (gpAdvManager->GetCell(
                                    s_neighborX, s_neighborY)->m_objTypeBits & 2) >> 1),
                            maximumCost,
                            static_cast<i8>(s_directionCosts[s_direction]),
                            s_hasAdjacentMonster, s_adjacentMonsterX, s_adjacentY,
                            s_currentNode.rvFlag2, s_currentNode.previousFlags,
                            s_currentNode.terrain);

                        if (s_hasTarget && targetColumn == s_neighborX &&
                            s_neighborY == targetY && !s_currentNode.rvFlag1) {
                            mapCell *neighborCell = gpAdvManager->GetCell(
                                s_neighborX, s_neighborY);
                            s_targetStepCost = CalcTerrainCost(
                                static_cast<i8>(
                                    s_possibleDirections[s_direction]),
                                s_direction,
                                giCurTempMobility - s_currentNode.distance,
                                pathfindingSkill,
                                (neighborCell->m_objTypeBits & 2) >> 1,
                                s_targetWater);
                            i32 targetCost =
                                s_currentNode.distance + s_targetStepCost;
                            if (targetCost < s_bestTargetCost)
                                s_bestTargetCost = targetCost;
                        }
                    }
                }
                ++s_direction;
            } while (s_direction < SEARCH_DIRECTION_COUNT);
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
                        mapCell *mapPosition =
                            gpAdvManager->GetCell(s_mapX, s_mapY);
                        if ((mapPosition->m_triggerType & SEARCH_TRIGGER_MASK) ==
                            SEARCH_TRIGGER_MONSTER) {
                            s_targetCell = gpAdvManager->GetCell(s_mapX, s_mapY);
                            s_direction = 0;
                            do {
                                s_adjacentX =
                                    normalDirTable[s_direction].x + s_mapX;
                                s_candidateY =
                                    normalDirTable[s_direction].y + s_mapY;
                                if (s_adjacentX >= 0 && s_adjacentX < MAP_WIDTH &&
                                    s_candidateY >= 0 &&
                                    s_candidateY < MAP_HEIGHT) {
                                    s_neighborCell = gpAdvManager->GetCell(
                                        s_adjacentX, s_candidateY);
                                    s_directionBlocked = 1;
                                    if (((1 << s_direction) &
                                         SEARCH_DIRECTION_OBJECT_MASK) != 0 &&
                                        s_neighborCell->m_objectIndex != SEARCH_NO_OBJECT &&
                                        (s_neighborCell->m_objTypeBits &
                                         SEARCH_OBJECT_TYPE_MASK) !=
                                            SEARCH_BLOCKING_OBJECT_TYPE &&
                                        !(s_neighborCell->m_flags &
                                          SEARCH_CELL_BLOCKED)) {
                                        s_directionBlocked = 0;
                                    }

                                    i32 adjacentIndex =
                                        MAP_WIDTH * s_candidateY + s_adjacentX;
                                    if (s_directionBlocked &&
                                        search->m_storage.nodes[adjacentIndex].visited &&
                                        !(s_neighborCell->m_triggerType &
                                          SEARCH_CELL_BLOCKED)) {
                                        s_terrain = giGroundToTerrain[
                                            s_neighborCell->m_terrainImageIndex];
                                        s_adjacentCost = search->m_storage
                                            .nodes[adjacentIndex].distance;
                                        search->PushPoint(
                                            s_mapX, s_mapY,
                                            (s_direction +
                                             SEARCH_DIRECTION_REVERSE) &
                                                SEARCH_DIRECTION_MASK,
                                            s_adjacentCost + CalcTerrainCost(
                                                s_terrain, s_direction,
                                                giCurTempMobility - s_adjacentCost,
                                                pathfindingSkill,
                                                (s_neighborCell->m_objTypeBits & 2) >> 1,
                                                (s_targetCell->m_objTypeBits & 2) >> 1),
                                            maximumCost, 1, 0,
                                            SEARCH_INVALID_COORDINATE,
                                            SEARCH_INVALID_COORDINATE, 0,
                                            SEARCH_INVALID_COORDINATE,
                                            SEARCH_INVALID_COORDINATE);
                                    }
                                }
                                ++s_direction;
                            } while (s_direction < SEARCH_DIRECTION_COUNT);
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
