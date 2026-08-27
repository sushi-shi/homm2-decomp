#include <EnumCode.h>

static_assert(H2EnumCode<i8>);
static_assert(H2EnumCode<u32>);
static_assert(!H2EnumCode<bool>);
static_assert(!H2EnumCode<float>);

static_assert(H2EnumIndex(ResourceTypeFromCode(6)) == 6);
static_assert(H2EnumIndex(CreatureTypeFromOrdinal(47)) == 47);
static_assert(H2EnumIndex(TilesetIdFromCode(12)) == 12);
static_assert(H2EnumIndex(FontDrawModeFromCode(3)) == 3);
static_assert(H2EnumIndex(PlayerHandicapFromCode(-1)) == -1);

int main() {
    return 0;
}
