/*
 * FlipMonoIconToBitmap clip-predicate ownership matrices.
 *
 * Motivation: the retained-MAX object has 26/26 flow-exact blocks but lacks
 * retail's second gFMY load in B4. Retail reloads gFMY separately for the
 * bottom-edge test; the candidate keeps its first load live. These matrices
 * tested whether natural short-circuit structure controls that ownership.
 *
 * 1. Every selected ordering family for the four independent conditions:
 *
 *     x0 < clipX
 *     clipX + clipW < right
 *     gFMY < clipY
 *     entry->h + gFMY > clipY + clipH
 *
 * Eight reviewed orders were crossed with the clean state plus 50 forest
 * states: 8 * 51 = 408/408 compiled. Only the current
 * left/right/top/bottom order re-produced the historical 80.275450% island
 * (size 515, 26/27 relocs). The strongest alternative order reached
 * 78.395210%; none emitted the missing relocation.
 *
 * 2. Three different high-level ownership forms compiled across all 51
 * states:
 *
 *     // nested horizontal/vertical branches with duplicated bounds
 *     // select clip first, then publish bounds in a second if
 *     // the retained flat four-term short circuit
 *
 * The flat form again reached 80.275450%. Select-then-publish reached
 * 78.952095%, size 524; nested duplicated bounds reached 77.245510%,
 * size 537. Neither recovered retail size or relocation order.
 *
 * 3. Staged predicate state was then tested with the VC4.2-compatible form:
 *
 *     i32 outside = x0 < clipX || clipX + clipW < right;
 *     if (!outside) {
 *         outside = gFMY < clipY
 *                   || entry->h + gFMY > clipY + clipH;
 *     }
 *
 * Horizontal-first and vertical-first each compiled against all 51 states:
 * 2 * 51 = 102/102. Their best results were 71.377240%/size 547 and
 * 70.904190%/size 551, both with 26/27 relocs. An initial diagnostic spelling
 * used `bool`, which this MSVC 4.2 profile rejects; those failed cells are not
 * counted as completed source variants and were replaced by the i32 matrix.
 *
 * Artifacts:
 *   build/iconmf2b-clip-predicate-order-axes-20260727.json
 *   build/iconmf2b-clip-predicate-order-manifest-20260727.json
 *   build/match-variants/iconmf2b-clip-predicate-order-20260727
 *   build/iconmf2b-clip-predicate-structure-axes-20260727.json
 *   build/iconmf2b-clip-predicate-structure-manifest-20260727.json
 *   build/match-variants/iconmf2b-clip-predicate-structure-20260727
 *   build/iconmf2b-staged-predicate-i32-axes-20260727.json
 *   build/iconmf2b-staged-predicate-i32-manifest-20260727.json
 *   build/match-variants/iconmf2b-staged-predicate-i32-20260727
 *
 * Disposition: no source/MAX update. The missing gFMY reload is not controlled
 * by term order or by splitting the same predicate into horizontal/vertical
 * state. The retained flat order remains the active family seed.
 */
