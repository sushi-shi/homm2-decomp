#ifndef HOMM2_SMACKMGR_H
#define HOMM2_SMACKMGR_H
// Declarations of the free functions DEFINED in SMACKMGR.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

void ConvertSmackerPalette(unsigned char *);
void DoAdvance(struct SmackTag *, int, int, int, int);
void SmackManagerMain(void);
void ShutDownSmacker(void);
int PlaySmacker(int);
int ExpansionCampaignRect(int, int);
signed char PointInRect(int, int, struct tag_rect *);
void PrintSummaryInfo(struct SmackSumTag *);

// ---- globals (declarations, RVA order) ----
DATA(0x004ec040) extern int bSmackSound;
DATA(0x004ec044) extern class icon *brotherIcon;
DATA(0x004ec068) extern class icon *backImage;
DATA(0x004ec070) extern struct SSmackOptions *SmackOptions;
DATA(0x004ecd48) extern int bTesting;
DATA(0x004ecd4c) extern struct SmackTag *smk1;
DATA(0x004ecd50) extern struct SmackTag *smk2;
DATA(0x00522f20) extern signed char bSmackNum;
DATA(0x00522f24) extern int gbLastFramePlayed;
DATA(0x00522f80) extern int gbPlayedThrough;
DATA(0x00522f84) extern signed char bMainDone;

#endif // HOMM2_SMACKMGR_H
