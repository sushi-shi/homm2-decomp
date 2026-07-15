// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\winextra\BITS.obj   from: .\basewin.lib
// functions: 3   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/BITS.h>

// Retail keeps an EBP frame in all three optimized routines. Express that
// proven TU-wide setting in source rather than adding a new manifest profile.
#pragma optimize("y", off)

// @match-note: complete 0/0-relocation checkpoint (live 27.63%; retained
// 47.63%). Base is 0x27 bytes versus retail's 0x2e including two trailing NOPs.
// The EBP frame, byte offset,
// dword load, mask and explicit 0/1 result agree. First divergence: base hoists
// the bit-index load ahead of `mov ebp,esp`, then selects indexed `test`/`setne`;
// retail materializes the address in ESI and branches. `/Od` and `/Od /Oi`
// supported profiles score lower; revisit on a genuine profile/TU-state find.
VA(0x004d1594, 0x2e)
extern "C" int __cdecl BitTest(const void *bits, unsigned int bitIndex)
{
    const unsigned char *bytes = static_cast<const unsigned char *>(bits);
    bytes += bitIndex >> 3;
    const unsigned int *word = reinterpret_cast<const unsigned int *>(bytes);
    bitIndex &= 7;
    unsigned int mask = 1U;
    mask <<= bitIndex;
    if ((*word & mask) != 0)
        return 1;
    return 0;
}

// @match-note: complete 0/0-relocation checkpoint (live 29.33%; retained
// 39.67%). Base is 0x27 bytes versus retail's 0x20. The EBP frame and
// byte-addressed dword mask agree. First
// divergence: base hoists the index load before `mov ebp,esp` and keeps an
// indexed load/or/store; retail materializes the address in ESI and emits direct
// `or [esi],eax`. The supported `/Od` profiles score lower; revisit with BitTest.
VA(0x004d15c2, 0x20)
extern "C" void __cdecl BitSet(void *bits, unsigned int bitIndex)
{
    unsigned char *bytes = static_cast<unsigned char *>(bits);
    bytes += bitIndex >> 3;
    unsigned int *word = reinterpret_cast<unsigned int *>(bytes);
    bitIndex &= 7;
    unsigned int mask = 1U;
    mask <<= bitIndex;
    *word |= mask;
}

// @match-note: complete 0/0-relocation checkpoint (live 33.44%; retained
// 43.12%). Base is 0x29 bytes versus retail's 0x22 including two trailing NOPs.
// The EBP frame, address,
// shift, complement and dword clear agree. First divergence is the same hoisted
// index load and indexed load/and/store scheduling as BitSet; retail uses direct
// `and [esi],eax`. The supported `/Od` profiles score lower; revisit with BitTest.
VA(0x004d15e2, 0x22)
extern "C" void __cdecl BitClear(void *bits, unsigned int bitIndex)
{
    unsigned char *bytes = static_cast<unsigned char *>(bits);
    bytes += bitIndex >> 3;
    unsigned int *word = reinterpret_cast<unsigned int *>(bytes);
    bitIndex &= 7;
    unsigned int mask = 1U;
    mask <<= bitIndex;
    *word &= ~mask;
}
