#ifndef HOMM2_BASE_ICONWIDGET_H
#define HOMM2_BASE_ICONWIDGET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 8 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
// forward declarations:
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class icon;
class iconWidget : public widget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    icon   *m_icon;  // +0x20  the icon resource
    short  m_frame;  // +0x24
    char   m_flip;  // +0x26
    short  field_0x27;  // +0x27
    int    m_iconId;  // +0x29
    // --- constructors ---
    iconWidget(void);
    iconWidget(short int, short int, short int, short int, unsigned long int, short int, signed char, short int, short int, short int);
    iconWidget(short int, short int, short int, short int, char *, short int, signed char, short int, short int, short int);
    virtual ~iconWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
};
#pragma pack(pop)
SIZE(iconWidget, 0x2d);
#endif // HOMM2_BASE_ICONWIDGET_H
