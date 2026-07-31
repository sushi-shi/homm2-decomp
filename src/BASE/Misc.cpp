#define HOMM2_MISC_INLINE_ICONENTRY
#include <Ints.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <BASE/heroWindow.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/icon.h>
#include <BASE/bmap2.h>
#include <BASE/font.h>
#include <BASE/textEntryWidget.h>
#include <BASE/Misc.h>
#include <BASE/MiscEnums.h>
#include <BASE/MISC_TYPES.h>
typedef enum DataEntryLayout {
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
    TEXT_FIELD_ICON_FRAME       = 3,
    TEXT_FIELD_HORIZONTAL_INSET = 10,
    TEXT_FIELD_VERTICAL_INSET   = 3,
    INPUT_BOX_X                 = 213,
    REDRAW_OFFSET               = 10,
    DRAW_MODE                   = 1,
    WIDGET_Z_ORDER              = -1
} DataEntryLayout;

typedef enum DataEntryWidgetId {
    ENTRY_PROMPT_WIDGET = 1,
    ENTRY_TEXT_WIDGET   = 10,
    ENTRY_BUTTON_ONE    = 0x7801,
    ENTRY_CANCEL_BUTTON = 0x7802,
    ENTRY_BUTTON_FIVE   = 0x7805,
    ENTRY_BUTTON_SIX    = 0x7806,
    ENTRY_BUTTON_SEVEN  = 0x7807,
    ENTRY_BUTTON_EIGHT  = 0x7808
} DataEntryWidgetId;

typedef enum MiscLogPrivateConstant {
    MEMORY_LEAK_DEBUG_LEVEL   = 1,
    FILE_DEBUG_LEVEL          = 2,
    DEBUGGER_OUTPUT_LEVEL     = 4,
    FORCED_DEBUG_LEVEL        = 9,
    FORMAT_BUFFER_SIZE        = 200,
    TEXT_BUFFER_SIZE          = 500,
    MEMORY_ENTRY_CAPACITY     = 2000,
    REPORTED_MEMORY_KILOBYTES = 16034,
    ENTRY_SEARCH_COMPLETE     = 99999
} MiscLogPrivateConstant;

typedef enum MiscGameDefaultConstant {
    DEFAULT_WINDOW_ORIGIN        = 10,
    DEFAULT_SMALL_WINDOW_WIDTH   = 0x1e0,
    DEFAULT_SMALL_WINDOW_HEIGHT  = 0x168,
    DEFAULT_WINDOW_WIDTH         = 0x280,
    DEFAULT_WINDOW_HEIGHT        = 0x1e0,
    DEFAULT_SLOW_VIDEO           = 3,
    DEFAULT_MAP_OFFSET_COUNT     = 32001,
    UNIQUE_ID_RANDOM_MODULUS     = 999999,
    UNIQUE_ID_ALPHANUMERIC_COUNT = 36,
    UNIQUE_ID_ALPHA_COUNT        = 26,
    UNIQUE_ID_LEADING_INDEX      = 0,
    UNIQUE_ID_MIDDLE_INDEX       = 1,
    UNIQUE_ID_TRAILING_INDEX     = 2,
    UNIQUE_ID_TERMINATOR_INDEX   = 3
} MiscGameDefaultConstant;

enum class MiscGraphicsFieldIndex : i32 {
    GRAPHICS_SHOW_MENU_FROM_FULLSCREEN   = -5,
    GRAPHICS_X_FROM_FULLSCREEN           = -4,
    GRAPHICS_Y_FROM_FULLSCREEN           = -3,
    GRAPHICS_WIDTH_FROM_FULLSCREEN       = -2,
    GRAPHICS_HEIGHT_FROM_FULLSCREEN      = -1,
    GRAPHICS_FULLSCREEN                  = 0,
    GRAPHICS_COLOR_MOUSE_FROM_FULLSCREEN = 1
};
using enum MiscGraphicsFieldIndex;

typedef enum PCXConstant {
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
} PCXConstant;

