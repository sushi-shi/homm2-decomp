#ifndef HOMM2_EDITOR_EDITMANAGER_H
#define HOMM2_EDITOR_EDITMANAGER_H

// The scenario editor's main manager (EDITMGR, EDT2PL.exe 0x00401a30..): it
// owns the map view, the tool panel and the map edits the tool managers ask
// for. Only the members the reconstructed editor units use are recovered so
// far; the reserved ranges stand for members EDITMGR's reconstruction names.

#include <va.h>
#include <BASE/baseManager.h>
#include <EDITOR/fullMap.h>

class heroWindow;

H2_ENUM_BEGIN(EditManagerConstant)
    // A map cell coordinate when there is none (the selection, a tool's last
    // drag cell).
    EDIT_NO_CELL = -1
H2_ENUM_END(EditManagerConstant)

H2_ENUM_BEGIN(EditManagerLayout)
    EDIT_MANAGER_RESERVED_0036_SIZE = 0x230,
    EDIT_MANAGER_RESERVED_026A_SIZE = 0x10,
    EDIT_MANAGER_RESERVED_027E_SIZE = 0xc,
    EDIT_MANAGER_RESERVED_028E_SIZE = 0xc04
H2_ENUM_END(EditManagerLayout)

#pragma pack(push, 1)
class editManager : public baseManager {
public:
    u8 reserved0036[EDIT_MANAGER_RESERVED_0036_SIZE];
    // Set by every edit; saving clears it.
    b32 m_mapChanged;
    u8 reserved026a[EDIT_MANAGER_RESERVED_026A_SIZE];
    // The cursor outline's size index (the clear tool's brush sizes).
    i32 m_cursorSize;
    u8 reserved027e[EDIT_MANAGER_RESERVED_027E_SIZE];
    heroWindow* m_window;
    u8 reserved028e[EDIT_MANAGER_RESERVED_028E_SIZE];
    // The map cell at the view's top-left corner.
    i32 m_viewX;
    i32 m_viewY;
    // The map cell under the pointer when the cursor outline was last drawn.
    i32 m_cursorX;
    i32 m_cursorY;

    void SaveUndo(void);
    void UpdateMapView(void);
    void UpdateCursor(void);
    void ScreenToCell(i32& x, i32& y);
    void DrawMap(void);
    void DrawRadar(b32 updateScreen);
    void ClearArea(i32 x, i32 y, i32 width, i32 height, i32 mask, i32 layer, i32 keepObjects);
};
#pragma pack(pop)

extern editManager* gEditManager;
// The map the scenario editor edits, and the copy its undo restores.
extern fullMap gMap;
extern fullMap gUndoMap;
// The drag selection the map view outlines (EDIT_NO_CELL when there is none).
extern i32 gSelectionX;
extern i32 gSelectionY;
extern i32 gSelectionWidth;
extern i32 gSelectionHeight;

#endif
