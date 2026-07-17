// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Misc.obj   from: .\basewin.lib
// functions: 46   data: 11
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#define HOMM2_MISC_INLINE_ICONENTRY
#include <va.h>
#include <SOURCE/kbwin.h>
#include <BASE/heroWindow.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/icon.h>
#include <BASE/bmap2.h>
#include <BASE/font.h>
#include <BASE/textEntryWidget.h>
#include <BASE/Misc.h>
#include <BASE/MiscConstants.h>
#include <BASE/MISC_TYPES.h>
#undef HOMM2_MISC_INLINE_ICONENTRY
#include <BASE/miscwin.h> // this TU's own free functions + indexArray/IconEntry
#include <SOURCE/KB.h>    // EventWindowHandler, FileError, ShutDown
#include <SOURCE/wingraph.h>
#include <SOURCE/NOOPT.h> // SetFullScreenStatus
#include <BASE/message.h> // tag_message (member access)
#include <windows.h>      // MessageBoxA
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <BASE/palette.h>
#include <SOURCE/X_GLOBAL.h>

DATA(0x005331c0) static i32 giFindMid;

// ---- module-private synthetic globals (retail xref: single-module) ----
DATA(0x005331cc) static i32 gBlitRight;  // BlitBitmapToScreen computed blit-rect right edge
DATA(0x005331d0) static i32 gBlitBottom; // BlitBitmapToScreen computed blit-rect bottom edge

// ---- initialized storage (retail RVA order) ----
DATA(0x0051dce8) i32 iMemEntries = 0;
DATA(0x0051dcec) MemEntry* gpMemEntry = 0;
DATA(0x0051dcf0) i32 giTotalMemAllocated = 0;
DATA(0x0051dcf8) u8
    giChangeThreshold[16] = {0, 1, 2, 3, 4, 6, 8, 10, 13, 16, 19, 22, 26, 31, 37, 46};
DATA(0x0051dd08) i32 iLastSeed = 0x08156a03;
DATA(0x0051dd0c) static char gMemEntryTag[sizeof("IME")] = "IME";

// Retail emits each text operand as an independent aligned allocation. Grouping
// them by owning workflow preserves those physical owners and their call-site
// addends without relying on compiler literal pooling.
DATA(0x0051dd10) static SMiscText gMiscText = {
    {{"KBAlloc    Size %d   Ptr %d   File %s  Line %d"},
     {"Free "},
     {"NULL POINTER"},
     {"MemEntries Below 0"},
     {"KBFree    Size %d   Ptr %d   File %s  Line %d"},
     {"Bad Delete,  File '%13s'  Line % 4d, ptr %12d"},
     {"Total Memory Leaks"},
     {"Memory Leak,  File '%13s'  Line % 4d, ptr %12d   size %6d"},
     {"Mem Left %dK"},
     {"Assert statement failed in module %s, line %d.  Do you wish to abort the program?"},
     {"Assert Failure"}},
    {{"AUTO"}, {"AUTO"}},
    {{"The Unknown Hero"}, {"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"}},
    {{"HEROES2.CFG"}, {"%s"}, {"rb"}, {""}, {""}},
    {{""},
     {MISC_REGISTRY_KEY},
     {"Music Volume"},
     {"Music Volume"},
     {"Sound Volume"},
     {"Walk Speed"},
     {"Computer Walk Speed"},
     {"Show Route"},
     {"Blackout Computer"},
     {"Sound Quality"},
     {"Use Opera"},
     {"Direct Connect Com Port"},
     {"Direct Connect Baud Rate"},
     {"Modem Com Port"},
     {"Modem Baud Rate"},
     {"Modem Init String"},
     {"Unique System ID"},
     {"Network Default Name"},
     {"Autosave"},
     {"Slow Video"},
     {"Show Combat Grid"},
     {"Show Combat Mouse Hex"},
     {"Combat Shade Level"},
     {"Combat Army Info Level"},
     {"Evil Interface Usage"},
     {"Quick Combat Level"},
     {"Combat Speed"},
     {"Auto Combat Use Spells"},
     {"First Map Offset"},
     {"Current Map Offset"},
     {"Show Object Boxes"},
     {"Editor Screen Animation"},
     {"Editor Palette Cycling"},
     {"Main Game Show Menu"},
     {"Main Game X"},
     {"Main Game Y"},
     {"Main Game Width"},
     {"Main Game Height"},
     {"Main Game Full Screen"},
     {"Main Game Color Mouse Cursor"},
     {"Editor Show Menu"},
     {"Editor X"},
     {"Editor Y"},
     {"Editor Width"},
     {"Editor Height"},
     {"Editor Full Screen"},
     {"Editor Color Mouse Cursor"},
     {"AppPath"},
     {""},
     {"CDDrive"},
     {""}},
    {{"RMT%sRL.BIN"},
     {"RMT%sRC.BIN"},
     {"RMT%sRD.BIN"},
     {"RMT%sSL.BIN"},
     {"RMT%sSC.BIN"},
     {"RMT%sSD.BIN"}},
    {{"HEROES2.CFG"}, {"%s"}},
    {{""},
     {MISC_REGISTRY_KEY},
     {"Music Volume"},
     {"Sound Volume"},
     {"Walk Speed"},
     {"Computer Walk Speed"},
     {"Show Route"},
     {"Blackout Computer"},
     {"Sound Quality"},
     {"Use Opera"},
     {"Direct Connect Com Port"},
     {"Direct Connect Baud Rate"},
     {"Modem Com Port"},
     {"Modem Baud Rate"},
     {"Modem Init String"},
     {"Unique System ID"},
     {"Network Default Name"},
     {"Autosave"},
     {"Slow Video"},
     {"Show Combat Grid"},
     {"Show Combat Mouse Hex"},
     {"Combat Shade Level"},
     {"Combat Army Info Level"},
     {"Evil Interface Usage"},
     {"Quick Combat Level"},
     {"Combat Speed"},
     {"Auto Combat Use Spells"},
     {"First Map Offset"},
     {"Current Map Offset"},
     {"Show Object Boxes"},
     {"Editor Screen Animation"},
     {"Editor Palette Cycling"},
     {"Main Game Show Menu"},
     {"Main Game X"},
     {"Main Game Y"},
     {"Main Game Width"},
     {"Main Game Height"},
     {"Main Game Full Screen"},
     {"Main Game Color Mouse Cursor"},
     {"Editor Show Menu"},
     {"Editor X"},
     {"Editor Y"},
     {"Editor Width"},
     {"Editor Height"},
     {"Editor Full Screen"},
     {"Editor Color Mouse Cursor"}},
    {{"A:\\"},
     {".\\DATA\\HEROES2.AGG"},
     {"%s\\heroes2\\anim\\voy24.smk"},
     {"%s"},
     {"open %c: type cdaudio alias CD"},
     {"info CD UPC wait"},
     {"close CD"},
     {"%c:\\heroes2\\anim\\voy24.smk"},
     {MISC_REGISTRY_KEY},
     {"%c:"},
     {"CDDrive"},
     {"%c:%s"}},
    {{"KB.LOG"},
     {"===========New Log=========="},
     {"\n"},
     {"at+"},
     {"KB.LOG"},
     {"\n"},
     {"%s : % 8d % 8d % 8d % 8d % 8d % 8d % 8d"},
     {"%s : % 8d % 8d % 8d % 8d % 8d % 8d"},
     {"%s : % 8d % 8d % 8d % 8d % 8d"},
     {"%s : % 8d % 8d % 8d % 8d"},
     {"%s : % 8d % 8d % 8d"},
     {"%s : % 8d % 8d"},
     {"%s : % 8d"}},
    {{MISC_PCX_SOURCE_FILE},
     {MISC_PCX_SOURCE_FILE},
     {MISC_PCX_SOURCE_FILE},
     {MISC_PCX_SOURCE_FILE}},
    {{"r+b"}},
    {{"advmice.mse"}, {""}, {"evntwin%d.bin"}, {""}, {"buybuild.icn"}, {"bigfont.fnt"}, {""}}
};

// @data-layout-note Retail initialized storage is the contiguous contribution
// 0x11dce8..0x11e94c (0xc64). The rebuilt section has the same extent and SHA-256
// 3a271f06aa947cc55114c53bb6d01778a45127f3e381ca1736ba0ebf00ff7e13. Its five
// public owners occupy offsets 0x0..0x20, gMemEntryTag is the real allocation at
// +0x24, and gMiscText owns the remaining typed text record at +0x28. All 274
// initialized/BSS target occurrences and owner-relative addends match retail as
// a multiset; 272 also align in relocation order. BlitBitmapToScreen interleaves
// two gBlitRight/gBlitBottom loads with gpMouseManager differently, as documented
// at that function, but retains the exact six blit-global targets and zero addends.
// The candidate owns the same nine BSS objects in 0x24 bytes, but VC 4.2 currently
// packs them in a different internal order; retail's NB09 contribution is 0x28 and
// includes four unowned tail bytes. Keep the DATA-proved owners and leave that as a
// final-link packing residual rather than adding aliases, padding, or placement rules.

VA(0x004c3d10, 0x58)
void InitMemEntry(void) {
    LogInt(gMemEntryTag, iMemEntries, -999, -999, -999, -999, -999, -999);
    gpMemEntry = static_cast<MemEntry*>(malloc(2000 * sizeof(MemEntry)));
    for (i32 i = 0; i < 2000; ++i)
        gpMemEntry[i].used = 0;
}

