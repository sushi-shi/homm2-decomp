# Semantic naming audit

This audit checks names against assignments, formulas, and callers. A matching
object proves executable behavior, not the historical spelling or accuracy of
a reconstructed identifier. Recommendations below are analysis, not applied
source changes. Anchors are function/member names because line numbers move.
Every proposed spelling is listed individually in
[the rename review](semantic-naming-renames.md), including separate caller
locals. The review does not authorize applying the changes.

## Symbol evidence

The PoL retail image used here is
`/tmp/homm2-rename-ai-decomp-pol/build/orig/HEROES2W.EXE`.
Run `python3 scripts/archive/codeview_symbols.py <image>` to reproduce its
public symbol inventory. Its NB09 symbol subsections contain 3,541 `S_PUB32`
records, zero `S_BPREL32` stack-local/argument records, zero `S_REGISTER`
records, and no `sstTypes` subsection. This establishes public name provenance;
it does not supply parameter names, locals, or member layouts/names.
The Buka image is stripped; transferring a PoL name to a Buka address remains
a separate correspondence claim. A synthesized comparison PDB is not original
debug evidence.

To reproduce the record census with the existing parser (pass the retail
image as the first argument to this snippet):

```python
import collections, contextlib, io, runpy, struct, sys

sys.argv = ["codeview_symbols.py", sys.argv[1]]
with contextlib.redirect_stdout(io.StringIO()):
    cv = runpy.run_path("scripts/archive/codeview_symbols.py")
counts = collections.Counter()
for subsection, module, offset, size in cv["entries"]:
    blob = cv["data"][offset:offset + size]
    if subsection == 0x125:  # sstAlignSym
        records = blob[4:]
    elif subsection in (0x129, 0x12a, 0x134):  # hashed symbol streams
        records = blob[16:16 + struct.unpack_from("<I", blob, 4)[0]]
    else:
        continue
    position = 0
    while position + 4 <= len(records):
        length, kind = struct.unpack_from("<HH", records, position)
        if not length:
            break
        assert position + 2 + length <= len(records)
        counts[kind] += 1
        position += 2 + length
print({hex(kind): count for kind, count in sorted(counts.items())})
print("sstTypes:", sum(s == 0x121 for s, _, _, _ in cv["entries"]))
```

Observed records: `0x1:177`, `0x6:182`, `0x9:177`, `0x203:3541`,
`0x206:182`, `0x402:36`; `sstTypes:0`. The absent local record IDs above
refer to this CV4/NB09 format, not later CodeView encodings.

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
| `MAP_MONSTER_GUARD_FLAG`, `MONSTER_GUARD_ROLL_MIN/MAX`, `MONSTER_GUARD_CUTOFF` | The flag is `0x1000`, the same bit read as `MONSTER_JOIN_FORCED` by both interaction functions and Visions. `game::PerWeek` preserves it while increasing monster count; random monster generation sets it using the named rolls. It marks the free-join disposition, not guards. | `MAP_MONSTER_FORCE_JOIN`, `MONSTER_FORCE_JOIN_ROLL_MIN/MAX`, `MONSTER_FORCE_JOIN_CUTOFF`. The existing `MONSTER_JOIN_FORCED` name is accurate and need not be renamed. |
| `usePathfinding` in `CalcTerrainCost` | `advManager::MoveHero`, `advManager::ShowRoute`, and `searchArray::SeedPosition` pass destination `m_isRoad`; skill level is already a separate argument. Both road arguments being true selects `TERRAIN_ROAD`. | `destinationHasRoad`; rename the paired `useRoad` argument to `sourceHasRoad`. Preserve the low-mobility branch that reads only the source flag. |
| `s_currentWater`, `s_targetWater` in SEARCH | Assigned exclusively from the respective map cell's `m_isRoad`, then passed to the two road arguments of `CalcTerrainCost`. Terrain-water tests are separate. | `s_currentHasRoad`, `s_targetHasRoad`. |
| `playerData::m_aiDifficulty` | Typed `PlayerPersonality`; compared to WARRIOR, BUILDER, EXPLORER, and HUMAN. `townManager::SetupThievesGuild` displays `cPersonality` at this index; actual difficulty is `game::m_difficulty`. Ironfist serializes it as `personality`. | `m_aiPersonality`. |
| `playerData::m_cheatValue` | `game::SetupPuzzlePieces` adds it to revealed pieces. Both campaign bonus paths assign it for PUZZLE_PIECES, while the puzzle cheat adds 12. Legitimate bonuses use it too. Ironfist serializes it as `puzzlePieces`. | `m_bonusPuzzlePieces`. Preserve the XML key. |
| `SamplePlaybackData::loopCount` | Set to true for adventure ambient loops, false for army samples. Audiere converts it to a Boolean repeat flag; Miles passes `loopCount == 0` to its loop-count API. The portable sound manager maps it to `-1` or `0` repeat mode. | `looping`. Do not rename the unrelated icon-decoder loop counters. |
| `game::m_viewArmyResult` | Initialized by `InitVars` and `game::ViewArmy`; the only increment/read is the tavern animation handler's frame modulo. Dismiss/upgrade results use separate globals. | `m_dialogAnimationCounter`, acknowledging the shared initialization rather than inventing an army-dialog return value. |

