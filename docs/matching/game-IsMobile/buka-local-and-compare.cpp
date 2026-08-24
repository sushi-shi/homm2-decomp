// VC6 SP3 /Od cross-version source/state matrix.
// Matrix: build/source-variant-batch/is-mobile/results.json
// Manifest: build/is-mobile-manifest.json
// Complete: 204/204 variants; generator and wall-time truncation false.
//
// Buka candidate: mapCell* cell; mobility >= CalcTerrainCost(...).
// Result:
//   - Both comparison directions were exact in all 51 paired states when the
//     local remained cp (181 bytes, 7 blocks, 4/4 relocations).
//   - Both cell-name arms had zero exact states; best fuzzy score 99.852460%.
//
// Disposition: port the byte-invariant Buka comparison; retain the
// slot-sensitive PoL name cp.
