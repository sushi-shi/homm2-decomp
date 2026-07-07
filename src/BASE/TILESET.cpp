// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\TILESET.OBJ   from: .\basewin.lib
// functions: 4   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/tileset.h>
VA(0x004dac60, 0x8f)
tileset::tileset(unsigned long int) {}

VA(0x004dad30, 0x21)
tileset::~tileset() {}

// ---- data / globals / vtables ----
DATA(0x004ebab0)  // const tileset::vftable

// ===== vtable tileset (root)  (1 slots) =====
//  [ 0] VA(0x004dacf0, 0x36)  void * tileset::scalar_dtor(unsigned int)   <- introduces virtual
