#ifndef HOMM2_BASE_TEXTENTRYWIDGET_H
#define HOMM2_BASE_TEXTENTRYWIDGET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 8 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include <BASE/textWidget.h>
#include "widget.h"
// forward declarations:
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class resource;
class icon;

class textEntryWidget : public textWidget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    icon   *m_icon;  // +0x2b  the loaded icon (disposed in dtor)
    i16  m_iconFrame;  // +0x2f
    u16 m_cursorPosition;  // +0x31  text cursor position
    u16 m_maxLength;  // +0x33
    i16  m_rectX;  // +0x35
    i16  m_rectY;  // +0x37
    i16  m_rectW;  // +0x39
    i16  m_rectH;  // +0x3b
    i16  m_innerW;  // +0x3d
    i16  m_innerH;  // +0x3f
    i16  m_innerX;  // +0x41
    i16  m_innerY;  // +0x43
    i16  m_maxLines;  // +0x45  maximum accepted wrapped line count
    i16  m_preserveTextOnFocus;  // +0x47  nonzero begins editing at the existing text
    i16  m_entryType;  // +0x49
    i16  m_displayOffset;  // +0x4b
    char   m_cursorBlink;  // +0x4d
    // --- constructors ---
    textEntryWidget(void);
    textEntryWidget(i16 x, i16 y, i16 width, i16 height,
                    i16 maxLength, char *text, char *fontName, i16 color,
                    char *iconName, i16 iconFrame, i16 id, i16 kind,
                    i16 layout, i32 horizontalInset, i32 verticalInset);
    virtual ~textEntryWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(i32);
    void SetupDisplayString(char *, u16);
};
#pragma pack(pop)
SIZE(textEntryWidget, 0x4e);
#endif // HOMM2_BASE_TEXTENTRYWIDGET_H
