#ifndef HOMM2_MISCWIN_H
#define HOMM2_MISCWIN_H

#include <Ints.h>

class bitmap;

extern "C" void __cdecl BlitBitmapToScreenVesa(class bitmap*, i32, i32, i32, i32, i32, i32);
i16 AutoInitSVGA(void);

struct tagPAINTSTRUCT;

#endif
