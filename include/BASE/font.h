#ifndef HOMM2_BASE_FONT_H
#define HOMM2_BASE_FONT_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 9 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <BASE/resource.h>
// forward declarations:
class icon;

#pragma pack(push, 1)  // recovered layout is byte-packed
// font is an asset subclass of resource (base ctor resource(5,K,1,0); slot-0 dtor override).
class font : public resource {
public:
    // --- own members (base resource occupies 0x00..0x10; widths from access-analysis) ---
    int    m_height;  // +0x10
    int    m_isLarge;  // +0x14
    int    m_suppressDraw;  // +0x18
    icon  *m_glyphIcon;  // +0x1c  (resourceManager::GetIcon result)
    // --- constructors ---
    font(unsigned long int);
    virtual ~font();
    // --- methods ---
protected:
    void DrawStringExecute(char *, int, int, int, int, int, int, int);   // ?...@font@@IAE... (protected)
public:
    void DrawString(char *, int, int, int);
    int GetCharacterWidth(unsigned char);
    void DrawBoundedString(char *, int, int, int, int, int, int);
    int LineLength(char *, int);
    int LineWidth(char *);
};
#pragma pack(pop)
SIZE(font, 0x20);
#endif // HOMM2_BASE_FONT_H
