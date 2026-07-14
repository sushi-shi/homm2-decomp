// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\winextra\BITS.obj   from: .\basewin.lib
// functions: 3   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/BITS.h>

// Bit-array primitives (winextra C file): index a flag by (unsigned) bit number. The byte holding
// bit n is at p + n/8; the retail reads/writes it a dword at a time (`*(unsigned int *)`), bit n&7.
// @match-note
// The logic is correct (byte-indexed bit ops), but our MSVC 4.2 /O2 emits an
// indexed [reg+reg] load/or/store with FPO, whereas the retail materializes the address into one
// register and uses a direct `or [mem],reg` under an ebp frame. No flag combo (/O2, /O2 /Oy-, /O1,
// /Ox, /Od, C, C++) reproduces it — same class as TILE's ebx-save. The winextra C-file primitives
// appear built with a different/earlier MSVC than basewin/SOURCE (which match).

VA(0x004d1594, 0x2e)
extern "C" int __cdecl BitTest(void *p, unsigned int n)
{
    return ((1 << (n & 7)) & *(unsigned int *)((char *)p + (n >> 3))) != 0;
}

VA(0x004d15c2, 0x20)
extern "C" void __cdecl BitSet(void *p, unsigned int n)
{
    *(unsigned int *)((char *)p + (n >> 3)) |= 1 << (n & 7);
}

VA(0x004d15e2, 0x22)
extern "C" void __cdecl BitClear(void *p, unsigned int n)
{
    *(unsigned int *)((char *)p + (n >> 3)) &= ~(1 << (n & 7));
}
