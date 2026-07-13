// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\ARMY.OBJ   from: (directly linked into exe)
// functions: 45   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <BASE/resourceManager.h>
#include <BASE/sample.h>
#include <BASE/font.h>
#include <BASE/Misc.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/soundManager.h>
#include <BASE/mouseManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/army.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/PATH.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/X_GLOBAL.h>

VA(0x0044a8c0, 0xcf)
army::army(void)
{
    int i;

    m_creatureIcon = 0;
    m_hex = 0;
    for (i = 0; i < ARMY_SAMPLE_COUNT; i++) {
        m_samples[i] = 0;
    }
    m_drawEnabled = 1;
    m_targetSide = -1;
    m_targetIndex = -1;
    m_attackDirection = -1;
    field_0x5e = 0;
    m_moveTargetHex = 0;
    m_palette = 0;
    m_showQuantity = 1;
    m_yOffset = 0;
    m_xOffset = 0;
}

VA(0x0044a98f, 0x18)
void army::WaitSample(int) {}

VA(0x0044a9a7, 0xde)
void army::InitClean(void)
{
    int i;

    for (i = 0; i < ARMY_SAMPLE_COUNT; i++) {
        m_samples[i] = 0;
    }
    field_0x72 = -1;
    m_spellCount = 0;
    memset(m_spellInfluence, 0, sizeof(m_spellInfluence));
    m_lastAnimationTime = KBTickCount();
    m_drawEnabled = 1;
    m_creatureIcon = 0;
    field_0x66 = 0;
    m_spellEffect = -1;
    field_0x6a = -1;
    field_0x6e = -1;
    field_0xa6 = -1;
    m_lastTargetHex = -1;
}

VA(0x0044aa85, 0x443)
void army::Init(int monsterType, int quantity, int side, int index, int hex, int unknown)
{
    hero *commander;
    int rearHex;

    InitClean();
    m_monsterType = monsterType;
    m_drawState = 1;
    m_monster = gMonsterDatabase[monsterType];
    field_0xd4 = 6;
    commander = gpCombatManager->m_heroes[side];
    if (commander) {
        m_monster.attack += commander->Stats(HERO_PRIMARY_ATTACK);
        m_monster.defense += commander->Stats(HERO_PRIMARY_DEFENSE);
    }
    m_facing = side ^ 1;
    m_animationSequence = ARMY_ANIMATION_STAND;
    m_animationFrame = 0;
    m_luckOutcome = 0;
    m_targetSide = -1;
    m_targetIndex = -1;
    m_attackDirection = -1;
    m_speed = m_monster.speed;
    m_quantity = quantity;
    m_initialQuantity = m_quantity;
    field_0x9e = 0;
    m_animationState = 0;
    m_hitPointsLost = 0;
    m_damagePenalty = 0;
    m_animationValue2 = 0;
    m_animationValue3 = m_animationValue2;
    m_animationValue1 = m_animationValue3;
    m_side = side;
    m_index = index;
    m_morale = gpCombatManager->m_armyGroups[m_side]->GetMorale(
        gpCombatManager->m_heroes[m_side],
        gpCombatManager->m_combatTowns[m_side],
        gpCombatManager->m_armyGroups[COMBAT_DEFENDER_SIDE - m_side]);
    if (m_monsterType == ARMY_CREATURE_EARTH_ELEMENTAL ||
        m_monsterType == ARMY_CREATURE_AIR_ELEMENTAL ||
        m_monsterType == ARMY_CREATURE_FIRE_ELEMENTAL ||
        m_monsterType == ARMY_CREATURE_WATER_ELEMENTAL ||
        (m_monster.flags.all & MONSTER_FLAGS_NO_MORALE)) {
        m_morale = 0;
    }
    m_luck = gpGame->GetLuck(
        gpCombatManager->m_heroes[m_side], this,
        gpCombatManager->m_combatTowns[m_side]);
    m_hex = hex;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = m_side;
    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = m_index;
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        rearHex = m_hex + (m_side == 0 ? 1 : -1);
        gpCombatManager->m_hexCells[rearHex].m_occupantSide = m_side;
        gpCombatManager->m_hexCells[rearHex].m_occupantIndex = m_index;
        if (rearHex >= m_hex) {
            gpCombatManager->m_hexCells[rearHex].m_occupantFrame = 1;
        } else {
            gpCombatManager->m_hexCells[rearHex].m_occupantFrame = 0;
        }
        if (rearHex >= m_hex) {
            gpCombatManager->m_hexCells[m_hex].m_occupantFrame = 0;
        } else {
            gpCombatManager->m_hexCells[m_hex].m_occupantFrame = 1;
        }
    }
    field_0xa6 = unknown;
}

VA(0x0044aec8, 0x4a6)
void army::LoadResources(void)
{
    int i;

    if (gbNoShowCombat) {
        return;
    }

    gpResourceManager->PointToFile(
        gpResourceManager->MakeId(cArmyFrameFileNames[m_monsterType], 1));
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(&m_frameInfo),
                                 sizeof(m_frameInfo));
    ModifyFrameInfo(&m_frameInfo, m_monsterType);
    m_walkDuration = m_frameInfo.walkDuration;

    sprintf(gText, "%smove.82M", m_monster.unknown11);
    m_samples[ARMY_SAMPLE_MOVE] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%sattk.82M", m_monster.unknown11);
    m_samples[ARMY_SAMPLE_ATTACK] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%swnce.82M", m_monster.unknown11);
    m_samples[ARMY_SAMPLE_WINCE] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%skill.82M", m_monster.unknown11);
    m_samples[ARMY_SAMPLE_KILL] = gpResourceManager->GetSample(gText);

    if (m_monster.flags.all & MONSTER_FLAGS_SHOOTER) {
        sprintf(gText, "%sshot.82M", m_monster.unknown11);
        m_samples[ARMY_SAMPLE_SHOT] = gpResourceManager->GetSample(gText);
    }
    if (m_monsterType == ARMY_CREATURE_VAMPIRE ||
        m_monsterType == ARMY_CREATURE_VAMPIRE_LORD) {
        sprintf(gText, "%sext1.82M", m_monster.unknown11);
        m_samples[ARMY_SAMPLE_EXTRA_ONE] = gpResourceManager->GetSample(gText);
        sprintf(gText, "%sext2.82M", m_monster.unknown11);
        m_samples[ARMY_SAMPLE_EXTRA_TWO] = gpResourceManager->GetSample(gText);
    } else if (m_monsterType == ARMY_CREATURE_LICH ||
               m_monsterType == ARMY_CREATURE_POWER_LICH) {
        sprintf(gText, "%sexpl.82M", m_monster.unknown11);
        m_samples[ARMY_SAMPLE_EXTRA_ONE] = gpResourceManager->GetSample(gText);
    }

    m_creatureIcon = gpResourceManager->GetIcon(cMonFilename[m_monsterType]);
    if (m_monster.flags.all & MONSTER_FLAGS_SHOOTER) {
        if (m_monsterType == ARMY_CREATURE_GIANT ||
            m_monsterType == ARMY_CREATURE_TITAN) {
            sprintf(gText, "titanmsl.icn");
        } else if (m_monsterType == ARMY_CREATURE_HALFLING) {
            sprintf(gText, "halflmsl.icn");
        } else if (m_monsterType == ARMY_CREATURE_ARCHER ||
                   m_monsterType == ARMY_CREATURE_RANGER) {
            sprintf(gText, "arch_msl.icn");
        } else if (m_monsterType == ARMY_CREATURE_LICH ||
                   m_monsterType == ARMY_CREATURE_POWER_LICH) {
            sprintf(gText, "lich_msl.icn");
        } else if (m_monsterType == ARMY_CREATURE_ORC ||
                   m_monsterType == ARMY_CREATURE_ORC_CHIEF) {
            sprintf(gText, "orc__msl.icn");
        } else if (m_monsterType == ARMY_CREATURE_DRUID ||
                   m_monsterType == ARMY_CREATURE_GREATER_DRUID) {
            sprintf(gText, "druidmsl.icn");
        } else if (m_monsterType == ARMY_CREATURE_TROLL ||
                   m_monsterType == ARMY_CREATURE_WAR_TROLL) {
            sprintf(gText, "trollmsl.icn");
        } else {
            sprintf(gText, "elf__msl.icn");
        }
        m_missileIcon = gpResourceManager->GetIcon(gText);
    } else {
        m_samples[ARMY_SAMPLE_SHOT] = 0;
        m_missileIcon = 0;
    }

    for (i = 0; i < ARMY_PRIMARY_SAMPLE_COUNT; i++) {
        if (m_samples[i]) {
            m_samples[i]->m_volume = ARMY_SAMPLE_VOLUME;
            m_samples[i]->m_channelType = ARMY_SAMPLE_CHANNEL;
            m_samples[i]->m_loopCount = 1;
        }
    }
}

