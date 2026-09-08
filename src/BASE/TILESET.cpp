#include <Ints.h>
#include <BASE/tileset.h>
#include <BASE/resourceManager.h>
#include <BASE/Raster.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>

tileset::tileset(u32l id)
    : resource(RESOURCE_CATEGORY_TILESET, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    m_data = nullptr;
    gpResourceManager->PointToFile(id);
    m_tileCount = gpResourceManager->ReadWord();
    m_tileWidth = gpResourceManager->ReadWord();
    m_tileHeight = gpResourceManager->ReadWord();
    const u32 memberSize = gpResourceManager->GetFileSize(id);
    u32 size = 0;
    if (memberSize < 6 || !images::RasterPayloadSize(
            m_tileCount, m_tileWidth, m_tileHeight, memberSize - 6, size)) {
        ShutDown("Invalid TIL count, dimensions or payload length.");
        return;
    }
    m_data = static_cast<char*>(H2_ALLOC(size));
    m_dataSize = size;
    gpResourceManager->ReadBlock(m_data, size);
}

tileset::~tileset() {
    H2_FREE(m_data);
}
