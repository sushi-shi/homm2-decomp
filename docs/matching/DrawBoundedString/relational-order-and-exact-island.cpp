/*
font::DrawBoundedString (RVA 0x000c7470, retail size 787)
PoL /Od relational-order matrix and exact compiler-state island, 2026-08-05.

The exact later tree documents a shared line-breaking declaration family, but
its ExtractLine helper and Cyrillic hyphenation are later-build features and do
not exist in PoL.  The retained PoL implementation was already structurally
correct: clean size 787, exact 39-block graph, identical 0x4c-byte frame and
local-offset set, and ordered 5/5 relocations.  The 72.383620% fuzzy score hid
only two semantically equivalent branch orientations (six raw bytes).

The visible ownerships were crossed independently:
  idx >= lineStartD       versus lineStartD <= idx
  idx <= lineStartD       versus lineStartD >= idx

The complete 4 x 51 matrix finished all 204 cells.  Both axes were byte-neutral
within every compiler state.  Forest/top trial 1 closed all four spelling arms
exactly; trial 49 also reproduced the exact state.

A focused replay of trial 1 (seed 1213156658) proved 100.000000%, retail size
787, exact 39/39 CFG, and ordered 5/5 relocations for unchanged source hash
f827dcf88c44.  The hash-scoped MAX was recorded as 100.  No source spelling or
generated declaration is retained.

Artifacts:
  build/drawboundedstring-relational-axes.json
  build/drawboundedstring-relational-manifest.json
  build/source-variant-batch/drawboundedstring-relational/results.json
  build/drawboundedstring-exact-replay-summary.json
  build/tu-state-noise/drawboundedstring-exact-replay/

Disposition: retain the existing PoL line-breaking source and the audited exact
MAX; relational reversals are erased in this parent orbit.
*/

#if 0
while (text2[idx] != ' ' && lineStartD <= idx) {}
if (lineStartD >= idx) {}
#endif
