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
// @semantic
// Exact 0x24 frame/live slots and 11/11 external relocations. The only raw
// residual is +0x21c/+0x21f/+0x221: ours loads effectScore then emits JGE,
// while retail loads bestEffectWork then emits JLE. Direct, commuted,
// negated, empty-if/else, continue, and both bestEffectWork OD_STEER(...) spellings
// were tried; revisit after later SPELLAI TU-state changes.
VA(0x004867c0, 0x279)
i32 combatManager::DoSpellAI(i32 side, i32 restricted) {
    i32 bestSpellChoice;
    i32 effectScore;
    i32 spellIndex;
    i32 bestHexWork;
    i32 candidateHex;
    i32 bestEffectWork;
    i32 manaRatioResult;

    bestEffectWork = 0;
    bestSpellChoice = -1;
    bestHexWork = -1;

    if (m_heroes[side] == 0)
        return 0;

    for (spellIndex = 0; spellIndex < COMBAT_SIDE_COUNT; spellIndex++) {
        if (m_heroes[spellIndex] != 0
            && m_heroes[spellIndex]->HasArtifact(ARTIFACT_SPHERE_NEGATION))
            return 0;
    }

    for (spellIndex = 0; spellIndex < IDX(SPELL_COUNT); spellIndex++) {
        if (m_heroes[side]->HasSpell(SpellType(spellIndex))
            && (gsSpellInfo[spellIndex].attributes & SPELL_ATTRIBUTE_COMBAT)
            && GetManaCost(SpellType(spellIndex), m_heroes[side]) <= m_heroes[side]->m_spellPoints) {
            if (restricted && spellIndex != IDX(SPELL_FIREBALL) && spellIndex != IDX(SPELL_FIREBLAST)
                && spellIndex != IDX(SPELL_LIGHTNING_BOLT) && spellIndex != IDX(SPELL_CHAIN_LIGHTNING)
                && spellIndex != IDX(SPELL_HOLY_WORD) && spellIndex != IDX(SPELL_HOLY_SHOUT)
                && spellIndex != IDX(SPELL_MAGIC_ARROW) && spellIndex != IDX(SPELL_ARMAGEDDON)
                && spellIndex != IDX(SPELL_ELEMENTAL_STORM) && spellIndex != IDX(SPELL_METEOR_SHOWER)
                && spellIndex != IDX(SPELL_COLD_RAY) && spellIndex != IDX(SPELL_COLD_RING)
                && spellIndex != IDX(SPELL_DEATH_RIPPLE) && spellIndex != IDX(SPELL_DEATH_WAVE))
                continue;
            DetermineEffectOfSpell(SpellType(spellIndex), &effectScore, &candidateHex);

            manaRatioResult =
                m_heroes[side]->m_spellPoints / GetManaCost(SpellType(spellIndex), m_heroes[side]);
            if (manaRatioResult > COMBAT_SPELL_AI_MAX_MANA_RATIO)
                manaRatioResult = COMBAT_SPELL_AI_MAX_MANA_RATIO;
            effectScore = static_cast<i32>(effectScore * gfSpellCastableCombatMod[manaRatioResult]);

            if (effectScore > bestEffectWork) {
                bestEffectWork = effectScore;
                bestSpellChoice = spellIndex;
                bestHexWork = candidateHex;
            }
        }
    }

    if (bestEffectWork > 0) {
        giNextAction = COMBAT_SPELL_AI_CAST_ACTION;
        giNextActionExtra = bestSpellChoice;
        giNextActionGridIndex = bestHexWork;
        return 1;
    }
    return 0;
}

