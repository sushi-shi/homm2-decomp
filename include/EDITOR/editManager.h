#ifndef HOMM2_EDITOR_EDITMANAGER_H
#define HOMM2_EDITOR_EDITMANAGER_H

// The scenario editor's main manager (EDITMGR, EDT2PL.exe 0x00401a30..): it
// owns the map view, the tool panel and the map edits the tool managers ask
// for. InitMainClasses allocates 0xea2 bytes; the constructor, Open and Close
// prove the member offsets.

#include <match.h>
#include <Domains.h>
#include <H2/Macros.h>
#include <BASE/baseManager.h>
#include <EDITOR/EDITOR.h>
#include <stdio.h>
#include <BASE/message.h>
#include <SOURCE/REQUEST.h>
#include <EDITOR/mapcell.h>
#include <EDITOR/OVERLAY.h>

H2_ENUM_CLASS_FORWARD(FileRequesterMode);

H2_ENUM_BEGIN(EditClearMask)
    // ClearArea's layer masks: everything, or what a road may cross.
    EDIT_CLEAR_ALL       = 0xffff,
    EDIT_CLEAR_ROAD_MASK = 0xfc7f
H2_ENUM_END(EditClearMask)

H2_ENUM_BEGIN(EditGroundShape)
    // giGroundShape: a terrain's plain tile, its border runs against water
    // (the edges and corners take the cell's flip flags), the second edge
    // runs and the decorated plain tiles.
    EDIT_SHAPE_PLAIN              = 0,
    EDIT_SHAPE_NORTH_EDGE         = 1,
    EDIT_SHAPE_NORTH_EAST_CORNER  = 2,
    EDIT_SHAPE_EAST_EDGE          = 3,
    EDIT_SHAPE_NORTH_EAST_INNER   = 4,
    // The coast borders (water or beach on that side), the borders where a
    // coast and another terrain meet, and an edge or corner with the coast
    // beyond its corner.
    EDIT_SHAPE_SHORE_NORTH_EDGE   = 5,
    EDIT_SHAPE_SHORE_CORNER       = 6,
    EDIT_SHAPE_SHORE_EAST_EDGE    = 7,
    EDIT_SHAPE_SHORE_INNER        = 8,
    EDIT_SHAPE_CORNER_SHORE_FAR   = 10,
    EDIT_SHAPE_CORNER_SHORE_NEAR  = 11,
    EDIT_SHAPE_NORTH_EDGE_SHORE   = 12,
    EDIT_SHAPE_EAST_EDGE_SHORE    = 13,
    EDIT_SHAPE_SHORE_EDGE_BORDER  = 14,
    EDIT_SHAPE_SHORE_SIDE_BORDER  = 15,
    EDIT_SHAPE_NORTH_EDGE_ALT     = 16,
    EDIT_SHAPE_EAST_EDGE_ALT      = 17,
    EDIT_SHAPE_DECORATED_FIRST    = 18,
    EDIT_SHAPE_DECORATED_SECOND   = 19,
    EDIT_SHAPE_DECORATED_THIRD    = 20,
    EDIT_SHAPE_DECORATED_FOURTH   = 21,
    EDIT_SHAPE_COUNT              = 22,
    // The shape without the varied-tile bit (GROUND_SHAPE_VARIED).
    EDIT_SHAPE_MASK               = 0x7f,
    // ChooseGroundTile's tile lists: plain and varied tiles, at most 20 of
    // each per terrain and shape.
    EDIT_GROUND_PLAIN             = 0,
    EDIT_GROUND_VARIED            = 1,
    EDIT_GROUND_VARIANTS          = 2,
    EDIT_GROUND_TILES_PER_SHAPE   = 20
H2_ENUM_END(EditGroundShape)

#pragma pack(push, 1)
// A town or capturable-site record of the map file.
struct EditMapRecord {
    u8 x;
    u8 y;
    u8 type;
};
#pragma pack(pop)

// The map text import's file: closed on every return.
class textFile {
public:
    FILE* m_file;

    textFile(void);
    ~textFile();
    operator FILE*(void);
};

// BlendShallowWater's count of a water cell's shaded neighbours by the coast
// corner (a flip state, 0-3) they face; `any` tests all four at once.
union EditCornerCounts {
    u8 corner[4];
    u32 any;
};

