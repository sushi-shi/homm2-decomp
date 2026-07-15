// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\RESOURCE.OBJ   from: .\basewin.lib
// functions: 3   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/resource.h>

VA(0x004d9730, 0x19)
resource::resource(void)
{
  m_resourceType = 0;
  m_id = 0;
  m_refCount = 0;
  m_next = 0;
}

VA(0x004d9750, 0x2d)
resource::resource(short int a1, unsigned long int a2, short int a3, resource* a4)
{
  m_resourceType = a1;
  m_id = a2;
  m_refCount = a3;
  m_next = a4;
}

VA(0x004d9780, 0x7)
resource::~resource() {}


// ===== vtable resource (root)  (1 slots) =====
//  [ 0] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(resource, 0x004ebaac);
