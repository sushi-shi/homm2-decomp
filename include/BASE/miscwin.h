#ifndef HOMM2_MISCWIN_H
#define HOMM2_MISCWIN_H
// Declarations of the free functions DEFINED in miscwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

extern "C" void __fastcall BlitBitmapToScreenVesa(int, int, int, int, int, int, int);
short int AutoInitSVGA(void);

#endif // HOMM2_MISCWIN_H
