// VC6 SP5 source product c79-events-recruit; RVA 0x435ad.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 4 arms, 4.900712s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-recruit/results.json and build/c79-events-recruit-axes.json
// Disposition: apply trial3. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'cast0': 'keep', 'cast1': 'keep', 'candidate': 'baseline'} / 224 / 283789b48ff509dc / 133171a75c3fcf71 / 6:6 / 6,0,0,0,0 / True
// 1 / {'cast0': 'keep', 'cast1': 'recover', 'candidate': 'baseline'} / 224 / 283789b48ff509dc / 8880c36353c03e72 / 6:6 / 6,0,0,0,0 / True
// 2 / {'cast0': 'recover', 'cast1': 'keep', 'candidate': 'baseline'} / 224 / 283789b48ff509dc / 8880c36353c03e72 / 6:6 / 6,0,0,0,0 / True
// 3 / {'cast0': 'recover', 'cast1': 'recover', 'candidate': 'baseline'} / 224 / 283789b48ff509dc / d69176e9f58afc37 / 6:6 / 6,0,0,0,0 / True

// AXIS cast0
// BEFORE (complete exact span):
// |     i16 availableCount = static_cast<i16>(cell->m_objectMetadata);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -    i16 availableCount = static_cast<i16>(cell->m_objectMetadata);
// | +    i16 availableCount = cell->m_objectMetadata;

// AXIS cast1
// BEFORE (complete exact span):
// | static_cast<u16>(availableCount)
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -static_cast<u16>(availableCount)
// | +availableCount
