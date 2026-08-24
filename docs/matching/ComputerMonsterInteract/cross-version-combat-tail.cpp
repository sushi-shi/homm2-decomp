/*
advManager::ComputerMonsterInteract cross-version audit, 2026-08-24.

Targets:
  PoL 2.0: RVA 0x000b5800, 1088 bytes, 48 blocks, 36 relocations.
  Buka:    RVA 0x00048730,  980 bytes, 41 blocks, 35 relocations.

Classification: matching-only non-transfer. The tail spellings are release-
equivalent: Buka returns after fleeing and nests the metadata update in the
zero-combat-result else; PoL uses outer-if fallthrough and an early return.

Clean structural matrices:
  build/computer-monster-interact-tail-manifest.json                 4/4
  build/source-variant-batch/computer-monster-interact-tail/results.json
  build/computer-monster-interact-flat-complete-manifest.json        3/3
  build/source-variant-batch/computer-monster-interact-flat-complete/results.json

Buka's literal return grows PoL to 1093 bytes. The flattened labeled tail
grows to 1103 bytes. Buka's nested-result else alone retains 1088 bytes and
raises fuzzy score, but worsens the labeled-edge/predecessor deltas. PoL's
current early-return parent remains the topology winner: 48/48 blocks, two
target-shift blocks, and the closest canonical graph.

Complete surviving-parent state matrix:
  build/computer-monster-interact-result-tail-state-manifest.json
  build/source-variant-batch/computer-monster-interact-result-tail-state/results.json

All 102/102 cells completed: early return vs nested else, each in clean plus
50 forest/top states. No state changed either structural orbit and no exact
closure appeared. Retain PoL source and 99.9684 MAX unchanged.
*/
