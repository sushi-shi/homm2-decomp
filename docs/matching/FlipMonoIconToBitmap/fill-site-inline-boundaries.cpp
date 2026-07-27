/*
 * FlipMonoIconToBitmap fill-site inline-boundary campaign, 2026-07-27.
 *
 * Earlier inline matrices targeted only the initial four-clause clipping
 * decision.  This pass tested the independent fill-site mechanism proven in
 * MonoIconToBitmap and the color/flip icon siblings.
 *
 * 1. Fill-site row visibility:
 *
 *      static inline i32 FlipMonoRowVisible(i32 clipTop) {
 *          return clipTop <= gFMY && gFMY <= gFMClipB;
 *      }
 *
 *      if (FlipMonoRowVisible(clipY)
 *          && (left = (X - cmd) + 1, clipX <= left)
 *          && gFMClipR >= X) {
 *
 *    Clean plus 50 forest states completed 51/51.  Trial 44 reached
 *    80.419160%, size 513, 26/27 relocations, and exact 26-block flow with
 *    21 exact plus five size-only blocks.  This reproduces, but does not
 *    exceed, the retained current-source MAX.
 *
 * 2. Whole-fill inline owner:
 *
 *      static inline void FlipMonoFillRun(
 *          i32 clip, i32 clipX, i32 clipY,
 *          i32 X, i32 cmd, i32 color
 *      ) {
 *          // complete unclipped/clipped fill body
 *      }
 *
 *      FlipMonoFillRun(clip, clipX, clipY, X, cmd, color);
 *
 *    Clean plus 50 states completed 51/51.  Trial 47 reached 79.580840%,
 *    size 513, 26/27 relocations, and the same 21-exact/five-size-only
 *    block partition.
 *
 * 3. Whole-fill owner calling FlipMonoRowVisible:
 *
 *    Clean plus 50 states completed 51/51.  Trial 30 reached 79.580840%.
 *    Its text and normalized relocation hashes are identical to the
 *    whole-fill-only trial-47 object, so the nested row boundary is folded
 *    once the whole fill owns the operation.
 *
 * Every arm retained all 26 candidate relocations and the retail 26-block
 * successor graph.  None emitted retail's missing second initial-clip gFMY
 * relocation, reached retail's 530-byte size, or produced exact closure.
 *
 * Artifacts:
 *   build/iconmf2b-fill-row-visible-manifest-20260727.json
 *   build/match-variants/iconmf2b-fill-row-visible-20260727/results.json
 *   build/iconmf2b-whole-fill-inline-manifest-20260727.json
 *   build/match-variants/iconmf2b-whole-fill-inline-20260727/results.json
 *   build/iconmf2b-whole-fill-row-visible-manifest-20260727.json
 *   build/match-variants/iconmf2b-whole-fill-row-visible-20260727/results.json
 *
 * Disposition: preserve all three source families as bounded evidence.  The
 * row helper is credible family structure but has a lower clean score and no
 * MAX gain; both whole-fill forms are lower islands.  Reconstructed source is
 * restored unchanged and no generated compiler-state input is retained.
 */

void FlipMonoIconToBitmap_fill_site_inline_boundaries_record() {
}
