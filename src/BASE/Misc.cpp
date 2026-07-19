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
#include <BASE/MISC_TYPES.h>
H2_ENUM_BEGIN(DataEntryLayout)
    WINDOW_X                    = 0xb1,
    WINDOW_Y                    = 0x14,
    INPUT_BOX_Y_OFFSET          = 0x17,
    PROMPT_WIDTH                = 240,
    PROMPT_LINE_HEIGHT          = 16,
    CANCEL_PROMPT_HEIGHT        = 39,
    ROW_ROUNDING_BIAS           = 15,
    ROW_HEIGHT                  = 45,
    MAX_ROW_COUNT               = 6,
    CANCEL_Y_OFFSET             = 30,
    ENTRY_BASE_Y                = 95,
    WINDOW_NAME_CAPACITY        = 16,
    TEXT_BUFFER_CAPACITY        = 100,
    TEXT_FIELD_X                = 35,
    TEXT_FIELD_WIDTH            = 251,
    TEXT_FIELD_HEIGHT           = 20,
    TEXT_FIELD_COLOR            = 0,
    TEXT_FIELD_ICON_FRAME       = 3,
    TEXT_FIELD_KIND             = 0,
    TEXT_FIELD_LAYOUT           = 4,
    TEXT_FIELD_HORIZONTAL_INSET = 10,
    TEXT_FIELD_VERTICAL_INSET   = 3,
    INPUT_BOX_X                 = 213,
    REDRAW_OFFSET               = 10,
    DRAW_MODE                   = 1,
    WIDGET_Z_ORDER              = -1
H2_ENUM_END(DataEntryLayout)

H2_ENUM_CLASS_BEGIN(DataEntryPhase)
    ENTRY_PHASE_IMMEDIATE    = 0,
    ENTRY_PHASE_POINTER_SENT = 1,
    ENTRY_PHASE_READY        = 2
H2_ENUM_CLASS_END(DataEntryPhase)

H2_ENUM_BEGIN(DataEntryWidgetId)
    ENTRY_PROMPT_WIDGET = 1,
    ENTRY_TEXT_WIDGET   = 10,
    ENTRY_BUTTON_ONE    = 0x7801,
    ENTRY_CANCEL_BUTTON = 0x7802,
    ENTRY_BUTTON_FIVE   = 0x7805,
    ENTRY_BUTTON_SIX    = 0x7806,
    ENTRY_BUTTON_SEVEN  = 0x7807,
    ENTRY_BUTTON_EIGHT  = 0x7808
H2_ENUM_END(DataEntryWidgetId)

H2_ENUM_BEGIN(LogConstant)
    MEMORY_LEAK_DEBUG_LEVEL   = 1,
    FILE_DEBUG_LEVEL          = 2,
    DEBUGGER_OUTPUT_LEVEL     = 4,
    FORCED_DEBUG_LEVEL        = 9,
    UNUSED_VALUE              = -999,
    FORMAT_BUFFER_SIZE        = 200,
    TEXT_BUFFER_SIZE          = 500,
    MEMORY_ENTRY_CAPACITY     = 2000,
    REPORTED_MEMORY_KILOBYTES = 16034,
    ENTRY_SEARCH_COMPLETE     = 99999
H2_ENUM_END(LogConstant)

H2_ENUM_BEGIN(MiscGameDefaultConstant)
    DEFAULT_WINDOW_ORIGIN        = 10,
    DEFAULT_SMALL_WINDOW_WIDTH   = 0x1e0,
    DEFAULT_SMALL_WINDOW_HEIGHT  = 0x168,
    DEFAULT_WINDOW_WIDTH         = 0x280,
    DEFAULT_WINDOW_HEIGHT        = 0x1e0,
    DEFAULT_SLOW_VIDEO           = 3,
    DEFAULT_COMPUTER_WALK_SPEED  = 3,
    DEFAULT_WALK_SPEED           = 2,
    DEFAULT_MAP_OFFSET_COUNT     = 32001,
    UNIQUE_ID_RANDOM_MODULUS     = 999999,
    UNIQUE_ID_ALPHANUMERIC_COUNT = 36,
    UNIQUE_ID_ALPHA_COUNT        = 26,
    UNIQUE_ID_LEADING_INDEX      = 0,
    UNIQUE_ID_MIDDLE_INDEX       = 1,
    UNIQUE_ID_TRAILING_INDEX     = 2,
    UNIQUE_ID_TERMINATOR_INDEX   = 3
H2_ENUM_END(MiscGameDefaultConstant)

H2_ENUM_CLASS_BEGIN(MiscGraphicsFieldIndex)
    GRAPHICS_SHOW_MENU_FROM_FULLSCREEN   = -5,
    GRAPHICS_X_FROM_FULLSCREEN           = -4,
    GRAPHICS_Y_FROM_FULLSCREEN           = -3,
    GRAPHICS_WIDTH_FROM_FULLSCREEN       = -2,
    GRAPHICS_HEIGHT_FROM_FULLSCREEN      = -1,
    GRAPHICS_FULLSCREEN                  = 0,
    GRAPHICS_COLOR_MOUSE_FROM_FULLSCREEN = 1
H2_ENUM_CLASS_END(MiscGraphicsFieldIndex)

H2_ENUM_BEGIN(MiscCDDriveConstant)
    CD_FIRST_DRIVE_INDEX        = 2,
    CD_DRIVE_SLOT_COUNT         = 26,
    CD_PATH_PREFIX_BYTES        = 2,
    CD_PATH_BUFFER_SIZE         = 100,
    CD_MCI_BUFFER_SIZE          = 256,
    CD_MCI_RESULT_LENGTH        = 0xFF,
    CD_PROBE_TRAILER_SIZE       = 100,
    CD_RETRY_DELAY_MILLISECONDS = 3000,
    CD_RETRY_LIMIT              = 2
H2_ENUM_END(MiscCDDriveConstant)

H2_ENUM_CLASS_BEGIN(MiscCDDriveResult)
    CD_DRIVE_READY              = 0,
    CD_DRIVE_NOT_FOUND          = 2,
    CD_APP_PATH_UNAVAILABLE     = 3,
    CD_DATA_ARCHIVE_UNAVAILABLE = 4
H2_ENUM_CLASS_END(MiscCDDriveResult)

H2_ENUM_BEGIN(PCXConstant)
    MANUFACTURER_ZSOFT    = 10,
    VERSION_3_0           = 5,
    ENCODING_RLE          = 1,
    BITS_PER_PIXEL        = 8,
    PLANE_COUNT           = 1,
    PALETTE_TYPE_COLOR    = 1,
    RLE_RUN_MARKER        = 0xc0,
    RLE_RUN_LIMIT         = 0x40,
    VGA_PALETTE_MARKER    = 0x0c,
    PALETTE_BYTE_COUNT    = 0x300,
    COMPONENT_SCALE_SHIFT = 2
H2_ENUM_END(PCXConstant)

H2_ENUM_CLASS_BEGIN(MiscCycleColorRange)
    CYCLE_RANGE_ONE_FIRST = 0xd6,
    CYCLE_RANGE_ONE_LAST  = 0xdd,
    CYCLE_RANGE_TWO_FIRST = 0xe7,
    CYCLE_RANGE_TWO_LAST  = 0xed
H2_ENUM_CLASS_END(MiscCycleColorRange)

