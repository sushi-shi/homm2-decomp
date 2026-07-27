/*
 * MonoIconToBitmap persistent local decoder-cursor matrix, 2026-07-27.
 *
 * Retail Mono and FlipMono both decode with an increment, cursor
 * publication, and a read from cursor[-1].  The FlipMono campaign proved
 * that a persistent loop-local cursor creates a distinct compiler orbit, so
 * the same untested ownership family was transferred here.
 *
 * Three reviewed forms used `u8* src` from setup through the decoder:
 *
 *     i32 cmd = ReadIconRleByte(src);
 *     gMonoSrc = src;
 *
 *     i32 cmd = *src++;
 *     gMonoSrc = src;
 *
 *     src++;
 *     gMonoSrc = src;
 *     i32 cmd = src[-1];
 *
 * Every form was crossed with clean plus 50 forest states.  All 153/153
 * cells compiled, source was restored, and the matrix was not truncated.
 *
 * Clean results were 78.303870%, 78.751380%, and 78.966850%, all size 619
 * with 36/38 relocations and exact 31-block flow.  The increment/publish/
 * index form reached the strongest island at trial 2: 79.961330%, size 617,
 * 36/38 relocations, 23 exact plus eight size-only blocks.  Unlike FlipMono,
 * persistent local ownership did not remove another relocation, but it also
 * did not recover retail's two coordinate reloads.
 *
 * Artifacts:
 *   build/iconm2b-local-cursor-publication-axes-20260727.json
 *   build/iconm2b-local-cursor-publication-manifest-20260727.json
 *   build/match-variants/iconm2b-local-cursor-publication-20260727
 *
 * Disposition: preserve this close-size lower island as structural evidence.
 * It remains three bytes larger than retail size 614 and below retained MAX,
 * so no live-source or generated-state change is retained.
 */
