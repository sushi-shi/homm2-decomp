#define HOMM2_MISC_INLINE_ICONENTRY
#include <Ints.h>
#include <BASE/dialog.h>
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
typedef enum DataEntryLayout {
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
} DataEntryLayout;

typedef enum DataEntryWidgetId {
    ENTRY_CANCEL_BUTTON = DIALOG_BUTTON_2,
    ENTRY_PROMPT_WIDGET = 1,
    ENTRY_TEXT_WIDGET   = 10,
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
    DEFAULT_SLOW_VIDEO           = 3,
    DEFAULT_MAP_OFFSET_MAX       = 32000,
    UNIQUE_ID_RANDOM_MAX         = 999999,
    UNIQUE_ID_ALPHANUMERIC_COUNT = 36,
    UNIQUE_ID_ALPHA_COUNT        = 26,
    UNIQUE_ID_LEADING_INDEX      = 0,
    UNIQUE_ID_MIDDLE_INDEX       = 1,
    UNIQUE_ID_TRAILING_INDEX     = 2,
    UNIQUE_ID_TERMINATOR_INDEX   = 3
} MiscGameDefaultConstant;

typedef enum MiscCDDriveConstant {
    CD_FIRST_DRIVE_INDEX        = 2,
    CD_DRIVE_SLOT_COUNT         = 26,
    CD_PATH_BUFFER_SIZE         = 100,
    CD_DRIVE_QUERY_PATH_SIZE    = 256,
    CD_READ_BUFFER_SIZE         = 256,
    CD_PROBE_TRAILER_SIZE       = 100,
    CD_RETRY_DELAY_MILLISECONDS = 3000,
    CD_RETRY_LIMIT              = 2
} MiscCDDriveConstant;

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
    COMPONENT_SCALE_SHIFT = 2
} PCXConstant;

typedef enum MiscCycleColorRange {
    CYCLE_RANGE_ONE_FIRST = 0xd6,
    CYCLE_RANGE_ONE_LAST  = 0xdd,
    CYCLE_RANGE_TWO_FIRST = 0xe7,
    CYCLE_RANGE_TWO_LAST  = 0xed
} MiscCycleColorRange;

typedef enum MiscFadeConstant {
    FADE_CHANGE_THRESHOLD_COUNT   = 16,
    FADE_FRAME_DELAY              = 0x14,
    WINDOWED_FADE_INCREMENT_SCALE = 2,
    FADE_TO_INCREMENT_SHIFT       = 2,
    FADE_TO_START_LEVEL           = 0x30,
    FADE_TO_FRAME_DELAY           = 0x32
} MiscFadeConstant;

typedef enum MiscWindowConstant {
    MINIMUM_WINDOW_WIDTH   = 320,
    MINIMUM_WINDOW_HEIGHT  = 240,
    WINDOW_POSITION_MARGIN = 200
} MiscWindowConstant;

