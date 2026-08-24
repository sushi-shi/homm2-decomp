// Reviewed structural matrix for AiPrint's debug-level guard.
// PoL: if (giDebugLevel >= FILE_DEBUG_LEVEL) { ... }
// Buka: if (giDebugLevel < FILE_DEBUG_LEVEL) return; ...
//
// Complete matrix:
//   build/ai-print-head-axes.json
//   build/source-variant-batch/ai-print-head/results.json
//   2 structural arms * (clean + 50 forest/top states) = 102/102 compiled;
//   no truncation; source restored.
//
// Every wrap and guard arm was exact at 108 bytes, 3 blocks, and 7/7
// relocations. Within all 51 paired states, text and raw/normalized relocation
// hashes were identical.
//
// Disposition: port the byte/relocation-invariant Buka guard clause.
