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

// Adventure-map object image set. Retail expressions use i32, while map records
// store the domain in packed 6/7-bit fields.
H2_ENUM_CLASS_BEGIN_SPLIT(TilesetId, u8)
    TILESET_NONE                    = 0,
    TILESET_OBJNHAUN                = 10,
    TILESET_OBJNARTI                = 11,
    TILESET_MONS32                  = 12,
    TILESET_ART32                   = 13,
    TILESET_FLAG32                  = 14,
    TILESET_RESSMALL                = 15,
    TILESET_HOURGLAS                = 16,
    TILESET_ROUTE                   = 17,
    TILESET_STONBACK                = 19,
    TILESET_MINIMON                 = 20,
    TILESET_MINIHERO                = 21,
    TILESET_MTNSNOW                 = 22,
    TILESET_MTNSWMP                 = 23,
    TILESET_MTNLAVA                 = 24,
    TILESET_MTNDSRT                 = 25,
    TILESET_MTNDIRT                 = 26,
    TILESET_MTNMULT                 = 27,
    TILESET_EXTRAOVR                = 29,
    TILESET_ROAD                    = 30,
    TILESET_MTNCRCK                 = 31,
    TILESET_MTNGRAS                 = 32,
    TILESET_TREJNGL                 = 33,
    TILESET_TREEVIL                 = 34,
    TILESET_OBJNTOWN                = 35,
    TILESET_OBJNTWBA                = 36,
    TILESET_OBJNTWSH                = 37,
    TILESET_OBJNTWRD                = 38,
    TILESET_OBJNXTRA                = 39,
    TILESET_OBJNWAT2                = 40,
    TILESET_OBJNMUL2                = 41,
    TILESET_TRESNOW                 = 42,
    TILESET_TREFIR                  = 43,
    TILESET_TREFALL                 = 44,
    TILESET_STREAM                  = 45,
    TILESET_OBJNRSRC                = 46,
    TILESET_DUMMY                   = 47,
    TILESET_OBJNGRA2                = 48,
    TILESET_TREDECI                 = 49,
    TILESET_OBJNWATR                = 50,
    TILESET_OBJNGRAS                = 51,
    TILESET_OBJNSNOW                = 52,
    TILESET_OBJNSWMP                = 53,
    TILESET_OBJNLAVA                = 54,
    TILESET_OBJNDSRT                = 55,
    TILESET_OBJNDIRT                = 56,
    TILESET_OBJNCRCK                = 57,
    TILESET_OBJNLAV3                = 58,
    TILESET_OBJNMULT                = 59,
    TILESET_OBJNLAV2                = 60,
    TILESET_X_LOC1                  = 61,
    TILESET_X_LOC2                  = 62,
    TILESET_X_LOC3                  = 63,
    ABANDONED_MINE_TILESET_CENTER   = TILESET_EXTRAOVR,
    ABANDONED_MINE_TILESET_TOP_A    = TILESET_OBJNDIRT,
    ABANDONED_MINE_TILESET_TOP_B    = TILESET_OBJNGRAS,
    ABANDONED_MINE_TILESET_BOTTOM_A = TILESET_MTNDIRT,
    ABANDONED_MINE_TILESET_BOTTOM_B = TILESET_MTNGRAS,
    RANDOM_TOWN_OBJECT_TILESET      = TILESET_OBJNTOWN,
    RANDOM_TOWN_OVERLAY_TILESET     = TILESET_OBJNTWSH,
    RANDOM_TOWN_SOURCE_TILESET      = TILESET_OBJNTWRD,
    TILESET_COUNT                   = 64
H2_ENUM_CLASS_END_SPLIT(TilesetId, u8)

#pragma pack(push, 1)
struct mapCellExtra {
    u16 nextIndex;
    u8 animatedObject : 1;
    H2_ENUM_BITFIELD(TilesetId, u8) objectTileset : 7;
    u8 objectIndex;
    u8 objectLayerBit0 : 1;
    u8 objectLayerBit1 : 1;
    u8 objectDrawnAsOverlay : 1;
    u8 objectMetadata : 5;
    u8 animatedOverlay : 1;
    u8 drawOverlayOnTop : 1;
    H2_ENUM_BITFIELD(TilesetId, u8) overlayTileset : 6;
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
            H2_ENUM_BITFIELD(TilesetId, u8) m_objectTileset : 6;
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
    H2_ENUM_BITFIELD(TilesetId, u8) m_overlayTileset : 6;
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
