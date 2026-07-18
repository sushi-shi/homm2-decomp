// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Newgame.obj   from: (directly linked into exe)
// functions: 16   data: 8
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <string.h>
#include <BASE/message.h>
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

// __FILE__ for the NWC memory/assert tracking (reloc-masked path string).
#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\Newgame.cpp"

// @early-stop
// @early-stop-reloc-only: Current Newgame.cpp/header epoch: all 0x1d5 bytes
// match after masking 37 ordered relocation sites. Effective targets agree;
// residual rows only rename compiler-local strings and _strcmpi/__strcmpi.
VA(0x004b6f40, 0x1d5)
void game::GetMap(void) {
    fileRequester* requesterResult;
    i32 resultCode;
    char fileMask[16];
    char savedName[44];

    strcpy(savedName, gMapName);
    strcpy(gcCurMapName, "");
    if (gbRemoteOn && xNetHasOldPlayers) {
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
        sprintf(fileMask, "*.%s", "MP2");
    } else if (xIsExpansionMap) {
        sprintf(fileMask, "*.%s", "MX2");
    } else {
        sprintf(fileMask, "*.%s", "MP2");
    }

    requesterResult =
        new fileRequester(212, 9, FILE_REQUESTER_MAP_GAME, fileMask, gcMapPath, fileMask);
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
void game::ProcessNewMap(struct SMapHeader* header) {
    m_newGameInitialized = 0;
    m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
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
void game::InitNewGame(struct SMapHeader* header) {
    i32 activePlayerCount;
    i32 unusedPlayerCount;
    i32 player;
    i32 computerPlayers;
    i32 humanPlayers;
    i32 flexiblePlayerType;

    activePlayerCount = 0;
    unusedPlayerCount = 0;
    humanPlayers = 0;
    computerPlayers = 0;

    if (m_newGameInitialized && m_newGameHumanCount == giNumHumanPlayers) {
        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_setupPlayerNetworkId[player] == NEW_GAME_COMPUTER_PLAYER
                || m_setupPlayerNetworkId[player] >= giNumHumanPlayers) {
                m_setupPlayerRace[player] = m_mapHeader.playerRace[m_setupPlayerColor[player]];
            }
        }
    } else {
        m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
        if (header != 0)
            m_mapHeader = *header;
        else
            GetMapHeader(m_mapFilename, &m_mapHeader);

        for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
            if (m_mapHeader.playerEnabled[player]) {
                m_setupPlayerColor[activePlayerCount] = static_cast<i8>(player);
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
                m_setupPlayerRace[player] = m_mapHeader.playerRace[m_setupPlayerColor[player]];
                m_setupPlayerNetworkId[player] = NEW_GAME_NETWORK_PLAYER_NONE;
                m_setupPlayerType[player] = NEW_GAME_NETWORK_PLAYER_NONE;
            }
        }

        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]
                && !m_mapHeader.playerCanComputer[m_setupPlayerColor[player]]) {
                m_setupPlayerType[player] = NEW_GAME_PLAYER_DEFAULT;
                m_setupPlayerNetworkId[player] = static_cast<i8>(humanPlayers);
                ++humanPlayers;
            } else if (!m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]
                       && m_mapHeader.playerCanComputer[m_setupPlayerColor[player]]) {
                m_setupPlayerNetworkId[player] = NEW_GAME_COMPUTER_PLAYER;
                m_setupPlayerType[player] = NEW_GAME_PLAYER_DEFAULT;
                ++computerPlayers;
            }
        }

        if (humanPlayers < giNumHumanPlayers
            && computerPlayers < m_mapHeader.playerCount - giNumHumanPlayers)
            flexiblePlayerType = NEW_GAME_PLAYER_FLEXIBLE;
        else
            flexiblePlayerType = NEW_GAME_PLAYER_DEFAULT;

        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_setupPlayerType[player] == NEW_GAME_NETWORK_PLAYER_NONE)
                m_setupPlayerType[player] = static_cast<i8>(flexiblePlayerType);
        }

        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_setupPlayerNetworkId[player] != NEW_GAME_NETWORK_PLAYER_NONE)
                continue;
            if (humanPlayers < giNumHumanPlayers
                && m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]) {
                m_setupPlayerNetworkId[player] = static_cast<i8>(humanPlayers);
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
void game::SetupNetPlayerNames(void) {
    i32 player;

    if (giNumHumanPlayers > 1) {
        for (player = 0; OD_STEER(giNumHumanPlayers) > player; ++player) {
            if (iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT)
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
i32 game::NewGame(void) {
    DATA(0x0051cdd0) static i16 newGameSourceLineBase = 319;
    char netPlayerPacket[NEW_GAME_PLAYER_INFO_BUFFER_SIZE];
    char mapInfoPacket[NEW_GAME_MAP_PACKET_SIZE];
    tag_message windowMessage;
    i32 mapHeaderLoaded;
    i32 playerDataReceived;
    NewGameRemotePacket* remoteBuffer;
    heroWindow* choiceWindow;
    char* extension;
    i32 textBufferIndex;
    i32 mapHeaderResult;
    i32 result;
    i8 wrongExpansionType;
    i32 transmitResult;

    result = 1;
    m_newGameWindow = 0;

    if ((!gbRemoteOn || giThisNetPos == 0) && (!gbRemoteOn || !xNetHasOldPlayers)) {
        choiceWindow = new heroWindow(405, 8, "x_mapmnu.bin");
        if (choiceWindow == 0)
            MemError();
        gpWindowManager->DoDialog(choiceWindow, ExpStdGameHandler, 0);
        delete choiceWindow;
        switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
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
    for (textBufferIndex = 0; textBufferIndex < NEW_GAME_TEXT_BUFFER_COUNT; ++textBufferIndex) {
        cTextReceivedBuffer[textBufferIndex] =
            static_cast<char*>(H2_ALLOC(NEW_GAME_TEXT_BUFFER_SIZE, 364));
        strcpy(cTextReceivedBuffer[textBufferIndex], "");
    }
    cNGKPCore = static_cast<char*>(H2_ALLOC(NEW_GAME_KEY_BUFFER_SIZE, 367));
    cNGKPDisplay = static_cast<char*>(H2_ALLOC(NEW_GAME_KEY_BUFFER_SIZE, 368));
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
                    remoteBuffer = reinterpret_cast<NewGameRemotePacket*>(GetRemoteData(1));
                } while (remoteBuffer == 0);
            } while (remoteBuffer->type != NEW_GAME_REMOTE_PACKET_TYPE);

            switch (remoteBuffer->command) {
                case NEW_GAME_REMOTE_MAP_HEADER:
                    memset(&m_mapHeader, 0, sizeof(m_mapHeader));
                    memcpy(&m_mapHeader, remoteBuffer->payload, NEW_GAME_MAP_PACKET_SIZE);
                    mapHeaderLoaded = 1;
                    break;
                case NEW_GAME_REMOTE_PLAYER_INFO:
                    memcpy(
                        gsNetPlayerInfo,
                        remoteBuffer->payload,
                        NEW_GAME_PLAYER_INFO_PACKET_SIZE
                    );
                    SetupNetPlayerNames();
                    playerDataReceived = 1;
                    break;
            }
        } while (!playerDataReceived || !mapHeaderLoaded);

        m_newGameWindow = new heroWindow(190, 4, "ngmp.bin");
        InitNewGame(&m_mapHeader);
        InitNewGameWindow();
        UpdateNewGameWindow();

        windowMessage.type = MESSAGE_WIDGET;
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

        gbNewGameDialogOver = false;
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
                m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
            }
            if (giNumHumanPlayers > 3 && _strcmpi(gpGame->m_mapFilename, "brokena.mp2") == 0)
                strcpy(gpGame->m_mapFilename, "slugfest.mp2");
            if (giNumHumanPlayers > 1 && _strcmpi(gpGame->m_mapFilename, "arrax.mx2") == 0)
                strcpy(gpGame->m_mapFilename, "fullhse.mx2");

            strcpy(gMapName, m_mapFilename);
            mapHeaderResult = GetMapHeader(m_mapFilename, &m_mapHeader);
            if (!mapHeaderResult || m_mapHeader.minHumanPlayers > giNumHumanPlayers
                || m_mapHeader.maxHumanPlayers < giNumHumanPlayers)
                gpGame->GetMap();
            else
                break;
        }

        if (gbRemoteOn) {
            memcpy(mapInfoPacket, &gpGame->m_mapHeader, NEW_GAME_MAP_PACKET_SIZE);
            transmitResult = TransmitRemoteData(
                mapInfoPacket,
                NEW_GAME_REMOTE_CHANNEL,
                NEW_GAME_MAP_PACKET_SIZE,
                NEW_GAME_REMOTE_MAP_HEADER,
                1,
                1,
                NEW_GAME_NETWORK_PLAYER_NONE
            );
            if (!transmitResult)
                ShutDown(0);
            memcpy(netPlayerPacket, gsNetPlayerInfo, NEW_GAME_PLAYER_INFO_PACKET_SIZE);
            transmitResult = TransmitRemoteData(
                netPlayerPacket,
                NEW_GAME_REMOTE_CHANNEL,
                NEW_GAME_PLAYER_INFO_PACKET_SIZE,
                NEW_GAME_REMOTE_PLAYER_INFO,
                1,
                1,
                NEW_GAME_NETWORK_PLAYER_NONE
            );
            if (!transmitResult)
                ShutDown(0);
        }

        LoadGame("origdata.bin", 1, 0);
        if (giNumHumanPlayers > 1) {
            if (iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
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
        gbNewGameShadowHidden = false;
        gbNewGameDialogOver = false;
        gpWindowManager->DoDialog(m_newGameWindow, NewGameHandler, 0);
        delete m_newGameWindow;
        if (gpWindowManager->m_dialogResult == NEW_GAME_DIALOG_CANCEL) {
            result = 0;
        } else {
            m_playerCount = m_mapHeader.playerCount;
            NewMap(gMapName);
        }
    }

    for (textBufferIndex = 0; textBufferIndex < NEW_GAME_TEXT_BUFFER_COUNT; ++textBufferIndex) {
        H2_FREE(cTextReceivedBuffer[textBufferIndex], 605);
    }
    H2_FREE(cNGKPCore, 607);
    H2_FREE(cNGKPDisplay, 608);
    gpResourceManager->Dispose(NGKPBkg);
    return result;
}

VA(0x004b8168, 0xf1)
void game::CleanUpNewGameWindow(void) {
    i32 player;

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
void game::InitNewGameWindow(void) {
    DATA(0x0051cfa0) static i16 newGameWindowSourceLineBase = 635;
    i32 availableWidthResult;
    widget* textControlLocal;
    i32 firstPlayerXLocal;
    i32 multiplayerYOffsetValue;
    i32 playerCounter;
    i32 playerSpacingTemp;
    widget* iconControlLocal;
    i32 playerGapValue;
    char* label;
    i32 raceTextWidth;
    i32 singlePlayerYOffsetValue;

    iconControlLocal = 0;
    textControlLocal = 0;
    availableWidthResult = 372 - m_mapHeader.playerCount * 62;
    playerGapValue = availableWidthResult / (m_mapHeader.playerCount + 1);
    firstPlayerXLocal = playerGapValue + 24;
    playerSpacingTemp = playerGapValue + 62;
    multiplayerYOffsetValue = 0;

    if (giNumHumanPlayers > 1 && iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT)
        multiplayerYOffsetValue = -14;

    for (playerCounter = 0; playerCounter < m_mapHeader.playerCount; ++playerCounter) {
        if (giNumHumanPlayers > 1) {
            iconControlLocal = new iconWidget(
                static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal + 13),
                static_cast<i16>(multiplayerYOffsetValue + 343),
                64,
                28,
                "ngextra.icn",
                59,
                0,
                static_cast<i16>(playerCounter + NEW_GAME_PLAYER_HUMAN_FIRST),
                16,
                1
            );
            if (iconControlLocal == 0)
                MemError();
            m_newGameWindow->AddWidget(iconControlLocal, -1);

            iconControlLocal = new iconWidget(
                static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal + 16),
                static_cast<i16>(multiplayerYOffsetValue + 340),
                62,
                26,
                "ngextra.icn",
                0,
                0,
                static_cast<i16>(playerCounter + NEW_GAME_HANDICAP_FIRST),
                16,
                1
            );
            if (iconControlLocal == 0)
                MemError();
            m_newGameWindow->AddWidget(iconControlLocal, -1);
        }

        iconControlLocal = new iconWidget(
            static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal + 11),
            197,
            68,
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (NEW_GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT - NEW_GAME_RACE_WIDGET_SINGLE_HEIGHT))
                + NEW_GAME_RACE_WIDGET_SINGLE_HEIGHT
            ),
            "ngextra.icn",
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (NEW_GAME_RACE_WIDGET_MULTIPLAYER_FRAME - NEW_GAME_RACE_WIDGET_SINGLE_FRAME))
                + NEW_GAME_RACE_WIDGET_SINGLE_FRAME
            ),
            0,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
            16,
            1
        );
        if (iconControlLocal == 0)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal + 13),
            191,
            68,
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (NEW_GAME_PLAYER_WIDGET_MULTIPLAYER_HEIGHT
                    - NEW_GAME_PLAYER_WIDGET_SINGLE_HEIGHT))
                + NEW_GAME_PLAYER_WIDGET_SINGLE_HEIGHT
            ),
            "ngextra.icn",
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (NEW_GAME_PLAYER_WIDGET_MULTIPLAYER_FRAME - NEW_GAME_PLAYER_WIDGET_SINGLE_FRAME))
                + NEW_GAME_PLAYER_WIDGET_SINGLE_FRAME
            ),
            0,
            static_cast<i16>(playerCounter + NEW_GAME_PLAYER_SELECT_FIRST),
            16,
            1
        );
        if (iconControlLocal == 0)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal + 16),
            194,
            62,
            58,
            "ngextra.icn",
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (NEW_GAME_COLOR_WIDGET_MULTIPLAYER_FRAME - NEW_GAME_COLOR_WIDGET_SINGLE_FRAME))
                + NEW_GAME_COLOR_WIDGET_SINGLE_FRAME
            ),
            0,
            static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
            16,
            1
        );
        if (iconControlLocal == 0)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);

        if (giNumHumanPlayers > 1) {
            label = static_cast<char*>(H2_ALLOC(2, 715));
            sprintf(label, " ");
            textControlLocal = new textWidget(
                static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal + 19),
                240,
                56,
                9,
                label,
                "smalfont.fnt",
                1,
                static_cast<i16>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
                MESSAGE_WIDGET,
                1
            );
            if (textControlLocal == 0)
                MemError();
            m_newGameWindow->AddWidget(textControlLocal, -1);
        }

        singlePlayerYOffsetValue = 0;
        if (giNumHumanPlayers == 1)
            singlePlayerYOffsetValue = -14;
        iconControlLocal = new iconWidget(
            static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal + 11),
            static_cast<i16>(OD_STEER(multiplayerYOffsetValue) + singlePlayerYOffsetValue + 277),
            64,
            47,
            "ngextra.icn",
            61,
            0,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
            16,
            1
        );
        if (iconControlLocal == 0)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);

        label = static_cast<char*>(H2_ALLOC(2, 747));
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
            static_cast<i16>(
                playerSpacingTemp * playerCounter + firstPlayerXLocal + 15 - raceTextWidth / 2
            ),
            static_cast<i16>(OD_STEER(multiplayerYOffsetValue) + singlePlayerYOffsetValue + 322),
            static_cast<i16>(raceTextWidth + 64),
            12,
            label,
            "smalfont.fnt",
            1,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_NAME_FIRST),
            MESSAGE_WIDGET,
            1
        );
        if (textControlLocal == 0)
            MemError();
        m_newGameWindow->AddWidget(textControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal + 16),
            static_cast<i16>(OD_STEER(multiplayerYOffsetValue) + singlePlayerYOffsetValue + 274),
            62,
            45,
            "ngextra.icn",
            51,
            0,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
            16,
            1
        );
        if (iconControlLocal == 0)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);
    }
}

