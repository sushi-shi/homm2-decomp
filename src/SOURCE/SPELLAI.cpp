#include <va.h>
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

H2_ENUM_CLASS_BEGIN(CombatSpellAITargetMode)
    SPELL_AI_GLOBAL       = 0,
    SPELL_AI_SUM_FRIENDLY = 1,
    SPELL_AI_SUM_ENEMY    = 2,
    SPELL_AI_AREA         = 3,
    SPELL_AI_FRIENDLY     = 4,
    SPELL_AI_ENEMY        = 5,
    SPELL_AI_RESURRECT    = 6,
    SPELL_AI_ANY_ARMY     = 7
H2_ENUM_CLASS_END(CombatSpellAITargetMode)

H2_ENUM_BEGIN(CombatLayoutConstant)
    SPELL_AI_FIRST_HEX                  = 1,
    SPELL_AI_LAST_HEX                   = 0x73,
    SPELL_AI_AREA_LAST_HEX              = 0x2b,
    SPELL_AI_ANY_SIDE                   = 2,
    SPELL_AI_MAX_DURATION               = 10,
    SPELL_AI_MAX_MANA_RATIO             = 10,
    SPELL_AI_EARTHQUAKE_NO_DAMAGE_SCORE = 29999,
    SPELL_AI_EARTHQUAKE_WALL_SCORE      = 100,
    SPELL_AI_HEX_ROW_END_OFFSET         = 2,
    SPELL_AI_HEX_ROW_SKIP               = 3,
    SPELL_AI_MIRROR_VALUE_DIVISOR       = 2
H2_ENUM_END(CombatLayoutConstant)

VA(0x00495da0, 0x214)
i32 combatManager::DoSpellAI(H2_ENUM_PARAM(CombatSide, i32) side, i32 restricted) {
    SpellType chosenSpell;
    i32 effect;
    H2_ENUM_STORAGE_STEPPED(SpellType, i32) spell;
    i32 bestHexWork;
    i32 candHex;
    i32 bestValue;
    i32 manaRatioResult;

    bestValue = 0;
    chosenSpell = SPELL_NONE;
    bestHexWork = -1;

    if (m_heroes[IDX(side)] == NULL)
        return 0;

    for (spell = IDX(COMBAT_ATTACKER_SIDE); IDX(spell) < COMBAT_SIDE_COUNT; spell++) {
        if (m_heroes[IDX(spell)] != NULL
            && m_heroes[IDX(spell)]->HasArtifact(ARTIFACT_SPHERE_NEGATION))
            return 0;
    }

    for (spell = SPELL_FIREBALL; spell < SPELL_COUNT; spell++) {
        if (m_heroes[IDX(side)]->HasSpell(spell)
            && HAS(gsSpellInfo[IDX(spell)].attributes, SPELL_INFO_ATTRIBUTE_COMBAT)
            && GetManaCost(spell, m_heroes[IDX(side)]) <= m_heroes[IDX(side)]->m_spellPoints) {
            if (restricted && spell != SPELL_FIREBALL && spell != SPELL_FIREBLAST
                && spell != SPELL_LIGHTNING_BOLT && spell != SPELL_CHAIN_LIGHTNING
                && spell != SPELL_HOLY_WORD && spell != SPELL_HOLY_SHOUT
                && spell != SPELL_MAGIC_ARROW && spell != SPELL_ARMAGEDDON
                && spell != SPELL_ELEMENTAL_STORM && spell != SPELL_METEOR_SHOWER
                && spell != SPELL_COLD_RAY && spell != SPELL_COLD_RING
                && spell != SPELL_DEATH_RIPPLE && spell != SPELL_DEATH_WAVE)
                continue;
            DetermineEffectOfSpell(spell, &effect, &candHex);

            manaRatioResult =
                m_heroes[IDX(side)]->m_spellPoints / GetManaCost(spell, m_heroes[IDX(side)]);
            if (manaRatioResult > SPELL_AI_MAX_MANA_RATIO)
                manaRatioResult = SPELL_AI_MAX_MANA_RATIO;
            effect = static_cast<i32>(effect * gfSpellCastableCombatMod[manaRatioResult]);

            if (effect > bestValue) {
                bestValue = effect;
                chosenSpell = spell;
                bestHexWork = candHex;
            }
        }
    }

    if (bestValue > 0) {
        giNextAction = ACTION_CAST_SPELL;
        giNextActionExtra = IDX(chosenSpell);
        giNextActionGridIndex = bestHexWork;
        return 1;
    }
    return 0;
}

