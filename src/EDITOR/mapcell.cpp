// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\mapcell.obj   from: (directly linked into exe)
// functions: 11   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include "EDITOR/fullMap.h"
#include "EDITOR/mapcell.h"
#include <string.h>
#include <io.h>

VA(0x0040b070, 0x36)
fullMap::fullMap(void)
{
    cells = 0;
    extras = 0;
    extraCount = 0;
}

VA(0x0040b0a6, 0x1e)
fullMap::~fullMap(void)
{
    Close();
}

VA(0x0040b0c4, 0x81)
void fullMap::Close(void)
{
    if (cells)
        delete cells;
    cells = 0;
    if (extras)
        delete extras;
    extras = 0;
    extraCount = 0;
}

VA(0x0040b145, 0x53)
void fullMap::Init(int w, int h)
{
    int n;  // retail reserves one unused /Od slot at -0x4 (this spills to -0x8)
    width = w;
    height = h;
    Close();
    cells = static_cast<mapCell *>(operator new(width * height * sizeof(mapCell)));
}

VA(0x0040b198, 0xce)
void fullMap::ClearCellExtra(int index)
{
    extras[index].objTileset = 0;
    extras[index].objIndex = 0xFF;
    extras[index].objFlag = 0;
    extras[index].f4a = 0;
    extras[index].f4b = 0;
    extras[index].f4c = 0;
    extras[index].ovlTileset = 0;
    extras[index].ovlIndex = 0xFF;
    extras[index].ovlFlag0 = 0;
    extras[index].ovlFlag1 = 0;
    extras[index].index = 0;
}

VA(0x0040b266, 0x130)
int fullMap::GetNewCellExtraIndex(void)
{
    // NOTE: /Od stack-slot order is MSVC symbol-HASH order, not declaration order.
    // The retail frame is loop1@-4, newbuf@-8, loop2@-c; names are chosen so their
    // hashes sort in that order (hash(nb) < hash(i) < hash(j)). See docs/patterns/od-hash-slots.md.
    int nb;            // loop1 counter  -> -0x4
    mapCellExtra *i;   // grown buffer   -> -0x8
    int j;             // loop2 counter  -> -0xc

    for (nb = 1; nb < extraCount; nb++) {
        if (extras[nb].index == 0xFFFF) {
            ClearCellExtra(nb);
            return nb;
        }
    }
    i = static_cast<mapCellExtra *>(operator new((extraCount + 100) * sizeof(mapCellExtra)));
    memcpy(i, extras, extraCount * sizeof(mapCellExtra));
    delete extras;
    extras = i;
    for (j = extraCount; j < extraCount + 100; j++)
        extras[j].index = 0xFFFF;
    extraCount += 100;
    ClearCellExtra(extraCount - 100);
    return extraCount - 100;
}

VA(0x0040b73c, 0x9e)
void fullMap::Write(int handle)
{
    _write(handle, &width, sizeof(width));
    _write(handle, &height, sizeof(height));
    _write(handle, cells, width * height * sizeof(mapCell));
    _write(handle, &extraCount, sizeof(extraCount));
    _write(handle, extras, extraCount * sizeof(mapCellExtra));
}

// The legacy cell stride is 20 bytes, while the current record is 12 bytes. Keeping
// the destination byte offset factored by each current-record index reproduces the
// retail conversion loop's separate scaled y and x address terms.
VA(0x0040b7da, 0x295)
void fullMap::Read(int handle, int convert)
{
    // /Od slots are MSVC symbol-hash order: retail frame is
    //   extraLoop@-4, oldCells@-8, x@-c, y@-10, oldExtras@-14.
    int nb;                 // extras-convert loop counter -> -0x4
    oldMapCell *tmp;        // legacy cell scratch buffer  -> -0x8
    int x, y;               // cell convert loops          -> -0xc / -0x10
    oldMapCellExtra *tmp2;  // legacy extra scratch buffer -> -0x14

    _read(handle, &width, sizeof(width));
    _read(handle, &height, sizeof(height));
    Init(width, height);
    if (convert) {
        tmp = static_cast<oldMapCell *>(operator new(width * height * sizeof(oldMapCell)));
        _read(handle, tmp, width * height * sizeof(oldMapCell));
        for (x = 0; x < width; x++)
            for (y = 0; y < height; y++)
                memcpy(reinterpret_cast<unsigned char *>(cells) +
                           (width * y * sizeof(mapCell) + x * sizeof(mapCell)),
                       tmp + width * y + x, sizeof(mapCell));
        delete tmp;
    } else {
        _read(handle, cells, width * height * sizeof(mapCell));
    }
    _read(handle, &extraCount, sizeof(extraCount));
    if (extras)
        delete extras;
    extras = static_cast<mapCellExtra *>(operator new(extraCount * sizeof(mapCellExtra)));
    if (convert) {
        tmp2 = static_cast<oldMapCellExtra *>(operator new(extraCount * sizeof(oldMapCellExtra)));
        _read(handle, tmp2, extraCount * sizeof(oldMapCellExtra));
        for (nb = 0; nb < extraCount; nb++)
            memcpy(extras + nb, tmp2 + nb, sizeof(mapCellExtra));
        delete tmp2;
    } else {
        _read(handle, extras, extraCount * sizeof(mapCellExtra));
    }
}