// @early-stop
// @early-stop-reloc-only: Current Newgame.cpp/header epoch: all 0x59c bytes
// match after masking 50 ordered relocation sites. Effective targets agree;
// residual rows only rename three compiler-local string symbols.
VA(0x004b88d6, 0x59c)
void game::UpdateNewGameWindow(void) {
    i32 playerLockedValue;
    tag_message messageTemp;
    i32 playerIndex3;
    i32 unusedPlayer17;

    strcpy(gText, m_mapHeader.name);
    messageTemp.type = MESSAGE_WIDGET;
    messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
    messageTemp.payload.widget.id = EncodeNewGameControl(NEW_GAME_SCENARIO_NAME);
    messageTemp.payload.widget.data.text = gText;
    m_newGameWindow->BroadcastMessage(messageTemp);

    messageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
    messageTemp.payload.widget.data.value = NEW_GAME_WIDGET_REFRESH_FRAME;
    for (playerIndex3 = 0; playerIndex3 < NEW_GAME_DIFFICULTY_COUNT; ++playerIndex3) {
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_DIFFICULTY_FIRST, playerIndex3);
        m_newGameWindow->BroadcastMessage(messageTemp);
    }
    messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
    messageTemp.payload.widget.id =
        EncodeNewGameControlIndex(NEW_GAME_DIFFICULTY_FIRST, m_difficulty);
    m_newGameWindow->BroadcastMessage(messageTemp);

    if (giNumHumanPlayers > 1) {
        for (playerIndex3 = 0; playerIndex3 < NEW_GAME_CHAT_LINE_COUNT; ++playerIndex3) {
            sprintf(gText, cTextReceivedBuffer[playerIndex3]);
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
            messageTemp.payload.widget.id =
                EncodeNewGameControlIndex(NEW_GAME_CHAT_FIRST, playerIndex3);
            messageTemp.payload.widget.data.text = gText;
            m_newGameWindow->BroadcastMessage(messageTemp);
        }
    }

    for (playerIndex3 = 0; playerIndex3 < m_mapHeader.playerCount; ++playerIndex3) {
        if (m_setupPlayerNetworkId[playerIndex3] == NEW_GAME_COMPUTER_PLAYER) {
            sprintf(gText, "");
        } else if (strlen(cPlayerNames[m_setupPlayerNetworkId[playerIndex3]]) != 0) {
            sprintf(gText, cPlayerNames[m_setupPlayerNetworkId[playerIndex3]]);
        } else {
            sprintf(gText, "Player %d", m_setupPlayerNetworkId[playerIndex3] + 1);
        }
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_PLAYER_NAME_FIRST, playerIndex3);
        messageTemp.payload.widget.data.text = gText;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (m_selectedSetupPlayer == playerIndex3)
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        else
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_PLAYER_SELECT_FIRST, playerIndex3);
        messageTemp.payload.widget.data.value = NEW_GAME_WIDGET_REFRESH_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (m_setupPlayerType[playerIndex3] != NEW_GAME_PLAYER_DEFAULT
            || (giNumHumanPlayers > 1
                && m_setupPlayerNetworkId[playerIndex3] != NEW_GAME_COMPUTER_PLAYER))
            playerLockedValue = 0;
        else
            playerLockedValue = 1;
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_COLOR_FIRST, playerIndex3);
        if (m_setupPlayerNetworkId[playerIndex3] == NEW_GAME_COMPUTER_PLAYER)
            messageTemp.payload.widget.data.value =
                m_setupPlayerColor[playerIndex3]
                + (playerLockedValue ? NEW_GAME_COMPUTER_COLOR_LOCKED_FRAME
                                     : NEW_GAME_COMPUTER_COLOR_UNLOCKED_FRAME);
        else
            messageTemp.payload.widget.data.value =
                m_setupPlayerColor[playerIndex3]
                + (playerLockedValue ? NEW_GAME_HUMAN_COLOR_LOCKED_FRAME
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
            EncodeNewGameControlIndex(NEW_GAME_HANDICAP_FIRST, playerIndex3);
        if (m_setupPlayerNetworkId[playerIndex3] == NEW_GAME_COMPUTER_PLAYER)
            messageTemp.payload.widget.data.value = NEW_GAME_RACE_NAME_FIRST;
        else
            messageTemp.payload.widget.data.value = m_playerHandicap[playerIndex3];
        m_newGameWindow->BroadcastMessage(messageTemp);
        if (m_setupPlayerNetworkId[playerIndex3] == NEW_GAME_COMPUTER_PLAYER)
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        else
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        messageTemp.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (m_mapHeader.playerRace[m_setupPlayerColor[playerIndex3]] == NEW_GAME_RANDOM_RACE)
            playerLockedValue = 0;
        else
            playerLockedValue = 1;
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        messageTemp.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_RACE_CYCLE_FIRST, playerIndex3);
        messageTemp.payload.widget.data.value =
            m_setupPlayerRace[playerIndex3]
            + (playerLockedValue ? NEW_GAME_FIXED_RACE_FRAME_BASE
                                 : NEW_GAME_RANDOM_RACE_FRAME_BASE);
        m_newGameWindow->BroadcastMessage(messageTemp);

        sprintf(gText, gAlignmentNames[m_setupPlayerRace[playerIndex3]]);
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_RACE_NAME_FIRST, playerIndex3);
        messageTemp.payload.widget.data.text = gText;
        m_newGameWindow->BroadcastMessage(messageTemp);
        if (playerLockedValue)
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        else
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        messageTemp.payload.widget.data.value = NEW_GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);
    }

    gpGame->m_difficultyRating = static_cast<i16>(CalcDifficultyRating());
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
i32 NewGameHandler(struct tag_message& message) {
    i32 transmitResultTemp;
    i32 redrawWindow = 0;
    i32 oldNetworkId;
    i32 swapPlayerTemp;
    i32 currentPlayerLocal;
    i32 synchronizeSetupResult = 0;
    tag_message windowMessage;
    SMapHeader remoteMapHeaderValue;
    NewGameRemotePacket* remotePacketResult;
    i32 sender;
    char setupData[NEW_GAME_SETUP_BUFFER_SIZE];
    char mapPacketLocal[NEW_GAME_MAP_PACKET_SIZE];
    tag_message mapWindowMessageTemp;
    i32 helpDialogIndexLocal;
    i32 unusedSender;
    char mapNamePacket[MAP_HEADER_NAME_SIZE];

    if (!gbNewGameShadowHidden) {
        gbNewGameShadowHidden = true;
        windowMessage.type = MESSAGE_WIDGET;
        windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        windowMessage.payload.widget.id = EncodeNewGameControl(NEW_GAME_SHADOW);
        windowMessage.payload.widget.data.value = NEW_GAME_SHADOW_FRAME;
        gpGame->m_newGameWindow->BroadcastMessage(windowMessage);
    }

    if (message.type == MESSAGE_NONE) {
        remotePacketResult = reinterpret_cast<NewGameRemotePacket*>(GetRemoteData(1));
        if (remotePacketResult != 0
            && (remotePacketResult->type == NEW_GAME_REMOTE_PACKET_TYPE
                || remotePacketResult->type == NEW_GAME_REMOTE_PACKET_TYPE_ALTERNATE)) {
            switch (remotePacketResult->command) {
                case NEW_GAME_REMOTE_START:
                    gpWindowManager->m_dialogResult = message.payload.widget.id;
                    gpWindowManager->m_dialogResult = NEW_GAME_DIALOG_OK;
                    message.type = MESSAGE_WIDGET;
                    message.payload.widget.id = NEW_GAME_DIALOG_CLOSE_MESSAGE;
                    message.payload.widget.command = message.payload.widget.id;
                    return 2;

                case NEW_GAME_REMOTE_CANCEL:
                    NormalDialog("The host has canceled the game.", 1, -1, -1, -1, 0, -1, 0, -1, 0);
                    ShutDown(0);
                    break;

                case NEW_GAME_REMOTE_SETUP:
                    if (strcmp(remotePacketResult->payload, gpGame->m_mapHeader.name) != 0)
                        break;
                    memcpy(
                        gpGame->m_setupPlayerColor,
                        remotePacketResult->payload + MAP_HEADER_NAME_SIZE,
                        NEW_GAME_SETUP_DATA_SIZE
                    );
                    redrawWindow = 1;
                    break;

                case NEW_GAME_REMOTE_MAP_HEADER:
                    memset(&remoteMapHeaderValue, 0, sizeof(remoteMapHeaderValue));
                    memcpy(
                        &remoteMapHeaderValue,
                        remotePacketResult->payload,
                        NEW_GAME_MAP_PACKET_SIZE
                    );
                    gpGame->ProcessNewMap(&remoteMapHeaderValue);
                    break;

                case NEW_GAME_REMOTE_CHAT:
                    redrawWindow = 1;
                    sender = remotePacketResult->sender;
                    if (sender >= 0) {
                        sprintf(
                            gText,
                            "%s:  %s",
                            gsNetPlayerInfo[sender].name,
                            remotePacketResult->payload
                        );
                    } else {
                        unusedSender = 0;
                    }
                    gText[NEW_GAME_CHAT_TEXT_LIMIT] = 0;
                    for (currentPlayerLocal = 0; currentPlayerLocal < NEW_GAME_CHAT_LINE_COUNT - 1;
                         ++currentPlayerLocal) {
                        strcpy(
                            cTextReceivedBuffer[currentPlayerLocal],
                            cTextReceivedBuffer[currentPlayerLocal + 1]
                        );
                    }
                    strcpy(cTextReceivedBuffer[NEW_GAME_CHAT_LINE_COUNT - 1], gText);
                    break;
            }
        }
        if (glTimers[0] < static_cast<i32>(KBTickCount())) {
            gpGame->NGKPSetupDisplayString(cNGKPCore, static_cast<u16>(NGKPcursorIndex));
            gpGame->DrawNGKPDisplayString(1);
        }
    }

    if (message.type == MESSAGE_KEY_DOWN && giNumHumanPlayers > 1
        && iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT && gpGame->ProcessNGKeyPress(message)) {
        redrawWindow = 1;
        for (currentPlayerLocal = 0; currentPlayerLocal < NEW_GAME_CHAT_LINE_COUNT - 1;
             ++currentPlayerLocal) {
            strcpy(
                cTextReceivedBuffer[currentPlayerLocal],
                cTextReceivedBuffer[currentPlayerLocal + 1]
            );
        }
        strcpy(cTextReceivedBuffer[NEW_GAME_CHAT_LINE_COUNT - 1], cNGKPCore);
        strcpy(cNGKPCore, "");
        strcpy(cNGKPDisplay, "");
        NGKPcursorIndex = 0;
        transmitResultTemp = TransmitRemoteData(
            cTextReceivedBuffer[NEW_GAME_CHAT_LINE_COUNT - 1],
            NEW_GAME_REMOTE_CHANNEL,
            strlen(cTextReceivedBuffer[NEW_GAME_CHAT_LINE_COUNT - 1]) + 1,
            NEW_GAME_REMOTE_CHAT,
            1,
            1,
            NEW_GAME_NETWORK_PLAYER_NONE
        );
        if (!transmitResultTemp)
            ShutDown(0);
    }

    if (message.type != MESSAGE_WIDGET)
        goto finish;

    if (message.payload.widget.parameter & NEW_GAME_MOUSE_RIGHT_FLAG) {
        if (message.payload.widget.command == NEW_GAME_EVENT_PRESS
            || message.payload.widget.command == NEW_GAME_EVENT_ALTERNATE_PRESS) {
            helpDialogIndexLocal = -1;
            if ((message.payload.widget.id >= NEW_GAME_DIFFICULTY_HELP_FIRST
                 && message.payload.widget.id <= LastNewGameControl(
                        NEW_GAME_DIFFICULTY_HELP_FIRST,
                        NEW_GAME_DIFFICULTY_COUNT
                    ))
                || (message.payload.widget.id >= NEW_GAME_DIFFICULTY_FIRST
                    && message.payload.widget.id <= LastNewGameControl(
                           NEW_GAME_DIFFICULTY_FIRST,
                           NEW_GAME_DIFFICULTY_COUNT
                       )))
                helpDialogIndexLocal = NEW_GAME_HELP_DIFFICULTY;
            if ((message.payload.widget.id >= NEW_GAME_HANDICAP_FIRST
                 && message.payload.widget.id <= LastNewGameControl(
                        NEW_GAME_HANDICAP_FIRST,
                        NEW_GAME_PLAYER_CONTROL_COUNT
                    ))
                || (message.payload.widget.id >= NEW_GAME_PLAYER_HUMAN_FIRST
                    && message.payload.widget.id <= LastNewGameControl(
                           NEW_GAME_PLAYER_HUMAN_FIRST,
                           NEW_GAME_PLAYER_CONTROL_COUNT
                       )))
                helpDialogIndexLocal = NEW_GAME_HELP_HANDICAP;
            if ((message.payload.widget.id >= NEW_GAME_COLOR_FIRST
                 && message.payload.widget.id
                        <= LastNewGameControl(NEW_GAME_COLOR_FIRST, NEW_GAME_PLAYER_CONTROL_COUNT))
                || (message.payload.widget.id >= NEW_GAME_RACE_FIRST
                    && message.payload.widget.id <= LastNewGameControl(
                           NEW_GAME_RACE_FIRST,
                           NEW_GAME_PLAYER_CONTROL_COUNT
                       ))
                || (message.payload.widget.id >= NEW_GAME_PLAYER_SELECT_FIRST
                    && message.payload.widget.id <= NEW_GAME_PLAYER_NAME_FIRST)
                || (message.payload.widget.id >= NEW_GAME_PLAYER_NAME_FIRST
                    && message.payload.widget.id <= NEW_GAME_RACE_ICON_FIRST))
                helpDialogIndexLocal = NEW_GAME_HELP_PLAYER;
            if ((message.payload.widget.id >= NEW_GAME_RACE_CYCLE_FIRST
                 && message.payload.widget.id <= LastNewGameControl(
                        NEW_GAME_RACE_CYCLE_FIRST,
                        NEW_GAME_PLAYER_CONTROL_COUNT
                    ))
                || (message.payload.widget.id >= NEW_GAME_RACE_ICON_FIRST
                    && message.payload.widget.id <= LastNewGameControl(
                           NEW_GAME_RACE_ICON_FIRST,
                           NEW_GAME_PLAYER_CONTROL_COUNT
                       )))
                helpDialogIndexLocal = NEW_GAME_HELP_RACE;
            if (message.payload.widget.id == NEW_GAME_MAP_OPTIONS_CONTROL
                || DecodeNewGameControl(message.payload.widget.id) == NEW_GAME_MAP_SELECT
                || DecodeNewGameControl(message.payload.widget.id) == NEW_GAME_SCENARIO_NAME)
                helpDialogIndexLocal = NEW_GAME_HELP_MAP;
            if (DecodeNewGameControl(message.payload.widget.id) == NEW_GAME_RATING)
                helpDialogIndexLocal = NEW_GAME_HELP_RATING;
            if (message.payload.widget.id == NEW_GAME_DIALOG_OK)
                helpDialogIndexLocal = NEW_GAME_HELP_OK;
            if (message.payload.widget.id == NEW_GAME_DIALOG_CANCEL)
                helpDialogIndexLocal = NEW_GAME_HELP_CANCEL;
            if (helpDialogIndexLocal != -1)
                NormalDialog(gNewGameHelp[helpDialogIndexLocal], 4, -1, -1, -1, 0, -1, 0, -1, 0);
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
                            0,
                            NEW_GAME_REMOTE_CHANNEL,
                            0,
                            NEW_GAME_REMOTE_START,
                            1,
                            1,
                            NEW_GAME_NETWORK_PLAYER_NONE
                        );
                    }
                    gpWindowManager->m_dialogResult = message.payload.widget.id;
                    message.payload.widget.id = NEW_GAME_DIALOG_CLOSE_MESSAGE;
                    message.payload.widget.command = message.payload.widget.id;
                    gbNewGameDialogOver = true;
                    return 2;

                case NEW_GAME_DIALOG_CANCEL:
                    if (gbRemoteOn) {
                        transmitResultTemp = TransmitRemoteData(
                            0,
                            NEW_GAME_REMOTE_CHANNEL,
                            0,
                            NEW_GAME_REMOTE_CANCEL,
                            1,
                            1,
                            NEW_GAME_NETWORK_PLAYER_NONE
                        );
                        ShutDown(0);
                    }
                    gpWindowManager->m_dialogResult = message.payload.widget.id;
                    message.payload.widget.id = NEW_GAME_DIALOG_CLOSE_MESSAGE;
                    message.payload.widget.command = message.payload.widget.id;
                    gbNewGameDialogOver = true;
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
                    currentPlayerLocal = message.payload.widget.id - NEW_GAME_DIFFICULTY_HELP_FIRST;
                    goto setDifficulty;

                case NEW_GAME_DIFFICULTY_FIRST:
                case NEW_GAME_DIFFICULTY_FIRST + 1:
                case NEW_GAME_DIFFICULTY_FIRST + 2:
                case NEW_GAME_DIFFICULTY_FIRST + 3:
                case NEW_GAME_DIFFICULTY_FIRST + 4:
                    currentPlayerLocal = message.payload.widget.id - NEW_GAME_DIFFICULTY_FIRST;
                setDifficulty:
                    gpGame->m_difficulty = static_cast<i8>(currentPlayerLocal);
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
                    if (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
                        != NEW_GAME_COMPUTER_PLAYER) {
                        gpGame->m_playerHandicap[currentPlayerLocal] = static_cast<i8>(
                            (gpGame->m_playerHandicap[currentPlayerLocal] + 1)
                            % NEW_GAME_HANDICAP_COUNT
                        );
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
                    if (gpGame->m_setupPlayerType[currentPlayerLocal] != NEW_GAME_PLAYER_DEFAULT
                        || (giNumHumanPlayers > 1
                            && gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
                                   != NEW_GAME_COMPUTER_PLAYER)) {
                        if (giNumHumanPlayers == 1) {
                            if (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
                                == NEW_GAME_COMPUTER_PLAYER) {
                                for (swapPlayerTemp = 0;
                                     swapPlayerTemp < gpGame->m_mapHeader.playerCount;
                                     ++swapPlayerTemp) {
                                    if (gpGame->m_setupPlayerNetworkId[swapPlayerTemp]
                                        != NEW_GAME_COMPUTER_PLAYER) {
                                        oldNetworkId =
                                            gpGame->m_setupPlayerNetworkId[swapPlayerTemp];
                                        gpGame->m_setupPlayerNetworkId[swapPlayerTemp] =
                                            gpGame->m_setupPlayerNetworkId[currentPlayerLocal];
                                        gpGame->m_setupPlayerNetworkId[currentPlayerLocal] =
                                            oldNetworkId;
                                        swapPlayerTemp = NEW_GAME_SWAP_SEARCH_DONE;
                                    }
                                }
                            }
                        } else if (gpGame->m_selectedSetupPlayer == NEW_GAME_NETWORK_PLAYER_NONE) {
                            gpGame->m_selectedSetupPlayer = static_cast<i8>(currentPlayerLocal);
                        } else if (gpGame->m_selectedSetupPlayer == currentPlayerLocal
                                   || (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
                                           == NEW_GAME_COMPUTER_PLAYER
                                       && gpGame->m_setupPlayerNetworkId
                                                  [gpGame->m_selectedSetupPlayer]
                                              == NEW_GAME_COMPUTER_PLAYER)) {
                            gpGame->m_selectedSetupPlayer = NEW_GAME_NETWORK_PLAYER_NONE;
                        } else {
                            if ((gpGame->m_setupPlayerType[currentPlayerLocal]
                                     != NEW_GAME_PLAYER_DEFAULT
                                 && gpGame->m_setupPlayerType[gpGame->m_selectedSetupPlayer]
                                        != NEW_GAME_PLAYER_DEFAULT)
                                || (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
                                        != NEW_GAME_COMPUTER_PLAYER
                                    && gpGame->m_setupPlayerNetworkId[gpGame->m_selectedSetupPlayer]
                                           != NEW_GAME_COMPUTER_PLAYER)) {
                                swapPlayerTemp = gpGame->m_setupPlayerNetworkId[currentPlayerLocal];
                                gpGame->m_setupPlayerNetworkId[currentPlayerLocal] =
                                    gpGame->m_setupPlayerNetworkId[gpGame->m_selectedSetupPlayer];
                                gpGame->m_setupPlayerNetworkId[gpGame->m_selectedSetupPlayer] =
                                    static_cast<i8>(swapPlayerTemp);
                            } else {
                                NormalDialog(
                                    "The two positions selected can not be swapped.",
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
                    if (gpGame->m_mapHeader
                            .playerRace[gpGame->m_setupPlayerColor[currentPlayerLocal]]
                        == NEW_GAME_RANDOM_RACE) {
                        if (gpGame->m_setupPlayerRace[currentPlayerLocal] == NEW_GAME_RANDOM_RACE)
                            gpGame->m_setupPlayerRace[currentPlayerLocal] = 0;
                        else if (gpGame->m_setupPlayerRace[currentPlayerLocal]
                                 == NEW_GAME_LAST_STANDARD_RACE)
                            gpGame->m_setupPlayerRace[currentPlayerLocal] = NEW_GAME_RANDOM_RACE;
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
                        mapWindowMessageTemp.type = MESSAGE_WIDGET;
                        mapWindowMessageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
                        mapWindowMessageTemp.payload.widget.id = NEW_GAME_DIALOG_CANCEL;
                        mapWindowMessageTemp.payload.widget.data.value =
                            NEW_GAME_WIDGET_INACTIVE_FRAME;
                        gpGame->m_newGameWindow->BroadcastMessage(mapWindowMessageTemp);
                        gpGame->GetMap();
                        mapWindowMessageTemp.type = MESSAGE_WIDGET;
                        mapWindowMessageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
                        mapWindowMessageTemp.payload.widget.id = NEW_GAME_DIALOG_CANCEL;
                        mapWindowMessageTemp.payload.widget.data.value =
                            NEW_GAME_WIDGET_INACTIVE_FRAME;
                        gpGame->m_newGameWindow->BroadcastMessage(mapWindowMessageTemp);
                        if (gbRemoteOn) {
                            memcpy(mapPacketLocal, &gpGame->m_mapHeader, NEW_GAME_MAP_PACKET_SIZE);
                            transmitResultTemp = TransmitRemoteData(
                                mapPacketLocal,
                                NEW_GAME_REMOTE_CHANNEL,
                                NEW_GAME_MAP_PACKET_SIZE,
                                NEW_GAME_REMOTE_MAP_HEADER,
                                1,
                                1,
                                NEW_GAME_NETWORK_PLAYER_NONE
                            );
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
        memcpy(setupData, gpGame->m_setupPlayerColor, NEW_GAME_SETUP_DATA_SIZE);
        transmitResultTemp = TransmitRemoteData(
            mapNamePacket,
            NEW_GAME_REMOTE_CHANNEL,
            NEW_GAME_SETUP_PACKET_SIZE,
            NEW_GAME_REMOTE_SETUP,
            1,
            1,
            NEW_GAME_NETWORK_PLAYER_NONE
        );
        if (!transmitResultTemp)
            ShutDown(0);
    }
    return 1;
}

// @semantic: Current Newgame.cpp/header epoch has the exact 0x80 frame/slots,
// 0x418 extent, complete semantics, and 84/84 ordered relocation sites/effective
// targets. The real local branch residuals are +0x16d (adjacent +0x370 hop versus
// direct +0x400) and +0x289 (no-op brace filter +0x297 versus direct insertion
// +0x2b5). Ten bounded CFG/guard attempts exhausted shared/default labels,
// nested/zero-aware/empty arms, operand reversals, and backspace polarity; the
// best alternatives regressed to 99.39341%. Revisit only after a relevant
// Newgame source/TU/header or comparison-state change affects local routing.
VA(0x004b9db8, 0x418)
i32 game::ProcessNGKeyPress(struct tag_message& message) {
    char workText[NEW_GAME_KEY_BUFFER_SIZE];
    char keyChar;
    i32 scanCode;
    i32 widthResult;

    if (giNumHumanPlayers == 1 || iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
        return 0;

    switch (message.payload.keyboard.keyCode) {
        case INPUT_SCAN_ESCAPE:
            if (!gbAllowTextEntryEscape)
                break;
            strcpy(cNGKPCore, "");
            break;

        case INPUT_SCAN_NUMPAD_DELETE:
            if (strlen(cNGKPCore) > NGKPcursorIndex) {
                strcpy(gText, cNGKPCore + (NGKPcursorIndex + 1));
                strcpy(cNGKPCore + NGKPcursorIndex, gText);
            }
            break;

        case INPUT_SCAN_NUMPAD_4:
            if (NGKPcursorIndex > 0)
                --NGKPcursorIndex;
            break;

        case INPUT_SCAN_NUMPAD_6:
            if (strlen(cNGKPCore) > NGKPcursorIndex)
                ++NGKPcursorIndex;
            break;

        default:
            gpInputManager->AsciiConvert(message);
            if (message.payload.keyboard.keyCode == IDX(NEW_GAME_KEY_ENTER))
                return 1;

            if (message.payload.keyboard.keyCode == IDX(NEW_GAME_KEY_BACKSPACE)) {
                if (NGKPcursorIndex > 0) {
                    strcpy(gText, cNGKPCore + NGKPcursorIndex);
                    strcpy(cNGKPCore + (NGKPcursorIndex - 1), gText);
                    --NGKPcursorIndex;
                }
            } else if (strlen(cNGKPCore) + 1 < NEW_GAME_CHAT_TEXT_LIMIT
                       && message.payload.keyboard.keyCode != 0) {
                strcpy(workText, cNGKPCore);
                keyChar = 0;
                if (message.payload.keyboard.keyCode >= IDX(NEW_GAME_KEY_FIRST_EXTENDED)) {
                    scanCode =
                        static_cast<u8>(static_cast<u32>(message.payload.keyboard.keyCode) >> 8);
                    switch (scanCode) {
                        case IDX(NEW_GAME_KEYPAD_INSERT):
                            keyChar = '0';
                            break;
                        case IDX(NEW_GAME_KEYPAD_END):
                            keyChar = '1';
                            break;
                        case IDX(NEW_GAME_KEYPAD_DOWN):
                            keyChar = '2';
                            break;
                        case IDX(NEW_GAME_KEYPAD_PAGE_DOWN):
                            keyChar = '3';
                            break;
                        case IDX(NEW_GAME_KEYPAD_LEFT):
                            keyChar = '4';
                            break;
                        case IDX(NEW_GAME_KEYPAD_CENTER):
                            keyChar = '5';
                            break;
                        case IDX(NEW_GAME_KEYPAD_RIGHT):
                            keyChar = '6';
                            break;
                        case IDX(NEW_GAME_KEYPAD_HOME):
                            keyChar = '7';
                            break;
                        case IDX(NEW_GAME_KEYPAD_UP):
                            keyChar = '8';
                            break;
                        case IDX(NEW_GAME_KEYPAD_PAGE_UP):
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
                    NGKPSetupDisplayString(cNGKPCore, static_cast<u16>(NGKPcursorIndex));
                    widthResult = smallFont->LineLength(cNGKPDisplay, NEW_GAME_CHAT_DRAW_WIDTH);
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
void game::NGKPSetupDisplayString(char* text, u16 cursor) {
    if (giNumHumanPlayers == 1 || iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
        return;

    if (glTimers[0] < static_cast<i32>(KBTickCount())) {
        NGKPcursorFlashOn = 1 - NGKPcursorFlashOn;
        glTimers[0] = KBTickCount() + NEW_GAME_CURSOR_FLASH_TICKS;
    }

    if (cursor > 0)
        strncpy(cNGKPDisplay, text, cursor);

    if (NGKPcursorFlashOn)
        cNGKPDisplay[cursor] = FONT_SPACER_CHAR;
    else
        cNGKPDisplay[cursor] = '_';

    if (cursor < strlen(text))
        strcpy(cNGKPDisplay + (cursor + 1), text + cursor);
    else
        cNGKPDisplay[cursor + 1] = 0;
}

VA(0x004ba2ee, 0xae)
void game::DrawNGKPDisplayString(i32 updateScreen) {
    if (gbNewGameDialogOver != 0)
        return;

    if (giNumHumanPlayers == 1 || iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
        return;

    NGKPBkg->DrawToBuffer(
        NEW_GAME_CHAT_DRAW_X,
        NEW_GAME_CHAT_DRAW_Y,
        NEW_GAME_CHAT_BACKGROUND_WIDTH,
        0
    );
    smallFont->DrawBoundedString(
        cNGKPDisplay,
        NEW_GAME_CHAT_DRAW_X,
        NEW_GAME_CHAT_DRAW_Y,
        NEW_GAME_CHAT_DRAW_WIDTH,
        NEW_GAME_CHAT_DRAW_HEIGHT,
        2,
        0
    );
    if (updateScreen)
        gpWindowManager->UpdateScreenRegion(
            NEW_GAME_CHAT_DRAW_X,
            NEW_GAME_CHAT_DRAW_Y,
            NEW_GAME_CHAT_DRAW_WIDTH,
            NEW_GAME_CHAT_DRAW_HEIGHT
        );
}

VA(0x004ba39c, 0xb71)
void game::ShowScenInfo(void) {
    DATA(0x0051d0fc) static i16 scenarioInfoSourceLineBase = 1615;
    i32 availableWidthResult;
    i32 mapSizeIndex;
    widget* textControlLocal;
    i32 firstPlayerXLocal;
    i32 multiplayerYOffsetValue;
    i32 playerCounter;
    i32 playerLockedLocal;
    i32 playerSpacingTemp;
    tag_message scenarioMessageTemp;
    widget* iconControlLocal;
    heroWindow* scenarioWindowValue;
    i32 playerGapValue;
    char* label;
    i32 raceTextWidth;
    i32 singlePlayerYOffsetValue;

    gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
    scenarioWindowValue = new heroWindow(90, 4, "sceninfo.bin");
    if (scenarioWindowValue == 0)
        MemError();
    SetWinText(scenarioWindowValue, NEW_GAME_SCENARIO_WINDOW_TEXT_ID);

    scenarioMessageTemp.type = MESSAGE_WIDGET;
    scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
    scenarioMessageTemp.payload.widget.id = EncodeNewGameControl(NEW_GAME_SCENARIO_NAME);
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
    if (m_mapHeader.width == IDX(MAP_DIMENSION_MEDIUM))
        mapSizeIndex = 1;
    else if (m_mapHeader.width == IDX(MAP_DIMENSION_LARGE))
        mapSizeIndex = 2;
    else if (m_mapHeader.width == IDX(MAP_DIMENSION_XLARGE))
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
                static_cast<i16>(
                    OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal + 13
                ),
                static_cast<i16>(multiplayerYOffsetValue + 309),
                64,
                28,
                "ngextra.icn",
                59,
                0,
                static_cast<i16>(playerCounter + NEW_GAME_PLAYER_HUMAN_FIRST),
                16,
                1
            );
            if (iconControlLocal == 0)
                MemError();
            scenarioWindowValue->AddWidget(iconControlLocal, -1);

            iconControlLocal = new iconWidget(
                static_cast<i16>(
                    OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal + 16
                ),
                static_cast<i16>(multiplayerYOffsetValue + 306),
                62,
                26,
                "ngextra.icn",
                0,
                0,
                static_cast<i16>(playerCounter + NEW_GAME_HANDICAP_FIRST),
                16,
                1
            );
            if (iconControlLocal == 0)
                MemError();
            scenarioWindowValue->AddWidget(iconControlLocal, -1);
        }

        iconControlLocal = new iconWidget(
            static_cast<i16>(OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal + 11),
            163,
            68,
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (NEW_GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT - NEW_GAME_RACE_WIDGET_SINGLE_HEIGHT))
                + NEW_GAME_RACE_WIDGET_SINGLE_HEIGHT
            ),
            "ngextra.icn",
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (NEW_GAME_RACE_WIDGET_MULTIPLAYER_FRAME - NEW_GAME_RACE_WIDGET_SINGLE_FRAME))
                + NEW_GAME_RACE_WIDGET_SINGLE_FRAME
            ),
            0,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
            16,
            1
        );
        if (iconControlLocal == 0)
            MemError();
        scenarioWindowValue->AddWidget(iconControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<i16>(OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal + 16),
            160,
            62,
            58,
            "ngextra.icn",
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (NEW_GAME_COLOR_WIDGET_MULTIPLAYER_FRAME - NEW_GAME_COLOR_WIDGET_SINGLE_FRAME))
                + NEW_GAME_COLOR_WIDGET_SINGLE_FRAME
            ),
            0,
            static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
            16,
            1
        );
        if (iconControlLocal == 0)
            MemError();
        scenarioWindowValue->AddWidget(iconControlLocal, -1);

        if (giNumHumanPlayers > 1) {
            label = static_cast<char*>(H2_ALLOC(2, 1729));
            sprintf(label, " ");
            textControlLocal = new textWidget(
                static_cast<i16>(
                    OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal + 19
                ),
                206,
                56,
                9,
                label,
                "smalfont.fnt",
                1,
                static_cast<i16>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
                MESSAGE_WIDGET,
                1
            );
            if (textControlLocal == 0)
                MemError();
            scenarioWindowValue->AddWidget(textControlLocal, -1);
        }

        singlePlayerYOffsetValue = 0;
        iconControlLocal = new iconWidget(
            static_cast<i16>(OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal + 11),
            static_cast<i16>(OD_STEER(multiplayerYOffsetValue) + singlePlayerYOffsetValue + 243),
            64,
            47,
            "ngextra.icn",
            61,
            0,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
            16,
            1
        );
        if (iconControlLocal == 0)
            MemError();
        scenarioWindowValue->AddWidget(iconControlLocal, -1);

        label = static_cast<char*>(H2_ALLOC(2, 1761));
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
            static_cast<i16>(
                OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal + 15
                - raceTextWidth / 2
            ),
            static_cast<i16>(OD_STEER(multiplayerYOffsetValue) + singlePlayerYOffsetValue + 288),
            static_cast<i16>(raceTextWidth + 64),
            12,
            label,
            "smalfont.fnt",
            1,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_NAME_FIRST),
            MESSAGE_WIDGET,
            1
        );
        if (textControlLocal == 0)
            MemError();
        scenarioWindowValue->AddWidget(textControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<i16>(OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal + 16),
            static_cast<i16>(OD_STEER(multiplayerYOffsetValue) + singlePlayerYOffsetValue + 240),
            62,
            45,
            "ngextra.icn",
            51,
            0,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
            16,
            1
        );
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
            sprintf(gText, "Player %d", m_setupPlayerNetworkId[playerCounter] + 1);
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

        if (m_setupPlayerType[playerCounter] != NEW_GAME_PLAYER_DEFAULT
            || (giNumHumanPlayers > 1
                && m_setupPlayerNetworkId[playerCounter] != NEW_GAME_COMPUTER_PLAYER))
            playerLockedLocal = 0;
        else
            playerLockedLocal = 1;
        scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        scenarioMessageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_COLOR_FIRST, playerCounter);
        if (m_setupPlayerNetworkId[playerCounter] == NEW_GAME_COMPUTER_PLAYER)
            scenarioMessageTemp.payload.widget.data.value =
                m_setupPlayerColor[playerCounter]
                + (playerLockedLocal ? NEW_GAME_COMPUTER_COLOR_LOCKED_FRAME
                                     : NEW_GAME_COMPUTER_COLOR_UNLOCKED_FRAME);
        else
            scenarioMessageTemp.payload.widget.data.value =
                m_setupPlayerColor[playerCounter]
                + (playerLockedLocal ? NEW_GAME_HUMAN_COLOR_LOCKED_FRAME
                                     : NEW_GAME_HUMAN_COLOR_UNLOCKED_FRAME);
        if (giNumHumanPlayers > 1)
            scenarioMessageTemp.payload.widget.data.value +=
                NEW_GAME_MULTIPLAYER_COLOR_FRAME_OFFSET;
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
        scenarioMessageTemp.payload.widget.data.value =
            m_setupPlayerRace[playerCounter]
            + (playerLockedLocal ? NEW_GAME_FIXED_RACE_FRAME_BASE
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
// @early-stop-reloc-only: Current Newgame.cpp/header epoch: all 0x1c7 bytes
// match after masking 14 ordered relocation sites. Semantic suffixes restore
// the six retail local slots; remaining rows only rename string symbols.
VA(0x004baf0d, 0x1c7)
void game::GetLossConditionText(char* text) {
    i32 week2;
    hero* lossHero11;
    i32 day26;
    i32 month19;
    town* lossTown9;
    i32 townId12;

    if (m_mapHeader.lossCondition != IDX(MAP_LOSS_STANDARD)) {
        switch (m_mapHeader.lossCondition) {
            case IDX(MAP_LOSS_TOWN):
                townId12 = GetTownId(m_mapHeader.lossConditionValue, m_mapHeader.lossTownY);
                lossTown9 = GetTown(townId12);
                sprintf(
                    text,
                    "Lose the %s '%s'.",
                    (lossTown9->m_buildings & IDX(TOWN_BUILDING_CASTLE)) ? "castle" : "town",
                    lossTown9->m_name
                );
                break;

            case IDX(MAP_LOSS_HERO):
                lossHero11 = GetHero(m_mapHeader.lossConditionValue);
                sprintf(text, "Lose the hero '%s'.", lossHero11->m_name);
                break;

            case IDX(MAP_LOSS_TIME):
                month19 =
                    (gpGame->m_mapHeader.lossConditionValue - 1) / NEW_GAME_DAYS_PER_MONTH + 1;
                week2 = (gpGame->m_mapHeader.lossConditionValue
                         - (month19 - 1) * NEW_GAME_DAYS_PER_MONTH - 1)
                            / NEW_GAME_DAYS_PER_WEEK
                        + 1;
                day26 = (gpGame->m_mapHeader.lossConditionValue - 1) % NEW_GAME_DAYS_PER_WEEK + 1;
                sprintf(
                    text,
                    "Fail to win by the end of month %d, week %d, day %d.",
                    month19,
                    week2,
                    day26
                );
                break;
        }
    } else {
        sprintf(text, "Lose all your heroes, towns and castles.");
    }
}

VA(0x004bb0d4, 0x2fb)
void game::GetVictoryConditionText(char* text) {
    i32 unusedVictoryWord;
    hero* victoryHeroData;
    i32 firstSideIsLocalResult;
    char firstSide[NEW_GAME_SIDE_TEXT_SIZE];
    char secondSideValue[NEW_GAME_SIDE_TEXT_SIZE];
    town* victoryTown;
    i32 townId;

    if (m_mapHeader.victoryCondition != IDX(MAP_VICTORY_DEFEAT_ALL)) {
        switch (m_mapHeader.victoryCondition) {
            case IDX(MAP_VICTORY_CAPTURE_TOWN):
                townId = GetTownId(m_mapHeader.victoryConditionValue, m_mapHeader.victoryTownY);
                victoryTown = GetTown(townId);
                sprintf(
                    text,
                    "Capture the %s '%s'",
                    (victoryTown->m_buildings & IDX(TOWN_BUILDING_CASTLE)) ? "castle" : "town",
                    victoryTown->m_name
                );
                break;

            case IDX(MAP_VICTORY_DEFEAT_HERO):
                victoryHeroData = GetHero(m_mapHeader.victoryConditionValue);
                sprintf(text, "Defeat the hero '%s'", victoryHeroData->m_name);
                break;

            case IDX(MAP_VICTORY_FIND_ARTIFACT):
                if (m_mapHeader.victoryConditionValue == 0)
                    sprintf(text, "Find the ultimate artifact");
                else
                    sprintf(
                        text,
                        "Find the %s",
                        gArtifactNames[m_mapHeader.victoryConditionValue - 1]
                    );
                break;

            case IDX(MAP_VICTORY_ACCUMULATE_GOLD):
                sprintf(
                    text,
                    "Accumulate %d gold",
                    m_mapHeader.victoryConditionValue * NEW_GAME_GOLD_CONDITION_MULTIPLIER
                );
                break;

            case IDX(MAP_VICTORY_DEFEAT_SIDE):
                firstSideIsLocalResult =
                    GetSideDesc(firstSide, 0, m_mapHeader.victoryConditionValue - 1);
                GetSideDesc(
                    secondSideValue,
                    m_mapHeader.victoryConditionValue,
                    m_mapHeader.playerCount - 1
                );
                if (firstSideIsLocalResult)
                    sprintf(text, "%s must defeat %s", firstSide, secondSideValue);
                else
                    sprintf(text, "%s must defeat %s", secondSideValue, firstSide);
        }

        if (m_mapHeader.victoryCondition != IDX(MAP_VICTORY_DEFEAT_SIDE)
            && m_mapHeader.allowNormalVictory != 0)
            strcat(
                text,
                ", or you may win by defeating all enemy heroes and capturing all enemy towns and "
                "castles."
            );
        else
            strcat(text, ".");
    } else {
        strcpy(text, "Defeat all enemy heroes and capture all enemy towns and castles.");
    }
}

VA(0x004bb3cf, 0x2a7)
i32 game::GetSideDesc(char* text, i32 firstPlayer, i32 lastPlayer) {
    char colorName3[NEW_GAME_SIDE_TEXT_SIZE];
    i32 sideSize;
    i32 localPlayerOnSide;
    i32 unusedSideWord1;
    i32 localPlayerIndex;
    i32 player5;
    i32 listedPlayerCount5;
    i32 otherPlayerCount2;

    localPlayerIndex = -1;
    for (player5 = 0; player5 < m_mapHeader.playerCount; ++player5) {
        if (m_setupPlayerNetworkId[player5] == giThisGamePos)
            localPlayerIndex = player5;
    }

    if (OD_STEER(localPlayerIndex) >= firstPlayer && OD_STEER(localPlayerIndex) <= lastPlayer)
        localPlayerOnSide = 1;
    else
        localPlayerOnSide = 0;

    sideSize = lastPlayer - firstPlayer + 1;
    otherPlayerCount2 = sideSize - (localPlayerOnSide != 0);

    if (localPlayerOnSide) {
        if (otherPlayerCount2 != 0) {
            if (otherPlayerCount2 > 1)
                sprintf(text, "You and your allies ");
            else
                sprintf(text, "You and your ally ");

            listedPlayerCount5 = 0;
            for (player5 = firstPlayer; OD_STEER(player5) <= lastPlayer; ++player5) {
                if (player5 != OD_STEER(localPlayerIndex)) {
                    ++listedPlayerCount5;
                    sprintf(colorName3, gColors[m_setupPlayerColor[player5]]);
                    colorName3[0] -= 'a' - 'A';
                    strcat(text, colorName3);
                    if (listedPlayerCount5 < otherPlayerCount2 - 1)
                        strcat(text, ", ");
                    else if (listedPlayerCount5 < otherPlayerCount2)
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

        listedPlayerCount5 = 0;
        for (player5 = firstPlayer; OD_STEER(player5) <= lastPlayer; ++player5) {
            ++listedPlayerCount5;
            sprintf(colorName3, gColors[m_setupPlayerColor[player5]]);
            colorName3[0] -= 'a' - 'A';
            strcat(text, colorName3);
            if (listedPlayerCount5 < otherPlayerCount2 - 1)
                strcat(text, ", ");
            else if (listedPlayerCount5 < otherPlayerCount2)
                strcat(text, " and ");
        }
    }

    return localPlayerOnSide;
}

// @data-layout-note
// Retail initialized storage is 0x11cd20..0x11d450 (0x730 bytes): 105 real
// definitions cover 0x72a bytes exactly and the final six bytes are zero tail
// alignment. VC 4.2 puts the three explicit function statics first at candidate
// offsets 0, 4, and 8; retail interleaves them at offsets 0xb0, 0x280, and
// 0x3dc. The remaining candidate ranges translate as [0x14,0xbc) - 0xc,
// [0xbc,0x288) - 8, [0x288,0x3e0) - 4, and [0x3e0,0x72a) unchanged. All 105
// owner extents are disjoint and byte-exact after that transformation
// (candidate SHA-256 caa3177722a117716f26b34dd4753e91b4ff168e0812d8ebbc1630bf60116e48).
// The three static words contain 0x013f, 0x027b, and 0x064f and have 6, 2,
// and 2 zero-addend references.
//
// All 100 compiler-local definitions have one candidate code reference and
// all 100 pair to the translated retail owner. In particular, retail sites
// 0xb9240, 0xb9253, and 0xb9e09 target $SG35237 at 0x11d0c0, $SG35238 at
// 0x11d0c4, and $SG35362 at 0x11d0f8. NewGameHandler has one unrelated extra
// candidate DIR32 at public-range index 116, to local label $L35342; deleting
// only that local-label site aligns its other 121 DIR32 records with retail.
// This is not an allocation identity or a reason to add synthetic storage.
//
// Retail and candidate zero-fill are both 0x20 bytes and contain the same six
// source-defined owners. Candidate reference counts are 20, 3, 12, 20, 3,
// and 11 for NGKPcursorIndex, gbNewGameShadowHidden, cTextReceivedBuffer,
// cNGKPCore, NGKPBkg, and cNGKPDisplay; cTextReceivedBuffer addends are six
// at 0, two at 4, and four at 8, and every other addend is zero. Retail COMMON
// order is cNGKPDisplay, gbNewGameShadowHidden, cNGKPCore, NGKPcursorIndex,
// cTextReceivedBuffer[3], NGKPBkg; candidate order differs but every owner and
// extent is present. Do not introduce aliases, padding, or fake storage.
// ---- globals (definitions, RVA order) ----
DATA(0x0051cd20) b32 gbNewGameDialogOver = true;
DATA(0x0051cd24) i32 NGKPcursorFlashOn = 1;
DATA(0x00533150) char* cNGKPDisplay;
DATA(0x00533154) b32 gbNewGameShadowHidden;
DATA(0x00533158) char* cNGKPCore;
DATA(0x0053315c) i32 NGKPcursorIndex;
DATA(0x00533160) char* cTextReceivedBuffer[3];
DATA(0x0053316c) class icon* NGKPBkg;

#undef RETAIL_FILE
