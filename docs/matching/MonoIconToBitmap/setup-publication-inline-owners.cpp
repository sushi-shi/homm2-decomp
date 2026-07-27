/*
 * MonoIconToBitmap setup-publication inline boundaries, 2026-07-27.
 *
 * Retail's first unresolved divergence is ownership of two second reads:
 * gMonoX0 in the right-edge clause and gMonoY in the bottom-edge clause.
 * Earlier clip helpers and coordinate accessors did not emit them.  This pass
 * tested a different boundary before clipping: publish coordinates through
 * inline setup helpers, then enter the retained MonoNeedsClipping helper.
 *
 * Three reviewed forms were crossed:
 *
 *     MonoPublishCoordinates(entry, x, y);
 *
 *     MonoPublishX(entry, x);
 *     MonoPublishY(entry, y);
 *
 *     MonoPublishSetup(data, entry, x, y);
 *
 * The first helper owned gMonoX0/gMonoX/gMonoY, the split form separated X
 * and Y publication, and the whole form additionally owned gMonoEntry and
 * gMonoSrc.  Each form received the clean state plus 50 forest states:
 * 153/153 compiled, source restored, no wall-time truncation.
 *
 * All three clean forms were 78.160220%, size 604, 36/38 relocations, with
 * exact 31-block flow.  Whole setup at trial 37 and combined coordinates at
 * trial 50 reached 81.005520%, size 602, 36/38 relocations, 24 exact plus
 * seven size-only blocks.  Their strongest object is identical to the
 * split-fill trial-50 island, showing a shared TU state rather than a new
 * coordinate-publication orbit.  Neither missing relocation was emitted.
 *
 * Artifacts:
 *   build/iconm2b-setup-publication-inline-axes-20260727.json
 *   build/iconm2b-setup-publication-inline-manifest-20260727.json
 *   build/match-variants/iconm2b-setup-publication-inline-20260727
 *
 * Disposition: inline setup publication is a tested lower family, not a
 * source or MAX improvement.  No generated state or helper is retained.
 */
