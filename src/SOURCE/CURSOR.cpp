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
#include <BASE/message.h>
#include <stdio.h>
#include <string.h>

#define RETAIL_FILE const_cast<char*>("I:\\Projects\\Heroes\\Prog\\SOURCE\\CURSOR.CPP")

// @early-stop
// All 88 normalized instructions, the 0x18 frame/slots, CFG, and six ordered
// relocation sites/effective targets align. The only unmasked bytes are
// +0xd9/+0xda/+0xe2/+0xe3: the cellY sum loads cursor Y before origin Y, while
// retail loads the commutative terms in reverse order. Five bounded candidates
// across direct reversal, both OD_STEER(term) escapes, and both OR-zero forms were
// byte-neutral. Revisit after an earlier CURSOR source edit or relevant
// advManager layout/header change perturbs TU-cumulative operand evaluation.
VA(0x0040d5e0, 0x138)
void advManager::StartCursor(i32 direction) {
    i32 directionX_a;
    i32 directionY_a1;
    i32 cellX;
    i32 cellY;

    m_cursorDirection = direction;
    m_cursorFrame = GetCursorBaseFrame(direction) + 1;
    if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] > 0)
        m_cursorCycle = 1;
    else
        m_cursorCycle = 2;

    directionX_a = normalDirTable[direction].x;
    directionY_a1 = normalDirTable[direction].y;
    m_previousCursorMapX = m_cursorMapX;
    m_previousCursorMapY = m_cursorMapY;
    m_cursorMapX += directionX_a;
    m_cursorMapY += directionY_a1;
    cellX = m_cursorMapX + m_mapOriginX;
    cellY = m_cursorMapY + m_mapOriginY;
    m_mapData->GetCell(cellX, cellY)->m_flags |= CURSOR_MAP_VISIBLE_FLAG;
}

// @early-stop
// All 72 normalized instructions, the 0x8 frame/slots, CFG, and five ordered
// relocation sites/effective targets align. The only unmasked bytes are
// +0xaa/+0xab/+0xb2/+0xb3: the previous-cursor cell offset adds origin X before
// cursor X, while retail loads the commutative terms in reverse order. Nine
// bounded candidates across direct term reversal, both OD_STEER(term) escapes, both
// OR-zero forms, zero grouping, and three depth-one commutative_order AST edits
// were byte-neutral. Revisit after an earlier CURSOR source edit or relevant
// advManager layout/header change perturbs TU-cumulative operand evaluation.
VA(0x0040d718, 0x11c)
void advManager::StopCursor(i32 stopSound) {
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
        m_mapData->GetCell(m_previousCursorMapX + m_mapOriginX, m_previousCursorMapY + m_mapOriginY)
            ->m_flags &= ~CURSOR_MAP_VISIBLE_FLAG;
        m_previousCursorMapY = -1;
        m_previousCursorMapX = m_previousCursorMapY;
    }
    m_cursorTurning = 0;
}

