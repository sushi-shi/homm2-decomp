#include <Ints.h>
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
#include <BASE/Utf8.h>
#include <BASE/widgetKind.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/KB.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/REQUEST.h>
#include <SOURCE/SETUP.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/fileRequester.h>
#include <SOURCE/game.h>
#include <PLATFORM/Runtime.h>
#include <PLATFORM/Strings.h>
#include <SOURCE/Newgame.h>

#include <string>
#include <SOURCE/Localization.h>

typedef enum NewGameConstant {
    GAME_TEXT_BUFFER_COUNT                = 3,
    GAME_TEXT_BUFFER_SIZE                 = 0x65,
    GAME_KEY_BUFFER_SIZE                  = 0x69,
    GAME_MAP_PACKET_SIZE                  = 0x74,
    GAME_PLAYER_INFO_BUFFER_SIZE          = 0x100,
    GAME_PLAYER_INFO_PACKET_SIZE          = 0xcc,
    GAME_SETUP_DATA_SIZE                  = 0x41,
    GAME_SETUP_BUFFER_SIZE                = 240,
    GAME_SETUP_PACKET_SIZE                = 0x7d,
    GAME_CHAT_TEXT_LIMIT                  = 100,
    GAME_REMOTE_CHANNEL                   = 0x7f,
    GAME_REMOTE_CHAT                      = 0x0b,
    GAME_REMOTE_SETUP                     = 0x33,
    GAME_REMOTE_MAP_HEADER                = 0x34,
    GAME_REMOTE_START                     = 0x35,
    GAME_REMOTE_CANCEL                    = 0x36,
    GAME_REMOTE_PLAYER_INFO               = 0x37,
    GAME_NETWORK_PLAYER_NONE              = -1,
    GAME_DIALOG_CLOSE_MESSAGE             = 10,
    GAME_PLAYER_DEFAULT                   = 0,
    GAME_PLAYER_FLEXIBLE                  = 1,
    GAME_DIALOG_CANCEL                    = 0x7801,
    GAME_DIALOG_OK                        = 0x7802,
    GAME_MAP_OPTIONS_CONTROL              = 0x36,
    GAME_WIDGET_ACTIVE_FRAME              = 0x1000,
    GAME_WIDGET_INACTIVE_FRAME            = 2,
    GAME_WIDGET_REFRESH_FRAME             = 4,
    GAME_SHADOW_FRAME                     = 6,
    GAME_PLAYER_CONTROL_COUNT             = 6,
    GAME_CHAT_LINE_COUNT                  = 3,
    GAME_SWAP_SEARCH_DONE                 = 999,
    GAME_COMPUTER_COLOR_LOCKED_FRAME      = 15,
    GAME_COMPUTER_COLOR_UNLOCKED_FRAME    = 3,
    GAME_HUMAN_COLOR_LOCKED_FRAME         = 21,
    GAME_HUMAN_COLOR_UNLOCKED_FRAME       = 9,
    GAME_MULTIPLAYER_COLOR_FRAME_OFFSET   = 24,
    GAME_RACE_WIDGET_SINGLE_HEIGHT        = 47,
    GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT   = 64,
    GAME_RACE_WIDGET_SINGLE_FRAME         = 79,
    GAME_RACE_WIDGET_MULTIPLAYER_FRAME    = 60,
    GAME_PLAYER_WIDGET_SINGLE_HEIGHT      = 51,
    GAME_PLAYER_WIDGET_MULTIPLAYER_HEIGHT = 64,
    GAME_PLAYER_WIDGET_SINGLE_FRAME       = 80,
    GAME_PLAYER_WIDGET_MULTIPLAYER_FRAME  = 63,
    GAME_COLOR_WIDGET_SINGLE_FRAME        = 3,
    GAME_COLOR_WIDGET_MULTIPLAYER_FRAME   = 27,
    GAME_FIXED_RACE_FRAME_BASE            = 70,
    GAME_RANDOM_RACE_FRAME_BASE           = 51,
    GAME_HELP_DIFFICULTY                  = 0,
    GAME_HELP_HANDICAP                    = 1,
    GAME_HELP_PLAYER                      = 2,
    GAME_HELP_RACE                        = 3,
    GAME_HELP_MAP                         = 4,
    GAME_HELP_RATING                      = 5,
    GAME_HELP_OK                          = 6,
    GAME_HELP_CANCEL                      = 7,
    GAME_CURSOR_FLASH_TICKS               = 0x168,
    GAME_CHAT_DRAW_X                      = 0xf4,
    GAME_CHAT_DRAW_Y                      = 0x196,
    GAME_CHAT_DRAW_WIDTH                  = 0x15c,
    GAME_CHAT_DRAW_HEIGHT                 = 0x0c,
    GAME_CHAT_BACKGROUND_WIDTH            = 0x51,
    GAME_CHAT_MAX_LINES                   = 1,
    GAME_DAYS_PER_WEEK                    = 7,
    GAME_DAYS_PER_MONTH                   = 28,
    GAME_GOLD_CONDITION_MULTIPLIER        = 1000,
    GAME_SIDE_TEXT_SIZE                   = 100,
    GAME_SCENARIO_WINDOW_TEXT_ID          = 23,
    GAME_SCENARIO_DIFFICULTY              = 200,
    GAME_SCENARIO_SELECTED_DIFFICULTY     = 201,
    GAME_SCENARIO_RATING                  = 202,
    GAME_SCENARIO_MAP_SIZE                = 203,
    GAME_SCENARIO_DESCRIPTION             = 204,
    GAME_SCENARIO_VICTORY                 = 205,
    GAME_SCENARIO_LOSS                    = 206
} NewGameConstant;

enum class NewGameKeyCode : i32 {
    GAME_KEY_ENTER          = 10,
    GAME_KEY_BACKSPACE      = 0x7f,
    GAME_KEY_FIRST_EXTENDED = 0x100,
    GAME_KEYPAD_HOME        = 0x47,
    GAME_KEYPAD_UP          = 0x48,
    GAME_KEYPAD_PAGE_UP     = 0x49,
    GAME_KEYPAD_LEFT        = 0x4b,
    GAME_KEYPAD_CENTER      = 0x4c,
    GAME_KEYPAD_RIGHT       = 0x4d,
    GAME_KEYPAD_END         = 0x4f,
    GAME_KEYPAD_DOWN        = 0x50,
    GAME_KEYPAD_PAGE_DOWN   = 0x51,
    GAME_KEYPAD_INSERT      = 0x52
};
using enum NewGameKeyCode;

typedef enum NewGameStorageConstant {
    FILE_MASK_CAPACITY      = 16,
    SAVED_MAP_NAME_CAPACITY = 16,
    PLAYER_LABEL_CAPACITY   = 2
} NewGameStorageConstant;

typedef enum NewGameDialogConstant {
    MAP_REQUESTER_X           = 212,
    MAP_REQUESTER_Y           = 9,
    MAP_CHOICE_WINDOW_X       = 405,
    MAP_CHOICE_WINDOW_Y       = 8,
    NEW_GAME_WINDOW_X         = 190,
    NEW_GAME_NETWORK_WINDOW_Y = 4,
    NEW_GAME_SINGLE_WINDOW_Y  = 33,
    NEW_GAME_WINDOW_TEXT_ID   = 7,
    BROKENA_MAX_HUMAN_PLAYERS = 3,
    SCENARIO_WINDOW_X         = 90,
    SCENARIO_WINDOW_Y         = 4,
    NEW_GAME_HELP_DIALOG_TYPE = NORMAL_DIALOG_QUICK_VIEW,
} NewGameDialogConstant;

enum class NewGameMapChoice : i32 {
    MAP_CHOICE_STANDARD  = 1,
    MAP_CHOICE_EXPANSION = 2,
    MAP_CHOICE_CANCEL    = GAME_DIALOG_CANCEL
};
using enum NewGameMapChoice;

constexpr NewGameMapChoice NewGameMapChoiceFromCode(i16 value) {
    return static_cast<NewGameMapChoice>(value); // H2_ENUM_CODE_BOUNDARY
}

enum class NewGamePlayerSlot : i32 {
    PLAYER_SLOT_FIRST  = 0,
    PLAYER_SLOT_SECOND = 1,
    PLAYER_SLOT_THIRD  = 2,
    PLAYER_SLOT_FOURTH = 3,
    PLAYER_SLOT_FIFTH  = 4,
    PLAYER_SLOT_SIXTH  = 5
};
using enum NewGamePlayerSlot;

typedef enum NewGameMapSizeIndex {
    MAP_SIZE_SMALL_INDEX  = 0,
    MAP_SIZE_MEDIUM_INDEX = 1,
    MAP_SIZE_LARGE_INDEX  = 2,
    MAP_SIZE_XLARGE_INDEX = 3
} NewGameMapSizeIndex;

typedef enum NewGamePlayerLayout {
    PLAYER_AREA_WIDTH                 = 372,
    PLAYER_COLUMN_WIDTH               = 62,
    PLAYER_FIRST_X_OFFSET             = 24,
    PLAYER_MULTIPLAYER_Y_OFFSET       = -14,
    PLAYER_SINGLE_Y_OFFSET            = -14,
    PLAYER_HUMAN_X_OFFSET             = 13,
    PLAYER_HUMAN_Y                    = 343,
    PLAYER_HUMAN_WIDTH                = 64,
    PLAYER_HUMAN_HEIGHT               = 28,
    PLAYER_HUMAN_FRAME                = 59,
    PLAYER_HANDICAP_X_OFFSET          = 16,
    PLAYER_HANDICAP_Y                 = 340,
    PLAYER_HANDICAP_WIDTH             = 62,
    PLAYER_HANDICAP_HEIGHT            = 26,
    PLAYER_RACE_X_OFFSET              = 11,
    PLAYER_RACE_Y                     = 197,
    PLAYER_RACE_WIDTH                 = 68,
    PLAYER_SELECT_X_OFFSET            = 13,
    PLAYER_SELECT_Y                   = 191,
    PLAYER_SELECT_WIDTH               = 68,
    PLAYER_COLOR_X_OFFSET             = 16,
    PLAYER_COLOR_Y                    = 194,
    PLAYER_COLOR_WIDTH                = 62,
    PLAYER_COLOR_HEIGHT               = 58,
    PLAYER_NAME_X_OFFSET              = 19,
    PLAYER_NAME_Y                     = 240,
    PLAYER_NAME_WIDTH                 = 56,
    PLAYER_NAME_HEIGHT                = 9,
    PLAYER_RACE_ICON_X_OFFSET         = 11,
    PLAYER_RACE_ICON_Y                = 277,
    PLAYER_RACE_ICON_WIDTH            = 64,
    PLAYER_RACE_ICON_HEIGHT           = 47,
    PLAYER_RACE_ICON_FRAME            = 61,
    PLAYER_RACE_NAME_X_OFFSET         = 15,
    PLAYER_RACE_NAME_Y                = 322,
    PLAYER_RACE_NAME_BASE_WIDTH       = 64,
    PLAYER_RACE_NAME_HEIGHT           = 24,
    PLAYER_RACE_NAME_CENTER_DIVISOR   = 2,
    PLAYER_RACE_NAME_NARROW_THRESHOLD = 5,
    PLAYER_RACE_NAME_HIDDEN_THRESHOLD = MAP_HEADER_PLAYER_COUNT,
    PLAYER_RACE_NAME_NARROW_WIDTH     = 16,
    PLAYER_RACE_NAME_WIDE_WIDTH       = 26,
    PLAYER_RACE_CYCLE_X_OFFSET        = 16,
    PLAYER_RACE_CYCLE_Y               = 274,
    PLAYER_RACE_CYCLE_WIDTH           = 62,
    PLAYER_RACE_CYCLE_HEIGHT          = 45,
    PLAYER_RACE_CYCLE_FRAME           = 51,
    PLAYER_WIDGET_FILL_COLOR          = 1,
    SCENARIO_PLAYER_Y_OFFSET          = 34,
    SCENARIO_PLAYER_HUMAN_Y           = PLAYER_HUMAN_Y - SCENARIO_PLAYER_Y_OFFSET,
    SCENARIO_PLAYER_HANDICAP_Y        = PLAYER_HANDICAP_Y - SCENARIO_PLAYER_Y_OFFSET,
    SCENARIO_PLAYER_RACE_Y            = PLAYER_RACE_Y - SCENARIO_PLAYER_Y_OFFSET,
    SCENARIO_PLAYER_COLOR_Y           = PLAYER_COLOR_Y - SCENARIO_PLAYER_Y_OFFSET,
    SCENARIO_PLAYER_NAME_Y            = PLAYER_NAME_Y - SCENARIO_PLAYER_Y_OFFSET,
    SCENARIO_PLAYER_RACE_ICON_Y       = PLAYER_RACE_ICON_Y - SCENARIO_PLAYER_Y_OFFSET,
    SCENARIO_PLAYER_RACE_NAME_Y       = PLAYER_RACE_NAME_Y - SCENARIO_PLAYER_Y_OFFSET,
    SCENARIO_PLAYER_RACE_CYCLE_Y      = PLAYER_RACE_CYCLE_Y - SCENARIO_PLAYER_Y_OFFSET
} NewGamePlayerLayout;

