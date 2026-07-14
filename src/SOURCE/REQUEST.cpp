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

VA(0x0048ca91, 0x7c1)
int fileRequester::InitializeFiles(char *directory, char *pattern, int countOnly)
{
    char fullName[FILE_REQUESTER_PATH_SIZE];
    SMapHeader header;
    WIN32_FIND_DATA findData;
    HANDLE findHandle;
    int moreFiles;

    sprintf(gText, "%s%s", directory, pattern);
    m_fileCount = 0;
    moreFiles = 1;
    findHandle = FindFirstFile(gText, &findData);
    if (findHandle != INVALID_HANDLE_VALUE) {
        while (moreFiles) {
            if ((m_mode != FILE_REQUESTER_MAP_GAME ||
                 (GetMapHeader(findData.cFileName, &header),
                  header.minHumanPlayers <= giNumHumanPlayers &&
                  giNumHumanPlayers <= header.maxHumanPlayers &&
                  (giMapSizeFilter == FILE_REQUESTER_MAP_SIZE_ALL ||
                   giMapSizes[giMapSizeFilter] == header.mapSize) &&
                  ShowThisMapGame(findData.cFileName))) &&
                (m_mode != FILE_REQUESTER_MAP ||
                 (GetMapHeader(findData.cFileName, &header),
                  (giMapSizeFilter == FILE_REQUESTER_MAP_SIZE_ALL ||
                   giMapSizes[giMapSizeFilter] == header.mapSize) &&
                  ShowThisMap(findData.cFileName)))) {
                ++m_fileCount;
            }
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

    for (int i = 0; i < m_fileCount; ++i) {
        strcpy(m_fileNames[i].text, "");
        strcpy(m_extensions[i].text, "");
    }

    int insertedCount = 0;
    sprintf(gText, "%s%s", directory, pattern);
    findHandle = FindFirstFile(gText, &findData);
    if (findHandle != INVALID_HANDLE_VALUE) {
        moreFiles = 1;
        while (moreFiles) {
            if ((m_mode != FILE_REQUESTER_MAP_GAME ||
                 (GetMapHeader(findData.cFileName, &header),
                  header.minHumanPlayers <= giNumHumanPlayers &&
                  giNumHumanPlayers <= header.maxHumanPlayers &&
                  (giMapSizeFilter == 4 || giMapSizes[giMapSizeFilter] == header.mapSize) &&
                  ShowThisMapGame(findData.cFileName))) &&
                (m_mode != FILE_REQUESTER_MAP ||
                 (GetMapHeader(findData.cFileName, &header),
                  (giMapSizeFilter == 4 || giMapSizes[giMapSizeFilter] == header.mapSize) &&
                  ShowThisMap(findData.cFileName)))) {
                char name[FILE_REQUESTER_NAME_SIZE];
                char extension[FILE_REQUESTER_EXTENSION_SIZE];
                strcpy(name, findData.cFileName);
                char *dot = FindLastToken(name, '.');
                if (dot != 0) {
                    strcpy(extension, dot);
                    *dot = 0;
                }

                int insertAt = 0;
                while (insertAt < insertedCount &&
                       _strcmpi(name, m_fileNames[insertAt].text) >= 0) {
                    ++insertAt;
                }
                for (int move = insertedCount; move > insertAt; --move) {
                    strcpy(m_fileNames[move].text, m_fileNames[move - 1].text);
                    strcpy(m_extensions[move].text, m_extensions[move - 1].text);
                }
                strcpy(m_fileNames[insertAt].text, name);
                strcpy(m_extensions[insertAt].text, extension);
                ++insertedCount;
            }
            moreFiles = FindNextFile(findHandle, &findData);
        }
        FindClose(findHandle);
    }

    if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
        for (int i = 0; i < insertedCount; ++i) {
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
    m_selectedIndex = -1;
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
int fileRequester::MapExistsForFilter(int filter)
{
    int oldFilter = giMapSizeFilter;
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
    m_selectedIndex = -1;
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
    message.field4 = 3;
    unsigned char okEnabled;
    if (m_mode == FILE_REQUESTER_SAVE_GAME) {
        okEnabled = 1;
        strcpy(m_filename, gpGame->m_saveName);
        char *dot = FindLastToken(m_filename, '.');
        if (dot != 0) {
            *dot = 0;
        }
        message.field8 = 15;
        message.text = m_filename;
        m_window->BroadcastMessage(message);
        message.field8 = 16;
        sprintf(gText, "File to Save:");
        message.text = gText;
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
        message.field8 = 16;
        sprintf(gText, "File to Load:");
        message.text = gText;
        m_window->BroadcastMessage(message);
    }

    message.type = 0x200;
    message.field4 = 0x33;
    message.field8 = 15;
    message.field18 = 201;
    m_window->BroadcastMessage(message);
    Update(0);
    if (m_selectedIndex != -1) {
        okEnabled = 1;
    }
    gpWindowManager->AddWindow(m_window, -1, 1);
    if (m_fileCount == 0) {
        okEnabled = 0;
    }
    if (m_mode == FILE_REQUESTER_SAVE_GAME && _strcmpi(m_filename, "NEWGAME") == 0 &&
        m_selectedIndex == -1) {
        okEnabled = 1;
    }
    SetOK(okEnabled);
    field_0xc = 0x4000;
    field_0x10 = id;
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
        message.field4 = 6;
    } else {
        message.field4 = 5;
    }
    message.field8 = 0x7802;
    if (m_active == 1) {
        message.field18 = 8;
    } else {
        message.field18 = 0x1000;
    }
    m_window->BroadcastMessage(message);
    if (enabled) {
        message.field4 = 5;
    } else {
        message.field4 = 6;
    }
    message.field18 = 2;
    m_window->BroadcastMessage(message);
}

VA(0x0048daec, 0x11ae)
int fileRequester::Main(struct tag_message &message)
{
    int accept = 0;

    switch (message.type) {
    case 1:
        switch (message.field4) {
        case 0x40: {
            char oldName[300];
            if (m_selectedIndex == -1) {
                strcpy(oldName, "");
            }
            else {
                strcpy(oldName, m_fileNames[m_selectedIndex].text);
            }
            giMapSizeFilter = (giMapSizeFilter + 1) % FILE_REQUESTER_MAP_SIZE_COUNT;
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
        case 0x48:
            if (m_selectedIndex > 0) {
                --m_selectedIndex;
                if (m_selectedIndex < m_topIndex) {
                    --m_topIndex;
                }
                Update(1);
            }
            break;
        case 0x50:
            if (m_selectedIndex < m_fileCount - 1) {
                ++m_selectedIndex;
                if (m_selectedIndex >= m_topIndex + iMaxListSize) {
                    ++m_topIndex;
                }
                Update(1);
            }
            break;
        }
        break;
    case 0x200:
        switch (message.field4) {
        case 13:
                if (message.field8 < FILE_REQUESTER_OK) {
                    if (message.field8 == FILE_REQUESTER_CANCEL) {
                        message.field18 = message.field8;
                        accept = 1;
                    } else if (message.field8 == 1) {
                        if (m_topIndex > 0) {
                            --m_topIndex;
                            Update(1);
                        }
                    } else if (message.field8 == 2 &&
                               m_topIndex + iMaxListSize < m_fileCount) {
                        ++m_topIndex;
                        if (m_topIndex + iMaxListSize - 1 >= m_fileCount) {
                            m_topIndex = m_fileCount - iMaxListSize;
                        }
                        Update(1);
                    }
                } else if (message.field8 == FILE_REQUESTER_OK) {
                    if (m_selectedIndex == -1 && m_filename[0] == 0) {
                        NormalDialog("Please make a selection from the list, or press cancel.",
                                     1, -1, -1, -1, 0, -1, 0, -1, 0);
                    } else {
                        message.field18 = message.field8;
                        accept = 1;
                    }
                }
                break;
        case 12:
        case 14:
                if ((message.fieldC & 2) == 0) {
                    switch (message.field8) {
                    case FILE_REQUESTER_SCROLL_KNOB: {
                        int positions = m_fileCount - (iMaxListSize - 1);
                        if (positions < 1) {
                            positions = 1;
                        }
                        int gutterStep = static_cast<int>((fGutterTravelLength * 100.0f) /
                                                          positions);
                        int mouseX = message.field10;
                        int mouseY = message.field14;
                        mouseY = static_cast<int>(mouseY - (m_y + fGutterMinY)) - 9;
                        m_topIndex = (mouseY * 100) / gutterStep;
                        if (m_topIndex + iMaxListSize - 1 >= m_fileCount) {
                            m_topIndex = m_fileCount - iMaxListSize;
                        }
                        if (m_topIndex < 0) {
                            m_topIndex = 0;
                        }
                        Update(1);
                        break;
                    }
                    case 14:
                        DoKnob();
                        break;
                    case FILE_REQUESTER_FILENAME_ENTRY: {
                        tag_message textMessage;
                        textMessage.type = 0x200;
                        textMessage.field4 = 7;
                        textMessage.field8 = FILE_REQUESTER_FILENAME_ENTRY;
                        m_window->BroadcastMessage(textMessage);

                        char newName[352];
                        memset(newName, 0, 9);
                        strcpy(newName, textMessage.text);
                        int length = strlen(newName);
                        int i;
                        for (i = 0; i < length; ++i) {
                            char ch = newName[i];
                            if (!((ch >= 'A' && ch <= 'Z') ||
                                  (ch >= 'a' && ch <= 'z') ||
                                  (ch >= '0' && ch <= '9') || ch == '_' || ch == ' ' ||
                                  FindToken("$%'-_@~`!(){}^#&+,;=[].", ch) != 0)) {
                                newName[i] = 0;
                            }
                        }
                        for (i = strlen(newName) - 1; i >= 0; --i) {
                            if (newName[i] == ' ') {
                                newName[i] = 0;
                            } else {
                                i = -1;
                            }
                        }
                        if (strlen(newName) != 0 && newName[0] > ' ') {
                            m_selectedIndex = -1;
                            strcpy(m_filename, newName);
                            SetOK(1);
                        }
                        textMessage.field4 = 3;
                        textMessage.field8 = FILE_REQUESTER_FILENAME_ENTRY;
                        textMessage.text = m_filename;
                        m_window->BroadcastMessage(textMessage);
                        Update(1);
                        break;
                    }
                    case FILE_REQUESTER_FILTER_SMALL:
                    case FILE_REQUESTER_FILTER_MEDIUM:
                    case FILE_REQUESTER_FILTER_LARGE:
                    case FILE_REQUESTER_FILTER_XLARGE:
                    case FILE_REQUESTER_FILTER_ALL: {
                        int filter = message.field8 - FILE_REQUESTER_FILTER_SMALL;
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
                            NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                        } else {
                            giMapSizeFilter = filter;
                            char oldName[300];
                            if (m_selectedIndex == -1) {
                                strcpy(oldName, "");
                            } else {
                                strcpy(oldName, m_fileNames[m_selectedIndex].text);
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
                    default: {
                        int item;
                        if (message.field8 >= 200 && message.field8 <= 219) {
                            item = message.field8 - 200;
                        } else if (message.field8 >= 220 && message.field8 <= 239) {
                            item = message.field8 - 220;
                        } else if (message.field8 >= 240 && message.field8 <= 259) {
                            item = message.field8 - 240;
                        } else if (message.field8 >= 260 && message.field8 <= 279) {
                            item = message.field8 - 260;
                        } else if (message.field8 >= 20 && message.field8 <= 39) {
                            item = message.field8 - 20;
                        } else {
                            break;
                        }
                        if (item + m_topIndex == m_selectedIndex) {
                            message.field18 = FILE_REQUESTER_OK;
                            message.field8 = FILE_REQUESTER_OK;
                            accept = 1;
                        } else if (item + m_topIndex < m_fileCount) {
                            m_selectedIndex = item + m_topIndex;
                            SetOK(1);
                            Update(1);
                        }
                        break;
                    }
                    }
                } else {
                    int helpIndex = -1;
                    switch (message.field8) {
                    case FILE_REQUESTER_FILENAME_ENTRY: helpIndex = 5; break;
                    case 0x50: helpIndex = 12; break;
                    case 0x51: helpIndex = 9; break;
                    case 0x52: helpIndex = 8; break;
                    case 0x53:
                    case 0x54: helpIndex = 14; break;
                    case 0x55: helpIndex = 13; break;
                    case 0x56: helpIndex = 10; break;
                    case 0x57: helpIndex = 11; break;
                    case FILE_REQUESTER_FILTER_SMALL: helpIndex = 0; break;
                    case FILE_REQUESTER_FILTER_MEDIUM: helpIndex = 1; break;
                    case FILE_REQUESTER_FILTER_LARGE: helpIndex = 2; break;
                    case FILE_REQUESTER_FILTER_XLARGE: helpIndex = 3; break;
                    case FILE_REQUESTER_FILTER_ALL: helpIndex = 4; break;
                    case FILE_REQUESTER_CANCEL: helpIndex = 7; break;
                    case FILE_REQUESTER_OK: helpIndex = 6; break;
                    default:
                        if (message.field8 >= 200 && message.field8 <= 219) {
                            helpIndex = 8;
                        } else if (message.field8 >= 220 && message.field8 <= 239) {
                            helpIndex = 9;
                        } else if (message.field8 >= 240 && message.field8 <= 259) {
                            helpIndex = 10;
                        } else if (message.field8 >= 260 && message.field8 <= 279) {
                            helpIndex = 11;
                        }
                        break;
                    }
                    if (helpIndex >= 0) {
                        NormalDialog(gFileRequestHelp[helpIndex], 4, -1, -1, -1,
                                     0, -1, 0, -1, 0);
                    }
                }
                break;
        }
        break;
    }

    if (accept == 1) {
        if (m_mode == FILE_REQUESTER_LOAD_GAME && m_selectedIndex >= 0 &&
            message.field18 != FILE_REQUESTER_CANCEL &&
            _strcmpi(m_extensions[m_selectedIndex].text, ".GMC") != 0 &&
            _strcmpi(m_extensions[m_selectedIndex].text, ".GXC") != 0) {
            int humans = m_extensions[m_selectedIndex].text[3] - '0';
            if (humans < giNumHumanPlayers && giDebugLevel < 2) {
                sprintf(gText,
                        "The game you have chosen only has slots for %d human(s).  You need one with room for at least %d humans.",
                        humans, giNumHumanPlayers);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                accept = 0;
            }
            if (humans > giNumHumanPlayers) {
                sprintf(gText,
                        "The game you have chosen was being played with %d humans. Is it OK if the computer takes the place of the last %d human(s)?",
                        humans, humans - giNumHumanPlayers);
                NormalDialog(gText, 2, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult != 0x7805) {
                    accept = 0;
                }
            }
        }
        if (accept != 0) {
            message.type = 0x4000;
            message.field4 = 4;
            return 2;
        }
    }
    return 1;
}

VA(0x0048ec9a, 0x2e8)
void fileRequester::DoKnob(void) {}

VA(0x0048ef82, 0xc42)
void fileRequester::Update(int) {}

VA(0x0048fbc4, 0x15b)
char * fileRequester::GetFilename(void) { return 0; }


// ===== vtable fileRequester : public baseManager  (3 slots) =====
//  [ 0] VA(0x0048d5e1, 0x466)  int fileRequester::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x0048d556, 0x8b)  void fileRequester::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x0048daec, 0x11ae)  int fileRequester::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(fileRequester, 0x004eb888);

// ---- globals (definitions, RVA order) ----
DATA(0x004f8674) int giMapSizeFilter;
DATA(0x004f88c4) char *cFRDummy;
DATA(0x0052857c) float fGutterMinY;
DATA(0x00528580) float fGutterTravelLength;
DATA(0x00528584) int iMaxListSize;
