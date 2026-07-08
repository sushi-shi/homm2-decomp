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
void dropListWidget::Read(void) {}

VA(0x004dc200, 0xd5)
void dropListWidget::DeleteItem(int) {}

VA(0x004dc2e0, 0x350)
int dropListWidget::Main(struct tag_message &) { return 0; }

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
