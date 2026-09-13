#include <va.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Icon2b.h>
#include <BASE/Iconf2b.h>
#include <BASE/DebugCheck.h>
#include <BASE/Misc.h>
#include <BASE/soundManager.h>
#include <EDITOR/fullMap.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/advManager.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/FINDPATH.h>
#include <SOURCE/GAME.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/playerData.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/town.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/message.h>
#include <stdio.h>
#include <string.h>
#include <SOURCE/KB_TYPES.h>

H2_ENUM_BEGIN(CursorPrivateConstant)
    SLOW_CURSOR_CYCLE_START  = 2,
    SKIPPED_ANIMATION_FRAME  = 4,
    FOOTSTEP_ANIMATION_FRAME = 3,
    DIRECTION_HALF_COUNT     = IDX(MAP_DIRECTION_COUNT) / 2,
    TURN_FRAME_MULTIPLIER    = 2,
    MOVE_TILE_HALF_COUNT     = 2,
    GROUP_ALLOC_LINE_OFFSET  = 7,
    GROUP_FREE_LINE_OFFSET   = 25
H2_ENUM_END(CursorPrivateConstant)

#define SLOW_TURN_DELAY_SCALE 1.5


#if H2_RETAIL_COMPILER
#define directionX directionX_a
#endif
VA(0x004332d0, 0x124)
void advManager::StartCursor(H2_ENUM_PARAM(MapDirection, i32) direction) {
    i32 cellX;
    i32 directionY;
    i32 directionX;
    i32 cellY;

    m_cursorDirection = direction;
    m_cursorFrame = GetCursorBaseFrame(direction) + 1;
    m_cursorCycle = gConfig.walkSpeeds[gbThisNetHumanPlayer[giCurPlayer]]
                            > CONFIG_WALK_SPEED_SLOWEST
                        ? 1
                        : SLOW_CURSOR_CYCLE_START;

    directionX = normalDirTable[IDX(direction)].x;
    directionY = normalDirTable[IDX(direction)].y;
    m_previousCursorMapX = m_cursorMapX;
    m_previousCursorMapY = m_cursorMapY;
    m_cursorMapX += directionX;
    m_cursorMapY += directionY;
    cellX = m_mapOriginX + m_cursorMapX;
    cellY = m_mapOriginY + m_cursorMapY;
    m_mapData->GetCell(cellX, cellY)->m_flags |= CURSOR_MAP_VISIBLE_FLAG;
}
#if H2_RETAIL_COMPILER
#undef directionX
#endif

VA(0x004333f4, 0xe6)
void advManager::StopCursor(i32 stopSound) {
    if (stopSound) {
        bMoveSoundMade = true;
        m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
        m_cursorFrameCount = 0;
        EveryOther = 0;
    }
    m_cursorCycle = 0;
    if (m_previousCursorMapX != -1) {
        m_mapData->GetCell(
            m_mapOriginX + m_previousCursorMapX,
            m_mapOriginY + m_previousCursorMapY
        )
            ->m_flags &= ~CURSOR_MAP_VISIBLE_FLAG;
        m_previousCursorMapY = -1;
        m_previousCursorMapX = -1;
    }
    m_cursorTurning = 0;
}

VA(0x004334da, 0x5dd)
void advManager::DrawCursor(void) {
    i32 drawY;
    i32 drawFrame;

    if (bShowIt == 0 || bSpecialHideCursor)
        return;
    if (gbDrawSavedCursor) {
        m_cursorDirection = S1cursorDirection;
        m_cursorFrame = S1cursorBaseFrame;
        m_cursorFrameCount = S1cursorFrameCount;
        m_cursorCycle = S1cursorCycle;
        m_cursorTurning = S1cursorTurning;
    }

    i32 drawX = m_updateMinX + CURSOR_DRAW_X;
    drawY = m_updateMinY + CURSOR_DRAW_Y;
    if (m_cursorType == HERO_TYPE_BOAT)
        drawY -= CURSOR_DRAW_Y - CURSOR_BOAT_DRAW_Y;

    if (m_cursorFrame & CURSOR_FLIP_FLAG) {
        drawX += CURSOR_FLIPPED_DRAW_X - CURSOR_DRAW_X;
        drawFrame = (m_cursorFrame & CURSOR_FRAME_MASK) + m_cursorFrameCount;
        if (m_cursorType == HERO_TYPE_BOAT
            && !(
                GetCell(
                    m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
                    m_mapOriginY + CURSOR_MAP_DRAW_OFFSET
                )
                    ->m_flags
                & CURSOR_CELL_UNCOVERED_FLAG
            )) {
            DRAW_FLIPPED_ADVENTURE_ICON(
                m_heroIcons[CURSOR_BOAT_WAKE_TYPE],
                drawX,
                drawY,
                drawFrame,
                ICON_DRAW_CLIP
            );
        }
        DRAW_FLIPPED_ADVENTURE_ICON(
            m_heroIcons[IDX(m_cursorType)],
            drawX,
            drawY,
            drawFrame,
            ICON_DRAW_CLIP
        );
        if (m_cursorType == HERO_TYPE_BOAT) {
            i32 H2_UNUSED(flagDrawn);

            FlipIconToBitmap(
                m_boatFlagIcons[gpCurPlayer->m_color],
                gpWindowManager->m_screen,
                drawX,
                drawY,
                drawFrame,
                ICON_DRAW_NO_CLIP,
                0,
                0,
                0,
                0,
                0
            );
            flagDrawn = 1;
        } else {
            if (m_cursorCycle == 0) {
                drawFrame = (m_cursorFrame & CURSOR_FRAME_MASK)
                            + m_updateMaxY % IDX(MAP_DIRECTION_COUNT) + CURSOR_FLAG_FRAME_BASE;
            }
            DRAW_FLIPPED_ADVENTURE_ICON(
                m_flagIcons[gpCurPlayer->m_color],
                drawX,
                drawY,
                drawFrame,
                ICON_DRAW_CLIP
            );
            ++m_updatePending;
        }
    } else {
        drawFrame = m_cursorFrame + m_cursorFrameCount;
        if (m_cursorType == HERO_TYPE_BOAT
            && !(
                GetCell(
                    m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
                    m_mapOriginY + CURSOR_MAP_DRAW_OFFSET
                )
                    ->m_flags
                & CURSOR_CELL_UNCOVERED_FLAG
            )) {
            DRAW_ADVENTURE_ICON(
                m_heroIcons[CURSOR_BOAT_WAKE_TYPE],
                drawX,
                drawY,
                drawFrame,
                ICON_DRAW_CLIP
            );
        }
        DRAW_ADVENTURE_ICON(
            m_heroIcons[IDX(m_cursorType)],
            drawX,
            drawY,
            drawFrame,
            ICON_DRAW_CLIP
        );
        if (m_cursorType == HERO_TYPE_BOAT) {
            i32 H2_UNUSED(flagDrawn);

            IconToBitmap(
                m_boatFlagIcons[gpCurPlayer->m_color],
                gpWindowManager->m_screen,
                drawX,
                drawY,
                drawFrame,
                ICON_DRAW_NO_CLIP,
                0,
                0,
                0,
                0,
                0
            );
            flagDrawn = 1;
        } else {
            if (m_cursorCycle == 0) {
                drawFrame = (m_cursorFrame & CURSOR_FRAME_MASK)
                            + m_updateMaxY % IDX(MAP_DIRECTION_COUNT) + CURSOR_FLAG_FRAME_BASE;
            }
            DRAW_ADVENTURE_ICON(
                m_flagIcons[gpCurPlayer->m_color],
                drawX,
                drawY,
                drawFrame,
                ICON_DRAW_CLIP
            );
            ++m_updatePending;
        }
    }

    if (m_cursorCycle
        && gConfig.walkSpeeds[gbThisNetHumanPlayer[giCurPlayer]]
               != CONFIG_WALK_SPEED_INSTANT) {
        ++m_cursorFrameCount;
        if (gConfig.walkSpeeds[gbThisNetHumanPlayer[giCurPlayer]]
                == CONFIG_WALK_SPEED_FAST
            && (m_cursorFrameCount == SKIPPED_ANIMATION_FRAME || m_cursorFrameCount == 1))
            ++m_cursorFrameCount;
        if (gConfig.walkSpeeds[gbThisNetHumanPlayer[giCurPlayer]]
            == CONFIG_WALK_SPEED_SLOWEST) {
            EveryOther = !EveryOther;
            if (EveryOther)
                --m_cursorFrameCount;
        }
    }
    if (m_cursorFrameCount >= CURSOR_LAST_FRAME_COUNT)
        m_cursorFrameCount = 0;

    if (!m_cursorTurning) {
        if (m_cursorFrameCount == FOOTSTEP_ANIMATION_FRAME
            || (gConfig.walkSpeeds[gbThisNetHumanPlayer[giCurPlayer]]
                    == CONFIG_WALK_SPEED_INSTANT
                && !bMoveSoundMade)) {
            bMoveSoundMade = true;
            if (EveryOther == 0) {
                gpSoundManager->MemorySample(
                    m_cursorSamples[IDX(CELL_TERRAIN(GetCell(
                        m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
                        m_mapOriginY + CURSOR_MAP_DRAW_OFFSET
                    )))]
                );
            }
        }
    }

    if (!gbDrawSavedCursor) {
        S1cursorDirection = m_cursorDirection;
        S1cursorBaseFrame = m_cursorFrame;
        S1cursorFrameCount = m_cursorFrameCount;
        S1cursorCycle = m_cursorCycle;
        S1cursorTurning = m_cursorTurning;
    }
}

