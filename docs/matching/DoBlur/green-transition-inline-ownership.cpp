/*
 * DoBlur structural experiment: stage green quantization before blue
 * accumulation and vary ownership of the green-to-blue transition.
 *
 * This is the channel-symmetric boundary left open by the earlier red
 * transition and channel-seed matrices.  Five reviewed forms were tested:
 *
 *   - the retained direct final lookup;
 *   - direct green quantization into a staged lookup contribution;
 *   - an inline helper owning the final green table read and quantization;
 *   - an inline helper owning the final two green reads and quantization;
 *   - an inline handoff owning the final green read, quantization, and the
 *     first two blue table reads through a reference output.
 *
 * Representative one-read helper:
 *
 *     static inline u32 BlurFinishGreen(
 *         u32 sum, BlurComponentTable& table, i32 finalSample
 *     ) {
 *         sum += BLUR_COMPONENT(table, finalSample);
 *         return sum & ~QUANTIZATION_MASK;
 *     }
 *
 *     u32 greenLookup =
 *         BlurFinishGreen(greenSum, greenComponents, sample13);
 *     // blue accumulation
 *     *output++ = lookup[redSum & ~QUANTIZATION_MASK]
 *                       [greenLookup + (blueSum >> QUANTIZATION_SHIFT)];
 *
 * Every form was crossed with the clean state and all 50 declaration-forest
 * states: 5 * 51 = 255/255 complete compilations in 212.62 seconds.  Source
 * was restored after the matrix.
 *
 * Clean results:
 *   direct:                   92.854430%, size 1701, relocs 43/43
 *   staged green:             92.632910%, size 1701, relocs 43/43
 *   final-one inline helper:  94.124470%, size 1704, relocs 43/43
 *   final-two inline helper:  92.578060%, size 1707, relocs 43/43
 *   finish-and-seed helper:   92.854430%, size 1701, relocs 43/43
 *
 * Best state results:
 *   direct and staged green:  95.729960%, size 1703
 *   final-one helper:         94.702530%, size 1701
 *   final-two helper:         94.719406%, size 1701
 *   finish-and-seed helper:   94.719406%, size 1701
 *
 * All best arms retain the exact 29-block CFG, with 28 instruction-count
 * exact blocks and B10 alone size-only, and all retain 43/43 relocation
 * targets.  Direct staged green canonicalizes to the retained direct object
 * in forest trials 10 and 24.  The one-read helper's materially stronger
 * clean object is a credible new 1704-byte island, but its state census does
 * not reach the durable 95.738396% MAX.  No arm reaches retail size 1700 or
 * exact ordered-relocation closure.
 *
 * Artifacts:
 *   build/doblur-green-transition-axes-20260727.json
 *   build/doblur-green-transition-states-20260727.json
 *   build/match-variants/doblur-green-transition-states-20260727/results.json
 *
 * Disposition: preserve the complete helper islands as structural evidence,
 * but retain the existing reconstructed source.  No generated helper,
 * declaration-forest probe, configuration change, or MAX update is retained.
 */

void DoBlur_green_transition_inline_ownership_record() {
}
