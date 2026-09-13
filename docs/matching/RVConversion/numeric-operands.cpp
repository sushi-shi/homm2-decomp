// C49, parent d216dd01: 129/129 complete in 127.932s; no truncation.
// Original return: (i32)(left-associated sum of seven products).
// Original product: (float)p[IDX(resource)] * gafAITurnCostResource[IDX(resource)].
// Candidate final conversion: static_cast<i32>(same grouped sum).
// Per-product choices, independently for GOLD, WOOD, ORE, CRYSTAL, SULFUR,
// MERCURY, GEMS:
//   static_cast<float>(p[IDX(resource)]) * gafAITurnCostResource[IDX(resource)]
//   p[IDX(resource)] * gafAITurnCostResource[IDX(resource)]
// All 2^7 choices plus the original control: build/c49-rv-axes.json,
// build/c49-rv-manifest.json, build/c49-rv-batch/results.json.
// Every arm: 113 bytes, 8 relocs, strict exact; text 64045bdc772ae2ef,
// normalized relocations 492d48c11d0ca28d. Retain all implicit operands,
// explicit result truncation; no addition reassociation or fake compiler input.
// Final independent retail check and all89-function native TU proof pass.
