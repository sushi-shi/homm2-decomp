#include <Ints.h>

#include <BASE/TILE.h>
#include <BASE/bitmap.h>
#include <BASE/tileset.h>

namespace {

enum class SourceOrder {
    Forward,
    RowsReversed,
    PixelsReversed,
    Both,
};

void BlitTile(
    const u8* source,
    u32 tileWidth,
    u32 tileHeight,
    u8* destination,
    u32 destinationStride,
    SourceOrder order
) {
    for (u32 row = 0; row < tileHeight; ++row) {
        const u32 sourceRow =
            order == SourceOrder::RowsReversed || order == SourceOrder::Both
                ? tileHeight - 1 - row
                : row;
        const u8* sourcePixels =
            source + static_cast<std::size_t>(sourceRow) * tileWidth;
        u8* destinationPixels =
            destination + static_cast<std::size_t>(row) * destinationStride;

        if (order == SourceOrder::PixelsReversed || order == SourceOrder::Both) {
            for (u32 column = 0; column < tileWidth; ++column) {
                destinationPixels[column] = sourcePixels[tileWidth - 1 - column];
            }
        } else {
            for (u32 column = 0; column < tileWidth; ++column) {
                destinationPixels[column] = sourcePixels[column];
            }
        }
    }
}

}

extern "C" void TileToBitmap(
    tileset* src,
    u32 flags,
    bitmap* dst,
    i32 x,
    i32 y
) {
    if (src == nullptr || dst == nullptr || src->m_data == nullptr
        || dst->m_pixels == nullptr) {
        return;
    }

    const u32 tileWidth = src->m_tileWidth;
    const u32 tileHeight = src->m_tileHeight;
    if (tileWidth == 0 || tileHeight == 0) {
        return;
    }

    const u32 tileIndex = flags & TILE_INDEX_MASK;
    const u8* source =
        reinterpret_cast<const u8*>(src->m_data)
        + static_cast<std::size_t>(tileWidth) * tileHeight * tileIndex;

    const u32 destinationStride = static_cast<u32>(dst->m_width);
    u8* destination =
        dst->m_pixels + static_cast<std::size_t>(y) * destinationStride + x;

    const bool flipHorizontal = (flags & TILE_FLIP_HORIZONTAL) != 0;
    const bool flipVertical = (flags & TILE_FLIP_VERTICAL) != 0;
    const SourceOrder order =
        flipHorizontal
            ? (flipVertical ? SourceOrder::Both : SourceOrder::PixelsReversed)
            : (flipVertical ? SourceOrder::RowsReversed : SourceOrder::Forward);

    BlitTile(
        source,
        tileWidth,
        tileHeight,
        destination,
        destinationStride,
        order
    );
}
