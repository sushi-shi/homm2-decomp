// Native test shim over the reconstructed HoMM2 icon decoders.
//
// Compiled by build.rs together with the ten reconstructed decoder
// translation units from the repository source tree (HOMM2_SOURCE_DIR).
// Provides the uDimPal storage the decoders reference and a C ABI the Rust
// parity tests call. Lightweight test constructors keep icon/bitmap objects
// alive without linking the game resource manager; the decoders only read the
// fields set here and never make virtual calls.

#include <cstring>

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

u8 uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][DIM_PALETTE_COLOR_COUNT];
// Generated parity streams use only the twelve real dim tables. These two
// adjacent retail fallback tables are still defined so the reconstructed
// decoder units link as a faithful standalone family.
u8 gColorTableLighten[DIM_PALETTE_COLOR_COUNT];
u8 gColorTableNoCycle[DIM_PALETTE_COLOR_COUNT];

resource::resource(void)
    : m_resourceType(RESOURCE_CATEGORY_BITMAP),
      m_refCount(RESOURCE_REFERENCE_EMPTY),
      m_id(0),
      m_next(nullptr) {}

resource::resource(ResourceCategory category, u32l id, i16 refCount, resource* next)
    : m_resourceType(category), m_refCount(refCount), m_id(id), m_next(next) {}

resource::~resource(void) {}

icon::icon(u32l id)
    : resource(RESOURCE_CATEGORY_ICON, id, RESOURCE_REFERENCE_UNMANAGED, nullptr),
      m_frameCount(0),
      m_data(nullptr) {}

inline icon::~icon() {
    m_data = nullptr;
}

bitmap::bitmap(void)
    : resource(RESOURCE_CATEGORY_BITMAP, 0, RESOURCE_REFERENCE_UNMANAGED, nullptr),
      m_bitmapType(BITMAP_TYPE_NONE),
      m_width(0),
      m_height(0),
      m_pixels(nullptr) {}

bitmap::~bitmap(void) {
    m_pixels = nullptr;
}

namespace {

thread_local icon g_icon(0);
thread_local bitmap g_bitmap;

icon* make_icon(u8* body) {
    g_icon.m_data = body;
    return &g_icon;
}

bitmap* make_bitmap(u8* pixels, i32 width) {
    g_bitmap.m_width = static_cast<i16>(width);
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

void h2cxx_icon_to_bitmap(
    unsigned char* body, unsigned char* pixels, int width,
    int x, int y, int frame, int clip,
    int clipX, int clipY, int clipW, int clipH, int color
) {
    IconToBitmap(make_icon(body), make_bitmap(pixels, width),
                 x, y, frame, clip_mode(clip), clipX, clipY, clipW, clipH, color);
}

void h2cxx_flip_icon_to_bitmap(
    unsigned char* body, unsigned char* pixels, int width,
    int x, int y, int frame, int clip,
    int clipX, int clipY, int clipW, int clipH, int color
) {
    FlipIconToBitmap(make_icon(body), make_bitmap(pixels, width),
                     x, y, frame, clip_mode(clip), clipX, clipY, clipW, clipH, color);
}

void h2cxx_icon_to_bitmap_color_table(
    unsigned char* body, unsigned char* pixels, int width,
    int x, int y, int frame, int clip,
    int clipX, int clipY, int clipW, int clipH, int color,
    unsigned char* colorTable, int dimGate
) {
    IconToBitmapColorTable(make_icon(body), make_bitmap(pixels, width),
                           x, y, frame, clip_mode(clip), clipX, clipY, clipW, clipH,
                           color, colorTable, dimGate);
}

void h2cxx_flip_icon_to_bitmap_color_table(
    unsigned char* body, unsigned char* pixels, int width,
    int x, int y, int frame, int clip,
    int clipX, int clipY, int clipW, int clipH, int color,
    unsigned char* colorTable
) {
    FlipIconToBitmapColorTable(make_icon(body), make_bitmap(pixels, width),
                               x, y, frame, clip_mode(clip), clipX, clipY, clipW, clipH,
                               color, colorTable);
}

void h2cxx_icon_to_bitmap_y_modify(
    unsigned char* body, unsigned char* pixels, int width,
    int x, int y, int frame, int clip,
    int clipX, int clipY, int clipW, int clipH, int color,
    signed char* shear
) {
    IconToBitmapYModify(make_icon(body), make_bitmap(pixels, width),
                        x, y, frame, clip_mode(clip), clipX, clipY, clipW, clipH,
                        color, shear);
}

void h2cxx_flip_icon_to_bitmap_y_modify(
    unsigned char* body, unsigned char* pixels, int width,
    int x, int y, int frame, int clip,
    int clipX, int clipY, int clipW, int clipH, int color,
    signed char* shear
) {
    FlipIconToBitmapYModify(make_icon(body), make_bitmap(pixels, width),
                            x, y, frame, clip_mode(clip), clipX, clipY, clipW, clipH,
                            color, shear);
}

void h2cxx_mono_icon_to_bitmap(
    unsigned char* body, unsigned char* pixels, int width,
    int x, int y, int frame, int color, int clip,
    int clipX, int clipY, int clipW, int clipH
) {
    MonoIconToBitmap(make_icon(body), make_bitmap(pixels, width),
                     x, y, frame, color, clip_mode(clip), clipX, clipY, clipW, clipH);
}

void h2cxx_flip_mono_icon_to_bitmap(
    unsigned char* body, unsigned char* pixels, int width,
    int x, int y, int frame, int color, int clip,
    int clipX, int clipY, int clipW, int clipH
) {
    FlipMonoIconToBitmap(make_icon(body), make_bitmap(pixels, width),
                         x, y, frame, color, clip_mode(clip), clipX, clipY, clipW, clipH);
}

void h2cxx_dim_icon_to_bitmap(
    unsigned char* body, unsigned char* pixels, int width,
    int x, int y, int frame, int color, int clip,
    int clipX, int clipY, int clipW, int clipH
) {
    DimIconToBitmap(make_icon(body), make_bitmap(pixels, width),
                    x, y, frame, color, clip_mode(clip), clipX, clipY, clipW, clipH);
}

void h2cxx_flip_dim_icon_to_bitmap(
    unsigned char* body, unsigned char* pixels, int width,
    int x, int y, int frame, int color, int clip,
    int clipX, int clipY, int clipW, int clipH
) {
    FlipDimIconToBitmap(make_icon(body), make_bitmap(pixels, width),
                        x, y, frame, color, clip_mode(clip), clipX, clipY, clipW, clipH);
}

} // extern "C"