// @semantic
// Structurally complete /O2 checkpoint: allocation/tracking semantics, the 12-branch CFG,
// and all 29 relocations agree.  With the required real icon definition in this TU, base
// currently reserves 0x2c4 bytes and ends at 0x204; retail reserves 0x2bc and is 0x20f.
// The first divergence is the frame immediate at +0x2, followed at +0xc by base keeping
// size in EBP while retail uses EBX.  The two arrays account for the retail 700-byte frame;
// there is no missing local.  Earlier exact-frame TU states left only the newline intrinsic.
// `strcat`, strcpy/memcpy-at-strlen, direct word stores, volatile loads, and a manual scan
// were already tried.  Revisit only after required shared-header/TU state changes.
VA(0x004c3d70, 0x20f)
void* BaseAlloc(u32 size, char* originalFile, i32 originalLine) {
    char text[200];
    char logText[500];
    if (size == 0)
        return 0;
    if (gpMemEntry == 0) {
        LogInt(gMemEntryTag, iMemEntries, -999, -999, -999, -999, -999, -999);
        gpMemEntry = static_cast<MemEntry*>(malloc(2000 * sizeof(MemEntry)));
        for (i32 initIndex = 0; initIndex < 2000; ++initIndex)
            gpMemEntry[initIndex].used = 0;
    }
    giTotalMemAllocated += size;
    void* ptr = malloc(size);
    if (ptr == 0) {
        MemError();
        return 0;
    }
    ++iMemEntries;
    i32 entryIndex;
    for (entryIndex = 0; entryIndex < 2000; ++entryIndex) {
        if (!gpMemEntry[entryIndex].used) {
            gpMemEntry[entryIndex].used = 1;
            gpMemEntry[entryIndex].ptr = ptr;
            gpMemEntry[entryIndex].size = size;
            strcpy(gpMemEntry[entryIndex].file, originalFile);
            gpMemEntry[entryIndex].line = originalLine;
            entryIndex = 99999;
        }
    }
    if (giDebugLevel == 4) {
        sprintf(
            text,
            gMiscText.memory.allocationFormat.text,
            size,
            ptr,
            originalFile,
            originalLine
        );
        if (giDebugLevel >= 2) {
            FILE* logFile = fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
            if (logFile != 0) {
                strcpy(logText, text);
                *reinterpret_cast<u16*>(logText + strlen(logText)) =
                    *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
                fputs(logText, logFile);
                fclose(logFile);
                if (giDebugLevel == 4)
                    OutputDebugStringA(logText);
            }
        }
    }
    return ptr;
}

// @semantic
// Structurally complete /O2 checkpoint: both spans are 0x386 with the same 0x2c4 frame,
// 20-branch CFG, and all 52 relocations.  The first residual is the first newline append:
// base preloads the word then derives `buf+strlen`; retail scans first and writes `[edi-1]`.
// The other two append sites have the same scheduling difference.  All allocation/free,
// MemEntry, logging, and bad-delete paths agree.  The BaseAlloc append spellings were also
// tested here; a cached MemEntry reference regressed the loop allocation. Revisit only
// after exact-preserving predecessor/TU-state changes.
VA(0x004c3f80, 0x386)
void BaseFree(void* ptr, char* originalFile, i32 originalLine) {
    char logText[500];
    char text[200];
    if (gpMemEntry == 0) {
        LogInt(gMemEntryTag, iMemEntries, -999, -999, -999, -999, -999, -999);
        gpMemEntry = static_cast<MemEntry*>(malloc(2000 * sizeof(MemEntry)));
        for (i32 initIndex = 0; initIndex < 2000; ++initIndex)
            gpMemEntry[initIndex].used = 0;
    }
    if (giDebugLevel == 4)
        LogInt(
            gMiscText.memory.freeLabel.text,
            reinterpret_cast<i32>(ptr),
            -999,
            -999,
            -999,
            -999,
            -999,
            -999
        );
    if (ptr == 0) {
        if (giDebugLevel >= 2) {
            FILE* logFile = fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
            if (logFile != 0) {
                strcpy(logText, gMiscText.memory.nullPointer.text);
                *reinterpret_cast<u16*>(logText + strlen(logText)) =
                    *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
                fputs(logText, logFile);
                fclose(logFile);
                if (giDebugLevel == 4)
                    OutputDebugStringA(logText);
            }
        }
        return;
    }
    --iMemEntries;
    if (iMemEntries < 0)
        LogInt(
            gMiscText.memory.entryUnderflow.text,
            iMemEntries,
            -999,
            -999,
            -999,
            -999,
            -999,
            -999
        );
    i32 entryIndex;
    for (entryIndex = 0; entryIndex < 2000; ++entryIndex) {
        if (gpMemEntry[entryIndex].ptr == ptr) {
            if (giDebugLevel == 4) {
                sprintf(
                    text,
                    gMiscText.memory.freeFormat.text,
                    gpMemEntry[entryIndex].size,
                    ptr,
                    gpMemEntry[entryIndex].file,
                    gpMemEntry[entryIndex].line
                );
                if (giDebugLevel >= 2) {
                    FILE* logFile =
                        fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
                    if (logFile != 0) {
                        strcpy(logText, text);
                        *reinterpret_cast<u16*>(logText + strlen(logText)) =
                            *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
                        fputs(logText, logFile);
                        fclose(logFile);
                        if (giDebugLevel == 4)
                            OutputDebugStringA(logText);
                    }
                }
            }
            gpMemEntry[entryIndex].used = 0;
            giTotalMemAllocated -= gpMemEntry[entryIndex].size;
            entryIndex = 99999;
        }
    }
    if (entryIndex < 99999) {
        sprintf(gText, gMiscText.memory.badDeleteFormat.text, originalFile, originalLine, ptr);
        if (giDebugLevel >= 2) {
            FILE* logFile = fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
            if (logFile != 0) {
                strcpy(logText, gText);
                *reinterpret_cast<u16*>(logText + strlen(logText)) =
                    *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
                fputs(logText, logFile);
                fclose(logFile);
                if (giDebugLevel == 4)
                    OutputDebugStringA(logText);
            }
        }
    } else {
        free(ptr);
    }
}

// @semantic
// Structurally complete /O2 checkpoint: base .text is 0x137 bytes versus retail 0x134.
// Only the completed log-line append differs: base loads `"\n"` at +0xd6 (reloc
// +0xd9) before deriving `buf+strlen`, while retail loads it at +0xe3 (reloc +0xe6)
// after `repne scasb` and stores through `[edi-1]`. LogInt, sprintf, fopen/fputs/
// fclose and OutputDebugStringA targets are otherwise identical. `strcat`, strlen+
// strcpy/memcpy, direct and named word stores, volatile load, and manual scan tried.
// A bounded libclang AST pass tested nine variants in 30 walks and retained none.
// Revisit through exact-preserving predecessor/TU-state variants; not byte-proven.
VA(0x004c4310, 0x134)
void PrintMemoryLeaks(void) {
    char logText[500];
    if (giDebugLevel >= 1 && gpMemEntry != 0) {
        LogInt(
            gMiscText.memory.leakCountLabel.text,
            iMemEntries,
            -999,
            -999,
            -999,
            -999,
            -999,
            -999
        );
        i32 entryIndex = 0;
        do {
            if (gpMemEntry[entryIndex].used != 0) {
                sprintf(
                    gText,
                    gMiscText.memory.leakFormat.text,
                    gpMemEntry[entryIndex].file,
                    gpMemEntry[entryIndex].line,
                    reinterpret_cast<i32>(gpMemEntry[entryIndex].ptr),
                    gpMemEntry[entryIndex].size
                );
                if (giDebugLevel >= 2) {
                    FILE* logFile =
                        fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
                    if (logFile != 0) {
                        strcpy(logText, gText);
                        *reinterpret_cast<u16*>(logText + strlen(logText)) =
                            *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
                        fputs(logText, logFile);
                        fclose(logFile);
                        if (giDebugLevel == 4)
                            OutputDebugStringA(logText);
                    }
                }
            }
            entryIndex = entryIndex + 1;
        } while (entryIndex < 2000);
    }
}

VA(0x004c4450, 0x91)
void ShowMemoryStatus(void) {
    sprintf(gText, gMiscText.memory.memoryStatusFormat.text, 0x3ea2);
    i32 savedDebugLevel = giDebugLevel;
    giDebugLevel = 9;
    FillBitmapArea(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0);
    smallFont->DrawBoundedString(gText, 0, 0x1d0, 0x280, 0x10, 1, 0);
    BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0, 0x1cc);
    giDebugLevel = savedDebugLevel;
}

// Structurally complete /O2 checkpoint: both sections are 0x48 with identical CFG and
// no relocations.  Recovering the explicit shifted-hash temporary changed the rotate from
// base `shr eax,25; shl esi,5` to retail's `shl eax,5; shr esi,25`, raising this from
// 93.29% to 99.14%. The only remaining bytes are the three equivalent SIB encodings at
// +0x1b, +0x28 and +0x35 (`[ecx+edx]` versus `[edx+ecx]`).  `text[i]`, `i[text]`, both
// rotate-expression orders were tried. A bounded 30-walk libclang AST pass exposed four
// variants and retained none; revisit after an exact-preserving predecessor/TU-state change.
VA(0x004c44f0, 0x48)
u32l MAKEFILEID(char* text) {
    u32 hash = 0;
    i32 sum = 0;
    for (i32 i = strlen(text) - 1; i >= 0; --i) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            text[i] &= ~0x20;
        }
        u32 shiftedHash = hash << 5;
        hash >>= 25;
        hash += shiftedHash;
        sum += text[i];
        hash += text[i] + sum;
    }
    return hash;
}

