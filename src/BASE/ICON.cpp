// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\ICON.OBJ   from: .\basewin.lib
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon.h>
VA(0x004c7a20, 0x67)
icon::icon(unsigned long int) {}

VA(0x004c7ad0, 0x21)
icon::~icon() {}

VA(0x004c7b00, 0x44)
void icon::DrawToBuffer(int, int, int, int) {}

VA(0x004c7b50, 0x2bb)
int icon::CombatClipDrawToBuffer(int, int, int, struct SLimitData *, int, int, unsigned char *, signed char *) { return 0; }

VA(0x004c7e10, 0x3d)
void icon::ClipFillToBuffer(int, int, int, int, int, int, int, int, int) {}

VA(0x004c7e50, 0x103)
void icon::FillToBuffer(int, int, int, int, int, struct SLimitData *) {}

VA(0x004c7f60, 0x3e)
void icon::DimToBuffer(int, int, int, int) {}


// ===== vtable icon (root)  (1 slots) =====
//  [ 0] VA(0x004c7a90, 0x36)  void * icon::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(icon, 0x004eb9e8);
