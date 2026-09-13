#include <va.h>
#include <BASE/executive.h>
#include <BASE/EXEC_TYPES.h>
#include <BASE/baseManager.h>
#include <BASE/Misc.h>
#include <stdio.h>
#include <BASE/resourceManager.h>
#include <BASE/inputManager.h>
#include <BASE/soundManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/mouseManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/X_GLOBAL.h>

H2_ENUM_BEGIN(ExecutiveManagerConstant)
    MANAGER_DEFAULT_PRIORITY = -1,
    MANAGER_SUCCESS          = 0,
    MANAGER_ERROR            = 3,
    DIALOG_MANAGER_CAPACITY  = 20
H2_ENUM_END(ExecutiveManagerConstant)

DATA(0x0051f2f4) static char gExecutiveTextStorage[sizeof(SExecutiveText)] =
    localization::Tr("system.resources.initialization_failed") "\0\0\0"
    localization::Tr("system.input.initialization_failed") "\0\0"
    localization::Tr("system.sound.initialization_failed") "\0\0"
    localization::Tr("system.mouse.initialization_failed") "\0\0"
    localization::Tr("system.window.initialization_failed") "\0\0\0"
    localization::Tr("system.manager.add_failed") "\0"
    localization::Tr("system.manager.add_failed") "\0"
    localization::Tr("system.manager.add_failed") "\0"
    localization::Tr("system.manager.add_failed") "\0"
    "-----Manager List Start-----" "\0\0\0\0"
    "-----" "\0\0\0"
    "Head %d   Tail %d" "\0\0\0"
    "-----" "\0\0\0"
    "Manager %20s  this %d   prev %d  next %d" "\0\0\0\0"
    "--*--Manager List Stop --*--\n\n" "\0\0"
    localization::Tr("system.manager.add_failed") "\0"
    localization::Tr("system.manager.add_failed") "\0"
    "Terminated" "\0";

#define gExecutiveText (*reinterpret_cast<SExecutiveText*>(gExecutiveTextStorage))

VA(0x004c4f20, 0x35)
executive::executive(void) {
    m_managerListHead = NULL;
    m_managerListTail = NULL;
    m_activeManager = NULL;
    m_result = 0;
}

