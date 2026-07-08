#ifndef HOMM2_BASE_LISTBOXWIDGET_H
#define HOMM2_BASE_LISTBOXWIDGET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 9 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
// forward declarations:
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class resource;
class widget;
class font;
class icon;
class listBoxWidget : public widget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    font   *m_font;  // +0x20  the list font
    icon   *m_icon;  // +0x24  the list icon
    short  field_0x28;  // +0x28
    short  field_0x2a;  // +0x2a
    short  field_0x2c;  // +0x2c
    short  field_0x2e;  // +0x2e
    short  field_0x30;  // +0x30
    short  m_itemCount;  // +0x32
    short  m_selectedIndex;  // +0x34
    short  field_0x36;  // +0x36
    int    field_0x38;  // +0x38
    void   **m_items;  // +0x3c  item-string array
    short  m_topIndex;  // +0x40
    short  field_0x42;  // +0x42
    short  field_0x44;  // +0x44
    short  field_0x46;  // +0x46
    short  field_0x48;  // +0x48
    short  field_0x4a;  // +0x4a
    short  field_0x4c;  // +0x4c
    short  field_0x4e;  // +0x4e
    short  field_0x50;  // +0x50
    short  field_0x52;  // +0x52
    short  field_0x54;  // +0x54
    short  field_0x56;  // +0x56
    short  field_0x58;  // +0x58
    short  field_0x5a;  // +0x5a
    short  field_0x5c;  // +0x5c
    short  field_0x5e;  // +0x5e
    short  m_listX;  // +0x60
    short  m_listY;  // +0x62
    short  field_0x64;  // +0x64
    short  field_0x66;  // +0x66
    short  field_0x68;  // +0x68
    short  field_0x6a;  // +0x6a
    short  field_0x6c;  // +0x6c
    short  field_0x6e;  // +0x6e
    short  field_0x70;  // +0x70
    short  field_0x72;  // +0x72
    short  field_0x74;  // +0x74
    short  field_0x76;  // +0x76
    short  field_0x78;  // +0x78
    short  field_0x7a;  // +0x7a
    short  field_0x7c;  // +0x7c
    short  field_0x7e;  // +0x7e
    short  field_0x80;  // +0x80
    short  field_0x82;  // +0x82
    short  field_0x84;  // +0x84
    short  field_0x86;  // +0x86
    short  field_0x88;  // +0x88
    char   field_0x8a;  // +0x8a
    char   field_0x8b;  // +0x8b
    char   field_0x8c;  // +0x8c
    char   field_0x8d;  // +0x8d
    widget *m_scrollbar;  // +0x8e  child widget (scrollbar)
    // --- constructors ---
    listBoxWidget(void);
    virtual ~listBoxWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
    void DeleteItem(int);
    void DrawLBStuff(int);
    int ProcessMouseMessage(struct tag_message &);
};
#pragma pack(pop)
SIZE(listBoxWidget, 0x92);
#endif // HOMM2_BASE_LISTBOXWIDGET_H
