// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\PALETTE.OBJ   from: .\basewin.lib
// functions: 6   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
VA(0x004d2580, 0x2e)
// void palette::constructor(void);

VA(0x004d25b0, 0x41)
// void * palette::scalar_dtor(unsigned int);   // virtual [introduces virtual]

VA(0x004d25b0, 0x41)
// void * palette::scalar_dtor(unsigned int);   // virtual [introduces virtual]

VA(0x004d2600, 0x55)
// void palette::constructor(unsigned long int);

VA(0x004d2660, 0x2c)
// void palette::~destructor(void);

VA(0x004d2690, 0x4)
// signed char * palette::Data(void);

// ---- data / globals / vtables ----
DATA(0x004eba7c)  // const palette::vftable

// ===== vtable palette (root)  (1 slots) =====
//  [ 0] VA(0x004d25b0, 0x41)  void * palette::scalar_dtor(unsigned int)   <- introduces virtual
