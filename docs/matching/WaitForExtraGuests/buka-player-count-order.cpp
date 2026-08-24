// Reviewed comparison-order matrices for ws/dpWaitForExtraGuests.
// PoL: last-message player count != current player count
// Buka: current player count != last-message player count
//
// Complete matrices:
//   build/ws-wait-extra-guests-compare-axes.json
//   build/source-variant-batch/ws-wait-extra-guests-compare/results.json
//   build/dp-wait-extra-guests-compare-axes.json
//   build/source-variant-batch/dp-wait-extra-guests-compare/results.json
//   102/102 compiled per TU (204 total); no truncation; sources restored.
//
// Every one of the 51 order pairs in each TU had identical text and
// raw/normalized relocation hashes. Winsock: both arms exact at 184 bytes,
// 7 blocks, and 19/19 relocs. DirectPlay: both arms 99.722220% at 132 bytes,
// 4 blocks, and 14/14 relocs. Its remaining swapped-global relocation load
// order is compiler/TU state, not controlled by this commutative spelling.
//
// Disposition: port the byte/relocation-invariant Buka order to both TUs.
