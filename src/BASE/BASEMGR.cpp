// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BASEMGR.OBJ   from: .\basewin.lib
// functions: 1   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/baseManager.h>
#include <string.h>
VA(0x004d2530, 0x47)
baseManager::baseManager(void)
{
    m_next = 0;
    m_prev = 0;
    m_priority = -1;
    m_messageMask = -1;
    m_active = 0;
    strcpy(name, "Unknown");
}


// ===== vtable baseManager : public widget  (3 slots) =====
//  [ 0] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 1] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 2] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(baseManager, 0x004eba70);
