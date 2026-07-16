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
#include <SOURCE/SPELLS.h>
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
    m_unknown5e = 0;
    m_moveTargetHex = 0;
    m_palette = 0;
    m_showQuantity = 1;
    m_yOffset = 0;
    m_xOffset = 0;
}

VA(0x0044a98f, 0x18)
void army::WaitSample(int sampleIndex)
{
    return;
}

VA(0x0044a9a7, 0xde)
void army::InitClean(void)
{
    int i;

    for (i = 0; i < ARMY_SAMPLE_COUNT; i++) {
        m_samples[i] = 0;
    }
    m_roundCounter = -1;
    m_spellCount = 0;
    memset(m_spellInfluence, 0, sizeof(m_spellInfluence));
    m_lastAnimationTime = KBTickCount();
    m_drawEnabled = 1;
    m_creatureIcon = 0;
    m_drawSpellEffect = 0;
    m_spellEffect = -1;
    m_mirrorSourceIndex = -1;
    m_mirrorImageIndex = -1;
    m_armyGroupSlot = -1;
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
    m_unknownD4 = 6;
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
    m_temporaryResurrectionQuantity = 0;
    m_animationState = 0;
    m_hitPointsLost = 0;
    m_damagePenalty = 0;
    m_killPending = 0;
    m_deathPending = m_killPending;
    m_damagePending = m_deathPending;
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
    m_armyGroupSlot = unknown;
}

