#include <BASE/TILE.h>
#include <BASE/bitmap.h>
#include <BASE/tileset.h>
#include <algorithm>

extern "C" void __cdecl TileToBitmap(tileset* source, u32 flags, bitmap* destination, i32 x, i32 y) {
    if (source == nullptr || source->m_data == nullptr || destination == nullptr || destination->m_pixels == nullptr
        || destination->m_width <= 0 || destination->m_height <= 0 || source->m_tileWidth == 0 || source->m_tileHeight == 0)
        return;
    const u32 index = flags & TILE_INDEX_MASK;
    const u64 area = static_cast<u64>(source->m_tileWidth) * source->m_tileHeight;
    if (index >= source->m_tileCount || area > source->m_dataSize
        || area * index > source->m_dataSize - area)
        return;
    const i64 firstColumn = std::max<i64>(0, -static_cast<i64>(x));
    const i64 lastColumn = std::min<i64>(source->m_tileWidth, static_cast<i64>(destination->m_width) - x);
    const i64 firstRow = std::max<i64>(0, -static_cast<i64>(y));
    const i64 lastRow = std::min<i64>(source->m_tileHeight, static_cast<i64>(destination->m_height) - y);
    if (firstColumn >= lastColumn || firstRow >= lastRow)
        return;
    const auto* tilePixels = reinterpret_cast<const u8*>(source->m_data)
        + static_cast<std::size_t>(area * index);
    for (i64 row = firstRow; row < lastRow; ++row) {
        const i64 sourceRow = (flags & TILE_FLIP_VERTICAL) != 0 ? source->m_tileHeight - 1 - row : row;
        auto* target = destination->m_pixels + static_cast<std::size_t>(y + row) * destination->m_width;
        for (i64 column = firstColumn; column < lastColumn; ++column) {
            const i64 sourceColumn = (flags & TILE_FLIP_HORIZONTAL) != 0
                ? source->m_tileWidth - 1 - column : column;
            target[x + column] = tilePixels[sourceRow * source->m_tileWidth + sourceColumn];
        }
    }
}
