#include <Ints.h>
#include <SOURCE/KB.h>
#include <SOURCE/PATH.h>
#include <SOURCE/SPELLS.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/hero.h>
#define COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER 0.5
#define COMBAT_SPELL_AI_SIEGE_SHOOTER_MODIFIER 1.5
static const float COMBAT_SPELL_AI_BLIND_MODIFIER = -0.4f;
static const float COMBAT_SPELL_AI_BERSERK_MODIFIER = -0.55f;
static const float COMBAT_SPELL_AI_PARALYZE_MODIFIER = -0.5f;
static const float COMBAT_SPELL_AI_HYPNOTIZE_MODIFIER = -0.65f;
static const float COMBAT_SPELL_AI_PETRIFIED_MODIFIER = -0.25f;
static const float COMBAT_SPELL_AI_BLOODLUST_MODIFIER = 0.14f;
static const float COMBAT_SPELL_AI_STONE_SKIN_MODIFIER = 0.16f;
static const float COMBAT_SPELL_AI_DRAGON_SLAYER_MODIFIER = 0.28f;
static const float COMBAT_SPELL_AI_ANTI_MAGIC_MODIFIER = 0.2f;
static const float COMBAT_SPELL_AI_STEEL_SKIN_MODIFIER = 0.28f;
static const float COMBAT_SPELL_AI_SHIELD_MODIFIER = 0.45f;
#define COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER 1.0f
#define COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER
#define COMBAT_SPELL_AI_MIRROR_POWER_ONE_MODIFIER 0.16f
#define COMBAT_SPELL_AI_MIRROR_POWER_TWO_MODIFIER 0.2f
#define COMBAT_SPELL_AI_MIRROR_DEFAULT_MODIFIER 0.22f
#define COMBAT_SPELL_AI_MIRROR_SHOOTER_MODIFIER 1.4
#define COMBAT_SPELL_AI_TURN_CAP 7.0f
#define COMBAT_SPELL_AI_TURN_DIVISOR 10.0f
#define COMBAT_SPELL_AI_ZERO_EFFECT 0.0
#define COMBAT_SPELL_AI_AVERAGE_DAMAGE_MODIFIER 0.5
#define COMBAT_SPELL_AI_BLESS_CURSE_MODIFIER 0.45
#define COMBAT_SPELL_AI_SIEGE_SHIELD_BONUS 0.3
#define COMBAT_SPELL_AI_CURE_VALUE_MODIFIER 0.75
#define COMBAT_SPELL_AI_RESURRECT_VALUE_MODIFIER 0.85
#define COMBAT_SPELL_AI_PARTIAL_DAMAGE_MODIFIER 0.75
#define COMBAT_SPELL_AI_DISRUPTING_RAY_MODIFIER 0.05
#define COMBAT_SPELL_AI_CASTLE_EFFECT_MODIFIER 1.25

enum class CombatSpellAITargetMode : i32 {
    SPELL_AI_GLOBAL       = 0,
    SPELL_AI_SUM_FRIENDLY = 1,
    SPELL_AI_SUM_ENEMY    = 2,
    SPELL_AI_AREA         = 3,
    SPELL_AI_FRIENDLY     = 4,
    SPELL_AI_ENEMY        = 5,
    SPELL_AI_RESURRECT    = 6,
    SPELL_AI_ANY_ARMY     = 7
};
using enum CombatSpellAITargetMode;

typedef enum CombatLayoutConstant {
    SPELL_AI_FIRST_HEX                  = 1,
    SPELL_AI_LAST_HEX                   = 0x73,
    SPELL_AI_ANY_SIDE                   = 2,
    SPELL_AI_MAX_DURATION               = 10,
    SPELL_AI_MAX_MANA_RATIO             = 10,
    SPELL_AI_EARTHQUAKE_NO_DAMAGE_SCORE = 29999,
    SPELL_AI_EARTHQUAKE_WALL_SCORE      = 100,
    SPELL_AI_HEX_ROW_END_OFFSET         = 2,
    SPELL_AI_HEX_ROW_SKIP               = 3,
    SPELL_AI_MIRROR_VALUE_DIVISOR       = 2
} CombatLayoutConstant;

i32 combatManager::DoSpellAI(CombatSide side, i32 restricted) {
    SpellType bestSpellChoice;
    i32 effectScore;
    H2SteppedEnumStorage<SpellType, i32> spellIndex;
    i32 bestHexWork;
    i32 candidateHex;
    i32 bestEffectWork;
    i32 manaRatioResult;

    bestEffectWork = 0;
    bestSpellChoice = SPELL_NONE;
    bestHexWork = -1;

    if (m_heroes[H2EnumIndex(side)] == NULL)
        return 0;


    for (spellIndex = H2EnumIndex(COMBAT_ATTACKER_SIDE); H2EnumIndex(spellIndex) < COMBAT_SIDE_COUNT; spellIndex++) {
        if (m_heroes[H2EnumIndex(spellIndex)] != NULL
            && m_heroes[H2EnumIndex(spellIndex)]->HasArtifact(ARTIFACT_SPHERE_NEGATION))
            return 0;
    }

    for (spellIndex = SPELL_FIREBALL; spellIndex < SPELL_COUNT; spellIndex++) {
        if (m_heroes[H2EnumIndex(side)]->HasSpell(spellIndex)
            && (H2EnumIndex((gsSpellInfo[H2EnumIndex(spellIndex)].attributes) & (SPELL_INFO_ATTRIBUTE_COMBAT)))
            && GetManaCost(spellIndex, m_heroes[H2EnumIndex(side)]) <= m_heroes[H2EnumIndex(side)]->m_spellPoints) {
            if (restricted && spellIndex != SPELL_FIREBALL && spellIndex != SPELL_FIREBLAST
                && spellIndex != SPELL_LIGHTNING_BOLT && spellIndex != SPELL_CHAIN_LIGHTNING
                && spellIndex != SPELL_HOLY_WORD && spellIndex != SPELL_HOLY_SHOUT
                && spellIndex != SPELL_MAGIC_ARROW && spellIndex != SPELL_ARMAGEDDON
                && spellIndex != SPELL_ELEMENTAL_STORM && spellIndex != SPELL_METEOR_SHOWER
                && spellIndex != SPELL_COLD_RAY && spellIndex != SPELL_COLD_RING
                && spellIndex != SPELL_DEATH_RIPPLE && spellIndex != SPELL_DEATH_WAVE)
                continue;
            DetermineEffectOfSpell(spellIndex, &effectScore, &candidateHex);

            manaRatioResult =
                m_heroes[H2EnumIndex(side)]->m_spellPoints / GetManaCost(spellIndex, m_heroes[H2EnumIndex(side)]);
            if (manaRatioResult > SPELL_AI_MAX_MANA_RATIO)
                manaRatioResult = SPELL_AI_MAX_MANA_RATIO;
            effectScore = static_cast<i32>(effectScore * gfSpellCastableCombatMod[manaRatioResult]);

            if (effectScore > bestEffectWork) {
                bestEffectWork = effectScore;
                bestSpellChoice = spellIndex;
                bestHexWork = candidateHex;
            }
        }
    }

    if (bestEffectWork > 0) {
        giNextAction = ACTION_CAST_SPELL;
        giNextActionExtra = H2EnumIndex(bestSpellChoice);
        giNextActionGridIndex = bestHexWork;
        return 1;
    }
    return 0;
}