H2_ENUM_BEGIN(MiscFadeConstant)
    FADE_LEVEL_COUNT              = 0x40,
    FADE_LEVEL_LAST               = 0x3f,
    FADE_CHANGE_THRESHOLD_COUNT   = 16,
    FADE_FRAME_DELAY              = 0x14,
    WINDOWED_FADE_INCREMENT_SCALE = 2,
    FADE_TO_INCREMENT_SHIFT       = 2,
    FADE_TO_START_LEVEL           = 0x30,
    FADE_TO_FRAME_DELAY           = 0x32
H2_ENUM_END(MiscFadeConstant)

H2_ENUM_BEGIN(MiscPaletteComponent)
    PALETTE_COMPONENT_COUNT     = 3,
    PALETTE_RED_INDEX           = 0,
    PALETTE_GREEN_INDEX         = 1,
    PALETTE_BLUE_INDEX          = 2,
    PALETTE_RED_OUTPUT_OFFSET   = -3,
    PALETTE_GREEN_OUTPUT_OFFSET = -2,
    PALETTE_BLUE_OUTPUT_OFFSET  = -1
H2_ENUM_END(MiscPaletteComponent)

H2_ENUM_BEGIN(MiscWindowConstant)
    MINIMUM_WINDOW_WIDTH   = 320,
    MINIMUM_WINDOW_HEIGHT  = 240,
    WINDOW_POSITION_MARGIN = 200
H2_ENUM_END(MiscWindowConstant)

H2_ENUM_BEGIN(MiscBlitConstant)
    BLIT_SCROLL_OFFSET = 0x10,
    BLIT_SCROLL_EXTENT = 0x1c0,
    BLIT_SCREEN_WIDTH  = 0x280,
    BLIT_SCREEN_HEIGHT = 0x1e0
H2_ENUM_END(MiscBlitConstant)

H2_ENUM_BEGIN(SeededRandomConstant)
    INITIAL_SEED               = 0x08156a03,
    RANDOM_TERM_MULTIPLIER     = 13,
    RANDOM_TERM_MASK           = 0xFF,
    RANDOM_HIGH_TERM_SHIFT     = 5,
    RANDOM_LOW_TERM_MULTIPLIER = 13233,
    RANDOM_FEEDBACK_MASK       = 0x3f,
    RANDOM_FEEDBACK_SHIFT      = 8,
    RANDOM_SEED_MASK           = 0xfff,
    RANDOM_MIX_MULTIPLIER      = 7,
    RANDOM_MIX_MASK            = 0xff0,
    RANDOM_MIX_SHIFT           = 4,
    RANDOM_TOP_BIT             = 31,
    RANDOM_HIGH_MIX_MULTIPLIER = 8
H2_ENUM_END(SeededRandomConstant)

H2_ENUM_BEGIN(FileIdHashConstant)
    HASH_LEFT_SHIFT  = 5,
    HASH_RIGHT_SHIFT = 25,
    INDEX_NOT_FOUND  = 0xffff
H2_ENUM_END(FileIdHashConstant)

#undef HOMM2_MISC_INLINE_ICONENTRY
#include <BASE/miscwin.h>
#include <SOURCE/KB.h>
#include <SOURCE/wingraph.h>
#include <SOURCE/NOOPT.h>
#include <BASE/message.h>
#include <windows.h>
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

DATA(0x005331cc) static i32 gBlitRight;  // BlitBitmapToScreen computed blit-rect right edge
DATA(0x005331d0) static i32 gBlitBottom; // BlitBitmapToScreen computed blit-rect bottom edge

DATA(0x0051dce8) i32 iMemEntries = 0;
DATA(0x0051dcec) MemEntry* gpMemEntry = NULL;
DATA(0x0051dcf0) i32 giTotalMemAllocated = 0;
DATA(0x0051dcf8) u8
    giChangeThreshold[FADE_CHANGE_THRESHOLD_COUNT] =
        {0, 1, 2, 3, 4, 6, 8, 10, 13, 16, 19, 22, 26, 31, 37, 46};
DATA(0x0051dd08) i32 iLastSeed = INITIAL_SEED;
DATA(0x0051dd0c) static char gMemEntryTag[sizeof("IME")] = "IME";

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

H2_ENUM_BEGIN(StatusBarLayout)
    STATUS_BAR_WIDTH   = 640,
    STATUS_BAR_Y       = 460,
    STATUS_BAR_HEIGHT  = 20,
    STATUS_TEXT_Y      = 464,
    STATUS_TEXT_HEIGHT = 16
H2_ENUM_END(StatusBarLayout)

VA(0x004c3d10, 0x58)
void InitMemEntry(void) {
    LogInt(gMemEntryTag, iMemEntries, UNUSED_VALUE, UNUSED_VALUE, UNUSED_VALUE, UNUSED_VALUE,
           UNUSED_VALUE, UNUSED_VALUE);
    gpMemEntry = static_cast<MemEntry*>(malloc(MEMORY_ENTRY_CAPACITY * sizeof(MemEntry)));
    for (i32 i = 0; i < MEMORY_ENTRY_CAPACITY; ++i)
        gpMemEntry[i].used = 0;
}

VA(0x004c3d70, 0x20f)
void* BaseAlloc(u32 size, char* originalFile, i32 originalLine) {
    char text[FORMAT_BUFFER_SIZE];
    char logText[TEXT_BUFFER_SIZE];
    if (size == 0)
        return NULL;
    if (gpMemEntry == NULL) {
        LogInt(gMemEntryTag, iMemEntries, UNUSED_VALUE, UNUSED_VALUE, UNUSED_VALUE,
               UNUSED_VALUE, UNUSED_VALUE, UNUSED_VALUE);
        gpMemEntry =
            static_cast<MemEntry*>(malloc(MEMORY_ENTRY_CAPACITY * sizeof(MemEntry)));
        for (i32 initIndex = 0; initIndex < MEMORY_ENTRY_CAPACITY; ++initIndex)
            gpMemEntry[initIndex].used = 0;
    }
    giTotalMemAllocated += size;
    void* ptr = malloc(size);
    if (ptr == NULL) {
        MemError();
        return NULL;
    }
    ++iMemEntries;
    i32 entryIndex;
    for (entryIndex = 0; entryIndex < MEMORY_ENTRY_CAPACITY; ++entryIndex) {
        if (!gpMemEntry[entryIndex].used) {
            gpMemEntry[entryIndex].used = 1;
            gpMemEntry[entryIndex].ptr = ptr;
            gpMemEntry[entryIndex].size = size;
            strcpy(gpMemEntry[entryIndex].file, originalFile);
            gpMemEntry[entryIndex].line = originalLine;
            entryIndex = ENTRY_SEARCH_COMPLETE;
        }
    }
    if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL) {
        sprintf(
            text,
            gMiscText.memory.allocationFormat.text,
            size,
            ptr,
            originalFile,
            originalLine
        );
        if (giDebugLevel >= FILE_DEBUG_LEVEL) {
            FILE* logFile = fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
            if (logFile != NULL) {
                strcpy(logText, text);
                *reinterpret_cast<u16*>(logText + strlen(logText)) =
                    *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
                fputs(logText, logFile);
                fclose(logFile);
                if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
                    OutputDebugStringA(logText);
            }
        }
    }
    return ptr;
}

