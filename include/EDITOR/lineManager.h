#ifndef HOMM2_EDITOR_LINEMANAGER_H
#define HOMM2_EDITOR_LINEMANAGER_H

// The road and stream tools (src/EDITOR/line.cpp): the editor runs a
// lineManager as the tool manager while either is selected. Dragging over the
// map marks cells in a line map, and each marked cell takes the road or
// stream tile its marked neighbours call for. Open stores the class name
// "lineManager".

#include <va.h>
#include <BASE/baseManager.h>

class icon;
struct tag_message;

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

#pragma pack(push, 1)
class lineManager : public baseManager {
public:
    // The outline drawn over the hovered cell (overlay.icn).
    icon* m_cursorIcon;
    // The cell the drag reached last (EDIT_NO_CELL when there is none).
    i32 m_lastX;
    i32 m_lastY;

    lineManager(void);
    virtual i32 Open(i32 priority) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message& message) OVERRIDE;
};
#pragma pack(pop)
SIZE(lineManager, 0x42);

// The line the tool draws (LineType), its tileset and its first overlay type.
extern i32 gLineType;
extern i32 gLineTileset;
extern i32 gLineOverlayFirst;
// The cells of the drawn line, a counter per map cell (MAP_WIDTH * MAP_HEIGHT).
extern u8* gLineMap;

void SetLineType(i32 type);
void AddLineCell(i32 x, i32 y);
// Whether an object of the tileset is a tile of the current line; a road's
// tiles count only when they join (gRoadTileJoins, or gRoadTileJoinsAlt for
// the alternate set).
b32 IsLineTile(i32 tileset, i32 index, b32 alternate);
void BuildLineMap(i32 fromX, i32 fromY, i32 toX, i32 toY, b32 alternate);
void DrawRoads(i32 fromX, i32 fromY, i32 toX, i32 toY);
void DrawStreams(i32 fromX, i32 fromY, i32 toX, i32 toY);
void DrawLines(i32 fromX, i32 fromY, i32 toX, i32 toY);
// Gives the cell the line tile `index` (LINE_NO_TILE removes it); half the
// time a cell takes `variant` instead when there is one.
void SetLineTile(i32 x, i32 y, i32 tileset, i32 index, i32 variant);
// Rebuilds the roads and then the streams around an edited area.
void RedrawLines(i32 fromX, i32 fromY, i32 toX, i32 toY);

#endif
