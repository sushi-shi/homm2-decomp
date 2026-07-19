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
    WINDOW_X                = 405,
    WINDOW_Y                = 8,
    DIALOG_CANCEL           = 0x7801,
    DIALOG_YES              = 0x7805,
    PLAYER_COUNT            = IDX(GAME_PLAYER_COUNT),
    PLAYER_NAME_LENGTH      = 20,
    MODEM_INIT_ENTRY_LENGTH = 40,
    TELEPHONE_ENTRY_LENGTH  = 35,
    DISABLED_WIDGET_ID      = 1,
    LOG_UNUSED              = -999,
    CAMPAIGN_INTRO          = 4,
    CAMPAIGN_SELECTION      = 35,
    HELP_DIALOG             = 4,
    DIALOG_RESULT_MIN       = 1,
    DIALOG_RESULT_MAX       = 1000,
    HANDLER_CONTINUE        = 1,
    HANDLER_CLOSE           = 2
H2_ENUM_END(SetupConstant)

H2_ENUM_BEGIN(SetupRemoteType)
    REMOTE_NETWORK_HOST  = 1,
    REMOTE_NETWORK_GUEST = 2,
    REMOTE_MODEM_DIAL    = 3,
    REMOTE_MODEM_ANSWER  = 4
H2_ENUM_END(SetupRemoteType)

H2_ENUM_CLASS_BEGIN(SetupBaudRate)
    SETUP_BAUD_2400  = 2400,
    SETUP_BAUD_9600  = 9600,
    SETUP_BAUD_19200 = 19200,
    SETUP_BAUD_38400 = 38400
H2_ENUM_CLASS_END(SetupBaudRate)

VA(0x00410e90, 0x2f)
i32 game::SetupCampaignGame(void) {
    PlaySmacker(CAMPAIGN_INTRO);
    PlaySmacker(CAMPAIGN_SELECTION);
    return 1;
}

// @early-stop: delinker artifact.
VA(0x00410ebf, 0x141)
i32 game::SetupBaud(void) {
    heroWindow* window = new heroWindow(WINDOW_X, WINDOW_Y, "stpbaud.bin");
    if (window == NULL)
        MemError();
    gpWindowManager->DoDialog(window, SetupBaudHandler, 0);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
        case 1:
            gConfig.baudRate[gbDirectConnect] = IDX(SETUP_BAUD_2400);
            break;
        case 2:
            gConfig.baudRate[gbDirectConnect] = IDX(SETUP_BAUD_9600);
            break;
        case 3:
            gConfig.baudRate[gbDirectConnect] = IDX(SETUP_BAUD_19200);
            break;
        case 4:
            gConfig.baudRate[gbDirectConnect] = IDX(SETUP_BAUD_38400);
            break;
        case DIALOG_CANCEL:
            return 0;
    }
    return 1;
}

// @early-stop: delinker jump-table artifact.
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
        case 1:
            gConfig.comPort[gbDirectConnect] = IDX(SETUP_COM_PORT_1);
            break;
        case 2:
            gConfig.comPort[gbDirectConnect] = IDX(SETUP_COM_PORT_2);
            break;
        case 3:
            gConfig.comPort[gbDirectConnect] = IDX(SETUP_COM_PORT_3);
            break;
        case 4:
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
    char defaultName[24];
    i32 ix;

    heroWindow* window = new heroWindow(WINDOW_X, WINDOW_Y, "stphotst.bin");
    if (window == NULL)
        MemError();
    gpWindowManager->DoDialog(window, SetupHotSeatGameHandler, 0);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
        case 1:
            giNumHumanPlayers = 2;
            break;
        case 2:
            giNumHumanPlayers = 3;
            break;
        case 3:
            giNumHumanPlayers = 4;
            break;
        case 4:
            giNumHumanPlayers = 5;
            break;
        case 5:
            giNumHumanPlayers = 6;
            break;
        case DIALOG_CANCEL:
            return 0;
    }

    for (ix = 0; ix < PLAYER_COUNT; ix++)
        strcpy(cPlayerNames[ix], "");

    if (giSetupGameType == 0) {
        sprintf(gText, "Do you wish to enter each player's name?");
        NormalDialog(gText, 2, -1, -1, -1, 0, -1, 0, -1, 0);
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
        case 1:
            iMPExtendedType = REMOTE_NETWORK_HOST;
            break;
        case 2:
            iMPExtendedType = REMOTE_NETWORK_GUEST;
            break;
        case DIALOG_CANCEL:
            return 0;
    }
    return 1;
}

