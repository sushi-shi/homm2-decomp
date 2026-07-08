// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\icon2bc.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon2bc.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x534ca8+ block.
static int gCTPitch;
static unsigned int gCTCnt;
static int gCTX;
static int gCTY;
static IconEntry *gCTEntry;
static int gCTRow;
static unsigned char *gCTDimPal;
static int gCTClipB;
static int gCTClipR;
static unsigned char *gCTSrcCopy;
static unsigned int gCTCnt2;
static unsigned char *gCTDst;
static unsigned char *gCTSrc;
static unsigned int gCTDimLen;
static int gCTX0;
static unsigned char gCTColor;
static unsigned int gCTRun;

// @early-stop
// Colour-table variant of IconToBitmap: literal pixels are remapped through param_12[] and solid-fill
// colours through param_12[*src]; the dim step is gated on param_13. Own 0x534ca8 scratch block. Full
// logic recovered; 0% for the shared Icon*2b* /O2 register-fusion codegen wall (Icon2b.cpp). Kept per breadth.
VA(0x004d32a0, 0x5af)
void IconToBitmapColorTable(class icon *srcIcon, class bitmap *dest, int x, int y,
                            int frame, int clip, int clipX, int clipY, int clipW, int clipH,
                            int color, unsigned char *colorTable, int dimGate)
{
    unsigned char bVar1;
    int iVar2, iVar3;
    unsigned int uVar4, uVar5, uVar6, uVar7;
    unsigned char *pbVar8;
    int *puVar9;
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    gCTEntry = &entries[frame];
    gCTSrc = reinterpret_cast<unsigned char *>(srcIcon->m_data) + gCTEntry->srcOffset;
    gCTX0 = x + gCTEntry->x;
    gCTPitch = dest->m_width;
    gCTY = gCTEntry->y + y;
    if (clip != 0) {
        if (gCTX0 < clipX || clipW + clipX < gCTEntry->w + gCTX0 || gCTY < clipY ||
            clipY + clipH < gCTEntry->h + gCTY) {
            clip = 1;
            gCTClipR = clipX - 1 + clipW;
            gCTClipB = clipY - 1 + clipH;
        } else {
            clip = 0;
        }
    }
    gCTRow = gCTPitch * gCTY + reinterpret_cast<int>(dest->m_pixels);
    gCTX = gCTX0;
    do {
        while (1) {
            while (1) {
                while (1) {
                    pbVar8 = gCTSrc;
                    gCTSrc = gCTSrc + 1;
                    bVar1 = *pbVar8;
                    gCTRun = bVar1;
                    if (static_cast<signed char>(bVar1) < 0)
                        break;
                    if (gCTRun == 0) {
                        gCTY = gCTY + 1;
                        gCTRow = gCTRow + gCTPitch;
                        gCTX = gCTX0;
                    } else {
                        uVar7 = gCTRun;
                        gCTSrcCopy = gCTSrc;
                        if (clip == 0) {
                            gCTDst = reinterpret_cast<unsigned char *>(gCTX + gCTRow);
                        } else if (gCTY < clipY || gCTClipB < gCTY ||
                                   (iVar2 = gCTX + gCTRun, iVar2 <= clipX || gCTClipR < gCTX)) {
                            uVar7 = 0;
                        } else if (gCTX < clipX) {
                            uVar7 = clipW;
                            if (iVar2 <= gCTClipR)
                                uVar7 = (gCTRun - clipX) + gCTX;
                            gCTDst = reinterpret_cast<unsigned char *>(clipX + gCTRow);
                            gCTSrcCopy = gCTSrc + (clipX - gCTX);
                        } else {
                            gCTDst = reinterpret_cast<unsigned char *>(gCTX + gCTRow);
                            if (gCTClipR < iVar2)
                                uVar7 = (gCTClipR - gCTX) + 1;
                        }
                        gCTCnt = uVar7;
                        if (uVar7 != 0 && (gCTCnt2 = 0, pbVar8 = gCTDst, uVar6 = uVar7,
                                           0 < static_cast<int>(uVar7))) {
                            do {
                                gCTCnt2 = uVar6;
                                bVar1 = *gCTSrcCopy;
                                gCTDst = pbVar8 + 1;
                                gCTSrcCopy = gCTSrcCopy + 1;
                                *pbVar8 = colorTable[bVar1];
                                uVar7 = uVar7 - 1;
                                pbVar8 = gCTDst;
                                uVar6 = gCTCnt2;
                            } while (uVar7 != 0);
                        }
                        gCTSrc = gCTSrc + gCTRun;
                        gCTX = gCTX + gCTRun;
                    }
                }
                if ((bVar1 & 0x40) != 0)
                    break;
                if ((bVar1 & 0x3f) == 0)
                    return;
                gCTX = gCTX + (bVar1 & 0x3f);
            }
            uVar7 = gCTRun & 0x3f;
            if ((bVar1 & 0x3f) != 0)
                break;
            bVar1 = *gCTSrc;
            uVar7 = bVar1 & 3;
            gCTSrc = pbVar8 + 2;
            if ((bVar1 & 3) == 0) {
                gCTSrc = pbVar8 + 3;
                uVar7 = pbVar8[2];
            }
            gCTCnt = uVar7;
            gCTDimLen = uVar7;
            if (color != 0 && (bVar1 & 0x80) != 0) {
                gCTColor = static_cast<unsigned char>(color);
                goto LAB_004d346a;
            }
            uVar6 = gCTCnt2;
            if ((bVar1 & 0x40) != 0) {
                gCTDimPal = reinterpret_cast<unsigned char *>(uDimPal) + (bVar1 & 0x3c) * 0x40;
                if (clip == 0) {
                    gCTDst = reinterpret_cast<unsigned char *>(gCTX + gCTRow);
                    gCTCnt2 = 0;
                    uVar6 = gCTCnt2;
                    gCTCnt2 = uVar7;
                    for (; uVar7 != 0; uVar7--) {
                        if (dimGate != 0)
                            *gCTDst = gCTDimPal[*gCTDst];
                        gCTDst = gCTDst + 1;
                        uVar6 = gCTCnt2;
                    }
                } else if (clipY <= gCTY && gCTY <= gCTClipB &&
                           (iVar2 = uVar7 + gCTX, clipX < iVar2 && gCTX <= gCTClipR)) {
                    if (gCTX < clipX) {
                        uVar5 = clipW;
                        iVar3 = clipX;
                        if (iVar2 <= gCTClipR)
                            uVar5 = (uVar7 - clipX) + gCTX;
                    } else {
                        uVar5 = uVar7;
                        iVar3 = gCTX;
                        if (gCTClipR < iVar2)
                            uVar5 = (gCTClipR - gCTX) + 1;
                    }
                    gCTDst = reinterpret_cast<unsigned char *>(iVar3 + gCTRow);
                    gCTCnt2 = 0;
                    gCTCnt = uVar5;
                    uVar6 = gCTCnt2;
                    gCTCnt2 = uVar5;
                    if (0 < static_cast<int>(uVar5)) {
                        do {
                            if (dimGate != 0)
                                *gCTDst = gCTDimPal[*gCTDst];
                            gCTDst = gCTDst + 1;
                            uVar5 = uVar5 - 1;
                            uVar6 = gCTCnt2;
                        } while (uVar5 != 0);
                    }
                }
            }
            gCTCnt2 = uVar6;
            gCTX = gCTX + gCTDimLen;
        }
        if (gCTRun == 0xc1) {
            uVar7 = *gCTSrc;
            gCTSrc = pbVar8 + 2;
        }
        gCTColor = colorTable[*gCTSrc];
        gCTSrc = gCTSrc + 1;
LAB_004d346a:
        uVar6 = uVar7;
        if (clip == 0) {
            uVar4 = gCTColor * 0x01010101;
            puVar9 = reinterpret_cast<int *>(gCTX + gCTRow);
            for (uVar5 = uVar7 >> 2; uVar5 != 0; uVar5--)
                *puVar9++ = uVar4;
LAB_004d35bf:
            for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6--)
                *reinterpret_cast<char *>(puVar9) = static_cast<char>(uVar4),
                puVar9 = reinterpret_cast<int *>(reinterpret_cast<char *>(puVar9) + 1);
        } else if (clipY <= gCTY && gCTY <= gCTClipB &&
                   (iVar2 = uVar7 + gCTX, clipX < iVar2) && gCTX <= gCTClipR) {
            if (gCTX < clipX) {
                if (gCTClipR < iVar2) {
                    uVar4 = gCTColor * 0x01010101;
                    puVar9 = reinterpret_cast<int *>(clipX + gCTRow);
                    for (uVar5 = static_cast<unsigned int>(clipW) >> 2; uVar6 = clipW, uVar5 != 0; uVar5--)
                        *puVar9++ = uVar4;
                } else {
                    uVar6 = (gCTX - clipX) + uVar7;
                    uVar4 = gCTColor * 0x01010101;
                    puVar9 = reinterpret_cast<int *>(clipX + gCTRow);
                    for (uVar5 = uVar6 >> 2; uVar5 != 0; uVar5--)
                        *puVar9++ = uVar4;
                }
            } else if (gCTClipR < iVar2) {
                uVar6 = (gCTClipR - gCTX) + 1;
                uVar4 = gCTColor * 0x01010101;
                puVar9 = reinterpret_cast<int *>(gCTX + gCTRow);
                for (uVar5 = uVar6 >> 2; uVar5 != 0; uVar5--)
                    *puVar9++ = uVar4;
            } else {
                uVar4 = gCTColor * 0x01010101;
                puVar9 = reinterpret_cast<int *>(gCTX + gCTRow);
                for (uVar5 = uVar7 >> 2; uVar5 != 0; uVar5--)
                    *puVar9++ = uVar4;
            }
            goto LAB_004d35bf;
        }
        gCTX = gCTX + uVar7;
    } while (1);
}
