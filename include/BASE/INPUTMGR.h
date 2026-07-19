#ifndef HOMM2_INPUTMGR_H
#define HOMM2_INPUTMGR_H

#include <Ints.h>

i32 KeyboardMessageHandler(void*, u32, u32, i32l);
i32 MouseMessageHandler(void*, u32, u32, i32l);
void CheckChangeCursor(i32, i32, i32);

#endif
