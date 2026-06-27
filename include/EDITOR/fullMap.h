#pragma once
// Reconstructed class (EDITOR) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 11 methods, 0 own-virtual, 0 static data.
#include <rva.h>
#include "mapcell.h"
// forward declarations:
class mapCell;
struct mapCellExtra;

class fullMap {
public:
    // --- members (offsets recovered from this+off accesses) ---
    mapCell      *cells;       // +0x00
    mapCellExtra *extras;      // +0x04
    int           width;       // +0x08
    int           height;      // +0x0c
    int           extraCount;  // +0x10

    // --- constructors ---
    fullMap(void);
    ~fullMap();
    // --- methods ---
    void Close(void);
    void Init(int, int);
    void ClearCellExtra(int);
    int GetNewCellExtraIndex(void);
    struct mapCellExtra * GetNewCellExtraOverlay(int, int);
    struct mapCellExtra * GetNewCellExtraObject(int, int);
    void Write(int);
    void Read(int, int);
    void ChangeTilesetIndex(class mapCell *, int, int, int, int, int, int);
};
SIZE(fullMap, 20);