## Names that hide a narrower contract

| Existing name | Evidence | Recommendation |
| --- | --- | --- |
| `town::m_buildState` | Only guild construction increments it; it indexes spell rows and is copied directly to the `mageGuildLevel` output of `CalcNumLevelArchers`. Map loading assigns `extra->mageGuildLevel` to it. | `m_mageGuildLevel`, not a generic building-progress field. |
| `useHero` in `philAI::FightValueOfStack` | Enables nonlinear stack-size weighting even when the hero pointer is null. Hero statistics and spell contributions are an additional guarded part of the same mode. `ProbableOutcomeOfBattle` deliberately calls it with `(townArmy,NULL,1)`. | `useAdjustedFightValue`, documenting both stack weighting and optional hero modifiers. |
| `useEnemyMods` in `philAI::FightValueOfStack` | Passed `useTown` for the attacking army in `ProbableOutcomeOfBattle`. Its branches use the attacking hero's Ballista, Earthquake, Ballistics, Archery, and Golden Bow to adjust siege weights. | `applySiegeAttackerModifiers`; validate other callers before applying. |
| `attackerLoss` / `defenderLoss` outputs of `ProbableOutcomeOfBattle` | Scaled raw fight-value totals, not troop counts or HP. The formula depends on `p`; it is not an actual combat casualty observation. | Include `Value` in the names and document the formulas; do not infer casualty counts. |
| `hexcell::m_pathReachable` | `searchArray::SeedCombatPosition` also sets it on distant enemies when a shooter is not melee-blocked. `combatManager::SetupGridForArmy` uses it for action-range shading. It is not a guarantee of a walking path to that hex. | `m_movementOrAttackReachable`. |
| `playerAIData::m_upgradeValueWeight` | Initialized from gold plus income divided by army strength, plus an attention baseline. It converts fight value into resource value for purchases, battles, and hero interactions, not only upgrades. | `m_fightValueResourceWeight`. |

## Caller traps that rule out a global replacement

`philAI::EvaluateMonsterEvent` reuses `attackerLoss` as the purchase-value output
of `EvaluateOneTimeCreaturePurchase`, and subsequently returns that value in
the join path. Its `purchaseCost` local receives **purchaseCount**, and
`unusedPurchaseValue` receives **replacementSlot**. The review proposes
`purchaseCount` and `replacementSlot` for those two locals, but deliberately
holds `attackerLoss`: split the two semantic roles with retail-frame evidence
before choosing a single-role name. Renaming every occurrence of
`attackerLoss` across PHILAI would also confuse integer fight-value estimates
with the float casualty fractions in `FightEvent` and `TownEvent`.

`liveChance` in `EvaluateMineEvent` receives a truncated `winChance` without
the percent scaling used in other event evaluators. This is a behavior/unit
question, not evidence that a source rename can repair it. It is outside this
proposal.

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
- `searchArray::m_maxQueueCount` is a measured peak, updated when the queue
  grows; `SEARCH_QUEUE_CAPACITY` is the storage limit. The names make different
  claims and do not require unification.
- `m_hideCount` is a balanced nested visibility counter, not a Boolean.
  `ReallyHidePointer`/`ReallyShowPointer` can manipulate the native cursor
  independently. `IsVis` reports the counter's state, not pixel occlusion.
- `RemotePacketHeader::payloadSize` and `RemoteMessage::payloadSize` describe
  separate packet layers. The encoded layer uses a byte; the message layer
  uses an i16. Keep their domains and layouts separate.
- `DP_PROTOCOL_IPX` and `DP_PROTOCOL_TCP` are contextual aliases used for
  DirectPlay service-provider GUID selection. The transport-selection names
  are used elsewhere. No replacement is proposed without resolving that
  transport/provider distinction.
- Ironfist's XML key `aiNumberPuzzlePieces` stores the artifact-hint chance,
  not the number of puzzle pieces. `hasEvilFaction` stores interface selection.
  These misleading wire keys are compatibility contracts: this review
  proposes no XML key or Lua API renames.

## Limits and application

The header screen covers SOURCE, EDITOR, and BASE. Detailed traces cover
adventure-monster removal and disposition, town recruitment/guild state,
army animation and spell lifetime, AI battle estimates and personality,
terrain/path costs, combat-range shading, puzzle bonuses, sound looping,
resource/input/UI state, and network packet/transport contracts. Relevant
portable callers and Ironfist persistence/hooks were checked. This is not a
claim that every identifier or every extension has been validated.
Portable and Ironfist extensions still need focused build checks before
propagating accepted source changes.
Source-level evidence above supports naming corrections, not new behavior.
Before changing a matching body, reproduce its retail dossier and relocation
review. Local/argument renames can move `/Od` slots; preserve/recover the retail
frame and run the required build and comparison. Member renames must retain
storage, offsets, and serialized records. Regenerate source/classic branches
from their decomp parents after accepted upstream changes.
