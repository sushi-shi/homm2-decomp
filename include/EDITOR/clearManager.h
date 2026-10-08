#ifndef HOMM2_EDITOR_CLEARMANAGER_H
#define HOMM2_EDITOR_CLEARMANAGER_H

// The eraser tool (src/EDITOR/CLEARMGR.cpp): the editor runs it as the tool
// manager while the eraser is selected. The unit name CLEARMGR is descriptive;
// Open stores the class name "clearManager".

#include <match.h>
#include <Domains.h>
#include <H2/Macros.h>
#include <BASE/baseManager.h>
#include <EDITOR/editManager.h>

class iconWidget;
struct tag_message;

H2_ENUM_BEGIN(ClearHelp)
    // gClearHelp's rows: the brushes (EditBrush) first.
    CLEAR_HELP_NONE = -1
H2_ENUM_END(ClearHelp)

#pragma pack(push, 1)
class clearManager : public baseManager {
public:
    iconWidget* m_brushButtons[EDIT_BRUSH_COUNT];
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

// The selected brush (EditBrush); it survives the tool's reopening.
extern i32 gClearBrush;
// Pointer moves since the brush outline was last redrawn.
extern i32 gClearCursorMoves;

#endif
