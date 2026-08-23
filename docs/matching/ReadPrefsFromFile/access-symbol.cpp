// Function: ReadPrefsFromFile
// Retail RVA/size: 0x000be340 / 0x00cb
//
// Reviewed source-shape matrix:
//   build/read-prefs-access-axes.json
//   build/read-prefs-access-manifest.json
//   build/source-variant-batch/read-prefs-access/results.json
//
// COMPLETE: 2/2 clean-state variants, no truncation, source restored.
// The source already carried the independently reviewed empty-string claims at
// 0x005360b8 and 0x005360bc. The two identifier spellings measured:
//
//   access(gText, 0):   99.636360%, size 203, relocs 26/26
//   _access(gText, 0):  99.727270%, size 203, relocs 26/26
//
// The disposable raw-object runner does not apply the repository's semantic
// DATA_COMPGEN names to the two fresh candidate $SG symbols, so neither raw
// score is an exactness verdict. The live normalized diff after retaining
// `_access` has no instruction or relocation divergence: all 203 bytes, seven
// blocks, and 26 ordered relocation sites are exact.
//
// Later final-link evidence superseded the source-spelling conclusion. The
// stripped PE proves that `_access` and `access` resolve to the same entry but
// cannot identify which COFF alias the caller used. The monolithic-library
// extraction and Rich producer census require the OLDNAMES `access` alias at
// this checkpoint, so the reconstructed source uses `access`. The paired
// comparison normalization keeps the function-byte proof independent of that
// unresolved stripped identity.

// Current link-compatible expression:
//
//     if (access(gText, 0) == -1) {
