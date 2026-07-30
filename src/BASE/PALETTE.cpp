#include <Ints.h>
#include <BASE/palette.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <BASE/MiscGraphicsConstants.h>
#include <SOURCE/KB.h>


palette::palette(void) : resource(RESOURCE_CATEGORY_PALETTE, 0, RESOURCE_REFERENCE_INITIAL, NULL) {
    m_data = static_cast<i8*>(H2_ALLOC(PALETTE_DATA_SIZE));
}

palette::palette(u32l id)
    : resource(RESOURCE_CATEGORY_PALETTE, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    m_data = static_cast<i8*>(H2_ALLOC(PALETTE_DATA_SIZE));
    gpResourceManager->PointToFile(id);
    gpResourceManager->ReadBlock(m_data, MISC_PALETTE_BYTE_COUNT);
}

palette::~palette() {
    if (m_data != NULL)
        H2_FREE(m_data);
    m_data = NULL;
}

i8* palette::Data(void) {
    return m_data;
}