VA(0x00495fb4, 0xf4e)
void combatManager::DetermineEffectOfSpell(SpellType spell, i32* bestEffect, i32* bestHex) {
    i32 durMax_29;
    i32 wallsDamagedTotal;
    i32 spellPowerWork;
    i32 hexCell_9;
    i32 hasDamageReductionResult;
    i32 effect_8;
    army* targetCreature;
    CombatSpellAITargetMode spellMode;
    i32 bDone;
    i32 team_9;
    float durationFactor_16;
    i32 isMindEffect_13;
    i32 fullQuantityFlag_4;
    i32 cureAmount_7;
    i32 idx_3;
    i32 sumEffect_9;

    bDone = 0;
    team_9 = 0;
    hexCell_9 = SPELL_AI_FIRST_HEX;
    durationFactor_16 = COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE;
    fullQuantityFlag_4 = 1;
    sumEffect_9 = 0;
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
            team_9 = IDX(m_currentSide);
            break;
        case SPELL_MASS_SLOW:
        case SPELL_MASS_CURSE:
            spellMode = SPELL_AI_SUM_ENEMY;
            team_9 = IDX(OppositeCombatSide(m_currentSide));
            break;
        case SPELL_DISPEL:
            spellMode = SPELL_AI_ANY_ARMY;
            team_9 = SPELL_AI_ANY_SIDE;
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
            team_9 = IDX(m_currentSide);
            break;
        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_ANIMATE_DEAD:
            spellMode = SPELL_AI_RESURRECT;
            team_9 = IDX(m_currentSide);
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
        case SPELL_COLD_RAY:
        case SPELL_DISRUPTING_RAY:
            spellMode = SPELL_AI_ENEMY;
            team_9 = IDX(OppositeCombatSide(m_currentSide));
            break;
        default:
            *bestEffect = 0;
            return;
    }

    if (spellMode == SPELL_AI_RESURRECT)
        bDone = FirstResurrectable(SPELL_AI_FIRST_HEX, &hexCell_9, spell);
    if (spellMode == SPELL_AI_FRIENDLY || spellMode == SPELL_AI_ENEMY
        || spellMode == SPELL_AI_SUM_ENEMY
        || spellMode == SPELL_AI_SUM_FRIENDLY
        || spellMode == SPELL_AI_ANY_ARMY)
        bDone = FirstArmy(SPELL_AI_FIRST_HEX, team_9, &hexCell_9);

    while (!bDone) {
        hasDamageReductionResult = 0;
        isMindEffect_13 = 0;
        effect_8 = 0;

        if (m_hexCells[hexCell_9].m_occupantIndex >= 0
            && m_hexCells[hexCell_9].m_occupantSide >= COMBAT_SIDE_VALID_BEGIN) {
            targetCreature = &m_armies[IDX(m_hexCells[hexCell_9].m_occupantSide)]
                                      [m_hexCells[hexCell_9].m_occupantIndex];
            giCurrSpellGroup = IDX(m_hexCells[hexCell_9].m_occupantSide);
            fullQuantityFlag_4 =
                HAS(targetCreature->m_monster.flags.all, MONSTER_FLAGS_FULL_AI_QUANTITY) != 0;

            spellPowerWork = m_spellPower[IDX(m_currentSide)];
            if (m_heroes[IDX(m_currentSide)]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS))
                spellPowerWork += SPELL_HOURGLASS_POWER_BONUS;
            if (m_heroes[IDX(m_currentSide)]->HasArtifact(ARTIFACT_WIZARD_HAT))
                spellPowerWork += SPELL_WIZARD_HAT_POWER_BONUS;

            durationFactor_16 = gfDurationMods
                [spellPowerWork - fullQuantityFlag_4 < SPELL_AI_MAX_DURATION
                     ? spellPowerWork - fullQuantityFlag_4
                     : SPELL_AI_MAX_DURATION];

            if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]
                || targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)])
                isMindEffect_13 = 1;
            if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)]
                || targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)]
                || targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)])
                hasDamageReductionResult = 1;
        } else {
            targetCreature = NULL;
        }

        switch (spell) {
            case SPELL_CURE:
                EffectSpellCure(&effect_8, IDX(m_currentSide), hexCell_9, 1);
                break;
            case SPELL_MASS_CURE:
                EffectSpellCure(&effect_8, IDX(m_currentSide), -1, 1);
                break;
            case SPELL_DISPEL:
                EffectSpellCure(&effect_8, IDX(targetCreature->m_side), targetCreature->m_index, 0);
                break;
            case SPELL_MASS_DISPEL:
                EffectSpellCure(&effect_8, SPELL_AI_ANY_SIDE, -1, 0);
                break;
            case SPELL_RESURRECT:
            case SPELL_TRUE_RESURRECT:
            case SPELL_ANIMATE_DEAD:
                if (isMindEffect_13)
                    break;
                EffectSpellResurrect(&effect_8, hexCell_9, spell);
                break;
            case SPELL_MIRROR_IMAGE:
            case SPELL_SUMMON_EARTH_ELEMENTAL:
            case SPELL_SUMMON_AIR_ELEMENTAL:
            case SPELL_SUMMON_FIRE_ELEMENTAL:
            case SPELL_SUMMON_WATER_ELEMENTAL:
                effect_8 = EffectSpellCreateCreature(hexCell_9, spell);
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
                if (isMindEffect_13)
                    break;
                EffectSpellDamage(&effect_8, spell, hexCell_9);
                if (hasDamageReductionResult)
                    effect_8 = static_cast<i32>(effect_8 * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER);
                break;
            case SPELL_HASTE:
            case SPELL_MASS_HASTE:
                if (hasDamageReductionResult)
                    break;
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HASTE)])
                    break;
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_HASTE
                    )
                    * durationFactor_16
                );
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_SLOW)]) {
                    effect_8 = static_cast<i32>(
                        effect_8
                        - RawEffectSpellInfluence(targetCreature, ARMY_SPELL_INFLUENCE_SLOW)
                              * gfCancelDurationMods
                                  [targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_SLOW)] + fullQuantityFlag_4
                                         < SPELL_AI_MAX_DURATION
                                     ? targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_SLOW)] + fullQuantityFlag_4
                                     : SPELL_AI_MAX_DURATION]
                    );
                }
                break;
            case SPELL_BERSERKER:
                if (hasDamageReductionResult)
                    break;
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)])
                    break;
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_BERSERK
                    )
                    * durationFactor_16
                );
                if (targetCreature
                        ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]) {
                    effect_8 = static_cast<i32>(
                        effect_8
                        - RawEffectSpellInfluence(targetCreature, ARMY_SPELL_INFLUENCE_HYPNOTIZE)
                              * gfCancelDurationMods
                                  [targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)] + fullQuantityFlag_4
                                         < SPELL_AI_MAX_DURATION
                                     ? targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)] + fullQuantityFlag_4
                                     : SPELL_AI_MAX_DURATION]
                    );
                }
                break;
            case SPELL_HYPNOTIZE:
                if (hasDamageReductionResult)
                    break;
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])
                    break;
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_HYPNOTIZE
                    )
                    * durationFactor_16
                );
                if (targetCreature
                        ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]) {
                    effect_8 = static_cast<i32>(
                        effect_8
                        - RawEffectSpellInfluence(targetCreature, ARMY_SPELL_INFLUENCE_BERSERK)
                              * gfCancelDurationMods
                                  [targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] + fullQuantityFlag_4
                                         < SPELL_AI_MAX_DURATION
                                     ? targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] + fullQuantityFlag_4
                                     : SPELL_AI_MAX_DURATION]
                    );
                }
                break;
            case SPELL_SLOW:
            case SPELL_MASS_SLOW:
                if (hasDamageReductionResult)
                    break;
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_SLOW)])
                    break;
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_SLOW
                    )
                    * durationFactor_16
                );
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HASTE)]) {
                    effect_8 = static_cast<i32>(
                        effect_8
                        - RawEffectSpellInfluence(targetCreature, ARMY_SPELL_INFLUENCE_HASTE)
                              * gfCancelDurationMods
                                  [targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HASTE)] + fullQuantityFlag_4
                                         < SPELL_AI_MAX_DURATION
                                     ? targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HASTE)] + fullQuantityFlag_4
                                     : SPELL_AI_MAX_DURATION]
                    );
                }
                break;
            case SPELL_BLESS:
            case SPELL_MASS_BLESS:
                if (hasDamageReductionResult)
                    break;
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLESS)])
                    break;
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_BLESS
                    )
                    * durationFactor_16
                );
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_CURSE)]) {
                    effect_8 = static_cast<i32>(
                        effect_8
                        - RawEffectSpellInfluence(targetCreature, ARMY_SPELL_INFLUENCE_CURSE)
                              * gfCancelDurationMods
                                  [targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_CURSE)] + fullQuantityFlag_4
                                         < SPELL_AI_MAX_DURATION
                                     ? targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_CURSE)] + fullQuantityFlag_4
                                     : SPELL_AI_MAX_DURATION]
                    );
                }
                break;
            case SPELL_CURSE:
            case SPELL_MASS_CURSE:
                if (hasDamageReductionResult)
                    break;
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_CURSE)])
                    break;
                effect_8 = static_cast<i32>(
                    -RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_CURSE
                    )
                    * durationFactor_16
                );
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLESS)]) {
                    effect_8 = static_cast<i32>(
                        effect_8
                        - RawEffectSpellInfluence(targetCreature, ARMY_SPELL_INFLUENCE_BLESS)
                              * gfCancelDurationMods
                                  [targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLESS)] + fullQuantityFlag_4
                                         < SPELL_AI_MAX_DURATION
                                     ? targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLESS)] + fullQuantityFlag_4
                                     : SPELL_AI_MAX_DURATION]
                    );
                }
                break;
            case SPELL_ANTI_MAGIC:
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_ANTI_MAGIC)])
                    break;
                if (spell == SPELL_ANTI_MAGIC
                    && m_heroes[IDX(OppositeCombatSide(m_currentSide))] == NULL) {
                    effect_8 = 0;
                    break;
                }
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_ANTI_MAGIC
                    )
                    * durationFactor_16
                );
                EffectSpellCure(
                    &cureAmount_7,
                    IDX(targetCreature->m_side),
                    targetCreature->m_index,
                    0
                );
                effect_8 += cureAmount_7;
                break;
            case SPELL_STONE_SKIN:
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STONESKIN)])
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STEELSKIN)])
                    break;
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_STONESKIN
                    )
                    * durationFactor_16
                );
                if (m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)] != NULL
                    && targetCreature->m_side == COMBAT_ATTACKER_SIDE
                    && HAS(targetCreature->m_monster.flags.all, MONSTER_FLAGS_SHOOTER))
                    effect_8 = static_cast<i32>(
                        effect_8 * COMBAT_SPELL_AI_SIEGE_SHOOTER_MODIFIER
                    );
                if (hasDamageReductionResult)
                    effect_8 = static_cast<i32>(
                        effect_8 * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER
                    );
                break;
            case SPELL_STEEL_SKIN:
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STONESKIN)])
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STEELSKIN)])
                    break;
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_STEELSKIN
                    )
                    * durationFactor_16
                );
                if (m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)] != NULL
                    && targetCreature->m_side == COMBAT_ATTACKER_SIDE
                    && HAS(targetCreature->m_monster.flags.all, MONSTER_FLAGS_SHOOTER))
                    effect_8 = static_cast<i32>(
                        effect_8 * COMBAT_SPELL_AI_SIEGE_SHOOTER_MODIFIER
                    );
                if (hasDamageReductionResult)
                    effect_8 = static_cast<i32>(
                        effect_8 * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER
                    );
                break;
            case SPELL_BLOOD_LUST:
                if (hasDamageReductionResult)
                    break;
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLOODLUST)])
                    break;
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_BLOODLUST
                    )
                    * durationFactor_16
                );
                break;
            case SPELL_SHIELD:
            case SPELL_MASS_SHIELD:
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_SHIELD)])
                    break;
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_SHIELD
                    )
                    * durationFactor_16
                );
                if (m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)] != NULL
                    && targetCreature->m_side == COMBAT_ATTACKER_SIDE
                    && HAS(targetCreature->m_monster.flags.all, MONSTER_FLAGS_SHOOTER))
                    effect_8 <<= 1;
                if (hasDamageReductionResult)
                    effect_8 =
                        static_cast<i32>(effect_8 * COMBAT_SPELL_AI_REDUCED_EFFECT_MODIFIER);
                break;
            case SPELL_BLIND:
                if (hasDamageReductionResult)
                    break;
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)])
                    break;
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_BLIND
                    )
                    * durationFactor_16
                );
                break;
            case SPELL_PARALYZE:
                if (hasDamageReductionResult)
                    break;
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)])
                    break;
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_PARALYZE
                    )
                    * durationFactor_16
                );
                break;
            case SPELL_DRAGON_SLAYER:
                if (hasDamageReductionResult)
                    break;
                if (isMindEffect_13)
                    break;
                if (targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER)])
                    break;
                effect_8 = static_cast<i32>(
                    RawEffectSpellInfluence(
                        targetCreature,
                        ARMY_SPELL_INFLUENCE_DRAGON_SLAYER
                    )
                    * durationFactor_16
                );
                break;
            case SPELL_TELEPORT:
                effect_8 = 0;
                break;
            case SPELL_EARTHQUAKE:
                if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0) {
                    wallsDamagedTotal = 0;
                    for (idx_3 = 0; idx_3 < COMBAT_WALL_SECTION_COUNT; idx_3++) {
                        if (m_wallStates[idx_3 + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                                == COMBAT_WALL_STATE_DESTROYED
                            || m_wallStates[idx_3 + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                                   == COMBAT_WALL_STATE_SECTION_DESTROYED)
                            wallsDamagedTotal++;
                    }
                    if (wallsDamagedTotal != 0) {
                        if (wallsDamagedTotal < COMBAT_WALL_SECTION_COUNT)
                            effect_8 = (COMBAT_WALL_SECTION_COUNT - wallsDamagedTotal)
                                     * SPELL_AI_EARTHQUAKE_WALL_SCORE;
                        else
                            effect_8 = 0;
                    } else {
                        effect_8 = SPELL_AI_EARTHQUAKE_NO_DAMAGE_SCORE;
                    }
                } else {
                    effect_8 = 0;
                }
                break;
            default:
                *bestEffect = 0;
                return;
        }

        switch (spellMode) {
            case SPELL_AI_SUM_FRIENDLY:
            case SPELL_AI_SUM_ENEMY:
                sumEffect_9 += effect_8;
                break;
            case SPELL_AI_GLOBAL:
            case SPELL_AI_AREA:
            case SPELL_AI_FRIENDLY:
            case SPELL_AI_ENEMY:
            case SPELL_AI_RESURRECT:
            case SPELL_AI_ANY_ARMY:
                sumEffect_9 = effect_8;
        }

        if (spellMode == SPELL_AI_SUM_FRIENDLY
            || spellMode == SPELL_AI_SUM_ENEMY || sumEffect_9 > *bestEffect) {
            *bestEffect = sumEffect_9;
            *bestHex = hexCell_9;
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
                bDone = FirstArmy(hexCell_9 + 1, team_9, &hexCell_9);
                break;
            case SPELL_AI_RESURRECT:
                bDone = FirstResurrectable(hexCell_9 + 1, &hexCell_9, spell);
                break;
            case SPELL_AI_AREA:
                NextPos(&hexCell_9);
                if (hexCell_9 > SPELL_AI_AREA_LAST_HEX)
                    bDone = 1;
        }
    }
}

