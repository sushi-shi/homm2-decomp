#include <va.h>
#include <BASE/Misc.h>
#include <BASE/executive.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/widget.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/KB.h>
#include <SOURCE/Modem.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/SMACKMGR.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/fileRequester.h>
#include <SOURCE/game.h>
#include <SOURCE/SETUP.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

H2_ENUM_BEGIN(SetupConstant)
    WINDOW_X                     = 405,
    WINDOW_Y                     = 8,
    DIALOG_CANCEL                = NORMAL_DIALOG_BUTTON_ONE,
    DIALOG_YES                   = NORMAL_DIALOG_BUTTON_FIVE,
    PLAYER_COUNT                 = IDX(GAME_PLAYER_COUNT),
    PLAYER_NAME_LENGTH           = GLOBAL_PLAYER_NAME_SIZE - 1,
    DEFAULT_PLAYER_NAME_CAPACITY = 24,
    MODEM_INIT_ENTRY_LENGTH      = 40,
    TELEPHONE_ENTRY_LENGTH       = MODEM_NUMBER_BUFFER_SIZE - 1,
    FILE_PATTERN_CAPACITY        = 12,
    FILE_REQUESTER_X             = 200,
    FILE_REQUESTER_Y             = 58,
    DISABLED_WIDGET_ID           = 1,
    CAMPAIGN_INTRO               = 4,
    CAMPAIGN_SELECTION           = 35,
    HELP_DIALOG                  = NORMAL_DIALOG_QUICK_VIEW,
    DIALOG_RESULT_MIN            = 1,
    DIALOG_RESULT_MAX            = 1000,
    HANDLER_CONTINUE             = 1,
    HANDLER_CLOSE                = 2
H2_ENUM_END(SetupConstant)

H2_ENUM_BEGIN(SetupDialogChoice)
    CHOICE_ONE   = 1,
    CHOICE_TWO   = 2,
    CHOICE_THREE = 3,
    CHOICE_FOUR  = 4,
    CHOICE_FIVE  = 5
H2_ENUM_END(SetupDialogChoice)

H2_ENUM_BEGIN(SetupHelpIndex)
    NO_HELP    = -1,
    FIRST_HELP = 0
H2_ENUM_END(SetupHelpIndex)

H2_ENUM_BEGIN(HotSeatPlayerCount)
    TWO_PLAYERS   = 2,
    THREE_PLAYERS = 3,
    FOUR_PLAYERS  = 4,
    FIVE_PLAYERS  = 5,
    SIX_PLAYERS   = 6
H2_ENUM_END(HotSeatPlayerCount)

H2_ENUM_BEGIN(SetupRemoteConstant)
    REMOTE_TYPE_UNINITIALIZED = 10
H2_ENUM_END(SetupRemoteConstant)

H2_ENUM_CLASS_BEGIN(SetupBaudRate)
    SETUP_BAUD_2400  = 2400,
    SETUP_BAUD_9600  = 9600,
    SETUP_BAUD_19200 = 19200,
    SETUP_BAUD_38400 = 38400
H2_ENUM_CLASS_END(SetupBaudRate)

static inline i32 GetSetupHelpIndex(i32 widgetId, i32 lastChoice) {
    if (widgetId >= CHOICE_ONE && widgetId <= lastChoice)
        return widgetId - CHOICE_ONE;
    if (widgetId == DIALOG_CANCEL)
        return lastChoice;
    return NO_HELP;
}

VA(0x00410e90, 0x2f)
i32 game::SetupCampaignGame(void) {
    PlaySmacker(CAMPAIGN_INTRO);
    PlaySmacker(CAMPAIGN_SELECTION);
    return 1;
}

VA(0x00410ebf, 0x141)
i32 game::SetupBaud(void) {
    heroWindow* window = new heroWindow(WINDOW_X, WINDOW_Y, "stpbaud.bin");
    if (window == NULL)
        MemError();
    gpWindowManager->DoDialog(window, SetupBaudHandler, 0);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            gConfig.baudRate[gbDirectConnect] = IDX(SETUP_BAUD_2400);
            break;
        case CHOICE_TWO:
            gConfig.baudRate[gbDirectConnect] = IDX(SETUP_BAUD_9600);
            break;
        case CHOICE_THREE:
            gConfig.baudRate[gbDirectConnect] = IDX(SETUP_BAUD_19200);
            break;
        case CHOICE_FOUR:
            gConfig.baudRate[gbDirectConnect] = IDX(SETUP_BAUD_38400);
            break;
        case DIALOG_CANCEL:
            return 0;
    }
    return 1;
}

