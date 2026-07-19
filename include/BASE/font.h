#ifndef HOMM2_BASE_FONT_H
#define HOMM2_BASE_FONT_H

#include <va.h>
#include <BASE/resource.h>

class icon;

H2_ENUM_BEGIN(FontGlyphConstant)
    FONT_SPACER_CHAR    = 0x1f, // Advances the pen without drawing a glyph.
    FONT_GLYPH_FALLBACK = 0x5f  // Out-of-range characters render this glyph.
H2_ENUM_END(FontGlyphConstant)

// Strict builds type-check the shared domains; production expansion keeps VC4.2 enum state neutral.
#ifdef HOMM2_STRICT_ENUM_TYPES
H2_ENUM_BEGIN(FontDrawMode)
    FONT_DRAW_DEFAULT      = 1,
    FONT_DRAW_YELLOW       = 2,
    FONT_DRAW_DARK_GRAY    = 3,
    FONT_DRAW_SCENARIO_WIN = 4
H2_ENUM_END(FontDrawMode)

H2_ENUM_BEGIN(FontAlignment)
    FONT_ALIGN_LEFT            = 0,
    FONT_ALIGN_CENTER          = 1,
    FONT_ALIGN_RIGHT           = 2,
    FONT_ALIGN_VERTICAL_CENTER = 4,
    FONT_ALIGN_CENTER_BOTH     = FONT_ALIGN_CENTER | FONT_ALIGN_VERTICAL_CENTER
H2_ENUM_END(FontAlignment)
#else
#define FONT_DRAW_DEFAULT 1
#define FONT_DRAW_YELLOW 2
#define FONT_DRAW_DARK_GRAY 3
#define FONT_DRAW_SCENARIO_WIN 4
#define FONT_ALIGN_LEFT 0
#define FONT_ALIGN_CENTER 1
#define FONT_ALIGN_RIGHT 2
#define FONT_ALIGN_VERTICAL_CENTER 4
#define FONT_ALIGN_CENTER_BOTH (FONT_ALIGN_CENTER | FONT_ALIGN_VERTICAL_CENTER)
#endif

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
    void DrawStringExecute(char*, i32, i32, i32, i32, i32, i32, i32);

public:
    void DrawString(char*, i32, i32, i32);
    i32 GetCharacterWidth(u8);
    void DrawBoundedString(char*, i32, i32, i32, i32, i32, i32);
    i32 LineLength(char*, i32);
    i32 LineWidth(char*);
};
#pragma pack(pop)
SIZE(font, 0x20);
#endif