// @semantic
// Exact 0xd4 frame and all source roles are recovered; all 156/156 external
// relocation occurrences agree. Named local allocation order still differs
// (for example, ours -0x4/-0x24/-0x28 map to retail
// -0x34/-0x4/-0x18). The immediate 1.0f spelling now matches the opening
// initializer. The first normalized report boundary is the local first switch
// label after 17 instructions, with retail-only continuation/table output
// thereafter. Revisit slot naming and the local-table residual in the
// byte-last-mile phase.
VA(0x00486a39, 0x1155)
void combatManager::DetermineEffectOfSpell(SpellType spell, i32* bestEffect, i32* bestHex) {
    i32 doneResult;
    i32 fullQuantityFlag;
    i32 hexIndex;
    i32 indexWork;
    i32 targetModeBySide[COMBAT_SIDE_COUNT];
    float durationMod;
    i32 totalEffect;
    i32 hasMindEffect;
    army* targetCreature;
    i32 effect;
    i32 hasDamageReductionResult;
    i32 side;
    i32 spellPowerWork;
    i32 wallsDamagedTotal;
    i32 cureEffectWork;

    doneResult = 0;
    side = 0;
    hexIndex = COMBAT_SPELL_AI_FIRST_HEX;
    durationMod = COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE;
    fullQuantityFlag = 1;
    totalEffect = 0;
    targetCreature = 0;
    *bestEffect = 0;

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
            targetModeBySide[0] = COMBAT_SPELL_AI_GLOBAL;
            break;
        case SPELL_FIREBALL:
        case SPELL_FIREBLAST:
        case SPELL_METEOR_SHOWER:
        case SPELL_COLD_RING:
            targetModeBySide[0] = COMBAT_SPELL_AI_AREA;
            break;
        case SPELL_MASS_HASTE:
        case SPELL_MASS_BLESS:
        case SPELL_MASS_SHIELD:
            targetModeBySide[0] = COMBAT_SPELL_AI_SUM_FRIENDLY;
            side = m_currentSide;
            break;
        case SPELL_MASS_SLOW:
        case SPELL_MASS_CURSE:
            targetModeBySide[0] = COMBAT_SPELL_AI_SUM_ENEMY;
            side = 1 - m_currentSide;
            break;
        case SPELL_DISPEL:
            targetModeBySide[0] = COMBAT_SPELL_AI_ANY_ARMY;
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
            targetModeBySide[0] = COMBAT_SPELL_AI_FRIENDLY;
            side = m_currentSide;
            break;
        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_ANIMATE_DEAD:
            targetModeBySide[0] = COMBAT_SPELL_AI_RESURRECT;
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
            targetModeBySide[0] = COMBAT_SPELL_AI_ENEMY;
            side = 1 - m_currentSide;
            break;
        default:
            *bestEffect = 0;
            return;
    }

    if (targetModeBySide[0] == COMBAT_SPELL_AI_RESURRECT)
        doneResult = FirstResurrectable(COMBAT_SPELL_AI_FIRST_HEX, &hexIndex, IDX(spell));
    if (targetModeBySide[0] == COMBAT_SPELL_AI_FRIENDLY
        || targetModeBySide[0] == COMBAT_SPELL_AI_ENEMY
        || targetModeBySide[0] == COMBAT_SPELL_AI_SUM_ENEMY
        || targetModeBySide[0] == COMBAT_SPELL_AI_SUM_FRIENDLY
        || targetModeBySide[0] == COMBAT_SPELL_AI_ANY_ARMY)
        doneResult = FirstArmy(COMBAT_SPELL_AI_FIRST_HEX, side, &hexIndex);

    while (!doneResult) {
        hasMindEffect = 0;
        hasDamageReductionResult = 0;
        effect = 0;

        if (m_hexCells[hexIndex].m_occupantIndex < 0 || m_hexCells[hexIndex].m_occupantSide < 0) {
            targetCreature = 0;
        } else {
            targetCreature = &m_armies[m_hexCells[hexIndex].m_occupantSide]
                                      [m_hexCells[hexIndex].m_occupantIndex];
            giCurrSpellGroup = m_hexCells[hexIndex].m_occupantSide;
            fullQuantityFlag =
                HAS(targetCreature->m_monster.flags.all, MONSTER_FLAGS_FULL_AI_QUANTITY) != 0;

            spellPowerWork = m_spellPower[m_currentSide];
            if (m_heroes[m_currentSide]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS))
                spellPowerWork += SPELL_HOURGLASS_POWER_BONUS;
            if (m_heroes[m_currentSide]->HasArtifact(ARTIFACT_WIZARD_HAT))
                spellPowerWork += SPELL_WIZARD_HAT_POWER_BONUS;

            durationMod = gfDurationMods
                [spellPowerWork - fullQuantityFlag >= COMBAT_SPELL_AI_MAX_DURATION
                     ? COMBAT_SPELL_AI_MAX_DURATION
                     : spellPowerWork - fullQuantityFlag];

            if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]
                || targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)])
                hasMindEffect = 1;
            if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)]
                || targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)]
                || targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)])
                hasDamageReductionResult = 1;
        }

        switch (spell) {
            case SPELL_CURE:
                EffectSpellCure(&effect, m_currentSide, hexIndex, 1);
                break;
            case SPELL_MASS_CURE:
                EffectSpellCure(&effect, m_currentSide, -1, 1);
                break;
            case SPELL_DISPEL:
                EffectSpellCure(&effect, targetCreature->m_side, targetCreature->m_index, 0);
                break;
            case SPELL_MASS_DISPEL:
                EffectSpellCure(&effect, COMBAT_SPELL_AI_ANY_SIDE, -1, 0);
                break;
            case SPELL_RESURRECT:
            case SPELL_TRUE_RESURRECT:
            case SPELL_ANIMATE_DEAD:
                if (hasMindEffect) {
                } else {
                    EffectSpellResurrect(&effect, hexIndex, spell);
                }
                break;
            case SPELL_MIRROR_IMAGE:
            case SPELL_SUMMON_EARTH_ELEMENTAL:
            case SPELL_SUMMON_AIR_ELEMENTAL:
            case SPELL_SUMMON_FIRE_ELEMENTAL:
            case SPELL_SUMMON_WATER_ELEMENTAL:
                effect = EffectSpellCreateCreature(hexIndex, spell);
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
                    EffectSpellDamage(&effect, spell, hexIndex);
                    if (hasDamageReductionResult)
                        effect = static_cast<i32>(effect * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER);
                }
                break;
            case SPELL_HASTE:
            case SPELL_MASS_HASTE:
                if (hasDamageReductionResult) {
                } else {
                    if (hasMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HASTE)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(targetCreature, IDX(ARMY_SPELL_INFLUENCE_HASTE))
                                * durationMod
                            );
                            if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_SLOW)]) {
                                effect = static_cast<i32>(
                                    RawEffectSpellInfluence(
                                        targetCreature,
                                        IDX(ARMY_SPELL_INFLUENCE_SLOW)
                                    )
                                        * gfCancelDurationMods
                                            [targetCreature->m_spellInfluence[IDX(
                                                 ARMY_SPELL_INFLUENCE_SLOW
                                             )] + fullQuantityFlag
                                                     >= COMBAT_SPELL_AI_MAX_DURATION
                                                 ? COMBAT_SPELL_AI_MAX_DURATION
                                                 : targetCreature->m_spellInfluence[IDX(
                                                       ARMY_SPELL_INFLUENCE_SLOW
                                                   )] + fullQuantityFlag]
                                    - effect
                                );
                            }
                        }
                    }
                }
                break;
            case SPELL_BERSERKER:
                if (hasDamageReductionResult) {
                } else {
                    if (hasMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    IDX(ARMY_SPELL_INFLUENCE_BERSERK)
                                )
                                * durationMod
                            );
                            if (targetCreature
                                    ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]) {
                                effect = static_cast<i32>(
                                    RawEffectSpellInfluence(
                                        targetCreature,
                                        IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)
                                    )
                                        * gfCancelDurationMods
                                            [targetCreature->m_spellInfluence[IDX(
                                                 ARMY_SPELL_INFLUENCE_HYPNOTIZE
                                             )] + fullQuantityFlag
                                                     >= COMBAT_SPELL_AI_MAX_DURATION
                                                 ? COMBAT_SPELL_AI_MAX_DURATION
                                                 : targetCreature->m_spellInfluence[IDX(
                                                       ARMY_SPELL_INFLUENCE_HYPNOTIZE
                                                   )] + fullQuantityFlag]
                                    - effect
                                );
                            }
                        }
                    }
                }
                break;
            case SPELL_HYPNOTIZE:
                if (hasDamageReductionResult) {
                } else {
                    if (hasMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)
                                )
                                * durationMod
                            );
                            if (targetCreature
                                    ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]) {
                                effect = static_cast<i32>(
                                    RawEffectSpellInfluence(
                                        targetCreature,
                                        IDX(ARMY_SPELL_INFLUENCE_BERSERK)
                                    )
                                        * gfCancelDurationMods
                                            [targetCreature->m_spellInfluence[IDX(
                                                 ARMY_SPELL_INFLUENCE_BERSERK
                                             )] + fullQuantityFlag
                                                     >= COMBAT_SPELL_AI_MAX_DURATION
                                                 ? COMBAT_SPELL_AI_MAX_DURATION
                                                 : targetCreature->m_spellInfluence[IDX(
                                                       ARMY_SPELL_INFLUENCE_BERSERK
                                                   )] + fullQuantityFlag]
                                    - effect
                                );
                            }
                        }
                    }
                }
                break;
            case SPELL_SLOW:
            case SPELL_MASS_SLOW:
                if (hasDamageReductionResult) {
                } else {
                    if (hasMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_SLOW)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(targetCreature, IDX(ARMY_SPELL_INFLUENCE_SLOW))
                                * durationMod
                            );
                            if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HASTE)]) {
                                effect = static_cast<i32>(
                                    RawEffectSpellInfluence(
                                        targetCreature,
                                        IDX(ARMY_SPELL_INFLUENCE_HASTE)
                                    )
                                        * gfCancelDurationMods
                                            [targetCreature->m_spellInfluence[IDX(
                                                 ARMY_SPELL_INFLUENCE_HASTE
                                             )] + fullQuantityFlag
                                                     >= COMBAT_SPELL_AI_MAX_DURATION
                                                 ? COMBAT_SPELL_AI_MAX_DURATION
                                                 : targetCreature->m_spellInfluence[IDX(
                                                       ARMY_SPELL_INFLUENCE_HASTE
                                                   )] + fullQuantityFlag]
                                    - effect
                                );
                            }
                        }
                    }
                }
                break;
            case SPELL_BLESS:
            case SPELL_MASS_BLESS:
                if (hasDamageReductionResult) {
                } else {
                    if (hasMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLESS)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(targetCreature, IDX(ARMY_SPELL_INFLUENCE_BLESS))
                                * durationMod
                            );
                            if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_CURSE)]) {
                                effect = static_cast<i32>(
                                    RawEffectSpellInfluence(
                                        targetCreature,
                                        IDX(ARMY_SPELL_INFLUENCE_CURSE)
                                    )
                                        * gfCancelDurationMods
                                            [targetCreature->m_spellInfluence[IDX(
                                                 ARMY_SPELL_INFLUENCE_CURSE
                                             )] + fullQuantityFlag
                                                     >= COMBAT_SPELL_AI_MAX_DURATION
                                                 ? COMBAT_SPELL_AI_MAX_DURATION
                                                 : targetCreature->m_spellInfluence[IDX(
                                                       ARMY_SPELL_INFLUENCE_CURSE
                                                   )] + fullQuantityFlag]
                                    - effect
                                );
                            }
                        }
                    }
                }
                break;
            case SPELL_CURSE:
            case SPELL_MASS_CURSE:
                if (hasDamageReductionResult) {
                } else {
                    if (hasMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_CURSE)]) {
                        } else {
                            effect = static_cast<i32>(
                                -RawEffectSpellInfluence(targetCreature, IDX(ARMY_SPELL_INFLUENCE_CURSE))
                                * durationMod
                            );
                            if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLESS)]) {
                                effect = static_cast<i32>(
                                    RawEffectSpellInfluence(
                                        targetCreature,
                                        IDX(ARMY_SPELL_INFLUENCE_BLESS)
                                    )
                                        * gfCancelDurationMods
                                            [targetCreature->m_spellInfluence[IDX(
                                                 ARMY_SPELL_INFLUENCE_BLESS
                                             )] + fullQuantityFlag
                                                     >= COMBAT_SPELL_AI_MAX_DURATION
                                                 ? COMBAT_SPELL_AI_MAX_DURATION
                                                 : targetCreature->m_spellInfluence[IDX(
                                                       ARMY_SPELL_INFLUENCE_BLESS
                                                   )] + fullQuantityFlag]
                                    - effect
                                );
                            }
                        }
                    }
                }
                break;
            case SPELL_ANTI_MAGIC:
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_ANTI_MAGIC)]) {
                } else {
                    if (spell == SPELL_ANTI_MAGIC && m_heroes[1 - m_currentSide] == 0) {
                        effect = 0;
                    } else {
                        effect = static_cast<i32>(
                            RawEffectSpellInfluence(targetCreature, IDX(ARMY_SPELL_INFLUENCE_ANTI_MAGIC))
                            * durationMod
                        );
                        EffectSpellCure(
                            &cureEffectWork,
                            targetCreature->m_side,
                            targetCreature->m_index,
                            0
                        );
                        effect += cureEffectWork;
                    }
                }
                break;
            case SPELL_STONE_SKIN:
                if (hasMindEffect) {
                } else {
                    if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STONESKIN)]) {
                    } else {
                        if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STEELSKIN)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    IDX(ARMY_SPELL_INFLUENCE_STONESKIN)
                                )
                                * durationMod
                            );
                            if (m_combatTowns[COMBAT_DEFENDER_SIDE] != 0
                                && targetCreature->m_side == COMBAT_ATTACKER_SIDE
                                && HAS(targetCreature->m_monster.flags.all, MONSTER_FLAGS_SHOOTER))
                                effect = static_cast<i32>(
                                    effect * COMBAT_SPELL_AI_SIEGE_SHOOTER_MODIFIER
                                );
                            if (hasDamageReductionResult)
                                effect = static_cast<i32>(
                                    effect * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER
                                );
                        }
                    }
                }
                break;
            case SPELL_STEEL_SKIN:
                if (hasMindEffect) {
                } else {
                    if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STONESKIN)]) {
                    } else {
                        if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STEELSKIN)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    IDX(ARMY_SPELL_INFLUENCE_STEELSKIN)
                                )
                                * durationMod
                            );
                            if (m_combatTowns[COMBAT_DEFENDER_SIDE] != 0
                                && targetCreature->m_side == COMBAT_ATTACKER_SIDE
                                && HAS(targetCreature->m_monster.flags.all, MONSTER_FLAGS_SHOOTER))
                                effect = static_cast<i32>(
                                    effect * COMBAT_SPELL_AI_SIEGE_SHOOTER_MODIFIER
                                );
                            if (hasDamageReductionResult)
                                effect = static_cast<i32>(
                                    effect * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER
                                );
                        }
                    }
                }
                break;
            case SPELL_BLOOD_LUST:
                if (hasDamageReductionResult) {
                } else {
                    if (hasMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLOODLUST)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    IDX(ARMY_SPELL_INFLUENCE_BLOODLUST)
                                )
                                * durationMod
                            );
                        }
                    }
                }
                break;
            case SPELL_SHIELD:
            case SPELL_MASS_SHIELD:
                if (hasMindEffect) {
                } else {
                    if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_SHIELD)]) {
                    } else {
                        effect = static_cast<i32>(
                            RawEffectSpellInfluence(targetCreature, IDX(ARMY_SPELL_INFLUENCE_SHIELD))
                            * durationMod
                        );
                        if (m_combatTowns[COMBAT_DEFENDER_SIDE] != 0
                            && targetCreature->m_side == COMBAT_ATTACKER_SIDE
                            && HAS(targetCreature->m_monster.flags.all, MONSTER_FLAGS_SHOOTER))
                            effect <<= 1;
                        if (hasDamageReductionResult)
                            effect =
                                static_cast<i32>(effect * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER);
                    }
                }
                break;
            case SPELL_BLIND:
                if (hasDamageReductionResult) {
                } else {
                    if (hasMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(targetCreature, IDX(ARMY_SPELL_INFLUENCE_BLIND))
                                * durationMod
                            );
                        }
                    }
                }
                break;
            case SPELL_PARALYZE:
                if (hasDamageReductionResult) {
                } else {
                    if (hasMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    IDX(ARMY_SPELL_INFLUENCE_PARALYZE)
                                )
                                * durationMod
                            );
                        }
                    }
                }
                break;
            case SPELL_DRAGON_SLAYER:
                if (hasDamageReductionResult) {
                } else {
                    if (hasMindEffect) {
                    } else {
                        if (targetCreature
                                ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    IDX(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER)
                                )
                                * durationMod
                            );
                        }
                    }
                }
                break;
            case SPELL_TELEPORT:
                effect = 0;
                break;
            case SPELL_EARTHQUAKE:
                if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0) {
                    wallsDamagedTotal = 0;
                    for (indexWork = 0; indexWork < COMBAT_SPELL_AI_EARTHQUAKE_WALL_COUNT;
                         indexWork++) {
                        if (m_wallStates[indexWork + COMBAT_SPELL_AI_EARTHQUAKE_WALL_FIRST]
                                == COMBAT_SPELL_AI_WALL_DAMAGED
                            || m_wallStates[indexWork + COMBAT_SPELL_AI_EARTHQUAKE_WALL_FIRST]
                                   == COMBAT_SPELL_AI_WALL_DESTROYED)
                            wallsDamagedTotal++;
                    }
                    if (wallsDamagedTotal == 0)
                        effect = COMBAT_SPELL_AI_EARTHQUAKE_NO_DAMAGE_SCORE;
                    else if (wallsDamagedTotal < COMBAT_SPELL_AI_EARTHQUAKE_WALL_COUNT)
                        effect = (COMBAT_SPELL_AI_EARTHQUAKE_WALL_COUNT - wallsDamagedTotal)
                                 * COMBAT_SPELL_AI_EARTHQUAKE_WALL_SCORE;
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

        switch (targetModeBySide[0]) {
            case IDX(COMBAT_SPELL_AI_GLOBAL):
            case IDX(COMBAT_SPELL_AI_AREA):
            case IDX(COMBAT_SPELL_AI_FRIENDLY):
            case IDX(COMBAT_SPELL_AI_ENEMY):
            case IDX(COMBAT_SPELL_AI_RESURRECT):
            case IDX(COMBAT_SPELL_AI_ANY_ARMY):
                totalEffect = effect;
                break;
            case IDX(COMBAT_SPELL_AI_SUM_FRIENDLY):
            case IDX(COMBAT_SPELL_AI_SUM_ENEMY):
                totalEffect += effect;
        }

        if (targetModeBySide[0] == COMBAT_SPELL_AI_SUM_FRIENDLY
            || targetModeBySide[0] == COMBAT_SPELL_AI_SUM_ENEMY || *bestEffect < totalEffect) {
            *bestEffect = totalEffect;
            *bestHex = hexIndex;
        }

        switch (targetModeBySide[0]) {
            case IDX(COMBAT_SPELL_AI_GLOBAL):
                doneResult = 1;
                break;
            case IDX(COMBAT_SPELL_AI_SUM_FRIENDLY):
            case IDX(COMBAT_SPELL_AI_SUM_ENEMY):
            case IDX(COMBAT_SPELL_AI_FRIENDLY):
            case IDX(COMBAT_SPELL_AI_ENEMY):
            case IDX(COMBAT_SPELL_AI_ANY_ARMY):
                doneResult = FirstArmy(hexIndex + 1, side, &hexIndex);
                break;
            case IDX(COMBAT_SPELL_AI_AREA):
                NextPos(&hexIndex);
                if (hexIndex > COMBAT_SPELL_AI_LAST_HEX)
                    doneResult = 1;
                break;
            case IDX(COMBAT_SPELL_AI_RESURRECT):
                doneResult = FirstResurrectable(hexIndex + 1, &hexIndex, IDX(spell));
        }
    }
}

