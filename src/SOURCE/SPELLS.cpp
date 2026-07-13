// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SPELLS.OBJ   from: (directly linked into exe)
// functions: 37   data: 7
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <string.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/SPELLS.h>
#include <SOURCE/X_GLOBAL.h>
VA(0x004204c0, 0x86)
int combatManager::HasValidSpellTarget(int) { return 0; }

VA(0x00420546, 0x44a)
int combatManager::ViewSpells(int) { return 0; }

VA(0x00420990, 0x15c)
int CombatSpecialHandler(struct tag_message &) { return 0; }

VA(0x00420aec, 0x2aa)
int HandleCastSpell(struct tag_message &) { return 0; }

VA(0x00420d96, 0x2e5)
int combatManager::FindResurrectArmyIndex(int, int, int) { return 0; }

VA(0x0042107b, 0x521)
int combatManager::ValidSpellTarget(int, int) { return 0; }

VA(0x0042159c, 0x222)
void combatManager::SpellMessage(int, int) {}

VA(0x004217be, 0x1eca)
void combatManager::CastSpell(int spell, int targetHex, int castByCreature, int teleportDestination)
{
    long damage_m;
    int spellPower_i;
    army *target_i;
    int targetIndex_k;
    int soundSpell_q;
    int armyIndex;
    char sampleName_i[16];
    SAMPLE2 spellSample;
    int targetY_b;
    int targetX_b;
    int side_i;
    int targetSide_i;
    int influence;
    int adjacentHex_q;
    int oldDefense;
    army *teleportArmy_i;
    icon *missileIcon_p;
    char *lightningArmyName_d;
    char *magicArrowArmyName_n;
    char *coldRayArmyName_f;
    float missileAngles[9];

    if (castByCreature == 0 &&
        m_eagleEyeSpell[1 - m_currentSide] == -1 &&
        m_heroes[1 - m_currentSide] != 0 &&
        !m_heroes[1 - m_currentSide]->HasSpell(spell) &&
        m_heroes[1 - m_currentSide]->m_secondarySkills[HERO_SKILL_EAGLE_EYE] != HERO_SKILL_LEVEL_NONE &&
        m_heroes[1 - m_currentSide]->m_secondarySkills[HERO_SKILL_EAGLE_EYE] + 1 >=
            gsSpellInfo[spell].level) {
        if (SRandom(0, 9) <=
            m_heroes[1 - m_currentSide]->m_secondarySkills[HERO_SKILL_EAGLE_EYE]) {
            m_eagleEyeSpell[1 - m_currentSide] = static_cast<short>(spell);
        }
    }

    spellSample = GetNullSample();
    if (m_limitCreature != 0) {
        ResetLimitCreature();
        if (ValidHex(m_limitCreatureHex) && m_hexCells[m_limitCreatureHex].m_occupantSide >= 0) {
            m_limitCreatureCount[m_hexCells[m_limitCreatureHex].m_occupantSide]
                                [m_hexCells[m_limitCreatureHex].m_occupantIndex] =
                m_limitCreatureCount[m_hexCells[m_limitCreatureHex].m_occupantSide]
                                    [m_hexCells[m_limitCreatureHex].m_occupantIndex] + 1;
        }
        m_limitCreature = 0;
        m_limitCreatureHex = COMBAT_HEX_EMPTY;
        gpCombatManager->DrawFrame(1, 1, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    }

    if (castByCreature == 0 && m_heroes[m_currentSide] != 0)
        m_heroes[m_currentSide]->UseSpell(spell);

    target_i = 0;
    if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST || spell == SPELL_COLD_RING ||
        spell == SPELL_METEOR_SHOWER || spell == SPELL_SUMMON_EARTH_ELEMENTAL ||
        spell == SPELL_SUMMON_AIR_ELEMENTAL || spell == SPELL_SUMMON_WATER_ELEMENTAL ||
        spell == SPELL_SUMMON_FIRE_ELEMENTAL || spell == SPELL_MASS_BLESS ||
        spell == SPELL_MASS_HASTE || spell == SPELL_EARTHQUAKE || spell == SPELL_MASS_CURSE ||
        spell == SPELL_MASS_CURE || spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT ||
        spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE || spell == SPELL_MASS_SHIELD ||
        spell == SPELL_ARMAGEDDON || spell == SPELL_ELEMENTAL_STORM || spell == SPELL_MASS_DISPEL) {
        target_i = 0;
    } else if (ValidHex(targetHex) && m_hexCells[targetHex].m_occupantSide >= 0) {
        target_i = &m_armies[m_hexCells[targetHex].m_occupantSide]
                            [m_hexCells[targetHex].m_occupantIndex];
        targetSide_i = m_hexCells[targetHex].m_occupantSide;
        targetIndex_k = m_hexCells[targetHex].m_occupantIndex;
    } else {
        target_i = 0;
    }

    if (castByCreature == 0)
        m_heroCastSpell[m_currentSide] = 1;

    if (castByCreature != 0) {
        spellPower_i = SPELL_DEFAULT_CREATURE_POWER;
    } else {
        spellPower_i = m_spellPower[m_currentSide];
        if (m_heroes[m_currentSide]->HasArtifact(SPELL_ARTIFACT_ENCHANTED_HOURGLASS) &&
            (gsSpellInfo[spell].m_e & SPELL_ATTRIBUTE_DURATION)) {
            spellPower_i += SPELL_HOURGLASS_POWER_BONUS;
        }
        if (m_heroes[m_currentSide]->HasArtifact(SPELL_ARTIFACT_WIZARD_HAT) &&
            (gsSpellInfo[spell].m_e & SPELL_ATTRIBUTE_DURATION)) {
            spellPower_i += SPELL_WIZARD_HAT_POWER_BONUS;
        }
    }

    if (castByCreature == 0) {
        targetX_b = -1;
        targetY_b = -1;
        if (target_i != 0) {
            targetX_b = target_i->MidX();
            targetY_b = target_i->MidY();
        } else {
            if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST ||
                spell == SPELL_COLD_RING || spell == SPELL_METEOR_SHOWER) {
                targetX_b = m_hexCells[targetHex].m_x;
                targetY_b = m_hexCells[targetHex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET;
            }
        }

        if (targetX_b != -1) {
            if (m_currentSide == 0) {
                castX = sCmbtHero[m_heroSpriteIndex[m_currentSide]].x[0] + COMBAT_HERO_CAST_LEFT_X_OFFSET;
                castY = sCmbtHero[m_heroSpriteIndex[m_currentSide]].y[0] + COMBAT_HERO_CAST_LEFT_Y_OFFSET;
            } else {
                castX = COMBAT_HERO_CAST_RIGHT_X - sCmbtHero[m_heroSpriteIndex[m_currentSide]].x[0];
                castY = sCmbtHero[m_heroSpriteIndex[m_currentSide]].y[0] + COMBAT_HERO_CAST_RIGHT_Y_OFFSET;
            }
            if ((m_currentSide == 0 ? 1 : -1) * (targetX_b - castX) < targetY_b - castY) {
                m_heroAnimationState[m_currentSide] = COMBAT_HERO_CAST_HIGH;
                if (m_currentSide == 0) {
                    castX = sCmbtHero[m_heroSpriteIndex[m_currentSide]].x[1] + COMBAT_HERO_CAST_LEFT_X_OFFSET;
                    castY = sCmbtHero[m_heroSpriteIndex[m_currentSide]].y[1] + COMBAT_HERO_CAST_LEFT_Y_OFFSET;
                } else {
                    castX = COMBAT_HERO_CAST_RIGHT_X - sCmbtHero[m_heroSpriteIndex[m_currentSide]].x[1];
                    castY = sCmbtHero[m_heroSpriteIndex[m_currentSide]].y[1] + COMBAT_HERO_CAST_RIGHT_Y_OFFSET;
                }
            } else {
                m_heroAnimationState[m_currentSide] = COMBAT_HERO_CAST_LOW;
            }
        } else {
            m_heroAnimationState[m_currentSide] = COMBAT_HERO_CAST_NO_TARGET;
        }
        m_heroAnimationFrame[m_currentSide] = 0;
        while (m_heroAnimationFrame[m_currentSide] <
               sCmbtHero[m_heroSpriteIndex[m_currentSide]]
                   .animationFrameCount[m_heroAnimationState[m_currentSide]]) {
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            m_heroAnimationFrame[m_currentSide]++;
        }
        m_heroAnimationFrame[m_currentSide]--;
    }

    soundSpell_q = spell;
    if (spell == SPELL_PETRIFY)
        soundSpell_q = SPELL_PARALYZE;
    if (spell == SPELL_CREATURE_DISPEL)
        soundSpell_q = SPELL_DISPEL;
    if (strlen(gsSpellInfo[soundSpell_q].soundName) != 0)
        sprintf(sampleName_i, "%s.82M", gsSpellInfo[soundSpell_q].soundName);

    if (castByCreature == 0 && target_i != 0 && !target_i->SpellCastWorks(spell)) {
        ShowSpellCastFailure(target_i, spell);
    } else {
        spellSample = LoadPlaySample(sampleName_i);
        switch (spell) {
        case SPELL_TELEPORT:
            teleportArmy_i = target_i;
            targetHex = teleportDestination;
            RippleCreature(teleportArmy_i->m_side, teleportArmy_i->m_index, 1);
            m_hexCells[teleportArmy_i->m_hex].m_occupantSide = COMBAT_HEX_EMPTY;
            m_hexCells[teleportArmy_i->m_hex].m_occupantIndex = COMBAT_HEX_EMPTY;
            if (m_hexCells[teleportArmy_i->m_hex].m_occupantFrame == ARMY_FACING_LEFT) {
                m_hexCells[teleportArmy_i->m_hex + 1].m_occupantSide = COMBAT_HEX_EMPTY;
                m_hexCells[teleportArmy_i->m_hex + 1].m_occupantIndex = COMBAT_HEX_EMPTY;
            } else if (m_hexCells[teleportArmy_i->m_hex].m_occupantFrame == ARMY_FACING_RIGHT) {
                m_hexCells[teleportArmy_i->m_hex - 1].m_occupantSide = COMBAT_HEX_EMPTY;
                m_hexCells[teleportArmy_i->m_hex - 1].m_occupantIndex = COMBAT_HEX_EMPTY;
            }
            if (gbNoShowCombat == 0)
                WaitEndSample(spellSample, -1);
            if (gbNoShowCombat == 0) {
                sprintf(gText, "telptin.82m");
                spellSample = LoadPlaySample(gText);
            }
            if ((teleportArmy_i->m_monster.flags.all & ARMY_FLAG_WIDE) != 0) {
                adjacentHex_q = teleportDestination;
                if (teleportArmy_i->m_facing == ARMY_FACING_RIGHT) {
                    adjacentHex_q = teleportArmy_i->GetAdjacentCellIndex(teleportDestination, 1);
                    if (adjacentHex_q == COMBAT_HEX_EMPTY ||
                        (m_hexCells[adjacentHex_q].m_occupantSide != COMBAT_HEX_EMPTY &&
                         (m_hexCells[adjacentHex_q].m_occupantSide != targetSide_i ||
                          m_hexCells[adjacentHex_q].m_occupantIndex != targetIndex_k)) ||
                        m_hexCells[adjacentHex_q].m_blocked != 0) {
                        targetHex = teleportDestination - 1;
                    }
                }
                if (teleportArmy_i->m_facing == ARMY_FACING_LEFT) {
                    adjacentHex_q = teleportArmy_i->GetAdjacentCellIndex(adjacentHex_q, 4);
                    if (adjacentHex_q == COMBAT_HEX_EMPTY ||
                        (m_hexCells[adjacentHex_q].m_occupantSide != COMBAT_HEX_EMPTY &&
                         (m_hexCells[adjacentHex_q].m_occupantSide != targetSide_i ||
                          m_hexCells[adjacentHex_q].m_occupantIndex != targetIndex_k)) ||
                        m_hexCells[adjacentHex_q].m_blocked != 0) {
                        targetHex++;
                    }
                }
                teleportArmy_i->m_hex = targetHex;
                if (teleportArmy_i->m_facing == ARMY_FACING_LEFT) {
                    m_hexCells[targetHex].m_occupantSide = static_cast<signed char>(targetSide_i);
                    m_hexCells[targetHex].m_occupantIndex = static_cast<signed char>(targetIndex_k);
                    m_hexCells[targetHex].m_occupantFrame = ARMY_FACING_RIGHT;
                    m_hexCells[targetHex - 1].m_occupantSide = static_cast<signed char>(targetSide_i);
                    m_hexCells[targetHex - 1].m_occupantIndex = static_cast<signed char>(targetIndex_k);
                    m_hexCells[targetHex - 1].m_occupantFrame = ARMY_FACING_LEFT;
                } else if (teleportArmy_i->m_facing == ARMY_FACING_RIGHT) {
                    m_hexCells[targetHex].m_occupantSide = static_cast<signed char>(targetSide_i);
                    m_hexCells[targetHex].m_occupantIndex = static_cast<signed char>(targetIndex_k);
                    m_hexCells[targetHex].m_occupantFrame = ARMY_FACING_LEFT;
                    m_hexCells[targetHex + 1].m_occupantSide = static_cast<signed char>(targetSide_i);
                    m_hexCells[targetHex + 1].m_occupantIndex = static_cast<signed char>(targetIndex_k);
                    m_hexCells[targetHex + 1].m_occupantFrame = ARMY_FACING_RIGHT;
                }
                RippleCreature(teleportArmy_i->m_side, teleportArmy_i->m_index, 2);
            } else {
                teleportArmy_i->m_hex = teleportDestination;
                m_hexCells[teleportArmy_i->m_hex].m_occupantSide = static_cast<signed char>(targetSide_i);
                m_hexCells[teleportArmy_i->m_hex].m_occupantIndex = static_cast<signed char>(targetIndex_k);
                m_hexCells[teleportArmy_i->m_hex].m_occupantFrame = COMBAT_HEX_EMPTY;
                RippleCreature(teleportArmy_i->m_side, teleportArmy_i->m_index, 2);
            }
            break;
        case SPELL_DISRUPTING_RAY:
            oldDefense = target_i->m_monster.defense;
            target_i->m_monster.defense -= SPELL_DISRUPTING_RAY_DEFENSE_REDUCTION;
            if (target_i->m_monster.defense < SPELL_MINIMUM_DEFENSE)
                target_i->m_monster.defense = SPELL_MINIMUM_DEFENSE;
            sprintf(gText, "The disrupting ray reduces defense by %d.",
                    oldDefense - target_i->m_monster.defense);
            CombatMessage(gText, 1, 1, 0);
            DoBlast(targetHex, spell);
            RippleCreature(target_i->m_side, target_i->m_index, 0);
            break;
        case SPELL_COLD_RAY:
            DelayMilli(static_cast<long>(gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_COLD_RAY_DELAY));
            damage_m = spellPower_i * SPELL_COLD_RAY_DAMAGE_PER_POWER;
            if (target_i->m_monsterType == SPELL_MONSTER_FIRE_ELEMENTAL)
                damage_m = spellPower_i * SPELL_COLD_RAY_FIRE_ELEMENTAL_DAMAGE_PER_POWER;
            if (target_i->m_monsterType == SPELL_MONSTER_IRON_GOLEM ||
                target_i->m_monsterType == SPELL_MONSTER_STEEL_GOLEM)
                damage_m = static_cast<long>(damage_m * SPELL_GOLEM_DAMAGE_MULTIPLIER);
            ModifyDamageForArtifacts(&damage_m, spell, m_heroes[m_currentSide],
                                     m_heroes[1 - m_currentSide]);
            coldRayArmyName_f = target_i->m_quantity < 2 ? gArmyNames[target_i->m_monsterType]
                                                     : gArmyNamesPlural[target_i->m_monsterType];
            sprintf(gText, "The cold ray does %d damage to the %s.", damage_m, coldRayArmyName_f);
            CombatMessage(gText, 1, 1, 0);
            DoBlast(targetHex, spell);
            target_i->SpellEffect(gsSpellInfo[SPELL_COLD_RAY].combatEffect, 0, 0);
            target_i->Damage(damage_m, -1);
            target_i->PowEffect(-1, 1, -1, -1);
            break;
        case SPELL_CHAIN_LIGHTNING:
            ChainLightning(targetHex, spellPower_i);
            break;
        case SPELL_MAGIC_ARROW:
            DelayMilli(static_cast<long>(gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_MAGIC_ARROW_DELAY));
            damage_m = spellPower_i * SPELL_MAGIC_ARROW_DAMAGE_PER_POWER;
            ModifyDamageForArtifacts(&damage_m, spell, m_heroes[m_currentSide],
                                     m_heroes[1 - m_currentSide]);
            magicArrowArmyName_n = target_i->m_quantity < 2 ? gArmyNames[target_i->m_monsterType]
                                                        : gArmyNamesPlural[target_i->m_monsterType];
            sprintf(gText, "The magic arrow does %d damage to the %s.", damage_m,
                    magicArrowArmyName_n);
            CombatMessage(gText, 1, 1, 0);
            missileIcon_p = gpResourceManager->GetIcon("keep.icn");
            missileAngles[0] = 90.0f;
            missileAngles[1] = 68.5f;
            missileAngles[2] = 45.0f;
            missileAngles[3] = 20.8f;
            missileAngles[4] = 0.0f;
            missileAngles[5] = -20.8f;
            missileAngles[6] = -45.0f;
            missileAngles[7] = -68.5f;
            missileAngles[8] = -90.0f;
            ShootMissile(castX, castY, target_i->MidX(), target_i->MidY(), missileAngles, missileIcon_p);
            gpResourceManager->Dispose(missileIcon_p);
            target_i->Damage(damage_m, -1);
            target_i->PowEffect(-1, 1, -1, -1);
            break;
        case SPELL_LIGHTNING_BOLT:
            damage_m = spellPower_i * SPELL_LIGHTNING_DAMAGE_PER_POWER;
            if (target_i->m_monsterType == SPELL_MONSTER_AIR_ELEMENTAL)
                damage_m = spellPower_i * SPELL_LIGHTNING_AIR_ELEMENTAL_DAMAGE_PER_POWER;
            if (target_i->m_monsterType == SPELL_MONSTER_IRON_GOLEM ||
                target_i->m_monsterType == SPELL_MONSTER_STEEL_GOLEM)
                damage_m = static_cast<long>(damage_m * SPELL_GOLEM_DAMAGE_MULTIPLIER);
            ModifyDamageForArtifacts(&damage_m, spell, m_heroes[m_currentSide],
                                     m_heroes[1 - m_currentSide]);
            lightningArmyName_d = target_i->m_quantity < 2 ? gArmyNames[target_i->m_monsterType]
                                                       : gArmyNamesPlural[target_i->m_monsterType];
            sprintf(gText, "The lightning bolt does %d damage to the %s.", damage_m,
                    lightningArmyName_d);
            CombatMessage(gText, 1, 1, 0);
            DoBolt(1, castX, castY, target_i->MidX(), target_i->MidY(), 150, 100, 9, 2, 301,
                   -40, 40, 30, 1, 0, 0, 1);
            target_i->SpellEffect(gsSpellInfo[SPELL_LIGHTNING_BOLT].combatEffect, 0, 0);
            target_i->Damage(damage_m, -1);
            target_i->PowEffect(-1, 1, -1, -1);
            break;
        case SPELL_MASS_CURE:
        case SPELL_MASS_HASTE:
        case SPELL_MASS_SLOW:
        case SPELL_MASS_BLESS:
        case SPELL_MASS_CURSE:
        case SPELL_HOLY_WORD:
        case SPELL_HOLY_SHOUT:
        case SPELL_MASS_DISPEL:
        case SPELL_DEATH_RIPPLE:
        case SPELL_DEATH_WAVE:
        case SPELL_MASS_SHIELD:
            CastMassSpell(spell, spellPower_i);
            break;
        case SPELL_MIRROR_IMAGE:
            MirrorImage(targetHex);
            break;
        case SPELL_SUMMON_EARTH_ELEMENTAL:
            SummonElemental(SPELL_MONSTER_EARTH_ELEMENTAL, spellPower_i);
            break;
        case SPELL_SUMMON_AIR_ELEMENTAL:
            SummonElemental(SPELL_MONSTER_AIR_ELEMENTAL, spellPower_i);
            break;
        case SPELL_SUMMON_FIRE_ELEMENTAL:
            SummonElemental(SPELL_MONSTER_FIRE_ELEMENTAL, spellPower_i);
            break;
        case SPELL_SUMMON_WATER_ELEMENTAL:
            SummonElemental(SPELL_MONSTER_WATER_ELEMENTAL, spellPower_i);
            break;
        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_ANIMATE_DEAD:
            Resurrect(spell, targetHex, spellPower_i);
            break;
        case SPELL_CURE:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_CURE].combatEffect, 0, 0);
            target_i->Cure(spellPower_i);
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            break;
        case SPELL_SLOW:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_SLOW, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_SLOW].combatEffect, 0, 0);
            break;
        case SPELL_HASTE:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_HASTE, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_HASTE].combatEffect, 0, 0);
            break;
        case SPELL_SHIELD:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_SHIELD, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_SHIELD].combatEffect, 0, 0);
            break;
        case SPELL_DRAGON_SLAYER:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_DRAGON_SLAYER, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_DRAGON_SLAYER].combatEffect, 0, 0);
            break;
        case SPELL_BLESS:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_BLESS, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_BLESS].combatEffect, 0, 0);
            break;
        case SPELL_STONE_SKIN:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_STONE_SKIN, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_STONE_SKIN].combatEffect, 0, 0);
            break;
        case SPELL_STEEL_SKIN:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_STEEL_SKIN, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_STEEL_SKIN].combatEffect, 0, 0);
            break;
        case SPELL_CURSE:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_CURSE, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_CURSE].combatEffect, 0, 0);
            break;
        case SPELL_BERSERKER:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_BERSERKER, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_BERSERKER].combatEffect, 0, 0);
            break;
        case SPELL_HYPNOTIZE:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_HYPNOTIZE, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_HYPNOTIZE].combatEffect, 0, 0);
            break;
        case SPELL_PARALYZE:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_PARALYZE, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_PARALYZE].combatEffect, 0, 0);
            break;
        case SPELL_CREATURE_DISPEL:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->DispelGood();
            target_i->SpellEffect(gsSpellInfo[SPELL_DISPEL].combatEffect, 0, 1);
            break;
        case SPELL_DISPEL:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->DispelGood();
            target_i->SpellEffect(gsSpellInfo[SPELL_DISPEL].combatEffect, 0, 0);
            for (influence = 0; influence < SPELL_INFLUENCE_COUNT; influence++)
                target_i->CancelIndividualSpell(influence);
            break;
        case SPELL_BLIND:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_BLIND, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_BLIND].combatEffect, 0, 0);
            break;
        case SPELL_BLOOD_LUST:
            ShowSpellMessage(castByCreature, spell, target_i);
            BloodLustEffect(target_i, SPELL_BLOOD_LUST_EFFECT);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_BLOOD_LUST, SPELL_BLOOD_LUST_DURATION);
            break;
        case SPELL_ANTI_MAGIC:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_ANTI_MAGIC, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_ANTI_MAGIC].combatEffect, 0, 0);
            break;
        case SPELL_PETRIFY:
            ShowSpellMessage(castByCreature, spell, target_i);
            TurnToStone(target_i);
            break;
        case SPELL_COLD_RING:
            Fireball(targetHex, SPELL_COLD_RING);
            break;
        case SPELL_FIREBALL:
            Fireball(targetHex, SPELL_FIREBALL);
            break;
        case SPELL_FIREBLAST:
            Fireball(targetHex, SPELL_FIREBLAST);
            break;
        case SPELL_METEOR_SHOWER:
            MeteorShower(targetHex);
            break;
        case SPELL_ELEMENTAL_STORM:
            ElementalStorm();
            break;
        case SPELL_ARMAGEDDON:
            Armageddon();
            break;
        case SPELL_EARTHQUAKE:
            Earthquake();
            break;
        default:
            DefaultSpell(targetHex);
            break;
        }
    }

    for (side_i = 0; side_i < COMBAT_SIDE_COUNT; side_i++) {
        for (armyIndex = 0; armyIndex < m_armyCount[side_i]; armyIndex++) {
            army &combatArmy = m_armies[side_i][armyIndex];
            combatArmy.m_animationValue2 = 0;
            combatArmy.m_animationValue3 = combatArmy.m_animationValue2;
            combatArmy.m_animationValue1 = combatArmy.m_animationValue3;
            combatArmy.m_drawState = 1;
            combatArmy.m_animationState = 0;
            combatArmy.m_lastTargetHex = COMBAT_HEX_EMPTY;
        }
    }

    if (castByCreature == 0) {
        m_heroAnimationState[m_currentSide]++;
        m_heroAnimationFrame[m_currentSide] = 0;
        while (m_heroAnimationFrame[m_currentSide] <
               sCmbtHero[m_heroSpriteIndex[m_currentSide]]
                   .animationFrameCount[m_heroAnimationState[m_currentSide]]) {
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            m_heroAnimationFrame[m_currentSide]++;
        }
        m_heroAnimationState[m_currentSide] = 0;
        m_heroAnimationFrame[m_currentSide] = 0;
        DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    }
    WaitEndSample(spellSample, -1);
    CheckChangeSelector();
}

