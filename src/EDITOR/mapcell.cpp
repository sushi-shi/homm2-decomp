// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\mapcell.obj   from: (directly linked into exe)
// functions: 11   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include "EDITOR/fullMap.h"
#include <string.h>
#include <io.h>

// Legacy on-disk record sizes (Read's convert path streams the old layout and
// copies the leading bytes into the new packed records).
struct oldMapCell {
    u8 raw[20];
};
struct oldMapCellExtra {
    u8 raw[15];
};

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

// PARTIAL (~86%): logic + the (cells+width*y)[x] cell-access form match retail;
// residual is /Od block-boundary jmp-to-next artifacts (incl. a leading jmp) plus
// the 4-local hash-slot order (cur/idx/ni/cp). See docs/patterns/od-hash-slots.md.
VA(0x0040b396, 0x1d3)
mapCellExtra *fullMap::GetNewCellExtraOverlay(int x, int y)
{
    mapCellExtra *cur;   // -0x4
    int idx;             // -0x8
    int ni;              // -0xc
    mapCell *cp;         // -0x10

    if ((cells + width * y)[x].extra == 0) {
        cp = (cells + width * y) + x;
        cp->extra = GetNewCellExtraIndex();
        return &extras[(cells + width * y)[x].extra];
    }
    idx = (cells + width * y)[x].extra;
    cur = &extras[(cells + width * y)[x].extra];
    for (;;) {
        if (cur->ovlIndex == 0xFF)
            return cur;
        if (cur->index == 0) {
            ni = GetNewCellExtraIndex();
            cur = &extras[idx];
            cur->index = ni;
            return &extras[cur->index];
        }
        idx = cur->index;
        cur = &extras[cur->index];
    }
}

// PARTIAL (~86%): twin of GetNewCellExtraOverlay (objIndex vs ovlIndex); same
// /Od block-jmp + hash-slot residual.
VA(0x0040b569, 0x1d3)
mapCellExtra *fullMap::GetNewCellExtraObject(int x, int y)
{
    mapCellExtra *cur;   // -0x4
    int idx;             // -0x8
    int ni;              // -0xc
    mapCell *cp;         // -0x10

    if ((cells + width * y)[x].extra == 0) {
        cp = (cells + width * y) + x;
        cp->extra = GetNewCellExtraIndex();
        return &extras[(cells + width * y)[x].extra];
    }
    idx = (cells + width * y)[x].extra;
    cur = &extras[(cells + width * y)[x].extra];
    for (;;) {
        if (cur->objIndex == 0xFF)
            return cur;
        if (cur->index == 0) {
            ni = GetNewCellExtraIndex();
            cur = &extras[idx];
            cur->index = ni;
            return &extras[cur->index];
        }
        idx = cur->index;
        cur = &extras[cur->index];
    }
}

// VA(0x0040ba6f, 0x2ea)
// void fullMap::ChangeTilesetIndex(class mapCell *, int, int, int, int, int, int);
