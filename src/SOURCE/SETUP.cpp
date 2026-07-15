// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SETUP.OBJ   from: (directly linked into exe)
// functions: 23   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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

VA(0x00410e90, 0x2f)
int game::SetupCampaignGame(void)
{
    PlaySmacker(SETUP_CAMPAIGN_INTRO);
    PlaySmacker(SETUP_CAMPAIGN_SELECTION);
    return 1;
}

// @early-stop All 0x141 code bytes match after relocation masking; external
// relocations match 22/22. Residual is delinked local/interior symbol identity.
VA(0x00410ebf, 0x141)
int game::SetupBaud(void)
{
    heroWindow *window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpbaud.bin");
    if (window == 0)
        MemError();
    gpWindowManager->DoDialog(window, SetupBaudHandler, 0);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
    case 1:
        gConfig.baudRate[gbDirectConnect] = SETUP_BAUD_2400;
        break;
    case 2:
        gConfig.baudRate[gbDirectConnect] = SETUP_BAUD_9600;
        break;
    case 3:
        gConfig.baudRate[gbDirectConnect] = SETUP_BAUD_19200;
        break;
    case 4:
        gConfig.baudRate[gbDirectConnect] = SETUP_BAUD_38400;
        break;
    case SETUP_DIALOG_CANCEL:
        return 0;
    }
    return 1;
}

// @early-stop All 0x200 code bytes match after relocation masking; external
// relocations match 47/47. Target-only entries are local jump-table relocs.
VA(0x00411000, 0x200)
int game::SetupComPort(void)
{
    char initString[SETUP_MODEM_INIT_ENTRY_LENGTH];

    LogStr("SCP 1");
    heroWindow *setupWindow = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpcom.bin");
    if (setupWindow == 0)
        MemError();
    LogStr("SCP 2");
    gpWindowManager->DoDialog(setupWindow, SetupComPortHandler, 0);
    delete setupWindow;
    LogStr("SCP 3");

    switch (gpWindowManager->m_dialogResult) {
    case 1:
        gConfig.comPort[gbDirectConnect] = SETUP_COM_PORT_1;
        break;
    case 2:
        gConfig.comPort[gbDirectConnect] = SETUP_COM_PORT_2;
        break;
    case 3:
        gConfig.comPort[gbDirectConnect] = SETUP_COM_PORT_3;
        break;
    case 4:
        gConfig.comPort[gbDirectConnect] = SETUP_COM_PORT_4;
        break;
    case SETUP_DIALOG_CANCEL:
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
            "Please enter any special initialization string required by your modem, or hit 'ENTER' to accept the default.",
            initString, SETUP_MODEM_INIT_ENTRY_LENGTH, gText, 0, 1);
        strcpy(gConfig.modemInitString, initString);
    }
    WritePrefs();
    return 1;
}

// @early-stop All 0x238 code bytes match after relocation masking; external
// relocations match 39/39. Target-only entries are local jump-table relocs.
VA(0x00411200, 0x238)
int game::SetupHotSeatGame(void)
{
    char defaultName[24];
    int ix;

    heroWindow *window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stphotst.bin");
    if (window == 0)
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
    case SETUP_DIALOG_CANCEL:
        return 0;
    }

    for (ix = 0; ix < SETUP_PLAYER_COUNT; ix++)
        strcpy(cPlayerNames[ix], "");

    if (giSetupGameType == 0) {
        sprintf(gText, "Do you wish to enter each player's name?");
        NormalDialog(gText, 2, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gpWindowManager->m_dialogResult == SETUP_DIALOG_YES) {
            for (ix = 0; ix < giNumHumanPlayers; ix++) {
                strcpy(defaultName, "");
                sprintf(gText, "Enter player %d's name.", ix + 1);
                GetDataEntry(
                    gText, cPlayerNames[ix], SETUP_PLAYER_NAME_LENGTH,
                    defaultName, 0, 1);
            }
        }
    }
    return 1;
}

