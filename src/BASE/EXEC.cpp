// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\EXEC.OBJ   from: .\basewin.lib
// functions: 10   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/executive.h>
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
VA(0x004d1610, 0x10)
executive::executive(void)
{
    m_managerListHead = 0;
    m_managerListTail = 0;
    m_activeManager = 0;
    field_0xc = 0;
}

VA(0x004d1620, 0x9e)
int executive::InitSystem(void)
{
    if (gpResourceManager->Open(-1) != 0)
        ShutDown("Unable to initialize resources!");
    if (gpInputManager->Open(-1) != 0)
        ShutDown("Unable to initialize input devices!");
    if (giCurExe == 1) {
        if (gpSoundManager->Open(-1) != 0)
            ShutDown("Unable to initialize sound!");
    }
    if (AddManager(gpMouseManager, -1) != 0)
        ShutDown("Unable to initialize mouse!");
    if (AddManager(gpWindowManager, -1) != 0)
        ShutDown("Unable to initialize windows!  Perhaps you are low on memory?");
    return 0;
}

VA(0x004d16c0, 0x86)
void executive::ShutDownSystem(void)
{
    EarlyShutDownSystem();
    gpSoundManager->Close();
    baseManager *next = m_managerListHead;
    baseManager *cur;
    while ((cur = next) != 0) {
        next = cur->m_next;
        if (cur != gpWindowManager && cur != gpMouseManager)
            RemoveManager(cur);
    }
    if (gpWindowManager->m_active == 1)
        RemoveManager(gpWindowManager);
    if (gpMouseManager->m_active == 1)
        RemoveManager(gpMouseManager);
    gpInputManager->Close();
    gpResourceManager->Close();
}

// @early-stop
// /O2 register-allocation wall at +0x19..+0x40: only EAX/ECX are exchanged in the
// manager snapshot loop; +0x00..+0x18 and +0x42..+0xfa are byte-identical.
VA(0x004d1750, 0xfb)
int executive::DoDialog(class baseManager *manager)
{
    baseManager *managerList[20];
    baseManager *previousList[20];
    baseManager *nextList[20];
    int dialogStorage[4];
    executive *dialog = reinterpret_cast<executive *>(dialogStorage);
    int managerCount;
    int managerIndex;
    baseManager *listManager;
    managerCount = 0;
    dialog->m_managerListHead = 0;
    dialog->m_managerListTail = 0;
    dialog->m_activeManager = 0;
    listManager = m_managerListHead;
    dialog->field_0xc = 0;
    managerIndex = 0;
    if (listManager != 0) {
        do {
            managerList[managerIndex] = listManager;
            previousList[managerIndex] = listManager->m_prev;
            listManager = listManager->m_next;
            nextList[managerIndex] = listManager;
            managerIndex++;
            managerCount++;
        } while (listManager != 0);
    }
    if (AddManager(manager, -1) != 0)
        ShutDown("Can't add manager!");
    if (dialog->AddManager(gpMouseManager, -1) != 0)
        ShutDown("Can't add manager!");
    if (dialog->AddManager(gpWindowManager, -1) != 0)
        ShutDown("Can't add manager!");
    if (dialog->AddManager(manager, -1) != 0)
        ShutDown("Can't add manager!");
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
    return dialog->field_0xc;
}

VA(0x004d1850, 0x86)
void executive::PrintManagerList(void)
{
    LogStr("----- Manager List Start -----");
    LogStr("-----");
    sprintf(gText, "Head: %d Tail: %d", m_managerListHead, m_managerListTail);
    LogStr(gText);
    LogStr("-----");
    for (baseManager *m = m_managerListHead; m != 0; m = m->m_next) {
        sprintf(gText, "Manager: %20s this: %d prev: %d next: %d", m->m_name, m,
                m->m_prev, m->m_next);
        LogStr(gText);
    }
    LogStr("----- Manager List Stop -----");
}

