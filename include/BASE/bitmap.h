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

#pragma pack(push, 1)  // recovered layout is byte-packed
class bitmap : public resource {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    short  m_bitmapType;  // +0x10  bitmap storage/drawing type
    short  m_width;  // +0x12
    short  m_height;  // +0x14
    unsigned char *m_pixels;  // +0x16  pixel data
    // --- constructors ---
    bitmap(void);
    bitmap(short int, short int, short int);
    bitmap(unsigned long int);
    __declspec(dllexport) virtual inline ~bitmap();
    // --- methods ---
    void DrawToBufferCareful(short int, short int);
    __declspec(dllexport) inline void DrawToBuffer(short int, short int);
    void DrawToScreen(short int, short int);
    void GrabScreen(short int, short int);
    __declspec(dllexport) inline void GrabBitmap(class bitmap *, short int, short int);
    void GrabBitmapCareful(class bitmap *, short int, short int);
    void CopyTo(class bitmap *, int, int, int, int, int, int);
    void CopyToCareful(class bitmap *, int, int, int, int, int, int);
};
#pragma pack(pop)
SIZE(bitmap, 0x1a);
#endif // HOMM2_BASE_BITMAP_H