VA(0x00411438, 0x127)
int game::SetupNetworkGame(void)
{
    tag_message message;
    heroWindow *window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpnet.bin");
    if (window == 0)
        MemError();

    if (gbNoCDRom != 0) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.id = SETUP_DISABLED_WIDGET_ID;
        message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
        window->BroadcastMessage(message);
    }

    gpWindowManager->DoDialog(window, SetupNetworkGameHandler, 0);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
    case 1:
        iMPExtendedType = SETUP_REMOTE_NETWORK_HOST;
        break;
    case 2:
        iMPExtendedType = SETUP_REMOTE_NETWORK_GUEST;
        break;
    case SETUP_DIALOG_CANCEL:
        return 0;
    }
    return 1;
}

// @early-stop All 0x236 code bytes match after relocation masking; external
// relocations match 24/24. Residual is fixed IAT/local jump-table identity.
VA(0x0041155f, 0x236)
int game::SetupNetworkGame2(void)
{
    tag_message message;
    OSVERSIONINFO osInfo;
    int result;

    heroWindow *window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpnet2.bin");
    if (window == 0)
        MemError();

    memset(&osInfo, 0, sizeof(osInfo));
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    result = GetVersionEx(&osInfo);
    LogInt("Version", result, osInfo.dwPlatformId,
           SETUP_LOG_UNUSED, SETUP_LOG_UNUSED, SETUP_LOG_UNUSED,
           SETUP_LOG_UNUSED, SETUP_LOG_UNUSED);
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
        iMPNetProtocol = 1;
        break;
    case 2:
        iMPNetProtocol = 2;
        break;
    case 3:
        iMPNetProtocol = 3;
        break;
    case 4:
        iMPNetProtocol = 4;
        break;
    case SETUP_DIALOG_CANCEL:
        return 0;
    }
    if (!SetupNetworkGame())
        return 0;
    else
        return 1;
}

VA(0x00411795, 0x317)
int game::SetupModemGame(void)
{
    tag_message message;
    heroWindow *window;

    LogStr("SMC 1");
    if (gbDirectConnect != 0) {
        if (gConfig.comPort[gbDirectConnect] == SETUP_COM_PORT_UNCONFIGURED)
            window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpdc.bin");
        else
            window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpdccfg.bin");
    } else {
        if (gConfig.comPort[gbDirectConnect] == SETUP_COM_PORT_UNCONFIGURED)
            window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpmodem.bin");
        else
            window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpmcfg.bin");
    }
    if (window == 0)
        MemError();

    LogStr("SMC 2");
    if (gbNoCDRom != 0) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.id = SETUP_DISABLED_WIDGET_ID;
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
        iMPExtendedType = SETUP_REMOTE_MODEM_DIAL;
        if (gConfig.comPort[gbDirectConnect] == SETUP_COM_PORT_UNCONFIGURED) {
            LogStr("SMC 7");
            if (!SetupComPort())
                return 0;
            LogStr("SMC 8");
        }
        LogStr("SMC 9");
        if (gbDirectConnect == 0) {
            GetDataEntry(
                "Please enter the telephone number.", numbuf,
                SETUP_TELEPHONE_ENTRY_LENGTH, 0, 0, 1);
        }
        LogStr("SMC a");
        break;
    case 2:
        iMPExtendedType = SETUP_REMOTE_MODEM_ANSWER;
        if (gConfig.comPort[gbDirectConnect] == SETUP_COM_PORT_UNCONFIGURED &&
            !SetupComPort())
            return 0;
        break;
    case 3:
        gbDoModemConfig = 1;
        break;
    case SETUP_DIALOG_CANCEL:
        return 0;
    }
    return 1;
}

