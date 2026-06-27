#pragma once
// Map data records for the EDITOR/mapcell TU (fullMap). NOT original source.
//
// Layout recovered from the retail /Od this+offset accesses (mapcell.c.obj):
//   - mapCellExtra is a PACKED 7-byte record (stride proven by *7 indexing and
//     the (extras - i) + i*8 addressing idiom in ClearCellExtra / the _write of
//     extraCount*7 bytes).
//   - mapCell is a 12-byte record (cells allocated as width*height*12; memcpy 12).
// Byte/word access widths + bit positions are read off the andb/andw/movw forms.
#include <va.h>
#include <Ints.h>

#pragma pack(push, 1)
struct mapCellExtra {       // 7 bytes (packed)
    u16 index;              // +0  next-in-chain / free-pool marker (0xFFFF == free)
    u8  objFlag    : 1;     // +2 bit0
    u8  objTileset : 7;     // +2 bits1-7
    u8  objIndex;           // +3  object sprite index (0xFF == none)
    u8  f4a : 1;            // +4 bit0
    u8  f4b : 1;            // +4 bit1
    u8  f4c : 1;            // +4 bit2
    u8  f4hi : 5;           // +4 bits3-7
    u8  ovlFlag0   : 1;     // +5 bit0
    u8  ovlFlag1   : 1;     // +5 bit1
    u8  ovlTileset : 6;     // +5 bits2-7
    u8  ovlIndex;           // +6  overlay sprite index (0xFF == none)
};
#pragma pack(pop)
SIZE(mapCellExtra, 7);

struct mapCell {            // 12 bytes
    u16 tile;               // +0
    u8  objFlag0   : 1;     // +2 bit0
    u8  objFlag1   : 1;     // +2 bit1
    u8  objTileset : 6;     // +2 bits2-7
    u8  objIndex;           // +3  object sprite index (0xFF == none)
    u16 w4a : 1;            // +4 bit0
    u16 w4b : 1;            // +4 bit1
    u16 w4c : 1;            // +4 bit2
    u16 w4hi : 13;          // +4 bits3-15
    u8  ovlFlag0   : 1;     // +6 bit0
    u8  ovlFlag1   : 1;     // +6 bit1
    u8  ovlTileset : 6;     // +6 bits2-7
    u8  ovlIndex;           // +7  overlay sprite index (0xFF == none)
    u16 unk8;               // +8
    u16 extra;              // +0xa  head index into the extras chain
};
SIZE(mapCell, 12);
