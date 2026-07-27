/*
 * IconToBitmapScale, RVA 0xd2f90: geometry-record ownership attempt.
 *
 * Representative source:
 *
 *   struct ScaleGeometry {
 *       i32 sourceBase;
 *       i32 sourceAdvance;
 *   };
 *   i32 step = SCALE_NATIVE_SIZE / scale;
 *   ScaleGeometry geometry;
 *   geometry.sourceBase = ((1 - scale) * step + SCALE_NATIVE_SIZE) >> 1;
 *   geometry.sourceAdvance = step * SCALE_WORK_BITMAP_SIZE;
 *
 * A complete clean + 50 forest-state sweep is in:
 *   build/icon2bs-geometry-record-manifest.json
 *   build/match-variants/icon2bs-geometry-record/
 *
 * Best: 85.829790%, with 4/4 relocations. The same 18-block structural
 * orbit remained (13 exact-size, five size-only). No source was retained and
 * the historical MAX did not increase.
 */

void IconToBitmapScale_geometry_record_attempt() {
}
