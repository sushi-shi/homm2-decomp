#ifndef HOMM2_BASE_FONT_H
#define HOMM2_BASE_FONT_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 9 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <BASE/resource.h>
// forward declarations:
class icon;

typedef enum FontGlyphConstant {
    FONT_SPACER_CHAR = 0x1f,    // advances the pen without drawing a glyph
    FONT_GLYPH_FALLBACK = 0x5f  // out-of-range characters render this glyph
} FontGlyphConstant;

#pragma pack(push, 1) // recovered layout is byte-packed
// font is an asset subclass of resource (base ctor resource(5,K,1,0); slot-0 dtor override).
class font : public resource {
public:
    // --- own members (base resource occupies 0x00..0x10; widths from access-analysis) ---
    i32 m_height;       // +0x10
    i32 m_isLarge;      // +0x14
    i32 m_suppressDraw; // +0x18
    icon* m_glyphIcon;  // +0x1c  (resourceManager::GetIcon result)
    // --- constructors ---
    font(u32l);
    virtual ~font();
    // --- methods ---
protected:
    void
    DrawStringExecute(char*, i32, i32, i32, i32, i32, i32, i32); // ?...@font@@IAE... (protected)
public:
    void DrawString(char*, i32, i32, i32);
    i32 GetCharacterWidth(u8);
    void DrawBoundedString(char*, i32, i32, i32, i32, i32, i32);
    i32 LineLength(char*, i32);
    i32 LineWidth(char*);
};
#pragma pack(pop)
SIZE(font, 0x20);
#endif // HOMM2_BASE_FONT_H
