#include <SOURCE/SaveEventHeader.h>

int main() {
    if (EncodeSaveEventHeader(0x1234, 0xabcd) != SaveEventHeader{0x34, 0x12, 0xcd, 0xab})
        return 1;
    for (u32 value = 0; value <= 0xffff; ++value) {
        const u16 first = value ^ 0xa55a;
        u16 count = 0, index = 0;
        DecodeSaveEventHeader(EncodeSaveEventHeader(value, first), count, index);
        if (count != value || index != first) return 2;
    }
    u16 count = 123, first = 456;
    DecodeSaveEventHeader(EncodeSaveEventHeader(0, 0x1234), count, first);
    return count == 0 && first == 0x1234 ? 0 : 3;
}
