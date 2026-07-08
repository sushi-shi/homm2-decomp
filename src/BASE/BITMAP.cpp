// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BITMAP.OBJ   from: .\basewin.lib
// functions: 14   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
#include <string.h>
VA(0x004cffc0, 0x2a)
bitmap::bitmap(void) : resource(0, 0, -1, 0)
{
    field_0x10 = 0;
    m_width = 0;
    m_height = 0;
    m_pixels = 0;
}

VA(0x004d0040, 0x53)
bitmap::bitmap(short p1, short p2, short p3) : resource(0, 0, -1, 0)
{
    field_0x10 = p1;
    m_width = p2;
    m_height = p3;
    m_pixels = static_cast<unsigned char *>(BaseAlloc(p3 * p2, __FILE__, __LINE__));
}

VA(0x004d00a0, 0x8f)
bitmap::bitmap(unsigned long id) : resource(0, id, 1, 0)
{
    gpResourceManager->PointToFile(id);
    field_0x10 = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    m_pixels = static_cast<unsigned char *>(BaseAlloc(m_height * m_width, __FILE__, __LINE__));
    PollSound();
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(m_pixels), m_height * m_width);
    PollSound();
}

VA(0x004d0130, 0x2c)
bitmap::~bitmap()
{
    if (m_pixels != 0)
        BaseFree(m_pixels, __FILE__, __LINE__);
    m_pixels = 0;
}

// @early-stop 1% — semantically exact clip-then-BlitBitmap (guards use >=0 to match the
// retail test/jl). /O2 assigns this/params to different base registers than retail from
// the first instruction, so every downstream byte differs — pervasive regalloc wall.
VA(0x004d0160, 0xff)
void bitmap::DrawToBufferCareful(short int param_1, short int param_2)
{
    if (param_1 >= 0) {
        if (m_width + param_1 <= gpWindowManager->m_screen->m_width && param_2 >= 0 &&
            m_height + param_2 <= gpWindowManager->m_screen->m_height) {
            PollSound();
            BlitBitmap(this, 0, 0, m_width, m_height, gpWindowManager->m_screen, param_1,
                       param_2);
            PollSound();
            return;
        }
    }
    int iVar3 = m_width;
    int iVar4 = m_height;
    int iVar2 = param_1;
    if (param_1 < 0) {
        iVar3 = iVar3 + iVar2;
        iVar2 = 0;
    }
    int local_8 = param_2;
    if (param_2 < 0) {
        local_8 = 0;
        iVar4 = iVar4 + param_2;
    }
    bitmap *pbVar1 = gpWindowManager->m_screen;
    if (pbVar1->m_width < iVar2 + iVar3) {
        iVar3 = pbVar1->m_width - iVar2;
    }
    if (pbVar1->m_height < local_8 + iVar4) {
        iVar4 = pbVar1->m_height - local_8;
    }
    if (iVar3 >= 0 && iVar4 >= 0) {
        BlitBitmap(this, 0, 0, iVar3, iVar4, pbVar1, iVar2, local_8);
    }
}

VA(0x004d0260, 0x3c)
void bitmap::DrawToBuffer(short x, short y)
{
    PollSound();
    BlitBitmap(this, 0, 0, m_width, m_height, gpWindowManager->m_screen, x, y);
    PollSound();
}

VA(0x004d02a0, 0x32)
void bitmap::DrawToScreen(short x, short y)
{
    PollSound();
    BlitBitmapToScreen(this, 0, 0, m_width, m_height, x, y);
    PollSound();
}

VA(0x004d02e0, 0x2d)
void bitmap::GrabScreen(short x, short y)
{
    BlitBitmap(gpWindowManager->m_screen, x, y, m_width, m_height, this, 0, 0);
}

VA(0x004d0310, 0x26)
void bitmap::GrabBitmap(class bitmap *src, short x, short y)
{
    BlitBitmap(src, x, y, m_width, m_height, this, 0, 0);
}

