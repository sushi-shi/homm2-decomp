// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\winextra\BITS.obj   from: .\basewin.lib
// functions: 3   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/BITS.h>

VA(0x004d1594, 0x2e)
extern "C" int __cdecl BitTest(void *, int) { return 0; }

VA(0x004d15c2, 0x20)
extern "C" void __cdecl BitSet(void *, int) {}

VA(0x004d15e2, 0x22)
extern "C" void __cdecl BitClear(void *, int) {}
