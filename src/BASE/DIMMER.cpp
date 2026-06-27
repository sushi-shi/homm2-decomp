// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\DIMMER.OBJ   from: .\basewin.lib
// functions: 7   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
VA(0x004dd330, 0x1e)
// void dimmerWidget::constructor(void);

VA(0x004dd350, 0x34)
// void dimmerWidget::constructor(short int, short int, short int, short int, short int, short int);

VA(0x004dd390, 0x5f)
// void dimmerWidget::Read(void);

VA(0x004dd3f0, 0xd)
// int dimmerWidget::Main(struct tag_message &);   // virtual [override (implements widget pure virtual)]

VA(0x004dd400, 0x5)
// void dimmerWidget::Draw(void);   // virtual [override (implements widget pure virtual)]

VA(0x004dd410, 0x25)
// void * dimmerWidget::scalar_dtor(unsigned int);   // virtual [override (implements widget pure virtual)]

VA(0x004dd410, 0x25)
// void * dimmerWidget::scalar_dtor(unsigned int);   // virtual [override (implements widget pure virtual)]

// ---- data / globals / vtables ----
DATA(0x004ebae0)  // const dimmerWidget::vftable

// ===== vtable dimmerWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004dd400, 0x5)  void dimmerWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004dd410, 0x25)  void * dimmerWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004dd3f0, 0xd)  int dimmerWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)
