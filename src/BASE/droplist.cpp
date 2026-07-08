// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\droplist.obj   from: .\basewin.lib
// functions: 12   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/dropListWidget.h>
#include <BASE/bitmap.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <BASE/icon.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
#include <string.h>
VA(0x004dbf00, 0x3b)
dropListWidget::dropListWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    field_0x40 = 0;
    field_0xb0 = 0;
    field_0x3c = 0;
    field_0x3e = -1;
}

VA(0x004dbf60, 0x7c)
dropListWidget::~dropListWidget()
{
    gpResourceManager->Dispose(field_0x20);
    gpResourceManager->Dispose(field_0x24);
    if (field_0xb0 != 0)
        delete field_0xb0;
    for (int i = 0; i < field_0x3c; i++)
        BaseFree(field_0x40[i], __FILE__, __LINE__);
    BaseFree(field_0x40, __FILE__, __LINE__);
}

VA(0x004dbfe0, 0x21d)
void dropListWidget::Read(void)
{
    int iVar1;
    short sVar2;
    int *piVar5;
    char local_10[16];
    field_0x18 = gpResourceManager->ReadWord();
    field_0x1a = gpResourceManager->ReadWord();
    field_0x1c = gpResourceManager->ReadWord();
    field_0x1e = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    field_0x20 = gpResourceManager->GetFont(local_10);
    gpResourceManager->RestorePosition();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    field_0x24 = gpResourceManager->GetIcon(local_10);
    gpResourceManager->RestorePosition();
    field_0x28 = field_0x18 + gpResourceManager->ReadWord();
    field_0x2a = field_0x1a + gpResourceManager->ReadWord();
    field_0x2c = gpResourceManager->ReadWord();
    field_0x2e = gpResourceManager->ReadWord();
    field_0x30 = gpResourceManager->ReadWord();
    field_0x34 = gpResourceManager->ReadWord();
    field_0x36 = gpResourceManager->ReadWord();
    field_0x38 = gpResourceManager->ReadWord();
    field_0x3a = gpResourceManager->ReadWord();
    sVar2 = gpResourceManager->ReadWord();
    field_0x48 = 0;
    field_0x4a = 1;
    field_0x4c = 2;
    field_0x4e = 3;
    field_0x50 = 4;
    field_0x52 = 5;
    field_0x54 = 6;
    field_0x56 = 7;
    field_0x58 = 8;
    field_0x5a = 9;
    field_0x5c = 10;
    field_0x10 = sVar2;
    piVar5 = reinterpret_cast<int *>(&field_0x24->field_0x12);
    field_0x5e = 0xb;
    field_0x60 = 0xc;
    field_0x62 = 0xd;
    iVar1 = *piVar5;
    field_0x6c = field_0x18;
    field_0x6e = field_0x1a;
    sVar2 = *reinterpret_cast<short *>(iVar1 + 4);
    field_0x70 = sVar2;
    field_0x72 = *reinterpret_cast<short *>(iVar1 + 6);
    iVar1 = *piVar5;
    field_0x64 = sVar2 + field_0x18;
    field_0x66 = field_0x1a;
    field_0x68 = *reinterpret_cast<short *>(iVar1 + 0x11);
    field_0x6a = *reinterpret_cast<short *>(iVar1 + 0x13);
    iVar1 = *piVar5;
    field_0xa6 = *reinterpret_cast<short *>(iVar1 + 0xad);
    field_0xa8 = *reinterpret_cast<short *>(iVar1 + 0xaf);
}

VA(0x004dc200, 0xd5)
void dropListWidget::DeleteItem(int param_1)
{
    short sVar1;
    char **puVar2, **puVar4, **puVar5;
    unsigned int uVar3;
    sVar1 = field_0x3c;
    if (param_1 < sVar1) {
        if (field_0x3e == param_1)
            field_0x3e = -1;
        if (sVar1 == 1) {
            BaseFree(field_0x40[0], __FILE__, __LINE__);
            BaseFree(field_0x40, __FILE__, __LINE__);
            field_0x40 = 0;
        } else {
            puVar2 = static_cast<char **>(BaseAlloc(sVar1 * 4 - 4, __FILE__, __LINE__));
            puVar4 = field_0x40;
            puVar5 = puVar2;
            for (uVar3 = (field_0x3c * 4 - 4U) >> 2; uVar3 != 0; uVar3--) {
                *puVar5 = *puVar4;
                puVar4 = puVar4 + 1;
                puVar5 = puVar5 + 1;
            }
            uVar3 = (field_0x3c - param_1) - 1;
            if (0 < static_cast<int>(uVar3)) {
                puVar4 = field_0x40 + param_1 + 1;
                puVar5 = puVar2 + param_1;
                for (uVar3 = uVar3 & 0x3fffffff; uVar3 != 0; uVar3--) {
                    *puVar5 = *puVar4;
                    puVar4 = puVar4 + 1;
                    puVar5 = puVar5 + 1;
                }
            }
            if (field_0x40 != 0)
                BaseFree(field_0x40, __FILE__, __LINE__);
            field_0x40 = puVar2;
        }
        field_0x3c = field_0x3c - 1;
    }
}