// @semantic
// Exact 0x28 frame, named local slots, and 14/14 external relocations after
// restoring the immediate 1.0f initializer and semantic local names. The
// first 52 normalized instructions agree, then objdump stops ours at a local
// switch label; the explicit-range residual begins in local branch/table
// layout at +0xcb. The fight-value/power versus retail
// fight-value/creature-type expressions were tried; revisit with
// local-table-aware comparison in the byte-last-mile phase.
VA(0x00487b8e, 0x34c)
i32 combatManager::EffectSpellCreateCreature(i32 hex, SpellType spell) {
    float workChance = COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE;
    i32 spellPowerValue = m_spellPower[m_currentSide];

    if ((spell == SPELL_SUMMON_EARTH_ELEMENTAL || spell == SPELL_SUMMON_AIR_ELEMENTAL
         || spell == SPELL_SUMMON_FIRE_ELEMENTAL || spell == SPELL_SUMMON_WATER_ELEMENTAL)
        && m_heroes[m_currentSide] != 0
        && m_heroes[m_currentSide]->HasArtifact(ARTIFACT_BOOK_ELEMENTS))
        spellPowerValue <<= 1;

    if ((spell == SPELL_SUMMON_EARTH_ELEMENTAL || spell == SPELL_SUMMON_AIR_ELEMENTAL
         || spell == SPELL_SUMMON_FIRE_ELEMENTAL || spell == SPELL_SUMMON_WATER_ELEMENTAL)
        && !SpaceForElementalExists())
        return 0;

    CreatureType creatureType;
    switch (spell) {
        case SPELL_SUMMON_EARTH_ELEMENTAL:
            creatureType = CREATURE_EARTH_ELEMENTAL;
            break;
        case SPELL_SUMMON_AIR_ELEMENTAL:
            creatureType = CREATURE_AIR_ELEMENTAL;
            break;
        case SPELL_SUMMON_FIRE_ELEMENTAL:
            creatureType = CREATURE_FIRE_ELEMENTAL;
            break;
        case SPELL_SUMMON_WATER_ELEMENTAL:
            creatureType = CREATURE_WATER_ELEMENTAL;
            break;
        default:
            workChance = m_armies[m_hexCells[hex].m_occupantSide][m_hexCells[hex].m_occupantIndex]
                             .SpellCastWorkChance(SPELL_MIRROR_IMAGE);
            creatureType = m_armies[m_hexCells[hex].m_occupantSide][m_hexCells[hex].m_occupantIndex]
                               .m_monsterType;
            spellPowerValue =
                m_armies[m_hexCells[hex].m_occupantSide][m_hexCells[hex].m_occupantIndex].m_quantity
                * SPELL_DEFAULT_CREATURE_POWER;
            break;
    }

    i32 creatureEffect = gMonsterDatabase[IDX(creatureType)].fightValue * IDX(creatureType);
    if (spell == SPELL_MIRROR_IMAGE) {
        float mirrorMod;
        if (m_spellPower[m_currentSide] == COMBAT_SPELL_AI_MIRROR_POWER_ONE)
            mirrorMod = COMBAT_SPELL_AI_MIRROR_POWER_ONE_MODIFIER;
        else if (m_spellPower[m_currentSide] == COMBAT_SPELL_AI_MIRROR_POWER_TWO)
            mirrorMod = COMBAT_SPELL_AI_MIRROR_POWER_TWO_MODIFIER;
        else
            mirrorMod = COMBAT_SPELL_AI_MIRROR_DEFAULT_MODIFIER;
        creatureEffect = static_cast<i32>(creatureEffect * mirrorMod);
        if (gMonsterDatabase[IDX(creatureType)].flags.bytes.abilities & 4)
            creatureEffect =
                static_cast<i32>(creatureEffect * COMBAT_SPELL_AI_MIRROR_SHOOTER_MODIFIER);
    }
    return static_cast<i32>(creatureEffect * workChance);
}

