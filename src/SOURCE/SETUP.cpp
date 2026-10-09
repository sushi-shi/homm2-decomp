#include <H2/Ints.h>
#include <BASE/Utf8.h>
#include <BASE/Misc.h>
#include <BASE/executive.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/widget.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/KB.h>
#include <SOURCE/Modem.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/smackManager.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/fileRequester.h>
#include <SOURCE/game.h>
#include <IRONFIST/campaigns.h>
#include <IRONFIST/save_xml.h>
#include <SOURCE/netwin.h>
#include <SOURCE/SETUP.h>
#include <stdio.h>
#include <string.h>
#include <SOURCE/Localization.h>
#include <BASE/dialog.h>
typedef enum SetupConstant {
    PLAYER_NAME_LENGTH           = GLOBAL_PLAYER_NAME_SIZE - 1,
    DEFAULT_PLAYER_NAME_CAPACITY = 24,
    MODEM_INIT_ENTRY_LENGTH      = 40,
    TELEPHONE_ENTRY_LENGTH       = MODEM_NUMBER_BUFFER_SIZE - 1,
    FILE_PATTERN_CAPACITY        = 12,
    FILE_REQUESTER_X             = 200,
    FILE_REQUESTER_Y             = 58,
    DIALOG_RESULT_MAX            = 1000,
} SetupConstant;

typedef enum HotSeatPlayerCount {
    TWO_PLAYERS = 2,
    THREE_PLAYERS = 3,
    FOUR_PLAYERS = 4,
    FIVE_PLAYERS = 5,
    SIX_PLAYERS = 6
} HotSeatPlayerCount;

i32 game::SetupCampaignGame(void) {
    PlaySmacker(CAMPAIGN_INTRO);
    PlaySmacker(CHOOSE_CAMPAIGN);
    return 1;
}

b32 game::SetupBaud(void) {
    heroWindow* window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpbaud.bin");
    if (window == NULL)
        MemError();
    gpWindowManager->DoDialog(window, SetupBaudHandler, false);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            gConfig.baudRate[gbDirectConnect] = CONFIG_BAUD_2400;
            break;
        case CHOICE_TWO:
            gConfig.baudRate[gbDirectConnect] = CONFIG_BAUD_9600;
            break;
        case CHOICE_THREE:
            gConfig.baudRate[gbDirectConnect] = CONFIG_BAUD_19200;
            break;
        case CHOICE_FOUR:
            gConfig.baudRate[gbDirectConnect] = CONFIG_BAUD_38400;
            break;
        case DIALOG_CANCEL:
            return false;
    }
    return true;
}

b32 game::SetupComPort(void) {
    char initString[MODEM_INIT_ENTRY_LENGTH];

    LogStr("SCP 1");
    heroWindow* setupWindow = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpcom.bin");
    if (setupWindow == NULL)
        MemError();
    LogStr("SCP 2");
    gpWindowManager->DoDialog(setupWindow, SetupComPortHandler, false);
    delete setupWindow;
    LogStr("SCP 3");

    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            gConfig.comPort[gbDirectConnect] = CONFIG_COM_PORT_1;
            break;
        case CHOICE_TWO:
            gConfig.comPort[gbDirectConnect] = CONFIG_COM_PORT_2;
            break;
        case CHOICE_THREE:
            gConfig.comPort[gbDirectConnect] = CONFIG_COM_PORT_3;
            break;
        case CHOICE_FOUR:
            gConfig.comPort[gbDirectConnect] = CONFIG_COM_PORT_4;
            break;
        case DIALOG_CANCEL:
            return false;
    }

    LogStr("SCP 4");
    if (!SetupBaud())
        return false;
    LogStr("SCP 5");
    if (gbDirectConnect == false) {
        strcpy(gConfig.modemInitString, "ATZ");
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%s", gConfig.modemInitString);
        GetDataEntry(
            localization::Tr("network.modem.initialization_prompt"),
            initString,
            MODEM_INIT_ENTRY_LENGTH,
            gText,
            false,
            true
        );
        strcpy(gConfig.modemInitString, initString);
    }
    WritePrefs();
    return true;
}

