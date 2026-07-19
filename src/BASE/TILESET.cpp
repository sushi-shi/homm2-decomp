#include <va.h>
#include <BASE/tileset.h>
#include <BASE/TILESET_TYPES.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\TILESET.CPP"

DATA(0x00520d9c) static STilesetSourceFiles gTilesetSourceFiles = {RETAIL_FILE, RETAIL_FILE};

VA(0x004dac60, 0x8f)
tileset::tileset(u32l id) : resource(RESOURCE_CATEGORY_TILESET, id, 1, NULL) {
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



VTBL(tileset, 0x004ebab0);

#undef RETAIL_FILE