VA(0x00433ab7, 0x327)
void advManager::DrawCursorShadow(void) {
    i32 frame;
    i32 boatIndex;
    i32 shadowPic;
    i32 drawY;

    if (bShowIt == 0 || bSpecialHideCursor)
        return;
    if (gbDrawSavedCursor) {
        m_cursorDirection = S1cursorDirection;
        m_cursorFrame = S1cursorBaseFrame;
        m_cursorFrameCount = S1cursorFrameCount;
        m_cursorCycle = S1cursorCycle;
        m_cursorTurning = S1cursorTurning;
    }

    i32 drawX = m_updateMinX + CURSOR_DRAW_X;
    drawY = m_updateMinY + CURSOR_DRAW_Y;
    if (m_cursorType == HERO_TYPE_BOAT)
        drawY -= CURSOR_DRAW_Y - CURSOR_BOAT_DRAW_Y;

    if (m_cursorFrame & CURSOR_FLIP_FLAG) {
        drawX += CURSOR_SHADOW_FLIP_X_ADJUST;
        frame = (m_cursorFrame & CURSOR_FRAME_MASK) + m_cursorFrameCount;
        if (m_drawHeroShadows && m_cursorType == HERO_TYPE_BOAT) {
            boatIndex = frame;
            DRAW_ADVENTURE_ICON(
                m_boatShadowIcon,
                drawX - CURSOR_SHADOW_FLIP_X_ADJUST,
                drawY,
                boatIndex
                    + (boatIndex >= CURSOR_SHADOW_ANIM_FIRST && boatIndex < CURSOR_SHADOW_ANIM_END
                           ? CURSOR_BOAT_SHADOW_OFFSET
                           : 0),
                ICON_DRAW_CLIP
            );
        } else if (m_drawHeroShadows && m_cursorType != HERO_TYPE_BOAT) {
            shadowPic = frame;
            if (shadowPic == CURSOR_HERO_TURN_FRAME_51)
                shadowPic = CURSOR_HERO_TURN_SHADOW_FRAME_56;
            if (shadowPic == CURSOR_HERO_TURN_FRAME_50)
                shadowPic = CURSOR_HERO_TURN_SHADOW_FRAME_57;
            if (shadowPic == CURSOR_HERO_TURN_FRAME_49)
                shadowPic = CURSOR_HERO_TURN_SHADOW_FRAME_58;
            if (shadowPic == CURSOR_HERO_TURN_FRAME_47)
                shadowPic = CURSOR_HERO_TURN_SHADOW_FRAME_55;
            if (shadowPic == CURSOR_HERO_TURN_FRAME_46)
                shadowPic = CURSOR_HERO_TURN_SHADOW_FRAME_55;
            DRAW_ADVENTURE_ICON(
                m_shadowIcon,
                drawX - CURSOR_SHADOW_FLIP_X_ADJUST,
                drawY,
                shadowPic
                    + (shadowPic >= CURSOR_SHADOW_ANIM_FIRST && shadowPic < CURSOR_SHADOW_ANIM_END
                           ? CURSOR_HORSE_SHADOW_OFFSET
                           : 0),
                ICON_DRAW_CLIP
            );
        }
    } else {
        frame = m_cursorFrame + m_cursorFrameCount;
        if (m_drawHeroShadows && m_cursorType == HERO_TYPE_BOAT) {
            DRAW_ADVENTURE_ICON(m_boatShadowIcon, drawX, drawY, frame, ICON_DRAW_CLIP);
        } else if (m_drawHeroShadows && m_cursorType != HERO_TYPE_BOAT) {
            DRAW_ADVENTURE_ICON(m_shadowIcon, drawX, drawY, frame, ICON_DRAW_CLIP);
        }
    }

    if (!gbDrawSavedCursor) {
        S1cursorDirection = m_cursorDirection;
        S1cursorBaseFrame = m_cursorFrame;
        S1cursorFrameCount = m_cursorFrameCount;
        S1cursorCycle = m_cursorCycle;
        S1cursorTurning = m_cursorTurning;
    }
}

VA(0x00433dde, 0x4e)
i32 advManager::GetCursorBaseFrame(H2_ENUM_PARAM(MapDirection, i32) direction) {
    if (direction > MAP_DIRECTION_SOUTH) {
        switch (direction) {
            case MAP_DIRECTION_SOUTH_WEST:
                return CURSOR_BOAT_BASE_FRAME_5;
            case MAP_DIRECTION_WEST:
                return CURSOR_BOAT_BASE_FRAME_6;
            case MAP_DIRECTION_NORTH_WEST:
                return CURSOR_BOAT_BASE_FRAME_7;
            default:
                return 0;
        }
    } else {
        return IDX(direction) * CURSOR_FRAMES_PER_DIRECTION;
    }
}

