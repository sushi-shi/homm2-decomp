/*
 * DoBlur structural experiment: begin the input-pointer lifetime before the
 * three register-resident kernel samples.
 *
 * Reviewed declaration shape:
 *
 *     i32 samples[SPILLED_ARRAY_SAMPLE_COUNT];
 *     u8* input = destination->m_pixels + rowOffset + BORDER_RADIUS;
 *     i32 sample15;
 *     i32 sample14;
 *     i32 sample13;
 *     i32 remaining = INTERIOR_COLUMN_COUNT;
 *     u8* output = source->m_pixels + rowOffset + BORDER_RADIUS;
 *
 * Motivation: in the retained source MSVC assigns input to ESI and sample14
 * to EDX, while retail assigns input to EDX and sample14 to ESI.
 *
 * The original 2026-07-26 spot check used an older structural base:
 *   clean fuzzy: 85.299576%, size 1612
 *   first forest state: 89.896620%, size 1707
 *
 * The structure was revisited on 2026-07-27 after retaining the alternate
 * sample partition and red/blue/green component-table order. Both lifetimes
 * were crossed with the clean state and exactly 50 forest states:
 *
 *   complete matrix: 2 * (clean + 50 states) = 102/102
 *   early-input clean: 94.719406%, size 1701, relocs 43/43
 *   retained-lifetime clean: 92.854430%, size 1701, relocs 43/43
 *   both best: 95.729960%, size 1703, relocs 43/43
 *
 * A later isolated replay corrected one statement from the matrix-era notes:
 * the early-input clean object assigns input to ECX, not retail EDX.  The old
 * wording had conflated the source lifetime with a different retained state.
 * The clean object still keeps the 29/29 CFG with only B10 size-different
 * (215 versus retail 216).  At trial 13 it canonicalizes to text SHA
 * 2745e498400d169d and normalized relocation SHA 9101b98c0d3519aa, exactly the
 * retained arm's trial-10 state.
 *
 * Artifacts:
 *   build/tu-state-noise/doblur-input-before-scalars-clean
 *   build/tu-state-noise/doblur-early-input-clean-replay
 *   build/doblur-input-lifetime-full-axes.json
 *   build/doblur-input-lifetime-full-manifest.json
 *   build/match-variants/doblur-input-lifetime-full
 *
 * Disposition: clue only. The revised structure is a much stronger clean
 * island and repairs the input register, but its complete state census does
 * not exceed or differ from the retained historical MAX island.
 */
