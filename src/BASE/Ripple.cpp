// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Ripple.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Ripple.h>
#include <BASE/bitmap.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <string.h>
// @semantic
// The complete /O2 kernel has the retail 0x23f extent, 0x2c4 frame, CFG, profile and
// previous-row arrays, pixel/redraw bounds, and all 13 ordered relocation targets and
// addends. Hoisting profileIndex to function scope fixed the +0x156 commutative SIB;
// the first remaining code divergence is +0x196, where retail `cmp esi,eax; jle` and
// ours `cmp eax,esi; jge` encode the same height <= sourceRow exit (repeated at +0x1b8).
// Ten non-improving current-state variants exhausted operand orders and trivial local
// hoists. Revisit only after a material Ripple header/declaration-state change.
VA(0x004d26a0, 0x23f)
void DoRipple(bitmap* source, bitmap* destination, i32 height, i32 strength) {
    i32 profileIndex;
    const i32 screenWidth = 640;
    const i32 profileRadius = 25;
    const i32 redrawRadius = 22;
    const i32 redrawWidth = 41;

    gpMouseManager->HideColorPointer();

    u8 profile[51] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4, 5, 6, 6, 6, 7, 7, 7,
                      7, 7, 6, 6, 6, 5, 4, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
    u8 previous[screenWidth];
    memset(previous, 0, sizeof(previous));
    i32 position = -profileRadius;

    do {
        PollSound();
        i32 deadline = KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * 9.0f;

        for (profileIndex = 0; profileIndex <= 50; ++profileIndex) {
            i32 column = profileIndex + position - profileRadius;
            if (column < 0 || column >= screenWidth)
                continue;

            if (previous[column] == profile[profileIndex])
                continue;

            u8* destinationPixel = destination->m_pixels + column;
            i32 sourceRow = profile[profileIndex] * strength;
            u8* sourcePixel = source->m_pixels + sourceRow * screenWidth + column;

            if (height > sourceRow) {
                do {
                    *destinationPixel = *sourcePixel;
                    if (sourceRow - height == -1)
                        break;
                    destinationPixel += screenWidth;
                    sourcePixel += screenWidth;
                    ++sourceRow;
                } while (height > sourceRow);
            }
            previous[column] = profile[profileIndex];
        }

        i32 redrawX = position - redrawRadius;
        i32 width = redrawWidth;
        if (redrawX < 0) {
            width += redrawX;
            redrawX = 0;
        }
        if (redrawX + width > screenWidth)
            width = screenWidth - redrawX;
        if (width >= 1) {
            BlitBitmapToScreen(gpWindowManager->m_screen, redrawX, 0, width, height, redrawX, 0);
            DelayTil(&deadline);
        }
        position += 4;
    } while (position < 665);

    gpMouseManager->ShowColorPointer();
}
