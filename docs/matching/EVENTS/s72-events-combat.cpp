// VC6 SP5 source product s72-events-combat; RVA 0x48c95.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.584891s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-combat/results.json and build/s72-events-combat-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'unusedCombat_a': 'keep', 'candidate': 'baseline'} / 1511 / f6e842ea60c73c6f / c1c5d0d744a7bb89 / 71:71 / 71,0,0,0,0 / True
// 1 / {'unusedCombat_a': 'recover', 'candidate': 'baseline'} / 1469 / bb05a88fca9db81e / 92f3f834214b4911 / 71:71 / 71,0,0,0,0 / True

// AXIS unusedCombat_a
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(unusedCombat_a);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(unusedCombat_a);
