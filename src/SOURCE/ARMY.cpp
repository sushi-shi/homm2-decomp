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
army::army(void) {
    i32 i;

    m_creatureIcon = 0;
    m_hex = 0;
    for (i = 0; i < IDX(ARMY_SAMPLE_COUNT); i++) {
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
void army::WaitSample(ArmySampleType sampleIndex) {
    return;
}

VA(0x0044a9a7, 0xde)
void army::InitClean(void) {
    i32 i;

    for (i = 0; i < IDX(ARMY_SAMPLE_COUNT); i++) {
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
void army::Init(CreatureType monsterType, i32 quantity, i32 side, i32 index, i32 hex, i32 unknown) {
    hero* commander;
    i32 rearHex;

    InitClean();
    m_monsterType = monsterType;
    m_drawState = 1;
    m_monster = gMonsterDatabase[IDX(monsterType)];
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
        gpCombatManager->m_armyGroups[COMBAT_DEFENDER_SIDE - m_side]
    );
    if (m_monsterType == CREATURE_EARTH_ELEMENTAL || m_monsterType == CREATURE_AIR_ELEMENTAL
        || m_monsterType == CREATURE_FIRE_ELEMENTAL || m_monsterType == CREATURE_WATER_ELEMENTAL
        || HAS(m_monster.flags.all, MONSTER_FLAGS_NO_MORALE)) {
        m_morale = 0;
    }
    m_luck = gpGame->GetLuck(
        gpCombatManager->m_heroes[m_side],
        this,
        gpCombatManager->m_combatTowns[m_side]
    );
    m_hex = hex;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = m_side;
    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = m_index;
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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

VA(0x0044aec8, 0x4a6)
void army::LoadResources(void) {
    i32 unusedLoadWord17;
    i32 unusedLoadWord14;
    i32 unusedLoadWord9;
    i32 unusedLoadWord0;
    i32 i;
    i32 unusedLoadWord2;
    i32 unusedLoadWord28;
    i32 unusedLoadWord34;
    i32 unusedLoadWord40;

    if (gbNoShowCombat) {
        return;
    }

    gpResourceManager->PointToFile(
        gpResourceManager->MakeId(cArmyFrameFileNames[IDX(m_monsterType)], 1)
    );
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(&m_frameInfo), sizeof(m_frameInfo));
    ModifyFrameInfo(&m_frameInfo, m_monsterType);
    m_walkDuration = m_frameInfo.walkDuration;

    sprintf(gText, "%smove.82M", m_monster.spriteName);
    m_samples[IDX(ARMY_SAMPLE_MOVE)] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%sattk.82M", m_monster.spriteName);
    m_samples[IDX(ARMY_SAMPLE_ATTACK)] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%swnce.82M", m_monster.spriteName);
    m_samples[IDX(ARMY_SAMPLE_WINCE)] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%skill.82M", m_monster.spriteName);
    m_samples[IDX(ARMY_SAMPLE_KILL)] = gpResourceManager->GetSample(gText);

    if (HAS(m_monster.flags.all, MONSTER_FLAGS_SHOOTER)) {
        sprintf(gText, "%sshot.82M", m_monster.spriteName);
        m_samples[IDX(ARMY_SAMPLE_SHOT)] = gpResourceManager->GetSample(gText);
    }
    if (m_monsterType == CREATURE_VAMPIRE || m_monsterType == CREATURE_VAMPIRE_LORD) {
        sprintf(gText, "%sext1.82M", m_monster.spriteName);
        m_samples[IDX(ARMY_SAMPLE_EXTRA_ONE)] = gpResourceManager->GetSample(gText);
        sprintf(gText, "%sext2.82M", m_monster.spriteName);
        m_samples[IDX(ARMY_SAMPLE_EXTRA_TWO)] = gpResourceManager->GetSample(gText);
    }
    if (m_monsterType == CREATURE_LICH || m_monsterType == CREATURE_POWER_LICH) {
        sprintf(gText, "%sexpl.82M", m_monster.spriteName);
        m_samples[IDX(ARMY_SAMPLE_EXTRA_ONE)] = gpResourceManager->GetSample(gText);
    }

    m_creatureIcon = gpResourceManager->GetIcon(cMonFilename[IDX(m_monsterType)]);
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_SHOOTER)) {
        if (m_monsterType == CREATURE_GIANT || m_monsterType == CREATURE_TITAN) {
            sprintf(gText, "titanmsl.icn");
        } else if (m_monsterType == CREATURE_HALFLING) {
            sprintf(gText, "halflmsl.icn");
        } else if (m_monsterType == CREATURE_ARCHER || m_monsterType == CREATURE_RANGER) {
            sprintf(gText, "arch_msl.icn");
        } else if (m_monsterType == CREATURE_LICH || m_monsterType == CREATURE_POWER_LICH) {
            sprintf(gText, "lich_msl.icn");
        } else if (m_monsterType == CREATURE_ORC || m_monsterType == CREATURE_ORC_CHIEF) {
            sprintf(gText, "orc__msl.icn");
        } else if (m_monsterType == CREATURE_DRUID || m_monsterType == CREATURE_GREATER_DRUID) {
            sprintf(gText, "druidmsl.icn");
        } else if (m_monsterType == CREATURE_TROLL || m_monsterType == CREATURE_WAR_TROLL) {
            sprintf(gText, "trollmsl.icn");
        } else {
            sprintf(gText, "elf__msl.icn");
        }
        m_missileIcon = gpResourceManager->GetIcon(gText);
    } else {
        m_samples[IDX(ARMY_SAMPLE_SHOT)] = 0;
        m_missileIcon = 0;
    }

    for (i = 0; i < ARMY_PRIMARY_SAMPLE_COUNT; i++) {
        if (m_samples[i]) {
            m_samples[i]->m_playbackData.volume = ARMY_SAMPLE_VOLUME;
            m_samples[i]->m_playbackData.channelType = ARMY_SAMPLE_CHANNEL;
            m_samples[i]->m_playbackData.loopCount = 1;
        }
    }
}

VA(0x0044b36e, 0xe8)
void army::FreeResources(void) {
    i32 i;

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
    for (i = 0; i < IDX(ARMY_SAMPLE_COUNT); i++) {
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
void army::DrawToBuffer(i32 x, i32 y, i32 effectsOnly) {
    i32 quantityY0;
    i32 spellY;
    u8* palette8;
    i32 spellX2;
    i32 standing7;
    i32 yDistance6;
    i32 xDistance;
    i32 unused;
    i32 effect6;
    i32 frameCount17;
    i32 color;
    i32 quantityX5;
    i32 goodEffects0;
    i32 drawn1;
    i32 neighborOccupied14;
    i32 badEffects0;
    i32 quantityOffset0;
    char quantityText3[ARMY_QUANTITY_TEXT_SIZE];
    i32 statusFrame0;
    i32 neighboringHex;

    if (gpCombatManager->m_nonVisualCombat) {
        return;
    }
    if (gbNoShowCombat) {
        return;
    }

    unused = 0;
    color = 0;
    standing7 = m_animationSequence == ARMY_ANIMATION_STAND
                || (m_animationSequence >= ARMY_ANIMATION_STANDING_FIRST
                    && IDX(m_animationSequence) <= IDX(ARMY_ANIMATION_STANDING_END) - 1);
    y += m_yOffset;
    x += m_xOffset;
    if (m_animationSequence == ARMY_ANIMATION_WALK
        && !HAS(m_monster.flags.all, MONSTER_FLAGS_FLYING)) {
        frameCount17 = m_frameInfo.animationFrameCount[IDX(m_animationSequence)];
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
        color = 237;
    }
    if (m_hex == gpCombatManager->m_limitCreatureHex && gpCombatManager->m_limitCreature == 1) {
        color = 236;
    }

    palette8 = 0;
    if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]) {
        palette8 = gColorTableGray;
    } else if (HAS(m_monster.flags.all, MONSTER_FLAGS_RED_PALETTE)) {
        palette8 = gColorTableRed;
    } else if (HAS(m_monster.flags.all, MONSTER_FLAGS_DARK_BROWN_PALETTE)) {
        palette8 = gColorTableDarkBrown;
    } else if (HAS(m_monster.flags.all, MONSTER_FLAGS_GRAY_PALETTE)) {
        palette8 = gColorTableGray;
    } else if (HAS(m_monster.flags.all, MONSTER_FLAGS_LIGHT_PALETTE)) {
        palette8 = gColorTableLighten;
    }
    if (effectsOnly == 0) {
        m_creatureIcon->CombatClipDrawToBuffer(
            x,
            y,
            m_frameInfo.animationFrames[IDX(m_animationSequence)][m_animationFrame],
            &m_spriteLimits,
            m_facing == 0,
            color,
            palette8,
            m_palette
        );
    }

    if (standing7 && gpCombatManager->m_showArmyQuantities && m_showQuantity) {
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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
        neighborOccupied14 = gpCombatManager->m_hexCells[neighboringHex].m_occupantSide != -1;
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
            drawn1 =
                gpCombatManager->m_combatIcons[IDX(COMBAT_ICON_STATUS)]->CombatClipDrawToBuffer(
                    quantityX5,
                    quantityY0,
                    10,
                    &m_creatureLimits,
                    0,
                    0,
                    0,
                    0
                );
        } else if (giSpellEffectShowType == 1) {
            drawn1 =
                gpCombatManager->m_combatIcons[IDX(COMBAT_ICON_STATUS)]->CombatClipDrawToBuffer(
                    quantityX5,
                    quantityY0,
                    11,
                    &m_creatureLimits,
                    0,
                    237,
                    0,
                    0
                );
        } else {
            statusFrame0 = 2;
            if (goodEffects0 < 1 || badEffects0 < 1) {
                if (badEffects0 > 0) {
                    statusFrame0 = 4;
                }
            } else {
                statusFrame0 = 3;
            }
            drawn1 =
                gpCombatManager->m_combatIcons[IDX(COMBAT_ICON_STATUS)]->CombatClipDrawToBuffer(
                    quantityX5,
                    quantityY0,
                    statusFrame0 + 10,
                    &m_creatureLimits,
                    0,
                    0,
                    0,
                    0
                );
        }
        if (drawn1) {
            sprintf(quantityText3, "%d", m_lastTargetHex == -1 ? m_quantity : m_lastTargetHex);
            smallFont->DrawBoundedString(quantityText3, quantityX5, quantityY0 + 2, 20, 12, 1, 1);
        }
    }

    if (m_drawSpellEffect && effectsOnly == 0) {
        spellX2 = x;
        spellY = GetPowBaseY();
        if (m_animationSequence == ARMY_ANIMATION_WINCE
            || m_animationSequence == ARMY_ANIMATION_WINCE_RETURN) {
            if (m_facing == 1) {
                spellX2 -= 4;
            } else {
                spellX2 += 4;
            }
        }
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
            if (m_facing == 1) {
                spellX2 += 22;
            } else {
                spellX2 -= 22;
            }
        }
        if (gCurLoadedSpellEffect == SPELL_DISPEL) {
            if (m_facing == 1) {
                spellX2 = RightX();
            } else {
                spellX2 = LeftX();
            }
        }
        if (gCurLoadedSpellEffect == SPELL_ELEMENTAL_STORM) {
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
            0
        );
    }
}

VA(0x0044bc28, 0x2d)
void army::Wince(void) {
    m_animationSequence = ARMY_ANIMATION_WINCE;
    m_animationFrame = 0;
}

