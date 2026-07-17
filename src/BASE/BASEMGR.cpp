// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BASEMGR.OBJ   from: .\basewin.lib
// functions: 1   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/baseManager.h>
#include <string.h>

// @semantic: structurally complete /O2 checkpoint (live/retained 79.67%).
// Base and retail are both 0x47 bytes and have the same two relocation targets
// and occurrences ("Unknown" and the baseManager vtable). Original PE bytes at
// 0x004eba70 confirm the three __purecall slots and CodeView confirms baseManager
// is a root class, so the early next/prev stores are not a hidden base ctor.
// The first divergence is at +0x00: base emits the vptr store before saving
// registers, while retail saves ebx, copies this to edx, saves esi/edi, then
// initializes next/prev before storing the vptr. From the priority store onward
// the instruction bytes are identical. Right-associative assignment chains,
// separate semantic-enum assignments, and a local this alias do not alter the
// scheduling. An initializer list was rejected because declaration order would
// reverse retail's priority-before-message-mask stores. No AST permutation was
// run below structural alignment. Revisit after an exact-preserving shared-header
// or predecessor/TU-state change; this is not a certified compiler wall.
VA(0x004d2530, 0x47)
baseManager::baseManager(void) {
    m_prev = m_next = 0;
    m_priority = BASE_MANAGER_PRIORITY_UNASSIGNED;
    m_messageMask = BASE_MANAGER_MESSAGE_MASK_ALL;
    m_active = false;
    strcpy(m_name, "Unknown");
}

// ===== vtable baseManager : public widget  (3 slots) =====
//  [ 0] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 1] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 2] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(baseManager, 0x004eba70);
