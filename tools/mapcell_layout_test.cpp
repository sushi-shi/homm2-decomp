#include <EDITOR/mapcell.h>

#include <cassert>
#include <cstddef>
#include <cstring>

int main() {
    mapCell cell;
    std::memset(&cell, 0, sizeof(cell));
    cell.m_animatedObject = 1;
    cell.m_isRoad = 1;
    cell.SetObjectTileset(TILESET_X_LOC3);
    cell.m_animatedOverlay = 1;
    cell.m_drawOverlayOnTop = 1;
    cell.SetOverlayTileset(TILESET_X_LOC3);

    assert(sizeof(cell) == 12);
    assert(cell.m_objTypeBits == 0xff);
    assert(reinterpret_cast<const u8*>(&cell)[6] == 0xff);
    assert(cell.ObjectTileset() == TILESET_X_LOC3);
    assert(cell.OverlayTileset() == TILESET_X_LOC3);

    mapCellExtra extra;
    std::memset(&extra, 0, sizeof(extra));
    extra.animatedObject = 1;
    extra.SetObjectTileset(TILESET_X_LOC3);
    extra.animatedOverlay = 1;
    extra.drawOverlayOnTop = 1;
    extra.SetOverlayTileset(TILESET_X_LOC3);

    assert(sizeof(extra) == 7);
    assert(reinterpret_cast<const u8*>(&extra)[2] == 0x7f);
    assert(reinterpret_cast<const u8*>(&extra)[5] == 0xff);
    assert(extra.ObjectTileset() == TILESET_X_LOC3);
    assert(extra.OverlayTileset() == TILESET_X_LOC3);
}