VA(0x00411000, 0x200)
i32 game::SetupComPort(void) {
    char initString[MODEM_INIT_ENTRY_LENGTH];

    LogStr("SCP 1");
    heroWindow* setupWindow = new heroWindow(WINDOW_X, WINDOW_Y, "stpcom.bin");
    if (setupWindow == NULL)
        MemError();
    LogStr("SCP 2");
    gpWindowManager->DoDialog(setupWindow, SetupComPortHandler, 0);
    delete setupWindow;
    LogStr("SCP 3");

    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            gConfig.comPort[gbDirectConnect] = IDX(SETUP_COM_PORT_1);
            break;
        case CHOICE_TWO:
            gConfig.comPort[gbDirectConnect] = IDX(SETUP_COM_PORT_2);
            break;
        case CHOICE_THREE:
            gConfig.comPort[gbDirectConnect] = IDX(SETUP_COM_PORT_3);
            break;
        case CHOICE_FOUR:
            gConfig.comPort[gbDirectConnect] = IDX(SETUP_COM_PORT_4);
            break;
        case DIALOG_CANCEL:
            return 0;
    }

    LogStr("SCP 4");
    if (!SetupBaud())
        return 0;
    LogStr("SCP 5");
    if (gbDirectConnect == 0) {
        strcpy(gConfig.modemInitString, "ATZ");
        sprintf(gText, "%s", gConfig.modemInitString);
        GetDataEntry(
            "Please enter any special initialization string required by your modem, or hit 'ENTER' "
            "to accept the default.",
            initString,
            MODEM_INIT_ENTRY_LENGTH,
            gText,
            0,
            1
        );
        strcpy(gConfig.modemInitString, initString);
    }
    WritePrefs();
    return 1;
}

VA(0x00411200, 0x238)
i32 game::SetupHotSeatGame(void) {
    char defaultName[DEFAULT_PLAYER_NAME_CAPACITY];
    i32 ix;

    heroWindow* window = new heroWindow(WINDOW_X, WINDOW_Y, "stphotst.bin");
    if (window == NULL)
        MemError();
    gpWindowManager->DoDialog(window, SetupHotSeatGameHandler, 0);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            giNumHumanPlayers = TWO_PLAYERS;
            break;
        case CHOICE_TWO:
            giNumHumanPlayers = THREE_PLAYERS;
            break;
        case CHOICE_THREE:
            giNumHumanPlayers = FOUR_PLAYERS;
            break;
        case CHOICE_FOUR:
            giNumHumanPlayers = FIVE_PLAYERS;
            break;
        case CHOICE_FIVE:
            giNumHumanPlayers = SIX_PLAYERS;
            break;
        case DIALOG_CANCEL:
            return 0;
    }

    for (ix = 0; ix < PLAYER_COUNT; ix++)
        strcpy(cPlayerNames[ix], "");

    if (giSetupGameType == 0) {
        sprintf(gText, "Do you wish to enter each player's name?");
        NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gpWindowManager->m_dialogResult == DIALOG_YES) {
            for (ix = 0; ix < OD_STEER(giNumHumanPlayers); ix++) {
                strcpy(defaultName, "");
                sprintf(gText, "Enter player %d's name.", ix + 1);
                GetDataEntry(gText, cPlayerNames[ix], PLAYER_NAME_LENGTH, defaultName, 0, 1);
            }
        }
    }
    return 1;
}

VA(0x00411438, 0x127)
i32 game::SetupNetworkGame(void) {
    tag_message message;
    heroWindow* window = new heroWindow(WINDOW_X, WINDOW_Y, "stpnet.bin");
    if (window == NULL)
        MemError();

    if (gbNoCDRom != 0) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.id = DISABLED_WIDGET_ID;
        message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
        window->BroadcastMessage(message);
    }

    gpWindowManager->DoDialog(window, SetupNetworkGameHandler, 0);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            iMPExtendedType = IDX(REMOTE_GAME_NETWORK_HOST);
            break;
        case CHOICE_TWO:
            iMPExtendedType = IDX(REMOTE_GAME_NETWORK_GUEST);
            break;
        case DIALOG_CANCEL:
            return 0;
    }
    return 1;
}

