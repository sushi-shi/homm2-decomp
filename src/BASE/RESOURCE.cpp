// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\RESOURCE.OBJ   from: .\basewin.lib
// functions: 3   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/resource.h>
VA(0x004d9730, 0x19)
resource::resource(void) {}

VA(0x004d9750, 0x2d)
resource::resource(short int, unsigned long int, short int, class resource *) {}

VA(0x004d9780, 0x7)
resource::~resource() {}


// ===== vtable resource (root)  (1 slots) =====
//  [ 0] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(resource, 0x004ebaac);
