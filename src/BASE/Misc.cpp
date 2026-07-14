// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Misc.obj   from: .\basewin.lib
// functions: 46   data: 11
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#define HOMM2_MISC_INLINE_ICONENTRY
#include <va.h>
#include <SOURCE/kbwin.h>
#include <BASE/heroWindow.h>
#include <BASE/mouseManager.h>
#include <_globals_model.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/icon.h>
#include <BASE/bmap2.h>
#include <BASE/font.h>
#include <BASE/textEntryWidget.h>
#include <BASE/Misc.h>
#undef HOMM2_MISC_INLINE_ICONENTRY
#include <BASE/miscwin.h>        // this TU's own free functions + indexArray/IconEntry
#include <SOURCE/KB.h>        // EventWindowHandler, FileError, ShutDown
#include <SOURCE/wingraph.h>
#include <SOURCE/NOOPT.h>  // SetFullScreenStatus
#include <_carcass_types.h>   // tag_message (member access)
#include <windows.h>      // MessageBoxA
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <fcntl.h>
#include <string.h>
#include <BASE/palette.h>
#include <SOURCE/X_GLOBAL.h>

DATA(0x005331c0) static int giFindMid;




// ---- module-private synthetic globals (retail xref: single-module) ----
DATA(0x005331cc) static int gBlitRight;   // BlitBitmapToScreen computed blit-rect right edge
DATA(0x005331d0) static int gBlitBottom;  // BlitBitmapToScreen computed blit-rect bottom edge

VA(0x004c3d10, 0x58)
void InitMemEntry(void)
{
    LogInt("IME", iMemEntries, -999, -999, -999, -999, -999, -999);
    gpMemEntry = static_cast<MemEntry *>(malloc(2000 * sizeof(MemEntry)));
    for (int i = 0; i < 2000; ++i)
        gpMemEntry[i].used = 0;
}

// @match-note
// Structurally complete /O2 checkpoint: allocation/tracking semantics, the 12-branch CFG,
// and all 29 relocations agree.  With the required real icon definition in this TU, base
// currently reserves 0x2c4 bytes and ends at 0x204; retail reserves 0x2bc and is 0x20f.
// The first divergence is the frame immediate at +0x2, followed at +0xc by base keeping
// size in EBP while retail uses EBX.  The two arrays account for the retail 700-byte frame;
// there is no missing local.  Earlier exact-frame TU states left only the newline intrinsic.
// `strcat`, strcpy/memcpy-at-strlen, direct word stores, volatile loads, and a manual scan
// were already tried.  Revisit only after required shared-header/TU state changes.
VA(0x004c3d70, 0x20f)
void *BaseAlloc(unsigned int size, char *originalFile, int originalLine)
{
    char text[200];
    char logText[500];
    if (size == 0)
        return 0;
    if (gpMemEntry == 0) {
        LogInt("IME", iMemEntries, -999, -999, -999, -999, -999, -999);
        gpMemEntry = static_cast<MemEntry *>(malloc(2000 * sizeof(MemEntry)));
        for (int initIndex = 0; initIndex < 2000; ++initIndex)
            gpMemEntry[initIndex].used = 0;
    }
    giTotalMemAllocated += size;
    void *ptr = malloc(size);
    if (ptr == 0) {
        MemError();
        return 0;
    }
    ++iMemEntries;
    int i;
    for (i = 0; i < 2000; ++i) {
        if (!gpMemEntry[i].used) {
            gpMemEntry[i].used = 1;
            gpMemEntry[i].ptr = ptr;
            gpMemEntry[i].size = size;
            strcpy(gpMemEntry[i].file, originalFile);
            gpMemEntry[i].line = originalLine;
            i = 99999;
        }
    }
    if (giDebugLevel == 4) {
        sprintf(text, "KBAlloc    Size %d   Ptr %d   File %s  Line %d", size, ptr,
                originalFile, originalLine);
        if (giDebugLevel >= 2) {
            FILE *f = fopen("KB.LOG", "at+");
            if (f != 0) {
                strcpy(logText, text);
                *reinterpret_cast<unsigned short *>(logText + strlen(logText)) =
                    *reinterpret_cast<const unsigned short *>("\n");
                fputs(logText, f);
                fclose(f);
                if (giDebugLevel == 4)
                    OutputDebugStringA(logText);
            }
        }
    }
    return ptr;
}

// @match-note
// Structurally complete /O2 checkpoint: both spans are 0x386 with the same 0x2c4 frame,
// 20-branch CFG, and all 52 relocations.  The first residual is the first newline append:
// base preloads the word then derives `buf+strlen`; retail scans first and writes `[edi-1]`.
// The other two append sites have the same scheduling difference.  All allocation/free,
// MemEntry, logging, and bad-delete paths agree.  The BaseAlloc append spellings were also
// tested here; revisit only after exact-preserving predecessor/TU-state changes.
VA(0x004c3f80, 0x386)
void BaseFree(void *ptr, char *originalFile, int originalLine)
{
    char logText[500];
    char text[200];
    if (gpMemEntry == 0) {
        LogInt("IME", iMemEntries, -999, -999, -999, -999, -999, -999);
        gpMemEntry = static_cast<MemEntry *>(malloc(2000 * sizeof(MemEntry)));
        for (int initIndex = 0; initIndex < 2000; ++initIndex)
            gpMemEntry[initIndex].used = 0;
    }
    if (giDebugLevel == 4)
        LogInt("Free ", reinterpret_cast<int>(ptr), -999, -999, -999, -999, -999, -999);
    if (ptr == 0) {
        if (giDebugLevel >= 2) {
            FILE *f = fopen("KB.LOG", "at+");
            if (f != 0) {
                strcpy(logText, "NULL POINTER");
                *reinterpret_cast<unsigned short *>(logText + strlen(logText)) =
                    *reinterpret_cast<const unsigned short *>("\n");
                fputs(logText, f);
                fclose(f);
                if (giDebugLevel == 4)
                    OutputDebugStringA(logText);
            }
        }
        return;
    }
    --iMemEntries;
    if (iMemEntries < 0)
        LogInt("MemEntries Below 0", iMemEntries, -999, -999, -999, -999, -999, -999);
    int i;
    for (i = 0; i < 2000; ++i) {
        if (gpMemEntry[i].ptr == ptr) {
            if (giDebugLevel == 4) {
                sprintf(text, "KBFree    Size %d   Ptr %d   File %s  Line %d", gpMemEntry[i].size,
                        ptr, gpMemEntry[i].file, gpMemEntry[i].line);
                if (giDebugLevel >= 2) {
                    FILE *f = fopen("KB.LOG", "at+");
                    if (f != 0) {
                        strcpy(logText, text);
                        *reinterpret_cast<unsigned short *>(logText + strlen(logText)) =
                            *reinterpret_cast<const unsigned short *>("\n");
                        fputs(logText, f);
                        fclose(f);
                        if (giDebugLevel == 4)
                            OutputDebugStringA(logText);
                    }
                }
            }
            gpMemEntry[i].used = 0;
            giTotalMemAllocated -= gpMemEntry[i].size;
            i = 99999;
        }
    }
    if (i < 99999) {
        sprintf(gText, "Bad Delete,  File '%13s'  Line % 4d, ptr %12d", originalFile,
                originalLine, ptr);
        if (giDebugLevel >= 2) {
            FILE *f = fopen("KB.LOG", "at+");
            if (f != 0) {
                strcpy(logText, gText);
                *reinterpret_cast<unsigned short *>(logText + strlen(logText)) =
                    *reinterpret_cast<const unsigned short *>("\n");
                fputs(logText, f);
                fclose(f);
                if (giDebugLevel == 4)
                    OutputDebugStringA(logText);
            }
        }
    } else {
        free(ptr);
    }
}

