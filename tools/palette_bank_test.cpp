#include <SOURCE/dimPalette.h>

#include <cstdio>

u8 uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][DIM_PALETTE_COLOR_COUNT];
u8 gColorTableLighten[DIM_PALETTE_COLOR_COUNT];
u8 gColorTableNoCycle[DIM_PALETTE_COLOR_COUNT];

namespace {

bool Expect(u8* actual, u8* expected, const char* description) {
    if (actual == expected) {
        return true;
    }
    std::fprintf(stderr, "palette bank mismatch: %s\n", description);
    return false;
}

}

int main() {
    bool valid = true;
    u32 table = 0;
    for (auto& set : uDimPal) {
        for (auto& level : set) {
            valid &= Expect(GetDimPaletteTable(table++), level, "dim table");
        }
    }
    valid &= Expect(GetDimPaletteTable(12), gColorTableLighten, "lighten table");
    valid &= Expect(GetDimPaletteTable(13), gColorTableNoCycle, "no-cycle table");
    valid &= Expect(GetDimPaletteTable(14), gColorTableNoCycle, "reserved level 14 fallback");
    valid &= Expect(GetDimPaletteTable(15), gColorTableNoCycle, "reserved level 15 fallback");
    return valid ? 0 : 1;
}
