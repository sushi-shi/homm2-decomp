#include <Ints.h>
#include <fcntl.h>
#include <PLATFORM/File.h>
#include <stdio.h>
#include <string.h>
#include <BASE/Misc.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <BASE/widgetKind.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/fileRequester.h>
#include <SOURCE/game.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/REQUEST.h>

enum class FileRequesterHelpIndex : i32 {
    REQUESTER_HELP_NONE          = -1,
    REQUESTER_HELP_VALID_BEGIN   = 0,
    REQUESTER_HELP_FILTER_SMALL  = REQUESTER_HELP_VALID_BEGIN,
    REQUESTER_HELP_FILTER_MEDIUM = 1,
    REQUESTER_HELP_FILTER_LARGE  = 2,
    REQUESTER_HELP_FILTER_XLARGE = 3,
    REQUESTER_HELP_FILTER_ALL    = 4,
    REQUESTER_HELP_FILENAME      = 5,
    REQUESTER_HELP_OK            = 6,
    REQUESTER_HELP_CANCEL        = 7,
    REQUESTER_HELP_MAP_SIZE      = 8,
    REQUESTER_HELP_PLAYER_COUNT  = 9,
    REQUESTER_HELP_VICTORY       = 10,
    REQUESTER_HELP_LOSS          = 11,
    REQUESTER_HELP_MAP_NAME      = 12,
    REQUESTER_HELP_DESCRIPTION   = 13,
    REQUESTER_HELP_DIFFICULTY    = 14
};
using enum FileRequesterHelpIndex;

enum class RequesterIconFrame : i32 {
    PLAYER_COUNT_FRAME_BASE = 0x13,
    MAP_SIZE_FRAME_SMALL    = 0x1a,
    MAP_SIZE_FRAME_MEDIUM   = 0x1b,
    MAP_SIZE_FRAME_LARGE    = 0x1c,
    MAP_SIZE_FRAME_XLARGE   = 0x1d,
    VICTORY_FRAME_BASE      = 0x1e,
    LOSS_FRAME_BASE         = 0x24
};
using enum RequesterIconFrame;

typedef enum FileRequesterPrivateConstant {
    LEGACY_MAP_BASENAME_SIZE    = 8,
    MAP_LIST_GUTTER_TRAVEL      = 123,
    STANDARD_LIST_GUTTER_TRAVEL = 163,
    GUTTER_MIN_Y                = 73,
    MAP_LIST_VISIBLE_COUNT      = 9,
    STANDARD_LIST_VISIBLE_COUNT = 11,
    RESULT_PENDING              = -2,
    SCROLL_KNOB_X               = 346,
    SCROLL_KNOB_WIDTH           = 8,
    SCROLL_KNOB_HEIGHT          = 17,
    SCROLL_KNOB_FRAME           = 4,
    CURRENT_MAP_NAME_CAPACITY   = 12,
    CURRENT_MAP_NAME_CLEAR_SIZE = LEGACY_MAP_BASENAME_SIZE + 1,
    FILENAME_ENTRY_LIMIT        = 201,
    FILTER_FRAME_STEP           = 2,
    FILTER_FRAME_BASE           = 9,
    SELECTED_FILL_COLOR         = 2,
    SCROLL_CENTER_DIVISOR       = 2,
    CP1251_UPPER_FIRST          = 0xc0,
    CP1251_UPPER_LAST           = 0xdf,
    CP1251_LOWER_FIRST          = 0xe0,
    CP1251_LOWER_LAST           = 0xff,
    CP1251_YO_UPPER             = 0xa8,
    CP1251_YO_LOWER             = 0xb8
} FileRequesterPrivateConstant;

i32 GetMapHeader(const char* filename, struct SMapHeader* header) {
    sprintf(gText, "%s%s", gcMapPath, filename);
    i32 file = platform::FileOpen(gText, platform::FileMode::Read);
    if (file == -1) {
        return 0;
    }
    platform::FileRead(file, header, sizeof(*header));
    platform::FileClose(file);
    return 1;
}

i32 CheckSumIsDemoOK(const char*) {
    return 1;
}

i32 ShowThisMapGame(const char* filename) {
    return 1;

    char mapName[FILE_REQUESTER_PATH_SIZE];
    i32 ix;
    strcpy(mapName, filename);
    mapName[LEGACY_MAP_BASENAME_SIZE] = 0;
    for (ix = 0; ix < LEGACY_MAP_BASENAME_SIZE; ++ix) {
        if (mapName[ix] == '.') {
            mapName[ix] = 0;
        }
    }
    if (strcmpi(mapName, "BROKENA") == 0 && CheckSumIsDemoOK(filename)) {
        return 1;
    }
    return 0;
}

i32 ShowThisMap(const char*) {
    return 1;
}

