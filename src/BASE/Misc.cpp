// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Misc.obj   from: .\basewin.lib
// functions: 46   data: 11
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/kbwin.h>
#include <BASE/heroWindow.h>
#include <BASE/mouseManager.h>
#include <_globals_model.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <BASE/font.h>
#include <BASE/Misc.h>
#include <BASE/miscwin.h>        // this TU's own free functions + indexArray/IconEntry
#include <SOURCE/KB.h>        // EventWindowHandler, FileError, ShutDown
#include <SOURCE/wingraph.h>
#include <SOURCE/NOOPT.h>  // SetFullScreenStatus
#include <_carcass_types.h>   // tag_message (member access)
#include <windows.h>      // MessageBoxA
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <BASE/palette.h>
#include <SOURCE/X_GLOBAL.h>

static int giFindMid;




// ---- module-private synthetic globals (retail xref: single-module) ----
DATA(0x005331cc) static int gBlitRight;   // BlitBitmapToScreen computed blit-rect right edge
DATA(0x005331d0) static int gBlitBottom;  // BlitBitmapToScreen computed blit-rect bottom edge

VA(0x004c3d10, 0x58)
// void InitMemEntry(void);

VA(0x004c3d70, 0x20f)
// void *BaseAlloc(unsigned int, char *, int);

VA(0x004c3f80, 0x386)
// void BaseFree(void *, char *, int);

VA(0x004c4310, 0x134)
void PrintMemoryLeaks(void)
{
    char local_1f4[500];
    if (0 < giDebugLevel && gpMemEntry != 0) {
        LogInt("Total Memory Leaks", iMemEntries, -999, -999, -999, -999, -999, -999);
        int i = 0;
        do {
            if (gpMemEntry[i].used != 0) {
                sprintf(gText, "Memory Leak,  File '%13s'  Line % 4d, ptr %12d   size %6d",
                        gpMemEntry[i].file, gpMemEntry[i].line, reinterpret_cast<int>(gpMemEntry[i].ptr),
                        gpMemEntry[i].size);
                if (1 < giDebugLevel) {
                    FILE *_File = fopen("KB.LOG", "a");
                    if (_File != 0) {
                        strcpy(local_1f4, gText);
                        strcat(local_1f4, "\n");
                        fputs(local_1f4, _File);
                        fclose(_File);
                        if (giDebugLevel == 4)
                            OutputDebugStringA(local_1f4);
                    }
                }
            }
            i = i + 1;
        } while (i < 2000);
    }
}

VA(0x004c4450, 0x91)
void ShowMemoryStatus(void)
{
    sprintf(gText, "Mem Left %dK", 0x3ea2);
    int iVar1 = giDebugLevel;
    giDebugLevel = 9;
    FillBitmapArea(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0);
    smallFont->DrawBoundedString(gText, 0, 0x1d0, 0x280, 0x10, 1, 0);
    BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0, 0x1cc);
    giDebugLevel = iVar1;
}

VA(0x004c44f0, 0x48)
unsigned long int MAKEFILEID(char *text)
{
    unsigned int hash = 0;
    int sum = 0;
    for (int i = strlen(text) - 1; i >= 0; --i) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            text[i] &= ~0x20;
        }
        hash = (hash << 5) + (hash >> 25);
        sum += text[i];
        hash += text[i] + sum;
    }
    return hash;
}

VA(0x004c4540, 0x95)
int FindIndex(struct indexArray *entries, int low, int high, int key)
{
    giFindMid = (low + high) >> 1;
    while (high - low > 1) {
        if (entries[giFindMid].field0 <= key) {
            low = giFindMid;
            if (entries[giFindMid].field0 == key) {
                return entries[low].field2;
            }
        } else {
            high = giFindMid;
        }
        giFindMid = (low + high) >> 1;
    }
    if (entries[low].field0 == key) {
        return entries[low].field2;
    }
    if (entries[high].field0 == key) {
        return entries[high].field2;
    }
    return 0xFFFF;
}

VA(0x004c45e0, 0xea)
// void FadeIn(int);

VA(0x004c46d0, 0xe6)
// void FadeOut(int);

VA(0x004c47c0, 0x28)
int Random(int low, int high)
{
    if (low == high) {
        return high;
    }
    if (high < low) {
        return low;
    }
    return low + rand() % (high - low + 1);
}

