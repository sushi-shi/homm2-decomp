// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Blur.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Blur.h>
#include <BASE/bitmap.h>
#include <BASE/palette.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>
VA(0x004d28e0, 0x6a4)
void DoBlur(class bitmap *param_1, class bitmap *param_2, int param_3, int param_4, int param_5, int param_6)
{
    int local_c00[0x100];
    int auStack_800[0x100];
    int auStack_400[0x100];
    unsigned char bVar1, bVar2, bVar3, bVar4, bVar5, bVar6, bVar7, bVar8;
    unsigned char bVar9, bVar10, bVar11, bVar12, bVar13, bVar14, bVar15, bVar16;
    int i, x, y;
    PollSound();
    gpMouseManager->HideColorPointer();
    gpWindowManager->SaveFizzleSource(0, 0, 0x280, param_3);
    bitmap *work = new bitmap(0, 0x280, param_3);
    unsigned int nbytes = param_3 * 0x280;
    memcpy(work->pixels, param_2->pixels, nbytes);
    char *pcVar18 = static_cast<char *>(BaseAlloc(0x8000, __FILE__, __LINE__));
    unsigned char *pbVar26 = reinterpret_cast<unsigned char *>(gpBufferPalette->field_0x10);
    for (i = 0; i < 0x100; i++) {
        local_c00[i] = pbVar26[0];
        auStack_800[i] = pbVar26[1];
        auStack_400[i] = pbVar26[2];
        pbVar26 += 3;
    }
    unsigned long id = gpResourceManager->MakeId("RGBLOOKP.BIN", 1);
    gpResourceManager->PointToFile(id);
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(pcVar18), 0x8000);
    memcpy(param_1->pixels, param_2->pixels, nbytes);
    PollSound();
    for (y = 4; y < param_3 - 4; y++) {
        if ((y & 0x3f) == 0x3f)
            PollSound();
        unsigned char *iVar25 = param_1->pixels + 4 + y * 0x280;
        char *dstRow = reinterpret_cast<char *>(param_2->pixels) + 4 + y * 0x280;
        for (x = 0x278; x != 0; x--) {
            bVar1 = iVar25[0x280];
            bVar2 = iVar25[-0xa00];
            bVar3 = iVar25[0x500];
            bVar4 = iVar25[-0x500];
            bVar5 = iVar25[0x1];
            bVar6 = iVar25[-0x1];
            bVar7 = iVar25[0x2];
            bVar8 = iVar25[0x780];
            bVar9 = iVar25[-0x2];
            bVar10 = iVar25[-0x780];
            bVar11 = iVar25[0x3];
            bVar12 = iVar25[0x4];
            bVar13 = iVar25[-0x3];
            bVar14 = iVar25[-0x4];
            bVar15 = iVar25[-0x280];
            bVar16 = iVar25[0xa00];
            iVar25++;
            *dstRow = pcVar18[
                (((local_c00[bVar2] + local_c00[bVar1] + local_c00[bVar3] + local_c00[bVar4] + local_c00[bVar5] + local_c00[bVar6] + local_c00[bVar7] + local_c00[bVar8] + local_c00[bVar9] + local_c00[bVar10] + local_c00[bVar11] + local_c00[bVar12] + local_c00[bVar13] + local_c00[bVar14] + local_c00[bVar15] + local_c00[bVar16]) & 0xffffffe0) * 0x20)
                + (((auStack_800[bVar1] + auStack_800[bVar16] + auStack_800[bVar15] + auStack_800[bVar3] + auStack_800[bVar4] + auStack_800[bVar5] + auStack_800[bVar6] + auStack_800[bVar7] + auStack_800[bVar8] + auStack_800[bVar9] + auStack_800[bVar10] + auStack_800[bVar11] + auStack_800[bVar12] + auStack_800[bVar13] + auStack_800[bVar14] + auStack_800[bVar2]) & 0xffffffe0))
                + (((auStack_400[bVar1] + auStack_400[bVar15] + auStack_400[bVar16] + auStack_400[bVar2] + auStack_400[bVar3] + auStack_400[bVar4] + auStack_400[bVar5] + auStack_400[bVar6] + auStack_400[bVar7] + auStack_400[bVar8] + auStack_400[bVar9] + auStack_400[bVar10] + auStack_400[bVar11] + auStack_400[bVar12] + auStack_400[bVar13] + auStack_400[bVar14])) >> 5)];
            dstRow++;
        }
    }
    PollSound();
    char *pcVar21 = static_cast<char *>(BaseAlloc(0x300, __FILE__, __LINE__));
    char *pcVar22 = static_cast<char *>(BaseAlloc(0x300, __FILE__, __LINE__));
    memcpy(pcVar21, gPalette->field_0x10, 0x300);
    char *pcVar23 = pcVar21;
    char *pcVar27 = pcVar22;
    for (i = 0x100; i != 0; i--) {
        pcVar27[0] = pcVar23[0] + static_cast<char>(param_4);
        pcVar27[1] = pcVar23[1] + static_cast<char>(param_5);
        pcVar27[2] = pcVar23[2] + static_cast<char>(param_6);
        if (pcVar27[0] > 0x3f) pcVar27[0] = 0x3f;
        if (pcVar27[0] < 0) pcVar27[0] = 0;
        if (pcVar27[1] > 0x3f) pcVar27[1] = 0x3f;
        if (pcVar27[1] < 0) pcVar27[1] = 0;
        if (pcVar27[2] > 0x3f) pcVar27[2] = 0x3f;
        if (pcVar27[2] < 0) pcVar27[2] = 0;
        pcVar23 += 3;
        pcVar27 += 3;
    }
    gpWindowManager->FizzleForward(0, 0, 0x280, param_3, 0x96,
                                   reinterpret_cast<signed char *>(pcVar21),
                                   reinterpret_cast<signed char *>(pcVar22));
    BaseFree(pcVar21, __FILE__, __LINE__);
    BaseFree(pcVar22, __FILE__, __LINE__);
    BaseFree(pcVar18, __FILE__, __LINE__);
    delete work;
    gpMouseManager->ShowColorPointer();
}

