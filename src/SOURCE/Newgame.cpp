// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Newgame.obj   from: (directly linked into exe)
// functions: 16   data: 8
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <string.h>
#include <_carcass_types.h>
#include <BASE/Misc.h>
#include <BASE/executive.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/resourceManager.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/KB.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/REQUEST.h>
#include <SOURCE/SETUP.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/fileRequester.h>
#include <SOURCE/game.h>
#include <SOURCE/Newgame.h>

DATA(0x0051cdd0) static short newGameSourceLineBase = NEW_GAME_SOURCE_LINE_BASE;

VA(0x004b6f40, 0x1d5)
void game::GetMap(void)
{
    char savedMapName[44];
    char filePattern[16];
    fileRequester *requester;
    int dialogResult;

    strcpy(savedMapName, gMapName);
    strcpy(gcCurMapName, "");
    if (gbRemoteOn && xNetHasOldPlayers) {
        NormalDialog(
            "At least one player does not have the Heroes II Expansion set.  You will only be able to choose from original Heroes II games.",
            1, -1, -1, -1, 0, -1, 0, -1, 0);
        sprintf(filePattern, "*.%s", "MP2");
    } else if (xIsExpansionMap) {
        sprintf(filePattern, "*.%s", "MX2");
    } else {
        sprintf(filePattern, "*.%s", "MP2");
    }

    requester = new fileRequester(212, 9, FILE_REQUESTER_MAP_GAME,
                                  filePattern, gcMapPath, filePattern);
    if (requester == 0)
        MemError();
    dialogResult = gpExec->DoDialog(requester);
    if (dialogResult == FILE_REQUESTER_OK) {
        delete requester;
        strcpy(gMapName, gLastFilename);
        if (_strcmpi(savedMapName, gMapName) != 0) {
            strcpy(m_mapFilename, gMapName);
            ProcessNewMap(0);
        }
    } else {
        delete requester;
        strcpy(gMapName, savedMapName);
    }
}

VA(0x004b7115, 0x77)
void game::ProcessNewMap(struct SMapHeader *header)
{
    m_newGameInitialized = 0;
    m_newGameHumanCount = static_cast<signed char>(giNumHumanPlayers);
    if (m_newGameWindow == 0) {
    } else {
        CleanUpNewGameWindow();
        InitNewGame(header);
        InitNewGameWindow();
        UpdateNewGameWindow();
        m_newGameWindow->DrawWindow();
    }
}

VA(0x004b718c, 0x491)
void game::InitNewGame(struct SMapHeader *header)
{
    int activePlayerCount;
    int unusedPlayerCount;
    int player;
    int computerPlayers;
    int humanPlayers;
    int flexiblePlayerType;

    activePlayerCount = 0;
    unusedPlayerCount = 0;
    humanPlayers = 0;
    computerPlayers = 0;

    if (m_newGameInitialized &&
        m_newGameHumanCount == giNumHumanPlayers) {
        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_setupPlayerNetworkId[player] == NEW_GAME_COMPUTER_PLAYER ||
                m_setupPlayerNetworkId[player] >= giNumHumanPlayers) {
                m_setupPlayerRace[player] =
                    m_mapHeader.playerRace[m_setupPlayerColor[player]];
            }
        }
    } else {
        m_newGameHumanCount = static_cast<signed char>(giNumHumanPlayers);
        if (header != 0)
            m_mapHeader = *header;
        else
            GetMapHeader(m_mapFilename, &m_mapHeader);

        for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
            if (m_mapHeader.playerEnabled[player]) {
                m_setupPlayerColor[activePlayerCount] =
                    static_cast<signed char>(player);
                ++activePlayerCount;
            }
        }

        for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
            if (player >= m_mapHeader.playerCount) {
                m_setupPlayerType[player] = NEW_GAME_NETWORK_PLAYER_NONE;
                m_setupPlayerNetworkId[player] = m_setupPlayerType[player];
                m_setupPlayerRace[player] = m_setupPlayerNetworkId[player];
                m_playerHandicap[player] = m_setupPlayerRace[player];
            } else {
                m_playerHandicap[player] = 0;
                m_setupPlayerRace[player] =
                    m_mapHeader.playerRace[m_setupPlayerColor[player]];
                m_setupPlayerNetworkId[player] = NEW_GAME_NETWORK_PLAYER_NONE;
                m_setupPlayerType[player] = NEW_GAME_NETWORK_PLAYER_NONE;
            }
        }

        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_mapHeader.playerCanHuman[m_setupPlayerColor[player]] &&
                !m_mapHeader.playerCanComputer[m_setupPlayerColor[player]]) {
                m_setupPlayerType[player] = NEW_GAME_PLAYER_DEFAULT;
                m_setupPlayerNetworkId[player] =
                    static_cast<signed char>(humanPlayers);
                ++humanPlayers;
            } else if (!m_mapHeader.playerCanHuman[m_setupPlayerColor[player]] &&
                       m_mapHeader.playerCanComputer[m_setupPlayerColor[player]]) {
                m_setupPlayerNetworkId[player] = NEW_GAME_COMPUTER_PLAYER;
                m_setupPlayerType[player] = NEW_GAME_PLAYER_DEFAULT;
                ++computerPlayers;
            }
        }

        if (giNumHumanPlayers > humanPlayers &&
            computerPlayers < m_mapHeader.playerCount - giNumHumanPlayers)
            flexiblePlayerType = NEW_GAME_PLAYER_FLEXIBLE;
        else
            flexiblePlayerType = NEW_GAME_PLAYER_DEFAULT;

        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_setupPlayerType[player] == NEW_GAME_NETWORK_PLAYER_NONE)
                m_setupPlayerType[player] =
                    static_cast<signed char>(flexiblePlayerType);
        }

        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_setupPlayerNetworkId[player] != NEW_GAME_NETWORK_PLAYER_NONE)
                continue;
            if (giNumHumanPlayers > humanPlayers &&
                m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]) {
                m_setupPlayerNetworkId[player] =
                    static_cast<signed char>(humanPlayers);
                ++humanPlayers;
            } else {
                m_setupPlayerNetworkId[player] = NEW_GAME_COMPUTER_PLAYER;
            }
        }
        m_difficulty = NEW_GAME_DEFAULT_DIFFICULTY;
        m_newGameInitialized = 1;
    }
    m_selectedSetupPlayer = NEW_GAME_NETWORK_PLAYER_NONE;
}

