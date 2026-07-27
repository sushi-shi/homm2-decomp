/*
 * DoBlur exact-frame structural matrix: copy the final three red component
 * entries into per-pixel named values before adding them to redSum.
 *
 * Representative arm:
 *
 *     i32 redWestFour = BLUR_COMPONENT(redComponents, sample13);
 *     i32 redNorthOne = BLUR_COMPONENT(redComponents, sample14);
 *     i32 redSouthFour = BLUR_COMPONENT(redComponents, sample15);
 *     redSum += redWestFour;
 *     redSum += redNorthOne;
 *     redSum += redSouthFour;
 *
 * Direct access and all six natural acquisition orders were crossed with the
 * clean state plus 50 target-adjacent TU states.
 *
 * Result (2026-07-27):
 *   complete matrix: 7 x 51 = 357/357
 *   register-perfect trial 40: every arm canonicalized to the direct
 *     93.708860%, 1705-byte state with 43/43 relocations
 *   trial 9 opened a distinct 92.607600%, 1702-byte state
 *
 * The shorter state is not a retail-compatible descendant.  Its input pointer
 * is ESI rather than retail EDX, and the final sample ownership becomes
 * west4=EBP, north1=ECX, south4=EDX.  Thus it loses the exact sample-load
 * prefix before reaching the red tail.
 *
 * Artifacts:
 *   build/doblur-exact-frame-red-final-values-axes.json
 *   build/doblur-exact-frame-red-final-values-manifest.json
 *   build/match-variants/doblur-exact-frame-red-final-values
 *   build/tu-state-noise/doblur-exact-frame-red-values-trial9
 *
 * Disposition: rejected.  Named copied values cannot steer the register-perfect
 * tail, and the only distinct shorter orbit contradicts retail ownership.
 */