// @early-stop
// All instructions and 87/87 relocation sites align; only delinked string
// literal symbol identities differ.
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

    sprintf(gText, "%smove.82M", m_monster.spriteName);
    m_samples[ARMY_SAMPLE_MOVE] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%sattk.82M", m_monster.spriteName);
    m_samples[ARMY_SAMPLE_ATTACK] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%swnce.82M", m_monster.spriteName);
    m_samples[ARMY_SAMPLE_WINCE] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%skill.82M", m_monster.spriteName);
    m_samples[ARMY_SAMPLE_KILL] = gpResourceManager->GetSample(gText);

    if (m_monster.flags.all & MONSTER_FLAGS_SHOOTER) {
        sprintf(gText, "%sshot.82M", m_monster.spriteName);
        m_samples[ARMY_SAMPLE_SHOT] = gpResourceManager->GetSample(gText);
    }
    if (m_monsterType == ARMY_CREATURE_VAMPIRE ||
        m_monsterType == ARMY_CREATURE_VAMPIRE_LORD) {
        sprintf(gText, "%sext1.82M", m_monster.spriteName);
        m_samples[ARMY_SAMPLE_EXTRA_ONE] = gpResourceManager->GetSample(gText);
        sprintf(gText, "%sext2.82M", m_monster.spriteName);
        m_samples[ARMY_SAMPLE_EXTRA_TWO] = gpResourceManager->GetSample(gText);
    }
    if (m_monsterType == ARMY_CREATURE_LICH ||
        m_monsterType == ARMY_CREATURE_POWER_LICH) {
        sprintf(gText, "%sexpl.82M", m_monster.spriteName);
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

// @early-stop
// All instructions before the function-local compressed switch table and all
// 42 relocation sites align. The table/selector span at +0x4f0..+0x513 has only
// local-label identities; its dispatch and five case bodies were audited raw.
VA(0x0044b456, 0x7d2)
void army::DrawToBuffer(int x, int y, int effectsOnly)
{
    int quantityY0;
    int spellY;
    unsigned char *palette8;
    int spellX2;
    int standing7;
    int yDistance6;
    int xDistance;
    int unused;
    int effect6;
    int frameCount17;
    int color;
    int quantityX5;
    int goodEffects0;
    int drawn1;
    int neighborOccupied14;
    int badEffects0;
    int quantityOffset0;
    char quantityText3[ARMY_QUANTITY_TEXT_SIZE];
    int statusFrame0;
    int neighboringHex;

    if (gpCombatManager->m_nonVisualCombat) {
        return;
    }
    if (gbNoShowCombat) {
        return;
    }

    unused = 0;
    color = 0;
    standing7 = m_animationSequence == ARMY_ANIMATION_STAND ||
        (m_animationSequence >= ARMY_ANIMATION_STANDING_FIRST &&
         m_animationSequence <= ARMY_ANIMATION_STANDING_END - 1);
    y += m_yOffset;
    x += m_xOffset;
    if (m_animationSequence == ARMY_ANIMATION_WALK &&
        !(m_monster.flags.all & MONSTER_FLAGS_FLYING)) {
        frameCount17 = m_frameInfo.animationFrameCount[m_animationSequence];
        yDistance6 = m_animationFrame * 42 / frameCount17;
        xDistance = m_animationFrame * 22 / frameCount17;
        if (m_walkDirection == 0 || m_walkDirection == 5) {
            y -= yDistance6;
            giWalkingYMod = -yDistance6;
        }
        if (m_walkDirection == 2 || m_walkDirection == 3) {
            y += yDistance6;
            giWalkingYMod = yDistance6;
        }
        if (m_walkDirection == 0 || m_walkDirection == 2) {
            x -= xDistance;
        }
        if (m_walkDirection == 5 || m_walkDirection == 3) {
            x += xDistance;
        }
    }
    if (giSpellEffectShowType == 0 && standing7 && m_spellCount > 0) {
        color = 0xed;
    }
    if (m_hex == gpCombatManager->m_limitCreatureHex &&
        gpCombatManager->m_limitCreature == 1) {
        color = 0xec;
    }

    palette8 = 0;
    if (m_spellInfluence[ARMY_SPELL_INFLUENCE_PETRIFIED]) {
        palette8 = gColorTableGray;
    } else if (m_monster.flags.all & MONSTER_FLAGS_RED_PALETTE) {
        palette8 = gColorTableRed;
    } else if (m_monster.flags.all & MONSTER_FLAGS_DARK_BROWN_PALETTE) {
        palette8 = gColorTableDarkBrown;
    } else if (m_monster.flags.all & MONSTER_FLAGS_GRAY_PALETTE) {
        palette8 = gColorTableGray;
    } else if (m_monster.flags.all & MONSTER_FLAGS_LIGHT_PALETTE) {
        palette8 = gColorTableLighten;
    }
    if (effectsOnly == 0) {
        m_creatureIcon->CombatClipDrawToBuffer(
            x,
            y,
            m_frameInfo.animationFrames[m_animationSequence][m_animationFrame],
            &m_spriteLimits,
            m_facing == 0,
            color,
            palette8,
            m_palette);
    }

    if (standing7 && gpCombatManager->m_showArmyQuantities && m_showQuantity) {
        if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            if (m_facing == 1) {
                quantityX5 = x + 53;
                neighboringHex = m_hex + 2;
            } else {
                quantityX5 = x - 73;
                neighboringHex = m_hex - 2;
            }
        } else if (m_facing == 1) {
            quantityX5 = x + 9;
            neighboringHex = m_hex + 1;
        } else {
            quantityX5 = x - 29;
            neighboringHex = m_hex - 1;
        }
        neighborOccupied14 =
            gpCombatManager->m_hexCells[neighboringHex].m_occupantSide != -1;
        quantityOffset0 = m_frameInfo.quantityX[1 - m_facing];
        if (neighborOccupied14 && quantityOffset0 > 0) {
            quantityOffset0 = 0;
        }
        if (m_facing == 1)
            quantityX5 += quantityOffset0;
        else
            quantityX5 -= -(-quantityOffset0);
        quantityY0 = m_facing == 1 ? y - 11 : y - 23;
        goodEffects0 = 0;
        badEffects0 = 0;
        for (effect6 = 0; effect6 < ARMY_SPELL_INFLUENCE_COUNT; effect6++) {
            if (m_spellInfluence[effect6]) {
                switch (effect6) {
                case 0:
                case 3:
                case 8:
                case 9:
                case 10:
                case 12:
                case 13:
                case 14:
                    goodEffects0++;
                    break;
                default:
                    badEffects0++;
                }
            }
        }
        if (giSpellEffectShowType == 0 || !standing7 || m_spellCount < 1) {
            drawn1 = gpCombatManager->m_combatIcons[COMBAT_ICON_STATUS]->CombatClipDrawToBuffer(
                quantityX5, quantityY0, 10, &m_creatureLimits, 0, 0, 0, 0);
        } else if (giSpellEffectShowType == 1) {
            drawn1 = gpCombatManager->m_combatIcons[COMBAT_ICON_STATUS]->CombatClipDrawToBuffer(
                quantityX5, quantityY0, 11, &m_creatureLimits, 0, 0xed, 0, 0);
        } else {
            statusFrame0 = 2;
            if (goodEffects0 < 1 || badEffects0 < 1) {
                if (badEffects0 > 0) {
                    statusFrame0 = 4;
                }
            } else {
                statusFrame0 = 3;
            }
            drawn1 = gpCombatManager->m_combatIcons[COMBAT_ICON_STATUS]->CombatClipDrawToBuffer(
                quantityX5, quantityY0, statusFrame0 + 10, &m_creatureLimits, 0, 0, 0, 0);
        }
        if (drawn1) {
            sprintf(quantityText3, "%d", m_lastTargetHex == -1 ? m_quantity : m_lastTargetHex);
            smallFont->DrawBoundedString(quantityText3, quantityX5, quantityY0 + 2, 20, 12, 1, 1);
        }
    }

    if (m_drawSpellEffect && effectsOnly == 0) {
        spellX2 = x;
        spellY = GetPowBaseY();
        if (m_animationSequence == ARMY_ANIMATION_WINCE ||
            m_animationSequence == ARMY_ANIMATION_WINCE_RETURN) {
            if (m_facing == 1) {
                spellX2 -= 4;
            } else {
                spellX2 += 4;
            }
        }
        if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            if (m_facing == 1) {
                spellX2 += 22;
            } else {
                spellX2 -= 22;
            }
        }
        if (gCurLoadedSpellEffect == 23) {
            if (m_facing == 1) {
                spellX2 = RightX();
            } else {
                spellX2 = LeftX();
            }
        }
        if (gCurLoadedSpellEffect == 28) {
            spellX2 = ((-(m_facing == 1) & 2) - 1) * m_frameInfo.spellEffectX + x;
            spellY = m_frameInfo.spellEffectY + y;
        }
        gCurLoadedSpellIcon->CombatClipDrawToBuffer(
            spellX2,
            m_spellEffectYOffset + spellY,
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

// @early-stop
// Logic, frame slots, CFG, and all 151 relocation sites align. The residual is
// limited to TU-cumulative operand order in four extent clamps and two final
// hex-range comparisons, plus one delinked gConfig relocation identity. Direct,
// commuted, negated, and scalar-SIB comparison spellings were audited.
VA(0x0044bc55, 0xb90)
void army::Walk(int direction, int finishStanding, int skipDrawing)
{
    int oldMaxX_1;
    int oldMaxY_1;
    int destination_3;
    int otherHex_1;
    int frame;
    int oldMinX;
    int oldHex_7;
    int oldMinY_4;
    int finalDestination;

    destination_3 = GetAdjacentCellIndex(m_hex, direction);
    if (m_side == 1 && gpCombatManager->m_inCastleCombat &&
        (destination_3 == 58 || destination_3 == 59 ||
         (destination_3 == 60 && m_side == 1 &&
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
        giWalkingFrom2 =
            (static_cast<unsigned int>(m_facing - 1) < 1 ? 1 : -1) + m_hex;
    } else {
        giWalkingFrom2 = -1;
    }
    giWalkingTo = destination_3;
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        giWalkingTo2 =
            (static_cast<unsigned int>(m_facing - 1) < 1 ? 1 : -1) +
            destination_3;
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
    oldMinY_4 = giMinExtentY;
    oldMaxX_1 = giMaxExtentX;
    oldMaxY_1 = giMaxExtentY;

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
    oldHex_7 = m_hex;
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
                oldMinY_4 = giMinExtentY;
                oldMaxX_1 = giMaxExtentX;
                oldMaxY_1 = giMaxExtentY;
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
            if (!(oldMinX <= giMinExtentX)) {
                oldMinX = giMinExtentX;
            }
            if (!(oldMinY_4 <= giMinExtentY)) {
                oldMinY_4 = giMinExtentY;
            }
            if (!(oldMaxX_1 >= giMaxExtentX)) {
                oldMaxX_1 = giMaxExtentX;
            }
            if (!(oldMaxY_1 >= giMaxExtentY)) {
                oldMaxY_1 = giMaxExtentY;
            }
            DelayTil(glTimers);
            glTimers[0] = static_cast<int>(
                KBTickCount() +
                m_walkDuration * gfCombatSpeedMod[gConfig.combatSpeed] /
                    m_frameInfo.animationFrameCount[ARMY_ANIMATION_WALK]);
            gpWindowManager->UpdateScreenRegion(
                oldMinX, oldMinY_4,
                oldMaxX_1 - oldMinX + 1,
                oldMaxY_1 - oldMinY_4 + 1);
        }
    }

    finalDestination = GetAdjacentCellIndex(m_hex, direction);
    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantFrame = -1;
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        otherHex_1 =
            (static_cast<unsigned int>(m_facing - 1) < 1 ? 1 : -1) + m_hex;
        gpCombatManager->m_hexCells[otherHex_1].m_occupantIndex = -1;
        gpCombatManager->m_hexCells[otherHex_1].m_occupantSide = -1;
        gpCombatManager->m_hexCells[otherHex_1].m_occupantFrame = -1;
    }
    gpCombatManager->m_hexCells[finalDestination].m_occupantSide = m_side;
    gpCombatManager->m_hexCells[finalDestination].m_occupantIndex = m_index;
    gpCombatManager->m_hexCells[finalDestination].m_occupantFrame = -1;
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        otherHex_1 =
            (static_cast<unsigned int>(m_facing - 1) < 1 ? 1 : -1) +
            finalDestination;
        gpCombatManager->m_hexCells[otherHex_1].m_occupantSide = m_side;
        gpCombatManager->m_hexCells[otherHex_1].m_occupantIndex = m_index;
        if (otherHex_1 >= finalDestination) {
            gpCombatManager->m_hexCells[otherHex_1].m_occupantFrame = 1;
        } else {
            gpCombatManager->m_hexCells[otherHex_1].m_occupantFrame = 0;
        }
        if (finalDestination >= otherHex_1) {
            gpCombatManager->m_hexCells[finalDestination].m_occupantFrame = 1;
        } else {
            gpCombatManager->m_hexCells[finalDestination].m_occupantFrame = 0;
        }
    }
    m_hex = finalDestination;
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

// @early-stop
// Logic, 0x1d8 frame/slots, CFG, and all external relocations align. Residuals
// are TU-cumulative army-array/extent operand order, equivalent float-compare
// polarity, constant-pool identities, one continuation jump, and the delinked
// recursive local call. Manual variants and an audited AST permutation pass
// were exhausted; revisit after later ARMY source/header changes.
VA(0x0044c7e5, 0x14aa)
void army::SpecialAttack(void)
{
    char combatText[ARMY_COMBAT_TEXT_SIZE];
    int missileDelay_2;
    int minY_1;
    int oldX_16;
    char targetColumn_2;
    int minX_1;
    float angle_2;
    int missileHalfHeight_1;
    char sourceColumn_8;
    int attackDirection_3;
    int effectType_1;
    int initialXDistance_6;
    int yStep_6;
    int currentMissileY_7;
    int xStep_29;
    int currentMissileX_7;
    int effectY_28;
    char sourceRow;
    int missileSteps_7;
    int oldY_9;
    int effectX_2;
    char targetRow_1;
    char reverseMissile;
    int distance_6;
    int originalFacing_6;
    int initialYDistance_8;
    int backgroundX_4;
    int maxX_3;
    int xDistance_7;
    int maxY_3;
    bitmap *missileBackground;
    int backgroundY_6;
    int originalAttack;
    int direction;
    int missileHalfWidth;
    int sourceY_1;
    int targetY_1;
    int missileSpacing_1;
    int missileY_7;
    int endX_36;
    int yDistance_16;
    int sourceX_1;
    int targetX_1;
    int missileX_7;
    int damage;
    float slope;
    int directionFrame_2;
    int endY_19;
    int killed_13;
    army *target_1;

    damage = 0;
    killed_13 = 0;
    originalFacing_6 = m_facing;
    m_palette = 0;
    target_1 = m_targetIndex + gpCombatManager->m_armies[m_targetSide];
    targetColumn_2 = target_1->m_hex % 13;
    targetRow_1 = target_1->m_hex / 13;
    sourceColumn_8 = m_hex % 13;
    sourceRow = m_hex / 13;
    originalFacing_6 = m_facing;
    if (!(targetColumn_2 <= sourceColumn_8) ||
        (!(sourceRow & 1) && targetColumn_2 == sourceColumn_8)) {
        m_facing = 1;
    } else {
        m_facing = 0;
    }
    if (m_facing != originalFacing_6) {
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

    targetX_1 = target_1->MidX();
    targetY_1 = target_1->MidY();
    if (m_monsterType == ARMY_CREATURE_LICH ||
        m_monsterType == ARMY_CREATURE_POWER_LICH) {
        targetX_1 = gpCombatManager->m_hexCells[target_1->m_hex].m_x;
        targetY_1 = gpCombatManager->m_hexCells[target_1->m_hex].m_y - 17;
    }
    if (m_facing == 1) {
        sourceX_1 = gpCombatManager->m_hexCells[m_hex].m_x +
            m_frameInfo.missileOffsets[1].x;
    } else {
        sourceX_1 = gpCombatManager->m_hexCells[m_hex].m_x -
            m_frameInfo.missileOffsets[1].x;
    }
    sourceY_1 = gpCombatManager->m_hexCells[m_hex].m_y +
        m_frameInfo.missileOffsets[1].y;
    initialXDistance_6 = targetX_1 - sourceX_1;
    reverseMissile = 0;
    if (initialXDistance_6 < 0) {
        reverseMissile = 1;
        initialXDistance_6 = -initialXDistance_6;
    }
    initialYDistance_8 = targetY_1 - sourceY_1;
    if (initialXDistance_6 == 0) {
        if (initialYDistance_8 > 0) {
            directionFrame_2 = m_frameInfo.projectileDirectionCount - 1;
        } else {
            directionFrame_2 = 0;
        }
        angle_2 = static_cast<float>(((initialYDistance_8 <= 0) - 1 & -180) + 90);
    } else {
        slope = static_cast<float>(-initialYDistance_8) / initialXDistance_6;
        angle_2 = static_cast<float>(atan(static_cast<double>(slope)) *
            180.0 / ARMY_PROJECTILE_PI);
        for (direction = 1;
             direction < m_frameInfo.projectileDirectionCount;
             direction++) {
            if ((m_frameInfo.projectileAngles[direction - 1] +
                 m_frameInfo.projectileAngles[direction]) / 2.0f < angle_2) {
                break;
            }
        }
        if (direction < m_frameInfo.projectileDirectionCount) {
            directionFrame_2 = direction - 1;
        } else {
            directionFrame_2 = m_frameInfo.projectileDirectionCount - 1;
        }
    }
    if (angle_2 > ARMY_SHOOT_UP_MIN_ANGLE) {
        m_animationSequence = ARMY_ANIMATION_SHOOT_UP;
        attackDirection_3 = 0;
    } else if (angle_2 > ARMY_SHOOT_FORWARD_MIN_ANGLE) {
        m_animationSequence = ARMY_ANIMATION_SHOOT_FORWARD;
        attackDirection_3 = 1;
    } else {
        m_animationSequence = ARMY_ANIMATION_SHOOT_DOWN;
        attackDirection_3 = 2;
    }
    for (m_animationFrame = 0;
         m_animationFrame < m_frameInfo.animationFrameCount[m_animationSequence];
         m_animationFrame++) {
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
    }
    m_animationFrame = m_frameInfo.animationFrameCount[m_animationSequence] - 1;

    missileHalfWidth = 25;
    missileHalfHeight_1 = 25;
    missileSpacing_1 = 31;
    missileDelay_2 = 25;
    if (m_monsterType == ARMY_CREATURE_LICH ||
        m_monsterType == ARMY_CREATURE_POWER_LICH) {
        missileSpacing_1 = 26;
        missileDelay_2 = 7;
        missileHalfWidth = 10;
        missileHalfHeight_1 = 10;
    }
    maxX_3 = 0;
    minX_1 = ARMY_COMBAT_MAX_X;
    maxY_3 = 0;
    minY_1 = ARMY_PROJECTILE_CLIP_HEIGHT - 1;
    if (m_facing == 1) {
        missileX_7 = gpCombatManager->m_hexCells[m_hex].m_x +
            m_frameInfo.missileOffsets[attackDirection_3].x;
    } else {
        missileX_7 = gpCombatManager->m_hexCells[m_hex].m_x -
            m_frameInfo.missileOffsets[attackDirection_3].x;
    }
    missileY_7 = gpCombatManager->m_hexCells[m_hex].m_y +
        m_frameInfo.missileOffsets[attackDirection_3].y;
    endX_36 = target_1->MidX();
    endY_19 = target_1->MidY();
    xDistance_7 = endX_36 - missileX_7;
    yDistance_16 = endY_19 - missileY_7;
    distance_6 = static_cast<int>(sqrt(static_cast<double>(
        xDistance_7 * xDistance_7 + yDistance_16 * yDistance_16)));
    missileSteps_7 = (distance_6 + (missileSpacing_1 >> 1)) / missileSpacing_1;

    if (m_monsterType == ARMY_CREATURE_MAGE ||
        m_monsterType == ARMY_CREATURE_ARCHMAGE) {
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1);
        DelayMilli(static_cast<long>(gfCombatSpeedMod[gConfig.combatSpeed] *
                                     ARMY_MAGE_BOLT_DELAY));
        gpCombatManager->DoBolt(
            1, missileX_7, missileY_7, endX_36, endY_19, 0, 0, 5, 4, 302, 0, 0,
            distance_6 / 15 + 15, 1, 0, 10, 0);
    } else {
        if (missileSteps_7 > 1) {
            xStep_29 = xDistance_7 / (missileSteps_7 - 1);
            yStep_6 = yDistance_16 / (missileSteps_7 - 1);
        } else {
            xStep_29 = xDistance_7;
            yStep_6 = yDistance_16;
        }
        currentMissileX_7 = missileX_7;
        currentMissileY_7 = missileY_7;
        missileBackground = new bitmap(
            33,
            static_cast<short>(missileHalfWidth * 2),
            static_cast<short>(missileHalfHeight_1 * 2));
        missileBackground->GrabBitmapCareful(
            gpWindowManager->m_screen,
            static_cast<short>(currentMissileX_7 - missileHalfWidth),
            static_cast<short>(currentMissileY_7 - missileHalfHeight_1));
        oldX_16 = currentMissileX_7;
        oldY_9 = currentMissileY_7;
        backgroundX_4 = 0;
        backgroundY_6 = 0;
        for (direction = 0; direction < missileSteps_7; direction++) {
            if (oldX_16 - missileHalfWidth < minX_1) {
                minX_1 = oldX_16 - missileHalfWidth;
            }
            if (minX_1 < 0) minX_1 = 0;
            if (maxX_3 < oldX_16 + missileHalfWidth) {
                maxX_3 = oldX_16 + missileHalfWidth;
            }
            if (maxX_3 > ARMY_COMBAT_MAX_X) maxX_3 = ARMY_COMBAT_MAX_X;
            if (oldY_9 - missileHalfHeight_1 < minY_1) {
                minY_1 = oldY_9 - missileHalfHeight_1;
            }
            if (minY_1 < 0) minY_1 = 0;
            if (maxY_3 < oldY_9 + missileHalfHeight_1) {
                maxY_3 = oldY_9 + missileHalfHeight_1;
            }
            if (maxY_3 > ARMY_COMBAT_MAX_Y) maxY_3 = ARMY_COMBAT_MAX_Y;
            if (direction != 0) {
                missileBackground->DrawToBufferCareful(
                    static_cast<short>(backgroundX_4),
                    static_cast<short>(backgroundY_6));
            } else {
                if (minX_1 < giMinExtentX) giMinExtentX = minX_1;
                if (giMaxExtentX < maxX_3) giMaxExtentX = maxX_3;
                if (giMinExtentY > minY_1) giMinExtentY = minY_1;
                if (maxY_3 > giMaxExtentY) giMaxExtentY = maxY_3;
            }
            backgroundX_4 = currentMissileX_7 - missileHalfWidth;
            if (backgroundX_4 < 0) backgroundX_4 = 0;
            if (backgroundX_4 + missileBackground->m_width > ARMY_COMBAT_WIDTH) {
                backgroundX_4 = ARMY_COMBAT_WIDTH - missileBackground->m_width;
            }
            backgroundY_6 = currentMissileY_7 - missileHalfHeight_1;
            if (backgroundY_6 < 0) backgroundY_6 = 0;
            if (backgroundY_6 + missileBackground->m_height > ARMY_COMBAT_WIDTH) {
                backgroundY_6 = ARMY_COMBAT_WIDTH - missileBackground->m_height;
            }
            missileBackground->GrabBitmapCareful(
                gpWindowManager->m_screen,
                static_cast<short>(backgroundX_4),
                static_cast<short>(backgroundY_6));
            m_missileIcon->DrawToBuffer(
                currentMissileX_7, currentMissileY_7, directionFrame_2, reverseMissile);
            if (direction == 0) {
                gpWindowManager->UpdateScreenRegion(
                    giMinExtentX,
                    giMinExtentY,
                    giMaxExtentX - giMinExtentX + 1,
                    giMaxExtentY - giMinExtentY + 1);
            } else {
                DelayTil(glTimers);
                gpWindowManager->UpdateScreenRegion(
                    minX_1, minY_1, maxX_3 - minX_1 + 1, maxY_3 - minY_1 + 1);
            }
            glTimers[0] = static_cast<int>(
                KBTickCount() +
                missileDelay_2 * gfCombatSpeedMod[gConfig.combatSpeed]);
            oldX_16 = currentMissileX_7;
            oldY_9 = currentMissileY_7;
            currentMissileX_7 += xStep_29;
            currentMissileY_7 += yStep_6;
            minX_1 = currentMissileX_7 - missileHalfWidth;
            maxX_3 = currentMissileX_7 + missileHalfWidth;
            minY_1 = currentMissileY_7 - missileHalfHeight_1;
            maxY_3 = currentMissileY_7 + missileHalfHeight_1;
        }
        missileBackground->DrawToBuffer(
            static_cast<short>(backgroundX_4), static_cast<short>(backgroundY_6));
        gpWindowManager->UpdateScreenRegion(
            oldX_16 - missileHalfWidth,
            oldY_9 - missileHalfHeight_1,
            missileHalfWidth * 2,
            missileHalfHeight_1 * 2);
        delete missileBackground;
        if (!gpCombatManager->m_heroes[m_side] ||
            !gpCombatManager->m_heroes[m_side]->HasArtifact(
                ARMY_ENDLESS_AMMUNITION_ARTIFACT)) {
            m_monster.shots--;
        }
    }

    originalAttack = m_monster.attack;
    effectType_1 = -1;
    effectX_2 = -1;
    effectY_28 = -1;
    if (m_monsterType == ARMY_CREATURE_LICH ||
        m_monsterType == ARMY_CREATURE_POWER_LICH) {
        int adjacentHex;
        army *splashTarget;

        gpCombatManager->ClearEffects();
        for (direction = 0; direction < 7; direction++) {
            if (direction < 6) {
                adjacentHex = target_1->GetAdjacentCellIndex(target_1->m_hex, direction);
            } else {
                adjacentHex = target_1->m_hex;
            }
            if (adjacentHex == -1) {
                continue;
            }
            if (gpCombatManager->m_hexCells[adjacentHex].m_occupantSide != -1) {
                splashTarget = &gpCombatManager->m_armies
                    [gpCombatManager->m_hexCells[adjacentHex].m_occupantSide]
                    [gpCombatManager->m_hexCells[adjacentHex].m_occupantIndex];
                if (!gArmyEffected[splashTarget->m_side][splashTarget->m_index] &&
                    (target_1 != splashTarget || direction == 6)) {
                    gArmyEffected[splashTarget->m_side][splashTarget->m_index] = 1;
                    DamageEnemy(splashTarget, &damage, &killed_13, 1, 0);
                }
            }
        }
        m_spellEffectYOffset = 0;
        effectType_1 = ARMY_LICH_EXPLOSION_EFFECT;
        effectX_2 = gpCombatManager->m_hexCells[adjacentHex].m_x;
        effectY_28 = gpCombatManager->m_hexCells[adjacentHex].m_y - 17;
        gpSoundManager->MemorySample(m_samples[ARMY_SAMPLE_EXTRA_ONE]);
    } else {
        DamageEnemy(target_1, &damage, &killed_13, 1, 0);
    }
    m_monster.attack = originalAttack;

    if (killed_13 > 0) {
        if (damage == -1) {
            sprintf(gText, "The mirror image is destroyed!");
        } else {
            sprintf(gText, "%s %s %d %s, %d %s %s.",
                m_quantity > 1 ? gArmyNamesPlural[m_monsterType] :
                                 gArmyNames[m_monsterType],
                m_quantity > 1 ? "do" : "does",
                damage,
                "damage",
                killed_13,
                killed_13 > 1 ? gArmyNamesPlural[target_1->m_monsterType] :
                             gArmyNames[target_1->m_monsterType],
                killed_13 > 1 ? "perish" : "perishes");
            gText[0] -= ARMY_ASCII_CASE_OFFSET;
        }
    } else {
        sprintf(gText, "%s %s %d %s.",
            m_quantity > 1 ? gArmyNamesPlural[m_monsterType] :
                             gArmyNames[m_monsterType],
            m_quantity > 1 ? "do" : "does",
            damage,
            "damage");
        gText[0] -= ARMY_ASCII_CASE_OFFSET;
    }
    strcpy(combatText, gText);
    switch (m_monsterType) {
    case ARMY_CREATURE_ARCHMAGE:
        if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ARCHMAGE_DISPEL_CHANCE &&
            target_1 && target_1->SpellCastWorks(102)) {
            target_1->m_spellEffect = 102;
        }
        break;
    }
    PowEffect(effectType_1, 0, effectX_2, effectY_28);
    gpCombatManager->CombatMessage(combatText, 1, 1, 0);
    WaitSample(ARMY_SAMPLE_SHOT);

    if (m_facing != originalFacing_6) {
        if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            if (m_facing == 1) {
                m_hex++;
            } else {
                m_hex--;
            }
        }
        m_facing = originalFacing_6;
    }
    if (!bSecondAttack &&
        (m_monsterType == ARMY_CREATURE_ELF ||
         m_monsterType == ARMY_CREATURE_GRAND_ELF ||
         m_monsterType == ARMY_CREATURE_RANGER) &&
        target_1->m_quantity > 0) {
        bSecondAttack = 1;
        SpecialAttack();
        bSecondAttack = 0;
    }
    if (m_spellInfluence[ARMY_SPELL_INFLUENCE_BERSERK] ||
        m_spellInfluence[ARMY_SPELL_INFLUENCE_HYPNOTIZE]) {
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

// @early-stop
// Logic, frame slots, CFG, and all 48 relocation sites align. The remaining
// code differences are TU-cumulative side/index address evaluation at three
// army-array sites; direct, commuted, flat-pointer, and scalar-SIB spellings
// were audited. Other residual identities are delinked string literals.
VA(0x0044dcba, 0x4e7)
void army::DoHydraAttack(int)
{
    int damage_8;
    int killed_4;
    int targetIndex_9;
    int direction;
    int targetSide_8;
    int targetHex_2;
    int totalKilled_7;
    army *target_2;
    short attackMask_7;
    char combatText_8[200];
    int totalDamage_1;

    totalKilled_7 = 0;
    totalDamage_1 = totalKilled_7;
    gpCombatManager->ResetHitByCreature();
    if (m_spellInfluence[ARMY_SPELL_INFLUENCE_BERSERK]) {
        attackMask_7 = static_cast<short>(GetAttackMask(m_hex, 2, -1));
    } else {
        attackMask_7 = static_cast<short>(GetAttackMask(m_hex, 1, -1));
    }
    CheckLuck();
    gpCombatManager->ResetLimitCreature();
    gpCombatManager->m_limitCreatureCount[m_side][m_index]++;
    for (direction = 0; direction < ARMY_COMBAT_DIRECTION_COUNT; direction++) {
        if (!(attackMask_7 & (1 << direction))) {
            targetHex_2 = m_hex;
            if ((m_monster.flags.all & MONSTER_FLAGS_WIDE) &&
                ((m_facing == 0 && direction > 2) ||
                 (m_facing == 1 && (direction < 3 || direction > 5)))) {
                if (m_facing == 0) {
                    targetHex_2 = m_hex - 1;
                } else {
                    targetHex_2 = m_hex + 1;
                }
            }
            targetHex_2 = GetAdjacentCellIndex(targetHex_2, direction);
            if (ValidHex(targetHex_2)) {
                targetSide_8 =
                    gpCombatManager->m_hexCells[targetHex_2].m_occupantSide;
                targetIndex_9 =
                    gpCombatManager->m_hexCells[targetHex_2].m_occupantIndex;
                if (targetSide_8 >= 0 && targetIndex_9 >= 0) {
                    gpCombatManager
                        ->m_limitCreatureCount[targetSide_8][targetIndex_9]++;
                    target_2 =
                        &gpCombatManager->m_armies[targetSide_8][targetIndex_9];
                    if (!target_2->m_hitByCreature) {
                        target_2->m_hitByCreature = 1;
                        DamageEnemy(target_2, &damage_8, &killed_4, 0, 0);
                        totalDamage_1 += damage_8;
                        totalKilled_7 += killed_4;
                        gpCombatManager->m_limitCreatureCount
                            [targetSide_8][targetIndex_9]++;
                    }
                }
            }
        }
    }
    gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    m_animationState = 1;
    m_pendingAnimationSequence = ARMY_ATTACK_DELAY_NORMAL;
    gpSoundManager->MemorySample(m_samples[ARMY_SAMPLE_ATTACK]);
    if (totalKilled_7 > 0) {
        sprintf(gText, "%s %s %d %s, %d %s %s.",
            m_quantity > 1 ? gArmyNamesPlural[m_monsterType] : gArmyNames[m_monsterType],
            m_quantity > 1 ? "do" : "does",
            totalDamage_1,
            "damage",
            totalKilled_7,
            totalKilled_7 > 1 ? "creatures" : "creature",
            totalKilled_7 > 1 ? "perish" : "perishes");
    } else {
        sprintf(gText, "%s %s %d %s.",
            m_quantity > 1 ? gArmyNamesPlural[m_monsterType] : gArmyNames[m_monsterType],
            m_quantity > 1 ? "do" : "does",
            totalDamage_1,
            "damage");
    }
    gText[0] -= ARMY_ASCII_CASE_OFFSET;
    strcpy(combatText_8, gText);
    PowEffect(-1, 0, -1, -1);
    gpCombatManager->CombatMessage(combatText_8, 1, 1, 0);
    gpCombatManager->m_limitCreatureCount[m_side][m_index] = 1;
}

// @early-stop
// Logic, the 0x124 frame, stack slots, switch-body order, and external relocation
// targets align. Excluding the 0x24 local-label jump table, the remaining code
// differences are two side/index evaluation orders, two condition-polarity
// trampolines, and eight five-byte inline continuations. The 0x2b compressed
// case map is byte-exact; all 11 jump-table DIR32 entries agree by case order.
// The two base-only self REL32 entries are the retail-resolved recursive calls;
// remaining relocation identities are strings, gConfig, and constant-pool names.
VA(0x0044e1a1, 0x1267)
void army::DoAttack(int retaliation)
{
    int targetOriginalFacing_5;
    int originalDirection;
    army *breathTarget_6;
    int secondAttackDirection_6;
    int breathDamage;
    int desiredFacing;
    int effectStopsRetaliation_4;
    int originalFacing_6;
    int occupantSide_5;
    char combatText[ARMY_COMBAT_TEXT_SIZE];
    int damage;
    int targetHex_3;
    int killed_13;
    army *target_1;
    int breathKilled;
    int adjacentHex_1;
    int revivedQuantity_3;
    int breathHex_1;

    m_drawState = 3;
    damage = 0;
    killed_13 = 0;
    breathDamage = 0;
    breathKilled = 0;
    effectStopsRetaliation_4 = 0;
    target_1 = 0;
    breathTarget_6 = 0;
    originalFacing_6 = m_facing;
    if (retaliation) {
        gpCombatManager->m_currentSide = 1 - gpCombatManager->m_currentSide;
    }
    if (m_monsterType == ARMY_CREATURE_HYDRA) {
        DoHydraAttack(retaliation);
    } else {
        originalDirection = m_attackDirection;
        targetHex_3 = m_hex;
        if ((m_monster.flags.all & MONSTER_FLAGS_WIDE) &&
            ((m_facing == 0 && m_attackDirection >= 3) ||
             (m_facing == 1 &&
              (m_attackDirection <= 2 || m_attackDirection >= 6)))) {
            if (originalFacing_6 == 0) {
                targetHex_3 = m_hex - 1;
            } else {
                targetHex_3 = m_hex + 1;
            }
        }
        targetHex_3 = GetAdjacentCellIndex(targetHex_3, m_attackDirection);
        target_1 = &gpCombatManager->m_armies
            [gpCombatManager->m_hexCells[targetHex_3].m_occupantSide]
            [gpCombatManager->m_hexCells[targetHex_3].m_occupantIndex];
        if (m_monster.flags.all & MONSTER_FLAGS_BREATH_ATTACK) {
            breathHex_1 = GetAdjacentCellIndex(targetHex_3, m_attackDirection);
            if (ValidHex(breathHex_1) &&
                gpCombatManager->m_hexCells[breathHex_1].m_occupantSide >= 0 &&
                gpCombatManager->m_hexCells[breathHex_1].m_occupantIndex >= 0 &&
                (gpCombatManager->m_hexCells[breathHex_1].m_occupantSide != target_1->m_side ||
                 gpCombatManager->m_hexCells[breathHex_1].m_occupantIndex != target_1->m_index)) {
                breathTarget_6 = &gpCombatManager->m_armies
                    [gpCombatManager->m_hexCells[breathHex_1].m_occupantSide]
                    [gpCombatManager->m_hexCells[breathHex_1].m_occupantIndex];
            }
        }
        gpCombatManager->ResetLimitCreature();
        gpCombatManager->m_limitCreatureCount[m_side][m_index]++;
        gpCombatManager->m_limitCreatureCount[target_1->m_side][target_1->m_index]++;
        if (breathTarget_6) {
            breathTarget_6->m_index[
                gpCombatManager->m_limitCreatureCount[breathTarget_6->m_side]]++;
        }
        gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        targetOriginalFacing_5 = target_1->m_facing;
        if (m_attackDirection <= 2) {
            desiredFacing = 1;
        } else if (m_attackDirection <= 5) {
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
            target_1->m_facing = 1 - m_facing;
            if (target_1->m_facing != targetOriginalFacing_5 &&
                (target_1->m_monster.flags.all & MONSTER_FLAGS_WIDE)) {
                if (target_1->m_facing == 1) {
                    target_1->m_hex--;
                } else {
                    target_1->m_hex++;
                }
            }
        }
        CheckLuck();
        m_animationState = 1;
        if (m_attackDirection == 6 || m_attackDirection == 5 ||
            m_attackDirection == 0) {
            m_pendingAnimationSequence = ARMY_ATTACK_DELAY_SHORT;
        } else if (m_attackDirection == 1 || m_attackDirection == 4) {
            m_pendingAnimationSequence = ARMY_ATTACK_DELAY_NORMAL;
        } else {
            m_pendingAnimationSequence = ARMY_ATTACK_DELAY_LONG;
        }
        if (breathTarget_6) {
            m_pendingAnimationSequence += ARMY_BREATH_ATTACK_DELAY_BONUS;
        }
        gpSoundManager->MemorySample(m_samples[ARMY_SAMPLE_ATTACK]);
        DamageEnemy(target_1, &damage, &killed_13, 0, 0);
        if (breathTarget_6) {
            DamageEnemy(breathTarget_6, &breathDamage, &breathKilled, 0, 0);
        }
        if (damage == -1) {
            sprintf(gText, "The mirror image is destroyed!");
        } else if (gbGenieHalf) {
            sprintf(gText, "%s %s half the enemy troops!",
                m_quantity > 1 ? gArmyNamesPlural[m_monsterType] :
                                 gArmyNames[m_monsterType],
                m_quantity > 1 ? "destroy" : "destroys");
            gText[0] -= ARMY_ASCII_CASE_OFFSET;
        } else {
            if (killed_13 > 0) {
                sprintf(gText, "%s %s %d %s.\n%d %s %s.",
                    m_quantity > 1 ? gArmyNamesPlural[m_monsterType] :
                                     gArmyNames[m_monsterType],
                    m_quantity > 1 ? "do" : "does",
                    damage,
                    "damage",
                    killed_13,
                    killed_13 > 1 ? gArmyNamesPlural[target_1->m_monsterType] :
                                 gArmyNames[target_1->m_monsterType],
                    killed_13 > 1 ? "perish" : "perishes");
                gText[0] -= ARMY_ASCII_CASE_OFFSET;
            } else {
                sprintf(gText, "%s %s %d %s.",
                    m_quantity > 1 ? gArmyNamesPlural[m_monsterType] :
                                     gArmyNames[m_monsterType],
                    m_quantity > 1 ? "do" : "does", damage, "damage");
                gText[0] -= ARMY_ASCII_CASE_OFFSET;
            }
        }
        strcpy(combatText, gText);

        switch (m_monsterType) {
        case ARMY_CREATURE_CYCLOPS:
            if (target_1->m_quantity > 0) {
                if (!breathTarget_6 || breathTarget_6->m_quantity > 0) {
                    if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE) {
                        if (target_1 && target_1->SpellCastWorks(ARMY_ATTACK_SPELL_CYCLOPS)) {
                            target_1->m_spellEffect = ARMY_ATTACK_SPELL_CYCLOPS;
                            effectStopsRetaliation_4 = 1;
                        }
                    } else if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE &&
                               breathTarget_6 &&
                               breathTarget_6->SpellCastWorks(ARMY_ATTACK_SPELL_CYCLOPS)) {
                        breathTarget_6->m_spellEffect = ARMY_ATTACK_SPELL_CYCLOPS;
                    }
                }
            }
            break;
        case ARMY_CREATURE_UNICORN:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target_1 &&
                target_1->SpellCastWorks(ARMY_ATTACK_SPELL_UNICORN)) {
                target_1->m_spellEffect = ARMY_ATTACK_SPELL_UNICORN;
                effectStopsRetaliation_4 = 1;
            }
            break;
        case ARMY_CREATURE_MEDUSA:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target_1 &&
                target_1->SpellCastWorks(ARMY_ATTACK_SPELL_MEDUSA)) {
                target_1->m_spellEffect = ARMY_ATTACK_SPELL_MEDUSA;
                effectStopsRetaliation_4 = 1;
            }
            break;
        case ARMY_CREATURE_MUMMY:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE) {
                goto applyMummySpell;
            }
            break;
        case ARMY_CREATURE_ROYAL_MUMMY:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ROYAL_MUMMY_EFFECT_CHANCE) {
applyMummySpell:
                if (target_1 && target_1->SpellCastWorks(ARMY_ATTACK_SPELL_MUMMY)) {
                    target_1->m_spellEffect = ARMY_ATTACK_SPELL_MUMMY;
                }
            }
            break;
        case ARMY_CREATURE_ARCHMAGE:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target_1 &&
                target_1->SpellCastWorks(ARMY_ATTACK_SPELL_ARCHMAGE)) {
                target_1->m_spellEffect = ARMY_ATTACK_SPELL_ARCHMAGE;
            }
            break;
        case ARMY_CREATURE_GHOST:
            gpCombatManager->m_killBenefit[
                gpCombatManager->m_hexCells[m_hex].m_occupantSide] = killed_13;
            break;
        case ARMY_CREATURE_VAMPIRE_LORD:
            gpCombatManager->m_killBenefit[
                gpCombatManager->m_hexCells[m_hex].m_occupantSide] =
                target_1->m_monster.hitPoints * killed_13;
            break;
        }
        PowEffect(-1, 0, -1, -1);
        gpCombatManager->CombatMessage(combatText, 1, 1, 0);
        m_index[gpCombatManager->m_limitCreatureCount[m_side]] = 1;

        if (m_monsterType == ARMY_CREATURE_GHOST) {
            m_quantity += gpCombatManager->m_killBenefit[
                gpCombatManager->m_hexCells[m_hex].m_occupantSide];
        }
        if (m_monsterType == ARMY_CREATURE_VAMPIRE_LORD) {
            if (gpCombatManager->m_killBenefit[
                    gpCombatManager->m_hexCells[m_hex].m_occupantSide] < m_hitPointsLost) {
                m_hitPointsLost -= gpCombatManager->m_killBenefit[
                    gpCombatManager->m_hexCells[m_hex].m_occupantSide];
            } else {
                gpCombatManager->m_killBenefit[
                    gpCombatManager->m_hexCells[m_hex].m_occupantSide] -= m_hitPointsLost;
                m_hitPointsLost = 0;
                revivedQuantity_3 = gpCombatManager->m_killBenefit[
                    gpCombatManager->m_hexCells[m_hex].m_occupantSide] / m_monster.hitPoints;
                if (revivedQuantity_3 < m_initialQuantity - m_quantity) {
                    m_quantity += revivedQuantity_3;
                } else {
                    m_quantity = m_initialQuantity;
                }
            }
        }

        if (target_1 && target_1->m_quantity > 0 &&
            !target_1->m_spellInfluence[ARMY_SPELL_INFLUENCE_PARALYZE] &&
            !target_1->m_spellInfluence[ARMY_SPELL_INFLUENCE_PETRIFIED] &&
            (target_1->m_monsterType == ARMY_CREATURE_GRIFFIN ||
             !(target_1->m_monster.flags.all & MONSTER_FLAGS_RETALIATED)) &&
            m_monsterType != ARMY_CREATURE_ROGUE &&
            m_monsterType != ARMY_CREATURE_SPRITE &&
            m_monsterType != ARMY_CREATURE_VAMPIRE &&
            m_monsterType != ARMY_CREATURE_VAMPIRE_LORD &&
            !effectStopsRetaliation_4 && !retaliation) {
            DelayMilli(static_cast<long>(
                gfCombatSpeedMod[gConfig.combatSpeed] * ARMY_RETALIATION_DELAY));
            target_1->m_attackDirection = OppositeDirection(m_attackDirection);
            if (target_1->m_monster.flags.all & MONSTER_FLAGS_WIDE) {
                adjacentHex_1 = GetAdjacentCellIndex(
                    target_1->m_hex,
                    (static_cast<unsigned int>(target_1->m_facing - 1) < 1 ? 0 : -1) & 5);
                if (adjacentHex_1 == m_hex) {
                    target_1->m_attackDirection = 6;
                }
                adjacentHex_1 = GetAdjacentCellIndex(
                    target_1->m_hex,
                    static_cast<unsigned int>(target_1->m_facing - 1) < 1 ? 2 : 3);
                if (adjacentHex_1 == m_hex) {
                    target_1->m_attackDirection = 7;
                }
            }
            target_1->DoAttack(1);
            target_1->m_monster.attributes |= MONSTER_ATTRIBUTE_RETALIATED;
            if (gbRemoteOn && gpCombatManager->m_networkArmyPresent[0] &&
                gpCombatManager->m_networkArmyPresent[1] &&
                target_1->m_monsterType == ARMY_CREATURE_GHOST) {
                target_1->m_quantity += gpCombatManager->m_killBenefit[
                    gpCombatManager->m_hexCells[target_1->m_hex].m_occupantSide];
            }
        }
        if ((m_monsterType == ARMY_CREATURE_WOLF ||
             m_monsterType == ARMY_CREATURE_PALADIN ||
             m_monsterType == ARMY_CREATURE_CRUSADER) &&
            target_1 && target_1->m_quantity > 0 && !retaliation &&
            !m_spellInfluence[ARMY_SPELL_INFLUENCE_PARALYZE] &&
            !m_spellInfluence[ARMY_SPELL_INFLUENCE_PETRIFIED] &&
            !m_spellInfluence[ARMY_SPELL_INFLUENCE_BLIND] &&
            m_quantity > 0) {
            DelayMilli(static_cast<long>(
                gfCombatSpeedMod[gConfig.combatSpeed] * ARMY_SECOND_ATTACK_DELAY));
            secondAttackDirection_6 = m_attackDirection;
            m_attackDirection = originalDirection;
            DoAttack(1);
            m_attackDirection = secondAttackDirection_6;
        }
        if (m_facing != originalFacing_6) {
            if (!(m_monster.flags.all & MONSTER_FLAGS_DEAD)) {
                m_facing = originalFacing_6;
                if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
                    if (originalFacing_6 == 1) {
                        m_hex--;
                    } else {
                        m_hex++;
                    }
                }
            }
            if (!(target_1->m_monster.flags.all & MONSTER_FLAGS_DEAD) &&
                target_1->m_facing != targetOriginalFacing_5) {
                target_1->m_facing = targetOriginalFacing_5;
                if (target_1->m_monster.flags.all & MONSTER_FLAGS_WIDE) {
                    if (target_1->m_facing == 1) {
                        target_1->m_hex--;
                    } else {
                        target_1->m_hex++;
                    }
                }
            }
        }
    }
    if (!retaliation &&
        (m_spellInfluence[ARMY_SPELL_INFLUENCE_BERSERK] ||
         m_spellInfluence[ARMY_SPELL_INFLUENCE_HYPNOTIZE])) {
        CancelSpellType(1);
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }
    targetHex_3 = -1;
    m_targetSide = targetHex_3;
    if (retaliation) {
        gpCombatManager->m_currentSide = 1 - gpCombatManager->m_currentSide;
    }
}

