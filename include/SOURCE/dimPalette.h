#ifndef HOMM2_SOURCE_DIMPALETTE_H
#define HOMM2_SOURCE_DIMPALETTE_H

#include <Ints.h>

typedef enum DimPaletteConstant {
    DIM_PALETTE_SET_COUNT   = 3,
    DIM_PALETTE_LEVEL_COUNT = 4,
    DIM_PALETTE_COLOR_COUNT = 256,
    DIM_PALETTE_LIGHTEN_TABLE = DIM_PALETTE_SET_COUNT * DIM_PALETTE_LEVEL_COUNT,
    DIM_PALETTE_NO_CYCLE_TABLE
} DimPaletteConstant;

extern u8 uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][DIM_PALETTE_COLOR_COUNT];
extern u8 gColorTableLighten[DIM_PALETTE_COLOR_COUNT];
extern u8 gColorTableNoCycle[DIM_PALETTE_COLOR_COUNT];

inline u8* GetDimPaletteTable(u32 table) {
    if (table < DIM_PALETTE_LIGHTEN_TABLE)
        return &uDimPal[0][0][table * DIM_PALETTE_COLOR_COUNT];
    if (table == DIM_PALETTE_LIGHTEN_TABLE)
        return gColorTableLighten;
    return gColorTableNoCycle;
}

#endif
