#include <va.h>
#include <BASE/tileset.h>
#include <BASE/TILESET_TYPES.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\TILESET.CPP"

static STilesetSourceFiles gTilesetSourceFiles = {RETAIL_FILE, RETAIL_FILE};

VA(0x004ce090, 0xf1)
tileset::tileset(u32l id)
    : resource(RESOURCE_CATEGORY_TILESET, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    gpResourceManager->PointToFile(id);
    m_tileCount = gpResourceManager->ReadWord();
    m_tileWidth = gpResourceManager->ReadWord();
    m_tileHeight = gpResourceManager->ReadWord();
    u32 size = m_tileHeight * m_tileCount * m_tileWidth;
    m_data = static_cast<char*>(H2_ALLOC_AT(size, gTilesetSourceFiles.allocation, 18));
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_data), size);
}

VA(0x004ce1c0, 0x33)
inline tileset::~tileset() {
    H2_FREE_AT(m_data, gTilesetSourceFiles.destruction, 0x1c);
}




#undef RETAIL_FILE
