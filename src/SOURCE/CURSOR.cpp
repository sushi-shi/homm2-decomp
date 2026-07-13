// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\CURSOR.OBJ   from: (directly linked into exe)
// functions: 18   data: 13
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_globals_model.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Icon2b.h>
#include <BASE/Iconf2b.h>
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
#include <SOURCE/town.h>
#include <SOURCE/X_GLOBAL.h>
#include <_carcass_types.h>
VA(0x0040d5e0, 0x138)
void advManager::StartCursor(int direction)
{
    int directionX;
    int directionY;
    mapCell *cell;

    m_cursorDirection = direction;
    m_cursorFrame = GetCursorBaseFrame(direction) + 1;
    if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] > 0)
        m_cursorCycle = 1;
    else
        m_cursorCycle = 2;

    directionX = normalDirTable[direction].x;
    directionY = normalDirTable[direction].y;
    m_previousCursorMapX = m_cursorMapX;
    m_previousCursorMapY = m_cursorMapY;
    m_cursorMapX += directionX;
    m_cursorMapY += directionY;
    directionX = m_cursorMapX + m_mapOriginX;
    directionY = m_cursorMapY + m_mapOriginY;
    cell = m_mapData->GetCell(directionX, directionY);
    cell->field8 |= CURSOR_MAP_VISIBLE_FLAG;
}

VA(0x0040d718, 0x11c)
void advManager::StopCursor(int stopSound)
{
    if (stopSound) {
        bMoveSoundMade = 1;
        m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
        m_cursorFrameCount = 0;
        EveryOther = 0;
        hOldWalkSample = 0;
        hNewWalkSample = 0;
    }
    m_cursorCycle = 0;
    if (m_previousCursorMapX != -1) {
        m_mapData->GetCell(m_previousCursorMapX + m_mapOriginX,
                           m_previousCursorMapY + m_mapOriginY)->field8 &=
            ~CURSOR_MAP_VISIBLE_FLAG;
        m_previousCursorMapY = -1;
        m_previousCursorMapX = m_previousCursorMapY;
    }
    m_cursorTurning = 0;
}