// @early-stop
// byte-exact except the MessageBoxA call: retail is a bare `ff 15 [0x53a650]` (the
// delinker left the IAT import unnamed); ours is the same `ff 15` with an
// __imp__MessageBoxA@16 reloc. Code bytes identical; only the masked IAT operand differs.
VA(0x004c47f0, 0x5d)
void ProcessAssert(int condition, char *file, int line)
{
    if (condition == 0) {
        gpMouseManager->SetColorMice(0);
        SetFullScreenStatus(0);
        sprintf(gText, "Assert statement failed in module %s, line %d.  Do you wish to abort the program?", file, line);
        if (MessageBoxA(hwndApp, gText, "Assert Failure", 0x14) != 7) {
            ShutDown(0);
        }
    }
}

VA(0x004c4850, 0x66)
char * FindStringInString(char *text, char *pattern)
{
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    int count = text_len - pattern_len + 1;
    for (int i = 0; i < count; ++i) {
        if (strncmp(text + i, pattern, pattern_len) == 0) {
            return text + i;
        }
    }
    return 0;
}

VA(0x004c48c0, 0x31)
char * FindToken(char *text, char token)
{
    int len = strlen(text);
    for (int i = 0; len > i; ++i) {
        if (text[i] == token) {
            return text + i;
        }
    }
    return 0;
}

VA(0x004c4900, 0x2b)
char * FindLastToken(char *text, char token)
{
    for (int i = strlen(text) - 1; i >= 0; --i) {
        if (text[i] == token) {
            return text + i;
        }
    }
    return 0;
}

VA(0x004c4930, 0x6c)
void SetInstallDefaults(void)
{
    memset(&gConfig, 0, 0x19d);
    strcpy(gConfig.autoLoadName, "AUTO");
    strcpy(gConfig.autoSaveName, "AUTO");
    gConfig.soundQuality = 1;
}

VA(0x004c49a0, 0x1b5)
void SetGameDefaults(void) {}

VA(0x004c4b60, 0x13f)
void ReadPrefsFromFile(void)
{
    sprintf(gText, "%s", "HEROES2.CFG");
    if (_access(gText, 0) == -1) {
        memset(&gConfig, 0, 0x19d);
        strcpy(gConfig.autoLoadName, "AUTO");
        strcpy(gConfig.autoSaveName, "AUTO");
        gConfig.soundQuality = 1;
    } else {
        FILE *f = fopen(gText, "rb");
        if (f == 0)
            FileError(gText);
        fread(&gConfig, 0x19d, 1, f);
        fclose(f);
        if (gConfig.autoSaveName[0xe] == 0)
            goto skipDefaults;
    }
    SetGameDefaults();
    UpdateSystemOptionsMenu();
    WritePrefsToRegistry();
skipDefaults:
    strcpy(gcRegCDRomPath, "");
    strcpy(gcRegAppPath, "");
}

// Byte counts the registry string values are written with (hard-coded in retail; not the
// same as the in-struct buffer sizes).
#define REG_MODEM_INIT_STRING_SIZE   0x62   // 98 bytes
#define REG_UNIQUE_SYSTEM_ID_SIZE    4      // 4 bytes
#define REG_NETWORK_DEFAULT_NAME_SIZE 0x1e  // 30 bytes