VA(0x0044f408, 0x16)
void army::ResetPath(void)
{
    return;
}

VA(0x0044f41e, 0x25)
int army::WalkTo(void)
{
    return WalkTo(m_moveTargetHex);
}

// @early-stop
// Exact 0x2ee-byte match, including all 19 relocation sites.
VA(0x0044f443, 0x2ee)
int army::WalkTo(int destination)
{
    int direction_3;
    int steps;
    int moatDestination;
    int moatIndex_1;
    int canEnterMoat_1;

    m_targetIndex = -1;
    m_targetSide = m_targetIndex;
    if (gpCombatManager->m_drawbridgeBackgroundVisible &&
        (m_monster.flags.all & MONSTER_FLAGS_WIDE)) {
        moatDestination = 0;
        moatIndex_1 = 0;
        for (direction_3 = 0; direction_3 < ARMY_MOAT_CELL_COUNT; direction_3++) {
            if (moatCell[direction_3] == destination) {
                moatDestination = 1;
                moatIndex_1 = direction_3;
            }
        }
        if (moatDestination) {
            canEnterMoat_1 = 0;
            if (moatIndex_1 == ARMY_MOAT_GATE_INDEX &&
                gpCombatManager->m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN) {
                canEnterMoat_1 = 1;
            }
            if ((moatIndex_1 > 0 && moatCell[moatIndex_1 - 1] == m_hex) ||
                (moatIndex_1 < ARMY_MOAT_CELL_COUNT - 1 &&
                 moatCell[moatIndex_1 + 1] == m_hex)) {
                canEnterMoat_1 = 1;
            }
            for (direction_3 = 0; direction_3 < ARMY_ADJACENT_DIRECTION_COUNT;
                 direction_3++) {
                if (moatCell[moatIndex_1] ==
                    GetAdjacentCellIndex(m_hex, direction_3)) {
                    canEnterMoat_1 = 1;
                }
            }
            if (m_side == 0 && moatCell[m_hex / ARMY_HEX_COLUMNS] < m_hex) {
                canEnterMoat_1 = 1;
            }
            if (m_side == 1 && m_hex < moatCell[m_hex / ARMY_HEX_COLUMNS]) {
                canEnterMoat_1 = 1;
            }
            if (!canEnterMoat_1) {
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
    for (direction_3 = gpSearchArray->m_pathLength - 1;
         direction_3 >= 0;
         direction_3--) {
        Walk(static_cast<unsigned char>(
                 gpSearchArray->m_storage.path.directions[direction_3 + 1]),
             0,
             gpSearchArray->m_pathLength - 1 != direction_3);
        steps++;
        if (steps >= m_monster.speed) {
            direction_3 = -1;
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

// @early-stop
// The complete instruction stream and all 18/18 relocation sites align.
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
            m_attackDirection = static_cast<unsigned char>(
                gpSearchArray->m_storage.path.directions[1]);
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
                Walk(static_cast<unsigned char>(
                         gpSearchArray->m_storage.path.directions[pathIndex + 1]),
                     finishStanding,
                     gpSearchArray->m_pathLength - 1 != pathIndex);
                if (steps >= m_monster.speed && pathIndex != 1) {
                    return ARMY_PATH_BLOCKED;
                }
            }
            CancelSpellType(0);
            m_attackDirection = static_cast<unsigned char>(
                gpSearchArray->m_storage.path.directions[1]);
            gpCombatManager->TestRaiseDoor();
            DoAttack(0);
        }
        return 0;
    }
    return ARMY_PATH_BLOCKED;
}

// @early-stop
// The complete instruction stream and all 37 relocation sites align. The
// residual is limited to delinked string and NULL_SAMPLE2 symbol identities.
VA(0x0044f93e, 0x282)
void army::CheckLuck(void)
{
    SAMPLE2 luckSample;
    char *armyName;

    m_luckOutcome = 0;
    if (!gpCombatManager->m_heroes[m_side]) {
        return;
    }
    if (m_luck > 0 && SRandom(1, ARMY_LUCK_ROLL_MAX) <= m_luck) {
        m_luckOutcome = 1;
    }
    if (m_luck < 0 && SRandom(1, ARMY_LUCK_ROLL_MAX) < -m_luck) {
        m_luckOutcome = -1;
    }
    if (m_luckOutcome) {
        luckSample = NULL_SAMPLE2;
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

// @early-stop
// All 41 relocation sites align. The only non-pool code difference is the
// value-equivalent genie clamp operand order at target 0x581f-0x5825; direct,
// reversed, negated, and empty-arm comparison spellings were audited.
VA(0x0044fbc0, 0x56e)
void army::DamageEnemy(army *target, int *damageResult, int *killedResult,
                       int rangedAttack, int defenseModifier)
{
    float damage1;
    int attackBonus6;
    int defenseBonus9;
    int attackDifference7;
    int rearHex19;
    int index16;
    int damageDone2;
    int genieDamage26;
    hero *commander1;

    if (!target) {
        return;
    }
    damage1 = 0;
    gbGenieHalf = 0;
    for (index16 = 0; index16 < m_quantity; index16++) {
        if (m_spellInfluence[ARMY_SPELL_INFLUENCE_BLESS]) {
            damage1 += m_monster.damageMax;
        } else if (m_spellInfluence[ARMY_SPELL_INFLUENCE_CURSE]) {
            damage1 += m_monster.damageMin;
        } else {
            damage1 += SRandom(m_monster.damageMin, m_monster.damageMax);
        }
    }
    attackBonus6 = 0;
    defenseBonus9 = 0;
    attackDifference7 = m_monster.attack + attackBonus6 -
        (target->m_monster.defense + (defenseBonus9 + defenseModifier));
    if (m_spellInfluence[ARMY_SPELL_INFLUENCE_DRAGON_SLAYER] &&
        (target->m_monsterType == ARMY_CREATURE_GREEN_DRAGON ||
         target->m_monsterType == ARMY_CREATURE_RED_DRAGON ||
         target->m_monsterType == ARMY_CREATURE_BLACK_DRAGON ||
         target->m_monsterType == ARMY_CREATURE_BONE_DRAGON)) {
        attackDifference7 += ARMY_DRAGON_SLAYER_BONUS;
    }
    if (gpCombatManager->m_drawbridgeBackgroundVisible) {
        rearHex19 = -1;
        if (target->m_monster.flags.all & MONSTER_FLAGS_WIDE) {
            rearHex19 =
                (static_cast<unsigned int>(target->m_facing - 1) < 1 ? 1 : -1) +
                target->m_hex;
        }
        for (index16 = 0; index16 < ARMY_MOAT_CELL_COUNT; index16++) {
            if (moatCell[index16] == target->m_hex ||
                moatCell[index16] == rearHex19) {
                attackDifference7 += ARMY_MOAT_ATTACK_BONUS;
            }
        }
    }
    if (attackDifference7 > ARMY_DAMAGE_STAT_LIMIT) {
        attackDifference7 = ARMY_DAMAGE_STAT_LIMIT;
    }
    if (attackDifference7 < -ARMY_DAMAGE_STAT_LIMIT) {
        attackDifference7 = -ARMY_DAMAGE_STAT_LIMIT;
    }
    damage1 *= gfBattleStat[attackDifference7 + ARMY_DAMAGE_STAT_LIMIT];
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
        damage1 *= 2.0f;
    }
    if (m_luckOutcome > 0) {
        damage1 *= 2.0f;
    }
    if (m_luckOutcome < 0) {
        damage1 /= 2.0f;
    }
    m_luckOutcome = 0;
    if (rangedAttack && gpCombatManager->ShotIsThroughWall(m_side, m_hex, target->m_hex)) {
        damage1 /= 2.0f;
    }
    commander1 = gpCombatManager->m_heroes[m_side];
    if (commander1 && rangedAttack) {
        damage1 *=
            gfSSArcheryMod[commander1->m_secondarySkills[HERO_SKILL_ARCHERY]];
    }
    if ((m_monster.flags.all & MONSTER_FLAGS_SHOOTER) && !rangedAttack &&
        m_monsterType != ARMY_CREATURE_TITAN &&
        m_monsterType != ARMY_CREATURE_MAGE &&
        m_monsterType != ARMY_CREATURE_ARCHMAGE) {
        damage1 /= 2.0f;
    }
    if (rangedAttack &&
        target->m_spellInfluence[ARMY_SPELL_INFLUENCE_SHIELD]) {
        damage1 /= 2.0f;
    }
    if (m_damagePenalty == 2) {
        damage1 /= 2.0f;
    }
    m_damagePenalty = 0;
    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_PETRIFIED]) {
        damage1 /= 2.0f;
    }
    damageDone2 = static_cast<int>(damage1 + 0.5);
    if (m_monsterType == ARMY_CREATURE_GENIE &&
        SRandom(1, ARMY_GENIE_HALF_ROLL_MAX) == ARMY_GENIE_HALF_ROLL) {
        genieDamage26 =
            ((target->m_quantity + 1) / 2) * target->m_monster.hitPoints;
        if (damageDone2 < genieDamage26) {
            gbGenieHalf = 1;
            damageDone2 = genieDamage26;
        }
    }
    if (damageDone2 <= 0) {
        damageDone2 = 1;
    }
    if (target->m_monster.flags.all & MONSTER_FLAGS_MIRROR_IMAGE) {
        damageDone2 = -1;
    }
    *damageResult = damageDone2;
    *killedResult = target->Damage(damageDone2, -1);
}

// @early-stop
// The complete 159-instruction stream and all 15/15 relocations align.
VA(0x0045012e, 0x23c)
int army::Damage(long damage, int spell)
{
    int killed;
    int originalFacing;
    int quantityFifth;

    damage += m_hitPointsLost;
    if (spell != -1) {
        if (gbRemoteOn) {
            gpCombatManager->ModifyDamageForArtifacts(
                &damage, spell,
                gpCombatManager->m_heroes[m_side],
                gpCombatManager->m_heroes[gpCombatManager->m_currentSide]);
        } else {
            gpCombatManager->ModifyDamageForArtifacts(
                &damage, spell,
                gpCombatManager->m_heroes[gpCombatManager->m_currentSide],
                gpCombatManager->m_heroes[m_side]);
        }
    }
    killed = damage / m_monster.hitPoints;
    m_hitPointsLost = damage % m_monster.hitPoints;
    quantityFifth = m_quantity / 5;
    if (m_monster.flags.all & MONSTER_FLAGS_MIRROR_IMAGE) {
        killed = m_quantity;
        m_hitPointsLost = 0;
    }
    if (!quantityFifth) {
        quantityFifth = 1;
    }
    m_damagePending = 1;
    if (killed > 0) {
        m_killPending = 1;
        m_lastTargetHex = m_quantity;
    }
    if (killed > m_quantity) {
        killed = m_quantity;
    }
    m_quantity -= killed;
    if (m_quantity <= 0) {
        m_deathPending = 1;
    }
    originalFacing = m_facing;
    m_facing = gpCombatManager
        ->m_armies[gpCombatManager->m_currentArmySide]
                   [gpCombatManager->m_currentArmyIndex].m_facing ^ 1;
    m_facing = originalFacing;
    CancelSpellType(ARMY_CANCEL_SPELLS_AFTER_DAMAGE);
    return killed;
}

// @early-stop
// The complete instruction stream and all 119 relocation sites align. The only
// residual is two delinked gConfig relocation identities.
VA(0x0045036a, 0x1361)
void army::PowEffect(int effect, int resetLimits, int effectX, int effectY)
{
    int damageFrames;
    int side_4;
    int maximumDamageFrames_3;
    army *current;
    int drawEffect_1;
    int effectFrames_1;
    int index_10;
    int overlapAdjustment_7;
    int frame;
    int maximumEndFrames_1;
    int keepAnimating_1;
    int endFrames_1;
    int maximumStartFrames;
    int totalFrames_4;
    int frameDelay_6;
    int startFrames_5;
    IconEntry *entry_1;

    maximumStartFrames = 0;
    maximumEndFrames_1 = 0;
    maximumDamageFrames_3 = 0;
    startFrames_5 = 0;
    endFrames_1 = 0;
    damageFrames = 0;
    effectFrames_1 = 0;
    totalFrames_4 = 0;
    drawEffect_1 = 0;
    overlapAdjustment_7 = 1;
    if (m_monsterType == ARMY_CREATURE_PALADIN ||
        m_monsterType == ARMY_CREATURE_CRUSADER) {
        overlapAdjustment_7 = 0;
    }
    if (m_monsterType == ARMY_CREATURE_DWARF ||
        m_monsterType == ARMY_CREATURE_BATTLE_DWARF) {
        overlapAdjustment_7 = 2;
    }
    if (effectX != ARMY_NO_EFFECT) {
        drawEffect_1 = 1;
    } else if (effect != ARMY_NO_EFFECT) {
        for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
            for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
                if (gpCombatManager->m_armies[side_4][index_10].m_drawSpellEffect) {
                    drawEffect_1 = 1;
                }
            }
        }
    }
    if (!gbNoShowCombat && effect != ARMY_NO_EFFECT && drawEffect_1 &&
        gCurLoadedSpellEffect != 0[&effect]) {
        gpResourceManager->Dispose(gCurLoadedSpellIcon);
        gCurLoadedSpellIcon = gpResourceManager->GetIcon(gCombatFxNames[effect]);
        gCurLoadedSpellEffect = effect;
    }
    if (drawEffect_1) {
        effectFrames_1 = giNumPowFrames[gCurLoadedSpellEffect];
    }
        for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
        for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
            current = &gpCombatManager->m_armies[side_4][index_10];
            if (static_cast<unsigned char>(current->m_animationState)) {
                startFrames_5 = current->m_frameInfo.animationFrameCount[
                    m_pendingAnimationSequence];
                endFrames_1 = current->m_frameInfo.animationFrameCount[
                    m_pendingAnimationSequence + 1] + 1;
            } else {
                if (current->m_deathPending) {
                    damageFrames = current->m_frameInfo.animationFrameCount[
                        ARMY_ANIMATION_DEATH];
                } else if (current->m_damagePending) {
                    damageFrames =
                        current->m_frameInfo.animationFrameCount[ARMY_ANIMATION_WINCE] +
                        current->m_frameInfo.animationFrameCount[
                            ARMY_ANIMATION_WINCE_RETURN] + 1;
                }
            }
            maximumStartFrames = maximumStartFrames > startFrames_5
                ? maximumStartFrames : startFrames_5;
            maximumEndFrames_1 = maximumEndFrames_1 > endFrames_1
                ? maximumEndFrames_1 : endFrames_1;
            maximumDamageFrames_3 = maximumDamageFrames_3 > damageFrames
                ? maximumDamageFrames_3 : damageFrames;
        }
    }
    totalFrames_4 = maximumStartFrames + maximumEndFrames_1;
    totalFrames_4 = maximumStartFrames + maximumDamageFrames_3 - overlapAdjustment_7 >
            totalFrames_4
        ? maximumStartFrames + maximumDamageFrames_3 - overlapAdjustment_7
        : totalFrames_4;
    totalFrames_4 = maximumDamageFrames_3 > totalFrames_4
        ? maximumDamageFrames_3 : totalFrames_4;
    effectFrames_1 = effectFrames_1 > totalFrames_4
        ? effectFrames_1 : totalFrames_4;
    frameDelay_6 = ARMY_POW_EFFECT_DELAY;
    if (resetLimits) {
        gpCombatManager->ResetLimitCreature();
    }
    for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
        for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
            if (gpCombatManager->m_armies[side_4][index_10].m_animationSequence ==
                    ARMY_ANIMATION_SHOOT_UP ||
                gpCombatManager->m_armies[side_4][index_10].m_animationSequence ==
                    ARMY_ANIMATION_SHOOT_FORWARD ||
                gpCombatManager->m_armies[side_4][index_10].m_animationSequence ==
                    ARMY_ANIMATION_SHOOT_DOWN) {
                gpCombatManager->m_armies[side_4][index_10].m_animationCycle = 1;
            } else {
                gpCombatManager->m_armies[side_4][index_10].m_animationCycle = 0;
            }
            if ((gpCombatManager->m_armies[side_4][index_10].m_damagePending ||
                 static_cast<unsigned char>(
                     gpCombatManager->m_armies[side_4][index_10].m_animationState) ||
                 static_cast<unsigned char>(
                     gpCombatManager->m_armies[side_4][index_10].m_animationCycle)) &&
                !*reinterpret_cast<int *>(
                    index_10 * sizeof(gpCombatManager->m_limitCreatureCount[0][0]) +
                    0[&side_4] * sizeof(gpCombatManager->m_limitCreatureCount[0]) +
                    reinterpret_cast<char *>(
                        gpCombatManager->m_limitCreatureCount))) {
                (*reinterpret_cast<int *>(
                    index_10 * sizeof(gpCombatManager->m_limitCreatureCount[0][0]) +
                    0[&side_4] * sizeof(gpCombatManager->m_limitCreatureCount[0]) +
                    reinterpret_cast<char *>(
                        gpCombatManager->m_limitCreatureCount)))++;
            }
        }
    }
    gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    if (effectX != ARMY_NO_EFFECT) {
        for (index_10 = 0; index_10 < gCurLoadedSpellIcon->m_frameCount; index_10++) {
            entry_1 = reinterpret_cast<IconEntry *>(
                index_10 * sizeof(IconEntry) + gCurLoadedSpellIcon->m_data);
            giMinExtentX = entry_1->x + effectX < giMinExtentX
                ? entry_1->x + effectX : giMinExtentX;
            giMinExtentY = entry_1->y + effectY < giMinExtentY
                ? entry_1->y + effectY : giMinExtentY;
            giMaxExtentX = entry_1->x + entry_1->w + effectX - 1 > giMaxExtentX
                ? entry_1->x + entry_1->w + effectX - 1 : giMaxExtentX;
            giMaxExtentY = entry_1->y + entry_1->h + effectY - 1 > giMaxExtentY
                ? entry_1->y + entry_1->h + effectY - 1 : giMaxExtentY;
        }
        giMinExtentX = giMinExtentX > 0 ? giMinExtentX : 0;
        giMinExtentY = giMinExtentY > 0 ? giMinExtentY : 0;
        giMaxExtentX = giMaxExtentX < ARMY_COMBAT_MAX_X
            ? giMaxExtentX : ARMY_COMBAT_MAX_X;
        giMaxExtentY = giMaxExtentY < ARMY_COMBAT_MAX_Y
            ? giMaxExtentY : ARMY_COMBAT_MAX_Y;
    }
    for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
        for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
            current = &gpCombatManager->m_armies[side_4][index_10];
            current->m_effectAnimationStart = -1;
            current->m_effectAnimationEnd = -1;
            current->m_effectAnimationStarted = 0;
            if (current->m_damagePending ||
                static_cast<unsigned char>(current->m_animationState)) {
                if (static_cast<unsigned char>(current->m_animationState)) {
                    current->m_effectAnimationStart = m_pendingAnimationSequence;
                    current->m_effectAnimationEnd =
                        m_pendingAnimationSequence + 1;
                } else if (current->m_deathPending) {
                    current->m_effectAnimationStart = ARMY_ANIMATION_DEATH;
                } else {
                    current->m_effectAnimationStart = ARMY_ANIMATION_WINCE;
                    current->m_effectAnimationEnd = ARMY_ANIMATION_WINCE_RETURN;
                }
                if (current->m_effectAnimationStart == ARMY_ANIMATION_DEATH) {
                    current->m_effectAnimationLength =
                        current->m_frameInfo.animationFrameCount[ARMY_ANIMATION_DEATH];
                } else {
                    current->m_effectAnimationLength =
                        current->m_frameInfo.animationFrameCount[
                            current->m_effectAnimationStart] +
                        current->m_frameInfo.animationFrameCount[
                            current->m_effectAnimationStart + 1];
                }
                if (current->m_animationSequence == current->m_effectAnimationStart) {
                    current->m_effectAnimationLength--;
                }
                if (m_drawState < 2) {
                    m_drawState = 2;
                }
            }
        }
    }
    frame = 0;
    for (frame = 0; 0[&effectFrames_1] > frame; frame++) {
        for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
            for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
                current = &gpCombatManager->m_armies[side_4][index_10];
                if (static_cast<unsigned char>(current->m_animationCycle)) {
                    if (current->m_animationSequence == ARMY_ANIMATION_SHOOT_UP ||
                        current->m_animationSequence == ARMY_ANIMATION_SHOOT_FORWARD ||
                        current->m_animationSequence == ARMY_ANIMATION_SHOOT_DOWN) {
                        current->m_animationSequence++;
                        current->m_animationFrame = 0;
                    } else if (current->m_animationSequence != ARMY_ANIMATION_STAND) {
                        if (current->m_animationFrame + 1 <
                            current->m_frameInfo.animationFrameCount[
                                current->m_animationSequence]) {
                            current->m_animationFrame++;
                        } else {
                            current->m_animationSequence = ARMY_ANIMATION_STAND;
                            current->m_animationFrame = 0;
                        }
                    }
                }
                if (current->m_effectAnimationStart != -1 &&
                    !current->m_effectAnimationStarted &&
                    (static_cast<unsigned char>(current->m_animationState) ||
                     static_cast<int>(effectFrames_1 - frame - 1) <=
                         current->m_effectAnimationLength ||
                     (maximumStartFrames && frame >= maximumStartFrames - 1) ||
                     (!maximumStartFrames &&
                      current->m_animationSequence != ARMY_ANIMATION_WINCE_RETURN &&
                      (current->m_animationSequence != ARMY_ANIMATION_WINCE ||
                       current->m_animationFrame + 1 <
                           current->m_frameInfo.animationFrameCount[
                               current->m_animationSequence])))) {
                    if (current->m_animationSequence != current->m_effectAnimationStart &&
                        current->m_animationSequence != current->m_effectAnimationEnd) {
                        if (!gbNoShowCombat && current->m_effectAnimationStart ==
                                ARMY_ANIMATION_WINCE) {
                            gpSoundManager->MemorySample(
                                gpCombatManager->m_armies[side_4][index_10]
                                    .m_samples[ARMY_SAMPLE_WINCE]);
                        }
                        if (!gbNoShowCombat && current->m_effectAnimationStart ==
                                ARMY_ANIMATION_DEATH) {
                            gpSoundManager->MemorySample(
                                gpCombatManager->m_armies[side_4][index_10]
                                    .m_samples[ARMY_SAMPLE_KILL]);
                        }
                        current->m_animationSequence = current->m_effectAnimationStart;
                        current->m_animationFrame = 0;
                    } else {
                        if (current->m_animationFrame + 1 <
                            current->m_frameInfo.animationFrameCount[
                                current->m_animationSequence]) {
                            current->m_animationFrame++;
                        } else if (current->m_animationSequence !=
                                       current->m_effectAnimationEnd &&
                                   current->m_effectAnimationEnd != -1) {
                            current->m_animationSequence = current->m_effectAnimationEnd;
                            current->m_animationFrame = 0;
                        } else {
                            if (current->m_animationSequence != ARMY_ANIMATION_STAND &&
                                current->m_animationSequence != ARMY_ANIMATION_DEATH) {
                                current->m_animationSequence = ARMY_ANIMATION_STAND;
                                current->m_animationFrame = 0;
                                current->m_effectAnimationStarted = 1;
                            }
                        }
                    }
                }
            }
        }
        glTimers[0] = static_cast<int>(KBTickCount() +
            frameDelay_6 * gfCombatSpeedMod[gConfig.combatSpeed]);
        if (drawEffect_1 && frame < giNumPowFrames[gCurLoadedSpellEffect]) {
            gCurSpellEffectFrame = frame;
        }
        gpCombatManager->DrawFrame(0, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        if (effectX != ARMY_NO_EFFECT &&
            frame < giNumPowFrames[gCurLoadedSpellEffect]) {
            gCurLoadedSpellIcon->CombatClipDrawToBuffer(
                effectX, m_spellEffectYOffset + effectY, gCurSpellEffectFrame,
                &m_spellLimits, 0, 0, 0, 0);
        }
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX, giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1);
    }
    if (!gbNoShowCombat) {
        WaitSample(ARMY_SAMPLE_ATTACK);
    }
    for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
        for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
            current = &gpCombatManager->m_armies[side_4][index_10];
            if (current->m_damagePending && current->m_spellEffect != ARMY_NO_EFFECT &&
                current->m_spellEffect != ARMY_DELAYED_MEDUSA_EFFECT) {
                gpCombatManager->CastSpell(
                    current->m_spellEffect, current->m_hex, 1, ARMY_NO_EFFECT);
                current->m_spellEffect = ARMY_NO_EFFECT;
            }
        }
    }
    keepAnimating_1 = 1;
    while (keepAnimating_1) {
        keepAnimating_1 = 0;
        for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
            for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
                current = &gpCombatManager->m_armies[side_4][index_10];
                if (current->m_animationSequence == 14 ||
                    current->m_animationSequence == 16 ||
                    current->m_animationSequence == 20 ||
                    current->m_animationSequence == 24 ||
                    current->m_animationSequence == 18 ||
                    current->m_animationSequence == 22 ||
                    current->m_animationSequence == 26 ||
                    current->m_animationSequence == 28 ||
                    current->m_animationSequence == 30 ||
                    current->m_animationSequence == 32) {
                    current->m_animationSequence++;
                    current->m_animationFrame = 0;
                    keepAnimating_1 = 1;
                } else if (current->m_animationSequence == 13 ||
                           current->m_animationSequence == 15 ||
                           current->m_animationSequence == 17 ||
                           current->m_animationSequence == 21 ||
                           current->m_animationSequence == 25 ||
                           current->m_animationSequence == 19 ||
                           current->m_animationSequence == 23 ||
                           current->m_animationSequence == 27 ||
                           current->m_animationSequence == 29 ||
                           current->m_animationSequence == 31 ||
                           current->m_animationSequence == 33) {
                    if (current->m_animationFrame + 1 <
                        current->m_frameInfo.animationFrameCount[
                            current->m_animationSequence]) {
                        current->m_animationFrame++;
                        keepAnimating_1 = 1;
                    } else if (current->m_animationSequence != ARMY_ANIMATION_DEATH) {
                        current->m_animationSequence = ARMY_ANIMATION_STAND;
                        current->m_animationFrame = 0;
                        keepAnimating_1 = 1;
                    }
                }
            }
        }
        if (keepAnimating_1) {
            glTimers[0] = static_cast<int>(KBTickCount() +
                frameDelay_6 * gfCombatSpeedMod[gConfig.combatSpeed]);
            gpCombatManager->DrawFrame(1, 1, 0, 0,
                                       ARMY_COMBAT_FRAME_DELAY, 1, 1);
        }
    }
    if (resetLimits) {
        gpCombatManager->ResetLimitCreature();
    }
    memset(gpCombatManager->m_removedArmies, 0,
           sizeof(gpCombatManager->m_removedArmies));
    gpCombatManager->m_removedArmyPresent = 0;
    for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
        for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
            current = &gpCombatManager->m_armies[side_4][index_10];
            if (current->m_deathPending) {
                current->ProcessDeath(0);
            }
        }
    }
    if (gpCombatManager->m_removedArmyPresent) {
        gpCombatManager->MakeCreaturesVanish();
    }
    for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
        for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
            current = &gpCombatManager->m_armies[side_4][index_10];
            if (current->m_damagePending &&
                current->m_spellEffect == ARMY_DELAYED_MEDUSA_EFFECT) {
                gpCombatManager->CastSpell(
                    current->m_spellEffect, current->m_hex, 1, ARMY_NO_EFFECT);
                current->m_spellEffect = ARMY_NO_EFFECT;
            }
            current->m_drawSpellEffect = 0;
            current->m_damagePending = 0;
            current->m_killPending = 0;
            current->m_drawState = 1;
            current->m_animationState = 0;
            current->m_lastTargetHex = -1;
        }
    }
    gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
        for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
            gpCombatManager->m_armies[side_4][index_10].WaitSample(ARMY_SAMPLE_WINCE);
        }
    }
}

