#ifndef HOMM2_MISC_H
#define HOMM2_MISC_H

#include <Ints.h>
#include <BASE/message.h>

enum class CDRomSetupResult : i32;

class bitmap;
class heroWindow;
class icon;
struct tag_message;

typedef enum MemoryDebugRecordConstant {
    MEM_ENTRY_FILE_CAPACITY = 0x3d,
} MemoryDebugRecordConstant;

typedef enum PcxHeaderConstant {
    PCX_HEADER_PALETTE16_BYTES = 48,
    PCX_HEADER_FILLER_BYTES    = 54,
} PcxHeaderConstant;

typedef enum LogConstant {
    LOG_UNUSED_VALUE = -999
} LogConstant;


#define MANHATTAN_LENGTH(dx, dy) (abs((dx)) + abs((dy)))
#define INTEGER_VECTOR_LENGTH(dx, dy) \
    (static_cast<i32>(sqrt((dx) * (dx) + (dy) * (dy))))

struct indexArray {
    u16 key;
    u16 value;
};

#pragma pack(push, 1)
struct MemEntry {
    u8 used;
    void* ptr;
    u32 size;
    char file[MEM_ENTRY_FILE_CAPACITY];
    i32 line;
};
#pragma pack(pop)
#ifdef HOMM2_MISC_INLINE_ICONENTRY
#pragma pack(push, 1)
struct IconEntry {
    i16 x;
    i16 y;
    i16 w;
    i16 h;
    u8 flags : 5;
    i32 srcOffset;
};
#pragma pack(pop)
#else
#define HOMM2_BASE_ICONENTRY_NO_SIZE
#include <BASE/IconEntry.h>
#undef HOMM2_BASE_ICONENTRY_NO_SIZE
#endif
#pragma pack(push, 1)
struct PCXHeader {
    u8 manufacturer, version, encoding, bitsPerPixel;
    u16 xMin, yMin, xMax, yMax;
    u16 horizontalDpi, verticalDpi;
    u8 palette16[PCX_HEADER_PALETTE16_BYTES];
    u8 reserved, planes;
    u16 bytesPerLine, paletteType, horizontalScreenSize, verticalScreenSize;
    u8 filler[PCX_HEADER_FILLER_BYTES];
};
#pragma pack(pop)

void InitMemEntry(void);
void* BaseAlloc(u32 size, const char* originalFile, i32 originalLine);
void BaseFree(void* pointer, const char* originalFile, i32 originalLine);
void PrintMemoryLeaks(void);
void ShowMemoryStatus(void);
u32l MAKEFILEID(const char* text);
i32 FindIndex(struct indexArray* entries, i32 low, i32 high, i32 key);
void FadeIn(i32 increment);
void FadeOut(i32 increment);
i32 Random(i32 low, i32 high);
void ProcessAssert(i32 condition, const char* file, i32 line);


constexpr const char* H2SourceName(const char* path) {
    const char* name = path;
    for (const char* cursor = path; *cursor != '\0'; ++cursor) {
        if (*cursor == '/' || *cursor == '\\') {
            name = cursor + 1;
        }
    }
    return name;
}

#define H2_ALLOC(size) BaseAlloc(size, H2SourceName(__FILE__), __LINE__)
#define H2_FREE(ptr) BaseFree(ptr, H2SourceName(__FILE__), __LINE__)
#define H2_ASSERT(condition)                                                                       \
    ProcessAssert(condition, H2SourceName(__FILE__), __LINE__)

#define ALLOC_COPY_STRING(destination, source)                                                     \
    ((destination) = static_cast<char*>(H2_ALLOC(strlen(source) + 1)),                             \
     strcpy((destination), (source)))
char* FindStringInString(char* text, const char* pattern);
const char* FindStringInString(const char* text, const char* pattern);
char* FindToken(char* text, char token);
const char* FindToken(const char* text, char token);
char* FindLastToken(char* text, char token);
const char* FindLastToken(const char* text, char token);
void SetInstallDefaults(void);
void SetGameDefaults(void);
void ReadPrefsFromFile(void);
void ReadPrefsFromRegistry(void);
void ReadPrefs(void);
void WritePrefsToFile(void);
void WritePrefsToRegistry(void);
void WritePrefs(void);
CDRomSetupResult SetupCDDrive(void);
void BitmapToScreen(class bitmap* image);
void SetPalette(i8* paletteData, i32 updateDisplay);
void BlitBitmapToScreenNoMouseCheck(class bitmap* image, i32 sourceX, i32 sourceY, i32 width, i32 height, i32 destinationX, i32 destinationY);
void BlitBitmapToScreen(class bitmap* image, i32 sourceX, i32 sourceY, i32 width, i32 height, i32 destinationX, i32 destinationY);
void LogTruncate(void);
void LogStr(const char* text);
void LogInt(
    const char* label,
    i32 value1,
    i32 value2 = LOG_UNUSED_VALUE,
    i32 value3 = LOG_UNUSED_VALUE,
    i32 value4 = LOG_UNUSED_VALUE,
    i32 value5 = LOG_UNUSED_VALUE,
    i32 value6 = LOG_UNUSED_VALUE,
    i32 value7 = LOG_UNUSED_VALUE
);
template<typename Enum>
requires __is_enum(Enum) inline void LogInt(
    const char* text,
    Enum value,
    i32 b = LOG_UNUSED_VALUE,
    i32 c = LOG_UNUSED_VALUE,
    i32 d = LOG_UNUSED_VALUE,
    i32 e = LOG_UNUSED_VALUE,
    i32 f = LOG_UNUSED_VALUE,
    i32 g = LOG_UNUSED_VALUE
) {
    LogInt(text, static_cast<i32>(value), b, c, d, e, f, g);
}
void AiPrint(const char* text);
void AbsAiPrint(const char* text);
void FadeTo(u8* source, u8* destination, i32 increment);
void FadeToColorTable(u8* colorTable, i32 increment);
i32 IsCycleColor(i32 color);
void CreatePCXFile(const char* filename, u8* pixels, i32 width, i32 height, u8* paletteData);
i32l FileSize(const char* filename);
struct IconEntry* GetIconEntry(class icon* iconPointer, i32 index);
i32 SRandom(i32 low, i32 high);
void SIncRandomize(i32 x, i32 y);
void SRand(i32 seed);
i32 SGenRand(void);
i32 MemSize(i32);
void GetDataEntry(const char* prompt, char* destination, i32 maximumLength, char* initialText, i32 showCancel, i32 useImmediateHandler);
MessageDispatchResult DataEntryWindowHandler(struct tag_message& message);

enum class DataEntryPhase : i32 {
    ENTRY_PHASE_IMMEDIATE    = 0,
    ENTRY_PHASE_POINTER_SENT = 1,
    ENTRY_PHASE_READY        = 2
};
using enum DataEntryPhase;

extern H2SteppedEnumStorage<DataEntryPhase, i32> bDataEntryTime;
extern char* cDEDest;
extern class heroWindow* DataEntryWin;
extern MemEntry* gpMemEntry;
extern i32 iDEMaxLen;
extern i32 iLastSeed;
extern i32 iMemEntries;
extern i32 inBoxX;
extern i32 inBoxY;

#endif