VA(0x004c3f80, 0x386)
void BaseFree(void* ptr, char* originalFile, i32 originalLine) {
    char logText[TEXT_BUFFER_SIZE];
    char text[FORMAT_BUFFER_SIZE];
    if (gpMemEntry == NULL) {
        LogInt(gMemEntryTag, iMemEntries, UNUSED_VALUE, UNUSED_VALUE, UNUSED_VALUE,
               UNUSED_VALUE, UNUSED_VALUE, UNUSED_VALUE);
        gpMemEntry =
            static_cast<MemEntry*>(malloc(MEMORY_ENTRY_CAPACITY * sizeof(MemEntry)));
        for (i32 initIndex = 0; initIndex < MEMORY_ENTRY_CAPACITY; ++initIndex)
            gpMemEntry[initIndex].used = 0;
    }
    if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
        LogInt(
            gMiscText.memory.freeLabel.text,
            reinterpret_cast<i32>(ptr),
            UNUSED_VALUE,
            UNUSED_VALUE,
            UNUSED_VALUE,
            UNUSED_VALUE,
            UNUSED_VALUE,
            UNUSED_VALUE
        );
    if (ptr == NULL) {
        if (giDebugLevel >= FILE_DEBUG_LEVEL) {
            FILE* logFile = fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
            if (logFile != NULL) {
                strcpy(logText, gMiscText.memory.nullPointer.text);
                *reinterpret_cast<u16*>(logText + strlen(logText)) =
                    *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
                fputs(logText, logFile);
                fclose(logFile);
                if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
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
            UNUSED_VALUE,
            UNUSED_VALUE,
            UNUSED_VALUE,
            UNUSED_VALUE,
            UNUSED_VALUE,
            UNUSED_VALUE
        );
    i32 entryIndex;
    for (entryIndex = 0; entryIndex < MEMORY_ENTRY_CAPACITY; ++entryIndex) {
        if (gpMemEntry[entryIndex].ptr == ptr) {
            if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL) {
                sprintf(
                    text,
                    gMiscText.memory.freeFormat.text,
                    gpMemEntry[entryIndex].size,
                    ptr,
                    gpMemEntry[entryIndex].file,
                    gpMemEntry[entryIndex].line
                );
                if (giDebugLevel >= FILE_DEBUG_LEVEL) {
                    FILE* logFile =
                        fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
                    if (logFile != NULL) {
                        strcpy(logText, text);
                        *reinterpret_cast<u16*>(logText + strlen(logText)) =
                            *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
                        fputs(logText, logFile);
                        fclose(logFile);
                        if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
                            OutputDebugStringA(logText);
                    }
                }
            }
            gpMemEntry[entryIndex].used = 0;
            giTotalMemAllocated -= gpMemEntry[entryIndex].size;
            entryIndex = ENTRY_SEARCH_COMPLETE;
        }
    }
    if (entryIndex < ENTRY_SEARCH_COMPLETE) {
        sprintf(gText, gMiscText.memory.badDeleteFormat.text, originalFile, originalLine, ptr);
        if (giDebugLevel >= FILE_DEBUG_LEVEL) {
            FILE* logFile = fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
            if (logFile != NULL) {
                strcpy(logText, gText);
                *reinterpret_cast<u16*>(logText + strlen(logText)) =
                    *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
                fputs(logText, logFile);
                fclose(logFile);
                if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
                    OutputDebugStringA(logText);
            }
        }
    } else {
        free(ptr);
    }
}

VA(0x004c4310, 0x134)
void PrintMemoryLeaks(void) {
    char logText[TEXT_BUFFER_SIZE];
    if (giDebugLevel >= MEMORY_LEAK_DEBUG_LEVEL && gpMemEntry != NULL) {
        LogInt(
            gMiscText.memory.leakCountLabel.text,
            iMemEntries,
            UNUSED_VALUE,
            UNUSED_VALUE,
            UNUSED_VALUE,
            UNUSED_VALUE,
            UNUSED_VALUE,
            UNUSED_VALUE
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
                if (giDebugLevel >= FILE_DEBUG_LEVEL) {
                    FILE* logFile =
                        fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
                    if (logFile != NULL) {
                        strcpy(logText, gText);
                        *reinterpret_cast<u16*>(logText + strlen(logText)) =
                            *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
                        fputs(logText, logFile);
                        fclose(logFile);
                        if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
                            OutputDebugStringA(logText);
                    }
                }
            }
            entryIndex = entryIndex + 1;
        } while (entryIndex < MEMORY_ENTRY_CAPACITY);
    }
}

VA(0x004c4450, 0x91)
void ShowMemoryStatus(void) {
    sprintf(gText, gMiscText.memory.memoryStatusFormat.text, REPORTED_MEMORY_KILOBYTES);
    i32 savedDebugLevel = giDebugLevel;
    giDebugLevel = FORCED_DEBUG_LEVEL;
    FillBitmapArea(
        gpWindowManager->m_screen, 0, STATUS_BAR_Y, STATUS_BAR_WIDTH, STATUS_BAR_HEIGHT, 0
    );
    smallFont->DrawBoundedString(
        gText,
        0,
        STATUS_TEXT_Y,
        STATUS_BAR_WIDTH,
        STATUS_TEXT_HEIGHT,
        FONT_DRAW_DEFAULT,
        FONT_ALIGN_LEFT
    );
    BlitBitmapToScreen(
        gpWindowManager->m_screen,
        0,
        STATUS_BAR_Y,
        STATUS_BAR_WIDTH,
        STATUS_BAR_HEIGHT,
        0,
        STATUS_BAR_Y
    );
    giDebugLevel = savedDebugLevel;
}

VA(0x004c44f0, 0x48)
u32l MAKEFILEID(char* text) {
    u32 hash = 0;
    i32 sum = 0;
    for (i32 i = strlen(text) - 1; i >= 0; --i) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            text[i] &= ~('a' - 'A');
        }
        u32 shiftedHash = hash << HASH_LEFT_SHIFT;
        hash >>= HASH_RIGHT_SHIFT;
        hash += shiftedHash;
        sum += text[i];
        hash += text[i] + sum;
    }
    return hash;
}

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
    return INDEX_NOT_FOUND;
}

#include <BASE/MiscGraphicsConstants.h>

VA(0x004c45e0, 0xea)
void FadeIn(i32 increment) {
    palette* fadePalette = new palette;
    if (fadePalette == NULL)
        MemError();
    i32 done = 0;
    if (gConfig.gfx[giCurExe].fullScreen == 0)
        increment *= WINDOWED_FADE_INCREMENT_SCALE;
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
        i32 delayUntil = KBTickCount() + FADE_FRAME_DELAY;
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
    if (fadePalette == NULL)
        MemError();
    i32 done = 0;
    if (gConfig.gfx[giCurExe].fullScreen == 0)
        increment *= WINDOWED_FADE_INCREMENT_SCALE;
    memcpy(fadePalette->m_data, gpBufferPalette->m_data, MISC_PALETTE_BYTE_COUNT);
    i32 level = 0;
    for (;;) {
        if (level >= FADE_LEVEL_COUNT) {
            if (done) {
                delete fadePalette;
                return;
            }
            level = FADE_LEVEL_LAST;
        }
        i32 delayUntil = KBTickCount() + FADE_FRAME_DELAY;
        PollSound();
        if (level == FADE_LEVEL_LAST)
            done = 1;
        for (i32 i = 0; i < PALETTE_DATA_SIZE; ++i) {
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
        if (MessageBoxA(hwndApp, gText, gMiscText.memory.assertTitle.text, MB_YESNO | MB_ICONHAND)
            != IDNO) {
            ShutDown(NULL);
        }
    }
}

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
    return NULL;
}

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
    return NULL;
}

