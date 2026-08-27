#include <Ints.h>
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

typedef enum BlurConstant {
    SCREEN_WIDTH              = 640,
    BORDER_RADIUS             = 4,
    PALETTE_COLOR_COUNT       = 256,
    PALETTE_BYTE_COUNT        = PALETTE_COLOR_COUNT * H2EnumIndex(PALETTE_CHANNEL_COUNT),
    PALETTE_COMPONENT_MAXIMUM = 0x3f,
    LOOKUP_BYTE_COUNT         = 0x8000,
    COMPONENT_SHIFT           = 5,
    RED_INDEX_SHIFT           = 10,
    GREEN_INDEX_SHIFT         = 5,
    SOUND_POLL_MASK           = 0x3f,
    FIZZLE_DELAY              = 150
} BlurConstant;


#define BLUR_TAP_SUM(table, at)                                                              \
    (table[(at)[1]] + table[(at)[2]] + table[(at)[3]] + table[(at)[4]] + table[(at)[-1]]      \
     + table[(at)[-2]] + table[(at)[-3]] + table[(at)[-4]] + table[(at)[SCREEN_WIDTH]]        \
     + table[(at)[SCREEN_WIDTH * 2]] + table[(at)[SCREEN_WIDTH * 3]]                          \
     + table[(at)[SCREEN_WIDTH * 4]] + table[(at)[-SCREEN_WIDTH]]                             \
     + table[(at)[-SCREEN_WIDTH * 2]] + table[(at)[-SCREEN_WIDTH * 3]]                        \
     + table[(at)[-SCREEN_WIDTH * 4]])

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
    i8* oldPalette0;
    i8* newPalette;
    bitmap* saved_i;

    PollSound();
    gpMouseManager->HideColorPointer();
    gpWindowManager->SaveFizzleSource(0, 0, SCREEN_WIDTH, height);

    saved_i = new bitmap(BITMAP_TYPE_NONE, SCREEN_WIDTH, static_cast<i16>(height));
    memcpy(saved_i->m_pixels, source->m_pixels, height * SCREEN_WIDTH);

    lookupTable = static_cast<u8*>(H2_ALLOC(LOOKUP_BYTE_COUNT));
    for (i = 0; i < PALETTE_COLOR_COUNT; i++) {
        redTable[i] =
            static_cast<u8>(gpBufferPalette->m_data[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)]);
        greenTable[i] =
            static_cast<u8>(gpBufferPalette->m_data[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 1]);
        blueTable[i] =
            static_cast<u8>(gpBufferPalette->m_data[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 2]);
    }

    gpResourceManager->PointToFile(gpResourceManager->MakeId("RGBLOOKP.BIN", 1));
    gpResourceManager->ReadBlock(lookupTable, LOOKUP_BYTE_COUNT);
    memcpy(destination->m_pixels, source->m_pixels, height * SCREEN_WIDTH);
    PollSound();

    for (y = BORDER_RADIUS; y < height - BORDER_RADIUS; y++) {
        if ((y & SOUND_POLL_MASK) == SOUND_POLL_MASK)
            PollSound();

        u8* input   = destination->m_pixels + y * SCREEN_WIDTH + BORDER_RADIUS;
        u8* output2 = source->m_pixels + y * SCREEN_WIDTH + BORDER_RADIUS;

        for (x = BORDER_RADIUS; x < SCREEN_WIDTH - BORDER_RADIUS; x++) {
            blendIndex = BLUR_TAP_SUM(redTable, input) >> COMPONENT_SHIFT << RED_INDEX_SHIFT;
            blendIndex += BLUR_TAP_SUM(greenTable, input) >> COMPONENT_SHIFT
                          << GREEN_INDEX_SHIFT;
            blendIndex += BLUR_TAP_SUM(blueTable, input) >> COMPONENT_SHIFT;
            *output2 = *(lookupTable + blendIndex);
            input++;
            output2++;
        }
    }

    PollSound();
    oldPalette0 = static_cast<i8*>(H2_ALLOC(PALETTE_BYTE_COUNT));
    newPalette  = static_cast<i8*>(H2_ALLOC(PALETTE_BYTE_COUNT));
    memcpy(oldPalette0, gPalette->m_data, PALETTE_BYTE_COUNT);

    for (i = 0; i < PALETTE_COLOR_COUNT; i++) {
        newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)] =
            oldPalette0[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)] + redAdjust;
        newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 1] =
            oldPalette0[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 1] + greenAdjust;
        newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 2] =
            oldPalette0[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 2] + blueAdjust;
        if (newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)] > PALETTE_COMPONENT_MAXIMUM)
            newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)] = PALETTE_COMPONENT_MAXIMUM;
        if (newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)] < 0)
            newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)] = 0;
        if (newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 1] > PALETTE_COMPONENT_MAXIMUM)
            newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 1] = PALETTE_COMPONENT_MAXIMUM;
        if (newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 1] < 0)
            newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 1] = 0;
        if (newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 2] > PALETTE_COMPONENT_MAXIMUM)
            newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 2] = PALETTE_COMPONENT_MAXIMUM;
        if (newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 2] < 0)
            newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + 2] = 0;
    }

    gpWindowManager
        ->FizzleForward(0, 0, SCREEN_WIDTH, height, FIZZLE_DELAY, oldPalette0, newPalette);
    DelayMilli(static_cast<i32l>(350.0f * gfCombatSpeedMod[gConfig.combatSpeed]));
    gpWindowManager->SaveFizzleSource(0, 0, SCREEN_WIDTH, height);
    memcpy(source->m_pixels, saved_i->m_pixels, height * SCREEN_WIDTH);
    gpWindowManager
        ->FizzleForward(0, 0, SCREEN_WIDTH, height, FIZZLE_DELAY, newPalette, oldPalette0);
    H2_FREE(lookupTable);
    delete saved_i;
    gpMouseManager->ShowColorPointer();
    H2_FREE(oldPalette0);
    H2_FREE(newPalette);
}

#undef BLUR_TAP_SUM
