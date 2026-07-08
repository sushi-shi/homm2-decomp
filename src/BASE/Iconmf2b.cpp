// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconmf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconmf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/Misc.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block.
static unsigned int gFMRun;
static unsigned char *gFMSrc;
static IconEntry *gFMEntry;
static int gFMX;
static int gFMX0;
static int gFMXEnd;
static int gFMClipB;
static int gFMRow;
static int gFMY;
static int gFMClipR;

// @early-stop
// Flip + mono variant: horizontal-flip silhouette. Every literal run is a solid single-colour fill
// (the `color` param) drawn right-to-left from (X-count+1); negative = skip (mask 0x7f). Working X
// lives in global gFMX (starts at gFMXEnd, decreases); row base is a register-local. /O2 reg wall.
VA(0x004da800, 0x212)
void FlipMonoIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                          int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    int w = entries[frame].w;
    gFMEntry = &entries[frame];
    gFMSrc = data + entries[frame].srcOffset;
    gFMX0 = ((x - entries[frame].x) - w) + 1;
    gFMXEnd = w + gFMX0 - 1;
    gFMY = y + entries[frame].y;
    if (clip != 0) {
        if (gFMX0 < clipX || clipW + clipX < gFMX0 + w || gFMY < clipY ||
            clipY + clipH < entries[frame].h + gFMY) {
            clip = 1;
            gFMClipR = clipX + clipW - 1;
            gFMClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    short pitch = dest->m_width;
    gFMRow = gFMY * pitch + reinterpret_cast<int>(dest->m_pixels);
    int X = gFMXEnd;
    for (;;) {
        gFMX = X;
        int cmd = *gFMSrc++;
        if (static_cast<signed char>(cmd) < 0) {
            gFMRun = cmd;
            int n = cmd & 0x7f;
            if (n == 0)
                return;
            X = X - n;
            continue;
        }
        gFMRun = cmd;
        if (cmd != 0) {
            if (clip == 0) {
                memset(reinterpret_cast<unsigned char *>((gFMRow - cmd) + 1 + X), color, cmd);
            } else {
                int left;
                if (clipY <= gFMY && gFMY <= gFMClipB &&
                    (left = (X - cmd) + 1, clipX <= left) && X <= gFMClipR) {
                    unsigned int cn = cmd;
                    unsigned char *dst;
                    if (left < clipX) {
                        cn = (X - clipX) + 1;
                        dst = reinterpret_cast<unsigned char *>(gFMRow + clipX);
                    } else {
                        dst = reinterpret_cast<unsigned char *>((gFMRow - cmd) + 1 + X);
                    }
                    memset(dst, color, cn);
                }
            }
            gFMRun = cmd;
            X = X - cmd;
            continue;
        }
        // newline
        X = gFMXEnd;
        gFMRow = gFMRow + pitch;
        gFMY = gFMY + 1;
    }
}
