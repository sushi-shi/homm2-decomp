#include <BASE/FontGlyph.h>

#include <cstdio>

int main() {
    using localization::FontProfile;
    struct Case { std::uint32_t codePoint; int western; int buka; };
    const Case cases[] = {
        {' ', 0, 0}, {'A', 33, 33}, {'a', 65, 65},
        {'I', 41, 41}, {'i', 73, 73}, {'Z', 58, 58}, {'z', 90, 90},
        {0x2014, 13, 13}, {0x0410, 95, 96}, {0x0430, 95, 129},
        {0x0401, 95, 128}, {0x0451, 95, 161}, {0x732b, 95, 95},
    };
    for (const Case& test : cases) {
        if (FontGlyphIndex(test.codePoint, FontProfile::Latin, 96) != test.western
            || FontGlyphIndex(test.codePoint, FontProfile::BukaCyrillic, 162) != test.buka) {
            std::fprintf(stderr, "incorrect retail glyph for U+%04x\n", test.codePoint);
            return 1;
        }
    }
    if (FontGlyphIndex('a', FontProfile::Latin, 40) != 39
        || FontGlyphIndex('a', FontProfile::Latin, 0) != 0) {
        std::fputs("font frame bounds were not respected\n", stderr);
        return 1;
    }
    return 0;
}