VA(0x0044b36e, 0xe8)
void army::FreeResources(void)
{
    int i;

    if (gbNoShowCombat) {
        return;
    }
    if (m_missileIcon) {
        gpResourceManager->Dispose(m_missileIcon);
        m_missileIcon = 0;
    }
    if (m_creatureIcon) {
        gpResourceManager->Dispose(m_creatureIcon);
        m_creatureIcon = 0;
    }
    for (i = 0; i < ARMY_SAMPLE_COUNT; i++) {
        if (m_samples[i]) {
            gpResourceManager->Dispose(m_samples[i]);
            m_samples[i] = 0;
        }
    }
}

VA(0x0044b456, 0x7d2)
void army::DrawToBuffer(int x, int y, int effectsOnly)
{
    char quantityText[ARMY_QUANTITY_TEXT_SIZE];
    int standing;
    int color;
    int frameCount;
    int yDistance;
    int xDistance;
    unsigned char *palette;
    int quantityX;
    int neighboringHex;
    int quantityY;
    int quantityOffset;
    int goodEffects;
    int badEffects;
    int effect;
    int statusFrame;
    int drawn;
    int spellX;
    int spellY;

    if (gpCombatManager->m_nonVisualCombat || gbNoShowCombat) {
        return;
    }

    color = 0;
    standing = m_animationSequence == ARMY_ANIMATION_STAND ||
        (m_animationSequence >= ARMY_ANIMATION_STANDING_FIRST &&
         m_animationSequence < ARMY_ANIMATION_STANDING_END);
    y += m_yOffset;
    x += m_xOffset;
    if (m_animationSequence == ARMY_ANIMATION_WALK &&
        !(m_monster.flags.all & MONSTER_FLAGS_FLYING)) {
        frameCount = m_frameInfo.animationFrameCount[m_animationSequence];
        yDistance = m_animationFrame * 42 / frameCount;
        xDistance = m_animationFrame * 22 / frameCount;
        if (m_walkDirection == 0 || m_walkDirection == 5) {
            y -= yDistance;
            giWalkingYMod = -yDistance;
        }
        if (m_walkDirection == 2 || m_walkDirection == 3) {
            y += yDistance;
            giWalkingYMod = yDistance;
        }
        if (m_walkDirection == 0 || m_walkDirection == 2) {
            x -= xDistance;
        }
        if (m_walkDirection == 5 || m_walkDirection == 3) {
            x += xDistance;
        }
    }
    if (giSpellEffectShowType == 0 && standing && m_spellCount > 0) {
        color = 0xed;
    }
    if (m_hex == gpCombatManager->m_limitCreatureHex &&
        gpCombatManager->m_limitCreature == 1) {
        color = 0xec;
    }

    palette = 0;
    if (m_spellInfluence[11] == 0) {
        if (!(m_monster.flags.all & MONSTER_FLAGS_RED_PALETTE)) {
            if (!(m_monster.flags.all & MONSTER_FLAGS_DARK_BROWN_PALETTE)) {
                if (!(m_monster.flags.all & MONSTER_FLAGS_GRAY_PALETTE)) {
                    if (m_monster.flags.all & MONSTER_FLAGS_LIGHT_PALETTE) {
                        palette = gColorTableLighten;
                    }
                } else {
                    palette = gColorTableGray;
                }
            } else {
                palette = gColorTableDarkBrown;
            }
        } else {
            palette = gColorTableRed;
        }
    } else {
        palette = gColorTableGray;
    }
    if (effectsOnly == 0) {
        m_creatureIcon->CombatClipDrawToBuffer(
            x,
            y,
            m_frameInfo.animationFrames[m_animationSequence][m_animationFrame],
            &m_spriteLimits,
            m_facing == 0,
            color,
            palette,
            m_palette);
    }

    if (standing && gpCombatManager->m_showArmyQuantities && m_showQuantity) {
        if (!(m_monster.flags.all & MONSTER_FLAGS_WIDE)) {
            if (m_facing == 1) {
                quantityX = x + 9;
                neighboringHex = m_hex + 1;
            } else {
                quantityX = x - 29;
                neighboringHex = m_hex - 1;
            }
        } else if (m_facing == 1) {
            quantityX = x + 53;
            neighboringHex = m_hex + 2;
        } else {
            quantityX = x - 73;
            neighboringHex = m_hex - 2;
        }
        drawn = gpCombatManager->m_hexCells[neighboringHex].m_occupantSide != -1;
        quantityOffset = m_frameInfo.quantityX[1 - m_facing];
        if (drawn && quantityOffset > 0) {
            quantityOffset = 0;
        }
        if (m_facing != 1) {
            quantityOffset = -quantityOffset;
        }
        quantityX += quantityOffset;
        quantityY = m_facing == 1 ? y - 11 : y - 23;
        goodEffects = 0;
        badEffects = 0;
        for (effect = 0; effect < ARMY_SPELL_INFLUENCE_COUNT; effect++) {
            if (m_spellInfluence[effect]) {
                switch (effect) {
                case 0:
                case 3:
                case 8:
                case 9:
                case 10:
                case 12:
                case 13:
                case 14:
                    goodEffects++;
                    break;
                default:
                    badEffects++;
                }
            }
        }
        if (giSpellEffectShowType == 0 || !standing || m_spellCount < 1) {
            drawn = gpCombatManager->m_combatStatusIcon->CombatClipDrawToBuffer(
                quantityX, quantityY, 10, &m_creatureLimits, 0, 0, 0, 0);
        } else if (giSpellEffectShowType == 1) {
            drawn = gpCombatManager->m_combatStatusIcon->CombatClipDrawToBuffer(
                quantityX, quantityY, 11, &m_creatureLimits, 0, 0xed, 0, 0);
        } else {
            statusFrame = 2;
            if (goodEffects < 1 || badEffects < 1) {
                if (badEffects > 0) {
                    statusFrame = 4;
                }
            } else {
                statusFrame = 3;
            }
            drawn = gpCombatManager->m_combatStatusIcon->CombatClipDrawToBuffer(
                quantityX, quantityY, statusFrame + 10, &m_creatureLimits, 0, 0, 0, 0);
        }
        if (drawn) {
            sprintf(quantityText, "%d", m_lastTargetHex == -1 ? m_quantity : m_lastTargetHex);
            smallFont->DrawBoundedString(quantityText, quantityX, quantityY + 2, 20, 12, 1, 1);
        }
    }

    if (field_0x66 && effectsOnly == 0) {
        spellX = x;
        spellY = GetPowBaseY();
        if (m_animationSequence == ARMY_ANIMATION_WINCE ||
            m_animationSequence == ARMY_ANIMATION_DEATH) {
            if (m_facing == 1) {
                spellX -= 4;
            } else {
                spellX += 4;
            }
        }
        if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            if (m_facing == 1) {
                spellX += 22;
            } else {
                spellX -= 22;
            }
        }
        if (gCurLoadedSpellEffect == 23) {
            if (m_facing == 1) {
                spellX = RightX();
            } else {
                spellX = LeftX();
            }
        }
        if (gCurLoadedSpellEffect == 28) {
            spellX = ((-(m_facing == 1) & 2) - 1) * m_frameInfo.spellEffectX + x;
            spellY = m_frameInfo.spellEffectY + y;
        }
        gCurLoadedSpellIcon->CombatClipDrawToBuffer(
            spellX,
            field_0xfa + spellY,
            gCurSpellEffectFrame,
            &m_spellLimits,
            1 - m_facing,
            0,
            0,
            0);
    }
}

VA(0x0044bc28, 0x2d)
void army::Wince(void)
{
    m_animationSequence = ARMY_ANIMATION_WINCE;
    m_animationFrame = 0;
}

