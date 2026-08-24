#include <Ints.h>
#include <BASE/tileset.h>
#include <BASE/TILESET_TYPES.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>


static STilesetSourceFiles gTilesetSourceFiles = {"TILESET.cpp", "TILESET.cpp"};

tileset::tileset(u32l id)
    : resource(RESOURCE_CATEGORY_TILESET, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    gpResourceManager->PointToFile(id);
    m_tileCount = gpResourceManager->ReadWord();
    m_tileWidth = gpResourceManager->ReadWord();
    m_tileHeight = gpResourceManager->ReadWord();
    u32 size = m_tileHeight * m_tileCount * m_tileWidth;
    m_data = static_cast<char*>(H2_ALLOC(size));
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_data), size);
}

inline tileset::~tileset() {
    H2_FREE(m_data);
}
