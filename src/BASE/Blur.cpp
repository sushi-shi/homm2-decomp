#define HOMM2_MISC_INLINE_ICONENTRY
#include <va.h>
#include <BASE/Blur.h>

// Fixed geometry of the 640-pixel framebuffer region processed by DoBlur.
// The blur samples sixteen pixels from the horizontal and vertical axes around
// each output pixel. Component-table indices are byte offsets into int tables.
// The game palette stores 256 RGB triplets with six-bit component values.
H2_ENUM_BEGIN(BlurPaletteConstant)
    PALETTE_RED_OFFSET          = -3,
    PALETTE_GREEN_OFFSET        = -2,
    PALETTE_BLUE_OFFSET         = -1,
    PALETTE_CHANNEL_COUNT       = 3,
    PALETTE_COLOR_COUNT         = 256,
    PALETTE_BYTE_COUNT          = PALETTE_CHANNEL_COUNT * PALETTE_COLOR_COUNT,
    PALETTE_COMPONENT_MAXIMUM   = 0x3f,
    COMPONENT_TABLE_BYTE_COUNT  = sizeof(i32) * PALETTE_COLOR_COUNT
H2_ENUM_END(BlurPaletteConstant)

// RGBLOOKP.BIN is addressed as 1024 rows of 32 quantized blue entries.
H2_ENUM_BEGIN(BlurLookupConstant)
    QUANTIZATION_SHIFT    = 5,
    QUANTIZATION_MASK     = (1 << QUANTIZATION_SHIFT) - 1,
    LOOKUP_ROW_BYTE_COUNT = 1 << QUANTIZATION_SHIFT,
    LOOKUP_ROW_COUNT      = 1024,
    LOOKUP_BYTE_COUNT     = LOOKUP_ROW_COUNT * LOOKUP_ROW_BYTE_COUNT
H2_ENUM_END(BlurLookupConstant)

typedef i32 BlurComponentTable[PALETTE_COLOR_COUNT];
typedef u8 BlurLookupRow[LOOKUP_ROW_BYTE_COUNT];

#include <BASE/bitmap.h>
#include <BASE/palette.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
H2_ENUM_BEGIN(BlurBoundsConstant)
    SCREEN_WIDTH          = 640,
    BORDER_RADIUS         = 4,
    INTERIOR_COLUMN_COUNT = SCREEN_WIDTH - BORDER_RADIUS * 2
H2_ENUM_END(BlurBoundsConstant)

H2_ENUM_BEGIN(BlurKernelConstant)
    SECOND_NEIGHBOR_DISTANCE   = 2,
    THIRD_NEIGHBOR_DISTANCE    = 3,
    KERNEL_SAMPLE_COUNT        = 16,
    SPILLED_ARRAY_SAMPLE_COUNT = KERNEL_SAMPLE_COUNT - 3,
    COMPONENT_INDEX_SHIFT      = 2,
    SOUND_POLL_MASK            = 0x3f
H2_ENUM_END(BlurKernelConstant)

H2_ENUM_CLASS_BEGIN(BlurKernelSample)
    SOUTH_ONE_SAMPLE   = 0,
    NORTH_FOUR_SAMPLE  = 1,
    SOUTH_TWO_SAMPLE   = 2,
    NORTH_TWO_SAMPLE   = 3,
    EAST_ONE_SAMPLE    = 4,
    WEST_ONE_SAMPLE    = 5,
    EAST_TWO_SAMPLE    = 6,
    SOUTH_THREE_SAMPLE = 7,
    WEST_TWO_SAMPLE    = 8,
    NORTH_THREE_SAMPLE = 9,
    EAST_THREE_SAMPLE  = 10,
    EAST_FOUR_SAMPLE   = 11,
    WEST_THREE_SAMPLE  = 12
H2_ENUM_CLASS_END(BlurKernelSample)

H2_ENUM_CLASS_BEGIN(BlurPaletteChannel)
    RED_CHANNEL   = 0,
    GREEN_CHANNEL = 1,
    BLUE_CHANNEL  = 2
H2_ENUM_CLASS_END(BlurPaletteChannel)

H2_ENUM_BEGIN(BlurTransitionConstant)
    FIZZLE_DELAY                    = 150,
    COMBAT_SPEED_DELAY_MILLISECONDS = 350
H2_ENUM_END(BlurTransitionConstant)

#undef HOMM2_MISC_INLINE_ICONENTRY
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\Blur.cpp"

