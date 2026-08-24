// combatManager::EffectSpellCreateCreature (PoL RVA 0x87b8e; Buka RVA 0x96f02)
//
// Artifact:
//   build/source-variant-batch/effect-spell-create-creature/results.json
// Coverage: complete 8 source arms x (clean + 50 forest/top states), 408/408.
// Seed: 1213156658. Generator untruncated; source restored.
//
// Fight-value-first and type-first products emit identical text for every
// paired name/state shape. The raw batch scores are 99.931880% with both PoL
// names, 99.907360% with only `spellPower`, and 99.885560% with `monType`
// (with or without `spellPower`). Keep both PoL slot-sensitive names.
//
// PoL adopts Buka's type-first product. Final normalized build: exact 844
// bytes, exact 32/32 CFG, complete 14/14 ordered relocations.