VA(0x004c4ca0, 0x7ab)
void ReadPrefsFromRegistry(void)
{
    HKEY hKey;
    unsigned long dwType;
    unsigned long dwSize;
    char szKey[100];
    char szScratch[88];

    strcpy(szScratch, "");
    strcpy(szKey, "SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0");
    hKey = 0;
    if (RegOpenKeyA(HKEY_LOCAL_MACHINE, szKey, &hKey) != 0)
        return;
    dwSize = 4;
    if (RegQueryValueExA(hKey, "Music Volume", 0, &dwType,
                         reinterpret_cast<unsigned char *>(&gConfig.musicVolume), &dwSize) != 0) {
        memset(&gConfig, 0, 0x19d);
        memset(&gConfig, 0, 0x19d);
        strcpy(gConfig.autoLoadName, "AUTO");
        strcpy(gConfig.autoSaveName, "AUTO");
        gConfig.soundQuality = 1;
        SetGameDefaults();
        RegCloseKey(hKey);
        UpdateSystemOptionsMenu();
        WritePrefsToRegistry();
        return;
    }
    RegQueryValueExA(hKey, "Music Volume", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.musicVolume), &dwSize);
    RegQueryValueExA(hKey, "Sound Volume", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.soundVolume), &dwSize);
    RegQueryValueExA(hKey, "Walk Speed", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.walkSpeed), &dwSize);
    RegQueryValueExA(hKey, "Computer Walk Speed", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.computerWalkSpeed), &dwSize);
    RegQueryValueExA(hKey, "Show Route", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.showRoute), &dwSize);
    RegQueryValueExA(hKey, "Blackout Computer", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.blackoutComputer), &dwSize);
    RegQueryValueExA(hKey, "Sound Quality", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.soundQuality), &dwSize);
    RegQueryValueExA(hKey, "Use Opera", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.useOpera), &dwSize);
    RegQueryValueExA(hKey, "Direct Connect Com Port", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.directConnectComPort), &dwSize);
    RegQueryValueExA(hKey, "Direct Connect Baud Rate", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.directConnectBaudRate), &dwSize);
    RegQueryValueExA(hKey, "Modem Com Port", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.modemComPort), &dwSize);
    RegQueryValueExA(hKey, "Modem Baud Rate", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.modemBaudRate), &dwSize);
    dwSize = 0x63;
    RegQueryValueExA(hKey, "Modem Init String", 0, &dwType, reinterpret_cast<unsigned char *>(gConfig.modemInitString), &dwSize);
    dwSize = 4;
    RegQueryValueExA(hKey, "Unique System ID", 0, &dwType, reinterpret_cast<unsigned char *>(gConfig.uniqueSystemID), &dwSize);
    dwSize = 0x1f;
    RegQueryValueExA(hKey, "Network Default Name", 0, &dwType, reinterpret_cast<unsigned char *>(gConfig.networkDefaultName), &dwSize);
    dwSize = 4;
    RegQueryValueExA(hKey, "Autosave", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.autosave), &dwSize);
    RegQueryValueExA(hKey, "Slow Video", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.slowVideo), &dwSize);
    RegQueryValueExA(hKey, "Show Combat Grid", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.showCombatGrid), &dwSize);
    RegQueryValueExA(hKey, "Show Combat Mouse Hex", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.showCombatMouseHex), &dwSize);
    RegQueryValueExA(hKey, "Combat Shade Level", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.combatShadeLevel), &dwSize);
    RegQueryValueExA(hKey, "Combat Army Info Level", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.combatArmyInfoLevel), &dwSize);
    RegQueryValueExA(hKey, "Evil Interface Usage", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.evilInterfaceUsage), &dwSize);
    RegQueryValueExA(hKey, "Quick Combat Level", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.quickCombatLevel), &dwSize);
    RegQueryValueExA(hKey, "Combat Speed", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.combatSpeed), &dwSize);
    RegQueryValueExA(hKey, "Auto Combat Use Spells", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.autoCombatUseSpells), &dwSize);
    RegQueryValueExA(hKey, "First Map Offset", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.firstMapOffset), &dwSize);
    RegQueryValueExA(hKey, "Current Map Offset", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.currentMapOffset), &dwSize);
    RegQueryValueExA(hKey, "Show Object Boxes", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.showObjectBoxes), &dwSize);
    RegQueryValueExA(hKey, "Editor Screen Animation", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.editorScreenAnimation), &dwSize);
    RegQueryValueExA(hKey, "Editor Palette Cycling", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.editorPaletteCycling), &dwSize);
    RegQueryValueExA(hKey, "Main Game Show Menu", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].showMenu), &dwSize);
    RegQueryValueExA(hKey, "Main Game X", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].x), &dwSize);
    RegQueryValueExA(hKey, "Main Game Y", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].y), &dwSize);
    RegQueryValueExA(hKey, "Main Game Width", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].width), &dwSize);
    RegQueryValueExA(hKey, "Main Game Height", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].height), &dwSize);
    RegQueryValueExA(hKey, "Main Game Full Screen", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].fullScreen), &dwSize);
    RegQueryValueExA(hKey, "Main Game Color Mouse Cursor", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].colorMouseCursor), &dwSize);
    RegQueryValueExA(hKey, "Editor Show Menu", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].showMenu), &dwSize);
    RegQueryValueExA(hKey, "Editor X", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].x), &dwSize);
    RegQueryValueExA(hKey, "Editor Y", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].y), &dwSize);
    RegQueryValueExA(hKey, "Editor Width", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].width), &dwSize);
    RegQueryValueExA(hKey, "Editor Height", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].height), &dwSize);
    RegQueryValueExA(hKey, "Editor Full Screen", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].fullScreen), &dwSize);
    RegQueryValueExA(hKey, "Editor Color Mouse Cursor", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].colorMouseCursor), &dwSize);
    dwSize = 0x63;
    if (RegQueryValueExA(hKey, "AppPath", 0, &dwType, reinterpret_cast<unsigned char *>(gcRegAppPath), &dwSize) != 0)
        strcpy(gcRegAppPath, "");
    if (RegQueryValueExA(hKey, "CDDrive", 0, &dwType, reinterpret_cast<unsigned char *>(gcRegCDRomPath), &dwSize) != 0)
        strcpy(gcRegCDRomPath, "");
    RegCloseKey(hKey);
    // Clamp the saved window geometry to sane defaults / on-screen bounds.
    if (gConfig.gfx[giCurExe].width <= 0)
        gConfig.gfx[giCurExe].width = 0x140;            // default 320 wide
    if (gConfig.gfx[giCurExe].height <= 0)
        gConfig.gfx[giCurExe].height = 0xf0;            // default 240 tall
    if (gConfig.gfx[giCurExe].x < 0)
        gConfig.gfx[giCurExe].x = 0;
    if (gConfig.gfx[giCurExe].x > giMainVideoModeHeight - 0xc8)   // keep >= 200px on-screen
        gConfig.gfx[giCurExe].x = giMainVideoModeHeight - 0xc8;
    if (gConfig.gfx[giCurExe].y < 0)
        gConfig.gfx[giCurExe].y = 0;
    if (gConfig.gfx[giCurExe].y > giMainVideoModeWidth - 0xc8)
        gConfig.gfx[giCurExe].y = giMainVideoModeWidth - 0xc8;
}

