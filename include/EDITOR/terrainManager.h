#ifndef HOMM2_EDITOR_TERRAINMANAGER_H
#define HOMM2_EDITOR_TERRAINMANAGER_H

// The terrain painting tool (src/EDITOR/terrain.cpp): the editor runs it as
// the tool manager while the terrain tool is selected. Open stores the class
// name "terrainManager".

#include <match.h>
#include <Domains.h>
#include <H2/Macros.h>
#include <BASE/baseManager.h>
#include <SOURCE/kbTypes.h>
#include <EDITOR/editManager.h>

class border;
class iconWidget;
class textWidget;
struct tag_message;

H2_ENUM_BEGIN(TerrainBrushSize)
    // editManager::m_brushSize: the brush's width in cells (0 for a single
    // cell), or 1 for the dragged rectangle (whose cells the selection holds).
    TERRAIN_BRUSH_SIZE_SINGLE    = 0,
    TERRAIN_BRUSH_SIZE_AREA      = 1,
    TERRAIN_BRUSH_SIZE_DOUBLE    = 2,
    TERRAIN_BRUSH_SIZE_QUADRUPLE = 4
H2_ENUM_END(TerrainBrushSize)

H2_ENUM_BEGIN(TerrainManagerLayout)
    // The terrain buttons: a 3 x 3 grid of transparent borders over the
    // panel's terrain swatches.
    TERRAIN_BUTTON_SIZE           = 0x1b,
    TERRAIN_BUTTON_ID_FIRST       = 0x1f4,
    // The selected terrain's name under the grid.
    TERRAIN_NAME_X                = 0x1ed,
    TERRAIN_NAME_Y                = 0x153,
    TERRAIN_NAME_WIDTH            = 0x75,
    TERRAIN_NAME_HEIGHT           = 10,
    TERRAIN_NAME_ID               = 0x578,
    TERRAIN_NAME_TEXT_SIZE        = 2,
    // The frame (terrains.icn) that outlines the selected terrain button,
    // two pixels outside it.
    TERRAIN_HIGHLIGHT_FRAME       = 9,
    TERRAIN_HIGHLIGHT_ID          = 0x19,
    TERRAIN_HIGHLIGHT_INSET       = 2,
    // Right-click help (gTerrainHelp): the terrains from 1, then the brushes.
    TERRAIN_HELP_NONE             = -1,
    TERRAIN_HELP_FIRST_TERRAIN    = 1,
    TERRAIN_HELP_FIRST_BRUSH      = 10,
    // The mouse-move repeats a redraw waits for while painting.
    TERRAIN_PAINT_REDRAW_INTERVAL  = 20
H2_ENUM_END(TerrainManagerLayout)

// A terrain button's position on the tool panel.
struct TerrainButtonPosition {
    i16 x;
    i16 y;
};

#pragma pack(push, 1)
class terrainManager : public baseManager {
public:
    H2_ENUM_STORAGE(TerrainType, i32) m_terrain;
    iconWidget* m_highlight;
    textWidget* m_terrainName;
    border* m_terrainButtons[IDX(TERRAIN_COUNT)];
    iconWidget* m_brushButtons[EDIT_BRUSH_COUNT];
    // The last map cell a drag step visited.
    i32 m_lastX;
    i32 m_lastY;

    terrainManager(void);
    virtual i32 Open(i32 priority) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    void UpdateButtons(void);
    i32 GetBrushSize(void);
    void SelectBrush(i32 size, i32 x, i32 y);
    void TrackCursor(void);
    virtual MessageDispatchResult Main(struct tag_message& message) OVERRIDE;
    void Idle(void);
    void SelectTerrain(H2_ENUM_PARAM(TerrainType, i32) terrain);
};
#pragma pack(pop)
SIZE(terrainManager, 0x7e);

// The selected brush (EditBrush); it survives the tool's reopening.
extern i32 gTerrainBrush;
extern TerrainButtonPosition gTerrainButtonPositions[IDX(TERRAIN_COUNT)];
// The terrain the tool last selected; Open restores it.
extern H2_ENUM_STORAGE(TerrainType, i32) gTerrainChoice;
extern i32 gTerrainCursorMoves;

#endif