typedef enum MiscCycleColorRange {
    CYCLE_RANGE_ONE_FIRST = 0xd6,
    CYCLE_RANGE_ONE_LAST  = 0xdd,
    CYCLE_RANGE_TWO_FIRST = 0xe7,
    CYCLE_RANGE_TWO_LAST  = 0xed
} MiscCycleColorRange;

typedef enum MiscFadeConstant {
    FADE_LEVEL_COUNT              = 0x40,
    FADE_LEVEL_LAST               = 0x3f,
    FADE_CHANGE_THRESHOLD_COUNT   = 16,
    FADE_FRAME_DELAY              = 0x14,
    WINDOWED_FADE_INCREMENT_SCALE = 2,
    FADE_TO_INCREMENT_SHIFT       = 2,
    FADE_TO_START_LEVEL           = 0x30,
    FADE_TO_FRAME_DELAY           = 0x32
} MiscFadeConstant;

typedef enum MiscPaletteComponent {
    PALETTE_COMPONENT_COUNT     = 3,
    PALETTE_RED_INDEX           = 0,
    PALETTE_GREEN_INDEX         = 1,
    PALETTE_BLUE_INDEX          = 2,
    PALETTE_RED_OUTPUT_OFFSET   = -3,
    PALETTE_GREEN_OUTPUT_OFFSET = -2,
    PALETTE_BLUE_OUTPUT_OFFSET  = -1
} MiscPaletteComponent;

typedef enum MiscWindowConstant {
    MINIMUM_WINDOW_WIDTH   = 320,
    MINIMUM_WINDOW_HEIGHT  = 240,
    WINDOW_POSITION_MARGIN = 200
} MiscWindowConstant;

typedef enum MiscBlitConstant {
    BLIT_SCROLL_OFFSET = 0x10,
    BLIT_SCROLL_EXTENT = 0x1c0,
    BLIT_SCREEN_WIDTH  = 0x280,
    BLIT_SCREEN_HEIGHT = 0x1e0
} MiscBlitConstant;

typedef enum SeededRandomConstant {
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
} SeededRandomConstant;

typedef enum FileIdHashConstant {
    HASH_LEFT_SHIFT  = 5,
    HASH_RIGHT_SHIFT = 25,
    INDEX_NOT_FOUND  = 0xffff
} FileIdHashConstant;

#undef HOMM2_MISC_INLINE_ICONENTRY
#include <BASE/miscwin.h>
#include <SOURCE/KB.h>
#include <PLATFORM/Graphics.h>
#include <SOURCE/NOOPT.h>
#include <BASE/message.h>
#include <stdlib.h>
#include <stdio.h>
#include <PLATFORM/File.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <BASE/palette.h>
#include <SOURCE/X_GLOBAL.h>

static i32 giFindMid;

static i32 gBlitRight;
static i32 gBlitBottom;

i32 iMemEntries = 0;
MemEntry* gpMemEntry = NULL;
i32 giTotalMemAllocated = 0;
u8
    giChangeThreshold[FADE_CHANGE_THRESHOLD_COUNT] =
        {0, 1, 2, 3, 4, 6, 8, 10, 13, 16, 19, 22, 26, 31, 37, 46};
i32 iLastSeed = INITIAL_SEED;
static char gMemEntryTag[sizeof("IME")] = "IME";

