// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconm2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconm2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconMonoRle.h>
#include <string.h>
// Per-call decoder scratch — its own 0x534bcc+ file-static block.
DATA(0x00534bcc) static int gMonoClipR;
DATA(0x00534bd0) static unsigned char *gMonoRow;
DATA(0x00534bd4) static IconEntry *gMonoEntry;
DATA(0x00534bd8) static unsigned char *gMonoSrc;
DATA(0x00534bdc) static int gMonoX0;
DATA(0x00534be0) static unsigned int gMonoRun;
DATA(0x00534be4) static int gMonoY;
DATA(0x00534be8) static int gMonoClipB;
DATA(0x00534bec) static int gMonoX;

// @match-note
// Structurally complete mono-RLE decoder. Both sides use one four-byte pitch slot (`sub esp,4`),
// save EBX/ESI/EDI/EBP, and return with `ret 0x24`; skip/end, solid fill, four clipping quadrants,
// and newline CFGs agree. First divergence is +0x11: ours retains the 13-byte frame offset in EBX
// and reads entry.x/srcOffset before publishing the entry, while retail materializes the entry in
// ECX and publishes it first. Ours ends at +0x25e versus retail +0x266. Relocations are 36/38 with
// no base-only target: retail X0 is +0x34/+0x59/+0x63/+0x23b versus ours
// +0x3b/+0x5e/+0x233, and retail Y is +0x4a/+0x7a/+0x88/+0xd8/+0x159/+0x247/+0x253
// versus ours +0x4e/+0x7a/+0xdc/+0x150/+0x23f/+0x24b. Thus the missing setup accesses are
// retail's first X0 clip reload (+0x59; ours forwards EAX) and second Y clip reload (+0x88; ours
// reuses EBX); all other seven scratch owners agree occurrence-for-occurrence. Publishing a local
// entry before its field snapshots worsens X0 to 2/4 (Y remains 6/7) and shrinks to 0x253. Moving
// the real working-X initialization after row setup yields the right X0 count but at the wrong
// point (+0xde), moves
// the X store from retail +0x38 to +0xe2, and grows to 0x263. Reversing or splitting the Y sum is
// byte-identical; accumulating into the by-value y parameter collapses further to 0x252.
// Indexed entry selection, direct scratch ownership, direct positive clip tests, narrow owner
// headers, typed row storage, and retail fill-arm order were also tried. This remains unresolved,
// not a proven wall. Revisit after a reachable icon/header TU-state change or post-coverage; no
// permutation tool was used in this pass.
VA(0x004cfae0, 0x266)
void MonoIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                      int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    int entryOffset = frame * sizeof(IconEntry);
    int entryX = reinterpret_cast<IconEntry *>(data + entryOffset)->x;
    int srcOffset = reinterpret_cast<IconEntry *>(data + entryOffset)->srcOffset;
    IconEntry *entry = reinterpret_cast<IconEntry *>(data + entryOffset);
    gMonoEntry = entry;
    gMonoSrc = data + srcOffset;
    gMonoX0 = entryX + x;
    gMonoX = gMonoX0;
    gMonoY = entry->y + y;
    if (clip != 0) {
        if (gMonoX0 < clipX || clipX + clipW < entry->w + gMonoX0 ||
            gMonoY < clipY || clipY + clipH < entry->h + gMonoY) {
            clip = 1;
            gMonoClipR = clipX + clipW - 1;
            gMonoClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    short pitch = dest->m_width;
    unsigned char *row = dest->m_pixels + gMonoY * pitch;
    for (;;) {
        gMonoSrc++;
        int cmd = gMonoSrc[-1];
        if (static_cast<signed char>(cmd) < 0) {
            // skip run / end-of-sprite (negative command masks 7 bits)
            gMonoRow = row;
            gMonoRun = cmd;
            int n = cmd & ICON_RLE_MONO_RUN_MASK;
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
