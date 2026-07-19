#ifndef HOMM2_SOUNDMGR_H
#define HOMM2_SOUNDMGR_H

#include <Ints.h>

void HandleMCIError(i32, char*);
void __stdcall SetReady2Poll(u32l);
void __stdcall UpdateTimers(u32l);
struct _DIG_DRIVER* WAVE_init_driver(u32l, u16, u16, u16);

#endif
