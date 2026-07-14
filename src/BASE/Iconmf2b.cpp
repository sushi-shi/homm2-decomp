// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconmf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconmf2b.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
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
// Fresh structural reconstruction; live 69.51% (previous retained maximum 69.4671%). Candidate is
// 0x20e bytes versus retail 0x212. Both have 167 instructions, 26 basic blocks, 20 branches, and an
// identical ordered successor graph. Both reserve one four-byte pitch home, save EBX/ESI/EDI/EBP,
// and return with `ret 0x24`. All 27 ordered relocation occurrences agree, including each of the
// ten real scratch owners at 0x538190..0x5381b4; there is no candidate-only target.
// The former @early-stop was invalid: that candidate had 28 blocks versus retail's 26 and first
// diverged during entry setup, so it did not prove a compiler wall. The typed frame selection,
// byte-row owner, named mono-RLE mask, short-circuit clip order, solid-run arm order, newline, and
// every loop backedge are now recovered. Retail deliberately tests clipX <= left both at clipped-
// run entry and before selecting the full/partial fill arm; the redundant shipped CFG is preserved.
// The remaining raw residual is broad register allocation/instruction scheduling: first difference
// is +0x12, where candidate's frame-offset LEA targets EBX while retail targets ECX. Relocation-
// masked comparison has 341 common unmasked bytes, 282 differing bytes, and a four-byte retail
// tail. This is structurally closed but remains unresolved/not-a-wall, not an @early-stop proof.
// Revisit only after a real shared-header/TU-state change; no regex or AST permuter was used.
VA(0x004da800, 0x212)
void FlipMonoIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                          int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    int entryOffset = frame * sizeof(IconEntry);
    int entryX = reinterpret_cast<IconEntry *>(data + entryOffset)->x;
    int srcOffset = reinterpret_cast<IconEntry *>(data + entryOffset)->srcOffset;
    IconEntry *entry = reinterpret_cast<IconEntry *>(data + entryOffset);
    gFMEntry = entry;
    gFMSrc = data + srcOffset;
    int X = x - entryX;
    int w = entry->w;
    X = X - w;
    int right = w + X + 1;
    X++;
    gFMX0 = X;
    X = right - 1;
    gFMXEnd = X;
    gFMY = y + entry->y;
    if (clip != 0) {
        if (gFMX0 < clipX || clipX + clipW < right || gFMY < clipY ||
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
