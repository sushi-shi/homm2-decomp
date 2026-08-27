#include <Ints.h>
#include "EDITOR/fullMap.h"
#include "EDITOR/mapcell.h"
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/Localization.h>
#include <SOURCE/REQUEST.h>
#include <string.h>
#include <PLATFORM/File.h>

typedef enum MapCellExtraConstant {
    EXTRA_ALLOCATION_STEP = 100
} MapCellExtraConstant;

namespace {

void ReadMapData(i32 file, void* buffer, i32 count) {
    if (!platform::FileReadExact(file, buffer, count))
        ShutDown(localization::Tr("system.file.read_error"));
}

void WriteMapData(i32 file, const void* buffer, i32 count) {
    if (!platform::FileWriteExact(file, buffer, count))
        ShutDown(localization::Tr("system.file.write_error"));
}

void RequireValidMapDimensions(i32 width, i32 height) {
    if (width <= 0 || height <= 0 || width > MAP_DIMENSION_XLARGE
        || height > MAP_DIMENSION_XLARGE)
        ShutDown(localization::Tr("system.file.read_error"));
}

void RequireRecordsFit(i32 file, i32 count, i32 recordSize) {
    const i32 position = platform::FileTell(file);
    const i32 length = platform::FileLength(file);
    if (count < 0 || recordSize <= 0 || position < 0 || length < position
        || count > (length - position) / recordSize)
        ShutDown(localization::Tr("system.file.read_error"));
}

}

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
        H2_FREE(cells);
    cells = NULL;
    if (extras)
        H2_FREE(extras);
    extras = NULL;
    extraCount = 0;
}

void fullMap::Init(i32 w, i32 h) {

    width = w;
    height = h;
    Close();
    cells = static_cast<mapCell*>(H2_ALLOC(width * height * sizeof(mapCell)));
}

void fullMap::ClearCellExtra(i32 index) {
    extras[index].SetObjectTileset(TILESET_NONE);
    extras[index].objectIndex = MAPCELL_SPRITE_NONE;
    extras[index].animatedObject = 0;
    extras[index].objectLayerBit0 = 0;
    extras[index].objectLayerBit1 = 0;
    extras[index].objectDrawnAsOverlay = 0;
    extras[index].SetOverlayTileset(TILESET_NONE);
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
    H2_FREE(extras);
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
    WriteMapData(handle, &width, sizeof(width));
    WriteMapData(handle, &height, sizeof(height));
    WriteMapData(handle, cells, width * height * sizeof(mapCell));
    WriteMapData(handle, &extraCount, sizeof(extraCount));
    WriteMapData(handle, extras, extraCount * sizeof(mapCellExtra));
}

void fullMap::Read(i32 handle, i32 convert) {
    i32 nb;
    oldMapCell* tmp1;
    i32 x, y;
    oldMapCellExtra* tmp2;

    ReadMapData(handle, &width, sizeof(width));
    ReadMapData(handle, &height, sizeof(height));
    RequireValidMapDimensions(width, height);
    Init(width, height);
    if (convert) {
        RequireRecordsFit(handle, width * height, sizeof(oldMapCell));
        tmp1 = static_cast<oldMapCell*>(H2_ALLOC(width * height * sizeof(oldMapCell)));
        ReadMapData(handle, tmp1, width * height * sizeof(oldMapCell));
        for (x = 0; x < width; x++)
            for (y = 0; y < height; y++)
                memcpy(cells + x + y * width, tmp1 + x + y * width, sizeof(mapCell));
        H2_FREE(tmp1);
    } else {
        RequireRecordsFit(handle, width * height, sizeof(mapCell));
        ReadMapData(handle, cells, width * height * sizeof(mapCell));
    }
    ReadMapData(handle, &extraCount, sizeof(extraCount));
    if (extras)
        H2_FREE(extras);
    if (convert) {
        RequireRecordsFit(handle, extraCount, sizeof(oldMapCellExtra));
        extras = static_cast<mapCellExtra*>(H2_ALLOC(extraCount * sizeof(mapCellExtra)));
        tmp2 = static_cast<oldMapCellExtra*>(H2_ALLOC(extraCount * sizeof(oldMapCellExtra)));
        ReadMapData(handle, tmp2, extraCount * sizeof(oldMapCellExtra));
        for (nb = 0; nb < extraCount; nb++)
            memcpy(extras + nb, tmp2 + nb, sizeof(mapCellExtra));
        H2_FREE(tmp2);
    } else {
        RequireRecordsFit(handle, extraCount, sizeof(mapCellExtra));
        extras = static_cast<mapCellExtra*>(H2_ALLOC(extraCount * sizeof(mapCellExtra)));
        ReadMapData(handle, extras, extraCount * sizeof(mapCellExtra));
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

    ptr = NULL;
    t = index != MAPCELL_SPRITE_NONE ? tileset : TILESET_NONE;

    if (overlay == 0) {
        if (cell->m_objectIndex != MAPCELL_SPRITE_NONE && cell->ObjectTileset() != tileset) {
            idx = cell->m_extraIndex;
            while (idx != 0) {
                ptr = Extra(idx);
                if (ptr->objectIndex != MAPCELL_SPRITE_NONE && ptr->ObjectTileset() != tileset) {
                    idx = ptr->nextIndex;
                } else {
                    ptr->animatedObject = 0;
                    ptr->objectLayerBit0 = 0;
                    ptr->objectLayerBit1 = 0;
                    ptr->objectDrawnAsOverlay = 0;
                    ptr->SetObjectTileset(t);
                    ptr->objectIndex = index;
                    break;
                }
            }
            if (idx == 0) {
                ptr = GetNewCellExtraObject(x, y);
                ptr->SetObjectTileset(t);
                ptr->objectIndex = index;
            }
        } else {
            cell->m_animatedObject = 0;
            cell->m_objectLayerBit0 = 0;
            cell->m_objectLayerBit1 = 0;
            cell->m_objectDrawnAsOverlay = 0;
            cell->SetObjectTileset(t);
            cell->m_objectIndex = index;
        }
    } else {
        if (cell->m_overlayIndex != MAPCELL_SPRITE_NONE && cell->OverlayTileset() != tileset) {
            idx = cell->m_extraIndex;
            while (idx != 0) {
                ptr = Extra(idx);
                if (ptr->overlayIndex != MAPCELL_SPRITE_NONE && ptr->OverlayTileset() != tileset) {
                    idx = ptr->nextIndex;
                } else {
                    ptr->animatedOverlay = 0;
                    ptr->drawOverlayOnTop = 0;
                    ptr->SetOverlayTileset(t);
                    ptr->overlayIndex = index;
                    break;
                }
            }
            if (idx == 0) {
                ptr = GetNewCellExtraOverlay(x, y);
                ptr->SetOverlayTileset(t);
                ptr->overlayIndex = index;
            }
        } else {
            cell->m_animatedOverlay = 0;
            cell->m_drawOverlayOnTop = 0;
            cell->SetOverlayTileset(t);
            cell->m_overlayIndex = index;
        }
    }
}
