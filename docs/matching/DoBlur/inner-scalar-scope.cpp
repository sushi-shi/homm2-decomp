/*
 * DoBlur structural experiment: declare and initialize the three register
 * samples inside the pixel loop instead of declaring them in the row scope
 * and assigning them for each pixel.
 *
 * Attempted body:
 *
 *     i32 sample13 =
 *         static_cast<u32>(input[-BORDER_RADIUS]) << COMPONENT_INDEX_SHIFT;
 *     i32 sample14 =
 *         static_cast<u32>(input[-SCREEN_WIDTH]) << COMPONENT_INDEX_SHIFT;
 *     i32 sample15 =
 *         static_cast<u32>(input[SCREEN_WIDTH * BORDER_RADIUS])
 *         << COMPONENT_INDEX_SHIFT;
 *
 * All six declaration orders were crossed with the same 50 target-adjacent
 * TU-state probes.  This directly tested whether source lifetime/order could
 * reproduce retail's sample13=EBP, sample14=ESI, sample15=ECX allocation.
 *
 * Result (2026-07-26):
 *   clean fuzzy: 91.221520%, size 1708
 *   best of six complete 50-state arms: 92.805910%, size 1707
 *   CFG: same 29-block family
 *   ordered relocations: 43/43
 *
 * Artifacts:
 *   build/doblur-inner-scalar-order-axes.json
 *   build/doblur-inner-scalar-order-manifest.json
 *   build/match-variants/doblur-inner-scalar-order
 *   build/doblur-inner-scalar-scope-trial1.json
 *
 * Disposition: rejected.  The branch is structurally credible and its paired
 * evidence is retained, but every declaration order remains below the
 * 93.411390% MAX of the row-scope scalar form.
 */
