/*
 * IconToBitmapScale, RVA 0xd2f90: whole-loop owner and indexed traversal.
 *
 * Three genuinely different copy-loop structures were compared with the
 * retained carried-row-pointer implementation:
 *
 *   1. one static inline helper owning pitch, row pointers, and both loops;
 *   2. a guarded do-loop indexed by row number;
 *   3. nested row/column for-loops using indexed row addresses.
 *
 * Representative indexed address:
 */

#if 0
u8* dstPix = dest->m_pixels + (y + rowIndex) * pitch + x;
u8* srcPix =
    tmp->m_pixels + srcBase * SCALE_WORK_BITMAP_SIZE + srcBase + rowIndex * srcAdv;
#endif

/*
 * Artifacts:
 *
 *   build/icon2bs-copy-control-manifest.json
 *   build/match-variants/icon2bs-copy-control/
 *
 * Complete product: four structural arms times clean plus 50 forest states,
 * 204 compilations.
 *
 * The whole-loop inline owner is a real new clean island: 81.709220%, size
 * 365, versus the direct clean 75.695040%/361.  Its best forest states
 * converge to the existing 85.829790% ceiling, size 365.
 *
 * Indexed do traversal starts at 72.539010%/383 and peaks at 78.496450%.
 * Indexed for traversal starts at 71.985820%/374 and peaks at 82.028366%,
 * size 397.  Both are coherent lower structural islands, but neither matches
 * retail's carried source/destination row updates.  All arms retain 4/4
 * relocation identity.  No source was retained and MAX did not change.
 */

void IconToBitmapScale_copy_control_island_attempts() {
}
