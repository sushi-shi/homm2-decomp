#ifndef HOMM2_EDITOR_EDITMANAGER_H
#define HOMM2_EDITOR_EDITMANAGER_H


#include <Ints.h>
#include <BASE/baseManager.h>
#include <EDITOR/EDITMGR.h>
#include <EDITOR/EDITOR.h>

class heroWindow;
class icon;
class iconWidget;
class tileset;

typedef enum EditManagerConstant {


    EDIT_NO_CELL = -1
} EditManagerConstant;

typedef enum EditTool {


    EDIT_TOOL_NONE          = -1,
    EDIT_TOOL_TERRAIN       = 0,
    EDIT_TOOL_OBJECT        = 1,
    EDIT_TOOL_DETAIL        = 2,
    EDIT_TOOL_STREAM        = 3,
    EDIT_TOOL_ROAD          = 4,
    EDIT_TOOL_ERASE         = 5,
    EDIT_TOOL_COUNT         = 6,

    EDIT_TOOL_PRIORITY      = -1
} EditTool;

typedef enum EditViewGeometry {

    EDIT_VIEW_LEFT   = 0x10,
    EDIT_VIEW_TOP    = 0x10,
    EDIT_VIEW_PIXELS = 0x1c0,
    EDIT_VIEW_RIGHT  = EDIT_VIEW_LEFT + EDIT_VIEW_PIXELS,
    EDIT_VIEW_BOTTOM = EDIT_VIEW_TOP + EDIT_VIEW_PIXELS,


    EDIT_MONSTER_LIFT = 5,
    EDIT_HERO_LIFT    = 0xe
} EditViewGeometry;

typedef enum EditControl {


    EDIT_CONTROL_MAP               = 9,
    EDIT_CONTROL_HORIZONTAL_TRACK  = 0xa,
    EDIT_CONTROL_VERTICAL_TRACK    = 0xb,
    EDIT_CONTROL_HORIZONTAL_KNOB   = 0xc,
    EDIT_CONTROL_VERTICAL_KNOB     = 0xd,
    EDIT_CONTROL_SCROLL_UP         = 0xe,
    EDIT_CONTROL_SCROLL_DOWN       = 0xf,
    EDIT_CONTROL_SCROLL_RIGHT      = 0x10,
    EDIT_CONTROL_SCROLL_LEFT       = 0x11,
    EDIT_CONTROL_SCROLL_UP_LEFT    = 0x12,
    EDIT_CONTROL_SCROLL_UP_RIGHT   = 0x13,
    EDIT_CONTROL_SCROLL_DOWN_LEFT  = 0x14,
    EDIT_CONTROL_SCROLL_DOWN_RIGHT = 0x15,
    EDIT_CONTROL_RADAR             = 0x27,
    EDIT_CONTROL_TOOL_FIRST        = 0x65,
    EDIT_CONTROL_TERRAIN           = EDIT_CONTROL_TOOL_FIRST + EDIT_TOOL_TERRAIN,
    EDIT_CONTROL_OBJECT            = EDIT_CONTROL_TOOL_FIRST + EDIT_TOOL_OBJECT,
    EDIT_CONTROL_DETAIL            = EDIT_CONTROL_TOOL_FIRST + EDIT_TOOL_DETAIL,
    EDIT_CONTROL_STREAM            = EDIT_CONTROL_TOOL_FIRST + EDIT_TOOL_STREAM,
    EDIT_CONTROL_ROAD              = EDIT_CONTROL_TOOL_FIRST + EDIT_TOOL_ROAD,
    EDIT_CONTROL_ERASE             = EDIT_CONTROL_TOOL_FIRST + EDIT_TOOL_ERASE,
    EDIT_CONTROL_ZOOM              = 0x6f,
    EDIT_CONTROL_UNDO              = 0x70,
    EDIT_CONTROL_NEW               = 0x71,
    EDIT_CONTROL_SPECIFICATIONS    = 0x72,
    EDIT_CONTROL_FILE              = 0x73,
    EDIT_CONTROL_SYSTEM            = 0x74,
    EDIT_CONTROL_LOAD              = 0x75,
    EDIT_CONTROL_SAVE              = 0x76,
    EDIT_CONTROL_QUIT              = 0x77,
    EDIT_CONTROL_TOOL_PANEL        = 0x78
} EditControl;

