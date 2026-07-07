#ifndef HOMM2_BASE_RESOURCE_H
#define HOMM2_BASE_RESOURCE_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 1 own-virtual (pure dtor), 0 static data.
// Abstract base of the loadable-asset classes. ??_7resource@@6B@ is a single __purecall
// slot = a pure virtual destructor (??1resource@@UAE@XZ, 0x7 — pure-virtual-WITH-body).
// Asset subclasses (bitmap, icon, font, palette, tileset, sample) override slot 0 with
// their own ??_E deleting destructor. (Those subclass headers do not yet declare
// `: public resource` — the header-gen base heuristic only maps slot-0 Draw->widget /
// Open->baseManager, not a dtor slot -> resource; wire that up when reconstructing them.)
#include <va.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class resource /* abstract */ {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    short     field_0x4;  // +0x04
    short     field_0x6;  // +0x06
    int       field_0x8;  // +0x08
    resource* field_0xc;  // +0x0c
    // size >= 0x10 (accessed lower bound; no ctor/stride oracle)
    // --- constructors ---
    resource(void);
    resource(short int, unsigned long int, short int, class resource *);
    // --- virtual methods (vtable order) ---
    virtual ~resource(void) = 0;
};
#pragma pack(pop)
#endif // HOMM2_BASE_RESOURCE_H
