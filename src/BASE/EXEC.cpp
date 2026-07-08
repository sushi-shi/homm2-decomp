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

VA(0x004d1750, 0xfb)
int executive::DoDialog(class baseManager *param_1)
{
    baseManager *aiStack_f0[20];
    baseManager *auStack_a0[20];
    baseManager *aiStack_50[20];
    executive local_100;
    int iVar5, iVar3;
    baseManager *pmVar;
    iVar5 = 0;
    local_100.m_managerListHead = 0;
    local_100.m_managerListTail = 0;
    local_100.m_activeManager = 0;
    local_100.field_0xc = 0;
    pmVar = m_managerListHead;
    if (pmVar != 0) {
        iVar3 = 0;
        do {
            aiStack_f0[iVar3] = pmVar;
            auStack_a0[iVar3] = pmVar->m_prev;
            pmVar = pmVar->m_next;
            aiStack_50[iVar3] = pmVar;
            iVar3 = iVar3 + 1;
            iVar5 = iVar5 + 1;
        } while (pmVar != 0);
    }
    if (AddManager(param_1, -1) != 0)
        ShutDown("Can't add manager!");
    if (local_100.AddManager(gpMouseManager, -1) != 0)
        ShutDown("Can't add manager!");
    if (local_100.AddManager(gpWindowManager, -1) != 0)
        ShutDown("Can't add manager!");
    if (local_100.AddManager(param_1, -1) != 0)
        ShutDown("Can't add manager!");
    local_100.MainLoop();
    RemoveManager(param_1);
    if (0 < iVar5) {
        iVar3 = 0;
        do {
            pmVar = aiStack_f0[iVar3];
            pmVar->m_prev = auStack_a0[iVar3];
            pmVar->m_next = aiStack_50[iVar3];
            iVar3 = iVar3 + 1;
            iVar5 = iVar5 - 1;
        } while (iVar5 != 0);
    }
    return local_100.field_0xc;
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
        sprintf(gText, "Manager: %20s this: %d prev: %d next: %d", m->name, m,
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
        param_2 = 0;
        if (m_managerListTail != 0)
            param_2 = m_managerListTail->field_0x10 + 1;
    }
    if (mgr->m_active == 0 && mgr->Open(param_2) != 0)
        return 3;
    baseManager *tail = m_managerListTail;
    baseManager *cur = tail;
    if (tail != 0) {
        do {
            if (cur->field_0x10 <= param_2)
                break;
            cur = cur->m_prev;
        } while (cur != 0);
        if (cur != 0) {
            if (cur->m_next != 0) {
                mgr->m_prev = cur;
                mgr->m_next = cur->m_next;
                cur->m_next->m_prev = mgr;
                cur->m_next = mgr;
                return 0;
            }
            mgr->m_next = 0;
            mgr->m_prev = tail;
            m_managerListTail->m_next = mgr;
            m_managerListTail = mgr;
            return 0;
        }
    }
    mgr->m_prev = 0;
    mgr->m_next = m_managerListHead;
    if (m_managerListHead != 0)
        m_managerListHead->m_prev = mgr;
    m_managerListHead = mgr;
    if (m_managerListTail != 0)
        return 0;
    m_managerListTail = mgr;
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

VA(0x004d1a90, 0xfa)
void executive::MainLoop(void)
{
    tag_message local_38;
    int bVar2, bVar3, iVar5;
    baseManager *phVar1;
    bVar3 = 0;
    if (m_managerListHead != 0) {
        gpInputManager->Flush();
        do {
            Process1WindowsMessage();
            local_38 = gpInputManager->GetEvent();
            bVar2 = 1;
            m_activeManager = m_managerListHead;
            if (m_managerListHead == 0)
                return;
            while (bVar2) {
                if (bVar3)
                    return;
                phVar1 = m_activeManager;
                if (phVar1->m_active == 1 && (local_38.type != 4 || gpWindowManager != phVar1)) {
                    iVar5 = phVar1->Main(local_38);
                    if (iVar5 == 1) {
                        bVar2 = 0;
                    } else if (iVar5 == 2 && (local_38.type & 0x4000) != 0) {
                        if (local_38.field4 == 1) {
                            bVar3 = 1;
                        } else if (local_38.field4 == 2) {
                            RemoveManager(m_activeManager);
                            m_activeManager = 0;
                        } else if (local_38.field4 == 4) {
                            field_0xc = reinterpret_cast<int>(local_38.text);
                            bVar3 = 1;
                        }
                    }
                }
                if (m_activeManager == 0 || (m_activeManager = m_activeManager->m_next, m_activeManager == 0))
                    break;
            }
        } while (!bVar3);
    }
}

VA(0x004d1b90, 0xa)
void executive::Terminate(void)
{
    ShutDown("Terminated");
}

