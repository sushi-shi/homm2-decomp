#include <Ints.h>
#include <BASE/tileset.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>

tileset::tileset(u32l id)
    : resource(RESOURCE_CATEGORY_TILESET, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    gpResourceManager->PointToFile(id);
    m_tileCount = gpResourceManager->ReadWord();
    m_tileWidth = gpResourceManager->ReadWord();
    m_tileHeight = gpResourceManager->ReadWord();
    u32 size = m_tileCount * m_tileWidth * m_tileHeight;
    m_data = static_cast<char*>(H2_ALLOC(size));
    gpResourceManager->ReadBlock(m_data, size);
}

inline tileset::~tileset() {
    H2_FREE(m_data);
}
