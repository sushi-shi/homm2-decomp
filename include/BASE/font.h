#ifndef HOMM2_BASE_FONT_H
#define HOMM2_BASE_FONT_H

#include <Ints.h>
#include <BASE/resource.h>

class icon;

typedef enum FontGlyphConstant {
    FONT_SPACER_CHAR    = 0x1f,
    FONT_GLYPH_FALLBACK = 0x5f
} FontGlyphConstant;

enum class FontDrawMode : i16 {
    FONT_DRAW_DARK_GRAY    = 0,
    FONT_DRAW_DEFAULT      = 1,
    FONT_DRAW_YELLOW       = 2,
    FONT_DRAW_DIMMED       = 3,
    FONT_DRAW_SCENARIO_WIN = 4
};
using enum FontDrawMode;

enum class FontAlignment : i16 {
    FONT_ALIGN_LEFT            = 0,
    FONT_ALIGN_CENTER          = 1,
    FONT_ALIGN_RIGHT           = 2,
    FONT_ALIGN_VERTICAL_CENTER = 4,
    FONT_ALIGN_CENTER_BOTH     = FONT_ALIGN_CENTER | FONT_ALIGN_VERTICAL_CENTER
};
using enum FontAlignment;
ENABLE_ENUM_FLAGS(FontAlignment)

#pragma pack(push, 1)
class font : public resource {
public:
    i32 m_height;
    i32 m_isLarge;
    i32 m_suppressDraw;
    icon* m_glyphIcon;
    font(u32l);
    virtual ~font();

protected:
    void DrawStringExecute(const char*, i32, i32, FontDrawMode, i32, i32, i32, i32);
    void PolishBoundedTextLayout(char*, char*, u32*, i32, i32*, char);

public:
    void DrawString(const char*, i32, i32, FontDrawMode);
    i32 GetCharacterWidth(u8);
    void ExtractLine(const char*, char*, i32*, i32, i32*, u8);
    void DrawBoundedString(const char*, i32, i32, i32, i32, FontDrawMode, FontAlignment);
    i32 LineLength(const char*, i32);
    i32 LineWidth(const char*);
};
#pragma pack(pop)
#endif
