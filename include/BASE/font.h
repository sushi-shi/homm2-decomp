#ifndef HOMM2_BASE_FONT_H
#define HOMM2_BASE_FONT_H
#include <va.h>
#include <BASE/resource.h>
class icon;

H2_ENUM_BEGIN(FontGlyphConstant)
    FONT_SPACER_CHAR = 0x1f, // advances the pen without drawing a glyph
    FONT_GLYPH_FALLBACK = 0x5f
// out-of-range characters render this glyph
H2_ENUM_END(FontGlyphConstant)

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
    void
    DrawStringExecute(char*, i32, i32, i32, i32, i32, i32, i32);
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
