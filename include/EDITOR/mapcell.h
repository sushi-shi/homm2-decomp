#ifndef HOMM2_EDITOR_MAPCELL_H
#define HOMM2_EDITOR_MAPCELL_H
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

// Bits of mapCell::m_flags (+8). Bit 0x80 marks a cell whose object stack holds
// no solid piece — every piece is shadow-layer or erased — so the occupancy
// pass (bit 0x08) skips it.
typedef enum MapCellFlag {
    MAP_CELL_OBJECT_SHADOW_ONLY = 0x80
} MapCellFlag;

typedef enum MapTilesetConstant {
    MAP_TILESET_FLAG = 14
} MapTilesetConstant;

#pragma pack(push, 1)
struct mapCellExtra {       // 7 bytes (packed)
    u16 nextIndex;          // +0  next-in-chain / free-pool marker (0xFFFF == free)
    u8 animatedObject : 1;  // +2 bit0
    u8 objectTileset : 7;   // +2 bits1-7
    u8 objectIndex;         // +3  object sprite index (0xFF == none)
    u8 objectLayerBit0 : 1; // +4 bit0; bits0-1 encode the object render layer
    u8 objectLayerBit1 : 1; // +4 bit1; 0=object, 1=background, 2=shadow, 3=terrain
    u8 objectDrawnAsOverlay
        : 1;                 // +4 bit2: draw the object sprite in the OVERLAY pass, above the hero
    u8 objectMetadata : 5;   // +4 bits3-7
    u8 animatedOverlay : 1;  // +5 bit0
    u8 drawOverlayOnTop : 1; // +5 bit1
    u8 overlayTileset : 6;   // +5 bits2-7
    u8 overlayIndex;         // +6  overlay sprite index (0xFF == none)
};
#pragma pack(pop)
SIZE(mapCellExtra, 7);

class mapCell { // 12 bytes (CodeView mangles every use as PAVmapCell -> class)
public:
    u16 m_terrainImageIndex; // +0
    union {
        char m_objType; // +2 complete object attribute byte
        u8 m_objTypeBits;
        struct {
            u8 m_animatedObject : 1; // +2 bit0
            u8 m_isRoad : 1;         // +2 bit1
            u8 m_objectTileset : 6;  // +2 bits2-7
        };
    };
    u8 m_objectIndex; // +3  object sprite index (0xFF == none)
    union {
        u16 m_objectData; // +4 complete object data word
        struct {
            u16 m_objectLayerBit0 : 1; // +4 bit0; bits0-1 encode the object render layer
            u16 m_objectLayerBit1 : 1; // +4 bit1; 0=object, 1=background, 2=shadow, 3=terrain
            u16 m_objectDrawnAsOverlay
                : 1; // +4 bit2: draw the object sprite in the OVERLAY pass, above the hero(decoded from the ADVMGR draw gates)
            u16 m_objectMetadata : 13; // +4 bits3-15
        };
        struct {
            u16 m_tentFlags : 3;
            u16 m_tentColor : 13;
        };
    };
    u8 m_animatedOverlay : 1;  // +6 bit0
    u8 m_drawOverlayOnTop : 1; // +6 bit1
    u8 m_overlayTileset : 6;   // +6 bits2-7
    u8 m_overlayIndex;         // +7  overlay sprite index (0xFF == none)
    u8 m_flags;                // +8  terrain orientation, occupancy, and passability flags
    u8 m_triggerType;          // +9 (high bit is a flag)
    u16 m_extraIndex;          // +0xa  head index into the extras chain
};
SIZE(mapCell, 12);

// Legacy on-disk record sizes (fullMap::Read's convert path streams the old layout and
// copies the leading bytes into the new packed records).
struct oldMapCell {
    u8 raw[20];
};
struct oldMapCellExtra {
    u8 raw[15];
};
#endif // HOMM2_EDITOR_MAPCELL_H
