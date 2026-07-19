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
    bitmap(BitmapType, i16, i16);
    bitmap(u32l);
    __declspec(dllexport) virtual inline ~bitmap();
    void DrawToBufferCareful(i16, i16);
    __declspec(dllexport) inline void DrawToBuffer(i16, i16);
    void DrawToScreen(i16, i16);
    void GrabScreen(i16, i16);
    __declspec(dllexport) inline void GrabBitmap(class bitmap*, i16, i16);
    void GrabBitmapCareful(class bitmap*, i16, i16);
    void CopyTo(class bitmap*, i32, i32, i32, i32, i32, i32);
    void CopyToCareful(class bitmap*, i32, i32, i32, i32, i32, i32);
};
#pragma pack(pop)
SIZE(bitmap, 0x1a);
#endif
