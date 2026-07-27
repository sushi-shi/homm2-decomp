/*
 * DoBlur structural descendants combining the retail-frame sample partition's
 * lower exact-instruction-count island with ownership mechanisms from the
 * stronger alternate-partition islands.
 *
 * First Cartesian:
 *   sample scalars at row scope / pixel scope
 *     x separate component sums / one RGB sum record
 *     x (clean + 50 forest states)
 *
 * Complete result: 4 * 51 = 204/204.
 *   pixel samples + RGB sum record: 95.054855%, size 1703, trial 25
 *   pixel samples + scalar sums:    94.833336%, size 1703
 *   row samples + RGB sum record:  94.044304%, size 1706
 *   row samples + scalar sums:     92.909290%, size 1701
 *
 * The hybrid is better than either parent, proving that these structural
 * mechanisms combine nontrivially rather than merely selecting TU states.
 *
 * The surviving pixel-scope/RGB-record shape was then crossed with the three
 * historically strongest component-table families.  Every family received
 * the clean state and all 50 probes: 3 * 51 = 153/153.
 *
 *   red/green/blue: 95.213080%, size 1703, trials 25 and 39
 *   red/blue/green: 95.054855%, size 1703
 *   blue/red/green: 95.054855%, size 1703
 *
 * The retained paired trial-25 object has the retail 29-block CFG.  B10 is
 * 217 instructions versus retail 216; the extra move appears at the start of
 * green accumulation.  Its input is ESI and its three special samples occupy
 * EBP/ECX/EDX, while retail uses input=EDX and samples EBP/ESI/ECX.  All 43
 * relocation counts are present, but the ordered relocation offsets/addends
 * still differ, so the fuzzy improvement is not closure.
 *
 * Evidence-led descendants of the 95.213080% shape:
 *
 *   input pointer direct / destination-pixel-base alias:
 *     2 * 51 = 102/102; alias best 94.991560%
 *
 *   direct palette loop / inline whole-table initializer:
 *     2 * 51 = 102/102; inline best 93.265820%
 *
 *   all six RGB sum-record field orders:
 *     6 * 51 = 306/306; byte-identical within every paired state
 *
 *   pixel-scope / row-scope sum-record lifetime:
 *     2 * 51 = 102/102; both reproduce the same 95.213080% trial-25 object
 *
 * Artifacts:
 *   build/doblur-early-input-exact-lifetime-sum-cartesian-manifest.json
 *   build/match-variants/doblur-early-input-exact-lifetime-sum-cartesian
 *   build/doblur-early-input-exact-pixel-sum-table-order-manifest.json
 *   build/match-variants/doblur-early-input-exact-pixel-sum-table-order
 *   build/tu-state-noise/doblur-early-input-exact-rgb-pixel-sum-trial25
 *   build/doblur-early-input-exact-rgb-pixel-sum-input-owner-manifest.json
 *   build/doblur-early-input-exact-rgb-pixel-sum-palette-helper-manifest.json
 *   build/doblur-early-input-exact-rgb-pixel-sum-field-order-manifest.json
 *   build/doblur-early-input-exact-rgb-pixel-sum-lifetime-manifest.json
 *
 * Disposition: preserve 95.213080% as a new lower structural island.  It does
 * not advance the durable 95.729960% MAX and is not retained in live source.
 */

void DoBlur_early_input_exact_hybrid_cartesian_record() {
}