b32 game::SetupHotSeatGame(void) {
    i32 i;
    char name[DEFAULT_PLAYER_NAME_CAPACITY];

    heroWindow* dialogWindow = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stphotst.bin");
    if (dialogWindow == NULL)
        MemError();
    gpWindowManager->DoDialog(dialogWindow, SetupHotSeatGameHandler, false);
    delete dialogWindow;

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
            return false;
    }

    for (i = 0; i < H2EnumIndex(GAME_PLAYER_COUNT); i++)
        strcpy(
            cPlayerNames[i],
            ""
        );

    if (giSetupGameType == OLD_MAIN_SETUP_NEW) {
        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("network.hotseat.enter_names_prompt"));
        NormalDialog(gText, NORMAL_DIALOG_CONFIRM);
        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
            for (i = 0; i < giNumHumanPlayers; i++) {
                strcpy(
                    name,
                    ""
                );
                utf8::Format(
                    gText, GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("network.hotseat.player_name_prompt"),
                    i + 1
                );
                GetDataEntry(gText, cPlayerNames[i], PLAYER_NAME_LENGTH, name, false, true);
            }
        }
    }
    return true;
}

b32 game::SetupNetworkGame(void) {
    tag_message message;
    heroWindow* window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpnet.bin");
    if (window == NULL)
        MemError();

    if (gbNoCDRom != 0) {
        SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FLAGS, CHOICE_ONE);
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAGS_ARGUMENT_DIMMED);
        window->BroadcastMessage(message);
    }

    gpWindowManager->DoDialog(window, SetupNetworkGameHandler, false);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            iMPExtendedType = REMOTE_GAME_NETWORK_HOST;
            break;
        case CHOICE_TWO:
            iMPExtendedType = REMOTE_GAME_NETWORK_GUEST;
            break;
        case DIALOG_CANCEL:
            return false;
    }
    return true;
}

b32 game::SetupNetworkGame2(void) {
    tag_message message;

    heroWindow* dialogWindow = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpnet2.bin");
    if (dialogWindow == NULL)
        MemError();

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
    message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAGS_ARGUMENT_DIMMED);
    message.payload.widget.id = CHOICE_ONE;
    dialogWindow->BroadcastMessage(message);
    message.payload.widget.id = CHOICE_THREE;
    dialogWindow->BroadcastMessage(message);

    gpWindowManager->DoDialog(dialogWindow, SetupNetworkGame2Handler, false);
    delete dialogWindow;

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
            return false;
    }
    if (!SetupNetworkGame())
        return false;
    else
        return true;
}

b32 game::SetupModemGame(void) {
    tag_message message;
    heroWindow* window;

    LogStr("SMC 1");
    if (gbDirectConnect != false) {
        if (gConfig.comPort[gbDirectConnect] == CONFIG_COM_PORT_UNCONFIGURED)
            window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpdc.bin");
        else
            window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpdccfg.bin");
    } else {
        if (gConfig.comPort[gbDirectConnect] == CONFIG_COM_PORT_UNCONFIGURED)
            window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpmodem.bin");
        else
            window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpmcfg.bin");
    }
    if (window == NULL)
        MemError();

    LogStr("SMC 2");
    if (gbNoCDRom != 0) {
        SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FLAGS, CHOICE_ONE);
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAGS_ARGUMENT_DIMMED);
        window->BroadcastMessage(message);
    }
    LogStr("SMC 3");
    gpWindowManager->DoDialog(window, SetupModemGameHandler, false);
    LogStr("SMC 4");
    delete window;
    LogStr("SMC 5");

    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            LogStr("SMC 6");
            iMPExtendedType = REMOTE_GAME_MODEM_HOST;
            if (gConfig.comPort[gbDirectConnect] == CONFIG_COM_PORT_UNCONFIGURED) {
                LogStr("SMC 7");
                if (!SetupComPort())
                    return false;
                LogStr("SMC 8");
            }
            LogStr("SMC 9");
            if (gbDirectConnect == false) {
                GetDataEntry(
                    localization::Tr("network.modem.telephone_prompt")
                    ,
                    numbuf,
                    TELEPHONE_ENTRY_LENGTH,
                    NULL,
                    false,
                    true
                );
            }
            LogStr("SMC a");
            break;
        case CHOICE_TWO:
            iMPExtendedType = REMOTE_GAME_MODEM_GUEST;
            if (gConfig.comPort[gbDirectConnect] == CONFIG_COM_PORT_UNCONFIGURED && !SetupComPort())
                return false;
            break;
        case CHOICE_THREE:
            gbDoModemConfig = true;
            break;
        case DIALOG_CANCEL:
            return false;
    }
    return true;
}