// @semantic
// Structurally complete /O2 checkpoint: base is 0x97 bytes, retail 0x95, with the same
// eight-branch CFG and four giFindMid relocations to 0x5331c0.  The first current
// divergence is +0x2a: base emits `cmp edi,eax; jge`, retail `cmp eax,edi; jle` and then
// reuses those flags for equality where base emits another compare at +0x38.  The key/value
// fields are typed and named.  Direct tests, saved values, reversed/negated predicates,
// combined conditions, and three-way forms were tried; this is not a proven artifact.
VA(0x004c4540, 0x95)
i32 FindIndex(struct indexArray* entries, i32 low, i32 high, i32 key) {
    giFindMid = (low + high) >> 1;
    while (high - low > 1) {
        i32 value = entries[giFindMid].key;
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

#include <BASE/MiscGraphicsConstants.h>

// @semantic
// Structurally complete /O2 checkpoint: both sections are 0xea with the same CFG and
// exact 11/11 relocation identities (the retail gConfig reference is its interior member).
// Explicit `threshold = maxLevel - level` preserves the recovered loop semantics. The
// typed graphics-domain boundary rotates base's EBX/ESI/EBP allocation and leaves live at
// 95.56% versus a retained 96.79%; the first divergence remains base testing ECX after
// allocation while retail tests EAX before construction. `new palette`, value-init and
// split declaration/assignment were tried. At head 34c93d1, the pre-domain bounded AST
// pass found eight variants and retained none after 30 walks with all 144 siblings pinned.
VA(0x004c45e0, 0xea)
void FadeIn(i32 increment) {
    palette* fadePalette = new palette;
    if (fadePalette == 0)
        MemError();
    i32 done = 0;
    if (gConfig.gfx[giCurExe].fullScreen == 0)
        increment *= 2;
    memset(fadePalette->m_data, 0, MISC_PALETTE_BYTE_COUNT);
    i32 level = 0;
    for (;;) {
        if (level >= MISC_PALETTE_LEVEL_COUNT) {
            if (done) {
                delete fadePalette;
                return;
            }
            level = MISC_PALETTE_MAX_LEVEL;
        }
        i32 delayUntil = KBTickCount() + MISC_FADE_IN_FRAME_DELAY;
        PollSound();
        i8* colors;
        if (level == MISC_PALETTE_MAX_LEVEL) {
            done = 1;
            colors = gpBufferPalette->m_data;
        } else {
            i32 threshold = MISC_PALETTE_MAX_LEVEL - level;
            for (i32 i = 0; i < MISC_PALETTE_BYTE_COUNT; ++i) {
                i8 color = gpBufferPalette->m_data[i];
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
void FadeOut(i32 increment) {
    palette* fadePalette = new palette;
    if (fadePalette == 0)
        MemError();
    i32 done = 0;
    if (gConfig.gfx[giCurExe].fullScreen == 0)
        increment *= 2;
    memcpy(fadePalette->m_data, gpBufferPalette->m_data, 0x300);
    i32 level = 0;
    for (;;) {
        if (level >= 0x40) {
            if (done) {
                delete fadePalette;
                return;
            }
            level = 0x3f;
        }
        i32 delayUntil = KBTickCount() + 0x14;
        PollSound();
        if (level == 0x3f)
            done = 1;
        for (i32 i = 0; i < 0x300; ++i) {
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
i32 Random(i32 low, i32 high) {
    if (low == high) {
        return high;
    }
    if (high < low) {
        return low;
    }
    return low + rand() % (high - low + 1);
}

VA(0x004c47f0, 0x5d)
void ProcessAssert(i32 condition, char* file, i32 line) {
    if (condition == 0) {
        gpMouseManager->SetColorMice(0);
        SetFullScreenStatus(0);
        sprintf(gText, gMiscText.memory.assertMessage.text, file, line);
        if (MessageBoxA(hwndApp, gText, gMiscText.memory.assertTitle.text, 0x14) != 7) {
            ShutDown(0);
        }
    }
}

// @semantic
// Structurally complete /O2 checkpoint with equal 0x66-byte sections, the same frame/CFG,
// and the sole strncmp relocation.  The explicit MAKEFILEID rotate temporary moved this
// from the old wholesale register-allocation residual to 98.60%; all instructions now agree
// except the final +0x57 loop test (`cmp esi,ebx; jl` versus `cmp ebx,esi; jg`). Pointer
// swaps, `&i[text]`, lvalue count loads, for/while/do forms, count|0, reversed bounds and a
// bounded 30-walk libclang AST pass were tried. Revisit only after another exact-preserving
// predecessor/TU-state change; this operand order is not a proven artifact.
VA(0x004c4850, 0x66)
char* FindStringInString(char* text, char* pattern) {
    i32 text_len = strlen(text);
    i32 pattern_len = strlen(pattern);
    i32 count = text_len - pattern_len + 1;
    i32 i = 0;
    if (count > 0) {
        do {
            if (strncmp(text + i, pattern, pattern_len) == 0)
                return text + i;
            ++i;
        } while (count > i);
    }
    return 0;
}

// @semantic
// Structurally complete /O2 checkpoint with equal 0x31-byte sections and no relocs/calls.
// The MAKEFILEID predecessor change fixed the old SIB residual; the only current difference
// is the +0x21 loop test (`cmp eax,ecx; jl` versus `cmp ecx,eax; jg`).
// for/while/do loops, explicit backedges, length/index `|0`, reversed bounds/returns,
// `i[text]`, lvalue length loads and a bounded 30-walk libclang AST pass did not steer
// them. Revisit after exact-preserving TU-state changes; these operand-order encodings are
// not byte-proven artifacts.
VA(0x004c48c0, 0x31)
char* FindToken(char* text, char token) {
    i32 len = strlen(text);
    i32 i = 0;
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
char* FindLastToken(char* text, char token) {
    for (i32 i = strlen(text) - 1; i >= 0; --i) {
        if (text[i] == token) {
            return text + i;
        }
    }
    return 0;
}

VA(0x004c4930, 0x6c)
void SetInstallDefaults(void) {
    memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
    strcpy(gConfig.autoLoadName, gMiscText.installDefaults.autoLoadName.text);
    strcpy(gConfig.autoSaveName, gMiscText.installDefaults.autoSaveName.text);
    gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
}

#include <BASE/MiscConfigConstants.h>

// @semantic
// Structurally complete /O2 checkpoint: base is 0x1b7, retail 0x1b5, with the same
// three-branch CFG and the same 42 relocation occurrences. Manual target-address review
// confirms the intended gConfig/giMainVideoModeWidth owners; `homm2 relocs` reports six
// only-base entries because the delinker names gConfig interior labels as constants/strings.
// Iteration now uses the real exeGfxConfig type and advances one seven-word record at a time
// to the showCombatGrid
// endpoint (+0x68); the former interior-int reconstruction and its EBP spill are gone.
// The first residual is +0x1: after the common `push ebx`, base materializes gfx[0]
// (+0x1c) before saving ESI/EDI, while retail saves them first and materializes
// &gfx[0].fullScreen (+0x30), addressing the other fields relative to that member.
// Direct typed-pointer, interior-member, index, and field-order variants have been tried;
// revisit after exact-preserving predecessor/shared-header TU state.
VA(0x004c49a0, 0x1b5)
void SetGameDefaults(void) {
    i32* fullScreen = &gConfig.gfx[0].fullScreen;
    gConfig.musicVolume = 1;
    gConfig.soundVolume = 1;
    gConfig.autosave = 1;
    gConfig.showRoute = 1;
    do {
        fullScreen[MISC_GRAPHICS_SHOW_MENU_FROM_FULLSCREEN] = 1;
        fullScreen[MISC_GRAPHICS_X_FROM_FULLSCREEN] = MISC_DEFAULT_WINDOW_ORIGIN;
        fullScreen[MISC_GRAPHICS_Y_FROM_FULLSCREEN] = MISC_DEFAULT_WINDOW_ORIGIN;
        fullScreen[MISC_GRAPHICS_COLOR_MOUSE_FROM_FULLSCREEN] = 0;
        fullScreen[MISC_GRAPHICS_FULLSCREEN] = 1;
        if (giMainVideoModeWidth <= MISC_DEFAULT_WINDOW_WIDTH) {
            fullScreen[MISC_GRAPHICS_WIDTH_FROM_FULLSCREEN] = MISC_DEFAULT_SMALL_WINDOW_WIDTH;
            fullScreen[MISC_GRAPHICS_HEIGHT_FROM_FULLSCREEN] = MISC_DEFAULT_SMALL_WINDOW_HEIGHT;
        } else {
            fullScreen[MISC_GRAPHICS_WIDTH_FROM_FULLSCREEN] = MISC_DEFAULT_WINDOW_WIDTH;
            fullScreen[MISC_GRAPHICS_HEIGHT_FROM_FULLSCREEN] = MISC_DEFAULT_WINDOW_HEIGHT;
        }
        fullScreen += CONFIG_GRAPHICS_SIZE / sizeof(*fullScreen);
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
    gConfig.firstMapOffset = rand() % MISC_DEFAULT_MAP_OFFSET_COUNT;
    gConfig.showObjectBoxes = 0;
    gConfig.editorScreenAnimation = 0;
    gConfig.editorPaletteCycling = 0;
    gbFirstTimeThrough = 1;
    gConfig.slowVideo = 3;
    gConfig.computerWalkSpeed = 3;
    gConfig.walkSpeed = 2;
    strcpy(gConfig.networkDefaultName, gMiscText.gameDefaults.unknownHeroName.text);
    *reinterpret_cast<i32*>(gConfig.uniqueSystemID) = 0;
    i32 idSeed = rand() % MISC_UNIQUE_ID_RANDOM_MODULUS + 1;
    idSeed += KBTickCount();
    gConfig.uniqueSystemID[2] =
        gMiscText.gameDefaults.uniqueIdAlphabet.text[idSeed % MISC_UNIQUE_ID_ALPHANUMERIC_COUNT];
    i32 idAdd = rand() % MISC_UNIQUE_ID_RANDOM_MODULUS + 1;
    idAdd += KBTickCount();
    idSeed += idAdd;
    gConfig.uniqueSystemID[1] =
        gMiscText.gameDefaults.uniqueIdAlphabet.text[idSeed % MISC_UNIQUE_ID_ALPHANUMERIC_COUNT];
    idAdd = rand() % MISC_UNIQUE_ID_RANDOM_MODULUS + 1;
    idAdd += KBTickCount();
    idSeed += idAdd;
    gConfig.uniqueSystemID[0] = static_cast<char>(idSeed % MISC_UNIQUE_ID_ALPHA_COUNT + 'A');
    gConfig.needsDefaultInitialization = 0;
}

// @early-stop
// @early-stop-reloc-only
// All 0x13f relocation-masked bytes and all 28 ordered relocation sites and
// effective targets agree. Residual identities are the Misc text block and the
// _access/__access CRT alias at the same retail address.
VA(0x004c4b60, 0x13f)
void ReadPrefsFromFile(void) {
    sprintf(gText, gMiscText.readFile.stringFormat.text, gMiscText.readFile.configFilename.text);
    if (_access(gText, 0) == -1) {
        memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
        strcpy(gConfig.autoLoadName, gMiscText.installDefaults.autoLoadName.text);
        strcpy(gConfig.autoSaveName, gMiscText.installDefaults.autoSaveName.text);
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
    } else {
        FILE* f = fopen(gText, gMiscText.readFile.binaryMode.text);
        if (f == 0)
            FileError(gText);
        fread(&gConfig, CONFIG_PERSISTED_SIZE, 1, f);
        fclose(f);
        if (gConfig.needsDefaultInitialization == 0)
            goto skipDefaults;
    }
    SetGameDefaults();
    UpdateSystemOptionsMenu();
    WritePrefsToRegistry();
skipDefaults:
    strcpy(gcRegCDRomPath, gMiscText.readFile.cdRomPathDefault.text);
    strcpy(gcRegAppPath, gMiscText.readFile.appPathDefault.text);
}

// Registry value byte counts passed to RegQueryValueExA/RegSetValueExA. They are
// retail literals, NOT sizeof the gConfig fields: modemInitString is char[0x64]
// but retail moves 0x62 bytes, and networkDefaultName is char[0x18] but retail
// moves 0x1e — six bytes past the field, faithfully reproduced.
typedef enum RegistryValueSize {
    REG_MODEM_INIT_STRING_SIZE = 0x62,
    REG_UNIQUE_SYSTEM_ID_SIZE = 4,
    REG_NETWORK_DEFAULT_NAME_SIZE = 0x1e
} RegistryValueSize;

VA(0x004c4ca0, 0x7ab)
void ReadPrefsFromRegistry(void) {
    HKEY hKey;
    DWORD dwType;
    DWORD dwSize;
    char szKey[100];
    char szScratch[100];

    strcpy(szScratch, gMiscText.readRegistry.scratchDefault.text);
    strcpy(szKey, gMiscText.readRegistry.key.text);
    hKey = 0;
    if (RegCreateKeyA(HKEY_LOCAL_MACHINE, szKey, &hKey) != 0)
        return;
    dwSize = 4;
    if (RegQueryValueExA(
            hKey,
            gMiscText.readRegistry.musicVolumeProbe.text,
            0,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.musicVolume),
            &dwSize
        )
        != 0) {
        memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
        memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
        strcpy(gConfig.autoLoadName, gMiscText.installDefaults.autoLoadName.text);
        strcpy(gConfig.autoSaveName, gMiscText.installDefaults.autoSaveName.text);
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
        SetGameDefaults();
        RegCloseKey(hKey);
        UpdateSystemOptionsMenu();
        WritePrefsToRegistry();
        return;
    }
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.musicVolume.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.musicVolume),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.soundVolume.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.soundVolume),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.walkSpeed.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.walkSpeed),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.computerWalkSpeed.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.computerWalkSpeed),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.showRoute.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.showRoute),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.blackoutComputer.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.blackoutComputer),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.soundQuality.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.musicSource),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.useOpera.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.useOpera),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.directComPort.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.comPort[CONFIG_CONNECTION_DIRECT]),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.directBaudRate.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.baudRate[CONFIG_CONNECTION_DIRECT]),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.modemComPort.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.comPort[CONFIG_CONNECTION_MODEM]),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.modemBaudRate.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.baudRate[CONFIG_CONNECTION_MODEM]),
        &dwSize
    );
    dwSize = 0x63;
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.modemInitString.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(gConfig.modemInitString),
        &dwSize
    );
    dwSize = 4;
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.uniqueSystemId.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(gConfig.uniqueSystemID),
        &dwSize
    );
    // Retail relocation at function +0x2f6 resolves to gConfig +0x125, the terminator byte after
    // this four-byte ID, rather than the unrelated modem string at gConfig +0x110.
    gConfig.uniqueSystemID[3] = 0;
    dwSize = 0x1f;
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.networkDefaultName.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(gConfig.networkDefaultName),
        &dwSize
    );
    dwSize = 4;
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.autosave.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.autosave),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.slowVideo.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.slowVideo),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.showCombatGrid.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.showCombatGrid),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.showCombatMouseHex.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.showCombatMouseHex),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.combatShadeLevel.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.combatShadeLevel),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.combatArmyInfoLevel.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.combatArmyInfoLevel),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.evilInterfaceUsage.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.evilInterfaceUsage),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.quickCombatLevel.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.quickCombatLevel),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.combatSpeed.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.combatSpeed),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.autoCombatUseSpells.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.autoCombatUseSpells),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.firstMapOffset.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.firstMapOffset),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.currentMapOffset.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.currentMapOffset),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.showObjectBoxes.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.showObjectBoxes),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorScreenAnimation.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.editorScreenAnimation),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorPaletteCycling.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.editorPaletteCycling),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameShowMenu.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[0].showMenu),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameX.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[0].x),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameY.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[0].y),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameWidth.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[0].width),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameHeight.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[0].height),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameFullScreen.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[0].fullScreen),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameColorMouseCursor.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[0].colorMouseCursor),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorShowMenu.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[1].showMenu),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorX.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[1].x),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorY.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[1].y),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorWidth.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[1].width),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorHeight.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[1].height),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorFullScreen.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[1].fullScreen),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorColorMouseCursor.text,
        0,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[1].colorMouseCursor),
        &dwSize
    );
    dwSize = 0x63;
    if (RegQueryValueExA(
            hKey,
            gMiscText.readRegistry.appPath.text,
            0,
            &dwType,
            reinterpret_cast<u8*>(gcRegAppPath),
            &dwSize
        )
        != 0)
        strcpy(gcRegAppPath, gMiscText.readRegistry.appPathDefault.text);
    if (RegQueryValueExA(
            hKey,
            gMiscText.readRegistry.cdDrive.text,
            0,
            &dwType,
            reinterpret_cast<u8*>(gcRegCDRomPath),
            &dwSize
        )
        != 0)
        strcpy(gcRegCDRomPath, gMiscText.readRegistry.cdDriveDefault.text);
    RegCloseKey(hKey);
    // Clamp the saved window geometry to sane defaults / on-screen bounds.
    if (gConfig.gfx[giCurExe].width <= 0)
        gConfig.gfx[giCurExe].width = 0x140; // default 320 wide
    if (gConfig.gfx[giCurExe].height <= 0)
        gConfig.gfx[giCurExe].height = 0xf0; // default 240 tall
    if (gConfig.gfx[giCurExe].x < 0)
        gConfig.gfx[giCurExe].x = 0;
    if (gConfig.gfx[giCurExe].x > giMainVideoModeHeight - 0xc8) // keep >= 200px on-screen
        gConfig.gfx[giCurExe].x = giMainVideoModeHeight - 0xc8;
    if (gConfig.gfx[giCurExe].y < 0)
        gConfig.gfx[giCurExe].y = 0;
    if (gConfig.gfx[giCurExe].y > giMainVideoModeWidth - 0xc8)
        gConfig.gfx[giCurExe].y = giMainVideoModeWidth - 0xc8;
}

