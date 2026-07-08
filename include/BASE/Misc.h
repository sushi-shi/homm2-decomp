#ifndef HOMM2_MISC_H
#define HOMM2_MISC_H
#include <va.h>
#include <_types.h>
// Declarations of the free functions DEFINED in Misc.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class heroWindow;
class icon;
struct tag_message;

// Misc-private record structs (params of the functions above).
struct indexArray { unsigned short field0; unsigned short field2; };
#pragma pack(push, 1)
struct IconEntry {          // one sprite frame header in the icon entry table (13B, indexed by frame)
    short x;                // +0  x offset
    short y;                // +2  y offset
    short w;                // +4  width
    short h;                // +6  height
    unsigned char flags;    // +8
    int srcOffset;          // +9  offset of this frame's RLE data within the icon blob
};
#pragma pack(pop)

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
DATA(0x005331d8) extern int bDataEntryTime;
DATA(0x005331c8) extern char *cDEDest;
DATA(0x005331c4) extern class heroWindow *DataEntryWin;
DATA(0x0051dcec) extern MemEntry *gpMemEntry;
DATA(0x005331d4) extern int iDEMaxLen;
DATA(0x0051dd08) extern int iLastSeed;
DATA(0x0051dce8) extern int iMemEntries;
DATA(0x005331dc) extern int inBoxX;
DATA(0x005331e0) extern int inBoxY;

#endif // HOMM2_MISC_H
