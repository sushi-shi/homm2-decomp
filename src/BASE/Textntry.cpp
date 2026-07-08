// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Textntry.obj   from: .\basewin.lib
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/textEntryWidget.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
VA(0x004d8740, 0x2d)
textEntryWidget::textEntryWidget(void) : textWidget()
{
    field_0x31 = 0;
    field_0x2b = 0;
    field_0x14 = 0x4000;
    field_0x33 = 0;
    field_0x2f = 0;
    field_0x4b = 0;
}

VA(0x004d87b0, 0x134)
textEntryWidget::textEntryWidget(short int, short int, short int, short int, short int, char *, char *, short int, char *, short int, short int, short int, short int, int, int) {}

VA(0x004d88f0, 0x21)
textEntryWidget::~textEntryWidget()
{
    gpResourceManager->Dispose(field_0x2b);
}

VA(0x004d8920, 0x26c)
void textEntryWidget::Read(int) {}

VA(0x004d8b90, 0x874)
int textEntryWidget::Main(struct tag_message &) { return 0; }

VA(0x004d9410, 0x160)
void textEntryWidget::Draw(void) {}

VA(0x004d9570, 0x1be)
void textEntryWidget::SetupDisplayString(char *, unsigned short int) {}


// ===== vtable textEntryWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d9410, 0x160)  void textEntryWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d8770, 0x36)  void * textEntryWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d8b90, 0x874)  int textEntryWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(textEntryWidget, 0x004ebaa0);
