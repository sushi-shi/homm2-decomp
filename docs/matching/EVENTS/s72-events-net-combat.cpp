// VC6 SP5 source product s72-events-net-combat; RVA 0x48b04.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 4 arms, 4.315346s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-net-combat/results.json and build/s72-events-net-combat-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'netUnused7': 'keep', 'outcome': 'keep', 'candidate': 'baseline'} / 401 / 471b3887dedc6435 / b9ae0960ef86b86f / 13:13 / 13,0,0,0,0 / True
// 1 / {'netUnused7': 'keep', 'outcome': 'recover', 'candidate': 'baseline'} / 401 / 99b30729b0ec4ada / b9ae0960ef86b86f / 13:13 / 13,0,0,0,0 / True
// 2 / {'netUnused7': 'recover', 'outcome': 'keep', 'candidate': 'baseline'} / 401 / 803064ca98102972 / b9ae0960ef86b86f / 13:13 / 13,0,0,0,0 / True
// 3 / {'netUnused7': 'recover', 'outcome': 'recover', 'candidate': 'baseline'} / 401 / 454695aa1add4f3c / b9ae0960ef86b86f / 13:13 / 13,0,0,0,0 / True

// AXIS netUnused7
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(netUnused7);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(netUnused7);

// AXIS outcome
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(outcome);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(outcome);
