#ifndef HOMM2_EDITOR_LINEMANAGER_H
#define HOMM2_EDITOR_LINEMANAGER_H

// The road and stream tools (src/EDITOR/line.cpp): the editor runs a
// lineManager as the tool manager while either is selected. Dragging over the
// map marks cells in a line map, and each marked cell takes the road or
// stream tile its marked neighbours call for (line.h). Open stores the
// class name "lineManager".

#include <va.h>
#include <BASE/baseManager.h>
#include <EDITOR/line.h>

class icon;
struct tag_message;

#pragma pack(push, 1)
class lineManager : public baseManager {
public:
    // The outline drawn over the hovered cell (overlay.icn).
    icon* m_cursorIcon;
    // The cell the drag reached last (EDIT_NO_CELL when there is none).
    i32 m_lastX;
    i32 m_lastY;

    lineManager(void);
    virtual i32 Open(i32 priority) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message& message) OVERRIDE;
};
#pragma pack(pop)
SIZE(lineManager, 0x42);

#endif
