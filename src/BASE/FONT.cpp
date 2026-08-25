#include <Ints.h>
#include <string.h>
#include <BASE/font.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/IconEntry.h>
#include <BASE/Icon2b.h>
#include <BASE/icon2bc.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <BASE/Utf8.h>
#include <SOURCE/KB.h>
#include <SOURCE/Localization.h>

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

typedef enum FontConstant {
    LARGE_FONT_HEIGHT_THRESHOLD = 14,
    FONT_DRAW_SCREEN_WIDTH      = 640,
    FONT_DRAW_SCREEN_HEIGHT     = 480,
    CENTER_DIVISOR              = 2,
    WRAP_HEIGHT_LINE_COUNT      = 2
} FontConstant;

font::font(u32l id) : resource(RESOURCE_CATEGORY_FONT, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    char name[RESOURCE_MANAGER_READ13_BYTES];
    gpResourceManager->PointToFile(id);
    m_height = gpResourceManager->ReadWord();
    i32 h = gpResourceManager->ReadWord();
    if (m_height >= LARGE_FONT_HEIGHT_THRESHOLD)
        m_isLarge = 1;
    else
        m_isLarge = 0;
    gpResourceManager->Read13(reinterpret_cast<i8*>(name));
    gbLoadingMonoIcon = true;
    m_glyphIcon = gpResourceManager->GetIcon(name);
    gbLoadingMonoIcon = false;
    if (localization::ActiveFontProfile() == localization::FontProfile::BukaCyrillic
        && m_glyphIcon->m_frameCount < 162) {
        localization::RejectResourceProfile(
            "the selected profile requires FONT.ICN and SMALFONT.ICN with at least 162 frames"
        );
        gpResourceManager->DisableLocaleAggregates();
    }
}

font::~font() {
    gpResourceManager->Dispose(m_glyphIcon);
}


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

i32 GlyphIndex(std::uint32_t codePoint, i32 frameCount) {
    i32 glyph;
    if (codePoint == 0x2013 || codePoint == 0x2014)
        codePoint = '-';
    if (localization::ActiveFontProfile() == localization::FontProfile::BukaCyrillic) {
        if (codePoint >= 0x0400)
            glyph = CyrillicGlyph(codePoint);
        else if (codePoint >= ' ' && codePoint <= 0x7f)
            glyph = static_cast<i32>(codePoint - ' ');
        else
            glyph = FONT_GLYPH_FALLBACK;
    } else {
        if (codePoint >= 'a' && codePoint <= 'z')
            codePoint -= 'a' - 'A';
        glyph = codePoint >= ' ' && codePoint <= 0x7f
            ? static_cast<i32>(codePoint - ' ')
            : FONT_GLYPH_FALLBACK;
    }
    if (frameCount <= 0)
        return 0;
    return std::clamp(glyph, 0, frameCount - 1);
}

bool IsVowel(std::uint32_t codePoint) {
    switch (codePoint) {
        case 'a': case 'e': case 'i': case 'o': case 'u': case 'y':
        case 'A': case 'E': case 'I': case 'O': case 'U': case 'Y':
        case 0x0410: case 0x0415: case 0x0401: case 0x0418: case 0x041e:
        case 0x0423: case 0x042b: case 0x042d: case 0x042e: case 0x042f:
        case 0x0430: case 0x0435: case 0x0451: case 0x0438: case 0x043e:
        case 0x0443: case 0x044b: case 0x044d: case 0x044e: case 0x044f:
            return true;
        default:
            return false;
    }
}

}