VA(0x0040d834, 0x5f8)
void advManager::DrawCursor(void)
{
    int drawY;
    int drawFrame;

    if (bShowIt) {
      if (bSpecialHideCursor == 0) {
        if (gbDrawSavedCursor) {
            m_cursorDirection = S1cursorDirection;
            m_cursorFrame = S1cursorBaseFrame;
            m_cursorFrameCount = S1cursorFrameCount;
            m_cursorCycle = S1cursorCycle;
            m_cursorTurning = S1cursorTurning;
        }

        int drawX = m_updateMinX + CURSOR_DRAW_X;
        drawY = m_updateMinY + CURSOR_DRAW_Y;
        if (m_cursorType == CURSOR_HERO_TYPE_BOAT)
            drawY -= CURSOR_DRAW_Y - CURSOR_BOAT_DRAW_Y;

        if (m_cursorFrame & CURSOR_FLIP_FLAG) {
            drawX += CURSOR_FLIPPED_DRAW_X - CURSOR_DRAW_X;
            drawFrame = (m_cursorFrame & CURSOR_FRAME_MASK) +
                        m_cursorFrameCount;
            if (m_cursorType == CURSOR_HERO_TYPE_BOAT &&
                !(GetCell(m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
                          m_mapOriginY + CURSOR_MAP_DRAW_OFFSET)->field8 &
                  CURSOR_CELL_UNCOVERED_FLAG)) {
                FlipIconToBitmap(m_heroIcons[CURSOR_BOAT_WAKE_TYPE],
                                 gpWindowManager->m_screen, drawX, drawY,
                                 drawFrame, 1, 0, 0, CURSOR_CLIP_SIZE,
                                 CURSOR_CLIP_SIZE, 0);
            }
            FlipIconToBitmap(m_heroIcons[m_cursorType],
                             gpWindowManager->m_screen, drawX, drawY,
                             drawFrame, 1, 0, 0, CURSOR_CLIP_SIZE,
                             CURSOR_CLIP_SIZE, 0);
            if (m_cursorType == CURSOR_HERO_TYPE_BOAT) {
                FlipIconToBitmap(m_boatFlagIcons[gpCurPlayer->m_color],
                                 gpWindowManager->m_screen, drawX, drawY,
                                 drawFrame, 0, 0, 0, 0, 0, 0);
            } else {
                if (m_cursorCycle == 0) {
                    drawFrame = (m_cursorFrame & CURSOR_FRAME_MASK) +
                                m_updateMaxY % CURSOR_DIRECTION_COUNT +
                                CURSOR_FLAG_FRAME_BASE;
                }
                FlipIconToBitmap(m_flagIcons[gpCurPlayer->m_color],
                                 gpWindowManager->m_screen, drawX, drawY,
                                 drawFrame, 1, 0, 0, CURSOR_CLIP_SIZE,
                                 CURSOR_CLIP_SIZE, 0);
                ++m_updatePending;
            }
        } else {
            drawFrame = m_cursorFrame + m_cursorFrameCount;
            if (m_cursorType == CURSOR_HERO_TYPE_BOAT &&
                !(GetCell(m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
                          m_mapOriginY + CURSOR_MAP_DRAW_OFFSET)->field8 &
                  CURSOR_CELL_UNCOVERED_FLAG)) {
                IconToBitmap(m_heroIcons[CURSOR_BOAT_WAKE_TYPE],
                             gpWindowManager->m_screen, drawX, drawY,
                             drawFrame, 1, 0, 0, CURSOR_CLIP_SIZE,
                             CURSOR_CLIP_SIZE, 0);
            }
            IconToBitmap(m_heroIcons[m_cursorType], gpWindowManager->m_screen,
                         drawX, drawY, drawFrame, 1, 0, 0,
                         CURSOR_CLIP_SIZE, CURSOR_CLIP_SIZE, 0);
            if (m_cursorType == CURSOR_HERO_TYPE_BOAT) {
                IconToBitmap(m_boatFlagIcons[gpCurPlayer->m_color],
                             gpWindowManager->m_screen, drawX, drawY,
                             drawFrame, 0, 0, 0, 0, 0, 0);
            } else {
                if (m_cursorCycle == 0) {
                    drawFrame = (m_cursorFrame & CURSOR_FRAME_MASK) +
                                m_updateMaxY % CURSOR_DIRECTION_COUNT +
                                CURSOR_FLAG_FRAME_BASE;
                }
                IconToBitmap(m_flagIcons[gpCurPlayer->m_color],
                             gpWindowManager->m_screen, drawX, drawY,
                             drawFrame, 1, 0, 0, CURSOR_CLIP_SIZE,
                             CURSOR_CLIP_SIZE, 0);
                ++m_updatePending;
            }
        }

        if (m_cursorCycle &&
            (&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] != 4) {
            ++m_cursorFrameCount;
            if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] == 3 &&
                (m_cursorFrameCount == 4 || m_cursorFrameCount == 1))
                ++m_cursorFrameCount;
            if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] == 0 &&
                (EveryOther = 1 - EveryOther) != 0)
                --m_cursorFrameCount;
        }
        if (m_cursorFrameCount >= CURSOR_LAST_FRAME_COUNT)
            m_cursorFrameCount = 0;

        if (!m_cursorTurning) {
            if (m_cursorFrameCount == 0)
                hOldWalkSample = hNewWalkSample;
            if (m_cursorFrameCount == 3 ||
                ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] == 4 &&
                 !bMoveSoundMade)) {
                bMoveSoundMade = 1;
                if (EveryOther == 0) {
                    mapCell *cell = GetCell(
                        m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
                        m_mapOriginY + CURSOR_MAP_DRAW_OFFSET);
                    hNewWalkSample = gpSoundManager->MemorySample(
                        m_cursorSamples[giGroundToTerrain[cell->tile]]);
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
    }
}

