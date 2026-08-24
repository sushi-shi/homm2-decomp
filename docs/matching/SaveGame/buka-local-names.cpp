// Free SaveGame(), PoL RVA 0x64e9f (not game::SaveGame()).
//
// Current PoL locals:
//   result11, humanPlayerCount1, playerLocal, extension7, patternState,
//   requester2, dialogResult7
// Buka exact locals:
//   ok, nPlayers, i, suffix, pattern, req, status
//
// Complete reviewed source product:
//   build/save-game-buka-local-names-manifest.json
//   build/source-variant-batch/save-game-buka-local-names/results.json
//   One bundled seven-rename arm, clean VC4.2 state, 1/1 cell.
//
// Baseline: live 100.000000%, 588 bytes, exact 24/24 blocks, complete ordered
// 41/41 relocations. Buka-name arm: 99.821655%, same size, same CFG and
// relocation stream; the changed identifier buckets perturb local references.
//
// DISPOSITION: reject the Buka names for PoL and retain the live-exact source.
// The algorithms are equivalent. Buka's 23-block/45-relocation form reflects
// VC6 plus localized string/data ownership, not a gameplay change.
