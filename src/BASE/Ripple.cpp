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
    // The profile is flat over its outer seven samples, so only the middle
    // REDRAW_WIDTH columns can change; the sweep step widens that span.
    REDRAW_RADIUS = 18,
    REDRAW_WIDTH  = 37,
    SWEEP_STEP    = 4,
    SWEEP_END     = SCREEN_WIDTH + PROFILE_RADIUS
H2_ENUM_END(RippleConstant)

#if H2_RETAIL_COMPILER
#define blitX blitX3
#define column column7
#define deadline deadline7
#define sourceRow srcRow
#define step step7
#endif
VA(0x004cb6b0, 0x35e)
void DoRipple(bitmap* source, bitmap* destination, i32 height, i32 strength) {
    i32 idx;
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

        for (idx = 0; idx <= PROFILE_SIZE - 1; idx++) {
            column = sweepPosition + idx - PROFILE_RADIUS;
            if (column < 0 || column >= SCREEN_WIDTH)
                continue;
            if (rippleProfile[idx] == previous[column])
                continue;

            u8* destinationPixel = destination->m_pixels + column;
            u8* sourcePixel =
                source->m_pixels + column + rippleProfile[idx] * SCREEN_WIDTH * strength;

            sourceRow = rippleProfile[idx] * strength;
            for (; sourceRow < height; sourceRow++) {
                *destinationPixel = *sourcePixel;
                if (sourceRow + 1 == height)
                    break;
                destinationPixel += SCREEN_WIDTH;
                sourcePixel += SCREEN_WIDTH;
            }
            previous[column] = rippleProfile[idx];
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
#if H2_RETAIL_COMPILER
#undef blitX
#undef column
#undef deadline
#undef sourceRow
#undef step
#endif
