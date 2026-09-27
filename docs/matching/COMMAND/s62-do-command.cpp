// VC6 SP5 COMMAND source family s62-do-command; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-do-command/results.json; RVA 0x2db4f
// Complete 8 arms; elapsed 6.718283s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 814 / bee3fed8894ca4a6 / 160818018ef86ec0 / 28:28 / 27,1,0,0 / True
// 1 / {'local_role_subset': 'army', 'candidate': 'baseline'} / 769 / 99fd5f8162899a5f / 27ccc8cd486d5029 / 28:28 / 26,2,0,0 / True
// 2 / {'local_role_subset': 'word5', 'candidate': 'baseline'} / 814 / cc689a352d7f2b1b / d735412f64ac883c / 28:28 / 27,1,0,0 / True
// 3 / {'local_role_subset': 'word5_army', 'candidate': 'baseline'} / 769 / 702771050f4909a1 / 95e06680b6468c09 / 28:28 / 26,2,0,0 / True
// 4 / {'local_role_subset': 'word2', 'candidate': 'baseline'} / 814 / cc689a352d7f2b1b / d735412f64ac883c / 28:28 / 27,1,0,0 / True
// 5 / {'local_role_subset': 'word2_army', 'candidate': 'baseline'} / 769 / 702771050f4909a1 / 95e06680b6468c09 / 28:28 / 26,2,0,0 / True
// 6 / {'local_role_subset': 'word2_word5', 'candidate': 'baseline'} / 814 / 83c5d566d6642e52 / 9853004104902ea0 / 28:28 / 27,1,0,0 / True
// 7 / {'local_role_subset': 'word2_word5_army', 'candidate': 'baseline'} / 769 / 5a8524c96cdd6d26 / 5e98810b37f0ca5b / 28:28 / 26,2,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | void combatManager::DoCommand(CombatMessageCommand command) {
// |     i32 H2_UNUSED(unusedCommandWord2);
// |     i32 H2_UNUSED(unusedCommandWord5);
// |     army* H2_UNUSED(currentArmy) = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |     switch (command) {
// |         case COMBAT_MESSAGE_COMMAND_DEFAULT:
// |             break;
// |         case COMBAT_MESSAGE_COMMAND_MOVE:
// |         case COMBAT_MESSAGE_COMMAND_FLY:
// |         case COMBAT_MESSAGE_COMMAND_SHOOT:
// |         case COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL:
// |             giNextAction = ACTION_MOVE;
// |             giNextActionGridIndex = m_selectedHex;
// |             giNextActionExtra = -1;
// |             break;
// |         case COMBAT_MESSAGE_COMMAND_ATTACK:
// |             giNextActionGridIndex = m_selectedHex;
// |             if (m_playerId[IDX(m_currentSide)] == -1
// |                 || gbHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0
// |                 || m_gridSelectionDisabled != 0) {
// |                 giNextAction = ACTION_MOVE;
// |                 giNextActionExtra = -1;
// |             } else {
// |                 giNextAction = ACTION_ATTACK;
// |                 giNextActionExtra = m_directionTargetHex;
// |             }
// |             break;
// |         case COMBAT_MESSAGE_COMMAND_OPTIONS:
// |             gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
// |             ViewGeneral(m_currentSide, 1, 0);
// |             ResetMouse();
// |             break;
// |         case COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS:
// |             gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
// |             ViewGeneral(OppositeCombatSide(m_currentSide), 1, 0);
// |             ResetMouse();
// |             break;
// |         case COMBAT_MESSAGE_COMMAND_VIEW_INFO:
// |             gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
// |             if (m_selectedHex == COMBAT_BALLISTA_HEX)
// |                 ViewBallista(0);
// |             else
// |                 ViewArmy(
// |                     &m_armies[IDX(m_hexCells[m_selectedHex].m_occupantSide)]
// |                              [m_hexCells[m_selectedHex].m_occupantIndex],
// |                     0
// |                 );
// |             ResetMouse();
// |             break;
// |         case COMBAT_MESSAGE_COMMAND_CAST_SPELL:
// |             if (IsNegationSphereInEffect() != 0) {
// |                 NormalDialog(
// |                     "\xd1\xf4\xe5\xf0\xe0 \xe0\xed\xf2\xe8\xec\xe0\xe3\xe8\xe8 \xe2 "
// |                     "\xe4\xe5\xe9\xf1\xf2\xe2\xe8\xe8. \xcd\xe5 \xec\xee\xe6\xe5\xf2 "
// |                     "\xe1\xfb\xf2\xfc \xed\xe0\xef\xf0\xe0\xe2\xeb\xe5\xed\xee \xed\xe8 "
// |                     "\xee\xe4\xed\xee \xe1\xee\xe5\xe2\xee\xe5 "
// |                     "\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5." /* "Сфера антимагии в действии. Не может быть направлено ни одно боевое заклинание." */
// |                     ,
// |                     NORMAL_DIALOG_INFO,
// |                     NORMAL_DIALOG_NO_RESOURCE,
// |                     NORMAL_DIALOG_NO_VALUE,
// |                     NORMAL_DIALOG_NO_RESOURCE,
// |                     0,
// |                     NORMAL_DIALOG_NO_RESOURCE,
// |                     0,
// |                     NORMAL_DIALOG_NO_RESOURCE,
// |                     0
// |                 );
// |                 break;
// |             }
// |             ViewSpells(0);
// |             ResetMouse();
// |             break;
// |         case COMBAT_MESSAGE_COMMAND_RETREAT:
// |             NormalDialog(
// |                 "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee "
// |                 "\xf5\xee\xf2\xe8\xf2\xe5 \xee\xf2\xf1\xf2\xf3\xef\xe8\xf2\xfc?" /* "Вы действительно хотите отступить?" */
// |                 ,
// |                 NORMAL_DIALOG_CONFIRM,
// |                 NORMAL_DIALOG_NO_RESOURCE,
// |                 NORMAL_DIALOG_NO_VALUE,
// |                 NORMAL_DIALOG_NO_RESOURCE,
// |                 0,
// |                 NORMAL_DIALOG_NO_RESOURCE,
// |                 0,
// |                 NORMAL_DIALOG_NO_RESOURCE,
// |                 0
// |             );
// |             if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE)
// |                 giNextAction = ACTION_RETREAT;
// |             ResetMouse();
// |             break;
// |         case COMBAT_MESSAGE_COMMAND_SURRENDER:
// |             if (DoSurrender() == 1) {
// |                 if (gpGame->m_players[m_playerId[IDX(m_currentSide)]].m_resources[IDX(RES_GOLD)]
// |                     < giSurrenderCost) {
// |                     NormalDialog(
// |                         "\xd3 \xe2\xe0\xf1 \xed\xe5\xe4\xee\xf1\xf2\xe0\xf2\xee\xf7\xed\xee "
// |                         "\xe7\xee\xeb\xee\xf2\xe0!" /* "У вас недостаточно золота!" */,
// |                         NORMAL_DIALOG_INFO,
// |                         NORMAL_DIALOG_NO_RESOURCE,
// |                         NORMAL_DIALOG_NO_VALUE,
// |                         NORMAL_DIALOG_NO_RESOURCE,
// |                         0,
// |                         NORMAL_DIALOG_NO_RESOURCE,
// |                         0,
// |                         NORMAL_DIALOG_NO_RESOURCE,
// |                         0
// |                     );
// |                 } else {
// |                     giNextAction = ACTION_SURRENDER;
// |                     giNextActionExtra = giSurrenderCost;
// |                 }
// |             }
// |             ResetMouse();
// |             break;
// |     }
// | }
// ALTERNATIVE army (unified source delta from BEFORE):
// | --- before
// | +++ army
// | @@ -1,7 +1,6 @@
// |  void combatManager::DoCommand(CombatMessageCommand command) {
// |      i32 H2_UNUSED(unusedCommandWord2);
// |      i32 H2_UNUSED(unusedCommandWord5);
// | -    army* H2_UNUSED(currentArmy) = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |      switch (command) {
// |          case COMBAT_MESSAGE_COMMAND_DEFAULT:
// |              break;
// ALTERNATIVE word5 (unified source delta from BEFORE):
// | --- before
// | +++ word5
// | @@ -1,6 +1,5 @@
// |  void combatManager::DoCommand(CombatMessageCommand command) {
// |      i32 H2_UNUSED(unusedCommandWord2);
// | -    i32 H2_UNUSED(unusedCommandWord5);
// |      army* H2_UNUSED(currentArmy) = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |      switch (command) {
// |          case COMBAT_MESSAGE_COMMAND_DEFAULT:
// ALTERNATIVE word5_army (unified source delta from BEFORE):
// | --- before
// | +++ word5_army
// | @@ -1,7 +1,5 @@
// |  void combatManager::DoCommand(CombatMessageCommand command) {
// |      i32 H2_UNUSED(unusedCommandWord2);
// | -    i32 H2_UNUSED(unusedCommandWord5);
// | -    army* H2_UNUSED(currentArmy) = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |      switch (command) {
// |          case COMBAT_MESSAGE_COMMAND_DEFAULT:
// |              break;
// ALTERNATIVE word2 (unified source delta from BEFORE):
// | --- before
// | +++ word2
// | @@ -1,5 +1,4 @@
// |  void combatManager::DoCommand(CombatMessageCommand command) {
// | -    i32 H2_UNUSED(unusedCommandWord2);
// |      i32 H2_UNUSED(unusedCommandWord5);
// |      army* H2_UNUSED(currentArmy) = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |      switch (command) {
// ALTERNATIVE word2_army (unified source delta from BEFORE):
// | --- before
// | +++ word2_army
// | @@ -1,7 +1,5 @@
// |  void combatManager::DoCommand(CombatMessageCommand command) {
// | -    i32 H2_UNUSED(unusedCommandWord2);
// |      i32 H2_UNUSED(unusedCommandWord5);
// | -    army* H2_UNUSED(currentArmy) = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |      switch (command) {
// |          case COMBAT_MESSAGE_COMMAND_DEFAULT:
// |              break;
// ALTERNATIVE word2_word5 (unified source delta from BEFORE):
// | --- before
// | +++ word2_word5
// | @@ -1,6 +1,4 @@
// |  void combatManager::DoCommand(CombatMessageCommand command) {
// | -    i32 H2_UNUSED(unusedCommandWord2);
// | -    i32 H2_UNUSED(unusedCommandWord5);
// |      army* H2_UNUSED(currentArmy) = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |      switch (command) {
// |          case COMBAT_MESSAGE_COMMAND_DEFAULT:
// ALTERNATIVE word2_word5_army (unified source delta from BEFORE):
// | --- before
// | +++ word2_word5_army
// | @@ -1,7 +1,4 @@
// |  void combatManager::DoCommand(CombatMessageCommand command) {
// | -    i32 H2_UNUSED(unusedCommandWord2);
// | -    i32 H2_UNUSED(unusedCommandWord5);
// | -    army* H2_UNUSED(currentArmy) = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |      switch (command) {
// |          case COMBAT_MESSAGE_COMMAND_DEFAULT:
// |              break;
