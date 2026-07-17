#ifndef HOMM2_MISCWIN_H
#define HOMM2_MISCWIN_H

#include <Ints.h>
// Declarations of the free functions DEFINED in miscwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
class bitmap;

extern "C" void __fastcall BlitBitmapToScreenVesa(class bitmap*, i32, i32, i32, i32, i32, i32);
i16 AutoInitSVGA(void);

struct tagPAINTSTRUCT;
extern struct tagPAINTSTRUCT ps;

#endif // HOMM2_MISCWIN_H
