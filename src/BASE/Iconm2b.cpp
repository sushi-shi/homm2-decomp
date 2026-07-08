// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconm2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconm2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block.
static IconEntry *gMonoEntry;
static unsigned char *gMonoSrc;
static int gMonoX0;
static int gMonoX;
static int gMonoY;
static int gMonoRow;
static unsigned int gMonoRun;
static int gMonoClipR;
static int gMonoClipB;

// @early-stop
// Mono silhouette variant of IconToBitmap: every literal run is a solid single-colour fill (the
// `color` param), no per-pixel source and no dim/colour sub-commands (negative = skip, mask 0x7f).
// Working X lives in the global gMonoX; row base is a register-local. Residual is the /O2 reg wall.
VA(0x004cfae0, 0x266)
void MonoIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                      int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    gMonoEntry = &entries[frame];
    gMonoSrc = data + entries[frame].srcOffset;
    gMonoX0 = x + entries[frame].x;
    gMonoX = gMonoX0;
    gMonoY = y + entries[frame].y;
    if (clip != 0) {
        if (gMonoX0 < clipX || clipW + clipX < entries[frame].w + gMonoX0 || gMonoY < clipY ||
            clipY + clipH < entries[frame].h + gMonoY) {
            clip = 1;
            gMonoClipR = clipX + clipW - 1;
            gMonoClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    short pitch = dest->m_width;
    unsigned char *row =
        reinterpret_cast<unsigned char *>(gMonoY * pitch + reinterpret_cast<int>(dest->m_pixels));
    for (;;) {
        int cmd = *gMonoSrc++;
        if (static_cast<signed char>(cmd) < 0) {
            // skip run / end-of-sprite (negative command masks 7 bits)
            gMonoRow = reinterpret_cast<int>(row);
            gMonoRun = cmd;
            int n = cmd & 0x7f;
            if (n == 0)
                return;
            gMonoX = gMonoX + n;
            continue;
        }
        gMonoRun = cmd;
        if (cmd != 0) {
            // solid mono fill of `cmd` pixels
            if (clip == 0) {
                memset(row + gMonoX, color, cmd);
            } else {
                int right;
                if (clipY <= gMonoY && gMonoClipB >= gMonoY &&
                    (right = gMonoX + cmd, clipX < right) && gMonoClipR >= gMonoX) {
                    if (clipX > gMonoX) {
                        unsigned int cn = clipW;
                        if (right <= gMonoClipR)
                            cn = (cmd - clipX) + gMonoX;
                        memset(row + clipX, color, cn);
                    } else {
                        unsigned int cn = cmd;
                        if (gMonoClipR < right)
                            cn = (gMonoClipR - gMonoX) + 1;
                        memset(row + gMonoX, color, cn);
                    }
                }
            }
            gMonoRun = cmd;
            gMonoX = gMonoX + cmd;
            continue;
        }
        // newline
        gMonoX = gMonoX0;
        row = row + pitch;
        gMonoY = gMonoY + 1;
    }
}