VA(0x0040d834, 0x5f8)
void advManager::DrawCursor(void) {
    i32 drawY;
    i32 drawFrame_f;

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
    if (m_cursorType == CURSOR_HERO_TYPE_BOAT)
        drawY -= CURSOR_DRAW_Y - CURSOR_BOAT_DRAW_Y;

    if (m_cursorFrame & CURSOR_FLIP_FLAG) {
        drawX += CURSOR_FLIPPED_DRAW_X - CURSOR_DRAW_X;
        drawFrame_f = (m_cursorFrame & CURSOR_FRAME_MASK) + m_cursorFrameCount;
        if (m_cursorType == CURSOR_HERO_TYPE_BOAT
            && !(
                GetCell(
                    m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
                    m_mapOriginY + CURSOR_MAP_DRAW_OFFSET
                )
                    ->m_flags
                & CURSOR_CELL_UNCOVERED_FLAG
            )) {
            FlipIconToBitmap(
                m_heroIcons[CURSOR_BOAT_WAKE_TYPE],
                gpWindowManager->m_screen,
                drawX,
                drawY,
                drawFrame_f,
                1,
                0,
                0,
                CURSOR_CLIP_SIZE,
                CURSOR_CLIP_SIZE,
                0
            );
        }
        FlipIconToBitmap(
            m_heroIcons[m_cursorType],
            gpWindowManager->m_screen,
            drawX,
            drawY,
            drawFrame_f,
            1,
            0,
            0,
            CURSOR_CLIP_SIZE,
            CURSOR_CLIP_SIZE,
            0
        );
        if (m_cursorType == CURSOR_HERO_TYPE_BOAT) {
            i32 flagDrawn;

            FlipIconToBitmap(
                m_boatFlagIcons[gpCurPlayer->m_color],
                gpWindowManager->m_screen,
                drawX,
                drawY,
                drawFrame_f,
                0,
                0,
                0,
                0,
                0,
                0
            );
            flagDrawn = 1;
        } else {
            if (m_cursorCycle == 0) {
                drawFrame_f =
                    m_updateMaxY % CURSOR_DIRECTION_COUNT
                    + ((OD_STEER(m_cursorFrame) & CURSOR_FRAME_MASK) + CURSOR_FLAG_FRAME_BASE);
            }
            FlipIconToBitmap(
                m_flagIcons[gpCurPlayer->m_color],
                gpWindowManager->m_screen,
                drawX,
                drawY,
                drawFrame_f,
                1,
                0,
                0,
                CURSOR_CLIP_SIZE,
                CURSOR_CLIP_SIZE,
                0
            );
            ++m_updatePending;
        }
    } else {
        drawFrame_f = m_cursorFrame + m_cursorFrameCount;
        if (m_cursorType == CURSOR_HERO_TYPE_BOAT
            && !(
                GetCell(
                    m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
                    m_mapOriginY + CURSOR_MAP_DRAW_OFFSET
                )
                    ->m_flags
                & CURSOR_CELL_UNCOVERED_FLAG
            )) {
            IconToBitmap(
                m_heroIcons[CURSOR_BOAT_WAKE_TYPE],
                gpWindowManager->m_screen,
                drawX,
                drawY,
                drawFrame_f,
                1,
                0,
                0,
                CURSOR_CLIP_SIZE,
                CURSOR_CLIP_SIZE,
                0
            );
        }
        IconToBitmap(
            m_heroIcons[m_cursorType],
            gpWindowManager->m_screen,
            drawX,
            drawY,
            drawFrame_f,
            1,
            0,
            0,
            CURSOR_CLIP_SIZE,
            CURSOR_CLIP_SIZE,
            0
        );
        if (m_cursorType == CURSOR_HERO_TYPE_BOAT) {
            i32 flagDrawn;

            IconToBitmap(
                m_boatFlagIcons[gpCurPlayer->m_color],
                gpWindowManager->m_screen,
                drawX,
                drawY,
                drawFrame_f,
                0,
                0,
                0,
                0,
                0,
                0
            );
            flagDrawn = 1;
        } else {
            if (m_cursorCycle == 0) {
                drawFrame_f =
                    m_updateMaxY % CURSOR_DIRECTION_COUNT
                    + ((OD_STEER(m_cursorFrame) & CURSOR_FRAME_MASK) + CURSOR_FLAG_FRAME_BASE);
            }
            IconToBitmap(
                m_flagIcons[gpCurPlayer->m_color],
                gpWindowManager->m_screen,
                drawX,
                drawY,
                drawFrame_f,
                1,
                0,
                0,
                CURSOR_CLIP_SIZE,
                CURSOR_CLIP_SIZE,
                0
            );
            ++m_updatePending;
        }
    }

    if (m_cursorCycle && (&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] != 4) {
        ++m_cursorFrameCount;
        if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] == 3
            && (m_cursorFrameCount == 4 || m_cursorFrameCount == 1))
            ++m_cursorFrameCount;
        if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] == 0
            && (EveryOther = 1 - EveryOther) != 0)
            --m_cursorFrameCount;
    }
    if (m_cursorFrameCount >= CURSOR_LAST_FRAME_COUNT)
        m_cursorFrameCount = 0;

    if (!m_cursorTurning) {
        if (m_cursorFrameCount == 0)
            hOldWalkSample = hNewWalkSample;
        if (m_cursorFrameCount == 3
            || ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] == 4
                && !bMoveSoundMade)) {
            bMoveSoundMade = 1;
            if (EveryOther == 0) {
                hNewWalkSample = gpSoundManager->MemorySample(
                    m_cursorSamples[giGroundToTerrain[GetCell(
                                                          m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
                                                          m_mapOriginY + CURSOR_MAP_DRAW_OFFSET
                    )
                                                          ->m_terrainImageIndex]]
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

VA(0x0040de2c, 0x36c)
void advManager::DrawCursorShadow(void) {
    i32 shadowOffset;
    i32 boatShadowOffset;
    i32 drawFrame_f;
    i32 boatFrame_i;
    CursorHeroShadowFrame shadowFrame;
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
    if (m_cursorType == CURSOR_HERO_TYPE_BOAT)
        drawY -= CURSOR_DRAW_Y - CURSOR_BOAT_DRAW_Y;

    if (m_cursorFrame & CURSOR_FLIP_FLAG) {
        drawX += CURSOR_SHADOW_FLIP_X_ADJUST;
        drawFrame_f = (m_cursorFrame & CURSOR_FRAME_MASK) + m_cursorFrameCount;
        if (m_drawHeroShadows && m_cursorType == CURSOR_HERO_TYPE_BOAT) {
            boatFrame_i = drawFrame_f;
            if (boatFrame_i >= CURSOR_SHADOW_ANIM_FIRST && boatFrame_i < CURSOR_SHADOW_ANIM_END)
                boatShadowOffset = CURSOR_BOAT_SHADOW_OFFSET;
            else
                boatShadowOffset = 0;
            IconToBitmap(
                m_boatShadowIcon,
                gpWindowManager->m_screen,
                drawX - CURSOR_SHADOW_FLIP_X_ADJUST,
                drawY,
                boatFrame_i + boatShadowOffset,
                1,
                0,
                0,
                CURSOR_CLIP_SIZE,
                CURSOR_CLIP_SIZE,
                0
            );
        } else if (m_drawHeroShadows && m_cursorType != CURSOR_HERO_TYPE_BOAT) {
            shadowFrame = drawFrame_f;
            if (shadowFrame == HERO_SPRITE_UP_STEP_5)
                shadowFrame = HERO_SPRITE_UP_SHADOW_STEP_5;
            if (shadowFrame == HERO_SPRITE_UP_STEP_4)
                shadowFrame = HERO_SPRITE_UP_SHADOW_STEP_4;
            if (shadowFrame == HERO_SPRITE_UP_STEP_3)
                shadowFrame = HERO_SPRITE_UP_SHADOW_STEP_3;
            if (shadowFrame == HERO_SPRITE_UP_STEP_2)
                shadowFrame = HERO_SPRITE_UP_SHADOW_WIDE;
            if (shadowFrame == HERO_SPRITE_UP_STEP_1)
                shadowFrame = HERO_SPRITE_UP_SHADOW_WIDE;
            if (shadowFrame >= CURSOR_SHADOW_ANIM_FIRST && shadowFrame < CURSOR_SHADOW_ANIM_END)
                shadowOffset = CURSOR_HORSE_SHADOW_OFFSET;
            else
                shadowOffset = 0;
            IconToBitmap(
                m_shadowIcon,
                gpWindowManager->m_screen,
                drawX - CURSOR_SHADOW_FLIP_X_ADJUST,
                drawY,
                shadowFrame + shadowOffset,
                1,
                0,
                0,
                CURSOR_CLIP_SIZE,
                CURSOR_CLIP_SIZE,
                0
            );
        }
    } else {
        drawFrame_f = m_cursorFrame + m_cursorFrameCount;
        if (m_drawHeroShadows && m_cursorType == CURSOR_HERO_TYPE_BOAT) {
            IconToBitmap(
                m_boatShadowIcon,
                gpWindowManager->m_screen,
                drawX,
                drawY,
                drawFrame_f,
                1,
                0,
                0,
                CURSOR_CLIP_SIZE,
                CURSOR_CLIP_SIZE,
                0
            );
        } else if (m_drawHeroShadows && m_cursorType != CURSOR_HERO_TYPE_BOAT) {
            IconToBitmap(
                m_shadowIcon,
                gpWindowManager->m_screen,
                drawX,
                drawY,
                drawFrame_f,
                1,
                0,
                0,
                CURSOR_CLIP_SIZE,
                CURSOR_CLIP_SIZE,
                0
            );
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

VA(0x0040e198, 0x85)
i32 advManager::GetCursorBaseFrame(i32 direction) {
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
    } else {
        return direction * CURSOR_FRAMES_PER_DIRECTION;
    }
}

VA(0x0040e21d, 0x256)
void advManager::TurnTo(i32 direction) {
    i32 turnStep_c = 1;
    i32 directionDifference = direction - m_cursorDirection;
    if (directionDifference == 0)
        return;
    if ((directionDifference < 0 && directionDifference >= -4)
        || (directionDifference > 0 && directionDifference > 4))
        turnStep_c = -1;
    m_cursorTurning = 1;
    i32 frameIndex_i = m_cursorDirection * 2;
    i32 delay_f = giStepDelay[(&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]]];
    if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] == 0)
        delay_f *= CURSOR_SLOW_TURN_MULTIPLIER;
    if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] == 1)
        delay_f = static_cast<i32>(delay_f * 1.5);

    do {
        m_cursorCycle = 1;
        if (m_cursorType >= CURSOR_HERO_TYPE_BOAT)
            m_cursorFrame = boatFrameFlip[frameIndex_i];
        else
            m_cursorFrame = horseFrameFlip[frameIndex_i];
        m_cursorFrameCount = 0;
        glTimers[1] = delay_f + KBTickCount();
        if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] != 4) {
            if (ComboDraw(m_mapOriginX, m_mapOriginY, 0))
                UpdateScreen(0, 0);
            if (bShowIt)
                DelayTil(&glTimers[1]);
        }
        frameIndex_i += turnStep_c;
        if (frameIndex_i < 0)
            frameIndex_i = CURSOR_TURN_FRAME_COUNT - 1;
        frameIndex_i %= CURSOR_TURN_FRAME_COUNT;
    } while (direction * 2 != frameIndex_i);

    m_cursorDirection = direction;
    StopCursor(1);
    if (bShowIt)
        DelayTil(&glTimers[1]);
    if (ComboDraw(m_mapOriginX, m_mapOriginY, 0))
        UpdateScreen(0, 0);
}

