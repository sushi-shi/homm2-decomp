// Reviewed structural matrix for inputManager::Close's active guard.
// PoL: if (m_active == true) { ... }
// Buka: if (m_active != true) return; ...
//
// Complete matrix:
//   build/input-manager-close-head-axes.json
//   build/source-variant-batch/input-manager-close-head/results.json
//   2 structural arms * (clean + 50 forest/top states) = 102/102 compiled;
//   no truncation; source restored.
//
// Every wrap and guard arm was exact at 32 bytes, 3 blocks, and 0/0
// relocations. Within all 51 paired states, text and raw/normalized relocation
// hashes were identical.
//
// Disposition: port the byte/relocation-invariant Buka guard clause.
