// soundManager::{AdjustMusicVolumes, MemorySample}
// PoL RVAs: 0xcd030, 0xcd7f0; Buka RVAs: 0xb6330, 0xb68d0.
//
// Reviewed arms: one combined || guard vs one return per leading term.
// Artifacts:
//   build/source-variant-batch/adjust-music-volumes-guard-shape/results.json
//   build/source-variant-batch/memory-sample-guard-shape/results.json
// Coverage: two complete 2/2 clean-source products; source restored.
//
// AdjustMusicVolumes: separate returns exact, 238 bytes, 18/18 CFG;
// combined guard 86.120690%, 228 bytes.
// MemorySample: separate returns exact, 655 bytes, 28/28 CFG;
// combined guard 95.783780%, 634 bytes.
// All arms preserve their matrix relocation counts.
// Disposition: authoritative Buka shape is already retained in PoL; no edit.
