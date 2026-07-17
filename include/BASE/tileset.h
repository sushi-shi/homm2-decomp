#ifndef HOMM2_BASE_TILESET_H
#define HOMM2_BASE_TILESET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <BASE/resource.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class tileset : public resource {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    u16 m_tileWidth;  // +0x10  tile width
    u16 m_tileHeight;  // +0x12  tile height
    u16 m_tileCount;  // +0x14  tile count
    char   *m_data;  // +0x16  tile pixel data
    // --- constructors ---
    tileset(u32l);
    virtual ~tileset();
};
#pragma pack(pop)
SIZE(tileset, 0x1a);
#endif // HOMM2_BASE_TILESET_H
