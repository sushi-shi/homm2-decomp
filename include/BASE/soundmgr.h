#ifndef HOMM2_SOUNDMGR_H
#define HOMM2_SOUNDMGR_H
// Declarations of the free functions DEFINED in soundmgr.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

void HandleMCIError(int, char *);
void __stdcall SetReady2Poll(unsigned long int);
void __stdcall UpdateTimers(unsigned long int);
struct _DIG_DRIVER * WAVE_init_driver(unsigned long int, unsigned short int, unsigned short int, unsigned short int);

#endif // HOMM2_SOUNDMGR_H
