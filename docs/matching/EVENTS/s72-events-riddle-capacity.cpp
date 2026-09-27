// VC6 SP5 source product s72-events-riddle-capacity; RVA 0x49b5c.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.907141s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-riddle-capacity/results.json and build/s72-events-riddle-capacity-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'answer': 'keep', 'candidate': 'baseline'} / 143 / 2f4f96d145db858e / 6d9116cfb9a76e55 / 11:7 / 7,0,0,0,4 / False
// 1 / {'answer': 'recover', 'candidate': 'baseline'} / 143 / 2f4f96d145db858e / 6d9116cfb9a76e55 / 11:7 / 7,0,0,0,4 / False

// AXIS answer
// BEFORE (complete exact span):
// | char answerPrefix[RIDDLE_ANSWER_BUFFER_SIZE];
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -char answerPrefix[RIDDLE_ANSWER_BUFFER_SIZE];
// | +char answerPrefix[RIDDLE_PREFIX_LENGTH + 1];