VA(0x004c5450, 0xa1)
void ReadPrefs(void)
{
    memset(&gConfig, 0, 0x19d);
    ReadPrefsFromRegistry();
    sprintf(gConfig.rmtRLName, "RMT%sRL.BIN", gConfig.uniqueSystemID);
    sprintf(gConfig.rmtRCName, "RMT%sRC.BIN", gConfig.uniqueSystemID);
    sprintf(gConfig.rmtRDName, "RMT%sRD.BIN", gConfig.uniqueSystemID);
    sprintf(gConfig.rmtSLName, "RMT%sSL.BIN", gConfig.uniqueSystemID);
    sprintf(gConfig.rmtSCName, "RMT%sSC.BIN", gConfig.uniqueSystemID);
    sprintf(gConfig.rmtSDName, "RMT%sSD.BIN", gConfig.uniqueSystemID);
}

VA(0x004c5500, 0x6a)
void WritePrefsToFile(void)
{
    int local_64[25];
    int i;
    int *p = local_64;
    for (i = 0x19; i != 0; i--) {
        *p = 0;
        p++;
    }
    sprintf(gText, "%s", "HEROES2.CFG");
    int fd = _open(gText, 0x8301, 0x80);
    if (fd != -1) {
        _write(fd, &gConfig, 0x19d);
        _close(fd);
    }
}

