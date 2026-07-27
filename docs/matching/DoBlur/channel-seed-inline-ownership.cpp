/*
 * DoBlur structural experiment: give the initial two component-table reads of
 * each red, green, and blue accumulator to a small inline helper.
 *
 * Attempted helper:
 *
 *     static inline u32 BlurSeedComponents(
 *         BlurComponentTable& table, i32 firstSample, i32 secondSample
 *     ) {
 *         return BLUR_COMPONENT(table, firstSample)
 *              + BLUR_COMPONENT(table, secondSample);
 *     }
 *
 * The direct form, each single channel, each pair of channels, and all three
 * channels were crossed with clean plus the same 50-state declaration forest:
 * 8 x 51 = 408/408 complete compilations.
 *
 * Result (2026-07-27):
 *   direct best: 95.563290%, size 1703, trial 50
 *   inline best: 94.713080%, size 1701, trial 2
 *   relocation counts: 43/43
 *   blocks: 29/29, 28 exact, B10 alone size-only
 *
 * Within one TU state, all seven inline ownership combinations emit identical
 * target text.  The helper body is fully canonicalized at the call sites; the
 * difference from the direct arm is parser-state perturbation from the helper
 * declaration, not a surviving B10 ownership boundary.  The direct best is
 * the already-known text SHA 0a540af53da4975f.
 *
 * Artifacts:
 *   build/doblur-channel-seed-inline-axes.json
 *   build/doblur-channel-seed-inline-manifest.json
 *   build/match-variants/doblur-channel-seed-inline
 *
 * Disposition: rejected.  No arm exceeds the 95.738396 durable MAX, no exact
 * 1700-byte result appears, and generated helper/state input is not retained.
 */

void DoBlur_channel_seed_inline_ownership_record() {
}
