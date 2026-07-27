/*
 * IconToBitmap, RVA 0xd0570: same-family row-visibility inline owner.
 *
 * The exact IconToBitmapYModify sibling uses a narrow row predicate helper.
 * IconToBitmap has the same vertical test in its solid, dim, and literal
 * clipping regions, so each site was tested independently and together:
 */

#if 0
static inline i32 IconRowVisible(
    i32 currentY, i32 clipBottom, i32 clipTop
) {
    return clipTop <= currentY && currentY <= clipBottom;
}

if (IconRowVisible(gIcY, gIcClipB, clipY)
    && (right = X + count, clipX < right) && gIcClipR >= X) {
    // Solid run clipping.
}
#endif

/*
 * Clean screen:
 *
 *   build/icon2b-row-visibility-clean-axes-20260727.json
 *   build/match-variants/icon2b-row-visibility-clean-20260727/
 *
 * A helper at any single site, all three value sites, or all three reference
 * sites canonicalizes to 80.945050%, size 1219, and 78/83 relocations.  The
 * open source is 79.318680%/1220 with 79/83.  Single-site placement is thus a
 * distinct same-family compiler island even though the inlined expression is
 * otherwise canonical.
 *
 * Complete product: six structures * (clean + 50 top forest states) = 306:
 *
 *   build/icon2b-row-visibility-states-v2-manifest-20260727.json
 *   build/match-variants/icon2b-row-visibility-states-v2-20260727/
 *
 * Each independent single-site helper reaches a new 81.631870% MAX at trial
 * 40, seed 0x49525631.  The candidate is size 1220 with 79/83 relocations and
 * exact 80-block flow: 63 exact plus 17 size-only blocks.  Combining all three
 * sites is lower at 81.508240%, showing that one narrow owner is the stronger
 * source family.
 *
 * The solid-fill site is retained because it is the first repeated predicate,
 * mirrors the exact sibling mechanism, and leaves the later dim/literal
 * regions open for independent reconstruction.  A standalone unchanged-source
 * census reproduced trial 40 and recorded MAX 81.6319 for hash
 * 423eab910174.02e5f0cc8453:
 *
 *   build/tu-state-noise/icon2b-row-visible-sibling-name-20260727/
 *   build/icon2b-row-visible-sibling-name-state-summary-20260727.json
 *
 * No generated probe declaration is retained.  Exact closure remains open;
 * one ordered occurrence each of gIcCnt2, gIcPitch, gIcX0, and gIcY is still
 * absent.
 */

void IconToBitmap_row_visibility_family_inline_attempt() {
}
