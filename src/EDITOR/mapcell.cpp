#include <va.h>
#include "EDITOR/fullMap.h"
#include "EDITOR/mapcell.h"
#include <string.h>
#include <io.h>

VA(0x0040b070, 0x36)
fullMap::fullMap(void) {
    cells = NULL;
    extras = NULL;
    extraCount = 0;
}

VA(0x0040b0a6, 0x1e)
fullMap::~fullMap(void) {
    Close();
}

VA(0x0040b0c4, 0x81)
void fullMap::Close(void) {
    if (cells)
        delete cells;
    cells = NULL;
    if (extras)
        delete extras;
    extras = NULL;
    extraCount = 0;
}

VA(0x0040b145, 0x53)
void fullMap::Init(i32 w, i32 h) {
    i32 n;
    width = w;
    height = h;
    Close();
    cells = static_cast<mapCell*>(operator new(width * height * sizeof(mapCell)));
}

VA(0x0040b198, 0xce)
void fullMap::ClearCellExtra(i32 index) {
    extras[index].objectTileset = 0;
    extras[index].objectIndex = IDX(MAPCELL_SPRITE_NONE);
    extras[index].animatedObject = 0;
    extras[index].objectLayerBit0 = 0;
    extras[index].objectLayerBit1 = 0;
    extras[index].objectDrawnAsOverlay = 0;
    extras[index].overlayTileset = 0;
    extras[index].overlayIndex = IDX(MAPCELL_SPRITE_NONE);
    extras[index].animatedOverlay = 0;
    extras[index].drawOverlayOnTop = 0;
    extras[index].nextIndex = 0;
}

VA(0x0040b266, 0x130)
i32 fullMap::GetNewCellExtraIndex(void) {
    i32 nb;
    mapCellExtra* i;
    i32 j;

    for (nb = 1; nb < extraCount; nb++) {
        if (extras[nb].nextIndex == IDX(MAPCELL_EXTRA_FREE)) {
            ClearCellExtra(nb);
            return nb;
        }
    }
    i = static_cast<mapCellExtra*>(operator new((extraCount + 100) * sizeof(mapCellExtra)));
    memcpy(i, extras, extraCount * sizeof(mapCellExtra));
    delete extras;
    extras = i;
    for (j = extraCount; j < extraCount + 100; j++)
        extras[j].nextIndex = IDX(MAPCELL_EXTRA_FREE);
    extraCount += 100;
    ClearCellExtra(extraCount - 100);
    return extraCount - 100;
}

VA(0x0040b73c, 0x9e)
void fullMap::Write(i32 handle) {
    write(handle, &width, sizeof(width));
    write(handle, &height, sizeof(height));
    write(handle, cells, width * height * sizeof(mapCell));
    write(handle, &extraCount, sizeof(extraCount));
    write(handle, extras, extraCount * sizeof(mapCellExtra));
}

VA(0x0040b7da, 0x295)
void fullMap::Read(i32 handle, i32 convert) {
    i32 nb;
    oldMapCell* tmp;
    i32 x, y;
    oldMapCellExtra* tmp2;

    read(handle, &width, sizeof(width));
    read(handle, &height, sizeof(height));
    Init(width, height);
    if (convert) {
        tmp = static_cast<oldMapCell*>(operator new(width * height * sizeof(oldMapCell)));
        read(handle, tmp, width * height * sizeof(oldMapCell));
        for (x = 0; x < width; x++)
            for (y = 0; y < height; y++)
                memcpy(&cells[width * y + x], tmp + width * y + x, sizeof(mapCell));
        delete tmp;
    } else {
        read(handle, cells, width * height * sizeof(mapCell));
    }
    read(handle, &extraCount, sizeof(extraCount));
    if (extras)
        delete extras;
    extras = static_cast<mapCellExtra*>(operator new(extraCount * sizeof(mapCellExtra)));
    if (convert) {
        tmp2 = static_cast<oldMapCellExtra*>(operator new(extraCount * sizeof(oldMapCellExtra)));
        read(handle, tmp2, extraCount * sizeof(oldMapCellExtra));
        for (nb = 0; nb < extraCount; nb++)
            memcpy(extras + nb, tmp2 + nb, sizeof(mapCellExtra));
        delete tmp2;
    } else {
        read(handle, extras, extraCount * sizeof(mapCellExtra));
    }
}

