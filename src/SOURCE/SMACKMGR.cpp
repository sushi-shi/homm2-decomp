// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SMACKMGR.OBJ   from: (directly linked into exe)
// functions: 8   data: 12
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/SMACKMGR.h>
VA(0x00401000, 0x4e)
void ConvertSmackerPalette(unsigned char *) {}

VA(0x0040104e, 0x21f)
void DoAdvance(struct SmackTag *, int, int, int, int) {}

VA(0x0040126d, 0x118c)
void SmackManagerMain(void) {}

VA(0x004023f9, 0x56)
void ShutDownSmacker(void) {}

VA(0x0040244f, 0x17f)
int PlaySmacker(int) { return 0; }

VA(0x004025ce, 0x65)
int ExpansionCampaignRect(int, int) { return 0; }

VA(0x00402633, 0x89)
signed char PointInRect(int, int, struct tag_rect *) { return 0; }

VA(0x004026bc, 0x251)
void PrintSummaryInfo(struct SmackSumTag *) {}

// ---- globals (definitions, RVA order) ----
int bSmackSound;
class icon *brotherIcon;
class icon *backImage;
struct SSmackOptions *SmackOptions;
int bTesting;
struct SmackTag *smk1;
struct SmackTag *smk2;
signed char bSmackNum;
int gbLastFramePlayed;
int gbPlayedThrough;
signed char bMainDone;
