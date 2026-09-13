#include <Ints.h>
#include "EDITOR/fullMap.h"
#include "EDITOR/mapcell.h"
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <string.h>
#include <io.h>

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

void fullMap::Init(i32 mapWidth, i32 mapHeight) {
    i32 unused [[maybe_unused]];
    width = mapWidth;
    height = mapHeight;
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
    i32 extraIndex;
    mapCellExtra* newExtras;
    i32 j;

    for (extraIndex = 1; extraIndex < extraCount; extraIndex++) {
        if (extras[extraIndex].nextIndex == MAPCELL_EXTRA_FREE) {
            ClearCellExtra(extraIndex);
            return extraIndex;
        }
    }
    newExtras = static_cast<mapCellExtra*>(
        H2_ALLOC((extraCount + EXTRA_ALLOCATION_STEP) * sizeof(mapCellExtra))
    );
    memcpy(newExtras, extras, extraCount * sizeof(mapCellExtra));
    delete extras;
    extras = newExtras;
    for (j = extraCount; j < extraCount + EXTRA_ALLOCATION_STEP; j++)
        extras[j].nextIndex = MAPCELL_EXTRA_FREE;
    extraCount += EXTRA_ALLOCATION_STEP;
    ClearCellExtra(extraCount - EXTRA_ALLOCATION_STEP);
    return extraCount - EXTRA_ALLOCATION_STEP;
}

mapCellExtra* fullMap::GetNewCellExtraOverlay(i32 x, i32 y) {
    mapCellExtra* node;
    i32 index;
    i32 newExtraIndex;
    mapCell* cell;

    if (Column(x)[y * width].m_extraIndex == 0) {
        Cell(cell, x, y);
        cell->m_extraIndex = GetNewCellExtraIndex();
        return &extras[Column(x)[y * width].m_extraIndex];
    } else {
        index = Column(x)[y * width].m_extraIndex;
        node = &extras[Column(x)[y * width].m_extraIndex];
        for (;;) {
            if (node->overlayIndex == MAPCELL_SPRITE_NONE)
                return node;
            if (node->nextIndex == 0) {
                newExtraIndex = GetNewCellExtraIndex();
                node = Extra(index);
                node->nextIndex = newExtraIndex;
                return Extra(node->nextIndex);
            } else {
                index = node->nextIndex;
                node = Extra(node->nextIndex);
            }
        }
    }
}

mapCellExtra* fullMap::GetNewCellExtraObject(i32 x, i32 y) {
    mapCellExtra* node;
    i32 index;
    i32 newExtraIndex;
    mapCell* cell;

    if (Column(x)[y * width].m_extraIndex == 0) {
        Cell(cell, x, y);
        cell->m_extraIndex = GetNewCellExtraIndex();
        return &extras[Column(x)[y * width].m_extraIndex];
    } else {
        index = Column(x)[y * width].m_extraIndex;
        node = &extras[Column(x)[y * width].m_extraIndex];
        for (;;) {
            if (node->objectIndex == MAPCELL_SPRITE_NONE)
                return node;
            if (node->nextIndex == 0) {
                newExtraIndex = GetNewCellExtraIndex();
                node = Extra(index);
                node->nextIndex = newExtraIndex;
                return Extra(node->nextIndex);
            } else {
                index = node->nextIndex;
                node = Extra(node->nextIndex);
            }
        }
    }
}

void fullMap::Write(i32 handle) {
    WRITE_FILE_VALUE(handle, width);
    WRITE_FILE_VALUE(handle, height);
    write(handle, cells, width * height * sizeof(mapCell));
    WRITE_FILE_VALUE(handle, extraCount);
    write(handle, extras, extraCount * sizeof(mapCellExtra));
}

