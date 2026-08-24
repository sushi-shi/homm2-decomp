// town::Deallocate (PoL RVA 0x32f54; Buka RVA 0xa42d0)
//
// Artifacts:
//   build/source-variant-batch/town-deallocate-compare-position/results.json
//   build/source-variant-batch/town-deallocate-local-census/results.json
//   build/town-deallocate-final-state-summary.json
//   build/town-deallocate-final-replay-summary.json
//
// Comparison/position census: complete 8/8 clean shapes. Comparison order is
// byte-neutral; `position -> pos` drops 98.742270% to 98.608246%.
// Local-name census: complete 16/16 clean shapes. Buka `pp/pos` is
// 98.546394%; `pp` alone 98.474230%. Keep PoL playerRecord/position names.
//
// Final unchanged-source forest/top census, seed 1213156658: trial 14 closes
// exactly at 333 bytes, exact 18/18 CFG, zero retail-byte delta, and complete
// 2/2 ordered relocations. Replay retained MAX 100.0000 for hash feae64879cdc.
// PoL adopts only Buka's byte-neutral owner-first comparison.