VA(0x004c5570, 0x491)
void WritePrefsToRegistry(void)
{
    HKEY hKey;
    char szKey[100];
    char szScratch[88];

    strcpy(szScratch, "");
    strcpy(szKey, "SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0");
    hKey = 0;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, szKey, 0, KEY_ALL_ACCESS, &hKey) != 0)
        return;
    RegSetValueExA(hKey, "Music Volume", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.musicVolume), 4);
    RegSetValueExA(hKey, "Sound Volume", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.soundVolume), 4);
    RegSetValueExA(hKey, "Walk Speed", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.walkSpeed), 4);
    RegSetValueExA(hKey, "Computer Walk Speed", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.computerWalkSpeed), 4);
    RegSetValueExA(hKey, "Show Route", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.showRoute), 4);
    RegSetValueExA(hKey, "Blackout Computer", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.blackoutComputer), 4);
    RegSetValueExA(hKey, "Sound Quality", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.soundQuality), 4);
    RegSetValueExA(hKey, "Use Opera", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.useOpera), 4);
    RegSetValueExA(hKey, "Direct Connect Com Port", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.directConnectComPort), 4);
    RegSetValueExA(hKey, "Direct Connect Baud Rate", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.directConnectBaudRate), 4);
    RegSetValueExA(hKey, "Modem Com Port", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.modemComPort), 4);
    RegSetValueExA(hKey, "Modem Baud Rate", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.modemBaudRate), 4);
    RegSetValueExA(hKey, "Modem Init String", 0, REG_SZ, reinterpret_cast<unsigned char *>(gConfig.modemInitString), REG_MODEM_INIT_STRING_SIZE);
    RegSetValueExA(hKey, "Unique System ID", 0, REG_SZ, reinterpret_cast<unsigned char *>(gConfig.uniqueSystemID), REG_UNIQUE_SYSTEM_ID_SIZE);
    RegSetValueExA(hKey, "Network Default Name", 0, REG_SZ, reinterpret_cast<unsigned char *>(gConfig.networkDefaultName), REG_NETWORK_DEFAULT_NAME_SIZE);
    RegSetValueExA(hKey, "Autosave", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.autosave), 4);
    RegSetValueExA(hKey, "Slow Video", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.slowVideo), 4);
    RegSetValueExA(hKey, "Show Combat Grid", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.showCombatGrid), 4);
    RegSetValueExA(hKey, "Show Combat Mouse Hex", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.showCombatMouseHex), 4);
    RegSetValueExA(hKey, "Combat Shade Level", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.combatShadeLevel), 4);
    RegSetValueExA(hKey, "Combat Army Info Level", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.combatArmyInfoLevel), 4);
    RegSetValueExA(hKey, "Evil Interface Usage", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.evilInterfaceUsage), 4);
    RegSetValueExA(hKey, "Quick Combat Level", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.quickCombatLevel), 4);
    RegSetValueExA(hKey, "Combat Speed", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.combatSpeed), 4);
    RegSetValueExA(hKey, "Auto Combat Use Spells", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.autoCombatUseSpells), 4);
    RegSetValueExA(hKey, "First Map Offset", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.firstMapOffset), 4);
    RegSetValueExA(hKey, "Current Map Offset", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.currentMapOffset), 4);
    RegSetValueExA(hKey, "Show Object Boxes", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.showObjectBoxes), 4);
    RegSetValueExA(hKey, "Editor Screen Animation", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.editorScreenAnimation), 4);
    RegSetValueExA(hKey, "Editor Palette Cycling", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.editorPaletteCycling), 4);
    RegSetValueExA(hKey, "Main Game Show Menu", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].showMenu), 4);
    RegSetValueExA(hKey, "Main Game X", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].x), 4);
    RegSetValueExA(hKey, "Main Game Y", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].y), 4);
    RegSetValueExA(hKey, "Main Game Width", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].width), 4);
    RegSetValueExA(hKey, "Main Game Height", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].height), 4);
    RegSetValueExA(hKey, "Main Game Full Screen", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].fullScreen), 4);
    RegSetValueExA(hKey, "Main Game Color Mouse Cursor", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[0].colorMouseCursor), 4);
    RegSetValueExA(hKey, "Editor Show Menu", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].showMenu), 4);
    RegSetValueExA(hKey, "Editor X", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].x), 4);
    RegSetValueExA(hKey, "Editor Y", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].y), 4);
    RegSetValueExA(hKey, "Editor Width", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].width), 4);
    RegSetValueExA(hKey, "Editor Height", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].height), 4);
    RegSetValueExA(hKey, "Editor Full Screen", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].fullScreen), 4);
    RegSetValueExA(hKey, "Editor Color Mouse Cursor", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.gfx[1].colorMouseCursor), 4);
    RegCloseKey(hKey);
}

VA(0x004c5a10, 0xa)
void WritePrefs(void)
{
    UpdateSystemOptionsMenu();
    WritePrefsToRegistry();
}

VA(0x004c5a20, 0x3c)
int IsCDDrive(int param_1)
{
    sprintf(gText, "A:\\");
    gText[0] = gText[0] + param_1;
    return GetDriveTypeA(gText) == DRIVE_CDROM;
}

VA(0x004c5a60, 0x3ed)
int SetupCDDrive(void) { return 0; }

VA(0x004c5e50, 0x18)
void BitmapToScreen(class bitmap *bmp)
{
    BlitBitmapToScreen(bmp, 0, 0, bmp->m_width, bmp->m_height, 0, 0);
}

