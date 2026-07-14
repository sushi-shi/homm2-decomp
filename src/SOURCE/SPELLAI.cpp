// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SPELLAI.OBJ   from: (directly linked into exe)
// functions: 11   data: 3
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/KB.h>
#include <SOURCE/SPELLS.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/hero.h>
VA(0x004867c0, 0x279)
int combatManager::DoSpellAI(int side, int restricted)
{
    int bestEffect = 0;
    int bestSpell = -1;
    int bestHex = -1;

    if (m_heroes[side] == 0)
        return 0;

    int heroSide;
    for (heroSide = 0; heroSide < COMBAT_SIDE_COUNT; heroSide++) {
        if (m_heroes[heroSide] != 0 &&
            m_heroes[heroSide]->HasArtifact(SPELL_ARTIFACT_SPHERE_NEGATION))
            return 0;
    }

    int spell;
    for (spell = 0; spell < COMBAT_SPELL_COUNT; spell++) {
        if (m_heroes[side]->HasSpell(spell) &&
            (gsSpellInfo[spell].m_e & SPELL_ATTRIBUTE_COMBAT) &&
            GetManaCost(spell, m_heroes[side]) <= m_heroes[side]->m_spellPoints) {
            if (restricted && spell != SPELL_FIREBALL && spell != SPELL_FIREBLAST &&
                spell != SPELL_LIGHTNING_BOLT && spell != SPELL_CHAIN_LIGHTNING &&
                spell != SPELL_HOLY_WORD && spell != SPELL_HOLY_SHOUT &&
                spell != SPELL_MAGIC_ARROW && spell != SPELL_ARMAGEDDON &&
                spell != SPELL_ELEMENTAL_STORM && spell != SPELL_METEOR_SHOWER &&
                spell != SPELL_COLD_RAY && spell != SPELL_COLD_RING &&
                spell != SPELL_DEATH_RIPPLE && spell != SPELL_DEATH_WAVE)
                continue;
            int effect;
            int hex;
            DetermineEffectOfSpell(spell, &effect, &hex);

            int manaRatio =
                m_heroes[side]->m_spellPoints / GetManaCost(spell, m_heroes[side]);
            if (manaRatio > COMBAT_SPELL_AI_MAX_MANA_RATIO)
                manaRatio = COMBAT_SPELL_AI_MAX_MANA_RATIO;
            effect = static_cast<int>(effect * gfSpellCastableCombatMod[manaRatio]);

            if (bestEffect < effect) {
                bestEffect = effect;
                bestSpell = spell;
                bestHex = hex;
            }
        }
    }

    if (bestEffect > 0) {
        giNextAction = COMBAT_SPELL_AI_CAST_ACTION;
        giNextActionExtra = bestSpell;
        giNextActionGridIndex = bestHex;
        return 1;
    }
    return 0;
}

