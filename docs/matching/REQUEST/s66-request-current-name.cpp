// VC6 SP5 REQUEST source family s66-request-current-name; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s66-request-current-name/results.json; RVA 0x8f275
// Complete 2 arms; elapsed 1.652883s; input restored.
// Disposition: retain all scalar removals in this family (nine-character scratch for S66); combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1099 / ef0b205e008ae0b1 / 89783870fc0e8045 / 46:46 / 46,0,0,0 / True
// 1 / {'source_family': 'nine_chars', 'candidate': 'baseline'} / 1099 / ef0b205e008ae0b1 / 89783870fc0e8045 / 46:46 / 46,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |         if (m_mode == FILE_REQUESTER_MAP_GAME) {
// |             char mapName[CURRENT_MAP_NAME_CAPACITY];
// |             fileSlot = 0;
// ALTERNATIVE nine_chars (unified source delta from BEFORE):
// | --- before
// | +++ nine_chars
// | @@ -1,3 +1,3 @@
// |          if (m_mode == FILE_REQUESTER_MAP_GAME) {
// | -            char mapName[CURRENT_MAP_NAME_CAPACITY];
// | +            char mapName[CURRENT_MAP_NAME_CLEAR_SIZE];
// |              fileSlot = 0;
