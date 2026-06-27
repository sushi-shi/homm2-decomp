// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\ICONWDGT.OBJ   from: .\basewin.lib
// functions: 9   data: 1
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
RVA(0x004d0a60, 0x2d)
// void iconWidget::constructor(void);

RVA(0x004d0a90, 0x36)
// void * iconWidget::scalar_dtor(unsigned int);   // virtual [override (implements widget pure virtual)]

RVA(0x004d0a90, 0x36)
// void * iconWidget::scalar_dtor(unsigned int);   // virtual [override (implements widget pure virtual)]

RVA(0x004d0ad0, 0x6a)
// void iconWidget::constructor(short int, short int, short int, short int, unsigned long int, short int, signed char, short int, short int, short int);

RVA(0x004d0b40, 0x78)
// void iconWidget::constructor(short int, short int, short int, short int, char *, short int, signed char, short int, short int, short int);

RVA(0x004d0bc0, 0xdf)
// void iconWidget::Read(void);

RVA(0x004d0ca0, 0x21)
// void iconWidget::~destructor(void);

RVA(0x004d0cd0, 0x291)
// int iconWidget::Main(struct tag_message &);   // virtual [override (implements widget pure virtual)]

RVA(0x004d0f70, 0xe5)
// void iconWidget::Draw(void);   // virtual [override (implements widget pure virtual)]

// ---- data / globals / vtables ----
DATA(0x004eba40)  // const iconWidget::vftable

// ===== vtable iconWidget : public widget  (3 slots) =====
//  [ 0] RVA(0x004d0f70, 0xe5)  void iconWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] RVA(0x004d0a90, 0x36)  void * iconWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] RVA(0x004d0cd0, 0x291)  int iconWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)
