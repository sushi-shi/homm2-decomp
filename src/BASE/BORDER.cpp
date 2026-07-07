// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BORDER.OBJ   from: .\basewin.lib
// functions: 8   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/border.h>
VA(0x004d20a0, 0x32)
border::border(void) {}

VA(0x004d2130, 0x64)
border::border(short int, short int, short int, short int, short int, short int, short int, char *) {}

VA(0x004d21a0, 0x38)
border::~border() {}

VA(0x004d21e0, 0x10e)
void border::Read(void) {}

VA(0x004d22f0, 0x181)
int border::Main(struct tag_message &) { return 0; }

VA(0x004d2480, 0xab)
void border::Draw(void) {}


// ===== vtable border : public widget  (3 slots) =====
//  [ 0] VA(0x004d2480, 0xab)  void border::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d20e0, 0x4d)  void * border::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d22f0, 0x181)  int border::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(border, 0x004eba60);
