// VC6 SP5 source product a04-events-mine-value; RVA 0x44d73.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.179350s; no compile failure or timeout; source restored.
// build/source-variant-batch/a04-events-mine-value/results.json and build/a04-events-mine-value-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'array-index': 'keep', 'candidate': 'baseline'} / 11116 / fe51168989c69b33 / c73a53b52e3d49b1 / 367:374 / 366,0,0,1,7 / False
// 1 / {'array-index': 'recover', 'candidate': 'baseline'} / 11124 / 670fb7a68a2a4ab3 / 76c38fec0e706812 / 367:374 / 365,1,0,1,7 / False

// AXIS array-index
// BEFORE (complete exact span):
// | *(gaiTurnValueOfMine + x + y * MAP_WIDTH)
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -*(gaiTurnValueOfMine + x + y * MAP_WIDTH)
// | +gaiTurnValueOfMine[x + y * MAP_WIDTH]
