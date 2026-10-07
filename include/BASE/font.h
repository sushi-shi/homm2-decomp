#ifndef HOMM2_BASE_FONT_H
#define HOMM2_BASE_FONT_H

#include <Ints.h>
#include <BASE/resource.h>

class icon;

typedef enum FontGlyphConstant {
    FONT_SPACER_CHAR    = 0x1f,
    FONT_GLYPH_FALLBACK = 0x5f
} FontGlyphConstant;


typedef enum Cp1251Code {
    CP1251_ASCII_LAST       = 0x7f,
    CP1251_ASCII_COUNT      = 0x80,
    CP1251_CAPITAL_IO       = 0xa8,
    CP1251_SMALL_IO         = 0xb8,
    CP1251_CAPITAL_A        = 0xc0,
    CP1251_CAPITAL_IE       = 0xc5,
    CP1251_CAPITAL_I        = 0xc8,
    CP1251_CAPITAL_O        = 0xce,
    CP1251_CAPITAL_U        = 0xd3,
    CP1251_CAPITAL_YERU     = 0xdb,
    CP1251_CAPITAL_E        = 0xdd,
    CP1251_CAPITAL_YU       = 0xde,
    CP1251_CAPITAL_YA       = 0xdf,
    CP1251_SMALL_A          = 0xe0,
    CP1251_SMALL_IE         = 0xe5,
    CP1251_SMALL_I          = 0xe8,
    CP1251_SMALL_O          = 0xee,
    CP1251_SMALL_U          = 0xf3,
    CP1251_SMALL_YERU       = 0xfb,
    CP1251_SMALL_E          = 0xfd,
    CP1251_SMALL_YU         = 0xfe,
    CP1251_SMALL_YA         = 0xff
} Cp1251Code;


typedef enum FontCharacterCode {
    FONT_CODE_UNPRINTABLE     = 0x7f,
    FONT_CODE_CAPITAL_A       = 0x80,
    FONT_CODE_CAPITAL_IO      = 0xa0,
    FONT_CODE_SMALL_A         = 0xa1,
    FONT_CODE_SMALL_IO        = 0xc1,
    FONT_CODE_CAPITAL_SHIFT   = CP1251_CAPITAL_A - FONT_CODE_CAPITAL_A,
    FONT_CODE_SMALL_SHIFT     = CP1251_SMALL_A - FONT_CODE_SMALL_A
} FontCharacterCode;

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
    font(u32l id);
    virtual ~font();

protected:
    void DrawStringExecute(const char* text, i32 x, i32 y, FontDrawMode mode, i32 clipX, i32 clipY, i32 clipW, i32 clipH);

public:
    void DrawString(const char* text, i32 x, i32 y, FontDrawMode mode);
    i32 GetCharacterWidth(u8 character);
    void ExtractLine(const char* text, char* line, i32* position, i32 maxWidth, i32* lineWidth, u8 lastLine);
    void DrawBoundedString(const char* text, i32 x, i32 y, i32 width, i32 height, FontDrawMode mode, FontAlignment align);
    i32 LineLength(const char* text, i32 maxW);
    i32 LineWidth(const char* text);
};
#pragma pack(pop)
#endif