VA(0x004dc2e0, 0x350)
int dropListWidget::Main(tag_message &param_1)
{
    char cVar1;
    unsigned int uVar6, uVar7;
    short sVar5, sVar8;
    char *pcVar10, *pcVar12;
    char **puVar3, **puVar9, **puVar11;
    if ((field_0x16 & 2) == 0) {
        if (param_1.type == 0x200)
            return widget::Main(param_1);
        return 0;
    } else if (param_1.type == 8 || param_1.type == 0x20) {
        if ((field_0x16 & 4) != 0) {
            sVar8 = static_cast<short>(param_1.field4) - field_0x4->field_0x28;
            sVar5 = static_cast<short>(param_1.field8) - field_0x4->field_0x2c;
            if (param_1.type != 0x20) {
                if (field_0x64 <= sVar8 && field_0x66 <= sVar5 &&
                    sVar8 < field_0x68 + field_0x64 && sVar5 < field_0x6a + field_0x66) {
                    ProcessSelectDialog();
                    param_1.field4 = 0xc;
                    param_1.type = 0x200;
                    param_1.field8 = field_0x10;
                    return 2;
                }
                return 0;
            }
            if (field_0x18 <= sVar8 && field_0x1a <= sVar5 && sVar8 < field_0x1c + field_0x18 &&
                sVar5 < field_0x1e + field_0x1a) {
                param_1.field4 = 0xe;
                param_1.type = 0x200;
                param_1.fieldC = 0x200;
                param_1.field8 = field_0x10;
                return 2;
            }
            return 0;
        }
    } else if (param_1.type == 0x200) {
        switch (param_1.field4) {
        case 0x36:
            if (field_0x10 == param_1.field8) {
                field_0x3e = static_cast<short>(reinterpret_cast<int>(param_1.text));
                return 1;
            }
            break;
        case 0x37:
            if (field_0x10 == param_1.field8) {
                param_1.text = reinterpret_cast<char *>(static_cast<int>(field_0x3e));
                return 1;
            }
            break;
        case 0x38:
            if (field_0x10 == param_1.field8) {
                pcVar10 = param_1.text;
                puVar3 = static_cast<char **>(BaseAlloc(field_0x3c * 4 + 4, __FILE__, __LINE__));
                if (field_0x3c != 0) {
                    puVar9 = field_0x40;
                    puVar11 = puVar3;
                    for (uVar6 = field_0x3c & 0x3fffffff; uVar6 != 0; uVar6--) {
                        *puVar11 = *puVar9;
                        puVar9 = puVar9 + 1;
                        puVar11 = puVar11 + 1;
                    }
                }
                puVar3[field_0x3c] = static_cast<char *>(BaseAlloc(strlen(pcVar10) + 1, __FILE__, __LINE__));
                strcpy(puVar3[field_0x3c], pcVar10);
                field_0x3c = field_0x3c + 1;
                if (field_0x40 != 0)
                    BaseFree(field_0x40, __FILE__, __LINE__);
                field_0x40 = puVar3;
            }
            break;
        case 0x39:
            if (field_0x10 == param_1.field8) {
                pcVar10 = param_1.text;
                if (param_1.fieldC < field_0x3c) {
                    BaseFree(field_0x40[param_1.fieldC], __FILE__, __LINE__);
                    field_0x40[param_1.fieldC] = static_cast<char *>(BaseAlloc(strlen(pcVar10) + 1, __FILE__, __LINE__));
                    strcpy(field_0x40[param_1.fieldC], pcVar10);
                }
            }
            break;
        case 0x3a:
            if (field_0x10 == param_1.field8)
                DeleteItem(param_1.fieldC);
            break;
        case 0x3b:
            if (field_0x10 == param_1.field8) {
                sVar5 = field_0x3c;
                while (sVar5 != 0) {
                    DeleteItem(0);
                    sVar5 = field_0x3c;
                }
            }
        }
    }
    return widget::Main(param_1);
}

VA(0x004dc630, 0xaf)
void dropListWidget::Draw(void)
{
    field_0x24->DrawToBuffer(field_0x6c + field_0x4->field_0x28, field_0x6e + field_0x4->field_0x2c,
                             field_0x48, 0);
    field_0x24->DrawToBuffer(field_0x64 + field_0x4->field_0x28, field_0x66 + field_0x4->field_0x2c,
                             field_0x4a, 0);
    if (field_0x3c > 0 && field_0x3e >= 0) {
        int color = 3;
        if ((field_0x16 & 8) == 0)
            color = field_0x34;
        field_0x20->DrawBoundedString(field_0x40[field_0x3e],
                                      field_0x28 + field_0x4->field_0x28,
                                      field_0x2a + field_0x4->field_0x2c,
                                      field_0x2c, field_0x2e, color, field_0x3a);
    }
}

VA(0x004dc6e0, 0x378)
void dropListWidget::DrawDropStuff(void) {}

VA(0x004dca60, 0x50)
void dropListWidget::SaveDropBackground(void)
{
    field_0xb0 = new bitmap(0, field_0x7e, field_0x80);
    field_0xb0->GrabScreen(field_0x7a, field_0x7c);
}

VA(0x004dcab0, 0x56)
void dropListWidget::RestoreDropBackground(void)
{
    field_0xb0->DrawToBuffer(field_0x7a, field_0x7c);
    gpWindowManager->UpdateScreenRegion(field_0x7a, field_0x7c, field_0x7e, field_0x80);
    if (field_0xb0 != 0)
        delete field_0xb0;
    field_0xb0 = 0;
}

VA(0x004dcb10, 0x81f)
void dropListWidget::ProcessSelectDialog(void) {}


// ===== vtable dropListWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004dc630, 0xaf)  void dropListWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004dbf40, 0x1f)  void * dropListWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004dc2e0, 0x350)  int dropListWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(dropListWidget, 0x004ebad0);
