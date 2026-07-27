/*
 * ReceiveRemotePlayerExit net-position comparison search, 2026-07-27.
 *
 * The current candidate has the retail 44-block skeleton, aligned frame,
 * 45/45 relocation count, and only one visible instruction-order residual:
 *
 *     candidate: load recipient; compare giThisNetPos
 *     retail:    load giThisNetPos; compare recipient
 *
 * A reviewed exact-span axis crossed both semantic comparison orientations
 * with clean plus 50 forest states:
 *
 *     recipient != giThisNetPos
 *     giThisNetPos != recipient
 *
 * All 102 cells completed.  VC4.2 canonicalized every paired orientation to
 * the same bytes; the best remained 99.478264%, size 866, with 45/45
 * relocations and no exact closure.
 *
 * Three structural ownership mechanisms then received independent complete
 * clean-plus-50 censuses:
 *
 *   - an inline IsOtherNetPosition predicate: best 97.278260%, size 891;
 *   - a per-iteration value snapshot of giThisNetPos: best 98.860870%,
 *     size 871;
 *   - a per-iteration reference owner bound to giThisNetPos: best
 *     98.604350%, size 872.
 *
 * The helper did not inline into the retail-sized family, while the value and
 * reference owners introduced non-retail loop storage.  All three contradict
 * the otherwise exact frame/size structure and were rejected.  Canonical
 * source and generated configuration were restored.
 *
 * Artifacts:
 *   build/receive-remote-player-exit-condition-manifest-20260727.json
 *   build/match-variants/receive-remote-player-exit-condition-20260727
 *   build/tu-state-noise/receive-remote-player-exit-inline-predicate-20260727
 *   build/receive-remote-player-exit-inline-predicate-states-20260727.json
 *   build/tu-state-noise/receive-remote-player-exit-loop-snapshot-20260727
 *   build/receive-remote-player-exit-loop-snapshot-states-20260727.json
 *   build/tu-state-noise/receive-remote-player-exit-global-reference-20260727
 *   build/receive-remote-player-exit-global-reference-states-20260727.json
 *
 * Disposition: retain the canonical direct comparison and the 99.478264%
 * hash-scoped MAX.  The remaining residual is a compiler-state load-owner
 * island, not evidence for a larger CFG or another semantic local.
 */

void ReceiveRemotePlayerExit_net_position_comparison_record() {
}
