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
// @early-stop
// Reloc-masked: only code-byte residual is +0x156, SIB 08 vs 01 for the same
// profileIndex + position LEA; all 13 relocation targets agree.
VA(0x004d26a0, 0x23f)
void DoRipple(bitmap *source, bitmap *destination, int height, int strength)
{
    const int screenWidth = 0x280;
    const int profileRadius = 0x19;
    const int redrawRadius = 0x16;
    const int redrawWidth = 0x29;

    gpMouseManager->HideColorPointer();

    unsigned char profile[51] = {
        0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1,
        2, 2, 2, 2,
        3, 3, 4, 5,
        6, 6, 6,
        7, 7, 7, 7, 7,
        6, 6, 6, 5, 4, 3, 3,
        2, 2, 2, 2,
        1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0
    };
    unsigned char previous[screenWidth];
    memset(previous, 0, sizeof(previous));
    int position = -profileRadius;

    do {
        PollSound();
        int deadline = KBTickCount() +
            gfCombatSpeedMod[gConfig.combatSpeed] * 9.0f;

        for (int profileIndex = 0; profileIndex <= 50; ++profileIndex) {
            int column = profileIndex + position - profileRadius;
            if (column < 0 || column >= screenWidth)
                continue;

            if (previous[column] == profile[profileIndex])
                continue;

            unsigned char *destinationPixel = destination->m_pixels + column;
            int sourceRow = profile[profileIndex] * strength;
            unsigned char *sourcePixel = source->m_pixels + sourceRow * screenWidth + column;

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

        int redrawX = position - redrawRadius;
        int width = redrawWidth;
        if (redrawX < 0) {
            width += redrawX;
            redrawX = 0;
        }
        if (redrawX + width > screenWidth)
            width = screenWidth - redrawX;
        if (width >= 1) {
            BlitBitmapToScreen(gpWindowManager->m_screen,
                               redrawX, 0, width, height, redrawX, 0);
            DelayTil(&deadline);
        }
        position += 4;
    } while (position < 0x299);

    gpMouseManager->ShowColorPointer();
}
