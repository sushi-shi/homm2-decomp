// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icondf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icondf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x5381b8+ block.
static int gFDX0;
static unsigned int gFDCnt;
static int gFDXEnd;
static unsigned char *gFDDst;
static unsigned int gFDCnt2;
static int gFDRow;
static IconEntry *gFDEntry;
static int gFDClipR;
static int gFDX;
static int gFDClipB;
static unsigned char *gFDSrc;
static int gFDY;
static unsigned int gFDRun;

// @early-stop
// Flip+Dim variant of IconToBitmap: dims the screen pixels (uDimPal[param_6]) in the flipped run
// position ((row-run)+1+X, X decrements). Full logic recovered; 0% for the shared Icon*2b* /O2
// register-fusion codegen wall documented in Icon2b.cpp. Kept per breadth mandate.
VA(0x004daa20, 0x23b)
void FlipDimIconToBitmap(class icon *param_1, class bitmap *param_2, int param_3, int param_4,
                         int param_5, int param_6, int param_7, int param_8, int param_9, int param_10,
                         int param_11)
{
    unsigned char bVar1;
    int iVar3;
    unsigned int uVar4;
    unsigned char *pbVar5;
    IconEntry *entries = reinterpret_cast<IconEntry *>(param_1->field_0x12);
    gFDEntry = &entries[param_5];
    gFDSrc = reinterpret_cast<unsigned char *>(param_1->field_0x12) + gFDEntry->srcOffset;
    iVar3 = gFDEntry->w;
    gFDY = param_4 + gFDEntry->y;
    gFDX0 = ((param_3 - gFDEntry->x) - iVar3) + 1;
    gFDXEnd = iVar3 - 1 + gFDX0;
    if (param_7 != 0) {
        if (gFDX0 < param_8 || param_10 + param_8 < iVar3 + gFDX0 || gFDY < param_9 ||
            param_9 + param_11 < gFDEntry->h + gFDY) {
            param_7 = 1;
            gFDClipR = param_8 - 1 + param_10;
            gFDClipB = param_9 - 1 + param_11;
        } else {
            param_7 = 0;
        }
    }
    short sVar2 = param_2->field_0x12;
    gFDRow = gFDY * sVar2 + reinterpret_cast<int>(param_2->field_0x16);
    gFDX = gFDXEnd;
    while (1) {
        while (1) {
            pbVar5 = gFDSrc;
            gFDSrc = gFDSrc + 1;
            bVar1 = *pbVar5;
            gFDRun = bVar1;
            if (static_cast<signed char>(bVar1) < 0)
                break;
            if (gFDRun == 0) {
                gFDY = gFDY + 1;
                gFDRow = gFDRow + sVar2;
                gFDX = gFDXEnd;
            } else {
                if (param_7 == 0) {
                    gFDCnt = 0;
                    gFDDst = reinterpret_cast<unsigned char *>((gFDRow - gFDRun) + 1 + gFDX);
                    if (gFDRun != 0) {
                        uVar4 = gFDRun;
                        pbVar5 = gFDDst;
                        gFDCnt = gFDRun;
                        do {
                            uVar4 = uVar4 - 1;
                            gFDDst = pbVar5 + 1;
                            *pbVar5 = (reinterpret_cast<unsigned char *>(uDimPal) + param_6 * 0x100)[*pbVar5];
                            pbVar5 = pbVar5 + 1;
                        } while (uVar4 != 0);
                    }
                } else if (param_9 <= gFDY && gFDY <= gFDClipB &&
                           (iVar3 = (gFDX - gFDRun) + 1, param_8 <= iVar3 && gFDX <= gFDClipR)) {
                    if (iVar3 < param_8) {
                        gFDDst = reinterpret_cast<unsigned char *>(gFDRow + param_8);
                        uVar4 = (gFDX - param_8) + 1;
                    } else {
                        gFDDst = reinterpret_cast<unsigned char *>((gFDRow - gFDRun) + 1 + gFDX);
                        uVar4 = gFDRun;
                    }
                    gFDCnt = 0;
                    gFDCnt2 = uVar4;
                    if (0 < static_cast<int>(uVar4)) {
                        pbVar5 = gFDDst;
                        gFDCnt = uVar4;
                        do {
                            uVar4 = uVar4 - 1;
                            gFDDst = pbVar5 + 1;
                            *pbVar5 = (reinterpret_cast<unsigned char *>(uDimPal) + param_6 * 0x100)[*pbVar5];
                            pbVar5 = pbVar5 + 1;
                        } while (uVar4 != 0);
                    }
                }
                gFDX = gFDX - gFDRun;
            }
        }
        if ((bVar1 & 0x7f) == 0)
            break;
        gFDX = gFDX - (bVar1 & 0x7f);
    }
}
