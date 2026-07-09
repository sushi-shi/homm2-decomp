// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icond2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icond2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own file-static block (0x534bf0+).
static IconEntry *gDimEntry;
static unsigned char *gDimSrc;
static int gDimX0;
static int gDimX;
static int gDimY;
static int gDimRow;
static unsigned int gDimRun;
static unsigned int gDimCnt;
static unsigned int gDimCnt2;
static unsigned char *gDimDst;
static int gDimClipR;
static int gDimClipB;

// @early-stop
// Dim variant of IconToBitmap: every literal run remaps the DESTINATION pixels through the dim
// palette row (uDimPal + color*0x100)[dst] (no source read, no solid/colour sub-commands; negative
// = skip, mask 0x7f). X/row are register-locals; the per-pixel dim loop uses a local dest pointer.
VA(0x004cfd50, 0x26e)
void DimIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                     int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    gDimEntry = &entries[frame];
    gDimSrc = data + entries[frame].srcOffset;
    gDimX0 = x + entries[frame].x;
    gDimY = y + entries[frame].y;
    if (clip != 0) {
        if (gDimX0 < clipX || clipW + clipX < entries[frame].w + gDimX0 || gDimY < clipY ||
            clipY + clipH < entries[frame].h + gDimY) {
            clip = 1;
            gDimClipR = clipX + clipW - 1;
            gDimClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    short pitch = dest->m_width;
    unsigned char *row =
        reinterpret_cast<unsigned char *>(gDimY * pitch + reinterpret_cast<int>(dest->m_pixels));
    int X = gDimX0;
    for (;;) {
        gDimX = X;
        int cmd = *gDimSrc++;
        if (static_cast<signed char>(cmd) < 0) {
            // skip run / end-of-sprite
            gDimRow = reinterpret_cast<int>(row);
            gDimRun = cmd;
            int n = cmd & 0x7f;
            if (n == 0)
                return;
            X = X + n;
            continue;
        }
        gDimRun = cmd;
        if (cmd != 0) {
            if (clip == 0) {
                unsigned char *dst = row + X;
                gDimCnt = 0;
                gDimDst = dst;
                if (static_cast<int>(cmd) > 0) {
                    gDimCnt = cmd;
                    unsigned int cnt = cmd;
                    do {
                        int px = *dst++;
                        cnt--;
                        gDimDst = dst;
                        dst[-1] = (reinterpret_cast<unsigned char *>(uDimPal) + color * 0x100)[px];
                    } while (cnt != 0);
                }
            } else {
                int right;
                if (clipY <= gDimY && gDimClipB >= gDimY &&
                    (right = X + cmd, clipX < right) && gDimClipR >= X) {
                    unsigned int cn;
                    unsigned char *dst;
                    if (clipX > X) {
                        cn = clipW;
                        if (right <= gDimClipR)
                            cn = (cmd - clipX) + X;
                        dst = row + clipX;
                    } else {
                        cn = cmd;
                        if (gDimClipR < right)
                            cn = (gDimClipR - X) + 1;
                        dst = row + X;
                    }
                    gDimDst = dst;
                    gDimCnt2 = cn;
                    gDimCnt = 0;
                    if (static_cast<int>(cn) > 0) {
                        gDimCnt = cn;
                        unsigned int cnt = cn;
                        do {
                            int px = *dst++;
                            cnt--;
                            gDimDst = dst;
                            dst[-1] = (reinterpret_cast<unsigned char *>(uDimPal) + color * 0x100)[px];
                        } while (cnt != 0);
                    }
                }
            }
            X = X + cmd;
            gDimRun = cmd;
            continue;
        }
        // newline
        X = gDimX0;
        gDimY = gDimY + 1;
        row = row + pitch;
    }
}
