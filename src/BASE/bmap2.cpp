#include <BASE/bmap2.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>

#include <algorithm>
#include <cstring>
#include <vector>

namespace {
struct Rectangle {
    i64 left, top, right, bottom;
    bool Empty() const { return left >= right || top >= bottom; }
};
bool Valid(const bitmap* image) {
    return image != nullptr && image->m_pixels != nullptr && image->m_width > 0 && image->m_height > 0;
}
Rectangle Area(const bitmap* image, i32 x, i32 y, i32 width, i32 height) {
    if (!Valid(image) || width <= 0 || height <= 0)
        return {};
    return {std::max<i64>(0, x), std::max<i64>(0, y),
            std::min<i64>(image->m_width, static_cast<i64>(x) + width),
            std::min<i64>(image->m_height, static_cast<i64>(y) + height)};
}
void Fill(bitmap* image, const Rectangle& rectangle, i32 color) {
    if (rectangle.Empty())
        return;
    for (i64 row = rectangle.top; row < rectangle.bottom; ++row)
        std::memset(image->m_pixels + row * image->m_width + rectangle.left, color,
                    static_cast<std::size_t>(rectangle.right - rectangle.left));
}
}

void FillBitmapArea(bitmap* image, i32 x, i32 y, i32 width, i32 height, i32 color) {
    Fill(image, Area(image, x, y, width, height), color);
}

void FillBitmapAreaClip(bitmap* image, i32 x, i32 y, i32 width, i32 height, i32 color,
                        i32 clipX, i32 clipY, i32 clipWidth, i32 clipHeight) {
    if (clipWidth <= 0 || clipHeight <= 0)
        return;
    const i64 clipRight = static_cast<i64>(clipX) + clipWidth;
    const i64 clipBottom = static_cast<i64>(clipY) + clipHeight;
    // Retain retail's exclusion of rectangles that only touch its final/first
    // clip column or row; widen the old guard before doing any arithmetic.
    if (x >= clipRight - 1 || static_cast<i64>(x) + width - 1 <= clipX
        || y >= clipBottom - 1 || static_cast<i64>(y) + height - 1 <= clipY)
        return;
    auto rectangle = Area(image, x, y, width, height);
    rectangle.left = std::max<i64>(rectangle.left, clipX);
    rectangle.top = std::max<i64>(rectangle.top, clipY);
    rectangle.right = std::min(rectangle.right, clipRight);
    rectangle.bottom = std::min(rectangle.bottom, clipBottom);
    Fill(image, rectangle, color);
}

void BlitBitmap(bitmap* src, i32 sx, i32 sy, i32 width, i32 height, bitmap* dst, i32 dx, i32 dy) {
    if (!Valid(src) || !Valid(dst) || width <= 0 || height <= 0)
        return;
    const i64 firstColumn = std::max({i64{0}, -static_cast<i64>(sx), -static_cast<i64>(dx)});
    const i64 firstRow = std::max({i64{0}, -static_cast<i64>(sy), -static_cast<i64>(dy)});
    const i64 lastColumn = std::min({static_cast<i64>(width), static_cast<i64>(src->m_width) - sx,
                                    static_cast<i64>(dst->m_width) - dx});
    const i64 lastRow = std::min({static_cast<i64>(height), static_cast<i64>(src->m_height) - sy,
                                 static_cast<i64>(dst->m_height) - dy});
    if (firstColumn >= lastColumn || firstRow >= lastRow)
        return;
    const auto bytes = static_cast<std::size_t>(lastColumn - firstColumn);
    const bool aliases = src->m_pixels == dst->m_pixels;
    if (aliases && src->m_width != dst->m_width) {
        std::vector<u8> copy(bytes * static_cast<std::size_t>(lastRow - firstRow));
        for (i64 row = firstRow; row < lastRow; ++row)
            std::memcpy(copy.data() + static_cast<std::size_t>(row - firstRow) * bytes,
                        src->m_pixels + (sy + row) * src->m_width + sx + firstColumn, bytes);
        for (i64 row = firstRow; row < lastRow; ++row)
            std::memcpy(dst->m_pixels + (dy + row) * dst->m_width + dx + firstColumn,
                        copy.data() + static_cast<std::size_t>(row - firstRow) * bytes, bytes);
        return;
    }
    const bool backwards = aliases && dy > sy;
    for (i64 step = 0; step < lastRow - firstRow; ++step) {
        const i64 row = backwards ? lastRow - 1 - step : firstRow + step;
        std::memmove(dst->m_pixels + (dy + row) * dst->m_width + dx + firstColumn,
                     src->m_pixels + (sy + row) * src->m_width + sx + firstColumn, bytes);
    }
}

void DimBitmapArea(bitmap* image, i32 x, i32 y, i32 width, i32 height, i32 level) {
    if (level < 0 || level >= DIM_PALETTE_LIGHTEN_TABLE)
        return;
    const auto rectangle = Area(image, x, y, width, height);
    if (rectangle.Empty())
        return;
    const u8* palette = uDimPal[level / DIM_PALETTE_LEVEL_COUNT][level % DIM_PALETTE_LEVEL_COUNT];
    for (i64 row = rectangle.top; row < rectangle.bottom; ++row) {
        auto* pixels = image->m_pixels + row * image->m_width;
        for (i64 column = rectangle.left; column < rectangle.right; ++column)
            pixels[column] = palette[pixels[column]];
    }
}
