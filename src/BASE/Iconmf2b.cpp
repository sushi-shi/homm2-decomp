// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconmf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconmf2b.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconMonoRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <string.h>
// Per-call decoder scratch — its own contiguous file-static block (0x538190+).
DATA(0x00538190) static unsigned int gFMRun;
DATA(0x00538194) static unsigned char *gFMSrc;
DATA(0x00538198) static IconEntry *gFMEntry;
DATA(0x0053819c) static int gFMX;
DATA(0x005381a0) static int gFMX0;
DATA(0x005381a4) static int gFMClipB;
DATA(0x005381a8) static unsigned char *gFMRow;
DATA(0x005381ac) static int gFMY;
DATA(0x005381b0) static int gFMClipR;
DATA(0x005381b4) static int gFMXEnd;

// @match-note
// Fresh geometry/type recovery. Candidate is 0x205 bytes versus retail 0x212, with 166 versus 167
// instructions. Both have 26 basic blocks, 20 branch instructions, an identical ordered successor
// graph, one four-byte pitch home, the same saved registers, and `ret 0x24`.
// Retail materializes the selected IconEntry pointer before reading fields. It then retains three
// distinct horizontal values through setup: x0 (left edge), right (exclusive right edge), and X
// (inclusive running edge, decremented by each command). The old body overwrote the left-edge local
// and reloaded gFMX0 during clipping. The typed pointer and real three-value lifetime are now
// recovered. Moving entryY to its retail lifetime before right-edge formation further aligns setup.
// Nonvolatile gFMY is canonical: neither PoL nor the related H2X build proves volatile storage. MSVC
// coalesces the two retail setup-clip gFMY reads into one reload, leaving 26/27 relocations with
// only-base=0; this is unresolved code generation and must not be forced with a qualifier or reads.
// The H2X build independently corroborates the geometry lifetimes only, not source qualifiers.
// First relocation-masked raw divergence is now +0x1c: retail publishes gFMEntry before adding the
// data base to srcOffset; candidate schedules that add first and publishes at +0x1e. The remaining
// residual is register allocation/instruction scheduling. Direct entry->srcOffset use regresses
// sharply; splitting x0 initialization is byte-identical. Earlier cursor, memset-tail, pointer/count,
// predicate, header-state, and AST-search families remain closed. Retail's redundant clipX <= left
// test and full/partial arm order are retained. This remains unresolved/not-a-wall; no regex or AST
// permutation was used. Revisit after a real TU-state change or in the post-placeholder last mile.
VA(0x004da800, 0x212)
void FlipMonoIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                          int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    int entryOffset = frame * sizeof(IconEntry);
    IconEntry *entry = reinterpret_cast<IconEntry *>(data + entryOffset);
    int entryX = entry->x;
    int srcOffset = entry->srcOffset;
    gFMEntry = entry;
    int x0 = x;
    x0 = x0 - entryX;
    gFMSrc = data + srcOffset;
    int w = entry->w;
    x0 = x0 - w;
    int entryY = entry->y;
    int right = w + x0 + 1;
    x0++;
    gFMX0 = x0;
    int X = right - 1;
    gFMXEnd = X;
    gFMY = y + entryY;
    if (clip != 0) {
        if (x0 < clipX || clipX + clipW < right || gFMY < clipY ||
            entry->h + gFMY > clipY + clipH) {
            clip = 1;
            gFMClipR = clipX + clipW - 1;
            gFMClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    short pitch = dest->m_width;
    gFMRow = dest->m_pixels + gFMY * pitch;
    for (;;) {
        unsigned char *src = gFMSrc + 1;
        gFMX = X;
        gFMSrc = src;
        int cmd = src[-1];
        if (static_cast<signed char>(cmd) < 0) {
            gFMRun = cmd;
            int n = cmd & ICON_RLE_MONO_RUN_MASK;
            if (n == 0)
                return;
            X = X - n;
            continue;
        }
        gFMRun = cmd;
        if (cmd != 0) {
            if (clip == 0) {
                memset((gFMRow - cmd) + 1 + X, color, cmd);
            } else {
                int left;
                int currentY = gFMY;
                if (clipY <= currentY && currentY <= gFMClipB &&
                    (left = (X - cmd) + 1, clipX <= left) && gFMClipR >= X) {
                    if (clipX <= left) {
                        memset((gFMRow - cmd) + 1 + X, color, cmd);
                    } else {
                        memset(gFMRow + clipX, color, (X - clipX) + 1);
                    }
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
