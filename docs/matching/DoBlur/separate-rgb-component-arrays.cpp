/*
 * DoBlur structural experiment: model the three 1 KiB component tables as
 * three named local arrays instead of one two-dimensional channel array.
 *
 * Retained declarations:
 *
 *     BlurComponentTable redComponents;
 *     BlurComponentTable greenComponents;
 *     BlurComponentTable blueComponents;
 *
 * Every component lookup names its channel array directly.  This matches the
 * retail frame evidence: three contiguous 0x400-byte regions addressed at
 * stack bases 0x6c, 0x46c, and 0x86c.
 *
 * Result (2026-07-26):
 *   clean fuzzy: 92.299576%, size 1708
 *   best of 50 target-adjacent states: 94.497890%, trial 45, size 1705
 *   CFG: same 29-block family; B10 is 219 versus retail 216 instructions
 *   ordered relocation targets: 43/43
 *
 * Artifacts:
 *   build/doblur-separate-rgb-target-50.json
 *   build/tu-state-noise/doblur-separate-rgb-target-50
 *   build/tu-state-noise/doblur-separate-rgb-trial45-recorded
 *
 * Disposition: retained.  The source is more explicit about the proven frame
 * layout, raises clean matching, reduces the only non-exact block by two
 * instructions, and raises historical MAX from 93.4114% to 94.4979%.
 * Trial 45 is disposable compiler-state input and is not retained in source.
 */
