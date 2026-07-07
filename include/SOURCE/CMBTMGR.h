#ifndef HOMM2_CMBTMGR_H
#define HOMM2_CMBTMGR_H
// Declarations of the free functions DEFINED in CMBTMGR.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

int ValidHex(int);
void UpdateCombatSystemOptions(int);
int CombatSystemOptionsHandler(struct tag_message &);

#endif // HOMM2_CMBTMGR_H