typedef enum NewGameKeyEncoding {
    KEY_SCAN_CODE_SHIFT = 8,
    KEY_SCAN_CODE_MASK  = 0xff00,
    KEY_ASCII_MASK      = 0xff
} NewGameKeyEncoding;

void game::GetMap(void) {

    fileRequester* requesterResult;
    i32 loadResult;
    char fileMask[FILE_MASK_CAPACITY];
    char savedName[SAVED_MAP_NAME_CAPACITY];

    strcpy(savedName, gMapName);
    strcpy(
        gcCurMapName,
        ""
    );
    if (gbRemoteOn && xNetHasOldPlayers) {
        NormalDialog(
            localization::Tr("network.load.expansion_unavailable"),
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
        utf8::Format(fileMask, "*.%s", "MP2");
    } else if (xIsExpansionMap) {
        utf8::Format(fileMask, "*.%s", "MX2");
    } else {
        utf8::Format(fileMask, "*.%s", "MP2");
    }

    requesterResult = new fileRequester(
        MAP_REQUESTER_X,
        MAP_REQUESTER_Y,
        FILE_REQUESTER_MAP_GAME,
        fileMask,
        gcMapPath,
        fileMask + 1
    );
    if (requesterResult == NULL)
        MemError();
    loadResult = gpExec->DoDialog(requesterResult);
    if (loadResult == FILE_REQUESTER_OK) {
        delete requesterResult;
        strcpy(gMapName, gLastFilename);
        if (platform::CompareIgnoringCase(savedName, gMapName) != 0) {
            strcpy(m_mapFilename, gMapName);
            ProcessNewMap(NULL);
        }
    } else {
        delete requesterResult;
        strcpy(gMapName, savedName);
    }
}

void game::ProcessNewMap(struct SMapHeader* header) {
    m_newGameInitialized = false;
    m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
    if (m_newGameWindow == NULL)
        return;
    CleanUpNewGameWindow();
    InitNewGame(header);
    InitNewGameWindow();
    UpdateNewGameWindow();
    m_newGameWindow->DrawWindow();
}

void game::InitNewGame(struct SMapHeader* header) {
    i32 humanCount;
    b32 playerType;
    i32 player;

    i32 activeColorCount;
    i32 computerCount;

    activeColorCount = 0;

    humanCount = 0;
    computerCount = 0;

    if (m_newGameInitialized && m_newGameHumanCount == giNumHumanPlayers) {
        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_setupPlayerNetworkId[player] == GAME_COMPUTER_PLAYER
                || m_setupPlayerNetworkId[player] >= giNumHumanPlayers) {
                m_setupPlayerRace[player] = m_mapHeader.playerRace[m_setupPlayerColor[player]];
            }
        }
    } else {
        m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
        if (header != NULL)
            m_mapHeader = *header;
        else
            GetMapHeader(m_mapFilename, &m_mapHeader);
        localization::SetCurrentFileTextEncoding(
            GetMapHeaderTextEncoding(&m_mapHeader)
        );

        for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
            if (m_mapHeader.playerEnabled[player]) {
                m_setupPlayerColor[activeColorCount] = static_cast<i8>(player);
                ++activeColorCount;
            }
        }

        for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
            if (player >= m_mapHeader.playerCount) {
                m_setupPlayerType[player] = GAME_NETWORK_PLAYER_NONE;
                m_setupPlayerNetworkId[player] = GAME_NETWORK_PLAYER_NONE;
                m_setupPlayerRace[player] = FACTION_ANY;
                m_playerHandicap[player] = PLAYER_HANDICAP_UNINITIALIZED;
            } else {
                m_playerHandicap[player] = PLAYER_HANDICAP_NONE;
                m_setupPlayerRace[player] = m_mapHeader.playerRace[m_setupPlayerColor[player]];
                m_setupPlayerNetworkId[player] = GAME_NETWORK_PLAYER_NONE;
                m_setupPlayerType[player] = GAME_NETWORK_PLAYER_NONE;
            }
        }

        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]
                && !m_mapHeader.playerCanComputer[m_setupPlayerColor[player]]) {
                m_setupPlayerType[player] = GAME_PLAYER_DEFAULT;
                m_setupPlayerNetworkId[player] = static_cast<i8>(humanCount);
                ++humanCount;
            } else if (!m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]
                       && m_mapHeader.playerCanComputer[m_setupPlayerColor[player]]) {
                m_setupPlayerNetworkId[player] = GAME_COMPUTER_PLAYER;
                m_setupPlayerType[player] = GAME_PLAYER_DEFAULT;
                ++computerCount;
            }
        }

        if (humanCount < giNumHumanPlayers
            && computerCount < m_mapHeader.playerCount - giNumHumanPlayers)
            playerType = GAME_PLAYER_FLEXIBLE;
        else
            playerType = GAME_PLAYER_DEFAULT;

        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_setupPlayerType[player] == GAME_NETWORK_PLAYER_NONE)
                m_setupPlayerType[player] = static_cast<i8>(playerType);
        }

        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_setupPlayerNetworkId[player] != GAME_NETWORK_PLAYER_NONE)
                continue;
            if (humanCount < giNumHumanPlayers
                && m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]) {
                m_setupPlayerNetworkId[player] = static_cast<i8>(humanCount);
                ++humanCount;
            } else {
                m_setupPlayerNetworkId[player] = GAME_COMPUTER_PLAYER;
            }
        }
        m_difficulty = DIFFICULTY_NORMAL;
        m_newGameInitialized = true;
    }

    m_selectedSetupPlayer = GAME_NETWORK_PLAYER_NONE;
}

void game::SetupNetPlayerNames(void) {
    i32 player;

    if (giNumHumanPlayers > 1) {
        for (player = 0; player < giNumHumanPlayers; ++player) {
            if (iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT)
                strcpy(cPlayerNames[player], gsNetPlayerInfo[player].name);
        }
    }
}

