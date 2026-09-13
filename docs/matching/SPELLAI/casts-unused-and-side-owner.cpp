// VC6 SP5 measured source families for C60/S57/R18.
// Exact-span axes; clean parent state. No TU-state closure is claimed.
// Tested source record, not compilable replacement TU input.
// See docs/reconstruction/C60-C61-S57-B77.md for byte/relocation proof and limits.

// c60-raw src/SOURCE/SPELLAI.cpp RVA 0x9716f
// build/source-variant-batch/c60-raw/results.json
// Complete 32 arms; elapsed 18.690855s; input restored.
// Disposition: remove maximum/dragon_ratio/shooter_ratio/siege_factor, KEEP minimum; final full native and retail proof.
/* AXIS maximum
BEFORE
static_cast<float>(target->m_monster.damageMax)

AFTER remove
target->m_monster.damageMax
*/
/* AXIS minimum
BEFORE
static_cast<float>(target->m_monster.damageMin)

AFTER remove
target->m_monster.damageMin
*/
/* AXIS dragon_ratio
BEFORE
static_cast<float>(
                    dragonCounter / m_armyCount[IDX(OppositeCombatSide(target->m_side))]
                )

AFTER remove
dragonCounter / m_armyCount[IDX(OppositeCombatSide(target->m_side))]
*/
/* AXIS shooter_ratio
BEFORE
static_cast<float>(
                shooters / m_armyCount[IDX(OppositeCombatSide(target->m_side))]
            )

AFTER remove
shooters / m_armyCount[IDX(OppositeCombatSide(target->m_side))]
*/
/* AXIS siege_factor
BEFORE
static_cast<float>(factor + COMBAT_SPELL_AI_SIEGE_SHIELD_BONUS)

AFTER remove
factor + COMBAT_SPELL_AI_SIEGE_SHIELD_BONUS
*/

// s57-raw src/SOURCE/SPELLAI.cpp RVA 0x9716f
// build/source-variant-batch/s57-raw/results.json
// Complete 8 arms; elapsed 5.111002s; input restored.
// Disposition: original retained; all nonoriginal arms change native bytes. No impossibility claim for other structural parents or states.
/* AXIS unused38_h
BEFORE
    i32 H2_UNUSED(unused38_h);


AFTER remove

*/
/* AXIS unused30_j
BEFORE
    i32 H2_UNUSED(unused30_j);


AFTER remove

*/
/* AXIS unused48_e
BEFORE
    i32 H2_UNUSED(unused48_e);


AFTER remove

*/

// s57-determine src/SOURCE/SPELLAI.cpp RVA 0x95fb4
// build/source-variant-batch/s57-determine/results.json
// Complete 2 arms; elapsed 1.73641s; input restored.
// Disposition: original retained; all nonoriginal arms change native bytes. No impossibility claim for other structural parents or states.
/* AXIS unused_duration
BEFORE
    i32 H2_UNUSED(durMax_29);


AFTER remove

*/

// s57-resurrect src/SOURCE/SPELLAI.cpp RVA 0x97be4
// build/source-variant-batch/s57-resurrect/results.json
// Complete 2 arms; elapsed 1.683743s; input restored.
// Disposition: original retained; all nonoriginal arms change native bytes. No impossibility claim for other structural parents or states.
/* AXIS unused_probability
BEFORE
    float H2_UNUSED(workChance);


AFTER remove

*/

// r18-side src/SOURCE/SPELLAI.cpp RVA 0x95da0
// build/source-variant-batch/r18-side/results.json
// Complete 4 arms; elapsed 2.805735s; input restored.
// Disposition: original retained; all nonoriginal arms change native bytes. No impossibility claim for other structural parents or states.
/* AXIS role_scope
BEFORE
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


AFTER function_side
i32 combatManager::DoSpellAI(H2_ENUM_PARAM(CombatSide, i32) side, i32 restricted) {
    SpellType chosenSpell;
    CombatSide checkedSide;
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

    for (checkedSide = COMBAT_ATTACKER_SIDE; IDX(checkedSide) < COMBAT_SIDE_COUNT; checkedSide++) {
        if (m_heroes[IDX(checkedSide)] != NULL
            && m_heroes[IDX(checkedSide)]->HasArtifact(ARTIFACT_SPHERE_NEGATION))
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


AFTER scoped_side
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

    {
        CombatSide checkedSide;
        for (checkedSide = COMBAT_ATTACKER_SIDE; IDX(checkedSide) < COMBAT_SIDE_COUNT; checkedSide++) {
            if (m_heroes[IDX(checkedSide)] != NULL
                && m_heroes[IDX(checkedSide)]->HasArtifact(ARTIFACT_SPHERE_NEGATION))
                return 0;
        }
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


AFTER sequential_scopes
i32 combatManager::DoSpellAI(H2_ENUM_PARAM(CombatSide, i32) side, i32 restricted) {
    SpellType chosenSpell;
    i32 effect;
    i32 bestHexWork;
    i32 candHex;
    i32 bestValue;
    i32 manaRatioResult;

    bestValue = 0;
    chosenSpell = SPELL_NONE;
    bestHexWork = -1;

    if (m_heroes[IDX(side)] == NULL)
        return 0;

    {
        CombatSide checkedSide;
        for (checkedSide = COMBAT_ATTACKER_SIDE; IDX(checkedSide) < COMBAT_SIDE_COUNT; checkedSide++) {
            if (m_heroes[IDX(checkedSide)] != NULL
                && m_heroes[IDX(checkedSide)]->HasArtifact(ARTIFACT_SPHERE_NEGATION))
                return 0;
        }
    }

    H2_ENUM_STORAGE_STEPPED(SpellType, i32) spell;

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

*/
