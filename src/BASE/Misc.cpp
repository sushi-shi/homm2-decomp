// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Misc.obj   from: .\basewin.lib
// functions: 46   data: 11
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
RVA(0x004c3d10, 0x58)
// void InitMemEntry(void);

RVA(0x004c3d70, 0x20f)
// void * BaseAlloc(unsigned int, char *, int);

RVA(0x004c3f80, 0x386)
// void BaseFree(void *, char *, int);

RVA(0x004c4310, 0x134)
// void PrintMemoryLeaks(void);

RVA(0x004c4450, 0x91)
// void ShowMemoryStatus(void);

RVA(0x004c44f0, 0x48)
// unsigned long int MAKEFILEID(char *);

RVA(0x004c4540, 0x95)
// int FindIndex(struct indexArray *, int, int, int);

RVA(0x004c45e0, 0xea)
// void FadeIn(int);

RVA(0x004c46d0, 0xe6)
// void FadeOut(int);

RVA(0x004c47c0, 0x28)
// int Random(int, int);

RVA(0x004c47f0, 0x5d)
// void ProcessAssert(int, char *, int);

RVA(0x004c4850, 0x66)
// char * FindStringInString(char *, char *);

RVA(0x004c48c0, 0x31)
// char * FindToken(char *, char);

RVA(0x004c4900, 0x2b)
// char * FindLastToken(char *, char);

RVA(0x004c4930, 0x6c)
// void SetInstallDefaults(void);

RVA(0x004c49a0, 0x1b5)
// void SetGameDefaults(void);

RVA(0x004c4b60, 0x13f)
// void ReadPrefsFromFile(void);

RVA(0x004c4ca0, 0x7ab)
// void ReadPrefsFromRegistry(void);

RVA(0x004c5450, 0xa1)
// void ReadPrefs(void);

RVA(0x004c5500, 0x6a)
// void WritePrefsToFile(void);

RVA(0x004c5570, 0x491)
// void WritePrefsToRegistry(void);

RVA(0x004c5a10, 0xa)
// void WritePrefs(void);

RVA(0x004c5a20, 0x3c)
// int IsCDDrive(int);

RVA(0x004c5a60, 0x3ed)
// int SetupCDDrive(void);

RVA(0x004c5e50, 0x18)
// void BitmapToScreen(class bitmap *);

RVA(0x004c5e70, 0x3d)
// void SetPalette(signed char *, int);

RVA(0x004c5eb0, 0x25)
// void BlitBitmapToScreenNoMouseCheck(class bitmap *, int, int, int, int, int, int);

RVA(0x004c5ee0, 0x18b)
// void BlitBitmapToScreen(class bitmap *, int, int, int, int, int, int);

RVA(0x004c6070, 0xa6)
// void LogTruncate(void);

RVA(0x004c6120, 0x9e)
// void LogStr(char *);

RVA(0x004c61c0, 0x224)
// void LogInt(char *, int, int, int, int, int, int, int);

RVA(0x004c63f0, 0x6c)
// void AiPrint(char *);

RVA(0x004c6460, 0x7a)
// void AbsAiPrint(char *);

RVA(0x004c64e0, 0xf8)
// void FadeTo(unsigned char *, unsigned char *, int);

RVA(0x004c65e0, 0xb8)
// void FadeToColorTable(unsigned char *, int);

RVA(0x004c66a0, 0x29)
// int IsCycleColor(int);

RVA(0x004c66d0, 0x1ee)
// void CreatePCXFile(char *, unsigned char *, int, int, unsigned char *);

RVA(0x004c68c0, 0x52)
// long int FileSize(char *);

RVA(0x004c6920, 0xc)
// struct IconEntry * GetIconEntry(class icon *, int);

RVA(0x004c6930, 0xb8)
// int SRandom(int, int);

RVA(0x004c69f0, 0x5c)
// void SIncRandomize(int, int);

RVA(0x004c6a50, 0x10)
// void SRand(int);

RVA(0x004c6a60, 0x48)
// int SGenRand(void);

RVA(0x004c6ab0, 0x6)
// int MemSize(int);

RVA(0x004c6ac0, 0x386)
// void GetDataEntry(char *, char *, int, char *, int, int);

RVA(0x004c6e50, 0x173)
// int DataEntryWindowHandler(struct tag_message &);

// ---- data / globals / vtables ----
DATA(0x0051dce8)  // int iMemEntries
DATA(0x0051dcec)  // struct MemEntry * gpMemEntry
DATA(0x0051dcf0)  // int giTotalMemAllocated
DATA(0x0051dcf8)  // unsigned char * giChangeThreshold
DATA(0x0051dd08)  // int iLastSeed
DATA(0x005331c4)  // class heroWindow * DataEntryWin
DATA(0x005331c8)  // char * cDEDest
DATA(0x005331d4)  // int iDEMaxLen
DATA(0x005331d8)  // int bDataEntryTime
DATA(0x005331dc)  // int inBoxX
DATA(0x005331e0)  // int inBoxY
