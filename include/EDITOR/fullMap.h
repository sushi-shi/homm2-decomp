#ifndef HOMM2_EDITOR_FULLMAP_H
#define HOMM2_EDITOR_FULLMAP_H

#include <H2/Ints.h>
#include <Domains.h>
#include <EDITOR/mapcell.h>

class mapCell;
struct mapCellExtra;

// The map lives inside the packed game record (game::m_worldMap), at an
// offset with no natural alignment, so its own layout is packed as well.
#pragma pack(push, 1)
class fullMap {
public:
    mapCell* cells;
    mapCellExtra* extras;
    i32 width;
    i32 height;
    i32 extraCount;

    fullMap(void);
    ~fullMap();
    void Close(void);
    void Init(i32 mapWidth, i32 mapHeight);
    void ClearCellExtra(i32 index);
    i32 GetNewCellExtraIndex(void);
    struct mapCellExtra* GetNewCellExtraOverlay(i32 x, i32 y);
    struct mapCellExtra* GetNewCellExtraObject(i32 x, i32 y);
    void Write(i32 handle);
    void Read(i32 handle, i32 convert);
    void ChangeTilesetIndex(
        class mapCell* cell,
        i32 x,
        i32 y,
        TilesetId tileset,
        i32 index,
        i32 overlay,
        i32 link
    );

    mapCell* Cells(void) {
        return cells;
    }
    mapCell* Row(i32 y) {
        return &cells[y * width];
    }
    mapCell* Column(i32 x) {
        return &cells[x];
    }
    void Cell(mapCell*& cell, i32 x, i32 y) {
        cell = cells + x + y * width;
    }
    mapCell* GetCell(i32 x, i32 y) {
        return &Column(x)[y * width];
    }

    mapCell* CellAt(i32 x, i32 y) {
        return cells + x + y * width;
    }
    mapCellExtra* Extra(i32 i) {
        return &extras[i];
    }
};
#pragma pack(pop)
#endif
