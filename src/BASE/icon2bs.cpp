#include <va.h>
#include <BASE/icon2bs.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/Icon2b.h>
#include <string.h>

H2_ENUM_BEGIN(IconScaleConstant)
    SCALE_NATIVE_SIZE      = 0x20,
    SCALE_WORK_BITMAP_SIZE = 0x40
H2_ENUM_END(IconScaleConstant)

// @semantic: first raw divergence is +0x0: candidate reserves 0x10 frame bytes while retail reserves 0x18.
VA(0x004d2f90, 0x179)
void IconToBitmapScale(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    i32 clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 scale
) {
    if (scale == SCALE_NATIVE_SIZE) {
        IconToBitmap(srcIcon, dest, x, y, frame, clip, clipX, clipY, clipW, clipH, 0);
        return;
    }
    i32 step = SCALE_NATIVE_SIZE / scale;
    i32 srcBase = ((1 - scale) * step + SCALE_NATIVE_SIZE) >> 1;
    i32 srcAdv = step * SCALE_WORK_BITMAP_SIZE;
    bitmap* tmp =
        new bitmap(BITMAP_TYPE_NONE, SCALE_WORK_BITMAP_SIZE, SCALE_WORK_BITMAP_SIZE);
    i32 rowOff = 0;
    u8* tmpPixels = tmp->m_pixels;
    do {
        memset(tmpPixels + rowOff, 0, SCALE_NATIVE_SIZE);
        rowOff = rowOff + SCALE_NATIVE_SIZE;
    } while (rowOff < SCALE_NATIVE_SIZE * SCALE_WORK_BITMAP_SIZE);
    IconToBitmap(
        srcIcon,
        tmp,
        0,
        0,
        frame,
        1,
        0,
        0,
        SCALE_NATIVE_SIZE,
        SCALE_NATIVE_SIZE,
        0
    );
    i32 pitch = dest->m_width;
    u8* dstRow = dest->m_pixels + y * pitch + x;
    u8* srcRow = tmp->m_pixels + srcBase * SCALE_WORK_BITMAP_SIZE + srcBase;
    if (0 < scale) {
        i32 rows = scale;
        do {
            i32 cols = scale;
            u8* dstPix = dstRow;
            u8* srcPix = srcRow;
            if (0 < scale) {
                do {
                    if (*srcPix != 0)
                        *dstPix = *srcPix;
                    dstPix = dstPix + 1;
                    srcPix = srcPix + step;
                    cols = cols - 1;
                } while (cols != 0);
            }
            srcRow = srcRow + srcAdv;
            dstRow = dstRow + pitch;
            rows = rows - 1;
        } while (rows != 0);
    }
    delete tmp;
}
