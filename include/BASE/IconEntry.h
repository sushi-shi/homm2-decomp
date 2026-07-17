#ifndef HOMM2_BASE_ICONENTRY_H
#define HOMM2_BASE_ICONENTRY_H

// Misc.h formerly owned this record without a SIZE declaration. It suppresses the assertion
// below so moving the declaration does not perturb MSVC 4.2's cumulative /Od state.
#ifndef HOMM2_BASE_ICONENTRY_NO_SIZE
#include <va.h>
#endif

#pragma pack(push, 1)
struct IconEntry {
    i16 x;                // +0  x offset
    i16 y;                // +2  y offset
    i16 w;                // +4  width
    i16 h;                // +6  height
    u8 flags;    // +8
    i32 srcOffset;          // +9  offset of this frame's RLE data within the icon blob
};
#pragma pack(pop)
#ifndef HOMM2_BASE_ICONENTRY_NO_SIZE
SIZE(IconEntry, 0xd);
#endif

#endif
