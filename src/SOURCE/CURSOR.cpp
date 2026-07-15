// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\CURSOR.OBJ   from: (directly linked into exe)
// functions: 18   data: 13
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Icon2b.h>
#include <BASE/Iconf2b.h>
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
#include <_carcass_types.h>
#include <stdio.h>
#include <string.h>

#define CURSOR_SOURCE_FILE \
    const_cast<char *>("I:\\Projects\\Heroes\\Prog\\SOURCE\\CURSOR.CPP")
VA(0x0040d5e0, 0x138)
void advManager::StartCursor(int direction)
{
    int directionX;
    int directionY;
    int cellX;
    int cellY;
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
    cellX = m_cursorMapX + m_mapOriginX;
    cellY = m_cursorMapY + m_mapOriginY;
    cell = m_mapData->GetCell(cellX, cellY);
    cell->m_flags |= CURSOR_MAP_VISIBLE_FLAG;
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
                           m_previousCursorMapY + m_mapOriginY)->m_flags &=
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
                          m_mapOriginY + CURSOR_MAP_DRAW_OFFSET)->m_flags &
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
                int flagDrawn;

                FlipIconToBitmap(m_boatFlagIcons[gpCurPlayer->m_color],
                                 gpWindowManager->m_screen, drawX, drawY,
                                 drawFrame, 0, 0, 0, 0, 0, 0);
                flagDrawn = 1;
            } else {
                if (m_cursorCycle == 0) {
                    drawFrame = m_updateMaxY % CURSOR_DIRECTION_COUNT +
                                ((m_cursorFrame & CURSOR_FRAME_MASK) +
                                 CURSOR_FLAG_FRAME_BASE);
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
                          m_mapOriginY + CURSOR_MAP_DRAW_OFFSET)->m_flags &
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
                int flagDrawn;

                IconToBitmap(m_boatFlagIcons[gpCurPlayer->m_color],
                             gpWindowManager->m_screen, drawX, drawY,
                             drawFrame, 0, 0, 0, 0, 0, 0);
                flagDrawn = 1;
            } else {
                if (m_cursorCycle == 0) {
                    drawFrame = m_updateMaxY % CURSOR_DIRECTION_COUNT +
                                ((m_cursorFrame & CURSOR_FRAME_MASK) +
                                 CURSOR_FLAG_FRAME_BASE);
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
                    hNewWalkSample = gpSoundManager->MemorySample(
                        m_cursorSamples[giGroundToTerrain[
                            GetCell(m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
                                    m_mapOriginY + CURSOR_MAP_DRAW_OFFSET)
                                ->m_terrainImageIndex]]);
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
    int boatFrame;
    int shadowFrame;
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

        int drawX = m_updateMinX + CURSOR_DRAW_X;
        drawY = m_updateMinY + CURSOR_DRAW_Y;
        if (m_cursorType == CURSOR_HERO_TYPE_BOAT)
            drawY -= CURSOR_DRAW_Y - CURSOR_BOAT_DRAW_Y;

        if (m_cursorFrame & CURSOR_FLIP_FLAG) {
            drawX += CURSOR_SHADOW_FLIP_X_ADJUST;
            drawFrame = (m_cursorFrame & CURSOR_FRAME_MASK) +
                        m_cursorFrameCount;
            if (m_drawHeroShadows &&
                m_cursorType == CURSOR_HERO_TYPE_BOAT) {
                boatFrame = drawFrame;
                if (boatFrame >= CURSOR_SHADOW_ANIM_FIRST &&
                    boatFrame < CURSOR_SHADOW_ANIM_END)
                    boatShadowOffset = CURSOR_BOAT_SHADOW_OFFSET;
                else
                    boatShadowOffset = 0;
                IconToBitmap(m_boatShadowIcon, gpWindowManager->m_screen,
                             drawX - CURSOR_SHADOW_FLIP_X_ADJUST, drawY,
                             boatFrame + boatShadowOffset, 1, 0, 0,
                             CURSOR_CLIP_SIZE, CURSOR_CLIP_SIZE, 0);
            } else if (m_drawHeroShadows &&
                       m_cursorType != CURSOR_HERO_TYPE_BOAT) {
                shadowFrame = drawFrame;
                if (shadowFrame == 0x33)
                    shadowFrame = 0x38;
                if (shadowFrame == 0x32)
                    shadowFrame = 0x39;
                if (shadowFrame == 0x31)
                    shadowFrame = 0x3a;
                if (shadowFrame == 0x2f)
                    shadowFrame = 0x37;
                if (shadowFrame == 0x2e)
                    shadowFrame = 0x37;
                if (shadowFrame >= CURSOR_SHADOW_ANIM_FIRST &&
                    shadowFrame < CURSOR_SHADOW_ANIM_END)
                    shadowOffset = CURSOR_HORSE_SHADOW_OFFSET;
                else
                    shadowOffset = 0;
                IconToBitmap(m_shadowIcon, gpWindowManager->m_screen,
                             drawX - CURSOR_SHADOW_FLIP_X_ADJUST, drawY,
                             shadowFrame + shadowOffset, 1, 0, 0,
                             CURSOR_CLIP_SIZE, CURSOR_CLIP_SIZE, 0);
            }
        } else {
            drawFrame = m_cursorFrame + m_cursorFrameCount;
            if (m_drawHeroShadows &&
                m_cursorType == CURSOR_HERO_TYPE_BOAT) {
                IconToBitmap(m_boatShadowIcon, gpWindowManager->m_screen,
                             drawX, drawY,
                             drawFrame,
                             1, 0, 0, CURSOR_CLIP_SIZE,
                             CURSOR_CLIP_SIZE, 0);
            } else if (m_drawHeroShadows &&
                       m_cursorType != CURSOR_HERO_TYPE_BOAT) {
                IconToBitmap(m_shadowIcon, gpWindowManager->m_screen,
                             drawX, drawY,
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
    if (gbThisNetHumanPlayer[giCurPlayer] || !gConfig.blackoutComputer) {
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
    int currentTerrain = giGroundToTerrain[currentCell->m_terrainImageIndex];
    destinationCell = GetCell(movingHero->m_x + directionX,
                              movingHero->m_y + directionY);
    terrainCost = CalcTerrainCost(
        currentTerrain, direction & 1,
        movingHero->m_remainingMobility,
        movingHero->m_secondarySkills[HERO_SKILL_PATHFINDING],
        currentCell->m_isRoad, destinationCell->m_isRoad);
    nextTerrainCost = CalcTerrainCost(
        giGroundToTerrain[destinationCell->m_terrainImageIndex], 0,
        movingHero->m_remainingMobility - terrainCost,
        movingHero->m_secondarySkills[HERO_SKILL_PATHFINDING],
        destinationCell->m_isRoad, 1);

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
        destinationCell->m_triggerType == MAP_EVENT_COAST) {
        for (step = 0; step < CURSOR_BOAT_COUNT; ++step) {
            if (gpGame->m_boats[step].heroId ==
                static_cast<unsigned char>(movingHero->m_id))
                break;
        }
        boatRecord *boat = &gpGame->m_boats[step];
        currentCell = GetCell(movingHero->m_x, movingHero->m_y);
        boat->savedTriggerType = currentCell->m_triggerType;
        boat->savedEventData =
            static_cast<unsigned char>(currentCell->m_objectMetadata);
        boat->direction = static_cast<signed char>(m_cursorDirection);
        boat->heroId = static_cast<signed char>(
            boat->heroId | MAP_EVENT_ACTION_FLAG);
        currentCell->m_triggerType = MAP_EVENT_ACTION_FLAG | MAP_EVENT_BOAT;
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

    if ((destinationCell->m_triggerType & MAP_EVENT_ACTION_FLAG) &&
        gpAdvManager->ValidMoveWithEvent(movingHero, direction)) {
        int eventType = destinationCell->m_triggerType & MAP_EVENT_TYPE_MASK;
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
                !(gpGame->m_heroRecs[destinationCell->m_objectMetadata].m_eventFlags &
                  HERO_EVENT_EMBARKED))
                goto movementDone;
            goto stoppingEvent;

        case MAP_EVENT_CASTLE: {
            if (gpGame->m_castleRecs[destinationCell->m_objectMetadata].m_owner !=
                    giCurPlayer &&
                gpGame->m_castleRecs[destinationCell->m_objectMetadata].HasGarrison())
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
            m_visibilityMap[(movingHero->m_y + directionY) * MAP_WIDTH +
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
        if (giGroundToTerrain[currentCell->m_terrainImageIndex] != m_currentTerrain &&
            (m_currentTerrain = giGroundToTerrain[currentCell->m_terrainImageIndex],
             gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI))
            gpSoundManager->SwitchAmbientMusic(
                giTerrainToMusicTrack[m_currentTerrain]);
        m_updateMinY = 0;
        m_updateMinX = m_updateMinY;

        cursorCell = GetCell(m_cursorMapX + m_mapOriginX,
                             m_cursorMapY + m_mapOriginY);
        *eventX = m_cursorMapX + m_mapOriginX;
        *eventY = m_cursorMapY + m_mapOriginY;
        if ((cursorCell->m_triggerType & MAP_EVENT_ACTION_FLAG) ||
            ((movingHero->m_eventFlags & HERO_EVENT_EMBARKED) &&
             cursorCell->m_triggerType == MAP_EVENT_COAST)) {
            eventCell = cursorCell;
            switch (cursorCell->m_triggerType & MAP_EVENT_TYPE_MASK) {
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
         (eventCell->m_triggerType & MAP_EVENT_TYPE_MASK) != MAP_EVENT_BOAT)) {
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
                if (gpGame->m_players[giCurPlayer].m_resources[step] <
                    -eventAmount) {
                    eventAmount =
                        -gpGame->m_players[giCurPlayer].m_resources[step];
                }
                gpGame->m_players[giCurPlayer].m_resources[step] +=
                    mapEvent->resources[step];
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
                gpGame->m_players[giCurPlayer].m_resources[step] +=
                    mapEvent->resources[step];
                if (gpGame->m_players[giCurPlayer].m_resources[step] < 0)
                    gpGame->m_players[giCurPlayer].m_resources[step] = 0;
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
void advManager::CheckAdjacentMon(int *adjacentMonster)
{
    hero *currentHero;
    int killed;
    int monsterX;
    int monsterY;
    mapCell *monsterCell;
    mapCell *heroCell;

    currentHero = &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
    killed = 0;
    if (FindAdjacentMonster(currentHero->m_x, currentHero->m_y,
                            &monsterX, &monsterY, -1, -1)) {
        StopCursor(1);
        CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
        UpdateScreen(0, 0);
        monsterCell = GetCell(monsterX, monsterY);
        heroCell = GetCell(currentHero->m_x, currentHero->m_y);
        if (gbThisNetHumanPlayer[giCurPlayer])
            PlayerMonsterInteract(monsterCell, heroCell, currentHero, &killed,
                                  currentHero->m_x, currentHero->m_y, 1,
                                  monsterX, monsterY);
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

VA(0x0040f8c7, 0x14e)
int advManager::ValidMoveWithEvent(hero *movingHero, int direction)
{
    int directionX0;
    int destinationX0;
    int directionY0;
    int destinationY0;
    mapCell *destinationCell0;

    directionX0 = normalDirTable[direction].x;
    directionY0 = normalDirTable[direction].y;
    destinationX0 = movingHero->m_x + directionX0;
    destinationY0 = movingHero->m_y + directionY0;
    if (destinationX0 < 0 || destinationX0 > MAP_WIDTH - 1 ||
        destinationY0 < 0 || destinationY0 > MAP_HEIGHT - 1)
        return 0;

    destinationCell0 = m_mapData->GetCell(destinationX0, destinationY0);
    switch (destinationCell0->m_triggerType & MAP_EVENT_TYPE_MASK) {
    case MAP_EVENT_HERO_INTERACTION:
        if (!(movingHero->m_eventFlags & HERO_EVENT_EMBARKED))
            return 1;
        if (!(gpGame->m_heroRecs[destinationCell0->m_objectMetadata].m_eventFlags &
              HERO_EVENT_EMBARKED))
            return 0;
        return 1;
    default:
        return ValidMove(direction, 1);
    }
}

VA(0x0040fa15, 0x4f2)
int advManager::ValidMove(int direction, int eventMode)
{
    int directionX;
    int directionY;
    int destinationX;
    int destinationY;
    int centerX;
    int centerY;
    mapCell *destinationCell;
    mapCell *currentCell;
    mapCell *horizontalCell;
    mapCell *verticalCell;
    mapCell *northNeighborCell;
    mapCell *southNeighborCell;
    int northDirection;
    int southDirection;

    directionX = normalDirTable[direction].x;
    directionY = normalDirTable[direction].y;
    destinationX = m_mapOriginX + directionX;
    destinationY = m_mapOriginY + directionY;
    centerX = m_mapOriginX + CURSOR_MAP_DRAW_OFFSET;
    centerY = m_mapOriginY + CURSOR_MAP_DRAW_OFFSET;
    destinationX = destinationX + CURSOR_MAP_DRAW_OFFSET;
    destinationY = destinationY + CURSOR_MAP_DRAW_OFFSET;

    if (destinationX < 0)
        return 0;
    if (destinationX > MAP_WIDTH - 1)
        return 0;
    if (destinationY < 0)
        return 0;
    if (destinationY > MAP_HEIGHT - 1)
        return 0;

    destinationCell = m_mapData->GetCell(destinationX, destinationY);
    currentCell = m_mapData->GetCell(centerX, centerY);
    if (destinationCell->m_flags & CURSOR_CELL_BLOCKED_FLAG)
        return 0;

    if (giGroundToTerrain[destinationCell->m_terrainImageIndex] == CURSOR_WATER_TERRAIN) {
        if (m_cursorType != CURSOR_HERO_TYPE_BOAT &&
            destinationCell->m_triggerType !=
                (MAP_EVENT_ACTION_FLAG | MAP_EVENT_BOAT) &&
            destinationCell->m_triggerType !=
                (MAP_EVENT_ACTION_FLAG | MAP_EVENT_SHIPWRECK))
            return 0;
        if (giGroundToTerrain[currentCell->m_terrainImageIndex] == CURSOR_WATER_TERRAIN &&
            directionX != 0 && directionY != 0) {
            if (giGroundToTerrain[
                    m_mapData->GetCell(centerX + directionX, centerY)->m_terrainImageIndex] !=
                CURSOR_WATER_TERRAIN)
                return 0;
            if (giGroundToTerrain[
                    m_mapData->GetCell(centerX, centerY + directionY)->m_terrainImageIndex] !=
                CURSOR_WATER_TERRAIN)
                return 0;
        }
    } else if (m_cursorType == CURSOR_HERO_TYPE_BOAT &&
               destinationCell->m_triggerType != MAP_EVENT_COAST) {
        return 0;
    }

    northDirection = (1 << direction) & CURSOR_NORTH_DIRECTION_MASK;
    southDirection = (1 << direction) & CURSOR_SOUTH_DIRECTION_MASK;
    if (northDirection) {
        if (currentCell->m_objectIndex != CURSOR_EMPTY_OBJECT_INDEX &&
            currentCell->m_objectTileset != CURSOR_PASSABLE_OBJECT_TILESET &&
            !(currentCell->m_flags & CURSOR_OBJECT_PASSABLE_FLAG) &&
            currentCell->m_triggerType !=
                (MAP_EVENT_ACTION_FLAG | MAP_EVENT_WHIRLPOOL))
            return 0;
        if (destinationCell->m_overlayIndex != CURSOR_EMPTY_OBJECT_INDEX) {
            northNeighborCell = m_mapData->GetCell(destinationX,
                                                   destinationY + 1);
            if (northNeighborCell->m_objectIndex != CURSOR_EMPTY_OBJECT_INDEX &&
                northNeighborCell->m_objectTileset != CURSOR_PASSABLE_OBJECT_TILESET &&
                !(northNeighborCell->m_flags & CURSOR_OBJECT_PASSABLE_FLAG))
                return 0;
        }
    } else if (southDirection) {
        if (destinationCell->m_objectIndex != CURSOR_EMPTY_OBJECT_INDEX &&
            destinationCell->m_objectTileset != CURSOR_PASSABLE_OBJECT_TILESET &&
            !(destinationCell->m_flags & CURSOR_OBJECT_PASSABLE_FLAG) &&
            destinationCell->m_triggerType !=
                (MAP_EVENT_ACTION_FLAG | MAP_EVENT_WHIRLPOOL) &&
            (!eventMode || !(destinationCell->m_triggerType & MAP_EVENT_ACTION_FLAG) ||
             !StopOnTrigger(destinationCell)))
            return 0;
        if (currentCell->m_overlayIndex != CURSOR_EMPTY_OBJECT_INDEX) {
            southNeighborCell = m_mapData->GetCell(
                m_cursorMapX + m_mapOriginX,
                m_cursorMapY + m_mapOriginY + 1);
            if (southNeighborCell->m_objectIndex != CURSOR_EMPTY_OBJECT_INDEX &&
                southNeighborCell->m_objectTileset != CURSOR_PASSABLE_OBJECT_TILESET &&
                !(southNeighborCell->m_flags & CURSOR_OBJECT_PASSABLE_FLAG) &&
                !(southNeighborCell->m_triggerType & MAP_EVENT_ACTION_FLAG))
                return 0;
        }
    }
    return 1;
}

VA(0x0040ff07, 0x24b)
void advManager::MoveOrigin(int directionX, int directionY)
{
    int oldOriginX0;
    int cellY1;
    int cellX5;
    int oldOriginY9;
    mapCell *newCursorCell5;
    mapCell *oldCursorCell0;
    mapCell *newPreviousCell4;
    mapCell *oldPreviousCell2;

    oldOriginX0 = m_mapOriginX;
    oldOriginY9 = m_mapOriginY;
    m_mapOriginX += directionX;
    m_mapOriginY += directionY;
    directionX = oldOriginX0 - m_mapOriginX;
    directionY = oldOriginY9 - m_mapOriginY;
    if (directionX != 0 || directionY != 0) {
        oldCursorCell0 = m_mapData->GetCell(m_cursorMapX + oldOriginX0,
                                            m_cursorMapY + oldOriginY9);
        oldCursorCell0->m_flags &= ~CURSOR_MAP_VISIBLE_FLAG;
        m_cursorMapX += directionX;
        m_cursorMapY += directionY;
        cellX5 = m_cursorMapX + m_mapOriginX;
        cellY1 = m_cursorMapY + m_mapOriginY;
        newCursorCell5 = m_mapData->GetCell(cellX5, cellY1);
        newCursorCell5->m_flags |= CURSOR_MAP_VISIBLE_FLAG;
        if (m_previousCursorMapX != CURSOR_INVALID_POSITION) {
            oldPreviousCell2 = m_mapData->GetCell(
                m_previousCursorMapX + oldOriginX0,
                m_previousCursorMapY + oldOriginY9);
            oldPreviousCell2->m_flags &= ~CURSOR_MAP_VISIBLE_FLAG;
            m_previousCursorMapX += directionX;
            m_previousCursorMapY += directionY;
            cellX5 = m_previousCursorMapX + m_mapOriginX;
            cellY1 = m_previousCursorMapY + m_mapOriginY;
            newPreviousCell4 = m_mapData->GetCell(cellX5, cellY1);
            newPreviousCell4->m_flags |= CURSOR_MAP_VISIBLE_FLAG;
        }
    }
    m_forceCompleteDraw = 1;
}

VA(0x00410152, 0x74f)
void advManager::ProcessMapChange(SMapChange change)
{
    hero *mapHero;
    mapCell *eventCell;
    int eventX;
    int eventY;
    int outOfMobility;
    int adjacentMonster;
    mapCell *recruitCell;
    mapCell *heroCell;

    giMapChangeCtr = change.sequence + 1;
    if (change.player != giCurPlayer) {
        gpAdvManager->DeactivateCurrTown();
        gpAdvManager->DeactivateCurrHero();
    }
    if (change.player >= 0 && change.player < CURSOR_PLAYER_COUNT) {
        giCurPlayer = change.player;
        gpCurPlayer = reinterpret_cast<playerData *>(
            &gpGame->m_players[giCurPlayer]);
        giCurPlayerBit = 1 << giCurPlayer;
    }

    switch (change.type) {
    case MAP_CHANGE_MOVE_HERO:
        LogInt("MC Move Hero", change.id, change.x, change.y,
               change.direction, change.sequence,
               gpGame->m_heroRecs[change.id].m_x,
               gpGame->m_heroRecs[change.id].m_y);
        mapHero = &gpGame->m_heroRecs[change.id];
        if (change.x != mapHero->m_x || change.y != mapHero->m_y) {
            sprintf(gText,
                    "Data miscommunication in hero position, first %d, %d, second %d, %d.  Please give Phil a copy of  your Autosave and, if possible, instructions to recreate this error",
                    mapHero->m_x, mapHero->m_y, change.x, change.y);
            LogStr(gText);
            break;
        }
        gpAdvManager->SetHeroContext(change.id, 0);
        eventCell = MoveHero(change.direction, change.stopAfterMove,
                             &eventX, &eventY, &outOfMobility, 1,
                             &adjacentMonster, 1);
        if (eventCell != 0) {
            switch (eventCell->m_triggerType & MAP_EVENT_TYPE_MASK) {
            case MAP_EVENT_COAST:
            case MAP_EVENT_BOAT:
                DoAIEvent(eventCell, mapHero, eventX, eventY);
                break;
            }
        }
        break;

    case MAP_CHANGE_MY_TURN:
        LogInt("MC My Turn", change.x, change.y,
               CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
               CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED);
        break;

    case MAP_CHANGE_TELEPORT_HERO:
        LogInt("MC Teleport Hero", change.x, change.y,
               CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
               CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED);
        mapHero = &gpGame->m_heroRecs[change.id];
        TeleportTo(mapHero, change.x, change.y, 0, 1);
        break;

    case MAP_CHANGE_CLAIM_MINE:
        LogInt("MC ClaimMine", CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
               CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
               CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED);
        gpGame->ClaimMine(change.id, change.player);
        CompleteDraw(0);
        UpdateScreen(0, 0);
        break;

    case MAP_CHANGE_CLAIM_TOWN:
        LogInt("MC ClaimTown", CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
               CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
               CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED);
        gpGame->ClaimTown(change.id, change.player, 1);
        CompleteDraw(0);
        UpdateScreen(0, 0);
        break;

    case MAP_CHANGE_BUILD_BOAT:
        LogInt("MC BuildBoat", CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
               CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
               CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED);
        gpGame->CreateBoat(change.x, change.y, 1);
        CompleteDraw(0);
        UpdateScreen(0, 0);
        break;

    case MAP_CHANGE_ERASE_OBJECT:
        LogInt("MC Erase Object", change.x, change.y,
               CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
               CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED);
        eventCell = GetCell(change.x, change.y);
        EraseObj(eventCell, change.x, change.y);
        CompleteDraw(0);
        UpdateScreen(0, 0);
        break;

    case MAP_CHANGE_DEAD_HERO:
        LogStr("MC DeadHero");
        mapHero = &gpGame->m_heroRecs[change.id];
        if (change.x != mapHero->m_x || change.y != mapHero->m_y)
            break;
        mapHero->Deallocate(1);
        CompleteDraw(0);
        UpdateScreen(0, 0);
        break;

    case MAP_CHANGE_RECRUIT_HERO:
        LogStr("MC RecruitHero");
        mapHero = &gpGame->m_heroRecs[change.id];
        mapHero->m_x = change.x;
        mapHero->m_y = change.y;
        mapHero->m_eventFlags = 0;
        mapHero->m_direction = CURSOR_RECRUIT_HERO_DIRECTION;
        mapHero->m_locationType =
            gpGame->m_worldMap.GetCell(change.x, change.y)->m_triggerType;
        mapHero->m_occupiedTown =
            gpGame->m_worldMap.GetCell(change.x, change.y)->m_objectMetadata;
        mapHero->m_owner = change.player;
        recruitCell = gpGame->m_worldMap.GetCell(change.x, change.y);
        recruitCell->m_triggerType =
            MAP_EVENT_ACTION_FLAG | MAP_EVENT_HERO_INTERACTION;
        heroCell = gpGame->m_worldMap.GetCell(change.x, change.y);
        heroCell->m_objectData = static_cast<unsigned short>(
            static_cast<short>(change.id) << MAP_EVENT_DATA_SHIFT |
            heroCell->m_objectData &
                ((1 << MAP_EVENT_DATA_SHIFT) - 1));
        gpAdvManager->SetHeroContext(change.id, 0);
        CompleteDraw(0);
        UpdateScreen(0, 0);
        break;

    case MAP_CHANGE_DEAD_PLAYER:
        LogStr("Dead Player");
        sprintf(gText, "%s has been vanquished!", cPlayerNames[change.id]);
        NormalDialog(gText, NORMAL_DIALOG_INFO,
                     NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_RESOURCE,
                     NORMAL_DIALOG_CREST,
                     gpGame->m_players[change.id].m_color,
                     NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_RESOURCE,
                     NORMAL_DIALOG_NO_RESOURCE,
                     CURSOR_DEAD_PLAYER_DIALOG_TIME);
        break;

    case MAP_CHANGE_UNUSED:
    default:
        break;
    }
}

VA(0x004108a1, 0x1ba)
void advManager::ProcessIncomingSingleMapChange(SMapChange *incoming)
{
    int slot;

    if (incoming->sequence < giMapChangeCtr)
        goto done;

    if (incoming->sequence == giMapChangeCtr) {
        ProcessMapChange(*incoming);
        goto unwindQueue;
    } else {
retryInsert:
        for (slot = 0; slot < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++slot) {
            if (sMapChangeQueue[slot].type != 0 &&
                sMapChangeQueue[slot].sequence == incoming->sequence) {
                LogInt("OQ", incoming->sequence, giMapChangeCtr,
                       CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
                       CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
                       CURSOR_LOG_UNUSED);
                goto duplicateChange;
            }
        }

        for (slot = 0; slot < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++slot) {
            if (sMapChangeQueue[slot].type == 0) {
                LogInt("SQ", incoming->sequence, giMapChangeCtr,
                       CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
                       CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED,
                       CURSOR_LOG_UNUSED);
                sMapChangeQueue[slot] = *incoming;
                goto done;
            }
        }

        UnwindMapChangeQueue(1, 1);
        goto retryInsert;
    }

duplicateChange:
    ++slot;
unwindQueue:
    UnwindMapChangeQueue(0, 1);
done:
    return;
}

VA(0x00410a5b, 0xce)
void advManager::ProcessIncomingGroupMapChange(char *incomingData)
{
    SMapChange *ptr;
    int size;
    SMapChange *buf;
    int i;
    int processed;

    size = sizeof(sMapChangeLastFew);
    buf = static_cast<SMapChange *>(
        BaseAlloc(size, CURSOR_SOURCE_FILE,
                  CURSOR_GROUP_ALLOC_LINE));
    memcpy(buf, incomingData, size);
    for (i = CURSOR_MAP_CHANGE_RECENT_COUNT - 1; i >= 0; --i) {
        ptr = &buf[i];
        if (ptr->type != 0 && ptr->sequence >= giMapChangeCtr) {
            ProcessIncomingSingleMapChange(ptr);
        } else {
            processed = 0;
        }
    }
    BaseFree(buf, CURSOR_SOURCE_FILE, CURSOR_GROUP_FREE_LINE);
}

VA(0x00410b29, 0x75)
void advManager::PurgeMapChangeQueue(void)
{
    int slot;

    for (slot = 0; slot < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++slot)
        sMapChangeQueue[slot].type = 0;
    for (slot = 0; slot < CURSOR_MAP_CHANGE_RECENT_COUNT; ++slot)
        sMapChangeLastFew[slot].type = 0;
}

VA(0x00410b9e, 0x1d4)
void advManager::UnwindMapChangeQueue(int maximumToUnwind, int processChanges)
{
    int queuedChanges;
    int unwoundChanges;
    int lowestSlot;
    int lowestSequence;
    int continueUnwinding;
    int slot;

    queuedChanges = CURSOR_MAP_CHANGE_PENDING_SENTINEL;
    unwoundChanges = 0;
    while (queuedChanges > 0 && unwoundChanges < maximumToUnwind) {
        lowestSlot = -1;
        lowestSequence = CURSOR_MAP_CHANGE_SEQUENCE_SENTINEL;
        queuedChanges = 0;
        for (slot = 0; slot < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++slot) {
            if (sMapChangeQueue[slot].type != 0) {
                ++queuedChanges;
                if (sMapChangeQueue[slot].sequence < lowestSequence) {
                    lowestSequence = sMapChangeQueue[slot].sequence;
                    lowestSlot = slot;
                }
            }
        }
        if (lowestSlot != -1) {
            --queuedChanges;
            if (processChanges)
                ProcessMapChange(sMapChangeQueue[lowestSlot]);
            sMapChangeQueue[lowestSlot].type = 0;
            ++unwoundChanges;
        }
    }

    continueUnwinding = 1;
    while (continueUnwinding) {
        continueUnwinding = 0;
        for (slot = 0; slot < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++slot) {
            if (sMapChangeQueue[slot].type != 0 &&
                sMapChangeQueue[slot].sequence == giMapChangeCtr) {
                if (processChanges)
                    ProcessMapChange(sMapChangeQueue[slot]);
                sMapChangeQueue[slot].type = 0;
                continueUnwinding = 1;
            }
        }
    }
}

VA(0x00410d72, 0x11a)
void SendMapChange(int type, signed char id, unsigned char x, unsigned char y,
                   int player, unsigned char stopAfterMove,
                   unsigned char direction)
{
    SMapChange change;

    if (gbThisNetGotAdventureControl) {
        if (gbRemoteOn) {
            if (player == CURSOR_LOG_UNUSED)
                player = giCurPlayer;
            LogInt("Send Map Change", type, id, x, y,
                   CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED, CURSOR_LOG_UNUSED);
            memset(&change, 0, sizeof(change));
            change.type = static_cast<unsigned char>(type);
            change.id = id;
            change.x = x;
            change.y = y;
            change.player = static_cast<signed char>(player);
            change.stopAfterMove = static_cast<signed char>(stopAfterMove);
            change.direction = static_cast<signed char>(direction);
            change.sequence = giMapChangeCtr;
            ++giMapChangeCtr;
            memmove(&sMapChangeLastFew[1], &sMapChangeLastFew[0],
                    sizeof(SMapChange) *
                        (CURSOR_MAP_CHANGE_RECENT_COUNT - 1));
            sMapChangeLastFew[0] = change;
            TransmitRemoteData(
                reinterpret_cast<char *>(sMapChangeLastFew),
                CURSOR_REMOTE_PLAYER_ALL, sizeof(sMapChangeLastFew),
                CURSOR_REMOTE_PACKET_TYPE, 0, CURSOR_REMOTE_RELIABLE,
                CURSOR_REMOTE_NO_TIMEOUT);
        }
    }
}

// ---- globals (definitions, RVA order) ----
DATA(0x004ee020) int bMoveSoundMade = 1;
DATA(0x004ee028) int giPixelsPerStep[6];
DATA(0x004ee040) int giStepDelay[5];
DATA(0x004ee054) struct _SAMPLE *hOldWalkSample = 0;
DATA(0x004ee058) struct _SAMPLE *hNewWalkSample = 0;
DATA(0x004ee05c) int EveryOther = 0;
DATA(0x004ee060) int startVals[3];
DATA(0x00524bc0) int S1cursorCycle;
DATA(0x00524bc4) int S1cursorFrameCount;
DATA(0x00524bc8) int S1cursorTurning;
DATA(0x00524bcc) int S1cursorBaseFrame;
DATA(0x00524bd0) int S1cursorDirection;
DATA(0x00524bd8) SMapChange sMapChangeLastFew[CURSOR_MAP_CHANGE_RECENT_COUNT];
