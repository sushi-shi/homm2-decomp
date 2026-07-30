#ifndef HOMM2_BASE_BITMAP_H
#define HOMM2_BASE_BITMAP_H

#include <Ints.h>
#include <BASE/resource.h>


enum class BitmapType : i16 {
    BITMAP_TYPE_NONE           = 0,
    BITMAP_TYPE_MEMORY         = 0x21,
    COMBAT_MISSILE_BITMAP_TYPE = BITMAP_TYPE_MEMORY
};
using enum BitmapType;

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
    virtual inline ~bitmap();
    void DrawToBufferCareful(i16 x, i16 y);
    inline void DrawToBuffer(i16 x, i16 y);
    void DrawToScreen(i16 x, i16 y);
    void GrabScreen(i16 x, i16 y);
    inline void GrabBitmap(class bitmap* source, i16 x, i16 y);
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
#endif
