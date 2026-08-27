#include <va.h>
#include <BASE/tileset.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>

VA(0x004ce090, 0xf1)
tileset::tileset(u32l id)
    : resource(RESOURCE_CATEGORY_TILESET, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    gpResourceManager->PointToFile(id);
    m_tileCount = gpResourceManager->ReadWord();
    m_tileWidth = gpResourceManager->ReadWord();
    m_tileHeight = gpResourceManager->ReadWord();
    u32 size = m_tileCount * m_tileWidth * m_tileHeight;
    m_data = static_cast<char*>(H2_ALLOC(size));
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_data), size);
}

VA(0x004ce1c0, 0x33)
H2_RETAIL_INLINE tileset::~tileset() {
    H2_FREE(m_data);
}

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(tileset, 0x004ea9d4)
