#define HOMM2_MISC_INLINE_ICONENTRY
#include <Ints.h>
#include <BASE/Blur.h>
#include <BASE/palette.h>


typedef enum BlurPaletteConstant {
    PALETTE_RED_OFFSET         = -3,
    PALETTE_GREEN_OFFSET       = -2,
    PALETTE_BLUE_OFFSET        = -1,
    PALETTE_COLOR_COUNT        = 256,
    PALETTE_BYTE_COUNT         = H2EnumIndex(PALETTE_CHANNEL_COUNT) * PALETTE_COLOR_COUNT,
    PALETTE_COMPONENT_MAXIMUM  = 0x3f,
    COMPONENT_TABLE_BYTE_COUNT = sizeof(i32) * PALETTE_COLOR_COUNT
} BlurPaletteConstant;


typedef enum BlurLookupConstant {
    QUANTIZATION_SHIFT    = 5,
    QUANTIZATION_MASK     = (1 << QUANTIZATION_SHIFT) - 1,
    LOOKUP_ROW_BYTE_COUNT = 1 << QUANTIZATION_SHIFT,
    LOOKUP_ROW_COUNT      = 1024,
    LOOKUP_BYTE_COUNT     = LOOKUP_ROW_COUNT * LOOKUP_ROW_BYTE_COUNT
} BlurLookupConstant;

typedef i32 BlurComponentTable[PALETTE_COLOR_COUNT];
typedef u8 BlurLookupRow[LOOKUP_ROW_BYTE_COUNT];

#include <BASE/bitmap.h>
#include <BASE/palette.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
typedef enum BlurBoundsConstant {
    SCREEN_WIDTH          = 640,
    BORDER_RADIUS         = 4,
    INTERIOR_COLUMN_COUNT = SCREEN_WIDTH - BORDER_RADIUS * 2
} BlurBoundsConstant;

typedef enum BlurKernelConstant {
    SECOND_NEIGHBOR_DISTANCE   = 2,
    THIRD_NEIGHBOR_DISTANCE    = 3,
    KERNEL_SAMPLE_COUNT        = 16,
    SPILLED_ARRAY_SAMPLE_COUNT = KERNEL_SAMPLE_COUNT - 3,
    COMPONENT_INDEX_SHIFT      = 2,
    SOUND_POLL_MASK            = 0x3f
} BlurKernelConstant;

enum class BlurKernelSample : i32 {
    SOUTH_ONE_SAMPLE   = 0,
    WEST_FOUR_SAMPLE   = 1,
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
};
using enum BlurKernelSample;

typedef enum BlurTransitionConstant {
    FIZZLE_DELAY                    = 150,
    COMBAT_SPEED_DELAY_MILLISECONDS = 350
} BlurTransitionConstant;

#undef HOMM2_MISC_INLINE_ICONENTRY
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>


#define BLUR_COMPONENT(table, offset)                                                              \
    BlurComponentAt((table), (offset))

static inline i32& BlurComponentAt(BlurComponentTable& table, i32 offset) {
    return *reinterpret_cast<i32*>(reinterpret_cast<u8*>(table) + offset);
}

typedef enum BlurTextSlotSize {
    SOURCE_FILE_SLOT_SIZE     = 0x28,
    LOOKUP_FILENAME_SLOT_SIZE = 0x10
} BlurTextSlotSize;

typedef struct SBlurText {
    char lookupAllocationSource[SOURCE_FILE_SLOT_SIZE];
    char lookupFilename[LOOKUP_FILENAME_SLOT_SIZE];
    char oldPaletteAllocationSource[SOURCE_FILE_SLOT_SIZE];
    char newPaletteAllocationSource[SOURCE_FILE_SLOT_SIZE];
    char lookupFreeSource[SOURCE_FILE_SLOT_SIZE];
    char oldPaletteFreeSource[SOURCE_FILE_SLOT_SIZE];
    char newPaletteFreeSource[SOURCE_FILE_SLOT_SIZE];
} SBlurText;


