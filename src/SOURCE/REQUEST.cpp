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
VA(0x0048c920, 0x80)
int GetMapHeader(char *filename, struct SMapHeader *header)
{
    sprintf(gText, "%s%s", gcMapPath, filename);
    int file = _open(gText, _O_BINARY);
    if (file == -1) {
        return 0;
    }
    _read(file, header, sizeof(*header));
    _close(file);
    return 1;
}

VA(0x0048c9a0, 0x1b)
int CheckSumIsDemoOK(char *)
{
    return 1;
}

// @match-note 99.79%: semantics, CFG, the 0x134 frame, and all 4/4 external
// relocations agree. Retail's declared 0xbb-byte range differs only at the
// pooled "BROKENA" string identity after normalization; this build retains an
// additional dead-code exit sequence beyond that comparison range. Revisit the
// unreachable return shape after SOURCE reaches 95% globally.
VA(0x0048c9bb, 0xbb)
int ShowThisMapGame(char *filename)
{
    return 1;

    char mapName[300];
    strcpy(mapName, filename);
    mapName[8] = 0;
    for (int i = 0; i < 8; ++i) {
        if (mapName[i] == '.') {
            mapName[i] = 0;
        }
    }
    if (_strcmpi(mapName, "BROKENA") == 0 && CheckSumIsDemoOK(filename)) {
        return 1;
    }
    return 0;
}

VA(0x0048ca76, 0x1b)
int ShowThisMap(char *)
{
    return 1;
}

