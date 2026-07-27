/*
 * FlipMonoIconToBitmap whole-clip inline-boundary matrix, 2026-07-27.
 *
 * MonoIconToBitmap owns its complete initial clipping decision in an inline
 * MonoNeedsClipping helper.  Previous FlipMono experiments tested zero-arg Y
 * accessors and a vertical-only predicate helper, but not this whole sibling
 * boundary.
 *
 * Two reviewed forms were crossed with clean plus 50 forest states:
 *
 *     static inline i32 FlipMonoNeedsClipping(
 *         IconEntry* entry, i32 x0, i32 currentY,
 *         i32 clipX, i32 clipY, i32 clipW, i32 clipH
 *     ) {
 *         return x0 < clipX || clipX + clipW < x0 + entry->w
 *                || currentY < clipY
 *                || entry->h + currentY > clipY + clipH;
 *     }
 *
 * The second form accepted the caller's already-materialized `right` edge
 * instead of recomputing `x0 + entry->w`.  Both were called from the existing
 * clip branch with `gFMY` as currentY.
 *
 * Complete product: 102/102 compiled, source restored, no wall-time
 * truncation.  Helper-owned right edge was clean 75.802400%, size 521.
 * Caller-passed right edge was clean 76.191620%, size 509.  The latter
 * entered a distinct equal-MAX island at forest trials 33 and 42:
 * 80.419160%, size 513, 26/27 relocations, exact 26-block flow with 21 exact
 * and five size-only blocks.  It did not recover retail size 530 or retail's
 * second initial-clip gFMY relocation.
 *
 * Artifacts:
 *   build/iconmf2b-whole-clip-inline-axes-20260727.json
 *   build/iconmf2b-whole-clip-inline-manifest-20260727.json
 *   build/match-variants/iconmf2b-whole-clip-inline-20260727
 *
 * Disposition: preserve the caller-passed helper as a credible equal-MAX
 * structural seed.  Its lower clean score and incomplete relocation evidence
 * do not justify changing live source; generated states are not retained.
 */
