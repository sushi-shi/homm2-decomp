// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\REQUEST.OBJ   from: (directly linked into exe)
// functions: 16   data: 6
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <BASE/Misc.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/fileRequester.h>
#include <SOURCE/game.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/REQUEST.h>

// Frames of the requester's map-info icons: the map-size frames are selected
// directly; player-count, victory, and loss frames are base + header value.
typedef enum RequesterIconFrame {
    PLAYER_COUNT_FRAME_BASE = 0x13,
    MAP_SIZE_FRAME_SMALL = 0x1a,
    MAP_SIZE_FRAME_MEDIUM = 0x1b,
    MAP_SIZE_FRAME_LARGE = 0x1c,
    MAP_SIZE_FRAME_XLARGE = 0x1d,
    VICTORY_FRAME_BASE = 0x1e,
    LOSS_FRAME_BASE = 0x24
} RequesterIconFrame;

// @early-stop
// @early-stop-reloc-only
// All 0x80 relocation-masked bytes and eight ordered relocation sites/effective
// targets agree; residuals are CRT/compiler-local symbol spellings only.
VA(0x0048c920, 0x80)
i32 GetMapHeader(char* filename, struct SMapHeader* header) {
    sprintf(gText, "%s%s", gcMapPath, filename);
    i32 file = _open(gText, _O_BINARY);
    if (file == -1) {
        return 0;
    }
    _read(file, header, sizeof(*header));
    _close(file);
    return 1;
}

VA(0x0048c9a0, 0x1b)
i32 CheckSumIsDemoOK(char*) {
    return 1;
}

// @early-stop
// @early-stop-reloc-only
// All relocation-masked bytes in the retail extent, the 0x134 frame/slots,
// CFG, and four ordered effective targets agree. Only the pooled "BROKENA"
// and _strcmpi compiler/import aliases differ.
VA(0x0048c9bb, 0xbb)
i32 ShowThisMapGame(char* filename) {
    return 1;

    char mapName[300];
    i32 ix;
    strcpy(mapName, filename);
    mapName[8] = 0;
    for (ix = 0; ix < 8; ++ix) {
        if (mapName[ix] == '.') {
            mapName[ix] = 0;
        }
    }
    if (_strcmpi(mapName, "BROKENA") == 0 && CheckSumIsDemoOK(filename)) {
        return 1;
    }
    return 0;
}

VA(0x0048ca76, 0x1b)
i32 ShowThisMap(char*) {
    return 1;
}

// @semantic
// Both enumeration passes, allocation, sorted insertion, extensions, and
// map-header population are complete; the 0x65c frame, aggregate/local slots,
// CFG, and all 53 ordered effective relocation targets agree. The first
// executable residual is the map-size comparison: base emits a local jne,
// while retail emits je followed by two continuation jumps. Negative and
// combined-positive predicates were tried; splitting nested positive arms
// lowered the match materially. Revisit if REQUEST TU state or inline
// continuation normalization changes.
VA(0x0048ca91, 0x7c1)
i32 fileRequester::InitializeFiles(char* directory, char* pattern, i32 countOnly) {
    char fullPath[FILE_REQUESTER_PATH_SIZE];
    SMapHeader mapHeader;
    WIN32_FIND_DATA findDataPath;
    char extensionEntry[FILE_REQUESTER_LOCAL_EXTENSION_SIZE];
    char fileName[FILE_REQUESTER_LOCAL_NAME_SIZE];
    HANDLE findHandleWork;
    i32 moreFilesHandle;
    i32 insertedCountResult;
    char* dotSource;
    i32 moveValue;
    i32 indexData;

    sprintf(gText, "%s%s", directory, pattern);
    m_fileCount = 0;
    moreFilesHandle = 1;
    findHandleWork = FindFirstFile(gText, &findDataPath);
    if (findHandleWork != INVALID_HANDLE_VALUE) {
        while (moreFilesHandle) {
            if (m_mode == FILE_REQUESTER_MAP_GAME) {
                GetMapHeader(findDataPath.cFileName, &mapHeader);
                if (mapHeader.minHumanPlayers <= giNumHumanPlayers
                    && giNumHumanPlayers <= mapHeader.maxHumanPlayers
                    && (giMapSizeFilter == FILE_REQUESTER_MAP_SIZE_ALL
                        || giMapSizes[giMapSizeFilter] == mapHeader.width)
                    && ShowThisMapGame(findDataPath.cFileName)) {
                } else {
                    goto CountNextFile;
                }
            }
            if (m_mode == FILE_REQUESTER_MAP) {
                GetMapHeader(findDataPath.cFileName, &mapHeader);
                if ((giMapSizeFilter == FILE_REQUESTER_MAP_SIZE_ALL
                     || giMapSizes[giMapSizeFilter] == mapHeader.width)
                    && ShowThisMap(findDataPath.cFileName)) {
                } else {
                    goto CountNextFile;
                }
            }
            ++m_fileCount;
        CountNextFile:
            moreFilesHandle = FindNextFile(findHandleWork, &findDataPath);
        }
        FindClose(findHandleWork);
    }

    if (countOnly) {
        return m_fileCount;
    }

    m_fileNames = new FileRequesterName[m_fileCount + 1];
    if (m_fileNames == 0) {
        MemError();
    }
    m_extensions = new FileRequesterExtension[m_fileCount + 1];
    if (m_extensions == 0) {
        MemError();
    }
    if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
        m_mapHeaders = new SMapHeader[m_fileCount];
        if (m_mapHeaders == 0) {
            MemError();
        }
    }

    for (indexData = 0; indexData < m_fileCount; ++indexData) {
        strcpy(m_fileNames[indexData].text, "");
        strcpy(m_extensions[indexData].text, "");
    }

    insertedCountResult = 0;
    sprintf(gText, "%s%s", directory, pattern);
    findHandleWork = FindFirstFile(gText, &findDataPath);
    if (findHandleWork != INVALID_HANDLE_VALUE) {
        moreFilesHandle = 1;
        while (moreFilesHandle) {
            if (m_mode == FILE_REQUESTER_MAP_GAME) {
                GetMapHeader(findDataPath.cFileName, &mapHeader);
                if (mapHeader.minHumanPlayers <= giNumHumanPlayers
                    && giNumHumanPlayers <= mapHeader.maxHumanPlayers
                    && (giMapSizeFilter == FILE_REQUESTER_MAP_SIZE_ALL
                        || giMapSizes[giMapSizeFilter] == mapHeader.width)
                    && ShowThisMapGame(findDataPath.cFileName)) {
                } else {
                    goto InsertNextFile;
                }
            }
            if (m_mode == FILE_REQUESTER_MAP) {
                GetMapHeader(findDataPath.cFileName, &mapHeader);
                if ((giMapSizeFilter == FILE_REQUESTER_MAP_SIZE_ALL
                     || giMapSizes[giMapSizeFilter] == mapHeader.width)
                    && ShowThisMap(findDataPath.cFileName)) {
                } else {
                    goto InsertNextFile;
                }
            }

            strcpy(fileName, findDataPath.cFileName);
            dotSource = FindLastToken(fileName, '.');
            if (dotSource != 0) {
                strcpy(extensionEntry, dotSource);
                *dotSource = 0;
            }

            for (indexData = 0; insertedCountResult > indexData; ++indexData) {
                if (_strcmpi(fileName, m_fileNames[indexData].text) < 0) {
                    for (moveValue = insertedCountResult; moveValue > indexData; --moveValue) {
                        strcpy(m_fileNames[moveValue].text, m_fileNames[moveValue - 1].text);
                        strcpy(m_extensions[moveValue].text, m_extensions[moveValue - 1].text);
                    }
                    break;
                }
            }
            strcpy(m_fileNames[indexData].text, fileName);
            strcpy(m_extensions[indexData].text, extensionEntry);
            ++insertedCountResult;
        InsertNextFile:
            moreFilesHandle = FindNextFile(findHandleWork, &findDataPath);
        }
        FindClose(findHandleWork);
    }

    if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
        for (indexData = 0; insertedCountResult > indexData; ++indexData) {
            sprintf(fullPath, "%s%s", m_fileNames[indexData].text, m_extensions[indexData].text);
            GetMapHeader(fullPath, &m_mapHeaders[indexData]);
        }
    }
    return m_fileCount;
}

