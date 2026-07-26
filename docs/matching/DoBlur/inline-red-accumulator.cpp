/*
 * DoBlur inline-boundary experiment: extract the sixteen-term red-channel
 * accumulation into `static inline u32 BlurRedTotal(...)`.
 *
 * The helper accepted the red BlurComponentTable by reference, the thirteen-
 * element spilled sample array, and samples 13/14/15 as scalar arguments.  Its
 * statement order was byte-for-byte the retained red accumulation order, and
 * the loop body became:
 *
 *     u32 redSum = BlurRedTotal(
 *         components[IDX(PALETTE_CHANNEL_RED)],
 *         samples,
 *         sample13,
 *         sample14,
 *         sample15
 *     );
 *
 * Result (2026-07-26):
 *   clean fuzzy: 90.308014%, size 1708
 *   first forest state: 86.641350%, size 1649
 *   ordered relocs: 43/43 in the clean state
 *
 * Artifact:
 *   build/tu-state-noise/doblur-inline-red-sum-clean
 *
 * Disposition: rejected.  Although the helper inlines in the clean state, its
 * argument/lifetime boundary destroys the better allocation orbit opened by
 * the small component accessor.  The large per-channel inline is not retail's
 * shape.
 */
