// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icondf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icondf2b.h>
#include <BASE/IconEntry.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
// Per-call decoder scratch — its own contiguous file-static block (0x5381b8+).
DATA(0x005381b8) static int gFDX0;
DATA(0x005381bc) static int gFDXEnd;
DATA(0x005381c0) static unsigned int gFDCnt;
DATA(0x005381c4) static int gFDX;
DATA(0x005381c8) static int gFDClipR;
DATA(0x005381cc) static unsigned int gFDCnt2;
DATA(0x005381d0) static int gFDRow;
DATA(0x005381d4) static int gFDClipB;
DATA(0x005381d8) static unsigned char *gFDSrc;
DATA(0x005381dc) static unsigned char *gFDDst;
DATA(0x005381e0) static volatile int gFDY;
DATA(0x005381e4) static IconEntry *gFDEntry;
DATA(0x005381e8) static unsigned int gFDRun;

// @match-note
// Structurally recovered /O2 decoder; live 69.89%, retained max 73.4337%. Ours is 0x237 bytes and
// retail is 0x23b. Both have the same 0x4-byte frame, saved registers, command-loop CFG, and exact
// 37-relocation identity/multiplicity sequence (all 13 scratch globals plus uDimPal). The first raw
// divergence is +0x1d: ours loads x into ECX while retail loads it into EBX, after identical entry
// address/source setup; later residuals are register allocation and instruction scheduling.
// Retail deliberately tests clipX <= left before entering a clipped run, then tests clipX > left
// again inside. This makes the partial-left arm unreachable, but retail +0x18c and +0x19a prove the
// shipped CFG. Tried the typed IconEntry form, local X clip bounds, scoped volatile gFDY reads,
// direct <=/> spellings, and the retail pre-loop gFDDst publication. Narrowing X_GLOBAL.h to the
// canonical dimPalette.h changed only compiler/TU state and moved live fuzzy 70.55% -> 69.89%; do
// not restore the broad header to chase that temporary score. No permutation tool was used.
// Revisit after the SOURCE placeholder census reaches zero or after a real shared-header change.
VA(0x004daa20, 0x23b)
void FlipDimIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                         int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    char *data = srcIcon->m_data;
    IconEntry *entry = reinterpret_cast<IconEntry *>(data + frame * sizeof(IconEntry));
    unsigned char *srcData = reinterpret_cast<unsigned char *>(data + entry->srcOffset);
    int X = x;
    gFDEntry = entry;
    gFDSrc = srcData;
    int w = entry->w;
    X = X - entry->x;
    int entryY = entry->y;
    X = X - w;
    X++;
    gFDX0 = X;
    gFDY = y + entryY;
    gFDXEnd = w + X - 1;
    if (clip != 0) {
        int currentY = gFDY;
        if (X < clipX || clipW + clipX < w + X || currentY < clipY ||
            entry->h + currentY > clipY + clipH) {
            clip = 1;
            gFDClipR = clipX + clipW - 1;
            gFDClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    short pitch = dest->m_width;
    gFDRow = gFDY * pitch + reinterpret_cast<int>(dest->m_pixels);
    for (;;) {
        unsigned char *src = gFDSrc + 1;
        gFDX = X;
        gFDSrc = src;
        int cmd = src[-1];
        if (static_cast<signed char>(cmd) < 0) {
            gFDRun = cmd;
            int n = cmd & 0x7f;
            if (n == 0)
                return;
            X = X - n;
            continue;
        }
        gFDRun = cmd;
        if (cmd != 0) {
            if (clip == 0) {
                unsigned int cnt;
                gFDCnt = 0;
                unsigned char *dst = reinterpret_cast<unsigned char *>((gFDRow - cmd) + X + 1);
                gFDDst = dst;
                if (static_cast<int>(cmd) > 0) {
                    cnt = cmd;
                    gFDCnt = cmd;
                    do {
                        int px = *dst++;
                        cnt--;
                        gFDDst = dst;
                        dst[-1] =
                            (color * 0x100 + reinterpret_cast<unsigned char *>(uDimPal))[px];
                    } while (cnt != 0);
                }
            } else {
                int left;
                int currentY = gFDY;
                if (clipY <= currentY && currentY <= gFDClipB &&
                    (left = (X - cmd) + 1, clipX <= left) && X <= gFDClipR) {
                    int cn;
                    unsigned char *dst;
                    if (clipX > left) {
                        dst = reinterpret_cast<unsigned char *>(gFDRow + clipX);
                        cn = (X - clipX) + 1;
                    } else {
                        cn = cmd;
                        dst = reinterpret_cast<unsigned char *>((gFDRow - cmd) + X + 1);
                    }
                    gFDCnt2 = cn;
                    gFDDst = dst;
                    gFDCnt = 0;
                    if (static_cast<int>(cn) > 0) {
                        gFDCnt = cn;
                        unsigned int cnt = cn;
                        do {
                            int px = *dst++;
                            cnt--;
                            gFDDst = dst;
                            dst[-1] =
                                (reinterpret_cast<unsigned char *>(uDimPal) + color * 0x100)[px];
                        } while (cnt != 0);
                    }
                }
            }
            X = X - cmd;
            gFDRun = cmd;
            continue;
        }
        // newline
        X = gFDXEnd;
        gFDY = gFDY + 1;
        gFDRow = gFDRow + pitch;
    }
}