VA(0x004516cb, 0x35)
unsigned long int army::Strength(void)
{
    return gMonsterDatabase[m_monsterType].fightValue * m_quantity;
}

VA(0x00451700, 0x66)
int army::LeaveNoBody(void)
{
    return m_monsterType == ARMY_CREATURE_EARTH_ELEMENTAL ||
        m_monsterType == ARMY_CREATURE_AIR_ELEMENTAL ||
        m_monsterType == ARMY_CREATURE_FIRE_ELEMENTAL ||
        m_monsterType == ARMY_CREATURE_WATER_ELEMENTAL ||
        (m_monster.flags.all & MONSTER_FLAGS_MIRROR_IMAGE);
}

// @early-stop
// All executable bytes align after masking the recursive call operand at
// +0x3e5..+0x3e8; retail binds that call directly to this TU-local function.
VA(0x00451766, 0x3f5)
void army::ProcessDeath(int immediate)
{
    int rearHex;
    army *mirrorImage_4;
    army *mirrorSource;
    hexcell *frontCell_1;
    hexcell *rearCell;

    if (m_monster.flags.all & MONSTER_FLAGS_DEAD) {
        return;
    }
    if (Random(0, ARMY_DEATH_RANDOM_MAX) < ARMY_DEATH_PRIMARY_CHANCE) {
        gpCombatManager->m_heroDeathPending[m_side] = 1;
    } else if (Random(0, ARMY_DEATH_RANDOM_MAX) < ARMY_DEATH_SECONDARY_CHANCE) {
        gpCombatManager->m_heroAlternateDeathPending[1 - m_side] = 1;
    }
    m_monster.attributes |= MONSTER_ATTRIBUTE_DEAD;
    m_deathPending = 0;
    frontCell_1 = &gpCombatManager->m_hexCells[m_hex];
    rearHex = 0;
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        rearHex = (static_cast<unsigned int>(m_facing - 1) < 1 ? 1 : -1) +
            m_hex;
        rearCell = &gpCombatManager->m_hexCells[rearHex];
    } else {
        rearCell = 0;
    }
    if (LeaveNoBody()) {
        if (immediate ||
            (m_monsterType != ARMY_CREATURE_AIR_ELEMENTAL &&
             m_monsterType != ARMY_CREATURE_FIRE_ELEMENTAL &&
             m_monsterType != ARMY_CREATURE_WATER_ELEMENTAL &&
             m_monsterType != ARMY_CREATURE_EARTH_ELEMENTAL)) {
            gpCombatManager->m_removedArmies[m_side][m_index] = 1;
            gpCombatManager->m_removedArmyPresent = 1;
        } else {
            frontCell_1->m_occupantSide = -1;
            frontCell_1->m_occupantIndex = -1;
        }
    }
    if (frontCell_1->m_deadOccupantCount < ARMY_CORPSE_LIMIT && !LeaveNoBody() &&
        (!rearCell || rearCell->m_deadOccupantCount < ARMY_CORPSE_LIMIT)) {
        frontCell_1->m_deadOccupantSides[frontCell_1->m_deadOccupantCount] =
            gpCombatManager->m_hexCells[m_hex].m_occupantSide;
        frontCell_1->m_deadOccupantIndices[frontCell_1->m_deadOccupantCount] =
            gpCombatManager->m_hexCells[m_hex].m_occupantIndex;
        frontCell_1->m_deadOccupantFrames[frontCell_1->m_deadOccupantCount] =
            gpCombatManager->m_hexCells[m_hex].m_occupantFrame;
        frontCell_1->m_deadOccupantCount++;
        if (rearCell) {
            rearCell->m_deadOccupantSides[rearCell->m_deadOccupantCount] =
                gpCombatManager->m_hexCells[rearHex].m_occupantSide;
            rearCell->m_deadOccupantIndices[rearCell->m_deadOccupantCount] =
                gpCombatManager->m_hexCells[rearHex].m_occupantIndex;
            rearCell->m_deadOccupantFrames[rearCell->m_deadOccupantCount] =
                gpCombatManager->m_hexCells[rearHex].m_occupantFrame;
            rearCell->m_deadOccupantCount++;
        }
    }
    if (!LeaveNoBody()) {
        frontCell_1->m_occupantSide = -1;
        frontCell_1->m_occupantIndex = -1;
        if (rearCell) {
            rearCell->m_occupantSide = -1;
            rearCell->m_occupantIndex = -1;
        }
    }
    if (m_mirrorSourceIndex != -1) {
        mirrorSource = &gpCombatManager->m_armies[m_side][m_mirrorSourceIndex];
        mirrorSource->m_mirrorImageIndex = -1;
    }
    if (m_mirrorImageIndex != -1) {
        mirrorImage_4 = &gpCombatManager->m_armies[m_side][m_mirrorImageIndex];
        mirrorImage_4->m_quantity = 0;
        mirrorImage_4->ProcessDeath(0);
    }
}

