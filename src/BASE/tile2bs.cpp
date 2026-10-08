#include <match.h>
#include <BASE/tile2bs.h>
#include <BASE/TILE.h>
#include <BASE/tileset.h>
#include <BASE/bitmap.h>
#include <SOURCE/KB.h>

// TileToBitmap at a reduced zoom: every `scale`-th pixel of every `scale`-th
// row of the tile, in the tile's flip. Its walk state is file-static, like
// the assembly blitters'. Only the editor links this object.
H2_ENUM_BEGIN(TileScaleConstant)
    TILE_SCALE_NATIVE = 1,
    TILE_SCALE_INDEX_MASK = 0x3fff
H2_ENUM_END(TileScaleConstant)

DATA(0x004a75e4) static u8* s_dest;
#define s_nextSourceRow s_nextSourceRowContents // spelling fixes .bss order
DATA(0x004a75e8) static char* s_nextSourceRow;
#define s_tileWidth s_tileWidthStore // spelling fixes .bss order
DATA(0x004a75ec) static i32 s_tileWidth;
DATA(0x004a75f0) static i32 s_y;
#define s_x s_xTable // spelling fixes .bss order
DATA(0x004a75f4) static i32 s_x;
#define s_tileHeight s_tileHeightContents // spelling fixes .bss order
DATA(0x004a75f8) static i32 s_tileHeight;
DATA(0x004a75fc) static i32 s_sourceRowStep;
#define s_destRow s_destRowBlockBase // spelling fixes .bss order
DATA(0x004a7600) static u8* s_destRow;
DATA(0x004a7604) static char* s_source;
#define s_destPitch s_destPitchHolder // spelling fixes .bss order
DATA(0x004a7608) static i32 s_destPitch;

VA(0x00439fb0, 0x524)
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
