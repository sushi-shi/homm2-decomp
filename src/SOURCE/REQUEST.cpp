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

// @match-note retained/live 95.14%: both enumeration passes,
// allocation, ordered insertion, extensions, and map-header population are
// reconstructed. All 53/53 relocation sites agree. This build uses a 0x658
// frame versus retail's 0x65c; retail places fullName/header/findData at
// -0x65c/-0x530/-0x37c and the 200/350-byte extension/name arrays at
// -0x238/-0x164. The first structural residual is the positive all-map-sizes
// branch and its two retail continuation jumps; spelling that positive arm
// directly lowered the live score to 92.18%. Moving the insertion shift loop
// into the negative comparison retained the current improvement. Revisit CFG
// polarity and identifier slots after SOURCE reaches 95% globally.
VA(0x0048ca91, 0x7c1)
int fileRequester::InitializeFiles(char *directory, char *pattern, int countOnly)
{
    char fullName[FILE_REQUESTER_PATH_SIZE];
    SMapHeader header;
    WIN32_FIND_DATA findData;
    char extension[FILE_REQUESTER_LOCAL_EXTENSION_SIZE];
    char name[FILE_REQUESTER_LOCAL_NAME_SIZE];
    HANDLE findHandle;
    int moreFiles;
    int insertedCount;
    char *dot;
    int insertAt;
    int move;
    int i;

    sprintf(gText, "%s%s", directory, pattern);
    m_fileCount = 0;
    moreFiles = 1;
    findHandle = FindFirstFile(gText, &findData);
    if (findHandle != INVALID_HANDLE_VALUE) {
        while (moreFiles) {
            if (m_mode == FILE_REQUESTER_MAP_GAME) {
                GetMapHeader(findData.cFileName, &header);
                if (header.minHumanPlayers > giNumHumanPlayers ||
                    giNumHumanPlayers > header.maxHumanPlayers ||
                    (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL &&
                     giMapSizes[giMapSizeFilter] != header.width) ||
                    ShowThisMapGame(findData.cFileName) == 0)
                    goto CountNextFile;
            }
            if (m_mode == FILE_REQUESTER_MAP) {
                GetMapHeader(findData.cFileName, &header);
                if ((giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL &&
                     giMapSizes[giMapSizeFilter] != header.width) ||
                    ShowThisMap(findData.cFileName) == 0)
                    goto CountNextFile;
            }
            ++m_fileCount;
CountNextFile:
            moreFiles = FindNextFile(findHandle, &findData);
        }
        FindClose(findHandle);
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

    for (i = 0; i < m_fileCount; ++i) {
        strcpy(m_fileNames[i].text, "");
        strcpy(m_extensions[i].text, "");
    }

    insertedCount = 0;
    sprintf(gText, "%s%s", directory, pattern);
    findHandle = FindFirstFile(gText, &findData);
    if (findHandle != INVALID_HANDLE_VALUE) {
        moreFiles = 1;
        while (moreFiles) {
            if (m_mode == FILE_REQUESTER_MAP_GAME) {
                GetMapHeader(findData.cFileName, &header);
                if (header.minHumanPlayers > giNumHumanPlayers ||
                    giNumHumanPlayers > header.maxHumanPlayers ||
                    (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL &&
                     giMapSizes[giMapSizeFilter] != header.width) ||
                    ShowThisMapGame(findData.cFileName) == 0)
                    goto InsertNextFile;
            }
            if (m_mode == FILE_REQUESTER_MAP) {
                GetMapHeader(findData.cFileName, &header);
                if ((giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL &&
                     giMapSizes[giMapSizeFilter] != header.width) ||
                    ShowThisMap(findData.cFileName) == 0)
                    goto InsertNextFile;
            }

            strcpy(name, findData.cFileName);
            dot = FindLastToken(name, '.');
            if (dot != 0) {
                strcpy(extension, dot);
                *dot = 0;
            }

            insertAt = 0;
            while (insertAt < insertedCount) {
                if (_strcmpi(name, m_fileNames[insertAt].text) < 0) {
                    for (move = insertedCount; move > insertAt; --move) {
                        strcpy(m_fileNames[move].text,
                               m_fileNames[move - 1].text);
                        strcpy(m_extensions[move].text,
                               m_extensions[move - 1].text);
                    }
                    break;
                }
                ++insertAt;
            }
            strcpy(m_fileNames[insertAt].text, name);
            strcpy(m_extensions[insertAt].text, extension);
            ++insertedCount;
InsertNextFile:
            moreFiles = FindNextFile(findHandle, &findData);
        }
        FindClose(findHandle);
    }

    if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
        for (i = 0; i < insertedCount; ++i) {
            sprintf(fullName, "%s%s", m_fileNames[i].text, m_extensions[i].text);
            GetMapHeader(fullName, &m_mapHeaders[i]);
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

// @match-note 99.67%: the complete dialog/widget construction and all 47/47
// external relocations agree. Remaining differences are pooled string-symbol
// identities plus a 0x48 frame in this build versus retail's 0x44 frame. Do not
// perturb this predecessor before global SOURCE 95%; revisit local slot naming
// once earlier REQUEST TU state settles.
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
        for (int i = 0; i < m_fileCount; ++i) {
            if (_strcmpi(m_fileNames[i].text, m_filename) == 0) {
                m_selectedIndex = i;
            }
        }
    } else {
        okEnabled = 0;
        if (m_mode == FILE_REQUESTER_MAP_GAME) {
            char mapName[12];
            int i = 0;
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
    strcpy(name, "fileRequester");
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

// @match-note retained 92.76%, combined live 92.70%: complete key, click,
// hover/help, map-filter, filename,
// scroll-knob, list-selection, saved-game validation, and executive-exit
// behavior is reconstructed with typed tag_message views. Retail uses a 0x420
// frame and 0x11ae bytes; this build uses 0x430 and 0x10de. Retail's stable
// slots include this/accept/item at -0x3fc/-0x16c/-0x170, the two 300-byte
// oldName arrays at -0x2a0/-0x3f4, helpIndex at -0x2a4, and switch temporaries
// at -0x400 through -0x414. The first normalized structural residual is the
// key-up selected/top comparison polarity near instruction 100. The helper's
// relocation view stops at a delinked jump-table label. An explicit
// 0x1190..0x2270 / 0x11dc..0x238c object-range audit finds all 76/76 external
// relocation identities and counts agree; only local jump-table and pooled
// string identities differ. Moving click behind hover/help lowered the score to
// 88.20%; retail relocation order for the active widget bodies retained this
// result. Revisit switch body placement and local slots after global SOURCE 95%.
VA(0x0048daec, 0x11ae)
int fileRequester::Main(struct tag_message &message)
{
    int accept = 0;

    switch (message.type) {
    case FILE_REQUESTER_EVENT_KEY:
        switch (message.payload.keyboard.keyCode) {
        case FILE_REQUESTER_KEY_CYCLE_FILTER: {
            char oldName[FILE_REQUESTER_PATH_SIZE];
            if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
                strcpy(oldName, m_fileNames[m_selectedIndex].text);
            } else {
                strcpy(oldName, "");
            }
            giMapSizeFilter = static_cast<FileRequesterMapSizeFilter>(
                (giMapSizeFilter + 1) % FILE_REQUESTER_MAP_SIZE_COUNT);
            SetupFiles();
            if (strlen(oldName) != 0) {
                for (int i = 0; i < m_fileCount; ++i) {
                    if (_strcmpi(m_fileNames[i].text, oldName) == 0) {
                        m_selectedIndex = i;
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
                        accept = 1;
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
                        accept = 1;
                    }
                }
                break;
        case FILE_REQUESTER_WIDGET_HOVER:
        case FILE_REQUESTER_WIDGET_HELP:
                if ((message.payload.widget.parameter &
                     FILE_REQUESTER_HELP_MODIFIER) != 0) {
                    int helpIndex = FILE_REQUESTER_HELP_NONE;
                    switch (message.payload.widget.id) {
                    case FILE_REQUESTER_FILENAME_ENTRY:
                        helpIndex = FILE_REQUESTER_HELP_FILENAME;
                        break;
                    case FILE_REQUESTER_MAP_NAME:
                        helpIndex = FILE_REQUESTER_HELP_MAP_NAME;
                        break;
                    case FILE_REQUESTER_MAP_PLAYER_COUNT:
                        helpIndex = FILE_REQUESTER_HELP_PLAYER_COUNT;
                        break;
                    case FILE_REQUESTER_MAP_SIZE:
                        helpIndex = FILE_REQUESTER_HELP_MAP_SIZE;
                        break;
                    case FILE_REQUESTER_MAP_DIFFICULTY_ICON:
                    case FILE_REQUESTER_MAP_DIFFICULTY_TEXT:
                        helpIndex = FILE_REQUESTER_HELP_DIFFICULTY;
                        break;
                    case FILE_REQUESTER_MAP_DESCRIPTION:
                        helpIndex = FILE_REQUESTER_HELP_DESCRIPTION;
                        break;
                    case FILE_REQUESTER_MAP_VICTORY:
                        helpIndex = FILE_REQUESTER_HELP_VICTORY;
                        break;
                    case FILE_REQUESTER_MAP_LOSS:
                        helpIndex = FILE_REQUESTER_HELP_LOSS;
                        break;
                    case FILE_REQUESTER_FILTER_SMALL:
                        helpIndex = FILE_REQUESTER_HELP_FILTER_SMALL;
                        break;
                    case FILE_REQUESTER_FILTER_MEDIUM:
                        helpIndex = FILE_REQUESTER_HELP_FILTER_MEDIUM;
                        break;
                    case FILE_REQUESTER_FILTER_LARGE:
                        helpIndex = FILE_REQUESTER_HELP_FILTER_LARGE;
                        break;
                    case FILE_REQUESTER_FILTER_XLARGE:
                        helpIndex = FILE_REQUESTER_HELP_FILTER_XLARGE;
                        break;
                    case FILE_REQUESTER_FILTER_ALL:
                        helpIndex = FILE_REQUESTER_HELP_FILTER_ALL;
                        break;
                    case FILE_REQUESTER_CANCEL:
                        helpIndex = FILE_REQUESTER_HELP_CANCEL;
                        break;
                    case FILE_REQUESTER_OK:
                        helpIndex = FILE_REQUESTER_HELP_OK;
                        break;
                    default:
                        if (message.payload.widget.id >=
                                FILE_REQUESTER_MAP_SIZE_ICON_FIRST &&
                            message.payload.widget.id <
                                FILE_REQUESTER_MAP_SIZE_ICON_FIRST +
                                    FILE_REQUESTER_LIST_RANGE_SIZE) {
                            helpIndex = FILE_REQUESTER_HELP_MAP_SIZE;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_MAP_PLAYER_ICON_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_MAP_PLAYER_ICON_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            helpIndex = FILE_REQUESTER_HELP_PLAYER_COUNT;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_MAP_VICTORY_ICON_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_MAP_VICTORY_ICON_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            helpIndex = FILE_REQUESTER_HELP_VICTORY;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_MAP_LOSS_ICON_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_MAP_LOSS_ICON_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            helpIndex = FILE_REQUESTER_HELP_LOSS;
                        }
                        break;
                    }
                    if (helpIndex >= 0) {
                        NormalDialog(
                            gFileRequestHelp[helpIndex],
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
                        FileRequesterMapSizeFilter filter =
                            static_cast<FileRequesterMapSizeFilter>(
                                message.payload.widget.id - FILE_REQUESTER_FILTER_SMALL);
                        if (!MapExistsForFilter(filter)) {
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
                            giMapSizeFilter = filter;
                            char oldName[FILE_REQUESTER_PATH_SIZE];
                            if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
                                strcpy(oldName, m_fileNames[m_selectedIndex].text);
                            } else {
                                strcpy(oldName, "");
                            }
                            SetupFiles();
                            if (strlen(oldName) != 0) {
                                for (int i = 0; i < m_fileCount; ++i) {
                                    if (_strcmpi(m_fileNames[i].text, oldName) == 0) {
                                        m_selectedIndex = i;
                                    }
                                }
                            }
                            Update(1);
                        }
                        break;
                    }
                    case FILE_REQUESTER_FILENAME_ENTRY: {
                        tag_message textMessage;
                        textMessage.type = FILE_REQUESTER_EVENT_WIDGET;
                        textMessage.payload.widget.command =
                            FILE_REQUESTER_WIDGET_GET_TEXT;
                        textMessage.payload.widget.id =
                            FILE_REQUESTER_FILENAME_ENTRY;
                        m_window->BroadcastMessage(textMessage);

                        char newName[FILE_REQUESTER_LOCAL_NAME_SIZE];
                        memset(newName, 0,
                               FILE_REQUESTER_FILENAME_INITIAL_CLEAR_SIZE);
                        strcpy(newName, textMessage.payload.widget.data.text);
                        int length = strlen(newName);
                        int i;
                        for (i = 0; i < length; ++i) {
                            char ch = newName[i];
                            if (!((ch >= 'A' && ch <= 'Z') ||
                                  (ch >= 'a' && ch <= 'z') ||
                                  (ch >= '0' && ch <= '9') || ch == '_' ||
                                  ch == ' ' ||
                                  FindToken("$%'-_@~`!(){}^#&+,;=[].", ch) !=
                                      0)) {
                                newName[i] = 0;
                            }
                        }
                        for (i = strlen(newName) - 1; i >= 0; --i) {
                            if (newName[i] == ' ')
                                newName[i] = 0;
                            else
                                i = -1;
                        }
                        if (strlen(newName) != 0 && newName[0] > ' ') {
                            m_selectedIndex = FILE_REQUESTER_SELECTION_NONE;
                            strcpy(m_filename, newName);
                            SetOK(1);
                        }
                        textMessage.payload.widget.command =
                            FILE_REQUESTER_WIDGET_SET_TEXT;
                        textMessage.payload.widget.id =
                            FILE_REQUESTER_FILENAME_ENTRY;
                        textMessage.payload.widget.data.text = m_filename;
                        m_window->BroadcastMessage(textMessage);
                        Update(1);
                        break;
                    }
                    case FILE_REQUESTER_SCROLL_KNOB: {
                        int positions = m_fileCount - (iMaxListSize - 1);
                        if (positions < 1)
                            positions = 1;
                        int gutterStep = static_cast<int>(
                            (fGutterTravelLength *
                             FILE_REQUESTER_GUTTER_SCALE) /
                            positions);
                        int mouseX = message.payload.widget.screenX;
                        int mouseY = message.payload.widget.screenY;
                        mouseY = static_cast<int>(
                                     mouseY - (m_y + fGutterMinY)) -
                                 FILE_REQUESTER_SCROLL_KNOB_HALF_HEIGHT;
                        m_topIndex =
                            (mouseY * FILE_REQUESTER_GUTTER_SCALE) /
                            gutterStep;
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
                        int item;
                        if (message.payload.widget.id >=
                                FILE_REQUESTER_MAP_SIZE_ICON_FIRST &&
                            message.payload.widget.id <
                                FILE_REQUESTER_MAP_SIZE_ICON_FIRST +
                                    FILE_REQUESTER_LIST_RANGE_SIZE) {
                            item = message.payload.widget.id -
                                   FILE_REQUESTER_MAP_SIZE_ICON_FIRST;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_MAP_PLAYER_ICON_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_MAP_PLAYER_ICON_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            item = message.payload.widget.id -
                                   FILE_REQUESTER_MAP_PLAYER_ICON_FIRST;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_MAP_VICTORY_ICON_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_MAP_VICTORY_ICON_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            item = message.payload.widget.id -
                                   FILE_REQUESTER_MAP_VICTORY_ICON_FIRST;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_MAP_LOSS_ICON_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_MAP_LOSS_ICON_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            item = message.payload.widget.id -
                                   FILE_REQUESTER_MAP_LOSS_ICON_FIRST;
                        } else if (message.payload.widget.id >=
                                       FILE_REQUESTER_LIST_TEXT_FIRST &&
                                   message.payload.widget.id <
                                       FILE_REQUESTER_LIST_TEXT_FIRST +
                                           FILE_REQUESTER_LIST_RANGE_SIZE) {
                            item = message.payload.widget.id -
                                   FILE_REQUESTER_LIST_TEXT_FIRST;
                        } else {
                            break;
                        }
                        if (item + m_topIndex == m_selectedIndex) {
                            message.payload.widget.data.value =
                                FILE_REQUESTER_OK;
                            message.payload.widget.id = FILE_REQUESTER_OK;
                            accept = 1;
                        } else if (item + m_topIndex < m_fileCount) {
                            m_selectedIndex = item + m_topIndex;
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

    if (accept == 1) {
        if (m_mode == FILE_REQUESTER_LOAD_GAME && m_selectedIndex >= 0 &&
            message.payload.widget.data.value != FILE_REQUESTER_CANCEL &&
            _strcmpi(m_extensions[m_selectedIndex].text, ".GMC") != 0 &&
            _strcmpi(m_extensions[m_selectedIndex].text, ".GXC") != 0) {
            int humans =
                m_extensions[m_selectedIndex]
                    .text[FILE_REQUESTER_EXTENSION_PLAYER_DIGIT] -
                '0';
            if (humans < giNumHumanPlayers &&
                giDebugLevel < FILE_REQUESTER_DEBUG_ALLOW_PLAYER_MISMATCH) {
                sprintf(gText,
                        "The game you have chosen only has slots for %d human(s).  You need one with room for at least %d humans.",
                        humans, giNumHumanPlayers);
                NormalDialog(gText, NORMAL_DIALOG_INFO,
                             NORMAL_DIALOG_NO_RESOURCE,
                             NORMAL_DIALOG_NO_VALUE,
                             NORMAL_DIALOG_NO_RESOURCE, 0,
                             NORMAL_DIALOG_NO_RESOURCE, 0,
                             NORMAL_DIALOG_NO_RESOURCE, 0);
                accept = 0;
            }
            if (humans > giNumHumanPlayers) {
                sprintf(gText,
                        "The game you have chosen was being played with %d humans. Is it OK if the computer takes the place of the last %d human(s)?",
                        humans, humans - giNumHumanPlayers);
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM,
                             NORMAL_DIALOG_NO_RESOURCE,
                             NORMAL_DIALOG_NO_VALUE,
                             NORMAL_DIALOG_NO_RESOURCE, 0,
                             NORMAL_DIALOG_NO_RESOURCE, 0,
                             NORMAL_DIALOG_NO_RESOURCE, 0);
                if (gpWindowManager->m_dialogResult !=
                    NORMAL_DIALOG_BUTTON_FIVE) {
                    accept = 0;
                }
            }
        }
        if (accept != 0) {
            message.type = FILE_REQUESTER_EVENT_EXIT;
            message.payload.executive.command =
                FILE_REQUESTER_EXECUTIVE_CLOSE;
            return FILE_REQUESTER_MAIN_CLOSE;
        }
    }
    return FILE_REQUESTER_MAIN_CONTINUE;
}

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

VA(0x0048ef82, 0xc42)
void fileRequester::Update(int drawWindow)
{
    tag_message message;
    message.type = 0x200;
    message.payload.widget.data.value = 0;

    int i;
    if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
        for (i = 0; i < FILE_REQUESTER_MAP_SIZE_COUNT; ++i) {
            message.payload.widget.command = 4;
            message.payload.widget.id = FILE_REQUESTER_FILTER_SMALL + i;
            message.payload.widget.data.value = (giMapSizeFilter == i) + i * 2 + 9;
            m_window->BroadcastMessage(message);
        }
        if (m_selectedIndex == FILE_REQUESTER_SELECTION_NONE && m_fileCount > 0) {
            m_selectedIndex = 0;
        }
        SetOK(1);

        message.payload.widget.command = 4;
        message.payload.widget.id = 0x52;
        if (m_mapHeaders[m_selectedIndex].width == MAP_DIMENSION_SMALL) {
            message.payload.widget.data.value = 0x1a;
        } else if (m_mapHeaders[m_selectedIndex].width == MAP_DIMENSION_MEDIUM) {
            message.payload.widget.data.value = 0x1b;
        } else if (m_mapHeaders[m_selectedIndex].width == MAP_DIMENSION_LARGE) {
            message.payload.widget.data.value = 0x1c;
        } else {
            message.payload.widget.data.value = 0x1d;
        }
        m_window->BroadcastMessage(message);

        message.payload.widget.id = 0x51;
        message.payload.widget.data.value = m_mapHeaders[m_selectedIndex].playerCount + 0x13;
        m_window->BroadcastMessage(message);

        message.payload.widget.id = 0x56;
        message.payload.widget.data.value = m_mapHeaders[m_selectedIndex].victoryCondition + 0x1e;
        m_window->BroadcastMessage(message);

        message.payload.widget.id = 0x57;
        message.payload.widget.data.value = m_mapHeaders[m_selectedIndex].lossCondition + 0x24;
        m_window->BroadcastMessage(message);

        message.payload.widget.command = 3;
        message.payload.widget.data.text = gText;
        sprintf(gText, "%s", m_mapHeaders[m_selectedIndex].name);
        message.payload.widget.id = 0x50;
        m_window->BroadcastMessage(message);

        sprintf(gText, "%s", cDifficulty[m_mapHeaders[m_selectedIndex].difficulty]);
        message.payload.widget.id = 0x54;
        m_window->BroadcastMessage(message);

        sprintf(gText, "%s", m_mapHeaders[m_selectedIndex].description);
        message.payload.widget.id = 0x55;
        m_window->BroadcastMessage(message);
    }

    for (i = 0; iMaxListSize > i; ++i) {
        if (m_topIndex + i >= m_fileCount) {
            message.payload.widget.command = 6;
            message.payload.widget.data.value = 4;
            message.payload.widget.id = i + 0x14;
            m_window->BroadcastMessage(message);
            if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
                message.payload.widget.id = i + 200;
                m_window->BroadcastMessage(message);
                message.payload.widget.id = i + 220;
                m_window->BroadcastMessage(message);
                message.payload.widget.id = i + 240;
                m_window->BroadcastMessage(message);
                message.payload.widget.id = i + 260;
                m_window->BroadcastMessage(message);
            }
        } else {
            message.payload.widget.id = i + 0x14;
            message.payload.widget.command = 5;
            message.payload.widget.data.value = 4;
            m_window->BroadcastMessage(message);

            if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
                message.payload.widget.id = i + 200;
                m_window->BroadcastMessage(message);
                message.payload.widget.id = i + 220;
                m_window->BroadcastMessage(message);
                message.payload.widget.id = i + 240;
                m_window->BroadcastMessage(message);
                message.payload.widget.id = i + 260;
                m_window->BroadcastMessage(message);

                message.payload.widget.command = 4;
                message.payload.widget.id = i + 200;
                if (m_mapHeaders[m_topIndex + i].width == MAP_DIMENSION_SMALL) {
                    message.payload.widget.data.value = 0x1a;
                } else if (m_mapHeaders[m_topIndex + i].width == MAP_DIMENSION_MEDIUM) {
                    message.payload.widget.data.value = 0x1b;
                } else if (m_mapHeaders[m_topIndex + i].width == MAP_DIMENSION_LARGE) {
                    message.payload.widget.data.value = 0x1c;
                } else {
                    message.payload.widget.data.value = 0x1d;
                }
                m_window->BroadcastMessage(message);

                message.payload.widget.id = i + 220;
                message.payload.widget.data.value = m_mapHeaders[m_topIndex + i].playerCount + 0x13;
                m_window->BroadcastMessage(message);

                message.payload.widget.id = i + 240;
                message.payload.widget.data.value = m_mapHeaders[m_topIndex + i].victoryCondition + 0x1e;
                m_window->BroadcastMessage(message);

                message.payload.widget.id = i + 260;
                message.payload.widget.data.value = m_mapHeaders[m_topIndex + i].lossCondition + 0x24;
                m_window->BroadcastMessage(message);
            }

            message.payload.widget.command = 3;
            if (m_mode == FILE_REQUESTER_MAP || m_mode == FILE_REQUESTER_MAP_GAME) {
                sprintf(gText, "%s", m_mapHeaders[m_topIndex + i].name);
            } else {
                sprintf(gText, "%s", m_fileNames[m_topIndex + i].text);
            }
            message.payload.widget.data.text = gText;
            message.payload.widget.id = i + 0x14;
            m_window->BroadcastMessage(message);
        }

        message.payload.widget.id = i + 0x14;
        message.payload.widget.command = 8;
        if (m_topIndex + i == m_selectedIndex) {
            message.payload.widget.data.value = 2;
        } else {
            message.payload.widget.data.value = 1;
        }
        m_window->BroadcastMessage(message);
    }

    message.payload.widget.id = FILE_REQUESTER_FILENAME_ENTRY;
    message.payload.widget.command = 5;
    message.payload.widget.data.value = 2;
    m_window->BroadcastMessage(message);
    if (m_selectedIndex != FILE_REQUESTER_SELECTION_NONE) {
        message.payload.widget.command = 3;
        if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
            sprintf(gText, "%s", m_mapHeaders[m_selectedIndex].name);
        } else {
            sprintf(gText, "%s", m_fileNames[m_selectedIndex].text);
        }
        message.payload.widget.data.text = gText;
        m_window->BroadcastMessage(message);
    }
    if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_LOAD_GAME ||
        m_mode == FILE_REQUESTER_MAP) {
        message.payload.widget.command = 6;
        message.payload.widget.data.value = 2;
        m_window->BroadcastMessage(message);
    }

    if (m_fileCount <= iMaxListSize) {
        m_scrollKnob->m_y = static_cast<short>(fGutterTravelLength / 2.0f + fGutterMinY);
    } else {
        double gutterStep = fGutterTravelLength / (m_fileCount - iMaxListSize);
        m_scrollKnob->m_y = static_cast<short>(m_topIndex * gutterStep + fGutterMinY);
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

// ---- globals (definitions, RVA order) ----
DATA(0x004f8674) FileRequesterMapSizeFilter giMapSizeFilter;
DATA(0x004f88c4) char *cFRDummy = "";
DATA(0x0052857c) float fGutterMinY;
DATA(0x00528580) float fGutterTravelLength;
DATA(0x00528584) int iMaxListSize;
