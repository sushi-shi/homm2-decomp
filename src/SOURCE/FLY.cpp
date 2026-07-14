// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\FLY.OBJ   from: (directly linked into exe)
// functions: 4   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <math.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/soundManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/army.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/PATH.h>
#include <SOURCE/X_GLOBAL.h>
// @match-note: Semantic CFG and the 0x0c frame are complete; all 14 relocations
// agree. First residual is +0x0f..+0x11: retail stores candidateHex at [ebp-0x8],
// ours at [ebp-0x4], with cell in the opposite slot. od_slots suffixes, declaration/
// initialization order, cached/direct cell access, and wide-branch polarity were tried.
VA(0x004a5900, 0x295)
int army::CanFit(int hex, int tryOtherSide, int *fittingHex)
{
    hexcell *cell;
    int candidateHex;

    candidateHex = hex;
    cell = 0;
    if (fittingHex) {
        *fittingHex = hex;
    }
    if (!ValidHex(candidateHex) || candidateHex % ARMY_HEX_COLUMNS == 0 ||
        candidateHex % ARMY_HEX_COLUMNS == ARMY_HEX_COLUMNS - 1) {
        return 0;
    }
    if (gpCombatManager->m_hexCells[candidateHex].m_occupantSide != -1 ||
        gpCombatManager->m_hexCells[candidateHex].m_blocked) {
        return 0;
    }
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        candidateHex = GetAdjacentCellIndex(
            hex,
            (-(static_cast<unsigned int>(m_facing - 1) < 1) & -3) + 4);
        if (ValidHex(candidateHex)) {
            cell = &gpCombatManager->m_hexCells[candidateHex];
        }
        if (ValidHex(candidateHex) &&
            (cell->m_occupantSide == -1 ||
             (cell->m_occupantSide == gpCombatManager->m_currentArmySide &&
              cell->m_occupantIndex == gpCombatManager->m_currentArmyIndex)) &&
            !cell->m_blocked) {
            return 1;
        } else {
            if (!tryOtherSide) {
                return 0;
            }

            candidateHex = GetAdjacentCellIndex(
                hex,
                (-(static_cast<unsigned int>(m_facing - 1) < 1) & 3) + 1);
            if (ValidHex(candidateHex)) {
                cell = &gpCombatManager->m_hexCells[candidateHex];
            } else {
                return 0;
            }
            if ((cell->m_occupantSide == -1 ||
                 (cell->m_occupantSide == gpCombatManager->m_currentArmySide &&
                  cell->m_occupantIndex ==
                      gpCombatManager->m_currentArmyIndex)) &&
                !cell->m_blocked) {
                if (fittingHex) {
                    *fittingHex = candidateHex;
                }
                return 1;
            } else {
                return 0;
            }
        }
    }
    return 1;
}

