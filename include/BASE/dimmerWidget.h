#ifndef HOMM2_BASE_DIMMERWIDGET_H
#define HOMM2_BASE_DIMMERWIDGET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 6 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
// forward declarations:
struct tag_message;

#pragma pack(push, 1) // recovered layout is byte-packed
class dimmerWidget : public widget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    // --- constructors ---
    dimmerWidget(void);
    dimmerWidget(i16, i16, i16, i16, i16, i16);
    virtual ~dimmerWidget() OVERRIDE {} // EXPLICIT but inline: retail has ??_E/??_G (deleting
                                        // dtors) at 0x4dd410 with the base dtor folded in and
                                        // NO standalone ??1 — an out-of-line body would emit one.
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    // --- methods ---
    void Read(void);
};
#pragma pack(pop)
SIZE(dimmerWidget, 0x20);
#endif // HOMM2_BASE_DIMMERWIDGET_H
