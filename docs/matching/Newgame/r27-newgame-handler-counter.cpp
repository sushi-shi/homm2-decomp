// VC6 SP5 source family r27-newgame-handler-counter; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/r27-newgame-handler-counter/results.json; RVA 0x7734a
// Complete 2 arms; elapsed 1.824252s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 3537 / a0a83514955879df / d917a82cb0289086 / 161:161 / 160,1,0,0 / True
// 1 / {'family': 'separate_chat_role', 'candidate': 'baseline'} / 3537 / 54d08421dcc38081 / da8573cd3d2ea607 / 161:161 / 160,1,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0xdd1)
// |     MessageDispatchResult NewGameHandler(struct tag_message& message) {
// |         i32 sendResult;
// |         i32 oldNetworkId;
// |         i32 swapPlayerTemp;
// |         i32 currentPlayerLocal;
// |         tag_message windowMessage;
// |         b32 redraw = false;
// |         b32 needSync = false;
// |         SMapHeader mapHeader;
// |         NewGameRemotePacket* remotePacketResult;
// |         i32 sender;
// |         char mapPacketLocal[GAME_MAP_PACKET_SIZE];
// |         tag_message mapWindowMessageTemp;
// |         i32 helpDialogIndexLocal;
// |         i32 H2_UNUSED(unusedSender);
// |         char mapNamePacket[MAP_HEADER_NAME_SIZE + GAME_SETUP_BUFFER_SIZE];
// |
// |         if (!gbNewGameShadowHidden) {
// |             gbNewGameShadowHidden = true;
// |             windowMessage.type = MESSAGE_WIDGET;
// |             windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
// |             windowMessage.payload.widget.id = NEW_GAME_SHADOW;
// |             windowMessage.payload.widget.data.value = GAME_SHADOW_FRAME;
// |             gpGame->m_newGameWindow->BroadcastMessage(windowMessage);
// |         }
// |
// |         if (message.type == MESSAGE_NONE) {
// |             remotePacketResult = reinterpret_cast<NewGameRemotePacket*>(GetRemoteData(1));
// |             if (remotePacketResult != NULL
// |                 && (remotePacketResult->type == REMOTE_MESSAGE_RELIABLE
// |                     || remotePacketResult->type == REMOTE_MESSAGE_UNRELIABLE)) {
// |                 switch (remotePacketResult->command) {
// |                     case GAME_REMOTE_START:
// |                         gpWindowManager->m_dialogResult = message.payload.widget.id;
// |                         gpWindowManager->m_dialogResult = GAME_DIALOG_OK;
// |                         message.type = MESSAGE_WIDGET;
// |                         message.payload.widget.id = GAME_DIALOG_CLOSE_MESSAGE;
// |                         message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
// |                         return MESSAGE_DISPATCH_FORWARD;
// |
// |                     case GAME_REMOTE_CANCEL:
// |                         NormalDialog(
// |                             "\xd1\xe5\xf0\xe2\xe5\xf0 \xef\xf0\xe5\xea\xf0\xe0\xf2"
// |                             "\xe8\xeb \xe8\xe3\xf0\xf3.",
// |                             NORMAL_DIALOG_INFO,
// |                             -1,
// |                             -1,
// |                             -1,
// |                             0,
// |                             -1,
// |                             0,
// |                             -1,
// |                             0
// |                         );
// |                         ShutDown(NULL);
// |                         break;
// |
// |                     case GAME_REMOTE_SETUP:
// |                         if (strcmp(remotePacketResult->payload, gpGame->m_mapHeader.name) != 0)
// |                             break;
// |                         memcpy(
// |                             gpGame->m_setupPlayerColor,
// |                             remotePacketResult->payload + MAP_HEADER_NAME_SIZE,
// |                             GAME_SETUP_DATA_SIZE
// |                         );
// |                         redraw = true;
// |                         break;
// |
// |                     case GAME_REMOTE_MAP_HEADER:
// |                         memset(&mapHeader, 0, sizeof(mapHeader));
// |                         memcpy(
// |                             &mapHeader,
// |                             remotePacketResult->payload,
// |                             GAME_MAP_PACKET_SIZE
// |                         );
// |                         gpGame->ProcessNewMap(&mapHeader);
// |                         break;
// |
// |                     case GAME_REMOTE_CHAT:
// |                         redraw = true;
// |                         sender = remotePacketResult->sender;
// |                         if (sender >= 0) {
// |                             sprintf(
// |                                 gText,
// |                                 "%s:  %s",
// |                                 gsNetPlayerInfo[sender].name,
// |                                 remotePacketResult->payload
// |                             );
// |                         } else {
// |                             unusedSender = 0;
// |                         }
// |                         gText[GAME_CHAT_TEXT_LIMIT] = 0;
// |                         for (currentPlayerLocal = 0; currentPlayerLocal < GAME_CHAT_LINE_COUNT - 1;
// |                              ++currentPlayerLocal) {
// |                             strcpy(
// |                                 cTextReceivedBuffer[currentPlayerLocal],
// |                                 cTextReceivedBuffer[currentPlayerLocal + 1]
// |                             );
// |                         }
// |                         strcpy(cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1], gText);
// |                         break;
// |                 }
// |             }
// |             if (static_cast<i32>(KBTickCount()) > glTimers[0]) {
// |                 gpGame->NGKPSetupDisplayString(cNGKPCore, static_cast<u16>(NGKPcursorIndex));
// |                 gpGame->DrawNGKPDisplayString(1);
// |             }
// |         }
// |
// |         if (message.type == MESSAGE_KEY_DOWN && giNumHumanPlayers > 1
// |             && iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT && gpGame->ProcessNGKeyPress(message)) {
// |             redraw = true;
// |             for (currentPlayerLocal = 0; currentPlayerLocal < GAME_CHAT_LINE_COUNT - 1;
// |                  ++currentPlayerLocal) {
// |                 strcpy(
// |                     cTextReceivedBuffer[currentPlayerLocal],
// |                     cTextReceivedBuffer[currentPlayerLocal + 1]
// |                 );
// |             }
// |             strcpy(cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1], cNGKPCore);
// |             strcpy(
// |                 cNGKPCore,
// |                 ""
// |             );
// |             strcpy(
// |                 cNGKPDisplay,
// |                 ""
// |             );
// |             NGKPcursorIndex = 0;
// |             sendResult = TransmitRemoteData(
// |                 cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1],
// |                 GAME_REMOTE_CHANNEL,
// |                 strlen(cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1]) + 1,
// |                 GAME_REMOTE_CHAT,
// |                 1,
// |                 1,
// |                 REMOTE_MESSAGE_DEFAULT
// |             );
// |             if (!sendResult)
// |                 ShutDown(NULL);
// |         }
// |
// |         if (message.type == MESSAGE_WIDGET) {
// |             if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) {
// |                 if (message.payload.widget.command == NEW_GAME_EVENT_PRESS
// |                     || message.payload.widget.command == NEW_GAME_EVENT_ALTERNATE_PRESS) {
// |                     helpDialogIndexLocal = -1;
// |                     if ((message.payload.widget.id >= NEW_GAME_DIFFICULTY_HELP_FIRST
// |                          && message.payload.widget.id
// |                                 <= NEW_GAME_DIFFICULTY_HELP_FIRST + IDX(DIFFICULTY_COUNT) - 1)
// |                         || (message.payload.widget.id >= NEW_GAME_DIFFICULTY_FIRST
// |                             && message.payload.widget.id
// |                                    <= NEW_GAME_DIFFICULTY_FIRST + IDX(DIFFICULTY_COUNT) - 1))
// |                         helpDialogIndexLocal = GAME_HELP_DIFFICULTY;
// |                     if ((message.payload.widget.id >= NEW_GAME_HANDICAP_FIRST
// |                          && message.payload.widget.id
// |                                 <= NEW_GAME_HANDICAP_FIRST + IDX(GAME_PLAYER_CONTROL_COUNT) - 1)
// |                         || (message.payload.widget.id >= NEW_GAME_PLAYER_HUMAN_FIRST
// |                             && message.payload.widget.id <= NEW_GAME_PLAYER_HUMAN_FIRST
// |                                                                 + IDX(GAME_PLAYER_CONTROL_COUNT)
// |                                                                 - 1))
// |                         helpDialogIndexLocal = GAME_HELP_HANDICAP;
// |                     if ((message.payload.widget.id >= NEW_GAME_COLOR_FIRST
// |                          && message.payload.widget.id
// |                                 <= NEW_GAME_COLOR_FIRST + IDX(GAME_PLAYER_CONTROL_COUNT) - 1)
// |                         || (message.payload.widget.id >= NEW_GAME_RACE_FIRST
// |                             && message.payload.widget.id
// |                                    <= NEW_GAME_RACE_FIRST + IDX(GAME_PLAYER_CONTROL_COUNT) - 1)
// |                         || (message.payload.widget.id >= NEW_GAME_PLAYER_SELECT_FIRST
// |                             && message.payload.widget.id <= NEW_GAME_PLAYER_NAME_FIRST)
// |                         || (message.payload.widget.id >= NEW_GAME_PLAYER_NAME_FIRST
// |                             && message.payload.widget.id <= NEW_GAME_RACE_ICON_FIRST))
// |                         helpDialogIndexLocal = GAME_HELP_PLAYER;
// |                     if ((message.payload.widget.id >= NEW_GAME_RACE_CYCLE_FIRST
// |                          && message.payload.widget.id
// |                                 <= NEW_GAME_RACE_CYCLE_FIRST + IDX(GAME_PLAYER_CONTROL_COUNT) - 1)
// |                         || (message.payload.widget.id >= NEW_GAME_RACE_ICON_FIRST
// |                             && message.payload.widget.id <= NEW_GAME_RACE_ICON_FIRST
// |                                                                 + IDX(GAME_PLAYER_CONTROL_COUNT)
// |                                                                 - 1))
// |                         helpDialogIndexLocal = GAME_HELP_RACE;
// |                     if (message.payload.widget.id == GAME_MAP_OPTIONS_CONTROL
// |                         || message.payload.widget.id == NEW_GAME_MAP_SELECT
// |                         || message.payload.widget.id == NEW_GAME_SCENARIO_NAME)
// |                         helpDialogIndexLocal = GAME_HELP_MAP;
// |                     if (message.payload.widget.id == NEW_GAME_RATING)
// |                         helpDialogIndexLocal = GAME_HELP_RATING;
// |                     if (message.payload.widget.id == GAME_DIALOG_OK)
// |                         helpDialogIndexLocal = GAME_HELP_OK;
// |                     if (message.payload.widget.id == GAME_DIALOG_CANCEL)
// |                         helpDialogIndexLocal = GAME_HELP_CANCEL;
// |                     if (helpDialogIndexLocal != -1)
// |                         NormalDialog(
// |                             gNewGameHelp[helpDialogIndexLocal],
// |                             NEW_GAME_HELP_DIALOG_TYPE,
// |                             -1,
// |                             -1,
// |                             -1,
// |                             0,
// |                             -1,
// |                             0,
// |                             -1,
// |                             0
// |                         );
// |                 }
// |             } else {
// |                 switch (message.payload.widget.command) {
// |                     case NEW_GAME_EVENT_RELEASE:
// |                         switch (message.payload.widget.id) {
// |                             case GAME_DIALOG_OK:
// |                                 if (gbRemoteOn) {
// |                                     sendResult = TransmitRemoteData(
// |                                         NULL,
// |                                         GAME_REMOTE_CHANNEL,
// |                                         0,
// |                                         GAME_REMOTE_START,
// |                                         1,
// |                                         1,
// |                                         REMOTE_MESSAGE_DEFAULT
// |                                     );
// |                                 }
// |                                 gpWindowManager->m_dialogResult = message.payload.widget.id;
// |                                 message.payload.widget.id = GAME_DIALOG_CLOSE_MESSAGE;
// |                                 message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
// |                                 gbNewGameDialogOver = true;
// |                                 return MESSAGE_DISPATCH_FORWARD;
// |
// |                             case GAME_DIALOG_CANCEL:
// |                                 if (gbRemoteOn) {
// |                                     sendResult = TransmitRemoteData(
// |                                         NULL,
// |                                         GAME_REMOTE_CHANNEL,
// |                                         0,
// |                                         GAME_REMOTE_CANCEL,
// |                                         1,
// |                                         1,
// |                                         REMOTE_MESSAGE_DEFAULT
// |                                     );
// |                                     ShutDown(NULL);
// |                                 }
// |                                 gpWindowManager->m_dialogResult = message.payload.widget.id;
// |                                 message.payload.widget.id = GAME_DIALOG_CLOSE_MESSAGE;
// |                                 message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
// |                                 gbNewGameDialogOver = true;
// |                                 return MESSAGE_DISPATCH_FORWARD;
// |
// |                             case GAME_MAP_OPTIONS_CONTROL:
// |                                 goto chooseMap;
// |
// |                             default:
// |                                 break;
// |                         }
// |                         break;
// |
// |                     case NEW_GAME_EVENT_PRESS:
// |                         switch (message.payload.widget.id) {
// |                             case NEW_GAME_DIFFICULTY_HELP_FIRST + IDX(DIFFICULTY_EASY):
// |                             case NEW_GAME_DIFFICULTY_HELP_FIRST + IDX(DIFFICULTY_NORMAL):
// |                             case NEW_GAME_DIFFICULTY_HELP_FIRST + IDX(DIFFICULTY_HARD):
// |                             case NEW_GAME_DIFFICULTY_HELP_FIRST + IDX(DIFFICULTY_EXPERT):
// |                             case NEW_GAME_DIFFICULTY_HELP_FIRST + IDX(DIFFICULTY_IMPOSSIBLE):
// |                                 currentPlayerLocal =
// |                                     message.payload.widget.id - NEW_GAME_DIFFICULTY_HELP_FIRST;
// |                                 goto setDifficulty;
// |
// |                             case NEW_GAME_DIFFICULTY_FIRST + IDX(DIFFICULTY_EASY):
// |                             case NEW_GAME_DIFFICULTY_FIRST + IDX(DIFFICULTY_NORMAL):
// |                             case NEW_GAME_DIFFICULTY_FIRST + IDX(DIFFICULTY_HARD):
// |                             case NEW_GAME_DIFFICULTY_FIRST + IDX(DIFFICULTY_EXPERT):
// |                             case NEW_GAME_DIFFICULTY_FIRST + IDX(DIFFICULTY_IMPOSSIBLE):
// |                                 currentPlayerLocal =
// |                                     message.payload.widget.id - NEW_GAME_DIFFICULTY_FIRST;
// |                             setDifficulty:
// |                                 gpGame->m_difficulty =
// |                                     static_cast<GameDifficulty>(currentPlayerLocal);
// |                                 needSync = true;
// |                                 redraw = true;
// |                                 break;
// |
// |                             case NEW_GAME_HANDICAP_FIRST + IDX(PLAYER_SLOT_FIRST):
// |                             case NEW_GAME_HANDICAP_FIRST + IDX(PLAYER_SLOT_SECOND):
// |                             case NEW_GAME_HANDICAP_FIRST + IDX(PLAYER_SLOT_THIRD):
// |                             case NEW_GAME_HANDICAP_FIRST + IDX(PLAYER_SLOT_FOURTH):
// |                             case NEW_GAME_HANDICAP_FIRST + IDX(PLAYER_SLOT_FIFTH):
// |                             case NEW_GAME_HANDICAP_FIRST + IDX(PLAYER_SLOT_SIXTH):
// |                                 currentPlayerLocal =
// |                                     message.payload.widget.id - NEW_GAME_HANDICAP_FIRST;
// |                                 goto cycleHandicap;
// |
// |                             case NEW_GAME_PLAYER_HUMAN_FIRST + IDX(PLAYER_SLOT_FIRST):
// |                             case NEW_GAME_PLAYER_HUMAN_FIRST + IDX(PLAYER_SLOT_SECOND):
// |                             case NEW_GAME_PLAYER_HUMAN_FIRST + IDX(PLAYER_SLOT_THIRD):
// |                             case NEW_GAME_PLAYER_HUMAN_FIRST + IDX(PLAYER_SLOT_FOURTH):
// |                             case NEW_GAME_PLAYER_HUMAN_FIRST + IDX(PLAYER_SLOT_FIFTH):
// |                             case NEW_GAME_PLAYER_HUMAN_FIRST + IDX(PLAYER_SLOT_SIXTH):
// |                                 currentPlayerLocal =
// |                                     message.payload.widget.id - NEW_GAME_PLAYER_HUMAN_FIRST;
// |                             cycleHandicap:
// |                                 needSync = true;
// |                                 redraw = true;
// |                                 if (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
// |                                     != GAME_COMPUTER_PLAYER) {
// |                                     gpGame->m_playerHandicap[currentPlayerLocal] = PlayerHandicap(
// |                                         (IDX(gpGame->m_playerHandicap[currentPlayerLocal]) + 1)
// |                                         % IDX(PLAYER_HANDICAP_COUNT)
// |                                     );
// |                                 }
// |                                 break;
// |
// |                             case NEW_GAME_RACE_FIRST + IDX(PLAYER_SLOT_FIRST):
// |                             case NEW_GAME_RACE_FIRST + IDX(PLAYER_SLOT_SECOND):
// |                             case NEW_GAME_RACE_FIRST + IDX(PLAYER_SLOT_THIRD):
// |                             case NEW_GAME_RACE_FIRST + IDX(PLAYER_SLOT_FOURTH):
// |                             case NEW_GAME_RACE_FIRST + IDX(PLAYER_SLOT_FIFTH):
// |                             case NEW_GAME_RACE_FIRST + IDX(PLAYER_SLOT_SIXTH):
// |                                 currentPlayerLocal =
// |                                     message.payload.widget.id - NEW_GAME_RACE_FIRST;
// |                                 goto selectPlayer;
// |
// |                             case NEW_GAME_COLOR_FIRST + IDX(PLAYER_SLOT_FIRST):
// |                             case NEW_GAME_COLOR_FIRST + IDX(PLAYER_SLOT_SECOND):
// |                             case NEW_GAME_COLOR_FIRST + IDX(PLAYER_SLOT_THIRD):
// |                             case NEW_GAME_COLOR_FIRST + IDX(PLAYER_SLOT_FOURTH):
// |                             case NEW_GAME_COLOR_FIRST + IDX(PLAYER_SLOT_FIFTH):
// |                             case NEW_GAME_COLOR_FIRST + IDX(PLAYER_SLOT_SIXTH):
// |                                 currentPlayerLocal =
// |                                     message.payload.widget.id - NEW_GAME_COLOR_FIRST;
// |                                 goto selectPlayer;
// |
// |                             case NEW_GAME_PLAYER_SELECT_FIRST + IDX(PLAYER_SLOT_FIRST):
// |                             case NEW_GAME_PLAYER_SELECT_FIRST + IDX(PLAYER_SLOT_SECOND):
// |                             case NEW_GAME_PLAYER_SELECT_FIRST + IDX(PLAYER_SLOT_THIRD):
// |                             case NEW_GAME_PLAYER_SELECT_FIRST + IDX(PLAYER_SLOT_FOURTH):
// |                             case NEW_GAME_PLAYER_SELECT_FIRST + IDX(PLAYER_SLOT_FIFTH):
// |                             case NEW_GAME_PLAYER_SELECT_FIRST + IDX(PLAYER_SLOT_SIXTH):
// |                                 currentPlayerLocal =
// |                                     message.payload.widget.id - NEW_GAME_PLAYER_SELECT_FIRST;
// |                                 goto selectPlayer;
// |
// |                             case NEW_GAME_PLAYER_NAME_FIRST + IDX(PLAYER_SLOT_FIRST):
// |                             case NEW_GAME_PLAYER_NAME_FIRST + IDX(PLAYER_SLOT_SECOND):
// |                             case NEW_GAME_PLAYER_NAME_FIRST + IDX(PLAYER_SLOT_THIRD):
// |                             case NEW_GAME_PLAYER_NAME_FIRST + IDX(PLAYER_SLOT_FOURTH):
// |                             case NEW_GAME_PLAYER_NAME_FIRST + IDX(PLAYER_SLOT_FIFTH):
// |                             case NEW_GAME_PLAYER_NAME_FIRST + IDX(PLAYER_SLOT_SIXTH):
// |                                 currentPlayerLocal =
// |                                     message.payload.widget.id - NEW_GAME_PLAYER_NAME_FIRST;
// |                             selectPlayer:
// |                                 needSync = true;
// |                                 redraw = true;
// |                                 if (gpGame->m_setupPlayerType[currentPlayerLocal]
// |                                         != GAME_PLAYER_DEFAULT
// |                                     || (giNumHumanPlayers > 1
// |                                         && gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
// |                                                != GAME_COMPUTER_PLAYER)) {
// |                                     if (giNumHumanPlayers == 1) {
// |                                         if (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
// |                                             == GAME_COMPUTER_PLAYER) {
// |                                             for (swapPlayerTemp = 0;
// |                                                  swapPlayerTemp < gpGame->m_mapHeader.playerCount;
// |                                                  ++swapPlayerTemp) {
// |                                                 if (gpGame->m_setupPlayerNetworkId[swapPlayerTemp]
// |                                                     != GAME_COMPUTER_PLAYER) {
// |                                                     oldNetworkId = gpGame->m_setupPlayerNetworkId
// |                                                                        [swapPlayerTemp];
// |                                                     gpGame->m_setupPlayerNetworkId[swapPlayerTemp] =
// |                                                         gpGame->m_setupPlayerNetworkId
// |                                                             [currentPlayerLocal];
// |                                                     gpGame->m_setupPlayerNetworkId
// |                                                         [currentPlayerLocal] = oldNetworkId;
// |                                                     swapPlayerTemp = GAME_SWAP_SEARCH_DONE;
// |                                                 }
// |                                             }
// |                                         }
// |                                     } else if (gpGame->m_selectedSetupPlayer
// |                                                == GAME_NETWORK_PLAYER_NONE) {
// |                                         gpGame->m_selectedSetupPlayer =
// |                                             static_cast<i8>(currentPlayerLocal);
// |                                     } else if (gpGame->m_selectedSetupPlayer == currentPlayerLocal
// |                                                || (gpGame->m_setupPlayerNetworkId
// |                                                            [currentPlayerLocal]
// |                                                        == GAME_COMPUTER_PLAYER
// |                                                    && gpGame->m_setupPlayerNetworkId
// |                                                               [gpGame->m_selectedSetupPlayer]
// |                                                           == GAME_COMPUTER_PLAYER)) {
// |                                         gpGame->m_selectedSetupPlayer = GAME_NETWORK_PLAYER_NONE;
// |                                     } else {
// |                                         if ((gpGame->m_setupPlayerType[currentPlayerLocal]
// |                                                  != GAME_PLAYER_DEFAULT
// |                                              && gpGame->m_setupPlayerType
// |                                                         [gpGame->m_selectedSetupPlayer]
// |                                                     != GAME_PLAYER_DEFAULT)
// |                                             || (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
// |                                                     != GAME_COMPUTER_PLAYER
// |                                                 && gpGame->m_setupPlayerNetworkId
// |                                                            [gpGame->m_selectedSetupPlayer]
// |                                                        != GAME_COMPUTER_PLAYER)) {
// |                                             swapPlayerTemp =
// |                                                 gpGame->m_setupPlayerNetworkId[currentPlayerLocal];
// |                                             gpGame->m_setupPlayerNetworkId[currentPlayerLocal] =
// |                                                 gpGame->m_setupPlayerNetworkId
// |                                                     [gpGame->m_selectedSetupPlayer];
// |                                             gpGame->m_setupPlayerNetworkId
// |                                                 [gpGame->m_selectedSetupPlayer] =
// |                                                 static_cast<i8>(swapPlayerTemp);
// |                                         } else {
// |                                             NormalDialog(
// |                                                 "\xc4\xe2\xe5 \xe2\xfb\xe1\xf0\xe0\xed\xed\xfb"
// |                                                 "\xf5 \xef\xee\xe7\xe8\xf6\xe8\xe8 \xed\xe5 \xec"
// |                                                 "\xee\xe3\xf3\xf2 \xef\xee\xec\xe5\xed\xff\xf2"
// |                                                 "\xfc\xf1\xff \xec\xe5\xf1\xf2\xe0\xec\xe8.",
// |                                                 NORMAL_DIALOG_INFO,
// |                                                 -1,
// |                                                 -1,
// |                                                 -1,
// |                                                 0,
// |                                                 -1,
// |                                                 0,
// |                                                 -1,
// |                                                 0
// |                                             );
// |                                         }
// |                                         gpGame->m_selectedSetupPlayer = GAME_NETWORK_PLAYER_NONE;
// |                                     }
// |                                 }
// |                                 break;
// |
// |                             case NEW_GAME_RACE_CYCLE_FIRST + IDX(PLAYER_SLOT_FIRST):
// |                             case NEW_GAME_RACE_CYCLE_FIRST + IDX(PLAYER_SLOT_SECOND):
// |                             case NEW_GAME_RACE_CYCLE_FIRST + IDX(PLAYER_SLOT_THIRD):
// |                             case NEW_GAME_RACE_CYCLE_FIRST + IDX(PLAYER_SLOT_FOURTH):
// |                             case NEW_GAME_RACE_CYCLE_FIRST + IDX(PLAYER_SLOT_FIFTH):
// |                             case NEW_GAME_RACE_CYCLE_FIRST + IDX(PLAYER_SLOT_SIXTH):
// |                                 currentPlayerLocal =
// |                                     message.payload.widget.id - NEW_GAME_RACE_CYCLE_FIRST;
// |                                 goto cycleRace;
// |
// |                             case NEW_GAME_RACE_ICON_FIRST + IDX(PLAYER_SLOT_FIRST):
// |                             case NEW_GAME_RACE_ICON_FIRST + IDX(PLAYER_SLOT_SECOND):
// |                             case NEW_GAME_RACE_ICON_FIRST + IDX(PLAYER_SLOT_THIRD):
// |                             case NEW_GAME_RACE_ICON_FIRST + IDX(PLAYER_SLOT_FOURTH):
// |                             case NEW_GAME_RACE_ICON_FIRST + IDX(PLAYER_SLOT_FIFTH):
// |                             case NEW_GAME_RACE_ICON_FIRST + IDX(PLAYER_SLOT_SIXTH):
// |                                 currentPlayerLocal =
// |                                     message.payload.widget.id - NEW_GAME_RACE_ICON_FIRST;
// |                             cycleRace:
// |                                 if (gpGame->m_mapHeader
// |                                         .playerRace[gpGame->m_setupPlayerColor[currentPlayerLocal]]
// |                                     == FACTION_RANDOM) {
// |                                     if (gpGame->m_setupPlayerRace[currentPlayerLocal]
// |                                         == FACTION_RANDOM)
// |                                         gpGame->m_setupPlayerRace[currentPlayerLocal] =
// |                                             FACTION_KNIGHT;
// |                                     else if (gpGame->m_setupPlayerRace[currentPlayerLocal]
// |                                              == FACTION_NECROMANCER)
// |                                         gpGame->m_setupPlayerRace[currentPlayerLocal] =
// |                                             FACTION_RANDOM;
// |                                     else
// |                                         ++gpGame->m_setupPlayerRace[currentPlayerLocal];
// |                                     needSync = true;
// |                                     redraw = true;
// |                                 }
// |                                 break;
// |
// |                             case GAME_MAP_OPTIONS_CONTROL:
// |                                 break;
// |
// |                             case NEW_GAME_SCENARIO_NAME:
// |                             case NEW_GAME_MAP_SELECT:
// |                             chooseMap:
// |                                 if (gbRemoteOn && giThisNetPos != 0)
// |                                     break;
// |                                 {
// |                                     mapWindowMessageTemp.type = MESSAGE_WIDGET;
// |                                     mapWindowMessageTemp.payload.widget.command =
// |                                         NEW_GAME_WIDGET_DISABLE;
// |                                     mapWindowMessageTemp.payload.widget.id = GAME_DIALOG_CANCEL;
// |                                     mapWindowMessageTemp.payload.widget.data.value =
// |                                         GAME_WIDGET_INACTIVE_FRAME;
// |                                     gpGame->m_newGameWindow->BroadcastMessage(mapWindowMessageTemp);
// |                                     gpGame->GetMap();
// |                                     mapWindowMessageTemp.type = MESSAGE_WIDGET;
// |                                     mapWindowMessageTemp.payload.widget.command =
// |                                         NEW_GAME_WIDGET_ENABLE;
// |                                     mapWindowMessageTemp.payload.widget.id = GAME_DIALOG_CANCEL;
// |                                     mapWindowMessageTemp.payload.widget.data.value =
// |                                         GAME_WIDGET_INACTIVE_FRAME;
// |                                     gpGame->m_newGameWindow->BroadcastMessage(mapWindowMessageTemp);
// |                                     if (gbRemoteOn) {
// |                                         memcpy(
// |                                             mapPacketLocal,
// |                                             &gpGame->m_mapHeader,
// |                                             GAME_MAP_PACKET_SIZE
// |                                         );
// |                                         sendResult = TransmitRemoteData(
// |                                             mapPacketLocal,
// |                                             GAME_REMOTE_CHANNEL,
// |                                             GAME_MAP_PACKET_SIZE,
// |                                             GAME_REMOTE_MAP_HEADER,
// |                                             1,
// |                                             1,
// |                                             REMOTE_MESSAGE_DEFAULT
// |                                         );
// |                                     }
// |                                 }
// |                                 break;
// |
// |                             default:
// |                                 break;
// |                         }
// |                         break;
// |
// |                     default:
// |                         break;
// |                 }
// |             }
// |         }
// |
// |     if (redraw) {
// |         gpGame->UpdateNewGameWindow();
// |         gpGame->m_newGameWindow->DrawWindow();
// |     }
// |     if (needSync && gbRemoteOn) {
// |         memcpy(mapNamePacket, gpGame->m_mapHeader.name, MAP_HEADER_NAME_SIZE);
// |         memcpy(mapNamePacket + MAP_HEADER_NAME_SIZE, gpGame->m_setupPlayerColor, GAME_SETUP_DATA_SIZE);
// |         sendResult = TransmitRemoteData(
// |             mapNamePacket,
// |             GAME_REMOTE_CHANNEL,
// |             GAME_SETUP_PACKET_SIZE,
// |             GAME_REMOTE_SETUP,
// |             1,
// |             1,
// |             REMOTE_MESSAGE_DEFAULT
// |         );
// |         if (!sendResult)
// |             ShutDown(NULL);
// |     }
// |     return MESSAGE_DISPATCH_CONSUME;
// | }
// |
// ALTERNATIVE separate_chat_role (unified delta from BEFORE):
// | --- before
// | +++ separate_chat_role
// | @@ -4,6 +4,7 @@
// |          i32 oldNetworkId;
// |          i32 swapPlayerTemp;
// |          i32 currentPlayerLocal;
// | +        i32 chatLine;
// |          tag_message windowMessage;
// |          b32 redraw = false;
// |          b32 needSync = false;
// | @@ -91,11 +92,11 @@
// |                              unusedSender = 0;
// |                          }
// |                          gText[GAME_CHAT_TEXT_LIMIT] = 0;
// | -                        for (currentPlayerLocal = 0; currentPlayerLocal < GAME_CHAT_LINE_COUNT - 1;
// | -                             ++currentPlayerLocal) {
// | +                        for (chatLine = 0; chatLine < GAME_CHAT_LINE_COUNT - 1;
// | +                             ++chatLine) {
// |                              strcpy(
// | -                                cTextReceivedBuffer[currentPlayerLocal],
// | -                                cTextReceivedBuffer[currentPlayerLocal + 1]
// | +                                cTextReceivedBuffer[chatLine],
// | +                                cTextReceivedBuffer[chatLine + 1]
// |                              );
// |                          }
// |                          strcpy(cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1], gText);
// | @@ -111,11 +112,11 @@
// |          if (message.type == MESSAGE_KEY_DOWN && giNumHumanPlayers > 1
// |              && iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT && gpGame->ProcessNGKeyPress(message)) {
// |              redraw = true;
// | -            for (currentPlayerLocal = 0; currentPlayerLocal < GAME_CHAT_LINE_COUNT - 1;
// | -                 ++currentPlayerLocal) {
// | +            for (chatLine = 0; chatLine < GAME_CHAT_LINE_COUNT - 1;
// | +                 ++chatLine) {
// |                  strcpy(
// | -                    cTextReceivedBuffer[currentPlayerLocal],
// | -                    cTextReceivedBuffer[currentPlayerLocal + 1]
// | +                    cTextReceivedBuffer[chatLine],
// | +                    cTextReceivedBuffer[chatLine + 1]
// |                  );
// |              }
// |              strcpy(cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1], cNGKPCore);
