/*
 * DoBlur exact-frame structural matrix: split redSum declaration from its
 * first assignment to extend accumulator ownership across sample acquisition.
 *
 * Arms:
 *
 *     // current
 *     u32 redSum = first + second;
 *
 *     // extended lifetime
 *     u32 redSum; // before all samples, or before the final three scalars
 *     ...
 *     redSum = first + second;
 *
 * Both split locations and initialized-at-first-use were crossed with the
 * clean state plus 50 target-adjacent TU states.
 *
 * Result (2026-07-27):
 *   complete matrix: 3 x 51 = 153/153
 *   trial 40: all three arms canonicalize to the identical 93.708860%,
 *     1705-byte register-perfect state with 43/43 relocations
 *   split forms at trial 33 reproduce text SHA 566cd2d231bdb25d, the same
 *     rejected 92.607600%, 1702-byte wrong-register island as named values
 *
 * Artifacts:
 *   build/doblur-exact-frame-red-accumulator-lifetime-axes.json
 *   build/doblur-exact-frame-red-accumulator-lifetime-manifest.json
 *   build/match-variants/doblur-exact-frame-red-accumulator-lifetime
 *
 * Disposition: rejected.  Accumulator declaration lifetime does not alter the
 * register-perfect sample prefix or final red reads.
 */
