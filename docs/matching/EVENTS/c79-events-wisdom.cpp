// VC6 SP5 source product c79-events-wisdom; RVA 0x3b640.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.178423s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-wisdom/results.json and build/c79-events-wisdom-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'enum-index': 'keep', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / c2595203b0e45670 / 557:561 / 556,0,0,1,4 / False
// 1 / {'enum-index': 'recover', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / 3044dbcab1db303c / 557:561 / 556,0,0,1,4 / False

// AXIS enum-index
// BEFORE (complete exact span):
// | static_cast<i32>(eventHero2->m_secondarySkills[IDX(HERO_SKILL_WISDOM)])
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1 @@
// | -static_cast<i32>(eventHero2->m_secondarySkills[IDX(HERO_SKILL_WISDOM)])
// | +IDX(eventHero2->m_secondarySkills[IDX(HERO_SKILL_WISDOM)])
