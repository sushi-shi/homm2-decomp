// VC6 SP5 source product c79-events-recruit-human; RVA 0x42952.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.567886s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-recruit-human/results.json and build/c79-events-recruit-human-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'cast0': 'keep', 'candidate': 'baseline'} / 368 / 648c564f141b1309 / baee7419de744f0f / 13:13 / 12,1,0,0,0 / True
// 1 / {'cast0': 'recover', 'candidate': 'baseline'} / 368 / 648c564f141b1309 / 0fc5f27ec86d170b / 13:13 / 12,1,0,0,0 / True

// AXIS cast0
// BEFORE (complete exact span):
// |     recruitSiteType &= IDX(EVENT_RECRUIT_TYPE_MASK);
// |     quantity = static_cast<i16>(cell->m_objectMetadata);
// |     quantity >>= EVENT_RECRUIT_COUNT_SHIFT;
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,3 +1,3 @@
// |      recruitSiteType &= IDX(EVENT_RECRUIT_TYPE_MASK);
// | -    quantity = static_cast<i16>(cell->m_objectMetadata);
// | +    quantity = cell->m_objectMetadata;
// |      quantity >>= EVENT_RECRUIT_COUNT_SHIFT;