// @match-note: Semantic CFG is complete and all 18 relocations agree. First
// residual is +0x03..+0x05: retail frame 0x38, ours 0x30 (8 bytes short); NB09
// has no local records for slot recovery. Local names/order/scope, attackHex forms,
// cached/direct target access, if/ternary forms, and direction-loop polarity were tried.
VA(0x004a5b95, 0x405)
int army::ValidFlight(int destination, int fromTargetHex)
{
    army *target;
    int targetHex;
    int attackMask;
    int initialDirection;
    int direction;
    int attackHex[2];
    unsigned int directionMask;
    int adjacentHex;
    int fittingHex;
    int i;

    if (!ValidHex(destination)) {
        return 0;
    }
    if (m_targetSide < 0 || m_targetSide > COMBAT_DEFENDER_SIDE ||
        m_targetIndex < 0 ||
        m_targetIndex > COMBAT_ARMY_SLOT_COUNT - 1) {
        if (CanFit(destination, 0, 0)) {
            m_moveTargetHex = destination;
            return 1;
        }
        return 0;
    }

    target = &gpCombatManager->m_armies[m_targetSide][m_targetIndex];
    if (fromTargetHex) {
        targetHex = destination;
    } else {
        targetHex = target->m_hex;
    }
    if (!ValidHex(targetHex)) {
        return 0;
    }

    attackMask = GetAttackMask(m_hex, 0, -1);
    while (attackMask != ARMY_ALL_ATTACK_DIRECTIONS) {
        initialDirection = GetBestDirection(m_hex, targetHex, attackMask);
        if (ValidAttack(m_hex, initialDirection, 0, -1, attackHex)) {
            m_attackDirection = initialDirection;
            m_moveTargetHex = m_hex;
            return 1;
        }
        attackMask |= 1 << initialDirection;
    }

    directionMask = 0;
    if ((target->m_monster.flags.all & MONSTER_FLAGS_WIDE) && !fromTargetHex) {
        if (target->m_facing == 1) {
            targetHex++;
        } else {
            targetHex--;
        }
        if (target->m_facing == 1) {
            directionMask = 1 << ARMY_DIRECTION_RIGHT;
        } else {
            directionMask = 1 << ARMY_DIRECTION_LEFT;
        }
    }
    while (directionMask != ARMY_ALL_ADJACENT_DIRECTIONS) {
        direction = GetBestDirection(targetHex, m_hex, directionMask);
        adjacentHex = GetAdjacentCellIndex(targetHex, direction);
        if (ValidHex(adjacentHex) &&
            CanFit(adjacentHex, 1 - fromTargetHex, &fittingHex)) {
            m_moveTargetHex = fittingHex;
            if (!(m_monster.flags.all & MONSTER_FLAGS_WIDE)) {
                m_attackDirection = OppositeDirection(direction);
            } else {
                attackMask = ~GetAttackMask(m_moveTargetHex, 0, -1);
                for (i = 0; i < ARMY_COMBAT_DIRECTION_COUNT; i++) {
                    if (attackMask & (1 << i)) {
                        m_attackDirection = i;
                    }
                }
            }
            return 1;
        }
        directionMask |= 1 << direction;
    }

    if ((target->m_monster.flags.all & MONSTER_FLAGS_WIDE) && !fromTargetHex) {
        if (target->m_facing == 1) {
            targetHex--;
        } else {
            targetHex++;
        }
        if (target->m_facing == 1) {
            directionMask = 1 << ARMY_DIRECTION_LEFT;
        } else {
            directionMask = 1 << ARMY_DIRECTION_RIGHT;
        }
        while (directionMask != ARMY_ALL_ADJACENT_DIRECTIONS) {
            direction = GetBestDirection(targetHex, m_hex, directionMask);
            adjacentHex = GetAdjacentCellIndex(targetHex, direction);
            if (ValidHex(adjacentHex) && CanFit(adjacentHex, 0, 0)) {
                m_moveTargetHex = adjacentHex;
                m_attackDirection = GetBestDirection(
                    m_moveTargetHex, targetHex, 0);
                return 1;
            }
            directionMask |= 1 << direction;
        }
    }
    return 0;
}

VA(0x004a5f9a, 0x25)
int army::FlyTo(void)
{
    return FlyTo(m_moveTargetHex);
}

