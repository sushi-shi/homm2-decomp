#ifndef HOMM2_BASE_WIDGET_H
#define HOMM2_BASE_WIDGET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 7 methods, 3 own-virtual (all pure), 0 static data.
// Abstract root of the BASE UI-widget hierarchy. Verified from ??_7widget@@6B@: the
// vtable is 3 all-__purecall slots in order [Draw, ~widget, Main]. Draw is pure with
// NO body (emits no symbol); ~widget (??1widget@@UAE, 0x7) and Main (?Main@widget@@UAE,
// 0x2f4) are pure-virtual-WITH-body. Declaration order == vtable slot order; derived
// classes (border, iconWidget, textWidget, dimmerWidget, ...) override these 3 slots.
#include <va.h>
// forward declarations:
class heroWindow;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class widget /* abstract */ {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    heroWindow *field_0x4;  // +0x04  owning window
    widget *field_0x8;  // +0x08  next widget in the owner window's list
    int    field_0xc;  // +0x0c
    short  field_0x10;  // +0x10
    short  field_0x12;  // +0x12
    short  field_0x14;  // +0x14
    short  field_0x16;  // +0x16
    short  field_0x18;  // +0x18
    short  field_0x1a;  // +0x1a
    short  field_0x1c;  // +0x1c
    short  field_0x1e;  // +0x1e
    // --- constructors ---
    widget(short int, short int, short int, short int, short int, short int);
    widget(void);
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) = 0;
    virtual ~widget(void) = 0;
    virtual int Main(struct tag_message &) = 0;
    // --- methods ---
    int Open(int, class heroWindow *);
    void Close(void);
    void Dim(void);
};
#pragma pack(pop)
SIZE(widget, 0x20);
#endif // HOMM2_BASE_WIDGET_H
