#include <BASE/TILE.h>
#include <BASE/bitmap.h>
#include <BASE/tileset.h>
#include <algorithm>

extern "C" void __cdecl TileToBitmap(tileset* src, u32 flags, bitmap* dst, i32 x, i32 y) {
    if (src == nullptr || src->m_data == nullptr || dst == nullptr || dst->m_pixels == nullptr
        || dst->m_width <= 0 || dst->m_height <= 0 || src->m_tileWidth == 0 || src->m_tileHeight == 0)
        return;
    const u32 index = flags & TILE_INDEX_MASK;
    const u64 area = static_cast<u64>(src->m_tileWidth) * src->m_tileHeight;
    if (index >= src->m_tileCount || area > src->m_dataSize
        || area * index > src->m_dataSize - area)
        return;
    const i64 firstColumn = std::max<i64>(0, -static_cast<i64>(x));
    const i64 lastColumn = std::min<i64>(src->m_tileWidth, static_cast<i64>(dst->m_width) - x);
    const i64 firstRow = std::max<i64>(0, -static_cast<i64>(y));
    const i64 lastRow = std::min<i64>(src->m_tileHeight, static_cast<i64>(dst->m_height) - y);
    if (firstColumn >= lastColumn || firstRow >= lastRow)
        return;
    const auto* source = reinterpret_cast<const u8*>(src->m_data)
        + static_cast<std::size_t>(area * index);
    for (i64 row = firstRow; row < lastRow; ++row) {
        const i64 sourceRow = (flags & TILE_FLIP_VERTICAL) != 0 ? src->m_tileHeight - 1 - row : row;
        auto* target = dst->m_pixels + static_cast<std::size_t>(y + row) * dst->m_width;
        for (i64 column = firstColumn; column < lastColumn; ++column) {
            const i64 sourceColumn = (flags & TILE_FLIP_HORIZONTAL) != 0
                ? src->m_tileWidth - 1 - column : column;
            target[x + column] = source[sourceRow * src->m_tileWidth + sourceColumn];
        }
    }
}