// @match-note
// Structurally complete /O2 checkpoint: base .text is 0x137 bytes versus retail 0x134.
// Only the completed log-line append differs: base loads `"\n"` at +0xd6 (reloc
// +0xd9) before deriving `buf+strlen`, while retail loads it at +0xe3 (reloc +0xe6)
// after `repne scasb` and stores through `[edi-1]`. LogInt, sprintf, fopen/fputs/
// fclose and OutputDebugStringA targets are otherwise identical. `strcat`, strlen+
// strcpy/memcpy, direct and named word stores, volatile load, and manual scan tried.
// Revisit through exact-preserving predecessor/TU-state variants; not byte-proven.
VA(0x004c4310, 0x134)
void PrintMemoryLeaks(void)
{
    char logText[500];
    if (giDebugLevel >= 1 && gpMemEntry != 0) {
        LogInt("Total Memory Leaks", iMemEntries, -999, -999, -999, -999, -999, -999);
        int i = 0;
        do {
            if (gpMemEntry[i].used != 0) {
                sprintf(gText, "Memory Leak,  File '%13s'  Line % 4d, ptr %12d   size %6d",
                        gpMemEntry[i].file, gpMemEntry[i].line, reinterpret_cast<int>(gpMemEntry[i].ptr),
                        gpMemEntry[i].size);
                if (giDebugLevel >= 2) {
                    FILE *logFile = fopen("KB.LOG", "at+");
                    if (logFile != 0) {
                        strcpy(logText, gText);
                        *reinterpret_cast<unsigned short *>(logText + strlen(logText)) =
                            *reinterpret_cast<const unsigned short *>("\n");
                        fputs(logText, logFile);
                        fclose(logFile);
                        if (giDebugLevel == 4)
                            OutputDebugStringA(logText);
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
    int savedDebugLevel = giDebugLevel;
    giDebugLevel = 9;
    FillBitmapArea(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0);
    smallFont->DrawBoundedString(gText, 0, 0x1d0, 0x280, 0x10, 1, 0);
    BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0, 0x1cc);
    giDebugLevel = savedDebugLevel;
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

// @match-note
// Structurally complete /O2 checkpoint: base is 0x97 bytes, retail 0x95, with the same
// eight-branch CFG and four giFindMid relocations to 0x5331c0.  The first current
// divergence is +0x2a: base emits `cmp edi,eax; jge`, retail `cmp eax,edi; jle` and then
// reuses those flags for equality where base emits another compare at +0x38.  The key/value
// fields are typed and named.  Direct tests, saved values, reversed/negated predicates,
// combined conditions, and three-way forms were tried; this is not a proven artifact.
VA(0x004c4540, 0x95)
int FindIndex(struct indexArray *entries, int low, int high, int key)
{
    giFindMid = (low + high) >> 1;
    while (high - low > 1) {
        int value = entries[giFindMid].key;
        if (value > key) {
            high = giFindMid;
        } else {
            low = giFindMid;
            if (value >= key)
                return entries[low].value;
        }
        giFindMid = (low + high) >> 1;
    }
    if (entries[low].key == key) {
        return entries[low].value;
    }
    if (entries[high].key == key) {
        return entries[high].value;
    }
    return 0xFFFF;
}

// @match-note
// Structurally complete /O2 checkpoint: both sections are 0xea with the same CFG and
// exact 11/11 relocations.  Explicit `threshold = 0x3f - level` preserves the recovered
// loop semantics.  In the current required-header TU state the first divergence is the
// allocation at +0x7: base carries fadePalette in ESI and tests ECX after construction;
// retail initially uses EBX and tests EAX before construction.  `new palette`, value-init,
// and split declaration/assignment were tried.  This register allocation moved after the
// icon type include; revisit after further exact-preserving shared-header/TU changes.
VA(0x004c45e0, 0xea)
void FadeIn(int increment)
{
    palette *fadePalette = new palette;
    if (fadePalette == 0)
        MemError();
    int done = 0;
    if (gConfig.gfx[giCurExe].fullScreen == 0)
        increment *= 2;
    memset(fadePalette->m_data, 0, 0x300);
    int level = 0;
    for (;;) {
        if (level >= 0x40) {
            if (done) {
                delete fadePalette;
                return;
            }
            level = 0x3f;
        }
        int delayUntil = KBTickCount() + 0x14;
        PollSound();
        signed char *colors;
        if (level == 0x3f) {
            done = 1;
            colors = gpBufferPalette->m_data;
        } else {
            int threshold = 0x3f - level;
            for (int i = 0; i < 0x300; ++i) {
                signed char color = gpBufferPalette->m_data[i];
                if (color > threshold)
                    fadePalette->m_data[i] = color - threshold;
            }
            colors = fadePalette->m_data;
        }
        UpdatePalette(colors);
        DelayTil(&delayUntil);
        level += increment;
    }
}

VA(0x004c46d0, 0xe6)
void FadeOut(int increment)
{
    palette *fadePalette = new palette;
    if (fadePalette == 0)
        MemError();
    int done = 0;
    if (gConfig.gfx[giCurExe].fullScreen == 0)
        increment *= 2;
    memcpy(fadePalette->m_data, gpBufferPalette->m_data, 0x300);
    int level = 0;
    for (;;) {
        if (level >= 0x40) {
            if (done) {
                delete fadePalette;
                return;
            }
            level = 0x3f;
        }
        int delayUntil = KBTickCount() + 0x14;
        PollSound();
        if (level == 0x3f)
            done = 1;
        for (int i = 0; i < 0x300; ++i) {
            if (fadePalette->m_data[i] > 0) {
                if (fadePalette->m_data[i] > increment)
                    fadePalette->m_data[i] -= increment;
                else
                    fadePalette->m_data[i] = 0;
            }
        }
        UpdatePalette(fadePalette->m_data);
        level += increment;
        DelayTil(&delayUntil);
    }
}

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

// @match-note
// Structurally complete /O2 checkpoint with equal 0x66-byte sections, the same frame/CFG,
// and the sole strncmp relocation.  In the current required-header state the first
// divergence is +0xa: base carries text/count/index in EBX/EBP/EDI while retail uses
// EBP/EBX/ESI; all calls, bounds, and return values remain equivalent.  Pointer operand
// swaps, `&i[text]`, lvalue count loads, for/while/do forms, count|0, and reversed bounds
// were tried.  This is a TU-state register allocation residual, not a proven artifact.
VA(0x004c4850, 0x66)
char * FindStringInString(char *text, char *pattern)
{
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    int count = text_len - pattern_len + 1;
    int i = 0;
    if (count > 0) {
        do {
            if (strncmp(text + i, pattern, pattern_len) == 0)
                return text + i;
            ++i;
        } while (count > i);
    }
    return 0;
}

// @match-note
// Structurally complete /O2 checkpoint with equal 0x31-byte sections and no relocs/calls.
// The current differences are the equivalent SIB at +0x17 (`[eax+esi]` versus
// `[esi+eax]`) and loop test at +0x21 (`cmp eax,ecx; jl` versus `cmp ecx,eax; jg`).
// for/while/do loops, explicit backedges, length/index `|0`, reversed bounds/returns,
// `i[text]`, and lvalue length loads did not steer them.  Revisit after TU-state changes;
// these operand-order encodings are not byte-proven artifacts.
VA(0x004c48c0, 0x31)
char * FindToken(char *text, char token)
{
    int len = strlen(text);
    int i = 0;
    if (len > 0) {
        do {
            if (text[i] == token)
                return text + i;
            ++i;
        } while (len > i);
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
    memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
    strcpy(gConfig.autoLoadName, "AUTO");
    strcpy(gConfig.autoSaveName, "AUTO");
    gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
}

// @match-note
// Structurally complete /O2 checkpoint: base is 0x1b9, retail 0x1b5, with the same
// three-branch CFG and exact 42/42 ordered relocations.  Retail's induction is now
// recovered: &gfx[0].fullScreen (+0x30), seven-word stride, endpoint showCombatGrid
// (+0x68), with semantic exeGfxConfig field access and no interior aliases.  The earlier
// gConfig.musicSource store was not present in retail and was removed.  The first residual
// is +0x1: after the common `push ebx`, base loads the induction address before saving the
// other registers and later preserves EBP, while retail saves ESI/EDI first and preserves
// only EBX/ESI/EDI.  Prior pointer/index/field-order variants are exhausted; revisit only
// after exact-preserving predecessor/shared-header TU state.
VA(0x004c49a0, 0x1b5)
void SetGameDefaults(void)
{
    int *fullScreen = &gConfig.gfx[0].fullScreen;
    gConfig.musicVolume = 1;
    gConfig.soundVolume = 1;
    gConfig.autosave = 1;
    gConfig.showRoute = 1;
    do {
        exeGfxConfig *gfx = reinterpret_cast<exeGfxConfig *>(
            fullScreen - (CONFIG_GRAPHICS_SIZE / sizeof(int) - 2));
        gfx->showMenu = 1;
        gfx->x = 10;
        gfx->y = 10;
        gfx->fullScreen = 1;
        gfx->colorMouseCursor = 0;
        if (giMainVideoModeWidth <= 0x280) {
            gfx->width = 0x1e0;
            gfx->height = 0x168;
        } else {
            gfx->width = 0x280;
            gfx->height = 0x1e0;
        }
        fullScreen += CONFIG_GRAPHICS_SIZE / sizeof(int);
    } while (fullScreen < &gConfig.showCombatGrid);
    gConfig.showCombatGrid = 0;
    gConfig.showCombatMouseHex = 0;
    gConfig.combatShadeLevel = 0;
    gConfig.combatArmyInfoLevel = 0;
    gConfig.evilInterfaceUsage = 0;
    gConfig.useOpera = CONFIG_OPERA_ENABLED;
    gConfig.quickCombatLevel = 0;
    gConfig.combatSpeed = 0;
    gConfig.autoCombatUseSpells = 0;
    gConfig.blackoutComputer = 0;
    gConfig.currentMapOffset = 0;
    gConfig.firstMapOffset = rand() % 32001;
    gConfig.showObjectBoxes = 0;
    gConfig.editorScreenAnimation = 0;
    gConfig.editorPaletteCycling = 0;
    gbFirstTimeThrough = 1;
    gConfig.slowVideo = 3;
    gConfig.computerWalkSpeed = 3;
    gConfig.walkSpeed = 2;
    strcpy(gConfig.networkDefaultName, "The Unknown Hero");
    *reinterpret_cast<int *>(gConfig.uniqueSystemID) = 0;
    int idSeed = rand() % 999999 + 1;
    idSeed += KBTickCount();
    gConfig.uniqueSystemID[2] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"[idSeed % 36];
    int idAdd = rand() % 999999 + 1;
    idAdd += KBTickCount();
    idSeed += idAdd;
    gConfig.uniqueSystemID[1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"[idSeed % 36];
    idAdd = rand() % 999999 + 1;
    idAdd += KBTickCount();
    idSeed += idAdd;
    gConfig.uniqueSystemID[0] = static_cast<char>(idSeed % 26 + 'A');
    gConfig.autoSaveName[14] = 0;
}

VA(0x004c4b60, 0x13f)
void ReadPrefsFromFile(void)
{
    sprintf(gText, "%s", "HEROES2.CFG");
    if (_access(gText, 0) == -1) {
        memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
        strcpy(gConfig.autoLoadName, "AUTO");
        strcpy(gConfig.autoSaveName, "AUTO");
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
    } else {
        FILE *f = fopen(gText, "rb");
        if (f == 0)
            FileError(gText);
        fread(&gConfig, CONFIG_PERSISTED_SIZE, 1, f);
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
    char szScratch[100];

    strcpy(szScratch, "");
    strcpy(szKey, "SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0");
    hKey = 0;
    if (RegOpenKeyA(HKEY_LOCAL_MACHINE, szKey, &hKey) != 0)
        return;
    dwSize = 4;
    if (RegQueryValueExA(hKey, "Music Volume", 0, &dwType,
                         reinterpret_cast<unsigned char *>(&gConfig.musicVolume), &dwSize) != 0) {
        memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
        memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
        strcpy(gConfig.autoLoadName, "AUTO");
        strcpy(gConfig.autoSaveName, "AUTO");
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
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
    RegQueryValueExA(hKey, "Sound Quality", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.musicSource), &dwSize);
    RegQueryValueExA(hKey, "Use Opera", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.useOpera), &dwSize);
    RegQueryValueExA(hKey, "Direct Connect Com Port", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.comPort[CONFIG_CONNECTION_DIRECT]), &dwSize);
    RegQueryValueExA(hKey, "Direct Connect Baud Rate", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.baudRate[CONFIG_CONNECTION_DIRECT]), &dwSize);
    RegQueryValueExA(hKey, "Modem Com Port", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.comPort[CONFIG_CONNECTION_MODEM]), &dwSize);
    RegQueryValueExA(hKey, "Modem Baud Rate", 0, &dwType, reinterpret_cast<unsigned char *>(&gConfig.baudRate[CONFIG_CONNECTION_MODEM]), &dwSize);
    dwSize = 0x63;
    RegQueryValueExA(hKey, "Modem Init String", 0, &dwType, reinterpret_cast<unsigned char *>(gConfig.modemInitString), &dwSize);
    dwSize = 4;
    RegQueryValueExA(hKey, "Unique System ID", 0, &dwType, reinterpret_cast<unsigned char *>(gConfig.uniqueSystemID), &dwSize);
    // Retail relocation at function +0x2f6 resolves to gConfig +0x125, the terminator byte after
    // this four-byte ID, rather than the unrelated modem string at gConfig +0x110.
    gConfig.uniqueSystemID[3] = 0;
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
    memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
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
    int zeroBuffer[25];
    int i;
    int *p = zeroBuffer;
    for (i = 0x19; i != 0; i--) {
        *p = 0;
        p++;
    }
    sprintf(gText, "%s", "HEROES2.CFG");
    int fd = _open(gText, 0x8301, 0x80);
    if (fd != -1) {
        _write(fd, &gConfig, CONFIG_PERSISTED_SIZE);
        _close(fd);
    }
}

VA(0x004c5570, 0x491)
void WritePrefsToRegistry(void)
{
    HKEY hKey;
    char szKey[100];
    char szScratch[100];

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
    RegSetValueExA(hKey, "Sound Quality", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.musicSource), 4);
    RegSetValueExA(hKey, "Use Opera", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.useOpera), 4);
    RegSetValueExA(hKey, "Direct Connect Com Port", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.comPort[CONFIG_CONNECTION_DIRECT]), 4);
    RegSetValueExA(hKey, "Direct Connect Baud Rate", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.baudRate[CONFIG_CONNECTION_DIRECT]), 4);
    RegSetValueExA(hKey, "Modem Com Port", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.comPort[CONFIG_CONNECTION_MODEM]), 4);
    RegSetValueExA(hKey, "Modem Baud Rate", 0, REG_DWORD, reinterpret_cast<unsigned char *>(&gConfig.baudRate[CONFIG_CONNECTION_MODEM]), 4);
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
int IsCDDrive(int driveIndex)
{
    sprintf(gText, "A:\\");
    gText[0] = gText[0] + driveIndex;
    return GetDriveTypeA(gText) == DRIVE_CDROM;
}

