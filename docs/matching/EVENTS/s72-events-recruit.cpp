// VC6 SP5 source product s72-events-recruit; RVA 0x435ad.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 4 arms, 4.319994s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-recruit/results.json and build/s72-events-recruit-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'recruitMessage': 'keep', 'eventResult': 'keep', 'candidate': 'baseline'} / 224 / 283789b48ff509dc / 133171a75c3fcf71 / 6:6 / 6,0,0,0,0 / True
// 1 / {'recruitMessage': 'keep', 'eventResult': 'recover', 'candidate': 'baseline'} / 224 / d4192a5ffb6c60aa / 8880c36353c03e72 / 6:6 / 6,0,0,0,0 / True
// 2 / {'recruitMessage': 'recover', 'eventResult': 'keep', 'candidate': 'baseline'} / 224 / 51d41e17dbb88a55 / 8880c36353c03e72 / 6:6 / 6,0,0,0,0 / True
// 3 / {'recruitMessage': 'recover', 'eventResult': 'recover', 'candidate': 'baseline'} / 224 / c6f908d50aff1de6 / d69176e9f58afc37 / 6:6 / 6,0,0,0,0 / True

// AXIS recruitMessage
// BEFORE (complete exact span):
// |     tag_message H2_UNUSED(recruitMessage);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    tag_message H2_UNUSED(recruitMessage);

// AXIS eventResult
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(eventResult);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(eventResult);
