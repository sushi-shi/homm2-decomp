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

DATA(0x00536088) static i32 giFindMid = 0;
DATA(0x0053608c) H2_ENUM_STORAGE_STEPPED(DataEntryPhase, i32) bDataEntryTime =
    ENTRY_PHASE_IMMEDIATE;
DATA(0x00536090) i32 inBoxY = 0;
DATA(0x00536094) i32 inBoxX = 0;
DATA(0x00536098) static i32 gBlitBottom = 0;
DATA(0x0053609c) static i32 gBlitRight = 0;
DATA(0x005360a0) class heroWindow* DataEntryWin = NULL;
DATA(0x005360a4) char* cDEDest = NULL;
DATA(0x005360a8) i32 iDEMaxLen = 0;
DATA(0x005360ac) i32 iMemEntries = 0;
DATA(0x005360b0) MemEntry* gpMemEntry = NULL;
DATA(0x005360b4) i32 giTotalMemAllocated = 0;
DATA(0x0051e5dc) static H2_CONST char* gcCDTrackName =
    "\\Tracks2\\02-AudioTrack 02.ogg";
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
    LogInt(gMemEntryTag, iMemEntries, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE,
           LOG_UNUSED_VALUE, LOG_UNUSED_VALUE);
    gpMemEntry = static_cast<MemEntry*>(malloc(MEMORY_ENTRY_CAPACITY * sizeof(MemEntry)));
    for (i32 i = 0; i < MEMORY_ENTRY_CAPACITY; ++i)
        gpMemEntry[i].used = 0;
}

VA(0x004bd530, 0x113)
void* BaseAlloc(u32 size, H2_CONST char* originalFile, i32 originalLine) {
    if (size == 0)
        return NULL;
    if (gpMemEntry == NULL)
        InitMemEntry();
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
    return ptr;
}

VA(0x004bd650, 0x154)
void BaseFree(void* ptr, H2_CONST char* originalFile, i32 originalLine) {
    if (gpMemEntry == NULL)
        InitMemEntry();
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
        LogStr("NULL POINTER");
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
            reinterpret_cast<i32>(ptr)
        );
        LogStr(gText);
    } else {
        free(ptr);
        ptr = NULL;
    }
}

VA(0x004bd7b0, 0xe7)
void PrintMemoryLeaks(void) {
    if (giDebugLevel < MEMORY_LEAK_DEBUG_LEVEL)
        return;
    if (gpMemEntry == NULL)
        return;
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

VA(0x004bd8e0, 0x10a)
u32l MAKEFILEID(H2_CONST char* text) {
    u32 fileId;
    i32 size;
    char buf[GLOBAL_AGGREGATE_PATH_SIZE];
    i32 total;
    i32 i;

    strcpy(buf, text);
    fileId = 0;
    total = 0;
    size = strlen(buf);
    for (i = size - 1; i >= 0; --i) {
        if (buf[i] >= 'a' && buf[i] <= 'z')
            buf[i] &= ~('a' - 'A');
        fileId = (fileId << HASH_LEFT_SHIFT) + (fileId >> HASH_RIGHT_SHIFT);
        total += buf[i];
        fileId += buf[i] + total;
    }
    return fileId;
}

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

VA(0x004bdae0, 0x1af)
void FadeIn(i32 increment) {
    b32 done;
    i32 i, j, delayTime, threshold;
    palette* pal = new palette;
    if (pal == NULL)
        MemError();
    done = false;
    if (gConfig.gfx[IDX(giCurExe)].fullScreen == 0)
        increment *= WINDOWED_FADE_INCREMENT_SCALE;
    memset(pal->m_data, 0, MISC_PALETTE_BYTE_COUNT);
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
                    pal->m_data[j] = gpBufferPalette->m_data[j] - threshold;
            }
            UpdatePalette(pal->m_data);
        }
        DelayTil(&delayTime);
    }
    if (done == 0) {
        i = MISC_PALETTE_MAX_LEVEL;
        goto fadeStep;
    }
    delete pal;
}

