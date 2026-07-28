/*
 * IconToBitmap, RVA 0xd0570: flat srcOffset structural branch.
 *
 * The retained trial-91 island shows retail reading entry->x and srcOffset
 * through flat [entries + 13*frame + disp] before materializing the entry
 * pointer. Re-examining the 164-arm flat-setup matrix per-arm: every flat
 * variant carries the exact retail relocation count (83) at size 1246,
 * while the pointer arms over-publish (86-87 relocations) and score
 * higher only through that inflation. A branch that cannot reach complete
 * ordered-relocation identity cannot close, so the flat spelling
 * (s_src = data + entries[frame].srcOffset) is adopted as the structural
 * parent even though its fuzzy ceiling is currently lower:
 *
 *   pointer branch: 79.68 max (relocs 86-87/83) - dead end
 *   flat branch:    79.14 max across 512 seed-606060 states,
 *                   exact 80/80 topology, 83/83 relocations
 *
 *   build/tu-state-noise/icon2b-trial91-retained-20260729/
 *   build/tu-state-noise/icon2b-flat-wide512-20260729/
 */

void IconToBitmap_flat_src_structural_branch_attempt() {
}
