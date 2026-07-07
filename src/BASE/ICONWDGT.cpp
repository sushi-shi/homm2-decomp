// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\ICONWDGT.OBJ   from: .\basewin.lib
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_all.h>
VA(0x004d0a60, 0x2d)
iconWidget::iconWidget(void) {}

VA(0x004d0ad0, 0x6a)
iconWidget::iconWidget(short int, short int, short int, short int, unsigned long int, short int, signed char, short int, short int, short int) {}

VA(0x004d0b40, 0x78)
iconWidget::iconWidget(short int, short int, short int, short int, char *, short int, signed char, short int, short int, short int) {}

VA(0x004d0bc0, 0xdf)
void iconWidget::Read(void) {}

VA(0x004d0ca0, 0x21)
iconWidget::~iconWidget() {}

VA(0x004d0cd0, 0x291)
int iconWidget::Main(struct tag_message &) { return 0; }

VA(0x004d0f70, 0xe5)
void iconWidget::Draw(void) {}

// ---- data / globals / vtables ----
DATA(0x004eba40)  // const iconWidget::vftable

// ===== vtable iconWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d0f70, 0xe5)  void iconWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d0a90, 0x36)  void * iconWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d0cd0, 0x291)  int iconWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)
