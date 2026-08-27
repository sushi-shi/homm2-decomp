#include <Ints.h>
#include <BASE/Utf8.h>
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
#include <PLATFORM/Runtime.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/Localization.h>

typedef enum ExecutiveManagerConstant {
    MANAGER_DEFAULT_PRIORITY = -1,
    MANAGER_SUCCESS          = 0,
    MANAGER_ERROR            = 3,
    DIALOG_MANAGER_CAPACITY  = 20
} ExecutiveManagerConstant;

static SExecutiveText gExecutiveText = {
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

executive::executive(void) {
    m_managerListHead = NULL;
    m_managerListTail = NULL;
    m_activeManager = NULL;
    m_result = 0;
}

i32 executive::InitSystem(void) {
    if (gpResourceManager->Open(MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.resources.initialization_failed"));
    if (gpInputManager->Open(MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.input.initialization_failed"));
    if (giCurExe == CONFIG_EXECUTABLE_EDITOR) {
        if (gpSoundManager->Open(MANAGER_DEFAULT_PRIORITY) != 0)
            ShutDown(localization::Tr("system.sound.initialization_failed"));
    }
    if (AddManager(gpMouseManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.mouse.initialization_failed"));
    if (AddManager(gpWindowManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.window.initialization_failed"));
    return 0;
}

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
        ShutDown(localization::Tr("system.manager.add_failed"));
    if (ex.AddManager(gpMouseManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.manager.add_failed"));
    if (ex.AddManager(gpWindowManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.manager.add_failed"));
    if (ex.AddManager(manager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.manager.add_failed"));
    ex.MainLoop();
    RemoveManager(manager);
    for (idx = 0; idx < count; idx++) {
        saveMgr[idx]->m_prev = savePrev[idx];
        saveMgr[idx]->m_next = saveNext[idx];
    }
    return ex.m_result;
}

void executive::PrintManagerList(void) {
    LogStr(gExecutiveText.managerListStart);
    LogStr(gExecutiveText.managerListDivider1);
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, gExecutiveText.managerListHeaderFormat, m_managerListHead, m_managerListTail);
    LogStr(gText);
    LogStr(gExecutiveText.managerListDivider2);
    baseManager* m = m_managerListHead;
    while (m != NULL) {
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, gExecutiveText.managerListEntryFormat, m->m_name, m, m->m_prev, m->m_next);
        LogStr(gText);
        m = m->m_next;
    }
    LogStr(gExecutiveText.managerListStop);
}

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

void executive::CallManager(class baseManager* mgr) {
    baseManager* saved = m_activeManager;
    RemoveManager(m_activeManager);
    if (AddManager(mgr, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.manager.add_failed"));
    MainLoop();
    RemoveManager(mgr);
    if (AddManager(saved, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.manager.add_failed"));
    m_activeManager = saved;
}

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
        platform::PumpEvents();
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
                        if ((H2EnumIndex((message.type) & (MESSAGE_EXECUTIVE)))) {
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
                    default:
                        break;
                }
            }
            if (m_activeManager != NULL)
                m_activeManager = m_activeManager->m_next;
        }
    }
}

void executive::Terminate(void) {
    ShutDown(localization::Tr("system.terminated"));
}
