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
    i16  m_frame;  // +0x24  icon frame index
    i8 m_flip;  // +0x26  signed flip/mirror flag passed to icon drawing
    i16  m_fillColor;  // +0x27  color index used by filled-icon draw mode
    u32l m_iconId;  // +0x29  aggregate resource id
    // --- constructors ---
    iconWidget(void);
    iconWidget(i16 x, i16 y, i16 width, i16 height,
               u32l iconId, i16 frame, i8 flip,
               i16 id, i16 kind, i16 fillColor);
    iconWidget(i16 x, i16 y, i16 width, i16 height,
               char *iconName, i16 frame, i8 flip,
               i16 id, i16 kind, i16 fillColor);
    virtual ~iconWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
};
#pragma pack(pop)
SIZE(iconWidget, 0x2d);
#endif // HOMM2_BASE_ICONWIDGET_H