VA(0x004c4f60, 0xb0)
i32 executive::InitSystem(void) {
    if (gpResourceManager->Open(MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.resourceInitError);
    if (gpInputManager->Open(MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.inputInitError);
    if (giCurExe == CONFIG_EXECUTABLE_EDITOR) {
        if (gpSoundManager->Open(MANAGER_DEFAULT_PRIORITY) != 0)
            ShutDown(gExecutiveText.soundInitError);
    }
    if (AddManager(gpMouseManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.mouseInitError);
    if (AddManager(gpWindowManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.windowInitError);
    return 0;
}

#if H2_RETAIL_COMPILER
#define manager cur
#endif
VA(0x004c5010, 0xb8)
void executive::ShutDownSystem(void) {
    EarlyShutDownSystem();
    gpSoundManager->Close();
    baseManager* next;
    baseManager* manager = m_managerListHead;
    while (manager != NULL) {
        next = manager->m_next;
        if (manager != gpWindowManager && manager != gpMouseManager)
            RemoveManager(manager);
        manager = next;
    }
    if (gpWindowManager->m_active == 1)
        RemoveManager(gpWindowManager);
    if (gpMouseManager->m_active == 1)
        RemoveManager(gpMouseManager);
    gpInputManager->Close();
    gpResourceManager->Close();
}
#if H2_RETAIL_COMPILER
#undef manager
#endif

#if H2_RETAIL_COMPILER
#define currentManager p
#define dialogExecutive ex
#define index idx
#define savedManagers saveMgr
#define savedNextManagers saveNext
#define savedPreviousManagers savePrev
#endif
VA(0x004c50d0, 0x170)
i32 executive::DoDialog(class baseManager* manager) {
    baseManager* savedPreviousManagers[DIALOG_MANAGER_CAPACITY];
    i32 index;
    baseManager* currentManager;
    baseManager* savedManagers[DIALOG_MANAGER_CAPACITY];
    baseManager* savedNextManagers[DIALOG_MANAGER_CAPACITY];
    executive dialogExecutive;
    i32 count = 0;

    currentManager = m_managerListHead;
    while (currentManager != NULL) {
        savedManagers[count] = currentManager;
        savedPreviousManagers[count] = currentManager->m_prev;
        savedNextManagers[count] = currentManager->m_next;
        currentManager = currentManager->m_next;
        count++;
    }
    if (AddManager(manager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError1);
    if (dialogExecutive.AddManager(gpMouseManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError2);
    if (dialogExecutive.AddManager(gpWindowManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError3);
    if (dialogExecutive.AddManager(manager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError4);
    dialogExecutive.MainLoop();
    RemoveManager(manager);
    for (index = 0; index < count; index++) {
        savedManagers[index]->m_prev = savedPreviousManagers[index];
        savedManagers[index]->m_next = savedNextManagers[index];
    }
    return dialogExecutive.m_result;
}
#if H2_RETAIL_COMPILER
#undef currentManager
#undef dialogExecutive
#undef index
#undef savedManagers
#undef savedNextManagers
#undef savedPreviousManagers
#endif

#if H2_RETAIL_COMPILER
#define currentManager m
#endif
VA(0x004c5240, 0xac)
void executive::PrintManagerList(void) {
    LogStr(gExecutiveText.managerListStart);
    LogStr(gExecutiveText.managerListDivider1);
    sprintf(gText, gExecutiveText.managerListHeaderFormat, m_managerListHead, m_managerListTail);
    LogStr(gText);
    LogStr(gExecutiveText.managerListDivider2);
    baseManager* currentManager = m_managerListHead;
    while (currentManager != NULL) {
        sprintf(gText, gExecutiveText.managerListEntryFormat, currentManager->m_name, currentManager, currentManager->m_prev, currentManager->m_next);
        LogStr(gText);
        currentManager = currentManager->m_next;
    }
    LogStr(gExecutiveText.managerListStop);
}
#if H2_RETAIL_COMPILER
#undef currentManager
#endif

#if H2_RETAIL_COMPILER
#define currentManager cur
#define manager mgr
#endif
VA(0x004c52f0, 0x13c)
i32 executive::AddManager(class baseManager* manager, i32 priority) {
    if (manager == NULL)
        return MANAGER_ERROR;
    if (priority == MANAGER_DEFAULT_PRIORITY) {
        if (m_managerListTail == NULL)
            priority = 0;
        else
            priority = m_managerListTail->m_priority + 1;
    }
    if (!manager->m_active && manager->Open(priority) != 0)
        return MANAGER_ERROR;
    baseManager* currentManager = m_managerListTail;
    while (currentManager != NULL && currentManager->m_priority > priority)
        currentManager = currentManager->m_prev;
    if (currentManager == NULL) {
        manager->m_next = m_managerListHead;
        manager->m_prev = NULL;
        if (m_managerListHead != NULL)
            m_managerListHead->m_prev = manager;
        m_managerListHead = manager;
        if (m_managerListTail == NULL)
            m_managerListTail = manager;
    } else if (currentManager->m_next == NULL) {
        manager->m_prev = m_managerListTail;
        manager->m_next = NULL;
        m_managerListTail->m_next = manager;
        m_managerListTail = manager;
    } else {
        manager->m_prev = currentManager;
        manager->m_next = currentManager->m_next;
        currentManager->m_next->m_prev = manager;
        currentManager->m_next = manager;
    }
    return MANAGER_SUCCESS;
}
#if H2_RETAIL_COMPILER
#undef currentManager
#undef manager
#endif

#if H2_RETAIL_COMPILER
#define manager mgr
#define previous prev
#endif
VA(0x004c5430, 0xc3)
void executive::RemoveManager(class baseManager* manager) {
    if (manager == NULL)
        return;
    manager->Close();
    baseManager* previous = manager->m_prev;
    if (previous == NULL) {
        if (m_managerListHead == m_managerListTail) {
            m_managerListTail = NULL;
            m_managerListHead = NULL;
        } else {
            m_managerListHead = manager->m_next;
            m_managerListHead->m_prev = NULL;
        }
        manager->m_prev = NULL;
        manager->m_next = NULL;
        return;
    }
    previous->m_next = manager->m_next;
    if (previous->m_next == NULL)
        m_managerListTail = previous;
    else
        previous->m_next->m_prev = previous;
    manager->m_prev = NULL;
    manager->m_next = NULL;
}
#if H2_RETAIL_COMPILER
#undef manager
#undef previous
#endif

#if H2_RETAIL_COMPILER
#define manager mgr
#endif
VA(0x004c5500, 0x7c)
void executive::CallManager(class baseManager* manager) {
    baseManager* saved = m_activeManager;
    RemoveManager(m_activeManager);
    if (AddManager(manager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.callManagerError1);
    MainLoop();
    RemoveManager(manager);
    if (AddManager(saved, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.callManagerError2);
    m_activeManager = saved;
}
#if H2_RETAIL_COMPILER
#undef manager
#endif

VA(0x004c5580, 0x179)
void executive::MainLoop(void) {
    i32 done;
    tag_message message;
    b32 dispatch;

    done = 0;
    dispatch = true;
    if (m_managerListHead == NULL)
        return;
    gpInputManager->Flush();
    while (!done) {
        Process1WindowsMessage();
        message = gpInputManager->GetEvent();
        dispatch = true;
        m_activeManager = m_managerListHead;
        if (m_activeManager == NULL)
            return;
        while (m_activeManager != NULL && dispatch && !done) {
            if (m_activeManager->m_active == true
                && (message.type != MESSAGE_MOUSE_MOVE || m_activeManager != gpWindowManager)) {
                switch (m_activeManager->Main(message)) {
                    case MESSAGE_DISPATCH_CONSUME:
                        dispatch = false;
                        break;
                    case MESSAGE_DISPATCH_FORWARD:
                        if (HAS(message.type, MESSAGE_EXECUTIVE)) {
                            switch (message.payload.executive.command) {
                                case EXECUTIVE_COMMAND_TERMINATE_LOOP:
                                    done++;
                                    break;
                                case EXECUTIVE_COMMAND_RETURN_RESULT:
                                    m_result = message.payload.executive.result;
                                    done++;
                                    break;
                                case EXECUTIVE_COMMAND_REMOVE_MANAGER:
                                    RemoveManager(m_activeManager);
                                    m_activeManager = NULL;
                                    break;
                            }
                        }
                        break;
                }
            }
            if (m_activeManager != NULL)
                m_activeManager = m_activeManager->m_next;
        }
    }
}

VA(0x004c5700, 0x15)
void executive::Terminate(void) {
    ShutDown(gExecutiveText.terminationMessage);
}
