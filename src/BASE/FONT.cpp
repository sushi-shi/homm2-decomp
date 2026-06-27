// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\FONT.OBJ   from: .\basewin.lib
// functions: 10   data: 1
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
RVA(0x004c6fd0, 0xc8)
// void font::constructor(unsigned long int);

RVA(0x004c70a0, 0x39)
// void * font::scalar_dtor(unsigned int);   // virtual [introduces virtual]

RVA(0x004c70a0, 0x39)
// void * font::scalar_dtor(unsigned int);   // virtual [introduces virtual]

RVA(0x004c70e0, 0x39)
// void font::~destructor(void);

RVA(0x004c7120, 0x24a)
// void font::DrawStringExecute(char *, int, int, int, int, int, int, int);

RVA(0x004c7370, 0x48)
// void font::DrawString(char *, int, int, int);

RVA(0x004c73c0, 0xaf)
// int font::GetCharacterWidth(unsigned char);

RVA(0x004c7470, 0x313)
// void font::DrawBoundedString(char *, int, int, int, int, int, int);

RVA(0x004c7790, 0x1b3)
// int font::LineLength(char *, int);

RVA(0x004c7950, 0xc4)
// int font::LineWidth(char *);

// ---- data / globals / vtables ----
DATA(0x004eb9e4)  // const font::vftable

// ===== vtable font (root)  (1 slots) =====
//  [ 0] RVA(0x004c70a0, 0x39)  void * font::scalar_dtor(unsigned int)   <- introduces virtual