VA(0x004c5e70, 0x3d)
void SetPalette(signed char *param_1, int param_2)
{
    memcpy(gpBufferPalette->m_data, param_1, 0x300);
    memcpy(gCyclePal, param_1 + 0x282, 0x60);
    if (param_2 != 0)
        UpdatePalette(gpBufferPalette->m_data);
}

VA(0x004c5eb0, 0x25)
void BlitBitmapToScreenNoMouseCheck(class bitmap *bmp, int p2, int p3, int p4, int p5, int p6, int p7)
{
    BlitBitmapToScreenVesa(reinterpret_cast<int>(bmp), p2, p3, p4, p5, p6, p7);
}

VA(0x004c5ee0, 0x18b)
void BlitBitmapToScreen(class bitmap *param_1, int param_2, int param_3, int param_4, int param_5,
                        int param_6, int param_7)
{
    int local_8;
    if (gbColorMice == 0) {
        BlitBitmapToScreenVesa(reinterpret_cast<int>(param_1), param_2, param_3, param_4, param_5,
                               param_6, param_7);
        return;
    }
    if (giScrollX != 0 || (local_8 = param_2, giScrollY != 0)) {
        param_4 = 0x1c0;
        local_8 = giScrollX + 0x10;
        param_3 = giScrollY + 0x10;
        param_5 = 0x1c0;
    }
    gBlitRight = param_4 + param_6 - 1;
    gBlitBottom = param_5 + param_7 - 1;
    if (gpMouseManager->IsVis() != 0 && gpMouseManager->m_savedW <= gBlitRight &&
        param_6 <= gpMouseManager->field_0x6e && gpMouseManager->m_savedH <= gBlitBottom &&
        param_7 <= gpMouseManager->field_0x72) {
        gpMouseManager->SaveAndDraw();
        BlitBitmapToScreenVesa(reinterpret_cast<int>(param_1), local_8, param_3, param_4, param_5,
                               param_6, param_7);
        if (gBlitRight < gpMouseManager->field_0x6e || gpMouseManager->m_savedW < param_6 ||
            gBlitBottom < gpMouseManager->field_0x72 || gpMouseManager->m_savedH < param_7) {
            int iVar1 = gpMouseManager->m_savedH;
            int iVar2 = gpMouseManager->m_savedW;
            BlitBitmapToScreenVesa(reinterpret_cast<int>(param_1), iVar2, iVar1,
                                   gpMouseManager->field_0x6e - iVar2 + 1,
                                   gpMouseManager->field_0x72 - iVar1 + 1, iVar2, iVar1);
        }
        gpMouseManager->RestoreUnderlying();
        return;
    }
    BlitBitmapToScreenVesa(reinterpret_cast<int>(param_1), local_8, param_3, param_4, param_5, param_6,
                           param_7);
}

VA(0x004c6070, 0xa6)
void LogTruncate(void)
{
    char local_1f4[500];
    if (1 < giDebugLevel) {
        int _FileHandle = _open("KB.LOG", 0x4301, 0x80);
        if (_FileHandle != -1) {
            strcpy(local_1f4, "===========New Log==========\n");
            _write(_FileHandle, local_1f4, strlen(local_1f4));
            _close(_FileHandle);
        }
    }
}

VA(0x004c6120, 0x9e)
void LogStr(char *param_1)
{
    char local_1f4[500];
    if (giDebugLevel > 1) {
        FILE *f = fopen("KB.LOG", "a");
        if (f != 0) {
            strcpy(local_1f4, param_1);
            strcat(local_1f4, "\n");
            fputs(local_1f4, f);
            fclose(f);
        }
        OutputDebugStringA(local_1f4);
    }
}

VA(0x004c61c0, 0x224)
// void LogInt(char *, int, int, int, int, int, int, int);

VA(0x004c63f0, 0x6c)
void AiPrint(char *param_1)
{
    if (giDebugLevel > 1) {
        FillBitmapArea(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0);
        smallFont->DrawBoundedString(param_1, 0, 0x1d0, 0x280, 0x10, 1, 0);
        BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0, 0x1cc);
    }
}

VA(0x004c6460, 0x7a)
void AbsAiPrint(char *param_1)
{
    int saved = giDebugLevel;
    giDebugLevel = 9;
    FillBitmapArea(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0);
    smallFont->DrawBoundedString(param_1, 0, 0x1d0, 0x280, 0x10, 1, 0);
    BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0, 0x1cc);
    giDebugLevel = saved;
}

