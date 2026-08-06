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
    "\xed\xe5 \xec\xee\xe3\xf3 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe8\xf0\xee\xe2\xe0\xf2\xfc \xf0\xe5\xf1\xf3\xf0\xf1\xfb. \xc2\xe5\xf0\xee\xff\xf2\xed\xee, \xee\xf8\xe8\xe1\xea\xe0 \xe4\xe8\xf1\xea\xe0." "\0\0\0"  /* "не могу инициализировать ресурсы. Вероятно, ошибка диска." */
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe8\xf0\xee\xe2\xe0\xf2\xfc \xf3\xf1\xf2\xf0\xee\xe9\xf1\xf2\xe2\xe0 \xe2\xe2\xee\xe4\xe0. \xc2\xe5\xf0\xee\xff\xf2\xed\xee, \xef\xf0\xee\xe1\xeb\xe5\xec\xe0 \xf1 \xea\xeb\xe0\xe2\xe8\xe0\xf2\xf3\xf0\xee\xe9 \xe8\xeb\xe8 \xec\xfb\xf8\xfc\xfe." "\0\0"  /* "Не могу инициализировать устройства ввода. Вероятно, проблема с клавиатурой или мышью." */
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe8\xf0\xee\xe2\xe0\xf2\xfc \xe7\xe2\xf3\xea." "\0\0"  /* "Не могу инициализировать звук." */
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe8\xf0\xee\xe2\xe0\xf2\xfc \xec\xfb\xf8\xfc." "\0\0"  /* "Не могу инициализировать мышь." */
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe8\xf0\xee\xe2\xe0\xf2\xfc \xee\xea\xed\xe0. \xc2\xe5\xf0\xee\xff\xf2\xed\xee, \xee\xf8\xe8\xe1\xea\xe0 \xe4\xe8\xf1\xea\xe0 \xe8\xeb\xe8 \xef\xe0\xec\xff\xf2\xe8." "\0\0\0"  /* "Не могу инициализировать окна. Вероятно, ошибка диска или памяти." */
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed\xe5\xe4\xe6\xe5\xf0\xe0!" "\0"  /* "Не могу добавить менеджера!" */
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed\xe5\xe4\xe6\xe5\xf0\xe0!" "\0"  /* "Не могу добавить менеджера!" */
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed\xe5\xe4\xe6\xe5\xf0\xe0!" "\0"  /* "Не могу добавить менеджера!" */
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed\xe5\xe4\xe6\xe5\xf0\xe0!" "\0"  /* "Не могу добавить менеджера!" */
    "-----Manager List Start-----" "\0\0\0\0"
    "-----" "\0\0\0"
    "Head %d   Tail %d" "\0\0\0"
    "-----" "\0\0\0"
    "Manager %20s  this %d   prev %d  next %d" "\0\0\0\0"
    "--*--Manager List Stop --*--\n\n" "\0\0"
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed\xe5\xe4\xe6\xe5\xf0\xe0!" "\0"  /* "Не могу добавить менеджера!" */
    "\xcd\xe5 \xec\xee\xe3\xf3 \xe4\xee\xe1\xe0\xe2\xe8\xf2\xfc \xec\xe5\xed\xe5\xe4\xe6\xe5\xf0\xe0!" "\0"  /* "Не могу добавить менеджера!" */
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
    baseManager* savePrev[DIALOG_MANAGER_CAPACITY];
    i32 idx;
    baseManager* p;
    baseManager* saveMgr[DIALOG_MANAGER_CAPACITY];
    baseManager* saveNext[DIALOG_MANAGER_CAPACITY];
    executive ex;
    i32 count = 0;

    p = m_managerListHead;
    while (p != NULL) {
        saveMgr[count] = p;
        savePrev[count] = p->m_prev;
        saveNext[count] = p->m_next;
        p = p->m_next;
        count++;
    }
    if (AddManager(manager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError1);
    if (ex.AddManager(gpMouseManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError2);
    if (ex.AddManager(gpWindowManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError3);
    if (ex.AddManager(manager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(gExecutiveText.dialogManagerError4);
    ex.MainLoop();
    RemoveManager(manager);
    for (idx = 0; idx < count; idx++) {
        saveMgr[idx]->m_prev = savePrev[idx];
        saveMgr[idx]->m_next = saveNext[idx];
    }
    return ex.m_result;
}

VA(0x004c5240, 0xac)
void executive::PrintManagerList(void) {
    LogStr(gExecutiveText.managerListStart);
    LogStr(gExecutiveText.managerListDivider1);
    sprintf(gText, gExecutiveText.managerListHeaderFormat, m_managerListHead, m_managerListTail);
    LogStr(gText);
    LogStr(gExecutiveText.managerListDivider2);
    baseManager* m = m_managerListHead;
    while (m != NULL) {
        sprintf(gText, gExecutiveText.managerListEntryFormat, m->m_name, m, m->m_prev, m->m_next);
        LogStr(gText);
        m = m->m_next;
    }
    LogStr(gExecutiveText.managerListStop);
}

VA(0x004c52f0, 0x13c)
i32 executive::AddManager(class baseManager* mgr, i32 priority) {
    if (mgr == NULL)
        return MANAGER_ERROR;
    if (priority == MANAGER_DEFAULT_PRIORITY) {
        if (m_managerListTail == NULL)
            priority = 0;
        else
            priority = m_managerListTail->m_priority + 1;
    }
    if (!mgr->m_active && mgr->Open(priority) != 0)
        return MANAGER_ERROR;
    baseManager* cur = m_managerListTail;
    while (cur != NULL && cur->m_priority > priority)
        cur = cur->m_prev;
    if (cur == NULL) {
        mgr->m_next = m_managerListHead;
        mgr->m_prev = NULL;
        if (m_managerListHead != NULL)
            m_managerListHead->m_prev = mgr;
        m_managerListHead = mgr;
        if (m_managerListTail == NULL)
            m_managerListTail = mgr;
    } else if (cur->m_next == NULL) {
        mgr->m_prev = m_managerListTail;
        mgr->m_next = NULL;
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
    RemoveManager(m_activeManager);
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
    i32 done;
    tag_message message;
    i32 dispatch;

    done = 0;
    dispatch = 1;
    if (m_managerListHead == NULL)
        return;
    gpInputManager->Flush();
    while (!done) {
        Process1WindowsMessage();
        message = gpInputManager->GetEvent();
        dispatch = 1;
        m_activeManager = m_managerListHead;
        if (m_activeManager == NULL)
            return;
        while (m_activeManager != NULL && dispatch && !done) {
            if (m_activeManager->m_active == true
                && (message.type != MESSAGE_MOUSE_MOVE || m_activeManager != gpWindowManager)) {
                switch (m_activeManager->Main(message)) {
                    case MESSAGE_DISPATCH_CONSUME:
                        dispatch = 0;
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
