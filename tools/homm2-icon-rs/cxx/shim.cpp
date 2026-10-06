// Native test shim over the reconstructed HoMM2 icon decoders.
//
// Compiled by build.rs together with the ten reconstructed decoder
// translation units from the repository source tree (HOMM2_SOURCE_DIR).
// Provides a C ABI for the Rust parity tests. image_test_support.cpp supplies
// lightweight resource constructors and palette storage without game startup.

#include <cstring>
#include <cstdio>
#include <BASE/ImageDecode.h>

#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <SOURCE/dimPalette.h>
#include <BASE/Icon2b.h>
#include <BASE/Iconf2b.h>
#include <BASE/icon2bc.h>
#include <BASE/iconf2bc.h>
#include <BASE/icon2by.h>
#include <BASE/iconf2by.h>
#include <BASE/Iconm2b.h>
#include <BASE/Iconmf2b.h>
#include <BASE/Icond2b.h>
#include <BASE/Icondf2b.h>

namespace {

thread_local icon g_icon(0);
thread_local bitmap g_bitmap;

icon* make_icon(u8* body, std::size_t size) {
    g_icon.m_data = body;
    g_icon.m_dataSize = static_cast<u32>(size);
    g_icon.m_frameCount = 1;
    return &g_icon;
}

bitmap* make_bitmap(u8* pixels, i32 width, i32 height) {
    g_bitmap.m_width = static_cast<i16>(width);
    g_bitmap.m_height = static_cast<i16>(height);
    g_bitmap.m_pixels = pixels;
    return &g_bitmap;
}

IconDrawClipMode clip_mode(i32 value) {
    return static_cast<IconDrawClipMode>(value);
}

} // namespace

