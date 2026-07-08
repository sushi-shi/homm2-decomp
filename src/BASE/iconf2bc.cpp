// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\iconf2bc.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconf2bc.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x5380c0+ block.
static unsigned char *gFCDst;
static unsigned int gFCDimLen;
static int gFCClipR;
static unsigned char gFCColor;
static int gFCX;
static int gFCSkip;
static int gFCY;
static unsigned char *gFCDimPal;
static unsigned int gFCCnt2;
static int gFCXEnd;
static unsigned int gFCCnt;
static IconEntry *gFCEntry;
static unsigned char *gFCSrc;
static unsigned char *gFCDimDst;
static int gFCX0;
static int gFCClipB;
static int gFCRow;
static unsigned int gFCRun;

// @early-stop
// Flip+ColorTable variant: horizontal-flip decoder (dst decrements) with literal pixels remapped through
// param_12[] and solid-fill colour = param_12[*src]. Own 0x5380c0 scratch block. Full logic recovered;
// 0% for the shared Icon*2b* /O2 register-fusion codegen wall (Icon2b.cpp). Kept per breadth mandate.
VA(0x004d9790, 0x54d)
void FlipIconToBitmapColorTable(class icon *param_1, class bitmap *param_2, int param_3, int param_4,
                                int param_5, int param_6, int param_7, int param_8, int param_9,
                                int param_10, int param_11, unsigned char *param_12)
{
    unsigned char bVar1;
    int iVar8;
    unsigned int uVar5, uVar7, uVar9, uVar10;
    unsigned char *pbVar11, *pbVar13;
    int *puVar12;
    IconEntry *entries = reinterpret_cast<IconEntry *>(param_1->field_0x12);
    gFCEntry = &entries[param_5];
    int iVar6 = gFCEntry->w;
    gFCX0 = ((param_3 - gFCEntry->x) - iVar6) + 1;
    gFCXEnd = iVar6 - 1 + gFCX0;
    gFCY = gFCEntry->y + param_4;
    if (param_6 != 0) {
        if (gFCX0 < param_7 || param_9 + param_7 < iVar6 + gFCX0 || gFCY < param_8 ||
            param_8 + param_10 < gFCEntry->h + gFCY) {
            param_6 = 1;
            gFCClipR = param_7 - 1 + param_9;
            gFCClipB = param_8 - 1 + param_10;
        } else {
            param_6 = 0;
        }
    }
    short sVar2 = param_2->m_width;
    gFCRow = gFCY * sVar2 + reinterpret_cast<int>(param_2->m_pixels);
    gFCX = gFCXEnd;
    pbVar13 = reinterpret_cast<unsigned char *>(param_1->field_0x12) + gFCEntry->srcOffset;
LAB_004d987c:
    do {
        while (1) {
            bVar1 = *pbVar13;
            gFCRun = bVar1;
            gFCSrc = pbVar13 + 1;
            if (static_cast<signed char>(bVar1) < 0)
                break;
            if (gFCRun == 0) {
                gFCY = gFCY + 1;
                gFCRow = gFCRow + sVar2;
                gFCX = gFCXEnd;
                pbVar13 = gFCSrc;
            } else {
                if (param_6 == 0) {
                    gFCCnt = 0;
                    gFCDst = reinterpret_cast<unsigned char *>(gFCRow + gFCX);
                    uVar10 = gFCCnt;
                    pbVar11 = gFCDst;
                    gFCCnt = gFCRun;
                    for (uVar9 = gFCRun; uVar9 != 0; uVar9--) {
                        gFCDst = pbVar11 + -1;
                        *pbVar11 = param_12[*gFCSrc];
                        gFCSrc = gFCSrc + 1;
                        uVar10 = gFCCnt;
                        pbVar11 = pbVar11 + -1;
                    }
                } else if (param_8 <= gFCY && gFCY <= gFCClipB) {
                    iVar8 = (gFCX - gFCRun) + 1;
                    if (iVar8 <= gFCClipR && param_7 <= gFCX) {
                        if (gFCClipR < gFCX) {
                            gFCDst = reinterpret_cast<unsigned char *>(gFCRow + gFCClipR);
                            gFCSrc = gFCSrc + (gFCX - gFCClipR);
                            if (static_cast<int>(gFCX - gFCRun) < param_7) {
                                gFCSkip = gFCClipR + ((gFCRun - gFCX) - param_9);
                                uVar10 = param_9;
                            } else {
                                gFCSkip = 0;
                                uVar10 = (gFCRun - gFCX) + gFCClipR;
                            }
                        } else {
                            gFCDst = reinterpret_cast<unsigned char *>(gFCRow + gFCX);
                            if (iVar8 < param_7) {
                                uVar10 = (gFCX - param_7) + 1;
                                gFCSkip = gFCRun - uVar10;
                            } else {
                                gFCSkip = 0;
                                uVar10 = gFCRun;
                            }
                        }
                        gFCCnt = 0;
                        gFCCnt2 = uVar10;
                        uVar9 = uVar10;
                        if (0 < static_cast<int>(uVar10)) {
                            do {
                                gFCCnt = uVar9;
                                bVar1 = *gFCSrc;
                                gFCSrc = gFCSrc + 1;
                                uVar10 = uVar10 - 1;
                                pbVar11 = gFCDst + -1;
                                *gFCDst = param_12[bVar1];
                                gFCDst = pbVar11;
                                uVar9 = gFCCnt;
                            } while (uVar10 != 0);
                        }
                        gFCSrc = gFCSrc + gFCSkip;
                        uVar10 = gFCCnt;
                        goto LAB_004d9c9f;
                    }
                    gFCSrc = gFCSrc + gFCRun;
                    uVar10 = gFCCnt;
                    goto LAB_004d9c9f;
                } else {
                    gFCSrc = gFCSrc + gFCRun;
                    uVar10 = gFCCnt;
                }
LAB_004d9c9f:
                gFCCnt = uVar10;
                gFCX = gFCX - gFCRun;
                pbVar13 = gFCSrc;
            }
        }
        if ((bVar1 & 0x40) == 0) {
            if ((bVar1 & 0x3f) == 0)
                return;
            gFCX = gFCX - (bVar1 & 0x3f);
            pbVar13 = gFCSrc;
            goto LAB_004d987c;
        }
        uVar10 = gFCRun & 0x3f;
        if ((bVar1 & 0x3f) == 0) {
            bVar1 = *gFCSrc;
            pbVar11 = pbVar13 + 2;
            uVar10 = bVar1 & 3;
            if ((bVar1 & 3) == 0) {
                uVar10 = *pbVar11;
                pbVar11 = pbVar13 + 3;
            }
            gFCDimLen = uVar10;
            gFCCnt2 = uVar10;
            if (param_11 != 0 && (bVar1 & 0x80) != 0) {
                gFCColor = static_cast<unsigned char>(param_11);
                goto LAB_004d9909;
            }
            uVar9 = gFCCnt;
            if ((bVar1 & 0x40) != 0) {
                gFCDimPal = reinterpret_cast<unsigned char *>(uDimPal) + (bVar1 & 0x3c) * 0x40;
                if (param_6 == 0) {
                    gFCCnt = 0;
                    gFCDimDst = reinterpret_cast<unsigned char *>((gFCRow - uVar10) + 1 + gFCX);
                    uVar9 = gFCCnt;
                    pbVar13 = gFCDimDst;
                    gFCCnt = uVar10;
                    for (; uVar10 != 0; uVar10--) {
                        gFCDimDst = pbVar13 + 1;
                        *pbVar13 = gFCDimPal[*pbVar13];
                        uVar9 = gFCCnt;
                        pbVar13 = pbVar13 + 1;
                    }
                } else if (param_8 <= gFCY && gFCY <= gFCClipB &&
                           (iVar8 = (gFCX - uVar10) + 1, param_7 <= iVar8 && gFCX <= gFCClipR)) {
                    if (iVar8 < param_7) {
                        uVar10 = (gFCX - param_7) + 1;
                        pbVar13 = reinterpret_cast<unsigned char *>(gFCRow + param_7);
                    } else {
                        pbVar13 = reinterpret_cast<unsigned char *>((gFCRow - uVar10) + 1 + gFCX);
                    }
                    gFCCnt = 0;
                    gFCCnt2 = uVar10;
                    uVar9 = gFCCnt;
                    gFCCnt = uVar10;
                    gFCDimDst = pbVar13;
                    if (0 < static_cast<int>(uVar10)) {
                        do {
                            uVar10 = uVar10 - 1;
                            gFCDimDst = pbVar13 + 1;
                            *pbVar13 = gFCDimPal[*pbVar13];
                            pbVar13 = pbVar13 + 1;
                            uVar9 = gFCCnt;
                        } while (uVar10 != 0);
                    }
                }
            }
            gFCCnt = uVar9;
            gFCX = gFCX - gFCDimLen;
            pbVar13 = pbVar11;
        } else {
            if (gFCRun == 0xc1) {
                uVar10 = *gFCSrc;
                gFCSrc = pbVar13 + 2;
            }
            pbVar11 = gFCSrc + 1;
            gFCColor = param_12[*gFCSrc];
LAB_004d9909:
            uVar9 = uVar10;
            if (param_6 == 0) {
                uVar5 = gFCColor * 0x01010101;
                puVar12 = reinterpret_cast<int *>((gFCRow - uVar10) + 1 + gFCX);
                for (uVar7 = uVar10 >> 2; uVar7 != 0; uVar7--)
                    *puVar12++ = uVar5;
LAB_004d99fa:
                for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9--)
                    *reinterpret_cast<char *>(puVar12) = static_cast<char>(uVar5),
                    puVar12 = reinterpret_cast<int *>(reinterpret_cast<char *>(puVar12) + 1);
            } else if (param_8 <= gFCY && gFCY <= gFCClipB &&
                       (iVar8 = (gFCX - uVar10) + 1, param_7 <= iVar8) && gFCX <= gFCClipR) {
                if (iVar8 < param_7) {
                    uVar9 = (gFCX - param_7) + 1;
                    uVar5 = gFCColor * 0x01010101;
                    puVar12 = reinterpret_cast<int *>(gFCRow + param_7);
                    for (uVar7 = uVar9 >> 2; uVar7 != 0; uVar7--)
                        *puVar12++ = uVar5;
                } else {
                    uVar5 = gFCColor * 0x01010101;
                    puVar12 = reinterpret_cast<int *>((gFCRow - uVar10) + 1 + gFCX);
                    for (uVar7 = uVar10 >> 2; uVar7 != 0; uVar7--)
                        *puVar12++ = uVar5;
                }
                goto LAB_004d99fa;
            }
            gFCX = gFCX - uVar10;
            pbVar13 = pbVar11;
        }
    } while (1);
}
