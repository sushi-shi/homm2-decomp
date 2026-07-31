#ifndef HOMM2_SOUNDMGR_H
#define HOMM2_SOUNDMGR_H

#include <Ints.h>

void __stdcall SetReady2Poll(u32l);
void __stdcall UpdateTimers(u32l);
i32 WAVE_init_driver(u32l, u16, u16, u16);

#endif