typedef enum MiscBlitConstant {
    BLIT_SCROLL_OFFSET = 0x10,
    BLIT_SCROLL_EXTENT = 0x1c0,
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

static i32 giFindMid = 0;
i32 bDataEntryTime =
    ENTRY_PHASE_IMMEDIATE;
i32 inBoxY = 0;
i32 inBoxX = 0;
i32 gBlitBottom = 0;
i32 gBlitRight = 0;
class heroWindow* DataEntryWin = NULL;
char* cDEDest = NULL;
i32 iDEMaxLen = 0;
i32 iMemEntries = 0;
MemEntry* gpMemEntry = NULL;
i32 giTotalMemAllocated = 0;
const char* gcCDTrackName = gcCDTrackNameText;
u8
    giChangeThreshold[FADE_CHANGE_THRESHOLD_COUNT] =
        {0, 1, 2, 3, 4, 6, 8, 10, 13, 16, 19, 22, 26, 31, 37, 46};
i32 iLastSeed = INITIAL_SEED;
static char gMemEntryTag[sizeof("IME")] = "IME";

typedef enum StatusBarLayout {
    STATUS_BAR_Y       = 460,
    STATUS_BAR_HEIGHT  = 20,
    STATUS_TEXT_Y      = 464,
    STATUS_TEXT_HEIGHT = 16
} StatusBarLayout;

void InitMemEntry(void) {
    LogInt(gMemEntryTag, iMemEntries);
    gpMemEntry = static_cast<MemEntry*>(malloc(MEMORY_ENTRY_CAPACITY * sizeof(MemEntry)));
    for (i32 i = 0; i < MEMORY_ENTRY_CAPACITY; ++i)
        gpMemEntry[i].used = 0;
}

void* BaseAlloc(u32 size, const char* originalFile, i32 originalLine) {
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

void BaseFree(void* pointer, const char* originalFile, i32 originalLine) {
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

void ShowMemoryStatus(void) {
    i32 memLeft = MemSize(1);
    sprintf(gText, "Mem Left %dK", memLeft);
    AbsAiPrint(gText);
}

u32l MAKEFILEID(const char* text) {
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
#include <BASE/display.h>

void FadeIn(i32 increment) {
    b32 done;
    i32 i, j, delayTime, threshold;
    palette* currentPalette = new palette;
    if (currentPalette == NULL)
        MemError();
    done = false;
    if (CURRENT_GRAPHICS_CONFIG.fullScreen == 0)
        increment *= WINDOWED_FADE_INCREMENT_SCALE;
    memset(currentPalette->m_data, 0, PALETTE_DATA_SIZE);
    for (i = 0; i < PALETTE_LEVEL_COUNT; i += increment) {
    fadeStep:
        delayTime = KBTickCount() + FADE_FRAME_DELAY;
        PollSound();
        if (i == PALETTE_CHANNEL_MAX) {
            done = true;
            UpdatePalette(gpBufferPalette->m_data);
        } else {
            threshold = PALETTE_CHANNEL_MAX - i;
            for (j = 0; j < PALETTE_DATA_SIZE; ++j) {
                if (gpBufferPalette->m_data[j] > threshold)
                    currentPalette->m_data[j] = gpBufferPalette->m_data[j] - threshold;
            }
            UpdatePalette(currentPalette->m_data);
        }
        DelayTil(&delayTime);
    }
    if (done == 0) {
        i = PALETTE_CHANNEL_MAX;
        goto fadeStep;
    }
    delete currentPalette;
}

void FadeOut(i32 increment) {
    b32 done;
    i32 i, j, delayTime;
    palette* currentPalette = new palette;
    if (currentPalette == NULL)
        MemError();
    done = false;
    if (CURRENT_GRAPHICS_CONFIG.fullScreen == 0)
        increment *= WINDOWED_FADE_INCREMENT_SCALE;
    memcpy(currentPalette->m_data, gpBufferPalette->m_data, PALETTE_DATA_SIZE);
    for (i = 0; i < PALETTE_LEVEL_COUNT; i += increment) {
    fadeStep:
        delayTime = KBTickCount() + FADE_FRAME_DELAY;
        PollSound();
        if (i == PALETTE_CHANNEL_MAX)
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
        i = PALETTE_CHANNEL_MAX;
        goto fadeStep;
    }
    delete currentPalette;
}

i32 Random(i32 low, i32 high) {
    if (high == low) {
        return high;
    }
    if (high < low) {
        return low;
    }
    return rand() % (high - low + 1) + low;
}

void ProcessAssert(i32 condition, const char* file, i32 line) {
    i32 unusedAssertWord [[maybe_unused]];
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

char* FindStringInString(char* text, const char* pattern) {
    i32 length = strlen(text);
    i32 patternLength = strlen(pattern);
    for (i32 i = 0; i < length - patternLength + 1; ++i) {
        if (strncmp(text + i, pattern, patternLength) == 0)
            return text + i;
    }
    return NULL;
}

char* FindToken(char* text, char token) {
    i32 length = strlen(text);
    for (i32 i = 0; i < length; ++i) {
        if (*(text + i) == token)
            return text + i;
    }
    return NULL;
}

const char* FindToken(const char* text, char token) {
    i32 iLen = strlen(text);
    for (i32 i = 0; i < iLen; ++i) {
        if (*(text + i) == token)
            return text + i;
    }
    return NULL;
}

char* FindLastToken(char* text, char token) {
    i32 length = strlen(text);
    for (i32 i = length - 1; i >= 0; --i) {
        if (*(text + i) == token)
            return text + i;
    }
    return NULL;
}

void SetInstallDefaults(void) {
    memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
    strcpy(gConfig.autoLoadName, "AUTO");
    strcpy(gConfig.autoSaveName, "AUTO");
    gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
}
void SetGameDefaults(void) {
    i32 i;
    i32 seed;
    i32 nAlpha [[maybe_unused]];
    const char* alpha;

    gConfig.musicVolume = CONFIG_VOLUME_MIN;
    gConfig.soundVolume = CONFIG_VOLUME_MIN;
    gConfig.autosave = 1;
    gConfig.showRoute = 1;
    gConfig.blackoutComputer = false;
    for (i = (CONFIG_EXECUTABLE_GAME); i < (CONFIG_EXECUTABLE_COUNT); ++i) {
        gConfig.gfx[i].showMenu = 1;
        gConfig.gfx[i].x = DEFAULT_WINDOW_ORIGIN;
        gConfig.gfx[i].y = DEFAULT_WINDOW_ORIGIN;
        gConfig.gfx[i].colorMouseCursor = false;
        gConfig.gfx[i].fullScreen = true;
        if (giMainVideoModeWidth <= LOGICAL_SCREEN_WIDTH) {
            gConfig.gfx[i].width = DEFAULT_SMALL_WINDOW_WIDTH;
            gConfig.gfx[i].height = DEFAULT_SMALL_WINDOW_HEIGHT;
        } else {
            gConfig.gfx[i].width = LOGICAL_SCREEN_WIDTH;
            gConfig.gfx[i].height = LOGICAL_SCREEN_HEIGHT;
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
    gConfig.walkSpeeds[(CONFIG_WALK_SPEED_HUMAN)] = CONFIG_WALK_SPEED_NORMAL;
    gConfig.slowVideo = DEFAULT_SLOW_VIDEO;
    gConfig.walkSpeeds[(CONFIG_WALK_SPEED_COMPUTER)] = CONFIG_WALK_SPEED_FAST;

    strcpy(
        gConfig.networkDefaultName,
        "Неизвестный герой"
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

void ReadPrefsFromFile(void) {
    i32 result [[maybe_unused]];
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

typedef enum RegistryValueSize {
    REGISTRY_TEXT_BUFFER_SIZE = 100,
    REGISTRY_DWORD_BYTES      = 4,
    MODEM_INIT_STRING_SIZE    = 0x62,
    UNIQUE_SYSTEM_ID_SIZE     = 4,
    NETWORK_DEFAULT_NAME_SIZE = 0x1e
} RegistryValueSize;

void ReadPrefsFromRegistry(void) {
    DWORD dwcbData;
    HKEY hKey;
    char szKey[REGISTRY_TEXT_BUFFER_SIZE];
    char szScratch [[maybe_unused]][REGISTRY_TEXT_BUFFER_SIZE];
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
            reinterpret_cast<u8*>(&gConfig.walkSpeeds[(CONFIG_WALK_SPEED_HUMAN)]),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL ComputerWalkSpeed",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.walkSpeeds[(CONFIG_WALK_SPEED_COMPUTER)]),
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
            reinterpret_cast<u8*>(&gConfig.comPort[(CONFIG_CONNECTION_DIRECT)]),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL DirectConnectBaudRate",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.baudRate[(CONFIG_CONNECTION_DIRECT)]),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL ModemComPort",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.comPort[(CONFIG_CONNECTION_MODEM)]),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL ModemBaudRate",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.baudRate[(CONFIG_CONNECTION_MODEM)]),
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
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].showMenu),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameWindowXLeft",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].x),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameWindowYTop",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].y),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameWindowWidth",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].width),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameWindowHeight",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].height),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameFullScreen",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].fullScreen),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL GameColorMouseCursor",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].colorMouseCursor),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorShowMenu",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].showMenu),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorWindowXLeft",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].x),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorWindowYTop",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].y),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorWindowWidth",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].width),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorWindowHeight",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].height),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorFullScreen",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].fullScreen),
            &dwcbData
        );
        RegQueryValueExA(
            hKey,
            "HMM2POL EditorColorMouseCursor",
            NULL,
            &dwType,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].colorMouseCursor),
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

