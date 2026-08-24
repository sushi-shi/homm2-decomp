/*
advManager::ScreenScroll comparisons and locals
===============================================

PoL 2.0 RVA 0x68ab6; Buka 2.1 RVA 0x12697.

Comparison product: 8/8
-----------------------
  build/screen-scroll-comparison-manifest.json
  build/source-variant-batch/screen-scroll-comparisons/results.json

Independent axes cover the two upper clamps and paired final-origin test.
All eight arms emit identical bytes, CFG, and 21/21 relocations in the
measured shared-header state. PoL adopts Buka's three comparison groups.

Local-family census: 102/102
----------------------------
  build/screen-scroll-structural-state-manifest.json
  build/source-variant-batch/screen-scroll-structural-state/results.json

Clean plus the same 50 forest/top states keep PoL's `originX/originY` parent
ahead of Buka's `xOrigin/yOrigin` parent in every state: 99.822136% versus
99.719370%, both 422 bytes and 21/21 relocations. Retain the PoL names as the
VC4.2 slot family; this is not gameplay logic.

Final retained source: clean exact, 422 bytes, exact 29/29 CFG, complete
ordered 21/21 relocations. No generated state is retained.
*/
