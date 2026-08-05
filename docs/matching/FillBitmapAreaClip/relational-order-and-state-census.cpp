/*
FillBitmapAreaClip (RVA 0x000ca450, retail size 276)
PoL-specific inline structure and bounded relational/state census, 2026-08-05.

The exact Gold/Buka source calls FillBitmapArea after clipping.  PoL retail has
no such call: it owns the complete memset loop, and the retained source already
has retail size, the exact 16-block CFG, and ordered 7/7 relocations.  The clean
object is 97.533330%; a 50-trial unchanged-source forest census reproduced the
durable 98.444440% trial-3 state, four raw bytes from retail.

The three visible clean-state differences were semantically equivalent
comparison ownerships:
  cx2 <= x - 1 + w       versus x - 1 + w >= cx2
  cy2 <= y - 1 + h       versus y - 1 + h >= cy2
  gFillRow < h           versus h > gFillRow

All three were independent axes in a complete 8 x 51 matrix.  All 408 cells
finished in 128.51 seconds.  Every spelling arm was byte-identical within each
compiler state; none exceeded 98.444440%, and all retained the exact CFG and
relocation stream.  Thus the matrix rejects these spellings only in the
current structurally aligned parent orbit.

Artifacts:
  build/fillbitmapareaclip-state-summary.json
  build/tu-state-noise/fillbitmapareaclip/
  build/fillbitmapareaclip-relational-axes.json
  build/fillbitmapareaclip-relational-manifest.json
  build/source-variant-batch/fillbitmapareaclip-relational/results.json

Disposition: retain the PoL inline-loop structure and existing MAX; retain no
generated state or relational spelling change.
*/

#if 0
if (x - 1 + w >= cx2) {}
if (y - 1 + h >= cy2) {}
do {} while (h > gFillRow);
#endif