VA(0x004bdc90, 0x1b2)
void FadeOut(i32 increment) {
    b32 done;
    i32 i, j, delayTime;
    palette* pal = new palette;
    if (pal == NULL)
        MemError();
    done = false;
    if (gConfig.gfx[IDX(giCurExe)].fullScreen == 0)
        increment *= WINDOWED_FADE_INCREMENT_SCALE;
    memcpy(pal->m_data, gpBufferPalette->m_data, MISC_PALETTE_BYTE_COUNT);
    for (i = 0; i < FADE_LEVEL_COUNT; i += increment) {
    fadeStep:
        delayTime = KBTickCount() + FADE_FRAME_DELAY;
        PollSound();
        if (i == FADE_LEVEL_LAST)
            done = true;
        for (j = 0; j < PALETTE_DATA_SIZE; ++j) {
            if (pal->m_data[j] > 0) {
                if (pal->m_data[j] > increment)
                    pal->m_data[j] -= increment;
                else
                    pal->m_data[j] = 0;
            }
        }
        UpdatePalette(pal->m_data);
        DelayTil(&delayTime);
    }
    if (done == 0) {
        i = FADE_LEVEL_LAST;
        goto fadeStep;
    }
    delete pal;
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

VA(0x004bdf10, 0x75)
char* FindStringInString(char* text, H2_CONST char* pattern) {
    i32 iLen = strlen(text);
    i32 patternLen = strlen(pattern);
    for (i32 i = 0; i < iLen - patternLen + 1; ++i) {
        if (strncmp(text + i, pattern, patternLen) == 0)
            return text + i;
    }
    return NULL;
}

VA(0x004bdf90, 0x56)
char* FindToken(char* text, char token) {
    i32 iLen = strlen(text);
    for (i32 i = 0; i < iLen; ++i) {
        if (*(text + i) == token)
            return text + i;
    }
    return NULL;
}

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

VA(0x004bdff0, 0x56)
char* FindLastToken(char* text, char token) {
    i32 iLen = strlen(text);
    for (i32 i = iLen - 1; i >= 0; --i) {
        if (*(text + i) == token)
            return text + i;
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
        "\xcd\xe5\xe8\xe7\xe2\xe5\xf1\xf2\xed\xfb\xe9 \xe3\xe5\xf0\xee\xe9"
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
    i32 fd;

    sprintf(gText, "%s", "HEROES2.CFG");
    fd = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fd == -1)
        return;
    write(fd, &gConfig, CONFIG_PERSISTED_SIZE);
    close(fd);
}

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

VA(0x004bf2f0, 0x7b)
bool DriveSupportsFreeSpaceQuery(char driveLetter) {
    UINT oldMode;
    char szPath[CD_DRIVE_QUERY_PATH_SIZE];
    ULARGE_INTEGER availToCaller;
    ULARGE_INTEGER total;
    ULARGE_INTEGER freeBytes;

    wsprintfA(szPath, "%c:", driveLetter);
    oldMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    if (GetDiskFreeSpaceExA(szPath, &availToCaller, &total, &freeBytes) != 0) {
        SetErrorMode(oldMode);
        return true;
    } else {
        SetErrorMode(oldMode);
        return false;
    }
}

VA(0x004bf370, 0x35f)
H2_ENUM_RETURN(CDRomSetupResult, i32) SetupCDDrive(void) {
    u32l H2_UNUSED(dwErr);
    u32l drives;
    i32 i;
    i32 handle;
    i32 j;
    i32 cdrom[CD_DRIVE_SLOT_COUNT];
    char buffer[CD_READ_BUFFER_SIZE];
    i32 status;
    i32 num;
    HKEY hKey;
    char szKey[CD_PATH_BUFFER_SIZE];

    sprintf(gText, "%sHEROES2x.AGG", ".\\DATA\\");
    handle = open(gText, _O_BINARY);
    if (handle == -1) {
        if (_chdir(gcRegAppPath) == -1)
            return CD_ROM_GAME_DIRECTORY_MISSING;
        handle = open(gText, _O_BINARY);
        if (handle == -1)
            return CD_ROM_DATA_FILES_MISSING;
    }
    close(handle);

    drives = GetLogicalDrives();
    memset(cdrom, 0, CD_DRIVE_SLOT_COUNT);
    for (i = CD_FIRST_DRIVE_INDEX, j = 0; i < CD_DRIVE_SLOT_COUNT; ++i) {
        if (drives & (1 << i)) {
            if (IsCDDrive(i)) {
                cdrom[j] = i;
                ++j;
            }
        }
    }
    num = j;

    if (strlen(gcRegCDRomPath) > 0 && gcRegCDRomPath[0] >= 'A' && gcRegCDRomPath[0] <= 'Z'
        && DriveSupportsFreeSpaceQuery(gcRegCDRomPath[0])) {
        sprintf(gText, "%s%s", gcRegCDRomPath, gcCDTrackName);
        handle = open(gText, _O_BINARY);
        if (handle != -1) {
            close(handle);
            return CD_ROM_READY;
        }
    }
    if (num <= 0)
        return CD_ROM_DRIVE_UNAVAILABLE;

    for (i = 0; i < CD_RETRY_LIMIT; ++i) {
        for (j = 0; j < num; ++j) {
            if (DriveSupportsFreeSpaceQuery(cdrom[j] + 'A')) {
                sprintf(gText, "%c:%s", cdrom[j] + 'A', gcCDTrackName);
                handle = open(gText, _O_BINARY);
                if (handle == -1)
                    continue;
                status = _lseek(handle, 0, SEEK_END);
                if (status != -1) {
                    status = _lseek(handle, -CD_PROBE_TRAILER_SIZE, SEEK_CUR);
                    if (status != -1)
                        status = read(handle, buffer, CD_PROBE_TRAILER_SIZE);
                }
                close(handle);
                if (status != -1) {
                    sprintf(gcRegCDRomPath, "%c:", cdrom[j] + 'A');
                    strcpy(szKey, MISC_REGISTRY_KEY);
                    hKey = NULL;
                    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, szKey, 0, KEY_WRITE, &hKey) == 0) {
                        RegSetValueExA(
                            hKey,
                            "HMM2POL CDDrive",
                            0,
                            REG_SZ,
                            reinterpret_cast<u8*>(gcRegCDRomPath),
                            strlen(gcRegCDRomPath) + 1
                        );
                        RegCloseKey(hKey);
                    }
                    return CD_ROM_READY;
                }
            }
        }
        Sleep(CD_RETRY_DELAY_MILLISECONDS);
    }
    return CD_ROM_EXPANSION_DISC_MISSING;
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
    if (gbColorMice == 0) {
        BlitBitmapToScreenVesa(bmp, sourceX, sourceY, width, height, destinationX, destinationY);
        return;
    }
    if (giScrollX != 0 || giScrollY != 0) {
        sourceX = giScrollX + BLIT_SCROLL_OFFSET;
        width = BLIT_SCROLL_EXTENT;
        sourceY = giScrollY + BLIT_SCROLL_OFFSET;
        height = BLIT_SCROLL_EXTENT;
    }
    gBlitRight = destinationX + width - 1;
    gBlitBottom = destinationY + height - 1;
    if (gpMouseManager->IsVis() == 0 || gBlitRight < gpMouseManager->m_savedLeft
        || destinationX > gpMouseManager->m_cursorRight
        || gBlitBottom < gpMouseManager->m_savedTop
        || destinationY > gpMouseManager->m_cursorBottom) {
        BlitBitmapToScreenVesa(bmp, sourceX, sourceY, width, height, destinationX, destinationY);
    } else {
        gpMouseManager->SaveAndDraw();
        BlitBitmapToScreenVesa(bmp, sourceX, sourceY, width, height, destinationX, destinationY);
        if (gBlitRight < gpMouseManager->m_cursorRight
            || destinationX > gpMouseManager->m_savedLeft
            || gBlitBottom < gpMouseManager->m_cursorBottom
            || destinationY > gpMouseManager->m_savedTop) {
            BlitBitmapToScreenVesa(
                bmp,
                gpMouseManager->m_savedLeft,
                gpMouseManager->m_savedTop,
                gpMouseManager->m_cursorRight - gpMouseManager->m_savedLeft + 1,
                gpMouseManager->m_cursorBottom - gpMouseManager->m_savedTop + 1,
                gpMouseManager->m_savedLeft,
                gpMouseManager->m_savedTop
            );
        }
        gpMouseManager->RestoreUnderlying();
    }
}
VA(0x004bf990, 0x91)
void LogTruncate(void) {
    char logText[TEXT_BUFFER_SIZE];
    i32 fileHandle;
    if (giDebugLevel < FILE_DEBUG_LEVEL)
        return;
    fileHandle = open("KB.LOG", _O_WRONLY | _O_CREAT | _O_TRUNC | _O_TEXT, _S_IWRITE);
    if (fileHandle == -1)
        return;
    strcpy(logText, "===========New Log==========");
    strcat(logText, "\n");
    write(fileHandle, logText, strlen(logText));
    close(fileHandle);
}


