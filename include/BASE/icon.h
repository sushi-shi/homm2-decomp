#ifndef HOMM2_BASE_ICON_H
#define HOMM2_BASE_ICON_H

#include <Ints.h>
#include <BASE/IconDraw.h>
#include <BASE/resource.h>

struct SLimitData {
    i32 left;
    i32 right;
    i32 top;
    i32 bottom;
};

#pragma pack(push, 1)
class icon : public resource {
public:
    i16 m_frameCount;
    u8* m_data;
    icon(u32l id);
    virtual inline ~icon();
    void DrawToBuffer(
        i32 x, i32 y, i32 frame, IconDrawOrientation orientation
    );
    IconDrawResult CombatClipDrawToBuffer(
        i32 x,
        i32 y,
        i32 frame,
        struct SLimitData* limits,
        IconDrawOrientation orientation,
        i32 offset,
        u8* colorTable,
        i8* yModify
    );
    void ClipFillToBuffer(
        i32 x,
        i32 y,
        i32 frame,
        i32 color,
        IconDrawOrientation orientation,
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
        IconDrawOrientation orientation,
        struct SLimitData* limits
    );
    void DimToBuffer(
        i32 x, i32 y, i32 frame, IconDrawOrientation orientation
    );
};
#pragma pack(pop)
#endif
