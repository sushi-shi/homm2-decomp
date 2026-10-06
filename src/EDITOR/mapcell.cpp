#include <Ints.h>
#include "EDITOR/fullMap.h"
#include "EDITOR/mapcell.h"
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <string.h>
#include <io.h>
#ifdef HOMM2_EDITOR
#include <EDITOR/EDITOR.h>
#endif

typedef enum MapCellExtraConstant {
    EXTRA_ALLOCATION_STEP = 100
} MapCellExtraConstant;

#ifdef HOMM2_EDITOR


typedef enum EditMapExtraConstant {
    EXTRA_POOL_SIZE         = 0xffff,
    EXTRA_REMAP_TABLE_BYTES = 0x20064,
    EXTRA_COMPACT_SLACK     = 0x95
} EditMapExtraConstant;
#endif

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

#ifdef HOMM2_EDITOR
void fullMap::Init(i32 mapWidth, i32 mapHeight) {
    i32 index;

    width = mapWidth;
    height = mapHeight;
    Close();
    cells = static_cast<mapCell*>(H2_ALLOC(width * height * sizeof(mapCell)));
    extras = static_cast<mapCellExtra*>(H2_ALLOC(EXTRA_POOL_SIZE * sizeof(mapCellExtra)));
    extraCount = EXTRA_POOL_SIZE;
    for (index = 0; index < EXTRA_POOL_SIZE; index++)
        extras[index].nextIndex = MAPCELL_EXTRA_FREE;
}
#else
void fullMap::Init(i32 mapWidth, i32 mapHeight) {
    i32 unused [[maybe_unused]];
    width = mapWidth;
    height = mapHeight;
    Close();
    cells = static_cast<mapCell*>(H2_ALLOC(width * height * sizeof(mapCell)));
}
#endif

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
#ifdef HOMM2_EDITOR
    extras[index].objectLink = 0;
    extras[index].overlayLink = 0;
#endif
}

#ifdef HOMM2_EDITOR
void fullMap::Copy(fullMap& source) {
    delete extras;
    extraCount = source.extraCount;
    extras = static_cast<mapCellExtra*>(H2_ALLOC(extraCount * sizeof(mapCellExtra)));
    memcpy(cells, source.cells, width * sizeof(mapCell) * height);
    memcpy(extras, source.extras, extraCount * sizeof(mapCellExtra));
}
#endif

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

#ifdef HOMM2_EDITOR


void fullMap::RemoveExtraObject(i32 index) {
    mapCellExtra* extra;
    i32 nextIndex_;
    mapCellExtra* next;

    extra = &extras[index];
    nextIndex_ = extra->nextIndex;
    if (nextIndex_ != 0 && extras[nextIndex_].objectIndex != MAPCELL_SPRITE_NONE) {
        next = &extras[nextIndex_];
        extra->objectLink = next->objectLink;
        extra->objectTileset = next->objectTileset;
        extra->objectIndex = next->objectIndex;
        extra->animatedObject = next->animatedObject;
        extra->objectLayerBit0 = next->objectLayerBit0;
        extra->objectLayerBit1 = next->objectLayerBit1;
        extra->objectDrawnAsOverlay = next->objectDrawnAsOverlay;
        RemoveExtraObject(nextIndex_);
        if (next->objectIndex == MAPCELL_SPRITE_NONE && next->overlayIndex == MAPCELL_SPRITE_NONE) {
            extra->nextIndex = 0;
            next->nextIndex = MAPCELL_EXTRA_FREE;
        }
    } else {
        extra->objectLink = 0;
        extra->objectTileset = TILESET_NONE;
        extra->objectIndex = MAPCELL_SPRITE_NONE;
        extra->animatedObject = 0;
        extra->objectLayerBit0 = 0;
        extra->objectLayerBit1 = 0;
        extra->objectDrawnAsOverlay = 0;
    }
}


void fullMap::RemoveCellObject(i32 x, i32 y) {
    mapCellExtra* extra;
    i32 nextIndex_;
    mapCell* cell;

    cell = &Column(x)[y * width];
    nextIndex_ = cell->m_extraIndex;
    if (nextIndex_ != 0 && extras[nextIndex_].objectIndex != MAPCELL_SPRITE_NONE) {
        extra = &extras[nextIndex_];
        cell->m_objectLink = extra->objectLink;
        cell->m_objectTileset = extra->objectTileset;
        cell->m_objectIndex = extra->objectIndex;
        cell->m_animatedObject = extra->animatedObject;
        cell->m_objectLayerBit0 = extra->objectLayerBit0;
        cell->m_objectLayerBit1 = extra->objectLayerBit1;
        cell->m_objectDrawnAsOverlay = extra->objectDrawnAsOverlay;
        cell->m_triggerType = MAP_OBJECT_NONE;
        cell->m_objectMetadata = 0;
        RemoveExtraObject(nextIndex_);
        if (extra->objectIndex == MAPCELL_SPRITE_NONE && extra->overlayIndex == MAPCELL_SPRITE_NONE) {
            cell->m_extraIndex = 0;
            extra->nextIndex = MAPCELL_EXTRA_FREE;
        }
    } else {
        cell->m_objectLink = 0;
        cell->m_objectTileset = TILESET_NONE;
        cell->m_objectIndex = MAPCELL_SPRITE_NONE;
        cell->m_animatedObject = 0;
        cell->m_objectLayerBit0 = 0;
        cell->m_objectLayerBit1 = 0;
        cell->m_objectDrawnAsOverlay = 0;
        cell->m_objectMetadata = 0;
        cell->m_triggerType = MAP_OBJECT_NONE;
    }
}


