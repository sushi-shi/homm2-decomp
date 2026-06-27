// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WIDGET.OBJ   from: .\basewin.lib
// functions: 7   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
VA(0x004dde00, 0x5a)
// void widget::constructor(short int, short int, short int, short int, short int, short int);

VA(0x004dde60, 0x3f)
// void widget::constructor(void);

VA(0x004ddea0, 0x7)
// void widget::~destructor(void);

VA(0x004ddeb0, 0x14)
// int widget::Open(int, class heroWindow *);

VA(0x004dded0, 0x1)
// void widget::Close(void);

VA(0x004ddee0, 0x2f4)
// int widget::Main(struct tag_message &);

VA(0x004de1e0, 0x47)
// void widget::Dim(void);

// ---- data / globals / vtables ----
DATA(0x004ebb00)  // const widget::vftable

// ===== vtable widget (root)  (3 slots) =====
//  [ 0] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 1] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 2] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
