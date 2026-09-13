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
        ShutDown(localization::Tr("system.manager.add_failed"));
    if (dialogExecutive.AddManager(gpMouseManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.manager.add_failed"));
    if (dialogExecutive.AddManager(gpWindowManager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.manager.add_failed"));
    if (dialogExecutive.AddManager(manager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.manager.add_failed"));
    dialogExecutive.MainLoop();
    RemoveManager(manager);
    for (index = 0; index < count; index++) {
        savedManagers[index]->m_prev = savedPreviousManagers[index];
        savedManagers[index]->m_next = savedNextManagers[index];
    }
    return dialogExecutive.m_result;
}

void executive::PrintManagerList(void) {
    LogStr(gExecutiveText.managerListStart);
    LogStr(gExecutiveText.managerListDivider1);
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, gExecutiveText.managerListHeaderFormat, m_managerListHead, m_managerListTail);
    LogStr(gText);
    LogStr(gExecutiveText.managerListDivider2);
    baseManager* currentManager = m_managerListHead;
    while (currentManager != NULL) {
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, gExecutiveText.managerListEntryFormat, currentManager->m_name, currentManager, currentManager->m_prev, currentManager->m_next);
        LogStr(gText);
        currentManager = currentManager->m_next;
    }
    LogStr(gExecutiveText.managerListStop);
}

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

void executive::CallManager(class baseManager* manager) {
    baseManager* saved = m_activeManager;
    RemoveManager(m_activeManager);
    if (AddManager(manager, MANAGER_DEFAULT_PRIORITY) != 0)
        ShutDown(localization::Tr("system.manager.add_failed"));
    MainLoop();
    RemoveManager(manager);
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
