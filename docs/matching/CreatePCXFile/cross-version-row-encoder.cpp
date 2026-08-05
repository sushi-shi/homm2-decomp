/*
CreatePCXFile (RVA 0x000c66d0, retail size 494)
Gold/Buka 2.1 row-encoder reconstruction, 2026-08-05.

The prior PoL loop advanced the input pointer after each row and used a compact
literal-first branch.  It scored 98.277020%, size 492, with the retail
18-block CFG and ordered 14/14 relocations.  The exact Gold/Buka source instead
names a rowPixels pointer from `pixels + row * width`, indexes the encoded row
with an explicit byte count, and owns the encoded-run branch first.

A complete 2 x 51 structure/state matrix finished 102/102.  The donor parent
improved the clean object to 99.256760%, size 493.  Five unchanged-source
states reached 99.425674% at the retail size 494; all retained exact CFG and
14/14 relocations.  No exact island appeared.

Two follow-up matrices tested the first real register-allocation divergence:

  - scoped locals versus the full Gold top-declaration order, 102/102;
    scoped ownership remained best, while the top order peaked at 98.986490%;
  - four counter-initialization orders x two run-end scopes x 51 states,
    408/408.  Non-donor counter orders reached a 99.662160% clue in several
    disposable states but regressed clean output to 97.668920%.  The donor
    source order remained the credible clean parent.

Artifacts:
  build/createpcx-cross-version-loop-axes.json
  build/createpcx-cross-version-loop-manifest.json
  build/source-variant-batch/createpcx-cross-version-loop/results.json
  build/createpcx-local-order-axes.json
  build/createpcx-local-order-manifest.json
  build/source-variant-batch/createpcx-local-order/results.json
  build/createpcx-counter-lifetime-axes.json
  build/createpcx-counter-lifetime-manifest.json
  build/source-variant-batch/createpcx-counter-lifetime/results.json

Disposition: retain the exact cross-version row-encoder structure with scoped
locals.  Preserve the 99.425674% unchanged-source island and 99.662160%
alternate-order island as clue artifacts only.  No generated declarations or
metric-only counter reorder is retained.

Follow-up, 2026-08-05: accepted changes elsewhere in optimized Misc changed
the ambient TU state, so the earlier trial-22 coordinate no longer reproduced.
A fresh 50-state census found the same 99.425674% orbit at trial 13.  A complete
second forest layer over that representative found no new target state and
again peaked at 99.425674% (layer trial 42).  This closes the justified layered
island expansion without changing source.

Follow-up artifacts:
  build/createpcxfile-current-state-summary.json
  build/tu-state-noise/createpcxfile-current/
  build/createpcxfile-layer2-state-summary.json
  build/tu-state-noise/createpcxfile-layer2/
*/

#if 0
for (i32 row = 0; row < height; ++row) {
    i32 sourceIndex = 0;
    u8* rowPixels = pixels + row * width;
    i32 encodedSize = 0;
    // Gold/Buka indexed row encoder retained in src/BASE/Misc.cpp.
}
#endif
