#pragma once
// Reconstructed class (EDITOR) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 11 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"
// forward declarations:
class mapCell;
struct mapCellExtra;

class fullMap {
public:
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
