// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\miscwin.obj   from: .\basewin.lib
// functions: 2   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/miscwin.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/X_GLOBAL.h>
#include <windows.h>
#include <string.h>

// @semantic
// Complete /O2 blit and invalidation checkpoint. Base and retail are both 0x1e2 bytes
// with the same CFG and all 18 resolved relocation targets/occurrences. First divergence
// is the destination-address association in the row copy: base adds the row offset before
// destinationX, while retail adds destinationX first and then uses LEA for the row offset.
// The remaining residuals are an equivalent loop comparison and operand order in the
// left/top scaling multiplies. Commuted loop/product operands, an explicit row destination
// pointer, and explicit scaled-coordinate lifetimes were byte-neutral in three audited
// variants. Revisit after a real predecessor/header/compiler-state change.
VA(0x004d8540, 0x1e2)
extern "C" void __fastcall BlitBitmapToScreenVesa(int bitmapAddress, int sourceX, int sourceY,
                                                   int width, int height, int destinationX,
                                                   int destinationY)
{
    bitmap *sourceBitmap = reinterpret_cast<bitmap *>(bitmapAddress);

    if (gpWindowManager->m_screen != sourceBitmap) {
        int row = 0;
        if (height > row) {
            int destinationOffset = destinationY * 640;
            do {
                memcpy(gpWindowManager->m_screen->m_pixels + destinationX + destinationOffset,
                       sourceBitmap->m_pixels + sourceX + (sourceY + row) * sourceBitmap->m_width,
                       width);
                destinationOffset += 640;
                ++row;
            } while (row < height);
        }
    }

    if (gbEnlargeScreenBlit != 0 && gConfig.gfx[giCurExe].fullScreen == 0) {
        if (iMainWinScreenWidth == 640 && iMainWinScreenHeight == 480) {
            if (width < 640)
                ++width;
            if (height < 640)
                ++height;
        } else {
            if (destinationX > 0)
                --destinationX;
            if (destinationY > 0)
                --destinationY;
            if (width < 637)
                width += 4;
            if (height < 637)
                height += 4;
        }
    }

    if (gbLeaveNetBoxAlone != 0) {
        if (destinationY >= 411)
            return;
        if (destinationY + height >= 411)
            height = 411 - destinationY;
    }

    RECT invalidRect;
    invalidRect.left = destinationX * iMainWinScreenWidth / 640;
    invalidRect.top = destinationY * iMainWinScreenHeight / 480;
    invalidRect.right = (destinationX + width) * iMainWinScreenWidth / 640 - 1;
    invalidRect.bottom = (destinationY + height) * iMainWinScreenHeight / 480 - 1;

    if (InvalidateRect(hwndApp, &invalidRect, 0) == 0)
        LogStr("InvalidateRect Failed");
    if (UpdateWindow(hwndApp) == 0)
        LogStr("UpdateWindow Failed");
}

VA(0x004d8730, 0x4)
short int AutoInitSVGA(void) { return 0; }

// ---- globals (definitions, RVA order) ----
DATA(0x00538080) struct tagPAINTSTRUCT ps;
