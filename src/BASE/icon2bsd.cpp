#include <Ints.h>
#include <BASE/icon2bsd.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/Icon2b.h>
#include <string.h>
#include <SOURCE/KB.h>


typedef enum IconScaleDoubleConstant {
    SCALE_DOUBLE_NATIVE_SIZE = 0x20,
    SCALE_DOUBLE_FRAME_SIZE = 0x40,
    SCALE_DOUBLE_WORK_BYTES = SCALE_DOUBLE_FRAME_SIZE * SCALE_DOUBLE_FRAME_SIZE,
    SCALE_DOUBLE_CELLS = SCALE_DOUBLE_FRAME_SIZE / SCALE_DOUBLE_NATIVE_SIZE
} IconScaleDoubleConstant;

void IconToBitmapScaleDouble(
    class icon* sourceIcon,
    class bitmap* destination,
    i32 destinationX,
    i32 destinationY,
    i32 frame,
    IconDrawClipMode clip,
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
    i32 steps;
    i32 increment;
    i32 x;
    i32 y;
    class bitmap* temp;
    i32 sourceBase;

    if (scale == SCALE_DOUBLE_NATIVE_SIZE) {
        IconToBitmap(sourceIcon, destination, destinationX, destinationY, frame, clip, clipX, clipY, clipW, clipH, 0);
        return;
    }
    steps = scale * SCALE_DOUBLE_CELLS;
    increment = SCALE_DOUBLE_NATIVE_SIZE / scale;
    sourceBase = (SCALE_DOUBLE_NATIVE_SIZE - (scale - 1) * increment) >> 1;
    lineStep = increment * SCALE_DOUBLE_FRAME_SIZE;
    temp = new bitmap(BITMAP_TYPE_NONE, SCALE_DOUBLE_FRAME_SIZE, SCALE_DOUBLE_FRAME_SIZE);
    memset(temp->m_pixels, 0, SCALE_DOUBLE_WORK_BYTES);
    IconToBitmap(
        sourceIcon,
        temp,
        0,
        0,
        frame,
        ICON_DRAW_CLIP,
        0,
        0,
        SCALE_DOUBLE_FRAME_SIZE,
        SCALE_DOUBLE_FRAME_SIZE,
        0
    );
    destinationOrigin = destination->m_pixels + destinationX + destinationY * destination->m_width;
    sourceOrigin = temp->m_pixels + sourceBase + sourceBase * SCALE_DOUBLE_FRAME_SIZE;
    for (y = 0; y < steps; y++) {
        source = sourceOrigin;
        destinationPixel = destinationOrigin;
        for (x = 0; x < steps; x++) {
            if (*source != 0)
                *destinationPixel = *source;
            destinationPixel++;
            source += increment;
        }
        sourceOrigin += lineStep;
        destinationOrigin += destination->m_width;
    }
    delete temp;
}
