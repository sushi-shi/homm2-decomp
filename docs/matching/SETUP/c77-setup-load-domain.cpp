// VC6 SP5 source family c77-setup-load-domain; parent292b9b73.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state census is claimed.
// build/source-variant-batch/c77-setup-load-domain/results.json; RVA 0x938d5
// Complete 2 arms; elapsed 1.663813s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// Full98 native objects/allocated sections/ordered relocations preserved after restoration.
// Nine private EH/FS site-model gaps remain explicit, not waived by native equality.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 651 / 8064135d39764986 / e3d67a031990e9de / 33:33 / 33,0,0,0,0 / True
// 1 / {'family': 'remove_1', 'candidate': 'baseline'} / 650 / 6a66f204bd7e90d0 / e979858402050259 / 33:33 / 33,0,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0x28b)
// | i32 game::PickLoadGame(void) {
// |     char fileMask[FILE_PATTERN_CAPACITY];
// |     i32 dialogResult;
// |     heroWindow* heroWin;
// |     fileRequester* fileReq;
// |
// |     if (gbWaitForRemoteReceive != 0)
// |         return 1;
// |
// |     if (gbInCampaign != 0) {
// |         sprintf(fileMask, "*.GMC");
// |     } else if (xIsPlayingExpansionCampaign != 0) {
// |         sprintf(fileMask, "*.GXC");
// |     } else if (gbRemoteOn != 0 && xNetHasOldPlayers != 0) {
// |         NormalDialog(
// |             /* Как минимум у одного игрока нет Героев II: Цена Верности. Вы можете выбрать карту только стандартного формата Героев II. */
// |             "\xca\xe0\xea \xec\xe8\xed\xe8\xec\xf3\xec \xf3 \xee\xe4\xed\xee\xe3\xee \xe8\xe3\xf0\xee\xea\xe0 \xed\xe5\xf2 \xc3\xe5\xf0\xee\xe5\xe2 II: \xd6\xe5\xed\xe0 \xc2\xe5\xf0\xed\xee\xf1\xf2\xe8. \xc2\xfb \xec\xee\xe6\xe5\xf2\xe5 \xe2\xfb\xe1\xf0\xe0\xf2\xfc \xea\xe0\xf0\xf2\xf3 \xf2\xee\xeb\xfc\xea\xee \xf1\xf2\xe0\xed\xe4\xe0\xf0\xf2\xed\xee\xe3\xee \xf4\xee\xf0\xec\xe0\xf2\xe0 \xc3\xe5\xf0\xee\xe5\xe2 II.",
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
// |         sprintf(fileMask, "*.GM%d", giNumHumanPlayers);
// |     } else {
// |         heroWin = new heroWindow(WINDOW_X, WINDOW_Y, "x_mapmnu.bin");
// |         if (heroWin == NULL)
// |             MemError();
// |         gpWindowManager->DoDialog(heroWin, ExpStdGameHandler, 0);
// |         delete heroWin;
// |
// |         switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// |             case CHOICE_ONE:
// |                 xIsExpansionMap = false;
// |                 break;
// |             case CHOICE_TWO:
// |                 xIsExpansionMap = true;
// |                 break;
// |             case DIALOG_CANCEL:
// |                 return 0;
// |         }
// |
// |         if (xIsExpansionMap != 0)
// |             sprintf(fileMask, "*.GX%d", giNumHumanPlayers);
// |         else
// |             sprintf(fileMask, "*.GM%d", giNumHumanPlayers);
// |     }
// |
// |     fileReq = new fileRequester(
// |         FILE_REQUESTER_X,
// |         FILE_REQUESTER_Y,
// |         FILE_REQUESTER_LOAD_GAME,
// |         fileMask,
// |         gcGamePath,
// |         fileMask
// |     );
// |     if (fileReq == NULL)
// |         MemError();
// |     dialogResult = gpExec->DoDialog(fileReq);
// |     if (dialogResult == FILE_REQUESTER_OK) {
// |         gpGame->LoadGame(gLastFilename, 0, 0);
// |         delete fileReq;
// |         return 1;
// |     } else {
// |         delete fileReq;
// |         return 0;
// |     }
// | }
// |
// ALTERNATIVE remove_1 (unified delta from BEFORE):
// | --- before
// | +++ remove_1
// | @@ -34,7 +34,7 @@
// |          gpWindowManager->DoDialog(heroWin, ExpStdGameHandler, 0);
// |          delete heroWin;
// |
// | -        switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +        switch (gpWindowManager->m_dialogResult) {
// |              case CHOICE_ONE:
// |                  xIsExpansionMap = false;
// |                  break;
