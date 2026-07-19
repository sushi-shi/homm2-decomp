#ifndef HOMM2_BASE_ICON_H
#define HOMM2_BASE_ICON_H

#include <va.h>
#include <BASE/resource.h>

struct SLimitData {
    i32 left;
    i32 right;
    i32 top;
    i32 bottom;
};
SIZE(SLimitData, 0x10);

#pragma pack(push, 1)
class icon : public resource {
public:
    i16 m_frameCount;
    u8* m_data;
    // m_data heads with the serialized IconEntry frame table (m_frameCount rows);
    // pixel runs follow at each entry's srcOffset. The typed view of the table
    // lives here so call sites do not re-cast the blob.
    struct IconEntry* Entries(void) {
        return reinterpret_cast<struct IconEntry*>(m_data);
    }
    icon(u32l);
    virtual ~icon();
    void DrawToBuffer(i32, i32, i32, i32);
    i32 CombatClipDrawToBuffer(i32, i32, i32, struct SLimitData*, i32, i32, u8*, i8*);
    void ClipFillToBuffer(i32, i32, i32, i32, i32, i32, i32, i32, i32);
    void FillToBuffer(i32, i32, i32, i32, i32, struct SLimitData*);
    void DimToBuffer(i32, i32, i32, i32);
};
#pragma pack(pop)
SIZE(icon, 0x16);
#endif
