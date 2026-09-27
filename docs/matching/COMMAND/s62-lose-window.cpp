// VC6 SP5 COMMAND source family s62-lose-window; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-lose-window/results.json; RVA 0x30118
// Complete 2 arms; elapsed 2.166309s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 899 / a05f4133a4cf7600 / c33004ecab30c6a9 / 36:36 / 36,0,0,0 / True
// 1 / {'local_role_subset': 'word', 'candidate': 'baseline'} / 892 / 66426e1d4c1b65aa / 163c148699f641a3 / 36:36 / 35,1,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | void combatManager::DoLoseWindow(void) {
// |     CombatResult losingSide;
// |     i32 H2_UNUSED(unusedLoseWord) = IDX(COMBAT_RESULT_ATTACKER);
// |     tag_message message;
// |     char animationFile[WIN_LOSE_ANIMATION_FILENAME_SIZE];
// |
// |     if (giCurPlayer == m_playerId[IDX(COMBAT_ATTACKER_SIDE)]
// |         && gbThisNetHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] != 0) {
// |         losingSide = COMBAT_RESULT_ATTACKER;
// |     } else if (giCurPlayer == m_playerId[IDX(COMBAT_DEFENDER_SIDE)]
// |                && gbThisNetHumanPlayer[m_playerId[IDX(COMBAT_DEFENDER_SIDE)]] != 0) {
// |         losingSide = COMBAT_RESULT_DEFENDER;
// |     } else if (m_playerId[IDX(COMBAT_ATTACKER_SIDE)] != -1
// |                && gbThisNetHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] != 0) {
// |         losingSide = COMBAT_RESULT_ATTACKER;
// |     } else {
// |         losingSide = COMBAT_RESULT_DEFENDER;
// |     }
// |
// |     gbShowingLoseWindow = true;
// |     if (gbCombatSurrender != 0) {
// |         sprintf(animationFile, "cmbtsurr.icn");
// |         gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_CYCLE_SECOND;
// |     } else if (gbRetreatWin != 0) {
// |         sprintf(animationFile, "cmbtfle1.icn");
// |         gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_FLEE;
// |     } else {
// |         sprintf(animationFile, "cmbtlos1.icn");
// |         gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_LOSS;
// |     }
// |
// |     m_winLoseWindow = new heroWindow(WIN_LOSE_WINDOW_X, WIN_LOSE_WINDOW_Y, "wincmbt.bin");
// |     if (m_winLoseWindow == NULL)
// |         MemError();
// |
// |     if (m_heroes[IDX(losingSide)] != NULL) {
// |         if (gbCombatSurrender != 0) {
// |             sprintf(
// |                 gText,
// |                 cBattleResults[IDX(RESULT_TEXT_HERO_SURRENDER)],
// |                 m_heroes[IDX(losingSide)]->m_name
// |             );
// |         } else if (gbRetreatWin != 0) {
// |             sprintf(
// |                 gText,
// |                 cBattleResults[IDX(RESULT_TEXT_HERO_FLEE)],
// |                 m_heroes[IDX(losingSide)]->m_name
// |             );
// |         } else {
// |             sprintf(
// |                 gText,
// |                 cBattleResults[IDX(RESULT_TEXT_HERO_DEFEAT)],
// |                 m_heroes[IDX(losingSide)]->m_name
// |             );
// |         }
// |     } else {
// |         if (gbCombatSurrender != 0) {
// |             sprintf(gText, cBattleResults[IDX(RESULT_TEXT_FORCES_SURRENDER)]);
// |         } else if (gbRetreatWin != 0) {
// |             sprintf(gText, cBattleResults[IDX(RESULT_TEXT_FORCES_FLEE)]);
// |         } else {
// |             sprintf(gText, cBattleResults[IDX(RESULT_TEXT_FORCES_DEFEAT)]);
// |         }
// |     }
// |
// |     message.type = MESSAGE_WIDGET;
// |     message.payload.widget.command = COMBAT_WIN_LOSE_RESOURCE_COMMAND;
// |     message.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
// |     message.payload.widget.data.text = animationFile;
// |     m_winLoseWindow->BroadcastMessage(message);
// |     message.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
// |     message.payload.widget.data.text = animationFile;
// |     m_winLoseWindow->BroadcastMessage(message);
// |     message.type = MESSAGE_WIDGET;
// |     message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
// |     message.payload.widget.id = WIN_LOSE_TEXT_ID;
// |     message.payload.widget.data.text = gText;
// |     m_winLoseWindow->BroadcastMessage(message);
// |     ShowDeadArmies(m_winLoseWindow);
// |     if (gbRemoteOn != 0 && gbThisNetGotAdventureControl == 0)
// |         giDialogTimeout = KBTickCount() + WIN_LOSE_DIALOG_TIMEOUT;
// |     gpWindowManager->DoDialog(m_winLoseWindow, WinCombatHandler, 0);
// |     giDialogTimeout = 0;
// |     delete m_winLoseWindow;
// |     m_winLoseWindow = NULL;
// | }
// ALTERNATIVE word (unified source delta from BEFORE):
// | --- before
// | +++ word
// | @@ -1,6 +1,5 @@
// |  void combatManager::DoLoseWindow(void) {
// |      CombatResult losingSide;
// | -    i32 H2_UNUSED(unusedLoseWord) = IDX(COMBAT_RESULT_ATTACKER);
// |      tag_message message;
// |      char animationFile[WIN_LOSE_ANIMATION_FILENAME_SIZE];
// |
