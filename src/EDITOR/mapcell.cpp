#include <Ints.h>
#include "EDITOR/fullMap.h"
#include "EDITOR/mapcell.h"
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <string.h>
#include <PLATFORM/File.h>

typedef enum MapCellExtraConstant {
    EXTRA_ALLOCATION_STEP = 100
} MapCellExtraConstant;

fullMap::fullMap(void) {
    cells = NULL;
    extras = NULL;
    extraCount = 0;
}

fullMap::~fullMap(void) {
    Close();
}

void fullMap::Close(void) {
    if (cells)
        delete cells;
    cells = NULL;
    if (extras)
        delete extras;
    extras = NULL;
    extraCount = 0;
}

void fullMap::Init(i32 w, i32 h) {
    i32 n;
    width = w;
    height = h;
    Close();
    cells = static_cast<mapCell*>(H2_ALLOC(width * height * sizeof(mapCell)));
}

void fullMap::ClearCellExtra(i32 index) {
    extras[index].objectTileset = TILESET_NONE;
    extras[index].objectIndex = MAPCELL_SPRITE_NONE;
    extras[index].animatedObject = 0;
    extras[index].objectLayerBit0 = 0;
    extras[index].objectLayerBit1 = 0;
    extras[index].objectDrawnAsOverlay = 0;
    extras[index].overlayTileset = TILESET_NONE;
    extras[index].overlayIndex = MAPCELL_SPRITE_NONE;
    extras[index].animatedOverlay = 0;
    extras[index].drawOverlayOnTop = 0;
    extras[index].nextIndex = 0;
}

i32 fullMap::GetNewCellExtraIndex(void) {
    i32 n;
    mapCellExtra* i;
    i32 j;

    for (n = 1; n < extraCount; n++) {
        if (extras[n].nextIndex == MAPCELL_EXTRA_FREE) {
            ClearCellExtra(n);
            return n;
        }
    }
    i = static_cast<mapCellExtra*>(
        H2_ALLOC((extraCount + EXTRA_ALLOCATION_STEP) * sizeof(mapCellExtra))
    );
    memcpy(i, extras, extraCount * sizeof(mapCellExtra));
    delete extras;
    extras = i;
    for (j = extraCount; j < extraCount + EXTRA_ALLOCATION_STEP; j++)
        extras[j].nextIndex = MAPCELL_EXTRA_FREE;
    extraCount += EXTRA_ALLOCATION_STEP;
    ClearCellExtra(extraCount - EXTRA_ALLOCATION_STEP);
    return extraCount - EXTRA_ALLOCATION_STEP;
}

mapCellExtra* fullMap::GetNewCellExtraOverlay(i32 x, i32 y) {
    mapCellExtra* node;
    i32 ix;
    i32 ni;
    mapCell* cell;

    if (Column(x)[y * width].m_extraIndex == 0) {
        Cell(cell, x, y);
        cell->m_extraIndex = GetNewCellExtraIndex();
        return &extras[Column(x)[y * width].m_extraIndex];
    } else {
        ix = Column(x)[y * width].m_extraIndex;
        node = &extras[Column(x)[y * width].m_extraIndex];
        for (;;) {
            if (node->overlayIndex == MAPCELL_SPRITE_NONE)
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

mapCellExtra* fullMap::GetNewCellExtraObject(i32 x, i32 y) {
    mapCellExtra* node;
    i32 ix;
    i32 ni;
    mapCell* cell;

    if (Column(x)[y * width].m_extraIndex == 0) {
        Cell(cell, x, y);
        cell->m_extraIndex = GetNewCellExtraIndex();
        return &extras[Column(x)[y * width].m_extraIndex];
    } else {
        ix = Column(x)[y * width].m_extraIndex;
        node = &extras[Column(x)[y * width].m_extraIndex];
        for (;;) {
            if (node->objectIndex == MAPCELL_SPRITE_NONE)
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

void fullMap::Write(i32 handle) {
    platform::FileWrite(handle, &width, sizeof(width));
    platform::FileWrite(handle, &height, sizeof(height));
    platform::FileWrite(handle, cells, width * height * sizeof(mapCell));
    platform::FileWrite(handle, &extraCount, sizeof(extraCount));
    platform::FileWrite(handle, extras, extraCount * sizeof(mapCellExtra));
}

void fullMap::Read(i32 handle, i32 convert) {
    i32 nb;
    oldMapCell* tmp1;
    i32 x, y;
    oldMapCellExtra* tmp2;

    platform::FileRead(handle, &width, sizeof(width));
    platform::FileRead(handle, &height, sizeof(height));
    Init(width, height);
    if (convert) {
        tmp1 = static_cast<oldMapCell*>(H2_ALLOC(width * height * sizeof(oldMapCell)));
        platform::FileRead(handle, tmp1, width * height * sizeof(oldMapCell));
        for (x = 0; x < width; x++)
            for (y = 0; y < height; y++)
                memcpy(cells + x + y * width, tmp1 + x + y * width, sizeof(mapCell));
        delete tmp1;
    } else {
        platform::FileRead(handle, cells, width * height * sizeof(mapCell));
    }
    platform::FileRead(handle, &extraCount, sizeof(extraCount));
    if (extras)
        delete extras;
    extras = static_cast<mapCellExtra*>(H2_ALLOC(extraCount * sizeof(mapCellExtra)));
    if (convert) {
        tmp2 = static_cast<oldMapCellExtra*>(H2_ALLOC(extraCount * sizeof(oldMapCellExtra)));
        platform::FileRead(handle, tmp2, extraCount * sizeof(oldMapCellExtra));
        for (nb = 0; nb < extraCount; nb++)
            memcpy(extras + nb, tmp2 + nb, sizeof(mapCellExtra));
        delete tmp2;
    } else {
        platform::FileRead(handle, extras, extraCount * sizeof(mapCellExtra));
    }
}

void fullMap::ChangeTilesetIndex(
    mapCell* cell,
    i32 x,
    i32 y,
    TilesetId tileset,
    i32 index,
    i32 overlay,
    i32
) {
    i32 idx;
    mapCellExtra* ptr;
    TilesetId t;
    i32 a;

    ptr = NULL;
    t = index != MAPCELL_SPRITE_NONE ? tileset : TILESET_NONE;

    if (overlay == 0) {
        if (cell->m_objectIndex != MAPCELL_SPRITE_NONE && cell->m_objectTileset != tileset) {
            idx = cell->m_extraIndex;
            while (idx != 0) {
                ptr = Extra(idx);
                if (ptr->objectIndex != MAPCELL_SPRITE_NONE && ptr->objectTileset != tileset) {
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
            cell->m_objectTileset = t;
            cell->m_objectIndex = index;
        }
    } else {
        if (cell->m_overlayIndex != MAPCELL_SPRITE_NONE && cell->m_overlayTileset != tileset) {
            idx = cell->m_extraIndex;
            while (idx != 0) {
                ptr = Extra(idx);
                if (ptr->overlayIndex != MAPCELL_SPRITE_NONE && ptr->overlayTileset != tileset) {
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