// @semantic: The 0x2c frame, every visible slot, complete CFG, and all 151
// ordered relocations align after the nested sample-layout correction. Live
// residuals are two equivalent extent-clamp operand orders, the two final
// range-branch polarities, and the __adjust_fdiv/iLeftRightSave alias. Direct,
// commuted, negated, and scalar-SIB forms were previously exhausted; revisit
// after later ARMY TU-state changes.
VA(0x0044bc55, 0xb90)
void army::Walk(i32 direction, i32 finishStanding, i32 skipDrawing) {
    i32 oldMaxX_1;
    i32 oldMaxY_1;
    i32 destination_3;
    i32 otherHex_1;
    i32 frame;
    i32 oldHex_7;
    i32 oldMinX;
    i32 oldMinY_4;
    i32 finalDestination;

    destination_3 = GetAdjacentCellIndex(m_hex, direction);
    if (m_side == 1 && gpCombatManager->m_inCastleCombat
        && (destination_3 == 58 || destination_3 == 59
            || (destination_3 == 60 && m_side == 1 && HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)))
        && gpCombatManager->m_drawbridgeState == COMBAT_DRAWBRIDGE_RAISED) {
        m_animationSequence = ARMY_ANIMATION_STAND;
        m_animationFrame = 0;
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        gpCombatManager->LowerDoor();
        skipDrawing = 0;
    }

    giWalkingFrom = m_hex;
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
        giWalkingFrom2 = (static_cast<u32>(m_facing - 1) < 1 ? 1 : -1) + m_hex;
    } else {
        giWalkingFrom2 = -1;
    }
    giWalkingTo = destination_3;
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
        giWalkingTo2 = (static_cast<u32>(m_facing - 1) < 1 ? 1 : -1) + destination_3;
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
        gbComputeExtent = true;
        gbSaveBiggestExtent = true;
        gbReturnAfterComputeExtent = true;
        DrawToBuffer(
            gpCombatManager->m_hexCells[m_hex].m_x,
            gpCombatManager->m_hexCells[m_hex].m_y,
            0
        );
        gbReturnAfterComputeExtent = false;
        gbSaveBiggestExtent = false;
        gbComputeExtent = false;
    }
    if (giMinExtentX < 0)
        giMinExtentX = 0;
    if (giMinExtentY < 0)
        giMinExtentY = 0;
    if (giMaxExtentX > ARMY_COMBAT_MAX_X)
        giMaxExtentX = ARMY_COMBAT_MAX_X;
    if (giMaxExtentY > ARMY_COMBAT_MAX_Y)
        giMaxExtentY = ARMY_COMBAT_MAX_Y;
    oldMinX = giMinExtentX;
    oldMinY_4 = giMinExtentY;
    oldMaxX_1 = giMaxExtentX;
    oldMaxY_1 = giMaxExtentY;

    m_facingChanged = 0;
    if (direction < 3) {
        if (m_facing == 0) {
            m_facingChanged = 1;
            m_facing = 1 - m_facing;
            if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
                m_hex--;
            }
        }
    } else if (m_facing == 1) {
        m_facingChanged = 1;
        m_facing = 1 - m_facing;
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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
        gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_MOVE)]);
    }

    if (skipDrawing == 0) {
        gpCombatManager->m_hexCells[m_hex].m_occupantSide = -1;
        gpCombatManager->DrawFrame(0, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        gpCombatManager->m_hexCells[m_hex].m_occupantSide = gpCombatManager->m_currentArmySide;
        if (!gbNoShowCombat) {
            gpWindowManager->m_screen->CopyTo(
                gpCombatManager->m_backgroundBuffer,
                0,
                0,
                0,
                0,
                ARMY_COMBAT_WIDTH,
                ARMY_COMBAT_HEIGHT
            );
        }
        gpCombatManager->m_backgroundDrawn = 0;
    }

    if (!gbNoShowCombat) {
        for (frame = 0; frame < m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)];
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
                    giMaxExtentY - giMinExtentY + 1
                );
                if (giMinExtentX < 0)
                    giMinExtentX = 0;
                if (giMinExtentY < 0)
                    giMinExtentY = 0;
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
            gbComputeExtent = true;
            gbSaveBiggestExtent = true;
            gbReturnAfterComputeExtent = true;
            DrawToBuffer(
                gpCombatManager->m_hexCells[m_hex].m_x,
                gpCombatManager->m_hexCells[m_hex].m_y,
                0
            );
            gbReturnAfterComputeExtent = false;
            gbComputeExtent = false;
            gbSaveBiggestExtent = false;
            if (giMinExtentX < 0)
                giMinExtentX = 0;
            if (giMinExtentY < 0)
                giMinExtentY = 0;
            if (giMaxExtentX > ARMY_COMBAT_MAX_X)
                giMaxExtentX = ARMY_COMBAT_MAX_X;
            if (giMaxExtentY > ARMY_COMBAT_MAX_Y)
                giMaxExtentY = ARMY_COMBAT_MAX_Y;
            gbCurrArmyDrawn = false;
            gbComputeExtent = true;
            gbLimitToExtent = true;
            m_drawEnabled = 0;
            gpCombatManager->DrawFrame(0, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 0, 1);
            m_drawEnabled = 1;
            gbLimitToExtent = false;
            gbComputeExtent = false;
            gbCurrArmyDrawn = true;
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
            glTimers[0] = static_cast<i32>(
                KBTickCount()
                + m_frameInfo.walkDuration * gfCombatSpeedMod[gConfig.combatSpeed]
                      / m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)]
            );
            gpWindowManager->UpdateScreenRegion(
                oldMinX,
                oldMinY_4,
                oldMaxX_1 - oldMinX + 1,
                oldMaxY_1 - oldMinY_4 + 1
            );
        }
    }

    finalDestination = GetAdjacentCellIndex(m_hex, direction);
    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantFrame = -1;
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
        otherHex_1 = (static_cast<u32>(m_facing - 1) < 1 ? 1 : -1) + m_hex;
        gpCombatManager->m_hexCells[otherHex_1].m_occupantIndex = -1;
        gpCombatManager->m_hexCells[otherHex_1].m_occupantSide = -1;
        gpCombatManager->m_hexCells[otherHex_1].m_occupantFrame = -1;
    }
    gpCombatManager->m_hexCells[finalDestination].m_occupantSide = m_side;
    gpCombatManager->m_hexCells[finalDestination].m_occupantIndex = m_index;
    gpCombatManager->m_hexCells[finalDestination].m_occupantFrame = -1;
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
        otherHex_1 = (static_cast<u32>(m_facing - 1) < 1 ? 1 : -1) + finalDestination;
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
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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

// @semantic
// Logic, 0x1d8 frame/slots, CFG, and all external relocations align. Residuals
// are TU-cumulative army-array/extent operand order, equivalent float-compare
// polarity, constant-pool identities, one continuation jump, and the delinked
// recursive local call. Manual variants and an audited AST permutation pass
// were exhausted; revisit after later ARMY source/header changes.
VA(0x0044c7e5, 0x14aa)
void army::SpecialAttack(void) {
    char combatText[ARMY_COMBAT_TEXT_SIZE];
    i32 missileDelay_2;
    i32 minY_1;
    i32 oldX_16;
    char targetColumn_2;
    i32 minX_1;
    float angle_2;
    i32 missileHalfHeight_1;
    char sourceColumn_8;
    i32 attackDirection_3;
    i32 effectType_1;
    i32 initialXDistance_6;
    i32 yStep_6;
    i32 currentMissileY_7;
    i32 xStep_29;
    i32 currentMissileX_7;
    i32 effectY_28;
    char sourceRow;
    i32 missileSteps_7;
    i32 oldY_9;
    i32 effectX_2;
    char targetRow_1;
    char reverseMissile;
    i32 distance_6;
    i32 originalFacing_6;
    i32 initialYDistance_8;
    i32 backgroundX_4;
    i32 maxX_3;
    i32 xDistance_7;
    i32 maxY_3;
    bitmap* missileBackground;
    i32 backgroundY_6;
    i32 originalAttack;
    i32 direction;
    i32 missileHalfWidth;
    i32 sourceY_1;
    i32 targetY_1;
    i32 missileSpacing_1;
    i32 missileY_7;
    i32 endX_36;
    i32 yDistance_16;
    i32 sourceX_1;
    i32 targetX_1;
    i32 missileX_7;
    i32 damage;
    float slope;
    i32 directionFrame_2;
    i32 endY_19;
    i32 killed_13;
    army* target_1;

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
    if (!(targetColumn_2 <= sourceColumn_8)
        || (!(sourceRow & 1) && targetColumn_2 == sourceColumn_8)) {
        m_facing = 1;
    } else {
        m_facing = 0;
    }
    if (m_facing != originalFacing_6) {
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
            if (m_facing == 1) {
                m_hex--;
            } else {
                m_hex++;
            }
        }
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }

    CheckLuck();
    gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_SHOT)]);
    gpCombatManager->ResetLimitCreature();
    gpCombatManager->m_limitCreatureCount[m_side][m_index]++;
    gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);

    targetX_1 = target_1->MidX();
    targetY_1 = target_1->MidY();
    if (m_monsterType == CREATURE_LICH || m_monsterType == CREATURE_POWER_LICH) {
        targetX_1 = gpCombatManager->m_hexCells[target_1->m_hex].m_x;
        targetY_1 = gpCombatManager->m_hexCells[target_1->m_hex].m_y - 17;
    }
    if (m_facing == 1) {
        sourceX_1 = gpCombatManager->m_hexCells[m_hex].m_x + m_frameInfo.missileOffsets[1].x;
    } else {
        sourceX_1 = gpCombatManager->m_hexCells[m_hex].m_x - m_frameInfo.missileOffsets[1].x;
    }
    sourceY_1 = gpCombatManager->m_hexCells[m_hex].m_y + m_frameInfo.missileOffsets[1].y;
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
        angle_2 = static_cast<float>(atan(static_cast<double>(slope)) * 180.0 / ARMY_PROJECTILE_PI);
        for (direction = 1; direction < m_frameInfo.projectileDirectionCount; direction++) {
            if ((m_frameInfo.projectileAngles[direction - 1]
                 + m_frameInfo.projectileAngles[direction])
                    / 2.0f
                < angle_2) {
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
         m_animationFrame < m_frameInfo.animationFrameCount[IDX(m_animationSequence)];
         m_animationFrame++) {
        if (m_animationFrame == m_frameInfo.animationFrameCount[IDX(m_animationSequence)] - 1) {
            gpCombatManager->DrawFrame(0, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        } else {
            gpCombatManager->DrawFrame(1, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        }
        glTimers[0] = static_cast<i32>(
            KBTickCount()
            + m_frameInfo.attackDuration * gfCombatSpeedMod[gConfig.combatSpeed]
                  / m_frameInfo.animationFrameCount[IDX(m_animationSequence)]
        );
    }
    m_animationFrame = m_frameInfo.animationFrameCount[IDX(m_animationSequence)] - 1;

    missileHalfWidth = 25;
    missileHalfHeight_1 = 25;
    missileSpacing_1 = 31;
    missileDelay_2 = 25;
    if (m_monsterType == CREATURE_LICH || m_monsterType == CREATURE_POWER_LICH) {
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
        missileX_7 = gpCombatManager->m_hexCells[m_hex].m_x
                     + m_frameInfo.missileOffsets[attackDirection_3].x;
    } else {
        missileX_7 = gpCombatManager->m_hexCells[m_hex].m_x
                     - m_frameInfo.missileOffsets[attackDirection_3].x;
    }
    missileY_7 =
        gpCombatManager->m_hexCells[m_hex].m_y + m_frameInfo.missileOffsets[attackDirection_3].y;
    endX_36 = target_1->MidX();
    endY_19 = target_1->MidY();
    xDistance_7 = endX_36 - missileX_7;
    yDistance_16 = endY_19 - missileY_7;
    distance_6 = static_cast<i32>(
        sqrt(static_cast<double>(xDistance_7 * xDistance_7 + yDistance_16 * yDistance_16))
    );
    missileSteps_7 = (distance_6 + (missileSpacing_1 >> 1)) / missileSpacing_1;

    if (m_monsterType == CREATURE_MAGE || m_monsterType == CREATURE_ARCHMAGE) {
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
        DelayMilli(static_cast<i32l>(gfCombatSpeedMod[gConfig.combatSpeed] * IDX(ARMY_MAGE_BOLT_DELAY)));
        gpCombatManager->DoBolt(
            1,
            missileX_7,
            missileY_7,
            endX_36,
            endY_19,
            0,
            0,
            5,
            4,
            302,
            0,
            0,
            distance_6 / 15 + 15,
            1,
            0,
            10,
            0
        );
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
            static_cast<i16>(missileHalfWidth * 2),
            static_cast<i16>(missileHalfHeight_1 * 2)
        );
        missileBackground->GrabBitmapCareful(
            gpWindowManager->m_screen,
            static_cast<i16>(currentMissileX_7 - missileHalfWidth),
            static_cast<i16>(currentMissileY_7 - missileHalfHeight_1)
        );
        oldX_16 = currentMissileX_7;
        oldY_9 = currentMissileY_7;
        backgroundX_4 = 0;
        backgroundY_6 = 0;
        for (direction = 0; direction < missileSteps_7; direction++) {
            if (oldX_16 - missileHalfWidth < minX_1) {
                minX_1 = oldX_16 - missileHalfWidth;
            }
            if (minX_1 < 0)
                minX_1 = 0;
            if (maxX_3 < oldX_16 + missileHalfWidth) {
                maxX_3 = oldX_16 + missileHalfWidth;
            }
            if (maxX_3 > ARMY_COMBAT_MAX_X)
                maxX_3 = ARMY_COMBAT_MAX_X;
            if (oldY_9 - missileHalfHeight_1 < minY_1) {
                minY_1 = oldY_9 - missileHalfHeight_1;
            }
            if (minY_1 < 0)
                minY_1 = 0;
            if (maxY_3 < oldY_9 + missileHalfHeight_1) {
                maxY_3 = oldY_9 + missileHalfHeight_1;
            }
            if (maxY_3 > ARMY_COMBAT_MAX_Y)
                maxY_3 = ARMY_COMBAT_MAX_Y;
            if (direction != 0) {
                missileBackground->DrawToBufferCareful(
                    static_cast<i16>(backgroundX_4),
                    static_cast<i16>(backgroundY_6)
                );
            } else {
                if (minX_1 < giMinExtentX)
                    giMinExtentX = minX_1;
                if (giMaxExtentX < maxX_3)
                    giMaxExtentX = maxX_3;
                if (giMinExtentY > minY_1)
                    giMinExtentY = minY_1;
                if (maxY_3 > giMaxExtentY)
                    giMaxExtentY = maxY_3;
            }
            backgroundX_4 = currentMissileX_7 - missileHalfWidth;
            if (backgroundX_4 < 0)
                backgroundX_4 = 0;
            if (backgroundX_4 + missileBackground->m_width > ARMY_COMBAT_WIDTH) {
                backgroundX_4 = ARMY_COMBAT_WIDTH - missileBackground->m_width;
            }
            backgroundY_6 = currentMissileY_7 - missileHalfHeight_1;
            if (backgroundY_6 < 0)
                backgroundY_6 = 0;
            if (backgroundY_6 + missileBackground->m_height > ARMY_COMBAT_WIDTH) {
                backgroundY_6 = ARMY_COMBAT_WIDTH - missileBackground->m_height;
            }
            missileBackground->GrabBitmapCareful(
                gpWindowManager->m_screen,
                static_cast<i16>(backgroundX_4),
                static_cast<i16>(backgroundY_6)
            );
            m_missileIcon->DrawToBuffer(
                currentMissileX_7,
                currentMissileY_7,
                directionFrame_2,
                reverseMissile
            );
            if (direction == 0) {
                gpWindowManager->UpdateScreenRegion(
                    giMinExtentX,
                    giMinExtentY,
                    giMaxExtentX - giMinExtentX + 1,
                    giMaxExtentY - giMinExtentY + 1
                );
            } else {
                DelayTil(glTimers);
                gpWindowManager
                    ->UpdateScreenRegion(minX_1, minY_1, maxX_3 - minX_1 + 1, maxY_3 - minY_1 + 1);
            }
            glTimers[0] = static_cast<i32>(
                KBTickCount() + missileDelay_2 * gfCombatSpeedMod[gConfig.combatSpeed]
            );
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
            static_cast<i16>(backgroundX_4),
            static_cast<i16>(backgroundY_6)
        );
        gpWindowManager->UpdateScreenRegion(
            oldX_16 - missileHalfWidth,
            oldY_9 - missileHalfHeight_1,
            missileHalfWidth * 2,
            missileHalfHeight_1 * 2
        );
        delete missileBackground;
        if (!gpCombatManager->m_heroes[m_side]
            || !gpCombatManager->m_heroes[m_side]->HasArtifact(ARTIFACT_AMMO_CART)) {
            m_monster.shots--;
        }
    }

    originalAttack = m_monster.attack;
    effectType_1 = -1;
    effectX_2 = -1;
    effectY_28 = -1;
    if (m_monsterType == CREATURE_LICH || m_monsterType == CREATURE_POWER_LICH) {
        i32 adjacentHex;
        army* splashTarget;

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
                splashTarget =
                    &gpCombatManager
                         ->m_armies[gpCombatManager->m_hexCells[adjacentHex].m_occupantSide]
                                   [gpCombatManager->m_hexCells[adjacentHex].m_occupantIndex];
                if (!gArmyEffected[splashTarget->m_side][splashTarget->m_index]
                    && (target_1 != splashTarget || direction == 6)) {
                    gArmyEffected[splashTarget->m_side][splashTarget->m_index] = 1;
                    DamageEnemy(splashTarget, &damage, &killed_13, 1, 0);
                }
            }
        }
        m_spellEffectYOffset = 0;
        effectType_1 = ARMY_LICH_EXPLOSION_EFFECT;
        effectX_2 = gpCombatManager->m_hexCells[adjacentHex].m_x;
        effectY_28 = gpCombatManager->m_hexCells[adjacentHex].m_y - 17;
        gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_EXTRA_ONE)]);
    } else {
        DamageEnemy(target_1, &damage, &killed_13, 1, 0);
    }
    m_monster.attack = originalAttack;

    if (killed_13 > 0) {
        if (damage == -1) {
            sprintf(gText, "The mirror image is destroyed!");
        } else {
            sprintf(
                gText,
                "%s %s %d %s.\n%d %s %s.",
                m_quantity > 1 ? gArmyNamesPlural[IDX(m_monsterType)]
                               : gArmyNames[IDX(m_monsterType)],
                m_quantity > 1 ? "do" : "does",
                damage,
                "damage",
                killed_13,
                killed_13 > 1 ? gArmyNamesPlural[IDX(target_1->m_monsterType)]
                              : gArmyNames[IDX(target_1->m_monsterType)],
                killed_13 > 1 ? "perish" : "perishes"
            );
            gText[0] -= ARMY_ASCII_CASE_OFFSET;
        }
    } else {
        sprintf(
            gText,
            "%s %s %d %s.",
            m_quantity > 1 ? gArmyNamesPlural[IDX(m_monsterType)] : gArmyNames[IDX(m_monsterType)],
            m_quantity > 1 ? "do" : "does",
            damage,
            "damage"
        );
        gText[0] -= ARMY_ASCII_CASE_OFFSET;
    }
    strcpy(combatText, gText);
    switch (m_monsterType) {
        case CREATURE_ARCHMAGE:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ARCHMAGE_DISPEL_CHANCE && target_1
                && target_1->SpellCastWorks(SpellType(102))) {
                target_1->m_spellEffect = 102;
            }
            break;
    }
    PowEffect(effectType_1, 0, effectX_2, effectY_28);
    gpCombatManager->CombatMessage(combatText, 1, 1, 0);
    WaitSample(ARMY_SAMPLE_SHOT);

    if (m_facing != originalFacing_6) {
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
            if (m_facing == 1) {
                m_hex++;
            } else {
                m_hex--;
            }
        }
        m_facing = originalFacing_6;
    }
    if (!bSecondAttack
        && (m_monsterType == CREATURE_ELF || m_monsterType == CREATURE_GRAND_ELF
            || m_monsterType == CREATURE_RANGER)
        && target_1->m_quantity > 0) {
        bSecondAttack = 1;
        SpecialAttack();
        bSecondAttack = 0;
    }
    if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]
        || m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]) {
        CancelSpellType(ArmySpellCancelType(1));
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }
}

