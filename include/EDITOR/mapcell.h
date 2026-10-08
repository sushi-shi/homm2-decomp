#ifndef HOMM2_EDITOR_MAPCELL_H
#define HOMM2_EDITOR_MAPCELL_H

#include <match.h>
#include <Domains.h>
#include <SOURCE/kbTypes.h>

// mapCell::m_flags: the ground tile's flips, the water cells along a shore
// (an edge or corner shape) and those at a shore's outer corner, a cell an
// object occupies, the cell the hero stands on, and a cell whose object parts
// are all shadow.
H2_ENUM_CLASS_BEGIN(MapCellFlag)
    MAP_CELL_FLIP_VERTICAL      = 0x01,
    MAP_CELL_FLIP_HORIZONTAL    = 0x02,
    MAP_CELL_SHORE              = 0x04,
    MAP_CELL_OCCUPIED           = 0x08,
    MAP_CELL_SHORE_CORNER       = 0x10,
    MAP_CELL_HERO               = 0x40,
    MAP_CELL_OBJECT_SHADOW_ONLY = 0x80,
    // What a repainted cell keeps: everything but the overlay-extra bit
    // (0x20) and the hero.
    MAP_CELL_GROUND_KEEP = MAP_CELL_FLIP_VERTICAL | MAP_CELL_FLIP_HORIZONTAL | MAP_CELL_SHORE
                         | MAP_CELL_OCCUPIED | MAP_CELL_SHORE_CORNER | MAP_CELL_OBJECT_SHADOW_ONLY
H2_ENUM_CLASS_END(MapCellFlag)
H2_ENUM_FLAGS(MapCellFlag)

H2_ENUM_BEGIN(MapCellSentinel)
    MAPCELL_SPRITE_NONE = 0xff,
    MAPCELL_EXTRA_FREE  = 0xffff
H2_ENUM_END(MapCellSentinel)

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
H2_ENUM_CLASS_END_SPLIT(TilesetId, u8)

enum { TILESET_COUNT = 64 };

#pragma pack(push, 1)
struct mapCellExtra {
    u16 nextIndex;
    u8 animatedObject : 1;
    H2_ENUM_BITFIELD(TilesetId, u8) objectTileset : 7;
    u8 objectIndex;
    // The object part lies on the ground (overlayType::groundLayer: drawn
    // under shadows and standing parts), or is only a shadow.
    u8 objectGroundLayer : 1;
    u8 objectShadow : 1;
    u8 objectDrawnAsOverlay : 1;
    u8 objectMetadata : 5;
    u8 animatedOverlay : 1;
    u8 drawOverlayOnTop : 1;
    H2_ENUM_BITFIELD(TilesetId, u8) overlayTileset : 6;
    u8 overlayIndex;
#ifdef HOMM2_EDITOR
    // The scenario editor links every placed object part to the placement
    // it belongs to, so erasing one part erases the whole object.
    i32 objectLink;
    i32 overlayLink;
#endif
};
#pragma pack(pop)
#ifdef HOMM2_EDITOR
SIZE(mapCellExtra, 15);
#else
SIZE(mapCellExtra, 7);
#endif

class mapCell {
public:
    u16 m_terrainImageIndex;
    union {
        char m_objectBits;
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
            // As mapCellExtra's: a part lying on the ground, and a shadow part.
            u16 m_objectGroundLayer : 1;
            u16 m_objectShadow : 1;
            u16 m_objectDrawnAsOverlay : 1;
            u16 m_objectMetadata : 13;
        };
        struct {
            u16 m_siteFlags : 3;
            u16 m_siteMetadata : 13;
        };
    };
    u8 m_animatedOverlay : 1;
    u8 m_drawOverlayOnTop : 1;
    H2_ENUM_BITFIELD(TilesetId, u8) m_overlayTileset : 6;
    u8 m_overlayIndex;
    u8 m_flags;
    H2_OPEN_CODE_STORAGE(MapTriggerCode, u8) m_triggerType;
    u16 m_extraIndex;
#ifdef HOMM2_EDITOR
    // The placement each part belongs to (see mapCellExtra).
    i32 m_objectLink;
    i32 m_overlayLink;
#endif

    inline b32 HasFlag(H2_ENUM_PARAM(MapCellFlag, i32) flag) const {
        return (m_flags & IDX(flag)) != 0;
    }
};
#ifdef HOMM2_EDITOR
SIZE(mapCell, 20);
#else
SIZE(mapCell, 12);
#endif
// This is the stored sprite property, not a passability or tileset-shadow query.
#define CELL_HAS_NON_SHADOW_OBJECT(cell)                                                           \
    ((cell)->m_objectIndex != MAPCELL_SPRITE_NONE && (cell)->m_objectTileset != TILESET_DUMMY      \
     && ((cell)->m_flags & IDX(MAP_CELL_OBJECT_SHADOW_ONLY)) == 0)

#ifndef HOMM2_EDITOR
#pragma pack(push, 1)
// A map cell and an extra record as the scenario editor keeps them: the
// game's record and its placement links (see mapCellExtra). fullMap::Read
// with `convert` drops the links.
struct oldMapCell {
    mapCell cell;
    i32 objectLink;
    i32 overlayLink;
};

struct oldMapCellExtra {
    mapCellExtra extra;
    i32 objectLink;
    i32 overlayLink;
};
#pragma pack(pop)
SIZE(oldMapCell, 20);
SIZE(oldMapCellExtra, 15);
#endif
#endif // HOMM2_EDITOR_MAPCELL_H
