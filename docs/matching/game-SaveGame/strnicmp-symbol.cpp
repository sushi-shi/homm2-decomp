// Function: game::SaveGame
// Retail RVA/size: 0x0004d3ae / 0x0b5a
//
// Reviewed source-shape matrix:
//   build/save-game-strnicmp-axes.json
//   build/source-variant-batch/save-game-strnicmp/results.json
//
// Complete product: 8/8 clean-state variants (not truncated).
//   strnicmp / strnicmp / strnicmp:    99.914696%, size 2906, 131 relocations
//   one _strnicmp arm (each of three): 99.921260%, size 2906, 131 relocations
//   two _strnicmp arms (each pairing): 99.927820%, size 2906, 131 relocations
//   _strnicmp / _strnicmp / _strnicmp: 99.934380%, size 2906, 131 relocations
//
// Each _strnicmp arm independently selects the retail __strnicmp COFF symbol.
// The standalone batch's disposable relocation normalization did not call any
// arm exact, so its scores are ranking evidence only.  After retaining the
// triple-_strnicmp arm, the full normalized build proves identical assembly
// (893 instructions), 60/60 exact blocks, retail size 0x0b5a, and all 131
// ordered relocations with no candidate-only site.
//
// The same audit proves the generated-name extension objects at retail VAs
// 0x004f7560 ("GMC") and 0x004f756c ("GXC"); both are retained as
// DATA_COMPGEN owners in the reconstructed source.
//
// Disposition: ACCEPTED.  All three save-name prefix comparisons use
// _strnicmp; generated variants were not retained.

// Accepted calls:
//
//     _strnicmp(genName, "RMT", sizeof("RMT") - 1)
//     _strnicmp(genName, "AUTOSAVE", sizeof("AUTOSAVE") - 1)
//     _strnicmp(genName, "AUTOSV2", sizeof("AUTOSV2") - 1)
