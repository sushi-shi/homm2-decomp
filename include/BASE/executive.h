#ifndef HOMM2_BASE_EXECUTIVE_H
#define HOMM2_BASE_EXECUTIVE_H
#include <va.h>
class baseManager;

H2_ENUM_BEGIN(ExecutiveManagerConstant)
    EXECUTIVE_MANAGER_DEFAULT_PRIORITY = -1,
    EXECUTIVE_MANAGER_SUCCESS = 0,
    EXECUTIVE_MANAGER_STOP_DISPATCH = 1,
    EXECUTIVE_MANAGER_HANDLE_EXECUTIVE_MESSAGE = 2,
    EXECUTIVE_MANAGER_ERROR = 3,
    EXECUTIVE_DIALOG_MANAGER_CAPACITY = 20
H2_ENUM_END(ExecutiveManagerConstant)

#pragma pack(push, 1)
class executive {
public:
    baseManager* m_managerListHead;
    baseManager* m_managerListTail;
    baseManager* m_activeManager;
    i32 m_result;
    executive(void);
    i32 InitSystem(void);
    void ShutDownSystem(void);
    i32 DoDialog(class baseManager*);
    void PrintManagerList(void);
    i32 AddManager(class baseManager*, i32);
    void RemoveManager(class baseManager*);
    void CallManager(class baseManager*);
    void MainLoop(void);
    void Terminate(void);
};
#pragma pack(pop)
SIZE(executive, 0x10);
#endif
