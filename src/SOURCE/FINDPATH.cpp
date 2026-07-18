// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\FINDPATH.OBJ   from: (directly linked into exe)
// functions: 12   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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

// ---- module-private synthetic globals (retail xref: single-module) ----
// Retail .data raw storage ends at VA 0x00523000. These VAs are in its loader-zero
// virtual tail, so the definitions intentionally have no initializer.
DATA(0x0052adc0) static i32 gSearchLow;
DATA(0x0052adc4) static mapCell* gSearchNextCell;
DATA(0x0052ae40) static searchNode* gSearchCell;
DATA(0x0052ae44) static i32 gSearchNextY;
DATA(0x0052ae48) static mapCell* gSearchCurrentCell;
DATA(0x0052ae4c) static i32 gSearchDirection;
DATA(0x0052ae50) static i32 gSearchTriggerType;
DATA(0x0052ae54) static i32 gSearchNextX;
DATA(0x0052ae58) static i32 gSearchTerrain;
DATA(0x0052ae5c) static searchNode* gSearchQueueNode;
DATA(0x0052ae60) static i32 gSearchMiddle;
DATA(0x0052ae64) static i32 gSearchHigh;

// ---- initialized allocation provenance (retail RVA order) ----
// @data-layout-note Retail initialized storage is exactly two contiguous 0x30-byte
// source-location records at 0x11733c..0x11739c. The fresh candidate has the same
// extent and SHA-256 d657e9dc9d8c980efa0bc771232ecc88ad147ea956dbf4cfbcc79d4a302975de.
// Its eight code relocations use the record starts and sourceFile at owner +0x4;
// retail has the same 1/1 allocation and 3/3 destruction target counts. Candidate
// and retail BSS are both 0xa8 and contain the same 13 DATA-proved logical owners.
// The candidate's internal owner order differs from retail packing. Thirty-one
// directly aligned owner references have exact RVAs/addends; all remaining BSS
// references use zero addends. The breadth include audit removed the former extra
// gSearchNextY site from TestPossibleDirections: its current 66/66 relocation set
// has no base-only owner. Preserve other nonexact call-site topology as function/link
// packing work; do not add aliases, padding, or placement rules.
DATA(0x0051733c) static SFindPathSourceLocation gSearchAllocationSource = {
    {20, 0},
    FINDPATH_SOURCE_FILE
};
DATA(0x0051736c) static SFindPathSourceLocation gSearchDestructionSource = {
    {26, 0},
    FINDPATH_SOURCE_FILE
};

// @early-stop: all 14 meaningful bytes are raw-identical and both sides have zero
// relocations; retail's only residual is the trailing 8B FF two-byte alignment pad.
VA(0x004a4a50, 0x10)
searchArray::searchArray(void) {
    m_storage.cells = 0;
    m_maxQueueCount = 0;
}

// @early-stop: relocation-masked instructions are identical; the 3/3 external
// relocations agree and the residual is the delinked line-constant identity plus
// one retail alignment LEA after RET.
VA(0x004a4a60, 0x30)
searchArray::~searchArray() {
    if (m_storage.cells != 0)
        H2_FREE_AT(
            m_storage.cells,
            gSearchDestructionSource.sourceFile,
            gSearchDestructionSource.line.value + 1
        );
    m_storage.cells = 0;
}

// @early-stop: relocation-masked instructions are identical; all 8/8 external
// relocations agree and only two delinked line-constant identities and retail
// post-RET alignment remain.
VA(0x004a4a90, 0x60)
void searchArray::Init(void) {
    if (m_storage.cells != 0)
        H2_FREE_AT(
            m_storage.cells,
            gSearchDestructionSource.sourceFile,
            gSearchDestructionSource.line.value + 1
        );
    m_storage.cells = 0;
    m_storage.cells = static_cast<searchCell*>(H2_ALLOC_AT(
        MAP_WIDTH * MAP_HEIGHT * sizeof(searchCell),
        gSearchAllocationSource.sourceFile,
        gSearchAllocationSource.line.value + 2
    ));
}