VA(0x004c4900, 0x2b)
char* FindLastToken(char* text, char token) {
    for (i32 i = strlen(text) - 1; i >= 0; --i) {
        if (text[i] == token) {
            return text + i;
        }
    }
    return NULL;
}

VA(0x004c4930, 0x6c)
void SetInstallDefaults(void) {
    memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
    strcpy(gConfig.autoLoadName, gMiscText.installDefaults.autoLoadName.text);
    strcpy(gConfig.autoSaveName, gMiscText.installDefaults.autoSaveName.text);
    gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
}
// Retail saves ESI/EDI before materializing the main-game graphics slot.
VA(0x004c49a0, 0x1b5)
void SetGameDefaults(void) {
    i32* fullScreen = &gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].fullScreen;
    gConfig.musicVolume = 1;
    gConfig.soundVolume = 1;
    gConfig.autosave = 1;
    gConfig.showRoute = 1;
    do {
        fullScreen[IDX(GRAPHICS_SHOW_MENU_FROM_FULLSCREEN)] = 1;
        fullScreen[IDX(GRAPHICS_X_FROM_FULLSCREEN)] = DEFAULT_WINDOW_ORIGIN;
        fullScreen[IDX(GRAPHICS_Y_FROM_FULLSCREEN)] = DEFAULT_WINDOW_ORIGIN;
        fullScreen[IDX(GRAPHICS_COLOR_MOUSE_FROM_FULLSCREEN)] = 0;
        fullScreen[IDX(GRAPHICS_FULLSCREEN)] = 1;
        if (giMainVideoModeWidth <= DEFAULT_WINDOW_WIDTH) {
            fullScreen[IDX(GRAPHICS_WIDTH_FROM_FULLSCREEN)] = DEFAULT_SMALL_WINDOW_WIDTH;
            fullScreen[IDX(GRAPHICS_HEIGHT_FROM_FULLSCREEN)] =
                DEFAULT_SMALL_WINDOW_HEIGHT;
        } else {
            fullScreen[IDX(GRAPHICS_WIDTH_FROM_FULLSCREEN)] = DEFAULT_WINDOW_WIDTH;
            fullScreen[IDX(GRAPHICS_HEIGHT_FROM_FULLSCREEN)] = DEFAULT_WINDOW_HEIGHT;
        }
        fullScreen += CONFIG_GRAPHICS_SIZE / sizeof(*fullScreen);
    } while (fullScreen < &gConfig.showCombatGrid);
    gConfig.showCombatGrid = 0;
    gConfig.showCombatMouseHex = 0;
    gConfig.combatShadeLevel = 0;
    gConfig.combatArmyInfoLevel = 0;
    gConfig.evilInterfaceUsage = 0;
    gConfig.useOpera = IDX(CONFIG_OPERA_ENABLED);
    gConfig.quickCombatLevel = 0;
    gConfig.combatSpeed = 0;
    gConfig.autoCombatUseSpells = 0;
    gConfig.blackoutComputer = 0;
    gConfig.currentMapOffset = 0;
    gConfig.firstMapOffset = rand() % DEFAULT_MAP_OFFSET_COUNT;
    gConfig.showObjectBoxes = 0;
    gConfig.editorScreenAnimation = 0;
    gConfig.editorPaletteCycling = 0;
    gbFirstTimeThrough = true;
    gConfig.slowVideo = DEFAULT_SLOW_VIDEO;
    gConfig.computerWalkSpeed = DEFAULT_COMPUTER_WALK_SPEED;
    gConfig.walkSpeed = DEFAULT_WALK_SPEED;
    strcpy(gConfig.networkDefaultName, gMiscText.gameDefaults.unknownHeroName.text);
    *reinterpret_cast<i32*>(gConfig.uniqueSystemID) = 0;
    i32 idSeed = rand() % UNIQUE_ID_RANDOM_MODULUS + 1;
    idSeed += KBTickCount();
    gConfig.uniqueSystemID[UNIQUE_ID_TRAILING_INDEX] =
        gMiscText.gameDefaults.uniqueIdAlphabet.text[idSeed % UNIQUE_ID_ALPHANUMERIC_COUNT];
    i32 idAdd = rand() % UNIQUE_ID_RANDOM_MODULUS + 1;
    idAdd += KBTickCount();
    idSeed += idAdd;
    gConfig.uniqueSystemID[UNIQUE_ID_MIDDLE_INDEX] =
        gMiscText.gameDefaults.uniqueIdAlphabet.text[idSeed % UNIQUE_ID_ALPHANUMERIC_COUNT];
    idAdd = rand() % UNIQUE_ID_RANDOM_MODULUS + 1;
    idAdd += KBTickCount();
    idSeed += idAdd;
    gConfig.uniqueSystemID[UNIQUE_ID_LEADING_INDEX] =
        static_cast<char>(idSeed % UNIQUE_ID_ALPHA_COUNT + 'A');
    gConfig.needsDefaultInitialization = 0;
}

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
        if (f == NULL)
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

H2_ENUM_BEGIN(RegistryValueSize)
    REGISTRY_TEXT_BUFFER_SIZE = 100,
    REGISTRY_DWORD_BYTES      = 4,
    CONFIG_ZERO_BUFFER_WORDS  = 25,
    MODEM_INIT_STRING_SIZE    = 0x62,
    UNIQUE_SYSTEM_ID_SIZE     = 4,
    NETWORK_DEFAULT_NAME_SIZE = 0x1e
H2_ENUM_END(RegistryValueSize)

