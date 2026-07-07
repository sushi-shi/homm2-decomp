#ifndef HOMM2_MISC_H
#define HOMM2_MISC_H
// Declarations of the free functions DEFINED in Misc.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class icon;
struct tag_message;

// Misc-private record structs (params of the functions above).
struct indexArray { unsigned short field0; unsigned short field2; };
struct IconEntry { char pad[13]; };

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

#endif // HOMM2_MISC_H
