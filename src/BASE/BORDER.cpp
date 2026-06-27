// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BORDER.OBJ   from: .\basewin.lib
// functions: 8   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
VA(0x004d20a0, 0x32)
// void border::constructor(void);

VA(0x004d20e0, 0x4d)
// void * border::scalar_dtor(unsigned int);   // virtual [override (implements widget pure virtual)]

VA(0x004d20e0, 0x4d)
// void * border::scalar_dtor(unsigned int);   // virtual [override (implements widget pure virtual)]

VA(0x004d2130, 0x64)
// void border::constructor(short int, short int, short int, short int, short int, short int, short int, char *);

VA(0x004d21a0, 0x38)
// void border::~destructor(void);

VA(0x004d21e0, 0x10e)
// void border::Read(void);

VA(0x004d22f0, 0x181)
// int border::Main(struct tag_message &);   // virtual [override (implements widget pure virtual)]

VA(0x004d2480, 0xab)
// void border::Draw(void);   // virtual [override (implements widget pure virtual)]

// ---- data / globals / vtables ----
DATA(0x004eba60)  // const border::vftable

// ===== vtable border : public widget  (3 slots) =====
//  [ 0] VA(0x004d2480, 0xab)  void border::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d20e0, 0x4d)  void * border::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d22f0, 0x181)  int border::Main(struct tag_message &)   <- override (implements widget pure virtual)
