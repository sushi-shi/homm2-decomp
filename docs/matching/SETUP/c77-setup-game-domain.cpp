// VC6 SP5 source family c77-setup-game-domain; parent292b9b73.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state census is claimed.
// build/source-variant-batch/c77-setup-game-domain/results.json; RVA 0x9333c
// Complete 8 arms; elapsed 4.721562s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// Full98 native objects/allocated sections/ordered relocations preserved after restoration.
// Nine private EH/FS site-model gaps remain explicit, not waived by native equality.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1433 / 4ff50d82a7889a1e / bcd298e38d0babbe / 67:69 / 66,0,0,1,2 / False
// 1 / {'family': 'remove_001', 'candidate': 'baseline'} / 1432 / 647c29859ac439e7 / 081e3139212caf81 / 67:69 / 66,0,0,1,2 / False
// 2 / {'family': 'remove_010', 'candidate': 'baseline'} / 1432 / c0a8771dc39ac974 / c94fded5bb0e097d / 67:69 / 66,0,0,1,2 / False
// 3 / {'family': 'remove_011', 'candidate': 'baseline'} / 1431 / a63f597bcde1759d / 4229d8d4e4d90ea0 / 67:69 / 66,0,0,1,2 / False
// 4 / {'family': 'remove_100', 'candidate': 'baseline'} / 1432 / 849b7fe5a74980cc / 86a09d702c1f8373 / 67:69 / 66,0,0,1,2 / False
// 5 / {'family': 'remove_101', 'candidate': 'baseline'} / 1431 / 73f9677bcbd11ae9 / 7b19f72a4eecff25 / 67:69 / 66,0,0,1,2 / False
// 6 / {'family': 'remove_110', 'candidate': 'baseline'} / 1431 / 943877b51d5fbfa5 / 2f95f12efb82949d / 67:69 / 66,0,0,1,2 / False
// 7 / {'family': 'remove_111', 'candidate': 'baseline'} / 1430 / b05a446477bd02f0 / 358f82b92e72126a / 67:69 / 66,0,0,1,2 / False

