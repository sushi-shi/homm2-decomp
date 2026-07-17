#ifndef HOMM2_BASE_ICON_H
#define HOMM2_BASE_ICON_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 8 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <BASE/resource.h>
struct SLimitData {
    i32 left;
    i32 right;
    i32 top;
    i32 bottom;
};
SIZE(SLimitData, 0x10);

#pragma pack(push, 1)  // recovered layout is byte-packed
// icon is an asset subclass of resource (slot-0 dtor override).
class icon : public resource {
public:
    // --- own members (base resource occupies 0x00..0x10; widths from access-analysis) ---
    i16  m_frameCount;  // +0x10
    char  *m_data;  // +0x12  (glyph/sprite data; indexed as a byte pointer)
    // m_data heads with the serialized IconEntry frame table (m_frameCount rows);
    // pixel runs follow at each entry's srcOffset. The typed view of the table
    // lives here so call sites do not re-cast the blob.
    struct IconEntry *Entries(void) {
        return reinterpret_cast<struct IconEntry *>(m_data);
    }
    // --- constructors ---
    icon(u32l);
    virtual ~icon();
    // --- methods ---
    void DrawToBuffer(i32, i32, i32, i32);
    i32 CombatClipDrawToBuffer(i32, i32, i32, struct SLimitData *, i32, i32, u8 *, i8 *);
    void ClipFillToBuffer(i32, i32, i32, i32, i32, i32, i32, i32, i32);
    void FillToBuffer(i32, i32, i32, i32, i32, struct SLimitData *);
    void DimToBuffer(i32, i32, i32, i32);
};
#pragma pack(pop)
SIZE(icon, 0x16);
#endif // HOMM2_BASE_ICON_H
