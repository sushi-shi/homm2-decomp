/*
combatManager::VaporizeCreature stripe-address expressions
==========================================================

PoL 2.0 RVA 0x26bbb; Buka 2.1 RVA 0x9e2b3.

Complete clean VC4.2 product: 4/4
---------------------------------
  build/vaporize-buka-stripes-manifest.json
  build/source-variant-batch/vaporize-buka-stripes/results.json

  PoL top + PoL bottom       93.575420%, 659 bytes
  Buka top + PoL bottom      93.586590%, 659 bytes
  PoL top + Buka bottom      96.145250%, 658 bytes
  Buka top + Buka bottom     96.156425%, 658 bytes  RETAINED

Retained expressions:
  *(row * WIDTH + gyModify + topOffset + firstY)
  *(gyModify - row * WIDTH - bottomOffset + lastY)

Final normal build:
  96.16%, exact retail size 658
  20/20 exact block skeleton
  complete ordered 31/31 relocations

The remaining loop-condition/register residual belongs to the separate SPELLS
slot-name campaign.

Disposition: matching-decomp port of both Buka stripe-address expressions.
No gameplay change.
*/
