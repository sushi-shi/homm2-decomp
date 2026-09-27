// VC6 SP5 source product s72-events-sound; RVA 0x42f2d.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.784154s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-sound/results.json and build/s72-events-sound-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'mineSound': 'keep', 'candidate': 'baseline'} / 1043 / 3d3cd3587eee551f / 4cc831b4d8c91336 / 50:53 / 49,0,0,1,3 / False
// 1 / {'mineSound': 'recover', 'candidate': 'baseline'} / 1036 / b28dfce431b040a6 / 3cb6f0c5e2497d14 / 50:53 / 48,1,0,1,3 / False

// AXIS mineSound
// BEFORE (complete exact span):
// |     const i32 H2_UNUSED(mineSound) = SOUND_MINE;
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    const i32 H2_UNUSED(mineSound) = SOUND_MINE;
