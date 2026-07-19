#ifndef HOMM2_EDITOR_MAPCELL_H
#define HOMM2_EDITOR_MAPCELL_H

#include <va.h>
#include <Ints.h>

// Occupancy is ignored when every object-stack entry is shadow-only or erased.
H2_ENUM_CLASS_BEGIN(MapCellFlag)
    MAP_CELL_OBJECT_SHADOW_ONLY = 0x80,
    MAP_CELL_OCCUPIED           = 0x08
H2_ENUM_CLASS_END(MapCellFlag)

H2_ENUM_CLASS_BEGIN(MapCellSentinel)
    MAPCELL_SPRITE_NONE = 0xff,
    MAPCELL_EXTRA_FREE  = 0xffff
H2_ENUM_CLASS_END(MapCellSentinel)

#pragma pack(push, 1)
struct mapCellExtra {
    u16 nextIndex;
    u8 animatedObject : 1;
    u8 objectTileset : 7;
    u8 objectIndex;
    u8 objectLayerBit0 : 1;
    u8 objectLayerBit1 : 1;
    u8 objectDrawnAsOverlay : 1;
    u8 objectMetadata : 5;
    u8 animatedOverlay : 1;
    u8 drawOverlayOnTop : 1;
    u8 overlayTileset : 6;
    u8 overlayIndex;
};
#pragma pack(pop)
SIZE(mapCellExtra, 7);

class mapCell {
public:
    u16 m_terrainImageIndex;
    union {
        char m_objType;
        u8 m_objTypeBits;
        struct {
            u8 m_animatedObject : 1;
            u8 m_isRoad : 1;
            u8 m_objectTileset : 6;
        };
    };
    u8 m_objectIndex;
    union {
        u16 m_objectData;
        struct {
            u16 m_objectLayerBit0 : 1;
            u16 m_objectLayerBit1 : 1;
            u16 m_objectDrawnAsOverlay : 1;
            u16 m_objectMetadata : 13;
        };
        struct {
            u16 m_tentFlags : 3;
            u16 m_tentColor : 13;
        };
    };
    u8 m_animatedOverlay : 1;
    u8 m_drawOverlayOnTop : 1;
    u8 m_overlayTileset : 6;
    u8 m_overlayIndex;
    u8 m_flags;
    u8 m_triggerType;
    u16 m_extraIndex;
};
SIZE(mapCell, 12);

// Legacy on-disk record sizes used by fullMap::Read's conversion path.
struct oldMapCell {
    u8 raw[20];
};

struct oldMapCellExtra {
    u8 raw[15];
};
#endif
