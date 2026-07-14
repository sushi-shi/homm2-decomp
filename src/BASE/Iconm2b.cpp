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
// Complete mono-RLE decoder with the selected IconEntry kept as one typed lifetime: entry/source
// publication, initial clipping, skip/end, solid fill, four clipped-fill quadrants, and newline
// state agree semantically. Both sides reserve only the four-byte pitch home (`sub esp,4`), save
// EBX/ESI/EDI/EBP, and return with `ret 0x24`. Excluding retail's two trailing padding NOPs, retail
// is 181 instructions and ours is 182; both have 31 basic blocks, 46 edges, and the same ordered
// successor graph. Ours ends at +0x25a versus retail +0x266.
// First divergence is setup +0x14: retail adds the data base to ECX, publishes the entry, and only
// then loads x; VC4.2 hoists ours' x into EBX before that add. Relocations remain 36/38 with no
// base-only target. Retail X0 is +0x34/+0x59/+0x63/+0x23b versus ours
// +0x39/+0x58/+0x22f, and retail Y is +0x4a/+0x7a/+0x88/+0xd8/+0x159/+0x247/+0x253
// versus ours +0x4d/+0x76/+0xc9/+0x14c/+0x23b/+0x247. The missing occurrences are the second
// setup X0 reload (+0x63; ours keeps the +0x58 value) and second setup Y reload (+0x88; ours keeps
// the +0x76 value); every other scratch owner agrees occurrence-for-occurrence.
// Replacing the former entry-offset/field snapshots with the single typed entry lifetime improves
// the retained shape and is kept. A chained X publication is byte-identical; an explicit working-X
// local changes entry/data registers broadly and regresses. Direct global-entry consumption adds
// one or three non-retail entry relocations. Earlier closed attempts include delayed working-X,
// reversed/split Y sums, by-value y accumulation, indexed entry selection, comparison reversals,
// direct scratch ownership, narrow headers, and alternate fill-arm spellings. This is unresolved,
// not a proven wall; revisit only after a real reachable header/TU-state change. No permutation
// tool was used.
VA(0x004cfae0, 0x266)
void MonoIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                      int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entry = reinterpret_cast<IconEntry *>(data + frame * sizeof(IconEntry));
    gMonoEntry = entry;
    gMonoSrc = data + entry->srcOffset;
    gMonoX0 = entry->x + x;
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
