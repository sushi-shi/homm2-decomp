#ifndef HOMM2_SOURCE_WINGRAPH_H
#define HOMM2_SOURCE_WINGRAPH_H

#include <Ints.h>

enum GraphicsConstant : i32 {
    GRAPHICS_WIDTH = 640,
    GRAPHICS_HEIGHT = 480,
    GRAPHICS_COLOR_DEPTH = 8,
    GRAPHICS_PALETTE_SIZE = 256,
    GRAPHICS_SYSTEM_PALETTE_SIZE = 10,
};

enum class WingraphGraphicsType : i32 {
    WINGRAPH_GRAPHICS_WING = 1,
    WINGRAPH_GRAPHICS_DIRECT_DRAW = 2,
};
using enum WingraphGraphicsType;

void GetGraphicsInfo();
void InitGraphics();
void UpdatePalette(i8* palette);
void CleanUpWinGraphics();
void SetFullScreenStatus(i32 fullscreen);

extern WingraphGraphicsType giGraphicsType;

#endif