// @semantic
// All source roles and external targets are accounted; named local allocation
// order differs, and retail's 0xb0 frame has one compiler-only word beyond
// ours' 0xac frame. The first normalized residual is pooled 0.0 identity
// ($T4772 versus const_000eb7f0), followed by the first local switch boundary;
// 65/62 COFF sites have no candidate-only external target. Float and double
// zero spellings were tried; revisit slot naming and the local-table residual
// in the byte-last-mile phase.
VA(0x00487eda, 0x72d)
i32 combatManager::RawEffectSpellInfluence(army* target, i32 influence) {
    i32 effect = 0;
    army* otherArmy = 0;
    float workChance = target->SpellCastWorkChance(SpellType(giSpellInfluenceToSpell[influence]));
    if (workChance <= COMBAT_SPELL_AI_ZERO_EFFECT)
        return 0;

    i32 armyValue = target->m_monster.fightValue * target->m_quantity;
    switch (influence) {
        case IDX(ARMY_SPELL_INFLUENCE_HASTE):
        case IDX(ARMY_SPELL_INFLUENCE_SLOW): {
            i32 newSpeed;
            if (influence == ARMY_SPELL_INFLUENCE_SLOW) {
                newSpeed = (target->m_monster.speed + 1) >> 1;
            } else {
                newSpeed = target->m_monster.speed + COMBAT_SPELL_AI_HASTE_SPEED_BONUS;
                if (HAS(target->m_monster.flags.all, MONSTER_FLAGS_FLYING))
                    return 0;
            }
            if (m_inCastleCombat && target->m_side == 1)
                return 0;
            if (HAS(target->m_monster.flags.all, MONSTER_FLAGS_SHOOTER))
                return 0;
            i32 attackMask = target->GetAttackMask(target->m_hex, 1, -1);
            if (attackMask != COMBAT_SPELL_AI_ALL_ATTACK_DIRECTIONS)
                return 0;

            i32 column = target->m_hex % ARMY_HEX_COLUMNS;
            i32 distance;
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
            if (HAS(target->m_monster.flags.all, MONSTER_FLAGS_FLYING))
                oldTurns = COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER;
            else
                oldTurns = static_cast<float>(distance) / target->m_monster.speed;
            float newTurns = static_cast<float>(distance) / newSpeed;
            if (newTurns > COMBAT_SPELL_AI_TURN_CAP)
                newTurns = COMBAT_SPELL_AI_TURN_CAP;
            if (oldTurns > COMBAT_SPELL_AI_TURN_CAP)
                oldTurns = COMBAT_SPELL_AI_TURN_CAP;
            effect =
                static_cast<i32>((oldTurns - newTurns) / COMBAT_SPELL_AI_TURN_DIVISOR * armyValue);
            break;
        }
        case IDX(ARMY_SPELL_INFLUENCE_BLESS):
        case IDX(ARMY_SPELL_INFLUENCE_CURSE): {
            float averageDamage = (static_cast<float>(target->m_monster.damageMin)
                                   + static_cast<float>(target->m_monster.damageMax))
                                  * COMBAT_SPELL_AI_AVERAGE_DAMAGE_MODIFIER;
            float damageEffect = static_cast<float>(
                (target->m_monster.damageMax - averageDamage) / averageDamage * armyValue
                * COMBAT_SPELL_AI_BLESS_CURSE_MODIFIER
            );
            effect = static_cast<i32>(
                influence == ARMY_SPELL_INFLUENCE_BLESS ? damageEffect : -damageEffect
            );
            break;
        }
        case IDX(ARMY_SPELL_INFLUENCE_BLIND):
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_BLIND_MODIFIER);
            break;
        case IDX(ARMY_SPELL_INFLUENCE_BERSERK):
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_BERSERK_MODIFIER);
            break;
        case IDX(ARMY_SPELL_INFLUENCE_PARALYZE):
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_PARALYZE_MODIFIER);
            break;
        case IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE):
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_HYPNOTIZE_MODIFIER);
            break;
        case IDX(ARMY_SPELL_INFLUENCE_BLOODLUST):
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_BLOODLUST_MODIFIER);
            break;
        case IDX(ARMY_SPELL_INFLUENCE_PETRIFIED):
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_PETRIFIED_MODIFIER);
            break;
        case IDX(ARMY_SPELL_INFLUENCE_ANTI_MAGIC):
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_ANTI_MAGIC_MODIFIER);
            break;
        case IDX(ARMY_SPELL_INFLUENCE_STONESKIN):
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_STONE_SKIN_MODIFIER);
            break;
        case IDX(ARMY_SPELL_INFLUENCE_STEELSKIN):
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_STEEL_SKIN_MODIFIER);
            break;
        case IDX(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER): {
            i32 adjacent = 0;
            i32 dragonCount = adjacent;
            i32 i;
            for (i = 0; i < m_armyCount[1 - target->m_side]; i++) {
                otherArmy = &m_armies[target->m_side][i];
                if (otherArmy->m_monsterType == CREATURE_GREEN_DRAGON
                    || otherArmy->m_monsterType == CREATURE_RED_DRAGON
                    || otherArmy->m_monsterType == CREATURE_BLACK_DRAGON
                    || otherArmy->m_monsterType == CREATURE_BONE_DRAGON) {
                    dragonCount++;
                    if (target->OtherArmyAdjacent(otherArmy->m_side, otherArmy->m_index))
                        adjacent = 1;
                }
            }
            float dragonMod;
            if (adjacent)
                dragonMod = COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER;
            else
                dragonMod = static_cast<float>(dragonCount / m_armyCount[1 - target->m_side]);
            effect = static_cast<i32>(COMBAT_SPELL_AI_DRAGON_SLAYER_MODIFIER * dragonMod);
            break;
        }
        case IDX(ARMY_SPELL_INFLUENCE_SHIELD): {
            i32 shooterCount = 0;
            i32 i;
            for (i = 0; i < m_armyCount[1 - target->m_side]; i++) {
                otherArmy = &m_armies[target->m_side][i];
                if (HAS(otherArmy->m_monster.flags.all, MONSTER_FLAGS_SHOOTER))
                    shooterCount++;
            }
            float shooterMod = static_cast<float>(shooterCount / m_armyCount[1 - target->m_side]);
            if (target->m_side == 0 && m_inCastleCombat) {
                shooterMod = static_cast<float>(shooterMod + COMBAT_SPELL_AI_SIEGE_SHIELD_BONUS);
                if (shooterMod > COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER)
                    shooterMod = COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER;
            }
            effect = static_cast<i32>(COMBAT_SPELL_AI_SHIELD_MODIFIER * shooterMod);
            break;
        }
        default:
            effect = 0;
            break;
    }

    effect = static_cast<i32>(effect * workChance);
    if ((target->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]
         || target->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])
        && influence != ARMY_SPELL_INFLUENCE_ANTI_MAGIC)
        effect = 0;
    return effect;
}

