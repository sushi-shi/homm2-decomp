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
#include <BASE/widgetKind.h>
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

H2_ENUM_BEGIN(NewGameConstant)
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
    GAME_REMOTE_PACKET_TYPE               = 2,
    GAME_REMOTE_PACKET_TYPE_ALTERNATE     = 3,
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
    GAME_DEFAULT_DIFFICULTY               = 1,
    GAME_DIALOG_CANCEL                    = 0x7801,
    GAME_DIALOG_OK                        = 0x7802,
    GAME_MAP_OPTIONS_CONTROL              = 0x36,
    GAME_WIDGET_ACTIVE_FRAME              = 0x1000,
    GAME_WIDGET_INACTIVE_FRAME            = 2,
    GAME_WIDGET_REFRESH_FRAME             = 4,
    GAME_SHADOW_FRAME                     = 6,
    GAME_MOUSE_RIGHT_FLAG                 = 0x200,
    GAME_LAST_STANDARD_RACE               = IDX(FACTION_NECROMANCER),
    GAME_HANDICAP_COUNT                   = 3,
    GAME_PLAYER_CONTROL_COUNT             = 6,
    GAME_DIFFICULTY_COUNT                 = 5,
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
H2_ENUM_END(NewGameConstant)

H2_ENUM_CLASS_BEGIN(NewGameKeyCode)
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
H2_ENUM_CLASS_END(NewGameKeyCode)

H2_ENUM_BEGIN(NewGameStorageConstant)
    FILE_MASK_CAPACITY      = 16,
    SAVED_MAP_NAME_CAPACITY = 44,
    PLAYER_LABEL_CAPACITY   = 2
H2_ENUM_END(NewGameStorageConstant)

H2_ENUM_BEGIN(NewGameDialogConstant)
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
    NEW_GAME_HANDLER_CONTINUE = 1,
    NEW_GAME_HANDLER_CLOSE    = 2
H2_ENUM_END(NewGameDialogConstant)

H2_ENUM_BEGIN(NewGameMapChoice)
    MAP_CHOICE_STANDARD  = 1,
    MAP_CHOICE_EXPANSION = 2
H2_ENUM_END(NewGameMapChoice)

H2_ENUM_CLASS_BEGIN(NewGameDifficultySlot)
    DIFFICULTY_SLOT_EASY       = 0,
    DIFFICULTY_SLOT_NORMAL     = 1,
    DIFFICULTY_SLOT_HARD       = 2,
    DIFFICULTY_SLOT_EXPERT     = 3,
    DIFFICULTY_SLOT_IMPOSSIBLE = 4
H2_ENUM_CLASS_END(NewGameDifficultySlot)

H2_ENUM_CLASS_BEGIN(NewGamePlayerSlot)
    PLAYER_SLOT_FIRST  = 0,
    PLAYER_SLOT_SECOND = 1,
    PLAYER_SLOT_THIRD  = 2,
    PLAYER_SLOT_FOURTH = 3,
    PLAYER_SLOT_FIFTH  = 4,
    PLAYER_SLOT_SIXTH  = 5
H2_ENUM_CLASS_END(NewGamePlayerSlot)

#ifdef HOMM2_STRICT_ENUM_TYPES
constexpr i32 operator+(NewGameControl first, NewGameDifficultySlot slot) {
    return static_cast<i32>(first) + static_cast<i32>(slot);
}

constexpr i32 operator+(NewGameControl first, NewGamePlayerSlot slot) {
    return static_cast<i32>(first) + static_cast<i32>(slot);
}
#endif

H2_ENUM_BEGIN(NewGameMapSizeIndex)
    MAP_SIZE_SMALL_INDEX  = 0,
    MAP_SIZE_MEDIUM_INDEX = 1,
    MAP_SIZE_LARGE_INDEX  = 2,
    MAP_SIZE_XLARGE_INDEX = 3
H2_ENUM_END(NewGameMapSizeIndex)

H2_ENUM_BEGIN(NewGamePlayerLayout)
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
    PLAYER_RACE_NAME_HEIGHT           = 12,
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
    PLAYER_WIDGET_KIND                = WIDGET_KIND_ICON_DIRECT,
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
H2_ENUM_END(NewGamePlayerLayout)

H2_ENUM_BEGIN(NewGameKeyEncoding)
    KEY_SCAN_CODE_SHIFT = 8
H2_ENUM_END(NewGameKeyEncoding)

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\Newgame.cpp"

VA(0x004b6f40, 0x1d5)
void game::GetMap(void) {
    fileRequester* requesterResult;
    i32 resultCode;
    char fileMask[FILE_MASK_CAPACITY];
    char savedName[SAVED_MAP_NAME_CAPACITY];

    strcpy(savedName, gMapName);
    strcpy(gcCurMapName, "");
    if (gbRemoteOn && xNetHasOldPlayers) {
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
        sprintf(fileMask, "*.%s", "MP2");
    } else if (xIsExpansionMap) {
        sprintf(fileMask, "*.%s", "MX2");
    } else {
        sprintf(fileMask, "*.%s", "MP2");
    }

    requesterResult = new fileRequester(
        MAP_REQUESTER_X,
        MAP_REQUESTER_Y,
        FILE_REQUESTER_MAP_GAME,
        fileMask,
        gcMapPath,
        fileMask
    );
    if (requesterResult == NULL)
        MemError();
    resultCode = gpExec->DoDialog(requesterResult);
    if (resultCode == FILE_REQUESTER_OK) {
        delete requesterResult;
        strcpy(gMapName, gLastFilename);
        if (_strcmpi(savedName, gMapName) != 0) {
            strcpy(m_mapFilename, gMapName);
            ProcessNewMap(NULL);
        }
    } else {
        delete requesterResult;
        strcpy(gMapName, savedName);
    }
}

VA(0x004b7115, 0x77)
void game::ProcessNewMap(struct SMapHeader* header) {
    m_newGameInitialized = 0;
    m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
    if (m_newGameWindow == NULL) {
    } else {
        CleanUpNewGameWindow();
        InitNewGame(header);
        InitNewGameWindow();
        UpdateNewGameWindow();
        m_newGameWindow->DrawWindow();
    }
}

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

        for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
            if (m_mapHeader.playerEnabled[player]) {
                m_setupPlayerColor[activePlayerCount] = static_cast<i8>(player);
                ++activePlayerCount;
            }
        }

        for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
            if (player >= m_mapHeader.playerCount) {
                m_setupPlayerType[player] = GAME_NETWORK_PLAYER_NONE;
                m_setupPlayerNetworkId[player] = m_setupPlayerType[player];
                m_setupPlayerRace[player] = m_setupPlayerNetworkId[player];
                m_playerHandicap[player] = m_setupPlayerRace[player];
            } else {
                m_playerHandicap[player] = 0;
                m_setupPlayerRace[player] = m_mapHeader.playerRace[m_setupPlayerColor[player]];
                m_setupPlayerNetworkId[player] = GAME_NETWORK_PLAYER_NONE;
                m_setupPlayerType[player] = GAME_NETWORK_PLAYER_NONE;
            }
        }

        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]
                && !m_mapHeader.playerCanComputer[m_setupPlayerColor[player]]) {
                m_setupPlayerType[player] = GAME_PLAYER_DEFAULT;
                m_setupPlayerNetworkId[player] = static_cast<i8>(humanPlayers);
                ++humanPlayers;
            } else if (!m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]
                       && m_mapHeader.playerCanComputer[m_setupPlayerColor[player]]) {
                m_setupPlayerNetworkId[player] = GAME_COMPUTER_PLAYER;
                m_setupPlayerType[player] = GAME_PLAYER_DEFAULT;
                ++computerPlayers;
            }
        }

        if (humanPlayers < giNumHumanPlayers
            && computerPlayers < m_mapHeader.playerCount - giNumHumanPlayers)
            flexiblePlayerType = GAME_PLAYER_FLEXIBLE;
        else
            flexiblePlayerType = GAME_PLAYER_DEFAULT;

        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_setupPlayerType[player] == GAME_NETWORK_PLAYER_NONE)
                m_setupPlayerType[player] = static_cast<i8>(flexiblePlayerType);
        }

        for (player = 0; player < m_mapHeader.playerCount; ++player) {
            if (m_setupPlayerNetworkId[player] != GAME_NETWORK_PLAYER_NONE)
                continue;
            if (humanPlayers < giNumHumanPlayers
                && m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]) {
                m_setupPlayerNetworkId[player] = static_cast<i8>(humanPlayers);
                ++humanPlayers;
            } else {
                m_setupPlayerNetworkId[player] = GAME_COMPUTER_PLAYER;
            }
        }
        m_difficulty = GAME_DEFAULT_DIFFICULTY;
        m_newGameInitialized = 1;
    }
    m_selectedSetupPlayer = GAME_NETWORK_PLAYER_NONE;
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

