#include <va.h>
#include <BASE/Blur.h>
#include <BASE/bitmap.h>
#include <BASE/palette.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>

H2_ENUM_BEGIN(BlurConstant)
    SCREEN_WIDTH              = 640,
    BORDER_RADIUS             = 4,
    PALETTE_COLOR_COUNT       = 256,
    PALETTE_BYTE_COUNT        = PALETTE_COLOR_COUNT * IDX(PALETTE_CHANNEL_COUNT),
    PALETTE_COMPONENT_MAXIMUM = 0x3f,
    LOOKUP_BYTE_COUNT         = 0x8000,
    COMPONENT_SHIFT           = 5,
    RED_INDEX_SHIFT           = 10,
    GREEN_INDEX_SHIFT         = 5,
    SOUND_POLL_MASK           = 0x3f,
    FIZZLE_DELAY              = 150
H2_ENUM_END(BlurConstant)

// The kernel is the four nearest pixels in each of the four directions; every
// channel sums the same sixteen taps through its own palette component table.
#define BLUR_TAP_SUM(table, at)                                                              \
    (table[(at)[1]] + table[(at)[2]] + table[(at)[3]] + table[(at)[4]] + table[(at)[-1]]      \
     + table[(at)[-2]] + table[(at)[-3]] + table[(at)[-4]] + table[(at)[SCREEN_WIDTH]]        \
     + table[(at)[SCREEN_WIDTH * 2]] + table[(at)[SCREEN_WIDTH * 3]]                          \
     + table[(at)[SCREEN_WIDTH * 4]] + table[(at)[-SCREEN_WIDTH]]                             \
     + table[(at)[-SCREEN_WIDTH * 2]] + table[(at)[-SCREEN_WIDTH * 3]]                        \
     + table[(at)[-SCREEN_WIDTH * 4]])

