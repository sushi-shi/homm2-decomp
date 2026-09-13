// VC6 SP5 COMMAND source family s62-win-handler; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-win-handler/results.json; RVA 0x2de7d
// Complete 2 arms; elapsed 2.307495s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 1267 / e39b7938cb338175 / 5f4350f4e043df12 / 51:51 / 51,0,0,0 / True
// 1 / {'local_role_subset': 'delay', 'candidate': 'baseline'} / 1260 / 09ddc93466cc9923 / 4f550577137e4953 / 51:51 / 50,1,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | MessageDispatchResult WinCombatHandler(struct tag_message& message) {
// |     char iconFile_3[WIN_LOSE_ICON_FILENAME_SIZE];
// |     tag_message animationMessage;
// |     i32 H2_UNUSED(finalDelay) = WIN_LOSE_INITIAL_DELAY;
// |     i32 frame;
// |     i32 iDelay_3;
// |
// |     if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
// |         message.type = MESSAGE_WIDGET;
// |         gpWindowManager->m_dialogResult = message.payload.widget.id;
// |         message.payload.widget.id = WIN_LOSE_CLOSE_COMMAND;
// |         message.payload.widget.command = BaseWidgetCommand(WIN_LOSE_CLOSE_COMMAND);
// |         giDialogTimeout = 0;
// |         return MESSAGE_DISPATCH_FORWARD;
// |     }
// |
// |     if (message.type == MESSAGE_WIDGET) {
// |         switch (message.payload.widget.command) {
// |             case WIDGET_COMMAND_DESELECT:
// |                 switch (message.payload.widget.id) {
// |                     case WIN_LOSE_NEXT_CONTROL:
// |                         if (gbShowingLoseWindow != 0)
// |                             goto ExitDialog;
// |                         if (iCurTransferArtifact + 1 < iMaxTransferArtifacts) {
// |                             gpCombatManager->ClearWinLoseBottom(gpCombatManager->m_winLoseWindow);
// |                             ++iCurTransferArtifact;
// |                             gpCombatManager->ShowWinLoseArtifact(
// |                                 gpCombatManager->m_winLoseWindow,
// |                                 iTransferArtifacts[iCurTransferArtifact]
// |                             );
// |                         } else if (giSkeletonsCreated != 0 && bSkeletonsShown == 0) {
// |                             bSkeletonsShown = true;
// |                             gpCombatManager->ClearWinLoseBottom(gpCombatManager->m_winLoseWindow);
// |                             gpCombatManager->ShowSkeletons(gpCombatManager->m_winLoseWindow);
// |                         } else {
// |                             if (gpCombatManager
// |                                     ->m_eagleEyeSpell[IDX(gpCombatManager->m_combatResult)]
// |                                 != SPELL_NONE) {
// |                                 gpCombatManager->ClearWinLoseBottom(
// |                                     gpCombatManager->m_winLoseWindow
// |                                 );
// |                                 gpCombatManager->ShowEagleEyeSpell(
// |                                     gpCombatManager->m_winLoseWindow
// |                                 );
// |                                 gpCombatManager
// |                                     ->m_eagleEyeSpell[IDX(gpCombatManager->m_combatResult)] =
// |                                     SPELL_NONE;
// |                             } else {
// |                             ExitDialog:
// |                                 gpWindowManager->m_dialogResult = message.payload.widget.id;
// |                                 message.payload.widget.id = WIN_LOSE_CLOSE_COMMAND;
// |                                 message.payload.widget.command =
// |                                     BaseWidgetCommand(WIN_LOSE_CLOSE_COMMAND);
// |                                 return MESSAGE_DISPATCH_FORWARD;
// |                             }
// |                         }
// |                         break;
// |                     default:
// |                         break;
// |                 }
// |                 break;
// |             default:
// |                 break;
// |         }
// |     }
// |
// |     if (glTimers[0] < KBTickCount()) {
// |         animationMessage.type = MESSAGE_WIDGET;
// |         animationMessage.payload.widget.command = COMBAT_WIN_LOSE_RESOURCE_COMMAND;
// |         animationMessage.payload.widget.data.text = iconFile_3;
// |         ++giWinCmbtFrame;
// |
// |         switch (gbWhichAnimationPlaying) {
// |             case WIN_LOSE_ANIMATION_CYCLE_FIRST:
// |                 frame = giWinCmbtFrame % WIN_LOSE_LOOP_FRAME_COUNT + WIN_LOSE_FIRST_ANIMATION_FRAME;
// |                 iDelay_3 = WIN_LOSE_LOOP_DELAY;
// |                 break;
// |             case WIN_LOSE_ANIMATION_CYCLE_SECOND:
// |                 frame = giWinCmbtFrame % WIN_LOSE_LOOP_FRAME_COUNT + WIN_LOSE_FIRST_ANIMATION_FRAME;
// |                 iDelay_3 = WIN_LOSE_LOOP_DELAY;
// |                 break;
// |             case WIN_LOSE_ANIMATION_FLEE:
// |                 if (giWinCmbtFrame == WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME) {
// |                     sprintf(iconFile_3, "cmbtfle2.icn");
// |                     animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
// |                     gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
// |                     animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
// |                     gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
// |                 }
// |                 if (giWinCmbtFrame == WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME) {
// |                     sprintf(iconFile_3, "cmbtfle3.icn");
// |                     animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
// |                     gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
// |                     animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
// |                     gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
// |                 }
// |                 if (giWinCmbtFrame < WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME) {
// |                     frame = giWinCmbtFrame + 1;
// |                 } else if (giWinCmbtFrame < WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME) {
// |                     frame = giWinCmbtFrame - (WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME - 1);
// |                 } else {
// |                     frame = giWinCmbtFrame - (WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME - 1);
// |                     if (frame > WIN_LOSE_FLEE_LAST_FRAME)
// |                         frame = WIN_LOSE_FLEE_LAST_FRAME;
// |                 }
// |                 iDelay_3 = WIN_LOSE_FLEE_DELAY;
// |                 break;
// |             default:
// |                 if (giWinCmbtFrame == WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME) {
// |                     sprintf(iconFile_3, "cmbtlos2.icn");
// |                     animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
// |                     gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
// |                     animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
// |                     gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
// |                 }
// |                 if (giWinCmbtFrame == WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME) {
// |                     sprintf(iconFile_3, "cmbtlos3.icn");
// |                     animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
// |                     gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
// |                     animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
// |                     gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
// |                 }
// |                 if (giWinCmbtFrame < WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME) {
// |                     frame = giWinCmbtFrame + 1;
// |                     iDelay_3 = WIN_LOSE_LOSS_DELAY;
// |                 } else if (giWinCmbtFrame < WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME) {
// |                     frame = giWinCmbtFrame - (WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME - 1);
// |                     iDelay_3 = WIN_LOSE_LOSS_DELAY;
// |                 } else {
// |                     if (giWinCmbtFrame == WIN_LOSE_LOSS_RESET_FRAME)
// |                         giWinCmbtFrame = WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME;
// |                     frame = giWinCmbtFrame - (WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME - 1);
// |                     iDelay_3 = WIN_LOSE_LOSS_FINAL_DELAY;
// |                 }
// |                 break;
// |         }
// |
// |         message.type = MESSAGE_WIDGET;
// |         message.payload.widget.command = COMBAT_WIN_LOSE_ANIMATION_COMMAND;
// |         message.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
// |         message.payload.widget.data.value = frame;
// |         gpCombatManager->m_winLoseWindow->BroadcastMessage(message);
// |         gpCombatManager->m_winLoseWindow->DrawWindow(1, 0, WIN_LOSE_DRAW_DEPTH);
// |         glTimers[0] = KBTickCount() + iDelay_3;
// |     }
// |     return MESSAGE_DISPATCH_CONSUME;
// | }
// ALTERNATIVE delay (unified source delta from BEFORE):
// | --- before
// | +++ delay
// | @@ -1,7 +1,6 @@
// |  MessageDispatchResult WinCombatHandler(struct tag_message& message) {
// |      char iconFile_3[WIN_LOSE_ICON_FILENAME_SIZE];
// |      tag_message animationMessage;
// | -    i32 H2_UNUSED(finalDelay) = WIN_LOSE_INITIAL_DELAY;
// |      i32 frame;
// |      i32 iDelay_3;
// |
