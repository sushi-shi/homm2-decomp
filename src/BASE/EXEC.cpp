// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\EXEC.OBJ   from: .\basewin.lib
// functions: 10   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/executive.h>
#include <BASE/baseManager.h>
#include <BASE/Misc.h>
#include <stdio.h>
#include <SOURCE/KB.h>
VA(0x004d1610, 0x10)
executive::executive(void)
{
    field_0x0 = 0;
    field_0x4 = 0;
    field_0x8 = 0;
    field_0xc = 0;
}

VA(0x004d1620, 0x9e)
int executive::InitSystem(void) { return 0; }

VA(0x004d16c0, 0x86)
void executive::ShutDownSystem(void) {}

VA(0x004d1750, 0xfb)
int executive::DoDialog(class baseManager *) { return 0; }

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
void executive::MainLoop(void) {}

VA(0x004d1b90, 0xa)
void executive::Terminate(void)
{
    ShutDown("Terminated");
}