class heroWindow;
class icon;
class iconWidget;
class tileset;

H2_ENUM_BEGIN(EditManagerConstant)
    // A map cell coordinate when there is none (the selection, a tool's last
    // drag cell).
    EDIT_NO_CELL = -1
H2_ENUM_END(EditManagerConstant)

H2_ENUM_BEGIN(EditTool)
    // The tool panel's tools (m_tool); their buttons are widgets
    // EDIT_CONTROL_TOOL_FIRST on, and EDIT_CONTROL_TOOL_PANEL shows the
    // selected tool's panel.
    EDIT_TOOL_NONE          = -1,
    EDIT_TOOL_TERRAIN       = 0,
    EDIT_TOOL_OBJECT        = 1,
    EDIT_TOOL_DETAIL        = 2,
    EDIT_TOOL_STREAM        = 3,
    EDIT_TOOL_ROAD          = 4,
    EDIT_TOOL_ERASE         = 5,
    EDIT_TOOL_COUNT         = 6,
    // A tool manager runs at the executive's default priority.
    EDIT_TOOL_PRIORITY      = -1
H2_ENUM_END(EditTool)

H2_ENUM_BEGIN(EditViewGeometry)
    // The map view's 448x448 square at (16, 16).
    EDIT_VIEW_LEFT   = 0x10,
    EDIT_VIEW_TOP    = 0x10,
    EDIT_VIEW_PIXELS = 0x1c0,
    EDIT_VIEW_RIGHT  = EDIT_VIEW_LEFT + EDIT_VIEW_PIXELS,
    EDIT_VIEW_BOTTOM = EDIT_VIEW_TOP + EDIT_VIEW_PIXELS,
    // Monsters stand 5 pixels and heroes 14 above their cell (at the normal
    // zoom).
    EDIT_MONSTER_LIFT = 5,
    EDIT_HERO_LIFT    = 0xe
H2_ENUM_END(EditViewGeometry)

H2_ENUM_BEGIN(EditControl)
    // The editor window's widgets (editwind.bin) the managers answer: the
    // map view, the scroll bars and arrows, the radar, the tool buttons
    // (EDIT_CONTROL_TOOL_FIRST + EditTool) and the panel's buttons.
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
H2_ENUM_END(EditControl)

H2_ENUM_BEGIN(EditBrush)
    // The terrain and eraser tools' brushes: a one-, two- or four-cell
    // square, or a dragged rectangle. Keys 1-4 pick them.
    EDIT_BRUSH_SINGLE    = 0,
    EDIT_BRUSH_DOUBLE    = 1,
    EDIT_BRUSH_QUADRUPLE = 2,
    EDIT_BRUSH_AREA      = 3,
    EDIT_BRUSH_COUNT     = 4,
    // The square brushes' widths in cells.
    EDIT_BRUSH_SINGLE_CELLS    = 1,
    EDIT_BRUSH_DOUBLE_CELLS    = 2,
    EDIT_BRUSH_QUADRUPLE_CELLS = 4
H2_ENUM_END(EditBrush)

H2_ENUM_BEGIN(EditToolPanel)
    // The tool panel's screen region, which a tool redraws its buttons into.
    EDIT_TOOL_PANEL_X      = 0x1e0,
    EDIT_TOOL_PANEL_Y      = 0xe8,
    EDIT_TOOL_PANEL_WIDTH  = 0x90,
    EDIT_TOOL_PANEL_HEIGHT = 0xa0,
    // The brush buttons along its bottom (editbtns.icn), left to right; each
    // brush has a normal and a selected frame, as each tool button has.
    EDIT_BUTTON_FRAMES         = 2,
    EDIT_BRUSH_BUTTON_X        = 0x1ee,
    EDIT_BRUSH_BUTTON_STEP     = 0x1e,
    EDIT_BRUSH_BUTTON_Y        = 0x168,
    EDIT_BRUSH_BUTTON_WIDTH    = 0x18,
    EDIT_BRUSH_BUTTON_HEIGHT   = 0x12,
    EDIT_BRUSH_FRAME_FIRST     = 0x18,
    EDIT_BRUSH_BUTTON_ID_FIRST = 0x514,
    EDIT_BRUSH_BUTTON_ID_LAST  = EDIT_BRUSH_BUTTON_ID_FIRST + EDIT_BRUSH_COUNT - 1,
    // The mouse-move repeats a tool's cursor redraw waits for.
    EDIT_CURSOR_REDRAW_INTERVAL = 10
