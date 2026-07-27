/*
 * FlipMonoIconToBitmap family setup, decoder, and inline-Y matrices.
 *
 * Retail evidence before this experiment:
 *   RVA 0xda800, size 530, optimized Iconmf2b TU.
 *   Live source: 77.502990%, size 517, relocs 26/27.
 *   Blocks: 26/26 flow-exact, 20 exact and six size-only.
 *   Historical unchanged-source MAX: 80.275450%, size 515.
 *
 * 1. Structural family matrix, clean states first:
 *   build/match-variants/iconmf2b-family-loop-structure-clean-20260727
 *
 * Four complete setup families were crossed with four decoder/run-publication
 * families (16/16 compiled):
 *
 *   setup:
 *     - current field snapshots and exclusive `right`;
 *     - direct entry-field ownership;
 *     - coherent FlipDimIconToBitmap setup order;
 *     - coherent FlipIconToBitmap setup order.
 *
 *   decoder/run ownership:
 *     - ReadIconRleByte with duplicated gFMRun publication;
 *     - direct *gFMSrc++ with duplicated publication;
 *     - accessor read with one common publication;
 *     - direct read with one common publication.
 *
 * Direct-field and FlipIcon setup lost relocation structure (25/27 and 24/27)
 * and were pruned. Current and FlipDim setup retained exact 26-block flow and
 * 26/27 relocations, so every new decoder descendant received a complete
 * clean-plus-50 forest census.
 *
 * 2. Current setup decoder/publication censuses:
 *   build/match-variants/iconmf2b-direct-decoder-states-20260727
 *   build/match-variants/iconmf2b-common-run-states-20260727
 *
 *   direct read + duplicated publication:
 *     51/51 complete; best trial 8, 79.556885%, size 515.
 *   accessor read + common publication:
 *     51/51 complete; best trial 8, 79.077840%, size 515.
 *   direct read + common publication:
 *     51/51 complete; best trial 8, 78.359280%, size 515.
 *
 * All retained 26/27 relocations. Moving gFMRun to a single source store did
 * not explain retail's scheduled store between `test dl, dl` and its branch.
 *
 * 3. Coherent FlipDim setup x all decoder/publication owners:
 *   build/match-variants/iconmf2b-flipdim-family-states-20260727
 *
 * Complete product: 4 * (clean + 50 states) = 204/204.
 * The accessor/duplicated arm entered a new higher island at trials 20 and 30:
 *   80.419160%, size 513, relocs 26/27;
 *   blocks 26/26 flow-exact, 21 exact and five size-only.
 * The direct/duplicated, accessor/common, and direct/common descendants peaked
 * at 79.700600%, 79.221560%, and 78.502990%, respectively.
 *
 * The FlipDim setup arm is retained in live source because it is a credible
 * same-family structure and raises MAX despite its lower clean score. The
 * baseline was generated, never hand-edited:
 *   clean hash initialization: `homm2 status update`
 *   deterministic replay:
 *     build/tu-state-noise/iconmf2b-flipdim-trial20-record-max-20260727
 *   retained MAX: 80.419160%, source hash 25c32ca3a4fa.
 *
 * 4. Inline Y-ownership descendants:
 *   build/match-variants/iconmf2b-flipdim-inline-y-states-20260727
 *
 * Two legitimate family mechanisms were each compiled against all 51 states:
 * a zero-argument inline Y accessor called by both vertical clauses, and one
 * inline vertical-outside predicate like the icon-family row helpers.
 * The complete 102/102 matrix did not emit retail's missing second initial
 * gFMY relocation. Best was the flat accessor at trial 45: 79.640720%, size
 * 513, relocs 26/27. The helper therefore remains clue evidence only.
 *
 * Disposition: retain the FlipDim setup family and its script-recorded MAX.
 * Retain none of the generated states, alternate decoders, common-publication
 * forms, or inline helpers. Exact closure remains open at 26/27 relocations;
 * the missing site is retail's second gFMY load in initial clip block B4.
 */

#if 0
// Retained coherent FlipDim setup order.
IconEntry* entries = srcIcon->Entries();
IconEntry* entry = &entries[frame];
u8* srcData = reinterpret_cast<u8*>(entries) + entry->srcOffset;
i32 x0 = x;
gFMEntry = entry;
gFMSrc = srcData;
i32 w = entry->w;
i32 entryY = entry->y;
x0 = x0 - entry->x;
x0 = x0 - w;
x0++;
gFMX0 = x0;
gFMY = y + entryY;
i32 X = w + x0 - 1;
gFMXEnd = X;
i32 right = w + x0;

// Direct decoder with common run publication.
gFMX = X;
i32 cmd = *gFMSrc++;
gFMRun = cmd;
if (static_cast<i8>(cmd) < 0) {
}

// Inline accessor descendant.
static inline i32 FlipMonoCurrentY() {
    return gFMY;
}
if (x0 < clipX || clipX + clipW < right || FlipMonoCurrentY() < clipY
    || entry->h + FlipMonoCurrentY() > clipY + clipH) {
}
#endif
