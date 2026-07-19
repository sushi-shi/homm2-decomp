#ifndef HOMM2_BASE_BLUR_TYPES_H
#define HOMM2_BASE_BLUR_TYPES_H

#include <va.h>

H2_ENUM_BEGIN(BlurTextSlotSize)
    BLUR_SOURCE_FILE_SLOT_SIZE     = 0x28,
    BLUR_LOOKUP_FILENAME_SLOT_SIZE = 0x10
H2_ENUM_END(BlurTextSlotSize)

typedef struct SBlurText {
    char lookupAllocationSource[BLUR_SOURCE_FILE_SLOT_SIZE];
    char lookupFilename[BLUR_LOOKUP_FILENAME_SLOT_SIZE];
    char oldPaletteAllocationSource[BLUR_SOURCE_FILE_SLOT_SIZE];
    char newPaletteAllocationSource[BLUR_SOURCE_FILE_SLOT_SIZE];
    char lookupFreeSource[BLUR_SOURCE_FILE_SLOT_SIZE];
    char oldPaletteFreeSource[BLUR_SOURCE_FILE_SLOT_SIZE];
    char newPaletteFreeSource[BLUR_SOURCE_FILE_SLOT_SIZE];
} SBlurText;

SIZE(SBlurText, 0x100);

#endif