void combatManager::DetermineEffectOfSpell(SpellType spell, i32* bestEffect, i32* bestHex) {


    i32 durMax;
    i32 wallsDamagedTotal;
    i32 spellPowerWork;
    i32 hexCell;
    i32 hasDamageReductionResult;
    i32 effect;
    army* targetCreature;
    CombatSpellAITargetMode spellMode;
    i32 bDone;
    i32 team;
    float durationFactor;
    i32 isMindEffect;
    i32 fullQuantityFlag;
    i32 cureAmount;
    i32 idx;
    i32 sumEffect;

    bDone = 0;
    team = 0;
    hexCell = SPELL_AI_FIRST_HEX;
    durationFactor = COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE;
    fullQuantityFlag = 1;
    sumEffect = 0;
    targetCreature = NULL;
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
            spellMode = SPELL_AI_GLOBAL;
            break;
        case SPELL_FIREBALL:
        case SPELL_FIREBLAST:
        case SPELL_METEOR_SHOWER:
        case SPELL_COLD_RING:
            spellMode = SPELL_AI_AREA;
            break;
        case SPELL_MASS_HASTE:
        case SPELL_MASS_BLESS:
        case SPELL_MASS_SHIELD:
            spellMode = SPELL_AI_SUM_FRIENDLY;
            team = H2EnumIndex(m_currentSide);
            break;
        case SPELL_MASS_SLOW:
        case SPELL_MASS_CURSE:
            spellMode = SPELL_AI_SUM_ENEMY;
            team = H2EnumIndex(OppositeCombatSide(m_currentSide));
            break;
        case SPELL_DISPEL:
            spellMode = SPELL_AI_ANY_ARMY;
            team = SPELL_AI_ANY_SIDE;
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
            spellMode = SPELL_AI_FRIENDLY;
            team = H2EnumIndex(m_currentSide);
            break;
        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_ANIMATE_DEAD:
            spellMode = SPELL_AI_RESURRECT;
            team = H2EnumIndex(m_currentSide);
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
            spellMode = SPELL_AI_ENEMY;
            team = H2EnumIndex(OppositeCombatSide(m_currentSide));
            break;
        default:
            *bestEffect = 0;
            return;
    }

    if (spellMode == SPELL_AI_RESURRECT)
        bDone = FirstResurrectable(SPELL_AI_FIRST_HEX, &hexCell, spell);
    if (spellMode == SPELL_AI_FRIENDLY || spellMode == SPELL_AI_ENEMY
        || spellMode == SPELL_AI_SUM_ENEMY
        || spellMode == SPELL_AI_SUM_FRIENDLY
        || spellMode == SPELL_AI_ANY_ARMY)
        bDone = FirstArmy(SPELL_AI_FIRST_HEX, team, &hexCell);

    while (!bDone) {
        isMindEffect = 0;
        hasDamageReductionResult = 0;
        effect = 0;

        if (m_hexCells[hexCell].m_occupantIndex < 0
            || m_hexCells[hexCell].m_occupantSide == COMBAT_SIDE_NONE) {
            targetCreature = NULL;
        } else {
            targetCreature = &m_armies[H2EnumIndex(m_hexCells[hexCell].m_occupantSide)]
                                      [m_hexCells[hexCell].m_occupantIndex];
            giCurrSpellGroup = H2EnumIndex(m_hexCells[hexCell].m_occupantSide);
            fullQuantityFlag =
                (H2EnumIndex((targetCreature->m_monster.flags.all) & (MONSTER_FLAGS_FULL_AI_QUANTITY))) != 0;

            spellPowerWork = m_spellPower[H2EnumIndex(m_currentSide)];
            if (m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS))
                spellPowerWork += SPELL_HOURGLASS_POWER_BONUS;
            if (m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_WIZARD_HAT))
                spellPowerWork += SPELL_WIZARD_HAT_POWER_BONUS;

            durationFactor = gfDurationMods
                [spellPowerWork - fullQuantityFlag >= SPELL_AI_MAX_DURATION
                     ? SPELL_AI_MAX_DURATION
                     : spellPowerWork - fullQuantityFlag];

            if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]
                || targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BERSERK)])
                isMindEffect = 1;
            if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLIND)]
                || targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PARALYZE)]
                || targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PETRIFIED)])
                hasDamageReductionResult = 1;
        }

        switch (spell) {
            case SPELL_CURE:
                EffectSpellCure(&effect, H2EnumIndex(m_currentSide), hexCell, 1);
                break;
            case SPELL_MASS_CURE:
                EffectSpellCure(&effect, H2EnumIndex(m_currentSide), -1, 1);
                break;
            case SPELL_DISPEL:
                EffectSpellCure(&effect, H2EnumIndex(targetCreature->m_side), targetCreature->m_index, 0);
                break;
            case SPELL_MASS_DISPEL:
                EffectSpellCure(&effect, SPELL_AI_ANY_SIDE, -1, 0);
                break;
            case SPELL_RESURRECT:
            case SPELL_TRUE_RESURRECT:
            case SPELL_ANIMATE_DEAD:
                if (isMindEffect) {
                } else {
                    EffectSpellResurrect(&effect, hexCell, spell);
                }
                break;
            case SPELL_MIRROR_IMAGE:
            case SPELL_SUMMON_EARTH_ELEMENTAL:
            case SPELL_SUMMON_AIR_ELEMENTAL:
            case SPELL_SUMMON_FIRE_ELEMENTAL:
            case SPELL_SUMMON_WATER_ELEMENTAL:
                effect = EffectSpellCreateCreature(hexCell, spell);
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
                if (isMindEffect) {
                } else {
                    EffectSpellDamage(&effect, spell, hexCell);
                    if (hasDamageReductionResult)
                        effect = static_cast<i32>(effect * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER);
                }
                break;
            case SPELL_HASTE:
            case SPELL_MASS_HASTE:
                if (hasDamageReductionResult) {
                } else {
                    if (isMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HASTE)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    ARMY_SPELL_INFLUENCE_HASTE
                                )
                                * durationFactor
                            );
                            if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_SLOW)]) {
                                effect = static_cast<i32>(
                                    RawEffectSpellInfluence(
                                        targetCreature,
                                        ARMY_SPELL_INFLUENCE_SLOW
                                    )
                                        * gfCancelDurationMods
                                            [targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_SLOW)] + fullQuantityFlag
                                                     >= SPELL_AI_MAX_DURATION
                                                 ? SPELL_AI_MAX_DURATION
                                                 : targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_SLOW)] + fullQuantityFlag]
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
                    if (isMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BERSERK)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    ARMY_SPELL_INFLUENCE_BERSERK
                                )
                                * durationFactor
                            );
                            if (targetCreature
                                    ->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]) {
                                effect = static_cast<i32>(
                                    RawEffectSpellInfluence(
                                        targetCreature,
                                        ARMY_SPELL_INFLUENCE_HYPNOTIZE
                                    )
                                        * gfCancelDurationMods
                                            [targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HYPNOTIZE)] + fullQuantityFlag
                                                     >= SPELL_AI_MAX_DURATION
                                                 ? SPELL_AI_MAX_DURATION
                                                 : targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HYPNOTIZE)] + fullQuantityFlag]
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
                    if (isMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    ARMY_SPELL_INFLUENCE_HYPNOTIZE
                                )
                                * durationFactor
                            );
                            if (targetCreature
                                    ->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BERSERK)]) {
                                effect = static_cast<i32>(
                                    RawEffectSpellInfluence(
                                        targetCreature,
                                        ARMY_SPELL_INFLUENCE_BERSERK
                                    )
                                        * gfCancelDurationMods
                                            [targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BERSERK)] + fullQuantityFlag
                                                     >= SPELL_AI_MAX_DURATION
                                                 ? SPELL_AI_MAX_DURATION
                                                 : targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BERSERK)] + fullQuantityFlag]
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
                    if (isMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_SLOW)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    ARMY_SPELL_INFLUENCE_SLOW
                                )
                                * durationFactor
                            );
                            if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HASTE)]) {
                                effect = static_cast<i32>(
                                    RawEffectSpellInfluence(
                                        targetCreature,
                                        ARMY_SPELL_INFLUENCE_HASTE
                                    )
                                        * gfCancelDurationMods
                                            [targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HASTE)] + fullQuantityFlag
                                                     >= SPELL_AI_MAX_DURATION
                                                 ? SPELL_AI_MAX_DURATION
                                                 : targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HASTE)] + fullQuantityFlag]
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
                    if (isMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLESS)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    ARMY_SPELL_INFLUENCE_BLESS
                                )
                                * durationFactor
                            );
                            if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_CURSE)]) {
                                effect = static_cast<i32>(
                                    RawEffectSpellInfluence(
                                        targetCreature,
                                        ARMY_SPELL_INFLUENCE_CURSE
                                    )
                                        * gfCancelDurationMods
                                            [targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_CURSE)] + fullQuantityFlag
                                                     >= SPELL_AI_MAX_DURATION
                                                 ? SPELL_AI_MAX_DURATION
                                                 : targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_CURSE)] + fullQuantityFlag]
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
                    if (isMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_CURSE)]) {
                        } else {
                            effect = static_cast<i32>(
                                -RawEffectSpellInfluence(
                                    targetCreature,
                                    ARMY_SPELL_INFLUENCE_CURSE
                                )
                                * durationFactor
                            );
                            if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLESS)]) {
                                effect = static_cast<i32>(
                                    RawEffectSpellInfluence(
                                        targetCreature,
                                        ARMY_SPELL_INFLUENCE_BLESS
                                    )
                                        * gfCancelDurationMods
                                            [targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLESS)] + fullQuantityFlag
                                                     >= SPELL_AI_MAX_DURATION
                                                 ? SPELL_AI_MAX_DURATION
                                                 : targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLESS)] + fullQuantityFlag]
                                    - effect
                                );
                            }
                        }
                    }
                }
                break;
            case SPELL_ANTI_MAGIC:
                if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_ANTI_MAGIC)]) {
                } else {
                    if (spell == SPELL_ANTI_MAGIC && m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))] == NULL) {
                        effect = 0;
                    } else {
                        effect = static_cast<i32>(
                            RawEffectSpellInfluence(
                                targetCreature,
                                ARMY_SPELL_INFLUENCE_ANTI_MAGIC
                            )
                            * durationFactor
                        );
                        EffectSpellCure(
                            &cureAmount,
                            H2EnumIndex(targetCreature->m_side),
                            targetCreature->m_index,
                            0
                        );
                        effect += cureAmount;
                    }
                }
                break;
            case SPELL_STONE_SKIN:
                if (isMindEffect) {
                } else {
                    if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_STONESKIN)]) {
                    } else {
                        if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_STEELSKIN)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    ARMY_SPELL_INFLUENCE_STONESKIN
                                )
                                * durationFactor
                            );
                            if (m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)] != NULL
                                && targetCreature->m_side == COMBAT_ATTACKER_SIDE
                                && (H2EnumIndex((targetCreature->m_monster.flags.all) & (MONSTER_FLAGS_SHOOTER))))
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
                if (isMindEffect) {
                } else {
                    if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_STONESKIN)]) {
                    } else {
                        if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_STEELSKIN)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    ARMY_SPELL_INFLUENCE_STEELSKIN
                                )
                                * durationFactor
                            );
                            if (m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)] != NULL
                                && targetCreature->m_side == COMBAT_ATTACKER_SIDE
                                && (H2EnumIndex((targetCreature->m_monster.flags.all) & (MONSTER_FLAGS_SHOOTER))))
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
                    if (isMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLOODLUST)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    ARMY_SPELL_INFLUENCE_BLOODLUST
                                )
                                * durationFactor
                            );
                        }
                    }
                }
                break;
            case SPELL_SHIELD:
            case SPELL_MASS_SHIELD:
                if (isMindEffect) {
                } else {
                    if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_SHIELD)]) {
                    } else {
                        effect = static_cast<i32>(
                            RawEffectSpellInfluence(
                                targetCreature,
                                ARMY_SPELL_INFLUENCE_SHIELD
                            )
                            * durationFactor
                        );
                        if (m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)] != NULL
                            && targetCreature->m_side == COMBAT_ATTACKER_SIDE
                            && (H2EnumIndex((targetCreature->m_monster.flags.all) & (MONSTER_FLAGS_SHOOTER))))
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
                    if (isMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLIND)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    ARMY_SPELL_INFLUENCE_BLIND
                                )
                                * durationFactor
                            );
                        }
                    }
                }
                break;
            case SPELL_PARALYZE:
                if (hasDamageReductionResult) {
                } else {
                    if (isMindEffect) {
                    } else {
                        if (targetCreature->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PARALYZE)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    ARMY_SPELL_INFLUENCE_PARALYZE
                                )
                                * durationFactor
                            );
                        }
                    }
                }
                break;
            case SPELL_DRAGON_SLAYER:
                if (hasDamageReductionResult) {
                } else {
                    if (isMindEffect) {
                    } else {
                        if (targetCreature
                                ->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER)]) {
                        } else {
                            effect = static_cast<i32>(
                                RawEffectSpellInfluence(
                                    targetCreature,
                                    ARMY_SPELL_INFLUENCE_DRAGON_SLAYER
                                )
                                * durationFactor
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
                    for (idx = 0; idx < COMBAT_WALL_SECTION_COUNT; idx++) {
                        if (m_wallStates[idx + H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FIRST)]
                                == COMBAT_WALL_STATE_DESTROYED
                            || m_wallStates[idx + H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FIRST)]
                                   == COMBAT_WALL_STATE_SECTION_DESTROYED)
                            wallsDamagedTotal++;
                    }
                    if (wallsDamagedTotal == 0)
                        effect = SPELL_AI_EARTHQUAKE_NO_DAMAGE_SCORE;
                    else if (wallsDamagedTotal < COMBAT_WALL_SECTION_COUNT)
                        effect = (COMBAT_WALL_SECTION_COUNT - wallsDamagedTotal)
                                 * SPELL_AI_EARTHQUAKE_WALL_SCORE;
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

        switch (spellMode) {
            case SPELL_AI_GLOBAL:
            case SPELL_AI_AREA:
            case SPELL_AI_FRIENDLY:
            case SPELL_AI_ENEMY:
            case SPELL_AI_RESURRECT:
            case SPELL_AI_ANY_ARMY:
                sumEffect = effect;
                break;
            case SPELL_AI_SUM_FRIENDLY:
            case SPELL_AI_SUM_ENEMY:
                sumEffect += effect;
        }

        if (spellMode == SPELL_AI_SUM_FRIENDLY
            || spellMode == SPELL_AI_SUM_ENEMY || *bestEffect < sumEffect) {
            *bestEffect = sumEffect;
            *bestHex = hexCell;
        }

        switch (spellMode) {
            case SPELL_AI_GLOBAL:
                bDone = 1;
                break;
            case SPELL_AI_SUM_FRIENDLY:
            case SPELL_AI_SUM_ENEMY:
            case SPELL_AI_FRIENDLY:
            case SPELL_AI_ENEMY:
            case SPELL_AI_ANY_ARMY:
                bDone = FirstArmy(hexCell + 1, team, &hexCell);
                break;
            case SPELL_AI_AREA:
                NextPos(&hexCell);
                if (hexCell > SPELL_AI_LAST_HEX)
                    bDone = 1;
                break;
            case SPELL_AI_RESURRECT:
                bDone = FirstResurrectable(hexCell + 1, &hexCell, spell);
        }
    }
}