VA(0x004d18e0, 0xce)
int executive::AddManager(class baseManager *mgr, int param_2)
{
    if (mgr == 0)
        return 3;
    if (param_2 == -1) {
        param_2 = m_managerListTail == 0 ? 0 : m_managerListTail->m_priority + 1;
    }
    if (mgr->m_active == 0 && mgr->Open(param_2) != 0)
        return 3;
    baseManager *tail = m_managerListTail;
    baseManager *cur = m_managerListTail;
    if (cur != 0) {
        do {
            if (cur->m_priority <= param_2)
                break;
            cur = cur->m_prev;
        } while (cur != 0);
    }
    if (cur == 0) {
        mgr->m_next = m_managerListHead;
        mgr->m_prev = 0;
        if (m_managerListHead != 0)
            m_managerListHead->m_prev = mgr;
        m_managerListHead = mgr;
        if (m_managerListTail == 0)
            m_managerListTail = mgr;
    } else if (cur->m_next == 0) {
        mgr->m_next = 0;
        mgr->m_prev = tail;
        m_managerListTail->m_next = mgr;
        m_managerListTail = mgr;
    } else {
        mgr->m_prev = cur;
        mgr->m_next = cur->m_next;
        cur->m_next->m_prev = mgr;
        cur->m_next = mgr;
    }
    return 0;
}

VA(0x004d19b0, 0x76)
void executive::RemoveManager(class baseManager *mgr)
{
    if (mgr != 0) {
        mgr->Close();
        baseManager *prev = mgr->m_prev;
        if (prev == 0) {
            if (m_managerListHead == m_managerListTail) {
                m_managerListTail = 0;
                m_managerListHead = 0;
            } else {
                baseManager *next = mgr->m_next;
                m_managerListHead = next;
                next->m_prev = 0;
            }
            mgr->m_prev = 0;
            mgr->m_next = 0;
            return;
        }
        baseManager *next = mgr->m_next;
        prev->m_next = next;
        if (next == 0)
            m_managerListTail = prev;
        else
            next->m_prev = prev;
        mgr->m_prev = 0;
        mgr->m_next = 0;
    }
}

VA(0x004d1a30, 0x5a)
void executive::CallManager(class baseManager *mgr)
{
    baseManager *saved = m_activeManager;
    RemoveManager(saved);
    if (AddManager(mgr, -1) != 0)
        ShutDown("Can't add manager!");
    MainLoop();
    RemoveManager(mgr);
    if (AddManager(saved, -1) != 0)
        ShutDown("Can't add manager!");
    m_activeManager = saved;
}

// @early-stop
// /O2 comparison-order wall at +0x7c: only CMP bytes 3b c8 vs retail 3b c1 differ;
// the equality operands are commutative and +0x00..+0x7b/+0x7e..+0xf9 are byte-identical.
VA(0x004d1a90, 0xfa)
void executive::MainLoop(void)
{
    tag_message message;
    int keepDispatching;
    int done = 0;
    int result;
    baseManager *manager;
    if (m_managerListHead != 0) {
        gpInputManager->Flush();
        do {
            Process1WindowsMessage();
            message = gpInputManager->GetEvent();
            keepDispatching = 1;
            m_activeManager = m_managerListHead;
            if (m_managerListHead == 0)
                return;
            do {
                if (!keepDispatching)
                    break;
                if (done)
                    return;
                manager = m_activeManager;
                if (manager->m_active == 1 &&
                    (message.type != 4 || gpWindowManager != manager)) {
                    result = manager->Main(message);
                    switch (result) {
                    case 1:
                        keepDispatching = 0;
                        break;
                    case 2:
                        if ((message.type & 0x4000) == 0)
                            break;
                        switch (message.payload.executive.command) {
                        case 1:
                            done++;
                            break;
                        case 2:
                            RemoveManager(m_activeManager);
                            m_activeManager = 0;
                            break;
                        case 4:
                            field_0xc = message.payload.executive.result;
                            done++;
                            break;
                        }
                        break;
                    }
                }
                if (m_activeManager != 0)
                    m_activeManager = m_activeManager->m_next;
            } while (m_activeManager != 0);
        } while (!done);
    }
}

VA(0x004d1b90, 0xa)
void executive::Terminate(void)
{
    ShutDown("Terminated");
}
