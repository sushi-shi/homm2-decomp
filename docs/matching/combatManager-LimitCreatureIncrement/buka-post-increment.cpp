/*
combatManager limit-creature counter increments
===============================================

PoL 2.0 RVAs: CastSpell 0x217be, TurnToStone 0x24e48,
BloodLustEffect 0x24f49. Buka 2.1 RVAs: 0x99653, 0x9c856, 0x9c97d.

Complete clean VC4.2 products: 2/2 per function
------------------------------------------------
  build/cast-spell-limit-increment-manifest.json
  build/turn-to-stone-limit-increment-manifest.json
  build/blood-lust-limit-increment-manifest.json

  CastSpell:
    pre-increment   99.753900%, 7864 bytes, 270/270 relocations
    post-increment  99.753900%, 7864 bytes, 270/270 relocations

  TurnToStone:
    pre-increment   100.000000%, 257 bytes, 23/23 relocations
    post-increment  100.000000%, 257 bytes, 23/23 relocations

  BloodLustEffect:
    pre-increment   100.000000%, 402 bytes, 40/40 relocations
    post-increment  100.000000%, 402 bytes, 40/40 relocations

All three pairs are byte- and relocation-identical under VC4.2. MirrorImage's
two corresponding sites had already been tested and adopted as byte-neutral.

Disposition: matching-decomp port of Buka's post-increment spelling at all
five sites. No gameplay change.
*/