// @early-stop
// The complete instruction stream and all 54 relocation sites align. The
// residual is limited to delinked gConfig and glTimers symbol identities.
VA(0x00451b5b, 0x39d)
void army::SpellEffect(int effect, int effectFrameDelay, int animateCreature)
{
    unsigned long effectFileId;
    IconEntry *entry;
    int frame;
    int minimumYOffset;
    int powBaseY;
    int frameDelay;
    int i;

    effectFileId = MAKEFILEID(gCombatFxNames[effect]);
    if (m_animationSequence == ARMY_ANIMATION_WINCE ||
        m_animationSequence == ARMY_ANIMATION_WINCE_RETURN) {
        animateCreature = 0;
    }
    if (!gbNoShowCombat) {
        if (effect != gCurLoadedSpellEffect) {
            gpResourceManager->Dispose(gCurLoadedSpellIcon);
            gCurLoadedSpellIcon = gpResourceManager->GetIcon(effectFileId);
            gCurLoadedSpellEffect = effect;
        }
    }
    frame = 0;
    m_drawSpellEffect = 1;
    m_spellEffectYOffset = 0;
    if (!gbNoShowCombat) {
        minimumYOffset = ARMY_EFFECT_MINIMUM_Y;
        for (i = 0; i < gCurLoadedSpellIcon->m_frameCount; i++) {
            entry = GetIconEntry(gCurLoadedSpellIcon, i);
            if (entry->y < minimumYOffset) {
                minimumYOffset = entry->y;
            }
        }
        powBaseY = GetPowBaseY();
        powBaseY += minimumYOffset;
        if (powBaseY < 0) {
            m_spellEffectYOffset = -powBaseY;
        }
        if (animateCreature) {
            frameDelay = ARMY_COMBAT_FRAME_DELAY /
                static_cast<signed char>(
                    m_frameInfo.animationFrameCount[ARMY_ANIMATION_WINCE]);
            m_animationSequence = ARMY_ANIMATION_WINCE;
            for (; frame < static_cast<signed char>(
                       m_frameInfo.animationFrameCount[ARMY_ANIMATION_WINCE]);
                 frame++) {
                m_animationFrame = frame;
                if (frame < giNumPowFrames[effect]) {
                    gCurSpellEffectFrame = frame;
                } else {
                    gCurSpellEffectFrame = giNumPowFrames[effect];
                }
                glTimers[1] = static_cast<int>(KBTickCount() +
                    gfCombatSpeedMod[gConfig.combatSpeed] * frameDelay);
                gpCombatManager->DrawFrame(1, 0, 0, 0,
                                           ARMY_COMBAT_FRAME_DELAY, 1, 1);
                DelayTil(&glTimers[1]);
            }
        }
        for (; frame < giNumPowFrames[effect]; frame++) {
            glTimers[1] = static_cast<int>(KBTickCount() +
                gfCombatSpeedMod[gConfig.combatSpeed] * effectFrameDelay);
            gCurSpellEffectFrame = frame;
            gpCombatManager->DrawFrame(1, 0, 0, 0,
                                       ARMY_COMBAT_FRAME_DELAY, 1, 1);
            DelayTil(&glTimers[1]);
        }
    }
    m_drawSpellEffect = 0;
    if (!gbNoShowCombat) {
        if (animateCreature) {
            frameDelay = ARMY_COMBAT_FRAME_DELAY /
                static_cast<signed char>(
                    m_frameInfo.animationFrameCount[
                        ARMY_ANIMATION_WINCE_RETURN]);
            m_animationSequence = ARMY_ANIMATION_WINCE_RETURN;
            for (frame = 0;
                 frame < static_cast<signed char>(
                     m_frameInfo.animationFrameCount[ARMY_ANIMATION_WINCE_RETURN]);
                 frame++) {
                m_animationFrame = frame;
                glTimers[1] = static_cast<int>(KBTickCount() +
                    gfCombatSpeedMod[gConfig.combatSpeed] * frameDelay);
                gpCombatManager->DrawFrame(1, 0, 0, 0,
                                           ARMY_COMBAT_FRAME_DELAY, 1, 1);
                DelayTil(&glTimers[1]);
            }
            m_animationSequence = ARMY_ANIMATION_STAND;
            m_animationFrame = 0;
            gpCombatManager->DrawFrame(1, 0, 0, 0,
                                       ARMY_COMBAT_FRAME_DELAY, 1, 1);
        } else {
            gpCombatManager->DrawFrame(1, 0, 0, 0,
                                       ARMY_COMBAT_FRAME_DELAY, 1, 1);
        }
    }
}

