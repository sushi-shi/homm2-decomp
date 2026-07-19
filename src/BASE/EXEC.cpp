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
    MANAGER_DEFAULT_PRIORITY         = -1,
    MANAGER_SUCCESS                  = 0,
    MANAGER_STOP_DISPATCH            = 1,
    MANAGER_HANDLE_EXECUTIVE_MESSAGE = 2,
    MANAGER_ERROR                    = 3,
    DIALOG_MANAGER_CAPACITY          = 20
H2_ENUM_END(ExecutiveManagerConstant)

DATA(0x0051fb20) static SExecutiveText gExecutiveText = {
    "Unable to initialize resources - possible disk problem.",
    "Unable to initialize input devices - possible problem with mouse or keyboard.",
    "Unable to initialize sound.",
    "Unable to initialize mouse.",
    "Unable to initialize windows - possible memory or disk error.",
    "Can't add manager!",
    "Can't add manager!",
    "Can't add manager!",
    "Can't add manager!",
    "-----Manager List Start-----",
    "-----",
    "Head %d   Tail %d",
    "-----",
    "Manager %20s  this %d   prev %d  next %d",
    "--*--Manager List Stop --*--\n\n",
    "Can't add manager!",
    "Can't add manager!",
    "Terminated"
};

VA(0x004d1610, 0x10)
executive::executive(void) {
    m_managerListHead = NULL;
    m_managerListTail = NULL;
    m_activeManager = NULL;
    m_result = 0;
}