VA(0x0044dc8f, 0x2b)
void army::DirDoAttack(i32 direction) {
    m_attackDirection = direction;
    DoAttack(0);
}

// @semantic
// Logic, frame slots, CFG, and all 48 relocation sites align. The remaining
// code differences are TU-cumulative side/index address evaluation at three
// army-array sites; direct, commuted, flat-pointer, and scalar-SIB spellings
// were audited. Other residual identities are delinked string literals.
VA(0x0044dcba, 0x4e7)
void army::DoHydraAttack(i32) {
    i32 damage_8;
    i32 killed_4;
    i32 targetIndex_9;
    i32 direction;
    i32 targetSide_8;
    i32 targetHex_2;
    i32 totalKilled_7;
    army* target_2;
    i16 attackMask_7;
    char combatText_8[200];
    i32 totalDamage_1;

    totalKilled_7 = 0;
    totalDamage_1 = totalKilled_7;
    gpCombatManager->ResetHitByCreature();
    if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]) {
        attackMask_7 = static_cast<i16>(GetAttackMask(m_hex, 2, -1));
    } else {
        attackMask_7 = static_cast<i16>(GetAttackMask(m_hex, 1, -1));
    }
    CheckLuck();
    gpCombatManager->ResetLimitCreature();
    gpCombatManager->m_limitCreatureCount[m_side][m_index]++;
    for (direction = 0; direction < ARMY_COMBAT_DIRECTION_COUNT; direction++) {
        if (!(attackMask_7 & (1 << direction))) {
            targetHex_2 = m_hex;
            if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)
                && ((m_facing == 0 && direction > 2)
                    || (m_facing == 1 && (direction < 3 || direction > 5)))) {
                if (m_facing == 0) {
                    targetHex_2 = m_hex - 1;
                } else {
                    targetHex_2 = m_hex + 1;
                }
            }
            targetHex_2 = GetAdjacentCellIndex(targetHex_2, direction);
            if (ValidHex(targetHex_2)) {
                targetSide_8 = gpCombatManager->m_hexCells[targetHex_2].m_occupantSide;
                targetIndex_9 = gpCombatManager->m_hexCells[targetHex_2].m_occupantIndex;
                if (targetSide_8 >= 0 && targetIndex_9 >= 0) {
                    gpCombatManager->m_limitCreatureCount[targetSide_8][targetIndex_9]++;
                    target_2 = &gpCombatManager->m_armies[targetSide_8][targetIndex_9];
                    if (!target_2->m_hitByCreature) {
                        target_2->m_hitByCreature = 1;
                        DamageEnemy(target_2, &damage_8, &killed_4, 0, 0);
                        totalDamage_1 += damage_8;
                        totalKilled_7 += killed_4;
                        gpCombatManager->m_limitCreatureCount[targetSide_8][targetIndex_9]++;
                    }
                }
            }
        }
    }
    gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    m_animationState = 1;
    m_pendingAnimationSequence = ARMY_ATTACK_DELAY_NORMAL;
    gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_ATTACK)]);
    if (totalKilled_7 > 0) {
        sprintf(
            gText,
            "%s %s %d %s.\n%d %s %s.",
            m_quantity > 1 ? gArmyNamesPlural[IDX(m_monsterType)] : gArmyNames[IDX(m_monsterType)],
            m_quantity > 1 ? "do" : "does",
            totalDamage_1,
            "damage",
            totalKilled_7,
            totalKilled_7 > 1 ? "creatures" : "creature",
            totalKilled_7 > 1 ? "perish" : "perishes"
        );
    } else {
        sprintf(
            gText,
            "%s %s %d %s.",
            m_quantity > 1 ? gArmyNamesPlural[IDX(m_monsterType)] : gArmyNames[IDX(m_monsterType)],
            m_quantity > 1 ? "do" : "does",
            totalDamage_1,
            "damage"
        );
    }
    gText[0] -= ARMY_ASCII_CASE_OFFSET;
    strcpy(combatText_8, gText);
    PowEffect(-1, 0, -1, -1);
    gpCombatManager->CombatMessage(combatText_8, 1, 1, 0);
    gpCombatManager->m_limitCreatureCount[m_side][m_index] = 1;
}

