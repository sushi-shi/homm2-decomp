// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\mapcell.obj   from: (directly linked into exe)
// functions: 11   data: 0
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
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

RVA(0x0040b070, 0x36)
fullMap::fullMap(void)
{
    cells = 0;
    extras = 0;
    extraCount = 0;
}

RVA(0x0040b0a6, 0x1e)
fullMap::~fullMap(void)
{
    Close();
}

RVA(0x0040b0c4, 0x81)
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

RVA(0x0040b145, 0x53)
void fullMap::Init(int w, int h)
{
    width = w;
    height = h;
    Close();
    cells = (mapCell *)operator new(width * height * sizeof(mapCell));
}

RVA(0x0040b198, 0xce)
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

RVA(0x0040b266, 0x130)
int fullMap::GetNewCellExtraIndex(void)
{
    int i;
    mapCellExtra *nb;
    int j;

    for (i = 1; i < extraCount; i++) {
        if (extras[i].index == 0xFFFF) {
            ClearCellExtra(i);
            return i;
        }
    }
    nb = (mapCellExtra *)operator new((extraCount + 100) * sizeof(mapCellExtra));
    memcpy(nb, extras, extraCount * sizeof(mapCellExtra));
    delete extras;
    extras = nb;
    for (j = extraCount; j < extraCount + 100; j++)
        extras[j].index = 0xFFFF;
    extraCount += 100;
    ClearCellExtra(extraCount - 100);
    return extraCount - 100;
}

RVA(0x0040b73c, 0x9e)
void fullMap::Write(int handle)
{
    _write(handle, &width, sizeof(width));
    _write(handle, &height, sizeof(height));
    _write(handle, cells, width * height * sizeof(mapCell));
    _write(handle, &extraCount, sizeof(extraCount));
    _write(handle, extras, extraCount * sizeof(mapCellExtra));
}

RVA(0x0040b7da, 0x295)
void fullMap::Read(int handle, int convert)
{
    int k;
    oldMapCell *tmp;
    int x, y;
    oldMapCellExtra *tmp2;

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
        for (k = 0; k < extraCount; k++)
            memcpy(extras + k, tmp2 + k, sizeof(mapCellExtra));
        delete tmp2;
    } else {
        _read(handle, extras, extraCount * sizeof(mapCellExtra));
    }
}

RVA(0x0040b396, 0x1d3)
// struct mapCellExtra * fullMap::GetNewCellExtraOverlay(int, int);

RVA(0x0040b569, 0x1d3)
// struct mapCellExtra * fullMap::GetNewCellExtraObject(int, int);

RVA(0x0040ba6f, 0x2ea)
// void fullMap::ChangeTilesetIndex(class mapCell *, int, int, int, int, int, int);