VA(0x0044bc55, 0xb90)
void army::Walk(int direction, int finishStanding, int skipDrawing)
{
    int destination;
    int otherHex;
    int frame;
    int oldMinX;
    int oldMinY;
    int oldMaxX;
    int oldMaxY;

    destination = GetAdjacentCellIndex(m_hex, direction);
    if (m_side == 1 && gpCombatManager->m_inCastleCombat &&
        (destination == 58 || destination == 59 ||
         (destination == 60 && m_side == 1 &&
          (m_monster.flags.all & MONSTER_FLAGS_WIDE))) &&
        gpCombatManager->m_drawbridgeState == 4) {
        m_animationSequence = ARMY_ANIMATION_STAND;
        m_animationFrame = 0;
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        gpCombatManager->LowerDoor();
        skipDrawing = 0;
    }

    giWalkingFrom = m_hex;
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        giWalkingFrom2 = ((-(m_facing == 1) & 2) - 1) + m_hex;
    } else {
        giWalkingFrom2 = -1;
    }
    giWalkingTo = destination;
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        giWalkingTo2 = ((-(m_facing == 1) & 2) - 1) + destination;
    } else {
        giWalkingTo2 = -1;
    }
    giWalkingYMod = 0;
    BuildTempWalkSeq(&m_frameInfo, finishStanding, skipDrawing);
    m_walkDirection = direction;

    if (skipDrawing == 0) {
        giMinExtentY = ARMY_COMBAT_WIDTH;
        giMinExtentX = giMinExtentY;
        giMaxExtentY = 0;
        giMaxExtentX = giMaxExtentY;
        gbComputeExtent = 1;
        gbSaveBiggestExtent = 1;
        gbReturnAfterComputeExtent = 1;
        DrawToBuffer(
            gpCombatManager->m_hexCells[m_hex].m_x,
            gpCombatManager->m_hexCells[m_hex].m_y,
            0);
        gbReturnAfterComputeExtent = 0;
        gbSaveBiggestExtent = 0;
        gbComputeExtent = 0;
    }
    if (giMinExtentX < 0) giMinExtentX = 0;
    if (giMinExtentY < 0) giMinExtentY = 0;
    if (giMaxExtentX > ARMY_COMBAT_MAX_X) giMaxExtentX = ARMY_COMBAT_MAX_X;
    if (giMaxExtentY > ARMY_COMBAT_MAX_Y) giMaxExtentY = ARMY_COMBAT_MAX_Y;
    oldMinX = giMinExtentX;
    oldMinY = giMinExtentY;
    oldMaxX = giMaxExtentX;
    oldMaxY = giMaxExtentY;

    m_facingChanged = 0;
    if (direction < 3) {
        if (m_facing == 0) {
            m_facingChanged = 1;
            m_facing = 1 - m_facing;
            if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
                m_hex--;
            }
        }
    } else if (m_facing == 1) {
        m_facingChanged = 1;
        m_facing = 1 - m_facing;
        if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            m_hex++;
        }
    }
    if (direction == 0 || direction == 5) {
        m_drawState = 0;
    }
    if (direction == 2 || direction == 3) {
        m_drawState = 3;
    }
    m_animationFrame = 0;
    m_animationSequence = ARMY_ANIMATION_WALK;
    if (!gbNoShowCombat) {
        gpSoundManager->MemorySample(m_samples[ARMY_SAMPLE_MOVE]);
    }

    if (skipDrawing == 0) {
        gpCombatManager->m_hexCells[m_hex].m_occupantSide = -1;
        gpCombatManager->DrawFrame(0, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        gpCombatManager->m_hexCells[m_hex].m_occupantSide =
            gpCombatManager->m_currentArmySide;
        if (!gbNoShowCombat) {
            gpWindowManager->m_screen->CopyTo(
                gpCombatManager->m_backgroundBuffer, 0, 0, 0, 0,
                ARMY_COMBAT_WIDTH, ARMY_COMBAT_HEIGHT);
        }
        gpCombatManager->m_backgroundDrawn = 0;
    }

    if (!gbNoShowCombat) {
        for (frame = 0;
             frame < m_frameInfo.animationFrameCount[ARMY_ANIMATION_WALK];
             frame++) {
            m_animationFrame = frame;
            if (skipDrawing || frame != 0) {
                gpCombatManager->m_backgroundBuffer->CopyTo(
                    gpWindowManager->m_screen,
                    giMinExtentX,
                    giMinExtentY,
                    giMinExtentX,
                    giMinExtentY,
                    giMaxExtentX - giMinExtentX + 1,
                    giMaxExtentY - giMinExtentY + 1);
                if (giMinExtentX < 0) giMinExtentX = 0;
                if (giMinExtentY < 0) giMinExtentY = 0;
                if (giMaxExtentX > ARMY_COMBAT_MAX_X)
                    giMaxExtentX = ARMY_COMBAT_MAX_X;
                if (giMaxExtentY > ARMY_COMBAT_MAX_Y)
                    giMaxExtentY = ARMY_COMBAT_MAX_Y;
                oldMinX = giMinExtentX;
                oldMinY = giMinExtentY;
                oldMaxX = giMaxExtentX;
                oldMaxY = giMaxExtentY;
            }
            giMinExtentY = ARMY_COMBAT_WIDTH;
            giMinExtentX = giMinExtentY;
            giMaxExtentY = 0;
            giMaxExtentX = giMaxExtentY;
            gbComputeExtent = 1;
            gbSaveBiggestExtent = 1;
            gbReturnAfterComputeExtent = 1;
            DrawToBuffer(
                gpCombatManager->m_hexCells[m_hex].m_x,
                gpCombatManager->m_hexCells[m_hex].m_y,
                0);
            gbReturnAfterComputeExtent = 0;
            gbComputeExtent = 0;
            gbSaveBiggestExtent = 0;
            if (giMinExtentX < 0) giMinExtentX = 0;
            if (giMinExtentY < 0) giMinExtentY = 0;
            if (giMaxExtentX > ARMY_COMBAT_MAX_X)
                giMaxExtentX = ARMY_COMBAT_MAX_X;
            if (giMaxExtentY > ARMY_COMBAT_MAX_Y)
                giMaxExtentY = ARMY_COMBAT_MAX_Y;
            gbCurrArmyDrawn = 0;
            gbComputeExtent = 1;
            gbLimitToExtent = 1;
            m_drawEnabled = 0;
            gpCombatManager->DrawFrame(0, 0, 0, 0,
                                       ARMY_COMBAT_FRAME_DELAY, 0, 1);
            m_drawEnabled = 1;
            gbLimitToExtent = 0;
            gbComputeExtent = 0;
            gbCurrArmyDrawn = 1;
            if (giMinExtentX < oldMinX) oldMinX = giMinExtentX;
            if (giMinExtentY < oldMinY) oldMinY = giMinExtentY;
            if (giMaxExtentX > oldMaxX) oldMaxX = giMaxExtentX;
            if (giMaxExtentY > oldMaxY) oldMaxY = giMaxExtentY;
            DelayTil(glTimers);
            glTimers[0] = static_cast<int>(
                KBTickCount() +
                m_walkDuration * gfCombatSpeedMod[gConfig.combatSpeed] /
                    m_frameInfo.animationFrameCount[ARMY_ANIMATION_WALK]);
            gpWindowManager->UpdateScreenRegion(
                oldMinX, oldMinY, oldMaxX - oldMinX + 1, oldMaxY - oldMinY + 1);
        }
    }

    destination = GetAdjacentCellIndex(m_hex, direction);
    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantFrame = -1;
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        otherHex = ((-(m_facing == 1) & 2) - 1) + m_hex;
        gpCombatManager->m_hexCells[otherHex].m_occupantIndex = -1;
        gpCombatManager->m_hexCells[otherHex].m_occupantSide = -1;
        gpCombatManager->m_hexCells[otherHex].m_occupantFrame = -1;
    }
    gpCombatManager->m_hexCells[destination].m_occupantSide = m_side;
    gpCombatManager->m_hexCells[destination].m_occupantIndex = m_index;
    gpCombatManager->m_hexCells[destination].m_occupantFrame = -1;
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        otherHex = ((-(m_facing == 1) & 2) - 1) + destination;
        gpCombatManager->m_hexCells[otherHex].m_occupantSide = m_side;
        gpCombatManager->m_hexCells[otherHex].m_occupantIndex = m_index;
        if (otherHex < destination) {
            gpCombatManager->m_hexCells[otherHex].m_occupantFrame = 0;
        } else {
            gpCombatManager->m_hexCells[otherHex].m_occupantFrame = 1;
        }
        if (destination < otherHex) {
            gpCombatManager->m_hexCells[destination].m_occupantFrame = 0;
        } else {
            gpCombatManager->m_hexCells[destination].m_occupantFrame = 1;
        }
    }
    m_hex = destination;
    if (m_facingChanged) {
        m_facing = 1 - m_facing;
        if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            if (m_facing == 0) {
                m_hex++;
            } else {
                m_hex--;
            }
        }
        m_facingChanged = 0;
    }
    giWalkingFrom = -1;
    giWalkingFrom2 = -1;
    giWalkingTo = -1;
    giWalkingTo2 = -1;
    m_drawState = 1;
    if (finishStanding == 1) {
        m_animationSequence = ARMY_ANIMATION_STAND;
        m_animationFrame = 0;
        gpCombatManager->DrawFrame(1, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }
}