// @semantic
// Logic, the 0x124 frame, stack slots, switch-body order, and external relocation
// targets align. Excluding the 0x24 local-label jump table, the remaining code
// differences are two side/index evaluation orders, two condition-polarity
// trampolines, and eight five-byte inline continuations. The 0x2b compressed
// case map is byte-exact; all 11 jump-table DIR32 entries agree by case order.
// The two base-only self REL32 entries are the retail-resolved recursive calls;
// remaining relocation identities are strings, gConfig, and constant-pool names.
VA(0x0044e1a1, 0x1267)
void army::DoAttack(i32 retaliation) {
    i32 targetOriginalFacing_5;
    i32 originalDirection;
    army* breathTarget_6;
    i32 secondAttackDirection_6;
    i32 breathDamage;
    i32 desiredFacing;
    i32 effectStopsRetaliation_4;
    i32 originalFacing_6;
    i32 occupantSide_5;
    char combatText[ARMY_COMBAT_TEXT_SIZE];
    i32 damage;
    i32 targetHex_3;
    i32 killed_13;
    army* target_1;
    i32 breathKilled;
    i32 adjacentHex_1;
    i32 revivedQuantity_3;
    i32 breathHex_1;

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
    if (m_monsterType == CREATURE_HYDRA) {
        DoHydraAttack(retaliation);
    } else {
        originalDirection = m_attackDirection;
        targetHex_3 = m_hex;
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)
            && ((m_facing == 0 && m_attackDirection >= 3)
                || (m_facing == 1 && (m_attackDirection <= 2 || m_attackDirection >= 6)))) {
            if (originalFacing_6 == 0) {
                targetHex_3 = m_hex - 1;
            } else {
                targetHex_3 = m_hex + 1;
            }
        }
        targetHex_3 = GetAdjacentCellIndex(targetHex_3, m_attackDirection);
        target_1 =
            &gpCombatManager->m_armies[gpCombatManager->m_hexCells[targetHex_3].m_occupantSide]
                                      [gpCombatManager->m_hexCells[targetHex_3].m_occupantIndex];
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_BREATH_ATTACK)) {
            breathHex_1 = GetAdjacentCellIndex(targetHex_3, m_attackDirection);
            if (ValidHex(breathHex_1)
                && gpCombatManager->m_hexCells[breathHex_1].m_occupantSide >= 0
                && gpCombatManager->m_hexCells[breathHex_1].m_occupantIndex >= 0
                && (gpCombatManager->m_hexCells[breathHex_1].m_occupantSide != target_1->m_side
                    || gpCombatManager->m_hexCells[breathHex_1].m_occupantIndex
                           != target_1->m_index)) {
                breathTarget_6 =
                    &gpCombatManager
                         ->m_armies[gpCombatManager->m_hexCells[breathHex_1].m_occupantSide]
                                   [gpCombatManager->m_hexCells[breathHex_1].m_occupantIndex];
            }
        }
        gpCombatManager->ResetLimitCreature();
        gpCombatManager->m_limitCreatureCount[m_side][m_index]++;
        gpCombatManager->m_limitCreatureCount[target_1->m_side][target_1->m_index]++;
        if (breathTarget_6) {
            breathTarget_6
                ->m_index[gpCombatManager->m_limitCreatureCount[breathTarget_6->m_side]]++;
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
            if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
                if (desiredFacing == 1) {
                    m_hex--;
                } else {
                    m_hex++;
                }
            }
            target_1->m_facing = 1 - m_facing;
            if (target_1->m_facing != targetOriginalFacing_5
                && HAS(target_1->m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
                if (target_1->m_facing == 1) {
                    target_1->m_hex--;
                } else {
                    target_1->m_hex++;
                }
            }
        }
        CheckLuck();
        m_animationState = 1;
        if (m_attackDirection == 6 || m_attackDirection == 5 || m_attackDirection == 0) {
            m_pendingAnimationSequence = ARMY_ATTACK_DELAY_SHORT;
        } else if (m_attackDirection == 1 || m_attackDirection == 4) {
            m_pendingAnimationSequence = ARMY_ATTACK_DELAY_NORMAL;
        } else {
            m_pendingAnimationSequence = ARMY_ATTACK_DELAY_LONG;
        }
        if (breathTarget_6) {
            m_pendingAnimationSequence += ARMY_BREATH_ATTACK_DELAY_BONUS;
        }
        gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_ATTACK)]);
        DamageEnemy(target_1, &damage, &killed_13, 0, 0);
        if (breathTarget_6) {
            DamageEnemy(breathTarget_6, &breathDamage, &breathKilled, 0, 0);
        }
        if (damage == -1) {
            sprintf(gText, "The mirror image is destroyed!");
        } else if (gbGenieHalf) {
            sprintf(
                gText,
                "%s %s half the enemy troops!",
                m_quantity > 1 ? gArmyNamesPlural[IDX(m_monsterType)]
                               : gArmyNames[IDX(m_monsterType)],
                m_quantity > 1 ? "destroy" : "destroys"
            );
            gText[0] -= ARMY_ASCII_CASE_OFFSET;
        } else {
            if (killed_13 > 0) {
                sprintf(
                    gText,
                    "%s %s %d %s.\n%d %s %s.",
                    m_quantity > 1 ? gArmyNamesPlural[IDX(m_monsterType)]
                                   : gArmyNames[IDX(m_monsterType)],
                    m_quantity > 1 ? "do" : "does",
                    damage,
                    "damage",
                    killed_13,
                    killed_13 > 1 ? gArmyNamesPlural[IDX(target_1->m_monsterType)]
                                  : gArmyNames[IDX(target_1->m_monsterType)],
                    killed_13 > 1 ? "perish" : "perishes"
                );
                gText[0] -= ARMY_ASCII_CASE_OFFSET;
            } else {
                sprintf(
                    gText,
                    "%s %s %d %s.",
                    m_quantity > 1 ? gArmyNamesPlural[IDX(m_monsterType)]
                                   : gArmyNames[IDX(m_monsterType)],
                    m_quantity > 1 ? "do" : "does",
                    damage,
                    "damage"
                );
                gText[0] -= ARMY_ASCII_CASE_OFFSET;
            }
        }
        strcpy(combatText, gText);

        switch (m_monsterType) {
            case CREATURE_CYCLOPS:
                if (target_1->m_quantity > 0) {
                    if (!breathTarget_6 || breathTarget_6->m_quantity > 0) {
                        if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE) {
                            if (target_1 && target_1->SpellCastWorks(SPELL_PARALYZE)) {
                                target_1->m_spellEffect = IDX(SPELL_PARALYZE);
                                effectStopsRetaliation_4 = 1;
                            }
                        } else if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE
                                   && breathTarget_6
                                   && breathTarget_6->SpellCastWorks(SPELL_PARALYZE)) {
                            breathTarget_6->m_spellEffect = IDX(SPELL_PARALYZE);
                        }
                    }
                }
                break;
            case CREATURE_UNICORN:
                if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target_1
                    && target_1->SpellCastWorks(SPELL_BLIND)) {
                    target_1->m_spellEffect = IDX(SPELL_BLIND);
                    effectStopsRetaliation_4 = 1;
                }
                break;
            case CREATURE_MEDUSA:
                if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target_1
                    && target_1->SpellCastWorks(CREATURE_SPELL_PETRIFY)) {
                    target_1->m_spellEffect = IDX(CREATURE_SPELL_PETRIFY);
                    effectStopsRetaliation_4 = 1;
                }
                break;
            case CREATURE_MUMMY:
                if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE) {
                    goto applyMummySpell;
                }
                break;
            case CREATURE_ROYAL_MUMMY:
                if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ROYAL_MUMMY_EFFECT_CHANCE) {
                applyMummySpell:
                    if (target_1 && target_1->SpellCastWorks(SPELL_CURSE)) {
                        target_1->m_spellEffect = IDX(SPELL_CURSE);
                    }
                }
                break;
            case CREATURE_ARCHMAGE:
                if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target_1
                    && target_1->SpellCastWorks(CREATURE_SPELL_DISPEL)) {
                    target_1->m_spellEffect = IDX(CREATURE_SPELL_DISPEL);
                }
                break;
            case CREATURE_GHOST:
                gpCombatManager->m_killBenefit[gpCombatManager->m_hexCells[m_hex].m_occupantSide] =
                    killed_13;
                break;
            case CREATURE_VAMPIRE_LORD:
                gpCombatManager->m_killBenefit[gpCombatManager->m_hexCells[m_hex].m_occupantSide] =
                    target_1->m_monster.hitPoints * killed_13;
                break;
        }
        PowEffect(-1, 0, -1, -1);
        gpCombatManager->CombatMessage(combatText, 1, 1, 0);
        m_index[gpCombatManager->m_limitCreatureCount[m_side]] = 1;

        if (m_monsterType == CREATURE_GHOST) {
            m_quantity +=
                gpCombatManager->m_killBenefit[gpCombatManager->m_hexCells[m_hex].m_occupantSide];
        }
        if (m_monsterType == CREATURE_VAMPIRE_LORD) {
            if (gpCombatManager->m_killBenefit[gpCombatManager->m_hexCells[m_hex].m_occupantSide]
                < m_hitPointsLost) {
                m_hitPointsLost -=
                    gpCombatManager
                        ->m_killBenefit[gpCombatManager->m_hexCells[m_hex].m_occupantSide];
            } else {
                gpCombatManager->m_killBenefit[gpCombatManager->m_hexCells[m_hex].m_occupantSide] -=
                    m_hitPointsLost;
                m_hitPointsLost = 0;
                revivedQuantity_3 =
                    gpCombatManager
                        ->m_killBenefit[gpCombatManager->m_hexCells[m_hex].m_occupantSide]
                    / m_monster.hitPoints;
                if (revivedQuantity_3 < m_initialQuantity - m_quantity) {
                    m_quantity += revivedQuantity_3;
                } else {
                    m_quantity = m_initialQuantity;
                }
            }
        }

        if (target_1 && target_1->m_quantity > 0
            && !target_1->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)]
            && !target_1->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]
            && (target_1->m_monsterType == CREATURE_GRIFFIN
                || !HAS(target_1->m_monster.flags.all, MONSTER_FLAGS_RETALIATED))
            && m_monsterType != CREATURE_ROGUE && m_monsterType != CREATURE_SPRITE
            && m_monsterType != CREATURE_VAMPIRE && m_monsterType != CREATURE_VAMPIRE_LORD
            && !effectStopsRetaliation_4 && !retaliation) {
            DelayMilli(
                static_cast<i32l>(gfCombatSpeedMod[gConfig.combatSpeed] * IDX(ARMY_RETALIATION_DELAY))
            );
            target_1->m_attackDirection = OppositeDirection(m_attackDirection);
            if (HAS(target_1->m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
                adjacentHex_1 = GetAdjacentCellIndex(
                    target_1->m_hex,
                    (static_cast<u32>(target_1->m_facing - 1) < 1 ? 0 : -1) & 5
                );
                if (adjacentHex_1 == m_hex) {
                    target_1->m_attackDirection = 6;
                }
                adjacentHex_1 = GetAdjacentCellIndex(
                    target_1->m_hex,
                    static_cast<u32>(target_1->m_facing - 1) < 1 ? 2 : 3
                );
                if (adjacentHex_1 == m_hex) {
                    target_1->m_attackDirection = 7;
                }
            }
            target_1->DoAttack(1);
            target_1->m_monster.attributes |= MONSTER_ATTRIBUTE_RETALIATED;
            if (gbRemoteOn && gpCombatManager->m_networkArmyPresent[0]
                && gpCombatManager->m_networkArmyPresent[1]
                && target_1->m_monsterType == CREATURE_GHOST) {
                target_1->m_quantity +=
                    gpCombatManager->m_killBenefit[gpCombatManager->m_hexCells[target_1->m_hex]
                                                       .m_occupantSide];
            }
        }
        if ((m_monsterType == CREATURE_WOLF || m_monsterType == CREATURE_PALADIN
             || m_monsterType == CREATURE_CRUSADER)
            && target_1 && target_1->m_quantity > 0 && !retaliation
            && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)]
            && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]
            && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)] && m_quantity > 0) {
            DelayMilli(
                static_cast<i32l>(gfCombatSpeedMod[gConfig.combatSpeed] * IDX(ARMY_SECOND_ATTACK_DELAY))
            );
            secondAttackDirection_6 = m_attackDirection;
            m_attackDirection = originalDirection;
            DoAttack(1);
            m_attackDirection = secondAttackDirection_6;
        }
        if (m_facing != originalFacing_6) {
            if (!HAS(m_monster.flags.all, MONSTER_FLAGS_DEAD)) {
                m_facing = originalFacing_6;
                if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
                    if (originalFacing_6 == 1) {
                        m_hex--;
                    } else {
                        m_hex++;
                    }
                }
            }
            if (!HAS(target_1->m_monster.flags.all, MONSTER_FLAGS_DEAD)
                && target_1->m_facing != targetOriginalFacing_5) {
                target_1->m_facing = targetOriginalFacing_5;
                if (HAS(target_1->m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
                    if (target_1->m_facing == 1) {
                        target_1->m_hex--;
                    } else {
                        target_1->m_hex++;
                    }
                }
            }
        }
    }
    if (!retaliation
        && (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]
            || m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])) {
        CancelSpellType(ArmySpellCancelType(1));
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }
    targetHex_3 = -1;
    m_targetSide = targetHex_3;
    if (retaliation) {
        gpCombatManager->m_currentSide = 1 - gpCombatManager->m_currentSide;
    }
}

VA(0x0044f408, 0x16)
void army::ResetPath(void) {
    return;
}

VA(0x0044f41e, 0x25)
i32 army::WalkTo(void) {
    return WalkTo(m_moveTargetHex);
}

// @semantic
// Complete 0x2ee body, 0x18 frame/slots, CFG, and all 19 ordered relocation
// targets agree. At +0x12e retail calls GetAdjacentCellIndex before loading
// moatCell[moatIndex]; base loads the table byte before the call. Swapped
// equality operands and qualifying the table lvalue were byte-neutral.
VA(0x0044f443, 0x2ee)
i32 army::WalkTo(i32 destination) {
    i32 direction_3;
    i32 steps;
    i32 moatDestination;
    i32 moatIndex_1;
    i32 canEnterMoat_1;

    m_targetIndex = -1;
    m_targetSide = m_targetIndex;
    if (gpCombatManager->m_drawbridgeBackgroundVisible
        && HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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
            if (moatIndex_1 == ARMY_MOAT_GATE_INDEX
                && gpCombatManager->m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN) {
                canEnterMoat_1 = 1;
            }
            if ((moatIndex_1 > 0 && moatCell[moatIndex_1 - 1] == m_hex)
                || (moatIndex_1 < ARMY_MOAT_CELL_COUNT - 1 && moatCell[moatIndex_1 + 1] == m_hex)) {
                canEnterMoat_1 = 1;
            }
            for (direction_3 = 0; direction_3 < ARMY_ADJACENT_DIRECTION_COUNT; direction_3++) {
                if (moatCell[moatIndex_1] == GetAdjacentCellIndex(m_hex, direction_3)) {
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
    for (direction_3 = gpSearchArray->m_pathLength - 1; direction_3 >= 0; direction_3--) {
        Walk(
            static_cast<u8>(gpSearchArray->m_storage.path.directions[direction_3 + 1]),
            0,
            gpSearchArray->m_pathLength - 1 != direction_3
        );
        steps++;
        if (steps >= m_monster.speed) {
            direction_3 = -1;
        }
    }
    CancelSpellType(ArmySpellCancelType(0));
    m_animationSequence = ARMY_ANIMATION_STAND;
    m_animationFrame = 0;
    gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    gpCombatManager->TestRaiseDoor();
    return 0;
}

VA(0x0044f731, 0x25)
i32 army::AttackTo(void) {
    return AttackTo(m_moveTargetHex);
}

// @semantic: Complete 0x10 frame, CFG, and ordered relocations. The only raw
// residual is +0x5e..+0x63: retail evaluates m_moveTargetHex then m_hex, while
// MSVC evaluates the commutative equality in the opposite order. Both source
// operand orders and the value-preserving |0 spelling emitted the same pair.
VA(0x0044f756, 0x1e8)
i32 army::AttackTo(i32 destination) {
    i32 finishStanding;
    i32 pathIndex_4;
    i32 steps;

    if (HAS(m_monster.flags.all, MONSTER_FLAGS_FLYING)) {
        if (m_hex != destination) {
            FlyTo(destination);
        }
        DoAttack(0);
        return 0;
    }
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_BREATH_ATTACK) && m_hex == m_moveTargetHex) {
        DoAttack(0);
        return 0;
    }
    if (FindPath(m_hex, destination, m_monster.speed, 1, 0)) {
        if (gpSearchArray->m_pathLength == 1) {
            m_attackDirection = static_cast<u8>(gpSearchArray->m_storage.path.directions[1]);
            gpCombatManager->TestRaiseDoor();
            DoAttack(0);
        } else {
            pathIndex_4 = 0;
            steps = 0;
            for (pathIndex_4 = gpSearchArray->m_pathLength - 1; pathIndex_4 != 0; pathIndex_4--) {
                steps++;
                if (pathIndex_4 == 1 || steps >= m_monster.speed) {
                    finishStanding = 1;
                } else {
                    finishStanding = 0;
                }
                Walk(
                    static_cast<u8>(gpSearchArray->m_storage.path.directions[pathIndex_4 + 1]),
                    finishStanding,
                    gpSearchArray->m_pathLength - 1 != pathIndex_4
                );
                if (steps >= m_monster.speed && pathIndex_4 != 1) {
                    return ARMY_PATH_BLOCKED;
                }
            }
            CancelSpellType(ArmySpellCancelType(0));
            m_attackDirection = static_cast<u8>(gpSearchArray->m_storage.path.directions[1]);
            gpCombatManager->TestRaiseDoor();
            DoAttack(0);
        }
        return 0;
    }
    return ARMY_PATH_BLOCKED;
}

// @semantic: The complete behavior and 0x1c frame are recovered, including
// distinct bad/good army-name lifetimes and all 37 ordered relocations. The
// SAMPLE2 return temporary is ours -0x18/-0x14 versus retail -0x10/-0x0c;
// the name slots are ours -0x0c/-0x10 versus retail -0x14/-0x18. Function-wide,
// branch-local, isolated-assignment, and two-name parent-scope forms were tried.
VA(0x0044f93e, 0x282)
void army::CheckLuck(void) {
    SAMPLE2 luckSample;

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
        char* badLuckArmyName;
        char* goodLuckArmyName;

        luckSample = NULL_SAMPLE2;
        if (m_luckOutcome < 0) {
            sprintf(gText, "badluck.82m");
        } else {
            sprintf(gText, "goodluck.82m");
        }
        luckSample = LoadPlaySample(gText);
        if (m_luckOutcome < 0) {
            if (m_quantity > 1) {
                badLuckArmyName = gArmyNamesPlural[IDX(m_monsterType)];
            } else {
                badLuckArmyName = gArmyNames[IDX(m_monsterType)];
            }
            sprintf(gText, "Bad luck descends on the %s", badLuckArmyName);
            gpCombatManager->CombatMessage(gText, 1, 1, 0);
            SpellEffect(ARMY_BAD_LUCK_EFFECT, ARMY_BAD_LUCK_EFFECT_DELAY, 0);
        } else {
            if (m_quantity > 1) {
                goodLuckArmyName = gArmyNamesPlural[IDX(m_monsterType)];
            } else {
                goodLuckArmyName = gArmyNames[IDX(m_monsterType)];
            }
            sprintf(gText, "Good luck shines on the %s", goodLuckArmyName);
            gpCombatManager->CombatMessage(gText, 1, 1, 0);
            gpCombatManager->DoLuck(m_side, m_index);
        }
        WaitEndSample(luckSample, -1);
        if (m_luckOutcome > 0) {
            gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
            gpMouseManager->ShowColorPointer();
        }
    }
}

