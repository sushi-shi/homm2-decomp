// VC6 SP5 REQUEST source family g90-request-selection; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/g90-request-selection/results.json; RVA 0x8f737
// Complete 2 arms; elapsed 1.681515s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 4388 / a96d6354363d97b7 / b5daded3af5efd4c / 195:196 / 194,0,0,1 / False
// 1 / {'source_family': 'if_else_chain', 'candidate': 'baseline'} / 4349 / 4cd46c7340dab079 / 355fa845e64fa80d / 190:196 / 181,0,6,3 / False

// AXIS source_family
// BEFORE (complete exact span):
// |                                               - FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
// |                                     goto SelectListItem;
// |                                 }
// |                                 if (message.payload.widget.id
// |                                         >= FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
// |                                     && message.payload.widget.id
// |                                            <= FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
// |                                                   + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// |                                     iResult = message.payload.widget.id
// |                                               - FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
// |                                     goto SelectListItem;
// |                                 }
// |                                 if (message.payload.widget.id
// |                                         >= FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
// |                                     && message.payload.widget.id
// |                                            <= FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
// |                                                   + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// |                                     iResult = message.payload.widget.id
// |                                               - FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
// |                                     goto SelectListItem;
// |                                 }
// |                                 if (message.payload.widget.id >= FILE_REQUESTER_MAP_LOSS_ICON_FIRST
// |                                     && message.payload.widget.id
// |                                            <= FILE_REQUESTER_MAP_LOSS_ICON_FIRST
// |                                                   + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// |                                     iResult = message.payload.widget.id
// |                                               - FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
// |                                     goto SelectListItem;
// |                                 }
// |                                 if (message.payload.widget.id >= FILE_REQUESTER_LIST_TEXT_FIRST
// |                                     && message.payload.widget.id
// |                                            <= FILE_REQUESTER_LIST_TEXT_FIRST
// |                                                   + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// |                                     iResult =
// |                                         message.payload.widget.id - FILE_REQUESTER_LIST_TEXT_FIRST;
// |                                     goto SelectListItem;
// |                                 }
// |                                 break;
// |                             SelectListItem:
// |                                 if (iResult + m_topIndex == m_selectedIndex) {
// ALTERNATIVE if_else_chain (unified source delta from BEFORE):
// | --- before
// | +++ if_else_chain
// | @@ -1,40 +1,31 @@
// |                                                - FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
// | -                                    goto SelectListItem;
// | -                                }
// | -                                if (message.payload.widget.id
// | +                                } else if (message.payload.widget.id
// |                                          >= FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
// |                                      && message.payload.widget.id
// |                                             <= FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
// |                                                    + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// |                                      iResult = message.payload.widget.id
// |                                                - FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
// | -                                    goto SelectListItem;
// | -                                }
// | -                                if (message.payload.widget.id
// | +                                } else if (message.payload.widget.id
// |                                          >= FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
// |                                      && message.payload.widget.id
// |                                             <= FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
// |                                                    + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// |                                      iResult = message.payload.widget.id
// |                                                - FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
// | -                                    goto SelectListItem;
// | -                                }
// | -                                if (message.payload.widget.id >= FILE_REQUESTER_MAP_LOSS_ICON_FIRST
// | +                                } else if (message.payload.widget.id >= FILE_REQUESTER_MAP_LOSS_ICON_FIRST
// |                                      && message.payload.widget.id
// |                                             <= FILE_REQUESTER_MAP_LOSS_ICON_FIRST
// |                                                    + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// |                                      iResult = message.payload.widget.id
// |                                                - FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
// | -                                    goto SelectListItem;
// | -                                }
// | -                                if (message.payload.widget.id >= FILE_REQUESTER_LIST_TEXT_FIRST
// | +                                } else if (message.payload.widget.id >= FILE_REQUESTER_LIST_TEXT_FIRST
// |                                      && message.payload.widget.id
// |                                             <= FILE_REQUESTER_LIST_TEXT_FIRST
// |                                                    + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// |                                      iResult =
// |                                          message.payload.widget.id - FILE_REQUESTER_LIST_TEXT_FIRST;
// | -                                    goto SelectListItem;
// | +                                } else {
// | +                                    break;
// |                                  }
// | -                                break;
// | -                            SelectListItem:
// |                                  if (iResult + m_topIndex == m_selectedIndex) {
