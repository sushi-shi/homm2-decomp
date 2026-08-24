// army::GetMoveMask @ PoL RVA 0x000bddd4
//
// Exact retail evidence before the source comparison:
//   PoL / VC4.2: 109 bytes, 8/8 exact blocks, ordered 1/1 relocations;
//                 return tail is one `or eax, 0xc0`.
//   Buka / VC6:   99 bytes, 7/7 exact blocks, ordered 1/1 relocations;
//                 return tail is `or al, 0x40; or al, 0x80`.
//
// Complete clean product:
//   build/source-variant-batch/get-move-mask-return/results.json
//   2/2 cells. `SPECIAL_DIRECTION_MASK` and the two named direction bits are
//   byte-, size-, topology-, and relocation-identical under VC4.2; both are
//   exact at 100.000000%.
//
// Disposition: invariant matching decomp. PoL adopts Buka's more semantic
// named-bit expression. No gameplay or shipped-byte change.
