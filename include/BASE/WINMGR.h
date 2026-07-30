#ifndef HOMM2_WINMGR_H
#define HOMM2_WINMGR_H

#include <Ints.h>

enum class WindowColorCycleMode : i32 {
    WINDOW_COLOR_CYCLE_DEFAULT          = 0,
    WINDOW_COLOR_CYCLE_COMBAT           = 1,
    WINDOW_COLOR_CYCLE_WORLD_VIEW       = 2,
    WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE = 3
};
using enum WindowColorCycleMode;

typedef enum WindowManagerConstant {
    WINDOW_CYCLE_PALETTE_BYTES = 0x60
} WindowManagerConstant;

void CycleColors(i32);
void CreateFizzleTables(void);
void CreateColorTables(void);
void CreateColorLookupTables(void);

#endif
