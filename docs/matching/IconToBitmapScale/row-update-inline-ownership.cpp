/*
 * IconToBitmapScale, RVA 0xd2f90: row-accumulator update ownership.
 *
 * Retail's outer-loop tail uses ten instructions and reloads/stores both
 * carried row pointers; the retained source emits six instructions.  This
 * new matrix compared direct updates, two return-value inline updates, and
 * one shared by-reference inline tail:
 *
 *     static inline void AdvanceIconScaleRows(
 *         u8*& sourceRow, u8*& destinationRow,
 *         i32 sourceAdvance, i32 destinationAdvance
 *     ) {
 *         sourceRow += sourceAdvance;
 *         destinationRow += destinationAdvance;
 *     }
 *
 * All three structures received clean plus 50 forest states: 153/153
 * complete compilations.
 *
 * Results:
 *   direct updates:       85.829790%, size 365, 18/18 blocks
 *   return-value updates: 85.829790%, size 365, same text SHA
 *   shared reference:     77.808510%, size 378, 17 blocks
 *   ordered relocations:  4/4 for every arm
 *
 * The shared-reference form is a genuine near-retail-size lower island (only
 * one byte larger than retail), but its visible ownership boundary damages
 * flow: five exact blocks, two size-only, two target shifts, eight flow-kind
 * mismatches, and one missing block.  Return-value helpers canonicalize to
 * the retained direct object.
 *
 * Artifacts:
 *   build/icon2bs-row-update-inline-axes.json
 *   build/icon2bs-row-update-inline-manifest.json
 *   build/match-variants/icon2bs-row-update-inline
 *
 * Disposition: preserve the 378-byte result set as a lower-island clue, but
 * reject both helper shapes from source. MAX remains 85.829790%.
 */

void IconToBitmapScale_row_update_inline_ownership_attempt() {
}
