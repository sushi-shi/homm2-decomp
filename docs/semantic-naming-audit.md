# Semantic naming audit

This audit checks names against assignments, formulas, and callers. A matching
object proves executable behavior, not the historical spelling or accuracy of
a reconstructed identifier. Recommendations below are analysis, not applied
source changes. Anchors are function/member names because line numbers move.

## Symbol evidence

The PoL retail image used here is
`/tmp/homm2-rename-ai-decomp-pol/build/orig/HEROES2W.EXE`.
The retired `scripts/archive/codeview_symbols.py <image>` (at `f0ae961d2`) reproduces its
public symbol inventory. Its NB09 symbol subsections contain 3,541 `S_PUB32`
records, zero `S_BPREL32` stack-local/argument records, zero `S_REGISTER`
records, and no `sstTypes` subsection. This establishes public name provenance;
it does not supply parameter names, locals, or member layouts/names.
The Buka image is stripped; transferring a PoL name to a Buka address remains
a separate correspondence claim. A synthesized comparison PDB is not original
debug evidence.

The public inventory explicitly names `armyGroup::DamageGroup`,
`armyGroup::IsHomogeneous`, `philAI::QuickCombat`, `philAI::DamageGroup`,
`philAI::GetOddsOfWinning`, `army::ValidRange`, `army::SpellCastWorkChance`,
`advManager::PlayerMonsterInteract`, `advManager::ComputerMonsterInteract`,
and `town::BuildBuilding`. Preserve those original function names even when
their contracts are surprising. None of the recommendations below asserts
recovery of the developers' original internal spellings.

## Behaviorally contradicted names

| Existing name | Behavior and reproducible source anchors | Recommended meaning/name |
| --- | --- | --- |
| `town::m_garrison`, `TOWN_GARRISON_SLOT_COUNT`, `nGarrison` in `philAI::CreaturesToBuy` | `town::BuildBuilding` seeds dwelling stock; `game::PerWeek` adds creature growth; the town-taking `recruitUnit` constructor points its availability field into this array; `philAI::BuildCreature` subtracts purchases. Actual defending troops are in `town::m_army`. | `m_dwellingAvailable`, `TOWN_DWELLING_STOCK_SLOT_COUNT`, `availableCount`. Preserve the 12-entry serialized layout; do not merge it with the five-slot army. |
| `attackerRemaining`, `defenderRemaining` outputs of `philAI::ProbableOutcomeOfBattle` | With attacker win probability `p`, raw strengths `A,D`, formulas are `(1-p*p)*A` and `p*(2-p)*D`, before integer rounding. At `p=1` they are `0,D`; at `p=0` they are `A,0`. The result subtracts the attacker's value and adds the defender's value. They are expected strength losses, not survivors. | `expectedAttackerLossValue`, `expectedDefenderLossValue`; carry this contract through caller locals. |
| `attackerDead`, `defenderDead` in `philAI::QuickCombat` | Passed to the first two integer outputs of `ProbableOutcomeOfBattle`: `(1-p)*A` and `p*D`, using `FightValueOfStack(...,0)`. Neither is a Boolean nor a number of dead creatures. | `attackerLossValue`, `defenderLossValue`. |
| `damagePercent` in `armyGroup::DamageGroup` | Multiplied by 100 to construct `killChance`; compared to `0.999` and `1.0`. The quick-combat caller supplies `fracLost`. It takes a casualty fraction, not a 0–100 percentage or hit-point damage. | `casualtyFraction`; use the same unit in `philAI::DamageGroup` and the `QuickCombat` output references currently called `attackerDamage`/`defenderDamage`. |
| `handled` in both monster-interaction functions; `killed` in `advManager::CheckAdjacentMon` | Callers pass `eraseObject`, or call `EraseObj` when the output is set. Joining and letting monsters flee also set it. Rejecting the free AI purchase sets it too. It requests object removal, not merely event handling or proof of death. | `removeMonsterObject` output; `removeMonster` local. |
| `army::m_roundCounter` | Initialized to `-1`; `combatManager::MirrorImage` sets it to the image's duration; `army::DecrementSpellRounds` decrements it; `combatManager::ResetRound` kills the image when it reaches zero. | `m_mirrorImageRoundsRemaining`, retaining the inactive `-1` sentinel. |
| `army::m_animationCycle` | `army::PowEffect` sets it precisely when the current sequence is SHOOT_UP, SHOOT_FORWARD, or SHOOT_DOWN. It selects the shooting continuation, not arbitrary animation cycling. | `m_shootingAnimationActive`. |
| `m_heroDeathPending`, `m_heroAlternateDeathPending`, corresponding `...AnimationPlayed`, and `HERO_ANIMATION_DEATH_FIRST/SECOND` | `army::ProcessDeath` schedules the owning hero's reaction or the opposing hero's reaction. `army::GoBerserk` and `combatManager::ChainLightning` also schedule the first reaction on friendly fire, without requiring a hero or even a troop stack to die. `combatManager::CycleCombatScreen` plays sprite sequences 1/2; it does not remove a hero. | Use reaction terms such as `m_heroLossReactionPending`, `m_heroOpponentLossReactionPending`, and matching sequence/played names. The exact emotional pose needs sprite inspection; do not guess "cry" or "celebrate" from code alone. |
| `countRaces` in `armyGroup::IsHomogeneous` | Only the sentinel value `-1` enables the faction tally. It is neither an input count nor a conventional nonzero Boolean. All current callers supply `-1`. | `alignmentMode` with a named sentinel; preserve behavior for other values. |
| `numCreatureTypes` in `armyGroup::IsHomogeneous` | Incremented when the next nonempty slot's type differs from the preceding nonempty type. `A,B,A` produces 3, not 2. This is a run count, not a distinct-type count. | `creatureTypeRuns`. Preserve the algorithm; renaming is not permission to replace it with set cardinality. |