i32 fileRequester::InitializeFiles(char* directory, char* pattern, i32 countOnly) {
    SMapHeader header;
    char nameBuffer[FILE_REQUESTER_LOCAL_NAME_SIZE];
    i32 insertCount;
    char* dotPtr;
    char extension[FILE_REQUESTER_EXTENSION_SIZE];
    i32 indexData5;
    i32 moveValue;
    char fullPath[FILE_REQUESTER_PATH_SIZE];

    sprintf(gText, "%s%s", directory, pattern);
    const std::vector<std::string> files = platform::Files().List(gText);
    const auto accepts = [&](const std::string& foundFile) {
        if (m_mode != FILE_REQUESTER_MAP && m_mode != FILE_REQUESTER_MAP_GAME)
            return true;

        GetMapHeader(foundFile.c_str(), &header);
        if (m_mode == FILE_REQUESTER_MAP_GAME
            && (header.minHumanPlayers > giNumHumanPlayers
                || header.maxHumanPlayers < giNumHumanPlayers))
            return false;
        if (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL
            && header.width != giMapSizes[H2EnumIndex(giMapSizeFilter)])
            return false;
        return m_mode == FILE_REQUESTER_MAP_GAME ? ShowThisMapGame(foundFile.c_str()) != 0
                                                : ShowThisMap(foundFile.c_str()) != 0;
    };

    m_fileCount = 0;
    for (const std::string& foundFile : files) {
        if (accepts(foundFile))
            ++m_fileCount;
    }

    if (countOnly) {
        return m_fileCount;
    }

    m_fileNames = new FileRequesterName[m_fileCount + 1];
    if (m_fileNames == NULL) {
        MemError();
    }
    m_extensions = new FileRequesterExtension[m_fileCount + 1];
    if (m_extensions == NULL) {
        MemError();
    }
    if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
        m_mapHeaders = new SMapHeader[m_fileCount];
        if (m_mapHeaders == NULL) {
            MemError();
        }
    }

    for (indexData5 = 0; indexData5 < m_fileCount; ++indexData5) {
        strcpy(
            m_fileNames[indexData5].text,
            ""
        );
        strcpy(
            m_extensions[indexData5].text,
            ""
        );
    }

    insertCount = 0;
    for (const std::string& foundFile : files) {
        if (!accepts(foundFile))
            continue;

        strcpy(nameBuffer, foundFile.c_str());
        extension[0] = 0;
        dotPtr = FindLastToken(nameBuffer, '.');
        if (dotPtr != NULL) {
            strcpy(extension, dotPtr);
            *dotPtr = 0;
        }

        for (indexData5 = 0; indexData5 < insertCount; ++indexData5) {
            if (strcmpi(nameBuffer, m_fileNames[indexData5].text) < 0) {
                for (moveValue = insertCount; moveValue > indexData5; --moveValue) {
                    strcpy(m_fileNames[moveValue].text, m_fileNames[moveValue - 1].text);
                    strcpy(m_extensions[moveValue].text, m_extensions[moveValue - 1].text);
                }
                break;
            }
        }
        strcpy(m_fileNames[indexData5].text, nameBuffer);
        strcpy(m_extensions[indexData5].text, extension);
        ++insertCount;
    }

    if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
        for (indexData5 = 0; indexData5 < insertCount; ++indexData5) {
            sprintf(fullPath, "%s%s", m_fileNames[indexData5].text, m_extensions[indexData5].text);
            GetMapHeader(fullPath, &m_mapHeaders[indexData5]);
        }
    }
    return m_fileCount;
}

fileRequester::fileRequester(
    i32 x,
    i32 y,
    FileRequesterMode mode,
    char* pattern,
    char* directory,
    char* defaultExtension
) {
    strcpy(m_filePattern, pattern);
    strcpy(m_directory, directory);
    m_selectedIndex = FILE_REQUESTER_SELECTION_NONE;
    m_fileCount = 0;
    m_topIndex = 0;
    m_fileNames = NULL;
    m_extensions = NULL;
    m_mapHeaders = NULL;
    m_x = x;
    m_y = y;
    m_mode = mode;
    strcpy(m_defaultExtension, defaultExtension);
    if (mode == FILE_REQUESTER_MAP_GAME || mode == FILE_REQUESTER_MAP) {
        fGutterTravelLength = MAP_LIST_GUTTER_TRAVEL;
        fGutterMinY = GUTTER_MIN_Y;
        iMaxListSize = MAP_LIST_VISIBLE_COUNT;
    } else {
        fGutterTravelLength = STANDARD_LIST_GUTTER_TRAVEL;
        fGutterMinY = GUTTER_MIN_Y;
        iMaxListSize = STANDARD_LIST_VISIBLE_COUNT;
    }
    if (!MapExistsForFilter(giMapSizeFilter)) {
        giMapSizeFilter = FILE_REQUESTER_MAP_SIZE_ALL;
    }
    InitializeFiles(m_directory, m_filePattern, 0);
    m_result = RESULT_PENDING;
}

i32 fileRequester::MapExistsForFilter(FileRequesterMapSizeFilter filter) {
    FileRequesterMapSizeFilter oldFilter = giMapSizeFilter;
    giMapSizeFilter = filter;
    i32 result = InitializeFiles(m_directory, m_filePattern, 1);
    giMapSizeFilter = oldFilter;
    return result > 0;
}

void fileRequester::SetupFiles(void) {
    CleanUpData();
    m_fileCount = 0;
    m_topIndex = 0;
    m_result = RESULT_PENDING;
    m_selectedIndex = FILE_REQUESTER_SELECTION_NONE;
    InitializeFiles(m_directory, m_filePattern, 0);
}

void fileRequester::CleanUpData(void) {
    if (m_fileNames != NULL) {
        delete[] m_fileNames;
    }
    m_fileNames = NULL;
    if (m_extensions != NULL) {
        delete[] m_extensions;
    }
    m_extensions = NULL;
    if (m_mapHeaders != NULL) {
        delete[] m_mapHeaders;
    }
    m_mapHeaders = NULL;
}

void fileRequester::Close(void) {
    if (!m_active) {
        return;
    }
    platform::ChangeMenu(m_previousMenu);
    strcpy(gLastFilename, GetFilename());
    CleanUpData();
    gpWindowManager->RemoveWindow(m_window);
    delete m_window;
    m_active = false;
}

