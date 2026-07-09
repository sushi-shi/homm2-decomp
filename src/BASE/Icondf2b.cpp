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
// Flip + dim variant: horizontal-flip decoder where every literal run remaps the DESTINATION pixels
// through the dim palette row (uDimPal + color*0x100)[dst], drawn right-to-left from (X-count+1);
// negative = skip (mask 0x7f). X/row are register-locals; the per-pixel dim loop uses a local ptr.
VA(0x004daa20, 0x23b)
void FlipDimIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                         int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    int off = frame * 13;
    gFDSrc = data + *reinterpret_cast<int *>(data + off + 9);
    IconEntry *e = reinterpret_cast<IconEntry *>(data + off);
    gFDEntry = e;
    int w = e->w;
    int ex = e->x;
    int ey = e->y;
    gFDX0 = ((x - ex) - w) + 1;
    gFDXEnd = w + gFDX0 - 1;
    gFDY = y + ey;
    if (clip != 0) {
        if (gFDX0 < clipX || clipW + clipX < w + gFDX0 || gFDY < clipY ||
            clipY + clipH < e->h + gFDY) {
            clip = 1;
            gFDClipR = clipX + clipW - 1;
            gFDClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    short pitch = dest->m_width;
    gFDRow = gFDY * pitch + reinterpret_cast<int>(dest->m_pixels);
    int X = gFDXEnd;
    for (;;) {
        gFDX = X;
        int cmd = *gFDSrc++;
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
                gFDCnt = 0;
                unsigned char *dp = reinterpret_cast<unsigned char *>((gFDRow - cmd) + 1 + X);
                gFDDst = dp;
                gFDCnt = cmd;
                if (cmd != 0) {
                    unsigned int cnt = cmd;
                    do {
                        int px = *dp++;
                        cnt--;
                        gFDDst = dp;
                        dp[-1] = (reinterpret_cast<unsigned char *>(uDimPal) + color * 0x100)[px];
                    } while (cnt != 0);
                }
            } else {
                int left;
                if (clipY <= gFDY && gFDY <= gFDClipB &&
                    (left = (X - cmd) + 1, clipX <= left) && X <= gFDClipR) {
                    unsigned int cn;
                    unsigned char *dp;
                    if (left < clipX) {
                        dp = reinterpret_cast<unsigned char *>(gFDRow + clipX);
                        cn = (X - clipX) + 1;
                    } else {
                        dp = reinterpret_cast<unsigned char *>((gFDRow - cmd) + 1 + X);
                        cn = cmd;
                    }
                    gFDCnt = 0;
                    gFDCnt2 = cn;
                    if (static_cast<int>(cn) > 0) {
                        gFDCnt = cn;
                        unsigned int cnt = cn;
                        do {
                            int px = *dp++;
                            cnt--;
                            gFDDst = dp;
                            dp[-1] = (reinterpret_cast<unsigned char *>(uDimPal) + color * 0x100)[px];
                        } while (cnt != 0);
                    }
                }
            }
            X = X - cmd;
            continue;
        }
        // newline
        X = gFDXEnd;
        gFDY = gFDY + 1;
        gFDRow = gFDRow + pitch;
    }
}
