#ifndef HOMM2_BASE_FONT_H
#define HOMM2_BASE_FONT_H

#include <Ints.h>
#include <BASE/resource.h>

class icon;

typedef enum FontGlyphConstant {
    FONT_SPACER_CHAR    = 0x1f,
    FONT_GLYPH_FALLBACK = 0x5f
} FontGlyphConstant;

enum {
    FONT_DRAW_DARK_GRAY    = 0,
    FONT_DRAW_DEFAULT      = 1,
    FONT_DRAW_YELLOW       = 2,
    FONT_DRAW_DIMMED       = 3,
    FONT_DRAW_SCENARIO_WIN = 4
};
typedef i32 FontDrawMode;
enum {
    FONT_ALIGN_LEFT            = 0,
    FONT_ALIGN_CENTER          = 1,
    FONT_ALIGN_RIGHT           = 2,
    FONT_ALIGN_VERTICAL_CENTER = 4,
    FONT_ALIGN_CENTER_BOTH     = FONT_ALIGN_CENTER | FONT_ALIGN_VERTICAL_CENTER
};
typedef i32 FontAlignment;

#pragma pack(push, 1)
class font : public resource {
public:
    i32 m_height;
    b32 m_isLarge;
    b32 m_suppressDraw;
    icon* m_glyphIcon;
    font(u32l);
    virtual ~font();

protected:
    void DrawStringExecute(char*, i32, i32, FontDrawMode, i32, i32, i32, i32);
    void PolishBoundedTextLayout(char*, char*, u32*, i32, i32*, char);

public:
    void DrawString(char*, i32, i32, FontDrawMode);
    i32 GetCharacterWidth(u8);
    void ExtractLine(char*, char*, i32*, i32, i32*, u8);
    void DrawBoundedString(char*, i32, i32, i32, i32, FontDrawMode, FontAlignment);
    i32 LineLength(char*, i32);
    i32 LineWidth(char*);
};
#pragma pack(pop)
#endif