#define BLUR_COMPONENT(table, offset)                                                              \
    (*reinterpret_cast<i32*>(reinterpret_cast<u8*>(table) + (offset)))

H2_ENUM_BEGIN(BlurTextSlotSize)
    SOURCE_FILE_SLOT_SIZE     = 0x28,
    LOOKUP_FILENAME_SLOT_SIZE = 0x10
H2_ENUM_END(BlurTextSlotSize)

typedef struct SBlurText {
    char lookupAllocationSource[SOURCE_FILE_SLOT_SIZE];
    char lookupFilename[LOOKUP_FILENAME_SLOT_SIZE];
    char oldPaletteAllocationSource[SOURCE_FILE_SLOT_SIZE];
    char newPaletteAllocationSource[SOURCE_FILE_SLOT_SIZE];
    char lookupFreeSource[SOURCE_FILE_SLOT_SIZE];
    char oldPaletteFreeSource[SOURCE_FILE_SLOT_SIZE];
    char newPaletteFreeSource[SOURCE_FILE_SLOT_SIZE];
} SBlurText;

SIZE(SBlurText, 0x100);

DATA(0x0051fdc0) static SBlurText gBlurText =
    {RETAIL_FILE, "RGBLOOKP.BIN", RETAIL_FILE, RETAIL_FILE, RETAIL_FILE, RETAIL_FILE, RETAIL_FILE};

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
    gpWindowManager->SaveFizzleSource(0, 0, SCREEN_WIDTH, height);

    bitmap* saved = new bitmap(BITMAP_TYPE_NONE, SCREEN_WIDTH, static_cast<i16>(height));
    u32 imageSize = height * SCREEN_WIDTH;
    memcpy(saved->m_pixels, source->m_pixels, imageSize);

    BlurLookupRow* lookup = static_cast<BlurLookupRow*>(
        H2_ALLOC_AT(LOOKUP_BYTE_COUNT, gBlurText.lookupAllocationSource, 0x19)
    );
    BlurComponentTable components[PALETTE_CHANNEL_COUNT];
    i8* paletteColor = gpBufferPalette->m_data;
    i32 componentOffset = 0;
    do {
        componentOffset += sizeof(i32);
        paletteColor += PALETTE_CHANNEL_COUNT;
        BLUR_COMPONENT(components[IDX(RED_CHANNEL)], componentOffset - sizeof(i32)) =
            static_cast<u8>(paletteColor[PALETTE_RED_OFFSET]);
        BLUR_COMPONENT(components[IDX(GREEN_CHANNEL)], componentOffset - sizeof(i32)) =
            static_cast<u8>(paletteColor[PALETTE_GREEN_OFFSET]);
        BLUR_COMPONENT(components[IDX(BLUE_CHANNEL)], componentOffset - sizeof(i32)) =
            static_cast<u8>(paletteColor[PALETTE_BLUE_OFFSET]);
    } while (componentOffset < COMPONENT_TABLE_BYTE_COUNT);

    resourceManager* resourceMgr;
    u32l lookupId = (resourceMgr = gpResourceManager)->MakeId(gBlurText.lookupFilename, 1);
    resourceMgr->PointToFile(lookupId);
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(lookup), LOOKUP_BYTE_COUNT);
    memcpy(destination->m_pixels, source->m_pixels, imageSize);
    PollSound();

    i32 lastRow = height - BORDER_RADIUS;
    i32 y = BORDER_RADIUS;
    if (y < lastRow) {
        i32 rowOffset = BORDER_RADIUS * SCREEN_WIDTH;
        do {
            if ((y & SOUND_POLL_MASK) == SOUND_POLL_MASK) {
                PollSound();
            }
            i32 samples[SPILLED_ARRAY_SAMPLE_COUNT];
            i32 sample14; // north one row
            i32 sample13; // west four pixels
            i32 sample15; // south four rows
            u8* input = destination->m_pixels + rowOffset + BORDER_RADIUS;
            i32 remaining = INTERIOR_COLUMN_COUNT;
            u8* output = source->m_pixels + rowOffset + BORDER_RADIUS;
            do {
                samples[IDX(SOUTH_ONE_SAMPLE)] =
                    static_cast<u32>(input[SCREEN_WIDTH]) << COMPONENT_INDEX_SHIFT;
                samples[IDX(NORTH_FOUR_SAMPLE)] =
                    static_cast<u32>(input[-SCREEN_WIDTH * BORDER_RADIUS])
                    << COMPONENT_INDEX_SHIFT;
                samples[IDX(SOUTH_TWO_SAMPLE)] =
                    static_cast<u32>(input[SCREEN_WIDTH * SECOND_NEIGHBOR_DISTANCE])
                    << COMPONENT_INDEX_SHIFT;
                samples[IDX(NORTH_TWO_SAMPLE)] =
                    static_cast<u32>(input[-SCREEN_WIDTH * SECOND_NEIGHBOR_DISTANCE])
                    << COMPONENT_INDEX_SHIFT;
                samples[IDX(EAST_ONE_SAMPLE)] =
                    static_cast<u32>(input[1]) << COMPONENT_INDEX_SHIFT;
                samples[IDX(WEST_ONE_SAMPLE)] =
                    static_cast<u32>(input[-1]) << COMPONENT_INDEX_SHIFT;
                samples[IDX(EAST_TWO_SAMPLE)] =
                    static_cast<u32>(input[SECOND_NEIGHBOR_DISTANCE]) << COMPONENT_INDEX_SHIFT;
                samples[IDX(SOUTH_THREE_SAMPLE)] =
                    static_cast<u32>(input[SCREEN_WIDTH * THIRD_NEIGHBOR_DISTANCE])
                    << COMPONENT_INDEX_SHIFT;
                samples[IDX(WEST_TWO_SAMPLE)] =
                    static_cast<u32>(input[-SECOND_NEIGHBOR_DISTANCE]) << COMPONENT_INDEX_SHIFT;
                samples[IDX(NORTH_THREE_SAMPLE)] =
                    static_cast<u32>(input[-SCREEN_WIDTH * THIRD_NEIGHBOR_DISTANCE])
                    << COMPONENT_INDEX_SHIFT;
                samples[IDX(EAST_THREE_SAMPLE)] =
                    static_cast<u32>(input[THIRD_NEIGHBOR_DISTANCE]) << COMPONENT_INDEX_SHIFT;
                samples[IDX(EAST_FOUR_SAMPLE)] =
                    static_cast<u32>(input[BORDER_RADIUS]) << COMPONENT_INDEX_SHIFT;
                samples[IDX(WEST_THREE_SAMPLE)] =
                    static_cast<u32>(input[-THIRD_NEIGHBOR_DISTANCE]) << COMPONENT_INDEX_SHIFT;
                sample13 = static_cast<u32>(input[-BORDER_RADIUS])
                           << COMPONENT_INDEX_SHIFT;
                sample14 = static_cast<u32>(input[-SCREEN_WIDTH])
                           << COMPONENT_INDEX_SHIFT;
                sample15 = static_cast<u32>(input[SCREEN_WIDTH * BORDER_RADIUS])
                           << COMPONENT_INDEX_SHIFT;

                i32 redSum = BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(NORTH_FOUR_SAMPLE)]
                ) + BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(SOUTH_ONE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(SOUTH_TWO_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(NORTH_TWO_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(EAST_ONE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(WEST_ONE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(EAST_TWO_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(SOUTH_THREE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(WEST_TWO_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(NORTH_THREE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(EAST_THREE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(EAST_FOUR_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    components[IDX(RED_CHANNEL)], samples[IDX(WEST_THREE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(components[IDX(RED_CHANNEL)], sample13);
                redSum += BLUR_COMPONENT(components[IDX(RED_CHANNEL)], sample14);
                redSum += BLUR_COMPONENT(components[IDX(RED_CHANNEL)], sample15);

                i32 greenSum = BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(SOUTH_ONE_SAMPLE)]
                )
                               + BLUR_COMPONENT(components[IDX(GREEN_CHANNEL)], sample15);
                greenSum += BLUR_COMPONENT(components[IDX(GREEN_CHANNEL)], sample14);
                greenSum += BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(SOUTH_TWO_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(NORTH_TWO_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(EAST_ONE_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(WEST_ONE_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(EAST_TWO_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(SOUTH_THREE_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(WEST_TWO_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(NORTH_THREE_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(EAST_THREE_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(EAST_FOUR_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(WEST_THREE_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(components[IDX(GREEN_CHANNEL)], sample13);
                greenSum += BLUR_COMPONENT(
                    components[IDX(GREEN_CHANNEL)], samples[IDX(NORTH_FOUR_SAMPLE)]
                );

                i32 blueSum = BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(SOUTH_ONE_SAMPLE)]
                )
                              + BLUR_COMPONENT(components[IDX(BLUE_CHANNEL)], sample14);
                blueSum += BLUR_COMPONENT(components[IDX(BLUE_CHANNEL)], sample15);
                blueSum += BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(NORTH_FOUR_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(SOUTH_TWO_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(NORTH_TWO_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(EAST_ONE_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(WEST_ONE_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(EAST_TWO_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(SOUTH_THREE_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(WEST_TWO_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(NORTH_THREE_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(EAST_THREE_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(EAST_FOUR_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    components[IDX(BLUE_CHANNEL)], samples[IDX(WEST_THREE_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(components[IDX(BLUE_CHANNEL)], sample13);
                *output++ = lookup[redSum & ~QUANTIZATION_MASK]
                                  [(greenSum & ~QUANTIZATION_MASK)
                                   + (blueSum >> QUANTIZATION_SHIFT)];
                input++;
            } while (--remaining != 0);
            rowOffset += SCREEN_WIDTH;
            y++;
        } while (y < lastRow);
    }

    PollSound();
    i8* oldPalette = static_cast<i8*>(
        H2_ALLOC_AT(PALETTE_BYTE_COUNT, gBlurText.oldPaletteAllocationSource, 0x8b)
    );
    i8* newPalette = static_cast<i8*>(
        H2_ALLOC_AT(PALETTE_BYTE_COUNT, gBlurText.newPaletteAllocationSource, 0x8c)
    );
    memcpy(oldPalette, gPalette->m_data, PALETTE_BYTE_COUNT);
    i8* oldColor = oldPalette;
    i32 remainingColors = PALETTE_COLOR_COUNT;
    i8* newColor = newPalette;
    do {
        newColor[IDX(RED_CHANNEL)] =
            oldColor[IDX(RED_CHANNEL)] + static_cast<i8>(redAdjust);
        newColor[IDX(GREEN_CHANNEL)] =
            oldColor[IDX(GREEN_CHANNEL)] + static_cast<i8>(greenAdjust);
        newColor[IDX(BLUE_CHANNEL)] =
            oldColor[IDX(BLUE_CHANNEL)] + static_cast<i8>(blueAdjust);
        if (newColor[IDX(RED_CHANNEL)] > PALETTE_COMPONENT_MAXIMUM) {
            newColor[IDX(RED_CHANNEL)] = PALETTE_COMPONENT_MAXIMUM;
        }
        if (newColor[IDX(RED_CHANNEL)] < 0) {
            newColor[IDX(RED_CHANNEL)] = 0;
        }
        if (newColor[IDX(GREEN_CHANNEL)] > PALETTE_COMPONENT_MAXIMUM) {
            newColor[IDX(GREEN_CHANNEL)] = PALETTE_COMPONENT_MAXIMUM;
        }
        if (newColor[IDX(GREEN_CHANNEL)] < 0) {
            newColor[IDX(GREEN_CHANNEL)] = 0;
        }
        if (newColor[IDX(BLUE_CHANNEL)] > PALETTE_COMPONENT_MAXIMUM) {
            newColor[IDX(BLUE_CHANNEL)] = PALETTE_COMPONENT_MAXIMUM;
        }
        if (newColor[IDX(BLUE_CHANNEL)] < 0) {
            newColor[IDX(BLUE_CHANNEL)] = 0;
        }
        oldColor += PALETTE_CHANNEL_COUNT;
        newColor += PALETTE_CHANNEL_COUNT;
    } while (--remainingColors != 0);

    gpWindowManager
        ->FizzleForward(0, 0, SCREEN_WIDTH, height, FIZZLE_DELAY, oldPalette, newPalette);
    DelayMilli(
        static_cast<i32l>(gfCombatSpeedMod[gConfig.combatSpeed] * COMBAT_SPEED_DELAY_MILLISECONDS)
    );
    gpWindowManager->SaveFizzleSource(0, 0, SCREEN_WIDTH, height);
    memcpy(source->m_pixels, saved->m_pixels, imageSize);
    gpWindowManager
        ->FizzleForward(0, 0, SCREEN_WIDTH, height, FIZZLE_DELAY, newPalette, oldPalette);
    H2_FREE_AT(lookup, gBlurText.lookupFreeSource, 0xa8);
    if (saved != NULL) {
        delete saved;
    }
    gpMouseManager->ShowColorPointer();
    H2_FREE_AT(oldPalette, gBlurText.oldPaletteFreeSource, 0xad);
    H2_FREE_AT(newPalette, gBlurText.newPaletteFreeSource, 0xae);
}


#undef BLUR_COMPONENT

#undef RETAIL_FILE