VA(0x00486a39, 0x1155)
void combatManager::DetermineEffectOfSpell(int spell, int *bestEffect, int *bestHex)
{
    int done = 0;
    int side = 0;
    int hex = COMBAT_SPELL_AI_FIRST_HEX;
    float durationMod = 1.0f;
    int fullQuantity = 1;
    int totalEffect = 0;
    army *target = 0;
    *bestEffect = 0;

    int targetMode;
    switch (spell) {
    case SPELL_MASS_CURE:
    case SPELL_MASS_DISPEL:
    case SPELL_ARMAGEDDON:
    case SPELL_ELEMENTAL_STORM:
    case SPELL_DEATH_RIPPLE:
    case SPELL_DEATH_WAVE:
    case SPELL_HOLY_WORD:
    case SPELL_HOLY_SHOUT:
    case SPELL_SUMMON_EARTH_ELEMENTAL:
    case SPELL_SUMMON_AIR_ELEMENTAL:
    case SPELL_SUMMON_FIRE_ELEMENTAL:
    case SPELL_SUMMON_WATER_ELEMENTAL:
    case SPELL_EARTHQUAKE:
        targetMode = COMBAT_SPELL_AI_GLOBAL;
        break;
    case SPELL_FIREBALL:
    case SPELL_FIREBLAST:
    case SPELL_METEOR_SHOWER:
    case SPELL_COLD_RING:
        targetMode = COMBAT_SPELL_AI_AREA;
        break;
    case SPELL_MASS_HASTE:
    case SPELL_MASS_BLESS:
    case SPELL_MASS_SHIELD:
        targetMode = COMBAT_SPELL_AI_SUM_FRIENDLY;
        side = m_currentSide;
        break;
    case SPELL_MASS_SLOW:
    case SPELL_MASS_CURSE:
        targetMode = COMBAT_SPELL_AI_SUM_ENEMY;
        side = 1 - m_currentSide;
        break;
    case SPELL_DISPEL:
        targetMode = COMBAT_SPELL_AI_ANY_ARMY;
        side = COMBAT_SPELL_AI_ANY_SIDE;
        break;
    case SPELL_TELEPORT:
    case SPELL_CURE:
    case SPELL_HASTE:
    case SPELL_BLESS:
    case SPELL_STONE_SKIN:
    case SPELL_STEEL_SKIN:
    case SPELL_ANTI_MAGIC:
    case SPELL_DRAGON_SLAYER:
    case SPELL_BLOOD_LUST:
    case SPELL_MIRROR_IMAGE:
    case SPELL_SHIELD:
        targetMode = COMBAT_SPELL_AI_FRIENDLY;
        side = m_currentSide;
        break;
    case SPELL_RESURRECT:
    case SPELL_TRUE_RESURRECT:
    case SPELL_ANIMATE_DEAD:
        targetMode = COMBAT_SPELL_AI_RESURRECT;
        side = m_currentSide;
        break;
    case SPELL_LIGHTNING_BOLT:
    case SPELL_CHAIN_LIGHTNING:
    case SPELL_SLOW:
    case SPELL_BLIND:
    case SPELL_CURSE:
    case SPELL_MAGIC_ARROW:
    case SPELL_BERSERKER:
    case SPELL_PARALYZE:
    case SPELL_HYPNOTIZE:
    case SPELL_DISRUPTING_RAY:
        targetMode = COMBAT_SPELL_AI_ENEMY;
        side = 1 - m_currentSide;
        break;
    default:
        *bestEffect = 0;
        return;
    }

    if (targetMode == COMBAT_SPELL_AI_RESURRECT)
        done = FirstResurrectable(COMBAT_SPELL_AI_FIRST_HEX, &hex, spell);
    if (targetMode == COMBAT_SPELL_AI_FRIENDLY ||
        targetMode == COMBAT_SPELL_AI_ENEMY ||
        targetMode == COMBAT_SPELL_AI_SUM_ENEMY ||
        targetMode == COMBAT_SPELL_AI_SUM_FRIENDLY ||
        targetMode == COMBAT_SPELL_AI_ANY_ARMY)
        done = FirstArmy(COMBAT_SPELL_AI_FIRST_HEX, side, &hex);

    while (!done) {
        int hasMindEffect = 0;
        int hasDamageReduction = 0;
        int effect = 0;

        if (m_hexCells[hex].m_occupantIndex < 0 ||
            m_hexCells[hex].m_occupantSide < 0) {
            target = 0;
        } else {
            target = &m_armies[m_hexCells[hex].m_occupantSide]
                                [m_hexCells[hex].m_occupantIndex];
            giCurrSpellGroup = m_hexCells[hex].m_occupantSide;
            fullQuantity =
                (target->m_monster.flags.all & MONSTER_FLAGS_FULL_AI_QUANTITY) != 0;

            int spellPower = m_spellPower[m_currentSide];
            if (m_heroes[m_currentSide]->HasArtifact(
                    SPELL_ARTIFACT_ENCHANTED_HOURGLASS))
                spellPower += SPELL_HOURGLASS_POWER_BONUS;
            if (m_heroes[m_currentSide]->HasArtifact(SPELL_ARTIFACT_WIZARD_HAT))
                spellPower += SPELL_WIZARD_HAT_POWER_BONUS;

            durationMod = gfDurationMods[
                spellPower - fullQuantity >= COMBAT_SPELL_AI_MAX_DURATION
                    ? COMBAT_SPELL_AI_MAX_DURATION
                    : spellPower - fullQuantity];

            if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_HYPNOTIZE] ||
                target->m_spellInfluence[ARMY_SPELL_INFLUENCE_BERSERK])
                hasMindEffect = 1;
            if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_BLIND] ||
                target->m_spellInfluence[ARMY_SPELL_INFLUENCE_PARALYZE] ||
                target->m_spellInfluence[ARMY_SPELL_INFLUENCE_PETRIFIED])
                hasDamageReduction = 1;
        }

        switch (spell) {
        case SPELL_CURE:
            EffectSpellCure(&effect, m_currentSide, hex, 1);
            break;
        case SPELL_MASS_CURE:
            EffectSpellCure(&effect, m_currentSide, -1, 1);
            break;
        case SPELL_DISPEL:
            EffectSpellCure(&effect, target->m_side, target->m_index, 0);
            break;
        case SPELL_MASS_DISPEL:
            EffectSpellCure(&effect, COMBAT_SPELL_AI_ANY_SIDE, -1, 0);
            break;
        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_ANIMATE_DEAD:
            if (hasMindEffect) {
            } else {
                EffectSpellResurrect(&effect, hex, spell);
            }
            break;
        case SPELL_MIRROR_IMAGE:
        case SPELL_SUMMON_EARTH_ELEMENTAL:
        case SPELL_SUMMON_AIR_ELEMENTAL:
        case SPELL_SUMMON_FIRE_ELEMENTAL:
        case SPELL_SUMMON_WATER_ELEMENTAL:
            effect = EffectSpellCreateCreature(hex, spell);
            break;
        case SPELL_FIREBALL:
        case SPELL_FIREBLAST:
        case SPELL_LIGHTNING_BOLT:
        case SPELL_CHAIN_LIGHTNING:
        case SPELL_HOLY_WORD:
        case SPELL_HOLY_SHOUT:
        case SPELL_MAGIC_ARROW:
        case SPELL_ARMAGEDDON:
        case SPELL_ELEMENTAL_STORM:
        case SPELL_METEOR_SHOWER:
        case SPELL_COLD_RAY:
        case SPELL_COLD_RING:
        case SPELL_DISRUPTING_RAY:
        case SPELL_DEATH_RIPPLE:
        case SPELL_DEATH_WAVE:
            if (hasMindEffect) {
            } else {
                EffectSpellDamage(&effect, spell, hex);
                if (hasDamageReduction)
                    effect = static_cast<int>(
                        effect * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER);
            }
            break;
        case SPELL_HASTE:
        case SPELL_MASS_HASTE:
            if (hasDamageReduction) {
            } else {
                if (hasMindEffect) {
                } else {
                    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_HASTE]) {
                    } else {
                        effect = static_cast<int>(
                            RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_HASTE) *
                            durationMod);
                        if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_SLOW]) {
                            effect = static_cast<int>(
                                RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_SLOW) *
                                    gfCancelDurationMods[
                                        target->m_spellInfluence[ARMY_SPELL_INFLUENCE_SLOW] +
                                                    fullQuantity >=
                                                COMBAT_SPELL_AI_MAX_DURATION
                                            ? COMBAT_SPELL_AI_MAX_DURATION
                                            : target->m_spellInfluence
                                                      [ARMY_SPELL_INFLUENCE_SLOW] +
                                                  fullQuantity] -
                                effect);
                        }
                    }
                }
            }
            break;
        case SPELL_BERSERKER:
            if (hasDamageReduction) {
            } else {
                if (hasMindEffect) {
                } else {
                    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_BERSERK]) {
                    } else {
                        effect = static_cast<int>(
                            RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_BERSERK) *
                            durationMod);
                        if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_HYPNOTIZE]) {
                            effect = static_cast<int>(
                                RawEffectSpellInfluence(target,
                                                        ARMY_SPELL_INFLUENCE_HYPNOTIZE) *
                                    gfCancelDurationMods[
                                        target->m_spellInfluence
                                                    [ARMY_SPELL_INFLUENCE_HYPNOTIZE] +
                                                fullQuantity >=
                                            COMBAT_SPELL_AI_MAX_DURATION
                                        ? COMBAT_SPELL_AI_MAX_DURATION
                                        : target->m_spellInfluence
                                                  [ARMY_SPELL_INFLUENCE_HYPNOTIZE] +
                                              fullQuantity] -
                                effect);
                        }
                    }
                }
            }
            break;
        case SPELL_HYPNOTIZE:
            if (hasDamageReduction) {
            } else {
                if (hasMindEffect) {
                } else {
                    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_HYPNOTIZE]) {
                    } else {
                        effect = static_cast<int>(
                            RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_HYPNOTIZE) *
                            durationMod);
                        if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_BERSERK]) {
                            effect = static_cast<int>(
                                RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_BERSERK) *
                                    gfCancelDurationMods[
                                        target->m_spellInfluence[ARMY_SPELL_INFLUENCE_BERSERK] +
                                                    fullQuantity >=
                                                COMBAT_SPELL_AI_MAX_DURATION
                                            ? COMBAT_SPELL_AI_MAX_DURATION
                                            : target->m_spellInfluence
                                                      [ARMY_SPELL_INFLUENCE_BERSERK] +
                                                  fullQuantity] -
                                effect);
                        }
                    }
                }
            }
            break;
        case SPELL_SLOW:
        case SPELL_MASS_SLOW:
            if (hasDamageReduction) {
            } else {
                if (hasMindEffect) {
                } else {
                    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_SLOW]) {
                    } else {
                        effect = static_cast<int>(
                            RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_SLOW) *
                            durationMod);
                        if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_HASTE]) {
                            effect = static_cast<int>(
                                RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_HASTE) *
                                    gfCancelDurationMods[
                                        target->m_spellInfluence[ARMY_SPELL_INFLUENCE_HASTE] +
                                                    fullQuantity >=
                                                COMBAT_SPELL_AI_MAX_DURATION
                                            ? COMBAT_SPELL_AI_MAX_DURATION
                                            : target->m_spellInfluence
                                                      [ARMY_SPELL_INFLUENCE_HASTE] +
                                                  fullQuantity] -
                                effect);
                        }
                    }
                }
            }
            break;
        case SPELL_BLESS:
        case SPELL_MASS_BLESS:
            if (hasDamageReduction) {
            } else {
                if (hasMindEffect) {
                } else {
                    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_BLESS]) {
                    } else {
                        effect = static_cast<int>(
                            RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_BLESS) *
                            durationMod);
                        if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_CURSE]) {
                            effect = static_cast<int>(
                                RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_CURSE) *
                                    gfCancelDurationMods[
                                        target->m_spellInfluence[ARMY_SPELL_INFLUENCE_CURSE] +
                                                    fullQuantity >=
                                                COMBAT_SPELL_AI_MAX_DURATION
                                            ? COMBAT_SPELL_AI_MAX_DURATION
                                            : target->m_spellInfluence
                                                      [ARMY_SPELL_INFLUENCE_CURSE] +
                                                  fullQuantity] -
                                effect);
                        }
                    }
                }
            }
            break;
        case SPELL_CURSE:
        case SPELL_MASS_CURSE:
            if (hasDamageReduction) {
            } else {
                if (hasMindEffect) {
                } else {
                    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_CURSE]) {
                    } else {
                        effect = static_cast<int>(
                            -RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_CURSE) *
                            durationMod);
                        if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_BLESS]) {
                            effect = static_cast<int>(
                                RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_BLESS) *
                                    gfCancelDurationMods[
                                        target->m_spellInfluence[ARMY_SPELL_INFLUENCE_BLESS] +
                                                    fullQuantity >=
                                                COMBAT_SPELL_AI_MAX_DURATION
                                            ? COMBAT_SPELL_AI_MAX_DURATION
                                            : target->m_spellInfluence
                                                      [ARMY_SPELL_INFLUENCE_BLESS] +
                                                  fullQuantity] -
                                effect);
                        }
                    }
                }
            }
            break;
        case SPELL_ANTI_MAGIC:
            if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_ANTI_MAGIC]) {
            } else {
                if (spell == SPELL_ANTI_MAGIC && m_heroes[1 - m_currentSide] == 0) {
                    effect = 0;
                } else {
                    effect = static_cast<int>(
                        RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_ANTI_MAGIC) *
                        durationMod);
                    int cureEffect;
                    EffectSpellCure(&cureEffect, target->m_side, target->m_index, 0);
                    effect += cureEffect;
                }
            }
            break;
        case SPELL_STONE_SKIN:
            if (hasMindEffect) {
            } else {
                if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_STONESKIN]) {
                } else {
                    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_STEELSKIN]) {
                    } else {
                        effect = static_cast<int>(
                            RawEffectSpellInfluence(target,
                                                    ARMY_SPELL_INFLUENCE_STONESKIN) *
                            durationMod);
                        if (m_combatTowns[COMBAT_DEFENDER_SIDE] != 0 &&
                            target->m_side == COMBAT_ATTACKER_SIDE &&
                            (target->m_monster.flags.all & MONSTER_FLAGS_SHOOTER))
                            effect = static_cast<int>(
                                effect * COMBAT_SPELL_AI_SIEGE_SHOOTER_MODIFIER);
                        if (hasDamageReduction)
                            effect = static_cast<int>(
                                effect * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER);
                    }
                }
            }
            break;
        case SPELL_STEEL_SKIN:
            if (hasMindEffect) {
            } else {
                if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_STONESKIN]) {
                } else {
                    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_STEELSKIN]) {
                    } else {
                        effect = static_cast<int>(
                            RawEffectSpellInfluence(target,
                                                    ARMY_SPELL_INFLUENCE_STEELSKIN) *
                            durationMod);
                        if (m_combatTowns[COMBAT_DEFENDER_SIDE] != 0 &&
                            target->m_side == COMBAT_ATTACKER_SIDE &&
                            (target->m_monster.flags.all & MONSTER_FLAGS_SHOOTER))
                            effect = static_cast<int>(
                                effect * COMBAT_SPELL_AI_SIEGE_SHOOTER_MODIFIER);
                        if (hasDamageReduction)
                            effect = static_cast<int>(
                                effect * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER);
                    }
                }
            }
            break;
        case SPELL_BLOOD_LUST:
            if (hasDamageReduction) {
            } else {
                if (hasMindEffect) {
                } else {
                    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_BLOODLUST]) {
                    } else {
                        effect = static_cast<int>(
                            RawEffectSpellInfluence(target,
                                                    ARMY_SPELL_INFLUENCE_BLOODLUST) *
                            durationMod);
                    }
                }
            }
            break;
        case SPELL_SHIELD:
        case SPELL_MASS_SHIELD:
            if (hasMindEffect) {
            } else {
                if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_SHIELD]) {
                } else {
                    effect = static_cast<int>(
                        RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_SHIELD) *
                        durationMod);
                    if (m_combatTowns[COMBAT_DEFENDER_SIDE] != 0 &&
                        target->m_side == COMBAT_ATTACKER_SIDE &&
                        (target->m_monster.flags.all & MONSTER_FLAGS_SHOOTER))
                        effect <<= 1;
                    if (hasDamageReduction)
                        effect = static_cast<int>(
                            effect * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER);
                }
            }
            break;
        case SPELL_BLIND:
            if (hasDamageReduction) {
            } else {
                if (hasMindEffect) {
                } else {
                    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_BLIND]) {
                    } else {
                        effect = static_cast<int>(
                            RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_BLIND) *
                            durationMod);
                    }
                }
            }
            break;
        case SPELL_PARALYZE:
            if (hasDamageReduction) {
            } else {
                if (hasMindEffect) {
                } else {
                    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_PARALYZE]) {
                    } else {
                        effect = static_cast<int>(
                            RawEffectSpellInfluence(target, ARMY_SPELL_INFLUENCE_PARALYZE) *
                            durationMod);
                    }
                }
            }
            break;
        case SPELL_DRAGON_SLAYER:
            if (hasDamageReduction) {
            } else {
                if (hasMindEffect) {
                } else {
                    if (target->m_spellInfluence[ARMY_SPELL_INFLUENCE_DRAGON_SLAYER]) {
                    } else {
                        effect = static_cast<int>(
                            RawEffectSpellInfluence(
                                target, ARMY_SPELL_INFLUENCE_DRAGON_SLAYER) *
                            durationMod);
                    }
                }
            }
            break;
        case SPELL_TELEPORT:
            effect = 0;
            break;
        case SPELL_EARTHQUAKE:
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0) {
                int damagedWalls = 0;
                int wall;
                for (wall = 0; wall < COMBAT_SPELL_AI_EARTHQUAKE_WALL_COUNT; wall++) {
                    if (m_wallStates[wall + COMBAT_SPELL_AI_EARTHQUAKE_WALL_FIRST] ==
                            COMBAT_SPELL_AI_WALL_DAMAGED ||
                        m_wallStates[wall + COMBAT_SPELL_AI_EARTHQUAKE_WALL_FIRST] ==
                            COMBAT_SPELL_AI_WALL_DESTROYED)
                        damagedWalls++;
                }
                if (damagedWalls == 0)
                    effect = COMBAT_SPELL_AI_EARTHQUAKE_NO_DAMAGE_SCORE;
                else if (damagedWalls < COMBAT_SPELL_AI_EARTHQUAKE_WALL_COUNT)
                    effect = (COMBAT_SPELL_AI_EARTHQUAKE_WALL_COUNT - damagedWalls) *
                             COMBAT_SPELL_AI_EARTHQUAKE_WALL_SCORE;
                else
                    effect = 0;
            } else {
                effect = 0;
            }
            break;
        default:
            *bestEffect = 0;
            return;
        }

        switch (targetMode) {
        case COMBAT_SPELL_AI_GLOBAL:
        case COMBAT_SPELL_AI_AREA:
        case COMBAT_SPELL_AI_FRIENDLY:
        case COMBAT_SPELL_AI_ENEMY:
        case COMBAT_SPELL_AI_RESURRECT:
        case COMBAT_SPELL_AI_ANY_ARMY:
            totalEffect = effect;
            break;
        case COMBAT_SPELL_AI_SUM_FRIENDLY:
        case COMBAT_SPELL_AI_SUM_ENEMY:
            totalEffect += effect;
        }

        if (targetMode == COMBAT_SPELL_AI_SUM_FRIENDLY ||
            targetMode == COMBAT_SPELL_AI_SUM_ENEMY || *bestEffect < totalEffect) {
            *bestEffect = totalEffect;
            *bestHex = hex;
        }

        switch (targetMode) {
        case COMBAT_SPELL_AI_GLOBAL:
            done = 1;
            break;
        case COMBAT_SPELL_AI_SUM_FRIENDLY:
        case COMBAT_SPELL_AI_SUM_ENEMY:
        case COMBAT_SPELL_AI_FRIENDLY:
        case COMBAT_SPELL_AI_ENEMY:
        case COMBAT_SPELL_AI_ANY_ARMY:
            done = FirstArmy(hex + 1, side, &hex);
            break;
        case COMBAT_SPELL_AI_AREA:
            NextPos(&hex);
            if (hex > COMBAT_SPELL_AI_LAST_HEX)
                done = 1;
            break;
        case COMBAT_SPELL_AI_RESURRECT:
            done = FirstResurrectable(hex + 1, &hex, spell);
        }
    }
}

VA(0x00487b8e, 0x34c)
int combatManager::EffectSpellCreateCreature(int, int) { return 0; }

VA(0x00487eda, 0x72d)
int combatManager::RawEffectSpellInfluence(class army *, int) { return 0; }

VA(0x00488607, 0x66)
void combatManager::ClearEffects(void) {}

VA(0x0048866d, 0x40)
void combatManager::NextPos(int *) {}

VA(0x004886ad, 0xa1)
int combatManager::FirstArmy(int, int, int *) { return 0; }

VA(0x0048874e, 0x73)
int combatManager::FirstResurrectable(int, int *, int) { return 0; }

VA(0x004887c1, 0x421)
void combatManager::EffectSpellCure(int *, int, int, int) {}

VA(0x00488be2, 0x176)
void combatManager::EffectSpellResurrect(int *, int, int) {}

VA(0x00488d58, 0xcc9)
void combatManager::EffectSpellDamage(int *, int, int) {}

// ---- globals (definitions, RVA order) ----
DATA(0x004f80b8) float gfDurationMods[12];
DATA(0x004f80e8) float gfCancelDurationMods[11];
DATA(0x005284b4) int giCurrSpellGroup;