VA(0x00488607, 0x66)
void combatManager::ClearEffects(void) {
    i32 side;
    i32 idx;
    for (side = 0; side < COMBAT_SIDE_COUNT; side++) {
        for (idx = 0; idx < COMBAT_ARMY_SLOT_COUNT; idx++)
            *(gArmyEffected[0] + side * COMBAT_ARMY_SLOT_COUNT + idx) = 0;
    }
}

VA(0x0048866d, 0x40)
void combatManager::NextPos(i32* hex) {
    if ((*hex + 2) % 13 == 0)
        *hex += 3;
    else
        (*hex)++;
}

VA(0x004886ad, 0xa1)
i32 combatManager::FirstArmy(i32 startHex, i32 side, i32* hex) {
    while (startHex <= COMBAT_SPELL_AI_LAST_HEX) {
        if (m_hexCells[startHex].m_occupantSide == side
            || (side == COMBAT_SPELL_AI_ANY_SIDE && m_hexCells[startHex].m_occupantSide >= 0)) {
            *hex = startHex;
            return 0;
        }
        NextPos(&startHex);
    }
    *hex = -1;
    return 1;
}

VA(0x0048874e, 0x73)
i32 combatManager::FirstResurrectable(i32 startHex, i32* hex, i32 spell) {
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

// @semantic
// All ten live retail slots align; retail's 0x54 frame has one unreferenced
// compiler word at -0x2c beyond ours' 0x50 frame. All 23/23 external
// relocations agree. The first CFG residual after target validation is three
// retail-only continuation jumps, followed by pooled-double identity. Compound
// versus early-continue filters and split versus grouped negative influences
// were tried; revisit in the byte-last-mile phase.
VA(0x004887c1, 0x421)
void combatManager::EffectSpellCure(i32* effect, i32 targetSide, i32 targetIndex, i32 cure) {
    i32 sideWork;
    i32 fullQuantityWork;
    i32 armyValueResult;
    i32 negativeEffectResult;
    *effect = 0;
    i32 done = 0;
    i32 influence;
    army* combatTarget;
    i32 curePointsTotal;
    i32 positiveEffectResult;
    i32 index;

    if (targetSide == COMBAT_SPELL_AI_ANY_SIDE)
        sideWork = m_currentSide;
    else
        sideWork = targetSide;

    while (!done) {
        positiveEffectResult = 0;
        negativeEffectResult = 0;
        for (index = 0; index < COMBAT_ARMY_SLOT_COUNT; index++) {
            if (targetIndex != -1 && index != targetIndex)
                continue;
            if (m_armies[sideWork][index].m_monsterType >= 0
                && m_armies[sideWork][index].m_quantity > 0) {
                combatTarget = &m_armies[sideWork][index];
                if (cure == 1) {
                    curePointsTotal =
                        m_spellPower[m_currentSide] * COMBAT_SPELL_AI_CURE_POINTS_PER_POWER;
                    if (curePointsTotal > combatTarget->m_hitPointsLost)
                        curePointsTotal = combatTarget->m_hitPointsLost;
                    positiveEffectResult = static_cast<i32>(
                        positiveEffectResult
                        + static_cast<float>(
                              gMonsterDatabase[IDX(combatTarget->m_monsterType)].fightValue
                          ) * curePointsTotal
                              * COMBAT_SPELL_AI_CURE_VALUE_MODIFIER
                              / combatTarget->m_monster.hitPoints
                    );
                }

                fullQuantityWork =
                    HAS(combatTarget->m_monster.flags.all, MONSTER_FLAGS_FULL_AI_QUANTITY) != 0;
                armyValueResult = gMonsterDatabase[IDX(combatTarget->m_monsterType)].fightValue
                                  * combatTarget->m_quantity;
                if (HAS(combatTarget->m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)) {
                    negativeEffectResult = armyValueResult;
                    continue;
                }

                for (influence = 0; influence < ARMY_SPELL_INFLUENCE_COUNT; influence++) {
                    if (!combatTarget->m_spellInfluence[influence])
                        continue;
                    switch (influence) {
                        case IDX(ARMY_SPELL_INFLUENCE_HASTE):
                        case IDX(ARMY_SPELL_INFLUENCE_BLESS):
                        case IDX(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER):
                        case IDX(ARMY_SPELL_INFLUENCE_BLOODLUST):
                        case IDX(ARMY_SPELL_INFLUENCE_SHIELD):
                        case IDX(ARMY_SPELL_INFLUENCE_STONESKIN):
                        case IDX(ARMY_SPELL_INFLUENCE_STEELSKIN):
                            positiveEffectResult = static_cast<i32>(
                                positiveEffectResult
                                + RawEffectSpellInfluence(combatTarget, influence)
                                      * gfCancelDurationMods
                                          [combatTarget->m_spellInfluence[influence]
                                                       + fullQuantityWork
                                                   >= COMBAT_SPELL_AI_MAX_DURATION
                                               ? COMBAT_SPELL_AI_MAX_DURATION
                                               : combatTarget->m_spellInfluence[influence]
                                                     + fullQuantityWork]
                            );
                            break;
                        case IDX(ARMY_SPELL_INFLUENCE_SLOW):
                        case IDX(ARMY_SPELL_INFLUENCE_CURSE):
                        case IDX(ARMY_SPELL_INFLUENCE_BLIND):
                        case IDX(ARMY_SPELL_INFLUENCE_BERSERK):
                        case IDX(ARMY_SPELL_INFLUENCE_PARALYZE):
                        case IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE):
                        case IDX(ARMY_SPELL_INFLUENCE_PETRIFIED):
                            negativeEffectResult = static_cast<i32>(
                                negativeEffectResult
                                + RawEffectSpellInfluence(combatTarget, influence)
                                      * gfCancelDurationMods
                                          [combatTarget->m_spellInfluence[influence]
                                                       + fullQuantityWork
                                                   >= COMBAT_SPELL_AI_MAX_DURATION
                                               ? COMBAT_SPELL_AI_MAX_DURATION
                                               : combatTarget->m_spellInfluence[influence]
                                                     + fullQuantityWork]
                            );
                            break;
                        case IDX(ARMY_SPELL_INFLUENCE_ANTI_MAGIC):
                            break;
                    }
                }
            }
        }
        if (cure == 1)
            positiveEffectResult = 0;
        if (m_currentSide == sideWork)
            *effect += -negativeEffectResult - positiveEffectResult;
        else
            *effect += positiveEffectResult + negativeEffectResult;
        if (targetSide == COMBAT_SPELL_AI_ANY_SIDE && m_currentSide == sideWork)
            sideWork = 1 - m_currentSide;
        else
            done = 1;
    }
}

