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
#include <BASE/iconWidget.h>
#include <BASE/resourceManager.h>
#include <BASE/textWidget.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/KB.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/REQUEST.h>
#include <SOURCE/SETUP.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/fileRequester.h>
#include <SOURCE/game.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/Newgame.h>

DATA(0x0051cdd0) static short newGameSourceLineBase = NEW_GAME_SOURCE_LINE_BASE;
DATA(0x0051cfa0) static short newGameWindowSourceLineBase =
    NEW_GAME_WINDOW_SOURCE_LINE_BASE;

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
void game::CleanUpNewGameWindow(void)
{
    int player;

    for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
        m_newGameWindow->RemoveAndDeleteWidget(player + NEW_GAME_RACE_FIRST);
        m_newGameWindow->RemoveAndDeleteWidget(player + NEW_GAME_PLAYER_SELECT_FIRST);
        m_newGameWindow->RemoveAndDeleteWidget(player + NEW_GAME_COLOR_FIRST);
        m_newGameWindow->RemoveAndDeleteWidget(player + NEW_GAME_PLAYER_NAME_FIRST);
        m_newGameWindow->RemoveAndDeleteWidget(player + NEW_GAME_RACE_ICON_FIRST);
        m_newGameWindow->RemoveAndDeleteWidget(player + NEW_GAME_RACE_CYCLE_FIRST);
        m_newGameWindow->RemoveAndDeleteWidget(player + NEW_GAME_RACE_NAME_FIRST);
        m_newGameWindow->RemoveAndDeleteWidget(player + NEW_GAME_PLAYER_HUMAN_FIRST);
        m_newGameWindow->RemoveAndDeleteWidget(player + NEW_GAME_HANDICAP_FIRST);
    }
}

VA(0x004b8259, 0x67d)
void game::InitNewGameWindow(void)
{
    int availableWidthResult;
    widget *textControlLocal;
    int firstPlayerXLocal;
    int multiplayerYOffsetValue;
    int playerCounter;
    int playerSpacingTemp;
    widget *iconControlLocal;
    int playerGapValue;
    char *label;
    int raceTextWidth;
    int singlePlayerYOffsetValue;

    iconControlLocal = 0;
    textControlLocal = 0;
    availableWidthResult = 372 - m_mapHeader.playerCount * 62;
    playerGapValue =
        availableWidthResult / (m_mapHeader.playerCount + 1);
    firstPlayerXLocal = playerGapValue + 24;
    playerSpacingTemp = playerGapValue + 62;
    multiplayerYOffsetValue = 0;

    if (giNumHumanPlayers > 1 &&
        iMPBaseType != NEW_GAME_MULTIPLAYER_HOTSEAT)
        multiplayerYOffsetValue = -14;

    for (playerCounter = 0; playerCounter < m_mapHeader.playerCount;
         ++playerCounter) {
        if (giNumHumanPlayers > 1) {
            iconControlLocal = new iconWidget(
                static_cast<short>(playerSpacingTemp * playerCounter +
                                   firstPlayerXLocal + 13),
                static_cast<short>(multiplayerYOffsetValue + 343), 64, 28,
                "ngextra.icn", 59, 0,
                static_cast<short>(playerCounter + NEW_GAME_PLAYER_HUMAN_FIRST),
                16, 1);
            if (iconControlLocal == 0)
                MemError();
            m_newGameWindow->AddWidget(iconControlLocal, -1);

            iconControlLocal = new iconWidget(
                static_cast<short>(playerSpacingTemp * playerCounter +
                                   firstPlayerXLocal + 16),
                static_cast<short>(multiplayerYOffsetValue + 340), 62, 26,
                "ngextra.icn", 0, 0,
                static_cast<short>(playerCounter + NEW_GAME_HANDICAP_FIRST),
                16, 1);
            if (iconControlLocal == 0)
                MemError();
            m_newGameWindow->AddWidget(iconControlLocal, -1);
        }

        iconControlLocal = new iconWidget(
            static_cast<short>(playerSpacingTemp * playerCounter +
                               firstPlayerXLocal + 11),
            197, 68,
            static_cast<short>(((giNumHumanPlayers <= 1) - 1 & 0x11) + 0x2f),
            "ngextra.icn",
            static_cast<short>(((giNumHumanPlayers <= 1) - 1 & -0x13) + 0x4f),
            0, static_cast<short>(playerCounter + NEW_GAME_RACE_FIRST), 16, 1);
        if (iconControlLocal == 0)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<short>(playerSpacingTemp * playerCounter +
                               firstPlayerXLocal + 13),
            191, 68,
            static_cast<short>(((giNumHumanPlayers <= 1) - 1 & 0x0d) + 0x33),
            "ngextra.icn",
            static_cast<short>(((giNumHumanPlayers <= 1) - 1 & -0x11) + 0x50),
            0, static_cast<short>(playerCounter + NEW_GAME_PLAYER_SELECT_FIRST),
            16, 1);
        if (iconControlLocal == 0)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<short>(playerSpacingTemp * playerCounter +
                               firstPlayerXLocal + 16),
            194, 62, 58, "ngextra.icn",
            static_cast<short>(((giNumHumanPlayers <= 1) - 1 & 0x18) + 3), 0,
            static_cast<short>(playerCounter + NEW_GAME_COLOR_FIRST), 16, 1);
        if (iconControlLocal == 0)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);

        if (giNumHumanPlayers > 1) {
            label = static_cast<char *>(BaseAlloc(
                2, "I:\\Projects\\Heroes\\Prog\\SOURCE\\Newgame.cpp",
                newGameWindowSourceLineBase + 0x50));
            sprintf(label, " ");
            textControlLocal = new textWidget(
                static_cast<short>(playerSpacingTemp * playerCounter +
                                   firstPlayerXLocal + 19),
                240, 56, 9, label, "smalfont.fnt", 1,
                static_cast<short>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
                NEW_GAME_MESSAGE_WIDGET, 1);
            if (textControlLocal == 0)
                MemError();
            m_newGameWindow->AddWidget(textControlLocal, -1);
        }

        singlePlayerYOffsetValue = 0;
        if (giNumHumanPlayers == 1)
            singlePlayerYOffsetValue = -14;
        iconControlLocal = new iconWidget(
            static_cast<short>(playerSpacingTemp * playerCounter +
                               firstPlayerXLocal + 11),
            static_cast<short>(multiplayerYOffsetValue +
                               singlePlayerYOffsetValue + 277),
            64, 47, "ngextra.icn", 61, 0,
            static_cast<short>(playerCounter + NEW_GAME_RACE_ICON_FIRST), 16,
            1);
        if (iconControlLocal == 0)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);

        label = static_cast<char *>(BaseAlloc(
            2, "I:\\Projects\\Heroes\\Prog\\SOURCE\\Newgame.cpp",
            newGameWindowSourceLineBase + 0x70));
        sprintf(label, "A");
        if (m_mapHeader.playerCount >= 5) {
            if (m_mapHeader.playerCount >= 6)
                raceTextWidth = 0;
            else
                raceTextWidth = 16;
        } else {
            raceTextWidth = 26;
        }
        textControlLocal = new textWidget(
            static_cast<short>(playerSpacingTemp * playerCounter +
                               firstPlayerXLocal + 15 -
                               raceTextWidth / 2),
            static_cast<short>(multiplayerYOffsetValue +
                               singlePlayerYOffsetValue + 322),
            static_cast<short>(raceTextWidth + 64), 12, label,
            "smalfont.fnt", 1,
            static_cast<short>(playerCounter + NEW_GAME_RACE_NAME_FIRST),
            NEW_GAME_MESSAGE_WIDGET, 1);
        if (textControlLocal == 0)
            MemError();
        m_newGameWindow->AddWidget(textControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<short>(playerSpacingTemp * playerCounter +
                               firstPlayerXLocal + 16),
            static_cast<short>(multiplayerYOffsetValue +
                               singlePlayerYOffsetValue + 274),
            62, 45, "ngextra.icn", 51, 0,
            static_cast<short>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST), 16,
            1);
        if (iconControlLocal == 0)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);
    }
}

