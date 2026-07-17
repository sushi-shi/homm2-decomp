#ifndef HOMM2_SOURCE_DIMPALETTE_H
#define HOMM2_SOURCE_DIMPALETTE_H

#include <Ints.h>

H2_ENUM_BEGIN(DimPaletteConstant)
    DIM_PALETTE_SET_COUNT = 3,
    DIM_PALETTE_LEVEL_COUNT = 4,
    DIM_PALETTE_COLOR_COUNT = 256
H2_ENUM_END(DimPaletteConstant)

// Shared dimming lookup table owned and defined by SOURCE/KB.
extern u8 uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][DIM_PALETTE_COLOR_COUNT];

#endif
