#ifndef HOMM2_MISC_H
#define HOMM2_MISC_H
#include <va.h>
#include <SOURCE/KB_TYPES.h>
// Declarations of the free functions DEFINED in Misc.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class heroWindow;
class icon;
struct tag_message;

// Misc-private record structs (params of the functions above).
struct indexArray {
    unsigned short key;
    unsigned short value;
};

// Leak-tracking allocation record (BaseAlloc/BaseFree). Packed: ptr sits at +1 (unaligned).
#pragma pack(push, 1)
struct MemEntry {
    char used;          // +0x00  in-use flag
    void *ptr;          // +0x01  allocated block
    unsigned int size;  // +0x05  block size
    char file[0x3d];    // +0x09  original source path supplied by the call site
    int line;           // +0x46  original source line supplied by the call site
};
#pragma pack(pop)
SIZE(MemEntry, 0x4a);
#ifdef HOMM2_MISC_INLINE_ICONENTRY
#pragma pack(push, 1)
struct IconEntry {
    short x;
    short y;
    short w;
    short h;
    unsigned char flags;
    int srcOffset;
};
#pragma pack(pop)
#else
#define HOMM2_BASE_ICONENTRY_NO_SIZE
#include <BASE/IconEntry.h>
#undef HOMM2_BASE_ICONENTRY_NO_SIZE
#endif
#pragma pack(push, 1)
struct PCXHeader {
    unsigned char manufacturer, version, encoding, bitsPerPixel;
    unsigned short xMin, yMin, xMax, yMax;
    unsigned short horizontalDpi, verticalDpi;
    unsigned char palette16[48];
    unsigned char reserved, planes;
    unsigned short bytesPerLine, paletteType, horizontalScreenSize, verticalScreenSize;
    unsigned char filler[54];
};
#pragma pack(pop)
SIZE(PCXHeader, 0x80);

void InitMemEntry(void);
void * BaseAlloc(unsigned int, char *, int);
void BaseFree(void *, char *, int);
void PrintMemoryLeaks(void);
void ShowMemoryStatus(void);
unsigned long int MAKEFILEID(char *text);
int FindIndex(struct indexArray *entries, int low, int high, int key);
void FadeIn(int);
void FadeOut(int);
int Random(int low, int high);
void ProcessAssert(int condition, char *file, int line);
// Retail source locations are explicit operands. Never substitute this build's __FILE__/__LINE__.
#define H2_ALLOC(size, originalFile, originalLine) BaseAlloc(size, originalFile, originalLine)
#define H2_FREE(ptr, originalFile, originalLine) BaseFree(ptr, originalFile, originalLine)
#define H2_ASSERT(condition, originalFile, originalLine) ProcessAssert(condition, originalFile, originalLine)
char * FindStringInString(char *text, char *pattern);
char * FindToken(char *text, char token);
char * FindLastToken(char *text, char token);
void SetInstallDefaults(void);
void SetGameDefaults(void);
void ReadPrefsFromFile(void);
void ReadPrefsFromRegistry(void);
void ReadPrefs(void);
void WritePrefsToFile(void);
void WritePrefsToRegistry(void);
void WritePrefs(void);
int IsCDDrive(int);
int SetupCDDrive(void);
void BitmapToScreen(class bitmap *);
void SetPalette(signed char *, int);
void BlitBitmapToScreenNoMouseCheck(class bitmap *, int, int, int, int, int, int);
void BlitBitmapToScreen(class bitmap *, int, int, int, int, int, int);
void LogTruncate(void);
void LogStr(char *);
void LogInt(char *, int, int, int, int, int, int, int);
void AiPrint(char *);
void AbsAiPrint(char *);
void FadeTo(unsigned char *, unsigned char *, int);
void FadeToColorTable(unsigned char *, int);
int IsCycleColor(int color);
void CreatePCXFile(char *, unsigned char *, int, int, unsigned char *);
long int FileSize(char *filename);
struct IconEntry * GetIconEntry(class icon *iconPtr, int index);
int SRandom(int low, int high);
void SIncRandomize(int x, int y);
void SRand(int seed);
int SGenRand(void);
int MemSize(int);
void GetDataEntry(char *, char *, int, char *, int, int);
int DataEntryWindowHandler(struct tag_message &message);


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
extern int bDataEntryTime;
extern char *cDEDest;
extern class heroWindow *DataEntryWin;
extern MemEntry *gpMemEntry;
extern int iDEMaxLen;
extern int iLastSeed;
extern int iMemEntries;
extern int inBoxX;
extern int inBoxY;

#endif // HOMM2_MISC_H