VA(0x004c64e0, 0xf8)
void FadeTo(unsigned char *param_1, unsigned char *param_2, int param_3)
{
    int local_310, local_304;
    unsigned char local_300[768];
    memcpy(local_300, param_1, 0x300);
    param_3 = param_3 >> 2;
    if (param_3 < 1)
        param_3 = 1;
    local_310 = 0x30;
    do {
        local_304 = KBTickCount() + 0x32;
        PollSound();
        int iVar6 = (0x40 - local_310) - param_3;
        if (iVar6 < 0)
            iVar6 = 0;
        unsigned char *pbVar8 = local_300;
        unsigned char bVar1 = giChangeThreshold[iVar6];
        iVar6 = 0x300;
        unsigned char *pbVar9 = param_2;
        do {
            unsigned char bVar2 = *pbVar8;
            unsigned int uVar7 = (unsigned int)*pbVar9 - (unsigned int)bVar2;
            int iVar5 = (uVar7 ^ ((int)uVar7 >> 0x1f)) - ((int)uVar7 >> 0x1f);
            if (static_cast<int>(bVar1) < iVar5) {
                char cVar3 = (char)iVar5 - bVar1;
                if ((int)uVar7 < 1)
                    *pbVar8 = bVar2 - cVar3;
                else
                    *pbVar8 = cVar3 + bVar2;
            }
            pbVar8++;
            pbVar9++;
            iVar6--;
        } while (iVar6 != 0);
        UpdatePalette(reinterpret_cast<signed char *>(local_300));
        DelayTil(&local_304);
        local_310 = local_310 + param_3;
    } while (local_310 < 0x40);
    UpdatePalette(reinterpret_cast<signed char *>(param_2));
}

VA(0x004c65e0, 0xb8)
void FadeToColorTable(unsigned char *param_1, int param_2)
{
    unsigned char auStack_300[768];
    int uVar1 = gpWindowManager->m_updateFlags;
    gpWindowManager->m_updateFlags = 0;
    int iVar8 = 0;
    unsigned char *puVar2 = reinterpret_cast<unsigned char *>(gpBufferPalette->m_data);
    unsigned char *puVar3 = auStack_300;
    unsigned char *puVar4;
    do {
        puVar4 = puVar3 + 3;
        int iVar5 = static_cast<unsigned int>(param_1[iVar8]) * 3;
        iVar8++;
        *puVar3 = puVar2[iVar5];
        puVar3[1] = puVar2[iVar5 + 1];
        puVar3[2] = puVar2[iVar5 + 2];
        puVar3 = puVar4;
    } while (puVar4 < auStack_300 + 0x300);
    iVar8 = 0x1e0;
    FadeTo(puVar2, auStack_300, param_2);
    unsigned char *pbVar7 = gpWindowManager->m_screen->m_pixels;
    do {
        int iVar5 = 0x280;
        unsigned char *pbVar6 = pbVar7;
        do {
            pbVar7 = pbVar6 + 1;
            iVar5--;
            *pbVar6 = param_1[*pbVar6];
            pbVar6 = pbVar7;
        } while (iVar5 != 0);
        iVar8--;
    } while (iVar8 != 0);
    gpWindowManager->UpdateScreen();
    UpdatePalette(reinterpret_cast<signed char *>(puVar2));
    gpWindowManager->m_updateFlags = uVar1;
}

VA(0x004c66a0, 0x29)
int IsCycleColor(int color)
{
    if ((color >= 0xD6 && color <= 0xDD) || (color >= 0xE7 && color <= 0xED)) {
        return 1;
    }
    return 0;
}

VA(0x004c66d0, 0x1ee)
void CreatePCXFile(char *, unsigned char *, int, int, unsigned char *) {}

VA(0x004c68c0, 0x52)
long int FileSize(char *filename)
{
    FILE *file = fopen(filename, "r+b");
    if (file == 0) {
        FileError(filename);
    }
    fseek(file, 0, 2);
    long size = ftell(file);
    fseek(file, 0, 0);
    fclose(file);
    return size;
}

VA(0x004c6920, 0xc)
struct IconEntry * GetIconEntry(class icon *iconPtr, int index)
{
    IconEntry *entries = *(IconEntry **)((char *)iconPtr + 0x12);
    return &entries[index];
}

