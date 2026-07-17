#ifndef HOMM2_BASE_EXECUTIVE_H
#define HOMM2_BASE_EXECUTIVE_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 10 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class baseManager;

H2_ENUM_CLASS_BEGIN(ExecutiveCommand)
    EXECUTIVE_COMMAND_TERMINATE_LOOP = 1,
    EXECUTIVE_COMMAND_REMOVE_MANAGER = 2,
    EXECUTIVE_COMMAND_RETURN_RESULT = 4
H2_ENUM_CLASS_END(ExecutiveCommand)

H2_ENUM_BEGIN(ExecutiveManagerConstant)
    EXECUTIVE_MANAGER_DEFAULT_PRIORITY = -1,
    EXECUTIVE_MANAGER_SUCCESS = 0,
    EXECUTIVE_MANAGER_STOP_DISPATCH = 1,
    EXECUTIVE_MANAGER_HANDLE_EXECUTIVE_MESSAGE = 2,
    EXECUTIVE_MANAGER_ERROR = 3,
    EXECUTIVE_DIALOG_MANAGER_CAPACITY = 20
H2_ENUM_END(ExecutiveManagerConstant)

#pragma pack(push, 1) // recovered layout is byte-packed
class executive {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    baseManager* m_managerListHead; // +0x00  manager-list head
    baseManager* m_managerListTail; // +0x04  manager-list tail
    baseManager* m_activeManager;   // +0x08  the current/active manager
    i32 m_result;                   // +0x0c  result returned by an executive dialog/main loop
    // --- constructors ---
    executive(void);
    // --- methods ---
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
#endif // HOMM2_BASE_EXECUTIVE_H
