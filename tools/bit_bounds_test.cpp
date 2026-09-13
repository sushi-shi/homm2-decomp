#include <BASE/BITS.h>

#include <cstdio>

int main() {
    // Exactly one byte: the final bit must not cause a wider load or store.
    u8* byte = new u8(0);
    for (u32 bit = 0; bit < 8; ++bit) {
        H2BitSet(byte, bit);
        if (!H2BitTest(byte, bit) || *byte != ((1u << (bit + 1)) - 1u)) {
            std::fputs("single-byte bit set/test failed\n", stderr);
            delete byte;
            return 1;
        }
    }
    for (u32 bit = 0; bit < 8; ++bit) {
        H2BitClear(byte, bit);
        if (H2BitTest(byte, bit)) {
            std::fputs("single-byte bit clear failed\n", stderr);
            delete byte;
            return 1;
        }
    }
    delete byte;

    // Include an odd-addressed tail and verify that neighboring bytes survive.
    u8 bytes[3] = {0x5a, 0, 0xa5};
    H2BitSet(bytes, 15);
    if (bytes[0] != 0x5a || bytes[1] != 0x80 || bytes[2] != 0xa5)
        return 1;
    H2BitClear(bytes, 15);
    if (bytes[0] != 0x5a || bytes[1] != 0 || bytes[2] != 0xa5)
        return 1;
    return 0;
}
