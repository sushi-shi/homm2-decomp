// VC6 SP5 source product c79-events-monster-combat; RVA 0x4403b.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 16 arms, 16.593586s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-monster-combat/results.json and build/c79-events-monster-combat-axes.json
// Disposition: apply trial15. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'cast0': 'keep', 'cast1': 'keep', 'cast2': 'keep', 'cast3': 'keep', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 1 / {'cast0': 'keep', 'cast1': 'keep', 'cast2': 'keep', 'cast3': 'recover', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 2 / {'cast0': 'keep', 'cast1': 'keep', 'cast2': 'recover', 'cast3': 'keep', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 3 / {'cast0': 'keep', 'cast1': 'keep', 'cast2': 'recover', 'cast3': 'recover', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 4 / {'cast0': 'keep', 'cast1': 'recover', 'cast2': 'keep', 'cast3': 'keep', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 5 / {'cast0': 'keep', 'cast1': 'recover', 'cast2': 'keep', 'cast3': 'recover', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 6 / {'cast0': 'keep', 'cast1': 'recover', 'cast2': 'recover', 'cast3': 'keep', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 7 / {'cast0': 'keep', 'cast1': 'recover', 'cast2': 'recover', 'cast3': 'recover', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 8 / {'cast0': 'recover', 'cast1': 'keep', 'cast2': 'keep', 'cast3': 'keep', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 9 / {'cast0': 'recover', 'cast1': 'keep', 'cast2': 'keep', 'cast3': 'recover', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 10 / {'cast0': 'recover', 'cast1': 'keep', 'cast2': 'recover', 'cast3': 'keep', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 11 / {'cast0': 'recover', 'cast1': 'keep', 'cast2': 'recover', 'cast3': 'recover', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 12 / {'cast0': 'recover', 'cast1': 'recover', 'cast2': 'keep', 'cast3': 'keep', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 13 / {'cast0': 'recover', 'cast1': 'recover', 'cast2': 'keep', 'cast3': 'recover', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 14 / {'cast0': 'recover', 'cast1': 'recover', 'cast2': 'recover', 'cast3': 'keep', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True
// 15 / {'cast0': 'recover', 'cast1': 'recover', 'cast2': 'recover', 'cast3': 'recover', 'candidate': 'baseline'} / 1334 / e0a618f2ca0bb8df / b649a18cbdbab061 / 90:90 / 90,0,0,0,0 / True

// AXIS cast0
// BEFORE (complete exact span):
// | static_cast<i16>(monsterCount / stackCount + (monsterCount % stackCount > stackIdx))
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -static_cast<i16>(monsterCount / stackCount + (monsterCount % stackCount > stackIdx))
// | +monsterCount / stackCount + (monsterCount % stackCount > stackIdx)

// AXIS cast1
// BEFORE (complete exact span):
// | static_cast<i16>(
// |                     secondaryCount / stackCount + (secondaryCount % stackCount > stackIdx)
// |                 )
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,3 +1 @@
// | -static_cast<i16>(
// | -                    secondaryCount / stackCount + (secondaryCount % stackCount > stackIdx)
// | -                )
// | +secondaryCount / stackCount + (secondaryCount % stackCount > stackIdx)

// AXIS cast2
// BEFORE (complete exact span):
// | static_cast<i16>(
// |                     tertiaryCount / stackCount + (secondaryCount % stackCount > stackIdx)
// |                 )
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,3 +1 @@
// | -static_cast<i16>(
// | -                    tertiaryCount / stackCount + (secondaryCount % stackCount > stackIdx)
// | -                )
// | +tertiaryCount / stackCount + (secondaryCount % stackCount > stackIdx)

// AXIS cast3
// BEFORE (complete exact span):
// | static_cast<i16>(savedCounts[placement4[stackIdx]])
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -static_cast<i16>(savedCounts[placement4[stackIdx]])
// | +savedCounts[placement4[stackIdx]]
