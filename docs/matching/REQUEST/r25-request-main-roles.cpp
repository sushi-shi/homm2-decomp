// VC6 SP5 REQUEST source family r25-request-main-roles; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r25-request-main-roles/results.json; RVA 0x8f737
// Complete 2 arms; elapsed 1.739971s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 4388 / a96d6354363d97b7 / b5daded3af5efd4c / 195:196 / 194,0,0,1 / False
// 1 / {'source_family': 'distinct_roles', 'candidate': 'baseline'} / 4379 / 22470fd45e70299f / 616c5aee28c406c7 / 195:196 / 194,0,0,1 / False

// AXIS source_family
// BEFORE (complete exact span):
// |     b32 acceptStep = false;
// |     i32 iResult;
// |     i32 lengthIndex;
// |     FileRequesterHelpIndex helpIndexMouse;
// |     char cycleNameBuffer[FILE_REQUESTER_PATH_SIZE];
// |     i32 positions;
// |     tag_message broadcastMessage;
// |     char filteredNameMap[FILE_REQUESTER_PATH_SIZE];
// |     i32 topIndexValue;
// |     i32 stepScreen;
// |
// |     switch (message.type) {
// |         case MESSAGE_KEY_DOWN:
// |             switch (message.payload.keyboard.keyCode) {
// |                 case INPUT_SCAN_F6: {
// |                     if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
// |                         strcpy(cycleNameBuffer, m_fileNames[m_selectedIndex].text);
// |                     } else {
// |                         strcpy(
// |                             cycleNameBuffer,
// |                             ""
// |                         );
// |                     }
// |                     giMapSizeFilter = static_cast<FileRequesterMapSizeFilter>(
// |                         (IDX(giMapSizeFilter) + 1) % IDX(FILE_REQUESTER_MAP_SIZE_COUNT)
// |                     );
// |                     SetupFiles();
// |                     if (strlen(cycleNameBuffer) != 0) {
// |                         for (iResult = 0; iResult < m_fileCount; ++iResult) {
// |                             if (strcmpi(m_fileNames[iResult].text, cycleNameBuffer) == 0) {
// |                                 m_selectedIndex = iResult;
// |                             }
// |                         }
// |                     }
// |                     Update(1);
// |                     break;
// |                 }
// |                 case INPUT_SCAN_NUMPAD_8:
// |                     if (m_selectedIndex > 0) {
// |                         --m_selectedIndex;
// |                         if (m_topIndex > m_selectedIndex) {
// |                             --m_topIndex;
// |                         }
// |                         Update(1);
// |                     }
// |                     break;
// |                 case INPUT_SCAN_NUMPAD_2:
// |                     if (m_selectedIndex < m_fileCount - 1) {
// |                         ++m_selectedIndex;
// |                         if (m_topIndex + iMaxListSize <= m_selectedIndex) {
// |                             ++m_topIndex;
// |                         }
// |                         Update(1);
// |                     }
// |                     break;
// |             }
// |             break;
// |         case MESSAGE_WIDGET:
// |             switch (message.payload.widget.command) {
// |                 case WIDGET_COMMAND_DESELECT:
// |                     switch (message.payload.widget.id) {
// |                         case FILE_REQUESTER_SCROLL_UP:
// |                             if (m_topIndex > 0) {
// |                                 --m_topIndex;
// |                                 Update(1);
// |                             }
// |                             break;
// |                         case FILE_REQUESTER_SCROLL_DOWN:
// |                             if (m_topIndex + iMaxListSize < m_fileCount) {
// |                                 ++m_topIndex;
// |                                 if (m_topIndex + iMaxListSize - 1 >= m_fileCount) {
// |                                     m_topIndex = m_fileCount - iMaxListSize;
// |                                 }
// |                                 Update(1);
// |                             }
// |                             break;
// |                         case FILE_REQUESTER_OK:
// |                             if (m_selectedIndex == FILE_REQUESTER_SELECTION_NONE
// |                                 && m_filename[0] == 0) {
// |                                 NormalDialog(
// |                                     "\xc2\xfb\xe1\xe5\xf0\xe8\xf2\xe5 \xe8\xe7 \xf1\xef\xe8\xf1\xea"
// |                                     "\xe0 \xe8\xeb\xe8 \xed\xe0\xe6\xec\xe8\xf2\xe5 \xea\xed\xee"
// |                                     "\xef\xea\xf3 \xee\xf2\xec\xe5\xed\xfb."
// |                                     /* "Выберите из списка или нажмите кнопку отмены." */
// |                                     ,
// |                                     NORMAL_DIALOG_INFO,
// |                                     NORMAL_DIALOG_NO_RESOURCE,
// |                                     NORMAL_DIALOG_NO_VALUE,
// |                                     NORMAL_DIALOG_NO_RESOURCE,
// |                                     0,
// |                                     NORMAL_DIALOG_NO_RESOURCE,
// |                                     0,
// |                                     NORMAL_DIALOG_NO_RESOURCE,
// |                                     0
// |                                 );
// |                                 break;
// |                             }
// |                             message.payload.widget.data.value = message.payload.widget.id;
// |                             acceptStep = true;
// |                             break;
// |                         case FILE_REQUESTER_CANCEL:
// |                             message.payload.widget.data.value = message.payload.widget.id;
// |                             acceptStep = true;
// |                             break;
// |                     }
// |                     break;
// |                 case WIDGET_COMMAND_SELECT:
// |                 case WIDGET_COMMAND_ALTERNATE_SELECT:
// |                     if (HAS(
// |                             message.payload.widget.modifiers,
// |                             MESSAGE_MODIFIER_RIGHT_BUTTON
// |                         )) {
// |                         helpIndexMouse = REQUESTER_HELP_NONE;
// |                         switch (message.payload.widget.id) {
// |                             case FILE_REQUESTER_FILTER_SMALL:
// |                                 helpIndexMouse = REQUESTER_HELP_FILTER_SMALL;
// |                                 break;
// |                             case FILE_REQUESTER_FILTER_MEDIUM:
// |                                 helpIndexMouse = REQUESTER_HELP_FILTER_MEDIUM;
// |                                 break;
// |                             case FILE_REQUESTER_FILTER_LARGE:
// |                                 helpIndexMouse = REQUESTER_HELP_FILTER_LARGE;
// |                                 break;
// |                             case FILE_REQUESTER_FILTER_XLARGE:
// |                                 helpIndexMouse = REQUESTER_HELP_FILTER_XLARGE;
// |                                 break;
// |                             case FILE_REQUESTER_FILTER_ALL:
// |                                 helpIndexMouse = REQUESTER_HELP_FILTER_ALL;
// |                                 break;
// |                             case FILE_REQUESTER_FILENAME_ENTRY:
// |                                 helpIndexMouse = REQUESTER_HELP_FILENAME;
// |                                 break;
// |                             case FILE_REQUESTER_OK:
// |                                 helpIndexMouse = REQUESTER_HELP_OK;
// |                                 break;
// |                             case FILE_REQUESTER_CANCEL:
// |                                 helpIndexMouse = REQUESTER_HELP_CANCEL;
// |                                 break;
// |                             case FILE_REQUESTER_MAP_SIZE:
// |                                 helpIndexMouse = REQUESTER_HELP_MAP_SIZE;
// |                                 break;
// |                             case FILE_REQUESTER_MAP_PLAYER_COUNT:
// |                                 helpIndexMouse = REQUESTER_HELP_PLAYER_COUNT;
// |                                 break;
// |                             case FILE_REQUESTER_MAP_VICTORY:
// |                                 helpIndexMouse = REQUESTER_HELP_VICTORY;
// |                                 break;
// |                             case FILE_REQUESTER_MAP_LOSS:
// |                                 helpIndexMouse = REQUESTER_HELP_LOSS;
// |                                 break;
// |                             case FILE_REQUESTER_MAP_NAME:
// |                                 helpIndexMouse = REQUESTER_HELP_MAP_NAME;
// |                                 break;
// |                             case FILE_REQUESTER_MAP_DESCRIPTION:
// |                                 helpIndexMouse = REQUESTER_HELP_DESCRIPTION;
// |                                 break;
// |                             case FILE_REQUESTER_MAP_DIFFICULTY_ICON:
// |                                 helpIndexMouse = REQUESTER_HELP_DIFFICULTY;
// |                                 break;
// |                             case FILE_REQUESTER_MAP_DIFFICULTY_TEXT:
// |                                 helpIndexMouse = REQUESTER_HELP_DIFFICULTY;
// |                                 break;
// |                             default:
// |                                 if (message.payload.widget.id >= FILE_REQUESTER_MAP_SIZE_ICON_FIRST
// |                                     && message.payload.widget.id
// |                                            < FILE_REQUESTER_MAP_SIZE_ICON_FIRST
// |                                                  + FILE_REQUESTER_LIST_RANGE_SIZE) {
// |                                     helpIndexMouse = REQUESTER_HELP_MAP_SIZE;
// |                                 } else if (message.payload.widget.id
// |                                                >= FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
// |                                            && message.payload.widget.id
// |                                                   < FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
// |                                                         + FILE_REQUESTER_LIST_RANGE_SIZE) {
// |                                     helpIndexMouse = REQUESTER_HELP_PLAYER_COUNT;
// |                                 } else if (message.payload.widget.id
// |                                                >= FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
// |                                            && message.payload.widget.id
// |                                                   < FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
// |                                                         + FILE_REQUESTER_LIST_RANGE_SIZE) {
// |                                     helpIndexMouse = REQUESTER_HELP_VICTORY;
// |                                 } else if (message.payload.widget.id
// |                                                >= FILE_REQUESTER_MAP_LOSS_ICON_FIRST
// |                                            && message.payload.widget.id
// |                                                   < FILE_REQUESTER_MAP_LOSS_ICON_FIRST
// |                                                         + FILE_REQUESTER_LIST_RANGE_SIZE) {
// |                                     helpIndexMouse = REQUESTER_HELP_LOSS;
// |                                 }
// |                                 break;
// |                         }
// |                         if (helpIndexMouse >= REQUESTER_HELP_VALID_BEGIN) {
// |                             NormalDialog(
// |                                 gFileRequestHelp[IDX(helpIndexMouse)],
// |                                 NORMAL_DIALOG_QUICK_VIEW,
// |                                 NORMAL_DIALOG_NO_RESOURCE,
// |                                 NORMAL_DIALOG_NO_VALUE,
// |                                 NORMAL_DIALOG_NO_RESOURCE,
// |                                 0,
// |                                 NORMAL_DIALOG_NO_RESOURCE,
// |                                 0,
// |                                 NORMAL_DIALOG_NO_RESOURCE,
// |                                 0
// |                             );
// |                         }
// |                     } else {
// |                         switch (message.payload.widget.id) {
// |                             case FILE_REQUESTER_FILTER_SMALL:
// |                             case FILE_REQUESTER_FILTER_MEDIUM:
// |                             case FILE_REQUESTER_FILTER_LARGE:
// |                             case FILE_REQUESTER_FILTER_XLARGE:
// |                             case FILE_REQUESTER_FILTER_ALL: {
// |                                 iResult = message.payload.widget.id - FILE_REQUESTER_FILTER_SMALL;
// |                                 if (!MapExistsForFilter(
// |                                         static_cast<FileRequesterMapSizeFilter>(iResult)
// |                                     )) {
// |                                     if (giNumHumanPlayers == 1) {
// |                                         sprintf(
// |                                             gText,
// |                                             "\xcd\xe5\xf2 \xea\xe0\xf0\xf2 \xf2\xe0\xea\xee\xe3\xee"
// |                                             " \xf0\xe0\xe7\xec\xe5\xf0\xe0 \xe4\xeb\xff %d \xe8\xe3"
// |                                             "\xf0\xee\xea\xee\xe2-\xeb\xfe\xe4\xe5\xe9."
// |                                             /* "Нет карт такого размера для %d игроков-людей." */
// |                                             ,
// |                                             giNumHumanPlayers
// |                                         );
// |                                     } else {
// |                                         sprintf(
// |                                             gText,
// |                                             "\xcd\xe5\xf2 \xea\xe0\xf0\xf2\xfb \xfd\xf2\xee\xe3\xee"
// |                                             " \xf0\xe0\xe7\xec\xe5\xf0\xe0 \xe4\xeb\xff %d \xe8\xe3"
// |                                             "\xf0\xee\xea\xee\xe2-\xeb\xfe\xe4\xe5\xe9."
// |                                             /* "Нет карты этого размера для %d игроков-людей." */
// |                                             ,
// |                                             giNumHumanPlayers
// |                                         );
// |                                     }
// |                                     NormalDialog(
// |                                         gText,
// |                                         NORMAL_DIALOG_INFO,
// |                                         NORMAL_DIALOG_NO_RESOURCE,
// |                                         NORMAL_DIALOG_NO_VALUE,
// |                                         NORMAL_DIALOG_NO_RESOURCE,
// |                                         0,
// |                                         NORMAL_DIALOG_NO_RESOURCE,
// |                                         0,
// |                                         NORMAL_DIALOG_NO_RESOURCE,
// |                                         0
// |                                     );
// |                                     break;
// |                                 }
// |                                 giMapSizeFilter = static_cast<FileRequesterMapSizeFilter>(iResult);
// |                                 if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
// |                                     strcpy(filteredNameMap, m_fileNames[m_selectedIndex].text);
// |                                 } else {
// |                                     strcpy(
// |                                         filteredNameMap,
// |                                         ""
// |                                     );
// |                                 }
// |                                 SetupFiles();
// |                                 if (strlen(filteredNameMap) != 0) {
// |                                     for (iResult = 0; iResult < m_fileCount; ++iResult) {
// |                                         if (strcmpi(m_fileNames[iResult].text, filteredNameMap)
// |                                             == 0) {
// |                                             m_selectedIndex = iResult;
// |                                         }
// |                                     }
// |                                 }
// |                                 Update(1);
// |                                 break;
// |                             }
// |                             case FILE_REQUESTER_FILENAME_ENTRY: {
// |                                 broadcastMessage.type = MESSAGE_WIDGET;
// |                                 broadcastMessage.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
// |                                 broadcastMessage.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
// |                                 m_window->BroadcastMessage(broadcastMessage);
// |
// |                                 memset(newNameData, 0, FILE_REQUESTER_FILENAME_INITIAL_CLEAR_SIZE);
// |                                 strcpy(reinterpret_cast<char*>(newNameData), broadcastMessage.payload.widget.data.text);
// |                                 lengthIndex = strlen(reinterpret_cast<char*>(newNameData));
// |                                 for (iResult = 0; iResult < lengthIndex; ++iResult) {
// |                                     if (!((newNameData[iResult] >= 'A'
// |                                            && newNameData[iResult] <= 'Z')
// |                                           || (newNameData[iResult] >= 'a'
// |                                               && newNameData[iResult] <= 'z')
// |                                           || (newNameData[iResult] >= '0'
// |                                               && newNameData[iResult] <= '9')
// |                                           || (newNameData[iResult] >= CP1251_UPPER_FIRST
// |                                               && newNameData[iResult] <= CP1251_UPPER_LAST)
// |                                           || (newNameData[iResult] >= CP1251_LOWER_FIRST
// |                                               && newNameData[iResult] <= CP1251_LOWER_LAST)
// |                                           || newNameData[iResult] == CP1251_YO_UPPER
// |                                           || newNameData[iResult] == CP1251_YO_LOWER
// |                                           || newNameData[iResult] == '_'
// |                                           || newNameData[iResult] == ' '
// |                                           || FindToken(
// |                                                  "$%'-_@~`!(){}^#&+,;=[].",
// |                                                  newNameData[iResult]
// |                                              ) != NULL)) {
// |                                         newNameData[iResult] = 0;
// |                                     }
// |                                 }
// |                                 for (iResult = strlen(reinterpret_cast<char*>(newNameData)) - 1; iResult >= 0; --iResult) {
// |                                     if (newNameData[iResult] == ' ')
// |                                         newNameData[iResult] = 0;
// |                                     else
// |                                         iResult = -1;
// |                                 }
// |                                 if (strlen(reinterpret_cast<char*>(newNameData)) > 0 && newNameData[0] > ' ') {
// |                                     m_selectedIndex = FILE_REQUESTER_SELECTION_NONE;
// |                                     strcpy(m_filename, reinterpret_cast<char*>(newNameData));
// |                                     SetOK(1);
// |                                 }
// |                                 broadcastMessage.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
// |                                 broadcastMessage.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
// |                                 broadcastMessage.payload.widget.data.text = m_filename;
// |                                 m_window->BroadcastMessage(broadcastMessage);
// |                                 Update(1);
// |                                 break;
// |                             }
// |                             case FILE_REQUESTER_SCROLL_GUTTER: {
// |                                 positions = m_fileCount - (iMaxListSize - 1);
// |                                 if (positions < 1)
// |                                     positions = 1;
// |                                 stepScreen = static_cast<i32>(
// |                                     (fGutterTravelLength
// |                                      * IDX(FILE_REQUESTER_GUTTER_SCALE))
// |                                     / positions
// |                                 );
// |                                 mouseX = message.payload.widget.screenX;
// |                                 screenY = message.payload.widget.screenY;
// |                                 screenY = static_cast<i32>(screenY - (m_y + fGutterMinY));
// |                                 screenY -= FILE_REQUESTER_SCROLL_KNOB_HALF_HEIGHT;
// |                                 topIndexValue =
// |                                     (screenY * FILE_REQUESTER_GUTTER_SCALE) / stepScreen;
// |                                 m_topIndex = topIndexValue;
// |                                 if (m_topIndex + iMaxListSize - 1 >= m_fileCount)
// |                                     m_topIndex = m_fileCount - iMaxListSize;
// |                                 if (m_topIndex < 0)
// |                                     m_topIndex = 0;
// |                                 Update(1);
// |                                 break;
// |                             }
// |                             case FILE_REQUESTER_SCROLL_KNOB:
// |                                 DoKnob();
// |                                 break;
// |                             default: {
// |                                 if (message.payload.widget.id >= FILE_REQUESTER_MAP_SIZE_ICON_FIRST
// |                                     && message.payload.widget.id
// |                                            <= FILE_REQUESTER_MAP_SIZE_ICON_FIRST
// |                                                   + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// |                                     iResult = message.payload.widget.id
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
// |                                     message.payload.widget.data.value = FILE_REQUESTER_OK;
// |                                     message.payload.widget.id = FILE_REQUESTER_OK;
// |                                     acceptStep = true;
// |                                     break;
// |                                 }
// |                                 if (iResult + m_topIndex >= m_fileCount)
// |                                     break;
// |                                 m_selectedIndex = iResult + m_topIndex;
// |                                 SetOK(1);
// |                                 Update(1);
// |                                 break;
// |                             }
// |                         }
// |                     }
// |                     break;
// |             }
// |             break;
// |     }
// |
// |     if (acceptStep == 1) {
// |         if (m_mode == FILE_REQUESTER_LOAD_GAME && m_selectedIndex >= 0
// |             && message.payload.widget.data.value != FILE_REQUESTER_CANCEL
// |             && strcmpi(m_extensions[m_selectedIndex].text, ".GMC") != 0
// |             && strcmpi(m_extensions[m_selectedIndex].text, ".GXC") != 0) {
// |             iResult =
// |                 m_extensions[m_selectedIndex].text[FILE_REQUESTER_EXTENSION_PLAYER_DIGIT] - '0';
// |             if (iResult < giNumHumanPlayers
// |                 && giDebugLevel < FILE_REQUESTER_DEBUG_ALLOW_PLAYER_MISMATCH) {
// |                 sprintf(
// |                     gText,
// |                     "\xc2\xfb\xe1\xf0\xe0\xed\xed\xe0\xff \xe2\xe0\xec\xe8 \xe8\xe3\xf0\xe0 \xf0"
// |                     "\xe0\xf1\xf1\xf7\xe8\xf2\xe0\xed\xe0 \xf2\xee\xeb\xfc\xea\xee \xed\xe0 %d \xf7"
// |                     "\xe5\xeb\xee\xe2\xe5\xea.  \xc0 \xe2\xe0\xec \xed\xf3\xe6\xed\xe0 \xea\xe0\xf0"
// |                     "\xf2\xe0, \xea\xe0\xea \xec\xe8\xed\xe8\xec\xf3\xec \xed\xe0 %d \xf7\xe5\xeb"
// |                     "\xee\xe2\xe5\xea."
// |                     /* "Выбранная вами игра рассчитана только на %d человек.  А вам нужна
// |                        карта, как минимум на %d человек." */
// |                     ,
// |                     iResult,
// |                     giNumHumanPlayers
// |                 );
// |                 NormalDialog(
// |                     gText,
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
// |                 acceptStep = false;
// |             }
// |             if (iResult > giNumHumanPlayers) {
// |                 sprintf(
// |                     gText,
// |                     "\xc2\xfb\xe1\xf0\xe0\xed\xed\xe0\xff \xe8\xe3\xf0\xe0 \xed\xe0\xf7\xed\xe5\xf2"
// |                     "\xf1\xff \xf1 %d \xe8\xe3\xf0\xee\xea\xe0\xec\xe8-\xeb\xfe\xe4\xfc\xec\xe8. "
// |                     "\xcc\xee\xe6\xed\xee \xeb\xe8 \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xf3 \xe2"
// |                     "\xe7\xff\xf2\xfc \xef\xee\xe4 \xf1\xe2\xee\xe5 \xf3\xef\xf0\xe0\xe2\xeb\xe5"
// |                     "\xed\xe8\xe5 \xee\xf1\xf2\xe0\xe2\xf8\xe8\xe5\xf1\xff %d \xec\xe5\xf1\xf2 \xeb"
// |                     "\xfe\xe4\xe5\xe9?"
// |                     /* "Выбранная игра начнется с %d игроками-людьми. Можно ли
// |                        компьютеру взять под свое управление оставшиеся %d мест людей?" */
// |                     ,
// |                     iResult,
// |                     iResult - giNumHumanPlayers
// |                 );
// ALTERNATIVE distinct_roles (unified source delta from BEFORE):
// | --- before
// | +++ distinct_roles
// | @@ -1,5 +1,9 @@
// |      b32 acceptStep = false;
// | -    i32 iResult;
// | +    i32 fileIndex;
// | +    i32 filterIndex;
// | +    i32 characterIndex;
// | +    i32 rowIndex;
// | +    i32 playerCount;
// |      i32 lengthIndex;
// |      FileRequesterHelpIndex helpIndexMouse;
// |      char cycleNameBuffer[FILE_REQUESTER_PATH_SIZE];
// | @@ -26,9 +30,9 @@
// |                      );
// |                      SetupFiles();
// |                      if (strlen(cycleNameBuffer) != 0) {
// | -                        for (iResult = 0; iResult < m_fileCount; ++iResult) {
// | -                            if (strcmpi(m_fileNames[iResult].text, cycleNameBuffer) == 0) {
// | -                                m_selectedIndex = iResult;
// | +                        for (fileIndex = 0; fileIndex < m_fileCount; ++fileIndex) {
// | +                            if (strcmpi(m_fileNames[fileIndex].text, cycleNameBuffer) == 0) {
// | +                                m_selectedIndex = fileIndex;
// |                              }
// |                          }
// |                      }
// | @@ -208,9 +212,9 @@
// |                              case FILE_REQUESTER_FILTER_LARGE:
// |                              case FILE_REQUESTER_FILTER_XLARGE:
// |                              case FILE_REQUESTER_FILTER_ALL: {
// | -                                iResult = message.payload.widget.id - FILE_REQUESTER_FILTER_SMALL;
// | +                                filterIndex = message.payload.widget.id - FILE_REQUESTER_FILTER_SMALL;
// |                                  if (!MapExistsForFilter(
// | -                                        static_cast<FileRequesterMapSizeFilter>(iResult)
// | +                                        static_cast<FileRequesterMapSizeFilter>(filterIndex)
// |                                      )) {
// |                                      if (giNumHumanPlayers == 1) {
// |                                          sprintf(
// | @@ -247,7 +251,7 @@
// |                                      );
// |                                      break;
// |                                  }
// | -                                giMapSizeFilter = static_cast<FileRequesterMapSizeFilter>(iResult);
// | +                                giMapSizeFilter = static_cast<FileRequesterMapSizeFilter>(filterIndex);
// |                                  if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
// |                                      strcpy(filteredNameMap, m_fileNames[m_selectedIndex].text);
// |                                  } else {
// | @@ -258,10 +262,10 @@
// |                                  }
// |                                  SetupFiles();
// |                                  if (strlen(filteredNameMap) != 0) {
// | -                                    for (iResult = 0; iResult < m_fileCount; ++iResult) {
// | -                                        if (strcmpi(m_fileNames[iResult].text, filteredNameMap)
// | +                                    for (fileIndex = 0; fileIndex < m_fileCount; ++fileIndex) {
// | +                                        if (strcmpi(m_fileNames[fileIndex].text, filteredNameMap)
// |                                              == 0) {
// | -                                            m_selectedIndex = iResult;
// | +                                            m_selectedIndex = fileIndex;
// |                                          }
// |                                      }
// |                                  }
// | @@ -277,33 +281,33 @@
// |                                  memset(newNameData, 0, FILE_REQUESTER_FILENAME_INITIAL_CLEAR_SIZE);
// |                                  strcpy(reinterpret_cast<char*>(newNameData), broadcastMessage.payload.widget.data.text);
// |                                  lengthIndex = strlen(reinterpret_cast<char*>(newNameData));
// | -                                for (iResult = 0; iResult < lengthIndex; ++iResult) {
// | -                                    if (!((newNameData[iResult] >= 'A'
// | -                                           && newNameData[iResult] <= 'Z')
// | -                                          || (newNameData[iResult] >= 'a'
// | -                                              && newNameData[iResult] <= 'z')
// | -                                          || (newNameData[iResult] >= '0'
// | -                                              && newNameData[iResult] <= '9')
// | -                                          || (newNameData[iResult] >= CP1251_UPPER_FIRST
// | -                                              && newNameData[iResult] <= CP1251_UPPER_LAST)
// | -                                          || (newNameData[iResult] >= CP1251_LOWER_FIRST
// | -                                              && newNameData[iResult] <= CP1251_LOWER_LAST)
// | -                                          || newNameData[iResult] == CP1251_YO_UPPER
// | -                                          || newNameData[iResult] == CP1251_YO_LOWER
// | -                                          || newNameData[iResult] == '_'
// | -                                          || newNameData[iResult] == ' '
// | +                                for (characterIndex = 0; characterIndex < lengthIndex; ++characterIndex) {
// | +                                    if (!((newNameData[characterIndex] >= 'A'
// | +                                           && newNameData[characterIndex] <= 'Z')
// | +                                          || (newNameData[characterIndex] >= 'a'
// | +                                              && newNameData[characterIndex] <= 'z')
// | +                                          || (newNameData[characterIndex] >= '0'
// | +                                              && newNameData[characterIndex] <= '9')
// | +                                          || (newNameData[characterIndex] >= CP1251_UPPER_FIRST
// | +                                              && newNameData[characterIndex] <= CP1251_UPPER_LAST)
// | +                                          || (newNameData[characterIndex] >= CP1251_LOWER_FIRST
// | +                                              && newNameData[characterIndex] <= CP1251_LOWER_LAST)
// | +                                          || newNameData[characterIndex] == CP1251_YO_UPPER
// | +                                          || newNameData[characterIndex] == CP1251_YO_LOWER
// | +                                          || newNameData[characterIndex] == '_'
// | +                                          || newNameData[characterIndex] == ' '
// |                                            || FindToken(
// |                                                   "$%'-_@~`!(){}^#&+,;=[].",
// | -                                                 newNameData[iResult]
// | +                                                 newNameData[characterIndex]
// |                                               ) != NULL)) {
// | -                                        newNameData[iResult] = 0;
// | +                                        newNameData[characterIndex] = 0;
// |                                      }
// |                                  }
// | -                                for (iResult = strlen(reinterpret_cast<char*>(newNameData)) - 1; iResult >= 0; --iResult) {
// | -                                    if (newNameData[iResult] == ' ')
// | -                                        newNameData[iResult] = 0;
// | +                                for (characterIndex = strlen(reinterpret_cast<char*>(newNameData)) - 1; characterIndex >= 0; --characterIndex) {
// | +                                    if (newNameData[characterIndex] == ' ')
// | +                                        newNameData[characterIndex] = 0;
// |                                      else
// | -                                        iResult = -1;
// | +                                        characterIndex = -1;
// |                                  }
// |                                  if (strlen(reinterpret_cast<char*>(newNameData)) > 0 && newNameData[0] > ' ') {
// |                                      m_selectedIndex = FILE_REQUESTER_SELECTION_NONE;
// | @@ -348,7 +352,7 @@
// |                                      && message.payload.widget.id
// |                                             <= FILE_REQUESTER_MAP_SIZE_ICON_FIRST
// |                                                    + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// | -                                    iResult = message.payload.widget.id
// | +                                    rowIndex = message.payload.widget.id
// |                                                - FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
// |                                      goto SelectListItem;
// |                                  }
// | @@ -357,7 +361,7 @@
// |                                      && message.payload.widget.id
// |                                             <= FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
// |                                                    + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// | -                                    iResult = message.payload.widget.id
// | +                                    rowIndex = message.payload.widget.id
// |                                                - FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
// |                                      goto SelectListItem;
// |                                  }
// | @@ -366,7 +370,7 @@
// |                                      && message.payload.widget.id
// |                                             <= FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
// |                                                    + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// | -                                    iResult = message.payload.widget.id
// | +                                    rowIndex = message.payload.widget.id
// |                                                - FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
// |                                      goto SelectListItem;
// |                                  }
// | @@ -374,7 +378,7 @@
// |                                      && message.payload.widget.id
// |                                             <= FILE_REQUESTER_MAP_LOSS_ICON_FIRST
// |                                                    + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// | -                                    iResult = message.payload.widget.id
// | +                                    rowIndex = message.payload.widget.id
// |                                                - FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
// |                                      goto SelectListItem;
// |                                  }
// | @@ -382,21 +386,21 @@
// |                                      && message.payload.widget.id
// |                                             <= FILE_REQUESTER_LIST_TEXT_FIRST
// |                                                    + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
// | -                                    iResult =
// | +                                    rowIndex =
// |                                          message.payload.widget.id - FILE_REQUESTER_LIST_TEXT_FIRST;
// |                                      goto SelectListItem;
// |                                  }
// |                                  break;
// |                              SelectListItem:
// | -                                if (iResult + m_topIndex == m_selectedIndex) {
// | +                                if (rowIndex + m_topIndex == m_selectedIndex) {
// |                                      message.payload.widget.data.value = FILE_REQUESTER_OK;
// |                                      message.payload.widget.id = FILE_REQUESTER_OK;
// |                                      acceptStep = true;
// |                                      break;
// |                                  }
// | -                                if (iResult + m_topIndex >= m_fileCount)
// | +                                if (rowIndex + m_topIndex >= m_fileCount)
// |                                      break;
// | -                                m_selectedIndex = iResult + m_topIndex;
// | +                                m_selectedIndex = rowIndex + m_topIndex;
// |                                  SetOK(1);
// |                                  Update(1);
// |                                  break;
// | @@ -413,9 +417,9 @@
// |              && message.payload.widget.data.value != FILE_REQUESTER_CANCEL
// |              && strcmpi(m_extensions[m_selectedIndex].text, ".GMC") != 0
// |              && strcmpi(m_extensions[m_selectedIndex].text, ".GXC") != 0) {
// | -            iResult =
// | +            playerCount =
// |                  m_extensions[m_selectedIndex].text[FILE_REQUESTER_EXTENSION_PLAYER_DIGIT] - '0';
// | -            if (iResult < giNumHumanPlayers
// | +            if (playerCount < giNumHumanPlayers
// |                  && giDebugLevel < FILE_REQUESTER_DEBUG_ALLOW_PLAYER_MISMATCH) {
// |                  sprintf(
// |                      gText,
// | @@ -427,7 +431,7 @@
// |                      /* "Выбранная вами игра рассчитана только на %d человек.  А вам нужна
// |                         карта, как минимум на %d человек." */
// |                      ,
// | -                    iResult,
// | +                    playerCount,
// |                      giNumHumanPlayers
// |                  );
// |                  NormalDialog(
// | @@ -444,7 +448,7 @@
// |                  );
// |                  acceptStep = false;
// |              }
// | -            if (iResult > giNumHumanPlayers) {
// | +            if (playerCount > giNumHumanPlayers) {
// |                  sprintf(
// |                      gText,
// |                      "\xc2\xfb\xe1\xf0\xe0\xed\xed\xe0\xff \xe8\xe3\xf0\xe0 \xed\xe0\xf7\xed\xe5\xf2"
// | @@ -456,6 +460,6 @@
// |                      /* "Выбранная игра начнется с %d игроками-людьми. Можно ли
// |                         компьютеру взять под свое управление оставшиеся %d мест людей?" */
// |                      ,
// | -                    iResult,
// | -                    iResult - giNumHumanPlayers
// | +                    playerCount,
// | +                    playerCount - giNumHumanPlayers
// |                  );
