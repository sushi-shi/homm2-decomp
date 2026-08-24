/*
combatManager::ShowMassSpell frame comparisons
===============================================

PoL 2.0 RVA 0x273df; Buka 2.1 RVA 0x9ea07.

Complete clean VC4.2 product: 16/16
----------------------------------
  build/show-mass-spell-buka-compares-manifest.json
  build/source-variant-batch/show-mass-spell-buka-compares/results.json

The independent axes cover two effect-frame clamps, the return-frame clamp,
and the effect-frame loop condition. All sixteen arms emit one clean state:
99.727890%, 1715 bytes, exact 97/97 CFG, 23/23 relocations.

Final-source compiler-state audit:
  build/show-mass-spell-final-state-summary.json
  build/show-mass-spell-final-replay-state-summary.json
  seed 1213156658 forest/top trial 2: exact, 1714 bytes, exact 97/97 CFG,
  complete ordered 23/23 relocations; MAX 100 at hash ca24c46a434c.

Disposition: invariant matching-decomp port of all four Buka comparisons.
No gameplay change.
*/
