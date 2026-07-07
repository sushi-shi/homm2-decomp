#ifndef HOMM2_BASE_BUTTON_H
#define HOMM2_BASE_BUTTON_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 10 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
class icon;
// forward declarations:
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class button : public widget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    icon  *field_0x20;  // +0x20  button icon
    short  field_0x24;  // +0x24
    short  field_0x26;  // +0x26
    short  field_0x28;  // +0x28
    short  field_0x2a;  // +0x2a
    int    field_0x2c;  // +0x2c
    // --- constructors ---
    button(void);
    button(short int, short int, short int, short int, unsigned long int, short int, short int, short int, short int, short int, short int);
    button(short int, short int, short int, short int, char *, short int, short int, short int, short int, short int, short int);
    virtual ~button() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
    short int Select(struct tag_message &);
    short int Deselect(struct tag_message &);
};
#pragma pack(pop)
SIZE(button, 0x30);
#endif // HOMM2_BASE_BUTTON_H
