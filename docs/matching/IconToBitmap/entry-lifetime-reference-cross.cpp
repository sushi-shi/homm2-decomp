/*
IconToBitmap, retail RVA 0x000d0570, size 0x4ed.
Selected-entry lifetime crossed with reference owners, 2026-07-27.

After retaining the semantic entryData record-base lifetime, the new structure
was crossed with the earlier same-family mutable-reference mechanisms:

  - open setup;
  - initial clipping helper owning gIcX0/gIcY references;
  - first-row helper owning gIcPitch/gIcY references;
  - both helpers;
  - with and without the left-clipped gIcCnt2 republication.

All eight structures received the clean state plus 50 top declaration-forest
states: 408/408 cells compiled in 331.97 seconds, the run was not truncated,
and source was restored:

  build/icon2b-entry-lifetime-reference-cross-manifest-20260727.json
  build/match-variants/icon2b-entry-lifetime-reference-cross-20260727/results.json

The open, initial-clipping, and combined helper families all reproduced the
new 81.673080% MAX in multiple states.  Each best object remained size 1220
with 79/83 relocations and exact 80-block flow.  The row-address-only family
was lower, peaking at 81.151100%.  The explicit gIcCnt2 republication remained
byte-identical to its open counterpart.

The helper cross therefore exposes no descendant beyond the selected-entry
orbit and recovers none of the four missing ordered relocations.  Retain the
narrow row-visibility owner already supported by an exact sibling, but do not
add modern parameterized setup/row helpers merely because they can reach the
same compiler island.  No generated declaration is retained.
*/
