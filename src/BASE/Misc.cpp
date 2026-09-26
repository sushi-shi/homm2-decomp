#define HOMM2_MISC_INLINE_ICONENTRY
#include <Ints.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/Localization.h>
#include <BASE/Utf8.h>
#include <BASE/heroWindow.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/icon.h>
#include <BASE/bmap2.h>
#include <BASE/font.h>
#include <BASE/textEntryWidget.h>
#include <BASE/Misc.h>
#include <BASE/dialog.h>

#include <string>

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
#include <SOURCE/KB.h>
#include <SOURCE/wingraph.h>
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

static i32 giFindMid = 0;
H2SteppedEnumStorage<DataEntryPhase, i32> bDataEntryTime = ENTRY_PHASE_IMMEDIATE;
i32 inBoxY = 0;
i32 inBoxX = 0;
static i32 gBlitBottom = 0;
static i32 gBlitRight = 0;

class heroWindow* DataEntryWin = NULL;
char* cDEDest = NULL;
i32 iDEMaxLen = 0;
i32 iMemEntries = 0;
MemEntry* gpMemEntry = NULL;
i32 giTotalMemAllocated = 0;
static const char* gcCDTrackName =
    "\\Tracks2\\02-AudioTrack 02.ogg";
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
        utf8::Format(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
            "Bad Delete,  File '%13s'  Line % 4d, ptr %12p",
            originalFile,
            originalLine,
            pointer
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
            utf8::Format(
                gText, GLOBAL_TEXT_BUFFER_SIZE,
                "Memory Leak,  File '%13s'  Line % 4d, ptr %12p   size %6d",
                gpMemEntry[entryIndex].file,
                gpMemEntry[entryIndex].line,
                gpMemEntry[entryIndex].ptr,
                gpMemEntry[entryIndex].size
            );
            LogStr(gText);
        }
    }
}

void ShowMemoryStatus(void) {
    i32 memLeft = MemSize(1);
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "Mem Left %dK", memLeft);
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
    i32 i, j, delayTime, threshold;
    palette* currentPalette = new palette;
    if (currentPalette == NULL)
        MemError();
    if (CURRENT_GRAPHICS_CONFIG.fullScreen == 0)
        increment *= WINDOWED_FADE_INCREMENT_SCALE;
    memset(currentPalette->m_data, 0, PALETTE_DATA_SIZE);
    i = 0;
    while (true) {
        delayTime = platform::Ticks() + FADE_FRAME_DELAY;
        PollSound();
        if (i == PALETTE_CHANNEL_MAX) {
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
        if (i == PALETTE_CHANNEL_MAX)
            break;
        i += increment;
        if (i >= PALETTE_LEVEL_COUNT)
            i = PALETTE_CHANNEL_MAX;
    }
    delete currentPalette;
}

void FadeOut(i32 increment) {
    i32 i, j, delayTime;
    palette* currentPalette = new palette;
    if (currentPalette == NULL)
        MemError();
    if (CURRENT_GRAPHICS_CONFIG.fullScreen == 0)
        increment *= WINDOWED_FADE_INCREMENT_SCALE;
    memcpy(currentPalette->m_data, gpBufferPalette->m_data, PALETTE_DATA_SIZE);
    i = 0;
    while (true) {
        delayTime = platform::Ticks() + FADE_FRAME_DELAY;
        PollSound();
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
        if (i == PALETTE_CHANNEL_MAX)
            break;
        i += increment;
        if (i >= PALETTE_LEVEL_COUNT)
            i = PALETTE_CHANNEL_MAX;
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

    if (condition == 0) {
        gpMouseManager->SetColorMice(false);
        SetFullScreenStatus(false);
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "Assert statement failed in module %s, line %d.  Do you wish to abort the program?", file, line);
        platform::ShowMessage("Assert Failure", gText);

        ShutDown(NULL);
    }
}

char* FindStringInString(char* text, const char* pattern) {
    return const_cast<char*>(FindStringInString(static_cast<const char*>(text), pattern));
}

const char* FindStringInString(const char* text, const char* pattern) {
    i32 length = strlen(text);
    i32 patternLength = strlen(pattern);
    for (i32 i = 0; i < length - patternLength + 1; ++i) {
        if (strncmp(text + i, pattern, patternLength) == 0)
            return text + i;
    }
    return NULL;
}

