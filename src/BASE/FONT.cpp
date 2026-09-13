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
#include <BASE/display.h>

typedef enum FontConstant {
    LARGE_FONT_HEIGHT_THRESHOLD = 14,
    CENTER_DIVISOR              = 2,
    WRAP_HEIGHT_LINE_COUNT      = 2
} FontConstant;

#if H2_RETAIL_COMPILER
#define fileDescriptor h
#endif
VA(0x004c3620, 0xe5)
font::font(u32l id) : resource(RESOURCE_CATEGORY_FONT, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    char name[RESOURCE_MANAGER_READ13_BYTES];
    gpResourceManager->PointToFile(id);
    m_height = gpResourceManager->ReadWord();
    i32 H2_UNUSED(fileDescriptor) = gpResourceManager->ReadWord();
    if (m_height >= LARGE_FONT_HEIGHT_THRESHOLD)
        m_isLarge = true;
    else
        m_isLarge = false;
    gpResourceManager->Read13(name);
    gbLoadingMonoIcon = true;
    m_glyphIcon = gpResourceManager->GetIcon(name);
    gbLoadingMonoIcon = false;
}
#if H2_RETAIL_COMPILER
#undef fileDescriptor
#endif

VA(0x004c3740, 0x5b)
font::~font() {
    gpResourceManager->Dispose(m_glyphIcon);
}

