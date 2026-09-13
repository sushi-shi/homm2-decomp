// VC6 SP5 source product s72-events-sphinx-capacity; RVA 0x3b640.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.134541s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-sphinx-capacity/results.json and build/s72-events-sphinx-capacity-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'text-capacity': 'keep', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / c2595203b0e45670 / 557:561 / 556,0,0,1,4 / False
// 1 / {'text-capacity': 'recover', 'candidate': 'baseline'} / 24281 / 4906051ab21cba11 / e2001868c801a201 / 557:561 / 556,0,0,1,4 / False

// AXIS text-capacity
// BEFORE (complete exact span):
// | char sphinxAnswer_a[SPHINX_ANSWER_BUFFER_SIZE];
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -char sphinxAnswer_a[SPHINX_ANSWER_BUFFER_SIZE];
// | +char sphinxAnswer_a[SPHINX_INPUT_LENGTH];
