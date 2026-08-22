// Function: searchArray::SeedPosition
// Retail RVA/size: 0x000917d6 / 0x0cc2
//
// Reviewed source-shape matrix:
//   build/seed-position-map-extra-axes.json
//   build/source-variant-batch/seed-position-map-extra-clean/results.json
//
// Complete product: 2/2 clean-state variants (not truncated).
//   MAP_EXTRA_AT_WFIRST(s_neighborX, s_neighborY): 99.987830%, size 3266
//   MAP_EXTRA_AT(s_neighborX, s_neighborY):        100.000000%, size 3266
//
// The batch runner included the adjacent _$E19/_$E18 initializer functions in
// its provisional end boundary, reporting four extra blocks and six extra
// relocations.  The retained normalized objects prove that SeedPosition itself
// spans candidate .text+0x116..0xdd8 and retail .text+0x118..0xdda: exactly
// 0xcc2 bytes on each side.  Those raw spans are byte-identical.  Their 286
// ordered relocation tuples (relative site, type, identity, addend) are also
// identical, and the 134 body blocks are exact.
//
// Disposition: ACCEPTED.  Retail uses the row-first multiply-order macro at
// this call site; the generated variants and TU-state declarations were not
// retained.

// Accepted expression:
//
//     || !(MAP_EXTRA_AT(s_neighborX, s_neighborY) & SEARCH_MAP_BLOCKED)
