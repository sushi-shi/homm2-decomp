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
    i32           width;       // +0x08
    i32           height;      // +0x0c
    i32           extraCount;  // +0x10

    // --- constructors ---
    fullMap(void);
    ~fullMap();
    // --- methods ---
    void Close(void);
    void Init(i32, i32);
    void ClearCellExtra(i32);
    i32 GetNewCellExtraIndex(void);
    struct mapCellExtra * GetNewCellExtraOverlay(i32, i32);
    struct mapCellExtra * GetNewCellExtraObject(i32, i32);
    void Write(i32);
    void Read(i32, i32);
    void ChangeTilesetIndex(class mapCell *, i32, i32, i32, i32, i32, i32);

    // Inline accessors. The retail build is /Od /Ob1 (unoptimized but inline
    // expansion ON), so these splice into each call site - reproducing the per-call
    // `jmp $+0` and the deferred `Row(y)[x]` indexing seen in the retail .text. Fully
    // inlined -> they emit no out-of-line symbol, which is why CodeView lists none.
    mapCell      *Cells(void)  { return cells; }
    mapCell      *Row(i32 y)   { return &cells[width * y]; }   // row base ptr; caller does [x]
    // Retail closes this inline region after assigning the caller's pointer. The
    // output reference is therefore part of the recovered source shape. (Retail's
    // inline lowering used byte-offset addressing here; natural indexing is kept
    // by policy — correctness over byte identity.)
    void Cell(mapCell *&cell, i32 x, i32 y) { cell = &cells[width * y + x]; }
    mapCell      *GetCell(i32 x, i32 y) { return cells + width * y + x; }
    mapCellExtra *Extra(i32 i) { return &extras[i]; }          // &extras[i] (stride 7)
};
SIZE(fullMap, 20);
#endif // HOMM2_EDITOR_FULLMAP_H
