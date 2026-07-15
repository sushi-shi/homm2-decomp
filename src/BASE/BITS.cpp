// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\winextra\BITS.obj   from: .\basewin.lib
// functions: 3   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/BITSConstants.h>
#include <BASE/BITS.h>

// @semantic
// Clean, structurally complete 0/0-relocation checkpoint. Base is 0x24; retail is 0x2e
// including two trailing alignment NOPs. The EBP frame, byte offset, dword load, bit mask
// and normalized 0/1 result agree. First divergence is +0x1: base zeroes EAX before
// `mov ebp,esp`, then uses indexed `test`/`setne`; retail establishes EBP immediately,
// materializes the byte address in ESI, emits `and eax,[esi]`, and branches to two result
// assignments. The dedicated `/O1 /Oy-` TU profile is an empirical reproduction
// setting, not a historical command line recovered from CodeView; it was byte-identical
// to the earlier `/O2` plus source-pragma probe. Direct and
// parameter-rebased lvalues, explicit quotient/remainder and offset/mask lifetimes,
// register hints, result/switch forms,
// C frontend, Windows preamble, G3/G4/G5/GB and Oa/Ow profiles were tried. No AST pass: the
// residual is not structurally aligned. Revisit only on a genuine predecessor/header state.
VA(0x004d1594, 0x2e)
extern "C" int __cdecl BitTest(const void *bits, BitIndex bitIndex)
{
    const BitByte *bytes = static_cast<const BitByte *>(bits);
    bytes += bitIndex >> BIT_INDEX_BYTE_SHIFT;
    const BitWord *word = reinterpret_cast<const BitWord *>(bytes);
    bitIndex &= BIT_INDEX_WITHIN_BYTE_MASK;
    BitWord mask = 1U;
    mask <<= bitIndex;
    if ((*word & mask) != 0)
        return 1;
    return 0;
}

// @semantic
// Clean, structurally complete 0/0-relocation checkpoint. Base and retail are both 0x20
// with the same EBP frame and direct byte-addressed dword OR. First divergence is +0x1:
// base reads bitIndex through ESP before establishing EBP and retains quotient/base as an
// indexed address with the mask in ESI; retail establishes EBP first, materializes the byte
// address in ESI and uses EAX for quotient then mask (`or [esi],eax`). The profile, source,
// frontend, preamble and CPU/alias variants listed at BitTest were also tested here; none
// changed this allocation. No AST pass below structural alignment. Revisit with BitTest.
VA(0x004d15c2, 0x20)
extern "C" void __cdecl BitSet(void *bits, BitIndex bitIndex)
{
    BitByte *bytes = static_cast<BitByte *>(bits);
    bytes += bitIndex >> BIT_INDEX_BYTE_SHIFT;
    BitWord *word = reinterpret_cast<BitWord *>(bytes);
    bitIndex &= BIT_INDEX_WITHIN_BYTE_MASK;
    BitWord mask = 1U;
    mask <<= bitIndex;
    *word |= mask;
}

// @semantic
// Clean, structurally complete 0/0-relocation checkpoint. Base and retail are both 0x22
// (retail ends with two alignment NOPs) with the same EBP frame, shift, complement and
// direct dword clear. First divergence is +0x1 and matches BitSet: base reads bitIndex
// through ESP before EBP and keeps an indexed address/mask-in-ESI allocation; retail uses
// materialized ESI address and quotient-then-mask EAX (`and [esi],eax`). The same bounded
// profile/source/frontend/preamble/CPU experiments were exhausted. No AST pass below
// structural alignment. Revisit with BitTest.
VA(0x004d15e2, 0x22)
extern "C" void __cdecl BitClear(void *bits, BitIndex bitIndex)
{
    BitByte *bytes = static_cast<BitByte *>(bits);
    bytes += bitIndex >> BIT_INDEX_BYTE_SHIFT;
    BitWord *word = reinterpret_cast<BitWord *>(bytes);
    bitIndex &= BIT_INDEX_WITHIN_BYTE_MASK;
    BitWord mask = 1U;
    mask <<= bitIndex;
    *word &= ~mask;
}
