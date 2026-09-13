// Parent 7d4ffd56; 48 complete arms in 35.52148418314755 seconds.
// Selected trial 0: {'source_shape': 'original_omit0', 'candidate': 'baseline'}; size/text/label-sensitive reloc signature (719, 'e29ee8a101daf7be', '9ffd2d52420f4de5').
// Same size and masked payload as baseline: ['original_omit0'].
// All arms compiled; source restored; no generator/wall truncation, AST mutation or TU-state census.
// Only selected combined source has all-98-object resolved native equivalence.
// Private counter-label hashes are not resolved-target proof; five existing retail EH/site gaps remain.
// Artifacts: build/g56-s48-split-handler-manifest.json; build/source-variant-batch/g56-s48-split-handler/results.json.
// See docs/reconstruction/C46-R10-S48-G56.md for exact per-function dispositions.
// Comment-only archive: strip the leading // and optional one space to recover the text below.
//
// Baseline:
// MessageDispatchResult SplitArmyHandler(tag_message& message) {
//     i16 H2_UNUSED(plusButton_d) = TOWN_SPLIT_INCREASE_CONTROL;
//     i16 H2_UNUSED(minusButton_g) = TOWN_SPLIT_DECREASE_CONTROL;
//     i16 H2_UNUSED(amountControl_e) = TOWN_SPLIT_AMOUNT_CONTROL;
//     b32 handled_c = false;
//     i32 H2_UNUSED(unusedAction_l);
//
//     if (message.type == MESSAGE_WIDGET) {
//         switch (message.payload.widget.command) {
//             case WIDGET_COMMAND_SELECT:
//                 switch (message.payload.widget.id) {
//                     case TOWN_SPLIT_AMOUNT_CONTROL:
//                         message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
//                         gpTownManager->m_heroWindow1->BroadcastMessage(message);
//                         gpTownManager->m_splitAmount = atoi(message.payload.widget.data.text);
//                         if (gpTownManager->m_splitAmount < 0)
//                             gpTownManager->m_splitAmount = 0;
//                         if (gpTownManager->m_splitAmount >= gpTownManager->m_splitMaximum)
//                             gpTownManager->m_splitAmount = gpTownManager->m_splitMaximum - 1;
//                         goto update_amount;
//                 }
//                 break;
//             case WIDGET_COMMAND_DESELECT:
//                 switch (message.payload.widget.id) {
//                     case TOWN_SPLIT_INCREASE_CONTROL:
//                         ++gpTownManager->m_splitAmount;
//                         if (gpTownManager->m_splitAmount >= gpTownManager->m_splitMaximum)
//                             gpTownManager->m_splitAmount = gpTownManager->m_splitMaximum - 1;
//                         goto update_amount;
//                     case TOWN_SPLIT_DECREASE_CONTROL:
//                         --gpTownManager->m_splitAmount;
//                         if (gpTownManager->m_splitAmount < 0)
//                             gpTownManager->m_splitAmount = 0;
//                         goto update_amount;
//                     case EVENT_WINDOW_FIRST_BUTTON:
//                     case EVENT_WINDOW_SECOND_BUTTON:
//                         gpTownManager->m_splitAmount = 0;
//                         gpWindowManager->m_dialogResult = message.payload.widget.id;
//                         handled_c = true;
//                         break;
//                     case TOWN_DIALOG_CONFIRM:
//                         if (gpTownManager->m_splitAmount == 0)
//                             gpWindowManager->m_dialogResult = IDX(DIALOG_CANCEL_ID);
//                         else
//                             gpWindowManager->m_dialogResult = IDX(TOWN_DIALOG_CONFIRM);
//                         handled_c = true;
//                         break;
//                     default:
//                         break;
//                 }
//                 break;
//         }
//     }
//
//     if (handled_c == 1) {
//         message.payload.widget.id = IDX(WIDGET_COMMAND_DIALOG_SELECT);
//         message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
//         return MESSAGE_DISPATCH_FORWARD;
//     }
//     return MESSAGE_DISPATCH_CONSUME;
//
// update_amount:
//     sprintf(gText, "%d", gpTownManager->m_splitAmount);
//     message.type = MESSAGE_WIDGET;
//     message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
//     message.payload.widget.id = TOWN_SPLIT_AMOUNT_CONTROL;
//     message.payload.widget.data.text = gText;
//     gpTownManager->m_heroWindow1->BroadcastMessage(message);
//     gpTownManager->m_heroWindow1
//         ->DrawWindow(1, TOWN_SPLIT_AMOUNT_CONTROL, TOWN_SPLIT_AMOUNT_CONTROL);
//     return MESSAGE_DISPATCH_CONSUME;
// }
//
// Structural version update_flag:
// --- baseline
// +++ update_flag
// @@ -3,6 +3,7 @@
//      i16 H2_UNUSED(minusButton_g) = TOWN_SPLIT_DECREASE_CONTROL;
//      i16 H2_UNUSED(amountControl_e) = TOWN_SPLIT_AMOUNT_CONTROL;
//      b32 handled_c = false;
// +    b32 updateAmount = false;
//      i32 H2_UNUSED(unusedAction_l);
//
//      if (message.type == MESSAGE_WIDGET) {
// @@ -17,7 +18,8 @@
//                              gpTownManager->m_splitAmount = 0;
//                          if (gpTownManager->m_splitAmount >= gpTownManager->m_splitMaximum)
//                              gpTownManager->m_splitAmount = gpTownManager->m_splitMaximum - 1;
// -                        goto update_amount;
// +                        updateAmount = true;
// +                        break;
//                  }
//                  break;
//              case WIDGET_COMMAND_DESELECT:
// @@ -26,12 +28,14 @@
//                          ++gpTownManager->m_splitAmount;
//                          if (gpTownManager->m_splitAmount >= gpTownManager->m_splitMaximum)
//                              gpTownManager->m_splitAmount = gpTownManager->m_splitMaximum - 1;
// -                        goto update_amount;
// +                        updateAmount = true;
// +                        break;
//                      case TOWN_SPLIT_DECREASE_CONTROL:
//                          --gpTownManager->m_splitAmount;
//                          if (gpTownManager->m_splitAmount < 0)
//                              gpTownManager->m_splitAmount = 0;
// -                        goto update_amount;
// +                        updateAmount = true;
// +                        break;
//                      case EVENT_WINDOW_FIRST_BUTTON:
//                      case EVENT_WINDOW_SECOND_BUTTON:
//                          gpTownManager->m_splitAmount = 0;
// @@ -52,6 +56,18 @@
//          }
//      }
//
// +    if (updateAmount) {
// +        sprintf(gText, "%d", gpTownManager->m_splitAmount);
// +        message.type = MESSAGE_WIDGET;
// +        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
// +        message.payload.widget.id = TOWN_SPLIT_AMOUNT_CONTROL;
// +        message.payload.widget.data.text = gText;
// +        gpTownManager->m_heroWindow1->BroadcastMessage(message);
// +        gpTownManager->m_heroWindow1
// +            ->DrawWindow(1, TOWN_SPLIT_AMOUNT_CONTROL, TOWN_SPLIT_AMOUNT_CONTROL);
// +        return MESSAGE_DISPATCH_CONSUME;
// +    }
// +
//      if (handled_c == 1) {
//          message.payload.widget.id = IDX(WIDGET_COMMAND_DIALOG_SELECT);
//          message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
// @@ -59,14 +75,4 @@
//      }
//      return MESSAGE_DISPATCH_CONSUME;
//
// -update_amount:
// -    sprintf(gText, "%d", gpTownManager->m_splitAmount);
// -    message.type = MESSAGE_WIDGET;
// -    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
// -    message.payload.widget.id = TOWN_SPLIT_AMOUNT_CONTROL;
// -    message.payload.widget.data.text = gText;
// -    gpTownManager->m_heroWindow1->BroadcastMessage(message);
// -    gpTownManager->m_heroWindow1
// -        ->DrawWindow(1, TOWN_SPLIT_AMOUNT_CONTROL, TOWN_SPLIT_AMOUNT_CONTROL);
// -    return MESSAGE_DISPATCH_CONSUME;
//  }
//
// Structural version inline_tails:
// --- baseline
// +++ inline_tails
// @@ -17,7 +17,15 @@
//                              gpTownManager->m_splitAmount = 0;
//                          if (gpTownManager->m_splitAmount >= gpTownManager->m_splitMaximum)
//                              gpTownManager->m_splitAmount = gpTownManager->m_splitMaximum - 1;
// -                        goto update_amount;
// +                        sprintf(gText, "%d", gpTownManager->m_splitAmount);
// +                        message.type = MESSAGE_WIDGET;
// +                        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
// +                        message.payload.widget.id = TOWN_SPLIT_AMOUNT_CONTROL;
// +                        message.payload.widget.data.text = gText;
// +                        gpTownManager->m_heroWindow1->BroadcastMessage(message);
// +                        gpTownManager->m_heroWindow1
// +                            ->DrawWindow(1, TOWN_SPLIT_AMOUNT_CONTROL, TOWN_SPLIT_AMOUNT_CONTROL);
// +                        return MESSAGE_DISPATCH_CONSUME;
//                  }
//                  break;
//              case WIDGET_COMMAND_DESELECT:
// @@ -26,12 +34,28 @@
//                          ++gpTownManager->m_splitAmount;
//                          if (gpTownManager->m_splitAmount >= gpTownManager->m_splitMaximum)
//                              gpTownManager->m_splitAmount = gpTownManager->m_splitMaximum - 1;
// -                        goto update_amount;
// +                        sprintf(gText, "%d", gpTownManager->m_splitAmount);
// +                        message.type = MESSAGE_WIDGET;
// +                        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
// +                        message.payload.widget.id = TOWN_SPLIT_AMOUNT_CONTROL;
// +                        message.payload.widget.data.text = gText;
// +                        gpTownManager->m_heroWindow1->BroadcastMessage(message);
// +                        gpTownManager->m_heroWindow1
// +                            ->DrawWindow(1, TOWN_SPLIT_AMOUNT_CONTROL, TOWN_SPLIT_AMOUNT_CONTROL);
// +                        return MESSAGE_DISPATCH_CONSUME;
//                      case TOWN_SPLIT_DECREASE_CONTROL:
//                          --gpTownManager->m_splitAmount;
//                          if (gpTownManager->m_splitAmount < 0)
//                              gpTownManager->m_splitAmount = 0;
// -                        goto update_amount;
// +                        sprintf(gText, "%d", gpTownManager->m_splitAmount);
// +                        message.type = MESSAGE_WIDGET;
// +                        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
// +                        message.payload.widget.id = TOWN_SPLIT_AMOUNT_CONTROL;
// +                        message.payload.widget.data.text = gText;
// +                        gpTownManager->m_heroWindow1->BroadcastMessage(message);
// +                        gpTownManager->m_heroWindow1
// +                            ->DrawWindow(1, TOWN_SPLIT_AMOUNT_CONTROL, TOWN_SPLIT_AMOUNT_CONTROL);
// +                        return MESSAGE_DISPATCH_CONSUME;
//                      case EVENT_WINDOW_FIRST_BUTTON:
//                      case EVENT_WINDOW_SECOND_BUTTON:
//                          gpTownManager->m_splitAmount = 0;
// @@ -59,14 +83,4 @@
//      }
//      return MESSAGE_DISPATCH_CONSUME;
//
// -update_amount:
// -    sprintf(gText, "%d", gpTownManager->m_splitAmount);
// -    message.type = MESSAGE_WIDGET;
// -    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
// -    message.payload.widget.id = TOWN_SPLIT_AMOUNT_CONTROL;
// -    message.payload.widget.data.text = gText;
// -    gpTownManager->m_heroWindow1->BroadcastMessage(message);
// -    gpTownManager->m_heroWindow1
// -        ->DrawWindow(1, TOWN_SPLIT_AMOUNT_CONTROL, TOWN_SPLIT_AMOUNT_CONTROL);
// -    return MESSAGE_DISPATCH_CONSUME;
//  }
//
// Exact independent storage edits; each structural version crosses MASK=0..15:
// [
//   {
//     "bit": 0,
//     "find": "    i16 H2_UNUSED(plusButton_d) = TOWN_SPLIT_INCREASE_CONTROL;\n",
//     "replace": ""
//   },
//   {
//     "bit": 1,
//     "find": "    i16 H2_UNUSED(minusButton_g) = TOWN_SPLIT_DECREASE_CONTROL;\n",
//     "replace": ""
//   },
//   {
//     "bit": 2,
//     "find": "    i16 H2_UNUSED(amountControl_e) = TOWN_SPLIT_AMOUNT_CONTROL;\n",
//     "replace": ""
//   },
//   {
//     "bit": 3,
//     "find": "    i32 H2_UNUSED(unusedAction_l);\n",
//     "replace": ""
//   }
// ]
// All 48 complete option strings replayed exactly.
