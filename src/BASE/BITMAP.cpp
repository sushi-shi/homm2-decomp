#include <Ints.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <BASE/Raster.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
#include <string.h>

bitmap::bitmap(void)
    : resource(RESOURCE_CATEGORY_BITMAP, 0, RESOURCE_REFERENCE_UNMANAGED, NULL) {
    m_bitmapType = BITMAP_TYPE_NONE;
    m_width = 0;
    m_height = 0;
    m_pixels = NULL;
}

bitmap::bitmap(BitmapType type, i16 width, i16 height)
    : bitmap() {
    u32 size = 0;
    if (width < 0 || height < 0 || (width != 0 && height != 0
        && !images::RasterPayloadSize(1, width, height, std::numeric_limits<i32>::max(), size))) {
        ShutDown("Invalid memory bitmap dimensions.");
        return;
    }
    m_bitmapType = type;
    m_width = width;
    m_height = height;
    m_pixels = size == 0 ? nullptr : new u8[size];
}

bitmap::bitmap(u32l id)
    : resource(RESOURCE_CATEGORY_BITMAP, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    m_pixels = nullptr;
    m_width = m_height = 0;
    gpResourceManager->PointToFile(id);
    m_bitmapType = BitmapType(gpResourceManager->ReadWord());
    const i16 width = gpResourceManager->ReadWord();
    const i16 height = gpResourceManager->ReadWord();
    const u32 memberSize = gpResourceManager->GetFileSize(id);
    u32 size = 0;
    if ((m_bitmapType != BITMAP_TYPE_NONE && m_bitmapType != BITMAP_TYPE_MEMORY)
        || memberSize < 6
        || !images::RasterPayloadSize(1, width, height, memberSize - 6, size)) {
        ShutDown("Invalid BMP type, dimensions or payload length.");
        return;
    }
    m_width = width;
    m_height = height;
    m_pixels = new u8[size];
    PollSound();
    gpResourceManager->ReadBlock(m_pixels, size);
    PollSound();
}
bitmap::~bitmap(void) {
    if (m_pixels != NULL)
        delete[] m_pixels;
    m_pixels = NULL;
}

void bitmap::DrawToBufferCareful(i16 x, i16 y) {
    BlitBitmap(this, 0, 0, m_width, m_height, gpWindowManager->m_screen, x, y);
}

void bitmap::DrawToBuffer(i16 x, i16 y) {
    PollSound();
    BlitBitmap(this, 0, 0, m_width, m_height, gpWindowManager->m_screen, x, y);
    PollSound();
}

void bitmap::DrawToScreen(i16 x, i16 y) {
    PollSound();
    BlitBitmapToScreen(this, 0, 0, m_width, m_height, x, y);
    PollSound();
}

void bitmap::GrabScreen(i16 x, i16 y) {
    GrabBitmap(gpWindowManager->m_screen, x, y);
}

void bitmap::GrabBitmap(class bitmap* src, i16 x, i16 y) {
    BlitBitmap(src, x, y, m_width, m_height, this, 0, 0);
}

void bitmap::GrabBitmapCareful(class bitmap* source, i16 x, i16 y) {
    BlitBitmap(source, x, y, m_width, m_height, this, 0, 0);
}

void bitmap::CopyTo(
    class bitmap* destination,
    i32 destinationX,
    i32 destinationY,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height
) {
    PollSound();
    BlitBitmap(this, sourceX, sourceY, width, height, destination, destinationX, destinationY);
    PollSound();
}

void bitmap::CopyToCareful(
    class bitmap* destination,
    i32 destinationX,
    i32 destinationY,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height
) {
    BlitBitmap(this, sourceX, sourceY, width, height, destination, destinationX, destinationY);
}
