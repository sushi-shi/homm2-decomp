#include <va.h>
#include <string.h>
#include <BASE/font.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/IconEntry.h>
#include <BASE/Icon2b.h>
#include <BASE/icon2bc.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>

typedef enum FontConstant {
    LARGE_FONT_HEIGHT_THRESHOLD = 14,
    FONT_DRAW_SCREEN_WIDTH      = 640,
    FONT_DRAW_SCREEN_HEIGHT     = 480,
    CENTER_DIVISOR              = 2,
    WRAP_HEIGHT_LINE_COUNT      = 2
} FontConstant;

VA(0x004c3620, 0xe5)
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
}

VA(0x004c3740, 0x5b)
font::~font() {
    gpResourceManager->Dispose(m_glyphIcon);
}

// Maps a CP1251 byte onto the font's glyph range. Retail compares the
// zero-extended byte, so the codes stay numeric: a signed char literal
// ('\xa8' == -88) would lower as cmp 0xffffffa8, not retail's cmp 0xa8.
VA(0x004c37a0, 0x52)
static i32 RemapCyrillicCharacter(i32 character) {
    if (character == 0xa8)              // 'Ё'
        return 0xa0;
    if (character == 0xb8)              // 'ё'
        return 0xc1;
    if (character < 0xc0)               // below 'А': not a Cyrillic letter
        return 0xa1;
    if (character < 0xe0)               // 'А'..'Я'
        return character - 0x40;
    return character - 0x3f;            // 'а'..'я'
}

