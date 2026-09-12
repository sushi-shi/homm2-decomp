#include <SOURCE/KB.h>
#include <stdio.h>

i32 __cdecl main() {
    for (i32 value = 0; value < 256; ++value) {
        const char input = static_cast<char>(value);
        const i32 upper = ((value >= 'a' && value <= 'z') || value >= 0xe0)
            ? value - 32 : (value == 0xb8 ? 0xa8 : value);
        const i32 lower = ((value >= 'A' && value <= 'Z') || (value >= 0xc0 && value <= 0xdf))
            ? value + 32 : (value == 0xa8 ? 0xb8 : value);
        if (static_cast<u8>(CyrillicToUpper(input)) != upper
            || static_cast<u8>(CyrillicToLower(input)) != lower) {
            printf("casefold mismatch at %d\n", value);
            return 1;
        }
    }
    printf("CP1251: all 256 uppercase and 256 lowercase inputs pass\n");
    return 0;
}