b32 game::SetupMultiPlayerGame(void) {
    tag_message message;
    b32 continueFlag;

    heroWindow* window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpmp.bin");
    if (window == NULL)
        MemError();

    if (gbNoCDRom != 0) {
        SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FLAGS, CHOICE_ONE);
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAGS_ARGUMENT_DIMMED);
        window->BroadcastMessage(message);
    }
    gpWindowManager->DoDialog(window, SetupMultiPlayerGameHandler, false);
    delete window;

    gbDirectConnect = false;
    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
            if (!SetupHotSeatGame())
                return false;
            break;
        case CHOICE_TWO:
            iMPBaseType = MULTIPLAYER_BASE_NETWORK;
            if (!SetupNetworkGame2())
                return false;
            break;
        case CHOICE_FOUR:
            gbDirectConnect = true;
            goto setupModem;
        case CHOICE_THREE:
            gbDirectConnect = false;
        setupModem:
            iMPBaseType = MULTIPLAYER_BASE_MODEM;
            continueFlag = true;
            LogStr("Common Modem 1");
            while (continueFlag) {
                LogStr("Common Modem 2");
                if (!SetupModemGame())
                    return false;
                LogStr("Common Modem 3");
                if (gbDoModemConfig != false) {
                    LogStr("Common Modem 4");
                    gbDoModemConfig = false;
                    if (!SetupComPort())
                        return false;
                    LogStr("Common Modem 5");
                } else {
                    continueFlag = false;
                }
                LogStr("Common Modem 6");
            }
            LogStr("Common Modem 7");
            break;
        case DIALOG_CANCEL:
            return false;
    }
    return true;
}

