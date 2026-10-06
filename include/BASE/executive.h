#ifndef HOMM2_BASE_EXECUTIVE_H
#define HOMM2_BASE_EXECUTIVE_H

#include <Ints.h>

class baseManager;

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
    i32 DoDialog(class baseManager* manager);
    void PrintManagerList(void);
    i32 AddManager(class baseManager* manager, i32 priority);
    void RemoveManager(class baseManager* manager);
    void CallManager(class baseManager* manager);
    void MainLoop(void);
    void Terminate(void);
};
#pragma pack(pop)

#endif
