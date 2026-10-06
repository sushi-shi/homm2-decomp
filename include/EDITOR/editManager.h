#ifndef HOMM2_EDITOR_EDITMANAGER_H
#define HOMM2_EDITOR_EDITMANAGER_H

// The scenario editor's main manager (EDITMGR, EDT2PL.exe 0x00401a30..): it
// owns the map view, the tool panel and the map edits the tool managers ask
// for. InitMainClasses allocates 0xea2 bytes; the constructor, Open and Close
// prove the member offsets.

#include <va.h>
#include <BASE/baseManager.h>
#include <SOURCE/REQUEST.h>
#include <EDITOR/mapcell.h>

class heroWindow;
class icon;
class iconWidget;
class tileset;

H2_ENUM_BEGIN(EditManagerConstant)
    // A map cell coordinate when there is none (the selection, a tool's last
    // drag cell).
    EDIT_NO_CELL = -1
H2_ENUM_END(EditManagerConstant)

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
    // The save checks keep at most this many messages.
    EDIT_MANAGER_ERROR_CAPACITY = 100
H2_ENUM_END(EditManagerLayout)

#pragma pack(push, 1)
// A town or capturable-site record of the map file.
struct EditMapRecord {
    u8 x;
    u8 y;
    u8 type;
};
#pragma pack(pop)

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
    // Set by every edit; saving clears it.
    b32 m_mapChanged;
    // The map cell of the object the object tool placed last (-1: none).
    i32 m_placedX;
    i32 m_placedY;
    i32 m_placedState;
    // The terrain tool's brush size.
    i32 m_brushSize;
    // The cursor outline's size index (the clear tool's brush sizes).
    i32 m_cursorSize;
    // The object animation frame the map view draws (0..5), and the step
    // counter the animated overlays divide.
    i32 m_animationFrame;
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
    // Copies the map into the undo map.
    void SaveUndo(void);
    // Copies the map view to the screen.
    void UpdateMapView(void);
    // Redraws the rulers' cursor marks.
    void UpdateCursor(void);
    void DrawRulers(i32 viewX, i32 viewY, i32 cursorX, i32 cursorY);
    // Turns screen coordinates into the map cell under them (clamped).
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
    void UpdateKnobs(i32 update);
    // The save checks: CheckObjects reports objects that cannot work, the
    // Count helpers count map objects and the Write helpers write the map
    // file's tables (and report what does not fit).
    void CheckObjects(void);
    b32 Confirm(char* question);
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
    void AddError(char* text);
    void ResetArea(i32 x, i32 y, i32 width, i32 height);
    // Clears the cells and gives them the terrain's tiles: PaintGround in
    // view cells, FillGround in map cells.
    void PaintGround(i32 column, i32 row, i32 width, i32 height, i32 terrain);
    void FillGround(i32 x, i32 y, i32 width, i32 height, i32 terrain);
    // Starts an empty (or random) map of the given size.
    void InitializeMap(b32 random, i32 width, i32 height);
    // Fits the terrain's edge tiles to their neighbours over the whole map.
    void BlendTerrain(i32 terrain, b32 unused, b32 fromUndo, b32 skipBorders, b32 skipFill);
    void ClearArea(i32 x, i32 y, i32 width, i32 height, i32 mask, i32 allLayers, i32 filtered);
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
    void PlaceTowns(void);
    i32 PlaceResourceSite(i32 x, i32 y, i32 resource);
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
    void ScreenScroll(i32 direction, b32 updatePointer);
    void CheckScreenScroll(void);
    // The map cell of the index-th artifact in row order (see FindTown).
    b32 FindArtifact(i32 index, i32* x, i32* y);
    // Switches the ground under the pointer between plain and varied.
    void ToggleGroundVariant(void);
    // The map cell of the index-th town or castle, or of the index-th hero,
    // in row order; false (and -1, -1) when there are fewer.
    b32 FindTown(i32 index, i32* x, i32* y);
    b32 FindHero(i32 index, i32* x, i32* y);
    // Erases every part of the placed object the link names.
    void RemoveLinkedObject(i32 link);
};
#pragma pack(pop)
SIZE(editManager, 0xea2);

extern editManager* gEditManager;
// The save checks' messages (editManager::AddError).
extern char* gEditErrors[EDIT_MANAGER_ERROR_CAPACITY];
extern i32 gEditErrorCount;
// Set while BlendTerrain may pick ground variants.
extern b32 gVaryTiles;
// Set when ClearArea erased a road or stream part (to redraw the lines).
extern b32 gLinesRemoved;
// ClearArea's object filter: the tilesets whose objects it erases.
extern u8 gClearTilesets[TILESET_COUNT];

H2_ENUM_BEGIN(EditClearMask)
    // ClearArea's layer masks: everything, or what a road may cross.
    EDIT_CLEAR_ALL       = 0xffff,
    EDIT_CLEAR_ROAD_MASK = 0xfc7f
H2_ENUM_END(EditClearMask)

// The ground tile of a terrain and shape: the plain or a varied tile (vary),
// whose variant is rolled at (x, y) with the given chance.
i32 ChooseGroundTile(i32 terrain, i32 shape, b32 vary, i32 x, i32 y, b32 force, float chance);
// The header of the edited map (its name, size and players).
extern SMapHeader gEditMapHeader;

void SetCellGround(i32 x, i32 y, i32 terrain, i32 shape);

// A map code of the serial (a letter from 'V' and three base-26 letters).
char* MakeMapCode(i32 serial);
// Shows a warning on the status bar with a beep.
void ShowStatusWarning(char* text);
// Whether the map needs the expansion (an event, sphinx, castle, hero,
// artifact or object only the expansion has): it then saves as .MX2.
b8 UsesExpansionObjects(void);
// Whether a cell's object (its trigger) keeps a map-extra record, and
// freeing one record (the later ones and their users move down).
b32 HasExtraObjectData(i32 triggerType);
void DeleteExtraObjectData(u32 index);

// Rebuilds the overlay tiles of the whole map.
void FillInOverlayTiles(void);

// Whether a cell's object (its trigger type) has a detail editor: towns,
// signs, events, sphinxes, monsters, the ultimate artifact, heroes, jails
// and artifacts.
b32 LocationHasSpecialDetails(i32 triggerType);

// Marks the players whose towns or heroes the map holds
// (gEditMapHeader.playerEnabled).
void ResetPlayerAvailability(void);

// The map file requester: loads or saves (`mode`) and stores the chosen
// file name in gMapFileName.
i32 PickMap(i32 mode);
void CalculatePlayerNumbers(void);
// The drag selection the map view outlines (EDIT_NO_CELL when there is none).
extern i32 gSelectionX;
extern i32 gSelectionY;
extern i32 gSelectionWidth;
extern i32 gSelectionHeight;

#endif
