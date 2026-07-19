#include <va.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
#include <string.h>

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\BITMAP.CPP"

H2_ENUM_BEGIN(BitmapConstant)
    COPY_STRIDE = 640 // Full-screen row stride assumed by CopyTo's fast path.
H2_ENUM_END(BitmapConstant)

VA(0x004cffc0, 0x2a)
bitmap::bitmap(void)
    : resource(RESOURCE_CATEGORY_BITMAP, 0, RESOURCE_REFERENCE_UNMANAGED, NULL) {
    m_bitmapType = BITMAP_TYPE_NONE;
    m_width = 0;
    m_height = 0;
    m_pixels = NULL;
}

#line 15
VA(0x004d0040, 0x53)
bitmap::bitmap(BitmapType p1, i16 p2, i16 p3)
    : resource(RESOURCE_CATEGORY_BITMAP, 0, RESOURCE_REFERENCE_UNMANAGED, NULL) {
    DATA(0x0051f9f8) static char dimensionsAllocationSourceFile[] = RETAIL_FILE;
    m_bitmapType = p1;
    m_width = p2;
    m_height = p3;
    m_pixels = static_cast<u8*>(H2_ALLOC_AT(p3 * p2, dimensionsAllocationSourceFile, 21));
}

#line 28
VA(0x004d00a0, 0x8f)
bitmap::bitmap(u32l id)
    : resource(RESOURCE_CATEGORY_BITMAP, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    DATA(0x0051fa20) static char resourceAllocationSourceFile[] = RETAIL_FILE;
    i32 size;
    gpResourceManager->PointToFile(id);
    m_bitmapType = BitmapType(gpResourceManager->ReadWord());
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    size = m_height * m_width;
    m_pixels = static_cast<u8*>(H2_ALLOC_AT(size, resourceAllocationSourceFile, 37));
    PollSound();
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_pixels), size);
    PollSound();
}
#line 48
VA(0x004d0130, 0x2c)
inline bitmap::~bitmap() {
    DATA(0x0051fa48) static char destructionSourceFile[] = RETAIL_FILE;
    if (m_pixels != NULL)
        H2_FREE_AT(m_pixels, destructionSourceFile, 51);
    m_pixels = NULL;
}

// @semantic: residual begins at +0x0: retail starts sub esp,8; ours first loads x into DX.
VA(0x004d0160, 0xFF)
void bitmap::DrawToBufferCareful(i16 x, i16 y) {
    i32 width;
    i32 destinationY;
    i32 height;
    i32 destinationX;
    bitmap* screen;

    if (x >= 0 && x + m_width <= gpWindowManager->m_screen->m_width && y >= 0
        && y + m_height <= gpWindowManager->m_screen->m_height) {
        DrawToBuffer(x, y);
    } else {
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
        screen = gpWindowManager->m_screen;
        if (destinationX + width > screen->m_width)
            width = screen->m_width - destinationX;
        if (destinationY + height > screen->m_height)
            height = screen->m_height - destinationY;
        if (width >= 0 && height >= 0)
            BlitBitmap(this, 0, 0, width, height, screen, destinationX, destinationY);
    }
}

VA(0x004d0260, 0x3c)
inline void bitmap::DrawToBuffer(i16 x, i16 y) {
    PollSound();
    BlitBitmap(this, 0, 0, m_width, m_height, gpWindowManager->m_screen, x, y);
    PollSound();
}

VA(0x004d02a0, 0x32)
void bitmap::DrawToScreen(i16 x, i16 y) {
    PollSound();
    BlitBitmapToScreen(this, 0, 0, m_width, m_height, x, y);
    PollSound();
}

VA(0x004d02e0, 0x2d)
void bitmap::GrabScreen(i16 x, i16 y) {
    GrabBitmap(gpWindowManager->m_screen, x, y);
}

VA(0x004d0310, 0x26)
inline void bitmap::GrabBitmap(class bitmap* src, i16 x, i16 y) {
    BlitBitmap(src, x, y, m_width, m_height, this, 0, 0);
}

VA(0x004d0340, 0xf0)
void bitmap::GrabBitmapCareful(class bitmap* source, i16 x, i16 y) {
    i32 sourceX;
    i32 sourceY;
    i32 height;
    i32 width;

    if (x >= 0 && x + m_width <= source->m_width && y >= 0 && y + m_height <= source->m_height) {
        GrabBitmap(source, x, y);
    } else {
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
}

VA(0x004d0430, 0xcb)
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
        if (height > 0) {
            i32 rowCount = height;
            i32 destinationRowOffset;
            i32 sourceRowOffset;
            sourceRowOffset = sourceY * COPY_STRIDE;
            destinationRowOffset = destinationY * COPY_STRIDE;
            do {
                memcpy(
                    destination->m_pixels + destinationRowOffset + destinationX,
                    m_pixels + sourceRowOffset + sourceX,
                    width
                );
                sourceRowOffset += COPY_STRIDE;
                destinationRowOffset += COPY_STRIDE;
                rowCount--;
            } while (rowCount != 0);
        }
    } else {
        memcpy(
            destination->m_pixels + destinationY * COPY_STRIDE + destinationX,
            m_pixels + sourceY * COPY_STRIDE + sourceX,
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

// @semantic: first residual is +0x1: retail keeps this/destination/row in EAX/EBX/EDX, while this TU state colors them EDX/EAX/EBX.
VA(0x004d0500, 0x65)
void bitmap::CopyToCareful(
    class bitmap* destination,
    i32 destinationX,
    i32 destinationY,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height
) {
    const u32 copyWidth = width;
    i32 row;
    bitmap* target = destination;
    bitmap* source = this;
    if (width >= 1) {
        row = 0;
        if (row < height) {
            do {
                memcpy(
                    BitmapPixels(target, (destinationY + row) * BitmapWidth(target)) + destinationX,
                    BitmapPixels(source) + (sourceY + row) * BitmapWidth(source) + sourceX,
                    copyWidth
                );
                ++row;
            } while (row < height);
        }
    }
}


VTBL(bitmap, 0x004eba3c);

#undef RETAIL_FILE
