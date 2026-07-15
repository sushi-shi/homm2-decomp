#ifndef HOMM2_BASE_TEXTWIDGET_H
#define HOMM2_BASE_TEXTWIDGET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 9 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
// forward declarations:
struct tag_message;

class resource;
class font;

#pragma pack(push, 1)  // recovered layout is byte-packed
class textWidget : public widget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    char   *m_text;  // +0x20  the text buffer (BaseAlloc'd)
    font   *m_font;  // +0x24  the loaded font (disposed as resource in dtor)
    short  m_color;  // +0x28
    char   m_alignment;  // +0x2a  DrawBoundedString alignment mode
    // --- constructors ---
    textWidget(void);
    textWidget(short int, short int, short int, short int, char *, char *, short int, short int, short int, short int);
    virtual ~textWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
    void SetColorIndex(short int);
    void SetText(char *);
};
#pragma pack(pop)
SIZE(textWidget, 0x2b);
#endif // HOMM2_BASE_TEXTWIDGET_H
