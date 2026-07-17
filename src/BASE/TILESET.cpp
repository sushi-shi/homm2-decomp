// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\TILESET.OBJ   from: .\basewin.lib
// functions: 4   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/tileset.h>
#include <BASE/TILESET_TYPES.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>

// __FILE__ for the NWC memory/assert tracking (reloc-masked path string).
#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\TILESET.CPP"

DATA(0x00520d9c) static STilesetSourceFiles gTilesetSourceFiles = {RETAIL_FILE, RETAIL_FILE};

VA(0x004dac60, 0x8f)
tileset::tileset(u32l id) : resource(3, id, 1, 0) {
    gpResourceManager->PointToFile(id);
    m_tileWidth = gpResourceManager->ReadWord();
    m_tileHeight = gpResourceManager->ReadWord();
    m_tileCount = gpResourceManager->ReadWord();
    u32 size = m_tileCount * m_tileWidth * m_tileHeight;
    m_data = static_cast<char*>(H2_ALLOC_AT(size, gTilesetSourceFiles.allocation, 18));
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_data), size);
}

VA(0x004dad30, 0x21)
tileset::~tileset() {
    H2_FREE_AT(m_data, gTilesetSourceFiles.destruction, 0x1c);
}

// @data-layout-note Retail initialized storage is exactly two 0x29-byte
// TILESET.CPP filename owners at 0x120d9c and 0x120dc8, separated by three
// alignment bytes. The constructor references owner addend zero and the
// destructor addend 0x2c. One typed 0x55-byte aggregate reproduces those exact
// payloads and addends; final-link alignment supplies the three trailing bytes.
// The only rdata is the existing exact four-byte tileset vtable; this TU has no
// loader-zero data. Do not pool the filenames or replace alignment with a fake
// symbol or section pragma.

// ===== vtable tileset (root)  (1 slots) =====
//  [ 0] VA(0x004dacf0, 0x36)  void * tileset::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(tileset, 0x004ebab0);

#undef RETAIL_FILE
