/*
ComputeUALoc map-cell probe reads
=================================

PoL 2.0 RVA 0x710f3; Buka 2.1 RVA 0x4c7a3.

Complete clean VC4.2 product: 16/16
-----------------------------------
  build/compute-ua-loc-cell-reads-manifest.json
  build/source-variant-batch/compute-ua-loc-cell-reads/results.json

The four independent axes cover trigger type, object index, overlay index,
and terrain image. Every Row/GetCell combination emits the same exact state:
100.000000%, 1037 bytes, exact 36/36 CFG, ordered 36/36 relocations.

Disposition: invariant matching-decomp port of all four Buka GetCell reads.
No gameplay or shipped-byte change.
*/