char* FindToken(char* text, char token) {
    return const_cast<char*>(FindToken(static_cast<const char*>(text), token));
}

const char* FindToken(const char* text, char token) {
    i32 length = strlen(text);
    for (i32 i = 0; i < length; ++i) {
        if (*(text + i) == token)
            return text + i;
    }
    return NULL;
}

char* FindLastToken(char* text, char token) {
    return const_cast<char*>(FindLastToken(static_cast<const char*>(text), token));
}

const char* FindLastToken(const char* text, char token) {
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

    const char* alpha;

    gConfig.musicVolume = CONFIG_VOLUME_MIN;
    gConfig.soundVolume = CONFIG_VOLUME_MIN;
    gConfig.autosave = 1;
    gConfig.showRoute = 1;
    gConfig.blackoutComputer = 0;
    for (i = H2EnumIndex(CONFIG_EXECUTABLE_GAME); i < H2EnumIndex(CONFIG_EXECUTABLE_COUNT); ++i) {
        gConfig.gfx[i].showMenu = 1;
        gConfig.gfx[i].x = DEFAULT_WINDOW_ORIGIN;
        gConfig.gfx[i].y = DEFAULT_WINDOW_ORIGIN;
        gConfig.gfx[i].colorMouseCursor = 0;
        gConfig.gfx[i].fullScreen = true;
        if (giMainVideoModeWidth <= LOGICAL_SCREEN_WIDTH) {
            gConfig.gfx[i].width = DEFAULT_SMALL_WINDOW_WIDTH;
            gConfig.gfx[i].height = DEFAULT_SMALL_WINDOW_HEIGHT;
        } else {
            gConfig.gfx[i].width = LOGICAL_SCREEN_WIDTH;
            gConfig.gfx[i].height = LOGICAL_SCREEN_HEIGHT;
        }
    }
    gConfig.showCombatGrid = 1;
    gConfig.showCombatMouseHex = 1;
    gConfig.combatShadeLevel = 1;
    gConfig.combatArmyInfoLevel = 2;
    gConfig.evilInterfaceUsage = 0;
    gConfig.useOpera = CONFIG_OPERA_ENABLED;
    gConfig.quickCombatLevel = 0;
    gConfig.combatSpeed = 0;
    gConfig.autoCombatUseSpells = 0;
    gConfig.blackoutComputer = 0;
    gConfig.currentMapOffset = 0;
    gConfig.firstMapOffset = Random(0, DEFAULT_MAP_OFFSET_MAX);
    gConfig.showObjectBoxes = 0;
    gConfig.editorScreenAnimation = 0;
    gConfig.editorPaletteCycling = 0;
    gbFirstTimeThrough = true;
    gConfig.walkSpeeds[H2EnumIndex(CONFIG_WALK_SPEED_HUMAN)] = CONFIG_WALK_SPEED_NORMAL;
    gConfig.slowVideo = DEFAULT_SLOW_VIDEO;
    gConfig.walkSpeeds[H2EnumIndex(CONFIG_WALK_SPEED_COMPUTER)] = CONFIG_WALK_SPEED_FAST;

    utf8::Copy(
        gConfig.networkDefaultName,
        sizeof(gConfig.networkDefaultName),
        localization::Tr("player.unknown_hero_name")
    );

    alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    memset(gConfig.uniqueSystemID, 0, CONFIG_UNIQUE_SYSTEM_ID_SIZE);
    seed = 0;
    seed += Random(1, UNIQUE_ID_RANDOM_MAX) + platform::Ticks();
    gConfig.uniqueSystemID[UNIQUE_ID_TRAILING_INDEX] =
        alpha[seed % UNIQUE_ID_ALPHANUMERIC_COUNT];
    seed += Random(1, UNIQUE_ID_RANDOM_MAX) + platform::Ticks();
    gConfig.uniqueSystemID[UNIQUE_ID_MIDDLE_INDEX] =
        alpha[seed % UNIQUE_ID_ALPHANUMERIC_COUNT];
    seed += Random(1, UNIQUE_ID_RANDOM_MAX) + platform::Ticks();
    gConfig.uniqueSystemID[UNIQUE_ID_LEADING_INDEX] =
        static_cast<char>(seed % UNIQUE_ID_ALPHA_COUNT + 'A');
    gConfig.needsDefaultInitialization = 0;
}

