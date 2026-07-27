/*
 * FlipMonoIconToBitmap reviewed structural and expression matrices.
 *
 * Canonical evidence:
 *   RVA 0xda800, retail size 530, optimized Iconmf2b TU.
 *   Live source: 77.502990%, size 517, 26/27 ordered relocations.
 *   CFG: 26/26 flow-exact, 20 exact-size and six size-only blocks.
 *
 * The unchanged-source forest trial 8 re-proved the retained historical MAX:
 *   build/tu-state-noise/iconmf2b-canonical-trial8-record-max-20260727
 *   source hash 5cdb92eca523; 80.275450%; size 515; 26/27 relocations.
 *   Its CFG remains 26/26 flow-exact and improves to 22 exact-size plus four
 *   size-only blocks. The four size-only blocks are B1, B2, B4, and B7, all
 *   in the outer clip predicate and clip-bound materialization. The generated
 *   MAX update correctly reported not_higher because config already retains
 *   this observation.
 *   The ordered relocation identities/addends otherwise agree; retail's extra
 *   relocation is the second gFMY load in B4 (retail function offset 0x86).
 *   The candidate keeps the earlier gFMY value live in a register instead.
 *
 * 1. Setup arithmetic, clip comparisons, and fill-address ownership:
 *    build/match-variants/iconmf2b-setup-clip-fill-20260727
 *    Complete product: 2 * 2 * 2 * (clean + 50 forest states) = 408.
 *
 *    Setup arms:
 *      i32 x0 = x;
 *      x0 = x0 - entryX;
 *
 *      i32 x0 = x - entryX;
 *
 *    Clip predicate arms:
 *      x0 < clipX || clipX + clipW < right || gFMY < clipY
 *          || entry->h + gFMY > clipY + clipH
 *
 *      clipX > x0 || clipW + clipX < right || clipY > gFMY
 *          || clipY + clipH < entry->h + gFMY
 *
 *    Both unclipped and clipped full-fill sites were crossed as:
 *      (gFMRow - cmd) + 1 + X
 *      gFMRow + (X - cmd) + 1
 *
 *    All eight authored arms converged byte-for-byte within each TU state.
 *    Trials 8, 14, and 20 reached 80.275450%, size 515, 26/27 relocs.
 *    None of these internal spellings changes the compiler orbit.
 *
 * 2. Materialized source-pointer structural descendant:
 *    build/tu-state-noise/iconmf2b-materialized-src-20260727
 *    Complete clean + 50-state census.
 *
 *      u8* src = srcIcon->m_data;
 *      IconEntry* entry = reinterpret_cast<IconEntry*>(src) + frame;
 *      ...
 *      src += srcOffset;
 *      i32 x0 = x - entryX;
 *      gFMSrc = src;
 *
 *    This sibling-family shape is a distinct lower orbit: clean 76.862274%;
 *    best 79.736530% at trial 8, size 514, 26/27 relocs, 18 normalized states.
 *    It was rejected and the canonical source restored.
 *
 * 3. Clip-bound materialization descendants of the trial-8 block evidence:
 *    build/match-variants/iconmf2b-clip-bound-materialization-20260727
 *    Complete product: 3 * (clean + 50 forest states) = 153.
 *
 *    Direct bounds:
 *      gFMClipR = clipX + clipW - 1;
 *      gFMClipB = clipY + clipH - 1;
 *
 *    Branch-local inclusive bounds:
 *      i32 clipRight = clipX + clipW - 1;
 *      i32 clipBottom = clipY + clipH - 1;
 *      gFMClipR = clipRight;
 *      gFMClipB = clipBottom;
 *
 *    Precomputed exclusive extents:
 *      i32 clipRight = clipX + clipW;
 *      i32 clipBottom = clipY + clipH;
 *      // Predicate uses clipRight/clipBottom.
 *      gFMClipR = clipRight - 1;
 *      gFMClipB = clipBottom - 1;
 *
 *    Branch-local inclusive bounds were byte-neutral to direct bounds in
 *    every state. Precomputed exclusive extents entered a lower orbit.
 *    The retained MAX, size, and missing relocation were unchanged.
 *
 * Disposition: no source arm is retained. The loop CFG and fill structure are
 * supported by retail; the remaining residual is optimized register/scheduling
 * ownership plus one missing relocation, not evidence for a control-flow rewrite.
 */

void FlipMonoIconToBitmap_setup_clip_fill_attempts() {
}