VA(0x0041155f, 0x236)
i32 game::SetupNetworkGame2(void) {
    tag_message message;
    OSVERSIONINFO osInfo;
    i32 result;

    heroWindow* window = new heroWindow(WINDOW_X, WINDOW_Y, "stpnet2.bin");
    if (window == NULL)
        MemError();

    memset(&osInfo, 0, sizeof(osInfo));
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    result = GetVersionEx(&osInfo);
    LogInt(
        "Version",
        result,
        osInfo.dwPlatformId,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
    if (result != 0 && osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
        message.payload.widget.id = CHOICE_ONE;
        window->BroadcastMessage(message);
        message.payload.widget.id = CHOICE_THREE;
        window->BroadcastMessage(message);
    }

    gpWindowManager->DoDialog(window, SetupNetworkGame2Handler, 0);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            iMPNetProtocol = REMOTE_PROTOCOL_DIRECT_PLAY;
            break;
        case CHOICE_TWO:
            iMPNetProtocol = REMOTE_PROTOCOL_WINSOCK;
            break;
        case CHOICE_THREE:
            iMPNetProtocol = REMOTE_PROTOCOL_MODEM;
            break;
        case CHOICE_FOUR:
            iMPNetProtocol = REMOTE_PROTOCOL_DIRECT_CONNECT;
            break;
        case DIALOG_CANCEL:
            return 0;
    }
    if (!SetupNetworkGame())
        return 0;
    else
        return 1;
}

VA(0x00411795, 0x317)
i32 game::SetupModemGame(void) {
    tag_message message;
    heroWindow* window;

    LogStr("SMC 1");
    if (gbDirectConnect != 0) {
        if (gConfig.comPort[gbDirectConnect] == IDX(SETUP_COM_PORT_UNCONFIGURED))
            window = new heroWindow(WINDOW_X, WINDOW_Y, "stpdc.bin");
        else
            window = new heroWindow(WINDOW_X, WINDOW_Y, "stpdccfg.bin");
    } else {
        if (gConfig.comPort[gbDirectConnect] == IDX(SETUP_COM_PORT_UNCONFIGURED))
            window = new heroWindow(WINDOW_X, WINDOW_Y, "stpmodem.bin");
        else
            window = new heroWindow(WINDOW_X, WINDOW_Y, "stpmcfg.bin");
    }
    if (window == NULL)
        MemError();

    LogStr("SMC 2");
    if (gbNoCDRom != 0) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.id = DISABLED_WIDGET_ID;
        message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
        window->BroadcastMessage(message);
    }
    LogStr("SMC 3");
    gpWindowManager->DoDialog(window, SetupModemGameHandler, 0);
    LogStr("SMC 4");
    delete window;
    LogStr("SMC 5");

    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            LogStr("SMC 6");
            iMPExtendedType = IDX(REMOTE_GAME_MODEM_HOST);
            if (gConfig.comPort[gbDirectConnect] == IDX(SETUP_COM_PORT_UNCONFIGURED)) {
                LogStr("SMC 7");
                if (!SetupComPort())
                    return 0;
                LogStr("SMC 8");
            }
            LogStr("SMC 9");
            if (gbDirectConnect == 0) {
                GetDataEntry(
                    "Please enter the telephone number.",
                    numbuf,
                    TELEPHONE_ENTRY_LENGTH,
                    NULL,
                    0,
                    1
                );
            }
            LogStr("SMC a");
            break;
        case CHOICE_TWO:
            iMPExtendedType = IDX(REMOTE_GAME_MODEM_GUEST);
            if (gConfig.comPort[gbDirectConnect] == IDX(SETUP_COM_PORT_UNCONFIGURED)
                && !SetupComPort())
                return 0;
            break;
        case CHOICE_THREE:
            gbDoModemConfig = true;
            break;
        case DIALOG_CANCEL:
            return 0;
    }
    return 1;
}