VA(0x00423688, 0xda)
void combatManager::DefaultSpell(int) {}

VA(0x00423762, 0x623)
void combatManager::Fireball(int, int) {}

VA(0x00423d85, 0x3c9)
void combatManager::MeteorShower(int) {}

VA(0x0042414e, 0x2fb)
void combatManager::ElementalStorm(void) {}

VA(0x00424449, 0x9ff)
void combatManager::Armageddon(void) {}

VA(0x00424e48, 0x101)
void combatManager::TurnToStone(class army *) {}

VA(0x00424f49, 0x192)
void combatManager::BloodLustEffect(class army *, int) {}

VA(0x004250db, 0x80)
void combatManager::Ripple(int) {}

VA(0x0042515b, 0x72)
void combatManager::Blur(int, int, int) {}

VA(0x004251cd, 0x320)
void combatManager::ResetBoltAngle(struct SBolt *) {}

VA(0x004254ed, 0x4f0)
void combatManager::DrawBolt(struct SBolt *, int) {}

VA(0x004259dd, 0x26f)
void combatManager::AddBolt(struct SBolt *, int, int, int, int, int, int, int, int, int, int, int, int) {}

VA(0x00425c4c, 0xa82)
void combatManager::DoBolt(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int) {}

VA(0x004266ce, 0x18c)
int combatManager::GetNextChainLightningTarget(class army *, int) { return 0; }