VA(0x004c4ca0, 0x7ab)
void ReadPrefsFromRegistry(void) {
    HKEY hKey;
    DWORD dwType;
    DWORD dwSize;
    char szKey[REGISTRY_TEXT_BUFFER_SIZE];
    char szScratch[REGISTRY_TEXT_BUFFER_SIZE];

    strcpy(szScratch, gMiscText.readRegistry.scratchDefault.text);
    strcpy(szKey, gMiscText.readRegistry.key.text);
    hKey = NULL;
    if (RegCreateKeyA(HKEY_LOCAL_MACHINE, szKey, &hKey) != 0)
        return;
    dwSize = REGISTRY_DWORD_BYTES;
    if (RegQueryValueExA(
            hKey,
            gMiscText.readRegistry.musicVolumeProbe.text,
            NULL,
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
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.musicVolume),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.soundVolume.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.soundVolume),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.walkSpeed.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.walkSpeed),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.computerWalkSpeed.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.computerWalkSpeed),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.showRoute.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.showRoute),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.blackoutComputer.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.blackoutComputer),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.soundQuality.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.musicSource),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.useOpera.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.useOpera),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.directComPort.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.comPort[IDX(CONFIG_CONNECTION_DIRECT)]),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.directBaudRate.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.baudRate[IDX(CONFIG_CONNECTION_DIRECT)]),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.modemComPort.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.comPort[IDX(CONFIG_CONNECTION_MODEM)]),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.modemBaudRate.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.baudRate[IDX(CONFIG_CONNECTION_MODEM)]),
        &dwSize
    );
    dwSize = MODEM_INIT_STRING_SIZE + 1;
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.modemInitString.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(gConfig.modemInitString),
        &dwSize
    );
    dwSize = REGISTRY_DWORD_BYTES;
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.uniqueSystemId.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(gConfig.uniqueSystemID),
        &dwSize
    );
    gConfig.uniqueSystemID[UNIQUE_ID_TERMINATOR_INDEX] = 0;
    dwSize = NETWORK_DEFAULT_NAME_SIZE + 1;
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.networkDefaultName.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(gConfig.networkDefaultName),
        &dwSize
    );
    dwSize = REGISTRY_DWORD_BYTES;
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.autosave.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.autosave),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.slowVideo.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.slowVideo),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.showCombatGrid.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.showCombatGrid),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.showCombatMouseHex.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.showCombatMouseHex),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.combatShadeLevel.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.combatShadeLevel),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.combatArmyInfoLevel.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.combatArmyInfoLevel),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.evilInterfaceUsage.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.evilInterfaceUsage),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.quickCombatLevel.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.quickCombatLevel),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.combatSpeed.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.combatSpeed),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.autoCombatUseSpells.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.autoCombatUseSpells),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.firstMapOffset.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.firstMapOffset),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.currentMapOffset.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.currentMapOffset),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.showObjectBoxes.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.showObjectBoxes),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorScreenAnimation.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.editorScreenAnimation),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorPaletteCycling.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.editorPaletteCycling),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameShowMenu.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].showMenu),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameX.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].x),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameY.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].y),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameWidth.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].width),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameHeight.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].height),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameFullScreen.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].fullScreen),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.mainGameColorMouseCursor.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].colorMouseCursor),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorShowMenu.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].showMenu),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorX.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].x),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorY.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].y),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorWidth.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].width),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorHeight.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].height),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorFullScreen.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].fullScreen),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        gMiscText.readRegistry.editorColorMouseCursor.text,
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].colorMouseCursor),
        &dwSize
    );
    dwSize = MODEM_INIT_STRING_SIZE + 1;
    if (RegQueryValueExA(
            hKey,
            gMiscText.readRegistry.appPath.text,
            NULL,
            &dwType,
            reinterpret_cast<u8*>(gcRegAppPath),
            &dwSize
        )
        != 0)
        strcpy(gcRegAppPath, gMiscText.readRegistry.appPathDefault.text);
    if (RegQueryValueExA(
            hKey,
            gMiscText.readRegistry.cdDrive.text,
            NULL,
            &dwType,
            reinterpret_cast<u8*>(gcRegCDRomPath),
            &dwSize
        )
        != 0)
        strcpy(gcRegCDRomPath, gMiscText.readRegistry.cdDriveDefault.text);
    RegCloseKey(hKey);
    // Clamp the saved window geometry to sane defaults / on-screen bounds.
    if (gConfig.gfx[giCurExe].width <= 0)
        gConfig.gfx[giCurExe].width = MINIMUM_WINDOW_WIDTH;
    if (gConfig.gfx[giCurExe].height <= 0)
        gConfig.gfx[giCurExe].height = MINIMUM_WINDOW_HEIGHT;
    if (gConfig.gfx[giCurExe].x < 0)
        gConfig.gfx[giCurExe].x = 0;
    if (gConfig.gfx[giCurExe].x > giMainVideoModeHeight - WINDOW_POSITION_MARGIN)
        gConfig.gfx[giCurExe].x = giMainVideoModeHeight - WINDOW_POSITION_MARGIN;
    if (gConfig.gfx[giCurExe].y < 0)
        gConfig.gfx[giCurExe].y = 0;
    if (gConfig.gfx[giCurExe].y > giMainVideoModeWidth - WINDOW_POSITION_MARGIN)
        gConfig.gfx[giCurExe].y = giMainVideoModeWidth - WINDOW_POSITION_MARGIN;
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

VA(0x004c5500, 0x6a)
void WritePrefsToFile(void) {
    i32 zeroBuffer[CONFIG_ZERO_BUFFER_WORDS];
    i32 i;
    i32* p = zeroBuffer;
    for (i = CONFIG_ZERO_BUFFER_WORDS; i != 0; i--) {
        *p = 0;
        p++;
    }
    sprintf(gText, gMiscText.writeFile.stringFormat.text, gMiscText.writeFile.configFilename.text);
    i32 fd = _open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fd != -1) {
        _write(fd, &gConfig, CONFIG_PERSISTED_SIZE);
        _close(fd);
    }
}

VA(0x004c5570, 0x491)
void WritePrefsToRegistry(void) {
    HKEY hKey;
    char szKey[REGISTRY_TEXT_BUFFER_SIZE];
    char szScratch[REGISTRY_TEXT_BUFFER_SIZE];

    strcpy(szScratch, gMiscText.writeRegistry.scratchDefault.text);
    strcpy(szKey, gMiscText.writeRegistry.key.text);
    hKey = NULL;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, szKey, 0, KEY_ALL_ACCESS, &hKey) != 0)
        return;
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.musicVolume.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.musicVolume),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.soundVolume.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.soundVolume),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.walkSpeed.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.walkSpeed),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.computerWalkSpeed.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.computerWalkSpeed),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.showRoute.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.showRoute),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.blackoutComputer.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.blackoutComputer),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.soundQuality.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.musicSource),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.useOpera.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.useOpera),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.directComPort.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.comPort[IDX(CONFIG_CONNECTION_DIRECT)]),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.directBaudRate.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.baudRate[IDX(CONFIG_CONNECTION_DIRECT)]),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.modemComPort.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.comPort[IDX(CONFIG_CONNECTION_MODEM)]),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.modemBaudRate.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.baudRate[IDX(CONFIG_CONNECTION_MODEM)]),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.modemInitString.text,
        0,
        REG_SZ,
        reinterpret_cast<u8*>(gConfig.modemInitString),
        MODEM_INIT_STRING_SIZE
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.uniqueSystemId.text,
        0,
        REG_SZ,
        reinterpret_cast<u8*>(gConfig.uniqueSystemID),
        UNIQUE_SYSTEM_ID_SIZE
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.networkDefaultName.text,
        0,
        REG_SZ,
        reinterpret_cast<u8*>(gConfig.networkDefaultName),
        NETWORK_DEFAULT_NAME_SIZE
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.autosave.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.autosave),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.slowVideo.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.slowVideo),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.showCombatGrid.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.showCombatGrid),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.showCombatMouseHex.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.showCombatMouseHex),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.combatShadeLevel.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.combatShadeLevel),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.combatArmyInfoLevel.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.combatArmyInfoLevel),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.evilInterfaceUsage.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.evilInterfaceUsage),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.quickCombatLevel.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.quickCombatLevel),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.combatSpeed.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.combatSpeed),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.autoCombatUseSpells.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.autoCombatUseSpells),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.firstMapOffset.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.firstMapOffset),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.currentMapOffset.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.currentMapOffset),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.showObjectBoxes.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.showObjectBoxes),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorScreenAnimation.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.editorScreenAnimation),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorPaletteCycling.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.editorPaletteCycling),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameShowMenu.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].showMenu),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameX.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].x),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameY.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].y),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameWidth.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].width),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameHeight.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].height),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameFullScreen.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].fullScreen),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.mainGameColorMouseCursor.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].colorMouseCursor),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorShowMenu.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].showMenu),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorX.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].x),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorY.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].y),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorWidth.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].width),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorHeight.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].height),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorFullScreen.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].fullScreen),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        gMiscText.writeRegistry.editorColorMouseCursor.text,
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].colorMouseCursor),
        REGISTRY_DWORD_BYTES
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