VA(0x004b761d, 0x81)
void game::SetupNetPlayerNames(void)
{
    int player;

    if (giNumHumanPlayers > 1) {
        for (player = 0; player < giNumHumanPlayers; ++player) {
            if (iMPBaseType != NEW_GAME_MULTIPLAYER_HOTSEAT)
                strcpy(cPlayerNames[player], gsNetPlayerInfo[player].name);
        }
    }
}

VA(0x004b769e, 0xaca)
int game::NewGame(void)
{
    char netPlayerPacket[NEW_GAME_PLAYER_INFO_BUFFER_SIZE];
    char mapInfoPacket[NEW_GAME_MAP_PACKET_SIZE];
    tag_message windowMessage;
    int mapHeaderLoaded;
    int playerDataReceived;
    NewGameRemotePacket *remoteBuffer;
    heroWindow *choiceWindow;
    char *extension;
    int textBufferIndex;
    int mapHeaderResult;
    int result;
    signed char wrongExpansionType;
    int transmitResult;

    result = 1;
    m_newGameWindow = 0;

    if ((!gbRemoteOn || giThisNetPos == 0) &&
        (!gbRemoteOn || !xNetHasOldPlayers)) {
        choiceWindow = new heroWindow(405, 8, "x_mapmnu.bin");
        if (choiceWindow == 0)
            MemError();
        gpWindowManager->DoDialog(choiceWindow, ExpStdGameHandler, 0);
        delete choiceWindow;
        switch (static_cast<short>(gpWindowManager->m_dialogResult)) {
        case 1:
            xIsExpansionMap = 0;
            break;
        case 2:
            xIsExpansionMap = 1;
            break;
        case NEW_GAME_DIALOG_CANCEL:
            return 0;
        }
    }

    SetupNetPlayerNames();
    glTimers[0] = 0;
    for (textBufferIndex = 0; textBufferIndex < NEW_GAME_TEXT_BUFFER_COUNT;
         ++textBufferIndex) {
        cTextReceivedBuffer[textBufferIndex] = static_cast<char *>(BaseAlloc(
            NEW_GAME_TEXT_BUFFER_SIZE,
            "I:\\Projects\\Heroes\\Prog\\SOURCE\\Newgame.cpp",
            newGameSourceLineBase + 0x2d));
        strcpy(cTextReceivedBuffer[textBufferIndex], "");
    }
    cNGKPCore = static_cast<char *>(BaseAlloc(
        NEW_GAME_KEY_BUFFER_SIZE,
        "I:\\Projects\\Heroes\\Prog\\SOURCE\\Newgame.cpp",
        newGameSourceLineBase + 0x30));
    cNGKPDisplay = static_cast<char *>(BaseAlloc(
        NEW_GAME_KEY_BUFFER_SIZE,
        "I:\\Projects\\Heroes\\Prog\\SOURCE\\Newgame.cpp",
        newGameSourceLineBase + 0x31));
    strcpy(cNGKPCore, "");
    strcpy(cNGKPDisplay, "");
    NGKPcursorIndex = 0;
    NGKPBkg = gpResourceManager->GetIcon("ngextra.icn");

    if (gbWaitForRemoteReceive) {
        mapHeaderLoaded = 0;
        playerDataReceived = 0;
        do {
            PollSound();
            remoteBuffer = reinterpret_cast<NewGameRemotePacket *>(
                GetRemoteData(1));
            if (remoteBuffer != 0 &&
                remoteBuffer->type == NEW_GAME_REMOTE_PACKET_TYPE) {
                switch (remoteBuffer->command) {
                case NEW_GAME_REMOTE_MAP_HEADER:
                    memset(&m_mapHeader, 0, sizeof(m_mapHeader));
                    memcpy(&m_mapHeader, remoteBuffer->payload,
                           NEW_GAME_MAP_PACKET_SIZE);
                    mapHeaderLoaded = 1;
                    break;
                case NEW_GAME_REMOTE_PLAYER_INFO:
                    memcpy(gsNetPlayerInfo, remoteBuffer->payload,
                           NEW_GAME_PLAYER_INFO_PACKET_SIZE);
                    SetupNetPlayerNames();
                    playerDataReceived = 1;
                    break;
                }
            }
        } while (!playerDataReceived || !mapHeaderLoaded);

        m_newGameWindow = new heroWindow(190, 4, "ngmp.bin");
        InitNewGame(&m_mapHeader);
        InitNewGameWindow();
        UpdateNewGameWindow();

        windowMessage.type = NEW_GAME_MESSAGE_WIDGET;
        windowMessage.field8 = NEW_GAME_MAP_OPTIONS_CONTROL;
        windowMessage.field4 = NEW_GAME_WIDGET_ENABLE;
        windowMessage.field18 = NEW_GAME_WIDGET_ACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(windowMessage);
        windowMessage.field4 = NEW_GAME_WIDGET_DISABLE;
        windowMessage.field18 = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(windowMessage);
        windowMessage.field8 = NEW_GAME_DIALOG_OK;
        windowMessage.field4 = NEW_GAME_WIDGET_ENABLE;
        windowMessage.field18 = NEW_GAME_WIDGET_ACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(windowMessage);
        windowMessage.field4 = NEW_GAME_WIDGET_DISABLE;
        windowMessage.field18 = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(windowMessage);
        windowMessage.field8 = NEW_GAME_DIALOG_CANCEL;
        windowMessage.field4 = NEW_GAME_WIDGET_ENABLE;
        windowMessage.field18 = NEW_GAME_WIDGET_ACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(windowMessage);
        windowMessage.field4 = NEW_GAME_WIDGET_DISABLE;
        windowMessage.field18 = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(windowMessage);

        gbNewGameDialogOver = 0;
        gpWindowManager->DoDialog(m_newGameWindow, NewGameHandler, 0);
        delete m_newGameWindow;
        if (gpWindowManager->m_dialogResult == NEW_GAME_DIALOG_CANCEL)
            result = 0;
        else
            result = 1;
    } else {
        for (;;) {
            wrongExpansionType = 0;
            extension = FindLastToken(m_mapFilename, '.');
            if (extension != 0) {
                if (StrEqNoCase(extension, ".MX2") && xIsExpansionMap)
                    wrongExpansionType = 1;
                if (StrEqNoCase(extension, ".MP2") && !xIsExpansionMap)
                    wrongExpansionType = 1;
            }
            if (!wrongExpansionType) {
                if (xIsExpansionMap)
                    strcpy(gpGame->m_mapFilename, "arrax.mx2");
                else
                    strcpy(gpGame->m_mapFilename, "brokena.mp2");
                m_newGameInitialized = 0;
                m_newGameHumanCount =
                    static_cast<signed char>(giNumHumanPlayers);
            }
            if (giNumHumanPlayers > 3 &&
                _strcmpi(gpGame->m_mapFilename, "brokena.mp2") == 0)
                strcpy(gpGame->m_mapFilename, "slugfest.mp2");
            if (giNumHumanPlayers > 1 &&
                _strcmpi(gpGame->m_mapFilename, "arrax.mx2") == 0)
                strcpy(gpGame->m_mapFilename, "fullhse.mx2");

            strcpy(gMapName, m_mapFilename);
            mapHeaderResult = GetMapHeader(m_mapFilename, &m_mapHeader);
            if (!mapHeaderResult ||
                m_mapHeader.minHumanPlayers > giNumHumanPlayers ||
                m_mapHeader.maxHumanPlayers < giNumHumanPlayers)
                gpGame->GetMap();
            else
                break;
        }

        if (gbRemoteOn) {
            memcpy(mapInfoPacket, &gpGame->m_mapHeader,
                   NEW_GAME_MAP_PACKET_SIZE);
            transmitResult = TransmitRemoteData(
                mapInfoPacket, NEW_GAME_REMOTE_CHANNEL,
                NEW_GAME_MAP_PACKET_SIZE, NEW_GAME_REMOTE_MAP_HEADER,
                1, 1, NEW_GAME_NETWORK_PLAYER_NONE);
            if (!transmitResult)
                ShutDown(0);
            memcpy(netPlayerPacket, gsNetPlayerInfo,
                   NEW_GAME_PLAYER_INFO_PACKET_SIZE);
            transmitResult = TransmitRemoteData(
                netPlayerPacket, NEW_GAME_REMOTE_CHANNEL,
                NEW_GAME_PLAYER_INFO_PACKET_SIZE,
                NEW_GAME_REMOTE_PLAYER_INFO, 1, 1,
                NEW_GAME_NETWORK_PLAYER_NONE);
            if (!transmitResult)
                ShutDown(0);
        }

        LoadGame("origdata.bin", 1, 0);
        if (giNumHumanPlayers > 1) {
            if (iMPBaseType == NEW_GAME_MULTIPLAYER_HOTSEAT)
                m_newGameWindow = new heroWindow(190, 4, "nghs.bin");
            else
                m_newGameWindow = new heroWindow(190, 4, "ngmp.bin");
        } else {
            m_newGameWindow = new heroWindow(190, 33, "ngsp.bin");
        }
        if (m_newGameWindow == 0)
            MemError();
        SetWinText(m_newGameWindow, 7);
        InitNewGame(0);
        InitNewGameWindow();
        UpdateNewGameWindow();
        gbNewGameShadowHidden = 0;
        gbNewGameDialogOver = 0;
        gpWindowManager->DoDialog(m_newGameWindow, NewGameHandler, 0);
        delete m_newGameWindow;
        if (gpWindowManager->m_dialogResult == NEW_GAME_DIALOG_CANCEL) {
            result = 0;
        } else {
            m_playerCount = m_mapHeader.playerCount;
            NewMap(gMapName);
        }
    }

    for (textBufferIndex = 0; textBufferIndex < NEW_GAME_TEXT_BUFFER_COUNT;
         ++textBufferIndex) {
        BaseFree(cTextReceivedBuffer[textBufferIndex],
                 "I:\\Projects\\Heroes\\Prog\\SOURCE\\Newgame.cpp",
                 newGameSourceLineBase + 0x11e);
    }
    BaseFree(cNGKPCore,
             "I:\\Projects\\Heroes\\Prog\\SOURCE\\Newgame.cpp",
             newGameSourceLineBase + 0x120);
    BaseFree(cNGKPDisplay,
             "I:\\Projects\\Heroes\\Prog\\SOURCE\\Newgame.cpp",
             newGameSourceLineBase + 0x121);
    gpResourceManager->Dispose(NGKPBkg);
    return result;
}

