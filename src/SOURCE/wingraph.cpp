#include <Ints.h>
#include <BASE/Misc.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

#include <SOURCE/wingraph.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>

typedef enum PaletteConstant {
    PALETTE_COMPONENT_COUNT = 3,
    PALETTE_RED_COMPONENT   = 0,
    PALETTE_GREEN_COMPONENT = 1,
    PALETTE_BLUE_COMPONENT  = 2,
    PALETTE_VALUE_SHIFT     = 2
} PaletteConstant;

WingraphGraphicsType giGraphicsType = WINGRAPH_GRAPHICS_WING;

namespace {

bool gDisplayOpen = false;

}

// The game asks before the display exists, so answer with what it will get:
// its own 8-bit pipeline at retail's size.
void GetGraphicsInfo(void) {
    giMainVideoModeColorDepth = GRAPHICS_COLOR_DEPTH;
    giMainVideoModeWidth = GRAPHICS_WIDTH;
    giMainVideoModeHeight = GRAPHICS_HEIGHT;
}

void InitGraphics(void) {
    if (gDisplayOpen) {
        return;
    }

    platform::DisplayMode mode;
    mode.width = GRAPHICS_WIDTH;
    mode.height = GRAPHICS_HEIGHT;
    if (!platform::Video().Open(mode)) {
        ShutDown("Heroes II could not open a display.");
        return;
    }
    gDisplayOpen = true;

    // Retail chose between DirectDraw and WinG here. There is one path now.
    giGraphicsType = WINGRAPH_GRAPHICS_WING;
    gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen = false;
    platform::SetMenuVisible(1);
}

// Retail left the ends of the palette to the desktop and only owned the middle.
void UpdatePalette(i8* paletteData) {
    platform::Color entries[GRAPHICS_PALETTE_SIZE];

    const i32 first = GRAPHICS_SYSTEM_PALETTE_SIZE;
    const i32 last = GRAPHICS_PALETTE_SIZE - GRAPHICS_SYSTEM_PALETTE_SIZE;
    for (i32 entry = first; entry < last; entry++) {
        const i8* component = &paletteData[entry * PALETTE_COMPONENT_COUNT];
        entries[entry].r = component[PALETTE_RED_COMPONENT] << PALETTE_VALUE_SHIFT;
        entries[entry].g = component[PALETTE_GREEN_COMPONENT] << PALETTE_VALUE_SHIFT;
        entries[entry].b = component[PALETTE_BLUE_COMPONENT] << PALETTE_VALUE_SHIFT;
    }

    platform::Video().SetPalette(&entries[first], first, last - first);
}

void CleanUpWinGraphics(void) {
    if (!gDisplayOpen) {
        return;
    }
    platform::Video().Close();
    gDisplayOpen = false;
}

// Retail could only go full screen through DirectDraw, and stayed windowed
// without it. Reopening the display would strand the screen bitmap the game
// holds, so the window stays as it is until the backend can switch in place.
void SetFullScreenStatus(i32 fullScreen) {
    static_cast<void>(fullScreen);
}
