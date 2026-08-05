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
    i32 c = 0;
    i32 pos = x;
    i32 i = 0;
    while (str[i] != 0) {
        c = static_cast<u8>(str[i]);
        if (c == FONT_SPACER_CHAR) {
            pos += GetCharacterWidth(c);
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
        // The same glyph remap GetCharacterWidth performs, open-coded: the
        // codes stay numeric so the byte compares zero-extend (see
        // RemapCyrillicCharacter).
        if (c < ' ' || (c > 0x7f && c < 0xc0 && c != 0xb8 && c != 0xa8)) {
            c = 0x7f;
        } else if (c > 0x7f) {
            c = RemapCyrillicCharacter(c);
        }
        c -= ' ';
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
        pos += GetCharacterWidth(str[i]);
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

static inline bool IsHyphen(u8 c) {
    return c == '-';
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
    i32 curPos = *position;
    i32 wStart = *position;
    i32 savedWidth;
    i32 lastEnd;
    i32 lastWidth;

    if (lastLine != 0) {
        while (text[curPos] != '\n' && text[curPos] != 0) {
            width += GetCharacterWidth(text[curPos]);
            line[curPos - *position] = text[curPos];
            curPos++;
        }
        if (text[curPos] == '\n') {
            line[curPos - *position] = 0;
            *position = curPos + 1;
            *lineWidth = width;
            return;
        }
        if (text[curPos] == 0) {
            line[curPos - *position] = 0;
            *position = curPos;
            *lineWidth = width;
            return;
        }
    }

    while (1) {
        wStart = curPos;
        savedWidth = width;
        while (text[curPos] != ' ' && text[curPos] != '\n' && text[curPos] != 0) {
            width += GetCharacterWidth(text[curPos]);
            line[curPos - *position] = text[curPos];
            curPos++;
        }
        if (width > maxWidth) {
            if (wStart != *position) {
                if (line[wStart - *position - 1] == ' ')
                    *lineWidth = savedWidth - GetCharacterWidth(' ');
                else
                    *lineWidth = savedWidth;
                line[wStart - *position - 1] = 0;
                *position = wStart;
                return;
            }
            lastEnd = curPos;
            lastWidth = width;
            curPos = curPos - 2;
            while (width >= maxWidth
                   || (curPos > wStart + 1
                       && !IsVowel(text[curPos])
                       && !((!IsVowel(text[curPos + 1]) && text[curPos] == text[curPos + 1])
                            || IsHyphen(text[curPos])))) {
                width -= GetCharacterWidth(text[curPos]);
                curPos--;
            }
            if (curPos <= wStart + 1) {
                line[lastEnd - *position] = 0;
                *position = lastEnd + 1;
                *lineWidth = lastWidth;
                return;
            }
            if (IsVowel(text[curPos])) {
                line[curPos - *position + 1] = '-';
                line[curPos - *position + 2] = 0;
                *lineWidth = width;
                *position = curPos + 1;
                return;
            }
            if (IsHyphen(text[curPos])) {
                line[curPos - *position + 1] = 0;
                *lineWidth = width;
                *position = curPos + 1;
                return;
            }
            if (!IsVowel(text[curPos])) {
                if (!IsVowel(text[curPos + 1]) && text[curPos] == text[curPos + 1]) {
                    line[curPos - *position + 1] = '-';
                    line[curPos - *position + 2] = 0;
                    *lineWidth = width;
                    *position = curPos + 1;
                    return;
                }
            }
        } else {
            if (text[curPos] == '\n') {
                line[curPos - *position] = 0;
                *position = curPos + 1;
                *lineWidth = width;
                return;
            }
            if (text[curPos] == ' ') {
                line[curPos - *position] = ' ';
                width += GetCharacterWidth(' ');
                curPos++;
            }
            if (text[curPos] == 0) {
                line[curPos - *position] = 0;
                *position = curPos;
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
    // blank, lastPos, spaceWidth, wordWidth and prevPos are leftovers of the
    // line-breaking block this function shares with LineLength; retail keeps
    // their frame slots (lastPos never even gets a store) and their zero
    // initializers, so the declarations are load-bearing.
    i32 len = strlen(str);
    char blank = ' ';
    i32 lastPos;
    i32 xPosition = 0;
    i32 yPosition = 0;
    i32 pos = 0;
    i32 spaceWidth = 0;
    i32 wordWidth = 0;
    i32 lw = 0;
    i32 prevPos = 0;
    char* line = static_cast<char*>(H2_ALLOC(strlen(str) + 1));
    strcpy(line, str);
    FontDrawMode drawMode = mode;
    if (HAS(align, FONT_ALIGN_VERTICAL_CENTER)) {
        align -= FONT_ALIGN_VERTICAL_CENTER;
        i32 lineCount = LineLength(str, w);
        i32 totalH = lineCount * m_height;
        if (totalH < h)
            yPosition = (h - totalH) / CENTER_DIVISOR;
    }
    m_suppressDraw = 0;
    while (pos < len && line[pos] != 0 && (yPosition + m_height <= h || yPosition == 0)) {
        if (yPosition + m_height * WRAP_HEIGHT_LINE_COUNT > h)
            ExtractLine(str, line, &pos, w, &lw, 1);
        else
            ExtractLine(str, line, &pos, w, &lw, 0);
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
        }
        DrawStringExecute(line, xPosition + x, yPosition + y, drawMode, x, y, w, h);
        yPosition += m_height;
        lw = 0;
    }
    H2_FREE(line);
}

#undef CENTER_DIVISOR
#undef WRAP_HEIGHT_LINE_COUNT

VA(0x004c4d70, 0xc7)
i32 font::LineLength(char* str, i32 maxW) {
    // Same shared line-breaking declaration block as DrawBoundedString: blank,
    // spaceWidth, wordWidth and prevPos are unused here but hold retail frame
    // slots and emit their initializers.
    i32 len = strlen(str);
    char blank = ' ';
    i32 count = 0;
    i32 pos = 0;
    i32 spaceWidth = 0;
    i32 wordWidth = 0;
    i32 lw = 0;
    i32 prevPos = 0;
    char* line = static_cast<char*>(H2_ALLOC(strlen(str) + 1));
    while (pos < len && str[pos] != 0) {
        ExtractLine(str, line, &pos, maxW, &lw, 0);
        count++;
        lw = 0;
    }
    H2_FREE(line);
    return count;
}

VA(0x004c4e40, 0x8b)
i32 font::LineWidth(char* str) {
    i32 s = strlen(str);
    i32 idx = 0, w = 0;
    char* p = str;
    while (idx < s && p[idx] != 0) {
        while (p[idx] != 0 && p[idx] != '\n') {
            w += GetCharacterWidth(p[idx]);
            idx++;
        }
    }
    return w;
}

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(font, 0x004ea9ac)
