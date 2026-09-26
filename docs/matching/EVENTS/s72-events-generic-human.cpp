// VC6 SP5 source product s72-events-generic-human; RVA 0x42256.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 4 arms, 4.663349s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-generic-human/results.json and build/s72-events-generic-human-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'siteLevel8': 'keep', 'unusedSite': 'keep', 'candidate': 'baseline'} / 1788 / 41c0c86968f91750 / 23c61b993bdc9c20 / 71:72 / 70,0,0,1,1 / False
// 1 / {'siteLevel8': 'keep', 'unusedSite': 'recover', 'candidate': 'baseline'} / 1788 / 255ec0afffd62fa4 / 2eab449d124a661b / 71:72 / 70,0,0,1,1 / False
// 2 / {'siteLevel8': 'recover', 'unusedSite': 'keep', 'candidate': 'baseline'} / 1754 / d1274945577054f7 / 4b8917f31af517c5 / 71:72 / 69,1,0,1,1 / False
// 3 / {'siteLevel8': 'recover', 'unusedSite': 'recover', 'candidate': 'baseline'} / 1754 / 1bb371b48f1b48dc / fee2121e8d02971a / 71:72 / 69,1,0,1,1 / False

// AXIS siteLevel8
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(siteLevel8);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(siteLevel8);
// EXTRA BEFORE:
// |     siteLevel8 = cell->m_objectMetadata;
// EXTRA REPLACEMENT:
// EXTRA BEFORE:
// |     siteLevel8 >>= GENERIC_SITE_LEVEL_SHIFT;
// EXTRA REPLACEMENT:

// AXIS unusedSite
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(unusedSite);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(unusedSite);