## Names that hide a narrower contract

| Existing name | Evidence | Recommendation |
| --- | --- | --- |
| `town::m_buildState` | Only guild construction increments it; it indexes spell rows and is copied directly to the `mageGuildLevel` output of `CalcNumLevelArchers`. Map loading assigns `extra->mageGuildLevel` to it. | `m_mageGuildLevel`, not a generic building-progress field. |
| `useHero` in `philAI::FightValueOfStack` | Enables nonlinear stack-size weighting even when the hero pointer is null. Hero statistics and spell contributions are an additional guarded part of the same mode. `ProbableOutcomeOfBattle` deliberately calls it with `(townArmy,NULL,1)`. | `useAdjustedFightValue`, documenting both stack weighting and optional hero modifiers. |
| `useEnemyMods` in `philAI::FightValueOfStack` | Passed `useTown` for the attacking army in `ProbableOutcomeOfBattle`. Its branches use the attacking hero's Ballista, Earthquake, Ballistics, Archery, and Golden Bow to adjust siege weights. | `applySiegeAttackerModifiers`; validate other callers before applying. |
| `attackerLoss` / `defenderLoss` outputs of `ProbableOutcomeOfBattle` | Scaled raw fight-value totals, not troop counts or HP. The formula depends on `p`; it is not an actual combat casualty observation. | Include `Value` in the names and document the formulas; do not infer casualty counts. |

## Reviewed contracts that should remain

- Human `monstersFlee` and Visions `creaturesFlee` describe real refusal/chase
  and prediction dialogs. They must not inherit the AI `autoDefeatMonsters`
  name merely because they share the same strength threshold.
- `MONSTER_STRENGTH_FLEE` is a shared behavioral threshold, used both in those
  human/Visions paths and the AI automatic-defeat path. Its existence does not
  mean the AI awards no experience or Necromancy skeletons.
- `m_hitPointsLost` is the wound remainder on a surviving creature, not
  cumulative battle damage. `army::Damage` assigns `damage % hitPoints`;
  `Cure` and Troll regeneration clear/reduce that remainder.
- `m_temporaryResurrectionQuantity` counts resurrected creatures removed from
  survivors in the postcombat pass. It is not the spell's duration.
- `m_ultimateArtifactHintChance` really is a 1–100 probability: `ComputeUALoc`
  compares a 1–100 roll to it before choosing the exact artifact location.
- `IS_INTERIOR_COMBAT_HEX` checks range and border columns only. It makes no
  occupancy or movement-validity claim, and its existing comment says so.
- `IsHomogeneous` is an original symbol returning a morale-domain integer,
  not a Boolean. `ValidRange` is an original symbol that also writes attack
  targeting state. `GetOddsOfWinning` is an original constant-return body in
  this reconstruction. Document these contracts rather than inventing new
  historical function names.

## Limits and application

This pass traces adventure-monster removal, town recruitment/guild state,
army animation and spell lifetime, and AI battle estimates. It is not a claim
that every identifier in every branch has been validated. Portable and
Ironfist extensions need their own caller checks before propagating changes.
Source-level evidence above supports naming corrections, not new behavior.
Before changing a matching body, reproduce its retail dossier and relocation
review. Local/argument renames can move `/Od` slots; preserve/recover the retail
frame and run the required build and comparison. Member renames must retain
storage, offsets, and serialized records. Regenerate source/classic branches
from their decomp parents after accepted upstream changes.
