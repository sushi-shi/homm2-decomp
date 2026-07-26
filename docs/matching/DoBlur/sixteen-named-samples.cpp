/*
 * DoBlur structural experiment: model all sixteen kernel values as named
 * scalar locals and let VC4.2 choose the 13+3 spill/register partition.
 *
 * Attempted declarations:
 *
 *     i32 sample0;
 *     // ...
 *     i32 sample15;
 *
 * Assignments and channel sums kept the established semantic order.  This is
 * more plausible original C++ than manually choosing thirteen array elements,
 * but the compiler shrinks the clean frame and explores highly variable frame
 * states under otherwise identical TU-state probes.
 *
 * Result (2026-07-26):
 *   clean fuzzy: 85.959915%, size 1678
 *   best of 50 target-adjacent states: 92.968350%, trial 30, size 1702
 *   CFG: same 29-block family
 *   relocation targets: 43/43
 *
 * Artifacts:
 *   build/doblur-sixteen-scalars-target-50.json
 *   build/tu-state-noise/doblur-sixteen-scalars-target-50
 *
 * Disposition: rejected.  No state approaches the retained 95.552740% MAX,
 * and the clean frame is inconsistent with retail's proven 0xc5c-byte frame.
 */