VA(0x004c5a60, 0x3ed)
int SetupCDDrive(void)
{
    unsigned char registryPath[100];
    char registryKey[100];
    char cdDrives[26];
    char count;
    int attempts;
    HKEY key;

    sprintf(gText, ".\\DATA\\HEROES2.AGG");
    int file = _open(gText, 0x8000);
    if (file == -1) {
        if (_chdir(gcRegAppPath) == -1)
            return 3;
        file = _open(gText, 0x8000);
        if (file == -1)
            return 4;
    }
    _close(file);

    unsigned long logicalDrives = GetLogicalDrives();
    int cdDriveCount = 0;
    memset(cdDrives, 0, sizeof(cdDrives));
    for (int drive = 2; drive < 26; ++drive) {
        if (logicalDrives & (1 << drive)) {
            sprintf(gText, "A:\\");
            gText[0] += static_cast<char>(drive);
            if (GetDriveTypeA(gText) == DRIVE_CDROM) {
                ++cdDriveCount;
                cdDrives[cdDriveCount - 1] = static_cast<char>(drive);
            }
        }
    }
    count = static_cast<char>(cdDriveCount);

    if (strlen(gcRegCDRomPath) != 0) {
        sprintf(gText, "%s\\heroes2\\anim\\voy24.smk", gcRegCDRomPath);
        file = _open(gText, 0x8000);
        if (file != -1) {
            _close(file);
            sprintf(gText + 2, "%s", gcAnimPath);
            strcpy(gcAnimPath, gText);
            return 0;
        }
    }

    attempts = 0;
    {
        char resultBuffer[256];
        char command[256];
        for (;;) {
            for (int index = 0; index < count; ++index) {
                wsprintfA(command, "open %c: type cdaudio alias CD", cdDrives[index] + 'A');
                if (mciSendStringA(command, resultBuffer, 0xff, 0) == 0) {
                    wsprintfA(command, "info CD UPC wait");
                    mciSendStringA(command, resultBuffer, 0xff, 0);
                    wsprintfA(command, "close CD");
                    mciSendStringA(command, resultBuffer, 0xff, 0);
                }
                sprintf(gText, "%c:\\heroes2\\anim\\voy24.smk", cdDrives[index] + 'A');
                file = _open(gText, 0x8000);
                if (file != -1) {
                    if (_lseek(file, 0, 2) != -1 && _lseek(file, -100, 1) != -1)
                        _read(file, resultBuffer, 100);
                    _close(file);

                    strcpy(registryKey, "SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0");
                    key = 0;
                    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, registryKey, 0, 0x20006, &key) == 0) {
                        wsprintfA(reinterpret_cast<char *>(registryPath), "%c:", cdDrives[index] + 'A');
                        RegSetValueExA(key, "CDDrive", 0, REG_SZ, registryPath,
                                       lstrlenA(reinterpret_cast<char *>(registryPath)) + 1);
                        RegCloseKey(key);
                    }
                    sprintf(gText, "%c:%s", cdDrives[index] + 'A', gcAnimPath);
                    strcpy(gcAnimPath, gText);
                    return 0;
                }
            }
            Sleep(3000);
            ++attempts;
            if (attempts >= 2)
                return 2;
        }
    }
}

