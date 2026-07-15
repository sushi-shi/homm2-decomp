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
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
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
DATA(0x0051d0fc) static short scenarioInfoSourceLineBase =
    NEW_GAME_SCENARIO_SOURCE_LINE_BASE;

VA(0x004b6f40, 0x1d5)
void game::GetMap(void)
{
    fileRequester *requesterResult;
    int resultCode;
    char fileMask[16];
    char savedName[44];

    strcpy(savedName, gMapName);
    strcpy(gcCurMapName, "");
    if (gbRemoteOn && xNetHasOldPlayers) {
        NormalDialog(
            "At least one player does not have the Heroes II Expansion set.  You will only be able to choose from original Heroes II games.",
            1, -1, -1, -1, 0, -1, 0, -1, 0);
        sprintf(fileMask, "*.%s", "MP2");
    } else if (xIsExpansionMap) {
        sprintf(fileMask, "*.%s", "MX2");
    } else {
        sprintf(fileMask, "*.%s", "MP2");
    }

    requesterResult = new fileRequester(212, 9, FILE_REQUESTER_MAP_GAME,
                                        fileMask, gcMapPath, fileMask);
    if (requesterResult == 0)
        MemError();
    resultCode = gpExec->DoDialog(requesterResult);
    if (resultCode == FILE_REQUESTER_OK) {
        delete requesterResult;
        strcpy(gMapName, gLastFilename);
        if (_strcmpi(savedName, gMapName) != 0) {
            strcpy(m_mapFilename, gMapName);
            ProcessNewMap(0);
        }
    } else {
        delete requesterResult;
        strcpy(gMapName, savedName);
    }
}

// @early-stop 99.85714%: byte-proven branch-destination wall. The 0x04 frame,
// all 33 non-jump instructions and operands, and all 6 ordered relocation
// types/resolved targets agree. The sole unmasked byte residual is the branch
// displacement at +0x35. A positive-arm spelling emitted an extra jne/jmp pair
// and dropped to 95.43%; do not retry it without a TU-state change.
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

// @early-stop 99.22932%: byte-proven /Od continuation-jump wall. The 0x1c
// frame and local slots, all 244 non-jump instructions and operands, and all 7
// ordered relocation types/resolved targets agree. Retail has 22 unconditional
// jumps versus 20 here, exactly accounting for the 10-byte size delta. Commuting
// the two human-player comparisons removed the only operand-order residual.
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

        if (humanPlayers < giNumHumanPlayers &&
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
            if (humanPlayers < giNumHumanPlayers &&
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

// @early-stop 98.35664%: byte-proven /Od continuation-jump wall. The complete
// new-game menu, allocation, remote packet wait/dispatch, map selection,
// transmit, dialog, and cleanup flow has the retail 0x1f0 frame. All 537
// non-jump instructions and operands and all 156 ordered relocation types and
// semantic targets agree; every resolvable pair has the same RVA/addend. Retail
// has 35 unconditional jumps versus 30 here, exactly accounting for the
// 25-byte size delta.
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
            do {
                do {
                    PollSound();
                    remoteBuffer = reinterpret_cast<NewGameRemotePacket *>(
                        GetRemoteData(1));
                } while (remoteBuffer == 0);
            } while (remoteBuffer->type != NEW_GAME_REMOTE_PACKET_TYPE);

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
        } while (!playerDataReceived || !mapHeaderLoaded);

        m_newGameWindow = new heroWindow(190, 4, "ngmp.bin");
        InitNewGame(&m_mapHeader);
        InitNewGameWindow();
        UpdateNewGameWindow();

        windowMessage.type = NEW_GAME_MESSAGE_WIDGET;
        windowMessage.payload.widget.id = NEW_GAME_MAP_OPTIONS_CONTROL;
        windowMessage.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        windowMessage.payload.widget.data.value = NEW_GAME_WIDGET_ACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(windowMessage);
        windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        windowMessage.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(windowMessage);
        windowMessage.payload.widget.id = NEW_GAME_DIALOG_OK;
        windowMessage.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        windowMessage.payload.widget.data.value = NEW_GAME_WIDGET_ACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(windowMessage);
        windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        windowMessage.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(windowMessage);
        windowMessage.payload.widget.id = NEW_GAME_DIALOG_CANCEL;
        windowMessage.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        windowMessage.payload.widget.data.value = NEW_GAME_WIDGET_ACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(windowMessage);
        windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        windowMessage.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
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

// @early-stop
// The full instruction stream and all 50 relocation sites match.
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
    messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
    messageTemp.payload.widget.id = EncodeNewGameControl(NEW_GAME_SCENARIO_NAME);
    messageTemp.payload.widget.data.text = gText;
    m_newGameWindow->BroadcastMessage(messageTemp);

    messageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
    messageTemp.payload.widget.data.value = NEW_GAME_WIDGET_REFRESH_FRAME;
    for (player = 0; player < NEW_GAME_DIFFICULTY_COUNT; ++player) {
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_DIFFICULTY_FIRST, player);
        m_newGameWindow->BroadcastMessage(messageTemp);
    }
    messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
    messageTemp.payload.widget.id =
        EncodeNewGameControlIndex(NEW_GAME_DIFFICULTY_FIRST, m_difficulty);
    m_newGameWindow->BroadcastMessage(messageTemp);

    if (giNumHumanPlayers > 1) {
        for (player = 0; player < NEW_GAME_CHAT_LINE_COUNT; ++player) {
            sprintf(gText, cTextReceivedBuffer[player]);
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
            messageTemp.payload.widget.id =
                EncodeNewGameControlIndex(NEW_GAME_CHAT_FIRST, player);
            messageTemp.payload.widget.data.text = gText;
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
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_PLAYER_NAME_FIRST, player);
        messageTemp.payload.widget.data.text = gText;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (m_selectedSetupPlayer == player)
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        else
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_PLAYER_SELECT_FIRST, player);
        messageTemp.payload.widget.data.value = NEW_GAME_WIDGET_REFRESH_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (m_setupPlayerType[player] != NEW_GAME_PLAYER_DEFAULT ||
            (giNumHumanPlayers > 1 &&
             m_setupPlayerNetworkId[player] != NEW_GAME_COMPUTER_PLAYER))
            playerLockedValue = 0;
        else
            playerLockedValue = 1;
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_COLOR_FIRST, player);
        if (m_setupPlayerNetworkId[player] == NEW_GAME_COMPUTER_PLAYER)
            messageTemp.payload.widget.data.value = m_setupPlayerColor[player] +
                (playerLockedValue ? NEW_GAME_COMPUTER_COLOR_LOCKED_FRAME
                                   : NEW_GAME_COMPUTER_COLOR_UNLOCKED_FRAME);
        else
            messageTemp.payload.widget.data.value = m_setupPlayerColor[player] +
                (playerLockedValue ? NEW_GAME_HUMAN_COLOR_LOCKED_FRAME
                                   : NEW_GAME_HUMAN_COLOR_UNLOCKED_FRAME);
        if (giNumHumanPlayers > 1)
            messageTemp.payload.widget.data.value += NEW_GAME_MULTIPLAYER_COLOR_FRAME_OFFSET;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (playerLockedValue)
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        else
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        messageTemp.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);

        messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_HANDICAP_FIRST, player);
        if (m_setupPlayerNetworkId[player] == NEW_GAME_COMPUTER_PLAYER)
            messageTemp.payload.widget.data.value = NEW_GAME_RACE_NAME_FIRST;
        else
            messageTemp.payload.widget.data.value = m_playerHandicap[player];
        m_newGameWindow->BroadcastMessage(messageTemp);
        if (m_setupPlayerNetworkId[player] == NEW_GAME_COMPUTER_PLAYER)
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        else
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        messageTemp.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (m_mapHeader.playerRace[m_setupPlayerColor[player]] ==
            NEW_GAME_RANDOM_RACE)
            playerLockedValue = 0;
        else
            playerLockedValue = 1;
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        messageTemp.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_RACE_CYCLE_FIRST, player);
        messageTemp.payload.widget.data.value = m_setupPlayerRace[player] +
            (playerLockedValue ? NEW_GAME_FIXED_RACE_FRAME_BASE
                               : NEW_GAME_RANDOM_RACE_FRAME_BASE);
        m_newGameWindow->BroadcastMessage(messageTemp);

        sprintf(gText, gAlignmentNames[m_setupPlayerRace[player]]);
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_RACE_NAME_FIRST, player);
        messageTemp.payload.widget.data.text = gText;
        m_newGameWindow->BroadcastMessage(messageTemp);
        if (playerLockedValue)
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        else
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        messageTemp.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);
    }

    gpGame->m_difficultyRating =
        static_cast<short>(CalcDifficultyRating());
    messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
    messageTemp.payload.widget.id = EncodeNewGameControl(NEW_GAME_RATING);
    sprintf(gText, "%s %d%%", "Rating", gpGame->m_difficultyRating);
    messageTemp.payload.widget.data.text = gText;
    m_newGameWindow->BroadcastMessage(messageTemp);
    DrawNGKPDisplayString(0);
}