VA(0x0044c7e5, 0x14aa)
void army::SpecialAttack(void)
{
    army *target;
    army *splashTarget;
    bitmap *missileBackground;
    char combatText[ARMY_COMBAT_TEXT_SIZE];
    int damage;
    int killed;
    int originalFacing;
    int targetColumn;
    int sourceColumn;
    int targetX;
    int targetY;
    int sourceX;
    int sourceY;
    int xDistance;
    int yDistance;
    int reverseMissile;
    int directionFrame;
    int attackDirection;
    int direction;
    int frame;
    int missileHalfWidth;
    int missileHalfHeight;
    int missileSpacing;
    int missileX;
    int missileY;
    int endX;
    int endY;
    int distance;
    int missileSteps;
    int xStep;
    int yStep;
    int oldX;
    int oldY;
    int backgroundX;
    int backgroundY;
    int minX;
    int minY;
    int maxX;
    int maxY;
    int adjacentHex;
    int originalAttack;
    int effectType;
    int effectX;
    int effectY;
    float angle;

    damage = 0;
    killed = 0;
    m_palette = 0;
    target = &gpCombatManager->m_armies[m_targetSide][m_targetIndex];
    targetColumn = target->m_hex % 13;
    sourceColumn = m_hex % 13;
    originalFacing = m_facing;
    if (sourceColumn < targetColumn ||
        (((m_hex / 13) & 1) == 0 && sourceColumn == targetColumn)) {
        m_facing = 1;
    } else {
        m_facing = 0;
    }
    if (m_facing != originalFacing) {
        if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            if (m_facing == 1) {
                m_hex--;
            } else {
                m_hex++;
            }
        }
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }

    CheckLuck();
    gpSoundManager->MemorySample(m_samples[ARMY_SAMPLE_SHOT]);
    gpCombatManager->ResetLimitCreature();
    gpCombatManager->m_limitCreatureCount[m_side][m_index]++;
    gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);

    targetX = target->MidX();
    targetY = target->MidY();
    if (m_monsterType == ARMY_CREATURE_LICH ||
        m_monsterType == ARMY_CREATURE_POWER_LICH) {
        targetX = gpCombatManager->m_hexCells[target->m_hex].m_x;
        targetY = gpCombatManager->m_hexCells[target->m_hex].m_y - 17;
    }
    if (m_facing == 1) {
        sourceX = gpCombatManager->m_hexCells[m_hex].m_x +
            m_frameInfo.missileOffsets[1].x;
    } else {
        sourceX = gpCombatManager->m_hexCells[m_hex].m_x -
            m_frameInfo.missileOffsets[1].x;
    }
    sourceY = gpCombatManager->m_hexCells[m_hex].m_y +
        m_frameInfo.missileOffsets[1].y;
    xDistance = targetX - sourceX;
    reverseMissile = xDistance < 0;
    if (reverseMissile) {
        xDistance = -xDistance;
    }
    yDistance = targetY - sourceY;
    if (xDistance == 0) {
        if (yDistance < 1) {
            directionFrame = 0;
        } else {
            directionFrame = m_frameInfo.projectileDirectionCount - 1;
        }
        angle = yDistance < 1 ? 90.0f : -90.0f;
    } else {
        angle = static_cast<float>(atan(static_cast<double>(-yDistance) / xDistance) *
            180.0 / 3.14159265358979323846);
        for (direction = 1;
             direction < m_frameInfo.projectileDirectionCount;
             direction++) {
            if ((m_frameInfo.projectileAngles[direction - 1] +
                 m_frameInfo.projectileAngles[direction]) / 2.0f < angle) {
                break;
            }
        }
        if (direction < m_frameInfo.projectileDirectionCount) {
            directionFrame = direction - 1;
        } else {
            directionFrame = m_frameInfo.projectileDirectionCount - 1;
        }
    }
    if (angle <= -45.0f) {
        m_animationSequence = ARMY_ANIMATION_SHOOT_DOWN;
        attackDirection = 2;
    } else if (angle <= 45.0f) {
        m_animationSequence = ARMY_ANIMATION_SHOOT_FORWARD;
        attackDirection = 1;
    } else {
        m_animationSequence = ARMY_ANIMATION_SHOOT_UP;
        attackDirection = 0;
    }
    m_animationFrame = 0;
    while (m_animationFrame < m_frameInfo.animationFrameCount[m_animationSequence]) {
        if (m_animationFrame ==
            m_frameInfo.animationFrameCount[m_animationSequence] - 1) {
            gpCombatManager->DrawFrame(0, 1, 0, 0,
                                       ARMY_COMBAT_FRAME_DELAY, 1, 1);
        } else {
            gpCombatManager->DrawFrame(1, 1, 0, 0,
                                       ARMY_COMBAT_FRAME_DELAY, 1, 1);
        }
        glTimers[0] = static_cast<int>(
            KBTickCount() + m_frameInfo.attackDuration *
            gfCombatSpeedMod[gConfig.combatSpeed] /
            m_frameInfo.animationFrameCount[m_animationSequence]);
        m_animationFrame++;
    }
    m_animationFrame = m_frameInfo.animationFrameCount[m_animationSequence] - 1;

    missileHalfWidth = 25;
    missileHalfHeight = 25;
    missileSpacing = 31;
    if (m_monsterType == ARMY_CREATURE_LICH ||
        m_monsterType == ARMY_CREATURE_POWER_LICH) {
        missileSpacing = 26;
        directionFrame = 7;
        missileHalfWidth = 10;
        missileHalfHeight = 10;
    }
    if (m_facing == 1) {
        missileX = gpCombatManager->m_hexCells[m_hex].m_x +
            m_frameInfo.missileOffsets[attackDirection].x;
    } else {
        missileX = gpCombatManager->m_hexCells[m_hex].m_x -
            m_frameInfo.missileOffsets[attackDirection].x;
    }
    missileY = gpCombatManager->m_hexCells[m_hex].m_y +
        m_frameInfo.missileOffsets[attackDirection].y;
    endX = target->MidX();
    endY = target->MidY();
    xDistance = endX - missileX;
    yDistance = endY - missileY;
    distance = static_cast<int>(sqrt(static_cast<double>(
        xDistance * xDistance + yDistance * yDistance)));
    missileSteps = (distance + (missileSpacing >> 1)) / missileSpacing;

    if (m_monsterType == ARMY_CREATURE_MAGE ||
        m_monsterType == ARMY_CREATURE_ARCHMAGE) {
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1);
        DelayMilli(static_cast<long>(gfCombatSpeedMod[gConfig.combatSpeed] *
                                     ARMY_COMBAT_FRAME_DELAY));
        gpCombatManager->DoBolt(
            1, missileX, missileY, endX, endY, 0, 0, 5, 4, 302, 0, 0,
            distance / 15 + 15, 1, 0, 10, 0);
    } else {
        if (missileSteps < 2) {
            xStep = xDistance;
            yStep = yDistance;
        } else {
            xStep = xDistance / (missileSteps - 1);
            yStep = yDistance / (missileSteps - 1);
        }
        oldX = missileX;
        oldY = missileY;
        missileBackground = new bitmap(
            33,
            static_cast<short>(missileHalfWidth * 2),
            static_cast<short>(missileHalfHeight * 2));
        missileBackground->GrabBitmapCareful(
            gpWindowManager->m_screen,
            static_cast<short>(missileX - missileHalfWidth),
            static_cast<short>(missileY - missileHalfHeight));
        backgroundX = missileX;
        backgroundY = missileY;
        for (frame = 0; frame < missileSteps; frame++) {
            minX = missileX - missileHalfWidth;
            if (minX < 0) minX = 0;
            maxX = missileX + missileHalfWidth;
            if (maxX > ARMY_COMBAT_MAX_X) maxX = ARMY_COMBAT_MAX_X;
            minY = missileY - missileHalfHeight;
            if (minY < 0) minY = 0;
            maxY = missileY + missileHalfHeight;
            if (maxY > ARMY_COMBAT_MAX_Y) maxY = ARMY_COMBAT_MAX_Y;
            if (frame == 0) {
                if (minX < giMinExtentX) giMinExtentX = minX;
                if (maxX > giMaxExtentX) giMaxExtentX = maxX;
                if (minY < giMinExtentY) giMinExtentY = minY;
                if (maxY > giMaxExtentY) giMaxExtentY = maxY;
            } else {
                missileBackground->DrawToBufferCareful(
                    static_cast<short>(backgroundX), static_cast<short>(backgroundY));
            }
            backgroundX = oldX - missileHalfWidth;
            if (backgroundX < 0) backgroundX = 0;
            if (backgroundX + missileBackground->m_width > ARMY_COMBAT_WIDTH) {
                backgroundX = ARMY_COMBAT_WIDTH - missileBackground->m_width;
            }
            backgroundY = oldY - missileHalfHeight;
            if (backgroundY < 0) backgroundY = 0;
            if (backgroundY + missileBackground->m_height > ARMY_COMBAT_WIDTH) {
                backgroundY = ARMY_COMBAT_WIDTH - missileBackground->m_height;
            }
            missileBackground->GrabBitmapCareful(
                gpWindowManager->m_screen,
                static_cast<short>(backgroundX),
                static_cast<short>(backgroundY));
            m_missileIcon->DrawToBuffer(
                missileX, missileY, directionFrame, reverseMissile);
            if (frame == 0) {
                gpWindowManager->UpdateScreenRegion(
                    giMinExtentX,
                    giMinExtentY,
                    giMaxExtentX - giMinExtentX + 1,
                    giMaxExtentY - giMinExtentY + 1);
            } else {
                DelayTil(glTimers);
                gpWindowManager->UpdateScreenRegion(
                    minX, minY, maxX - minX + 1, maxY - minY + 1);
            }
            glTimers[0] = static_cast<int>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * 15);
            oldX = missileX;
            oldY = missileY;
            missileX += xStep;
            missileY += yStep;
        }
        missileBackground->DrawToBuffer(
            static_cast<short>(backgroundX), static_cast<short>(backgroundY));
        gpWindowManager->UpdateScreenRegion(
            oldX - missileHalfWidth,
            oldY - missileHalfHeight,
            missileHalfWidth * 2,
            missileHalfHeight * 2);
        delete missileBackground;
        if (!gpCombatManager->m_heroes[m_side] ||
            !gpCombatManager->m_heroes[m_side]->HasArtifact(
                ARMY_ENDLESS_AMMUNITION_ARTIFACT)) {
            m_monster.shots--;
        }
    }

    originalAttack = m_monster.attack;
    effectType = -1;
    effectX = -1;
    effectY = -1;
    if (m_monsterType == ARMY_CREATURE_LICH ||
        m_monsterType == ARMY_CREATURE_POWER_LICH) {
        gpCombatManager->ClearEffects();
        for (direction = 0; direction < 7; direction++) {
            if (direction < 6) {
                adjacentHex = target->GetAdjacentCellIndex(target->m_hex, direction);
            } else {
                adjacentHex = target->m_hex;
            }
            if (adjacentHex != -1 &&
                gpCombatManager->m_hexCells[adjacentHex].m_occupantSide != -1) {
                splashTarget = &gpCombatManager->m_armies
                    [gpCombatManager->m_hexCells[adjacentHex].m_occupantSide]
                    [gpCombatManager->m_hexCells[adjacentHex].m_occupantIndex];
                if (!gArmyEffected[splashTarget->m_side][splashTarget->m_index] &&
                    (target != splashTarget || direction == 6)) {
                    gArmyEffected[splashTarget->m_side][splashTarget->m_index] = 1;
                    DamageEnemy(splashTarget, &damage, &killed, 1, 0);
                }
            }
        }
        field_0xfa = 0;
        effectType = ARMY_LICH_EXPLOSION_EFFECT;
        effectX = gpCombatManager->m_hexCells[adjacentHex].m_x;
        effectY = gpCombatManager->m_hexCells[adjacentHex].m_y - 17;
        gpSoundManager->MemorySample(m_samples[ARMY_SAMPLE_EXTRA_ONE]);
    } else {
        DamageEnemy(target, &damage, &killed, 1, 0);
    }
    m_monster.attack = originalAttack;

    if (killed < 1) {
        sprintf(gText, "%s %s %d %s.",
            m_quantity < 2 ? gArmyNames[m_monsterType] : gArmyNamesPlural[m_monsterType],
            m_quantity < 2 ? "does" : "do",
            damage,
            "damage");
        gText[0] -= 32;
    } else if (damage == -1) {
        sprintf(gText, "The mirror image is destroyed!");
    } else {
        sprintf(gText, "%s %s %d damage, %d %s %s.",
            m_quantity < 2 ? gArmyNames[m_monsterType] : gArmyNamesPlural[m_monsterType],
            m_quantity < 2 ? "does" : "do",
            damage,
            killed,
            killed < 2 ? gArmyNames[target->m_monsterType] :
                         gArmyNamesPlural[target->m_monsterType],
            killed < 2 ? "perishes" : "perish");
        gText[0] -= 32;
    }
    strcpy(combatText, gText);
    if (m_monsterType == ARMY_CREATURE_ARCHMAGE &&
        SRandom(1, ARMY_PERCENT_MAX) < ARMY_ARCHMAGE_DISPEL_CHANCE && target &&
        target->SpellCastWorks(102)) {
        target->m_spellEffect = 102;
    }
    PowEffect(effectType, 0, effectX, effectY);
    gpCombatManager->CombatMessage(combatText, 1, 1, 0);
    WaitSample(ARMY_SAMPLE_SHOT);

    if (m_facing != originalFacing) {
        if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            if (m_facing == 1) {
                m_hex++;
            } else {
                m_hex--;
            }
        }
        m_facing = originalFacing;
    }
    if (!bSecondAttack &&
        (m_monsterType == ARMY_CREATURE_ELF ||
         m_monsterType == ARMY_CREATURE_GRAND_ELF ||
         m_monsterType == ARMY_CREATURE_RANGER) &&
        target->m_quantity > 0) {
        bSecondAttack = 1;
        SpecialAttack();
        bSecondAttack = 0;
    }
    if (m_spellInfluence[ARMY_SPELL_INFLUENCE_ATTACK_TARGETING] ||
        m_spellInfluence[ARMY_SPELL_INFLUENCE_ATTACK_EXPIRING]) {
        CancelSpellType(1);
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }
}

