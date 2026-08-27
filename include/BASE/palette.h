#ifndef HOMM2_BASE_PALETTE_H
#define HOMM2_BASE_PALETTE_H

#include <va.h>
#include <BASE/resource.h>

H2_ENUM_BEGIN(PaletteConstant)
    PALETTE_DATA_SIZE = 0x300
H2_ENUM_END(PaletteConstant)

H2_ENUM_CLASS_BEGIN(PaletteChannel)
    PALETTE_CHANNEL_RED   = 0,
    PALETTE_CHANNEL_GREEN = 1,
    PALETTE_CHANNEL_BLUE  = 2,
    PALETTE_CHANNEL_COUNT = 3
H2_ENUM_CLASS_END(PaletteChannel)

#pragma pack(push, 1)
class palette : public resource {
public:
    i8* m_data;
    palette(void);
    palette(u32l id);
    virtual H2_RETAIL_INLINE ~palette();
    i8* Data(void);
};
#pragma pack(pop)
SIZE(palette, 0x14);
#endif
