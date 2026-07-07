// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BUTTON.OBJ   from: .\basewin.lib
// functions: 11   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_all.h>
VA(0x004dd440, 0x34)
button::button(void) {}

VA(0x004dd4c0, 0x6e)
button::button(short int, short int, short int, short int, unsigned long int, short int, short int, short int, short int, short int, short int) {}

VA(0x004dd530, 0x7c)
button::button(short int, short int, short int, short int, char *, short int, short int, short int, short int, short int, short int) {}

VA(0x004dd5b0, 0xeb)
void button::Read(void) {}

VA(0x004dd6a0, 0x21)
button::~button() {}

VA(0x004dd6d0, 0x595)
int button::Main(struct tag_message &) { return 0; }

VA(0x004ddc70, 0x96)
short int button::Select(struct tag_message &) { return 0; }

VA(0x004ddd10, 0x83)
short int button::Deselect(struct tag_message &) { return 0; }

VA(0x004ddda0, 0x55)
void button::Draw(void) {}

// ---- data / globals / vtables ----
DATA(0x004ebaf0)  // const button::vftable
DATA(0x0052125c)  // int iLeftRightSave

// ===== vtable button : public widget  (3 slots) =====
//  [ 0] VA(0x004ddda0, 0x55)  void button::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004dd480, 0x36)  void * button::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004dd6d0, 0x595)  int button::Main(struct tag_message &)   <- override (implements widget pure virtual)
