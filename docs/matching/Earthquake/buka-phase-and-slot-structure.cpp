// combatManager::Earthquake (SOURCE/SPELLS, RVA 0x29ae0)
//
// Baseline dossier:
//   build/earthquake-cross-version-state-summary.json
//   build/tu-state-noise/earthquake-cross-version
// Clean PoL began at 97.206140%, 2276/2353 bytes, 69 blocks with four
// size-only bodies, and 62/62 relocation owners. Fifty forest/top probes found
// four states and no improvement.
//
// Shake-pointer product (102/102, complete):
//   build/earthquake-shake-pointers-axes.json
//   build/source-variant-batch/earthquake-shake-pointers
// Buka's inline X-first pointer family is worse clean and does not improve the
// structural frontier. Retain PoL's named destinationX/destinationY family.
//
// Phase product (408/408, complete):
//   build/earthquake-structural-axes.json
//   build/source-variant-batch/earthquake-structural
// Current pointer ownership crossed with the two impact-count shapes and two
// cloud-frame shapes. The winner is PoL pointers plus Buka's two individual
// keep-impact increments and its reused outer counter/recomputed cloud frame:
// 99.583336%, 2284 bytes, exact 69/69 masked CFG and 62/62 relocations.
//
// Buka's function-scope declarations are also structural evidence. Hoisting
// the working locals exposes one VC4.2 /Od scope. Aligned EBP operands recover
// the retail object order; scripts/homm2/core/od_slots.py then selects readable
// suffixed names for that order. The two Buka-evidenced unused locals supply
// the otherwise-missing eight frame bytes.
//
// Slot products:
//   build/earthquake-dead-local-axes.json
//   build/source-variant-batch/earthquake-dead-local       (256/256 complete)
//   build/earthquake-slot-layout-axes.json
//   build/source-variant-batch/earthquake-slot-layout      (102/102 complete)
// The dead-name bucket sweep proves their names are not the visible-slot fix.
// The hoisted retail-object-order arm reaches 99.728070% clean at exact retail
// size. The public batch's anonymous data labels require source-manifest
// normalization, so exact closure was audited with the unchanged retained
// source instead:
//
//   build/earthquake-retained-state-summary.json
//   build/tu-state-noise/earthquake-retained
//   build/earthquake-retained-trial3-summary.json
//   build/tu-state-noise/earthquake-retained-trial3
//
// Forest/top seed 1213156658 trial 3 is exact: 2353 bytes, exact 69-block CFG,
// zero retail-byte delta, and the complete ordered 62-relocation stream after
// semantic DATA_COMPGEN normalization. MAX 100 is recorded for source hash
// 78be6d3939d2; generated declarations are not retained.
//
// Disposition: matching-decomp transfer only. Retain Buka's phase ownership,
// declaration scope and dead-slot evidence with PoL-specific VC4.2 names.
// Keep PoL's shake-pointer family. No behavior change.
