#ifndef HOMM2_EDITOR_LINE_H
#define HOMM2_EDITOR_LINE_H


#include <Ints.h>
#include <Ints.h>

enum class TilesetId : u8;

typedef enum LineType {
    LINE_ROAD   = 0,
    LINE_STREAM = 1
} LineType;

typedef enum LineConstant {

    LINE_ROAD_OVERLAY_FIRST   = 399,
    LINE_STREAM_OVERLAY_FIRST = 431,

    LINE_NO_TILE              = 0xff,

    LINE_NO_VARIANT           = -1
} LineConstant;


extern i32 gLineType;
extern TilesetId gLineTileset;
extern i32 gLineOverlayFirst;

extern u8* gLineMap;


#define LINE_MAP_AT(x, y) ((gLineMap + x)[(y) * MAP_WIDTH])

void SetLineType(i32 type);
void AddLineCell(i32 x, i32 y);


b32 IsLineTile(TilesetId tileset, i32 index, b32 alternate);
void BuildLineMap(i32 fromX, i32 fromY, i32 toX, i32 toY, b32 alternate);
void DrawRoads(i32 fromX, i32 fromY, i32 toX, i32 toY);
void DrawStreams(i32 fromX, i32 fromY, i32 toX, i32 toY);
void DrawLines(i32 fromX, i32 fromY, i32 toX, i32 toY);


void SetLineTile(i32 x, i32 y, TilesetId tileset, i32 index, i32 variant);

void RedrawLines(i32 fromX, i32 fromY, i32 toX, i32 toY);

#endif