void fullMap::PushCellObject(i32 x, i32 y) {
    mapCellExtra* extra;
    mapCell* cell;

    cell = &Column(x)[y * width];
    extra = gMap.GetNewCellExtraObject(x, y);
    extra->animatedObject = cell->m_animatedObject;
    extra->objectTileset = cell->m_objectTileset;
    extra->objectIndex = cell->m_objectIndex;
    extra->objectLayerBit0 = cell->m_objectLayerBit0;
    extra->objectLayerBit1 = cell->m_objectLayerBit1;
    extra->objectDrawnAsOverlay = cell->m_objectDrawnAsOverlay;
    extra->objectLink = cell->m_objectLink;
    cell->m_animatedObject = 0;
    cell->m_objectTileset = TILESET_NONE;
    cell->m_objectIndex = MAPCELL_SPRITE_NONE;
    cell->m_objectLayerBit0 = 0;
    cell->m_objectLayerBit1 = 0;
    cell->m_objectDrawnAsOverlay = 0;
    cell->m_objectLink = 0;
    cell->m_triggerType = MAP_OBJECT_NONE;
}


void fullMap::RemoveExtraOverlay(i32 index) {
    mapCellExtra* extra;
    i32 nextIndex_;
    mapCellExtra* next;

    extra = &extras[index];
    nextIndex_ = extra->nextIndex;
    if (nextIndex_ != 0 && extras[nextIndex_].overlayIndex != MAPCELL_SPRITE_NONE) {
        next = &extras[nextIndex_];
        extra->overlayLink = next->overlayLink;
        extra->overlayTileset = next->overlayTileset;
        extra->overlayIndex = next->overlayIndex;
        extra->animatedOverlay = next->animatedOverlay;
        extra->drawOverlayOnTop = next->drawOverlayOnTop;
        RemoveExtraOverlay(nextIndex_);
        if (next->overlayIndex == MAPCELL_SPRITE_NONE && next->overlayIndex == MAPCELL_SPRITE_NONE) {
            extra->nextIndex = 0;
            next->nextIndex = MAPCELL_EXTRA_FREE;
        }
    } else {
        extra->overlayLink = 0;
        extra->overlayTileset = TILESET_NONE;
        extra->overlayIndex = MAPCELL_SPRITE_NONE;
        extra->animatedOverlay = 0;
        extra->drawOverlayOnTop = 0;
    }
}


void fullMap::RemoveCellOverlay(i32 x, i32 y) {
    mapCellExtra* extra;
    i32 nextIndex_;
    mapCell* cell;

    cell = &Column(x)[y * width];
    nextIndex_ = cell->m_extraIndex;
    if (nextIndex_ != 0 && extras[nextIndex_].overlayIndex != MAPCELL_SPRITE_NONE) {
        extra = &extras[nextIndex_];
        cell->m_overlayLink = extra->overlayLink;
        cell->m_overlayTileset = extra->overlayTileset;
        cell->m_overlayIndex = extra->overlayIndex;
        cell->m_animatedOverlay = extra->animatedOverlay;
        cell->m_drawOverlayOnTop = extra->drawOverlayOnTop;
        RemoveExtraOverlay(nextIndex_);
        if (extra->overlayIndex == MAPCELL_SPRITE_NONE && extra->overlayIndex == MAPCELL_SPRITE_NONE) {
            cell->m_extraIndex = 0;
            extra->nextIndex = MAPCELL_EXTRA_FREE;
        }
    } else {
        cell->m_overlayLink = 0;
        cell->m_overlayTileset = TILESET_NONE;
        cell->m_overlayIndex = MAPCELL_SPRITE_NONE;
        cell->m_animatedOverlay = 0;
        cell->m_drawOverlayOnTop = 0;
    }
}