VA(0x00488be2, 0x176)
void combatManager::EffectSpellResurrect(i32* effect, i32 hex, SpellType spell) {
    army* targetStack;
    i32 resurrectPowerWork;
    i32 quantityResult[COMBAT_SIDE_COUNT];
    i32 armyIndexWork;

    resurrectPowerWork = m_spellPower[m_currentSide] * COMBAT_SPELL_AI_RESURRECT_POINTS_PER_POWER;
    if (m_heroes[m_currentSide] != 0 && m_heroes[m_currentSide]->HasArtifact(ARTIFACT_ANKH))
        resurrectPowerWork <<= 1;

    armyIndexWork = FindResurrectArmyIndex(m_currentSide, IDX(spell), hex);
    targetStack = &m_armies[m_currentSide][armyIndexWork];
    quantityResult[0] = resurrectPowerWork / targetStack->m_monster.hitPoints;
    if (targetStack->m_quantity + quantityResult[0] > targetStack->m_initialQuantity)
        quantityResult[0] = targetStack->m_initialQuantity - targetStack->m_quantity;
    *effect = gMonsterDatabase[IDX(targetStack->m_monsterType)].fightValue * quantityResult[0];
    *effect = static_cast<i32>(*effect * targetStack->SpellCastWorkChance(SpellType(spell)));
    if (spell == SPELL_RESURRECT)
        *effect = static_cast<i32>(*effect * COMBAT_SPELL_AI_RESURRECT_VALUE_MODIFIER);
}

