/*
 * IconToBitmapScale, RVA 0xd2f90: declaration-then-assignment setup.
 *
 * Representative source:
 *
 *   i32 step;
 *   i32 srcBase;
 *   i32 srcAdv;
 *   step = SCALE_NATIVE_SIZE / scale;
 *   srcBase = ((1 - scale) * step + SCALE_NATIVE_SIZE) >> 1;
 *   srcAdv = step * SCALE_WORK_BITMAP_SIZE;
 *
 * Complete clean + 50 sweep:
 *   build/icon2bs-separated-setup-manifest.json
 *   build/match-variants/icon2bs-separated-setup/
 *
 * This plausible older-C++ declaration style is byte-neutral in clean state
 * and converges to the same 85.829790% best state with 4/4 relocations.
 * It was removed.
 */

void IconToBitmapScale_separated_setup_declarations_attempt() {
}
