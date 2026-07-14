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

static int giFindMid;




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

// @early-stop
// /O2 intrinsic-shape wall: base COMDAT .text is 0x202 bytes versus retail 0x20f.
// The complete allocation/tracking flow and calls match (LogInt, malloc x2, MemError,
// sprintf, fopen/fputs/fclose, OutputDebugStringA). The remaining source divergence is
// the newline append at base +0x1ab..+0x1cb versus retail +0x1b2..+0x1d5: base loads
// "\n" before `repne scasb` and addresses the end through `not ecx`; retail scans first,
// loads the same relocated word afterward, and stores it at `[edi-1]`. `strcat`,
// `strcpy(buf+strlen)`, `memcpy(...,2)`, direct/named word stores, volatile loads, and a
// manual end scan were compiled; none selected retail's hybrid intrinsic sequence.
VA(0x004c3d70, 0x20f)
void *BaseAlloc(unsigned int size, char *file, int line)
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
            strcpy(gpMemEntry[i].file, file);
            gpMemEntry[i].line = line;
            i = 99999;
        }
    }
    if (giDebugLevel == 4) {
        sprintf(text, "KBAlloc    Size %d   Ptr %d   File %s  Line %d", size, ptr, file, line);
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

// @early-stop
// /O2 intrinsic-shape wall: base COMDAT .text is 0x38f bytes versus retail 0x386.
// The three equivalent newline-append clusters load the same `"\n"` relocation at
// base +0x105/+0x23a/+0x330 (reloc operands +0x108/+0x23d/+0x333) versus retail
// +0x113/+0x244/+0x338 (operands +0x116/+0x247/+0x33b); retail keeps the post-scan
// pointer in EDI while base materializes the strlen result. All LogInt/malloc/free,
// sprintf, fopen/fputs/fclose and OutputDebugStringA sites and tracked MemEntry fields
// agree. The same six append spellings documented on BaseAlloc were exhausted.
VA(0x004c3f80, 0x386)
void BaseFree(void *ptr, char *file, int line)
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
        sprintf(gText, "Bad Delete,  File '%13s'  Line % 4d, ptr %12d", file, line, ptr);
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

// @early-stop
// /O2 intrinsic-shape wall: base COMDAT .text is 0x137 bytes versus retail 0x134.
// Only the completed log-line append differs: base loads `"\n"` at +0xd6 (reloc
// +0xd9) before deriving `buf+strlen`, while retail loads it at +0xe3 (reloc +0xe6)
// after `repne scasb` and stores through `[edi-1]`. LogInt, sprintf, fopen/fputs/
// fclose and OutputDebugStringA targets are otherwise identical. `strcat`, strlen+
// strcpy/memcpy, direct and named word stores, volatile load, and manual scan tried.
VA(0x004c4310, 0x134)
void PrintMemoryLeaks(void)
{
    char local_1f4[500];
    if (giDebugLevel >= 1 && gpMemEntry != 0) {
        LogInt("Total Memory Leaks", iMemEntries, -999, -999, -999, -999, -999, -999);
        int i = 0;
        do {
            if (gpMemEntry[i].used != 0) {
                sprintf(gText, "Memory Leak,  File '%13s'  Line % 4d, ptr %12d   size %6d",
                        gpMemEntry[i].file, gpMemEntry[i].line, reinterpret_cast<int>(gpMemEntry[i].ptr),
                        gpMemEntry[i].size);
                if (giDebugLevel >= 2) {
                    FILE *_File = fopen("KB.LOG", "at+");
                    if (_File != 0) {
                        strcpy(local_1f4, gText);
                        *reinterpret_cast<unsigned short *>(local_1f4 + strlen(local_1f4)) =
                            *reinterpret_cast<const unsigned short *>("\n");
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

// @early-stop
// /O2 flag-reuse wall: base COMDAT .text is 0x97 bytes versus retail 0x95.
// Base alone emits `3b c7` (`cmp eax,edi`) at +0x38..+0x39 before the equality
// `jge`; retail reuses flags from the identical +0x2a comparison. Everything after
// that differs only by the resulting two-byte jump displacements. There are no calls;
// all four giFindMid relocations resolve to the same 0x5331c0 storage (retail delinks
// it as const_001331c0). Direct field tests, saved int/ushort values, nested ==/>=,
// reversed relational operands, comma/combined conditions, and three-way forms tried.
VA(0x004c4540, 0x95)
int FindIndex(struct indexArray *entries, int low, int high, int key)
{
    giFindMid = (low + high) >> 1;
    while (high - low > 1) {
        int value = entries[giFindMid].field0;
        if (value > key) {
            high = giFindMid;
        } else {
            low = giFindMid;
            if (value >= key)
                return entries[low].field2;
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

// @early-stop
// /O2 register/instruction-selection wall: base COMDAT .text is 0xee bytes versus
// retail 0xea. Register allocation differs throughout +0x0b..+0x68 (base palette/done/
// level ESI/EBX/EDI; retail EBX/EBP/ESI). The value update at base +0x92..+0xc3 uses
// `lea eax,[eax+edi]; sub al,0x3f`, while retail +0x92..+0xbf uses threshold EAX and
// `sub cl,al`, accounting for the four-byte size delta; later call offsets shift by
// four only. All seven callees and 11 relocations agree, including gConfig+0x30 (retail
// delink name const_00128d50). Separate/repeated color loads, signed/unsigned locals,
// threshold locals, branch inversion, update ordering, and explicit pointer init tried.
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
            for (int i = 0; i < 0x300; ++i) {
                signed char color = gpBufferPalette->m_data[i];
                if (color > 0x3f - level)
                    fadePalette->m_data[i] = color - (0x3f - level);
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

// @early-stop
// /O2 register-allocation wall with equal 0x66-byte sections. Base +0x09..+0x34
// retains text/count in EBX/EBP; retail uses EBP/EBX and schedules the two intrinsic
// strlen scans differently. Equivalent SIB bytes consequently differ at +0x3c..+0x3e
// and +0x5b..+0x5d; the loop backedge at +0x4d..+0x50 is `cmp esi,ebp; jl` versus
// retail `cmp ebx,esi; jg`. The sole call is strncmp at +0x40 with the same relocation
// and arguments. for/while/do forms, count|0, >=1+i, operand swaps and AST permutations
// were exhausted without changing the allocation.
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

// @early-stop
// /O2 relational-encoding wall with equal 0x31-byte sections and no relocations/calls.
// The only bytes that differ are +0x21..+0x24: base `3b c1 7c f4`
// (`cmp eax,ecx; jl`) versus retail `3b c8 7f f4` (`cmp ecx,eax; jg`). They are the
// same signed `index < length` condition. for/while/do loops, explicit backedges,
// `length|0`, `>= 1+index`, reversed operands/returns, SIB spelling, and the AST
// relational permuter all retained this canonical base encoding.
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
    memset(&gConfig, 0, 0x19d);
    strcpy(gConfig.autoLoadName, "AUTO");
    strcpy(gConfig.autoSaveName, "AUTO");
    gConfig.soundQuality = 1;
}

// @early-stop
// /O2 induction/register wall: base COMDAT .text is 0x1b9 bytes versus retail 0x1b5.
// At +0x03..+0x68 base induces from exeGfxConfig start and uses positive offsets;
// retail +0x03..+0x6c induces from `fullScreen` and uses -0x14..+4, while writing the
// same two records/fields and constants. The later four-byte skew is scheduling only
// (notably default-name setup at base +0xd2..+0x123 versus retail +0xd4..+0x147);
// all four rand and three KBTickCount calls match. Relocations resolve to the same
// gConfig field VAs despite delinked const/string labels (`homm2 relocs`: base 43,
// retail 42; helper-only 0x128d3c/0x128d74/0x128dca are respectively the loop anchor
// and direct fields visible under retail's const labels). Indexed/pointer loops, field
// orders, <641/>640/<=640 branches, and split/combined random-tick expressions tried.
VA(0x004c49a0, 0x1b5)
void SetGameDefaults(void)
{
    gConfig.musicVolume = 1;
    gConfig.soundVolume = 1;
    gConfig.autosave = 1;
    gConfig.showRoute = 1;
    exeGfxConfig *gfx = gConfig.gfx;
    do {
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
        ++gfx;
    } while (gfx < gConfig.gfx + 2);
    gConfig.showCombatGrid = 0;
    gConfig.showCombatMouseHex = 0;
    gConfig.combatShadeLevel = 0;
    gConfig.combatArmyInfoLevel = 0;
    gConfig.evilInterfaceUsage = 0;
    gConfig.useOpera = 1;
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
    gConfig.soundQuality = 3;
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

// @early-stop
// Relocation/delink identity only: base and retail are both 0x7ab bytes and the
// relocation-masked instruction stream is identical. Both have 126 relocations;
// the five `homm2 relocs` exceptions are gConfig members at retail RVA
// 0x128d20 (two uses), 0x128e10, 0x128e30 and 0x128e60, delinked as anonymous
// `const_`/string-owner labels. The other displayed spans are empty-string local
// labels and raw IAT operands. Direct member, array-index and named-local shapes
// were checked; changing them only changes delink ownership, not bytes.
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
    gConfig.modemInitString[98] = 0;
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

// @early-stop
// Relocation/delink identity only: base and retail are both 0x491 bytes with an
// identical relocation-masked instruction stream and 90/90 matching relocation
// targets. The diff consists solely of gConfig member labels, three raw registry
// IAT operands, and the empty-string local label. Direct field and indexed-field
// spellings were checked; they preserve bytes and only rename delinked owners.
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

// @early-stop
// reloc-masked: base COMDAT .text is 0x3fc including tail padding versus the retail
// 0x3ed code symbol; every instruction within the retail span is identical. Only
// +0x16a..+0x16d names the same
// `gText + 2` operand as `const_0012899a` in the delinked retail object. Retail's raw
// IAT operands also lack several import relocation names; the audit pairs all 51/51
// relocations with only-base=0 and every call opcode/target slot is unchanged.
VA(0x004c5a60, 0x3ed)
int SetupCDDrive(void)
{
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
    char cdDrives[26];
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
    char count = static_cast<char>(cdDriveCount);

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

    int attempts = 0;
    int resultBuffer[64];
    char command[256];
    for (;;) {
        for (int index = 0; index < count; ++index) {
            wsprintfA(command, "open %c: type cdaudio alias CD", cdDrives[index] + 'A');
            if (mciSendStringA(command, reinterpret_cast<char *>(resultBuffer), 0xff, 0) == 0) {
                wsprintfA(command, "info CD UPC wait");
                mciSendStringA(command, reinterpret_cast<char *>(resultBuffer), 0xff, 0);
                wsprintfA(command, "close CD");
                mciSendStringA(command, reinterpret_cast<char *>(resultBuffer), 0xff, 0);
            }
            sprintf(gText, "%c:\\heroes2\\anim\\voy24.smk", cdDrives[index] + 'A');
            file = _open(gText, 0x8000);
            if (file != -1) {
                if (_lseek(file, 0, 2) != -1 && _lseek(file, -100, 1) != -1)
                    _read(file, resultBuffer, 100);
                _close(file);

                char registryKey[100];
                strcpy(registryKey, "SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0");
                HKEY key = 0;
                if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, registryKey, 0, 0x20006, &key) == 0) {
                    unsigned char registryPath[100];
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

// @early-stop
// /O2 register-allocation wall: base is 0x17b bytes, retail 0x18b, with all 24
// relocation targets agreeing. The divergent instruction spans are base
// +0x06..+0x178 / retail +0x06..+0x188: retail spills the bitmap at entry and
// assigns EBX/ESI/EDI to the two destination coordinates differently. All four
// VESA sites and IsVis/SaveAndDraw/RestoreUnderlying calls, overlap branches,
// and rectangle fields agree. Parameter copies, direct parameters, saved-coordinate
// locals and reordered overlap expressions were tried; they only permute registers.
VA(0x004c5ee0, 0x18b)
void BlitBitmapToScreen(class bitmap *bmp, int sourceX, int sourceY, int width, int height,
                        int destinationX, int destinationY)
{
    if (gbColorMice == 0) {
        BlitBitmapToScreenVesa(reinterpret_cast<int>(bmp), sourceX, sourceY, width, height,
                               destinationX, destinationY);
        return;
    }
    if (giScrollX != 0 || giScrollY != 0) {
        sourceX = giScrollX + 0x10;
        sourceY = giScrollY + 0x10;
        width = 0x1c0;
        height = 0x1c0;
    }
    gBlitRight = width + destinationX - 1;
    gBlitBottom = height + destinationY - 1;
    if (gpMouseManager->IsVis() != 0 && gpMouseManager->m_savedW <= gBlitRight &&
        gpMouseManager->field_0x6e >= destinationX && gpMouseManager->m_savedH <= gBlitBottom &&
        gpMouseManager->field_0x72 >= destinationY) {
        gpMouseManager->SaveAndDraw();
        BlitBitmapToScreenVesa(reinterpret_cast<int>(bmp), sourceX, sourceY, width, height,
                               destinationX, destinationY);
        if (gpMouseManager->field_0x6e <= gBlitRight &&
            gpMouseManager->m_savedW >= destinationX &&
            gpMouseManager->field_0x72 <= gBlitBottom &&
            gpMouseManager->m_savedH < destinationY) {
            int savedX = gpMouseManager->m_savedW;
            int savedY = gpMouseManager->m_savedH;
            BlitBitmapToScreenVesa(reinterpret_cast<int>(bmp), savedX, savedY,
                                   gpMouseManager->field_0x6e - savedX + 1,
                                   gpMouseManager->field_0x72 - savedY + 1, savedX, savedY);
        }
        gpMouseManager->RestoreUnderlying();
        return;
    }
    BlitBitmapToScreenVesa(reinterpret_cast<int>(bmp), sourceX, sourceY, width, height,
                           destinationX, destinationY);
}

// @early-stop
// /O2 intrinsic wall: base is 0xa9 bytes, retail 0xa6; all 7 relocations agree.
// Only base +0x5c..+0x7c differs from retail +0x5c..+0x75: base loads the newline
// word before strlen and addresses via `not ecx`, while retail scans first and
// writes through `[edi-1]`. Direct word stores, strcat, strcpy-at-strlen, memcpy
// and a manual end scan were tried; none select retail's hybrid intrinsic.
VA(0x004c6070, 0xa6)
void LogTruncate(void)
{
    char local_1f4[500];
    if (giDebugLevel >= 2) {
        int _FileHandle = _open("KB.LOG", 0x4301, 0x80);
        if (_FileHandle != -1) {
            strcpy(local_1f4, "===========New Log==========");
            *reinterpret_cast<unsigned short *>(local_1f4 + strlen(local_1f4)) =
                *reinterpret_cast<const unsigned short *>("\n");
            _write(_FileHandle, local_1f4, strlen(local_1f4));
            _close(_FileHandle);
        }
    }
}

// @early-stop
// /O2 intrinsic/delink wall: base is 0xa4 bytes, retail 0x9e; all 8 relocation
// targets agree. The newline append is base +0x53..+0x73 versus retail
// +0x4f..+0x6c (preloaded word/not-ECX versus post-scan `[edi-1]`); retail's raw
// OutputDebugString IAT operand at +0x8e is not named by delink. The same direct,
// strcat, strcpy-at-strlen, memcpy and manual-scan append shapes were exhausted.
VA(0x004c6120, 0x9e)
void LogStr(char *param_1)
{
    char local_1f4[500];
    if (giDebugLevel >= 2) {
        FILE *f = fopen("KB.LOG", "at+");
        if (f != 0) {
            strcpy(local_1f4, param_1);
            *reinterpret_cast<unsigned short *>(local_1f4 + strlen(local_1f4)) =
                *reinterpret_cast<const unsigned short *>("\n");
            fputs(local_1f4, f);
            fclose(f);
        }
        if (giDebugLevel == 4)
            OutputDebugStringA(local_1f4);
    }
}

// @early-stop
// /O2 intrinsic/delink wall: base is 0x22a bytes, retail 0x224; all 22 relocation
// targets and all seven sprintf call/format branches agree. Only the newline
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
void AiPrint(char *param_1)
{
    if (giDebugLevel >= 2) {
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

// @early-stop
// /O2 register-allocation wall: base is 0xfa bytes, retail 0xf8 and all 6
// relocations agree. The sole divergent instruction span is the inner color
// adjustment at base +0x49..+0x85 / retail +0x47..+0x81: threshold/value use
// EAX/ECX/EBX in a different permutation and the equivalent comparison is
// reversed. Signed difference, unsigned value, abs/ternary, and both comparison
// orientations were tried; the current form retains the 95.12% maximum.
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

// @early-stop
// /O2 register-allocation wall: base and retail are both 0xb8 bytes and all 8
// relocations agree. Differences are limited to palette translation
// +0x22..+0x51 and screen remapping +0x65..+0x87: retail holds the row count in
// EDI and advances EDX, while base uses EDX and advances EAX. Indexed output,
// pre/post-increment output, explicit row pointers and a single linear pointer
// loop were tried; the linear form gives the retained 87.18% maximum.
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

// @early-stop
// /O2 register-allocation wall: base is 0x1ec bytes, retail 0x1ee; all 14
// relocations and every open/write/alloc/free/close target agree. Divergences are
// confined to the RLE loop (base +0x67..+0xe0 / retail +0x67..+0xdf), where EBX
// and EDI swap encoded-size/run-end roles, and palette scaling +0x19a..+0x1ae,
// where equivalent indexed operands are commuted. `<2`, `==1`, and `<=1` run
// tests plus reordered loop locals were tried; `<=1` retains 98.04%.
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
    IconEntry *entries = *(IconEntry **)((char *)iconPtr + 0x12);
    return &entries[index];
}

// @early-stop
// /O2 register-allocation wall: base is 0xb7 bytes, retail 0xb8 and all 3
// relocations agree. The semantic body is identical; base +0x1d..+0xb4 versus
// retail +0x1d..+0xb5 assigns seed/mix/result to EDI/EBX/EAX instead of
// ECX/EDI/EAX and schedules the final seed store before division. Direct term
// locals, folded arithmetic and explicit bit-loop forms were checked; they only
// rotate registers or move the invariant store.
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

// @early-stop
// /O2 register-allocation wall: base is 0x58 bytes, retail 0x5c and both
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

// @early-stop
// /O2 invariant-store scheduling wall: base and retail are both 0x48 bytes and
// all 3 relocations agree. Every byte matches except the six-byte iLastSeed store:
// base +0x30..+0x35 places it before the bit loop; retail +0x3e..+0x43 places it
// after `dec ecx` in the loop. for/do-while placement and a volatile store were
// tried; volatile spills the mix and drops the match to 53%, while nonvolatile
// forms are hoisted and retain 90.64%.
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

// @early-stop
// /O2 local-allocation/delink wall: base is 0x381 bytes, retail 0x386; all 59
// relocation targets agree. Differences are the three empty-string local labels,
// row/entry-Y arithmetic at retail +0xbc..+0x102, and swapped LEA roles for the
// entry buffer/message near +0x185..+0x1c6. All broadcasts, constructors,
// MemError calls, widget arguments, dialog/delete and cursor restoration agree.
// Branched and boolean-mask Y adjustment, direct/local empty strings, and local
// declaration reorderings were tried; the current form retains 95.90%.
VA(0x004c6ac0, 0x386)
void GetDataEntry(char *prompt, char *destination, int maximumLength, char *initialText,
                  int showCancel, int useImmediateHandler)
{
    int savedCursorType = gpMouseManager->m_cursorType;
    int savedCursorFrame = gpMouseManager->m_cursorFrame;
    while (gpMouseManager->m_hideCount != 0)
        gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer("advmice.mse", 0, -999);

    cDEDest = destination;
    iDEMaxLen = maximumLength;
    strcpy(destination, "");

    int rows = bigFont->LineLength(prompt, 0xf0) * 0x10;
    if (showCancel != 0)
        rows += 0x27;
    rows = (rows + 0xf) / 0x2d;
    if (rows > 6)
        rows = 6;
    int entryY = rows * 0x2d - (((showCancel == 0) - 1) & 0x1e) + 0x5f;

    char windowName[16];
    sprintf(windowName, "evntwin%d.bin", rows);
    DataEntryWin = new heroWindow(0xb1, 0x14, windowName);
    if (DataEntryWin == 0)
        MemError();

    tag_message message;
    message.type = 0x200;
    message.field4 = 3;
    message.field8 = 1;
    message.text = prompt;
    DataEntryWin->BroadcastMessage(message);

    char entryText[100];
    if (initialText == 0)
        initialText = "";
    strcpy(entryText, initialText);
    message.field8 = 10;
    message.text = entryText;
    DataEntryWin->BroadcastMessage(message);
    strcpy(destination, entryText);

    message.type = 0x200;
    message.field4 = 6;
    message.field8 = 0x7801;
    message.text = reinterpret_cast<char *>(6);
    DataEntryWin->BroadcastMessage(message);
    message.field8 = 0x7807;
    DataEntryWin->BroadcastMessage(message);
    message.field8 = 0x7808;
    DataEntryWin->BroadcastMessage(message);
    message.field8 = 0x7805;
    DataEntryWin->BroadcastMessage(message);
    message.field8 = 0x7806;
    DataEntryWin->BroadcastMessage(message);
    if (showCancel == 0) {
        message.field8 = 0x7802;
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

// @early-stop
// /O2 block-layout wall: base is 0x17b bytes, retail 0x173; all 23 relocations
// agree. The only differing instruction blocks are the field4==13/cancel dispatch
// (retail +0x4d..+0x70) and its equivalent tail block (+0x15a..+0x172); the entire
// broadcast/copy/draw/dialog-result path between them matches. Early returns,
// shared fallback labels, combined predicates and a late possible-cancel label
// were tried; MSVC continues to invert and reorder these equivalent blocks.
VA(0x004c6e50, 0x173)
int DataEntryWindowHandler(struct tag_message &message)
{
    if (bDataEntryTime == 0) {
        ++bDataEntryTime;
        message.type = 8;
        message.field4 = inBoxX;
        message.field8 = inBoxY;
        DataEntryWin->BroadcastMessage(message);
        return 1;
    }

    if (bDataEntryTime == 1)
        ++bDataEntryTime;
    else {
        if (message.type != 0x200)
            goto normalEvent;
        if (message.field4 != 0xc) {
            if (message.field4 == 0xd)
                goto possibleCancelEvent;
            goto normalEvent;
        }
        if (message.field8 != 10)
            goto normalEvent;
    }

    message.type = 0x200;
    message.field8 = 0xA;
    message.field4 = 7;
    DataEntryWin->BroadcastMessage(message);
    if (strlen(message.text) != 0) {
        memset(cDEDest, 0, iDEMaxLen);
        strncpy(cDEDest, message.text, iDEMaxLen - 1);
        message.type = 0x200;
        message.field4 = 3;
        message.field8 = 10;
        message.text = cDEDest;
        DataEntryWin->BroadcastMessage(message);
        DataEntryWin->DrawWindow(1, 10, 10);
        if (gbTextEntryEscaped == 0) {
            gpWindowManager->m_dialogResult = message.field8;
            message.field8 = 10;
            message.field4 = 10;
            return 2;
        }
    }

normalEvent:
    return EventWindowHandler(message);

possibleCancelEvent:
    if (message.field8 != 0x7802)
        goto normalEvent;
    message.field8 = 10;
    message.field4 = 10;
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