VA(0x00411aac, 0x257)
i32 game::SetupMultiPlayerGame(void) {
    tag_message message;
    i32 continueFlag;

    heroWindow* window = new heroWindow(WINDOW_X, WINDOW_Y, "stpmp.bin");
    if (window == NULL)
        MemError();

    if (gbNoCDRom != 0) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.id = DISABLED_WIDGET_ID;
        message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
        window->BroadcastMessage(message);
    }
    gpWindowManager->DoDialog(window, SetupMultiPlayerGameHandler, 0);
    delete window;

    gbDirectConnect = false;
    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
            if (!SetupHotSeatGame())
                return 0;
            break;
        case CHOICE_TWO:
            iMPBaseType = MULTIPLAYER_BASE_NETWORK;
            if (!SetupNetworkGame2())
                return 0;
            break;
        case CHOICE_FOUR:
            gbDirectConnect = true;
            goto setupModem;
        case CHOICE_THREE:
            gbDirectConnect = false;
        setupModem:
            iMPBaseType = MULTIPLAYER_BASE_MODEM;
            continueFlag = 1;
            LogStr("Common Modem 1");
            while (continueFlag) {
                LogStr("Common Modem 2");
                if (!SetupModemGame())
                    return 0;
                LogStr("Common Modem 3");
                if (gbDoModemConfig != 0) {
                    LogStr("Common Modem 4");
                    gbDoModemConfig = false;
                    if (!SetupComPort())
                        return 0;
                    LogStr("Common Modem 5");
                } else {
                    continueFlag = 0;
                }
                LogStr("Common Modem 6");
            }
            LogStr("Common Modem 7");
            break;
        case DIALOG_CANCEL:
            return 0;
    }
    return 1;
}