VA(0x0044dc8f, 0x2b)
void army::DirDoAttack(int direction)
{
    m_attackDirection = direction;
    DoAttack(0);
}

VA(0x0044dcba, 0x4e7)
void army::DoHydraAttack(int)
{
    char combatText[200];
    short attackMask;
    army *target;
    int totalDamage;
    int totalKilled;
    int damage;
    int killed;
    int targetSide;
    int targetIndex;
    int targetHex;
    int direction;

    totalKilled = 0;
    totalDamage = totalKilled;
    gpCombatManager->ResetHitByCreature();
    if (m_spellInfluence[ARMY_SPELL_INFLUENCE_ATTACK_TARGETING]) {
        attackMask = static_cast<short>(GetAttackMask(m_hex, 2, -1));
    } else {
        attackMask = static_cast<short>(GetAttackMask(m_hex, 1, -1));
    }
    CheckLuck();
    gpCombatManager->ResetLimitCreature();
    gpCombatManager->m_limitCreatureCount[m_side][m_index]++;
    for (direction = 0; direction < ARMY_COMBAT_DIRECTION_COUNT; direction++) {
        if (!(attackMask & (1 << direction))) {
            targetHex = m_hex;
            if ((m_monster.flags.all & MONSTER_FLAGS_WIDE) &&
                ((m_facing == 0 && direction > 2) ||
                 (m_facing == 1 && (direction < 3 || direction > 5)))) {
                if (m_facing == 0) {
                    targetHex = m_hex - 1;
                } else {
                    targetHex = m_hex + 1;
                }
            }
            targetHex = GetAdjacentCellIndex(targetHex, direction);
            if (ValidHex(targetHex)) {
                targetSide = gpCombatManager->m_hexCells[targetHex].m_occupantSide;
                targetIndex = gpCombatManager->m_hexCells[targetHex].m_occupantIndex;
                if (targetSide >= 0 && targetIndex >= 0) {
                    gpCombatManager->m_limitCreatureCount[targetSide][targetIndex]++;
                    target = &gpCombatManager->m_armies[targetSide][targetIndex];
                    if (!target->m_hitByCreature) {
                        target->m_hitByCreature = 1;
                        DamageEnemy(target, &damage, &killed, 0, 0);
                        totalDamage += damage;
                        totalKilled += killed;
                        gpCombatManager->m_limitCreatureCount[targetSide][targetIndex]++;
                    }
                }
            }
        }
    }
    gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    m_animationState = 1;
    m_animationDelay = ARMY_ATTACK_DELAY_NORMAL;
    gpSoundManager->MemorySample(m_samples[ARMY_SAMPLE_ATTACK]);
    if (totalKilled > 0) {
        sprintf(gText, "%s %s %d %s, %d %s %s.",
            m_quantity > 1 ? gArmyNamesPlural[m_monsterType] : gArmyNames[m_monsterType],
            m_quantity > 1 ? "do" : "does",
            totalDamage,
            "damage",
            totalKilled,
            totalKilled > 1 ? "creatures" : "creature",
            totalKilled > 1 ? "perish" : "perishes");
    } else {
        sprintf(gText, "%s %s %d %s.",
            m_quantity > 1 ? gArmyNamesPlural[m_monsterType] : gArmyNames[m_monsterType],
            m_quantity > 1 ? "do" : "does",
            totalDamage,
            "damage");
    }
    gText[0] -= ARMY_ASCII_CASE_OFFSET;
    strcpy(combatText, gText);
    PowEffect(-1, 0, -1, -1);
    gpCombatManager->CombatMessage(combatText, 1, 1, 0);
    gpCombatManager->m_limitCreatureCount[m_side][m_index] = 1;
}

