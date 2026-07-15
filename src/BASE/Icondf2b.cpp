// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icondf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icondf2b.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMonoRle.h>
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
DATA(0x005381d0) static unsigned char *gFDRow;
DATA(0x005381d4) static int gFDClipB;
DATA(0x005381d8) static unsigned char *gFDSrc;
DATA(0x005381dc) static unsigned char *gFDDst;
DATA(0x005381e0) static int gFDY;
DATA(0x005381e4) static IconEntry *gFDEntry;
DATA(0x005381e8) static unsigned int gFDRun;

// @match-note
// Complete /O2 decoder: 166 instructions, 25 branches, the same four-byte frame/saved registers,
// and exact 37/37 external relocation identity/multiplicity (all scratch owners plus uDimPal).
// Candidate ends at +0x238 versus retail +0x23b. First divergence is +0x1d: retail schedules the
// formal-x load before publishing gFDEntry, while this TU publishes the entry first and carries the
// left edge in EDI. The remaining differences are broad register allocation/scheduling with the
// same command-loop and clipping successors. gFDRow is a byte pointer and gFDY is ordinary scratch;
// the former integer row model and `volatile` reload coercion were removed. Retail tests
// clipX <= left both before the clipped run and before choosing its full arm, leaving the emitted
// partial-left arm unreachable; preserve that shipped CFG. Prior entry/bound, destination/count,
// loop-order, predicate, header-state, and AST families are exhausted. Revisit only after a genuine
// shared-header/TU-state change; do not restore qualifiers or integer pointer arithmetic.
VA(0x004daa20, 0x23b)
void FlipDimIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                         int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    IconEntry *entry = &entries[frame];
    unsigned char *srcData = reinterpret_cast<unsigned char *>(entries) + entry->srcOffset;
    int x0 = x;
    gFDEntry = entry;
    gFDSrc = srcData;
    int w = entry->w;
    int entryY = entry->y;
    x0 = x0 - entry->x;
    x0 = x0 - w;
    x0++;
    gFDX0 = x0;
    gFDY = y + entryY;
    int X = w + x0 - 1;
    gFDXEnd = X;
    if (clip != 0) {
        int currentY;
        if (x0 < clipX || clipW + clipX < w + x0 || (currentY = gFDY) < clipY ||
            entry->h + currentY > clipY + clipH) {
            clip = 1;
            gFDClipR = clipX + clipW - 1;
            gFDClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    short pitch = dest->m_width;
    gFDRow = dest->m_pixels + gFDY * pitch;
    for (;;) {
        unsigned char *src = gFDSrc + 1;
        gFDX = X;
        gFDSrc = src;
        int cmd = src[-1];
        if (static_cast<signed char>(cmd) < 0) {
            gFDRun = cmd;
            int n = cmd & ICON_RLE_MONO_RUN_MASK;
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
                unsigned char *dst = (gFDRow - cmd) + X + 1;
                gFDDst = dst;
                if (static_cast<int>(cmd) > 0) {
                    cnt = cmd;
                    gFDCnt = cmd;
                    do {
                        int px = *dst++;
                        cnt--;
                        gFDDst = dst;
                        dst[-1] = uDimPal[0][color][px];
                    } while (cnt != 0);
                }
            } else {
                int left;
                int currentY = gFDY;
                if (clipY <= currentY && currentY <= gFDClipB &&
                    (left = (X - cmd) + 1, clipX <= left) && X <= gFDClipR) {
                    int cn;
                    unsigned char *dst;
                    if (clipX <= left) {
                        cn = cmd;
                        dst = (gFDRow - cmd) + X + 1;
                    } else {
                        dst = gFDRow + clipX;
                        cn = (X - clipX) + 1;
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
                            dst[-1] = uDimPal[0][color][px];
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