VA(0x004c5e50, 0x18)
void BitmapToScreen(class bitmap *bmp)
{
    BlitBitmapToScreen(bmp, 0, 0, bmp->m_width, bmp->m_height, 0, 0);
}

VA(0x004c5e70, 0x3d)
void SetPalette(signed char *paletteData, int updateDisplay)
{
    memcpy(gpBufferPalette->m_data, paletteData, 0x300);
    memcpy(gCyclePal, paletteData + 0x282, 0x60);
    if (updateDisplay != 0)
        UpdatePalette(gpBufferPalette->m_data);
}

VA(0x004c5eb0, 0x25)
void BlitBitmapToScreenNoMouseCheck(class bitmap *bmp, int sourceX, int sourceY, int width,
                                    int height, int destinationX, int destinationY)
{
    BlitBitmapToScreenVesa(reinterpret_cast<int>(bmp), sourceX, sourceY, width, height,
                           destinationX, destinationY);
}

// @match-note
// Structurally complete /O2 checkpoint: base is 0x188 versus retail 0x18b; both use the
// eight-byte frame and same 13-branch/four-blit CFG. External relocation identity/order is exact,
// but base has 23 occurrences versus retail 24. The instruction stream agrees through the IsVis
// test; the first opcode divergence is +0xac, where base loads gpMouseManager into EAX and carries
// it through both axes. Retail loads it into ECX at +0xac (reloc +0xad), then reloads it into EAX
// at +0xcb (reloc +0xcc) for the vertical test. Union-masking relocation fields leaves 270 common
// comparable bytes, 121 different (+0xa8..+0x187; +0xa8 is the shifted local branch), and a
// three-byte retail tail. Using bmp directly on the early call, placing the real width assignment
// between sourceX/sourceY, and declaring savedY before savedX recovered the call, scroll and saved
// coordinate schedules. Flat and fully nested predicates, relational operand reversal, sourceX
// splitting and width-first order were retested; the latter two changed broad register allocation.
// The libclang AST pass found 22 value-preserving variants and retained none after a 30-walk pass.
// At integrated head 5a48d4c, all 65 single AST variants across the 19 exact predecessors and 149
// deterministic two-to-five-variant predecessor combinations were downstream-byte-neutral while
// all 144 Misc symbols were pinned. Moving the two private bound definitions to this function's
// source boundary, or making them function-static, also emitted identical code; the latter only
// changed their COFF identities into unauditable local-static names. Revisit only after a real
// semantic header/type or predecessor change; this CSE residual is not a proven wall.
VA(0x004c5ee0, 0x18b)
void BlitBitmapToScreen(class bitmap *bmp, int sourceX, int sourceY, int width, int height,
                        int destinationX, int destinationY)
{
    bitmap *volatile sourceBitmap = bmp;
    if (gbColorMice == 0) {
        BlitBitmapToScreenVesa(reinterpret_cast<int>(bmp), sourceX, sourceY, width, height,
                               destinationX, destinationY);
        return;
    }
    if (giScrollX != 0 || giScrollY != 0) {
        sourceX = giScrollX + 0x10;
        width = 0x1c0;
        sourceY = giScrollY + 0x10;
        height = 0x1c0;
    }
    gBlitRight = width + destinationX - 1;
    gBlitBottom = height + destinationY - 1;
    if (gpMouseManager->IsVis() != 0 && gBlitRight >= gpMouseManager->m_savedLeft &&
        gpMouseManager->m_cursorRight >= destinationX) {
        if (gBlitBottom >= gpMouseManager->m_savedTop &&
            gpMouseManager->m_cursorBottom >= destinationY) {
            gpMouseManager->SaveAndDraw();
            BlitBitmapToScreenVesa(reinterpret_cast<int>(sourceBitmap), sourceX, sourceY, width, height,
                                   destinationX, destinationY);
            if (gpMouseManager->m_cursorRight > gBlitRight ||
                gpMouseManager->m_savedLeft < destinationX ||
                gpMouseManager->m_cursorBottom > gBlitBottom ||
                gpMouseManager->m_savedTop < destinationY) {
                int savedY = gpMouseManager->m_savedTop;
                int savedX = gpMouseManager->m_savedLeft;
                BlitBitmapToScreenVesa(reinterpret_cast<int>(sourceBitmap), savedX, savedY,
                                       gpMouseManager->m_cursorRight - savedX + 1,
                                       gpMouseManager->m_cursorBottom - savedY + 1, savedX, savedY);
            }
            gpMouseManager->RestoreUnderlying();
            return;
        }
    }
    BlitBitmapToScreenVesa(reinterpret_cast<int>(sourceBitmap), sourceX, sourceY, width, height,
                           destinationX, destinationY);
}