VA(0x0044e1a1, 0x1267)
void army::DoAttack(int retaliation)
{
    char combatText[ARMY_COMBAT_TEXT_SIZE];
    army *target;
    army *breathTarget;
    int damage;
    int killed;
    int breathDamage;
    int breathKilled;
    int targetHex;
    int breathHex;
    int originalFacing;
    int targetOriginalFacing;
    int desiredFacing;
    int originalDirection;
    int effectStopsRetaliation;
    int occupantSide;

    m_drawState = 3;
    damage = 0;
    killed = 0;
    breathDamage = 0;
    breathKilled = 0;
    target = 0;
    breathTarget = 0;
    effectStopsRetaliation = 0;
    originalFacing = m_facing;
    if (retaliation) {
        gpCombatManager->m_currentSide = 1 - gpCombatManager->m_currentSide;
    }
    if (m_monsterType == ARMY_CREATURE_HYDRA) {
        DoHydraAttack(retaliation);
    } else {
        originalDirection = m_attackDirection;
        targetHex = m_hex;
        if ((m_monster.flags.all & MONSTER_FLAGS_WIDE) &&
            ((m_facing == 0 && m_attackDirection > 2) ||
             (m_facing == 1 &&
              (m_attackDirection < 3 || m_attackDirection > 5)))) {
            if (originalFacing == 0) {
                targetHex = m_hex - 1;
            } else {
                targetHex = m_hex + 1;
            }
        }
        targetHex = GetAdjacentCellIndex(targetHex, m_attackDirection);
        target = &gpCombatManager->m_armies
            [gpCombatManager->m_hexCells[targetHex].m_occupantSide]
            [gpCombatManager->m_hexCells[targetHex].m_occupantIndex];
        if (m_monster.flags.all & MONSTER_FLAGS_BREATH_ATTACK) {
            breathHex = GetAdjacentCellIndex(targetHex, m_attackDirection);
            if (ValidHex(breathHex) &&
                gpCombatManager->m_hexCells[breathHex].m_occupantSide >= 0 &&
                gpCombatManager->m_hexCells[breathHex].m_occupantIndex >= 0 &&
                (gpCombatManager->m_hexCells[breathHex].m_occupantSide != target->m_side ||
                 gpCombatManager->m_hexCells[breathHex].m_occupantIndex != target->m_index)) {
                breathTarget = &gpCombatManager->m_armies
                    [gpCombatManager->m_hexCells[breathHex].m_occupantSide]
                    [gpCombatManager->m_hexCells[breathHex].m_occupantIndex];
            }
        }
        gpCombatManager->ResetLimitCreature();
        gpCombatManager->m_limitCreatureCount[m_side][m_index]++;
        gpCombatManager->m_limitCreatureCount[target->m_side][target->m_index]++;
        if (breathTarget) {
            gpCombatManager->m_limitCreatureCount
                [breathTarget->m_side][breathTarget->m_index]++;
        }
        gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        targetOriginalFacing = target->m_facing;
        if (m_attackDirection < 3) {
            desiredFacing = 1;
        } else if (m_attackDirection < 6) {
            desiredFacing = 0;
        } else {
            desiredFacing = m_facing;
        }
        if (m_facing != desiredFacing) {
            m_facing = desiredFacing;
            if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
                if (desiredFacing == 1) {
                    m_hex--;
                } else {
                    m_hex++;
                }
            }
            target->m_facing = 1 - m_facing;
            if (target->m_facing != targetOriginalFacing &&
                (target->m_monster.flags.all & MONSTER_FLAGS_WIDE)) {
                if (target->m_facing == 1) {
                    target->m_hex--;
                } else {
                    target->m_hex++;
                }
            }
        }
        CheckLuck();
        m_animationState = 1;
        if (m_attackDirection == 6 || m_attackDirection == 5 ||
            m_attackDirection == 0) {
            m_animationDelay = ARMY_ATTACK_DELAY_SHORT;
        } else if (m_attackDirection == 1 || m_attackDirection == 4) {
            m_animationDelay = ARMY_ATTACK_DELAY_NORMAL;
        } else {
            m_animationDelay = ARMY_ATTACK_DELAY_LONG;
        }
        if (breathTarget) {
            m_animationDelay += ARMY_BREATH_ATTACK_DELAY_BONUS;
        }
        gpSoundManager->MemorySample(m_samples[ARMY_SAMPLE_ATTACK]);
        DamageEnemy(target, &damage, &killed, 0, 0);
        if (breathTarget) {
            DamageEnemy(breathTarget, &breathDamage, &breathKilled, 0, 0);
        }
        if (damage == -1) {
            sprintf(gText, "The mirror image is destroyed!");
        } else if (!gbGenieHalf) {
            if (killed < 1) {
                sprintf(gText, "%s %s %d %s.",
                    m_quantity < 2 ? gArmyNames[m_monsterType] :
                                     gArmyNamesPlural[m_monsterType],
                    m_quantity < 2 ? "does" : "do", damage, "damage");
                gText[0] -= ARMY_ASCII_CASE_OFFSET;
            } else {
                sprintf(gText, "%s %s %d %s, %d %s %s.",
                    m_quantity > 1 ? gArmyNamesPlural[m_monsterType] :
                                     gArmyNames[m_monsterType],
                    m_quantity > 1 ? "do" : "does",
                    damage,
                    "damage",
                    killed,
                    killed > 1 ? gArmyNamesPlural[target->m_monsterType] :
                                 gArmyNames[target->m_monsterType],
                    killed > 1 ? "perish" : "perishes");
                gText[0] -= ARMY_ASCII_CASE_OFFSET;
            }
        } else {
            sprintf(gText, "%s %s half the enemy troops!",
                m_quantity < 2 ? gArmyNames[m_monsterType] :
                                 gArmyNamesPlural[m_monsterType],
                m_quantity < 2 ? "destroys" : "destroy");
            gText[0] -= ARMY_ASCII_CASE_OFFSET;
        }
        strcpy(combatText, gText);

        switch (m_monsterType) {
        case ARMY_CREATURE_CYCLOPS:
            if (target->m_quantity > 0 && (!breathTarget || breathTarget->m_quantity > 0)) {
                if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE) {
                    if (target && target->SpellCastWorks(ARMY_ATTACK_SPELL_CYCLOPS)) {
                        target->m_spellEffect = ARMY_ATTACK_SPELL_CYCLOPS;
                        effectStopsRetaliation = 1;
                    }
                } else if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE &&
                           breathTarget &&
                           breathTarget->SpellCastWorks(ARMY_ATTACK_SPELL_CYCLOPS)) {
                    breathTarget->m_spellEffect = ARMY_ATTACK_SPELL_CYCLOPS;
                }
            }
            break;
        case ARMY_CREATURE_UNICORN:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target &&
                target->SpellCastWorks(ARMY_ATTACK_SPELL_UNICORN)) {
                target->m_spellEffect = ARMY_ATTACK_SPELL_UNICORN;
                effectStopsRetaliation = 1;
            }
            break;
        case ARMY_CREATURE_ARCHMAGE:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target &&
                target->SpellCastWorks(ARMY_ATTACK_SPELL_ARCHMAGE)) {
                target->m_spellEffect = ARMY_ATTACK_SPELL_ARCHMAGE;
            }
            break;
        case ARMY_CREATURE_MUMMY:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target &&
                target->SpellCastWorks(ARMY_ATTACK_SPELL_MUMMY)) {
                target->m_spellEffect = ARMY_ATTACK_SPELL_MUMMY;
            }
            break;
        case ARMY_CREATURE_ROYAL_MUMMY:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ROYAL_MUMMY_EFFECT_CHANCE && target &&
                target->SpellCastWorks(ARMY_ATTACK_SPELL_MUMMY)) {
                target->m_spellEffect = ARMY_ATTACK_SPELL_MUMMY;
            }
            break;
        case ARMY_CREATURE_VAMPIRE_LORD:
            occupantSide = gpCombatManager->m_hexCells[m_hex].m_occupantSide;
            gpCombatManager->m_killBenefit[occupantSide] =
                target->m_monster.hitPoints * killed;
            break;
        case ARMY_CREATURE_GHOST:
            occupantSide = gpCombatManager->m_hexCells[m_hex].m_occupantSide;
            gpCombatManager->m_killBenefit[occupantSide] = killed;
            break;
        case ARMY_CREATURE_MEDUSA:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target &&
                target->SpellCastWorks(ARMY_ATTACK_SPELL_MEDUSA)) {
                target->m_spellEffect = ARMY_ATTACK_SPELL_MEDUSA;
                effectStopsRetaliation = 1;
            }
            break;
        }
        PowEffect(-1, 0, -1, -1);
        gpCombatManager->CombatMessage(combatText, 1, 1, 0);
        gpCombatManager->m_limitCreatureCount[m_side][m_index] = 1;

        occupantSide = gpCombatManager->m_hexCells[m_hex].m_occupantSide;
        if (m_monsterType == ARMY_CREATURE_GHOST) {
            m_quantity += gpCombatManager->m_killBenefit[occupantSide];
        }
        if (m_monsterType == ARMY_CREATURE_VAMPIRE_LORD) {
            if (gpCombatManager->m_killBenefit[occupantSide] < m_hitPointsLost) {
                m_hitPointsLost -= gpCombatManager->m_killBenefit[occupantSide];
            } else {
                gpCombatManager->m_killBenefit[occupantSide] -= m_hitPointsLost;
                m_hitPointsLost = 0;
                if (gpCombatManager->m_killBenefit[occupantSide] /
                        m_monster.hitPoints <
                    m_initialQuantity - m_quantity) {
                    m_quantity += gpCombatManager->m_killBenefit[occupantSide] /
                        m_monster.hitPoints;
                } else {
                    m_quantity = m_initialQuantity;
                }
            }
        }

        if (target && target->m_quantity > 0 &&
            !target->m_spellInfluence[ARMY_SPELL_INFLUENCE_NO_RETALIATION] &&
            !target->m_spellInfluence[ARMY_SPELL_INFLUENCE_PETRIFIED] &&
            (target->m_monsterType == ARMY_CREATURE_GRIFFIN ||
             !(target->m_monster.flags.all & MONSTER_FLAGS_RETALIATED)) &&
            m_monsterType != ARMY_CREATURE_ROGUE &&
            m_monsterType != ARMY_CREATURE_SPRITE &&
            m_monsterType != ARMY_CREATURE_VAMPIRE &&
            m_monsterType != ARMY_CREATURE_VAMPIRE_LORD &&
            !effectStopsRetaliation && !retaliation) {
            DelayMilli(static_cast<long>(
                gfCombatSpeedMod[gConfig.combatSpeed] * ARMY_RETALIATION_DELAY));
            target->m_attackDirection = OppositeDirection(m_attackDirection);
            if (target->m_monster.flags.all & MONSTER_FLAGS_WIDE) {
                if (GetAdjacentCellIndex(
                        target->m_hex, ((target->m_facing == 1) - 1) & 5) == m_hex) {
                    target->m_attackDirection = 6;
                }
                if (GetAdjacentCellIndex(
                        target->m_hex, 3 - (target->m_facing == 1)) == m_hex) {
                    target->m_attackDirection = 7;
                }
            }
            target->DoAttack(1);
            target->m_monster.flags.all |= MONSTER_FLAGS_RETALIATED;
            if (gbRemoteOn && gpCombatManager->m_networkArmyPresent[0] &&
                gpCombatManager->m_networkArmyPresent[1] &&
                target->m_monsterType == ARMY_CREATURE_GHOST) {
                occupantSide = gpCombatManager->m_hexCells[target->m_hex].m_occupantSide;
                target->m_quantity += gpCombatManager->m_killBenefit[occupantSide];
            }
        }
        if ((m_monsterType == ARMY_CREATURE_WOLF ||
             m_monsterType == ARMY_CREATURE_PALADIN ||
             m_monsterType == ARMY_CREATURE_CRUSADER) &&
            target && target->m_quantity > 0 && !retaliation &&
            !m_spellInfluence[ARMY_SPELL_INFLUENCE_NO_RETALIATION] &&
            !m_spellInfluence[ARMY_SPELL_INFLUENCE_PETRIFIED] &&
            !m_spellInfluence[ARMY_SPELL_INFLUENCE_DOUBLE_ATTACK_BLOCK] &&
            m_quantity > 0) {
            DelayMilli(static_cast<long>(
                gfCombatSpeedMod[gConfig.combatSpeed] * ARMY_SECOND_ATTACK_DELAY));
            desiredFacing = m_attackDirection;
            m_attackDirection = originalDirection;
            DoAttack(1);
            m_attackDirection = desiredFacing;
        }
        if (m_facing != originalFacing) {
            if (!(m_monster.flags.all & MONSTER_FLAGS_FIXED_FACING)) {
                m_facing = originalFacing;
                if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
                    if (originalFacing == 1) {
                        m_hex--;
                    } else {
                        m_hex++;
                    }
                }
            }
            if (!(target->m_monster.flags.all & MONSTER_FLAGS_FIXED_FACING) &&
                target->m_facing != targetOriginalFacing) {
                target->m_facing = targetOriginalFacing;
                if (target->m_monster.flags.all & MONSTER_FLAGS_WIDE) {
                    if (target->m_facing == 1) {
                        target->m_hex--;
                    } else {
                        target->m_hex++;
                    }
                }
            }
        }
    }
    if (!retaliation &&
        (m_spellInfluence[ARMY_SPELL_INFLUENCE_ATTACK_TARGETING] ||
         m_spellInfluence[ARMY_SPELL_INFLUENCE_ATTACK_EXPIRING])) {
        CancelSpellType(1);
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }
    m_targetSide = -1;
    if (retaliation) {
        gpCombatManager->m_currentSide = 1 - gpCombatManager->m_currentSide;
    }
}

