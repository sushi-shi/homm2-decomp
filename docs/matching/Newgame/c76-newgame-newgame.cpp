// VC6 SP5 source family c76-newgame-newgame; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/c76-newgame-newgame/results.json; RVA 0x75b4b
// Complete 2 arms; elapsed 1.813434s; source restored.
// Disposition: retain all tested destination/promotion cast removals; map domain keeps inner i16 truncation and uses enum constructor.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 2673 / 072be471f71f2c32 / f12e7436ced3db53 / 91:91 / 91,0,0,0 / True
// 1 / {'family': 'remove_1', 'candidate': 'baseline'} / 2673 / 072be471f71f2c32 / f7d42f632491ad03 / 91:91 / 91,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0xa71)
// | i32 game::NewGame(void) {
// |     char netPlayerPacket[GAME_PLAYER_INFO_BUFFER_SIZE];
// |     char mapInfo[GAME_MAP_PACKET_SIZE];
// |     tag_message windowMessage;
// |     b32 mapHeaderOk;
// |     b32 playerInfoOk;
// |     NewGameRemotePacket* remoteBuffer;
// |     heroWindow* choiceWindow;
// |     b32 result;
// |     b8 wrongExpansionType;
// |     char* mapExt;
// |     i32 mapHeaderRead;
// |     i32 textBufferIndex;
// |     i32 transmitResult;
// |
// |     result = true;
// |     m_newGameWindow = NULL;
// |
// |     if ((!gbRemoteOn || giThisNetPos == 0) && (!gbRemoteOn || !xNetHasOldPlayers)) {
// |         choiceWindow = new heroWindow(MAP_CHOICE_WINDOW_X, MAP_CHOICE_WINDOW_Y, "x_mapmnu.bin");
// |         if (choiceWindow == NULL)
// |             MemError();
// |         gpWindowManager->DoDialog(choiceWindow, ExpStdGameHandler, 0);
// |         delete choiceWindow;
// |         switch (static_cast<NewGameMapChoice>(static_cast<i16>(gpWindowManager->m_dialogResult))) {
// |             case MAP_CHOICE_STANDARD:
// |                 xIsExpansionMap = false;
// |                 break;
// |             case MAP_CHOICE_EXPANSION:
// |                 xIsExpansionMap = true;
// |                 break;
// |             case MAP_CHOICE_CANCEL:
// |                 return 0;
// |         }
// |     }
// |
// |     SetupNetPlayerNames();
// |     glTimers[0] = 0;
// |     for (textBufferIndex = 0; textBufferIndex < GAME_TEXT_BUFFER_COUNT; ++textBufferIndex) {
// |         cTextReceivedBuffer[textBufferIndex] =
// |             static_cast<char*>(H2_ALLOC(GAME_TEXT_BUFFER_SIZE));
// |         strcpy(
// |             cTextReceivedBuffer[textBufferIndex],
// |             ""
// |         );
// |     }
// |     cNGKPCore = static_cast<char*>(H2_ALLOC(GAME_KEY_BUFFER_SIZE));
// |     cNGKPDisplay = static_cast<char*>(H2_ALLOC(GAME_KEY_BUFFER_SIZE));
// |     strcpy(cNGKPCore, "");
// |     strcpy(cNGKPDisplay, "");
// |     NGKPcursorIndex = 0;
// |     NGKPBkg = gpResourceManager->GetIcon("ngextra.icn");
// |
// |     if (gbWaitForRemoteReceive) {
// |         mapHeaderOk = false;
// |         playerInfoOk = false;
// |         for (;;) {
// |             PollSound();
// |             remoteBuffer = reinterpret_cast<NewGameRemotePacket*>(GetRemoteData(1));
// |             if (remoteBuffer != NULL && remoteBuffer->type == REMOTE_MESSAGE_RELIABLE) {
// |                 switch (remoteBuffer->command) {
// |                     case GAME_REMOTE_MAP_HEADER:
// |                         memset(&m_mapHeader, 0, sizeof(m_mapHeader));
// |                         memcpy(&m_mapHeader, remoteBuffer->payload, GAME_MAP_PACKET_SIZE);
// |                         mapHeaderOk = true;
// |                         break;
// |                     case GAME_REMOTE_PLAYER_INFO:
// |                         memcpy(
// |                             gsNetPlayerInfo,
// |                             remoteBuffer->payload,
// |                             GAME_PLAYER_INFO_PACKET_SIZE
// |                         );
// |                         SetupNetPlayerNames();
// |                         playerInfoOk = true;
// |                         break;
// |                 }
// |                 if (playerInfoOk && mapHeaderOk) {
// |                     m_newGameWindow =
// |                         new heroWindow(NEW_GAME_WINDOW_X, NEW_GAME_NETWORK_WINDOW_Y, "ngmp.bin");
// |                     InitNewGame(&m_mapHeader);
// |                     InitNewGameWindow();
// |                     UpdateNewGameWindow();
// |
// |                     windowMessage.type = MESSAGE_WIDGET;
// |                     windowMessage.payload.widget.id = GAME_MAP_OPTIONS_CONTROL;
// |                     windowMessage.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
// |                     windowMessage.payload.widget.data.value = GAME_WIDGET_ACTIVE_FRAME;
// |                     m_newGameWindow->BroadcastMessage(windowMessage);
// |                     windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
// |                     windowMessage.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
// |                     m_newGameWindow->BroadcastMessage(windowMessage);
// |                     windowMessage.payload.widget.id = GAME_DIALOG_OK;
// |                     windowMessage.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
// |                     windowMessage.payload.widget.data.value = GAME_WIDGET_ACTIVE_FRAME;
// |                     m_newGameWindow->BroadcastMessage(windowMessage);
// |                     windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
// |                     windowMessage.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
// |                     m_newGameWindow->BroadcastMessage(windowMessage);
// |                     windowMessage.payload.widget.id = GAME_DIALOG_CANCEL;
// |                     windowMessage.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
// |                     windowMessage.payload.widget.data.value = GAME_WIDGET_ACTIVE_FRAME;
// |                     m_newGameWindow->BroadcastMessage(windowMessage);
// |                     windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
// |                     windowMessage.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
// |                     m_newGameWindow->BroadcastMessage(windowMessage);
// |
// |                     gbNewGameDialogOver = false;
// |                     gpWindowManager->DoDialog(m_newGameWindow, NewGameHandler, 0);
// |                     delete m_newGameWindow;
// |                     if (gpWindowManager->m_dialogResult == GAME_DIALOG_CANCEL) {
// |                         result = false;
// |                         goto cleanup;
// |                     }
// |                     result = true;
// |                     goto cleanup;
// |                 }
// |             }
// |         }
// |     } else {
// |     pick_map:
// |         wrongExpansionType = false;
// |         mapExt = FindLastToken(m_mapFilename, '.');
// |         if (mapExt != NULL) {
// |             if (StrEqNoCase(mapExt, ".MX2") && xIsExpansionMap)
// |                 wrongExpansionType = true;
// |             if (StrEqNoCase(mapExt, ".MP2") && !xIsExpansionMap)
// |                 wrongExpansionType = true;
// |         }
// |         if (!wrongExpansionType) {
// |             if (xIsExpansionMap)
// |                 strcpy(gpGame->m_mapFilename, "arrax.mx2");
// |             else
// |                 strcpy(gpGame->m_mapFilename, "brokena.mp2");
// |             m_newGameInitialized = false;
// |             m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
// |         }
// |         if (giNumHumanPlayers > BROKENA_MAX_HUMAN_PLAYERS
// |             && stricmp(gpGame->m_mapFilename, "brokena.mp2") == 0)
// |             strcpy(gpGame->m_mapFilename, "slugfest.mp2");
// |         if (giNumHumanPlayers > 1 && stricmp(gpGame->m_mapFilename, "arrax.mx2") == 0)
// |             strcpy(gpGame->m_mapFilename, "fullhse.mx2");
// |
// |         strcpy(gMapName, m_mapFilename);
// |         mapHeaderRead = GetMapHeader(m_mapFilename, &m_mapHeader);
// |         if (!mapHeaderRead || giNumHumanPlayers < m_mapHeader.minHumanPlayers
// |             || giNumHumanPlayers > m_mapHeader.maxHumanPlayers) {
// |             gpGame->GetMap();
// |             goto pick_map;
// |         }
// |
// |         if (gbRemoteOn) {
// |             memcpy(mapInfo, &gpGame->m_mapHeader, GAME_MAP_PACKET_SIZE);
// |             transmitResult = TransmitRemoteData(
// |                 mapInfo,
// |                 GAME_REMOTE_CHANNEL,
// |                 GAME_MAP_PACKET_SIZE,
// |                 GAME_REMOTE_MAP_HEADER,
// |                 1,
// |                 1,
// |                 REMOTE_MESSAGE_DEFAULT
// |             );
// |             if (!transmitResult)
// |                 ShutDown(NULL);
// |             memcpy(netPlayerPacket, gsNetPlayerInfo, GAME_PLAYER_INFO_PACKET_SIZE);
// |             transmitResult = TransmitRemoteData(
// |                 netPlayerPacket,
// |                 GAME_REMOTE_CHANNEL,
// |                 GAME_PLAYER_INFO_PACKET_SIZE,
// |                 GAME_REMOTE_PLAYER_INFO,
// |                 1,
// |                 1,
// |                 REMOTE_MESSAGE_DEFAULT
// |             );
// |             if (!transmitResult)
// |                 ShutDown(NULL);
// |         }
// |
// |         LoadGame("origdata.bin", 1, 0);
// |         if (giNumHumanPlayers > 1) {
// |             if (iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
// |                 m_newGameWindow =
// |                     new heroWindow(NEW_GAME_WINDOW_X, NEW_GAME_NETWORK_WINDOW_Y, "nghs.bin");
// |             else
// |                 m_newGameWindow =
// |                     new heroWindow(NEW_GAME_WINDOW_X, NEW_GAME_NETWORK_WINDOW_Y, "ngmp.bin");
// |         } else {
// |             m_newGameWindow =
// |                 new heroWindow(NEW_GAME_WINDOW_X, NEW_GAME_SINGLE_WINDOW_Y, "ngsp.bin");
// |         }
// |         if (m_newGameWindow == NULL)
// |             MemError();
// |         SetWinText(m_newGameWindow, NEW_GAME_WINDOW_TEXT_ID);
// |         InitNewGame(NULL);
// |         InitNewGameWindow();
// |         UpdateNewGameWindow();
// |         gbNewGameShadowHidden = false;
// |         gbNewGameDialogOver = false;
// |         gpWindowManager->DoDialog(m_newGameWindow, NewGameHandler, 0);
// |         delete m_newGameWindow;
// |         if (gpWindowManager->m_dialogResult == GAME_DIALOG_CANCEL) {
// |             result = false;
// |         } else {
// |             m_playerCount = m_mapHeader.playerCount;
// |             NewMap(gMapName);
// |         }
// |     }
// |
// | cleanup:
// |     for (textBufferIndex = 0; textBufferIndex < GAME_TEXT_BUFFER_COUNT; ++textBufferIndex) {
// |         H2_FREE(cTextReceivedBuffer[textBufferIndex]);
// |     }
// |     H2_FREE(cNGKPCore);
// |     H2_FREE(cNGKPDisplay);
// |     gpResourceManager->Dispose(NGKPBkg);
// |     return result;
// | }
// |
// ALTERNATIVE remove_1 (unified delta from BEFORE):
// | --- before
// | +++ remove_1
// | @@ -133,7 +133,7 @@
// |              else
// |                  strcpy(gpGame->m_mapFilename, "brokena.mp2");
// |              m_newGameInitialized = false;
// | -            m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
// | +            m_newGameHumanCount = giNumHumanPlayers;
// |          }
// |          if (giNumHumanPlayers > BROKENA_MAX_HUMAN_PLAYERS
// |              && stricmp(gpGame->m_mapFilename, "brokena.mp2") == 0)