VA(0x004c5450, 0xa1)
void ReadPrefs(void) {
    memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
    ReadPrefsFromRegistry();
    sprintf(gConfig.rmtRLName, gMiscText.remoteNames.remoteLocal.text, gConfig.uniqueSystemID);
    sprintf(gConfig.rmtRCName, gMiscText.remoteNames.remoteCenter.text, gConfig.uniqueSystemID);
    sprintf(gConfig.rmtRDName, gMiscText.remoteNames.remoteRight.text, gConfig.uniqueSystemID);
    sprintf(gConfig.rmtSLName, gMiscText.remoteNames.serverLocal.text, gConfig.uniqueSystemID);
    sprintf(gConfig.rmtSCName, gMiscText.remoteNames.serverCenter.text, gConfig.uniqueSystemID);
    sprintf(gConfig.rmtSDName, gMiscText.remoteNames.serverRight.text, gConfig.uniqueSystemID);
}

// @early-stop
// @early-stop-reloc-only
// All 0x6a relocation-masked bytes, CFG, and nine ordered relocation sites and
// effective targets agree. Residual identities are the Misc text block and the
// _open/_write/_close CRT aliases at their retail addresses.
VA(0x004c5500, 0x6a)
void WritePrefsToFile(void) {
    i32 zeroBuffer[25];
    i32 i;
    i32* p = zeroBuffer;
    for (i = 0x19; i != 0; i--) {
        *p = 0;
        p++;
    }
    sprintf(gText, gMiscText.writeFile.stringFormat.text, gMiscText.writeFile.configFilename.text);
    i32 fd = _open(gText, 0x8301, 0x80);
    if (fd != -1) {
        _write(fd, &gConfig, CONFIG_PERSISTED_SIZE);
        _close(fd);
    }
}

