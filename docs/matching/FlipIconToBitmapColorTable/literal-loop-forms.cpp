// Experiment: unclipped literal-copy loop ownership and direction.
// Artifacts:
//   build/match-variants/iconf2bc-literal-loop-forms-clean-20260726/
//   build/tu-state-noise/iconf2bc-forward-index-50-20260726/
// Clean matrix:
//   countdown do-while       87.035260%
//   countdown while         byte-identical
//   countdown for           byte-identical
//   decrement in condition  byte-identical
//   forward index           86.934510%, same size/relocations/76-block CFG
// The forward-index form received 50 forest trials (37 unique states). Trials
// 12, 14, and 50 reached 87.035260%, but none beat official MAX 87.0479%.
// Disposition: retained countdown source on master; the forward-index shape is
// an active branch seed, not rejected merely for being below current MAX.

#if 0
// Countdown family: all four spellings canonicalized identically.
i32 k = cmd;
do {
    i32 c = *src++;
    dst--;
    gFCDst = dst;
    dst[1] = colorTable[c];
    k--;
} while (k != 0);

// Genuinely different lower island.
for (i32 k = 0; k < cmd; k++) {
    i32 c = *src++;
    dst--;
    gFCDst = dst;
    dst[1] = colorTable[c];
}
#endif