VA(0x0044f408, 0x16)
void army::ResetPath(void) {}

VA(0x0044f41e, 0x25)
int army::WalkTo(void)
{
    return WalkTo(m_moveTargetHex);
}

VA(0x0044f443, 0x2ee)
int army::WalkTo(int destination)
{
    int moatDestination;
    int canEnterMoat;
    int moatIndex;
    int direction;
    int steps;

    m_targetIndex = -1;
    m_targetSide = m_targetIndex;
    if (gpCombatManager->m_drawbridgeBackgroundVisible &&
        (m_monster.flags.all & MONSTER_FLAGS_WIDE)) {
        moatDestination = 0;
        moatIndex = 0;
        for (direction = 0; direction < ARMY_MOAT_CELL_COUNT; direction++) {
            if (moatCell[direction] == destination) {
                moatDestination = 1;
                moatIndex = direction;
            }
        }
        if (moatDestination) {
            canEnterMoat = 0;
            if (moatIndex == ARMY_MOAT_GATE_INDEX &&
                gpCombatManager->m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN) {
                canEnterMoat = 1;
            }
            if ((moatIndex > 0 && moatCell[moatIndex - 1] == m_hex) ||
                (moatIndex < ARMY_MOAT_CELL_COUNT - 1 &&
                 moatCell[moatIndex + 1] == m_hex)) {
                canEnterMoat = 1;
            }
            for (direction = 0; direction < ARMY_ADJACENT_DIRECTION_COUNT; direction++) {
                if (GetAdjacentCellIndex(m_hex, direction) == moatCell[moatIndex]) {
                    canEnterMoat = 1;
                }
            }
            if (m_side == 0 && moatCell[m_hex / ARMY_HEX_COLUMNS] < m_hex) {
                canEnterMoat = 1;
            }
            if (m_side == 1 && m_hex < moatCell[m_hex / ARMY_HEX_COLUMNS]) {
                canEnterMoat = 1;
            }
            if (!canEnterMoat) {
                if (m_facing == 1) {
                    destination--;
                } else {
                    destination++;
                }
            }
        }
    }
    if (!FindPath(m_hex, destination, m_monster.speed, 1, 0)) {
        return ARMY_PATH_BLOCKED;
    }

    steps = 0;
    for (direction = gpSearchArray->m_pathLength - 1; direction >= 0; direction--) {
        Walk(gpSearchArray->m_storage.path.directions[direction],
             0,
             gpSearchArray->m_pathLength - 1 != direction);
        steps++;
        if (steps >= m_monster.speed) {
            direction = -1;
        }
    }
    CancelSpellType(0);
    m_animationSequence = ARMY_ANIMATION_STAND;
    m_animationFrame = 0;
    gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    gpCombatManager->TestRaiseDoor();
    return 0;
}

VA(0x0044f731, 0x25)
int army::AttackTo(void)
{
    return AttackTo(m_moveTargetHex);
}

VA(0x0044f756, 0x1e8)
int army::AttackTo(int destination)
{
    int finishStanding;
    int pathIndex;
    int steps;

    if (m_monster.flags.all & MONSTER_FLAGS_FLYING) {
        if (m_hex != destination) {
            FlyTo(destination);
        }
        DoAttack(0);
        return 0;
    }
    if ((m_monster.flags.all & MONSTER_FLAGS_BREATH_ATTACK) &&
        m_hex == m_moveTargetHex) {
        DoAttack(0);
        return 0;
    }
    if (FindPath(m_hex, destination, m_monster.speed, 1, 0)) {
        if (gpSearchArray->m_pathLength == 1) {
            m_attackDirection = gpSearchArray->m_storage.path.directions[1];
            gpCombatManager->TestRaiseDoor();
            DoAttack(0);
        } else {
            pathIndex = 0;
            steps = 0;
            for (pathIndex = gpSearchArray->m_pathLength - 1;
                 pathIndex != 0;
                 pathIndex--) {
                steps++;
                if (pathIndex == 1 || steps >= m_monster.speed) {
                    finishStanding = 1;
                } else {
                    finishStanding = 0;
                }
                Walk(gpSearchArray->m_storage.path.directions[pathIndex + 1],
                     finishStanding,
                     gpSearchArray->m_pathLength - 1 != pathIndex);
                if (steps >= m_monster.speed && pathIndex != 1) {
                    return ARMY_PATH_BLOCKED;
                }
            }
            CancelSpellType(0);
            m_attackDirection = gpSearchArray->m_storage.path.directions[1];
            gpCombatManager->TestRaiseDoor();
            DoAttack(0);
        }
        return 0;
    }
    return ARMY_PATH_BLOCKED;
}

