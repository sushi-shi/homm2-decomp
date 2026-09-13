// VC6 SP5 COMMAND source family s62-process-message; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-process-message/results.json; RVA 0x2c958
// Complete 2 arms; elapsed 2.264083s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 2037 / 466763d9e73aced8 / f826751720c01472 / 93:94 / 92,0,0,1 / False
// 1 / {'local_role_subset': 'result', 'candidate': 'baseline'} / 2030 / a4f1ccaac60879bd / 836ca5cb0fd7b23a / 93:94 / 91,1,0,1 / False

// AXIS local_role_subset
// BEFORE (complete exact span):
// | MessageDispatchResult combatManager::ProcessCombatMsg(tag_message& message) {
// |     i32 mouseX = message.payload.mouse.screenX;
// |     i32 mouseY = message.payload.mouse.screenY;
// |     i32 H2_UNUSED(unusedResult) = 0;
// |     i32 selectedHex_0;
// |     tag_message pendingMessage_2;
// |
// |     switch (message.type) {
// |         case MESSAGE_WIDGET:
// |             if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) {
// |                 if (message.payload.widget.command == WIDGET_COMMAND_SELECT
// |                     || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT) {
// |                     i32 helpIndex = -1;
// |                     switch (static_cast<CombatControlId>(message.payload.widget.id)) {
// |                         case CONTROL_MAIN_BUTTON:
// |                             RightClick(m_selectedHex);
// |                             break;
// |                         case CONTROL_ATTACK:
// |                             helpIndex = IDX(LONG_HELP_ATTACK);
// |                             break;
// |                         case CONTROL_WAIT:
// |                             helpIndex = IDX(LONG_HELP_WAIT);
// |                             break;
// |                         case CONTROL_DISABLE_SELECTION:
// |                             helpIndex = IDX(LONG_HELP_DISABLE_SELECTION);
// |                             break;
// |                         case CONTROL_SYSTEM_OPTIONS:
// |                             helpIndex = IDX(LONG_HELP_SYSTEM_OPTIONS);
// |                             break;
// |                         case CONTROL_HELP_FIRST:
// |                         case CONTROL_HELP_SECOND:
// |                         case CONTROL_HELP_THIRD:
// |                         case CONTROL_HELP_FOURTH:
// |                             helpIndex = IDX(LONG_HELP_CONTROLS);
// |                             break;
// |                     }
// |                     if (helpIndex != -1) {
// |                         NormalDialog(
// |                             cLongCombatHelp[helpIndex],
// |                             NORMAL_DIALOG_QUICK_VIEW,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             NORMAL_DIALOG_NO_VALUE,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             0,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             0,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             0
// |                         );
// |                     }
// |                 }
// |                 break;
// |             }
// |             switch (message.payload.widget.command) {
// |                 case WIDGET_COMMAND_SELECT:
// |                     switch (static_cast<CombatControlId>(message.payload.widget.id)) {
// |                         case CONTROL_MAIN_BUTTON:
// |                             DoCommand(m_currentCommand);
// |                             break;
// |                     }
// |                     break;
// |                 case WIDGET_COMMAND_DESELECT:
// |                     switch (static_cast<CombatControlId>(message.payload.widget.id)) {
// |                         case CONTROL_DISABLE_SELECTION:
// |                             m_gridSelectionDisabled = true;
// |                             break;
// |                         case CONTROL_WAIT:
// |                             giNextAction = ACTION_WAIT;
// |                             break;
// |                         case CONTROL_ATTACK:
// |                             giNextAction = ACTION_DEFEND;
// |                             break;
// |                         case CONTROL_SYSTEM_OPTIONS:
// |                             CombatSystemOptions();
// |                             break;
// |                     }
// |                     break;
// |             }
// |             break;
// |
// |         case MESSAGE_MOUSE_MOVE:
// |             if (m_gridSelectionDisabled != 0)
// |                 break;
// |             pendingMessage_2 = gpInputManager->PeekEvent();
// |             if (pendingMessage_2.type == MESSAGE_MOUSE_MOVE)
// |                 break;
// |             if (InCombatArea(message.payload.mouse.screenX, message.payload.mouse.screenY) != 0)
// |                 selectedHex_0 = GetGridIndex(mouseX, mouseY);
// |             else
// |                 selectedHex_0 = INVALID_HEX;
// |
// |             UpdateMouseGrid(selectedHex_0, 0);
// |             if (InCombatArea(message.payload.mouse.screenX, message.payload.mouse.screenY) != 0) {
// |                 if (selectedHex_0 != m_selectedHex || selectedHex_0 == INVALID_HEX) {
// |                     m_selectedHex = selectedHex_0;
// |                     m_previousCommand = COMBAT_INVALID_COMMAND;
// |                     m_currentCommand = GetCommand(m_selectedHex);
// |                     m_mouseDirection = INVALID_HEX;
// |                     if (m_currentCommand == COMBAT_MESSAGE_COMMAND_ATTACK) {
// |                         SetCombatDirections(selectedHex_0);
// |                         CheckSetMouseDirection(mouseX, mouseY, selectedHex_0);
// |                     } else {
// |                         gpMouseManager->SetPointer(GetPointer(m_currentCommand, selectedHex_0));
// |                     }
// |                 } else if (m_currentCommand == COMBAT_MESSAGE_COMMAND_ATTACK) {
// |                     CheckSetMouseDirection(mouseX, mouseY, selectedHex_0);
// |                 }
// |                 if (m_currentCommand != m_previousCommand) {
// |                     m_previousCommand = m_currentCommand;
// |                     CombatMessage(m_currentCommand);
// |                 }
// |             } else {
// |                 if (mouseX >= CONTROL_RIGHT_MIN_X) {
// |                     CombatMessage(cCombatHelp[IDX(HELP_SKIP_UNIT)], 1, 0, 0);
// |                 } else if (mouseX <= CONTROL_LEFT_MAX_X && mouseY < CONTROL_SYSTEM_OPTIONS_MIN_Y) {
// |                     CombatMessage(cCombatHelp[IDX(HELP_AUTO_COMBAT)], 1, 0, 0);
// |                 } else if (mouseX <= CONTROL_LEFT_MAX_X) {
// |                     CombatMessage(cCombatHelp[IDX(HELP_SYSTEM_OPTIONS)], 1, 0, 0);
// |                 } else {
// |                     CombatMessage(cCombatHelp[IDX(HELP_OTHER_CONTROL)], 1, 0, 0);
// |                 }
// |                 gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
// |                 m_selectedHex = INVALID_HEX;
// |                 m_previousCommand = COMBAT_INVALID_COMMAND;
// |             }
// |             return MESSAGE_DISPATCH_CONSUME;
// |
// |         case MESSAGE_KEY_DOWN:
// |             switch (static_cast<CombatKeyCommand>(message.payload.keyboard.keyCode)) {
// |                 case KEY_CLOSE_NETWORK_BOX:
// |                     PopNetBox(NULL, -1);
// |                     break;
// |                 case KEY_REDRAW_SCREEN:
// |                     gpWindowManager->UpdateScreenRegion(
// |                         0,
// |                         0,
// |                         COMBAT_SCREEN_WIDTH - 1,
// |                         COMBAT_SCREEN_HEIGHT - 1
// |                     );
// |                     break;
// |                 case KEY_CYCLE_ARMY_VIEW:
// |                     SetCombatViewArmySmallLevel(
// |                         (gConfig.combatArmyInfoLevel + 1) % ARMY_VIEW_LEVEL_COUNT
// |                     );
// |                     break;
// |                 case KEY_TOGGLE_GRID:
// |                     SetCombatGrid(
// |                         1 - gConfig.showCombatGrid,
// |                         gConfig.showCombatMouseHex,
// |                         gConfig.combatShadeLevel
// |                     );
// |                     break;
// |                 case KEY_TOGGLE_MOUSE_HEX:
// |                     SetCombatGrid(
// |                         gConfig.showCombatGrid,
// |                         1 - gConfig.showCombatMouseHex,
// |                         gConfig.combatShadeLevel
// |                     );
// |                     break;
// |                 case KEY_CYCLE_SHADE:
// |                     SetCombatGrid(
// |                         gConfig.showCombatGrid,
// |                         gConfig.showCombatMouseHex,
// |                         1 - gConfig.combatShadeLevel
// |                     );
// |                     break;
// |                 case KEY_CYCLE_SPELL_EFFECT:
// |                     giSpellEffectShowType = NextSpellEffectDisplayType(giSpellEffectShowType);
// |                     DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
// |                     break;
// |                 case KEY_DEBUG_CREATURE_EFFECT:
// |                     if (HAS(message.payload.keyboard.modifiers, MESSAGE_MODIFIER_SHIFT_KEYS)) {
// |                         VaporizeCreature(COMBAT_DEFENDER_SIDE, 1);
// |                     } else if (HAS(message.payload.keyboard.modifiers,
// |                                    MESSAGE_MODIFIER_CONTROL_KEYS)) {
// |                         RippleCreature(COMBAT_DEFENDER_SIDE, 1, COMBAT_RIPPLE_DEATH_RIPPLE);
// |                         RippleCreature(COMBAT_DEFENDER_SIDE, 1, COMBAT_RIPPLE_DEATH_WAVE);
// |                     } else {
// |                         RippleCreature(COMBAT_DEFENDER_SIDE, 1, COMBAT_RIPPLE_WAVE);
// |                     }
// |                     break;
// |                 case KEY_WAIT:
// |                     giNextAction = ACTION_WAIT;
// |                     break;
// |                 case KEY_MOUSE_COORDS: {
// |                     i32 currentMouseX_18;
// |                     i32 currentMouseY_18;
// |                     gpMouseManager->MouseCoords(currentMouseX_18, currentMouseY_18);
// |                     break;
// |                 }
// |                 case KEY_VIEW_GENERAL:
// |                     if (m_heroes[IDX(m_currentSide)] != NULL) {
// |                         gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
// |                         ViewGeneral(m_currentSide, 1, 0);
// |                         ResetMouse();
// |                     }
// |                     break;
// |                 case KEY_VIEW_ARMY:
// |                     gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
// |                     ViewArmy(&m_armies[IDX(m_currentArmySide)][m_currentArmyIndex], 0);
// |                     ResetMouse();
// |                     break;
// |                 case KEY_CAST_SPELL:
// |                     if (m_heroes[IDX(m_currentSide)] == NULL) {
// |                         NormalDialog(
// |                             "\xd3 \xe2\xe0\xf1 \xed\xe5\xf2 \xe3\xe5\xf0\xee\xe5\xe2, \xf7\xf2\xee\xe1\xfb "
// |                                 "\xed\xe0\xef\xf0\xe0\xe2\xeb\xff\xf2\xfc \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff."
// |                                 /* "У вас нет героев, чтобы направлять заклинания." */,
// |                             NORMAL_DIALOG_INFO,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             NORMAL_DIALOG_NO_VALUE,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             0,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             0,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             0
// |                         );
// |                     } else if (IsNegationSphereInEffect() != 0) {
// |                         NormalDialog(
// |                             "\xd1\xf4\xe5\xf0\xe0 \xe0\xed\xf2\xe8\xec\xe0\xe3\xe8\xe8 \xe2 "
// |                             "\xe4\xe5\xe9\xf1\xf2\xe2\xe8\xe8. \xcd\xe5 \xec\xee\xe6\xe5\xf2 "
// |                             "\xe1\xfb\xf2\xfc \xed\xe0\xef\xf0\xe0\xe2\xeb\xe5\xed\xee \xed\xe8 "
// |                             "\xee\xe4\xed\xee \xe1\xee\xe5\xe2\xee\xe5 "
// |                             "\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5." /* "Сфера антимагии в действии. Не может быть направлено ни одно боевое заклинание." */
// |                             ,
// |                             NORMAL_DIALOG_INFO,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             NORMAL_DIALOG_NO_VALUE,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             0,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             0,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             0
// |                         );
// |                     } else if (m_heroCastSpell[IDX(m_currentSide)] != 0 && giDebugLevel == 0) {
// |                         NormalDialog(
// |                             "\xc2\xfb \xf3\xe6\xe5 \xed\xe0\xef\xf0\xe0\xe2\xeb\xff\xeb\xe8 \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff \xe2 \xfd\xf2\xee\xec \xf0\xe0\xf3\xed\xe4\xe5.",
// |                             NORMAL_DIALOG_INFO,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             NORMAL_DIALOG_NO_VALUE,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             0,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             0,
// |                             NORMAL_DIALOG_NO_RESOURCE,
// |                             0
// |                         );
// |                     } else {
// |                         gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
// |                         giCurGeneral = m_currentSide;
// |                         ViewSpells(0);
// |                         ResetMouse();
// |                     }
// |                     break;
// |             }
// |             break;
// |     }
// |
// |     return MESSAGE_DISPATCH_CONSUME;
// | }
// ALTERNATIVE result (unified source delta from BEFORE):
// | --- before
// | +++ result
// | @@ -1,7 +1,6 @@
// |  MessageDispatchResult combatManager::ProcessCombatMsg(tag_message& message) {
// |      i32 mouseX = message.payload.mouse.screenX;
// |      i32 mouseY = message.payload.mouse.screenY;
// | -    i32 H2_UNUSED(unusedResult) = 0;
// |      i32 selectedHex_0;
// |      tag_message pendingMessage_2;
// |
