// InitRandomArtifacts (SOURCE/GAME, RVA 0x7fc0a) — 2026-07-29 matrix -> MAX 100
//
// Matrix: build/ira-axes.json -> build/source-variant-batch/ira
// (2 arms x {clean + 8 forest states}, complete). Sibling matrices the same
// day: mawv (MakeAllWaterVisible — unchanged source re-closed, ledger already
// 100) and sms (SetMapSize — both orientations byte-identical and 2 bytes
// short; residual is elsewhere, still open).
//
// Axis outer_guard (retail: mov eax,[x]; cmp [MAP_WIDTH],eax — global-left
// encoding, one byte fatter than our moffs load):
//   local_left  `x < MAP_WIDTH`: canonicalized identical at the clean state.
//   global_left `MAP_WIDTH > x` (WINNER): byte-identical clean, but reached
//   audited exact at forest states — the orientation is a distinct search
//   coordinate even where the clean state canonicalizes it.
//
// Integrated global_left; official replay banked hash-scoped MAX
// 96.8421 -> 100.0000 (build/tu-state-noise/game-ira-rerun, trial 7).
// The inner `y < MAP_HEIGHT` guard already matched as spelled.
