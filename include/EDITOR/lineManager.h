#ifndef HOMM2_EDITOR_LINEMANAGER_H
#define HOMM2_EDITOR_LINEMANAGER_H


#include <H2/Ints.h>
#include <BASE/baseManager.h>
#include <EDITOR/line.h>

class icon;
struct tag_message;

#pragma pack(push, 1)
class lineManager : public baseManager {
public:

    icon* m_cursorIcon;

    i32 m_lastX;
    i32 m_lastY;

    lineManager(void);
    virtual i32 Open(i32 priority) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message& message) override;
};
#pragma pack(pop)

#endif