// Maps a CP1251 byte onto the font's glyph range. Retail compares the
// zero-extended byte, so the codes stay numeric: a signed char literal
// (a signed CP1251 'Ё' byte == -88) would lower as cmp 0xffffffa8, not retail's cmp 0xa8.
VA(0x004c37a0, 0x52)
i32 RemapCyrillicCharacter(i32 character) {
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

#if H2_RETAIL_COMPILER
#define character c
#define position pos
#define text str
#endif
VA(0x004c3800, 0x222)
void font::DrawStringExecute(
    H2_CONST char* text,
    i32 x,
    i32 y,
    FontDrawMode mode,
    i32 clipL,
    i32 clipT,
    i32 clipR,
    i32 clipB
) {
    i32 character = 0;
    i32 position = x;
    i32 i = 0;
    while (text[i] != 0) {
        character = static_cast<u8>(text[i]);
        if (character == FONT_SPACER_CHAR) {
            position += GetCharacterWidth(character);
            goto next;
        }
        if (character == '{') {
            m_suppressDraw = true;
            goto next;
        }
        if (character == '}') {
            m_suppressDraw = false;
            goto next;
        }
        // The same glyph remap GetCharacterWidth performs, open-coded: the
        // codes stay numeric so the byte compares zero-extend (see
        // RemapCyrillicCharacter).
        if (character < ' ' || (character > 0x7f && character < 0xc0 && character != 0xb8 && character != 0xa8)) {
            character = 0x7f;
        } else if (character > 0x7f) {
            character = RemapCyrillicCharacter(character);
        }
        character -= ' ';
        if (character != 0) {
            if (mode == FONT_DRAW_DEFAULT && m_suppressDraw == 0)
                IconToBitmap(
                    m_glyphIcon,
                    gpWindowManager->m_screen,
                    position,
                    y,
                    character,
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
                    position,
                    y,
                    character,
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
                    position,
                    y,
                    character,
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
                    position,
                    y,
                    character,
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
        position += GetCharacterWidth(text[i]);
    next:
        i++;
    }
}
#if H2_RETAIL_COMPILER
#undef character
#undef position
#undef text
#endif

#if H2_RETAIL_COMPILER
#define text s
#endif
VA(0x004c3a30, 0x3d)
void font::DrawString(H2_CONST char* text, i32 x, i32 y, FontDrawMode mode) {
    m_suppressDraw = false;
    DrawStringExecute(text, x, y, mode, 0, 0, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT);
}
#if H2_RETAIL_COMPILER
#undef text
#endif

#if H2_RETAIL_COMPILER
#define character c
#endif
VA(0x004c3a70, 0xa8)
i32 font::GetCharacterWidth(u8 character) {
    i32 code = character;
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
#if H2_RETAIL_COMPILER
#undef character
#endif

// Buka's Cyrillic line breaker. Retail compares the zero-extended byte, so
// like RemapCyrillicCharacter the CP1251 codes stay numeric: with a u8
// parameter a signed CP1251 'а' byte would compare -32 against 224 and never match.
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

#if H2_RETAIL_COMPILER
#define curPosition curPos
#endif
VA(0x004c3b20, 0x1044)
void font::ExtractLine(
    H2_CONST char* text,
    char* line,
    i32* position,
    i32 maxWidth,
    i32* lineWidth,
    u8 lastLine
) {
    i32 width = 0;
    i32 curPosition = *position;
    i32 wStart = *position;
    i32 savedWidth;
    i32 lastEnd;
    i32 lastWidth;

    if (lastLine != 0) {
        while (text[curPosition] != '\n' && text[curPosition] != 0) {
            width += GetCharacterWidth(text[curPosition]);
            line[curPosition - *position] = text[curPosition];
            curPosition++;
        }
        if (text[curPosition] == '\n') {
            line[curPosition - *position] = 0;
            *position = curPosition + 1;
            *lineWidth = width;
            return;
        }
        if (text[curPosition] == 0) {
            line[curPosition - *position] = 0;
            *position = curPosition;
            *lineWidth = width;
            return;
        }
    }

    while (1) {
        wStart = curPosition;
        savedWidth = width;
        while (text[curPosition] != ' ' && text[curPosition] != '\n' && text[curPosition] != 0) {
            width += GetCharacterWidth(text[curPosition]);
            line[curPosition - *position] = text[curPosition];
            curPosition++;
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
            lastEnd = curPosition;
            lastWidth = width;
            curPosition = curPosition - 2;
            while (width >= maxWidth
                   || (curPosition > wStart + 1
                       && !IsVowel(text[curPosition])
                       && !((!IsVowel(text[curPosition + 1]) && text[curPosition] == text[curPosition + 1])
                            || IsHyphen(text[curPosition])))) {
                width -= GetCharacterWidth(text[curPosition]);
                curPosition--;
            }
            if (curPosition <= wStart + 1) {
                line[lastEnd - *position] = 0;
                *position = lastEnd + 1;
                *lineWidth = lastWidth;
                return;
            }
            if (IsVowel(text[curPosition])) {
                line[curPosition - *position + 1] = '-';
                line[curPosition - *position + 2] = 0;
                *lineWidth = width;
                *position = curPosition + 1;
                return;
            }
            if (IsHyphen(text[curPosition])) {
                line[curPosition - *position + 1] = 0;
                *lineWidth = width;
                *position = curPosition + 1;
                return;
            }
            if (!IsVowel(text[curPosition])) {
                if (!IsVowel(text[curPosition + 1]) && text[curPosition] == text[curPosition + 1]) {
                    line[curPosition - *position + 1] = '-';
                    line[curPosition - *position + 2] = 0;
                    *lineWidth = width;
                    *position = curPosition + 1;
                    return;
                }
            }
        } else {
            if (text[curPosition] == '\n') {
                line[curPosition - *position] = 0;
                *position = curPosition + 1;
                *lineWidth = width;
                return;
            }
            if (text[curPosition] == ' ') {
                line[curPosition - *position] = ' ';
                width += GetCharacterWidth(' ');
                curPosition++;
            }
            if (text[curPosition] == 0) {
                line[curPosition - *position] = 0;
                *position = curPosition;
                *lineWidth = width;
                return;
            }
        }
    }
}
#if H2_RETAIL_COMPILER
#undef curPosition
#endif

#if H2_RETAIL_COMPILER
#define height h
#define lastPosition lastPos
#define length len
#define lineWidth lw
#define position pos
#define prevPosition prevPos
#define text str
#define width w
#endif
VA(0x004c4b70, 0x1f8)
void font::DrawBoundedString(
    H2_CONST char* text,
    i32 x,
    i32 y,
    i32 width,
    i32 height,
    FontDrawMode mode,
    FontAlignment align
) {
    // blank, lastPosition, spaceWidth, wordWidth and prevPosition are leftovers of the
    // line-breaking block this function shares with LineLength; retail keeps
    // their frame slots (lastPosition never even gets a store) and their zero
    // initializers, so the declarations are load-bearing.
    i32 length = strlen(text);
    char H2_UNUSED(blank) = ' ';
    i32 H2_UNUSED(lastPosition);
    i32 xPosition = 0;
    i32 yPosition = 0;
    i32 position = 0;
    i32 H2_UNUSED(spaceWidth) = 0;
    i32 H2_UNUSED(wordWidth) = 0;
    i32 lineWidth = 0;
    i32 H2_UNUSED(prevPosition) = 0;
    char* line = static_cast<char*>(H2_ALLOC(strlen(text) + 1));
    strcpy(line, text);
    FontDrawMode drawMode = mode;
    if (HAS(align, FONT_ALIGN_VERTICAL_CENTER)) {
        align -= FONT_ALIGN_VERTICAL_CENTER;
        i32 lineCount = LineLength(text, width);
        i32 totalH = lineCount * m_height;
        if (totalH < height)
            yPosition = (height - totalH) / CENTER_DIVISOR;
    }
    m_suppressDraw = false;
    while (position < length && line[position] != 0 && (yPosition + m_height <= height || yPosition == 0)) {
        if (yPosition + m_height * WRAP_HEIGHT_LINE_COUNT > height)
            ExtractLine(text, line, &position, width, &lineWidth, 1);
        else
            ExtractLine(text, line, &position, width, &lineWidth, 0);
        switch (align) {
        case FONT_ALIGN_LEFT:
            xPosition = 0;
            break;
        case FONT_ALIGN_CENTER:
            xPosition = (width - lineWidth) / CENTER_DIVISOR + 1;
            break;
        case FONT_ALIGN_RIGHT:
            xPosition = width - lineWidth;
            break;
        }
        DrawStringExecute(line, xPosition + x, yPosition + y, drawMode, x, y, width, height);
        yPosition += m_height;
        lineWidth = 0;
    }
    H2_FREE(line);
}
#if H2_RETAIL_COMPILER
#undef height
#undef lastPosition
#undef length
#undef lineWidth
#undef position
#undef prevPosition
#undef text
#undef width
#endif

#undef CENTER_DIVISOR
#undef WRAP_HEIGHT_LINE_COUNT

#if H2_RETAIL_COMPILER
#define length len
#define lineWidth lw
#define position pos
#define prevPosition prevPos
#define text str
#endif
VA(0x004c4d70, 0xc7)
i32 font::LineLength(H2_CONST char* text, i32 maxW) {
    // Same shared line-breaking declaration block as DrawBoundedString: blank,
    // spaceWidth, wordWidth and prevPosition are unused here but hold retail frame
    // slots and emit their initializers.
    i32 length = strlen(text);
    char H2_UNUSED(blank) = ' ';
    i32 count = 0;
    i32 position = 0;
    i32 H2_UNUSED(spaceWidth) = 0;
    i32 H2_UNUSED(wordWidth) = 0;
    i32 lineWidth = 0;
    i32 H2_UNUSED(prevPosition) = 0;
    char* line = static_cast<char*>(H2_ALLOC(strlen(text) + 1));
    while (position < length && text[position] != 0) {
        ExtractLine(text, line, &position, maxW, &lineWidth, 0);
        count++;
        lineWidth = 0;
    }
    H2_FREE(line);
    return count;
}
#if H2_RETAIL_COMPILER
#undef length
#undef lineWidth
#undef position
#undef prevPosition
#undef text
#endif

#if H2_RETAIL_COMPILER
#define character p
#define characterWidth w
#define index idx
#define text str
#define width s
#endif
VA(0x004c4e40, 0x8b)
i32 font::LineWidth(H2_CONST char* text) {
    i32 width = strlen(text);
    i32 index = 0, characterWidth = 0;
    H2_CONST char* character = text;
    while (index < width && character[index] != 0) {
        while (character[index] != 0 && character[index] != '\n') {
            characterWidth += GetCharacterWidth(character[index]);
            index++;
        }
    }
    return characterWidth;
}
#if H2_RETAIL_COMPILER
#undef character
#undef characterWidth
#undef index
#undef text
#undef width
#endif

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(font, 0x004ea9ac)
