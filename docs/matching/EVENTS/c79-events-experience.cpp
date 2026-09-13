// VC6 SP5 source product c79-events-experience; RVA 0x434c9.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.625650s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-experience/results.json and build/c79-events-experience-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'cast0': 'keep', 'candidate': 'baseline'} / 97 / b2e7e5b1e6cf5ec7 / 45cd2d5481ae4234 / 3:3 / 3,0,0,0,0 / True
// 1 / {'cast0': 'recover', 'candidate': 'baseline'} / 97 / b2e7e5b1e6cf5ec7 / 45cd2d5481ae4234 / 3:3 / 3,0,0,0,0 / True

// AXIS cast0
// BEFORE (complete exact span):
// | static_cast<i16>(oldLevel)
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -static_cast<i16>(oldLevel)
// | +oldLevel
