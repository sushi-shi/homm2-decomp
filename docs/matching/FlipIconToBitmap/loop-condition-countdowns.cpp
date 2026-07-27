/*
 * FlipIconToBitmap loop-condition countdown matrix, 2026-07-27.
 *
 * Question: do the dim and literal loop decrements belong in the trailing
 * while condition rather than as separate statements in the loop body?
 *
 * The structural axes were:
 *
 *   - unclipped dim: count--; while (count != 0)
 *                    versus while (--count != 0);
 *   - clipped dim:   the same independent choice;
 *   - guarded unclipped literal loop:
 *                    k--; while (k != 0)
 *                    versus while (--k != 0).
 *
 * The dim transformations are semantically equivalent because each loop is
 * entered only after its dimCount > 0 guard. The literal transformation is
 * likewise guarded by cmd > 0.
 *
 * Clean structural screen:
 *
 *   body/body dim countdowns:              86.846150%;
 *   unclipped condition only:              87.055700%;
 *   clipped condition only:                87.055700%;
 *   both dim conditions:                   87.265250%.
 *
 * Every clean arm remained size 1251 with 79/81 relocations. The literal
 * condition choice was byte-identical for every corresponding dim choice.
 *
 * Artifacts:
 *
 *   build/iconf2b-loop-condition-countdowns-axes-20260727.json
 *   build/iconf2b-loop-condition-countdowns-clean-manifest-20260727.json
 *   build/match-variants/
 *     iconf2b-loop-condition-countdowns-clean-20260727/
 *
 * Complete state product:
 *
 * All eight source shapes received clean plus the same 50 top-inserted forest
 * states: 408/408 variants completed in 308.75 seconds without truncation,
 * and the source was restored.
 *
 *   build/iconf2b-loop-condition-countdowns-state-manifest-20260727.json
 *   build/match-variants/
 *     iconf2b-loop-condition-countdowns-state-20260727/
 *
 * Forest trial 46 produced a new 88.066315% MAX for both the original
 * body-decrement source and the unclipped-dim condition form. Both measured
 * size 1251, 79/81 relocations, and the existing 75-block graph versus
 * retail's 76. The clipped condition form reached only 87.891240% in that
 * state, whether used alone or with the unclipped condition. Literal --k
 * remained byte-identical in all 51 states.
 *
 * The retained source therefore moves only the unclipped dim decrement into
 * the condition. It improves the live clean score to 87.055700%, preserves
 * the best state and topology, and matches a common guarded do/while
 * countdown structure. The clipped condition and literal condition forms
 * are not retained.
 *
 * Trial 46 was replayed through the official MAX recorder. Paired objects,
 * assembly, diff, state summary, and reproducible probe manifest are at:
 *
 *   build/tu-state-noise/
 *     iconf2b-unclipped-dim-predecrement-trial46-record-max-20260727/
 *
 * The generated hash-scoped MAX is 88.0663 for
 * f12f7656df1f.d97a2149d483. No forest declaration was retained.
 */

#if 0
// Retained unclipped dim loop.
if (dimCount > 0) {
    gFlipCnt = dimCount;
    do {
        u8* dimPalette = gFlipDimPal;
        i32 px = *dp++;
        gFlipDimDst = dp;
        dp[-1] = dimPalette[px];
    } while (--count != 0);
}

// Rejected clipped-dim condition arm.
if (dimCount > 0) {
    gFlipCnt = dimCount;
    do {
        u8* dimPalette = gFlipDimPal;
        i32 px = *dp++;
        gFlipDimDst = dp;
        dp[-1] = dimPalette[px];
    } while (--count != 0);
}

// Byte-neutral guarded literal arm.
if (cmd > 0) {
    i32 k = cmd;
    do {
        u8 c = *src++;
        *dst-- = c;
        gFlipDst = dst;
    } while (--k != 0);
}
#endif
