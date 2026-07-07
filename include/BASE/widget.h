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

class widget /* abstract */ {
public:
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
#endif // HOMM2_BASE_WIDGET_H