static SMiscText gMiscText = {
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
    {{"HEROES2.CFG"}, {"%s"}, {"rb"}, {""}},
    {{""},
     {MISC_REGISTRY_KEY},
     {"Music Volume"},
     {"Music Volume"},
     {"Sound Volume"},
     {"Walk Speed"},
     {"Computer Walk Speed"},
     {"Show Route"},
     {"Blackout Computer"},
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
    {{".\\DATA\\HEROES2.AGG"}, {".\\HEROES2\\ANIM\\VOY24.SMK"}},
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

typedef enum StatusBarLayout {
    STATUS_BAR_WIDTH   = 640,
    STATUS_BAR_Y       = 460,
    STATUS_BAR_HEIGHT  = 20,
    STATUS_TEXT_Y      = 464,
    STATUS_TEXT_HEIGHT = 16
} StatusBarLayout;

void InitMemEntry(void) {
    LogInt(gMemEntryTag, iMemEntries, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE,
           LOG_UNUSED_VALUE, LOG_UNUSED_VALUE);
    gpMemEntry = static_cast<MemEntry*>(malloc(MEMORY_ENTRY_CAPACITY * sizeof(MemEntry)));
    for (i32 i = 0; i < MEMORY_ENTRY_CAPACITY; ++i)
        gpMemEntry[i].used = 0;
}

void* BaseAlloc(u32 size, char* originalFile, i32 originalLine) {
    char text[FORMAT_BUFFER_SIZE];
    char logText[TEXT_BUFFER_SIZE];
    if (size == 0)
        return NULL;
    if (gpMemEntry == NULL) {
        LogInt(gMemEntryTag, iMemEntries, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE,
               LOG_UNUSED_VALUE, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE);
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
                    platform::Host().Log(platform::LogLevel::Debug, logText);
            }
        }
    }
    return ptr;
}

void BaseFree(void* ptr, char* originalFile, i32 originalLine) {
    char logText[TEXT_BUFFER_SIZE];
    char text[FORMAT_BUFFER_SIZE];
    if (gpMemEntry == NULL) {
        LogInt(gMemEntryTag, iMemEntries, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE,
               LOG_UNUSED_VALUE, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE);
        gpMemEntry =
            static_cast<MemEntry*>(malloc(MEMORY_ENTRY_CAPACITY * sizeof(MemEntry)));
        for (i32 initIndex = 0; initIndex < MEMORY_ENTRY_CAPACITY; ++initIndex)
            gpMemEntry[initIndex].used = 0;
    }
    if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
        LogInt(
            gMiscText.memory.freeLabel.text,
            reinterpret_cast<i32>(ptr),
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE
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
                    platform::Host().Log(platform::LogLevel::Debug, logText);
            }
        }
        return;
    }
    --iMemEntries;
    if (iMemEntries < 0)
        LogInt(
            gMiscText.memory.entryUnderflow.text,
            iMemEntries,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE
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
                            platform::Host().Log(platform::LogLevel::Debug, logText);
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
                    platform::Host().Log(platform::LogLevel::Debug, logText);
            }
        }
    } else {
        free(ptr);
    }
}

void PrintMemoryLeaks(void) {
    char logText[TEXT_BUFFER_SIZE];
    if (giDebugLevel >= MEMORY_LEAK_DEBUG_LEVEL && gpMemEntry != NULL) {
        LogInt(
            gMiscText.memory.leakCountLabel.text,
            iMemEntries,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE
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
                            platform::Host().Log(platform::LogLevel::Debug, logText);
                    }
                }
            }
            entryIndex = entryIndex + 1;
        } while (entryIndex < MEMORY_ENTRY_CAPACITY);
    }
}

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

u32l MAKEFILEID(const char* text) {
    u32 hash = 0;
    i32 sum = 0;
    for (i32 i = strlen(text) - 1; i >= 0; --i) {
        char value = text[i];
        if (value >= 'a' && value <= 'z') {
            value &= ~('a' - 'A');
        }
        u32 shiftedHash = hash << HASH_LEFT_SHIFT;
        hash >>= HASH_RIGHT_SHIFT;
        hash += shiftedHash;
        sum += value;
        hash += value + sum;
    }
    return hash;
}

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