VA(0x0048d252, 0x16e)
fileRequester::fileRequester(
    i32 x,
    i32 y,
    i32 mode,
    char* pattern,
    char* directory,
    char* defaultExtension
) {
    strcpy(m_filePattern, pattern);
    strcpy(m_directory, directory);
    m_selectedIndex = FILE_REQUESTER_SELECTION_NONE;
    m_fileCount = 0;
    m_topIndex = 0;
    m_fileNames = 0;
    m_extensions = 0;
    m_mapHeaders = 0;
    m_x = x;
    m_y = y;
    m_mode = static_cast<FileRequesterMode>(mode);
    strcpy(m_defaultExtension, defaultExtension);
    if (mode == FILE_REQUESTER_MAP_GAME || mode == FILE_REQUESTER_MAP) {
        fGutterTravelLength = 123.0f;
        fGutterMinY = 73.0f;
        iMaxListSize = 9;
    } else {
        fGutterTravelLength = 163.0f;
        fGutterMinY = 73.0f;
        iMaxListSize = 11;
    }
    if (!MapExistsForFilter(giMapSizeFilter)) {
        giMapSizeFilter = FILE_REQUESTER_MAP_SIZE_ALL;
    }
    InitializeFiles(m_directory, m_filePattern, 0);
    m_result = -2;
}

VA(0x0048d3c0, 0x63)
i32 fileRequester::MapExistsForFilter(FileRequesterMapSizeFilter filter) {
    FileRequesterMapSizeFilter oldFilter = giMapSizeFilter;
    giMapSizeFilter = filter;
    i32 result = InitializeFiles(m_directory, m_filePattern, 1);
    giMapSizeFilter = oldFilter;
    return result > 0;
}

VA(0x0048d423, 0x6c)
void fileRequester::SetupFiles(void) {
    CleanUpData();
    m_fileCount = 0;
    m_topIndex = 0;
    m_result = -2;
    m_selectedIndex = FILE_REQUESTER_SELECTION_NONE;
    InitializeFiles(m_directory, m_filePattern, 0);
}

VA(0x0048d48f, 0xc7)
void fileRequester::CleanUpData(void) {
    if (m_fileNames != 0) {
        delete[] m_fileNames;
    }
    m_fileNames = 0;
    if (m_extensions != 0) {
        delete[] m_extensions;
    }
    m_extensions = 0;
    if (m_mapHeaders != 0) {
        delete[] m_mapHeaders;
    }
    m_mapHeaders = 0;
}

VA(0x0048d556, 0x8b)
void fileRequester::Close(void) {
    if (!m_active) {
        return;
    }
    KBChangeMenu(m_previousMenu);
    strcpy(gLastFilename, GetFilename());
    CleanUpData();
    gpWindowManager->RemoveWindow(m_window);
    delete m_window;
    m_active = 0;
}

