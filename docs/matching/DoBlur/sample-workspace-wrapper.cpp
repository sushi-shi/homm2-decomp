/*
 * DoBlur single-field sample-workspace wrapper, 2026-07-27.
 *
 * The thirteen spilled samples were wrapped without changing their indexing:
 */

#if 0
struct SBlurSampleWorkspace {
    i32 values[SPILLED_ARRAY_SAMPLE_COUNT];
} sampleWorkspace;

// Every former samples[index] expression directly names
// sampleWorkspace.values[index].
#endif

/*
 * This differs from the previously tested horizontal/vertical grouped sample
 * record: it preserves one flat array, has no accessor, and changes only the
 * compiler-visible physical owner.  Flat/wrapped storage was crossed with
 * array-first/scalar-first declarations, post/pre-increment publication, and
 * clean plus 50 top forest states:
 *
 *   4 * 2 * 51 = 408/408 cells in 375.45 seconds, untruncated.
 *
 *   storage/order          publication       clean       best
 *   flat array-first       post-increment    92.854430   95.729960
 *   flat scalar-first      post-increment    86.660340   95.383965
 *   wrapped array-first    post-increment    92.938820   94.481010
 *   wrapped scalar-first   post-increment    92.092830   94.812230
 *
 * Pre-increment descendants top out at 93.483120% or below.
 *
 * The wrapped array-first clean object is a genuine all-29-instruction-count
 * island at 92.938820%, size 1703.  Forest trials 2 and 28 form a second
 * useful clue at exact retail size 1700 and 94.430380%, but B10 remains the
 * sole size-only block.  The wrapper therefore supplies exact-block and
 * exact-length islands in different states; it does not make them coexist,
 * eliminate the known frame hole, or exceed MAX.
 *
 * Artifacts:
 *   build/doblur-sample-workspace-axes-20260727.json
 *   build/doblur-sample-workspace-manifest-20260727.json
 *   build/match-variants/doblur-sample-workspace-20260727
 *
 * Disposition: preserve both wrapper islands as structural evidence and
 * retain the flat array source.  No generated macro/type, configuration
 * change, or MAX change survives.
 */

void DoBlur_sample_workspace_wrapper_record() {
}