// @early-stop All 0x257 code bytes match after relocation masking; external
// relocations match 42/42. Residual is delinked local jump-table identity.
VA(0x00411aac, 0x257)
int game::SetupMultiPlayerGame(void)
{
    tag_message message;
    int continueFlag;

    heroWindow *window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpmp.bin");
    if (window == 0)
        MemError();

    if (gbNoCDRom != 0) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.id = SETUP_DISABLED_WIDGET_ID;
        message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
        window->BroadcastMessage(message);
    }
    gpWindowManager->DoDialog(window, SetupMultiPlayerGameHandler, 0);
    delete window;

    gbDirectConnect = 0;
    switch (gpWindowManager->m_dialogResult) {
    case 1:
        iMPBaseType = SETUP_MULTIPLAYER_HOT_SEAT;
        if (!SetupHotSeatGame())
            return 0;
        break;
    case 2:
        iMPBaseType = SETUP_MULTIPLAYER_NETWORK;
        if (!SetupNetworkGame2())
            return 0;
        break;
    case 4:
        gbDirectConnect = 1;
        goto setupModem;
    case 3:
        gbDirectConnect = 0;
    setupModem:
        iMPBaseType = SETUP_MULTIPLAYER_MODEM;
        continueFlag = 1;
        LogStr("Common Modem 1");
        while (continueFlag) {
            LogStr("Common Modem 2");
            if (!SetupModemGame())
                return 0;
            LogStr("Common Modem 3");
            if (gbDoModemConfig != 0) {
                LogStr("Common Modem 4");
                gbDoModemConfig = 0;
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
    case SETUP_DIALOG_CANCEL:
        return 0;
    }
    return 1;
}

// @match-note 92.78%; complete 0x5c frame, menu/dialog CFG, and semantics.
// External relocations match 122/122. Revisit at 95% for switch-trampoline tuning.
VA(0x00411d03, 0x6c9)
int game::SetupGame(void)
{
    heroWindow *window;
    int result;

    LogStr("Setup 0");
    result = 1;
    xIsPlayingExpansionCampaign = 0;
    xIsExpansionMap = 0;
    gbInCampaign = 0;
    gbCampaignSideChoice = 0;
    iMPExtendedType = 10;
    iMPBaseType = 10;
    giNumHumanPlayers = 1;
    gbWaitForRemoteReceive = 0;
    gbDirectConnect = 0;
    gbInSetupDialog = 1;

    if (giMenuCommand != -1) {
        switch (giMenuCommand) {
        case APP_MENU_RESTART_0:
        case APP_MENU_LOAD_0:
            break;

        case APP_MENU_RESTART_5:
        case APP_MENU_LOAD_2:
            giNumHumanPlayers = 2;
            iMPBaseType = SETUP_MULTIPLAYER_HOT_SEAT;
            break;
        case APP_MENU_RESTART_6:
        case APP_MENU_LOAD_3:
            giNumHumanPlayers = 3;
            iMPBaseType = SETUP_MULTIPLAYER_HOT_SEAT;
            break;
        case APP_MENU_RESTART_7:
        case APP_MENU_LOAD_4:
            giNumHumanPlayers = 4;
            iMPBaseType = SETUP_MULTIPLAYER_HOT_SEAT;
            break;

        case APP_MENU_RESTART_8:
        case APP_MENU_LOAD_5:
            iMPBaseType = SETUP_MULTIPLAYER_NETWORK;
            iMPExtendedType = SETUP_REMOTE_NETWORK_HOST;
            goto remoteSetup;
        case APP_MENU_RESTART_9:
        case APP_MENU_LOAD_6:
            iMPBaseType = SETUP_MULTIPLAYER_NETWORK;
            iMPExtendedType = SETUP_REMOTE_NETWORK_GUEST;
            goto remoteSetup;
        case APP_MENU_RESTART_10:
        case APP_MENU_LOAD_7:
            iMPBaseType = SETUP_MULTIPLAYER_MODEM;
            iMPExtendedType = SETUP_REMOTE_MODEM_DIAL;
            goto remoteSetup;
        case APP_MENU_RESTART_11:
        case APP_MENU_LOAD_8:
            iMPBaseType = SETUP_MULTIPLAYER_MODEM;
            iMPExtendedType = SETUP_REMOTE_MODEM_ANSWER;
            goto remoteSetup;
        case APP_MENU_RESTART_12:
        case APP_MENU_LOAD_9:
            iMPBaseType = SETUP_MULTIPLAYER_MODEM;
            iMPExtendedType = SETUP_REMOTE_MODEM_DIAL;
            gbDirectConnect = 1;
            goto remoteSetup;
        case APP_MENU_RESTART_13:
        case APP_MENU_LOAD_10:
            iMPBaseType = SETUP_MULTIPLAYER_MODEM;
            iMPExtendedType = SETUP_REMOTE_MODEM_ANSWER;
            gbDirectConnect = 1;
            goto remoteSetup;
        }
        goto menuDone;

    remoteSetup:
        LogStr("Setup 0a");
        RemoteMain(iMPExtendedType);
        if (iMPExtendedType == SETUP_REMOTE_NETWORK_GUEST ||
            iMPExtendedType == SETUP_REMOTE_MODEM_ANSWER)
            gbWaitForRemoteReceive = 1;

    menuDone:
        giMenuCommand = -1;
        result = 1;
        goto done;
    }

    window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpnewgm.bin");
    if (window == 0)
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

    switch (gpWindowManager->m_dialogResult) {
    case 1:
        break;

    case 2:
        if (giSetupGameType == OLD_MAIN_SETUP_LOAD) {
            window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "x_loadcm.bin");
            if (window == 0)
                MemError();
            gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
            delete window;

            switch (gpWindowManager->m_dialogResult) {
            case 1:
                gbInCampaign = 1;
                break;
            case 2:
                xIsPlayingExpansionCampaign = 1;
                xIsExpansionMap = 1;
                break;
            case SETUP_DIALOG_CANCEL:
                result = 0;
                goto done;
            }
        } else {
            window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "x_loadcm.bin");
            if (window == 0)
                MemError();
            gpWindowManager->DoDialog(window, ExpLoadCampaignHandler, 0);
            delete window;

            switch (gpWindowManager->m_dialogResult) {
            case 1:
                gbInCampaign = 1;
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
            case SETUP_DIALOG_CANCEL:
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

    case SETUP_DIALOG_CANCEL:
        result = 0;
        goto done;
    }

    LogStr(" Setup 1");
    if (iMPBaseType == SETUP_MULTIPLAYER_NETWORK ||
        iMPBaseType == SETUP_MULTIPLAYER_MODEM) {
        LogStr(" Setup 2");
        RemoteMain(iMPExtendedType);
        LogStr(" Setup 3");
        if (iMPExtendedType == SETUP_REMOTE_NETWORK_GUEST ||
            iMPExtendedType == SETUP_REMOTE_MODEM_ANSWER)
            gbWaitForRemoteReceive = 1;
    }

done:
    gbInSetupDialog = 0;
    return result;
}

// @match-note Complete requester/dialog CFG and semantics; external relocation
// identities and occurrences match 43/43. Revisit at 95% for local block order.
VA(0x004123cc, 0x2aa)
int game::PickLoadGame(void)
{
    char filePattern[12];
    int dialogResult;
    heroWindow *window;
    fileRequester *requester;

    if (gbWaitForRemoteReceive != 0)
        return 1;

    if (gbInCampaign != 0) {
        sprintf(filePattern, "*.GMC");
    } else if (xIsPlayingExpansionCampaign != 0) {
        sprintf(filePattern, "*.GXC");
    } else if (gbRemoteOn != 0 && xNetHasOldPlayers != 0) {
        NormalDialog(
            "At least one player does not have the Heroes II Expansion set.  You will only be able to choose from original Heroes II games.",
            1, -1, -1, -1, 0, -1, 0, -1, 0);
        sprintf(filePattern, "*.GM%d", giNumHumanPlayers);
    } else {
        window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "x_mapmnu.bin");
        if (window == 0)
            MemError();
        gpWindowManager->DoDialog(window, ExpStdGameHandler, 0);
        delete window;

        switch (gpWindowManager->m_dialogResult) {
        case 1:
            xIsExpansionMap = 0;
            break;
        case 2:
            xIsExpansionMap = 1;
            break;
        case SETUP_DIALOG_CANCEL:
            return 0;
        }

        if (xIsExpansionMap != 0)
            sprintf(filePattern, "*.GX%d", giNumHumanPlayers);
        else
            sprintf(filePattern, "*.GM%d", giNumHumanPlayers);
    }

    requester = new fileRequester(
        200, 58, FILE_REQUESTER_LOAD_GAME,
        filePattern, gcGamePath, filePattern);
    if (requester == 0)
        MemError();
    dialogResult = gpExec->DoDialog(requester);
    if (dialogResult == FILE_REQUESTER_OK) {
        gpGame->LoadGame(gLastFilename, 0, 0);
        delete requester;
        return 1;
    }

    delete requester;
    return 0;
}

