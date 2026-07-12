// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icon2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icon2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
#include <string.h>
// Per-call decoder scratch — its own 0x534c20+ global block (modeled in _globals_model.h).
DATA(0x00534c20) static int gIcRow;
DATA(0x00534c24) static int gIcPitch;
DATA(0x00534c28) static unsigned char gIcColor;
DATA(0x00534c2c) static unsigned char *gIcDimPal;
DATA(0x00534c30) static unsigned int gIcRun;
DATA(0x00534c34) static unsigned int gIcCnt;
DATA(0x00534c38) static unsigned char *gIcSrc;
DATA(0x00534c3c) static unsigned char *gIcDimDst;
DATA(0x00534c40) static int gIcClipR;
DATA(0x00534c44) static int gIcClipB;
DATA(0x00534c48) static int gIcX0;
DATA(0x00534c4c) static unsigned int gIcDimLen;
DATA(0x00534c50) static int gIcY;
DATA(0x00534c54) static int gIcX;
DATA(0x00534c58) static unsigned char *gIcEntry;
DATA(0x00534c5c) static unsigned int gIcCnt2;

// @early-stop
// RLE icon->bitmap blitter with per-scanline clip (base of the Icon*2b* /O2 family). Full logic
// recovered; block layout + memcpy/memset/dim loops match. Residual ~41% is the /O2 register-fusion
// wall: MSVC allocates srcIcon->m_data to esi in retail but edi here, and that reg-coloring choice
// cascades into scratch-register (eax/ecx/edx/esi/edi) mismatches through the whole loop body. The
// documented family wall (Iconm2b peaked at 41%; this reaches ~59%).


VA(0x004d0570, 0x4ed)
void IconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                  int clip, int clipX, int clipY, int clipW, int clipH, int color)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    gIcSrc = data + entries[frame].srcOffset;
    gIcEntry = reinterpret_cast<unsigned char *>(&entries[frame]);
    gIcX0 = x + entries[frame].x;
    gIcPitch = dest->m_width;
    gIcY = entries[frame].y + y;
    if (clip != 0) {
        if (gIcX0 < clipX || clipW + clipX < entries[frame].w + gIcX0 || gIcY < clipY ||
            clipY + clipH < entries[frame].h + gIcY) {
            clip = 1;
            gIcClipR = clipX + clipW - 1;
            gIcClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    unsigned char *row =
        reinterpret_cast<unsigned char *>(gIcPitch * gIcY + reinterpret_cast<int>(dest->m_pixels));
    int X = gIcX0;
    for (;;) {
        gIcSrc++;
        int cmd = gIcSrc[-1];
        if (static_cast<signed char>(cmd) < 0) {
            // ---- negative command ----
            if ((cmd & 0x40) == 0) {
                // skip run / end-of-sprite
                gIcX = X;
                gIcRow = reinterpret_cast<int>(row);
                gIcRun = cmd;
                if ((cmd & 0x3f) == 0)
                    return;
                X = X + (cmd & 0x3f);
                continue;
            }
            // 0xc0 - 0xff
            gIcRun = cmd;
            unsigned int count = cmd & 0x3f;
            int flags = 0;
            if (count != 0) {
                // 0xc1 - 0xff : solid colour run
                if (cmd == 0xc1)
                    count = *gIcSrc++;
                gIcColor = *gIcSrc++;
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            flags = *gIcSrc++;
            count = flags & 3;
            if (count == 0)
                count = *gIcSrc++;
            gIcCnt2 = count;
            if (color != 0) {
                gIcRun = flags;
                if (flags & 0x80) {
                    gIcCnt = count;
                    gIcColor = static_cast<unsigned char>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset(row + X, gIcColor, count);
            } else {
                int right;
                if (clipY <= gIcY && gIcClipB >= gIcY &&
                    (right = X + count, clipX < right) && gIcClipR >= X) {
                    if (clipX > X) {
                        unsigned int cn = clipW;
                        if (right <= gIcClipR)
                            cn = (count - clipX) + X;
                        memset(row + clipX, gIcColor, cn);
                    } else {
                        unsigned int cn = count;
                        if (gIcClipR < right)
                            cn = (gIcClipR - X) + 1;
                        memset(row + X, gIcColor, cn);
                    }
                }
            }
            X = X + count;
            gIcRun = count;
            continue;
        do_dim:
            gIcCnt = count;
            gIcRun = flags;
            if (flags & 0x40) {
                unsigned int lvl = (flags & 0x3c) * 0x40;
                unsigned char *palette = reinterpret_cast<unsigned char *>(uDimPal) + lvl;
                if (clip == 0) {
                    unsigned char *dp = row + X;
                    gIcDimPal = palette;
                    gIcDimDst = dp;
                    gIcDimLen = 0;
                    if (static_cast<int>(count) > 0) {
                        gIcDimLen = count;
                        do {
                            int px = *dp++;
                            gIcDimDst = dp;
                            count--;
                            gIcDimPal = palette;
                            dp[-1] = palette[px];
                        } while (count != 0);
                    }
                } else {
                    gIcCnt = count;
                    gIcDimPal = palette;
                    int right;
                    if (clipY <= gIcY && gIcClipB >= gIcY &&
                        (right = X + count, clipX < right) && gIcClipR >= X) {
                        unsigned int cn;
                        unsigned char *dst;
                        if (clipX > X) {
                            gIcCnt = count;
                            if (gIcClipR < right)
                                cn = clipW;
                            else
                                cn = (count - clipX) + X;
                            dst = row + clipX;
                        } else {
                            cn = count;
                            if (gIcClipR < right)
                                cn = (gIcClipR - X) + 1;
                            dst = row + X;
                        }
                        gIcDimDst = dst;
                        gIcCnt = cn;
                        gIcDimPal = palette;
                        gIcDimLen = 0;
                        if (static_cast<int>(cn) > 0) {
                            gIcDimLen = cn;
                            do {
                                int px = *dst++;
                                gIcDimDst = dst;
                                cn--;
                                gIcDimPal = palette;
                                dst[-1] = palette[px];
                            } while (cn != 0);
                        }
                    }
                }
            }
            X = X + gIcCnt2;
            continue;
        }
        // ---- positive command : literal copy / newline ----
        gIcX = X;
        gIcRun = cmd;
        if (cmd != 0) {
            int right;
            if (clip == 0) {
                memcpy(row + X, gIcSrc, cmd);
            } else if (gIcY < clipY || gIcClipB < gIcY ||
                       (right = X + cmd, right <= clipX) || gIcClipR < X) {
                // fully clipped out
            } else if (clipX > X) {
                unsigned int cn = clipW;
                if (right <= gIcClipR)
                    cn = (cmd - clipX) + X;
                memcpy(row + clipX, gIcSrc + (clipX - X), cn);
            } else {
                unsigned int cn = cmd;
                if (gIcClipR < right)
                    cn = (gIcClipR - X) + 1;
                memcpy(row + X, gIcSrc, cn);
            }
            X = X + cmd;
            gIcSrc = gIcSrc + cmd;
            gIcRun = cmd;
            continue;
        }
        // newline
        X = gIcX0;
        gIcY = gIcY + 1;
        row = row + gIcPitch;
    }
}