VA(0x0040de2c, 0x36c)
void advManager::DrawCursorShadow(void)
{
    int shadowOffset;
    int boatShadowOffset;
    int drawFrame;
    int drawY;

    if (bShowIt) {
      if (bSpecialHideCursor == 0) {
        if (gbDrawSavedCursor) {
            m_cursorDirection = S1cursorDirection;
            m_cursorFrame = S1cursorBaseFrame;
            m_cursorFrameCount = S1cursorFrameCount;
            m_cursorCycle = S1cursorCycle;
            m_cursorTurning = S1cursorTurning;
        }

        int drawX = m_updateMinX;
        drawY = m_updateMinY + CURSOR_DRAW_Y;
        if (m_cursorType == CURSOR_HERO_TYPE_BOAT)
            drawY -= CURSOR_DRAW_Y - CURSOR_BOAT_DRAW_Y;

        if (m_cursorFrame & CURSOR_FLIP_FLAG) {
            drawFrame = (m_cursorFrame & CURSOR_FRAME_MASK) +
                        m_cursorFrameCount;
            if (m_drawHeroShadows &&
                m_cursorType == CURSOR_HERO_TYPE_BOAT) {
                if (drawFrame >= CURSOR_SHADOW_ANIM_FIRST &&
                    drawFrame < CURSOR_SHADOW_ANIM_END)
                    boatShadowOffset = CURSOR_BOAT_SHADOW_OFFSET;
                else
                    boatShadowOffset = 0;
                IconToBitmap(m_boatShadowIcon, gpWindowManager->m_screen,
                             drawX + CURSOR_DRAW_X, drawY,
                             drawFrame + boatShadowOffset, 1, 0, 0,
                             CURSOR_CLIP_SIZE, CURSOR_CLIP_SIZE, 0);
            } else if (m_drawHeroShadows &&
                       m_cursorType != CURSOR_HERO_TYPE_BOAT) {
                if (drawFrame == 0x33)
                    drawFrame = 0x38;
                if (drawFrame == 0x32)
                    drawFrame = 0x39;
                if (drawFrame == 0x31)
                    drawFrame = 0x3a;
                if (drawFrame == 0x2f)
                    drawFrame = 0x37;
                if (drawFrame == 0x2e)
                    drawFrame = 0x37;
                if (drawFrame >= CURSOR_SHADOW_ANIM_FIRST &&
                    drawFrame < CURSOR_SHADOW_ANIM_END)
                    shadowOffset = CURSOR_HORSE_SHADOW_OFFSET;
                else
                    shadowOffset = 0;
                IconToBitmap(m_shadowIcon, gpWindowManager->m_screen,
                             drawX + CURSOR_DRAW_X, drawY,
                             drawFrame + shadowOffset, 1, 0, 0,
                             CURSOR_CLIP_SIZE, CURSOR_CLIP_SIZE, 0);
            }
        } else {
            drawFrame = m_cursorFrame + m_cursorFrameCount;
            if (m_drawHeroShadows &&
                m_cursorType == CURSOR_HERO_TYPE_BOAT) {
                IconToBitmap(m_boatShadowIcon, gpWindowManager->m_screen,
                             drawX + CURSOR_DRAW_X, drawY,
                             drawFrame,
                             1, 0, 0, CURSOR_CLIP_SIZE,
                             CURSOR_CLIP_SIZE, 0);
            } else if (m_drawHeroShadows &&
                       m_cursorType != CURSOR_HERO_TYPE_BOAT) {
                IconToBitmap(m_shadowIcon, gpWindowManager->m_screen,
                             drawX + CURSOR_DRAW_X, drawY,
                             drawFrame,
                             1, 0, 0, CURSOR_CLIP_SIZE,
                             CURSOR_CLIP_SIZE, 0);
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
    }
}

VA(0x0040e198, 0x85)
int advManager::GetCursorBaseFrame(int direction)
{
    if (direction > 4) {
        switch (direction) {
        case 5:
            return CURSOR_BOAT_BASE_FRAME_5;
        case 6:
            return CURSOR_BOAT_BASE_FRAME_6;
        case 7:
            return CURSOR_BOAT_BASE_FRAME_7;
        default:
            return 0;
        }
    }
    return direction * CURSOR_FRAMES_PER_DIRECTION;
}

VA(0x0040e21d, 0x256)
void advManager::TurnTo(int direction)
{
    int turnStep = 1;
    int directionDifference = direction - m_cursorDirection;
    if (directionDifference == 0)
        return;
    {
        if ((directionDifference < 0 && directionDifference >= -4) ||
            (directionDifference > 0 && directionDifference > 4))
            turnStep = -1;
        m_cursorTurning = 1;
        int frameIndex = m_cursorDirection * 2;
        int delay = giStepDelay[
            (&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]]];
        if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] == 0)
            delay *= CURSOR_SLOW_TURN_MULTIPLIER;
        if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] == 1)
            delay = static_cast<int>(delay * 1.5);

        do {
            m_cursorCycle = 1;
            if (m_cursorType >= CURSOR_HERO_TYPE_BOAT)
                m_cursorFrame = boatFrameFlip[frameIndex];
            else
                m_cursorFrame = horseFrameFlip[frameIndex];
            m_cursorFrameCount = 0;
            glTimers[1] = delay + KBTickCount();
            if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] != 4) {
                if (ComboDraw(m_mapOriginX, m_mapOriginY, 0))
                    UpdateScreen(0, 0);
                if (bShowIt)
                    DelayTil(&glTimers[1]);
            }
            frameIndex += turnStep;
            if (frameIndex < 0)
                frameIndex = CURSOR_TURN_FRAME_COUNT - 1;
            frameIndex %= CURSOR_TURN_FRAME_COUNT;
        } while (direction * 2 != frameIndex);

        m_cursorDirection = direction;
        StopCursor(1);
        if (bShowIt)
            DelayTil(&glTimers[1]);
        if (ComboDraw(m_mapOriginX, m_mapOriginY, 0))
            UpdateScreen(0, 0);
    }
}