// @match-note
// Structurally complete /O2 checkpoint: the 0x1f4 frame, CFG and 7/7 relocations agree.
// Only base +0x5c..+0x7c differs from retail +0x5c..+0x75: base loads the newline
// word before strlen and addresses via `not ecx`, while retail scans first and
// writes through `[edi-1]`. Direct word stores, strcat, strcpy-at-strlen, memcpy
// and a manual end scan were tried; the combined-TU strcat retest regressed badly.
VA(0x004c6070, 0xa6)
void LogTruncate(void)
{
    char logText[500];
    if (giDebugLevel >= 2) {
        int fileHandle = _open("KB.LOG", 0x4301, 0x80);
        if (fileHandle != -1) {
            strcpy(logText, "===========New Log==========");
            *reinterpret_cast<unsigned short *>(logText + strlen(logText)) =
                *reinterpret_cast<const unsigned short *>("\n");
            _write(fileHandle, logText, strlen(logText));
            _close(fileHandle);
        }
    }
}

// @match-note
// Structurally complete /O2 checkpoint: the 0x1f4 frame, CFG and external targets
// agree. The newline append is base +0x53..+0x73 versus retail
// +0x4f..+0x6c (preloaded word/not-ECX versus post-scan `[edi-1]`); retail's raw
// OutputDebugString IAT operand at +0x8e is not named by delink. The same direct,
// strcpy-at-strlen, memcpy and manual-scan forms were tried; strcat regressed.
VA(0x004c6120, 0x9e)
void LogStr(char *text)
{
    char logText[500];
    if (giDebugLevel >= 2) {
        FILE *f = fopen("KB.LOG", "at+");
        if (f != 0) {
            strcpy(logText, text);
            *reinterpret_cast<unsigned short *>(logText + strlen(logText)) =
                *reinterpret_cast<const unsigned short *>("\n");
            fputs(logText, f);
            fclose(f);
        }
        if (giDebugLevel == 4)
            OutputDebugStringA(logText);
    }
}

