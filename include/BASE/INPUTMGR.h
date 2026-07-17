#ifndef HOMM2_INPUTMGR_H
#define HOMM2_INPUTMGR_H

#include <Ints.h>
// Declarations of the free functions DEFINED in INPUTMGR.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

i32 KeyboardMessageHandler(void *, u32, u32, i32l);
i32 MouseMessageHandler(void *, u32, u32, i32l);
void CheckChangeCursor(i32, i32, i32);

#endif // HOMM2_INPUTMGR_H
