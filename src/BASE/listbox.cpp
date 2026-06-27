// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\listbox.obj   from: .\basewin.lib
// functions: 10   data: 1
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
RVA(0x004db060, 0x42)
// void listBoxWidget::constructor(void);

RVA(0x004db0b0, 0x1f)
// void * listBoxWidget::scalar_dtor(unsigned int);   // virtual [override (implements widget pure virtual)]

RVA(0x004db0b0, 0x1f)
// void * listBoxWidget::scalar_dtor(unsigned int);   // virtual [override (implements widget pure virtual)]

RVA(0x004db0d0, 0x86)
// void listBoxWidget::~destructor(void);

RVA(0x004db160, 0x26e)
// void listBoxWidget::Read(void);

RVA(0x004db3d0, 0x142)
// void listBoxWidget::DeleteItem(int);

RVA(0x004db520, 0x368)
// int listBoxWidget::Main(struct tag_message &);   // virtual [override (implements widget pure virtual)]

RVA(0x004db890, 0x8)
// void listBoxWidget::Draw(void);   // virtual [override (implements widget pure virtual)]

RVA(0x004db8a0, 0x334)
// void listBoxWidget::DrawLBStuff(int);

RVA(0x004dbbe0, 0x312)
// int listBoxWidget::ProcessMouseMessage(struct tag_message &);

// ---- data / globals / vtables ----
DATA(0x004ebac0)  // const listBoxWidget::vftable

// ===== vtable listBoxWidget : public widget  (3 slots) =====
//  [ 0] RVA(0x004db890, 0x8)  void listBoxWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] RVA(0x004db0b0, 0x1f)  void * listBoxWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] RVA(0x004db520, 0x368)  int listBoxWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)