// @early-stop 1% — same clip-then-BlitBitmap shape and /O2 base-register wall as
// DrawToBufferCareful above.
VA(0x004d0340, 0xf0)
void bitmap::GrabBitmapCareful(class bitmap *param_1, short int param_2, short int param_3)
{
    if (param_2 >= 0) {
        if (param_2 + m_width <= param_1->m_width && param_3 >= 0) {
            if (param_3 + m_height <= param_1->m_height) {
                BlitBitmap(param_1, param_2, param_3, m_width, m_height, this, 0, 0);
                return;
            }
        }
    }
    int iVar1, iVar2;
    int iVar3 = m_width;
    int iVar4 = m_height;
    if (param_2 < 0) {
        iVar3 = iVar3 + param_2;
        iVar1 = 0;
    } else {
        iVar1 = param_2;
    }
    if (param_3 < 0) {
        iVar4 = iVar4 + param_3;
        iVar2 = 0;
    } else {
        iVar2 = param_3;
    }
    if (param_1->m_width < iVar1 + iVar3) {
        iVar3 = param_1->m_width - iVar1;
    }
    if (param_1->m_height < iVar2 + iVar4) {
        iVar4 = param_1->m_height - iVar2;
    }
    if (iVar3 >= 0 && iVar4 >= 0) {
        BlitBitmap(param_1, iVar1, iVar2, iVar3, iVar4, this, 0, 0);
    }
}

// @early-stop 1% — semantically exact: both memcpys inline to the retail rep movs (the
// full-width fast path even drops rep movsb since param_7*0x280 is a compile-time multiple
// of 4, matching the dead byte-remainder in the decomp). Only the per-row address
// arithmetic differs by /O2 register allocation — same loop-body regalloc wall as
// CopyToCareful, not source-controllable.
VA(0x004d0430, 0xcb)
void bitmap::CopyTo(class bitmap *param_1, int param_2, int param_3, int param_4, int param_5,
                    int param_6, int param_7)
{
    PollSound();
    if (param_6 == 0x280) {
        // full-width (640) blit: source and dest rows are contiguous, one flat copy.
        unsigned char *puVar5 = m_pixels + param_4 + param_5 * 0x280;
        unsigned char *puVar6 = param_1->m_pixels + param_2 + param_3 * 0x280;
        memcpy(puVar6, puVar5, param_7 * 0x280);
    } else if (param_7 > 0) {
        int iVar4 = param_5 * 0x280;
        int iVar1 = param_3 * 0x280;
        do {
            int iVar2 = reinterpret_cast<int>(m_pixels) + iVar4;
            iVar4 = iVar4 + 0x280;
            unsigned char *puVar5 = reinterpret_cast<unsigned char *>(param_4 + iVar2);
            unsigned char *puVar6 = param_1->m_pixels + iVar1 + param_2;
            memcpy(puVar6, puVar5, param_6);
            iVar1 = iVar1 + 0x280;
            param_7 = param_7 - 1;
        } while (param_7 != 0);
    }
    PollSound();
}

// @early-stop 2% — semantically exact (memcpy inlines to the retail rep movsd/movsb,
// prologue byte-identical, comma-guard iVar3=0 placement matches). The per-row address
// math in the loop body differs only by /O2 register allocation (retail interleaves
// index→addr per row; MSVC 4.2 schedules both indices first regardless of how the source
// is written — verified across three expression forms). Loop-body regalloc wall.
VA(0x004d0500, 0x65)
void bitmap::CopyToCareful(class bitmap *param_1, int param_2, int param_3, int param_4,
                           int param_5, int param_6, int param_7)
{
    int iVar3;
    if (param_6 > 0 && (iVar3 = 0, param_7 > 0)) {
        do {
            int iVar1 = param_5 + iVar3;
            int iVar4 = param_3 + iVar3;
            iVar3 = iVar3 + 1;
            unsigned char *puVar5 = m_pixels + iVar1 * m_width + param_4;
            unsigned char *puVar6 = param_1->m_pixels + iVar4 * param_1->m_width + param_2;
            memcpy(puVar6, puVar5, param_6);
        } while (iVar3 < param_7);
    }
}


// ===== vtable bitmap (root)  (1 slots) =====
//  [ 0] VA(0x004cfff0, 0x41)  void * bitmap::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(bitmap, 0x004eba3c);