i32 fileRequester::Open(i32 id) {
    strcpy(
        gLastFilename,
        ""
    );
    m_previousMenu = platform::CurrentMenu();
    platform::ChangeMenu(hmnuDflt);

    m_window = new heroWindow(
        m_x,
        m_y,
        const_cast<char*>(
            m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP ? "requests.bin"
                                                                              : "request.bin"
        )
    );
    if (m_window == NULL) {
        MemError();
    }

    m_scrollKnob = new iconWidget(
        SCROLL_KNOB_X,
        static_cast<i16>(fGutterMinY),
        SCROLL_KNOB_WIDTH,
        SCROLL_KNOB_HEIGHT,
        "scrollcn.icn",
        SCROLL_KNOB_FRAME,
        ICON_DRAW_NORMAL,
        FILE_REQUESTER_SCROLL_KNOB,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_scrollKnob == NULL) {
        MemError();
    }
    m_window->AddWidget(m_scrollKnob, -1);

    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    u8 enabled;
    i32 fileSlot;
    char* dot;
    if (m_mode == FILE_REQUESTER_SAVE_GAME) {
        enabled = 1;
        strcpy(m_filename, gpGame->m_saveName);
        dot = FindLastToken(m_filename, '.');
        if (dot != NULL) {
            *dot = 0;
        }
        message.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
        message.payload.widget.data.text = m_filename;
        m_window->BroadcastMessage(message);
        message.payload.widget.id = FILE_REQUESTER_FILENAME_LABEL;
        sprintf(
            gText,
              "\xd1\xee\xf5\xf0\xe0\xed\xe8\xf2\xfc\x20\xf4\xe0\xe9\xeb\x3a"
        );
        message.payload.widget.data.text = gText;
        m_window->BroadcastMessage(message);
        for (fileSlot = 0; fileSlot < m_fileCount; ++fileSlot) {
            if (strcmpi(m_fileNames[fileSlot].text, m_filename) == 0) {
                m_selectedIndex = fileSlot;
            }
        }
    } else {
        enabled = 0;
        if (m_mode == FILE_REQUESTER_MAP_GAME) {
            char mapName[CURRENT_MAP_NAME_CAPACITY];
            fileSlot = 0;
            memset(mapName, 0, CURRENT_MAP_NAME_CLEAR_SIZE);
            while (fileSlot < LEGACY_MAP_BASENAME_SIZE && gMapName[fileSlot] != 0
                   && gMapName[fileSlot] != '.') {
                mapName[fileSlot] = gMapName[fileSlot];
                ++fileSlot;
            }
            for (fileSlot = 0; fileSlot < m_fileCount; ++fileSlot) {
                if (strcmpi(m_fileNames[fileSlot].text, mapName) == 0) {
                    m_selectedIndex = fileSlot;
                    enabled = 1;
                }
            }
        }
        message.payload.widget.id = FILE_REQUESTER_FILENAME_LABEL;
        sprintf(
            gText,
              "\xc7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc\x20\xf4\xe0\xe9\xeb\x3a"
        );
        message.payload.widget.data.text = gText;
        m_window->BroadcastMessage(message);
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_MAX_LENGTH;
    message.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
    message.payload.widget.data.value = FILENAME_ENTRY_LIMIT;
    m_window->BroadcastMessage(message);
    Update(0);
    if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
        enabled = 1;
    }
    gpWindowManager->AddWindow(m_window, -1, 1);
    if (m_fileCount == 0) {
        enabled = 0;
    }
    if (m_mode == FILE_REQUESTER_SAVE_GAME
        && strcmpi(
               m_filename,
                 "\xcd\xce\xc2\xc0\xdf\x20\xc8\xc3\xd0\xc0"
           )
               == 0
        && m_selectedIndex == FILE_REQUESTER_SELECTION_NONE) {
        enabled = 1;
    }
    SetOK(enabled);
    m_messageMask = BASE_MANAGER_ACCEPT_EXECUTIVE;
    m_priority = id;
    m_active = true;
    strcpy(m_name, "fileRequester");
    return 0;
}

void fileRequester::SetOK(i32 enabled) {
    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = enabled ? WIDGET_COMMAND_CLEAR_FLAGS : WIDGET_COMMAND_SET_FLAGS;
    message.payload.widget.id = FILE_REQUESTER_OK;
    message.payload.widget.data.value = m_active == 1 ? H2EnumIndex(WIDGET_FLAG_DIMMED) : H2EnumIndex(WIDGET_FLAG_GRAYED);
    m_window->BroadcastMessage(message);
    message.payload.widget.command = enabled ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
    m_window->BroadcastMessage(message);
}

