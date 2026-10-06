#ifndef HOMM2_EDITOR_CLEARMANAGER_H
#define HOMM2_EDITOR_CLEARMANAGER_H

// The eraser tool (src/EDITOR/CLEARMGR.cpp): the editor runs it as the tool
// manager while the eraser is selected. The unit name CLEARMGR is descriptive;
// Open stores the class name "clearManager".

#include <va.h>
#include <BASE/baseManager.h>

class iconWidget;
struct tag_message;

H2_ENUM_BEGIN(ClearBrushSize)
    // A one-, two- or four-cell square brush, or a dragged rectangle.
    CLEAR_BRUSH_SINGLE    = 0,
    CLEAR_BRUSH_DOUBLE    = 1,
    CLEAR_BRUSH_QUADRUPLE = 2,
    CLEAR_BRUSH_AREA      = 3,
    CLEAR_BRUSH_COUNT     = 4,
    CLEAR_HELP_COUNT      = 20
H2_ENUM_END(ClearBrushSize)

H2_ENUM_BEGIN(ClearManagerLayout)
    // The brush buttons on the tool panel (editbtns.icn), left to right.
    CLEAR_BRUSH_BUTTON_X         = 0x1ee,
    CLEAR_BRUSH_BUTTON_STEP      = 0x1e,
    CLEAR_BRUSH_BUTTON_Y         = 0x168,
    CLEAR_BRUSH_BUTTON_WIDTH     = 0x18,
    CLEAR_BRUSH_BUTTON_HEIGHT    = 0x12,
    // Each brush has a normal and a selected frame.
    CLEAR_BRUSH_FRAME_FIRST      = 0x18,
    CLEAR_BRUSH_BUTTON_ID_FIRST  = 0x514,
    CLEAR_BRUSH_BUTTON_ID_LAST   = 0x517,
    // The tool panel's screen region the buttons are redrawn into.
    CLEAR_PANEL_REGION_X         = 0x1e0,
    CLEAR_PANEL_REGION_Y         = 0xe8,
    CLEAR_PANEL_REGION_WIDTH     = 0x90,
    CLEAR_PANEL_REGION_HEIGHT    = 0xa0,
    // The map view's widget id.
    EDIT_CONTROL_MAP             = 9,
    // Keys 1-4 (scan codes 2-5) pick a brush.
    CLEAR_BRUSH_KEY_FIRST        = 2,
    CLEAR_BRUSH_KEY_LAST         = 5,
    // The mouse-move repeats a cursor redraw waits for.
    CLEAR_CURSOR_REDRAW_INTERVAL = 10
H2_ENUM_END(ClearManagerLayout)

#pragma pack(push, 1)
class clearManager : public baseManager {
public:
    iconWidget* m_brushButtons[CLEAR_BRUSH_COUNT];
    // The last map cell a drag step visited.
    i32 m_lastX;
    i32 m_lastY;

    clearManager(void);
    virtual i32 Open(i32 priority) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message& message) OVERRIDE;
    void UpdateBrushButtons(void);
    void TrackCursor(void);
    void SelectBrush(i32 brush, i32 x, i32 y);
};
#pragma pack(pop)
SIZE(clearManager, 0x4e);

extern i32 gClearBrush;
// Right-click help for the four brush buttons.
// The eraser panel's help: its brushes, then the object classes it erases.
extern H2_CONST char* gClearHelp[CLEAR_HELP_COUNT];
extern i32 gClearCursorMoves;

#endif
