// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\TILESET.OBJ   from: .\basewin.lib
// functions: 4   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/tileset.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
VA(0x004dac60, 0x8f)
tileset::tileset(unsigned long id) : resource(3, id, 1, 0)
{
    gpResourceManager->PointToFile(id);
    m_tileWidth = gpResourceManager->ReadWord();
    m_tileHeight = gpResourceManager->ReadWord();
    m_tileCount = gpResourceManager->ReadWord();
    unsigned int size = m_tileCount * m_tileWidth * m_tileHeight;
    m_data = static_cast<char *>(BaseAlloc(size, __FILE__, __LINE__));
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(m_data), size);
}

VA(0x004dad30, 0x21)
tileset::~tileset()
{
    BaseFree(m_data, __FILE__, __LINE__);
}


// ===== vtable tileset (root)  (1 slots) =====
//  [ 0] VA(0x004dacf0, 0x36)  void * tileset::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(tileset, 0x004ebab0);