// @semantic
// All 41 relocation sites align. The only non-pool code difference is the
// value-equivalent genie clamp operand order at target 0x581f-0x5825; direct,
// reversed, negated, and empty-arm comparison spellings were audited.
VA(0x0044fbc0, 0x56e)
void army::DamageEnemy(
    army* target,
    i32* damageResult,
    i32* killedResult,
    i32 rangedAttack,
    i32 defenseModifier
) {
    float damage1;
    i32 attackBonus6;
    i32 defenseBonus9;
    i32 attackDifference7;
    i32 rearHex19;
    i32 index16;
    i32 damageDone2;
    i32 genieDamage26;
    hero* commander1;

    if (!target) {
        return;
    }
    damage1 = 0;
    gbGenieHalf = false;
    for (index16 = 0; index16 < m_quantity; index16++) {
        if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLESS)]) {
            damage1 += m_monster.damageMax;
        } else if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLESS)]) {
            damage1 += m_monster.damageMin;
        } else {
            damage1 += SRandom(m_monster.damageMin, m_monster.damageMax);
        }
    }
    attackBonus6 = 0;
    defenseBonus9 = 0;
    attackDifference7 = m_monster.attack + attackBonus6
                        - (target->m_monster.defense + (defenseBonus9 + defenseModifier));
    if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER)]
        && (target->m_monsterType == CREATURE_GREEN_DRAGON
            || target->m_monsterType == CREATURE_RED_DRAGON
            || target->m_monsterType == CREATURE_BLACK_DRAGON
            || target->m_monsterType == CREATURE_BONE_DRAGON)) {
        attackDifference7 += ARMY_DRAGON_SLAYER_BONUS;
    }
    if (gpCombatManager->m_drawbridgeBackgroundVisible) {
        rearHex19 = -1;
        if (HAS(target->m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
            rearHex19 = (static_cast<u32>(target->m_facing - 1) < 1 ? 1 : -1) + target->m_hex;
        }
        for (index16 = 0; index16 < ARMY_MOAT_CELL_COUNT; index16++) {
            if (moatCell[index16] == target->m_hex || moatCell[index16] == rearHex19) {
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
    if ((m_monsterType == CREATURE_CRUSADER
         && HAS(target->m_monster.flags.all, MONSTER_FLAGS_UNDEAD))
        || (m_monsterType == CREATURE_EARTH_ELEMENTAL
            && target->m_monsterType == CREATURE_AIR_ELEMENTAL)
        || (m_monsterType == CREATURE_AIR_ELEMENTAL
            && target->m_monsterType == CREATURE_EARTH_ELEMENTAL)
        || (m_monsterType == CREATURE_WATER_ELEMENTAL
            && target->m_monsterType == CREATURE_FIRE_ELEMENTAL)
        || (m_monsterType == CREATURE_FIRE_ELEMENTAL
            && target->m_monsterType == CREATURE_WATER_ELEMENTAL)) {
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
        damage1 *= gfSSArcheryMod[commander1->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]];
    }
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_SHOOTER) && !rangedAttack
        && m_monsterType != CREATURE_TITAN && m_monsterType != CREATURE_MAGE
        && m_monsterType != CREATURE_ARCHMAGE) {
        damage1 /= 2.0f;
    }
    if (rangedAttack && target->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_SHIELD)]) {
        damage1 /= 2.0f;
    }
    if (m_damagePenalty == 2) {
        damage1 /= 2.0f;
    }
    m_damagePenalty = 0;
    if (target->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]) {
        damage1 /= 2.0f;
    }
    damageDone2 = static_cast<i32>(damage1 + 0.5);
    if (m_monsterType == CREATURE_GENIE
        && SRandom(1, ARMY_GENIE_HALF_ROLL_MAX) == ARMY_GENIE_HALF_ROLL) {
        genieDamage26 = ((target->m_quantity + 1) / 2) * target->m_monster.hitPoints;
        if (damageDone2 < genieDamage26) {
            gbGenieHalf = true;
            damageDone2 = genieDamage26;
        }
    }
    if (damageDone2 <= 0) {
        damageDone2 = 1;
    }
    if (HAS(target->m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)) {
        damageDone2 = -1;
    }
    *damageResult = damageDone2;
    *killedResult = target->Damage(damageDone2, SPELL_NONE);
}

// @semantic: Complete behavior, 0x10 frame, CFG, and ordered relocations. The
// first residual is +0x1ca: retail evaluates current side before army index in
// the m_armies address, while the candidate evaluates the equivalent two
// subscripts in reverse order. Direct and commuted subscript forms were checked.
VA(0x0045012e, 0x23c)
i32 army::Damage(i32l damage, SpellType spell) {
    i32 killed_13;
    i32 originalFacing;
    i32 quantityFifth;

    damage += m_hitPointsLost;
    if (spell != SPELL_NONE) {
        if (gbRemoteOn) {
            gpCombatManager->ModifyDamageForArtifacts(
                &damage,
                IDX(spell),
                gpCombatManager->m_heroes[m_side],
                gpCombatManager->m_heroes[gpCombatManager->m_currentSide]
            );
        } else {
            gpCombatManager->ModifyDamageForArtifacts(
                &damage,
                IDX(spell),
                gpCombatManager->m_heroes[gpCombatManager->m_currentSide],
                gpCombatManager->m_heroes[m_side]
            );
        }
    }
    killed_13 = damage / m_monster.hitPoints;
    m_hitPointsLost = damage % m_monster.hitPoints;
    quantityFifth = m_quantity / 5;
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)) {
        killed_13 = m_quantity;
        m_hitPointsLost = 0;
    }
    if (!quantityFifth) {
        quantityFifth = 1;
    }
    m_damagePending = 1;
    if (killed_13 > 0) {
        m_killPending = 1;
        m_lastTargetHex = m_quantity;
    }
    if (killed_13 > m_quantity) {
        killed_13 = m_quantity;
    }
    m_quantity -= killed_13;
    if (m_quantity <= 0) {
        m_deathPending = 1;
    }
    originalFacing = m_facing;
    m_facing =
        gpCombatManager
            ->m_armies[gpCombatManager->m_currentArmySide][gpCombatManager->m_currentArmyIndex]
            .m_facing
        ^ 1;
    m_facing = originalFacing;
    CancelSpellType(ARMY_CANCEL_SPELLS_AFTER_DAMAGE);
    return killed_13;
}