// @early-stop
// After excluding retail switch data [0x21fe,0x2242) and
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
        windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        windowMessage.payload.widget.id = EncodeNewGameControl(NEW_GAME_SHADOW);
        windowMessage.payload.widget.data.value = NEW_GAME_SHADOW_FRAME;
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
                gpWindowManager->m_dialogResult = message.payload.widget.id;
                gpWindowManager->m_dialogResult = NEW_GAME_DIALOG_OK;
                message.type = NEW_GAME_MESSAGE_WIDGET;
                message.payload.widget.id = NEW_GAME_DIALOG_CLOSE_MESSAGE;
                message.payload.widget.command = message.payload.widget.id;
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

    if (message.payload.widget.parameter & NEW_GAME_MOUSE_RIGHT_FLAG) {
        if (message.payload.widget.command == NEW_GAME_EVENT_PRESS ||
            message.payload.widget.command == NEW_GAME_EVENT_ALTERNATE_PRESS) {
            helpDialogIndexLocal = -1;
            if ((message.payload.widget.id >= NEW_GAME_DIFFICULTY_HELP_FIRST &&
                message.payload.widget.id <= LastNewGameControl(
                    NEW_GAME_DIFFICULTY_HELP_FIRST, NEW_GAME_DIFFICULTY_COUNT)) ||
                (message.payload.widget.id >= NEW_GAME_DIFFICULTY_FIRST &&
                 message.payload.widget.id <= LastNewGameControl(
                    NEW_GAME_DIFFICULTY_FIRST, NEW_GAME_DIFFICULTY_COUNT)))
                helpDialogIndexLocal = NEW_GAME_HELP_DIFFICULTY;
            if ((message.payload.widget.id >= NEW_GAME_HANDICAP_FIRST &&
                message.payload.widget.id <= LastNewGameControl(
                    NEW_GAME_HANDICAP_FIRST, NEW_GAME_PLAYER_CONTROL_COUNT)) ||
                (message.payload.widget.id >= NEW_GAME_PLAYER_HUMAN_FIRST &&
                 message.payload.widget.id <= LastNewGameControl(
                    NEW_GAME_PLAYER_HUMAN_FIRST, NEW_GAME_PLAYER_CONTROL_COUNT)))
                helpDialogIndexLocal = NEW_GAME_HELP_HANDICAP;
            if ((message.payload.widget.id >= NEW_GAME_COLOR_FIRST &&
                message.payload.widget.id <= LastNewGameControl(
                    NEW_GAME_COLOR_FIRST, NEW_GAME_PLAYER_CONTROL_COUNT)) ||
                (message.payload.widget.id >= NEW_GAME_RACE_FIRST &&
                 message.payload.widget.id <= LastNewGameControl(
                    NEW_GAME_RACE_FIRST, NEW_GAME_PLAYER_CONTROL_COUNT)) ||
                (message.payload.widget.id >= NEW_GAME_PLAYER_SELECT_FIRST &&
                 message.payload.widget.id <= NEW_GAME_PLAYER_NAME_FIRST) ||
                (message.payload.widget.id >= NEW_GAME_PLAYER_NAME_FIRST &&
                 message.payload.widget.id <= NEW_GAME_RACE_ICON_FIRST))
                helpDialogIndexLocal = NEW_GAME_HELP_PLAYER;
            if ((message.payload.widget.id >= NEW_GAME_RACE_CYCLE_FIRST &&
                message.payload.widget.id <= LastNewGameControl(
                    NEW_GAME_RACE_CYCLE_FIRST, NEW_GAME_PLAYER_CONTROL_COUNT)) ||
                (message.payload.widget.id >= NEW_GAME_RACE_ICON_FIRST &&
                 message.payload.widget.id <= LastNewGameControl(
                    NEW_GAME_RACE_ICON_FIRST, NEW_GAME_PLAYER_CONTROL_COUNT)))
                helpDialogIndexLocal = NEW_GAME_HELP_RACE;
            if (message.payload.widget.id == NEW_GAME_MAP_OPTIONS_CONTROL ||
                DecodeNewGameControl(message.payload.widget.id) ==
                    NEW_GAME_MAP_SELECT ||
                DecodeNewGameControl(message.payload.widget.id) ==
                    NEW_GAME_SCENARIO_NAME)
                helpDialogIndexLocal = NEW_GAME_HELP_MAP;
            if (DecodeNewGameControl(message.payload.widget.id) == NEW_GAME_RATING)
                helpDialogIndexLocal = NEW_GAME_HELP_RATING;
            if (message.payload.widget.id == NEW_GAME_DIALOG_OK)
                helpDialogIndexLocal = NEW_GAME_HELP_OK;
            if (message.payload.widget.id == NEW_GAME_DIALOG_CANCEL)
                helpDialogIndexLocal = NEW_GAME_HELP_CANCEL;
            if (helpDialogIndexLocal != -1)
                NormalDialog(gNewGameHelp[helpDialogIndexLocal], 4,
                             -1, -1, -1, 0, -1, 0, -1, 0);
        }
        goto finish;
    }

    switch (message.payload.widget.command) {
    case NEW_GAME_EVENT_RELEASE:
        switch (message.payload.widget.id) {
        case NEW_GAME_MAP_OPTIONS_CONTROL:
            goto chooseMap;

        case NEW_GAME_DIALOG_OK:
            if (gbRemoteOn) {
                transmitResultTemp = TransmitRemoteData(
                    0, NEW_GAME_REMOTE_CHANNEL, 0,
                    NEW_GAME_REMOTE_START, 1, 1,
                    NEW_GAME_NETWORK_PLAYER_NONE);
            }
            gpWindowManager->m_dialogResult = message.payload.widget.id;
            message.payload.widget.id = NEW_GAME_DIALOG_CLOSE_MESSAGE;
            message.payload.widget.command = message.payload.widget.id;
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
            gpWindowManager->m_dialogResult = message.payload.widget.id;
            message.payload.widget.id = NEW_GAME_DIALOG_CLOSE_MESSAGE;
            message.payload.widget.command = message.payload.widget.id;
            gbNewGameDialogOver = 1;
            return 2;

        default:
            break;
        }
        break;

    case NEW_GAME_EVENT_PRESS:
        switch (message.payload.widget.id) {
    case NEW_GAME_DIFFICULTY_HELP_FIRST + 0:
    case NEW_GAME_DIFFICULTY_HELP_FIRST + 1:
    case NEW_GAME_DIFFICULTY_HELP_FIRST + 2:
    case NEW_GAME_DIFFICULTY_HELP_FIRST + 3:
    case NEW_GAME_DIFFICULTY_HELP_FIRST + 4:
        currentPlayerLocal =
            message.payload.widget.id - NEW_GAME_DIFFICULTY_HELP_FIRST;
        goto setDifficulty;

    case NEW_GAME_DIFFICULTY_FIRST:
    case NEW_GAME_DIFFICULTY_FIRST + 1:
    case NEW_GAME_DIFFICULTY_FIRST + 2:
    case NEW_GAME_DIFFICULTY_FIRST + 3:
    case NEW_GAME_DIFFICULTY_FIRST + 4:
        currentPlayerLocal = message.payload.widget.id - NEW_GAME_DIFFICULTY_FIRST;
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
        currentPlayerLocal = message.payload.widget.id - NEW_GAME_HANDICAP_FIRST;
        goto cycleHandicap;

    case NEW_GAME_PLAYER_HUMAN_FIRST + 0:
    case NEW_GAME_PLAYER_HUMAN_FIRST + 1:
    case NEW_GAME_PLAYER_HUMAN_FIRST + 2:
    case NEW_GAME_PLAYER_HUMAN_FIRST + 3:
    case NEW_GAME_PLAYER_HUMAN_FIRST + 4:
    case NEW_GAME_PLAYER_HUMAN_FIRST + 5:
        currentPlayerLocal = message.payload.widget.id - NEW_GAME_PLAYER_HUMAN_FIRST;
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
        currentPlayerLocal = message.payload.widget.id - NEW_GAME_RACE_FIRST;
        goto selectPlayer;

    case NEW_GAME_COLOR_FIRST + 0:
    case NEW_GAME_COLOR_FIRST + 1:
    case NEW_GAME_COLOR_FIRST + 2:
    case NEW_GAME_COLOR_FIRST + 3:
    case NEW_GAME_COLOR_FIRST + 4:
    case NEW_GAME_COLOR_FIRST + 5:
        currentPlayerLocal = message.payload.widget.id - NEW_GAME_COLOR_FIRST;
        goto selectPlayer;

    case NEW_GAME_PLAYER_SELECT_FIRST + 0:
    case NEW_GAME_PLAYER_SELECT_FIRST + 1:
    case NEW_GAME_PLAYER_SELECT_FIRST + 2:
    case NEW_GAME_PLAYER_SELECT_FIRST + 3:
    case NEW_GAME_PLAYER_SELECT_FIRST + 4:
    case NEW_GAME_PLAYER_SELECT_FIRST + 5:
        currentPlayerLocal = message.payload.widget.id - NEW_GAME_PLAYER_SELECT_FIRST;
        goto selectPlayer;

    case NEW_GAME_PLAYER_NAME_FIRST + 0:
    case NEW_GAME_PLAYER_NAME_FIRST + 1:
    case NEW_GAME_PLAYER_NAME_FIRST + 2:
    case NEW_GAME_PLAYER_NAME_FIRST + 3:
    case NEW_GAME_PLAYER_NAME_FIRST + 4:
    case NEW_GAME_PLAYER_NAME_FIRST + 5:
        currentPlayerLocal = message.payload.widget.id - NEW_GAME_PLAYER_NAME_FIRST;
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
        currentPlayerLocal = message.payload.widget.id - NEW_GAME_RACE_CYCLE_FIRST;
        goto cycleRace;

    case NEW_GAME_RACE_ICON_FIRST + 0:
    case NEW_GAME_RACE_ICON_FIRST + 1:
    case NEW_GAME_RACE_ICON_FIRST + 2:
    case NEW_GAME_RACE_ICON_FIRST + 3:
    case NEW_GAME_RACE_ICON_FIRST + 4:
    case NEW_GAME_RACE_ICON_FIRST + 5:
        currentPlayerLocal = message.payload.widget.id - NEW_GAME_RACE_ICON_FIRST;
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
            mapWindowMessageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
            mapWindowMessageTemp.payload.widget.id = NEW_GAME_DIALOG_CANCEL;
            mapWindowMessageTemp.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
            gpGame->m_newGameWindow->BroadcastMessage(mapWindowMessageTemp);
            gpGame->GetMap();
            mapWindowMessageTemp.type = NEW_GAME_MESSAGE_WIDGET;
            mapWindowMessageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
            mapWindowMessageTemp.payload.widget.id = NEW_GAME_DIALOG_CANCEL;
            mapWindowMessageTemp.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
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

// @early-stop
// Excluding jump-table data at retail +0x259..+0x289 and +0x399..+0x400,
// every instruction byte agrees. Both objects have 84 relocation sites and every
// external target agrees; residuals are delinked gText+1/local-table identities.
VA(0x004b9db8, 0x418)
int game::ProcessNGKeyPress(struct tag_message &message)
{
    char workText[NEW_GAME_KEY_BUFFER_SIZE];
    char keyChar;
    int scanCode;
    int widthResult;

    if (giNumHumanPlayers == 1 ||
        iMPBaseType == NEW_GAME_MULTIPLAYER_HOTSEAT)
        return 0;

    switch (message.payload.keyboard.keyCode) {
    case NEW_GAME_KEY_ESCAPE:
        if (!gbAllowTextEntryEscape)
            break;
        strcpy(cNGKPCore, "");
        break;

    case NEW_GAME_KEY_DELETE:
        if (strlen(cNGKPCore) > NGKPcursorIndex) {
            strcpy(gText, cNGKPCore + (NGKPcursorIndex + 1));
            strcpy(cNGKPCore + NGKPcursorIndex, gText);
        }
        break;

    case NEW_GAME_KEY_LEFT:
        if (NGKPcursorIndex > 0)
            --NGKPcursorIndex;
        break;

    case NEW_GAME_KEY_RIGHT:
        if (strlen(cNGKPCore) > NGKPcursorIndex)
            ++NGKPcursorIndex;
        break;

    default:
        gpInputManager->AsciiConvert(message);
        if (message.payload.keyboard.keyCode == NEW_GAME_KEY_ENTER)
            return 1;

        if (message.payload.keyboard.keyCode == NEW_GAME_KEY_BACKSPACE) {
            if (NGKPcursorIndex > 0) {
                strcpy(gText, cNGKPCore + NGKPcursorIndex);
                strcpy(cNGKPCore + (NGKPcursorIndex - 1), gText);
                --NGKPcursorIndex;
            }
        } else if (strlen(cNGKPCore) + 1 < NEW_GAME_CHAT_TEXT_LIMIT &&
                   message.payload.keyboard.keyCode != 0) {
            strcpy(workText, cNGKPCore);
            keyChar = 0;
            if (message.payload.keyboard.keyCode >= NEW_GAME_KEY_FIRST_EXTENDED) {
                scanCode = static_cast<unsigned char>(
                    static_cast<unsigned int>(message.payload.keyboard.keyCode) >>
                    8);
                switch (scanCode) {
                case NEW_GAME_KEYPAD_INSERT:
                    keyChar = '0';
                    break;
                case NEW_GAME_KEYPAD_END:
                    keyChar = '1';
                    break;
                case NEW_GAME_KEYPAD_DOWN:
                    keyChar = '2';
                    break;
                case NEW_GAME_KEYPAD_PAGE_DOWN:
                    keyChar = '3';
                    break;
                case NEW_GAME_KEYPAD_LEFT:
                    keyChar = '4';
                    break;
                case NEW_GAME_KEYPAD_CENTER:
                    keyChar = '5';
                    break;
                case NEW_GAME_KEYPAD_RIGHT:
                    keyChar = '6';
                    break;
                case NEW_GAME_KEYPAD_HOME:
                    keyChar = '7';
                    break;
                case NEW_GAME_KEYPAD_UP:
                    keyChar = '8';
                    break;
                case NEW_GAME_KEYPAD_PAGE_UP:
                    keyChar = '9';
                    break;
                }
            } else {
                keyChar = static_cast<char>(message.payload.keyboard.keyCode);
            }

            if (keyChar == '{' || keyChar == '}')
                keyChar = 0;

            if (keyChar != 0) {
                strcpy(gText, cNGKPCore);
                gText[NGKPcursorIndex] = keyChar;
                gText[NGKPcursorIndex + 1] = 0;
                strcat(gText, cNGKPCore + NGKPcursorIndex);
                strcpy(cNGKPCore, gText);
                ++NGKPcursorIndex;
                NGKPSetupDisplayString(cNGKPCore,
                                       static_cast<unsigned short>(NGKPcursorIndex));
                widthResult = smallFont->LineLength(
                    cNGKPDisplay, NEW_GAME_CHAT_DRAW_WIDTH);
                if (widthResult > NEW_GAME_CHAT_MAX_LINES) {
                    strcpy(cNGKPCore, workText);
                    --NGKPcursorIndex;
                }
            }
        }
        break;
    }

    DrawNGKPDisplayString(1);
    return 0;
}

VA(0x004ba1d0, 0x11e)
void game::NGKPSetupDisplayString(char *text, unsigned short cursor)
{
    if (giNumHumanPlayers == 1 ||
        iMPBaseType == NEW_GAME_MULTIPLAYER_HOTSEAT)
        return;

    if (glTimers[0] < static_cast<int>(KBTickCount())) {
        NGKPcursorFlashOn = 1 - NGKPcursorFlashOn;
        glTimers[0] = KBTickCount() + NEW_GAME_CURSOR_FLASH_TICKS;
    }

    if (cursor > 0)
        strncpy(cNGKPDisplay, text, cursor);

    if (NGKPcursorFlashOn)
        cNGKPDisplay[cursor] = 0x1f;
    else
        cNGKPDisplay[cursor] = '_';

    if (cursor < strlen(text))
        strcpy(cNGKPDisplay + (cursor + 1), text + cursor);
    else
        cNGKPDisplay[cursor + 1] = 0;
}

// @early-stop
// The complete 55-instruction stream is identical with relocations masked and
// all 10 relocation sites and targets agree; the residual is delinker identity.
VA(0x004ba2ee, 0xae)
void game::DrawNGKPDisplayString(int updateScreen)
{
    if (gbNewGameDialogOver != 0) {
    } else if (giNumHumanPlayers != 1) {
        if (iMPBaseType == NEW_GAME_MULTIPLAYER_HOTSEAT) {
        } else {
            NGKPBkg->DrawToBuffer(NEW_GAME_CHAT_DRAW_X, NEW_GAME_CHAT_DRAW_Y,
                                  NEW_GAME_CHAT_BACKGROUND_WIDTH, 0);
            smallFont->DrawBoundedString(
                cNGKPDisplay, NEW_GAME_CHAT_DRAW_X, NEW_GAME_CHAT_DRAW_Y,
                NEW_GAME_CHAT_DRAW_WIDTH, NEW_GAME_CHAT_DRAW_HEIGHT, 2, 0);
            if (updateScreen)
                gpWindowManager->UpdateScreenRegion(
                    NEW_GAME_CHAT_DRAW_X, NEW_GAME_CHAT_DRAW_Y,
                    NEW_GAME_CHAT_DRAW_WIDTH, NEW_GAME_CHAT_DRAW_HEIGHT);
        }
    }
}

// @early-stop
// Instruction bytes, frame slots, and all 118 relocation sites and targets
// agree. Residual rows are string and local static-constant symbol identities.
VA(0x004ba39c, 0xb71)
void game::ShowScenInfo(void)
{
    int availableWidthResult;
    int mapSizeIndex;
    widget *textControlLocal;
    int firstPlayerXLocal;
    int multiplayerYOffsetValue;
    int playerCounter;
    int playerLockedLocal;
    int playerSpacingTemp;
    tag_message scenarioMessageTemp;
    widget *iconControlLocal;
    heroWindow *scenarioWindowValue;
    int playerGapValue;
    char *label;
    int raceTextWidth;
    int singlePlayerYOffsetValue;

    gpMouseManager->SetPointer("advmice.mse", 0,
                               NEW_GAME_POINTER_DEFAULT_FRAME);
    scenarioWindowValue = new heroWindow(90, 4, "sceninfo.bin");
    if (scenarioWindowValue == 0)
        MemError();
    SetWinText(scenarioWindowValue, NEW_GAME_SCENARIO_WINDOW_TEXT_ID);

    scenarioMessageTemp.type = NEW_GAME_MESSAGE_WIDGET;
    scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
    scenarioMessageTemp.payload.widget.id =
        EncodeNewGameControl(NEW_GAME_SCENARIO_NAME);
    scenarioMessageTemp.payload.widget.data.text = m_mapHeader.name;
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

    scenarioMessageTemp.payload.widget.id = NEW_GAME_SCENARIO_DIFFICULTY;
    scenarioMessageTemp.payload.widget.data.text = cDifficulty[m_mapHeader.difficulty];
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);
    scenarioMessageTemp.payload.widget.id = NEW_GAME_SCENARIO_SELECTED_DIFFICULTY;
    scenarioMessageTemp.payload.widget.data.text = cDifficulty[m_difficulty];
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

    sprintf(gText, "%d", CalcDifficultyRating());
    strcat(gText, "%");
    scenarioMessageTemp.payload.widget.id = NEW_GAME_SCENARIO_RATING;
    scenarioMessageTemp.payload.widget.data.text = gText;
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

    mapSizeIndex = 0;
    if (m_mapHeader.width == MAP_DIMENSION_MEDIUM)
        mapSizeIndex = 1;
    else if (m_mapHeader.width == MAP_DIMENSION_LARGE)
        mapSizeIndex = 2;
    else if (m_mapHeader.width == MAP_DIMENSION_XLARGE)
        mapSizeIndex = 3;
    scenarioMessageTemp.payload.widget.id = NEW_GAME_SCENARIO_MAP_SIZE;
    scenarioMessageTemp.payload.widget.data.text = cMapSize[mapSizeIndex];
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

    scenarioMessageTemp.payload.widget.id = NEW_GAME_SCENARIO_DESCRIPTION;
    scenarioMessageTemp.payload.widget.data.text = m_mapHeader.description;
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);
    GetVictoryConditionText(gText);
    scenarioMessageTemp.payload.widget.id = NEW_GAME_SCENARIO_VICTORY;
    scenarioMessageTemp.payload.widget.data.text = gText;
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);
    GetLossConditionText(gText);
    scenarioMessageTemp.payload.widget.id = NEW_GAME_SCENARIO_LOSS;
    scenarioMessageTemp.payload.widget.data.text = gText;
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

    iconControlLocal = 0;
    textControlLocal = 0;
    availableWidthResult = 372 - m_mapHeader.playerCount * 62;
    playerGapValue = availableWidthResult / (m_mapHeader.playerCount + 1);
    firstPlayerXLocal = playerGapValue + 24;
    playerSpacingTemp = playerGapValue + 62;
    multiplayerYOffsetValue = 0;

    for (playerCounter = 0; playerCounter < m_mapHeader.playerCount; ++playerCounter) {
        if (giNumHumanPlayers > 1) {
            iconControlLocal = new iconWidget(
                static_cast<short>(playerSpacingTemp * playerCounter +
                                   firstPlayerXLocal + 13),
                static_cast<short>(multiplayerYOffsetValue + 309), 64, 28,
                "ngextra.icn", 59, 0,
                static_cast<short>(playerCounter + NEW_GAME_PLAYER_HUMAN_FIRST), 16,
                1);
            if (iconControlLocal == 0)
                MemError();
            scenarioWindowValue->AddWidget(iconControlLocal, -1);

            iconControlLocal = new iconWidget(
                static_cast<short>(playerSpacingTemp * playerCounter +
                                   firstPlayerXLocal + 16),
                static_cast<short>(multiplayerYOffsetValue + 306), 62, 26,
                "ngextra.icn", 0, 0,
                static_cast<short>(playerCounter + NEW_GAME_HANDICAP_FIRST), 16, 1);
            if (iconControlLocal == 0)
                MemError();
            scenarioWindowValue->AddWidget(iconControlLocal, -1);
        }

        iconControlLocal = new iconWidget(
            static_cast<short>(playerSpacingTemp * playerCounter + firstPlayerXLocal +
                               11),
            163, 68,
            static_cast<short>(((giNumHumanPlayers <= 1) - 1 & 0x11) + 0x2f),
            "ngextra.icn",
            static_cast<short>(((giNumHumanPlayers <= 1) - 1 & -0x13) + 0x4f),
            0, static_cast<short>(playerCounter + NEW_GAME_RACE_FIRST), 16, 1);
        if (iconControlLocal == 0)
            MemError();
        scenarioWindowValue->AddWidget(iconControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<short>(playerSpacingTemp * playerCounter + firstPlayerXLocal +
                               16),
            160, 62, 58, "ngextra.icn",
            static_cast<short>(((giNumHumanPlayers <= 1) - 1 & 0x18) + 3), 0,
            static_cast<short>(playerCounter + NEW_GAME_COLOR_FIRST), 16, 1);
        if (iconControlLocal == 0)
            MemError();
        scenarioWindowValue->AddWidget(iconControlLocal, -1);

        if (giNumHumanPlayers > 1) {
            label = static_cast<char *>(BaseAlloc(
                2, "I:\\Projects\\Heroes\\Prog\\SOURCE\\Newgame.cpp",
                scenarioInfoSourceLineBase + 0x72));
            sprintf(label, " ");
            textControlLocal = new textWidget(
                static_cast<short>(playerSpacingTemp * playerCounter +
                                   firstPlayerXLocal + 19),
                206, 56, 9, label, "smalfont.fnt", 1,
                static_cast<short>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
                NEW_GAME_MESSAGE_WIDGET, 1);
            if (textControlLocal == 0)
                MemError();
            scenarioWindowValue->AddWidget(textControlLocal, -1);
        }

        singlePlayerYOffsetValue = 0;
        iconControlLocal = new iconWidget(
            static_cast<short>(playerSpacingTemp * playerCounter + firstPlayerXLocal +
                               11),
            static_cast<short>(multiplayerYOffsetValue +
                               singlePlayerYOffsetValue + 243),
            64, 47,
            "ngextra.icn", 61, 0,
            static_cast<short>(playerCounter + NEW_GAME_RACE_ICON_FIRST), 16, 1);
        if (iconControlLocal == 0)
            MemError();
        scenarioWindowValue->AddWidget(iconControlLocal, -1);

        label = static_cast<char *>(BaseAlloc(
            2, "I:\\Projects\\Heroes\\Prog\\SOURCE\\Newgame.cpp",
            scenarioInfoSourceLineBase + 0x92));
        sprintf(label, "A");
        if (m_mapHeader.playerCount >= 5) {
            if (m_mapHeader.playerCount >= MAP_HEADER_PLAYER_COUNT)
                raceTextWidth = 0;
            else
                raceTextWidth = 16;
        } else {
            raceTextWidth = 26;
        }
        textControlLocal = new textWidget(
            static_cast<short>(playerSpacingTemp * playerCounter + firstPlayerXLocal +
                               15 - raceTextWidth / 2),
            static_cast<short>(multiplayerYOffsetValue +
                               singlePlayerYOffsetValue + 288),
            static_cast<short>(raceTextWidth + 64), 12, label,
            "smalfont.fnt", 1,
            static_cast<short>(playerCounter + NEW_GAME_RACE_NAME_FIRST),
            NEW_GAME_MESSAGE_WIDGET, 1);
        if (textControlLocal == 0)
            MemError();
        scenarioWindowValue->AddWidget(textControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<short>(playerSpacingTemp * playerCounter + firstPlayerXLocal +
                               16),
            static_cast<short>(multiplayerYOffsetValue +
                               singlePlayerYOffsetValue + 240),
            62, 45,
            "ngextra.icn", 51, 0,
            static_cast<short>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST), 16, 1);
        if (iconControlLocal == 0)
            MemError();
        scenarioWindowValue->AddWidget(iconControlLocal, -1);
    }

    for (playerCounter = 0; playerCounter < m_mapHeader.playerCount; ++playerCounter) {
        if (m_setupPlayerNetworkId[playerCounter] == NEW_GAME_COMPUTER_PLAYER) {
            sprintf(gText, "");
        } else if (strlen(cPlayerNames[m_setupPlayerNetworkId[playerCounter]]) != 0) {
            sprintf(gText, cPlayerNames[m_setupPlayerNetworkId[playerCounter]]);
        } else {
            sprintf(gText, "Player %d",
                    m_setupPlayerNetworkId[playerCounter] + 1);
        }
        scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
        scenarioMessageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_PLAYER_NAME_FIRST, playerCounter);
        scenarioMessageTemp.payload.widget.data.text = gText;
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

        if (m_selectedSetupPlayer == playerCounter)
            scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        else
            scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        scenarioMessageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_PLAYER_SELECT_FIRST, playerCounter);
        scenarioMessageTemp.payload.widget.data.value = NEW_GAME_WIDGET_REFRESH_FRAME;
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

        if (m_setupPlayerType[playerCounter] != NEW_GAME_PLAYER_DEFAULT ||
            (giNumHumanPlayers > 1 &&
             m_setupPlayerNetworkId[playerCounter] != NEW_GAME_COMPUTER_PLAYER))
            playerLockedLocal = 0;
        else
            playerLockedLocal = 1;
        scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        scenarioMessageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_COLOR_FIRST, playerCounter);
        if (m_setupPlayerNetworkId[playerCounter] == NEW_GAME_COMPUTER_PLAYER)
            scenarioMessageTemp.payload.widget.data.value = m_setupPlayerColor[playerCounter] +
                (playerLockedLocal ? NEW_GAME_COMPUTER_COLOR_LOCKED_FRAME
                                   : NEW_GAME_COMPUTER_COLOR_UNLOCKED_FRAME);
        else
            scenarioMessageTemp.payload.widget.data.value = m_setupPlayerColor[playerCounter] +
                (playerLockedLocal ? NEW_GAME_HUMAN_COLOR_LOCKED_FRAME
                                   : NEW_GAME_HUMAN_COLOR_UNLOCKED_FRAME);
        if (giNumHumanPlayers > 1)
            scenarioMessageTemp.payload.widget.data.value += NEW_GAME_MULTIPLAYER_COLOR_FRAME_OFFSET;
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

        if (playerLockedLocal)
            scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        else
            scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        scenarioMessageTemp.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

        scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        scenarioMessageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_HANDICAP_FIRST, playerCounter);
        if (m_setupPlayerNetworkId[playerCounter] == NEW_GAME_COMPUTER_PLAYER)
            scenarioMessageTemp.payload.widget.data.value = NEW_GAME_RACE_NAME_FIRST;
        else
            scenarioMessageTemp.payload.widget.data.value = m_playerHandicap[playerCounter];
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);
        if (m_setupPlayerNetworkId[playerCounter] == NEW_GAME_COMPUTER_PLAYER)
            scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        else
            scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        scenarioMessageTemp.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

        scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        scenarioMessageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_RACE_CYCLE_FIRST, playerCounter);
        scenarioMessageTemp.payload.widget.data.value = m_setupPlayerRace[playerCounter] +
            (playerLockedLocal ? NEW_GAME_FIXED_RACE_FRAME_BASE
                               : NEW_GAME_RANDOM_RACE_FRAME_BASE);
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

        sprintf(gText, gAlignmentNames[m_setupPlayerRace[playerCounter]]);
        scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
        scenarioMessageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_RACE_NAME_FIRST, playerCounter);
        scenarioMessageTemp.payload.widget.data.text = gText;
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);
    }

    gpWindowManager->DoDialog(scenarioWindowValue, EventWindowHandler, 0);
    delete scenarioWindowValue;
}