VA(0x00411d03, 0x6c9)
i32 game::SetupGame(void) {
    heroWindow* window;
    i32 result;

    LogStr("Setup 0");
    result = 1;
    xIsPlayingExpansionCampaign = 0;
    xIsExpansionMap = 0;
    gbInCampaign = false;
    gbCampaignSideChoice = false;
    iMPExtendedType = REMOTE_TYPE_UNINITIALIZED;
    iMPBaseType = MULTIPLAYER_BASE_UNINITIALIZED;
    giNumHumanPlayers = 1;
    gbWaitForRemoteReceive = false;
    gbDirectConnect = false;
    gbInSetupDialog = true;

    if (giMenuCommand != -1) {
        switch (giMenuCommand) {
            case APP_MENU_RESTART_0:
            case APP_MENU_LOAD_0:
                break;

            case APP_MENU_RESTART_5:
            case APP_MENU_LOAD_2:
                giNumHumanPlayers = TWO_PLAYERS;
                iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
                break;
            case APP_MENU_RESTART_6:
            case APP_MENU_LOAD_3:
                giNumHumanPlayers = THREE_PLAYERS;
                iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
                break;
            case APP_MENU_RESTART_7:
            case APP_MENU_LOAD_4:
                giNumHumanPlayers = FOUR_PLAYERS;
                iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
                break;

            case APP_MENU_RESTART_8:
            case APP_MENU_LOAD_5:
                iMPBaseType = MULTIPLAYER_BASE_NETWORK;
                iMPExtendedType = IDX(REMOTE_GAME_NETWORK_HOST);
                goto remoteSetup;
            case APP_MENU_RESTART_9:
            case APP_MENU_LOAD_6:
                iMPBaseType = MULTIPLAYER_BASE_NETWORK;
                iMPExtendedType = IDX(REMOTE_GAME_NETWORK_GUEST);
                goto remoteSetup;
            case APP_MENU_RESTART_10:
            case APP_MENU_LOAD_7:
                iMPBaseType = MULTIPLAYER_BASE_MODEM;
                iMPExtendedType = IDX(REMOTE_GAME_MODEM_HOST);
                goto remoteSetup;
            case APP_MENU_RESTART_11:
            case APP_MENU_LOAD_8:
                iMPBaseType = MULTIPLAYER_BASE_MODEM;
                iMPExtendedType = IDX(REMOTE_GAME_MODEM_GUEST);
                goto remoteSetup;
            case APP_MENU_RESTART_12:
            case APP_MENU_LOAD_9:
                iMPBaseType = MULTIPLAYER_BASE_MODEM;
                iMPExtendedType = IDX(REMOTE_GAME_MODEM_HOST);
                gbDirectConnect = true;
                goto remoteSetup;
            case APP_MENU_RESTART_13:
            case APP_MENU_LOAD_10:
                iMPBaseType = MULTIPLAYER_BASE_MODEM;
                iMPExtendedType = IDX(REMOTE_GAME_MODEM_GUEST);
                gbDirectConnect = true;
                goto remoteSetup;

            remoteSetup:
                LogStr("Setup 0a");
                RemoteMain(static_cast<RemoteGameMode>(iMPExtendedType));
                if (iMPExtendedType == IDX(REMOTE_GAME_NETWORK_GUEST)
                    || iMPExtendedType == IDX(REMOTE_GAME_MODEM_GUEST))
                    gbWaitForRemoteReceive = true;
                break;
            default:;
        }

    menuDone:
        giMenuCommand = -1;
        result = 1;
        goto done;
    }

    window = new heroWindow(WINDOW_X, WINDOW_Y, "stpnewgm.bin");
    if (window == NULL)
        MemError();

    if (gbNoCDRom != 0) {
        tag_message message;
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
        message.payload.widget.id = CHOICE_ONE;
        window->BroadcastMessage(message);
        message.payload.widget.id = CHOICE_TWO;
        window->BroadcastMessage(message);
    }

    gpWindowManager->DoDialog(window, SetupGameHandler, 0);
    delete window;

    switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
        case CHOICE_ONE:
            break;

        case CHOICE_TWO:
            if (giSetupGameType == OLD_MAIN_SETUP_LOAD) {
                window = new heroWindow(WINDOW_X, WINDOW_Y, "x_loadcm.bin");
                if (window == NULL)
                    MemError();
                gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
                delete window;

                switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
                    case CHOICE_ONE:
                        gbInCampaign = true;
                        break;
                    case CHOICE_TWO:
                        xIsPlayingExpansionCampaign = 1;
                        xIsExpansionMap = 1;
                        break;
                    case DIALOG_CANCEL:
                        result = 0;
                        goto done;
                }
            } else {
                window = new heroWindow(WINDOW_X, WINDOW_Y, "x_loadcm.bin");
                if (window == NULL)
                    MemError();
                gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
                delete window;

                switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
                    case CHOICE_ONE:
                        gbInCampaign = true;
                        if (!SetupCampaignGame()) {
                            result = 0;
                            goto done;
                        }
                        break;
                    case CHOICE_TWO:
                        xIsPlayingExpansionCampaign = 1;
                        xIsExpansionMap = 1;
                        xCampaign.InitNewCampaign(xCampaign.Choose());
                        break;
                    case DIALOG_CANCEL:
                        result = 0;
                        goto done;
                }
            }
            break;

        case CHOICE_THREE:
            if (!SetupMultiPlayerGame()) {
                result = 0;
                goto done;
            }
            break;

        case DIALOG_CANCEL:
            result = 0;
            goto done;
    }

    LogStr(" Setup 1");
    if (iMPBaseType == MULTIPLAYER_BASE_NETWORK || iMPBaseType == MULTIPLAYER_BASE_MODEM) {
        LogStr(" Setup 2");
        RemoteMain(static_cast<RemoteGameMode>(iMPExtendedType));
        LogStr(" Setup 3");
        if (iMPExtendedType == IDX(REMOTE_GAME_NETWORK_GUEST)
            || iMPExtendedType == IDX(REMOTE_GAME_MODEM_GUEST))
            gbWaitForRemoteReceive = true;
    }

done:
    gbInSetupDialog = false;
    return result;
}