#if H2_RETAIL_COMPILER
#define increment inc
#endif
VA(0x00433e2c, 0x213)
void advManager::TurnTo(H2_ENUM_PARAM(MapDirection, i32) direction) {
    i32 increment = 1;
    i32 directionDifference = IDX(direction) - IDX(m_cursorDirection);
    if (directionDifference == 0)
        return;
    if ((directionDifference < 0 && directionDifference >= -DIRECTION_HALF_COUNT)
        || (directionDifference > 0 && directionDifference > DIRECTION_HALF_COUNT))
        increment = -1;
    m_cursorTurning = 1;
    i32 frameIndex = IDX(m_cursorDirection) * TURN_FRAME_MULTIPLIER;
    i32 delayTime =
        giStepDelay[IDX(gConfig.walkSpeeds[gbThisNetHumanPlayer[giCurPlayer]])];
    if (gConfig.walkSpeeds[gbThisNetHumanPlayer[giCurPlayer]]
        == CONFIG_WALK_SPEED_SLOWEST)
        delayTime *= CURSOR_SLOW_TURN_MULTIPLIER;
    if (gConfig.walkSpeeds[gbThisNetHumanPlayer[giCurPlayer]]
        == CONFIG_WALK_SPEED_SLOW)
        delayTime = static_cast<i32>(delayTime * SLOW_TURN_DELAY_SCALE);

    do {
        m_cursorCycle = 1;
        m_cursorFrame = m_cursorType < HERO_TYPE_BOAT ? horseFrameFlip[frameIndex]
                                                      : boatFrameFlip[frameIndex];
        m_cursorFrameCount = 0;
        glTimers[1] = KBTickCount() + delayTime;
        if (gConfig.walkSpeeds[gbThisNetHumanPlayer[giCurPlayer]]
            != CONFIG_WALK_SPEED_INSTANT) {
            if (ComboDraw(m_mapOriginX, m_mapOriginY, 0))
                UpdateScreen(0, 0);
            if (bShowIt)
                DelayTil(&glTimers[1]);
        }
        frameIndex += increment;
        if (frameIndex < 0)
            frameIndex = CURSOR_TURN_FRAME_COUNT - 1;
        frameIndex %= CURSOR_TURN_FRAME_COUNT;
    } while (frameIndex != IDX(direction) * TURN_FRAME_MULTIPLIER);

    m_cursorDirection = direction;
    StopCursor(1);
    if (bShowIt)
        DelayTil(&glTimers[1]);
    if (ComboDraw(m_mapOriginX, m_mapOriginY, 0))
        UpdateScreen(0, 0);
}
#if H2_RETAIL_COMPILER
#undef increment
#endif

VA(0x0043403f, 0x8b)
b32 advManager::GetMoveShowIt(
    hero* movingHero,
    H2_ENUM_PARAM(MapDirection, i32) direction
) {
    i32 dx = normalDirTable[IDX(direction)].x;
    i32 dy = normalDirTable[IDX(direction)].y;
    if ((gbThisNetHumanPlayer[giCurPlayer] || !gConfig.blackoutComputer)
        && (MapExtraPosAndAdjacentsSet(movingHero->m_x, movingHero->m_y, giCurWatchPlayerBit)
            || MapExtraPosAndAdjacentsSet(
                movingHero->m_x + dx,
                movingHero->m_y + dy,
                giCurWatchPlayerBit
            )))
        return 1;
    else
        return 0;
}

