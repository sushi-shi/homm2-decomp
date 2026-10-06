#ifndef HOMM2_EDITOR_EDITMANAGER_H
#define HOMM2_EDITOR_EDITMANAGER_H

// The scenario editor's main manager (EDITMGR, EDT2PL.exe 0x00401a30..): it
// owns the map view, the tool panel and the map edits the tool managers ask
// for. InitMainClasses allocates 0xea2 bytes; the constructor, Open and Close
// prove the member offsets.

#include <va.h>
#include <BASE/baseManager.h>
#include <SOURCE/REQUEST.h>

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
    EDIT_MANAGER_EXTRA_CAPACITY = 512
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
    void SaveUndo(void);
    i32 LoadMap(char* name);
    void SelectTool(i32 tool);
    void UpdateMapView(void);
    void UpdateCursor(void);
    void ScreenToCell(i32& x, i32& y);
    void DrawMap(void);
    void DrawRadar(b32 updateScreen);
    void ClearArea(i32 x, i32 y, i32 width, i32 height, i32 mask, i32 layer, i32 keepObjects);
};
#pragma pack(pop)
SIZE(editManager, 0xea2);

extern editManager* gEditManager;
// The header of the edited map (its name, size and players).
extern SMapHeader gEditMapHeader;

// The map file requester: loads or saves (`mode`) and stores the chosen
// file name in gMapFileName.
i32 PickMap(i32 mode);
void ResetPlayerAvailability(void);
// The drag selection the map view outlines (EDIT_NO_CELL when there is none).
extern i32 gSelectionX;
extern i32 gSelectionY;
extern i32 gSelectionWidth;
extern i32 gSelectionHeight;

#endif