// @match-note
// Structurally complete /O2 checkpoint: the 0x2bc frame, all external targets and
// all seven sprintf call/format branches agree. Only the newline
// append differs (base +0x1c9..+0x1f2, retail +0x1c9..+0x1ec), followed by the
// unnamed retail OutputDebugString IAT operand at +0x211. Direct word, strcat,
// strcpy-at-strlen, memcpy and manual end-scan spellings were tried.
VA(0x004c61c0, 0x224)
void LogInt(char *label, int value1, int value2, int value3, int value4, int value5,
            int value6, int value7)
{
    char text[200];
    char logText[500];
    if (value7 != -999)
        sprintf(text, "%s : % 8d % 8d % 8d % 8d % 8d % 8d % 8d", label, value1, value2,
                value3, value4, value5, value6, value7);
    else if (value6 != -999)
        sprintf(text, "%s : % 8d % 8d % 8d % 8d % 8d % 8d", label, value1, value2,
                value3, value4, value5, value6);
    else if (value5 != -999)
        sprintf(text, "%s : % 8d % 8d % 8d % 8d % 8d", label, value1, value2, value3,
                value4, value5);
    else if (value4 != -999)
        sprintf(text, "%s : % 8d % 8d % 8d % 8d", label, value1, value2, value3, value4);
    else if (value3 != -999)
        sprintf(text, "%s : % 8d % 8d % 8d", label, value1, value2, value3);
    else if (value2 != -999)
        sprintf(text, "%s : % 8d % 8d", label, value1, value2);
    else
        sprintf(text, "%s : % 8d", label, value1);
    if (giDebugLevel >= 2) {
        FILE *file = fopen("KB.LOG", "at+");
        if (file != 0) {
            strcpy(logText, text);
            *reinterpret_cast<unsigned short *>(logText + strlen(logText)) =
                *reinterpret_cast<const unsigned short *>("\n");
            fputs(logText, file);
            fclose(file);
            if (giDebugLevel == 4)
                OutputDebugStringA(logText);
        }
    }
}

VA(0x004c63f0, 0x6c)
void AiPrint(char *text)
{
    if (giDebugLevel >= 2) {
        FillBitmapArea(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0);
        smallFont->DrawBoundedString(text, 0, 0x1d0, 0x280, 0x10, 1, 0);
        BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0, 0x1cc);
    }
}

VA(0x004c6460, 0x7a)
void AbsAiPrint(char *text)
{
    int saved = giDebugLevel;
    giDebugLevel = 9;
    FillBitmapArea(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0);
    smallFont->DrawBoundedString(text, 0, 0x1d0, 0x280, 0x10, 1, 0);
    BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0, 0x1cc);
    giDebugLevel = saved;
}

// @match-note
// Structurally complete /O2 checkpoint: the calls, 768-byte loop, CFG and 6/6
// relocations agree. The current combined-TU state reserves 0x314 bytes versus
// retail's 0x310 and moves the palette base by four bytes; the retained source-hash
// maximum had the retail frame. The remaining inner adjustment differs by register
// allocation and threshold scheduling. Signed/unsigned difference, abs/ternary,
// comparison polarity and volatile level forms were tried; volatile regressed.
VA(0x004c64e0, 0xf8)
void FadeTo(unsigned char *source, unsigned char *destination, int increment)
{
    unsigned char colors[0x300];
    memcpy(colors, source, sizeof(colors));
    increment >>= 2;
    if (increment < 1)
        increment = 1;
    int level = 0x30;
    do {
        int delayUntil = KBTickCount() + 0x32;
        PollSound();
        int thresholdIndex = 0x40 - level - increment;
        if (thresholdIndex < 0)
            thresholdIndex = 0;
        unsigned char threshold = giChangeThreshold[thresholdIndex];
        unsigned char *current = colors;
        unsigned char *target = destination;
        int count = 0x300;
        do {
            unsigned char value = *current;
            int difference = static_cast<int>(*target) - static_cast<int>(value);
            int distance = difference < 0 ? -difference : difference;
            if (threshold < distance) {
                distance -= threshold;
                if (difference > 0)
                    *current = static_cast<unsigned char>(value + distance);
                else
                    *current = static_cast<unsigned char>(value - distance);
            }
            ++current;
            ++target;
            --count;
        } while (count != 0);
        UpdatePalette(reinterpret_cast<signed char *>(colors));
        DelayTil(&delayUntil);
        level += increment;
    } while (level < 0x40);
    UpdatePalette(reinterpret_cast<signed char *>(destination));
}