// @early-stop
// The instruction stream is identical with relocations masked and all 14
// relocation sites and targets agree; only string symbol identities differ.
VA(0x004baf0d, 0x1c7)
void game::GetLossConditionText(char *text)
{
    int week;
    hero *lossHero;
    int day;
    int month;
    town *lossTown;
    int townId;

    if (m_mapHeader.lossCondition != MAP_LOSS_STANDARD) {
        switch (m_mapHeader.lossCondition) {
        case MAP_LOSS_TOWN:
            townId = GetTownId(m_mapHeader.lossConditionValue,
                               m_mapHeader.lossTownY);
            lossTown = GetTown(townId);
            sprintf(text, "Lose the %s '%s'.",
                    (lossTown->m_buildings & TOWN_BUILDING_CASTLE)
                        ? "castle"
                        : "town",
                    lossTown->m_name);
            break;

        case MAP_LOSS_HERO:
            lossHero = GetHero(m_mapHeader.lossConditionValue);
            sprintf(text, "Lose the hero '%s'.", lossHero->m_name);
            break;

        case MAP_LOSS_TIME:
            month = (gpGame->m_mapHeader.lossConditionValue - 1) /
                        NEW_GAME_DAYS_PER_MONTH +
                    1;
            week = (gpGame->m_mapHeader.lossConditionValue -
                    (month - 1) * NEW_GAME_DAYS_PER_MONTH - 1) /
                       NEW_GAME_DAYS_PER_WEEK +
                   1;
            day = (gpGame->m_mapHeader.lossConditionValue - 1) %
                      NEW_GAME_DAYS_PER_WEEK +
                  1;
            sprintf(text,
                    "Fail to win by the end of month %d, week %d, day %d.",
                    month, week, day);
            break;
        }
    } else {
        sprintf(text, "Lose all your heroes, towns and castles.");
    }
}

