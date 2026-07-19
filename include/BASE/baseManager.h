#ifndef HOMM2_BASE_BASEMANAGER_H
#define HOMM2_BASE_BASEMANAGER_H
#include <va.h>
struct tag_message;

// Message classes a manager accepts: the bits are the tag_message type ids.
H2_ENUM_CLASS_BEGIN_T(BaseManagerMessageMask, i32)
    BASE_MANAGER_MESSAGE_MASK_ALL = -1,
    BASE_MANAGER_ACCEPT_MOUSE_MOVE = 4,
    BASE_MANAGER_ACCEPT_LEFT_BUTTON_UP = 0x10,
    BASE_MANAGER_ACCEPT_RIGHT_BUTTON_DOWN = 0x20,
    BASE_MANAGER_ACCEPT_RIGHT_BUTTON_UP = 0x40,
    BASE_MANAGER_ACCEPT_RESOURCE = 0x80,
    BASE_MANAGER_ACCEPT_SWAP = 0x100,
    BASE_MANAGER_ACCEPT_WIDGET = 0x200,
    BASE_MANAGER_ACCEPT_ADVENTURE = 0x400,
    BASE_MANAGER_ACCEPT_TOWN_EVENT = 0x800,
    BASE_MANAGER_ACCEPT_EXECUTIVE = 0x4000
H2_ENUM_CLASS_END_T(BaseManagerMessageMask, i32)
H2_ENUM_FLAGS(BaseManagerMessageMask)

// Executive ordering keys stay arithmetic; the catalog names each manager.
H2_ENUM_BEGIN(BaseManagerPriority)
    BASE_MANAGER_PRIORITY_UNASSIGNED = -1,
    INPUT_MANAGER_PRIORITY = BASE_MANAGER_PRIORITY_UNASSIGNED,
    SOUND_MANAGER_PRIORITY = BASE_MANAGER_PRIORITY_UNASSIGNED
H2_ENUM_END(BaseManagerPriority)

H2_ENUM_BEGIN(BaseManagerConstant)
    BASE_MANAGER_NAME_CAPACITY = 0x1e
H2_ENUM_END(BaseManagerConstant)

#pragma pack(push, 1)
class baseManager   {
public:
    baseManager* m_next;
    baseManager* m_prev;
    BaseManagerMessageMask m_messageMask;
    BaseManagerPriority m_priority;
    char m_name[BASE_MANAGER_NAME_CAPACITY];
    b32 m_active;
    baseManager(void);
    void Activate(void) {
        m_active = true;
    }
    virtual i32 Open(i32) = 0;
    virtual void Close(void) = 0;
    virtual i32 Main(struct tag_message&) = 0;
};
#pragma pack(pop)
SIZE(baseManager, 0x36);
#endif
