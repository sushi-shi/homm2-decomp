#include <BASE/FontGlyph.h>
#include <BASE/font.h>

#include <algorithm>

namespace {

i32 CyrillicGlyph(std::uint32_t codePoint) {
    if (codePoint == 0x0401)
        return 128;
    if (codePoint == 0x0451)
        return 161;
    if (codePoint >= 0x0410 && codePoint <= 0x042f)
        return 96 + static_cast<i32>(codePoint - 0x0410);
    if (codePoint >= 0x0430 && codePoint <= 0x044f)
        return 129 + static_cast<i32>(codePoint - 0x0430);
    return FONT_GLYPH_FALLBACK;
}

}

i32 FontGlyphIndex(std::uint32_t codePoint, localization::FontProfile profile, i32 frameCount) {
    i32 glyph;
    if (codePoint == 0x2013 || codePoint == 0x2014)
        codePoint = '-';
    if (profile == localization::FontProfile::BukaCyrillic) {
        if (codePoint >= 0x0400)
            glyph = CyrillicGlyph(codePoint);
        else if (codePoint >= ' ' && codePoint <= 0x7f)
            glyph = static_cast<i32>(codePoint - ' ');
        else
            glyph = FONT_GLYPH_FALLBACK;
    } else {
        glyph = codePoint >= ' ' && codePoint <= 0x7f
            ? static_cast<i32>(codePoint - ' ')
            : FONT_GLYPH_FALLBACK;
    }
    if (frameCount <= 0)
        return 0;
    return std::clamp(glyph, 0, frameCount - 1);
}