void FadeIn(i32 increment) {
    palette* fadePalette = new palette;
    if (fadePalette == NULL)
        MemError();
    i32 done = 0;
    if (gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen == 0)
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
        i32 delayUntil = platform::Ticks() + FADE_FRAME_DELAY;
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

void FadeOut(i32 increment) {
    palette* fadePalette = new palette;
    if (fadePalette == NULL)
        MemError();
    i32 done = 0;
    if (gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen == 0)
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
        i32 delayUntil = platform::Ticks() + FADE_FRAME_DELAY;
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

i32 Random(i32 low, i32 high) {
    if (low == high) {
        return high;
    }
    if (high < low) {
        return low;
    }
    return low + rand() % (high - low + 1);
}

void ProcessAssert(i32 condition, char* file, i32 line) {
    if (condition == 0) {
        gpMouseManager->SetColorMice(0);
        SetFullScreenStatus(0);
        sprintf(gText, gMiscText.memory.assertMessage.text, file, line);
        platform::ShowMessage(gMiscText.memory.assertTitle.text, gText);
        ShutDown(NULL);
    }
}

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

char* FindLastToken(char* text, char token) {
    for (i32 i = strlen(text) - 1; i >= 0; --i) {
        if (text[i] == token) {
            return text + i;
        }
    }
    return NULL;
}

void SetInstallDefaults(void) {
    memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
    strcpy(gConfig.autoLoadName, gMiscText.installDefaults.autoLoadName.text);
    strcpy(gConfig.autoSaveName, gMiscText.installDefaults.autoSaveName.text);
}

void SetGameDefaults(void) {
    gConfig.musicVolume = CONFIG_VOLUME_MIN;
    gConfig.soundVolume = CONFIG_VOLUME_MIN;
    gConfig.autosave = 1;
    gConfig.showRoute = 1;
    i32* fullScreen = &gConfig.gfx[H2EnumIndex(CONFIG_EXECUTABLE_GAME)].fullScreen;
    do {
        fullScreen[H2EnumIndex(GRAPHICS_SHOW_MENU_FROM_FULLSCREEN)] = 1;
        fullScreen[H2EnumIndex(GRAPHICS_X_FROM_FULLSCREEN)] = DEFAULT_WINDOW_ORIGIN;
        fullScreen[H2EnumIndex(GRAPHICS_Y_FROM_FULLSCREEN)] = DEFAULT_WINDOW_ORIGIN;
        fullScreen[H2EnumIndex(GRAPHICS_COLOR_MOUSE_FROM_FULLSCREEN)] = 0;
        fullScreen[H2EnumIndex(GRAPHICS_FULLSCREEN)] = 1;
        if (giMainVideoModeWidth <= DEFAULT_WINDOW_WIDTH) {
            fullScreen[H2EnumIndex(GRAPHICS_WIDTH_FROM_FULLSCREEN)] = DEFAULT_SMALL_WINDOW_WIDTH;
            fullScreen[H2EnumIndex(GRAPHICS_HEIGHT_FROM_FULLSCREEN)] =
                DEFAULT_SMALL_WINDOW_HEIGHT;
        } else {
            fullScreen[H2EnumIndex(GRAPHICS_WIDTH_FROM_FULLSCREEN)] = DEFAULT_WINDOW_WIDTH;
            fullScreen[H2EnumIndex(GRAPHICS_HEIGHT_FROM_FULLSCREEN)] = DEFAULT_WINDOW_HEIGHT;
        }
        fullScreen += CONFIG_GRAPHICS_SIZE / sizeof(*fullScreen);
    } while (fullScreen < &gConfig.showCombatGrid);
    gConfig.showCombatGrid = 1;
    gConfig.showCombatMouseHex = 1;
    gConfig.combatShadeLevel = 1;
    gConfig.combatArmyInfoLevel = 2;
    gConfig.evilInterfaceUsage = 0;
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
    gConfig.computerWalkSpeed = CONFIG_WALK_SPEED_FAST;
    gConfig.walkSpeed = CONFIG_WALK_SPEED_NORMAL;
    strcpy(gConfig.networkDefaultName, gMiscText.gameDefaults.unknownHeroName.text);
    *reinterpret_cast<i32*>(gConfig.uniqueSystemID) = 0;
    i32 idSeed = rand() % UNIQUE_ID_RANDOM_MODULUS + 1;
    idSeed += platform::Ticks();
    gConfig.uniqueSystemID[UNIQUE_ID_TRAILING_INDEX] =
        gMiscText.gameDefaults.uniqueIdAlphabet.text[idSeed % UNIQUE_ID_ALPHANUMERIC_COUNT];
    i32 idAdd = rand() % UNIQUE_ID_RANDOM_MODULUS + 1;
    idAdd += platform::Ticks();
    idSeed += idAdd;
    gConfig.uniqueSystemID[UNIQUE_ID_MIDDLE_INDEX] =
        gMiscText.gameDefaults.uniqueIdAlphabet.text[idSeed % UNIQUE_ID_ALPHANUMERIC_COUNT];
    idAdd = rand() % UNIQUE_ID_RANDOM_MODULUS + 1;
    idAdd += platform::Ticks();
    idSeed += idAdd;
    gConfig.uniqueSystemID[UNIQUE_ID_LEADING_INDEX] =
        static_cast<char>(idSeed % UNIQUE_ID_ALPHA_COUNT + 'A');
    gConfig.needsDefaultInitialization = 0;
}

void ReadPrefsFromFile(void) {
    snprintf(
        gText,
        GLOBAL_TEXT_BUFFER_SIZE,
        "%s/%s",
        platform::Files().UserRoot().c_str(),
        gMiscText.readFile.configFilename.text
    );
    FILE* f = platform::FileExists(gText) ? fopen(gText, gMiscText.readFile.binaryMode.text) : NULL;
    if (f == NULL) {
        memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
        strcpy(gConfig.autoLoadName, gMiscText.installDefaults.autoLoadName.text);
        strcpy(gConfig.autoSaveName, gMiscText.installDefaults.autoSaveName.text);
    } else {
        fread(&gConfig, CONFIG_PERSISTED_SIZE, 1, f);
        fclose(f);
        if (gConfig.needsDefaultInitialization == 0)
            goto skipDefaults;
    }
    SetGameDefaults();
    UpdateSystemOptionsMenu();
    WritePrefsToRegistry();
skipDefaults:
    strcpy(gcRegAppPath, gMiscText.readFile.appPathDefault.text);
}

void ReadPrefsFromRegistry(void) {
    ReadPrefsFromFile();
}

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

void WritePrefsToFile(void) {
    snprintf(
        gText,
        GLOBAL_TEXT_BUFFER_SIZE,
        "%s/%s",
        platform::Files().UserRoot().c_str(),
        gMiscText.writeFile.configFilename.text
    );
    i32 fd = platform::FileOpen(gText, platform::FileMode::Write);
    if (fd != -1) {
        platform::FileWrite(fd, &gConfig, CONFIG_PERSISTED_SIZE);
        platform::FileClose(fd);
    }
}

void WritePrefsToRegistry(void) {
    WritePrefsToFile();
}

void WritePrefs(void) {
    UpdateSystemOptionsMenu();
    WritePrefsToRegistry();
}

GameDataStatus VerifyGameData(void) {
    sprintf(gText, gMiscText.media.dataArchive.text);
    i32 file = platform::FileOpen(gText, platform::FileMode::Read);
    if (file == -1)
        return GAME_DATA_MISSING;
    platform::FileClose(file);

    sprintf(gText, gMiscText.media.animationFile.text);
    file = platform::FileOpen(gText, platform::FileMode::Read);
    if (file == -1)
        return GAME_DATA_MISSING;
    platform::FileClose(file);

    strcpy(gcAnimPath, ".\\HEROES2\\ANIM\\");
    return GAME_DATA_READY;
}

void BitmapToScreen(class bitmap* bmp) {
    BlitBitmapToScreen(bmp, 0, 0, bmp->m_width, bmp->m_height, 0, 0);
}

void SetPalette(i8* paletteData, i32 updateDisplay) {
    memcpy(gpBufferPalette->m_data, paletteData, MISC_PALETTE_BYTE_COUNT);
    memcpy(
        gCyclePal,
        paletteData + H2EnumIndex(CYCLE_RANGE_ONE_FIRST) * PALETTE_COMPONENT_COUNT,
        sizeof(gCyclePal)
    );
    if (updateDisplay != 0)
        UpdatePalette(gpBufferPalette->m_data);
}

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
void LogTruncate(void) {
    char logText[TEXT_BUFFER_SIZE];
    if (giDebugLevel >= FILE_DEBUG_LEVEL) {
        i32 fileHandle = platform::FileOpen(
            gMiscText.log.truncateFilename.text,
            platform::FileMode::Write
        );
        if (fileHandle != -1) {
            strcpy(logText, gMiscText.log.newLogLabel.text);
            *reinterpret_cast<u16*>(logText + strlen(logText)) =
                *reinterpret_cast<const u16*>(gMiscText.log.truncateNewline.text);
            platform::FileWrite(fileHandle, logText, strlen(logText));
            platform::FileClose(fileHandle);
        }
    }
}


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
                platform::Host().Log(platform::LogLevel::Debug, logText);
        }
    }
}

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
    if (giDebugLevel >= FILE_DEBUG_LEVEL) {
        FILE* file = fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
        if (file != NULL) {
            strcpy(logText, text);
            *reinterpret_cast<u16*>(logText + strlen(logText)) =
                *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
            fputs(logText, file);
            fclose(file);
            if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
                platform::Host().Log(platform::LogLevel::Debug, logText);
        }
    }
}

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