// @early-stop
// All non-table bytes match. The table at +0xf3..+0x102 contains four
// function-local label relocations; its dispatch and all five calls also align.
VA(0x00451ef8, 0x10f)
void army::CancelSpellType(int cancelType)
{
    switch (cancelType) {
    case ARMY_CANCEL_SPELLS_AFTER_MOVE:
        break;
    case ARMY_CANCEL_SPELLS_AFTER_ATTACK:
        CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BERSERK);
        CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HYPNOTIZE);
        break;
    case ARMY_CANCEL_SPELLS_AFTER_DAMAGE:
        if (m_spellInfluence[ARMY_SPELL_INFLUENCE_BLIND]) {
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BLIND);
            m_damagePenalty = ARMY_DAMAGE_PENALTY_HALF;
            m_monster.attributes |= MONSTER_ATTRIBUTE_WOKE_FROM_DAMAGE;
        }
        if (m_spellInfluence[ARMY_SPELL_INFLUENCE_PARALYZE] ||
            m_spellInfluence[ARMY_SPELL_INFLUENCE_HYPNOTIZE] ||
            m_spellInfluence[ARMY_SPELL_INFLUENCE_PETRIFIED]) {
            m_monster.attributes |= MONSTER_ATTRIBUTE_WOKE_FROM_DAMAGE;
            m_monster.attributes |= MONSTER_ATTRIBUTE_RETALIATED;
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_PARALYZE);
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_PETRIFIED);
        }
        break;
    case ARMY_CANCEL_SPELLS_UNUSED:
        break;
    }
}

// @early-stop
// All non-table bytes match. The table at +0x130..+0x16b contains only 15
// function-local label relocations; its dispatch and database field also align.
VA(0x00452007, 0x178)
void army::CancelIndividualSpell(int influence)
{
    if (!m_spellInfluence[influence]) {
        return;
    }
    m_spellCount--;
    m_spellInfluence[influence] = 0;
    switch (influence) {
    case ARMY_SPELL_INFLUENCE_HASTE:
    case ARMY_SPELL_INFLUENCE_SLOW:
        m_monster.speed = static_cast<signed char>(m_speed);
        m_frameInfo.walkDuration = m_walkDuration;
        m_monster.attributes |=
            gMonsterDatabase[m_monsterType].attributes & MONSTER_ATTRIBUTE_FLYING;
        break;
    case ARMY_SPELL_INFLUENCE_BLIND:
        break;
    case ARMY_SPELL_INFLUENCE_BLESS:
        break;
    case ARMY_SPELL_INFLUENCE_CURSE:
        break;
    case ARMY_SPELL_INFLUENCE_BERSERK:
        break;
    case ARMY_SPELL_INFLUENCE_PARALYZE:
        break;
    case ARMY_SPELL_INFLUENCE_HYPNOTIZE:
        break;
    case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER:
        break;
    case ARMY_SPELL_INFLUENCE_BLOODLUST:
        m_monster.attack -= ARMY_BLOODLUST_ATTACK_BONUS;
        break;
    case ARMY_SPELL_INFLUENCE_SHIELD:
        break;
    case ARMY_SPELL_INFLUENCE_PETRIFIED:
        break;
    case ARMY_SPELL_INFLUENCE_ANTI_MAGIC:
        break;
    case ARMY_SPELL_INFLUENCE_STONESKIN:
        m_monster.defense -= ARMY_STONESKIN_DEFENSE_BONUS;
        break;
    case ARMY_SPELL_INFLUENCE_STEELSKIN:
        m_monster.defense -= ARMY_STEELSKIN_DEFENSE_BONUS;
        break;
    }
}

// @early-stop
// All non-table bytes match. The table at +0x21c..+0x257 contains only 15
// function-local label relocations; its dispatch and all 12 externals also align.
VA(0x0045217f, 0x282)
int army::SetSpellInfluence(int influence, int rounds)
{
    int i;

    if (m_spellInfluence[influence]) {
        if (m_spellInfluence[influence] < rounds) {
            m_spellInfluence[influence] = static_cast<unsigned char>(rounds);
        }
        return 0;
    }
    switch (influence) {
    case ARMY_SPELL_INFLUENCE_HASTE:
        CancelIndividualSpell(ARMY_SPELL_INFLUENCE_SLOW);
        m_monster.speed += ARMY_HASTE_SPEED_BONUS;
        m_frameInfo.walkDuration = static_cast<int>(
            m_frameInfo.walkDuration * ARMY_HASTE_WALK_DURATION_SCALE);
        break;
    case ARMY_SPELL_INFLUENCE_SLOW:
        CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HASTE);
        m_monster.speed = static_cast<signed char>((m_monster.speed + 1) / 2);
        if (m_monster.attributes & MONSTER_ATTRIBUTE_FLYING) {
            m_monster.attributes -= MONSTER_ATTRIBUTE_FLYING;
        }
        m_frameInfo.walkDuration = static_cast<int>(
            m_frameInfo.walkDuration * ARMY_SLOW_WALK_DURATION_SCALE);
        break;
    case ARMY_SPELL_INFLUENCE_BLIND:
        break;
    case ARMY_SPELL_INFLUENCE_BLESS:
        CancelIndividualSpell(ARMY_SPELL_INFLUENCE_CURSE);
        break;
    case ARMY_SPELL_INFLUENCE_CURSE:
        CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BLESS);
        break;
    case ARMY_SPELL_INFLUENCE_BERSERK:
        CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HYPNOTIZE);
        break;
    case ARMY_SPELL_INFLUENCE_PARALYZE:
        break;
    case ARMY_SPELL_INFLUENCE_HYPNOTIZE:
        CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BERSERK);
        break;
    case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER:
        break;
    case ARMY_SPELL_INFLUENCE_BLOODLUST:
        m_monster.attack += ARMY_BLOODLUST_ATTACK_BONUS;
        break;
    case ARMY_SPELL_INFLUENCE_SHIELD:
        break;
    case ARMY_SPELL_INFLUENCE_PETRIFIED:
        break;
    case ARMY_SPELL_INFLUENCE_ANTI_MAGIC:
        for (i = 0; i < ARMY_SPELL_INFLUENCE_COUNT; i++) {
            CancelIndividualSpell(i);
        }
        break;
    case ARMY_SPELL_INFLUENCE_STONESKIN:
        if (m_spellInfluence[ARMY_SPELL_INFLUENCE_STEELSKIN]) {
            return 0;
        }
        m_monster.defense += ARMY_STONESKIN_DEFENSE_BONUS;
        break;
    case ARMY_SPELL_INFLUENCE_STEELSKIN:
        CancelIndividualSpell(ARMY_SPELL_INFLUENCE_STONESKIN);
        m_monster.defense += ARMY_STEELSKIN_DEFENSE_BONUS;
        break;
    }
    m_spellCount++;
    m_spellInfluence[influence] = static_cast<unsigned char>(rounds);
    return 1;
}

VA(0x00452401, 0x94)
void army::DecrementSpellRounds(void)
{
    int i;

    for (i = 0; i < ARMY_SPELL_INFLUENCE_COUNT; i++) {
        if (m_spellInfluence[i]) {
            if (m_spellInfluence[i] == 1) {
                CancelIndividualSpell(i);
            } else {
                m_spellInfluence[i]--;
            }
        }
    }
    if (m_roundCounter > 0) {
        m_roundCounter--;
    }
}

