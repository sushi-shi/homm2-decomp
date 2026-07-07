#ifndef HOMM2_BASE_BORDER_H
#define HOMM2_BASE_BORDER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 7 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
class bitmap;
class icon;
// forward declarations:
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class border : public widget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    bitmap *field_0x20;  // +0x20  background bitmap
    icon  *field_0x24;  // +0x24  background icon
    short  field_0x28;  // +0x28
    // --- constructors ---
    border(void);
    border(short int, short int, short int, short int, short int, short int, short int, char *);
    virtual ~border() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
};
#pragma pack(pop)
SIZE(border, 0x2a);
#endif // HOMM2_BASE_BORDER_H
