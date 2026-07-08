// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconm2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconm2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x534bcc+ block.
static int gMonoClipR;
static int gMonoRow;
static IconEntry *gMonoEntry;
static unsigned char *gMonoSrc;
static int gMonoX0;
static unsigned int gMonoRun;
static int gMonoY;
static int gMonoClipB;
static int gMonoX;

// @early-stop
// Mono variant of IconToBitmap: fills each RLE run with a single colour (param_6) — a run-length mono
// silhouette (no per-pixel source data, no dim). Full logic recovered; 0% for the shared Icon*2b* /O2
// register-fusion codegen wall documented in Icon2b.cpp. Kept per breadth mandate.
VA(0x004cfae0, 0x266)
void MonoIconToBitmap(class icon *param_1, class bitmap *param_2, int param_3, int param_4, int param_5,
                      int param_6, int param_7, int param_8, int param_9, int param_10, int param_11)
{
    unsigned char bVar1;
    unsigned char *pbVar3;
    unsigned int uVar4;
    int iVar5, iVar9;
    unsigned int uVar6, uVar7;
    unsigned char uVar8;
    int *puVar10;
    IconEntry *entries = reinterpret_cast<IconEntry *>(param_1->field_0x12);
    gMonoEntry = &entries[param_5];
    gMonoSrc = reinterpret_cast<unsigned char *>(param_1->field_0x12) + gMonoEntry->srcOffset;
    gMonoX0 = param_3 + gMonoEntry->x;
    gMonoY = param_4 + gMonoEntry->y;
    if (param_7 != 0) {
        if (gMonoX0 < param_8 || param_10 + param_8 < gMonoEntry->w + gMonoX0 || gMonoY < param_9 ||
            param_9 + param_11 < gMonoEntry->h + gMonoY) {
            param_7 = 1;
            gMonoClipR = param_8 - 1 + param_10;
            gMonoClipB = param_9 - 1 + param_11;
        } else {
            param_7 = 0;
        }
    }
    short sVar2 = param_2->width;
    gMonoRow = gMonoY * sVar2 + reinterpret_cast<int>(param_2->pixels);
    gMonoX = gMonoX0;
    do {
        while (1) {
            while (1) {
                pbVar3 = gMonoSrc;
                gMonoSrc = gMonoSrc + 1;
                bVar1 = *pbVar3;
                gMonoRun = bVar1;
                if (-1 < static_cast<signed char>(bVar1))
                    break;
                if ((bVar1 & 0x7f) == 0)
                    return;
                gMonoX = gMonoX + (bVar1 & 0x7f);
            }
            if (gMonoRun != 0)
                break;
            gMonoRow = gMonoRow + sVar2;
            gMonoY = gMonoY + 1;
            gMonoX = gMonoX0;
        }
        uVar8 = static_cast<unsigned char>(param_6);
        uVar7 = gMonoRun;
        iVar9 = gMonoX;
        if (param_7 == 0) {
            uVar4 = uVar8 * 0x01010101;
LAB_004cfcf3:
            puVar10 = reinterpret_cast<int *>(iVar9 + gMonoRow);
            for (uVar6 = uVar7 >> 2; uVar6 != 0; uVar6--)
                *puVar10++ = uVar4;
            for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7--)
                *reinterpret_cast<char *>(puVar10) = static_cast<char>(uVar4),
                puVar10 = reinterpret_cast<int *>(reinterpret_cast<char *>(puVar10) + 1);
        } else if (param_9 <= gMonoY && gMonoY <= gMonoClipB &&
                   (iVar5 = gMonoX + gMonoRun, param_8 < iVar5 && gMonoX <= gMonoClipR)) {
            if (gMonoX < param_8) {
                uVar7 = param_10;
                if (iVar5 <= gMonoClipR)
                    uVar7 = (gMonoRun - param_8) + gMonoX;
                uVar4 = uVar8 * 0x01010101;
                iVar9 = param_8;
            } else if (gMonoClipR < iVar5) {
                uVar7 = (gMonoClipR - gMonoX) + 1;
                uVar4 = uVar8 * 0x01010101;
            } else {
                uVar4 = uVar8 * 0x01010101;
            }
            goto LAB_004cfcf3;
        }
        gMonoX = gMonoX + gMonoRun;
    } while (1);
}