// @semantic
// The complete behavior, 0x58 frame, role-to-slot mapping, initialization
// order, CFG, and all 119 ordered relocations align. Residuals are four
// commutative max/add load orders plus three trailing retail padding NOPs;
// reversed comparison/addition spellings were byte-neutral, while typed direct
// limit-count indexing regressed both address sites.
VA(0x0045036a, 0x1361)
void army::PowEffect(i32 effect, i32 resetLimits, i32 effectX, i32 effectY) {
    i32 damageFrames;
    i32 side_4;
    i32 maximumDamageFrames_3;
    army* current;
    i32 drawEffect_1;
    i32 effectFrames_1;
    i32 index_10;
    i32 overlapAdjustment_7;
    i32 frame;
    i32 maximumEndFrames_1;
    i32 keepAnimating_1;
    i32 endFrames_1;
    i32 maximumStartFrames;
    i32 totalFrames_4;
    i32 frameDelay_6;
    i32 startFrames_5;
    IconEntry* entry_1;

    maximumStartFrames = 0;
    maximumEndFrames_1 = 0;
    maximumDamageFrames_3 = 0;
    effectFrames_1 = 0;
    totalFrames_4 = 0;
    startFrames_5 = 0;
    endFrames_1 = 0;
    damageFrames = 0;
    drawEffect_1 = 0;
    overlapAdjustment_7 = 1;
    if (m_monsterType == CREATURE_PALADIN || m_monsterType == CREATURE_CRUSADER) {
        overlapAdjustment_7 = 0;
    }
    if (m_monsterType == CREATURE_DWARF || m_monsterType == CREATURE_BATTLE_DWARF) {
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
    if (!gbNoShowCombat && effect != ARMY_NO_EFFECT && drawEffect_1
        && IDX(gCurLoadedSpellEffect) != OD_STEER(effect)) {
        gpResourceManager->Dispose(gCurLoadedSpellIcon);
        gCurLoadedSpellIcon = gpResourceManager->GetIcon(gCombatFxNames[effect]);
        gCurLoadedSpellEffect = SpellType(effect);
    }
    if (drawEffect_1) {
        effectFrames_1 = giNumPowFrames[IDX(gCurLoadedSpellEffect)];
    }
    for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
        for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
            current = &gpCombatManager->m_armies[side_4][index_10];
            if (static_cast<u8>(current->m_animationState)) {
                startFrames_5 =
                    current->m_frameInfo.animationFrameCount[m_pendingAnimationSequence];
                endFrames_1 =
                    current->m_frameInfo.animationFrameCount[m_pendingAnimationSequence + 1] + 1;
            } else {
                if (current->m_deathPending) {
                    damageFrames =
                        current->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_DEATH)];
                } else if (current->m_damagePending) {
                    damageFrames =
                        current->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE)]
                        + current->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE_RETURN)]
                        + 1;
                }
            }
            maximumStartFrames =
                maximumStartFrames > startFrames_5 ? maximumStartFrames : startFrames_5;
            maximumEndFrames_1 =
                maximumEndFrames_1 > endFrames_1 ? maximumEndFrames_1 : endFrames_1;
            maximumDamageFrames_3 =
                maximumDamageFrames_3 > damageFrames ? maximumDamageFrames_3 : damageFrames;
        }
    }
    totalFrames_4 = maximumStartFrames + maximumEndFrames_1;
    totalFrames_4 = maximumStartFrames + maximumDamageFrames_3 - overlapAdjustment_7 > totalFrames_4
                        ? maximumStartFrames + maximumDamageFrames_3 - overlapAdjustment_7
                        : totalFrames_4;
    totalFrames_4 = maximumDamageFrames_3 > totalFrames_4 ? maximumDamageFrames_3 : totalFrames_4;
    effectFrames_1 = effectFrames_1 > totalFrames_4 ? effectFrames_1 : totalFrames_4;
    frameDelay_6 = ARMY_POW_EFFECT_DELAY;
    if (resetLimits) {
        gpCombatManager->ResetLimitCreature();
    }
    for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
        for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
            if (gpCombatManager->m_armies[side_4][index_10].m_animationSequence
                    == ARMY_ANIMATION_SHOOT_UP
                || gpCombatManager->m_armies[side_4][index_10].m_animationSequence
                       == ARMY_ANIMATION_SHOOT_FORWARD
                || gpCombatManager->m_armies[side_4][index_10].m_animationSequence
                       == ARMY_ANIMATION_SHOOT_DOWN) {
                gpCombatManager->m_armies[side_4][index_10].m_animationCycle = 1;
            } else {
                gpCombatManager->m_armies[side_4][index_10].m_animationCycle = 0;
            }
            if ((gpCombatManager->m_armies[side_4][index_10].m_damagePending
                 || static_cast<u8>(gpCombatManager->m_armies[side_4][index_10].m_animationState)
                 || static_cast<u8>(gpCombatManager->m_armies[side_4][index_10].m_animationCycle))
                && !gpCombatManager->m_limitCreatureCount[side_4][index_10]) {
                gpCombatManager->m_limitCreatureCount[side_4][index_10]++;
            }
        }
    }
    gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    if (effectX != ARMY_NO_EFFECT) {
        for (index_10 = 0; index_10 < gCurLoadedSpellIcon->m_frameCount; index_10++) {
            entry_1 = &gCurLoadedSpellIcon->Entries()[index_10];
            giMinExtentX =
                entry_1->x + effectX < giMinExtentX ? entry_1->x + effectX : giMinExtentX;
            giMinExtentY =
                entry_1->y + effectY < giMinExtentY ? entry_1->y + effectY : giMinExtentY;
            giMaxExtentX = entry_1->x + entry_1->w + effectX - 1 > giMaxExtentX
                               ? entry_1->x + entry_1->w + effectX - 1
                               : giMaxExtentX;
            giMaxExtentY = entry_1->y + entry_1->h + effectY - 1 > giMaxExtentY
                               ? entry_1->y + entry_1->h + effectY - 1
                               : giMaxExtentY;
        }
        giMinExtentX = giMinExtentX > 0 ? giMinExtentX : 0;
        giMinExtentY = giMinExtentY > 0 ? giMinExtentY : 0;
        giMaxExtentX = giMaxExtentX < ARMY_COMBAT_MAX_X ? giMaxExtentX : ARMY_COMBAT_MAX_X;
        giMaxExtentY = giMaxExtentY < ARMY_COMBAT_MAX_Y ? giMaxExtentY : ARMY_COMBAT_MAX_Y;
    }
    for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
        for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
            current = &gpCombatManager->m_armies[side_4][index_10];
            current->m_effectAnimationStart = -1;
            current->m_effectAnimationEnd = -1;
            current->m_effectAnimationStarted = 0;
            if (current->m_damagePending || static_cast<u8>(current->m_animationState)) {
                if (static_cast<u8>(current->m_animationState)) {
                    current->m_effectAnimationStart = m_pendingAnimationSequence;
                    current->m_effectAnimationEnd = m_pendingAnimationSequence + 1;
                } else if (current->m_deathPending) {
                    current->m_effectAnimationStart = IDX(ARMY_ANIMATION_DEATH);
                } else {
                    current->m_effectAnimationStart = IDX(ARMY_ANIMATION_WINCE);
                    current->m_effectAnimationEnd = IDX(ARMY_ANIMATION_WINCE_RETURN);
                }
                if (current->m_effectAnimationStart == IDX(ARMY_ANIMATION_DEATH)) {
                    current->m_effectAnimationLength =
                        current->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_DEATH)];
                } else {
                    current->m_effectAnimationLength =
                        current->m_frameInfo.animationFrameCount[current->m_effectAnimationStart]
                        + current->m_frameInfo
                              .animationFrameCount[current->m_effectAnimationStart + 1];
                }
                if (IDX(current->m_animationSequence) == current->m_effectAnimationStart) {
                    current->m_effectAnimationLength--;
                }
                if (m_drawState < 2) {
                    m_drawState = 2;
                }
            }
        }
    }
    frame = 0;
    for (frame = 0; OD_STEER(effectFrames_1) > frame; frame++) {
        for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
            for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
                current = &gpCombatManager->m_armies[side_4][index_10];
                if (static_cast<u8>(current->m_animationCycle)) {
                    if (current->m_animationSequence == ARMY_ANIMATION_SHOOT_UP
                        || current->m_animationSequence == ARMY_ANIMATION_SHOOT_FORWARD
                        || current->m_animationSequence == ARMY_ANIMATION_SHOOT_DOWN) {
                        current->m_animationSequence++;
                        current->m_animationFrame = 0;
                    } else if (current->m_animationSequence != ARMY_ANIMATION_STAND) {
                        if (current->m_animationFrame + 1
                            < current->m_frameInfo
                                  .animationFrameCount[IDX(current->m_animationSequence)]) {
                            current->m_animationFrame++;
                        } else {
                            current->m_animationSequence = ARMY_ANIMATION_STAND;
                            current->m_animationFrame = 0;
                        }
                    }
                }
                if (current->m_effectAnimationStart != -1 && !current->m_effectAnimationStarted
                    && (static_cast<u8>(current->m_animationState)
                        || static_cast<i32>(effectFrames_1 - frame - 1)
                               <= current->m_effectAnimationLength
                        || (maximumStartFrames && frame >= maximumStartFrames - 1)
                        || (!maximumStartFrames
                            && current->m_animationSequence != ARMY_ANIMATION_WINCE_RETURN
                            && (current->m_animationSequence != ARMY_ANIMATION_WINCE
                                || current->m_animationFrame + 1
                                       < current->m_frameInfo.animationFrameCount
                                             [IDX(current->m_animationSequence)])))) {
                    if (IDX(current->m_animationSequence) != current->m_effectAnimationStart
                        && IDX(current->m_animationSequence) != current->m_effectAnimationEnd) {
                        if (!gbNoShowCombat
                            && current->m_effectAnimationStart == IDX(ARMY_ANIMATION_WINCE)) {
                            gpSoundManager->MemorySample(gpCombatManager->m_armies[side_4][index_10]
                                                             .m_samples[IDX(ARMY_SAMPLE_WINCE)]);
                        }
                        if (!gbNoShowCombat
                            && current->m_effectAnimationStart == IDX(ARMY_ANIMATION_DEATH)) {
                            gpSoundManager->MemorySample(gpCombatManager->m_armies[side_4][index_10]
                                                             .m_samples[IDX(ARMY_SAMPLE_KILL)]);
                        }
                        current->m_animationSequence = ArmyAnimationSequence(current->m_effectAnimationStart);
                        current->m_animationFrame = 0;
                    } else {
                        if (current->m_animationFrame + 1
                            < current->m_frameInfo
                                  .animationFrameCount[IDX(current->m_animationSequence)]) {
                            current->m_animationFrame++;
                        } else if (IDX(current->m_animationSequence) != current->m_effectAnimationEnd
                                   && current->m_effectAnimationEnd != -1) {
                            current->m_animationSequence = ArmyAnimationSequence(current->m_effectAnimationEnd);
                            current->m_animationFrame = 0;
                        } else {
                            if (current->m_animationSequence != ARMY_ANIMATION_STAND
                                && current->m_animationSequence != ARMY_ANIMATION_DEATH) {
                                current->m_animationSequence = ARMY_ANIMATION_STAND;
                                current->m_animationFrame = 0;
                                current->m_effectAnimationStarted = 1;
                            }
                        }
                    }
                }
            }
        }
        glTimers[0] =
            static_cast<i32>(KBTickCount() + frameDelay_6 * gfCombatSpeedMod[gConfig.combatSpeed]);
        if (drawEffect_1 && frame < giNumPowFrames[IDX(gCurLoadedSpellEffect)]) {
            gCurSpellEffectFrame = frame;
        }
        gpCombatManager->DrawFrame(0, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        if (effectX != ARMY_NO_EFFECT && frame < giNumPowFrames[IDX(gCurLoadedSpellEffect)]) {
            gCurLoadedSpellIcon->CombatClipDrawToBuffer(
                effectX,
                m_spellEffectYOffset + effectY,
                gCurSpellEffectFrame,
                &m_spellLimits,
                0,
                0,
                0,
                0
            );
        }
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
    }
    if (!gbNoShowCombat) {
        WaitSample(ARMY_SAMPLE_ATTACK);
    }
    for (side_4 = 0; side_4 < ARMY_COMBAT_SIDE_COUNT; side_4++) {
        for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[side_4]; index_10++) {
            current = &gpCombatManager->m_armies[side_4][index_10];
            if (current->m_damagePending && current->m_spellEffect != ARMY_NO_EFFECT
                && current->m_spellEffect != ARMY_DELAYED_MEDUSA_EFFECT) {
                gpCombatManager
                    ->CastSpell(SpellType(current->m_spellEffect), current->m_hex, 1, ARMY_NO_EFFECT);
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
                if (current->m_animationSequence == ARMY_ANIMATION_WINCE || IDX(current->m_animationSequence) == 16
                    || current->m_animationSequence == 20 || IDX(current->m_animationSequence) == 24
                    || current->m_animationSequence == 18 || IDX(current->m_animationSequence) == 22
                    || current->m_animationSequence == 26 || IDX(current->m_animationSequence) == 28
                    || current->m_animationSequence == 30 || IDX(current->m_animationSequence) == 32) {
                    current->m_animationSequence++;
                    current->m_animationFrame = 0;
                    keepAnimating_1 = 1;
                } else if (current->m_animationSequence == 13 || IDX(current->m_animationSequence) == 15
                           || current->m_animationSequence == 17
                           || current->m_animationSequence == 21
                           || current->m_animationSequence == 25
                           || current->m_animationSequence == 19
                           || current->m_animationSequence == 23
                           || current->m_animationSequence == 27
                           || current->m_animationSequence == 29
                           || current->m_animationSequence == 31
                           || current->m_animationSequence == 33) {
                    if (current->m_animationFrame + 1
                        < current->m_frameInfo
                              .animationFrameCount[IDX(current->m_animationSequence)]) {
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
            glTimers[0] = static_cast<i32>(
                KBTickCount() + frameDelay_6 * gfCombatSpeedMod[gConfig.combatSpeed]
            );
            gpCombatManager->DrawFrame(1, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        }
    }
    if (resetLimits) {
        gpCombatManager->ResetLimitCreature();
    }
    memset(gpCombatManager->m_removedArmies, 0, sizeof(gpCombatManager->m_removedArmies));
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
            if (current->m_damagePending && current->m_spellEffect == ARMY_DELAYED_MEDUSA_EFFECT) {
                gpCombatManager
                    ->CastSpell(SpellType(current->m_spellEffect), current->m_hex, 1, ARMY_NO_EFFECT);
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
u32l army::Strength(void) {
    return gMonsterDatabase[IDX(m_monsterType)].fightValue * m_quantity;
}

VA(0x00451700, 0x66)
i32 army::LeaveNoBody(void) {
    return m_monsterType == CREATURE_EARTH_ELEMENTAL || m_monsterType == CREATURE_AIR_ELEMENTAL
           || m_monsterType == CREATURE_FIRE_ELEMENTAL || m_monsterType == CREATURE_WATER_ELEMENTAL
           || HAS(m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE);
}

// @semantic
// The complete behavior, 0x18 frame/slots, CFG, and external targets align.
// Residuals are the commutative side/mirror-index address order at the two
// mirror army lookups, the retail-resolved recursive call at +0x3e5, and three
// trailing retail NOPs. A typed flat-array spelling changed the multiplication
// shape and regressed the match, so the real two-dimensional access is retained.
VA(0x00451766, 0x3f5)
void army::ProcessDeath(i32 immediate) {
    i32 rearHex;
    army* mirrorImage_4;
    army* mirrorSource;
    hexcell* frontCell_1;
    hexcell* rearCell;

    if (HAS(m_monster.flags.all, MONSTER_FLAGS_DEAD)) {
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
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
        rearHex = (static_cast<u32>(m_facing - 1) < 1 ? 1 : -1) + m_hex;
        rearCell = &gpCombatManager->m_hexCells[rearHex];
    } else {
        rearCell = 0;
    }
    if (LeaveNoBody()) {
        if (immediate
            || (m_monsterType != CREATURE_AIR_ELEMENTAL && m_monsterType != CREATURE_FIRE_ELEMENTAL
                && m_monsterType != CREATURE_WATER_ELEMENTAL
                && m_monsterType != CREATURE_EARTH_ELEMENTAL)) {
            gpCombatManager->m_removedArmies[m_side][m_index] = 1;
            gpCombatManager->m_removedArmyPresent = 1;
        } else {
            frontCell_1->m_occupantSide = -1;
            frontCell_1->m_occupantIndex = -1;
        }
    }
    if (frontCell_1->m_deadOccupantCount < ARMY_CORPSE_LIMIT && !LeaveNoBody()
        && (!rearCell || rearCell->m_deadOccupantCount < ARMY_CORPSE_LIMIT)) {
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

VA(0x00451b5b, 0x39d)
void army::SpellEffect(i32 effect, i32 effectFrameDelay, i32 animateCreature) {
    u32l effectFileId_3;
    IconEntry* entry_8;
    i32 frame_4;
    i32 minimumYOffset_16;
    i32 powBaseY_14;
    i32 frameDelay_2;
    i32 i_16;
    i32 unusedSpellEffectWord_12;

    effectFileId_3 = MAKEFILEID(gCombatFxNames[effect]);
    if (m_animationSequence == ARMY_ANIMATION_WINCE
        || m_animationSequence == ARMY_ANIMATION_WINCE_RETURN) {
        animateCreature = 0;
    }
    if (!gbNoShowCombat) {
        if (gCurLoadedSpellEffect != IDX(effect)) {
            gpResourceManager->Dispose(gCurLoadedSpellIcon);
            gCurLoadedSpellIcon = gpResourceManager->GetIcon(effectFileId_3);
            gCurLoadedSpellEffect = SpellType(effect);
        }
    }
    frame_4 = 0;
    m_drawSpellEffect = 1;
    m_spellEffectYOffset = 0;
    if (!gbNoShowCombat) {
        minimumYOffset_16 = ARMY_EFFECT_MINIMUM_Y;
        for (i_16 = 0; i_16 < gCurLoadedSpellIcon->m_frameCount; i_16++) {
            entry_8 = GetIconEntry(gCurLoadedSpellIcon, i_16);
            if (entry_8->y < minimumYOffset_16) {
                minimumYOffset_16 = entry_8->y;
            }
        }
        powBaseY_14 = GetPowBaseY();
        powBaseY_14 += minimumYOffset_16;
        if (powBaseY_14 < 0) {
            m_spellEffectYOffset = -powBaseY_14;
        }
        if (animateCreature) {
            frameDelay_2 =
                ARMY_SPELL_EFFECT_ANIMATION_DURATION
                / static_cast<i8>(m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE)]);
            m_animationSequence = ARMY_ANIMATION_WINCE;
            for (; frame_4
                   < static_cast<i8>(m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE)]);
                 frame_4++) {
                m_animationFrame = frame_4;
                if (frame_4 < giNumPowFrames[effect]) {
                    gCurSpellEffectFrame = frame_4;
                } else {
                    gCurSpellEffectFrame = giNumPowFrames[effect];
                }
                glTimers[1] = static_cast<i32>(
                    KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * frameDelay_2
                );
                gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
                DelayTil(&glTimers[1]);
            }
        }
        for (; frame_4 < giNumPowFrames[effect]; frame_4++) {
            glTimers[1] = static_cast<i32>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * effectFrameDelay
            );
            gCurSpellEffectFrame = frame_4;
            gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
            DelayTil(&glTimers[1]);
        }
    }
    m_drawSpellEffect = 0;
    if (!gbNoShowCombat) {
        if (animateCreature) {
            frameDelay_2 = ARMY_SPELL_EFFECT_ANIMATION_DURATION
                           / static_cast<i8>(
                               m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE_RETURN)]
                           );
            m_animationSequence = ARMY_ANIMATION_WINCE_RETURN;
            for (frame_4 = 0; frame_4 < static_cast<i8>(
                                  m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE_RETURN)]
                              );
                 frame_4++) {
                m_animationFrame = frame_4;
                glTimers[1] = static_cast<i32>(
                    KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * frameDelay_2
                );
                gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
                DelayTil(&glTimers[1]);
            }
            m_animationSequence = ARMY_ANIMATION_STAND;
            m_animationFrame = 0;
            gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        } else {
            gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        }
    }
}