MessageDispatchResult fileRequester::Main(struct tag_message& message) {
    u8 newNameData[FILE_REQUESTER_LOCAL_NAME_SIZE];
    i32 screenY;
    i32 mouseX;
    i32 acceptStep = 0;
    i32 iResult;
    i32 lengthIndex;
    FileRequesterHelpIndex helpIndexMouse;
    char cycleNameBuffer[FILE_REQUESTER_PATH_SIZE];
    i32 positions;
    tag_message broadcastMessage;
    char filteredNameMap[FILE_REQUESTER_PATH_SIZE];
    i32 topIndexValue;
    i32 stepScreen;

    switch (message.type) {
        case MESSAGE_KEY_DOWN:
            switch (message.payload.keyboard.keyCode) {
                case INPUT_SCAN_F6: {
                    if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
                        strcpy(cycleNameBuffer, m_fileNames[m_selectedIndex].text);
                    } else {
                        strcpy(
                            cycleNameBuffer,
                            ""
                        );
                    }
                    giMapSizeFilter = static_cast<FileRequesterMapSizeFilter>(
                        (H2EnumIndex(giMapSizeFilter) + 1) % H2EnumIndex(FILE_REQUESTER_MAP_SIZE_COUNT)
                    );
                    SetupFiles();
                    if (strlen(cycleNameBuffer) != 0) {
                        for (iResult = 0; iResult < m_fileCount; ++iResult) {
                            if (strcmpi(m_fileNames[iResult].text, cycleNameBuffer) == 0) {
                                m_selectedIndex = iResult;
                            }
                        }
                    }
                    Update(1);
                    break;
                }
                case INPUT_SCAN_NUMPAD_8:
                    if (m_selectedIndex > 0) {
                        --m_selectedIndex;
                        if (m_topIndex > m_selectedIndex) {
                            --m_topIndex;
                        }
                        Update(1);
                    }
                    break;
                case INPUT_SCAN_NUMPAD_2:
                    if (m_selectedIndex < m_fileCount - 1) {
                        ++m_selectedIndex;
                        if (m_topIndex + iMaxListSize <= m_selectedIndex) {
                            ++m_topIndex;
                        }
                        Update(1);
                    }
                    break;
            }
            break;
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_DESELECT:
                    switch (message.payload.widget.id) {
                        case FILE_REQUESTER_SCROLL_UP:
                            if (m_topIndex > 0) {
                                --m_topIndex;
                                Update(1);
                            }
                            break;
                        case FILE_REQUESTER_SCROLL_DOWN:
                            if (m_topIndex + iMaxListSize < m_fileCount) {
                                ++m_topIndex;
                                if (m_topIndex + iMaxListSize - 1 >= m_fileCount) {
                                    m_topIndex = m_fileCount - iMaxListSize;
                                }
                                Update(1);
                            }
                            break;
                        case FILE_REQUESTER_OK:
                            if (m_selectedIndex == FILE_REQUESTER_SELECTION_NONE
                                && m_filename[0] == 0) {
                                NormalDialog(
                                    "\xc2\xfb\xe1\xe5\xf0\xe8\xf2\xe5 \xe8\xe7 \xf1\xef\xe8\xf1\xea"
                                    "\xe0 \xe8\xeb\xe8 \xed\xe0\xe6\xec\xe8\xf2\xe5 \xea\xed\xee"
                                    "\xef\xea\xf3 \xee\xf2\xec\xe5\xed\xfb."

                                    ,
                                    NORMAL_DIALOG_INFO,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    NORMAL_DIALOG_NO_VALUE,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0
                                );
                                break;
                            }
                            message.payload.widget.data.value = message.payload.widget.id;
                            acceptStep = 1;
                            break;
                        case FILE_REQUESTER_CANCEL:
                            message.payload.widget.data.value = message.payload.widget.id;
                            acceptStep = 1;
                            break;
                    }
                    break;
                case WIDGET_COMMAND_SELECT:
                case WIDGET_COMMAND_ALTERNATE_SELECT:
                    if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                        helpIndexMouse = REQUESTER_HELP_NONE;
                        switch (message.payload.widget.id) {
                            case FILE_REQUESTER_FILTER_SMALL:
                                helpIndexMouse = REQUESTER_HELP_FILTER_SMALL;
                                break;
                            case FILE_REQUESTER_FILTER_MEDIUM:
                                helpIndexMouse = REQUESTER_HELP_FILTER_MEDIUM;
                                break;
                            case FILE_REQUESTER_FILTER_LARGE:
                                helpIndexMouse = REQUESTER_HELP_FILTER_LARGE;
                                break;
                            case FILE_REQUESTER_FILTER_XLARGE:
                                helpIndexMouse = REQUESTER_HELP_FILTER_XLARGE;
                                break;
                            case FILE_REQUESTER_FILTER_ALL:
                                helpIndexMouse = REQUESTER_HELP_FILTER_ALL;
                                break;
                            case FILE_REQUESTER_FILENAME_ENTRY:
                                helpIndexMouse = REQUESTER_HELP_FILENAME;
                                break;
                            case FILE_REQUESTER_OK:
                                helpIndexMouse = REQUESTER_HELP_OK;
                                break;
                            case FILE_REQUESTER_CANCEL:
                                helpIndexMouse = REQUESTER_HELP_CANCEL;
                                break;
                            case FILE_REQUESTER_MAP_SIZE:
                                helpIndexMouse = REQUESTER_HELP_MAP_SIZE;
                                break;
                            case FILE_REQUESTER_MAP_PLAYER_COUNT:
                                helpIndexMouse = REQUESTER_HELP_PLAYER_COUNT;
                                break;
                            case FILE_REQUESTER_MAP_VICTORY:
                                helpIndexMouse = REQUESTER_HELP_VICTORY;
                                break;
                            case FILE_REQUESTER_MAP_LOSS:
                                helpIndexMouse = REQUESTER_HELP_LOSS;
                                break;
                            case FILE_REQUESTER_MAP_NAME:
                                helpIndexMouse = REQUESTER_HELP_MAP_NAME;
                                break;
                            case FILE_REQUESTER_MAP_DESCRIPTION:
                                helpIndexMouse = REQUESTER_HELP_DESCRIPTION;
                                break;
                            case FILE_REQUESTER_MAP_DIFFICULTY_ICON:
                                helpIndexMouse = REQUESTER_HELP_DIFFICULTY;
                                break;
                            case FILE_REQUESTER_MAP_DIFFICULTY_TEXT:
                                helpIndexMouse = REQUESTER_HELP_DIFFICULTY;
                                break;
                            default:
                                if (message.payload.widget.id >= FILE_REQUESTER_MAP_SIZE_ICON_FIRST
                                    && message.payload.widget.id
                                           < FILE_REQUESTER_MAP_SIZE_ICON_FIRST
                                                 + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    helpIndexMouse = REQUESTER_HELP_MAP_SIZE;
                                } else if (message.payload.widget.id
                                               >= FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
                                           && message.payload.widget.id
                                                  < FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
                                                        + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    helpIndexMouse = REQUESTER_HELP_PLAYER_COUNT;
                                } else if (message.payload.widget.id
                                               >= FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
                                           && message.payload.widget.id
                                                  < FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
                                                        + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    helpIndexMouse = REQUESTER_HELP_VICTORY;
                                } else if (message.payload.widget.id
                                               >= FILE_REQUESTER_MAP_LOSS_ICON_FIRST
                                           && message.payload.widget.id
                                                  < FILE_REQUESTER_MAP_LOSS_ICON_FIRST
                                                        + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    helpIndexMouse = REQUESTER_HELP_LOSS;
                                }
                                break;
                        }
                        if (helpIndexMouse >= REQUESTER_HELP_VALID_BEGIN) {
                            NormalDialog(
                                gFileRequestHelp[H2EnumIndex(helpIndexMouse)],
                                NORMAL_DIALOG_QUICK_VIEW,
                                NORMAL_DIALOG_NO_RESOURCE,
                                NORMAL_DIALOG_NO_VALUE,
                                NORMAL_DIALOG_NO_RESOURCE,
                                0,
                                NORMAL_DIALOG_NO_RESOURCE,
                                0,
                                NORMAL_DIALOG_NO_RESOURCE,
                                0
                            );
                        }
                    } else {
                        switch (message.payload.widget.id) {
                            case FILE_REQUESTER_FILTER_SMALL:
                            case FILE_REQUESTER_FILTER_MEDIUM:
                            case FILE_REQUESTER_FILTER_LARGE:
                            case FILE_REQUESTER_FILTER_XLARGE:
                            case FILE_REQUESTER_FILTER_ALL: {
                                iResult = message.payload.widget.id - FILE_REQUESTER_FILTER_SMALL;
                                if (!MapExistsForFilter(
                                        static_cast<FileRequesterMapSizeFilter>(iResult)
                                    )) {
                                    if (giNumHumanPlayers == 1) {
                                        sprintf(
                                            gText,
                                            "\xcd\xe5\xf2 \xea\xe0\xf0\xf2 \xf2\xe0\xea\xee\xe3\xee"
                                            " \xf0\xe0\xe7\xec\xe5\xf0\xe0 \xe4\xeb\xff %d \xe8\xe3"
                                            "\xf0\xee\xea\xee\xe2-\xeb\xfe\xe4\xe5\xe9."

                                            ,
                                            giNumHumanPlayers
                                        );
                                    } else {
                                        sprintf(
                                            gText,
                                            "\xcd\xe5\xf2 \xea\xe0\xf0\xf2\xfb \xfd\xf2\xee\xe3\xee"
                                            " \xf0\xe0\xe7\xec\xe5\xf0\xe0 \xe4\xeb\xff %d \xe8\xe3"
                                            "\xf0\xee\xea\xee\xe2-\xeb\xfe\xe4\xe5\xe9."

                                            ,
                                            giNumHumanPlayers
                                        );
                                    }
                                    NormalDialog(
                                        gText,
                                        NORMAL_DIALOG_INFO,
                                        NORMAL_DIALOG_NO_RESOURCE,
                                        NORMAL_DIALOG_NO_VALUE,
                                        NORMAL_DIALOG_NO_RESOURCE,
                                        0,
                                        NORMAL_DIALOG_NO_RESOURCE,
                                        0,
                                        NORMAL_DIALOG_NO_RESOURCE,
                                        0
                                    );
                                    break;
                                }
                                giMapSizeFilter = static_cast<FileRequesterMapSizeFilter>(iResult);
                                if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
                                    strcpy(filteredNameMap, m_fileNames[m_selectedIndex].text);
                                } else {
                                    strcpy(
                                        filteredNameMap,
                                        ""
                                    );
                                }
                                SetupFiles();
                                if (strlen(filteredNameMap) != 0) {
                                    for (iResult = 0; iResult < m_fileCount; ++iResult) {
                                        if (strcmpi(m_fileNames[iResult].text, filteredNameMap)
                                            == 0) {
                                            m_selectedIndex = iResult;
                                        }
                                    }
                                }
                                Update(1);
                                break;
                            }
                            case FILE_REQUESTER_FILENAME_ENTRY: {
                                broadcastMessage.type = MESSAGE_WIDGET;
                                broadcastMessage.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                                broadcastMessage.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
                                m_window->BroadcastMessage(broadcastMessage);

                                memset(newNameData, 0, FILE_REQUESTER_FILENAME_INITIAL_CLEAR_SIZE);
                                strcpy(reinterpret_cast<char*>(newNameData), broadcastMessage.payload.widget.data.text);
                                lengthIndex = strlen(reinterpret_cast<char*>(newNameData));
                                for (iResult = 0; iResult < lengthIndex; ++iResult) {
                                    if (!((newNameData[iResult] >= 'A'
                                           && newNameData[iResult] <= 'Z')
                                          || (newNameData[iResult] >= 'a'
                                              && newNameData[iResult] <= 'z')
                                          || (newNameData[iResult] >= '0'
                                              && newNameData[iResult] <= '9')
                                          || (newNameData[iResult] >= CP1251_UPPER_FIRST
                                              && newNameData[iResult] <= CP1251_UPPER_LAST)
                                          || (newNameData[iResult] >= CP1251_LOWER_FIRST
                                              && newNameData[iResult] <= CP1251_LOWER_LAST)
                                          || newNameData[iResult] == CP1251_YO_UPPER
                                          || newNameData[iResult] == CP1251_YO_LOWER
                                          || newNameData[iResult] == '_'
                                          || newNameData[iResult] == ' '
                                          || FindToken(
                                                 "$%'-_@~`!(){}^#&+,;=[].",
                                                 newNameData[iResult]
                                             ) != NULL)) {
                                        newNameData[iResult] = 0;
                                    }
                                }
                                for (iResult = strlen(reinterpret_cast<char*>(newNameData)) - 1; iResult >= 0; --iResult) {
                                    if (newNameData[iResult] == ' ')
                                        newNameData[iResult] = 0;
                                    else
                                        iResult = -1;
                                }
                                if (strlen(reinterpret_cast<char*>(newNameData)) > 0 && newNameData[0] > ' ') {
                                    m_selectedIndex = FILE_REQUESTER_SELECTION_NONE;
                                    strcpy(m_filename, reinterpret_cast<char*>(newNameData));
                                    SetOK(1);
                                }
                                broadcastMessage.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                                broadcastMessage.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
                                broadcastMessage.payload.widget.data.text = m_filename;
                                m_window->BroadcastMessage(broadcastMessage);
                                Update(1);
                                break;
                            }
                            case FILE_REQUESTER_SCROLL_GUTTER: {
                                positions = m_fileCount - (iMaxListSize - 1);
                                if (positions < 1)
                                    positions = 1;
                                stepScreen = static_cast<i32>(
                                    (fGutterTravelLength
                                     * H2EnumIndex(FILE_REQUESTER_GUTTER_SCALE))
                                    / positions
                                );
                                mouseX = message.payload.widget.screenX;
                                screenY = message.payload.widget.screenY;
                                screenY = static_cast<i32>(screenY - (m_y + fGutterMinY));
                                screenY -= FILE_REQUESTER_SCROLL_KNOB_HALF_HEIGHT;
                                topIndexValue =
                                    (screenY * FILE_REQUESTER_GUTTER_SCALE) / stepScreen;
                                m_topIndex = topIndexValue;
                                if (m_topIndex + iMaxListSize - 1 >= m_fileCount)
                                    m_topIndex = m_fileCount - iMaxListSize;
                                if (m_topIndex < 0)
                                    m_topIndex = 0;
                                Update(1);
                                break;
                            }
                            case FILE_REQUESTER_SCROLL_KNOB:
                                DoKnob();
                                break;
                            default: {
                                if (message.payload.widget.id >= FILE_REQUESTER_MAP_SIZE_ICON_FIRST
                                    && message.payload.widget.id
                                           <= FILE_REQUESTER_MAP_SIZE_ICON_FIRST
                                                  + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
                                    iResult = message.payload.widget.id
                                              - FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
                                    goto SelectListItem;
                                }
                                if (message.payload.widget.id
                                        >= FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
                                    && message.payload.widget.id
                                           <= FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
                                                  + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
                                    iResult = message.payload.widget.id
                                              - FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
                                    goto SelectListItem;
                                }
                                if (message.payload.widget.id
                                        >= FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
                                    && message.payload.widget.id
                                           <= FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
                                                  + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
                                    iResult = message.payload.widget.id
                                              - FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
                                    goto SelectListItem;
                                }
                                if (message.payload.widget.id >= FILE_REQUESTER_MAP_LOSS_ICON_FIRST
                                    && message.payload.widget.id
                                           <= FILE_REQUESTER_MAP_LOSS_ICON_FIRST
                                                  + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
                                    iResult = message.payload.widget.id
                                              - FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
                                    goto SelectListItem;
                                }
                                if (message.payload.widget.id >= FILE_REQUESTER_LIST_TEXT_FIRST
                                    && message.payload.widget.id
                                           <= FILE_REQUESTER_LIST_TEXT_FIRST
                                                  + FILE_REQUESTER_LIST_RANGE_SIZE - 1) {
                                    iResult =
                                        message.payload.widget.id - FILE_REQUESTER_LIST_TEXT_FIRST;
                                    goto SelectListItem;
                                }
                                break;
                            SelectListItem:
                                if (iResult + m_topIndex == m_selectedIndex) {
                                    message.payload.widget.data.value = FILE_REQUESTER_OK;
                                    message.payload.widget.id = FILE_REQUESTER_OK;
                                    acceptStep = 1;
                                    break;
                                }
                                if (iResult + m_topIndex >= m_fileCount)
                                    break;
                                m_selectedIndex = iResult + m_topIndex;
                                SetOK(1);
                                Update(1);
                                break;
                            }
                        }
                    }
                    break;
            }
            break;
    }

    if (acceptStep == 1) {
        if (m_mode == FILE_REQUESTER_LOAD_GAME && m_selectedIndex >= 0
            && message.payload.widget.data.value != FILE_REQUESTER_CANCEL
            && strcmpi(m_extensions[m_selectedIndex].text, ".GMC") != 0
            && strcmpi(m_extensions[m_selectedIndex].text, ".GXC") != 0) {
            iResult =
                m_extensions[m_selectedIndex].text[FILE_REQUESTER_EXTENSION_PLAYER_DIGIT] - '0';
            if (iResult < giNumHumanPlayers
                && giDebugLevel < FILE_REQUESTER_DEBUG_ALLOW_PLAYER_MISMATCH) {
                sprintf(
                    gText,
                    "\xc2\xfb\xe1\xf0\xe0\xed\xed\xe0\xff \xe2\xe0\xec\xe8 \xe8\xe3\xf0\xe0 \xf0"
                    "\xe0\xf1\xf1\xf7\xe8\xf2\xe0\xed\xe0 \xf2\xee\xeb\xfc\xea\xee \xed\xe0 %d \xf7"
                    "\xe5\xeb\xee\xe2\xe5\xea.  \xc0 \xe2\xe0\xec \xed\xf3\xe6\xed\xe0 \xea\xe0\xf0"
                    "\xf2\xe0, \xea\xe0\xea \xec\xe8\xed\xe8\xec\xf3\xec \xed\xe0 %d \xf7\xe5\xeb"
                    "\xee\xe2\xe5\xea."


                    ,
                    iResult,
                    giNumHumanPlayers
                );
                NormalDialog(
                    gText,
                    NORMAL_DIALOG_INFO,
                    NORMAL_DIALOG_NO_RESOURCE,
                    NORMAL_DIALOG_NO_VALUE,
                    NORMAL_DIALOG_NO_RESOURCE,
                    0,
                    NORMAL_DIALOG_NO_RESOURCE,
                    0,
                    NORMAL_DIALOG_NO_RESOURCE,
                    0
                );
                acceptStep = 0;
            }
            if (iResult > giNumHumanPlayers) {
                sprintf(
                    gText,
                    "\xc2\xfb\xe1\xf0\xe0\xed\xed\xe0\xff \xe8\xe3\xf0\xe0 \xed\xe0\xf7\xed\xe5\xf2"
                    "\xf1\xff \xf1 %d \xe8\xe3\xf0\xee\xea\xe0\xec\xe8-\xeb\xfe\xe4\xfc\xec\xe8. "
                    "\xcc\xee\xe6\xed\xee \xeb\xe8 \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xf3 \xe2"
                    "\xe7\xff\xf2\xfc \xef\xee\xe4 \xf1\xe2\xee\xe5 \xf3\xef\xf0\xe0\xe2\xeb\xe5"
                    "\xed\xe8\xe5 \xee\xf1\xf2\xe0\xe2\xf8\xe8\xe5\xf1\xff %d \xec\xe5\xf1\xf2 \xeb"
                    "\xfe\xe4\xe5\xe9?"


                    ,
                    iResult,
                    iResult - giNumHumanPlayers
                );
                NormalDialog(
                    gText,
                    NORMAL_DIALOG_CONFIRM,
                    NORMAL_DIALOG_NO_RESOURCE,
                    NORMAL_DIALOG_NO_VALUE,
                    NORMAL_DIALOG_NO_RESOURCE,
                    0,
                    NORMAL_DIALOG_NO_RESOURCE,
                    0,
                    NORMAL_DIALOG_NO_RESOURCE,
                    0
                );
                if (gpWindowManager->m_dialogResult != NORMAL_DIALOG_BUTTON_FIVE) {
                    acceptStep = 0;
                }
            }
        }
        if (acceptStep != 0) {
            message.type = MESSAGE_EXECUTIVE;
            message.payload.executive.command = FILE_REQUESTER_EXECUTIVE_CLOSE;
            return MESSAGE_DISPATCH_FORWARD;
        }
    }
    return MESSAGE_DISPATCH_CONSUME;
}

