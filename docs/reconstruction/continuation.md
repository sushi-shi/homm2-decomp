# Continuation after PRs #59 and #63

These PRs are verified cleanup checkpoints, **not completion of the broader
semantic/reconstruction audit**. Merge order is #59 into `decomp-gold-2.1-buka`,
then #63 into decomp. If #59 is squash-merged, reconcile #63 with that new base
before merging; do not import its historical base commits again.

## Integration checkpoint

#59 combines its audited head `36ace4cae` with decomp `299514f88` (#62, #64 and
the subsequent packet-name fixes). The 35 conflicted files retain the recovered
owners, layouts, casts/control-flow cleanup and the newer readable identifiers.
VC6 aliases for the sending packet index and remote-player exit flag use distinct
local names so they cannot expand recovered record fields with the same name.

Validation: Russian matching build reports 1,727/1,727 functions and
291,987/291,987 data bytes under the existing comparison model; all 98 raw objects
and 42,433 ordered relocations preserve the `36ace4cae` result. English ordinary
build compiles and links; readability contracts pass; 971 selftests pass with
seven skips. These are compatibility checks, not gameplay tests or closure of
the independent retail-model gaps documented in the historical findings.

A supplemental C++20 parse covered 96 TUs with no project-source diagnostics,
but emitted 1,520 errors in legacy MSVC library headers. It is not a clean
whole-tree strict-build result and is not used as the compatibility proof.

## What is left

Do not add these overlapping counts together or interpret them as confirmed bugs:

- Historical ten-pattern research: 951 functions with an open category, comprising
  1,437 function/category entries (897 bounds entries). Shared input contracts,
  uncertain reconstruction claims and proven retail defects are included.
- Live #59 source inventory after this integration: 1,654 definitions; 923 retain
  their review hash and 731 need review refresh (597 changed hashes, 134 missing
  keys). There are 132 orphaned historical keys after signature/name changes.
  File reviews: 42/229 hashes match; 187 need refresh. Macro census: 2,736.
- #63 has six known naming findings from its review at `f17bc1704`; they remain
  follow-up work, not conflict resolutions. See the next batch below.

`functions.tsv`, `files.tsv`, `macros.tsv`, `progress.md`, `reviews.json` and
`file-reviews.json` remain the historical `36ace4cae` snapshot. Do not cite their
coverage as current. Inventory `--check` is not a passing integration gate yet:
orphaned keys must be reconciled before regeneration. Preserve old review hashes
and notes; migrating a key must not certify a new function body as reviewed.

## First follow-up batch: six naming findings

1. `searchArray::FindCombatPath` (`FINDPATH.cpp`): its two combat-mask loops need
   `COMBAT_DIRECTION_COUNT`, not `MAP_DIRECTION_COUNT`. Keep adventure-map loops
   in the map domain; trace masks to `GetAttackMask`/`GetMoveMask` in `PATH.cpp`.
2. Wagon/lean-to rewards (`PHILAI.cpp`, interactive and AI paths in `EVENTS.cpp`):
   distinguish resource quantity/type encoding from artifact and daemon fields.
   Establish shared encoding names from producers, then update every consumer.
3. `advManager::SetEnvironmentOrigin` and `TrimLoopingSounds` callers: distinguish
   spatial search range/cache policy from active sound-channel capacity.
4. Daily wood/ore income (`GAME.cpp`) and sawmill display (`EVENTS.cpp`): use a
   meaningful common rate or resource-role aliases, not an ore-only name for wood.
5. `combatManager::EffectSpellDamage` (`SPELLAI.cpp`): Cold Ring uses a Fireball-only
   rate name. Runtime `Fireball` handles Fireball/Fireblast/Cold Ring; name that
   shared rate accurately or retain explicit spell aliases.
6. Campaign confirmation (`Campaign.cpp`, `X_CAMPGN.cpp`): use standard Yes/No
   names only in proven standard dialogs; align campaign activation aliases
   across both handlers. Keep raw slots in generic dispatch/unknown layouts.

For each: inspect producer and consumers, preserve numeric values/storage,
update focused contracts, and compare fresh before/after raw objects. Equal
enum values or matching bytes alone do not prove the new name is meaningful.

## Then resume the broader audit

Migrate the unambiguous historical keys using path, function identity and VA;
retain old hashes, and investigate ambiguous/deleted definitions separately.
Regenerate the inventory, review changed bodies and affected header consumers,
and group open entries by their underlying issue before choosing a bounded batch.
Useful research starting points are B13/B20 network layout/receive bounds and
B28/B29 icon decoder input contracts. Separate reconstruction fixes from proven
retail bugs; gameplay/input hardening requires a distinct, explicitly scoped change.

Inside `nix develop .#build`, with Universal Ctags 6 available:

```
python3 -m homm2.audit.reconstruction --write
python3 -m homm2.audit.reconstruction --check
homm2 build
python3 -m homm2.audit.object_equivalence BEFORE build/objdiff/base
python3 -m homm2.audit.readability_contracts
homm2 selftest
homm2 build --no-match --en
```

`BEFORE` must be a separately saved/built pre-change raw-object tree. Generated
evidence stays under `build/`; add concise dispositions here or in the existing
finding reports, not another large committed audit dump. Downstream propagation
and source/classic regeneration are subsequent work, not performed by this merge.
