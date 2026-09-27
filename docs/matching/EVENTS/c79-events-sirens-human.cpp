// VC6 SP5 source product c79-events-sirens-human; RVA 0x42256.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.597618s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-sirens-human/results.json and build/c79-events-sirens-human-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'cast0': 'keep', 'candidate': 'baseline'} / 1788 / 41c0c86968f91750 / 23c61b993bdc9c20 / 71:72 / 70,0,0,1,1 / False
// 1 / {'cast0': 'recover', 'candidate': 'baseline'} / 1788 / 41c0c86968f91750 / 86e344899c6cff97 / 71:72 / 70,0,0,1,1 / False

// AXIS cast0
// BEFORE (complete exact span):
// | static_cast<i16>(oldQuantity3 * GENERIC_SITE_SIREN_ARMY_REMAINDER)
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -static_cast<i16>(oldQuantity3 * GENERIC_SITE_SIREN_ARMY_REMAINDER)
// | +oldQuantity3 * GENERIC_SITE_SIREN_ARMY_REMAINDER