void WritePrefsToFile(void) {
    i32 fileDescriptor;

    sprintf(gText, "%s", "HEROES2.CFG");
    fileDescriptor = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fileDescriptor == -1)
        return;
    write(fileDescriptor, &gConfig, CONFIG_PERSISTED_SIZE);
    close(fileDescriptor);
}

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
            reinterpret_cast<u8*>(&gConfig.walkSpeeds[(CONFIG_WALK_SPEED_HUMAN)]),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL ComputerWalkSpeed",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.walkSpeeds[(CONFIG_WALK_SPEED_COMPUTER)]),
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
            reinterpret_cast<u8*>(&gConfig.comPort[(CONFIG_CONNECTION_DIRECT)]),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL DirectConnectBaudRate",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.baudRate[(CONFIG_CONNECTION_DIRECT)]),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL ModemComPort",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.comPort[(CONFIG_CONNECTION_MODEM)]),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL ModemBaudRate",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.baudRate[(CONFIG_CONNECTION_MODEM)]),
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
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].showMenu),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameWindowXLeft",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].x),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameWindowYTop",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].y),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameWindowWidth",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].width),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameWindowHeight",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].height),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameFullScreen",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].fullScreen),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL GameColorMouseCursor",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_GAME)].colorMouseCursor),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorShowMenu",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].showMenu),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorWindowXLeft",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].x),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorWindowYTop",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].y),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorWindowWidth",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].width),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorWindowHeight",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].height),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorFullScreen",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].fullScreen),
            REGISTRY_DWORD_BYTES
        );
        RegSetValueExA(
            hKey,
            "HMM2POL EditorColorMouseCursor",
            0,
            REG_DWORD,
            reinterpret_cast<u8*>(&gConfig.gfx[(CONFIG_EXECUTABLE_EDITOR)].colorMouseCursor),
            REGISTRY_DWORD_BYTES
        );
        RegCloseKey(hKey);
    }
}

void WritePrefs(void) {
    UpdateSystemOptionsMenu();
    WritePrefsToRegistry();
}

i32 IsCDDrive(i32 driveIndex) {
    sprintf(gText, "A:\\");
    gText[0] += driveIndex;
    return GetDriveTypeA(gText) == DRIVE_CDROM;
}