VA(0x004d1620, 0x9e)
i32 executive::InitSystem(void) {
    if (gpResourceManager->Open(MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.resourceInitError);
    if (gpInputManager->Open(MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.inputInitError);
    if (giCurExe == IDX(CONFIG_EXECUTABLE_EDITOR)) {
        if (gpSoundManager->Open(MANAGER_DEFAULT_PRIORITY) != 0)
            ShutDown(gExecutiveText.soundInitError);
    }
    if (AddManager(gpMouseManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.mouseInitError);
    if (AddManager(gpWindowManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.windowInitError);
    return 0;
}

VA(0x004d16c0, 0x86)
void executive::ShutDownSystem(void) {
    EarlyShutDownSystem();
    gpSoundManager->Close();
    baseManager* next = m_managerListHead;
    baseManager* cur;
    while ((cur = next) != NULL) {
        next = cur->m_next;
        if (cur != gpWindowManager && cur != gpMouseManager)
            RemoveManager(cur);
    }
    if (gpWindowManager->m_active)
        RemoveManager(gpWindowManager);
    if (gpMouseManager->m_active)
        RemoveManager(gpMouseManager);
    gpInputManager->Close();
    gpResourceManager->Close();
}

VA(0x004d1750, 0xfb)
i32 executive::DoDialog(class baseManager* manager) {
    baseManager* managerList[DIALOG_MANAGER_CAPACITY];
    baseManager* previousList[DIALOG_MANAGER_CAPACITY];
    baseManager* nextList[DIALOG_MANAGER_CAPACITY];
    i32 dialogStorage[sizeof(executive) / sizeof(i32)];
    executive* dialog = reinterpret_cast<executive*>(dialogStorage);
    baseManager* listManager;
    i32 managerIndex;
    i32 managerCount;
    managerCount = 0;
    dialog->m_managerListHead = NULL;
    dialog->m_managerListTail = NULL;
    dialog->m_activeManager = NULL;
    listManager = m_managerListHead;
    dialog->m_result = 0;
    managerIndex = 0;
    if (listManager != NULL) {
        do {
            managerList[managerIndex] = listManager;
            previousList[managerIndex] = listManager->m_prev;
            listManager = listManager->m_next;
            nextList[managerIndex] = listManager;
            managerIndex++;
            managerCount++;
        } while (listManager != NULL);
    }
    if (AddManager(manager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError1);
    if (dialog->AddManager(gpMouseManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError2);
    if (dialog->AddManager(gpWindowManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError3);
    if (dialog->AddManager(manager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError4);
    dialog->MainLoop();
    RemoveManager(manager);
    if (managerCount > 0) {
        managerIndex = 0;
        do {
            listManager = managerList[managerIndex];
            listManager->m_prev = previousList[managerIndex];
            listManager->m_next = nextList[managerIndex];
            managerIndex++;
            managerCount--;
        } while (managerCount != 0);
    }
    return dialog->m_result;
}

VA(0x004d1850, 0x86)
void executive::PrintManagerList(void) {
    LogStr(gExecutiveText.managerListStart);
    LogStr(gExecutiveText.managerListDivider1);
    sprintf(gText, gExecutiveText.managerListHeaderFormat, m_managerListHead, m_managerListTail);
    LogStr(gText);
    LogStr(gExecutiveText.managerListDivider2);
    for (baseManager* m = m_managerListHead; m != NULL; m = m->m_next) {
        sprintf(gText, gExecutiveText.managerListEntryFormat, m->m_name, m, m->m_prev, m->m_next);
        LogStr(gText);
    }
    LogStr(gExecutiveText.managerListStop);
}

VA(0x004d18e0, 0xce)
i32 executive::AddManager(class baseManager* mgr, i32 priority) {
    if (mgr == NULL)
        return MANAGER_ERROR;
    if (priority == MANAGER_DEFAULT_PRIORITY) {
        priority = m_managerListTail == NULL ? 0 : m_managerListTail->m_priority + 1;
    }
    if (!mgr->m_active && mgr->Open(priority) != 0)
        return MANAGER_ERROR;
    baseManager* tail = m_managerListTail;
    baseManager* cur = m_managerListTail;
    if (cur != NULL) {
        do {
            if (cur->m_priority <= priority)
                break;
            cur = cur->m_prev;
        } while (cur != NULL);
    }
    if (cur == NULL) {
        mgr->m_next = m_managerListHead;
        mgr->m_prev = NULL;
        if (m_managerListHead != NULL)
            m_managerListHead->m_prev = mgr;
        m_managerListHead = mgr;
        if (m_managerListTail == NULL)
            m_managerListTail = mgr;
    } else if (cur->m_next == NULL) {
        mgr->m_next = NULL;
        mgr->m_prev = tail;
        m_managerListTail->m_next = mgr;
        m_managerListTail = mgr;
    } else {
        mgr->m_prev = cur;
        mgr->m_next = cur->m_next;
        cur->m_next->m_prev = mgr;
        cur->m_next = mgr;
    }
    return MANAGER_SUCCESS;
}

VA(0x004d19b0, 0x76)
void executive::RemoveManager(class baseManager* mgr) {
    if (mgr != NULL) {
        mgr->Close();
        baseManager* next;
        baseManager* prev = mgr->m_prev;
        if (prev == NULL) {
            baseManager* tail = m_managerListTail;
            if (m_managerListHead == tail) {
                m_managerListTail = NULL;
                m_managerListHead = NULL;
            } else {
                next = mgr->m_next;
                m_managerListHead = next;
                next->m_prev = NULL;
            }
            mgr->m_prev = NULL;
            mgr->m_next = NULL;
            return;
        }
        next = mgr->m_next;
        prev->m_next = next;
        if (next == NULL)
            m_managerListTail = prev;
        else
            next->m_prev = prev;
        mgr->m_prev = NULL;
        mgr->m_next = NULL;
    }
}

VA(0x004d1a30, 0x5a)
void executive::CallManager(class baseManager* mgr) {
    baseManager* saved = m_activeManager;
    RemoveManager(saved);
    if (AddManager(mgr, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.callManagerError1);
    MainLoop();
    RemoveManager(mgr);
    if (AddManager(saved, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.callManagerError2);
    m_activeManager = saved;
}

VA(0x004d1a90, 0xfa)
void executive::MainLoop(void) {
    tag_message message;
    i32 keepDispatching;
    i32 done = 0;
    i32 result;
    baseManager* manager;
    if (m_managerListHead != NULL) {
        gpInputManager->Flush();
        do {
            Process1WindowsMessage();
            message = gpInputManager->GetEvent();
            keepDispatching = 1;
            m_activeManager = m_managerListHead;
            if (m_managerListHead == NULL)
                return;
            do {
                if (!keepDispatching)
                    break;
                if (done)
                    return;
                manager = m_activeManager;
                if (manager->m_active
                    && (message.type != MESSAGE_MOUSE_MOVE || gpWindowManager != manager)) {
                    result = manager->Main(message);
                    switch (result) {
                        case MANAGER_STOP_DISPATCH:
                            keepDispatching = 0;
                            break;
                        case MANAGER_HANDLE_EXECUTIVE_MESSAGE:
                            if ((message.type & MESSAGE_EXECUTIVE) == 0)
                                break;
                            switch (message.payload.executive.command) {
                                case EXECUTIVE_COMMAND_TERMINATE_LOOP:
                                    done++;
                                    break;
                                case EXECUTIVE_COMMAND_REMOVE_MANAGER:
                                    RemoveManager(m_activeManager);
                                    m_activeManager = NULL;
                                    break;
                                case EXECUTIVE_COMMAND_RETURN_RESULT:
                                    m_result = message.payload.executive.result;
                                    done++;
                                    break;
                            }
                            break;
                    }
                }
                if (m_activeManager != NULL)
                    m_activeManager = m_activeManager->m_next;
            } while (m_activeManager != NULL);
        } while (!done);
    }
}

VA(0x004d1b90, 0xa)
void executive::Terminate(void) {
    ShutDown(gExecutiveText.terminationMessage);
}