// @early-stop
// All non-table bytes match. The table at +0xf3..+0x102 contains four
// function-local label relocations; its dispatch and all five calls also align.
VA(0x00451ef8, 0x10f)
void army::CancelSpellType(ArmySpellCancelType cancelType) {
    switch (cancelType) {
        case ARMY_CANCEL_SPELLS_AFTER_MOVE:
            break;
        case ARMY_CANCEL_SPELLS_AFTER_ATTACK:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BERSERK);
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HYPNOTIZE);
            break;
        case ARMY_CANCEL_SPELLS_AFTER_DAMAGE:
            if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)]) {
                CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BLIND);
                m_damagePenalty = ARMY_DAMAGE_PENALTY_HALF;
                m_monster.attributes |= MONSTER_ATTRIBUTE_WOKE_FROM_DAMAGE;
            }
            if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)]
                || m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]
                || m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]) {
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
void army::CancelIndividualSpell(ArmySpellInfluence influence) {
    if (!m_spellInfluence[IDX(influence)]) {
        return;
    }
    m_spellCount--;
    m_spellInfluence[IDX(influence)] = 0;
    switch (influence) {
        case ARMY_SPELL_INFLUENCE_HASTE:
        case ARMY_SPELL_INFLUENCE_SLOW:
            m_monster.speed = static_cast<i8>(m_speed);
            m_frameInfo.walkDuration = m_walkDuration;
            m_monster.attributes |=
                gMonsterDatabase[IDX(m_monsterType)].attributes & MONSTER_ATTRIBUTE_FLYING;
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
i32 army::SetSpellInfluence(ArmySpellInfluence influence, i32 rounds) {
    i32 i;

    if (m_spellInfluence[IDX(influence)]) {
        if (m_spellInfluence[IDX(influence)] < rounds) {
            m_spellInfluence[IDX(influence)] = static_cast<u8>(rounds);
        }
        return 0;
    }
    switch (influence) {
        case ARMY_SPELL_INFLUENCE_HASTE:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_SLOW);
            m_monster.speed += ARMY_HASTE_SPEED_BONUS;
            m_frameInfo.walkDuration =
                static_cast<i32>(m_frameInfo.walkDuration * ARMY_HASTE_WALK_DURATION_SCALE);
            break;
        case ARMY_SPELL_INFLUENCE_SLOW:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HASTE);
            m_monster.speed = static_cast<i8>((m_monster.speed + 1) / 2);
            if HAS(m_monster.attributes, MONSTER_ATTRIBUTE_FLYING) {
                m_monster.attributes -= MONSTER_ATTRIBUTE_FLYING;
            }
            m_frameInfo.walkDuration =
                static_cast<i32>(m_frameInfo.walkDuration * ARMY_SLOW_WALK_DURATION_SCALE);
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
                CancelIndividualSpell(ArmySpellInfluence(i));
            }
            break;
        case ARMY_SPELL_INFLUENCE_STONESKIN:
            if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STEELSKIN)]) {
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
    m_spellInfluence[IDX(influence)] = static_cast<u8>(rounds);
    return 1;
}

// @semantic
// Complete 0x94 body, 0x8 frame/slots, CFG, and the sole ordered relocation
// agree. At +0x67/+0x6a retail loads this from -0x8 into EAX and i from -0x4
// into ECX for the decrement SIB; base assigns those registers in reverse.
// i[m_spellInfluence] was byte-neutral and explicit pointer subtraction
// materially regressed the lowering. Revisit after an ARMY TU-state change.
VA(0x00452401, 0x94)
void army::DecrementSpellRounds(void) {
    i32 i;

    for (i = 0; i < ARMY_SPELL_INFLUENCE_COUNT; i++) {
        if (m_spellInfluence[i]) {
            if (m_spellInfluence[i] == 1) {
                CancelIndividualSpell(ArmySpellInfluence(i));
            } else {
                m_spellInfluence[i]--;
            }
        }
    }
    if (m_roundCounter > 0) {
        m_roundCounter--;
    }
}

// @semantic
// Logic, the 0x50 frame, every retail stack slot, and all 55 relocation sites
// align. Raw instruction review limits the non-jump residue to the candidate
// army side/index SIB evaluation and three equivalent comparison operand orders.
// The remaining extent delta is inline-accessor continuation and condition-
// polarity jumps; positive, negative, nested, and compound forms were audited.
VA(0x00452495, 0x644)
void army::GoBerserk(void) {
    i32 masks_28[5];
    i32 savedQuantity_10;
    i32 direction_4;
    i32 targetHex_9;
    i32 nearestSide_8;
    i32 nearestIndex_19;
    i32 nearestDistance_2;
    i32 side_8;
    i32 index_11;
    i32 distance_6;

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
                army* candidate_16 = &gpCombatManager->m_armies[side_8][index_11];
                if (!HAS(candidate_16->m_monster.flags.all, MONSTER_FLAGS_DEAD)) {
                    if (candidate_16->m_quantity > 0) {
                        distance_6 = gpSearchArray->QuickDistance(
                            gpCombatManager->m_hexCells[m_hex].m_x,
                            gpCombatManager->m_hexCells[m_hex].m_y,
                            gpCombatManager->m_hexCells[candidate_16->m_hex].m_x,
                            gpCombatManager->m_hexCells[candidate_16->m_hex].m_y
                        );
                        if (distance_6 < nearestDistance_2) {
                            nearestIndex_19 = index_11;
                            nearestSide_8 = side_8;
                            nearestDistance_2 = distance_6;
                        }
                    }
                }
            }
        }
        if (nearestIndex_19 != -1 && HAS(m_monster.flags.all, MONSTER_FLAGS_SHOOTER)
            && m_monster.shots > 0) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex = gpCombatManager->m_armies[nearestSide_8][nearestIndex_19].m_hex;
        } else {
            i32 sideZeroTarget_12 = -1;
            i32 sideOneTarget_18 = -1;
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
                    i32 sideZeroDistance = gpSearchArray->QuickDistance(
                        gpCombatManager->m_hexCells[m_hex].m_x,
                        gpCombatManager->m_hexCells[m_hex].m_y,
                        gpCombatManager->m_hexCells[sideZeroTarget_12].m_x,
                        gpCombatManager->m_hexCells[sideZeroTarget_12].m_y
                    );
                    i32 sideOneDistance = gpSearchArray->QuickDistance(
                        gpCombatManager->m_hexCells[m_hex].m_x,
                        gpCombatManager->m_hexCells[m_hex].m_y,
                        gpCombatManager->m_hexCells[sideOneTarget_18].m_x,
                        gpCombatManager->m_hexCells[sideOneTarget_18].m_y
                    );
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
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_FLYING)
        || (!gpCombatManager->WalkTowardArmy(this, m_side, masks_28[m_side])
            && !gpCombatManager->WalkTowardArmy(this, 1 - m_side, masks_28[1 - m_side]))) {
        giNextAction = COMBAT_AI_ACTION_WAIT;
    }
berserkFinish:
    if (giNextAction == COMBAT_AI_ACTION_MOVE
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == m_side) {
        gpCombatManager->m_heroDeathPending[m_side] = 1;
    }
}

// @early-stop
// All non-jump opcodes/operands and all 24 relocation sites align. The 32-byte
// object extent delta is limited to seven five-byte inline-accessor continuation
// jumps plus the equivalent first-condition branch encoding.
VA(0x00452ad9, 0x3f1)
void army::MoveAttack(i32 destination, i32 moveOnly) {
    i32 baseAttackMask;
    i32 targetAttackMask;
    i32 sourceHex;
    i32 adjacentHex;
    i32 direction;
    hexcell* adjacentCell;

    while (1) {
        gpCombatManager->m_limitCreature = 0;
        m_targetSide = -1;
        m_targetIndex = -1;
        if (!ValidHex(destination)) {
            return;
        }
        if (gpCombatManager->m_hexCells[destination].m_occupantSide == -1
            || (gpCombatManager->m_hexCells[destination].m_occupantSide
                    == gpCombatManager->m_currentArmySide
                && gpCombatManager->m_hexCells[destination].m_occupantIndex
                       == gpCombatManager->m_currentArmyIndex)) {
            goto move;
        }
        if (moveOnly) {
            return;
        }
        m_targetSide = gpCombatManager->m_hexCells[destination].m_occupantSide;
        m_targetIndex = gpCombatManager->m_hexCells[destination].m_occupantIndex;
        m_moveTargetHex = destination;
        baseAttackMask = GetAttackMask(m_hex, 0, -1);
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_FLYING)
            && baseAttackMask == ARMY_ALL_ATTACK_DIRECTIONS) {
            if (m_hex != m_moveTargetHex && !ValidFlight(m_moveTargetHex, 0)) {
                return;
            }
            FlyTo(m_moveTargetHex);
            continue;
        }
        break;
    }
    if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]) {
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
            if (direction < ARMY_ADJACENT_DIRECTION_COUNT
                || HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
                sourceHex = m_hex;
                if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE) && m_facing == 1 && direction >= 0
                    && direction <= 2) {
                    sourceHex++;
                }
                if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE) && m_facing == 0 && direction >= 3
                    && direction <= 5) {
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
                    if (adjacentCell->m_occupantSide == m_targetSide
                        && adjacentCell->m_occupantIndex == m_targetIndex) {
                        m_attackDirection = direction;
                    }
                }
            }
        }
        DoAttack(0);
    }
    goto finish;
move:
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_FLYING)) {
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

// @semantic: The complete behavior, 0x14 frame, loop/found/resurrect/hypnotize
// slots at -0x04/-0x08/-0x0c/-0x10, CFG, and all 59 ordered relocations align.
// The first remaining structural delta is the equivalent branch polarity at
// instruction 134; retail also has three trailing NOPs. Computed local-name
// buckets fixed all prior slot differences; revisit after ARMY TU-state changes.
VA(0x00452eca, 0x931)
float army::SpellCastWorkChance(SpellType spell) {
    i32 foundSpell_8;
    i32 i_15;
    i32 resurrectPower_5;
    i32 hypnotizeHitPoints_37;

    if (HAS(m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)
        && (spell == SPELL_MIRROR_IMAGE || spell == SPELL_ANTI_MAGIC)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (spell == SPELL_DISPEL || spell == SPELL_MASS_DISPEL) {
        foundSpell_8 = 0;
        for (i_15 = 0; i_15 < ARMY_SPELL_INFLUENCE_COUNT; i_15++) {
            if (m_spellInfluence[i_15]) {
                foundSpell_8 = 1;
                break;
            }
        }
        if (!foundSpell_8) {
            return ARMY_SPELL_CHANCE_NONE;
        }
    }
    if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_ANTI_MAGIC)]
        || (HAS(m_monster.flags.all, MONSTER_FLAGS_DEAD) && spell != SPELL_RESURRECT
            && spell != SPELL_TRUE_RESURRECT && spell != SPELL_ANIMATE_DEAD)
        || m_deathPending || m_monsterType == CREATURE_GREEN_DRAGON
        || m_monsterType == CREATURE_RED_DRAGON || m_monsterType == CREATURE_BLACK_DRAGON) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (spell == SPELL_MIRROR_IMAGE && m_mirrorImageIndex != -1) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if ((spell == SPELL_RESURRECT || spell == SPELL_TRUE_RESURRECT)
        && (HAS(m_monster.flags.all, MONSTER_FLAGS_UNDEAD) || m_initialQuantity == m_quantity)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (spell == SPELL_ANIMATE_DEAD
        && (!HAS(m_monster.flags.all, MONSTER_FLAGS_UNDEAD) || m_initialQuantity == m_quantity)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if ((spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT)
        && !HAS(m_monster.flags.all, MONSTER_FLAGS_UNDEAD)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if ((spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE)
        && HAS(m_monster.flags.all, MONSTER_FLAGS_UNDEAD)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == CREATURE_PHOENIX && spell != SPELL_FIREBALL && spell != SPELL_FIREBLAST
        && spell != SPELL_LIGHTNING_BOLT && spell != SPELL_CHAIN_LIGHTNING
        && spell != SPELL_COLD_RAY && spell != SPELL_COLD_RING && spell != SPELL_ELEMENTAL_STORM) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == CREATURE_CRUSADER && (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if ((HAS(m_monster.flags.all, MONSTER_FLAGS_UNDEAD) || m_monsterType == CREATURE_EARTH_ELEMENTAL
         || m_monsterType == CREATURE_AIR_ELEMENTAL || m_monsterType == CREATURE_FIRE_ELEMENTAL
         || m_monsterType == CREATURE_WATER_ELEMENTAL || m_monsterType == CREATURE_GIANT
         || m_monsterType == CREATURE_TITAN)
        && (spell == SPELL_BERSERKER || spell == SPELL_HYPNOTIZE || spell == SPELL_PARALYZE
            || spell == SPELL_BLIND)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_UNDEAD)
        && (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE || spell == SPELL_BLESS
            || spell == SPELL_MASS_BLESS)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == CREATURE_EARTH_ELEMENTAL
        && (spell == SPELL_LIGHTNING_BOLT || spell == SPELL_CHAIN_LIGHTNING
            || spell == SPELL_METEOR_SHOWER)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == CREATURE_AIR_ELEMENTAL && spell == SPELL_METEOR_SHOWER) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == CREATURE_FIRE_ELEMENTAL
        && (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == CREATURE_WATER_ELEMENTAL
        && (spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (gpCombatManager->m_heroes[m_side]) {
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(ARTIFACT_HOLY_PENDANT)
            && (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE)) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(ARTIFACT_PENDANT_FREE_WILL)
            && spell == SPELL_HYPNOTIZE) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(ARTIFACT_PENDANT_LIFE)
            && (spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE)) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(ARTIFACT_SERENITY_PENDANT)
            && spell == SPELL_BERSERKER) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(ARTIFACT_SEEING_EYE_PENDANT)
            && spell == SPELL_BLIND) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(ARTIFACT_KINETIC_PENDANT)
            && spell == SPELL_PARALYZE) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(ARTIFACT_PENDANT_DEATH)
            && (spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT)) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[m_side]->HasArtifact(ARTIFACT_WAND_NEGATION)
            && (spell == SPELL_DISPEL || spell == SPELL_MASS_DISPEL
                || spell == IDX(CREATURE_SPELL_DISPEL))) {
            return ARMY_SPELL_CHANCE_NONE;
        }
    }
    if (m_quantity == 0
        && (spell == SPELL_RESURRECT || spell == SPELL_TRUE_RESURRECT
            || spell == SPELL_ANIMATE_DEAD)) {
        resurrectPower_5 = gpCombatManager->m_spellPower[gpCombatManager->m_currentSide]
                           * ARMY_RESURRECT_POWER_PER_SPELL_POWER;
        if (gpCombatManager->m_heroes[gpCombatManager->m_currentSide]
            && gpCombatManager->m_heroes[gpCombatManager->m_currentSide]->HasArtifact(
                ARTIFACT_ANKH
            )) {
            resurrectPower_5 *= ARMY_ARTIFACT_POWER_MULTIPLIER;
        }
        if (resurrectPower_5 < m_monster.hitPoints) {
            return ARMY_SPELL_CHANCE_NONE;
        }
    }
    if (spell == IDX(CREATURE_SPELL_DISPEL) && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HASTE)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLESS)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLOODLUST)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_SHIELD)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_ANTI_MAGIC)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STONESKIN)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STEELSKIN)]) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (spell == SPELL_HYPNOTIZE) {
        hypnotizeHitPoints_37 = gpCombatManager->m_heroes[gpCombatManager->m_currentSide]->Stats(
                                    HERO_PRIMARY_SPELL_POWER
                                )
                                * ARMY_HYPNOTIZE_HIT_POINTS_PER_POWER;
        if (gpCombatManager->m_heroes[gpCombatManager->m_currentSide]->HasArtifact(
                ARTIFACT_GOLD_WATCH
            )) {
            hypnotizeHitPoints_37 *= ARMY_ARTIFACT_POWER_MULTIPLIER;
        }
        if (m_monster.hitPoints * m_quantity > hypnotizeHitPoints_37) {
            return ARMY_SPELL_CHANCE_NONE;
        }
    }
    if ((m_monsterType == CREATURE_DWARF || m_monsterType == CREATURE_BATTLE_DWARF)
        && spell != SPELL_TELEPORT && spell != SPELL_CURE && spell != SPELL_MASS_CURE
        && spell != SPELL_RESURRECT && spell != SPELL_TRUE_RESURRECT && spell != SPELL_HASTE
        && spell != SPELL_MASS_HASTE && spell != SPELL_BLESS && spell != SPELL_MASS_BLESS
        && spell != SPELL_STONE_SKIN && spell != SPELL_STEEL_SKIN && spell != SPELL_ANTI_MAGIC
        && spell != SPELL_DRAGON_SLAYER && spell != SPELL_BLOOD_LUST && spell != SPELL_MIRROR_IMAGE
        && spell != SPELL_SHIELD && spell != SPELL_MASS_SHIELD) {
        return ARMY_SPELL_CHANCE_DWARF;
    }
    return ARMY_SPELL_CHANCE_ALWAYS;
}