VA(0x00412676, 0x1e)
int SetupCampaignGameHandler(struct tag_message &message)
{
    return BaseSetupHandler(message);
}

// @match-note Complete frame/CFG and help-table semantics; external relocs 11/11.
// Revisit at 95% for delinked local jump-table identity only.
VA(0x00412694, 0x13e)
int SetupComPortHandler(struct tag_message &message)
{
    int helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0 &&
        (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
         message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = -1;
        switch (message.payload.widget.id) {
        case 1: helpIndex = 0; break;
        case 2: helpIndex = 1; break;
        case 3: helpIndex = 2; break;
        case 4: helpIndex = 3; break;
        case SETUP_DIALOG_CANCEL: helpIndex = 4; break;
        }
        if (helpIndex >= 0) {
            if (gbDirectConnect == 0)
                NormalDialog(gSetupComPortHelp[helpIndex], SETUP_HELP_DIALOG,
                             -1, -1, -1, 0, -1, 0, -1, 0);
            else
                NormalDialog(gSetupDCComPortHelp[helpIndex], SETUP_HELP_DIALOG,
                             -1, -1, -1, 0, -1, 0, -1, 0);
        }
    }
    return BaseSetupHandler(message);
}

// @match-note Complete frame/CFG and help-table semantics; external relocs 11/11.
// Revisit at 95% for delinked local jump-table identity only.
VA(0x004127d2, 0x13e)
int SetupBaudHandler(struct tag_message &message)
{
    int helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0 &&
        (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
         message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = -1;
        switch (message.payload.widget.id) {
        case 1: helpIndex = 0; break;
        case 2: helpIndex = 1; break;
        case 3: helpIndex = 2; break;
        case 4: helpIndex = 3; break;
        case SETUP_DIALOG_CANCEL: helpIndex = 4; break;
        }
        if (helpIndex >= 0) {
            if (gbDirectConnect == 0)
                NormalDialog(gSetupBaudHelp[helpIndex], SETUP_HELP_DIALOG,
                             -1, -1, -1, 0, -1, 0, -1, 0);
            else
                NormalDialog(gSetupDCBaudHelp[helpIndex], SETUP_HELP_DIALOG,
                             -1, -1, -1, 0, -1, 0, -1, 0);
        }
    }
    return BaseSetupHandler(message);
}