H2_ENUM_END(EditToolPanel)

H2_ENUM_BEGIN(EditManagerSetting)
    // The editor manager's pointer is editor.mse frame 0.
    EDIT_POINTER_DEFAULT = 0
H2_ENUM_END(EditManagerSetting)

H2_ENUM_BEGIN(EditManagerLayout)
    // m_objectIcons: two icons per adventure tileset slot (gTilesetFiles);
    // Open loads the first and clears both.
    EDIT_MANAGER_TILESET_COUNT = 64,
    EDIT_MANAGER_ICON_SETS     = 2,
    // The ground and cloud tilesets: the 32-pixel set and an unused slot.
    EDIT_MANAGER_TILESET_SLOTS = 2,
    // The map-extra records (towns, heroes, events, signs) cells name, as
    // the map file stores them; record 0 is never allocated.
    EDIT_MANAGER_EXTRA_CAPACITY = 512,
    EDIT_MANAGER_FIRST_EXTRA    = 1,
    // The save checks keep at most this many messages.
    EDIT_MANAGER_ERROR_CAPACITY = 100
H2_ENUM_END(EditManagerLayout)

#pragma pack(push, 1)
class editManager : public baseManager {
public:
    // The selected tool (-1: none).
    i32 m_tool;
    icon* m_radarIcons;
    icon* m_buttons;
    tileset* m_groundTiles[EDIT_MANAGER_TILESET_SLOTS];
    tileset* m_cloudTiles[EDIT_MANAGER_TILESET_SLOTS];
    icon* m_objectIcons[EDIT_MANAGER_TILESET_COUNT][EDIT_MANAGER_ICON_SETS];
    // The map view's scroll tracks and knobs (escroll.icn).
    iconWidget* m_horizontalTrack;
    iconWidget* m_verticalTrack;
    iconWidget* m_horizontalKnob;
    iconWidget* m_verticalKnob;
    i32 m_zoomLevel;
    // Set by every edit and the text import; saving and loading clear it. Nothing reads it: quit
    // and load always ask.
    b32 m_mapChanged;
    // The map cell of the object the object tool placed last (-1: none), and a state the tool
    // clears on each placement; nothing reads any of the three.
    i32 m_placedX;
    i32 m_placedY;
    i32 m_placedState;
    // The terrain tool's brush size.
    i32 m_brushSize;
    // The brush size (terrain or eraser) the cursor outline was last drawn for.
    i32 m_cursorSize;
    // The object animation frame the map view draws (0..5), and the step
    // counter the animated overlays divide.
    i32 m_unusedAnimationFrame;
    i32 m_animationCounter;
    // The executive manager of the selected tool.
    baseManager* m_toolManager;
    heroWindow* m_window;
    i32 m_extraCount;
    i16 m_extraSizes[EDIT_MANAGER_EXTRA_CAPACITY];
    void* m_extras[EDIT_MANAGER_EXTRA_CAPACITY];
    // The map cell at the view's top-left corner.
    i32 m_viewX;
    i32 m_viewY;
    // The map cell under the pointer when the cursor outline was last drawn.
    i32 m_cursorX;
    i32 m_cursorY;

