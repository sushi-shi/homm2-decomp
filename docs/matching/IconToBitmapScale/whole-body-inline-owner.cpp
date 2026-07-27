/*
 * IconToBitmapScale, RVA 0xd2f90: whole-body inline ownership attempt.
 *
 * Earlier matrices tested native-path and non-native-path helpers separately.
 * This experiment instead moved the entire implementation behind one inline
 * owner so that inlined arguments could acquire entry-spanning lifetimes.
 * Four plausible parameter orders were tested:
 */

#if 0
static inline void IconToBitmapScaleBody(
    icon* source,
    bitmap* destination,
    i32 x,
    i32 y,
    i32 frame,
    IconDrawClipMode clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 scaledSize
) {
    // Native path, geometry, temporary bitmap, clear, scaled copy, delete.
}

void IconToBitmapScale(/* public ABI parameters */) {
    IconToBitmapScaleBody(/* reordered arguments */);
}
#endif

/*
 * The helper used source/destination/scale, destination/scale/source,
 * scale/destination/source, and source/scale/destination leading orders.
 *
 * Clean screening:
 *
 *   build/icon2bs-whole-body-inline-clean-axes-20260727.json
 *   build/match-variants/icon2bs-whole-body-inline-clean-20260727/
 *
 * All four helper orders canonicalize to the same 73.276596%/363-byte lower
 * island.  Direct source remains 75.695040%/361.
 *
 * Complete product over clean + 50 top forest states:
 *
 *   build/icon2bs-whole-body-inline-states-manifest-20260727.json
 *   build/match-variants/icon2bs-whole-body-inline-states-20260727/
 *
 * All 255 cells completed.  Direct and every helper order converge to the
 * existing 85.829790% ceiling at size 365 or 366 with 4/4 relocations.  The
 * helper never produces retail's 0x18 frame, destination/scale entry register
 * ownership, or a new block island.  No source was retained and MAX did not
 * change.
 */

void IconToBitmapScale_whole_body_inline_owner_attempt() {
}
