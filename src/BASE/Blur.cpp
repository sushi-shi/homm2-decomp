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
#include <BASE/display.h>

typedef enum BlurConstant {
    BORDER_RADIUS             = 4,
    LOOKUP_BYTE_COUNT         = 0x8000,
    COMPONENT_SHIFT           = 5,
    RED_INDEX_SHIFT           = 10,
    GREEN_INDEX_SHIFT         = 5,
    SOUND_POLL_MASK           = 0x3f,
    BLUR_HOLD_MILLISECONDS    = 350
} BlurConstant;


#define BLUR_TAP_SUM(table, at)                                                              \
    (table[(at)[1]] + table[(at)[2]] + table[(at)[3]] + table[(at)[4]] + table[(at)[-1]]      \
     + table[(at)[-2]] + table[(at)[-3]] + table[(at)[-4]] + table[(at)[LOGICAL_SCREEN_WIDTH]]        \
     + table[(at)[LOGICAL_SCREEN_WIDTH * 2]] + table[(at)[LOGICAL_SCREEN_WIDTH * 3]]                          \
     + table[(at)[LOGICAL_SCREEN_WIDTH * 4]] + table[(at)[-LOGICAL_SCREEN_WIDTH]]                             \
     + table[(at)[-LOGICAL_SCREEN_WIDTH * 2]] + table[(at)[-LOGICAL_SCREEN_WIDTH * 3]]                        \
     + table[(at)[-LOGICAL_SCREEN_WIDTH * 4]])

void DoBlur(
    bitmap* scratch,
    bitmap* screen,
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
    gpWindowManager->SaveFizzleSource(0, 0, LOGICAL_SCREEN_WIDTH, height);

    savedBitmap = new bitmap(BITMAP_TYPE_NONE, LOGICAL_SCREEN_WIDTH, static_cast<i16>(height));
    memcpy(savedBitmap->m_pixels, screen->m_pixels, height * LOGICAL_SCREEN_WIDTH);

    lookupTable = static_cast<u8*>(H2_ALLOC(LOOKUP_BYTE_COUNT));
    for (i = 0; i < PALETTE_COLOR_COUNT; i++) {
        redTable[i] =
            static_cast<u8>(gpBufferPalette->m_data[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)]);
        greenTable[i] =
            static_cast<u8>(gpBufferPalette->m_data[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)]);
        blueTable[i] =
            static_cast<u8>(gpBufferPalette->m_data[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)]);
    }

    gpResourceManager->PointToFile(gpResourceManager->MakeId("RGBLOOKP.BIN", 1));
    gpResourceManager->ReadBlock(lookupTable, LOOKUP_BYTE_COUNT);
    memcpy(scratch->m_pixels, screen->m_pixels, height * LOGICAL_SCREEN_WIDTH);
    PollSound();

    for (y = BORDER_RADIUS; y < height - BORDER_RADIUS; y++) {
        if ((y & SOUND_POLL_MASK) == SOUND_POLL_MASK)
            PollSound();

        u8* input = scratch->m_pixels + y * LOGICAL_SCREEN_WIDTH + BORDER_RADIUS;
        u8* outputPixel = screen->m_pixels + y * LOGICAL_SCREEN_WIDTH + BORDER_RADIUS;

        for (x = BORDER_RADIUS; x < H2EnumIndex(LOGICAL_SCREEN_WIDTH) - BORDER_RADIUS; x++) {
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
    oldPalette = static_cast<i8*>(H2_ALLOC(PALETTE_DATA_SIZE));
    newPalette = static_cast<i8*>(H2_ALLOC(PALETTE_DATA_SIZE));
    memcpy(oldPalette, gPalette->m_data, PALETTE_DATA_SIZE);

    for (i = 0; i < PALETTE_COLOR_COUNT; i++) {
        newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)] =
            oldPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)] + redAdjust;
        newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)] =
            oldPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)] + greenAdjust;
        newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)] =
            oldPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)] + blueAdjust;
        if (newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)] > PALETTE_CHANNEL_MAX)
            newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)] = PALETTE_CHANNEL_MAX;
        if (newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)] < 0)
            newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT)] = 0;
        if (newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)] > PALETTE_CHANNEL_MAX)
            newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)] = PALETTE_CHANNEL_MAX;
        if (newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)] < 0)
            newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)] = 0;
        if (newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)] > PALETTE_CHANNEL_MAX)
            newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)] = PALETTE_CHANNEL_MAX;
        if (newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)] < 0)
            newPalette[i * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)] = 0;
    }

    gpWindowManager
        ->FizzleForward(0, 0, LOGICAL_SCREEN_WIDTH, height, FIZZLE_DEFAULT_DELAY, oldPalette, newPalette);
    DelayMilli(static_cast<i32l>(static_cast<float>(BLUR_HOLD_MILLISECONDS) * gfCombatSpeedMod[gConfig.combatSpeed]));
    gpWindowManager->SaveFizzleSource(0, 0, LOGICAL_SCREEN_WIDTH, height);
    memcpy(screen->m_pixels, savedBitmap->m_pixels, height * LOGICAL_SCREEN_WIDTH);
    gpWindowManager
        ->FizzleForward(0, 0, LOGICAL_SCREEN_WIDTH, height, FIZZLE_DEFAULT_DELAY, newPalette, oldPalette);
    H2_FREE(lookupTable);
    delete savedBitmap;
    gpMouseManager->ShowColorPointer();
    H2_FREE(oldPalette);
    H2_FREE(newPalette);
}

#undef BLUR_TAP_SUM
