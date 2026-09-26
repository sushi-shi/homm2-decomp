// Parent 7d4ffd56; 4 complete arms in 3.5388695895671844 seconds.
// Selected trial 0: {'source_shape': 'omit0', 'candidate': 'baseline'}; size/text/label-sensitive reloc signature (886, '8dd4c30c9ba1bbe9', '24d719cdfd14a076').
// Same size and masked payload as baseline: ['omit0'].
// All arms compiled; source restored; no generator/wall truncation, AST mutation or TU-state census.
// Only selected combined source has all-98-object resolved native equivalence.
// Private counter-label hashes are not resolved-target proof; five existing retail EH/site gaps remain.
// Artifacts: build/s48-split-manifest.json; build/source-variant-batch/s48-split/results.json.
// See docs/reconstruction/C46-R10-S48-G56.md for exact per-function dispositions.
// Comment-only archive: strip the leading // and optional one space to recover the text below.
//
// Baseline:
// void townManager::SplitArmy(void) {
//     i16 H2_UNUSED(msgId) = 1;
//     i16 H2_UNUSED(amountId) = 4;
//     b32 sameType;
//     tag_message message;
//
//     m_heroWindow1 = new heroWindow(SMALL_DIALOG_WINDOW_X, SMALL_DIALOG_WINDOW_Y, "splitwin.bin");
//     if (m_heroWindow1 == NULL)
//         MemError();
//     m_splitAmount = 0;
//     m_splitMaximum = m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
//     message.type = MESSAGE_WIDGET;
//     if (m_pendingStrip->m_army == m_swapStrip->m_army) {
//         sprintf(gText, "\xd1\xea\xee\xeb\xfc\xea\xee \xe2\xee\xe8\xed\xee\xe2 \xef\xe5\xf0\xe5\xed\xe5\xf1\xf2\xe8?" /* "Сколько воинов перенести?" */);
//     } else {
//         sprintf(
//             gText,
//             "\xca\xe0\xea \xec\xed\xee\xe3\xee %s \xef\xe5\xf0\xe5\xed\xe5\xf1\xf2\xe8 \xe8\xe7 \xe0\xf0\xec\xe8\xe8 %s \xe2 \xe0\xf0\xec\xe8\xfe %s?" /* "Как много %s перенести из армии %s в армию %s?" */,
//             gArmyNamesPlural[IDX(m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])],
//             m_swapStrip == m_heroStrip ? "\xe3\xe5\xf0\xee\xff" /* "героя" */ : "\xe3\xe0\xf0\xed\xe8\xe7\xee\xed\xe0" /* "гарнизона" */,
//             m_pendingStrip == m_heroStrip ? "\xe3\xe5\xf0\xee\xff" /* "героя" */ : "\xe3\xe0\xf0\xed\xe8\xe7\xee\xed\xe0" /* "гарнизона" */
//         );
//     }
//     message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
//     message.payload.widget.id = 1;
//     message.payload.widget.data.text = gText;
//     m_heroWindow1->BroadcastMessage(message);
//     sprintf(gText, "%d", m_splitAmount);
//     message.payload.widget.id = SPLIT_SETUP_AMOUNT_CONTROL;
//     message.payload.widget.data.text = gText;
//     m_heroWindow1->BroadcastMessage(message);
//     gpWindowManager->DoDialog(m_heroWindow1, SplitArmyHandler, 0);
//     delete m_heroWindow1;
//     if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
//         sameType = false;
//         if (m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]
//             == m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])
//             sameType = true;
//         if (sameType != 0) {
//             m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] += m_splitAmount;
//         } else {
//             m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] =
//                 m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot];
//             m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] = m_splitAmount;
//         }
//         m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] -= m_splitAmount;
//     }
// }
//
// Option omit0:
//
//
// Option omit1:
// --- baseline
// +++ omit1
// @@ -1,5 +1,4 @@
//  void townManager::SplitArmy(void) {
// -    i16 H2_UNUSED(msgId) = 1;
//      i16 H2_UNUSED(amountId) = 4;
//      b32 sameType;
//      tag_message message;
//
// Option omit2:
// --- baseline
// +++ omit2
// @@ -1,6 +1,5 @@
//  void townManager::SplitArmy(void) {
//      i16 H2_UNUSED(msgId) = 1;
// -    i16 H2_UNUSED(amountId) = 4;
//      b32 sameType;
//      tag_message message;
//
//
// Option omit3:
// --- baseline
// +++ omit3
// @@ -1,6 +1,4 @@
//  void townManager::SplitArmy(void) {
// -    i16 H2_UNUSED(msgId) = 1;
// -    i16 H2_UNUSED(amountId) = 4;
//      b32 sameType;
//      tag_message message;
//
//
