#define HOMM2_MISC_INLINE_ICONENTRY
#include <va.h>
#include <BASE/Blur.h>
#include <BASE/BlurConstants.h>
#include <BASE/BLUR_TYPES.h>
#include <BASE/bitmap.h>
#include <BASE/palette.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#undef HOMM2_MISC_INLINE_ICONENTRY
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\Blur.cpp"

#define BLUR_COMPONENT(table, offset)                                                              \
    (*reinterpret_cast<i32*>(reinterpret_cast<u8*>(table) + (offset)))

DATA(0x0051fdc0) static SBlurText gBlurText =
    {RETAIL_FILE, "RGBLOOKP.BIN", RETAIL_FILE, RETAIL_FILE, RETAIL_FILE, RETAIL_FILE, RETAIL_FILE};

// @semantic: First divergence is +0x22: ours loads height into eax and loads gpWindowManager before pushing the four arguments.
VA(0x004d28e0, 0x6a4)
void DoBlur(
    bitmap* destination,
    bitmap* source,
    i32 height,
    i32 redAdjust,
    i32 greenAdjust,
    i32 blueAdjust
) {
    PollSound();
    gpMouseManager->HideColorPointer();
    gpWindowManager->SaveFizzleSource(0, 0, BLUR_SCREEN_WIDTH, height);

    bitmap* saved = new bitmap(BITMAP_TYPE_NONE, BLUR_SCREEN_WIDTH, static_cast<i16>(height));
    u32 imageSize = height * BLUR_SCREEN_WIDTH;
    memcpy(saved->m_pixels, source->m_pixels, imageSize);

    BlurLookupRow* lookup = static_cast<BlurLookupRow*>(
        H2_ALLOC_AT(BLUR_LOOKUP_BYTE_COUNT, gBlurText.lookupAllocationSource, 0x19)
    );
    BlurComponentTable components[BLUR_PALETTE_CHANNEL_COUNT];
    i8* paletteColor = gpBufferPalette->m_data;
    i32 componentOffset = 0;
    do {
        componentOffset += sizeof(i32);
        paletteColor += BLUR_PALETTE_CHANNEL_COUNT;
        BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], componentOffset - sizeof(i32)) =
            static_cast<u8>(paletteColor[-3]);
        BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], componentOffset - sizeof(i32)) =
            static_cast<u8>(paletteColor[-2]);
        BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], componentOffset - sizeof(i32)) =
            static_cast<u8>(paletteColor[-1]);
    } while (componentOffset < BLUR_COMPONENT_TABLE_BYTE_COUNT);

    resourceManager* resourceMgr;
    u32l lookupId = (resourceMgr = gpResourceManager)->MakeId(gBlurText.lookupFilename, 1);
    resourceMgr->PointToFile(lookupId);
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(lookup), BLUR_LOOKUP_BYTE_COUNT);
    memcpy(destination->m_pixels, source->m_pixels, imageSize);
    PollSound();

    i32 lastRow = height - BLUR_BORDER_RADIUS;
    i32 y = BLUR_BORDER_RADIUS;
    if (y < lastRow) {
        i32 rowOffset = BLUR_BORDER_RADIUS * BLUR_SCREEN_WIDTH;
        do {
            if ((y & BLUR_SOUND_POLL_MASK) == BLUR_SOUND_POLL_MASK) {
                PollSound();
            }
            i32 samples[BLUR_SPILLED_ARRAY_SAMPLE_COUNT];
            i32 sample14; // north one row
            i32 sample13; // west four pixels
            i32 sample15; // south four rows
            u8* input = destination->m_pixels + rowOffset + BLUR_BORDER_RADIUS;
            i32 remaining = BLUR_INTERIOR_COLUMN_COUNT;
            u8* output = source->m_pixels + rowOffset + BLUR_BORDER_RADIUS;
            do {
                samples[0] = static_cast<u32>(input[BLUR_SCREEN_WIDTH])
                             << BLUR_COMPONENT_INDEX_SHIFT; // south 1
                samples[1] = static_cast<u32>(input[-BLUR_SCREEN_WIDTH * BLUR_BORDER_RADIUS])
                             << BLUR_COMPONENT_INDEX_SHIFT; // north 4
                samples[2] = static_cast<u32>(input[BLUR_SCREEN_WIDTH * 2])
                             << BLUR_COMPONENT_INDEX_SHIFT; // south 2
                samples[3] = static_cast<u32>(input[-BLUR_SCREEN_WIDTH * 2])
                             << BLUR_COMPONENT_INDEX_SHIFT;                             // north 2
                samples[4] = static_cast<u32>(input[1]) << BLUR_COMPONENT_INDEX_SHIFT;  // east 1
                samples[5] = static_cast<u32>(input[-1]) << BLUR_COMPONENT_INDEX_SHIFT; // west 1
                samples[6] = static_cast<u32>(input[2]) << BLUR_COMPONENT_INDEX_SHIFT;  // east 2
                samples[7] = static_cast<u32>(input[BLUR_SCREEN_WIDTH * 3])
                             << BLUR_COMPONENT_INDEX_SHIFT;                             // south 3
                samples[8] = static_cast<u32>(input[-2]) << BLUR_COMPONENT_INDEX_SHIFT; // west 2
                samples[9] = static_cast<u32>(input[-BLUR_SCREEN_WIDTH * 3])
                             << BLUR_COMPONENT_INDEX_SHIFT;                             // north 3
                samples[10] = static_cast<u32>(input[3]) << BLUR_COMPONENT_INDEX_SHIFT; // east 3
                samples[11] = static_cast<u32>(input[BLUR_BORDER_RADIUS])
                              << BLUR_COMPONENT_INDEX_SHIFT;                             // east 4
                samples[12] = static_cast<u32>(input[-3]) << BLUR_COMPONENT_INDEX_SHIFT; // west 3
                sample13 = static_cast<u32>(input[-BLUR_BORDER_RADIUS])
                           << BLUR_COMPONENT_INDEX_SHIFT;
                sample14 = static_cast<u32>(input[-BLUR_SCREEN_WIDTH])
                           << BLUR_COMPONENT_INDEX_SHIFT;
                sample15 = static_cast<u32>(input[BLUR_SCREEN_WIDTH * BLUR_BORDER_RADIUS])
                           << BLUR_COMPONENT_INDEX_SHIFT;

                i32 redSum = BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[1])
                             + BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[0]);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[2]);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[3]);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[4]);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[5]);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[6]);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[7]);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[8]);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[9]);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[10]);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[11]);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], samples[12]);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], sample13);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], sample14);
                redSum += BLUR_COMPONENT(components[IDX(BLUR_RED_CHANNEL)], sample15);

                i32 greenSum = BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[0])
                               + BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], sample15);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], sample14);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[2]);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[3]);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[4]);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[5]);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[6]);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[7]);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[8]);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[9]);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[10]);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[11]);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[12]);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], sample13);
                greenSum += BLUR_COMPONENT(components[IDX(BLUR_GREEN_CHANNEL)], samples[1]);

                i32 blueSum = BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[0])
                              + BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], sample14);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], sample15);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[1]);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[2]);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[3]);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[4]);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[5]);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[6]);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[7]);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[8]);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[9]);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[10]);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[11]);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], samples[12]);
                blueSum += BLUR_COMPONENT(components[IDX(BLUR_BLUE_CHANNEL)], sample13);
                *output++ = lookup[redSum & ~BLUR_QUANTIZATION_MASK]
                                  [(greenSum & ~BLUR_QUANTIZATION_MASK)
                                   + (blueSum >> BLUR_QUANTIZATION_SHIFT)];
                input++;
            } while (--remaining != 0);
            rowOffset += BLUR_SCREEN_WIDTH;
            y++;
        } while (y < lastRow);
    }

    PollSound();
    i8* oldPalette = static_cast<i8*>(
        H2_ALLOC_AT(BLUR_PALETTE_BYTE_COUNT, gBlurText.oldPaletteAllocationSource, 0x8b)
    );
    i8* newPalette = static_cast<i8*>(
        H2_ALLOC_AT(BLUR_PALETTE_BYTE_COUNT, gBlurText.newPaletteAllocationSource, 0x8c)
    );
    memcpy(oldPalette, gPalette->m_data, BLUR_PALETTE_BYTE_COUNT);
    i8* oldColor = oldPalette;
    i32 remainingColors = BLUR_PALETTE_COLOR_COUNT;
    i8* newColor = newPalette;
    do {
        newColor[IDX(BLUR_RED_CHANNEL)] =
            oldColor[IDX(BLUR_RED_CHANNEL)] + static_cast<i8>(redAdjust);
        newColor[IDX(BLUR_GREEN_CHANNEL)] =
            oldColor[IDX(BLUR_GREEN_CHANNEL)] + static_cast<i8>(greenAdjust);
        newColor[IDX(BLUR_BLUE_CHANNEL)] =
            oldColor[IDX(BLUR_BLUE_CHANNEL)] + static_cast<i8>(blueAdjust);
        if (newColor[IDX(BLUR_RED_CHANNEL)] > BLUR_PALETTE_COMPONENT_MAXIMUM) {
            newColor[IDX(BLUR_RED_CHANNEL)] = BLUR_PALETTE_COMPONENT_MAXIMUM;
        }
        if (newColor[IDX(BLUR_RED_CHANNEL)] < 0) {
            newColor[IDX(BLUR_RED_CHANNEL)] = 0;
        }
        if (newColor[IDX(BLUR_GREEN_CHANNEL)] > BLUR_PALETTE_COMPONENT_MAXIMUM) {
            newColor[IDX(BLUR_GREEN_CHANNEL)] = BLUR_PALETTE_COMPONENT_MAXIMUM;
        }
        if (newColor[IDX(BLUR_GREEN_CHANNEL)] < 0) {
            newColor[IDX(BLUR_GREEN_CHANNEL)] = 0;
        }
        if (newColor[IDX(BLUR_BLUE_CHANNEL)] > BLUR_PALETTE_COMPONENT_MAXIMUM) {
            newColor[IDX(BLUR_BLUE_CHANNEL)] = BLUR_PALETTE_COMPONENT_MAXIMUM;
        }
        if (newColor[IDX(BLUR_BLUE_CHANNEL)] < 0) {
            newColor[IDX(BLUR_BLUE_CHANNEL)] = 0;
        }
        oldColor += BLUR_PALETTE_CHANNEL_COUNT;
        newColor += BLUR_PALETTE_CHANNEL_COUNT;
    } while (--remainingColors != 0);

    gpWindowManager
        ->FizzleForward(0, 0, BLUR_SCREEN_WIDTH, height, BLUR_FIZZLE_DELAY, oldPalette, newPalette);
    DelayMilli(static_cast<i32l>(gfCombatSpeedMod[gConfig.combatSpeed] * 350.0f));
    gpWindowManager->SaveFizzleSource(0, 0, BLUR_SCREEN_WIDTH, height);
    memcpy(source->m_pixels, saved->m_pixels, imageSize);
    gpWindowManager
        ->FizzleForward(0, 0, BLUR_SCREEN_WIDTH, height, BLUR_FIZZLE_DELAY, newPalette, oldPalette);
    H2_FREE_AT(lookup, gBlurText.lookupFreeSource, 0xa8);
    if (saved != 0) {
        delete saved;
    }
    gpMouseManager->ShowColorPointer();
    H2_FREE_AT(oldPalette, gBlurText.oldPaletteFreeSource, 0xad);
    H2_FREE_AT(newPalette, gBlurText.newPaletteFreeSource, 0xae);
}


#undef BLUR_COMPONENT

#undef RETAIL_FILE
