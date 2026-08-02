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

#define MISC_REGISTRY_KEY "SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0"
#include <BASE/MiscEnums.h>
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
    TEXT_FIELD_ICON_FRAME       = 3,
    TEXT_FIELD_HORIZONTAL_INSET = 10,
    TEXT_FIELD_VERTICAL_INSET   = 3,
    INPUT_BOX_X                 = 213,
    REDRAW_OFFSET               = 10,
    DRAW_MODE                   = 1,
    WIDGET_Z_ORDER              = -1
H2_ENUM_END(DataEntryLayout)

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

H2_ENUM_BEGIN(MiscLogPrivateConstant)
    MEMORY_LEAK_DEBUG_LEVEL   = 1,
    FILE_DEBUG_LEVEL          = 2,
    DEBUGGER_OUTPUT_LEVEL     = 4,
    FORCED_DEBUG_LEVEL        = 9,
    FORMAT_BUFFER_SIZE        = 200,
    TEXT_BUFFER_SIZE          = 500,
    MEMORY_ENTRY_CAPACITY     = 2000,
    REPORTED_MEMORY_KILOBYTES = 16034,
    ENTRY_SEARCH_COMPLETE     = 99999
H2_ENUM_END(MiscLogPrivateConstant)

H2_ENUM_BEGIN(MiscGameDefaultConstant)
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
    CD_DRIVE_QUERY_PATH_SIZE    = 256,
    CD_MCI_BUFFER_SIZE          = 256,
    CD_MCI_RESULT_LENGTH        = 0xFF,
    CD_PROBE_TRAILER_SIZE       = 100,
    CD_RETRY_DELAY_MILLISECONDS = 3000,
    CD_RETRY_LIMIT              = 2
H2_ENUM_END(MiscCDDriveConstant)

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

H2_ENUM_BEGIN(MiscCycleColorRange)
    CYCLE_RANGE_ONE_FIRST = 0xd6,
    CYCLE_RANGE_ONE_LAST  = 0xdd,
    CYCLE_RANGE_TWO_FIRST = 0xe7,
    CYCLE_RANGE_TWO_LAST  = 0xed
H2_ENUM_END(MiscCycleColorRange)

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

H2_ENUM_BEGIN(StatusBarLayout)
    STATUS_BAR_WIDTH   = 640,
    STATUS_BAR_Y       = 460,
    STATUS_BAR_HEIGHT  = 20,
    STATUS_TEXT_Y      = 464,
    STATUS_TEXT_HEIGHT = 16
H2_ENUM_END(StatusBarLayout)

VA(0x004bd4b0, 0x75)
void InitMemEntry(void) {
    LogInt(gMemEntryTag, iMemEntries, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE,
           LOG_UNUSED_VALUE, LOG_UNUSED_VALUE);
    gpMemEntry = static_cast<MemEntry*>(malloc(MEMORY_ENTRY_CAPACITY * sizeof(MemEntry)));
    for (i32 i = 0; i < MEMORY_ENTRY_CAPACITY; ++i)
        gpMemEntry[i].used = 0;
}

VA(0x004bd530, 0x113)
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
            "KBAlloc    Size %d   Ptr %d   File %s  Line %d",
            size,
            ptr,
            originalFile,
            originalLine
        );
        if (giDebugLevel >= FILE_DEBUG_LEVEL) {
            FILE* logFile = fopen("KB.LOG", "at+");
            if (logFile != NULL) {
                strcpy(logText, text);
                *reinterpret_cast<u16*>(logText + strlen(logText)) =
                    *reinterpret_cast<const u16*>("\n");
                fputs(logText, logFile);
                fclose(logFile);
                if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
                    OutputDebugStringA(logText);
            }
        }
    }
    return ptr;
}

VA(0x004bd650, 0x154)
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
            "Free ",
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
            FILE* logFile = fopen("KB.LOG", "at+");
            if (logFile != NULL) {
                strcpy(logText, "NULL POINTER");
                *reinterpret_cast<u16*>(logText + strlen(logText)) =
                    *reinterpret_cast<const u16*>("\n");
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
            "MemEntries Below 0",
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
                    "KBFree    Size %d   Ptr %d   File %s  Line %d",
                    gpMemEntry[entryIndex].size,
                    ptr,
                    gpMemEntry[entryIndex].file,
                    gpMemEntry[entryIndex].line
                );
                if (giDebugLevel >= FILE_DEBUG_LEVEL) {
                    FILE* logFile =
                        fopen("KB.LOG", "at+");
                    if (logFile != NULL) {
                        strcpy(logText, text);
                        *reinterpret_cast<u16*>(logText + strlen(logText)) =
                            *reinterpret_cast<const u16*>("\n");
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
        sprintf(gText, "Bad Delete,  File '%13s'  Line % 4d, ptr %12d", originalFile, originalLine, ptr);
        if (giDebugLevel >= FILE_DEBUG_LEVEL) {
            FILE* logFile = fopen("KB.LOG", "at+");
            if (logFile != NULL) {
                strcpy(logText, gText);
                *reinterpret_cast<u16*>(logText + strlen(logText)) =
                    *reinterpret_cast<const u16*>("\n");
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

VA(0x004bd7b0, 0xe7)
void PrintMemoryLeaks(void) {
    char logText[TEXT_BUFFER_SIZE];
    if (giDebugLevel >= MEMORY_LEAK_DEBUG_LEVEL && gpMemEntry != NULL) {
        LogInt(
            "Total Memory Leaks",
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
                    "Memory Leak,  File '%13s'  Line % 4d, ptr %12d   size %6d",
                    gpMemEntry[entryIndex].file,
                    gpMemEntry[entryIndex].line,
                    reinterpret_cast<i32>(gpMemEntry[entryIndex].ptr),
                    gpMemEntry[entryIndex].size
                );
                if (giDebugLevel >= FILE_DEBUG_LEVEL) {
                    FILE* logFile =
                        fopen("KB.LOG", "at+");
                    if (logFile != NULL) {
                        strcpy(logText, gText);
                        *reinterpret_cast<u16*>(logText + strlen(logText)) =
                            *reinterpret_cast<const u16*>("\n");
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

VA(0x004bd8a0, 0x35)
void ShowMemoryStatus(void) {
    sprintf(gText, "Mem Left %dK", REPORTED_MEMORY_KILOBYTES);
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

VA(0x004bd8e0, 0x10a)
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

VA(0x004bd9f0, 0xe7)
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

VA(0x004bdae0, 0x1af)
void FadeIn(i32 increment) {
    palette* fadePalette = new palette;
    if (fadePalette == NULL)
        MemError();
    i32 done = 0;
    if (gConfig.gfx[IDX(giCurExe)].fullScreen == 0)
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

VA(0x004bdc90, 0x1b2)
void FadeOut(i32 increment) {
    palette* fadePalette = new palette;
    if (fadePalette == NULL)
        MemError();
    i32 done = 0;
    if (gConfig.gfx[IDX(giCurExe)].fullScreen == 0)
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

VA(0x004bde50, 0x40)
i32 Random(i32 low, i32 high) {
    if (high == low) {
        return high;
    }
    if (high < low) {
        return low;
    }
    return rand() % (high - low + 1) + low;
}

VA(0x004bde90, 0x74)
void ProcessAssert(i32 condition, char* file, i32 line) {
    if (condition == 0) {
        gpMouseManager->SetColorMice(0);
        SetFullScreenStatus(0);
        sprintf(gText, "Assert statement failed in module %s, line %d.  Do you wish to abort the program?", file, line);
        if (MessageBoxA(hwndApp, gText, "Assert Failure", MB_YESNO | MB_ICONHAND)
            != IDNO) {
            ShutDown(NULL);
        }
    }
}

VA(0x004bdf10, 0x75)
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

VA(0x004bdf90, 0x56)
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

VA(0x004bdff0, 0x56)
char* FindLastToken(char* text, char token) {
    for (i32 i = strlen(text) - 1; i >= 0; --i) {
        if (text[i] == token) {
            return text + i;
        }
    }
    return NULL;
}

VA(0x004be050, 0x47)
void SetInstallDefaults(void) {
    memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
    strcpy(gConfig.autoLoadName, "AUTO");
    strcpy(gConfig.autoSaveName, "AUTO");
    gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
}
VA(0x004be0a0, 0x29d)
void SetGameDefaults(void) {
    gConfig.musicVolume = CONFIG_VOLUME_MIN;
    gConfig.soundVolume = CONFIG_VOLUME_MIN;
    gConfig.autosave = 1;
    gConfig.showRoute = 1;
    i32* fullScreen = &gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].fullScreen;
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
    gConfig.useOpera = CONFIG_OPERA_ENABLED;
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
    strcpy(gConfig.networkDefaultName, "The Unknown Hero");
    *reinterpret_cast<i32*>(gConfig.uniqueSystemID) = 0;
    i32 idSeed = rand() % UNIQUE_ID_RANDOM_MODULUS + 1;
    idSeed += KBTickCount();
    gConfig.uniqueSystemID[UNIQUE_ID_TRAILING_INDEX] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"[idSeed % UNIQUE_ID_ALPHANUMERIC_COUNT];
    i32 idAdd = rand() % UNIQUE_ID_RANDOM_MODULUS + 1;
    idAdd += KBTickCount();
    idSeed += idAdd;
    gConfig.uniqueSystemID[UNIQUE_ID_MIDDLE_INDEX] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"[idSeed % UNIQUE_ID_ALPHANUMERIC_COUNT];
    idAdd = rand() % UNIQUE_ID_RANDOM_MODULUS + 1;
    idAdd += KBTickCount();
    idSeed += idAdd;
    gConfig.uniqueSystemID[UNIQUE_ID_LEADING_INDEX] =
        static_cast<char>(idSeed % UNIQUE_ID_ALPHA_COUNT + 'A');
    gConfig.needsDefaultInitialization = 0;
}

VA(0x004be340, 0xcb)
void ReadPrefsFromFile(void) {
    sprintf(gText, "%s", "HEROES2.CFG");
    if (access(gText, 0) == -1) {
        memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
        strcpy(gConfig.autoLoadName, "AUTO");
        strcpy(gConfig.autoSaveName, "AUTO");
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
    } else {
        FILE* f = fopen(gText, "rb");
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
    strcpy(gcRegCDRomPath, "");
    strcpy(gcRegAppPath, "");
}

H2_ENUM_BEGIN(RegistryValueSize)
    REGISTRY_TEXT_BUFFER_SIZE = 100,
    REGISTRY_DWORD_BYTES      = 4,
    CONFIG_ZERO_BUFFER_WORDS  = 25,
    MODEM_INIT_STRING_SIZE    = 0x62,
    UNIQUE_SYSTEM_ID_SIZE     = 4,
    NETWORK_DEFAULT_NAME_SIZE = 0x1e
H2_ENUM_END(RegistryValueSize)

VA(0x004be410, 0x89f)
void ReadPrefsFromRegistry(void) {
    HKEY hKey;
    DWORD dwType;
    DWORD dwSize;
    char szKey[REGISTRY_TEXT_BUFFER_SIZE];
    char szScratch[REGISTRY_TEXT_BUFFER_SIZE];

    strcpy(szScratch, "");
    strcpy(szKey, MISC_REGISTRY_KEY);
    hKey = NULL;
    if (RegCreateKeyA(HKEY_LOCAL_MACHINE, szKey, &hKey) != 0)
        return;
    dwSize = REGISTRY_DWORD_BYTES;
    if (RegQueryValueExA(
            hKey,
            "Music Volume",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.musicVolume),
            &dwSize
        )
        != 0) {
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
    RegQueryValueExA(
        hKey,
        "Music Volume",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.musicVolume),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Sound Volume",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.soundVolume),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Walk Speed",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.walkSpeed),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Computer Walk Speed",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.computerWalkSpeed),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Show Route",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.showRoute),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Blackout Computer",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.blackoutComputer),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Sound Quality",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.musicSource),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Use Opera",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.useOpera),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Direct Connect Com Port",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.comPort[IDX(CONFIG_CONNECTION_DIRECT)]),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Direct Connect Baud Rate",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.baudRate[IDX(CONFIG_CONNECTION_DIRECT)]),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Modem Com Port",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.comPort[IDX(CONFIG_CONNECTION_MODEM)]),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Modem Baud Rate",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.baudRate[IDX(CONFIG_CONNECTION_MODEM)]),
        &dwSize
    );
    dwSize = MODEM_INIT_STRING_SIZE + 1;
    RegQueryValueExA(
        hKey,
        "Modem Init String",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(gConfig.modemInitString),
        &dwSize
    );
    dwSize = REGISTRY_DWORD_BYTES;
    RegQueryValueExA(
        hKey,
        "Unique System ID",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(gConfig.uniqueSystemID),
        &dwSize
    );
    gConfig.uniqueSystemID[UNIQUE_ID_TERMINATOR_INDEX] = 0;
    dwSize = NETWORK_DEFAULT_NAME_SIZE + 1;
    RegQueryValueExA(
        hKey,
        "Network Default Name",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(gConfig.networkDefaultName),
        &dwSize
    );
    dwSize = REGISTRY_DWORD_BYTES;
    RegQueryValueExA(
        hKey,
        "Autosave",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.autosave),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Slow Video",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.slowVideo),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Show Combat Grid",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.showCombatGrid),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Show Combat Mouse Hex",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.showCombatMouseHex),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Combat Shade Level",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.combatShadeLevel),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Combat Army Info Level",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.combatArmyInfoLevel),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Evil Interface Usage",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.evilInterfaceUsage),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Quick Combat Level",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.quickCombatLevel),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Combat Speed",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.combatSpeed),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Auto Combat Use Spells",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.autoCombatUseSpells),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "First Map Offset",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.firstMapOffset),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Current Map Offset",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.currentMapOffset),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Show Object Boxes",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.showObjectBoxes),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Editor Screen Animation",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.editorScreenAnimation),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Editor Palette Cycling",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.editorPaletteCycling),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Main Game Show Menu",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].showMenu),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Main Game X",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].x),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Main Game Y",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].y),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Main Game Width",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].width),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Main Game Height",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].height),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Main Game Full Screen",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].fullScreen),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Main Game Color Mouse Cursor",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].colorMouseCursor),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Editor Show Menu",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].showMenu),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Editor X",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].x),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Editor Y",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].y),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Editor Width",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].width),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Editor Height",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].height),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Editor Full Screen",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].fullScreen),
        &dwSize
    );
    RegQueryValueExA(
        hKey,
        "Editor Color Mouse Cursor",
        NULL,
        &dwType,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].colorMouseCursor),
        &dwSize
    );
    dwSize = MODEM_INIT_STRING_SIZE + 1;
    if (RegQueryValueExA(
            hKey,
            "AppPath",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(gcRegAppPath),
            &dwSize
        )
        != 0)
        strcpy(gcRegAppPath, "");
    if (RegQueryValueExA(
            hKey,
            "CDDrive",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(gcRegCDRomPath),
            &dwSize
        )
        != 0)
        strcpy(gcRegCDRomPath, "");
    RegCloseKey(hKey);
    if (gConfig.gfx[IDX(giCurExe)].width <= 0)
        gConfig.gfx[IDX(giCurExe)].width = MINIMUM_WINDOW_WIDTH;
    if (gConfig.gfx[IDX(giCurExe)].height <= 0)
        gConfig.gfx[IDX(giCurExe)].height = MINIMUM_WINDOW_HEIGHT;
    if (gConfig.gfx[IDX(giCurExe)].x < 0)
        gConfig.gfx[IDX(giCurExe)].x = 0;
    if (gConfig.gfx[IDX(giCurExe)].x > giMainVideoModeHeight - WINDOW_POSITION_MARGIN)
        gConfig.gfx[IDX(giCurExe)].x = giMainVideoModeHeight - WINDOW_POSITION_MARGIN;
    if (gConfig.gfx[IDX(giCurExe)].y < 0)
        gConfig.gfx[IDX(giCurExe)].y = 0;
    if (gConfig.gfx[IDX(giCurExe)].y > giMainVideoModeWidth - WINDOW_POSITION_MARGIN)
        gConfig.gfx[IDX(giCurExe)].y = giMainVideoModeWidth - WINDOW_POSITION_MARGIN;
}

VA(0x004becb0, 0xa8)
void ReadPrefs(void) {
    memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
    ReadPrefsFromRegistry();
    sprintf(gConfig.rmtRLName, "RMT%sRL.BIN", gConfig.uniqueSystemID);
    sprintf(gConfig.rmtRCName, "RMT%sRC.BIN", gConfig.uniqueSystemID);
    sprintf(gConfig.rmtRDName, "RMT%sRD.BIN", gConfig.uniqueSystemID);
    sprintf(gConfig.rmtSLName, "RMT%sSL.BIN", gConfig.uniqueSystemID);
    sprintf(gConfig.rmtSCName, "RMT%sSC.BIN", gConfig.uniqueSystemID);
    sprintf(gConfig.rmtSDName, "RMT%sSD.BIN", gConfig.uniqueSystemID);
}

VA(0x004bed60, 0x63)
void WritePrefsToFile(void) {
    i32 zeroBuffer[CONFIG_ZERO_BUFFER_WORDS];
    i32 i;
    i32* p = zeroBuffer;
    for (i = CONFIG_ZERO_BUFFER_WORDS; i != 0; i--) {
        *p = 0;
        p++;
    }
    sprintf(gText, "%s", "HEROES2.CFG");
    i32 fd = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fd != -1) {
        write(fd, &gConfig, CONFIG_PERSISTED_SIZE);
        close(fd);
    }
}

VA(0x004bedd0, 0x4cb)
void WritePrefsToRegistry(void) {
    HKEY hKey;
    char szKey[REGISTRY_TEXT_BUFFER_SIZE];
    char szScratch[REGISTRY_TEXT_BUFFER_SIZE];

    strcpy(szScratch, "");
    strcpy(szKey, MISC_REGISTRY_KEY);
    hKey = NULL;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, szKey, 0, KEY_ALL_ACCESS, &hKey) != 0)
        return;
    RegSetValueExA(
        hKey,
        "Music Volume",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.musicVolume),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Sound Volume",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.soundVolume),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Walk Speed",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.walkSpeed),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Computer Walk Speed",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.computerWalkSpeed),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Show Route",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.showRoute),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Blackout Computer",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.blackoutComputer),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Sound Quality",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.musicSource),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Use Opera",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.useOpera),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Direct Connect Com Port",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.comPort[IDX(CONFIG_CONNECTION_DIRECT)]),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Direct Connect Baud Rate",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.baudRate[IDX(CONFIG_CONNECTION_DIRECT)]),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Modem Com Port",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.comPort[IDX(CONFIG_CONNECTION_MODEM)]),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Modem Baud Rate",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.baudRate[IDX(CONFIG_CONNECTION_MODEM)]),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Modem Init String",
        0,
        REG_SZ,
        reinterpret_cast<u8*>(gConfig.modemInitString),
        MODEM_INIT_STRING_SIZE
    );
    RegSetValueExA(
        hKey,
        "Unique System ID",
        0,
        REG_SZ,
        reinterpret_cast<u8*>(gConfig.uniqueSystemID),
        UNIQUE_SYSTEM_ID_SIZE
    );
    RegSetValueExA(
        hKey,
        "Network Default Name",
        0,
        REG_SZ,
        reinterpret_cast<u8*>(gConfig.networkDefaultName),
        NETWORK_DEFAULT_NAME_SIZE
    );
    RegSetValueExA(
        hKey,
        "Autosave",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.autosave),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Slow Video",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.slowVideo),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Show Combat Grid",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.showCombatGrid),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Show Combat Mouse Hex",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.showCombatMouseHex),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Combat Shade Level",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.combatShadeLevel),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Combat Army Info Level",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.combatArmyInfoLevel),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Evil Interface Usage",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.evilInterfaceUsage),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Quick Combat Level",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.quickCombatLevel),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Combat Speed",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.combatSpeed),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Auto Combat Use Spells",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.autoCombatUseSpells),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "First Map Offset",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.firstMapOffset),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Current Map Offset",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.currentMapOffset),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Show Object Boxes",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.showObjectBoxes),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Editor Screen Animation",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.editorScreenAnimation),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Editor Palette Cycling",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.editorPaletteCycling),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Main Game Show Menu",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].showMenu),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Main Game X",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].x),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Main Game Y",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].y),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Main Game Width",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].width),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Main Game Height",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].height),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Main Game Full Screen",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].fullScreen),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Main Game Color Mouse Cursor",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].colorMouseCursor),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Editor Show Menu",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].showMenu),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Editor X",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].x),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Editor Y",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].y),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Editor Width",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].width),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Editor Height",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].height),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Editor Full Screen",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].fullScreen),
        REGISTRY_DWORD_BYTES
    );
    RegSetValueExA(
        hKey,
        "Editor Color Mouse Cursor",
        0,
        REG_DWORD,
        reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].colorMouseCursor),
        REGISTRY_DWORD_BYTES
    );
    RegCloseKey(hKey);
}

VA(0x004bf2a0, 0xf)
void WritePrefs(void) {
    UpdateSystemOptionsMenu();
    WritePrefsToRegistry();
}

VA(0x004bf2b0, 0x3f)
i32 IsCDDrive(i32 driveIndex) {
    sprintf(gText, "A:\\");
    gText[0] = gText[0] + driveIndex;
    return GetDriveTypeA(gText) == DRIVE_CDROM;
}

VA(0x004bf2f0, 0x7b)
bool DriveSupportsFreeSpaceQuery(char driveLetter) {
    char rootPath[CD_DRIVE_QUERY_PATH_SIZE];
    ULARGE_INTEGER availableBytes;
    ULARGE_INTEGER totalBytes;
    ULARGE_INTEGER freeBytes;

    wsprintfA(rootPath, "%c:", driveLetter);
    UINT previousErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    BOOL result = GetDiskFreeSpaceExA(rootPath, &availableBytes, &totalBytes, &freeBytes);
    SetErrorMode(previousErrorMode);
    return result != 0;
}

VA(0x004bf370, 0x35f)
H2_ENUM_RETURN(CDRomSetupResult, i32) SetupCDDrive(void) {
    char registryPath[CD_PATH_BUFFER_SIZE];
    char registryKey[CD_PATH_BUFFER_SIZE];
    char cdDrives[CD_DRIVE_SLOT_COUNT];
    char count;
    i32 attempts;
    HKEY key;

    sprintf(gText, ".\\DATA\\HEROES2.AGG");
    i32 file = open(gText, _O_BINARY);
    if (file == -1) {
        if (_chdir(gcRegAppPath) == -1)
            return CD_ROM_GAME_DIRECTORY_MISSING;
        file = open(gText, _O_BINARY);
        if (file == -1)
            return CD_ROM_DATA_FILES_MISSING;
    }
    close(file);

    u32l logicalDrives = GetLogicalDrives();
    i32 cdDriveCount = 0;
    memset(cdDrives, 0, sizeof(cdDrives));
    for (i32 drive = CD_FIRST_DRIVE_INDEX; drive < CD_DRIVE_SLOT_COUNT; ++drive) {
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
        file = open(gText, _O_BINARY);
        if (file != -1) {
            close(file);
            sprintf(gText + CD_PATH_PREFIX_BYTES, "%s", gcAnimPath);
            strcpy(gcAnimPath, gText);
            return CD_ROM_READY;
        }
    }

    typedef i32 (__cdecl *CDFormatCommand)(char*, const char*, ...);
    CDFormatCommand formatCommand = wsprintfA;
    attempts = 0;
    {
        char resultBuffer[CD_MCI_BUFFER_SIZE];
        char command[CD_MCI_BUFFER_SIZE];
        for (;;) {
            for (i32 index = 0; index < count; ++index) {
                formatCommand(command, "open %c: type cdaudio alias CD", cdDrives[index] + 'A');
                if (mciSendStringA(command, resultBuffer, CD_MCI_RESULT_LENGTH, NULL) == 0) {
                    formatCommand(command, "info CD UPC wait");
                    mciSendStringA(command, resultBuffer, CD_MCI_RESULT_LENGTH, NULL);
                    formatCommand(command, "close CD");
                    mciSendStringA(command, resultBuffer, CD_MCI_RESULT_LENGTH, NULL);
                }
                sprintf(gText, "%c:\\heroes2\\anim\\voy24.smk", cdDrives[index] + 'A');
                file = open(gText, _O_BINARY);
                if (file != -1) {
                    if (lseek(file, 0, SEEK_END) != -1
                        && lseek(file, -CD_PROBE_TRAILER_SIZE, SEEK_CUR) != -1)
                        read(file, resultBuffer, CD_PROBE_TRAILER_SIZE);
                    close(file);

                    strcpy(registryKey, MISC_REGISTRY_KEY);
                    key = NULL;
                    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, registryKey, 0, KEY_WRITE, &key) == 0) {
                        wsprintfA(
                            registryPath,
                            "%c:",
                            cdDrives[index] + 'A'
                        );
                        RegSetValueExA(
                            key,
                            "CDDrive",
                            0,
                            REG_SZ,
                            reinterpret_cast<u8*>(registryPath),
                            lstrlenA(registryPath) + 1
                        );
                        RegCloseKey(key);
                    }
                    sprintf(
                        gText,
                        "%c:%s",
                        cdDrives[index] + 'A',
                        gcAnimPath
                    );
                    strcpy(gcAnimPath, gText);
                    return CD_ROM_READY;
                }
            }
            Sleep(CD_RETRY_DELAY_MILLISECONDS);
            ++attempts;
            if (attempts >= CD_RETRY_LIMIT)
                return CD_ROM_EXPANSION_DISC_MISSING;
        }
    }
}

VA(0x004bf6d0, 0x2b)
void BitmapToScreen(class bitmap* bmp) {
    BlitBitmapToScreen(bmp, 0, 0, bmp->m_width, bmp->m_height, 0, 0);
}

VA(0x004bf700, 0x5b)
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

VA(0x004bf760, 0x36)
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

VA(0x004bf7a0, 0x1e4)
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
VA(0x004bf990, 0x91)
void LogTruncate(void) {
    char logText[TEXT_BUFFER_SIZE];
    if (giDebugLevel >= FILE_DEBUG_LEVEL) {
        i32 fileHandle = open(
            "KB.LOG",
            _O_WRONLY | _O_CREAT | _O_TRUNC | _O_TEXT,
            _S_IWRITE
        );
        if (fileHandle != -1) {
            strcpy(logText, "===========New Log==========");
            *reinterpret_cast<u16*>(logText + strlen(logText)) =
                *reinterpret_cast<const u16*>("\n");
            write(fileHandle, logText, strlen(logText));
            close(fileHandle);
        }
    }
}


VA(0x004bfa30, 0x9a)
void LogStr(char* text) {
    char logText[TEXT_BUFFER_SIZE];
    if (giDebugLevel >= FILE_DEBUG_LEVEL) {
        FILE* logFile = fopen("KB.LOG", "at+");
        if (logFile != NULL) {
            strcpy(logText, text);
            *reinterpret_cast<u16*>(logText + strlen(logText)) =
                *reinterpret_cast<const u16*>("\n");
            fputs(logText, logFile);
            fclose(logFile);
            if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
                OutputDebugStringA(logText);
        }
    }
}

VA(0x004bfad0, 0x1b6)
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
            "%s : % 8d % 8d % 8d % 8d % 8d % 8d % 8d",
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
            "%s : % 8d % 8d % 8d % 8d % 8d % 8d",
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
            "%s : % 8d % 8d % 8d % 8d % 8d",
            label,
            value1,
            value2,
            value3,
            value4,
            value5
        );
    else if (value4 != LOG_UNUSED_VALUE)
        sprintf(text, "%s : % 8d % 8d % 8d % 8d", label, value1, value2, value3, value4);
    else if (value3 != LOG_UNUSED_VALUE)
        sprintf(text, "%s : % 8d % 8d % 8d", label, value1, value2, value3);
    else if (value2 != LOG_UNUSED_VALUE)
        sprintf(text, "%s : % 8d % 8d", label, value1, value2);
    else
        sprintf(text, "%s : % 8d", label, value1);
    if (giDebugLevel >= FILE_DEBUG_LEVEL) {
        FILE* file = fopen("KB.LOG", "at+");
        if (file != NULL) {
            strcpy(logText, text);
            *reinterpret_cast<u16*>(logText + strlen(logText)) =
                *reinterpret_cast<const u16*>("\n");
            fputs(logText, file);
            fclose(file);
            if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
                OutputDebugStringA(logText);
        }
    }
}

VA(0x004bfc90, 0x76)
void AiPrint(char* text) {
    if (giDebugLevel < FILE_DEBUG_LEVEL)
        return;

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

VA(0x004bfd10, 0x30)
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

VA(0x004bfd40, 0x19c)
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

VA(0x004bfee0, 0x163)
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

VA(0x004c0050, 0x44)
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

VA(0x004c00a0, 0x2a9)
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
    i32 fileHandle = open(filename, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fileHandle == -1)
        return;
    write(fileHandle, &header, sizeof(header));
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
        write(fileHandle, encodedRow, encodedSize);
        pixels += width;
    }
    H2_FREE(encodedRow);
    u8 paletteMarker = VGA_PALETTE_MARKER;
    write(fileHandle, &paletteMarker, 1);
    u8* outputPalette = static_cast<u8*>(
        H2_ALLOC(PALETTE_BYTE_COUNT)
    );
    for (i32 i = 0; i < PALETTE_BYTE_COUNT; ++i)
        outputPalette[i] = paletteData[i] << COMPONENT_SCALE_SHIFT;
    write(fileHandle, outputPalette, PALETTE_BYTE_COUNT);
    H2_FREE(outputPalette);
    close(fileHandle);
}

VA(0x004c0350, 0x73)
i32l FileSize(char* filename) {
    FILE* file = fopen(filename, "r+b");
    if (file == NULL) {
        FileError(filename);
    }
    fseek(file, 0, SEEK_END);
    i32l size = ftell(file);
    fseek(file, 0, SEEK_SET);
    fclose(file);
    return size;
}

VA(0x004c03d0, 0x1e)
struct IconEntry* GetIconEntry(class icon* iconPtr, i32 index) {
    return &iconPtr->Entries()[index];
}
VA(0x004c03f0, 0x72)
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

VA(0x004c0470, 0x92)
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

VA(0x004c0510, 0x1f)
void SRand(i32 seed) {
    iLastSeed = seed;
    srand(seed);
}

VA(0x004c0530, 0x92)
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

VA(0x004c05d0, 0x10)
i32 MemSize(i32) {
    return REPORTED_MEMORY_KILOBYTES;
}
VA(0x004c05e0, 0x464)
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
    gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);

    cDEDest = destination;
    iDEMaxLen = maximumLength;
    strcpy(destination, "");

    i32 rows = bigFont->LineLength(prompt, PROMPT_WIDTH) * PROMPT_LINE_HEIGHT;
    if (showCancel != 0)
        rows += CANCEL_PROMPT_HEIGHT;
    rows = (rows + ROW_ROUNDING_BIAS) / ROW_HEIGHT;
    if (rows > MAX_ROW_COUNT)
        rows = MAX_ROW_COUNT;
    i32 entryY = rows * ROW_HEIGHT - (showCancel != 0 ? CANCEL_Y_OFFSET : 0) + ENTRY_BASE_Y;

    char windowName[WINDOW_NAME_CAPACITY];
    sprintf(windowName, "evntwin%d.bin", rows);
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
        initialText = "";
    strcpy(entryText, initialText);
    message.payload.widget.id = ENTRY_TEXT_WIDGET;
    message.payload.widget.data.text = entryText;
    DataEntryWin->BroadcastMessage(message);
    strcpy(destination, entryText);

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.id = ENTRY_BUTTON_ONE;
    message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
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
        "bigfont.fnt",
        FONT_DRAW_DARK_GRAY,
        "buybuild.icn",
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
        "",
        savedCursorFrame,
        savedCursorType
    );
    gbAllowTextEntryEscape = true;
}

VA(0x004c0a50, 0x1fa)
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
H2_ENUM_STORAGE_STEPPED(DataEntryPhase, i32) bDataEntryTime;
i32 inBoxX;
i32 inBoxY;
