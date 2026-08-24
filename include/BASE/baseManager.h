#ifndef HOMM2_BASE_BASEMANAGER_H
#define HOMM2_BASE_BASEMANAGER_H

#include <Ints.h>
#include <BASE/message.h>

struct tag_message;

enum {
    BASE_MANAGER_MESSAGE_MASK_ALL         = -1,
    BASE_MANAGER_ACCEPT_MOUSE_MOVE        = 4,
    BASE_MANAGER_ACCEPT_LEFT_BUTTON_UP    = 0x10,
    BASE_MANAGER_ACCEPT_RIGHT_BUTTON_DOWN = 0x20,
    BASE_MANAGER_ACCEPT_RIGHT_BUTTON_UP   = 0x40,
    BASE_MANAGER_ACCEPT_RESOURCE          = 0x80,
    BASE_MANAGER_ACCEPT_SWAP              = 0x100,
    BASE_MANAGER_ACCEPT_WIDGET            = 0x200,
    BASE_MANAGER_ACCEPT_ADVENTURE         = 0x400,
    BASE_MANAGER_ACCEPT_TOWN_EVENT        = 0x800,
    BASE_MANAGER_ACCEPT_EXECUTIVE         = 0x4000
};
typedef i32 BaseManagerMessageMask;

typedef enum BaseManagerPriorityConstant {
    BASE_MANAGER_PRIORITY_UNASSIGNED = -1,
    INPUT_MANAGER_PRIORITY           = BASE_MANAGER_PRIORITY_UNASSIGNED,
    SOUND_MANAGER_PRIORITY           = BASE_MANAGER_PRIORITY_UNASSIGNED
} BaseManagerPriorityConstant;

typedef enum BaseManagerConstant {
    BASE_MANAGER_NAME_CAPACITY = 0x1e
} BaseManagerConstant;

#pragma pack(push, 1)
class baseManager {
public:
    baseManager* m_next;
    baseManager* m_prev;
    BaseManagerMessageMask m_messageMask;
    i32 m_priority;
    char m_name[BASE_MANAGER_NAME_CAPACITY];
    b32 m_active;
    baseManager(void);
    void Activate(bool on = true) {
        m_active = on;
    }
    virtual i32 Open(i32) = 0;
    virtual void Close(void) = 0;
    virtual MessageDispatchResult Main(struct tag_message&) = 0;
};
#pragma pack(pop)
#endif
