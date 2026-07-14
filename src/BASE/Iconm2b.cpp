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
// /O2 byte-proven register-allocation wall: base 0x257 versus retail 0x266. Setup is base
// +0x000..+0x0de versus retail +0x000..+0x0f0. After the cursor load/xor scheduling pair, base
// +0x0e5..+0x146 and retail +0x0f7..+0x158 are raw-byte identical with relocation and branch-
// displacement bytes masked; the same is true of suffix +0x214..+0x257/+0x223..+0x266. The
// clipped four-quadrant fill is base +0x146..+0x214 versus retail +0x158..+0x223.
// Base has 36/38 retail relocations with no base-only target. The missing occurrences are retail's
// second gMonoX0 and gMonoY setup reloads at +0x061 and +0x086; every decoder/fill relocation
// occurrence agrees. Tried merged and four-way memset forms, global/local/static entry forms,
// source-cursor postincrement forms, comparison reversals and inclusive-bound equivalents,
// combined positive-AND, explicit else-if clipping, and the full 180-attempt AST permutation.
VA(0x004cfae0, 0x266)
void MonoIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                      int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    gMonoEntry = &entries[frame];
    gMonoSrc = data + entries[frame].srcOffset;
    gMonoX0 = entries[frame].x + x;
    gMonoX = gMonoX0;
    gMonoY = entries[frame].y + y;
    if (clip != 0) {
        if (clipX > gMonoX0 || gMonoX0 + entries[frame].w > clipX + clipW ||
            clipY > gMonoY || gMonoY + entries[frame].h > clipY + clipH) {
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
        gMonoSrc++;
        int cmd = gMonoSrc[-1];
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
                    if (clipX <= gMonoX) {
                        if (gMonoClipR >= right) {
                            memset(row + gMonoX, color, cmd);
                        } else {
                            memset(row + gMonoX, color, (gMonoClipR - gMonoX) + 1);
                        }
                    } else {
                        if (gMonoClipR >= right) {
                            memset(row + clipX, color, (cmd - clipX) + gMonoX);
                        } else {
                            memset(row + clipX, color, clipW);
                        }
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
