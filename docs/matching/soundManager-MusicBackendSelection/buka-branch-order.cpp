// soundManager::{Open, SetMusicQuality}
// PoL RVAs: 0xcc560, 0xcd160; Buka RVAs: 0xb5d20, 0xb63e0.
//
// Reviewed semantic branch orders:
//   Buka/current PoL: non-MIDI backend first, MIDI backend in else.
//   Alternative:       MIDI backend first, non-MIDI backend in else.
// Artifacts:
//   build/source-variant-batch/sound-open-branch-order/results.json
//   build/source-variant-batch/set-music-quality-branch-order/results.json
// Coverage: two complete 2/2 clean-source products; source restored.
//
// Open: current/Buka order exact, 936 bytes, 30/30 CFG, complete relocation
// closure; inverted order 99.544975% at the same size/count.
// SetMusicQuality: current/Buka order exact, 227 bytes, 15/15 CFG, ordered
// 8/8 relocations; inverted order 92.033900%.
// Disposition: matching decomp is already in PoL; no source edit required.
//
// Buka-only SetMusicQuality tail after storing gConfig.musicSource:
//   re-read gConfig.musicSource; start Audiere or Miles; replay old track.
// PoL has already-initialized CD/MIDI backends and only replays old track.
// Disposition: backend-rewrite logic; do not port to PoL.
