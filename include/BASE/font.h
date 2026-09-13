#ifndef HOMM2_BASE_FONT_H
#define HOMM2_BASE_FONT_H

#include <va.h>
#include <BASE/resource.h>

class icon;

H2_ENUM_BEGIN(FontGlyphConstant)
    FONT_SPACER_CHAR    = 0x1f,
    FONT_GLYPH_FALLBACK = 0x5f
H2_ENUM_END(FontGlyphConstant)

H2_ENUM_CLASS_BEGIN_SPLIT(FontDrawMode, i16)
    FONT_DRAW_DARK_GRAY    = 0,
    FONT_DRAW_DEFAULT      = 1,
    FONT_DRAW_YELLOW       = 2,
    FONT_DRAW_DIMMED       = 3,
    FONT_DRAW_SCENARIO_WIN = 4
H2_ENUM_CLASS_END_SPLIT(FontDrawMode, i16)

H2_ENUM_CLASS_BEGIN_SPLIT(FontAlignment, i16)
    FONT_ALIGN_LEFT            = 0,
    FONT_ALIGN_CENTER          = 1,
    FONT_ALIGN_RIGHT           = 2,
    FONT_ALIGN_VERTICAL_CENTER = 4,
    FONT_ALIGN_CENTER_BOTH     = FONT_ALIGN_CENTER | FONT_ALIGN_VERTICAL_CENTER
H2_ENUM_CLASS_END_SPLIT(FontAlignment, i16)
H2_ENUM_FLAGS(FontAlignment)

#pragma pack(push, 1)
class font : public resource {
public:
    i32 m_height;
    b32 m_isLarge;
    b32 m_suppressDraw;
    icon* m_glyphIcon;
    font(u32l id);
    virtual ~font();

protected:
    void DrawStringExecute(H2_CONST char* str, i32 x, i32 y, FontDrawMode mode, i32 clipL, i32 clipT, i32 clipR, i32 clipB);
    void PolishBoundedTextLayout(H2_CONST char*, char*, u32*, i32, i32*, char);

public:
    void DrawString(H2_CONST char* s, i32 x, i32 y, FontDrawMode mode);
    i32 GetCharacterWidth(u8 c);
    void ExtractLine(H2_CONST char* text, char* line, i32* position, i32 maxWidth, i32* lineWidth, u8 lastLine);
    void DrawBoundedString(H2_CONST char* str, i32 x, i32 y, i32 w, i32 h, FontDrawMode mode, FontAlignment align);
    i32 LineLength(H2_CONST char* str, i32 maxW);
    i32 LineWidth(H2_CONST char* str);
};
#pragma pack(pop)
SIZE(font, 0x20);
#endif