VA(0x004b769e, 0xaca)
i32 game::NewGame(void) {
    DATA(0x0051cdd0) static i16 newGameSourceLineBase = 319;
    char netPlayerPacket[GAME_PLAYER_INFO_BUFFER_SIZE];
    char mapInfoPacket[GAME_MAP_PACKET_SIZE];
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
    m_newGameWindow = NULL;

    if ((!gbRemoteOn || giThisNetPos == 0) && (!gbRemoteOn || !xNetHasOldPlayers)) {
        choiceWindow = new heroWindow(MAP_CHOICE_WINDOW_X, MAP_CHOICE_WINDOW_Y, "x_mapmnu.bin");
        if (choiceWindow == NULL)
            MemError();
        gpWindowManager->DoDialog(choiceWindow, ExpStdGameHandler, 0);
        delete choiceWindow;
        switch (static_cast<i16>(gpWindowManager->m_dialogResult)) {
            case MAP_CHOICE_STANDARD:
                xIsExpansionMap = 0;
                break;
            case MAP_CHOICE_EXPANSION:
                xIsExpansionMap = 1;
                break;
            case GAME_DIALOG_CANCEL:
                return 0;
        }
    }

    SetupNetPlayerNames();
    glTimers[0] = 0;
    for (textBufferIndex = 0; textBufferIndex < GAME_TEXT_BUFFER_COUNT; ++textBufferIndex) {
        cTextReceivedBuffer[textBufferIndex] =
            static_cast<char*>(H2_ALLOC(GAME_TEXT_BUFFER_SIZE, newGameSourceLineBase + 45));
        strcpy(cTextReceivedBuffer[textBufferIndex], "");
    }
    cNGKPCore = static_cast<char*>(H2_ALLOC(GAME_KEY_BUFFER_SIZE, newGameSourceLineBase + 48));
    cNGKPDisplay = static_cast<char*>(H2_ALLOC(GAME_KEY_BUFFER_SIZE, newGameSourceLineBase + 49));
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
                } while (remoteBuffer == NULL);
            } while (remoteBuffer->type != GAME_REMOTE_PACKET_TYPE);

            switch (remoteBuffer->command) {
                case GAME_REMOTE_MAP_HEADER:
                    memset(&m_mapHeader, 0, sizeof(m_mapHeader));
                    memcpy(&m_mapHeader, remoteBuffer->payload, GAME_MAP_PACKET_SIZE);
                    mapHeaderLoaded = 1;
                    break;
                case GAME_REMOTE_PLAYER_INFO:
                    memcpy(
                        gsNetPlayerInfo,
                        remoteBuffer->payload,
                        GAME_PLAYER_INFO_PACKET_SIZE
                    );
                    SetupNetPlayerNames();
                    playerDataReceived = 1;
                    break;
            }
        } while (!playerDataReceived || !mapHeaderLoaded);

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
        gpWindowManager->DoDialog(m_newGameWindow, NewGameHandler, 0);
        delete m_newGameWindow;
        if (gpWindowManager->m_dialogResult == GAME_DIALOG_CANCEL)
            result = 0;
        else
            result = 1;
    } else {
        for (;;) {
            wrongExpansionType = 0;
            extension = FindLastToken(m_mapFilename, '.');
            if (extension != NULL) {
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
            if (giNumHumanPlayers > BROKENA_MAX_HUMAN_PLAYERS
                && _strcmpi(gpGame->m_mapFilename, "brokena.mp2") == 0)
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
            memcpy(mapInfoPacket, &gpGame->m_mapHeader, GAME_MAP_PACKET_SIZE);
            transmitResult = TransmitRemoteData(
                mapInfoPacket,
                GAME_REMOTE_CHANNEL,
                GAME_MAP_PACKET_SIZE,
                GAME_REMOTE_MAP_HEADER,
                1,
                1,
                GAME_NETWORK_PLAYER_NONE
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
                GAME_NETWORK_PLAYER_NONE
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
        gpWindowManager->DoDialog(m_newGameWindow, NewGameHandler, 0);
        delete m_newGameWindow;
        if (gpWindowManager->m_dialogResult == GAME_DIALOG_CANCEL) {
            result = 0;
        } else {
            m_playerCount = m_mapHeader.playerCount;
            NewMap(gMapName);
        }
    }

    for (textBufferIndex = 0; textBufferIndex < GAME_TEXT_BUFFER_COUNT; ++textBufferIndex) {
        H2_FREE(cTextReceivedBuffer[textBufferIndex], newGameSourceLineBase + 286);
    }
    H2_FREE(cNGKPCore, newGameSourceLineBase + 288);
    H2_FREE(cNGKPDisplay, newGameSourceLineBase + 289);
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

    iconControlLocal = NULL;
    textControlLocal = NULL;
    availableWidthResult = PLAYER_AREA_WIDTH - m_mapHeader.playerCount * PLAYER_COLUMN_WIDTH;
    playerGapValue = availableWidthResult / (m_mapHeader.playerCount + 1);
    firstPlayerXLocal = playerGapValue + PLAYER_FIRST_X_OFFSET;
    playerSpacingTemp = playerGapValue + PLAYER_COLUMN_WIDTH;
    multiplayerYOffsetValue = 0;

    if (giNumHumanPlayers > 1 && iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT)
        multiplayerYOffsetValue = PLAYER_MULTIPLAYER_Y_OFFSET;

    for (playerCounter = 0; playerCounter < m_mapHeader.playerCount; ++playerCounter) {
        if (giNumHumanPlayers > 1) {
            iconControlLocal = new iconWidget(
                static_cast<i16>(
                    playerSpacingTemp * playerCounter + firstPlayerXLocal + PLAYER_HUMAN_X_OFFSET
                ),
                static_cast<i16>(multiplayerYOffsetValue + PLAYER_HUMAN_Y),
                PLAYER_HUMAN_WIDTH,
                PLAYER_HUMAN_HEIGHT,
                "ngextra.icn",
                PLAYER_HUMAN_FRAME,
                0,
                static_cast<i16>(playerCounter + NEW_GAME_PLAYER_HUMAN_FIRST),
                PLAYER_WIDGET_KIND,
                PLAYER_WIDGET_FILL_COLOR
            );
            if (iconControlLocal == NULL)
                MemError();
            m_newGameWindow->AddWidget(iconControlLocal, -1);

            iconControlLocal = new iconWidget(
                static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal
                                 + PLAYER_HANDICAP_X_OFFSET),
                static_cast<i16>(multiplayerYOffsetValue + PLAYER_HANDICAP_Y),
                PLAYER_HANDICAP_WIDTH,
                PLAYER_HANDICAP_HEIGHT,
                "ngextra.icn",
                0,
                0,
                static_cast<i16>(playerCounter + NEW_GAME_HANDICAP_FIRST),
                PLAYER_WIDGET_KIND,
                PLAYER_WIDGET_FILL_COLOR
            );
            if (iconControlLocal == NULL)
                MemError();
            m_newGameWindow->AddWidget(iconControlLocal, -1);
        }

        iconControlLocal = new iconWidget(
            static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal
                             + PLAYER_RACE_X_OFFSET),
            PLAYER_RACE_Y,
            PLAYER_RACE_WIDTH,
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT - GAME_RACE_WIDGET_SINGLE_HEIGHT))
                + GAME_RACE_WIDGET_SINGLE_HEIGHT
            ),
            "ngextra.icn",
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (GAME_RACE_WIDGET_MULTIPLAYER_FRAME - GAME_RACE_WIDGET_SINGLE_FRAME))
                + GAME_RACE_WIDGET_SINGLE_FRAME
            ),
            0,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
            PLAYER_WIDGET_KIND,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControlLocal == NULL)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal
                             + PLAYER_SELECT_X_OFFSET),
            PLAYER_SELECT_Y,
            PLAYER_SELECT_WIDTH,
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (GAME_PLAYER_WIDGET_MULTIPLAYER_HEIGHT
                    - GAME_PLAYER_WIDGET_SINGLE_HEIGHT))
                + GAME_PLAYER_WIDGET_SINGLE_HEIGHT
            ),
            "ngextra.icn",
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (GAME_PLAYER_WIDGET_MULTIPLAYER_FRAME - GAME_PLAYER_WIDGET_SINGLE_FRAME))
                + GAME_PLAYER_WIDGET_SINGLE_FRAME
            ),
            0,
            static_cast<i16>(playerCounter + NEW_GAME_PLAYER_SELECT_FIRST),
            PLAYER_WIDGET_KIND,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControlLocal == NULL)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal
                             + PLAYER_COLOR_X_OFFSET),
            PLAYER_COLOR_Y,
            PLAYER_COLOR_WIDTH,
            PLAYER_COLOR_HEIGHT,
            "ngextra.icn",
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (GAME_COLOR_WIDGET_MULTIPLAYER_FRAME - GAME_COLOR_WIDGET_SINGLE_FRAME))
                + GAME_COLOR_WIDGET_SINGLE_FRAME
            ),
            0,
            static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
            PLAYER_WIDGET_KIND,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControlLocal == NULL)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);

        if (giNumHumanPlayers > 1) {
            label = static_cast<char*>(
                H2_ALLOC(PLAYER_LABEL_CAPACITY, newGameWindowSourceLineBase + 80)
            );
            sprintf(label, " ");
            textControlLocal = new textWidget(
                static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal
                                 + PLAYER_NAME_X_OFFSET),
                PLAYER_NAME_Y,
                PLAYER_NAME_WIDTH,
                PLAYER_NAME_HEIGHT,
                label,
                "smalfont.fnt",
                FONT_DRAW_DEFAULT,
                static_cast<i16>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
                MESSAGE_WIDGET,
                FONT_ALIGN_CENTER
            );
            if (textControlLocal == NULL)
                MemError();
            m_newGameWindow->AddWidget(textControlLocal, -1);
        }

        singlePlayerYOffsetValue = 0;
        if (giNumHumanPlayers == 1)
            singlePlayerYOffsetValue = PLAYER_SINGLE_Y_OFFSET;
        iconControlLocal = new iconWidget(
            static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal
                             + PLAYER_RACE_ICON_X_OFFSET),
            static_cast<i16>(OD_STEER(multiplayerYOffsetValue) + singlePlayerYOffsetValue
                             + PLAYER_RACE_ICON_Y),
            PLAYER_RACE_ICON_WIDTH,
            PLAYER_RACE_ICON_HEIGHT,
            "ngextra.icn",
            PLAYER_RACE_ICON_FRAME,
            0,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
            PLAYER_WIDGET_KIND,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControlLocal == NULL)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);

        label = static_cast<char*>(
            H2_ALLOC(PLAYER_LABEL_CAPACITY, newGameWindowSourceLineBase + 112)
        );
        sprintf(label, "A");
        if (m_mapHeader.playerCount >= PLAYER_RACE_NAME_NARROW_THRESHOLD) {
            if (m_mapHeader.playerCount >= PLAYER_RACE_NAME_HIDDEN_THRESHOLD)
                raceTextWidth = 0;
            else
                raceTextWidth = PLAYER_RACE_NAME_NARROW_WIDTH;
        } else {
            raceTextWidth = PLAYER_RACE_NAME_WIDE_WIDTH;
        }
        textControlLocal = new textWidget(
            static_cast<i16>(
                playerSpacingTemp * playerCounter + firstPlayerXLocal + PLAYER_RACE_NAME_X_OFFSET
                - raceTextWidth / PLAYER_RACE_NAME_CENTER_DIVISOR
            ),
            static_cast<i16>(OD_STEER(multiplayerYOffsetValue) + singlePlayerYOffsetValue
                             + PLAYER_RACE_NAME_Y),
            static_cast<i16>(raceTextWidth + PLAYER_RACE_NAME_BASE_WIDTH),
            PLAYER_RACE_NAME_HEIGHT,
            label,
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_NAME_FIRST),
            MESSAGE_WIDGET,
            FONT_ALIGN_CENTER
        );
        if (textControlLocal == NULL)
            MemError();
        m_newGameWindow->AddWidget(textControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<i16>(playerSpacingTemp * playerCounter + firstPlayerXLocal
                             + PLAYER_RACE_CYCLE_X_OFFSET),
            static_cast<i16>(OD_STEER(multiplayerYOffsetValue) + singlePlayerYOffsetValue
                             + PLAYER_RACE_CYCLE_Y),
            PLAYER_RACE_CYCLE_WIDTH,
            PLAYER_RACE_CYCLE_HEIGHT,
            "ngextra.icn",
            PLAYER_RACE_CYCLE_FRAME,
            0,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
            PLAYER_WIDGET_KIND,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControlLocal == NULL)
            MemError();
        m_newGameWindow->AddWidget(iconControlLocal, -1);
    }
}

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
    messageTemp.payload.widget.data.value = GAME_WIDGET_REFRESH_FRAME;
    for (playerIndex3 = 0; playerIndex3 < GAME_DIFFICULTY_COUNT; ++playerIndex3) {
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_DIFFICULTY_FIRST, playerIndex3);
        m_newGameWindow->BroadcastMessage(messageTemp);
    }
    messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
    messageTemp.payload.widget.id =
        EncodeNewGameControlIndex(NEW_GAME_DIFFICULTY_FIRST, m_difficulty);
    m_newGameWindow->BroadcastMessage(messageTemp);

    if (giNumHumanPlayers > 1) {
        for (playerIndex3 = 0; playerIndex3 < GAME_CHAT_LINE_COUNT; ++playerIndex3) {
            sprintf(gText, cTextReceivedBuffer[playerIndex3]);
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
            messageTemp.payload.widget.id =
                EncodeNewGameControlIndex(NEW_GAME_CHAT_FIRST, playerIndex3);
            messageTemp.payload.widget.data.text = gText;
            m_newGameWindow->BroadcastMessage(messageTemp);
        }
    }

    for (playerIndex3 = 0; playerIndex3 < m_mapHeader.playerCount; ++playerIndex3) {
        if (m_setupPlayerNetworkId[playerIndex3] == GAME_COMPUTER_PLAYER) {
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
        messageTemp.payload.widget.data.value = GAME_WIDGET_REFRESH_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (m_setupPlayerType[playerIndex3] != GAME_PLAYER_DEFAULT
            || (giNumHumanPlayers > 1
                && m_setupPlayerNetworkId[playerIndex3] != GAME_COMPUTER_PLAYER))
            playerLockedValue = 0;
        else
            playerLockedValue = 1;
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_COLOR_FIRST, playerIndex3);
        if (m_setupPlayerNetworkId[playerIndex3] == GAME_COMPUTER_PLAYER)
            messageTemp.payload.widget.data.value =
                m_setupPlayerColor[playerIndex3]
                + (playerLockedValue ? GAME_COMPUTER_COLOR_LOCKED_FRAME
                                     : GAME_COMPUTER_COLOR_UNLOCKED_FRAME);
        else
            messageTemp.payload.widget.data.value =
                m_setupPlayerColor[playerIndex3]
                + (playerLockedValue ? GAME_HUMAN_COLOR_LOCKED_FRAME
                                     : GAME_HUMAN_COLOR_UNLOCKED_FRAME);
        if (giNumHumanPlayers > 1)
            messageTemp.payload.widget.data.value += GAME_MULTIPLAYER_COLOR_FRAME_OFFSET;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (playerLockedValue)
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        else
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        messageTemp.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);

        messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_HANDICAP_FIRST, playerIndex3);
        if (m_setupPlayerNetworkId[playerIndex3] == GAME_COMPUTER_PLAYER)
            messageTemp.payload.widget.data.value = NEW_GAME_RACE_NAME_FIRST;
        else
            messageTemp.payload.widget.data.value = m_playerHandicap[playerIndex3];
        m_newGameWindow->BroadcastMessage(messageTemp);
        if (m_setupPlayerNetworkId[playerIndex3] == GAME_COMPUTER_PLAYER)
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        else
            messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        messageTemp.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);

        if (m_mapHeader.playerRace[m_setupPlayerColor[playerIndex3]] == GAME_RANDOM_RACE)
            playerLockedValue = 0;
        else
            playerLockedValue = 1;
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        messageTemp.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
        m_newGameWindow->BroadcastMessage(messageTemp);
        messageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        messageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_RACE_CYCLE_FIRST, playerIndex3);
        messageTemp.payload.widget.data.value =
            m_setupPlayerRace[playerIndex3]
            + (playerLockedValue ? GAME_FIXED_RACE_FRAME_BASE
                                 : GAME_RANDOM_RACE_FRAME_BASE);
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
        messageTemp.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
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
    char setupData[GAME_SETUP_BUFFER_SIZE];
    char mapPacketLocal[GAME_MAP_PACKET_SIZE];
    tag_message mapWindowMessageTemp;
    i32 helpDialogIndexLocal;
    i32 unusedSender;
    char mapNamePacket[MAP_HEADER_NAME_SIZE];

    if (!gbNewGameShadowHidden) {
        gbNewGameShadowHidden = true;
        windowMessage.type = MESSAGE_WIDGET;
        windowMessage.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        windowMessage.payload.widget.id = EncodeNewGameControl(NEW_GAME_SHADOW);
        windowMessage.payload.widget.data.value = GAME_SHADOW_FRAME;
        gpGame->m_newGameWindow->BroadcastMessage(windowMessage);
    }

    if (message.type == MESSAGE_NONE) {
        remotePacketResult = reinterpret_cast<NewGameRemotePacket*>(GetRemoteData(1));
        if (remotePacketResult != NULL
            && (remotePacketResult->type == GAME_REMOTE_PACKET_TYPE
                || remotePacketResult->type == GAME_REMOTE_PACKET_TYPE_ALTERNATE)) {
            switch (remotePacketResult->command) {
                case GAME_REMOTE_START:
                    gpWindowManager->m_dialogResult = message.payload.widget.id;
                    gpWindowManager->m_dialogResult = GAME_DIALOG_OK;
                    message.type = MESSAGE_WIDGET;
                    message.payload.widget.id = GAME_DIALOG_CLOSE_MESSAGE;
                    message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
                    return NEW_GAME_HANDLER_CLOSE;

                case GAME_REMOTE_CANCEL:
                    NormalDialog(
                        "The host has canceled the game.",
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
                    redrawWindow = 1;
                    break;

                case GAME_REMOTE_MAP_HEADER:
                    memset(&remoteMapHeaderValue, 0, sizeof(remoteMapHeaderValue));
                    memcpy(
                        &remoteMapHeaderValue,
                        remotePacketResult->payload,
                        GAME_MAP_PACKET_SIZE
                    );
                    gpGame->ProcessNewMap(&remoteMapHeaderValue);
                    break;

                case GAME_REMOTE_CHAT:
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
        if (glTimers[0] < static_cast<i32>(KBTickCount())) {
            gpGame->NGKPSetupDisplayString(cNGKPCore, static_cast<u16>(NGKPcursorIndex));
            gpGame->DrawNGKPDisplayString(1);
        }
    }

    if (message.type == MESSAGE_KEY_DOWN && giNumHumanPlayers > 1
        && iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT && gpGame->ProcessNGKeyPress(message)) {
        redrawWindow = 1;
        for (currentPlayerLocal = 0; currentPlayerLocal < GAME_CHAT_LINE_COUNT - 1;
             ++currentPlayerLocal) {
            strcpy(
                cTextReceivedBuffer[currentPlayerLocal],
                cTextReceivedBuffer[currentPlayerLocal + 1]
            );
        }
        strcpy(cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1], cNGKPCore);
        strcpy(cNGKPCore, "");
        strcpy(cNGKPDisplay, "");
        NGKPcursorIndex = 0;
        transmitResultTemp = TransmitRemoteData(
            cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1],
            GAME_REMOTE_CHANNEL,
            strlen(cTextReceivedBuffer[GAME_CHAT_LINE_COUNT - 1]) + 1,
            GAME_REMOTE_CHAT,
            1,
            1,
            GAME_NETWORK_PLAYER_NONE
        );
        if (!transmitResultTemp)
            ShutDown(NULL);
    }

    if (message.type != MESSAGE_WIDGET)
        goto finish;

    if (message.payload.widget.parameter & GAME_MOUSE_RIGHT_FLAG) {
        if (message.payload.widget.command == NEW_GAME_EVENT_PRESS
            || message.payload.widget.command == NEW_GAME_EVENT_ALTERNATE_PRESS) {
            helpDialogIndexLocal = -1;
            if ((message.payload.widget.id >= NEW_GAME_DIFFICULTY_HELP_FIRST
                 && message.payload.widget.id <= LastNewGameControl(
                        NEW_GAME_DIFFICULTY_HELP_FIRST,
                        GAME_DIFFICULTY_COUNT
                    ))
                || (message.payload.widget.id >= NEW_GAME_DIFFICULTY_FIRST
                    && message.payload.widget.id <= LastNewGameControl(
                           NEW_GAME_DIFFICULTY_FIRST,
                           GAME_DIFFICULTY_COUNT
                       )))
                helpDialogIndexLocal = GAME_HELP_DIFFICULTY;
            if ((message.payload.widget.id >= NEW_GAME_HANDICAP_FIRST
                 && message.payload.widget.id <= LastNewGameControl(
                        NEW_GAME_HANDICAP_FIRST,
                        GAME_PLAYER_CONTROL_COUNT
                    ))
                || (message.payload.widget.id >= NEW_GAME_PLAYER_HUMAN_FIRST
                    && message.payload.widget.id <= LastNewGameControl(
                           NEW_GAME_PLAYER_HUMAN_FIRST,
                           GAME_PLAYER_CONTROL_COUNT
                       )))
                helpDialogIndexLocal = GAME_HELP_HANDICAP;
            if ((message.payload.widget.id >= NEW_GAME_COLOR_FIRST
                 && message.payload.widget.id
                        <= LastNewGameControl(NEW_GAME_COLOR_FIRST, GAME_PLAYER_CONTROL_COUNT))
                || (message.payload.widget.id >= NEW_GAME_RACE_FIRST
                    && message.payload.widget.id <= LastNewGameControl(
                           NEW_GAME_RACE_FIRST,
                           GAME_PLAYER_CONTROL_COUNT
                       ))
                || (message.payload.widget.id >= NEW_GAME_PLAYER_SELECT_FIRST
                    && message.payload.widget.id <= NEW_GAME_PLAYER_NAME_FIRST)
                || (message.payload.widget.id >= NEW_GAME_PLAYER_NAME_FIRST
                    && message.payload.widget.id <= NEW_GAME_RACE_ICON_FIRST))
                helpDialogIndexLocal = GAME_HELP_PLAYER;
            if ((message.payload.widget.id >= NEW_GAME_RACE_CYCLE_FIRST
                 && message.payload.widget.id <= LastNewGameControl(
                        NEW_GAME_RACE_CYCLE_FIRST,
                        GAME_PLAYER_CONTROL_COUNT
                    ))
                || (message.payload.widget.id >= NEW_GAME_RACE_ICON_FIRST
                    && message.payload.widget.id <= LastNewGameControl(
                           NEW_GAME_RACE_ICON_FIRST,
                           GAME_PLAYER_CONTROL_COUNT
                       )))
                helpDialogIndexLocal = GAME_HELP_RACE;
            if (message.payload.widget.id == GAME_MAP_OPTIONS_CONTROL
                || DecodeNewGameControl(message.payload.widget.id) == NEW_GAME_MAP_SELECT
                || DecodeNewGameControl(message.payload.widget.id) == NEW_GAME_SCENARIO_NAME)
                helpDialogIndexLocal = GAME_HELP_MAP;
            if (DecodeNewGameControl(message.payload.widget.id) == NEW_GAME_RATING)
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
        goto finish;
    }

    switch (message.payload.widget.command) {
        case NEW_GAME_EVENT_RELEASE:
            switch (message.payload.widget.id) {
                case GAME_MAP_OPTIONS_CONTROL:
                    goto chooseMap;

                case GAME_DIALOG_OK:
                    if (gbRemoteOn) {
                        transmitResultTemp = TransmitRemoteData(
                            NULL,
                            GAME_REMOTE_CHANNEL,
                            0,
                            GAME_REMOTE_START,
                            1,
                            1,
                            GAME_NETWORK_PLAYER_NONE
                        );
                    }
                    gpWindowManager->m_dialogResult = message.payload.widget.id;
                    message.payload.widget.id = GAME_DIALOG_CLOSE_MESSAGE;
                    message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
                    gbNewGameDialogOver = true;
                    return NEW_GAME_HANDLER_CLOSE;

                case GAME_DIALOG_CANCEL:
                    if (gbRemoteOn) {
                        transmitResultTemp = TransmitRemoteData(
                            NULL,
                            GAME_REMOTE_CHANNEL,
                            0,
                            GAME_REMOTE_CANCEL,
                            1,
                            1,
                            GAME_NETWORK_PLAYER_NONE
                        );
                        ShutDown(NULL);
                    }
                    gpWindowManager->m_dialogResult = message.payload.widget.id;
                    message.payload.widget.id = GAME_DIALOG_CLOSE_MESSAGE;
                    message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
                    gbNewGameDialogOver = true;
                    return NEW_GAME_HANDLER_CLOSE;

                default:
                    break;
            }
            break;

        case NEW_GAME_EVENT_PRESS:
            switch (message.payload.widget.id) {
                case NEW_GAME_DIFFICULTY_HELP_FIRST + DIFFICULTY_SLOT_EASY:
                case NEW_GAME_DIFFICULTY_HELP_FIRST + DIFFICULTY_SLOT_NORMAL:
                case NEW_GAME_DIFFICULTY_HELP_FIRST + DIFFICULTY_SLOT_HARD:
                case NEW_GAME_DIFFICULTY_HELP_FIRST + DIFFICULTY_SLOT_EXPERT:
                case NEW_GAME_DIFFICULTY_HELP_FIRST + DIFFICULTY_SLOT_IMPOSSIBLE:
                    currentPlayerLocal = message.payload.widget.id - NEW_GAME_DIFFICULTY_HELP_FIRST;
                    goto setDifficulty;

                case NEW_GAME_DIFFICULTY_FIRST + DIFFICULTY_SLOT_EASY:
                case NEW_GAME_DIFFICULTY_FIRST + DIFFICULTY_SLOT_NORMAL:
                case NEW_GAME_DIFFICULTY_FIRST + DIFFICULTY_SLOT_HARD:
                case NEW_GAME_DIFFICULTY_FIRST + DIFFICULTY_SLOT_EXPERT:
                case NEW_GAME_DIFFICULTY_FIRST + DIFFICULTY_SLOT_IMPOSSIBLE:
                    currentPlayerLocal = message.payload.widget.id - NEW_GAME_DIFFICULTY_FIRST;
                setDifficulty:
                    gpGame->m_difficulty = static_cast<i8>(currentPlayerLocal);
                    synchronizeSetupResult = 1;
                    redrawWindow = 1;
                    break;

                case NEW_GAME_HANDICAP_FIRST + PLAYER_SLOT_FIRST:
                case NEW_GAME_HANDICAP_FIRST + PLAYER_SLOT_SECOND:
                case NEW_GAME_HANDICAP_FIRST + PLAYER_SLOT_THIRD:
                case NEW_GAME_HANDICAP_FIRST + PLAYER_SLOT_FOURTH:
                case NEW_GAME_HANDICAP_FIRST + PLAYER_SLOT_FIFTH:
                case NEW_GAME_HANDICAP_FIRST + PLAYER_SLOT_SIXTH:
                    currentPlayerLocal = message.payload.widget.id - NEW_GAME_HANDICAP_FIRST;
                    goto cycleHandicap;

                case NEW_GAME_PLAYER_HUMAN_FIRST + PLAYER_SLOT_FIRST:
                case NEW_GAME_PLAYER_HUMAN_FIRST + PLAYER_SLOT_SECOND:
                case NEW_GAME_PLAYER_HUMAN_FIRST + PLAYER_SLOT_THIRD:
                case NEW_GAME_PLAYER_HUMAN_FIRST + PLAYER_SLOT_FOURTH:
                case NEW_GAME_PLAYER_HUMAN_FIRST + PLAYER_SLOT_FIFTH:
                case NEW_GAME_PLAYER_HUMAN_FIRST + PLAYER_SLOT_SIXTH:
                    currentPlayerLocal = message.payload.widget.id - NEW_GAME_PLAYER_HUMAN_FIRST;
                cycleHandicap:
                    synchronizeSetupResult = 1;
                    redrawWindow = 1;
                    if (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
                        != GAME_COMPUTER_PLAYER) {
                        gpGame->m_playerHandicap[currentPlayerLocal] = static_cast<i8>(
                            (gpGame->m_playerHandicap[currentPlayerLocal] + 1)
                            % GAME_HANDICAP_COUNT
                        );
                    }
                    break;

                case NEW_GAME_RACE_FIRST + PLAYER_SLOT_FIRST:
                case NEW_GAME_RACE_FIRST + PLAYER_SLOT_SECOND:
                case NEW_GAME_RACE_FIRST + PLAYER_SLOT_THIRD:
                case NEW_GAME_RACE_FIRST + PLAYER_SLOT_FOURTH:
                case NEW_GAME_RACE_FIRST + PLAYER_SLOT_FIFTH:
                case NEW_GAME_RACE_FIRST + PLAYER_SLOT_SIXTH:
                    currentPlayerLocal = message.payload.widget.id - NEW_GAME_RACE_FIRST;
                    goto selectPlayer;

                case NEW_GAME_COLOR_FIRST + PLAYER_SLOT_FIRST:
                case NEW_GAME_COLOR_FIRST + PLAYER_SLOT_SECOND:
                case NEW_GAME_COLOR_FIRST + PLAYER_SLOT_THIRD:
                case NEW_GAME_COLOR_FIRST + PLAYER_SLOT_FOURTH:
                case NEW_GAME_COLOR_FIRST + PLAYER_SLOT_FIFTH:
                case NEW_GAME_COLOR_FIRST + PLAYER_SLOT_SIXTH:
                    currentPlayerLocal = message.payload.widget.id - NEW_GAME_COLOR_FIRST;
                    goto selectPlayer;

                case NEW_GAME_PLAYER_SELECT_FIRST + PLAYER_SLOT_FIRST:
                case NEW_GAME_PLAYER_SELECT_FIRST + PLAYER_SLOT_SECOND:
                case NEW_GAME_PLAYER_SELECT_FIRST + PLAYER_SLOT_THIRD:
                case NEW_GAME_PLAYER_SELECT_FIRST + PLAYER_SLOT_FOURTH:
                case NEW_GAME_PLAYER_SELECT_FIRST + PLAYER_SLOT_FIFTH:
                case NEW_GAME_PLAYER_SELECT_FIRST + PLAYER_SLOT_SIXTH:
                    currentPlayerLocal = message.payload.widget.id - NEW_GAME_PLAYER_SELECT_FIRST;
                    goto selectPlayer;

                case NEW_GAME_PLAYER_NAME_FIRST + PLAYER_SLOT_FIRST:
                case NEW_GAME_PLAYER_NAME_FIRST + PLAYER_SLOT_SECOND:
                case NEW_GAME_PLAYER_NAME_FIRST + PLAYER_SLOT_THIRD:
                case NEW_GAME_PLAYER_NAME_FIRST + PLAYER_SLOT_FOURTH:
                case NEW_GAME_PLAYER_NAME_FIRST + PLAYER_SLOT_FIFTH:
                case NEW_GAME_PLAYER_NAME_FIRST + PLAYER_SLOT_SIXTH:
                    currentPlayerLocal = message.payload.widget.id - NEW_GAME_PLAYER_NAME_FIRST;
                selectPlayer:
                    synchronizeSetupResult = 1;
                    redrawWindow = 1;
                    if (gpGame->m_setupPlayerType[currentPlayerLocal] != GAME_PLAYER_DEFAULT
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
                                        oldNetworkId =
                                            gpGame->m_setupPlayerNetworkId[swapPlayerTemp];
                                        gpGame->m_setupPlayerNetworkId[swapPlayerTemp] =
                                            gpGame->m_setupPlayerNetworkId[currentPlayerLocal];
                                        gpGame->m_setupPlayerNetworkId[currentPlayerLocal] =
                                            oldNetworkId;
                                        swapPlayerTemp = GAME_SWAP_SEARCH_DONE;
                                    }
                                }
                            }
                        } else if (gpGame->m_selectedSetupPlayer == GAME_NETWORK_PLAYER_NONE) {
                            gpGame->m_selectedSetupPlayer = static_cast<i8>(currentPlayerLocal);
                        } else if (gpGame->m_selectedSetupPlayer == currentPlayerLocal
                                   || (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
                                           == GAME_COMPUTER_PLAYER
                                       && gpGame->m_setupPlayerNetworkId
                                                  [gpGame->m_selectedSetupPlayer]
                                              == GAME_COMPUTER_PLAYER)) {
                            gpGame->m_selectedSetupPlayer = GAME_NETWORK_PLAYER_NONE;
                        } else {
                            if ((gpGame->m_setupPlayerType[currentPlayerLocal]
                                     != GAME_PLAYER_DEFAULT
                                 && gpGame->m_setupPlayerType[gpGame->m_selectedSetupPlayer]
                                        != GAME_PLAYER_DEFAULT)
                                || (gpGame->m_setupPlayerNetworkId[currentPlayerLocal]
                                        != GAME_COMPUTER_PLAYER
                                    && gpGame->m_setupPlayerNetworkId[gpGame->m_selectedSetupPlayer]
                                           != GAME_COMPUTER_PLAYER)) {
                                swapPlayerTemp = gpGame->m_setupPlayerNetworkId[currentPlayerLocal];
                                gpGame->m_setupPlayerNetworkId[currentPlayerLocal] =
                                    gpGame->m_setupPlayerNetworkId[gpGame->m_selectedSetupPlayer];
                                gpGame->m_setupPlayerNetworkId[gpGame->m_selectedSetupPlayer] =
                                    static_cast<i8>(swapPlayerTemp);
                            } else {
                                NormalDialog(
                                    "The two positions selected can not be swapped.",
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

                case NEW_GAME_RACE_CYCLE_FIRST + PLAYER_SLOT_FIRST:
                case NEW_GAME_RACE_CYCLE_FIRST + PLAYER_SLOT_SECOND:
                case NEW_GAME_RACE_CYCLE_FIRST + PLAYER_SLOT_THIRD:
                case NEW_GAME_RACE_CYCLE_FIRST + PLAYER_SLOT_FOURTH:
                case NEW_GAME_RACE_CYCLE_FIRST + PLAYER_SLOT_FIFTH:
                case NEW_GAME_RACE_CYCLE_FIRST + PLAYER_SLOT_SIXTH:
                    currentPlayerLocal = message.payload.widget.id - NEW_GAME_RACE_CYCLE_FIRST;
                    goto cycleRace;

                case NEW_GAME_RACE_ICON_FIRST + PLAYER_SLOT_FIRST:
                case NEW_GAME_RACE_ICON_FIRST + PLAYER_SLOT_SECOND:
                case NEW_GAME_RACE_ICON_FIRST + PLAYER_SLOT_THIRD:
                case NEW_GAME_RACE_ICON_FIRST + PLAYER_SLOT_FOURTH:
                case NEW_GAME_RACE_ICON_FIRST + PLAYER_SLOT_FIFTH:
                case NEW_GAME_RACE_ICON_FIRST + PLAYER_SLOT_SIXTH:
                    currentPlayerLocal = message.payload.widget.id - NEW_GAME_RACE_ICON_FIRST;
                cycleRace:
                    if (gpGame->m_mapHeader
                            .playerRace[gpGame->m_setupPlayerColor[currentPlayerLocal]]
                        == GAME_RANDOM_RACE) {
                        if (gpGame->m_setupPlayerRace[currentPlayerLocal] == GAME_RANDOM_RACE)
                            gpGame->m_setupPlayerRace[currentPlayerLocal] = 0;
                        else if (gpGame->m_setupPlayerRace[currentPlayerLocal]
                                 == GAME_LAST_STANDARD_RACE)
                            gpGame->m_setupPlayerRace[currentPlayerLocal] = GAME_RANDOM_RACE;
                        else
                            ++gpGame->m_setupPlayerRace[currentPlayerLocal];
                        synchronizeSetupResult = 1;
                        redrawWindow = 1;
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
                        mapWindowMessageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
                        mapWindowMessageTemp.payload.widget.id = GAME_DIALOG_CANCEL;
                        mapWindowMessageTemp.payload.widget.data.value =
                            GAME_WIDGET_INACTIVE_FRAME;
                        gpGame->m_newGameWindow->BroadcastMessage(mapWindowMessageTemp);
                        gpGame->GetMap();
                        mapWindowMessageTemp.type = MESSAGE_WIDGET;
                        mapWindowMessageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
                        mapWindowMessageTemp.payload.widget.id = GAME_DIALOG_CANCEL;
                        mapWindowMessageTemp.payload.widget.data.value =
                            GAME_WIDGET_INACTIVE_FRAME;
                        gpGame->m_newGameWindow->BroadcastMessage(mapWindowMessageTemp);
                        if (gbRemoteOn) {
                            memcpy(mapPacketLocal, &gpGame->m_mapHeader, GAME_MAP_PACKET_SIZE);
                            transmitResultTemp = TransmitRemoteData(
                                mapPacketLocal,
                                GAME_REMOTE_CHANNEL,
                                GAME_MAP_PACKET_SIZE,
                                GAME_REMOTE_MAP_HEADER,
                                1,
                                1,
                                GAME_NETWORK_PLAYER_NONE
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
        memcpy(setupData, gpGame->m_setupPlayerColor, GAME_SETUP_DATA_SIZE);
        transmitResultTemp = TransmitRemoteData(
            mapNamePacket,
            GAME_REMOTE_CHANNEL,
            GAME_SETUP_PACKET_SIZE,
            GAME_REMOTE_SETUP,
            1,
            1,
            GAME_NETWORK_PLAYER_NONE
        );
        if (!transmitResultTemp)
            ShutDown(NULL);
    }
    return NEW_GAME_HANDLER_CONTINUE;
}

VA(0x004b9db8, 0x418)
i32 game::ProcessNGKeyPress(struct tag_message& message) {
    char workText[GAME_KEY_BUFFER_SIZE];
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
            if (message.payload.keyboard.keyCode == IDX(GAME_KEY_ENTER))
                return 1;

            if (message.payload.keyboard.keyCode == IDX(GAME_KEY_BACKSPACE)) {
                if (NGKPcursorIndex > 0) {
                    strcpy(gText, cNGKPCore + NGKPcursorIndex);
                    strcpy(cNGKPCore + (NGKPcursorIndex - 1), gText);
                    --NGKPcursorIndex;
                }
            } else if (strlen(cNGKPCore) + 1 < GAME_CHAT_TEXT_LIMIT
                       && message.payload.keyboard.keyCode != 0) {
                strcpy(workText, cNGKPCore);
                keyChar = 0;
                if (message.payload.keyboard.keyCode >= IDX(GAME_KEY_FIRST_EXTENDED)) {
                    scanCode =
                        static_cast<u8>(static_cast<u32>(message.payload.keyboard.keyCode)
                                        >> KEY_SCAN_CODE_SHIFT);
                    switch (scanCode) {
                        case IDX(GAME_KEYPAD_INSERT):
                            keyChar = '0';
                            break;
                        case IDX(GAME_KEYPAD_END):
                            keyChar = '1';
                            break;
                        case IDX(GAME_KEYPAD_DOWN):
                            keyChar = '2';
                            break;
                        case IDX(GAME_KEYPAD_PAGE_DOWN):
                            keyChar = '3';
                            break;
                        case IDX(GAME_KEYPAD_LEFT):
                            keyChar = '4';
                            break;
                        case IDX(GAME_KEYPAD_CENTER):
                            keyChar = '5';
                            break;
                        case IDX(GAME_KEYPAD_RIGHT):
                            keyChar = '6';
                            break;
                        case IDX(GAME_KEYPAD_HOME):
                            keyChar = '7';
                            break;
                        case IDX(GAME_KEYPAD_UP):
                            keyChar = '8';
                            break;
                        case IDX(GAME_KEYPAD_PAGE_UP):
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
                    widthResult = smallFont->LineLength(cNGKPDisplay, GAME_CHAT_DRAW_WIDTH);
                    if (widthResult > GAME_CHAT_MAX_LINES) {
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
        glTimers[0] = KBTickCount() + GAME_CURSOR_FLASH_TICKS;
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
        GAME_CHAT_DRAW_X,
        GAME_CHAT_DRAW_Y,
        GAME_CHAT_BACKGROUND_WIDTH,
        0
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
    scenarioWindowValue = new heroWindow(SCENARIO_WINDOW_X, SCENARIO_WINDOW_Y, "sceninfo.bin");
    if (scenarioWindowValue == NULL)
        MemError();
    SetWinText(scenarioWindowValue, GAME_SCENARIO_WINDOW_TEXT_ID);

    scenarioMessageTemp.type = MESSAGE_WIDGET;
    scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
    scenarioMessageTemp.payload.widget.id = EncodeNewGameControl(NEW_GAME_SCENARIO_NAME);
    scenarioMessageTemp.payload.widget.data.text = m_mapHeader.name;
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

    scenarioMessageTemp.payload.widget.id = GAME_SCENARIO_DIFFICULTY;
    scenarioMessageTemp.payload.widget.data.text = cDifficulty[m_mapHeader.difficulty];
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);
    scenarioMessageTemp.payload.widget.id = GAME_SCENARIO_SELECTED_DIFFICULTY;
    scenarioMessageTemp.payload.widget.data.text = cDifficulty[m_difficulty];
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

    sprintf(gText, "%d", CalcDifficultyRating());
    strcat(gText, "%");
    scenarioMessageTemp.payload.widget.id = GAME_SCENARIO_RATING;
    scenarioMessageTemp.payload.widget.data.text = gText;
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

    mapSizeIndex = MAP_SIZE_SMALL_INDEX;
    if (m_mapHeader.width == IDX(MAP_DIMENSION_MEDIUM))
        mapSizeIndex = MAP_SIZE_MEDIUM_INDEX;
    else if (m_mapHeader.width == IDX(MAP_DIMENSION_LARGE))
        mapSizeIndex = MAP_SIZE_LARGE_INDEX;
    else if (m_mapHeader.width == IDX(MAP_DIMENSION_XLARGE))
        mapSizeIndex = MAP_SIZE_XLARGE_INDEX;
    scenarioMessageTemp.payload.widget.id = GAME_SCENARIO_MAP_SIZE;
    scenarioMessageTemp.payload.widget.data.text = cMapSize[mapSizeIndex];
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

    scenarioMessageTemp.payload.widget.id = GAME_SCENARIO_DESCRIPTION;
    scenarioMessageTemp.payload.widget.data.text = m_mapHeader.description;
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);
    GetVictoryConditionText(gText);
    scenarioMessageTemp.payload.widget.id = GAME_SCENARIO_VICTORY;
    scenarioMessageTemp.payload.widget.data.text = gText;
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);
    GetLossConditionText(gText);
    scenarioMessageTemp.payload.widget.id = GAME_SCENARIO_LOSS;
    scenarioMessageTemp.payload.widget.data.text = gText;
    scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

    iconControlLocal = NULL;
    textControlLocal = NULL;
    availableWidthResult = PLAYER_AREA_WIDTH - m_mapHeader.playerCount * PLAYER_COLUMN_WIDTH;
    playerGapValue = availableWidthResult / (m_mapHeader.playerCount + 1);
    firstPlayerXLocal = playerGapValue + PLAYER_FIRST_X_OFFSET;
    playerSpacingTemp = playerGapValue + PLAYER_COLUMN_WIDTH;
    multiplayerYOffsetValue = 0;

    for (playerCounter = 0; playerCounter < m_mapHeader.playerCount; ++playerCounter) {
        if (giNumHumanPlayers > 1) {
            iconControlLocal = new iconWidget(
                static_cast<i16>(
                    OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal
                    + PLAYER_HUMAN_X_OFFSET
                ),
                static_cast<i16>(multiplayerYOffsetValue + SCENARIO_PLAYER_HUMAN_Y),
                PLAYER_HUMAN_WIDTH,
                PLAYER_HUMAN_HEIGHT,
                "ngextra.icn",
                PLAYER_HUMAN_FRAME,
                0,
                static_cast<i16>(playerCounter + NEW_GAME_PLAYER_HUMAN_FIRST),
                PLAYER_WIDGET_KIND,
                PLAYER_WIDGET_FILL_COLOR
            );
            if (iconControlLocal == NULL)
                MemError();
            scenarioWindowValue->AddWidget(iconControlLocal, -1);

            iconControlLocal = new iconWidget(
                static_cast<i16>(
                    OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal
                    + PLAYER_HANDICAP_X_OFFSET
                ),
                static_cast<i16>(multiplayerYOffsetValue + SCENARIO_PLAYER_HANDICAP_Y),
                PLAYER_HANDICAP_WIDTH,
                PLAYER_HANDICAP_HEIGHT,
                "ngextra.icn",
                0,
                0,
                static_cast<i16>(playerCounter + NEW_GAME_HANDICAP_FIRST),
                PLAYER_WIDGET_KIND,
                PLAYER_WIDGET_FILL_COLOR
            );
            if (iconControlLocal == NULL)
                MemError();
            scenarioWindowValue->AddWidget(iconControlLocal, -1);
        }

        iconControlLocal = new iconWidget(
            static_cast<i16>(OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal
                             + PLAYER_RACE_X_OFFSET),
            SCENARIO_PLAYER_RACE_Y,
            PLAYER_RACE_WIDTH,
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT - GAME_RACE_WIDGET_SINGLE_HEIGHT))
                + GAME_RACE_WIDGET_SINGLE_HEIGHT
            ),
            "ngextra.icn",
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (GAME_RACE_WIDGET_MULTIPLAYER_FRAME - GAME_RACE_WIDGET_SINGLE_FRAME))
                + GAME_RACE_WIDGET_SINGLE_FRAME
            ),
            0,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
            PLAYER_WIDGET_KIND,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControlLocal == NULL)
            MemError();
        scenarioWindowValue->AddWidget(iconControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<i16>(OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal
                             + PLAYER_COLOR_X_OFFSET),
            SCENARIO_PLAYER_COLOR_Y,
            PLAYER_COLOR_WIDTH,
            PLAYER_COLOR_HEIGHT,
            "ngextra.icn",
            static_cast<i16>(
                ((giNumHumanPlayers <= 1) - 1
                 & (GAME_COLOR_WIDGET_MULTIPLAYER_FRAME - GAME_COLOR_WIDGET_SINGLE_FRAME))
                + GAME_COLOR_WIDGET_SINGLE_FRAME
            ),
            0,
            static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
            PLAYER_WIDGET_KIND,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControlLocal == NULL)
            MemError();
        scenarioWindowValue->AddWidget(iconControlLocal, -1);

        if (giNumHumanPlayers > 1) {
            label = static_cast<char*>(
                H2_ALLOC(PLAYER_LABEL_CAPACITY, scenarioInfoSourceLineBase + 114)
            );
            sprintf(label, " ");
            textControlLocal = new textWidget(
                static_cast<i16>(
                    OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal
                    + PLAYER_NAME_X_OFFSET
                ),
                SCENARIO_PLAYER_NAME_Y,
                PLAYER_NAME_WIDTH,
                PLAYER_NAME_HEIGHT,
                label,
                "smalfont.fnt",
                FONT_DRAW_DEFAULT,
                static_cast<i16>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
                MESSAGE_WIDGET,
                FONT_ALIGN_CENTER
            );
            if (textControlLocal == NULL)
                MemError();
            scenarioWindowValue->AddWidget(textControlLocal, -1);
        }

        singlePlayerYOffsetValue = 0;
        iconControlLocal = new iconWidget(
            static_cast<i16>(OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal
                             + PLAYER_RACE_ICON_X_OFFSET),
            static_cast<i16>(OD_STEER(multiplayerYOffsetValue) + singlePlayerYOffsetValue
                             + SCENARIO_PLAYER_RACE_ICON_Y),
            PLAYER_RACE_ICON_WIDTH,
            PLAYER_RACE_ICON_HEIGHT,
            "ngextra.icn",
            PLAYER_RACE_ICON_FRAME,
            0,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
            PLAYER_WIDGET_KIND,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControlLocal == NULL)
            MemError();
        scenarioWindowValue->AddWidget(iconControlLocal, -1);

        label = static_cast<char*>(
            H2_ALLOC(PLAYER_LABEL_CAPACITY, scenarioInfoSourceLineBase + 146)
        );
        sprintf(label, "A");
        if (m_mapHeader.playerCount >= PLAYER_RACE_NAME_NARROW_THRESHOLD) {
            if (m_mapHeader.playerCount >= MAP_HEADER_PLAYER_COUNT)
                raceTextWidth = 0;
            else
                raceTextWidth = PLAYER_RACE_NAME_NARROW_WIDTH;
        } else {
            raceTextWidth = PLAYER_RACE_NAME_WIDE_WIDTH;
        }
        textControlLocal = new textWidget(
            static_cast<i16>(
                OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal
                + PLAYER_RACE_NAME_X_OFFSET - raceTextWidth / PLAYER_RACE_NAME_CENTER_DIVISOR
            ),
            static_cast<i16>(OD_STEER(multiplayerYOffsetValue) + singlePlayerYOffsetValue
                             + SCENARIO_PLAYER_RACE_NAME_Y),
            static_cast<i16>(raceTextWidth + PLAYER_RACE_NAME_BASE_WIDTH),
            PLAYER_RACE_NAME_HEIGHT,
            label,
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_NAME_FIRST),
            MESSAGE_WIDGET,
            FONT_ALIGN_CENTER
        );
        if (textControlLocal == NULL)
            MemError();
        scenarioWindowValue->AddWidget(textControlLocal, -1);

        iconControlLocal = new iconWidget(
            static_cast<i16>(OD_STEER(playerSpacingTemp) * playerCounter + firstPlayerXLocal
                             + PLAYER_RACE_CYCLE_X_OFFSET),
            static_cast<i16>(OD_STEER(multiplayerYOffsetValue) + singlePlayerYOffsetValue
                             + SCENARIO_PLAYER_RACE_CYCLE_Y),
            PLAYER_RACE_CYCLE_WIDTH,
            PLAYER_RACE_CYCLE_HEIGHT,
            "ngextra.icn",
            PLAYER_RACE_CYCLE_FRAME,
            0,
            static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
            PLAYER_WIDGET_KIND,
            PLAYER_WIDGET_FILL_COLOR
        );
        if (iconControlLocal == NULL)
            MemError();
        scenarioWindowValue->AddWidget(iconControlLocal, -1);
    }

    for (playerCounter = 0; playerCounter < m_mapHeader.playerCount; ++playerCounter) {
        if (m_setupPlayerNetworkId[playerCounter] == GAME_COMPUTER_PLAYER) {
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
        scenarioMessageTemp.payload.widget.data.value = GAME_WIDGET_REFRESH_FRAME;
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

        if (m_setupPlayerType[playerCounter] != GAME_PLAYER_DEFAULT
            || (giNumHumanPlayers > 1
                && m_setupPlayerNetworkId[playerCounter] != GAME_COMPUTER_PLAYER))
            playerLockedLocal = 0;
        else
            playerLockedLocal = 1;
        scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        scenarioMessageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_COLOR_FIRST, playerCounter);
        if (m_setupPlayerNetworkId[playerCounter] == GAME_COMPUTER_PLAYER)
            scenarioMessageTemp.payload.widget.data.value =
                m_setupPlayerColor[playerCounter]
                + (playerLockedLocal ? GAME_COMPUTER_COLOR_LOCKED_FRAME
                                     : GAME_COMPUTER_COLOR_UNLOCKED_FRAME);
        else
            scenarioMessageTemp.payload.widget.data.value =
                m_setupPlayerColor[playerCounter]
                + (playerLockedLocal ? GAME_HUMAN_COLOR_LOCKED_FRAME
                                     : GAME_HUMAN_COLOR_UNLOCKED_FRAME);
        if (giNumHumanPlayers > 1)
            scenarioMessageTemp.payload.widget.data.value +=
                GAME_MULTIPLAYER_COLOR_FRAME_OFFSET;
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

        if (playerLockedLocal)
            scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        else
            scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        scenarioMessageTemp.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

        scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        scenarioMessageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_HANDICAP_FIRST, playerCounter);
        if (m_setupPlayerNetworkId[playerCounter] == GAME_COMPUTER_PLAYER)
            scenarioMessageTemp.payload.widget.data.value = NEW_GAME_RACE_NAME_FIRST;
        else
            scenarioMessageTemp.payload.widget.data.value = m_playerHandicap[playerCounter];
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);
        if (m_setupPlayerNetworkId[playerCounter] == GAME_COMPUTER_PLAYER)
            scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
        else
            scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
        scenarioMessageTemp.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
        scenarioWindowValue->BroadcastMessage(scenarioMessageTemp);

        scenarioMessageTemp.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
        scenarioMessageTemp.payload.widget.id =
            EncodeNewGameControlIndex(NEW_GAME_RACE_CYCLE_FIRST, playerCounter);
        scenarioMessageTemp.payload.widget.data.value =
            m_setupPlayerRace[playerCounter]
            + (playerLockedLocal ? GAME_FIXED_RACE_FRAME_BASE
                                 : GAME_RANDOM_RACE_FRAME_BASE);
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

