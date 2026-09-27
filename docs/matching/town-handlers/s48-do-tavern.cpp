// Parent 7d4ffd56; 2 complete arms in 2.244278797879815 seconds.
// Selected trial 0: {'source_shape': 'omit0', 'candidate': 'baseline'}; size/text/label-sensitive reloc signature (299, '64a74b71ea63b066', 'b3b7534207d072bf').
// Same size and masked payload as baseline: ['omit0'].
// All arms compiled; source restored; no generator/wall truncation, AST mutation or TU-state census.
// Only selected combined source has all-98-object resolved native equivalence.
// Private counter-label hashes are not resolved-target proof; five existing retail EH/site gaps remain.
// Artifacts: build/s48-do-tavern-manifest.json; build/source-variant-batch/s48-do-tavern/results.json.
// See docs/reconstruction/C46-R10-S48-G56.md for exact per-function dispositions.
// Comment-only archive: strip the leading // and optional one space to recover the text below.
//
// Baseline:
// void townManager::DoTavern(void) {
//     i32 H2_UNUSED(unusedValue) = 0;
//     tag_message message;
//
//     m_heroWindow0 = new heroWindow(TAVERN_WINDOW_X, TAVERN_WINDOW_Y, "tavwin.bin");
//     if (m_heroWindow0 == NULL)
//         MemError();
//     SetWinText(m_heroWindow0, TAVERN_WINDOW_TEXT_ID);
//     sprintf(
//         gText,
//         "\xd9\xe5\xe4\xf0\xfb\xe5 \xf7\xe0\xe5\xe2\xfb\xe5 \xe2\xfb\xf2\xff\xed\xf3\xeb\xe8 "
//         "\xe8\xe7 \xf3\xf1\xf2 \xf2\xf0\xe0\xea\xf2\xe8\xf0\xf9\xe8\xea\xe0 "
//         "\xf1\xeb\xe5\xe4\xf3\xfe\xf9\xe8\xe9 \xf1\xeb\xf3\xf5:\n\n%s" /* "Щедрые чаевые вытянули из уст трактирщика следующий слух:
//
// %s" */
//         ,
//         gpGame->m_rumour
//     );
//     message.type = MESSAGE_WIDGET;
//     message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
//     message.payload.widget.id = TOWN_TAVERN_RUMOUR_CONTROL;
//     message.payload.widget.data.text = gText;
//     m_heroWindow0->BroadcastMessage(message);
//     gpWindowManager->DoDialog(m_heroWindow0, TavernHandler, 0);
//     delete m_heroWindow0;
// }
//
// Option omit0:
//
//
// Option omit1:
// --- baseline
// +++ omit1
// @@ -1,5 +1,4 @@
//  void townManager::DoTavern(void) {
// -    i32 H2_UNUSED(unusedValue) = 0;
//      tag_message message;
//
//      m_heroWindow0 = new heroWindow(TAVERN_WINDOW_X, TAVERN_WINDOW_Y, "tavwin.bin");
//
