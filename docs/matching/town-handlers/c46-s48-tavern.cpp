// Parent 7d4ffd56; 8 complete arms in 6.454889985732734 seconds.
// Selected trial 1: {'source_shape': 'omit0_cast1', 'candidate': 'baseline'}; size/text/label-sensitive reloc signature (288, '6061f3023d838dcb', '688c123bebaf3499').
// Same size and masked payload as baseline: ['omit0_cast0', 'omit0_cast1'].
// All arms compiled; source restored; no generator/wall truncation, AST mutation or TU-state census.
// Only selected combined source has all-98-object resolved native equivalence.
// Private counter-label hashes are not resolved-target proof; five existing retail EH/site gaps remain.
// Artifacts: build/c46-s48-tavern-manifest.json; build/source-variant-batch/c46-s48-tavern/results.json.
// See docs/reconstruction/C46-R10-S48-G56.md for exact per-function dispositions.
// Comment-only archive: strip the leading // and optional one space to recover the text below.
//
// Baseline:
// MessageDispatchResult TavernHandler(tag_message& message) {
//     i32 H2_UNUSED(unusedDelay) = TOWN_TAVERN_ANIMATION_DELAY;
//     i16 H2_UNUSED(unusedFirstFrame) = TOWN_TAVERN_FIRST_ANIMATION_FRAME;
//
//     if (message.type == MESSAGE_WIDGET) {
//         switch (message.payload.widget.command) {
//             case WIDGET_COMMAND_DESELECT:
//                 switch (message.payload.widget.id) {
//                     case EVENT_WINDOW_FIRST_BUTTON:
//                     case EVENT_WINDOW_SECOND_BUTTON:
//                     case TOWN_DIALOG_CONFIRM:
//                         gpWindowManager->m_dialogResult = message.payload.widget.id;
//                         message.payload.widget.id = IDX(WIDGET_COMMAND_DIALOG_SELECT);
//                         message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
//                         return MESSAGE_DISPATCH_FORWARD;
//                     default:
//                         break;
//                 }
//                 break;
//             default:
//                 break;
//         }
//     }
//     if (glTimers[0] < KBTickCount()) {
//         message.type = MESSAGE_WIDGET;
//         message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
//         message.payload.widget.id = TAVERN_ANIMATION_CONTROL;
//         ++gpGame->m_viewArmyResult;
//         message.payload.widget.data.value =
//             gpGame->m_viewArmyResult % TOWN_TAVERN_ANIMATION_FRAME_COUNT
//             + TOWN_TAVERN_FIRST_ANIMATION_FRAME;
//         gpTownManager->m_heroWindow0->BroadcastMessage(message);
//         gpTownManager->m_heroWindow0->MoveWindow(0, 0);
//         glTimers[0] = static_cast<i32>(KBTickCount() + TOWN_TAVERN_ANIMATION_DELAY);
//     }
//     return MESSAGE_DISPATCH_CONSUME;
// }
//
// Option omit0_cast0:
//
//
// Option omit0_cast1:
// --- baseline
// +++ omit0_cast1
// @@ -31,7 +31,7 @@
//              + TOWN_TAVERN_FIRST_ANIMATION_FRAME;
//          gpTownManager->m_heroWindow0->BroadcastMessage(message);
//          gpTownManager->m_heroWindow0->MoveWindow(0, 0);
// -        glTimers[0] = static_cast<i32>(KBTickCount() + TOWN_TAVERN_ANIMATION_DELAY);
// +        glTimers[0] = KBTickCount() + TOWN_TAVERN_ANIMATION_DELAY;
//      }
//      return MESSAGE_DISPATCH_CONSUME;
//  }
//
// Option omit1_cast0:
// --- baseline
// +++ omit1_cast0
// @@ -1,5 +1,4 @@
//  MessageDispatchResult TavernHandler(tag_message& message) {
// -    i32 H2_UNUSED(unusedDelay) = TOWN_TAVERN_ANIMATION_DELAY;
//      i16 H2_UNUSED(unusedFirstFrame) = TOWN_TAVERN_FIRST_ANIMATION_FRAME;
//
//      if (message.type == MESSAGE_WIDGET) {
//
// Option omit1_cast1:
// --- baseline
// +++ omit1_cast1
// @@ -1,5 +1,4 @@
//  MessageDispatchResult TavernHandler(tag_message& message) {
// -    i32 H2_UNUSED(unusedDelay) = TOWN_TAVERN_ANIMATION_DELAY;
//      i16 H2_UNUSED(unusedFirstFrame) = TOWN_TAVERN_FIRST_ANIMATION_FRAME;
//
//      if (message.type == MESSAGE_WIDGET) {
// @@ -31,7 +30,7 @@
//              + TOWN_TAVERN_FIRST_ANIMATION_FRAME;
//          gpTownManager->m_heroWindow0->BroadcastMessage(message);
//          gpTownManager->m_heroWindow0->MoveWindow(0, 0);
// -        glTimers[0] = static_cast<i32>(KBTickCount() + TOWN_TAVERN_ANIMATION_DELAY);
// +        glTimers[0] = KBTickCount() + TOWN_TAVERN_ANIMATION_DELAY;
//      }
//      return MESSAGE_DISPATCH_CONSUME;
//  }
//
// Option omit2_cast0:
// --- baseline
// +++ omit2_cast0
// @@ -1,6 +1,5 @@
//  MessageDispatchResult TavernHandler(tag_message& message) {
//      i32 H2_UNUSED(unusedDelay) = TOWN_TAVERN_ANIMATION_DELAY;
// -    i16 H2_UNUSED(unusedFirstFrame) = TOWN_TAVERN_FIRST_ANIMATION_FRAME;
//
//      if (message.type == MESSAGE_WIDGET) {
//          switch (message.payload.widget.command) {
//
// Option omit2_cast1:
// --- baseline
// +++ omit2_cast1
// @@ -1,6 +1,5 @@
//  MessageDispatchResult TavernHandler(tag_message& message) {
//      i32 H2_UNUSED(unusedDelay) = TOWN_TAVERN_ANIMATION_DELAY;
// -    i16 H2_UNUSED(unusedFirstFrame) = TOWN_TAVERN_FIRST_ANIMATION_FRAME;
//
//      if (message.type == MESSAGE_WIDGET) {
//          switch (message.payload.widget.command) {
// @@ -31,7 +30,7 @@
//              + TOWN_TAVERN_FIRST_ANIMATION_FRAME;
//          gpTownManager->m_heroWindow0->BroadcastMessage(message);
//          gpTownManager->m_heroWindow0->MoveWindow(0, 0);
// -        glTimers[0] = static_cast<i32>(KBTickCount() + TOWN_TAVERN_ANIMATION_DELAY);
// +        glTimers[0] = KBTickCount() + TOWN_TAVERN_ANIMATION_DELAY;
//      }
//      return MESSAGE_DISPATCH_CONSUME;
//  }
//
// Option omit3_cast0:
// --- baseline
// +++ omit3_cast0
// @@ -1,6 +1,4 @@
//  MessageDispatchResult TavernHandler(tag_message& message) {
// -    i32 H2_UNUSED(unusedDelay) = TOWN_TAVERN_ANIMATION_DELAY;
// -    i16 H2_UNUSED(unusedFirstFrame) = TOWN_TAVERN_FIRST_ANIMATION_FRAME;
//
//      if (message.type == MESSAGE_WIDGET) {
//          switch (message.payload.widget.command) {
//
// Option omit3_cast1:
// --- baseline
// +++ omit3_cast1
// @@ -1,6 +1,4 @@
//  MessageDispatchResult TavernHandler(tag_message& message) {
// -    i32 H2_UNUSED(unusedDelay) = TOWN_TAVERN_ANIMATION_DELAY;
// -    i16 H2_UNUSED(unusedFirstFrame) = TOWN_TAVERN_FIRST_ANIMATION_FRAME;
//
//      if (message.type == MESSAGE_WIDGET) {
//          switch (message.payload.widget.command) {
// @@ -31,7 +29,7 @@
//              + TOWN_TAVERN_FIRST_ANIMATION_FRAME;
//          gpTownManager->m_heroWindow0->BroadcastMessage(message);
//          gpTownManager->m_heroWindow0->MoveWindow(0, 0);
// -        glTimers[0] = static_cast<i32>(KBTickCount() + TOWN_TAVERN_ANIMATION_DELAY);
// +        glTimers[0] = KBTickCount() + TOWN_TAVERN_ANIMATION_DELAY;
//      }
//      return MESSAGE_DISPATCH_CONSUME;
//  }
//
