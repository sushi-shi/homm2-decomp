/*
 * FlipIconToBitmap dim-loop read/increment ownership matrix, 2026-07-27.
 *
 * In both retail dim loops the instruction order is:
 *
 *     load gFlipDimPal
 *     read byte at dp
 *     increment dp
 *     decrement count
 *     publish gFlipDimDst
 *     translate and store the byte
 *
 * The reconstructed source used `i32 px = *dp++;`.  A new explicit spelling
 * split the read and increment:
 *
 *     u8* dimPalette = gFlipDimPal;
 *     i32 px = *dp;
 *     dp++;
 *     count--;
 *     gFlipDimDst = dp;
 *     dp[-1] = dimPalette[px];
 *
 * Unclipped-only, clipped-only, and both-loop forms were each crossed with
 * clean plus 50 forest states.  All 153/153 cells compiled, source was
 * restored, and the matrix was not truncated.
 *
 * The three forms were byte-identical within every one of the 51 states.
 * Clean remained 86.846150%, size 1251, 79/81 relocations.  Forest trial 25
 * reproduced the retained 88.026530% MAX for every form, with the same size,
 * relocation stream, and 75-block graph.  Thus MSVC recognizes these source
 * spellings as the same loop ownership; the apparent scheduling difference
 * is controlled by TU state rather than the explicit `dp` increment.
 *
 * Artifacts:
 *   build/iconf2b-dim-read-increment-axes-20260727.json
 *   build/iconf2b-dim-read-increment-manifest-20260727.json
 *   build/match-variants/iconf2b-dim-read-increment-20260727
 *
 * Disposition: record the source-equivalence result and retain no source or
 * generated-state change.  Exact closure remains open at size 1265 and
 * complete ordered relocation identity.
 */
