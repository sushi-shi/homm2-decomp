#ifndef HOMM2_EDITOR_LINE_H
#define HOMM2_EDITOR_LINE_H

// The road and stream drawing of src/EDITOR/line.cpp (the 2.0 editor's
// line.cpp): the line map the tools mark and the tiles each marked cell
// takes. The tool manager itself is lineManager (lineManager.h).

#include <va.h>
#include <Ints.h>

H2_ENUM_CLASS_FORWARD_SPLIT(TilesetId, u8);

H2_ENUM_BEGIN(LineType)
    LINE_ROAD   = 0,
    LINE_STREAM = 1
H2_ENUM_END(LineType)

H2_ENUM_BEGIN(LineConstant)
    // The first overlay type (gOverlayTypes) of each line's tiles.
    LINE_ROAD_OVERLAY_FIRST   = 399,
    LINE_STREAM_OVERLAY_FIRST = 431,
    // No tile: the cell's line object is removed.
    LINE_NO_TILE              = 0xff,
    // No alternative tile to pick at random.
    LINE_NO_VARIANT           = -1
H2_ENUM_END(LineConstant)

// The line the tool draws (LineType), its tileset and its first overlay type.
extern i32 gLineType;
extern TilesetId gLineTileset;
extern i32 gLineOverlayFirst;
// The cells of the drawn line, a counter per map cell (MAP_WIDTH * MAP_HEIGHT).
extern u8* gLineMap;

// The line map's counter of map cell (x, y).
#define LINE_MAP_AT(x, y) ((gLineMap + x)[(y) * MAP_WIDTH])

void SetLineType(i32 type);
void AddLineCell(i32 x, i32 y);
// Whether an object of the tileset is a tile of the current line; a road's
// tiles count only when they join (gRoadTileJoins, or gRoadTileJoinsAlt for
// the alternate set).
b32 IsLineTile(TilesetId tileset, i32 index, b32 alternate);
void BuildLineMap(i32 fromX, i32 fromY, i32 toX, i32 toY, b32 alternate);
void DrawRoads(i32 fromX, i32 fromY, i32 toX, i32 toY);
void DrawStreams(i32 fromX, i32 fromY, i32 toX, i32 toY);
void DrawLines(i32 fromX, i32 fromY, i32 toX, i32 toY);
// Gives the cell the line tile `index` (LINE_NO_TILE removes it); half the
// time a cell takes `variant` instead when there is one.
void SetLineTile(i32 x, i32 y, TilesetId tileset, i32 index, i32 variant);
// Rebuilds the roads and then the streams around an edited area.
void RedrawLines(i32 fromX, i32 fromY, i32 toX, i32 toY);

#endif
