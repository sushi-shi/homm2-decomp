// VC6 SP5 family s71-xcamp-army-name-capacity; parent2abf1283 (capacity arm after C78/O08 integration).
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state census is claimed.
// build/source-variant-batch/s71-xcamp-army-name-capacity/results.json; RVA 0xb3e05
// Complete 2 arms; elapsed 1.799118s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// O08 changes only +0x20 DIR32 owner/addend: name table minus16 = original retail operand.
// Full98 native comparison allows exactly those two pinned physical-identity rewrites, nothing else.
// ShowInfo private EH/FS model gaps remain explicit; native equality does not close them.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 2109 / 2a32552fad1c723f / a285d29b42df3ed0 / 84:84 / 83,1,0,0,0 / True
// 1 / {'family': 'catalog_capacity22', 'candidate': 'baseline'} / 2109 / 625386656069bb4a / a285d29b42df3ed0 / 84:84 / 83,1,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// | char armyName[EXPANSION_CAMPAIGN_ARMY_NAME_BUFFER_SIZE];
// ALTERNATIVE catalog_capacity22 (unified delta from BEFORE):
// | --- before
// | +++ catalog_capacity22
// | @@ -1 +1 @@
// | -char armyName[EXPANSION_CAMPAIGN_ARMY_NAME_BUFFER_SIZE];
// | +char armyName[22];