void fullMap::Compact(void) {
    u16* remap;
    i32 freeIndex;
    i32 index;
    i32 x;
    i32 y;
    i32 i;
    mapCellExtra* newExtras;
    i32 slack;

    remap = static_cast<u16*>(H2_ALLOC(EXTRA_REMAP_TABLE_BYTES));
    freeIndex = 1;
    for (index = 1; index < extraCount; index++) {
        if (extras[index].nextIndex != MAPCELL_EXTRA_FREE) {
            if (freeIndex != index) {
                for (; freeIndex < EXTRA_POOL_SIZE; freeIndex++) {
                    if (extras[freeIndex].nextIndex == MAPCELL_EXTRA_FREE)
                        break;
                }
                extras[freeIndex] = extras[index];
                extras[index].nextIndex = MAPCELL_EXTRA_FREE;
            }
            remap[index] = freeIndex;
            freeIndex++;
        }
    }
    remap[0] = 0;
    remap[MAPCELL_EXTRA_FREE] = MAPCELL_EXTRA_FREE;
    extraCount = freeIndex;
    for (x = 0; x < MAP_WIDTH; x++)
        for (y = 0; y < MAP_HEIGHT; y++)
            Column(x)[y * width].m_extraIndex = remap[Column(x)[y * width].m_extraIndex];
    for (i = 1; i < extraCount; i++)
        extras[i].nextIndex = remap[extras[i].nextIndex];
    delete remap;
    slack = EXTRA_COMPACT_SLACK;
    newExtras = static_cast<mapCellExtra*>(H2_ALLOC((extraCount + slack) * sizeof(mapCellExtra)));
    memcpy(newExtras, extras, extraCount * sizeof(mapCellExtra));
    delete extras;
    extras = newExtras;
    for (i = extraCount; i < extraCount + slack; i++)
        extras[i].nextIndex = MAPCELL_EXTRA_FREE;
    extraCount += slack;
    ClearCellExtra(extraCount - slack);
}
#endif

void fullMap::Write(i32 handle) {
    WRITE_FILE_VALUE(handle, width);
    WRITE_FILE_VALUE(handle, height);
    write(handle, cells, width * height * sizeof(mapCell));
    WRITE_FILE_VALUE(handle, extraCount);
    write(handle, extras, extraCount * sizeof(mapCellExtra));
}

#ifdef HOMM2_EDITOR
void fullMap::Read(i32 handle, i32) {
    i32 extraIndex;

    READ_FILE_VALUE(handle, width);
    READ_FILE_VALUE(handle, height);
    Init(width, height);
    read(handle, cells, width * height * sizeof(mapCell));
    READ_FILE_VALUE(handle, extraCount);
    read(handle, extras, extraCount * sizeof(mapCellExtra));
    for (extraIndex = extraCount; extraIndex < EXTRA_POOL_SIZE; extraIndex++)
        extras[extraIndex].nextIndex = MAPCELL_EXTRA_FREE;
}
#else
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
#endif

void fullMap::ChangeTilesetIndex(
    mapCell* cell,
    i32 x,
    i32 y,
    TilesetId tileset,
    i32 index,
    i32 overlay,
    i32 link [[maybe_unused]]
) {
    i32 extraIndex;
    mapCellExtra* extra;
    TilesetId newTileset;
    i32 unused [[maybe_unused]];

    extra = NULL;
#ifdef HOMM2_EDITOR
    if (link == -1)
        link = cell->m_objectLink;
#endif
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
#ifdef HOMM2_EDITOR
                    extra->objectLink = link;
#endif
                    break;
                }
            }
            if (extraIndex == 0) {
                extra = GetNewCellExtraObject(x, y);
                extra->objectTileset = newTileset;
                extra->objectIndex = index;
#ifdef HOMM2_EDITOR
                extra->objectLink = link;
#endif
            }
        } else {
            cell->m_animatedObject = 0;
            cell->m_objectLayerBit0 = 0;
            cell->m_objectLayerBit1 = 0;
            cell->m_objectDrawnAsOverlay = 0;
            cell->m_objectTileset = newTileset;
            cell->m_objectIndex = index;
#ifdef HOMM2_EDITOR
            cell->m_objectLink = link;
#endif
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
#ifdef HOMM2_EDITOR
                    extra->overlayLink = link;
#endif
                    break;
                }
            }
            if (extraIndex == 0) {
                extra = GetNewCellExtraOverlay(x, y);
                extra->overlayTileset = newTileset;
                extra->overlayIndex = index;
#ifdef HOMM2_EDITOR
                extra->overlayLink = link;
#endif
            }
        } else {
            cell->m_animatedOverlay = 0;
            cell->m_drawOverlayOnTop = 0;
            cell->m_overlayTileset = newTileset;
            cell->m_overlayIndex = index;
#ifdef HOMM2_EDITOR
            cell->m_overlayLink = link;
#endif
        }
    }
}
