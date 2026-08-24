// advManager::DrawCursor @ PoL RVA 0x0000d834
//
// PoL source before (two sites):
//   updateMaxY % 8 + ((cursorFrame & MASK) + FLAG_BASE)
// Buka exact / retained PoL source:
//   (cursorFrame & MASK) + updateMaxY % 8 + FLAG_BASE
//
// Complete clean product:
//   build/source-variant-batch/draw-cursor-flag-frame/results.json
//   2/2 cells; both arms have identical text and ordered relocations
//   100.000000%, 1528 bytes, 48/48 blocks, 61/61 relocations
//
// Disposition: invariant matching decomp; retain Buka source in PoL. No
// gameplay or shipped-byte change.

