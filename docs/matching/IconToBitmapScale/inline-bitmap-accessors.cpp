/*
 * IconToBitmapScale, RVA 0xd2f90: sibling-style bitmap accessors.
 *
 * BITMAP.cpp already uses this family, so the attempted boundary was:
 *
 *   static inline i16 IconScaleBitmapWidth(bitmap* value) {
 *       return value->m_width;
 *   }
 *   static inline u8* IconScaleBitmapPixels(bitmap* value) {
 *       return value->m_pixels;
 *   }
 *
 * The width load and all three pixel-pointer loads used the accessors.
 * Complete clean + 50 sweep:
 *   build/icon2bs-inline-bitmap-accessors-manifest.json
 *   build/match-variants/icon2bs-inline-bitmap-accessors/
 *
 * This does form a distinct clean island: 73.709220%, size 363. State trials
 * nevertheless converge to the existing 85.829790% ceiling, 4/4
 * relocations. The accessors were removed.
 *
 * Future structurally distinct families, if evidence justifies another pass:
 * test width-only versus pixels-only accessor boundaries; split temporary
 * allocation/initialization lifetimes; and test indexed row traversal as a
 * deliberately lower island before permuting its internal spellings.
 */

void IconToBitmapScale_inline_bitmap_accessors_attempt() {
}
