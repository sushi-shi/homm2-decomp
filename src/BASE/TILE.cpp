#include <va.h>
#include <BASE/TILE.h>
#include <BASE/bitmap.h>
#include <BASE/tileset.h>

DATA(0x0051f2ec) static u32 gTileMode;
DATA(0x0051f2f0) static i32 gTileRowCtr;

VA(0x004c2554, 0x18f)
extern "C" void __cdecl
TileToBitmap(tileset* src, u32 flags, bitmap* dst, i32 x, i32 y) {
    gTileMode = flags;

    const u32 tileWidth = src->m_tileWidth;
    const u32 tileHeight = src->m_tileHeight;
    const u32 tileIndex = flags & TILE_INDEX_MASK;
    const u8* source = reinterpret_cast<const u8*>(src->m_data)
        + tileWidth * tileHeight * tileIndex;

    const u32 destinationStride = dst->m_width;
    u8* destination = dst->m_pixels + y * destinationStride + x;

    const b32 flipHorizontal = (gTileMode & TILE_FLIP_HORIZONTAL) != 0;
    const b32 flipVertical = (gTileMode & TILE_FLIP_VERTICAL) != 0;
    for (gTileRowCtr = 0; gTileRowCtr < static_cast<i32>(tileHeight); ++gTileRowCtr) {
        const u32 row = static_cast<u32>(gTileRowCtr);
        const u32 sourceRow = flipVertical ? tileHeight - 1 - row : row;
        const u8* sourcePixels = source + sourceRow * tileWidth;
        u8* destinationPixels = destination + row * destinationStride;

        if (flipHorizontal) {
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
