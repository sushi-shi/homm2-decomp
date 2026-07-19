#ifndef HOMM2_BASE_ICONENTRY_H
#define HOMM2_BASE_ICONENTRY_H

#ifndef HOMM2_BASE_ICONENTRY_NO_SIZE
#include <va.h>
#endif

#pragma pack(push, 1)
struct IconEntry {
    i16 x;
    i16 y;
    i16 w;
    i16 h;
    u8 flags;
    i32 srcOffset;
};
#pragma pack(pop)
#ifndef HOMM2_BASE_ICONENTRY_NO_SIZE
SIZE(IconEntry, 0xd);
#endif

#endif