#if H2_RETAIL_COMPILER
#define oldPalette oldPalette0
#define outputPixel output2
#define savedBitmap saved_i
#endif
VA(0x004cba60, 0xa22)
void DoBlur(
    bitmap* destination,
    bitmap* source,
    i32 height,
    i32 redAdjust,
    i32 greenAdjust,
    i32 blueAdjust
) {
    u32 blendIndex;
    i32 x;
    i32 i;
    i32 y;
    u32 redTable[PALETTE_COLOR_COUNT];
    u32 greenTable[PALETTE_COLOR_COUNT];
    u32 blueTable[PALETTE_COLOR_COUNT];
    u8* lookupTable;
    i8* oldPalette;
    i8* newPalette;
    bitmap* savedBitmap;

    PollSound();
    gpMouseManager->HideColorPointer();
    gpWindowManager->SaveFizzleSource(0, 0, SCREEN_WIDTH, height);

    savedBitmap = new bitmap(BITMAP_TYPE_NONE, SCREEN_WIDTH, static_cast<i16>(height));
    memcpy(savedBitmap->m_pixels, source->m_pixels, height * SCREEN_WIDTH);

    lookupTable = static_cast<u8*>(H2_ALLOC(LOOKUP_BYTE_COUNT));
    for (i = 0; i < PALETTE_COLOR_COUNT; i++) {
        redTable[i] =
            static_cast<u8>(gpBufferPalette->m_data[i * IDX(PALETTE_CHANNEL_COUNT)]);
        greenTable[i] =
            static_cast<u8>(gpBufferPalette->m_data[i * IDX(PALETTE_CHANNEL_COUNT) + 1]);
        blueTable[i] =
            static_cast<u8>(gpBufferPalette->m_data[i * IDX(PALETTE_CHANNEL_COUNT) + 2]);
    }

    gpResourceManager->PointToFile(gpResourceManager->MakeId("RGBLOOKP.BIN", 1));
    gpResourceManager->ReadBlock(lookupTable, LOOKUP_BYTE_COUNT);
    memcpy(destination->m_pixels, source->m_pixels, height * SCREEN_WIDTH);
    PollSound();

    for (y = BORDER_RADIUS; y < height - BORDER_RADIUS; y++) {
        if ((y & SOUND_POLL_MASK) == SOUND_POLL_MASK)
            PollSound();

        u8* input   = destination->m_pixels + y * SCREEN_WIDTH + BORDER_RADIUS;
        u8* outputPixel = source->m_pixels + y * SCREEN_WIDTH + BORDER_RADIUS;

        for (x = BORDER_RADIUS; x < SCREEN_WIDTH - BORDER_RADIUS; x++) {
            blendIndex = BLUR_TAP_SUM(redTable, input) >> COMPONENT_SHIFT << RED_INDEX_SHIFT;
            blendIndex += BLUR_TAP_SUM(greenTable, input) >> COMPONENT_SHIFT
                          << GREEN_INDEX_SHIFT;
            blendIndex += BLUR_TAP_SUM(blueTable, input) >> COMPONENT_SHIFT;
            *outputPixel = *(lookupTable + blendIndex);
            input++;
            outputPixel++;
        }
    }

    PollSound();
    oldPalette = static_cast<i8*>(H2_ALLOC(PALETTE_BYTE_COUNT));
    newPalette  = static_cast<i8*>(H2_ALLOC(PALETTE_BYTE_COUNT));
    memcpy(oldPalette, gPalette->m_data, PALETTE_BYTE_COUNT);

    for (i = 0; i < PALETTE_COLOR_COUNT; i++) {
        newPalette[i * IDX(PALETTE_CHANNEL_COUNT)] =
            oldPalette[i * IDX(PALETTE_CHANNEL_COUNT)] + redAdjust;
        newPalette[i * IDX(PALETTE_CHANNEL_COUNT) + 1] =
            oldPalette[i * IDX(PALETTE_CHANNEL_COUNT) + 1] + greenAdjust;
        newPalette[i * IDX(PALETTE_CHANNEL_COUNT) + 2] =
            oldPalette[i * IDX(PALETTE_CHANNEL_COUNT) + 2] + blueAdjust;
        if (newPalette[i * IDX(PALETTE_CHANNEL_COUNT)] > PALETTE_COMPONENT_MAXIMUM)
            newPalette[i * IDX(PALETTE_CHANNEL_COUNT)] = PALETTE_COMPONENT_MAXIMUM;
        if (newPalette[i * IDX(PALETTE_CHANNEL_COUNT)] < 0)
            newPalette[i * IDX(PALETTE_CHANNEL_COUNT)] = 0;
        if (newPalette[i * IDX(PALETTE_CHANNEL_COUNT) + 1] > PALETTE_COMPONENT_MAXIMUM)
            newPalette[i * IDX(PALETTE_CHANNEL_COUNT) + 1] = PALETTE_COMPONENT_MAXIMUM;
        if (newPalette[i * IDX(PALETTE_CHANNEL_COUNT) + 1] < 0)
            newPalette[i * IDX(PALETTE_CHANNEL_COUNT) + 1] = 0;
        if (newPalette[i * IDX(PALETTE_CHANNEL_COUNT) + 2] > PALETTE_COMPONENT_MAXIMUM)
            newPalette[i * IDX(PALETTE_CHANNEL_COUNT) + 2] = PALETTE_COMPONENT_MAXIMUM;
        if (newPalette[i * IDX(PALETTE_CHANNEL_COUNT) + 2] < 0)
            newPalette[i * IDX(PALETTE_CHANNEL_COUNT) + 2] = 0;
    }

    gpWindowManager
        ->FizzleForward(0, 0, SCREEN_WIDTH, height, FIZZLE_DELAY, oldPalette, newPalette);
    DelayMilli(static_cast<i32l>(350.0f * gfCombatSpeedMod[gConfig.combatSpeed]));
    gpWindowManager->SaveFizzleSource(0, 0, SCREEN_WIDTH, height);
    memcpy(source->m_pixels, savedBitmap->m_pixels, height * SCREEN_WIDTH);
    gpWindowManager
        ->FizzleForward(0, 0, SCREEN_WIDTH, height, FIZZLE_DELAY, newPalette, oldPalette);
    H2_FREE(lookupTable);
    delete savedBitmap;
    gpMouseManager->ShowColorPointer();
    H2_FREE(oldPalette);
    H2_FREE(newPalette);
}
#if H2_RETAIL_COMPILER
#undef oldPalette
#undef outputPixel
#undef savedBitmap
#endif

#undef BLUR_TAP_SUM