b32 game::SetupGame(void) {
    heroWindow* window;
    b32 result;

    LogStr("Setup 0");
    result = true;
    xIsPlayingExpansionCampaign = 0;
    xIsExpansionMap = 0;
    gbInCampaign = false;
    gbCampaignSideChoice = CAMPAIGN_ROLAND;
    iMPExtendedType = REMOTE_GAME_UNINITIALIZED;
    iMPBaseType = MULTIPLAYER_BASE_UNINITIALIZED;
    giNumHumanPlayers = 1;
    gbWaitForRemoteReceive = false;
    gbDirectConnect = false;
    gbInSetupDialog = true;

    if (giMenuCommand != -1) {
        switch (giMenuCommand) {
            case APP_MENU_NEW_STANDARD_GAME:
            case APP_MENU_LOAD_STANDARD_GAME:
                break;

            case APP_MENU_NEW_HOT_SEAT_2:
            case APP_MENU_LOAD_HOT_SEAT_2:
                giNumHumanPlayers = TWO_PLAYERS;
                iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
                break;
            case APP_MENU_NEW_HOT_SEAT_3:
            case APP_MENU_LOAD_HOT_SEAT_3:
                giNumHumanPlayers = THREE_PLAYERS;
                iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
                break;
            case APP_MENU_NEW_HOT_SEAT_4:
            case APP_MENU_LOAD_HOT_SEAT_4:
                giNumHumanPlayers = FOUR_PLAYERS;
                iMPBaseType = MULTIPLAYER_BASE_HOT_SEAT;
                break;

            case APP_MENU_NEW_NETWORK_HOST:
            case APP_MENU_LOAD_NETWORK_HOST:
                iMPBaseType = MULTIPLAYER_BASE_NETWORK;
                iMPExtendedType = REMOTE_GAME_NETWORK_HOST;
                goto remoteSetup;
            case APP_MENU_NEW_NETWORK_GUEST:
            case APP_MENU_LOAD_NETWORK_GUEST:
                iMPBaseType = MULTIPLAYER_BASE_NETWORK;
                iMPExtendedType = REMOTE_GAME_NETWORK_GUEST;
                goto remoteSetup;
            case APP_MENU_NEW_MODEM_HOST:
            case APP_MENU_LOAD_MODEM_HOST:
                iMPBaseType = MULTIPLAYER_BASE_MODEM;
                iMPExtendedType = REMOTE_GAME_MODEM_HOST;
                goto remoteSetup;
            case APP_MENU_NEW_MODEM_GUEST:
            case APP_MENU_LOAD_MODEM_GUEST:
                iMPBaseType = MULTIPLAYER_BASE_MODEM;
                iMPExtendedType = REMOTE_GAME_MODEM_GUEST;
                goto remoteSetup;
            case APP_MENU_NEW_DIRECT_HOST:
            case APP_MENU_LOAD_DIRECT_HOST:
                iMPBaseType = MULTIPLAYER_BASE_MODEM;
                iMPExtendedType = REMOTE_GAME_MODEM_HOST;
                gbDirectConnect = true;
                goto remoteSetup;
            case APP_MENU_NEW_DIRECT_GUEST:
            case APP_MENU_LOAD_DIRECT_GUEST:
                iMPBaseType = MULTIPLAYER_BASE_MODEM;
                iMPExtendedType = REMOTE_GAME_MODEM_GUEST;
                gbDirectConnect = true;
                goto remoteSetup;

            remoteSetup:
                LogStr("Setup 0a");
                RemoteMain(iMPExtendedType);
                if (iMPExtendedType == REMOTE_GAME_NETWORK_GUEST
                    || iMPExtendedType == REMOTE_GAME_MODEM_GUEST)
                    gbWaitForRemoteReceive = true;
                break;
        }

        giMenuCommand = -1;
        result = true;
        goto done;
    }

    window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpnewgm.bin");
    if (window == NULL)
        MemError();

    if (gbNoCDRom != 0) {
        tag_message message;
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAGS_ARGUMENT_DIMMED);
        message.payload.widget.id = CHOICE_ONE;
        window->BroadcastMessage(message);
        message.payload.widget.id = CHOICE_TWO;
        window->BroadcastMessage(message);
    }

    gpWindowManager->DoDialog(window, SetupGameHandler, false);
    delete window;

    switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
        case CHOICE_ONE:
            break;

        case CHOICE_TWO:
            if (giSetupGameType == OLD_MAIN_SETUP_LOAD) {
                window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "x_loadcm.bin");
                if (window == NULL)
                    MemError();
                gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, false);
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
                        result = false;
                        goto done;
                }
            } else {
                window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "x_loadcm.bin");
                if (window == NULL)
                    MemError();
                gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, false);
                delete window;

                switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
                    case CHOICE_ONE:
                        gbInCampaign = true;
                        if (!SetupCampaignGame()) {
                            result = false;
                            goto done;
                        }
                        break;
                    case CHOICE_TWO:
                        xIsPlayingExpansionCampaign = 1;
                        xIsExpansionMap = 1;
                        xCampaign.InitNewCampaign(xCampaign.Choose());
                        break;
                    case CHOICE_THREE: {
                        // The bundled Ironfist campaign.
                        xIsPlayingExpansionCampaign = 1;
                        xIsExpansionMap = 1;
                        i32 campaignId = LoadCampaignFromFile("cyborg.cmp");
                        if (campaignId == -1) {
                            result = false;
                            goto done;
                        }
        xCampaign.InitNewCampaign(ExpansionCampaignIdFromOrdinal(campaignId));
                        break;
                    }
                    case CHOICE_FOUR: {
                        // Any other campaign out of CAMPAIGNS/*.cmp.  The
                        // requester hardcodes error text for retail saves;
                        // keep it quiet while it browses campaign files.
                        xIsPlayingExpansionCampaign = 1;
                        xIsExpansionMap = 1;
                        i32 savedDebugLevel = giDebugLevel;
                        i32 savedHumanCount = iWSLastMsgNumHumanPlayers;
                        iWSLastMsgNumHumanPlayers = 999;
                        giDebugLevel = 3;
                        fileRequester* campaignRequester = new fileRequester(
                            FILE_REQUESTER_X,
                            FILE_REQUESTER_Y,
                            FILE_REQUESTER_LOAD_GAME,
                            "*.cmp",
                            ".\\CAMPAIGNS\\",
                            "*.cmp"
                        );
                        if (campaignRequester == NULL)
                            MemError();
                        i32 requesterResult = gpExec->DoDialog(campaignRequester);
                        delete campaignRequester;
                        giDebugLevel = savedDebugLevel;
                        iWSLastMsgNumHumanPlayers = savedHumanCount;
                        if (requesterResult != FILE_REQUESTER_OK) {
                            result = false;
                            goto done;
                        }
                        i32 campaignId = LoadCampaignFromFile(gLastFilename);
                        if (campaignId == -1) {
                            result = false;
                            goto done;
                        }
        xCampaign.InitNewCampaign(ExpansionCampaignIdFromOrdinal(campaignId));
                        break;
                    }
                    case DIALOG_CANCEL:
                        result = false;
                        goto done;
                }
            }
            break;

        case CHOICE_THREE:
            if (!SetupMultiPlayerGame()) {
                result = false;
                goto done;
            }
            break;

        case DIALOG_CANCEL:
            result = false;
            goto done;
    }

    LogStr(" Setup 1");
    if (iMPBaseType == MULTIPLAYER_BASE_NETWORK || iMPBaseType == MULTIPLAYER_BASE_MODEM) {
        LogStr(" Setup 2");
        RemoteMain(iMPExtendedType);
        LogStr(" Setup 3");
        if (iMPExtendedType == REMOTE_GAME_NETWORK_GUEST
            || iMPExtendedType == REMOTE_GAME_MODEM_GUEST)
            gbWaitForRemoteReceive = true;
    }

