/*
 * IconToBitmapScale, RVA 0xd2f90: explicit register-owner screening.
 *
 * The retail entry keeps destination in ESI, scale in EBX, and step in EBP.
 * Old MSVC source in this repository sometimes uses the C++ `register`
 * storage hint, so eight clean combinations were compiled:
 */

#if 0
register bitmap* destination = dest;
register i32 scaledSize = scale;
register i32 step = SCALE_NATIVE_SIZE / scaledSize;
#endif

/*
 * Destination and scale were independently direct or register-qualified, and
 * step was independently ordinary or register-qualified:
 *
 *   build/icon2bs-register-owners-clean-axes-20260727.json
 *   build/match-variants/icon2bs-register-owners-clean-20260727/
 *
 * All eight source forms are byte-identical: 75.695040%, size 361, 4/4
 * relocations, and the same 18-block object.  The aliases and storage hints
 * are eliminated before allocation and therefore do not define a distinct
 * structural island worth a TU-state product.  No source was retained and
 * MAX did not change.
 */

void IconToBitmapScale_register_owner_keyword_attempts() {
}