// @match-note Complete frame/CFG and help-table semantics; external relocs 9/9.
// Revisit at 95% for delinked local jump-table identity only.
VA(0x00412910, 0x118)
int SetupHotSeatGameHandler(struct tag_message &message)
{
    int helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0 &&
        (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
         message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = -1;
        switch (message.payload.widget.id) {
        case 1: helpIndex = 0; break;
        case 2: helpIndex = 1; break;
        case 3: helpIndex = 2; break;
        case 4: helpIndex = 3; break;
        case 5: helpIndex = 4; break;
        case SETUP_DIALOG_CANCEL: helpIndex = 5; break;
        }
        if (helpIndex >= 0)
            NormalDialog(gSetupHotSeatGameHelp[helpIndex], SETUP_HELP_DIALOG,
                         -1, -1, -1, 0, -1, 0, -1, 0);
    }
    return BaseSetupHandler(message);
}

// @match-note 99.23%; complete frame/CFG and modem/DC help semantics; relocs 6/6.
// Revisit at 95% for packed switch/conditional register allocation.
VA(0x00412a28, 0x12e)
int SetupModemGameHandler(struct tag_message &message)
{
    int helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0 &&
        (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
         message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = -1;
        switch (message.payload.widget.id) {
        case 1: helpIndex = 0; break;
        case 2: helpIndex = 1; break;
        case 3: helpIndex = 2; break;
        case SETUP_DIALOG_CANCEL: helpIndex = 3; break;
        }
        if (helpIndex >= 0) {
            if (gbDirectConnect == 0)
                NormalDialog(gSetupModemGameHelp[helpIndex], SETUP_HELP_DIALOG,
                             -1, -1, -1, 0, -1, 0, -1, 0);
            else
                NormalDialog(gSetupDCGameHelp[helpIndex], SETUP_HELP_DIALOG,
                             -1, -1, -1, 0, -1, 0, -1, 0);
        }
    }
    return BaseSetupHandler(message);
}

