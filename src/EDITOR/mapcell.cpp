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
    cells = (mapCell *)operator new(width * height * sizeof(mapCell));
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
    i = (mapCellExtra *)operator new((extraCount + 100) * sizeof(mapCellExtra));
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

// ~98%: all slots + logic match; one convert-path memcpy dst (cells+width*y+x)
// schedules the cells-base add differently (regalloc artifact, see report).
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
        tmp = (oldMapCell *)operator new(width * height * sizeof(oldMapCell));
        _read(handle, tmp, width * height * sizeof(oldMapCell));
        for (x = 0; x < width; x++)
            for (y = 0; y < height; y++)
                memcpy(cells + width * y + x, tmp + width * y + x, sizeof(mapCell));
        delete tmp;
    } else {
        _read(handle, cells, width * height * sizeof(mapCell));
    }
    _read(handle, &extraCount, sizeof(extraCount));
    if (extras)
        delete extras;
    extras = (mapCellExtra *)operator new(extraCount * sizeof(mapCellExtra));
    if (convert) {
        tmp2 = (oldMapCellExtra *)operator new(extraCount * sizeof(oldMapCellExtra));
        _read(handle, tmp2, extraCount * sizeof(oldMapCellExtra));
        for (nb = 0; nb < extraCount; nb++)
            memcpy(extras + nb, tmp2 + nb, sizeof(mapCellExtra));
        delete tmp2;
    } else {
        _read(handle, extras, extraCount * sizeof(mapCellExtra));
    }
}

// @early-stop ~97%: cell/extra access goes through inline Row()/Extra() so /Ob1
// splices them in - matching retail's deferred Row(y)[x] addressing AND its jmp$+0
// count (10/10; raw expressions capped this at ~91%). Locals node/ix/ni/cell are
// named so the /Od hash drops them on retail's exact frame slots (-0x4/-0x8/-0xc/
// -0x10) via od_slots. Residual: ~2 inline-bracket jmp$+0 the compiler emits leading
// (after the `if`) vs retail's trailing (after the `cell` store) - an opaque MSVC /Od
// inline-bracketing choice that resisted accessor + statement-shape variants.
// See docs/patterns/inline-accessors.md, docs/patterns/od-hash-slots.md.
VA(0x0040b396, 0x1d3)
mapCellExtra *fullMap::GetNewCellExtraOverlay(int x, int y)
{
    mapCellExtra *node;  // -0x4
    int ix;              // -0x8
    int ni;              // -0xc
    mapCell *cell;       // -0x10

    if (Row(y)[x].extra == 0) {
        cell = &Row(y)[x];
        cell->extra = GetNewCellExtraIndex();
        return Extra(Row(y)[x].extra);
    }
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
        }
        ix = node->index;
        node = Extra(node->index);
    }
}

// @early-stop ~97%: twin of GetNewCellExtraOverlay (objIndex vs ovlIndex); same
// inline-accessor + /Ob1 reconstruction, same jmp-placement residual.
VA(0x0040b569, 0x1d3)
mapCellExtra *fullMap::GetNewCellExtraObject(int x, int y)
{
    mapCellExtra *node;  // -0x4
    int ix;              // -0x8
    int ni;              // -0xc
    mapCell *cell;       // -0x10

    if (Row(y)[x].extra == 0) {
        cell = &Row(y)[x];
        cell->extra = GetNewCellExtraIndex();
        return Extra(Row(y)[x].extra);
    }
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
        }
        ix = node->index;
        node = Extra(node->index);
    }
}

// Sets the object (overlay==0) or overlay (overlay!=0) tileset+index on a cell:
// either directly on the cell (when its current obj/ovl is empty or already on the
// requested tileset) or by walking/appending the cell's extras chain.
// /Od slots are MSVC symbol-hash order: retail frame is idx@-4, ptr@-8, t@-c, with
// one reserved unused local @-10 (this spills to -0x14). Names chosen so their
// buckets (0,1,4,6) sort into that order. The 7th param is unused in retail.
//
// NEEDS /G5: the four objTileset/ovlTileset bitfield READS lower in retail to
// `movb; shrb; andw $mask; andl $0xffff` — the unsigned-bitfield->int zero-extend
// idiom that MSVC 4.2 emits only under /G5 (Pentium). Under the build's /Gr-only
// flags they instead become `andb; xor; movb` (4 sites mismatch). Compiling this
// TU with /G5 makes them byte-exact AND leaves the 7 already-exact functions
// byte-identical (verified). Two residuals survive even with /G5: the /Od leading
// `jmp`-to-next at each chain-loop head (documented block-jmp wall,
// docs/patterns/od-cell-access-and-block-jmps.md) and the cell-reset objTileset
// write operand-order (t-first vs field-first; an /Od schedule driven by t's
// liveness across both branches).
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
                ptr = &extras[idx];
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
            cell->objTileset = t;
            cell->objIndex = index;
        }
    } else {
        if (cell->ovlIndex != 0xFF && cell->ovlTileset != tileset) {
            idx = cell->extra;
            while (idx != 0) {
                ptr = &extras[idx];
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
