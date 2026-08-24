/*
philAI::RVConversion resource sum order
=======================================

PoL 2.0 RVA 0x3e7a2; Buka 2.1 RVA 0x84504.

Complete clean VC4.2 product: 2/2
--------------------------------
  build/rv-conversion-order-manifest.json
  build/source-variant-batch/rv-conversion-order/results.json

PoL order:
  ORE, GEMS, MERCURY, GOLD, WOOD, SULFUR, CRYSTAL
Buka order:
  GOLD, WOOD, ORE, CRYSTAL, SULFUR, MERCURY, GEMS

PoL's arm is exact at 166 bytes, exact 2/2 CFG, and ordered 8/8
relocations. Buka order retains size/CFG/relocations but falls to
97.090910%. Buka's own 113-byte function is independently exact.

Disposition: floating-point evaluation-order logic change; do not port.
The left-associated single-precision additions can round differently before
the final integer conversion. Keep each retail version's order.
*/
