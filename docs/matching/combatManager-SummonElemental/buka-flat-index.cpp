/*
combatManager::SummonElemental summon-hex indexing
===================================================

PoL 2.0 RVA 0x28951; Buka 2.1 RVA 0x9fe0a.

Complete VC4.2 source/state product: 204/204
------------------------------------------------
  build/summon-elemental-flat-index-manifest.json
  build/source-variant-batch/summon-elemental-flat-index/results.json

The two loop uses are independent axes: indexed `summonHexes_l[...]` versus
Buka's flat `*(summonHexes_l + ...)`. Each of the four source shapes was
compiled clean and against 50 forest/top TU states.

Clean: all four arms 99.973690%, 536 bytes, 5/5 relocations.
Trial 22: all four arms exact, 536 bytes, exact 15/15 CFG, 5/5 relocations.
Trial 23: the full Buka pair is exact while the three mixed/PoL arms are
91.072365%, 95.289474%, and 95.782900%; this is a compiler-state interaction,
not a semantic difference.

Disposition: matching-decomp port of both authoritative Buka flat-index
expressions. No gameplay change. The final source is replayed at seed
1213156658 forest/top trial 22 before recording MAX.
*/