VA(0x0040e473, 0xac)
int advManager::GetMoveShowIt(hero *movingHero, int direction)
{
    int directionX = normalDirTable[direction].x;
    int directionY = normalDirTable[direction].y;
    if (gbThisNetHumanPlayer[giCurPlayer] || !gBlackoutComputer) {
        if (MapExtraPosAndAdjacentsSet(movingHero->m_x, movingHero->m_y,
                                      giCurWatchPlayerBit) ||
            MapExtraPosAndAdjacentsSet(movingHero->m_x + directionX,
                                      movingHero->m_y + directionY,
                                      giCurWatchPlayerBit))
            return 1;
    }
    return 0;
}

VA(0x0040e51f, 0x1234)
mapCell *advManager::MoveHero(int direction, int stopAfterMove,
                             int *eventX, int *eventY,
                             int *outOfMobility, int processEvent,
                             int *adjacentMonster, int forceMove)
{
    EventExtra *mapEvent = 0;
    SAMPLE2 fizzleSample = NULL_SAMPLE2;
    mapCell *eventCell;
    hero *movingHero;
    mapCell *currentCell;
    mapCell *destinationCell;
    mapCell *cursorCell;
    int oldHeroX;
    int oldHeroY;
    int directionX;
    int directionY;
    int terrainCost;
    int nextTerrainCost;
    int pixelsPerStep;
    int stepDelay;
    int halfSteps;
    int step;

    if (gbThisNetHumanPlayer[giCurPlayer])
        SetNoDialogMenus(0);
    gbHitEvent = 0;
    *adjacentMonster = 0;
    *outOfMobility = 0;
    gbHeroMoving = 1;
    eventCell = 0;

    movingHero = &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
    oldHeroX = movingHero->m_x;
    oldHeroY = movingHero->m_y;
    directionX = normalDirTable[direction].x;
    directionY = normalDirTable[direction].y;
    bShowIt = GetMoveShowIt(movingHero, direction);
    if (bShowIt)
        gbMoveShown = 1;

    currentCell = GetCell(movingHero->m_x, movingHero->m_y);
    int currentTerrain = giGroundToTerrain[currentCell->tile];
    destinationCell = GetCell(movingHero->m_x + directionX,
                              movingHero->m_y + directionY);
    terrainCost = CalcTerrainCost(
        currentTerrain, direction & 1,
        movingHero->m_remainingMobility,
        movingHero->m_secondarySkills[HERO_SKILL_PATHFINDING],
        currentCell->objFlag1, destinationCell->objFlag1);
    nextTerrainCost = CalcTerrainCost(
        giGroundToTerrain[destinationCell->tile], 0,
        movingHero->m_remainingMobility - terrainCost,
        movingHero->m_secondarySkills[HERO_SKILL_PATHFINDING],
        destinationCell->objFlag1, 1);

    if (!forceMove && movingHero->m_remainingMobility < terrainCost) {
        *outOfMobility = 1;
        movingHero->m_remainingMobility = 0;
        StopCursor(1);
        goto movementDone;
    }

    SendMapChange(1, movingHero->m_id,
                  static_cast<unsigned char>(movingHero->m_x),
                  static_cast<unsigned char>(movingHero->m_y),
                  giCurPlayer, static_cast<unsigned char>(stopAfterMove),
                  static_cast<unsigned char>(direction));
    MobilizeCurrHero(0);
    *eventX = movingHero->m_x + directionX;
    *eventY = movingHero->m_y + directionY;
    if (m_cursorDirection != direction)
        TurnTo(direction);
    movingHero->m_direction = static_cast<unsigned char>(direction);

    if ((movingHero->m_eventFlags & HERO_EVENT_EMBARKED) &&
        destinationCell->triggerType == MAP_EVENT_COAST) {
        for (step = 0; step < CURSOR_BOAT_COUNT; ++step) {
            if (gpGame->m_boats[step].heroId ==
                static_cast<unsigned char>(movingHero->m_id))
                break;
        }
        boatRecord *boat = &gpGame->m_boats[step];
        currentCell = GetCell(movingHero->m_x, movingHero->m_y);
        boat->savedTriggerType = currentCell->triggerType;
        boat->savedEventData =
            static_cast<unsigned char>(currentCell->w4hi);
        boat->direction = static_cast<signed char>(m_cursorDirection);
        boat->heroId = static_cast<signed char>(
            boat->heroId | MAP_EVENT_ACTION_FLAG);
        currentCell->triggerType = MAP_EVENT_ACTION_FLAG | MAP_EVENT_BOAT;
        currentCell->m_objectData = static_cast<unsigned short>(
            static_cast<unsigned short>(step) << MAP_EVENT_DATA_SHIFT |
            currentCell->m_objectData &
                ((1 << MAP_EVENT_DATA_SHIFT) - 1));
        boat->x = static_cast<signed char>(movingHero->m_x);
        boat->y = static_cast<signed char>(movingHero->m_y);
        StopCursor(1);
        CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
        UpdateScreen(0, 0);
        m_cursorActive = 0;
    }

    if ((destinationCell->triggerType & MAP_EVENT_ACTION_FLAG) &&
        gpAdvManager->ValidMoveWithEvent(movingHero, direction)) {
        int eventType = destinationCell->triggerType & MAP_EVENT_TYPE_MASK;
        switch (eventType) {
        case MAP_EVENT_BOAT:
            if (movingHero->m_eventFlags & HERO_EVENT_EMBARKED)
                goto movementDone;
            StopCursor(1);
            m_cursorActive = 0;
            fizzleSample = LoadPlaySample("killfade.82m");
            gpWindowManager->SaveFizzleSource(
                CURSOR_FIZZLE_X, CURSOR_FIZZLE_Y,
                CURSOR_FIZZLE_WIDTH, CURSOR_FIZZLE_HEIGHT);
            CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
            gpWindowManager->FizzleForward(
                CURSOR_FIZZLE_X, CURSOR_FIZZLE_Y,
                CURSOR_FIZZLE_WIDTH, CURSOR_FIZZLE_HEIGHT,
                gbThisNetHumanPlayer[giCurPlayer]
                    ? CURSOR_INVALID_POSITION
                    : CURSOR_FIZZLE_COMPUTER_TYPE,
                0, 0);
            WaitEndSample(fizzleSample, -1);
            break;

        case MAP_EVENT_HERO_INTERACTION:
            if ((movingHero->m_eventFlags & HERO_EVENT_EMBARKED) &&
                !(gpGame->m_heroRecs[destinationCell->w4hi].m_eventFlags &
                  HERO_EVENT_EMBARKED))
                goto movementDone;
            goto stoppingEvent;

        case MAP_EVENT_CASTLE: {
            if (gpGame->m_castleRecs[destinationCell->w4hi].m_owner !=
                    giCurPlayer &&
                gpGame->m_castleRecs[destinationCell->w4hi].HasGarrison())
                goto stoppingEvent;
            break;
        }

stoppingEvent:
            StopCursor(1);
            CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
            UpdateScreen(0, 0);
            movingHero->m_remainingMobility -= terrainCost;
            eventCell = destinationCell;
            if (movingHero->m_remainingMobility < nextTerrainCost) {
                movingHero->m_remainingMobility = 0;
                stopAfterMove = 1;
            }
            goto movementDone;

        default:
            if (StopOnTrigger(destinationCell)) {
                StopCursor(1);
                CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
                UpdateScreen(0, 0);
                movingHero->m_remainingMobility -= terrainCost;
                eventCell = destinationCell;
                if (movingHero->m_remainingMobility < nextTerrainCost) {
                    movingHero->m_remainingMobility = 0;
                    stopAfterMove = 1;
                }
                goto movementDone;
            }
            break;
        }
    }

    if (ValidMove(direction, 0)) {
        if (movingHero->m_locationType == HERO_TOWN_LOCATION) {
            gpGame->m_castleRecs[movingHero->m_occupiedTown]
                .m_occupyingHeroId = -1;
        }
        if (m_visibilityMapValid) {
            reinterpret_cast<unsigned short *>(m_visibilityMap)[
                (movingHero->m_y + directionY) * MAP_WIDTH +
                movingHero->m_x + directionX] = 0;
        }
        m_updateMinY = 0;
        m_updateMinX = m_updateMinY;
        gpGame->SetVisibility(
            m_mapOriginX + directionX + CURSOR_MAP_DRAW_OFFSET,
            m_mapOriginY + directionY + CURSOR_MAP_DRAW_OFFSET,
            giCurPlayer,
            giVisRange[
                movingHero->m_secondarySkills[HERO_SKILL_SCOUTING]] +
                (movingHero->HasArtifact(CURSOR_VISIBILITY_ARTIFACT) != 0));
        m_forceCompleteDraw = 1;

        pixelsPerStep = giPixelsPerStep[
            (&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]]];
        stepDelay = giStepDelay[
            (&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]]];
        StartCursor(direction);
        if ((&gConfig.computerWalkSpeed)
                [gbThisNetHumanPlayer[giCurPlayer]] == 4) {
            if (EveryOther)
                --m_cursorFrame;
            bMoveSoundMade = 0;
            MoveOrigin(directionX, directionY);
            movingHero->m_x += directionX;
            movingHero->m_y += directionY;
            if (ComboDraw(0))
                UpdateScreen(0, 0);
            EveryOther = 1 - EveryOther;
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
                giDeferObjDrawX = movingHero->m_x + directionX;
                giDeferObjDrawY = movingHero->m_y;
            }

            gbEnlargeScreenBlit = 0;
            gbNoBorder = 1;
            halfSteps = CURSOR_MOVE_HALF_TILE_PIXELS / pixelsPerStep;
            for (step = 0; step < halfSteps * 2; ++step) {
                if (step == halfSteps) {
                    MoveOrigin(directionX, directionY);
                    movingHero->m_x += directionX;
                    movingHero->m_y += directionY;
                    m_updateMinX = startVals[directionX + 1];
                    m_updateMinY = startVals[directionY + 1];
                }
                long tick = KBTickCount();
                if (step + 1 == halfSteps * 2) {
                    m_updateMinX = 0;
                    m_updateMinY = 0;
                } else {
                    m_updateMinX += pixelsPerStep * directionX;
                    m_updateMinY += pixelsPerStep * directionY;
                }
                if (ComboDraw(0)) {
                    giLimitUpdMinX = -1;
                    UpdateScreen(0, 0);
                }
                if (bShowIt)
                    DelayTilMilli(tick + stepDelay);
            }
            giDeferObjDrawX = giDeferObjDrawY = -1;
            gbNoBorder = 0;
            DrawAdventureBorder();
            gbEnlargeScreenBlit = 1;
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

        SetEnvironmentOrigin(m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
                             m_mapOriginY + CURSOR_MAP_DRAW_OFFSET, 0);
        currentCell = GetCell(m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
                              m_mapOriginY + CURSOR_MAP_DRAW_OFFSET);
        if (giGroundToTerrain[currentCell->tile] != m_currentTerrain &&
            (m_currentTerrain = giGroundToTerrain[currentCell->tile],
             !gCdMusic))
            gpSoundManager->SwitchAmbientMusic(
                giTerrainToMusicTrack[m_currentTerrain]);
        m_updateMinY = 0;
        m_updateMinX = m_updateMinY;

        cursorCell = GetCell(m_cursorMapX + m_mapOriginX,
                             m_cursorMapY + m_mapOriginY);
        *eventX = m_cursorMapX + m_mapOriginX;
        *eventY = m_cursorMapY + m_mapOriginY;
        if ((cursorCell->triggerType & MAP_EVENT_ACTION_FLAG) ||
            ((movingHero->m_eventFlags & HERO_EVENT_EMBARKED) &&
             cursorCell->triggerType == MAP_EVENT_COAST)) {
            eventCell = cursorCell;
            switch (cursorCell->triggerType & MAP_EVENT_TYPE_MASK) {
            case MAP_EVENT_NOTHING_SPECIAL:
            case MAP_EVENT_MOSSY_ROCK:
            case MAP_EVENT_REEFS:
            case MAP_EVENT_TREES:
            case MAP_EVENT_MOUNTAINS:
            case MAP_EVENT_VOLCANO:
            case MAP_EVENT_FLOWERS:
            case MAP_EVENT_ROCK:
            case MAP_EVENT_WATER_LAKE:
            case MAP_EVENT_MANDRAKE:
            case MAP_EVENT_DEAD_TREE:
            case MAP_EVENT_STUMP:
            case MAP_EVENT_CRATER:
            case MAP_EVENT_CACTUS:
            case MAP_EVENT_MOUND:
            case MAP_EVENT_DUNE:
            case MAP_EVENT_LAVA_POOL:
            case MAP_EVENT_SHRUB:
                eventCell = 0;
            }
        }
    }

