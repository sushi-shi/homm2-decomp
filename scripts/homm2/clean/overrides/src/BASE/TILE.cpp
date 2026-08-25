#include <BASE/TILE.h>
#include <BASE/bitmap.h>
#include <BASE/tileset.h>

struct TileBlitScratch {
    u32 mode;
    i32 row;
};

static TileBlitScratch gTileScratch;

extern "C" void __cdecl
TileToBitmap(tileset* src, u32 flags, bitmap* dst, i32 x, i32 y) {
    gTileScratch.mode = flags;

    const u32 tileWidth = src->m_tileWidth;
    const u32 tileHeight = src->m_tileHeight;
    const u32 tileIndex = flags & TILE_INDEX_MASK;
    const u8* source = reinterpret_cast<const u8*>(src->m_data)
        + tileWidth * tileHeight * tileIndex;

    const u32 destinationStride = dst->m_width;
    u8* destination = dst->m_pixels + y * destinationStride + x;

    const b32 flipHorizontal =
        (gTileScratch.mode & TILE_FLIP_HORIZONTAL) != 0;
    const b32 flipVertical =
        (gTileScratch.mode & TILE_FLIP_VERTICAL) != 0;
    for (gTileScratch.row = 0;
         gTileScratch.row < static_cast<i32>(tileHeight);
         ++gTileScratch.row) {
        const u32 row = static_cast<u32>(gTileScratch.row);
        const u32 sourceRow = flipVertical ? tileHeight - 1 - row : row;
        const u8* sourcePixels = source + sourceRow * tileWidth;
        u8* destinationPixels = destination + row * destinationStride;

        if (flipHorizontal) {
            for (u32 column = 0; column < tileWidth; ++column) {
                destinationPixels[column] =
                    sourcePixels[tileWidth - 1 - column];
            }
        } else {
            for (u32 column = 0; column < tileWidth; ++column) {
                destinationPixels[column] = sourcePixels[column];
            }
        }
    }
}
