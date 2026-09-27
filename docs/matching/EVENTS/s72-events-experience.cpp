// VC6 SP5 source product s72-events-experience; RVA 0x434c9.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 8 arms, 7.727511s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-experience/results.json and build/s72-events-experience-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'unusedLevel2': 'keep', 'unusedLevel1': 'keep', 'levelGap1': 'keep', 'candidate': 'baseline'} / 97 / b2e7e5b1e6cf5ec7 / 45cd2d5481ae4234 / 3:3 / 3,0,0,0,0 / True
// 1 / {'unusedLevel2': 'keep', 'unusedLevel1': 'keep', 'levelGap1': 'recover', 'candidate': 'baseline'} / 97 / 9d3573270b45f630 / 45cd2d5481ae4234 / 3:3 / 3,0,0,0,0 / True
// 2 / {'unusedLevel2': 'keep', 'unusedLevel1': 'recover', 'levelGap1': 'keep', 'candidate': 'baseline'} / 97 / 53a991b1b1fb2751 / 45cd2d5481ae4234 / 3:3 / 3,0,0,0,0 / True
// 3 / {'unusedLevel2': 'keep', 'unusedLevel1': 'recover', 'levelGap1': 'recover', 'candidate': 'baseline'} / 97 / e18989d0f8ea6299 / 45cd2d5481ae4234 / 3:3 / 3,0,0,0,0 / True
// 4 / {'unusedLevel2': 'recover', 'unusedLevel1': 'keep', 'levelGap1': 'keep', 'candidate': 'baseline'} / 97 / 53a991b1b1fb2751 / 45cd2d5481ae4234 / 3:3 / 3,0,0,0,0 / True
// 5 / {'unusedLevel2': 'recover', 'unusedLevel1': 'keep', 'levelGap1': 'recover', 'candidate': 'baseline'} / 97 / e18989d0f8ea6299 / 45cd2d5481ae4234 / 3:3 / 3,0,0,0,0 / True
// 6 / {'unusedLevel2': 'recover', 'unusedLevel1': 'recover', 'levelGap1': 'keep', 'candidate': 'baseline'} / 97 / a733a8aefa6c19c3 / 45cd2d5481ae4234 / 3:3 / 3,0,0,0,0 / True
// 7 / {'unusedLevel2': 'recover', 'unusedLevel1': 'recover', 'levelGap1': 'recover', 'candidate': 'baseline'} / 97 / c98bf5e52aee3359 / 45cd2d5481ae4234 / 3:3 / 3,0,0,0,0 / True

// AXIS unusedLevel2
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(unusedLevel2);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(unusedLevel2);

// AXIS unusedLevel1
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(unusedLevel1);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(unusedLevel1);

// AXIS levelGap1
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(levelGap1);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(levelGap1);