// @early-stop: relocation-masked instructions are identical; the 3/3 external
// relocations agree and the residual is one delinked line constant plus a retail
// alignment LEA after RET.
VA(0x004a4af0, 0x30)
void searchArray::Close(void) {
    if (m_storage.cells != 0)
        H2_FREE_AT(
            m_storage.cells,
            gSearchDestructionSource.sourceFile,
            gSearchDestructionSource.line.value + 1
        );
    m_storage.cells = 0;
}

// @early-stop: every meaningful instruction and both external relocations match;
// retail has one trailing alignment LEA after RET.
VA(0x004a4b20, 0x40)
void searchArray::Clear(void) {
    memset(m_queue, 0, sizeof(m_queue));
    memset(m_storage.cells, 0, MAP_WIDTH * MAP_HEIGHT * sizeof(searchCell));
    m_pathLength = 0;
    m_queueCount = 0;
}

// @semantic
// Complete distance semantics, no frame, zero relocations, and matching return
// blocks. First residual is the compare at +0x1f: retail emits CMP ESI,EAX/JL,
// while candidate emits CMP EAX,ESI/JG; retail also has a trailing alignment LEA.
// x>=y, y<=x, y<x, and x<y with reversed return-arm order were all compiled.
VA(0x004a4b60, 0x40)
i32 searchArray::QuickDistance(i32 x1, i32 y1, i32 x2, i32 y2) {
    i32 xDistance = abs(x1 - x2);
    i32 yDistance = abs(y1 - y2);

    if (xDistance >= yDistance)
        return xDistance + yDistance / 2;
    return yDistance + xDistance / 2;
}

// @semantic: complete semantics, no stack frame, matching CFG and 5/5 external
// relocations. First live divergence is retail keeping mobility in EBX and saving
// EBP for roadCost while ours uses EDI/EBX. Tried direct/reused cost values, scoped
// and top-level locals, operand reversal, row pointers, semantic aliases, register
// hints, declaration order, and parameter names; revisit after 95%/TU-state changes.
VA(0x004a4ba0, 0x80)
i32 CalcTerrainCost(
    i32 terrain,
    i32 diagonal,
    i32 mobility,
    i32 direction,
    i32 useRoad,
    i32 usePathfinding
) {
    i32 roadCost;

    if (mobility < giTerrainCost[terrain][direction][1]) {
        i32 baseCost = giTerrainCost[terrain][direction][0];
        if (mobility < baseCost) {
            if (useRoad == 0)
                goto terrainCost;
            roadCost = giTerrainCost[SEARCH_TERRAIN_ROAD][direction][0];
            if (mobility < roadCost)
                goto pathfindingCost;
        }
        if (useRoad != 0)
            return giTerrainCost[SEARCH_TERRAIN_ROAD][direction][0];
    } else {
    pathfindingCost:
        if (useRoad != 0 && usePathfinding != 0)
            terrain = SEARCH_TERRAIN_ROAD;
    }

terrainCost:
    return giTerrainCost[terrain][direction][diagonal & SEARCH_DIAGONAL_COST_MASK];
}

