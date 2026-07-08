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
int gIcRow;
int gIcPitch;
unsigned char gIcColor;
unsigned char * gIcDimPal;
unsigned int gIcRun;
unsigned int gIcCnt;
unsigned char * gIcSrc;
unsigned char * gIcDimDst;
int gIcClipR;
int gIcClipB;
int gIcX0;
unsigned int gIcDimLen;
int gIcY;
int gIcX;
unsigned char * gIcEntry;
unsigned int gIcCnt2;

// @early-stop
// Full RLE sprite decoder recovered + compiles, but /O2 codegen is ~2x-3x retail -> 0%.
// INVESTIGATED (findings for the next codegen pass; unlocks the 11-TU Icon*2b* family — all share this):
//   1. The scratch state IS global (retail stores to 0x534c20+ via DIR32 relocs — the delinker just
//      NAMES them ?_C@_0..@ string-constant-style, not ?gIc*). So keep them file-static globals here;
//      making them locals is WRONG (the /O2 DCE elides the dead stores -> even further off). Confirmed
//      via `llvm-objdump -r build/delink/BASE/Icon2b.c.obj` (relocs: ?_C@... + ?uDimPal, no ?gIc*).
//   2. The gap is the ENTRY ACCESS + loop scheduling: retail keeps the bitmap base (esi) and 13*index
//      (ebx=lea [eax+4*(eax*3)]) in SEPARATE registers and reads fields via SIB `[ebx+esi+off]`; mine
//      materialises `base + index*0xd` into one reg (add esi,ebx) and reads `[esi+off]`.
//      TRIED: modelled the table as a packed IconEntry[] (Misc.h) + index it (entries[i].member) —
//      correct model + cleaner (kept), BUT /O2 STILL fuses base+13*i into one reg (it CSEs &entries[i]
//      for the gIcEntry store), so instr count is unchanged (505 vs 161). The wall is the optimizer's
//      register allocation across the whole decoder loop, not source-steerable via the struct alone;
//      needs a codegen-level pass (maybe: don't store gIcEntry, or access fields off gIcSrc/base+idx
//      without a shared pointer temp). Handed off with the diff isolated.
VA(0x004d0570, 0x4ed)
void IconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                  int clip, int clipX, int clipY, int clipW, int clipH, int color)
{
    unsigned char bVar2;
    int iVar3, iVar4;
    unsigned int uVar6, uVar7, uVar8;
    unsigned char *pbVar9, *pbVar11;
    unsigned char *ppuVar1;
    unsigned int uVar5;
    int *puVar10;
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    gIcEntry = reinterpret_cast<unsigned char *>(&entries[frame]);
    gIcSrc = reinterpret_cast<unsigned char *>(srcIcon->m_data) + entries[frame].srcOffset;
    gIcX0 = x + entries[frame].x;
    gIcY = entries[frame].y + y;
    gIcPitch = dest->m_width;
    if (clip != 0) {
        if (gIcX0 < clipX || clipW + clipX < entries[frame].w + gIcX0 ||
            gIcY < clipY || clipY + clipH < entries[frame].h + gIcY) {
            clip = 1;
            gIcClipR = clipX - 1 + clipW;
            gIcClipB = clipY - 1 + clipH;
        } else {
            clip = 0;
        }
    }
    gIcRow = gIcPitch * gIcY + reinterpret_cast<int>(dest->m_pixels);
    gIcX = gIcX0;
    while (1) {
        pbVar9 = gIcSrc;
        gIcSrc = gIcSrc + 1;
        bVar2 = *pbVar9;
        gIcRun = bVar2;
        if (static_cast<signed char>(bVar2) >= 0) {
            // copy or newline run (command 0x00-0x7f)
            if (gIcRun == 0) {
                gIcY = gIcY + 1;
                gIcRow = gIcRow + gIcPitch;
                gIcX = gIcX0;
            } else {
                uVar8 = gIcRun;
                pbVar9 = gIcSrc;
                int doCopy = 0;
                if (clip == 0) {
                    pbVar11 = reinterpret_cast<unsigned char *>(gIcX + gIcRow);
                    doCopy = 1;
                } else if (clipY <= gIcY && gIcY <= gIcClipB &&
                           (iVar3 = gIcX + gIcRun, clipX < iVar3 && gIcX <= gIcClipR)) {
                    if (gIcX < clipX) {
                        uVar8 = clipW;
                        if (iVar3 <= gIcClipR)
                            uVar8 = (gIcRun - clipX) + gIcX;
                        pbVar11 = reinterpret_cast<unsigned char *>(clipX + gIcRow);
                        pbVar9 = gIcSrc + (clipX - gIcX);
                    } else if (gIcClipR < iVar3) {
                        pbVar11 = reinterpret_cast<unsigned char *>(gIcX + gIcRow);
                        uVar8 = (gIcClipR - gIcX) + 1;
                    } else {
                        pbVar11 = reinterpret_cast<unsigned char *>(gIcX + gIcRow);
                    }
                    doCopy = 1;
                }
                if (doCopy) {
                    memcpy(pbVar11, pbVar9, uVar8);
                }
                gIcSrc = gIcSrc + gIcRun;
                gIcX = gIcX + gIcRun;
            }
        } else if ((bVar2 & 0x40) == 0) {
            // skip run or end-of-sprite (command 0x80-0xbf)
            if ((bVar2 & 0x3f) == 0)
                return;
            gIcX = gIcX + (bVar2 & 0x3f);
        } else {
            // dim or color run (command 0xc0-0xff)
            uVar8 = gIcRun & 0x3f;
            int doColorFill = 0;
            if ((bVar2 & 0x3f) == 0) {
                // extended-length command
                bVar2 = *gIcSrc;
                uVar8 = bVar2 & 3;
                gIcSrc = pbVar9 + 2;
                if ((bVar2 & 3) == 0) {
                    gIcSrc = pbVar9 + 3;
                    uVar8 = pbVar9[2];
                }
                gIcDimLen = uVar8;
                gIcCnt2 = uVar8;
                if (color != 0 && (bVar2 & 0x80) != 0) {
                    gIcColor = static_cast<unsigned char>(color);
                    doColorFill = 1;
                } else {
                    uVar7 = gIcCnt;
                    if ((bVar2 & 0x40) != 0) {
                        ppuVar1 = reinterpret_cast<unsigned char *>(uDimPal) + (bVar2 & 0x3c) * 0x40;
                        gIcDimPal = ppuVar1;
                        if (clip == 0) {
                            gIcDimDst = reinterpret_cast<unsigned char *>(gIcX + gIcRow);
                            gIcCnt = uVar8;
                            pbVar9 = gIcDimDst;
                            for (; uVar8 != 0; uVar8--) {
                                gIcDimPal = ppuVar1;
                                gIcDimDst = pbVar9 + 1;
                                *pbVar9 = ppuVar1[*pbVar9];
                                pbVar9++;
                                uVar7 = gIcCnt;
                            }
                        } else if (clipY <= gIcY && gIcY <= gIcClipB &&
                                   (iVar3 = gIcX + uVar8, clipX < iVar3 && gIcX <= gIcClipR)) {
                            if (gIcX < clipX) {
                                uVar6 = clipW;
                                iVar4 = clipX;
                                if (iVar3 <= gIcClipR)
                                    uVar6 = (uVar8 - clipX) + gIcX;
                            } else {
                                uVar6 = uVar8;
                                iVar4 = gIcX;
                                if (gIcClipR < iVar3)
                                    uVar6 = (gIcClipR - gIcX) + 1;
                            }
                            pbVar9 = reinterpret_cast<unsigned char *>(iVar4 + gIcRow);
                            gIcCnt = uVar6;
                            gIcDimDst = pbVar9;
                            gIcCnt2 = uVar6;
                            for (; uVar6 != 0; uVar6--) {
                                gIcDimPal = ppuVar1;
                                gIcDimDst = pbVar9 + 1;
                                *pbVar9 = ppuVar1[*pbVar9];
                                pbVar9++;
                                uVar7 = gIcCnt;
                            }
                        }
                        gIcCnt = uVar7;
                        gIcX = gIcX + gIcDimLen;
                    }
                }
            } else {
                // short color run (command 0xc1-0xff)
                if (gIcRun == 0xc1) {
                    uVar8 = *gIcSrc;
                    gIcSrc = pbVar9 + 2;
                }
                gIcColor = *gIcSrc;
                gIcSrc = gIcSrc + 1;
                doColorFill = 1;
            }
            if (doColorFill) {
                unsigned char *fillDst;
                unsigned int fillCount = uVar8;
                int didFill = 0;
                if (clip == 0) {
                    fillDst = reinterpret_cast<unsigned char *>(gIcX + gIcRow);
                    didFill = 1;
                } else if (clipY <= gIcY && gIcY <= gIcClipB &&
                           (iVar3 = gIcX + uVar8, clipX < iVar3) && gIcX <= gIcClipR) {
                    if (gIcX < clipX) {
                        fillCount = clipW;
                        if (iVar3 <= gIcClipR)
                            fillCount = (uVar8 - clipX) + gIcX;
                        fillDst = reinterpret_cast<unsigned char *>(clipX + gIcRow);
                    } else if (gIcClipR < iVar3) {
                        fillCount = (gIcClipR - gIcX) + 1;
                        fillDst = reinterpret_cast<unsigned char *>(gIcX + gIcRow);
                    } else {
                        fillDst = reinterpret_cast<unsigned char *>(gIcX + gIcRow);
                    }
                    didFill = 1;
                }
                if (didFill) {
                    memset(fillDst, gIcColor, fillCount);
                }
                gIcX = gIcX + uVar8;
            }
        }
    }
}

