#include <va.h>
#include <BASE/BITSConstants.h>
#include <BASE/BITS.h>

H2_ENUM_BEGIN(BitIndexConstant)
    INDEX_BYTE_SHIFT       = 3,
    INDEX_WITHIN_BYTE_MASK = 7
H2_ENUM_END(BitIndexConstant)

VA(0x004c2ed4, 0x2e)
extern "C" i32 __cdecl BitTest(const void* bits, BitIndex bitIndex) {
    __asm {
        mov esi, bits
        mov eax, bitIndex
        mov ecx, eax
        shr eax, INDEX_BYTE_SHIFT
        and ecx, INDEX_WITHIN_BYTE_MASK
        add esi, eax
        mov eax, 1
        shl eax, cl
        and eax, [esi]
        jne bitSet
        mov eax, 0
        jmp done
    bitSet:
        mov eax, 1
    done:
    }
}

VA(0x004c2f02, 0x20)
extern "C" void __cdecl BitSet(void* bits, BitIndex bitIndex) {
    __asm {
        mov esi, bits
        mov eax, bitIndex
        mov ecx, eax
        shr eax, INDEX_BYTE_SHIFT
        and ecx, INDEX_WITHIN_BYTE_MASK
        add esi, eax
        mov eax, 1
        shl eax, cl
        or [esi], eax
    }
}

VA(0x004c2f22, 0x22)
extern "C" void __cdecl BitClear(void* bits, BitIndex bitIndex) {
    __asm {
        mov esi, bits
        mov eax, bitIndex
        mov ecx, eax
        shr eax, INDEX_BYTE_SHIFT
        and ecx, INDEX_WITHIN_BYTE_MASK
        add esi, eax
        mov eax, 1
        shl eax, cl
        not eax
        and [esi], eax
    }
}