VA(0x004bfa30, 0x9a)
void LogStr(H2_CONST char* text) {
    char logText[TEXT_BUFFER_SIZE];
    FILE* out;
    if (giDebugLevel < FILE_DEBUG_LEVEL)
        return;
    out = fopen("KB.LOG", "at+");
    if (out == NULL)
        return;
    strcpy(logText, text);
    strcat(logText, "\n");
    fputs(logText, out);
    fclose(out);
    if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
        OutputDebugStringA(logText);
}

VA(0x004bfad0, 0x1b6)
void LogInt(
    H2_CONST char* label,
    i32 value1,
    i32 value2,
    i32 value3,
    i32 value4,
    i32 value5,
    i32 value6,
    i32 value7
) {
    char text[FORMAT_BUFFER_SIZE];
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
    LogStr(text);
}

VA(0x004bfc90, 0x76)
void AiPrint(H2_CONST char* text) {
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
void AbsAiPrint(H2_CONST char* text) {
    i32 saved = giDebugLevel;
    giDebugLevel = FORCED_DEBUG_LEVEL;
    AiPrint(text);
    giDebugLevel = saved;
}

VA(0x004bfd40, 0x19c)
void FadeTo(u8* source, u8* destination, i32 increment) {
    u8 temp[MISC_PALETTE_BYTE_COUNT];
    u8 *current, *to;
    i32 idx, change, diff, move, H2_UNUSED(delay), iLevel, nextTime, k;

    delay = FADE_TO_FRAME_DELAY;
    memcpy(temp, source, MISC_PALETTE_BYTE_COUNT);
    increment >>= FADE_TO_INCREMENT_SHIFT;
    if (increment < 1) {
        increment = 1;
        delay *= WINDOWED_FADE_INCREMENT_SCALE;
    }
    for (iLevel = FADE_TO_START_LEVEL; iLevel < MISC_PALETTE_LEVEL_COUNT; iLevel += increment) {
        nextTime = KBTickCount() + FADE_TO_FRAME_DELAY;
        PollSound();
        idx = MISC_PALETTE_LEVEL_COUNT - iLevel - increment;
        if (idx < 0)
            idx = 0;
        change = giChangeThreshold[idx];
        current = temp;
        to = destination;
        for (k = 0; k < MISC_PALETTE_BYTE_COUNT; ++k) {
            diff = *to - *current;
            if (abs(diff) > change) {
                move = abs(diff) - change;
                if (diff > 0)
                    *current = *current + move;
                else
                    *current = *current - move;
            }
            ++current;
            ++to;
        }
        UpdatePalette(reinterpret_cast<i8*>(temp));
        DelayTil(&nextTime);
    }
    UpdatePalette(reinterpret_cast<i8*>(destination));
}

VA(0x004bfee0, 0x163)
void FadeToColorTable(u8* colorTable, i32 increment) {
    u8* p;
    i32 x;
    i32 i;
    i32 y;
    u8 tempPal[MISC_PALETTE_BYTE_COUNT];
    i8* pal;
    i32 savedFlags;

    savedFlags = gpWindowManager->m_updateFlags;
    gpWindowManager->m_updateFlags = 0;
    pal = gpBufferPalette->m_data;
    for (i = 0; i < IDX(MISC_PALETTE_BYTE_COUNT) / IDX(PALETTE_COMPONENT_COUNT); ++i) {
        tempPal[i * PALETTE_COMPONENT_COUNT + PALETTE_RED_INDEX] =
            pal[colorTable[i] * PALETTE_COMPONENT_COUNT + PALETTE_RED_INDEX];
        tempPal[i * PALETTE_COMPONENT_COUNT + PALETTE_GREEN_INDEX] =
            pal[colorTable[i] * PALETTE_COMPONENT_COUNT + PALETTE_GREEN_INDEX];
        tempPal[i * PALETTE_COMPONENT_COUNT + PALETTE_BLUE_INDEX] =
            pal[colorTable[i] * PALETTE_COMPONENT_COUNT + PALETTE_BLUE_INDEX];
    }
    FadeTo(reinterpret_cast<u8*>(pal), tempPal, increment);
    p = gpWindowManager->m_screen->m_pixels;
    for (y = 0; y < BLIT_SCREEN_HEIGHT; ++y) {
        for (x = 0; x < BLIT_SCREEN_WIDTH; ++x) {
            *p = colorTable[*p];
            ++p;
        }
    }
    gpWindowManager->UpdateScreen();
    UpdatePalette(pal);
    gpWindowManager->m_updateFlags = savedFlags;
}

VA(0x004c0050, 0x44)
i32 IsCycleColor(i32 color) {
    return (color >= CYCLE_RANGE_ONE_FIRST && color <= CYCLE_RANGE_ONE_LAST)
        || (color >= CYCLE_RANGE_TWO_FIRST && color <= CYCLE_RANGE_TWO_LAST);
}

VA(0x004c00a0, 0x2a9)
void CreatePCXFile(char* filename, u8* pixels, i32 width, i32 height, u8* paletteData) {
    i32 fd;
    i32 iLen;
    u8 bMark;
    u8 color;
    i32 sourceIndex;
    i32 x;
    i32 y;
    i32 endPos;
    u8* rowPtr;
    u8* encodedRow;
    PCXHeader pcxHdr;
    u8* palOut;
    i32 runLength;

    memset(&pcxHdr, 0, sizeof(pcxHdr));
    pcxHdr.manufacturer = MANUFACTURER_ZSOFT;
    pcxHdr.version = VERSION_3_0;
    pcxHdr.encoding = ENCODING_RLE;
    pcxHdr.bitsPerPixel = BITS_PER_PIXEL;
    pcxHdr.xMax = static_cast<u16>(width - 1);
    pcxHdr.yMax = static_cast<u16>(height - 1);
    pcxHdr.planes = PLANE_COUNT;
    pcxHdr.bytesPerLine = static_cast<u16>(width);
    pcxHdr.paletteType = PALETTE_TYPE_COLOR;
    fd = open(filename, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fd == -1)
        return;
    write(fd, &pcxHdr, sizeof(pcxHdr));
    encodedRow = static_cast<u8*>(H2_ALLOC(width * 2));
    for (y = 0; y < height; ++y) {
        sourceIndex = 0;
        rowPtr = pixels + y * width;
        iLen = 0;
        while (sourceIndex < width) {
            color = *(rowPtr + sourceIndex);
            endPos = sourceIndex;
            while (endPos < width && *(rowPtr + endPos) == color
                   && endPos - sourceIndex + 1 < RLE_RUN_LIMIT)
                ++endPos;
            runLength = endPos - sourceIndex;
            if (runLength > 1 || (color & RLE_RUN_MARKER) == RLE_RUN_MARKER) {
                *(encodedRow + iLen) = static_cast<u8>(runLength | RLE_RUN_MARKER);
                *(encodedRow + iLen + 1) = color;
                iLen += 2;
                sourceIndex += runLength;
            } else {
                *(encodedRow + iLen) = color;
                iLen += 1;
                sourceIndex += 1;
            }
        }
        write(fd, encodedRow, iLen);
    }
    H2_FREE(encodedRow);
    bMark = VGA_PALETTE_MARKER;
    write(fd, &bMark, 1);
    palOut = static_cast<u8*>(H2_ALLOC(PALETTE_BYTE_COUNT));
    for (x = 0; x < PALETTE_BYTE_COUNT; ++x)
        *(palOut + x) = *(paletteData + x) << COMPONENT_SCALE_SHIFT;
    write(fd, palOut, PALETTE_BYTE_COUNT);
    H2_FREE(palOut);
    close(fd);
}

VA(0x004c0350, 0x73)
i32l FileSize(char* filename) {
    FILE* f;
    i32l lLen;

    f = fopen(filename, "r+b");
    if (f == NULL) {
        if (f == NULL)
            FileError(filename);
    }
    fseek(f, 0, SEEK_END);
    lLen = ftell(f);
    fseek(f, 0, SEEK_SET);
    fclose(f);
    return lLen;
}

VA(0x004c03d0, 0x1e)
struct IconEntry* GetIconEntry(class icon* iconPtr, i32 index) {
    return reinterpret_cast<struct IconEntry*>(index * sizeof(IconEntry) + iconPtr->m_data);
}
VA(0x004c03f0, 0x72)
i32 SRandom(i32 low, i32 high) {
    if (high == low) {
        return high;
    }
    if (high < low) {
        return low;
    }
    SIncRandomize(low, high);
    i32 result = SGenRand();
    iLastSeed += low;
    iLastSeed += high * RANDOM_HIGH_MIX_MULTIPLIER;
    return result % (high - low + 1) + low;
}

VA(0x004c0470, 0x92)
void SIncRandomize(i32 x, i32 y) {
    x *= RANDOM_TERM_MULTIPLIER;
    y *= RANDOM_TERM_MULTIPLIER;
    x &= RANDOM_TERM_MASK;
    y &= RANDOM_TERM_MASK;
    iLastSeed += y << RANDOM_HIGH_TERM_SHIFT;
    iLastSeed += x * RANDOM_LOW_TERM_MULTIPLIER;
    iLastSeed += y;
    i32 feedback = iLastSeed & RANDOM_FEEDBACK_MASK;
    iLastSeed += feedback << RANDOM_FEEDBACK_SHIFT;
}

VA(0x004c0510, 0x1f)
void SRand(i32 seed) {
    iLastSeed = seed;
    srand(seed);
}

VA(0x004c0530, 0x92)
i32 SGenRand(void) {
    i32 bitMask;
    i32 ret = 0;
    iLastSeed &= RANDOM_SEED_MASK;
    iLastSeed *= RANDOM_MIX_MULTIPLIER;
    iLastSeed += (iLastSeed & RANDOM_MIX_MASK) >> RANDOM_MIX_SHIFT;
    for (i32 i = RANDOM_TOP_BIT; i >= 0; --i) {
        bitMask = 1 << i;
        if (iLastSeed & bitMask) {
            ret |= 1 << i;
        }
    }
    return ret;
}

VA(0x004c05d0, 0x10)
i32 MemSize(i32) {
    return REPORTED_MEMORY_KILOBYTES;
}
VA(0x004c05e0, 0x464)
void GetDataEntry(
    H2_CONST char* prompt,
    char* destination,
    i32 maximumLength,
    H2_CONST char* initialText,
    i32 showCancel,
    i32 useImmediateHandler
) {
    MouseCursorType savedCursorType;
    i16 H2_UNUSED(wId);
    i32 nRows;
    char windowName[WINDOW_NAME_CAPACITY];
    i32 entryY;
    i32 nHeight;
    i32 textLines;
    char cBuf[TEXT_BUFFER_CAPACITY];
    textEntryWidget* pText;
    tag_message msg;
    i32 nFrame;

    wId = ENTRY_TEXT_WIDGET;
    savedCursorType = gpMouseManager->m_cursorType;
    nFrame = gpMouseManager->m_cursorFrame;
    while (gpMouseManager->m_hideCount != 0)
        gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);

    cDEDest = destination;
    iDEMaxLen = maximumLength;
    strcpy(
        cDEDest,
        ""
    );

    textLines = bigFont->LineLength(prompt, PROMPT_WIDTH);
    nHeight = textLines * PROMPT_LINE_HEIGHT;
    if (showCancel != 0)
        nHeight += CANCEL_PROMPT_HEIGHT;
    nHeight += ROW_TOP_MARGIN;
    nRows = (nHeight - ROW_ROUNDING_BIAS) / ROW_HEIGHT;
    if (nRows > MAX_ROW_COUNT)
        nRows = MAX_ROW_COUNT;
    entryY = (nRows + 1) * ROW_HEIGHT + ENTRY_BASE_Y - (showCancel != 0 ? CANCEL_Y_OFFSET : 0);

    sprintf(windowName, "evntwin%d.bin", nRows);
    DataEntryWin = new heroWindow(WINDOW_X, WINDOW_Y, windowName);
    if (DataEntryWin == NULL)
        MemError();

    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    msg.payload.widget.id = ENTRY_PROMPT_WIDGET;
    msg.payload.widget.data.text = prompt;
    DataEntryWin->BroadcastMessage(msg);

    if (initialText != NULL)
        strcpy(cBuf, initialText);
    else
        strcpy(
            cBuf,
            ""
        );
    msg.payload.widget.id = ENTRY_TEXT_WIDGET;
    msg.payload.widget.data.text = cBuf;
    DataEntryWin->BroadcastMessage(msg);
    strcpy(destination, cBuf);

    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    msg.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
    msg.payload.widget.id = ENTRY_BUTTON_ONE;
    DataEntryWin->BroadcastMessage(msg);
    msg.payload.widget.id = ENTRY_BUTTON_SEVEN;
    DataEntryWin->BroadcastMessage(msg);
    msg.payload.widget.id = ENTRY_BUTTON_EIGHT;
    DataEntryWin->BroadcastMessage(msg);
    msg.payload.widget.id = ENTRY_BUTTON_FIVE;
    DataEntryWin->BroadcastMessage(msg);
    msg.payload.widget.id = ENTRY_BUTTON_SIX;
    DataEntryWin->BroadcastMessage(msg);
    if (showCancel == 0) {
        msg.payload.widget.id = ENTRY_CANCEL_BUTTON;
        DataEntryWin->BroadcastMessage(msg);
    }

    pText = new textEntryWidget(
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
    if (pText == NULL)
        MemError();
    inBoxX = INPUT_BOX_X;
    inBoxY = entryY + INPUT_BOX_Y_OFFSET;
    DataEntryWin->AddWidget(pText, WIDGET_Z_ORDER);

    if (useImmediateHandler != 0) {
        bDataEntryTime = ENTRY_PHASE_IMMEDIATE;
        gbAllowTextEntryEscape = false;
    } else
        bDataEntryTime = ENTRY_PHASE_READY;
    gpWindowManager->DoDialog(DataEntryWin, DataEntryWindowHandler, 0);
    delete DataEntryWin;
    gpMouseManager->SetPointer(
        "",
        nFrame,
        savedCursorType
    );
    gbAllowTextEntryEscape = true;
}

VA(0x004c0a50, 0x1fa)
MessageDispatchResult DataEntryWindowHandler(struct tag_message& message) {
    i16 H2_UNUSED(wId);

    wId = ENTRY_TEXT_WIDGET;
    if (bDataEntryTime == ENTRY_PHASE_IMMEDIATE) {
        ++bDataEntryTime;
        message.type = MESSAGE_LEFT_BUTTON_DOWN;
        message.payload.mouse.x = inBoxX;
        message.payload.mouse.y = inBoxY;
        DataEntryWin->BroadcastMessage(message);
        return MESSAGE_DISPATCH_CONSUME;
    }

    if (bDataEntryTime == ENTRY_PHASE_POINTER_SENT) {
        ++bDataEntryTime;
        goto gotText;
    }
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case ENTRY_CANCEL_BUTTON:
                        message.payload.widget.id = ENTRY_TEXT_WIDGET;
                        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
                        return MESSAGE_DISPATCH_FORWARD;
                }
                break;
            case WIDGET_COMMAND_SELECT:
                switch (message.payload.widget.id) {
                    case ENTRY_TEXT_WIDGET:
                    gotText:
                        message.type = MESSAGE_WIDGET;
                        message.payload.widget.id = ENTRY_TEXT_WIDGET;
                        message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                        DataEntryWin->BroadcastMessage(message);
                        if (strlen(message.payload.widget.data.text) == 0)
                            break;
                        memset(cDEDest, 0, iDEMaxLen);
                        strncpy(cDEDest, message.payload.widget.data.text, iDEMaxLen - 1);
                        message.type = MESSAGE_WIDGET;
                        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                        message.payload.widget.id = ENTRY_TEXT_WIDGET;
                        message.payload.widget.data.text = cDEDest;
                        DataEntryWin->BroadcastMessage(message);
                        DataEntryWin->DrawWindow(DRAW_MODE, REDRAW_OFFSET, REDRAW_OFFSET);
                        if (gbTextEntryEscaped != 0)
                            break;
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        message.payload.widget.id = ENTRY_TEXT_WIDGET;
                        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
                        return MESSAGE_DISPATCH_FORWARD;
                }
        }
    }
    return EventWindowHandler(message);
}