// @early-stop: the full instruction stream and all 50 relocation sites match.
// Objdiff's residual is only string/local-constant relocation identity.
VA(0x004b88d6, 0x59c)
void game::UpdateNewGameWindow(void)
{
    int playerLockedValue;
    tag_message messageTemp;
    int player;
    int previousPlayer;

    strcpy(gText, m_mapHeader.name);
    messageTemp.type = NEW_GAME_MESSAGE_WIDGET;
    messageTemp.field4 = NEW_GAME_WIDGET_SET_TEXT;
    messageTemp.field8 = NEW_GAME_SCENARIO_NAME;
    messageTemp.text = gText;
    m_newGameWindow->BroadcastMessage(messageTemp);

    messageTemp.field4 = NEW_GAME_WIDGET_DISABLE;
    messageTemp.field18 = NEW_GAME_WIDGET_REFRESH_FRAME;
    for (player = 0; player < NEW_GAME_DIFFICULTY_COUNT; ++player) {
        messageTemp.field8 = player + NEW_GAME_DIFFICULTY_FIRST;
        m_newGameWindow->BroadcastMessage(messageTemp);
    }
    messageTemp.field4 = NEW_GAME_WIDGET_ENABLE;
    messageTemp.field8 = m_difficulty + NEW_GAME_DIFFICULTY_FIRST;
    m_newGameWindow->BroadcastMessage(messageTemp);

    if (giNumHumanPlayers > 1) {
        for (player = 0; player < NEW_GAME_CHAT_LINE_COUNT; ++player) {
            sprintf(gText, cTextReceivedBuffer[player]);
            messageTemp.field4 = NEW_GAME_WIDGET_SET_TEXT;
            messageTemp.field8 = player + NEW_GAME_CHAT_FIRST;
            messageTemp.text = gText;
            m_newGameWindow->BroadcastMessage(messageTemp);
        }
    }

    for (player = 0; player < m_mapHeader.playerCount; ++player) {
        if (m_setupPlayerNetworkId[player] == NEW_GAME_COMPUTER_PLAYER) {
            sprintf(gText, "");
        } else if (strlen(cPlayerNames[m_setupPlayerNetworkId[player]]) != 0) {
            sprintf(gText, cPlayerNames[m_setupPlayerNetworkId[player]]);
        } else {
            sprintf(gText, "Player %d", m_setupPlayerNetworkId[player] + 1);
        }
        messageTemp.field4 = NEW_GAME_WIDGET_SET_TEXT;
        messageTemp.field8 = player + NEW_GAME_PLAYER_NAME_FIRST;
        messageTemp.text = gText;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (m_selectedSetupPlayer == player)
            messageTemp.field4 = NEW_GAME_WIDGET_ENABLE;
        else
            messageTemp.field4 = NEW_GAME_WIDGET_DISABLE;
        messageTemp.field8 = player + NEW_GAME_PLAYER_SELECT_FIRST;
        messageTemp.field18 = NEW_GAME_WIDGET_REFRESH_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (m_setupPlayerType[player] != NEW_GAME_PLAYER_DEFAULT ||
            (giNumHumanPlayers > 1 &&
             m_setupPlayerNetworkId[player] != NEW_GAME_COMPUTER_PLAYER))
            playerLockedValue = 0;
        else
            playerLockedValue = 1;
        messageTemp.field4 = NEW_GAME_WIDGET_SET_FRAME;
        messageTemp.field8 = player + NEW_GAME_COLOR_FIRST;
        if (m_setupPlayerNetworkId[player] == NEW_GAME_COMPUTER_PLAYER)
            messageTemp.field18 = m_setupPlayerColor[player] +
                (playerLockedValue ? NEW_GAME_COMPUTER_COLOR_LOCKED_FRAME
                                   : NEW_GAME_COMPUTER_COLOR_UNLOCKED_FRAME);
        else
            messageTemp.field18 = m_setupPlayerColor[player] +
                (playerLockedValue ? NEW_GAME_HUMAN_COLOR_LOCKED_FRAME
                                   : NEW_GAME_HUMAN_COLOR_UNLOCKED_FRAME);
        if (giNumHumanPlayers > 1)
            messageTemp.field18 += NEW_GAME_MULTIPLAYER_COLOR_FRAME_OFFSET;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (playerLockedValue)
            messageTemp.field4 = NEW_GAME_WIDGET_DISABLE;
        else
            messageTemp.field4 = NEW_GAME_WIDGET_ENABLE;
        messageTemp.field18 = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);

        messageTemp.field4 = NEW_GAME_WIDGET_SET_FRAME;
        messageTemp.field8 = player + NEW_GAME_HANDICAP_FIRST;
        if (m_setupPlayerNetworkId[player] == NEW_GAME_COMPUTER_PLAYER)
            messageTemp.field18 = NEW_GAME_RACE_NAME_FIRST;
        else
            messageTemp.field18 = m_playerHandicap[player];
        m_newGameWindow->BroadcastMessage(messageTemp);
        if (m_setupPlayerNetworkId[player] == NEW_GAME_COMPUTER_PLAYER)
            messageTemp.field4 = NEW_GAME_WIDGET_DISABLE;
        else
            messageTemp.field4 = NEW_GAME_WIDGET_ENABLE;
        messageTemp.field18 = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (m_mapHeader.playerRace[m_setupPlayerColor[player]] ==
            NEW_GAME_RANDOM_RACE)
            playerLockedValue = 0;
        else
            playerLockedValue = 1;
        messageTemp.field4 = NEW_GAME_WIDGET_ENABLE;
        messageTemp.field18 = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);
        messageTemp.field4 = NEW_GAME_WIDGET_SET_FRAME;
        messageTemp.field8 = player + NEW_GAME_RACE_CYCLE_FIRST;
        messageTemp.field18 = m_setupPlayerRace[player] +
            (playerLockedValue ? NEW_GAME_FIXED_RACE_FRAME_BASE
                               : NEW_GAME_RANDOM_RACE_FRAME_BASE);
        m_newGameWindow->BroadcastMessage(messageTemp);

        sprintf(gText, gAlignmentNames[m_setupPlayerRace[player]]);
        messageTemp.field4 = NEW_GAME_WIDGET_SET_TEXT;
        messageTemp.field8 = player + NEW_GAME_RACE_NAME_FIRST;
        messageTemp.text = gText;
        m_newGameWindow->BroadcastMessage(messageTemp);
        if (playerLockedValue)
            messageTemp.field4 = NEW_GAME_WIDGET_DISABLE;
        else
            messageTemp.field4 = NEW_GAME_WIDGET_ENABLE;
        messageTemp.field18 = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);
    }

    gpGame->m_difficultyRating =
        static_cast<short>(CalcDifficultyRating());
    messageTemp.field4 = NEW_GAME_WIDGET_SET_TEXT;
    messageTemp.field8 = NEW_GAME_RATING;
    sprintf(gText, "%s %d%%", "Rating", gpGame->m_difficultyRating);
    messageTemp.text = gText;
    m_newGameWindow->BroadcastMessage(messageTemp);
    DrawNGKPDisplayString(0);
}

