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
 * Result (2026-07-26):
 *   clean fuzzy: 85.299576%, size 1612
 *   first forest state: 89.896620%, size 1707
 *   ordered relocations: 43/43
 *
 * Artifact:
 *   build/tu-state-noise/doblur-input-before-scalars-clean
 *
 * Disposition: rejected without a full island sweep.  Merely extending the
 * pointer lifetime moves the function into two much worse allocation orbits
 * and does not preserve retail's 1700-byte shape.
 */
