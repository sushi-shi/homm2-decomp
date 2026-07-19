#ifndef HOMM2_BASE_BITMAP_H
#define HOMM2_BASE_BITMAP_H

#include <va.h>
#include <BASE/resource.h>

// Proven bitmap storage tags. The packed class member remains a short because
// bitmap resources serialize this value as a 16-bit word.
H2_ENUM_CLASS_BEGIN_T(BitmapType, i16)
    BITMAP_TYPE_NONE           = 0,
    BITMAP_TYPE_MEMORY         = 0x21,
    COMBAT_MISSILE_BITMAP_TYPE = BITMAP_TYPE_MEMORY
H2_ENUM_CLASS_END_T(BitmapType, i16)

#pragma pack(push, 1)
class bitmap : public resource {
public:
    BitmapType m_bitmapType;
    i16 m_width;
    i16 m_height;
    u8* m_pixels;
    bitmap(void);
    bitmap(BitmapType type, i16 width, i16 height);
    bitmap(u32l id);
    __declspec(dllexport) virtual inline ~bitmap();
    void DrawToBufferCareful(i16 x, i16 y);
    __declspec(dllexport) inline void DrawToBuffer(i16 x, i16 y);
    void DrawToScreen(i16 x, i16 y);
    void GrabScreen(i16 x, i16 y);
    __declspec(dllexport) inline void GrabBitmap(class bitmap* source, i16 x, i16 y);
    void GrabBitmapCareful(class bitmap* source, i16 x, i16 y);
    void CopyTo(
        class bitmap* destination,
        i32 destinationX,
        i32 destinationY,
        i32 sourceX,
        i32 sourceY,
        i32 width,
        i32 height
    );
    void CopyToCareful(
        class bitmap* destination,
        i32 destinationX,
        i32 destinationY,
        i32 sourceX,
        i32 sourceY,
        i32 width,
        i32 height
    );
};
#pragma pack(pop)
SIZE(bitmap, 0x1a);
#endif
