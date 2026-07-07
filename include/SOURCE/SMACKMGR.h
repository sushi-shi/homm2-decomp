#ifndef HOMM2_SMACKMGR_H
#define HOMM2_SMACKMGR_H
// Declarations of the free functions DEFINED in SMACKMGR.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
#include <_all.h>

void ConvertSmackerPalette(unsigned char *);
void DoAdvance(struct SmackTag *, int, int, int, int);
void SmackManagerMain(void);
void ShutDownSmacker(void);
int PlaySmacker(int);
int ExpansionCampaignRect(int, int);
signed char PointInRect(int, int, struct tag_rect *);
void PrintSummaryInfo(struct SmackSumTag *);

#endif // HOMM2_SMACKMGR_H
