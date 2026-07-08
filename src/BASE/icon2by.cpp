// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\icon2by.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon2by.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/Misc.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block.
static int gYMClipB;
static int gYMDimIdx;
static int gYMPitch;
static int gYMY;
static int gYMX;
static unsigned char *gYMDimDst;
static int gYMX0;
static unsigned int gYMRun;
static unsigned char *gYMDimPal;
static int gYMRow;
static unsigned char *gYMSrc;
static IconEntry *gYMEntry;
static unsigned int gYMDimLen;
static unsigned char gYMColor;
static unsigned int gYMDimLen2;
static int gYMClipR;

// @early-stop
// Y-shear variant of IconToBitmap: each scanline's X origin is offset by shear[Y] (0x7f = skip this
// line). Always clips (no fast path); X and row live in globals gYMX/gYMRow. Literal copy = memcpy,
// solid run = memset, dim = per-pixel uDimPal remap. Residual is the /O2 register-fusion wall.
VA(0x004da270, 0x588)
void IconToBitmapYModify(class icon *srcIcon, class bitmap *dest, int x, int y, int frame, int clip,
                         int clipX, int clipY, int clipW, int clipH, int color, signed char *shear)
{
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    gYMEntry = &entries[frame];
    gYMSrc = reinterpret_cast<unsigned char *>(srcIcon->m_data) + gYMEntry->srcOffset;
    gYMX0 = gYMEntry->x + x;
    gYMPitch = dest->m_width;
    gYMY = gYMEntry->y + y;
    gYMX = shear[gYMY] + gYMX0;
    gYMClipB = clipY + clipH - 1;
    gYMClipR = clipX + clipW - 1;
    gYMRow = gYMPitch * gYMY + reinterpret_cast<int>(dest->m_pixels);
    for (;;) {
        int cmd = *gYMSrc++;
        if (static_cast<signed char>(cmd) < 0) {
            gYMRun = cmd;
            if ((cmd & 0x40) == 0) {
                if ((cmd & 0x3f) == 0)
                    return;
                gYMX = gYMX + (cmd & 0x3f);
                continue;
            }
            // 0xc0 - 0xff
            unsigned int count = cmd & 0x3f;
            gYMRun = count;
            int flags = 0;
            if (count != 0) {
                if (cmd == 0xc1) {
                    gYMRun = *gYMSrc;
                    gYMSrc = gYMSrc + 1;
                }
                gYMColor = *gYMSrc;
                gYMSrc = gYMSrc + 1;
                goto do_fill;
            }
            flags = *gYMSrc;
            gYMSrc = gYMSrc + 1;
            gYMDimLen = flags & 3;
            if ((flags & 3) == 0) {
                gYMDimLen = *gYMSrc;
                gYMSrc = gYMSrc + 1;
            }
            gYMDimLen2 = gYMDimLen;
            if (color != 0 && (flags & 0x80) != 0) {
                gYMColor = static_cast<unsigned char>(color);
                gYMRun = gYMDimLen;
                goto do_fill;
            }
            if ((flags & 0x40) != 0) {
                gYMDimPal = reinterpret_cast<unsigned char *>(uDimPal) + (flags & 0x3c) * 0x40;
                if (shear[gYMY] != 0x7f && clipY <= gYMY && gYMY <= gYMClipB &&
                    (int)(gYMDimLen + gYMX) > clipX && gYMX <= gYMClipR) {
                    int right = gYMDimLen + gYMX;
                    int at;
                    if (gYMX < clipX) {
                        at = clipX;
                        if (gYMClipR < right)
                            gYMDimLen = clipW;
                        else
                            gYMDimLen = gYMDimLen + (gYMX - clipX);
                    } else {
                        at = gYMX;
                        if (gYMClipR < right)
                            gYMDimLen = (gYMClipR - gYMX) + 1;
                    }
                    gYMDimDst = reinterpret_cast<unsigned char *>(gYMRow + at);
                    gYMDimIdx = 0;
                    if (0 < static_cast<int>(gYMDimLen)) {
                        do {
                            *gYMDimDst = gYMDimPal[*gYMDimDst];
                            gYMDimDst = gYMDimDst + 1;
                            gYMDimIdx = gYMDimIdx + 1;
                        } while (gYMDimIdx < static_cast<int>(gYMDimLen));
                    }
                }
            }
            gYMX = gYMX + gYMDimLen2;
            continue;
        do_fill:
            if (shear[gYMY] != 0x7f && clipY <= gYMY && gYMY <= gYMClipB &&
                (int)(gYMX + gYMRun) > clipX && gYMX <= gYMClipR) {
                int right = gYMX + gYMRun;
                unsigned int cn;
                unsigned char *dst;
                if (gYMX < clipX) {
                    if (gYMClipR < right)
                        cn = clipW;
                    else
                        cn = (gYMRun - clipX) + gYMX;
                    dst = reinterpret_cast<unsigned char *>(gYMRow + clipX);
                } else {
                    if (gYMClipR < right)
                        cn = (gYMClipR - gYMX) + 1;
                    else
                        cn = gYMRun;
                    dst = reinterpret_cast<unsigned char *>(gYMRow + gYMX);
                }
                memset(dst, gYMColor, cn);
            }
            gYMX = gYMX + gYMRun;
            continue;
        }
        // ---- positive command : literal copy / newline ----
        gYMRun = cmd;
        if (cmd != 0) {
            if (shear[gYMY] != 0x7f && clipY <= gYMY && gYMY <= gYMClipB &&
                (int)(gYMX + cmd) > clipX && gYMX <= gYMClipR) {
                int right = gYMX + cmd;
                unsigned int cn;
                unsigned char *src2;
                unsigned char *dst;
                if (gYMX < clipX) {
                    if (gYMClipR < right)
                        cn = clipW;
                    else
                        cn = (cmd - clipX) + gYMX;
                    src2 = gYMSrc + (clipX - gYMX);
                    dst = reinterpret_cast<unsigned char *>(gYMRow + clipX);
                } else {
                    src2 = gYMSrc;
                    if (gYMClipR < right)
                        cn = (gYMClipR - gYMX) + 1;
                    else
                        cn = cmd;
                    dst = reinterpret_cast<unsigned char *>(gYMRow + gYMX);
                }
                memcpy(dst, src2, cn);
            }
            gYMX = gYMX + cmd;
            gYMSrc = gYMSrc + cmd;
            continue;
        }
        // newline
        gYMX = shear[gYMY] + gYMX0;
        gYMRow = gYMRow + gYMPitch;
        gYMY = gYMY + 1;
    }
}
