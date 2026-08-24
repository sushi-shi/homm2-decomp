/*
combatManager::Resurrect cross-version comparison family
========================================================

PoL 2.0 RVA 0x29089; Buka 2.1 RVA 0xa0433.

Complete clean VC4.2 product: 8/8
---------------------------------
  build/resurrect-buka-comparisons-manifest.json
  build/source-variant-batch/resurrect-buka-comparisons/results.json

Independent axes:
  - quantity > initialQuantity cap comparison
  - index + 1 == deadOccupantCount comparison
  - reverseFrame < deathFrameCount min-select order

All eight arms compile to one candidate state:
  93.233990%, 1599 bytes, 33/33 relocations
  48/48 blocks, 41 exact and 7 size-only

The retained normal build has the same bytes, relocation stream, and topology.
The larger index/layout residual is independent of these expressions and
remains open under the separate SPELLS slot-name campaign.

Disposition: invariant matching-decomp port of all three Buka comparison/select
spellings. No gameplay change.
*/
