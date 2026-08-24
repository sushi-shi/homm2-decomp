// Reviewed structural matrix for ChangeTilesetIndex's TilesetId selection.
// PoL:
//   if (index == MAPCELL_SPRITE_NONE)
//       t = TILESET_NONE;
//   else
//       t = tileset;
// Buka:
//   t = index != MAPCELL_SPRITE_NONE ? tileset : TILESET_NONE;
//
// Complete matrix:
//   build/full-map-change-tileset-head-axes.json
//   build/source-variant-batch/full-map-change-tileset-head/results.json
//   2 structural arms * (clean + 50 forest/top states) = 102/102 compiled;
//   no truncation; source restored.
//
// PoL if/else: 25 exact states, max 100%; clean 98.095024% at 747 bytes.
// Buka conditional: no exact state, max 98.280540%; clean 96.375565%.
// Both families retained 35 blocks and 2/2 relocations; exact PoL islands use
// the retail 746-byte size. The Buka family also emitted 746-byte states, but
// none had exact bytes.
//
// Disposition: do not port Buka's compiler/version-specific conditional head.
