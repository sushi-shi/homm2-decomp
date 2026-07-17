// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\PALETTE.OBJ   from: .\basewin.lib
// functions: 6   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/palette.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>

#define PALETTE_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\PALETTE.CPP"

// @data-layout-note Retail's initialized PALETTE contribution is
// RVA 0x11fd3c..0x11fdc0 (0x84), with no loader-zero contribution. It contains
// three byte-identical source paths in distinct 0x2c slots at addends 0x00, 0x2c,
// and 0x58. PE HIGHLOW sites prove that the default constructor uses the first,
// the resource constructor uses the second, and both destructor forms use the
// third. The only rdata contribution is the already reviewed four-byte palette
// vtable at RVA 0x0eba7c. The former source pooled all path uses into one 0x29
// literal COMDAT, leaving retail references 0x11fd68 and 0x11fd94 uncovered.
// Suffixing the latter two C strings with one and two explicit terminators keeps
// their runtime values identical while making MSVC emit three natural
// four-byte-aligned literal allocations of 0x29, 0x2a, and 0x2b bytes. Their
// ordinary section alignment supplies the remaining zeros in each 0x2c retail
// slot. Do not restore literal pooling or add aliases, padding symbols, fake
// owners, or section pragmas.

VA(0x004d2580, 0x2e)
palette::palette(void) : resource(2, 0, 1, 0) {
    m_data = static_cast<i8*>(H2_ALLOC(0x300, PALETTE_SOURCE_FILE, 8));
}

VA(0x004d2600, 0x55)
palette::palette(u32l id) : resource(2, id, 1, 0) {
    m_data = static_cast<i8*>(H2_ALLOC(0x300, PALETTE_SOURCE_FILE "\0", 13));
    gpResourceManager->PointToFile(id);
    gpResourceManager->ReadBlock(m_data, 0x300);
}

VA(0x004d2660, 0x2c)
palette::~palette() {
    if (m_data != 0)
        H2_FREE(m_data, PALETTE_SOURCE_FILE "\0\0", 21);
    m_data = 0;
}

VA(0x004d2690, 0x4)
i8* palette::Data(void) {
    return m_data;
}

// ===== vtable palette (root)  (1 slots) =====
//  [ 0] VA(0x004d25b0, 0x41)  void * palette::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(palette, 0x004eba7c);
