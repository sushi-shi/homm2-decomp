/*
 * DoBlur structural experiment: vary ownership of the final blue component
 * reads, blue quantization, and the output palette lookup.
 *
 * This is the remaining channel transition after the red-transition,
 * green-transition, channel-seed, and pixel-tail matrices.  Six reviewed
 * forms were tested:
 *
 *   - the retained direct lookup;
 *   - blue quantization staged into a named lookup contribution;
 *   - a final-one inline helper with named and nested results;
 *   - a nested final-two inline helper;
 *   - one inline helper owning the final blue read and complete lookup.
 *
 * Representative final-one helper:
 *
 *     static inline u32 BlurFinishBlue(
 *         u32 sum, BlurComponentTable& table, i32 finalSample
 *     ) {
 *         sum += BLUR_COMPONENT(table, finalSample);
 *         return sum >> QUANTIZATION_SHIFT;
 *     }
 *
 * The complete helper/output matrix crossed every form with the clean state
 * and all 50 declaration-forest states: 6 * 51 = 306/306 compilations in
 * 260.27 seconds.  Source was restored after the matrix.
 *
 * Clean results:
 *   direct:                       92.854430%, size 1701, relocs 43/43
 *   staged blue:                  93.111820%, size 1701, relocs 43/43
 *   final-one named/nested:       94.124470%, size 1704, relocs 43/43
 *   final-two nested:             92.578060%, size 1707, relocs 43/43
 *   final-read plus full lookup:  85.755270%, size 1662, relocs 43/43
 *
 * Best state results:
 *   direct and staged blue:       95.729960%, size 1703
 *   final-one named:              94.812230%, size 1701
 *   final-one nested:             94.719406%, size 1701
 *   final-two nested:             94.719406%, size 1701
 *   final-read plus full lookup:  94.719406%, size 1701
 *
 * Direct staging canonicalizes to the retained target text in forest trials
 * 10 and 24.  At clean state, final-one and final-two ownership produce the
 * same text hashes as the corresponding green-transition helpers
 * (2f7dfed906c3ea26 and faa6aa5305422410).  Their inline boundaries do not
 * survive optimization as channel-specific schedules; their declarations
 * select known compiler states.  The staged-blue hash e5c49d6520bf2639 and
 * de-inlined complete-lookup hash 3bedae3759730b8b are distinct lower islands.
 *
 * Every best state retains the exact 29-block CFG, with 28 instruction-count
 * exact blocks and B10 alone size-only, and all 43 relocation targets are
 * present.  No arm reaches retail size 1700, exact ordered relocations, or
 * the durable 95.738396% MAX.
 *
 * Artifacts:
 *   build/doblur-blue-finalization-axes-20260727.json
 *   build/doblur-blue-finalization-states-20260727.json
 *   build/match-variants/doblur-blue-finalization-states-20260727/results.json
 *
 * Disposition: retain no source change.  Preserve the staged-blue and
 * complete-lookup objects as lower structural evidence; generated helpers,
 * declaration-forest probes, configuration changes, and MAX changes remain
 * disposable.
 */

void DoBlur_blue_finalization_output_ownership_record() {
}