#if H2_RETAIL_COMPILER
#define currentCell currentCell_g
#define currentTerrain currentTerrain_a
#define cursorCell cursorCell_h
#define directionX directionX_a
#define eventCell eventCell_i
#define halfSteps halfSteps_g
#define movingHero movingHero_g
#define nextTerrainCost nextTerrainCost_n
#define step step_a
#define stepDelay stepDelay_b
#define terrainCost terrainCost_h
#endif
VA(0x004340ca, 0x10fb)
mapCell* advManager::MoveHero(
    H2_ENUM_PARAM(MapDirection, i32) direction,
    i32 stopAfterMove,
    i32* eventX,
    i32* eventY,
    i32* outOfMobility,
    i32 processEvent,
    i32* adjacentMonster,
    i32 forceMove
) {
    mapCell* destinationCell;
    i32 halfSteps;
    EventExtra* mapEvent = NULL;
    mapCell* eventCell;
    i32 nextTerrainCost;
    SAMPLE2 fizzleSample = NULL;
    mapCell* cursorCell;
    mapCell* currentCell;
    i32 step;
    hero* movingHero;
    i32 terrainCost;
    i32 stepDelay;
    H2_ENUM_STORAGE(TerrainType, i32) currentTerrain;
    i32 oldHeroX;
    i32 directionY;
    i32 oldHeroY;
    i32 directionX;
    i32 pixelsPerStep;

    if (gbThisNetHumanPlayer[giCurPlayer])
        SetNoDialogMenus(false);
    gbHitEvent = false;
    *adjacentMonster = 0;
    *outOfMobility = 0;
    gbHeroMoving = true;
    eventCell = NULL;

    movingHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    oldHeroX = movingHero->m_x;
    oldHeroY = movingHero->m_y;
    directionX = normalDirTable[IDX(direction)].x;
    directionY = normalDirTable[IDX(direction)].y;
    bShowIt = GetMoveShowIt(movingHero, direction);
    if (bShowIt)
        gbMoveShown = true;

    currentCell = GetCell(movingHero->m_x, movingHero->m_y);
    currentTerrain = CELL_TERRAIN(currentCell);
    destinationCell = GetCell(movingHero->m_x + directionX, movingHero->m_y + directionY);
    terrainCost = CalcTerrainCost(
        currentTerrain,
        IDX(direction) & 1,
        movingHero->m_remainingMobility,
        IDX(movingHero->m_secondarySkills[IDX(HERO_SKILL_PATHFINDING)]),
        currentCell->m_isRoad,
        destinationCell->m_isRoad
    );
    nextTerrainCost = CalcTerrainCost(
        CELL_TERRAIN(destinationCell),
        0,
        movingHero->m_remainingMobility - terrainCost,
        IDX(movingHero->m_secondarySkills[IDX(HERO_SKILL_PATHFINDING)]),
        destinationCell->m_isRoad,
        1
    );

    if (!forceMove && movingHero->m_remainingMobility < terrainCost) {
        *outOfMobility = 1;
        movingHero->m_remainingMobility = 0;
        StopCursor(1);
        goto movementDone;
    }

    SendMapChange(
        MAP_CHANGE_MOVE_HERO,
        movingHero->m_id,
        static_cast<u8>(movingHero->m_x),
        static_cast<u8>(movingHero->m_y),
        giCurPlayer,
        static_cast<u8>(stopAfterMove),
        static_cast<u8>(direction)
    );
    MobilizeCurrHero(0);
    *eventX = movingHero->m_x + directionX;
    *eventY = movingHero->m_y + directionY;
    if (m_cursorDirection != direction)
        TurnTo(direction);
    movingHero->m_direction = direction;

    if (movingHero->IsEmbarked() && destinationCell->m_triggerType == MAP_OBJECT_COAST) {
        for (step = 0; step < GAME_BOAT_COUNT; ++step) {
            if (gpGame->m_boats[step].heroId == movingHero->m_id)
                break;
        }
        boatRecord* boat = &gpGame->m_boats[step];
        mapCell* boatCell = GetCell(movingHero->m_x, movingHero->m_y);
        boat->savedTriggerType = boatCell->m_triggerType;
        boat->savedEventData = static_cast<u8>(boatCell->m_objectMetadata);
        boat->direction = m_cursorDirection;
        boat->heroId |= BOAT_OCCUPIED_FLAG;
        boatCell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_BOAT);
        boatCell->m_objectMetadata = static_cast<u16>(step);
        boat->x = static_cast<i8>(movingHero->m_x);
        boat->y = static_cast<i8>(movingHero->m_y);
        StopCursor(1);
        CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
        UpdateScreen(0, 0);
        m_cursorActive = false;
    }

    if (HAS(destinationCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)
        && gpAdvManager->ValidMoveWithEvent(movingHero, direction)) {
        switch (destinationCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
            case MAP_OBJECT_BOAT:
                if (movingHero->IsEmbarked())
                    goto movementDone;
                StopCursor(1);
                m_cursorActive = false;
                fizzleSample = LoadPlaySample("killfade.82m");
                gpWindowManager->SaveFizzleSource(
                    CURSOR_FIZZLE_X,
                    CURSOR_FIZZLE_Y,
                    CURSOR_FIZZLE_WIDTH,
                    CURSOR_FIZZLE_HEIGHT
                );
                CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
                gpWindowManager->FizzleForward(
                    CURSOR_FIZZLE_X,
                    CURSOR_FIZZLE_Y,
                    CURSOR_FIZZLE_WIDTH,
                    CURSOR_FIZZLE_HEIGHT,
                    gbThisNetHumanPlayer[giCurPlayer] ? CURSOR_INVALID_POSITION
                                                      : CURSOR_FIZZLE_COMPUTER_TYPE,
                    NULL,
                    NULL
                );
                WaitEndSample(&fizzleSample);
                break;

            case MAP_OBJECT_HERO_INTERACTION:
                if (movingHero->IsEmbarked()) {
                    if (gpGame->GetHero(destinationCell->m_objectMetadata)->IsEmbarked())
                        goto stoppingEvent;
                    else
                        goto movementDone;
                } else {
                    goto stoppingEvent;
                }

            case MAP_OBJECT_CASTLE:
                if (gpGame->GetTown(destinationCell->m_objectMetadata)->m_owner != giCurPlayer
                    && gpGame->GetTown(destinationCell->m_objectMetadata)->HasGarrison()) {
                    StopCursor(1);
                    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
                    UpdateScreen(0, 0);
                    movingHero->m_remainingMobility -= terrainCost;
                    if (movingHero->m_remainingMobility < nextTerrainCost) {
                        movingHero->m_remainingMobility = 0;
                        stopAfterMove = 1;
                    }
                    eventCell = destinationCell;
                    goto movementDone;
                }
                break;

            default:
                if (StopOnTrigger(destinationCell)) {
                stoppingEvent:
                    StopCursor(1);
                    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
                    UpdateScreen(0, 0);
                    movingHero->m_remainingMobility -= terrainCost;
                    if (movingHero->m_remainingMobility < nextTerrainCost) {
                        movingHero->m_remainingMobility = 0;
                        stopAfterMove = 1;
                    }
                    eventCell = destinationCell;
                    goto movementDone;
                }
                break;
        }
    }

    if (!ValidMove(direction, 0))
        goto movementDone;

    if (movingHero->m_locationType == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))) {
        town* occupiedTown = gpGame->GetTown(movingHero->m_occupiedTown);
        occupiedTown->m_occupyingHeroId = -1;
    }
    if (m_visibilityMapValid) {
        *(m_visibilityMap + (movingHero->m_x + directionX)
          + (movingHero->m_y + directionY) * MAP_WIDTH) = 0;
    }
    m_updateMinY = 0;
    m_updateMinX = 0;
    gpGame->SetVisibility(
        m_mapOriginX + directionX + CURSOR_MAP_DRAW_OFFSET,
        m_mapOriginY + directionY + CURSOR_MAP_DRAW_OFFSET,
        giCurPlayer,
        HERO_SCOUTING_VISIBILITY_RADIUS(*movingHero)
    );
    m_forceCompleteDraw = true;

    pixelsPerStep =
        giPixelsPerStep[IDX(gConfig.walkSpeeds[gbThisNetHumanPlayer[giCurPlayer]])];
    stepDelay =
        giStepDelay[IDX(gConfig.walkSpeeds[gbThisNetHumanPlayer[giCurPlayer]])];
    StartCursor(direction);
    if (gConfig.walkSpeeds[gbThisNetHumanPlayer[giCurPlayer]]
        == CONFIG_WALK_SPEED_INSTANT) {
        if (EveryOther)
            --m_cursorFrame;
        bMoveSoundMade = false;
        MoveOrigin(directionX, directionY);
        movingHero->m_x += directionX;
        movingHero->m_y += directionY;
        if (ComboDraw(0))
            UpdateScreen(0, 0);
        EveryOther = !EveryOther;
    } else {
        if (directionX == 1 && directionY == -1) {
            giDeferObjDrawX = movingHero->m_x + directionX;
            giDeferObjDrawY = movingHero->m_y;
        } else if (directionX == -1 && directionY == 1) {
            giDeferObjDrawX = movingHero->m_x;
            giDeferObjDrawY = movingHero->m_y + directionY;
        } else if (directionX == 1 && directionY == 1) {
            giDeferObjDrawX = movingHero->m_x;
            giDeferObjDrawY = movingHero->m_y + directionY;
        } else if (directionX == -1 && directionY == -1) {
            /* retail adds the Y delta here; both are -1 in this arm */
            giDeferObjDrawX = movingHero->m_x + directionY;
            giDeferObjDrawY = movingHero->m_y;
        }

        gbEnlargeScreenBlit = false;
        gbNoBorder = true;
        halfSteps = CURSOR_MOVE_HALF_TILE_PIXELS / pixelsPerStep;
        for (step = 0; step < halfSteps * MOVE_TILE_HALF_COUNT; ++step) {
            i32 tick;

            if (step == halfSteps) {
                MoveOrigin(directionX, directionY);
                movingHero->m_x += directionX;
                movingHero->m_y += directionY;
                m_updateMinX = startVals[directionX + 1];
                m_updateMinY = startVals[directionY + 1];
            }
            tick = KBTickCount();
            if (step + 1 == halfSteps * MOVE_TILE_HALF_COUNT) {
                m_updateMinX = 0;
                m_updateMinY = 0;
            } else {
                m_updateMinX += directionX * pixelsPerStep;
                m_updateMinY += directionY * pixelsPerStep;
            }
            if (ComboDraw(0)) {
                giLimitUpdMinX = -1;
                UpdateScreen(0, 0);
            }
            if (bShowIt)
                DelayTilMilli(tick + stepDelay);
        }
        giDeferObjDrawX = giDeferObjDrawY = -1;
        gbNoBorder = false;
        DrawAdventureBorder();
        gbEnlargeScreenBlit = true;
    }

    movingHero->m_remainingMobility -= terrainCost;
    if (movingHero->m_remainingMobility < nextTerrainCost) {
        movingHero->m_remainingMobility = 0;
        stopAfterMove = 1;
    }
    mapEvent = GetMapEvent(*eventX, *eventY);
    if (mapEvent && !processEvent)
        stopAfterMove = 1;
    StopCursor(stopAfterMove);
    if (processEvent && stopAfterMove && ComboDraw(0))
        UpdateScreen(0, 0);

    SetEnvironmentOrigin(
        m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
        m_mapOriginY + CURSOR_MAP_DRAW_OFFSET,
        0
    );
    step = GetCell(m_mapOriginX + CURSOR_MAP_DRAW_OFFSET, m_mapOriginY + CURSOR_MAP_DRAW_OFFSET)
                 ->m_terrainImageIndex;
    if (giGroundToTerrain[step] != m_currentTerrain) {
        m_currentTerrain = giGroundToTerrain[step];
        if (gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI)
            gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
    }
    m_updateMinY = 0;
    m_updateMinX = 0;

    cursorCell = GetCell(m_mapOriginX + m_cursorMapX, m_mapOriginY + m_cursorMapY);
    *eventX = m_mapOriginX + m_cursorMapX;
    *eventY = m_mapOriginY + m_cursorMapY;
    if (HAS(cursorCell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)
        || (movingHero->IsEmbarked() && cursorCell->m_triggerType == MAP_OBJECT_COAST)) {
        eventCell = cursorCell;
        switch (cursorCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
            case MAP_OBJECT_NOTHING_SPECIAL:
            case MAP_OBJECT_MOSSY_ROCK:
            case MAP_OBJECT_REEFS:
            case MAP_OBJECT_TREES:
            case MAP_OBJECT_MOUNTAINS:
            case MAP_OBJECT_VOLCANO:
            case MAP_OBJECT_FLOWERS:
            case MAP_OBJECT_ROCK:
            case MAP_OBJECT_WATER_LAKE:
            case MAP_OBJECT_MANDRAKE:
            case MAP_OBJECT_DEAD_TREE:
            case MAP_OBJECT_STUMP:
            case MAP_OBJECT_CRATER:
            case MAP_OBJECT_CACTUS:
            case MAP_OBJECT_MOUND:
            case MAP_OBJECT_DUNE:
            case MAP_OBJECT_LAVA_POOL:
            case MAP_OBJECT_SHRUB:
            case MAP_OBJECT_ARENA:
            case MAP_OBJECT_BARROW_MOUNDS:
                eventCell = NULL;
        }
        goto movementDone;
    }
    goto movementDone;