// @early-stop
// @early-stop-reloc-only
// All relocation-masked bytes, the 0x44 frame/slots, CFG, and 47 ordered
// effective targets agree. Only pooled-string and _strcmpi import aliases
// differ.
VA(0x0048d5e1, 0x466)
i32 fileRequester::Open(i32 id) {
    strcpy(gLastFilename, "");
    m_previousMenu = hmnuCurrent;
    KBChangeMenu(hmnuDflt);

    m_window = new heroWindow(
        m_x,
        m_y,
        m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP ? "requests.bin"
                                                                          : "request.bin"
    );
    if (m_window == 0) {
        MemError();
    }

    m_scrollKnob =
        new iconWidget(346, static_cast<i16>(fGutterMinY), 8, 17, "scrollcn.icn", 4, 0, 14, 16, 1);
    if (m_scrollKnob == 0) {
        MemError();
    }
    m_window->AddWidget(m_scrollKnob, -1);

    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    u8 okEnabled3;
    i32 fileIndex;
    if (m_mode == FILE_REQUESTER_SAVE_GAME) {
        okEnabled3 = 1;
        strcpy(m_filename, gpGame->m_saveName);
        char* dot = FindLastToken(m_filename, '.');
        if (dot != 0) {
            *dot = 0;
        }
        message.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
        message.payload.widget.data.text = m_filename;
        m_window->BroadcastMessage(message);
        message.payload.widget.id = FILE_REQUESTER_FILENAME_LABEL;
        sprintf(gText, "File to Save:");
        message.payload.widget.data.text = gText;
        m_window->BroadcastMessage(message);
        for (fileIndex = 0; fileIndex < m_fileCount; ++fileIndex) {
            if (_strcmpi(m_fileNames[fileIndex].text, m_filename) == 0) {
                m_selectedIndex = fileIndex;
            }
        }
    } else {
        okEnabled3 = 0;
        if (m_mode == FILE_REQUESTER_MAP_GAME) {
            char mapName[12];
            fileIndex = 0;
            memset(mapName, 0, 9);
            while (fileIndex < 8 && gMapName[fileIndex] != 0 && gMapName[fileIndex] != '.') {
                mapName[fileIndex] = gMapName[fileIndex];
                ++fileIndex;
            }
            for (fileIndex = 0; fileIndex < m_fileCount; ++fileIndex) {
                if (_strcmpi(m_fileNames[fileIndex].text, mapName) == 0) {
                    m_selectedIndex = fileIndex;
                    okEnabled3 = 1;
                }
            }
        }
        message.payload.widget.id = FILE_REQUESTER_FILENAME_LABEL;
        sprintf(gText, "File to Load:");
        message.payload.widget.data.text = gText;
        m_window->BroadcastMessage(message);
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_MAX_LENGTH;
    message.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
    message.payload.widget.data.value = 201;
    m_window->BroadcastMessage(message);
    Update(0);
    if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
        okEnabled3 = 1;
    }
    gpWindowManager->AddWindow(m_window, -1, 1);
    if (m_fileCount == 0) {
        okEnabled3 = 0;
    }
    if (m_mode == FILE_REQUESTER_SAVE_GAME && _strcmpi(m_filename, "NEWGAME") == 0
        && m_selectedIndex == FILE_REQUESTER_SELECTION_NONE) {
        okEnabled3 = 1;
    }
    SetOK(okEnabled3);
    m_messageMask = 0x4000;
    m_priority = id;
    m_active = 1;
    strcpy(m_name, "fileRequester");
    return 0;
}

VA(0x0048da47, 0xa5)
void fileRequester::SetOK(i32 enabled) {
    tag_message message;
    message.type = MESSAGE_WIDGET;
    if (enabled) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    } else {
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
    }
    message.payload.widget.id = FILE_REQUESTER_OK;
    if (m_active == 1) {
        message.payload.widget.data.value = 8;
    } else {
        message.payload.widget.data.value = 0x1000;
    }
    m_window->BroadcastMessage(message);
    if (enabled) {
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
    } else {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    }
    message.payload.widget.data.value = 2;
    m_window->BroadcastMessage(message);
}