VA(0x0040b396, 0x1d3)
mapCellExtra* fullMap::GetNewCellExtraOverlay(i32 x, i32 y) {
    mapCellExtra* node;
    i32 ix;
    i32 ni;
    mapCell* cell;

    if (Row(y)[x].m_extraIndex == 0) {
        Cell(cell, x, y);
        cell->m_extraIndex = GetNewCellExtraIndex();
        return Extra(Row(y)[x].m_extraIndex);
    } else {
        ix = Row(y)[x].m_extraIndex;
        node = Extra(Row(y)[x].m_extraIndex);
        for (;;) {
            if (node->overlayIndex == IDX(MAPCELL_SPRITE_NONE))
                return node;
            if (node->nextIndex == 0) {
                ni = GetNewCellExtraIndex();
                node = Extra(ix);
                node->nextIndex = ni;
                return Extra(node->nextIndex);
            } else {
                ix = node->nextIndex;
                node = Extra(node->nextIndex);
            }
        }
    }
}

VA(0x0040b569, 0x1d3)
mapCellExtra* fullMap::GetNewCellExtraObject(i32 x, i32 y) {
    mapCellExtra* node;
    i32 ix;
    i32 ni;
    mapCell* cell;

    if (Row(y)[x].m_extraIndex == 0) {
        Cell(cell, x, y);
        cell->m_extraIndex = GetNewCellExtraIndex();
        return Extra(Row(y)[x].m_extraIndex);
    } else {
        ix = Row(y)[x].m_extraIndex;
        node = Extra(Row(y)[x].m_extraIndex);
        for (;;) {
            if (node->objectIndex == IDX(MAPCELL_SPRITE_NONE))
                return node;
            if (node->nextIndex == 0) {
                ni = GetNewCellExtraIndex();
                node = Extra(ix);
                node->nextIndex = ni;
                return Extra(node->nextIndex);
            } else {
                ix = node->nextIndex;
                node = Extra(node->nextIndex);
            }
        }
    }
}

// Updates a cell directly or appends to its extra-object chain.
VA(0x0040ba6f, 0x2ea)
void fullMap::ChangeTilesetIndex(
    mapCell* cell,
    i32 x,
    i32 y,
    i32 tileset,
    i32 index,
    i32 overlay,
    i32
) {
    i32 idx;
    mapCellExtra* ptr;
    i32 t;
    i32 dummy;

    ptr = NULL;
    if (index == IDX(MAPCELL_SPRITE_NONE))
        t = 0;
    else
        t = tileset;

    if (overlay == 0) {
        if (cell->m_objectIndex != IDX(MAPCELL_SPRITE_NONE) && cell->m_objectTileset != tileset) {
            idx = cell->m_extraIndex;
            while (idx != 0) {
                ptr = Extra(idx);
                if (ptr->objectIndex != IDX(MAPCELL_SPRITE_NONE) && ptr->objectTileset != tileset) {
                    idx = ptr->nextIndex;
                } else {
                    ptr->animatedObject = 0;
                    ptr->objectLayerBit0 = 0;
                    ptr->objectLayerBit1 = 0;
                    ptr->objectDrawnAsOverlay = 0;
                    ptr->objectTileset = t;
                    ptr->objectIndex = index;
                    break;
                }
            }
            if (idx == 0) {
                ptr = GetNewCellExtraObject(x, y);
                ptr->objectTileset = t;
                ptr->objectIndex = index;
            }
        } else {
            cell->m_animatedObject = 0;
            cell->m_objectLayerBit0 = 0;
            cell->m_objectLayerBit1 = 0;
            cell->m_objectDrawnAsOverlay = 0;
            cell->m_objectTileset = static_cast<u8>(t);
            cell->m_objectIndex = index;
        }
    } else {
        if (cell->m_overlayIndex != IDX(MAPCELL_SPRITE_NONE) && cell->m_overlayTileset != tileset) {
            idx = cell->m_extraIndex;
            while (idx != 0) {
                ptr = Extra(idx);
                if (ptr->overlayIndex != IDX(MAPCELL_SPRITE_NONE) && ptr->overlayTileset != tileset) {
                    idx = ptr->nextIndex;
                } else {
                    ptr->animatedOverlay = 0;
                    ptr->drawOverlayOnTop = 0;
                    ptr->overlayTileset = t;
                    ptr->overlayIndex = index;
                    break;
                }
            }
            if (idx == 0) {
                ptr = GetNewCellExtraOverlay(x, y);
                ptr->overlayTileset = t;
                ptr->overlayIndex = index;
            }
        } else {
            cell->m_animatedOverlay = 0;
            cell->m_drawOverlayOnTop = 0;
            cell->m_overlayTileset = t;
            cell->m_overlayIndex = index;
        }
    }
}
