#ifndef HOMM2_EDITOR_CLEARMANAGER_H
#define HOMM2_EDITOR_CLEARMANAGER_H


#include <Ints.h>
#include <BASE/baseManager.h>

class iconWidget;
struct tag_message;

typedef enum ClearBrushSize {

    CLEAR_BRUSH_SINGLE    = 0,
    CLEAR_BRUSH_DOUBLE    = 1,
    CLEAR_BRUSH_QUADRUPLE = 2,
    CLEAR_BRUSH_AREA      = 3,
    CLEAR_BRUSH_COUNT     = 4,
    CLEAR_HELP_COUNT      = 20
} ClearBrushSize;

typedef enum ClearManagerLayout {

    CLEAR_BRUSH_BUTTON_X         = 0x1ee,
    CLEAR_BRUSH_BUTTON_STEP      = 0x1e,
    CLEAR_BRUSH_BUTTON_Y         = 0x168,
    CLEAR_BRUSH_BUTTON_WIDTH     = 0x18,
    CLEAR_BRUSH_BUTTON_HEIGHT    = 0x12,

    CLEAR_BRUSH_FRAME_FIRST      = 0x18,
    CLEAR_BRUSH_BUTTON_ID_FIRST  = 0x514,
    CLEAR_BRUSH_BUTTON_ID_LAST   = 0x517,

    CLEAR_PANEL_REGION_X         = 0x1e0,
    CLEAR_PANEL_REGION_Y         = 0xe8,
    CLEAR_PANEL_REGION_WIDTH     = 0x90,
    CLEAR_PANEL_REGION_HEIGHT    = 0xa0,

    EDIT_CONTROL_MAP             = 9,

    CLEAR_BRUSH_KEY_FIRST        = 2,
    CLEAR_BRUSH_KEY_LAST         = 5,

    CLEAR_CURSOR_REDRAW_INTERVAL = 10
} ClearManagerLayout;

#pragma pack(push, 1)
class clearManager : public baseManager {
public:
    iconWidget* m_brushButtons[CLEAR_BRUSH_COUNT];

    i32 m_lastX;
    i32 m_lastY;

    clearManager(void);
    virtual i32 Open(i32 priority) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message& message) override;
    void UpdateBrushButtons(void);
    void TrackCursor(void);
    void SelectBrush(i32 brush, i32 x, i32 y);
};
#pragma pack(pop)

extern i32 gClearBrush;


extern const char* gClearHelp[CLEAR_HELP_COUNT];
extern i32 gClearCursorMoves;

#endif
