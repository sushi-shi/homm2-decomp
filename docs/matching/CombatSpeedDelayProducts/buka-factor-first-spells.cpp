// SOURCE/SPELLS gfCombatSpeedMod delay-product family
//
// PoL 2.0 source before:
//   gfCombatSpeedMod[gConfig.combatSpeed] * DELAY
// Buka 2.1 exact source / retained PoL source:
//   DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
//
// Reviewed manifest generator:
//   build/make-gf-combat-speed-order-axes.py
// Results:
//   build/source-variant-batch/gf-speed-*/results.json
//
// Complete clean products (2/2 each):
//
//   function             score       size  relocs  sites
//   CastSpell            99.753900   7864     270      2
//   Fireball            100.000000   1571      51      1
//   ElementalStorm       99.990524    763      31      1
//   Armageddon            98.382866   2724      79      4
//   ChainLightning        99.983930    865      37      2
//   VaporizeCreature      93.575420    659      31      1
//   RippleCreature        98.847650   1425      68      1
//   MirrorImage           98.852300   1480      38      2
//   DoBlast               99.665200    826      50      1
//   Resurrect             93.233990   1599      33      1
//   Earthquake            99.864040   2353      62      2
//
// In every product the PoL and Buka arms have identical function text,
// ordered relocation, size, score, and topology metrics. After applying all
// 18 Buka spellings together and rebuilding the TU, every affected function's
// text_sha and reloc_sha still equals its retained PoL baseline.
//
// Disposition: matching decomp; adopt the exact Buka factor-first spelling in
// PoL. No gameplay or shipped-byte change. MeteorShower and DoBolt are not in
// this product: exact Buka itself retains their speed-first source order.

