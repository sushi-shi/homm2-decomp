// VC6 SP5 source family s69-newgame-get-map; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/s69-newgame-get-map/results.json; RVA 0x754b0
// Complete 2 arms; elapsed 1.845351s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 466 / c735f7f2101e1eb1 / 559854548f3ea639 / 17:17 / 17,0,0,0 / True
// 1 / {'family': 'remove_1', 'candidate': 'baseline'} / 466 / b339121e35270140 / 064cadbe3f1e65e2 / 17:17 / 17,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0x1d2)
// | void game::GetMap(void) {
// |     // Unreferenced, but retail's frame reserves its 28 bytes above loadResult.
// |             tag_message H2_UNUSED(dlgMessage);
// |     fileRequester* requesterResult;
// |     i32 loadResult;
// |     char fileMask[FILE_MASK_CAPACITY];
// |     char savedName[SAVED_MAP_NAME_CAPACITY];
// |
// |     strcpy(savedName, gMapName);
// |     strcpy(
// |         gcCurMapName,
// |         ""
// |     );
// |     if (gbRemoteOn && xNetHasOldPlayers) {
// |         NormalDialog(
// |             "\xca\xe0\xea \xec\xe8\xed\xe8\xec\xf3\xec \xf3 \xee\xe4\xed\xee\xe3"
// |             "\xee \xe8\xe3\xf0\xee\xea\xe0 \xed\xe5\xf2 \xc3\xe5\xf0\xee\xe5\xe2 "
// |             "II: \xd6\xe5\xed\xe0 \xc2\xe5\xf0\xed\xee\xf1\xf2\xe8. \xc2\xfb \xec"
// |             "\xee\xe6\xe5\xf2\xe5 \xe2\xfb\xe1\xf0\xe0\xf2\xfc \xea\xe0\xf0\xf2"
// |             "\xf3 \xf2\xee\xeb\xfc\xea\xee \xf1\xf2\xe0\xed\xe4\xe0\xf0\xf2\xed"
// |             "\xee\xe3\xee \xf4\xee\xf0\xec\xe0\xf2\xe0 \xc3\xe5\xf0\xee\xe5\xe2 "
// |             "II.",
// |             NORMAL_DIALOG_INFO,
// |             -1,
// |             -1,
// |             -1,
// |             0,
// |             -1,
// |             0,
// |             -1,
// |             0
// |         );
// |         sprintf(fileMask, "*.%s", "MP2");
// |     } else if (xIsExpansionMap) {
// |         sprintf(fileMask, "*.%s", "MX2");
// |     } else {
// |         sprintf(fileMask, "*.%s", "MP2");
// |     }
// |
// |     requesterResult = new fileRequester(
// |         MAP_REQUESTER_X,
// |         MAP_REQUESTER_Y,
// |         FILE_REQUESTER_MAP_GAME,
// |         fileMask,
// |         gcMapPath,
// |         fileMask
// |     );
// |     if (requesterResult == NULL)
// |         MemError();
// |     loadResult = gpExec->DoDialog(requesterResult);
// |     if (loadResult == FILE_REQUESTER_OK) {
// |         delete requesterResult;
// |         strcpy(gMapName, gLastFilename);
// |         if (stricmp(savedName, gMapName) != 0) {
// |             strcpy(m_mapFilename, gMapName);
// |             ProcessNewMap(NULL);
// |         }
// |     } else {
// |         delete requesterResult;
// |         strcpy(gMapName, savedName);
// |     }
// | }
// |
// ALTERNATIVE remove_1 (unified delta from BEFORE):
// | --- before
// | +++ remove_1
// | @@ -1,7 +1,5 @@
// |   0x1d2)
// |  void game::GetMap(void) {
// | -    // Unreferenced, but retail's frame reserves its 28 bytes above loadResult.
// | -            tag_message H2_UNUSED(dlgMessage);
// |      fileRequester* requesterResult;
// |      i32 loadResult;
// |      char fileMask[FILE_MASK_CAPACITY];