VA(0x004b8168, 0xf1)
void game::CleanUpNewGameWindow(void) {}

VA(0x004b8259, 0x67d)
void game::InitNewGameWindow(void) {}

VA(0x004b88d6, 0x59c)
void game::UpdateNewGameWindow(void) {}

VA(0x004b8e72, 0xf46)
int NewGameHandler(struct tag_message &) { return 0; }

VA(0x004b9db8, 0x418)
int game::ProcessNGKeyPress(struct tag_message &) { return 0; }

VA(0x004ba1d0, 0x11e)
void game::NGKPSetupDisplayString(char *, unsigned short int) {}

VA(0x004ba2ee, 0xae)
void game::DrawNGKPDisplayString(int) {}

VA(0x004ba39c, 0xb71)
void game::ShowScenInfo(void) {}

VA(0x004baf0d, 0x1c7)
void game::GetLossConditionText(char *) {}

VA(0x004bb0d4, 0x2fb)
void game::GetVictoryConditionText(char *) {}

VA(0x004bb3cf, 0x2a7)
int game::GetSideDesc(char *, int, int) { return 0; }

// ---- globals (definitions, RVA order) ----
DATA(0x0051cd20) int gbNewGameDialogOver;
DATA(0x0051cd24) int NGKPcursorFlashOn;
DATA(0x00533150) char *cNGKPDisplay;
DATA(0x00533154) int gbNewGameShadowHidden;
DATA(0x00533158) char *cNGKPCore;
DATA(0x0053315c) int NGKPcursorIndex;
DATA(0x00533160) char *cTextReceivedBuffer[3];
DATA(0x0053316c) class icon *NGKPBkg;
