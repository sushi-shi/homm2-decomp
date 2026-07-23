#ifndef HOMM2_BASE_ICON_H
#define HOMM2_BASE_ICON_H

#include <va.h>
#include <BASE/IconDraw.h>
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
    inline struct IconEntry& Entries(i32 frame);
    icon(u32l id);
    __declspec(dllexport) virtual inline ~icon();
    void DrawToBuffer(
        i32 x, i32 y, i32 frame, H2_ENUM_PARAM(IconDrawOrientation, i32) orientation
    );
    H2_ENUM_RETURN(IconDrawResult, i32) CombatClipDrawToBuffer(
        i32 x,
        i32 y,
        i32 frame,
        struct SLimitData* limits,
        H2_ENUM_PARAM(IconDrawOrientation, i32) orientation,
        i32 offset,
        u8* colorTable,
        i8* yModify
    );
    void ClipFillToBuffer(
        i32 x,
        i32 y,
        i32 frame,
        i32 color,
        H2_ENUM_PARAM(IconDrawOrientation, i32) orientation,
        i32 clipX,
        i32 clipY,
        i32 clipW,
        i32 clipH
    );
    void FillToBuffer(
        i32 x,
        i32 y,
        i32 frame,
        i32 color,
        H2_ENUM_PARAM(IconDrawOrientation, i32) orientation,
        struct SLimitData* limits
    );
    void DimToBuffer(
        i32 x, i32 y, i32 frame, H2_ENUM_PARAM(IconDrawOrientation, i32) orientation
    );
};
#pragma pack(pop)
SIZE(icon, 0x16);
#endif