// @semantic
// Key, click, hover/help, filters, filename editing, knob/list selection,
// saved-game validation, and executive exit are complete. The primary retail
// slots and all 76 jump-table-excluded external relocations agree. Base's 0x41c
// frame lacks one retail compiler-generated FP temporary below this; adding a
// source local shifts real slots and is invalid. The first executable residual
// is the key-up selected/top comparison polarity. Empty positive <= and >=
// arms and moving click behind hover/help were rejected by lower scores.
// Revisit if switch/FP lowering or REQUEST TU state changes.
VA(0x0048daec, 0x11ae)
i32 fileRequester::Main(struct tag_message& message) {
    i32 acceptStep = 0;
    i32 iResult;
    i32 helpIndexMouse;
    i32 lengthIndex;
    i32 positions;
    i32 gutterStepScreen;
    i32 mouseXIndex;
    i32 screenY;
    i32 newTopIndexBuffer;
    char newNameData[FILE_REQUESTER_LOCAL_NAME_SIZE];
    char cycleNameBuffer[FILE_REQUESTER_PATH_SIZE];
    char filteredNameMap[FILE_REQUESTER_PATH_SIZE];
    tag_message broadcastMessage;

    switch (message.type) {
        case MESSAGE_KEY_DOWN:
            switch (message.payload.keyboard.keyCode) {
                case FILE_REQUESTER_KEY_CYCLE_FILTER: {
                    if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
                        strcpy(cycleNameBuffer, m_fileNames[m_selectedIndex].text);
                    } else {
                        strcpy(cycleNameBuffer, "");
                    }
                    giMapSizeFilter = static_cast<FileRequesterMapSizeFilter>(
                        (giMapSizeFilter + 1) % FILE_REQUESTER_MAP_SIZE_COUNT
                    );
                    SetupFiles();
                    if (strlen(cycleNameBuffer) != 0) {
                        for (iResult = 0; iResult < m_fileCount; ++iResult) {
                            if (_strcmpi(m_fileNames[iResult].text, cycleNameBuffer) == 0) {
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
                        if (m_selectedIndex < m_topIndex) {
                            --m_topIndex;
                        }
                        Update(1);
                    }
                    break;
                case INPUT_SCAN_NUMPAD_2:
                    if (m_fileCount - 1 > m_selectedIndex) {
                        ++m_selectedIndex;
                        if (m_selectedIndex >= m_topIndex + iMaxListSize) {
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
                    if (message.payload.widget.id < FILE_REQUESTER_OK) {
                        if (message.payload.widget.id == FILE_REQUESTER_CANCEL) {
                            message.payload.widget.data.value = message.payload.widget.id;
                            acceptStep = 1;
                        } else if (message.payload.widget.id == FILE_REQUESTER_SCROLL_UP) {
                            if (m_topIndex > 0) {
                                --m_topIndex;
                                Update(1);
                            }
                        } else if (message.payload.widget.id == FILE_REQUESTER_SCROLL_DOWN
                                   && m_topIndex + iMaxListSize < m_fileCount) {
                            ++m_topIndex;
                            if (m_topIndex + iMaxListSize - 1 >= m_fileCount) {
                                m_topIndex = m_fileCount - iMaxListSize;
                            }
                            Update(1);
                        }
                    } else if (message.payload.widget.id == FILE_REQUESTER_OK) {
                        if (m_selectedIndex == FILE_REQUESTER_SELECTION_NONE
                            && m_filename[0] == 0) {
                            NormalDialog(
                                "Please make a selection from the list, or press cancel.",
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
                        } else {
                            message.payload.widget.data.value = message.payload.widget.id;
                            acceptStep = 1;
                        }
                    }
                    break;
                case WIDGET_COMMAND_SELECT:
                case WIDGET_COMMAND_ALTERNATE_SELECT:
                    if ((message.payload.widget.parameter & FILE_REQUESTER_HELP_MODIFIER) != 0) {
                        helpIndexMouse = FILE_REQUESTER_HELP_NONE;
                        switch (message.payload.widget.id) {
                            case FILE_REQUESTER_FILENAME_ENTRY:
                                helpIndexMouse = FILE_REQUESTER_HELP_FILENAME;
                                break;
                            case FILE_REQUESTER_MAP_NAME:
                                helpIndexMouse = FILE_REQUESTER_HELP_MAP_NAME;
                                break;
                            case FILE_REQUESTER_MAP_PLAYER_COUNT:
                                helpIndexMouse = FILE_REQUESTER_HELP_PLAYER_COUNT;
                                break;
                            case FILE_REQUESTER_MAP_SIZE:
                                helpIndexMouse = FILE_REQUESTER_HELP_MAP_SIZE;
                                break;
                            case FILE_REQUESTER_MAP_DIFFICULTY_ICON:
                            case FILE_REQUESTER_MAP_DIFFICULTY_TEXT:
                                helpIndexMouse = FILE_REQUESTER_HELP_DIFFICULTY;
                                break;
                            case FILE_REQUESTER_MAP_DESCRIPTION:
                                helpIndexMouse = FILE_REQUESTER_HELP_DESCRIPTION;
                                break;
                            case FILE_REQUESTER_MAP_VICTORY:
                                helpIndexMouse = FILE_REQUESTER_HELP_VICTORY;
                                break;
                            case FILE_REQUESTER_MAP_LOSS:
                                helpIndexMouse = FILE_REQUESTER_HELP_LOSS;
                                break;
                            case FILE_REQUESTER_FILTER_SMALL:
                                helpIndexMouse = FILE_REQUESTER_HELP_FILTER_SMALL;
                                break;
                            case FILE_REQUESTER_FILTER_MEDIUM:
                                helpIndexMouse = FILE_REQUESTER_HELP_FILTER_MEDIUM;
                                break;
                            case FILE_REQUESTER_FILTER_LARGE:
                                helpIndexMouse = FILE_REQUESTER_HELP_FILTER_LARGE;
                                break;
                            case FILE_REQUESTER_FILTER_XLARGE:
                                helpIndexMouse = FILE_REQUESTER_HELP_FILTER_XLARGE;
                                break;
                            case FILE_REQUESTER_FILTER_ALL:
                                helpIndexMouse = FILE_REQUESTER_HELP_FILTER_ALL;
                                break;
                            case FILE_REQUESTER_CANCEL:
                                helpIndexMouse = FILE_REQUESTER_HELP_CANCEL;
                                break;
                            case FILE_REQUESTER_OK:
                                helpIndexMouse = FILE_REQUESTER_HELP_OK;
                                break;
                            default:
                                if (message.payload.widget.id >= FILE_REQUESTER_MAP_SIZE_ICON_FIRST
                                    && message.payload.widget.id
                                           < FILE_REQUESTER_MAP_SIZE_ICON_FIRST
                                                 + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    helpIndexMouse = FILE_REQUESTER_HELP_MAP_SIZE;
                                } else if (message.payload.widget.id
                                               >= FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
                                           && message.payload.widget.id
                                                  < FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
                                                        + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    helpIndexMouse = FILE_REQUESTER_HELP_PLAYER_COUNT;
                                } else if (message.payload.widget.id
                                               >= FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
                                           && message.payload.widget.id
                                                  < FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
                                                        + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    helpIndexMouse = FILE_REQUESTER_HELP_VICTORY;
                                } else if (message.payload.widget.id
                                               >= FILE_REQUESTER_MAP_LOSS_ICON_FIRST
                                           && message.payload.widget.id
                                                  < FILE_REQUESTER_MAP_LOSS_ICON_FIRST
                                                        + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    helpIndexMouse = FILE_REQUESTER_HELP_LOSS;
                                }
                                break;
                        }
                        if (helpIndexMouse >= 0) {
                            NormalDialog(
                                gFileRequestHelp[helpIndexMouse],
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
                                            "No maps exist for %d human player at that size.",
                                            giNumHumanPlayers
                                        );
                                    } else {
                                        sprintf(
                                            gText,
                                            "No maps exist for %d human players at that size.",
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
                                } else {
                                    giMapSizeFilter =
                                        static_cast<FileRequesterMapSizeFilter>(iResult);
                                    if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
                                        strcpy(filteredNameMap, m_fileNames[m_selectedIndex].text);
                                    } else {
                                        strcpy(filteredNameMap, "");
                                    }
                                    SetupFiles();
                                    if (strlen(filteredNameMap) != 0) {
                                        for (iResult = 0; iResult < m_fileCount; ++iResult) {
                                            if (_strcmpi(m_fileNames[iResult].text, filteredNameMap)
                                                == 0) {
                                                m_selectedIndex = iResult;
                                            }
                                        }
                                    }
                                    Update(1);
                                }
                                break;
                            }
                            case FILE_REQUESTER_FILENAME_ENTRY: {
                                broadcastMessage.type = MESSAGE_WIDGET;
                                broadcastMessage.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                                broadcastMessage.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
                                m_window->BroadcastMessage(broadcastMessage);

                                memset(newNameData, 0, FILE_REQUESTER_FILENAME_INITIAL_CLEAR_SIZE);
                                strcpy(newNameData, broadcastMessage.payload.widget.data.text);
                                lengthIndex = strlen(newNameData);
                                for (iResult = 0; iResult < lengthIndex; ++iResult) {
                                    if (!((newNameData[iResult] >= 'A'
                                           && newNameData[iResult] <= 'Z')
                                          || (newNameData[iResult] >= 'a'
                                              && newNameData[iResult] <= 'z')
                                          || (newNameData[iResult] >= '0'
                                              && newNameData[iResult] <= '9')
                                          || newNameData[iResult] == '_'
                                          || newNameData[iResult] == ' '
                                          || FindToken(
                                                 "$%'-_@~`!(){}^#&+,;=[].",
                                                 newNameData[iResult]
                                             ) != 0)) {
                                        newNameData[iResult] = 0;
                                    }
                                }
                                for (iResult = strlen(newNameData) - 1; iResult >= 0; --iResult) {
                                    if (newNameData[iResult] == ' ')
                                        newNameData[iResult] = 0;
                                    else
                                        iResult = -1;
                                }
                                if (strlen(newNameData) != 0 && newNameData[0] > ' ') {
                                    m_selectedIndex = FILE_REQUESTER_SELECTION_NONE;
                                    strcpy(m_filename, newNameData);
                                    SetOK(1);
                                }
                                broadcastMessage.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                                broadcastMessage.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
                                broadcastMessage.payload.widget.data.text = m_filename;
                                m_window->BroadcastMessage(broadcastMessage);
                                Update(1);
                                break;
                            }
                            case FILE_REQUESTER_SCROLL_KNOB: {
                                positions = m_fileCount - (iMaxListSize - 1);
                                if (positions < 1)
                                    positions = 1;
                                gutterStepScreen = static_cast<i32>(
                                    (fGutterTravelLength * FILE_REQUESTER_GUTTER_SCALE) / positions
                                );
                                mouseXIndex = message.payload.widget.screenX;
                                screenY = message.payload.widget.screenY;
                                screenY = static_cast<i32>(screenY - (m_y + fGutterMinY))
                                          - FILE_REQUESTER_SCROLL_KNOB_HALF_HEIGHT;
                                newTopIndexBuffer =
                                    (screenY * FILE_REQUESTER_GUTTER_SCALE) / gutterStepScreen;
                                m_topIndex = newTopIndexBuffer;
                                if (m_topIndex + iMaxListSize - 1 >= m_fileCount)
                                    m_topIndex = m_fileCount - iMaxListSize;
                                if (m_topIndex < 0)
                                    m_topIndex = 0;
                                Update(1);
                                break;
                            }
                            case FILE_REQUESTER_SCROLL_GUTTER:
                                DoKnob();
                                break;
                            default: {
                                if (message.payload.widget.id >= FILE_REQUESTER_MAP_SIZE_ICON_FIRST
                                    && message.payload.widget.id
                                           < FILE_REQUESTER_MAP_SIZE_ICON_FIRST
                                                 + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    iResult = message.payload.widget.id
                                              - FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
                                } else if (message.payload.widget.id
                                               >= FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
                                           && message.payload.widget.id
                                                  < FILE_REQUESTER_MAP_PLAYER_ICON_FIRST
                                                        + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    iResult = message.payload.widget.id
                                              - FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
                                } else if (message.payload.widget.id
                                               >= FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
                                           && message.payload.widget.id
                                                  < FILE_REQUESTER_MAP_VICTORY_ICON_FIRST
                                                        + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    iResult = message.payload.widget.id
                                              - FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
                                } else if (message.payload.widget.id
                                               >= FILE_REQUESTER_MAP_LOSS_ICON_FIRST
                                           && message.payload.widget.id
                                                  < FILE_REQUESTER_MAP_LOSS_ICON_FIRST
                                                        + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    iResult = message.payload.widget.id
                                              - FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
                                } else if (message.payload.widget.id
                                               >= FILE_REQUESTER_LIST_TEXT_FIRST
                                           && message.payload.widget.id
                                                  < FILE_REQUESTER_LIST_TEXT_FIRST
                                                        + FILE_REQUESTER_LIST_RANGE_SIZE) {
                                    iResult =
                                        message.payload.widget.id - FILE_REQUESTER_LIST_TEXT_FIRST;
                                } else {
                                    break;
                                }
                                if (iResult + m_topIndex == m_selectedIndex) {
                                    message.payload.widget.data.value = FILE_REQUESTER_OK;
                                    message.payload.widget.id = FILE_REQUESTER_OK;
                                    acceptStep = 1;
                                } else if (iResult + m_topIndex < m_fileCount) {
                                    m_selectedIndex = iResult + m_topIndex;
                                    SetOK(1);
                                    Update(1);
                                }
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
            && _strcmpi(m_extensions[m_selectedIndex].text, ".GMC") != 0
            && _strcmpi(m_extensions[m_selectedIndex].text, ".GXC") != 0) {
            iResult =
                m_extensions[m_selectedIndex].text[FILE_REQUESTER_EXTENSION_PLAYER_DIGIT] - '0';
            if (iResult < giNumHumanPlayers
                && giDebugLevel < FILE_REQUESTER_DEBUG_ALLOW_PLAYER_MISMATCH) {
                sprintf(
                    gText,
                    "The game you have chosen only has slots for %d human(s).  You need one with "
                    "room for at least %d humans.",
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
                    "The game you have chosen was being played with %d humans. Is it OK if the "
                    "computer takes the place of the last %d human(s)?",
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
            return FILE_REQUESTER_MAIN_CLOSE;
        }
    }
    return FILE_REQUESTER_MAIN_CONTINUE;
}

// @early-stop
// @early-stop-reloc-only
// All 228 relocation-masked instructions, the 0x94 frame/slots, CFG, and 36
// ordered effective targets agree. Only the FP-adjust compiler alias differs.
VA(0x0048ec9a, 0x2e8)
void fileRequester::DoKnob(void) {
    i32 oldTopIndex = m_topIndex;
    double gutterStep9 = fGutterTravelLength / (m_fileCount - (iMaxListSize - 1));
    i32 mouseX7;
    i32 mouseY5;
    i32 newTopIndex;
    gpMouseManager->MouseCoords(mouseX7, mouseY5);
    i32 mouseOffset2 = mouseY5 - m_scrollKnob->m_y;

    gpInputManager->Flush();
    tag_message dragMessage = gpInputManager->GetEvent();
    while (dragMessage.type != MESSAGE_LEFT_BUTTON_UP
           && dragMessage.type != MESSAGE_RIGHT_BUTTON_UP) {
        if (dragMessage.type == MESSAGE_MOUSE_MOVE) {
            if (static_cast<float>(dragMessage.payload.mouse.y) < mouseOffset2 + fGutterMinY) {
                dragMessage.payload.mouse.y = static_cast<i32>(mouseOffset2 + fGutterMinY);
            }
            if (mouseOffset2 + fGutterTravelLength + fGutterMinY
                < static_cast<float>(dragMessage.payload.mouse.y)) {
                dragMessage.payload.mouse.y =
                    static_cast<i32>(mouseOffset2 + fGutterTravelLength + fGutterMinY);
            }
            gpMouseManager->Main(dragMessage);
            m_scrollKnob->m_y = dragMessage.payload.mouse.y - mouseOffset2;
            if (m_fileCount > iMaxListSize) {
                newTopIndex = static_cast<i32>((m_scrollKnob->m_y - fGutterMinY) / gutterStep9);
                if (OD_STEER(newTopIndex) != oldTopIndex) {
                    if (newTopIndex > m_fileCount - iMaxListSize) {
                        newTopIndex = m_fileCount - iMaxListSize;
                    }
                    if (newTopIndex < 0) {
                        newTopIndex = 0;
                    }
                    m_topIndex = newTopIndex;
                    Update(0);
                    m_scrollKnob->m_y = dragMessage.payload.mouse.y - mouseOffset2;
                    m_window->DrawWindow(1, 0, 0x7fff);
                    oldTopIndex = newTopIndex;
                } else {
                    m_window->DrawWindow(1, 0, 0x7fff);
                }
            } else {
                m_window->DrawWindow(1, 0, 0x7fff);
            }
        }
        Process1WindowsMessage();
        dragMessage = gpInputManager->GetEvent();
    }
    m_scrollKnob->m_flags &= ~1;
    Update(1);
}

// @semantic
// Complete 0x1bc frame/slots and CFG; all 71/71 relocation owners/addends agree.
// First divergence is at +0x81: retail loads giMapSizeFilter then compares the
// loop index against EAX, while candidate loads the loop index then compares the
// global against EAX. The equivalent operand order shifts later relocation sites.
VA(0x0048ef82, 0xc42)
void fileRequester::Update(i32 drawWindow) {
    tag_message broadcastMessage;
    char localStorage[FILE_REQUESTER_UPDATE_STORAGE_SIZE];
    i32 localState;
    i32 i;
    double gutterStepCount;
    double gutterState0;
    broadcastMessage.type = MESSAGE_WIDGET;
    localState = 0;

    if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
        for (i = 0; i < FILE_REQUESTER_MAP_SIZE_COUNT; ++i) {
            broadcastMessage.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            broadcastMessage.payload.widget.id = FILE_REQUESTER_FILTER_SMALL + i;
            broadcastMessage.payload.widget.data.value = (giMapSizeFilter == i) + i * 2 + 9;
            m_window->BroadcastMessage(broadcastMessage);
        }
        if (m_selectedIndex == FILE_REQUESTER_SELECTION_NONE && m_fileCount > 0) {
            m_selectedIndex = 0;
        }
        SetOK(1);

        broadcastMessage.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        broadcastMessage.payload.widget.id = FILE_REQUESTER_MAP_SIZE;
        if (m_mapHeaders[m_selectedIndex].width == MAP_DIMENSION_SMALL) {
            broadcastMessage.payload.widget.data.value = MAP_SIZE_FRAME_SMALL;
        } else if (m_mapHeaders[m_selectedIndex].width == MAP_DIMENSION_MEDIUM) {
            broadcastMessage.payload.widget.data.value = MAP_SIZE_FRAME_MEDIUM;
        } else if (m_mapHeaders[m_selectedIndex].width == MAP_DIMENSION_LARGE) {
            broadcastMessage.payload.widget.data.value = MAP_SIZE_FRAME_LARGE;
        } else {
            broadcastMessage.payload.widget.data.value = MAP_SIZE_FRAME_XLARGE;
        }
        m_window->BroadcastMessage(broadcastMessage);

        broadcastMessage.payload.widget.id = FILE_REQUESTER_MAP_PLAYER_COUNT;
        broadcastMessage.payload.widget.data.value =
            m_mapHeaders[m_selectedIndex].playerCount + PLAYER_COUNT_FRAME_BASE;
        m_window->BroadcastMessage(broadcastMessage);

        broadcastMessage.payload.widget.id = FILE_REQUESTER_MAP_VICTORY;
        broadcastMessage.payload.widget.data.value =
            m_mapHeaders[m_selectedIndex].victoryCondition + VICTORY_FRAME_BASE;
        m_window->BroadcastMessage(broadcastMessage);

        broadcastMessage.payload.widget.id = FILE_REQUESTER_MAP_LOSS;
        broadcastMessage.payload.widget.data.value =
            m_mapHeaders[m_selectedIndex].lossCondition + LOSS_FRAME_BASE;
        m_window->BroadcastMessage(broadcastMessage);

        broadcastMessage.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        broadcastMessage.payload.widget.data.text = gText;
        sprintf(gText, "%s", m_mapHeaders[m_selectedIndex].name);
        broadcastMessage.payload.widget.id = FILE_REQUESTER_MAP_NAME;
        m_window->BroadcastMessage(broadcastMessage);

        sprintf(gText, "%s", cDifficulty[m_mapHeaders[m_selectedIndex].difficulty]);
        broadcastMessage.payload.widget.id = FILE_REQUESTER_MAP_DIFFICULTY_TEXT;
        m_window->BroadcastMessage(broadcastMessage);

        sprintf(gText, "%s", m_mapHeaders[m_selectedIndex].description);
        broadcastMessage.payload.widget.id = FILE_REQUESTER_MAP_DESCRIPTION;
        m_window->BroadcastMessage(broadcastMessage);
    }

    for (i = 0; iMaxListSize > i; ++i) {
        if (m_topIndex + i >= m_fileCount) {
            broadcastMessage.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            broadcastMessage.payload.widget.data.value = 4;
            broadcastMessage.payload.widget.id = i + FILE_REQUESTER_LIST_TEXT_FIRST;
            m_window->BroadcastMessage(broadcastMessage);
            if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
                broadcastMessage.payload.widget.id = i + FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
                m_window->BroadcastMessage(broadcastMessage);
                broadcastMessage.payload.widget.id = i + FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
                m_window->BroadcastMessage(broadcastMessage);
                broadcastMessage.payload.widget.id = i + FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
                m_window->BroadcastMessage(broadcastMessage);
                broadcastMessage.payload.widget.id = i + FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
                m_window->BroadcastMessage(broadcastMessage);
            }
        } else {
            broadcastMessage.payload.widget.id = i + FILE_REQUESTER_LIST_TEXT_FIRST;
            broadcastMessage.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            broadcastMessage.payload.widget.data.value = 4;
            m_window->BroadcastMessage(broadcastMessage);

            if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
                broadcastMessage.payload.widget.id = i + FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
                m_window->BroadcastMessage(broadcastMessage);
                broadcastMessage.payload.widget.id = i + FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
                m_window->BroadcastMessage(broadcastMessage);
                broadcastMessage.payload.widget.id = i + FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
                m_window->BroadcastMessage(broadcastMessage);
                broadcastMessage.payload.widget.id = i + FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
                m_window->BroadcastMessage(broadcastMessage);

                broadcastMessage.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                broadcastMessage.payload.widget.id = i + FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
                if (m_mapHeaders[m_topIndex + i].width == MAP_DIMENSION_SMALL) {
                    broadcastMessage.payload.widget.data.value = MAP_SIZE_FRAME_SMALL;
                } else if (m_mapHeaders[m_topIndex + i].width == MAP_DIMENSION_MEDIUM) {
                    broadcastMessage.payload.widget.data.value = MAP_SIZE_FRAME_MEDIUM;
                } else if (m_mapHeaders[m_topIndex + i].width == MAP_DIMENSION_LARGE) {
                    broadcastMessage.payload.widget.data.value = MAP_SIZE_FRAME_LARGE;
                } else {
                    broadcastMessage.payload.widget.data.value = MAP_SIZE_FRAME_XLARGE;
                }
                m_window->BroadcastMessage(broadcastMessage);

                broadcastMessage.payload.widget.id = i + FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
                broadcastMessage.payload.widget.data.value =
                    m_mapHeaders[m_topIndex + i].playerCount + PLAYER_COUNT_FRAME_BASE;
                m_window->BroadcastMessage(broadcastMessage);

                broadcastMessage.payload.widget.id = i + FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
                broadcastMessage.payload.widget.data.value =
                    m_mapHeaders[m_topIndex + i].victoryCondition + VICTORY_FRAME_BASE;
                m_window->BroadcastMessage(broadcastMessage);

                broadcastMessage.payload.widget.id = i + FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
                broadcastMessage.payload.widget.data.value =
                    m_mapHeaders[m_topIndex + i].lossCondition + LOSS_FRAME_BASE;
                m_window->BroadcastMessage(broadcastMessage);
            }

            broadcastMessage.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
                sprintf(gText, "%s", m_mapHeaders[m_topIndex + i].name);
            } else {
                sprintf(gText, "%s", m_fileNames[m_topIndex + i].text);
            }
            broadcastMessage.payload.widget.data.text = gText;
            broadcastMessage.payload.widget.id = i + FILE_REQUESTER_LIST_TEXT_FIRST;
            m_window->BroadcastMessage(broadcastMessage);
        }

        broadcastMessage.payload.widget.id = i + FILE_REQUESTER_LIST_TEXT_FIRST;
        broadcastMessage.payload.widget.command = WIDGET_COMMAND_SET_FILL_COLOR;
        if (m_topIndex + i == m_selectedIndex) {
            broadcastMessage.payload.widget.data.value = 2;
        } else {
            broadcastMessage.payload.widget.data.value = 1;
        }
        m_window->BroadcastMessage(broadcastMessage);
    }

    broadcastMessage.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
    broadcastMessage.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
    broadcastMessage.payload.widget.data.value = 2;
    m_window->BroadcastMessage(broadcastMessage);
    if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
        broadcastMessage.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
            sprintf(gText, "%s", m_mapHeaders[m_selectedIndex].name);
        } else {
            sprintf(gText, "%s", m_fileNames[m_selectedIndex].text);
        }
        broadcastMessage.payload.widget.data.text = gText;
        m_window->BroadcastMessage(broadcastMessage);
    }
    if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_LOAD_GAME
        || m_mode == FILE_REQUESTER_MAP) {
        broadcastMessage.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        broadcastMessage.payload.widget.data.value = 2;
        m_window->BroadcastMessage(broadcastMessage);
    }

    if (m_fileCount <= iMaxListSize) {
        m_scrollKnob->m_y = static_cast<i16>(fGutterTravelLength / 2.0f + fGutterMinY);
    } else {
        gutterStepCount = fGutterTravelLength / (m_fileCount - iMaxListSize);
        m_scrollKnob->m_y = static_cast<i16>(m_topIndex * gutterStepCount + fGutterMinY);
    }
    if (drawWindow) {
        m_window->DrawWindow(1, 0, 0x7fff);
    }
}

// @semantic
// Complete 0x15b body, 0x4 frame/this slot, CFG, and all 12 ordered relocations
// agree. At +0x3c retail keeps the positive fileCount > selectedIndex test as
// `jg`; base canonicalizes the equivalent invalid guard to `jl`. Positive-arm,
// negated-positive, and explicit !(>) spellings were exhausted.
VA(0x0048fbc4, 0x15b)
char* fileRequester::GetFilename(void) {
    if (m_mode != FILE_REQUESTER_SAVE_GAME
        && (m_selectedIndex < 0 || m_fileCount <= m_selectedIndex)) {
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

// ===== vtable fileRequester : public baseManager  (3 slots) =====
//  [ 0] VA(0x0048d5e1, 0x466)  int fileRequester::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x0048d556, 0x8b)  void fileRequester::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x0048daec, 0x11ae)  int fileRequester::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(fileRequester, 0x004eb888);

// @data-layout-note Retail initialized storage is 0xf8674..0xf8900.
// Thirty-six private owners plus giMapSizeFilter and cFRDummy cover all
// non-alignment bytes. cFRDummy's stored pointer proves its empty-string owner
// at 0xf88e4. In Main, the first absolute relocation proves the empty string at
// 0xf8710; the later corresponding strcpy site proves the copy at 0xf87b0.
// Both candidate addends are zero. The remaining owners have singleton
// relocation or unique-payload proof. The final three retail bytes are zero
// alignment. Both rdata contributions and the 0xc-byte BSS contribution are
// already exact; do not model alignment with a fake owner.

// ---- globals (definitions, RVA order) ----
DATA(0x004f8674) FileRequesterMapSizeFilter giMapSizeFilter = FILE_REQUESTER_MAP_SIZE_ALL;
DATA(0x004f88c4) char* cFRDummy = "";
DATA(0x0052857c) float fGutterMinY;
DATA(0x00528580) float fGutterTravelLength;
DATA(0x00528584) i32 iMaxListSize;
