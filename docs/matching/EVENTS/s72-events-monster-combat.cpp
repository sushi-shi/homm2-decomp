// VC6 SP5 source product s72-events-monster-combat; RVA 0x4403b.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 4 arms, 4.438037s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-monster-combat/results.json and build/s72-events-monster-combat-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'combatUnused': 'keep', 'combatUnused0': 'keep', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 1 / {'combatUnused': 'keep', 'combatUnused0': 'recover', 'candidate': 'baseline'} / 1334 / 6bc3cc1254596a5a / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 2 / {'combatUnused': 'recover', 'combatUnused0': 'keep', 'candidate': 'baseline'} / 1334 / 74e3893e1e74e211 / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 3 / {'combatUnused': 'recover', 'combatUnused0': 'recover', 'candidate': 'baseline'} / 1334 / e6cd58ec9bac78f3 / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True

// AXIS combatUnused
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(combatUnused);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(combatUnused);

// AXIS combatUnused0
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(combatUnused0);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(combatUnused0);