typedef enum EditBrush {


    EDIT_BRUSH_SINGLE    = 0,
    EDIT_BRUSH_DOUBLE    = 1,
    EDIT_BRUSH_QUADRUPLE = 2,
    EDIT_BRUSH_AREA      = 3,
    EDIT_BRUSH_COUNT     = 4,

    EDIT_BRUSH_SINGLE_CELLS    = 1,
    EDIT_BRUSH_DOUBLE_CELLS    = 2,
    EDIT_BRUSH_QUADRUPLE_CELLS = 4
} EditBrush;

typedef enum EditToolPanel {

    EDIT_TOOL_PANEL_X      = 0x1e0,
    EDIT_TOOL_PANEL_Y      = 0xe8,
    EDIT_TOOL_PANEL_WIDTH  = 0x90,
    EDIT_TOOL_PANEL_HEIGHT = 0xa0,


    EDIT_BUTTON_FRAMES         = 2,
    EDIT_BRUSH_BUTTON_X        = 0x1ee,
    EDIT_BRUSH_BUTTON_STEP     = 0x1e,
    EDIT_BRUSH_BUTTON_Y        = 0x168,
    EDIT_BRUSH_BUTTON_WIDTH    = 0x18,
    EDIT_BRUSH_BUTTON_HEIGHT   = 0x12,
    EDIT_BRUSH_FRAME_FIRST     = 0x18,
    EDIT_BRUSH_BUTTON_ID_FIRST = 0x514,
    EDIT_BRUSH_BUTTON_ID_LAST  = EDIT_BRUSH_BUTTON_ID_FIRST + EDIT_BRUSH_COUNT - 1,

    EDIT_CURSOR_REDRAW_INTERVAL = 10
} EditToolPanel;

typedef enum EditManagerSetting {

    EDIT_POINTER_DEFAULT = 0
} EditManagerSetting;

typedef enum EditManagerLayout {


    EDIT_MANAGER_TILESET_COUNT = 64,
    EDIT_MANAGER_ICON_SETS     = 2,

    EDIT_MANAGER_TILESET_SLOTS = 2,


    EDIT_MANAGER_EXTRA_CAPACITY = 512,
    EDIT_MANAGER_FIRST_EXTRA    = 1,

    EDIT_MANAGER_ERROR_CAPACITY = 100
} EditManagerLayout;

#pragma pack(push, 1)
class editManager : public baseManager {
public:

    i32 m_tool;
    icon* m_radarIcons;
    icon* m_buttons;
    tileset* m_groundTiles[EDIT_MANAGER_TILESET_SLOTS];
    tileset* m_cloudTiles[EDIT_MANAGER_TILESET_SLOTS];
    icon* m_objectIcons[EDIT_MANAGER_TILESET_COUNT][EDIT_MANAGER_ICON_SETS];

    iconWidget* m_horizontalTrack;
    iconWidget* m_verticalTrack;
    iconWidget* m_horizontalKnob;
    iconWidget* m_verticalKnob;
    i32 m_zoomLevel;

    b32 m_mapChanged;

    i32 m_placedX;
    i32 m_placedY;
    i32 m_placedState;

    i32 m_brushSize;

    i32 m_cursorSize;


    i32 m_animationFrame;
    i32 m_animationCounter;

    baseManager* m_toolManager;
    heroWindow* m_window;
    i32 m_extraCount;
    i16 m_extraSizes[EDIT_MANAGER_EXTRA_CAPACITY];
    void* m_extras[EDIT_MANAGER_EXTRA_CAPACITY];

