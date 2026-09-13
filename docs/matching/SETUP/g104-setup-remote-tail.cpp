// VC6 SP5 source family g104-setup-remote-tail; parent292b9b73.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state census is claimed.
// build/source-variant-batch/g104-setup-remote-tail/results.json; RVA 0x9333c
// Complete 2 arms; elapsed 1.617384s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// Full98 native objects/allocated sections/ordered relocations preserved after restoration.
// Nine private EH/FS site-model gaps remain explicit, not waived by native equality.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1433 / 4ff50d82a7889a1e / bcd298e38d0babbe / 67:69 / 66,0,0,1,2 / False
// 1 / {'family': 'after_choice_switch', 'candidate': 'baseline'} / 1451 / 3706ab7bccff7f71 / 46de08645f3450ca / 69:69 / 10,6,25,28,0 / False

// AXIS family
// BEFORE (complete exact span):
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
// ALTERNATIVE after_choice_switch (unified delta from BEFORE):
// | --- before
// | +++ after_choice_switch
// | @@ -24,41 +24,42 @@
// |              case APP_MENU_LOAD_5:
// |                  iMPBaseType = MULTIPLAYER_BASE_NETWORK;
// |                  iMPExtendedType = REMOTE_GAME_NETWORK_HOST;
// | -                goto remoteSetup;
// | +                break;
// |              case APP_MENU_RESTART_9:
// |              case APP_MENU_LOAD_6:
// |                  iMPBaseType = MULTIPLAYER_BASE_NETWORK;
// |                  iMPExtendedType = REMOTE_GAME_NETWORK_GUEST;
// | -                goto remoteSetup;
// | +                break;
// |              case APP_MENU_RESTART_10:
// |              case APP_MENU_LOAD_7:
// |                  iMPBaseType = MULTIPLAYER_BASE_MODEM;
// |                  iMPExtendedType = REMOTE_GAME_MODEM_HOST;
// | -                goto remoteSetup;
// | +                break;
// |              case APP_MENU_RESTART_11:
// |              case APP_MENU_LOAD_8:
// |                  iMPBaseType = MULTIPLAYER_BASE_MODEM;
// |                  iMPExtendedType = REMOTE_GAME_MODEM_GUEST;
// | -                goto remoteSetup;
// | +                break;
// |              case APP_MENU_RESTART_12:
// |              case APP_MENU_LOAD_9:
// |                  iMPBaseType = MULTIPLAYER_BASE_MODEM;
// |                  iMPExtendedType = REMOTE_GAME_MODEM_HOST;
// |                  gbDirectConnect = true;
// | -                goto remoteSetup;
// | +                break;
// |              case APP_MENU_RESTART_13:
// |              case APP_MENU_LOAD_10:
// |                  iMPBaseType = MULTIPLAYER_BASE_MODEM;
// |                  iMPExtendedType = REMOTE_GAME_MODEM_GUEST;
// |                  gbDirectConnect = true;
// | -                goto remoteSetup;
// | +                break;
// |
// | -            remoteSetup:
// | +        }
// | +
// | +        if (iMPBaseType == MULTIPLAYER_BASE_NETWORK || iMPBaseType == MULTIPLAYER_BASE_MODEM) {
// |                  LogStr("Setup 0a");
// |                  RemoteMain(iMPExtendedType);
// |                  if (iMPExtendedType == REMOTE_GAME_NETWORK_GUEST
// |                      || iMPExtendedType == REMOTE_GAME_MODEM_GUEST)
// |                      gbWaitForRemoteReceive = true;
// | -                break;
// |          }
// |
