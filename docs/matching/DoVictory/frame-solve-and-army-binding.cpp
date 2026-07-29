// combatManager::DoVictory (SOURCE/COMMAND, RVA 0x2ec8b) — 2026-07-29
// MAX 97.55 -> 99.33 · GetCommand (RVA 0x2c8ff) -> EXACT 100
//
// DoVictory matrices: build/dovic-axes.json, build/dovic2-axes.json.
// 1. army_binding natural_2d (integrated): &m_armies[IDX(side)][idx] replaces
//    the flat `IDX(side) * COMBAT_ARMY_STORAGE_SLOT_COUNT + &m_armies[ATT][i]`
//    contortion at both sites (97.28 -> 99.31, 121/121 blocks).
// 2. Frame solve (od_slots bijection): retail walk order lastLivingArmy0,
//    deadCreatureCount3, side, experienceLevels, message, currentArmy26,
//    fadeIndex26, cost, eligibleWinnerStacks, armyIndex, fadeTimer, fadeCount,
//    livingCount, emptySlots, experienceText. Renames: i(9), victoryLevels(9),
//    necroEligible(9), combatSide(9), timer(14), fadeCount(14), living(14),
//    emptySlots(14); declarations in REVERSE walk order so within-bucket ties
//    resolve automatically. Caveat learned: a 1x-count slot pair from the row
//    dump was a cross-ALIGNMENT artifact (side vs armyIndex) — re-derive
//    ambiguous pairs from unambiguous rows (inc/cmp-bound loop vars).
// 3. army_terms flat_index_first: REJECTED (term order is at-state).
// Residual: one index/side term-order mirror in the army address math.
//
// GetCommand: the enemy-side switch's last case ends with an explicit
// `break;` — retail emits jmp;jmp (case break + switch fallout). Adding the
// break closed the function EXACT at the clean state (93.82 -> 100), the
// od-goto-stub `default:break`-family lever.
//
// 2026-07-29 later pass (build/dovic4-axes.json): the 1-byte deficit was the
// loser-side argument to ExperienceValueOfStack — retail computes it as
// OppositeCombatResult(winningSide), which under the retail build is the
// macro (COMBAT_RESULT_DEFENDER - result) = `mov eax,1; sub eax,[esp+8]`;
// the literal ternary compiled to the cmp/sbb/neg bool. Integrated
// IDX(OppositeCombatResult(winningSide)); first-divergence is now NONE with
// 121/121 blocks at 99.687. dovic3/dovic5 measured rejections: reversed
// subscript, flat index-first, row+index pointer sums — the m_armies term
// order canonicalizes in every spelling; the one remaining cluster is the
// at-state index/side evaluation-order mirror (island only).