// @semantic: complete queue semantics, slots and CFG; 38/38 relocation
// occurrences, with only delinked interior aliases for normalDirTable and search
// scratch globals. First divergence is scratch-register assignment at queue setup;
// revisit after 95% or shared-layout/TU-state changes.
VA(0x004a4c20, 0x270)
void searchArray::PushPoint(
    i32 x,
    i32 y,
    i32 direction,
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
    if (x >= 0 && x <= MAP_WIDTH - 1 && y >= 0 && y <= MAP_HEIGHT - 1 && m_queueCount < 1024) {
        gSearchLow = 0;
        gSearchHigh = m_queueCount;
        gSearchCell = &GetNode(x, y);
        if (!gSearchCell->visited
            || ((gSearchCell->rvFlag1 || !rvFlag1)
                && (cost < gSearchCell->distance || (gSearchCell->rvFlag1 && !rvFlag1)))) {

            for (;;) {
                gSearchMiddle = (gSearchLow + gSearchHigh) / 2;
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

            if (giCurTempMobility < cost && rvFlag2 == 0) {
                gSearchQueueNode->rvFlag2 = 1;
                gSearchQueueNode->previousX = static_cast<i8>(x - normalDirTable[direction].x);
                gSearchQueueNode->previousY = static_cast<i8>(y - normalDirTable[direction].y);
            } else {
                gSearchQueueNode->rvFlag2 = static_cast<u8>(rvFlag2);
                gSearchQueueNode->previousX = static_cast<i8>(previousX);
                gSearchQueueNode->previousY = static_cast<i8>(previousY);
            }
            gSearchQueueNode->x = static_cast<u8>(x);
            gSearchQueueNode->y = static_cast<u8>(y);
            gSearchQueueNode->direction = static_cast<u8>(direction);
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

// @semantic: complete eight-direction CFG, object/trigger semantics and no local
// frame; 66/66 relocation occurrences, with four delinked interior aliases only.
// First divergence is EBX/EDI ownership in occupied-array initialization; memset,
// byte stores, sentinels and both terrain-arm orders were tried. Revisit after 95%.
VA(0x004a4e90, 0x36f)
void searchArray::TestPossibleDirections(
    i32 x,
    i32 y,
    i8* const terrain,
    i8* const occupied,
    i32 allowOccupied,
    i32 waterMode
) {
    i32 invalidTerrain = SEARCH_INVALID_COORDINATE;

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

        if ((gSearchNextCell->m_triggerType & SEARCH_TRIGGER_PRESENT) != 0) {
            if (!allowOccupied) {
                if (m_specialTargetX != gSearchNextX || m_specialTargetY != gSearchNextY)
                    goto invalidDirection;
            } else {
                occupied[gSearchDirection] = 1;
            }
        }

        gSearchTerrain = giGroundToTerrain[gSearchNextCell->m_terrainImageIndex];
        if (gSearchTerrain == SEARCH_TERRAIN_WATER) {
            if (waterMode != 0) {
                if (gSearchNextCell->m_triggerType == SEARCH_WATER_ENTRY_SECOND)
                    goto invalidDirection;
                if (giGroundToTerrain[gSearchCurrentCell->m_terrainImageIndex]
                        == SEARCH_TERRAIN_WATER
                    && normalDirTable[gSearchDirection].x != 0
                    && normalDirTable[gSearchDirection].y != 0) {
                    if (giGroundToTerrain[gpAdvManager->GetCell(gSearchNextX, y)
                                              ->m_terrainImageIndex]
                            != SEARCH_TERRAIN_WATER
                        || giGroundToTerrain[gpAdvManager->GetCell(x, gSearchNextY)
                                                 ->m_terrainImageIndex]
                               != SEARCH_TERRAIN_WATER)
                        goto invalidDirection;
                }
            } else {
                if (gSearchNextCell->m_triggerType != SEARCH_WATER_ENTRY_FIRST
                    && gSearchNextCell->m_triggerType != SEARCH_WATER_ENTRY_SECOND
                    && gSearchNextCell->m_triggerType != SEARCH_WATER_ENTRY_THIRD)
                    goto invalidDirection;
            }
        } else if (waterMode != 0 && gSearchNextCell->m_triggerType != SEARCH_TRIGGER_BOAT) {
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
                || ((gSearchNextCell->m_triggerType & SEARCH_TRIGGER_PRESENT) != 0
                    && (gSearchTriggerType = gSearchNextCell->m_triggerType & SEARCH_TRIGGER_MASK,
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
        terrain[gSearchDirection] = static_cast<i8>(gSearchTerrain);
        gSearchDirection++;
    } while (gSearchDirection < SEARCH_DIRECTION_COUNT);
}

// @early-stop: after restoring the signed speed <= 0 predicate, every meaningful
// instruction and all 16/16 ordered external relocations match. Retail alone has
// a trailing three-byte alignment LEA after RET.
VA(0x004a5200, 0x1f0)
void searchArray::SeedCombatPosition(class army* unit) {
    i32 hex;

    for (hex = 0; hex < COMBAT_HEX_COUNT; hex++)
        gpCombatManager->m_hexCells[hex].m_pathReachable = 0;

    if (HAS(unit->m_monster.attributes, MONSTER_ATTRIBUTE_FLYING) != 0) {
        for (hex = 0; hex < COMBAT_HEX_COUNT; hex++) {
            if (unit->CanFit(hex, 0, 0))
                gpCombatManager->m_hexCells[hex].m_pathReachable = 1;
        }
    } else {
        for (hex = 0; hex < COMBAT_HEX_COUNT; hex++) {
            if (unit->ValidPath(hex, 1))
                gpCombatManager->m_hexCells[hex].m_pathReachable = 1;
        }
    }

    for (i32 index = 0; index < gpCombatManager->m_armyCount[1 - unit->m_side]; index++) {
        army* enemy = &gpCombatManager->m_armies[1 - unit->m_side][index];
        unit->m_targetSide = enemy->m_side;
        unit->m_targetIndex = enemy->m_index;
        hex = enemy->m_hex;

        if (unit->m_monster.speed <= 0
            || unit->GetAttackMask(unit->m_hex, 1, -1) != PATH_ATTACK_MASK_SURROUNDED) {
            if (unit->ValidPath(hex, 1) == 1)
                gpCombatManager->m_hexCells[hex].m_pathReachable = 1;
        } else {
            gpCombatManager->m_hexCells[hex].m_pathReachable = 1;
        }

        if (HAS(enemy->m_monster.attributes, MONSTER_ATTRIBUTE_WIDE) != 0) {
            hex = enemy->GetAdjacentCellIndex(
                hex,
                enemy->m_facing == 1 ? COMBAT_DIRECTION_NORTHEAST : COMBAT_DIRECTION_SOUTHWEST
            );
            if ((unit->m_monster.speed > 0
                 && unit->GetAttackMask(unit->m_hex, 1, -1) == PATH_ATTACK_MASK_SURROUNDED)
                || unit->ValidPath(hex, 1) == 1) {
                gpCombatManager->m_hexCells[hex].m_pathReachable = 1;
            }
        }
    }
    unit->m_targetIndex = -1;
    unit->m_targetSide = -1;
}

// @semantic: complete moat, queue, attack and reconstruction CFG with the retail
// 0x30 frame; all 23/23 external relocations agree. First divergence is EAX versus
// ECX for the moat-cell byte. Result/attack-target lifetime reuse and branch
// polarities are recovered; revisit after 95% for register allocation only.
VA(0x004a53f0, 0x410)
i32 searchArray::FindCombatPath(
    i32 sourceHex,
    i32 targetHex,
    class army* unit,
    i32 attackPath,
    i32 ignoreTargetMoat
) {
    u8 savedMoatState[9];
    memset(bIsMoatSlowed, 0, sizeof(bIsMoatSlowed));

    if (gpCombatManager->m_drawbridgeBackgroundVisible != 0) {
        i32 moatIndex;
        i32 sourceWideHex = -1;
        i32 targetWideHex = -1;
        if (HAS(unit->m_monster.attributes, MONSTER_ATTRIBUTE_WIDE) != 0) {
            i32 offset = unit->m_facing == 1 ? 1 : -1;
            sourceWideHex = unit->m_hex + offset;
            targetWideHex = targetHex + offset;
        }

        for (moatIndex = 0; moatIndex < 9; moatIndex++) {
            i32 moatHex = moatCell[moatIndex];
            savedMoatState[moatIndex] = gpCombatManager->m_hexCells[moatHex].m_pathReachable;
            if ((moatIndex != 4 || gpCombatManager->m_drawbridgeState == COMBAT_DRAWBRIDGE_RAISED)
                && ((targetHex != moatHex && targetWideHex != moatHex) || ignoreTargetMoat != 0)
                && (unit->m_hex != moatHex && sourceWideHex != moatHex)) {
                bIsMoatSlowed[moatHex] = 1;
            }
        }
    }

    i32 bestDistance = 640;
    i32 bestHex = -1;
    i32 attackTargetHex = targetHex;
    if (attackPath == 0)
        attackTargetHex = -1;
    memset(m_queue, 0, sizeof(m_queue));
    memset(m_storage.cells, 0, MAP_WIDTH * MAP_HEIGHT * sizeof(searchCell));
    m_queueCount = 0;
    m_pathLength = 0;

    i8* path;
    if (ValidHex(sourceHex) && ValidHex(targetHex) && unit != 0) {
        path = m_storage.aiPath.directions;
        u32 attackMask;
        i32 attackDirection;
        i32 currentHex;
        PushCombatPoint(
            sourceHex,
            unit->m_facing == 0 ? COMBAT_DIRECTION_SOUTHWEST : COMBAT_DIRECTION_NORTHEAST,
            0,
            unit->m_monster.speed
        );

        while (m_queueCount != 0) {
            searchNode node = m_queue[--m_queueCount];
            if (node.distance > unit->m_monster.speed)
                continue;

            currentHex = node.x;
            i32 xDistance =
                abs(gpCombatManager->m_hexCells[currentHex].m_x
                    - gpCombatManager->m_hexCells[targetHex].m_x);
            i32 yDistance =
                abs(gpCombatManager->m_hexCells[currentHex].m_y
                    - gpCombatManager->m_hexCells[targetHex].m_y);
            i32 distance;
            if (xDistance >= yDistance)
                distance = xDistance + yDistance / 2;
            else
                distance = yDistance + xDistance / 2;

            if (unit->m_targetSide != -1) {
                attackMask = unit->GetAttackMask(currentHex, 0, attackTargetHex);
                if (attackMask != PATH_ATTACK_MASK_SURROUNDED) {
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
            for (i32 direction = 0; direction < 8; direction++) {
                if ((moveMask & (1U << direction)) == 0) {
                    i32 nextHex = unit->GetAdjacentCellIndex(currentHex, direction);
                    i32 moatCost = 0;
                    if (bIsMoatSlowed[nextHex])
                        moatCost = unit->m_speed + 2;
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
            path++;
            path[-1] = static_cast<i8>(attackDirection);
            bestHex = currentHex;
            m_pathLength++;
            goto searchComplete;
        }
        attackDirection++;
        if (attackDirection < 8)
            goto findAttackDirection;
        goto searchComplete;

    searchComplete:
        if (unit->m_targetSide != -1) {
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
        for (i32 moatIndex = 0; moatIndex < 9; moatIndex++)
            gpCombatManager->m_hexCells[moatCell[moatIndex]].m_pathReachable =
                savedMoatState[moatIndex];
    }
    return attackTargetHex;

reconstructPath:
    path = m_storage.aiPath.directions + m_pathLength;
    while (bestHex != sourceHex) {
        searchNode& cell = m_storage.nodes[bestHex];
        *path++ = static_cast<i8>(cell.direction);
        m_pathLength++;
        if (m_pathLength > PATH_LENGTH_LIMIT)
            break;
        i32 opposite = OppositeDirection(cell.direction);
        bestHex = unit->GetAdjacentCellIndex(bestHex, opposite);
    }
    attackTargetHex = m_pathLength;
    goto restoreMoat;
}

// @semantic: complete nested gates, signed binary search, queue update and packed
// node writes; both external relocations agree and there is no frame. First
// divergence is ESI/EDI ownership in the prologue. Compound/nested gates, signed
// and unsigned indices, and early/late cell formation were tried; revisit at 95%.
VA(0x004a5800, 0x100)
void searchArray::PushCombatPoint(i32 hex, i32 direction, i32 distance, i32 speed) {
    if (ValidHex(hex)) {
        i32 low = 0;
        i32 high = m_queueCount;
        searchNode* cell;
        if (speed <= 0 || distance <= speed) {
            cell = &m_storage.nodes[hex];
            if ((!cell->visited || distance < cell->distance) && m_queueCount < 1024) {
                i32 middle;
                searchNode* node;

                for (;;) {
                    middle = (low + high) / 2;
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

// ---- globals (definitions, RVA order) ----
DATA(0x0052adc8) u8 bIsMoatSlowed[117];