// @match-note
// Structurally complete /O2 checkpoint: the 0x304 frame, complete two-loop CFG and
// all 8 relocations agree. Differences are limited to palette translation
// +0x22..+0x51 and screen remapping +0x65..+0x87: retail holds the row count in
// EDI and advances EDX, while base uses EDX and advances EAX. Indexed output,
// pre/post-increment output, explicit row pointers and a single linear pointer
// loop were tried; revisit only after a new predecessor/header TU state.
VA(0x004c65e0, 0xb8)
void FadeToColorTable(unsigned char *colorTable, int increment)
{
    unsigned char translatedPalette[0x300];
    int savedUpdateFlags = gpWindowManager->m_updateFlags;
    gpWindowManager->m_updateFlags = 0;
    signed char *paletteData = gpBufferPalette->m_data;
    unsigned char *output = translatedPalette;
    int index = 0;
    do {
        int paletteIndex = colorTable[index++] * 3;
        output += 3;
        output[-3] = paletteData[paletteIndex];
        output[-2] = paletteData[paletteIndex + 1];
        output[-1] = paletteData[paletteIndex + 2];
    } while (output < translatedPalette + sizeof(translatedPalette));
    FadeTo(reinterpret_cast<unsigned char *>(paletteData), translatedPalette, increment);
    unsigned char *pixel = gpWindowManager->m_screen->m_pixels;
    int rows = 0x1e0;
    do {
        int columns = 0x280;
        do {
            *pixel = colorTable[*pixel];
            ++pixel;
            --columns;
        } while (columns != 0);
        --rows;
    } while (rows != 0);
    gpWindowManager->UpdateScreen();
    UpdatePalette(paletteData);
    gpWindowManager->m_updateFlags = savedUpdateFlags;
}

VA(0x004c66a0, 0x29)
int IsCycleColor(int color)
{
    if ((color >= 0xD6 && color <= 0xDD) || (color >= 0xE7 && color <= 0xED)) {
        return 1;
    }
    return 0;
}

// @match-note
// Structurally complete /O2 checkpoint: the 0x8c frame and all 14
// relocations and every open/write/alloc/free/close target agree. Divergences are
// confined to the RLE loop (base +0x67..+0xe0 / retail +0x67..+0xdf), where EBX
// and EDI swap encoded-size/run-end roles, and palette scaling +0x19a..+0x1ae,
// where equivalent indexed operands are commuted. `<2`, `==1`, and `<=1` run
// tests, reordered locals, predicate polarity and commuted SIB forms were tried.
VA(0x004c66d0, 0x1ee)
void CreatePCXFile(char *filename, unsigned char *pixels, int width, int height,
                   unsigned char *paletteData)
{
    PCXHeader header;
    memset(&header, 0, sizeof(header));
    header.manufacturer = 10;
    header.version = 5;
    header.encoding = 1;
    header.bitsPerPixel = 8;
    header.xMax = static_cast<unsigned short>(width - 1);
    header.yMax = static_cast<unsigned short>(height - 1);
    header.planes = 1;
    header.bytesPerLine = static_cast<unsigned short>(width);
    header.paletteType = 1;
    int file = _open(filename, 0x8301, 0x80);
    if (file == -1)
        return;
    _write(file, &header, sizeof(header));
    unsigned char *encoded = static_cast<unsigned char *>(
        H2_ALLOC(width * 2, "I:\\Projects\\Heroes\\Prog\\BASE\\Misc.cpp", 0x5c8));
    for (int row = 0; row < height; ++row) {
        int sourceIndex = 0;
        unsigned int encodedSize = 0;
        while (sourceIndex < width) {
            unsigned char value = pixels[sourceIndex];
            int runEnd = sourceIndex;
            while (runEnd < width && pixels[runEnd] == value && runEnd - sourceIndex + 1 < 0x40)
                ++runEnd;
            int runLength = runEnd - sourceIndex;
            if (runLength <= 1 && (value & 0xc0) != 0xc0) {
                encoded[encodedSize++] = value;
                ++sourceIndex;
            } else {
                encoded[encodedSize++] = static_cast<unsigned char>(runLength | 0xc0);
                encoded[encodedSize++] = value;
                sourceIndex += runLength;
            }
        }
        _write(file, encoded, encodedSize);
        pixels += width;
    }
    H2_FREE(encoded, "I:\\Projects\\Heroes\\Prog\\BASE\\Misc.cpp", 0x5f0);
    unsigned char paletteMarker = 0x0c;
    _write(file, &paletteMarker, 1);
    unsigned char *outputPalette = static_cast<unsigned char *>(
        H2_ALLOC(0x300, "I:\\Projects\\Heroes\\Prog\\BASE\\Misc.cpp", 0x5f6));
    for (int i = 0; i < 0x300; ++i)
        outputPalette[i] = paletteData[i] << 2;
    _write(file, outputPalette, 0x300);
    H2_FREE(outputPalette, "I:\\Projects\\Heroes\\Prog\\BASE\\Misc.cpp", 0x5fb);
    _close(file);
}

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
    IconEntry *entries = reinterpret_cast<IconEntry *>(iconPtr->m_data);
    return &entries[index];
}

// @match-note
// Structurally complete /O2 checkpoint: all three relocations and the complete seeded
// random CFG agree.  The current prefix matches through +0x43; base then uses EAX for
// the feedback term and stores iLastSeed before range division, while retail uses ECX,
// folds the range LEA first, and publishes the seed after division.  Direct term locals,
// folded arithmetic, explicit bit-loop forms, and a volatile seed store were checked.
// The broad score movement followed a required header-state change; do not retune it
// until the shared/TU declaration state is stable.
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

// @match-note
// Structurally complete /O2 checkpoint: base is 0x57 bytes, retail 0x5c and both
// relocations agree. The complete +0x00..return span differs only in allocation:
// base preserves EDI and uses EDX/ESI for terms; retail preserves EBP and uses
// ESI/EDX. Split term locals and folded multiply/add expressions were checked;
// the generated seed arithmetic and final store remain instruction-equivalent.
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