void ReadPrefsFromFile(void) {
    i32 file = platform::FileOpen("HEROES2.CFG", platform::FileMode::Read);
    if (file == -1) {
        SetInstallDefaults();
        SetGameDefaults();
        WritePrefs();
    } else {
        const bool complete = platform::FileReadExact(file, &gConfig, CONFIG_PERSISTED_SIZE);
        platform::FileClose(file);
        if (!complete) {
            memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
            SetInstallDefaults();
            SetGameDefaults();
            WritePrefs();
        } else if (gConfig.needsDefaultInitialization != 0) {
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

void ReadPrefsFromRegistry(void) {
    ReadPrefsFromFile();
}

void ReadPrefs(void) {
    memset(&gConfig, 0, CONFIG_PERSISTED_SIZE);
    ReadPrefsFromRegistry();
    utf8::Format(gConfig.rmtRLName, "RMT%sRL.BIN", gConfig.uniqueSystemID);
    utf8::Format(gConfig.rmtRCName, "RMT%sRC.BIN", gConfig.uniqueSystemID);
    utf8::Format(gConfig.rmtRDName, "RMT%sRD.BIN", gConfig.uniqueSystemID);
    utf8::Format(gConfig.rmtSLName, "RMT%sSL.BIN", gConfig.uniqueSystemID);
    utf8::Format(gConfig.rmtSCName, "RMT%sSC.BIN", gConfig.uniqueSystemID);
    utf8::Format(gConfig.rmtSDName, "RMT%sSD.BIN", gConfig.uniqueSystemID);
}

void WritePrefsToFile(void) {
    i32 fileDescriptor;

    fileDescriptor = platform::FileOpen("HEROES2.CFG", platform::FileMode::Write);
    if (fileDescriptor == -1)
        return;
    if (!platform::FileWriteExact(fileDescriptor, &gConfig, CONFIG_PERSISTED_SIZE)) {
        platform::Host().Log(platform::LogLevel::Warning, "preferences: incomplete write");
    }
    platform::FileClose(fileDescriptor);
}

void WritePrefsToRegistry(void) {
    WritePrefsToFile();
}

void WritePrefs(void) {
    UpdateSystemOptionsMenu();
    WritePrefsToRegistry();
}

CDRomSetupResult SetupCDDrive(void) {
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%sHEROES2x.AGG", ".\\DATA\\");
    i32 file = platform::FileOpen(gText, platform::FileMode::Read);
    if (file == -1)
        return CD_ROM_DATA_FILES_MISSING;
    platform::FileClose(file);

    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%s%s", ".", gcCDTrackName);
    file = platform::FileOpen(gText, platform::FileMode::Read);
    if (file == -1)
        return CD_ROM_DATA_FILES_MISSING;
    platform::FileClose(file);

    strcpy(gcAnimPath, ".\\HEROES2\\ANIM\\");
    return CD_ROM_READY;
}

void BitmapToScreen(class bitmap* image) {
    BlitBitmapToScreen(image, 0, 0, image->m_width, image->m_height, 0, 0);
}

void SetPalette(i8* paletteData, i32 updateDisplay) {
    memcpy(gpBufferPalette->m_data, paletteData, PALETTE_DATA_SIZE);
    memcpy(
        gCyclePal,
        paletteData + H2EnumIndex(CYCLE_RANGE_ONE_FIRST) * H2EnumIndex(PALETTE_CHANNEL_COUNT),
        sizeof(gCyclePal)
    );
    if (updateDisplay != 0)
        UpdatePalette(gpBufferPalette->m_data);
}

void BlitBitmapToScreenNoMouseCheck(
    class bitmap* image,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height,
    i32 destinationX,
    i32 destinationY
) {
    BlitBitmapToScreenVesa(image, sourceX, sourceY, width, height, destinationX, destinationY);
    platform::Video().Present();
}

void BlitBitmapToScreen(
    class bitmap* image,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height,
    i32 destinationX,
    i32 destinationY
) {
    if (gbColorMice == 0) {
        BlitBitmapToScreenVesa(image, sourceX, sourceY, width, height, destinationX, destinationY);
        platform::Video().Present();
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
        BlitBitmapToScreenVesa(image, sourceX, sourceY, width, height, destinationX, destinationY);
    } else {
        gpMouseManager->SaveAndDraw();
        BlitBitmapToScreenVesa(image, sourceX, sourceY, width, height, destinationX, destinationY);
        if (gBlitRight < gpMouseManager->m_cursorRight
            || destinationX > gpMouseManager->m_savedLeft
            || gBlitBottom < gpMouseManager->m_cursorBottom
            || destinationY > gpMouseManager->m_savedTop) {
            BlitBitmapToScreenVesa(
                image,
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
    platform::Video().Present();
}
void LogTruncate(void) {
    char logText[TEXT_BUFFER_SIZE];
    i32 fileHandle;
    if (giDebugLevel < FILE_DEBUG_LEVEL)
        return;
    fileHandle = platform::FileOpen("KB.LOG", platform::FileMode::Write);
    if (fileHandle == -1)
        return;
    strcpy(logText, "===========New Log==========");
    strcat(logText, "\n");
    if (!platform::FileWriteExact(fileHandle, logText, static_cast<i32>(strlen(logText)))) {
        platform::FileClose(fileHandle);
        return;
    }
    platform::FileClose(fileHandle);
}

void LogStr(const char* text) {
    char logText[TEXT_BUFFER_SIZE];
    i32 out;
    if (giDebugLevel < FILE_DEBUG_LEVEL)
        return;
    out = platform::FileOpen("KB.LOG", platform::FileMode::Append);
    if (out == -1)
        return;
    strcpy(logText, text);
    strcat(logText, "\n");
    if (!platform::FileWriteExact(out, logText, static_cast<i32>(strlen(logText)))) {
        platform::FileClose(out);
        return;
    }
    platform::FileClose(out);
    if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
        platform::Host().Log(platform::LogLevel::Debug, logText);
}

void LogInt(
    const char* label,
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
        utf8::Format(
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
        utf8::Format(
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
        utf8::Format(
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
        utf8::Format(text, "%s : % 8d % 8d % 8d % 8d", label, value1, value2, value3, value4);
    else if (value3 != LOG_UNUSED_VALUE)
        utf8::Format(text, "%s : % 8d % 8d % 8d", label, value1, value2, value3);
    else if (value2 != LOG_UNUSED_VALUE)
        utf8::Format(text, "%s : % 8d % 8d", label, value1, value2);
    else
        utf8::Format(text, "%s : % 8d", label, value1);
    LogStr(text);
}

void AiPrint(const char* text) {
    if (giDebugLevel < FILE_DEBUG_LEVEL)
        return;

    FillBitmapArea(
        gpWindowManager->m_screen, 0, STATUS_BAR_Y, LOGICAL_SCREEN_WIDTH, STATUS_BAR_HEIGHT, 0
    );
    smallFont->DrawBoundedString(
        text,
        0,
        STATUS_TEXT_Y,
        LOGICAL_SCREEN_WIDTH,
        STATUS_TEXT_HEIGHT,
        FONT_DRAW_DEFAULT,
        FONT_ALIGN_LEFT
    );
    BlitBitmapToScreen(
        gpWindowManager->m_screen,
        0,
        STATUS_BAR_Y,
        LOGICAL_SCREEN_WIDTH,
        STATUS_BAR_HEIGHT,
        0,
        STATUS_BAR_Y
    );
}

void AbsAiPrint(const char* text) {
    i32 saved = giDebugLevel;
    giDebugLevel = FORCED_DEBUG_LEVEL;
    AiPrint(text);
    giDebugLevel = saved;
}

void FadeTo(u8* source, u8* destination, i32 increment) {
    u8 temp[PALETTE_DATA_SIZE];
    u8 *current, *destinationPalette;
    i32 index, change, diff, move, iLevel, nextTime, k;

    memcpy(temp, source, PALETTE_DATA_SIZE);
    increment >>= FADE_TO_INCREMENT_SHIFT;
    if (increment < 1) {
        increment = 1;
    }
    for (iLevel = FADE_TO_START_LEVEL; iLevel < PALETTE_LEVEL_COUNT; iLevel += increment) {
        nextTime = platform::Ticks() + FADE_TO_FRAME_DELAY;
        PollSound();
        index = PALETTE_LEVEL_COUNT - iLevel - increment;
        if (index < 0)
            index = 0;
        change = giChangeThreshold[index];
        current = temp;
        destinationPalette = destination;
        for (k = 0; k < PALETTE_DATA_SIZE; ++k) {
            diff = *destinationPalette - *current;
            if (abs(diff) > change) {
                move = abs(diff) - change;
                if (diff > 0)
                    *current = *current + move;
                else
                    *current = *current - move;
            }
            ++current;
            ++destinationPalette;
        }
        UpdatePalette(reinterpret_cast<i8*>(temp));
        DelayTil(&nextTime);
    }
    UpdatePalette(reinterpret_cast<i8*>(destination));
}

void FadeToColorTable(u8* colorTable, i32 increment) {
    u8* currentColorTable;
    i32 x;
    i32 i;
    i32 y;
    u8 tempPal[PALETTE_DATA_SIZE];
    i8* paletteData;
    i32 savedFlags;

    savedFlags = gpWindowManager->m_updateFlags;
    gpWindowManager->m_updateFlags = 0;
    paletteData = gpBufferPalette->m_data;
    for (i = 0;
         i < H2EnumIndex(PALETTE_DATA_SIZE) / H2EnumIndex(PALETTE_CHANNEL_COUNT);
         ++i) {
        tempPal[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_RED)] =
            paletteData[colorTable[i] * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_RED)];
        tempPal[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)] =
            paletteData[colorTable[i] * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)];
        tempPal[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)] =
            paletteData[colorTable[i] * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)];
    }
    FadeTo(reinterpret_cast<u8*>(paletteData), tempPal, increment);
    currentColorTable = gpWindowManager->m_screen->m_pixels;
    for (y = 0; y < LOGICAL_SCREEN_HEIGHT; ++y) {
        for (x = 0; x < LOGICAL_SCREEN_WIDTH; ++x) {
            *currentColorTable = colorTable[*currentColorTable];
            ++currentColorTable;
        }
    }
    gpWindowManager->UpdateScreen();
    UpdatePalette(paletteData);
    gpWindowManager->m_updateFlags = savedFlags;
}

i32 IsCycleColor(i32 color) {
    return (color >= CYCLE_RANGE_ONE_FIRST && color <= CYCLE_RANGE_ONE_LAST)
        || (color >= CYCLE_RANGE_TWO_FIRST && color <= CYCLE_RANGE_TWO_LAST);
}

void CreatePCXFile(
    const char* filename,
    u8* pixels,
    i32 width,
    i32 height,
    u8* paletteData
) {
    i32 fileDescriptor;
    i32 encodedLength;
    u8 bMark;
    u8 color;
    i32 sourceIndex;
    i32 x;
    i32 y;
    i32 endPosition;
    u8* rowPointer;
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
    fileDescriptor = platform::FileOpen(filename, platform::FileMode::Write);
    if (fileDescriptor == -1)
        return;
    bool complete = platform::FileWriteExact(fileDescriptor, &pcxHdr, sizeof(pcxHdr));
    encodedRow = static_cast<u8*>(H2_ALLOC(width * 2));
    for (y = 0; y < height; ++y) {
        sourceIndex = 0;
        rowPointer = pixels + y * width;
        encodedLength = 0;
        while (sourceIndex < width) {
            color = *(rowPointer + sourceIndex);
            endPosition = sourceIndex;
            while (endPosition < width && *(rowPointer + endPosition) == color
                   && endPosition - sourceIndex + 1 < RLE_RUN_LIMIT)
                ++endPosition;
            runLength = endPosition - sourceIndex;
            if (runLength > 1 || (color & RLE_RUN_MARKER) == RLE_RUN_MARKER) {
                *(encodedRow + encodedLength) = static_cast<u8>(runLength | RLE_RUN_MARKER);
                *(encodedRow + encodedLength + 1) = color;
                encodedLength += 2;
                sourceIndex += runLength;
            } else {
                *(encodedRow + encodedLength) = color;
                encodedLength += 1;
                sourceIndex += 1;
            }
        }
        complete = complete && platform::FileWriteExact(fileDescriptor, encodedRow, encodedLength);
    }
    H2_FREE(encodedRow);
    bMark = VGA_PALETTE_MARKER;
    complete = complete && platform::FileWriteExact(fileDescriptor, &bMark, 1);
    palOut = static_cast<u8*>(H2_ALLOC(PALETTE_DATA_SIZE));
    for (x = 0; x < PALETTE_DATA_SIZE; ++x)
        *(palOut + x) = *(paletteData + x) << COMPONENT_SCALE_SHIFT;
    complete = complete && platform::FileWriteExact(fileDescriptor, palOut, PALETTE_DATA_SIZE);
    H2_FREE(palOut);
    platform::FileClose(fileDescriptor);
    if (!complete)
        platform::Host().Log(platform::LogLevel::Warning, "screenshot: incomplete PCX write");
}

i32l FileSize(const char* filename) {
    i32 file = platform::FileOpen(filename, platform::FileMode::Read);
    if (file == -1)
        FileError(filename);
    i32l size = platform::FileLength(file);
    platform::FileClose(file);
    return size;
}

struct IconEntry* GetIconEntry(class icon* iconPointer, i32 index) {
    return reinterpret_cast<struct IconEntry*>(index * sizeof(IconEntry) + iconPointer->m_data);
}
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

void SRand(i32 seed) {
    iLastSeed = seed;
    srand(seed);
}

i32 SGenRand(void) {
    i32 bitMask;
    i32 result = 0;
    iLastSeed &= RANDOM_SEED_MASK;
    iLastSeed *= RANDOM_MIX_MULTIPLIER;
    iLastSeed += (iLastSeed & RANDOM_MIX_MASK) >> RANDOM_MIX_SHIFT;
    for (i32 i = RANDOM_TOP_BIT; i >= 0; --i) {
        bitMask = 1 << i;
        if (iLastSeed & bitMask) {
            result |= 1 << i;
        }
    }
    return result;
}

i32 MemSize(i32) {
    return REPORTED_MEMORY_KILOBYTES;
}
void GetDataEntry(
    const char* prompt,
    char* destination,
    i32 maximumLength,
    char* initialText,
    i32 showCancel,
    i32 useImmediateHandler
) {
    MouseCursorType savedCursorType;

    i32 nRows;
    i32 entryY;
    i32 nHeight;
    i32 textLines;
    char textBuffer[TEXT_BUFFER_CAPACITY];
    textEntryWidget* pText;
    tag_message message;
    i32 nFrame;

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

    const std::string windowName = "evntwin" + std::to_string(nRows) + ".bin";
    DataEntryWin = new heroWindow(WINDOW_X, WINDOW_Y, windowName.c_str());
    if (DataEntryWin == NULL)
        MemError();

    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, ENTRY_PROMPT_WIDGET);
    message.payload.widget.data.text = prompt;
    DataEntryWin->BroadcastMessage(message);

    if (initialText != NULL)
        strcpy(textBuffer, initialText);
    else
        strcpy(
            textBuffer,
            ""
        );
    message.payload.widget.id = ENTRY_TEXT_WIDGET;
    message.payload.widget.data.text = textBuffer;
    DataEntryWin->BroadcastMessage(message);
    strcpy(destination, textBuffer);

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
    message.payload.widget.id = DIALOG_BUTTON_1;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = DIALOG_BUTTON_7;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = DIALOG_BUTTON_8;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = DIALOG_BUTTON_5;
    DataEntryWin->BroadcastMessage(message);
    message.payload.widget.id = DIALOG_BUTTON_6;
    DataEntryWin->BroadcastMessage(message);
    if (showCancel == 0) {
        message.payload.widget.id = ENTRY_CANCEL_BUTTON;
        DataEntryWin->BroadcastMessage(message);
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

MessageDispatchResult DataEntryWindowHandler(struct tag_message& message) {

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
            case WIDGET_NOTIFY_DESELECT:
                switch (message.payload.widget.id) {
                    case ENTRY_CANCEL_BUTTON:
                        message.payload.widget.id = ENTRY_TEXT_WIDGET;
                        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
                        return MESSAGE_DISPATCH_FORWARD;
                }
                break;
            case WIDGET_NOTIFY_SELECT:
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
                        SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, ENTRY_TEXT_WIDGET);
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
