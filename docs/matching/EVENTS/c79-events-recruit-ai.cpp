// VC6 SP5 source product c79-events-recruit-ai; RVA 0x47cda.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.589433s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-recruit-ai/results.json and build/c79-events-recruit-ai-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'cast0': 'keep', 'candidate': 'baseline'} / 393 / d500d07874d49d82 / fec67042d31d78b1 / 15:17 / 14,0,0,1,2 / False
// 1 / {'cast0': 'recover', 'candidate': 'baseline'} / 393 / d500d07874d49d82 / 6452ee0b56f3a7e6 / 15:17 / 14,0,0,1,2 / False

// AXIS cast0
// BEFORE (complete exact span):
// |     recruitType &= IDX(EVENT_RECRUIT_TYPE_MASK);
// |     quantity = static_cast<i16>(cell->m_objectMetadata);
// |     quantity >>= EVENT_RECRUIT_COUNT_SHIFT;
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,3 +1,3 @@
// |      recruitType &= IDX(EVENT_RECRUIT_TYPE_MASK);
// | -    quantity = static_cast<i16>(cell->m_objectMetadata);
// | +    quantity = cell->m_objectMetadata;
// |      quantity >>= EVENT_RECRUIT_COUNT_SHIFT;
