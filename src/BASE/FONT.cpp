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
#include <SOURCE/KB.h>

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
    i32 fileDescriptor [[maybe_unused]] = gpResourceManager->ReadWord();
    if (m_height >= LARGE_FONT_HEIGHT_THRESHOLD)
        m_isLarge = true;
    else
        m_isLarge = false;
    gpResourceManager->Read13(reinterpret_cast<i8*>(name));
    gbLoadingMonoIcon = true;
    m_glyphIcon = gpResourceManager->GetIcon(name);
    gbLoadingMonoIcon = false;
}

font::~font() {
    gpResourceManager->Dispose(m_glyphIcon);
}


i32 RemapCyrillicCharacter(i32 character) {
    if (character == 0xa8)
        return 0xa0;
    if (character == 0xb8)
        return 0xc1;
    if (character < 0xc0)
        return 0xa1;
    if (character < 0xe0)
        return character - 0x40;
    return character - 0x3f;
}

void font::DrawStringExecute(
    const char* text,
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

void font::DrawString(const char* text, i32 x, i32 y, FontDrawMode mode) {
    m_suppressDraw = false;
    DrawStringExecute(text, x, y, mode, 0, 0, FONT_DRAW_SCREEN_WIDTH, FONT_DRAW_SCREEN_HEIGHT);
}

i32 font::GetCharacterWidth(u8 character) {
    i32 code = character;
    if (code == '{' || code == '}') {
        return 0;
    }
    if (code == ' ')
        code = 'i';
    if (code == '.')
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


static inline bool IsVowel(u8 c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y'
        || c == 0xe0   || c == 0xe5   || c == 0xb8
        || c == 0xe8   || c == 0xee   || c == 0xf3
        || c == 0xfb   || c == 0xfd   || c == 0xfe
        || c == 0xff
        || c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' || c == 'Y'
        || c == 0xc0   || c == 0xc5   || c == 0xa8
        || c == 0xc8   || c == 0xce   || c == 0xd3
        || c == 0xdb   || c == 0xdd   || c == 0xde
        || c == 0xdf  ;
}

static inline bool IsHyphen(u8 c) {
    return c == '-';
}

void font::ExtractLine(
    const char* text,
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

void font::DrawBoundedString(
    const char* text,
    i32 x,
    i32 y,
    i32 width,
    i32 height,
    FontDrawMode mode,
    FontAlignment align
) {


    i32 length = strlen(text);
    char blank [[maybe_unused]] = ' ';
    i32 lastPosition [[maybe_unused]];
    i32 xPosition = 0;
    i32 yPosition = 0;
    i32 position = 0;
    i32 spaceWidth [[maybe_unused]] = 0;
    i32 wordWidth [[maybe_unused]] = 0;
    i32 lineWidth = 0;
    i32 prevPosition [[maybe_unused]] = 0;
    char* line = static_cast<char*>(H2_ALLOC(strlen(text) + 1));
    strcpy(line, text);
    FontDrawMode drawMode = mode;
    if ((((align) & (FONT_ALIGN_VERTICAL_CENTER)))) {
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

#undef CENTER_DIVISOR
#undef WRAP_HEIGHT_LINE_COUNT

i32 font::LineLength(const char* text, i32 maxW) {


    i32 length = strlen(text);
    char blank [[maybe_unused]] = ' ';
    i32 count = 0;
    i32 position = 0;
    i32 spaceWidth [[maybe_unused]] = 0;
    i32 wordWidth [[maybe_unused]] = 0;
    i32 lineWidth = 0;
    i32 prevPosition [[maybe_unused]] = 0;
    char* line = static_cast<char*>(H2_ALLOC(strlen(text) + 1));
    while (position < length && text[position] != 0) {
        ExtractLine(text, line, &position, maxW, &lineWidth, 0);
        count++;
        lineWidth = 0;
    }
    H2_FREE(line);
    return count;
}

i32 font::LineWidth(const char* text) {
    i32 width = strlen(text);
    i32 index = 0, characterWidth = 0;
    const char* character = text;
    while (index < width && character[index] != 0) {
        while (character[index] != 0 && character[index] != '\n') {
            characterWidth += GetCharacterWidth(character[index]);
            index++;
        }
    }
    return characterWidth;
}
