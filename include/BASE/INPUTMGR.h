#ifndef HOMM2_INPUTMGR_H
#define HOMM2_INPUTMGR_H
// Declarations of the free functions DEFINED in INPUTMGR.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
#include <_all.h>

int KeyboardMessageHandler(void *, unsigned int, unsigned int, long int);
int MouseMessageHandler(void *, unsigned int, unsigned int, long int);
void CheckChangeCursor(int, int, int);

#endif // HOMM2_INPUTMGR_H
