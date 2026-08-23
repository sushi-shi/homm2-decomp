// Function: game::LoadGame
// Retail RVA/size: 0x0004e8d9 / 0x0a79
//
// Reviewed source-shape matrix:
//   build/load-game-strnicmp-axes.json
//   build/source-variant-batch/load-game-strnicmp/results.json
//
// Complete product: 4/4 clean-state variants (not truncated).
//   strnicmp / strnicmp:   99.934210%, size 2681, 158 relocations
//   strnicmp / _strnicmp:  99.940790%, size 2681, 158 relocations
//   _strnicmp / strnicmp:  99.940790%, size 2681, 158 relocations
//   _strnicmp / _strnicmp: 99.947365%, size 2681, 158 relocations
//
// Each _strnicmp arm independently selects the retail __strnicmp COFF symbol.
// The standalone batch's disposable relocation normalization did not call any
// arm exact, so its scores are ranking evidence only.  After retaining the
// double-_strnicmp arm, the full normalized build proves identical assembly,
// 57/57 exact blocks, retail size 0x0a79, and all 158 ordered relocations with
// no candidate-only site.
//
// Later final-link evidence superseded the source-spelling conclusion. The
// stripped PE proves one call destination but cannot distinguish the direct
// `_strnicmp` spelling from the OLDNAMES `strnicmp` alias. The retail producer
// census and monolithic-library extraction require the alias member at this
// checkpoint, so the reconstructed source uses `strnicmp`; paired relocation
// normalization keeps this ambiguity out of the function-byte verdict.

// Current link-compatible calls:
//
//     strnicmp(filename, "RMT", sizeof("RMT") - 1)
