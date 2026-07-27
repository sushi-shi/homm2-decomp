/*
 * IconToBitmapScale, RVA 0xd2f90: three-parameter owner attempt.
 *
 * Retail begins by spilling srcIcon, retaining dest in ESI, and retaining
 * scale in EBX. This attempt modeled all three source-level owners:
 *
 *   icon* source = srcIcon;
 *   bitmap* destination = dest;
 *   i32 scaledSize = scale;
 *
 * All later uses were routed through those names. Complete clean + 50 sweep:
 *   build/icon2bs-source-dest-scale-owners-manifest.json
 *   build/match-variants/icon2bs-source-dest-scale-owners/
 *
 * The aliases optimize away. Clean and every state reproduce already-known
 * islands; best is 85.829790%, 4/4 relocations. No source was retained and
 * MAX did not increase.
 */

void IconToBitmapScale_source_destination_scale_owner_attempt() {
}
