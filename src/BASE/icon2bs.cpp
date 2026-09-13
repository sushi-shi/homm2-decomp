#include <va.h>
#include <BASE/icon2bs.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/Icon2b.h>
#include <string.h>
#include <SOURCE/KB.h>

H2_ENUM_BEGIN(IconScaleConstant)
    SCALE_NATIVE_SIZE = 0x20,
    SCALE_WORK_BITMAP_SIZE = 0x40
H2_ENUM_END(IconScaleConstant)

#if !H2_STRICT_ENUMS
#define destinationOrigin dstOrg
#define destinationPixel destPix
#define sourceBase srcBase
#define sourceOrigin srcOrg
#endif
VA(0x004cc4e0, 0x209)
void IconToBitmapScale(
    class icon* sourceIcon,
    class bitmap* destination,
    i32 destinationX,
    i32 destinationY,
    i32 frame,
    H2_ENUM_PARAM(IconDrawClipMode, i32) clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 scale
) {
    u8* sourceOrigin;
    u8* destinationOrigin;
    i32 lineStep;
    u8* source;
    u8* destinationPixel;
    i32 inc;
    i32 x;
    i32 y;
    class bitmap* temp;
    i32 sourceBase;

    if (scale == SCALE_NATIVE_SIZE) {
        IconToBitmap(sourceIcon, destination, destinationX, destinationY, frame, clip, clipX, clipY, clipW, clipH, 0);
        return;
    }
    inc = SCALE_NATIVE_SIZE / scale;
    sourceBase = (SCALE_NATIVE_SIZE - (scale - 1) * inc) >> 1;
    lineStep = inc * SCALE_WORK_BITMAP_SIZE;
    temp = new bitmap(BITMAP_TYPE_NONE, SCALE_WORK_BITMAP_SIZE, SCALE_WORK_BITMAP_SIZE);
    for (y = 0; y < SCALE_NATIVE_SIZE * SCALE_WORK_BITMAP_SIZE; y += SCALE_NATIVE_SIZE)
        memset(temp->m_pixels + y, 0, SCALE_NATIVE_SIZE);
    IconToBitmap(
        sourceIcon,
        temp,
        0,
        0,
        frame,
        ICON_DRAW_CLIP,
        0,
        0,
        SCALE_NATIVE_SIZE,
        SCALE_NATIVE_SIZE,
        0
    );
    destinationOrigin = destination->m_pixels + destinationX + destinationY * destination->m_width;
    sourceOrigin = temp->m_pixels + sourceBase + sourceBase * SCALE_WORK_BITMAP_SIZE;
    for (y = 0; y < scale; y++) {
        source = sourceOrigin;
        destinationPixel = destinationOrigin;
        for (x = 0; x < scale; x++) {
            if (*source != 0)
                *destinationPixel = *source;
            destinationPixel++;
            source += inc;
        }
        sourceOrigin += lineStep;
        destinationOrigin += destination->m_width;
    }
    delete temp;
}
#if !H2_STRICT_ENUMS
#undef destinationOrigin
#undef destinationPixel
#undef sourceBase
#undef sourceOrigin
#endif
