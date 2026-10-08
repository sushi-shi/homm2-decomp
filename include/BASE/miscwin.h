#ifndef HOMM2_BASE_MISCWIN_H
#define HOMM2_BASE_MISCWIN_H

#include <H2/Ints.h>

class bitmap;

extern "C" void __cdecl BlitBitmapToScreenVesa(class bitmap* sourceBitmap, i32 sourceX, i32 sourceY, i32 width, i32 height, i32 destinationX, i32 destinationY);
i16 AutoInitSVGA(void);

struct tagPAINTSTRUCT;
extern struct tagPAINTSTRUCT ps;

#endif // HOMM2_BASE_MISCWIN_H
