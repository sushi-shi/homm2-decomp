#include <EDITOR/mapcell.h>

#include <cstdio>
#include <cstring>

static_assert(sizeof(mapCell) == 12);
static_assert(sizeof(mapCellExtra) == 7);

namespace {

bool Expect(bool condition, const char* description) {
    if (!condition)
        std::fprintf(stderr, "map cell layout mismatch: %s\n", description);
    return condition;
}

}

int main() {
    bool valid = true;
    mapCell cell;
    std::memset(&cell, 0, sizeof(cell));
    cell.m_animatedObject = 1;
    cell.m_isRoad = 1;
    cell.SetObjectTileset(TILESET_X_LOC3);
    cell.m_animatedOverlay = 1;
    cell.m_drawOverlayOnTop = 1;
    cell.SetOverlayTileset(TILESET_X_LOC3);

    valid &= Expect(cell.m_objTypeBits == 0xff, "object bits");
    valid &= Expect(reinterpret_cast<const u8*>(&cell)[6] == 0xff, "overlay byte");
    valid &= Expect(cell.ObjectTileset() == TILESET_X_LOC3, "object tileset");
    valid &= Expect(cell.OverlayTileset() == TILESET_X_LOC3, "overlay tileset");

    mapCellExtra extra;
    std::memset(&extra, 0, sizeof(extra));
    extra.animatedObject = 1;
    extra.SetObjectTileset(TILESET_X_LOC3);
    extra.animatedOverlay = 1;
    extra.drawOverlayOnTop = 1;
    extra.SetOverlayTileset(TILESET_X_LOC3);

    valid &= Expect(reinterpret_cast<const u8*>(&extra)[2] == 0x7f, "extra object byte");
    valid &= Expect(reinterpret_cast<const u8*>(&extra)[5] == 0xff, "extra overlay byte");
    valid &= Expect(extra.ObjectTileset() == TILESET_X_LOC3, "extra object tileset");
    valid &= Expect(extra.OverlayTileset() == TILESET_X_LOC3, "extra overlay tileset");
    return valid ? 0 : 1;
}
