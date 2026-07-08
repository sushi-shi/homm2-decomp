// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\PALETTE.OBJ   from: .\basewin.lib
// functions: 6   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/palette.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
VA(0x004d2580, 0x2e)
palette::palette(void) : resource(2, 0, 1, 0)
{
    m_data = static_cast<signed char *>(BaseAlloc(0x300, __FILE__, __LINE__));
}

VA(0x004d2600, 0x55)
palette::palette(unsigned long int id) : resource(2, id, 1, 0)
{
    m_data = static_cast<signed char *>(BaseAlloc(0x300, __FILE__, __LINE__));
    gpResourceManager->PointToFile(id);
    gpResourceManager->ReadBlock(m_data, 0x300);
}

VA(0x004d2660, 0x2c)
palette::~palette()
{
    if (m_data != 0)
        BaseFree(m_data, __FILE__, __LINE__);
    m_data = 0;
}

VA(0x004d2690, 0x4)
signed char * palette::Data(void) { return m_data; }


// ===== vtable palette (root)  (1 slots) =====
//  [ 0] VA(0x004d25b0, 0x41)  void * palette::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(palette, 0x004eba7c);
