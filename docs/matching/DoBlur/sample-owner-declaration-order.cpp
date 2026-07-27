/*
 * DoBlur sample-owner declaration order, 2026-07-27.
 *
 * Prior matrices permuted input/remaining/output, the three scalar samples,
 * and row/pixel array scope independently.  They never moved the physical
 * thirteen-element sample-array declaration across those lifetime groups.
 * This experiment tested five reviewed declaration layouts and crossed each
 * with both output-publication forms:
 */

#if 0
// Strong exact-block lower island.
i32 sample15;
i32 sample14;
i32 sample13;
u8* input = destination->m_pixels + rowOffset + BORDER_RADIUS;
u8* output = source->m_pixels + rowOffset + BORDER_RADIUS;
i32 remaining = INTERIOR_COLUMN_COUNT;
i32 samples[SPILLED_ARRAY_SAMPLE_COUNT];
#endif

/*
 * The other layouts were:
 *
 *   array, scalars, input, count, output (retained);
 *   input, output, count, array, scalars;
 *   input, array, scalars, count, output;
 *   scalars, array, input, count, output.
 *
 * All 5 * 2 * 51 = 510/510 cells completed in 472.26 seconds without
 * truncation, and generated source was restored.
 *
 * The retained array-first/post-increment arm reproduced 95.729960%.
 * Input-before-array reached the same text island at forest trial 13, showing
 * that this declaration movement can be state-canonical.  Its clean state is
 * substantially better than retained clean source: 94.719406% versus
 * 92.854430%, both size 1701 and with 28 exact plus one size-only block.
 *
 * The important new island is scalars/input/output/count/array with ordinary
 * post-increment publication.  Forest trial 32 reaches 95.383965%, size 1703,
 * 43/43 relocation counts, and all 29/29 retail instruction-count blocks.
 * This is materially stronger than the prior exact-block publication family,
 * which topped out near 93.71%.  A focused official replay retained the paired
 * objects and confirms that the new island still has the 0xc60 frame and
 * component tables at 0x70; retail uses 0xc5c and 0x6c.  It therefore bridges
 * the high-score and exact-block orbits without eliminating the allocator
 * hole.
 *
 * The scalar-first clean state is contradictory (86.660340%, size 1661), so
 * the declaration order is not retained merely for its forest island.  No
 * arm exceeds the durable 95.729960% MAX.
 *
 * Artifacts:
 *   build/doblur-sample-owner-declaration-order-axes-20260727.json
 *   build/doblur-sample-owner-declaration-order-manifest-20260727.json
 *   build/match-variants/doblur-sample-owner-declaration-order-20260727
 *   build/tu-state-noise/doblur-scalars-before-array-trial32-20260727
 *
 * Disposition: retain canonical source and preserve the scalar-first
 * trial-32 paired object as a descendant base.  No generated source,
 * configuration change, or MAX change survives.
 */

void DoBlur_sample_owner_declaration_order_record() {
}
