// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/IconEntry.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block (0x534c60+).
DATA(0x00534c78) static IconEntry *gFlipEntry;
DATA(0x00534c68) static int gFlipX0;
DATA(0x00534ca4) static int gFlipXEnd;
DATA(0x00534c74) static int gFlipY;
DATA(0x00534c7c) static int gFlipX;
DATA(0x00534c80) static unsigned char *gFlipSrc;
DATA(0x00534c9c) static int gFlipClipR;
DATA(0x00534c8c) static int gFlipClipB;
DATA(0x00534c90) static int gFlipRow;
DATA(0x00534c64) static unsigned int gFlipRun;
DATA(0x00534c98) static unsigned char gFlipColor;
DATA(0x00534c94) static unsigned int gFlipCnt2;
DATA(0x00534c70) static unsigned int gFlipDimLen;
DATA(0x00534c84) static unsigned char *gFlipDimPal;
DATA(0x00534c6c) static unsigned int gFlipCnt;
DATA(0x00534c88) static unsigned char *gFlipDimDst;
DATA(0x00534ca0) static unsigned char *gFlipDst;
DATA(0x00534c60) static int gFlipSkip;

// @match-note
// 214bd52 fresh structural pass: complete CFG, sub esp,8 frame, and all external relocation
// targets; candidate .text remains 0x4e5 versus retail 0x4f1, with 82/81 relocations. The sole
// count excess is the setup gFlipY reload (9/8): retail keeps the +0x73 load through the fourth
// clip clause, while this build reloads it at +0x89. The first unmasked divergence remains +0x1d:
// candidate loads entry Y before forming the entry pointer; retail forms EDI, loads/subtracts
// entry X, then loads entry Y. The decoder begins at +0xe9 versus +0xe3.
// A retail/adjacent-decoder-evidenced scratch declaration sequence raised 85.63395 to 86.5756;
// the literal path now has the real shared nonzero-skip publication and split right-arm skip
// lifetime, but VC4.2 still keeps that lifetime in ECX where retail spills three intermediates
// through [esp+0x10]. Mutating parameter x regressed, while clause-local Y caching and split cursor
// loads were byte-identical. This is not a wall or permitted stop. Continue from the setup Y/X
// allocation and right-skip spill after a newly proved lifetime/header/compiler-state change;
// do not replay either older matrix or iconf2b-fresh-214bd52.tsv.
// The 032a2b3 retry exhausted a new typed-alias/right-skip ownership family. Whole-entry
// references, a separate right-arm temporary, semantic skip-name buckets, cn/skip declaration
// order, setup-width slot reuse, compound updates, and the new icon2bc decoder-lifetime
// declaration surface were byte-identical. A field-Y reference regressed to 85.52%; volatile
// diagnostics did force memory ownership but regressed to 81.44%/80.44%, so they are not source
// evidence and were discarded. See iconf2b-fresh-032a2b3.tsv; revisit only with a new real
// aliasing/declaration surface that explains the retail spill without volatile semantics.
// The 8116876 sibling-template differential aligned the retail decoder with iconf2bc/iconf2by
// and recovered the exact scratch owner VAs: the earlier DATA annotations had assigned most
// semantic roles to adjacent four-byte cells. The shared clipped-fill current-Y lifetime and
// clipped-literal copy-count snapshot are retained, although VC4.2 emits the same 0x4e5 bytes.
// Nested/early-reject gates were also byte-identical; a shared selected-destination lifetime
// regressed to 85.78% and was discarded. See iconf2b-template-8116876.tsv. The remaining setup
// and [esp+0x10] spill residual is still unresolved and is not a wall.
VA(0x004d1ba0, 0x4f1)
void FlipIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                      int clip, int clipX, int clipY, int clipW, int clipH, int color)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    IconEntry *entry = &entries[frame];
    int x0 = x;
    int w = entry->w;
    unsigned char *src = data + entry->srcOffset;
    int entryY = entry->y;
    x0 = x0 - entry->x;
    x0 = x0 - w;
    gFlipEntry = entry;
    x0++;
    gFlipX0 = x0;
    int X = w + x0 - 1;
    gFlipXEnd = X;
    gFlipY = y + entryY;
    if (clip != 0) {
        if (gFlipX0 < clipX || clipW + clipX < gFlipX0 + w || gFlipY < clipY ||
            clipY + clipH < entry->h + gFlipY) {
            clip = 1;
            gFlipClipR = clipX + clipW - 1;
            gFlipClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    short pitch = dest->m_width;
    gFlipRow = gFlipY * pitch + reinterpret_cast<int>(dest->m_pixels);
    for (;;) {
        int cmd = *src++;
        if (static_cast<signed char>(cmd) < 0) {
            if ((cmd & 0x40) == 0) {
                // skip run / end-of-sprite
                gFlipRun = cmd;
                int n = cmd & 0x3f;
                gFlipX = X;
                gFlipSrc = src;
                if (n == 0)
                    return;
                X = X - n;
                continue;
            }
            // 0xc0 - 0xff
            gFlipRun = cmd;
            unsigned int count = cmd & 0x3f;
            int flags = 0;
            if (count != 0) {
                // 0xc1 - 0xff : solid colour run
                if (cmd == 0xc1)
                    count = *src++;
                gFlipColor = *src++;
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            flags = *src++;
            count = flags & 3;
            if (count == 0)
                count = *src++;
            gFlipCnt2 = count;
            if (color != 0) {
                gFlipRun = flags;
                if (flags & 0x80) {
                    gFlipDimLen = count;
                    gFlipColor = static_cast<unsigned char>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset(reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X), gFlipColor,
                       count);
            } else {
                int currentY = gFlipY;
                int left;
                if (currentY >= clipY && currentY <= gFlipClipB &&
                    (left = (X - count) + 1, clipX <= left) && X <= gFlipClipR) {
                    if (clipX <= left) {
                        memset(reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X),
                               gFlipColor, count);
                    } else {
                        memset(reinterpret_cast<unsigned char *>(gFlipRow + clipX), gFlipColor,
                               (X - clipX) + 1);
                    }
                }
            }
            X = X - count;
            gFlipRun = count;
            continue;
        do_dim:
            gFlipRun = flags;
            gFlipDimLen = count;
            if (flags & 0x40) {
                unsigned char *palette =
                    reinterpret_cast<unsigned char *>(uDimPal) + (flags & 0x3c) * 0x40;
                gFlipDimPal = palette;
                if (clip == 0) {
                    unsigned char *dp =
                        reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X);
                    gFlipCnt = 0;
                    int dimCount = count;
                    gFlipDimDst = dp;
                    if (dimCount > 0) {
                        gFlipCnt = dimCount;
                        do {
                            unsigned char *dimPalette = gFlipDimPal;
                            int px = *dp++;
                            count--;
                            gFlipDimDst = dp;
                            dp[-1] = dimPalette[px];
                        } while (count != 0);
                    }
                } else {
                    const int currentY = gFlipY;
                    gFlipDimLen = count;
                    if (clipY <= currentY && currentY <= gFlipClipB &&
                        clipX <= static_cast<int>((X - count) + 1) && X <= gFlipClipR) {
                        int left = (X - count) + 1;
                        unsigned char *dp;
                        if (clipX <= left) {
                            dp = reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X);
                        } else {
                            count = (X - clipX) + 1;
                            dp = reinterpret_cast<unsigned char *>(gFlipRow + clipX);
                        }
                        int dimCount = count;
                        gFlipDimLen = count;
                        gFlipDimDst = dp;
                        gFlipCnt = 0;
                        if (dimCount > 0) {
                            gFlipCnt = dimCount;
                            do {
                                unsigned char *dimPalette = gFlipDimPal;
                                int px = *dp++;
                                count--;
                                gFlipDimDst = dp;
                                dp[-1] = dimPalette[px];
                            } while (count != 0);
                        }
                    }
                }
            }
            X = X - gFlipCnt2;
            continue;
        }
        // ---- positive command : literal copy / newline ----
        gFlipRun = cmd;
        gFlipX = X;
        if (cmd != 0) {
            if (clip == 0) {
                gFlipCnt = 0;
                unsigned char *dst = reinterpret_cast<unsigned char *>(gFlipRow + X);
                gFlipDst = dst;
                if (cmd > 0) {
                    gFlipCnt = cmd;
                    int k = cmd;
                    do {
                        unsigned char c = *src++;
                        *dst-- = c;
                        gFlipDst = dst;
                        k--;
                    } while (k != 0);
                }
            } else {
                int currentY = gFlipY;
                if (currentY >= clipY && currentY <= gFlipClipB) {
                    int left = (X - cmd) + 1;
                    if (left <= gFlipClipR && clipX <= X) {
                        unsigned int cn;
                        int skip;
                        if (X <= gFlipClipR) {
                            gFlipDst = reinterpret_cast<unsigned char *>(gFlipRow + X);
                            if (clipX <= left) {
                                gFlipSkip = 0;
                                cn = cmd;
                            } else {
                                cn = (X - clipX) + 1;
                                skip = cmd - cn;
                                goto set_skip;
                            }
                        } else {
                            cn = gFlipClipR;
                            src = src + (X - cn);
                            gFlipDst =
                                reinterpret_cast<unsigned char *>(gFlipRow + cn);
                            if (clipX <= (X - cmd)) {
                                gFlipSkip = 0;
                                cn = (cmd - X) + cn;
                            } else {
                                cn = clipW;
                                skip = cmd;
                                skip = skip - X;
                                skip = skip - clipW;
                                skip = gFlipClipR + skip;
                                goto set_skip;
                            }
                        }
                        goto skip_set;
                    set_skip:
                        gFlipSkip = skip;
                    skip_set:
                        int copyCount = cn;
                        gFlipDimLen = cn;
                        gFlipCnt = 0;
                        if (copyCount > 0) {
                            gFlipCnt = copyCount;
                            do {
                                unsigned char c = *src++;
                                (gFlipDst = gFlipDst - 1)[1] = c;
                                cn--;
                            } while (cn != 0);
                        }
                        src = src + gFlipSkip;
                    } else {
                        src = src + cmd;
                    }
                } else {
                    src = src + cmd;
                }
            }
            X = X - cmd;
            gFlipRun = cmd;
            continue;
        }
        // newline
        X = gFlipXEnd;
        gFlipY = gFlipY + 1;
        gFlipRow = gFlipRow + pitch;
    }
}