// @early-stop
// Excluding the 0x14-byte jump table, opcodes and CFG agree after normalizing a
// uniform four-byte stack displacement. Retail reserves one unreferenced frame
// word and one trailing alignment NOP; its const_000fe2ac relocation is the
// linked equivalent of gArtifactNames-1. Both objects have 32 relocations and
// every external target agrees.
VA(0x004bb0d4, 0x2fb)
void game::GetVictoryConditionText(char *text)
{
    hero *victoryHeroData;
    int firstSideIsLocalResult;
    char firstSide[NEW_GAME_SIDE_TEXT_SIZE];
    char secondSideValue[NEW_GAME_SIDE_TEXT_SIZE];
    town *victoryTown;
    int townId;

    if (m_mapHeader.victoryCondition != MAP_VICTORY_DEFEAT_ALL) {
        switch (m_mapHeader.victoryCondition) {
        case MAP_VICTORY_CAPTURE_TOWN:
            townId = GetTownId(m_mapHeader.victoryConditionValue,
                               m_mapHeader.victoryTownY);
            victoryTown = GetTown(townId);
            sprintf(text, "Capture the %s '%s'",
                    (victoryTown->m_buildings & TOWN_BUILDING_CASTLE)
                        ? "castle"
                        : "town",
                    victoryTown->m_name);
            break;

        case MAP_VICTORY_DEFEAT_HERO:
            victoryHeroData = GetHero(m_mapHeader.victoryConditionValue);
            sprintf(text, "Defeat the hero '%s'", victoryHeroData->m_name);
            break;

        case MAP_VICTORY_FIND_ARTIFACT:
            if (m_mapHeader.victoryConditionValue == 0)
                sprintf(text, "Find the ultimate artifact");
            else
                sprintf(text, "Find the %s",
                        gArtifactNames[m_mapHeader.victoryConditionValue - 1]);
            break;

        case MAP_VICTORY_ACCUMULATE_GOLD:
            sprintf(text, "Accumulate %d gold",
                    m_mapHeader.victoryConditionValue *
                        NEW_GAME_GOLD_CONDITION_MULTIPLIER);
            break;

        case MAP_VICTORY_DEFEAT_SIDE:
            firstSideIsLocalResult = GetSideDesc(
                firstSide, 0, m_mapHeader.victoryConditionValue - 1);
            GetSideDesc(secondSideValue, m_mapHeader.victoryConditionValue,
                        m_mapHeader.playerCount - 1);
            if (firstSideIsLocalResult)
                sprintf(text, "%s must defeat %s", firstSide, secondSideValue);
            else
                sprintf(text, "%s must defeat %s", secondSideValue, firstSide);
        }

        if (m_mapHeader.victoryCondition != MAP_VICTORY_DEFEAT_SIDE &&
            m_mapHeader.allowNormalVictory != 0)
            strcat(text,
                   ", or you may win by defeating all enemy heroes and capturing all enemy towns and castles.");
        else
            strcat(text, ".");
    } else {
        strcpy(text,
               "Defeat all enemy heroes and capture all enemy towns and castles.");
    }
}

