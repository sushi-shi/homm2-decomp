// VC6 SP3 /Od cross-version source/state matrix.
// Matrix: build/source-variant-batch/can-buy-bhc/results.json
// Manifest: build/can-buy-bhc-manifest.json
// Complete: 816/816 variants; generator and wall-time truncation false.
//
// Buka candidate: j/index; num-first garrison compare; resources-first cost compare.
// Result:
//   - All four comparison combinations had identical bytes and raw/normalized
//     relocation hashes within every state.
//   - jb/idx was exact in all 204 cases (346 bytes, 25 blocks, 7/7 relocs).
//   - j/idx reached 99.955055%; either index family reached 99.898880%.
//
// Disposition: port both byte-invariant Buka comparisons; retain the
// slot-sensitive PoL names jb and idx.
