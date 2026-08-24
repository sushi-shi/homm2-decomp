/*
game::HasLateOverlay cell fetch
===============================

PoL 2.0 RVA 0x81541; Buka 2.1 RVA 0x5c03e.

Complete clean VC4.2 product: 2/2
---------------------------------
  build/has-late-overlay-cell-fetch-manifest.json
  build/source-variant-batch/has-late-overlay-cell-fetch/results.json

  WORLDMAP->Row(row) + col   100.000000%, 260 bytes
  WORLDMAP->GetCell(col,row) 100.000000%, 260 bytes  RETAINED

Both arms have identical text, the exact 17/17 block graph, and 0/0
relocations.

Disposition: invariant matching-decomp port of Buka's semantic GetCell
accessor. No gameplay or shipped-byte change.
*/