done:
    gbInSetupDialog = false;
    return result;
}

b32 game::PickLoadGame(void) {
    char fileMask[FILE_PATTERN_CAPACITY];
    i32 dialogResult;
    heroWindow* loadWindow;
    fileRequester* fileReq;

    if (gbWaitForRemoteReceive != false)
        return true;

    if (gbInCampaign != false || xIsPlayingExpansionCampaign != 0) {
        // Campaign saves pick their extension by campaign type, custom
        // campaigns included.
        utf8::Format(fileMask, sizeof(fileMask), "*%s", GetSaveFileExtension(1).c_str());
    } else if (gbRemoteOn != false && xNetHasOldPlayers != false) {
        NormalDialog(localization::Tr("network.load.expansion_unavailable"), NORMAL_DIALOG_INFO);
        utf8::Format(fileMask, "*.GM%d", giNumHumanPlayers);
    } else {
        loadWindow = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "x_mapmnu.bin");
        if (loadWindow == NULL)
            MemError();
        gpWindowManager->DoDialog(loadWindow, ExpStdGameHandler, false);
        delete loadWindow;

        switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
            case CHOICE_ONE:
                xIsExpansionMap = 0;
                break;
            case CHOICE_TWO:
                xIsExpansionMap = 1;
                break;
            case DIALOG_CANCEL:
                return false;
        }

        if (xIsExpansionMap != false)
            utf8::Format(fileMask, "*.GX%d", giNumHumanPlayers);
        else
            utf8::Format(fileMask, "*.GM%d", giNumHumanPlayers);
    }

    // The requester hardcodes error text for the retail campaigns; keep it
    // quiet for the custom ones.
    i32 savedDebugLevel_2 = giDebugLevel;
    i32 savedHumanCount_2 = iWSLastMsgNumHumanPlayers;
    if (xIsPlayingExpansionCampaign != 0 && IsCustomCampaign(xCampaign.m_campaignId)) {
        iWSLastMsgNumHumanPlayers = 999;
        giDebugLevel = 3;
    }
    fileReq = new fileRequester(
        FILE_REQUESTER_X,
        FILE_REQUESTER_Y,
        FILE_REQUESTER_LOAD_GAME,
        fileMask,
        gcGamePath,
        fileMask + 1
    );
    if (fileReq == NULL)
        MemError();
    dialogResult = gpExec->DoDialog(fileReq);
    iWSLastMsgNumHumanPlayers = savedHumanCount_2;
    giDebugLevel = savedDebugLevel_2;
    if (dialogResult == FILE_REQUESTER_OK) {
        gpGame->LoadGame(gLastFilename, false, false);
        delete fileReq;
        return true;
    } else {
        delete fileReq;
        return false;
    }
}

