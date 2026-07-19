#ifndef HOMM2_MISC_H
#define HOMM2_MISC_H
#include <va.h>
class bitmap;
class heroWindow;
class icon;
struct tag_message;

struct indexArray {
    u16 key;
    u16 value;
};

// Leak-tracking allocation record (BaseAlloc/BaseFree). Packed: ptr sits at +1 (unaligned).
#pragma pack(push, 1)
struct MemEntry {
    char used;
    void* ptr;
    u32 size;
    char file[0x3d];
    i32 line;
};
#pragma pack(pop)
SIZE(MemEntry, 0x4a);
#ifdef HOMM2_MISC_INLINE_ICONENTRY
#pragma pack(push, 1)
struct IconEntry {
    i16 x;
    i16 y;
    i16 w;
    i16 h;
    u8 flags;
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
    u8 palette16[48];
    u8 reserved, planes;
    u16 bytesPerLine, paletteType, horizontalScreenSize, verticalScreenSize;
    u8 filler[54];
};
#pragma pack(pop)
SIZE(PCXHeader, 0x80);

void InitMemEntry(void);
void* BaseAlloc(u32, char*, i32);
void BaseFree(void*, char*, i32);
void PrintMemoryLeaks(void);
void ShowMemoryStatus(void);
u32l MAKEFILEID(char* text);
i32 FindIndex(struct indexArray* entries, i32 low, i32 high, i32 key);
void FadeIn(i32);
void FadeOut(i32);
i32 Random(i32 low, i32 high);
void ProcessAssert(i32 condition, char* file, i32 line);
// Allocation wrappers preserve explicit source-file and line operands.
#define H2_ALLOC(size, originalLine) BaseAlloc(size, const_cast<char*>(RETAIL_FILE), originalLine)
#define H2_FREE(ptr, originalLine) BaseFree(ptr, const_cast<char*>(RETAIL_FILE), originalLine)
#define H2_ALLOC_AT(size, originalFile, originalLine) BaseAlloc(size, const_cast<char*>(originalFile), originalLine)
#define H2_FREE_AT(ptr, originalFile, originalLine) BaseFree(ptr, const_cast<char*>(originalFile), originalLine)
#define H2_ASSERT(condition, originalFile, originalLine)                                           \
    ProcessAssert(condition, originalFile, originalLine)
char* FindStringInString(char* text, char* pattern);
char* FindToken(char* text, char token);
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
i32 SetupCDDrive(void);
void BitmapToScreen(class bitmap*);
void SetPalette(i8*, i32);
void BlitBitmapToScreenNoMouseCheck(class bitmap*, i32, i32, i32, i32, i32, i32);
void BlitBitmapToScreen(class bitmap*, i32, i32, i32, i32, i32, i32);
void LogTruncate(void);
void LogStr(char*);
void LogInt(char*, i32, i32, i32, i32, i32, i32, i32);
void AiPrint(char*);
void AbsAiPrint(char*);
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
void GetDataEntry(char*, char*, i32, char*, i32, i32);
i32 DataEntryWindowHandler(struct tag_message& message);

extern i32 bDataEntryTime;
extern char* cDEDest;
extern class heroWindow* DataEntryWin;
extern MemEntry* gpMemEntry;
extern i32 iDEMaxLen;
extern i32 iLastSeed;
extern i32 iMemEntries;
extern i32 inBoxX;
extern i32 inBoxY;

#endif