// AXIS family
// BEFORE (complete exact span):
// |  0x599)
// | i32 game::SetupGame(void) {
// |     heroWindow* window;
// |     b32 result;
// |
// |     LogStr("Setup 0");
// |     result = true;
// |     xIsPlayingExpansionCampaign = 0;
// |     xIsExpansionMap = false;
// |     gbInCampaign = false;
// |     gbCampaignSideChoice = CAMPAIGN_ROLAND;
// |     iMPExtendedType = REMOTE_GAME_UNINITIALIZED;
// |     iMPBaseType = MULTIPLAYER_BASE_UNINITIALIZED;
// |     giNumHumanPlayers = 1;
// |     gbWaitForRemoteReceive = false;
// |     gbDirectConnect = false;
// |     gbInSetupDialog = true;
// |
// |     if (giMenuCommand != -1) {
// |         switch (giMenuCommand) {
// |             case APP_MENU_RESTART_0:
// |             case APP_MENU_LOAD_0:
// |                 break;
// |
// |             case APP_MENU_RESTART_5:
// |             case APP_MENU_LOAD_2:
// |                 giNumHumanPlayers = TWO_PLAYERS;
// |                 iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
// |                 break;
// |             case APP_MENU_RESTART_6:
// |             case APP_MENU_LOAD_3:
// |                 giNumHumanPlayers = THREE_PLAYERS;
// |                 iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
// |                 break;
// |             case APP_MENU_RESTART_7:
// |             case APP_MENU_LOAD_4:
// |                 giNumHumanPlayers = FOUR_PLAYERS;
// |                 iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
// |                 break;
// |
// |             case APP_MENU_RESTART_8:
// |             case APP_MENU_LOAD_5:
// |                 iMPBaseType = MULTIPLAYER_BASE_NETWORK;
// |                 iMPExtendedType = REMOTE_GAME_NETWORK_HOST;
// |                 goto remoteSetup;
// |             case APP_MENU_RESTART_9:
// |             case APP_MENU_LOAD_6:
// |                 iMPBaseType = MULTIPLAYER_BASE_NETWORK;
// |                 iMPExtendedType = REMOTE_GAME_NETWORK_GUEST;
// |                 goto remoteSetup;
// |             case APP_MENU_RESTART_10:
// |             case APP_MENU_LOAD_7:
// |                 iMPBaseType = MULTIPLAYER_BASE_MODEM;
// |                 iMPExtendedType = REMOTE_GAME_MODEM_HOST;
// |                 goto remoteSetup;
// |             case APP_MENU_RESTART_11:
// |             case APP_MENU_LOAD_8:
// |                 iMPBaseType = MULTIPLAYER_BASE_MODEM;
// |                 iMPExtendedType = REMOTE_GAME_MODEM_GUEST;
// |                 goto remoteSetup;
// |             case APP_MENU_RESTART_12:
// |             case APP_MENU_LOAD_9:
// |                 iMPBaseType = MULTIPLAYER_BASE_MODEM;
// |                 iMPExtendedType = REMOTE_GAME_MODEM_HOST;
// |                 gbDirectConnect = true;
// |                 goto remoteSetup;
// |             case APP_MENU_RESTART_13:
// |             case APP_MENU_LOAD_10:
// |                 iMPBaseType = MULTIPLAYER_BASE_MODEM;
// |                 iMPExtendedType = REMOTE_GAME_MODEM_GUEST;
// |                 gbDirectConnect = true;
// |                 goto remoteSetup;
// |
// |             remoteSetup:
// |                 LogStr("Setup 0a");
// |                 RemoteMain(iMPExtendedType);
// |                 if (iMPExtendedType == REMOTE_GAME_NETWORK_GUEST
// |                     || iMPExtendedType == REMOTE_GAME_MODEM_GUEST)
// |                     gbWaitForRemoteReceive = true;
// |                 break;
// |         }
// |
// |         giMenuCommand = -1;
// |         result = true;
// |         goto done;
// |     }
// |
// |     window = new heroWindow(WINDOW_X, WINDOW_Y, "stpnewgm.bin");
// |     if (window == NULL)
// |         MemError();
// |
// |     if (gbNoCDRom != 0) {
// |         tag_message message;
// |         message.type = MESSAGE_WIDGET;
// |         message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
// |         message.payload.widget.data.value = IDX(WIDGET_COMMAND_DIMMED);
// |         message.payload.widget.id = CHOICE_ONE;
// |         window->BroadcastMessage(message);
// |         message.payload.widget.id = CHOICE_TWO;
// |         window->BroadcastMessage(message);
// |     }
// |
// |     gpWindowManager->DoDialog(window, SetupGameHandler, 0);
// |     delete window;
// |
// |     switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// |         case CHOICE_ONE:
// |             break;
// |
// |         case CHOICE_TWO:
// |             if (giSetupGameType == OLD_MAIN_SETUP_LOAD) {
// |                 window = new heroWindow(WINDOW_X, WINDOW_Y, "x_loadcm.bin");
// |                 if (window == NULL)
// |                     MemError();
// |                 gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
// |                 delete window;
// |
// |                 switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// |                     case CHOICE_ONE:
// |                         gbInCampaign = true;
// |                         break;
// |                     case CHOICE_TWO:
// |                         xIsPlayingExpansionCampaign = 1;
// |                         xIsExpansionMap = true;
// |                         break;
// |                     case DIALOG_CANCEL:
// |                         result = false;
// |                         goto done;
// |                 }
// |             } else {
// |                 window = new heroWindow(WINDOW_X, WINDOW_Y, "x_loadcm.bin");
// |                 if (window == NULL)
// |                     MemError();
// |                 gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
// |                 delete window;
// |
// |                 switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// |                     case CHOICE_ONE:
// |                         gbInCampaign = true;
// |                         if (!SetupCampaignGame()) {
// |                             result = false;
// |                             goto done;
// |                         }
// |                         break;
// |                     case CHOICE_TWO:
// |                         xIsPlayingExpansionCampaign = 1;
// |                         xIsExpansionMap = true;
// |                         xCampaign.InitNewCampaign(xCampaign.Choose());
// |                         break;
// |                     case DIALOG_CANCEL:
// |                         result = false;
// |                         goto done;
// |                 }
// |             }
// |             break;
// |
// |         case CHOICE_THREE:
// |             if (!SetupMultiPlayerGame()) {
// |                 result = false;
// |                 goto done;
// |             }
// |             break;
// |
// |         case DIALOG_CANCEL:
// |             result = false;
// |             goto done;
// |     }
// |
// |     LogStr(" Setup 1");
// |     if (iMPBaseType == MULTIPLAYER_BASE_NETWORK || iMPBaseType == MULTIPLAYER_BASE_MODEM) {
// |         LogStr(" Setup 2");
// |         RemoteMain(iMPExtendedType);
// |         LogStr(" Setup 3");
// |         if (iMPExtendedType == REMOTE_GAME_NETWORK_GUEST
// |             || iMPExtendedType == REMOTE_GAME_MODEM_GUEST)
// |             gbWaitForRemoteReceive = true;
// |     }
// |
// | done:
// |     gbInSetupDialog = false;
// |     return result;
// | }
// |
// ALTERNATIVE remove_001 (unified delta from BEFORE):
// | --- before
// | +++ remove_001
// | @@ -134,7 +134,7 @@
// |                  gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
// |                  delete window;
// |
// | -                switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +                switch (gpWindowManager->m_dialogResult) {
// |                      case CHOICE_ONE:
// |                          gbInCampaign = true;
// |                          if (!SetupCampaignGame()) {
// ALTERNATIVE remove_010 (unified delta from BEFORE):
// | --- before
// | +++ remove_010
// | @@ -115,7 +115,7 @@
// |                  gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
// |                  delete window;
// |
// | -                switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +                switch (gpWindowManager->m_dialogResult) {
// |                      case CHOICE_ONE:
// |                          gbInCampaign = true;
// |                          break;
// ALTERNATIVE remove_011 (unified delta from BEFORE):
// | --- before
// | +++ remove_011
// | @@ -115,7 +115,7 @@
// |                  gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
// |                  delete window;
// |
// | -                switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +                switch (gpWindowManager->m_dialogResult) {
// |                      case CHOICE_ONE:
// |                          gbInCampaign = true;
// |                          break;
// | @@ -134,7 +134,7 @@
// |                  gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
// |                  delete window;
// |
// | -                switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +                switch (gpWindowManager->m_dialogResult) {
// |                      case CHOICE_ONE:
// |                          gbInCampaign = true;
// |                          if (!SetupCampaignGame()) {
// ALTERNATIVE remove_100 (unified delta from BEFORE):
// | --- before
// | +++ remove_100
// | @@ -103,7 +103,7 @@
// |      gpWindowManager->DoDialog(window, SetupGameHandler, 0);
// |      delete window;
// |
// | -    switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +    switch (gpWindowManager->m_dialogResult) {
// |          case CHOICE_ONE:
// |              break;
// |
// ALTERNATIVE remove_101 (unified delta from BEFORE):
// | --- before
// | +++ remove_101
// | @@ -103,7 +103,7 @@
// |      gpWindowManager->DoDialog(window, SetupGameHandler, 0);
// |      delete window;
// |
// | -    switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +    switch (gpWindowManager->m_dialogResult) {
// |          case CHOICE_ONE:
// |              break;
// |
// | @@ -134,7 +134,7 @@
// |                  gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
// |                  delete window;
// |
// | -                switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +                switch (gpWindowManager->m_dialogResult) {
// |                      case CHOICE_ONE:
// |                          gbInCampaign = true;
// |                          if (!SetupCampaignGame()) {
// ALTERNATIVE remove_110 (unified delta from BEFORE):
// | --- before
// | +++ remove_110
// | @@ -103,7 +103,7 @@
// |      gpWindowManager->DoDialog(window, SetupGameHandler, 0);
// |      delete window;
// |
// | -    switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +    switch (gpWindowManager->m_dialogResult) {
// |          case CHOICE_ONE:
// |              break;
// |
// | @@ -115,7 +115,7 @@
// |                  gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
// |                  delete window;
// |
// | -                switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +                switch (gpWindowManager->m_dialogResult) {
// |                      case CHOICE_ONE:
// |                          gbInCampaign = true;
// |                          break;
// ALTERNATIVE remove_111 (unified delta from BEFORE):
// | --- before
// | +++ remove_111
// | @@ -103,7 +103,7 @@
// |      gpWindowManager->DoDialog(window, SetupGameHandler, 0);
// |      delete window;
// |
// | -    switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +    switch (gpWindowManager->m_dialogResult) {
// |          case CHOICE_ONE:
// |              break;
// |
// | @@ -115,7 +115,7 @@
// |                  gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
// |                  delete window;
// |
// | -                switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +                switch (gpWindowManager->m_dialogResult) {
// |                      case CHOICE_ONE:
// |                          gbInCampaign = true;
// |                          break;
// | @@ -134,7 +134,7 @@
// |                  gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
// |                  delete window;
// |
// | -                switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
// | +                switch (gpWindowManager->m_dialogResult) {
// |                      case CHOICE_ONE:
// |                          gbInCampaign = true;
// |                          if (!SetupCampaignGame()) {
