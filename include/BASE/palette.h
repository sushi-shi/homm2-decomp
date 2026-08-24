#ifndef HOMM2_BASE_PALETTE_H
#define HOMM2_BASE_PALETTE_H

#include <Ints.h>
#include <BASE/resource.h>

typedef enum PaletteConstant {
    PALETTE_DATA_SIZE = 0x300
} PaletteConstant;

enum class PaletteChannel : i32 {
    PALETTE_CHANNEL_RED   = 0,
    PALETTE_CHANNEL_GREEN = 1,
    PALETTE_CHANNEL_BLUE  = 2,
    PALETTE_CHANNEL_COUNT = 3
};
using enum PaletteChannel;

#pragma pack(push, 1)
class palette : public resource {
public:
    i8* m_data;
    palette(void);
    palette(u32l id);
    virtual inline ~palette();
    i8* Data(void);
};
#pragma pack(pop)
#endif