    editManager(void);
    virtual i32 Open(i32 priority) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message& message) OVERRIDE;
    // The catalogue type (gOverlayTypes index) of the object on a map
    // cell, and the object tool picking it up from under the pointer.
    i32 OverlayTypeAt(i32 x, i32 y);
    void GrabObject(void);
    // Copies the map into the undo map.
    void SaveUndo(void);
    // Copies the map view to the screen.
    void UpdateMapView(void);
    // Redraws the rulers' cursor marks.
    void UpdateCursor(void);
    void DrawRulers(i32 viewX, i32 viewY, i32 cursorX, i32 cursorY);
    // Turns screen coordinates into the view cell under them (clamped);
    // callers add the view origin.
    void ScreenToCell(i32& x, i32& y);
    // Redraws the map view at the current view origin.
    void DrawMap(void);
    void DrawView(i32 viewX, i32 viewY);
    void DrawRadar(b32 updateScreen);
    // Draws one map cell into a view cell: the layers are EditCellLayer bits.
    void DrawCell(i32 x, i32 y, i32 column, i32 row, i32 layers);
    void ToggleZoom(void);
    void SelectTool(i32 tool);
    void Scroll(i32 dx, i32 dy);
    // The radar and the scroll knobs follow the pointer while it drags.
    void DoRadar(void);
    void DoHorizontalKnob(void);
    void DoVerticalKnob(void);
    // Moves the scroll knobs to the view origin (and redraws them).
    void UpdateKnobs(b32 updateScreen);
    // The map's texts to and from its .TXT (for translating).
    void ExportMapText(void);
    bool ImportMapText(void);
    // The save checks: CheckObjects reports objects that cannot work, the
    // Count helpers count map objects and the Write helpers write the map
    // file's tables (WriteObelisks reports what does not fit).
    void CheckObjects(void);
    // Before a save: compacts the extras and gives every cell the trigger
    // of its catalogue type, its coast and its line flags.
    void UpdateTriggers(void);
    b32 Confirm(H2_CONST char* question);
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
    // The save checks' message list.
    void ClearErrors(void);
    void ShowErrors(void);
    void AddError(H2_CONST char* text);
    void ResetArea(i32 x, i32 y, i32 width, i32 height);
    // Clears the cells and gives them the terrain's tiles: PaintGround in
    // view cells, FillGround in map cells.
    void PaintGround(i32 column, i32 row, i32 width, i32 height, i32 terrain);
    void FillGround(i32 x, i32 y, i32 width, i32 height, i32 terrain);
    // Starts an empty (or random) map of the given size.
    void InitializeMap(b32 random, i32 width, i32 height);
    // Fits the terrain's edge tiles to their neighbours over the whole map
    // (the terrain tool's terrain, the generator's flag and fromUndo go
    // unread).
    void BlendTerrain(i32 terrain, b32 generating, b32 fromUndo, b32 skipBorders, b32 skipFill);
    void ClearArea(i32 x, i32 y, i32 width, i32 height, i32 mask, b32 allLayers, b32 filtered);
    i32 SaveMap(char* name);
    // Rerolls every plain ground tile to one of its variants, more often at
    // a higher variety (0-9).
    void RandomizeGround(i32 variety);
    // The random map generator (RANDOM).
    void GenerateRandomMap(void);
    b32 HasEnoughCastles(void);
    void PaintRandomTerrain(i32 terrain, i32 percent, i32 baseTerrain);
    void RemoveSmallRegions(void);
    i32 CountNearbyObstacles(i32 x, i32 y);
    void PlaceObstacleChains(i32 density, b32 mountains);
    b32 PlaceChainLink(i32* x, i32* y, i32 direction, b32 mountains, char tileset);
    void PlaceCastles(void);
    b32 PlaceResourceSite(i32 x, i32 y, i32 resource);
    void PlaceRandomObjects(i32 density, i32 monsterDensity);
    void PlaceTreasures(i32 density, i32 monsterDensity);
    void ScatterDecorations(void);
    // Map maintenance: frees the map-extra records, and marks the land
    // cells a coast tile borders as coast (where a boat lands).
    void FreeMapExtras(void);
    b32 CanBeCoast(i32 x, i32 y);
    void SetCoast(i32 x, i32 y);
    // Before a save: drops unused map-extra records and renumbers the rest.
    void CoalesceObjectData(void);
    // Gives the map's towns distinct random names.
    void RandomizeTownNames(void);
    // Scrolls the view one cell in a MapDirection, and while the pointer
    // rests at a screen edge.
    void ScreenScroll(H2_ENUM_PARAM(MapDirection, i32) direction, b32 updatePointer);
    void CheckScreenScroll(void);
    // The map cell of the index-th artifact in row order (see FindTown).
    b32 FindArtifact(i32 index, i32* x, i32* y);
    // Switches the ground under the pointer between plain and varied.
    void ToggleGroundVariant(void);
    // The system options dialog (espanel.bin).
    void SystemOptions(void);
    // Gives the water along the coast its shallow tiles.
    void BlendShallowWater(void);
    // The map cell of the index-th town or castle, or of the index-th hero,
    // in row order; false (and -1, -1) when there are fewer.
    b32 FindTown(i32 index, i32* x, i32* y);
    b32 FindHero(i32 index, i32* x, i32* y);
    // Erases every part of the placed object the link names.
    void RemoveLinkedObject(b32 link);
};
#pragma pack(pop)
SIZE(editManager, 0xea2);