VA(0x004537fb, 0x56)
i32 army::SpellCastWorks(SpellType spell) {
    i32 chance;

    chance = static_cast<i32>(SpellCastWorkChance(spell) * ARMY_SPELL_CHANCE_PERCENT);
    return SRandom(1, ARMY_RANDOM_SPELL_ROLL_MAX) <= chance;
}

VA(0x00453851, 0x39e)
void BuildTempWalkSeq(struct SMonFrameInfo* frameInfo, i32 finishStanding, i32 skipDrawing) {
    frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] = 0;
    if (!skipDrawing && finishStanding) {
        if (frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_STAND)] > 0) {
            memcpy(
                &frameInfo
                     ->animationFrames[IDX(ARMY_ANIMATION_WALK)]
                                      [frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationFrames[IDX(ARMY_WALK_SEGMENT_STAND)],
                frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_STAND)]
            );
            memcpy(
                &frameInfo->walkXOffsets[frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationXOffsets[IDX(ARMY_WALK_SEGMENT_STAND)],
                frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_STAND)]
            );
            frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] +=
                frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_STAND)];
        }
    } else {
        if (!skipDrawing) {
            if (frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_BEGIN)] > 0) {
                memcpy(
                    &frameInfo->animationFrames[IDX(
                        ARMY_ANIMATION_WALK
                    )][frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                    frameInfo->animationFrames[IDX(ARMY_WALK_SEGMENT_BEGIN)],
                    frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_BEGIN)]
                );
                memcpy(
                    &frameInfo
                         ->walkXOffsets[frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                    frameInfo->animationXOffsets[IDX(ARMY_WALK_SEGMENT_BEGIN)],
                    frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_BEGIN)]
                );
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] +=
                    frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_BEGIN)];
            }
        } else if (frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_BEGIN_STANDING)] > 0) {
            memcpy(
                &frameInfo
                     ->animationFrames[IDX(ARMY_ANIMATION_WALK)]
                                      [frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationFrames[IDX(ARMY_WALK_SEGMENT_BEGIN_STANDING)],
                frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_BEGIN_STANDING)]
            );
            memcpy(
                &frameInfo->walkXOffsets[frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationXOffsets[IDX(ARMY_WALK_SEGMENT_BEGIN_STANDING)],
                frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_BEGIN_STANDING)]
            );
            frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] +=
                frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_BEGIN_STANDING)];
        }
        if (frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_MIDDLE)] > 0) {
            memcpy(
                &frameInfo
                     ->animationFrames[IDX(ARMY_ANIMATION_WALK)]
                                      [frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationFrames[IDX(ARMY_WALK_SEGMENT_MIDDLE)],
                frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_MIDDLE)]
            );
            memcpy(
                &frameInfo->walkXOffsets[frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationXOffsets[IDX(ARMY_WALK_SEGMENT_MIDDLE)],
                frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_MIDDLE)]
            );
            frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] +=
                frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_MIDDLE)];
        }
        if (finishStanding) {
            if (frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_END_STANDING)] > 0) {
                memcpy(
                    &frameInfo->animationFrames[IDX(
                        ARMY_ANIMATION_WALK
                    )][frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                    frameInfo->animationFrames[IDX(ARMY_WALK_SEGMENT_END_STANDING)],
                    frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_END_STANDING)]
                );
                memcpy(
                    &frameInfo
                         ->walkXOffsets[frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                    frameInfo->animationXOffsets[IDX(ARMY_WALK_SEGMENT_END_STANDING)],
                    frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_END_STANDING)]
                );
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] +=
                    frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_END_STANDING)];
            }
        } else if (frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_END)] > 0) {
            memcpy(
                &frameInfo
                     ->animationFrames[IDX(ARMY_ANIMATION_WALK)]
                                      [frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationFrames[IDX(ARMY_WALK_SEGMENT_END)],
                frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_END)]
            );
            memcpy(
                &frameInfo->walkXOffsets[frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationXOffsets[IDX(ARMY_WALK_SEGMENT_END)],
                frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_END)]
            );
            frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] +=
                frameInfo->animationFrameCount[IDX(ARMY_WALK_SEGMENT_END)];
        }
    }
}

VA(0x00453bef, 0x66)
void army::DispelGood(void) {
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
void army::Cure(i32 amount) {
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
i32 army::MidX(void) {
    i32 wideOffset;

    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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
i32 army::MidY(void) {
    return gpCombatManager->m_hexCells[m_hex].m_y
           - (GetIconEntry(
                  m_creatureIcon,
                  m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
              )
                  ->h
              >> 1);
}

VA(0x00453db7, 0x57)
i32 army::TopY(void) {
    return gpCombatManager->m_hexCells[m_hex].m_y
           - GetIconEntry(m_creatureIcon, m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0])
                 ->h;
}

VA(0x00453e0e, 0xcc)
i32 army::RightX(void) {
    if (m_facing == 1) {
        return gpCombatManager->m_hexCells[m_hex].m_x
               + GetIconEntry(
                     m_creatureIcon,
                     m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
               )
                     ->w
               + GetIconEntry(
                     m_creatureIcon,
                     m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
               )
                     ->x;
    } else {
        return gpCombatManager->m_hexCells[m_hex].m_x
               - GetIconEntry(
                     m_creatureIcon,
                     m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
               )
                     ->x;
    }
}

VA(0x00453eda, 0xcc)
i32 army::LeftX(void) {
    if (m_facing == 1) {
        return gpCombatManager->m_hexCells[m_hex].m_x
               + GetIconEntry(
                     m_creatureIcon,
                     m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
               )
                     ->x;
    } else {
        return gpCombatManager->m_hexCells[m_hex].m_x
               - (GetIconEntry(
                      m_creatureIcon,
                      m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
                  )
                      ->w
                  + GetIconEntry(
                        m_creatureIcon,
                        m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
                  )
                        ->x);
    }
}

// @semantic: The complete 108-instruction CFG, 0x1c frame, all seven stack
// slots, and all 3/3 ordered relocations align. At +0xae/+0xb1 and +0xc4/+0xc7
// (repeated at +0x134/+0x137 and +0x14a/+0x14d), retail loads adjacentHex into
// eax and compares the other hex slot; ours loads the other slot and compares
// adjacentHex. Reversed equality operands, | 0 on either side, and negated
// inequality forms all canonicalized unchanged. Revisit in the byte-last-mile
// phase or after ARMY TU/header state changes.
VA(0x00453fa6, 0x171)
i32 army::OtherArmyAdjacent(i32 side, i32 index) {
    army* otherArmy1;
    i32 otherHex1;
    i32 otherRearHex;
    i32 adjacentHex;
    i32 rearHex;
    i32 directionResult;

    otherArmy1 = &gpCombatManager->m_armies[side][index];
    otherHex1 = otherArmy1->m_hex;
    if (HAS(otherArmy1->m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
        otherRearHex = otherHex1 + (otherArmy1->m_side == 0 ? 1 : -1);
    } else {
        otherRearHex = -1;
    }
    for (directionResult = 0; directionResult < ARMY_ADJACENT_DIRECTION_COUNT; directionResult++) {
        adjacentHex = GetAdjacentCellIndex(m_hex, directionResult);
        if (adjacentHex == otherHex1 || (adjacentHex != -1 && adjacentHex == otherRearHex)) {
            return 1;
        }
    }
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
        rearHex = m_hex + (m_side == 0 ? 1 : -1);
        for (directionResult = 0; directionResult < ARMY_ADJACENT_DIRECTION_COUNT;
             directionResult++) {
            adjacentHex = GetAdjacentCellIndex(rearHex, directionResult);
            if (adjacentHex == otherHex1 || (adjacentHex != -1 && adjacentHex == otherRearHex)) {
                return 1;
            }
        }
    }
    return 0;
}

VA(0x00454117, 0x1e1)
void ModifyFrameInfo(struct SMonFrameInfo* frameInfo, CreatureType monsterType) {
    i32 speedDifference;

    speedDifference = 0;
    if (monsterType == CREATURE_RANGER || monsterType == CREATURE_VETERAN_PIKEMAN
        || monsterType == CREATURE_MASTER_SWORDSMAN || monsterType == CREATURE_CHAMPION
        || monsterType == CREATURE_CRUSADER || monsterType == CREATURE_ORC_CHIEF
        || monsterType == CREATURE_OGRE_LORD || monsterType == CREATURE_WAR_TROLL
        || monsterType == CREATURE_BATTLE_DWARF || monsterType == CREATURE_GRAND_ELF
        || monsterType == CREATURE_GREATER_DRUID || monsterType == CREATURE_MINOTAUR_KING
        || monsterType == CREATURE_STEEL_GOLEM || monsterType == CREATURE_ARCHMAGE
        || monsterType == CREATURE_MUTANT_ZOMBIE || monsterType == CREATURE_ROYAL_MUMMY
        || monsterType == CREATURE_VAMPIRE_LORD || monsterType == CREATURE_POWER_LICH) {
        speedDifference =
            gMonsterDatabase[IDX(monsterType)].speed - gMonsterDatabase[IDX(monsterType) - 1].speed;
    }
    if (monsterType == CREATURE_EARTH_ELEMENTAL || monsterType == CREATURE_AIR_ELEMENTAL
        || monsterType == CREATURE_WATER_ELEMENTAL) {
        speedDifference = gMonsterDatabase[IDX(monsterType)].speed
                          - gMonsterDatabase[IDX(CREATURE_FIRE_ELEMENTAL)].speed;
    }
    if (speedDifference) {
        if (monsterType == CREATURE_RANGER) {
            frameInfo->attackDuration =
                static_cast<i32>(frameInfo->attackDuration * ARMY_RANGER_ATTACK_DURATION_SCALE);
        } else {
            frameInfo->attackDuration = static_cast<i32>(
                (ARMY_DURATION_BASE_SCALE - speedDifference * ARMY_ATTACK_DURATION_SPEED_SCALE)
                * frameInfo->attackDuration
            );
        }
        frameInfo->walkDuration = static_cast<i32>(
            (ARMY_DURATION_BASE_SCALE - speedDifference * ARMY_WALK_DURATION_SPEED_SCALE)
            * frameInfo->walkDuration
        );
    }
}

VA(0x004542f8, 0xbe)
i32 army::GetPowBaseY(void) {
    i32 y;

    y = MidY();
    if (gCurLoadedSpellEffect == SPELL_BERSERKER || gCurLoadedSpellEffect == SPELL_MAGIC_ARROW
        || gCurLoadedSpellEffect == SPELL_HOLY_SHOUT || gCurLoadedSpellEffect == SPELL_METEOR_SHOWER
        || gCurLoadedSpellEffect == SPELL_ARMAGEDDON || gCurLoadedSpellEffect == SPELL_ANTI_MAGIC) {
        y = TopY();
    }
    if (gCurLoadedSpellEffect == SPELL_PARALYZE || gCurLoadedSpellEffect == SPELL_HYPNOTIZE) {
        y = gpCombatManager->m_hexCells[m_hex].m_y + ARMY_CONTROL_EFFECT_Y_OFFSET;
    }
    return y;
}

// ---- globals (definitions, RVA order) ----
// @data-layout-note Retail .data is 0xf53c4+0x2f4: 58 exact literal payloads
// plus bSecondAttack account for 59 allocations and all 61 HIGHLOW references.
// Retail places bSecondAttack at +0xe4, while candidate COFF places it at +0;
// this is allocation order, not missing storage. The sole uncovered retail byte
// is terminal zero alignment at +0x2f3. Retail .rdata 0xeb5e0..0xeb650 matches
// the complete candidate section (18 constants, 63 references), and .bss
// 0x127eb4..0x127eb8 is exactly gbGenieHalf (3 references, addend zero). Do not
// model the .data order or terminal alignment with aliases or invented padding.
DATA(0x004f54a8) i32 bSecondAttack = 0;
DATA(0x00527eb4) b32 gbGenieHalf;
