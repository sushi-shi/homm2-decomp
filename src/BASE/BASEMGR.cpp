// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BASEMGR.OBJ   from: .\basewin.lib
// functions: 1   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/baseManager.h>
#include <string.h>

// @match-note: structurally complete /O2 checkpoint (live/retained 79.67%).
// Base and retail are both 0x47 bytes with the same two ordered text relocations
// ("Unknown", vtable), and original PE bytes at 0x004eba70 confirm three
// __purecall slots. Retail stores next/prev zero before the vptr, then priority/
// message-mask -1 and active zero; from the priority store onward the objects are
// byte-identical. Base instead schedules the vptr before its register saves.
// Right-associative body assignment chains preserve retail's field-store order;
// an initializer list cannot, because declaration order would reverse the two
// -1 stores. Revisit after a genuine class-header/TU-state change.
VA(0x004d2530, 0x47)
baseManager::baseManager(void)
{
    m_prev = m_next = 0;
    m_messageMask = m_priority = -1;
    m_active = 0;
    strcpy(m_name, "Unknown");
}


// ===== vtable baseManager : public widget  (3 slots) =====
//  [ 0] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 1] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 2] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(baseManager, 0x004eba70);
