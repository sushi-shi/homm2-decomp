// VC6 SP5 source product s72-events-barrier-capacity; RVA 0x41fbc.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.977154s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-barrier-capacity/results.json and build/s72-events-barrier-capacity-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'input-capacity': 'keep', 'candidate': 'baseline'} / 293 / c17e05a02a074b98 / b9371b5dcd0f2262 / 5:5 / 5,0,0,0,0 / True
// 1 / {'input-capacity': 'recover', 'candidate': 'baseline'} / 293 / c17e05a02a074b98 / b9371b5dcd0f2262 / 5:5 / 5,0,0,0,0 / True

// AXIS input-capacity
// BEFORE (complete exact span):
// | char word[INPUT_BUFFER_SIZE];
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -char word[INPUT_BUFFER_SIZE];
// | +char word[INPUT_LENGTH];
