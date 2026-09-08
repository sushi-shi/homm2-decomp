// Test resource constructors: production decoders, without game startup or AGG
// ownership. Only the explicit bitmap allocation constructor owns its pixels.
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>

u8 uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][DIM_PALETTE_COLOR_COUNT];
u8 gColorTableLighten[DIM_PALETTE_COLOR_COUNT];
u8 gColorTableNoCycle[DIM_PALETTE_COLOR_COUNT];

resource::resource(void)
    : resource(RESOURCE_CATEGORY_BITMAP, 0, RESOURCE_REFERENCE_EMPTY, nullptr) {}
resource::resource(ResourceCategory category, u32l id, i16 references, resource* next)
    : m_resourceType(category), m_refCount(references), m_id(id), m_next(next) {}
resource::~resource(void) {}

icon::icon(u32l id)
    : resource(RESOURCE_CATEGORY_ICON, id, RESOURCE_REFERENCE_UNMANAGED, nullptr),
      m_frameCount(0), m_data(nullptr) {}
icon::~icon() {}

bitmap::bitmap(void)
    : resource(RESOURCE_CATEGORY_BITMAP, 0, RESOURCE_REFERENCE_UNMANAGED, nullptr),
      m_bitmapType(BITMAP_TYPE_NONE), m_width(0), m_height(0), m_pixels(nullptr) {}
bitmap::bitmap(BitmapType type, i16 width, i16 height) : bitmap() {
    m_bitmapType = type;
    m_width = width;
    m_height = height;
    m_pixels = new u8[static_cast<std::size_t>(width) * height];
    m_refCount = RESOURCE_REFERENCE_INITIAL;
}
bitmap::~bitmap(void) {
    if (m_refCount == RESOURCE_REFERENCE_INITIAL)
        delete[] m_pixels;
}