VA(0x004c5570, 0x491)
void WritePrefsToRegistry(void) {
    HKEY hKey;
    char szKey[100];
    char szScratch[100];

    strcpy(szScratch, gMiscText.writeRegistry.scratchDefault.text);
    strcpy(szKey, gMiscText.writeRegistry.key.text);
    hKey = 0;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, szKey, 0, KEY_ALL_ACCESS, &hKey) != 0)
        return;
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.musicVolume.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.musicVolume),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.soundVolume.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.soundVolume),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.walkSpeed.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.walkSpeed),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.computerWalkSpeed.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.computerWalkSpeed),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.showRoute.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.showRoute),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.blackoutComputer.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.blackoutComputer),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.soundQuality.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.musicSource),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.useOpera.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.useOpera),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.directComPort.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.comPort[CONFIG_CONNECTION_DIRECT]),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.directBaudRate.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.baudRate[CONFIG_CONNECTION_DIRECT]),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.modemComPort.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.comPort[CONFIG_CONNECTION_MODEM]),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.modemBaudRate.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.baudRate[CONFIG_CONNECTION_MODEM]),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.modemInitString.text,
        0,
        REG_SZ,
        reinterpret_cast<u8*>(gConfig.modemInitString),
        REG_MODEM_INIT_STRING_SIZE
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.uniqueSystemId.text,
        0,
        REG_SZ,
        reinterpret_cast<u8*>(gConfig.uniqueSystemID),
        REG_UNIQUE_SYSTEM_ID_SIZE
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.networkDefaultName.text,
        0,
        REG_SZ,
        reinterpret_cast<u8*>(gConfig.networkDefaultName),
        REG_NETWORK_DEFAULT_NAME_SIZE
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.autosave.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.autosave),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.slowVideo.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.slowVideo),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.showCombatGrid.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.showCombatGrid),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.showCombatMouseHex.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.showCombatMouseHex),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.combatShadeLevel.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.combatShadeLevel),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.combatArmyInfoLevel.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.combatArmyInfoLevel),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.evilInterfaceUsage.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.evilInterfaceUsage),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.quickCombatLevel.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.quickCombatLevel),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.combatSpeed.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.combatSpeed),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.autoCombatUseSpells.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.autoCombatUseSpells),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.firstMapOffset.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.firstMapOffset),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.currentMapOffset.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.currentMapOffset),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.showObjectBoxes.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.showObjectBoxes),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorScreenAnimation.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.editorScreenAnimation),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorPaletteCycling.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.editorPaletteCycling),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameShowMenu.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[0].showMenu),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameX.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[0].x),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameY.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[0].y),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameWidth.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[0].width),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameHeight.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[0].height),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameFullScreen.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[0].fullScreen),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameColorMouseCursor.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[0].colorMouseCursor),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorShowMenu.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[1].showMenu),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorX.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[1].x),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorY.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[1].y),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorWidth.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[1].width),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorHeight.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[1].height),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorFullScreen.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[1].fullScreen),
        4
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorColorMouseCursor.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[1].colorMouseCursor),
        4
    );
    RegCloseKey(hKey);
}

VA(0x004c5a10, 0xa)
void WritePrefs(void) {
    UpdateSystemOptionsMenu();
    WritePrefsToRegistry();
}

VA(0x004c5a20, 0x3c)
i32 IsCDDrive(i32 driveIndex) {
    sprintf(gText, gMiscText.cd.rootDrive.text);
    gText[0] = gText[0] + driveIndex;
    return GetDriveTypeA(gText) == DRIVE_CDROM;
}

// @semantic
// Structurally complete /O2 checkpoint: both sections are 0x3ed with the same 0x2f0 frame,
// CFG and 51 relocation occurrences. Manual raw review confirms the indirect Win32/MCI
// targets; `homm2 relocs` reports only three delinker owner aliases (the archive literal and
// gText interior labels). The sole code-order residual is +0x19b: base loads mciSendStringA
// into ESI then wsprintfA into EBP, while retail loads EBP then ESI; every following use is
// identical. Standard `_O_BINARY`, `SEEK_*`, `KEY_WRITE`, typed buffers/results and a bounded
// 30-walk libclang AST pass preserve that residual. Revisit after an exact-preserving
// predecessor/TU-state change; this is not a proven artifact.
VA(0x004c5a60, 0x3ed)
i32 SetupCDDrive(void) {
    char registryPath[MISC_CD_PATH_BUFFER_SIZE];
    char registryKey[MISC_CD_PATH_BUFFER_SIZE];
    char cdDrives[MISC_CD_DRIVE_SLOT_COUNT];
    char count;
    i32 attempts;
    HKEY key;

    sprintf(gText, gMiscText.cd.dataArchive.text);
    i32 file = _open(gText, _O_BINARY);
    if (file == -1) {
        if (_chdir(gcRegAppPath) == -1)
            return MISC_CD_APP_PATH_UNAVAILABLE;
        file = _open(gText, _O_BINARY);
        if (file == -1)
            return MISC_CD_DATA_ARCHIVE_UNAVAILABLE;
    }
    _close(file);

    u32l logicalDrives = GetLogicalDrives();
    i32 cdDriveCount = 0;
    memset(cdDrives, 0, sizeof(cdDrives));
    for (i32 drive = MISC_CD_FIRST_DRIVE_INDEX; drive < MISC_CD_DRIVE_SLOT_COUNT; ++drive) {
        if (logicalDrives & (1 << drive)) {
            sprintf(gText, gMiscText.cd.rootDrive.text);
            gText[0] += static_cast<char>(drive);
            if (GetDriveTypeA(gText) == DRIVE_CDROM) {
                ++cdDriveCount;
                cdDrives[cdDriveCount - 1] = static_cast<char>(drive);
            }
        }
    }
    count = static_cast<char>(cdDriveCount);

    if (strlen(gcRegCDRomPath) != 0) {
        sprintf(gText, gMiscText.cd.configuredAnimationPath.text, gcRegCDRomPath);
        file = _open(gText, _O_BINARY);
        if (file != -1) {
            _close(file);
            sprintf(gText + 2, gMiscText.cd.stringFormat.text, gcAnimPath);
            strcpy(gcAnimPath, gText);
            return MISC_CD_DRIVE_READY;
        }
    }

    attempts = 0;
    {
        char resultBuffer[MISC_CD_MCI_BUFFER_SIZE];
        char command[MISC_CD_MCI_BUFFER_SIZE];
        for (;;) {
            for (i32 index = 0; index < count; ++index) {
                wsprintfA(command, gMiscText.cd.openAudioCommand.text, cdDrives[index] + 'A');
                if (mciSendStringA(command, resultBuffer, MISC_CD_MCI_RESULT_LENGTH, 0) == 0) {
                    wsprintfA(command, gMiscText.cd.audioInfoCommand.text);
                    mciSendStringA(command, resultBuffer, MISC_CD_MCI_RESULT_LENGTH, 0);
                    wsprintfA(command, gMiscText.cd.closeAudioCommand.text);
                    mciSendStringA(command, resultBuffer, MISC_CD_MCI_RESULT_LENGTH, 0);
                }
                sprintf(gText, gMiscText.cd.driveAnimationPath.text, cdDrives[index] + 'A');
                file = _open(gText, _O_BINARY);
                if (file != -1) {
                    if (_lseek(file, 0, SEEK_END) != -1
                        && _lseek(file, -MISC_CD_PROBE_TRAILER_SIZE, SEEK_CUR) != -1)
                        _read(file, resultBuffer, MISC_CD_PROBE_TRAILER_SIZE);
                    _close(file);

                    strcpy(registryKey, gMiscText.cd.registryKey.text);
                    key = 0;
                    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, registryKey, 0, KEY_WRITE, &key) == 0) {
                        wsprintfA(
                            registryPath,
                            gMiscText.cd.driveFormat.text,
                            cdDrives[index] + 'A'
                        );
                        RegSetValueExA(
                            key,
                            gMiscText.cd.cdDrive.text,
                            0,
                            REG_SZ,
                            reinterpret_cast<u8*>(registryPath),
                            lstrlenA(registryPath) + 1
                        );
                        RegCloseKey(key);
                    }
                    sprintf(
                        gText,
                        gMiscText.cd.driveAndPathFormat.text,
                        cdDrives[index] + 'A',
                        gcAnimPath
                    );
                    strcpy(gcAnimPath, gText);
                    return MISC_CD_DRIVE_READY;
                }
            }
            Sleep(MISC_CD_RETRY_DELAY_MILLISECONDS);
            ++attempts;
            if (attempts >= MISC_CD_RETRY_LIMIT)
                return MISC_CD_DRIVE_NOT_FOUND;
        }
    }
}

VA(0x004c5e50, 0x18)
void BitmapToScreen(class bitmap* bmp) {
    BlitBitmapToScreen(bmp, 0, 0, bmp->m_width, bmp->m_height, 0, 0);
}

VA(0x004c5e70, 0x3d)
void SetPalette(i8* paletteData, i32 updateDisplay) {
    memcpy(gpBufferPalette->m_data, paletteData, 0x300);
    memcpy(gCyclePal, paletteData + 0x282, 0x60);
    if (updateDisplay != 0)
        UpdatePalette(gpBufferPalette->m_data);
}

VA(0x004c5eb0, 0x25)
void BlitBitmapToScreenNoMouseCheck(
    class bitmap* bmp,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height,
    i32 destinationX,
    i32 destinationY
) {
    BlitBitmapToScreenVesa(bmp, sourceX, sourceY, width, height, destinationX, destinationY);
}