// @early-stop: delinker jump-table artifact.
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
        LOG_UNUSED,
        LOG_UNUSED,
        LOG_UNUSED,
        LOG_UNUSED,
        LOG_UNUSED
    );
    if (result != 0 && osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
        message.payload.widget.id = 1;
        window->BroadcastMessage(message);
        message.payload.widget.id = 3;
        window->BroadcastMessage(message);
    }

    gpWindowManager->DoDialog(window, SetupNetworkGame2Handler, 0);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
        case 1:
            iMPNetProtocol = REMOTE_PROTOCOL_DIRECT_PLAY;
            break;
        case 2:
            iMPNetProtocol = REMOTE_PROTOCOL_WINSOCK;
            break;
        case 3:
            iMPNetProtocol = REMOTE_PROTOCOL_MODEM;
            break;
        case 4:
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
        case 1:
            LogStr("SMC 6");
            iMPExtendedType = REMOTE_MODEM_DIAL;
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
        case 2:
            iMPExtendedType = REMOTE_MODEM_ANSWER;
            if (gConfig.comPort[gbDirectConnect] == IDX(SETUP_COM_PORT_UNCONFIGURED)
                && !SetupComPort())
                return 0;
            break;
        case 3:
            gbDoModemConfig = true;
            break;
        case DIALOG_CANCEL:
            return 0;
    }
    return 1;
}

// @early-stop: delinker jump-table artifact.
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
        case 1:
            iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
            if (!SetupHotSeatGame())
                return 0;
            break;
        case 2:
            iMPBaseType = MULTIPLAYER_BASE_NETWORK;
            if (!SetupNetworkGame2())
                return 0;
            break;
        case 4:
            gbDirectConnect = true;
            goto setupModem;
        case 3:
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
    iMPExtendedType = 10;
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
                giNumHumanPlayers = 2;
                iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
                break;
            case APP_MENU_RESTART_6:
            case APP_MENU_LOAD_3:
                giNumHumanPlayers = 3;
                iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
                break;
            case APP_MENU_RESTART_7:
            case APP_MENU_LOAD_4:
                giNumHumanPlayers = 4;
                iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
                break;

            case APP_MENU_RESTART_8:
            case APP_MENU_LOAD_5:
                iMPBaseType = MULTIPLAYER_BASE_NETWORK;
                iMPExtendedType = REMOTE_NETWORK_HOST;
                goto remoteSetup;
            case APP_MENU_RESTART_9:
            case APP_MENU_LOAD_6:
                iMPBaseType = MULTIPLAYER_BASE_NETWORK;
                iMPExtendedType = REMOTE_NETWORK_GUEST;
                goto remoteSetup;
            case APP_MENU_RESTART_10:
            case APP_MENU_LOAD_7:
                iMPBaseType = MULTIPLAYER_BASE_MODEM;
                iMPExtendedType = REMOTE_MODEM_DIAL;
                goto remoteSetup;
            case APP_MENU_RESTART_11:
            case APP_MENU_LOAD_8:
                iMPBaseType = MULTIPLAYER_BASE_MODEM;
                iMPExtendedType = REMOTE_MODEM_ANSWER;
                goto remoteSetup;
            case APP_MENU_RESTART_12:
            case APP_MENU_LOAD_9:
                iMPBaseType = MULTIPLAYER_BASE_MODEM;
                iMPExtendedType = REMOTE_MODEM_DIAL;
                gbDirectConnect = true;
                goto remoteSetup;
            case APP_MENU_RESTART_13:
            case APP_MENU_LOAD_10:
                iMPBaseType = MULTIPLAYER_BASE_MODEM;
                iMPExtendedType = REMOTE_MODEM_ANSWER;
                gbDirectConnect = true;
                goto remoteSetup;

            remoteSetup:
                LogStr("Setup 0a");
                RemoteMain(iMPExtendedType);
                if (iMPExtendedType == IDX(REMOTE_NETWORK_GUEST)
                    || iMPExtendedType == IDX(REMOTE_MODEM_ANSWER))
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
        message.payload.widget.id = 1;
        window->BroadcastMessage(message);
        message.payload.widget.id = 2;
        window->BroadcastMessage(message);
    }

    gpWindowManager->DoDialog(window, SetupGameHandler, 0);
    delete window;

    switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
        case 1:
            break;

        case 2:
            if (giSetupGameType == OLD_MAIN_SETUP_LOAD) {
                window = new heroWindow(WINDOW_X, WINDOW_Y, "x_loadcm.bin");
                if (window == NULL)
                    MemError();
                gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
                delete window;

                switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
                    case 1:
                        gbInCampaign = true;
                        break;
                    case 2:
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
                    case 1:
                        gbInCampaign = true;
                        if (!SetupCampaignGame()) {
                            result = 0;
                            goto done;
                        }
                        break;
                    case 2:
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

        case 3:
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
        RemoteMain(iMPExtendedType);
        LogStr(" Setup 3");
        if (iMPExtendedType == IDX(REMOTE_NETWORK_GUEST)
            || iMPExtendedType == IDX(REMOTE_MODEM_ANSWER))
            gbWaitForRemoteReceive = true;
    }

