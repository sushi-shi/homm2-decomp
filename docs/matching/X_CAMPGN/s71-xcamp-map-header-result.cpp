// VC6 SP5 family s71-xcamp-map-header-result; parent2abf1283 (capacity arm after C78/O08 integration).
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state census is claimed.
// build/source-variant-batch/s71-xcamp-map-header-result/results.json; RVA 0xb3427
// Complete 3 arms; elapsed 2.170327s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// O08 changes only +0x20 DIR32 owner/addend: name table minus16 = original retail operand.
// Full98 native comparison allows exactly those two pinned physical-identity rewrites, nothing else.
// ShowInfo private EH/FS model gaps remain explicit; native equality does not close them.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1683 / f5a7559553c2560b / a0e4d7059168b409 / 84:84 / 83,1,0,0,0 / True
// 1 / {'family': 'remove_slot_store', 'candidate': 'baseline'} / 1680 / 7672bf69ef6e3a5d / 4fb6a794820a1bd8 / 84:84 / 82,2,0,0,0 / True
// 2 / {'family': 'remove_store', 'candidate': 'baseline'} / 1680 / f23d7545af6c930c / e3af87027b6e7b27 / 84:84 / 82,2,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(mapHeaderResult) =
// |         GetMapHeader(gpGame->m_mapFilename, &gpGame->m_mapHeader);
// ALTERNATIVE remove_slot_store (unified delta from BEFORE):
// | --- before
// | +++ remove_slot_store
// | @@ -1,2 +1 @@
// | -    i32 H2_UNUSED(mapHeaderResult) =
// | -        GetMapHeader(gpGame->m_mapFilename, &gpGame->m_mapHeader);
// | +    GetMapHeader(gpGame->m_mapFilename, &gpGame->m_mapHeader);
// ALTERNATIVE remove_store (unified delta from BEFORE):
// | --- before
// | +++ remove_store
// | @@ -1,2 +1,2 @@
// | -    i32 H2_UNUSED(mapHeaderResult) =
// | -        GetMapHeader(gpGame->m_mapFilename, &gpGame->m_mapHeader);
// | +    i32 H2_UNUSED(mapHeaderResult);
// | +    GetMapHeader(gpGame->m_mapFilename, &gpGame->m_mapHeader);