VA(0x004c3800, 0x222)
void font::DrawStringExecute(
    char* str,
    i32 x,
    i32 y,
    FontDrawMode mode,
    i32 clipL,
    i32 clipT,
    i32 clipR,
    i32 clipB
) {
    char c = 0;
    i32 pos = x;
    i32 i = 0;
    while (str[i] != 0) {
        c = str[i];
        if (c == FONT_SPACER_CHAR) {
            pos += GetCharacterWidth(static_cast<u8>(str[i]));
            goto next;
        }
        if (c == '{') {
            m_suppressDraw = 1;
            goto next;
        }
        if (c == '}') {
            m_suppressDraw = 0;
            goto next;
        }
        c -= 'a' - 'A';
        if (c < 0 || c > FONT_GLYPH_FALLBACK)
            c = FONT_GLYPH_FALLBACK;
        if (c != 0) {
            if (mode == FONT_DRAW_DEFAULT && m_suppressDraw == 0)
                IconToBitmap(
                    m_glyphIcon,
                    gpWindowManager->m_screen,
                    pos,
                    y,
                    c,
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
                    c,
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
                    c,
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
                    c,
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
        pos += GetCharacterWidth(static_cast<u8>(str[i]));
    next:
        i++;
    }
}

VA(0x004c3a30, 0x3d)
void font::DrawString(char* s, i32 x, i32 y, FontDrawMode mode) {
    m_suppressDraw = 0;
    DrawStringExecute(s, x, y, mode, 0, 0, FONT_DRAW_SCREEN_WIDTH, FONT_DRAW_SCREEN_HEIGHT);
}

VA(0x004c3a70, 0xa8)
i32 font::GetCharacterWidth(u8 c) {
    i32 code = c;
    if (code == '{' || code == '}') {
        return 0;
    }
    if (code == ' ')
        code = 'i';
    if (code == '.')  // the width path measures '.' as the underscore glyph
        code = '_';
    if (code < ' '
        || (code > 0x7f && code < 0xc0 && code != 0xb8 && code != 0xa8)) {
        code = 0x7f;
    } else if (code > 0x7f) {
        code = RemapCyrillicCharacter(code);
    }
    code -= ' ';
    return reinterpret_cast<struct IconEntry*>(m_glyphIcon->m_data)[code].w + m_isLarge;
}

// Buka's Cyrillic line breaker. Retail compares the zero-extended byte, so
// like RemapCyrillicCharacter the CP1251 codes stay numeric: with a u8
// parameter a '\xe0' literal would compare -32 against 224 and never match.
static inline bool IsVowel(u8 c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y'
        || c == 0xe0 /* а */ || c == 0xe5 /* е */ || c == 0xb8 /* ё */
        || c == 0xe8 /* и */ || c == 0xee /* о */ || c == 0xf3 /* у */
        || c == 0xfb /* ы */ || c == 0xfd /* э */ || c == 0xfe /* ю */
        || c == 0xff /* я */
        || c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' || c == 'Y'
        || c == 0xc0 /* А */ || c == 0xc5 /* Е */ || c == 0xa8 /* Ё */
        || c == 0xc8 /* И */ || c == 0xce /* О */ || c == 0xd3 /* У */
        || c == 0xdb /* Ы */ || c == 0xdd /* Э */ || c == 0xde /* Ю */
        || c == 0xdf /* Я */;
}

VA(0x004c3b20, 0x1044)
void font::ExtractLine(
    char* text,
    char* line,
    i32* position,
    i32 maxWidth,
    i32* lineWidth,
    u8 lastLine
) {
    i32 width = 0;
    i32 pos = *position;
    i32 wordStart = *position;
    i32 wordWidth;
    i32 lineEnd;
    i32 lineEndWidth;

    if (lastLine != 0) {
        while (text[pos] != '\n' && text[pos] != 0) {
            width += GetCharacterWidth(text[pos]);
            line[pos - *position] = text[pos];
            pos++;
        }
        if (text[pos] == '\n') {
            line[pos - *position] = 0;
            *position = pos + 1;
            *lineWidth = width;
            return;
        }
        if (text[pos] == 0) {
            line[pos - *position] = 0;
            *position = pos;
            *lineWidth = width;
            return;
        }
    }

    while (1) {
        wordStart = pos;
        wordWidth = width;
        while (text[pos] != ' ' && text[pos] != '\n' && text[pos] != 0) {
            width += GetCharacterWidth(text[pos]);
            line[pos - *position] = text[pos];
            pos++;
        }
        if (maxWidth < width) {
            if (wordStart != *position) {
                if (line[wordStart - *position - 1] == ' ')
                    *lineWidth = wordWidth - GetCharacterWidth(' ');
                else
                    *lineWidth = wordWidth;
                line[wordStart - *position - 1] = 0;
                *position = wordStart;
                return;
            }
            lineEnd = pos;
            lineEndWidth = width;
            pos = pos - 2;
            while (width >= maxWidth
                   || (pos > wordStart + 1
                       && !IsVowel(text[pos])
                       && !((!IsVowel(text[pos + 1]) && text[pos] == text[pos + 1])
                            || text[pos] == '-'))) {
                width -= GetCharacterWidth(text[pos]);
                pos--;
            }
            if (pos <= wordStart + 1) {
                line[lineEnd - *position] = 0;
                *position = lineEnd + 1;
                *lineWidth = lineEndWidth;
                return;
            }
            if (IsVowel(text[pos])) {
                line[pos + 1 - *position] = '-';
                line[pos + 2 - *position] = 0;
                *lineWidth = width;
                *position = pos + 1;
                return;
            }
            if (text[pos] == '-') {
                line[pos + 1 - *position] = 0;
                *lineWidth = width;
                *position = pos + 1;
                return;
            }
            wordStart = pos;
            wordWidth = width;
            if (!IsVowel(text[pos])) {
                if (!IsVowel(text[pos + 1]) && text[pos] == text[pos + 1]) {
                    line[pos + 1 - *position] = '-';
                    line[pos + 2 - *position] = 0;
                    *lineWidth = width;
                    *position = pos + 1;
                    return;
                }
            }
        } else {
            if (text[pos] == '\n') {
                line[pos - *position] = 0;
                *position = pos + 1;
                *lineWidth = width;
                return;
            }
            if (text[pos] == ' ') {
                line[pos - *position] = ' ';
                width += GetCharacterWidth(' ');
                pos++;
            }
            if (text[pos] == 0) {
                line[pos - *position] = 0;
                *position = pos;
                *lineWidth = width;
                return;
            }
        }
    }
}

VA(0x004c4b70, 0x1f8)
void font::DrawBoundedString(
    char* str,
    i32 x,
    i32 y,
    i32 w,
    i32 h,
    FontDrawMode mode,
    FontAlignment align
) {
    i32 size = strlen(str);
    char space = ' ';
    i32 xPosition = 0;
    i32 yOffset = 0;
    i32 pos = 0;
    i32 wordBreak = 0;
    i32 lineWidth = 0;
    i32 savedWidth = 0;
    char* line = static_cast<char*>(H2_ALLOC(strlen(str) + 1));
    strcpy(line, str);
    FontDrawMode drawMode = mode;
    if (HAS(align, FONT_ALIGN_VERTICAL_CENTER)) {
        align -= FONT_ALIGN_VERTICAL_CENTER;
        i32 lineCount = LineLength(str, w);
        i32 totalH = lineCount * m_height;
        if (totalH < h)
            yOffset = (h - totalH) / CENTER_DIVISOR;
    }
    m_suppressDraw = 0;
    for (; size > pos && line[pos] != 0 && (yOffset + m_height <= h || yOffset == 0);
         yOffset += m_height) {
        if (h < yOffset + m_height * WRAP_HEIGHT_LINE_COUNT)
            ExtractLine(str, line, &pos, w, &lineWidth, 1);
        else
            ExtractLine(str, line, &pos, w, &lineWidth, 0);
        if (align == FONT_ALIGN_LEFT)
            xPosition = 0;
        else if (align == FONT_ALIGN_CENTER)
            xPosition = (w - lineWidth) / CENTER_DIVISOR + 1;
        else if (align == FONT_ALIGN_RIGHT)
            xPosition = w - lineWidth;
        DrawStringExecute(line, xPosition + x, yOffset + y, drawMode, x, y, w, h);
        lineWidth = 0;
    }
    H2_FREE(line);
}

#undef CENTER_DIVISOR
#undef WRAP_HEIGHT_LINE_COUNT

VA(0x004c4d70, 0xc7)
i32 font::LineLength(char* str, i32 maxW) {
    i32 size = strlen(str);
    char space = ' ';
    i32 count = 0;
    i32 pos = 0;
    i32 wordBreak = 0;
    i32 lineWidth = 0;
    i32 savedWidth = 0;
    char* line = static_cast<char*>(H2_ALLOC(strlen(str) + 1));
    while (pos < size && str[pos] != 0) {
        ExtractLine(str, line, &pos, maxW, &lineWidth, 0);
        count++;
        lineWidth = 0;
    }
    H2_FREE(line);
    return count;
}

VA(0x004c4e40, 0x8b)
i32 font::LineWidth(char* str) {
    i32 s = strlen(str);
    i32 q, u;
    i32 y = 0, t = 0, r = 0, x = 0, p = 0,
        w = 0;
    char* v = str;
    while (p < s && v[p] != 0) {
        while (v[p] != 0 && v[p] != '\n') {
            w += GetCharacterWidth(v[p]);
            p++;
        }
    }
    return w;
}
