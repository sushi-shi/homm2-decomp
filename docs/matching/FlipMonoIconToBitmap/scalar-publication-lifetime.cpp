/*
 * FlipMonoIconToBitmap scalar/publication-lifetime matrix, 2026-07-27.
 *
 * Fresh retail evidence at RVA 0xda800 showed a setup ordering not expressed
 * by the earlier current/direct/FlipDim/FlipIcon setup families.  Retail
 * snapshots entry->x before srcOffset, owns srcOffset across publication of
 * gFMEntry, subtracts entry X before publishing gFMSrc, and forms the
 * exclusive right edge before publishing gFMX0.
 *
 * Three reviewed forms expressed that lifetime:
 *
 *     i32 entryX = entry->x;
 *     i32 sourceOffset = entry->srcOffset;
 *     gFMEntry = entry;
 *     u8* srcData = reinterpret_cast<u8*>(entries) + sourceOffset;
 *     i32 x0 = x - entryX;
 *     gFMSrc = srcData;
 *     i32 w = entry->w;
 *     x0 = x0 - w;
 *     i32 entryY = entry->y;
 *     i32 right = w + x0 + 1;
 *     x0++;
 *     gFMX0 = x0;
 *     i32 X = right - 1;
 *     gFMXEnd = X;
 *     gFMY = y + entryY;
 *
 * The other two arms removed the sourceOffset temporary or published the
 * computed source pointer directly before the scalar tail.  Every arm was
 * crossed with the clean state plus 50 forest states: 153/153 compiled,
 * source was restored, and the matrix was not wall-time truncated.
 *
 * Clean results were 77.473050%, 77.502990%, and 77.502990%, all size 517
 * with 26/27 relocations and exact 26-block flow.  All three forms converged
 * to the known trial-8 orbit at 80.275450%, size 515, 26/27 relocations,
 * with 22 exact and four size-only blocks.  They did not reproduce the
 * retained 80.419160% MAX, retail size 530, or the missing gFMY relocation.
 *
 * Artifacts:
 *   build/iconmf2b-scalar-publication-lifetime-axes-20260727.json
 *   build/iconmf2b-scalar-publication-lifetime-manifest-20260727.json
 *   build/match-variants/iconmf2b-scalar-publication-lifetime-20260727
 *
 * Disposition: retain this as a credible but convergent structural family.
 * No generated state or source change is retained.
 */
