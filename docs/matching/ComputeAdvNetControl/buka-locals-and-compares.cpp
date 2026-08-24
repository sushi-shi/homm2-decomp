// ComputeAdvNetControl (PoL RVA 0x69fc2; Buka RVA 0x13a54)
//
// Artifacts:
//   build/source-variant-batch/compute-adv-clean/results.json
//   build/source-variant-batch/compute-adv-compare/results.json
//   build/source-variant-batch/compute-adv-name/results.json
// Clean cross-product: complete 16/16. Comparison and name censuses: each
// complete 4 arms x (clean + 50 forest/top states), 204/204. Seed 1213156658;
// source restored; wall-time and generators untruncated.
//
// `currentPlayer -> myPlayer` pairs byte/relocation-identically in all 51
// states. `selectedPlayer -> selected` never pairs or closes: clean 99.794520%,
// alternate state orbit 96.534250%. Keep `selectedPlayer`.
//
// The combined Buka orientation for both while comparisons is clean-exact and
// pairs with the PoL orientation in 50/51 states. In the remaining state the
// Buka arm is exact 293 bytes while the PoL arm is 96.712326% and 295 bytes.
// Adopt both Buka comparisons together. `/Od` frame audit: 1/1 aligned.
//
// Final build: exact 293 bytes, exact 17/17 CFG, complete 16/16 ordered
// relocations.
