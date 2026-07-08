// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\listbox.obj   from: .\basewin.lib
// functions: 10   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/listBoxWidget.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/font.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
VA(0x004db060, 0x42)
listBoxWidget::listBoxWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    field_0x3c = 0;
    field_0x8e = 0;
    field_0x32 = 0;
    field_0x34 = -1;
    field_0x36 = -1;
}

VA(0x004db0d0, 0x86)
listBoxWidget::~listBoxWidget()
{
    gpResourceManager->Dispose(field_0x20);
    gpResourceManager->Dispose(field_0x24);
    if (field_0x8e != 0)
        delete field_0x8e;
    for (int i = 0; i < field_0x32; i++)
        BaseFree(field_0x3c[i], __FILE__, __LINE__);
    BaseFree(field_0x3c, __FILE__, __LINE__);
    gbSendMouseMoveMessages = 0;
}

VA(0x004db160, 0x26e)
void listBoxWidget::Read(void)
{
    unsigned short uVar1;
    int iVar2;
    short sVar3, sVar4, sVar7, sVar8;
    int *piVar9;
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
    field_0x28 = gpResourceManager->ReadWord();
    field_0x2c = gpResourceManager->ReadWord();
    field_0x2e = gpResourceManager->ReadWord();
    field_0x30 = gpResourceManager->ReadWord();
    sVar3 = gpResourceManager->ReadWord();
    field_0x44 = 0;
    field_0x10 = sVar3;
    field_0x46 = 1;
    field_0x48 = 2;
    field_0x4a = 3;
    field_0x4c = 4;
    field_0x4e = 5;
    field_0x50 = 6;
    field_0x52 = 7;
    field_0x54 = 8;
    field_0x56 = 9;
    field_0x58 = 10;
    iVar2 = *reinterpret_cast<int *>(&field_0x24->field_0x12);
    piVar9 = reinterpret_cast<int *>(&field_0x24->field_0x12);
    field_0x84 = *reinterpret_cast<short *>(iVar2 + 0x86);
    sVar3 = *reinterpret_cast<short *>(iVar2 + 0x88);
    field_0x86 = sVar3;
    sVar4 = *reinterpret_cast<short *>(*piVar9 + 6);
    field_0x5a = sVar4;
    sVar8 = *reinterpret_cast<short *>(*piVar9 + 0x13);
    field_0x5c = sVar8;
    sVar7 = *reinterpret_cast<short *>(*piVar9 + 0x20);
    field_0x5e = sVar7;
    field_0x60 = field_0x18;
    field_0x62 = field_0x1a;
    field_0x64 = *reinterpret_cast<short *>(*piVar9 + 4);
    field_0x66 = (field_0x28 - 2) * sVar8 + sVar4 + sVar7;
    iVar2 = *piVar9;
    sVar4 = *reinterpret_cast<short *>(iVar2 + 0x2b);
    field_0x6c = sVar4;
    sVar8 = *reinterpret_cast<short *>(iVar2 + 0x2d);
    field_0x6e = sVar8;
    iVar2 = *piVar9;
    uVar1 = *reinterpret_cast<unsigned short *>(iVar2 + 0x45);
    field_0x7c = uVar1;
    sVar7 = (field_0x1c - sVar4) + field_0x18;
    sVar4 = *reinterpret_cast<short *>(iVar2 + 0x47);
    field_0x7e = sVar4;
    field_0x68 = sVar7;
    field_0x6a = field_0x1a;
    field_0x78 = sVar7;
    sVar4 = field_0x1a + (field_0x1e - sVar4);
    field_0x7a = sVar4;
    field_0x70 = sVar7;
    sVar8 = field_0x1a + sVar8;
    sVar4 = sVar4 - sVar8;
    field_0x8a = 0;
    field_0x8b = 0;
    field_0x8d = 0;
    field_0x8c = 0;
    field_0x40 = 0;
    field_0x42 = 0;
    field_0x2a = 0;
    field_0x72 = sVar8;
    field_0x74 = uVar1;
    field_0x76 = sVar4;
    field_0x88 = (sVar4 - sVar3) + -7;
}

VA(0x004db3d0, 0x142)
void listBoxWidget::DeleteItem(int) {}

VA(0x004db520, 0x368)
int listBoxWidget::Main(struct tag_message &) { return 0; }

VA(0x004db890, 0x8)
void listBoxWidget::Draw(void)
{
    DrawLBStuff(0);
}

VA(0x004db8a0, 0x334)
void listBoxWidget::DrawLBStuff(int) {}

VA(0x004dbbe0, 0x312)
int listBoxWidget::ProcessMouseMessage(struct tag_message &) { return 0; }


// ===== vtable listBoxWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004db890, 0x8)  void listBoxWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004db0b0, 0x1f)  void * listBoxWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004db520, 0x368)  int listBoxWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(listBoxWidget, 0x004ebac0);