movementDone:
    UpdateRadar(1, 1);
    gbHeroMoving = false;
    if (!forceMove) {
        if (oldHeroX != movingHero->m_x || oldHeroY != movingHero->m_y) {
            if (*(mapExtra + movingHero->m_x + movingHero->m_y * MAP_WIDTH)
                & IDX(MAP_EXTRA_ADJACENT_MONSTER)) {
                if (movingHero->IsEmbarked())
                    goto adjacentDone;
                if (eventCell
                    && (eventCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_BOAT)
                    goto adjacentDone;
                CheckAdjacentMon(adjacentMonster);
                if (movingHero->m_owner != giCurPlayer)
                    eventCell = NULL;
            }
        }
    }
adjacentDone:
    if (gbThisNetHumanPlayer[giCurPlayer])
        SetNoDialogMenus(true);

    if (mapEvent) {
        if (processEvent) {
            if (mapEvent->applyToComputer) {
                for (step = 0; step < IDX(RES_COUNT); ++step) {
                    gpGame->m_players[giCurPlayer].m_resources[step] +=
                        mapEvent->resources[step];
                    if (gpGame->m_players[giCurPlayer].m_resources[step] < 0)
                        gpGame->m_players[giCurPlayer].m_resources[step] = 0;
                }
                if (mapEvent->artifact != -1
                    && movingHero->NumArtifacts() < HERO_ARTIFACT_SLOT_COUNT)
                    GiveArtifact(movingHero, ArtifactType(mapEvent->artifact), true);
                if (mapEvent->cancelAfterVisit)
                    mapEvent->active = false;
            }
        } else {
            i32 primaryType = -1;
            i32 primaryAmount = 0;
            i32 secondaryType = -1;
            i32 secondaryAmount = 0;
            for (step = 0; step < IDX(RES_COUNT); ++step) {
                i32 eventAmount = mapEvent->resources[step];
                if (-eventAmount > gpGame->m_players[giCurPlayer].m_resources[step]) {
                    eventAmount = -gpGame->m_players[giCurPlayer].m_resources[step];
                }
                gpGame->m_players[giCurPlayer].m_resources[step] += mapEvent->resources[step];
                if (gpGame->m_players[giCurPlayer].m_resources[step] < 0)
                    gpGame->m_players[giCurPlayer].m_resources[step] = 0;
                if (eventAmount) {
                    if (primaryType != -1) {
                        secondaryType = primaryType;
                        secondaryAmount = primaryAmount;
                    }
                    primaryType = step;
                    primaryAmount = eventAmount;
                }
            }
            if (mapEvent->artifact != -1
                && movingHero->NumArtifacts() < HERO_ARTIFACT_SLOT_COUNT) {
                GiveArtifact(movingHero, ArtifactType(mapEvent->artifact), true);
                if (primaryType != -1) {
                    secondaryType = primaryType;
                    secondaryAmount = primaryAmount;
                }
                primaryType = NORMAL_DIALOG_ARTIFACT;
                primaryAmount = mapEvent->artifact;
            }
            if (mapEvent->cancelAfterVisit)
                mapEvent->active = false;
            if (primaryType >= 0 && primaryType <= CURSOR_RESOURCE_LAST && primaryAmount < 0)
                primaryAmount -= CURSOR_RESOURCE_DIALOG_PENALTY;
            if (secondaryType >= 0 && secondaryType <= CURSOR_RESOURCE_LAST
                && secondaryAmount < 0)
                secondaryAmount -= CURSOR_RESOURCE_DIALOG_PENALTY;
            NormalDialog(
                mapEvent->message,
                1,
                -1,
                -1,
                primaryType,
                primaryAmount,
                secondaryType,
                secondaryAmount,
                -1,
                0
            );
            gbHitEvent = true;
        }
    }
    return eventCell;
}
#if H2_RETAIL_COMPILER
#undef currentCell
#undef currentTerrain
#undef cursorCell
#undef directionX
#undef eventCell
#undef halfSteps
#undef movingHero
#undef nextTerrainCost
#undef step
#undef stepDelay
#undef terrainCost
#endif

