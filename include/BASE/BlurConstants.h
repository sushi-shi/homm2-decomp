#ifndef HOMM2_BASE_BLURCONSTANTS_H
#define HOMM2_BASE_BLURCONSTANTS_H

#include <Ints.h>

// Fixed geometry of the 640-pixel framebuffer region processed by DoBlur.
// The blur samples sixteen pixels from the horizontal and vertical axes around
// each output pixel. Component-table indices are byte offsets into int tables.
// The game palette stores 256 RGB triplets with six-bit component values.
H2_ENUM_BEGIN(BlurPaletteConstant)
    BLUR_PALETTE_CHANNEL_COUNT      = 3,
    BLUR_PALETTE_COLOR_COUNT        = 256,
    BLUR_PALETTE_BYTE_COUNT         = BLUR_PALETTE_CHANNEL_COUNT * BLUR_PALETTE_COLOR_COUNT,
    BLUR_PALETTE_COMPONENT_MAXIMUM  = 0x3f,
    BLUR_COMPONENT_TABLE_BYTE_COUNT = sizeof(i32) * BLUR_PALETTE_COLOR_COUNT
H2_ENUM_END(BlurPaletteConstant)

// RGBLOOKP.BIN is addressed as 1024 rows of 32 quantized blue entries.
H2_ENUM_BEGIN(BlurLookupConstant)
    BLUR_QUANTIZATION_SHIFT    = 5,
    BLUR_QUANTIZATION_MASK     = (1 << BLUR_QUANTIZATION_SHIFT) - 1,
    BLUR_LOOKUP_ROW_BYTE_COUNT = 1 << BLUR_QUANTIZATION_SHIFT,
    BLUR_LOOKUP_ROW_COUNT      = 1024,
    BLUR_LOOKUP_BYTE_COUNT     = BLUR_LOOKUP_ROW_COUNT * BLUR_LOOKUP_ROW_BYTE_COUNT
H2_ENUM_END(BlurLookupConstant)

typedef i32 BlurComponentTable[BLUR_PALETTE_COLOR_COUNT];
typedef u8 BlurLookupRow[BLUR_LOOKUP_ROW_BYTE_COUNT];

#endif
