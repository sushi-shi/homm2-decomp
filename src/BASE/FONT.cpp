#include <va.h>
#include <string.h>
#include <BASE/font.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/IconEntry.h>
#include <BASE/Icon2b.h>
#include <BASE/icon2bc.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>

H2_ENUM_BEGIN(FontConstant)
    LARGE_FONT_HEIGHT_THRESHOLD = 14,
    FONT_DRAW_SCREEN_WIDTH      = 640,
    FONT_DRAW_SCREEN_HEIGHT     = 480,
    CENTER_DIVISOR              = 2,
    WRAP_HEIGHT_LINE_COUNT      = 2
H2_ENUM_END(FontConstant)

VA(0x004c6fd0, 0xc8)
font::font(u32l id) : resource(RESOURCE_CATEGORY_FONT, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    gpResourceManager->PointToFile(id);
    m_height = gpResourceManager->ReadWord();
    i32 h = gpResourceManager->ReadWord();
    if (m_height >= LARGE_FONT_HEIGHT_THRESHOLD)
        m_isLarge = 1;
    else
        m_isLarge = 0;
    char fname[RESOURCE_MANAGER_READ13_BYTES];
    gpResourceManager->Read13(reinterpret_cast<i8*>(fname));
    gbLoadingMonoIcon = true;
    m_glyphIcon = gpResourceManager->GetIcon(fname);
    gbLoadingMonoIcon = false;
}

VA(0x004c70e0, 0x39)
font::~font() {
    gpResourceManager->Dispose(m_glyphIcon);
}

VA(0x004c7120, 0x24a)
void font::DrawStringExecute(
    char* str,
    i32 x,
    i32 y,
    i32 mode,
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
                    1,
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
                    1,
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
                    1,
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
                    1,
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

VA(0x004c7370, 0x48)
void font::DrawString(char* s, i32 x, i32 y, i32 mode) {
    m_suppressDraw = 0;
    DrawStringExecute(s, x, y, mode, 0, 0, FONT_DRAW_SCREEN_WIDTH, FONT_DRAW_SCREEN_HEIGHT);
}

VA(0x004c73c0, 0xaf)
i32 font::GetCharacterWidth(u8 c) {
    if (c == '{' || c == '}') {
        return 0;
    } else {
        if (c == ' ')
            c = 'i';
        if (c == FONT_SPACER_CHAR)
            c = '_';
        c -= ' ';
        if (c < 0 || c > FONT_GLYPH_FALLBACK)
            c = FONT_GLYPH_FALLBACK;
        return m_glyphIcon->Entries()[c].w + m_isLarge;
    }
}

VA(0x004c7470, 0x313)
void font::DrawBoundedString(char* str, i32 x, i32 y, i32 w, i32 h, i32 mode, i32 align) {
    i32 size = strlen(str);
    u8* glyphPos = m_glyphIcon->m_data;
    char space9 = ' ';
    i32 xPosition = 0;
    i32 yOffC = 0;
    char savedChar;
    i32 lineStartD = 0;
    i32 lineEnd1 = 0;
    i32 v1;
    i32 idx = 0;
    i32 lineWidth3 = 0;
    i32 wordBreak0 = 0;
    char* text2 = str;
    i32 drawMode2 = mode;
    if (align & FONT_ALIGN_VERTICAL_CENTER) {
        align -= FONT_ALIGN_VERTICAL_CENTER;
        i32 lineCount = LineLength(str, w);
        i32 totalH = m_height * lineCount;
        if (totalH < h)
            yOffC = (h - totalH) / CENTER_DIVISOR;
    }
    m_suppressDraw = 0;
    while (size > idx && text2[idx] != 0 && (m_height + yOffC <= h || yOffC == 0)) {
        while (text2[idx] != 0 && text2[idx] != '\n' && lineWidth3 <= w) {
            lineWidth3 += GetCharacterWidth(text2[idx]);
            idx++;
        }
        i32 savedWidth = lineWidth3;
        if (w < lineWidth3) {
            idx--;
            wordBreak0 = 0;
            while (text2[idx] != ' ' && OD_STEER(idx) >= lineStartD) {
                lineWidth3 -= GetCharacterWidth(text2[idx]);
                if (m_height * WRAP_HEIGHT_LINE_COUNT + yOffC > h && lineWidth3 < w)
                    break;
                if (wordBreak0 == 0 && lineWidth3 < w)
                    wordBreak0 = idx;
                idx--;
            }
            if (OD_STEER(idx) <= lineStartD) {
                idx = wordBreak0;
                lineWidth3 = savedWidth;
            }
            if (text2[idx] == ' ')
                lineWidth3 -= GetCharacterWidth(text2[idx]);
        }
        lineEnd1 = idx;
        savedChar = text2[lineEnd1];
        text2[lineEnd1] = 0;
        switch (align) {
            case FONT_ALIGN_LEFT:
                xPosition = 0;
                break;
            case FONT_ALIGN_CENTER:
                xPosition = (w - lineWidth3) / CENTER_DIVISOR;
                break;
            case FONT_ALIGN_RIGHT:
                xPosition = w - lineWidth3;
                break;
        }
        DrawStringExecute(text2 + lineStartD, x + xPosition, y + yOffC, drawMode2, x, y, w, h);
        text2[lineEnd1] = savedChar;
        yOffC += m_height;
        lineStartD = lineEnd1 + 1;
        idx = lineStartD;
        lineWidth3 = 0;
    }
}

#undef CENTER_DIVISOR
#undef WRAP_HEIGHT_LINE_COUNT

VA(0x004c7790, 0x1b3)
i32 font::LineLength(char* str, i32 maxW) {
    i32 s = strlen(str);
    char aa = ' ';
    i32 z = 0, t = 0, r = 0, y = 0, p = 0, u = 0, x = 0, gap = 0;
    char* w = str;
    i32 q, v;
    while (p < s && w[p] != 0) {
        while (w[p] != 0 && w[p] != '\n' && OD_STEER(x) <= maxW) {
            x += GetCharacterWidth(w[p]);
            p++;
        }
        if (OD_STEER(x) > maxW) {
            p--;
            gap = 0;
            while (w[p] != ' ' && p >= r) {
                x -= GetCharacterWidth(w[p]);
                if (gap == 0 && OD_STEER(x) < maxW)
                    gap = p;
                p--;
            }
            if (p <= r)
                p = gap;
            if (w[p] == ' ')
                x -= GetCharacterWidth(w[p]);
        }
        y = p;
        z++;
        r = y + 1;
        p = r;
        x = 0;
    }
    return z;
}

VA(0x004c7950, 0xc4)
i32 font::LineWidth(char* str) {
    i32 s = strlen(str);
    i32 q, u;
    i32 y = 0, t = 0, r = 0, x = 0, p = 0,
        w = 0;
    char* v = str;
    while (OD_STEER(p) < s && v[p] != 0) {
        while (v[p] != 0 && v[p] != '\n') {
            w += GetCharacterWidth(v[p]);
            p++;
        }
    }
    return w;
}


VTBL(font, 0x004eb9e4);
