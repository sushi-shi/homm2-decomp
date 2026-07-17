#ifndef HOMM2_BASE_BITMAP_H
#define HOMM2_BASE_BITMAP_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 13 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <BASE/resource.h>

// Proven bitmap storage tags. The packed class member remains a short because
// bitmap resources serialize this value as a 16-bit word.
typedef enum BitmapType {
    BITMAP_TYPE_NONE = 0,
    BITMAP_TYPE_MEMORY = 0x21
} BitmapType;

typedef enum BitmapConstant {
    BITMAP_COPY_STRIDE = 640 // full-screen row stride assumed by CopyTo's fast path
} BitmapConstant;

#pragma pack(push, 1) // recovered layout is byte-packed
class bitmap : public resource {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    i16 m_bitmapType; // +0x10  bitmap storage/drawing type
    i16 m_width;      // +0x12
    i16 m_height;     // +0x14
    u8* m_pixels;     // +0x16  pixel data
    // --- constructors ---
    bitmap(void);
    bitmap(i16, i16, i16);
    bitmap(u32l);
    __declspec(dllexport) virtual inline ~bitmap();
    // --- methods ---
    void DrawToBufferCareful(i16, i16);
    __declspec(dllexport) inline void DrawToBuffer(i16, i16);
    void DrawToScreen(i16, i16);
    void GrabScreen(i16, i16);
    __declspec(dllexport) inline void GrabBitmap(class bitmap*, i16, i16);
    void GrabBitmapCareful(class bitmap*, i16, i16);
    void CopyTo(class bitmap*, i32, i32, i32, i32, i32, i32);
    void CopyToCareful(class bitmap*, i32, i32, i32, i32, i32, i32);
};
#pragma pack(pop)
SIZE(bitmap, 0x1a);
#endif // HOMM2_BASE_BITMAP_H