#if H2_RETAIL_COMPILER
#define currentHero currentHero_f
#define heroCell heroCell_f
#define killed killed_e
#define monsterCell monsterCell_d
#define monsterX monsterX_e
#define monsterY monsterY_f
#endif
VA(0x004351c5, 0x156)
void advManager::CheckAdjacentMon(i32* adjacentMonster) {
    hero* currentHero;
    i32 killed;
    i32 monsterX;
    i32 monsterY;
    mapCell* monsterCell;
    mapCell* heroCell;

    currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    killed = 0;
    if (FindAdjacentMonster(
            currentHero->m_x,
            currentHero->m_y,
            &monsterX,
            &monsterY,
            -1,
            -1
        )) {
        StopCursor(1);
        CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
        UpdateScreen(0, 0);
        monsterCell = GetCell(monsterX, monsterY);
        heroCell = GetCell(currentHero->m_x, currentHero->m_y);
        if (gbThisNetHumanPlayer[giCurPlayer])
            PlayerMonsterInteract(
                monsterCell,
                heroCell,
                currentHero,
                &killed,
                currentHero->m_x,
                currentHero->m_y,
                1,
                monsterX,
                monsterY
            );
        else
            ComputerMonsterInteract(monsterCell, currentHero, &killed);
        if (killed) {
            EraseObj(monsterCell, monsterX, monsterY);
            if (gbThisNetHumanPlayer[giCurPlayer])
                FizzleCenter(EVENT_FIZZLE_HERO_LOSS);
        }
        *adjacentMonster = 1;
    }
}
#if H2_RETAIL_COMPILER
#undef currentHero
#undef heroCell
#undef killed
#undef monsterCell
#undef monsterX
#undef monsterY
#endif

VA(0x0043531b, 0x116)
i32 advManager::ValidMoveWithEvent(
    hero* movingHero,
    H2_ENUM_PARAM(MapDirection, i32) direction
) {
    i32 dirX;
    i32 newX;
    i32 dirY;
    i32 newY;
    mapCell* destinationCell;

    dirX = normalDirTable[IDX(direction)].x;
    dirY = normalDirTable[IDX(direction)].y;
    newX = movingHero->m_x + dirX;
    newY = movingHero->m_y + dirY;
    if (newX < 0 || newX > MAP_WIDTH - 1 || newY < 0
        || newY > MAP_HEIGHT - 1)
        return 0;

    destinationCell = m_mapData->GetCell(newX, newY);
    switch (destinationCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
        case MAP_OBJECT_HERO_INTERACTION:
            if (movingHero->IsEmbarked()) {
                if (gpGame->GetHero(destinationCell->m_objectMetadata)->IsEmbarked())
                    return 1;
                else
                    return 0;
            }
            return 1;
        default:
            return ValidMove(direction, 1);
    }
}

