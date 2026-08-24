// combatManager::CastSpell (SOURCE/SPELLS, RVA 0x217be)
//
// Unchanged-source census (51/51):
//   build/cast-spell-magic-arrow-state-summary.json
//   build/tu-state-noise/cast-spell-magic-arrow
// Eight VC4.2 /Od states were observed. The clean state remains best at
// 99.759900%, 7864/7882 bytes and 270/270 relocation entries.
//
// Magic Arrow source/state product (204/204, complete):
//   build/cast-spell-magic-arrow-axes.json
//   build/source-variant-batch/cast-spell-magic-arrow-anchored
// Arms: plain or whole-case `if (1)`, crossed with the PoL or Buka order of
// SPELL_MAGIC_ARROW_DELAY * gfCombatSpeedMod[...]. All four arms are byte-
// identical within every clean-plus-50 forest/top state under VC4.2.
//
// Buka's exact VC6 /Od object emits `mov eax,1; test eax,eax; je <case end>`
// for the guard. VC4.2 folds both the guard and multiplication-order choice.
// Disposition: no PoL source port. This is a compiler/source-switch boundary,
// not evidence for the remaining PoL frame/jump-table residual.
