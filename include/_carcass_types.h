#ifndef HOMM2_CARCASS_TYPES_H
#define HOMM2_CARCASS_TYPES_H
// Minimal COMPLETE definitions for structs that some functions pass/return BY VALUE but
// that the recovered headers only forward-declare. Needed so carcass-phase stub bodies
// compile (a by-value param/return requires a complete type at the definition). Layouts
// are best-effort placeholders (tag_message taken from KB.cpp's local def) — the real
// layouts get pinned down during byte-matching. Included at the tail of _all.h.

struct tag_message {
    int type, field4, field8, fieldC, field10, field14;
    union { char *text; int field18; };
};
struct SMapChange { char _pad[64]; };

#endif // HOMM2_CARCASS_TYPES_H
