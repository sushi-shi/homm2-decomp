#ifndef HOMM2_BASE_DROPLISTWIDGET_H
#define HOMM2_BASE_DROPLISTWIDGET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 11 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
// forward declarations:
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class bitmap;
class icon;
class font;
class dropListWidget : public widget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    font   *m_font;  // +0x20  the list font
    icon   *m_icon;  // +0x24  drop button icon
    short  m_contentX;  // +0x28
    short  m_contentY;  // +0x2a
    short  field_0x2c;  // +0x2c
    short  field_0x2e;  // +0x2e
    short  field_0x30;  // +0x30
    short  field_0x32;  // +0x32
    short  field_0x34;  // +0x34
    short  field_0x36;  // +0x36
    short  field_0x38;  // +0x38
    short  field_0x3a;  // +0x3a
    short  field_0x3c;  // +0x3c
    short  m_selectedIndex;  // +0x3e
    char   **m_items;  // +0x40  item-string array
    short  m_topIndex;  // +0x44
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
    short  field_0x60;  // +0x60
    short  field_0x62;  // +0x62
    short  field_0x64;  // +0x64
    short  field_0x66;  // +0x66
    short  field_0x68;  // +0x68
    short  field_0x6a;  // +0x6a
    short  m_iconX;  // +0x6c
    short  m_iconY;  // +0x6e
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
    short  field_0x8a;  // +0x8a
    short  field_0x8c;  // +0x8c
    short  field_0x8e;  // +0x8e
    short  field_0x90;  // +0x90
    short  field_0x92;  // +0x92
    short  field_0x94;  // +0x94
    short  field_0x96;  // +0x96
    short  field_0x98;  // +0x98
    short  field_0x9a;  // +0x9a
    short  field_0x9c;  // +0x9c
    short  field_0x9e;  // +0x9e
    short  field_0xa0;  // +0xa0
    short  field_0xa2;  // +0xa2
    short  field_0xa4;  // +0xa4
    short  field_0xa6;  // +0xa6
    short  field_0xa8;  // +0xa8
    short  field_0xaa;  // +0xaa
    char   field_0xac;  // +0xac
    char   field_0xad;  // +0xad
    char   field_0xae;  // +0xae
    char   field_0xaf;  // +0xaf
    bitmap *m_savedBackground;  // +0xb0  saved drop-background bitmap
    // --- constructors ---
    dropListWidget(void);
    virtual ~dropListWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
    void DeleteItem(int);
    void DrawDropStuff(void);
    void SaveDropBackground(void);
    void RestoreDropBackground(void);
    void ProcessSelectDialog(void);
};
#pragma pack(pop)
SIZE(dropListWidget, 0xb4);
#endif // HOMM2_BASE_DROPLISTWIDGET_H
