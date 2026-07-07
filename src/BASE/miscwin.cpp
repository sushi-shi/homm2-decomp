// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\miscwin.obj   from: .\basewin.lib
// functions: 2   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/miscwin.h>
#include <_all.h>
VA(0x004d8540, 0x1e2)
extern "C" void __fastcall BlitBitmapToScreenVesa(int, int, int, int, int, int, int) {}

VA(0x004d8730, 0x4)
short int AutoInitSVGA(void) { return 0; }

// ---- data / globals / vtables ----
DATA(0x00538080)  // struct tagPAINTSTRUCT ps
