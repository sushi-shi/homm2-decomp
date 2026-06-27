// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SMACKMGR.OBJ   from: (directly linked into exe)
// functions: 8   data: 12
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
VA(0x00401000, 0x4e)
// void ConvertSmackerPalette(unsigned char *);

VA(0x0040104e, 0x21f)
// void DoAdvance(struct SmackTag *, int, int, int, int);

VA(0x0040126d, 0x118c)
// void SmackManagerMain(void);

VA(0x004023f9, 0x56)
// void ShutDownSmacker(void);

VA(0x0040244f, 0x17f)
// int PlaySmacker(int);

VA(0x004025ce, 0x65)
// int ExpansionCampaignRect(int, int);

VA(0x00402633, 0x89)
// signed char PointInRect(int, int, struct tag_rect *);

VA(0x004026bc, 0x251)
// void PrintSummaryInfo(struct SmackSumTag *);

// ---- data / globals / vtables ----
DATA(0x004ec040)  // int bSmackSound
DATA(0x004ec044)  // class icon * brotherIcon
DATA(0x004ec068)  // class icon * backImage
DATA(0x004ec070)  // struct SSmackOptions * SmackOptions
DATA(0x004ecd48)  // int bTesting
DATA(0x004ecd4c)  // struct SmackTag * smk1
DATA(0x004ecd50)  // struct SmackTag * smk2
DATA(0x00522f20)  // signed char bSmackNum
DATA(0x00522f24)  // int gbLastFramePlayed
DATA(0x00522f28)  // struct SmackSumTag smksum
DATA(0x00522f80)  // int gbPlayedThrough
DATA(0x00522f84)  // signed char bMainDone
