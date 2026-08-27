#ifndef HOMM2_MISC_H
#define HOMM2_MISC_H

#include <Ints.h>
#include <BASE/message.h>

enum class CDRomSetupResult : i32;

class bitmap;
class heroWindow;
class icon;
struct tag_message;

typedef enum MiscRecordConstant {
    MEM_ENTRY_FILE_CAPACITY    = 0x3d,
    PCX_HEADER_PALETTE16_BYTES = 48,
    PCX_HEADER_FILLER_BYTES    = 54
} MiscRecordConstant;

typedef enum LogConstant {
    LOG_UNUSED_VALUE = -999
} LogConstant;

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
void* BaseAlloc(u32, const char*, i32);
void BaseFree(void*, const char*, i32);
void PrintMemoryLeaks(void);
void ShowMemoryStatus(void);
u32l MAKEFILEID(const char* text);
i32 FindIndex(struct indexArray* entries, i32 low, i32 high, i32 key);
void FadeIn(i32);
void FadeOut(i32);
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

#define H2_ALLOC(size) BaseAlloc(size, const_cast<char*>(H2SourceName(__FILE__)), __LINE__)
#define H2_FREE(ptr) BaseFree(ptr, const_cast<char*>(H2SourceName(__FILE__)), __LINE__)
#define H2_ASSERT(condition)                                                                       \
    ProcessAssert(condition, const_cast<char*>(H2SourceName(__FILE__)), __LINE__)
char* FindStringInString(char* text, const char* pattern);
char* FindToken(char* text, char token);
const char* FindToken(const char* text, char token);
char* FindLastToken(char* text, char token);
void SetInstallDefaults(void);
void SetGameDefaults(void);
void ReadPrefsFromFile(void);
void ReadPrefsFromRegistry(void);
void ReadPrefs(void);
void WritePrefsToFile(void);
void WritePrefsToRegistry(void);
void WritePrefs(void);
i32 IsCDDrive(i32);
bool DriveSupportsFreeSpaceQuery(char);
CDRomSetupResult SetupCDDrive(void);
void BitmapToScreen(class bitmap*);
void SetPalette(i8*, i32);
void BlitBitmapToScreenNoMouseCheck(class bitmap*, i32, i32, i32, i32, i32, i32);
void BlitBitmapToScreen(class bitmap*, i32, i32, i32, i32, i32, i32);
void LogTruncate(void);
void LogStr(const char*);
void LogInt(const char*, i32, i32, i32, i32, i32, i32, i32);
template <typename Enum>
    requires __is_enum(Enum)
inline void LogInt(const char* text, Enum value, i32 b, i32 c, i32 d, i32 e, i32 f, i32 g) {
    LogInt(text, static_cast<i32>(value), b, c, d, e, f, g);
}
void AiPrint(const char*);
void AbsAiPrint(const char*);
void FadeTo(u8*, u8*, i32);
void FadeToColorTable(u8*, i32);
i32 IsCycleColor(i32 color);
void CreatePCXFile(char*, u8*, i32, i32, u8*);
i32l FileSize(char* filename);
struct IconEntry* GetIconEntry(class icon* iconPtr, i32 index);
i32 SRandom(i32 low, i32 high);
void SIncRandomize(i32 x, i32 y);
void SRand(i32 seed);
i32 SGenRand(void);
i32 MemSize(i32);
void GetDataEntry(const char*, char*, i32, const char*, i32, i32);
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