void font::DrawStringExecute(
    const char* str,
    i32 x,
    i32 y,
    FontDrawMode mode,
    i32 clipL,
    i32 clipT,
    i32 clipR,
    i32 clipB
) {
    i32 pos = x;
    const char* cursor = str;
    while (cursor != NULL && *cursor != 0) {
        const utf8::Decoded decoded = utf8::Decode(cursor);
        const std::uint32_t codePoint = decoded.codePoint;
        if (codePoint == FONT_SPACER_CHAR) {
            pos += GetCharacterWidth(codePoint);
            cursor += decoded.length;
            continue;
        }
        if (codePoint == '{') {
            m_suppressDraw = 1;
            cursor += decoded.length;
            continue;
        }
        if (codePoint == '}') {
            m_suppressDraw = 0;
            cursor += decoded.length;
            continue;
        }

        const i32 glyph = GlyphIndex(codePoint, m_glyphIcon->m_frameCount);
        if (glyph != 0) {
            if (mode == FONT_DRAW_DEFAULT && m_suppressDraw == 0)
                IconToBitmap(
                    m_glyphIcon,
                    gpWindowManager->m_screen,
                    pos,
                    y,
                    glyph,
                    ICON_DRAW_CLIP,
                    clipL,
                    clipT,
                    clipR,
                    clipB,
                    0
                );
            else if (mode == FONT_DRAW_YELLOW
                     || (mode == FONT_DRAW_DEFAULT && m_suppressDraw != 0))
                IconToBitmapColorTable(
                    m_glyphIcon,
                    gpWindowManager->m_screen,
                    pos,
                    y,
                    glyph,
                    ICON_DRAW_CLIP,
                    clipL,
                    clipT,
                    clipR,
                    clipB,
                    0,
                    gColorTableYellow,
                    1
                );
            else if (mode == FONT_DRAW_SCENARIO_WIN)
                IconToBitmapColorTable(
                    m_glyphIcon,
                    gpWindowManager->m_screen,
                    pos,
                    y,
                    glyph,
                    ICON_DRAW_CLIP,
                    clipL,
                    clipT,
                    clipR,
                    clipB,
                    0,
                    gColorTableScenWin,
                    0
                );
            else
                IconToBitmapColorTable(
                    m_glyphIcon,
                    gpWindowManager->m_screen,
                    pos,
                    y,
                    glyph,
                    ICON_DRAW_CLIP,
                    clipL,
                    clipT,
                    clipR,
                    clipB,
                    0,
                    gColorTableDarkGray,
                    1
                );
        }
        pos += GetCharacterWidth(codePoint);
        cursor += decoded.length;
    }
}

void font::DrawString(const char* s, i32 x, i32 y, FontDrawMode mode) {
    m_suppressDraw = 0;
    DrawStringExecute(s, x, y, mode, 0, 0, FONT_DRAW_SCREEN_WIDTH, FONT_DRAW_SCREEN_HEIGHT);
}

i32 font::GetCharacterWidth(std::uint32_t codePoint) {
    if (codePoint == '{' || codePoint == '}') {
        return 0;
    }
    if (codePoint == ' ')
        codePoint = 'i';
    if (localization::ActiveFontProfile() == localization::FontProfile::BukaCyrillic
        && codePoint == '.')
        codePoint = '_';
    const i32 glyph = GlyphIndex(codePoint, m_glyphIcon->m_frameCount);
    return reinterpret_cast<struct IconEntry*>(m_glyphIcon->m_data)[glyph].w + m_isLarge;
}

void font::ExtractLine(
    const char* text,
    char* line,
    i32* position,
    i32 maxWidth,
    i32* lineWidth,
    u8 lastLine
) {
    struct Character {
        std::size_t end;
        std::uint32_t codePoint;
        i32 width;
    };

    const std::size_t start = static_cast<std::size_t>(*position);
    std::size_t cursor = start;
    i32 width = 0;
    const auto finish = [&](std::size_t end, std::size_t next, i32 measured, bool hyphen) {
        std::size_t length = end - start;
        memcpy(line, text + start, length);
        if (hyphen)
            line[length++] = '-';
        line[length] = 0;
        *position = static_cast<i32>(next);
        *lineWidth = measured + (hyphen ? GetCharacterWidth('-') : 0);
    };

    if (lastLine != 0) {
        while (text[cursor] != '\n' && text[cursor] != 0) {
            const utf8::Decoded decoded = utf8::Decode(text + cursor);
            width += GetCharacterWidth(decoded.codePoint);
            cursor += decoded.length;
        }
        finish(cursor, text[cursor] == '\n' ? cursor + 1 : cursor, width, false);
        return;
    }

    while (true) {
        const std::size_t wordStart = cursor;
        const i32 widthBeforeWord = width;
        std::vector<Character> word;
        while (text[cursor] != ' ' && text[cursor] != '\n' && text[cursor] != 0) {
            const utf8::Decoded decoded = utf8::Decode(text + cursor);
            width += GetCharacterWidth(decoded.codePoint);
            cursor += decoded.length;
            word.push_back({cursor, decoded.codePoint, GetCharacterWidth(decoded.codePoint)});
        }

        if (width > maxWidth) {
            if (wordStart != start) {
                const std::size_t previous = utf8::Previous(text, wordStart);
                const bool trailingSpace = text[previous] == ' ';
                finish(
                    trailingSpace ? previous : wordStart,
                    wordStart,
                    widthBeforeWord - (trailingSpace ? GetCharacterWidth(' ') : 0),
                    false
                );
                return;
            }

            i32 prefixWidth = 0;
            std::size_t fallback = 0;
            i32 fallbackWidth = 0;
            for (std::size_t index = 0; index < word.size(); ++index) {
                prefixWidth += word[index].width;
                if (prefixWidth <= maxWidth || index == 0) {
                    fallback = index;
                    fallbackWidth = prefixWidth;
                }
            }

            for (std::size_t count = word.size(); count > 2; --count) {
                const std::size_t index = count - 2;
                i32 candidateWidth = 0;
                for (std::size_t prefix = 0; prefix <= index; ++prefix)
                    candidateWidth += word[prefix].width;
                const bool hyphenInText = word[index].codePoint == '-';
                const bool doubledConsonant = index + 1 < word.size()
                    && !IsVowel(word[index].codePoint)
                    && !IsVowel(word[index + 1].codePoint)
                    && word[index].codePoint == word[index + 1].codePoint;
                const bool breakable = IsVowel(word[index].codePoint)
                    || hyphenInText || doubledConsonant;
                const bool addHyphen = !hyphenInText;
                if (breakable
                    && candidateWidth + (addHyphen ? GetCharacterWidth('-') : 0) <= maxWidth) {
                    finish(word[index].end, word[index].end, candidateWidth, addHyphen);
                    return;
                }
            }

            finish(word[fallback].end, word[fallback].end, fallbackWidth, false);
            return;
        }

        if (text[cursor] == '\n') {
            finish(cursor, cursor + 1, width, false);
            return;
        }
        if (text[cursor] == 0) {
            finish(cursor, cursor, width, false);
            return;
        }
        width += GetCharacterWidth(' ');
        ++cursor;
    }
}

