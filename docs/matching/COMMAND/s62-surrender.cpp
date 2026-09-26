// VC6 SP5 COMMAND source family s62-surrender; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-surrender/results.json; RVA 0x3049b
// Complete 8 arms; elapsed 6.50428s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 957 / 77258fa625f3d2e8 / 69148334ec93a97c / 24:24 / 24,0,0,0 / True
// 1 / {'local_role_subset': 'width', 'candidate': 'baseline'} / 951 / 97009bb332d64866 / ea7c4974ce17d89a / 24:24 / 23,1,0,0 / True
// 2 / {'local_role_subset': 'result', 'candidate': 'baseline'} / 951 / 2cffd68be74210cc / ea7c4974ce17d89a / 24:24 / 23,1,0,0 / True
// 3 / {'local_role_subset': 'result_width', 'candidate': 'baseline'} / 945 / 89457557d8d3dc0d / 0bfa7090b71aae76 / 24:24 / 23,1,0,0 / True
// 4 / {'local_role_subset': 'type', 'candidate': 'baseline'} / 951 / a6b6cf1792480553 / ea7c4974ce17d89a / 24:24 / 23,1,0,0 / True
// 5 / {'local_role_subset': 'type_width', 'candidate': 'baseline'} / 945 / fe48676515f0d05a / 0bfa7090b71aae76 / 24:24 / 23,1,0,0 / True
// 6 / {'local_role_subset': 'type_result', 'candidate': 'baseline'} / 945 / c62215f1a0f95fd3 / 0bfa7090b71aae76 / 24:24 / 23,1,0,0 / True
// 7 / {'local_role_subset': 'type_result_width', 'candidate': 'baseline'} / 939 / 081c17fcc8491c99 / c03ae3adbb7843ad / 24:24 / 23,1,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | i32 combatManager::DoSurrender(void) {
// |     i32 armyNum;
// |     i16 H2_UNUSED(dlgType);
// |     i16 H2_UNUSED(dlgResult);
// |     i16 H2_UNUSED(lineWidth);
// |     heroWindow* window;
// |     tag_message message;
// |
// |     giSurrenderCost = 0;
// |     for (armyNum = 0; armyNum < COMBAT_ARMY_SLOT_COUNT; ++armyNum) {
// |         if (m_armies[IDX(m_currentSide)][armyNum].IsAlive()) {
// |             giSurrenderCost += m_armies[IDX(m_currentSide)][armyNum].m_quantity
// |                 * gMonsterDatabase[IDX(m_armies[IDX(m_currentSide)][armyNum].m_monsterType)]
// |                       .cost;
// |         }
// |     }
// |     if (m_heroes[IDX(m_currentSide)]->HasArtifact(ARTIFACT_STATESMANS_QUILL) != 0)
// |         giSurrenderCost = static_cast<i32>(giSurrenderCost * COMBAT_SURRENDER_QUILL_FACTOR);
// |     else
// |         giSurrenderCost = static_cast<i32>(giSurrenderCost * COMBAT_SURRENDER_BASE_FACTOR);
// |     giSurrenderCost = static_cast<i32>(
// |         giSurrenderCost
// |         * (1.0
// |            - IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)])
// |                  * COMBAT_SURRENDER_DIPLOMACY_FACTOR)
// |     );
// |
// |     dlgType = SURRENDER_DIALOG_TYPE;
// |     dlgResult = SURRENDER_DIALOG_ACCEPT_RESULT;
// |     lineWidth = SURRENDER_TEXT_WIDTH;
// |     window = new heroWindow(SURRENDER_WINDOW_X, SURRENDER_WINDOW_Y, "surrendr.bin");
// |     if (window == NULL)
// |         MemError();
// |     message.type = MESSAGE_WIDGET;
// |     message.payload.widget.command = COMBAT_WIN_LOSE_RESOURCE_COMMAND;
// |     message.payload.widget.id = SURRENDER_PORTRAIT_RESOURCE_ID;
// |     sprintf(
// |         gText,
// |         "port%04d.icn",
// |         static_cast<i32>(m_heroes[IDX(OppositeCombatSide(m_currentSide))]->m_portrait)
// |     );
// |     message.payload.widget.data.text = gText;
// |     window->BroadcastMessage(message);
// |     message.payload.widget.command =
// |         m_heroes[IDX(OppositeCombatSide(m_currentSide))]->m_isCaptain
// |         ? COMBAT_SURRENDER_CAPTAIN_PORTRAIT_COMMAND
// |         : COMBAT_SURRENDER_HERO_PORTRAIT_COMMAND;
// |     message.payload.widget.id = SURRENDER_PORTRAIT_WIDGET_ID;
// |     message.payload.widget.data.value = SURRENDER_PORTRAIT_DEFAULT_COLOR;
// |     window->BroadcastMessage(message);
// |     if (m_heroes[IDX(OppositeCombatSide(m_currentSide))]->m_isCaptain != 0) {
// |         message.payload.widget.command = COMBAT_SURRENDER_CAPTAIN_OVERLAY_COMMAND;
// |         message.payload.widget.data.value =
// |             m_playerId[IDX(OppositeCombatSide(m_currentSide))] == -1
// |             ? COMBAT_NEUTRAL_HERO_COLOR
// |             : gpGame->m_players[m_playerId[IDX(OppositeCombatSide(m_currentSide))]].m_color;
// |         window->BroadcastMessage(message);
// |     }
// |     message.payload.widget.data.text = gText;
// |     message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
// |     message.payload.widget.id = SURRENDER_TEXT_ID;
// |     sprintf(
// |         gText,
// |         "%s \xe3\xee\xe2\xee\xf0\xe8\xf2:\n\n\"\xdf \xef\xf0\xe8\xec\xf3 \xe2\xe0\xf8\xf3 "
// |             "\xea\xe0\xef\xe8\xf2\xf3\xeb\xff\xf6\xe8\xfe \xe8 \xef\xee\xe7\xe2\xee\xeb\xfe \xe2\xe0\xec \xe8 "
// |             "\xe2\xe0\xf8\xe8\xec \xe2\xee\xe8\xed\xe0\xec \xf3\xe9\xf2\xe8 \xf1 \xec\xe8\xf0\xee\xec \xe7\xe0 "
// |             "%d \xe7\xee\xeb\xee\xf2\xfb\xf5.\""
// |             /* "%s говорит:\n\n\"Я приму вашу капитуляцию и позволю вам и вашим воинам уйти с миром за %d золотых.\"" */,
// |         m_heroes[IDX(OppositeCombatSide(m_currentSide))]->m_name,
// |         giSurrenderCost
// |     );
// |     window->BroadcastMessage(message);
// |     gpWindowManager->DoDialog(window, TrueFalseDialogHandler, 0);
// |     delete window;
// |     return gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_TWO;
// | }
// ALTERNATIVE width (unified source delta from BEFORE):
// | --- before
// | +++ width
// | @@ -2,7 +2,6 @@
// |      i32 armyNum;
// |      i16 H2_UNUSED(dlgType);
// |      i16 H2_UNUSED(dlgResult);
// | -    i16 H2_UNUSED(lineWidth);
// |      heroWindow* window;
// |      tag_message message;
// |
// | @@ -27,7 +26,6 @@
// |
// |      dlgType = SURRENDER_DIALOG_TYPE;
// |      dlgResult = SURRENDER_DIALOG_ACCEPT_RESULT;
// | -    lineWidth = SURRENDER_TEXT_WIDTH;
// |      window = new heroWindow(SURRENDER_WINDOW_X, SURRENDER_WINDOW_Y, "surrendr.bin");
// |      if (window == NULL)
// |          MemError();
// ALTERNATIVE result (unified source delta from BEFORE):
// | --- before
// | +++ result
// | @@ -1,7 +1,6 @@
// |  i32 combatManager::DoSurrender(void) {
// |      i32 armyNum;
// |      i16 H2_UNUSED(dlgType);
// | -    i16 H2_UNUSED(dlgResult);
// |      i16 H2_UNUSED(lineWidth);
// |      heroWindow* window;
// |      tag_message message;
// | @@ -26,7 +25,6 @@
// |      );
// |
// |      dlgType = SURRENDER_DIALOG_TYPE;
// | -    dlgResult = SURRENDER_DIALOG_ACCEPT_RESULT;
// |      lineWidth = SURRENDER_TEXT_WIDTH;
// |      window = new heroWindow(SURRENDER_WINDOW_X, SURRENDER_WINDOW_Y, "surrendr.bin");
// |      if (window == NULL)
// ALTERNATIVE result_width (unified source delta from BEFORE):
// | --- before
// | +++ result_width
// | @@ -1,8 +1,6 @@
// |  i32 combatManager::DoSurrender(void) {
// |      i32 armyNum;
// |      i16 H2_UNUSED(dlgType);
// | -    i16 H2_UNUSED(dlgResult);
// | -    i16 H2_UNUSED(lineWidth);
// |      heroWindow* window;
// |      tag_message message;
// |
// | @@ -26,8 +24,6 @@
// |      );
// |
// |      dlgType = SURRENDER_DIALOG_TYPE;
// | -    dlgResult = SURRENDER_DIALOG_ACCEPT_RESULT;
// | -    lineWidth = SURRENDER_TEXT_WIDTH;
// |      window = new heroWindow(SURRENDER_WINDOW_X, SURRENDER_WINDOW_Y, "surrendr.bin");
// |      if (window == NULL)
// |          MemError();
// ALTERNATIVE type (unified source delta from BEFORE):
// | --- before
// | +++ type
// | @@ -1,6 +1,5 @@
// |  i32 combatManager::DoSurrender(void) {
// |      i32 armyNum;
// | -    i16 H2_UNUSED(dlgType);
// |      i16 H2_UNUSED(dlgResult);
// |      i16 H2_UNUSED(lineWidth);
// |      heroWindow* window;
// | @@ -25,7 +24,6 @@
// |                   * COMBAT_SURRENDER_DIPLOMACY_FACTOR)
// |      );
// |
// | -    dlgType = SURRENDER_DIALOG_TYPE;
// |      dlgResult = SURRENDER_DIALOG_ACCEPT_RESULT;
// |      lineWidth = SURRENDER_TEXT_WIDTH;
// |      window = new heroWindow(SURRENDER_WINDOW_X, SURRENDER_WINDOW_Y, "surrendr.bin");
// ALTERNATIVE type_width (unified source delta from BEFORE):
// | --- before
// | +++ type_width
// | @@ -1,8 +1,6 @@
// |  i32 combatManager::DoSurrender(void) {
// |      i32 armyNum;
// | -    i16 H2_UNUSED(dlgType);
// |      i16 H2_UNUSED(dlgResult);
// | -    i16 H2_UNUSED(lineWidth);
// |      heroWindow* window;
// |      tag_message message;
// |
// | @@ -25,9 +23,7 @@
// |                   * COMBAT_SURRENDER_DIPLOMACY_FACTOR)
// |      );
// |
// | -    dlgType = SURRENDER_DIALOG_TYPE;
// |      dlgResult = SURRENDER_DIALOG_ACCEPT_RESULT;
// | -    lineWidth = SURRENDER_TEXT_WIDTH;
// |      window = new heroWindow(SURRENDER_WINDOW_X, SURRENDER_WINDOW_Y, "surrendr.bin");
// |      if (window == NULL)
// |          MemError();
// ALTERNATIVE type_result (unified source delta from BEFORE):
// | --- before
// | +++ type_result
// | @@ -1,7 +1,5 @@
// |  i32 combatManager::DoSurrender(void) {
// |      i32 armyNum;
// | -    i16 H2_UNUSED(dlgType);
// | -    i16 H2_UNUSED(dlgResult);
// |      i16 H2_UNUSED(lineWidth);
// |      heroWindow* window;
// |      tag_message message;
// | @@ -25,8 +23,6 @@
// |                   * COMBAT_SURRENDER_DIPLOMACY_FACTOR)
// |      );
// |
// | -    dlgType = SURRENDER_DIALOG_TYPE;
// | -    dlgResult = SURRENDER_DIALOG_ACCEPT_RESULT;
// |      lineWidth = SURRENDER_TEXT_WIDTH;
// |      window = new heroWindow(SURRENDER_WINDOW_X, SURRENDER_WINDOW_Y, "surrendr.bin");
// |      if (window == NULL)
// ALTERNATIVE type_result_width (unified source delta from BEFORE):
// | --- before
// | +++ type_result_width
// | @@ -1,8 +1,5 @@
// |  i32 combatManager::DoSurrender(void) {
// |      i32 armyNum;
// | -    i16 H2_UNUSED(dlgType);
// | -    i16 H2_UNUSED(dlgResult);
// | -    i16 H2_UNUSED(lineWidth);
// |      heroWindow* window;
// |      tag_message message;
// |
// | @@ -25,9 +22,6 @@
// |                   * COMBAT_SURRENDER_DIPLOMACY_FACTOR)
// |      );
// |
// | -    dlgType = SURRENDER_DIALOG_TYPE;
// | -    dlgResult = SURRENDER_DIALOG_ACCEPT_RESULT;
// | -    lineWidth = SURRENDER_TEXT_WIDTH;
// |      window = new heroWindow(SURRENDER_WINDOW_X, SURRENDER_WINDOW_Y, "surrendr.bin");
// |      if (window == NULL)
// |          MemError();