VA(0x0044f93e, 0x282)
void army::CheckLuck(void)
{
    SAMPLE2 luckSample;
    char *armyName;

    m_luckOutcome = 0;
    if (gpCombatManager->m_heroes[m_side]) {
        if (m_luck > 0 && SRandom(1, ARMY_LUCK_ROLL_MAX) <= m_luck) {
            m_luckOutcome = 1;
        }
        if (m_luck < 0 && SRandom(1, ARMY_LUCK_ROLL_MAX) < -m_luck) {
            m_luckOutcome = -1;
        }
        if (m_luckOutcome) {
            luckSample = GetNullSample();
            if (m_luckOutcome < 0) {
                sprintf(gText, "badluck.82m");
            } else {
                sprintf(gText, "goodluck.82m");
            }
            luckSample = LoadPlaySample(gText);
            if (m_luckOutcome < 0) {
                if (m_quantity > 1) {
                    armyName = gArmyNamesPlural[m_monsterType];
                } else {
                    armyName = gArmyNames[m_monsterType];
                }
                sprintf(gText, "Bad luck descends on the %s", armyName);
                gpCombatManager->CombatMessage(gText, 1, 1, 0);
                SpellEffect(ARMY_BAD_LUCK_EFFECT, ARMY_BAD_LUCK_EFFECT_DELAY, 0);
            } else {
                if (m_quantity > 1) {
                    armyName = gArmyNamesPlural[m_monsterType];
                } else {
                    armyName = gArmyNames[m_monsterType];
                }
                sprintf(gText, "Good luck shines on the %s", armyName);
                gpCombatManager->CombatMessage(gText, 1, 1, 0);
                gpCombatManager->DoLuck(m_side, m_index);
            }
            WaitEndSample(luckSample, -1);
            if (m_luckOutcome > 0) {
                gpCombatManager->DrawFrame(
                    1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
                gpMouseManager->ShowColorPointer();
            }
        }
    }
}

VA(0x0044fbc0, 0x56e)
void army::DamageEnemy(army *target, int *damageResult, int *killedResult,
                       int rangedAttack, int defenseModifier)
{
    float damage;
    int attackBonus;
    int defenseBonus;
    int attackDifference;
    int rearHex;
    int i;
    int damageDone;
    int genieDamage;
    hero *commander;

    if (!target) {
        return;
    }
    damage = 0;
    gbGenieHalf = 0;
    for (i = 0; i < m_quantity; i++) {
        if (m_spellInfluence[ARMY_SPELL_INFLUENCE_FIXED_DAMAGE]) {
            damage += m_monster.damageMax;
        } else if (m_spellInfluence[ARMY_SPELL_INFLUENCE_FIXED_DAMAGE]) {
            damage += m_monster.damageMin;
        } else {
            damage += SRandom(m_monster.damageMin, m_monster.damageMax);
        }
    }
    attackBonus = 0;
    defenseBonus = 0;
    attackDifference = m_monster.attack + attackBonus -
        (target->m_monster.defense + defenseBonus + defenseModifier);
    if (m_spellInfluence[ARMY_SPELL_INFLUENCE_DRAGON_SLAYER] &&
        (target->m_monsterType == ARMY_CREATURE_GREEN_DRAGON ||
         target->m_monsterType == ARMY_CREATURE_RED_DRAGON ||
         target->m_monsterType == ARMY_CREATURE_BLACK_DRAGON ||
         target->m_monsterType == ARMY_CREATURE_BONE_DRAGON)) {
        attackDifference += ARMY_DRAGON_SLAYER_BONUS;
    }
    if (gpCombatManager->m_drawbridgeBackgroundVisible) {
        rearHex = -1;
        if (target->m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            rearHex =
                ((-(static_cast<unsigned int>(target->m_facing - 1) < 1) & 2) - 1) +
                target->m_hex;
        }
        for (i = 0; i < ARMY_MOAT_CELL_COUNT; i++) {
            if (moatCell[i] == target->m_hex || moatCell[i] == rearHex) {
                attackDifference += ARMY_MOAT_ATTACK_BONUS;
            }
        }
    }
    if (attackDifference > ARMY_DAMAGE_STAT_LIMIT) {
        attackDifference = ARMY_DAMAGE_STAT_LIMIT;
    }
    if (attackDifference < -ARMY_DAMAGE_STAT_LIMIT) {
        attackDifference = -ARMY_DAMAGE_STAT_LIMIT;
    }
    damage *= gfBattleStat[attackDifference + ARMY_DAMAGE_STAT_LIMIT];
    if ((m_monsterType == ARMY_CREATURE_CRUSADER &&
         (target->m_monster.flags.all & MONSTER_FLAGS_UNDEAD)) ||
        (m_monsterType == ARMY_CREATURE_EARTH_ELEMENTAL &&
         target->m_monsterType == ARMY_CREATURE_AIR_ELEMENTAL) ||
        (m_monsterType == ARMY_CREATURE_AIR_ELEMENTAL &&
         target->m_monsterType == ARMY_CREATURE_EARTH_ELEMENTAL) ||
        (m_monsterType == ARMY_CREATURE_WATER_ELEMENTAL &&
         target->m_monsterType == ARMY_CREATURE_FIRE_ELEMENTAL) ||
        (m_monsterType == ARMY_CREATURE_FIRE_ELEMENTAL &&
         target->m_monsterType == ARMY_CREATURE_WATER_ELEMENTAL)) {
        damage *= 2.0f;
    }
    if (m_luckOutcome > 0) {
        damage *= 2.0f;
    }
    if (m_luckOutcome < 0) {
        damage /= 2.0f;
    }
    m_luckOutcome = 0;
    if (rangedAttack && gpCombatManager->ShotIsThroughWall(m_side, m_hex, target->m_hex)) {
        damage /= 2.0f;
    }
    commander = gpCombatManager->m_heroes[m_side];
    if (commander && rangedAttack) {
        damage *= gfSSArcheryMod[commander->m_secondarySkills[HERO_SKILL_ARCHERY]];
    }
    if ((m_monster.flags.all & MONSTER_FLAGS_SHOOTER) && !rangedAttack &&
        m_monsterType != ARMY_CREATURE_TITAN &&
        m_monsterType != ARMY_CREATURE_MAGE &&
        m_monsterType != ARMY_CREATURE_ARCHMAGE) {
        damage /= 2.0f;
    }
    if (rangedAttack &&
        target->m_spellInfluence[ARMY_SPELL_INFLUENCE_RANGED_SHIELD]) {
        damage /= 2.0f;
    }
    if (m_damagePenalty == 2) {
        damage /= 2.0f;
    }
    m_damagePenalty = 0;
    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_PETRIFIED]) {
        damage /= 2.0f;
    }
    damageDone = static_cast<int>(damage + 0.5);
    if (m_monsterType == ARMY_CREATURE_GENIE &&
        SRandom(1, ARMY_GENIE_HALF_ROLL_MAX) == ARMY_GENIE_HALF_ROLL) {
        genieDamage = ((target->m_quantity + 1) / 2) * target->m_monster.hitPoints;
        if (genieDamage > damageDone) {
            gbGenieHalf = 1;
            damageDone = genieDamage;
        }
    }
    if (damageDone <= 0) {
        damageDone = 1;
    }
    if (target->m_monster.flags.all & MONSTER_FLAGS_LIGHT_PALETTE) {
        damageDone = -1;
    }
    *damageResult = damageDone;
    *killedResult = target->Damage(damageDone, -1);
}

VA(0x0045012e, 0x23c)
int army::Damage(long int, int) { return 0; }

VA(0x0045036a, 0x1361)
void army::PowEffect(int, int, int, int) {}

VA(0x004516cb, 0x35)
unsigned long int army::Strength(void) { return 0; }

VA(0x00451700, 0x66)
int army::LeaveNoBody(void) { return 0; }

VA(0x00451766, 0x3f5)
void army::ProcessDeath(int) {}

VA(0x00451b5b, 0x39d)
void army::SpellEffect(int, int, int) {}

VA(0x00451ef8, 0x10f)
void army::CancelSpellType(int) {}

VA(0x00452007, 0x178)
void army::CancelIndividualSpell(int) {}

VA(0x0045217f, 0x282)
int army::SetSpellInfluence(int, int) { return 0; }

VA(0x00452401, 0x94)
void army::DecrementSpellRounds(void) {}

VA(0x00452495, 0x644)
void army::GoBerserk(void) {}

VA(0x00452ad9, 0x3f1)
void army::MoveAttack(int, int) {}

VA(0x00452eca, 0x931)
float army::SpellCastWorkChance(int) { return 0; }

VA(0x004537fb, 0x56)
int army::SpellCastWorks(int) { return 0; }

VA(0x00453851, 0x39e)
void BuildTempWalkSeq(struct SMonFrameInfo *, int, int) {}

VA(0x00453bef, 0x66)
void army::DispelGood(void) {}

VA(0x00453c55, 0x90)
void army::Cure(int) {}

VA(0x00453ce5, 0x79)
int army::MidX(void) { return 0; }

VA(0x00453d5e, 0x59)
int army::MidY(void) { return 0; }

VA(0x00453db7, 0x57)
int army::TopY(void) { return 0; }

VA(0x00453e0e, 0xcc)
int army::RightX(void) { return 0; }

VA(0x00453eda, 0xcc)
int army::LeftX(void) { return 0; }

VA(0x00453fa6, 0x171)
int army::OtherArmyAdjacent(int, int) { return 0; }

VA(0x00454117, 0x1e1)
void ModifyFrameInfo(struct SMonFrameInfo *, int) {}

VA(0x004542f8, 0xbe)
int army::GetPowBaseY(void) { return 0; }

// ---- globals (definitions, RVA order) ----
DATA(0x004f54a8) int bSecondAttack;
DATA(0x00527eb4) int gbGenieHalf;
