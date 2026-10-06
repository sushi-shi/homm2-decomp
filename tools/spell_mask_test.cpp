#include <SOURCE/SpellMask.h>
#include <array>
#include <limits>

int main() {
    std::array<i8, 480> rows{};
    // Both edges, empty/reversed ranges, and extreme endpoints.
    FillSpellMask(rows, -1, 0, -1);
    if (rows[0] != -1 || rows[1] != 0) return 1;
    FillSpellMask(rows, 479, 490, -1);
    if (rows[479] != -1 || rows[478] != 0) return 2;
    FillSpellMask(rows, 8, 7, 42);
    FillSpellMask(rows, -10, -1, 42);
    FillSpellMask(rows, 480, 490, 42);
    SetSpellMaskRow(rows, -1, 42);
    SetSpellMaskRow(rows, 480, 42);
    for (int row = 1; row < 479; ++row)
        if (rows[row] != 0) return 3;
    FillSpellMask(rows, std::numeric_limits<i32>::min(),
                  std::numeric_limits<i32>::max(), 7);
    for (i8 value : rows) if (value != 7) return 4;
    SetSpellMaskRow(rows, 479, 3);
    if (rows[479] != 3) return 5;
    FillSpellMask({}, -1, 1, 0);
    SetSpellMaskRow({}, 0, 0);
    return 0;
}
