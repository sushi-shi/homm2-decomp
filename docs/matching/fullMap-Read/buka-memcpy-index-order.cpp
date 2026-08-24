// Reviewed source matrix for fullMap::Read's conversion memcpy arguments.
//
// PoL:
//   cells + width * y + x
//   tmp + width * y + x
// Buka:
//   cells + x + y * width
//   tmp + x + y * width
//
// Complete matrix:
//   build/full-map-read-memcpy-axes.json
//   build/source-variant-batch/full-map-read-memcpy/results.json
//   2 destination orders * 2 source orders * (clean + 50 forest/top states)
//   = 204/204 compiled; no truncation; source restored.
//
// Every state paired all four arms with identical text and raw/normalized
// relocation hashes. Each arm reached exact in the same seven states, at
// 661 bytes, 22 blocks, and 16/16 relocations.
// Exact current-source replay:
//   build/full-map-read-buka-state-summary.json
//   build/tu-state-noise/full-map-read-buka
// Forest/top seed 1213156658 trial 1 closed exactly and recorded the new
// source hash's audited 100% maximum. The clean state retains the exact
// 22-block topology and 16/16 relocation identity at 97.239820%.
//
// Disposition: port the byte/relocation-invariant Buka index order.
