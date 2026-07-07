#ifndef HOMM2_TOWNMGR_H
#define HOMM2_TOWNMGR_H
// Declarations of the free functions DEFINED in TOWNMGR.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

int MageGuildHandler(struct tag_message &);
int TavernHandler(struct tag_message &);
int SplitArmyHandler(struct tag_message &);
void GetCategoryStats(int, long int * const, signed char * const);
void SortStats(long int * const, signed char * const);

#endif // HOMM2_TOWNMGR_H
