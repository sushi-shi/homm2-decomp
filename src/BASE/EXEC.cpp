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
    field_0x0 = 0;
    field_0x4 = 0;
    field_0x8 = 0;
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
    baseManager *next = field_0x0;
    baseManager *cur;
    while ((cur = next) != 0) {
        next = cur->field_0x4;
        if (cur != gpWindowManager && cur != gpMouseManager)
            RemoveManager(cur);
    }
    if (gpWindowManager->field_0x32 == 1)
        RemoveManager(gpWindowManager);
    if (gpMouseManager->field_0x32 == 1)
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
    local_100.field_0x0 = 0;
    local_100.field_0x4 = 0;
    local_100.field_0x8 = 0;
    local_100.field_0xc = 0;
    pmVar = field_0x0;
    if (pmVar != 0) {
        iVar3 = 0;
        do {
            aiStack_f0[iVar3] = pmVar;
            auStack_a0[iVar3] = pmVar->field_0x8;
            pmVar = pmVar->field_0x4;
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
            pmVar->field_0x8 = auStack_a0[iVar3];
            pmVar->field_0x4 = aiStack_50[iVar3];
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
    sprintf(gText, "Head: %d Tail: %d", field_0x0, field_0x4);
    LogStr(gText);
    LogStr("-----");
    for (baseManager *m = field_0x0; m != 0; m = m->field_0x4) {
        sprintf(gText, "Manager: %20s this: %d prev: %d next: %d", m->name, m,
                m->field_0x8, m->field_0x4);
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
        if (field_0x4 != 0)
            param_2 = field_0x4->field_0x10 + 1;
    }
    if (mgr->field_0x32 == 0 && mgr->Open(param_2) != 0)
        return 3;
    baseManager *tail = field_0x4;
    baseManager *cur = tail;
    if (tail != 0) {
        do {
            if (cur->field_0x10 <= param_2)
                break;
            cur = cur->field_0x8;
        } while (cur != 0);
        if (cur != 0) {
            if (cur->field_0x4 != 0) {
                mgr->field_0x8 = cur;
                mgr->field_0x4 = cur->field_0x4;
                cur->field_0x4->field_0x8 = mgr;
                cur->field_0x4 = mgr;
                return 0;
            }
            mgr->field_0x4 = 0;
            mgr->field_0x8 = tail;
            field_0x4->field_0x4 = mgr;
            field_0x4 = mgr;
            return 0;
        }
    }
    mgr->field_0x8 = 0;
    mgr->field_0x4 = field_0x0;
    if (field_0x0 != 0)
        field_0x0->field_0x8 = mgr;
    field_0x0 = mgr;
    if (field_0x4 != 0)
        return 0;
    field_0x4 = mgr;
    return 0;
}

VA(0x004d19b0, 0x76)
void executive::RemoveManager(class baseManager *mgr)
{
    if (mgr != 0) {
        mgr->Close();
        baseManager *prev = mgr->field_0x8;
        if (prev == 0) {
            if (field_0x0 == field_0x4) {
                field_0x4 = 0;
                field_0x0 = 0;
            } else {
                baseManager *next = mgr->field_0x4;
                field_0x0 = next;
                next->field_0x8 = 0;
            }
            mgr->field_0x8 = 0;
            mgr->field_0x4 = 0;
            return;
        }
        baseManager *next = mgr->field_0x4;
        prev->field_0x4 = next;
        if (next == 0)
            field_0x4 = prev;
        else
            next->field_0x8 = prev;
        mgr->field_0x8 = 0;
        mgr->field_0x4 = 0;
    }
}

VA(0x004d1a30, 0x5a)
void executive::CallManager(class baseManager *mgr)
{
    baseManager *saved = field_0x8;
    RemoveManager(saved);
    if (AddManager(mgr, -1) != 0)
        ShutDown("Can't add manager!");
    MainLoop();
    RemoveManager(mgr);
    if (AddManager(saved, -1) != 0)
        ShutDown("Can't add manager!");
    field_0x8 = saved;
}

VA(0x004d1a90, 0xfa)
void executive::MainLoop(void)
{
    tag_message local_38;
    int bVar2, bVar3, iVar5;
    baseManager *phVar1;
    bVar3 = 0;
    if (field_0x0 != 0) {
        gpInputManager->Flush();
        do {
            Process1WindowsMessage();
            local_38 = gpInputManager->GetEvent();
            bVar2 = 1;
            field_0x8 = field_0x0;
            if (field_0x0 == 0)
                return;
            while (bVar2) {
                if (bVar3)
                    return;
                phVar1 = field_0x8;
                if (phVar1->field_0x32 == 1 && (local_38.type != 4 || gpWindowManager != phVar1)) {
                    iVar5 = phVar1->Main(local_38);
                    if (iVar5 == 1) {
                        bVar2 = 0;
                    } else if (iVar5 == 2 && (local_38.type & 0x4000) != 0) {
                        if (local_38.field4 == 1) {
                            bVar3 = 1;
                        } else if (local_38.field4 == 2) {
                            RemoveManager(field_0x8);
                            field_0x8 = 0;
                        } else if (local_38.field4 == 4) {
                            field_0xc = reinterpret_cast<int>(local_38.text);
                            bVar3 = 1;
                        }
                    }
                }
                if (field_0x8 == 0 || (field_0x8 = field_0x8->field_0x4, field_0x8 == 0))
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

