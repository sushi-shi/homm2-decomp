/*
 * IconToBitmapColorTable, RVA 0xd32a0:
 * saved-destination lifetime crossed with sibling dim-value ownership.
 *
 * The retained source reads gCTDst early and uses an inline helper that owns
 * the complete conditional dim/store/pointer-advance operation.  Two missing
 * structural crosses were tested independently in clean state and 50
 * top-of-TU declaration-forest states:
 *
 *   // Retail-shaped late gCTDst read with the retained dim-pixel helper.
 *   u8* savedDst;
 *   // ... clipping and row setup ...
 *   u8* row = dest->m_pixels + gCTPitch * gCTY;
 *   savedDst = gCTDst;
 *
 *   // FlipIconToBitmapColorTable-style mapped-value helper, crossed with
 *   // both the late lifetime above and the retained early lifetime.
 *   static inline u8 ColorTableDimValue(u8* dst, u8* palette) {
 *       return palette[*dst];
 *   }
 *   if (dimGate != 0)
 *       *savedDst = ColorTableDimValue(savedDst, palette);
 *   savedDst++;
 *
 * The late lifetime made setup blocks B0 and B8 retail-sized and increased
 * the exact-size partition from 67 to 69 blocks.  It remained a lower island:
 * best 82.798120%, size 1428, 89/91 relocations, exact 86-block flow.
 *
 * The mapped-value helper was byte-neutral in each clean lifetime but changed
 * the state census.  Its best late-lifetime result was 82.669014%; its best
 * early-lifetime result was 83.737090%.  Neither exceeded the retained MAX.
 *
 * Artifacts:
 *   build/tu-state-noise/icon2bc-late-saved-dst-dim-owner/
 *   build/icon2bc-late-saved-dst-dim-owner-states.json
 *   build/tu-state-noise/icon2bc-late-saved-dst-dim-value/
 *   build/icon2bc-late-saved-dst-dim-value-states.json
 *   build/tu-state-noise/icon2bc-early-saved-dst-dim-value/
 *   build/icon2bc-early-saved-dst-dim-value-states.json
 *
 * Disposition: retain the early destination lifetime and complete dim-pixel
 * inline owner.  Preserve the late lifetime as a credible lower structural
 * island because its setup partition is closer to retail.
 */

void IconToBitmapColorTable_saved_destination_dim_descendants_attempt() {
}
