#ifndef HOMM2_SOURCE_SWAPMANAGER_H
#define HOMM2_SOURCE_SWAPMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 13 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
class hero;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class swapManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    int    field_0x36;  // +0x36
    int    field_0x3a;  // +0x3a
    int    field_0x3e;  // +0x3e
    int    field_0x42;  // +0x42
    int    field_0x46;  // +0x46
    int    field_0x4a;  // +0x4a
    int    field_0x4e;  // +0x4e
    int    field_0x52;  // +0x52
    int    field_0x56;  // +0x56
    // size >= 0x5a (accessed lower bound; no ctor/stride oracle)
    // --- constructors ---
    swapManager(void);
    swapManager(class hero *, class hero *);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Reset(void);
    int DrawSwapWin(void);
    void DrawSelector(void);
    void ViewMon(void);
    void SwapArtifacts(void);
    void SwapMons(void);
    void Update(void);
    void SplitMons(void);
};
#pragma pack(pop)
#endif // HOMM2_SOURCE_SWAPMANAGER_H
