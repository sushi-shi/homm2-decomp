// VC6 SP5 source product s72-events-stable-unused; RVA 0x42256.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 4 arms, 4.902629s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-stable-unused/results.json and build/s72-events-stable-unused-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'unusedOne18': 'keep', 'unusedTwo1': 'keep', 'candidate': 'baseline'} / 1788 / 41c0c86968f91750 / 23c61b993bdc9c20 / 71:72 / 70,0,0,1,1 / False
// 1 / {'unusedOne18': 'keep', 'unusedTwo1': 'recover', 'candidate': 'baseline'} / 1781 / c02a6c34e9441e39 / b5e3011bb703ccbe / 71:72 / 69,1,0,1,1 / False
// 2 / {'unusedOne18': 'recover', 'unusedTwo1': 'keep', 'candidate': 'baseline'} / 1781 / aeb6db50873bbc8f / b5e3011bb703ccbe / 71:72 / 69,1,0,1,1 / False
// 3 / {'unusedOne18': 'recover', 'unusedTwo1': 'recover', 'candidate': 'baseline'} / 1774 / f97bf1052ef3f808 / 53704d2e8cfdb2cd / 71:72 / 69,1,0,1,1 / False

// AXIS unusedOne18
// BEFORE (complete exact span):
// |     H2_ENUM_STORAGE(StableVisitResult, i32) unusedOne18;
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    H2_ENUM_STORAGE(StableVisitResult, i32) unusedOne18;
// EXTRA BEFORE:
// |             unusedOne18 = STABLE_VISIT_MOBILITY;
// EXTRA REPLACEMENT:

// AXIS unusedTwo1
// BEFORE (complete exact span):
// |     H2_ENUM_STORAGE(StableVisitResult, i32) unusedTwo1;
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    H2_ENUM_STORAGE(StableVisitResult, i32) unusedTwo1;
// EXTRA BEFORE:
// |             unusedTwo1 = STABLE_VISIT_UPGRADE;
// EXTRA REPLACEMENT:
