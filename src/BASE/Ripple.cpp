#include <Ints.h>
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

typedef enum RippleConstant {
    SCREEN_WIDTH   = 640,
    PROFILE_RADIUS = 25,
    PROFILE_SIZE   = PROFILE_RADIUS * 2 + 1,


    REDRAW_RADIUS = 18,
    REDRAW_WIDTH  = 37,
    SWEEP_STEP    = 4,
    SWEEP_END     = SCREEN_WIDTH + PROFILE_RADIUS
} RippleConstant;

void DoRipple(bitmap* source, bitmap* destination, i32 height, i32 strength) {
    i32 idx;
    u8 previous[SCREEN_WIDTH];
    i32 deadline7;
    i32 blitWidth;
    i32 column7;
    i32 blitX3;
    i32 srcRow;
    i32 sweepPosition;

    gpMouseManager->HideColorPointer();

    u8 rippleProfile[PROFILE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3,
                                      4, 5, 6, 6, 6, 7, 7, 7, 7, 7, 6, 6, 6, 5, 4, 3, 3, 2,
                                      2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
    i32 step7 = SWEEP_STEP;

    memset(previous, 0, sizeof(previous));

    for (sweepPosition = -PROFILE_RADIUS; sweepPosition < SWEEP_END; sweepPosition += step7) {
        PollSound();
        deadline7 =
            KBTickCount() + static_cast<i32>(9.0f * gfCombatSpeedMod[gConfig.combatSpeed]);

        for (idx = 0; idx <= PROFILE_SIZE - 1; idx++) {
            column7 = sweepPosition + idx - PROFILE_RADIUS;
            if (column7 < 0 || column7 >= SCREEN_WIDTH)
                continue;
            if (rippleProfile[idx] == previous[column7])
                continue;

            u8* destinationPixel = destination->m_pixels + column7;
            u8* sourcePixel =
                source->m_pixels + column7 + rippleProfile[idx] * SCREEN_WIDTH * strength;

            srcRow = rippleProfile[idx] * strength;
            for (; srcRow < height; srcRow++) {
                *destinationPixel = *sourcePixel;
                if (srcRow + 1 == height)
                    break;
                destinationPixel += SCREEN_WIDTH;
                sourcePixel += SCREEN_WIDTH;
            }
            previous[column7] = rippleProfile[idx];
        }

        blitX3    = sweepPosition - REDRAW_RADIUS - step7;
        blitWidth = step7 + REDRAW_WIDTH;
        if (blitX3 < 0) {
            blitWidth += blitX3;
            blitX3 = 0;
        }
        if (blitX3 + blitWidth > SCREEN_WIDTH)
            blitWidth = SCREEN_WIDTH - blitX3;
        if (blitWidth < 1)
            continue;

        BlitBitmapToScreen(gpWindowManager->m_screen, blitX3, 0, blitWidth, height, blitX3, 0);
        DelayTil(&deadline7);
    }

    gpMouseManager->ShowColorPointer();
}
