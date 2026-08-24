// Reviewed structural matrix for mouseManager::Close's active guard.
// Historical PoL: if (m_active == 1) { ... }
// Buka/current PoL: if (m_active != 1) return; ...
// The Buka guard was already ported by commit ebee4529.
//
// Complete matrix:
//   build/mouse-manager-close-head-axes.json
//   build/source-variant-batch/mouse-manager-close-head/results.json
//   2 structural arms * (clean + 50 forest/top states) = 102/102 compiled;
//   no truncation; source restored.
//
// Every paired state emitted identical text and raw/normalized relocation
// hashes for wrap and guard. All states were 99.692310%, 237 bytes, 17 blocks,
// and 18/18 raw relocations (14/14 externally owned in `homm2 relocs`). The
// residual is an unrelated DestroyIcon/DeleteObject IAT register swap.
//
// Disposition: retain the already-ported, byte-invariant Buka guard clause.
