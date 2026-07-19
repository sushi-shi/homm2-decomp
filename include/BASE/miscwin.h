#ifndef HOMM2_MISCWIN_H
#define HOMM2_MISCWIN_H

#include <Ints.h>
class bitmap;

extern "C" void __fastcall BlitBitmapToScreenVesa(class bitmap*, i32, i32, i32, i32, i32, i32);
i16 AutoInitSVGA(void);

struct tagPAINTSTRUCT;
extern struct tagPAINTSTRUCT ps;

#endif