extern "C" {

void h2cxx_set_dim_palettes(const unsigned char* flat) {
    std::memcpy(uDimPal, flat, sizeof(uDimPal));
}

int h2cxx_validate_icon(const unsigned char* body, std::size_t size, int count) {
    const char* error = nullptr;
    if (images::ValidateIconPayload({body, size}, count, error))
        return 1;
    std::fprintf(stderr, "ICN validation failed: %s\n", error);
    return 0;
}

void h2cxx_retail_variant(unsigned char* body, std::size_t size, int count, int frame,
                          unsigned char* pixels, int width, int height, int variant,
                          const int* geometry, unsigned char* table, signed char* shear) {
    icon* source = make_icon(body, size);
    source->m_frameCount = static_cast<i16>(count);
    bitmap* target = make_bitmap(pixels, width, height);
    const int x = geometry[0], y = geometry[1], cx = geometry[2], cy = geometry[3];
    const int cw = geometry[4], ch = geometry[5];
    switch (variant) {
    case 0: IconToBitmap(source, target, x, y, frame, ICON_DRAW_CLIP, cx, cy, cw, ch, 7); break;
    case 1: FlipIconToBitmap(source, target, x, y, frame, ICON_DRAW_CLIP, cx, cy, cw, ch, 7); break;
    case 2: IconToBitmapColorTable(source, target, x, y, frame, ICON_DRAW_CLIP, cx, cy, cw, ch, 7, table, 1); break;
    case 3: FlipIconToBitmapColorTable(source, target, x, y, frame, ICON_DRAW_CLIP, cx, cy, cw, ch, 7, table); break;
    case 4: IconToBitmapYModify(source, target, x, y, frame, ICON_DRAW_CLIP, cx, cy, cw, ch, 7,
                               {shear, static_cast<std::size_t>(height)}); break;
    case 5: FlipIconToBitmapYModify(source, target, x, y, frame, ICON_DRAW_CLIP, cx, cy, cw, ch, 7,
                                   {shear, static_cast<std::size_t>(height)}); break;
    case 6: MonoIconToBitmap(source, target, x, y, frame, 7, ICON_DRAW_CLIP, cx, cy, cw, ch); break;
    case 7: FlipMonoIconToBitmap(source, target, x, y, frame, 7, ICON_DRAW_CLIP, cx, cy, cw, ch); break;
    case 8: DimIconToBitmap(source, target, x, y, frame, 3, ICON_DRAW_CLIP, cx, cy, cw, ch); break;
    case 9: FlipDimIconToBitmap(source, target, x, y, frame, 3, ICON_DRAW_CLIP, cx, cy, cw, ch); break;
    }
}

void h2cxx_icon_to_bitmap(
    unsigned char* body, std::size_t bodySize, unsigned char* pixels, int width, int height,
    int x, int y, int frame, int clip,
    int clipX, int clipY, int clipW, int clipH, int color
) {
    IconToBitmap(make_icon(body, bodySize), make_bitmap(pixels, width, height),
                 x, y, frame, clip_mode(clip), clipX, clipY, clipW, clipH, color);
}

void h2cxx_flip_icon_to_bitmap(
    unsigned char* body, std::size_t bodySize, unsigned char* pixels, int width, int height,
    int x, int y, int frame, int clip,
    int clipX, int clipY, int clipW, int clipH, int color
) {
    FlipIconToBitmap(make_icon(body, bodySize), make_bitmap(pixels, width, height),
                     x, y, frame, clip_mode(clip), clipX, clipY, clipW, clipH, color);
}

void h2cxx_icon_to_bitmap_color_table(
    unsigned char* body, std::size_t bodySize, unsigned char* pixels, int width, int height,
    int x, int y, int frame, int clip,
    int clipX, int clipY, int clipW, int clipH, int color,
    unsigned char* colorTable, int dimGate
) {
    IconToBitmapColorTable(make_icon(body, bodySize), make_bitmap(pixels, width, height),
                           x, y, frame, clip_mode(clip), clipX, clipY, clipW, clipH,
                           color, colorTable, dimGate);
}

void h2cxx_flip_icon_to_bitmap_color_table(
    unsigned char* body, std::size_t bodySize, unsigned char* pixels, int width, int height,
    int x, int y, int frame, int clip,
    int clipX, int clipY, int clipW, int clipH, int color,
    unsigned char* colorTable
) {
    FlipIconToBitmapColorTable(make_icon(body, bodySize), make_bitmap(pixels, width, height),
                               x, y, frame, clip_mode(clip), clipX, clipY, clipW, clipH,
                               color, colorTable);
}

void h2cxx_icon_to_bitmap_y_modify(
    unsigned char* body, std::size_t bodySize, unsigned char* pixels, int width, int height,
    int x, int y, int frame, int clip,
    int clipX, int clipY, int clipW, int clipH, int color,
    signed char* shear
) {
    IconToBitmapYModify(make_icon(body, bodySize), make_bitmap(pixels, width, height),
                        x, y, frame, clip_mode(clip), clipX, clipY, clipW, clipH,
                        color, {shear, static_cast<std::size_t>(height)});
}

void h2cxx_flip_icon_to_bitmap_y_modify(
    unsigned char* body, std::size_t bodySize, unsigned char* pixels, int width, int height,
    int x, int y, int frame, int clip,
    int clipX, int clipY, int clipW, int clipH, int color,
    signed char* shear
) {
    FlipIconToBitmapYModify(make_icon(body, bodySize), make_bitmap(pixels, width, height),
                            x, y, frame, clip_mode(clip), clipX, clipY, clipW, clipH,
                            color, {shear, static_cast<std::size_t>(height)});
}

void h2cxx_mono_icon_to_bitmap(
    unsigned char* body, std::size_t bodySize, unsigned char* pixels, int width, int height,
    int x, int y, int frame, int color, int clip,
    int clipX, int clipY, int clipW, int clipH
) {
    MonoIconToBitmap(make_icon(body, bodySize), make_bitmap(pixels, width, height),
                     x, y, frame, color, clip_mode(clip), clipX, clipY, clipW, clipH);
}

void h2cxx_flip_mono_icon_to_bitmap(
    unsigned char* body, std::size_t bodySize, unsigned char* pixels, int width, int height,
    int x, int y, int frame, int color, int clip,
    int clipX, int clipY, int clipW, int clipH
) {
    FlipMonoIconToBitmap(make_icon(body, bodySize), make_bitmap(pixels, width, height),
                         x, y, frame, color, clip_mode(clip), clipX, clipY, clipW, clipH);
}

void h2cxx_dim_icon_to_bitmap(
    unsigned char* body, std::size_t bodySize, unsigned char* pixels, int width, int height,
    int x, int y, int frame, int color, int clip,
    int clipX, int clipY, int clipW, int clipH
) {
    DimIconToBitmap(make_icon(body, bodySize), make_bitmap(pixels, width, height),
                    x, y, frame, color, clip_mode(clip), clipX, clipY, clipW, clipH);
}

void h2cxx_flip_dim_icon_to_bitmap(
    unsigned char* body, std::size_t bodySize, unsigned char* pixels, int width, int height,
    int x, int y, int frame, int color, int clip,
    int clipX, int clipY, int clipW, int clipH
) {
    FlipDimIconToBitmap(make_icon(body, bodySize), make_bitmap(pixels, width, height),
                        x, y, frame, color, clip_mode(clip), clipX, clipY, clipW, clipH);
}

} // extern "C"
