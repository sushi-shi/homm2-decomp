// VC6 SP5 source family g105-setup-return-cleanup; parent292b9b73.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state census is claimed.
// build/source-variant-batch/g105-setup-return-cleanup/results.json; RVA 0x9333c
// Complete 2 arms; elapsed 1.555874s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// Full98 native objects/allocated sections/ordered relocations preserved after restoration.
// Nine private EH/FS site-model gaps remain explicit, not waived by native equality.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1433 / 4ff50d82a7889a1e / bcd298e38d0babbe / 67:69 / 66,0,0,1,2 / False
// 1 / {'family': 'early_cleanup_returns', 'candidate': 'baseline'} / 1514 / dac3f01dab0d4652 / e8944ffadab51c4b / 68:69 / 59,1,6,2,1 / False

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
// ALTERNATIVE early_cleanup_returns (unified delta from BEFORE):
// | --- before
// | +++ early_cleanup_returns
// | @@ -82,7 +82,8 @@
// |
// |          giMenuCommand = -1;
// |          result = true;
// | -        goto done;
// | +        gbInSetupDialog = false;
// | +                return result;
// |      }
// |
// |      window = new heroWindow(WINDOW_X, WINDOW_Y, "stpnewgm.bin");
// | @@ -125,7 +126,8 @@
// |                          break;
// |                      case DIALOG_CANCEL:
// |                          result = false;
// | -                        goto done;
// | +                        gbInSetupDialog = false;
// | +                return result;
// |                  }
// |              } else {
// |                  window = new heroWindow(WINDOW_X, WINDOW_Y, "x_loadcm.bin");
// | @@ -139,7 +141,8 @@
// |                          gbInCampaign = true;
// |                          if (!SetupCampaignGame()) {
// |                              result = false;
// | -                            goto done;
// | +                            gbInSetupDialog = false;
// | +                return result;
// |                          }
// |                          break;
// |                      case CHOICE_TWO:
// | @@ -149,7 +152,8 @@
// |                          break;
// |                      case DIALOG_CANCEL:
// |                          result = false;
// | -                        goto done;
// | +                        gbInSetupDialog = false;
// | +                return result;
// |                  }
// |              }
// |              break;
// | @@ -157,13 +161,15 @@
// |          case CHOICE_THREE:
// |              if (!SetupMultiPlayerGame()) {
// |                  result = false;
// | -                goto done;
// | +                gbInSetupDialog = false;
// | +                return result;
// |              }
// |              break;
// |
// |          case DIALOG_CANCEL:
// |              result = false;
// | -            goto done;
// | +            gbInSetupDialog = false;
// | +                return result;
// |      }
// |
// |      LogStr(" Setup 1");
// | @@ -176,7 +182,6 @@
// |              gbWaitForRemoteReceive = true;
// |      }
// |
// | -done:
// |      gbInSetupDialog = false;
// |      return result;
// |  }