// The object catalogue (overlayType) and the object tool's classes: the
// category each class lists and the terrains it lists them on.
extern overlayType gOverlayTypes[OVERLAY_TYPE_COUNT];
extern u8 gObjectClassCategories[OVERLAY_CLASS_COUNT];
extern u32 gObjectClassTerrains[OVERLAY_CLASS_COUNT];
// The header of the edited map (its name, size and players).
extern SMapHeader gEditMapHeader;
// The map text export's file (set while it runs).
#define gTextFileName gTextFileNameBlockBuffer // spelling fixes .bss order
extern char* gTextFileName;
// Set by the random map generator around its last BlendTerrain and RandomizeGround; nothing reads
// it.
#define gVaryTiles gVaryTilesBase // spelling fixes .bss order
extern b32 gVaryTiles;
// Set when ClearArea erased a road or stream part (to redraw the lines).
#define gLinesRemoved gLinesRemovedLocal // spelling fixes .bss order
extern b32 gLinesRemoved;
// ClearArea's object filter: the tilesets whose objects it erases.
#define gClearTilesets gClearTilesetsInstance // spelling fixes .bss order
extern u8 gClearTilesets[TILESET_COUNT];

// Gives the cell a new tile of the terrain and shape unless it already has
// one.
void SetCellGround(i32 x, i32 y, i32 terrain, i32 shape);
// The map text export's line writers: ClearTextFile empties the .TXT,
// AppendTextLine adds a line and WriteTextHeader a blank line and an
// object's heading. The import reads a line without its newline
// (ReadTextLine) and checks a blank line and an object's heading
// (FindTextHeader).
void ClearTextFile(void);
void AppendTextLine(H2_CONST char* text);
void WriteTextHeader(i32 x, i32 y, H2_CONST char* kind);
void ReadTextLine(FILE* file, char* line);
bool FindTextHeader(FILE* file, i32 x, i32 y, H2_CONST char* kind);
// The map file requester: loads or saves by `mode` and
// stores the chosen file name in gMapFileName.
b32 PickMap(FileRequesterMode mode);
// A map code of the serial (a letter from 'V' and three base-26 letters).
char* MakeMapCode(i32 serial);
// Shows a warning on the status bar with a beep.
void ShowStatusWarning(H2_CONST char* text);
// The ground tile of a terrain and shape: the plain or a varied tile (vary),
// whose variant is rolled at (x, y) with the given chance (force: whenever
// the terrain has variants).
i32 ChooseGroundTile(i32 terrain, i32 shape, b32 vary, i32 x, i32 y, b32 force, float chance);
// Whether screen point (x, y) lies on the map view, short of its last 16 pixels on the right and
// bottom (the test compares with the view's width, not its right edge).
b32 InMapArea(i32 x, i32 y);
// Numbers the parts of every catalogue entry (gOverlayTypes) by the frames
// of its tileset.
void FillInOverlayTiles(void);
// Whether a cell's object (its trigger type) has a detail editor: towns,
// signs, bottles, events, sphinxes, monsters, the ultimate artifact, heroes
// and jails.
b32 LocationHasSpecialDetails(i32 triggerType);
// Whether a cell's object (its trigger) keeps a map-extra record, and
// freeing one record (the later ones and their users move down).
b32 HasExtraObjectData(i32 triggerType);
void DeleteExtraObjectData(u32 index);
// Counts the players who may play and gives each one its faction.
void CalculatePlayerNumbers(void);
// Marks the players whose towns or heroes the map holds
// (gEditMapHeader.playerEnabled), then recounts.
void ResetPlayerAvailability(void);
// The file menu (ecpanel.bin): the button chosen, or EDIT_FILE_MENU_NONE.
i32 FileOptions(void);
MessageDispatchResult FileOptionsHandler(struct tag_message& message);
// The system options dialog's toggles and handler.
void UpdateEditorSystemOptions(b32 initialDraw);
MessageDispatchResult EditorSystemOptionsHandler(struct tag_message& message);
// Whether the map needs the expansion (an event, sphinx, castle, hero,
// artifact or object only the expansion has): it then saves as .MX2.
b8 UsesExpansionObjects(void);

#endif // HOMM2_EDITOR_EDITMANAGER_H