movementDone:
    UpdateRadar(1, 1);
    gbHeroMoving = 0;
    if (!forceMove &&
        (movingHero->m_x != oldHeroX || movingHero->m_y != oldHeroY) &&
        (mapExtra[movingHero->m_y * MAP_WIDTH + movingHero->m_x] &
         MAP_EVENT_ACTION_FLAG) &&
        !(movingHero->m_eventFlags & HERO_EVENT_EMBARKED) &&
        (!eventCell ||
         (eventCell->triggerType & MAP_EVENT_TYPE_MASK) != MAP_EVENT_BOAT)) {
        CheckAdjacentMon(adjacentMonster);
        if (movingHero->m_owner != giCurPlayer)
            eventCell = 0;
    }
    if (gbThisNetHumanPlayer[giCurPlayer])
        SetNoDialogMenus(1);

    if (mapEvent) {
        if (!processEvent) {
            int primaryType = -1;
            int primaryAmount = 0;
            int secondaryType = -1;
            int secondaryAmount = 0;
            for (step = 0; step < 7; ++step) {
                int eventAmount = mapEvent->resources[step];
                if (gpGame->m_players[giCurPlayer].resources[step] <
                    -eventAmount) {
                    eventAmount =
                        -gpGame->m_players[giCurPlayer].resources[step];
                }
                gpGame->m_players[giCurPlayer].resources[step] +=
                    mapEvent->resources[step];
                if (gpGame->m_players[giCurPlayer].resources[step] < 0)
                    gpGame->m_players[giCurPlayer].resources[step] = 0;
                if (eventAmount) {
                    if (primaryType != -1) {
                        secondaryType = primaryType;
                        secondaryAmount = primaryAmount;
                    }
                    primaryType = step;
                    primaryAmount = eventAmount;
                }
            }
            if (mapEvent->artifact != -1 &&
                movingHero->NumArtifacts() < CURSOR_ARTIFACT_CAPACITY) {
                GiveArtifact(movingHero, mapEvent->artifact, 1, -1);
                if (primaryType != -1) {
                    secondaryType = primaryType;
                    secondaryAmount = primaryAmount;
                }
                primaryType = 7;
                primaryAmount = mapEvent->artifact;
            }
            if (mapEvent->cancelAfterVisit)
                mapEvent->active = 0;
            if (primaryType >= 0 && primaryType < 7 && primaryAmount < 0)
                primaryAmount -= CURSOR_RESOURCE_DIALOG_PENALTY;
            if (secondaryType >= 0 && secondaryType < 7 &&
                secondaryAmount < 0)
                secondaryAmount -= CURSOR_RESOURCE_DIALOG_PENALTY;
            NormalDialog(mapEvent->message, 1, -1, -1,
                         primaryType, primaryAmount,
                         secondaryType, secondaryAmount, -1, 0);
            gbHitEvent = 1;
        } else if (mapEvent->applyToComputer) {
            for (step = 0; step < 7; ++step) {
                gpGame->m_players[giCurPlayer].resources[step] +=
                    mapEvent->resources[step];
                if (gpGame->m_players[giCurPlayer].resources[step] < 0)
                    gpGame->m_players[giCurPlayer].resources[step] = 0;
            }
            if (mapEvent->artifact != -1 &&
                movingHero->NumArtifacts() < CURSOR_ARTIFACT_CAPACITY)
                GiveArtifact(movingHero, mapEvent->artifact, 1, -1);
            if (mapEvent->cancelAfterVisit)
                mapEvent->active = 0;
        }
    }
    return eventCell;
}