// Cell's output-reference form places its /Ob1 continuation after the caller's
// cell-pointer assignment, matching the retail inline boundary.
VA(0x0040b396, 0x1d3)
mapCellExtra *fullMap::GetNewCellExtraOverlay(int x, int y)
{
    mapCellExtra *node;  // -0x4
    int ix;              // -0x8
    int ni;              // -0xc
    mapCell *cell;       // -0x10

    if (Row(y)[x].extra == 0) {
        Cell(cell, x, y);
        cell->extra = GetNewCellExtraIndex();
        return Extra(Row(y)[x].extra);
    } else {
        ix = Row(y)[x].extra;
        node = Extra(Row(y)[x].extra);
        for (;;) {
            if (node->ovlIndex == 0xFF)
                return node;
            if (node->index == 0) {
                ni = GetNewCellExtraIndex();
                node = Extra(ix);
                node->index = ni;
                return Extra(node->index);
            } else {
                ix = node->index;
                node = Extra(node->index);
            }
        }
    }
}

VA(0x0040b569, 0x1d3)
mapCellExtra *fullMap::GetNewCellExtraObject(int x, int y)
{
    mapCellExtra *node;  // -0x4
    int ix;              // -0x8
    int ni;              // -0xc
    mapCell *cell;       // -0x10

    if (Row(y)[x].extra == 0) {
        Cell(cell, x, y);
        cell->extra = GetNewCellExtraIndex();
        return Extra(Row(y)[x].extra);
    } else {
        ix = Row(y)[x].extra;
        node = Extra(Row(y)[x].extra);
        for (;;) {
            if (node->objIndex == 0xFF)
                return node;
            if (node->index == 0) {
                ni = GetNewCellExtraIndex();
                node = Extra(ix);
                node->index = ni;
                return Extra(node->index);
            } else {
                ix = node->index;
                node = Extra(node->index);
            }
        }
    }
}

// Sets the object (overlay==0) or overlay (overlay!=0) tileset+index on a cell:
// either directly on the cell (when its current obj/ovl is empty or already on the
// requested tileset) or by walking/appending the cell's extras chain.
// /Od slots are MSVC symbol-hash order: retail frame is idx@-4, ptr@-8, t@-c, with
// one reserved unused local @-10 (this spills to -0x14). Names chosen so their
// buckets (0,1,4,6) sort into that order. The 7th param is unused in retail.
//
// The explicit u8 conversion on the direct-cell object assignment preserves the
// field-first read/modify/write schedule used by retail after the earlier legacy
// allocation casts are expressed with C++ casts.
VA(0x0040ba6f, 0x2ea)
void fullMap::ChangeTilesetIndex(mapCell *cell, int x, int y, int tileset, int index, int overlay, int)
{
    int idx;             // chain index        -> -0x4
    mapCellExtra *ptr;   // current chain node -> -0x8
    int t;               // tileset to store   -> -0xc
    int dummy;           // reserved unused    -> -0x10

    ptr = 0;
    if (index == 0xFF)
        t = 0;
    else
        t = tileset;

    if (overlay == 0) {
        if (cell->objIndex != 0xFF && cell->objTileset != tileset) {
            idx = cell->extra;
            while (idx != 0) {
                ptr = Extra(idx);
                if (ptr->objIndex != 0xFF && ptr->objTileset != tileset) {
                    idx = ptr->index;
                } else {
                    ptr->objFlag = 0;
                    ptr->f4a = 0;
                    ptr->f4b = 0;
                    ptr->f4c = 0;
                    ptr->objTileset = t;
                    ptr->objIndex = index;
                    break;
                }
            }
            if (idx == 0) {
                ptr = GetNewCellExtraObject(x, y);
                ptr->objTileset = t;
                ptr->objIndex = index;
            }
        } else {
            cell->objFlag0 = 0;
            cell->w4a = 0;
            cell->w4b = 0;
            cell->w4c = 0;
            cell->objTileset = static_cast<u8>(t);
            cell->objIndex = index;
        }
    } else {
        if (cell->ovlIndex != 0xFF && cell->ovlTileset != tileset) {
            idx = cell->extra;
            while (idx != 0) {
                ptr = Extra(idx);
                if (ptr->ovlIndex != 0xFF && ptr->ovlTileset != tileset) {
                    idx = ptr->index;
                } else {
                    ptr->ovlFlag0 = 0;
                    ptr->ovlFlag1 = 0;
                    ptr->ovlTileset = t;
                    ptr->ovlIndex = index;
                    break;
                }
            }
            if (idx == 0) {
                ptr = GetNewCellExtraOverlay(x, y);
                ptr->ovlTileset = t;
                ptr->ovlIndex = index;
            }
        } else {
            cell->ovlFlag0 = 0;
            cell->ovlFlag1 = 0;
            cell->ovlTileset = t;
            cell->ovlIndex = index;
        }
    }
}