// @match-note retained 95.32%, live 94.22%: both enumeration passes,
// allocation, sorted insertion, extensions, and map-header population are
// complete, with all 53 external relocations agreeing. The retail 0x65c frame
// and every aggregate/local slot now agree: fullPath/mapHeader/findData at
// -0x658/-0x52c/-0x378, extension/fileName at -0x238/-0x164, and the reused
// index at -0x37c. The first residual is the map-size comparison: ours emits a
// local jne, while retail emits je followed by two continuation jumps. The old
// negative predicate retained the maximum; the combined positive predicate is
// structurally closer, while splitting it into nested positive arms fell to
// 89.81%. Revisit only in the post-95 CFG/continuation phase.
VA(0x0048ca91, 0x7c1)
int fileRequester::InitializeFiles(char *directory, char *pattern, int countOnly)
{
    char fullPath[FILE_REQUESTER_PATH_SIZE];
    SMapHeader mapHeader;
    WIN32_FIND_DATA findDataPath;
    char extensionEntry[FILE_REQUESTER_LOCAL_EXTENSION_SIZE];
    char fileName[FILE_REQUESTER_LOCAL_NAME_SIZE];
    HANDLE findHandleWork;
    int moreFilesHandle;
    int insertedCountResult;
    char *dotSource;
    int moveValue;
    int indexData;

    sprintf(gText, "%s%s", directory, pattern);
    m_fileCount = 0;
    moreFilesHandle = 1;
    findHandleWork = FindFirstFile(gText, &findDataPath);
    if (findHandleWork != INVALID_HANDLE_VALUE) {
        while (moreFilesHandle) {
            if (m_mode == FILE_REQUESTER_MAP_GAME) {
                GetMapHeader(findDataPath.cFileName, &mapHeader);
                if (mapHeader.minHumanPlayers <= giNumHumanPlayers &&
                    giNumHumanPlayers <= mapHeader.maxHumanPlayers &&
                    (giMapSizeFilter == FILE_REQUESTER_MAP_SIZE_ALL ||
                     giMapSizes[giMapSizeFilter] == mapHeader.width) &&
                    ShowThisMapGame(findDataPath.cFileName)) {
                } else {
                    goto CountNextFile;
                }
            }
            if (m_mode == FILE_REQUESTER_MAP) {
                GetMapHeader(findDataPath.cFileName, &mapHeader);
                if ((giMapSizeFilter == FILE_REQUESTER_MAP_SIZE_ALL ||
                     giMapSizes[giMapSizeFilter] == mapHeader.width) &&
                    ShowThisMap(findDataPath.cFileName)) {
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
                if (mapHeader.minHumanPlayers <= giNumHumanPlayers &&
                    giNumHumanPlayers <= mapHeader.maxHumanPlayers &&
                    (giMapSizeFilter == FILE_REQUESTER_MAP_SIZE_ALL ||
                     giMapSizes[giMapSizeFilter] == mapHeader.width) &&
                    ShowThisMapGame(findDataPath.cFileName)) {
                } else {
                    goto InsertNextFile;
                }
            }
            if (m_mode == FILE_REQUESTER_MAP) {
                GetMapHeader(findDataPath.cFileName, &mapHeader);
                if ((giMapSizeFilter == FILE_REQUESTER_MAP_SIZE_ALL ||
                     giMapSizes[giMapSizeFilter] == mapHeader.width) &&
                    ShowThisMap(findDataPath.cFileName)) {
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
                        strcpy(m_fileNames[moveValue].text,
                               m_fileNames[moveValue - 1].text);
                        strcpy(m_extensions[moveValue].text,
                               m_extensions[moveValue - 1].text);
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
fileRequester::fileRequester(int x, int y, int mode, char *pattern, char *directory,
                             char *defaultExtension)
{
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
int fileRequester::MapExistsForFilter(FileRequesterMapSizeFilter filter)
{
    FileRequesterMapSizeFilter oldFilter = giMapSizeFilter;
    giMapSizeFilter = filter;
    int result = InitializeFiles(m_directory, m_filePattern, 1);
    giMapSizeFilter = oldFilter;
    return result > 0;
}

VA(0x0048d423, 0x6c)
void fileRequester::SetupFiles(void)
{
    CleanUpData();
    m_fileCount = 0;
    m_topIndex = 0;
    m_result = -2;
    m_selectedIndex = FILE_REQUESTER_SELECTION_NONE;
    InitializeFiles(m_directory, m_filePattern, 0);
}

VA(0x0048d48f, 0xc7)
void fileRequester::CleanUpData(void)
{
    if (m_fileNames != 0) {
        delete [] m_fileNames;
    }
    m_fileNames = 0;
    if (m_extensions != 0) {
        delete [] m_extensions;
    }
    m_extensions = 0;
    if (m_mapHeaders != 0) {
        delete [] m_mapHeaders;
    }
    m_mapHeaders = 0;
}

VA(0x0048d556, 0x8b)
void fileRequester::Close(void)
{
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

// @early-stop 99.92%: normalized instructions, the 0x44 frame and all local
// slots are exact; all 47 external relocations agree. Objdiff's remaining
// bytes are only delinked pooled-string symbol identities (requests.bin,
// request.bin, scrollcn.icn and dialog labels), not code or target changes.
VA(0x0048d5e1, 0x466)
int fileRequester::Open(int id)
{
    strcpy(gLastFilename, "");
    m_previousMenu = hmnuCurrent;
    KBChangeMenu(hmnuDflt);

    m_window = new heroWindow(m_x, m_y,
                              m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP
                                  ? "requests.bin" : "request.bin");
    if (m_window == 0) {
        MemError();
    }

    m_scrollKnob = new iconWidget(346, static_cast<short>(fGutterMinY), 8, 17,
                                  "scrollcn.icn", 4, 0, 14, 16, 1);
    if (m_scrollKnob == 0) {
        MemError();
    }
    m_window->AddWidget(m_scrollKnob, -1);

    tag_message message;
    message.type = 0x200;
    message.payload.widget.command = 3;
    unsigned char okEnabled;
    int i;
    if (m_mode == FILE_REQUESTER_SAVE_GAME) {
        okEnabled = 1;
        strcpy(m_filename, gpGame->m_saveName);
        char *dot = FindLastToken(m_filename, '.');
        if (dot != 0) {
            *dot = 0;
        }
        message.payload.widget.id = 15;
        message.payload.widget.data.text = m_filename;
        m_window->BroadcastMessage(message);
        message.payload.widget.id = 16;
        sprintf(gText, "File to Save:");
        message.payload.widget.data.text = gText;
        m_window->BroadcastMessage(message);
        for (i = 0; i < m_fileCount; ++i) {
            if (_strcmpi(m_fileNames[i].text, m_filename) == 0) {
                m_selectedIndex = i;
            }
        }
    } else {
        okEnabled = 0;
        if (m_mode == FILE_REQUESTER_MAP_GAME) {
            char mapName[12];
            i = 0;
            memset(mapName, 0, 9);
            while (i < 8 && gMapName[i] != 0 && gMapName[i] != '.') {
                mapName[i] = gMapName[i];
                ++i;
            }
            for (i = 0; i < m_fileCount; ++i) {
                if (_strcmpi(m_fileNames[i].text, mapName) == 0) {
                    m_selectedIndex = i;
                    okEnabled = 1;
                }
            }
        }
        message.payload.widget.id = 16;
        sprintf(gText, "File to Load:");
        message.payload.widget.data.text = gText;
        m_window->BroadcastMessage(message);
    }

    message.type = 0x200;
    message.payload.widget.command = 0x33;
    message.payload.widget.id = 15;
    message.payload.widget.data.value = 201;
    m_window->BroadcastMessage(message);
    Update(0);
    if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
        okEnabled = 1;
    }
    gpWindowManager->AddWindow(m_window, -1, 1);
    if (m_fileCount == 0) {
        okEnabled = 0;
    }
    if (m_mode == FILE_REQUESTER_SAVE_GAME && _strcmpi(m_filename, "NEWGAME") == 0 &&
        m_selectedIndex == FILE_REQUESTER_SELECTION_NONE) {
        okEnabled = 1;
    }
    SetOK(okEnabled);
    m_messageMask = 0x4000;
    m_priority = id;
    m_active = 1;
    strcpy(m_name, "fileRequester");
    return 0;
}

VA(0x0048da47, 0xa5)
void fileRequester::SetOK(int enabled)
{
    tag_message message;
    message.type = 0x200;
    if (enabled) {
        message.payload.widget.command = 6;
    } else {
        message.payload.widget.command = 5;
    }
    message.payload.widget.id = 0x7802;
    if (m_active == 1) {
        message.payload.widget.data.value = 8;
    } else {
        message.payload.widget.data.value = 0x1000;
    }
    m_window->BroadcastMessage(message);
    if (enabled) {
        message.payload.widget.command = 5;
    } else {
        message.payload.widget.command = 6;
    }
    message.payload.widget.data.value = 2;
    m_window->BroadcastMessage(message);
}

// @match-note retained/live 95.11%: key, click, hover/help, filters, filename,
// knob, list selection, saved-game validation and executive exit are complete.
// All primary retail slots agree: this/accept/index at -0x3fc/-0x16c/-0x170,
// edit buffer -0x160, names -0x2a0/-0x3f4, help -0x2a4 and message -0x2c0.
// This build's 0x41c frame is short only one compiler-generated FP temporary
// below this versus retail 0x420; adding a source local shifts this and is not
// valid. The first code residual is the key-up selected/top compare polarity.
// Empty positive <= and >= spellings both fell to 95.08%; moving click behind
// hover/help fell to 88.20%. Explicit jump-table-excluded object-range review
// finds all 76 external relocations agreeing; only local tables/pooled strings
// differ. Revisit the switch/FP compiler shape after global SOURCE reaches 95%.
VA(0x0048daec, 0x11ae)
int fileRequester::Main(struct tag_message &message)
{
    int acceptStep = 0;
    int iResult;
    int helpIndexMouse;
    int lengthIndex;
    int positions;
    int gutterStepScreen;
    int mouseXIndex;
    int screenY;
    int newTopIndexBuffer;
    char newNameData[FILE_REQUESTER_LOCAL_NAME_SIZE];
    char cycleNameBuffer[FILE_REQUESTER_PATH_SIZE];
    char filteredNameMap[FILE_REQUESTER_PATH_SIZE];
    tag_message broadcastMessage;

    switch (message.type) {
    case FILE_REQUESTER_EVENT_KEY:
        switch (message.payload.keyboard.keyCode) {
        case FILE_REQUESTER_KEY_CYCLE_FILTER: {
            if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
                strcpy(cycleNameBuffer, m_fileNames[m_selectedIndex].text);
            } else {
                strcpy(cycleNameBuffer, "");
            }
            giMapSizeFilter = static_cast<FileRequesterMapSizeFilter>(
                (giMapSizeFilter + 1) % FILE_REQUESTER_MAP_SIZE_COUNT);
            SetupFiles();
            if (strlen(cycleNameBuffer) != 0) {
                for (iResult = 0; iResult < m_fileCount; ++iResult) {
                    if (_strcmpi(m_fileNames[iResult].text,
                                 cycleNameBuffer) == 0) {
                        m_selectedIndex = iResult;
                    }
                }
            }
            Update(1);
            break;
        }
        case FILE_REQUESTER_KEY_UP:
            if (m_selectedIndex > 0) {
                --m_selectedIndex;
                if (m_selectedIndex < m_topIndex) {
                    --m_topIndex;
                }
                Update(1);
            }
            break;
        case FILE_REQUESTER_KEY_DOWN:
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
    case FILE_REQUESTER_EVENT_WIDGET:
        switch (message.payload.widget.command) {
        case FILE_REQUESTER_WIDGET_CLICK:
                if (message.payload.widget.id < FILE_REQUESTER_OK) {
                    if (message.payload.widget.id == FILE_REQUESTER_CANCEL) {
                        message.payload.widget.data.value =
                            message.payload.widget.id;
                        acceptStep = 1;
                    } else if (message.payload.widget.id ==
                               FILE_REQUESTER_SCROLL_UP) {
                        if (m_topIndex > 0) {
                            --m_topIndex;
                            Update(1);
                        }
                    } else if (message.payload.widget.id ==
                                   FILE_REQUESTER_SCROLL_DOWN &&
                               m_topIndex + iMaxListSize < m_fileCount) {
                        ++m_topIndex;
                        if (m_topIndex + iMaxListSize - 1 >= m_fileCount) {
                            m_topIndex = m_fileCount - iMaxListSize;
                        }
                        Update(1);
                    }
                } else if (message.payload.widget.id == FILE_REQUESTER_OK) {
                    if (m_selectedIndex == FILE_REQUESTER_SELECTION_NONE && m_filename[0] == 0) {
                        NormalDialog(
                            "Please make a selection from the list, or press cancel.",
                            NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE,
                            NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE,
                            0, NORMAL_DIALOG_NO_RESOURCE, 0,
                            NORMAL_DIALOG_NO_RESOURCE, 0);
                    } else {
                        message.payload.widget.data.value =
                            message.payload.widget.id;
                        acceptStep = 1;
                    }
                }
                break;
        case FILE_REQUESTER_WIDGET_HOVER:
        case FILE_REQUESTER_WIDGET_HELP:
                if ((message.payload.widget.parameter &
                     FILE_REQUESTER_HELP_MODIFIER) != 0) {
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
                        if (message.payload.widget.id >=
                                FILE_REQUESTER_MAP_SIZE_ICON_FIRST &&
                            message.payload.widget.id <
                                FILE_REQUESTER_MAP_SIZE_ICON_FIRST +
                                    FILE_REQUESTER_LIST_RANGE_SIZE) {
                            helpIndexMouse = FILE_REQUESTER_HELP_MAP_SIZE;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_MAP_PLAYER_ICON_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_MAP_PLAYER_ICON_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            helpIndexMouse = FILE_REQUESTER_HELP_PLAYER_COUNT;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_MAP_VICTORY_ICON_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_MAP_VICTORY_ICON_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            helpIndexMouse = FILE_REQUESTER_HELP_VICTORY;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_MAP_LOSS_ICON_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_MAP_LOSS_ICON_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            helpIndexMouse = FILE_REQUESTER_HELP_LOSS;
                        }
                        break;
                    }
                    if (helpIndexMouse >= 0) {
                        NormalDialog(
                            gFileRequestHelp[helpIndexMouse],
                            NORMAL_DIALOG_QUICK_VIEW,
                            NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE,
                            NORMAL_DIALOG_NO_RESOURCE, 0,
                            NORMAL_DIALOG_NO_RESOURCE, 0,
                            NORMAL_DIALOG_NO_RESOURCE, 0);
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
                                static_cast<FileRequesterMapSizeFilter>(iResult))) {
                            if (giNumHumanPlayers == 1) {
                                sprintf(gText,
                                        "No maps exist for %d human player at that size.",
                                        giNumHumanPlayers);
                            } else {
                                sprintf(gText,
                                        "No maps exist for %d human players at that size.",
                                        giNumHumanPlayers);
                            }
                            NormalDialog(
                                gText, NORMAL_DIALOG_INFO,
                                NORMAL_DIALOG_NO_RESOURCE,
                                NORMAL_DIALOG_NO_VALUE,
                                NORMAL_DIALOG_NO_RESOURCE, 0,
                                NORMAL_DIALOG_NO_RESOURCE, 0,
                                NORMAL_DIALOG_NO_RESOURCE, 0);
                        } else {
                            giMapSizeFilter =
                                static_cast<FileRequesterMapSizeFilter>(iResult);
                            if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
                                strcpy(filteredNameMap,
                                       m_fileNames[m_selectedIndex].text);
                            } else {
                                strcpy(filteredNameMap, "");
                            }
                            SetupFiles();
                            if (strlen(filteredNameMap) != 0) {
                                for (iResult = 0; iResult < m_fileCount;
                                     ++iResult) {
                                    if (_strcmpi(m_fileNames[iResult].text,
                                                 filteredNameMap) == 0) {
                                        m_selectedIndex = iResult;
                                    }
                                }
                            }
                            Update(1);
                        }
                        break;
                    }
                    case FILE_REQUESTER_FILENAME_ENTRY: {
                        broadcastMessage.type = FILE_REQUESTER_EVENT_WIDGET;
                        broadcastMessage.payload.widget.command =
                            FILE_REQUESTER_WIDGET_GET_TEXT;
                        broadcastMessage.payload.widget.id =
                            FILE_REQUESTER_FILENAME_ENTRY;
                        m_window->BroadcastMessage(broadcastMessage);

                        memset(newNameData, 0,
                               FILE_REQUESTER_FILENAME_INITIAL_CLEAR_SIZE);
                        strcpy(newNameData,
                               broadcastMessage.payload.widget.data.text);
                        lengthIndex = strlen(newNameData);
                        for (iResult = 0; iResult < lengthIndex; ++iResult) {
                            if (!((newNameData[iResult] >= 'A' &&
                                   newNameData[iResult] <= 'Z') ||
                                  (newNameData[iResult] >= 'a' &&
                                   newNameData[iResult] <= 'z') ||
                                  (newNameData[iResult] >= '0' &&
                                   newNameData[iResult] <= '9') ||
                                  newNameData[iResult] == '_' ||
                                  newNameData[iResult] == ' ' ||
                                  FindToken("$%'-_@~`!(){}^#&+,;=[].",
                                            newNameData[iResult]) !=
                                      0)) {
                                newNameData[iResult] = 0;
                            }
                        }
                        for (iResult = strlen(newNameData) - 1; iResult >= 0;
                             --iResult) {
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
                        broadcastMessage.payload.widget.command =
                            FILE_REQUESTER_WIDGET_SET_TEXT;
                        broadcastMessage.payload.widget.id =
                            FILE_REQUESTER_FILENAME_ENTRY;
                        broadcastMessage.payload.widget.data.text = m_filename;
                        m_window->BroadcastMessage(broadcastMessage);
                        Update(1);
                        break;
                    }
                    case FILE_REQUESTER_SCROLL_KNOB: {
                        positions = m_fileCount - (iMaxListSize - 1);
                        if (positions < 1)
                            positions = 1;
                        gutterStepScreen = static_cast<int>(
                            (fGutterTravelLength *
                             FILE_REQUESTER_GUTTER_SCALE) /
                            positions);
                        mouseXIndex = message.payload.widget.screenX;
                        screenY = message.payload.widget.screenY;
                        screenY = static_cast<int>(
                                      screenY - (m_y + fGutterMinY)) -
                                 FILE_REQUESTER_SCROLL_KNOB_HALF_HEIGHT;
                        newTopIndexBuffer =
                            (screenY * FILE_REQUESTER_GUTTER_SCALE) /
                            gutterStepScreen;
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
                        if (message.payload.widget.id >=
                                FILE_REQUESTER_MAP_SIZE_ICON_FIRST &&
                            message.payload.widget.id <
                                FILE_REQUESTER_MAP_SIZE_ICON_FIRST +
                                    FILE_REQUESTER_LIST_RANGE_SIZE) {
                            iResult = message.payload.widget.id -
                                FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_MAP_PLAYER_ICON_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_MAP_PLAYER_ICON_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            iResult = message.payload.widget.id -
                                FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_MAP_VICTORY_ICON_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_MAP_VICTORY_ICON_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            iResult = message.payload.widget.id -
                                FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_MAP_LOSS_ICON_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_MAP_LOSS_ICON_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            iResult = message.payload.widget.id -
                                FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_LIST_TEXT_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_LIST_TEXT_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            iResult = message.payload.widget.id -
                                FILE_REQUESTER_LIST_TEXT_FIRST;
                        } else {
                            break;
                        }
                        if (iResult + m_topIndex == m_selectedIndex) {
                            message.payload.widget.data.value =
                                FILE_REQUESTER_OK;
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
        if (m_mode == FILE_REQUESTER_LOAD_GAME && m_selectedIndex >= 0 &&
            message.payload.widget.data.value != FILE_REQUESTER_CANCEL &&
            _strcmpi(m_extensions[m_selectedIndex].text, ".GMC") != 0 &&
            _strcmpi(m_extensions[m_selectedIndex].text, ".GXC") != 0) {
            iResult = m_extensions[m_selectedIndex]
                    .text[FILE_REQUESTER_EXTENSION_PLAYER_DIGIT] -
                '0';
            if (iResult < giNumHumanPlayers &&
                giDebugLevel < FILE_REQUESTER_DEBUG_ALLOW_PLAYER_MISMATCH) {
                sprintf(gText,
                        "The game you have chosen only has slots for %d human(s).  You need one with room for at least %d humans.",
                        iResult, giNumHumanPlayers);
                NormalDialog(gText, NORMAL_DIALOG_INFO,
                             NORMAL_DIALOG_NO_RESOURCE,
                             NORMAL_DIALOG_NO_VALUE,
                             NORMAL_DIALOG_NO_RESOURCE, 0,
                             NORMAL_DIALOG_NO_RESOURCE, 0,
                             NORMAL_DIALOG_NO_RESOURCE, 0);
                acceptStep = 0;
            }
            if (iResult > giNumHumanPlayers) {
                sprintf(gText,
                        "The game you have chosen was being played with %d humans. Is it OK if the computer takes the place of the last %d human(s)?",
                        iResult, iResult - giNumHumanPlayers);
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM,
                             NORMAL_DIALOG_NO_RESOURCE,
                             NORMAL_DIALOG_NO_VALUE,
                             NORMAL_DIALOG_NO_RESOURCE, 0,
                             NORMAL_DIALOG_NO_RESOURCE, 0,
                             NORMAL_DIALOG_NO_RESOURCE, 0);
                if (gpWindowManager->m_dialogResult !=
                    NORMAL_DIALOG_BUTTON_FIVE) {
                    acceptStep = 0;
                }
            }
        }
        if (acceptStep != 0) {
            message.type = FILE_REQUESTER_EVENT_EXIT;
            message.payload.executive.command =
                FILE_REQUESTER_EXECUTIVE_CLOSE;
            return FILE_REQUESTER_MAIN_CLOSE;
        }
    }
    return FILE_REQUESTER_MAIN_CONTINUE;
}

// @early-stop 99.82%: all 228 normalized instructions and the 0x94 frame/local
// slots are exact, and all 36 external relocations agree. The residual is only
// delinked local-symbol identity after relocation masking; no opcode, operand,
// stack displacement, call target or external data target differs.
VA(0x0048ec9a, 0x2e8)
void fileRequester::DoKnob(void)
{
    int oldTopIndex = m_topIndex;
    double gutterStep = fGutterTravelLength / (m_fileCount - (iMaxListSize - 1));
    int mouseX;
    int mouseY;
    gpMouseManager->MouseCoords(mouseX, mouseY);
    int mouseOffset = mouseY - m_scrollKnob->m_y;

    gpInputManager->Flush();
    tag_message message = gpInputManager->GetEvent();
    while (message.type != 0x10 && message.type != 0x40) {
        if (message.type == 4) {
            if (static_cast<float>(message.payload.mouse.y) < mouseOffset + fGutterMinY) {
                message.payload.mouse.y = static_cast<int>(mouseOffset + fGutterMinY);
            }
            if (mouseOffset + fGutterTravelLength + fGutterMinY <
                static_cast<float>(message.payload.mouse.y)) {
                message.payload.mouse.y = static_cast<int>(mouseOffset + fGutterTravelLength +
                                                  fGutterMinY);
            }
            gpMouseManager->Main(message);
            m_scrollKnob->m_y = message.payload.mouse.y - mouseOffset;
            if (m_fileCount > iMaxListSize) {
                int newTopIndex = static_cast<int>((m_scrollKnob->m_y - fGutterMinY) /
                                                   gutterStep);
                if (newTopIndex != oldTopIndex) {
                    if (newTopIndex > m_fileCount - iMaxListSize) {
                        newTopIndex = m_fileCount - iMaxListSize;
                    }
                    if (newTopIndex < 0) {
                        newTopIndex = 0;
                    }
                    m_topIndex = newTopIndex;
                    Update(0);
                    m_scrollKnob->m_y = message.payload.mouse.y - mouseOffset;
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
        message = gpInputManager->GetEvent();
    }
    m_scrollKnob->m_flags &= ~1;
    Update(1);
}

// @match-note retained 99.65%, live 99.53%: behavior and CFG are complete; all
// 71 external relocations agree. The 0x1bc frame and accessed slots now match,
// including message -0x19c, index -0x1a0, state -0x180 and this -0x1b4; retail
// also reserves the evidenced 380-byte local storage and unused eight-byte
// gutter state. The first code residual is operand load order for the filter
// equality, followed by the equivalent list-bound comparison. Reversing both
// source comparisons did not change those instructions. The remaining diff is
// pooled %s/float-constant identity. Revisit register steering after 95%.
VA(0x0048ef82, 0xc42)
void fileRequester::Update(int drawWindow)
{
    tag_message broadcastMessage;
    char localStorage[FILE_REQUESTER_UPDATE_STORAGE_SIZE];
    int localState = 0;
    int i;
    double gutterStepCount;
    double gutterState;
    broadcastMessage.type = 0x200;

    if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
        for (i = 0; i < FILE_REQUESTER_MAP_SIZE_COUNT;
             ++i) {
            broadcastMessage.payload.widget.command = 4;
            broadcastMessage.payload.widget.id =
                FILE_REQUESTER_FILTER_SMALL + i;
            broadcastMessage.payload.widget.data.value =
                (giMapSizeFilter == i) + i * 2 + 9;
            m_window->BroadcastMessage(broadcastMessage);
        }
        if (m_selectedIndex == FILE_REQUESTER_SELECTION_NONE && m_fileCount > 0) {
            m_selectedIndex = 0;
        }
        SetOK(1);

        broadcastMessage.payload.widget.command = 4;
        broadcastMessage.payload.widget.id = 0x52;
        if (m_mapHeaders[m_selectedIndex].width == MAP_DIMENSION_SMALL) {
            broadcastMessage.payload.widget.data.value = 0x1a;
        } else if (m_mapHeaders[m_selectedIndex].width == MAP_DIMENSION_MEDIUM) {
            broadcastMessage.payload.widget.data.value = 0x1b;
        } else if (m_mapHeaders[m_selectedIndex].width == MAP_DIMENSION_LARGE) {
            broadcastMessage.payload.widget.data.value = 0x1c;
        } else {
            broadcastMessage.payload.widget.data.value = 0x1d;
        }
        m_window->BroadcastMessage(broadcastMessage);

        broadcastMessage.payload.widget.id = 0x51;
        broadcastMessage.payload.widget.data.value = m_mapHeaders[m_selectedIndex].playerCount + 0x13;
        m_window->BroadcastMessage(broadcastMessage);

        broadcastMessage.payload.widget.id = 0x56;
        broadcastMessage.payload.widget.data.value = m_mapHeaders[m_selectedIndex].victoryCondition + 0x1e;
        m_window->BroadcastMessage(broadcastMessage);

        broadcastMessage.payload.widget.id = 0x57;
        broadcastMessage.payload.widget.data.value = m_mapHeaders[m_selectedIndex].lossCondition + 0x24;
        m_window->BroadcastMessage(broadcastMessage);

        broadcastMessage.payload.widget.command = 3;
        broadcastMessage.payload.widget.data.text = gText;
        sprintf(gText, "%s", m_mapHeaders[m_selectedIndex].name);
        broadcastMessage.payload.widget.id = 0x50;
        m_window->BroadcastMessage(broadcastMessage);

        sprintf(gText, "%s", cDifficulty[m_mapHeaders[m_selectedIndex].difficulty]);
        broadcastMessage.payload.widget.id = 0x54;
        m_window->BroadcastMessage(broadcastMessage);

        sprintf(gText, "%s", m_mapHeaders[m_selectedIndex].description);
        broadcastMessage.payload.widget.id = 0x55;
        m_window->BroadcastMessage(broadcastMessage);
    }

    for (i = 0; iMaxListSize > i; ++i) {
        if (m_topIndex + i >= m_fileCount) {
            broadcastMessage.payload.widget.command = 6;
            broadcastMessage.payload.widget.data.value = 4;
            broadcastMessage.payload.widget.id = i + 0x14;
            m_window->BroadcastMessage(broadcastMessage);
            if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
                broadcastMessage.payload.widget.id = i + 200;
                m_window->BroadcastMessage(broadcastMessage);
                broadcastMessage.payload.widget.id = i + 220;
                m_window->BroadcastMessage(broadcastMessage);
                broadcastMessage.payload.widget.id = i + 240;
                m_window->BroadcastMessage(broadcastMessage);
                broadcastMessage.payload.widget.id = i + 260;
                m_window->BroadcastMessage(broadcastMessage);
            }
        } else {
            broadcastMessage.payload.widget.id = i + 0x14;
            broadcastMessage.payload.widget.command = 5;
            broadcastMessage.payload.widget.data.value = 4;
            m_window->BroadcastMessage(broadcastMessage);

            if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
                broadcastMessage.payload.widget.id = i + 200;
                m_window->BroadcastMessage(broadcastMessage);
                broadcastMessage.payload.widget.id = i + 220;
                m_window->BroadcastMessage(broadcastMessage);
                broadcastMessage.payload.widget.id = i + 240;
                m_window->BroadcastMessage(broadcastMessage);
                broadcastMessage.payload.widget.id = i + 260;
                m_window->BroadcastMessage(broadcastMessage);

                broadcastMessage.payload.widget.command = 4;
                broadcastMessage.payload.widget.id = i + 200;
                if (m_mapHeaders[m_topIndex + i].width == MAP_DIMENSION_SMALL) {
                    broadcastMessage.payload.widget.data.value = 0x1a;
                } else if (m_mapHeaders[m_topIndex + i].width == MAP_DIMENSION_MEDIUM) {
                    broadcastMessage.payload.widget.data.value = 0x1b;
                } else if (m_mapHeaders[m_topIndex + i].width == MAP_DIMENSION_LARGE) {
                    broadcastMessage.payload.widget.data.value = 0x1c;
                } else {
                    broadcastMessage.payload.widget.data.value = 0x1d;
                }
                m_window->BroadcastMessage(broadcastMessage);

                broadcastMessage.payload.widget.id = i + 220;
                broadcastMessage.payload.widget.data.value =
                    m_mapHeaders[m_topIndex + i].playerCount + 0x13;
                m_window->BroadcastMessage(broadcastMessage);

                broadcastMessage.payload.widget.id = i + 240;
                broadcastMessage.payload.widget.data.value =
                    m_mapHeaders[m_topIndex + i].victoryCondition +
                    0x1e;
                m_window->BroadcastMessage(broadcastMessage);

                broadcastMessage.payload.widget.id = i + 260;
                broadcastMessage.payload.widget.data.value =
                    m_mapHeaders[m_topIndex + i].lossCondition + 0x24;
                m_window->BroadcastMessage(broadcastMessage);
            }

            broadcastMessage.payload.widget.command = 3;
            if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
                sprintf(gText, "%s",
                        m_mapHeaders[m_topIndex + i].name);
            } else {
                sprintf(gText, "%s",
                        m_fileNames[m_topIndex + i].text);
            }
            broadcastMessage.payload.widget.data.text = gText;
            broadcastMessage.payload.widget.id = i + 0x14;
            m_window->BroadcastMessage(broadcastMessage);
        }

        broadcastMessage.payload.widget.id = i + 0x14;
        broadcastMessage.payload.widget.command = 8;
        if (m_topIndex + i == m_selectedIndex) {
            broadcastMessage.payload.widget.data.value = 2;
        } else {
            broadcastMessage.payload.widget.data.value = 1;
        }
        m_window->BroadcastMessage(broadcastMessage);
    }

    broadcastMessage.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
    broadcastMessage.payload.widget.command = 5;
    broadcastMessage.payload.widget.data.value = 2;
    m_window->BroadcastMessage(broadcastMessage);
    if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
        broadcastMessage.payload.widget.command = 3;
        if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
            sprintf(gText, "%s", m_mapHeaders[m_selectedIndex].name);
        } else {
            sprintf(gText, "%s", m_fileNames[m_selectedIndex].text);
        }
        broadcastMessage.payload.widget.data.text = gText;
        m_window->BroadcastMessage(broadcastMessage);
    }
    if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_LOAD_GAME ||
        m_mode == FILE_REQUESTER_MAP) {
        broadcastMessage.payload.widget.command = 6;
        broadcastMessage.payload.widget.data.value = 2;
        m_window->BroadcastMessage(broadcastMessage);
    }

    if (m_fileCount <= iMaxListSize) {
        m_scrollKnob->m_y = static_cast<short>(fGutterTravelLength / 2.0f + fGutterMinY);
    } else {
        gutterStepCount =
            fGutterTravelLength / (m_fileCount - iMaxListSize);
        m_scrollKnob->m_y = static_cast<short>(
            m_topIndex * gutterStepCount + fGutterMinY);
    }
    if (drawWindow) {
        m_window->DrawWindow(1, 0, 0x7fff);
    }
}

VA(0x0048fbc4, 0x15b)
char * fileRequester::GetFilename(void)
{
    if (m_mode != FILE_REQUESTER_SAVE_GAME &&
        (m_selectedIndex < 0 || m_fileCount <= m_selectedIndex)) {
        return cFRDummy;
    }

    if (m_selectedIndex == FILE_REQUESTER_SELECTION_NONE) {
        sprintf(gText, "%s%s", m_filename, m_defaultExtension);
    } else if (m_mode == FILE_REQUESTER_LOAD_GAME || m_mode == FILE_REQUESTER_MAP ||
               m_mode == FILE_REQUESTER_MAP_GAME) {
        sprintf(gText, "%s%s", m_fileNames[m_selectedIndex].text,
                m_extensions[m_selectedIndex].text);
    } else {
        sprintf(gText, "%s%s", m_fileNames[m_selectedIndex].text,
                m_defaultExtension);
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
DATA(0x004f8674) FileRequesterMapSizeFilter giMapSizeFilter =
    FILE_REQUESTER_MAP_SIZE_ALL;
DATA(0x004f88c4) char *cFRDummy = "";
DATA(0x0052857c) float fGutterMinY;
DATA(0x00528580) float fGutterTravelLength;
DATA(0x00528584) int iMaxListSize;
