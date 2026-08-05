/*
BlitBitmapToScreen (RVA 0x000c5ee0, retail size 395)
Gold/Buka 2.1 cursor-overlap structure and exact island, 2026-08-05.

The prior PoL reconstruction introduced `blitSourceX`, expressed the main
condition as a positive overlap test, cached the saved cursor coordinates, and
returned from that arm.  It emitted 392 bytes at 98.214810%, with the exact
18-block graph but only 23/24 target relocations.

The exact Gold/Buka source instead mutates `sourceX`, owns the ordinary blit in
the negative no-overlap branch, and reloads the mouse-manager fields directly
for the cursor repair blit.  A complete 2 x 51 structure/state matrix finished
102/102.  The donor clean object improved to 99.481480%, retail size 395, the
exact CFG, and ordered 24/24 relocations.

A focused replay of forest/top trial 3 (seed 1213156658) proved 100.000000%,
retail size 395, exact 18/18 CFG and ordered 24/24 relocations for retained
source hash 0ab236af3f96.  The hash-scoped MAX was recorded as 100.  The
declaration-forest probe is not retained.

Artifacts:
  build/blitbitmaptoscreen-cross-version-axes.json
  build/blitbitmaptoscreen-cross-version-manifest.json
  build/blitbitmaptoscreen-exact-replay-summary.json
  build/tu-state-noise/blitbitmaptoscreen-exact-replay/

Disposition: retain the donor cursor-overlap structure and the audited exact
MAX; retain no generated state declarations.
*/

#if 0
if (gpMouseManager->IsVis() == 0 || gBlitRight < gpMouseManager->m_savedLeft
    || destinationX > gpMouseManager->m_cursorRight
    || gBlitBottom < gpMouseManager->m_savedTop
    || destinationY > gpMouseManager->m_cursorBottom) {
    // Ordinary blit.
} else {
    // Save/draw mouse, blit, repair exposed cursor rectangle, then restore.
}
#endif
