/*
 * MonoIconToBitmap separate clipped/unclipped inline owners, 2026-07-27.
 *
 * The prior whole-fill matrix placed both branches under one helper.  This
 * matrix tested finer, non-overlapping ownership at the first fill-body
 * divergence while preserving the established 31-block CFG:
 *
 *   - a helper owning only the clipped path;
 *   - a helper owning only the unclipped memset;
 *   - separate inline helpers owning both paths.
 *
 * The clipped helper retained the complete row-visibility and four-way fill
 * logic.  The unclipped helper was:
 *
 *     static inline void MonoFillUnclipped(
 *         u8* row, i32 cmd, i32 color
 *     ) {
 *         memset(row + gMonoX, color, cmd);
 *     }
 *
 * Complete product: three forms times clean plus 50 forest states, 153/153
 * compiled without truncation; source was restored.
 *
 * Clean scores were 77.359116% clipped-only, 78.160220% unclipped-only, and
 * 80.756905% separate helpers, all size 604 with 36/38 relocations and exact
 * 31-block flow.  The strongest descendant was the unclipped-only helper at
 * trial 50: 81.005520%, size 602, 36/38 relocations, 24 exact plus seven
 * size-only blocks.  Separate helpers reproduced the older 80.784530% orbit.
 *
 * Artifacts:
 *   build/iconm2b-split-fill-inline-axes-20260727.json
 *   build/iconm2b-split-fill-inline-manifest-20260727.json
 *   build/match-variants/iconm2b-split-fill-inline-20260727
 *
 * Disposition: both path-specific boundaries are credible lower structural
 * islands, but neither reaches retail size, restores either missing ordered
 * relocation, or exceeds the retained 83.607735% MAX.  Retain no source.
 */
