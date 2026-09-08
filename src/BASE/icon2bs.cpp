#include <BASE/icon2bs.h>
#include <BASE/ImageDecode.h>
#include <BASE/bitmap.h>

#include <algorithm>

void IconToBitmapScale(icon* srcIcon, bitmap* dest, i32 destX, i32 destY, i32 frame,
                       IconDrawClipMode clip, i32 clipX, i32 clipY, i32 clipW, i32 clipH,
                       i32 scale) {
    if (dest == nullptr || dest->m_pixels == nullptr || dest->m_width <= 0
        || dest->m_height <= 0 || scale <= 0 || scale > 32)
        return;
    if (scale == 32) {
        images::DrawIcon(srcIcon, dest, destX, destY, frame, clip, clipX, clipY, clipW, clipH);
        return;
    }
    bitmap temporary(BITMAP_TYPE_NONE, 64, 64);
    std::fill_n(temporary.m_pixels, 64 * 64, 0);
    if (!images::DrawIcon(srcIcon, &temporary, 0, 0, frame, ICON_DRAW_CLIP, 0, 0, 32, 32))
        return;
    const i32 increment = 32 / scale;
    const i32 base = (32 - (scale - 1) * increment) / 2;
    for (i32 row = 0; row < scale; ++row) {
        const i64 y = static_cast<i64>(destY) + row;
        if (y < 0 || y >= dest->m_height
            || (clip != ICON_DRAW_NO_CLIP && (clipH <= 0 || y < clipY
                || y >= static_cast<i64>(clipY) + clipH)))
            continue;
        for (i32 column = 0; column < scale; ++column) {
            const i64 x = static_cast<i64>(destX) + column;
            if (x < 0 || x >= dest->m_width
                || (clip != ICON_DRAW_NO_CLIP && (clipW <= 0 || x < clipX
                    || x >= static_cast<i64>(clipX) + clipW)))
                continue;
            const u8 value = temporary.m_pixels[(base + row * increment) * 64
                                               + base + column * increment];
            if (value != 0)
                dest->m_pixels[static_cast<std::size_t>(y) * dest->m_width + x] = value;
        }
    }
}
