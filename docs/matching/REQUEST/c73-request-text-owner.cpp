// VC6 SP5 REQUEST source family c73-request-text-owner; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c73-request-text-owner/results.json; RVA 0x8f737
// Complete 3 arms; elapsed 2.237443s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 4388 / a96d6354363d97b7 / b5daded3af5efd4c / 195:196 / 194,0,0,1 / False
// 1 / {'source_family': 'unsigned_casts', 'candidate': 'baseline'} / 4387 / 167a7f7d594693e6 / 6c4bbec14bf5ec7e / 195:196 / 193,1,0,1 / False
// 2 / {'source_family': 'character_value', 'candidate': 'baseline'} / 4356 / 7aafeb6e3247c46e / 103bd5b8c7e90070 / 195:196 / 178,16,0,1 / False

// AXIS source_family
// BEFORE (complete exact span):
// | MessageDispatchResult fileRequester::Main(struct tag_message& message) {
// |     u8 newNameData[FILE_REQUESTER_LOCAL_NAME_SIZE];
// |     i32 screenY;
// |     i32 H2_UNUSED(mouseX);
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
// ALTERNATIVE unsigned_casts (unified source delta from BEFORE):
// | --- before
// | +++ unsigned_casts
// | @@ -1,5 +1,5 @@
// |  MessageDispatchResult fileRequester::Main(struct tag_message& message) {
// | -    u8 newNameData[FILE_REQUESTER_LOCAL_NAME_SIZE];
// | +    char newNameData[FILE_REQUESTER_LOCAL_NAME_SIZE];
// |      i32 screenY;
// |      i32 H2_UNUSED(mouseX);
// |      b32 acceptStep = false;
// | @@ -279,37 +279,37 @@
// |                                  m_window->BroadcastMessage(broadcastMessage);
// |
// |                                  memset(newNameData, 0, FILE_REQUESTER_FILENAME_INITIAL_CLEAR_SIZE);
// | -                                strcpy(reinterpret_cast<char*>(newNameData), broadcastMessage.payload.widget.data.text);
// | -                                lengthIndex = strlen(reinterpret_cast<char*>(newNameData));
// | +                                strcpy(newNameData, broadcastMessage.payload.widget.data.text);
// | +                                lengthIndex = strlen(newNameData);
// |                                  for (iResult = 0; iResult < lengthIndex; ++iResult) {
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
// | +                                    if (!((static_cast<u8>(newNameData[iResult]) >= 'A'
// | +                                           && static_cast<u8>(newNameData[iResult]) <= 'Z')
// | +                                          || (static_cast<u8>(newNameData[iResult]) >= 'a'
// | +                                              && static_cast<u8>(newNameData[iResult]) <= 'z')
// | +                                          || (static_cast<u8>(newNameData[iResult]) >= '0'
// | +                                              && static_cast<u8>(newNameData[iResult]) <= '9')
// | +                                          || (static_cast<u8>(newNameData[iResult]) >= CP1251_UPPER_FIRST
// | +                                              && static_cast<u8>(newNameData[iResult]) <= CP1251_UPPER_LAST)
// | +                                          || (static_cast<u8>(newNameData[iResult]) >= CP1251_LOWER_FIRST
// | +                                              && static_cast<u8>(newNameData[iResult]) <= CP1251_LOWER_LAST)
// | +                                          || static_cast<u8>(newNameData[iResult]) == CP1251_YO_UPPER
// | +                                          || static_cast<u8>(newNameData[iResult]) == CP1251_YO_LOWER
// | +                                          || static_cast<u8>(newNameData[iResult]) == '_'
// | +                                          || static_cast<u8>(newNameData[iResult]) == ' '
// |                                            || FindToken(
// |                                                   "$%'-_@~`!(){}^#&+,;=[].",
// | -                                                 newNameData[iResult]
// | +                                                 static_cast<u8>(newNameData[iResult])
// |                                               ) != NULL)) {
// |                                          newNameData[iResult] = 0;
// |                                      }
// |                                  }
// | -                                for (iResult = strlen(reinterpret_cast<char*>(newNameData)) - 1; iResult >= 0; --iResult) {
// | +                                for (iResult = strlen(newNameData) - 1; iResult >= 0; --iResult) {
// |                                      if (newNameData[iResult] == ' ')
// |                                          newNameData[iResult] = 0;
// |                                      else
// |                                          iResult = -1;
// |                                  }
// | -                                if (strlen(reinterpret_cast<char*>(newNameData)) > 0 && newNameData[0] > ' ') {
// | +                                if (strlen(newNameData) > 0 && static_cast<u8>(newNameData[0]) > ' ') {
// |                                      m_selectedIndex = FILE_REQUESTER_SELECTION_NONE;
// | -                                    strcpy(m_filename, reinterpret_cast<char*>(newNameData));
// | +                                    strcpy(m_filename, newNameData);
// |                                      SetOK(1);
// ALTERNATIVE character_value (unified source delta from BEFORE):
// | --- before
// | +++ character_value
// | @@ -1,5 +1,5 @@
// |  MessageDispatchResult fileRequester::Main(struct tag_message& message) {
// | -    u8 newNameData[FILE_REQUESTER_LOCAL_NAME_SIZE];
// | +    char newNameData[FILE_REQUESTER_LOCAL_NAME_SIZE];
// |      i32 screenY;
// |      i32 H2_UNUSED(mouseX);
// |      b32 acceptStep = false;
// | @@ -279,37 +279,38 @@
// |                                  m_window->BroadcastMessage(broadcastMessage);
// |
// |                                  memset(newNameData, 0, FILE_REQUESTER_FILENAME_INITIAL_CLEAR_SIZE);
// | -                                strcpy(reinterpret_cast<char*>(newNameData), broadcastMessage.payload.widget.data.text);
// | -                                lengthIndex = strlen(reinterpret_cast<char*>(newNameData));
// | +                                strcpy(newNameData, broadcastMessage.payload.widget.data.text);
// | +                                lengthIndex = strlen(newNameData);
// |                                  for (iResult = 0; iResult < lengthIndex; ++iResult) {
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
// | +                                    u8 characterValue = static_cast<u8>(newNameData[iResult]);
// | +                                    if (!((characterValue >= 'A'
// | +                                           && characterValue <= 'Z')
// | +                                          || (characterValue >= 'a'
// | +                                              && characterValue <= 'z')
// | +                                          || (characterValue >= '0'
// | +                                              && characterValue <= '9')
// | +                                          || (characterValue >= CP1251_UPPER_FIRST
// | +                                              && characterValue <= CP1251_UPPER_LAST)
// | +                                          || (characterValue >= CP1251_LOWER_FIRST
// | +                                              && characterValue <= CP1251_LOWER_LAST)
// | +                                          || characterValue == CP1251_YO_UPPER
// | +                                          || characterValue == CP1251_YO_LOWER
// | +                                          || characterValue == '_'
// | +                                          || characterValue == ' '
// |                                            || FindToken(
// |                                                   "$%'-_@~`!(){}^#&+,;=[].",
// | -                                                 newNameData[iResult]
// | +                                                 characterValue
// |                                               ) != NULL)) {
// |                                          newNameData[iResult] = 0;
// |                                      }
// |                                  }
// | -                                for (iResult = strlen(reinterpret_cast<char*>(newNameData)) - 1; iResult >= 0; --iResult) {
// | +                                for (iResult = strlen(newNameData) - 1; iResult >= 0; --iResult) {
// |                                      if (newNameData[iResult] == ' ')
// |                                          newNameData[iResult] = 0;
// |                                      else
// |                                          iResult = -1;
// |                                  }
// | -                                if (strlen(reinterpret_cast<char*>(newNameData)) > 0 && newNameData[0] > ' ') {
// | +                                if (strlen(newNameData) > 0 && static_cast<u8>(newNameData[0]) > ' ') {
// |                                      m_selectedIndex = FILE_REQUESTER_SELECTION_NONE;
// | -                                    strcpy(m_filename, reinterpret_cast<char*>(newNameData));
// | +                                    strcpy(m_filename, newNameData);
// |                                      SetOK(1);