done:
    gbInSetupDialog = false;
    return result;
}

VA(0x004123cc, 0x2aa)
i32 game::PickLoadGame(void) {
    char filePattern_4[12];
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
            1,
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
            case 1:
                xIsExpansionMap = 0;
                break;
            case 2:
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
        200,
        58,
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
        helpIndex = -1;
        switch (message.payload.widget.id) {
            case 1:
                helpIndex = 0;
                break;
            case 2:
                helpIndex = 1;
                break;
            case 3:
                helpIndex = 2;
                break;
            case 4:
                helpIndex = 3;
                break;
            case DIALOG_CANCEL:
                helpIndex = 4;
                break;
        }
        if (helpIndex >= 0) {
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
        helpIndex = -1;
        switch (message.payload.widget.id) {
            case 1:
                helpIndex = 0;
                break;
            case 2:
                helpIndex = 1;
                break;
            case 3:
                helpIndex = 2;
                break;
            case 4:
                helpIndex = 3;
                break;
            case DIALOG_CANCEL:
                helpIndex = 4;
                break;
        }
        if (helpIndex >= 0) {
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
        helpIndex = -1;
        switch (message.payload.widget.id) {
            case 1:
                helpIndex = 0;
                break;
            case 2:
                helpIndex = 1;
                break;
            case 3:
                helpIndex = 2;
                break;
            case 4:
                helpIndex = 3;
                break;
            case 5:
                helpIndex = 4;
                break;
            case DIALOG_CANCEL:
                helpIndex = 5;
                break;
        }
        if (helpIndex >= 0)
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
        helpIndex = -1;
        switch (message.payload.widget.id) {
            case 1:
                helpIndex = 0;
                break;
            case 2:
                helpIndex = 1;
                break;
            case 3:
                helpIndex = 2;
                break;
            case DIALOG_CANCEL:
                helpIndex = 3;
                break;
        }
        if (helpIndex >= 0) {
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
        helpIndex = -1;
        switch (message.payload.widget.id) {
            case 1:
                helpIndex = 0;
                break;
            case 2:
                helpIndex = 1;
                break;
            case 3:
                helpIndex = 2;
                break;
            case 4:
                helpIndex = 3;
                break;
            case DIALOG_CANCEL:
                helpIndex = 4;
                break;
        }
        if (helpIndex >= 0)
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
        helpIndex = -1;
        switch (message.payload.widget.id) {
            case 1:
                helpIndex = 0;
                break;
            case 2:
                helpIndex = 1;
                break;
            case DIALOG_CANCEL:
                helpIndex = 2;
                break;
        }
        if (helpIndex >= 0)
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
        helpIndex = -1;
        switch (message.payload.widget.id) {
            case 1:
                helpIndex = 0;
                break;
            case 2:
                helpIndex = 1;
                break;
            case 3:
                helpIndex = 2;
                break;
            case DIALOG_CANCEL:
                helpIndex = 3;
                break;
        }
        if (helpIndex >= 0)
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
            helpIndex = -1;
            switch (message.payload.widget.id) {
                case 1:
                    helpIndex = 0;
                    break;
                case 2:
                    helpIndex = 1;
                    break;
                case 3:
                    helpIndex = 2;
                    break;
                case DIALOG_CANCEL:
                    helpIndex = 3;
                    break;
            }
            if (helpIndex >= 0)
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
                    case 1:
                    case 2:
                    case 3:
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
        helpIndex = -1;
        switch (message.payload.widget.id) {
            case 1:
                helpIndex = 0;
                break;
            case 2:
                helpIndex = 1;
                break;
            case DIALOG_CANCEL:
                helpIndex = 2;
                break;
        }
        if (helpIndex >= 0)
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
        helpIndex = -1;
        switch (message.payload.widget.id) {
            case 1:
                helpIndex = 0;
                break;
            case 2:
                helpIndex = 1;
                break;
            case DIALOG_CANCEL:
                helpIndex = 2;
                break;
        }
        if (helpIndex >= 0)
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
        helpIndex = -1;
        switch (message.payload.widget.id) {
            case 1:
                helpIndex = 0;
                break;
            case 2:
                helpIndex = 1;
                break;
            case DIALOG_CANCEL:
                helpIndex = 2;
                break;
        }
        if (helpIndex >= 0)
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
                if ((message.payload.widget.id > 0
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
