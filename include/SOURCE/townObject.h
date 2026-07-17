#ifndef HOMM2_SOURCE_TOWNOBJECT_H
#define HOMM2_SOURCE_TOWNOBJECT_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 0 own-virtual, 0 static data.
#include <va.h>

class border;
class icon;

#pragma pack(push, 1)  // recovered layout is byte-packed
class townObject {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    i32 m_animationFrameCount;  // +0x00
    i32 m_animationFrame;  // +0x04
    i32 m_visible;  // +0x08
    i32 m_buildingId;  // +0x0c
    icon *m_icon;  // +0x10
    border *m_border;  // +0x14
    // --- constructors ---
    townObject(i32, i32, char *);
    ~townObject();
    // --- methods ---
    void Draw(i32);
};
#pragma pack(pop)
SIZE(townObject, 0x18);
#endif // HOMM2_SOURCE_TOWNOBJECT_H
