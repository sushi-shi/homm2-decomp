#include <va.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
#include <string.h>

H2_ENUM_BEGIN(BitmapConstant)
    COPY_STRIDE = 640
H2_ENUM_END(BitmapConstant)

VA(0x004c5cd0, 0x4c)
bitmap::bitmap(void)
    : resource(RESOURCE_CATEGORY_BITMAP, 0, RESOURCE_REFERENCE_UNMANAGED, NULL) {
    m_bitmapType = BITMAP_TYPE_NONE;
    m_width = 0;
    m_height = 0;
    m_pixels = NULL;
}

VA(0x004c5d50, 0x99)
bitmap::bitmap(BitmapType type, i16 width, i16 height)
    : resource(RESOURCE_CATEGORY_BITMAP, 0, RESOURCE_REFERENCE_UNMANAGED, NULL) {
    m_bitmapType = type;
    m_width = width;
    m_height = height;
    m_pixels = new u8[width * height];
}

VA(0x004c5df0, 0xeb)
bitmap::bitmap(u32l id)
    : resource(RESOURCE_CATEGORY_BITMAP, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    i32 size;
    gpResourceManager->PointToFile(id);
    m_bitmapType = BitmapType(gpResourceManager->ReadWord());
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    size = m_width * m_height;
    m_pixels = new u8[size];
    PollSound();
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_pixels), size);
    PollSound();
}

VA(0x004c5ee0, 0x46)
bitmap::~bitmap() {
    if (m_pixels != NULL)
        delete m_pixels;
    m_pixels = NULL;
}

VA(0x004c5f30, 0x154)
void bitmap::DrawToBufferCareful(i16 x, i16 y) {
    i32 width;
    i32 destinationY;
    i32 height;
    i32 destinationX;

    if (x >= 0 && x + m_width <= gpWindowManager->m_screen->m_width && y >= 0
        && y + m_height <= gpWindowManager->m_screen->m_height) {
        DrawToBuffer(x, y);
        return;
    }
    width = m_width;
    height = m_height;
    if (x < 0) {
        width += x;
        destinationX = 0;
    } else {
        destinationX = x;
    }
    if (y < 0) {
        height += y;
        destinationY = 0;
    } else {
        destinationY = y;
    }
    if (destinationX + width > gpWindowManager->m_screen->m_width)
        width = gpWindowManager->m_screen->m_width - destinationX;
    if (destinationY + height > gpWindowManager->m_screen->m_height)
        height = gpWindowManager->m_screen->m_height - destinationY;
    if (width >= 0 && height >= 0)
        BlitBitmap(
            this, 0, 0, width, height, gpWindowManager->m_screen, destinationX, destinationY
        );
}

VA(0x004c6090, 0x47)
void bitmap::DrawToBuffer(i16 x, i16 y) {
    PollSound();
    BlitBitmap(this, 0, 0, m_width, m_height, gpWindowManager->m_screen, x, y);
    PollSound();
}

VA(0x004c60e0, 0x3d)
void bitmap::DrawToScreen(i16 x, i16 y) {
    PollSound();
    BlitBitmapToScreen(this, 0, 0, m_width, m_height, x, y);
    PollSound();
}

VA(0x004c6120, 0x29)
void bitmap::GrabScreen(i16 x, i16 y) {
    GrabBitmap(gpWindowManager->m_screen, x, y);
}

VA(0x004c6150, 0x36)
void bitmap::GrabBitmap(class bitmap* src, i16 x, i16 y) {
    BlitBitmap(src, x, y, m_width, m_height, this, 0, 0);
}

VA(0x004c6190, 0x12f)
void bitmap::GrabBitmapCareful(class bitmap* source, i16 x, i16 y) {
    i32 sourceX;
    i32 sourceY;
    i32 height;
    i32 width;

    if (x >= 0 && x + m_width <= source->m_width && y >= 0 && y + m_height <= source->m_height) {
        GrabBitmap(source, x, y);
        return;
    }
    width = m_width;
    height = m_height;
    if (x < 0) {
        width += x;
        sourceX = 0;
    } else {
        sourceX = x;
    }
    if (y < 0) {
        height += y;
        sourceY = 0;
    } else {
        sourceY = y;
    }
    if (sourceX + width > source->m_width)
        width = source->m_width - sourceX;
    if (sourceY + height > source->m_height)
        height = source->m_height - sourceY;
    if (width >= 0 && height >= 0)
        BlitBitmap(source, sourceX, sourceY, width, height, this, 0, 0);
}

VA(0x004c62c0, 0xb6)
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
    if (width != COPY_STRIDE) {
        for (i32 row = 0; row < height; row++) {
            memcpy(
                destination->m_pixels + destinationX + (destinationY + row) * COPY_STRIDE,
                m_pixels + sourceX + (sourceY + row) * COPY_STRIDE,
                width
            );
        }
    } else {
        memcpy(
            destination->m_pixels + destinationX + destinationY * COPY_STRIDE,
            m_pixels + sourceX + sourceY * COPY_STRIDE,
            width * height
        );
    }
    PollSound();
}

static inline i16 BitmapWidth(bitmap* value) {
    return value->m_width;
}

static inline u8* BitmapPixels(bitmap* value) {
    return value->m_pixels;
}

static inline u8* BitmapPixels(bitmap* value, i32 offset) {
    return value->m_pixels + offset;
}

VA(0x004c6380, 0x77)
void bitmap::CopyToCareful(
    class bitmap* destination,
    i32 destinationX,
    i32 destinationY,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height
) {
    if (width < 1)
        return;
    for (i32 row = 0; row < height; row++) {
        memcpy(
            destination->m_pixels + destinationX + (destinationY + row) * destination->m_width,
            m_pixels + sourceX + (sourceY + row) * m_width,
            width
        );
    }
}



