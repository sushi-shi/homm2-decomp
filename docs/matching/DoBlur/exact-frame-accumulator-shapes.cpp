/*
 * DoBlur exact-frame accumulator-shape matrices.
 *
 * The register-perfect inner-scalar branch was tested with:
 *   - every source order of the final three red += statements;
 *   - grouped, temporary, and left/right red-tail expression trees;
 *   - one full sixteen-term red expression;
 *   - repeated versus single-tail green and blue accumulations, crossed as
 *     four source shapes against the clean state plus 50 TU-state probes.
 *
 * Representative attempted shape:
 *
 *     greenSum += term2 + term3 + ... + term15;
 *     blueSum  += term2 + term3 + ... + term15;
 *
 * Results (2026-07-26):
 *   red tail statement order: 306/306, byte-identical per state
 *   red tail tree: 357/357, byte-identical at the relevant trial
 *   full red expression: 102/102; trial 40 fell to 93.683550% and MSVC
 *     scrambled the first thirteen red stack reads
 *   green/blue shape cross: 204/204; best 93.708860%
 *
 * The green single-expression deltas begin at target offset 0x319 and blue
 * deltas at 0x3e5.  Therefore neither changes the earlier exact sample loads
 * or the red tail.  Blue is score-neutral at trial 40; green is 93.706750%.
 *
 * Artifacts:
 *   build/doblur-exact-frame-inner-red-tail-order-manifest.json
 *   build/doblur-exact-frame-inner-red-tail-tree-manifest.json
 *   build/doblur-exact-frame-inner-full-red-expression-manifest.json
 *   build/tu-state-noise/doblur-exact-frame-inner-single-expression-trial40
 *   build/doblur-exact-frame-inner-green-blue-accumulator-manifest.json
 *   build/match-variants/doblur-exact-frame-inner-green-blue-accumulator
 *
 * Disposition: pruned.  Repeated red/green/blue accumulation is the closest
 * known schedule on this island; no local expression spelling fixes the final
 * red register order.
 */
