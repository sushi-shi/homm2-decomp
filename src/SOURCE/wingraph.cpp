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
    platform::DisplaySettings settings;
    settings.fullscreen = gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen != 0;
    platform::ReadDisplaySettings(platform::Files(), settings);
    mode.fullscreen = settings.fullscreen;
    mode.scaling = settings.scaling;
    mode.vsync = settings.vsync;
    if (!platform::Video().Open(mode)) {
        ShutDown("Heroes II could not open a display.");
        return;
    }
    gDisplayOpen = true;

    // Retail chose between DirectDraw and WinG here. There is one path now.
    giGraphicsType = WINGRAPH_GRAPHICS_WING;
    gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen = platform::Video().Settings().fullscreen;
    platform::SetMenuVisible(!gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen);
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

void SetFullScreenStatus(b32 fullScreen) {
    if (gDisplayOpen && platform::Video().SetFullscreen(fullScreen != 0)) {
        gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen = platform::Video().Settings().fullscreen;
        platform::SetMenuVisible(!gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen);
    }
}

void ChangeDisplaySettings(bool scaling, bool vsync) {
    if (!gDisplayOpen) return;
    const platform::DisplaySettings before = platform::Video().Settings();
    bool changed = false;
    if (scaling) {
        const platform::Scaling next = before.scaling == platform::Scaling::Nearest
            ? platform::Scaling::Linear : before.scaling == platform::Scaling::Linear
            ? platform::Scaling::Integer : platform::Scaling::Nearest;
        changed = platform::Video().SetScaling(next);
    } else if (vsync) {
        changed = platform::Video().SetVSync(!before.vsync);
    } else {
        changed = platform::Video().SetFullscreen(!before.fullscreen);
    }
    if (!changed) {
        platform::Host().Log(platform::LogLevel::Warning, "The requested display setting is unavailable.");
        return;
    }
    const platform::DisplaySettings current = platform::Video().Settings();
    gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen = current.fullscreen;
    platform::SetMenuVisible(!current.fullscreen);
    if (!platform::WriteDisplaySettings(platform::Files(), current))
        platform::Host().Log(platform::LogLevel::Warning, "Could not save display preferences.");
    platform::Host().Log(platform::LogLevel::Info, current.fullscreen ? "Display: fullscreen" : "Display: windowed");
    platform::Host().Log(platform::LogLevel::Info, current.vsync ? "VSync: on" : "VSync: off");
    platform::Host().Log(platform::LogLevel::Info,
        current.scaling == platform::Scaling::Nearest ? "Display scaling: nearest"
        : current.scaling == platform::Scaling::Linear ? "Display scaling: linear"
                                                    : "Display scaling: integer");
}