    i32 m_viewX;
    i32 m_viewY;

    i32 m_cursorX;
    i32 m_cursorY;

    editManager(void);
    virtual i32 Open(i32 priority) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message& message) override;


    i32 OverlayTypeAt(i32 x, i32 y);
    void GrabObject(void);

    void SaveUndo(void);

    void UpdateMapView(void);

    void UpdateCursor(void);
    void DrawRulers(i32 viewX, i32 viewY, i32 cursorX, i32 cursorY);


    void ScreenToCell(i32& x, i32& y);

    void DrawMap(void);
    void DrawView(i32 viewX, i32 viewY);
    void DrawRadar(b32 updateScreen);

    void DrawCell(i32 x, i32 y, i32 column, i32 row, i32 layers);
    void ToggleZoom(void);
    void SelectTool(i32 tool);
    void Scroll(i32 dx, i32 dy);

    void DoRadar(void);
    void DoHorizontalKnob(void);
    void DoVerticalKnob(void);

    void UpdateKnobs(b32 updateScreen);

    void ExportMapText(void);
    bool ImportMapText(void);


    void CheckObjects(void);


    void UpdateTriggers(void);
    b32 Confirm(const char* question);
    b32 HasObject(i32 trigger);
    i32 CountArtifacts(void);
    i32 CountEvents(void);
    i32 CountTowns(void);
    i32 CountMines(void);
    void WriteTowns(i32 file);
    void WriteMines(i32 file);
    void WriteArtifacts(i32 file);
    void WriteObelisks(i32 file);
    i32 LoadMap(char* name);

    void ClearErrors(void);
    void ShowErrors(void);
    void AddError(const char* text);
    void ResetArea(i32 x, i32 y, i32 width, i32 height);


    void PaintGround(i32 column, i32 row, i32 width, i32 height, i32 terrain);
    void FillGround(i32 x, i32 y, i32 width, i32 height, i32 terrain);

    void InitializeMap(b32 random, i32 width, i32 height);


    void BlendTerrain(i32 terrain, b32 generating, b32 fromUndo, b32 skipBorders, b32 skipFill);
    void ClearArea(i32 x, i32 y, i32 width, i32 height, i32 mask, b32 allLayers, b32 filtered);
    i32 SaveMap(char* name);


    void RandomizeGround(i32 variety);

    void GenerateRandomMap(void);
    b32 HasEnoughCastles(void);
    void PaintRandomTerrain(i32 terrain, i32 percent, i32 baseTerrain);
    void RemoveSmallRegions(void);
    i32 CountNearbyObstacles(i32 x, i32 y);
    void PlaceObstacleChains(i32 density, b32 mountains);
    b32 PlaceChainLink(i32* x, i32* y, i32 direction, b32 mountains, char tileset);
    void PlaceTowns(void);
    b32 PlaceResourceSite(i32 x, i32 y, i32 resource);
    void PlaceRandomObjects(i32 density, i32 monsterDensity);
    void PlaceTreasures(i32 density, i32 monsterDensity);
    void ScatterDecorations(void);


    void FreeMapExtras(void);
    b32 CanBeCoast(i32 x, i32 y);
    void SetCoast(i32 x, i32 y);

    void CoalesceObjectData(void);

    void RandomizeTownNames(void);


    void ScreenScroll(MapDirection direction, b32 updatePointer);
    void CheckScreenScroll(void);

    b32 FindArtifact(i32 index, i32* x, i32* y);

    void ToggleGroundVariant(void);

    void SystemOptions(void);

    void BlendShallowWater(void);


    b32 FindTown(i32 index, i32* x, i32* y);
    b32 FindHero(i32 index, i32* x, i32* y);

    void RemoveLinkedObject(i32 link);
};
#pragma pack(pop)

#endif