// @match-note Complete frame/CFG and help-table semantics; external relocs 8/8.
// Revisit at 95% for delinked local jump-table identity only.
VA(0x00412b56, 0x108)
int SetupMultiPlayerGameHandler(struct tag_message &message)
{
    int helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0 &&
        (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
         message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = -1;
        switch (message.payload.widget.id) {
        case 1: helpIndex = 0; break;
        case 2: helpIndex = 1; break;
        case 3: helpIndex = 2; break;
        case 4: helpIndex = 3; break;
        case SETUP_DIALOG_CANCEL: helpIndex = 4; break;
        }
        if (helpIndex >= 0)
            NormalDialog(gSetupMultiPlayerGameHelp[helpIndex], SETUP_HELP_DIALOG,
                         -1, -1, -1, 0, -1, 0, -1, 0);
    }
    return BaseSetupHandler(message);
}

VA(0x00412c5e, 0xd7)
int SetupNetworkGameHandler(struct tag_message &message)
{
    int helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0 &&
        (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
         message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = -1;
        switch (message.payload.widget.id) {
        case 1: helpIndex = 0; break;
        case 2: helpIndex = 1; break;
        case SETUP_DIALOG_CANCEL: helpIndex = 2; break;
        }
        if (helpIndex >= 0)
            NormalDialog(gSetupNetworkGameHelp[helpIndex], SETUP_HELP_DIALOG,
                         -1, -1, -1, 0, -1, 0, -1, 0);
    }
    return BaseSetupHandler(message);
}

VA(0x00412d35, 0xf8)
int SetupNetworkGame2Handler(struct tag_message &message)
{
    int helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0 &&
        (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
         message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = -1;
        switch (message.payload.widget.id) {
        case 1: helpIndex = 0; break;
        case 2: helpIndex = 1; break;
        case 3: helpIndex = 2; break;
        case SETUP_DIALOG_CANCEL: helpIndex = 3; break;
        }
        if (helpIndex >= 0)
            NormalDialog(gSetupNetworkGame2Help[helpIndex], SETUP_HELP_DIALOG,
                         -1, -1, -1, 0, -1, 0, -1, 0);
    }
    return BaseSetupHandler(message);
}

