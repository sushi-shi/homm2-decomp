/*
 * DoBlur reviewed permutation matrix: pixel-loop input/output tail shape.
 *
 * Tested four equivalent forms:
 *
 *     *output++ = lookup[...]; input++;
 *     input++; *output++ = lookup[...];
 *     *output = lookup[...]; input++; output++;
 *     input++; *output = lookup[...]; output++;
 *
 * The retained island increments the input pointer earlier than the source
 * spelling, and retail uses different input/output registers near the only
 * non-exact block tail.  Each form was therefore crossed with the same 50
 * target-adjacent TU-state probes rather than edited and compiled manually.
 *
 * Result (2026-07-26):
 *   current order and split output increment: 93.411390%, size 1707
 *   input-first forms at the same state: 93.018990%, size 1707
 *   ordered relocations: 43/43
 *
 * Artifacts:
 *   build/doblur-tail-order-axes.json
 *   build/doblur-tail-order-manifest.json
 *   build/match-variants/doblur-tail-order
 *
 * Disposition: retain the existing source.  Splitting the output increment is
 * compiler-canonical at the best state; moving input first reaches a lower
 * orbit and does not remove the target-local residual.
 */