VA(0x0042685a, 0x361)
void combatManager::ChainLightning(int, int) {}

VA(0x00426bbb, 0x292)
void combatManager::VaporizeCreature(int, int) {}

VA(0x00426e4d, 0x592)
void combatManager::RippleCreature(int, int, int) {}

VA(0x004273df, 0x6b2)
void combatManager::ShowMassSpell(signed char (* const)[20], int, int) {}

VA(0x00427a91, 0x8f8)
void combatManager::CastMassSpell(int, int) {}

VA(0x00428389, 0x5c8)
void combatManager::MirrorImage(int) {}

VA(0x00428951, 0x218)
void combatManager::SummonElemental(int, int) {}

VA(0x00428b69, 0x1e6)
void combatManager::DoLuck(int, int) {}

VA(0x00428d4f, 0x33a)
void combatManager::DoBlast(int, int) {}

VA(0x00429089, 0x655)
void combatManager::Resurrect(int, int, int) {}

VA(0x004296de, 0xb9)
int combatManager::SpaceForElementalExists(void) { return 0; }

VA(0x00429797, 0xd9)
void combatManager::ShowSpellCastFailure(class army *, int) {}

VA(0x00429870, 0x270)
void combatManager::ModifyDamageForArtifacts(long int *, int, class hero *, class hero *) {}

VA(0x00429ae0, 0x931)
void combatManager::Earthquake(void) {}

VA(0x0042a411, 0x2b1)
void combatManager::ShowSpellMessage(int, int, class army *) {}

// ---- globals (definitions, RVA order) ----
DATA(0x004f00b0) int castX;
DATA(0x004f00b4) int castY;
DATA(0x004f00b8) int bInTeleportGetDest;
DATA(0x004f0278) int indexToCastOn;
DATA(0x004f04c0) unsigned char *uRedBeam;
DATA(0x004f04c8) unsigned char *uRainbow;
DATA(0x004f04e0) signed char *gyModify;
