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
    short  field_0x2f;  // +0x2f
    unsigned short field_0x31;  // +0x31  text cursor position
    unsigned short m_maxLength;  // +0x33
    short  m_rectX;  // +0x35
    short  m_rectY;  // +0x37
    short  m_rectW;  // +0x39
    short  m_rectH;  // +0x3b
    short  m_innerW;  // +0x3d
    short  m_innerH;  // +0x3f
    short  m_innerX;  // +0x41
    short  m_innerY;  // +0x43
    short  field_0x45;  // +0x45
    short  m_hasInset;  // +0x47  !union: conflicting widths
    short  field_0x49;  // +0x49
    short  field_0x4b;  // +0x4b
    char   m_cursorBlink;  // +0x4d
    // --- constructors ---
    textEntryWidget(void);
    textEntryWidget(short int, short int, short int, short int, short int, char *, char *, short int, char *, short int, short int, short int, short int, int, int);
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
