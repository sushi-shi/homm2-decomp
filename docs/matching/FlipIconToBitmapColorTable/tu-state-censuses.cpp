// Experiment: unchanged-source compiler-state censuses.
// Artifacts:
//   build/tu-state-noise/iconf2bc-original-target-120-20260726/
//   build/tu-state-noise/iconf2bc-original-target-seed2-120-20260726/
//   build/tu-state-noise/iconf2bc-exact-flow-20260726/
// These varied disposable declaration forests at top/target insertion points
// under independent seeds. Historical best for the old effective source:
// 86.720406%. No exact state appeared.
//
// There is intentionally no probe code here. Probe declarations are generated
// disposable compiler input and are forbidden in reconstructed or preserved
// source. The effective function source was the unchanged baseline:

#if 0
void FlipIconToBitmapColorTable(
    icon* srcIcon,
    bitmap* dstBitmap,
    i32 frame,
    i32 x,
    i32 y,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 clip,
    u8* colorTable);
#endif