VA(0x004c6930, 0xb8)
int SRandom(int low, int high)
{
    if (high == low) {
        return high;
    }
    if (high < low) {
        return low;
    }

    int high_term = (high * 13) & 0xff;
    int low_term = (low * 13) & 0xff;
    iLastSeed += high_term << 5;
    iLastSeed += low_term * 13233;
    iLastSeed += high_term;
    iLastSeed += (iLastSeed & 0x3f) << 8;
    iLastSeed &= 0xfff;

    int mix = iLastSeed * 7;
    mix += (mix & 0xff0) >> 4;
    int result = 0;
    for (int i = 31; i >= 0; --i) {
        if (mix & (1 << i)) {
            result |= 1 << i;
        }
    }
    iLastSeed = mix + low + high * 8;
    return low + result % (high - low + 1);
}

VA(0x004c69f0, 0x5c)
void SIncRandomize(int x, int y)
{
    int x_term = (x * 13) & 0xff;
    int y_term = (y * 13) & 0xff;
    iLastSeed += y_term << 5;
    iLastSeed += x_term * 13233;
    iLastSeed += y_term;
    iLastSeed += (iLastSeed & 0x3f) << 8;
}

VA(0x004c6a50, 0x10)
void SRand(int seed)
{
    iLastSeed = seed;
    srand(seed);
}

VA(0x004c6a60, 0x48)
int SGenRand(void)
{
    int result = 0;
    iLastSeed &= 0xfff;
    int mix = iLastSeed * 7;
    mix += (mix & 0xff0) >> 4;
    for (int i = 31; i >= 0; --i) {
        if (mix & (1 << i)) {
            result |= 1 << i;
        }
        iLastSeed = mix;
    }
    return result;
}

VA(0x004c6ab0, 0x6)
int MemSize(int)
{
    return 0x3ea2;
}

VA(0x004c6ac0, 0x386)
void GetDataEntry(char *, char *, int, char *, int, int) {}

VA(0x004c6e50, 0x173)
int DataEntryWindowHandler(struct tag_message &message)
{
    if (bDataEntryTime == 0) {
        bDataEntryTime = 1;
        message.type = 8;
        message.field4 = inBoxX;
        message.field8 = inBoxY;
        DataEntryWin->BroadcastMessage(message);
        return 1;
    }

    if (bDataEntryTime != 1) {
        if (message.type != 0x200) {
            return EventWindowHandler(message);
        }
        if (message.field4 == 0xC) {
            if (message.field8 != 0xA) {
                return EventWindowHandler(message);
            }
        } else if (message.field4 == 0xD) {
            if (message.field8 == 0x7802) {
                message.field8 = 0xA;
                message.field4 = 0xA;
                return 2;
            }
            return EventWindowHandler(message);
        } else {
            return EventWindowHandler(message);
        }
    }

    message.type = 0x200;
    message.field8 = 0xA;
    message.field4 = 7;
    DataEntryWin->BroadcastMessage(message);
    if (strlen(message.text) == 0) {
        return EventWindowHandler(message);
    }
    memset(cDEDest, 0, iDEMaxLen);
    strncpy(cDEDest, message.text, iDEMaxLen - 1);
    message.type = 0x200;
    message.field4 = 3;
    message.field8 = 0xA;
    message.text = cDEDest;
    DataEntryWin->BroadcastMessage(message);
    DataEntryWin->DrawWindow(1, 10, 10);
    if (gbTextEntryEscaped != 0) {
        return EventWindowHandler(message);
    }
    *(int *)((char *)gpWindowManager + 0x5a) = message.field8;
    message.field8 = 0xA;
    message.field4 = 0xA;
    return 2;
}

// ---- globals (definitions, RVA order) ----
DATA(0x0051dce8) int iMemEntries;
DATA(0x0051dcec) MemEntry *gpMemEntry;
DATA(0x0051dcf0) int giTotalMemAllocated;
DATA(0x0051dcf8) unsigned char giChangeThreshold[16];
DATA(0x0051dd08) int iLastSeed;
DATA(0x005331c4) class heroWindow *DataEntryWin;
DATA(0x005331c8) char *cDEDest;
DATA(0x005331d4) int iDEMaxLen;
DATA(0x005331d8) int bDataEntryTime;
DATA(0x005331dc) int inBoxX;
DATA(0x005331e0) int inBoxY;
