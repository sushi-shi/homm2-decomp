#ifndef HOMM2_SOURCE_RECRUITUNIT_H
#define HOMM2_SOURCE_RECRUITUNIT_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 6 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
class armyGroup;
class town;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class recruitUnit : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    int    field_0x36;  // +0x36
    int    field_0x3a;  // +0x3a
    char _pad_0x3e[0x4];
    int    field_0x42;  // +0x42
    int    field_0x46;  // +0x46
    int    field_0x4a;  // +0x4a
    int    field_0x4e;  // +0x4e
    int    field_0x52;  // +0x52
    char _pad_0x56[0x4];
    int    field_0x5a;  // +0x5a
    char _pad_0x5e[0x4];
    int    field_0x62;  // +0x62
    int    field_0x66;  // +0x66
    int    field_0x6a;  // +0x6a
    int    field_0x6e;  // +0x6e
    int    field_0x72;  // +0x72
    int    field_0x76;  // +0x76
    int    field_0x7a;  // +0x7a
    // --- constructors ---
    recruitUnit(class armyGroup *, int, short int *);
    recruitUnit(class town *, int, int);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Update(void);
};
#pragma pack(pop)
SIZE(recruitUnit, 0x7e);
#endif // HOMM2_SOURCE_RECRUITUNIT_H
