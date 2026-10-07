#ifndef HOMM2_BASE_FONT_H
#define HOMM2_BASE_FONT_H

#include <va.h>
#include <BASE/resource.h>

class icon;

H2_ENUM_BEGIN(FontGlyphConstant)
    FONT_SPACER_CHAR    = 0x1f,
    FONT_GLYPH_FALLBACK = 0x5f
H2_ENUM_END(FontGlyphConstant)

// The CP1251 (Windows Cyrillic) byte codes the Buka font remap and line
// breaker test. Callers compare the zero-extended byte.
H2_ENUM_BEGIN(Cp1251Code)
    CP1251_ASCII_LAST       = 0x7f,
    CP1251_ASCII_COUNT      = 0x80,
    CP1251_CAPITAL_IO       = 0xa8, // 'Ё'
    CP1251_SMALL_IO         = 0xb8, // 'ё'
    CP1251_CAPITAL_A        = 0xc0, // 'А', the first Cyrillic capital
    CP1251_CAPITAL_IE       = 0xc5, // 'Е'
    CP1251_CAPITAL_I        = 0xc8, // 'И'
    CP1251_CAPITAL_O        = 0xce, // 'О'
    CP1251_CAPITAL_U        = 0xd3, // 'У'
    CP1251_CAPITAL_YERU     = 0xdb, // 'Ы'
    CP1251_CAPITAL_E        = 0xdd, // 'Э'
    CP1251_CAPITAL_YU       = 0xde, // 'Ю'
    CP1251_CAPITAL_YA       = 0xdf, // 'Я'
    CP1251_SMALL_A          = 0xe0, // 'а', the first Cyrillic small letter
    CP1251_SMALL_IE         = 0xe5, // 'е'
    CP1251_SMALL_I          = 0xe8, // 'и'
    CP1251_SMALL_O          = 0xee, // 'о'
    CP1251_SMALL_U          = 0xf3, // 'у'
    CP1251_SMALL_YERU       = 0xfb, // 'ы'
    CP1251_SMALL_E          = 0xfd, // 'э'
    CP1251_SMALL_YU         = 0xfe, // 'ю'
    CP1251_SMALL_YA         = 0xff  // 'я'
H2_ENUM_END(Cp1251Code)

// The font's character codes past ASCII (glyph index + ' '): the unprintable
// glyph, then 'А'..'Я', 'Ё', 'а'..'я' and 'ё'.
H2_ENUM_BEGIN(FontCharacterCode)
    FONT_CODE_UNPRINTABLE     = 0x7f,
    FONT_CODE_CAPITAL_A       = 0x80,
    FONT_CODE_CAPITAL_IO      = 0xa0,
    FONT_CODE_SMALL_A         = 0xa1,
    FONT_CODE_SMALL_IO        = 0xc1,
    FONT_CODE_CAPITAL_SHIFT   = CP1251_CAPITAL_A - FONT_CODE_CAPITAL_A,
    FONT_CODE_SMALL_SHIFT     = CP1251_SMALL_A - FONT_CODE_SMALL_A
H2_ENUM_END(FontCharacterCode)

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
    void DrawStringExecute(H2_CONST char* text, i32 x, i32 y, FontDrawMode mode, i32 clipX, i32 clipY, i32 clipW, i32 clipH);

public:
    void DrawString(H2_CONST char* text, i32 x, i32 y, FontDrawMode mode);
    i32 GetCharacterWidth(u8 character);
    void ExtractLine(H2_CONST char* text, char* line, i32* position, i32 maxWidth, i32* lineWidth, u8 lastLine);
    void DrawBoundedString(H2_CONST char* text, i32 x, i32 y, i32 width, i32 height, FontDrawMode mode, FontAlignment align);
    i32 LineLength(H2_CONST char* text, i32 maxW);
    i32 LineWidth(H2_CONST char* text);
};
#pragma pack(pop)
SIZE(font, 0x20);
#endif
