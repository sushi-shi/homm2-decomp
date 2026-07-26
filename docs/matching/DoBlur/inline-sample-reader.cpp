/*
 * DoBlur inline-boundary experiment: extract the repeated byte-read and
 * component-index shift used for all sixteen kernel samples.
 *
 * Reviewed helper:
 *
 *     static inline i32 BlurSampleAt(u8* input, i32 offset) {
 *         return static_cast<u32>(input[offset]) << COMPONENT_INDEX_SHIFT;
 *     }
 *
 * Every direct `static_cast<u32>(input[offset]) << 2` in the kernel was
 * replaced by `BlurSampleAt(input, offset)`.
 *
 * Result (2026-07-26):
 *   clean and first forest state: 90.824900%, size 1708
 *   ordered relocations: 43/43
 *
 * Artifact:
 *   build/tu-state-noise/doblur-inline-sample-access-clean
 *
 * Disposition: rejected.  The helper inlines, but reaches exactly state
 * f8114b72b01d4404, already present as trial 26 in the retained component-
 * accessor sweep.  It adds no new orbit and loses 2.58649 points versus that
 * structure's best island.
 */
