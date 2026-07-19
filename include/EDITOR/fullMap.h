#ifndef HOMM2_EDITOR_FULLMAP_H
#define HOMM2_EDITOR_FULLMAP_H

#include <va.h>
#include "mapcell.h"

class mapCell;
struct mapCellExtra;

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
    void Init(i32, i32);
    void ClearCellExtra(i32);
    i32 GetNewCellExtraIndex(void);
    struct mapCellExtra* GetNewCellExtraOverlay(i32, i32);
    struct mapCellExtra* GetNewCellExtraObject(i32, i32);
    void Write(i32);
    void Read(i32, i32);
    void ChangeTilesetIndex(class mapCell*, i32, i32, i32, i32, i32, i32);

    mapCell* Cells(void) {
        return cells;
    }
    mapCell* Row(i32 y) {
        return &cells[width * y];
    }
    void Cell(mapCell*& cell, i32 x, i32 y) {
        cell = &cells[width * y + x];
    }
    mapCell* GetCell(i32 x, i32 y) {
        return cells + width * y + x;
    }
    mapCellExtra* Extra(i32 i) {
        return &extras[i];
    }
};
SIZE(fullMap, 20);
#endif
