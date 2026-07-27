/*
 * IconToBitmapScale, RVA 0xd2f90: destination/scale owner attempts.
 *
 * Representative source:
 *
 *   bitmap* destination = dest;
 *   i32 scaledSize = scale;
 *
 * Every later use was routed through these owners. The two declaration
 * orders were also tested as a complete hand-authored axis.
 *
 * Complete matrices:
 *   build/icon2bs-dest-scale-owners-manifest.json
 *   build/match-variants/icon2bs-dest-scale-owners/
 *   build/icon2bs-owner-order-manifest.json
 *   build/match-variants/icon2bs-owner-order/
 *
 * The first matrix covered clean + 50 states. The order product covered
 * 2 * (clean + 50) = 102 compilations. Both orders were byte-identical
 * within a state. Best: 85.829790%, 4/4 relocations.
 *
 * A retained clue object from trial 27 moves destination ownership into ESI,
 * as retail does, but scale remains stack/EAX rather than retail EBX. It also
 * trades one size-only block for another: B7 becomes exact while B2 diverges.
 * No source was retained and MAX did not increase.
 */

void IconToBitmapScale_destination_scale_owner_attempts() {
}