void font::DrawBoundedString(
    const char* str,
    i32 x,
    i32 y,
    i32 w,
    i32 h,
    FontDrawMode mode,
    FontAlignment align
) {
    if (str == NULL)
        return;

    const i32 len = static_cast<i32>(strlen(str));
    i32 xPosition = 0;
    i32 yPosition = 0;
    i32 pos = 0;
    i32 lw = 0;
    std::vector<char> line(static_cast<std::size_t>(len) + 2, 0);
    if ((H2EnumIndex((align) & (FONT_ALIGN_VERTICAL_CENTER)))) {
        align -= FONT_ALIGN_VERTICAL_CENTER;
        i32 lineCount = LineLength(str, w);
        i32 totalH = lineCount * m_height;
        if (totalH < h)
            yPosition = (h - totalH) / CENTER_DIVISOR;
    }
    m_suppressDraw = 0;
    while (pos < len && str[pos] != 0 && (yPosition + m_height <= h || yPosition == 0)) {
        if (yPosition + m_height * WRAP_HEIGHT_LINE_COUNT > h)
            ExtractLine(str, line.data(), &pos, w, &lw, 1);
        else
            ExtractLine(str, line.data(), &pos, w, &lw, 0);
        switch (align) {
            case FONT_ALIGN_LEFT:
                xPosition = 0;
                break;
            case FONT_ALIGN_CENTER:
                xPosition = (w - lw) / CENTER_DIVISOR + 1;
                break;
            case FONT_ALIGN_RIGHT:
                xPosition = w - lw;
                break;
            default:
                xPosition = 0;
                break;
        }
        DrawStringExecute(line.data(), xPosition + x, yPosition + y, mode, x, y, w, h);
        yPosition += m_height;
        lw = 0;
    }
}

#undef CENTER_DIVISOR
#undef WRAP_HEIGHT_LINE_COUNT

i32 font::LineLength(const char* str, i32 maxW) {
    if (str == NULL)
        return 0;
    const i32 len = static_cast<i32>(strlen(str));
    i32 count = 0;
    i32 pos = 0;
    i32 lw = 0;
    std::vector<char> line(static_cast<std::size_t>(len) + 2, 0);
    while (pos < len && str[pos] != 0) {
        ExtractLine(str, line.data(), &pos, maxW, &lw, 0);
        count++;
        lw = 0;
    }
    return count;
}

i32 font::LineWidth(const char* str) {
    i32 width = 0;
    const char* cursor = str;
    while (cursor != NULL && *cursor != 0) {
        const utf8::Decoded decoded = utf8::Decode(cursor);
        if (decoded.codePoint == '\n')
            break;
        width += GetCharacterWidth(decoded.codePoint);
        cursor += decoded.length;
    }
    return width;
}