// @early-stop
// Logic, the 0x50 frame, every retail stack slot, and all 55 relocation sites
// align. Raw instruction review limits the non-jump residue to the candidate
// army side/index SIB evaluation and three equivalent comparison operand orders.
// The remaining extent delta is inline-accessor continuation and condition-
// polarity jumps; positive, negative, nested, and compound forms were audited.
VA(0x00452495, 0x644)
void army::GoBerserk(void)
{
    int masks_28[5];
    int savedQuantity_10;
    int direction_4;
    int targetHex_9;
    int nearestSide_8;
    int nearestIndex_19;
    int nearestDistance_2;
    int side_8;
    int index_11;
    int distance_6;

    masks_28[4] = 0;
    direction_4 = 0;
    masks_28[3] = 0;
    savedQuantity_10 = m_quantity;
    m_quantity = 0;
    masks_28[0] = gpCombatManager->GetAllMask(0);
    masks_28[1] = gpCombatManager->GetAllMask(1);
    m_quantity = savedQuantity_10;
    masks_28[2] = GetAttackMask(m_hex, 2, -1);
    if (masks_28[2] != ARMY_ALL_ATTACK_DIRECTIONS) {
        do {
            if (!masks_28[4]) {
                direction_4 = Random(0, ARMY_COMBAT_DIRECTION_COUNT - 1);
            } else {
                goto walkToward;
            }
        } while (masks_28[2] & (1 << direction_4));
        giNextAction = COMBAT_AI_ACTION_MOVE;
        ValidAttack(m_hex, direction_4, 2, -1, &targetHex_9);
        giNextActionGridIndex = targetHex_9;
        masks_28[4] = 1;
        goto berserkFinish;
    }
    {
        nearestIndex_19 = -1;
        nearestSide_8 = -1;
        nearestDistance_2 = ARMY_NEAREST_DISTANCE_LIMIT;
        for (side_8 = 0; side_8 < ARMY_COMBAT_SIDE_COUNT; side_8++) {
            for (index_11 = 0; index_11 < gpCombatManager->m_armyCount[side_8]; index_11++) {
                if (m_side == side_8 && m_index == index_11) {
                    continue;
                }
                army *candidate_16 = &gpCombatManager->m_armies[side_8][index_11];
                if (!(candidate_16->m_monster.flags.all & MONSTER_FLAGS_DEAD)) {
                    if (candidate_16->m_quantity > 0) {
                        distance_6 = gpSearchArray->QuickDistance(
                            gpCombatManager->m_hexCells[m_hex].m_x,
                            gpCombatManager->m_hexCells[m_hex].m_y,
                            gpCombatManager->m_hexCells[candidate_16->m_hex].m_x,
                            gpCombatManager->m_hexCells[candidate_16->m_hex].m_y);
                        if (distance_6 < nearestDistance_2) {
                            nearestIndex_19 = index_11;
                            nearestSide_8 = side_8;
                            nearestDistance_2 = distance_6;
                        }
                    }
                }
            }
        }
        if (nearestIndex_19 != -1 &&
            (m_monster.flags.all & MONSTER_FLAGS_SHOOTER) &&
            m_monster.shots > 0) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex =
                gpCombatManager->m_armies[nearestSide_8][nearestIndex_19].m_hex;
        } else {
            int sideZeroTarget_12 = -1;
            int sideOneTarget_18 = -1;
            if (gpCombatManager->AttemptAttack(this, 0, masks_28[0])) {
                giNextAction = COMBAT_AI_ACTION_MOVE;
                sideZeroTarget_12 = giNextActionGridIndex;
            }
            if (gpCombatManager->AttemptAttack(this, 1, masks_28[1])) {
                giNextAction = COMBAT_AI_ACTION_MOVE;
                sideOneTarget_18 = giNextActionGridIndex;
            }
            giNextActionGridIndex = -1;
            if (sideZeroTarget_12 != -1 || sideOneTarget_18 != -1) {
                if (sideZeroTarget_12 != -1 && sideOneTarget_18 != -1) {
                    int sideZeroDistance = gpSearchArray->QuickDistance(
                        gpCombatManager->m_hexCells[m_hex].m_x,
                        gpCombatManager->m_hexCells[m_hex].m_y,
                        gpCombatManager->m_hexCells[sideZeroTarget_12].m_x,
                        gpCombatManager->m_hexCells[sideZeroTarget_12].m_y);
                    int sideOneDistance = gpSearchArray->QuickDistance(
                        gpCombatManager->m_hexCells[m_hex].m_x,
                        gpCombatManager->m_hexCells[m_hex].m_y,
                        gpCombatManager->m_hexCells[sideOneTarget_18].m_x,
                        gpCombatManager->m_hexCells[sideOneTarget_18].m_y);
                    if (sideOneDistance > sideZeroDistance) {
                        giNextActionGridIndex = sideZeroTarget_12;
                    } else {
                        giNextActionGridIndex = sideOneTarget_18;
                    }
                } else if (sideZeroTarget_12 != -1) {
                    giNextActionGridIndex = sideZeroTarget_12;
                } else if (sideOneTarget_18 != -1) {
                    giNextActionGridIndex = sideOneTarget_18;
                }
            }
            if (giNextActionGridIndex == -1) {
                goto walkToward;
            }
        }
    }
    goto berserkFinish;
walkToward:
    if ((m_monster.flags.all & MONSTER_FLAGS_FLYING) ||
        (!gpCombatManager->WalkTowardArmy(this, m_side, masks_28[m_side]) &&
         !gpCombatManager->WalkTowardArmy(
             this, 1 - m_side, masks_28[1 - m_side]))) {
        giNextAction = COMBAT_AI_ACTION_WAIT;
    }
berserkFinish:
    if (giNextAction == COMBAT_AI_ACTION_MOVE &&
        gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == m_side) {
        gpCombatManager->m_heroDeathPending[m_side] = 1;
    }
}

// @early-stop
// All non-jump opcodes/operands and all 24 relocation sites align. The 32-byte
// object extent delta is limited to seven five-byte inline-accessor continuation
// jumps plus the equivalent first-condition branch encoding.
VA(0x00452ad9, 0x3f1)
void army::MoveAttack(int destination, int moveOnly)
{
    int baseAttackMask;
    int targetAttackMask;
    int sourceHex;
    int adjacentHex;
    int direction;
    hexcell *adjacentCell;

    while (1) {
        gpCombatManager->m_limitCreature = 0;
        m_targetSide = -1;
        m_targetIndex = -1;
        if (!ValidHex(destination)) {
            return;
        }
        if (gpCombatManager->m_hexCells[destination].m_occupantSide == -1 ||
            (gpCombatManager->m_hexCells[destination].m_occupantSide ==
                 gpCombatManager->m_currentArmySide &&
             gpCombatManager->m_hexCells[destination].m_occupantIndex ==
                 gpCombatManager->m_currentArmyIndex)) {
            goto move;
        }
        if (moveOnly) {
            return;
        }
        m_targetSide = gpCombatManager->m_hexCells[destination].m_occupantSide;
        m_targetIndex = gpCombatManager->m_hexCells[destination].m_occupantIndex;
        m_moveTargetHex = destination;
        baseAttackMask = GetAttackMask(m_hex, 0, -1);
        if ((m_monster.flags.all & MONSTER_FLAGS_FLYING) &&
            baseAttackMask == ARMY_ALL_ATTACK_DIRECTIONS) {
            if (m_hex != m_moveTargetHex && !ValidFlight(m_moveTargetHex, 0)) {
                return;
            }
            FlyTo(m_moveTargetHex);
            continue;
        }
        break;
    }
    if (m_spellInfluence[ARMY_SPELL_INFLUENCE_BERSERK]) {
        targetAttackMask = GetAttackMask(m_hex, 2, -1);
    } else {
        targetAttackMask = GetAttackMask(m_hex, 1, -1);
    }
    if (targetAttackMask == ARMY_ALL_ATTACK_DIRECTIONS && m_monster.shots > 0) {
        SpecialAttack();
    } else if (baseAttackMask == ARMY_ALL_ATTACK_DIRECTIONS) {
        AttackTo();
    } else {
        for (direction = 0; direction < ARMY_COMBAT_DIRECTION_COUNT; direction++) {
            if (direction < ARMY_ADJACENT_DIRECTION_COUNT ||
                (m_monster.flags.all & MONSTER_FLAGS_WIDE)) {
                sourceHex = m_hex;
                if ((m_monster.flags.all & MONSTER_FLAGS_WIDE) &&
                    m_facing == 1 && direction >= 0 && direction <= 2) {
                    sourceHex++;
                }
                if ((m_monster.flags.all & MONSTER_FLAGS_WIDE) &&
                    m_facing == 0 && direction >= 3 && direction <= 5) {
                    sourceHex--;
                }
                if (direction >= 6) {
                    if (m_facing == 1) {
                        sourceHex++;
                    } else {
                        sourceHex--;
                    }
                }
                adjacentHex = GetAdjacentCellIndex(sourceHex, direction);
                if (ValidHex(adjacentHex)) {
                    adjacentCell = &gpCombatManager->m_hexCells[adjacentHex];
                    if (adjacentCell->m_occupantSide == m_targetSide &&
                        adjacentCell->m_occupantIndex == m_targetIndex) {
                        m_attackDirection = direction;
                    }
                }
            }
        }
        DoAttack(0);
    }
    goto finish;
move:
    if (m_monster.flags.all & MONSTER_FLAGS_FLYING) {
        m_moveTargetHex = destination;
        if (!ValidFlight(m_moveTargetHex, 0)) {
            return;
        }
        FlyTo(m_moveTargetHex);
    } else {
        WalkTo(destination);
    }
finish:
    gpCombatManager->m_limitCreature = 1;
}

// @early-stop
// The instruction stream and all 59/59 relocations align; only TU-local
// floating-constant relocation identities differ.
VA(0x00452eca, 0x931)
float army::SpellCastWorkChance(int spell)
{
    int foundSpell;
    int i;
    int resurrectPower;
    int hypnotizeHitPoints;

    if ((m_monster.flags.all & MONSTER_FLAGS_MIRROR_IMAGE) &&
        (spell == SPELL_MIRROR_IMAGE || spell == SPELL_ANTI_MAGIC)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (spell == SPELL_DISPEL || spell == SPELL_MASS_DISPEL) {
        foundSpell = 0;
        for (i = 0; i < ARMY_SPELL_INFLUENCE_COUNT; i++) {
            if (m_spellInfluence[i]) {
                foundSpell = 1;
                break;
            }
        }
        if (!foundSpell) {
            return ARMY_SPELL_CHANCE_NONE;
        }
    }
    if (m_spellInfluence[ARMY_SPELL_INFLUENCE_ANTI_MAGIC] ||
        ((m_monster.flags.all & MONSTER_FLAGS_DEAD) &&
         spell != SPELL_RESURRECT && spell != SPELL_TRUE_RESURRECT &&
         spell != SPELL_ANIMATE_DEAD) ||
        m_deathPending ||
        m_monsterType == ARMY_CREATURE_GREEN_DRAGON ||
        m_monsterType == ARMY_CREATURE_RED_DRAGON ||
        m_monsterType == ARMY_CREATURE_BLACK_DRAGON) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (spell == SPELL_MIRROR_IMAGE && m_mirrorImageIndex != -1) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if ((spell == SPELL_RESURRECT || spell == SPELL_TRUE_RESURRECT) &&
        ((m_monster.flags.all & MONSTER_FLAGS_UNDEAD) ||
         m_initialQuantity == m_quantity)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (spell == SPELL_ANIMATE_DEAD &&
        (!(m_monster.flags.all & MONSTER_FLAGS_UNDEAD) ||
         m_initialQuantity == m_quantity)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if ((spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT) &&
        !(m_monster.flags.all & MONSTER_FLAGS_UNDEAD)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if ((spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE) &&
        (m_monster.flags.all & MONSTER_FLAGS_UNDEAD)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == ARMY_CREATURE_PHOENIX &&
        spell != SPELL_FIREBALL && spell != SPELL_FIREBLAST &&
        spell != SPELL_LIGHTNING_BOLT && spell != SPELL_CHAIN_LIGHTNING &&
        spell != SPELL_COLD_RAY && spell != SPELL_COLD_RING &&
        spell != SPELL_ELEMENTAL_STORM) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == ARMY_CREATURE_CRUSADER &&
        (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (((m_monster.flags.all & MONSTER_FLAGS_UNDEAD) ||
         m_monsterType == ARMY_CREATURE_EARTH_ELEMENTAL ||
         m_monsterType == ARMY_CREATURE_AIR_ELEMENTAL ||
         m_monsterType == ARMY_CREATURE_FIRE_ELEMENTAL ||
         m_monsterType == ARMY_CREATURE_WATER_ELEMENTAL ||
         m_monsterType == ARMY_CREATURE_GIANT ||
         m_monsterType == ARMY_CREATURE_TITAN) &&
        (spell == SPELL_BERSERKER || spell == SPELL_HYPNOTIZE ||
         spell == SPELL_PARALYZE || spell == SPELL_BLIND)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if ((m_monster.flags.all & MONSTER_FLAGS_UNDEAD) &&
        (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE ||
         spell == SPELL_BLESS || spell == SPELL_MASS_BLESS)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == ARMY_CREATURE_EARTH_ELEMENTAL &&
        (spell == SPELL_LIGHTNING_BOLT || spell == SPELL_CHAIN_LIGHTNING ||
         spell == SPELL_METEOR_SHOWER)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == ARMY_CREATURE_AIR_ELEMENTAL &&
        spell == SPELL_METEOR_SHOWER) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == ARMY_CREATURE_FIRE_ELEMENTAL &&
        (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == ARMY_CREATURE_WATER_ELEMENTAL &&
        (spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (gpCombatManager->m_heroes[m_side]) {
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(
                SPELL_ARTIFACT_HOLY_PENDANT) &&
            (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE)) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(
                SPELL_ARTIFACT_PENDANT_FREE_WILL) &&
            spell == SPELL_HYPNOTIZE) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(
                SPELL_ARTIFACT_PENDANT_LIFE) &&
            (spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE)) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(
                SPELL_ARTIFACT_SERENITY_PENDANT) &&
            spell == SPELL_BERSERKER) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(
                SPELL_ARTIFACT_SEEING_EYE_PENDANT) &&
            spell == SPELL_BLIND) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(
                SPELL_ARTIFACT_KINETIC_PENDANT) &&
            spell == SPELL_PARALYZE) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(
                SPELL_ARTIFACT_PENDANT_DEATH) &&
            (spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT)) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(
                SPELL_ARTIFACT_WAND_NEGATION) &&
            (spell == SPELL_DISPEL || spell == SPELL_MASS_DISPEL ||
             spell == SPELL_CREATURE_DISPEL)) {
            return ARMY_SPELL_CHANCE_NONE;
        }
    }
    if (m_quantity == 0 &&
        (spell == SPELL_RESURRECT || spell == SPELL_TRUE_RESURRECT ||
         spell == SPELL_ANIMATE_DEAD)) {
        resurrectPower = gpCombatManager->m_spellPower[
                             gpCombatManager->m_currentSide] *
                         ARMY_RESURRECT_POWER_PER_SPELL_POWER;
        if (gpCombatManager->m_heroes[gpCombatManager->m_currentSide] &&
            gpCombatManager->m_heroes[gpCombatManager->m_currentSide]
                ->HasArtifact(SPELL_ARTIFACT_ANKH)) {
            resurrectPower *= ARMY_ARTIFACT_POWER_MULTIPLIER;
        }
        if (resurrectPower < m_monster.hitPoints) {
            return ARMY_SPELL_CHANCE_NONE;
        }
    }
    if (spell == SPELL_CREATURE_DISPEL &&
        !m_spellInfluence[ARMY_SPELL_INFLUENCE_HASTE] &&
        !m_spellInfluence[ARMY_SPELL_INFLUENCE_BLESS] &&
        !m_spellInfluence[ARMY_SPELL_INFLUENCE_DRAGON_SLAYER] &&
        !m_spellInfluence[ARMY_SPELL_INFLUENCE_BLOODLUST] &&
        !m_spellInfluence[ARMY_SPELL_INFLUENCE_SHIELD] &&
        !m_spellInfluence[ARMY_SPELL_INFLUENCE_ANTI_MAGIC] &&
        !m_spellInfluence[ARMY_SPELL_INFLUENCE_STONESKIN] &&
        !m_spellInfluence[ARMY_SPELL_INFLUENCE_STEELSKIN]) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (spell == SPELL_HYPNOTIZE) {
        hypnotizeHitPoints = gpCombatManager->m_heroes[
                                  gpCombatManager->m_currentSide]
                                  ->Stats(HERO_PRIMARY_SPELL_POWER) *
                              ARMY_HYPNOTIZE_HIT_POINTS_PER_POWER;
        if (gpCombatManager->m_heroes[gpCombatManager->m_currentSide]
                ->HasArtifact(SPELL_ARTIFACT_GOLD_WATCH)) {
            hypnotizeHitPoints *= ARMY_ARTIFACT_POWER_MULTIPLIER;
        }
        if (m_monster.hitPoints * m_quantity > hypnotizeHitPoints) {
            return ARMY_SPELL_CHANCE_NONE;
        }
    }
    if ((m_monsterType == ARMY_CREATURE_DWARF ||
         m_monsterType == ARMY_CREATURE_BATTLE_DWARF) &&
        spell != SPELL_TELEPORT && spell != SPELL_CURE &&
        spell != SPELL_MASS_CURE && spell != SPELL_RESURRECT &&
        spell != SPELL_TRUE_RESURRECT && spell != SPELL_HASTE &&
        spell != SPELL_MASS_HASTE && spell != SPELL_BLESS &&
        spell != SPELL_MASS_BLESS && spell != SPELL_STONE_SKIN &&
        spell != SPELL_STEEL_SKIN && spell != SPELL_ANTI_MAGIC &&
        spell != SPELL_DRAGON_SLAYER && spell != SPELL_BLOOD_LUST &&
        spell != SPELL_MIRROR_IMAGE && spell != SPELL_SHIELD &&
        spell != SPELL_MASS_SHIELD) {
        return ARMY_SPELL_CHANCE_DWARF;
    }
    return ARMY_SPELL_CHANCE_ALWAYS;
}

