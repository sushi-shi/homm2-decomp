// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\EXEC.OBJ   from: .\basewin.lib
// functions: 10   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/executive.h>
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
void executive::PrintManagerList(void) {}

VA(0x004d18e0, 0xce)
int executive::AddManager(class baseManager *, int) { return 0; }

VA(0x004d19b0, 0x76)
void executive::RemoveManager(class baseManager *) {}

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
void executive::Terminate(void) {}