void fileRequester::DoKnob(void) {
    i32 oldTopIndex;
    double gutterStep7;
    i32 mouseX7;
    i32 knobOffset;
    i32 mouseY7;
    tag_message knobMessage;
    i32 topIndex;

    oldTopIndex = m_topIndex;
    gutterStep7 = fGutterTravelLength / (m_fileCount - (iMaxListSize - 1));
    gpMouseManager->MouseCoords(mouseX7, mouseY7);
    knobOffset = mouseY7 - m_scrollKnob->m_y;
    gpInputManager->Flush();
    knobMessage = gpInputManager->GetEvent();
    while (knobMessage.type != MESSAGE_LEFT_BUTTON_UP
           && knobMessage.type != MESSAGE_RIGHT_BUTTON_UP) {
        if (knobMessage.type == MESSAGE_MOUSE_MOVE) {
            if (static_cast<float>(knobMessage.payload.mouse.y) < knobOffset + fGutterMinY) {
                knobMessage.payload.mouse.y = static_cast<i32>(knobOffset + fGutterMinY);
            }
            if (static_cast<float>(knobMessage.payload.mouse.y)
                > knobOffset + fGutterMinY + fGutterTravelLength) {
                knobMessage.payload.mouse.y =
                    static_cast<i32>(knobOffset + fGutterMinY + fGutterTravelLength);
            }
            gpMouseManager->Main(knobMessage);
            m_scrollKnob->m_y = knobMessage.payload.mouse.y - knobOffset;
            if (m_fileCount > iMaxListSize) {
                topIndex = static_cast<i32>((m_scrollKnob->m_y - fGutterMinY) / gutterStep7);
                if (topIndex != oldTopIndex) {
                    if (topIndex > m_fileCount - iMaxListSize) {
                        topIndex = m_fileCount - iMaxListSize;
                    }
                    if (topIndex < 0) {
                        topIndex = 0;
                    }
                    m_topIndex = topIndex;
                    Update(0);
                    m_scrollKnob->m_y = knobMessage.payload.mouse.y - knobOffset;
                    m_window->DrawWindow(1, 0, WINDOW_DRAW_ID_LIMIT);
                    oldTopIndex = topIndex;
                } else {
                    m_window->DrawWindow(1, 0, WINDOW_DRAW_ID_LIMIT);
                }
            } else {
                m_window->DrawWindow(1, 0, WINDOW_DRAW_ID_LIMIT);
            }
        }
        platform::PumpEvents();
        knobMessage = gpInputManager->GetEvent();
    }
    m_scrollKnob->m_flags &= ~WIDGET_FLAG_SELECTED;
    Update(1);
}

