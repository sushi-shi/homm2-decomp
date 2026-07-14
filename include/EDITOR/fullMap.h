#ifndef HOMM2_EDITOR_FULLMAP_H
#define HOMM2_EDITOR_FULLMAP_H
// Reconstructed class (EDITOR) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 11 methods, 0 own-virtual, 0 static data.
#include <va.h>
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

    // Inline accessors. The retail build is /Od /Ob1 (unoptimized but inline
    // expansion ON), so these splice into each call site - reproducing the per-call
    // `jmp $+0` and the deferred `Row(y)[x]` indexing seen in the retail .text. Fully
    // inlined -> they emit no out-of-line symbol, which is why CodeView lists none.
    mapCell      *Cells(void)  { return cells; }
    mapCell      *Row(int y)   { return &cells[width * y]; }   // row base ptr; caller does [x]
    // Retail closes this inline region after assigning the caller's pointer. The
    // output reference is therefore part of the recovered source shape.
    void Cell(mapCell *&cell, int x, int y) {
        cell = reinterpret_cast<mapCell *>(reinterpret_cast<unsigned char *>(cells) +
                                           width * y * sizeof(mapCell)) + x;
    }
    mapCell      *GetCell(int x, int y) { return cells + width * y + x; }
    mapCellExtra *Extra(int i) { return &extras[i]; }          // &extras[i] (stride 7)
};
SIZE(fullMap, 20);
#endif // HOMM2_EDITOR_FULLMAP_H