i32 combatManager::EffectSpellCreateCreature(i32 hex, SpellType spell) {
    float workChance = COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE;
    i32 spellPowerValue = m_spellPower[H2EnumIndex(m_currentSide)];

    if ((spell == SPELL_SUMMON_EARTH_ELEMENTAL || spell == SPELL_SUMMON_AIR_ELEMENTAL
         || spell == SPELL_SUMMON_FIRE_ELEMENTAL || spell == SPELL_SUMMON_WATER_ELEMENTAL)
        && m_heroes[H2EnumIndex(m_currentSide)] != NULL
        && m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_BOOK_ELEMENTS))
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
            workChance = m_armies[H2EnumIndex(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex]
                             .SpellCastWorkChance(SPELL_MIRROR_IMAGE);
            creatureType = m_armies[H2EnumIndex(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex]
                               .m_monsterType;
            spellPowerValue =
                m_armies[H2EnumIndex(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex].m_quantity
                * SPELL_DEFAULT_CREATURE_POWER;
            break;
    }

    i32 creatureEffect = gMonsterDatabase[H2EnumIndex(creatureType)].fightValue * H2EnumIndex(creatureType);
    if (spell == SPELL_MIRROR_IMAGE) {
        float mirrorMod;
        if (m_spellPower[H2EnumIndex(m_currentSide)] == COMBAT_SPELL_AI_MIRROR_POWER_ONE)
            mirrorMod = COMBAT_SPELL_AI_MIRROR_POWER_ONE_MODIFIER;
        else if (m_spellPower[H2EnumIndex(m_currentSide)] == COMBAT_SPELL_AI_MIRROR_POWER_TWO)
            mirrorMod = COMBAT_SPELL_AI_MIRROR_POWER_TWO_MODIFIER;
        else
            mirrorMod = COMBAT_SPELL_AI_MIRROR_DEFAULT_MODIFIER;
        creatureEffect = static_cast<i32>(creatureEffect * mirrorMod);
        if ((H2EnumIndex((gMonsterDatabase[H2EnumIndex(creatureType)].flags.abilityFlags) & (MONSTER_ABILITY_FLAG_SHOOTER))))
            creatureEffect =
                static_cast<i32>(creatureEffect * COMBAT_SPELL_AI_MIRROR_SHOOTER_MODIFIER);
    }
    return static_cast<i32>(creatureEffect * workChance);
}

i32 combatManager::RawEffectSpellInfluence(army* target, ArmySpellInfluence influence) {
    i32 effect = 0;
    army* otherArmy = NULL;
    float workChance =
        target->SpellCastWorkChance(SpellType(giSpellInfluenceToSpell[H2EnumIndex(influence)]));
    if (workChance <= COMBAT_SPELL_AI_ZERO_EFFECT)
        return 0;

    i32 armyValue = target->m_monster.fightValue * target->m_quantity;
    switch (influence) {
        case ARMY_SPELL_INFLUENCE_HASTE:
        case ARMY_SPELL_INFLUENCE_SLOW: {
            i32 newSpeed;
            if (influence == ARMY_SPELL_INFLUENCE_SLOW) {
                newSpeed = (target->m_monster.speed + 1) >> 1;
            } else {
                newSpeed = target->m_monster.speed + COMBAT_SPELL_AI_HASTE_SPEED_BONUS;
                if ((H2EnumIndex((target->m_monster.flags.all) & (MONSTER_FLAGS_FLYING))))
                    return 0;
            }
            if (m_inCastleCombat && target->m_side == COMBAT_DEFENDER_SIDE)
                return 0;
            if ((H2EnumIndex((target->m_monster.flags.all) & (MONSTER_FLAGS_SHOOTER))))
                return 0;
            i32 attackMask =
                target->GetAttackMask(
                    target->m_hex, ARMY_ATTACK_TARGET_ENEMY, ARMY_HEX_INVALID
                );
            if (attackMask != COMBAT_SPELL_AI_ALL_ATTACK_DIRECTIONS)
                return 0;

            i32 column = target->m_hex % ARMY_HEX_COLUMNS;
            i32 distance;
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                distance = column - COMBAT_SPELL_AI_MINIMUM_DISTANCE;
            else
                distance = COMBAT_SPELL_AI_RIGHT_DISTANCE_COLUMN - column;
            if (distance < 0)
                distance = 0;
            distance += COMBAT_SPELL_AI_MINIMUM_DISTANCE;
            if (m_inCastleCombat)
                distance += COMBAT_SPELL_AI_CASTLE_DISTANCE_BONUS;

            float oldTurns;
            if ((H2EnumIndex((target->m_monster.flags.all) & (MONSTER_FLAGS_FLYING))))
                oldTurns = COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE;
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
        case ARMY_SPELL_INFLUENCE_BLESS:
        case ARMY_SPELL_INFLUENCE_CURSE: {
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
        case ARMY_SPELL_INFLUENCE_BLIND:
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_BLIND_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_BERSERK:
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_BERSERK_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_PARALYZE:
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_PARALYZE_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_HYPNOTIZE:
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_HYPNOTIZE_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_BLOODLUST:
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_BLOODLUST_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_PETRIFIED:
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_PETRIFIED_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_ANTI_MAGIC:
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_ANTI_MAGIC_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_STONESKIN:
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_STONE_SKIN_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_STEELSKIN:
            effect = static_cast<i32>(armyValue * COMBAT_SPELL_AI_STEEL_SKIN_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER: {
            i32 adjacent = 0;
            i32 dragonCount = adjacent;
            i32 i;
            for (i = 0; i < m_armyCount[H2EnumIndex(OppositeCombatSide(target->m_side))]; i++) {
                otherArmy = &m_armies[H2EnumIndex(target->m_side)][i];
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
                dragonMod = COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE;
            else
                dragonMod = static_cast<float>(dragonCount / m_armyCount[H2EnumIndex(OppositeCombatSide(target->m_side))]);
            effect = static_cast<i32>(COMBAT_SPELL_AI_DRAGON_SLAYER_MODIFIER * dragonMod);
            break;
        }
        case ARMY_SPELL_INFLUENCE_SHIELD: {
            i32 shooterCount = 0;
            i32 i;
            for (i = 0; i < m_armyCount[H2EnumIndex(OppositeCombatSide(target->m_side))]; i++) {
                otherArmy = &m_armies[H2EnumIndex(target->m_side)][i];
                if ((H2EnumIndex((otherArmy->m_monster.flags.all) & (MONSTER_FLAGS_SHOOTER))))
                    shooterCount++;
            }
            float shooterMod = static_cast<float>(shooterCount / m_armyCount[H2EnumIndex(OppositeCombatSide(target->m_side))]);
            if (target->m_side == COMBAT_ATTACKER_SIDE && m_inCastleCombat) {
                shooterMod = static_cast<float>(shooterMod + COMBAT_SPELL_AI_SIEGE_SHIELD_BONUS);
                if (shooterMod > COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER)
                    shooterMod = COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE;
            }
            effect = static_cast<i32>(COMBAT_SPELL_AI_SHIELD_MODIFIER * shooterMod);
            break;
        }
        default:
            effect = 0;
            break;
    }

    effect = static_cast<i32>(effect * workChance);
    if ((target->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BERSERK)]
         || target->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])
        && influence != ARMY_SPELL_INFLUENCE_ANTI_MAGIC)
        effect = 0;
    return effect;
}

void combatManager::ClearEffects(void) {
    CombatSide side;
    i32 idx;
    for (side = COMBAT_ATTACKER_SIDE; H2EnumIndex(side) < COMBAT_SIDE_COUNT; side++) {
        for (idx = 0; idx < COMBAT_ARMY_SLOT_COUNT; idx++)
            *(gArmyEffected[0] + H2EnumIndex(side) * COMBAT_ARMY_SLOT_COUNT + idx) = 0;
    }
}

void combatManager::NextPos(i32* hex) {
    if ((*hex + SPELL_AI_HEX_ROW_END_OFFSET) % COMBAT_GRID_ROW_LENGTH == 0)
        *hex += SPELL_AI_HEX_ROW_SKIP;
    else
        (*hex)++;
}

i32 combatManager::FirstArmy(i32 startHex, i32 side, i32* hex) {
    while (startHex <= SPELL_AI_LAST_HEX) {
        if (H2EnumIndex(m_hexCells[startHex].m_occupantSide) == side
            || (side == SPELL_AI_ANY_SIDE && H2EnumIndex(m_hexCells[startHex].m_occupantSide) >= 0)) {
            *hex = startHex;
            return 0;
        }
        NextPos(&startHex);
    }
    *hex = -1;
    return 1;
}

i32 combatManager::FirstResurrectable(
    i32 startHex,
    i32* hex,
    SpellType spell
) {
    while (startHex <= SPELL_AI_LAST_HEX) {
        if (FindResurrectArmyIndex(m_currentSide, spell, startHex) != -1) {
            *hex = startHex;
            return 0;
        }
        NextPos(&startHex);
    }
    *hex = -1;
    return 1;
}

void combatManager::EffectSpellCure(i32* effect, i32 targetSide, i32 targetIndex, i32 cure) {
    i32 sideWork;
    i32 fullQuantityWork;
    i32 armyValueResult;
    i32 negativeEffectResult;
    *effect = 0;
    i32 done = 0;
    H2SteppedEnumStorage<ArmySpellInfluence, i32> influence;
    army* combatTarget;
    i32 curePointsTotal;
    i32 positiveEffectResult;
    i32 index;

    if (targetSide == SPELL_AI_ANY_SIDE)
        sideWork = H2EnumIndex(m_currentSide);
    else
        sideWork = targetSide;

    while (!done) {
        positiveEffectResult = 0;
        negativeEffectResult = 0;
        for (index = 0; index < COMBAT_ARMY_SLOT_COUNT; index++) {
            if (targetIndex != -1 && index != targetIndex)
                continue;
            if (m_armies[sideWork][index].IsAlive()) {
                combatTarget = &m_armies[sideWork][index];
                if (cure == 1) {
                    curePointsTotal =
                        m_spellPower[H2EnumIndex(m_currentSide)] * COMBAT_SPELL_AI_CURE_POINTS_PER_POWER;
                    if (curePointsTotal > combatTarget->m_hitPointsLost)
                        curePointsTotal = combatTarget->m_hitPointsLost;
                    positiveEffectResult = static_cast<i32>(
                        positiveEffectResult
                        + static_cast<float>(
                              gMonsterDatabase[H2EnumIndex(combatTarget->m_monsterType)].fightValue
                          ) * curePointsTotal
                              * COMBAT_SPELL_AI_CURE_VALUE_MODIFIER
                              / combatTarget->m_monster.hitPoints
                    );
                }

                fullQuantityWork =
                    (H2EnumIndex((combatTarget->m_monster.flags.all) & (MONSTER_FLAGS_FULL_AI_QUANTITY))) != 0;
                armyValueResult = gMonsterDatabase[H2EnumIndex(combatTarget->m_monsterType)].fightValue
                                  * combatTarget->m_quantity;
                if ((H2EnumIndex((combatTarget->m_monster.flags.all) & (MONSTER_FLAGS_MIRROR_IMAGE)))) {
                    negativeEffectResult = armyValueResult;
                } else {
                for (influence = ARMY_SPELL_INFLUENCE_HASTE;
                     influence < ARMY_SPELL_INFLUENCE_COUNT;
                     influence++) {
                    if (combatTarget->m_spellInfluence[H2EnumIndex(influence)]) {
                        switch (influence) {
                            case ARMY_SPELL_INFLUENCE_HASTE:
                            case ARMY_SPELL_INFLUENCE_BLESS:
                            case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER:
                            case ARMY_SPELL_INFLUENCE_BLOODLUST:
                            case ARMY_SPELL_INFLUENCE_SHIELD:
                            case ARMY_SPELL_INFLUENCE_STONESKIN:
                            case ARMY_SPELL_INFLUENCE_STEELSKIN:
                                positiveEffectResult = static_cast<i32>(
                                    positiveEffectResult
                                    + RawEffectSpellInfluence(
                                          combatTarget,
                                          influence
                                      )
                                          * gfCancelDurationMods
                                              [combatTarget->m_spellInfluence[H2EnumIndex(influence)]
                                                           + fullQuantityWork
                                                       >= SPELL_AI_MAX_DURATION
                                                   ? SPELL_AI_MAX_DURATION
                                                   : combatTarget->m_spellInfluence[H2EnumIndex(influence)]
                                                         + fullQuantityWork]
                                );
                                break;
                            case ARMY_SPELL_INFLUENCE_ANTI_MAGIC:
                                break;
                            case ARMY_SPELL_INFLUENCE_SLOW:
                            case ARMY_SPELL_INFLUENCE_CURSE:
                            case ARMY_SPELL_INFLUENCE_BLIND:
                            case ARMY_SPELL_INFLUENCE_BERSERK:
                            case ARMY_SPELL_INFLUENCE_PARALYZE:
                            case ARMY_SPELL_INFLUENCE_HYPNOTIZE:
                            case ARMY_SPELL_INFLUENCE_PETRIFIED:
                                negativeEffectResult = static_cast<i32>(
                                    negativeEffectResult
                                    + RawEffectSpellInfluence(
                                          combatTarget,
                                          influence
                                      )
                                          * gfCancelDurationMods
                                              [combatTarget->m_spellInfluence[H2EnumIndex(influence)]
                                                           + fullQuantityWork
                                                       >= SPELL_AI_MAX_DURATION
                                                   ? SPELL_AI_MAX_DURATION
                                                   : combatTarget->m_spellInfluence[H2EnumIndex(influence)]
                                                         + fullQuantityWork]
                                );
                                break;
                        }
                    }
                }
                }
            }
        }
        if (cure == 1)
            positiveEffectResult = 0;
    if (H2EnumIndex(m_currentSide) == sideWork)
            *effect += -negativeEffectResult - positiveEffectResult;
        else
            *effect += positiveEffectResult + negativeEffectResult;
        if (targetSide == SPELL_AI_ANY_SIDE && H2EnumIndex(m_currentSide) == sideWork)
            sideWork = H2EnumIndex(OppositeCombatSide(m_currentSide));
        else
            done = 1;
    }
}

void combatManager::EffectSpellResurrect(i32* effect, i32 hex, SpellType spell) {
    army* targetStack;
    i32 resurrectPowerWork;
    i32 quantityResult[COMBAT_SIDE_COUNT];
    i32 armyIndexWork;

    resurrectPowerWork = m_spellPower[H2EnumIndex(m_currentSide)] * COMBAT_SPELL_AI_RESURRECT_POINTS_PER_POWER;
    if (m_heroes[H2EnumIndex(m_currentSide)] != NULL && m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_ANKH))
        resurrectPowerWork <<= 1;

    armyIndexWork = FindResurrectArmyIndex(m_currentSide, spell, hex);
    targetStack = &m_armies[H2EnumIndex(m_currentSide)][armyIndexWork];
    quantityResult[0] = resurrectPowerWork / targetStack->m_monster.hitPoints;
    if (targetStack->m_quantity + quantityResult[0] > targetStack->m_initialQuantity)
        quantityResult[0] = targetStack->m_initialQuantity - targetStack->m_quantity;
    *effect = gMonsterDatabase[H2EnumIndex(targetStack->m_monsterType)].fightValue * quantityResult[0];
    *effect = static_cast<i32>(*effect * targetStack->SpellCastWorkChance(spell));
    if (spell == SPELL_RESURRECT)
        *effect = static_cast<i32>(*effect * COMBAT_SPELL_AI_RESURRECT_VALUE_MODIFIER);
}

void combatManager::EffectSpellDamage(i32* effect, SpellType spell, i32 targetHex) {
    i32 fightValueKilledAI[COMBAT_SIDE_COUNT];
    i32 creaturesKilledResult;
    i32 remainderResult;
    i32 damagePerPowerResult;
    i32 stacksKilledCandidate[COMBAT_SIDE_COUNT];
    i32 doneWork;
    army* targetCreature;
    i32 killedCombatValue[COMBAT_SIDE_COUNT];
    CombatSide side;
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

    damage = m_spellPower[H2EnumIndex(m_currentSide)] * damagePerPowerResult;
    currentHex = 0;
    step = 0;
    doneWork = 0;
    creaturesKilledResult = 0;
    disruptingRayValueTotal = 0;
    if (m_hexCells[targetHex].m_occupantIndex >= 0)
        targetCreature =
            &m_armies[H2EnumIndex(m_hexCells[targetHex].m_occupantSide)][m_hexCells[targetHex].m_occupantIndex];

    for (side = COMBAT_ATTACKER_SIDE; H2EnumIndex(side) < COMBAT_SIDE_COUNT; side++) {
        stacksKilledCandidate[H2EnumIndex(side)] = 0;
        fightValueKilledAI[H2EnumIndex(side)] = 0;
        killedCombatValue[H2EnumIndex(side)] = 0;
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
                if (step > CHAIN_LIGHTNING_MAX_TARGETS || currentHex == COMBAT_HEX_EMPTY)
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
                if (currentHex < SPELL_AI_LAST_HEX + 1)
                    doneWork = 0;
                else
                    doneWork = 1;
                doneWork = currentHex >= SPELL_AI_LAST_HEX + 1;
                break;
            case SPELL_COLD_RING:
                if (step == 0)
                    step++;
            case SPELL_FIREBALL:
            case SPELL_FIREBLAST:
            case SPELL_METEOR_SHOWER:
                if (((step < SPELL_FIREBLAST_SECOND_RING_FIRST) || spell == SPELL_FIREBLAST)
                    && step < SPELL_FIREBALL_AFFECTED_HEX_COUNT) {
                    if (step == 0)
                        currentHex = targetHex;
                    if (step > 0 && step < SPELL_FIREBLAST_SECOND_RING_FIRST)
                        currentHex = GetAdjacentCellIndexNoArmy(
                            targetHex, static_cast<CombatHexDirection>(step - 1)
                        );
                    if (step >= SPELL_FIREBLAST_SECOND_RING_FIRST
                        && step < SPELL_FIREBLAST_AXIAL_FIRST) {
                        currentHex = GetAdjacentCellIndexNoArmy(
                            targetHex,
                            static_cast<CombatHexDirection>(
                                step - SPELL_FIREBLAST_SECOND_RING_FIRST
                            )
                        );
                        currentHex = GetAdjacentCellIndexNoArmy(
                            currentHex,
                            static_cast<CombatHexDirection>(
                                step - SPELL_FIREBLAST_SECOND_RING_FIRST
                            )
                        );
                    }
                    if (step == SPELL_FIREBLAST_AXIAL_FIRST)
                        currentHex = targetHex - SPELL_FIREBLAST_HEX_ROW_STRIDE;
                    if (step == SPELL_FIREBLAST_AXIAL_SECOND)
                        currentHex = targetHex + SPELL_FIREBLAST_HEX_ROW_STRIDE;
                    if (step == SPELL_FIREBLAST_CORNER_FIRST) {
                        currentHex = GetAdjacentCellIndexNoArmy(
                            targetHex, COMBAT_DIRECTION_EAST
                        );
                        currentHex = GetAdjacentCellIndexNoArmy(
                            currentHex, COMBAT_DIRECTION_NORTHEAST
                        );
                    }
                    if (step == SPELL_FIREBLAST_CORNER_SECOND) {
                        currentHex = GetAdjacentCellIndexNoArmy(
                            targetHex, COMBAT_DIRECTION_EAST
                        );
                        currentHex = GetAdjacentCellIndexNoArmy(
                            currentHex, COMBAT_DIRECTION_SOUTHEAST
                        );
                    }
                    if (step == SPELL_FIREBLAST_CORNER_THIRD) {
                        currentHex = GetAdjacentCellIndexNoArmy(
                            targetHex, COMBAT_DIRECTION_WEST
                        );
                        currentHex = GetAdjacentCellIndexNoArmy(
                            currentHex, COMBAT_DIRECTION_NORTHWEST
                        );
                    }
                    if (step == SPELL_FIREBLAST_CORNER_FOURTH) {
                        currentHex = GetAdjacentCellIndexNoArmy(
                            targetHex, COMBAT_DIRECTION_WEST
                        );
                        currentHex = GetAdjacentCellIndexNoArmy(
                            currentHex, COMBAT_DIRECTION_SOUTHWEST
                        );
                    }
                    step++;
                } else {
                    doneWork = 1;
                }
        }

        if (!doneWork && currentHex >= 0 && currentHex < SPELL_AI_LAST_HEX + 1
            && m_hexCells[currentHex].m_occupantIndex >= 0
            && m_hexCells[currentHex].m_occupantSide != COMBAT_SIDE_NONE) {
            targetCreature = &m_armies[H2EnumIndex(m_hexCells[currentHex].m_occupantSide)]
                                      [m_hexCells[currentHex].m_occupantIndex];
            if (!gArmyEffected[H2EnumIndex(m_hexCells[currentHex].m_occupantSide)]
                              [m_hexCells[currentHex].m_occupantIndex]) {
                gArmyEffected[H2EnumIndex(m_hexCells[currentHex].m_occupantSide)]
                             [m_hexCells[currentHex].m_occupantIndex] = 1;
                workChanceWork = targetCreature->SpellCastWorkChance(spell);
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
                        spell,
                        m_heroes[H2EnumIndex(m_currentSide)],
                        m_heroes[H2EnumIndex(targetCreature->m_side)]
                    );
                    if ((H2EnumIndex((targetCreature->m_monster.flags.all) & (MONSTER_FLAGS_MIRROR_IMAGE)))
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
                        stacksKilledCandidate[H2EnumIndex(m_hexCells[currentHex].m_occupantSide)]++;
                    }

                    fightValueKilledAI[H2EnumIndex(m_hexCells[currentHex].m_occupantSide)] = static_cast<i32>(
                        fightValueKilledAI[H2EnumIndex(m_hexCells[currentHex].m_occupantSide)]
                        + (remainderResult * COMBAT_SPELL_AI_PARTIAL_DAMAGE_MODIFIER
                           + targetCreature->m_monster.hitPoints * creaturesKilledResult)
                              * gMonsterDatabase[H2EnumIndex(targetCreature->m_monsterType)].fightValue
                              / targetCreature->m_monster.hitPoints
                    );
                    killedCombatValue[H2EnumIndex(m_hexCells[currentHex].m_occupantSide)] +=
                        gMonsterDatabase[H2EnumIndex(targetCreature->m_monsterType)].fightValue
                        * targetCreature->m_monster.hitPoints * creaturesKilledResult
                        / targetCreature->m_monster.hitPoints;
                    if ((H2EnumIndex((targetCreature->m_monster.flags.all) & (MONSTER_FLAGS_MIRROR_IMAGE)))) {
                        killedCombatValue[H2EnumIndex(m_hexCells[currentHex].m_occupantSide)] /=
                            SPELL_AI_MIRROR_VALUE_DIVISOR;
                        fightValueKilledAI[H2EnumIndex(m_hexCells[currentHex].m_occupantSide)] /=
                            SPELL_AI_MIRROR_VALUE_DIVISOR;
                    }

                    if (spell == SPELL_DISRUPTING_RAY) {
                        newDefense = targetCreature->m_monster.defense
                                     - SPELL_DISRUPTING_RAY_DEFENSE_REDUCTION;
                        if (newDefense < SPELL_MINIMUM_DEFENSE)
                            newDefense = SPELL_MINIMUM_DEFENSE;
                        disruptingRayValueTotal = static_cast<i32>(
                            disruptingRayValueTotal
                            + gMonsterDatabase[H2EnumIndex(targetCreature->m_monsterType)].fightValue
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
        *effect = fightValueKilledAI[H2EnumIndex(OppositeCombatSide(m_currentSide))] - fightValueKilledAI[H2EnumIndex(m_currentSide)]
                  + disruptingRayValueTotal;
    } else if (killedCombatValue[H2EnumIndex(m_currentSide)] > 0) {
        *effect = killedCombatValue[H2EnumIndex(OppositeCombatSide(m_currentSide))] - killedCombatValue[H2EnumIndex(m_currentSide)]
                  + disruptingRayValueTotal;
    } else {
        *effect = COMBAT_SPELL_AI_DECISIVE_EFFECT - gsSpellInfo[H2EnumIndex(spell)].aiValue
                  + disruptingRayValueTotal;
    }
    if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat)
        *effect = static_cast<i32>(*effect * COMBAT_SPELL_AI_CASTLE_EFFECT_MODIFIER);
}

float gfDurationMods[COMBAT_DURATION_MOD_COUNT] =
    {0.0f, 0.33f, 0.55f, 0.72f, 0.85f, 0.95f, 1.03f, 1.08f, 1.12f, 1.15f, 1.18f, 0.0f};
float gfCancelDurationMods[COMBAT_CANCEL_DURATION_MOD_COUNT] =
    {0.0f, 0.5f, 0.65f, 0.78f, 0.85f, 0.95f, 1.03f, 1.08f, 1.12f, 1.15f, 1.18f};
i32 giCurrSpellGroup;
