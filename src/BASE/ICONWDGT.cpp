// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\ICONWDGT.OBJ   from: .\basewin.lib
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconWidget.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
VA(0x004d0a60, 0x2d)
iconWidget::iconWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    field_0x24 = 0;
    field_0x27 = 0;
    field_0x20 = 0;
    field_0x26 = 0;
    field_0x29 = 0;
}

VA(0x004d0ad0, 0x6a)
iconWidget::iconWidget(short int, short int, short int, short int, unsigned long int, short int, signed char, short int, short int, short int) {}

VA(0x004d0b40, 0x78)
iconWidget::iconWidget(short int, short int, short int, short int, char *, short int, signed char, short int, short int, short int) {}

VA(0x004d0bc0, 0xdf)
void iconWidget::Read(void)
{
    char local_10[16];
    field_0x18 = gpResourceManager->ReadWord();
    field_0x1a = gpResourceManager->ReadWord();
    field_0x1c = gpResourceManager->ReadWord();
    field_0x1e = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    unsigned long id = gpResourceManager->MakeId(local_10, 1);
    field_0x20 = gpResourceManager->GetIcon(id);
    gpResourceManager->RestorePosition();
    field_0x24 = gpResourceManager->ReadWord();
    field_0x26 = static_cast<char>(gpResourceManager->ReadWord());
    field_0x10 = gpResourceManager->ReadWord();
    field_0x14 = gpResourceManager->ReadWord();
    field_0x27 = gpResourceManager->ReadWord() & 0xff;
}

VA(0x004d0ca0, 0x21)
iconWidget::~iconWidget()
{
    gpResourceManager->Dispose(field_0x20);
}

VA(0x004d0cd0, 0x291)
int iconWidget::Main(struct tag_message &) { return 0; }

VA(0x004d0f70, 0xe5)
void iconWidget::Draw(void)
{
    short type = field_0x14;
    short x = field_0x4->field_0x28 + field_0x18;
    short y = field_0x4->field_0x2c + field_0x1a;
    if (type == 0x10) {
        field_0x20->DrawToBuffer(x, y, field_0x24, field_0x26);
        return;
    }
    if (type != 0x11) {
        if (type != 0x80)
            return;
        field_0x20->FillToBuffer(x, y, field_0x24, field_0x27, field_0x26, 0);
        return;
    }
    short *entry = reinterpret_cast<short *>(GetIconEntry(field_0x20, field_0x24));
    y = y - entry[1];
    x = x - entry[0];
    if (entry[2] < field_0x1c)
        x = x + (short)((field_0x1c - entry[2]) >> 1);
    if (entry[3] + 2 < field_0x1e)
        y = y + (field_0x1e - entry[3]) - 2;
    field_0x20->DrawToBuffer(x, y, field_0x24, field_0x26);
}


// ===== vtable iconWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d0f70, 0xe5)  void iconWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d0a90, 0x36)  void * iconWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d0cd0, 0x291)  int iconWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(iconWidget, 0x004eba40);