VA(0x0040e473, 0xac)
i32 advManager::GetMoveShowIt(hero* movingHero, i32 direction) {
    i32 directionX = normalDirTable[direction].x;
    i32 directionY = normalDirTable[direction].y;
    if ((gbThisNetHumanPlayer[giCurPlayer] || !gConfig.blackoutComputer)
        && (MapExtraPosAndAdjacentsSet(movingHero->m_x, movingHero->m_y, giCurWatchPlayerBit)
            || MapExtraPosAndAdjacentsSet(
                movingHero->m_x + directionX,
                movingHero->m_y + directionY,
                giCurWatchPlayerBit
            )))
        return 1;
    else
        return 0;
}

// @semantic
// Complete semantics, 0x84 frame, named/temporary slots, case-body order,
// jump-table data ranges, and 158/158 ordered effective relocations are
// accounted for. Scalar-lvalue escapes fixed the packed object-metadata write
// and step/halfSteps operand order. The first non-relocation residual is one
// extra retail continuation jump after the hero embarked test; later residuals
// are local-scope trampoline counts and compiler-local tables. Explicit nested
// hero arms, equality operand orders, pointer-form resource access, and the
// positive eventCell arm were already rejected. Revisit if CURSOR TU state,
// relevant hero/map layout, or continuation-target normalization changes.
VA(0x0040e51f, 0x1234)
mapCell* advManager::MoveHero(
    i32 direction,
    i32 stopAfterMove,
    i32* eventX,
    i32* eventY,
    i32* outOfMobility,
    i32 processEvent,
    i32* adjacentMonster,
    i32 forceMove
) {
    EventExtra* mapEvent_e = 0;
    SAMPLE2 fizzleSample_h = NULL_SAMPLE2;
    mapCell* eventCell_g;
    hero* movingHero_f;
    mapCell* currentCell_f;
    mapCell* destinationCell_j;
    mapCell* cursorCell_m;
    i32 oldHeroX_b;
    i32 oldHeroY_b;
    i32 directionX_b;
    i32 directionY_b;
    i32 terrainCost_e;
    i32 nextTerrainCost_b;
    i32 pixelsPerStep_o;
    i32 stepDelay_d;
    i32 halfSteps_o;
    i32 step_a;

    if (gbThisNetHumanPlayer[giCurPlayer])
        SetNoDialogMenus(0);
    gbHitEvent = false;
    *adjacentMonster = 0;
    *outOfMobility = 0;
    gbHeroMoving = true;
    eventCell_g = 0;

    movingHero_f = gpGame->GetHero(gpCurPlayer->m_currentHero);
    oldHeroX_b = movingHero_f->m_x;
    oldHeroY_b = movingHero_f->m_y;
    directionX_b = normalDirTable[direction].x;
    directionY_b = normalDirTable[direction].y;
    bShowIt = GetMoveShowIt(movingHero_f, direction);
    if (bShowIt)
        gbMoveShown = true;

    currentCell_f = GetCell(movingHero_f->m_x, movingHero_f->m_y);
    i32 currentTerrain_b = giGroundToTerrain[currentCell_f->m_terrainImageIndex];
    destinationCell_j = GetCell(movingHero_f->m_x + directionX_b, movingHero_f->m_y + directionY_b);
    terrainCost_e = CalcTerrainCost(
        currentTerrain_b,
        direction & 1,
        movingHero_f->m_remainingMobility,
        movingHero_f->m_secondarySkills[IDX(HERO_SKILL_PATHFINDING)],
        currentCell_f->m_isRoad,
        destinationCell_j->m_isRoad
    );
    nextTerrainCost_b = CalcTerrainCost(
        giGroundToTerrain[destinationCell_j->m_terrainImageIndex],
        0,
        movingHero_f->m_remainingMobility - terrainCost_e,
        movingHero_f->m_secondarySkills[IDX(HERO_SKILL_PATHFINDING)],
        destinationCell_j->m_isRoad,
        1
    );

    if (!forceMove && movingHero_f->m_remainingMobility < terrainCost_e) {
        *outOfMobility = 1;
        movingHero_f->m_remainingMobility = 0;
        StopCursor(1);
        goto movementDone;
    }

    SendMapChange(
        1,
        movingHero_f->m_id,
        static_cast<u8>(movingHero_f->m_x),
        static_cast<u8>(movingHero_f->m_y),
        giCurPlayer,
        static_cast<u8>(stopAfterMove),
        static_cast<u8>(direction)
    );
    MobilizeCurrHero(0);
    *eventX = movingHero_f->m_x + directionX_b;
    *eventY = movingHero_f->m_y + directionY_b;
    if (m_cursorDirection != direction)
        TurnTo(direction);
    movingHero_f->m_direction = static_cast<u8>(direction);

    if (HAS(movingHero_f->m_eventFlags, HERO_EVENT_EMBARKED)
        && destinationCell_j->m_triggerType == MAP_OBJECT_COAST) {
        for (step_a = 0; step_a < CURSOR_BOAT_COUNT; ++step_a) {
            if (gpGame->m_boats[step_a].heroId == static_cast<u8>(movingHero_f->m_id))
                break;
        }
        boatRecord* boat = &gpGame->m_boats[step_a];
        mapCell* boatCell_a = GetCell(movingHero_f->m_x, movingHero_f->m_y);
        boat->savedTriggerType = boatCell_a->m_triggerType;
        boat->savedEventData = static_cast<u8>(boatCell_a->m_objectMetadata);
        boat->direction = static_cast<i8>(m_cursorDirection);
        boat->heroId = static_cast<i8>(boat->heroId | MAP_TRIGGER_ACTION_FLAG);
        boatCell_a->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT;
        boatCell_a->m_objectMetadata = static_cast<u16>(OD_STEER(step_a));
        boat->x = static_cast<i8>(movingHero_f->m_x);
        boat->y = static_cast<i8>(movingHero_f->m_y);
        StopCursor(1);
        CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
        UpdateScreen(0, 0);
        m_cursorActive = 0;
    }

    if ((destinationCell_j->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
        && gpAdvManager->ValidMoveWithEvent(movingHero_f, direction)) {
        switch (destinationCell_j->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
            case MAP_OBJECT_BOAT:
                if (HAS(movingHero_f->m_eventFlags, HERO_EVENT_EMBARKED))
                    goto movementDone;
                StopCursor(1);
                m_cursorActive = 0;
                fizzleSample_h = LoadPlaySample("killfade.82m");
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
                    0,
                    0
                );
                WaitEndSample(fizzleSample_h, -1);
                break;

            case MAP_OBJECT_HERO_INTERACTION:
                if (HAS(movingHero_f->m_eventFlags, HERO_EVENT_EMBARKED)) {
                    if (gpGame->GetHero(destinationCell_j->m_objectMetadata)->m_eventFlags
                        & HERO_EVENT_EMBARKED)
                        goto stoppingEvent;
                    else
                        goto movementDone;
                } else {
                    goto stoppingEvent;
                }

            case MAP_OBJECT_CASTLE:
                if (gpGame->GetTown(destinationCell_j->m_objectMetadata)->m_owner != giCurPlayer
                    && gpGame->GetTown(destinationCell_j->m_objectMetadata)->HasGarrison()) {

                stoppingEvent:
                    StopCursor(1);
                    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
                    UpdateScreen(0, 0);
                    movingHero_f->m_remainingMobility -= terrainCost_e;
                    if (movingHero_f->m_remainingMobility < nextTerrainCost_b) {
                        movingHero_f->m_remainingMobility = 0;
                        stopAfterMove = 1;
                    }
                    eventCell_g = destinationCell_j;
                    goto movementDone;
                }
                break;

            default:
                if (StopOnTrigger(destinationCell_j)) {
                    StopCursor(1);
                    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
                    UpdateScreen(0, 0);
                    movingHero_f->m_remainingMobility -= terrainCost_e;
                    if (movingHero_f->m_remainingMobility < nextTerrainCost_b) {
                        movingHero_f->m_remainingMobility = 0;
                        stopAfterMove = 1;
                    }
                    eventCell_g = destinationCell_j;
                    goto movementDone;
                }
                break;
        }
    }

    if (!ValidMove(direction, 0))
        goto movementDone;

    if (movingHero_f->m_locationType == HERO_TOWN_LOCATION) {
        town* occupiedTown = gpGame->GetTown(movingHero_f->m_occupiedTown);
        occupiedTown->m_occupyingHeroId = -1;
    }
    if (m_visibilityMapValid) {
        *(m_visibilityMap + (movingHero_f->m_y + directionY_b) * MAP_WIDTH
          + (movingHero_f->m_x + directionX_b)) = 0;
    }
    m_updateMinY = 0;
    m_updateMinX = m_updateMinY;
    gpGame->SetVisibility(
        m_mapOriginX + directionX_b + CURSOR_MAP_DRAW_OFFSET,
        m_mapOriginY + directionY_b + CURSOR_MAP_DRAW_OFFSET,
        giCurPlayer,
        giVisRange[movingHero_f->m_secondarySkills[IDX(HERO_SKILL_SCOUTING)]]
            + (movingHero_f->HasArtifact(CURSOR_VISIBILITY_ARTIFACT) != 0)
    );
    m_forceCompleteDraw = 1;

    pixelsPerStep_o =
        giPixelsPerStep[(&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]]];
    stepDelay_d = giStepDelay[(&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]]];
    StartCursor(direction);
    if ((&gConfig.computerWalkSpeed)[gbThisNetHumanPlayer[giCurPlayer]] == 4) {
        if (EveryOther)
            --m_cursorFrame;
        bMoveSoundMade = 0;
        MoveOrigin(directionX_b, directionY_b);
        movingHero_f->m_x += directionX_b;
        movingHero_f->m_y += directionY_b;
        if (ComboDraw(0))
            UpdateScreen(0, 0);
        EveryOther = 1 - EveryOther;
    } else {
        if (directionX_b == 1 && directionY_b == -1) {
            giDeferObjDrawX = movingHero_f->m_x + directionX_b;
            giDeferObjDrawY = movingHero_f->m_y;
        } else if (directionX_b == -1 && directionY_b == 1) {
            giDeferObjDrawX = movingHero_f->m_x;
            giDeferObjDrawY = movingHero_f->m_y + directionY_b;
        } else if (directionX_b == 1 && directionY_b == 1) {
            giDeferObjDrawX = movingHero_f->m_x;
            giDeferObjDrawY = movingHero_f->m_y + directionY_b;
        } else if (directionX_b == -1 && directionY_b == -1) {
            giDeferObjDrawX = movingHero_f->m_x + directionX_b;
            giDeferObjDrawY = movingHero_f->m_y;
        }

        gbEnlargeScreenBlit = false;
        gbNoBorder = true;
        halfSteps_o = CURSOR_MOVE_HALF_TILE_PIXELS / pixelsPerStep_o;
        for (step_a = 0; step_a < halfSteps_o * 2; ++step_a) {
            if (step_a == halfSteps_o) {
                MoveOrigin(directionX_b, directionY_b);
                movingHero_f->m_x += directionX_b;
                movingHero_f->m_y += directionY_b;
                m_updateMinX = startVals[directionX_b + 1];
                m_updateMinY = startVals[directionY_b + 1];
            }
            i32l tick = KBTickCount();
            if (OD_STEER(step_a) + 1 == halfSteps_o * 2) {
                m_updateMinX = 0;
                m_updateMinY = 0;
            } else {
                m_updateMinX += pixelsPerStep_o * directionX_b;
                m_updateMinY += pixelsPerStep_o * directionY_b;
            }
            if (ComboDraw(0)) {
                giLimitUpdMinX = -1;
                UpdateScreen(0, 0);
            }
            if (bShowIt)
                DelayTilMilli(tick + stepDelay_d);
        }
        giDeferObjDrawX = giDeferObjDrawY = -1;
        gbNoBorder = false;
        DrawAdventureBorder();
        gbEnlargeScreenBlit = true;
    }

    movingHero_f->m_remainingMobility -= terrainCost_e;
    if (movingHero_f->m_remainingMobility < nextTerrainCost_b) {
        movingHero_f->m_remainingMobility = 0;
        stopAfterMove = 1;
    }
    mapEvent_e = GetMapEvent(*eventX, *eventY);
    if (mapEvent_e && !processEvent)
        stopAfterMove = 1;
    StopCursor(stopAfterMove);
    if (processEvent && stopAfterMove && ComboDraw(0))
        UpdateScreen(0, 0);

    SetEnvironmentOrigin(
        m_mapOriginX + CURSOR_MAP_DRAW_OFFSET,
        m_mapOriginY + CURSOR_MAP_DRAW_OFFSET,
        0
    );
    step_a = GetCell(m_mapOriginX + CURSOR_MAP_DRAW_OFFSET, m_mapOriginY + CURSOR_MAP_DRAW_OFFSET)
                 ->m_terrainImageIndex;
    if (giGroundToTerrain[step_a] != m_currentTerrain) {
        m_currentTerrain = giGroundToTerrain[step_a];
        if (gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI)
            gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[m_currentTerrain]);
    }
    m_updateMinY = 0;
    m_updateMinX = m_updateMinY;

    cursorCell_m = GetCell(m_cursorMapX + m_mapOriginX, m_cursorMapY + m_mapOriginY);
    *eventX = m_cursorMapX + m_mapOriginX;
    *eventY = m_cursorMapY + m_mapOriginY;
    if ((cursorCell_m->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
        || (HAS(movingHero_f->m_eventFlags, HERO_EVENT_EMBARKED)
            && cursorCell_m->m_triggerType == MAP_OBJECT_COAST)) {
        eventCell_g = cursorCell_m;
        switch (cursorCell_m->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
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
                eventCell_g = 0;
        }
    }

movementDone:
    UpdateRadar(1, 1);
    gbHeroMoving = false;
    if (!forceMove) {
        if (movingHero_f->m_x != oldHeroX_b || movingHero_f->m_y != oldHeroY_b) {
            if (mapExtra[movingHero_f->m_y * MAP_WIDTH + movingHero_f->m_x]
                & MAP_TRIGGER_ACTION_FLAG) {
                if (HAS(movingHero_f->m_eventFlags, HERO_EVENT_EMBARKED)) {
                } else {
                    if (!eventCell_g)
                        goto checkAdjacent;
                    if ((eventCell_g->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_BOAT) {
                    } else {
                    checkAdjacent:
                        CheckAdjacentMon(adjacentMonster);
                        if (movingHero_f->m_owner != giCurPlayer)
                            eventCell_g = 0;
                    }
                }
            }
        }
    }
    if (gbThisNetHumanPlayer[giCurPlayer])
        SetNoDialogMenus(1);

    if (mapEvent_e) {
        if (processEvent) {
            if (mapEvent_e->applyToComputer) {
                for (step_a = 0; step_a < CURSOR_RESOURCE_COUNT; ++step_a) {
                    gpGame->m_players[giCurPlayer].m_resources[step_a] +=
                        mapEvent_e->resources[step_a];
                    if (gpGame->m_players[giCurPlayer].m_resources[step_a] < 0)
                        gpGame->m_players[giCurPlayer].m_resources[step_a] = 0;
                }
                if (mapEvent_e->artifact != -1
                    && movingHero_f->NumArtifacts() < CURSOR_ARTIFACT_CAPACITY)
                    GiveArtifact(movingHero_f, mapEvent_e->artifact, 1, -1);
                if (mapEvent_e->cancelAfterVisit)
                    mapEvent_e->active = 0;
            }
        } else {
            i32 primaryType_k = -1;
            i32 primaryAmount_a = 0;
            i32 secondaryType_k = -1;
            i32 secondaryAmount_e = 0;
            for (step_a = 0; step_a < CURSOR_RESOURCE_COUNT; ++step_a) {
                i32 eventAmount = mapEvent_e->resources[step_a];
                if (gpGame->m_players[giCurPlayer].m_resources[step_a] < -eventAmount) {
                    eventAmount = -gpGame->m_players[giCurPlayer].m_resources[step_a];
                }
                gpGame->m_players[giCurPlayer].m_resources[step_a] += mapEvent_e->resources[step_a];
                if (gpGame->m_players[giCurPlayer].m_resources[step_a] < 0)
                    gpGame->m_players[giCurPlayer].m_resources[step_a] = 0;
                if (eventAmount) {
                    if (primaryType_k != -1) {
                        secondaryType_k = primaryType_k;
                        secondaryAmount_e = primaryAmount_a;
                    }
                    primaryType_k = step_a;
                    primaryAmount_a = eventAmount;
                }
            }
            if (mapEvent_e->artifact != -1
                && movingHero_f->NumArtifacts() < CURSOR_ARTIFACT_CAPACITY) {
                GiveArtifact(movingHero_f, mapEvent_e->artifact, 1, -1);
                if (primaryType_k != -1) {
                    secondaryType_k = primaryType_k;
                    secondaryAmount_e = primaryAmount_a;
                }
                primaryType_k = CURSOR_RESOURCE_COUNT;
                primaryAmount_a = mapEvent_e->artifact;
            }
            if (mapEvent_e->cancelAfterVisit)
                mapEvent_e->active = 0;
            if (primaryType_k >= 0 && primaryType_k <= CURSOR_RESOURCE_LAST && primaryAmount_a < 0)
                primaryAmount_a -= CURSOR_RESOURCE_DIALOG_PENALTY;
            if (secondaryType_k >= 0 && secondaryType_k <= CURSOR_RESOURCE_LAST
                && secondaryAmount_e < 0)
                secondaryAmount_e -= CURSOR_RESOURCE_DIALOG_PENALTY;
            NormalDialog(
                mapEvent_e->message,
                1,
                -1,
                -1,
                primaryType_k,
                primaryAmount_a,
                secondaryType_k,
                secondaryAmount_e,
                -1,
                0
            );
            gbHitEvent = true;
        }
    }
    return eventCell_g;
}

VA(0x0040f753, 0x174)
void advManager::CheckAdjacentMon(i32* adjacentMonster) {
    hero* currentHero_f;
    i32 killed_e;
    i32 monsterX_e;
    i32 monsterY_f;
    mapCell* monsterCell_d;
    mapCell* heroCell_f;

    currentHero_f = gpGame->GetHero(gpCurPlayer->m_currentHero);
    killed_e = 0;
    if (FindAdjacentMonster(
            currentHero_f->m_x,
            currentHero_f->m_y,
            &monsterX_e,
            &monsterY_f,
            -1,
            -1
        )) {
        StopCursor(1);
        CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
        UpdateScreen(0, 0);
        monsterCell_d = GetCell(monsterX_e, monsterY_f);
        heroCell_f = GetCell(currentHero_f->m_x, currentHero_f->m_y);
        if (gbThisNetHumanPlayer[giCurPlayer])
            PlayerMonsterInteract(
                monsterCell_d,
                heroCell_f,
                currentHero_f,
                &killed_e,
                currentHero_f->m_x,
                currentHero_f->m_y,
                1,
                monsterX_e,
                monsterY_f
            );
        else
            ComputerMonsterInteract(monsterCell_d, currentHero_f, &killed_e);
        if (killed_e) {
            EraseObj(monsterCell_d, monsterX_e, monsterY_f);
            if (gbThisNetHumanPlayer[giCurPlayer])
                FizzleCenter(EVENT_FIZZLE_HERO_LOSS);
        }
        *adjacentMonster = 1;
    }
}

VA(0x0040f8c7, 0x14e)
i32 advManager::ValidMoveWithEvent(hero* movingHero, i32 direction) {
    i32 directionX0;
    i32 destinationX0;
    i32 directionY0;
    i32 destinationY0;
    mapCell* destinationCell0;

    directionX0 = normalDirTable[direction].x;
    directionY0 = normalDirTable[direction].y;
    destinationX0 = movingHero->m_x + directionX0;
    destinationY0 = movingHero->m_y + directionY0;
    if (destinationX0 < 0 || destinationX0 > MAP_WIDTH - 1 || destinationY0 < 0
        || destinationY0 > MAP_HEIGHT - 1)
        return 0;

    destinationCell0 = m_mapData->GetCell(destinationX0, destinationY0);
    switch (destinationCell0->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
        case MAP_OBJECT_HERO_INTERACTION:
            if (HAS(movingHero->m_eventFlags, HERO_EVENT_EMBARKED)) {
                if (gpGame->GetHero(destinationCell0->m_objectMetadata)->m_eventFlags
                    & HERO_EVENT_EMBARKED)
                    return 1;
                else
                    return 0;
            }
            return 1;
        default:
            return ValidMove(direction, 1);
    }
}

// @early-stop
// All 352 normalized instructions, the 0x3c frame/slots, CFG, and nine ordered
// relocation sites/effective targets align. Retained second-term OR-zero removed
// the +0x211/+0x214 centerY/directionY span; only +0x442/+0x443/+0x44b/+0x44c
// remains, where the cursor/origin Y addends load in reverse order. Nine bounded
// candidates covered direct reversal, both OD_STEER(term) escapes, both OR-zero forms,
// regroupings, and a targeted depth-one commutative_order AST edit. Revisit after
// an earlier CURSOR source edit or relevant advManager layout/header change
// perturbs TU-cumulative operand evaluation. Retail also delinks normalDirTable.y.
VA(0x0040fa15, 0x4f2)
i32 advManager::ValidMove(i32 direction, i32 eventMode) {
    i32 directionX_j;
    i32 directionY_h;
    i32 destinationMapX_e;
    i32 destinationMapY_e;
    i32 centerX_p;
    i32 centerY_n;
    i32 destinationCellX_h;
    i32 destinationCellY_e;
    mapCell* destinationCell_g;
    mapCell* currentCell_c;
    mapCell* northNeighborCell_b;
    mapCell* southNeighborCell_g;
    i32 northDirection_b;
    i32 southDirection_e;

    directionX_j = normalDirTable[direction].x;
    directionY_h = normalDirTable[direction].y;
    destinationMapX_e = m_mapOriginX + directionX_j;
    destinationMapY_e = m_mapOriginY + directionY_h;
    centerX_p = m_mapOriginX + CURSOR_MAP_DRAW_OFFSET;
    centerY_n = m_mapOriginY + CURSOR_MAP_DRAW_OFFSET;
    destinationCellX_h = destinationMapX_e + CURSOR_MAP_DRAW_OFFSET;
    destinationCellY_e = destinationMapY_e + CURSOR_MAP_DRAW_OFFSET;

    if (destinationMapX_e < -CURSOR_MAP_DRAW_OFFSET
        || destinationMapX_e > MAP_WIDTH - CURSOR_MAP_DRAW_OFFSET - 1)
        return 0;
    if (destinationMapY_e < -CURSOR_MAP_DRAW_OFFSET
        || destinationMapY_e > MAP_HEIGHT - CURSOR_MAP_DRAW_OFFSET - 1)
        return 0;

    destinationCell_g = m_mapData->GetCell(destinationCellX_h, destinationCellY_e);
    currentCell_c = m_mapData->GetCell(centerX_p, centerY_n);
    if (destinationCell_g->m_flags & CURSOR_CELL_BLOCKED_FLAG)
        return 0;

    if (giGroundToTerrain[destinationCell_g->m_terrainImageIndex] == CURSOR_WATER_TERRAIN) {
        if (m_cursorType != CURSOR_HERO_TYPE_BOAT
            && destinationCell_g->m_triggerType != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT)
            && destinationCell_g->m_triggerType != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK))
            return 0;
        if (giGroundToTerrain[currentCell_c->m_terrainImageIndex] == CURSOR_WATER_TERRAIN
            && directionX_j != 0 && directionY_h != 0) {
            if (giGroundToTerrain[m_mapData->GetCell(centerX_p + directionX_j, centerY_n)
                                      ->m_terrainImageIndex]
                    != CURSOR_WATER_TERRAIN
                || giGroundToTerrain[m_mapData->GetCell(centerX_p, centerY_n + (directionY_h | 0))
                                         ->m_terrainImageIndex]
                       != CURSOR_WATER_TERRAIN)
                return 0;
        }
    } else if (m_cursorType == CURSOR_HERO_TYPE_BOAT
               && destinationCell_g->m_triggerType != MAP_OBJECT_COAST) {
        return 0;
    }

    northDirection_b = (1 << direction) & CURSOR_NORTH_DIRECTION_MASK;
    southDirection_e = (1 << direction) & CURSOR_SOUTH_DIRECTION_MASK;
    if (northDirection_b) {
        if (currentCell_c->m_objectIndex != CURSOR_EMPTY_OBJECT_INDEX
            && currentCell_c->m_objectTileset != CURSOR_PASSABLE_OBJECT_TILESET
            && !(currentCell_c->m_flags & CURSOR_OBJECT_PASSABLE_FLAG)
            && currentCell_c->m_triggerType != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WHIRLPOOL))
            return 0;
        if (destinationCell_g->m_overlayIndex != CURSOR_EMPTY_OBJECT_INDEX) {
            northNeighborCell_b = m_mapData->GetCell(destinationCellX_h, destinationCellY_e + 1);
            if (northNeighborCell_b->m_objectIndex != CURSOR_EMPTY_OBJECT_INDEX
                && northNeighborCell_b->m_objectTileset != CURSOR_PASSABLE_OBJECT_TILESET
                && !(northNeighborCell_b->m_flags & CURSOR_OBJECT_PASSABLE_FLAG))
                return 0;
        }
    }
    if (southDirection_e) {
        if (destinationCell_g->m_objectIndex != CURSOR_EMPTY_OBJECT_INDEX
            && destinationCell_g->m_objectTileset != CURSOR_PASSABLE_OBJECT_TILESET
            && !(destinationCell_g->m_flags & CURSOR_OBJECT_PASSABLE_FLAG)
            && destinationCell_g->m_triggerType != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WHIRLPOOL)
            && (!eventMode || !(destinationCell_g->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                || !StopOnTrigger(destinationCell_g)))
            return 0;
        if (currentCell_c->m_overlayIndex != CURSOR_EMPTY_OBJECT_INDEX) {
            southNeighborCell_g =
                m_mapData->GetCell(m_cursorMapX + m_mapOriginX, m_cursorMapY + m_mapOriginY + 1);
            if (southNeighborCell_g->m_objectIndex != CURSOR_EMPTY_OBJECT_INDEX
                && southNeighborCell_g->m_objectTileset != CURSOR_PASSABLE_OBJECT_TILESET
                && !(southNeighborCell_g->m_flags & CURSOR_OBJECT_PASSABLE_FLAG)
                && !(southNeighborCell_g->m_triggerType & MAP_TRIGGER_ACTION_FLAG))
                return 0;
        }
    }
    return 1;
}

