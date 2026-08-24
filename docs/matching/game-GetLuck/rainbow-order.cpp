/*
game::GetLuck Rainbow order: version logic
===========================================

PoL 2.0 RVA 0x80ff9: exact, 294 bytes, 25/25 blocks, 6/6 relocs.
Buka 2.1 RVA 0x5bb52: exact, 292 bytes, 25/25 blocks, 6/6 relocs.

Complete clean VC4.2 product: 2/2
---------------------------------
  build/get-luck-rainbow-order-manifest.json
  build/source-variant-batch/get-luck-rainbow-order/results.json

  PoL: clamps -> Battle Garb -> Rainbow  100.000000%, 294 bytes
  Buka: Rainbow -> clamps -> Battle Garb   74.912500%, 294 bytes

Behavior at luck 3 with Rainbow (+2), without Battle Garb:
  PoL returns 5; Buka returns 3.

Disposition: logic change. Keep PoL order in 2.0; do not backport Buka.
*/
