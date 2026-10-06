#ifndef HOMM2_EDITOR_EDITMANAGER_H
#define HOMM2_EDITOR_EDITMANAGER_H


#include <Ints.h>
#include <BASE/baseManager.h>
#include <SOURCE/REQUEST.h>

class heroWindow;
class icon;
class iconWidget;
class tileset;

typedef enum EditManagerConstant {


    EDIT_NO_CELL = -1
} EditManagerConstant;

typedef enum EditManagerLayout {


    EDIT_MANAGER_TILESET_COUNT = 64,
    EDIT_MANAGER_ICON_SETS     = 2,

    EDIT_MANAGER_TILESET_SLOTS = 2,


    EDIT_MANAGER_EXTRA_CAPACITY = 512
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

extern editManager* gEditManager;

extern SMapHeader gEditMapHeader;


i32 PickMap(i32 mode);

extern i32 gSelectionX;
extern i32 gSelectionY;
extern i32 gSelectionWidth;
extern i32 gSelectionHeight;

#endif
