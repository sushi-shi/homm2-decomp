#ifndef HOMM2_EDITOR_TERRAINMANAGER_H
#define HOMM2_EDITOR_TERRAINMANAGER_H


#include <H2/Ints.h>
#include <Domains.h>
#include <BASE/baseManager.h>
#include <SOURCE/kbTypes.h>
#include <EDITOR/editManager.h>

class border;
class iconWidget;
class textWidget;
struct tag_message;

typedef enum TerrainBrushSize {


    TERRAIN_BRUSH_SIZE_SINGLE    = 0,
    TERRAIN_BRUSH_SIZE_AREA      = 1,
    TERRAIN_BRUSH_SIZE_DOUBLE    = 2,
    TERRAIN_BRUSH_SIZE_QUADRUPLE = 4
} TerrainBrushSize;

typedef enum TerrainManagerLayout {


    TERRAIN_BUTTON_SIZE           = 0x1b,
    TERRAIN_BUTTON_ID_FIRST       = 0x1f4,

    TERRAIN_NAME_X                = 0x1ed,
    TERRAIN_NAME_Y                = 0x153,
    TERRAIN_NAME_WIDTH            = 0x75,
    TERRAIN_NAME_HEIGHT           = 10,
    TERRAIN_NAME_ID               = 0x578,
    TERRAIN_NAME_TEXT_SIZE        = 2,


    TERRAIN_HIGHLIGHT_FRAME       = 9,
    TERRAIN_HIGHLIGHT_ID          = 0x19,
    TERRAIN_HIGHLIGHT_INSET       = 2,

    TERRAIN_HELP_NONE             = -1,
    TERRAIN_HELP_FIRST_TERRAIN    = 1,
    TERRAIN_HELP_FIRST_BRUSH      = 10,

    TERRAIN_PAINT_REDRAW_INTERVAL  = 20
} TerrainManagerLayout;


struct TerrainButtonPosition {
    i16 x;
    i16 y;
};

#pragma pack(push, 1)
class terrainManager : public baseManager {
public:
    H2EnumStorage<TerrainType, i32> m_terrain;
    iconWidget* m_highlight;
    textWidget* m_terrainName;
    border* m_terrainButtons[H2EnumIndex(TERRAIN_COUNT)];
    iconWidget* m_brushButtons[EDIT_BRUSH_COUNT];

    i32 m_lastX;
    i32 m_lastY;

    terrainManager(void);
    virtual i32 Open(i32 priority) override;
    virtual void Close(void) override;
    void UpdateButtons(void);
    i32 GetBrushSize(void);
    void OutlineBrush(i32 size, i32 x, i32 y);
    void TrackCursor(void);
    virtual MessageDispatchResult Main(struct tag_message& message) override;
    void Idle(void);
    void SelectTerrain(TerrainType terrain);
};
#pragma pack(pop)


extern i32 gTerrainBrush;
extern TerrainButtonPosition gTerrainButtonPositions[H2EnumIndex(TERRAIN_COUNT)];

extern H2EnumStorage<TerrainType, i32> gTerrainChoice;
extern i32 gTerrainCursorMoves;

#endif
