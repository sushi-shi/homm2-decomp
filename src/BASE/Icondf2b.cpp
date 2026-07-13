// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icondf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icondf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own file-static block.
static int gFDX0;
static int gFDXEnd;
static unsigned int gFDCnt;
static unsigned int gFDCnt2;
static int gFDRow;
static IconEntry *gFDEntry;
static int gFDClipR;
static int gFDX;
static int gFDClipB;
static unsigned char *gFDSrc;
static unsigned char *gFDDst;
static int gFDY;
static unsigned int gFDRun;

// @early-stop
// /O2 register-allocation wall after complete decoder recovery: base and retail are both 0x23b.
// The relocation/branch-masked command dispatch is byte-identical at base +0xf6..+0x11c versus
// retail +0xf9..+0x11f (39 bytes). Residual spans are setup/fetch base +0x00..+0xf5 versus retail
// +0x00..+0xf8, no-clip base +0x11d..+0x170 versus retail +0x120..+0x16f, clipped base
// +0x171..+0x208 versus retail +0x170..+0x208, and newline/return +0x209..+0x23a in both: retail
// assigns the setup fields, row/X operands, palette/count loads, and loop load/decrement among
// EAX/EBX/ESI/EDI differently. Relocations are base 38 versus retail 37; the sole extra is a gFDY
// reload at base +0x185, with no base-only target, and uDimPal plus all 13 scratch addresses agree.
// Retail itself gates clipped runs with clipX <= left && X <= clipR, then repeats clipX > left for
// a dead partial-left arm; exact compare/branch dataflow was verified and the normal overlap gate
// was rejected. Tried direct/indexed entry forms, local/global X bounds and destination pointers,
// field/store orders, split source fetches, signed/unsigned counts, palette/count/loop schedules,
// repeated/local clipping expressions, three AST searches (580 walks), and 120 text variants.
VA(0x004daa20, 0x23b)
void FlipDimIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                         int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    int off = frame * 13;
    gFDSrc = data + *reinterpret_cast<int *>(off + data + 9);
    IconEntry *e = reinterpret_cast<IconEntry *>(off + data);
    gFDEntry = e;
    int w = e->w;
    gFDX0 = ((x - e->x) - w) + 1;
    int X = w + gFDX0 - 1;
    gFDXEnd = X;
    gFDY = y + e->y;
    if (clip != 0) {
        if (clipX > gFDX0 || clipW + clipX < w + gFDX0 || gFDY < clipY ||
            e->h + gFDY > clipY + clipH) {
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
                if (static_cast<int>(cmd) >= 1) {
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
                if (clipY <= gFDY && gFDY <= gFDClipB &&
                    (left = (X - cmd) + 1, clipX < left + 1) && X <= gFDClipR) {
                    unsigned int cn;
                    unsigned char *dst;
                    if (left < clipX) {
                        dst = reinterpret_cast<unsigned char *>(gFDRow + clipX);
                        cn = (X - clipX) + 1;
                    } else {
                        cn = cmd;
                        dst = reinterpret_cast<unsigned char *>((gFDRow - cmd) + X + 1);
                    }
                    gFDCnt = 0;
                    gFDCnt2 = cn;
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