VA(0x00496f02, 0x26d)
i32 combatManager::EffectSpellCreateCreature(i32 hex, SpellType spell) {
    float workChance = COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE;
    i32 spellPower = m_spellPower[IDX(m_currentSide)];

    if ((spell == SPELL_SUMMON_EARTH_ELEMENTAL || spell == SPELL_SUMMON_AIR_ELEMENTAL
         || spell == SPELL_SUMMON_FIRE_ELEMENTAL || spell == SPELL_SUMMON_WATER_ELEMENTAL)
        && m_heroes[IDX(m_currentSide)] != NULL
        && m_heroes[IDX(m_currentSide)]->HasArtifact(ARTIFACT_BOOK_ELEMENTS))
        spellPower <<= 1;

    if ((spell == SPELL_SUMMON_EARTH_ELEMENTAL || spell == SPELL_SUMMON_AIR_ELEMENTAL
         || spell == SPELL_SUMMON_FIRE_ELEMENTAL || spell == SPELL_SUMMON_WATER_ELEMENTAL)
        && !SpaceForElementalExists())
        return 0;

    CreatureType monType;
    switch (spell) {
        case SPELL_SUMMON_EARTH_ELEMENTAL:
            monType = CREATURE_EARTH_ELEMENTAL;
            break;
        case SPELL_SUMMON_AIR_ELEMENTAL:
            monType = CREATURE_AIR_ELEMENTAL;
            break;
        case SPELL_SUMMON_FIRE_ELEMENTAL:
            monType = CREATURE_FIRE_ELEMENTAL;
            break;
        case SPELL_SUMMON_WATER_ELEMENTAL:
            monType = CREATURE_WATER_ELEMENTAL;
            break;
        default:
            workChance = m_armies[IDX(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex]
                             .SpellCastWorkChance(SPELL_MIRROR_IMAGE);
            monType = m_armies[IDX(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex]
                               .m_monsterType;
            spellPower =
                m_armies[IDX(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex].m_quantity
                * SPELL_DEFAULT_CREATURE_POWER;
            break;
    }

    i32 creatureEffect = IDX(monType) * gMonsterDatabase[IDX(monType)].fightValue;
    if (spell == SPELL_MIRROR_IMAGE) {
        float mirrorMod;
        if (m_spellPower[IDX(m_currentSide)] == COMBAT_SPELL_AI_MIRROR_POWER_ONE)
            mirrorMod = COMBAT_SPELL_AI_MIRROR_POWER_ONE_MODIFIER;
        else if (m_spellPower[IDX(m_currentSide)] == COMBAT_SPELL_AI_MIRROR_POWER_TWO)
            mirrorMod = COMBAT_SPELL_AI_MIRROR_POWER_TWO_MODIFIER;
        else
            mirrorMod = COMBAT_SPELL_AI_MIRROR_DEFAULT_MODIFIER;
        creatureEffect = static_cast<i32>(creatureEffect * mirrorMod);
        if (HAS(
                gMonsterDatabase[IDX(monType)].flags.abilityFlags,
                MONSTER_ABILITY_FLAG_SHOOTER
            ))
            creatureEffect =
                static_cast<i32>(creatureEffect * COMBAT_SPELL_AI_MIRROR_SHOOTER_MODIFIER);
    }
    return static_cast<i32>(creatureEffect * workChance);
}

VA(0x0049716f, 0x5b5)
i32 combatManager::RawEffectSpellInfluence(army* target, ArmySpellInfluence influence) {
    i32 effect = 0;
    float damageDelta;
    i32 unused38_h;
    i32 newSpd;
    i32 unused30_j;
    army* other = NULL;
    float castChance =
        target->SpellCastWorkChance(SpellType(giSpellInfluenceToSpell[IDX(influence)]));
    if (castChance <= COMBAT_SPELL_AI_ZERO_EFFECT)
        return 0;

    i32 cnt;
    i32 worth = target->m_quantity * target->m_monster.fightValue;
    i32 columnIndex;
    float avgDmg;
    float factor;
    i32 distance;
    float afterTurns;
    float beforeTurns;
    i32 unused48_e;
    i32 shooters;
    i32 adjacent;
    i32 dragonCounter;
    i32 attackMask;
    switch (influence) {
        case ARMY_SPELL_INFLUENCE_SLOW:
            newSpd = (target->m_monster.speed + 1) >> 1;
            goto hasteSlowCommon;
        case ARMY_SPELL_INFLUENCE_HASTE:
            newSpd = target->m_monster.speed + COMBAT_SPELL_AI_HASTE_SPEED_BONUS;
            if (HAS(target->m_monster.flags.all, MONSTER_FLAGS_FLYING))
                return 0;
        hasteSlowCommon:
            if (m_inCastleCombat && target->m_side == COMBAT_DEFENDER_SIDE)
                return 0;
            if (HAS(target->m_monster.flags.all, MONSTER_FLAGS_SHOOTER))
                return 0;
            attackMask =
                target->GetAttackMask(target->m_hex, ARMY_ATTACK_TARGET_ENEMY, ARMY_HEX_INVALID);
            if (attackMask != COMBAT_SPELL_AI_ALL_ATTACK_DIRECTIONS)
                return 0;

            columnIndex = target->m_hex % ARMY_HEX_COLUMNS;
            distance = m_currentSide == COMBAT_ATTACKER_SIDE
                           ? columnIndex - COMBAT_SPELL_AI_MINIMUM_DISTANCE
                           : COMBAT_SPELL_AI_RIGHT_DISTANCE_COLUMN - columnIndex;
            if (distance < 0)
                distance = 0;
            distance += COMBAT_SPELL_AI_MINIMUM_DISTANCE;
            if (m_inCastleCombat)
                distance += COMBAT_SPELL_AI_CASTLE_DISTANCE_BONUS;

            if (HAS(target->m_monster.flags.all, MONSTER_FLAGS_FLYING))
                beforeTurns = COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE;
            else
                beforeTurns =
                    static_cast<float>(distance) / (static_cast<float>(target->m_monster.speed));
            afterTurns = static_cast<float>(distance) / (static_cast<float>(newSpd));
            if (afterTurns > COMBAT_SPELL_AI_TURN_CAP)
                afterTurns = COMBAT_SPELL_AI_TURN_CAP;
            if (beforeTurns > COMBAT_SPELL_AI_TURN_CAP)
                beforeTurns = COMBAT_SPELL_AI_TURN_CAP;
            effect = static_cast<i32>(
                (beforeTurns - afterTurns) / COMBAT_SPELL_AI_TURN_DIVISOR * worth
            );
            break;
        case ARMY_SPELL_INFLUENCE_BLESS:
        case ARMY_SPELL_INFLUENCE_CURSE:
            avgDmg = (static_cast<float>(target->m_monster.damageMax)
                      + (static_cast<float>(target->m_monster.damageMin)))
                   * COMBAT_SPELL_AI_AVERAGE_DAMAGE_MODIFIER;
            damageDelta = static_cast<float>(
                (target->m_monster.damageMax - avgDmg) / avgDmg * worth
                * COMBAT_SPELL_AI_BLESS_CURSE_MODIFIER
            );
            effect = static_cast<i32>(
                influence == ARMY_SPELL_INFLUENCE_BLESS ? damageDelta : -damageDelta
            );
            break;
        case ARMY_SPELL_INFLUENCE_BLIND:
            effect = static_cast<i32>(worth * COMBAT_SPELL_AI_BLIND_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_BERSERK:
            effect = static_cast<i32>(worth * COMBAT_SPELL_AI_BERSERK_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_PARALYZE:
            effect = static_cast<i32>(worth * COMBAT_SPELL_AI_PARALYZE_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_HYPNOTIZE:
            effect = static_cast<i32>(worth * COMBAT_SPELL_AI_HYPNOTIZE_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_BLOODLUST:
            effect = static_cast<i32>(worth * COMBAT_SPELL_AI_BLOODLUST_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_PETRIFIED:
            effect = static_cast<i32>(worth * COMBAT_SPELL_AI_PETRIFIED_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_STONESKIN:
            effect = static_cast<i32>(worth * COMBAT_SPELL_AI_STONE_SKIN_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_STEELSKIN:
            effect = static_cast<i32>(worth * COMBAT_SPELL_AI_STEEL_SKIN_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_ANTI_MAGIC:
            effect = static_cast<i32>(worth * COMBAT_SPELL_AI_ANTI_MAGIC_MODIFIER);
            break;
        case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER:
            adjacent = 0;
            dragonCounter = adjacent;
            for (cnt = 0; cnt < m_armyCount[IDX(OppositeCombatSide(target->m_side))]; cnt++) {
                other = &m_armies[IDX(target->m_side)][cnt];
                if (other->m_monsterType == CREATURE_GREEN_DRAGON
                    || other->m_monsterType == CREATURE_RED_DRAGON
                    || other->m_monsterType == CREATURE_BLACK_DRAGON
                    || other->m_monsterType == CREATURE_BONE_DRAGON) {
                    dragonCounter++;
                    if (target->OtherArmyAdjacent(other->m_side, other->m_index))
                        adjacent = 1;
                }
            }
            if (adjacent)
                factor = COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE;
            else
                factor = static_cast<float>(
                    dragonCounter / m_armyCount[IDX(OppositeCombatSide(target->m_side))]
                );
            effect = static_cast<i32>(COMBAT_SPELL_AI_DRAGON_SLAYER_MODIFIER * factor);
            break;
        case ARMY_SPELL_INFLUENCE_SHIELD:
            shooters = 0;
            for (cnt = 0; cnt < m_armyCount[IDX(OppositeCombatSide(target->m_side))]; cnt++) {
                other = &m_armies[IDX(target->m_side)][cnt];
                if (HAS(other->m_monster.flags.all, MONSTER_FLAGS_SHOOTER))
                    shooters++;
            }
            factor = static_cast<float>(
                shooters / m_armyCount[IDX(OppositeCombatSide(target->m_side))]
            );
            if (target->m_side == COMBAT_ATTACKER_SIDE && m_inCastleCombat) {
                factor = static_cast<float>(factor + COMBAT_SPELL_AI_SIEGE_SHIELD_BONUS);
                if (factor > COMBAT_SPELL_AI_FULL_EFFECT_MODIFIER)
                    factor = COMBAT_SPELL_AI_FULL_EFFECT_IMMEDIATE;
            }
            effect = static_cast<i32>(COMBAT_SPELL_AI_SHIELD_MODIFIER * factor);
            break;
        default:
            effect = 0;
            break;
    }

    effect = static_cast<i32>(effect * castChance);
    if ((target->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]
         || target->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])
        && influence != ARMY_SPELL_INFLUENCE_ANTI_MAGIC)
        effect = 0;
    return effect;
}

VA(0x00497724, 0x52)
void combatManager::ClearEffects(void) {
    CombatSide side;
    i32 idx;
    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; side++) {
        for (idx = 0; idx < COMBAT_ARMY_SLOT_COUNT; idx++)
            *(gArmyEffected[0] + IDX(side) * COMBAT_ARMY_SLOT_COUNT + idx) = 0;
    }
}

VA(0x00497776, 0x3d)
void combatManager::NextPos(i32* hex) {
    if ((*hex + SPELL_AI_HEX_ROW_END_OFFSET) % COMBAT_GRID_ROW_LENGTH == 0)
        *hex += SPELL_AI_HEX_ROW_SKIP;
    else
        (*hex)++;
}

VA(0x004977b3, 0x6c)
i32 combatManager::FirstArmy(i32 startHex, i32 side, i32* hex) {
    while (startHex <= SPELL_AI_LAST_HEX) {
        if (IDX(m_hexCells[startHex].m_occupantSide) == side
            || (side == SPELL_AI_ANY_SIDE && IDX(m_hexCells[startHex].m_occupantSide) >= 0)) {
            *hex = startHex;
            return 0;
        }
        NextPos(&startHex);
    }
    *hex = -1;
    return 1;
}

VA(0x0049781f, 0x5a)
i32 combatManager::FirstResurrectable(
    i32 startHex,
    i32* hex,
    H2_ENUM_PARAM(SpellType, i32) spell
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

VA(0x00497879, 0x36b)
void combatManager::EffectSpellCure(i32* effect, i32 targetSide, i32 targetIndex, i32 cure) {
    i32 sideWork_6;
    i32 fullQuantityWork;
    i32 armyValueResult_3;
    i32 negativeEffectResult;
    *effect = 0;
    i32 done_11 = 0;
    H2_ENUM_STORAGE_STEPPED(ArmySpellInfluence, i32) influence_9;
    army* combatTarget;
    i32 curePointsTotal;
    i32 positiveEffectResult_19;
    i32 index_1;

    if (targetSide == SPELL_AI_ANY_SIDE)
        sideWork_6 = IDX(m_currentSide);
    else
        sideWork_6 = targetSide;

    while (!done_11) {
        positiveEffectResult_19 = 0;
        negativeEffectResult = 0;
        for (index_1 = 0; index_1 < COMBAT_ARMY_SLOT_COUNT; index_1++) {
            if (targetIndex != -1 && targetIndex != index_1)
                continue;
            if (m_armies[sideWork_6][index_1].IsAlive()) {
                combatTarget = &m_armies[sideWork_6][index_1];
                if (cure == 1) {
                    curePointsTotal =
                        m_spellPower[IDX(m_currentSide)] * COMBAT_SPELL_AI_CURE_POINTS_PER_POWER;
                    if (curePointsTotal > combatTarget->m_hitPointsLost)
                        curePointsTotal = combatTarget->m_hitPointsLost;
                    positiveEffectResult_19 = static_cast<i32>(
                        positiveEffectResult_19
                        + static_cast<float>(curePointsTotal) * COMBAT_SPELL_AI_CURE_VALUE_MODIFIER
                              * gMonsterDatabase[IDX(combatTarget->m_monsterType)].fightValue
                              / combatTarget->m_monster.hitPoints
                    );
                }

                fullQuantityWork =
                    HAS(combatTarget->m_monster.flags.all, MONSTER_FLAGS_FULL_AI_QUANTITY) != 0;
                armyValueResult_3 = combatTarget->m_quantity
                                  * gMonsterDatabase[IDX(combatTarget->m_monsterType)].fightValue;
                if (HAS(combatTarget->m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)) {
                    negativeEffectResult = armyValueResult_3;
                } else {
                for (influence_9 = ARMY_SPELL_INFLUENCE_HASTE;
                     influence_9 < ARMY_SPELL_INFLUENCE_COUNT;
                     influence_9++) {
                    if (combatTarget->m_spellInfluence[IDX(influence_9)]) {
                        switch (influence_9) {
                            case ARMY_SPELL_INFLUENCE_HASTE:
                            case ARMY_SPELL_INFLUENCE_BLESS:
                            case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER:
                            case ARMY_SPELL_INFLUENCE_BLOODLUST:
                            case ARMY_SPELL_INFLUENCE_SHIELD:
                            case ARMY_SPELL_INFLUENCE_STONESKIN:
                            case ARMY_SPELL_INFLUENCE_STEELSKIN:
                                positiveEffectResult_19 = static_cast<i32>(
                                    positiveEffectResult_19
                                    + RawEffectSpellInfluence(
                                          combatTarget,
                                          influence_9
                                      )
                                          * gfCancelDurationMods
                                              [combatTarget->m_spellInfluence[IDX(influence_9)]
                                                           + fullQuantityWork
                                                       < SPELL_AI_MAX_DURATION
                                                   ? combatTarget->m_spellInfluence[IDX(influence_9)]
                                                         + fullQuantityWork
                                                   : SPELL_AI_MAX_DURATION]
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
                                          influence_9
                                      )
                                          * gfCancelDurationMods
                                              [combatTarget->m_spellInfluence[IDX(influence_9)]
                                                           + fullQuantityWork
                                                       < SPELL_AI_MAX_DURATION
                                                   ? combatTarget->m_spellInfluence[IDX(influence_9)]
                                                         + fullQuantityWork
                                                   : SPELL_AI_MAX_DURATION]
                                );
                                break;
                        }
                    }
                }
                }
            }
        }
        if (cure == 1)
            positiveEffectResult_19 = 0;
    if (sideWork_6 == IDX(m_currentSide))
            *effect += -negativeEffectResult - positiveEffectResult_19;
        else
            *effect += positiveEffectResult_19 + negativeEffectResult;
        if (targetSide == SPELL_AI_ANY_SIDE && sideWork_6 == IDX(m_currentSide))
            sideWork_6 = IDX(OppositeCombatSide(m_currentSide));
        else
            done_11 = 1;
    }
}

VA(0x00497be4, 0x13f)
void combatManager::EffectSpellResurrect(i32* effect, i32 hex, SpellType spell) {
    army* target;
    i32 resurrectPower;
    i32 armyIndex;
    i32 count;
    float workChance;

    resurrectPower = m_spellPower[IDX(m_currentSide)] * COMBAT_SPELL_AI_RESURRECT_POINTS_PER_POWER;
    if (m_heroes[IDX(m_currentSide)] != NULL && m_heroes[IDX(m_currentSide)]->HasArtifact(ARTIFACT_ANKH))
        resurrectPower <<= 1;

    armyIndex = FindResurrectArmyIndex(m_currentSide, spell, hex);
    target = &m_armies[IDX(m_currentSide)][armyIndex];
    count = resurrectPower / target->m_monster.hitPoints;
    if (count + target->m_quantity > target->m_initialQuantity)
        count = target->m_initialQuantity - target->m_quantity;
    *effect = count * gMonsterDatabase[IDX(target->m_monsterType)].fightValue;
    *effect = static_cast<i32>(*effect * target->SpellCastWorkChance(spell));
    if (spell == SPELL_RESURRECT)
        *effect = static_cast<i32>(*effect * COMBAT_SPELL_AI_RESURRECT_VALUE_MODIFIER);
}

VA(0x00497d23, 0xa31)
void combatManager::EffectSpellDamage(i32* effect, SpellType spell, i32 targetHex) {
    i32 fightValueKilledAI_3[COMBAT_SIDE_COUNT];
    i32 creaturesKilledResult_5;
    i32 remainderResult_4;
    i32 damagePerPowerResult_9;
    i32 stacksKilledCandidate_1[COMBAT_SIDE_COUNT];
    i32 doneWork;
    army* targetCreature_18;
    i32 killedCombatValue_1[COMBAT_SIDE_COUNT];
    CombatSide side_6;
    i32 damage;
    i32 currentHex_1;
    i32l spellDamageWork;
    float workChanceWork;
    i32 newDefense;
    CreatureType monsterTotal;
    i32 step_3;
    i32 disruptingRayValueTotal_11;

    switch (spell) {
        case SPELL_ARMAGEDDON:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_ARMAGEDDON_DAMAGE_PER_POWER;
            break;
        case SPELL_HOLY_WORD:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_HOLY_WORD_DAMAGE_PER_POWER;
            break;
        case SPELL_HOLY_SHOUT:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_HOLY_SHOUT_DAMAGE_PER_POWER;
            break;
        case SPELL_DEATH_RIPPLE:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_DEATH_RIPPLE_DAMAGE_PER_POWER;
            break;
        case SPELL_DEATH_WAVE:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_DEATH_WAVE_DAMAGE_PER_POWER;
            break;
        case SPELL_ELEMENTAL_STORM:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_ELEMENTAL_STORM_DAMAGE_PER_POWER;
            break;
        case SPELL_FIREBALL:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_FIRE_DAMAGE_PER_POWER;
            break;
        case SPELL_FIREBLAST:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_FIRE_DAMAGE_PER_POWER;
            break;
        case SPELL_METEOR_SHOWER:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_ELEMENTAL_STORM_DAMAGE_PER_POWER;
            break;
        case SPELL_LIGHTNING_BOLT:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_LIGHTNING_DAMAGE_PER_POWER;
            break;
        case SPELL_MAGIC_ARROW:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_MAGIC_ARROW_DAMAGE_PER_POWER;
            break;
        case SPELL_CHAIN_LIGHTNING:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_CHAIN_LIGHTNING_DAMAGE_PER_POWER;
            break;
        case SPELL_COLD_RAY:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_COLD_RAY_DAMAGE_PER_POWER;
            break;
        case SPELL_COLD_RING:
            damagePerPowerResult_9 = COMBAT_SPELL_AI_COLD_RING_DAMAGE_PER_POWER;
            break;
        default:
            damagePerPowerResult_9 = 0;
            break;
    }

    damage = m_spellPower[IDX(m_currentSide)] * damagePerPowerResult_9;
    currentHex_1 = 0;
    step_3 = 0;
    doneWork = 0;
    creaturesKilledResult_5 = 0;
    disruptingRayValueTotal_11 = 0;
    if (m_hexCells[targetHex].m_occupantIndex >= 0)
        targetCreature_18 =
            &m_armies[IDX(m_hexCells[targetHex].m_occupantSide)][m_hexCells[targetHex].m_occupantIndex];

    for (side_6 = COMBAT_ATTACKER_SIDE; IDX(side_6) < COMBAT_SIDE_COUNT; side_6++) {
        stacksKilledCandidate_1[IDX(side_6)] = 0;
        fightValueKilledAI_3[IDX(side_6)] = 0;
        killedCombatValue_1[IDX(side_6)] = 0;
    }
    ClearEffects();

    while (!doneWork) {
        switch (spell) {
            case SPELL_HOLY_WORD:
            case SPELL_HOLY_SHOUT:
            case SPELL_ARMAGEDDON:
            case SPELL_ELEMENTAL_STORM:
            case SPELL_DEATH_RIPPLE:
            case SPELL_DEATH_WAVE:
                NextPos(&currentHex_1);
                doneWork = currentHex_1 >= SPELL_AI_LAST_HEX + 1;
                break;
            case SPELL_COLD_RING:
                if (step_3 == 0)
                    step_3++;
            case SPELL_FIREBALL:
            case SPELL_FIREBLAST:
            case SPELL_METEOR_SHOWER:
                if ((step_3 >= SPELL_FIREBLAST_SECOND_RING_FIRST && spell != SPELL_FIREBLAST)
                    || step_3 >= SPELL_FIREBALL_AFFECTED_HEX_COUNT) {
                    doneWork = 1;
                    break;
                }
                if (step_3 == 0)
                    currentHex_1 = targetHex;
                if (step_3 > 0 && step_3 <= SPELL_ADJACENT_DIRECTION_COUNT)
                    currentHex_1 = GetAdjacentCellIndexNoArmy(
                        targetHex, static_cast<CombatHexDirection>(step_3 - 1)
                    );
                if (step_3 > SPELL_ADJACENT_DIRECTION_COUNT
                    && step_3 <= SPELL_ADJACENT_DIRECTION_COUNT * 2) {
                    currentHex_1 = GetAdjacentCellIndexNoArmy(
                        targetHex,
                        static_cast<CombatHexDirection>(
                            step_3 - SPELL_FIREBLAST_SECOND_RING_FIRST
                        )
                    );
                    currentHex_1 = GetAdjacentCellIndexNoArmy(
                        currentHex_1,
                        static_cast<CombatHexDirection>(
                            step_3 - SPELL_FIREBLAST_SECOND_RING_FIRST
                        )
                    );
                }
                if (step_3 == SPELL_FIREBLAST_AXIAL_FIRST)
                    currentHex_1 = targetHex - SPELL_FIREBLAST_HEX_ROW_STRIDE;
                if (step_3 == SPELL_FIREBLAST_AXIAL_SECOND)
                    currentHex_1 = targetHex + SPELL_FIREBLAST_HEX_ROW_STRIDE;
                if (step_3 == SPELL_FIREBLAST_CORNER_FIRST) {
                    currentHex_1 = GetAdjacentCellIndexNoArmy(
                        targetHex, COMBAT_DIRECTION_EAST
                    );
                    currentHex_1 = GetAdjacentCellIndexNoArmy(
                        currentHex_1, COMBAT_DIRECTION_NORTHEAST
                    );
                }
                if (step_3 == SPELL_FIREBLAST_CORNER_SECOND) {
                    currentHex_1 = GetAdjacentCellIndexNoArmy(
                        targetHex, COMBAT_DIRECTION_EAST
                    );
                    currentHex_1 = GetAdjacentCellIndexNoArmy(
                        currentHex_1, COMBAT_DIRECTION_SOUTHEAST
                    );
                }
                if (step_3 == SPELL_FIREBLAST_CORNER_THIRD) {
                    currentHex_1 = GetAdjacentCellIndexNoArmy(
                        targetHex, COMBAT_DIRECTION_WEST
                    );
                    currentHex_1 = GetAdjacentCellIndexNoArmy(
                        currentHex_1, COMBAT_DIRECTION_NORTHWEST
                    );
                }
                if (step_3 == SPELL_FIREBLAST_CORNER_FOURTH) {
                    currentHex_1 = GetAdjacentCellIndexNoArmy(
                        targetHex, COMBAT_DIRECTION_WEST
                    );
                    currentHex_1 = GetAdjacentCellIndexNoArmy(
                        currentHex_1, COMBAT_DIRECTION_SOUTHWEST
                    );
                }
                step_3++;
                break;
            case SPELL_LIGHTNING_BOLT:
            case SPELL_MAGIC_ARROW:
            case SPELL_COLD_RAY:
            case SPELL_DISRUPTING_RAY:
                if (currentHex_1 == targetHex)
                    doneWork = 1;
                else
                    currentHex_1 = targetHex;
                break;
            case SPELL_CHAIN_LIGHTNING:
                if (currentHex_1 == 0) {
                    currentHex_1 = targetHex;
                } else {
                    damage >>= 1;
                    currentHex_1 = GetNextChainLightningTarget(
                        &m_armies[IDX(m_hexCells[currentHex_1].m_occupantSide)]
                                 [m_hexCells[currentHex_1].m_occupantIndex],
                        0
                    );
                }
                step_3++;
                if (step_3 > CHAIN_LIGHTNING_MAX_TARGETS || currentHex_1 == COMBAT_HEX_EMPTY)
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
        }

        if (!doneWork) {
            if (currentHex_1 < 0 || currentHex_1 >= SPELL_AI_LAST_HEX + 1)
                continue;
            if (m_hexCells[currentHex_1].m_occupantIndex >= 0
                && m_hexCells[currentHex_1].m_occupantSide >= COMBAT_SIDE_VALID_BEGIN) {
            targetCreature_18 = &m_armies[IDX(m_hexCells[currentHex_1].m_occupantSide)]
                                      [m_hexCells[currentHex_1].m_occupantIndex];
            if (!gArmyEffected[IDX(m_hexCells[currentHex_1].m_occupantSide)]
                              [m_hexCells[currentHex_1].m_occupantIndex]) {
                gArmyEffected[IDX(m_hexCells[currentHex_1].m_occupantSide)]
                             [m_hexCells[currentHex_1].m_occupantIndex] = 1;
                workChanceWork = targetCreature_18->SpellCastWorkChance(spell);
                if (workChanceWork > 0.0f) {
                    spellDamageWork = static_cast<i32l>(damage * workChanceWork);
                    monsterTotal = targetCreature_18->m_monsterType;
                    switch (spell) {
                        case SPELL_ARMAGEDDON:
                            if (monsterTotal == CREATURE_IRON_GOLEM
                                || monsterTotal == CREATURE_STEEL_GOLEM)
                                spellDamageWork = static_cast<i32l>(
                                    spellDamageWork * SPELL_GOLEM_DAMAGE_MULTIPLIER
                                );
                            break;
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
                        case SPELL_METEOR_SHOWER:
                            if (monsterTotal == CREATURE_IRON_GOLEM
                                || monsterTotal == CREATURE_STEEL_GOLEM)
                                spellDamageWork = static_cast<i32l>(
                                    spellDamageWork * SPELL_GOLEM_DAMAGE_MULTIPLIER
                                );
                            if (monsterTotal == CREATURE_EARTH_ELEMENTAL)
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
                        m_heroes[IDX(m_currentSide)],
                        m_heroes[IDX(targetCreature_18->m_side)]
                    );
                    if (HAS(targetCreature_18->m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)
                        && spellDamageWork != 0)
                        spellDamageWork = COMBAT_SPELL_AI_MIRROR_LETHAL_DAMAGE;

                    creaturesKilledResult_5 = spellDamageWork / targetCreature_18->m_monster.hitPoints;
                    remainderResult_4 = spellDamageWork % targetCreature_18->m_monster.hitPoints;
                    if (remainderResult_4 + targetCreature_18->m_hitPointsLost
                        >= targetCreature_18->m_monster.hitPoints) {
                        creaturesKilledResult_5++;
                        remainderResult_4 -=
                            targetCreature_18->m_monster.hitPoints - targetCreature_18->m_hitPointsLost;
                    }
                    if (creaturesKilledResult_5 >= targetCreature_18->m_quantity) {
                        creaturesKilledResult_5 = targetCreature_18->m_quantity;
                        remainderResult_4 = 0;
                        stacksKilledCandidate_1[IDX(m_hexCells[currentHex_1].m_occupantSide)]++;
                    }

                    fightValueKilledAI_3[IDX(m_hexCells[currentHex_1].m_occupantSide)] +=
                        (creaturesKilledResult_5 * targetCreature_18->m_monster.hitPoints
                         + remainderResult_4 * COMBAT_SPELL_AI_PARTIAL_DAMAGE_MODIFIER)
                        * gMonsterDatabase[IDX(targetCreature_18->m_monsterType)].fightValue
                        / targetCreature_18->m_monster.hitPoints;
                    killedCombatValue_1[IDX(m_hexCells[currentHex_1].m_occupantSide)] +=
                        creaturesKilledResult_5 * targetCreature_18->m_monster.hitPoints
                        * gMonsterDatabase[IDX(targetCreature_18->m_monsterType)].fightValue
                        / targetCreature_18->m_monster.hitPoints;
                    if (HAS(targetCreature_18->m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)) {
                        killedCombatValue_1[IDX(m_hexCells[currentHex_1].m_occupantSide)] /=
                            SPELL_AI_MIRROR_VALUE_DIVISOR;
                        fightValueKilledAI_3[IDX(m_hexCells[currentHex_1].m_occupantSide)] /=
                            SPELL_AI_MIRROR_VALUE_DIVISOR;
                    }

                    if (spell == SPELL_DISRUPTING_RAY) {
                        newDefense = targetCreature_18->m_monster.defense
                                     - SPELL_DISRUPTING_RAY_DEFENSE_REDUCTION;
                        if (newDefense < SPELL_MINIMUM_DEFENSE)
                            newDefense = SPELL_MINIMUM_DEFENSE;
                        disruptingRayValueTotal_11 = static_cast<i32>(
                            disruptingRayValueTotal_11
                            + (targetCreature_18->m_monster.defense - newDefense)
                                  * (targetCreature_18->m_quantity - creaturesKilledResult_5)
                                  * gMonsterDatabase[IDX(targetCreature_18->m_monsterType)].fightValue
                                  * COMBAT_SPELL_AI_DISRUPTING_RAY_MODIFIER
                        );
                    }
                }
            }
            }
        }
    }

    if (stacksKilledCandidate_1[0] >= m_armyCount[0] || stacksKilledCandidate_1[1] >= m_armyCount[1]) {
        if (killedCombatValue_1[IDX(m_currentSide)] <= 0) {
            *effect = COMBAT_SPELL_AI_DECISIVE_EFFECT - gsSpellInfo[IDX(spell)].aiValue
                      + disruptingRayValueTotal_11;
        } else {
            *effect = killedCombatValue_1[IDX(OppositeCombatSide(m_currentSide))] - killedCombatValue_1[IDX(m_currentSide)]
                      + disruptingRayValueTotal_11;
        }
    } else {
        *effect = fightValueKilledAI_3[IDX(OppositeCombatSide(m_currentSide))] - fightValueKilledAI_3[IDX(m_currentSide)]
                  + disruptingRayValueTotal_11;
    }
    if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat)
        *effect = static_cast<i32>(*effect * COMBAT_SPELL_AI_CASTLE_EFFECT_MODIFIER);
}

float gfDurationMods[COMBAT_DURATION_MOD_COUNT] =
    {0.0f, 0.33f, 0.55f, 0.72f, 0.85f, 0.95f, 1.03f, 1.08f, 1.12f, 1.15f, 1.18f, 0.0f};
float gfCancelDurationMods[COMBAT_CANCEL_DURATION_MOD_COUNT] =
    {0.0f, 0.5f, 0.65f, 0.78f, 0.85f, 0.95f, 1.03f, 1.08f, 1.12f, 1.15f, 1.18f};
i32 giCurrSpellGroup;
