// SOURCE/PATH select and compare donor family
//   army::GetAttackMask         @ PoL RVA 0x000bde41
//   army::ValidAttack           @ PoL RVA 0x000be217
//   army::GetAdjacentCellIndex  @ PoL RVA 0x000be48a
//
// All three Buka donors preserve gameplay values. Reviewed clean products:
//
//   build/source-variant-batch/get-attack-mask-select/results.json
//     2 blocked-mask shapes * 2 direction-count shapes * 2 loop comparisons
//     = 8/8 cells. Every arm is exact: 100.000000%, 181 bytes, 14/14
//     blocks, ordered 1/1 relocations.
//
//   build/source-variant-batch/valid-attack-select/results.json
//     2 west-facing tests * 2 east-facing tests * 2 assigned-side orders *
//     2 enemy-side orders = 16/16 cells. Every arm is exact: 100.000000%,
//     627 bytes, 46/46 blocks, ordered 11/11 relocations.
//
//   build/source-variant-batch/get-adjacent-cell-index-select/results.json
//     2 west-select shapes * 2 east-select shapes = 4/4 cells. Every arm is
//     byte/topology/relocation-identical at 89.850000%, 164 bytes, 13/13
//     blocks, ordered 1/1 relocations in the post-GetMoveMask TU state.
//     This target was exact before the preceding invariant source port; the
//     residual is therefore compiler state, not evidence against the donors.
//
// Retained-source compiler-state replay:
//   build/get-adjacent-cell-index-buka-selects-state-summary.json
//   build/tu-state-noise/get-adjacent-cell-index-buka-selects-replay
//   Forest/top trial 3 closes exactly at retail size 165, 13/13 blocks, and
//   ordered 1/1 relocations. The audited 100.000000% MAX is retained for the
//   final Buka-source hash; generated probe declarations are not retained.
//
// Disposition: invariant matching decomp. Adopt all Buka ternaries, facing
// equalities, and local-first target-side comparisons in PoL. The downstream
// GetAdjacentCellIndex state is recovered separately. No gameplay change.