// @semantic
// Exact 0x98 frame and all live slots after recovering the three independent
// two-side result arrays and the post-loop finalization CFG. The first report
// boundary is the opening local switch table after eight instructions; 86/82
// COFF sites have no candidate-only external target. Overlapping-array and
// do/finalize forms were exhausted; revisit with jump-table-aware explicit
// ranges in the byte-last-mile phase.
VA(0x00488d58, 0xcc9)
void combatManager::EffectSpellDamage(i32* effect, SpellType spell, i32 targetHex) {
    i32 fightValueKilledAI[2];
    i32 creaturesKilledResult;
    i32 remainderResult;
    i32 damagePerPowerResult;
    i32 stacksKilledCandidate[2];
    i32 doneWork;
    army* targetCreature;
    i32 killedCombatValue[2];
    i32 side;
    i32 damage;
    i32 currentHex;
    i32l spellDamageWork;
    float workChanceWork;
    i32 newDefense;
    CreatureType monsterTotal;
    i32 step;
    i32 disruptingRayValueTotal;

    switch (spell) {
        case SPELL_ARMAGEDDON:
            damagePerPowerResult = COMBAT_SPELL_AI_ARMAGEDDON_DAMAGE_PER_POWER;
            break;
        case SPELL_HOLY_WORD:
            damagePerPowerResult = COMBAT_SPELL_AI_HOLY_WORD_DAMAGE_PER_POWER;
            break;
        case SPELL_HOLY_SHOUT:
            damagePerPowerResult = COMBAT_SPELL_AI_HOLY_SHOUT_DAMAGE_PER_POWER;
            break;
        case SPELL_DEATH_RIPPLE:
            damagePerPowerResult = COMBAT_SPELL_AI_DEATH_RIPPLE_DAMAGE_PER_POWER;
            break;
        case SPELL_DEATH_WAVE:
            damagePerPowerResult = COMBAT_SPELL_AI_DEATH_WAVE_DAMAGE_PER_POWER;
            break;
        case SPELL_ELEMENTAL_STORM:
            damagePerPowerResult = COMBAT_SPELL_AI_ELEMENTAL_STORM_DAMAGE_PER_POWER;
            break;
        case SPELL_FIREBALL:
            damagePerPowerResult = COMBAT_SPELL_AI_FIRE_DAMAGE_PER_POWER;
            break;
        case SPELL_FIREBLAST:
            damagePerPowerResult = COMBAT_SPELL_AI_FIRE_DAMAGE_PER_POWER;
            break;
        case SPELL_METEOR_SHOWER:
            damagePerPowerResult = COMBAT_SPELL_AI_ELEMENTAL_STORM_DAMAGE_PER_POWER;
            break;
        case SPELL_LIGHTNING_BOLT:
            damagePerPowerResult = COMBAT_SPELL_AI_LIGHTNING_DAMAGE_PER_POWER;
            break;
        case SPELL_MAGIC_ARROW:
            damagePerPowerResult = COMBAT_SPELL_AI_MAGIC_ARROW_DAMAGE_PER_POWER;
            break;
        case SPELL_CHAIN_LIGHTNING:
            damagePerPowerResult = COMBAT_SPELL_AI_CHAIN_LIGHTNING_DAMAGE_PER_POWER;
            break;
        case SPELL_COLD_RAY:
            damagePerPowerResult = COMBAT_SPELL_AI_COLD_RAY_DAMAGE_PER_POWER;
            break;
        case SPELL_COLD_RING:
            damagePerPowerResult = COMBAT_SPELL_AI_COLD_RING_DAMAGE_PER_POWER;
            break;
        default:
            damagePerPowerResult = 0;
            break;
    }

    damage = m_spellPower[m_currentSide] * damagePerPowerResult;
    currentHex = 0;
    step = 0;
    doneWork = 0;
    creaturesKilledResult = 0;
    disruptingRayValueTotal = 0;
    if (m_hexCells[targetHex].m_occupantIndex >= 0)
        targetCreature =
            &m_armies[m_hexCells[targetHex].m_occupantSide][m_hexCells[targetHex].m_occupantIndex];

    for (side = 0; side < COMBAT_SIDE_COUNT; side++) {
        stacksKilledCandidate[side] = 0;
        fightValueKilledAI[side] = 0;
        killedCombatValue[side] = 0;
    }
    ClearEffects();

    while (!doneWork) {
        switch (spell) {
            case SPELL_LIGHTNING_BOLT:
            case SPELL_MAGIC_ARROW:
            case SPELL_COLD_RAY:
            case SPELL_DISRUPTING_RAY:
                if (currentHex == targetHex)
                    doneWork = 1;
                else
                    currentHex = targetHex;
                break;
            case SPELL_CHAIN_LIGHTNING:
                if (currentHex == 0) {
                    currentHex = targetHex;
                } else {
                    damage >>= 1;
                    currentHex = GetNextChainLightningTarget(targetCreature, 0);
                }
                step++;
                if (step > 4 || currentHex == -1)
                    doneWork = 1;
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
                    doneWork = 0;
                else
                    doneWork = 1;
                doneWork = currentHex >= COMBAT_SPELL_AI_LAST_HEX + 1;
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
                    doneWork = 1;
                }
        }

        if (!doneWork && currentHex >= 0 && currentHex < COMBAT_SPELL_AI_LAST_HEX + 1
            && m_hexCells[currentHex].m_occupantIndex >= 0
            && m_hexCells[currentHex].m_occupantSide >= 0) {
            targetCreature = &m_armies[m_hexCells[currentHex].m_occupantSide]
                                      [m_hexCells[currentHex].m_occupantIndex];
            if (!gArmyEffected[m_hexCells[currentHex].m_occupantSide]
                              [m_hexCells[currentHex].m_occupantIndex]) {
                gArmyEffected[m_hexCells[currentHex].m_occupantSide]
                             [m_hexCells[currentHex].m_occupantIndex] = 1;
                workChanceWork = targetCreature->SpellCastWorkChance(SpellType(spell));
                if (workChanceWork > 0.0f) {
                    spellDamageWork = static_cast<i32l>(damage * workChanceWork);
                    monsterTotal = targetCreature->m_monsterType;
                    switch (spell) {
                        case SPELL_FIREBALL:
                        case SPELL_FIREBLAST:
                            if (monsterTotal == CREATURE_IRON_GOLEM
                                || monsterTotal == CREATURE_STEEL_GOLEM)
                                spellDamageWork = static_cast<i32l>(
                                    spellDamageWork * SPELL_GOLEM_DAMAGE_MULTIPLIER
                                );
                            if (monsterTotal == CREATURE_WATER_ELEMENTAL)
                                spellDamageWork <<= 1;
                            break;
                        case SPELL_LIGHTNING_BOLT:
                        case SPELL_CHAIN_LIGHTNING:
                        case SPELL_ELEMENTAL_STORM:
                            if (monsterTotal == CREATURE_IRON_GOLEM
                                || monsterTotal == CREATURE_STEEL_GOLEM)
                                spellDamageWork = static_cast<i32l>(
                                    spellDamageWork * SPELL_GOLEM_DAMAGE_MULTIPLIER
                                );
                            if (monsterTotal == CREATURE_AIR_ELEMENTAL)
                                spellDamageWork <<= 1;
                            break;
                        case SPELL_ARMAGEDDON:
                            if (monsterTotal == CREATURE_IRON_GOLEM
                                || monsterTotal == CREATURE_STEEL_GOLEM)
                                spellDamageWork = static_cast<i32l>(
                                    spellDamageWork * SPELL_GOLEM_DAMAGE_MULTIPLIER
                                );
                            break;
                        case SPELL_METEOR_SHOWER:
                            if (monsterTotal == CREATURE_IRON_GOLEM
                                || monsterTotal == CREATURE_STEEL_GOLEM)
                                spellDamageWork = static_cast<i32l>(
                                    spellDamageWork * SPELL_GOLEM_DAMAGE_MULTIPLIER
                                );
                            if (monsterTotal == CREATURE_EARTH_ELEMENTAL)
                                spellDamageWork <<= 1;
                            break;
                        case SPELL_COLD_RAY:
                        case SPELL_COLD_RING:
                            if (monsterTotal == CREATURE_IRON_GOLEM
                                || monsterTotal == CREATURE_STEEL_GOLEM)
                                spellDamageWork = static_cast<i32l>(
                                    spellDamageWork * SPELL_GOLEM_DAMAGE_MULTIPLIER
                                );
                            if (monsterTotal == CREATURE_FIRE_ELEMENTAL)
                                spellDamageWork <<= 1;
                            break;
                    }

                    ModifyDamageForArtifacts(
                        &spellDamageWork,
                        IDX(spell),
                        m_heroes[m_currentSide],
                        m_heroes[targetCreature->m_side]
                    );
                    if (HAS(targetCreature->m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)
                        && spellDamageWork != 0)
                        spellDamageWork = COMBAT_SPELL_AI_MIRROR_LETHAL_DAMAGE;

                    creaturesKilledResult = spellDamageWork / targetCreature->m_monster.hitPoints;
                    remainderResult = spellDamageWork % targetCreature->m_monster.hitPoints;
                    if (targetCreature->m_monster.hitPoints
                        <= targetCreature->m_hitPointsLost + remainderResult) {
                        creaturesKilledResult =
                            spellDamageWork / targetCreature->m_monster.hitPoints + 1;
                        remainderResult -=
                            targetCreature->m_monster.hitPoints - targetCreature->m_hitPointsLost;
                    }
                    if (targetCreature->m_quantity <= creaturesKilledResult) {
                        creaturesKilledResult = targetCreature->m_quantity;
                        remainderResult = 0;
                        stacksKilledCandidate[m_hexCells[currentHex].m_occupantSide]++;
                    }

                    fightValueKilledAI[m_hexCells[currentHex].m_occupantSide] = static_cast<i32>(
                        fightValueKilledAI[m_hexCells[currentHex].m_occupantSide]
                        + (remainderResult * COMBAT_SPELL_AI_PARTIAL_DAMAGE_MODIFIER
                           + targetCreature->m_monster.hitPoints * creaturesKilledResult)
                              * gMonsterDatabase[IDX(targetCreature->m_monsterType)].fightValue
                              / targetCreature->m_monster.hitPoints
                    );
                    killedCombatValue[m_hexCells[currentHex].m_occupantSide] +=
                        gMonsterDatabase[IDX(targetCreature->m_monsterType)].fightValue
                        * targetCreature->m_monster.hitPoints * creaturesKilledResult
                        / targetCreature->m_monster.hitPoints;
                    if (HAS(targetCreature->m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)) {
                        killedCombatValue[m_hexCells[currentHex].m_occupantSide] /= 2;
                        fightValueKilledAI[m_hexCells[currentHex].m_occupantSide] /= 2;
                    }

                    if (spell == SPELL_DISRUPTING_RAY) {
                        newDefense = targetCreature->m_monster.defense
                                     - SPELL_DISRUPTING_RAY_DEFENSE_REDUCTION;
                        if (newDefense < SPELL_MINIMUM_DEFENSE)
                            newDefense = SPELL_MINIMUM_DEFENSE;
                        disruptingRayValueTotal = static_cast<i32>(
                            disruptingRayValueTotal
                            + gMonsterDatabase[IDX(targetCreature->m_monsterType)].fightValue
                                  * (targetCreature->m_monster.defense - newDefense)
                                  * (targetCreature->m_quantity - creaturesKilledResult)
                                  * COMBAT_SPELL_AI_DISRUPTING_RAY_MODIFIER
                        );
                    }
                }
            }
        }
    }

    if (stacksKilledCandidate[0] < m_armyCount[0] && stacksKilledCandidate[1] < m_armyCount[1]) {
        *effect = fightValueKilledAI[1 - m_currentSide] - fightValueKilledAI[m_currentSide]
                  + disruptingRayValueTotal;
    } else if (killedCombatValue[m_currentSide] > 0) {
        *effect = killedCombatValue[1 - m_currentSide] - killedCombatValue[m_currentSide]
                  + disruptingRayValueTotal;
    } else {
        *effect =
            COMBAT_SPELL_AI_DECISIVE_EFFECT - gsSpellInfo[IDX(spell)].aiValue + disruptingRayValueTotal;
    }
    if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat)
        *effect = static_cast<i32>(*effect * COMBAT_SPELL_AI_CASTLE_EFFECT_MODIFIER);
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f80b8) float gfDurationMods[12] =
    {0.0f, 0.33f, 0.55f, 0.72f, 0.85f, 0.95f, 1.03f, 1.08f, 1.12f, 1.15f, 1.18f, 0.0f};
DATA(0x004f80e8) float gfCancelDurationMods[11] =
    {0.0f, 0.5f, 0.65f, 0.78f, 0.85f, 0.95f, 1.03f, 1.08f, 1.12f, 1.15f, 1.18f};
DATA(0x005284b4) i32 giCurrSpellGroup;
