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
// Flip + mono variant: literal runs are solid `color` fills drawn right-to-left. Retail's clipped
// predicate really requires clipX <= left before repeating the comparison for the consequently dead
// left < clipX arm; this was verified directly at +0x165 and +0x17c, rather than simplified away.
// /O2 codegen wall: base body 0x20a versus retail 0x212, with the exact 27-entry relocation target
// multiset and no wrong target. After relocation masking, prologue +0x0..+0x14 and row setup base
// +0xde..+0xfe / retail +0xd3..+0xf3 are byte-identical. Cursor tail base +0x105..+0x12b /
// retail +0xfa..+0x120, post-fill +0x1cb..+0x1d8 / +0x1d3..+0x1e0, and suffix
// +0x1d8..+0x20a / +0x1e0..+0x212 are byte-identical except one branch displacement each.
// Systematic clean variants covered direct/temporary entry fields, source/cursor expression order,
// four-call versus shared memset tails, pointer/count spellings, and a 180-iteration AST search.
VA(0x004da800, 0x212)
void FlipMonoIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                          int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    gFMEntry = &entries[frame];
    gFMSrc = data + entries[frame].srcOffset;
    gFMX0 = ((x - entries[frame].x) - entries[frame].w) + 1;
    int X = entries[frame].w + gFMX0 - 1;
    gFMXEnd = X;
    gFMY = y + entries[frame].y;
    if (clip != 0) {
        if (gFMX0 < clipX || clipW + clipX < gFMX0 + entries[frame].w || gFMY < clipY ||
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
    for (;;) {
        gFMSrc++;
        gFMX = X;
        int cmd = gFMSrc[-1];
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
                    (left = (X - cmd) + 1, clipX <= left) && gFMClipR >= X) {
                    unsigned int count = cmd;
                    unsigned char *dst;
                    if (left < clipX) {
                        count = (X - clipX) + 1;
                        dst = reinterpret_cast<unsigned char *>(gFMRow + clipX);
                    } else {
                        dst = reinterpret_cast<unsigned char *>((gFMRow - cmd) + 1 + X);
                    }
                    memset(dst, color, count);
                }
            }
            gFMRun = cmd;
            X = X - cmd;
            continue;
        }
        // newline
        X = gFMXEnd;
        gFMY = gFMY + 1;
        gFMRow = gFMRow + pitch;
    }
}
