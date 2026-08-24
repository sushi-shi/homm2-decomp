// Reviewed structural matrix for heroWindowManager::Close's active guard.
// PoL: if (m_active == 1) { ... } with loop locals in the wrapped scope.
// Buka: if (m_active != 1) return; with the body/locals in function scope.
//
// Complete matrix:
//   build/hero-window-manager-close-head-axes.json
//   build/source-variant-batch/hero-window-manager-close-head/results.json
//   2 structural arms * (clean + 50 forest/top states) = 102/102 compiled;
//   no truncation; source restored.
//
// Every wrap and guard arm was exact at 69 bytes, 7 blocks, and 1/1
// relocation. Within all 51 paired states, text and raw/normalized relocation
// hashes were identical.
//
// Disposition: port the byte/relocation-invariant Buka guard and local scope.
