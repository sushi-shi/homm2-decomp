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

H2_ENUM_BEGIN(RippleConstant)
    SCREEN_WIDTH   = 640,
    PROFILE_RADIUS = 25,
    PROFILE_SIZE   = PROFILE_RADIUS * 2 + 1,
    REDRAW_RADIUS  = 22,
    REDRAW_WIDTH   = 41,
    SWEEP_STEP     = 4,
    SWEEP_END      = SCREEN_WIDTH + PROFILE_RADIUS
H2_ENUM_END(RippleConstant)

VA(0x004d26a0, 0x23f)
void DoRipple(bitmap* source, bitmap* destination, i32 height, i32 strength) {
    i32 profileIndex;

    gpMouseManager->HideColorPointer();

    // Retail stack-local displacement samples are data, not symbolic domains.
    // NOLINTBEGIN(readability-magic-numbers)
    u8 profile[PROFILE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4, 5, 6, 6, 6, 7, 7, 7,
                                7, 7, 6, 6, 6, 5, 4, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
    // NOLINTEND(readability-magic-numbers)
    u8 previous[SCREEN_WIDTH];
    memset(previous, 0, sizeof(previous));
    i32 position = -PROFILE_RADIUS;

    do {
        PollSound();
        // Keep the retail anonymous constant-pool relocation.
        i32 deadline = KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * 9.0f; // NOLINT(readability-magic-numbers)

        for (profileIndex = 0; profileIndex <= PROFILE_SIZE - 1; ++profileIndex) {
            i32 column = profileIndex + position - PROFILE_RADIUS;
            if (column < 0 || column >= SCREEN_WIDTH)
                continue;

            if (previous[column] == profile[profileIndex])
                continue;

            u8* destinationPixel = destination->m_pixels + column;
            i32 sourceRow = profile[profileIndex] * strength;
            u8* sourcePixel = source->m_pixels + sourceRow * SCREEN_WIDTH + column;

            if (height > sourceRow) {
                do {
                    *destinationPixel = *sourcePixel;
                    if (sourceRow - height == -1)
                        break;
                    destinationPixel += SCREEN_WIDTH;
                    sourcePixel += SCREEN_WIDTH;
                    ++sourceRow;
                } while (height > sourceRow);
            }
            previous[column] = profile[profileIndex];
        }

        i32 redrawX = position - REDRAW_RADIUS;
        i32 width = REDRAW_WIDTH;
        if (redrawX < 0) {
            width += redrawX;
            redrawX = 0;
        }
        if (redrawX + width > SCREEN_WIDTH)
            width = SCREEN_WIDTH - redrawX;
        if (width >= 1) {
            BlitBitmapToScreen(gpWindowManager->m_screen, redrawX, 0, width, height, redrawX, 0);
            DelayTil(&deadline);
        }
        position += SWEEP_STEP;
    } while (position < SWEEP_END);

    gpMouseManager->ShowColorPointer();
}
