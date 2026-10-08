#ifndef HOMM2_SOURCE_WINGRAPH_H
#define HOMM2_SOURCE_WINGRAPH_H

#include <H2/Ints.h>
#include <Domains.h>
#include <BASE/display.h>

enum GraphicsConstant : i32 {
    GRAPHICS_WIDTH = 640,
    GRAPHICS_HEIGHT = 480,
    GRAPHICS_COLOR_DEPTH = 8,
    GRAPHICS_PALETTE_SIZE = 256,
    GRAPHICS_SYSTEM_PALETTE_SIZE = 10,
    PALETTE_VALUE_SHIFT = 2,
};

enum class WingraphGraphicsType : i32 {
    WINGRAPH_GRAPHICS_WING = 1,
    WINGRAPH_GRAPHICS_DIRECT_DRAW = 2,
};
using enum WingraphGraphicsType;

void GetGraphicsInfo();
void InitGraphics();
void UpdatePalette(i8* paletteData);
void CleanUpWinGraphics();
void SetFullScreenStatus(b32 fullScreen);
void ChangeDisplaySettings(bool scaling, bool vsync);

extern WingraphGraphicsType giGraphicsType;

#endif
