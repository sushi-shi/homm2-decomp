/*
 * IconToBitmapScale, RVA 0xd2f90: split bitmap accessors and whole-clear owner.
 *
 * The prior all-accessor experiment did not distinguish which member boundary
 * affected code generation.  This complete matrix tested the direct body,
 * width-only access, pixels-only access, and a helper owning allocation plus
 * the entire clear loop:
 */

#if 0
static inline i16 IconScaleBitmapWidth(bitmap* value) {
    return value->m_width;
}

static inline u8* IconScaleBitmapPixels(bitmap* value) {
    return value->m_pixels;
}

static inline bitmap* CreateIconScaleBitmap(void) {
    bitmap* result =
        new bitmap(BITMAP_TYPE_NONE, SCALE_WORK_BITMAP_SIZE, SCALE_WORK_BITMAP_SIZE);
    i32 rowOffset = 0;
    u8* pixels = result->m_pixels;
    do {
        memset(pixels + rowOffset, 0, SCALE_NATIVE_SIZE);
        rowOffset = rowOffset + SCALE_NATIVE_SIZE;
    } while (rowOffset < SCALE_NATIVE_SIZE * SCALE_WORK_BITMAP_SIZE);
    return result;
}
#endif

/*
 * Artifacts:
 *
 *   build/icon2bs-split-accessor-clear-manifest.json
 *   build/match-variants/icon2bs-split-accessor-clear/
 *
 * Complete product: four structural arms times clean plus 50 forest states,
 * 204 compilations.  Direct, width-only, and pixels-only are byte-identical
 * in clean state at 75.695040%, size 361.  The whole create-and-clear helper
 * opens a lower clean island at 73.276596%, size 363.  Every arm nevertheless
 * reaches the existing 85.829790% ceiling in at least one state, with all four
 * ordered relocations.  No arm changes the ceiling or reproduces retail's
 * 0x18 frame, so none was retained.
 */

void IconToBitmapScale_split_accessor_and_clear_owner_attempts() {
}