// @semantic
// Structurally complete /O2 checkpoint: retail decomp proves a real adjusted-source-X local;
// recovering it removes the prior unjustified volatile bitmap alias and gives the same 0x18b
// code span, four-blit CFG, and 24 relocation occurrences. Base reserves four stack bytes while
// retail reserves eight and spills bmp in the second slot. The current 88.61% live score is lower
// than the artifact-assisted retained 98.89%, but the source is now structurally honest. Manual
// relocation audit shows the same globals/callees and totals, with gpMouseManager CSE placement
// still differing: retail reloads between the horizontal/vertical entry tests, while base reloads
// between the later redraw tests. Flat versus axis-nested predicates are byte-neutral in this TU.
// Prior work exhausted relational reversal, width/source ordering, 22 local AST variants,
// 65 exact-predecessor variants, 149 deterministic predecessor combinations, and private-bound
// placement. Revisit only after a semantic predecessor/type change, never by restoring the
// volatile alias or inventing reloads.
VA(0x004c5ee0, 0x18b)
void BlitBitmapToScreen(
    class bitmap* bmp,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height,
    i32 destinationX,
    i32 destinationY
) {
    i32 blitSourceX = sourceX;
    if (gbColorMice == 0) {
        BlitBitmapToScreenVesa(
            bmp,
            blitSourceX,
            sourceY,
            width,
            height,
            destinationX,
            destinationY
        );
        return;
    }
    if (giScrollX != 0 || giScrollY != 0) {
        blitSourceX = giScrollX + MISC_BLIT_SCROLL_OFFSET;
        width = MISC_BLIT_SCROLL_EXTENT;
        sourceY = giScrollY + MISC_BLIT_SCROLL_OFFSET;
        height = MISC_BLIT_SCROLL_EXTENT;
    }
    gBlitRight = width + destinationX - 1;
    gBlitBottom = height + destinationY - 1;
    if (gpMouseManager->IsVis() != 0 && gBlitRight >= gpMouseManager->m_savedLeft
        && gpMouseManager->m_cursorRight >= destinationX
        && gBlitBottom >= gpMouseManager->m_savedTop
        && gpMouseManager->m_cursorBottom >= destinationY) {
        gpMouseManager->SaveAndDraw();
        BlitBitmapToScreenVesa(
            bmp,
            blitSourceX,
            sourceY,
            width,
            height,
            destinationX,
            destinationY
        );
        if (gpMouseManager->m_cursorRight > gBlitRight || gpMouseManager->m_savedLeft < destinationX
            || gpMouseManager->m_cursorBottom > gBlitBottom
            || gpMouseManager->m_savedTop < destinationY) {
            i32 savedY = gpMouseManager->m_savedTop;
            i32 savedX = gpMouseManager->m_savedLeft;
            BlitBitmapToScreenVesa(
                bmp,
                savedX,
                savedY,
                gpMouseManager->m_cursorRight - savedX + 1,
                gpMouseManager->m_cursorBottom - savedY + 1,
                savedX,
                savedY
            );
        }
        gpMouseManager->RestoreUnderlying();
        return;
    }
    BlitBitmapToScreenVesa(bmp, blitSourceX, sourceY, width, height, destinationX, destinationY);
}

#include <BASE/LogConstants.h>

// @semantic
// Structurally complete /O2 checkpoint: the 0x1f4 frame, CFG and 7/7 relocations agree.
// Only base +0x5c..+0x7c differs from retail +0x5c..+0x75: base loads the newline
// word before strlen and addresses via `not ecx`, while retail scans first and
// writes through `[edi-1]`. Direct word stores, strcat, strcpy-at-strlen, memcpy
// and a manual end scan were tried. At integrated head 56a50b7, the combined-TU
// strcat retest emitted a full second string scan/copy and regressed to 70.80%.
VA(0x004c6070, 0xa6)
void LogTruncate(void) {
    char logText[LOG_TEXT_BUFFER_SIZE];
    if (giDebugLevel >= LOG_FILE_DEBUG_LEVEL) {
        i32 fileHandle = _open(
            gMiscText.log.truncateFilename.text,
            _O_WRONLY | _O_CREAT | _O_TRUNC | _O_TEXT,
            _S_IWRITE
        );
        if (fileHandle != -1) {
            strcpy(logText, gMiscText.log.newLogLabel.text);
            *reinterpret_cast<u16*>(logText + strlen(logText)) =
                *reinterpret_cast<const u16*>(gMiscText.log.truncateNewline.text);
            _write(fileHandle, logText, strlen(logText));
            _close(fileHandle);
        }
    }
}

// LogTruncate matching history: `strchr(logText, '\0')` emitted an out-of-line call
// and regressed to 84.98%; it is not a viable spelling for the retail inline scan.

// @semantic
// Structurally complete /O2 checkpoint: the 0x1f4 frame, CFG and external targets
// agree. The newline append is base +0x53..+0x73 versus retail
// +0x4f..+0x6c (preloaded word/not-ECX versus post-scan `[edi-1]`); retail's raw
// OutputDebugString IAT operand at +0x8e is not named by delink. The same direct,
// strcpy-at-strlen, memcpy and manual-scan forms were tried. At integrated head
// 56a50b7, strcat emitted a full second string scan/copy and regressed to 64.70%.
VA(0x004c6120, 0x9e)
void LogStr(char* text) {
    char logText[LOG_TEXT_BUFFER_SIZE];
    if (giDebugLevel >= LOG_FILE_DEBUG_LEVEL) {
        FILE* logFile = fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
        if (logFile != 0) {
            strcpy(logText, text);
            *reinterpret_cast<u16*>(logText + strlen(logText)) =
                *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
            fputs(logText, logFile);
            fclose(logFile);
            if (giDebugLevel == LOG_DEBUGGER_OUTPUT_LEVEL)
                OutputDebugStringA(logText);
        }
    }
}

// @semantic
// Structurally complete /O2 checkpoint: the 0x2bc frame, all external targets and
// all seven sprintf call/format branches agree. Only the newline
// append differs (base +0x1c9..+0x1f2, retail +0x1c9..+0x1ec), followed by the
// unnamed retail OutputDebugString IAT operand at +0x211. Direct word, strcat,
// strcpy-at-strlen, memcpy and manual end-scan spellings were tried. Strcat regressed
// to 88.81%; a bounded libclang AST pass retained none after 30 walks, and 24 reversible
// predecessor/TU-state probes produced no exact closure or eligible score change.
VA(0x004c61c0, 0x224)
void LogInt(
    char* label,
    i32 value1,
    i32 value2,
    i32 value3,
    i32 value4,
    i32 value5,
    i32 value6,
    i32 value7
) {
    char text[LOG_FORMAT_BUFFER_SIZE];
    char logText[LOG_TEXT_BUFFER_SIZE];
    if (value7 != LOG_UNUSED_VALUE)
        sprintf(
            text,
            gMiscText.log.sevenValueFormat.text,
            label,
            value1,
            value2,
            value3,
            value4,
            value5,
            value6,
            value7
        );
    else if (value6 != LOG_UNUSED_VALUE)
        sprintf(
            text,
            gMiscText.log.sixValueFormat.text,
            label,
            value1,
            value2,
            value3,
            value4,
            value5,
            value6
        );
    else if (value5 != LOG_UNUSED_VALUE)
        sprintf(
            text,
            gMiscText.log.fiveValueFormat.text,
            label,
            value1,
            value2,
            value3,
            value4,
            value5
        );
    else if (value4 != LOG_UNUSED_VALUE)
        sprintf(text, gMiscText.log.fourValueFormat.text, label, value1, value2, value3, value4);
    else if (value3 != LOG_UNUSED_VALUE)
        sprintf(text, gMiscText.log.threeValueFormat.text, label, value1, value2, value3);
    else if (value2 != LOG_UNUSED_VALUE)
        sprintf(text, gMiscText.log.twoValueFormat.text, label, value1, value2);
    else
        sprintf(text, gMiscText.log.oneValueFormat.text, label, value1);
    if (giDebugLevel >= LOG_FILE_DEBUG_LEVEL) {
        FILE* file = fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
        if (file != 0) {
            strcpy(logText, text);
            *reinterpret_cast<u16*>(logText + strlen(logText)) =
                *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
            fputs(logText, file);
            fclose(file);
            if (giDebugLevel == LOG_DEBUGGER_OUTPUT_LEVEL)
                OutputDebugStringA(logText);
        }
    }
}

VA(0x004c63f0, 0x6c)
void AiPrint(char* text) {
    if (giDebugLevel >= 2) {
        FillBitmapArea(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0);
        smallFont->DrawBoundedString(text, 0, 0x1d0, 0x280, 0x10, 1, 0);
        BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0, 0x1cc);
    }
}

VA(0x004c6460, 0x7a)
void AbsAiPrint(char* text) {
    i32 saved = giDebugLevel;
    giDebugLevel = 9;
    FillBitmapArea(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0);
    smallFont->DrawBoundedString(text, 0, 0x1d0, 0x280, 0x10, 1, 0);
    BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0x1cc, 0x280, 0x14, 0, 0x1cc);
    giDebugLevel = saved;
}

// @semantic
// Structurally complete /O2 checkpoint: the typed graphics-domain boundary restores the
// retail 0x310 frame and makes every byte through PollSound at +0x55 exact, raising live
// from 86.18% to 95.12%. The 768-byte loop, CFG and all six relocation identities agree.
// The residual begins with threshold/cursor scheduling and then exchanges ECX/EBX roles
// in the byte adjustment. Signed/unsigned difference, abs/ternary and volatile level forms
// were tried earlier; at head 34c93d1, cursor/threshold lifetime order regressed to 92.79%
// and `distance > threshold` versus its commuted spelling was byte-neutral.
VA(0x004c64e0, 0xf8)
void FadeTo(u8* source, u8* destination, i32 increment) {
    u8 colors[MISC_PALETTE_BYTE_COUNT];
    memcpy(colors, source, sizeof(colors));
    increment >>= MISC_FADE_TO_INCREMENT_SHIFT;
    if (increment < 1)
        increment = 1;
    i32 level = MISC_FADE_TO_START_LEVEL;
    do {
        i32 delayUntil = KBTickCount() + MISC_FADE_TO_FRAME_DELAY;
        PollSound();
        i32 thresholdIndex = MISC_PALETTE_LEVEL_COUNT - level - increment;
        if (thresholdIndex < 0)
            thresholdIndex = 0;
        u8 threshold = giChangeThreshold[thresholdIndex];
        u8* current = colors;
        u8* target = destination;
        i32 count = MISC_PALETTE_BYTE_COUNT;
        do {
            u8 value = *current;
            i32 difference = static_cast<i32>(*target) - static_cast<i32>(value);
            i32 distance = difference < 0 ? -difference : difference;
            if (distance > threshold) {
                distance -= threshold;
                if (difference > 0)
                    *current = static_cast<u8>(value + distance);
                else
                    *current = static_cast<u8>(value - distance);
            }
            ++current;
            ++target;
            --count;
        } while (count != 0);
        UpdatePalette(reinterpret_cast<i8*>(colors));
        DelayTil(&delayUntil);
        level += increment;
    } while (level < MISC_PALETTE_LEVEL_COUNT);
    UpdatePalette(reinterpret_cast<i8*>(destination));
}