// @semantic
// Complete 0x24b body, 0x24 frame/slots, CFG, and zero relocations. Every
// non-jump opcode/operand matches, but four GetCell continuation jumps are real
// branch-byte residuals: ours lead at +0x6e/+0x105/+0x158/+0x1ef and retail
// trails at +0xad/+0x132/+0x197/+0x21c. One m_mapData object-lvalue family
// changed the accessor operands and was rejected. Revisit only if fullMap's
// GetCell accessor/source changes, an earlier CURSOR edit moves inline tails, or
// comparison gains proved continuation-target normalization.
VA(0x0040ff07, 0x24b)
void advManager::MoveOrigin(i32 directionX, i32 directionY) {
    i32 oldOriginX0;
    i32 cellY1;
    i32 cellX5;
    i32 oldOriginY9;
    mapCell* newCursorCell5;
    mapCell* oldCursorCell0;
    mapCell* newPreviousCell4;
    mapCell* oldPreviousCell2;

    oldOriginX0 = m_mapOriginX;
    oldOriginY9 = m_mapOriginY;
    m_mapOriginX += directionX;
    m_mapOriginY += directionY;
    directionX = oldOriginX0 - m_mapOriginX;
    directionY = oldOriginY9 - m_mapOriginY;
    if (directionX != 0 || directionY != 0) {
        oldCursorCell0 = m_mapData->GetCell(m_cursorMapX + oldOriginX0, m_cursorMapY + oldOriginY9);
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
                m_previousCursorMapY + oldOriginY9
            );
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

// @semantic
// After excluding the +0x717..+0x743 jump table, executable extent,
// 0x2c frame, all eight stack roles, every non-jump opcode/operand, and 98/98
// relocation targets agree. Two fullMap::GetCell continuations are leading in
// base at +0x5bc/+0x5fa and trailing in retail at +0x5ee/+0x62c. Retail also
// carries one trailing NOP at +0x74f. No source variant was retained because the
// two branch sites are isolated; revisit only if fullMap::GetCell changes, an
// earlier CURSOR edit moves inline tails, or comparison gains proved continuation
// normalization.
VA(0x00410152, 0x74f)
void advManager::ProcessMapChange(SMapChange change) {
    hero* mapHero_n;
    mapCell* eventCell_n;
    i32 eventX_b;
    i32 eventY_c;
    i32 outOfMobility_g;
    i32 adjacentMonster_m;
    mapCell* recruitCell_i;
    mapCell* heroCell_i;

    giMapChangeCtr = change.sequence + 1;
    if (change.player != giCurPlayer) {
        gpAdvManager->DeactivateCurrTown();
        gpAdvManager->DeactivateCurrHero();
    }
    if (change.player >= 0 && change.player < CURSOR_PLAYER_COUNT) {
        giCurPlayer = change.player;
        gpCurPlayer = &gpGame->m_players[giCurPlayer];
        giCurPlayerBit = 1 << giCurPlayer;
    }

    switch (change.type) {
        case IDX(MAP_CHANGE_MOVE_HERO):
            LogInt(
                "MC Move Hero",
                change.id,
                change.x,
                change.y,
                change.direction,
                change.sequence,
                gpGame->GetHero(change.id)->m_x,
                gpGame->GetHero(change.id)->m_y
            );
            mapHero_n = gpGame->GetHero(change.id);
            if (change.x != mapHero_n->m_x || change.y != mapHero_n->m_y) {
                sprintf(
                    gText,
                    "Data miscommunication in hero position, first %d, %d, second %d, %d.  Please "
                    "give Phil a copy of  your Autosave and, if possible, instructions to recreate "
                    "this error",
                    mapHero_n->m_x,
                    mapHero_n->m_y,
                    change.x,
                    change.y
                );
                LogStr(gText);
                break;
            }
            gpAdvManager->SetHeroContext(change.id, 0);
            eventCell_n = MoveHero(
                change.direction,
                change.stopAfterMove,
                &eventX_b,
                &eventY_c,
                &outOfMobility_g,
                1,
                &adjacentMonster_m,
                1
            );
            if (eventCell_n != 0) {
                switch (eventCell_n->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
                    case MAP_OBJECT_COAST:
                    case MAP_OBJECT_BOAT:
                        DoAIEvent(eventCell_n, mapHero_n, eventX_b, eventY_c);
                        break;
                }
            }
            break;

        case IDX(MAP_CHANGE_MY_TURN):
            LogInt(
                "MC My Turn",
                change.x,
                change.y,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED
            );
            break;

        case IDX(MAP_CHANGE_TELEPORT_HERO):
            LogInt(
                "MC Teleport Hero",
                change.x,
                change.y,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED
            );
            mapHero_n = gpGame->GetHero(change.id);
            TeleportTo(mapHero_n, change.x, change.y, 0, 1);
            break;

        case IDX(MAP_CHANGE_CLAIM_MINE):
            LogInt(
                "MC ClaimMine",
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED
            );
            gpGame->ClaimMine(change.id, change.player);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case IDX(MAP_CHANGE_CLAIM_TOWN):
            LogInt(
                "MC ClaimTown",
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED
            );
            gpGame->ClaimTown(change.id, change.player, 1);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case IDX(MAP_CHANGE_BUILD_BOAT):
            LogInt(
                "MC BuildBoat",
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED
            );
            gpGame->CreateBoat(change.x, change.y, 1);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case IDX(MAP_CHANGE_ERASE_OBJECT):
            LogInt(
                "MC Erase Object",
                change.x,
                change.y,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED,
                CURSOR_LOG_UNUSED
            );
            eventCell_n = GetCell(change.x, change.y);
            EraseObj(eventCell_n, change.x, change.y);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case IDX(MAP_CHANGE_DEAD_HERO):
            LogStr("MC DeadHero");
            mapHero_n = gpGame->GetHero(change.id);
            if (change.x != mapHero_n->m_x || change.y != mapHero_n->m_y)
                break;
            mapHero_n->Deallocate(1);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case IDX(MAP_CHANGE_RECRUIT_HERO):
            LogStr("MC RecruitHero");
            mapHero_n = gpGame->GetHero(change.id);
            mapHero_n->m_x = change.x;
            mapHero_n->m_y = change.y;
            mapHero_n->m_eventFlags = HERO_EVENT_NONE;
            mapHero_n->m_direction = CURSOR_RECRUIT_HERO_DIRECTION;
            mapHero_n->m_locationType =
                gpGame->m_worldMap.GetCell(change.x, change.y)->m_triggerType;
            mapHero_n->m_occupiedTown =
                gpGame->m_worldMap.GetCell(change.x, change.y)->m_objectMetadata;
            mapHero_n->m_owner = change.player;
            recruitCell_i = gpGame->m_worldMap.GetCell(change.x, change.y);
            recruitCell_i->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION;
            heroCell_i = gpGame->m_worldMap.GetCell(change.x, change.y);
            heroCell_i->m_objectMetadata = change.id;
            gpAdvManager->SetHeroContext(change.id, 0);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case IDX(MAP_CHANGE_DEAD_PLAYER):
            LogStr("Dead Player");
            sprintf(gText, "%s has been vanquished!", cPlayerNames[change.id]);
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

        case IDX(MAP_CHANGE_UNUSED):
        default:
            break;
    }
}

VA(0x004108a1, 0x1ba)
void advManager::ProcessIncomingSingleMapChange(SMapChange* incoming) {
    i32 slot;

    if (incoming->sequence < giMapChangeCtr)
        return;

    if (incoming->sequence == giMapChangeCtr) {
        ProcessMapChange(*incoming);
    } else {
    retryInsert:
        for (slot = 0; slot < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++slot) {
            if (sMapChangeQueue[slot].type != 0
                && sMapChangeQueue[slot].sequence == incoming->sequence) {
                LogInt(
                    "OQ",
                    incoming->sequence,
                    giMapChangeCtr,
                    CURSOR_LOG_UNUSED,
                    CURSOR_LOG_UNUSED,
                    CURSOR_LOG_UNUSED,
                    CURSOR_LOG_UNUSED,
                    CURSOR_LOG_UNUSED
                );
                goto duplicateChange;
            }
        }

        for (slot = 0; slot < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++slot) {
            if (sMapChangeQueue[slot].type == 0) {
                LogInt(
                    "SQ",
                    incoming->sequence,
                    giMapChangeCtr,
                    CURSOR_LOG_UNUSED,
                    CURSOR_LOG_UNUSED,
                    CURSOR_LOG_UNUSED,
                    CURSOR_LOG_UNUSED,
                    CURSOR_LOG_UNUSED
                );
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

VA(0x00410a5b, 0xce)
void advManager::ProcessIncomingGroupMapChange(char* incomingData) {
    DATA(0x004ee1dc) static i16 s_groupLineBase = 1505;
    SMapChange* ptr;
    i32 size;
    SMapChange* buf;
    i32 i;
    i32 processed;

    size = sizeof(sMapChangeLastFew);
    buf = static_cast<SMapChange*>(H2_ALLOC(size, 1512));
    memcpy(buf, incomingData, size);
    for (i = CURSOR_MAP_CHANGE_RECENT_COUNT - 1; i >= 0; --i) {
        ptr = &buf[i];
        if (ptr->type != 0 && ptr->sequence >= giMapChangeCtr) {
            ProcessIncomingSingleMapChange(ptr);
        } else {
            processed = 0;
        }
    }
    H2_FREE(buf, 1530);
}

VA(0x00410b29, 0x75)
void advManager::PurgeMapChangeQueue(void) {
    i32 slot;

    for (slot = 0; slot < CURSOR_MAP_CHANGE_QUEUE_COUNT; ++slot)
        sMapChangeQueue[slot].type = 0;
    for (slot = 0; slot < CURSOR_MAP_CHANGE_RECENT_COUNT; ++slot)
        sMapChangeLastFew[slot].type = 0;
}

// @semantic
// Complete 0x1d4 body, 0x1c frame/slots, CFG, and 12/12 ordered relocation
// sites/effective targets align. First executable divergence is +0xe: retail
// loads maximumToUnwind before comparing unwoundChanges (`cmp local,eax; jge`),
// while ours loads the local first and emits the reversed equivalent compare.
// Four bounded condition families (commuted relation, OD_STEER(parameter), explicit
// break, and negated relation) were byte-neutral or added a jump. Revisit only
// if the parameter/local representation or an earlier CURSOR edit changes operand
// evaluation, or comparison gains proved relational-swap normalization. Retail
// also delinks three sMapChangeQueue+7 references as a string identity.
VA(0x00410b9e, 0x1d4)
void advManager::UnwindMapChangeQueue(i32 maximumToUnwind, i32 processChanges) {
    i32 queuedChanges;
    i32 unwoundChanges;
    i32 lowestSlot;
    i32 lowestSequence;
    i32 continueUnwinding;
    i32 slot;

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
            if (sMapChangeQueue[slot].type != 0
                && sMapChangeQueue[slot].sequence == giMapChangeCtr) {
                if (processChanges)
                    ProcessMapChange(sMapChangeQueue[slot]);
                sMapChangeQueue[slot].type = 0;
                continueUnwinding = 1;
            }
        }
    }
}

VA(0x00410d72, 0x11a)
void SendMapChange(i32 type, i8 id, u8 x, u8 y, i32 player, u8 stopAfterMove, u8 direction) {
    SMapChange change;

    if (!gbThisNetGotAdventureControl)
        return;
    if (!gbRemoteOn)
        return;
    if (player == CURSOR_LOG_UNUSED)
        player = giCurPlayer;
    LogInt(
        "Send Map Change",
        type,
        id,
        x,
        y,
        CURSOR_LOG_UNUSED,
        CURSOR_LOG_UNUSED,
        CURSOR_LOG_UNUSED
    );
    memset(&change, 0, sizeof(change));
    change.type = static_cast<u8>(type);
    change.id = id;
    change.x = x;
    change.y = y;
    change.player = static_cast<i8>(player);
    change.stopAfterMove = static_cast<i8>(stopAfterMove);
    change.direction = static_cast<i8>(direction);
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
        0,
        CURSOR_REMOTE_RELIABLE,
        CURSOR_REMOTE_NO_TIMEOUT
    );
}

// ---- globals (definitions, RVA order) ----
// @data-layout-note Retail initialized storage is 0xee020+0x228; candidate is
// 0x224. All public initializers and private literals are byte-exact. Candidate
// groups ProcessIncomingGroupMapChange's 1505 line-base word at +0 and
// bMoveSoundMade at +4, while retail places bMoveSoundMade at +0, a zero word
// at +4, and the line base at +0x1bc. Candidate +8..+0x1bc equals retail at the
// same offsets; candidate +0x1bc..+0x224 equals retail +0x1c0..+0x228. The two
// retail references from ProcessIncomingGroupMapChange load the word owner with
// addend zero, then add 7 and 25; candidate emits the same instructions and
// addends. The opt-in relocation helper calls the recovered private identity
// fake because the delinked target synthesizes const_000ee1dc; this is the same
// publics-only representational limitation as COMMAND, not a fallback identity.
// Candidate and retail .rdata are byte-exact at 0xeb078+0x8, with SHA-256
// e163f8cb0f7067a7fc78ca859a77f849aea3214f38fb75b884e4a16be725c905.
//
// Retail zero-fill is 0x124bc0+0x48; candidate is 0x40. Retail orders cycle,
// frame-count, turning, base-frame, and direction at +0..+0x14, leaves four
// bytes before sMapChangeLastFew at +0x18, and has four owner-tail bytes.
// Candidate COMMON order is frame-count +0, base-frame +4,
// sMapChangeLastFew +8, direction +0x34, cycle +0x38, turning +0x3c. All six
// public allocations have their proven types and extents. Focused audits for
// DrawCursor, DrawCursorShadow, MoveHero, ProcessIncomingGroupMapChange,
// PurgeMapChangeQueue, and SendMapChange cover 61/61, 22/22, 158/158, 9/9,
// 2/2, and 14/14 ordered relocations with only-base=0. Revisit only with a
// natural compiler allocation-order change; do not add padding, aliases,
// synthetic identities, or unattached literals.
DATA(0x004ee020) i32 bMoveSoundMade = 1;
DATA(0x004ee028) i32 giPixelsPerStep[6] = {2, 4, 6, 8, 16, 0};
DATA(0x004ee040) i32 giStepDelay[5] = {20, 25, 20, 15, 15};
DATA(0x004ee054) struct _SAMPLE* hOldWalkSample = 0;
DATA(0x004ee058) struct _SAMPLE* hNewWalkSample = 0;
DATA(0x004ee05c) i32 EveryOther = 0;
DATA(0x004ee060) i32 startVals[3] = {16, 0, -16};
DATA(0x00524bc0) i32 S1cursorCycle;
DATA(0x00524bc4) i32 S1cursorFrameCount;
DATA(0x00524bc8) i32 S1cursorTurning;
DATA(0x00524bcc) i32 S1cursorBaseFrame;
DATA(0x00524bd0) i32 S1cursorDirection;
DATA(0x00524bd8) SMapChange sMapChangeLastFew[CURSOR_MAP_CHANGE_RECENT_COUNT];

#undef RETAIL_FILE