// @early-stop: after excluding retail switch data [0x21fe,0x2242) and
// [0x2d11,0x2d8d), and base switch data [0x21c8,0x220c) and
// [0x2cf3,0x2d73), the base has eight extra five-byte continuation jumps. Its
// equivalent race increment load order saves one byte, and its second local
// switch table has one extra four-byte entry, exactly accounting for the 0x2b
// size delta. External relocation order was audited over the full object
// ranges; residual identities are equivalent adjacent-global addends and
// delinked local labels.
VA(0x004b8e72, 0xf46)
int NewGameHandler(struct tag_message &message)
{
    int transmitResultTemp;
    int redrawWindow = 0;
    int oldNetworkId;
    int swapPlayerTemp;
    int currentPlayerLocal;
    int synchronizeSetupResult = 0;
    tag_message windowMessage;
    SMapHeader remoteMapHeaderValue;
    NewGameRemotePacket *remotePacketResult;
    int sender;
    char setupData[NEW_GAME_SETUP_BUFFER_SIZE];
    char mapPacketLocal[NEW_GAME_MAP_PACKET_SIZE];
    tag_message mapWindowMessageTemp;
    int helpDialogIndexLocal;
    int unusedSender;
    char mapNamePacket[MAP_HEADER_NAME_SIZE];

    if (!gbNewGameShadowHidden) {
        gbNewGameShadowHidden = 1;
        windowMessage.type = NEW_GAME_MESSAGE_WIDGET;
        windowMessage.field4 = NEW_GAME_WIDGET_DISABLE;
        windowMessage.field8 = NEW_GAME_SHADOW;
        windowMessage.field18 = NEW_GAME_SHADOW_FRAME;
        gpGame->m_newGameWindow->BroadcastMessage(windowMessage);
    }

    if (message.type == NEW_GAME_MESSAGE_IDLE) {
        remotePacketResult =
            reinterpret_cast<NewGameRemotePacket *>(GetRemoteData(1));
        if (remotePacketResult != 0 &&
            (remotePacketResult->type == NEW_GAME_REMOTE_PACKET_TYPE ||
             remotePacketResult->type ==
                 NEW_GAME_REMOTE_PACKET_TYPE_ALTERNATE)) {
            switch (remotePacketResult->command) {
            case NEW_GAME_REMOTE_START:
                gpWindowManager->m_dialogResult = message.field8;
                gpWindowManager->m_dialogResult = NEW_GAME_DIALOG_OK;
                message.type = NEW_GAME_MESSAGE_WIDGET;
                message.field8 = NEW_GAME_DIALOG_CLOSE_MESSAGE;
                message.field4 = message.field8;
                return 2;

            case NEW_GAME_REMOTE_CANCEL:
                NormalDialog("The host has canceled the game.", 1,
                             -1, -1, -1, 0, -1, 0, -1, 0);
                ShutDown(0);
                break;

            case NEW_GAME_REMOTE_SETUP:
                if (strcmp(remotePacketResult->payload,
                           gpGame->m_mapHeader.name) != 0)
                    break;
                memcpy(gpGame->m_setupPlayerColor,
                       remotePacketResult->payload + MAP_HEADER_NAME_SIZE,
                       NEW_GAME_SETUP_DATA_SIZE);
                redrawWindow = 1;
                break;

            case NEW_GAME_REMOTE_MAP_HEADER:
                memset(&remoteMapHeaderValue, 0,
                       sizeof(remoteMapHeaderValue));
                memcpy(&remoteMapHeaderValue, remotePacketResult->payload,
                       NEW_GAME_MAP_PACKET_SIZE);
                gpGame->ProcessNewMap(&remoteMapHeaderValue);
                break;

            case NEW_GAME_REMOTE_CHAT:
                redrawWindow = 1;
                sender = remotePacketResult->sender;
                if (sender >= 0) {
                    sprintf(gText, "%s:  %s", gsNetPlayerInfo[sender].name,
                            remotePacketResult->payload);
                } else {
                    unusedSender = 0;
                }
                gText[NEW_GAME_CHAT_TEXT_LIMIT] = 0;
                for (currentPlayerLocal = 0;
                     currentPlayerLocal < NEW_GAME_CHAT_LINE_COUNT - 1;
                     ++currentPlayerLocal) {
                    strcpy(cTextReceivedBuffer[currentPlayerLocal],
                           cTextReceivedBuffer[currentPlayerLocal + 1]);
                }
                strcpy(cTextReceivedBuffer[NEW_GAME_CHAT_LINE_COUNT - 1], gText);
                break;
            }
        }
        if (glTimers[0] < static_cast<int>(KBTickCount())) {
            gpGame->NGKPSetupDisplayString(
                cNGKPCore, static_cast<unsigned short>(NGKPcursorIndex));
            gpGame->DrawNGKPDisplayString(1);
        }
    }

    if (message.type == NEW_GAME_MESSAGE_KEY &&
        giNumHumanPlayers > 1 &&
        iMPBaseType != NEW_GAME_MULTIPLAYER_HOTSEAT &&
        gpGame->ProcessNGKeyPress(message)) {
        redrawWindow = 1;
        for (currentPlayerLocal = 0;
             currentPlayerLocal < NEW_GAME_CHAT_LINE_COUNT - 1;
             ++currentPlayerLocal) {
            strcpy(cTextReceivedBuffer[currentPlayerLocal],
                   cTextReceivedBuffer[currentPlayerLocal + 1]);
        }
        strcpy(cTextReceivedBuffer[NEW_GAME_CHAT_LINE_COUNT - 1], cNGKPCore);
        strcpy(cNGKPCore, "");
        strcpy(cNGKPDisplay, "");
        NGKPcursorIndex = 0;
        transmitResultTemp = TransmitRemoteData(
            cTextReceivedBuffer[NEW_GAME_CHAT_LINE_COUNT - 1],
            NEW_GAME_REMOTE_CHANNEL,
            strlen(cTextReceivedBuffer[NEW_GAME_CHAT_LINE_COUNT - 1]) + 1,
            NEW_GAME_REMOTE_CHAT, 1, 1, NEW_GAME_NETWORK_PLAYER_NONE);
        if (!transmitResultTemp)
            ShutDown(0);
    }

    if (message.type != NEW_GAME_MESSAGE_WIDGET)
        goto finish;

    if (message.fieldC & NEW_GAME_MOUSE_RIGHT_FLAG) {
        if (message.field4 == NEW_GAME_EVENT_PRESS ||
            message.field4 == NEW_GAME_EVENT_ALTERNATE_PRESS) {
            helpDialogIndexLocal = -1;
            if ((message.field8 >= NEW_GAME_DIFFICULTY_HELP_FIRST &&
                 message.field8 <= NEW_GAME_DIFFICULTY_HELP_FIRST +
                                           NEW_GAME_DIFFICULTY_COUNT - 1) ||
                (message.field8 >= NEW_GAME_DIFFICULTY_FIRST &&
                 message.field8 <= NEW_GAME_DIFFICULTY_FIRST +
                                           NEW_GAME_DIFFICULTY_COUNT - 1))
                helpDialogIndexLocal = NEW_GAME_HELP_DIFFICULTY;
            if ((message.field8 >= NEW_GAME_HANDICAP_FIRST &&
                 message.field8 <= NEW_GAME_HANDICAP_FIRST +
                                           NEW_GAME_PLAYER_CONTROL_COUNT - 1) ||
                (message.field8 >= NEW_GAME_PLAYER_HUMAN_FIRST &&
                 message.field8 <= NEW_GAME_PLAYER_HUMAN_FIRST +
                                           NEW_GAME_PLAYER_CONTROL_COUNT - 1))
                helpDialogIndexLocal = NEW_GAME_HELP_HANDICAP;
            if ((message.field8 >= NEW_GAME_COLOR_FIRST &&
                 message.field8 <= NEW_GAME_COLOR_FIRST +
                                           NEW_GAME_PLAYER_CONTROL_COUNT - 1) ||
                (message.field8 >= NEW_GAME_RACE_FIRST &&
                 message.field8 <= NEW_GAME_RACE_FIRST +
                                           NEW_GAME_PLAYER_CONTROL_COUNT - 1) ||
                (message.field8 >= NEW_GAME_PLAYER_SELECT_FIRST &&
                 message.field8 <= NEW_GAME_PLAYER_NAME_FIRST) ||
                (message.field8 >= NEW_GAME_PLAYER_NAME_FIRST &&
                 message.field8 <= NEW_GAME_RACE_ICON_FIRST))
                helpDialogIndexLocal = NEW_GAME_HELP_PLAYER;
            if ((message.field8 >= NEW_GAME_RACE_CYCLE_FIRST &&
                 message.field8 <= NEW_GAME_RACE_CYCLE_FIRST +
                                           NEW_GAME_PLAYER_CONTROL_COUNT - 1) ||
                (message.field8 >= NEW_GAME_RACE_ICON_FIRST &&
                 message.field8 <= NEW_GAME_RACE_ICON_FIRST +
                                           NEW_GAME_PLAYER_CONTROL_COUNT - 1))
                helpDialogIndexLocal = NEW_GAME_HELP_RACE;
            if (message.field8 == NEW_GAME_MAP_OPTIONS_CONTROL ||
                message.field8 == NEW_GAME_MAP_SELECT ||
                message.field8 == NEW_GAME_SCENARIO_NAME)
                helpDialogIndexLocal = NEW_GAME_HELP_MAP;
            if (message.field8 == NEW_GAME_RATING)
                helpDialogIndexLocal = NEW_GAME_HELP_RATING;
            if (message.field8 == NEW_GAME_DIALOG_OK)
                helpDialogIndexLocal = NEW_GAME_HELP_OK;
            if (message.field8 == NEW_GAME_DIALOG_CANCEL)
                helpDialogIndexLocal = NEW_GAME_HELP_CANCEL;
            if (helpDialogIndexLocal != -1)
                NormalDialog(gNewGameHelp[helpDialogIndexLocal], 4,
                             -1, -1, -1, 0, -1, 0, -1, 0);
        }
        goto finish;
    }

    switch (message.field4) {
    case NEW_GAME_EVENT_RELEASE:
        switch (message.field8) {
        case NEW_GAME_MAP_OPTIONS_CONTROL:
            goto chooseMap;

        case NEW_GAME_DIALOG_OK:
            if (gbRemoteOn) {
                transmitResultTemp = TransmitRemoteData(
                    0, NEW_GAME_REMOTE_CHANNEL, 0,
                    NEW_GAME_REMOTE_START, 1, 1,
                    NEW_GAME_NETWORK_PLAYER_NONE);
            }
            gpWindowManager->m_dialogResult = message.field8;
            message.field8 = NEW_GAME_DIALOG_CLOSE_MESSAGE;
            message.field4 = message.field8;
            gbNewGameDialogOver = 1;
            return 2;

        case NEW_GAME_DIALOG_CANCEL:
            if (gbRemoteOn) {
                transmitResultTemp = TransmitRemoteData(
                    0, NEW_GAME_REMOTE_CHANNEL, 0,
                    NEW_GAME_REMOTE_CANCEL, 1, 1,
                    NEW_GAME_NETWORK_PLAYER_NONE);
                ShutDown(0);
            }
            gpWindowManager->m_dialogResult = message.field8;
            message.field8 = NEW_GAME_DIALOG_CLOSE_MESSAGE;
            message.field4 = message.field8;
            gbNewGameDialogOver = 1;
            return 2;

        default:
            break;
        }
        break;

    case NEW_GAME_EVENT_PRESS:
        switch (message.field8) {
    case NEW_GAME_DIFFICULTY_HELP_FIRST + 0:
    case NEW_GAME_DIFFICULTY_HELP_FIRST + 1:
    case NEW_GAME_DIFFICULTY_HELP_FIRST + 2:
    case NEW_GAME_DIFFICULTY_HELP_FIRST + 3:
    case NEW_GAME_DIFFICULTY_HELP_FIRST + 4:
        currentPlayerLocal =
            message.field8 - NEW_GAME_DIFFICULTY_HELP_FIRST;
        goto setDifficulty;

    case NEW_GAME_DIFFICULTY_FIRST:
    case NEW_GAME_DIFFICULTY_FIRST + 1:
    case NEW_GAME_DIFFICULTY_FIRST + 2:
    case NEW_GAME_DIFFICULTY_FIRST + 3:
    case NEW_GAME_DIFFICULTY_FIRST + 4:
        currentPlayerLocal = message.field8 - NEW_GAME_DIFFICULTY_FIRST;
setDifficulty:
        gpGame->m_difficulty = static_cast<signed char>(currentPlayerLocal);
        synchronizeSetupResult = 1;
        redrawWindow = 1;
        break;

    case NEW_GAME_HANDICAP_FIRST + 0:
    case NEW_GAME_HANDICAP_FIRST + 1:
    case NEW_GAME_HANDICAP_FIRST + 2:
    case NEW_GAME_HANDICAP_FIRST + 3:
    case NEW_GAME_HANDICAP_FIRST + 4:
    case NEW_GAME_HANDICAP_FIRST + 5:
        currentPlayerLocal = message.field8 - NEW_GAME_HANDICAP_FIRST;
        goto cycleHandicap;

    case NEW_GAME_PLAYER_HUMAN_FIRST + 0:
    case NEW_GAME_PLAYER_HUMAN_FIRST + 1:
    case NEW_GAME_PLAYER_HUMAN_FIRST + 2:
    case NEW_GAME_PLAYER_HUMAN_FIRST + 3:
    case NEW_GAME_PLAYER_HUMAN_FIRST + 4:
    case NEW_GAME_PLAYER_HUMAN_FIRST + 5:
        currentPlayerLocal = message.field8 - NEW_GAME_PLAYER_HUMAN_FIRST;
cycleHandicap:
        synchronizeSetupResult = 1;
        redrawWindow = 1;
        if (gpGame->m_setupPlayerNetworkId[currentPlayerLocal] !=
            NEW_GAME_COMPUTER_PLAYER) {
            gpGame->m_playerHandicap[currentPlayerLocal] =
                static_cast<signed char>(
                    (gpGame->m_playerHandicap[currentPlayerLocal] + 1) %
                    NEW_GAME_HANDICAP_COUNT);
        }
        break;

    case NEW_GAME_RACE_FIRST + 0:
    case NEW_GAME_RACE_FIRST + 1:
    case NEW_GAME_RACE_FIRST + 2:
    case NEW_GAME_RACE_FIRST + 3:
    case NEW_GAME_RACE_FIRST + 4:
    case NEW_GAME_RACE_FIRST + 5:
        currentPlayerLocal = message.field8 - NEW_GAME_RACE_FIRST;
        goto selectPlayer;

    case NEW_GAME_COLOR_FIRST + 0:
    case NEW_GAME_COLOR_FIRST + 1:
    case NEW_GAME_COLOR_FIRST + 2:
    case NEW_GAME_COLOR_FIRST + 3:
    case NEW_GAME_COLOR_FIRST + 4:
    case NEW_GAME_COLOR_FIRST + 5:
        currentPlayerLocal = message.field8 - NEW_GAME_COLOR_FIRST;
        goto selectPlayer;

    case NEW_GAME_PLAYER_SELECT_FIRST + 0:
    case NEW_GAME_PLAYER_SELECT_FIRST + 1:
    case NEW_GAME_PLAYER_SELECT_FIRST + 2:
    case NEW_GAME_PLAYER_SELECT_FIRST + 3:
    case NEW_GAME_PLAYER_SELECT_FIRST + 4:
    case NEW_GAME_PLAYER_SELECT_FIRST + 5:
        currentPlayerLocal = message.field8 - NEW_GAME_PLAYER_SELECT_FIRST;
        goto selectPlayer;

    case NEW_GAME_PLAYER_NAME_FIRST + 0:
    case NEW_GAME_PLAYER_NAME_FIRST + 1:
    case NEW_GAME_PLAYER_NAME_FIRST + 2:
    case NEW_GAME_PLAYER_NAME_FIRST + 3:
    case NEW_GAME_PLAYER_NAME_FIRST + 4:
    case NEW_GAME_PLAYER_NAME_FIRST + 5:
        currentPlayerLocal = message.field8 - NEW_GAME_PLAYER_NAME_FIRST;
selectPlayer:
        synchronizeSetupResult = 1;
        redrawWindow = 1;
        if (gpGame->m_setupPlayerType[currentPlayerLocal] !=
                NEW_GAME_PLAYER_DEFAULT ||
            (giNumHumanPlayers > 1 &&
             gpGame->m_setupPlayerNetworkId[currentPlayerLocal] !=
                 NEW_GAME_COMPUTER_PLAYER)) {
            if (giNumHumanPlayers == 1) {
                if (gpGame->m_setupPlayerNetworkId[currentPlayerLocal] ==
                    NEW_GAME_COMPUTER_PLAYER) {
                    for (swapPlayerTemp = 0;
                         swapPlayerTemp < gpGame->m_mapHeader.playerCount;
                         ++swapPlayerTemp) {
                        if (gpGame->m_setupPlayerNetworkId[swapPlayerTemp] !=
                            NEW_GAME_COMPUTER_PLAYER) {
                            oldNetworkId =
                                gpGame->m_setupPlayerNetworkId[swapPlayerTemp];
                            gpGame->m_setupPlayerNetworkId[swapPlayerTemp] =
                                gpGame->m_setupPlayerNetworkId[
                                    currentPlayerLocal];
                            gpGame->m_setupPlayerNetworkId[currentPlayerLocal] =
                                oldNetworkId;
                            swapPlayerTemp = NEW_GAME_SWAP_SEARCH_DONE;
                        }
                    }
                }
            } else if (gpGame->m_selectedSetupPlayer ==
                       NEW_GAME_NETWORK_PLAYER_NONE) {
                gpGame->m_selectedSetupPlayer =
                    static_cast<signed char>(currentPlayerLocal);
            } else if (gpGame->m_selectedSetupPlayer == currentPlayerLocal ||
                       (gpGame->m_setupPlayerNetworkId[currentPlayerLocal] ==
                            NEW_GAME_COMPUTER_PLAYER &&
                        gpGame->m_setupPlayerNetworkId[
                            gpGame->m_selectedSetupPlayer] ==
                            NEW_GAME_COMPUTER_PLAYER)) {
                gpGame->m_selectedSetupPlayer = NEW_GAME_NETWORK_PLAYER_NONE;
            } else {
                if ((gpGame->m_setupPlayerType[currentPlayerLocal] !=
                         NEW_GAME_PLAYER_DEFAULT &&
                     gpGame->m_setupPlayerType[
                         gpGame->m_selectedSetupPlayer] !=
                         NEW_GAME_PLAYER_DEFAULT) ||
                    (gpGame->m_setupPlayerNetworkId[currentPlayerLocal] !=
                         NEW_GAME_COMPUTER_PLAYER &&
                     gpGame->m_setupPlayerNetworkId[
                         gpGame->m_selectedSetupPlayer] !=
                         NEW_GAME_COMPUTER_PLAYER)) {
                    swapPlayerTemp =
                        gpGame->m_setupPlayerNetworkId[currentPlayerLocal];
                    gpGame->m_setupPlayerNetworkId[currentPlayerLocal] =
                        gpGame->m_setupPlayerNetworkId[
                            gpGame->m_selectedSetupPlayer];
                    gpGame->m_setupPlayerNetworkId[
                        gpGame->m_selectedSetupPlayer] =
                        static_cast<signed char>(swapPlayerTemp);
                } else {
                    NormalDialog(
                        "The two positions selected can not be swapped.",
                        1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
                gpGame->m_selectedSetupPlayer = NEW_GAME_NETWORK_PLAYER_NONE;
            }
        }
        break;

    case NEW_GAME_RACE_CYCLE_FIRST + 0:
    case NEW_GAME_RACE_CYCLE_FIRST + 1:
    case NEW_GAME_RACE_CYCLE_FIRST + 2:
    case NEW_GAME_RACE_CYCLE_FIRST + 3:
    case NEW_GAME_RACE_CYCLE_FIRST + 4:
    case NEW_GAME_RACE_CYCLE_FIRST + 5:
        currentPlayerLocal = message.field8 - NEW_GAME_RACE_CYCLE_FIRST;
        goto cycleRace;

    case NEW_GAME_RACE_ICON_FIRST + 0:
    case NEW_GAME_RACE_ICON_FIRST + 1:
    case NEW_GAME_RACE_ICON_FIRST + 2:
    case NEW_GAME_RACE_ICON_FIRST + 3:
    case NEW_GAME_RACE_ICON_FIRST + 4:
    case NEW_GAME_RACE_ICON_FIRST + 5:
        currentPlayerLocal = message.field8 - NEW_GAME_RACE_ICON_FIRST;
cycleRace:
        if (gpGame->m_mapHeader.playerRace[
                gpGame->m_setupPlayerColor[currentPlayerLocal]] ==
            NEW_GAME_RANDOM_RACE) {
            if (gpGame->m_setupPlayerRace[currentPlayerLocal] ==
                NEW_GAME_RANDOM_RACE)
                gpGame->m_setupPlayerRace[currentPlayerLocal] = 0;
            else if (gpGame->m_setupPlayerRace[currentPlayerLocal] ==
                     NEW_GAME_LAST_STANDARD_RACE)
                gpGame->m_setupPlayerRace[currentPlayerLocal] =
                    NEW_GAME_RANDOM_RACE;
            else
                ++gpGame->m_setupPlayerRace[currentPlayerLocal];
            synchronizeSetupResult = 1;
            redrawWindow = 1;
        }
        break;

    case NEW_GAME_MAP_OPTIONS_CONTROL:
        break;

    case NEW_GAME_SCENARIO_NAME:
    case NEW_GAME_MAP_SELECT:
chooseMap:
        if (gbRemoteOn && giThisNetPos != 0)
            break;
        {
            mapWindowMessageTemp.type = NEW_GAME_MESSAGE_WIDGET;
            mapWindowMessageTemp.field4 = NEW_GAME_WIDGET_DISABLE;
            mapWindowMessageTemp.field8 = NEW_GAME_DIALOG_CANCEL;
            mapWindowMessageTemp.field18 = NEW_GAME_WIDGET_INACTIVE_FRAME;
            gpGame->m_newGameWindow->BroadcastMessage(mapWindowMessageTemp);
            gpGame->GetMap();
            mapWindowMessageTemp.type = NEW_GAME_MESSAGE_WIDGET;
            mapWindowMessageTemp.field4 = NEW_GAME_WIDGET_ENABLE;
            mapWindowMessageTemp.field8 = NEW_GAME_DIALOG_CANCEL;
            mapWindowMessageTemp.field18 = NEW_GAME_WIDGET_INACTIVE_FRAME;
            gpGame->m_newGameWindow->BroadcastMessage(mapWindowMessageTemp);
            if (gbRemoteOn) {
                memcpy(mapPacketLocal, &gpGame->m_mapHeader,
                       NEW_GAME_MAP_PACKET_SIZE);
                transmitResultTemp = TransmitRemoteData(
                    mapPacketLocal, NEW_GAME_REMOTE_CHANNEL,
                    NEW_GAME_MAP_PACKET_SIZE, NEW_GAME_REMOTE_MAP_HEADER,
                    1, 1, NEW_GAME_NETWORK_PLAYER_NONE);
            }
        }
        break;

    default:
        break;
        }
        break;

    default:
        break;
    }

finish:
    if (redrawWindow) {
        gpGame->UpdateNewGameWindow();
        gpGame->m_newGameWindow->DrawWindow();
    }
    if (synchronizeSetupResult && gbRemoteOn) {
        memcpy(mapNamePacket, gpGame->m_mapHeader.name, MAP_HEADER_NAME_SIZE);
        memcpy(setupData, gpGame->m_setupPlayerColor,
               NEW_GAME_SETUP_DATA_SIZE);
        transmitResultTemp = TransmitRemoteData(
            mapNamePacket, NEW_GAME_REMOTE_CHANNEL,
            NEW_GAME_SETUP_PACKET_SIZE, NEW_GAME_REMOTE_SETUP, 1, 1,
            NEW_GAME_NETWORK_PLAYER_NONE);
        if (!transmitResultTemp)
            ShutDown(0);
    }
    return 1;
}

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
