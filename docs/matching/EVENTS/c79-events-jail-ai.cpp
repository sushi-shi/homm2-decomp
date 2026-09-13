// VC6 SP5 source product c79-events-jail-ai; RVA 0x47e63.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.664439s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-jail-ai/results.json and build/c79-events-jail-ai-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'cast0': 'keep', 'candidate': 'baseline'} / 412 / 1a7f05c50616d60b / c9350c2cb8d21eff / 6:6 / 6,0,0,0,0 / True
// 1 / {'cast0': 'recover', 'candidate': 'baseline'} / 412 / 1a7f05c50616d60b / c9350c2cb8d21eff / 6:6 / 6,0,0,0,0 / True

// AXIS cast0
// BEFORE (complete exact span):
// |     gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] = static_cast<i8>(heroId9);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -    gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] = static_cast<i8>(heroId9);
// | +    gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] = heroId9;
// EXTRA BEFORE:
// |         static_cast<i8>(heroId9),
// EXTRA REPLACEMENT:
// |         heroId9,
