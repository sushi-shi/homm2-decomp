#include <H2/Ints.h>
#include <BASE/tile2bs.h>
#include <BASE/TILE.h>
#include <BASE/tileset.h>
#include <BASE/bitmap.h>
#include <SOURCE/KB.h>


typedef enum TileScaleConstant {
    TILE_SCALE_NATIVE = 1,
    TILE_SCALE_INDEX_MASK = 0x3fff
} TileScaleConstant;

static u8* s_dest;
static char* s_nextSourceRow;
static i32 s_tileWidth;
static i32 s_y;
static i32 s_x;
static i32 s_tileHeight;
static i32 s_sourceRowStep;
static u8* s_destRow;
static char* s_source;
static i32 s_destPitch;

void TileToBitmapScale(tileset* source, u32 flags, bitmap* destination, i32 x, i32 y, i32 scale) {
    if (scale == TILE_SCALE_NATIVE) {
        TileToBitmap(source, flags, destination, x, y);
        return;
    }
    s_tileHeight = source->m_tileHeight;
    s_tileWidth = source->m_tileWidth;
    s_sourceRowStep = s_tileWidth * scale;
    s_destPitch = destination->m_width;
    if ((flags & TILE_FLIP_HORIZONTAL) == 0) {
        if ((flags & TILE_FLIP_VERTICAL) == 0) {
            s_source = source->m_data + s_tileWidth * s_tileHeight * (flags & TILE_SCALE_INDEX_MASK);
            s_dest = destination->m_pixels + x + y * destination->m_width;
            s_destRow = s_dest;
            for (s_y = 0; s_y < s_tileHeight; s_y += scale) {
                s_nextSourceRow = s_source + s_sourceRowStep;
                s_destRow += s_destPitch;
                for (s_x = 0; s_x < s_tileWidth; s_x += scale) {
                    *s_dest = *s_source;
                    s_dest++;
                    s_source += scale;
                }
                s_source = s_nextSourceRow;
                s_dest = s_destRow;
            }
        } else {
            s_source = source->m_data + s_tileWidth * s_tileHeight * (flags & TILE_SCALE_INDEX_MASK)
                       + (s_tileHeight - 1) * s_tileWidth;
            s_dest = destination->m_pixels + x + y * destination->m_width;
            s_destRow = s_dest;
            for (s_y = 0; s_y < s_tileHeight; s_y += scale) {
                s_nextSourceRow = s_source - s_sourceRowStep;
                s_destRow += s_destPitch;
                for (s_x = 0; s_x < s_tileWidth; s_x += scale) {
                    *s_dest = *s_source;
                    s_dest++;
                    s_source += scale;
                }
                s_source = s_nextSourceRow;
                s_dest = s_destRow;
            }
        }
    } else {
        if ((flags & TILE_FLIP_VERTICAL) == 0) {
            s_source = source->m_data + s_tileWidth * s_tileHeight * (flags & TILE_SCALE_INDEX_MASK)
                       + s_tileWidth - 1;
            s_dest = destination->m_pixels + x + y * destination->m_width;
            s_destRow = s_dest;
            for (s_y = 0; s_y < s_tileHeight; s_y += scale) {
                s_nextSourceRow = s_source + s_sourceRowStep;
                s_destRow += s_destPitch;
                for (s_x = 0; s_x < s_tileWidth; s_x += scale) {
                    *s_dest = *s_source;
                    s_source -= scale;
                    s_dest++;
                }
                s_source = s_nextSourceRow;
                s_dest = s_destRow;
            }
        } else {
            s_source = source->m_data
                       + s_tileWidth * s_tileHeight * ((flags & TILE_SCALE_INDEX_MASK) + 1) - 1;
            s_dest = destination->m_pixels + x + y * destination->m_width;
            s_nextSourceRow = s_source - s_tileWidth;
            s_destRow = s_dest;
            for (s_y = 0; s_y < s_tileHeight; s_y += scale) {
                s_nextSourceRow = s_source - s_sourceRowStep;
                s_destRow += s_destPitch;
                for (s_x = 0; s_x < s_tileWidth; s_x += scale) {
                    *s_dest = *s_source;
                    s_source -= scale;
                    s_dest++;
                }
                s_source = s_nextSourceRow;
                s_dest = s_destRow;
            }
        }
    }
}
