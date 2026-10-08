#ifndef HOMM2_EDITOR_CLEARMANAGER_H
#define HOMM2_EDITOR_CLEARMANAGER_H


#include <H2/Ints.h>
#include <Domains.h>
#include <BASE/baseManager.h>
#include <EDITOR/editManager.h>

class iconWidget;
struct tag_message;

typedef enum ClearHelp {

    CLEAR_HELP_NONE = -1
} ClearHelp;

#pragma pack(push, 1)
class clearManager : public baseManager {
public:
    iconWidget* m_brushButtons[EDIT_BRUSH_COUNT];

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

extern i32 gClearCursorMoves;

#endif
