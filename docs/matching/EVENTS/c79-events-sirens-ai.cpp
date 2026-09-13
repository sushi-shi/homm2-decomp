// VC6 SP5 source product c79-events-sirens-ai; RVA 0x47989.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.554071s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-sirens-ai/results.json and build/c79-events-sirens-ai-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'cast0': 'keep', 'candidate': 'baseline'} / 849 / 96a2ced660456cf4 / 7fa5b65d569be490 / 48:48 / 47,1,0,0,0 / True
// 1 / {'cast0': 'recover', 'candidate': 'baseline'} / 849 / 96a2ced660456cf4 / b98905690514e356 / 48:48 / 47,1,0,0,0 / True

// AXIS cast0
// BEFORE (complete exact span):
// | static_cast<i16>(quantity1 * AI_GENERIC_SITE_SIRENS_ARMY_REMAINDER)
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -static_cast<i16>(quantity1 * AI_GENERIC_SITE_SIRENS_ARMY_REMAINDER)
// | +quantity1 * AI_GENERIC_SITE_SIRENS_ARMY_REMAINDER
