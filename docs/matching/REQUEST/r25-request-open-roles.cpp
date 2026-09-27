// VC6 SP5 REQUEST source family r25-request-open-roles; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r25-request-open-roles/results.json; RVA 0x8f275
// Complete 2 arms; elapsed 1.751514s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1099 / ef0b205e008ae0b1 / 89783870fc0e8045 / 46:46 / 46,0,0,0 / True
// 1 / {'source_family': 'name_offset', 'candidate': 'baseline'} / 1099 / 6219e9107f1bf521 / f0af8de8146f1374 / 46:46 / 46,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |             char mapName[CURRENT_MAP_NAME_CAPACITY];
// |             fileSlot = 0;
// |             memset(mapName, 0, CURRENT_MAP_NAME_CLEAR_SIZE);
// |             while (fileSlot < LEGACY_MAP_BASENAME_SIZE && gMapName[fileSlot] != 0
// |                    && gMapName[fileSlot] != '.') {
// |                 mapName[fileSlot] = gMapName[fileSlot];
// |                 ++fileSlot;
// |             }
// ALTERNATIVE name_offset (unified source delta from BEFORE):
// | --- before
// | +++ name_offset
// | @@ -1,8 +1,8 @@
// |              char mapName[CURRENT_MAP_NAME_CAPACITY];
// | -            fileSlot = 0;
// | +            i32 nameOffset = 0;
// |              memset(mapName, 0, CURRENT_MAP_NAME_CLEAR_SIZE);
// | -            while (fileSlot < LEGACY_MAP_BASENAME_SIZE && gMapName[fileSlot] != 0
// | -                   && gMapName[fileSlot] != '.') {
// | -                mapName[fileSlot] = gMapName[fileSlot];
// | -                ++fileSlot;
// | +            while (nameOffset < LEGACY_MAP_BASENAME_SIZE && gMapName[nameOffset] != 0
// | +                   && gMapName[nameOffset] != '.') {
// | +                mapName[nameOffset] = gMapName[nameOffset];
// | +                ++nameOffset;
// |              }