// @match-note 95.98%; complete help and empty deselect-switch CFG; relocs 3/3.
// Revisit at 95% for no-effect switch label placement.
VA(0x00412e2d, 0x14b)
int SetupGameHandler(struct tag_message &message)
{
    int helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0 &&
        (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
         message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = -1;
        switch (message.payload.widget.id) {
        case 1: helpIndex = 0; break;
        case 2: helpIndex = 1; break;
        case 3: helpIndex = 2; break;
        case SETUP_DIALOG_CANCEL: helpIndex = 3; break;
        }
        if (helpIndex >= 0)
            NormalDialog(gSetupGameHelp[helpIndex], SETUP_HELP_DIALOG,
                         -1, -1, -1, 0, -1, 0, -1, 0);
    } else if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
        case WIDGET_COMMAND_DESELECT:
            switch (message.payload.widget.id) {
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            }
            break;
        }
    }
    return BaseSetupHandler(message);
}

VA(0x00412f78, 0xd7)
int ExpNewCampaignHandler(struct tag_message &message)
{
    int helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0 &&
        (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
         message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = -1;
        switch (message.payload.widget.id) {
        case 1: helpIndex = 0; break;
        case 2: helpIndex = 1; break;
        case SETUP_DIALOG_CANCEL: helpIndex = 2; break;
        }
        if (helpIndex >= 0)
            NormalDialog(xSetupCampaignGameHelp[helpIndex], SETUP_HELP_DIALOG,
                         -1, -1, -1, 0, -1, 0, -1, 0);
    }
    return BaseSetupHandler(message);
}

VA(0x0041304f, 0xd7)
int ExpLoadCampaignHandler(struct tag_message &message)
{
    int helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0 &&
        (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
         message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = -1;
        switch (message.payload.widget.id) {
        case 1: helpIndex = 0; break;
        case 2: helpIndex = 1; break;
        case SETUP_DIALOG_CANCEL: helpIndex = 2; break;
        }
        if (helpIndex >= 0)
            NormalDialog(xSetupCampaignGameHelp[helpIndex], SETUP_HELP_DIALOG,
                         -1, -1, -1, 0, -1, 0, -1, 0);
    }
    return BaseSetupHandler(message);
}

VA(0x00413126, 0xd7)
int ExpStdGameHandler(struct tag_message &message)
{
    int helpIndex;

    if ((message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0 &&
        (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
         message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {
        helpIndex = -1;
        switch (message.payload.widget.id) {
        case 1: helpIndex = 0; break;
        case 2: helpIndex = 1; break;
        case SETUP_DIALOG_CANCEL: helpIndex = 2; break;
        }
        if (helpIndex >= 0)
            NormalDialog(xSetupStandardGameHelp[helpIndex], SETUP_HELP_DIALOG,
                         -1, -1, -1, 0, -1, 0, -1, 0);
    }
    return BaseSetupHandler(message);
}

// @match-note 97.22%; complete close/continue CFG and dialog semantics; relocs 6/6.
// Revisit at 95% for handled-local compare polarity.
VA(0x004131fd, 0xf0)
int BaseSetupHandler(struct tag_message &message)
{
    int handled = 0;

    PollSound();
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
        case WIDGET_COMMAND_DESELECT:
            if ((message.payload.widget.id > 0 &&
                 message.payload.widget.id < 1001) ||
                message.payload.widget.id == SETUP_DIALOG_CANCEL)
                handled = 1;
            break;
        }
    }

    if (handled || giMenuCommand != -1) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = WIDGET_COMMAND_DIALOG_SELECT;
        message.payload.widget.command = message.payload.widget.id;
        if (giMenuCommand != -1)
            gpWindowManager->m_dialogResult = SETUP_DIALOG_CANCEL;
        return SETUP_HANDLER_CLOSE;
    }

    CheckShingleUpdate();
    return SETUP_HANDLER_CONTINUE;
}

// ---- globals (definitions, RVA order) ----
DATA(0x004ee248) int gbDoModemConfig = 0;
