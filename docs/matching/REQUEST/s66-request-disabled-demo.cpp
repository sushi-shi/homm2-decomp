// VC6 SP5 REQUEST source family s66-request-disabled-demo; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s66-request-disabled-demo/results.json; RVA 0x8e7b0
// Complete 2 arms; elapsed 1.650029s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 118 / 79633caefdfe8292 / 068b905655b19918 / 10:10 / 10,0,0,0 / True
// 1 / {'source_family': 'early_return', 'candidate': 'baseline'} / 16 / c3fbe9accd2b9f61 / e3b0c44298fc1c14 / 1:10 / 0,0,0,1 / False

// AXIS source_family
// BEFORE (complete exact span):
// |     return 1;
// |
// |     char mapName[FILE_REQUESTER_PATH_SIZE];
// |     i32 ix;
// |     strcpy(mapName, filename);
// |     mapName[LEGACY_MAP_BASENAME_SIZE] = 0;
// |     for (ix = 0; ix < LEGACY_MAP_BASENAME_SIZE; ++ix) {
// |         if (mapName[ix] == '.') {
// |             mapName[ix] = 0;
// |         }
// |     }
// |     if (strcmpi(mapName, "BROKENA") == 0 && CheckSumIsDemoOK(filename)) {
// |         return 1;
// |     }
// |     return 0;
// | }
// ALTERNATIVE early_return (unified source delta from BEFORE):
// | --- before
// | +++ early_return
// | @@ -1,16 +1,2 @@
// |      return 1;
// | -
// | -    char mapName[FILE_REQUESTER_PATH_SIZE];
// | -    i32 ix;
// | -    strcpy(mapName, filename);
// | -    mapName[LEGACY_MAP_BASENAME_SIZE] = 0;
// | -    for (ix = 0; ix < LEGACY_MAP_BASENAME_SIZE; ++ix) {
// | -        if (mapName[ix] == '.') {
// | -            mapName[ix] = 0;
// | -        }
// | -    }
// | -    if (strcmpi(mapName, "BROKENA") == 0 && CheckSumIsDemoOK(filename)) {
// | -        return 1;
// | -    }
// | -    return 0;
// |  }
