#ifndef HOMM2_SOUNDMGR_H
#define HOMM2_SOUNDMGR_H

#include <Ints.h>
// Declarations of the free functions DEFINED in soundmgr.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

void HandleMCIError(i32, char*);
void __stdcall SetReady2Poll(u32l);
void __stdcall UpdateTimers(u32l);
struct _DIG_DRIVER* WAVE_init_driver(u32l, u16, u16, u16);

#endif // HOMM2_SOUNDMGR_H