void fileRequester::Update(i32 drawWindow) {
    i32 unusedState;
    double gutterSpan;
    i32 localState;
    tag_message message;
    char localStorage1[FILE_REQUESTER_UPDATE_STORAGE_SIZE];
    i32 i;
    double gutterStepCount1;
    i32 unusedState7;

    message.type = MESSAGE_WIDGET;
    localState = 0;

    if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
        for (i = 0; i < H2EnumIndex(FILE_REQUESTER_MAP_SIZE_COUNT); ++i) {
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.id = FILE_REQUESTER_FILTER_SMALL + i;
            message.payload.widget.data.value =
                (i == H2EnumIndex(giMapSizeFilter)) + i * FILTER_FRAME_STEP + FILTER_FRAME_BASE;
            m_window->BroadcastMessage(message);
        }
        if (m_selectedIndex == FILE_REQUESTER_SELECTION_NONE && m_fileCount > 0) {
            m_selectedIndex = 0;
        }
        SetOK(1);

        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = FILE_REQUESTER_MAP_SIZE;
        if (m_mapHeaders[m_selectedIndex].width == MAP_DIMENSION_SMALL) {
            message.payload.widget.data.value = H2EnumIndex(MAP_SIZE_FRAME_SMALL);
        } else if (m_mapHeaders[m_selectedIndex].width == MAP_DIMENSION_MEDIUM) {
            message.payload.widget.data.value = H2EnumIndex(MAP_SIZE_FRAME_MEDIUM);
        } else if (m_mapHeaders[m_selectedIndex].width == MAP_DIMENSION_LARGE) {
            message.payload.widget.data.value = H2EnumIndex(MAP_SIZE_FRAME_LARGE);
        } else {
            message.payload.widget.data.value = H2EnumIndex(MAP_SIZE_FRAME_XLARGE);
        }
        m_window->BroadcastMessage(message);

        message.payload.widget.id = FILE_REQUESTER_MAP_PLAYER_COUNT;
        message.payload.widget.data.value =
            m_mapHeaders[m_selectedIndex].playerCount + H2EnumIndex(PLAYER_COUNT_FRAME_BASE);
        m_window->BroadcastMessage(message);

        message.payload.widget.id = FILE_REQUESTER_MAP_VICTORY;
        message.payload.widget.data.value =
            H2EnumIndex(m_mapHeaders[m_selectedIndex].victoryCondition) + H2EnumIndex(VICTORY_FRAME_BASE);
        m_window->BroadcastMessage(message);

        message.payload.widget.id = FILE_REQUESTER_MAP_LOSS;
        message.payload.widget.data.value =
            H2EnumIndex(m_mapHeaders[m_selectedIndex].lossCondition) + H2EnumIndex(LOSS_FRAME_BASE);
        m_window->BroadcastMessage(message);

        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message.payload.widget.data.text = gText;
        sprintf(gText, "%s", m_mapHeaders[m_selectedIndex].name);
        message.payload.widget.id = FILE_REQUESTER_MAP_NAME;
        m_window->BroadcastMessage(message);

        sprintf(gText, "%s", cDifficulty[H2EnumIndex(m_mapHeaders[m_selectedIndex].difficulty)]);
        message.payload.widget.id = FILE_REQUESTER_MAP_DIFFICULTY_TEXT;
        m_window->BroadcastMessage(message);

        sprintf(gText, "%s", m_mapHeaders[m_selectedIndex].description);
        message.payload.widget.id = FILE_REQUESTER_MAP_DESCRIPTION;
        m_window->BroadcastMessage(message);
    }

    for (i = 0; i < iMaxListSize; ++i) {
        if (m_topIndex + i >= m_fileCount) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            message.payload.widget.id = i + FILE_REQUESTER_LIST_TEXT_FIRST;
            m_window->BroadcastMessage(message);
            if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
                message.payload.widget.id = i + FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
                m_window->BroadcastMessage(message);
                message.payload.widget.id = i + FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
                m_window->BroadcastMessage(message);
                message.payload.widget.id = i + FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
                m_window->BroadcastMessage(message);
                message.payload.widget.id = i + FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
                m_window->BroadcastMessage(message);
            }
        } else {
            message.payload.widget.id = i + FILE_REQUESTER_LIST_TEXT_FIRST;
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);

            if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
                message.payload.widget.id = i + FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
                m_window->BroadcastMessage(message);
                message.payload.widget.id = i + FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
                m_window->BroadcastMessage(message);
                message.payload.widget.id = i + FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
                m_window->BroadcastMessage(message);
                message.payload.widget.id = i + FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
                m_window->BroadcastMessage(message);

                message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                message.payload.widget.id = i + FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
                if (m_mapHeaders[m_topIndex + i].width == MAP_DIMENSION_SMALL) {
                    message.payload.widget.data.value = H2EnumIndex(MAP_SIZE_FRAME_SMALL);
                } else if (m_mapHeaders[m_topIndex + i].width == MAP_DIMENSION_MEDIUM) {
                    message.payload.widget.data.value = H2EnumIndex(MAP_SIZE_FRAME_MEDIUM);
                } else if (m_mapHeaders[m_topIndex + i].width == MAP_DIMENSION_LARGE) {
                    message.payload.widget.data.value = H2EnumIndex(MAP_SIZE_FRAME_LARGE);
                } else {
                    message.payload.widget.data.value = H2EnumIndex(MAP_SIZE_FRAME_XLARGE);
                }
                m_window->BroadcastMessage(message);

                message.payload.widget.id = i + FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
                message.payload.widget.data.value =
                    m_mapHeaders[m_topIndex + i].playerCount + H2EnumIndex(PLAYER_COUNT_FRAME_BASE);
                m_window->BroadcastMessage(message);

                message.payload.widget.id = i + FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
                message.payload.widget.data.value =
                    H2EnumIndex(m_mapHeaders[m_topIndex + i].victoryCondition) + H2EnumIndex(VICTORY_FRAME_BASE);
                m_window->BroadcastMessage(message);

                message.payload.widget.id = i + FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
                message.payload.widget.data.value =
                    H2EnumIndex(m_mapHeaders[m_topIndex + i].lossCondition) + H2EnumIndex(LOSS_FRAME_BASE);
                m_window->BroadcastMessage(message);
            }

            message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
                sprintf(gText, "%s", m_mapHeaders[m_topIndex + i].name);
            } else {
                sprintf(gText, "%s", m_fileNames[m_topIndex + i].text);
            }
            message.payload.widget.data.text = gText;
            message.payload.widget.id = i + FILE_REQUESTER_LIST_TEXT_FIRST;
            m_window->BroadcastMessage(message);
        }

        message.payload.widget.id = i + FILE_REQUESTER_LIST_TEXT_FIRST;
        message.payload.widget.command = WIDGET_COMMAND_SET_FILL_COLOR;
        if (m_selectedIndex == m_topIndex + i) {
            message.payload.widget.data.value = SELECTED_FILL_COLOR;
        } else {
            message.payload.widget.data.value = 1;
        }
        m_window->BroadcastMessage(message);
    }

    message.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
    message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
    message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
    m_window->BroadcastMessage(message);
    if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
            sprintf(gText, "%s", m_mapHeaders[m_selectedIndex].name);
        } else {
            sprintf(gText, "%s", m_fileNames[m_selectedIndex].text);
        }
        message.payload.widget.data.text = gText;
        m_window->BroadcastMessage(message);
    }
    if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_LOAD_GAME
        || m_mode == FILE_REQUESTER_MAP) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
        m_window->BroadcastMessage(message);
    }

    if (m_fileCount <= iMaxListSize) {
        m_scrollKnob->m_y =
            static_cast<i16>(
                fGutterTravelLength
                    / H2EnumIndex(SCROLL_CENTER_DIVISOR)
                + fGutterMinY
            );
    } else {
        gutterStepCount1 = fGutterTravelLength / (m_fileCount - iMaxListSize);
        m_scrollKnob->m_y = static_cast<i16>(fGutterMinY + m_topIndex * gutterStepCount1);
    }
    if (drawWindow) {
        m_window->DrawWindow(1, 0, WINDOW_DRAW_ID_LIMIT);
    }
}

char* fileRequester::GetFilename(void) {
    if (m_mode != FILE_REQUESTER_SAVE_GAME
        && (m_selectedIndex < 0 || m_selectedIndex >= m_fileCount)) {
        return cFRDummy;
    }

    if (m_selectedIndex == FILE_REQUESTER_SELECTION_NONE) {
        sprintf(gText, "%s%s", m_filename, m_defaultExtension);
    } else if (m_mode == FILE_REQUESTER_LOAD_GAME || m_mode == FILE_REQUESTER_MAP
               || m_mode == FILE_REQUESTER_MAP_GAME) {
        sprintf(
            gText,
            "%s%s",
            m_fileNames[m_selectedIndex].text,
            m_extensions[m_selectedIndex].text
        );
    } else {
        sprintf(gText, "%s%s", m_fileNames[m_selectedIndex].text, m_defaultExtension);
    }
    strcpy(m_filename, gText);
    return m_filename;
}


FileRequesterMapSizeFilter giMapSizeFilter = FILE_REQUESTER_MAP_SIZE_ALL;
struct cFRDummyStorageOwner {
    static char storage[1];
};
char* cFRDummy = cFRDummyStorageOwner::storage;
float fGutterMinY;
float fGutterTravelLength;
i32 iMaxListSize;
