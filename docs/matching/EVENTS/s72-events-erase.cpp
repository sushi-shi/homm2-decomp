// VC6 SP5 source product s72-events-erase; RVA 0x41609.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 8 arms, 8.312104s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-erase/results.json and build/s72-events-erase-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'erased': 'keep', 'unused_m': 'keep', 'changed_f': 'keep', 'candidate': 'baseline'} / 2308 / 14137659ce549714 / cde985b55d2c55b4 / 125:125 / 124,1,0,0,0 / True
// 1 / {'erased': 'keep', 'unused_m': 'keep', 'changed_f': 'recover', 'candidate': 'baseline'} / 2301 / 74f32ee33e0893ce / e9a3eb829e88e644 / 125:125 / 123,2,0,0,0 / True
// 2 / {'erased': 'keep', 'unused_m': 'recover', 'changed_f': 'keep', 'candidate': 'baseline'} / 2308 / 2b2fae717f66f445 / 8a80f01ec6883a91 / 125:125 / 124,1,0,0,0 / True
// 3 / {'erased': 'keep', 'unused_m': 'recover', 'changed_f': 'recover', 'candidate': 'baseline'} / 2301 / c51ca079f0238f52 / ef3980112e4b5fdd / 125:125 / 123,2,0,0,0 / True
// 4 / {'erased': 'recover', 'unused_m': 'keep', 'changed_f': 'keep', 'candidate': 'baseline'} / 2294 / 3e6a0a02a1dd3aa4 / d70178515d908666 / 125:125 / 122,3,0,0,0 / True
// 5 / {'erased': 'recover', 'unused_m': 'keep', 'changed_f': 'recover', 'candidate': 'baseline'} / 2287 / f079d858fbef6cd8 / de2fbab358942f6e / 125:125 / 121,4,0,0,0 / True
// 6 / {'erased': 'recover', 'unused_m': 'recover', 'changed_f': 'keep', 'candidate': 'baseline'} / 2294 / 1a0e275bbdd28d82 / 3cbd5a03d046e9a6 / 125:125 / 122,3,0,0,0 / True
// 7 / {'erased': 'recover', 'unused_m': 'recover', 'changed_f': 'recover', 'candidate': 'baseline'} / 2287 / 3b47b1a2b4e1ee94 / 68aaa6519dbe0cbc / 125:125 / 121,4,0,0,0 / True

// AXIS erased
// BEFORE (complete exact span):
// |     b32 H2_UNUSED(erased) = false;
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    b32 H2_UNUSED(erased) = false;
// EXTRA BEFORE:
// |     erased = true;
// EXTRA REPLACEMENT:

// AXIS unused_m
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(unused_m)[ERASE_COORDINATE_COUNT];
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(unused_m)[ERASE_COORDINATE_COUNT];

// AXIS changed_f
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(changed_f);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(changed_f);
// EXTRA BEFORE:
// |         changed_f = 0;
// EXTRA REPLACEMENT:
