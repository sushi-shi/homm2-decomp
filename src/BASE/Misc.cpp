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
#include <BASE/MiscState.h>

#define MISC_REGISTRY_KEY "SOFTWARE\\Buka\\3DO\\Heroes of Might and Magic Platinum\\1.000"
#include <BASE/MiscEnums.h>
H2_ENUM_BEGIN(DataEntryLayout)
    WINDOW_X                    = 0xb1,
    WINDOW_Y                    = 0x14,
    INPUT_BOX_Y_OFFSET          = 0x17,
    PROMPT_WIDTH                = 240,
    PROMPT_LINE_HEIGHT          = 16,
    CANCEL_PROMPT_HEIGHT        = 39,
    ROW_TOP_MARGIN              = 40,
    ROW_ROUNDING_BIAS           = 25,
    ROW_HEIGHT                  = 45,
    MAX_ROW_COUNT               = 6,
    CANCEL_Y_OFFSET             = 30,
    ENTRY_BASE_Y                = 50,
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
    DEFAULT_MAP_OFFSET_MAX       = 32000,
    UNIQUE_ID_RANDOM_MAX         = 999999,
    UNIQUE_ID_ALPHANUMERIC_COUNT = 36,
    UNIQUE_ID_ALPHA_COUNT        = 26,
    UNIQUE_ID_LEADING_INDEX      = 0,
    UNIQUE_ID_MIDDLE_INDEX       = 1,
    UNIQUE_ID_TRAILING_INDEX     = 2,
    UNIQUE_ID_TERMINATOR_INDEX   = 3
H2_ENUM_END(MiscGameDefaultConstant)

H2_ENUM_BEGIN(MiscCDDriveConstant)
    CD_FIRST_DRIVE_INDEX        = 2,
    CD_DRIVE_SLOT_COUNT         = 26,
    CD_PATH_BUFFER_SIZE         = 100,
    CD_DRIVE_QUERY_PATH_SIZE    = 256,
    CD_READ_BUFFER_SIZE         = 256,
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
    PALETTE_BLUE_INDEX          = 2
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
#include <SOURCE/KBDeclarations.h>
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

DATA(0x00536088) static i32 giFindMid = 0;
DATA(0x0053608c) H2_ENUM_STORAGE_STEPPED(DataEntryPhase, i32) bDataEntryTime =
    ENTRY_PHASE_IMMEDIATE;
DATA(0x00536090) i32 inBoxY = 0;
DATA(0x00536094) i32 inBoxX = 0;
DATA(0x00536098) i32 gBlitBottom = 0;
DATA(0x0053609c) i32 gBlitRight = 0;
DATA(0x005360a0) class heroWindow* DataEntryWin = NULL;
DATA(0x005360a4) char* cDEDest = NULL;
DATA(0x005360a8) i32 iDEMaxLen = 0;
DATA(0x005360ac) i32 iMemEntries = 0;
DATA(0x005360b0) MemEntry* gpMemEntry = NULL;
DATA(0x005360b4) i32 giTotalMemAllocated = 0;
DATA(0x0051e5dc) H2_CONST char* gcCDTrackName = gcCDTrackNameText;
DATA(0x0051e5e0) u8
    giChangeThreshold[FADE_CHANGE_THRESHOLD_COUNT] =
        {0, 1, 2, 3, 4, 6, 8, 10, 13, 16, 19, 22, 26, 31, 37, 46};
DATA(0x0051e5f0) i32 iLastSeed = INITIAL_SEED;
DATA(0x0051e5f4) static char gMemEntryTag[sizeof("IME")] = "IME";

H2_ENUM_BEGIN(StatusBarLayout)
    STATUS_BAR_WIDTH   = 640,
    STATUS_BAR_Y       = 460,
    STATUS_BAR_HEIGHT  = 20,
    STATUS_TEXT_Y      = 464,
    STATUS_TEXT_HEIGHT = 16
H2_ENUM_END(StatusBarLayout)

VA(0x004bd4b0, 0x75)
void InitMemEntry(void) {
    LogInt(gMemEntryTag, iMemEntries);
    gpMemEntry = static_cast<MemEntry*>(malloc(MEMORY_ENTRY_CAPACITY * sizeof(MemEntry)));
    for (i32 i = 0; i < MEMORY_ENTRY_CAPACITY; ++i)
        gpMemEntry[i].used = 0;
}

#if H2_RETAIL_COMPILER
#define pointer ptr
#endif
VA(0x004bd530, 0x113)
void* BaseAlloc(u32 size, H2_CONST char* originalFile, i32 originalLine) {
    if (size == 0)
        return NULL;
    if (gpMemEntry == NULL)
        InitMemEntry();
    giTotalMemAllocated += size;
    void* pointer = malloc(size);
    if (pointer == NULL) {
        MemError();
        return NULL;
    }
    ++iMemEntries;
    i32 entryIndex;
    for (entryIndex = 0; entryIndex < MEMORY_ENTRY_CAPACITY; ++entryIndex) {
        if (!gpMemEntry[entryIndex].used) {
            gpMemEntry[entryIndex].used = 1;
            gpMemEntry[entryIndex].ptr = pointer;
            gpMemEntry[entryIndex].size = size;
            strcpy(gpMemEntry[entryIndex].file, originalFile);
            gpMemEntry[entryIndex].line = originalLine;
            entryIndex = ENTRY_SEARCH_COMPLETE;
        }
    }
    return pointer;
}
#if H2_RETAIL_COMPILER
#undef pointer
#endif

#if H2_RETAIL_COMPILER
#define pointer ptr
#endif
VA(0x004bd650, 0x154)
void BaseFree(void* pointer, H2_CONST char* originalFile, i32 originalLine) {
    if (gpMemEntry == NULL)
        InitMemEntry();
    if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
        LogInt("Free ", reinterpret_cast<i32>(pointer));
    if (pointer == NULL) {
        LogStr("NULL POINTER");
        return;
    }
    --iMemEntries;
    if (iMemEntries < 0)
        LogInt("MemEntries Below 0", iMemEntries);
    i32 entryIndex;
    for (entryIndex = 0; entryIndex < MEMORY_ENTRY_CAPACITY; ++entryIndex) {
        if (gpMemEntry[entryIndex].ptr == pointer) {
            gpMemEntry[entryIndex].used = 0;
            giTotalMemAllocated -= gpMemEntry[entryIndex].size;
            entryIndex = ENTRY_SEARCH_COMPLETE;
        }
    }
    if (entryIndex < ENTRY_SEARCH_COMPLETE) {
        sprintf(
            gText,
            "Bad Delete,  File '%13s'  Line % 4d, ptr %12d",
            originalFile,
            originalLine,
            reinterpret_cast<i32>(pointer)
        );
        LogStr(gText);
    } else {
        free(pointer);
        pointer = NULL;
    }
}
#if H2_RETAIL_COMPILER
#undef pointer
#endif

VA(0x004bd7b0, 0xe7)
void PrintMemoryLeaks(void) {
    if (giDebugLevel < MEMORY_LEAK_DEBUG_LEVEL)
        return;
    if (gpMemEntry == NULL)
        return;
    LogInt("Total Memory Leaks", iMemEntries);
    for (i32 entryIndex = 0; entryIndex < MEMORY_ENTRY_CAPACITY; ++entryIndex) {
        if (gpMemEntry[entryIndex].used != 0) {
            sprintf(
                gText,
                "Memory Leak,  File '%13s'  Line % 4d, ptr %12d   size %6d",
                gpMemEntry[entryIndex].file,
                gpMemEntry[entryIndex].line,
                reinterpret_cast<i32>(gpMemEntry[entryIndex].ptr),
                gpMemEntry[entryIndex].size
            );
            LogStr(gText);
        }
    }
}

VA(0x004bd8a0, 0x35)
void ShowMemoryStatus(void) {
    i32 memLeft = MemSize(1);
    sprintf(gText, "Mem Left %dK", memLeft);
    AbsAiPrint(gText);
}

#if H2_RETAIL_COMPILER
#define buffer buf
#endif
VA(0x004bd8e0, 0x10a)
u32l MAKEFILEID(H2_CONST char* text) {
    u32 fileId;
    i32 size;
    char buffer[GLOBAL_AGGREGATE_PATH_SIZE];
    i32 total;
    i32 i;

    strcpy(buffer, text);
    fileId = 0;
    total = 0;
    size = strlen(buffer);
    for (i = size - 1; i >= 0; --i) {
        if (buffer[i] >= 'a' && buffer[i] <= 'z')
            buffer[i] &= ~('a' - 'A');
        fileId = (fileId << HASH_LEFT_SHIFT) + (fileId >> HASH_RIGHT_SHIFT);
        total += buffer[i];
        fileId += buffer[i] + total;
    }
    return fileId;
}
#if H2_RETAIL_COMPILER
#undef buffer
#endif

VA(0x004bd9f0, 0xe7)
i32 FindIndex(struct indexArray* entries, i32 low, i32 high, i32 key) {
    giFindMid = (low + high) >> 1;
    while (1) {
        if (high - low > 1) {
            if (key < entries[giFindMid].key)
                high = giFindMid;
            else if (key > entries[giFindMid].key)
                low = giFindMid;
            else
                return entries[giFindMid].value;
        } else {
            if (key == entries[low].key)
                return entries[low].value;
            if (key == entries[high].key)
                return entries[high].value;
            return INDEX_NOT_FOUND;
        }
        giFindMid = (low + high) >> 1;
    }
}

#include <BASE/MiscGraphicsConstants.h>

#if H2_RETAIL_COMPILER
#define currentPalette pal
#endif
VA(0x004bdae0, 0x1af)
void FadeIn(i32 increment) {
    b32 done;
    i32 i, j, delayTime, threshold;
    palette* currentPalette = new palette;
    if (currentPalette == NULL)
        MemError();
    done = false;
    if (CURRENT_GRAPHICS_CONFIG.fullScreen == 0)
        increment *= WINDOWED_FADE_INCREMENT_SCALE;
    memset(currentPalette->m_data, 0, MISC_PALETTE_BYTE_COUNT);
    for (i = 0; i < MISC_PALETTE_LEVEL_COUNT; i += increment) {
    fadeStep:
        delayTime = KBTickCount() + FADE_FRAME_DELAY;
        PollSound();
        if (i == MISC_PALETTE_MAX_LEVEL) {
            done = true;
            UpdatePalette(gpBufferPalette->m_data);
        } else {
            threshold = MISC_PALETTE_MAX_LEVEL - i;
            for (j = 0; j < MISC_PALETTE_BYTE_COUNT; ++j) {
                if (gpBufferPalette->m_data[j] > threshold)
                    currentPalette->m_data[j] = gpBufferPalette->m_data[j] - threshold;
            }
            UpdatePalette(currentPalette->m_data);
        }
        DelayTil(&delayTime);
    }
    if (done == 0) {
        i = MISC_PALETTE_MAX_LEVEL;
        goto fadeStep;
    }
    delete currentPalette;
}
#if H2_RETAIL_COMPILER
#undef currentPalette
#endif

#if H2_RETAIL_COMPILER
#define currentPalette pal
#endif
VA(0x004bdc90, 0x1b2)
void FadeOut(i32 increment) {
    b32 done;
    i32 i, j, delayTime;
    palette* currentPalette = new palette;
    if (currentPalette == NULL)
        MemError();
    done = false;
    if (CURRENT_GRAPHICS_CONFIG.fullScreen == 0)
        increment *= WINDOWED_FADE_INCREMENT_SCALE;
    memcpy(currentPalette->m_data, gpBufferPalette->m_data, MISC_PALETTE_BYTE_COUNT);
    for (i = 0; i < FADE_LEVEL_COUNT; i += increment) {
    fadeStep:
        delayTime = KBTickCount() + FADE_FRAME_DELAY;
        PollSound();
        if (i == FADE_LEVEL_LAST)
            done = true;
        for (j = 0; j < PALETTE_DATA_SIZE; ++j) {
            if (currentPalette->m_data[j] > 0) {
                if (currentPalette->m_data[j] > increment)
                    currentPalette->m_data[j] -= increment;
                else
                    currentPalette->m_data[j] = 0;
            }
        }
        UpdatePalette(currentPalette->m_data);
        DelayTil(&delayTime);
    }
    if (done == 0) {
        i = FADE_LEVEL_LAST;
        goto fadeStep;
    }
    delete currentPalette;
}
#if H2_RETAIL_COMPILER
#undef currentPalette
#endif

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
void ProcessAssert(i32 condition, H2_CONST char* file, i32 line) {
    i32 H2_UNUSED(unusedAssertWord);
    if (condition == 0) {
        gpMouseManager->SetColorMice(false);
        SetFullScreenStatus(false);
        sprintf(gText, "Assert statement failed in module %s, line %d.  Do you wish to abort the program?", file, line);
        if (MessageBoxA(hwndApp, gText, "Assert Failure", MB_YESNO | MB_ICONHAND) == IDNO)
            return;
        unusedAssertWord = 0;
        ShutDown(NULL);
    }
}

#if H2_RETAIL_COMPILER
#define length iLen
#define patternLength patternLen
#endif
VA(0x004bdf10, 0x75)
char* FindStringInString(char* text, H2_CONST char* pattern) {
    i32 length = strlen(text);
    i32 patternLength = strlen(pattern);
    for (i32 i = 0; i < length - patternLength + 1; ++i) {
        if (strncmp(text + i, pattern, patternLength) == 0)
            return text + i;
    }
    return NULL;
}
#if H2_RETAIL_COMPILER
#undef length
#undef patternLength
#endif

#if H2_RETAIL_COMPILER
#define length iLen
#endif
VA(0x004bdf90, 0x56)
char* FindToken(char* text, char token) {
    i32 length = strlen(text);
    for (i32 i = 0; i < length; ++i) {
        if (*(text + i) == token)
            return text + i;
    }
    return NULL;
}
#if H2_RETAIL_COMPILER
#undef length
#endif

#if H2_STRICT_ENUMS
H2_CONST char* FindToken(H2_CONST char* text, char token) {
    i32 iLen = strlen(text);
    for (i32 i = 0; i < iLen; ++i) {
        if (*(text + i) == token)
            return text + i;
    }
    return NULL;
}
#endif

#if H2_RETAIL_COMPILER
#define length iLen
#endif
VA(0x004bdff0, 0x56)
char* FindLastToken(char* text, char token) {
    i32 length = strlen(text);
    for (i32 i = length - 1; i >= 0; --i) {
        if (*(text + i) == token)
            return text + i;
    }
    return NULL;
}
#if H2_RETAIL_COMPILER
#undef length
#endif

VA(0x004be050, 0x47)
void SetInstallDefaults(void) {
    memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
    strcpy(gConfig.autoLoadName, "AUTO");
    strcpy(gConfig.autoSaveName, "AUTO");
    gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
}
VA(0x004be0a0, 0x29d)
void SetGameDefaults(void) {
    i32 i;
    i32 seed;
    i32 H2_UNUSED(nAlpha);
    H2_CONST char* alpha;

    gConfig.musicVolume = CONFIG_VOLUME_MIN;
    gConfig.soundVolume = CONFIG_VOLUME_MIN;
    gConfig.autosave = 1;
    gConfig.showRoute = 1;
    gConfig.blackoutComputer = false;
    for (i = IDX(CONFIG_EXECUTABLE_GAME); i < IDX(CONFIG_EXECUTABLE_COUNT); ++i) {
        gConfig.gfx[i].showMenu = 1;
        gConfig.gfx[i].x = DEFAULT_WINDOW_ORIGIN;
        gConfig.gfx[i].y = DEFAULT_WINDOW_ORIGIN;
        gConfig.gfx[i].colorMouseCursor = false;
        gConfig.gfx[i].fullScreen = true;
        if (giMainVideoModeWidth <= DEFAULT_WINDOW_WIDTH) {
            gConfig.gfx[i].width = DEFAULT_SMALL_WINDOW_WIDTH;
            gConfig.gfx[i].height = DEFAULT_SMALL_WINDOW_HEIGHT;
        } else {
            gConfig.gfx[i].width = DEFAULT_WINDOW_WIDTH;
            gConfig.gfx[i].height = DEFAULT_WINDOW_HEIGHT;
        }
    }
    gConfig.showCombatGrid = 0;
    gConfig.showCombatMouseHex = 0;
    gConfig.combatShadeLevel = 0;
    gConfig.combatArmyInfoLevel = 0;
    gConfig.evilInterfaceUsage = 0;
    gConfig.useOpera = CONFIG_OPERA_ENABLED;
    gConfig.quickCombatLevel = 0;
    gConfig.combatSpeed = 0;
    gConfig.autoCombatUseSpells = 0;
    gConfig.blackoutComputer = false;
    gConfig.currentMapOffset = 0;
    gConfig.firstMapOffset = Random(0, DEFAULT_MAP_OFFSET_MAX);
    gConfig.showObjectBoxes = 0;
    gConfig.editorScreenAnimation = 0;
    gConfig.editorPaletteCycling = 0;
    gbFirstTimeThrough = true;
    gConfig.walkSpeed = CONFIG_WALK_SPEED_NORMAL;
    gConfig.slowVideo = DEFAULT_SLOW_VIDEO;
    gConfig.computerWalkSpeed = CONFIG_WALK_SPEED_FAST;
    // Неизвестный герой
    strcpy(
        gConfig.networkDefaultName,
        localization::Tr("player.unknown_hero_name")
    );
    nAlpha = UNIQUE_ID_ALPHANUMERIC_COUNT;
    alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    memset(gConfig.uniqueSystemID, 0, CONFIG_UNIQUE_SYSTEM_ID_SIZE);
    seed = 0;
    seed += Random(1, UNIQUE_ID_RANDOM_MAX) + KBTickCount();
    gConfig.uniqueSystemID[UNIQUE_ID_TRAILING_INDEX] =
        alpha[seed % UNIQUE_ID_ALPHANUMERIC_COUNT];
    seed += Random(1, UNIQUE_ID_RANDOM_MAX) + KBTickCount();
    gConfig.uniqueSystemID[UNIQUE_ID_MIDDLE_INDEX] =
        alpha[seed % UNIQUE_ID_ALPHANUMERIC_COUNT];
    seed += Random(1, UNIQUE_ID_RANDOM_MAX) + KBTickCount();
    gConfig.uniqueSystemID[UNIQUE_ID_LEADING_INDEX] =
        static_cast<char>(seed % UNIQUE_ID_ALPHA_COUNT + 'A');
    gConfig.needsDefaultInitialization = 0;
}

VA(0x004be340, 0xcb)
void ReadPrefsFromFile(void) {
    i32 H2_UNUSED(result);
    FILE* file;

    sprintf(gText, "%s", "HEROES2.CFG");
    if (access(gText, 0) == -1) {
        SetInstallDefaults();
        SetGameDefaults();
        WritePrefs();
    } else {
        file = fopen(gText, "rb");
        if (file == NULL)
            FileError(gText);
        fread(&gConfig, CONFIG_PERSISTED_SIZE, 1, file);
        result = fclose(file);
        if (gConfig.needsDefaultInitialization != 0) {
            SetGameDefaults();
            WritePrefs();
        }
    }
    strcpy(
        gcRegCDRomPath,
        ""
    );
    strcpy(
        gcRegAppPath,
        ""
    );
}

H2_ENUM_BEGIN(RegistryValueSize)
    REGISTRY_TEXT_BUFFER_SIZE = 100,
    REGISTRY_DWORD_BYTES      = 4,
    MODEM_INIT_STRING_SIZE    = 0x62,
    UNIQUE_SYSTEM_ID_SIZE     = 4,
    NETWORK_DEFAULT_NAME_SIZE = 0x1e
H2_ENUM_END(RegistryValueSize)

VA(0x004be410, 0x89f)
void ReadPrefsFromRegistry(void) {
    DWORD dwcbData;
    HKEY hKey;
    char szKey[REGISTRY_TEXT_BUFFER_SIZE];
    char H2_UNUSED(szScratch)[REGISTRY_TEXT_BUFFER_SIZE];
    LONG lRet;
    DWORD dwType;

    strcpy(szKey, MISC_REGISTRY_KEY);
    hKey = NULL;
    lRet = RegCreateKeyA(HKEY_LOCAL_MACHINE, szKey, &hKey);
    if (lRet == 0) {
        dwcbData = REGISTRY_DWORD_BYTES;
        if (RegQueryValueExA(
                hKey,
                "HMM2POL MusicVolume",
                NULL,
                &dwType,
                reinterpret_cast<u8*>(&gConfig.musicVolume),
                &dwcbData
            )
            != 0) {
            memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
            SetInstallDefaults();
            SetGameDefaults();
            RegCloseKey(hKey);
            WritePrefs();
            return;
        }
        RegQueryValueExA(
            hKey,
            "HMM2POL MusicVolume",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.musicVolume),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL FXVolume",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.soundVolume),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL WalkSpeed",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.walkSpeed),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL ComputerWalkSpeed",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.computerWalkSpeed),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL ShowRoute",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.showRoute),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL BlackoutComputer",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.blackoutComputer),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL SoundQuality",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.musicSource),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL UseOpera",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.useOpera),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL DirectConnectComPort",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.comPort[IDX(CONFIG_CONNECTION_DIRECT)]),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL DirectConnectBaudRate",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.baudRate[IDX(CONFIG_CONNECTION_DIRECT)]),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL ModemComPort",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.comPort[IDX(CONFIG_CONNECTION_MODEM)]),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL ModemBaudRate",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.baudRate[IDX(CONFIG_CONNECTION_MODEM)]),
            &dwcbData
        );
        dwcbData = MODEM_INIT_STRING_SIZE + 1;
        RegQueryValueExA(
            hKey,
            "HMM2POL ModemInitString",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(gConfig.modemInitString),
            &dwcbData
        );
        dwcbData = REGISTRY_DWORD_BYTES;
        RegQueryValueExA(
            hKey,
            "HMM2POL UniqueSystemID",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(gConfig.uniqueSystemID),
            &dwcbData
        );
        gConfig.uniqueSystemID[UNIQUE_ID_TERMINATOR_INDEX] = 0;
        dwcbData = NETWORK_DEFAULT_NAME_SIZE + 1;
        RegQueryValueExA(
            hKey,
            "HMM2POL NetName",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(gConfig.networkDefaultName),
            &dwcbData
        );
        dwcbData = REGISTRY_DWORD_BYTES;
        RegQueryValueExA(
            hKey,
            "HMM2POL UseAutosave",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.autosave),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL SlowVideo",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.slowVideo),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL CombatShowGrid",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.showCombatGrid),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL CombatShowMouseHex",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.showCombatMouseHex),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL CombatGridLevel",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.combatShadeLevel),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL CombatViewArmyLevel",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.combatArmyInfoLevel),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EvilInterfaceUsage",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.evilInterfaceUsage),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL AutoCombat",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.quickCombatLevel),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL CombatSpeed",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.combatSpeed),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL AutoCombatSpells",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.autoCombatUseSpells),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL FirstMapOffset",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.firstMapOffset),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL CurrentMapOffset",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.currentMapOffset),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL ShowObjectBoxes",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.showObjectBoxes),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorAnimateScreen",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.editorScreenAnimation),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorPaletteCycling",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.editorPaletteCycling),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameShowMenu",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].showMenu),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameWindowXLeft",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].x),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameWindowYTop",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].y),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameWindowWidth",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].width),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameWindowHeight",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].height),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameFullScreen",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].fullScreen),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameColorMouseCursor",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].colorMouseCursor),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorShowMenu",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].showMenu),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorWindowXLeft",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].x),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorWindowYTop",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].y),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorWindowWidth",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].width),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorWindowHeight",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].height),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorFullScreen",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].fullScreen),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorColorMouseCursor",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].colorMouseCursor),
            &dwcbData
        );
        dwcbData = MODEM_INIT_STRING_SIZE + 1;
        if (RegQueryValueExA(
                hKey,
                "PathPL2",
                NULL,
                &dwType,
                reinterpret_cast<u8*>(gcRegAppPath),
                &dwcbData
            )
            != 0)
            strcpy(
                gcRegAppPath,
                ""
            );
        if (RegQueryValueExA(
                hKey,
                "HMM2POL CDDrive",
                NULL,
                &dwType,
                reinterpret_cast<u8*>(gcRegCDRomPath),
                &dwcbData
            )
            != 0)
            strcpy(
                gcRegCDRomPath,
                ""
            );
        RegCloseKey(hKey);
        if (CURRENT_GRAPHICS_CONFIG.width <= 0)
            CURRENT_GRAPHICS_CONFIG.width = MINIMUM_WINDOW_WIDTH;
        if (CURRENT_GRAPHICS_CONFIG.height <= 0)
            CURRENT_GRAPHICS_CONFIG.height = MINIMUM_WINDOW_HEIGHT;
        if (CURRENT_GRAPHICS_CONFIG.x < 0)
            CURRENT_GRAPHICS_CONFIG.x = 0;
        if (CURRENT_GRAPHICS_CONFIG.x > giMainVideoModeHeight - WINDOW_POSITION_MARGIN)
            CURRENT_GRAPHICS_CONFIG.x = giMainVideoModeHeight - WINDOW_POSITION_MARGIN;
        if (CURRENT_GRAPHICS_CONFIG.y < 0)
            CURRENT_GRAPHICS_CONFIG.y = 0;
        if (CURRENT_GRAPHICS_CONFIG.y > giMainVideoModeWidth - WINDOW_POSITION_MARGIN)
            CURRENT_GRAPHICS_CONFIG.y = giMainVideoModeWidth - WINDOW_POSITION_MARGIN;
    }
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

#if H2_RETAIL_COMPILER
#define fileDescriptor fd
#endif
VA(0x004bed60, 0x63)
void WritePrefsToFile(void) {
    i32 fileDescriptor;

    sprintf(gText, "%s", "HEROES2.CFG");
    fileDescriptor = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fileDescriptor == -1)
        return;
    write(fileDescriptor, &gConfig, CONFIG_PERSISTED_SIZE);
    close(fileDescriptor);
}
#if H2_RETAIL_COMPILER
#undef fileDescriptor
#endif

VA(0x004bedd0, 0x4cb)
void WritePrefsToRegistry(void) {
    HKEY hKey;
    char szKey[REGISTRY_TEXT_BUFFER_SIZE];
    LONG lRet;

    strcpy(szKey, MISC_REGISTRY_KEY);
    hKey = NULL;
    lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, szKey, 0, KEY_ALL_ACCESS, &hKey);
    if (lRet == 0) {
        RegSetValueExA(
            hKey,
            "HMM2POL MusicVolume",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.musicVolume),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL FXVolume",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.soundVolume),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL WalkSpeed",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.walkSpeed),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL ComputerWalkSpeed",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.computerWalkSpeed),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL ShowRoute",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.showRoute),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL BlackoutComputer",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.blackoutComputer),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL SoundQuality",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.musicSource),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL UseOpera",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.useOpera),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL DirectConnectComPort",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.comPort[IDX(CONFIG_CONNECTION_DIRECT)]),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL DirectConnectBaudRate",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.baudRate[IDX(CONFIG_CONNECTION_DIRECT)]),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL ModemComPort",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.comPort[IDX(CONFIG_CONNECTION_MODEM)]),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL ModemBaudRate",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.baudRate[IDX(CONFIG_CONNECTION_MODEM)]),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL ModemInitString",
            0,
            REG_SZ,
            reinterpret_cast<u8*>(gConfig.modemInitString),
            MODEM_INIT_STRING_SIZE
        );
        RegSetValueExA(
            hKey,
            "HMM2POL UniqueSystemID",
            0,
            REG_SZ,
            reinterpret_cast<u8*>(gConfig.uniqueSystemID),
            UNIQUE_SYSTEM_ID_SIZE
        );
        RegSetValueExA(
            hKey,
            "HMM2POL NetName",
            0,
            REG_SZ,
            reinterpret_cast<u8*>(gConfig.networkDefaultName),
            NETWORK_DEFAULT_NAME_SIZE
        );
        RegSetValueExA(
            hKey,
            "HMM2POL UseAutosave",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.autosave),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL SlowVideo",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.slowVideo),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL CombatShowGrid",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.showCombatGrid),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL CombatShowMouseHex",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.showCombatMouseHex),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL CombatGridLevel",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.combatShadeLevel),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL CombatViewArmyLevel",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.combatArmyInfoLevel),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EvilInterfaceUsage",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.evilInterfaceUsage),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL AutoCombat",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.quickCombatLevel),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL CombatSpeed",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.combatSpeed),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL AutoCombatSpells",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.autoCombatUseSpells),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL FirstMapOffset",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.firstMapOffset),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL CurrentMapOffset",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.currentMapOffset),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL ShowObjectBoxes",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.showObjectBoxes),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorAnimateScreen",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.editorScreenAnimation),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorPaletteCycling",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.editorPaletteCycling),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameShowMenu",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].showMenu),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameWindowXLeft",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].x),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameWindowYTop",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].y),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameWindowWidth",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].width),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameWindowHeight",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].height),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameFullScreen",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].fullScreen),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameColorMouseCursor",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].colorMouseCursor),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorShowMenu",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].showMenu),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorWindowXLeft",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].x),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorWindowYTop",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].y),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorWindowWidth",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].width),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorWindowHeight",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].height),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorFullScreen",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].fullScreen),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorColorMouseCursor",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[IDX(CONFIG_EXECUTABLE_EDITOR)].colorMouseCursor),
            REGISTRY_DWORD_BYTES
        );
        RegCloseKey(hKey);
    }
}

VA(0x004bf2a0, 0xf)
void WritePrefs(void) {
    UpdateSystemOptionsMenu();
    WritePrefsToRegistry();
}

VA(0x004bf2b0, 0x3f)
i32 IsCDDrive(i32 driveIndex) {
    sprintf(gText, "A:\\");
    gText[0] += driveIndex;
    return GetDriveTypeA(gText) == DRIVE_CDROM;
}
