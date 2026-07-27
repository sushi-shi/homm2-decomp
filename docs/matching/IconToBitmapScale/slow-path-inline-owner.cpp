/*
 * IconToBitmapScale, RVA 0xd2f90: complete non-native-path inline owner.
 *
 * Retail owns dest in ESI and scale in EBX from entry, unlike the best
 * reconstructed islands.  To test whether a larger inlined parameter boundary
 * selected that ownership, the complete non-native path was moved into:
 */

#if 0
static inline void DrawScaledIconBody(
    icon* source,
    bitmap* destination,
    i32 x,
    i32 y,
    i32 frame,
    i32 scale
) {
    // Geometry, bitmap allocation, 0x800-byte clear, IconToBitmap,
    // scaled copy loops, and deletion all remain inside this helper.
}

if (scale == SCALE_NATIVE_SIZE) {
    IconToBitmap(srcIcon, dest, x, y, frame, clip, clipX, clipY, clipW, clipH, 0);
    return;
}
DrawScaledIconBody(srcIcon, dest, x, y, frame, scale);
#endif

/*
 * Artifacts:
 *
 *   build/icon2bs-slow-path-owner-manifest.json
 *   build/match-variants/icon2bs-slow-path-owner/
 *
 * Complete product: two structural arms times clean plus 50 forest states,
 * 102 compilations.  The helper inlines successfully.  Its clean island is
 * 73.801420%, size 365, and six forest states reach the existing
 * 85.829790% ceiling at size 365 or 366.  Relocations remain 4/4.
 *
 * The boundary therefore changes compiler state but still converges to the
 * same non-retail register/frame orbit.  It was not retained.
 */

void IconToBitmapScale_slow_path_inline_owner_attempt() {
}
