/*
 * FlipMonoIconToBitmap persistent local decoder-cursor matrix, 2026-07-27.
 *
 * FlipIconToBitmap owns its decoder cursor as a loop-local pointer.  Retail
 * FlipMono's decode block increments a pointer, publishes gFMSrc, then reads
 * the byte at src[-1].  Earlier FlipMono matrices changed direct/accessor
 * reads through gFMSrc, but did not test a persistent local cursor whose
 * publication lifetime spans the loop.
 *
 * Three reviewed forms were tested:
 *
 *     i32 cmd = ReadIconRleByte(src);
 *     gFMSrc = src;
 *
 *     i32 cmd = *src++;
 *     gFMSrc = src;
 *
 *     src++;
 *     gFMSrc = src;
 *     i32 cmd = src[-1];
 *
 * Each used `u8* src` from setup through the decoder loop.  The complete
 * product of three forms and clean plus 50 forest states compiled 153/153;
 * source was restored and the matrix was not wall-time truncated.
 *
 * The clean forms reached 71.850296%, 73.556885%, and 72.568860%, size 521,
 * with 25/27 relocations and exact 26-block flow.  The strongest island was
 * local post-increment plus publication at trial 20: 79.000000%, size 520,
 * 25/27 relocations, 21 exact and five size-only blocks.  A distinct trial-15
 * island reached size 526 but only 75.221560% and still 25/27 relocations.
 * Thus local cursor ownership changes the orbit and approaches retail size,
 * but contradicts retail relocation ownership by losing a second site.
 *
 * Artifacts:
 *   build/iconmf2b-local-cursor-publication-axes-20260727.json
 *   build/iconmf2b-local-cursor-publication-manifest-20260727.json
 *   build/match-variants/iconmf2b-local-cursor-publication-20260727
 *
 * Disposition: preserve the lower-score size-526 island as descendant clue
 * evidence, but retain no live-source or MAX change.
 */