void fullMap::Read(i32 handle, i32 convert) {
    i32 extraIndex;
    oldMapCell* oldCells;
    i32 x, y;
    oldMapCellExtra* oldExtras;

    READ_FILE_VALUE(handle, width);
    READ_FILE_VALUE(handle, height);
    Init(width, height);
    if (convert) {
        oldCells = static_cast<oldMapCell*>(H2_ALLOC(width * height * sizeof(oldMapCell)));
        read(handle, oldCells, width * height * sizeof(oldMapCell));
        for (x = 0; x < width; x++)
            for (y = 0; y < height; y++)
                memcpy(cells + x + y * width, oldCells + x + y * width, sizeof(mapCell));
        delete oldCells;
    } else {
        read(handle, cells, width * height * sizeof(mapCell));
    }
    READ_FILE_VALUE(handle, extraCount);
    if (extras)
        delete extras;
    extras = static_cast<mapCellExtra*>(H2_ALLOC(extraCount * sizeof(mapCellExtra)));
    if (convert) {
        oldExtras = static_cast<oldMapCellExtra*>(H2_ALLOC(extraCount * sizeof(oldMapCellExtra)));
        read(handle, oldExtras, extraCount * sizeof(oldMapCellExtra));
        for (extraIndex = 0; extraIndex < extraCount; extraIndex++)
            memcpy(extras + extraIndex, oldExtras + extraIndex, sizeof(mapCellExtra));
        delete oldExtras;
    } else {
        read(handle, extras, extraCount * sizeof(mapCellExtra));
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
    i32 extraIndex;
    mapCellExtra* extra;
    TilesetId newTileset;
    i32 unused [[maybe_unused]];

    extra = NULL;
    newTileset = index != MAPCELL_SPRITE_NONE ? tileset : TILESET_NONE;

    if (overlay == 0) {
        if (cell->m_objectIndex != MAPCELL_SPRITE_NONE && cell->m_objectTileset != tileset) {
            extraIndex = cell->m_extraIndex;
            while (extraIndex != 0) {
                extra = Extra(extraIndex);
                if (extra->objectIndex != MAPCELL_SPRITE_NONE && extra->objectTileset != tileset) {
                    extraIndex = extra->nextIndex;
                } else {
                    extra->animatedObject = 0;
                    extra->objectLayerBit0 = 0;
                    extra->objectLayerBit1 = 0;
                    extra->objectDrawnAsOverlay = 0;
                    extra->objectTileset = newTileset;
                    extra->objectIndex = index;
                    break;
                }
            }
            if (extraIndex == 0) {
                extra = GetNewCellExtraObject(x, y);
                extra->objectTileset = newTileset;
                extra->objectIndex = index;
            }
        } else {
            cell->m_animatedObject = 0;
            cell->m_objectLayerBit0 = 0;
            cell->m_objectLayerBit1 = 0;
            cell->m_objectDrawnAsOverlay = 0;
            cell->m_objectTileset = newTileset;
            cell->m_objectIndex = index;
        }
    } else {
        if (cell->m_overlayIndex != MAPCELL_SPRITE_NONE && cell->m_overlayTileset != tileset) {
            extraIndex = cell->m_extraIndex;
            while (extraIndex != 0) {
                extra = Extra(extraIndex);
                if (extra->overlayIndex != MAPCELL_SPRITE_NONE && extra->overlayTileset != tileset) {
                    extraIndex = extra->nextIndex;
                } else {
                    extra->animatedOverlay = 0;
                    extra->drawOverlayOnTop = 0;
                    extra->overlayTileset = newTileset;
                    extra->overlayIndex = index;
                    break;
                }
            }
            if (extraIndex == 0) {
                extra = GetNewCellExtraOverlay(x, y);
                extra->overlayTileset = newTileset;
                extra->overlayIndex = index;
            }
        } else {
            cell->m_animatedOverlay = 0;
            cell->m_drawOverlayOnTop = 0;
            cell->m_overlayTileset = newTileset;
            cell->m_overlayIndex = index;
        }
    }
}