VA(0x004123cc, 0x2aa)
i32 game::PickLoadGame(void) {
    char filePattern_4[FILE_PATTERN_CAPACITY];
    i32 dialogResult_18;
    heroWindow* window_27;
    fileRequester* requester_11;

    if (gbWaitForRemoteReceive != 0)
        return 1;

    if (gbInCampaign != 0) {
        sprintf(filePattern_4, "*.GMC");
    } else if (xIsPlayingExpansionCampaign != 0) {
        sprintf(filePattern_4, "*.GXC");
    } else if (gbRemoteOn != 0 && xNetHasOldPlayers != 0) {
        NormalDialog(
            "At least one player does not have the Heroes II Expansion set.  You will only be able "
            "to choose from original Heroes II games.",
            NORMAL_DIALOG_INFO,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
        sprintf(filePattern_4, "*.GM%d", giNumHumanPlayers);
    } else {
        window_27 = new heroWindow(WINDOW_X, WINDOW_Y, "x_mapmnu.bin");
        if (window_27 == NULL)
            MemError();
        gpWindowManager->DoDialog(window_27, ExpStdGameHandler, 0);
        delete window_27;

        switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
            case CHOICE_ONE:
                xIsExpansionMap = 0;
                break;
            case CHOICE_TWO:
                xIsExpansionMap = 1;
                break;
            case DIALOG_CANCEL:
                return 0;
        }

        if (xIsExpansionMap != 0)
            sprintf(filePattern_4, "*.GX%d", giNumHumanPlayers);
        else
            sprintf(filePattern_4, "*.GM%d", giNumHumanPlayers);
    }

    requester_11 = new fileRequester(
        FILE_REQUESTER_X,
        FILE_REQUESTER_Y,
        FILE_REQUESTER_LOAD_GAME,
        filePattern_4,
        gcGamePath,
        filePattern_4
    );
    if (requester_11 == NULL)
        MemError();
    dialogResult_18 = gpExec->DoDialog(requester_11);
    if (dialogResult_18 == FILE_REQUESTER_OK) {
        gpGame->LoadGame(gLastFilename, 0, 0);
        delete requester_11;
        return 1;
    } else {
        delete requester_11;
        return 0;
    }
}

VA(0x00412676, 0x1e)
i32 SetupCampaignGameHandler(struct tag_message& message) {
    return BaseSetupHandler(message);
}

