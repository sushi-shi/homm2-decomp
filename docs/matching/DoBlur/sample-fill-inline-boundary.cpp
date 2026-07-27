/*
 * DoBlur structural experiment: move the complete sixteen-sample acquisition
 * sequence behind one static inline helper.
 *
 * The helper accepted the input pixel pointer, the thirteen-element spilled
 * array, and references to the three scalar samples.  Its body preserved the
 * established acquisition order exactly.  Direct acquisition and the helper
 * form were each crossed with the clean state and exactly 50 declaration-
 * forest states.
 *
 * Result (2026-07-27):
 *   complete matrix: 2 * (clean + 50 states) = 102/102
 *   direct best: 95.729960%, size 1703, ordered relocations 43/43
 *   helper best: 77.751050%, size 1678, ordered relocations 43/43
 *
 * Unlike the smaller accessor and accumulator helpers, this helper exceeds
 * the MSVC 4.2 /Ob1 inline threshold.  It therefore creates a call boundary
 * inside the hot pixel loop and a radically different allocation island.
 * Retail's B10 is call-free, so this is evidence against whole-kernel
 * acquisition deinlining rather than a candidate for retention.
 *
 * A narrower follow-up moved only north-four, north-one, and south-four into
 * one reference-output helper.  The helper was invoked either at the original
 * early north-four position or at the late north/south positions.  Both call
 * placements received clean plus 50 states alongside the direct arm:
 *
 *   complete matrix: 3 * (clean + 50 states) = 153/153
 *   early helper best: 88.793250%, size 1667, relocations 43/43
 *   late helper best:  87.348100%, size 1668, relocations 43/43
 *
 * Even this three-load, four-argument helper does not enter the desired
 * inlined schedule under /Ob1.  Its much shorter target span demonstrates the
 * same out-of-line call boundary.  Smaller single-read helpers were already
 * classified separately and canonicalize to known lower islands.
 *
 * Artifacts:
 *   build/doblur-sample-fill-helper-axes.json
 *   build/doblur-sample-fill-helper-manifest.json
 *   build/match-variants/doblur-sample-fill-helper
 *   build/doblur-special-sample-helper-axes.json
 *   build/doblur-special-sample-helper-manifest.json
 *   build/match-variants/doblur-special-sample-helper
 *
 * Disposition: rejected.  The direct source is restored, no generated probe
 * input is retained, and the durable 95.729960% MAX is unchanged.
 */
