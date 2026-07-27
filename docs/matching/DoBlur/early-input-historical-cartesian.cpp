/*
 * DoBlur Cartesian cross of the three historically strongest table-order
 * families with both sample partitions and the prefix ownership choices.
 *
 * On the early-input base the matrix crossed:
 *   - red/blue/green, red/green/blue, and blue/red/green tables;
 *   - alternate north-four-scalar and retail-frame west-four-scalar samples;
 *   - initialized and split lookup allocation;
 *   - component-then-palette and palette-then-component cursor updates.
 *
 * Every one of the 24 authored source shapes was compiled in the clean state
 * and all 50 forest states: 24 * 51 = 1224/1224.
 *
 * Best component-first measurements by main structural family:
 *   red/blue/green + alternate:    95.729960%, size 1703
 *   red/green/blue + alternate:    95.552740%, size 1703
 *   blue/red/green + alternate:    94.812230%, size 1701
 *   red/blue/green + retail-frame: 93.818565%, size 1707
 *   red/green/blue + retail-frame: 92.978905%, size 1706
 *   blue/red/green + retail-frame: 92.909290%, size 1701
 *
 * Thus the 95.552740% historical label reproduces exactly.  The historical
 * blue/red/green label 94.626580% is base-dependent: the same broad family on
 * the early-input base reaches 94.812230%.  Lookup initializer/split forms
 * are byte-identical pairwise.  Palette-first is consistently slightly lower.
 *
 * The low blue/red/green retail-frame trial-50 island is structurally notable:
 * all 29 blocks retain the retail instruction counts and branch targets even
 * though the fuzzy score is only 92.909290%.  It was therefore retained for
 * descendant ownership and lifetime experiments rather than fuzzy-pruned.
 *
 * Artifacts:
 *   build/doblur-early-input-historical-structures-cartesian-manifest.json
 *   build/match-variants/doblur-early-input-historical-structures-cartesian
 *
 * Disposition: no exact closure and no MAX increase.  Preserve the lower
 * retail-frame island as evidence; retain no generated source or TU state.
 */

void DoBlur_early_input_historical_cartesian_record() {
}