static SBlurText gBlurText =
    {"Blur.cpp", "RGBLOOKP.BIN", "Blur.cpp", "Blur.cpp", "Blur.cpp", "Blur.cpp", "Blur.cpp"};

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
        H2_ALLOC(LOOKUP_BYTE_COUNT)
    );
    BlurComponentTable redComponents;
    BlurComponentTable blueComponents;
    BlurComponentTable greenComponents;
    i8* paletteColor = gpBufferPalette->m_data;
    i32 componentOffset = 0;
    do {
        componentOffset += sizeof(i32);
        paletteColor += H2EnumIndex(PALETTE_CHANNEL_COUNT);
        BLUR_COMPONENT(redComponents, componentOffset - sizeof(i32)) =
            static_cast<u8>(paletteColor[PALETTE_RED_OFFSET]);
        BLUR_COMPONENT(greenComponents, componentOffset - sizeof(i32)) =
            static_cast<u8>(paletteColor[PALETTE_GREEN_OFFSET]);
        BLUR_COMPONENT(blueComponents, componentOffset - sizeof(i32)) =
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
            i32 sample15;
            i32 sample14;
            i32 sample13;
            u8* input = destination->m_pixels + rowOffset + BORDER_RADIUS;
            i32 remaining = INTERIOR_COLUMN_COUNT;
            u8* output = source->m_pixels + rowOffset + BORDER_RADIUS;
            do {
                samples[H2EnumIndex(SOUTH_ONE_SAMPLE)] =
                    static_cast<u32>(input[SCREEN_WIDTH]) << COMPONENT_INDEX_SHIFT;
                sample13 =
                    static_cast<u32>(input[-SCREEN_WIDTH * BORDER_RADIUS])
                    << COMPONENT_INDEX_SHIFT;
                samples[H2EnumIndex(SOUTH_TWO_SAMPLE)] =
                    static_cast<u32>(input[SCREEN_WIDTH * H2EnumIndex(SECOND_NEIGHBOR_DISTANCE)])
                    << COMPONENT_INDEX_SHIFT;
                samples[H2EnumIndex(NORTH_TWO_SAMPLE)] =
                    static_cast<u32>(input[-SCREEN_WIDTH * H2EnumIndex(SECOND_NEIGHBOR_DISTANCE)])
                    << COMPONENT_INDEX_SHIFT;
                samples[H2EnumIndex(EAST_ONE_SAMPLE)] =
                    static_cast<u32>(input[1]) << COMPONENT_INDEX_SHIFT;
                samples[H2EnumIndex(WEST_ONE_SAMPLE)] =
                    static_cast<u32>(input[-1]) << COMPONENT_INDEX_SHIFT;
                samples[H2EnumIndex(EAST_TWO_SAMPLE)] =
                    static_cast<u32>(input[SECOND_NEIGHBOR_DISTANCE]) << COMPONENT_INDEX_SHIFT;
                samples[H2EnumIndex(SOUTH_THREE_SAMPLE)] =
                    static_cast<u32>(input[SCREEN_WIDTH * H2EnumIndex(THIRD_NEIGHBOR_DISTANCE)])
                    << COMPONENT_INDEX_SHIFT;
                samples[H2EnumIndex(WEST_TWO_SAMPLE)] =
                    static_cast<u32>(input[-SECOND_NEIGHBOR_DISTANCE]) << COMPONENT_INDEX_SHIFT;
                samples[H2EnumIndex(NORTH_THREE_SAMPLE)] =
                    static_cast<u32>(input[-SCREEN_WIDTH * H2EnumIndex(THIRD_NEIGHBOR_DISTANCE)])
                    << COMPONENT_INDEX_SHIFT;
                samples[H2EnumIndex(EAST_THREE_SAMPLE)] =
                    static_cast<u32>(input[THIRD_NEIGHBOR_DISTANCE]) << COMPONENT_INDEX_SHIFT;
                samples[H2EnumIndex(EAST_FOUR_SAMPLE)] =
                    static_cast<u32>(input[BORDER_RADIUS]) << COMPONENT_INDEX_SHIFT;
                samples[H2EnumIndex(WEST_THREE_SAMPLE)] =
                    static_cast<u32>(input[-THIRD_NEIGHBOR_DISTANCE]) << COMPONENT_INDEX_SHIFT;
                samples[H2EnumIndex(WEST_FOUR_SAMPLE)] =
                    static_cast<u32>(input[-BORDER_RADIUS]) << COMPONENT_INDEX_SHIFT;
                sample14 = static_cast<u32>(input[-SCREEN_WIDTH])
                           << COMPONENT_INDEX_SHIFT;
                sample15 = static_cast<u32>(input[SCREEN_WIDTH * BORDER_RADIUS])
                           << COMPONENT_INDEX_SHIFT;

                u32 redSum = BLUR_COMPONENT(
                    redComponents, sample13
                ) + BLUR_COMPONENT(
                    redComponents, samples[H2EnumIndex(SOUTH_ONE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    redComponents, samples[H2EnumIndex(SOUTH_TWO_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    redComponents, samples[H2EnumIndex(NORTH_TWO_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    redComponents, samples[H2EnumIndex(EAST_ONE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    redComponents, samples[H2EnumIndex(WEST_ONE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    redComponents, samples[H2EnumIndex(EAST_TWO_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    redComponents, samples[H2EnumIndex(SOUTH_THREE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    redComponents, samples[H2EnumIndex(WEST_TWO_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    redComponents, samples[H2EnumIndex(NORTH_THREE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    redComponents, samples[H2EnumIndex(EAST_THREE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    redComponents, samples[H2EnumIndex(EAST_FOUR_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(
                    redComponents, samples[H2EnumIndex(WEST_THREE_SAMPLE)]
                );
                redSum += BLUR_COMPONENT(redComponents, samples[H2EnumIndex(WEST_FOUR_SAMPLE)]);
                redSum += BLUR_COMPONENT(redComponents, sample14);
                redSum += BLUR_COMPONENT(redComponents, sample15);

                u32 greenSum = BLUR_COMPONENT(
                    greenComponents, samples[H2EnumIndex(SOUTH_ONE_SAMPLE)]
                )
                               + BLUR_COMPONENT(greenComponents, sample15);
                greenSum += BLUR_COMPONENT(greenComponents, sample14);
                greenSum += BLUR_COMPONENT(
                    greenComponents, samples[H2EnumIndex(SOUTH_TWO_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    greenComponents, samples[H2EnumIndex(NORTH_TWO_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    greenComponents, samples[H2EnumIndex(EAST_ONE_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    greenComponents, samples[H2EnumIndex(WEST_ONE_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    greenComponents, samples[H2EnumIndex(EAST_TWO_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    greenComponents, samples[H2EnumIndex(SOUTH_THREE_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    greenComponents, samples[H2EnumIndex(WEST_TWO_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    greenComponents, samples[H2EnumIndex(NORTH_THREE_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    greenComponents, samples[H2EnumIndex(EAST_THREE_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    greenComponents, samples[H2EnumIndex(EAST_FOUR_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(
                    greenComponents, samples[H2EnumIndex(WEST_THREE_SAMPLE)]
                );
                greenSum += BLUR_COMPONENT(greenComponents, samples[H2EnumIndex(WEST_FOUR_SAMPLE)]);
                greenSum += BLUR_COMPONENT(
                    greenComponents, sample13
                );

                u32 blueSum = BLUR_COMPONENT(
                    blueComponents, samples[H2EnumIndex(SOUTH_ONE_SAMPLE)]
                )
                              + BLUR_COMPONENT(blueComponents, sample14);
                blueSum += BLUR_COMPONENT(blueComponents, sample15);
                blueSum += BLUR_COMPONENT(
                    blueComponents, sample13
                );
                blueSum += BLUR_COMPONENT(
                    blueComponents, samples[H2EnumIndex(SOUTH_TWO_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    blueComponents, samples[H2EnumIndex(NORTH_TWO_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    blueComponents, samples[H2EnumIndex(EAST_ONE_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    blueComponents, samples[H2EnumIndex(WEST_ONE_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    blueComponents, samples[H2EnumIndex(EAST_TWO_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    blueComponents, samples[H2EnumIndex(SOUTH_THREE_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    blueComponents, samples[H2EnumIndex(WEST_TWO_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    blueComponents, samples[H2EnumIndex(NORTH_THREE_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    blueComponents, samples[H2EnumIndex(EAST_THREE_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    blueComponents, samples[H2EnumIndex(EAST_FOUR_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(
                    blueComponents, samples[H2EnumIndex(WEST_THREE_SAMPLE)]
                );
                blueSum += BLUR_COMPONENT(blueComponents, samples[H2EnumIndex(WEST_FOUR_SAMPLE)]);
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
        H2_ALLOC(PALETTE_BYTE_COUNT)
    );
    i8* newPalette = static_cast<i8*>(
        H2_ALLOC(PALETTE_BYTE_COUNT)
    );
    memcpy(oldPalette, gPalette->m_data, PALETTE_BYTE_COUNT);
    i8* oldColor = oldPalette;
    i32 remainingColors = PALETTE_COLOR_COUNT;
    i8* newColor = newPalette;
    do {
        newColor[H2EnumIndex(PALETTE_CHANNEL_RED)] =
            oldColor[H2EnumIndex(PALETTE_CHANNEL_RED)] + static_cast<i8>(redAdjust);
        newColor[H2EnumIndex(PALETTE_CHANNEL_GREEN)] =
            oldColor[H2EnumIndex(PALETTE_CHANNEL_GREEN)] + static_cast<i8>(greenAdjust);
        newColor[H2EnumIndex(PALETTE_CHANNEL_BLUE)] =
            oldColor[H2EnumIndex(PALETTE_CHANNEL_BLUE)] + static_cast<i8>(blueAdjust);
        if (newColor[H2EnumIndex(PALETTE_CHANNEL_RED)] > PALETTE_COMPONENT_MAXIMUM) {
            newColor[H2EnumIndex(PALETTE_CHANNEL_RED)] = PALETTE_COMPONENT_MAXIMUM;
        }
        if (newColor[H2EnumIndex(PALETTE_CHANNEL_RED)] < 0) {
            newColor[H2EnumIndex(PALETTE_CHANNEL_RED)] = 0;
        }
        if (newColor[H2EnumIndex(PALETTE_CHANNEL_GREEN)] > PALETTE_COMPONENT_MAXIMUM) {
            newColor[H2EnumIndex(PALETTE_CHANNEL_GREEN)] = PALETTE_COMPONENT_MAXIMUM;
        }
        if (newColor[H2EnumIndex(PALETTE_CHANNEL_GREEN)] < 0) {
            newColor[H2EnumIndex(PALETTE_CHANNEL_GREEN)] = 0;
        }
        if (newColor[H2EnumIndex(PALETTE_CHANNEL_BLUE)] > PALETTE_COMPONENT_MAXIMUM) {
            newColor[H2EnumIndex(PALETTE_CHANNEL_BLUE)] = PALETTE_COMPONENT_MAXIMUM;
        }
        if (newColor[H2EnumIndex(PALETTE_CHANNEL_BLUE)] < 0) {
            newColor[H2EnumIndex(PALETTE_CHANNEL_BLUE)] = 0;
        }
        oldColor += H2EnumIndex(PALETTE_CHANNEL_COUNT);
        newColor += H2EnumIndex(PALETTE_CHANNEL_COUNT);
    } while (--remainingColors != 0);

    gpWindowManager
        ->FizzleForward(0, 0, SCREEN_WIDTH, height, FIZZLE_DELAY, oldPalette, newPalette);
    DelayMilli(
        static_cast<i32l>(
            gfCombatSpeedMod[gConfig.combatSpeed]
            * H2EnumIndex(COMBAT_SPEED_DELAY_MILLISECONDS)
        )
    );
    gpWindowManager->SaveFizzleSource(0, 0, SCREEN_WIDTH, height);
    memcpy(source->m_pixels, saved->m_pixels, imageSize);
    gpWindowManager
        ->FizzleForward(0, 0, SCREEN_WIDTH, height, FIZZLE_DELAY, newPalette, oldPalette);
    H2_FREE(lookup);
    if (saved != NULL) {
        delete saved;
    }
    gpMouseManager->ShowColorPointer();
    H2_FREE(oldPalette);
    H2_FREE(newPalette);
}


#undef BLUR_COMPONENT