// @match-note: Semantic CFG is complete and all 119 relocations agree. First
// residual is +0x03..+0x08: retail frame 0xc0, ours 0xb4 (12 bytes short); NB09
// has no local records. Local names/order/scope, scalar/aggregate temporary forms,
// initialization grouping, condition polarity, and for/while spellings were tried.
// The dead retail segment-count==0 fallback and the BEGIN_STANDING frame count
// overwritten by the MIDDLE assignment were audited and intentionally retained.
VA(0x004a5fbf, 0xc1f)
int army::FlyTo(int destination)
{
    int sourceColumn;
    int destinationColumn;
    int columnDifference;
    int destinationX;
    int destinationY;
    int sourceX;
    int sourceY;
    float x;
    float y;
    int xDistance;
    int yDistance;
    int distance;
    int flightSegmentCount;
    float xStep;
    float yStep;
    int sourceRearHex;
    int destinationRearHex;
    int flightSegment;
    int flightFrameCount;
    int middleFrameCount;
    int frameStart;
    int oldMinX;
    int oldMinY;
    int oldMaxX;
    int oldMaxY;

    if (!ValidHex(destination)) {
        return 0;
    }

    sourceColumn = m_hex % ARMY_HEX_COLUMNS;
    destinationColumn = destination % ARMY_HEX_COLUMNS;
    columnDifference = destinationColumn - sourceColumn;
    m_facingChanged = 0;
    if (columnDifference > 0 && m_facing == 0) {
        m_facingChanged = 1;
        m_facing = 1 - m_facing;
        if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            m_hex--;
            destination--;
        }
    } else if (columnDifference < 0 && m_facing == 1) {
        m_facingChanged = 1;
        m_facing = 1 - m_facing;
        if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            m_hex++;
            destination++;
        }
    }
    if (m_facingChanged) {
        gpCombatManager->DrawFrame(
            1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }

    destinationX = gpCombatManager->m_hexCells[destination].m_x;
    destinationY = gpCombatManager->m_hexCells[destination].m_y;
    sourceX = gpCombatManager->m_hexCells[m_hex].m_x;
    sourceY = gpCombatManager->m_hexCells[m_hex].m_y;
    x = static_cast<float>(sourceX);
    y = static_cast<float>(sourceY);
    xDistance = destinationX - sourceX;
    yDistance = destinationY - sourceY;
    distance = static_cast<int>(sqrt(static_cast<double>(
        xDistance * xDistance + yDistance * yDistance)));
    flightSegmentCount = 0;
    if (m_frameInfo.attackDuration > 0) {
        flightSegmentCount = ((m_frameInfo.attackDuration >> 1) + distance) /
                             m_frameInfo.attackDuration;
    }
    if (flightSegmentCount <= 0) {
        flightSegmentCount = 1;
    }
    xStep = static_cast<float>(xDistance) / flightSegmentCount;
    yStep = static_cast<float>(yDistance) / flightSegmentCount;

    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantFrame = -1;
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        sourceRearHex =
            ((-(static_cast<unsigned int>(m_facing - 1) < 1) & 2) - 1) +
            m_hex;
        gpCombatManager->m_hexCells[sourceRearHex].m_occupantIndex = -1;
        gpCombatManager->m_hexCells[sourceRearHex].m_occupantSide = -1;
        gpCombatManager->m_hexCells[sourceRearHex].m_occupantFrame = -1;
    }

    if (!gbNoShowCombat) {
        flightFrameCount = 0;
        frameStart = 0;
        middleFrameCount = 0;
        gpCombatManager->DrawFrame(
            0, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        gpWindowManager->m_screen->CopyTo(
            gpCombatManager->m_backgroundBuffer, 0, 0, 0, 0,
            ARMY_COMBAT_WIDTH, ARMY_COMBAT_HEIGHT);
        gpCombatManager->m_backgroundDrawn = 0;
        m_animationSequence = ARMY_ANIMATION_WALK;
        for (flightSegment = 0;
             flightSegmentCount > flightSegment;
             flightSegment++) {
            BuildTempWalkSeq(
                &m_frameInfo,
                flightSegment + 1 == flightSegmentCount,
                flightSegment > 0);
            // Retail retains this fallback after clamping the segment count to one.
            if (flightSegmentCount == 0) {
                flightFrameCount =
                    m_frameInfo.animationFrameCount[ARMY_ANIMATION_WALK];
                frameStart = 0;
            } else {
                if (flightSegment > 0) {
                    // This transition count is overwritten by the retail MIDDLE assignment.
                    flightFrameCount += m_frameInfo.animationFrameCount[
                        ARMY_WALK_SEGMENT_BEGIN_STANDING];
                    frameStart = 0;
                } else {
                    frameStart = m_frameInfo.animationFrameCount[
                        ARMY_WALK_SEGMENT_BEGIN];
                }
                flightFrameCount = m_frameInfo.animationFrameCount[
                    ARMY_WALK_SEGMENT_MIDDLE];
                middleFrameCount = flightFrameCount;
                if (flightSegment + 1 < flightSegmentCount) {
                    flightFrameCount += m_frameInfo.animationFrameCount[
                        ARMY_WALK_SEGMENT_END];
                }
            }

            for (m_animationFrame = 0;
                 m_animationFrame <
                     m_frameInfo.animationFrameCount[ARMY_ANIMATION_WALK];
                 m_animationFrame++) {
                if (m_animationFrame >= frameStart &&
                    m_animationFrame < flightFrameCount + frameStart) {
                    x += xStep / flightFrameCount;
                    y += yStep / flightFrameCount;
                }
                if (m_animationFrame %
                        m_frameInfo.animationFrameCount[ARMY_ANIMATION_WALK] ==
                    ARMY_FLIGHT_SOUND_FRAME) {
                    if ((m_monsterType == ARMY_CREATURE_VAMPIRE ||
                         m_monsterType == ARMY_CREATURE_VAMPIRE_LORD) &&
                        flightSegment == 0) {
                        gpSoundManager->MemorySample(
                            m_samples[ARMY_SAMPLE_EXTRA_ONE]);
                        DelayMilli(ARMY_VAMPIRE_FLIGHT_SOUND_DELAY);
                    } else if ((m_monsterType == ARMY_CREATURE_VAMPIRE ||
                                m_monsterType ==
                                    ARMY_CREATURE_VAMPIRE_LORD) &&
                               flightSegmentCount - 1 == flightSegment) {
                        gpSoundManager->MemorySample(
                            m_samples[ARMY_SAMPLE_EXTRA_TWO]);
                    } else {
                        gpSoundManager->MemorySample(
                            m_samples[ARMY_SAMPLE_MOVE]);
                    }
                }

                if (flightSegment != 0 || m_animationFrame != 0) {
                    gpCombatManager->m_backgroundBuffer->CopyTo(
                        gpWindowManager->m_screen,
                        giMinExtentX, giMinExtentY,
                        giMinExtentX, giMinExtentY,
                        giMaxExtentX - giMinExtentX + 1,
                        giMaxExtentY - giMinExtentY + 1);
                    oldMinX = giMinExtentX;
                    oldMinY = giMinExtentY;
                    oldMaxX = giMaxExtentX;
                    oldMaxY = giMaxExtentY;
                } else {
                    oldMinX = 0;
                    oldMinY = 0;
                    oldMaxX = ARMY_COMBAT_MAX_X;
                    oldMaxY = ARMY_COMBAT_MAX_Y;
                }
                giMinExtentY = ARMY_COMBAT_WIDTH;
                giMinExtentX = giMinExtentY;
                giMaxExtentY = 0;
                giMaxExtentX = giMaxExtentY;
                gbComputeExtent = 1;
                gbSaveBiggestExtent = 1;
                DrawToBuffer(static_cast<int>(x), static_cast<int>(y), 0);
                gbComputeExtent = 0;
                gbSaveBiggestExtent = 0;
                if (giMinExtentX < 0) giMinExtentX = 0;
                if (giMinExtentY < 0) giMinExtentY = 0;
                if (giMaxExtentX > ARMY_COMBAT_MAX_X)
                    giMaxExtentX = ARMY_COMBAT_MAX_X;
                if (giMaxExtentY > ARMY_COMBAT_MAX_Y)
                    giMaxExtentY = ARMY_COMBAT_MAX_Y;
                if (oldMinX > giMinExtentX) oldMinX = giMinExtentX;
                if (oldMinY > giMinExtentY) oldMinY = giMinExtentY;
                if (oldMaxX < giMaxExtentX) oldMaxX = giMaxExtentX;
                if (oldMaxY < giMaxExtentY) oldMaxY = giMaxExtentY;

                DelayTil(glTimers);
                if (m_animationFrame < frameStart ||
                    (m_animationFrame + 1 >= middleFrameCount &&
                     (m_monsterType == ARMY_CREATURE_VAMPIRE ||
                      m_monsterType == ARMY_CREATURE_VAMPIRE_LORD))) {
                    glTimers[0] = static_cast<int>(
                        KBTickCount() + m_frameInfo.walkDuration *
                        gfCombatSpeedMod[gConfig.combatSpeed] *
                        ARMY_VAMPIRE_FLIGHT_DURATION_SCALE /
                        flightFrameCount);
                } else {
                    glTimers[0] = static_cast<int>(
                        KBTickCount() + m_frameInfo.walkDuration *
                        gfCombatSpeedMod[gConfig.combatSpeed] /
                        flightFrameCount);
                }
                gpWindowManager->UpdateScreenRegion(
                    oldMinX, oldMinY,
                    oldMaxX - oldMinX + 1,
                    oldMaxY - oldMinY + 1);
                if (m_animationFrame ==
                    m_frameInfo.animationFrameCount[ARMY_ANIMATION_WALK] - 1) {
                    x = sourceX + (flightSegment + 1) * xStep;
                    y = sourceY + (flightSegment + 1) * yStep;
                }
            }
        }
    }

    CancelSpellType(ARMY_CANCEL_SPELLS_AFTER_MOVE);
    gpCombatManager->m_hexCells[destination].m_occupantSide =
        static_cast<signed char>(gpCombatManager->m_currentArmySide);
    gpCombatManager->m_hexCells[destination].m_occupantIndex =
        static_cast<signed char>(gpCombatManager->m_currentArmyIndex);
    gpCombatManager->m_hexCells[destination].m_occupantFrame = -1;
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        destinationRearHex =
            ((-(static_cast<unsigned int>(m_facing - 1) < 1) & 2) - 1) +
            destination;
        gpCombatManager->m_hexCells[destinationRearHex].m_occupantSide =
            static_cast<signed char>(gpCombatManager->m_currentArmySide);
        gpCombatManager->m_hexCells[destinationRearHex].m_occupantIndex =
            static_cast<signed char>(gpCombatManager->m_currentArmyIndex);
        if (destinationRearHex >= destination) {
            gpCombatManager->m_hexCells[destinationRearHex].m_occupantFrame = 1;
        } else {
            gpCombatManager->m_hexCells[destinationRearHex].m_occupantFrame = 0;
        }
        if (destinationRearHex <= destination) {
            gpCombatManager->m_hexCells[destination].m_occupantFrame = 1;
        } else {
            gpCombatManager->m_hexCells[destination].m_occupantFrame = 0;
        }
    }
    m_hex = destination;
    m_animationSequence = ARMY_ANIMATION_STAND;
    m_animationFrame = 0;
    if (m_facingChanged) {
        m_facing = 1 - m_facing;
        if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            if (m_facing == 0) {
                m_hex++;
                destination++;
            } else {
                m_hex--;
                destination--;
            }
        }
        m_facingChanged = 0;
    }
    gpCombatManager->DrawFrame(
        1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    gpCombatManager->TestRaiseDoor();
    return 1;
}