VA(0x004c5a60, 0x3ed)
i32 SetupCDDrive(void) {
    char registryPath[CD_PATH_BUFFER_SIZE];
    char registryKey[CD_PATH_BUFFER_SIZE];
    char cdDrives[CD_DRIVE_SLOT_COUNT];
    char count;
    i32 attempts;
    HKEY key;

    sprintf(gText, gMiscText.cd.dataArchive.text);
    i32 file = _open(gText, _O_BINARY);
    if (file == -1) {
        if (_chdir(gcRegAppPath) == -1)
            return IDX(CD_APP_PATH_UNAVAILABLE);
        file = _open(gText, _O_BINARY);
        if (file == -1)
            return IDX(CD_DATA_ARCHIVE_UNAVAILABLE);
    }
    _close(file);

    u32l logicalDrives = GetLogicalDrives();
    i32 cdDriveCount = 0;
    memset(cdDrives, 0, sizeof(cdDrives));
    for (i32 drive = CD_FIRST_DRIVE_INDEX; drive < CD_DRIVE_SLOT_COUNT; ++drive) {
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
            sprintf(gText + CD_PATH_PREFIX_BYTES, gMiscText.cd.stringFormat.text, gcAnimPath);
            strcpy(gcAnimPath, gText);
            return IDX(CD_DRIVE_READY);
        }
    }

    attempts = 0;
    {
        char resultBuffer[CD_MCI_BUFFER_SIZE];
        char command[CD_MCI_BUFFER_SIZE];
        for (;;) {
            for (i32 index = 0; index < count; ++index) {
                wsprintfA(command, gMiscText.cd.openAudioCommand.text, cdDrives[index] + 'A');
                if (mciSendStringA(command, resultBuffer, CD_MCI_RESULT_LENGTH, NULL) == 0) {
                    wsprintfA(command, gMiscText.cd.audioInfoCommand.text);
                    mciSendStringA(command, resultBuffer, CD_MCI_RESULT_LENGTH, NULL);
                    wsprintfA(command, gMiscText.cd.closeAudioCommand.text);
                    mciSendStringA(command, resultBuffer, CD_MCI_RESULT_LENGTH, NULL);
                }
                sprintf(gText, gMiscText.cd.driveAnimationPath.text, cdDrives[index] + 'A');
                file = _open(gText, _O_BINARY);
                if (file != -1) {
                    if (_lseek(file, 0, SEEK_END) != -1
                        && _lseek(file, -CD_PROBE_TRAILER_SIZE, SEEK_CUR) != -1)
                        _read(file, resultBuffer, CD_PROBE_TRAILER_SIZE);
                    _close(file);

                    strcpy(registryKey, gMiscText.cd.registryKey.text);
                    key = NULL;
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
                    return IDX(CD_DRIVE_READY);
                }
            }
            Sleep(CD_RETRY_DELAY_MILLISECONDS);
            ++attempts;
            if (attempts >= CD_RETRY_LIMIT)
                return IDX(CD_DRIVE_NOT_FOUND);
        }
    }
}

VA(0x004c5e50, 0x18)
void BitmapToScreen(class bitmap* bmp) {
    BlitBitmapToScreen(bmp, 0, 0, bmp->m_width, bmp->m_height, 0, 0);
}

