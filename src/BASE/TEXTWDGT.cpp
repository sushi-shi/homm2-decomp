// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\TEXTWDGT.OBJ   from: .\basewin.lib
// functions: 10   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/textWidget.h>
VA(0x004d1060, 0x3e)
textWidget::textWidget(void) {}

VA(0x004d10f0, 0x64)
textWidget::textWidget(short int, short int, short int, short int, char *, char *, short int, short int, short int, short int) {}

VA(0x004d1160, 0xef)
void textWidget::Read(void) {}

VA(0x004d1250, 0x30)
textWidget::~textWidget() {}

VA(0x004d1280, 0x210)
int textWidget::Main(struct tag_message &) { return 0; }

VA(0x004d1490, 0x49)
void textWidget::Draw(void) {}

VA(0x004d14e0, 0xc)
void textWidget::SetColorIndex(short int) {}

VA(0x004d14f0, 0xa2)
void textWidget::SetText(char *) {}

// ---- data / globals / vtables ----
DATA(0x004eba50)  // const textWidget::vftable

// ===== vtable textWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d1490, 0x49)  void textWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d10a0, 0x45)  void * textWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d1280, 0x210)  int textWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)