#if H2_RETAIL_COMPILER
#define currentCell currentCell_c
#define destinationCellY destinationCellY_f
#define destinationMapX destinationMapX_b
#define directionY directionY_c
#define northDirection northDirection_a
#define northNeighborCell northNeighborCell_a
#define southDirection southDirection_a
#define southNeighborCell southNeighborCell_i
#endif
VA(0x00435431, 0x457)
i32 advManager::ValidMove(H2_ENUM_PARAM(MapDirection, i32) direction, i32 eventMode) {
    mapCell* destinationCell;
    i32 destinationMapY;
    i32 southDirection;
    i32 northDirection;
    i32 destinationMapX;
    i32 destinationCellX;
    mapCell* currentCell;
    i32 destinationCellY;
    i32 directionX;
    i32 centerX;
    i32 directionY;
    mapCell* southNeighborCell;
    mapCell* northNeighborCell;
    i32 centerY;

    directionX = normalDirTable[IDX(direction)].x;
    directionY = normalDirTable[IDX(direction)].y;
    destinationMapX = m_mapOriginX + directionX;
    destinationMapY = m_mapOriginY + directionY;
    centerX = m_mapOriginX + CURSOR_MAP_DRAW_OFFSET;
    centerY = m_mapOriginY + CURSOR_MAP_DRAW_OFFSET;
    destinationCellX = destinationMapX + CURSOR_MAP_DRAW_OFFSET;
    destinationCellY = destinationMapY + CURSOR_MAP_DRAW_OFFSET;

    if (destinationMapX < -CURSOR_MAP_DRAW_OFFSET
        || destinationMapX > MAP_WIDTH - CURSOR_MAP_DRAW_OFFSET - 1)
        return 0;
    if (destinationMapY < -CURSOR_MAP_DRAW_OFFSET
        || destinationMapY > MAP_HEIGHT - CURSOR_MAP_DRAW_OFFSET - 1)
        return 0;

    destinationCell = m_mapData->GetCell(destinationCellX, destinationCellY);
    currentCell = m_mapData->GetCell(centerX, centerY);
    if (destinationCell->m_flags & IDX(MAP_CELL_OCCUPIED))
        return 0;

    if (CELL_TERRAIN(destinationCell) == TERRAIN_WATER) {
        if (m_cursorType != HERO_TYPE_BOAT
            && destinationCell->m_triggerType != (MAP_ACTION_TRIGGER(MAP_OBJECT_BOAT))
            && destinationCell->m_triggerType != (MAP_ACTION_TRIGGER(MAP_OBJECT_SHIPWRECK)))
            return 0;
        if (CELL_TERRAIN(currentCell) == TERRAIN_WATER && directionX != 0 && directionY != 0) {
            if (CELL_TERRAIN(m_mapData->GetCell(centerX + directionX, centerY)) != TERRAIN_WATER
                || CELL_TERRAIN(m_mapData->GetCell(centerX, centerY + directionY))
                       != TERRAIN_WATER)
                return 0;
        }
    } else if (m_cursorType == HERO_TYPE_BOAT
               && destinationCell->m_triggerType != MAP_OBJECT_COAST) {
        return 0;
    }

    northDirection = (1 << IDX(direction)) & CURSOR_NORTH_DIRECTION_MASK;
    southDirection = (1 << IDX(direction)) & CURSOR_SOUTH_DIRECTION_MASK;
    if (northDirection) {
        if (CELL_HAS_NON_SHADOW_OBJECT(currentCell)
            && currentCell->m_triggerType != (MAP_ACTION_TRIGGER(MAP_OBJECT_WHIRLPOOL)))
            return 0;
        if (destinationCell->m_overlayIndex != MAPCELL_SPRITE_NONE) {
            northNeighborCell = m_mapData->GetCell(destinationCellX, destinationCellY + 1);
            if (CELL_HAS_NON_SHADOW_OBJECT(northNeighborCell))
                return 0;
        }
    }
    if (southDirection) {
        if (CELL_HAS_NON_SHADOW_OBJECT(destinationCell)
            && destinationCell->m_triggerType != (MAP_ACTION_TRIGGER(MAP_OBJECT_WHIRLPOOL))
            && (!eventMode || !(destinationCell->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                || !StopOnTrigger(destinationCell)))
            return 0;
        if (currentCell->m_overlayIndex != MAPCELL_SPRITE_NONE) {
            southNeighborCell =
                m_mapData->GetCell(m_mapOriginX + m_cursorMapX, m_mapOriginY + m_cursorMapY + 1);
            if (CELL_HAS_NON_SHADOW_OBJECT(southNeighborCell)
                && !(southNeighborCell->m_triggerType & MAP_TRIGGER_ACTION_FLAG))
                return 0;
        }
    }
    return 1;
}
#if H2_RETAIL_COMPILER
#undef currentCell
#undef destinationCellY
#undef destinationMapX
#undef directionY
#undef northDirection
#undef northNeighborCell
#undef southDirection
#undef southNeighborCell
#endif

VA(0x00435888, 0x250)
void advManager::MoveOrigin(i32 directionX, i32 directionY) {
    i32 oldOriginX;
    i32 oldOriginY;
    i32 cellX;
    i32 cellY;

    oldOriginX = m_mapOriginX;
    oldOriginY = m_mapOriginY;
    m_mapOriginX += directionX;
    m_mapOriginY += directionY;
    directionX = oldOriginX - m_mapOriginX;
    directionY = oldOriginY - m_mapOriginY;
    if (directionX != 0 || directionY != 0) {
        m_mapData->GetCell(oldOriginX + m_cursorMapX, oldOriginY + m_cursorMapY)->m_flags
            &= ~CURSOR_MAP_VISIBLE_FLAG;
        m_cursorMapX += directionX;
        m_cursorMapY += directionY;
        cellX = m_mapOriginX + m_cursorMapX;
        cellY = m_mapOriginY + m_cursorMapY;
        m_mapData->GetCell(cellX, cellY)->m_flags |= CURSOR_MAP_VISIBLE_FLAG;
        if (m_previousCursorMapX != CURSOR_INVALID_POSITION) {
            m_mapData
                ->GetCell(oldOriginX + m_previousCursorMapX, oldOriginY + m_previousCursorMapY)
                ->m_flags &= ~CURSOR_MAP_VISIBLE_FLAG;
            m_previousCursorMapX += directionX;
            m_previousCursorMapY += directionY;
            cellX = m_mapOriginX + m_previousCursorMapX;
            cellY = m_mapOriginY + m_previousCursorMapY;
            m_mapData->GetCell(cellX, cellY)->m_flags |= CURSOR_MAP_VISIBLE_FLAG;
        }
    }
    m_forceCompleteDraw = true;
}

#if H2_RETAIL_COMPILER
#define eventCell eventCell_a
#define eventX eventX_f
#define mapHero mapHero_b
#endif
VA(0x00435ad8, 0x705)
void advManager::ProcessMapChange(SMapChange change) {
    i32 outOfMobility;
    i32 adjacentMonster;
    mapCell* eventCell;
    hero* mapHero;
    i32 eventY;
    i32 eventX;

    giMapChangeCtr = change.sequence + 1;
    if (giCurPlayer != change.player) {
        gpAdvManager->DeactivateCurrTown();
        gpAdvManager->DeactivateCurrHero();
    }
    if (change.player >= 0 && change.player < GAME_PLAYER_COUNT) {
        giCurPlayer = change.player;
        gpCurPlayer = &gpGame->m_players[giCurPlayer];
        giCurPlayerBit = 1 << giCurPlayer;
    }

    switch (change.type) {
        case MAP_CHANGE_MOVE_HERO:
            LogInt(
                "MC Move Hero",
                change.id,
                change.x,
                change.y,
                IDX(change.movement.direction),
                change.sequence,
                gpGame->GetHero(change.id)->m_x,
                gpGame->GetHero(change.id)->m_y
            );
            mapHero = gpGame->GetHero(change.id);
            if (mapHero->m_x != change.x || mapHero->m_y != change.y) {
                sprintf(
                    gText,
                    "Data miscommunication in hero position, first %d, %d, second %d, %d.  Please "
                    "give Phil a copy of  your Autosave and, if possible, instructions to recreate "
                    "this error",
                    mapHero->m_x,
                    mapHero->m_y,
                    change.x,
                    change.y
                );
                LogStr(gText);
                break;
            }
            gpAdvManager->SetHeroContext(change.id, 0);
            eventCell = MoveHero(
                change.movement.direction,
                change.movement.stopAfterMove,
                &eventX,
                &eventY,
                &outOfMobility,
                1,
                &adjacentMonster,
                1
            );
            if (eventCell != NULL) {
                switch (eventCell->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
                    case MAP_OBJECT_COAST:
                    case MAP_OBJECT_BOAT:
                        DoAIEvent(eventCell, mapHero, eventX, eventY);
                        break;
                }
            }
            break;

        case MAP_CHANGE_MY_TURN:
            LogInt("MC My Turn", change.x, change.y);
            break;

        case MAP_CHANGE_TELEPORT_HERO:
            LogInt("MC Teleport Hero", change.x, change.y);
            mapHero = gpGame->GetHero(change.id);
            TeleportTo(mapHero, change.x, change.y, 0, 1);
            break;

        case MAP_CHANGE_CLAIM_MINE:
            LogInt("MC ClaimMine", LOG_UNUSED_VALUE);
            gpGame->ClaimMine(change.id, change.player);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case MAP_CHANGE_CLAIM_TOWN:
            LogInt("MC ClaimTown", LOG_UNUSED_VALUE);
            gpGame->ClaimTown(change.id, change.player, 1);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case MAP_CHANGE_BUILD_BOAT:
            LogInt("MC BuildBoat", LOG_UNUSED_VALUE);
            gpGame->CreateBoat(change.x, change.y, 1);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case MAP_CHANGE_ERASE_OBJECT:
            LogInt("MC Erase Object", change.x, change.y);
            eventCell = GetCell(change.x, change.y);
            EraseObj(eventCell, change.x, change.y);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case MAP_CHANGE_DEAD_HERO:
            LogStr("MC DeadHero");
            mapHero = gpGame->GetHero(change.id);
            if (mapHero->m_x != change.x || mapHero->m_y != change.y)
                break;
            mapHero->Deallocate(1);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case MAP_CHANGE_RECRUIT_HERO:
            LogStr("MC RecruitHero");
            mapHero = gpGame->GetHero(change.id);
            mapHero->m_x = change.x;
            mapHero->m_y = change.y;
            mapHero->m_eventFlags = HERO_EVENT_NONE;
            mapHero->m_direction = MAP_DIRECTION_EAST;
            mapHero->m_locationType =
                gpGame->m_worldMap.GetCell(change.x, change.y)->m_triggerType;
            mapHero->m_occupiedTown =
                gpGame->m_worldMap.GetCell(change.x, change.y)->m_objectMetadata;
            mapHero->m_owner = change.player;
            gpGame->m_worldMap.GetCell(change.x, change.y)->m_triggerType =
                MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION);
            gpGame->m_worldMap.GetCell(change.x, change.y)->m_objectMetadata = change.id;
            gpAdvManager->SetHeroContext(change.id, 0);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case MAP_CHANGE_DEAD_PLAYER:
            LogStr("Dead Player");
            sprintf(
                gText,
                localization::Tr("player.vanquished"),
                cPlayerNames[change.id]
            );
            NormalDialog(
                gText,
                NORMAL_DIALOG_INFO,
                NORMAL_DIALOG_NO_RESOURCE,
                NORMAL_DIALOG_NO_RESOURCE,
                NORMAL_DIALOG_CREST,
                gpGame->GetPlayerColor(change.id),
                NORMAL_DIALOG_NO_RESOURCE,
                NORMAL_DIALOG_NO_RESOURCE,
                NORMAL_DIALOG_NO_RESOURCE,
                CURSOR_DEAD_PLAYER_DIALOG_TIME
            );
            break;

        case MAP_CHANGE_UNUSED:
        default:
            break;
    }
}
#if H2_RETAIL_COMPILER
#undef eventCell
#undef eventX
#undef mapHero
#endif

