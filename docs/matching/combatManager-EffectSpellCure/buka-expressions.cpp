/*
combatManager::EffectSpellCure cross-version expressions
========================================================

PoL 2.0 RVA 0x887c1; Buka 2.1 RVA 0x97879.

Complete clean VC4.2 product: 128/128
-------------------------------------
  build/effect-spell-cure-buka-manifest.json
  build/source-variant-batch/effect-spell-cure-buka/results.json

Independent axes:
  1. targetIndex != index
  2. curePoints * modifier * fightValue
  3. quantity * fightValue
  4. positive duration: sum < MAX ? sum : MAX
  5. negative duration: sum < MAX ? sum : MAX
  6. sideWork == IDX(m_currentSide) for effect sign
  7. sideWork == IDX(m_currentSide) for second-side traversal

All 128 combinations collapse to one candidate text/relocation state:
  state_count = 1
  size = 1057
  blocks = 58/58 exact skeleton
  relocations = 23/23
  text_sha = 9f8deff7070f53ed
  normalized_reloc_sha = 8158a003055c9152

The public matrix's 99.820890% is the known disposable-object undercount from
the function-local DATA_COMPGEN identity and anonymous switch-label addends.
The normalized mainline build is 99.970146%; its only two live byte differences
are the compare operand load order at the first target-index gate.

Final-source exact-state audit
------------------------------
  seed 7272, forest/top trial 7
  build/effect-spell-cure-replay-summary.json
  exact 100.000000%, retail size 1057, ordered 23/23 relocations
  source hash 8f77b22b7791; audited MAX 100 retained
  generated declarations absent from reconstructed source

Disposition: invariant matching-decomp port of the complete seven-axis Buka
expression family. No gameplay change.
*/
