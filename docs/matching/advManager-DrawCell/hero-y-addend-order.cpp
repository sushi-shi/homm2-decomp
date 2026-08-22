// VC6-measured complete matrix for advManager::DrawCell, RVA 0x00405fc9.
//
// Evidence before the matrix:
//   object bytes/CFG: exact after relocation masking, 299/299 blocks
//   ordered relocs: 554/554, with eight balanced transpositions
//   source arm: s_drawHeroYOffset + s_drawPixelY + CELL_LAST_PIXEL
//   retail order: load s_drawHeroYOffset, then s_drawPixelY
//   candidate order: load s_drawPixelY, then s_drawHeroYOffset
//
// Complete two-arm axis:
//   build/draw-cell-reloc-axes.json
//   build/draw-cell-reloc-manifest.json
//   build/source-variant-batch/draw-cell-relocs/results.json
// No AST mutations or TU-state probes were mixed into the source axis.
//
// Results:
//   hero offset first (clean): 99.967150%, size 0x245e, relocs 554/554
//   pixel Y first:             100.000000%, size 0x245e, relocs 554/554
//                              299/299 blocks, first divergence none
//
// Retained spelling at all eight semantically commutative sites:
//   s_drawPixelY + s_drawHeroYOffset + CELL_LAST_PIXEL
//
// This is source order, not data-identity steering: both arms use the same two
// proven globals and the selected arm reproduces retail bytes and ordered relocs.