// @match-note
// Structurally complete /O2 checkpoint: base and retail are both 0x48 bytes and
// all 3 relocations agree. Every byte matches except the six-byte iLastSeed store:
// base +0x30..+0x35 places it before the bit loop; retail +0x3e..+0x43 places it
// after `dec ecx` in the loop. for/do-while placement and a volatile store were
// tried; volatile spills the mix and caused broad TU regressions, while nonvolatile
// forms are hoisted. Revisit only after a new predecessor/header TU state.
VA(0x004c6a60, 0x48)
int SGenRand(void)
{
    int result = 0;
    iLastSeed &= 0xfff;
    int mix = iLastSeed * 7;
    mix += (mix & 0xff0) >> 4;
    int i = 31;
    do {
        if (mix & (1 << i)) {
            result |= 1 << i;
        }
        --i;
        iLastSeed = mix;
    } while (i >= 0);
    return result;
}

VA(0x004c6ab0, 0x6)
int MemSize(int)
{
    return 0x3ea2;
}

// @match-note
// Structurally aligned /O2 checkpoint: both code spans are 0x386, the 0x9c frame,
// CFG and all 59 relocations agree. The recovered conditional Y adjustment leaves
// only 12 unmasked bytes: one `mov ecx,0x2d` schedule at +0xbc and swapped LEAs for
// entryText/message near +0x141, plus three delinked empty-string owner names.
// Boolean-mask/branched Y forms, direct/local empty strings and declaration order
// were tried. Resume with exact-preserving predecessor/TU state, then AST last mile.
VA(0x004c6ac0, 0x386)
void GetDataEntry(char *prompt, char *destination, int maximumLength, char *initialText,
                  int showCancel, int useImmediateHandler)
{
    int savedCursorType = gpMouseManager->m_cursorType;
    int savedCursorFrame = gpMouseManager->m_cursorFrame;
    while (gpMouseManager->m_hideCount != 0)
        gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);

    cDEDest = destination;
    iDEMaxLen = maximumLength;
    strcpy(destination, "");

    int rows = bigFont->LineLength(prompt, 0xf0) * 0x10;
    if (showCancel != 0)
        rows += 0x27;
    rows = (rows + 0xf) / 0x2d;
    if (rows > 6)
        rows = 6;
    int entryY = rows * 0x2d - (showCancel != 0 ? 0x1e : 0) + 0x5f;

    char windowName[16];
    sprintf(windowName, "evntwin%d.bin", rows);
    DataEntryWin = new heroWindow(0xb1, 0x14, windowName);
    if (DataEntryWin == 0)
        MemError();

    tag_message message;
    message.type = 0x200;
    message.payload.widget.command = 3;
    message.payload.widget.id = 1;
    message.payload.widget.data.text = prompt;
    DataEntryWin->BroadcastMessage(message);

    char entryText[100];
    if (initialText == 0)
        initialText = "";
    strcpy(entryText, initialText);
    message.payload.widget.id = 10;
    message.payload.widget.data.text = entryText;
    DataEntryWin->BroadcastMessage(message);
    strcpy(destination, entryText);

    message.type = 0x200;
    message.payload.widget.command = 6;
    message.payload.widget.id = 0x7801;
    message.payload.widget.data.text = reinterpret_cast<char *>(6);
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = 0x7807;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = 0x7808;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = 0x7805;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = 0x7806;
    DataEntryWin->BroadcastMessage(message);
    if (showCancel == 0) {
        message.payload.widget.id = 0x7802;
        DataEntryWin->BroadcastMessage(message);
    }

    textEntryWidget *entry = new textEntryWidget(
        0x23, static_cast<short>(entryY), 0xfb, 0x14, static_cast<short>(maximumLength),
        destination, "bigfont.fnt", 0, "buybuild.icn", 3, 10, 0, 4, 10, 3);
    if (entry == 0)
        MemError();
    inBoxY = entryY + 0x17;
    inBoxX = 0xd5;
    DataEntryWin->AddWidget(entry, -1);

    if (useImmediateHandler != 0) {
        bDataEntryTime = 0;
        gbAllowTextEntryEscape = 0;
    } else
        bDataEntryTime = 2;
    gpWindowManager->DoDialog(DataEntryWin, DataEntryWindowHandler, 0);
    delete DataEntryWin;
    gpMouseManager->SetPointer("", savedCursorFrame, savedCursorType);
    gbAllowTextEntryEscape = 1;
}

// @match-note
// Structurally complete /O2 checkpoint: all 23 relocations and the complete
// broadcast/copy/draw/dialog-result path agree. Moving the possible-cancel tail
// before the normal fallback recovered retail body order and shortened base to
// 0x16f versus retail's 0x173. The remaining difference is widget id 0x7802 being
// hoisted into the initial command dispatch instead of emitted in the tail. Direct
// returns regressed badly; a volatile tail read emitted no change and was removed.
VA(0x004c6e50, 0x173)
int DataEntryWindowHandler(struct tag_message &message)
{
    if (bDataEntryTime == 0) {
        ++bDataEntryTime;
        message.type = 8;
        message.payload.mouse.x = inBoxX;
        message.payload.mouse.y = inBoxY;
        DataEntryWin->BroadcastMessage(message);
        return 1;
    }

    if (bDataEntryTime == 1)
        ++bDataEntryTime;
    else {
        if (message.type != 0x200)
            goto normalEvent;
        if (message.payload.widget.command != 0xc) {
            if (message.payload.widget.command == 0xd)
                goto possibleCancelEvent;
            goto normalEvent;
        }
        if (message.payload.widget.id != 10)
            goto normalEvent;
    }

    message.type = 0x200;
    message.payload.widget.id = 0xA;
    message.payload.widget.command = 7;
    DataEntryWin->BroadcastMessage(message);
    if (strlen(message.payload.widget.data.text) != 0) {
        memset(cDEDest, 0, iDEMaxLen);
        strncpy(cDEDest, message.payload.widget.data.text, iDEMaxLen - 1);
        message.type = 0x200;
        message.payload.widget.command = 3;
        message.payload.widget.id = 10;
        message.payload.widget.data.text = cDEDest;
        DataEntryWin->BroadcastMessage(message);
        DataEntryWin->DrawWindow(1, 10, 10);
        if (gbTextEntryEscaped == 0) {
            gpWindowManager->m_dialogResult = message.payload.widget.id;
            message.payload.widget.id = 10;
            message.payload.widget.command = 10;
            return 2;
        }
    }

possibleCancelEvent:
    if (message.payload.widget.id != 0x7802)
        goto normalEvent;
    message.payload.widget.id = 10;
    message.payload.widget.command = 10;
    return 2;

normalEvent:
    return EventWindowHandler(message);
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