VA(0x004537fb, 0x56)
int army::SpellCastWorks(int spell)
{
    int chance;

    chance = static_cast<int>(SpellCastWorkChance(spell) *
                              ARMY_SPELL_CHANCE_PERCENT);
    return SRandom(1, ARMY_RANDOM_SPELL_ROLL_MAX) <= chance;
}

VA(0x00453851, 0x39e)
void BuildTempWalkSeq(struct SMonFrameInfo *frameInfo, int finishStanding,
                      int skipDrawing)
{
    frameInfo->animationFrameCount[ARMY_ANIMATION_WALK] = 0;
    if (!skipDrawing && finishStanding) {
        if (frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_STAND] > 0) {
            memcpy(&frameInfo->animationFrames[ARMY_ANIMATION_WALK]
                                                [frameInfo->animationFrameCount[
                                                    ARMY_ANIMATION_WALK]],
                   frameInfo->animationFrames[ARMY_WALK_SEGMENT_STAND],
                   frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_STAND]);
            memcpy(&frameInfo->walkXOffsets[frameInfo->animationFrameCount[
                                                ARMY_ANIMATION_WALK]],
                   frameInfo->animationXOffsets[ARMY_WALK_SEGMENT_STAND],
                   frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_STAND]);
            frameInfo->animationFrameCount[ARMY_ANIMATION_WALK] +=
                frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_STAND];
        }
    } else {
        if (!skipDrawing) {
            if (frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_BEGIN] > 0) {
                memcpy(&frameInfo->animationFrames[ARMY_ANIMATION_WALK]
                                                    [frameInfo->animationFrameCount[
                                                        ARMY_ANIMATION_WALK]],
                       frameInfo->animationFrames[ARMY_WALK_SEGMENT_BEGIN],
                       frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_BEGIN]);
                memcpy(&frameInfo->walkXOffsets[frameInfo->animationFrameCount[
                                                    ARMY_ANIMATION_WALK]],
                       frameInfo->animationXOffsets[ARMY_WALK_SEGMENT_BEGIN],
                       frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_BEGIN]);
                frameInfo->animationFrameCount[ARMY_ANIMATION_WALK] +=
                    frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_BEGIN];
            }
        } else if (frameInfo->animationFrameCount[
                       ARMY_WALK_SEGMENT_BEGIN_STANDING] > 0) {
            memcpy(&frameInfo->animationFrames[ARMY_ANIMATION_WALK]
                                                [frameInfo->animationFrameCount[
                                                    ARMY_ANIMATION_WALK]],
                   frameInfo->animationFrames[ARMY_WALK_SEGMENT_BEGIN_STANDING],
                   frameInfo->animationFrameCount[
                       ARMY_WALK_SEGMENT_BEGIN_STANDING]);
            memcpy(&frameInfo->walkXOffsets[frameInfo->animationFrameCount[
                                                ARMY_ANIMATION_WALK]],
                   frameInfo->animationXOffsets[ARMY_WALK_SEGMENT_BEGIN_STANDING],
                   frameInfo->animationFrameCount[
                       ARMY_WALK_SEGMENT_BEGIN_STANDING]);
            frameInfo->animationFrameCount[ARMY_ANIMATION_WALK] +=
                frameInfo->animationFrameCount[
                    ARMY_WALK_SEGMENT_BEGIN_STANDING];
        }
        if (frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_MIDDLE] > 0) {
            memcpy(&frameInfo->animationFrames[ARMY_ANIMATION_WALK]
                                                [frameInfo->animationFrameCount[
                                                    ARMY_ANIMATION_WALK]],
                   frameInfo->animationFrames[ARMY_WALK_SEGMENT_MIDDLE],
                   frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_MIDDLE]);
            memcpy(&frameInfo->walkXOffsets[frameInfo->animationFrameCount[
                                                ARMY_ANIMATION_WALK]],
                   frameInfo->animationXOffsets[ARMY_WALK_SEGMENT_MIDDLE],
                   frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_MIDDLE]);
            frameInfo->animationFrameCount[ARMY_ANIMATION_WALK] +=
                frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_MIDDLE];
        }
        if (finishStanding) {
            if (frameInfo->animationFrameCount[
                    ARMY_WALK_SEGMENT_END_STANDING] > 0) {
                memcpy(&frameInfo->animationFrames[ARMY_ANIMATION_WALK]
                                                    [frameInfo->animationFrameCount[
                                                        ARMY_ANIMATION_WALK]],
                       frameInfo->animationFrames[
                           ARMY_WALK_SEGMENT_END_STANDING],
                       frameInfo->animationFrameCount[
                           ARMY_WALK_SEGMENT_END_STANDING]);
                memcpy(&frameInfo->walkXOffsets[frameInfo->animationFrameCount[
                                                    ARMY_ANIMATION_WALK]],
                       frameInfo->animationXOffsets[
                           ARMY_WALK_SEGMENT_END_STANDING],
                       frameInfo->animationFrameCount[
                           ARMY_WALK_SEGMENT_END_STANDING]);
                frameInfo->animationFrameCount[ARMY_ANIMATION_WALK] +=
                    frameInfo->animationFrameCount[
                        ARMY_WALK_SEGMENT_END_STANDING];
            }
        } else if (frameInfo->animationFrameCount[
                       ARMY_WALK_SEGMENT_END] > 0) {
            memcpy(&frameInfo->animationFrames[ARMY_ANIMATION_WALK]
                                                [frameInfo->animationFrameCount[
                                                    ARMY_ANIMATION_WALK]],
                   frameInfo->animationFrames[ARMY_WALK_SEGMENT_END],
                   frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_END]);
            memcpy(&frameInfo->walkXOffsets[frameInfo->animationFrameCount[
                                                ARMY_ANIMATION_WALK]],
                   frameInfo->animationXOffsets[ARMY_WALK_SEGMENT_END],
                   frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_END]);
            frameInfo->animationFrameCount[ARMY_ANIMATION_WALK] +=
                frameInfo->animationFrameCount[ARMY_WALK_SEGMENT_END];
        }
    }
}

VA(0x00453bef, 0x66)
void army::DispelGood(void)
{
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HASTE);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BLESS);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BLOODLUST);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_SHIELD);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_ANTI_MAGIC);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_STONESKIN);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_STEELSKIN);
}

VA(0x00453c55, 0x90)
void army::Cure(int amount)
{
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_SLOW);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BLIND);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_CURSE);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BERSERK);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_PARALYZE);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HYPNOTIZE);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_PETRIFIED);
    m_hitPointsLost -= amount * ARMY_CURE_HIT_POINTS_PER_POWER;
    if (m_hitPointsLost < 0) {
        m_hitPointsLost = 0;
    }
}

VA(0x00453ce5, 0x79)
int army::MidX(void)
{
    int wideOffset;

    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        if (m_facing == 1) {
            wideOffset = ARMY_WIDE_CREATURE_HALF_WIDTH;
        } else {
            wideOffset = -ARMY_WIDE_CREATURE_HALF_WIDTH;
        }
    } else {
        wideOffset = 0;
    }
    return gpCombatManager->m_hexCells[m_hex].m_x + wideOffset;
}

VA(0x00453d5e, 0x59)
int army::MidY(void)
{
    return gpCombatManager->m_hexCells[m_hex].m_y -
           (GetIconEntry(
                m_creatureIcon,
                m_frameInfo.animationFrames[ARMY_ANIMATION_STAND][0])
                ->h >>
            1);
}

VA(0x00453db7, 0x57)
int army::TopY(void)
{
    return gpCombatManager->m_hexCells[m_hex].m_y -
           GetIconEntry(
               m_creatureIcon,
               m_frameInfo.animationFrames[ARMY_ANIMATION_STAND][0])
               ->h;
}

VA(0x00453e0e, 0xcc)
int army::RightX(void)
{
    if (m_facing == 1) {
        return gpCombatManager->m_hexCells[m_hex].m_x +
               GetIconEntry(
                   m_creatureIcon,
                   m_frameInfo.animationFrames[ARMY_ANIMATION_STAND][0])
                   ->w +
               GetIconEntry(
                   m_creatureIcon,
                   m_frameInfo.animationFrames[ARMY_ANIMATION_STAND][0])
                   ->x;
    } else {
        return gpCombatManager->m_hexCells[m_hex].m_x -
               GetIconEntry(
                   m_creatureIcon,
                   m_frameInfo.animationFrames[ARMY_ANIMATION_STAND][0])
                   ->x;
    }
}

VA(0x00453eda, 0xcc)
int army::LeftX(void)
{
    if (m_facing == 1) {
        return gpCombatManager->m_hexCells[m_hex].m_x +
               GetIconEntry(
                   m_creatureIcon,
                   m_frameInfo.animationFrames[ARMY_ANIMATION_STAND][0])
                   ->x;
    } else {
        return gpCombatManager->m_hexCells[m_hex].m_x -
               (GetIconEntry(
                    m_creatureIcon,
                    m_frameInfo.animationFrames[ARMY_ANIMATION_STAND][0])
                    ->w +
                GetIconEntry(
                    m_creatureIcon,
                    m_frameInfo.animationFrames[ARMY_ANIMATION_STAND][0])
                    ->x);
    }
}

// @early-stop
// At the retained 99.92% pass, the relocation-masked instruction stream and
// all 3/3 relocations aligned. Exact sibling edits only changed the live
// two-dimensional army-array address evaluation order.
VA(0x00453fa6, 0x171)
int army::OtherArmyAdjacent(int side, int index)
{
    army *otherArmy1;
    int otherHex1;
    int otherRearHex;
    int adjacentHex;
    int rearHex;
    int directionResult;

    otherArmy1 = &gpCombatManager->m_armies[side][index];
    otherHex1 = otherArmy1->m_hex;
    if (otherArmy1->m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        otherRearHex = otherHex1 + (otherArmy1->m_side == 0 ? 1 : -1);
    } else {
        otherRearHex = -1;
    }
    for (directionResult = 0; directionResult < ARMY_ADJACENT_DIRECTION_COUNT;
         directionResult++) {
        adjacentHex = GetAdjacentCellIndex(m_hex, directionResult);
        if (adjacentHex == otherHex1 ||
            (adjacentHex != -1 && adjacentHex == otherRearHex)) {
            return 1;
        }
    }
    if (m_monster.flags.all & MONSTER_FLAGS_WIDE) {
        rearHex = m_hex + (m_side == 0 ? 1 : -1);
        for (directionResult = 0;
             directionResult < ARMY_ADJACENT_DIRECTION_COUNT;
             directionResult++) {
            adjacentHex = GetAdjacentCellIndex(rearHex, directionResult);
            if (adjacentHex == otherHex1 ||
                (adjacentHex != -1 && adjacentHex == otherRearHex)) {
                return 1;
            }
        }
    }
    return 0;
}

// @early-stop
// Code bytes and all 12/12 relocation targets align; only delinked
// gMonsterDatabase/float-constant relocation identities differ.
VA(0x00454117, 0x1e1)
void ModifyFrameInfo(struct SMonFrameInfo *frameInfo, int monsterType)
{
    int speedDifference;

    speedDifference = 0;
    if (monsterType == ARMY_CREATURE_RANGER ||
        monsterType == ARMY_CREATURE_VETERAN_PIKEMAN ||
        monsterType == ARMY_CREATURE_MASTER_SWORDSMAN ||
        monsterType == ARMY_CREATURE_CHAMPION ||
        monsterType == ARMY_CREATURE_CRUSADER ||
        monsterType == ARMY_CREATURE_ORC_CHIEF ||
        monsterType == ARMY_CREATURE_OGRE_LORD ||
        monsterType == ARMY_CREATURE_WAR_TROLL ||
        monsterType == ARMY_CREATURE_BATTLE_DWARF ||
        monsterType == ARMY_CREATURE_GRAND_ELF ||
        monsterType == ARMY_CREATURE_GREATER_DRUID ||
        monsterType == ARMY_CREATURE_MINOTAUR_KING ||
        monsterType == ARMY_CREATURE_STEEL_GOLEM ||
        monsterType == ARMY_CREATURE_ARCHMAGE ||
        monsterType == ARMY_CREATURE_MUTANT_ZOMBIE ||
        monsterType == ARMY_CREATURE_ROYAL_MUMMY ||
        monsterType == ARMY_CREATURE_VAMPIRE_LORD ||
        monsterType == ARMY_CREATURE_POWER_LICH) {
        speedDifference = gMonsterDatabase[monsterType].speed -
                          gMonsterDatabase[monsterType - 1].speed;
    }
    if (monsterType == ARMY_CREATURE_EARTH_ELEMENTAL ||
        monsterType == ARMY_CREATURE_AIR_ELEMENTAL ||
        monsterType == ARMY_CREATURE_WATER_ELEMENTAL) {
        speedDifference = gMonsterDatabase[monsterType].speed -
                          gMonsterDatabase[ARMY_CREATURE_FIRE_ELEMENTAL].speed;
    }
    if (speedDifference) {
        if (monsterType == ARMY_CREATURE_RANGER) {
            frameInfo->attackDuration = static_cast<int>(
                frameInfo->attackDuration * ARMY_RANGER_ATTACK_DURATION_SCALE);
        } else {
            frameInfo->attackDuration = static_cast<int>(
                (ARMY_DURATION_BASE_SCALE -
                 speedDifference * ARMY_ATTACK_DURATION_SPEED_SCALE) *
                frameInfo->attackDuration);
        }
        frameInfo->walkDuration = static_cast<int>(
            (ARMY_DURATION_BASE_SCALE -
             speedDifference * ARMY_WALK_DURATION_SPEED_SCALE) *
            frameInfo->walkDuration);
    }
}

VA(0x004542f8, 0xbe)
int army::GetPowBaseY(void)
{
    int y;

    y = MidY();
    if (gCurLoadedSpellEffect == SPELL_BERSERKER ||
        gCurLoadedSpellEffect == SPELL_MAGIC_ARROW ||
        gCurLoadedSpellEffect == SPELL_HOLY_SHOUT ||
        gCurLoadedSpellEffect == SPELL_METEOR_SHOWER ||
        gCurLoadedSpellEffect == SPELL_ARMAGEDDON ||
        gCurLoadedSpellEffect == SPELL_ANTI_MAGIC) {
        y = TopY();
    }
    if (gCurLoadedSpellEffect == SPELL_PARALYZE ||
        gCurLoadedSpellEffect == SPELL_HYPNOTIZE) {
        y = gpCombatManager->m_hexCells[m_hex].m_y +
            ARMY_CONTROL_EFFECT_Y_OFFSET;
    }
    return y;
}

// ---- globals (definitions, RVA order) ----
// @data-layout-note Retail .data is 0xf53c4+0x2f4 and places bSecondAttack at
// +0xe4, after the first function literal pool. Candidate .data is 0x2f3 and
// places this external definition at +0 while several later literals still
// differ in spelling/order. Revisit with exact function literals; do not model
// the 0xe4 prefix as invented storage.
DATA(0x004f54a8) int bSecondAttack = 0;
DATA(0x00527eb4) int gbGenieHalf;