VA(0x00412694, 0x13e)
i32 SetupComPortHandler(struct tag_message& message) {
    i32 helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0
        && (message.payload.widget.command == WIDGET_COMMAND_SELECT
            || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = GetSetupHelpIndex(message.payload.widget.id, CHOICE_FOUR);
        if (helpIndex >= FIRST_HELP) {
            if (gbDirectConnect != 0)
                NormalDialog(
                    gSetupDCComPortHelp[helpIndex],
                    HELP_DIALOG,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            else
                NormalDialog(
                    gSetupComPortHelp[helpIndex],
                    HELP_DIALOG,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
        }
    }
    return BaseSetupHandler(message);
}

VA(0x004127d2, 0x13e)
i32 SetupBaudHandler(struct tag_message& message) {
    i32 helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0
        && (message.payload.widget.command == WIDGET_COMMAND_SELECT
            || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = GetSetupHelpIndex(message.payload.widget.id, CHOICE_FOUR);
        if (helpIndex >= FIRST_HELP) {
            if (gbDirectConnect != 0)
                NormalDialog(
                    gSetupDCBaudHelp[helpIndex],
                    HELP_DIALOG,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            else
                NormalDialog(
                    gSetupBaudHelp[helpIndex],
                    HELP_DIALOG,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
        }
    }
    return BaseSetupHandler(message);
}

VA(0x00412910, 0x118)
i32 SetupHotSeatGameHandler(struct tag_message& message) {
    i32 helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0
        && (message.payload.widget.command == WIDGET_COMMAND_SELECT
            || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = GetSetupHelpIndex(message.payload.widget.id, CHOICE_FIVE);
        if (helpIndex >= FIRST_HELP)
            NormalDialog(
                gSetupHotSeatGameHelp[helpIndex],
                HELP_DIALOG,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
    }
    return BaseSetupHandler(message);
}

VA(0x00412a28, 0x12e)
i32 SetupModemGameHandler(struct tag_message& message) {
    i32 helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0
        && (message.payload.widget.command == WIDGET_COMMAND_SELECT
            || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = GetSetupHelpIndex(message.payload.widget.id, CHOICE_THREE);
        if (helpIndex >= FIRST_HELP) {
            if (gbDirectConnect != 0)
                NormalDialog(
                    gSetupDCGameHelp[helpIndex],
                    HELP_DIALOG,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            else
                NormalDialog(
                    gSetupModemGameHelp[helpIndex],
                    HELP_DIALOG,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
        }
    }
    return BaseSetupHandler(message);
}

VA(0x00412b56, 0x108)
i32 SetupMultiPlayerGameHandler(struct tag_message& message) {
    i32 helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0
        && (message.payload.widget.command == WIDGET_COMMAND_SELECT
            || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = GetSetupHelpIndex(message.payload.widget.id, CHOICE_FOUR);
        if (helpIndex >= FIRST_HELP)
            NormalDialog(
                gSetupMultiPlayerGameHelp[helpIndex],
                HELP_DIALOG,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
    }
    return BaseSetupHandler(message);
}

VA(0x00412c5e, 0xd7)
i32 SetupNetworkGameHandler(struct tag_message& message) {
    i32 helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0
        && (message.payload.widget.command == WIDGET_COMMAND_SELECT
            || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = GetSetupHelpIndex(message.payload.widget.id, CHOICE_TWO);
        if (helpIndex >= FIRST_HELP)
            NormalDialog(
                gSetupNetworkGameHelp[helpIndex],
                HELP_DIALOG,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
    }
    return BaseSetupHandler(message);
}

VA(0x00412d35, 0xf8)
i32 SetupNetworkGame2Handler(struct tag_message& message) {
    i32 helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0
        && (message.payload.widget.command == WIDGET_COMMAND_SELECT
            || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = GetSetupHelpIndex(message.payload.widget.id, CHOICE_THREE);
        if (helpIndex >= FIRST_HELP)
            NormalDialog(
                gSetupNetworkGame2Help[helpIndex],
                HELP_DIALOG,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
    }
    return BaseSetupHandler(message);
}

VA(0x00412e2d, 0x14b)
i32 SetupGameHandler(struct tag_message& message) {
    i32 helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0) {
        if (message.payload.widget.command == WIDGET_COMMAND_SELECT
            || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT) {
            helpIndex = GetSetupHelpIndex(message.payload.widget.id, CHOICE_THREE);
            if (helpIndex >= FIRST_HELP)
                NormalDialog(
                    gSetupGameHelp[helpIndex],
                    HELP_DIALOG,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
        }
    } else if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case CHOICE_ONE:
                    case CHOICE_TWO:
                    case CHOICE_THREE:
                        break;
                }
        }
    }
    return BaseSetupHandler(message);
}

VA(0x00412f78, 0xd7)
i32 ExpNewCampaignHandler(struct tag_message& message) {
    i32 helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0
        && (message.payload.widget.command == WIDGET_COMMAND_SELECT
            || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = GetSetupHelpIndex(message.payload.widget.id, CHOICE_TWO);
        if (helpIndex >= FIRST_HELP)
            NormalDialog(
                xSetupCampaignGameHelp[helpIndex],
                HELP_DIALOG,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
    }
    return BaseSetupHandler(message);
}

VA(0x0041304f, 0xd7)
i32 ExpLoadCampaignHandler(struct tag_message& message) {
    i32 helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0
        && (message.payload.widget.command == WIDGET_COMMAND_SELECT
            || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = GetSetupHelpIndex(message.payload.widget.id, CHOICE_TWO);
        if (helpIndex >= FIRST_HELP)
            NormalDialog(
                xSetupCampaignGameHelp[helpIndex],
                HELP_DIALOG,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
    }
    return BaseSetupHandler(message);
}

VA(0x00413126, 0xd7)
i32 ExpStdGameHandler(struct tag_message& message) {
    i32 helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0
        && (message.payload.widget.command == WIDGET_COMMAND_SELECT
            || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = GetSetupHelpIndex(message.payload.widget.id, CHOICE_TWO);
        if (helpIndex >= FIRST_HELP)
            NormalDialog(
                xSetupStandardGameHelp[helpIndex],
                HELP_DIALOG,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
    }
    return BaseSetupHandler(message);
}

VA(0x004131fd, 0xf0)
i32 BaseSetupHandler(struct tag_message& message) {
    i32 handled = 0;

    PollSound();
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                if ((message.payload.widget.id >= DIALOG_RESULT_MIN
                     && message.payload.widget.id <= DIALOG_RESULT_MAX)
                    || message.payload.widget.id == DIALOG_CANCEL)
                    handled = 1;
        }
    }

    if (handled || giMenuCommand != -1) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = WIDGET_COMMAND_DIALOG_SELECT;
        message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
        if (giMenuCommand != -1)
            gpWindowManager->m_dialogResult = DIALOG_CANCEL;
        return HANDLER_CLOSE;
    }

    CheckShingleUpdate();
    return HANDLER_CONTINUE;
}

DATA(0x004ee248) b32 gbDoModemConfig = false;