// @semantic
// Structurally complete /O2 checkpoint: base is 0xba versus retail 0xb8; both have the
// 0x304 frame, complete two-loop CFG, and all eight ordered relocation identities. The
// typed palette/screen constants leave live at 95.55% (retained 95.71%). Current residuals
// are equivalent SIB base/index encodings in the color-table/palette reads, FadeTo argument
// scheduling, and EAX/ECX exchange in the screen loop. Indexed output, pre/post-increment,
// explicit row pointers, linear pointer loop, inner/outer column scope and explicit pixel
// value were tried; at head 34c93d1, commuted palette-pointer addition was byte-neutral.
VA(0x004c65e0, 0xb8)
void FadeToColorTable(u8* colorTable, i32 increment) {
    u8 translatedPalette[MISC_PALETTE_BYTE_COUNT];
    i32 savedUpdateFlags = gpWindowManager->m_updateFlags;
    gpWindowManager->m_updateFlags = 0;
    i8* paletteData = gpBufferPalette->m_data;
    u8* output = translatedPalette;
    i32 index = 0;
    do {
        i32 paletteIndex = colorTable[index] * MISC_PALETTE_COMPONENT_BYTES;
        output += MISC_PALETTE_COMPONENT_BYTES;
        ++index;
        u8* sourceColor = reinterpret_cast<u8*>(paletteData) + paletteIndex;
        output[-3] = sourceColor[0];
        output[-2] = sourceColor[1];
        output[-1] = sourceColor[2];
    } while (output < translatedPalette + sizeof(translatedPalette));
    i32 rows = MISC_BLIT_SCREEN_HEIGHT;
    FadeTo(reinterpret_cast<u8*>(paletteData), translatedPalette, increment);
    i32 columns;
    u8* pixel = gpWindowManager->m_screen->m_pixels;
    do {
        columns = MISC_BLIT_SCREEN_WIDTH;
        do {
            u8 pixelValue = *pixel;
            *pixel = colorTable[pixelValue];
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
i32 IsCycleColor(i32 color) {
    if ((color >= 0xD6 && color <= 0xDD) || (color >= 0xE7 && color <= 0xED)) {
        return 1;
    }
    return 0;
}

// @semantic
// Structurally complete /O2 checkpoint: the 0x8c frame, all 14 ordered relocations,
// and every open/write/alloc/free/close target agree. The PCX header, RLE and VGA
// palette constants are now named in the private header. Differences are confined
// to the RLE loop (base +0x67..+0xe0 / retail +0x67..+0xdf), where EBX and EDI swap
// encoded-size/run-end roles. `<2`, `==1`, and `<=1` run tests, reordered locals,
// predicate polarity and commuted SIB forms were tried. At integrated head b8c73ff,
// a fresh bounded libclang AST pass found 23 single variants and retained none after
// 30 walks with all 144 siblings pinned.
VA(0x004c66d0, 0x1ee)
void CreatePCXFile(char* filename, u8* pixels, i32 width, i32 height, u8* paletteData) {
    PCXHeader header;
    memset(&header, 0, sizeof(header));
    header.manufacturer = PCX_MANUFACTURER_ZSOFT;
    header.version = PCX_VERSION_3_0;
    header.encoding = PCX_ENCODING_RLE;
    header.bitsPerPixel = PCX_BITS_PER_PIXEL;
    header.xMax = static_cast<u16>(width - 1);
    header.yMax = static_cast<u16>(height - 1);
    header.planes = PCX_PLANE_COUNT;
    header.bytesPerLine = static_cast<u16>(width);
    header.paletteType = PCX_PALETTE_TYPE_COLOR;
    i32 fileHandle = _open(filename, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fileHandle == -1)
        return;
    _write(fileHandle, &header, sizeof(header));
    u8* encodedRow =
        static_cast<u8*>(H2_ALLOC(width * 2, gMiscText.pcx.encodedRowAllocation.text, 0x5c8));
    for (i32 row = 0; row < height; ++row) {
        i32 sourceIndex = 0;
        u32 encodedSize = 0;
        while (sourceIndex < width) {
            u8 value = pixels[sourceIndex];
            i32 runEnd = sourceIndex;
            while (runEnd < width && pixels[runEnd] == value
                   && runEnd - sourceIndex + 1 < PCX_RLE_RUN_LIMIT)
                ++runEnd;
            i32 runLength = runEnd - sourceIndex;
            if (runLength <= 1 && (value & PCX_RLE_RUN_MARKER) != PCX_RLE_RUN_MARKER) {
                encodedRow[encodedSize++] = value;
                ++sourceIndex;
            } else {
                encodedRow[encodedSize++] = static_cast<u8>(runLength | PCX_RLE_RUN_MARKER);
                encodedRow[encodedSize++] = value;
                sourceIndex += runLength;
            }
        }
        _write(fileHandle, encodedRow, encodedSize);
        pixels += width;
    }
    H2_FREE(encodedRow, gMiscText.pcx.encodedRowDestruction.text, 0x5f0);
    u8 paletteMarker = PCX_VGA_PALETTE_MARKER;
    _write(fileHandle, &paletteMarker, 1);
    u8* outputPalette = static_cast<u8*>(
        H2_ALLOC(PCX_PALETTE_BYTE_COUNT, gMiscText.pcx.outputPaletteAllocation.text, 0x5f6)
    );
    for (i32 i = 0; i < PCX_PALETTE_BYTE_COUNT; ++i)
        outputPalette[i] = paletteData[i] << PCX_COMPONENT_SCALE_SHIFT;
    _write(fileHandle, outputPalette, PCX_PALETTE_BYTE_COUNT);
    H2_FREE(outputPalette, gMiscText.pcx.outputPaletteDestruction.text, 0x5fb);
    _close(fileHandle);
}

VA(0x004c68c0, 0x52)
i32l FileSize(char* filename) {
    FILE* file = fopen(filename, gMiscText.file.readWriteBinaryMode.text);
    if (file == 0) {
        FileError(filename);
    }
    fseek(file, 0, 2);
    i32l size = ftell(file);
    fseek(file, 0, 0);
    fclose(file);
    return size;
}

VA(0x004c6920, 0xc)
struct IconEntry* GetIconEntry(class icon* iconPtr, i32 index) {
    return &iconPtr->Entries()[index];
}

#include <BASE/SeededRandomConstants.h>

// @semantic
// Structurally complete /O2 checkpoint: base and retail are both 0xb8 with the same
// seeded-random CFG and all 3 ordered relocations. Moving the result lifetime before
// the mix raised the retained maximum from 67.80% to 91.87%. Reusing `mix` for the
// post-loop seed and naming the range restores retail's first `add edi,esi`; live is
// 91.87%. The bit test still has the symmetric operand encoding, then base folds the
// high contribution into ECX and stores before idiv while retail keeps EDI live and
// stores after idiv. Direct term locals, folded/incremental seed arithmetic, both
// final-update orders, explicit bit-loop forms, commuted bit tests and a volatile seed
// store were checked. In-place modulo regressed to 88.13%; 24 reversible TU-state
// probes found no exact closure, so every generated variant was discarded.
VA(0x004c6930, 0xb8)
i32 SRandom(i32 low, i32 high) {
    if (high == low) {
        return high;
    }
    if (high < low) {
        return low;
    }

    i32 highTerm = (high * SEEDED_RANDOM_TERM_MULTIPLIER) & SEEDED_RANDOM_TERM_MASK;
    i32 lowTerm = (low * SEEDED_RANDOM_TERM_MULTIPLIER) & SEEDED_RANDOM_TERM_MASK;
    iLastSeed += highTerm << SEEDED_RANDOM_HIGH_TERM_SHIFT;
    iLastSeed += lowTerm * SEEDED_RANDOM_LOW_TERM_MULTIPLIER;
    iLastSeed += highTerm;
    iLastSeed += (iLastSeed & SEEDED_RANDOM_FEEDBACK_MASK) << SEEDED_RANDOM_FEEDBACK_SHIFT;
    iLastSeed &= SEEDED_RANDOM_SEED_MASK;

    i32 result = 0;
    i32 mix = iLastSeed * SEEDED_RANDOM_MIX_MULTIPLIER;
    mix += (mix & SEEDED_RANDOM_MIX_MASK) >> SEEDED_RANDOM_MIX_SHIFT;
    for (i32 i = SEEDED_RANDOM_TOP_BIT; i >= 0; --i) {
        if (mix & (1 << i)) {
            result |= 1 << i;
        }
    }
    mix += low;
    i32 range = high - low;
    mix += high * 8;
    i32 rangedResult = low + result % (range + 1);
    iLastSeed = mix;
    return rangedResult;
}

// @semantic: Complete seeded-coordinate mixing semantics and both ordered
// iLastSeed relocations agree. The 0x5c retail register allocation keeps x in
// ESI, y in EDX, and the seed in ECX; base assigns EDI/ECX/EDX respectively.
// A direct xTerm/yTerm/seed-local form regressed to 46.53%, so the compact
// in-place expression is retained. Revisit after Misc TU-state changes.
VA(0x004c69f0, 0x5c)
void SIncRandomize(i32 x, i32 y) {
    x *= SEEDED_RANDOM_TERM_MULTIPLIER;
    x &= SEEDED_RANDOM_TERM_MASK;
    y *= SEEDED_RANDOM_TERM_MULTIPLIER;
    y &= SEEDED_RANDOM_TERM_MASK;
    iLastSeed += y << SEEDED_RANDOM_HIGH_TERM_SHIFT;
    iLastSeed += x * SEEDED_RANDOM_LOW_TERM_MULTIPLIER;
    iLastSeed += y;
    iLastSeed += (iLastSeed & SEEDED_RANDOM_FEEDBACK_MASK) << SEEDED_RANDOM_FEEDBACK_SHIFT;
}

VA(0x004c6a50, 0x10)
void SRand(i32 seed) {
    iLastSeed = seed;
    srand(seed);
}

// @semantic
// Structurally complete /O2 checkpoint: base and retail are both 0x48 bytes and
// all 3 relocations agree. The retained 90.64% state differed only in the six-byte
// iLastSeed store: base placed it before the bit loop and retail after `dec ecx` inside
// the loop. Splitting the RNG/data-entry enum domains leaves live at 90.18% with one
// additional symmetric `test` operand encoding. for/do-while placement, commuted bit
// tests, assignment in the loop condition and a volatile store were tried; the condition
// spelling was byte-neutral, volatile spills the mix, and nonvolatile forms are hoisted.
// Twenty-four reversible predecessor/TU-state probes found no exact closure; their
// sub-100 disposable improvement was discarded.
VA(0x004c6a60, 0x48)
i32 SGenRand(void) {
    i32 result = 0;
    iLastSeed &= SEEDED_RANDOM_SEED_MASK;
    i32 mix = iLastSeed * SEEDED_RANDOM_MIX_MULTIPLIER;
    mix += (mix & SEEDED_RANDOM_MIX_MASK) >> SEEDED_RANDOM_MIX_SHIFT;
    i32 i = SEEDED_RANDOM_TOP_BIT;
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
i32 MemSize(i32) {
    return 0x3ea2;
}

#include <BASE/DataEntryConstants.h>

// @semantic
// Structurally aligned /O2 checkpoint: both code spans are 0x386, the 0x9c frame,
// CFG and 59 relocation occurrences agree. The recovered conditional Y adjustment leaves
// only 12 unmasked bytes: one `mov ecx,0x2d` schedule at +0xbc and swapped LEAs for
// entryText/message near +0x141, plus three delinked empty-string owner names.
// Boolean-mask/branched Y forms, direct/local empty strings and declaration order
// were tried. A bounded libclang AST pass found 25 single variants and retained none
// after 30 walks. The current manual relocation audit remains 59/59; its four only-base
// reports are delinked gpMouseManager/empty-string owner identities. Revisit only after
// exact-preserving predecessor/TU state changes.
VA(0x004c6ac0, 0x386)
void GetDataEntry(
    char* prompt,
    char* destination,
    i32 maximumLength,
    char* initialText,
    i32 showCancel,
    i32 useImmediateHandler
) {
    i32 savedCursorType = gpMouseManager->m_cursorType;
    i32 savedCursorFrame = gpMouseManager->m_cursorFrame;
    while (gpMouseManager->m_hideCount != 0)
        gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer(gMiscText.dataEntry.mouseFilename.text, 0, MOUSE_AUTO_CURSOR_TYPE);

    cDEDest = destination;
    iDEMaxLen = maximumLength;
    strcpy(destination, gMiscText.dataEntry.destinationDefault.text);

    i32 rows = bigFont->LineLength(prompt, 0xf0) * 0x10;
    if (showCancel != 0)
        rows += 0x27;
    rows = (rows + 0xf) / 0x2d;
    if (rows > 6)
        rows = 6;
    i32 entryY = rows * 0x2d - (showCancel != 0 ? 0x1e : 0) + 0x5f;

    char windowName[16];
    sprintf(windowName, gMiscText.dataEntry.windowFilenameFormat.text, rows);
    DataEntryWin = new heroWindow(0xb1, 0x14, windowName);
    if (DataEntryWin == 0)
        MemError();

    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = DATA_ENTRY_PROMPT_WIDGET;
    message.payload.widget.data.text = prompt;
    DataEntryWin->BroadcastMessage(message);

    char entryText[100];
    if (initialText == 0)
        initialText = gMiscText.dataEntry.initialTextDefault.text;
    strcpy(entryText, initialText);
    message.payload.widget.id = DATA_ENTRY_TEXT_WIDGET;
    message.payload.widget.data.text = entryText;
    DataEntryWin->BroadcastMessage(message);
    strcpy(destination, entryText);

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.id = DATA_ENTRY_BUTTON_ONE;
    message.payload.widget.data.value = WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = DATA_ENTRY_BUTTON_SEVEN;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = DATA_ENTRY_BUTTON_EIGHT;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = DATA_ENTRY_BUTTON_FIVE;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = DATA_ENTRY_BUTTON_SIX;
    DataEntryWin->BroadcastMessage(message);
    if (showCancel == 0) {
        message.payload.widget.id = DATA_ENTRY_CANCEL_BUTTON;
        DataEntryWin->BroadcastMessage(message);
    }

    textEntryWidget* entry = new textEntryWidget(
        0x23,
        static_cast<i16>(entryY),
        0xfb,
        0x14,
        static_cast<i16>(maximumLength),
        destination,
        gMiscText.dataEntry.fontFilename.text,
        0,
        gMiscText.dataEntry.iconFilename.text,
        3,
        10,
        0,
        4,
        10,
        3
    );
    if (entry == 0)
        MemError();
    inBoxY = entryY + 0x17;
    inBoxX = 0xd5;
    DataEntryWin->AddWidget(entry, -1);

    if (useImmediateHandler != 0) {
        bDataEntryTime = DATA_ENTRY_PHASE_IMMEDIATE;
        gbAllowTextEntryEscape = 0;
    } else
        bDataEntryTime = DATA_ENTRY_PHASE_READY;
    gpWindowManager->DoDialog(DataEntryWin, DataEntryWindowHandler, 0);
    delete DataEntryWin;
    gpMouseManager->SetPointer(
        gMiscText.dataEntry.restoredMouseFilename.text,
        savedCursorFrame,
        savedCursorType
    );
    gbAllowTextEntryEscape = 1;
}

// @early-stop
// Structurally complete /O2 checkpoint: the command-domain switch preserves the
// retail case-body order, including the physical cancel tail. Base and retail are
// both 0x173 with an identical relocation-masked instruction stream and 23 relocation
// occurrences. The remaining raw residual is one exchanged near/short JE and one near/short
// JNE; the
// total size is unchanged. The current manual audit remains 23/23 relocations; its seven
// only-base reports are interior aliases for the same DataEntry globals. A bounded libclang
// AST pass tested 16 variants in 30 walks and retained none. Revisit after an
// exact-preserving predecessor/TU-state change.
VA(0x004c6e50, 0x173)
i32 DataEntryWindowHandler(struct tag_message& message) {
    if (bDataEntryTime == DATA_ENTRY_PHASE_IMMEDIATE) {
        ++bDataEntryTime;
        message.type = MESSAGE_LEFT_BUTTON_DOWN;
        message.payload.mouse.x = inBoxX;
        message.payload.mouse.y = inBoxY;
        DataEntryWin->BroadcastMessage(message);
        return EVENT_WINDOW_CONTINUE;
    }

    if (bDataEntryTime == DATA_ENTRY_PHASE_POINTER_SENT)
        ++bDataEntryTime;
    else {
        if (message.type != MESSAGE_WIDGET)
            goto normalEvent;
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
                if (message.payload.widget.id != DATA_ENTRY_TEXT_WIDGET)
                    goto normalEvent;
                break;
            case WIDGET_COMMAND_DESELECT:
                goto possibleCancelEvent;
            default:
                goto normalEvent;
        }
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.id = DATA_ENTRY_TEXT_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
    DataEntryWin->BroadcastMessage(message);
    if (strlen(message.payload.widget.data.text) != 0) {
        memset(cDEDest, 0, iDEMaxLen);
        strncpy(cDEDest, message.payload.widget.data.text, iDEMaxLen - 1);
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message.payload.widget.id = DATA_ENTRY_TEXT_WIDGET;
        message.payload.widget.data.text = cDEDest;
        DataEntryWin->BroadcastMessage(message);
        DataEntryWin->DrawWindow(1, 10, 10);
        if (gbTextEntryEscaped == 0) {
            gpWindowManager->m_dialogResult = message.payload.widget.id;
            message.payload.widget.id = DATA_ENTRY_TEXT_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
            return EVENT_WINDOW_CLOSE;
        }
    }

possibleCancelEvent:
    if (message.payload.widget.id != DATA_ENTRY_CANCEL_BUTTON)
        goto normalEvent;
    message.payload.widget.id = DATA_ENTRY_TEXT_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
    return EVENT_WINDOW_CLOSE;

normalEvent:
    return EventWindowHandler(message);
}

// ---- zero-fill globals (definitions, RVA order) ----
DATA(0x005331c4) class heroWindow* DataEntryWin;
DATA(0x005331c8) char* cDEDest;
DATA(0x005331d4) i32 iDEMaxLen;
DATA(0x005331d8) i32 bDataEntryTime;
DATA(0x005331dc) i32 inBoxX;
DATA(0x005331e0) i32 inBoxY;