static inline i32 FadeThreshold(i32 level, i32 increment) {
    i32 index = MISC_PALETTE_LEVEL_COUNT - level - increment;
    if (index < 0)
        index = 0;
    return giChangeThreshold[index];
}

void FadeTo(u8* source, u8* destination, i32 increment) {
    u8 colors[MISC_PALETTE_BYTE_COUNT];
    memcpy(colors, source, sizeof(colors));
    increment >>= FADE_TO_INCREMENT_SHIFT;
    if (increment < 1)
        increment = 1;
    i32 level = FADE_TO_START_LEVEL;
    do {
        i32 delayUntil = platform::Ticks() + FADE_TO_FRAME_DELAY;
        PollSound();
        i32 threshold = FadeThreshold(level, increment);
        u8* current = colors;
        u8* target = destination;
        i32 count = MISC_PALETTE_BYTE_COUNT;
        do {
            i32 difference = static_cast<i32>(*target) - static_cast<i32>(*current);
            i32 distance = difference < 0 ? -difference : difference;
            if (distance > threshold) {
                distance -= threshold;
                if (difference > 0)
                    *current += static_cast<u8>(distance);
                else
                    *current -= static_cast<u8>(distance);
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

i32 IsCycleColor(i32 color) {
    if ((color >= CYCLE_RANGE_ONE_FIRST && color <= CYCLE_RANGE_ONE_LAST)
        || (color >= CYCLE_RANGE_TWO_FIRST && color <= CYCLE_RANGE_TWO_LAST)) {
        return 1;
    }
    return 0;
}

static inline i32 PCXValueIsLiteral(u8 value) {
    return (value & RLE_RUN_MARKER) != RLE_RUN_MARKER;
}

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
    i32 fileHandle = platform::FileOpen(filename, platform::FileMode::Write);
    if (fileHandle == -1)
        return;
    platform::FileWrite(fileHandle, &header, sizeof(header));
    u8* encodedRow =
        static_cast<u8*>(H2_ALLOC(width * 2));
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
            if (runLength <= 1 && PCXValueIsLiteral(value)) {
                encodedRow[encodedSize++] = value;
                ++sourceIndex;
            } else {
                encodedRow[encodedSize++] = static_cast<u8>(runLength | RLE_RUN_MARKER);
                encodedRow[encodedSize++] = value;
                sourceIndex += runLength;
            }
        }
        platform::FileWrite(fileHandle, encodedRow, encodedSize);
        pixels += width;
    }
    H2_FREE(encodedRow);
    u8 paletteMarker = VGA_PALETTE_MARKER;
    platform::FileWrite(fileHandle, &paletteMarker, 1);
    u8* outputPalette = static_cast<u8*>(
        H2_ALLOC(PALETTE_BYTE_COUNT)
    );
    for (i32 i = 0; i < PALETTE_BYTE_COUNT; ++i)
        outputPalette[i] = paletteData[i] << COMPONENT_SCALE_SHIFT;
    platform::FileWrite(fileHandle, outputPalette, PALETTE_BYTE_COUNT);
    H2_FREE(outputPalette);
    platform::FileClose(fileHandle);
}

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

struct IconEntry* GetIconEntry(class icon* iconPtr, i32 index) {
    return &iconPtr->Entries()[index];
}
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

void SRand(i32 seed) {
    iLastSeed = seed;
    srand(seed);
}

i32 SGenRand(void) {
    i32 result = 0;
    iLastSeed &= RANDOM_SEED_MASK;
    i32 mix = iLastSeed * RANDOM_MIX_MULTIPLIER;
    mix += (mix & RANDOM_MIX_MASK) >> RANDOM_MIX_SHIFT;
    for (i32 i = RANDOM_TOP_BIT; i >= 0; --i) {
        if (mix & (1 << i)) {
            result |= 1 << i;
        }
    }
    iLastSeed = mix;
    return result;
}

i32 MemSize(i32) {
    return REPORTED_MEMORY_KILOBYTES;
}
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
    message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
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
        FONT_DRAW_DARK_GRAY,
        gMiscText.dataEntry.iconFilename.text,
        TEXT_FIELD_ICON_FRAME,
        ENTRY_TEXT_WIDGET,
        WIDGET_KIND_NONE,
        TEXT_ENTRY_LAYOUT_INSET,
        TEXT_FIELD_HORIZONTAL_INSET,
        TEXT_FIELD_VERTICAL_INSET
    );
    if (entry == NULL)
        MemError();
    inBoxY = entryY + INPUT_BOX_Y_OFFSET;
    inBoxX = INPUT_BOX_X;
    DataEntryWin->AddWidget(entry, WIDGET_Z_ORDER);

    if (useImmediateHandler != 0) {
        bDataEntryTime = ENTRY_PHASE_IMMEDIATE;
        gbAllowTextEntryEscape = false;
    } else
        bDataEntryTime = ENTRY_PHASE_READY;
    gpWindowManager->DoDialog(DataEntryWin, DataEntryWindowHandler, 0);
    delete DataEntryWin;
    gpMouseManager->SetPointer(
        gMiscText.dataEntry.restoredMouseFilename.text,
        savedCursorFrame,
        savedCursorType
    );
    gbAllowTextEntryEscape = true;
}

