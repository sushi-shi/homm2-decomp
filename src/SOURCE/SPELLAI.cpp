// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SPELLAI.OBJ   from: (directly linked into exe)
// functions: 11   data: 3
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/KB.h>
#include <SOURCE/PATH.h>
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

    int spell;
    for (spell = 0; spell < COMBAT_SIDE_COUNT; spell++) {
        if (m_heroes[spell] != 0 &&
            m_heroes[spell]->HasArtifact(SPELL_ARTIFACT_SPHERE_NEGATION))
            return 0;
    }

    for (spell = 0; spell < COMBAT_SPELL_COUNT; spell++) {
        if (m_heroes[side]->HasSpell(spell) &&
            (gsSpellInfo[spell].attributes & SPELL_ATTRIBUTE_COMBAT) &&
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

            if (effect > bestEffect) {
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
    float durationMod = COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER;
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
int combatManager::EffectSpellCreateCreature(int hex, int spell)
{
    float workChance = COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER;
    int spellPower = m_spellPower[m_currentSide];

    if ((spell == SPELL_SUMMON_EARTH_ELEMENTAL ||
         spell == SPELL_SUMMON_AIR_ELEMENTAL ||
         spell == SPELL_SUMMON_FIRE_ELEMENTAL ||
         spell == SPELL_SUMMON_WATER_ELEMENTAL) &&
        m_heroes[m_currentSide] != 0 &&
        m_heroes[m_currentSide]->HasArtifact(SPELL_ARTIFACT_BOOK_ELEMENTS))
        spellPower <<= 1;

    if ((spell == SPELL_SUMMON_EARTH_ELEMENTAL ||
         spell == SPELL_SUMMON_AIR_ELEMENTAL ||
         spell == SPELL_SUMMON_FIRE_ELEMENTAL ||
         spell == SPELL_SUMMON_WATER_ELEMENTAL) &&
        !SpaceForElementalExists())
        return 0;

    int monster;
    switch (spell) {
    case SPELL_SUMMON_EARTH_ELEMENTAL:
        monster = SPELL_MONSTER_EARTH_ELEMENTAL;
        break;
    case SPELL_SUMMON_AIR_ELEMENTAL:
        monster = SPELL_MONSTER_AIR_ELEMENTAL;
        break;
    case SPELL_SUMMON_FIRE_ELEMENTAL:
        monster = SPELL_MONSTER_FIRE_ELEMENTAL;
        break;
    case SPELL_SUMMON_WATER_ELEMENTAL:
        monster = SPELL_MONSTER_WATER_ELEMENTAL;
        break;
    default:
        workChance =
            m_armies[m_hexCells[hex].m_occupantSide]
                    [m_hexCells[hex].m_occupantIndex]
                        .SpellCastWorkChance(SPELL_MIRROR_IMAGE);
        monster = m_armies[m_hexCells[hex].m_occupantSide]
                            [m_hexCells[hex].m_occupantIndex]
                                .m_monsterType;
        spellPower = m_armies[m_hexCells[hex].m_occupantSide]
                             [m_hexCells[hex].m_occupantIndex]
                                 .m_quantity *
                     SPELL_DEFAULT_CREATURE_POWER;
        break;
    }

    int effect = gMonsterDatabase[monster].fightValue * spellPower;
    if (spell == SPELL_MIRROR_IMAGE) {
        float mirrorMod;
        if (m_spellPower[m_currentSide] == COMBAT_SPELL_AI_MIRROR_POWER_ONE)
            mirrorMod = COMBAT_SPELL_AI_MIRROR_POWER_ONE_MODIFIER;
        else if (m_spellPower[m_currentSide] == COMBAT_SPELL_AI_MIRROR_POWER_TWO)
            mirrorMod = COMBAT_SPELL_AI_MIRROR_POWER_TWO_MODIFIER;
        else
            mirrorMod = COMBAT_SPELL_AI_MIRROR_DEFAULT_MODIFIER;
        effect = static_cast<int>(effect * mirrorMod);
        if (gMonsterDatabase[monster].flags.bytes.abilities & 4)
            effect = static_cast<int>(effect *
                                      COMBAT_SPELL_AI_MIRROR_SHOOTER_MODIFIER);
    }
    return static_cast<int>(effect * workChance);
}

VA(0x00487eda, 0x72d)
int combatManager::RawEffectSpellInfluence(army *target, int influence)
{
    int effect = 0;
    army *otherArmy = 0;
    float workChance =
        target->SpellCastWorkChance(giSpellInfluenceToSpell[influence]);
    if (workChance <= COMBAT_SPELL_AI_ZERO_EFFECT)
        return 0;

    int armyValue = target->m_monster.fightValue * target->m_quantity;
    switch (influence) {
    case ARMY_SPELL_INFLUENCE_HASTE:
    case ARMY_SPELL_INFLUENCE_SLOW: {
        int newSpeed;
        if (influence == ARMY_SPELL_INFLUENCE_SLOW) {
            newSpeed = (target->m_monster.speed + 1) >> 1;
        } else {
            newSpeed = target->m_monster.speed +
                       COMBAT_SPELL_AI_HASTE_SPEED_BONUS;
            if (target->m_monster.flags.all & MONSTER_FLAGS_FLYING)
                return 0;
        }
        if (m_inCastleCombat && target->m_side == 1)
            return 0;
        if (target->m_monster.flags.all & MONSTER_FLAGS_SHOOTER)
            return 0;
        int attackMask = target->GetAttackMask(target->m_hex, 1, -1);
        if (attackMask != COMBAT_SPELL_AI_ALL_ATTACK_DIRECTIONS)
            return 0;

        int column = target->m_hex % ARMY_HEX_COLUMNS;
        int distance;
        if (m_currentSide == 0)
            distance = column - COMBAT_SPELL_AI_MINIMUM_DISTANCE;
        else
            distance = COMBAT_SPELL_AI_RIGHT_DISTANCE_COLUMN - column;
        if (distance < 0)
            distance = 0;
        distance += COMBAT_SPELL_AI_MINIMUM_DISTANCE;
        if (m_inCastleCombat)
            distance += COMBAT_SPELL_AI_CASTLE_DISTANCE_BONUS;

        float oldTurns;
        if (target->m_monster.flags.all & MONSTER_FLAGS_FLYING)
            oldTurns = COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER;
        else
            oldTurns = static_cast<float>(distance) / target->m_monster.speed;
        float newTurns = static_cast<float>(distance) / newSpeed;
        if (newTurns > COMBAT_SPELL_AI_TURN_CAP)
            newTurns = COMBAT_SPELL_AI_TURN_CAP;
        if (oldTurns > COMBAT_SPELL_AI_TURN_CAP)
            oldTurns = COMBAT_SPELL_AI_TURN_CAP;
        effect = static_cast<int>((oldTurns - newTurns) /
                                  COMBAT_SPELL_AI_TURN_DIVISOR * armyValue);
        break;
    }
    case ARMY_SPELL_INFLUENCE_BLESS:
    case ARMY_SPELL_INFLUENCE_CURSE: {
        float averageDamage =
            (static_cast<float>(target->m_monster.damageMin) +
             static_cast<float>(target->m_monster.damageMax)) *
            COMBAT_SPELL_AI_AVERAGE_DAMAGE_MODIFIER;
        float damageEffect =
            static_cast<float>((target->m_monster.damageMax - averageDamage) /
                               averageDamage * armyValue *
                               COMBAT_SPELL_AI_BLESS_CURSE_MODIFIER);
        effect = static_cast<int>(influence == ARMY_SPELL_INFLUENCE_BLESS
                                      ? damageEffect
                                      : -damageEffect);
        break;
    }
    case ARMY_SPELL_INFLUENCE_BLIND:
        effect = static_cast<int>(armyValue * COMBAT_SPELL_AI_BLIND_MODIFIER);
        break;
    case ARMY_SPELL_INFLUENCE_BERSERK:
        effect = static_cast<int>(armyValue * COMBAT_SPELL_AI_BERSERK_MODIFIER);
        break;
    case ARMY_SPELL_INFLUENCE_PARALYZE:
        effect = static_cast<int>(armyValue * COMBAT_SPELL_AI_PARALYZE_MODIFIER);
        break;
    case ARMY_SPELL_INFLUENCE_HYPNOTIZE:
        effect = static_cast<int>(armyValue *
                                  COMBAT_SPELL_AI_HYPNOTIZE_MODIFIER);
        break;
    case ARMY_SPELL_INFLUENCE_BLOODLUST:
        effect = static_cast<int>(armyValue *
                                  COMBAT_SPELL_AI_BLOODLUST_MODIFIER);
        break;
    case ARMY_SPELL_INFLUENCE_PETRIFIED:
        effect = static_cast<int>(armyValue *
                                  COMBAT_SPELL_AI_PETRIFIED_MODIFIER);
        break;
    case ARMY_SPELL_INFLUENCE_ANTI_MAGIC:
        effect = static_cast<int>(armyValue *
                                  COMBAT_SPELL_AI_ANTI_MAGIC_MODIFIER);
        break;
    case ARMY_SPELL_INFLUENCE_STONESKIN:
        effect = static_cast<int>(armyValue *
                                  COMBAT_SPELL_AI_STONE_SKIN_MODIFIER);
        break;
    case ARMY_SPELL_INFLUENCE_STEELSKIN:
        effect = static_cast<int>(armyValue *
                                  COMBAT_SPELL_AI_STEEL_SKIN_MODIFIER);
        break;
    case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER: {
        int adjacent = 0;
        int dragonCount = adjacent;
        int i;
        for (i = 0; i < m_armyCount[1 - target->m_side]; i++) {
            otherArmy = &m_armies[target->m_side][i];
            if (otherArmy->m_monsterType == ARMY_CREATURE_GREEN_DRAGON ||
                otherArmy->m_monsterType == ARMY_CREATURE_RED_DRAGON ||
                otherArmy->m_monsterType == ARMY_CREATURE_BLACK_DRAGON ||
                otherArmy->m_monsterType == ARMY_CREATURE_BONE_DRAGON) {
                dragonCount++;
                if (target->OtherArmyAdjacent(otherArmy->m_side,
                                               otherArmy->m_index))
                    adjacent = 1;
            }
        }
        float dragonMod;
        if (adjacent)
            dragonMod = COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER;
        else
            dragonMod = static_cast<float>(
                dragonCount / m_armyCount[1 - target->m_side]);
        effect = static_cast<int>(COMBAT_SPELL_AI_DRAGON_SLAYER_MODIFIER *
                                  dragonMod);
        break;
    }
    case ARMY_SPELL_INFLUENCE_SHIELD: {
        int shooterCount = 0;
        int i;
        for (i = 0; i < m_armyCount[1 - target->m_side]; i++) {
            otherArmy = &m_armies[target->m_side][i];
            if (otherArmy->m_monster.flags.all & MONSTER_FLAGS_SHOOTER)
                shooterCount++;
        }
        float shooterMod = static_cast<float>(
            shooterCount / m_armyCount[1 - target->m_side]);
        if (target->m_side == 0 && m_inCastleCombat) {
            shooterMod = static_cast<float>(
                shooterMod + COMBAT_SPELL_AI_SIEGE_SHIELD_BONUS);
            if (shooterMod > COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER)
                shooterMod = COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER;
        }
        effect = static_cast<int>(COMBAT_SPELL_AI_SHIELD_MODIFIER *
                                  shooterMod);
        break;
    }
    default:
        effect = 0;
        break;
    }

    effect = static_cast<int>(effect * workChance);
    if ((target->m_spellInfluence[ARMY_SPELL_INFLUENCE_BERSERK] ||
         target->m_spellInfluence[ARMY_SPELL_INFLUENCE_HYPNOTIZE]) &&
        influence != ARMY_SPELL_INFLUENCE_ANTI_MAGIC)
        effect = 0;
    return effect;
}

VA(0x00488607, 0x66)
void combatManager::ClearEffects(void)
{
    int side;
    int index;
    for (side = 0; side < COMBAT_SIDE_COUNT; side++) {
        for (index = 0; index < COMBAT_ARMY_SLOT_COUNT; index++)
            gArmyEffected[side][index] = 0;
    }
}

VA(0x0048866d, 0x40)
void combatManager::NextPos(int *hex)
{
    if ((*hex + 2) % 13 == 0)
        *hex += 3;
    else
        (*hex)++;
}

VA(0x004886ad, 0xa1)
int combatManager::FirstArmy(int startHex, int side, int *hex)
{
    while (startHex <= COMBAT_SPELL_AI_LAST_HEX) {
        if (m_hexCells[startHex].m_occupantSide == side ||
            (side == COMBAT_SPELL_AI_ANY_SIDE &&
             m_hexCells[startHex].m_occupantSide >= 0)) {
            *hex = startHex;
            return 0;
        }
        NextPos(&startHex);
    }
    *hex = -1;
    return 1;
}

VA(0x0048874e, 0x73)
int combatManager::FirstResurrectable(int startHex, int *hex, int spell)
{
    while (startHex <= COMBAT_SPELL_AI_LAST_HEX) {
        if (FindResurrectArmyIndex(m_currentSide, spell, startHex) != -1) {
            *hex = startHex;
            return 0;
        }
        NextPos(&startHex);
    }
    *hex = -1;
    return 1;
}

VA(0x004887c1, 0x421)
void combatManager::EffectSpellCure(int *effect, int targetSide,
                                    int targetIndex, int cure)
{
    *effect = 0;
    int done = 0;
    int side;
    if (targetSide == COMBAT_SPELL_AI_ANY_SIDE)
        side = m_currentSide;
    else
        side = targetSide;

    while (!done) {
        int positiveEffect = 0;
        int negativeEffect = 0;
        int index;
        for (index = 0; index < COMBAT_ARMY_SLOT_COUNT; index++) {
            if ((targetIndex == -1 || targetIndex == index) &&
                m_armies[side][index].m_monsterType >= 0 &&
                m_armies[side][index].m_quantity > 0) {
                army *target = &m_armies[side][index];
                if (cure == 1) {
                    int curePoints = m_spellPower[m_currentSide] *
                                     COMBAT_SPELL_AI_CURE_POINTS_PER_POWER;
                    if (curePoints > target->m_hitPointsLost)
                        curePoints = target->m_hitPointsLost;
                    positiveEffect = static_cast<int>(
                        positiveEffect +
                        static_cast<float>(
                            gMonsterDatabase[target->m_monsterType].fightValue) *
                            curePoints * COMBAT_SPELL_AI_CURE_VALUE_MODIFIER /
                            target->m_monster.hitPoints);
                }

                int fullQuantity =
                    (target->m_monster.flags.all &
                     MONSTER_FLAGS_FULL_AI_QUANTITY) != 0;
                int armyValue =
                    gMonsterDatabase[target->m_monsterType].fightValue *
                    target->m_quantity;
                if (target->m_monster.flags.all & MONSTER_FLAGS_UNDEAD) {
                    negativeEffect = armyValue;
                    continue;
                }

                int influence;
                for (influence = 0; influence < ARMY_SPELL_INFLUENCE_COUNT;
                     influence++) {
                    if (!target->m_spellInfluence[influence])
                        continue;
                    switch (influence) {
                    case ARMY_SPELL_INFLUENCE_HASTE:
                    case ARMY_SPELL_INFLUENCE_BLESS:
                    case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER:
                    case ARMY_SPELL_INFLUENCE_BLOODLUST:
                    case ARMY_SPELL_INFLUENCE_SHIELD:
                    case ARMY_SPELL_INFLUENCE_STONESKIN:
                    case ARMY_SPELL_INFLUENCE_STEELSKIN:
                        positiveEffect = static_cast<int>(
                            positiveEffect +
                            RawEffectSpellInfluence(target, influence) *
                                gfCancelDurationMods[
                                    target->m_spellInfluence[influence] +
                                                fullQuantity >=
                                            COMBAT_SPELL_AI_MAX_DURATION
                                        ? COMBAT_SPELL_AI_MAX_DURATION
                                        : target->m_spellInfluence[influence] +
                                              fullQuantity]);
                        break;
                    case ARMY_SPELL_INFLUENCE_SLOW:
                    case ARMY_SPELL_INFLUENCE_BLIND:
                    case ARMY_SPELL_INFLUENCE_CURSE:
                    case ARMY_SPELL_INFLUENCE_BERSERK:
                    case ARMY_SPELL_INFLUENCE_PARALYZE:
                    case ARMY_SPELL_INFLUENCE_HYPNOTIZE:
                    case ARMY_SPELL_INFLUENCE_PETRIFIED:
                        negativeEffect = static_cast<int>(
                            negativeEffect +
                            RawEffectSpellInfluence(target, influence) *
                                gfCancelDurationMods[
                                    target->m_spellInfluence[influence] +
                                                fullQuantity >=
                                            COMBAT_SPELL_AI_MAX_DURATION
                                        ? COMBAT_SPELL_AI_MAX_DURATION
                                        : target->m_spellInfluence[influence] +
                                              fullQuantity]);
                        break;
                    case ARMY_SPELL_INFLUENCE_ANTI_MAGIC:
                        break;
                    }
                }
            }
        }
        if (cure == 1)
            positiveEffect = 0;
        if (m_currentSide == side)
            *effect += -negativeEffect - positiveEffect;
        else
            *effect += positiveEffect + negativeEffect;
        if (targetSide == COMBAT_SPELL_AI_ANY_SIDE && m_currentSide == side)
            side = 1 - m_currentSide;
        else
            done = 1;
    }
}

VA(0x00488be2, 0x176)
void combatManager::EffectSpellResurrect(int *effect, int hex, int spell)
{
    int resurrectPower = m_spellPower[m_currentSide] *
                         COMBAT_SPELL_AI_RESURRECT_POINTS_PER_POWER;
    if (m_heroes[m_currentSide] != 0 &&
        m_heroes[m_currentSide]->HasArtifact(SPELL_ARTIFACT_ANKH))
        resurrectPower <<= 1;

    int armyIndex = FindResurrectArmyIndex(m_currentSide, spell, hex);
    army *target = &m_armies[m_currentSide][armyIndex];
    int quantity = resurrectPower / target->m_monster.hitPoints;
    if (target->m_quantity + quantity > target->m_initialQuantity)
        quantity = target->m_initialQuantity - target->m_quantity;
    *effect = gMonsterDatabase[target->m_monsterType].fightValue * quantity;
    *effect = static_cast<int>(*effect * target->SpellCastWorkChance(spell));
    if (spell == SPELL_RESURRECT)
        *effect = static_cast<int>(
            *effect * COMBAT_SPELL_AI_RESURRECT_VALUE_MODIFIER);
}

VA(0x00488d58, 0xcc9)
void combatManager::EffectSpellDamage(int *effect, int spell, int targetHex)
{
    int damagePerPower;
    switch (spell) {
    case SPELL_ARMAGEDDON:
        damagePerPower = COMBAT_SPELL_AI_ARMAGEDDON_DAMAGE_PER_POWER;
        break;
    case SPELL_HOLY_WORD:
        damagePerPower = COMBAT_SPELL_AI_HOLY_WORD_DAMAGE_PER_POWER;
        break;
    case SPELL_HOLY_SHOUT:
        damagePerPower = COMBAT_SPELL_AI_HOLY_SHOUT_DAMAGE_PER_POWER;
        break;
    case SPELL_DEATH_RIPPLE:
        damagePerPower = COMBAT_SPELL_AI_DEATH_RIPPLE_DAMAGE_PER_POWER;
        break;
    case SPELL_DEATH_WAVE:
        damagePerPower = COMBAT_SPELL_AI_DEATH_WAVE_DAMAGE_PER_POWER;
        break;
    case SPELL_ELEMENTAL_STORM:
        damagePerPower = COMBAT_SPELL_AI_ELEMENTAL_STORM_DAMAGE_PER_POWER;
        break;
    case SPELL_FIREBALL:
        damagePerPower = COMBAT_SPELL_AI_FIRE_DAMAGE_PER_POWER;
        break;
    case SPELL_FIREBLAST:
        damagePerPower = COMBAT_SPELL_AI_FIRE_DAMAGE_PER_POWER;
        break;
    case SPELL_METEOR_SHOWER:
        damagePerPower = COMBAT_SPELL_AI_ELEMENTAL_STORM_DAMAGE_PER_POWER;
        break;
    case SPELL_LIGHTNING_BOLT:
        damagePerPower = COMBAT_SPELL_AI_LIGHTNING_DAMAGE_PER_POWER;
        break;
    case SPELL_MAGIC_ARROW:
        damagePerPower = COMBAT_SPELL_AI_MAGIC_ARROW_DAMAGE_PER_POWER;
        break;
    case SPELL_CHAIN_LIGHTNING:
        damagePerPower = COMBAT_SPELL_AI_CHAIN_LIGHTNING_DAMAGE_PER_POWER;
        break;
    case SPELL_COLD_RAY:
        damagePerPower = COMBAT_SPELL_AI_COLD_RAY_DAMAGE_PER_POWER;
        break;
    case SPELL_COLD_RING:
        damagePerPower = COMBAT_SPELL_AI_COLD_RING_DAMAGE_PER_POWER;
        break;
    default:
        damagePerPower = 0;
        break;
    }

    int damage = m_spellPower[m_currentSide] * damagePerPower;
    int currentHex = 0;
    int step = 0;
    unsigned int done = 0;
    int killedValue[2];
    int damageValue[5];
    int disruptingRayValue = 0;
    army *target = 0;
    if (m_hexCells[targetHex].m_occupantIndex >= 0)
        target = &m_armies[m_hexCells[targetHex].m_occupantSide]
                            [m_hexCells[targetHex].m_occupantIndex];

    damageValue[2] = 0;
    int side;
    for (side = 0; side < COMBAT_SIDE_COUNT; side++) {
        damageValue[side] = 0;
        damageValue[side + 3] = 0;
        killedValue[side] = 0;
    }
    ClearEffects();

    do {
        if (done) {
            if (damageValue[0] < m_armyCount[0] &&
                damageValue[1] < m_armyCount[1]) {
                *effect = damageValue[4 - m_currentSide] -
                          damageValue[m_currentSide + 3] + disruptingRayValue;
            } else if (killedValue[m_currentSide] < 1) {
                *effect = COMBAT_SPELL_AI_DECISIVE_EFFECT -
                          gsSpellInfo[spell].aiValue +
                          disruptingRayValue;
            } else {
                *effect = killedValue[1 - m_currentSide] -
                          killedValue[m_currentSide] + disruptingRayValue;
            }
            if (m_currentSide == 0 && m_inCastleCombat)
                *effect = static_cast<int>(
                    *effect * COMBAT_SPELL_AI_CASTLE_EFFECT_MODIFIER);
            return;
        }

        switch (spell) {
        case SPELL_LIGHTNING_BOLT:
        case SPELL_MAGIC_ARROW:
        case SPELL_COLD_RAY:
        case SPELL_DISRUPTING_RAY:
            if (currentHex == targetHex)
                done = 1;
            else
                currentHex = targetHex;
            break;
        case SPELL_CHAIN_LIGHTNING:
            if (currentHex == 0) {
                currentHex = targetHex;
            } else {
                damage >>= 1;
                currentHex = GetNextChainLightningTarget(target, 0);
            }
            step++;
            if (step > 4 || currentHex == -1)
                done = 1;
            break;
        case SPELL_TELEPORT:
        case SPELL_CURE:
        case SPELL_MASS_CURE:
        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_HASTE:
        case SPELL_MASS_HASTE:
        case SPELL_SLOW:
        case SPELL_MASS_SLOW:
        case SPELL_BLIND:
        case SPELL_BLESS:
        case SPELL_MASS_BLESS:
        case SPELL_STONE_SKIN:
        case SPELL_STEEL_SKIN:
        case SPELL_CURSE:
        case SPELL_MASS_CURSE:
        case SPELL_ANTI_MAGIC:
        case SPELL_DISPEL:
        case SPELL_MASS_DISPEL:
        case SPELL_BERSERKER:
        case SPELL_PARALYZE:
        case SPELL_HYPNOTIZE:
            break;
        case SPELL_HOLY_WORD:
        case SPELL_HOLY_SHOUT:
        case SPELL_ARMAGEDDON:
        case SPELL_ELEMENTAL_STORM:
        case SPELL_DEATH_RIPPLE:
        case SPELL_DEATH_WAVE:
            NextPos(&currentHex);
            if (currentHex < COMBAT_SPELL_AI_LAST_HEX + 1)
                done = 0;
            else
                done = 1;
            done = currentHex >= COMBAT_SPELL_AI_LAST_HEX + 1;
            break;
        case SPELL_COLD_RING:
            if (step == 0)
                step++;
        case SPELL_FIREBALL:
        case SPELL_FIREBLAST:
        case SPELL_METEOR_SHOWER:
            if (((step < 7) || spell == SPELL_FIREBLAST) && step < 19) {
                if (step == 0)
                    currentHex = targetHex;
                if (step > 0 && step < 7)
                    currentHex = GetAdjacentCellIndexNoArmy(targetHex, step - 1);
                if (step > 6 && step < 13) {
                    currentHex = GetAdjacentCellIndexNoArmy(targetHex, step - 7);
                    currentHex = GetAdjacentCellIndexNoArmy(currentHex, step - 7);
                }
                if (step == 13)
                    currentHex = targetHex - 26;
                if (step == 14)
                    currentHex = targetHex + 26;
                if (step == 15) {
                    currentHex = GetAdjacentCellIndexNoArmy(targetHex, 1);
                    currentHex = GetAdjacentCellIndexNoArmy(currentHex, 0);
                }
                if (step == 16) {
                    currentHex = GetAdjacentCellIndexNoArmy(targetHex, 1);
                    currentHex = GetAdjacentCellIndexNoArmy(currentHex, 2);
                }
                if (step == 17) {
                    currentHex = GetAdjacentCellIndexNoArmy(targetHex, 4);
                    currentHex = GetAdjacentCellIndexNoArmy(currentHex, 5);
                }
                if (step == 18) {
                    currentHex = GetAdjacentCellIndexNoArmy(targetHex, 4);
                    currentHex = GetAdjacentCellIndexNoArmy(currentHex, 3);
                }
                step++;
            } else {
                done = 1;
            }
        }

        if (!done && currentHex >= 0 &&
            currentHex < COMBAT_SPELL_AI_LAST_HEX + 1 &&
            m_hexCells[currentHex].m_occupantIndex >= 0 &&
            m_hexCells[currentHex].m_occupantSide >= 0) {
            target = &m_armies[m_hexCells[currentHex].m_occupantSide]
                                [m_hexCells[currentHex].m_occupantIndex];
            if (!gArmyEffected[m_hexCells[currentHex].m_occupantSide]
                               [m_hexCells[currentHex].m_occupantIndex]) {
                gArmyEffected[m_hexCells[currentHex].m_occupantSide]
                              [m_hexCells[currentHex].m_occupantIndex] = 1;
                float workChance = target->SpellCastWorkChance(spell);
                if (workChance > 0.0) {
                    long spellDamage = static_cast<long>(damage * workChance);
                    int monster = target->m_monsterType;
                    switch (spell) {
                    case SPELL_FIREBALL:
                    case SPELL_FIREBLAST:
                        if (monster == SPELL_MONSTER_IRON_GOLEM ||
                            monster == SPELL_MONSTER_STEEL_GOLEM)
                            spellDamage = static_cast<long>(
                                spellDamage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                        if (monster == SPELL_MONSTER_WATER_ELEMENTAL)
                            spellDamage <<= 1;
                        break;
                    case SPELL_LIGHTNING_BOLT:
                    case SPELL_CHAIN_LIGHTNING:
                    case SPELL_ELEMENTAL_STORM:
                        if (monster == SPELL_MONSTER_IRON_GOLEM ||
                            monster == SPELL_MONSTER_STEEL_GOLEM)
                            spellDamage = static_cast<long>(
                                spellDamage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                        if (monster == SPELL_MONSTER_AIR_ELEMENTAL)
                            spellDamage <<= 1;
                        break;
                    case SPELL_ARMAGEDDON:
                        if (monster == SPELL_MONSTER_IRON_GOLEM ||
                            monster == SPELL_MONSTER_STEEL_GOLEM)
                            spellDamage = static_cast<long>(
                                spellDamage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                        break;
                    case SPELL_METEOR_SHOWER:
                        if (monster == SPELL_MONSTER_IRON_GOLEM ||
                            monster == SPELL_MONSTER_STEEL_GOLEM)
                            spellDamage = static_cast<long>(
                                spellDamage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                        if (monster == SPELL_MONSTER_EARTH_ELEMENTAL)
                            spellDamage <<= 1;
                        break;
                    case SPELL_COLD_RAY:
                    case SPELL_COLD_RING:
                        if (monster == SPELL_MONSTER_IRON_GOLEM ||
                            monster == SPELL_MONSTER_STEEL_GOLEM)
                            spellDamage = static_cast<long>(
                                spellDamage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                        if (monster == SPELL_MONSTER_FIRE_ELEMENTAL)
                            spellDamage <<= 1;
                        break;
                    }

                    ModifyDamageForArtifacts(
                        &spellDamage, spell, m_heroes[m_currentSide],
                        m_heroes[target->m_side]);
                    if ((target->m_monster.flags.all & MONSTER_FLAGS_UNDEAD) &&
                        spellDamage != 0)
                        spellDamage = COMBAT_SPELL_AI_MIRROR_LETHAL_DAMAGE;

                    damageValue[2] =
                        spellDamage / target->m_monster.hitPoints;
                    int remainder = spellDamage % target->m_monster.hitPoints;
                    if (target->m_monster.hitPoints <=
                        target->m_hitPointsLost + remainder) {
                        damageValue[2] =
                            spellDamage / target->m_monster.hitPoints + 1;
                        remainder -= target->m_monster.hitPoints -
                                     target->m_hitPointsLost;
                    }
                    if (target->m_quantity <= damageValue[2]) {
                        damageValue[2] = target->m_quantity;
                        remainder = 0;
                        damageValue[m_hexCells[currentHex].m_occupantSide]++;
                    }

                    damageValue[m_hexCells[currentHex].m_occupantSide + 3] =
                        static_cast<int>(
                        damageValue[m_hexCells[currentHex].m_occupantSide + 3] +
                        (remainder * COMBAT_SPELL_AI_PARTIAL_DAMAGE_MODIFIER +
                         target->m_monster.hitPoints * damageValue[2]) *
                            gMonsterDatabase[target->m_monsterType].fightValue /
                            target->m_monster.hitPoints);
                    killedValue[m_hexCells[currentHex].m_occupantSide] +=
                        gMonsterDatabase[target->m_monsterType].fightValue *
                        target->m_monster.hitPoints * damageValue[2] /
                        target->m_monster.hitPoints;
                    if (target->m_monster.flags.all & MONSTER_FLAGS_UNDEAD) {
                        killedValue[m_hexCells[currentHex].m_occupantSide] /= 2;
                        damageValue[m_hexCells[currentHex].m_occupantSide + 3] /= 2;
                    }

                    if (spell == SPELL_DISRUPTING_RAY) {
                        int newDefense = target->m_monster.defense -
                                         SPELL_DISRUPTING_RAY_DEFENSE_REDUCTION;
                        if (newDefense < SPELL_MINIMUM_DEFENSE)
                            newDefense = SPELL_MINIMUM_DEFENSE;
                        disruptingRayValue = static_cast<int>(
                            disruptingRayValue +
                            gMonsterDatabase[target->m_monsterType].fightValue *
                                (target->m_monster.defense - newDefense) *
                                (target->m_quantity - damageValue[2]) *
                                COMBAT_SPELL_AI_DISRUPTING_RAY_MODIFIER);
                    }
                }
            }
        }
    } while (1);
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f80b8) float gfDurationMods[12] = {
    0.0f, 0.33f, 0.55f, 0.72f, 0.85f, 0.95f,
    1.03f, 1.08f, 1.12f, 1.15f, 1.18f, 0.0f
};
DATA(0x004f80e8) float gfCancelDurationMods[11] = {
    0.0f, 0.5f, 0.65f, 0.78f, 0.85f, 0.95f,
    1.03f, 1.08f, 1.12f, 1.15f, 1.18f
};
DATA(0x005284b4) int giCurrSpellGroup;