VA(0x004c5e70, 0x3d)
void SetPalette(i8* paletteData, i32 updateDisplay) {
    memcpy(gpBufferPalette->m_data, paletteData, MISC_PALETTE_BYTE_COUNT);
    memcpy(
        gCyclePal,
        paletteData + IDX(CYCLE_RANGE_ONE_FIRST) * PALETTE_COMPONENT_COUNT,
        sizeof(gCyclePal)
    );
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
        blitSourceX = giScrollX + BLIT_SCROLL_OFFSET;
        width = BLIT_SCROLL_EXTENT;
        sourceY = giScrollY + BLIT_SCROLL_OFFSET;
        height = BLIT_SCROLL_EXTENT;
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
VA(0x004c6070, 0xa6)
void LogTruncate(void) {
    char logText[TEXT_BUFFER_SIZE];
    if (giDebugLevel >= FILE_DEBUG_LEVEL) {
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


VA(0x004c6120, 0x9e)
void LogStr(char* text) {
    char logText[TEXT_BUFFER_SIZE];
    if (giDebugLevel >= FILE_DEBUG_LEVEL) {
        FILE* logFile = fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
        if (logFile != NULL) {
            strcpy(logText, text);
            *reinterpret_cast<u16*>(logText + strlen(logText)) =
                *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
            fputs(logText, logFile);
            fclose(logFile);
            if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
                OutputDebugStringA(logText);
        }
    }
}

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
    char text[FORMAT_BUFFER_SIZE];
    char logText[TEXT_BUFFER_SIZE];
    if (value7 != UNUSED_VALUE)
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
    else if (value6 != UNUSED_VALUE)
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
    else if (value5 != UNUSED_VALUE)
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
    else if (value4 != UNUSED_VALUE)
        sprintf(text, gMiscText.log.fourValueFormat.text, label, value1, value2, value3, value4);
    else if (value3 != UNUSED_VALUE)
        sprintf(text, gMiscText.log.threeValueFormat.text, label, value1, value2, value3);
    else if (value2 != UNUSED_VALUE)
        sprintf(text, gMiscText.log.twoValueFormat.text, label, value1, value2);
    else
        sprintf(text, gMiscText.log.oneValueFormat.text, label, value1);
    if (giDebugLevel >= FILE_DEBUG_LEVEL) {
        FILE* file = fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
        if (file != NULL) {
            strcpy(logText, text);
            *reinterpret_cast<u16*>(logText + strlen(logText)) =
                *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
            fputs(logText, file);
            fclose(file);
            if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
                OutputDebugStringA(logText);
        }
    }
}

VA(0x004c63f0, 0x6c)
void AiPrint(char* text) {
    if (giDebugLevel >= FILE_DEBUG_LEVEL) {
        FillBitmapArea(
            gpWindowManager->m_screen, 0, STATUS_BAR_Y, STATUS_BAR_WIDTH, STATUS_BAR_HEIGHT, 0
        );
        smallFont->DrawBoundedString(
            text,
            0,
            STATUS_TEXT_Y,
            STATUS_BAR_WIDTH,
            STATUS_TEXT_HEIGHT,
            FONT_DRAW_DEFAULT,
            FONT_ALIGN_LEFT
        );
        BlitBitmapToScreen(
            gpWindowManager->m_screen,
            0,
            STATUS_BAR_Y,
            STATUS_BAR_WIDTH,
            STATUS_BAR_HEIGHT,
            0,
            STATUS_BAR_Y
        );
    }
}

VA(0x004c6460, 0x7a)
void AbsAiPrint(char* text) {
    i32 saved = giDebugLevel;
    giDebugLevel = FORCED_DEBUG_LEVEL;
    FillBitmapArea(
        gpWindowManager->m_screen, 0, STATUS_BAR_Y, STATUS_BAR_WIDTH, STATUS_BAR_HEIGHT, 0
    );
    smallFont->DrawBoundedString(
        text,
        0,
        STATUS_TEXT_Y,
        STATUS_BAR_WIDTH,
        STATUS_TEXT_HEIGHT,
        FONT_DRAW_DEFAULT,
        FONT_ALIGN_LEFT
    );
    BlitBitmapToScreen(
        gpWindowManager->m_screen,
        0,
        STATUS_BAR_Y,
        STATUS_BAR_WIDTH,
        STATUS_BAR_HEIGHT,
        0,
        STATUS_BAR_Y
    );
    giDebugLevel = saved;
}

VA(0x004c64e0, 0xf8)
void FadeTo(u8* source, u8* destination, i32 increment) {
    u8 colors[MISC_PALETTE_BYTE_COUNT];
    memcpy(colors, source, sizeof(colors));
    increment >>= FADE_TO_INCREMENT_SHIFT;
    if (increment < 1)
        increment = 1;
    i32 level = FADE_TO_START_LEVEL;
    do {
        i32 delayUntil = KBTickCount() + FADE_TO_FRAME_DELAY;
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
        output[PALETTE_RED_OUTPUT_OFFSET] = sourceColor[PALETTE_RED_INDEX];
        output[PALETTE_GREEN_OUTPUT_OFFSET] = sourceColor[PALETTE_GREEN_INDEX];
        output[PALETTE_BLUE_OUTPUT_OFFSET] = sourceColor[PALETTE_BLUE_INDEX];
    } while (output < translatedPalette + sizeof(translatedPalette));
    i32 rows = BLIT_SCREEN_HEIGHT;
    FadeTo(reinterpret_cast<u8*>(paletteData), translatedPalette, increment);
    i32 columns;
    u8* pixel = gpWindowManager->m_screen->m_pixels;
    do {
        columns = BLIT_SCREEN_WIDTH;
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
    if ((color >= IDX(CYCLE_RANGE_ONE_FIRST) && color <= IDX(CYCLE_RANGE_ONE_LAST))
        || (color >= IDX(CYCLE_RANGE_TWO_FIRST) && color <= IDX(CYCLE_RANGE_TWO_LAST))) {
        return 1;
    }
    return 0;
}

VA(0x004c66d0, 0x1ee)
void CreatePCXFile(char* filename, u8* pixels, i32 width, i32 height, u8* paletteData) {
    PCXHeader header;
    memset(&header, 0, sizeof(header));
    header.manufacturer = MANUFACTURER_ZSOFT;
    header.version = VERSION_3_0;
    header.encoding = ENCODING_RLE;
    header.bitsPerPixel = BITS_PER_PIXEL;
    header.xMax = static_cast<u16>(width - 1);
    header.yMax = static_cast<u16>(height - 1);
    header.planes = PLANE_COUNT;
    header.bytesPerLine = static_cast<u16>(width);
    header.paletteType = PALETTE_TYPE_COLOR;
    i32 fileHandle = _open(filename, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fileHandle == -1)
        return;
    _write(fileHandle, &header, sizeof(header));
    u8* encodedRow =
        static_cast<u8*>(H2_ALLOC_AT(width * 2, gMiscText.pcx.encodedRowAllocation.text, 1480));
    for (i32 row = 0; row < height; ++row) {
        i32 sourceIndex = 0;
        u32 encodedSize = 0;
        while (sourceIndex < width) {
            u8 value = pixels[sourceIndex];
            i32 runEnd = sourceIndex;
            while (runEnd < width && pixels[runEnd] == value
                   && runEnd - sourceIndex + 1 < RLE_RUN_LIMIT)
                ++runEnd;
            i32 runLength = runEnd - sourceIndex;
            if (runLength <= 1 && (value & RLE_RUN_MARKER) != RLE_RUN_MARKER) {
                encodedRow[encodedSize++] = value;
                ++sourceIndex;
            } else {
                encodedRow[encodedSize++] = static_cast<u8>(runLength | RLE_RUN_MARKER);
                encodedRow[encodedSize++] = value;
                sourceIndex += runLength;
            }
        }
        _write(fileHandle, encodedRow, encodedSize);
        pixels += width;
    }
    H2_FREE_AT(encodedRow, gMiscText.pcx.encodedRowDestruction.text, 0x5f0);
    u8 paletteMarker = VGA_PALETTE_MARKER;
    _write(fileHandle, &paletteMarker, 1);
    u8* outputPalette = static_cast<u8*>(
        H2_ALLOC_AT(PALETTE_BYTE_COUNT, gMiscText.pcx.outputPaletteAllocation.text, 1526)
    );
    for (i32 i = 0; i < PALETTE_BYTE_COUNT; ++i)
        outputPalette[i] = paletteData[i] << COMPONENT_SCALE_SHIFT;
    _write(fileHandle, outputPalette, PALETTE_BYTE_COUNT);
    H2_FREE_AT(outputPalette, gMiscText.pcx.outputPaletteDestruction.text, 0x5fb);
    _close(fileHandle);
}

VA(0x004c68c0, 0x52)
i32l FileSize(char* filename) {
    FILE* file = fopen(filename, gMiscText.file.readWriteBinaryMode.text);
    if (file == NULL) {
        FileError(filename);
    }
    fseek(file, 0, SEEK_END);
    i32l size = ftell(file);
    fseek(file, 0, SEEK_SET);
    fclose(file);
    return size;
}

VA(0x004c6920, 0xc)
struct IconEntry* GetIconEntry(class icon* iconPtr, i32 index) {
    return &iconPtr->Entries()[index];
}
VA(0x004c6930, 0xb8)
i32 SRandom(i32 low, i32 high) {
    if (high == low) {
        return high;
    }
    if (high < low) {
        return low;
    }

    i32 highTerm = (high * RANDOM_TERM_MULTIPLIER) & RANDOM_TERM_MASK;
    i32 lowTerm = (low * RANDOM_TERM_MULTIPLIER) & RANDOM_TERM_MASK;
    iLastSeed += highTerm << RANDOM_HIGH_TERM_SHIFT;
    iLastSeed += lowTerm * RANDOM_LOW_TERM_MULTIPLIER;
    iLastSeed += highTerm;
    iLastSeed += (iLastSeed & RANDOM_FEEDBACK_MASK) << RANDOM_FEEDBACK_SHIFT;
    iLastSeed &= RANDOM_SEED_MASK;

    i32 result = 0;
    i32 mix = iLastSeed * RANDOM_MIX_MULTIPLIER;
    mix += (mix & RANDOM_MIX_MASK) >> RANDOM_MIX_SHIFT;
    for (i32 i = RANDOM_TOP_BIT; i >= 0; --i) {
        if (mix & (1 << i)) {
            result |= 1 << i;
        }
    }
    mix += low;
    i32 range = high - low;
    mix += high * RANDOM_HIGH_MIX_MULTIPLIER;
    i32 rangedResult = low + result % (range + 1);
    iLastSeed = mix;
    return rangedResult;
}

VA(0x004c69f0, 0x5c)
void SIncRandomize(i32 x, i32 y) {
    x *= RANDOM_TERM_MULTIPLIER;
    x &= RANDOM_TERM_MASK;
    y *= RANDOM_TERM_MULTIPLIER;
    y &= RANDOM_TERM_MASK;
    iLastSeed += y << RANDOM_HIGH_TERM_SHIFT;
    iLastSeed += x * RANDOM_LOW_TERM_MULTIPLIER;
    iLastSeed += y;
    iLastSeed += (iLastSeed & RANDOM_FEEDBACK_MASK) << RANDOM_FEEDBACK_SHIFT;
}

VA(0x004c6a50, 0x10)
void SRand(i32 seed) {
    iLastSeed = seed;
    srand(seed);
}

VA(0x004c6a60, 0x48)
i32 SGenRand(void) {
    i32 result = 0;
    iLastSeed &= RANDOM_SEED_MASK;
    i32 mix = iLastSeed * RANDOM_MIX_MULTIPLIER;
    mix += (mix & RANDOM_MIX_MASK) >> RANDOM_MIX_SHIFT;
    i32 i = RANDOM_TOP_BIT;
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
    return REPORTED_MEMORY_KILOBYTES;
}
VA(0x004c6ac0, 0x386)
void GetDataEntry(
    char* prompt,
    char* destination,
    i32 maximumLength,
    char* initialText,
    i32 showCancel,
    i32 useImmediateHandler
) {
    MouseCursorType savedCursorType = gpMouseManager->m_cursorType;
    i32 savedCursorFrame = gpMouseManager->m_cursorFrame;
    while (gpMouseManager->m_hideCount != 0)
        gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer(gMiscText.dataEntry.mouseFilename.text, 0, MOUSE_AUTO_CURSOR_TYPE);

    cDEDest = destination;
    iDEMaxLen = maximumLength;
    strcpy(destination, gMiscText.dataEntry.destinationDefault.text);

    i32 rows = bigFont->LineLength(prompt, PROMPT_WIDTH) * PROMPT_LINE_HEIGHT;
    if (showCancel != 0)
        rows += CANCEL_PROMPT_HEIGHT;
    rows = (rows + ROW_ROUNDING_BIAS) / ROW_HEIGHT;
    if (rows > MAX_ROW_COUNT)
        rows = MAX_ROW_COUNT;
    i32 entryY = rows * ROW_HEIGHT - (showCancel != 0 ? CANCEL_Y_OFFSET : 0) + ENTRY_BASE_Y;

    char windowName[WINDOW_NAME_CAPACITY];
    sprintf(windowName, gMiscText.dataEntry.windowFilenameFormat.text, rows);
    DataEntryWin = new heroWindow(WINDOW_X, WINDOW_Y, windowName);
    if (DataEntryWin == NULL)
        MemError();

    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = ENTRY_PROMPT_WIDGET;
    message.payload.widget.data.text = prompt;
    DataEntryWin->BroadcastMessage(message);

    char entryText[TEXT_BUFFER_CAPACITY];
    if (initialText == NULL)
        initialText = gMiscText.dataEntry.initialTextDefault.text;
    strcpy(entryText, initialText);
    message.payload.widget.id = ENTRY_TEXT_WIDGET;
    message.payload.widget.data.text = entryText;
    DataEntryWin->BroadcastMessage(message);
    strcpy(destination, entryText);

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.id = ENTRY_BUTTON_ONE;
    message.payload.widget.data.value = WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = ENTRY_BUTTON_SEVEN;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = ENTRY_BUTTON_EIGHT;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = ENTRY_BUTTON_FIVE;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = ENTRY_BUTTON_SIX;
    DataEntryWin->BroadcastMessage(message);
    if (showCancel == 0) {
        message.payload.widget.id = ENTRY_CANCEL_BUTTON;
        DataEntryWin->BroadcastMessage(message);
    }

    textEntryWidget* entry = new textEntryWidget(
        TEXT_FIELD_X,
        static_cast<i16>(entryY),
        TEXT_FIELD_WIDTH,
        TEXT_FIELD_HEIGHT,
        static_cast<i16>(maximumLength),
        destination,
        gMiscText.dataEntry.fontFilename.text,
        TEXT_FIELD_COLOR,
        gMiscText.dataEntry.iconFilename.text,
        TEXT_FIELD_ICON_FRAME,
        ENTRY_TEXT_WIDGET,
        TEXT_FIELD_KIND,
        TEXT_FIELD_LAYOUT,
        TEXT_FIELD_HORIZONTAL_INSET,
        TEXT_FIELD_VERTICAL_INSET
    );
    if (entry == NULL)
        MemError();
    inBoxY = entryY + INPUT_BOX_Y_OFFSET;
    inBoxX = INPUT_BOX_X;
    DataEntryWin->AddWidget(entry, WIDGET_Z_ORDER);

    if (useImmediateHandler != 0) {
        bDataEntryTime = IDX(ENTRY_PHASE_IMMEDIATE);
        gbAllowTextEntryEscape = false;
    } else
        bDataEntryTime = IDX(ENTRY_PHASE_READY);
    gpWindowManager->DoDialog(DataEntryWin, DataEntryWindowHandler, 0);
    delete DataEntryWin;
    gpMouseManager->SetPointer(
        gMiscText.dataEntry.restoredMouseFilename.text,
        savedCursorFrame,
        savedCursorType
    );
    gbAllowTextEntryEscape = true;
}

VA(0x004c6e50, 0x173)
i32 DataEntryWindowHandler(struct tag_message& message) {
    if (bDataEntryTime == IDX(ENTRY_PHASE_IMMEDIATE)) {
        ++bDataEntryTime;
        message.type = MESSAGE_LEFT_BUTTON_DOWN;
        message.payload.mouse.x = inBoxX;
        message.payload.mouse.y = inBoxY;
        DataEntryWin->BroadcastMessage(message);
        return EVENT_WINDOW_CONTINUE;
    }

    if (bDataEntryTime == IDX(ENTRY_PHASE_POINTER_SENT))
        ++bDataEntryTime;
    else {
        if (message.type != MESSAGE_WIDGET)
            goto normalEvent;
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
                if (message.payload.widget.id != ENTRY_TEXT_WIDGET)
                    goto normalEvent;
                break;
            case WIDGET_COMMAND_DESELECT:
                goto possibleCancelEvent;
            default:
                goto normalEvent;
        }
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.id = ENTRY_TEXT_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
    DataEntryWin->BroadcastMessage(message);
    if (strlen(message.payload.widget.data.text) != 0) {
        memset(cDEDest, 0, iDEMaxLen);
        strncpy(cDEDest, message.payload.widget.data.text, iDEMaxLen - 1);
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message.payload.widget.id = ENTRY_TEXT_WIDGET;
        message.payload.widget.data.text = cDEDest;
        DataEntryWin->BroadcastMessage(message);
        DataEntryWin->DrawWindow(DRAW_MODE, REDRAW_OFFSET, REDRAW_OFFSET);
        if (gbTextEntryEscaped == 0) {
            gpWindowManager->m_dialogResult = message.payload.widget.id;
            message.payload.widget.id = ENTRY_TEXT_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
            return EVENT_WINDOW_CLOSE;
        }
    }

possibleCancelEvent:
    if (message.payload.widget.id != ENTRY_CANCEL_BUTTON)
        goto normalEvent;
    message.payload.widget.id = ENTRY_TEXT_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
    return EVENT_WINDOW_CLOSE;

normalEvent:
    return EventWindowHandler(message);
}

DATA(0x005331c4) class heroWindow* DataEntryWin;
DATA(0x005331c8) char* cDEDest;
DATA(0x005331d4) i32 iDEMaxLen;
DATA(0x005331d8) i32 bDataEntryTime;
DATA(0x005331dc) i32 inBoxX;
DATA(0x005331e0) i32 inBoxY;
