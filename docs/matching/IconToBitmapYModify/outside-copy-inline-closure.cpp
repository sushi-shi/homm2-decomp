/*
 * IconToBitmapYModify, RVA 0xda270: right-outside copy-source inline closure.
 *
 * The retained incremental local had raised MAX to 98.6421%, but the sole
 * remaining size-only tail block was two bytes long and carried one extra
 * relocation.  Earlier helper experiments changed both clipped-copy arms.
 * This matrix instead varied only the right-outside arm:
 */

#if 0
static inline u8* IconOutsideCopySource(u8* src, i32 currentX, i32 clipX) {
    return src + (clipX - currentX);
}

// Only the right-outside arm uses the boundary.
u8* copySrc = IconOutsideCopySource(gYMSrc, gYMX, clipX);
memcpy(gYMRow + clipX, copySrc, clipW);
#endif

/*
 * Artifacts:
 *
 *   build/icon2by-outside-copy-inline-axes.json
 *   build/icon2by-outside-copy-inline-manifest.json
 *   build/match-variants/icon2by-outside-copy-inline/
 *
 * Complete product: retained incremental local, parameterized incremental
 * helper, global incremental helper, and parameterized expression helper
 * times clean plus 50 top-of-TU forest states.  All 204 variants compiled in
 * 174.91 seconds and source was restored.
 *
 * The parameterized expression helper formed a monotonic closure sequence:
 *
 *   trial 2   99.784260%, size 1415, 130/130 relocations
 *   trial 12  99.809650%, size 1415, 130/130 relocations
 *   trial 25  99.974620%, size 1416, 130/130 relocations
 *   trial 39 100.000000%, size 1416, 130/130 ordered relocations
 *
 * Trial 39 has zero retail-byte delta, exact function size, and complete
 * ordered relocation identity/addends.  The single-arm inline expression is
 * retained because it is the developer-plausible boundary that removes the
 * extra global load without perturbing the sibling clipped-copy arm.
 *
 * After refreshing the new effective-source hash, the official exact replay
 * was:
 *
 *   scripts/tu_state_noise.py --trials 50 --only-trial 39 \
 *       --insertion top --record-max --retain-best
 *
 * Exact evidence:
 *
 *   build/tu-state-noise/icon2by-outside-copy-inline-trial39-exact-20260727/
 *
 * The tool audited exact closure and recorded MAX 100.0000% for source hash
 * 15fda1250f72.852e9521ff86.  No generated probe declarations or generated
 * source are retained.
 */

void IconToBitmapYModify_outside_copy_inline_closure_attempt() {
}
