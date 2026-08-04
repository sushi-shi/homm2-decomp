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

VA(0x004cc4e0, 0x209)
void IconToBitmapScale(
    class icon* srcIcon,
    class bitmap* dest,
    i32 destX,
    i32 destY,
    i32 frame,
    H2_ENUM_PARAM(IconDrawClipMode, i32) clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 scale
) {
    u8* srcOrg;
    u8* dstOrg;
    i32 lineStep;
    u8* source;
    u8* destPix;
    i32 inc;
    i32 x;
    i32 y;
    class bitmap* temp;
    i32 srcBase;

    if (scale == SCALE_NATIVE_SIZE) {
        IconToBitmap(srcIcon, dest, destX, destY, frame, clip, clipX, clipY, clipW, clipH, 0);
        return;
    }
    inc = SCALE_NATIVE_SIZE / scale;
    srcBase = (SCALE_NATIVE_SIZE - (scale - 1) * inc) >> 1;
    lineStep = inc * SCALE_WORK_BITMAP_SIZE;
    temp = new bitmap(BITMAP_TYPE_NONE, SCALE_WORK_BITMAP_SIZE, SCALE_WORK_BITMAP_SIZE);
    for (y = 0; y < SCALE_NATIVE_SIZE * SCALE_WORK_BITMAP_SIZE; y += SCALE_NATIVE_SIZE)
        memset(temp->m_pixels + y, 0, SCALE_NATIVE_SIZE);
    IconToBitmap(
        srcIcon,
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
    dstOrg = dest->m_pixels + destX + destY * dest->m_width;
    srcOrg = temp->m_pixels + srcBase + srcBase * SCALE_WORK_BITMAP_SIZE;
    for (y = 0; y < scale; y++) {
        source = srcOrg;
        destPix = dstOrg;
        for (x = 0; x < scale; x++) {
            if (*source != 0)
                *destPix = *source;
            destPix++;
            source += inc;
        }
        srcOrg += lineStep;
        dstOrg += dest->m_width;
    }
    delete temp;
}
