#ifndef HOMM2_SOURCE_BANKBOX_H
#define HOMM2_SOURCE_BANKBOX_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class playerData;

#pragma pack(push, 1)  // recovered layout is byte-packed
class bankBox {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    int    field_0x0;  // +0x00
    short  field_0x4;  // +0x04
    short  field_0x6;  // +0x06
    int    field_0x8;  // +0x08
    // --- constructors ---
    bankBox(int, int, class playerData *);
    ~bankBox();
    // --- methods ---
    void Update(int);
};
#pragma pack(pop)
SIZE(bankBox, 0xc);
#endif // HOMM2_SOURCE_BANKBOX_H
