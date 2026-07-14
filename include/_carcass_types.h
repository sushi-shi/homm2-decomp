#ifndef HOMM2_CARCASS_TYPES_H
#define HOMM2_CARCASS_TYPES_H
#include <va.h>
// Minimal COMPLETE definitions for structs that some functions pass/return BY VALUE but
// that the recovered headers only forward-declare. Needed so carcass-phase stub bodies
// compile (a by-value param/return requires a complete type at the definition). Included
// at the tail of _all.h; recovered layouts are pinned as byte-matching proves them.

#pragma pack(push, 1)
struct tag_messageKeyView {
    unsigned char keyCode;
    unsigned char scanCode;
    unsigned char trailing2;
    unsigned char trailing3;
};
#pragma pack(pop)
SIZE(tag_messageKeyView, 0x4);

struct tag_message {
    int type;
    union {
        int field4;
        tag_messageKeyView keyView;
    };
    int field8, fieldC, field10, field14;
    union { char *text; int field18; };
};
SIZE(tag_message, 0x1c);
#endif // HOMM2_CARCASS_TYPES_H
