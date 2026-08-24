// combatManager::Armageddon (SOURCE/SPELLS, RVA 0x24449)
//
// Unchanged-source census (51/51):
//   build/armageddon-cross-version-state-summary.json
//   build/tu-state-noise/armageddon-cross-version
// Eleven VC4.2 states were observed. Forest/top trial 47 reproduces the
// historical 99.687065% maximum at retail size, 92/92 blocks and 79 relocs.
//
// Pointer-order product (204/204, complete):
//   build/armageddon-pointer-order-axes.json
//   build/source-variant-batch/armageddon-pointer-order
// Source and destination X/Y ownership are independent axes over clean plus
// 50 states. Source order is byte-neutral. Buka's destination X-then-Y order
// improves every paired state; trial 47 rises to 99.690560% with unchanged
// size, topology and relocation count. That destination structure is retained.
//
// Source-comparison product (204/204, complete):
//   build/armageddon-source-comparisons-axes.json
//   build/source-variant-batch/armageddon-source-comparisons
// Both source `> 0` / `>= 0` choices are byte-neutral in every state. PoL keeps
// its current comparisons and source order. Direct normalized build remains
// 2724 bytes, 92 blocks and ordered 79/79 relocations.
//
// Disposition: matching-source transfer only; no behavior change. Remaining
// islands are army-address arithmetic and two loop/restore comparisons.
