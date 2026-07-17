#ifndef HOMM2_BASE_PALETTE_H
#define HOMM2_BASE_PALETTE_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 5 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <BASE/resource.h>

H2_ENUM_BEGIN(PaletteConstant)
    PALETTE_DATA_SIZE = 0x300
H2_ENUM_END(PaletteConstant)

#pragma pack(push, 1) // recovered layout is byte-packed
class palette : public resource {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    i8* m_data; // +0x10  palette data (256*3 RGB)
    // --- constructors ---
    palette(void);
    palette(u32l);
    virtual ~palette();
    // --- methods ---
    i8* Data(void);
};
#pragma pack(pop)
SIZE(palette, 0x14);
#endif // HOMM2_BASE_PALETTE_H