i32 game::NewGame(void) {
    char netPlayerPacket[GAME_PLAYER_INFO_BUFFER_SIZE];
    char mapInfo[GAME_MAP_PACKET_SIZE];
    tag_message windowMessage;
    b32 mapHeaderOk;
    b32 playerInfoOk;
    NewGameRemotePacket* remoteBuffer;
    heroWindow* choiceWindow;
    b32 result;
    b8 wrongExpansionType;
    char* mapExt;
    i32 mapHeaderRead;
    i32 textBufferIndex;
    i32 transmitResult;

    result = true;
    m_newGameWindow = NULL;

    if ((!gbRemoteOn || giThisNetPos == 0) && (!gbRemoteOn || !xNetHasOldPlayers)) {
        choiceWindow = new heroWindow(MAP_CHOICE_WINDOW_X, MAP_CHOICE_WINDOW_Y, "x_mapmnu.bin");
        if (choiceWindow == NULL)
            MemError();
        gpWindowManager->DoDialog(choiceWindow, ExpStdGameHandler, 0);
        delete choiceWindow;
        switch (NewGameMapChoiceFromCode(static_cast<i16>(gpWindowManager->m_dialogResult))) {
            case MAP_CHOICE_STANDARD:
                xIsExpansionMap = false;
                break;
            case MAP_CHOICE_EXPANSION:
                xIsExpansionMap = true;
                break;
            case MAP_CHOICE_CANCEL:
                return 0;
        }
    }

    SetupNetPlayerNames();
    glTimers[0] = 0;
    for (textBufferIndex = 0; textBufferIndex < GAME_TEXT_BUFFER_COUNT; ++textBufferIndex) {
        cTextReceivedBuffer[textBufferIndex] =
            static_cast<char*>(H2_ALLOC(GAME_TEXT_BUFFER_SIZE));
        strcpy(
            cTextReceivedBuffer[textBufferIndex],
            ""
        );
    }
    cNGKPCore = static_cast<char*>(H2_ALLOC(GAME_KEY_BUFFER_SIZE));
    cNGKPDisplay = static_cast<char*>(H2_ALLOC(GAME_KEY_BUFFER_SIZE));
    strcpy(cNGKPCore, "");
    strcpy(cNGKPDisplay, "");
    NGKPcursorIndex = 0;
    NGKPBkg = gpResourceManager->GetIcon("ngextra.icn");

    if (gbWaitForRemoteReceive) {
        mapHeaderOk = false;
        playerInfoOk = false;
        for (;;) {
            PollSound();
            remoteBuffer = reinterpret_cast<NewGameRemotePacket*>(GetRemoteData(1));
            if (remoteBuffer != NULL && remoteBuffer->type == REMOTE_MESSAGE_RELIABLE) {
                switch (remoteBuffer->command) {
                    case GAME_REMOTE_MAP_HEADER:
                        memset(&m_mapHeader, 0, sizeof(m_mapHeader));
                        memcpy(&m_mapHeader, remoteBuffer->payload, GAME_MAP_PACKET_SIZE);
                        mapHeaderOk = true;
                        break;
                    case GAME_REMOTE_PLAYER_INFO:
                        memcpy(
                            gsNetPlayerInfo,
                            remoteBuffer->payload,
                            GAME_PLAYER_INFO_PACKET_SIZE
                        );
                        SetupNetPlayerNames();
                        playerInfoOk = true;
                        break;
                }
                if (playerInfoOk && mapHeaderOk) {
                    m_newGameWindow =
                        new heroWindow(NEW_GAME_WINDOW_X, NEW_GAME_NETWORK_WINDOW_Y, "ngmp.bin");
                    InitNewGame(&m_mapHeader);
                    InitNewGameWindow();
                    UpdateNewGameWindow();

                    windowMessage.type = MESSAGE_WIDGET;
                    windowMessage.payload.widget.id = GAME_MAP_OPTIONS_CONTROL;
                    windowMessage.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
                    windowMessage.payload.widget.data.value = GAME_WIDGET_ACTIVE_FRAME;
                    m_newGameWindow->BroadcastMessage(windowMessage);
                    windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
                    windowMessage.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
                    m_newGameWindow->BroadcastMessage(windowMessage);
                    windowMessage.payload.widget.id = GAME_DIALOG_OK;
                    windowMessage.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
                    windowMessage.payload.widget.data.value = GAME_WIDGET_ACTIVE_FRAME;
                    m_newGameWindow->BroadcastMessage(windowMessage);
                    windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
                    windowMessage.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
                    m_newGameWindow->BroadcastMessage(windowMessage);
                    windowMessage.payload.widget.id = GAME_DIALOG_CANCEL;
                    windowMessage.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
                    windowMessage.payload.widget.data.value = GAME_WIDGET_ACTIVE_FRAME;
                    m_newGameWindow->BroadcastMessage(windowMessage);
                    windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
                    windowMessage.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
                    m_newGameWindow->BroadcastMessage(windowMessage);

                    gbNewGameDialogOver = false;
                    platform::StartTextInput();
                    gpWindowManager->DoDialog(m_newGameWindow, NewGameHandler, 0);
                    platform::StopTextInput();
                    delete m_newGameWindow;
                    if (gpWindowManager->m_dialogResult == GAME_DIALOG_CANCEL) {
                        result = false;
                        goto cleanup;
                    }
                    result = true;
                    goto cleanup;
                }
            }
        }
    } else {
        while (true) {
            wrongExpansionType = false;
            mapExt = FindLastToken(m_mapFilename, '.');
            if (mapExt != NULL) {
                if (StrEqNoCase(mapExt, ".MX2") && xIsExpansionMap)
                    wrongExpansionType = true;
                if (StrEqNoCase(mapExt, ".MP2") && !xIsExpansionMap)
                    wrongExpansionType = true;
            }
            if (!wrongExpansionType) {
                if (xIsExpansionMap)
                    strcpy(gpGame->m_mapFilename, "arrax.mx2");
                else
                    strcpy(gpGame->m_mapFilename, "brokena.mp2");
                m_newGameInitialized = false;
                m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
            }
            if (giNumHumanPlayers > BROKENA_MAX_HUMAN_PLAYERS
                && platform::CompareIgnoringCase(gpGame->m_mapFilename, "brokena.mp2") == 0)
                strcpy(gpGame->m_mapFilename, "slugfest.mp2");
            if (giNumHumanPlayers > 1
                && platform::CompareIgnoringCase(gpGame->m_mapFilename, "arrax.mx2") == 0)
                strcpy(gpGame->m_mapFilename, "fullhse.mx2");

            strcpy(gMapName, m_mapFilename);
            mapHeaderRead = GetMapHeader(m_mapFilename, &m_mapHeader);
            if (mapHeaderRead && giNumHumanPlayers >= m_mapHeader.minHumanPlayers
                && giNumHumanPlayers <= m_mapHeader.maxHumanPlayers)
                break;
            gpGame->GetMap();
        }

        if (gbRemoteOn) {
            memcpy(mapInfo, &gpGame->m_mapHeader, GAME_MAP_PACKET_SIZE);
            transmitResult = TransmitRemoteData(
                mapInfo,
                GAME_REMOTE_CHANNEL,
                GAME_MAP_PACKET_SIZE,
                GAME_REMOTE_MAP_HEADER,
                1,
                1,
                REMOTE_MESSAGE_DEFAULT
            );
            if (!transmitResult)
                ShutDown(NULL);
            memcpy(netPlayerPacket, gsNetPlayerInfo, GAME_PLAYER_INFO_PACKET_SIZE);
            transmitResult = TransmitRemoteData(
                netPlayerPacket,
                GAME_REMOTE_CHANNEL,
                GAME_PLAYER_INFO_PACKET_SIZE,
                GAME_REMOTE_PLAYER_INFO,
                1,
                1,
                REMOTE_MESSAGE_DEFAULT
            );
            if (!transmitResult)
                ShutDown(NULL);
        }

        LoadGame("origdata.bin", 1, 0);
        if (giNumHumanPlayers > 1) {
            if (iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
                m_newGameWindow =
                    new heroWindow(NEW_GAME_WINDOW_X, NEW_GAME_NETWORK_WINDOW_Y, "nghs.bin");
            else
                m_newGameWindow =
                    new heroWindow(NEW_GAME_WINDOW_X, NEW_GAME_NETWORK_WINDOW_Y, "ngmp.bin");
        } else {
            m_newGameWindow =
                new heroWindow(NEW_GAME_WINDOW_X, NEW_GAME_SINGLE_WINDOW_Y, "ngsp.bin");
        }
        if (m_newGameWindow == NULL)
            MemError();
        SetWinText(m_newGameWindow, NEW_GAME_WINDOW_TEXT_ID);
        InitNewGame(NULL);
        InitNewGameWindow();
        UpdateNewGameWindow();
        gbNewGameShadowHidden = false;
        gbNewGameDialogOver = false;
        const bool networkChatInput =
            giNumHumanPlayers > 1 && iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT;
        if (networkChatInput)
            platform::StartTextInput();
        gpWindowManager->DoDialog(m_newGameWindow, NewGameHandler, 0);
        if (networkChatInput)
            platform::StopTextInput();
        delete m_newGameWindow;
        if (gpWindowManager->m_dialogResult == GAME_DIALOG_CANCEL) {
            result = false;
        } else {
            m_playerCount = m_mapHeader.playerCount;
            NewMap(gMapName);
        }
    }

cleanup:
    for (textBufferIndex = 0; textBufferIndex < GAME_TEXT_BUFFER_COUNT; ++textBufferIndex) {
        H2_FREE(cTextReceivedBuffer[textBufferIndex]);
    }
    H2_FREE(cNGKPCore);
    H2_FREE(cNGKPDisplay);
    gpResourceManager->Dispose(NGKPBkg);
    return result;
}

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

    void game::InitNewGameWindow(void) {
        i32 columnGap;
        i32 availWidth;
        i32 firstColumnX;
        i32 playerStep;
        i32 playerCounter;
        char* name;
        i32 yExtra;
        i32 raceNameWidth;
        i32 multiplayerYOffset;
        widget* nameWidget;
        widget* iconControl;

        iconControl = NULL;
        nameWidget = NULL;
        availWidth = PLAYER_AREA_WIDTH - m_mapHeader.playerCount * PLAYER_COLUMN_WIDTH;
        columnGap = availWidth / (m_mapHeader.playerCount + 1);
        firstColumnX = columnGap + PLAYER_FIRST_X_OFFSET;
        playerStep = columnGap + PLAYER_COLUMN_WIDTH;
        multiplayerYOffset = 0;

        if (giNumHumanPlayers > 1 && iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT)
            multiplayerYOffset = PLAYER_MULTIPLAYER_Y_OFFSET;

        for (playerCounter = 0; playerCounter < m_mapHeader.playerCount; ++playerCounter) {
            if (giNumHumanPlayers > 1) {
                iconControl = new iconWidget(
                    static_cast<i16>(
                        firstColumnX + playerStep * playerCounter
                        + PLAYER_HUMAN_X_OFFSET
                    ),
                    static_cast<i16>(multiplayerYOffset + PLAYER_HUMAN_Y),
                    PLAYER_HUMAN_WIDTH,
                    PLAYER_HUMAN_HEIGHT,
                    "ngextra.icn",
                    PLAYER_HUMAN_FRAME,
                    ICON_DRAW_NORMAL,
                    static_cast<i16>(playerCounter + NEW_GAME_PLAYER_HUMAN_FIRST),
                    WIDGET_KIND_ICON_DIRECT,
                    PLAYER_WIDGET_FILL_COLOR
                );
                if (iconControl == NULL)
                    MemError();
                m_newGameWindow->AddWidget(iconControl, -1);

                iconControl = new iconWidget(
                    static_cast<i16>(
                        firstColumnX + playerStep * playerCounter
                        + PLAYER_HANDICAP_X_OFFSET
                    ),
                    static_cast<i16>(multiplayerYOffset + PLAYER_HANDICAP_Y),
                    PLAYER_HANDICAP_WIDTH,
                    PLAYER_HANDICAP_HEIGHT,
                    "ngextra.icn",
                    0,
                    ICON_DRAW_NORMAL,
                    static_cast<i16>(playerCounter + NEW_GAME_HANDICAP_FIRST),
                    WIDGET_KIND_ICON_DIRECT,
                    PLAYER_WIDGET_FILL_COLOR
                );
                if (iconControl == NULL)
                    MemError();
                m_newGameWindow->AddWidget(iconControl, -1);
            }

            iconControl = new iconWidget(
                static_cast<i16>(
                    firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
                ),
                PLAYER_RACE_Y,
                PLAYER_RACE_WIDTH,
                static_cast<i16>(
                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT
                ),
                "ngextra.icn",
                static_cast<i16>(
                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
                                          : GAME_RACE_WIDGET_SINGLE_FRAME
                ),
                ICON_DRAW_NORMAL,
                static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
                WIDGET_KIND_ICON_DIRECT,
                PLAYER_WIDGET_FILL_COLOR
            );
            if (iconControl == NULL)
                MemError();
            m_newGameWindow->AddWidget(iconControl, -1);

            iconControl = new iconWidget(
                static_cast<i16>(
                    firstColumnX + playerStep * playerCounter + PLAYER_SELECT_X_OFFSET
                ),
                PLAYER_SELECT_Y,
                PLAYER_SELECT_WIDTH,
                static_cast<i16>(
                    giNumHumanPlayers > 1 ? GAME_PLAYER_WIDGET_MULTIPLAYER_HEIGHT
                                          : GAME_PLAYER_WIDGET_SINGLE_HEIGHT
                ),
                "ngextra.icn",
                static_cast<i16>(
                    giNumHumanPlayers > 1 ? GAME_PLAYER_WIDGET_MULTIPLAYER_FRAME
                                          : GAME_PLAYER_WIDGET_SINGLE_FRAME
                ),
                ICON_DRAW_NORMAL,
                static_cast<i16>(playerCounter + NEW_GAME_PLAYER_SELECT_FIRST),
                WIDGET_KIND_ICON_DIRECT,
                PLAYER_WIDGET_FILL_COLOR
            );
            if (iconControl == NULL)
                MemError();
            m_newGameWindow->AddWidget(iconControl, -1);

            iconControl = new iconWidget(
                static_cast<i16>(
                    firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET
                ),
                PLAYER_COLOR_Y,
                PLAYER_COLOR_WIDTH,
                PLAYER_COLOR_HEIGHT,
                "ngextra.icn",
                static_cast<i16>(
                    giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
                                          : GAME_COLOR_WIDGET_SINGLE_FRAME
                ),
                ICON_DRAW_NORMAL,
                static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
                WIDGET_KIND_ICON_DIRECT,
                PLAYER_WIDGET_FILL_COLOR
            );
            if (iconControl == NULL)
                MemError();
            m_newGameWindow->AddWidget(iconControl, -1);

            if (giNumHumanPlayers > 1) {
                name = static_cast<char*>(H2_ALLOC(PLAYER_LABEL_CAPACITY));
                utf8::Copy(name, PLAYER_LABEL_CAPACITY, " ");
                nameWidget = new textWidget(
                    static_cast<i16>(
                        firstColumnX + playerStep * playerCounter + PLAYER_NAME_X_OFFSET
                    ),
                    PLAYER_NAME_Y,
                    PLAYER_NAME_WIDTH,
                    PLAYER_NAME_HEIGHT,
                    name,
                    "smalfont.fnt",
                    FONT_DRAW_DEFAULT,
                    static_cast<i16>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
                if (nameWidget == NULL)
                    MemError();
                m_newGameWindow->AddWidget(nameWidget, -1);
            }

            yExtra = 0;
            if (giNumHumanPlayers == 1)
                yExtra = PLAYER_SINGLE_Y_OFFSET;
            iconControl = new iconWidget(
                static_cast<i16>(
                    firstColumnX + playerStep * playerCounter
                    + PLAYER_RACE_ICON_X_OFFSET
                ),
                static_cast<i16>(
                    yExtra + multiplayerYOffset + PLAYER_RACE_ICON_Y
                ),
                PLAYER_RACE_ICON_WIDTH,
                PLAYER_RACE_ICON_HEIGHT,
                "ngextra.icn",
                PLAYER_RACE_ICON_FRAME,
                ICON_DRAW_NORMAL,
                static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
                WIDGET_KIND_ICON_DIRECT,
                PLAYER_WIDGET_FILL_COLOR
            );
            if (iconControl == NULL)
                MemError();
            m_newGameWindow->AddWidget(iconControl, -1);

            name = static_cast<char*>(
                H2_ALLOC(PLAYER_LABEL_CAPACITY)
            );
            utf8::Copy(name, PLAYER_LABEL_CAPACITY, "A");
            raceNameWidth = m_mapHeader.playerCount < PLAYER_RACE_NAME_NARROW_THRESHOLD
                                ? PLAYER_RACE_NAME_WIDE_WIDTH
                            : m_mapHeader.playerCount < PLAYER_RACE_NAME_HIDDEN_THRESHOLD
                                ? PLAYER_RACE_NAME_NARROW_WIDTH
                                : 0;
            nameWidget = new textWidget(
                static_cast<i16>(
                    firstColumnX + playerStep * playerCounter
                    + PLAYER_RACE_NAME_X_OFFSET - raceNameWidth / PLAYER_RACE_NAME_CENTER_DIVISOR
                ),
                static_cast<i16>(
                    yExtra + multiplayerYOffset + PLAYER_RACE_NAME_Y
                ),
                static_cast<i16>(raceNameWidth + PLAYER_RACE_NAME_BASE_WIDTH),
                PLAYER_RACE_NAME_HEIGHT,
                name,
                "smalfont.fnt",
                FONT_DRAW_DEFAULT,
                static_cast<i16>(playerCounter + NEW_GAME_RACE_NAME_FIRST),
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (nameWidget == NULL)
                MemError();
            m_newGameWindow->AddWidget(nameWidget, -1);

            iconControl = new iconWidget(
                static_cast<i16>(
                    firstColumnX + playerStep * playerCounter
                    + PLAYER_RACE_CYCLE_X_OFFSET
                ),
                static_cast<i16>(
                    yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y
                ),
                PLAYER_RACE_CYCLE_WIDTH,
                PLAYER_RACE_CYCLE_HEIGHT,
                "ngextra.icn",
                PLAYER_RACE_CYCLE_FRAME,
                ICON_DRAW_NORMAL,
                static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
                WIDGET_KIND_ICON_DIRECT,
                PLAYER_WIDGET_FILL_COLOR
            );
            if (iconControl == NULL)
                MemError();
            m_newGameWindow->AddWidget(iconControl, -1);
        }
    }

    void game::UpdateNewGameWindow(void) {
        b32 playerLockedValue;
        tag_message message;
        i32 playerIndex;

        const std::string mapName = localization::DecodeExternalText(
            m_mapHeader.name, GetMapHeaderTextEncoding(&m_mapHeader)
        );
        strcpy(gText, mapName.c_str());
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
        message.payload.widget.id = NEW_GAME_SCENARIO_NAME;
        message.payload.widget.data.text = gText;
        m_newGameWindow->BroadcastMessage(message);

        message.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        message.payload.widget.data.value = GAME_WIDGET_REFRESH_FRAME;
        for (playerIndex = 0; playerIndex < H2EnumIndex(DIFFICULTY_COUNT); ++playerIndex) {
            message.payload.widget.id = NEW_GAME_DIFFICULTY_FIRST + playerIndex;
            m_newGameWindow->BroadcastMessage(message);
        }
        message.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        message.payload.widget.id = NEW_GAME_DIFFICULTY_FIRST + H2EnumIndex(m_difficulty);
        m_newGameWindow->BroadcastMessage(message);

        if (giNumHumanPlayers > 1) {
            for (playerIndex = 0; playerIndex < GAME_CHAT_LINE_COUNT; ++playerIndex) {
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cTextReceivedBuffer[playerIndex]);
                message.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
                message.payload.widget.id = NEW_GAME_CHAT_FIRST + playerIndex;
                message.payload.widget.data.text = gText;
                m_newGameWindow->BroadcastMessage(message);
            }
        }

        for (playerIndex = 0; playerIndex < m_mapHeader.playerCount; ++playerIndex) {
            if (m_setupPlayerNetworkId[playerIndex] == GAME_COMPUTER_PLAYER) {
                gText[0] = 0;
            } else if (strlen(cPlayerNames[m_setupPlayerNetworkId[playerIndex]]) > 0) {
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cPlayerNames[m_setupPlayerNetworkId[playerIndex]]);
            } else {
                utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("player.number"), m_setupPlayerNetworkId[playerIndex] + 1);
            }
            message.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
            message.payload.widget.id = NEW_GAME_PLAYER_NAME_FIRST + playerIndex;
            message.payload.widget.data.text = gText;
            m_newGameWindow->BroadcastMessage(message);

            message.payload.widget.command = playerIndex == m_selectedSetupPlayer
                                                 ? NEW_GAME_WIDGET_ENABLE
                                                 : NEW_GAME_WIDGET_DISABLE;
            message.payload.widget.id = NEW_GAME_PLAYER_SELECT_FIRST + playerIndex;
            message.payload.widget.data.value = GAME_WIDGET_REFRESH_FRAME;
            m_newGameWindow->BroadcastMessage(message);

            if (m_setupPlayerType[playerIndex] != GAME_PLAYER_DEFAULT
                || (giNumHumanPlayers > 1
                    && m_setupPlayerNetworkId[playerIndex] != GAME_COMPUTER_PLAYER))
                playerLockedValue = false;
            else
                playerLockedValue = true;
            message.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
            message.payload.widget.id = NEW_GAME_COLOR_FIRST + playerIndex;
            if (m_setupPlayerNetworkId[playerIndex] == GAME_COMPUTER_PLAYER)
                message.payload.widget.data.value =
                    (playerLockedValue ? GAME_COMPUTER_COLOR_LOCKED_FRAME
                                       : GAME_COMPUTER_COLOR_UNLOCKED_FRAME)
                    + m_setupPlayerColor[playerIndex];
            else
                message.payload.widget.data.value =
                    (playerLockedValue ? GAME_HUMAN_COLOR_LOCKED_FRAME
                                       : GAME_HUMAN_COLOR_UNLOCKED_FRAME)
                    + m_setupPlayerColor[playerIndex];
            if (giNumHumanPlayers > 1)
                message.payload.widget.data.value += GAME_MULTIPLAYER_COLOR_FRAME_OFFSET;
            m_newGameWindow->BroadcastMessage(message);

            message.payload.widget.command =
                playerLockedValue ? NEW_GAME_WIDGET_DISABLE : NEW_GAME_WIDGET_ENABLE;
            message.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
            m_newGameWindow->BroadcastMessage(message);

            message.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
            message.payload.widget.id = NEW_GAME_HANDICAP_FIRST + playerIndex;
            if (m_setupPlayerNetworkId[playerIndex] == GAME_COMPUTER_PLAYER)
                message.payload.widget.data.value = NEW_GAME_RACE_NAME_FIRST;
            else
                message.payload.widget.data.value = H2EnumIndex(m_playerHandicap[playerIndex]);
            m_newGameWindow->BroadcastMessage(message);
            message.payload.widget.command =
                m_setupPlayerNetworkId[playerIndex] == GAME_COMPUTER_PLAYER
                    ? NEW_GAME_WIDGET_DISABLE
                    : NEW_GAME_WIDGET_ENABLE;
            message.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
            m_newGameWindow->BroadcastMessage(message);

            if (m_mapHeader.playerRace[m_setupPlayerColor[playerIndex]] == FACTION_RANDOM)
                playerLockedValue = false;
            else
                playerLockedValue = true;
            message.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
            message.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
            m_newGameWindow->BroadcastMessage(message);
            message.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
            message.payload.widget.id = NEW_GAME_RACE_CYCLE_FIRST + playerIndex;
            message.payload.widget.data.value =
                (playerLockedValue ? GAME_FIXED_RACE_FRAME_BASE : GAME_RANDOM_RACE_FRAME_BASE)
                + H2EnumIndex(m_setupPlayerRace[playerIndex]);
            m_newGameWindow->BroadcastMessage(message);

            utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, gAlignmentNames[H2EnumIndex(m_setupPlayerRace[playerIndex])]);
            message.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
            message.payload.widget.id = NEW_GAME_RACE_NAME_FIRST + playerIndex;
            message.payload.widget.data.text = gText;
            m_newGameWindow->BroadcastMessage(message);
            message.payload.widget.command =
                playerLockedValue ? NEW_GAME_WIDGET_DISABLE : NEW_GAME_WIDGET_ENABLE;
            message.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
            m_newGameWindow->BroadcastMessage(message);
        }

        gpGame->m_difficultyRating = static_cast<i16>(CalcDifficultyRating());
        message.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
        message.payload.widget.id = NEW_GAME_RATING;
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("new_game.rating"), gpGame->m_difficultyRating);
        message.payload.widget.data.text = gText;
        m_newGameWindow->BroadcastMessage(message);
        DrawNGKPDisplayString(0);
    }

    MessageDispatchResult NewGameHandler(struct tag_message& message) {
        i32 sendResult;
        i32 oldNetworkId;
        i32 swapPlayerTemp;
        i32 currentPlayerLocal;
        tag_message windowMessage;
        b32 redraw = false;
        b32 needSync = false;
        SMapHeader mapHeader;
        NewGameRemotePacket* remotePacketResult;
        i32 sender;
        char setupData[GAME_SETUP_BUFFER_SIZE];
        char mapPacketLocal[GAME_MAP_PACKET_SIZE];
        tag_message mapWindowMessageTemp;
        i32 helpDialogIndexLocal;

        char mapNamePacket[MAP_HEADER_NAME_SIZE];

        if (!gbNewGameShadowHidden) {
            gbNewGameShadowHidden = true;
            windowMessage.type = MESSAGE_WIDGET;
            windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
            windowMessage.payload.widget.id = NEW_GAME_SHADOW;
            windowMessage.payload.widget.data.value = GAME_SHADOW_FRAME;
            gpGame->m_newGameWindow->BroadcastMessage(windowMessage);
        }

        if (message.type == MESSAGE_NONE) {
            remotePacketResult = reinterpret_cast<NewGameRemotePacket*>(GetRemoteData(1));
            if (remotePacketResult != NULL
                && (remotePacketResult->type == REMOTE_MESSAGE_RELIABLE
                    || remotePacketResult->type == REMOTE_MESSAGE_UNRELIABLE)) {
                switch (remotePacketResult->command) {
                    case GAME_REMOTE_START:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        gpWindowManager->m_dialogResult = GAME_DIALOG_OK;
                        message.type = MESSAGE_WIDGET;
                        message.payload.widget.id = GAME_DIALOG_CLOSE_MESSAGE;
                        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
                        return MESSAGE_DISPATCH_FORWARD;

                    case GAME_REMOTE_CANCEL:
                        NormalDialog(
                            localization::Tr("network.host.canceled_game"),
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
                        ShutDown(NULL);
                        break;

                    case GAME_REMOTE_SETUP:
                        if (strcmp(remotePacketResult->payload, gpGame->m_mapHeader.name) != 0)
                            break;
                        memcpy(
                            gpGame->m_setupPlayerColor,
                            remotePacketResult->payload + MAP_HEADER_NAME_SIZE,
                            GAME_SETUP_DATA_SIZE
                        );
                        redraw = true;
                        break;

                    case GAME_REMOTE_MAP_HEADER:
                        memset(&mapHeader, 0, sizeof(mapHeader));
                        memcpy(
                            &mapHeader,
                            remotePacketResult->payload,
                            GAME_MAP_PACKET_SIZE
                        );
                        gpGame->ProcessNewMap(&mapHeader);
                        break;

                    case GAME_REMOTE_CHAT:
                        redraw = true;
                        sender = remotePacketResult->sender;
                        if (sender >= 0) {
                            utf8::Format(
                                gText, GLOBAL_TEXT_BUFFER_SIZE,
                                "%s:  %s",
                                gsNetPlayerInfo[sender].name,
                                remotePacketResult->payload
                            );
                        } else {

                        }
                        gText[GAME_CHAT_TEXT_LIMIT] = 0;
                        for (currentPlayerLocal = 0; currentPlayerLocal < GAME_CHAT_LINE_COUNT - 1;
                             ++currentPlayerLocal) {
                            strcpy(
                                cTextReceivedBuffer[currentPlayerLocal],
                                cTextReceivedBuffer[currentPlayerLocal + 1]
                            );
                        }
                        strcpy(cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1], gText);
                        break;
                }
            }
            if (static_cast<i32>(platform::Ticks()) > glTimers[0]) {
                gpGame->NGKPSetupDisplayString(cNGKPCore, static_cast<u16>(NGKPcursorIndex));
                gpGame->DrawNGKPDisplayString(1);
            }
        }

        if ((message.type == MESSAGE_KEY_DOWN || message.type == MESSAGE_TEXT_INPUT)
            && giNumHumanPlayers > 1
            && iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT && gpGame->ProcessNGKeyPress(message)) {
            redraw = true;
            for (currentPlayerLocal = 0; currentPlayerLocal < GAME_CHAT_LINE_COUNT - 1;
                 ++currentPlayerLocal) {
                strcpy(
                    cTextReceivedBuffer[currentPlayerLocal],
                    cTextReceivedBuffer[currentPlayerLocal + 1]
                );
            }
            strcpy(cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1], cNGKPCore);
            strcpy(
                cNGKPCore,
                ""
            );
            strcpy(
                cNGKPDisplay,
                ""
            );
            NGKPcursorIndex = 0;
            sendResult = TransmitRemoteData(
                cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1],
                GAME_REMOTE_CHANNEL,
                strlen(cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1]) + 1,
                GAME_REMOTE_CHAT,
                1,
                1,
                REMOTE_MESSAGE_DEFAULT
            );
            if (!sendResult)
                ShutDown(NULL);
        }

        if (message.type == MESSAGE_WIDGET) {
            if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                if (message.payload.widget.command == NEW_GAME_EVENT_PRESS
                    || message.payload.widget.command == NEW_GAME_EVENT_ALTERNATE_PRESS) {
                    helpDialogIndexLocal = -1;
                    if ((message.payload.widget.id >= NEW_GAME_DIFFICULTY_HELP_FIRST
                         && message.payload.widget.id
                                <= NEW_GAME_DIFFICULTY_HELP_FIRST + H2EnumIndex(DIFFICULTY_COUNT) - 1)
                        || (message.payload.widget.id >= NEW_GAME_DIFFICULTY_FIRST
                            && message.payload.widget.id
                                   <= NEW_GAME_DIFFICULTY_FIRST + H2EnumIndex(DIFFICULTY_COUNT) - 1))
                        helpDialogIndexLocal = GAME_HELP_DIFFICULTY;
                    if ((message.payload.widget.id >= NEW_GAME_HANDICAP_FIRST
                         && message.payload.widget.id
                                <= NEW_GAME_HANDICAP_FIRST + H2EnumIndex(GAME_PLAYER_CONTROL_COUNT) - 1)
                        || (message.payload.widget.id >= NEW_GAME_PLAYER_HUMAN_FIRST
                            && message.payload.widget.id <= NEW_GAME_PLAYER_HUMAN_FIRST
                                                                + H2EnumIndex(GAME_PLAYER_CONTROL_COUNT)
                                                                - 1))
                        helpDialogIndexLocal = GAME_HELP_HANDICAP;
                    if ((message.payload.widget.id >= NEW_GAME_COLOR_FIRST
                         && message.payload.widget.id
                                <= NEW_GAME_COLOR_FIRST + H2EnumIndex(GAME_PLAYER_CONTROL_COUNT) - 1)
                        || (message.payload.widget.id >= NEW_GAME_RACE_FIRST
                            && message.payload.widget.id
                                   <= NEW_GAME_RACE_FIRST + H2EnumIndex(GAME_PLAYER_CONTROL_COUNT) - 1)
                        || (message.payload.widget.id >= NEW_GAME_PLAYER_SELECT_FIRST
                            && message.payload.widget.id <= NEW_GAME_PLAYER_NAME_FIRST)
                        || (message.payload.widget.id >= NEW_GAME_PLAYER_NAME_FIRST
                            && message.payload.widget.id <= NEW_GAME_RACE_ICON_FIRST))
                        helpDialogIndexLocal = GAME_HELP_PLAYER;
                    if ((message.payload.widget.id >= NEW_GAME_RACE_CYCLE_FIRST
                         && message.payload.widget.id
                                <= NEW_GAME_RACE_CYCLE_FIRST + H2EnumIndex(GAME_PLAYER_CONTROL_COUNT) - 1)
                        || (message.payload.widget.id >= NEW_GAME_RACE_ICON_FIRST
                            && message.payload.widget.id <= NEW_GAME_RACE_ICON_FIRST
                                                                + H2EnumIndex(GAME_PLAYER_CONTROL_COUNT)
                                                                - 1))
                        helpDialogIndexLocal = GAME_HELP_RACE;
                    if (message.payload.widget.id == GAME_MAP_OPTIONS_CONTROL
                        || message.payload.widget.id == NEW_GAME_MAP_SELECT
                        || message.payload.widget.id == NEW_GAME_SCENARIO_NAME)
                        helpDialogIndexLocal = GAME_HELP_MAP;
                    if (message.payload.widget.id == NEW_GAME_RATING)
                        helpDialogIndexLocal = GAME_HELP_RATING;
                    if (message.payload.widget.id == GAME_DIALOG_OK)
                        helpDialogIndexLocal = GAME_HELP_OK;
                    if (message.payload.widget.id == GAME_DIALOG_CANCEL)
                        helpDialogIndexLocal = GAME_HELP_CANCEL;
                    if (helpDialogIndexLocal != -1)
                        NormalDialog(
                            gNewGameHelp[helpDialogIndexLocal],
                            NEW_GAME_HELP_DIALOG_TYPE,
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
            } else {
                switch (message.payload.widget.command) {
                    case NEW_GAME_EVENT_RELEASE:
                        switch (message.payload.widget.id) {
                            case GAME_DIALOG_OK:
                                if (gbRemoteOn) {
                                    sendResult = TransmitRemoteData(
                                        NULL,
                                        GAME_REMOTE_CHANNEL,
                                        0,
                                        GAME_REMOTE_START,
                                        1,
                                        1,
                                        REMOTE_MESSAGE_DEFAULT
                                    );
                                }
                                gpWindowManager->m_dialogResult = message.payload.widget.id;
                                message.payload.widget.id = GAME_DIALOG_CLOSE_MESSAGE;
                                message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
                                gbNewGameDialogOver = true;
                                return MESSAGE_DISPATCH_FORWARD;

                            case GAME_DIALOG_CANCEL:
                                if (gbRemoteOn) {
                                    sendResult = TransmitRemoteData(
                                        NULL,
                                        GAME_REMOTE_CHANNEL,
                                        0,
                                        GAME_REMOTE_CANCEL,
                                        1,
                                        1,
                                        REMOTE_MESSAGE_DEFAULT
                                    );
                                    ShutDown(NULL);
                                }
                                gpWindowManager->m_dialogResult = message.payload.widget.id;
                                message.payload.widget.id = GAME_DIALOG_CLOSE_MESSAGE;
                                message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
                                gbNewGameDialogOver = true;
                                return MESSAGE_DISPATCH_FORWARD;

                            case GAME_MAP_OPTIONS_CONTROL:
                                goto chooseMap;

                            default:
                                break;
                        }
                        break;

                    case NEW_GAME_EVENT_PRESS:
                        switch (message.payload.widget.id) {
                            case NEW_GAME_DIFFICULTY_HELP_FIRST + H2EnumIndex(DIFFICULTY_EASY):
                            case NEW_GAME_DIFFICULTY_HELP_FIRST + H2EnumIndex(DIFFICULTY_NORMAL):
                            case NEW_GAME_DIFFICULTY_HELP_FIRST + H2EnumIndex(DIFFICULTY_HARD):
                            case NEW_GAME_DIFFICULTY_HELP_FIRST + H2EnumIndex(DIFFICULTY_EXPERT):
                            case NEW_GAME_DIFFICULTY_HELP_FIRST + H2EnumIndex(DIFFICULTY_IMPOSSIBLE):
                                currentPlayerLocal =
                                    message.payload.widget.id - NEW_GAME_DIFFICULTY_HELP_FIRST;
                                goto setDifficulty;

                            case NEW_GAME_DIFFICULTY_FIRST + H2EnumIndex(DIFFICULTY_EASY):
                            case NEW_GAME_DIFFICULTY_FIRST + H2EnumIndex(DIFFICULTY_NORMAL):
                            case NEW_GAME_DIFFICULTY_FIRST + H2EnumIndex(DIFFICULTY_HARD):
                            case NEW_GAME_DIFFICULTY_FIRST + H2EnumIndex(DIFFICULTY_EXPERT):
                            case NEW_GAME_DIFFICULTY_FIRST + H2EnumIndex(DIFFICULTY_IMPOSSIBLE):
                                currentPlayerLocal =
                                    message.payload.widget.id - NEW_GAME_DIFFICULTY_FIRST;
                            setDifficulty:
                                gpGame->m_difficulty =
                                    GameDifficultyFromCode(currentPlayerLocal);
                                needSync = true;
                                redraw = true;
                                break;

                            case NEW_GAME_HANDICAP_FIRST + H2EnumIndex(PLAYER_SLOT_FIRST):
                            case NEW_GAME_HANDICAP_FIRST + H2EnumIndex(PLAYER_SLOT_SECOND):
                            case NEW_GAME_HANDICAP_FIRST + H2EnumIndex(PLAYER_SLOT_THIRD):
                            case NEW_GAME_HANDICAP_FIRST + H2EnumIndex(PLAYER_SLOT_FOURTH):
                            case NEW_GAME_HANDICAP_FIRST + H2EnumIndex(PLAYER_SLOT_FIFTH):
                            case NEW_GAME_HANDICAP_FIRST + H2EnumIndex(PLAYER_SLOT_SIXTH):
                                currentPlayerLocal =
                                    message.payload.widget.id - NEW_GAME_HANDICAP_FIRST;
                                goto cycleHandicap;

                            case NEW_GAME_PLAYER_HUMAN_FIRST + H2EnumIndex(PLAYER_SLOT_FIRST):
                            case NEW_GAME_PLAYER_HUMAN_FIRST + H2EnumIndex(PLAYER_SLOT_SECOND):
                            case NEW_GAME_PLAYER_HUMAN_FIRST + H2EnumIndex(PLAYER_SLOT_THIRD):
                            case NEW_GAME_PLAYER_HUMAN_FIRST + H2EnumIndex(PLAYER_SLOT_FOURTH):
                            case NEW_GAME_PLAYER_HUMAN_FIRST + H2EnumIndex(PLAYER_SLOT_FIFTH):
                            case NEW_GAME_PLAYER_HUMAN_FIRST + H2EnumIndex(PLAYER_SLOT_SIXTH):
                                currentPlayerLocal =
                                    message.payload.widget.id - NEW_GAME_PLAYER_HUMAN_FIRST;
                            cycleHandicap:
                                needSync = true;
                                redraw = true;
                                if (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
                                    != GAME_COMPUTER_PLAYER) {
                                    gpGame->m_playerHandicap[currentPlayerLocal] = PlayerHandicap(
                                        (H2EnumIndex(gpGame->m_playerHandicap[currentPlayerLocal]) + 1)
                                        % H2EnumIndex(PLAYER_HANDICAP_COUNT)
                                    );
                                }
                                break;

                            case NEW_GAME_RACE_FIRST + H2EnumIndex(PLAYER_SLOT_FIRST):
                            case NEW_GAME_RACE_FIRST + H2EnumIndex(PLAYER_SLOT_SECOND):
                            case NEW_GAME_RACE_FIRST + H2EnumIndex(PLAYER_SLOT_THIRD):
                            case NEW_GAME_RACE_FIRST + H2EnumIndex(PLAYER_SLOT_FOURTH):
                            case NEW_GAME_RACE_FIRST + H2EnumIndex(PLAYER_SLOT_FIFTH):
                            case NEW_GAME_RACE_FIRST + H2EnumIndex(PLAYER_SLOT_SIXTH):
                                currentPlayerLocal =
                                    message.payload.widget.id - NEW_GAME_RACE_FIRST;
                                goto selectPlayer;

                            case NEW_GAME_COLOR_FIRST + H2EnumIndex(PLAYER_SLOT_FIRST):
                            case NEW_GAME_COLOR_FIRST + H2EnumIndex(PLAYER_SLOT_SECOND):
                            case NEW_GAME_COLOR_FIRST + H2EnumIndex(PLAYER_SLOT_THIRD):
                            case NEW_GAME_COLOR_FIRST + H2EnumIndex(PLAYER_SLOT_FOURTH):
                            case NEW_GAME_COLOR_FIRST + H2EnumIndex(PLAYER_SLOT_FIFTH):
                            case NEW_GAME_COLOR_FIRST + H2EnumIndex(PLAYER_SLOT_SIXTH):
                                currentPlayerLocal =
                                    message.payload.widget.id - NEW_GAME_COLOR_FIRST;
                                goto selectPlayer;

                            case NEW_GAME_PLAYER_SELECT_FIRST + H2EnumIndex(PLAYER_SLOT_FIRST):
                            case NEW_GAME_PLAYER_SELECT_FIRST + H2EnumIndex(PLAYER_SLOT_SECOND):
                            case NEW_GAME_PLAYER_SELECT_FIRST + H2EnumIndex(PLAYER_SLOT_THIRD):
                            case NEW_GAME_PLAYER_SELECT_FIRST + H2EnumIndex(PLAYER_SLOT_FOURTH):
                            case NEW_GAME_PLAYER_SELECT_FIRST + H2EnumIndex(PLAYER_SLOT_FIFTH):
                            case NEW_GAME_PLAYER_SELECT_FIRST + H2EnumIndex(PLAYER_SLOT_SIXTH):
                                currentPlayerLocal =
                                    message.payload.widget.id - NEW_GAME_PLAYER_SELECT_FIRST;
                                goto selectPlayer;

                            case NEW_GAME_PLAYER_NAME_FIRST + H2EnumIndex(PLAYER_SLOT_FIRST):
                            case NEW_GAME_PLAYER_NAME_FIRST + H2EnumIndex(PLAYER_SLOT_SECOND):
                            case NEW_GAME_PLAYER_NAME_FIRST + H2EnumIndex(PLAYER_SLOT_THIRD):
                            case NEW_GAME_PLAYER_NAME_FIRST + H2EnumIndex(PLAYER_SLOT_FOURTH):
                            case NEW_GAME_PLAYER_NAME_FIRST + H2EnumIndex(PLAYER_SLOT_FIFTH):
                            case NEW_GAME_PLAYER_NAME_FIRST + H2EnumIndex(PLAYER_SLOT_SIXTH):
                                currentPlayerLocal =
                                    message.payload.widget.id - NEW_GAME_PLAYER_NAME_FIRST;
                            selectPlayer:
                                needSync = true;
                                redraw = true;
                                if (gpGame->m_setupPlayerType[currentPlayerLocal]
                                        != GAME_PLAYER_DEFAULT
                                    || (giNumHumanPlayers > 1
                                        && gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
                                               != GAME_COMPUTER_PLAYER)) {
                                    if (giNumHumanPlayers == 1) {
                                        if (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
                                            == GAME_COMPUTER_PLAYER) {
                                            for (swapPlayerTemp = 0;
                                                 swapPlayerTemp < gpGame->m_mapHeader.playerCount;
                                                 ++swapPlayerTemp) {
                                                if (gpGame->m_setupPlayerNetworkId[swapPlayerTemp]
                                                    != GAME_COMPUTER_PLAYER) {
                                                    oldNetworkId = gpGame->m_setupPlayerNetworkId
                                                                       [swapPlayerTemp];
                                                    gpGame->m_setupPlayerNetworkId[swapPlayerTemp] =
                                                        gpGame->m_setupPlayerNetworkId
                                                            [currentPlayerLocal];
                                                    gpGame->m_setupPlayerNetworkId
                                                        [currentPlayerLocal] = oldNetworkId;
                                                    swapPlayerTemp = GAME_SWAP_SEARCH_DONE;
                                                }
                                            }
                                        }
                                    } else if (gpGame->m_selectedSetupPlayer
                                               == GAME_NETWORK_PLAYER_NONE) {
                                        gpGame->m_selectedSetupPlayer =
                                            static_cast<i8>(currentPlayerLocal);
                                    } else if (gpGame->m_selectedSetupPlayer == currentPlayerLocal
                                               || (gpGame->m_setupPlayerNetworkId
                                                           [currentPlayerLocal]
                                                       == GAME_COMPUTER_PLAYER
                                                   && gpGame->m_setupPlayerNetworkId
                                                              [gpGame->m_selectedSetupPlayer]
                                                          == GAME_COMPUTER_PLAYER)) {
                                        gpGame->m_selectedSetupPlayer = GAME_NETWORK_PLAYER_NONE;
                                    } else {
                                        if ((gpGame->m_setupPlayerType[currentPlayerLocal]
                                                 != GAME_PLAYER_DEFAULT
                                             && gpGame->m_setupPlayerType
                                                        [gpGame->m_selectedSetupPlayer]
                                                    != GAME_PLAYER_DEFAULT)
                                            || (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
                                                    != GAME_COMPUTER_PLAYER
                                                && gpGame->m_setupPlayerNetworkId
                                                           [gpGame->m_selectedSetupPlayer]
                                                       != GAME_COMPUTER_PLAYER)) {
                                            swapPlayerTemp =
                                                gpGame->m_setupPlayerNetworkId[currentPlayerLocal];
                                            gpGame->m_setupPlayerNetworkId[currentPlayerLocal] =
                                                gpGame->m_setupPlayerNetworkId
                                                    [gpGame->m_selectedSetupPlayer];
                                            gpGame->m_setupPlayerNetworkId
                                                [gpGame->m_selectedSetupPlayer] =
                                                static_cast<i8>(swapPlayerTemp);
                                        } else {
                                            NormalDialog(
                                                localization::Tr("new_game.positions.cannot_swap"),
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
                                        }
                                        gpGame->m_selectedSetupPlayer = GAME_NETWORK_PLAYER_NONE;
                                    }
                                }
                                break;

                            case NEW_GAME_RACE_CYCLE_FIRST + H2EnumIndex(PLAYER_SLOT_FIRST):
                            case NEW_GAME_RACE_CYCLE_FIRST + H2EnumIndex(PLAYER_SLOT_SECOND):
                            case NEW_GAME_RACE_CYCLE_FIRST + H2EnumIndex(PLAYER_SLOT_THIRD):
                            case NEW_GAME_RACE_CYCLE_FIRST + H2EnumIndex(PLAYER_SLOT_FOURTH):
                            case NEW_GAME_RACE_CYCLE_FIRST + H2EnumIndex(PLAYER_SLOT_FIFTH):
                            case NEW_GAME_RACE_CYCLE_FIRST + H2EnumIndex(PLAYER_SLOT_SIXTH):
                                currentPlayerLocal =
                                    message.payload.widget.id - NEW_GAME_RACE_CYCLE_FIRST;
                                goto cycleRace;

                            case NEW_GAME_RACE_ICON_FIRST + H2EnumIndex(PLAYER_SLOT_FIRST):
                            case NEW_GAME_RACE_ICON_FIRST + H2EnumIndex(PLAYER_SLOT_SECOND):
                            case NEW_GAME_RACE_ICON_FIRST + H2EnumIndex(PLAYER_SLOT_THIRD):
                            case NEW_GAME_RACE_ICON_FIRST + H2EnumIndex(PLAYER_SLOT_FOURTH):
                            case NEW_GAME_RACE_ICON_FIRST + H2EnumIndex(PLAYER_SLOT_FIFTH):
                            case NEW_GAME_RACE_ICON_FIRST + H2EnumIndex(PLAYER_SLOT_SIXTH):
                                currentPlayerLocal =
                                    message.payload.widget.id - NEW_GAME_RACE_ICON_FIRST;
                            cycleRace:
                                if (gpGame->m_mapHeader
                                        .playerRace[gpGame->m_setupPlayerColor[currentPlayerLocal]]
                                    == FACTION_RANDOM) {
                                    if (gpGame->m_setupPlayerRace[currentPlayerLocal]
                                        == FACTION_RANDOM)
                                        gpGame->m_setupPlayerRace[currentPlayerLocal] =
                                            FACTION_KNIGHT;
                                    else if (gpGame->m_setupPlayerRace[currentPlayerLocal]
                                             == FACTION_NECROMANCER)
                                        gpGame->m_setupPlayerRace[currentPlayerLocal] =
                                            FACTION_RANDOM;
                                    else
                                        ++gpGame->m_setupPlayerRace[currentPlayerLocal];
                                    needSync = true;
                                    redraw = true;
                                }
                                break;

                            case GAME_MAP_OPTIONS_CONTROL:
                                break;

                            case NEW_GAME_SCENARIO_NAME:
                            case NEW_GAME_MAP_SELECT:
                            chooseMap:
                                if (gbRemoteOn && giThisNetPos != 0)
                                    break;
                                {
                                    mapWindowMessageTemp.type = MESSAGE_WIDGET;
                                    mapWindowMessageTemp.payload.widget.command =
                                        NEW_GAME_WIDGET_DISABLE;
                                    mapWindowMessageTemp.payload.widget.id = GAME_DIALOG_CANCEL;
                                    mapWindowMessageTemp.payload.widget.data.value =
                                        GAME_WIDGET_INACTIVE_FRAME;
                                    gpGame->m_newGameWindow->BroadcastMessage(mapWindowMessageTemp);
                                    gpGame->GetMap();
                                    mapWindowMessageTemp.type = MESSAGE_WIDGET;
                                    mapWindowMessageTemp.payload.widget.command =
                                        NEW_GAME_WIDGET_ENABLE;
                                    mapWindowMessageTemp.payload.widget.id = GAME_DIALOG_CANCEL;
                                    mapWindowMessageTemp.payload.widget.data.value =
                                        GAME_WIDGET_INACTIVE_FRAME;
                                    gpGame->m_newGameWindow->BroadcastMessage(mapWindowMessageTemp);
                                    if (gbRemoteOn) {
                                        memcpy(
                                            mapPacketLocal,
                                            &gpGame->m_mapHeader,
                                            GAME_MAP_PACKET_SIZE
                                        );
                                        sendResult = TransmitRemoteData(
                                            mapPacketLocal,
                                            GAME_REMOTE_CHANNEL,
                                            GAME_MAP_PACKET_SIZE,
                                            GAME_REMOTE_MAP_HEADER,
                                            1,
                                            1,
                                            REMOTE_MESSAGE_DEFAULT
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
            }
        }

    if (redraw) {
        gpGame->UpdateNewGameWindow();
        gpGame->m_newGameWindow->DrawWindow();
    }
    if (needSync && gbRemoteOn) {
        memcpy(mapNamePacket, gpGame->m_mapHeader.name, MAP_HEADER_NAME_SIZE);
        memcpy(setupData, gpGame->m_setupPlayerColor, GAME_SETUP_DATA_SIZE);
        sendResult = TransmitRemoteData(
            mapNamePacket,
            GAME_REMOTE_CHANNEL,
            GAME_SETUP_PACKET_SIZE,
            GAME_REMOTE_SETUP,
            1,
            1,
            REMOTE_MESSAGE_DEFAULT
        );
        if (!sendResult)
            ShutDown(NULL);
    }
    return MESSAGE_DISPATCH_CONSUME;
}

i32 game::ProcessNGKeyPress(struct tag_message& message) {
    char workText[GAME_KEY_BUFFER_SIZE];
    i32 widthResult;

    if (giNumHumanPlayers == 1 || iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
        return 0;

    if (message.type == MESSAGE_TEXT_INPUT) {
        const std::uint32_t codePoint =
            static_cast<std::uint32_t>(message.payload.keyboard.keyCode);
        if (codePoint < ' ' || codePoint == '{' || codePoint == '}')
            return 0;

        char encoded[4];
        const std::size_t encodedLength = utf8::Encode(codePoint, encoded);
        const std::size_t textLength = strlen(cNGKPCore);
        if (encodedLength == 0 || textLength + encodedLength >= GAME_CHAT_TEXT_LIMIT
            || textLength + encodedLength >= GAME_KEY_BUFFER_SIZE)
            return 0;

        strcpy(workText, cNGKPCore);
        memmove(
            cNGKPCore + NGKPcursorIndex + encodedLength,
            cNGKPCore + NGKPcursorIndex,
            textLength - NGKPcursorIndex + 1
        );
        memcpy(cNGKPCore + NGKPcursorIndex, encoded, encodedLength);
        const i32 oldCursor = NGKPcursorIndex;
        NGKPcursorIndex += static_cast<i32>(encodedLength);
        NGKPSetupDisplayString(cNGKPCore, static_cast<u16>(NGKPcursorIndex));
        widthResult = smallFont->LineLength(cNGKPDisplay, GAME_CHAT_DRAW_WIDTH);
        if (widthResult > GAME_CHAT_MAX_LINES) {
            strcpy(cNGKPCore, workText);
            NGKPcursorIndex = oldCursor;
        }
        return 0;
    }

    switch (message.payload.keyboard.keyCode) {
        case INPUT_SCAN_ESCAPE:
            if (!gbAllowTextEntryEscape)
                break;
            strcpy(
                cNGKPCore,
                ""
            );
            break;

        case INPUT_SCAN_NUMPAD_DELETE:
            if (static_cast<std::size_t>(NGKPcursorIndex) < strlen(cNGKPCore)) {
                const std::size_t next = utf8::Next(cNGKPCore, NGKPcursorIndex);
                strcpy(gText, cNGKPCore + next);
                strcpy(cNGKPCore + NGKPcursorIndex, gText);
            }
            break;

        case INPUT_SCAN_NUMPAD_4:
            if (NGKPcursorIndex > 0) {
                NGKPcursorIndex = static_cast<i32>(
                    utf8::Previous(cNGKPCore, NGKPcursorIndex)
                );
            }
            break;

        case INPUT_SCAN_NUMPAD_6:
            if (static_cast<std::size_t>(NGKPcursorIndex) < strlen(cNGKPCore)) {
                NGKPcursorIndex = static_cast<i32>(
                    utf8::Next(cNGKPCore, NGKPcursorIndex)
                );
            }
            break;

        case INPUT_SCAN_ENTER:
            return 1;

        case INPUT_SCAN_BACKSPACE:
            if (NGKPcursorIndex > 0) {
                const std::size_t previous = utf8::Previous(cNGKPCore, NGKPcursorIndex);
                strcpy(gText, cNGKPCore + NGKPcursorIndex);
                strcpy(cNGKPCore + previous, gText);
                NGKPcursorIndex = static_cast<i32>(previous);
            }
            break;

        default:
            break;
    }

    DrawNGKPDisplayString(1);
    return 0;
}

void game::NGKPSetupDisplayString(char* text, u16 cursor) {
    if (giNumHumanPlayers == 1 || iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
        return;

    if (static_cast<i32>(platform::Ticks()) > glTimers[0]) {
        NGKPcursorFlashOn = 1 - NGKPcursorFlashOn;
        glTimers[0] = platform::Ticks() + GAME_CURSOR_FLASH_TICKS;
    }

    if (cursor > 0)
        strncpy(cNGKPDisplay, text, cursor);

    if (NGKPcursorFlashOn)
        cNGKPDisplay[cursor] = FONT_SPACER_CHAR;
    else
        cNGKPDisplay[cursor] = '_';

    if (strlen(text) > cursor)
        strcpy(cNGKPDisplay + (cursor + 1), text + cursor);
    else
        cNGKPDisplay[cursor + 1] = 0;
}

void game::DrawNGKPDisplayString(i32 updateScreen) {
    if (gbNewGameDialogOver != 0)
        return;

    if (giNumHumanPlayers == 1 || iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
        return;

    NGKPBkg->DrawToBuffer(
        GAME_CHAT_DRAW_X,
        GAME_CHAT_DRAW_Y,
        GAME_CHAT_BACKGROUND_WIDTH,
        ICON_DRAW_NORMAL
    );
    smallFont->DrawBoundedString(
        cNGKPDisplay,
        GAME_CHAT_DRAW_X,
        GAME_CHAT_DRAW_Y,
        GAME_CHAT_DRAW_WIDTH,
        GAME_CHAT_DRAW_HEIGHT,
        FONT_DRAW_YELLOW,
        FONT_ALIGN_LEFT
    );
    if (updateScreen)
        gpWindowManager->UpdateScreenRegion(
            GAME_CHAT_DRAW_X,
            GAME_CHAT_DRAW_Y,
            GAME_CHAT_DRAW_WIDTH,
            GAME_CHAT_DRAW_HEIGHT
        );
}

void game::ShowScenInfo(void) {
    i32 columnGap;
    i32 availWidth;
    i32 firstColumnX;
    i32 playerStep;
    i32 playerCounter;
    i32 mapSize;
    b32 locked;
    char* name;
    i32 yExtra;
    i32 raceNameWidth;
    tag_message msg;
    i32 multiplayerYOffset;
    widget* nameWidget;
    widget* iconControl;
    heroWindow* window;
    const localization::TextEncoding mapEncoding = GetMapHeaderTextEncoding(&m_mapHeader);
    const std::string mapName = localization::DecodeExternalText(
        m_mapHeader.name, mapEncoding
    );
    const std::string mapDescription =
        localization::DecodeExternalText(m_mapHeader.description, mapEncoding);

    gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
    window = new heroWindow(SCENARIO_WINDOW_X, SCENARIO_WINDOW_Y, "sceninfo.bin");
    if (window == NULL)
        MemError();
    SetWinText(window, GAME_SCENARIO_WINDOW_TEXT_ID);

    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
    msg.payload.widget.id = NEW_GAME_SCENARIO_NAME;
    msg.payload.widget.data.text = mapName.c_str();
    window->BroadcastMessage(msg);

    msg.payload.widget.id = GAME_SCENARIO_DIFFICULTY;
    msg.payload.widget.data.text = cDifficulty[H2EnumIndex(m_mapHeader.difficulty)];
    window->BroadcastMessage(msg);
    msg.payload.widget.id = GAME_SCENARIO_SELECTED_DIFFICULTY;
    msg.payload.widget.data.text = cDifficulty[H2EnumIndex(m_difficulty)];
    window->BroadcastMessage(msg);

    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%d", CalcDifficultyRating());
    strcat(gText, "%");
    msg.payload.widget.id = GAME_SCENARIO_RATING;
    msg.payload.widget.data.text = gText;
    window->BroadcastMessage(msg);

    mapSize = MAP_SIZE_SMALL_INDEX;
    if (m_mapHeader.width == MAP_DIMENSION_MEDIUM)
        mapSize = MAP_SIZE_MEDIUM_INDEX;
    else if (m_mapHeader.width == MAP_DIMENSION_LARGE)
        mapSize = MAP_SIZE_LARGE_INDEX;
    else if (m_mapHeader.width == MAP_DIMENSION_XLARGE)
        mapSize = MAP_SIZE_XLARGE_INDEX;
    msg.payload.widget.id = GAME_SCENARIO_MAP_SIZE;
    msg.payload.widget.data.text = cMapSize[mapSize];
    window->BroadcastMessage(msg);

    msg.payload.widget.id = GAME_SCENARIO_DESCRIPTION;
    msg.payload.widget.data.text = mapDescription.c_str();
    window->BroadcastMessage(msg);
    GetVictoryConditionText(gText);
    msg.payload.widget.id = GAME_SCENARIO_VICTORY;
    msg.payload.widget.data.text = gText;
    window->BroadcastMessage(msg);
    GetLossConditionText(gText);
    msg.payload.widget.id = GAME_SCENARIO_LOSS;
    msg.payload.widget.data.text = gText;
    window->BroadcastMessage(msg);

    iconControl = NULL;
    nameWidget = NULL;
    availWidth = PLAYER_AREA_WIDTH - m_mapHeader.playerCount * PLAYER_COLUMN_WIDTH;
    columnGap = availWidth / (m_mapHeader.playerCount + 1);
    firstColumnX = columnGap + PLAYER_FIRST_X_OFFSET;
    playerStep = columnGap + PLAYER_COLUMN_WIDTH;
    multiplayerYOffset = 0;

    for (playerCounter = 0; playerCounter < m_mapHeader.playerCount; ++playerCounter) {
        if (giNumHumanPlayers > 1) {
            iconControl = new iconWidget(
                static_cast<i16>(
                    firstColumnX + playerStep * playerCounter + PLAYER_HUMAN_X_OFFSET
                ),
                static_cast<i16>(multiplayerYOffset + SCENARIO_PLAYER_HUMAN_Y),
                PLAYER_HUMAN_WIDTH,
                PLAYER_HUMAN_HEIGHT,
                "ngextra.icn",
                PLAYER_HUMAN_FRAME,
                ICON_DRAW_NORMAL,
                static_cast<i16>(playerCounter + NEW_GAME_PLAYER_HUMAN_FIRST),
                WIDGET_KIND_ICON_DIRECT,
                PLAYER_WIDGET_FILL_COLOR
            );
            if (iconControl == NULL)
                MemError();
            window->AddWidget(iconControl, -1);

            iconControl = new iconWidget(
                static_cast<i16>(
                    firstColumnX + playerStep * playerCounter + PLAYER_HANDICAP_X_OFFSET
                ),
                static_cast<i16>(multiplayerYOffset + SCENARIO_PLAYER_HANDICAP_Y),
                PLAYER_HANDICAP_WIDTH,
                PLAYER_HANDICAP_HEIGHT,
                "ngextra.icn",
                0,
                ICON_DRAW_NORMAL,
                static_cast<i16>(playerCounter + NEW_GAME_HANDICAP_FIRST),
                WIDGET_KIND_ICON_DIRECT,
                PLAYER_WIDGET_FILL_COLOR
            );
            if (iconControl == NULL)
                MemError();
            window->AddWidget(iconControl, -1);
        }

        iconControl = new iconWidget(
            static_cast<i16>(
                firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
            ),
            SCENARIO_PLAYER_RACE_Y,
            PLAYER_RACE_WIDTH,
            static_cast<i16>(
                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
                                      : GAME_RACE_WIDGET_SINGLE_HEIGHT
            ),
            "ngextra.icn",
            static_cast<i16>(
                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
                                      : GAME_RACE_WIDGET_SINGLE_FRAME
            ),
            ICON_DRAW_NORMAL,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
            WIDGET_KIND_ICON_DIRECT,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControl == NULL)
            MemError();
        window->AddWidget(iconControl, -1);

        iconControl = new iconWidget(
            static_cast<i16>(
                firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET
            ),
            SCENARIO_PLAYER_COLOR_Y,
            PLAYER_COLOR_WIDTH,
            PLAYER_COLOR_HEIGHT,
            "ngextra.icn",
            static_cast<i16>(
                giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
                                      : GAME_COLOR_WIDGET_SINGLE_FRAME
            ),
            ICON_DRAW_NORMAL,
            static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
            WIDGET_KIND_ICON_DIRECT,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControl == NULL)
            MemError();
        window->AddWidget(iconControl, -1);

        if (giNumHumanPlayers > 1) {
            name = static_cast<char*>(
                H2_ALLOC(PLAYER_LABEL_CAPACITY)
            );
            utf8::Copy(name, PLAYER_LABEL_CAPACITY, " ");
            nameWidget = new textWidget(
                static_cast<i16>(
                    firstColumnX + playerStep * playerCounter + PLAYER_NAME_X_OFFSET
                ),
                SCENARIO_PLAYER_NAME_Y,
                PLAYER_NAME_WIDTH,
                PLAYER_NAME_HEIGHT,
                name,
                "smalfont.fnt",
                FONT_DRAW_DEFAULT,
                static_cast<i16>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (nameWidget == NULL)
                MemError();
            window->AddWidget(nameWidget, -1);
        }

        yExtra = 0;
        iconControl = new iconWidget(
            static_cast<i16>(
                firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET
            ),
            static_cast<i16>(
                yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y
            ),
            PLAYER_RACE_ICON_WIDTH,
            PLAYER_RACE_ICON_HEIGHT,
            "ngextra.icn",
            PLAYER_RACE_ICON_FRAME,
            ICON_DRAW_NORMAL,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
            WIDGET_KIND_ICON_DIRECT,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControl == NULL)
            MemError();
        window->AddWidget(iconControl, -1);

        name = static_cast<char*>(
            H2_ALLOC(PLAYER_LABEL_CAPACITY)
        );
        utf8::Copy(name, PLAYER_LABEL_CAPACITY, "A");
        raceNameWidth = m_mapHeader.playerCount < PLAYER_RACE_NAME_NARROW_THRESHOLD
                            ? PLAYER_RACE_NAME_WIDE_WIDTH
                        : m_mapHeader.playerCount < MAP_HEADER_PLAYER_COUNT
                            ? PLAYER_RACE_NAME_NARROW_WIDTH
                            : 0;
        nameWidget = new textWidget(
            static_cast<i16>(
                firstColumnX + playerStep * playerCounter + PLAYER_RACE_NAME_X_OFFSET
                - raceNameWidth / PLAYER_RACE_NAME_CENTER_DIVISOR
            ),
            static_cast<i16>(
                yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_NAME_Y
            ),
            static_cast<i16>(raceNameWidth + PLAYER_RACE_NAME_BASE_WIDTH),
            PLAYER_RACE_NAME_HEIGHT,
            name,
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_NAME_FIRST),
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (nameWidget == NULL)
            MemError();
        window->AddWidget(nameWidget, -1);

        iconControl = new iconWidget(
            static_cast<i16>(
                firstColumnX + playerStep * playerCounter + PLAYER_RACE_CYCLE_X_OFFSET
            ),
            static_cast<i16>(
                yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_CYCLE_Y
            ),
            PLAYER_RACE_CYCLE_WIDTH,
            PLAYER_RACE_CYCLE_HEIGHT,
            "ngextra.icn",
            PLAYER_RACE_CYCLE_FRAME,
            ICON_DRAW_NORMAL,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
            WIDGET_KIND_ICON_DIRECT,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControl == NULL)
            MemError();
        window->AddWidget(iconControl, -1);
    }

    for (playerCounter = 0; playerCounter < m_mapHeader.playerCount; ++playerCounter) {
        if (m_setupPlayerNetworkId[playerCounter] == GAME_COMPUTER_PLAYER) {
            gText[0] = 0;
        } else if (strlen(cPlayerNames[m_setupPlayerNetworkId[playerCounter]]) > 0) {
            utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cPlayerNames[m_setupPlayerNetworkId[playerCounter]]);
        } else {
            utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("player.number"), m_setupPlayerNetworkId[playerCounter] + 1);
        }
        msg.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
        msg.payload.widget.id =
            NEW_GAME_PLAYER_NAME_FIRST + playerCounter;
        msg.payload.widget.data.text = gText;
        window->BroadcastMessage(msg);

        msg.payload.widget.command = playerCounter != m_selectedSetupPlayer
                                                         ? NEW_GAME_WIDGET_DISABLE
                                                         : NEW_GAME_WIDGET_ENABLE;
        msg.payload.widget.id =
            NEW_GAME_PLAYER_SELECT_FIRST + playerCounter;
        msg.payload.widget.data.value = GAME_WIDGET_REFRESH_FRAME;
        window->BroadcastMessage(msg);

        if (m_setupPlayerType[playerCounter] != GAME_PLAYER_DEFAULT
            || (giNumHumanPlayers > 1
                && m_setupPlayerNetworkId[playerCounter] != GAME_COMPUTER_PLAYER))
            locked = false;
        else
            locked = true;
        msg.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        msg.payload.widget.id =
            NEW_GAME_COLOR_FIRST + playerCounter;
        if (m_setupPlayerNetworkId[playerCounter] == GAME_COMPUTER_PLAYER)
            msg.payload.widget.data.value =
                (locked ? GAME_COMPUTER_COLOR_LOCKED_FRAME
                                   : GAME_COMPUTER_COLOR_UNLOCKED_FRAME)
                + m_setupPlayerColor[playerCounter];
        else
            msg.payload.widget.data.value =
                (locked ? GAME_HUMAN_COLOR_LOCKED_FRAME
                                   : GAME_HUMAN_COLOR_UNLOCKED_FRAME)
                + m_setupPlayerColor[playerCounter];
        if (giNumHumanPlayers > 1)
            msg.payload.widget.data.value +=
                GAME_MULTIPLAYER_COLOR_FRAME_OFFSET;
        window->BroadcastMessage(msg);

        msg.payload.widget.command =
            locked ? NEW_GAME_WIDGET_DISABLE : NEW_GAME_WIDGET_ENABLE;
        msg.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
        window->BroadcastMessage(msg);

        msg.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        msg.payload.widget.id =
            NEW_GAME_HANDICAP_FIRST + playerCounter;
        if (m_setupPlayerNetworkId[playerCounter] == GAME_COMPUTER_PLAYER)
            msg.payload.widget.data.value = NEW_GAME_RACE_NAME_FIRST;
        else
            msg.payload.widget.data.value = H2EnumIndex(m_playerHandicap[playerCounter]);
        window->BroadcastMessage(msg);
        msg.payload.widget.command =
            m_setupPlayerNetworkId[playerCounter] == GAME_COMPUTER_PLAYER ? NEW_GAME_WIDGET_DISABLE
                                                                          : NEW_GAME_WIDGET_ENABLE;
        msg.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
        window->BroadcastMessage(msg);

        msg.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        msg.payload.widget.id =
            NEW_GAME_RACE_CYCLE_FIRST + playerCounter;
        msg.payload.widget.data.value =
            (locked ? GAME_FIXED_RACE_FRAME_BASE : GAME_RANDOM_RACE_FRAME_BASE)
            + H2EnumIndex(m_setupPlayerRace[playerCounter]);
        window->BroadcastMessage(msg);

        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, gAlignmentNames[H2EnumIndex(m_setupPlayerRace[playerCounter])]);
        msg.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
        msg.payload.widget.id =
            NEW_GAME_RACE_NAME_FIRST + playerCounter;
        msg.payload.widget.data.text = gText;
        window->BroadcastMessage(msg);
    }

    gpWindowManager->DoDialog(window, EventWindowHandler, 0);
    delete window;
}

void game::GetLossConditionText(char* text) {
    i32 week2;
    hero* lossHero11;
    i32 day26;
    i32 month19;
    town* city2;
    i32 townId12;

    if (m_mapHeader.lossCondition != MAP_LOSS_STANDARD) {
        switch (m_mapHeader.lossCondition) {
            case MAP_LOSS_TOWN:
                townId12 = GetTownId(m_mapHeader.lossConditionValue, m_mapHeader.lossTownY);
                city2 = GetTown(townId12);
                utf8::Format(
                    text,
                    GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr(
                        (city2->m_buildings & H2EnumIndex(TOWN_BUILDING_CASTLE))
                            ? "scenario.loss.castle"
                            : "scenario.loss.town"
                    ),
                    city2->m_name
                );
                break;

            case MAP_LOSS_HERO:
                lossHero11 = GetHero(m_mapHeader.lossConditionValue);
                utf8::Format(
                    text,
                    GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("scenario.loss.hero"),
                    lossHero11->m_name
                );
                break;

            case MAP_LOSS_TIME:
                month19 =
                    (gpGame->m_mapHeader.lossConditionValue - 1) / GAME_DAYS_PER_MONTH + 1;
                week2 = (gpGame->m_mapHeader.lossConditionValue
                         - (month19 - 1) * GAME_DAYS_PER_MONTH - 1)
                            / GAME_DAYS_PER_WEEK
                        + 1;
                day26 = (gpGame->m_mapHeader.lossConditionValue - 1) % GAME_DAYS_PER_WEEK + 1;
                utf8::Format(
                    text,
                    GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("scenario.loss.time"),
                    month19,
                    week2,
                    day26
                );
                break;
        }
    } else {
        strcpy(text, localization::Tr("scenario.loss.standard"));
    }
}

void game::GetVictoryConditionText(char* text) {
    town* targetTown;
    i32 localPlayerFirst;
    hero* victoryHero;
    char firstSide[GAME_SIDE_TEXT_SIZE];
    char secondSideValue[GAME_SIDE_TEXT_SIZE];

    if (m_mapHeader.victoryCondition != MAP_VICTORY_DEFEAT_ALL) {
        switch (m_mapHeader.victoryCondition) {
            case MAP_VICTORY_CAPTURE_TOWN:
                targetTown = GetTown(
                    GetTownId(m_mapHeader.victoryConditionValue, m_mapHeader.victoryTownY)
                );
                utf8::Format(
                    text,
                    GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr(
                        (targetTown->m_buildings & H2EnumIndex(TOWN_BUILDING_CASTLE))
                            ? "scenario.victory.capture_castle"
                            : "scenario.victory.capture_town"
                    ),
                    targetTown->m_name
                );
                break;

            case MAP_VICTORY_DEFEAT_HERO:
                victoryHero = GetHero(m_mapHeader.victoryConditionValue);
                utf8::Format(
                    text,
                    GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("scenario.victory.defeat_hero"),
                    victoryHero->m_name
                );
                break;

            case MAP_VICTORY_FIND_ARTIFACT:
                if (m_mapHeader.victoryConditionValue == 0)
                    utf8::Copy(
                        text, GLOBAL_TEXT_BUFFER_SIZE,
                        localization::Tr("scenario.victory.find_ultimate_artifact")
                    );
                else
                    utf8::Format(
                        text,
                        GLOBAL_TEXT_BUFFER_SIZE,
                        localization::Tr("scenario.victory.find_artifact"),
                        gArtifactNames[m_mapHeader.victoryConditionValue - 1]
                    );
                break;

            case MAP_VICTORY_ACCUMULATE_GOLD:
                utf8::Format(
                    text,
                    GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("scenario.victory.accumulate_gold"),
                    m_mapHeader.victoryConditionValue * GAME_GOLD_CONDITION_MULTIPLIER
                );
                break;

            case MAP_VICTORY_DEFEAT_SIDE:
                localPlayerFirst =
                    GetSideDesc(firstSide, 0, m_mapHeader.victoryConditionValue - 1);
                GetSideDesc(
                    secondSideValue,
                    m_mapHeader.victoryConditionValue,
                    m_mapHeader.playerCount - 1
                );
                if (localPlayerFirst)
                    utf8::Format(
                        text,
                        GLOBAL_TEXT_BUFFER_SIZE,
                        localization::Tr("scenario.victory.side_must_defeat"),
                        firstSide,
                        secondSideValue
                    );
                else
                    utf8::Format(
                        text,
                        GLOBAL_TEXT_BUFFER_SIZE,
                        localization::Tr("scenario.victory.side_must_defeat"),
                        secondSideValue,
                        firstSide
                    );
        }

        if (m_mapHeader.victoryCondition != MAP_VICTORY_DEFEAT_SIDE
            && m_mapHeader.allowNormalVictory != 0) {
            char primaryCondition[GAME_SIDE_TEXT_SIZE];
            utf8::Copy(primaryCondition, sizeof(primaryCondition), text);
            utf8::Format(
                text,
                GLOBAL_TEXT_BUFFER_SIZE,
                localization::Tr("scenario.victory.with_standard_alternative"),
                primaryCondition
            );
        } else {
            strcat(text, ".");
        }
    } else {
        strcpy(text, localization::Tr("scenario.victory.standard"));
    }
}

static void FormatScenarioSideList(
    char* text,
    char names[GAME_PLAYER_COUNT][GAME_SIDE_TEXT_SIZE],
    i32 count
) {
    switch (count) {
        case 1:
            utf8::Format(
                text, GAME_SIDE_TEXT_SIZE, localization::Tr("scenario.side.list.1"), names[0]
            );
            break;
        case 2:
            utf8::Format(
                text,
                GAME_SIDE_TEXT_SIZE,
                localization::Tr("scenario.side.list.2"),
                names[0], names[1]
            );
            break;
        case 3:
            utf8::Format(
                text,
                GAME_SIDE_TEXT_SIZE,
                localization::Tr("scenario.side.list.3"),
                names[0], names[1], names[2]
            );
            break;
        case 4:
            utf8::Format(
                text,
                GAME_SIDE_TEXT_SIZE,
                localization::Tr("scenario.side.list.4"),
                names[0], names[1], names[2], names[3]
            );
            break;
        case 5:
            utf8::Format(
                text,
                GAME_SIDE_TEXT_SIZE,
                localization::Tr("scenario.side.list.5"),
                names[0], names[1], names[2], names[3], names[4]
            );
            break;
        case 6:
            utf8::Format(
                text,
                GAME_SIDE_TEXT_SIZE,
                localization::Tr("scenario.side.list.6"),
                names[0], names[1], names[2], names[3], names[4], names[5]
            );
            break;
        default:
            text[0] = '\0';
            break;
    }
}

i32 game::GetSideDesc(char* text, i32 firstPlayer, i32 lastPlayer) {
    char colorNames[GAME_PLAYER_COUNT][GAME_SIDE_TEXT_SIZE];
    char playerList[GAME_SIDE_TEXT_SIZE];
    i32 sideCount;
    i32 i;
    b32 onSide;
    i32 otherPlayerCount;
    i32 localPlayer;
    i32 listIndex;

    localPlayer = -1;
    for (i = 0; i < m_mapHeader.playerCount; ++i) {
        if (m_setupPlayerNetworkId[i] == giThisGamePos)
            localPlayer = i;
    }

    onSide = localPlayer >= firstPlayer && localPlayer <= lastPlayer ? 1 : 0;
    sideCount = lastPlayer - firstPlayer + 1;
    otherPlayerCount = sideCount - (onSide != 0);

    listIndex = 0;
    for (i = firstPlayer; i <= lastPlayer; ++i) {
        if (!onSide || localPlayer != i) {
            utf8::Copy(
                colorNames[listIndex],
                sizeof(colorNames[listIndex]),
                gColors[m_setupPlayerColor[i]]
            );
            utf8::UppercaseFirst(colorNames[listIndex]);
            ++listIndex;
        }
    }

    if (onSide && otherPlayerCount == 0) {
        utf8::Format(
            text, GAME_SIDE_TEXT_SIZE, "%s", localization::Tr("scenario.side.you")
        );
        return onSide;
    }

    FormatScenarioSideList(playerList, colorNames, listIndex);
    if (onSide) {
        utf8::Format(
            text,
            GAME_SIDE_TEXT_SIZE,
            localization::Tr(
                otherPlayerCount == 1 ? "scenario.side.you_and_ally"
                                      : "scenario.side.you_and_allies"
            ),
            playerList
        );
    } else {
        utf8::Format(
            text,
            GAME_SIDE_TEXT_SIZE,
            localization::Tr(
                sideCount == 1 ? "scenario.side.enemy" : "scenario.side.enemy_alliance"
            ),
            playerList
        );
    }

    return onSide;
}

b32 gbNewGameDialogOver = true;
i32 NGKPcursorFlashOn = 1;
char* cNGKPDisplay;
b32 gbNewGameShadowHidden;
char* cNGKPCore;
i32 NGKPcursorIndex;
char* cTextReceivedBuffer[GAME_TEXT_BUFFER_COUNT];
class icon* NGKPBkg;
