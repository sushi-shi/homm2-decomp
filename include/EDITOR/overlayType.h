#ifndef HOMM2_EDITOR_OVERLAYTYPE_H
#define HOMM2_EDITOR_OVERLAYTYPE_H

// The editor's object catalogue (EDITMGR's gOverlayTypes): one record per
// placeable object, with the tileset its icons come from and the 8 x 8 grid
// of cells it occupies around its anchor. Only the fields the reconstructed
// units read are named so far.

#include <va.h>
#include <Ints.h>

H2_ENUM_BEGIN(OverlayTypeConstant)
    OVERLAY_TYPE_COUNT            = 956,
    // occupiedRows: the grid's top and bottom four rows, a byte per row.
    OVERLAY_OCCUPIED_HALVES       = 2,
    OVERLAY_OCCUPIED_BOTTOM       = 1,
    OVERLAY_TYPE_RESERVED_1D_SIZE = 0x65
H2_ENUM_END(OverlayTypeConstant)

#pragma pack(push, 1)
struct overlayType {
    i32 id;
    i32 iconSet;
    i32 ordinal;
    // The adventure tileset (TilesetId) its icons come from.
    i8 tileset;
    i8 category;
    // How often ScatterDecorations picks it, in tenths of a percent.
    u16 frequency;
    u8 reserved10;
    u32 occupiedRows[OVERLAY_OCCUPIED_HALVES];
    // The grounds (a TerrainType bit each) it may stand on.
    u32 terrainMask;
    u8 reserved1d[OVERLAY_TYPE_RESERVED_1D_SIZE];
};
#pragma pack(pop)
SIZE(overlayType, 0x82);

extern overlayType gOverlayTypes[OVERLAY_TYPE_COUNT];

// OVERLAY: whether the object type fits with its grid's top-left on cell
// (x, y), and placing it there.
i32 CanPlaceOverlay(overlayType* type, i32 x, i32 y, i32 strict);
i32 PlaceOverlay(overlayType* type, i32 x, i32 y, i32 link);

#endif
