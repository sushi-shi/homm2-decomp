#ifndef HOMM2_WINMGR_H
#define HOMM2_WINMGR_H
// Declarations of the free functions DEFINED in WINMGR.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

typedef enum WindowColorCycleMode {
    WINDOW_COLOR_CYCLE_DEFAULT = 0,
    WINDOW_COLOR_CYCLE_COMBAT = 1,
    WINDOW_COLOR_CYCLE_WORLD_VIEW = 2,
    WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE = 3
} WindowColorCycleMode;

typedef enum WindowFadeMode {
    WINDOW_FADE_IN = 0,
    WINDOW_FADE_OUT = 1
} WindowFadeMode;

typedef enum WindowManagerConstant {
    WINDOW_SCREEN_WIDTH = 640,
    WINDOW_SCREEN_HEIGHT = 480,
    WINDOW_PALETTE_COLOR_BYTES = 3,
    WINDOW_PALETTE_BYTE_COUNT = 0x300,
    WINDOW_PALETTE_DWORD_COUNT = 0xc0,
    WINDOW_FRAMEBUFFER_DWORD_COUNT = 0x12c00,
    WINDOW_CYCLE_FRAME_COUNT = 8,
    WINDOW_ALTERNATE_CYCLE_FRAME_COUNT = 6,
    WINDOW_CYCLE_PALETTE_OFFSET = 0x282,
    WINDOW_CYCLE_PALETTE_BYTES = 84,
    WINDOW_COMBAT_CYCLE_SOURCE_OFFSET = 0x14a,
    WINDOW_ALTERNATE_CYCLE_SOURCE_OFFSET = 0x144,
    WINDOW_FIZZLE_DEFAULT_DELAY = 150,
    WINDOW_FIZZLE_CYCLE_TABLE_BYTES = 0x10000
} WindowManagerConstant;

void CycleColors(int);
void CreateFizzleTables(void);
void CreateColorTables(void);
void CreateColorLookupTables(void);

#endif // HOMM2_WINMGR_H
