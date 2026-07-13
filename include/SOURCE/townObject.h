#ifndef HOMM2_SOURCE_TOWNOBJECT_H
#define HOMM2_SOURCE_TOWNOBJECT_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 0 own-virtual, 0 static data.
#include <va.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class townObject {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    int    field_0x0;  // +0x00
    int    field_0x4;  // +0x04
    int m_visible;  // +0x08
    int m_buildingId;  // +0x0c
    int    field_0x10;  // +0x10
    int    field_0x14;  // +0x14
    // --- constructors ---
    townObject(int, int, char *);
    ~townObject();
    // --- methods ---
    void Draw(int);
};
#pragma pack(pop)
SIZE(townObject, 0x18);
#endif // HOMM2_SOURCE_TOWNOBJECT_H
