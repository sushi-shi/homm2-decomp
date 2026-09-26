// VC6 SP5 source product c79-events-necromancy; RVA 0x48730.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.079919s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-necromancy/results.json and build/c79-events-necromancy-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'cast0': 'keep', 'candidate': 'baseline'} / 980 / adbe4cfa4b829b03 / e9b6a002f53a44b5 / 41:41 / 41,0,0,0,0 / True
// 1 / {'cast0': 'recover', 'candidate': 'baseline'} / 980 / adbe4cfa4b829b03 / e9b6a002f53a44b5 / 41:41 / 41,0,0,0,0 / True

// AXIS cast0
// BEFORE (complete exact span):
// | static_cast<double>(creatureCount[MONSTER_COMBAT_REMAINING_COUNT])
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -static_cast<double>(creatureCount[MONSTER_COMBAT_REMAINING_COUNT])
// | +creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
