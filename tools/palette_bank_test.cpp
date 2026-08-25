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
    valid &= Expect(GetDimPaletteTable(0), &uDimPal[0][0][0], "first dim table");
    valid &= Expect(GetDimPaletteTable(11), &uDimPal[2][3][0], "last dim table");
    valid &= Expect(GetDimPaletteTable(12), gColorTableLighten, "lighten table");
    valid &= Expect(GetDimPaletteTable(13), gColorTableNoCycle, "no-cycle table");
    return valid ? 0 : 1;
}
