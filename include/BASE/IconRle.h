#ifndef HOMM2_BASE_ICONRLE_H
#define HOMM2_BASE_ICONRLE_H

#include <Ints.h>

H2_ENUM_BEGIN(IconRleCommandConstant)
    ICON_RLE_COMMAND_RUN_MASK = 0x3f,
    ICON_RLE_COMMAND_SOLID_FLAG = 0x40,
    ICON_RLE_LONG_SOLID_COMMAND = 0xc1,
    ICON_RLE_DIM_SHORT_COUNT_MASK = 0x03,
    ICON_RLE_DIM_LEVEL_MASK = 0x3c,
    ICON_RLE_DIM_APPLY_FLAG = 0x40,
    ICON_RLE_DIM_RECOLOR_FLAG = 0x80,
    ICON_RLE_DIM_PALETTE_LEVEL_STRIDE = 0x40
H2_ENUM_END(IconRleCommandConstant)

inline u8 ReadIconRleByte(u8*& cursor) {
    cursor++;
    return cursor[-1];
}

#endif
