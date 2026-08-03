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

static SExecutiveText gExecutiveText = {
    "\xed\xe5 \xec\xee\xe3\xf3 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe8\xf0\xee\xe2\xe0\xf2\xfc \xf0\xe5\xf1\xf3\xf0\xf1\xfb. \xc2\xe5\xf0\xee\xff\xf2\xed\xee, \xee\xf8\xe8\xe1\xea\xe0 \xe4\xe8\xf1\xea\xe0."  /* "не могу инициализировать ресурсы. Вероятно, ошибка диска." */,
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe8\xf0\xee\xe2\xe0\xf2\xfc \xf3\xf1\xf2\xf0\xee\xe9\xf1\xf2\xe2\xe0 \xe2\xe2\xee\xe4\xe0. \xc2\xe5\xf0\xee\xff\xf2\xed\xee, \xef\xf0\xee\xe1\xeb\xe5\xec\xe0 \xf1 \xea\xeb\xe0\xe2\xe8\xe0\xf2\xf3\xf0\xee\xe9 \xe8\xeb\xe8 \xec\xfb\xf8\xfc\xfe."  /* "Не могу инициализировать устройства ввода. Вероятно, проблема с клавиатурой или мышью." */,
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe8\xf0\xee\xe2\xe0\xf2\xfc \xe7\xe2\xf3\xea."  /* "Не могу инициализировать звук." */,
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe8\xf0\xee\xe2\xe0\xf2\xfc \xec\xfb\xf8\xfc."  /* "Не могу инициализировать мышь." */,
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe8\xf0\xee\xe2\xe0\xf2\xfc \xee\xea\xed\xe0. \xc2\xe5\xf0\xee\xff\xf2\xed\xee, \xee\xf8\xe8\xe1\xea\xe0 \xe4\xe8\xf1\xea\xe0 \xe8\xeb\xe8 \xef\xe0\xec\xff\xf2\xe8."  /* "Не могу инициализировать окна. Вероятно, ошибка диска или памяти." */,
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed\xe5\xe4\xe6\xe5\xf0\xe0!"  /* "Не могу добавить менеджера!" */,
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed\xe5\xe4\xe6\xe5\xf0\xe0!"  /* "Не могу добавить менеджера!" */,
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed\xe5\xe4\xe6\xe5\xf0\xe0!"  /* "Не могу добавить менеджера!" */,
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed\xe5\xe4\xe6\xe5\xf0\xe0!"  /* "Не могу добавить менеджера!" */,
    "-----Manager List Start-----",
    "-----",
    "Head %d   Tail %d",
    "-----",
    "Manager %20s  this %d   prev %d  next %d",
    "--*--Manager List Stop --*--\n\n",
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed\xe5\xe4\xe6\xe5\xf0\xe0!"  /* "Не могу добавить менеджера!" */,
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed\xe5\xe4\xe6\xe5\xf0\xe0!"  /* "Не могу добавить менеджера!" */,
    "Terminated"
};

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

VA(0x004c5010, 0xb8)
void executive::ShutDownSystem(void) {
    EarlyShutDownSystem();
    gpSoundManager->Close();
    baseManager* next;
    baseManager* cur = m_managerListHead;
    while (cur != NULL) {
        next = cur->m_next;
        if (cur != gpWindowManager && cur != gpMouseManager)
            RemoveManager(cur);
        cur = next;
    }
    if (gpWindowManager->m_active == 1)
        RemoveManager(gpWindowManager);
    if (gpMouseManager->m_active == 1)
        RemoveManager(gpMouseManager);
    gpInputManager->Close();
    gpResourceManager->Close();
}

VA(0x004c50d0, 0x170)
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

VA(0x004c5240, 0xac)
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

VA(0x004c52f0, 0x13c)
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

VA(0x004c5430, 0xc3)
void executive::RemoveManager(class baseManager* mgr) {
    if (mgr == NULL)
        return;
    mgr->Close();
    baseManager* prev = mgr->m_prev;
    if (prev == NULL) {
        if (m_managerListHead == m_managerListTail) {
            m_managerListTail = NULL;
            m_managerListHead = NULL;
        } else {
            m_managerListHead = mgr->m_next;
            m_managerListHead->m_prev = NULL;
        }
        mgr->m_prev = NULL;
        mgr->m_next = NULL;
        return;
    }
    prev->m_next = mgr->m_next;
    if (prev->m_next == NULL)
        m_managerListTail = prev;
    else
        prev->m_next->m_prev = prev;
    mgr->m_prev = NULL;
    mgr->m_next = NULL;
}

VA(0x004c5500, 0x7c)
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

VA(0x004c5580, 0x179)
void executive::MainLoop(void) {
    tag_message message;
    i32 keepDispatching;
    i32 done = 0;
    MessageDispatchResult result;
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
                if (manager->m_active == true
                    && (message.type != MESSAGE_MOUSE_MOVE || gpWindowManager != manager)) {
                    result = manager->Main(message);
                    switch (result) {
                        case MESSAGE_DISPATCH_CONSUME:
                            keepDispatching = 0;
                            break;
                        case MESSAGE_DISPATCH_FORWARD:
                            if ((message.type & MESSAGE_EXECUTIVE) == MESSAGE_NONE)
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

VA(0x004c5700, 0x15)
void executive::Terminate(void) {
    ShutDown(gExecutiveText.terminationMessage);
}