// @early-stop
// At this source hash's retained max, the instruction stream differs only in
// string-symbol identities. The current TU-cumulative output also reverses the
// operands of equivalent signed bounds checks; frame roles and 25 relocations agree.
VA(0x004bb3cf, 0x2a7)
int game::GetSideDesc(char *text, int firstPlayer, int lastPlayer)
{
    char colorName[NEW_GAME_SIDE_TEXT_SIZE];
    int sideSize;
    int localPlayerOnSide;
    int localPlayerIndex;
    int player;
    int listedPlayerCount;
    int otherPlayerCount;

    localPlayerIndex = -1;
    for (player = 0; player < m_mapHeader.playerCount; ++player) {
        if (m_setupPlayerNetworkId[player] == giThisGamePos)
            localPlayerIndex = player;
    }

    if (localPlayerIndex >= firstPlayer && localPlayerIndex <= lastPlayer)
        localPlayerOnSide = 1;
    else
        localPlayerOnSide = 0;

    sideSize = lastPlayer - firstPlayer + 1;
    otherPlayerCount = sideSize - (localPlayerOnSide != 0);

    if (localPlayerOnSide) {
        if (otherPlayerCount != 0) {
            if (otherPlayerCount > 1)
                sprintf(text, "You and your allies ");
            else
                sprintf(text, "You and your ally ");

            listedPlayerCount = 0;
            for (player = firstPlayer; player <= lastPlayer; ++player) {
                if (player != localPlayerIndex) {
                    ++listedPlayerCount;
                    sprintf(colorName, gColors[m_setupPlayerColor[player]]);
                    colorName[0] -= 'a' - 'A';
                    strcat(text, colorName);
                    if (listedPlayerCount < otherPlayerCount - 1)
                        strcat(text, ", ");
                    else if (listedPlayerCount < otherPlayerCount)
                        strcat(text, " and ");
                }
            }
        } else {
            sprintf(text, "You");
        }
    } else {
        if (sideSize > 1)
            strcpy(text, "the enemy alliance of ");
        else
            strcpy(text, "the enemy - ");

        listedPlayerCount = 0;
        for (player = firstPlayer; player <= lastPlayer; ++player) {
            ++listedPlayerCount;
            sprintf(colorName, gColors[m_setupPlayerColor[player]]);
            colorName[0] -= 'a' - 'A';
            strcat(text, colorName);
            if (listedPlayerCount < otherPlayerCount - 1)
                strcat(text, ", ");
            else if (listedPlayerCount < otherPlayerCount)
                strcat(text, " and ");
        }
    }

    return localPlayerOnSide;
}

// ---- globals (definitions, RVA order) ----
DATA(0x0051cd20) int gbNewGameDialogOver = 1;
DATA(0x0051cd24) int NGKPcursorFlashOn = 1;
DATA(0x00533150) char *cNGKPDisplay;
DATA(0x00533154) int gbNewGameShadowHidden;
DATA(0x00533158) char *cNGKPCore;
DATA(0x0053315c) int NGKPcursorIndex;
DATA(0x00533160) char *cTextReceivedBuffer[3];
DATA(0x0053316c) class icon *NGKPBkg;