VA(0x004baf0d, 0x1c7)
void game::GetLossConditionText(char* text) {
    i32 week2;
    hero* lossHero11;
    i32 day26;
    i32 month19;
    town* lossTown9;
    i32 townId12;

    if (m_mapHeader.lossCondition != MAP_LOSS_STANDARD) {
        switch (m_mapHeader.lossCondition) {
            case MAP_LOSS_TOWN:
                townId12 = GetTownId(m_mapHeader.lossConditionValue, m_mapHeader.lossTownY);
                lossTown9 = GetTown(townId12);
                sprintf(
                    text,
                    "Lose the %s '%s'.",
                    (lossTown9->m_buildings & IDX(TOWN_BUILDING_CASTLE)) ? "castle" : "town",
                    lossTown9->m_name
                );
                break;

            case MAP_LOSS_HERO:
                lossHero11 = GetHero(m_mapHeader.lossConditionValue);
                sprintf(text, "Lose the hero '%s'.", lossHero11->m_name);
                break;

            case MAP_LOSS_TIME:
                month19 =
                    (gpGame->m_mapHeader.lossConditionValue - 1) / GAME_DAYS_PER_MONTH + 1;
                week2 = (gpGame->m_mapHeader.lossConditionValue
                         - (month19 - 1) * GAME_DAYS_PER_MONTH - 1)
                            / GAME_DAYS_PER_WEEK
                        + 1;
                day26 = (gpGame->m_mapHeader.lossConditionValue - 1) % GAME_DAYS_PER_WEEK + 1;
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
    char firstSide[GAME_SIDE_TEXT_SIZE];
    char secondSideValue[GAME_SIDE_TEXT_SIZE];
    town* victoryTown;
    i32 townId;

    if (m_mapHeader.victoryCondition != MAP_VICTORY_DEFEAT_ALL) {
        switch (m_mapHeader.victoryCondition) {
            case MAP_VICTORY_CAPTURE_TOWN:
                townId = GetTownId(m_mapHeader.victoryConditionValue, m_mapHeader.victoryTownY);
                victoryTown = GetTown(townId);
                sprintf(
                    text,
                    "Capture the %s '%s'",
                    (victoryTown->m_buildings & IDX(TOWN_BUILDING_CASTLE)) ? "castle" : "town",
                    victoryTown->m_name
                );
                break;

            case MAP_VICTORY_DEFEAT_HERO:
                victoryHeroData = GetHero(m_mapHeader.victoryConditionValue);
                sprintf(text, "Defeat the hero '%s'", victoryHeroData->m_name);
                break;

            case MAP_VICTORY_FIND_ARTIFACT:
                if (m_mapHeader.victoryConditionValue == 0)
                    sprintf(text, "Find the ultimate artifact");
                else
                    sprintf(
                        text,
                        "Find the %s",
                        gArtifactNames[m_mapHeader.victoryConditionValue - 1]
                    );
                break;

            case MAP_VICTORY_ACCUMULATE_GOLD:
                sprintf(
                    text,
                    "Accumulate %d gold",
                    m_mapHeader.victoryConditionValue * GAME_GOLD_CONDITION_MULTIPLIER
                );
                break;

            case MAP_VICTORY_DEFEAT_SIDE:
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

        if (m_mapHeader.victoryCondition != MAP_VICTORY_DEFEAT_SIDE
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
    char colorName3[GAME_SIDE_TEXT_SIZE];
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

DATA(0x0051cd20) b32 gbNewGameDialogOver = true;
DATA(0x0051cd24) i32 NGKPcursorFlashOn = 1;
DATA(0x00533150) char* cNGKPDisplay;
DATA(0x00533154) b32 gbNewGameShadowHidden;
DATA(0x00533158) char* cNGKPCore;
DATA(0x0053315c) i32 NGKPcursorIndex;
DATA(0x00533160) char* cTextReceivedBuffer[GAME_TEXT_BUFFER_COUNT];
DATA(0x0053316c) class icon* NGKPBkg;

#undef RETAIL_FILE