VA(0x004361dd, 0x189)
void advManager::ProcessIncomingSingleMapChange(SMapChange* incoming) {
    i32 slot;

    if (incoming->sequence < giMapChangeCtr)
        return;

    if (incoming->sequence == giMapChangeCtr) {
        ProcessMapChange(*incoming);
    } else {
    retryInsert:
        for (slot = 0; slot < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++slot) {
            if (sMapChangeQueue[slot].type != MAP_CHANGE_NONE
                && sMapChangeQueue[slot].sequence == incoming->sequence) {
                LogInt("OQ", incoming->sequence, giMapChangeCtr);
                goto duplicateChange;
            }
        }

        for (slot = 0; slot < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++slot) {
            if (sMapChangeQueue[slot].type == MAP_CHANGE_NONE) {
                LogInt("SQ", incoming->sequence, giMapChangeCtr);
                sMapChangeQueue[slot] = *incoming;
                goto duplicateChange;
            }
        }

        UnwindMapChangeQueue(1, 1);
        goto retryInsert;

    duplicateChange:
        ++slot;
    }
    UnwindMapChangeQueue(0, 1);
}

#if H2_RETAIL_COMPILER
#define buffer buf
#define index ix
#define mapChange ptr0
#endif
VA(0x00436366, 0xa7)
void advManager::ProcessIncomingGroupMapChange(char* incomingData) {
    SMapChange* mapChange;
    i32 size;
    SMapChange* buffer;
    i32 index;
    i32 H2_UNUSED(processed);

    size = sizeof(sMapChangeLastFew);
    buffer = static_cast<SMapChange*>(H2_ALLOC(size));
    memcpy(buffer, incomingData, size);
    for (index = CURSOR_MAP_CHANGE_RECENT_COUNT - 1; index >= 0; --index) {
        mapChange = &buffer[index];
        if (mapChange->type != MAP_CHANGE_NONE && mapChange->sequence >= giMapChangeCtr) {
            ProcessIncomingSingleMapChange(mapChange);
        } else {
            processed = 0;
        }
    }
    H2_FREE(buffer);
}
#if H2_RETAIL_COMPILER
#undef buffer
#undef index
#undef mapChange
#endif

VA(0x0043640d, 0x5e)
void advManager::PurgeMapChangeQueue(void) {
    i32 slot;

    for (slot = 0; slot < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++slot)
        sMapChangeQueue[slot].type = MAP_CHANGE_NONE;
    for (slot = 0; slot < CURSOR_MAP_CHANGE_RECENT_COUNT; ++slot)
        sMapChangeLastFew[slot].type = MAP_CHANGE_NONE;
}

#if H2_RETAIL_COMPILER
#define index n
#define queueIndex pos1
#endif
VA(0x0043646b, 0x1b5)
void advManager::UnwindMapChangeQueue(i32 maximumToUnwind, i32 processChanges) {
    i32 queueCount;
    i32 unwoundChanges;
    i32 queueIndex;
    i32 lowestSequence;
    b32 continueUnwinding;
    i32 index;

    queueCount = CURSOR_MAP_CHANGE_PENDING_SENTINEL;
    unwoundChanges = 0;
    while (queueCount > 0 && unwoundChanges < maximumToUnwind) {
        queueIndex = -1;
        lowestSequence = CURSOR_MAP_CHANGE_SEQUENCE_SENTINEL;
        queueCount = 0;
        for (index = 0; index < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++index) {
            if (sMapChangeQueue[index].type != MAP_CHANGE_NONE) {
                ++queueCount;
                if (sMapChangeQueue[index].sequence < lowestSequence) {
                    lowestSequence = sMapChangeQueue[index].sequence;
                    queueIndex = index;
                }
            }
        }
        if (queueIndex != -1) {
            --queueCount;
            if (processChanges)
                ProcessMapChange(sMapChangeQueue[queueIndex]);
            sMapChangeQueue[queueIndex].type = MAP_CHANGE_NONE;
            ++unwoundChanges;
        }
    }

    continueUnwinding = true;
    while (continueUnwinding) {
        continueUnwinding = false;
        for (index = 0; index < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++index) {
            if (sMapChangeQueue[index].type != MAP_CHANGE_NONE
                && sMapChangeQueue[index].sequence == giMapChangeCtr) {
                if (processChanges)
                    ProcessMapChange(sMapChangeQueue[index]);
                sMapChangeQueue[index].type = MAP_CHANGE_NONE;
                continueUnwinding = true;
            }
        }
    }
}
#if H2_RETAIL_COMPILER
#undef index
#undef queueIndex
#endif

VA(0x00436620, 0x117)
void SendMapChange(
    MapChangeType type,
    i8 id,
    u8 x,
    u8 y,
    i32 player,
    u8 stopAfterMove,
    u8 direction
) {
    SMapChange change;

    if (!gbThisNetGotAdventureControl)
        return;
    if (!gbRemoteOn)
        return;
    if (player == MAP_CHANGE_CURRENT_PLAYER)
        player = giCurPlayer;
    LogInt("Send Map Change", type, id, x, y);
    memset(&change, 0, sizeof(change));
    change.type = type;
    change.id = id;
    change.x = x;
    change.y = y;
    change.player = static_cast<i8>(player);
    change.wire.stopAfterMove = static_cast<i8>(stopAfterMove);
    change.wire.direction = static_cast<i8>(direction);
    change.sequence = giMapChangeCtr;
    ++giMapChangeCtr;
    memmove(
        &sMapChangeLastFew[1],
        &sMapChangeLastFew[0],
        sizeof(SMapChange) * (CURSOR_MAP_CHANGE_RECENT_COUNT - 1)
    );
    sMapChangeLastFew[0] = change;
    TransmitRemoteData(
        reinterpret_cast<char*>(sMapChangeLastFew),
        CURSOR_REMOTE_PLAYER_ALL,
        sizeof(sMapChangeLastFew),
        CURSOR_REMOTE_PACKET_TYPE,
        0
    );
}

DATA(0x004f19f0) b32 bMoveSoundMade = true;
DATA(0x004f19f4) i32 giPixelsPerStep[ADVMGR_STEP_PIXEL_COUNT] = {2, 4, 6, 8, 16};
DATA(0x004f1a08) i32 giStepDelay[ADVMGR_STEP_DELAY_COUNT] = {20, 25, 20, 15, 15};
DATA(0x00524260) u8 EveryOther = 0;
DATA(0x004f1a1c) i32 startVals[ADVMGR_VIEW_WORLD_SCALE_COUNT] = {16, 0, -16};
DATA(0x00524258) i32 S1cursorCycle;
DATA(0x00524220) i32 S1cursorFrameCount;
DATA(0x0052425c) i32 S1cursorTurning;
DATA(0x00524224) i32 S1cursorBaseFrame;
DATA(0x00524254) H2_ENUM_STORAGE(MapDirection, i32) S1cursorDirection;
DATA(0x00524228) SMapChange sMapChangeLastFew[CURSOR_MAP_CHANGE_RECENT_COUNT];

#undef SLOW_TURN_DELAY_SCALE