MessageDispatchResult DataEntryWindowHandler(struct tag_message& message) {
    if (bDataEntryTime == ENTRY_PHASE_IMMEDIATE) {
        ++bDataEntryTime;
        message.type = MESSAGE_LEFT_BUTTON_DOWN;
        message.payload.mouse.x = inBoxX;
        message.payload.mouse.y = inBoxY;
        DataEntryWin->BroadcastMessage(message);
        return MESSAGE_DISPATCH_CONSUME;
    }

    if (bDataEntryTime == ENTRY_PHASE_POINTER_SENT)
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
    if (strlen(message.payload.widget.data.text) == 0)
        goto normalEvent;
    memset(cDEDest, 0, iDEMaxLen);
    strncpy(cDEDest, message.payload.widget.data.text, iDEMaxLen - 1);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = ENTRY_TEXT_WIDGET;
    message.payload.widget.data.text = cDEDest;
    DataEntryWin->BroadcastMessage(message);
    DataEntryWin->DrawWindow(DRAW_MODE, REDRAW_OFFSET, REDRAW_OFFSET);
    if (gbTextEntryEscaped != 0)
        goto normalEvent;
    gpWindowManager->m_dialogResult = message.payload.widget.id;
    message.payload.widget.id = ENTRY_TEXT_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
    return MESSAGE_DISPATCH_FORWARD;

possibleCancelEvent:
    if (message.payload.widget.id != ENTRY_CANCEL_BUTTON)
        goto normalEvent;
    message.payload.widget.id = ENTRY_TEXT_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
    return MESSAGE_DISPATCH_FORWARD;

normalEvent:
    return EventWindowHandler(message);
}

class heroWindow* DataEntryWin;
char* cDEDest;
i32 iDEMaxLen;
H2SteppedEnumStorage<DataEntryPhase, i32> bDataEntryTime;
i32 inBoxX;
i32 inBoxY;
