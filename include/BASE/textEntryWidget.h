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
    short  m_iconFrame;  // +0x2f
    unsigned short m_cursorPosition;  // +0x31  text cursor position
    unsigned short m_maxLength;  // +0x33
    short  m_rectX;  // +0x35
    short  m_rectY;  // +0x37
    short  m_rectW;  // +0x39
    short  m_rectH;  // +0x3b
    short  m_innerW;  // +0x3d
    short  m_innerH;  // +0x3f
    short  m_innerX;  // +0x41
    short  m_innerY;  // +0x43
    short  m_maxLines;  // +0x45  maximum accepted wrapped line count
    short  m_preserveTextOnFocus;  // +0x47  nonzero begins editing at the existing text
    short  m_entryType;  // +0x49
    short  m_displayOffset;  // +0x4b
    char   m_cursorBlink;  // +0x4d
    // --- constructors ---
    textEntryWidget(void);
    textEntryWidget(short int x, short int y, short int width, short int height,
                    short int maxLength, char *text, char *fontName, short int color,
                    char *iconName, short int iconFrame, short int id, short int kind,
                    short int layout, int horizontalInset, int verticalInset);
    virtual ~textEntryWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(int);
    void SetupDisplayString(char *, unsigned short int);
};
#pragma pack(pop)
SIZE(textEntryWidget, 0x4e);
#endif // HOMM2_BASE_TEXTENTRYWIDGET_H
