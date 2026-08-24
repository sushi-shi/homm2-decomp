/*
combatManager::DoVictory cross-version split
============================================

PoL 2.0 RVA 0x2ec8b; Buka 2.1 RVA 0x2f601.

Matching-decomp comparisons
---------------------------
  build/do-victory-comparison-manifest.json
  build/source-variant-batch/do-victory-comparisons/results.json

Complete clean VC4.2 product: 8/8. The winner-side equality, fade-loop
condition, and artifact-capacity condition are independent axes. Every arm
emits the same 97.553570%, 2984-byte state with exact 121/121 CFG and the
same ordered 112/112 relocations. PoL adopts Buka's three spellings.

Final-source forest/top census:
  build/do-victory-final-state-summary.json
  seed 1213156658, 50/50 complete; nine states; best trial 2 at
  99.686690%, 2984 bytes, exact 121/121 CFG, 112/112 relocations.
  No exact island; the residual is independent of the donor comparisons.

Logic value: necromancy capacity exclusion
-------------------------------------------
  build/do-victory-necromancy-manifest.json
  build/source-variant-batch/do-victory-necromancy/results.json

Complete clean VC4.2 product: 2/2. PoL retail compares immediate 0x3b
(CREATURE_GHOST); exact Buka compares 0x2f (CREATURE_SKELETON). Both arms
retain 2984 bytes, exact 121/121 CFG, and 112/112 relocations; fuzzy scores
are 97.553570% and 97.555190%, respectively. The retail immediates establish
the versioned behavior. Keep CREATURE_GHOST in PoL; do not backport Buka's
logic change.
*/
