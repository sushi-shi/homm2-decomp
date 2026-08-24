// HandleRemoteDeadPlayerExit @ PoL RVA 0x000a0234
//
// Source choices:
//   PoL:  giThisGamePos == pos
//   Buka: pos == giThisGamePos
//
// Complete clean product:
//   build/source-variant-batch/handle-remote-dead-player-exit-guard/results.json
//   2/2 cells. Both source orders are byte-, size-, topology-, and
//   relocation-identical under VC4.2 at 97.446810%, 144 bytes, 7/7 blocks,
//   and ordered 9/9 relocations.
//
// Retained Buka-source state closure:
//   build/handle-remote-dead-player-exit-buka-guard-state-summary.json
//   build/tu-state-noise/handle-remote-dead-player-exit-buka-guard-replay2
//   Forest/top trial 8 closes exactly at retail size 145, 7/7 blocks, and
//   ordered 9/9 relocations. Audited MAX 100.000000% is retained for source
//   hash 0804e0dbbf80; generated probe declarations are not retained.
//
// Disposition: invariant matching decomp. PoL adopts Buka's parameter-first
// equality. No gameplay change.
