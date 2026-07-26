/*
 * DoBlur reviewed permutation matrix: palette-loop local and update order.
 *
 * Axes:
 *   - paletteColor then componentOffset, or the reverse declaration order
 *   - componentOffset += 4 then paletteColor += 3, or the reverse update order
 *
 * Retail advances the palette pointer before the component-table byte offset,
 * while the retained source does the reverse.  All four combinations were
 * crossed with the same 50 target-adjacent TU-state probes on both the staged
 * lookup-index branch and the retained component-accessor branch.
 *
 * Result (2026-07-26), retained branch:
 *   clean fuzzy: 92.000000%, size 1708
 *   best: 93.411390%, size 1707, current declaration/update order
 *   reversed update order at the same state: 93.394516%
 *   ordered relocations: 43/43 in every ranked arm
 *
 * Artifacts:
 *   build/doblur-staged-descendants-axes.json
 *   build/doblur-retained-palette-order-manifest.json
 *   build/match-variants/doblur-retained-palette-order
 *
 * Disposition: retain the existing source order.  The retail-looking update
 * reversal improves local palette register spelling but slightly worsens the
 * complete target, so it is not a MAX improvement.
 */
