#include <Ints.h>
#include <BASE/miscwin.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/X_GLOBAL.h>
#include <windows.h>
#include <string.h>
#include <BASE/display.h>

typedef enum VesaBlitConstant {
    ENLARGE_EXTENT_LIMIT = LOGICAL_SCREEN_WIDTH - 3,
    ENLARGE_PIXEL_GROWTH = 4,
    NET_BOX_TOP          = 411
} VesaBlitConstant;

static char gInvalidateRectFailedText[] = "InvalidateRect Failed";
static char gUpdateWindowFailedText[] = "UpdateWindow Failed";

extern "C" void __cdecl BlitBitmapToScreenVesa(
    bitmap* sourceBitmap,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height,
    i32 destinationX,
    i32 destinationY
) {
    if (sourceBitmap != gpWindowManager->m_screen) {
        i32 row;
        for (row = 0; row < height; row++)
            memcpy(
                gpWindowManager->m_screen->m_pixels
                    + (destinationY + row) * LOGICAL_SCREEN_WIDTH + destinationX,
                sourceBitmap->m_pixels + (row + sourceY) * sourceBitmap->m_width + sourceX,
                width
            );
    }

    if (gbEnlargeScreenBlit != 0 && CURRENT_GRAPHICS_CONFIG.fullScreen == 0) {
        if (iMainWinScreenWidth == LOGICAL_SCREEN_WIDTH
            && iMainWinScreenHeight == LOGICAL_SCREEN_HEIGHT) {
            if (width < LOGICAL_SCREEN_WIDTH)
                ++width;
            if (height < LOGICAL_SCREEN_WIDTH)
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

    RECT invalidRectangle;
    invalidRectangle.left = destinationX * iMainWinScreenWidth / LOGICAL_SCREEN_WIDTH;
    invalidRectangle.top = destinationY * iMainWinScreenHeight / LOGICAL_SCREEN_HEIGHT;
    invalidRectangle.right =
        (destinationX + width) * iMainWinScreenWidth / LOGICAL_SCREEN_WIDTH - 1;
    invalidRectangle.bottom =
        (destinationY + height) * iMainWinScreenHeight / LOGICAL_SCREEN_HEIGHT - 1;

    if (InvalidateRect(hwndApp, &invalidRectangle, 0) == 0)
        LogStr(gInvalidateRectFailedText);
    if (UpdateWindow(hwndApp) == 0)
        LogStr(gUpdateWindowFailedText);
}

i16 AutoInitSVGA(void) {
    return 0;
}

struct tagPAINTSTRUCT ps;