MessageDispatchResult SetupCampaignGameHandler(struct tag_message& message) {
    return BaseSetupHandler(message);
}

MessageDispatchResult SetupComPortHandler(struct tag_message& message) {
    i32 helpIndex;

    if (((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case CHOICE_THREE:
                helpIndex = 2;
                break;
            case CHOICE_FOUR:
                helpIndex = 3;
                break;
            case DIALOG_CANCEL:
                helpIndex = 4;
                break;
        }
        if (helpIndex >= FIRST_HELP) {
            if (gbDirectConnect != false)
                NormalDialog(gSetupDCComPortHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
            else
                NormalDialog(gSetupComPortHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
        }
    }
    return BaseSetupHandler(message);
}

MessageDispatchResult SetupBaudHandler(struct tag_message& message) {
    i32 helpIndex;

    if (((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case CHOICE_THREE:
                helpIndex = 2;
                break;
            case CHOICE_FOUR:
                helpIndex = 3;
                break;
            case DIALOG_CANCEL:
                helpIndex = 4;
                break;
        }
        if (helpIndex >= FIRST_HELP) {
            if (gbDirectConnect != false)
                NormalDialog(gSetupDCBaudHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
            else
                NormalDialog(gSetupBaudHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
        }
    }
    return BaseSetupHandler(message);
}

MessageDispatchResult SetupHotSeatGameHandler(struct tag_message& message) {
    i32 helpIndex;

    if (((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case CHOICE_THREE:
                helpIndex = 2;
                break;
            case CHOICE_FOUR:
                helpIndex = 3;
                break;
            case CHOICE_FIVE:
                helpIndex = 4;
                break;
            case DIALOG_CANCEL:
                helpIndex = 5;
                break;
        }
        if (helpIndex >= FIRST_HELP)
            NormalDialog(gSetupHotSeatGameHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
    }
    return BaseSetupHandler(message);
}

MessageDispatchResult SetupModemGameHandler(struct tag_message& message) {
    i32 helpIndex;

    if (((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case CHOICE_THREE:
                helpIndex = 2;
                break;
            case DIALOG_CANCEL:
                helpIndex = 3;
                break;
        }
        if (helpIndex >= FIRST_HELP) {
            if (gbDirectConnect != false)
                NormalDialog(gSetupDCGameHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
            else
                NormalDialog(gSetupModemGameHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
        }
    }
    return BaseSetupHandler(message);
}

MessageDispatchResult SetupMultiPlayerGameHandler(struct tag_message& message) {
    i32 helpIndex;

    if (((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case CHOICE_THREE:
                helpIndex = 2;
                break;
            case CHOICE_FOUR:
                helpIndex = 3;
                break;
            case DIALOG_CANCEL:
                helpIndex = 4;
                break;
        }
        if (helpIndex >= FIRST_HELP)
            NormalDialog(gSetupMultiPlayerGameHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
    }
    return BaseSetupHandler(message);
}

MessageDispatchResult SetupNetworkGameHandler(struct tag_message& message) {
    i32 helpIndex;

    if (((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case DIALOG_CANCEL:
                helpIndex = 2;
                break;
        }
        if (helpIndex >= FIRST_HELP)
            NormalDialog(gSetupNetworkGameHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
    }
    return BaseSetupHandler(message);
}

MessageDispatchResult SetupNetworkGame2Handler(struct tag_message& message) {
    i32 helpIndex;

    if (((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case CHOICE_THREE:
                helpIndex = 2;
                break;
            case DIALOG_CANCEL:
                helpIndex = 3;
                break;
        }
        if (helpIndex >= FIRST_HELP)
            NormalDialog(gSetupNetworkGame2Help[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
    }
    return BaseSetupHandler(message);
}

MessageDispatchResult SetupGameHandler(struct tag_message& message) {
    i32 helpIndex;

    if (((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0) {
        if (IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
            helpIndex = NO_HELP;
            switch (message.payload.widget.id) {
                case CHOICE_ONE:
                    helpIndex = 0;
                    break;
                case CHOICE_TWO:
                    helpIndex = 1;
                    break;
                case CHOICE_THREE:
                    helpIndex = 2;
                    break;
                case DIALOG_CANCEL:
                    helpIndex = 3;
                    break;
            }
            if (helpIndex >= FIRST_HELP)
                NormalDialog(gSetupGameHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
        }
    } else if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_NOTIFY_DESELECT:
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

MessageDispatchResult ExpNewCampaignHandler(struct tag_message& message) {
    i32 helpIndex;

    if (((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case DIALOG_CANCEL:
                helpIndex = 2;
                break;
        }
        if (helpIndex >= FIRST_HELP)
            NormalDialog(xSetupCampaignGameHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
    }
    return BaseSetupHandler(message);
}

MessageDispatchResult ExpLoadCampaignHandler(struct tag_message& message) {
    i32 helpIndex;

    if (((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case DIALOG_CANCEL:
                helpIndex = 2;
                break;
        }
        if (helpIndex >= FIRST_HELP)
            NormalDialog(xSetupCampaignGameHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
    }
    return BaseSetupHandler(message);
}

MessageDispatchResult ExpStdGameHandler(struct tag_message& message) {
    i32 helpIndex;

    if (((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case DIALOG_CANCEL:
                helpIndex = 2;
                break;
        }
        if (helpIndex >= FIRST_HELP)
            NormalDialog(xSetupStandardGameHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
    }
    return BaseSetupHandler(message);
}

MessageDispatchResult BaseSetupHandler(struct tag_message& message) {
    b32 handled = false;

    PollSound();
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_NOTIFY_DESELECT:
                if ((message.payload.widget.id > 0
                     && message.payload.widget.id <= DIALOG_RESULT_MAX)
                    || message.payload.widget.id == DIALOG_CANCEL)
                    handled = true;
        }
    }

    if (handled || giMenuCommand != -1) {
        FINISH_DIALOG_MESSAGE(message);
        if (giMenuCommand != -1)
            gpWindowManager->m_dialogResult = DIALOG_CANCEL;
        return MESSAGE_DISPATCH_FORWARD;
    }

    CheckShingleUpdate();
    return MESSAGE_DISPATCH_CONSUME;
}

b32 gbDoModemConfig = false;
