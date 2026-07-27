/*
 * DoBlur structural matrix: explicit register ownership hints for the input
 * pointer and the three scalar kernel samples.
 *
 * This is a plausible spelling for a mid-1990s optimized pixel kernel, and it
 * had not been covered by the earlier declaration-order/lifetime matrices.
 * Eight source shapes were crossed with the clean state and exactly 50
 * declaration-forest states:
 *
 *   - no register qualifiers;
 *   - register on input only;
 *   - register on north-one, south-four, or north-four individually;
 *   - register on all three samples;
 *   - register on input plus north-one;
 *   - register on input plus all three samples.
 *
 * Result (2026-07-27):
 *   complete matrix: 8 * (clean + 50 states) = 408/408
 *   best: 95.729960%, size 1703, ordered relocations 43/43
 *   best states: forest trials 10 and 24
 *
 * Every register-qualified arm was byte-identical to the ordinary-local arm
 * at the same TU state.  MSVC 4.2 /O2 therefore discards these source hints
 * before the allocation decision that separates the candidate from retail.
 *
 * Artifacts:
 *   build/doblur-register-ownership-axes.json
 *   build/doblur-register-ownership-manifest.json
 *   build/match-variants/doblur-register-ownership
 *
 * Disposition: rejected.  The reconstructed source remains qualifier-free,
 * generated TU-state input was not retained, and the durable 95.729960% MAX
 * is unchanged.
 */
