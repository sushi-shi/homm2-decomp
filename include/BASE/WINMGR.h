#ifndef HOMM2_WINMGR_H
#define HOMM2_WINMGR_H

#include <Ints.h>

H2_ENUM_CLASS_BEGIN(WindowColorCycleMode)
    WINDOW_COLOR_CYCLE_DEFAULT          = 0,
    WINDOW_COLOR_CYCLE_COMBAT           = 1,
    WINDOW_COLOR_CYCLE_WORLD_VIEW       = 2,
    WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE = 3
H2_ENUM_CLASS_END(WindowColorCycleMode)

H2_ENUM_BEGIN(WindowManagerConstant)
    WINDOW_CYCLE_PALETTE_BYTES = 0x60
H2_ENUM_END(WindowManagerConstant)

void CycleColors(i32);
void CreateFizzleTables(void);
void CreateColorTables(void);
void CreateColorLookupTables(void);

#endif