VA(0x0040f753, 0x174)
void advManager::CheckAdjacentMon(int *) {}

VA(0x0040f8c7, 0x14e)
int advManager::ValidMoveWithEvent(class hero *, int) { return 0; }

VA(0x0040fa15, 0x4f2)
int advManager::ValidMove(int, int) { return 0; }

VA(0x0040ff07, 0x24b)
void advManager::MoveOrigin(int, int) {}

VA(0x00410152, 0x74f)
void advManager::ProcessMapChange(struct SMapChange) {}

VA(0x004108a1, 0x1ba)
void advManager::ProcessIncomingSingleMapChange(struct SMapChange *) {}

VA(0x00410a5b, 0xce)
void advManager::ProcessIncomingGroupMapChange(char *) {}

VA(0x00410b29, 0x75)
void advManager::PurgeMapChangeQueue(void) {}

VA(0x00410b9e, 0x1d4)
void advManager::UnwindMapChangeQueue(int, int) {}

VA(0x00410d72, 0x11a)
void SendMapChange(int, signed char, unsigned char, unsigned char, int, unsigned char, unsigned char) {}

// ---- globals (definitions, RVA order) ----
DATA(0x004ee020) int bMoveSoundMade;
DATA(0x004ee028) int giPixelsPerStep[6];
DATA(0x004ee040) int giStepDelay[5];
DATA(0x004ee054) struct _SAMPLE *hOldWalkSample;
DATA(0x004ee058) struct _SAMPLE *hNewWalkSample;
DATA(0x004ee05c) int EveryOther;
DATA(0x004ee060) int startVals[3];
DATA(0x00524bc0) int S1cursorCycle;
DATA(0x00524bc4) int S1cursorFrameCount;
DATA(0x00524bc8) int S1cursorTurning;
DATA(0x00524bcc) int S1cursorBaseFrame;
DATA(0x00524bd0) int S1cursorDirection;
DATA(0x00524bd8) struct SMapChange *sMapChangeLastFew;
