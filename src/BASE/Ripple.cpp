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
    i32 index;
    u8 previous[SCREEN_WIDTH];
    i32 deadline;
    i32 blitWidth;
    i32 column;
    i32 blitX;
    i32 sourceRow;
    i32 sweepPosition;

    gpMouseManager->HideColorPointer();

    u8 rippleProfile[PROFILE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3,
                                      4, 5, 6, 6, 6, 7, 7, 7, 7, 7, 6, 6, 6, 5, 4, 3, 3, 2,
                                      2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
    i32 step = SWEEP_STEP;

    memset(previous, 0, sizeof(previous));

    for (sweepPosition = -PROFILE_RADIUS; sweepPosition < SWEEP_END; sweepPosition += step) {
        PollSound();
        deadline =
            KBTickCount() + static_cast<i32>(9.0f * gfCombatSpeedMod[gConfig.combatSpeed]);

        for (index = 0; index <= PROFILE_SIZE - 1; index++) {
            column = sweepPosition + index - PROFILE_RADIUS;
            if (column < 0 || column >= SCREEN_WIDTH)
                continue;
            if (rippleProfile[index] == previous[column])
                continue;

            u8* destinationPixel = destination->m_pixels + column;
            u8* sourcePixel =
                source->m_pixels + column + rippleProfile[index] * SCREEN_WIDTH * strength;

            sourceRow = rippleProfile[index] * strength;
            for (; sourceRow < height; sourceRow++) {
                *destinationPixel = *sourcePixel;
                if (sourceRow + 1 == height)
                    break;
                destinationPixel += SCREEN_WIDTH;
                sourcePixel += SCREEN_WIDTH;
            }
            previous[column] = rippleProfile[index];
        }

        blitX    = sweepPosition - REDRAW_RADIUS - step;
        blitWidth = step + REDRAW_WIDTH;
        if (blitX < 0) {
            blitWidth += blitX;
            blitX = 0;
        }
        if (blitX + blitWidth > SCREEN_WIDTH)
            blitWidth = SCREEN_WIDTH - blitX;
        if (blitWidth < 1)
            continue;

        BlitBitmapToScreen(gpWindowManager->m_screen, blitX, 0, blitWidth, height, blitX, 0);
        DelayTil(&deadline);
    }

    gpMouseManager->ShowColorPointer();
}
