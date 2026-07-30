#include <Ints.h>
#include <BASE/miscwin.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>

typedef enum VesaBlitConstant {
    VESA_SCREEN_WIDTH    = 640,
    VESA_SCREEN_HEIGHT   = 480,
    ENLARGE_EXTENT_LIMIT = VESA_SCREEN_WIDTH - 3,
    ENLARGE_PIXEL_GROWTH = 4,
    NET_BOX_TOP          = 411
} VesaBlitConstant;

extern "C" void __fastcall BlitBitmapToScreenVesa(
    bitmap* sourceBitmap,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height,
    i32 destinationX,
    i32 destinationY
) {
    if (gpWindowManager->m_screen != sourceBitmap) {
        i32 row = 0;
        if (height > row) {
            i32 destinationOffset = destinationY * VESA_SCREEN_WIDTH;
            do {
                memcpy(
                    gpWindowManager->m_screen->m_pixels + destinationX + destinationOffset,
                    sourceBitmap->m_pixels + sourceX + (sourceY + row) * sourceBitmap->m_width,
                    width
                );
                destinationOffset += VESA_SCREEN_WIDTH;
                ++row;
            } while (row < height);
        }
    }

    if (gbEnlargeScreenBlit != 0 && gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen == 0) {
        const platform::Size screen = platform::Video().Resolution();
        if (screen.width == VESA_SCREEN_WIDTH && screen.height == VESA_SCREEN_HEIGHT) {
            if (width < VESA_SCREEN_WIDTH)
                ++width;
            if (height < VESA_SCREEN_WIDTH)
                ++height;
        } else {
            if (destinationX > 0)
                --destinationX;
            if (destinationY > 0)
                --destinationY;
            if (width < ENLARGE_EXTENT_LIMIT)
                width += ENLARGE_PIXEL_GROWTH;
            if (height < ENLARGE_EXTENT_LIMIT)
                height += ENLARGE_PIXEL_GROWTH;
        }
    }

    if (gbLeaveNetBoxAlone != 0) {
        if (destinationY >= NET_BOX_TOP)
            return;
        if (destinationY + height >= NET_BOX_TOP)
            height = NET_BOX_TOP - destinationY;
    }

    platform::Video().Blit({destinationX, destinationY, width, height},
                           {destinationX, destinationY});
    platform::Video().Present();
}

i16 AutoInitSVGA(void) {
    return 0;
}
